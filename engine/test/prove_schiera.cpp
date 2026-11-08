#include <doctest.h>

#include <cmath>
#include <map>
#include <vector>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/schiera.hpp"

using namespace m688;

// La schiera trainata dei sottomarini (blocco 1 del sonar, D; docs/20_sonar_blocco1.md §7)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}
const DatiSchiera& tb16() { return *catalogo().schiera("tb16"); }

}  // namespace

TEST_CASE("schiera: la quota della TB-16 con la velocita', come la tabella del dossier 04 §1.5") {
    // discesa del cavo di 730 m sotto il battello (dossier 04 §1.5), entro il 20%
    const std::map<double, double> tabella = {{2, 390}, {3, 270}, {4, 205}, {5, 165}, {6, 140}, {8, 105}, {10, 85}, {12, 70}, {15, 55}, {20, 40}};
    for (const auto& [v, dz] : tabella) {
        CAPTURE(v);
        const double discesa = 730.0 * std::sin(radianti(angolo_critico_deg(tb16(), v)));
        CHECK(discesa == doctest::Approx(dz).epsilon(0.2));
    }
    CHECK(angolo_critico_deg(tb16(), 5.0) == doctest::Approx(13.0).epsilon(0.05));
    CHECK(angolo_critico_deg(tb16(), 0.0) == doctest::Approx(90.0));
    // la quota del centro scende rallentando; col cavo corto sta piu' su; il profilo va dalla poppa alla coda
    double prima = 1e9;
    for (double v = 2.0; v <= 20.0; v += 1.0) {
        const FormaSchiera f = forma_schiera(tb16(), v, 60.0, 790.0);
        CHECK(f.quota_centro_m < prima);
        CHECK(f.quota_bassa_m > f.quota_centro_m);
        prima = f.quota_centro_m;
    }
    const FormaSchiera f5 = forma_schiera(tb16(), 5.0, 60.0, 790.0), corto = forma_schiera(tb16(), 5.0, 60.0, 300.0);
    CHECK(f5.quota_centro_m - 60.0 == doctest::Approx(790.0 * std::sin(radianti(13.07)) + 0.25 * flessione_m(tb16(), 5.0)).epsilon(0.03));
    CHECK(corto.quota_centro_m < f5.quota_centro_m);
    REQUIRE(f5.profilo.size() == PUNTI_CAVO + PUNTI_SCHIERA);
    CHECK(f5.profilo.front().second == doctest::Approx(60.0));
    CHECK(f5.profilo.back().second == doctest::Approx(f5.quota_bassa_m));
    CHECK(f5.dietro_m > 700.0);
    // oltre la quota massima la schiera e' tenuta li', col segnale per i danni
    const FormaSchiera giu = forma_schiera(tb16(), 1.0, 300.0, 790.0);
    CHECK(giu.oltre_quota_max);
    CHECK(giu.quota_centro_m == doctest::Approx(tb16().quota_max_m));
    // il cavo oltre il massimo del sistema non c'e'
    CHECK(forma_schiera(tb16(), 5.0, 60.0, 5000.0).quota_centro_m == doctest::Approx(f5.quota_centro_m));
}

TEST_CASE("schiera: la quota ci mette minuti, rallentando e accelerando") {
    CavoSchiera s;
    const FormaSchiera a10 = s.avanza(tb16(), 1.0, 10.0, 60.0, 790.0);   // la prima volta: in equilibrio
    const double eq3 = forma_schiera(tb16(), 3.0, 60.0, 790.0).quota_centro_m;
    // rallenta a 3 kn (tau circa 9 minuti): dopo un minuto e' ancora molto sopra l'equilibrio, dopo 30 minuti quasi
    FormaSchiera f = a10;
    for (int k = 0; k < 60; ++k) f = s.avanza(tb16(), 1.0, 3.0, 60.0, 790.0);
    CHECK(f.quota_centro_m < a10.quota_centro_m + 0.5 * (eq3 - a10.quota_centro_m));
    for (int k = 0; k < 1740; ++k) f = s.avanza(tb16(), 1.0, 3.0, 60.0, 790.0);
    CHECK(f.quota_centro_m == doctest::Approx(eq3).epsilon(0.05));
    // accelera di colpo a 15 kn: risale piano (dopo 30 s e' ancora sotto meta' strada)
    const double eq15 = forma_schiera(tb16(), 15.0, 60.0, 790.0).quota_centro_m;
    for (int k = 0; k < 30; ++k) f = s.avanza(tb16(), 1.0, 15.0, 60.0, 790.0);
    CHECK(f.quota_centro_m > eq15 + 0.5 * (eq3 - eq15));
    for (int k = 0; k < 900; ++k) f = s.avanza(tb16(), 1.0, 15.0, 60.0, 790.0);
    CHECK(f.quota_centro_m == doctest::Approx(eq15).epsilon(0.05));
    // il battello che scende se la porta dietro subito
    const FormaSchiera giu = s.avanza(tb16(), 1.0, 15.0, 160.0, 790.0);
    CHECK(giu.quota_centro_m == doctest::Approx(f.quota_centro_m + 100.0).epsilon(0.01));
    CHECK(tempo_schiera_s(tb16(), 5.0, 790.0) == doctest::Approx(863.0 / (5.0 * NODO)).epsilon(0.01));
}

TEST_CASE("schiera: il fondo in acqua bassa a bassa velocita', mai in acqua profonda; lo sfregamento") {
    // 200 m d'acqua, battello a 60 m e cavo pieno: tocca sotto i 5 kn (dossier 04 §2.1)
    const std::vector<double> piatto(PUNTI_CAVO + PUNTI_SCHIERA, 200.0), profondo(PUNTI_CAVO + PUNTI_SCHIERA, 3000.0);
    CHECK(contatto_fondo(forma_schiera(tb16(), 4.0, 60.0, 790.0), piatto).tocca);
    CHECK(contatto_fondo(forma_schiera(tb16(), 4.0, 60.0, 790.0), piatto).frazione_schiera == doctest::Approx(1.0));
    CHECK_FALSE(contatto_fondo(forma_schiera(tb16(), 8.0, 60.0, 790.0), piatto).tocca);
    for (double v : {1.0, 3.0, 10.0}) CHECK_FALSE(contatto_fondo(forma_schiera(tb16(), v, 60.0, 790.0), profondo).tocca);
    // una secca dietro: tocca solo la coda
    std::vector<double> secca(PUNTI_CAVO + PUNTI_SCHIERA, 3000.0);
    secca.back() = forma_schiera(tb16(), 6.0, 60.0, 790.0).quota_bassa_m + 2.0;
    const ContattoFondo coda = contatto_fondo(forma_schiera(tb16(), 6.0, 60.0, 790.0), secca);
    CHECK(coda.tocca);
    CHECK(coda.frazione_schiera == doctest::Approx(1.0 / PUNTI_SCHIERA));
    // lo sfregamento: niente senza contatto, cresce con la velocita' e col fondo duro, cala sopra i 300 Hz
    CHECK(rumore_sfregamento_db(100.0, 4.0, 0.0, TipoFondo::sabbia) < -200.0);
    CHECK(rumore_sfregamento_db(100.0, 6.0, 1.0, TipoFondo::sabbia) > rumore_sfregamento_db(100.0, 3.0, 1.0, TipoFondo::sabbia) + 8.0);
    CHECK(rumore_sfregamento_db(100.0, 4.0, 1.0, TipoFondo::roccia) > rumore_sfregamento_db(100.0, 4.0, 1.0, TipoFondo::sabbia));
    CHECK(rumore_sfregamento_db(100.0, 4.0, 1.0, TipoFondo::sabbia) > rumore_sfregamento_db(100.0, 4.0, 1.0, TipoFondo::fango));
    CHECK(rumore_sfregamento_db(3000.0, 4.0, 1.0, TipoFondo::sabbia) == doctest::Approx(rumore_sfregamento_db(300.0, 4.0, 1.0, TipoFondo::sabbia) - 20.0));
    // nel ricevitore: la schiera che striscia sente meno (il rumore proprio sale di oltre 15 dB sotto 300 Hz)
    Ricevitore r{{0.0, 0.0}, 190.0, 0.0, 4.0, catalogo().sensore("tb16")};
    const double pulito = rumore_proprio_ricevitore(100.0, r, 4.0, false);
    CHECK(pulito == rumore_proprio(100.0, *r.sensore, 4.0, false));   // senza contatto: quello di sempre
    r.sfregamento = 1.0;
    CHECK(rumore_proprio_ricevitore(100.0, r, 4.0, false) > pulito + 15.0);
}

TEST_CASE("schiera: la vibrazione del cavo sul LOFAR (Strouhal), solo col cavo filato") {
    const auto f = frequenze_vibrazione(0.0094, 5.0);
    REQUIRE(f.size() == 2);
    CHECK(f[0] == doctest::Approx(0.2 * 5.0 * NODO / 0.0094));   // circa 55 Hz
    CHECK(f[1] == doctest::Approx(2.0 * f[0]));
    CHECK(frequenze_vibrazione(0.0094, 1.5).empty());
    CHECK(frequenze_vibrazione(0.0, 5.0).empty());
    Ambiente amb;
    amb.fondale_m = 3000.0;
    Ricevitore r{{0.0, 0.0}, 200.0, 0.0, 5.0, catalogo().sensore("tb16")};
    const Spettro senza = spettro_fascio({}, r, amb, 90.0, 20.0, 150.0, 0.05);
    r.cavo_filato_m = 790.0;
    const Spettro con = spettro_fascio({}, r, amb, 90.0, 20.0, 150.0, 0.05);
    const auto picco = [&](const Spettro& s, double hz) {
        double m = 0.0;
        for (std::size_t i = 0; i < s.snr_db.size(); ++i)
            if (std::fabs(s.frequenza(i) - hz) < 1.0) m = std::max(m, static_cast<double>(s.snr_db[i]));
        return m;
    };
    CHECK(picco(senza, f[0]) < 0.01);
    CHECK(picco(con, f[0]) > 2.0);   // una riga propria da riconoscere
    CHECK(picco(con, f[1]) > 0.5);
    CHECK(picco(con, 40.0) < 0.01);
    // e la cuffia la sente: una riga ricevuta senza sorgente
    bool propria = false;
    for (const auto& x : righe_ricevute({}, r, amb, 90.0, 20.0, 150.0, 0.05)) propria = propria || (x.sorgente == -2 && std::fabs(x.hz - f[0]) < 0.1);
    CHECK(propria);
}

TEST_CASE("schiera: a bassa velocita' la schiera sotto lo strato sente quello che la sfera non sente") {
    // Fær Øer a febbraio: strato di 250 m col termoclino sotto; il battello a 60 m, un Victor III a 400 m e 6 km
    static const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente amb;
    amb.profilo = &clima.profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 5.0;
    amb.traffico = 0.5;
    REQUIRE(amb.profilo->sld_m > 200.0);
    UnitaAcustica u;
    u.id = 1;
    u.classe = "victor3";
    u.categoria = "sottomarino";
    u.quota_m = 400.0;
    u.velocita_kn = 6.0;
    u.pos = {6000.0, 0.0};
    const Sorgente victor = sorgente_acustica(catalogo(), u);
    const FormaSchiera f = forma_schiera(tb16(), 4.0, 60.0, 790.0);
    CHECK(f.quota_centro_m > amb.profilo->sld_m);   // a 4 kn la schiera e' sotto lo strato
    const double schiera = valuta(victor, Ricevitore{{0.0, 0.0}, f.quota_centro_m, 0.0, 4.0, catalogo().sensore("tb16")}, amb).se_db;
    const double alla_quota_del_battello = valuta(victor, Ricevitore{{0.0, 0.0}, 60.0, 0.0, 4.0, catalogo().sensore("tb16")}, amb).se_db;
    const double sfera = valuta(victor, Ricevitore{{0.0, 0.0}, 60.0, 0.0, 4.0, catalogo().sensore("bqq5_sfera")}, amb).se_db;
    MESSAGE("schiera a " << f.quota_centro_m << " m: " << schiera << " dB; alla quota del battello " << alla_quota_del_battello << "; sfera " << sfera);
    CHECK(schiera > 3.0);
    CHECK(sfera < 0.0);
    CHECK(schiera > alla_quota_del_battello + 3.0);
}

TEST_CASE("schiera: il cavo sicuro in acqua bassa") {
    // 200 m d'acqua, battello a 60 m: a 4 kn il cavo pieno tocca, il cavo sicuro no; in acqua profonda tutto il cavo
    const double c = cavo_sicuro_m(tb16(), 4.0, 60.0, 200.0);
    CHECK(c > 100.0);
    CHECK(c < 790.0);
    CHECK(forma_schiera(tb16(), 4.0, 60.0, c).quota_bassa_m <= 180.0 + 1e-6);
    CHECK(forma_schiera(tb16(), 4.0, 60.0, c + 20.0).quota_bassa_m > 180.0);
    CHECK(cavo_sicuro_m(tb16(), 4.0, 60.0, 3000.0) == doctest::Approx(tb16().cavo_max_m));
    CHECK(cavo_sicuro_m(tb16(), 4.0, 60.0, 60.0) == 0.0);   // il battello e' gia' quasi sul fondo
    CHECK(cavo_sicuro_m(tb16(), 8.0, 60.0, 200.0) > c);      // piu' veloce, piu' cavo
}
