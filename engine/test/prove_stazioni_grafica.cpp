// Grafica delle stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md): font FNT2 delle consolle
// (tools/pipeline/font_stazioni.py), font originale invariato, temi di colore negli indici 16-47, strumenti delle
// consolle (grafica/consolle), pannelli composti (grafica/sale).
#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "grafica/consolle.hpp"
#include "grafica/font.hpp"
#include "grafica/sale.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {
const std::string FONT = std::string(M688_DATI) + "/font/";
const bool CON_FONT = fs::exists(FONT + "fosforo688_5x7.fnt");
const char* const FONT_STAZIONI[] = {"fosforo688_5x7", "fosforo688_7x9", "incisione688_7", "incisione688_5",
                                     "stampino_urss_10", "stampino_urss_10_ponti", "stampino_urss_10_inclinato",
                                     "stampino_urss_7", "stampino_urss_7_ponti", "stampino_urss_7_inclinato",
                                     "cifre_5x9", "cifre_6x11"};

int conta(const Tela& t, std::uint8_t c) {
    int n = 0;
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) n += t.at(x, y) == c;
    return n;
}
}  // namespace

TEST_CASE("font: l'originale (FNT1) si disegna come i suoi bit, con a acuta e paragrafo") {
    const Font f = Font::carica(FONT + "originale.fnt");
    CHECK(f.formato() == 1);
    CHECK(f.ha(U'á'));
    CHECK(f.ha(U'§'));
    // i pixel della tela sono i bit del file, glifo per glifo (scala 1, senza allineamento alla base)
    std::ifstream in(FONT + "originale.fnt", std::ios::binary);
    const std::vector<unsigned char> d((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const int n = d[4] | (d[5] << 8), righe = d[6];
    Font g = f;
    g.allinea_base(false);
    int controllati = 0;
    for (std::size_t o = 7, k = 0; k < static_cast<std::size_t>(n); ++k, o += 5 + static_cast<std::size_t>(righe)) {
        const char32_t cp = static_cast<char32_t>(d[o] | (d[o + 1] << 8) | (d[o + 2] << 16) | (d[o + 3] << 24));
        Tela t;
        t.riempi(0);
        std::string s;   // il codepoint in UTF-8
        if (cp < 0x80) s = std::string(1, static_cast<char>(cp));
        else if (cp < 0x800) s = {static_cast<char>(0xc0 | (cp >> 6)), static_cast<char>(0x80 | (cp & 0x3f))};
        else s = {static_cast<char>(0xe0 | (cp >> 12)), static_cast<char>(0x80 | ((cp >> 6) & 0x3f)), static_cast<char>(0x80 | (cp & 0x3f))};
        g.scrivi(t, 10, 10, s, 9, 1, 1, false);
        bool uguale = true;
        for (int r = 0; r < righe; ++r)
            for (int c = 0; c < 8; ++c) uguale &= (t.at(10 + c, 10 + r) == 9) == ((d[o + 5 + static_cast<std::size_t>(r)] & (0x80 >> c)) != 0);
        CHECK_MESSAGE(uguale, "glifo ", static_cast<int>(cp));
        ++controllati;
    }
    CHECK(controllati == n);   // tutti: ASCII, accentate, simboli, icone e cifre grandi dell'originale (U+E080-U+E0FF)
}

TEST_CASE("font: le lettere che il font non ha si scrivono con la riserva latina") {
    const Font f = Font::carica(FONT + "originale.fnt");
    const auto testo = [&](std::string_view s) {
        std::string out;
        for (const char32_t cp : f.caratteri(s)) out += cp < 0x80 ? static_cast<char>(cp) : '#';
        return out;
    };
    // le lettere senza glifo: Æ AE, Ø O, ß SS, Š S, Ł L; le minuscole allo stesso modo
    CHECK(!f.ha(U'Æ'));
    CHECK(testo("FÆR ØER") == "FAER OER");
    // la tipografia delle decorazioni sovietiche e delle schede: virgolette basse, ordinali, segni (trailer-1, 29/10/2026)
    CHECK(testo("MEDAGLIA «PER DISTINZIONE NEL SERVIZIO MILITARE» DI 1ª CLASSE") ==
          "MEDAGLIA \"PER DISTINZIONE NEL SERVIZIO MILITARE\" DI 1A CLASSE");
    CHECK(testo("2º GRADO — ≥ 5 m²") == "2O GRADO - >= 5 m2");
    CHECK(testo("Fær Øer") == "Faer Oer");
    CHECK(testo("Gadžievo, Łeba, Straße") == "Gadzievo, Leba, Strasse");
    // le accentate italiane del font restano com'erano, e cosi' l'ASCII
    CHECK(f.caratteri("città").back() == U'à');
    CHECK(testo("TORPEX 89") == "TORPEX 89");
    // la larghezza e il disegno seguono la riserva
    CHECK(f.larghezza("FÆR ØER", 1) == f.larghezza("FAER OER", 1));
    Tela a, b;
    a.riempi(0);
    b.riempi(0);
    f.scrivi(a, 10, 10, "FÆR ØER", 9, 1);
    f.scrivi(b, 10, 10, "FAER OER", 9, 1);
    bool uguali = true;
    for (int y = 0; y < 40; ++y)
        for (int x = 0; x < 200; ++x) uguali &= a.at(x, y) == b.at(x, y);
    CHECK(uguali);
}

TEST_CASE("font delle stazioni (FNT2): caricati, solo maiuscole, toni" * doctest::skip(!CON_FONT)) {
    for (const char* nome : FONT_STAZIONI) {
        const Font f = Font::carica(FONT + nome + ".fnt");
        CHECK(f.formato() == 2);
        CHECK(f.ha(U'?'));
        CHECK(f.ha(U'0'));
        CHECK(f.ha(U'9'));
    }
    const Font f = Font::carica(FONT + "fosforo688_5x7.fnt");
    // le minuscole e le accentate minuscole si scrivono con le maiuscole
    CHECK(f.larghezza("rotta") == f.larghezza("ROTTA"));
    Tela a, b;
    a.riempi(0);
    b.riempi(0);
    f.scrivi(a, 0, 0, "città", 15, 1);
    f.scrivi(b, 0, 0, "CITTÀ", 15, 1);
    CHECK(conta(a, 15) == conta(b, 15));
    CHECK(conta(a, 15) > 40);
    // terminale: tutti i glifi larghi 5, con lo spazio di 1 (cella di 6)
    CHECK(f.larghezza("I", 1) == 5);
    CHECK(f.larghezza("W", 1) == 5);
    CHECK(f.larghezza("IW", 1) == 11);
    // scrivi(): solo il tratto; scrivi_toni(): anche l'alone del fosforo nel primo tono
    Tela t, u;
    t.riempi(0);
    u.riempi(0);
    f.scrivi(t, 20, 20, "SONAR", 2, 1);
    f.scrivi_toni(u, 20, 20, "SONAR", {1, 2, 3}, 1);
    CHECK(conta(t, 2) == conta(u, 2));
    CHECK(conta(t, 1) == 0);
    CHECK(conta(u, 1) > conta(u, 2));   // l'alone circonda il tratto
    // incisione: proporzionale (la I piu' stretta della M), capi dei tratti a mezzo tono
    const Font g = Font::carica(FONT + "incisione688_7.fnt");
    CHECK(g.larghezza("I", 1) < g.larghezza("M", 1));
    Tela v;
    v.riempi(0);
    g.scrivi_toni(v, 0, 0, "T", {1, 2, 3}, 1);
    CHECK(conta(v, 1) >= 3);   // i capi della T
    // stampino: le larghezze del GOST (E 5, normali 6, M 7, W 8) e lo spazio di 2
    const Font s = Font::carica(FONT + "stampino_urss_10.fnt");
    CHECK(s.spazio_predefinito() == 2);
    CHECK(s.larghezza("E", 1) == 5);
    CHECK(s.larghezza("N", 1) == 6);
    CHECK(s.larghezza("M", 1) == 7);
    CHECK(s.larghezza("W", 1) == 8);
    // inclinato: stesso avanzamento del dritto, pixel spostati a destra in alto
    const Font si = Font::carica(FONT + "stampino_urss_10_inclinato.fnt");
    CHECK(si.larghezza("AKKORD", 1, 2) == s.larghezza("AKKORD", 1, 2));
    // ponticelli: la O dello stampino ha meno pixel di quella intera
    const Font sp = Font::carica(FONT + "stampino_urss_10_ponti.fnt");
    Tela o1, o2;
    o1.riempi(0);
    o2.riempi(0);
    s.scrivi(o1, 0, 0, "O", 7, 1);
    sp.scrivi(o2, 0, 0, "O", 7, 1);
    CHECK(conta(o2, 7) == conta(o1, 7) - 2);
    // cifre a 7 segmenti: l'8 acceso del tutto, l'1 con i segmenti spenti al mezzo tono
    const Font c = Font::carica(FONT + "cifre_5x9.fnt");
    Tela s8, s1;
    s8.riempi(0);
    s1.riempi(0);
    c.scrivi_toni(s8, 0, 0, "8", {1, 2, 3}, 1);
    c.scrivi_toni(s1, 0, 0, "1", {1, 2, 3}, 1);
    CHECK(conta(s8, 1) == 0);
    CHECK(conta(s1, 2) + conta(s1, 1) == conta(s8, 2));
    CHECK(conta(s1, 2) < conta(s1, 1));
}

TEST_CASE("temi delle stazioni: solo gli indici 16-47, luce della sala sulle superfici") {
    const Palette base = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    Palette a = base, b = base;
    applica_tema(a, {BattelloTema::usa688, LuceSala::bianca, Fosforo::del_battello});
    applica_tema(b, {BattelloTema::alfa, LuceSala::bianca, Fosforo::del_battello});
    for (int i = 0; i < 256; ++i)
        if (i < colori_tema::PRIMO || i > colori_tema::ULTIMO) {
            REQUIRE(a.rgb(i) == base.rgb(i));
            REQUIRE(b.rgb(i) == base.rgb(i));
        }
    // smalto dal piu' scuro al piu' chiaro; 688 grigio (r = g = b circa), Alfa crema in alto (r > b)
    const auto lum = [](std::array<std::uint8_t, 3> c) { return 0.30 * c[0] + 0.59 * c[1] + 0.11 * c[2]; };
    for (int i = 1; i < colori_tema::N_SMALTO; ++i) CHECK(lum(a.rgb(smalto(i))) > lum(a.rgb(smalto(i - 1))));
    CHECK(std::abs(a.rgb(smalto(8))[0] - a.rgb(smalto(8))[2]) < 12);
    CHECK(b.rgb(smalto(11))[0] > b.rgb(smalto(11))[2] + 20);
    // targhette: nero il fondo del 688, rosso quello dell'Alfa
    CHECK(lum(a.rgb(colori_tema::TARGHETTA)) < 40);
    CHECK(b.rgb(colori_tema::TARGHETTA)[0] > 2 * b.rgb(colori_tema::TARGHETTA)[1]);
    // fosforo: verde sul 688, verde giallino sull'Alfa (piu' rosso a parita' di verde), ambra a richiesta
    const auto na = a.rgb(fosforo(6)), nb = b.rgb(fosforo(6));
    CHECK(na[1] > na[0] + 60);
    CHECK(nb[0] > na[0] + 60);
    Palette c = base;
    applica_tema(c, {BattelloTema::usa688, LuceSala::bianca, Fosforo::ambra});
    CHECK(c.rgb(fosforo(6))[0] > c.rgb(fosforo(6))[2] + 100);
    // luce rossa e blu: cambiano smalto e targhette, non lampade ne' fosforo
    Palette r = base, u = base;
    applica_tema(r, {BattelloTema::usa688, LuceSala::rossa, Fosforo::del_battello});
    applica_tema(u, {BattelloTema::usa688, LuceSala::blu, Fosforo::del_battello});
    CHECK(r.rgb(smalto(8))[0] > 3 * r.rgb(smalto(8))[1]);
    CHECK(u.rgb(smalto(8))[2] > u.rgb(smalto(8))[0] + 20);
    for (int i = colori_tema::LAMPADE; i <= colori_tema::FOSFORO + colori_tema::N_FOSFORO - 1; ++i)
        if (i < colori_tema::TARGHETTA || i > colori_tema::TARGHETTA + 3) {
            CHECK(r.rgb(i) == a.rgb(i));
            CHECK(u.rgb(i) == a.rgb(i));
        }
    // lampade: accesa piu' luminosa della spenta
    for (const auto l : {ColoreLampada::rossa, ColoreLampada::ambra, ColoreLampada::verde})
        CHECK(lum(a.rgb(lampada(l, true))) > 2 * lum(a.rgb(lampada(l, false))));
}

namespace {
// i pixel diversi dal fondo stanno dentro r (allargato di `margine`) e sono del tema, neri o bianchi
bool dentro_e_del_tema(const Tela& t, Rettangolo r, std::uint8_t fondo, int margine = 0) {
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) {
            const int c = t.at(x, y);
            if (c == fondo) continue;
            if (x < r.x - margine || y < r.y - margine || x >= r.x + r.w + margine || y >= r.y + r.h + margine) return false;
            if (!(c == colori::NERO || c == colori::BIANCO || (c >= colori_tema::PRIMO && c <= colori_tema::ULTIMO))) return false;
        }
    return true;
}
}  // namespace

TEST_CASE("strumenti delle consolle: nel loro rettangolo, coi colori del tema" * doctest::skip(!CON_FONT)) {
    const Font targa = Font::carica(FONT + "incisione688_7.fnt"), stamp = Font::carica(FONT + "stampino_urss_7.fnt"),
               cifre = Font::carica(FONT + "cifre_6x11.fnt"), fos = Font::carica(FONT + "fosforo688_5x7.fnt");
    const std::uint8_t F = 255;
    auto prova = [&](Rettangolo r, int margine, auto&& disegna) {
        Tela t;
        t.riempi(F);
        disegna(t);
        return dentro_e_del_tema(t, r, F, margine);
    };
    CHECK(prova({20, 20, 200, 100}, 0, [&](Tela& t) { pannello(t, {20, 20, 200, 100}, BattelloTema::usa688); }));
    CHECK(prova({20, 20, 200, 100}, 0, [&](Tela& t) { pannello(t, {20, 20, 200, 100}, BattelloTema::alfa); }));
    CHECK(prova({30, 30, 120, 12}, 0, [&](Tela& t) { targhetta(t, {30, 30, 120, 12}, "BALLAST CONTROL", targa); }));
    for (const auto st : {StileQuadrante::scuro_688, StileQuadrante::bianco_alfa})
        CHECK(prova({100 - 40, 100 - 40, 81, 81}, 1, [&](Tela& t) {
            const double c = 30.0;
            quadrante(t, 100, 100, 40, 200.0, {}, st, &targa, &c);
        }));
    CHECK(prova({50, 50, 14, 70}, 3, [&](Tela& t) { barra(t, {50, 50, 14, 70}, 0.5, StileQuadrante::scuro_688, 0.7); }));
    CHECK(prova({10, 10, 40, 22}, 0, [&](Tela& t) { lampada_rettangolare(t, {10, 10, 40, 22}, ColoreLampada::ambra, true); }));
    CHECK(prova({100 - 6, 100 - 6, 13, 13}, 1, [&](Tela& t) { lampada_tonda(t, 100, 100, 6, ColoreLampada::verde, true); }));
    CHECK(prova({10, 10, 140, 22}, 0, [&](Tela& t) { finestrella(t, {10, 10, 140, 22}, "AKKORD AVARIJA", true, stamp); }));
    CHECK(prova({10, 10, 50, 30}, 0, [&](Tela& t) { pulsante(t, {10, 10, 50, 30}, "PROGR", ColoreLampada::ambra, true, true, stamp); }));
    CHECK(prova({10, 10, 60, 60}, 0, [&](Tela& t) { pulsante_coperto(t, {10, 10, 60, 60}, false, false); }));
    CHECK(prova({100 - 11, 100 - 11, 23, 23}, 1, [&](Tela& t) { manopola(t, 100, 100, 11, 45.0); }));
    CHECK(prova({100 - 3, 100 - 9, 7, 19}, 1, [&](Tela& t) { levetta(t, 100, 100, true); }));
    CHECK(prova({10, 10, 300, 236}, 0, [&](Tela& t) { crt_rettangolare(t, {10, 10, 300, 236}); }));
    CHECK(prova({200 - 101, 200 - 101, 203, 203}, 1, [&](Tela& t) { crt_tondo(t, 200, 200, 92); }));
    CHECK(prova({20, 20, 90, 85}, 3, [&](Tela& t) { cloche(t, {20, 20, 90, 85}, 0.5, -0.5); }));
    // il vetro del CRT: dentro il rettangolo, spento (fosforo 0) al centro
    Tela t;
    t.riempi(F);
    const Rettangolo v = crt_rettangolare(t, {10, 10, 300, 236});
    CHECK(v.x > 10);
    CHECK(v.x + v.w < 310);
    CHECK(v.y + v.h < 246);
    CHECK(t.at(v.x + v.w / 2, v.y + v.h / 2) == fosforo(0));
    const Rettangolo d = crt_tondo(t, 400, 300, 60);
    CHECK(d.w == 121);
    CHECK(t.at(400, 300) == fosforo(0));
    testo_fosforo(t, v.x + 4, v.y + 4, "BQQ-5", fos);
    CHECK(conta(t, fosforo(5)) > 20);
    CHECK(conta(t, fosforo(2)) > conta(t, fosforo(5)));   // l'alone attorno al tratto
}

TEST_CASE("strumenti delle consolle: indici, lampade, tamburi" * doctest::skip(!CON_FONT)) {
    const Font targa = Font::carica(FONT + "incisione688_7.fnt");
    const std::uint8_t ago = smalto(11);
    // l'indice del quadrante bussola: a 90 gradi punta a destra, a 270 a sinistra
    for (const double v : {90.0, 270.0}) {
        Tela t;
        t.riempi(255);
        quadrante(t, 200, 200, 40, v, {0, 360, 0, 360, 36, 0, 0.1}, StileQuadrante::scuro_688);
        int destra = 0, sinistra = 0;
        for (int x = 210; x < 230; ++x) destra += t.at(x, 200) == ago;
        for (int x = 170; x < 190; ++x) sinistra += t.at(x, 200) == ago;
        CHECK((v == 90.0 ? destra > 10 && sinistra == 0 : sinistra > 10 && destra == 0));
    }
    // albero di Natale: un tondo rosso per apertura aperta, barrette verdi per le chiuse
    Tela a;
    a.riempi(255);
    std::vector<bool> ap(16, false);
    ap[2] = ap[9] = ap[15] = true;
    albero_di_natale(a, {10, 10, 160, 60}, ap, 8);
    CHECK(conta(a, lampada(ColoreLampada::rossa, true)) >= 3 * 5);
    CHECK(conta(a, lampada(ColoreLampada::verde, true)) >= 13 * 4);
    // tamburo: il rotolamento cambia solo l'ultima cifra
    Tela t0, t1;
    t0.riempi(255);
    t1.riempi(255);
    tamburo(t0, 20, 20, "0150", 0.0, targa);
    tamburo(t1, 20, 20, "0150", 0.5, targa);
    const int cw = targa.larghezza("0", 1, 1, false) + 3;
    bool prime_uguali = true, ultima_diversa = false;
    for (int y = 0; y < 60; ++y)
        for (int x = 0; x < 200; ++x) {
            const bool diverso = t0.at(x, y) != t1.at(x, y);
            if (x < 20 + 3 * cw - 1) prime_uguali &= !diverso;
            else ultima_diversa |= diverso;
        }
    CHECK(prime_uguali);
    CHECK(ultima_diversa);
    // 7 segmenti: dentro la finestrella nera, accesi del colore e spenti in rosso scurissimo
    const Font cifre = Font::carica(FONT + "cifre_6x11.fnt");
    Tela s;
    s.riempi(255);
    sette_segmenti(s, 30, 30, "1", ColoreLampada::verde, cifre);
    CHECK(conta(s, lampada(ColoreLampada::verde, true)) > 0);
    CHECK(conta(s, lampada(ColoreLampada::rossa, false)) > conta(s, lampada(ColoreLampada::verde, true)));
    CHECK(conta(s, colori::NERO) > 0);
}

TEST_CASE("pannelli di governo e armi: nel rettangolo, aree coi comandi della mappa dei tasti" * doctest::skip(!CON_FONT)) {
    // comandi di nucleo/mappa_tasti usati dai pannelli («tubo_» + numero vale per ogni tubo)
    const std::vector<std::string> noti = {"pilota_automatico", "timone_sinistra", "timone_centro", "timone_dritta",
                                           "barra_su", "barra_zero", "barra_giu", "telegrafo_avanti", "telegrafo_indietro",
                                           "emersione_emergenza", "lancia", "tubo_1", "tubo_2", "tubo_3", "tubo_4", "tubo_5",
                                           "tubo_6",
                                           // Alfa: Boksit, ПОКС, Akkord
                                           "boksit_rotta", "boksit_quota", "boksit_tappa", "tan", "sfoga", "compensa_piu",
                                           "compensa_meno", "akkord_designa", "akkord_1", "akkord_2", "akkord_3"};
    for (const auto b : {BattelloTema::usa688, BattelloTema::alfa}) {
        const FontConsolle f = FontConsolle::carica(M688_DATI, b);
        const TemaStazione tema{b, LuceSala::bianca, Fosforo::del_battello};
        DatiGoverno g;
        g.campana = 6;
        g.aperture = std::vector<bool>(16, false);
        DatiArmi a;
        a.tubi.resize(b == BattelloTema::alfa ? 6 : 4);
        a.tubi[0].pronto = a.tubi[0].porta = true;
        for (const int h : {88, 120}) {
            const Rettangolo r{0, 360, 640, h};
            Tela t;
            t.riempi(255);
            std::vector<AreaPulto> aree;
            pannello_governo(t, r, tema, f, g, &aree);
            CHECK(dentro_e_del_tema(t, r, 255));
            pannello_armi(t, r, tema, f, a, &aree);
            for (int y = 0; y < r.y; ++y)
                for (int x = 0; x < LARGHEZZA_TELA; ++x) REQUIRE(t.at(x, y) == 255);
            int telegrafo = 0, tubi = 0, lancia = 0;
            for (const auto& ar : aree) {
                CHECK(ar.r.x >= r.x);
                CHECK(ar.r.y >= r.y);
                CHECK(ar.r.x + ar.r.w <= r.x + r.w);
                CHECK(ar.r.y + ar.r.h <= r.y + r.h);
                CHECK_MESSAGE((ar.comando.empty() || std::find(noti.begin(), noti.end(), ar.comando) != noti.end()), ar.comando);
                telegrafo += ar.comando.rfind("telegrafo_", 0) == 0;
                tubi += ar.comando.rfind("tubo_", 0) == 0;
                lancia += ar.comando == "lancia";
            }
            CHECK(telegrafo == 8);   // le campane sopra e sotto quella ordinata
            CHECK(tubi == static_cast<int>(a.tubi.size()));
            CHECK(lancia == 1);
        }
        // l'avaria di Akkord toglie il lancio all'Alfa (il 688 spara anche senza il Mk 81)
        a.akkord_guasto = true;
        Tela t;
        std::vector<AreaPulto> aree;
        pannello_armi(t, {0, 360, 640, 120}, tema, f, a, &aree);
        const bool lancia = std::any_of(aree.begin(), aree.end(), [](const AreaPulto& x) { return x.comando == "lancia"; });
        CHECK(lancia == (b == BattelloTema::usa688));
    }
}

TEST_CASE("centrale, cornici, Okean, Ritm e radio: nei loro spazi" * doctest::skip(!CON_FONT)) {
    const std::vector<std::string> stazioni = {"stazione_manovra", "stazione_stato", "stazione_radio", "stazione_armi",
                                               "stazione_sonar", "stazione_navigazione", "stazione_periscopio"};
    for (const auto b : {BattelloTema::usa688, BattelloTema::alfa}) {
        const FontConsolle f = FontConsolle::carica(M688_DATI, b);
        const TemaStazione tema{b, LuceSala::bianca, Fosforo::del_battello};
        // la centrale: solo nelle 360 righe della vista, aree coi comandi delle stazioni, una per ciascuna almeno
        Tela t;
        t.riempi(255);
        std::vector<AreaPulto> aree;
        vista_centrale(t, tema, f, 10.0, &aree);
        for (int y = 360; y < ALTEZZA_TELA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) REQUIRE(t.at(x, y) == 255);
        for (const auto& st : stazioni)
            CHECK_MESSAGE(std::any_of(aree.begin(), aree.end(), [&](const AreaPulto& a) { return a.comando == st; }), st);
        for (const auto& a : aree) {
            CHECK((a.comando.empty() || std::find(stazioni.begin(), stazioni.end(), a.comando) != stazioni.end()));
            CHECK(a.r.y + a.r.h <= 360);
        }
        // la cornice del periscopio non tocca l'oculare (cerchio di raggio 150 attorno a 320, 180)
        Tela c;
        c.riempi(255);
        cornice_periscopio(c, tema, f, false, 2);
        bool libero = true;
        for (int y = 30; y <= 330; ++y)
            for (int x = 170; x <= 470; ++x)
                if ((x - 320) * (x - 320) + (y - 180) * (y - 180) <= 150 * 150) libero &= c.at(x, y) == 255;
        CHECK(libero);
        // Okean: dentro la sua cappa
        Tela o;
        o.riempi(255);
        schermo_okean(o, 200, 200, 60, std::vector<float>(360, 0.5f), 30.0, f);
        CHECK(dentro_e_del_tema(o, {200 - 69, 200 - 69, 139, 139}, 255, 1));
        // pannelli di stato (Alfa) e radio: nel rettangolo
        const Rettangolo r{0, 364, 640, 112};
        DatiStato st;
        st.sinottici = {{"GAMMA", {ColoreLampada::verde, ColoreLampada::rossa}, {true, false}}};
        st.allarmi = {"T OTSEK > 60"};
        Tela p1;
        p1.riempi(255);
        pannello_stato_alfa(p1, r, tema, f, st);
        CHECK(dentro_e_del_tema(p1, r, 255));
        DatiRadioEsm re;
        re.righe_telescrivente = {"FM COMSUBLANT", "BT"};
        re.panoramica = std::vector<float>(100, 0.3f);
        re.intercettazioni = {"045 X SPS-49"};
        Tela p2;
        p2.riempi(255);
        pannello_radio_esm(p2, r, tema, f, re);
        CHECK(dentro_e_del_tema(p2, r, 255));
        // la variante coi pulsanti della sala radio: un'area per pulsante col suo comando, tutto nel rettangolo (88 righe)
        re.pulsanti = {{"FILO", "pulsante_1", true}, {"SSIXS", "pulsante_2"}, {"SLOT", "pulsante_3"}, {"TRASM", "pulsante_4"},
                       {"BOA", "pulsante_5"}, {"ARCHIV", "pulsante_6"}};
        const Rettangolo r88{0, 374, 640, 88};
        Tela p3;
        p3.riempi(255);
        std::vector<AreaPulto> ap;
        pannello_radio_esm(p3, r88, tema, f, re, &ap);
        CHECK(dentro_e_del_tema(p3, r88, 255));
        int pulsanti = 0;
        for (const auto& x : ap)
            if (x.comando.rfind("pulsante_", 0) == 0) {
                ++pulsanti;
                CHECK(x.r.y + x.r.h <= r88.y + r88.h);
                CHECK(x.r.x + x.r.w <= r88.x + r88.w);
            }
        CHECK(pulsanti == 6);
    }
    // la sala sonar del 688: il rettangolo libero per le cascate dentro la vista, il vetro scuro intorno
    const FontConsolle f = FontConsolle::carica(M688_DATI, BattelloTema::usa688);
    Tela s;
    s.riempi(255);
    const Rettangolo l = cornice_sala_sonar(s, {BattelloTema::usa688, LuceSala::blu, Fosforo::del_battello}, f);
    CHECK(l.x > 0);
    CHECK(l.y > 0);
    CHECK(l.x + l.w < LARGHEZZA_TELA);
    CHECK(l.y + l.h < 360);
    CHECK(s.at(l.x - 1, l.y + 10) == colori_tema::ACCENTO);
}

TEST_CASE("testo nel vetro: a capo a parole intere, righe in piu' tagliate, dentro il vetro" * doctest::skip(!CON_FONT)) {
    const Font f = Font::carica(FONT + "fosforo688_5x7.fnt");
    // a capo: nessuna riga piu' larga del vetro, le parole intere quando ci stanno
    const auto r = a_capo("(DAL LANCIO) [ROTTA 000] PASSIVO VEL. ALTA, QUOTA 100 M", f, 60);
    REQUIRE(r.size() > 1);
    for (const auto& x : r) CHECK(f.larghezza(x, 1, 1, false) <= 60);
    CHECK(r[0] == "(DAL");   // 60 pixel = 10 caratteri da 6: «(DAL LANCIO)» non ci sta intera
    CHECK(r[1] == "LANCIO)");
    // una parola piu' lunga del vetro si spezza
    const auto lunga = a_capo("ABCDEFGHIJKLMNOPQRSTUVWXYZ", f, 30);
    CHECK(lunga.size() == 6);
    for (const auto& x : lunga) CHECK(f.larghezza(x, 1, 1, false) <= 30);
    // nel vetro: niente fuori dal rettangolo, le righe in piu' non scritte
    Tela t;
    t.riempi(255);
    const Rettangolo v{100, 100, 80, 30};
    const int scritte = testo_fosforo_nel_vetro(t, v, {"(DAL LANCIO) [ROTTA 000] PASSIVO VEL. ALTA", "SECONDA RIGA", "TERZA"}, f);
    CHECK(scritte == 2);   // 30 righe di pixel, dalla 2: due righe da 10 (cella di 9 + 1), la terza non ci sta
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            if (t.at(x, y) != 255) REQUIRE((x >= v.x && x < v.x + v.w && y >= v.y && y < v.y + v.h));
}

TEST_CASE("finestrella spenta: vetro scuro e scritta scura, nessun colore acceso" * doctest::skip(!CON_FONT)) {
    const Font f = Font::carica(FONT + "stampino_urss_7.fnt");
    Tela t;
    t.riempi(255);
    finestrella(t, {10, 10, 140, 14}, "AKKORD AVARIJA", false, f);
    CHECK(conta(t, lampada(ColoreLampada::rossa, true)) == 0);
    CHECK(conta(t, colori::BIANCO) == 0);
    CHECK(conta(t, lampada(ColoreLampada::rossa, false)) > 500);
    Tela u;
    u.riempi(255);
    finestrella(u, {10, 10, 140, 14}, "AKKORD AVARIJA", true, f);
    CHECK(conta(u, lampada(ColoreLampada::rossa, true)) > 500);
    CHECK(conta(u, colori::BIANCO) > 20);
}

TEST_CASE("Sozh: il carrello sulla carta e' una riga puntinata, le X dei contatti restano visibili" * doctest::skip(!CON_FONT)) {
    const FontConsolle f = FontConsolle::carica(M688_DATI, BattelloTema::alfa);
    Tela t;
    t.riempi(9);
    const Rettangolo carta{40, 40, 560, 290};
    cornice_sozh(t, carta, {BattelloTema::alfa, LuceSala::bianca, Fosforo::del_battello}, f, 300, 140);
    int coperti = 0;
    for (int x = carta.x; x < carta.x + carta.w; ++x)
        for (int y = carta.y + 135; y <= carta.y + 145; ++y) coperti += t.at(x, y) != 9;
    CHECK(coperti < carta.w / 2 + 120);   // la riga a puntini e la testina del pennino, non una barra piena
}
