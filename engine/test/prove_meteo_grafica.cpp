// Meteo nella vista dell'Enhanced (docs/15_meteo_e_mare.md, M3): cielo, foschia, fulmini, fumo di mare, pioggia e
// neve, moto delle navi, elicotteri col vento, conversione dallo stato del meteo.
#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "grafica/cielo.hpp"
#include "grafica/effetti.hpp"
#include "grafica/moto_navi.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/strati_meteo.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/meteo.hpp"
#include "nucleo/sagoma_notte.hpp"
#include "nucleo/sole.hpp"

using namespace m688;

namespace {
// cielo sopra (199), mare sotto (110) attorno all'orizzonte della riga 180
Tela sfondo() {
    Tela t;
    for (int y = 0; y < ALTEZZA_VISTA; ++y) t.linea_orizzontale(0, LARGHEZZA_TELA - 1, y, static_cast<std::uint8_t>(y < 180 ? 199 : 110));
    return t;
}
GeometriaVista q15() {
    GeometriaVista q;
    q.ppr = 640.0 / radianti(32.0);
    return q;
}
int conta(const Tela& t, int y0, int y1, bool (*f)(int)) {
    int n = 0;
    for (int y = y0; y < y1; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) n += f(t.at(x, y));
    return n;
}
bool grigio(int c) { return c >= colori::GRIGI && c <= colori::GRIGI + 14; }
bool bianco(int c) { return c == colori::BIANCO; }
bool mare(int c) { return c == 110; }
}  // namespace

TEST_CASE("meteo spento: la vista e' quella di oggi, pixel per pixel") {
    // senza risorse: la vista vuota (cielo, mare, reticolo) con e senza i campi del meteo riempiti ma spenti
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    std::map<std::string, ClasseNave> classi;
    std::map<std::string, InsiemeSprite> sprite;
    Periscopio p(f, classi, sprite);
    Mondo m;
    Nave occhio;
    VistaPeriscopio a, b;
    b.meteo.nubi = 1.0;
    b.meteo.pioggia_mm_h = 20.0;
    b.meteo.lampi.push_back({0.0, 0.0, 3000.0, 4, true, 3});
    Tela ta, tb;
    p.disegna(ta, m, occhio, a);
    p.disegna(tb, m, occhio, b);
    bool uguali = true;
    for (int y = 0; y < ALTEZZA_VISTA && uguali; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) uguali = uguali && ta.at(x, y) == tb.at(x, y);
    CHECK(uguali);
}

TEST_CASE("nubi: copertura dal sereno al coperto, solo sul cielo, cumulonembi piu' scuri") {
    MeteoVista m;
    m.attivo = true;
    auto disegna = [&](double nubi, double temporale) {
        Tela t = sfondo();
        m.nubi = nubi;
        m.temporale = temporale;
        cielo::nubi(t, q15(), m, 100.0);
        return t;
    };
    const Tela sereno = disegna(0.0, 0.0), sparse = disegna(0.3, 0.0), coperto = disegna(0.97, 0.0);
    const int cielo_px = 150 * LARGHEZZA_TELA;
    CHECK(conta(sereno, 0, 150, grigio) == 0);
    CHECK(conta(sparse, 0, 150, grigio) > cielo_px / 20);
    CHECK(conta(sparse, 0, 150, grigio) < cielo_px * 6 / 10);
    CHECK(conta(coperto, 0, 150, grigio) > cielo_px * 8 / 10);
    CHECK(conta(coperto, 185, ALTEZZA_VISTA, mare) == (ALTEZZA_VISTA - 185) * LARGHEZZA_TELA);   // il mare resta
    // la base dei cumulonembi e' piu' scura
    const Tela cb = disegna(0.97, 1.0);
    double s1 = 0, s2 = 0;
    for (int x = 0; x < LARGHEZZA_TELA; ++x) { s1 += coperto.at(x, 100); s2 += cb.at(x, 100); }
    CHECK(s2 < s1 - 2 * LARGHEZZA_TELA);
}

TEST_CASE("nubi: niente colonne verticali verso l'orizzonte, anche nel campo stretto") {
    MeteoVista m;
    m.attivo = true;
    m.nubi = 0.5;
    m.plafond_m = 1500.0;
    GeometriaVista stretto;
    stretto.ppr = 640.0 / radianti(8.0);
    const auto nube = [](const Tela& t, int x, int y) { return t.at(x, y) != 199; };
    for (const GeometriaVista& q : {stretto, q15()}) {
        Tela t = sfondo();
        cielo::nubi(t, q, m, 100.0);
        // due righe con la stessa soglia del retino, sopra e sotto il punto in cui la distanza era bloccata (60 km): prima uguali
        const int y0 = static_cast<int>(q.riga_elevazione(std::atan(1500.0 / 50000.0))), y1 = y0 + 4 * std::max(1, (179 - y0) / 8);
        int diverse = 0;
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diverse += nube(t, x, y1) != nube(t, x, y1 + 4);
        MESSAGE("ppr ", q.ppr, ": righe ", y1, " e ", y1 + 4, ", colonne diverse ", diverse);
        CHECK(diverse > LARGHEZZA_TELA / 20);
    }
    // radente all'orizzonte le nubi sparse spariscono, e nessun retino a griglia; il coperto resta un velo uniforme
    for (const double copertura : {0.125, 0.5, 0.97}) {
        m.nubi = copertura;
        Tela t = sfondo();
        cielo::nubi(t, stretto, m, 100.0);
        for (const int y : {172, 176}) {
            int n = 0;
            for (int x = 0; x < LARGHEZZA_TELA; ++x) n += nube(t, x, y);
            CHECK(n == (copertura > 0.9 ? LARGHEZZA_TELA : 0));
        }
    }
}

TEST_CASE("foschia e nebbia: niente col tempo limpido, mare lontano grigio con la nebbia") {
    MeteoVista m;
    m.attivo = true;
    Tela limpido = sfondo(), nebbia = sfondo();
    cielo::foschia(limpido, q15(), m, 45000.0);
    CHECK(conta(limpido, 0, ALTEZZA_VISTA, grigio) == 0);
    m.nebbia = NebbiaVista::avvezione;
    cielo::foschia(nebbia, q15(), m, 500.0);
    CHECK(conta(nebbia, 180, 184, grigio) > LARGHEZZA_TELA * 4 * 9 / 10);   // a pelo dell'orizzonte: tutto nebbia
    CHECK(conta(nebbia, 340, 360, grigio) < conta(nebbia, 180, 200, grigio));   // vicino si vede ancora il mare
}

TEST_CASE("fulmini: canale bianco durante i colpi, niente fra un colpo e l'altro, lampo piu' forte di notte") {
    MeteoVista m;
    m.attivo = true;
    m.plafond_m = 800.0;
    m.lampi.push_back({10.0, 0.0, 5000.0, 4, true, 11});
    Tela durante = sfondo(), fra = sfondo();
    cielo::fulmini(durante, q15(), m, 10.01);
    cielo::fulmini(fra, q15(), m, 10.045);   // dopo il primo colpo (30 ms), prima del secondo (57 ms)
    CHECK(conta(durante, 0, ALTEZZA_VISTA, bianco) > 20);
    CHECK(conta(fra, 0, ALTEZZA_VISTA, bianco) == 0);
    CHECK(cielo::lampo(m, 10.01, 0.05) > cielo::lampo(m, 10.01, 1.0) * 3.0);
    CHECK(cielo::lampo(m, 10.045, 0.05) == 0.0);
    CHECK(cielo::lampo(m, 10.0 + 4 * cielo::INTERVALLO_COLPI_S + 0.01, 0.05) == 0.0);   // dopo l'ultimo colpo
    // da lontano niente canale, solo il bagliore
    MeteoVista l = m;
    l.lampi[0].distanza_m = 60000.0;
    Tela lontano = sfondo();
    cielo::fulmini(lontano, q15(), l, 10.01);
    CHECK(conta(lontano, 0, ALTEZZA_VISTA, bianco) == 0);
    CHECK(conta(lontano, 0, 180, grigio) > 0);
}

TEST_CASE("fumo di mare: una fascia bassa sull'orizzonte, il cielo alto resta") {
    MeteoVista m;
    m.attivo = true;
    m.nebbia = NebbiaVista::fumo_di_mare;
    m.strato_nebbia_m = 30.0;
    Tela t = sfondo();
    GeometriaVista q = q15();
    q.ppr = 640.0 / radianti(8.0);   // a 6x la fascia e' piu' alta
    cielo::fumo_di_mare(t, q, m, 5.0);
    CHECK(conta(t, 160, 185, grigio) > 300);
    CHECK(conta(t, 0, 120, grigio) == 0);
}

TEST_CASE("pioggia e neve: piu' fitte con l'intensita'") {
    MeteoVista m;
    m.attivo = true;
    auto gocce = [&](double pioggia, double neve, int c) {
        Tela t;
        t.riempi(0);
        m.pioggia_mm_h = pioggia;
        m.neve_mm_h = neve;
        cielo::precipitazione(t, q15(), m, 3.3);
        int n = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) n += t.at(x, y) == c;
        return n;
    };
    CHECK(gocce(0.0, 0.0, cielo::PIOGGIA) == 0);
    CHECK(gocce(10.0, 0.0, cielo::PIOGGIA) > gocce(1.0, 0.0, cielo::PIOGGIA) * 3);
    CHECK(gocce(0.0, 2.0, cielo::NEVE) > 50);
}

TEST_CASE("moto delle navi: RMS della tabella, periodo d'incontro, angolo apparente") {
    double rr, bb;
    ampiezze_rms(170.0, 3.25, rr, bb);   // incrociatore, mare 5: rollio 4,5 RMS (DD-963)
    CHECK(rr == doctest::Approx(4.5));
    CHECK(bb == doctest::Approx(1.5));
    ampiezze_rms(59.0, 5.0, rr, bb);     // Nanuchka, mare 6
    CHECK(rr == doctest::Approx(10.0));
    auto rms = [](double mare_da, double rotta, bool rollio) {
        double s = 0;
        int n = 0;
        for (double t = 0; t < 3000; t += 0.25, ++n) {
            const MotoNave m = moto_nave(170.0, 17.0, 3.25, 0.0, mare_da, rotta, 8.0, t, 0.3);
            const double v = rollio ? m.rollio_gradi : m.beccheggio_gradi;
            s += v * v;
        }
        return std::sqrt(s / n);
    };
    CHECK(rms(90.0, 0.0, true) == doctest::Approx(4.5).epsilon(0.1));   // mare al traverso: rollio pieno
    CHECK(rms(0.0, 0.0, false) == doctest::Approx(1.5).epsilon(0.1));   // di prua: beccheggio pieno
    CHECK(rms(0.0, 0.0, true) < rms(90.0, 0.0, true) * 0.5);
    // col mare di prua il beccheggio e' piu' rapido che di poppa (periodo d'incontro)
    auto passaggi = [](double mare_da) {
        int n = 0;
        double prima = 0;
        for (double t = 0; t < 600; t += 0.1) {
            const double b = moto_nave(130.0, 13.0, 3.25, 0.0, mare_da, 0.0, 12.0, t, 0.0).beccheggio_gradi;
            n += (prima < 0) != (b < 0);
            prima = b;
        }
        return n;
    };
    CHECK(passaggi(0.0) > passaggi(180.0) * 1.3);
    CHECK(moto_nave(130.0, 13.0, 0.0, 0.0, 0.0, 0.0, 10.0, 5.0, 0.0).rollio_gradi == 0.0);
    // di fianco si vede il beccheggio, di prua il rollio
    MotoNave m;
    m.beccheggio_gradi = 3.0;
    m.rollio_gradi = 10.0;
    CHECK(angolo_apparente(m, 90.0, true) == doctest::Approx(radianti(3.0)));
    CHECK(angolo_apparente(m, 0.0, true) == doctest::Approx(radianti(10.0)));
    // la prua si pianta solo col mare grosso
    double spruzzo = 0;
    for (double t = 0; t < 200; t += 0.1) spruzzo = std::max(spruzzo, moto_nave(100.0, 12.0, 4.0, 0.0, 0.0, 0.0, 10.0, t, 0.0).spruzzo);
    CHECK(spruzzo > 0.3);
    for (double t = 0; t < 200; t += 0.1) CHECK(moto_nave(100.0, 12.0, 1.0, 0.0, 0.0, 0.0, 10.0, t, 0.0).spruzzo == 0.0);
}

TEST_CASE("sprite ruotato: identico senza angolo, gira attorno al galleggiamento") {
    Sprite s;
    s.w = 40; s.h = 10; s.ancora_x = 20; s.ancora_y = 10;
    s.pix.assign(400, static_cast<std::uint8_t>(colori::GRIGI + 5));
    Tela a, b, c;
    a.riempi(0); b.riempi(0); c.riempi(0);
    a.disegna_sprite(s, 300, 200, 2.0);
    disegna_sprite_ruotato(b, s, 300, 200, 2.0, 0.0, 1 << 30, 0.0);
    int diversi = 0, pieni_a = 0, pieni_c = 0;
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diversi += a.at(x, y) != b.at(x, y);
    CHECK(diversi == 0);
    disegna_sprite_ruotato(c, s, 300, 200, 2.0, radianti(10.0), 1 << 30, 0.0);
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) { pieni_a += a.at(x, y) != 0; pieni_c += c.at(x, y) != 0; }
    CHECK(std::abs(pieni_c - pieni_a) < pieni_a / 10);
    CHECK(c.at(300 + 35, 200 - 2 - static_cast<int>(35 * std::tan(radianti(10.0)))) != 0);   // la destra sale (antiorario)
    CHECK(c.at(300 + 38, 199) == 0);
}

TEST_CASE("elicottero in hover: il cavo pende sottovento e l'anello si sposta") {
    Mondo mondo;
    auto cavi = [&](double vento) {
        Effetti e(3);
        AereoAsw a;
        a.id = 1; a.pos = {0, 0}; a.quota_m = 15.0; a.elicottero = true; a.hover = true; a.rotore_m = 18.9;
        a.cavo_m = 60.0; a.vento_m_s = vento; a.vento_da_gradi = 0.0;   // vento da nord: sottovento e' il sud
        for (int i = 0; i < 30; ++i) { e.aerei_asw({a}); e.avanza(0.1, mondo); }
        double miny = 0;
        int n = 0;
        for (const auto& p : e.orli())
            if (p.materia == Materia::cavo) { miny = std::min(miny, p.y); ++n; }
        return std::pair{miny, n};
    };
    const auto [y0, n0] = cavi(0.0);
    const auto [y1, n1] = cavi(15.0);
    CHECK(n0 == 1);
    CHECK(y0 == 0.0);
    CHECK(n1 > 1);
    CHECK(y1 < -1.0);   // spostato a sud, sottovento
}

TEST_CASE("dallo stato del meteo alla vista") {
    StatoMeteo s;
    s.nubi_ottavi = 6.0;
    s.plafond_m = 400.0;
    s.vento_m_s = 15.0;
    s.vento_da_gradi = 300.0;
    s.hs_vento_m = 3.0;
    s.hs_swell_m = 4.0;
    s.tp_swell_s = 13.0;
    s.swell_da_gradi = 260.0;
    s.pioggia_mm_h = 4.0;
    s.visibilita_m = 6000.0;
    s.lampi_al_minuto = 2.0;
    s.nebbia = Nebbia::fumo_di_mare;
    s.strato_nebbia_m = 20.0;
    Lampo l;
    l.pos = {0.0, 8000.0};
    l.t = 12.0;
    l.colpi = 3;
    l.nube_mare = true;
    VistaPeriscopio v;
    applica_meteo(v, s, {l}, Vec2{0.0, 0.0});
    CHECK(v.meteo.attivo);
    CHECK(v.meteo.nubi == doctest::Approx(0.75));
    CHECK(v.meteo.temporale > 0.5);
    CHECK(v.altezza_onde_m == doctest::Approx(5.0));
    CHECK(v.meteo.mare_da_gradi == 260.0);   // domina lo swell
    CHECK(v.meteo.tp_s == 13.0);
    CHECK(v.visibilita_m == 6000.0);
    CHECK(v.meteo.nebbia == NebbiaVista::fumo_di_mare);
    REQUIRE(v.meteo.lampi.size() == 1);
    CHECK(v.meteo.lampi[0].distanza_m == doctest::Approx(8000.0));
    CHECK(v.meteo.lampi[0].rilevamento_gradi == doctest::Approx(0.0).epsilon(1e-6));
    CHECK(v.oculare.vento_kn == doctest::Approx(15.0 / 0.5144));
    CHECK(v.oculare.pioggia == doctest::Approx(0.5));
}

TEST_CASE("luna: la luce della notte dai lux dell'IA, il disco nel cielo e la scia sul mare sotto la luna") {
    // la luna piena alta (~0,25 lux) con l'occhio adattato: 0,22, piu' del crepuscolo equivalente (Sole a -8,5) e meno
    // di quello col Sole a -4; le stelle sole niente
    CHECK(cielo::luce_della_notte(0.25) == doctest::Approx(0.22));
    CHECK(cielo::luce_della_notte(0.25) > luce_ambiente(-8.5));
    CHECK(cielo::luce_della_notte(0.25) < luce_ambiente(-4.0));
    CHECK(cielo::luce_della_notte(0.002) < 0.01);
    CHECK(cielo::luce_della_notte(0.0) == 0.0);
    CHECK(cielo::luce_della_notte(0.05) < cielo::luce_della_notte(0.25));
    CHECK(cielo::luce_della_notte(1e6) == doctest::Approx(1.0));
    CieloNotte c;
    c.lux = 0.25;
    c.luna_su = true;
    c.luna_elevazione = 5.0;
    c.luna_azimut = 0.0;
    c.luna_illuminata = 1.0;
    GeometriaVista q = q15();
    Tela t = sfondo();
    cielo::luna(t, q, c, 0.05);
    // il disco a 5 gradi sopra la linea di mira, al centro
    const int yl = static_cast<int>(q.riga_elevazione(radianti(5.0)));
    CHECK(t.at(q.cx, yl) == 15);
    cielo::scia_lunare(t, q, c, 0.05, 1.0, 0.0);
    auto luce_luna = [](int v) { return v == 15 || v == 7; };
    int sotto = 0, di_lato = 0;
    for (int y = 181; y < ALTEZZA_VISTA; ++y) {
        for (int x = q.cx - 20; x < q.cx + 20; ++x) sotto += luce_luna(t.at(x, y));
        for (int x = 0; x < 40; ++x) di_lato += luce_luna(t.at(x, y));
    }
    CHECK(sotto > 30);
    CHECK(sotto > 5 * di_lato);
    // la fase: mezza luna col lembo a destra, a 6x (disco di ~20 px): accesa a destra del centro, buia a sinistra
    {
        GeometriaVista q6 = q15();
        q6.ppr = 640.0 / radianti(8.0);
        CieloNotte m = c;
        m.luna_elevazione = 1.0;
        m.luna_illuminata = 0.5;
        m.luna_angolo_lembo = 90.0;
        Tela s6 = sfondo();
        cielo::luna(s6, q6, m, 0.05);
        const int yc = static_cast<int>(q6.riga_elevazione(radianti(1.0)));
        auto acceso = [&](int x) { const int v = s6.at(x, yc); return v == 14 || v == 15 || v == 6 || v == 7; };
        CHECK(acceso(q6.cx + 6));
        CHECK_FALSE(acceso(q6.cx - 6));
        m.luna_illuminata = 0.0;   // luna nuova: niente disco
        Tela n6 = sfondo();
        cielo::luna(n6, q6, m, 0.05);
        CHECK(conta(n6, 0, ALTEZZA_VISTA, [](int v) { return v == 14 || v == 15 || v == 6 || v == 7; }) == 0);
    }
    // di giorno niente scia; la luna sotto l'orizzonte niente di niente
    Tela g = sfondo();
    cielo::scia_lunare(g, q, c, 0.9, 1.0, 0.0);
    c.luna_su = false;
    cielo::luna(g, q, c, 0.05);
    cielo::scia_lunare(g, q, c, 0.05, 1.0, 0.0);
    CHECK(conta(g, 0, ALTEZZA_VISTA, [](int v) { return v == 15 || v == 7; }) == 0);
}

TEST_CASE("luce sotto le nubi: il coperto scurisce, il meteo spento no") {
    MeteoVista m;
    CHECK(cielo::luce_sotto_le_nubi(0.8, m) == 0.8);
    m.attivo = true;
    CHECK(cielo::luce_sotto_le_nubi(1.0, m) == 1.0);
    m.nubi = 1.0;
    CHECK(cielo::luce_sotto_le_nubi(1.0, m) == doctest::Approx(0.55));
    m.temporale = 1.0;
    CHECK(cielo::luce_sotto_le_nubi(1.0, m) == doctest::Approx(0.25));
}

TEST_CASE("strati meteo sulla carta: barbe, cifre del mare, celle, ghiaccio") {
    // barba: l'asta va verso da dove viene il vento (nord: in su sullo schermo)
    Tela b;
    b.riempi(0);
    disegna_barba(b, 100, 100, 25.0, 0.0, 24, 14);
    int sopra = 0, sotto = 0;
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            if (b.at(x, y) == 14 && y != 100) (y < 100 ? sopra : sotto) += 1;
    CHECK(sopra > 20);
    CHECK(sotto == 0);
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    Tela t;
    t.riempi(1);
    StratiMeteo s;
    s.a_tela = [](Vec2 m) { return Vec2{320.0 + m.x / 100.0, 240.0 - m.y / 100.0}; };
    s.a_mondo = [](Vec2 q) { return Vec2{(q.x - 320.0) * 100.0, (240.0 - q.y) * 100.0}; };
    s.vento = [](Vec2, double& v, double& da) { v = 15.0; da = 270.0; };
    s.stato_mare = [](Vec2) { return 5; };
    s.ghiaccio = [](Vec2 m) { return m.y > 10000.0 ? 1.0 : 0.0; };   // ghiaccio a nord dei 10 km
    s.celle.push_back({Vec2{-15000.0, -10000.0}, 4000.0});
    disegna_strati_meteo(t, f, s);
    auto conta_c = [&](std::uint8_t c, int y0, int y1) {
        int n = 0;
        for (int y = y0; y < y1; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) n += t.at(x, y) == c;
        return n;
    };
    CHECK(conta_c(s.colori.ghiaccio, 0, 130) > 1000);   // tratteggio a nord
    CHECK(conta_c(s.colori.ghiaccio, 150, 480) == 0);
    CHECK(conta_c(s.colori.vento, 0, 480) > 200);
    CHECK(t.at(170, 340 - 40) == s.colori.celle);   // il bordo della cella, 40 pixel sopra il centro (170, 340)
}
