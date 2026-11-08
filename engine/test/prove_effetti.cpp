#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <set>

#include "grafica/effetti.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/fenomeni.hpp"

using namespace m688;
using doctest::Approx;

// --- fenomeni: numeri del dossier 07 (§2.6 e §11.2) -------------------------------------------------------------

TEST_CASE("fenomeni: Mk 48 sotto la chiglia (544 kg a 8 e 12 m)") {
    const auto a = fenomeno_subacqueo(544, 8);
    CHECK(a.A_max == Approx(10.9).epsilon(0.02));
    CHECK(a.T1 == Approx(1.55).epsilon(0.02));
    CHECK(a.regime == RegimeSubacqueo::sfiato);
    CHECK(a.corona_scura);
    CHECK(a.H_pennacchio > 180); CHECK(a.H_pennacchio < 230);   // ~215 x 90
    CHECK(a.R_pennacchio > 80); CHECK(a.R_pennacchio < 100);
    CHECK(a.t_pennacchio == Approx(a.T1));
    CHECK(a.R_slick == Approx(18.0 * std::cbrt(544.0)));
    const auto b = fenomeno_subacqueo(544, 12);
    CHECK(b.A_max == Approx(10.2).epsilon(0.02));
    CHECK(b.T1 == Approx(1.31).epsilon(0.02));
    CHECK(b.regime == RegimeSubacqueo::getto);
    CHECK(b.H_pennacchio > 150); CHECK(b.H_pennacchio < 175);   // ~160 x 105
    CHECK(b.R_pennacchio > 95); CHECK(b.R_pennacchio < 115);
    CHECK(b.v0_cupola == Approx(52).epsilon(0.05));
    CHECK(b.H_cupola <= 0.5 * b.H_pennacchio + 1e-9);           // la cupola resta sotto i pennacchi
    CHECK(b.T2 == Approx(0.74 * b.T1));
    CHECK(b.durata_nebbia > 20); CHECK(b.durata_nebbia < 40);
}

TEST_CASE("fenomeni: bombe di profondita', razzi e mine") {
    const auto c15 = fenomeno_subacqueo(135, 15);
    CHECK(c15.T1 == Approx(0.74).epsilon(0.03));
    CHECK(c15.regime == RegimeSubacqueo::pennacchi);
    CHECK(c15.H_pennacchio == Approx(85).epsilon(0.1));
    CHECK(c15.R_pennacchio == Approx(60).epsilon(0.1));
    CHECK(c15.v0_cupola == Approx(23).epsilon(0.05));
    CHECK(c15.pulsazioni >= 1);
    const auto c30 = fenomeno_subacqueo(135, 30);
    CHECK(c30.regime == RegimeSubacqueo::profonda);
    CHECK(c30.H_pennacchio > 25); CHECK(c30.H_pennacchio < 60);
    const auto c75 = fenomeno_subacqueo(135, 75);   // solo una gobba di 2-5 m e la chiazza
    CHECK(c75.H_pennacchio == 0.0);
    CHECK(c75.H_gobba > 2.0); CHECK(c75.H_gobba < 5.0);
    CHECK(2 * c75.R_chiazza > 30); CHECK(2 * c75.R_chiazza < 60);
    const auto rbu = fenomeno_subacqueo(25, 10);
    CHECK(rbu.T1 == Approx(0.51).epsilon(0.03));
    CHECK(rbu.H_pennacchio == Approx(50).epsilon(0.1));
    CHECK(fenomeno_subacqueo(25, 100).H_pennacchio == 0.0);
    const auto mina = fenomeno_subacqueo(600, 20, 20);
    CHECK(mina.fango);
    CHECK(mina.H_pennacchio == Approx(145).epsilon(0.1));
    CHECK_FALSE(fenomeno_subacqueo(600, 20).fango);
    CHECK(fenomeno_subacqueo(3, 2).H_pennacchio == 0.0);   // cariche minuscole: niente pennacchi vistosi
    CHECK(fenomeno_subacqueo(1, 1).H_cupola <= 6.0);         // ne' la cupola di Kolsky delle cariche grandi
}

TEST_CASE("fenomeni: nell'aria e incendi") {
    const auto m = fenomeno_aereo(165, false);   // Exocet: palla di fuoco di 10-20 m
    CHECK(m.D_palla > 10); CHECK(m.D_palla < 20);
    const auto d = fenomeno_aereo(10000, true);   // deposito: fungo di centinaia di metri
    CHECK(d.H_nube > 500);
    CHECK(d.lampo > m.lampo);
    // Briggs: risalita crescente e limitata; piu' vento, colonna piu' bassa
    const double F = flusso_galleggiamento(50e6);
    CHECK(risalita_fumo(F, 7, 200) < risalita_fumo(F, 7, 800));
    CHECK(risalita_fumo(F, 7, 1e5) == Approx(risalita_finale(F, 7)));
    CHECK(risalita_finale(F, 7) > 100); CHECK(risalita_finale(F, 7) < 300);
    CHECK(risalita_finale(F, 14) < risalita_finale(F, 7));
    // Heskestad: 1 MW su 1 m di diametro, fiamma di circa 2,7 m
    CHECK(altezza_fiamma(1e6, 1.0) == Approx(2.7).epsilon(0.05));
    CHECK(altezza_fiamma(1e3, 3.0) == 0.0);
}

// --- effetti ------------------------------------------------------------------------------------------------------

namespace {

double quota_massima(const Effetti& e, Materia m) {
    double z = 0;
    for (const auto& p : e.particelle())
        if (p.materia == m && p.ritardo <= 0) z = std::max(z, p.z);
    return z;
}

}  // namespace

TEST_CASE("effetti: sequenza di un'esplosione subacquea (Mk 48 a 12 m)") {
    const auto f = fenomeno_subacqueo(544, 12);
    Effetti e(7);
    Mondo m;
    e.esplosione_subacquea({0, 0}, 12, 544);
    const double dt = 1.0 / 30;
    double z_max = 0, t_max = 0, prima_sopra = -1;
    bool anello = false, anello_tardi = false, sotto = false;
    for (int i = 0; i < 60 * 30; ++i) {
        e.avanza(dt, m);
        const double t = e.tempo(), z = quota_massima(e, Materia::spruzzo);
        if (z > z_max) { z_max = z; t_max = t; }
        if (prima_sopra < 0 && z > 1.05 * f.H_cupola * 1.2) prima_sopra = t;   // oltre la cupola: pennacchi
        for (const auto& p : e.particelle()) {
            if (p.ritardo > 0) continue;
            if (p.materia == Materia::anello_scuro) (t < 0.4 ? anello : anello_tardi) = true;
            if (!p.piatta && p.z < -1e-6) sotto = true;
        }
    }
    CHECK(anello);
    CHECK_FALSE(anello_tardi);           // l'anello scuro dura meno di 0,3 s
    CHECK_FALSE(sotto);                  // niente sotto il pelo dell'acqua
    CHECK(z_max > 0.7 * f.H_pennacchio); // altezza dei pennacchi dalla tabella (variazione del seme +-20%)
    CHECK(z_max < 1.3 * f.H_pennacchio);
    CHECK(prima_sopra >= f.t_pennacchio);   // i pennacchi escono all'impulso della bolla
    CHECK(t_max > f.t_pennacchio + 0.6 * f.t_salita);
    CHECK(t_max < f.t_pennacchio + 1.6 * f.t_salita);
    // dopo un minuto restano solo la chiazza di schiuma e il ribollio
    for (const auto& p : e.particelle()) CHECK((p.materia == Materia::schiuma || p.materia == Materia::nebbia));
    CHECK(std::any_of(e.particelle().begin(), e.particelle().end(), [](const Particella& p) { return p.piatta; }));
}

TEST_CASE("effetti: carica profonda, solo gobba e chiazza; determinismo del seme") {
    Effetti e(3);
    Mondo m;
    e.esplosione_subacquea({0, 0}, 75, 135);
    double z = 0;
    for (int i = 0; i < 20 * 30; ++i) {
        e.avanza(1.0 / 30, m);
        for (const auto& p : e.particelle()) z = std::max(z, p.z);
    }
    CHECK(z < 6.0);

    auto impronta = [](std::uint64_t seme) {
        Effetti a(seme);
        Mondo mm;
        a.esplosione_subacquea({100, 200}, 15, 135);
        a.esplosione_aerea({0, 500}, 5, 165, false);
        for (int i = 0; i < 300; ++i) a.avanza(1.0 / 30, mm);
        double s = 0;
        for (const auto& p : a.particelle()) s += p.x * 1e-3 + p.y * 1e-5 + p.z;
        return std::make_pair(a.numero(), s);
    };
    CHECK(impronta(5) == impronta(5));
    CHECK(impronta(5) != impronta(6));
}

TEST_CASE("effetti: nave colpita da un missile, fiamme e colonna di fumo sottovento") {
    DanniNave d(DatiScafo::carica(M688_DATI "/scafi/perry.toml"), 1);
    const auto& st = d.corpi().front().stazioni;
    Esplosione x;
    x.tipo = Esplosione::Tipo::missile; x.x = (st.front().x + st.back().x) / 2 + 7; x.y = -1; x.z = 2.5; x.tnt_kg = 165;
    d.colpo(x);
    REQUIRE(d.corpi().front().celle.end() != std::find_if(d.corpi().front().celle.begin(), d.corpi().front().celle.end(),
                                                         [](const Cella& c) { return c.fuoco > 0; }));
    Mondo m;
    Nave n; n.id = 4; n.classe = "perry"; n.rotta = 80;
    m.aggiungi(n);
    Effetti e(1);
    e.imposta_vento(270, 8);   // da ovest: il fumo va verso est
    e.segui(4, &d);
    CHECK(e.lampo() == 0.0);
    d.avanza(0.1); e.avanza(0.1, m);
    CHECK(e.lampo() > 0.0);    // lampo dello scoppio
    for (int i = 0; i < 300 * 10; ++i) { d.avanza(0.1); e.avanza(0.1, m); }
    int fiamme = 0, fumo = 0;
    double x_fumo = 0;
    for (const auto& p : e.particelle()) {
        if (p.materia == Materia::fuoco) ++fiamme;
        if (p.materia == Materia::fumo && p.F > 0) { ++fumo; x_fumo += p.x; }
    }
    CHECK(fiamme > 0);
    CHECK(fumo > 100);
    CHECK(x_fumo / fumo > 300.0);                          // trascinato verso est
    CHECK(quota_massima(e, Materia::fumo) > 80.0);         // colonna di Briggs
    CHECK_FALSE(e.orli().empty());                         // orlo di schiuma sul galleggiamento
    for (const auto& o : e.orli()) CHECK(std::hypot(o.x, o.y) < 80.0);
}

TEST_CASE("effetti: dopo l'affondamento restano nafta e rottami a galla") {
    DanniNave d(DatiScafo::carica(M688_DATI "/scafi/perry.toml"), 2);
    const auto& st = d.corpi().front().stazioni;
    Esplosione x;
    x.x = (st.front().x + st.back().x) / 2; x.y = 0; x.z = st[st.size() / 2].z_chiglia - 6; x.contatto = false; x.tnt_kg = 544;
    d.colpo(x);
    Mondo m;
    Nave n; n.id = 1; n.classe = "perry"; n.pos = {1000, -2000}; n.rotta = 80;
    m.aggiungi(n);
    Effetti e(2);
    e.segui(1, &d);
    for (int i = 0; i < 2400 * 10 && !d.affondata(); ++i) { d.avanza(0.1); e.avanza(0.1, m); }   // la poppa in ~33 minuti
    REQUIRE(d.affondata());
    for (int i = 0; i < 600; ++i) { d.avanza(0.1); e.avanza(0.1, m); }
    int nafta = 0, rottami = 0;
    for (const auto& p : e.particelle()) {
        if (p.materia == Materia::nafta) { ++nafta; CHECK(std::hypot(p.x - 1000, p.y + 2000) < 400.0); }
        if (p.materia == Materia::detrito && p.galleggia) { ++rottami; CHECK(p.z == 0.0); }
    }
    CHECK(nafta > 0);
    CHECK(rottami > 10);
    CHECK(e.orli().empty());   // niente piu' galleggiamento
}

TEST_CASE("effetti: disegno, limite del mare, fuoco emissivo e fumo acceso di notte") {
    Effetti e(1);
    Tela t;
    t.riempi(110);   // mare
    Particella p;
    p.materia = Materia::spruzzo; p.r = 40; p.eta = 1; p.vita = 10; p.opacita = 1; p.seme = 9;
    e.disegna(t, p, 320, 200, 1.0, 210, 1.0, 1.5, 3000);
    std::set<int> usati;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            if (t.at(x, y) == 110) continue;
            usati.insert(t.at(x, y));
            CHECK(y < 210);   // niente oltre il limite
        }
    CHECK(!usati.empty());
    for (int i : usati) { CHECK(i > colori::GRIGI); CHECK(i < colori::GRIGI + 15); }   // mai trasparente ne' nero

    t.riempi(110);
    Particella f = p;
    f.materia = Materia::fuoco; f.calore = 1.0; f.r = 5; f.eta = 0.1; f.vita = 1;
    e.disegna(t, f, 320, 200, 1.0, 480, 0.0, 1.5, 3000);
    int fuoco = 0;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x)
            if (std::find(std::begin(colori::FUOCO), std::end(colori::FUOCO), t.at(x, y)) != std::end(colori::FUOCO)) ++fuoco;
    CHECK(fuoco > 30);

    // fumo appena uscito dalle fiamme: di notte acceso dal basso, di giorno grigio
    Particella s = p;
    s.materia = Materia::fumo; s.calore = 1.0; s.eta = 1.0; s.vita = 600; s.r = 20; s.tono = 0.1;
    auto accesi = [&](double luce) {
        t.riempi(110);
        e.disegna(t, s, 320, 200, 1.0, 480, luce, 1.5, 3000);
        int n = 0;
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) n += t.at(x, y) == colori::FUMO_ACCESO || t.at(x, y) == colori::FUMO_ACCESO_CHIARO;
        return n;
    };
    CHECK(accesi(0.0) > 50);
    CHECK(accesi(1.0) == 0);

    // chiazza di nafta: scurisce il mare su una riga sottile
    t.riempi(120);
    Particella c;
    c.materia = Materia::nafta; c.piatta = true; c.r = 100; c.eta = 10; c.vita = 7200; c.opacita = 1; c.seme = 3;
    e.disegna(t, c, 320, 200, 1.0, 480, 1.0, 1.5, 3000);
    int scuri = 0, righe = 0;
    for (int y = 0; y < 480; ++y) {
        bool riga = false;
        for (int x = 0; x < 640; ++x) if (t.at(x, y) < 120) { ++scuri; riga = true; }
        righe += riga;
    }
    CHECK(scuri > 100);
    CHECK(righe <= 2);
}

TEST_CASE("palette: colori degli effetti nei posti liberi") {
    const auto pal = Palette::carica(M688_DATI "/palette/giorno.pal");
    for (int i : {190, 192, 193, 194, 196, 197, 198}) {
        const auto c = pal.rgb(i);
        CHECK_FALSE((c[0] == 255 && c[1] == 0 && c[2] == 202));   // niente piu' segnaposto magenta
    }
    // di notte il fuoco resta luminoso, il fango si scurisce
    const auto notte = pal.illuminata(0.0, {{196, 198}});
    CHECK(notte.rgb(colori::FUOCO[4]) == pal.rgb(colori::FUOCO[4]));
    CHECK(notte.rgb(colori::FANGO)[0] < pal.rgb(colori::FANGO)[0]);
    const auto lampo = pal.schiarita(0.5, {{144, 158}});
    CHECK(lampo.rgb(150)[0] > pal.rgb(150)[0]);
}

// --- deformazioni del modello (§5.2): falle, fuliggine, alberi, inarcamento -------------------------------------

#include "grafica/modello.hpp"

namespace {
int conta(const Sprite& s, int indice_max) {
    int n = 0;
    for (auto c : s.pix) n += c != TRASPARENTE && c <= indice_max;
    return n;
}
double grigio_medio(const Sprite& s) {
    double v = 0;
    int n = 0;
    for (auto c : s.pix) if (c != TRASPARENTE) { v += c - colori::GRIGI; ++n; }
    return n ? v / n : 0.0;
}
}  // namespace

TEST_CASE("deformazioni: falla nera, fuliggine, albero caduto e inarcamento sul modello") {
    const auto pal = Palette::carica(M688_DATI "/palette/giorno.pal");
    const auto modello = Modello::carica(M688_DATI "/modelli/perry.mdl");
    const auto rampa = rampa_grigi(pal);
    PosaNave intatta;
    intatta.danneggiata = true;
    const Sprite base = genera_sprite(modello, intatta, 90, false, rampa, 2.0);   // vista dal lato sinistro (y > 0)

    PosaNave falla = intatta;   // falla di 4 m sopra il galleggiamento, sul fianco sinistro
    falla.segni.push_back({SegnoScafo::Tipo::falla, -20.0, 6.8, 3.0, 2.0, 1.5, 2.0, 1.0});
    const Sprite s1 = genera_sprite(modello, falla, 90, false, rampa, 2.0);
    CHECK(conta(s1, colori::GRIGI + 1) > conta(base, colori::GRIGI + 1) + 10);   // buco nero di ~8 x 8 pixel

    PosaNave fuliggine = intatta;   // incendio a centro nave: tutto piu' scuro attorno
    fuliggine.segni.push_back({SegnoScafo::Tipo::fuliggine, 0.0, 0.0, 6.0, 12.0, 14.0, 9.0, 1.0});
    CHECK(grigio_medio(genera_sprite(modello, fuliggine, 90, false, rampa, 2.0)) < grigio_medio(base) - 0.3);

    // l'albero principale coricato verso dritta: la sagoma si abbassa
    Effetti e(1);
    DanniNave d(DatiScafo::carica(M688_DATI "/scafi/perry.toml"), 1);
    e.segui(9, &d, &modello);
    Esplosione x;
    x.tipo = Esplosione::Tipo::missile; x.x = 5.0; x.y = 1.0; x.z = 8.0; x.tnt_kg = 165;
    d.colpo(x);
    Mondo m;
    Nave n; n.id = 9; n.classe = "perry";
    m.aggiungi(n);
    for (int i = 0; i < 100; ++i) { d.avanza(0.1); e.avanza(0.1, m); }
    PosaNave caduto = d.posa();
    e.decora(9, caduto);
    REQUIRE_FALSE(caduto.parti.empty());
    for (const auto& pm : caduto.parti) CHECK(pm.angolo == Approx(1.45));
    caduto.segni.clear();
    auto alti = [](const Sprite& sp) {   // pixel piu' di 20 m sopra il galleggiamento (scala 3,75 px/m)
        int conto = 0;
        for (int yy = 0; yy < sp.h && yy < sp.ancora_y - 20 * 3.75; ++yy)
            for (int xx = 0; xx < sp.w; ++xx) conto += sp.pix[static_cast<std::size_t>(yy * sp.w + xx)] != TRASPARENTE;
        return conto;
    };
    const Sprite alto = genera_sprite(modello, intatta, 90, false, rampa, 3.75);
    const Sprite s3 = genera_sprite(modello, caduto, 90, false, rampa, 3.75);
    CHECK(alti(alto) > 100);
    CHECK(alti(s3) < alti(alto) - 150);   // l'albero (36 m) non sporge piu'; restano radar e antenne

    PosaNave arcata = intatta;   // inarcamento di 4 m a centro nave: la sagoma sale
    arcata.flessione_m = 4.0; arcata.flessione_larghezza = 35.0;
    const Sprite s4 = genera_sprite(modello, arcata, 90, false, rampa, 2.0);
    CHECK(s4.ancora_y > base.ancora_y + 5);
}

TEST_CASE("danni: la bolla sotto la chiglia solleva la nave e la inarca") {
    DanniNave d(DatiScafo::carica(M688_DATI "/scafi/perry.toml"), 1);
    const auto& st = d.corpi().front().stazioni;
    Esplosione x;
    x.x = (st.front().x + st.back().x) / 2; x.y = 0; x.z = st[st.size() / 2].z_chiglia - 6; x.contatto = false; x.tnt_kg = 544;
    d.colpo(x);
    double imm = 0, arco = 0;
    for (int i = 0; i < 20; ++i) {
        d.avanza(0.05);
        imm = std::min(imm, d.corpi().front().assetto.immersione);
        arco = std::max(arco, d.posa().flessione_m);
    }
    CHECK(imm < -0.5);   // sollevata
    CHECK(arco > 2.0);   // inarcata di qualche metro (0,4 A_max, meno con la distanza)
}
