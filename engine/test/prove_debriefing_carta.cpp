// La carta del debriefing (app/debriefing.hpp; scelta dell'utente del 07/11/2026): si trascina e si ingrandisce con la
// rotella attorno al cursore, si ricentra solo cambiando evento, e le etichette che si toccherebbero si raccolgono
#include <doctest.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../app/debriefing.hpp"
#include "../app/etichette_carta.hpp"
#include "../app/partita.hpp"
#include "../app/risorse.hpp"
#include "../app/trascina_carta.hpp"
#include "grafica/effetti.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tela.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool c_e() { return fs::exists(M688_MISSIONI "/SCENE1A.lua") && fs::exists(M688_DATI "/classi.toml"); }

struct Sala {
    Risorse r = carica_risorse(M688_DATI);
    Effetti effetti{1};
    Partita p;
    static ImpostazioniPartita prova() {
        ImpostazioniPartita o;
        o.dati = M688_DATI;
        o.cartella_missioni = M688_MISSIONI;
        o.missione = "SCENE1A";
        o.regola = RegolaDanni::simulazione;
        return o;
    }
    Sala() : p(r, prova(), effetti) {
        for (int k = 0; k < 600 * 9; ++k) p.avanza(1.0 / 9.0);
    }
};
Sala& sala() {
    static Sala s;
    return s;
}

Comandi mano(int x, int y, bool premuto) {
    Comandi c;
    c.mouse = {x, y};
    c.premuto = premuto;
    return c;
}

int larghezza(const std::string& s) { return static_cast<int>(s.size()) * 6; }

}  // namespace

TEST_CASE("carta del debriefing: si trascina, e sullo stesso evento non torna indietro" * doctest::skip(!c_e())) {
    SchermataDebriefing d(sala().p, false, StileCarta::vga);
    Tela t;
    d.mostra_tutti(true);
    d.mostra_verita(true);
    d.disegna(t, sala().r.font);
    REQUIRE(d.eventi() > 3);
    d.vai(1);
    d.comandi(mano(-1, -1, false), 0.1);   // l'evento visto
    const Rettangolo r = d.carta();
    const int x = r.x + r.w / 2, y = r.y + r.h / 2;
    const Vec2 prima = d.centro_carta();
    const double mpp = d.metri_per_pixel();
    d.comandi(mano(x, y, true), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(prima.x));   // la pressione da sola non sposta
    d.comandi(mano(x + 30, y - 10, true), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(prima.x - 30 * mpp));
    CHECK(d.centro_carta().y == doctest::Approx(prima.y - 10 * mpp));
    const Vec2 spostato = d.centro_carta();
    d.comandi(mano(x + 30, y - 10, false), 0.1);   // rilasciato: resta, e passando il mouse non si muove
    d.comandi(mano(x + 90, y + 40, false), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(spostato.x));
    CHECK(d.centro_carta().y == doctest::Approx(spostato.y));
    // una pressione che comincia fuori dalla carta non la prende
    d.comandi(mano(r.x + r.w + 40, y, true), 0.1);
    d.comandi(mano(r.x + r.w + 10, y + 30, true), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(spostato.x));
    d.comandi(mano(-1, -1, false), 0.1);
}

TEST_CASE("carta del debriefing: si ricentra solo cambiando evento, se quello nuovo e' fuori" * doctest::skip(!c_e())) {
    SchermataDebriefing d(sala().p, false, StileCarta::vga);
    Tela t;
    d.disegna(t, sala().r.font);
    d.mostra_tutti(true);
    d.mostra_verita(true);
    REQUIRE(d.eventi() > 3);
    d.vai(0);
    d.comandi(mano(-1, -1, false), 0.1);
    // molto vicino: l'evento 0 esce dalla vista trascinando, e la carta resta dov'e'
    Comandi zoom = mano(-1, -1, false);
    zoom.ingrandisci = true;
    for (int k = 0; k < 20; ++k) d.comandi(zoom, 0.1);
    const Rettangolo r = d.carta();
    const int x = r.x + r.w / 2, y = r.y + r.h / 2;
    d.comandi(mano(x, y, true), 0.1);
    d.comandi(mano(x + 180, y, true), 0.1);
    d.comandi(mano(x + 180, y, false), 0.1);
    const Vec2 messa = d.centro_carta();
    d.comandi(mano(-1, -1, false), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(messa.x));
    // un evento nuovo: se e' fuori la carta ci va, se e' dentro resta dov'e'
    const auto& eventi = sala().p.missione().eventi_missione();
    REQUIRE(static_cast<int>(eventi.size()) == d.eventi());
    const auto in_vista = [&](const EventoMissione& e) {
        return std::abs(e.pos.x - d.centro_carta().x) / d.metri_per_pixel() <= r.w / 2 - 24 && std::abs(e.pos.y - d.centro_carta().y) / d.metri_per_pixel() <= r.h / 2 - 24;
    };
    int fuori = -1;
    for (int i = 1; i < d.eventi() && fuori < 0; ++i)
        if (!in_vista(eventi[static_cast<std::size_t>(i)])) fuori = i;
    REQUIRE(fuori > 0);
    d.vai(fuori);
    d.comandi(mano(-1, -1, false), 0.1);
    CHECK(d.centro_carta().x == doctest::Approx(eventi[static_cast<std::size_t>(fuori)].pos.x));
    CHECK(d.centro_carta().y == doctest::Approx(eventi[static_cast<std::size_t>(fuori)].pos.y));
    int dentro = -1;
    for (int i = 0; i < d.eventi() && dentro < 0; ++i)
        if (i != fuori && in_vista(eventi[static_cast<std::size_t>(i)]) && eventi[static_cast<std::size_t>(i)].pos.x != d.centro_carta().x) dentro = i;
    if (dentro >= 0) {
        const Vec2 qui = d.centro_carta();
        d.vai(dentro);
        d.comandi(mano(-1, -1, false), 0.1);
        CHECK(d.centro_carta().x == doctest::Approx(qui.x));
        CHECK(d.centro_carta().y == doctest::Approx(qui.y));
    }
}

TEST_CASE("carta del debriefing: la rotella attorno al cursore, fuori dalla carta nulla, limiti e bordi" * doctest::skip(!c_e())) {
    SchermataDebriefing d(sala().p, false, StileCarta::vga);
    Tela t;
    d.mostra_tutti(true);
    d.mostra_verita(true);
    d.disegna(t, sala().r.font);
    REQUIRE(d.eventi() > 1);
    d.vai(1);
    d.comandi(mano(-1, -1, false), 0.1);
    const Rettangolo r = d.carta();
    const int x = r.x + r.w / 4, y = r.y + r.h / 3;
    const auto mondo = [&] {
        return Vec2{d.centro_carta().x + (x - r.x - r.w / 2.0) * d.metri_per_pixel(), d.centro_carta().y - (y - r.y - r.h / 2.0) * d.metri_per_pixel()};
    };
    const Vec2 sotto = mondo();
    const double mpp = d.metri_per_pixel();
    Comandi su = mano(x, y, false);
    su.rotella = 1;
    d.comandi(su, 0.1);
    CHECK(d.metri_per_pixel() < mpp);
    CHECK(mondo().x == doctest::Approx(sotto.x).epsilon(0.001));
    CHECK(mondo().y == doctest::Approx(sotto.y).epsilon(0.001));
    // sul foglio la rotella non fa nulla: ne' zoom ne' cambio d'evento
    const double m2 = d.metri_per_pixel();
    const int evento = d.corrente();
    for (const int verso : {3, -3}) {   // nei due versi: da un limite dello zoom uno dei due non si vedrebbe
        Comandi fuori = mano(r.x + r.w + 60, y, false);
        fuori.rotella = verso;
        d.comandi(fuori, 0.1);
        CHECK(d.metri_per_pixel() == doctest::Approx(m2));
        CHECK(d.corrente() == evento);
    }
    // i limiti: tanti scatti non passano il minimo e il massimo
    Comandi tanti = mano(r.x + r.w / 2, r.y + r.h / 2, false);
    tanti.rotella = 60;
    d.comandi(tanti, 0.1);
    const double vicino = d.metri_per_pixel();
    d.comandi(tanti, 0.1);
    CHECK(d.metri_per_pixel() == doctest::Approx(vicino));
    // piu' vicino della scala piu' grande della carta, che in S5 e' gia' quella di partenza
    CHECK(vicino == doctest::Approx(2.0 * sala().p.missione().metri_per_unita() / 2.0 / 4.0));
    tanti.rotella = -60;
    d.comandi(tanti, 0.1);
    const double lontano = d.metri_per_pixel();
    d.comandi(tanti, 0.1);
    CHECK(d.metri_per_pixel() == doctest::Approx(lontano));
    CHECK(lontano > vicino);
    // i bordi: trascinando a lungo il centro resta fra le tracce e gli eventi
    Comandi z = mano(-1, -1, false);
    z.ingrandisci = true;
    for (int k = 0; k < 20; ++k) d.comandi(z, 0.1);
    const int cx = r.x + r.w / 2, cy = r.y + r.h / 2;
    double x0 = 1e18, x1 = -1e18;
    for (const auto& q : sala().p.tracce()) x0 = std::min(x0, q.pos.x), x1 = std::max(x1, q.pos.x);
    for (const auto& e : sala().p.missione().eventi_missione()) x0 = std::min(x0, e.pos.x), x1 = std::max(x1, e.pos.x);
    for (int k = 0; k < 400; ++k) {
        d.comandi(mano(cx, cy, true), 0.1);
        d.comandi(mano(cx + 150, cy, true), 0.1);
        d.comandi(mano(cx + 150, cy, false), 0.1);
    }
    CHECK(d.centro_carta().x == doctest::Approx(x0));
    for (int k = 0; k < 400; ++k) {
        d.comandi(mano(cx + 150, cy, true), 0.1);
        d.comandi(mano(cx, cy, true), 0.1);
        d.comandi(mano(cx, cy, false), 0.1);
    }
    CHECK(d.centro_carta().x == doctest::Approx(x1));
}

TEST_CASE("etichette della carta: quelle che si toccano si raccolgono, lontane restano sole") {
    // tre vicine e una lontana
    auto e = raccogli_etichette({{100, 100, 12, 30}, {104, 102, 13, 90}, {98, 96, 14, 10}, {300, 300, 20, 5}}, larghezza);
    REQUIRE(e.size() == 2);
    CHECK(e[0].testo() == "12-14");
    CHECK(e[0].quanti == 3);
    CHECK(e[0].capo == 13);   // il colore dal piu' interessante
    CHECK(e[1].testo() == "20");
    // numeri non di seguito: il primo e quanti altri, non un intervallo che non c'e'
    e = raccogli_etichette({{100, 100, 12, 30}, {104, 102, 13, 90}, {98, 96, 241, 10}}, larghezza);
    REQUIRE(e.size() == 1);
    CHECK(e[0].testo() == "12+2");
    // avvicinandosi (i pixel raddoppiano piu' volte) tornano separate
    e = raccogli_etichette({{100, 100, 12, 30}, {180, 140, 13, 90}, {60, 40, 18, 10}, {300, 300, 20, 5}}, larghezza);
    CHECK(e.size() == 4);
    // nessuna coppia si tocca dopo la raccolta, anche se il gruppo cresce e ne incontra altri
    std::vector<PuntoEtichetta> tanti;
    for (int k = 0; k < 336; ++k) tanti.push_back({50 + k * 7 % 300, 40 + k * 13 % 250, k + 1, k % 100});
    const auto g = raccogli_etichette(tanti, larghezza);
    CHECK(g.size() < tanti.size());
    int quanti = 0;
    for (std::size_t i = 0; i < g.size(); ++i) {
        quanti += g[i].quanti;
        for (std::size_t j = i + 1; j < g.size(); ++j) {
            const double w = (larghezza(g[i].testo()) + larghezza(g[j].testo())) / 2.0 + 4.0;
            CHECK_FALSE((std::abs(g[i].x - g[j].x) < w && std::abs(g[i].y - g[j].y) < ALTEZZA_ETICHETTA));
        }
    }
    CHECK(quanti == 336);
}

TEST_CASE("carta del debriefing: l'evento corrente e' sempre sopra, in giallo, anche in mezzo agli altri" * doctest::skip(!c_e())) {
    SchermataDebriefing d(sala().p, false, StileCarta::vga);
    Tela t;
    d.mostra_tutti(true);
    d.mostra_verita(true);
    d.disegna(t, sala().r.font);
    REQUIRE(d.eventi() > 5);
    d.vai(d.eventi() - 1);
    d.comandi(mano(-1, -1, false), 0.1);
    d.disegna(t, sala().r.font);
    const Rettangolo r = d.carta();
    int gialli = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) gialli += t.at(x, y) == colori::GIALLO;
    CHECK(gialli > 20);
}

// La pagina vera: un evento della partita diventa l'affondamento di una petroliera, o di un battello immerso
TEST_CASE("cartolina dell'affondamento: sulla pagina del debriefing, coi grigi dello scafo; gli altri eventi non cambiano" * doctest::skip(!c_e())) {
    Sala& s = sala();
    Periscopio per(s.r.font, s.r.classi, s.r.sprite);
    per.usa_modelli(s.r.modelli, rampa_grigi(s.r.giorno));
    const auto& eventi = s.p.missione().eventi_missione();
    const auto visto = std::find_if(eventi.begin(), eventi.end(), [](const EventoMissione& e) { return e.inquadratura.visibile && !e.pose.empty() && e.pose[0].categoria != "arma"; });
    REQUIRE(visto != eventi.end());
    const int indice = static_cast<int>(visto - eventi.begin());
    auto& e = const_cast<EventoMissione&>(*visto);
    const EventoMissione com_era = e;
    const Rettangolo r{441, 64, 184, 78};   // la cartolina sul foglio
    const auto pagina = [&](Tela& t, Palette& pal) {
        SchermataDebriefing d(s.p, false, StileCarta::vga, &per, &s.r.giorno);
        d.imposta_dati(M688_DATI);
        d.mostra_tutti(true);
        d.mostra_verita(true);
        d.vai(indice);
        d.disegna(t, s.r.font);
        pal = s.r.giorno;
        d.palette(pal);
    };
    const auto impronta = [&](const Tela& t, const Palette& pal) {
        std::vector<std::array<int, 3>> v;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) {
                const auto c = pal.rgb(t.at(x, y));
                v.push_back({static_cast<int>(c[0]), static_cast<int>(c[1]), static_cast<int>(c[2])});
            }
        return v;
    };
    const auto grigi = [](const std::vector<std::array<int, 3>>& v) {
        std::set<int> toni;
        int n = 0;
        for (const auto& c : v)
            if (std::abs(c[0] - c[1]) <= 6 && std::abs(c[1] - c[2]) <= 6 && c[0] > 20 && c[0] < 235) ++n, toni.insert(c[0]);
        return std::pair<int, int>{n, static_cast<int>(toni.size())};
    };
    Tela t;
    Palette pal;
    pagina(t, pal);
    const auto prima = impronta(t, pal);

    PosaEvento posa;
    posa.unita = 7777;
    posa.classe = "petroliera";
    posa.categoria = "nave";
    posa.pos = e.pose[0].pos;
    posa.rotta = 60.0;
    e.tipo = TipoEvento::affondamento;
    e.unita = 7777;
    e.categoria = "nave";
    e.inquadratura.soggetto = 7777;
    e.pose = {posa};
    pagina(t, pal);
    const auto nave = impronta(t, pal);
    CHECK(nave != prima);
    const auto [n_nave, toni_nave] = grigi(nave);
    CHECK(n_nave > 400);
    CHECK(toni_nave >= 8);   // i grigi dello scafo hanno i loro posti nella palette della cartolina

    posa.classe = "los_angeles";
    posa.categoria = "sottomarino";
    posa.quota = 200.0;
    e.categoria = "sottomarino";
    e.pose = {posa};
    pagina(t, pal);
    const auto immerso = impronta(t, pal);
    CHECK(immerso != nave);
    CHECK(grigi(immerso).first > 100);   // la colonna d'acqua e la schiuma, non lo scafo: quello e' provato in prove_cerimonia

    // rimesso com'era: la pagina di prima, identica
    e = com_era;
    pagina(t, pal);
    CHECK(impronta(t, pal) == prima);
}

TEST_CASE("carte: le funzioni comuni del replay, degli affiancati e del briefing") {
    const Rettangolo r{100, 50, 200, 100};
    Vec2 centro{1000.0, 2000.0};
    double mpp = 100.0;
    // la rotella: il punto sotto il cursore resta dov'e', fuori dalla carta nulla, ai limiti si ferma
    Comandi c = mano(150, 60, false);
    c.rotella = 1;
    const auto sotto = [&] { return Vec2{centro.x + (150 - 200.0) * mpp, centro.y - (60 - 100.0) * mpp}; };
    const Vec2 prima = sotto();
    rotella_carta(c, r, centro, mpp);
    CHECK(mpp == doctest::Approx(100.0 / 1.5));
    CHECK(sotto().x == doctest::Approx(prima.x));
    CHECK(sotto().y == doctest::Approx(prima.y));
    const double m = mpp;
    Comandi fuori = mano(20, 20, false);
    fuori.rotella = 2;
    rotella_carta(fuori, r, centro, mpp);
    CHECK(mpp == doctest::Approx(m));
    c.rotella = 60;
    rotella_carta(c, r, centro, mpp);
    CHECK(mpp == doctest::Approx(10.0));
    c.rotella = -60;
    rotella_carta(c, r, centro, mpp);
    CHECK(mpp == doctest::Approx(2000.0));
    // il trascinamento: come nel replay, solo se comincia sulla carta
    std::optional<std::pair<int, int>> presa;
    centro = {0.0, 0.0};
    CHECK_FALSE(trascina_carta(mano(150, 60, true), r, 10.0, presa, centro));
    CHECK(trascina_carta(mano(160, 50, true), r, 10.0, presa, centro));
    CHECK(centro.x == doctest::Approx(-100.0));
    CHECK(centro.y == doctest::Approx(-100.0));
    CHECK_FALSE(trascina_carta(mano(160, 50, false), r, 10.0, presa, centro));
    CHECK_FALSE(trascina_carta(mano(20, 20, true), r, 10.0, presa, centro));
    CHECK(centro.x == doctest::Approx(-100.0));
}
