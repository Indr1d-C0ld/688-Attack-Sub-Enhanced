#include <doctest.h>

#include <cmath>
#include <string>

#include "interfaccia.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/elettronica.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/onde.hpp"
#include "nucleo/periscopi.hpp"

using namespace m688;

// Periscopi dell'Enhanced (docs/14_periscopi.md, P1): dati, alzata ed esposizione, tremolio e danno con la velocita',
// portate visive per testa, mare e notte, sezione radar, guardate dell'IA

namespace {

const CatalogoPeriscopi& catalogo() {
    static const CatalogoPeriscopi c = carica_periscopi(M688_DATI "/periscopi.toml");
    return c;
}

constexpr double QUOTA_688 = 18.3;

}  // namespace

TEST_CASE("periscopi: il telemetro a reticolo dell'Alfa legge un angolo positivo fino a 5 gradi") {
    PostoPeriscopi p(catalogo(), "alfa");
    REQUIRE(p.dati() != nullptr);
    CHECK(p.dati()->telemetro == "reticolo");
    CHECK(p.dati()->telemetro_altezza_max_m == doctest::Approx(80.0));
    p.sdoppiamento(1.2);
    CHECK(p.stato().sdoppiamento_gradi == doctest::Approx(1.2));
    p.sdoppiamento(9.0);
    CHECK(p.stato().sdoppiamento_gradi == doctest::Approx(5.0));
    p.sdoppiamento(-1.0);
    CHECK(p.stato().sdoppiamento_gradi == 0.0);
}

TEST_CASE("periscopi: dati del 688 e delle classi") {
    const auto& c = catalogo();
    const auto v = c.di_classe("los_angeles");
    REQUIRE(v.size() == 2);
    CHECK(v[0]->id == "type2");
    CHECK(v[1]->id == "type18");
    CHECK(v[0]->ingrandimenti.size() == 2);
    CHECK(v[1]->ingrandimenti.size() == 3);
    CHECK(v[1]->ingrandimenti.back() == doctest::Approx(12.0));
    CHECK(v[0]->elevazione_max == doctest::Approx(74.5));
    CHECK(v[1]->tv);
    CHECK(v[1]->esm);
    CHECK_FALSE(v[0]->tv);
    CHECK(v[0]->modi.size() == 1);
    CHECK(v[1]->sezione_m2 > v[0]->sezione_m2);
    CHECK(v[0]->telemetro_altezza_max_m > v[0]->telemetro_altezza_min_m);
    const auto* alfa = c.classe("alfa");
    REQUIRE(alfa);
    CHECK(alfa->avvisatore_prima);
    CHECK(alfa->telecamera_verticale);
    REQUIRE(c.di_classe("alfa").size() == 1);
    CHECK(c.di_classe("alfa")[0]->telemetro == "reticolo");
    const auto* v3 = c.classe("victor3");
    REQUIRE(v3);
    CHECK(v3->tv == "mt70");
    CHECK(v3->tv_quota_max_m == doctest::Approx(50.0));
    CHECK(modo_vista_da_nome("intensificatore") == ModoVista::intensificatore);
    CHECK_FALSE(modo_vista_da_nome("radar"));
}

TEST_CASE("periscopi: alzata e discesa coi tempi, flag a fine corsa, esposizione") {
    PostoPeriscopi p(catalogo(), "los_angeles");
    Casuale caso(1);
    CHECK(p.stato().scelto == "type2");
    p.comanda(true);
    double t = 0.0;
    const double dt = 0.1;
    // il Type 2 sale in 6 s; la testa esce quando manca meno di 1,5 m di corsa (gli ultimi 1,5 s)
    for (; t < 4.4; t += dt) p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    CHECK_FALSE(p.stato().testa_fuori);
    CHECK_FALSE(p.stato().su);
    CHECK(p.esposizioni().empty());
    for (; t < 5.0; t += dt) p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().testa_fuori);
    CHECK_FALSE(p.stato().su);
    for (; t < 6.05; t += dt) p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().su);
    CHECK(p.stato().altezza_testa_m == doctest::Approx(1.5));
    for (; t < 16.0; t += dt) p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().esposto_s == doctest::Approx(16.0 - 4.5).epsilon(0.03));
    // giu': il flag resta finche' la testa e' fuori, l'esposizione si chiude
    p.comanda(false);
    p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    t += dt;
    CHECK(p.stato().su);
    for (; t < 22.0; t += dt) p.avanza(t, dt, QUOTA_688, 3.0, 2, caso);
    CHECK_FALSE(p.stato().su);
    CHECK_FALSE(p.stato().testa_fuori);
    CHECK(p.stato().esposto_s == 0.0);
    REQUIRE(p.esposizioni().size() == 1);
    CHECK(p.esposizioni()[0].durata() == doctest::Approx(16.0 - 4.5 + 1.25).epsilon(0.05));
    const auto f = p.finite();
    REQUIRE(f.size() == 1);
    CHECK(p.finite().empty());
    // troppo profondo: l'albero sale ma la testa resta sott'acqua
    p.comanda(true);
    for (; t < 32.0; t += dt) p.avanza(t, dt, 30.0, 3.0, 2, caso);
    CHECK(p.stato().alzata == doctest::Approx(1.0));
    CHECK_FALSE(p.stato().testa_fuori);
    CHECK(p.esposizioni().size() == 1);
}

TEST_CASE("periscopi: l'onda e l'errore di quota coprono la finestra col mare grosso, di rado col mare calmo") {
    CHECK(altezza_significativa_m(3) == doctest::Approx(0.875));
    CHECK(altezza_significativa_m(12) == altezza_significativa_m(9));
    for (const int mare : {2, 6}) {
        PostoPeriscopi p(catalogo(), "los_angeles");
        Casuale caso(1);
        p.comanda(true);
        int coperta = 0, fuori = 0;
        for (double t = 0.0; t < 600.0; t += 0.25) {
            p.avanza(t, 0.25, QUOTA_688, 3.0, mare, caso);
            if (p.stato().testa_fuori) ++fuori;
            if (p.stato().finestra_coperta) {
                ++coperta;
                CHECK(elevazione_onda_m(altezza_significativa_m(mare), t) > p.stato().altezza_vera_m - FINESTRA_M);
            }
        }
        CAPTURE(mare);
        CHECK(fuori > 2000);
        if (mare == 2) CHECK(coperta < fuori / 100);
        else CHECK(coperta > fuori / 20);
        CHECK(p.esposizioni().size() == 1);   // l'onda non spezza la guardata
    }
}

TEST_CASE("periscopi: scelta del Type 18 con l'altro alzato, ingrandimenti, modi, elevazione") {
    PostoPeriscopi p(catalogo(), "los_angeles");
    Casuale caso(1);
    p.ingrandimento(5);
    CHECK(p.stato().ingrandimento == 1);   // il Type 2 ne ha due
    p.modo(ModoVista::bassa_luminosita);
    CHECK(p.stato().modo == ModoVista::giorno);   // non ce l'ha
    p.elevazione(80.0);
    CHECK(p.stato().elevazione == doctest::Approx(74.5));
    p.comanda(true);
    double t = 0.0;
    for (; t < 7.0; t += 0.1) p.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    REQUIRE(p.stato().su);
    CHECK(p.scegli(1));
    CHECK(p.stato().scelto == "type2");   // prima scende
    for (; t < 13.0; t += 0.1) p.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().scelto == "type18");
    CHECK(p.stato().elevazione == doctest::Approx(60.0));
    for (; t < 22.0; t += 0.1) p.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().su);   // e sale il nuovo, con l'ordine ancora su
    p.ingrandimento(2);
    CHECK(p.stato().ingrandimento == 2);
    p.modo(ModoVista::tv);
    CHECK(p.stato().modo == ModoVista::tv);
    CHECK_FALSE(p.scegli(2));
    // della vista: filtri del modello, sdoppiamento fino all'angolo massimo, telecamera solo dell'Alfa
    p.filtro(0);
    CHECK(p.stato().filtro == 0);
    p.filtro(9);
    CHECK(p.stato().filtro == -1);
    p.sdoppiamento(5.0);
    CHECK(p.stato().sdoppiamento_gradi == doctest::Approx(2.0));
    p.telecamera_verticale(true);
    CHECK_FALSE(p.stato().telecamera_verticale);
    PostoPeriscopi alfa(catalogo(), "alfa");
    alfa.telecamera_verticale(true);
    CHECK(alfa.stato().telecamera_verticale);
    alfa.sdoppiamento(1.0);
    CHECK(alfa.stato().sdoppiamento_gradi == doctest::Approx(1.0));   // reticolo: l'angolo del cursore (kit, 31/10/2026)
    // la classe sconosciuta ha un Type 2 di riserva; la copia del posto resta valida
    PostoPeriscopi r(catalogo(), "nessuna");
    PostoPeriscopi copia = r;
    REQUIRE(copia.dati());
    CHECK(copia.dati()->id == "type2");
    CHECK(copia.dati() == &copia.periscopi()[0]);
}

TEST_CASE("periscopi: tremolio, piuma e danno con la velocita' e il mare") {
    const auto& t2 = *catalogo().periscopio("type2");
    const auto& t18 = *catalogo().periscopio("type18");
    CHECK(tremolio(t2, 4.0, 2) == 0.0);
    CHECK(tremolio(t2, 8.0, 2) == doctest::Approx(0.5));
    CHECK(tremolio(t2, 12.0, 2) == doctest::Approx(1.0));
    CHECK(tremolio(t18, 8.0, 2) == doctest::Approx(0.25));   // stabilizzato
    CHECK(tremolio(t2, 4.0, 5) > 0.0);                        // il mare scuote
    CHECK(piuma(t2, 1.0) == 0.0);
    CHECK(piuma(t18, 10.0) == doctest::Approx(1.0));
    CHECK(piuma(t2, 10.0) == doctest::Approx(0.5));
    CHECK(probabilita_danno_s(t2, 14.0) == 0.0);
    CHECK(probabilita_danno_s(t2, 20.0) == doctest::Approx(0.02));
    // a 25 nodi col periscopio alzato si rompe presto, a 10 mai
    PostoPeriscopi p(catalogo(), "los_angeles");
    Casuale caso(7);
    p.comanda(true);
    bool rotto = false;
    for (double t = 0.0; t < 120.0 && !rotto; t += 0.5) rotto = p.avanza(t, 0.5, QUOTA_688, 25.0, 2, caso);
    CHECK(rotto);
    CHECK(p.stato().danneggiato);
    for (double t = 120.0; t < 140.0; t += 0.5) p.avanza(t, 0.5, QUOTA_688, 25.0, 2, caso);
    CHECK(p.stato().alzata == 0.0);   // danneggiato: non sale
    PostoPeriscopi q(catalogo(), "los_angeles");
    q.comanda(true);
    for (double t = 0.0; t < 600.0; t += 0.5) CHECK_FALSE(q.avanza(t, 0.5, QUOTA_688, 10.0, 2, caso));
}

TEST_CASE("periscopi: portate visive per testa, piuma, mare, notte e osservatore") {
    const auto& t2 = *catalogo().periscopio("type2");
    const auto& t18 = *catalogo().periscopio("type18");
    Osservatore nave;   // vedette a 20 m, di giorno
    CondizioniVista calmo;
    const double r2 = portata_visiva_m(t2, sagoma_testa(t2, 1.5, 3.0), nave, calmo);
    const double r18 = portata_visiva_m(t18, sagoma_testa(t18, 1.5, 3.0), nave, calmo);
    CHECK(r2 > 1000.0);
    CHECK(r2 < 4000.0);
    CHECK(r18 > r2);   // la testa grossa
    // la piuma a 10 nodi si vede da miglia
    const double r18v = portata_visiva_m(t18, sagoma_testa(t18, 1.5, 10.0), nave, calmo);
    CHECK(r18v > 2.0 * MIGLIO);
    CHECK(r18v > 1.5 * r18);
    // mare 5 e notte la riducono; il sensore notturno ne recupera una parte
    CondizioniVista mosso = calmo;
    mosso.stato_mare = 5;
    CHECK(portata_visiva_m(t2, sagoma_testa(t2, 1.5, 3.0), nave, mosso) < 0.5 * r2);
    CondizioniVista notte = calmo;
    notte.luce = 0.0;
    const double rn = portata_visiva_m(t2, sagoma_testa(t2, 1.5, 3.0), nave, notte);
    CHECK(rn < 0.15 * r2);
    Osservatore intens = nave;
    intens.notte = ModoVista::intensificatore;
    CHECK(portata_visiva_m(t2, sagoma_testa(t2, 1.5, 3.0), intens, notte) > 3.0 * rn);
    // il riflesso del sole la allunga
    CondizioniVista sole = calmo;
    sole.riflesso = 1.0;
    CHECK(portata_visiva_m(t2, sagoma_testa(t2, 1.5, 3.0), nave, sole) > r2);
    // l'aereo vede meglio la piuma, il periscopio nemico ha l'orizzonte corto
    Osservatore aereo{TipoOsservatore::aereo, 150.0, ModoVista::giorno, true};
    CHECK(portata_visiva_m(t18, sagoma_testa(t18, 1.5, 10.0), aereo, calmo) > r18v);
    Osservatore peri{TipoOsservatore::periscopio, 1.2, ModoVista::giorno, true};
    CHECK(portata_visiva_m(t18, sagoma_testa(t18, 1.5, 10.0), peri, calmo) <= 3843.0 * (std::sqrt(1.2) + std::sqrt(1.5)) + 1.0);
    // niente testa, niente vista; probabilita' piena da vicino, nulla oltre l'orizzonte
    CHECK(portata_visiva_m(t2, sagoma_testa(t2, 0.0, 3.0), nave, calmo) == 0.0);
    const auto s = sagoma_testa(t2, 1.5, 3.0);
    CHECK(probabilita_vista(t2, s, nave, calmo, 200.0, 10.0) > 0.5);
    CHECK(probabilita_vista(t2, s, nave, calmo, 3.0 * r2, 10.0) < 0.05);
    CHECK(probabilita_vista(t2, s, nave, calmo, 30000.0, 10.0) == 0.0);
}

TEST_CASE("periscopi: sezione radar della testa per l'elettronica") {
    const auto& t2 = *catalogo().periscopio("type2");
    const auto& t18 = *catalogo().periscopio("type18");
    const auto s2 = sagoma_testa(t2, 1.5, 3.0);
    const auto s18 = sagoma_testa(t18, 1.5, 3.0);
    CHECK(s2.sezione_m2 == doctest::Approx(0.2 * 2.25));
    CHECK(s18.sezione_m2 == doctest::Approx(1.5 * 2.25));
    CHECK(sagoma_testa(t18, 3.0, 3.0).sezione_m2 == doctest::Approx(4.0 * s18.sezione_m2));   // il doppio vale ~4
    CHECK(sagoma_testa(t2, 0.0, 3.0).sezione_m2 == 0.0);
    // nel giro del radar: il Type 18 si vede piu' lontano del Type 2
    Radar r;
    r.id = "prova";
    r.periodo_s = 2.0;
    r.r50_periscopio_mare2_m = 6.0 * MIGLIO;
    r.r50_periscopio_mare4_m = 2.0 * MIGLIO;
    const auto testa = [](const SagomaTesta& s) { return TestaEsposta{s.sezione_m2, s.altezza_m, s.piuma}; };
    const double p2 = probabilita_giro(r, 6.0 * MIGLIO, 300.0, testa(s2), 2, 0.7, 3.0);
    const double p18 = probabilita_giro(r, 6.0 * MIGLIO, 300.0, testa(s18), 2, 0.7, 3.0);
    CHECK(p18 > 1.5 * p2);
    // la sezione di riferimento (0,5 m2 a 1,5 m) da' il giro dell'esposizione «periscopio»
    const double rif = probabilita_giro(r, 6.0 * MIGLIO, 300.0, Esposizione::periscopio, 2, 0.7, 3.0);
    CHECK(probabilita_giro(r, 6.0 * MIGLIO, 300.0, TestaEsposta{0.5, 1.5, 0.0}, 2, 0.7, 3.0) == doctest::Approx(rif));
    CHECK(probabilita_giro(r, 6.0 * MIGLIO, 300.0, TestaEsposta{}, 2, 0.7, 3.0) == 0.0);
}

TEST_CASE("periscopi: guardate dell'IA, avvisatore dei sovietici, TV МТ-70") {
    Casuale caso(11);
    // 688 dell'IA a quota periscopica: guardate da 10 s ogni 3-12 minuti
    PeriscopioIa a(catalogo(), "los_angeles", 0.0, caso);
    REQUIRE(a.modello());
    CHECK(a.modello()->id == "type18");   // per guardare, quello di ricerca
    CHECK(a.prossima() >= 180.0);
    CHECK(a.prossima() <= 720.0);
    double esposto = 0.0;
    for (double t = 0.0; t < 3600.0; t += 1.0) {
        a.avanza(t, QUOTA_688, 4.0, false, false, 1.0, caso);
        if (a.esposto()) {
            esposto += 1.0;
            CHECK(a.altezza_testa_m(QUOTA_688) == doctest::Approx(1.5));
        }
    }
    CHECK(a.guardate() >= 5);
    CHECK(a.guardate() <= 20);
    CHECK(esposto == doctest::Approx(10.0 * a.guardate()).epsilon(0.15));
    // in allarme, troppo veloce o troppo profondo: niente
    PeriscopioIa b(catalogo(), "los_angeles", 0.0, caso);
    for (double t = 0.0; t < 3600.0; t += 1.0) b.avanza(t, QUOTA_688, 4.0, true, false, 1.0, caso);
    CHECK(b.guardate() == 0);
    PeriscopioIa c(catalogo(), "los_angeles", 0.0, caso);
    for (double t = 0.0; t < 3600.0; t += 1.0) c.avanza(t, 100.0, 4.0, false, false, 1.0, caso);
    CHECK(c.guardate() == 0);
    // Alfa: prima l'avvisatore; un radar sentito rimanda la guardata
    PeriscopioIa d(catalogo(), "alfa", 0.0, caso);
    bool avvisatore_prima = false, visto_esposto = false;
    for (double t = 0.0; t < 1800.0 && !visto_esposto; t += 1.0) {
        d.avanza(t, 13.0, 4.0, false, false, 1.0, caso);
        if (d.fase() == FaseGuardata::avvisatore && !visto_esposto) avvisatore_prima = true;
        visto_esposto = d.esposto();
    }
    CHECK(avvisatore_prima);
    CHECK(visto_esposto);
    CHECK(d.avvisatore_su());
    PeriscopioIa e(catalogo(), "alfa", 0.0, caso);
    for (double t = 0.0; t < 3600.0; t += 1.0) e.avanza(t, 13.0, 4.0, false, true, 1.0, caso);
    CHECK(e.guardate() == 0);
    // Victor III a 40 m di giorno: la TV, senza testa fuori; di notte niente
    PeriscopioIa v(catalogo(), "victor3", 0.0, caso);
    bool tv = false;
    for (double t = 0.0; t < 3600.0; t += 1.0) {
        v.avanza(t, 40.0, 4.0, false, false, 1.0, caso);
        tv = tv || v.fase() == FaseGuardata::tv;
        CHECK(v.altezza_testa_m(40.0) == 0.0);
    }
    CHECK(tv);
    CHECK(v.guardate() > 0);
    PeriscopioIa n(catalogo(), "victor3", 0.0, caso);
    for (double t = 0.0; t < 3600.0; t += 1.0) n.avanza(t, 40.0, 4.0, false, false, 0.0, caso);
    CHECK(n.guardate() == 0);
    // deterministico
    Casuale c1(5), c2(5);
    PeriscopioIa x(catalogo(), "kilo", 0.0, c1), y(catalogo(), "kilo", 0.0, c2);
    for (double t = 0.0; t < 3600.0; t += 1.0) {
        x.avanza(t, 13.0, 3.0, false, false, 1.0, c1);
        y.avanza(t, 13.0, 3.0, false, false, 1.0, c2);
        CHECK(x.fase() == y.fase());
    }
    // l'osservatore della guardata: la testa, il sensore notturno del modello
    PeriscopioIa k(catalogo(), "trafalgar", 0.0, caso);
    CHECK(k.osservatore(18.0).tipo == TipoOsservatore::periscopio);
    CHECK(k.osservatore(18.0).notte == ModoVista::termico);
}

// --- nella missione (P2) ---

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"

namespace {

// Il 688 del giocatore a quota periscopica, fermo; un Alfa a quota periscopica a `distanza_m` verso nord, fermo; una
// fregata Krivak II a 8 km verso est (in vista per le foto)
std::string scena_periscopi(const std::string& nome, double distanza_m, double ora_utc = 12.0, double velocita_kn = 0.0,
                            const std::string& extra = "") {
    const auto f = std::filesystem::temp_directory_path() / ("m688_prova_periscopi_" + nome + ".lua");
    const std::string sub = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 10 }";
    const std::string nave = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";
    std::ofstream o(f);
    o << "return { id = 'PROVA_PERISCOPI_" << nome << "', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', ora_utc = " << ora_utc << ",\n" << extra
      << "gestori_giocatore = { periscopio = 'PERI' }, script = { PERI = function(u, p) m688.periscopio(u, p[1]); m688.salva_modello(u) end },\n"
      << "varianti = { { probabilita = 1, unita = {\n"
      << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
         "  posizione = { x = 0, y = 0 }, quota_m = 6.096, rotta = 0, velocita_kn = " << velocita_kn << ", " << sub << " },\n"
      << "{ id = 1, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = " << distanza_m << " },"
         "  quota_m = 6.096, rotta = 0, velocita_kn = 0, " << sub << " },\n"
      << "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 8000, y = 0 },"
         "  quota_m = 0, rotta = 0, velocita_kn = 0, " << nave << " },\n"
      << "} } } }\n";
    return f.string();
}

struct PartitaPeriscopi {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    std::unique_ptr<DanniMissione> dm;
    std::unique_ptr<DanniSottomarini> ds;
    PartitaPeriscopi(const std::string& file, std::uint64_t seme, bool enhanced = true) : m(file, seme) {
        if (!enhanced) return;
        m.usa_meteo(Missione::ModoMeteo::semplificato);   // le prove sono tarate su mare 3, sereno
        dm = std::make_unique<DanniMissione>(classi, M688_DATI, seme, RegolaDanni::simulazione);
        ds = std::make_unique<DanniSottomarini>(classi, M688_DATI, seme);
        ds->prepara(m, 0);
    }
    void passo() {
        m.avanza(1.0 / 9.0);
        if (dm) { dm->avanza(m, 1.0 / 9.0); ds->avanza(m, 1.0 / 9.0); m.raccogli_danni(ds.get(), dm.get()); }
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) passo(); }
    bool flag() const { return (m.giocatore()->flag54 & 2) != 0; }
    bool messaggio(int codice) const {
        for (const auto& x : m.messaggi_periscopi()) if (x.codice == codice) return true;
        return false;
    }
};

}  // namespace

TEST_CASE("periscopi missione: il classico alza subito, senza posto") {
    PartitaPeriscopi p(scena_periscopi("classico", 3000.0), 1, false);
    p.m.applica({0.0, TipoOrdine::periscopio, 0, 0, 1});
    CHECK(p.flag());
    CHECK_FALSE(p.m.con_periscopi());
    CHECK(p.m.posto_periscopi() == nullptr);
    p.avanza(60.0);
    CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::scegli_periscopio, 0, 0, 1}));
    CHECK(p.m.teste_ia().empty());
}

TEST_CASE("periscopi missione: alzata coi secondi, flag a fine corsa, esposizione lunga, foto del Type 18") {
    PartitaPeriscopi p(scena_periscopi("alzata", 30000.0), 1);
    p.avanza(1.0);
    REQUIRE(p.m.con_periscopi());
    REQUIRE(p.m.errori().empty());
    const PostoPeriscopi& posto = *p.m.posto_periscopi();
    CHECK(posto.periscopi().size() == 2);
    p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
    p.avanza(3.0);
    CHECK_FALSE(p.flag());               // il Type 2 sale in 6 s
    p.avanza(4.0);
    CHECK(p.flag());
    CHECK(p.messaggio(MSG_PERI_SU));
    CHECK(posto.stato().testa_fuori);
    // il Type 2 non fotografa
    CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::foto_periscopio, 90.0, 0, 1}));
    p.avanza(40.0);
    CHECK(p.messaggio(MSG_PERI_ESPOSTI));
    // il Type 18: scende il Type 2, sale l'altro
    CHECK(p.m.applica({p.m.tempo(), TipoOrdine::scegli_periscopio, 0, 0, 1}));
    p.avanza(20.0);
    CHECK(posto.stato().scelto == "type18");
    CHECK(posto.stato().su);
    CHECK(p.flag());
    p.m.applica({p.m.tempo(), TipoOrdine::elevazione_periscopio, 30.0, 0, 0});
    CHECK(posto.stato().elevazione == doctest::Approx(30.0));
    // foto verso est: la fregata nel campo
    CHECK(p.m.applica({p.m.tempo(), TipoOrdine::foto_periscopio, 90.0, 0, 1}));
    p.avanza(1.0);
    REQUIRE(p.m.foto_periscopio().size() == 1);
    CHECK(p.m.foto_periscopio()[0].in_vista.size() == 1);
    CHECK(p.m.foto_periscopio()[0].in_vista[0] == 2);
    // giu': il flag cade con la testa sotto; l'esposizione del Type 2 (oltre 30 s) e' nel registro
    p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 0});
    p.avanza(10.0);
    CHECK_FALSE(p.flag());
    CHECK_FALSE(posto.stato().testa_fuori);
    CHECK(p.m.esposizioni_lunghe().size() >= 1);
    bool foto = false, lunga = false;
    for (const auto& e : p.m.registro().eventi()) {
        if (e.tipo == TipoEvento::foto_periscopio) {
            foto = true;
            CHECK(e.unita == 2);
            CHECK(e.valore == doctest::Approx(90.0));
            CHECK(e.livello == 1);
            CHECK(e.inquadratura.visibile);
            CHECK(e.inquadratura.soggetto == 2);
            CHECK(e.noto);
        }
        lunga = lunga || e.tipo == TipoEvento::esposizione_lunga;
    }
    CHECK(foto);
    CHECK(lunga);
    bool fra_i_momenti = false;   // la foto del giocatore resta fra i momenti salienti
    for (const auto* e : p.m.registro().momenti_salienti(3)) fra_i_momenti = fra_i_momenti || e->tipo == TipoEvento::foto_periscopio;
    CHECK(fra_i_momenti);
    const auto esito = p.m.registro().esito(p.m);
    CHECK(esito.foto_periscopio == 1);
    CHECK(esito.esposizioni_lunghe >= 1);
    CHECK(esito.esposizione_periscopio_s > 40.0);
}

TEST_CASE("periscopi missione: l'Alfa fa le sue guardate, il giocatore vede la testa e l'Alfa vede la nostra") {
    PartitaPeriscopi p(scena_periscopi("guardate", 1500.0), 3);
    p.avanza(1.0);
    REQUIRE(p.m.con_periscopi());
    p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
    bool testa = false, vista_da_alfa = false;
    for (int i = 0; i < 9 * 3600; ++i) {   // un'ora: le guardate dell'Alfa sono 6-10
        p.passo();
        testa = testa || !p.m.teste_ia().empty();
        if (const auto* a = [&]() -> const UnitaClassica* { const auto it = p.m.unita().find(1); return it == p.m.unita().end() ? nullptr : &it->second; }())
            if (const auto c = a->contatti.find(0); c != a->contatti.end()) vista_da_alfa = vista_da_alfa || c->second.sensore == "vista";
    }
    REQUIRE(p.m.errori().empty());
    CHECK(testa);
    CHECK(p.messaggio(MSG_PERI_NEMICO));
    CHECK(vista_da_alfa);
    // col periscopio giu' l'Alfa non ci vede mai
    PartitaPeriscopi q(scena_periscopi("guardate_giu", 1500.0), 3);
    bool vista = false;
    for (int i = 0; i < 9 * 1800; ++i) {
        q.passo();
        if (const auto it = q.m.unita().find(1); it != q.m.unita().end())
            if (const auto c = it->second.contatti.find(0); c != it->second.contatti.end()) vista = vista || c->second.sensore == "vista";
    }
    CHECK_FALSE(vista);
}

TEST_CASE("periscopi missione: la notte e la luce dal sole della missione") {
    PartitaPeriscopi giorno(scena_periscopi("giorno", 30000.0, 12.0), 1);
    PartitaPeriscopi notte(scena_periscopi("notte", 30000.0, 0.0), 1);
    giorno.avanza(2.0);
    notte.avanza(2.0);
    CHECK(giorno.m.luce_periscopi() > 0.5);
    CHECK(notte.m.luce_periscopi() < 0.2);
}

TEST_CASE("periscopi missione: oltre la velocita' di danno il sistema «periscopio» si rompe e non sale piu'") {
    int rotti = 0;
    for (std::uint64_t seme = 1; seme <= 5; ++seme) {
        PartitaPeriscopi p(scena_periscopi("veloce", 30000.0, 12.0, 28.0), seme);
        p.avanza(1.0);
        REQUIRE(p.m.con_periscopi());
        p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
        p.avanza(90.0);
        if (p.m.guasti_richiesti().empty()) continue;
        ++rotti;
        CAPTURE(seme);
        CHECK(p.m.guasti_richiesti()[0].sistema == static_cast<int>(Sistema::periscopio));
        CHECK(p.messaggio(MSG_PERI_DANNEGGIATO));
        const auto* sist = p.ds->sistemi(0);
        REQUIRE(sist);
        CHECK((*sist)[static_cast<std::size_t>(Sistema::periscopio)].efficienza < 0.5);
        CHECK(p.m.posto_periscopi()->stato().danneggiato);
        p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
        CHECK(p.messaggio(MSG_PERI_NON_SALE));
        p.avanza(8.0);
        if (p.m.posto_periscopi()->stato().danneggiato) CHECK_FALSE(p.flag());   // finche' le squadre non lo riparano
    }
    CHECK(rotti >= 1);
}

TEST_CASE("periscopi missione: gli ordini al secondo 0 (replay) aspettano il posto") {
    PartitaPeriscopi p(scena_periscopi("secondo_zero", 30000.0), 1);
    CHECK(p.m.applica({0.0, TipoOrdine::scegli_periscopio, 0, 0, 1}));
    CHECK(p.m.applica({0.0, TipoOrdine::elevazione_periscopio, 20.0, 0, 0}));
    p.avanza(1.0);
    REQUIRE(p.m.con_periscopi());
    CHECK(p.m.posto_periscopi()->stato().scelto == "type18");
    CHECK(p.m.posto_periscopi()->stato().elevazione == doctest::Approx(20.0));
    // su al secondo 0: l'alzata ha i suoi secondi anche nel replay
    PartitaPeriscopi su(scena_periscopi("secondo_zero_su", 30000.0), 1);
    su.m.applica({0.0, TipoOrdine::periscopio, 0, 0, 1});
    su.avanza(2.0);
    CHECK_FALSE(su.flag());
    CHECK(su.m.posto_periscopi()->stato().comandato_su);
    su.avanza(6.0);
    CHECK(su.flag());
    // nel classico restano senza effetto
    PartitaPeriscopi c(scena_periscopi("secondo_zero_classico", 30000.0), 1, false);
    c.m.applica({0.0, TipoOrdine::scegli_periscopio, 0, 0, 1});
    c.avanza(1.0);
    CHECK_FALSE(c.m.con_periscopi());
}

TEST_CASE("periscopi: la testa colpita dall'artiglieria, bloccata (non scende) o cieca con la canna che si allaga") {
    PostoPeriscopi p(catalogo(), "los_angeles");
    Casuale caso(1);
    const int attacco = p.indice_ruolo("attacco"), ricerca = p.indice_ruolo("ricerca");
    REQUIRE(attacco >= 0);
    REQUIRE(ricerca >= 0);
    REQUIRE(p.scegli(attacco));
    p.comanda(true);
    double t = 0.0;
    for (int i = 0; i < 300; ++i, t += 0.1) p.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    REQUIRE(p.stato().alzata == doctest::Approx(1.0));
    // bloccato su: l'ordine di abbassarlo non lo muove, e l'altro non si puo' scegliere
    p.danneggia_testa(attacco, PostoPeriscopi::DannoTesta::bloccata);
    CHECK(p.bloccato());
    p.comanda(false);
    for (int i = 0; i < 300; ++i, t += 0.1) p.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    CHECK(p.stato().alzata == doctest::Approx(1.0));
    CHECK_FALSE(p.scegli(ricerca));
    // cieco con la canna che si allaga: imbarca finche' non e' giu'
    PostoPeriscopi q(catalogo(), "los_angeles");
    REQUIRE(q.scegli(attacco));
    q.comanda(true);
    for (int i = 0; i < 300; ++i, t += 0.1) q.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    q.danneggia_testa(attacco, PostoPeriscopi::DannoTesta::cieca, true);
    CHECK_FALSE(q.bloccato());
    CHECK(q.canna_aperta());
    q.comanda(false);
    for (int i = 0; i < 300; ++i, t += 0.1) q.avanza(t, 0.1, QUOTA_688, 3.0, 2, caso);
    CHECK(q.stato().alzata == doctest::Approx(0.0));
    CHECK_FALSE(q.canna_aperta());   // abbassato e chiuso
    CHECK(q.danno_testa(attacco) == PostoPeriscopi::DannoTesta::cieca);
}

TEST_CASE("periscopi missione: obiettivo «foto» di un'unita' o di una classe, con la distanza massima") {
    PartitaPeriscopi p(scena_periscopi("obiettivo_foto", 30000.0, 12.0, 0.0,
        "obiettivi = { { id = 'krivak', tipo = 'foto', classi = { 'krivak2' }, distanza_m = 10000, testo = 'Foto del Krivak' },"
        " { id = 'vicina', tipo = 'foto', unita = { 2 }, distanza_m = 5000, testo = 'Foto da vicino' },"
        " { id = 'alfa', tipo = 'foto', unita = { 1 }, entro_s = 120, primario = false, testo = 'Foto del sottomarino' } },\n"), 1);
    p.avanza(1.0);
    REQUIRE(p.m.con_periscopi());
    REQUIRE(p.m.errori().empty());
    const auto stato = [&](const char* id) {
        const auto e = p.m.esito_dettagliato();
        const auto o = std::find_if(e.obiettivi.begin(), e.obiettivi.end(), [&](const Obiettivo& x) { return x.id == id; });
        REQUIRE(o != e.obiettivi.end());
        return o->stato;
    };
    CHECK(stato("krivak") == Obiettivo::Stato::in_corso);
    p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
    p.avanza(8.0);
    p.m.applica({p.m.tempo(), TipoOrdine::scegli_periscopio, 0, 0, 1});   // il Type 18, che fotografa
    p.avanza(20.0);
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::foto_periscopio, 90.0, 0, 1}));   // verso est: il Krivak a 8 km
    p.avanza(1.0);
    CHECK(stato("krivak") == Obiettivo::Stato::raggiunto);
    CHECK(stato("vicina") == Obiettivo::Stato::in_corso);   // 8 km: troppo lontano per questa
    p.avanza(100.0);
    CHECK(stato("alfa") == Obiettivo::Stato::fallito);       // l'Alfa a 30 km, a nord: mai nel campo, e il tempo e' scaduto
}

// La simmetria dell'AOB (docs/25 §3.5): l'Alfa che alza il periscopio nelle sue guardate stima la rotta della Perry che
// vede (090), con l'errore che cresce con la distanza, e la mette nella sua TMA come il giocatore con osserva_aob
TEST_CASE("periscopi missione: l'Alfa al periscopio stima la rotta della Perry e la passa alla sua TMA") {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_periscopi_aob_ia.lua";
    {
        const std::string sub = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 10 }";
        const std::string nave = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 0 }";
        std::ofstream o(f);
        o << "return { id = 'PROVA_PERISCOPI_AOB_IA', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', ora_utc = 12, carriera = { situazione = 'pace' },\n"
          << "script = {}, varianti = { { probabilita = 1, unita = {\n"
          << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -90000 },"
             " quota_m = 200, rotta = 0, velocita_kn = 3, " << sub << " },\n"
          << "{ id = 1, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 6.096, rotta = 0,"
             " velocita_kn = 0, " << sub << " },\n"
          << "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -3000, y = 6000 }, quota_m = 0, rotta = 90,"
             " velocita_kn = 12, emcon = 'silenzio', " << nave << " },\n"   // senza radar: l'avvisatore dell'Alfa non la sente
          << "} } } }\n";
    }
    PartitaPeriscopi p(f.string(), 3);
    const Tma* t = nullptr;
    for (int i = 0; i < 9 * 3600 && !(t && t->rotte().size() >= 2); ++i) {
        p.passo();
        t = p.m.tma_dell_ia(1, 2);
    }
    REQUIRE(p.m.errori().empty());
    REQUIRE(t);
    REQUIRE_FALSE(t->rotte().empty());
    double somma = 0.0;
    for (const auto& r : t->rotte()) somma += std::fabs(normalizza_180(r.rotta - 90.0));
    MESSAGE("l'Alfa: ", t->rotte().size(), " rotte stimate, errore medio ", somma / static_cast<double>(t->rotte().size()), " gradi, sigma ",
            t->rotte().back().sigma_gradi);
    CHECK(somma / static_cast<double>(t->rotte().size()) < 40.0);
    CHECK(t->rotte().back().sigma_gradi >= 10.0);
    std::filesystem::remove(f);
}

TEST_CASE("periscopi: il nome sul pulsante si scrive col font del gioco, il cirillico traslitterato") {
    CHECK(nome_periscopio_pulsante("Type 18 (ricerca)") == "TYPE 18");
    CHECK(nome_periscopio_pulsante("«Сигнал» (universale, sistema ТВ-1)") == "SIGNAL");
    CHECK(nome_periscopio_pulsante("«Сигнал-3» (universale TV)") == "SIGNAL-3");
    CHECK(nome_periscopio_pulsante("«Лебедь-11/21» (del comandante)") == "LEBED'-11/21");
    CHECK(nome_periscopio_pulsante("ПЗНГ-10М (zenitale e di navigazione)") == "PZNG-10M");
    CHECK(nome_periscopio_pulsante("ПК-8,5 (d'attacco)") == "PK-8,5");
    // tutti quelli del catalogo: niente che il font non scriva, e abbastanza corti per il pulsante (101 px)
    CHECK(catalogo().periscopi.size() >= 13);
    for (const auto& [id, d] : catalogo().periscopi) {
        const std::string n = nome_periscopio_pulsante(d.nome);
        CAPTURE(id);
        CAPTURE(n);
        CHECK_FALSE(n.empty());
        CHECK(n.size() <= 13);
        for (const char c : n) CHECK((c >= 0x20 && c < 0x7F && c != '"'));
    }
}
