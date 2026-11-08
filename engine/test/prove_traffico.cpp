#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>

#include "nucleo/traffico.hpp"

using namespace m688;

// Traffico civile come contatti veri (blocco 1 del sonar, docs/20_sonar_blocco1.md §3)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

// una corsia dritta da ovest a est, 400 km, a doppio senso; una a senso unico piu' a nord; una zona di pesca a sud
RotteTraffico rotte_di_prova(double navi_giorno = 48.0) {
    RotteTraffico r;
    r.teatro = "prova";
    CorsiaTraffico a;
    a.id = "ovest_est";
    a.punti = {{-200000.0, 0.0}, {0.0, 0.0}, {200000.0, 0.0}};
    a.larghezza_m = 8000.0;
    a.navi_giorno = navi_giorno;
    a.miscela = {{"mercantile", 0.5}, {"portarinfuse", 0.3}, {"petroliera", 0.2}};
    r.corsie.push_back(a);
    CorsiaTraffico b = a;
    b.id = "solo_est";
    b.punti = {{-200000.0, 60000.0}, {200000.0, 60000.0}};
    b.doppio_senso = false;
    b.navi_giorno = 12.0;
    r.corsie.push_back(b);
    ZonaPesca z;
    z.id = "banco";
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j) z.punti.push_back({-20000.0 + 4000.0 * i, -60000.0 + 4000.0 * j});
    z.navi = 8.0;
    z.miscela = {{"peschereccio", 1.0}};
    r.zone.push_back(z);
    return r;
}

}  // namespace

TEST_CASE("traffico: allo stato stazionario le navi sono quante dice la portata, ripetibili") {
    const RotteTraffico r = rotte_di_prova();
    Traffico t(r, catalogo(), 6, 1234, 100000.0, 110000.0, 200);
    // atteso: nella corsia a doppio senso 48 al giorno per 200 km a circa 13 kn, circa 16 navi per senso; in quella a
    // senso unico 12 al giorno su 160 km, circa 3; pescherecci 8. In tutto circa 44, con la varianza di Poisson
    double somma = 0.0;
    int prove = 0;
    for (double s = 0.0; s <= 24.0 * 3600.0; s += 3 * 3600.0, ++prove) {
        t.avanza(s, 1.0, {0.0, 0.0}, 12.0);
        somma += static_cast<double>(t.navi().size());
    }
    const double media = somma / prove;
    MESSAGE("navi nel disco in media: " << media);
    CHECK(media > 33.0);
    CHECK(media < 55.0);
    // lo stesso seme, le stesse navi; un altro seme, altre
    Traffico a(r, catalogo(), 6, 99, 100000.0, 110000.0, 200), b(r, catalogo(), 6, 99, 100000.0, 110000.0, 200), c(r, catalogo(), 6, 100, 100000.0, 110000.0, 200);
    a.avanza(5000.0, 1.0, {0.0, 0.0}, 12.0);
    b.avanza(5000.0, 1.0, {0.0, 0.0}, 12.0);
    c.avanza(5000.0, 1.0, {0.0, 0.0}, 12.0);
    REQUIRE(a.navi().size() == b.navi().size());
    for (std::size_t i = 0; i < a.navi().size(); ++i) {
        CHECK(a.navi()[i].pos.x == b.navi()[i].pos.x);
        CHECK(a.navi()[i].tipo == b.navi()[i].tipo);
    }
    bool diverso = a.navi().size() != c.navi().size();
    for (std::size_t i = 0; !diverso && i < a.navi().size(); ++i) diverso = a.navi()[i].pos.x != c.navi()[i].pos.x;
    CHECK(diverso);
}

TEST_CASE("traffico: le navi scorrono, a destra nel doppio senso, e i pescherecci restano sul banco") {
    const RotteTraffico r = rotte_di_prova();
    Traffico t(r, catalogo(), 6, 7, 100000.0, 110000.0, 200);
    t.avanza(10000.0, 1.0, {0.0, 0.0}, 12.0);
    std::map<int, NaveTraffico> prima;
    for (const auto& n : t.navi()) prima[n.indice] = n;
    t.avanza(10010.0, 10.0, {0.0, 0.0}, 12.0);
    int controllate = 0, est = 0, ovest = 0, pesca = 0;
    for (const auto& n : t.navi()) {
        const auto it = prima.find(n.indice);
        if (it == prima.end()) continue;
        ++controllate;
        const double passo = distanza(it->second.pos, n.pos), atteso = n.velocita_kn * NODO * 10.0;
        CAPTURE(n.tipo);
        CHECK(passo == doctest::Approx(atteso).epsilon(0.1));
        if (n.attivita == AttivitaNave::pesca) {
            ++pesca;
            CHECK(n.pos.x >= -20001.0);
            CHECK(n.pos.x <= 16001.0);
            CHECK(n.pos.y <= -23999.0);
            CHECK(n.velocita_kn > 2.0);
            CHECK(n.velocita_kn < 5.5);
        } else if (std::fabs(n.pos.y) < 30000.0) {   // la corsia a doppio senso: verso est a sud dell'asse
            if (std::fabs(n.rotta - 90.0) < 5.0) { ++est; CHECK(n.pos.y < 0.0); }
            if (std::fabs(n.rotta - 270.0) < 5.0) { ++ovest; CHECK(n.pos.y > 0.0); }
        } else {   // quella a senso unico: solo verso est
            CHECK(std::fabs(n.rotta - 90.0) < 5.0);
        }
    }
    CHECK(controllate > 20);
    CHECK(est > 3);
    CHECK(ovest > 3);
    CHECK(pesca == 8);
    // ogni nave ha la firma del suo individuo e la classe del tipo
    for (const auto& n : t.navi()) {
        REQUIRE(t.firma(n.indice) != nullptr);
        CHECK(t.firma(n.indice)->classe == n.tipo);
        CHECK(n.classe == n.tipo);
        CHECK(n.lunghezza_m > 10.0);
    }
}

TEST_CASE("traffico: il tetto tiene le piu' vicine, le uscite e le entrate seguono il giocatore") {
    const RotteTraffico r = rotte_di_prova();
    Traffico t(r, catalogo(), 6, 5, 100000.0, 110000.0, 10);
    const Ricambio primo = t.avanza(20000.0, 1.0, {0.0, 0.0}, 12.0);
    CHECK(t.navi().size() == 10);
    CHECK(primo.entrate.size() == 10);
    CHECK(primo.uscite.empty());
    double piu_lontana = 0.0;
    for (const auto& n : t.navi()) piu_lontana = std::max(piu_lontana, distanza(n.pos, {0.0, 0.0}));
    CHECK(piu_lontana < 60000.0);   // con 40 navi nel disco, le 10 piu' vicine stanno nei primi 60 km
    // il giocatore va lontano: escono tutte, nessuna nave nuova senza rotte vicine
    const Ricambio via = t.avanza(20010.0, 10.0, {0.0, 500000.0}, 12.0);
    CHECK(via.uscite.size() == 10);
    CHECK(t.navi().empty());
    // torna: le navi rientrano coi loro indici di prima, se sono ancora li'
    const Ricambio torna = t.avanza(20020.0, 10.0, {0.0, 0.0}, 12.0);
    std::set<int> vecchi(primo.entrate.begin(), primo.entrate.end());
    int rientrate = 0;
    for (int i : torna.entrate) rientrate += static_cast<int>(vecchi.count(i));
    CHECK(rientrate >= 8);
}

TEST_CASE("traffico: i traghetti di linea partono all'ora, e le navi fuggono e si perdono") {
    RotteTraffico r;
    CorsiaTraffico f;
    f.id = "linea";
    f.punti = {{0.0, 0.0}, {60000.0, 0.0}};
    f.navi_giorno = 2.0;
    f.partenze_h = {8.0, 20.0};
    f.miscela = {{"traghetto", 1.0}};
    r.corsie.push_back(f);
    Traffico t(r, catalogo(), 6, 3, 100000.0, 110000.0, 40);
    // alle 8:30 locali: un traghetto partito da ogni capo mezz'ora fa, a 8-11 km dal porto
    t.avanza(30000.0, 1.0, {30000.0, 0.0}, 8.5);
    REQUIRE(t.navi().size() == 2);
    for (const auto& n : t.navi()) {
        const double dal_porto = std::min(distanza(n.pos, {0.0, 0.0}), distanza(n.pos, {60000.0, 0.0}));
        CHECK(dal_porto > 0.85 * 17.0 * NODO * 1800.0 - 1000.0);
        CHECK(dal_porto < 23.0 * NODO * 1800.0 + 1000.0);
    }
    // alle 7 non c'e' nessuno in mare (le corse delle 20 sono gia' arrivate)
    Traffico presto(r, catalogo(), 6, 3, 100000.0, 110000.0, 40);
    presto.avanza(30000.0, 1.0, {30000.0, 0.0}, 7.0);
    CHECK(presto.navi().empty());
    // fuga da un'esplosione: via dal punto alla massima, poi in 10 minuti di nuovo in rotta
    const NaveTraffico n = t.navi().front();
    const Vec2 scoppio{n.pos.x, n.pos.y - 2000.0};
    t.fuggi(n.indice, scoppio, 30300.0);
    for (double s = 30010.0; s <= 30290.0; s += 10.0) t.avanza(s, 10.0, {30000.0, 0.0}, 8.5);   // ancora in fuga
    const NaveTraffico* in_fuga = t.nave(n.indice);
    REQUIRE(in_fuga != nullptr);
    CHECK(in_fuga->attivita == AttivitaNave::fuga);
    CHECK(distanza(in_fuga->pos, scoppio) > 2000.0 + 0.9 * 290.0 * in_fuga->velocita_kn * NODO);
    for (double s = 30310.0; s <= 31000.0; s += 10.0) t.avanza(s, 10.0, {30000.0, 0.0}, 8.5);
    CHECK(std::fabs(t.nave(n.indice)->pos.y) < 1500.0);   // di nuovo sulla linea
    // persa (affondata): non torna piu', la firma resta
    t.perduta(n.indice);
    CHECK(t.nave(n.indice) == nullptr);
    const Ricambio dopo = t.avanza(31010.0, 10.0, {30000.0, 0.0}, 8.5);
    CHECK(std::find(dopo.entrate.begin(), dopo.entrate.end(), n.indice) == dopo.entrate.end());
    CHECK(t.firma(n.indice) != nullptr);
}

TEST_CASE("traffico: i file dei teatri si caricano e le corsie hanno tipi noti") {
    for (const char* teatro : {"gibilterra", "otranto", "celtica", "baltico", "barents", "faroe", "kunashir"}) {
        CAPTURE(teatro);
        const RotteTraffico r = carica_traffico(file_traffico(M688_DATI, teatro));
        CHECK(!r.corsie.empty());
        for (const auto& c : r.corsie)
            for (const auto& [tipo, w] : c.miscela) CHECK_MESSAGE(catalogo().tipo_civile(tipo) != nullptr, c.id << " " << tipo);
        for (const auto& z : r.zone)
            for (const auto& [tipo, w] : z.miscela) CHECK_MESSAGE(catalogo().tipo_civile(tipo) != nullptr, z.id << " " << tipo);
        // al centro del teatro, a mezzogiorno di giugno, c'e' traffico e sta sotto il tetto
        Traffico t(r, catalogo(), 6, 1, 100000.0, 110000.0, 40);
        t.avanza(36000.0, 1.0, {0.0, 0.0}, 12.0);
        MESSAGE(std::string(teatro) << ": " << t.navi().size() << " navi");
        CHECK(t.navi().size() <= 40);
    }
}
