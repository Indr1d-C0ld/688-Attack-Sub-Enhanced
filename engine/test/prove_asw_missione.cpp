#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/mondo.hpp"

using namespace m688;
namespace fs = std::filesystem;

// ASW aereo dell'Enhanced nella missione (docs/13_asw_aereo.md, A3): controllori degli aerei, boe, armi col paracadute,
// la boa marcatrice degli script, i cue dagli script, il classico che non cambia

namespace {

constexpr double DT = 1.0 / 9.0;

// Un Alfa dell'IA rumoroso (20 kn a 100 m verso nord), il 688 del giocatore lontano, un P-3C (1) o un SH-60B; la
// configurazione ASW nella chiave `aereo_asw`
std::string scena(const std::string& nome, const std::string& aereo, const std::string& asw, const std::string& script = "",
                  const std::string& timer = "", double x_giocatore = -60000.0) {
    const auto f = fs::temp_directory_path() / ("m688_prova_asw_" + nome + ".lua");
    const std::string sub = "modello = { massima_kn = 40, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 12, sonar = 19 }";
    const std::string volo = "modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 }";
    std::ofstream o(f);
    o << "return { id = 'PROVA_ASW_" << nome << "', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa',\n"
      << "aereo_asw = " << asw << ",\n"
      << "script = { " << script << " },\n"
      << "varianti = { { probabilita = 1, unita = {\n"
      << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
         "  posizione = { x = " << x_giocatore << ", y = -60000 }, quota_m = 60, rotta = 0, velocita_kn = 0, " << sub << " },\n"
      << "{ id = 1, classe = '" << aereo << "', categoria = 'aereo', flag = 11, posizione = { x = 0, y = -20000 },"
         "  quota_m = 0, rotta = 0, velocita_kn = 100, " << volo << " },\n"
      << "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 },"
         "  quota_m = 100, rotta = 0, velocita_kn = 20, " << sub << ", rotta_punti = { { x = 0, y = 400000 } } },\n"
      << "}" << (timer.empty() ? "" : ", timer = { " + timer + " }") << " } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    explicit Partita(const std::string& file, std::uint64_t seme, bool asw = true)
        : m(file, seme), dm(classi, M688_DATI, seme, RegolaDanni::simulazione), ds(classi, M688_DATI, seme) {
        m.usa_asw(asw);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(DT);
            dm.avanza(m, DT);
            ds.avanza(m, DT);
            m.raccogli_danni(&ds, &dm);
        }
    }
};

const char* CUE_SOSUS = "{ { unita = 1, missione = 'caccia', situazione = 'guerra', zona = { x = 0, y = 0, raggio_m = 20000 },"
                        " cue = { tipo = 'sosus', x = 0, y = 2000, raggio_m = 3000, ritardo_s = 300, rotta = 0, velocita_kn = 18 } } }";

}  // namespace

TEST_CASE("asw missione: il classico non cambia, quote degli aerei per la vista") {
    Missione m(scena("classico", "p3c", CUE_SOSUS), 1);
    for (int i = 0; i < 9 * 60; ++i) m.avanza(DT);
    CHECK_FALSE(m.con_asw());
    CHECK_FALSE(m.controllato_asw(1));
    CHECK(m.boe().empty());
    CHECK(m.voli_asw().empty());
    CHECK(m.quota_aerea(m.unita().at(1)) == doctest::Approx(300.0));
    Mondo mondo;
    m.sincronizza(mondo);
    const auto it = std::find_if(mondo.navi().begin(), mondo.navi().end(), [](const Nave& n) { return n.id == 1; });
    REQUIRE(it != mondo.navi().end());
    CHECK(it->quota == doctest::Approx(-300.0));   // aerei: quota negativa, in volo a -quota metri
    Missione e(scena("classico_eli", "sh60b", CUE_SOSUS), 1);
    CHECK(e.quota_aerea(e.unita().at(1)) == doctest::Approx(150.0));
    // con la regola di simulazione ma senza ASW: gli aerei restano quelli del classico
    Partita senza(scena("senza", "p3c", CUE_SOSUS), 1, false);
    senza.avanza(120.0);
    CHECK_FALSE(senza.m.con_asw());
}

TEST_CASE("asw missione: P-3C in guerra dal cue SOSUS al Mk 46 col paracadute, deterministico") {
    const auto gioca = [](std::uint64_t seme) {
        auto p = std::make_unique<Partita>(scena("guerra", "p3c", CUE_SOSUS), seme);
        p->avanza(3.0 * 3600.0);
        return p;
    };
    const auto a = gioca(3);
    const Missione& m = a->m;
    REQUIRE(m.errori().empty());
    REQUIRE(m.con_asw());
    REQUIRE(m.controllore_asw(1));
    const ControlloreAsw& c = *m.controllore_asw(1);
    CHECK(c.mezzo()->id == "p3c");
    CHECK(m.boe().size() >= 6);
    for (const auto& b : m.boe()) CHECK(b.lanciatore == 1);
    // il datum, poi il contatto e l'attacco con un Mk 46 che parte col modello dell'Enhanced
    std::vector<StatoAsw> visti;
    for (const auto& s : c.storia()) visti.push_back(s.a);
    CHECK(std::find(visti.begin(), visti.end(), StatoAsw::datum) != visti.end());
    CHECK(std::find(visti.begin(), visti.end(), StatoAsw::contatto) != visti.end());
    REQUIRE_FALSE(m.lanci_aerei().empty());
    const LancioAereo& l = m.lanci_aerei().front();
    CHECK(l.aereo == 1);
    CHECK(l.tipo == "mk46");
    CHECK(l.arma >= 0);
    CHECK_FALSE(m.marcatori_asw().empty());   // fumogeno al lancio o sul MAD
    // la quota vera dell'aereo e il volo per la vista
    const auto voli = m.voli_asw();
    REQUIRE(voli.size() == 1);
    CHECK(voli[0].quota_m > 20.0);
    // stessa partita con lo stesso seme
    const auto b = gioca(3);
    REQUIRE(b->m.controllore_asw(1)->storia().size() == c.storia().size());
    for (std::size_t i = 0; i < c.storia().size(); ++i) {
        CHECK(b->m.controllore_asw(1)->storia()[i].t == c.storia()[i].t);
        CHECK(b->m.controllore_asw(1)->storia()[i].a == c.storia()[i].a);
    }
    REQUIRE(b->m.boe().size() == m.boe().size());
    for (std::size_t i = 0; i < m.boe().size(); ++i) CHECK(b->m.boe()[i].pos.x == m.boe()[i].pos.x);
}

TEST_CASE("asw missione: in pace e in ricognizione niente armi") {
    const std::string pace = "{ { unita = 1, situazione = 'pace', zona = { x = 0, y = 0, raggio_m = 20000 },"
                             " cue = { tipo = 'sosus', x = 0, y = 2000, raggio_m = 3000, ritardo_s = 300 } } }";
    Partita p(scena("pace", "p3c", pace), 3);
    p.avanza(3.0 * 3600.0);
    REQUIRE(p.m.errori().empty());
    REQUIRE(p.m.controllore_asw(1));
    CHECK(p.m.lanci_aerei().empty());
    for (const auto& s : p.m.controllore_asw(1)->storia()) CHECK(s.a != StatoAsw::attacco);
    const std::string rico = "{ { unita = 1, missione = 'ricognizione', situazione = 'guerra', zona = { x = 0, y = 0, raggio_m = 20000 },"
                             " cue = { tipo = 'sosus', x = 0, y = 2000, raggio_m = 3000, ritardo_s = 300 } } }";
    Partita r(scena("ricognizione", "p3c", rico), 3);
    r.avanza(3.0 * 3600.0);
    CHECK(r.m.lanci_aerei().empty());
}

TEST_CASE("asw missione: la boa marcatrice dello script diventa una DICASS, cue dallo script") {
    // l'aereo fa comparire una boa marcatrice sul suo punto dopo 60 s (come la missione 7), e a 30 s un cue
    const std::string script =
        "MARCA = function(u) m688.modello_classe(u, 'boa_marcatrice'); m688.modello_flag(u, 193); m688.nuova_unita(u) end, "
        "CUE = function(u) m688.cue_asw(u, 1, 0, 5000, 2000, 60) end";
    const std::string timer = "{ dopo_s = 60, unita = 1, gestore = 'MARCA' }, { dopo_s = 30, unita = 1, gestore = 'CUE' }";
    const std::string asw = "{ { unita = 1, situazione = 'guerra', zona = { x = 0, y = -20000, raggio_m = 20000 } } }";
    Partita p(scena("marcatrice", "sh60b", asw, script, timer), 2);
    p.avanza(120.0);
    REQUIRE(p.m.errori().empty());
    REQUIRE(p.m.controllore_asw(1));
    const auto boe = p.m.boe();
    const auto dicass = std::find_if(boe.begin(), boe.end(), [](const Sonoboa& b) { return b.tipo == "ssq62b" && b.lanciatore == 1; });
    REQUIRE(dicass != boe.end());
    CHECK(dicass->attiva_sonar);
    CHECK_FALSE(dicass->guasta);
    // il cue dello script ha portato il controllore sul datum
    bool datum = false;
    for (const auto& s : p.m.controllore_asw(1)->storia()) datum = datum || s.a == StatoAsw::datum;
    CHECK(datum);
}

TEST_CASE("asw missione: missione 7 nell'Enhanced, il LAMPS in ricognizione col controllore") {
    const std::string file = std::string(M688_MISSIONI) + "/SCENE7A.lua";
    if (!fs::exists(file)) return;
    for (const std::uint64_t seme : {1u, 2u}) {
        Partita p(file, seme);
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : p.classi) pescaggi[id] = c.pescaggio_m;
        p.m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(std::string(M688_DATI) + "/teatri/" + p.m.teatro() + ".fdl")),
                            pescaggi);
        p.avanza(1800.0);
        CAPTURE(seme);
        REQUIRE(p.m.errori().empty());
        REQUIRE(p.m.controllato_asw(6));
        CHECK(p.m.controllore_asw(6)->mezzo()->id == "sh60b");
        CHECK_FALSE(p.m.aereo_ostile_al_giocatore(6));   // il LAMPS amico (il flag urss dell'originale non conta)
        CHECK(p.m.boe().size() >= 1);
        CHECK(p.m.lanci_aerei().empty());
        CHECK(p.m.voli_asw().at(0).quota_m > 20.0);
    }
}

TEST_CASE("asw missione: gli aerei previsti per il briefing prima del primo passo, la difficolta' scelta") {
    const std::string file = std::string(M688_MISSIONI) + "/SCENE1A.lua";
    if (!fs::exists(file)) return;
    Missione m(file, 1);
    const auto v = m.aerei_asw_previsti(M688_DATI);
    REQUIRE(v.size() == 1);
    CHECK(v[0].unita == 5);
    CHECK_FALSE(v[0].classe.empty());
    CHECK_FALSE(v[0].parte.empty());
    CHECK(v[0].missione == "caccia");
    REQUIRE(v[0].zona);
    CHECK(v[0].raggio_zona_m == doctest::Approx(31000.0));
    CHECK_FALSE(m.con_asw());   // l'ASW non e' partito
    // la difficolta' dell'app al posto della normale
    Partita p(scena("difficolta", "p3c", CUE_SOSUS), 3);
    p.m.imposta_difficolta_asw(DifficoltaAsw::facile);
    p.avanza(10.0);
    REQUIRE(p.m.controllore_asw(1));
    CHECK(p.m.controllore_asw(1)->configurazione().difficolta == DifficoltaAsw::facile);
}

TEST_CASE("asw missione: l'Alfa dell'IA evade la caccia del P-3C, e senza evasione no") {
    Partita con(scena("evasione", "p3c", CUE_SOSUS), 3);
    con.avanza(3600.0);
    CHECK(con.m.manovre_evasione(2) > 0);
    CHECK(con.m.manovre_evasione(0) == 0);   // il giocatore manovra da se'
    auto senza = std::make_unique<Partita>(scena("senza_evasione", "p3c", CUE_SOSUS), 3);
    senza->m.usa_evasione(false);
    senza->avanza(3600.0);
    CHECK(senza->m.manovre_evasione(2) == 0);
}

TEST_CASE("asw missione: l'ala fissa non scende sotto la velocita' d'attesa, nemmeno posando il box DICASS") {
    Partita p(scena("attesa", "p3c", CUE_SOSUS), 3);
    p.avanza(300.0);   // decollo da 100 kn: prima accelera
    double minima = 1e9;
    for (int i = 0; i < 3 * 3600; ++i) {
        p.avanza(1.0);
        if (const auto it = p.m.unita().find(1); it != p.m.unita().end() && !it->second.distrutta && !it->second.persa)
            minima = std::min(minima, it->second.velocita);
    }
    CAPTURE(minima);
    const DatiMezzo* p3c = p.m.controllore_asw(1)->mezzo();
    CHECK(minima >= 0.95 * std::min(p3c->velocita_ricerca_kn, p3c->mad->velocita_kn));
    CHECK(minima >= 180.0);
}

TEST_CASE("asw missione: il Mk 46 aviolanciato pinga nella scena acustica del giocatore (R7)") {
    Partita p(scena("ping_testa", "p3c", CUE_SOSUS), 3);
    double t = 0.0;
    while (p.m.lanci_aerei().empty() && t < 3.0 * 3600.0) { p.avanza(10.0); t += 10.0; }
    REQUIRE_FALSE(p.m.lanci_aerei().empty());
    bool ping = false;
    for (int i = 0; i < 120 && !ping; ++i) {
        p.avanza(1.0);
        for (const auto& s : p.m.scena_acustica(0, "bqq5_sfera").sorgenti) ping = ping || (s.id >= 960000 && s.id <= 979999);
    }
    CHECK(ping);
    Missione classica(scena("ping_classico", "p3c", CUE_SOSUS), 3);
    CHECK(classica.scena_acustica(0, "bqq5_sfera").sorgenti.empty());
}

// l'intercettazione (blocco 2 del sonar, E): la caccia del P-3C all'Alfa (2) arriva al suo intercettatore, il Rubin. Il
// Rubin misura solo frequenza e rilevamento (dossier 28 §2), ma la cadenza fitta della testa in acquisizione gli basta
// per l'allarme
TEST_CASE("asw missione: l'intercettatore dell'Alfa sente la caccia del P-3C e la testa del Mk 46") {
    Partita p(scena("intercettazione", "p3c", CUE_SOSUS), 3);
    double t = 0.0;
    while (p.m.lanci_aerei().empty() && t < 3.0 * 3600.0) { p.avanza(10.0); t += 10.0; }
    REQUIRE_FALSE(p.m.lanci_aerei().empty());
    int allarme = 0;
    for (int i = 0; i < 180; ++i) {
        p.avanza(1.0);
        if (const auto* r = p.m.intercettatore(2)) allarme = std::max(allarme, static_cast<int>(r->allarme()));
    }
    const auto* r = p.m.intercettatore(2);
    REQUIRE(r);
    MESSAGE(r->dati().id, ": ", r->tracce().size(), " tracce, ", r->storia().size(), " intercetti; allarme massimo ",
            std::string(nome_allarme(static_cast<Allarme>(allarme))));
    CHECK_FALSE(r->storia().empty());
    CHECK(allarme > 0);   // qui la cadenza fitta della testa basta anche al Rubin
}

TEST_CASE("asw missione: col Mk 46 in coda l'Alfa lancia un'MG-74 semovente sulla rotta che lascia") {
    Partita p(scena("mg74", "p3c", CUE_SOSUS), 3);
    double t = 0.0;
    while (p.m.lanci_aerei().empty() && t < 3.0 * 3600.0) { p.avanza(10.0); t += 10.0; }
    REQUIRE_FALSE(p.m.lanci_aerei().empty());
    int esca = -1;
    for (int i = 0; i < 300 && esca < 0; ++i) {
        p.avanza(1.0);
        for (const auto& [id, u] : p.m.unita())
            if (p.m.esca_mobile(id) && u.lanciatore == 2) esca = id;
    }
    REQUIRE(esca >= 0);
    const DatiEsca& d = *p.m.esca_mobile(esca);
    CHECK(d.classe == "mg74");
    const double rotta0 = p.m.unita().at(esca).rotta;
    p.avanza(60.0);
    const UnitaClassica& e = p.m.unita().at(esca);
    CHECK(e.velocita == doctest::Approx(d.velocita_kn).epsilon(0.05));   // Doppler vero: la verifica non la smaschera
    CHECK(e.quota <= d.quota_max_m + 1.0);
    CHECK((e.flag & 0x80) != 0);
    CHECK(std::fabs(normalizza_180(e.rotta - rotta0)) < 1.0);   // dritta fino alla prima virata programmata
    // suona come l'Alfa (imita il rumore), resta un falso bersaglio
    const Sorgente* s = nullptr;
    const auto scena_g = p.m.scena_acustica(0, "bqq5_sfera");
    for (const auto& x : scena_g.sorgenti) if (x.id == esca) s = &x;
    REQUIRE(s != nullptr);
    CHECK(s->esca);
    CHECK(s->firma == p.m.catalogo_acustico()->firma("alfa"));
    // la batteria finisce: esce dal gioco
    p.avanza(d.durata_s);
    CHECK(p.m.esca_mobile(esca) == nullptr);
    CHECK((p.m.unita().count(esca) == 0 || p.m.unita().at(esca).persa));
    // senza i siluri dell'Enhanced nessuna esca semovente; l'ADC Mk 2 non lo e'
    Missione classica(scena("mg74_classico", "p3c", CUE_SOSUS), 3);
    CHECK(classica.lancia_esca_mobile(2, "mg74", 0.0) == -1);
    CHECK(p.m.lancia_esca_mobile(0, "adc_mk2", 0.0) == -1);
    CHECK(p.m.lancia_esca_mobile(0, "moss_mk70", 90.0) >= 0);   // il what-if del giocatore
}

TEST_CASE("asw missione: l'SH-2F posa le SSQ-47B, che pingano ognuna ogni 10 s") {
    // l'SH-2F (LAMPS I: 9 SSQ-41B e 5 SSQ-47B) caccia l'Alfa; i ping delle boe nelle emissioni della R5 (i messaggi 386
    // al giocatore no: un avviso al minuto)
    Partita p(scena("ssq47b", "sh2f", CUE_SOSUS), 3);
    p.m.registra_statistiche(true);
    const auto attive = [&] {
        int n = 0;
        for (const auto& b : p.m.boe()) n += b.tipo == "ssq47b" && boa_in_ascolto(b, p.m.tempo());
        return n;
    };
    double t = 0.0;
    while (attive() < 2 && t < 3.0 * 3600.0) { p.avanza(10.0); t += 10.0; }
    REQUIRE(attive() >= 2);
    const int n0 = attive();
    const double da = p.m.tempo();
    p.avanza(60.0);
    int ping = 0;
    for (const auto& e : p.m.emissioni()) ping += e.t > da && e.unita == 1 && e.dettaglio == "ssq47b";
    MESSAGE(n0, " SSQ-47B in acqua a ", t, " s: ", ping, " ping in 60 s");
    CHECK(ping >= 5 * n0);   // 6 per boa al minuto; una sola per aereo ne dava al piu' 6-7
    CHECK(ping > 7);
}

// i contatti dell'aereo nella sua lista (trailer-1, la principale, 31/10/2026): la traccia del controllore del P-3C
// entra come contatto sull'Alfa, con la posizione stimata e il sensore «boe»; la storia dei contatti la conta (le
// condizioni «unita' vede», «tenuto da»); confermata, e' classificata (la durata lunga)
TEST_CASE("asw missione: il contatto del P-3C dalle boe entra nella sua lista e nella storia dei contatti") {
    Partita p(scena("contatti_aereo", "p3c", CUE_SOSUS), 3);
    CHECK(p.m.contatto_cumulato_s(1, 2) == 0.0);   // da qui la storia conta anche il P-3C
    bool visto = false, confermato = false;
    for (int k = 0; k < 3 * 360 && !(visto && confermato); ++k) {
        p.avanza(10.0);
        const auto& u = p.m.unita().at(1);
        const auto c = u.contatti.find(2);
        if (c == u.contatti.end()) continue;
        visto = visto || (c->second.posizione_nota && c->second.sensore == "boe");
        confermato = confermato || c->second.durata >= 1400.0;
    }
    MESSAGE("P-3C sull'Alfa: contatto ", visto, ", confermato ", confermato, ", storia ", p.m.contatto_cumulato_s(1, 2), " s a ", p.m.tempo(), " s");
    CHECK(visto);
    CHECK(confermato);
    CHECK(p.m.contatto_cumulato_s(1, 2) > 0.0);
}

// la simmetria (disassemblatore, 31/10/2026): il battello dell'IA sente tonfi e ping delle boe col modello acustico,
// coi suoi sensori, lo strato e il rumore proprio, come il giocatore, e ne ha un contatto sull'aereo; non serve
// l'evasione accesa, ne' che l'aereo lo stia cacciando
TEST_CASE("asw missione: l'Alfa dell'IA sente le boe del P-3C col modello acustico e ne ha il contatto, anche senza evasione") {
    // in pace: l'aereo caccia senza sparare, e l'Alfa resta in mare per tutta la prova
    const std::string pace = "{ { unita = 1, situazione = 'pace', zona = { x = 0, y = 0, raggio_m = 20000 },"
                             " cue = { tipo = 'sosus', x = 0, y = 2000, raggio_m = 3000, ritardo_s = 300 } } }";
    Partita p(scena("sente_boe", "p3c", pace), 3);
    p.m.usa_evasione(false);
    double quando = -1.0;
    // il contatto dura quanto il transitorio: senza segnale perde qualita' come ogni contatto passivo, in pochi
    // secondi; per questo lo si guarda a ogni passo
    for (int k = 0; k < 3600 * 9 && quando < 0.0; ++k) {
        p.avanza(1.0 / 9.0);
        const auto a = p.m.unita().find(2);
        REQUIRE(a != p.m.unita().end());
        if (const auto c = a->second.contatti.find(1); c != a->second.contatti.end() && c->second.sensore == "transitorio") quando = p.m.tempo();
    }
    MESSAGE("contatto dell'Alfa sul P-3C dai transitori delle boe a ", quando, " s");
    CHECK(quando > 0.0);
    CHECK(p.m.manovre_evasione(2) == 0);
}

// in pace nessuno tira senza un ordine dello scenario, e un neutrale mai (la principale, 31/10/2026, dal giro della S23):
// (a) un P-3C senza la voce aereo_asw in una missione di pace segue la situazione della missione; (b) un P-3C neutrale,
// anche con una voce di guerra, non tira. Tre ore sull'Alfa rumoroso: nessun lancio
TEST_CASE("asw missione: in pace un aereo senza voce non tira, e un aereo neutrale mai") {
    const auto lanci = [](const std::string& nome, int flag, const std::string& asw, const std::string& carriera) {
        const auto f = fs::temp_directory_path() / ("m688_prova_asw_" + nome + ".lua");
        const std::string sub = "modello = { massima_kn = 40, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 12, sonar = 19 }";
        const std::string volo = "modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 }";
        std::ofstream(f) << "return { id = 'PROVA_ASW_" << nome << "', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa',\n" << carriera
                         << (asw.empty() ? std::string() : "aereo_asw = " + asw + ",\n") << "script = {},\n"
                         << "varianti = { { probabilita = 1, unita = {\n"
                         << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -60000, y = -60000 }, quota_m = 60, rotta = 0, velocita_kn = 0, " << sub << " },\n"
                         << "{ id = 1, classe = 'p3c', categoria = 'aereo', flag = " << flag << ", posizione = { x = 0, y = -20000 },"
                            "  quota_m = 0, rotta = 0, velocita_kn = 100, " << volo << " },\n"
                         << "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 },"
                            "  quota_m = 100, rotta = 0, velocita_kn = 20, " << sub << ", rotta_punti = { { x = 0, y = 400000 } } },\n"
                         << "} } } }\n";
        Partita p(f.string(), 3);
        p.avanza(3.0 * 3600.0);
        fs::remove(f);
        return p.m.lanci_aerei().size();
    };
    const std::string pace = "carriera = { situazione = 'pace' },\n";
    // il riscontro: lo stesso P-3C americano con la voce di guerra tira
    CHECK(lanci("guerra_voce", 11, CUE_SOSUS, pace) > 0);
    CHECK(lanci("pace_senza_voce", 11, "", pace) == 0);          // (a)
    CHECK(lanci("neutrale_guerra", 3, CUE_SOSUS, pace) == 0);   // (b)
}

// il radar dell'aereo lo tiene spento il controllore (ComandoVolo::radar): col sonar calato, in attacco, sulla via di casa non
// emette, quindi nessun ESM lo sente e non scopre nessuno; quando lo vuole acceso decide l'EMCON. Oggi il comando non
// lo leggeva nessuno
TEST_CASE("asw missione: col radar spento dal controllore l'elicottero non emette") {
    // di giorno: il Ka-27 di notte non cala il sonar
    Partita p(scena("radar_hover", "ka27", std::string(CUE_SOSUS) + ", data = '1989-06-15', ora_utc = 12"), 3);
    p.m.registra_statistiche(true);
    const auto acceso = [&] {
        return std::any_of(p.m.emissioni().begin(), p.m.emissioni().end(), [](const auto& e) { return e.unita == 1 && e.tipo == "radar" && e.durata_s < 0.0; });
    };
    int spenti = 0, hover = 0, accesi = 0;
    bool spento_prima = false;
    for (int k = 0; k < 720; ++k) {
        p.avanza(10.0);
        const auto voli = p.m.voli_asw();
        if (voli.empty()) break;
        const bool spento = !voli[0].comando.radar;
        if (spento && spento_prima) {   // un passo di margine: la voce si chiude all'elettronica dopo
            ++spenti;
            hover += voli[0].comando.hover;
            CHECK_FALSE(acceso());
        }
        accesi += acceso();
        spento_prima = spento;
    }
    MESSAGE("radar spento dal controllore in ", spenti, " campioni (", hover, " in hover), acceso in ", accesi);
    CHECK(spenti > 0);
    CHECK(hover > 0);
    CHECK(accesi > 0);
}

// il radar resta acceso su un bersaglio visto (dossier 11 §6.2): l'aereo che ha il battello in superficie al radar non lo
// spegne passando al contatto o all'attacco. Il Viking col radar continuo dopo l'albero: T9 dice 60 s, 0,998
TEST_CASE("asw missione: sul battello in superficie visto il controllore non spegne il radar") {
    // il giocatore fuori scena oltre la griglia dei raggi (300 km): da qui sente l'Alfa, e sposta il percorso della prova
    Partita p(scena("radar_superficie", "p3c", CUE_SOSUS, "", "", -350000.0), 3);
    p.m.registra_statistiche(true);
    const auto acceso = [&] {
        return std::any_of(p.m.emissioni().begin(), p.m.emissioni().end(), [](const auto& e) { return e.unita == 1 && e.tipo == "radar" && e.durata_s < 0.0; });
    };
    double visto = -1e18;
    int comando_spento = 0, emette = 0;
    for (int k = 0; k < 720; ++k) {
        auto* alfa = p.m.trova(2);
        if (!alfa) break;
        alfa->quota = alfa->quota_ordinata = 0.0;
        p.avanza(10.0);
        const auto voli = p.m.voli_asw();
        if (voli.empty()) break;
        const auto* aereo = p.m.trova(1);
        if (!aereo) break;
        if (const auto c = aereo->contatti.find(2); c != aereo->contatti.end() && (c->second.sensore == "radar" || c->second.sensore == "vista"))
            visto = std::max(visto, c->second.ultima_scoperta);
        if (!voli[0].comando.radar && !voli[0].comando.hover && p.m.tempo() - visto < 600.0) {
            ++comando_spento;
            emette += acceso();
        }
    }
    MESSAGE("col comando spento e il bersaglio visto: ", comando_spento, " campioni, radar acceso in ", emette);
    CHECK(comando_spento > 0);
    CHECK(emette > 0);
}
