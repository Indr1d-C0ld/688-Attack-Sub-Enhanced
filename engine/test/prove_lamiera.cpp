// I pezzi della variante A (grafica/lamiera.hpp): che restino nei loro rettangoli e che gli stati del tasto si
// distinguano. In fondo il piede di cerimonia e il posto libero (grafica/cornici.hpp, gruppo 4).
#include <doctest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/libretto.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/tema_stazioni.hpp"
#include "grafica/tela.hpp"
#include "nucleo/elettronica.hpp"
#include <toml.hpp>

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;
namespace lm = m688::lamiera;

namespace {

bool fuori_vuoto(const Tela& t, Rettangolo r) {
    for (int y = 0; y < t.altezza(); ++y)
        for (int x = 0; x < t.larghezza(); ++x)
            if (t.at(x, y) && (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h)) return false;
    return true;
}

std::set<int> tinte(const Tela& t, Rettangolo r) {
    std::set<int> c;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) c.insert(t.at(x, y));
    return c;
}

}  // namespace

TEST_CASE("lamiera: pannello, vetro e lamiera restano nel loro rettangolo") {
    const Rettangolo r{20, 30, 120, 60};
    Tela t(200, 120);
    t.riempi(0);
    lm::lamiera(t, r, 7, 3);
    CHECK(fuori_vuoto(t, r));
    CHECK(tinte(t, r).size() >= 2);   // la grana: non e' una tinta unita
    t.riempi(0);
    lm::pannello(t, r);
    CHECK(fuori_vuoto(t, r));
    t.riempi(0);
    lm::vetro(t, r);
    CHECK(fuori_vuoto(t, r));
    CHECK(t.at(r.x + 10, r.y + 10) == 0);   // il vetro e' nero
    CHECK(t.at(r.x, r.y) != t.at(r.x + r.w - 1, r.y + r.h - 1));   // lo smusso: luce in alto a sinistra
}

TEST_CASE("lamiera: gli stati del tasto si distinguono e il piede sta nella sua fascia") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Rettangolo r{10, 10, 100, 26};
    const auto disegna = [&](lm::Stato s, bool pred) {
        Tela t(120, 46);
        t.riempi(0);
        lm::tasto(t, r, "R  SCHEDA", ColoreLampada::ambra, s, pred, f);
        return t;
    };
    const Tela normale = disegna(lm::Stato::normale, false), acceso = disegna(lm::Stato::evidenziato, false),
               premuto = disegna(lm::Stato::premuto, false), spento = disegna(lm::Stato::disabilitato, false),
               pred = disegna(lm::Stato::normale, true);
    const int cx = r.x + 6, cy = r.y + r.h / 2;   // un punto della lente, fuori dalla scritta
    CHECK(normale.at(cx, cy) != acceso.at(cx, cy));
    CHECK(spento.at(cx, cy) != normale.at(cx, cy));
    CHECK(premuto.at(r.x + 3, r.y + 2) != acceso.at(r.x + 3, r.y + 2));   // premuto: la lente scende di un pixel
    CHECK(pred.at(r.x - 2, r.y - 2) == lampada(ColoreLampada::ambra, true));   // il filo d'oro del predefinito
    CHECK(normale.at(r.x - 2, r.y - 2) == 0);
    Tela t(640, 480);
    t.riempi(0);
    lm::piede(t, f, 400, "F1 MUSEO", {{"ESC  ESCI", ColoreLampada::rossa}, {"INVIO  AVVIA", ColoreLampada::verde}}, 1);
    CHECK(fuori_vuoto(t, {6, 398, 628, lm::ALTEZZA_PIEDE + 2}));
}

TEST_CASE("lamiera: i documenti del gruppo 3 restano nei loro rettangoli; il testo va a capo nella larghezza") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    Tela t(320, 240);
    const Rettangolo r{10, 10, 200, 150};
    const Rettangolo a = lm::modulo_continuo(t, r, 13);
    CHECK(fuori_vuoto(t, r));
    CHECK(a.x > r.x + 10);                    // dopo il margine dei fori
    CHECK(a.x + a.w < r.x + r.w - 10);
    for (const auto& riga : lm::a_capo(f, "LA SUA MISSIONE E' DISTRUGGERE TUTTE LE UNITA' DA GUERRA ENTRO IL TEMPO ASSEGNATO", a.w))
        CHECK(f.larghezza(riga, 1, 1, true) <= a.w);
    CHECK(lm::a_capo(f, "", 100).empty());
    Tela d(320, 240);
    const Rettangolo rf{20, 20, 120, 100};
    const Rettangolo fa = lm::foglio(d, rf);
    CHECK(fuori_vuoto(d, {rf.x, rf.y, rf.w + 3, rf.h + 3}));   // l'ombra sta 3 pixel fuori
    CHECK(d.at(fa.x + 2, fa.y + 2) == 15);                     // il foglio bianco
    lm::voce_modulo(d, f, fa.x, fa.y, fa.w, "DURATA", "00:06:59");
    lm::casella(d, fa.x, fa.y + 20, true);
    const Rettangolo tb = lm::timbro(d, f, fa.x, fa.y + 40, "IN CORSO", 4);
    CHECK(d.at(tb.x, tb.y) == 4);
    CHECK(tb.x + tb.w <= fa.x + fa.w);
    Tela v(200, 80);
    const int h = lm::campo_vetro(v, f, 10, 10, 150, "METEO", {{"VENTO 270", 11}, {"MARE 3", 11}});
    CHECK(h > 2 * f.righe());
    CHECK(fuori_vuoto(v, {9, 9, 152, h + 2}));
    Tela s(40, 20);
    lm::spia_voce(s, {10, 5, 12, 9}, ColoreLampada::verde, true);
    CHECK(s.at(15, 9) == lampada(ColoreLampada::verde, true));
    lm::spia_voce(s, {10, 5, 12, 9}, ColoreLampada::verde, false);
    CHECK(s.at(15, 9) == lampada(ColoreLampada::verde, false));
}


TEST_CASE("lamiera: l'elenco nel vetro scorre, e nei colori della vista non resta niente del tema") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    std::vector<std::string> voci;
    for (int i = 0; i < 20; ++i) voci.push_back("VOCE " + std::to_string(i));
    const Rettangolo r{10, 10, 150, 100};
    const auto tacca = [&](int primo) {   // la prima riga della tacca dello scorrimento
        Tela t(200, 140);
        t.riempi(0);
        lm::elenco_vetro(t, r, f, voci, primo + 1, 12, primo);
        CHECK(fuori_vuoto(t, r));
        CHECK(t.at(r.x + 4, r.y + 4 + 12 - 1) == lampada(ColoreLampada::ambra, true));   // la scelta, seconda riga visibile
        for (int y = r.y; y < r.y + r.h; ++y)
            if (t.at(r.x + r.w - 5, y) == 7) return y;
        return -1;
    };
    CHECK(tacca(0) < tacca(10));
    // il museo: la cornice e i tasti, poi la rimappatura; le lampade restano colorate
    Palette p = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    applica_tema(p, TemaStazione{});
    Tela t(320, 120);
    t.riempi(0);
    lm::pannello(t, {0, 0, 320, 120});
    lm::tasto(t, {20, 20, 120, 26}, "ESC  INDIETRO", ColoreLampada::rossa, lm::Stato::normale, false, f);
    lm::in_colori_vista(t, {0, 0, 320, 120}, p);
    bool tema = false;
    for (int y = 0; y < 120; ++y)
        for (int x = 0; x < 320; ++x) tema = tema || (t.at(x, y) >= colori_tema::PRIMO && t.at(x, y) <= colori_tema::ULTIMO);
    CHECK_FALSE(tema);
    const int lente = t.at(26, 33);
    CHECK((lente == 4 || lente == 12 || lente == 5 || lente == 6));   // un rosso EGA, non un grigio
}

TEST_CASE("cornici: il piede di cerimonia e il posto libero usano solo le rampe di cerimonia") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    Tela t(640, 480);
    t.riempi(0);
    piede_cerimonia(t, f, 428, "F FASCICOLO", {{"ESC  MENU"}, {"F  FASCICOLO", false}, {"INVIO  NUOVA CARRIERA"}}, 2);
    posto_libero(t, {12, 46, 148, 182}, f, 2);
    std::set<int> fuori;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            const bool ok = c == 0 || (c >= cerimonia::NERO && c < cerimonia::NERO + cerimonia::N_NERO) ||
                            (c >= cerimonia::BIANCO && c < cerimonia::BIANCO + cerimonia::N_BIANCO) ||
                            (c >= cerimonia::ORO && c < cerimonia::ORO + cerimonia::N_ORO) ||
                            (c >= cerimonia::LEGNO && c < cerimonia::LEGNO + cerimonia::N_LEGNO);
            if (!ok) fuori.insert(c);
        }
    CHECK(fuori.empty());
    CHECK(fuori_vuoto(t, {0, 46, 640, 434}));   // il posto libero dentro il suo rettangolo, il piede in fondo
    const int w = (640 - 24 - 8 * 2) / 3;
    CHECK(t.at(12 + 2 * (w + 8) - 2, 448) == cerimonia::ORO + 7);   // il filo d'oro del predefinito
    CHECK(t.at(12 + (w + 8) + 2, 452) == cerimonia::LEGNO);        // il tasto spento: il vano nel legno
    CHECK(t.at(14, 452) == cerimonia::ORO + 3);                     // quello acceso: l'ottone
}

TEST_CASE("libretto: la sagoma e' d'inchiostro e la pagina sta nel suo rettangolo coi soli EGA" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/modelli/krivak2.mdl"))) {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Font p = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    const Palette giorno = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    const Modello m = Modello::carica(std::string(M688_DATI) + "/modelli/krivak2.mdl");
    const auto rampa = rampa_grigi(giorno);
    const Sprite s = libretto::sagoma(m, 90.0, 1.5, rampa);
    int nero = 0;
    for (const auto c : s.pix) {
        CHECK((c == 0 || c == 15 || c == TRASPARENTE));
        nero += c == 0;
    }
    CHECK(nero > 100);
    const Sprite vela = libretto::sagoma(m, 90.0, 1.5, rampa, true);   // solo sopra i 3 m: meno inchiostro
    int nero_vela = 0;
    for (const auto c : vela.pix) nero_vela += c == 0;
    CHECK(nero_vela < nero);
    libretto::Voce v;
    v.nome = "UN NOME LUNGHISSIMO CHE NON ENTRA NELLA PAGINA";
    v.sezione = "NAVI DA GUERRA - URSS";
    v.albero_m = 29, v.albero_sigma_m = 2;
    v.segni = "LANCIATORE QUADRUPLO A PRUA; UN FUMAIOLO; DUE TORRI A POPPA";
    v.armi = {"SS-N-14", "2 X 100 MM"};
    Tela t(640, 480);
    t.riempi(TRASPARENTE);
    const Rettangolo r{422, 8, 210, 340};
    CHECK(libretto::pagina(t, r, f, p, v, {&s, &s, &s, &s}, 3, 36));
    bool fuori = false, colori = true;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            if (c == TRASPARENTE) continue;
            fuori = fuori || x < r.x || y < r.y || x >= r.x + r.w + 3 || y >= r.y + r.h + 3;   // l'ombra sta 3 pixel fuori
            colori = colori && c < 16;
        }
    CHECK_FALSE(fuori);
    CHECK(colori);
}

TEST_CASE("libretto: le tavole si caricano nell'ordine del libretto, con l'albero e il filtro" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/libretto.toml"))) {
    const auto voci = libretto::carica(std::string(M688_DATI) + "/libretto.toml");
    REQUIRE(voci.size() >= 30);
    for (std::size_t i = 1; i < voci.size(); ++i) CHECK(voci[i - 1].ordine < voci[i].ordine);
    for (const auto& v : voci) {
        CHECK(v.albero_m > 0.0);
        CHECK(v.albero_sigma_m > 0.0);
        CHECK_FALSE(v.vede.empty());
        if (v.sottomarino) CHECK(std::find(v.vede.begin(), v.vede.end(), "vela") != v.vede.end());
    }
    CHECK(voci.front().sezione == "NAVI DA GUERRA - URSS");
    // oltre il quadrante del Type 2 e del Type 18 (39,62 m) c'e' il secondo riferimento, e ci sta
    for (const auto& v : voci) {
        if (v.albero_m > 39.62) {
            CHECK_FALSE(v.riferimento_nome.empty());
            CHECK(v.riferimento_m >= 4.57);
            CHECK(v.riferimento_m <= 39.62);
            CHECK(v.riferimento_sigma_m > 0.0);
        }
    }
    // i sottomarini (docs/25 §6): una posizione, una lunghezza e un profilo della vela, e nessuno uguale a un altro
    std::set<std::set<std::string>> visti;
    for (const auto& v : voci) {
        if (!v.sottomarino) continue;
        const std::set<std::string> e(v.vede.begin(), v.vede.end());
        CHECK(e.count("vela_corta") + e.count("vela_lunga") == 1);
        CHECK(e.count("vela_squadrata") + e.count("vela_spiovente") == 1);
        INFO(v.classe);
        CHECK(visti.insert(e).second);
    }
    CHECK(visti.size() >= 15);
    // il filtro separa i tre che prima erano uguali: Charlie II, Kilo e Trafalgar
    const auto sola = [&](std::vector<std::string> e) {
        std::vector<std::string> out;
        for (const int i : libretto::filtra(voci, {"", "", std::move(e)})) out.push_back(voci[static_cast<std::size_t>(i)].classe);
        return out;
    };
    for (const auto& c : sola({"vela_centro", "vela_corta", "vela_squadrata"})) CHECK(c != "kilo");
    CHECK(sola({"vela_centro", "vela_lunga", "vela_squadrata", "vela_bassa", "bulbo_timone"}) == std::vector<std::string>{"sierra"});
    CHECK(libretto::testo_etichetta("vela_spiovente") == "VELA SPIOVENTE A POPPA");
}

TEST_CASE("libretto: il filtro tiene le tavole con la parte, il tipo e tutto quello che si vede") {
    const auto voce = [](std::string classe, std::string parte, std::string tipo, std::vector<std::string> vede) {
        libretto::Voce v;
        v.classe = std::move(classe), v.parte = std::move(parte), v.tipo = std::move(tipo), v.vede = std::move(vede);
        return v;
    };
    const std::vector<libretto::Voce> voci = {
        voce("krivak2", "patto", "fregata", {"lanciatori", "torri", "un_fumaiolo"}),
        voce("slava", "patto", "incrociatore", {"lanciatori", "piu_fumaioli", "radomi"}),
        voce("perry", "nato", "fregata", {"ponte_volo", "lanciatori", "un_fumaiolo"}),
        voce("akula", "patto", "sottomarino", {"vela"}),
        voce("cargo", "civili", "mercantile", {"container", "zattere"}),
    };
    using V = std::vector<int>;
    CHECK(libretto::filtra(voci, {}) == V{0, 1, 2, 3, 4});
    CHECK(libretto::filtra(voci, {"patto", "", {}}) == V{0, 1, 3});
    CHECK(libretto::filtra(voci, {"", "fregata", {}}) == V{0, 2});
    CHECK(libretto::filtra(voci, {"", "", {"lanciatori", "un_fumaiolo"}}) == V{0, 2});
    CHECK(libretto::filtra(voci, {"patto", "fregata", {"ponte_volo"}}).empty());
    CHECK_FALSE(libretto::Filtro{}.attivo());
    CHECK(libretto::Filtro{"", "", {"vela"}}.attivo());
    // gli elenchi: le parti e i tipi nell'ordine del libretto, le etichette in quello del vocabolario e le altre in fondo
    CHECK(libretto::parti(voci) == std::vector<std::string>{"patto", "nato", "civili"});
    CHECK(libretto::tipi(voci) == std::vector<std::string>{"fregata", "incrociatore", "sottomarino", "mercantile"});
    CHECK(libretto::tipi(voci, "patto") == std::vector<std::string>{"fregata", "incrociatore", "sottomarino"});
    CHECK(libretto::etichette(voci) == std::vector<std::string>{"ponte_volo", "lanciatori", "torri", "un_fumaiolo", "piu_fumaioli", "radomi",
                                                                  "container", "vela", "zattere"});
    CHECK(libretto::testo_etichetta("piu_fumaioli") == "PIU' FUMAIOLI");
    CHECK(libretto::testo_etichetta("nave spia") == "NAVE SPIA");
    CHECK(libretto::testo_etichetta("timoni_vela") == "TIMONI SULLA VELA");
    CHECK(libretto::testo_etichetta("zattere_rosse") == "ZATTERE ROSSE");
    CHECK(libretto::righe_filtro(voci) == 11);
    // scegli: la parte e il tipo girano in tondo passando da TUTTE, 0 torna a TUTTE; le etichette si mettono e si tolgono
    libretto::Filtro f;
    f = libretto::scegli(voci, f, 0, +1);
    CHECK(f.parte == "patto");
    f = libretto::scegli(voci, f, 0, -1);
    CHECK(f.parte.empty());
    f = libretto::scegli(voci, f, 0, -1);
    CHECK(f.parte == "civili");
    f = libretto::scegli(voci, f, 0, 0);
    CHECK(f.parte.empty());
    f = libretto::scegli(voci, f, 1, +1);
    CHECK(f.tipo == "fregata");
    f = libretto::scegli(voci, f, 0, -1);   // le civili non hanno fregate: il tipo torna a TUTTI
    CHECK(f.parte == "civili");
    CHECK(f.tipo.empty());
    f = libretto::scegli(voci, f, 1, +1);   // fra i tipi delle civili
    CHECK(f.tipo == "mercantile");
    f = libretto::scegli(voci, {}, 3, +1);
    CHECK(f.vede == std::vector<std::string>{"lanciatori"});
    f = libretto::scegli(voci, f, 3, -1);
    CHECK(f.vede.empty());
    CHECK(libretto::scegli(voci, f, 99, 1).vede.empty());   // fuori dalle righe non cambia niente
}

TEST_CASE("libretto: la striscia e la scheda del filtro stanno nel loro rettangolo coi soli EGA" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/libretto.toml"))) {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Font p = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    const auto voci = libretto::carica(std::string(M688_DATI) + "/libretto.toml");
    const auto controlla = [](const Tela& t, Rettangolo r, int ombra) {
        bool fuori = false, colori = true;
        int segnati = 0;
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) {
                const int c = t.at(x, y);
                if (c == TRASPARENTE) continue;
                ++segnati;
                fuori = fuori || x < r.x || y < r.y || x >= r.x + r.w + ombra || y >= r.y + r.h + ombra;
                colori = colori && c < 16;
            }
        CHECK_FALSE(fuori);
        CHECK(colori);
        CHECK(segnati > 0);
    };
    const libretto::Filtro filtro{"patto", "fregata", {"lanciatori", "un_fumaiolo", "plancia_centro", "torri"}};
    Tela t(640, 480);
    t.riempi(TRASPARENTE);
    const Rettangolo rs{422, 8, 210, p.righe() + 3};
    libretto::striscia(t, rs, p, filtro, "12 TAVOLE");   // troppo lunga: si tronca, non esce
    controlla(t, rs, 0);
    t.riempi(TRASPARENTE);
    const Rettangolo r{422, 8, 210, 340};
    libretto::scheda_filtro(t, r, f, p, voci, filtro, 5, "SU E GIU' LA RIGA, SINISTRA E DESTRA LA SCELTA, SPAZIO SEGNA, ESC CHIUDE");
    controlla(t, r, 3);
    // l'elenco scorre: anche col cursore sull'ultima etichetta la riga in negativo e' nella scheda. Il negativo e'
    // l'unico nero sul margine sinistro della carta, due pixel prima del testo
    const auto cursore = [&](int riga) {
        t.riempi(TRASPARENTE);
        libretto::scheda_filtro(t, r, f, p, voci, {}, riga, "ESC CHIUDE");
        int y_nero = -1;
        for (int y = r.y + 4; y < r.y + r.h - 4; ++y)
            if (t.at(r.x + 5, y) == 0) y_nero = y;
        return y_nero;
    };
    const int righe = libretto::righe_filtro(voci);
    CHECK(righe > 20);
    for (const int k : {0, 2, righe / 2, righe - 1}) CHECK(cursore(k) > r.y + 20);
    CHECK(cursore(righe - 1) > cursore(2));
    // ogni sottomarino dice dove sta la vela, una volta sola
    for (const auto& v : voci)
        if (v.sottomarino) {
            int posizioni = 0;
            for (const char* e : {"vela_prua", "vela_centro", "vela_arretrata"})
                posizioni += std::find(v.vede.begin(), v.vede.end(), e) != v.vede.end();
            CHECK(posizioni == 1);
        }
}

TEST_CASE("lamiera: le scritte scure sul beige dell'Alfa non usano il chiaro dello smalto; le chiare restano quelle di prima") {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    CHECK(lamiera::scritte_per(BattelloTema::alfa) == lamiera::Scritte::scure);
    CHECK(lamiera::scritte_per(BattelloTema::usa688) == lamiera::Scritte::chiare);
    const auto conta = [](const Tela& t, std::uint8_t c) {
        int n = 0;
        for (int y = 0; y < t.altezza(); ++y)
            for (int x = 0; x < t.larghezza(); ++x) n += t.at(x, y) == c;
        return n;
    };
    // la secondaria: chiara col chiaro dello smalto e l'ombra; scura in grigio 8, senza l'uno e senza l'altra
    Tela a(200, 40), b(200, 40), c(200, 40);
    a.riempi(TRASPARENTE), b.riempi(TRASPARENTE), c.riempi(TRASPARENTE);
    lamiera::secondaria(a, f, 4, 4, "IN ONDA");
    lamiera::secondaria(b, f, 4, 4, "IN ONDA", false, lamiera::Scritte::chiare);
    lamiera::secondaria(c, f, 4, 4, "IN ONDA", false, lamiera::Scritte::scure);
    CHECK(conta(a, smalto(11)) > 0);
    CHECK(conta(a, smalto(11)) == conta(b, smalto(11)));   // l'argomento di prima non cambia niente
    CHECK(conta(c, smalto(11)) == 0);
    CHECK(conta(c, 8) > 0);
    // il campo nel vetro passa le scritte all'etichetta
    Tela d(220, 80);
    d.riempi(TRASPARENTE);
    lamiera::campo_vetro(d, f, 4, 4, 200, "IN PROGRAMMA", {{"5 R 0602Z", fosforo(5)}}, lamiera::Scritte::scure);
    CHECK(conta(d, 8) > 0);
    // il piede: la riga d'aiuto nera sulla fascia chiara
    Tela e(640, 480);
    e.riempi(TRASPARENTE);
    lamiera::piede(e, f, 428, "FRECCE SCELGONO", {{"ESC  INDIETRO"}}, 0, lamiera::Scritte::scure);
    int nero_fascia = 0;
    for (int y = 428; y < 441; ++y)
        for (int x = 8; x < 632; ++x) nero_fascia += e.at(x, y) == 0;
    CHECK(nero_fascia > 0);
    // il principale: bianco inciso, o nero senza ombra
    Tela g(200, 40), h(200, 40);
    g.riempi(TRASPARENTE), h.riempi(TRASPARENTE);
    lamiera::principale(g, f, 4, 4, "KURS");
    lamiera::principale(h, f, 4, 4, "KURS", lamiera::Scritte::scure);
    CHECK(conta(g, 15) > 0);
    CHECK(conta(h, 15) == 0);
    CHECK(conta(h, 0) > 0);
}

TEST_CASE("libretto: la tavola delle luci sta nella pagina coi soli EGA e mostra i colori dei fanali" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Font p = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    const auto configurazioni = luci::carica(std::string(M688_DATI) + "/luci.toml");
    Tela t(640, 480);
    t.riempi(TRASPARENTE);
    const Rettangolo r{422, 8, 210, 340};
    CHECK(libretto::pagina_luci(t, r, f, p, configurazioni, 38, 38));
    bool fuori = false, colori = true;
    int rosso = 0, verde = 0, ambra = 0;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            if (c == TRASPARENTE) continue;
            fuori = fuori || x < r.x || y < r.y || x >= r.x + r.w + 3 || y >= r.y + r.h + 3;
            colori = colori && c < 16;
            rosso += c == 12, verde += c == 10, ambra += c == 14;
        }
    CHECK_FALSE(fuori);
    CHECK(colori);
    CHECK(rosso > 0);
    CHECK(verde > 0);
    CHECK(ambra > 0);
    // senza le classi degli schemi i riquadri restano vuoti e la funzione lo dice
    t.riempi(TRASPARENTE);
    CHECK_FALSE(libretto::pagina_luci(t, r, f, p, {}, 1, 1));
}

TEST_CASE("libretto: la tavola dell'AOB dalle luci sta nella pagina; di scorcio i due bianchi d'albero si allontanano" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const Font p = Font::carica(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt");
    const auto configurazioni = luci::carica(std::string(M688_DATI) + "/luci.toml");
    Tela t(640, 480);
    t.riempi(TRASPARENTE);
    const Rettangolo r{422, 8, 210, 340};
    CHECK(libretto::pagina_luci_aob(t, r, f, p, configurazioni, 39, 39));
    bool fuori = false, colori = true;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            if (c == TRASPARENTE) continue;
            fuori = fuori || x < r.x || y < r.y || x >= r.x + r.w + 3 || y >= r.y + r.h + 3;
            colori = colori && c < 16;
        }
    CHECK_FALSE(fuori);
    CHECK(colori);
    t.riempi(TRASPARENTE);
    CHECK_FALSE(libretto::pagina_luci_aob(t, r, f, p, {}, 1, 1));
}

// Le antenne radar (31/10/2026): emettitori.toml [antenne.<classe>] e' misurato sui modelli del kit, e l'altezza di
// ogni radar delle navi e' la mediana delle sue navi. L'orizzonte radar viene da quello che si vede
TEST_CASE("emettitori: le antenne radar tornano coi modelli del kit" * doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/emettitori.toml"))) {
    const auto t = toml::parse_file(std::string(M688_DATI) + "/emettitori.toml");
    const auto* ant = t["antenne"].as_table();
    REQUIRE(ant);
    std::map<std::string, std::vector<double>> quote;
    int coppie = 0;
    for (auto&& [classe, nodo] : *ant) {
        const auto modello = Modello::carica(std::string(M688_DATI) + "/modelli/" + std::string(classe.str()) + ".mdl");
        const auto& nomi = modello.parti();
        for (auto&& [radar, x] : *nodo.as_table()) {
            const std::string parte = (*x.as_table())["parte"].value_or(std::string{});
            const double q = (*x.as_table())["quota_m"].value_or(-1.0);
            const auto it = std::find(nomi.begin(), nomi.end(), parte);
            INFO(std::string(classe.str()) << " " << std::string(radar.str()) << " " << parte);
            REQUIRE(it != nomi.end());
            const auto k = static_cast<std::uint16_t>(it - nomi.begin());
            float z = -1e9f;
            for (const auto& tr : modello.triangoli())
                if (tr.parte == k)
                    for (const auto& v : tr.v) z = std::max(z, v[2]);
            CHECK(std::abs(z - q) <= 0.06);
            quote[std::string(radar.str())].push_back(q);
            ++coppie;
        }
    }
    CHECK(coppie >= 60);
    // l'altezza del radar e' la mediana delle sue navi, al metro
    for (auto& [radar, q] : quote) {
        std::sort(q.begin(), q.end());
        const double med = q.size() % 2 ? q[q.size() / 2] : 0.5 * (q[q.size() / 2 - 1] + q[q.size() / 2]);
        INFO(radar);
        CHECK(std::abs(t["radar"][radar]["altezza_m"].value_or(0.0) - med) <= 0.5 + 1e-9);
    }
    // i radar che stanno su una nave sola hanno proprio la sua antenna: Type 967/968, Sea Giraffe, DRBV 22, Top Plate
    CHECK(t["radar"]["sea_giraffe"]["altezza_m"].value_or(0.0) == doctest::Approx(19.0));
    CHECK(t["radar"]["drbv22"]["altezza_m"].value_or(0.0) == doctest::Approx(16.0));
    CHECK(t["radar"]["top_plate"]["altezza_m"].value_or(0.0) == doctest::Approx(31.0));
    CHECK(t["radar"]["tipo967"]["altezza_m"].value_or(0.0) == doctest::Approx(32.0));
    // il motore la usa per classe (Missione::altezza_radar): il radar civile del peschereccio costiero a 6,9 m, quello
    // di una classe senza antenna misurata all'altezza del radar
    const auto cat = carica_elettronica(std::string(M688_DATI) + "/emettitori.toml");
    const Radar* civ = cat.trova_radar("nav_civ");
    REQUIRE(civ);
    CHECK(cat.altezza_antenna("peschereccio_costiero", *civ) == doctest::Approx(6.9));
    CHECK(cat.altezza_antenna("cargo", *civ) == doctest::Approx(43.7));
    CHECK(cat.altezza_antenna("classe_che_non_c_e", *civ) == doctest::Approx(civ->altezza_m));
}

// Misura (non una prova: si lancia con --no-skip): quanto costa uno sprite dal modello, per decidere quali navi intatte
// vicine disegnare dal modello invece che dagli sprite ogni 5 gradi (E9 §7)
TEST_CASE("misura: il tempo di uno sprite generato dal modello" * doctest::skip(true)) {
    const auto rampa = rampa_grigi(Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal"));
    for (const char* c : {"nimitz", "kirov", "perry", "petroliera_costiera", "peschereccio_costiero"}) {
        const auto m = Modello::carica(std::string(M688_DATI) + "/modelli/" + c + ".mdl");
        for (const double px_m : {3.75, 2.0, 1.0, 0.5}) {
            const auto t0 = std::chrono::steady_clock::now();
            int n = 0;
            for (double aob = 10.0; aob < 170.0; aob += 16.0, ++n) (void)genera_sprite(m, PosaNave{}, aob, true, rampa, px_m);
            const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count() / n;
            MESSAGE(std::string(c) << " " << m.triangoli().size() << " triangoli, " << px_m << " px/m: " << ms << " ms per sprite");
        }
    }
}

// Misura (--no-skip): lo stesso sprite con 3 x 3 e 2 x 2 campioni, affiancati in una BMP per il confronto a vista
TEST_CASE("misura: sprite dal modello con 3 x 3 e 2 x 2 campioni" * doctest::skip(true)) {
    const auto giorno = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    const auto m = Modello::carica(std::string(M688_DATI) + "/modelli/perry.mdl");
    Tela t;
    t.riempi(100);
    int y = 10;
    for (const int ss : {3, 2}) {
        const Sprite s = genera_sprite(m, PosaNave{}, 37.0, true, rampa_grigi(giorno), 3.75, ss);
        t.disegna_sprite(s, 320, y + s.ancora_y, 1.0, 480, 0.0);
        y += s.h + 20;
    }
    t.salva_bmp((std::filesystem::temp_directory_path() / "m688_confronto_ss.bmp").string(), giorno);
}
