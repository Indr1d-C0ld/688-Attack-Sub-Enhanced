// Vista del periscopio dell'Enhanced (docs/14_periscopi.md, P3): oculare, reticoli, elevazione, stadimetro, modi.
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <string>

#include "grafica/oculare.hpp"
#include "grafica/periscopio.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/onde.hpp"
#include "nucleo/periscopi.hpp"

using namespace m688;

namespace {
ParametriOculare a6x() {
    ParametriOculare p;
    p.attivo = true;
    p.campo_gradi = 8.0;
    p.passo_gradi = 1.0;
    p.passo_fine_gradi = 0.25;
    return p;
}
int conta(const Tela& t, std::uint8_t c, const Rettangolo& r) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) n += t.at(x, y) == c;
    return n;
}
}  // namespace

TEST_CASE("oculare spento: la geometria della vista e' quella del classico") {
    VistaPeriscopio v;
    v.quota_occhio_m = 1.5;
    CHECK(Periscopio::pixel_per_radiante(v) == Periscopio::pixel_per_radiante(false));
    v.alto_ingrandimento = true;
    CHECK(Periscopio::pixel_per_radiante(v) == Periscopio::pixel_per_radiante(true));
    const double atteso = ALTEZZA_VISTA / 2 + depressione_orizzonte(1.5) * Periscopio::pixel_per_radiante(true);
    CHECK(Periscopio::riga_orizzonte(v) == atteso);
    CHECK(Periscopio::riga_elevazione(v, 0.01) == ALTEZZA_VISTA / 2 - std::tan(0.01) * Periscopio::pixel_per_radiante(true));
}

TEST_CASE("oculare: il campo copre il diametro, l'elevazione sposta la linea di mira") {
    VistaPeriscopio v;
    v.oculare = a6x();
    const double ppr = Periscopio::pixel_per_radiante(v);
    CHECK(ppr == doctest::Approx(2 * oculare::RAGGIO / radianti(8.0)));
    // un oggetto a mezzo campo sopra l'orizzontale tocca il bordo alto del cerchio (piccoli angoli)
    CHECK(Periscopio::riga_elevazione(v, radianti(4.0)) == doctest::Approx(oculare::CENTRO_Y - oculare::RAGGIO).epsilon(0.01));
    // 12x: campo di 4 gradi, il doppio dei pixel per grado
    VistaPeriscopio d = v;
    d.oculare.campo_gradi = 4.0;
    CHECK(Periscopio::pixel_per_radiante(d) == doctest::Approx(2 * ppr));
    // alzato a 44 gradi (fermo del Type 2): quell'elevazione e' al centro, l'orizzonte esce sotto la vista
    v.oculare.campo_gradi = 32.0;
    v.oculare.elevazione_gradi = 44.0;
    CHECK(Periscopio::riga_elevazione(v, radianti(44.0)) == doctest::Approx(oculare::CENTRO_Y));
    CHECK(Periscopio::riga_orizzonte(v) > ALTEZZA_VISTA);
    // TV: il campo e' la larghezza dello schermo 4:3
    VistaPeriscopio tv = d;
    tv.oculare.modo = ModoOculare::tv;
    CHECK(Periscopio::pixel_per_radiante(tv) == doctest::Approx(2 * oculare::TV_MEZZA_L / radianti(4.0)));
}

TEST_CASE("reticolo americano: tacche lunghe ogni grado e corte ogni quarto a 6x, niente fuori dal cerchio") {
    Tela t;
    t.riempi(9);
    const auto p = a6x();
    oculare::disegna_reticolo(t, p);
    const double ppr = oculare::pixel_per_radiante(p);
    const int x1 = oculare::CENTRO_X + static_cast<int>(std::lround(std::tan(radianti(1.0)) * ppr));
    const int xq = oculare::CENTRO_X + static_cast<int>(std::lround(std::tan(radianti(0.25)) * ppr));
    CHECK(t.at(x1, oculare::CENTRO_Y - 6) == colori::NERO);   // grande
    CHECK(t.at(xq, oculare::CENTRO_Y - 3) == colori::NERO);   // piccola
    CHECK(t.at(xq, oculare::CENTRO_Y - 6) == 9);
    CHECK(t.at(oculare::CENTRO_X, oculare::CENTRO_Y) == colori::NERO);   // croce a tutto campo
    CHECK(t.at(oculare::CENTRO_X + oculare::RAGGIO + 5, oculare::CENTRO_Y) == 9);
    CHECK(t.at(20, 40) == 9);
}

TEST_CASE("reticolo sovietico: scale con un solo lato di tacche e vuoto al centro") {
    Tela t;
    t.riempi(9);
    auto p = a6x();
    p.reticolo = Reticolo::sovietico;
    oculare::disegna_reticolo(t, p);
    const double ppr = oculare::pixel_per_radiante(p);
    const int x1 = oculare::CENTRO_X + static_cast<int>(std::lround(std::tan(radianti(1.0)) * ppr));
    CHECK(t.at(x1, oculare::CENTRO_Y - 8) == colori::NERO);
    CHECK(t.at(x1, oculare::CENTRO_Y + 4) == 9);   // tacche solo in su
    CHECK(t.at(oculare::CENTRO_X, oculare::CENTRO_Y) == 9);   // vuoto sul bersaglio
}

TEST_CASE("maschera: nero fuori dal cerchio, centro intatto, bordo vignettato") {
    Tela t;
    t.riempi(9);
    t.imposta_ritaglio({0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA});
    oculare::maschera(t, a6x());
    CHECK(t.at(10, 100) == colori::NERO);
    CHECK(t.at(oculare::CENTRO_X, oculare::CENTRO_Y) == 9);
    CHECK(t.at(10, 10) == 9);   // la fascia della scala dei rilevamenti resta
    const int interno = conta(t, colori::NERO, {oculare::CENTRO_X - 40, oculare::CENTRO_Y - 40, 80, 80});
    const int anello = conta(t, colori::NERO, {oculare::CENTRO_X + 135, oculare::CENTRO_Y - 10, 12, 20});
    CHECK(interno == 0);
    CHECK(anello > 40);
    CHECK(anello < 240);
}

TEST_CASE("stadimetro: l'immagine fantasma sale dello sdoppiamento, a scacchiera") {
    Tela t;
    t.riempi(9);
    auto p = a6x();
    p.sdoppiamento_gradi = 0.5;
    const int d = static_cast<int>(std::lround(std::tan(radianti(0.5)) * oculare::pixel_per_radiante(p)));
    for (int x = 300; x < 340; ++x) t.pixel(x, 200, 3);   // un galleggiamento
    oculare::sdoppia(t, p);
    int fantasma = 0, vero = 0;
    for (int x = 300; x < 340; ++x) { fantasma += t.at(x, 200 - d) == 3; vero += t.at(x, 200) == 3; }
    CHECK(fantasma == 20);
    CHECK(vero == 20);
    // lente ruotata: a sinistra
    Tela o;
    o.riempi(9);
    p.sdoppiamento_orizzontale = true;
    for (int y = 150; y < 190; ++y) o.pixel(330, y, 3);
    oculare::sdoppia(o, p);
    int sx = 0;
    for (int y = 150; y < 190; ++y) sx += o.at(330 - d, y) == 3;
    CHECK(sx == 20);
}

TEST_CASE("telemetro a reticolo (sovietici): il cursore sulla scala verticale, niente immagine fantasma") {
    auto p = a6x();
    p.reticolo = Reticolo::sovietico;
    p.passo_gradi = 1.0;
    p.passo_fine_gradi = 0.2;
    p.campo_gradi = 10.0;
    p.reticolo_misura = true;
    p.sdoppiamento_gradi = 0.6;   // tre divisioni piccole
    Tela t;
    t.riempi(9);
    for (int x = 300; x < 340; ++x) t.pixel(x, 200, 3);
    oculare::sdoppia(t, p);   // col reticolo non copia niente
    int copie = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 300; x < 340; ++x) copie += y != 200 && t.at(x, y) == 3;
    CHECK(copie == 0);
    oculare::disegna_reticolo(t, p);
    const int yc = oculare::CENTRO_Y - static_cast<int>(std::lround(std::tan(radianti(0.6)) * oculare::pixel_per_radiante(p)));
    int barra = 0;
    for (int x = oculare::CENTRO_X - 16; x <= oculare::CENTRO_X - 2; ++x) barra += t.at(x, yc) == colori::NERO;
    CHECK(barra == 15);
    // senza misura in corso niente barra
    p.sdoppiamento_gradi = 0.0;
    Tela u;
    u.riempi(9);
    oculare::disegna_reticolo(u, p);
    int vuota = 0;
    for (int x = oculare::CENTRO_X - 16; x <= oculare::CENTRO_X - 9; ++x) vuota += u.at(x, yc) == colori::NERO;
    CHECK(vuota == 0);
}

TEST_CASE("modi monocromi: rampa, intensificatore che vede di notte, TV che di notte vede poco") {
    Palette g;
    for (int i = 0; i < 256; ++i) g.imposta(i, 150, 150, 150);
    for (int i = 96; i <= 143; ++i) g.imposta(i, 30, 60, 120);   // mare
    auto media = [&](ModoOculare m, double luce) {
        Tela t;
        t.riempi(150);
        auto p = a6x();
        p.modo = m;
        oculare::converti(t, g, p, luce, 3.0);
        const Rettangolo a = oculare::area(p);
        double s = 0;
        int n = 0;
        for (int y = a.y; y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x) {
                const int c = t.at(x, y);
                REQUIRE(c >= oculare::RAMPA);
                REQUIRE(c < oculare::RAMPA + oculare::N_RAMPA);
                s += c - oculare::RAMPA;
                ++n;
            }
        return s / n;
    };
    const double notte = 0.03;
    CHECK(media(ModoOculare::intensificatore, notte) > media(ModoOculare::bassa_luminosita, notte));
    CHECK(media(ModoOculare::bassa_luminosita, notte) > media(ModoOculare::tv, notte) + 2);
    CHECK(media(ModoOculare::tv, 1.0) > 6);
    // giorno: niente conversione
    Tela t;
    t.riempi(150);
    oculare::converti(t, g, a6x(), 0.0, 0.0);
    CHECK(t.at(oculare::CENTRO_X, oculare::CENTRO_Y) == 150);
}

TEST_CASE("palette: fosfori per modo e filtri solo sugli indici della vista") {
    Palette p;
    for (int i = 0; i < 256; ++i) p.imposta(i, 200, 200, 200);
    auto o = a6x();
    o.modo = ModoOculare::intensificatore;
    oculare::palette(p, o);
    const auto c = p.rgb(oculare::RAMPA + oculare::N_RAMPA - 1);
    CHECK(c[1] > c[0]);
    CHECK(c[0] > c[2]);   // verde giallino
    o.modo = ModoOculare::tv;
    oculare::palette(p, o);
    const auto b = p.rgb(oculare::RAMPA + oculare::N_RAMPA - 1);
    CHECK(std::abs(b[0] - b[2]) < 16);
    o.modo = ModoOculare::giorno;
    o.filtro = Filtro::rosso;
    oculare::palette(p, o);
    CHECK(p.rgb(100)[1] < 80);       // mare: filtrato
    CHECK(p.rgb(100)[0] == 200);
    CHECK(p.rgb(7)[1] == 200);       // interfaccia: no
}

TEST_CASE("tremolio: fermo a zero, limitato dall'ampiezza") {
    auto p = a6x();
    double r = 1, e = 1;
    oculare::tremolio(p, 2.3, r, e);
    CHECK(r == 0.0);
    CHECK(e == 0.0);
    p.tremolio = 1.0;
    double massimo = 0;
    for (double t = 0; t < 10; t += 0.05) {
        oculare::tremolio(p, t, r, e);
        massimo = std::max({massimo, std::fabs(r), std::fabs(e)});
    }
    CHECK(massimo > 0.3 * oculare::TREMOLIO_GRADI);
    CHECK(massimo <= oculare::TREMOLIO_GRADI + 1e-9);
}

TEST_CASE("telecamera verticale: piu' scura in profondita', il ghiaccio fa lastre scure e canali chiari") {
    auto media = [](double quota, double ghiaccio, int& scuri) {
        Tela t;
        const Rettangolo r{0, 0, 160, 120};
        oculare::telecamera_verticale(t, r, quota, 1.0, ghiaccio, 1.0);
        double s = 0;
        scuri = 0;
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) {
                const int v = t.at(x, y) - oculare::RAMPA;
                REQUIRE(v >= 0);
                REQUIRE(v < oculare::N_RAMPA);
                s += v;
                scuri += v <= 3;
            }
        return s / (r.w * r.h);
    };
    int s1 = 0, s2 = 0, s3 = 0;
    const double vicino = media(10, 0, s1), lontano = media(60, 0, s2);
    CHECK(vicino > lontano + 2);
    media(10, 0.9, s3);
    CHECK(s3 > s1 + 1000);
}

TEST_CASE("dal modello e dallo stato del posto (nucleo/periscopi.hpp) ai parametri dell'oculare") {
    DatiPeriscopio d;
    d.id = "type18";
    d.ingrandimenti = {1.5, 6.0, 12.0};
    d.campi_gradi = {32.0, 8.0, 4.0};
    d.reticolo_gradi = {4.0, 1.0, 0.5};
    d.reticolo_fine_gradi = {1.0, 0.25, 0.125};
    StatoPeriscopio s;
    s.ingrandimento = 2;
    s.elevazione = 14.0;
    s.modo = ModoVista::bassa_luminosita;
    s.tremolio = 0.3;
    s.testa_fuori = true;
    auto p = parametri_oculare(d, s);
    CHECK(p.attivo);
    CHECK(p.campo_gradi == 4.0);
    CHECK(p.passo_gradi == 0.5);
    CHECK(p.passo_fine_gradi == 0.125);
    CHECK(p.reticolo == Reticolo::americano);
    CHECK(p.elevazione_gradi == 14.0);
    CHECK(p.modo == ModoOculare::bassa_luminosita);
    CHECK(p.tremolio == 0.3);
    CHECK_FALSE(p.sott_acqua);
    // ingrandimento oltre l'elenco: l'ultimo; scuola sovietica; testa ancora sott'acqua
    d.reticolo = "sovietico";
    s.ingrandimento = 7;
    s.modo = ModoVista::termico;
    s.testa_fuori = false;
    p = parametri_oculare(d, s);
    CHECK(p.campo_gradi == 4.0);
    CHECK(p.reticolo == Reticolo::sovietico);
    CHECK(p.modo == ModoOculare::termico);
    CHECK(p.sott_acqua);
    CHECK(filtro_da_nome("polaroid") == Filtro::polaroid);
    CHECK(filtro_da_nome("affumicato") == Filtro::affumicato);
    CHECK(filtro_da_nome("?") == Filtro::nessuno);
}

TEST_CASE("testa sott'acqua: solo acqua nel cerchio, piu' chiara verso la superficie") {
    Tela t;
    t.riempi(9);
    auto p = a6x();
    p.sott_acqua = true;
    oculare::acqua(t, p, 1.0, 2.0);
    const Rettangolo a = oculare::area(p);
    double alto = 0, basso = 0;
    for (int x = a.x; x < a.x + a.w; ++x)
        for (int k = 0; k < 10; ++k) {
            const int c1 = t.at(x, a.y + k), c2 = t.at(x, a.y + a.h - 1 - k);
            REQUIRE(c1 >= 96);
            REQUIRE(c1 <= 143);
            REQUIRE(c2 >= 96);
            REQUIRE(c2 <= 143);
            alto += c1;
            basso += c2;
        }
    CHECK(alto > basso + 10 * a.w * 10);
}

TEST_CASE("onde: varianza (Hs/4)^2 e periodo di picco di Pierson-Moskowitz") {
    const double hs = 3.0;
    double s2 = 0;
    int n = 0;
    for (double t = 0; t < 3000; t += 0.25, ++n) s2 += elevazione_onda_m(hs, t) * elevazione_onda_m(hs, t);
    CHECK(std::sqrt(s2 / n) == doctest::Approx(hs / 4).epsilon(0.05));
    CHECK(periodo_picco_s(4.0) == doctest::Approx(10.0));
    CHECK(elevazione_onda_m(0.0, 12.0) == 0.0);
}

TEST_CASE("acqua sulla finestra: sommersa, l'acqua che sale dal basso, gocce che asciugano, niente senza onde") {
    const double hs = 3.0, testa = 1.0;
    auto p = a6x();
    p.altezza_testa_m = testa;
    auto cop = [&](double t) { return std::clamp((elevazione_onda_m(hs, t) - testa + oculare::FINESTRA_M) / (2 * oculare::FINESTRA_M), 0.0, 1.0); };
    double sommersa = -1, meta = -1, uscita = -1;
    for (double t = 0; t < 600 && uscita < 0; t += 0.01) {
        const double c = cop(t);
        if (sommersa < 0 && c >= 1.0) sommersa = t;
        if (meta < 0 && c > 0.45 && c < 0.55) meta = t;
        if (sommersa > 0 && c <= 0.0) uscita = t;
    }
    REQUIRE(sommersa > 0);
    REQUIRE(meta > 0);
    REQUIRE(uscita > 0);
    auto scena = [] {
        Tela t;
        t.riempi(9);
        return t;
    };
    const Rettangolo a = oculare::area(p);
    auto acqua = [&](const Tela& t, int y) {
        int n = 0;
        for (int x = a.x; x < a.x + a.w; ++x) n += t.at(x, y) >= 96 && t.at(x, y) <= 143;
        return n;
    };
    Tela t = scena();
    CHECK(oculare::acqua_sulla_finestra(t, p, hs, 1.0, sommersa) == 1.0);
    CHECK(acqua(t, oculare::CENTRO_Y) == a.w);
    t = scena();
    CHECK(oculare::acqua_sulla_finestra(t, p, hs, 1.0, meta) == doctest::Approx(0.5).epsilon(0.1));
    CHECK(acqua(t, a.y + a.h - 5) == a.w);   // in basso acqua
    CHECK(acqua(t, a.y + 5) == 0);           // in alto la scena
    // appena uscita: velo e gocce cambiano la scena (righe a scacchiera per vederle); dopo molto: niente
    auto righe = [] {
        Tela r;
        for (int y = 0; y < ALTEZZA_VISTA; ++y) r.linea_orizzontale(0, LARGHEZZA_TELA - 1, y, static_cast<std::uint8_t>(20 + y % 7));
        return r;
    };
    const Tela pulita = righe();
    auto diversi = [&](const Tela& d) {
        int n = 0;
        for (int y = a.y; y < a.y + a.h; ++y)
            for (int x = a.x; x < a.x + a.w; ++x) n += d.at(x, y) != pulita.at(x, y);
        return n;
    };
    Tela dopo = righe();
    oculare::acqua_sulla_finestra(dopo, p, hs, 1.0, uscita + 0.3);
    CHECK(diversi(dopo) > 1000);
    // senza onde e senza pioggia, o col posto sconosciuto: niente
    Tela calma = righe();
    CHECK(oculare::acqua_sulla_finestra(calma, p, 0.0, 1.0, 5.0) == 0.0);
    auto ignoto = p;
    ignoto.altezza_testa_m = -1.0;
    oculare::acqua_sulla_finestra(calma, ignoto, hs, 1.0, sommersa);
    CHECK(diversi(calma) == 0);
    // pioggia: gocce anche col mare calmo
    Tela pioggia = righe();
    p.pioggia = 1.0;
    oculare::acqua_sulla_finestra(pioggia, p, 0.0, 1.0, 5.0);
    CHECK(diversi(pioggia) > 200);
}

TEST_CASE("mare mosso: schiuma di Monahan, solo sotto l'orizzonte e nel cerchio, piu' moto col mare grosso") {
    CHECK(oculare::frazione_schiuma(0.0) == 0.0);
    CHECK(oculare::frazione_schiuma(10.0) == doctest::Approx(3.84e-6 * std::pow(10.0, 3.41)));
    CHECK(oculare::frazione_schiuma(40.0) == 0.2);
    auto p = a6x();
    p.campo_gradi = 32.0;
    auto disegna = [&](double hs) {
        Tela t;
        t.riempi(9);
        oculare::mare(t, p, 0.0, 0.0, 0.0, 1.5, hs, 30000.0, 4.0);
        return t;
    };
    const Tela calmo = disegna(0.0);
    CHECK(calmo.at(oculare::CENTRO_X, oculare::CENTRO_Y + 60) == 9);   // mare calmo: niente
    const Tela poco = disegna(0.5), molto = disegna(4.0);
    CHECK(molto.at(oculare::CENTRO_X, oculare::CENTRO_Y - 90) == 9);    // cielo intatto (vicino all'orizzonte le creste)
    CHECK(molto.at(10, oculare::CENTRO_Y + 60) == 9);                   // fuori dal cerchio intatto
    auto variazione = [](const Tela& t) {   // scarto dei pixel di mare lungo una colonna
        double s = 0, s2 = 0;
        int n = 0;
        for (int y = oculare::CENTRO_Y + 10; y < oculare::CENTRO_Y + 140; ++y) {
            const double c = t.at(oculare::CENTRO_X + 30, y);
            REQUIRE(((c >= 96 && c <= 143) || (c >= colori::GRIGI + 11 && c <= colori::GRIGI + 14)));
            s += c; s2 += c * c; ++n;
        }
        return s2 / n - (s / n) * (s / n);
    };
    CHECK(variazione(molto) > variazione(poco));
}

TEST_CASE("mare mosso: le creste vicine coprono l'orizzonte e le navi; la schiuma per forza del vento") {
    // strisce di schiuma solo da forza 7 (13,9 m/s), crescenti, al piu' il 70%
    CHECK(oculare::frazione_strisce(13.0) == 0.0);
    CHECK(oculare::frazione_strisce(15.0) > 0.0);
    CHECK(oculare::frazione_strisce(25.0) > oculare::frazione_strisce(18.0));
    CHECK(oculare::frazione_strisce(60.0) <= 0.7);
    auto p = a6x();
    p.campo_gradi = 8.0;
    p.direzione_mare_gradi = 300.0;
    const int orizzonte = oculare::CENTRO_Y;   // elevazione 0: l'orizzonte vero e' poco sotto il centro
    auto profilo = [&](double hs, double occhio, double vento_kn, double t) {
        p.vento_kn = vento_kn;
        Tela tela;
        tela.riempi(9);
        oculare::ProfiloMare pr;
        oculare::mare(tela, p, 0.0, 0.0, 0.0, occhio, hs, 30000.0, t, &pr);
        return std::make_pair(pr, tela);
    };
    // mare 2 con l'occhio a 1,5 m: niente copre una nave a 3 km sopra l'orizzonte
    const auto [calmo, tela_calma] = profilo(0.5, 1.5, 8.0, 10.0);
    int sopra = 0;
    for (int x = oculare::CENTRO_X - 100; x <= oculare::CENTRO_X + 100; ++x) sopra += calmo.riga(x, 3000.0) < orizzonte - 2;
    CHECK(sopra == 0);
    // mare 6 con l'occhio a 0,6 m: in qualche momento le creste vicine salgono sopra l'orizzonte e coprono la nave
    int coperte = 0, colonne_cielo = 0;
    for (double t = 10.0; t < 30.0; t += 2.5) {
        const auto [mosso, tela] = profilo(4.0, 0.6, 27.0, t);
        for (int x = oculare::CENTRO_X - 100; x <= oculare::CENTRO_X + 100; ++x) {
            coperte += mosso.riga(x, 3000.0) < orizzonte - 2;
            colonne_cielo += tela.at(x, orizzonte - 3) != 9;   // il mare sopra l'orizzonte piatto
        }
        CHECK(mosso.riga(oculare::CENTRO_X, 1.0) == INT_MAX);   // a un metro niente davanti all'occhio
    }
    CHECK(coperte > 50);
    CHECK(colonne_cielo > 50);
    // deterministico: stesso istante, stessa immagine
    const auto [a, ta] = profilo(4.0, 0.6, 27.0, 12.5);
    const auto [b, tb] = profilo(4.0, 0.6, 27.0, 12.5);
    int diversi = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diversi += ta.at(x, y) != tb.at(x, y);
    CHECK(diversi == 0);
    // la schiuma cresce col vento: forza 4, 7, 10 (testa alta, l'occhio a 3 m)
    p.campo_gradi = 32.0;
    auto schiuma = [&](double hs, double vento_kn) {
        int n = 0;
        for (double t = 10.0; t < 40.0; t += 6.0) {
            const auto [pr, tela] = profilo(hs, 3.0, vento_kn, t);
            for (int y = orizzonte; y < oculare::CENTRO_Y + 140; ++y)
                for (int x = oculare::CENTRO_X - 120; x <= oculare::CENTRO_X + 120; ++x) n += tela.at(x, y) >= colori::GRIGI + 11 && tela.at(x, y) <= colori::GRIGI + 14;
        }
        return n;
    };
    const int s4 = schiuma(1.2, 13.0), s7 = schiuma(4.0, 30.0), s10 = schiuma(9.0, 52.0);
    CAPTURE(s4);
    CAPTURE(s7);
    CAPTURE(s10);
    CHECK(s7 > s4);
    CHECK(s10 > s7);
}

TEST_CASE("banchisa: spenta niente; nel pack lastre e acqua calma, le creste coprono; fuori dal pack il riflesso") {
    auto p = a6x();
    p.campo_gradi = 32.0;
    const int orizzonte = oculare::CENTRO_Y;
    // il pack oltre 100 m dall'occhio, con una cresta alta 4 m a 300 m davanti (y 295-305); nell'acqua niente
    oculare::GhiaccioVista g;
    g.attivo = true;
    g.copertura = 1.0;
    g.campione = [](Vec2 q) {
        oculare::CampioneGhiaccio c;
        if (std::hypot(q.x, q.y) < 100.0) return c;
        c.ghiaccio = true;
        c.spessore_m = 1.5;
        if (q.y > 250.0 && q.y < 350.0 && std::abs(q.x) < 150.0) c.cresta_m = 4.0;
        return c;
    };
    auto disegna = [&](const oculare::GhiaccioVista& gv, double occhio, oculare::ProfiloMare* pr) {
        Tela t;
        t.riempi(9);
        oculare::banchisa(t, p, 0.0, 0.0, 0.0, occhio, 30000.0, 0.0, gv, pr);
        return t;
    };
    auto grigi = [](const Tela& t, int y0, int y1) {
        int n = 0;
        for (int y = y0; y < y1; ++y)
            for (int x = oculare::CENTRO_X - 100; x <= oculare::CENTRO_X + 100; ++x) n += t.at(x, y) >= colori::GRIGI && t.at(x, y) <= colori::GRIGI + 14;
        return n;
    };
    // spenta: la tela non cambia
    const Tela vuota = disegna(oculare::GhiaccioVista{}, 1.5, nullptr);
    CHECK(grigi(vuota, 0, ALTEZZA_VISTA) == 0);
    CHECK(vuota.at(oculare::CENTRO_X, orizzonte + 20) == 9);
    // emersi nella polynya (occhio a 12 m): lastre grigie sotto l'orizzonte, l'acqua calma (rampa del mare) vicino,
    // niente sopra il cielo (le creste di 4 m restano sotto l'occhio)
    oculare::ProfiloMare pr;
    const Tela pack = disegna(g, 12.0, &pr);
    CHECK(grigi(pack, orizzonte + 1, orizzonte + 40) > 3000);
    CHECK(grigi(pack, 0, orizzonte - 3) == 0);
    const std::uint8_t vicino = pack.at(oculare::CENTRO_X, oculare::CENTRO_Y + 140);
    CHECK(vicino >= 96);
    CHECK(vicino <= 143);
    // la cresta copre: una nave a 2 km dietro la cresta e' nascosta piu' in alto di una a 200 m davanti
    CHECK(pr.riga(oculare::CENTRO_X, 2000.0) < pr.riga(oculare::CENTRO_X, 200.0));
    // con l'occhio a 1,5 m le creste di 4 m a 300 m salgono sopra l'orizzonte (celle di 40 m: una cresta ogni 40 m
    // nella fascia, a ogni colonna)
    oculare::GhiaccioVista fitte = g;
    fitte.cella_m = 40.0;
    const Tela basso = disegna(fitte, 1.5, nullptr);
    CHECK(grigi(basso, orizzonte - 12, orizzonte - 1) > 50);
    // deterministica
    const Tela ancora = disegna(g, 12.0, nullptr);
    int diversi = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diversi += pack.at(x, y) != ancora.at(x, y);
    CHECK(diversi == 0);
    // fuori dal pack (copertura 0) il margine a 1,5 km: l'acqua vicina non si tocca, il cielo basso si schiarisce
    oculare::GhiaccioVista m = g;
    m.copertura = 0.0;
    m.campione = [](Vec2 q) {
        oculare::CampioneGhiaccio c;
        c.ghiaccio = q.y > 1500.0;
        c.spessore_m = 1.0;
        return c;
    };
    const Tela margine = disegna(m, 1.5, nullptr);
    CHECK(margine.at(oculare::CENTRO_X, oculare::CENTRO_Y + 100) == 9);
    CHECK(conta(margine, colori::GRIGI + 13, {oculare::CENTRO_X - 100, orizzonte - 15, 201, 14}) > 5);
    // sotto le nubi (plafond 300 m) la base si schiarisce sopra il ghiaccio e si scurisce un poco sopra l'acqua: dal mare
    // aperto il margine a 4 km (sotto i 4,3 gradi chiaro, sopra no; il riflesso e' diffuso su un chilometro); dalla polynya un canale a 3-4,5 km (fra 3,8 e
    // 5,7 gradi il cielo d'acqua, sopra e sotto chiaro)
    const double ppr = oculare::pixel_per_radiante(p);
    auto riga = [&](double gradi) { return static_cast<int>(std::lround(oculare::CENTRO_Y - ppr * std::tan(gradi * PI / 180.0))); };
    auto sotto_le_nubi = [&](oculare::GhiaccioVista gv) {
        gv.nubi = 0.9;
        gv.plafond_m = 300.0;
        Tela t;
        t.riempi(9);
        for (int y = 0; y < orizzonte - 2; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) t.pixel(x, y, static_cast<std::uint8_t>(cielo::NUBE));
        oculare::banchisa(t, p, 0.0, 0.0, 0.0, 1.5, 30000.0, 0.0, gv, nullptr);
        return t;
    };
    auto media = [](const Tela& t, int y) {
        double n = 0.0;
        for (int x = oculare::CENTRO_X - 60; x <= oculare::CENTRO_X + 60; ++x) n += t.at(x, y);
        return n / 121.0 - cielo::NUBE;
    };
    oculare::GhiaccioVista lontano = m;
    lontano.campione = [](Vec2 q) {
        oculare::CampioneGhiaccio c;
        c.ghiaccio = q.y > 4000.0;
        c.spessore_m = 1.0;
        return c;
    };
    const Tela nubi_margine = sotto_le_nubi(lontano);
    CHECK(media(nubi_margine, riga(2.0)) > 3.0);
    CHECK(media(nubi_margine, riga(9.0)) < -0.5);
    CHECK(media(nubi_margine, riga(12.0)) < -0.5);
    oculare::GhiaccioVista canale = g;
    canale.campione = [](Vec2 q) {
        oculare::CampioneGhiaccio c;
        if (std::hypot(q.x, q.y) < 100.0 || std::abs(q.y - 3750.0) < 750.0) return c;
        c.ghiaccio = true;
        c.spessore_m = 1.5;
        return c;
    };
    const Tela nubi_canale = sotto_le_nubi(canale);
    CHECK(media(nubi_canale, riga(4.7)) < 1.0);
    CHECK(media(nubi_canale, riga(2.5)) > 3.0);
    CHECK(media(nubi_canale, riga(9.0)) > 3.0);
    // senza nubi le stesse righe restano come sono
    CHECK(media(disegna(canale, 1.5, nullptr), riga(9.0)) + cielo::NUBE == doctest::Approx(9.0));
}

TEST_CASE("banchisa: le creste passano da una cella all'altra e calano in punta") {
    // la chiglia in una cella sola (x e y fra 0 e 200 m): la sua cresta esce nelle vicine, che non ne hanno, e non
    // finisce di colpo al bordo (su un passo di 1 m la quota cambia poco); fuori dalla portata delle punte niente
    oculare::GhiaccioVista g;
    g.attivo = true;
    g.campione = [](Vec2 q) {
        oculare::CampioneGhiaccio c;
        c.ghiaccio = true;
        c.spessore_m = 1.5;
        if (q.x >= 0.0 && q.x < 200.0 && q.y >= 0.0 && q.y < 200.0) c.cresta_m = 3.0;
        return c;
    };
    double dentro = 0.0, fuori = 0.0, salto = 0.0, lontano = 0.0;
    for (int i = -150; i <= 350; ++i) {
        double prima = oculare::quota_cresta(g, {i - 1.0, -150.0});
        for (int j = -150; j <= 350; ++j) {
            const double z = oculare::quota_cresta(g, {static_cast<double>(i), static_cast<double>(j)});
            const bool nella = i >= 0 && i < 200 && j >= 0 && j < 200;
            (nella ? dentro : fuori) = std::max(nella ? dentro : fuori, z);
            if (i < -130 || i > 330 || j < -130 || j > 330) lontano = std::max(lontano, z);
            const double sinistra = oculare::quota_cresta(g, {i - 1.0, static_cast<double>(j)});
            if (j > -150) salto = std::max({salto, std::abs(z - prima), std::abs(z - sinistra)});
            prima = z;
        }
    }
    MESSAGE("cresta: dentro ", dentro, " m, fuori ", fuori, " m, salto massimo ", salto, " m/m");
    CHECK(dentro > 1.5);
    CHECK(fuori > 0.5);
    CHECK(salto < 1.5);
    CHECK(lontano == 0.0);
}

TEST_CASE("costa: spenta niente; le pareti del fiordo sopra l'orizzonte, il mare davanti intatto, l'isola copre") {
    auto p = a6x();
    p.campo_gradi = 32.0;
    const int orizzonte = oculare::CENTRO_Y;
    auto disegna = [&](const oculare::CostaVista& c, oculare::ProfiloMare* pr) {
        Tela t;
        t.riempi(9);
        oculare::costa(t, p, 0.0, 0.0, 0.0, 1.5, 30000.0, 1.0, c, pr);
        return t;
    };
    auto diversi_da_9 = [](const Tela& t, int x0, int x1, int y0, int y1) {
        int n = 0;
        for (int y = y0; y < y1; ++y)
            for (int x = x0; x < x1; ++x) n += t.at(x, y) != 9;
        return n;
    };
    // spenta: la tela non cambia
    CHECK(diversi_da_9(disegna(oculare::CostaVista{}, nullptr), 0, LARGHEZZA_TELA, 0, ALTEZZA_VISTA) == 0);
    // il fiordo verso nord: le pareti ripide a 400 m dall'asse, alte 400 m
    oculare::CostaVista fiordo;
    fiordo.attiva = true;
    fiordo.quota = [](Vec2 q) {
        const double lato = std::abs(q.x) - 400.0;
        return lato > 0.0 ? 400.0 * (1.0 - std::exp(-lato / 90.0)) : -80.0;
    };
    const Tela f = disegna(fiordo, nullptr);
    const int lato_px = 70;   // le colonne vicine al bordo del campo, dove le pareti salgono
    CHECK(diversi_da_9(f, oculare::CENTRO_X - 150, oculare::CENTRO_X - 150 + lato_px, orizzonte - 60, orizzonte) > 300);
    CHECK(diversi_da_9(f, oculare::CENTRO_X + 150 - lato_px, oculare::CENTRO_X + 150, orizzonte - 60, orizzonte) > 300);
    // al centro il fiordo e' aperto per chilometri: il cielo alto e il mare vicino restano come sono
    CHECK(diversi_da_9(f, oculare::CENTRO_X - 10, oculare::CENTRO_X + 10, orizzonte - 60, orizzonte - 20) == 0);
    CHECK(diversi_da_9(f, 0, LARGHEZZA_TELA, oculare::CENTRO_Y + 60, ALTEZZA_VISTA) == 0);
    // deterministica
    const Tela f2 = disegna(fiordo, nullptr);
    int diff = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diff += f.at(x, y) != f2.at(x, y);
    CHECK(diff == 0);
    // un'isola alta 40 m a 2 km davanti: copre una nave a 3 km e non una a 1 km
    oculare::CostaVista isola;
    isola.attiva = true;
    isola.quota = [](Vec2 q) {
        const double d2 = (q.x * q.x + (q.y - 2000.0) * (q.y - 2000.0)) / (300.0 * 300.0);
        return 48.0 * std::exp(-d2) - 8.0;
    };
    oculare::ProfiloMare pr;
    const Tela i = disegna(isola, &pr);
    CHECK(diversi_da_9(i, oculare::CENTRO_X - 5, oculare::CENTRO_X + 5, orizzonte - 15, orizzonte + 1) > 20);
    CHECK(pr.riga(oculare::CENTRO_X, 3000.0) < orizzonte - 3);
    CHECK(pr.riga(oculare::CENTRO_X, 1000.0) >= orizzonte);
}

TEST_CASE("costa: con la foschia del meteo le pareti del fiordo restano, non diventano cielo grigio") {
    // in partita la foschia (cielo::foschia) veniva dopo la costa e, alle righe sopra l'orizzonte, la copriva come cielo
    // lontano: la terra spariva con ogni visibilita' sotto i 40 km
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    std::map<std::string, ClasseNave> classi;
    std::map<std::string, InsiemeSprite> sprite;
    Periscopio per(f, classi, sprite);
    Mondo mondo;
    Nave occhio;
    VistaPeriscopio v;
    v.oculare = a6x();
    v.oculare.campo_gradi = 32.0;
    v.quota_occhio_m = 1.5;
    v.luce = 1.0;
    v.visibilita_m = 15000.0;
    v.meteo.attivo = true;
    v.costa.attiva = true;
    v.costa.quota = [](Vec2 q) {
        const double lato = std::abs(q.x) - 400.0;
        return lato > 0.0 ? 400.0 * (1.0 - std::exp(-lato / 90.0)) : -80.0;
    };
    Tela t;
    per.disegna(t, mondo, occhio, v);
    auto terra = [](int c) { return (c >= 196 && c <= 198) || c == colori::GRIGI + 5 || c == colori::GRIGI + 10; };
    int n = 0;
    for (int y = oculare::CENTRO_Y - 60; y < oculare::CENTRO_Y - 5; ++y)
        for (int x = oculare::CENTRO_X - 150; x < oculare::CENTRO_X - 80; ++x) n += terra(t.at(x, y));
    CHECK(n > 300);
}

TEST_CASE("filtro, stadimetro e telecamera verticale dallo stato del posto") {
    DatiPeriscopio d;
    d.campi_gradi = {32.0};
    d.filtri = {"polaroid", "rosso"};
    StatoPeriscopio s;
    s.filtro = 1;
    s.sdoppiamento_gradi = 0.4;
    s.telecamera_verticale = true;
    s.testa_fuori = true;
    const auto p = parametri_oculare(d, s);
    CHECK(p.filtro == Filtro::rosso);
    CHECK(p.sdoppiamento_gradi == 0.4);
    CHECK(p.telecamera_verticale);
    s.filtro = 5;   // fuori elenco: nessuno
    CHECK(parametri_oculare(d, s).filtro == Filtro::nessuno);
}
