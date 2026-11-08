#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/elettronica.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;

// Emettitori radar, EMCON ed ESM dell'Enhanced (punto 23): dossier docs/ricerca/11_emettitori_radar_esm.md §6.

namespace {

constexpr double NM = 1852.0;

const CatalogoElettronico& catalogo() {
    static const CatalogoElettronico c = carica_elettronica(M688_DATI "/emettitori.toml");
    return c;
}

bool c_e(const std::vector<EventoEsm>& v, EventoEsm::Tipo t) {
    return std::any_of(v.begin(), v.end(), [&](const EventoEsm& e) { return e.tipo == t; });
}

}  // namespace

TEST_CASE("elettronica: catalogo, suite delle classi e ricevitori ESM") {
    const auto& c = catalogo();
    const auto perry = c.radar_di("perry");
    const auto ha = [&](const char* id) { return std::any_of(perry.begin(), perry.end(), [&](const Radar* r) { return r->id == id; }); };
    CHECK(ha("sps55"));
    CHECK(ha("sps49"));
    CHECK(ha("mk92_cas"));
    REQUIRE(c.esm_di("los_angeles") != nullptr);
    CHECK(c.esm_di("los_angeles")->id == "wlr8");
    CHECK(c.esm_di("alfa")->id == "bukhta");
    CHECK(c.esm_di("perry") == nullptr);
    REQUIRE(c.intermittenza_di("p3c") != nullptr);
    CHECK(c.trova_radar("aps115")->ruolo == RuoloRadar::asw_aereo);
}

TEST_CASE("elettronica: orizzonte radar e probabilita' per giro contro il periscopio (esempio del dossier §6.4)") {
    // aereo a 1000 ft contro un periscopio a 3 ft: 41 nm (dossier 02 §7.7)
    CHECK(orizzonte_radar_m(1000.0 * 0.3048, 3.0 * 0.3048) / NM == doctest::Approx(41.0).epsilon(0.02));
    const Radar& aps115 = *catalogo().trova_radar("aps115");
    CHECK(r50_m(aps115, Esposizione::periscopio, 2) / NM == doctest::Approx(6.0).epsilon(0.01));
    CHECK(r50_m(aps115, Esposizione::periscopio, 4) / NM == doctest::Approx(2.0).epsilon(0.01));
    CHECK(r50_m(aps115, Esposizione::periscopio, 6) == 0.0);
    CHECK(r50_m(aps115, Esposizione::snorkel, 2) > r50_m(aps115, Esposizione::periscopio, 2));
    // P-3C a 5 nm, mare 2, operatore allertato, 5 nodi: 0,7 / (1 + (5/6)^4) = 0,47
    const double p = probabilita_giro(aps115, 5.0 * NM, 1000.0 * 0.3048, Esposizione::periscopio, 2, 0.7, 5.0);
    CHECK(p == doctest::Approx(0.7 / (1.0 + std::pow(5.0 / 6.0, 4.0))).epsilon(0.02));
    // il pennacchio oltre 8 nodi raddoppia, fino a 0,95
    CHECK(probabilita_giro(aps115, 5.0 * NM, 304.8, Esposizione::periscopio, 2, 0.7, 10.0) == doctest::Approx(std::min(0.95, 2.0 * p)).epsilon(0.02));
    // oltre l'orizzonte niente: l'aereo a 45 nm, una nave col radar a 25 m e il periscopio a 15 nm (orizzonte 13,9)
    CHECK(probabilita_giro(aps115, 45.0 * NM, 304.8, Esposizione::periscopio, 2, 0.7, 5.0) == 0.0);
    const Radar& sps55 = *catalogo().trova_radar("sps55");
    CHECK(probabilita_giro(sps55, 15.0 * NM, 25.0, Esposizione::periscopio, 2, 0.7, 5.0) == 0.0);
    CHECK(probabilita_giro(sps55, 12.0 * NM, 25.0, Esposizione::periscopio, 2, 0.7, 5.0) < 0.05);
    // il battello emerso alla vela vera della classe (kit e principale, 31/10/2026): col radar a 33 m e i 6 m fissi
    // l'orizzonte e' a 33,7 km; la vela del Typhoon (14,3 m) lo porta a 39 km
    const double fisso = orizzonte_radar_m(33.0, 6.0), typhoon = orizzonte_radar_m(33.0, 14.3);
    CHECK(fisso < 36000.0);
    CHECK(typhoon > 36000.0);
    CHECK(probabilita_giro(sps55, 36000.0, 33.0, Esposizione::emerso, 2, 0.7, 5.0) == 0.0);
    CHECK(probabilita_giro(sps55, 36000.0, 33.0, Esposizione::emerso, 2, 0.7, 5.0, 0.0, 14.3) > 0.0);
}

TEST_CASE("elettronica: EMCON dell'IA, allarmi e ritorno; aerei intermittenti") {
    const auto& c = catalogo();
    const Radar& sps55 = *c.trova_radar("sps55");
    const Radar& sps49 = *c.trova_radar("sps49");
    const Radar& fc = *c.trova_radar("fc");
    CHECK(emcon_di_partenza("nave", false, false, false) == StatoEmcon::navigazione);
    CHECK(emcon_di_partenza("nave", true, false, false) == StatoEmcon::ricerca);
    CHECK(emcon_di_partenza("sottomarino", true, false, false) == StatoEmcon::silenzio);
    CHECK(emcon_di_partenza("nave", false, false, true) == StatoEmcon::navigazione);
    Emcon perry(StatoEmcon::navigazione, std::nullopt, 1);
    CHECK(perry.emette(sps55));
    CHECK_FALSE(perry.emette(sps49));
    perry.evento(EventoEmcon::scoperti, 100.0);
    CHECK(perry.stato() == StatoEmcon::ricerca);
    CHECK(perry.emette(sps49));
    CHECK_FALSE(perry.emette(fc));
    perry.evento(EventoEmcon::albero_visto, 200.0);
    CHECK(perry.emette(fc));                       // ingaggio: anche il tiro
    perry.avanza(200.0 + 1300.0);
    CHECK(perry.stato() == StatoEmcon::navigazione);   // 20 minuti senza allarmi
    // P-3C: brevi giri e lunghi silenzi; con l'albero visto, continuo
    Emcon p3c(StatoEmcon::intermittente, *c.intermittenza_di("p3c"), 7);
    const Radar& aps115 = *c.trova_radar("aps115");
    int acceso = 0, passi = 0;
    for (double t = 0.0; t < 7200.0; t += 1.0, ++passi) { p3c.avanza(t); acceso += p3c.emette(aps115); }
    CHECK(acceso > 0);
    CHECK(acceso < passi / 5);                     // acceso molto meno di un quinto del tempo
    p3c.evento(EventoEmcon::albero_visto, 7200.0);
    p3c.avanza(7201.0);
    CHECK(p3c.emette(aps115));
}

TEST_CASE("elettronica: l'ESM del 688 sente il P-3C ben prima di esserne visto, identifica e stima il rischio") {
    const auto& c = catalogo();
    const Radar& aps115 = *c.trova_radar("aps115");
    EsmBattello esm(c, *c.esm_di("los_angeles"), 3);
    // P-3C a 1000 ft, 30 nm a nord: radar acceso
    EmissioneVista p3c{7, &aps115, {0.0, 30.0 * NM}, 304.8, false, true};
    double t = 0.0;
    for (; t < 120.0; t += 1.0) esm.avanza(t, 1.0, {0.0, 0.0}, 10.0, true, Esposizione::periscopio, 2, {p3c});
    REQUIRE(esm.intercettazioni().size() == 1);
    const Intercettazione& i = esm.intercettazioni()[0];
    CHECK(i.unita == 7);
    CHECK(i.aereo);
    CHECK(i.passaggi >= 3);
    CHECK(std::fabs(normalizza_180(i.rilevamento - 0.0)) < 3.0 * i.errore_gradi + 1.0);
    CHECK(i.rischio == Rischio::sicuro);           // a 30 nm il radar non ci vede
    CHECK(i.distanza_stimata_m > 15.0 * NM);
    CHECK(i.distanza_stimata_m < 60.0 * NM);
    CHECK_FALSE(i.identita.empty());
    auto ev = esm.eventi();
    CHECK(c_e(ev, EventoEsm::Tipo::nuovo));
    CHECK(c_e(ev, EventoEsm::Tipo::aereo_asw));
    CHECK(esm.livello_originale() > 0);
    // si avvicina a 300 nodi: tendenza in avvicinamento, poi rischio alto quando e' vicino
    for (; t < 600.0; t += 1.0) {
        p3c.pos.y = std::max(2.0 * NM, 30.0 * NM - 300.0 * NODO * (t - 120.0));
        esm.avanza(t, 1.0, {0.0, 0.0}, 10.0, true, Esposizione::periscopio, 2, {p3c});
        if (t == 400.0) CHECK(esm.intercettazioni()[0].tendenza == Tendenza::avvicinamento);   // oltre 3 dB/min
    }
    CHECK(esm.intercettazioni()[0].rischio == Rischio::probabile);
    CHECK(c_e(esm.eventi(), EventoEsm::Tipo::rischio_alto));
    // albero giu': dopo un minuto senza passaggi l'emettitore e' perso
    for (; t < 700.0; t += 1.0) esm.avanza(t, 1.0, {0.0, 0.0}, 10.0, false, Esposizione::periscopio, 2, {p3c});
    CHECK(esm.intercettazioni().empty());
    CHECK(c_e(esm.eventi(), EventoEsm::Tipo::perso));
}

TEST_CASE("elettronica: oltre l'orizzonte niente intercetto; il radar di tiro puntato su di noi e' SCOPERTO") {
    const auto& c = catalogo();
    EsmBattello esm(c, *c.esm_di("los_angeles"), 5);
    EmissioneVista nave{3, c.trova_radar("sps55"), {0.0, 40.0 * NM}, 25.0, false, false};
    for (double t = 0.0; t < 120.0; t += 1.0) esm.avanza(t, 1.0, {0.0, 0.0}, 10.0, true, Esposizione::albero, 2, {nave});
    CHECK(esm.intercettazioni().empty());
    EmissioneVista tiro{3, c.trova_radar("fc"), {0.0, 8.0 * NM}, 20.0, true, false};
    for (double t = 120.0; t < 130.0; t += 1.0) esm.avanza(t, 1.0, {0.0, 0.0}, 10.0, true, Esposizione::albero, 2, {tiro});
    REQUIRE(esm.intercettazioni().size() == 1);
    CHECK(esm.intercettazioni()[0].rischio == Rischio::scoperto);
    CHECK(c_e(esm.eventi(), EventoEsm::Tipo::illuminato));
}

// --- nella missione (regola di simulazione) ---

#include <filesystem>
#include <fstream>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"

namespace {

namespace fs = std::filesystem;

// il 688 a quota periscopica con l'antenna alzata e una Krivak II (sovietica: EMCON in ricerca) a `distanza` m
std::string missione_esm(double distanza) {
    const auto f = fs::temp_directory_path() / "m688_prova_esm.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA_ESM', teatro = 'faroe', lato = 'usa',"
                        " script = { SU = function(u) m688.antenna(u, true) m688.salva_modello(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 3, " << modello << " },"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << distanza << " },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 0, " << modello << " },"
                        "\n}, timer = { { dopo_s = 0, unita = 0, gestore = 'SU' } } } } }\n";
    return f.string();
}

struct PartitaEsm {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit PartitaEsm(const std::string& f) : m(f, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (double t = 0; t < s; t += 1.0 / 9.0) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool messaggio_esm(const Missione& m, int codice) {
    for (const auto& x : m.messaggi_esm()) if (x.codice == codice) return true;
    return false;
}

}  // namespace

TEST_CASE("elettronica nella missione: a 30 km l'ESM sente la Krivak, che non vede l'albero; a 4 km il radar lo vede") {
    const auto f = missione_esm(30000.0);
    Missione classica(f, 1, false);
    classica.avanza(10.0);
    CHECK_FALSE(classica.con_elettronica());
    CHECK(classica.intercettazioni().empty());
    {
        PartitaEsm p(f);
        p.avanza(180.0);
        REQUIRE(p.m.con_elettronica());
        REQUIRE(p.m.emcon(2));
        CHECK(*p.m.emcon(2) == StatoEmcon::ricerca);   // gruppi sovietici: radar accesi
        REQUIRE_FALSE(p.m.intercettazioni().empty());
        CHECK(p.m.intercettazioni()[0].unita == 2);
        CHECK(messaggio_esm(p.m, 334));
        CHECK(p.m.livello_esm() > 0);
        REQUIRE(p.m.unita().at(0).contatti.count(2) == 1);
        CHECK_FALSE(p.m.unita().at(0).contatti.at(2).posizione_nota);   // solo il rilevamento
        const auto& k = p.m.unita().at(2).contatti;
        CHECK((k.count(0) == 0 || k.at(0).sensore != "radar"));          // oltre l'orizzonte del suo radar
    }
    const auto vicino = missione_esm(4000.0);
    PartitaEsm q(vicino);
    q.avanza(180.0);
    const auto& k = q.m.unita().at(2).contatti;
    REQUIRE(k.count(0) == 1);
    CHECK(k.at(0).posizione_nota);
    CHECK(*q.m.emcon(2) == StatoEmcon::ingaggio);
    CHECK(messaggio_esm(q.m, 336));   // rischio alto
    fs::remove(f);
}

TEST_CASE("elettronica nella missione: la chiave d'unita' emcon fissa l'EMCON di partenza (S13, il bersaglio coi radar accesi)") {
    // una Kara della parte americana in pace: la dottrina la tiene in navigazione; con emcon = 'ricerca' accende anche
    // la ricerca aerea (Top Sail, Head Net-C), che l'ESM del 688 sente
    const auto prova = [](const std::string& chiave) {
        const auto f = fs::temp_directory_path() / "m688_prova_emcon_chiave.lua";
        const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
        std::ofstream(f) << "return { id = 'PROVA_EMCON', teatro = 'faroe', lato = 'usa', carriera = { situazione = 'pace' },"
                            " script = { SU = function(u) m688.antenna(u, true) m688.salva_modello(u) end },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 3, " << modello << " },"
                            "{ id = 2, classe = 'kara', categoria = 'nave', flag = 10, posizione = { x = 0, y = 30000 },"
                            "  quota_m = 0, rotta = 90, velocita_kn = 0, " << chiave << modello << " },"
                            "\n}, timer = { { dopo_s = 0, unita = 0, gestore = 'SU' } } } } }\n";
        PartitaEsm p(f.string());
        p.avanza(60.0);
        const auto e = p.m.emcon(2);
        fs::remove(f);
        return e;
    };
    const auto senza = prova("");
    REQUIRE(senza);
    CHECK(*senza == StatoEmcon::navigazione);
    const auto con = prova("emcon = 'ricerca', ");
    REQUIRE(con);
    CHECK(*con == StatoEmcon::ricerca);
}
