#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "../app/stima_mate.hpp"
#include "missioni/achievement_missione.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il libretto di riconoscimento al periscopio nella missione dell'Enhanced (docs/25 §3.2, §3.4, §3.5;
// missione_libretto.cpp): la classe dichiarata a vista dal giocatore, l'AOB, l'identificazione visiva dell'IA.

namespace {

const std::string NAVE = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 14, sonar = 0 }";
const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 (quota_688) e un Krivak a distanza_m verso nord che va a 090, in pace; un mercantile e un Victor lontani danno
// le altre classi con cui l'IA puo' confondersi
std::string scena(double quota_688, double distanza_m, int ora_utc = 12, const std::string& chiavi = "") {
    const auto f = fs::temp_directory_path() / "m688_prova_libretto.lua";
    std::ofstream(f) << "return { id = 'PROVA_LIBRETTO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = " << ora_utc
                     << ", paesaggio_sonoro = false, script = {}," << chiavi << " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = " << quota_688 << ", rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << distanza_m << " }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 10, " << NAVE << " },\n"
                        "{ id = 5, classe = 'cargo', categoria = 'nave', flag = 2, posizione = { x = 60000, y = 60000 }, quota_m = 0, rotta = 90,"
                        "  velocita_kn = 0, " << NAVE << " },\n"
                        "{ id = 6, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = -60000, y = 60000 }, quota_m = 150,"
                        "  rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, std::uint64_t seme = 1, bool enhanced = true)
        : m(file, seme, false), dm(classi, M688_DATI, seme, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    bool ordina(const Ordine& o) { return m.applica(o); }
};

bool c_e() { return fs::exists(M688_DATI "/periscopi.toml"); }

}  // namespace

TEST_CASE("libretto nella missione: la classe dichiarata a vista va nel contatto e nel registro, anche sbagliata" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0);
    Partita p(f);
    for (int i = 0; i < 60 && !p.m.giocatore()->contatti.count(2); ++i) p.avanza(5.0);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    CHECK_FALSE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 99, "cargo")));   // nessun contatto
    REQUIRE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 2, "cargo")));        // il Krivak preso per un mercantile
    const auto* v = p.m.identificazione_visiva(0, 2);
    REQUIRE(v);
    CHECK(v->classe == "cargo");
    CHECK_FALSE(v->giusta);
    p.avanza(1.0);
    const auto& ev = p.m.eventi_missione();
    const auto e = std::find_if(ev.begin(), ev.end(), [](const EventoMissione& x) { return x.tipo == TipoEvento::classificazione && x.visiva; });
    REQUIRE(e != ev.end());
    CHECK(e->classe_creduta == "cargo");
    CHECK_FALSE(e->giusta);
    // poi giusta: l'ultima vale
    REQUIRE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 2, "krivak2")));
    CHECK(p.m.identificazione_visiva(0, 2)->giusta);
    fs::remove(f);
}

TEST_CASE("libretto nella missione: l'AOB da' la rotta del bersaglio, rilevamento + 180 - AOB" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0);
    Partita p(f);
    for (int i = 0; i < 60 && !p.m.giocatore()->contatti.count(2); ++i) p.avanza(5.0);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    CHECK_FALSE(p.ordina({p.m.tempo(), TipoOrdine::osserva_aob, 200.0, 10.0, 2}));   // fuori da +-180
    REQUIRE(p.ordina({p.m.tempo(), TipoOrdine::osserva_aob, 90.0, 10.0, 2}));         // 90 a dritta: la vede di fianco
    REQUIRE(p.m.osservazioni_aob().size() == 1);
    const auto& o = p.m.osservazioni_aob().back();
    MESSAGE("rilevamento ", o.rilevamento, ", rotta stimata ", o.rotta, " (vera 90)");
    CHECK(o.rotta == doctest::Approx(normalizza_360(o.rilevamento + 90.0)));
    CHECK(std::fabs(normalizza_180(o.rotta - 90.0)) < 10.0);   // il Krivak a nord che va a 090 mostra la dritta
    CHECK(o.incertezza_gradi == doctest::Approx(10.0));
    // e alla TMA dell'equipaggio, con la sua incertezza (nucleo/tma.hpp, OsservazioneRotta)
    const Tma* t = p.m.tma_equipaggio(2);
    REQUIRE(t);
    REQUIRE(t->rotte().size() == 1);
    CHECK(t->rotte().back().rotta == doctest::Approx(o.rotta));
    CHECK(t->rotte().back().sigma_gradi == doctest::Approx(10.0));
    fs::remove(f);
}

TEST_CASE("libretto nella missione: nel classico gli ordini del libretto non valgono" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0);
    Partita p(f, 1, false);
    p.avanza(60.0);
    CHECK_FALSE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 2, "krivak2")));
    CHECK_FALSE(p.ordina({p.m.tempo(), TipoOrdine::osserva_aob, 30.0, 10.0, 2}));
    fs::remove(f);
}

// l'IA a vista: il Krivak che vede il 688 in superficie lo identifica; vicino di giorno quasi sempre giusto, lontano
// molto meno (e si confonde col Victor, l'altra classe di sottomarino in mare)
TEST_CASE("libretto nella missione: l'identificazione visiva dell'IA sbaglia di piu' da lontano" * doctest::skip(!c_e())) {
    const auto giuste = [](double distanza_m) {
        const std::string f = scena(0.0, distanza_m);
        int viste = 0, buone = 0;
        for (std::uint64_t seme = 1; seme <= 24; ++seme) {
            Partita p(f, seme);
            p.avanza(240.0);
            const auto* v = p.m.identificazione_visiva(2, 0);
            if (!v) continue;
            ++viste;
            if (v->giusta) ++buone;
            const auto& c = p.m.classificazioni_ia();
            CHECK(std::any_of(c.begin(), c.end(), [](const Missione::ClassificazioneIa& x) { return x.visiva && x.unita == 2 && x.sorgente == 0; }));
        }
        fs::remove(f);
        return std::pair<int, int>{viste, buone};
    };
    const auto vicino = giuste(2500.0), lontano = giuste(14000.0);
    MESSAGE("a 2,5 km: ", vicino.second, " giuste su ", vicino.first, "; a 14 km: ", lontano.second, " su ", lontano.first);
    REQUIRE(vicino.first >= 20);
    REQUIRE(lontano.first >= 10);
    CHECK(static_cast<double>(vicino.second) / vicino.first > 0.6);
    CHECK(static_cast<double>(lontano.second) / lontano.first < static_cast<double>(vicino.second) / vicino.first);
}

TEST_CASE("libretto nella missione: la lettura del telemetro va nel registro con la distanza vera, non ai messaggi" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0);
    Partita p(f);
    for (int i = 0; i < 60 && !p.m.giocatore()->contatti.count(2); ++i) p.avanza(5.0);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    const std::size_t messaggi_prima = p.m.messaggi().size();
    CHECK_FALSE(p.ordina(ordine_misura_telemetro(p.m.tempo(), 2, 0.0, 30.0)));   // senza lettura
    CHECK_FALSE(p.ordina(ordine_misura_telemetro(p.m.tempo(), 99, 4000.0, 30.0)));
    REQUIRE(p.ordina(ordine_misura_telemetro(p.m.tempo(), 2, 4600.0, 31.0)));
    REQUIRE(p.m.misure_telemetro().size() == 1);
    const auto& x = p.m.misure_telemetro().back();
    CHECK(x.letta_m == doctest::Approx(4600.0));
    CHECK(x.altezza_m == doctest::Approx(31.0));
    CHECK(x.vera_m == doctest::Approx(distanza(p.m.giocatore()->pos, p.m.unita().at(2).pos)).epsilon(0.01));
    p.avanza(1.0);
    CHECK(p.m.messaggi().size() == messaggi_prima);   // l'equipaggio non dice la vera
    const auto& ev = p.m.eventi_missione();
    const auto e = std::find_if(ev.begin(), ev.end(), [](const EventoMissione& y) { return y.tipo == TipoEvento::misura_telemetro; });
    REQUIRE(e != ev.end());
    CHECK(e->unita == 2);
    CHECK(e->valore == doctest::Approx(4600.0));
    CHECK(e->misura_m == doctest::Approx(31.0));
    MESSAGE(e->testo_it);
    CHECK(e->testo_it.find("albero 31 m") != std::string::npos);
    fs::remove(f);
    // nel classico no
    const std::string g = scena(18.0, 5000.0);
    Partita c(g, 1, false);
    c.avanza(60.0);
    CHECK_FALSE(c.ordina(ordine_misura_telemetro(c.m.tempo(), 2, 4600.0, 31.0)));
    fs::remove(g);
}

// gli obiettivi delle missioni di riconoscimento (LIB-I13 e I14 di trailer-1): identificare, telemetro, rotta_stimata
// rotta_stimata con da_tma (LIB-S20): le stime della TMA, anche sulla stessa unita'; contano quelle entro l'errore e ad
// almeno 300 s l'una dall'altra. Senza da_tma le stime non contano, e l'AOB non conta con da_tma
TEST_CASE("libretto nella missione: rotta_stimata dalla TMA, tre rotte dello stesso bersaglio valgono tre" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0, 12,
        " obiettivi = { { id = 'tma', tipo = 'rotta_stimata', unita = { 2 }, minimo = 3, errore = 20, da_tma = true },"
        " { id = 'aob', tipo = 'rotta_stimata', unita = { 2 }, minimo = 1, errore = 20 } },");
    Partita p(f);
    for (int i = 0; i < 60 && !p.m.giocatore()->contatti.count(2); ++i) p.avanza(5.0);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    const auto voce = [&](const std::string& id) {
        for (const auto& o : p.m.esito_dettagliato().obiettivi)
            if (o.id == id) return o;
        return Obiettivo{};
    };
    const double vera = p.m.unita().at(2).rotta;
    p.m.stima_tma_giocatore(2, vera + 40.0);   // oltre i 20 gradi: non conta
    CHECK(voce("tma").fatti == 0);
    p.m.stima_tma_giocatore(2, vera + 15.0);   // la prima
    CHECK(voce("tma").fatti == 1);
    CHECK(voce("tma").stato == Obiettivo::Stato::in_corso);
    p.avanza(60.0);
    p.m.stima_tma_giocatore(2, p.m.unita().at(2).rotta);   // troppo vicina alla prima
    CHECK(voce("tma").fatti == 1);
    p.avanza(250.0);
    p.m.stima_tma_giocatore(2, p.m.unita().at(2).rotta - 10.0);   // la seconda: 310 s dalla prima
    CHECK(voce("tma").fatti == 2);
    p.avanza(310.0);
    p.m.stima_tma_giocatore(2, p.m.unita().at(2).rotta);   // la terza, dello stesso bersaglio
    CHECK(voce("tma").fatti == 3);
    CHECK(voce("tma").stato == Obiettivo::Stato::raggiunto);
    CHECK(voce("aob").fatti == 0);   // le stime della TMA non sono AOB
    CHECK(voce("aob").stato == Obiettivo::Stato::in_corso);
    // una stima su un'unita' che non c'e' non si registra
    const auto n = p.m.stime_tma_giocatore().size();
    p.m.stima_tma_giocatore(999, 10.0);
    CHECK(p.m.stime_tma_giocatore().size() == n);
}

// quando la soluzione del MATE vale come stima: solo se i rilevamenti ne hanno una e il MATE li spiega altrettanto bene
TEST_CASE("libretto nella missione: la stima dal MATE vale solo se la TMA ha una soluzione e il MATE la spiega") {
    Tma tma;
    const SoluzioneTma qualunque{5000.0, 45.0, 5.0};
    CHECK_FALSE(stima_mate_valida(tma, qualunque, 0.0));   // senza rilevamenti
    // come nella S20: il bersaglio a 3 km per 045, a 090 e 5 nodi; il battello a 5 m/s, gambe di 300 s con accostate di 90 gradi
    const Vec2 b0{3000.0 * std::sin(radianti(45.0)), 3000.0 * std::cos(radianti(45.0))};
    const auto bersaglio = [&](double t) { return Vec2{b0.x + 2.57 * t, b0.y}; };
    const auto proprio = [](double t) {
        if (t < 300.0) return Vec2{0.0, 5.0 * t};
        if (t < 600.0) return Vec2{5.0 * (t - 300.0), 1500.0};
        return Vec2{1500.0, 1500.0 + 5.0 * (t - 600.0)};
    };
    const auto aggiungi = [&](double da, double a) {
        for (double t = da; t <= a; t += 5.0) tma.aggiungi({t, proprio(t), normalizza_360(rilevamento(proprio(t), bersaglio(t)))});
    };
    aggiungi(0.0, 60.0);
    CHECK_FALSE(stima_mate_valida(tma, qualunque, 60.0));  // pochi rilevamenti, arco troppo breve
    aggiungi(65.0, 900.0);
    const auto migliore = risolvi_tma(tma, 900.0, SIGMA_TMA_MATE_DEG);
    REQUIRE(migliore);
    REQUIRE(migliore->osservabile);
    CHECK(std::fabs(normalizza_180(migliore->soluzione.rotta - 90.0)) < 20.0);   // la rotta giusta, dai soli rilevamenti
    CHECK(stima_mate_valida(tma, migliore->soluzione, 900.0));   // la soluzione giusta nel MATE
    SoluzioneTma storta = migliore->soluzione;
    storta.rotta = normalizza_360(storta.rotta + 90.0);
    storta.distanza_m *= 2.0;
    CHECK_FALSE(stima_mate_valida(tma, storta, 900.0));          // una soluzione che non spiega i rilevamenti
}

TEST_CASE("libretto nella missione: gli obiettivi identificare, telemetro e rotta_stimata" * doctest::skip(!c_e())) {
    const std::string f = scena(18.0, 5000.0, 12,
        " obiettivi = { { id = 'identifica', tipo = 'identificare', unita = { 2 }, minimo = 1 },"
        " { id = 'telemetro', tipo = 'telemetro', minimo = 1, errore = 0.1 },"
        " { id = 'aob', tipo = 'rotta_stimata', minimo = 1, errore = 20 } },");
    Partita p(f);
    for (int i = 0; i < 60 && !p.m.giocatore()->contatti.count(2); ++i) p.avanza(5.0);
    REQUIRE(p.m.giocatore()->contatti.count(2));
    const auto stato = [&](const std::string& id) {
        for (const auto& o : p.m.esito_dettagliato().obiettivi)
            if (o.id == id) return o.stato;
        return Obiettivo::Stato::fallito;
    };
    CHECK(stato("identifica") == Obiettivo::Stato::in_corso);
    // sbagliata, poi giusta: conta l'ultima
    REQUIRE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 2, "cargo")));
    p.avanza(1.0);
    CHECK(stato("identifica") == Obiettivo::Stato::in_corso);
    REQUIRE(p.ordina(ordine_identifica_visiva(p.m.tempo(), 2, "krivak2")));
    p.avanza(1.0);
    CHECK(stato("identifica") == Obiettivo::Stato::raggiunto);
    // il telemetro: 20% fuori, poi 4% dentro
    const double vera = distanza(p.m.giocatore()->pos, p.m.unita().at(2).pos);
    REQUIRE(p.ordina(ordine_misura_telemetro(p.m.tempo(), 2, vera * 1.2, 31.0)));
    p.avanza(1.0);
    CHECK(stato("telemetro") == Obiettivo::Stato::in_corso);
    REQUIRE(p.ordina(ordine_misura_telemetro(p.m.tempo(), 2, vera * 0.96, 31.0)));
    p.avanza(1.0);
    CHECK(stato("telemetro") == Obiettivo::Stato::raggiunto);
    // l'AOB: il Krivak a nord che va a 090 mostra la dritta (AOB +90); +150 e' fuori di 60 gradi
    REQUIRE(p.ordina({p.m.tempo(), TipoOrdine::osserva_aob, -150.0, 10.0, 2}));
    p.avanza(1.0);
    CHECK(stato("aob") == Obiettivo::Stato::in_corso);
    REQUIRE(p.ordina({p.m.tempo(), TipoOrdine::osserva_aob, 90.0, 10.0, 2}));
    p.avanza(1.0);
    CHECK(stato("aob") == Obiettivo::Stato::raggiunto);
    // e le misure degli achievement di docs/26
    const Misure x = misure_achievement(p.m, ContestoMissione{});
    CHECK(x["visive_giuste"] == 1.0);
    CHECK(x["telemetro_entro_5"] == 1.0);   // il 4%, non il 20%
    CHECK(x["aob_entro_10"] == 1.0);
    CHECK(x["prima_la_vista"] == 1.0);      // nessuna classificazione del sonar prima
    CHECK(x["colpi_artiglieria_subiti"] == 0.0);
    CHECK(x["mare_max"] >= 0.0);
    fs::remove(f);
}

// I fanali di notte letti dall'IA (runtime, 29/10/2026; leggi_fanali in missione_luci.cpp): lo stato, il tipo quando lo
// dicono, l'aspetto dai laterali e dal coronamento; oltre la sagoma niente classe. A mezzanotte di dicembre alle Faroe
namespace {
std::string scena_fanali(const std::string& situazione, double krivak_m) {
    const auto f = fs::temp_directory_path() / "m688_prova_libretto_fanali.lua";
    std::ofstream(f) << "return { id = 'PROVA_FANALI', teatro = 'faroe', lato = 'usa', data = '1989-12-15', ora_utc = 0,"
                        " paesaggio_sonoro = false, script = {}, carriera = { situazione = '" << situazione << "', difficolta = 3 },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 0, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 0, y = 4000 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
                        "{ id = 3, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 4000, y = 0 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 0, " << NAVE << " },\n"
                        "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << -krivak_m << " }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 10, emcon = 'silenzio', " << NAVE << " },\n"
                        "{ id = 5, classe = 'peschereccio', categoria = 'nave', flag = 2, posizione = { x = -4000, y = 0 }, quota_m = 0,"
                        "  rotta = 90, velocita_kn = 3, " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}
bool c_luci() { return fs::exists(M688_DATI "/luci.toml") && c_e(); }
}  // namespace

TEST_CASE("libretto nella missione: i fanali di notte dicono lo stato, il tipo e l'aspetto, non la classe" * doctest::skip(!c_luci())) {
    const std::string f = scena_fanali("guerra", 3000.0);
    Partita p(f);
    p.m.luci_enhanced(M688_DATI);
    p.avanza(1.0);
    REQUIRE(p.m.luce_periscopi() < 0.2);
    Osservatore o;
    o.altezza_occhio_m = 20.0;
    o.allertato = true;
    // il mercantile in moto verso 090 da (0, 4 km): di prua rosso e verde, dal lato sinistro (nord) il rosso, di poppa il
    // coronamento
    const auto prua = p.m.leggi_fanali(2, {3000.0, 4000.0}, o);
    REQUIRE(prua.visti > 0);
    CHECK(prua.stato == std::optional<luci::Stato>(luci::Stato::moto));
    REQUIRE(prua.aspetto);
    CHECK(*prua.aspetto == doctest::Approx(0.0));
    CHECK(prua.sigma_aspetto == doctest::Approx(5.0));
    const auto sinistra = p.m.leggi_fanali(2, {0.0, 7000.0}, o);
    REQUIRE(sinistra.aspetto);
    CHECK(*sinistra.aspetto < 0.0);
    const auto poppa = p.m.leggi_fanali(2, {-3000.0, 4000.0}, o);
    REQUIRE(poppa.aspetto);
    CHECK(*poppa.aspetto == doctest::Approx(180.0));
    CHECK(prua.tipo.empty());   // un mercantile in moto: le luci non dicono che nave e'
    // il mercantile fermo alla fonda, il peschereccio in pesca
    CHECK(p.m.leggi_fanali(3, {4000.0, 3000.0}, o).stato == std::optional<luci::Stato>(luci::Stato::fonda));
    const auto pesca = p.m.leggi_fanali(5, {-4000.0, 3000.0}, o);
    CHECK(pesca.stato == std::optional<luci::Stato>(luci::Stato::pesca));
    CHECK(pesca.tipo == "peschereccio");
    // la Krivak in guerra a luci spente, e tutto oltre la portata dei fanali: niente
    CHECK(p.m.leggi_fanali(4, {0.0, -1000.0}, o).visti == 0);
    CHECK(p.m.leggi_fanali(2, {0.0, 40000.0}, o).visti == 0);
    fs::remove(f);
}

TEST_CASE("libretto nella missione: la Krivak vede di notte il faro ambra del 688 oltre la sagoma: sottomarino, classe no" * doctest::skip(!c_luci())) {
    // a 7 km: la sagoma del 688 emerso di notte arriva a circa 1,5 km, i fanali molto piu' in la' (prove_luci_missione)
    const std::string f = scena_fanali("crisi", 7000.0);
    Partita p(f);
    p.m.luci_enhanced(M688_DATI);
    REQUIRE(p.m.applica(Ordine{p.m.tempo(), TipoOrdine::luci_giocatore, 1.0}));
    p.avanza(1.0);
    REQUIRE(p.m.luce_periscopi() < 0.2);
    for (int i = 0; i < 60 && !p.m.fanali_visti(4, 0); ++i) p.avanza(5.0);
    const auto* l = p.m.fanali_visti(4, 0);
    REQUIRE(l);
    CHECK(l->lettura.tipo == "sottomarino");   // il faro ambra USN
    CHECK(l->lettura.stato == std::optional<luci::Stato>(luci::Stato::moto));
    CHECK(p.m.identificazione_visiva(4, 0) == nullptr);   // la classe no: e' oltre la sagoma
    fs::remove(f);
}

// la Luna nella vista notturna dell'IA (principale, 30/10/2026): con la data e il fondale georeferenziato il cielo e'
// quello di nucleo/sagoma_notte con la Luna vera. Alle Faroe a mezzanotte UTC: il 13/12/1989 la Luna e' quasi piena (il
// 12 alle 16:30) e alta, il 28/12/1989 e' nuova (alle 3:20) e sotto l'orizzonte. La sagoma del mercantile a 4 km si vede
// piu' lontano con la luna piena; senza fondale resta il cielo di prima, uguale nelle due notti
TEST_CASE("libretto nella missione: di notte la luna piena alta allunga la portata della sagoma, la nuova no" * doctest::skip(!c_e())) {
    const auto notte = [](const std::string& data, bool con_fondale) {
        const auto f = fs::temp_directory_path() / ("m688_prova_luna_" + data + ".lua");
        std::ofstream(f) << "return { id = 'PROVA_LUNA', teatro = 'faroe', lato = 'usa', data = '" << data << "', ora_utc = 0,"
                            " paesaggio_sonoro = false, script = {}, carriera = { situazione = 'pace', difficolta = 3 },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                            "  quota_m = 0, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 2, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 0, y = 4000 }, quota_m = 0,"
                            "  rotta = 90, velocita_kn = 8, " << NAVE << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        if (con_fondale) {
            std::map<std::string, double> pescaggi;
            for (const auto& [id, c] : p.classi) pescaggi[id] = c.pescaggio_m;
            p.m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/faroe.fdl")), pescaggi);
        }
        p.avanza(1.0);
        REQUIRE(p.m.luce_periscopi() < 0.2);
        Osservatore o;
        o.altezza_occhio_m = 20.0;
        o.allertato = true;
        const CieloNotte c = p.m.cielo_notte_in({0.0, 0.0});
        const double portata = p.m.portata_sagoma(2, {0.0, 0.0}, o);
        fs::remove(f);
        return std::tuple{c.luna_su, c.lux, portata};
    };
    const auto [su_piena, lux_piena, piena] = notte("1989-12-13", true);
    const auto [su_nuova, lux_nuova, nuova] = notte("1989-12-28", true);
    MESSAGE("luna piena: ", lux_piena, " lux, sagoma a ", piena, " m; luna nuova: ", lux_nuova, " lux, ", nuova, " m");
    CHECK(su_piena);
    CHECK_FALSE(su_nuova);
    CHECK(lux_piena > 20.0 * lux_nuova);
    CHECK(piena > 1.3 * nuova);
    // senza fondale georeferenziato: il cielo di prima, senza Luna, uguale nelle due notti
    const auto [su_a, lux_a, senza_a] = notte("1989-12-13", false);
    const auto [su_b, lux_b, senza_b] = notte("1989-12-28", false);
    CHECK_FALSE(su_a);
    CHECK(senza_a == doctest::Approx(senza_b));
}
