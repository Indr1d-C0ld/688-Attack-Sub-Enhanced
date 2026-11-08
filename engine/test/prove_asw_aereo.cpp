#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "nucleo/asw_aereo.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/elettronica.hpp"
#include "nucleo/evasione.hpp"

using namespace m688;

// ASW aereo dell'Enhanced (docs/13_asw_aereo.md, A1; nucleo/asw_aereo.hpp, dossier docs/ricerca/17_asw_aereo.md)

namespace {

const DatiAsw& dati() {
    static const DatiAsw d = carica_asw_aereo(M688_DATI "/asw_aereo.toml");
    return d;
}

// Un aereo che va dritto sulla meta del ComandoVolo
struct AereoDiProva {
    Vec2 pos;
    double quota = 0.0, rotta = 0.0;
    void vola(const ComandoVolo& v, double dt) {
        const double d = distanza(pos, v.meta), passo = v.velocita_kn * NODO * dt;
        if (d > 1e-6) rotta = rilevamento(pos, v.meta);
        if (d <= passo) pos = v.meta;
        else pos = {pos.x + passo * std::sin(radianti(rotta)), pos.y + passo * std::cos(radianti(rotta))};
        quota = v.quota_m;
    }
};

// Partita minima: un aereo, un sottomarino vero che va a nord a 5 kn; le boe passive in ascolto entro 6 km
// danno il rilevamento (con 3° d'errore), le attive e il dip entro 2 km anche la distanza
struct Scena {
    ControlloreAsw c;
    AereoDiProva a;
    Vec2 sub{1500.0, 2500.0};
    bool sottomarino = true;
    std::vector<AzioneAsw> azioni;
    Casuale caso{0xa5a};
    Casuale rumore{7};
    double ora = 12.0;
    std::function<bool(Vec2)> libera;   // l'acqua libera (vuota: ovunque)
    // le boe come in missione: le attive omni e di sola distanza (SSQ-47B) senza rilevamento, le passive omni col livello
    // che cala con la distanza (di serie le attive danno anche il rilevamento, il livello e' fisso)
    bool realistica = false;

    Scena(const std::string& classe, SituazioneAsw s, std::uint64_t seme, Vec2 partenza = {}, const std::string& carico = "",
          const DatiAsw* d = nullptr)
        : caso(seme) {
        ConfigurazioneAsw conf;
        conf.situazione = s;
        conf.carico = carico;
        conf.base = {0.0, (dati().mezzo(classe)->elicottero ? -20.0 : -300.0) * MIGLIO};   // TOS (§6.8)
        conf.zona = {0.0, 0.0};
        conf.raggio_zona_m = 20000.0;
        c = ControlloreAsw(d ? *d : dati(), classe, 7, conf, 0.0);
        a.pos = partenza;
    }

    void passo(double t, std::vector<Cue> cue = {}) {
        PercezioneAsw p;
        p.pos = a.pos;
        p.quota_m = a.quota;
        p.rotta = a.rotta;
        p.cue = std::move(cue);
        p.ora_locale = ora;
        p.acqua_libera = libera;
        if (sottomarino) {
            const auto seguite = c.boe_seguite(t);
            for (const auto& b : c.boe()) {
                if (std::find(seguite.begin(), seguite.end(), b.id) == seguite.end()) continue;
                const Vec2 bp = posizione_boa(b, t);
                const double d = distanza(bp, sub);
                const DatiBoa* db = dati().boa(b.tipo);
                Rivelazione r;
                r.boa_o_aereo = b.id;
                r.t = t;
                r.livello_db = realistica ? 20.0 * std::log10(6000.0 / std::max(d, 100.0)) : 3.0;
                r.sorgente = 99;
                r.sottomarino = true;
                if (b.attiva_sonar && d < 2000.0) {
                    r.distanza_m = d;
                    if (!realistica || db->funzione == FunzioneBoa::attiva_dir)
                        r.rilevamento = rilevamento(bp, sub) + 3.0 * (2.0 * rumore.uniforme() - 1.0);
                } else if (realistica && b.attiva_sonar) {
                    continue;
                } else if (!b.attiva_sonar && d < 6000.0) {
                    if (db->funzione == FunzioneBoa::passiva_dir)
                        r.rilevamento = rilevamento(bp, sub) + 3.0 * (2.0 * rumore.uniforme() - 1.0);
                } else {
                    continue;
                }
                p.acustiche.push_back(r);
            }
            if (c.dip().in_acqua() && distanza(a.pos, sub) < 4000.0) {
                Rivelazione r;
                r.boa_o_aereo = 7;
                r.t = t;
                r.distanza_m = distanza(a.pos, sub);
                r.rilevamento = rilevamento(a.pos, sub);
                r.sorgente = 99;
                r.sottomarino = true;
                p.acustiche.push_back(r);
            }
        }
        // il MAD come in missione, semplificato: l'aereo basso sulla verticale del battello (entro 300 m)
        if (realistica && sottomarino && c.mezzo()->mad && a.quota <= 150.0 && distanza(a.pos, sub) <= 300.0) p.mad.push_back(99);
        auto x = c.passo(t, p, caso);
        azioni.insert(azioni.end(), x.begin(), x.end());
        a.vola(c.volo(), 5.0);
        sub.y += 5.0 * NODO * 5.0;
    }

    bool passato_per(std::vector<StatoAsw> ordine) const {
        std::size_t i = 0;
        for (const auto& s : c.storia())
            if (i < ordine.size() && s.a == ordine[i]) ++i;
        return i == ordine.size();
    }
    int massimo_per_bersaglio(AzioneAsw::Tipo t) const {
        std::map<int, int> n;
        for (const auto& x : azioni) if (x.tipo == t) ++n[x.bersaglio];
        int m = 0;
        for (const auto& [b, k] : n) m = std::max(m, k);
        return m;
    }
    int conta(AzioneAsw::Tipo t) const {
        return static_cast<int>(std::count_if(azioni.begin(), azioni.end(), [&](const AzioneAsw& x) { return x.tipo == t; }));
    }
};

Cue cue_sosus(double t_fix, double raggio_m) {
    Cue c;
    c.tipo = Cue::Tipo::sosus;
    c.pos = {0.0, 0.0};
    c.t = t_fix;
    c.raggio_m = raggio_m;
    return c;
}

}  // namespace

TEST_CASE("asw aereo: dati, classi ed emettitori coerenti") {
    const DatiAsw& d = dati();
    CHECK(d.mezzi.size() == 14);   // con l'SH-2F delle Knox, l'Hkp 4 svedese (30/10/2026) e il Lynx delle Type 22 (31/10/2026)
    CHECK(d.boe.size() == 17);
    CHECK(d.pensiero_s == doctest::Approx(5.0));
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto el = carica_elettronica(M688_DATI "/emettitori.toml");
    for (const auto& [id, m] : d.mezzi) {
        CAPTURE(id);
        CHECK(classi.count(id) == 1);   // ogni mezzo e' una classe (s3a ... be12 col segnaposto)
        if (!m.radar.empty()) CHECK(el.radar.count(m.radar) == 1);
        if (!m.sonar.empty()) CHECK(d.sonar(m.sonar) != nullptr);
        CHECK(m.carichi.count(m.carico_predefinito) == 1);
        CHECK(tempo_in_zona_h(m, 50.0 * MIGLIO) > 0.0);
    }
    CHECK(d.mezzo("sh60f")->dal_anno == 1990);
    CHECK(el.radar.at("korshun").agilita_frequenza);
    CHECK(el.radar.at("aps137").firma_unica);
    // TOS (§6.8): SH-60B 3 h a 50 nm e 1 h a 150 nm; P-3C 6,9 h a 200 nm
    CHECK(tempo_in_zona_h(*d.mezzo("sh60b"), 50.0 * MIGLIO) == doctest::Approx(3.0));
    CHECK(tempo_in_zona_h(*d.mezzo("sh60b"), 150.0 * MIGLIO) == doctest::Approx(1.0));
    CHECK(tempo_in_zona_h(*d.mezzo("p3c"), 200.0 * MIGLIO) == doctest::Approx(6.89).epsilon(0.01));
}

TEST_CASE("asw aereo: sonoboe, discesa, stabilizzazione, guasti e deriva in proporzione") {
    const DatiAsw& d = dati();
    CHECK(stabilizzazione_s(30.0) == doctest::Approx(100.0));
    CHECK(stabilizzazione_s(60.0) == doctest::Approx(125.0));
    CHECK(stabilizzazione_s(300.0) == doctest::Approx(240.0));
    Casuale caso(11);
    const DatiDifficolta& norm = d.difficolta.at(DifficoltaAsw::normale);
    const Sonoboa b = crea_boa(*d.boa("ssq53b"), {0, 0}, 457.2, 100.0, 7001, 7, norm, caso);
    CHECK(b.t_impatto == doctest::Approx(100.0 + 457.2 / 30.0));   // ~30 m/s col paracadute
    CHECK(b.t_attiva == doctest::Approx(b.t_impatto + 100.0));      // idrofono a 90 ft
    CHECK(b.t_fine == doctest::Approx(b.t_impatto + 3600.0));
    CHECK_FALSE(b.attiva_sonar);
    CHECK(crea_boa(*d.boa("ssq62b"), {0, 0}, 100.0, 0.0, 1, 7, norm, caso).attiva_sonar);

    // boe guaste: RGB-1 al 25%, SSQ-53B al 5%; con la difficolta' facile nessuna
    const auto guaste = [&](const char* tipo, DifficoltaAsw l) {
        int n = 0;
        for (int i = 0; i < 20000; ++i) n += crea_boa(*d.boa(tipo), {0, 0}, 500.0, 0.0, i, 1, d.difficolta.at(l), caso).guasta;
        return n / 20000.0;
    };
    CHECK(guaste("rgb1", DifficoltaAsw::normale) == doctest::Approx(0.25).epsilon(0.05));
    CHECK(guaste("ssq53b", DifficoltaAsw::normale) == doctest::Approx(0.05).epsilon(0.12));
    CHECK(guaste("rgb1", DifficoltaAsw::facile) == 0.0);

    // deriva 0,5-2 kn solo nel realistico
    const Sonoboa r = crea_boa(*d.boa("ssq53b"), {0, 0}, 500.0, 0.0, 1, 1, d.difficolta.at(DifficoltaAsw::realistico), caso);
    const double v = std::hypot(r.deriva.x, r.deriva.y) / NODO;
    CHECK(v >= 0.5);
    CHECK(v <= 2.0);
    CHECK(distanza(posizione_boa(r, r.t_impatto + 3600.0), r.pos) == doctest::Approx(v * NODO * 3600.0));
    CHECK(std::hypot(b.deriva.x, b.deriva.y) == 0.0);
    CHECK_FALSE(boa_in_ascolto(b, b.t_attiva - 1.0));
    CHECK(boa_in_ascolto(b, b.t_attiva + 1.0) != b.guasta);
    CHECK_FALSE(boa_in_ascolto(b, b.t_fine + 1.0));
}

TEST_CASE("asw aereo: geometria degli schemi di posa") {
    ParametriSchema p;
    p.datum = {10000.0, 20000.0};
    p.rotta_deg = 0.0;
    p.portata_m = 4000.0;
    p.velocita_kn = 10.0;
    p.ritardo_s = 1200.0;
    p.raggio0_m = 1852.0;
    const double r_t = raggio_incertezza_m(1852.0, 10.0, 1200.0);
    CHECK(r_t == doctest::Approx(1852.0 + 10.0 * NODO * 1200.0));

    SUBCASE("barriera: in linea perpendicolare alla rotta, S = 1,75 R50") {
        const auto v = posa_schema(SchemaBoe::barriera, p);
        REQUIRE(v.size() == 6);
        for (std::size_t i = 0; i < v.size(); ++i) CHECK(v[i].y == doctest::Approx(p.datum.y));
        for (std::size_t i = 1; i < v.size(); ++i) CHECK(distanza(v[i - 1], v[i]) == doctest::Approx(7000.0));
        CHECK((v.front().x + v.back().x) / 2.0 == doctest::Approx(p.datum.x));
    }
    SUBCASE("cerchio: anello a 0,7·(r0 + u·t) e boa centrale") {
        const auto v = posa_schema(SchemaBoe::cerchio, p);
        REQUIRE(v.size() == 9);
        for (int i = 0; i < 8; ++i) CHECK(distanza(v[i], p.datum) == doctest::Approx(0.7 * r_t));
        CHECK(distanza(v.back(), p.datum) == doctest::Approx(0.0));
    }
    SUBCASE("chevron: vertice verso il sottomarino, bracci in avanti") {
        const auto v = posa_schema(SchemaBoe::chevron, p);
        REQUIRE(v.size() == 6);
        for (const auto& x : v) CHECK(x.y > p.datum.y + r_t);   // tutto davanti al vertice
        CHECK(v[0].x < p.datum.x);
        CHECK(v[3].x > p.datum.x);
    }
    SUBCASE("cuneo e trappola davanti alla fuga") {
        for (const auto& x : posa_schema(SchemaBoe::cuneo, p)) CHECK(x.y > p.datum.y);
        const auto t = posa_schema(SchemaBoe::trappola, p);
        REQUIRE(t.size() == 4);
        for (const auto& x : t)
            CHECK(x.y == doctest::Approx(p.datum.y + 10.0 * NODO * 1200.0 + 1.5 * 4000.0));
    }
    SUBCASE("box DICASS di lato 1,5 kyd") {
        const auto v = posa_schema(SchemaBoe::box_dicass, p);
        REQUIRE(v.size() == 4);
        CHECK(distanza(v[0], v[1]) == doctest::Approx(1372.0));
        CHECK(distanza(v[0], v[2]) == doctest::Approx(1372.0 * std::sqrt(2.0)));
    }
    SUBCASE("sovietici: barriera circolare, intercettazione, spirale, scacchiera") {
        const auto c = posa_schema(SchemaBoe::barriera_circolare, p);
        for (const auto& x : c) CHECK(distanza(x, p.datum) == doctest::Approx(std::max(3000.0, r_t)));
        CHECK(distanza(c[0], c[1]) <= 0.75 * 4000.0 * 1.01);
        for (const auto& x : posa_schema(SchemaBoe::intercettazione, p)) {
            CHECK(distanza(x, p.datum) >= 3000.0);
            CHECK(std::abs(normalizza_180(rilevamento(p.datum, x))) <= 60.0 + 1e-6);   // arco di 120° davanti
        }
        const auto s = posa_schema(SchemaBoe::spirale, p);
        REQUIRE(s.size() == 10);
        for (std::size_t i = 1; i < s.size(); ++i) CHECK(distanza(s[i], p.datum) > distanza(s[i - 1], p.datum));
        CHECK(distanza(s[6], p.datum) == doctest::Approx(1.5 * distanza(s[0], p.datum)));   // +50% a giro
        CHECK(posa_schema(SchemaBoe::scacchiera, p).size() == 16);
        p.boe = 7;
        CHECK(posa_schema(SchemaBoe::scacchiera, p).size() == 7);
    }
}

TEST_CASE("asw aereo: MAD sulla tabella del §6.3") {
    // R50 obliqua (OPTEVFOR, rumore 0,1 gamma): 688 ~450 m, Victor III ~455, Akula ~555, Typhoon ~690, Kilo 300-350
    CHECK(portata_mad_m(4000.0) == doctest::Approx(376.0));
    CHECK(portata_mad_m(6900.0) == doctest::Approx(450.0).epsilon(0.015));
    CHECK(portata_mad_m(7000.0) == doctest::Approx(455.0).epsilon(0.015));
    CHECK(portata_mad_m(13000.0) == doctest::Approx(555.0).epsilon(0.015));
    CHECK(portata_mad_m(25000.0) == doctest::Approx(690.0).epsilon(0.015));
    CHECK(portata_mad_m(2300.0) >= 300.0);
    CHECK(portata_mad_m(3200.0) <= 350.0);
    CHECK(portata_mad_m(6900.0, 0.4) < portata_mad_m(6900.0));   // mare agitato, alte latitudini
    CHECK(probabilita_mad(450.0, 450.0) == doctest::Approx(0.5));
    CHECK(probabilita_mad(450.0, 450.0 - 0.2 * 450.0) == doctest::Approx(0.8413).epsilon(0.001));
    // spazzata: P-3 a 300 ft contro un 688 a 400 ft ~400 m, a 1000 ft ~210 m: andare profondi la dimezza
    const double R = portata_mad_m(6900.0);
    CHECK(spazzata_mad_m(R, (300.0 + 400.0) * 0.3048) == doctest::Approx(400.0).epsilon(0.03));
    CHECK(spazzata_mad_m(R, (300.0 + 1000.0) * 0.3048) == doctest::Approx(210.0).epsilon(0.05));
    CHECK(spazzata_mad_m(R, 600.0) == 0.0);

    // passaggi in linea retta a diverse distanze laterali: la frequenza dei contatti segue probabilita_mad
    DatiMad m;
    Casuale caso(5);
    for (const double lato : {0.0, 350.0, 450.0}) {
        int sentiti = 0;
        const int passaggi = 3000;
        for (int k = 0; k < passaggi; ++k) {
            SensoreMad s(m);
            const std::vector<BersaglioMad> b{{3, {lato, 0.0}, 100.0, 6900.0}};
            for (double y = -3000.0; y <= 3000.0; y += 50.0)
                if (!s.passo({0.0, y}, 76.0, 0.1, b, caso).empty()) ++sentiti;
        }
        const double atteso = probabilita_mad(R, std::hypot(lato, 176.0));
        CAPTURE(lato);
        CHECK(sentiti / static_cast<double>(passaggi) == doctest::Approx(atteso).epsilon(0.05));
    }
}

TEST_CASE("asw aereo: tempi del ciclo del sonar a immersione") {
    const DatiAsw& d = dati();
    Casuale caso(3);
    const auto stazione = [&](const char* sonar, bool insegui, int* ping = nullptr) {
        CicloDip c(d.sonar(sonar));
        c.inizia(0.0, 60.0, insegui, caso);
        int n = 0;
        double t = 0.0, cavo_max = 0.0;
        while (!c.finito() && t < 7200.0) {
            t += 1.0;
            n += c.avanza(t);
            cavo_max = std::max(cavo_max, c.cavo_m());
        }
        CHECK(cavo_max == doctest::Approx(d.sonar(sonar)->hover_m + 60.0));
        CHECK(c.cavo_m() == 0.0);
        CHECK(t == doctest::Approx(c.durata_stazione_s()).epsilon(0.01));
        if (ping) *ping = n;
        return t;
    };
    for (int i = 0; i < 20; ++i) {
        // SH-3H (§6.4): inseguimento 4-6 min col salto di 5 nm; ricerca dip di 15 min e salto di 10 nm: ~20 min
        const DatiSonarImmersione& h = *d.sonar("aqs13b");
        int ping = 0;
        const double insegui = stazione("aqs13b", true, &ping) + h.salto_inseguimento_m / (h.salto_velocita_kn * NODO);
        CHECK(insegui >= 4.0 * 60.0);
        CHECK(insegui <= 6.5 * 60.0);
        CHECK(ping >= h.ping_min);
        CHECK(ping <= h.ping_max);
        const double ricerca = stazione("aqs13b", false) + h.salto_ricerca_m / (h.salto_velocita_kn * NODO);
        CHECK(ricerca >= 18.0 * 60.0);
        CHECK(ricerca <= 23.0 * 60.0);
        // Ka-27 (§6.5): ciclo completo 10-12 min, passo di 4-6 km
        const DatiSonarImmersione& k = *d.sonar("vgs3");
        const double ka = stazione("vgs3", true) + k.salto_inseguimento_m / (k.salto_velocita_kn * NODO);
        CHECK(ka >= 10.0 * 60.0);
        CHECK(ka <= 12.8 * 60.0);
    }
    // un contatto in ascolto fa passare ai ping
    CicloDip c(d.sonar("vgs3"));
    c.inizia(0.0, 100.0, false, caso);
    double t = 0.0;
    while (c.fase() != FaseDip::ascolto) c.avanza(t += 1.0);
    c.forza_attivo(t);
    CHECK(c.fase() == FaseDip::ping);
    CHECK(c.avanza(t + 0.5) == 1);
    c.recupera(t + 1.0);
    CHECK(c.fase() == FaseDip::recupero);
}

TEST_CASE("asw aereo: P-3C in guerra su un datum SOSUS, dal datum all'attacco, deterministico") {
    const auto gioca = [](std::uint64_t seme) {
        Scena s("p3c", SituazioneAsw::guerra, seme);
        s.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        for (double t = 5.0; t <= 4.0 * 3600.0; t += 5.0) s.passo(t);
        return s;
    };
    const Scena a = gioca(21);
    CHECK(a.passato_per({StatoAsw::datum, StatoAsw::contatto, StatoAsw::localizzazione, StatoAsw::attacco,
                         StatoAsw::verifica}));
    CHECK(a.conta(AzioneAsw::Tipo::posa_boa) >= 6);
    CHECK(a.conta(AzioneAsw::Tipo::lancia_arma) >= 1);
    CHECK(a.conta(AzioneAsw::Tipo::marcatore) >= 1);
    for (const auto& x : a.azioni)
        if (x.tipo == AzioneAsw::Tipo::lancia_arma) CHECK(x.oggetto == "mk46");
    // stessa partita con lo stesso seme
    const Scena b = gioca(21);
    REQUIRE(a.c.storia().size() == b.c.storia().size());
    for (std::size_t i = 0; i < a.c.storia().size(); ++i) {
        CHECK(a.c.storia()[i].t == b.c.storia()[i].t);
        CHECK(a.c.storia()[i].a == b.c.storia()[i].a);
    }
    REQUIRE(a.azioni.size() == b.azioni.size());
    for (std::size_t i = 0; i < a.azioni.size(); ++i) {
        CHECK(a.azioni[i].tipo == b.azioni[i].tipo);
        CHECK(a.azioni[i].pos.x == b.azioni[i].pos.x);
    }
}

TEST_CASE("asw aereo: in pace niente armi, attacco simulato col SUS e inseguimento") {
    Scena s("p3c", SituazioneAsw::pace, 21);
    s.passo(0.0, {cue_sosus(-600.0, 3000.0)});
    for (double t = 5.0; t <= 4.0 * 3600.0; t += 5.0) s.passo(t);
    CHECK(s.passato_per({StatoAsw::datum, StatoAsw::contatto, StatoAsw::localizzazione, StatoAsw::inseguimento}));
    CHECK(s.conta(AzioneAsw::Tipo::lancia_arma) == 0);
    CHECK(s.conta(AzioneAsw::Tipo::sus) == 1);
    for (const auto& p : s.c.storia()) CHECK(p.a != StatoAsw::attacco);
}

TEST_CASE("asw aereo: la DICASS pinga a comando finche' ha i suoi ping-secondi (§4.1)") {
    const DatiBoa& d = *dati().boa("ssq62b");
    CHECK(d.ping_secondi == doctest::Approx(100.0));
    CHECK(d.impulso_s == doctest::Approx(0.25));   // dal sensore di acustica.toml: 400 ping per boa
    CHECK(dati().boa("ssq47b")->impulso_s == doctest::Approx(0.1));
    Sonoboa b;
    b.ping_secondi_usati = 99.75;
    CHECK(ping_disponibile(b, d));
    b.ping_secondi_usati = 99.8;
    CHECK_FALSE(ping_disponibile(b, d));
    CHECK(ping_disponibile(b, *dati().boa("rgb55a")));   // senza budget nei dati: senza limite
    // con 1 ping-secondo la boa fa 4 ping, poi il controllore passa a un'altra; coi 100 una boa ne fa di piu'
    DatiAsw scarsa = dati();
    scarsa.boe["ssq62b"].ping_secondi = 1.0;
    const auto massimo = [&](const DatiAsw* dd) {
        Scena s("p3c", SituazioneAsw::pace, 21, {}, "", dd);
        s.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        for (double t = 5.0; t <= 4.0 * 3600.0; t += 5.0) s.passo(t);
        return std::pair<int, int>{s.massimo_per_bersaglio(AzioneAsw::Tipo::comanda_boa), s.conta(AzioneAsw::Tipo::comanda_boa)};
    };
    const auto piena = massimo(nullptr), poca = massimo(&scarsa);
    MESSAGE("ping a comando: coi 100 ping-secondi ", piena.second, " (al piu' ", piena.first, " per boa); con 1 ", poca.second,
            " (al piu' ", poca.first, " per boa)");
    CHECK(piena.first > 4);
    CHECK(poca.first == 4);
}

TEST_CASE("asw aereo: il Lynx (MATCH) va sul contatto della nave e attacca col MAD e lo Sting Ray") {
    // senza boe ne' sonar: la nave gli passa il contatto ogni minuto (cue contatto_nave, 500 m), lui lo conferma col MAD e
    // sgancia lo Sting Ray (docs/ricerca/38)
    int lanci = 0, stingray = 0;
    for (std::uint64_t seme = 1; seme <= 6; ++seme) {
        Scena s("lynx", SituazioneAsw::guerra, seme, {0.0, -10.0 * MIGLIO});
        s.realistica = true;
        for (double t = 0.0; t <= 2.0 * 3600.0; t += 5.0) {
            std::vector<Cue> cue;
            if (std::fmod(t, 60.0) < 1.0) {
                Cue c;
                c.tipo = Cue::Tipo::contatto_nave;
                c.pos = {s.sub.x + 300.0 * (2.0 * s.rumore.uniforme() - 1.0), s.sub.y + 300.0 * (2.0 * s.rumore.uniforme() - 1.0)};
                c.t = t;
                c.raggio_m = 500.0;
                cue.push_back(c);
            }
            s.passo(t, cue);
        }
        lanci += s.conta(AzioneAsw::Tipo::lancia_arma) > 0;
        stingray += std::any_of(s.azioni.begin(), s.azioni.end(),
                                [](const AzioneAsw& x) { return x.tipo == AzioneAsw::Tipo::lancia_arma && x.oggetto == "stingray"; });
    }
    MESSAGE("Lynx su 6 semi: attacchi in ", lanci, ", con lo Sting Ray in ", stingray);
    CHECK(dati().mezzo("lynx")->canali_boe == 0);
    CHECK(lanci >= 3);
    CHECK(stingray == lanci);
}

TEST_CASE("asw aereo: l'SH-2F (LAMPS I) localizza con le omni e le SSQ-47B, fino al fix e al Mk 46") {
    // senza boe direzionali: le SSQ-41B che sentono insieme stringono la zona, il box di SSQ-47B da' le distanze, la
    // trilaterazione il fix (docs/ricerca/37)
    int fix = 0, lanci = 0, con_47b = 0;
    double migliore = 1e9;
    for (std::uint64_t seme = 1; seme <= 12; ++seme) {
        Scena s("sh2f", SituazioneAsw::guerra, seme, {0.0, -20.0 * MIGLIO});
        s.realistica = true;
        s.passo(0.0, {cue_sosus(-300.0, 3000.0)});
        double errore = 1e9;
        for (double t = 5.0; t <= 3.0 * 3600.0; t += 5.0) {
            s.passo(t);
            if (s.c.traccia().valida) errore = std::min(errore, s.c.traccia().errore_m);
        }
        migliore = std::min(migliore, errore);
        con_47b += std::any_of(s.c.boe().begin(), s.c.boe().end(), [](const Sonoboa& b) { return b.tipo == "ssq47b"; });
        fix += s.passato_per({StatoAsw::localizzazione, StatoAsw::attacco});
        lanci += s.conta(AzioneAsw::Tipo::lancia_arma) > 0;
    }
    MESSAGE("SH-2F su 12 semi: SSQ-47B posate in ", con_47b, ", fix e attacco in ", fix, ", Mk 46 in ", lanci, "; errore minimo ", migliore, " m");
    CHECK(con_47b >= 7);   // il 30% dei fix e' il passaggio MAD, senza boe attive
    CHECK(fix >= 2);       // senza il rilevamento della nave il LAMPS I da solo localizza di rado
    CHECK(lanci >= 2);
    CHECK(migliore <= 914.0);
}

TEST_CASE("asw aereo: elicotteri col sonar a immersione, il Ka-27 non cala di notte") {
    SUBCASE("SH-3H sul datum: spirale di dip, ping, attacco in guerra") {
        Scena s("sh3h", SituazioneAsw::guerra, 4);
        s.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 2.0 * 3600.0; t += 5.0) s.passo(t);
        CHECK(s.conta(AzioneAsw::Tipo::cala_sonar) >= 1);
        CHECK(s.conta(AzioneAsw::Tipo::ping) >= 3);
        CHECK(s.conta(AzioneAsw::Tipo::recupera_sonar) >= 1);
        CHECK(s.passato_per({StatoAsw::datum, StatoAsw::contatto, StatoAsw::localizzazione}));
    }
    SUBCASE("Hkp 4 svedese sul datum: senza boe cala l'HS-73 e pinga (docs/ricerca/35)") {
        Scena s("hkp4", SituazioneAsw::guerra, 4);
        s.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 2.0 * 3600.0; t += 5.0) s.passo(t);
        CHECK(s.conta(AzioneAsw::Tipo::cala_sonar) >= 1);
        CHECK(s.conta(AzioneAsw::Tipo::ping) >= 3);
        CHECK(s.conta(AzioneAsw::Tipo::posa_boa) == 0);   // non ne ha
    }
    SUBCASE("Hkp 4 col carico delle bombe: sul sottomarino localizzato sgancia le Sjunkbomb 51, non i Tp 42") {
        Scena s("hkp4", SituazioneAsw::guerra, 4, {}, "bombe");
        s.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 2.0 * 3600.0; t += 5.0) s.passo(t);
        int bombe = 0;
        for (const auto& x : s.azioni)
            if (x.tipo == AzioneAsw::Tipo::lancia_arma) {
                CHECK(x.oggetto == "sjunkbomb51");
                ++bombe;
            }
        MESSAGE("Hkp 4 con le bombe: ", bombe, " Sjunkbomb 51 sganciate");
        CHECK(bombe >= 1);
    }
    SUBCASE("Ka-27 di giorno cala, di notte posa solo boe") {
        Scena g("ka27", SituazioneAsw::guerra, 4);
        g.sottomarino = false;
        g.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 3600.0; t += 5.0) g.passo(t);
        CHECK(g.conta(AzioneAsw::Tipo::cala_sonar) >= 1);
        Scena n("ka27", SituazioneAsw::guerra, 4);
        n.ora = 23.0;
        n.sottomarino = false;
        n.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 3600.0; t += 5.0) n.passo(t);
        CHECK(n.conta(AzioneAsw::Tipo::cala_sonar) == 0);
        CHECK(n.conta(AzioneAsw::Tipo::posa_boa) >= 1);
        CHECK_FALSE(n.c.volo().luci);   // oscurato in caccia in guerra
    }
}

// il ghiaccio (LIB-I16 di trailer-1, 29/10/2026): boe, trasduttore e armi solo in acqua libera. Sul datum sotto il
// pack lo schema va al margine dell'acqua libera, entro la portata delle boe; tutto pack: si rinuncia al datum
TEST_CASE("asw aereo: sotto il pack le boe e il sonar a immersione solo in acqua libera") {
    const auto margine = [](double x0) { return [x0](Vec2 p) { return p.x > x0; }; };   // pack a ovest di x0
    const auto conta_motivo = [](const Scena& s, const std::string& m) {
        return std::count_if(s.c.storia().begin(), s.c.storia().end(), [&](const ControlloreAsw::Passaggio& x) { return x.motivo == m; });
    };
    SUBCASE("P-3C: lo schema al margine, nessuna boa ne' arma sul ghiaccio") {
        Scena s("p3c", SituazioneAsw::guerra, 21);
        s.libera = margine(4000.0);
        s.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        for (double t = 5.0; t <= 3.0 * 3600.0; t += 5.0) s.passo(t);
        CHECK(s.conta(AzioneAsw::Tipo::posa_boa) >= 1);
        for (const auto& x : s.azioni)
            if (x.tipo == AzioneAsw::Tipo::posa_boa || x.tipo == AzioneAsw::Tipo::lancia_arma) CHECK(x.pos.x > 4000.0);
    }
    SUBCASE("P-3C: tutto pack, rinuncia al datum") {
        Scena s("p3c", SituazioneAsw::guerra, 21);
        s.libera = [](Vec2) { return false; };
        s.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        for (double t = 5.0; t <= 3600.0; t += 5.0) s.passo(t);
        CHECK(s.conta(AzioneAsw::Tipo::posa_boa) == 0);
        CHECK(s.conta(AzioneAsw::Tipo::lancia_arma) == 0);
        CHECK(conta_motivo(s, "datum sul pack") >= 1);
    }
    SUBCASE("SH-3H: il dip nell'acqua libera; tutto pack, niente dip") {
        Scena s("sh3h", SituazioneAsw::guerra, 4);
        s.libera = margine(2000.0);
        s.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 2.0 * 3600.0; t += 5.0) s.passo(t);
        CHECK(s.conta(AzioneAsw::Tipo::cala_sonar) >= 1);
        for (const auto& x : s.azioni)   // in hover entro 150 m dalla stazione, che e' libera
            if (x.tipo == AzioneAsw::Tipo::cala_sonar || x.tipo == AzioneAsw::Tipo::posa_boa) CHECK(x.pos.x > 2000.0 - 150.0);
        Scena n("sh3h", SituazioneAsw::guerra, 4);
        n.libera = [](Vec2) { return false; };
        n.passo(0.0, {cue_sosus(-300.0, 1500.0)});
        for (double t = 5.0; t <= 3600.0; t += 5.0) n.passo(t);
        CHECK(n.conta(AzioneAsw::Tipo::cala_sonar) == 0);
        CHECK(n.conta(AzioneAsw::Tipo::posa_boa) == 0);
    }
    SUBCASE("senza ghiaccio come prima") {
        Scena a("p3c", SituazioneAsw::guerra, 21), b("p3c", SituazioneAsw::guerra, 21);
        b.libera = [](Vec2) { return true; };
        a.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        b.passo(0.0, {cue_sosus(-600.0, 3000.0)});
        for (double t = 5.0; t <= 3600.0; t += 5.0) { a.passo(t); b.passo(t); }
        REQUIRE(a.azioni.size() == b.azioni.size());
        for (std::size_t i = 0; i < a.azioni.size(); ++i) CHECK(a.azioni[i].pos.x == b.azioni[i].pos.x);
    }
}

TEST_CASE("asw aereo: falsi allarmi in proporzione al tasso delle boe, creduti con crede_falso") {
    // nessun sottomarino: ogni contatto e' un falso allarme spontaneo delle boe seguite
    const auto prova = [](const char* classe, double* attesi, int* contatti, int* creduti, int* falsi) {
        for (std::uint64_t seme = 1; seme <= 12; ++seme) {
            Scena s(classe, SituazioneAsw::pace, seme);
            s.sottomarino = false;
            s.passo(0.0);
            for (double t = 5.0; t <= 6.0 * 3600.0; t += 5.0) {
                if (s.c.stato() == StatoAsw::ricerca)
                    for (const int id : s.c.boe_seguite(t)) {
                        const auto& b = *std::find_if(s.c.boe().begin(), s.c.boe().end(),
                                                      [&](const Sonoboa& x) { return x.id == id; });
                        *attesi += dati().boa(b.tipo)->falsi_allarmi_h * dati().mezzo(classe)->falsi_allarmi * 5.0 / 3600.0;
                    }
                s.passo(t);
            }
            for (const auto& p : s.c.storia()) {
                if (p.da == StatoAsw::ricerca && p.a == StatoAsw::contatto) ++*contatti;
                if (p.motivo == "falso contatto creduto") ++*creduti;
                if (p.motivo.rfind("falso allarme", 0) == 0) ++*falsi;   // anche «falso allarme, si riprende»
            }
        }
    };
    double attesi = 0.0;
    int contatti = 0, creduti = 0, falsi = 0;
    prova("il38", &attesi, &contatti, &creduti, &falsi);
    CAPTURE(attesi);
    CAPTURE(contatti);
    REQUIRE(attesi > 40.0);
    CHECK(contatti == doctest::Approx(attesi).epsilon(0.25));
    // URSS: crede_falso 0,15 con l'equipaggio medio
    CHECK(creduti / static_cast<double>(creduti + falsi) == doctest::Approx(0.15).epsilon(0.6));
    // il P-3 (falsi allarmi 0,03/h per boa) ne ha molti meno dell'Il-38 (0,25/h)
    double attesi_p3 = 0.0;
    int contatti_p3 = 0, c2 = 0, f2 = 0;
    prova("p3c", &attesi_p3, &contatti_p3, &c2, &f2);
    CHECK(contatti_p3 < contatti / 3);
}

TEST_CASE("asw aereo: il mare del meteo sul MAD e sul sonar a immersione (dossier 21 §8)") {
    CHECK(rumore_mad_col_mare(0.1, 3) == doctest::Approx(0.1));
    CHECK(portata_mad_m(4000.0, rumore_mad_col_mare(0.1, 4)) == doctest::Approx(0.8 * portata_mad_m(4000.0)));
    CHECK(portata_mad_m(4000.0, rumore_mad_col_mare(0.1, 6)) == doctest::Approx(0.6 * portata_mad_m(4000.0)));
    CHECK(dati().boa("ssq53b")->mare_max == doctest::Approx(6.0));
    ConfigurazioneAsw conf;
    conf.situazione = SituazioneAsw::guerra;
    const ControlloreAsw sh60f(dati(), "sh60f", 7, conf, 0.0), ka27(dati(), "ka27", 8, conf, 0.0);
    PercezioneAsw p;
    p.ora_locale = 12.0;
    p.mare = 4;
    p.hs_m = 2.0;
    CHECK(sh60f.dip_possibile(p));
    p.mare = 5;
    p.hs_m = 2.8;
    CHECK(sh60f.dip_possibile(p));           // AQS-13 a mare 5 con onde sotto i 10 ft
    p.hs_m = 3.5;
    CHECK_FALSE(sh60f.dip_possibile(p));     // oltre i 10 ft no
    CHECK(ka27.dip_possibile(p));            // il Ka-27 fino a 5
    p.mare = 6;
    CHECK_FALSE(ka27.dip_possibile(p));
    p.mare = 3;
    p.hs_m = 0.9;
    p.temporale_vicino = true;
    CHECK_FALSE(sh60f.dip_possibile(p));     // fulmini entro 5 nm
    p.temporale_vicino = false;
    p.vento_kn = 40.0;
    CHECK_FALSE(ka27.dip_possibile(p));
}

TEST_CASE("evasione dell'IA: tonfo, priorita' degli stimoli, durata e fondo in acque basse") {
    BattelloEvasione b;
    b.rotta_deg = 90.0;
    b.velocita_kn = 12.0;
    b.quota_m = 60.0;
    b.strato_m = 80.0;
    b.velocita_max_kn = 30.0;
    b.quota_max_m = 400.0;
    Casuale caso(7);
    Evasione e;
    CHECK_FALSE(e.comando(0.0, b));
    // un tonfo a nord: silenzio a 5 kn, 50 m sotto lo strato, via di 60-90° dalla rotta e non verso il tonfo
    e.stimolo(StimoloEvasione::tonfo, 0.0, 100.0, b, caso);
    auto c = e.comando(100.0, b);
    REQUIRE(c);
    CHECK(c->velocita_kn == doctest::Approx(5.0));
    CHECK(c->quota_m == doctest::Approx(130.0));
    const double virata = std::abs(normalizza_180(c->rotta_deg - b.rotta_deg));
    CHECK(virata >= 60.0);
    CHECK(virata <= 90.0);
    CHECK(std::abs(normalizza_180(c->rotta_deg - 0.0)) >= 60.0);
    // un ping lontano non cambia la manovra; un siluro si', a tutta forza e via dalla sorgente
    e.stimolo(StimoloEvasione::ping_lontano, 200.0, 200.0, b, caso);
    CHECK(e.comando(200.0, b)->stimolo == StimoloEvasione::tonfo);
    e.stimolo(StimoloEvasione::siluro, 270.0, 300.0, b, caso);
    c = e.comando(300.0, b);
    CHECK(c->stimolo == StimoloEvasione::siluro);
    CHECK(c->velocita_kn == doctest::Approx(30.0));
    CHECK(std::abs(normalizza_180(c->rotta_deg - 90.0)) <= 30.0);
    CHECK(e.manovre() == 2);
    // finita la corsa del siluro (480 s) il battello torna suo
    CHECK(e.comando(300.0 + 479.0, b));
    CHECK_FALSE(e.comando(300.0 + 481.0, b));
    // acque basse: un SSK col sonar vicino si posa sul fondo, spesso; un SSN meno; mai per un tonfo
    int ssk = 0, ssn = 0, tonfi = 0;
    BattelloEvasione basso = b;
    basso.fondale_m = 90.0;
    for (int i = 0; i < 200; ++i) {
        Evasione x, y, z;
        basso.diesel = true;
        x.stimolo(StimoloEvasione::ping_vicino, 0.0, 0.0, basso, caso);
        ssk += x.comando(0.0, basso)->sul_fondo;
        z.stimolo(StimoloEvasione::tonfo, 0.0, 0.0, basso, caso);
        tonfi += z.comando(0.0, basso)->sul_fondo;
        basso.diesel = false;
        y.stimolo(StimoloEvasione::ping_vicino, 0.0, 0.0, basso, caso);
        ssn += y.comando(0.0, basso)->sul_fondo;
    }
    CHECK(ssk > 110);
    CHECK(ssn < 80);
    CHECK(ssn > 20);
    CHECK(tonfi == 0);
    Evasione f;
    basso.diesel = true;
    while (!f.comando(0.0, basso) || !f.comando(0.0, basso)->sul_fondo) {
        f = Evasione{};
        f.stimolo(StimoloEvasione::siluro, 0.0, 0.0, basso, caso);
    }
    CHECK(f.comando(0.0, basso)->velocita_kn == doctest::Approx(0.0));
    CHECK(f.comando(0.0, basso)->quota_m == doctest::Approx(89.5));
    CHECK(f.comando(1799.0, basso));   // sul fondo si resta mezz'ora
    // in acque profonde la quota resta entro il franco dal fondo e dalla quota massima
    BattelloEvasione stretto = b;
    stretto.fondale_m = 120.0;
    Evasione g;
    g.stimolo(StimoloEvasione::tonfo, 0.0, 0.0, stretto, caso);
    CHECK(g.comando(0.0, stretto)->quota_m == doctest::Approx(105.0));
    // ping vicino: una rotta sola via dal sonar, dosata, che non cambia ai ping delle stazioni dopo
    Evasione h;
    h.stimolo(StimoloEvasione::ping_vicino, 0.0, 0.0, b, caso);
    CHECK(h.comando(10.0, b)->velocita_kn == doctest::Approx(12.0));
    const double prima = h.comando(10.0, b)->rotta_deg;
    CHECK(std::abs(normalizza_180(prima - 180.0)) <= 20.0);
    h.stimolo(StimoloEvasione::ping_vicino, 270.0, 200.0, b, caso);
    CHECK(h.comando(200.0, b)->rotta_deg == doctest::Approx(prima));
    CHECK(h.comando(200.0, b)->velocita_kn == doctest::Approx(12.0));
    CHECK(h.manovre() == 1);
    // siluro: poppa al siluro e tutta forza; sotto il pavimento del siluro se lo scafo ci arriva (Alfa e Mk 46)
    BattelloEvasione alfa = b;
    alfa.quota_max_m = 800.0;
    alfa.velocita_max_kn = 40.0;
    alfa.pavimento_siluro_m = 450.0;
    Evasione k;
    k.stimolo(StimoloEvasione::siluro_vicino, 0.0, 0.0, alfa, caso);
    CHECK(k.comando(0.0, alfa)->velocita_kn == doctest::Approx(40.0));
    CHECK(std::abs(normalizza_180(k.comando(0.0, alfa)->rotta_deg - 180.0)) <= 10.0);
    CHECK(k.comando(0.0, alfa)->quota_m == doctest::Approx(500.0));
    BattelloEvasione victor = alfa;
    victor.quota_max_m = 450.0;   // non ci arriva: alla quota massima, col franco
    Evasione v;
    v.stimolo(StimoloEvasione::siluro, 0.0, 0.0, victor, caso);
    CHECK(v.comando(0.0, victor)->quota_m == doctest::Approx(430.0));
    CHECK(battello_diesel("foxtrot"));
    CHECK_FALSE(battello_diesel("victor3"));
}
