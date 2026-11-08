#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/dinamica_sottomarino.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/scafo.hpp"

using namespace m688;

// Dinamica del sottomarino dell'Enhanced (fase C6a): casi dei dossier 01 §2 e 02 §2.3.

namespace {

constexpr double DT = 1.0 / 9.0;

DinamicaSottomarino battello(const std::string& classe, double kn = 0.0, double quota = 100.0) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto& c = classi.at(classe);
    const auto s = DatiScafo::carica(M688_DATI "/" + c.scafo).sottomarino;
    return DinamicaSottomarino(parametri_sottomarino(classe, &c, &s), 0.0, quota, kn);
}

// tiene la velocita' e la quota per t secondi
void tieni(DinamicaSottomarino& d, ComandiSottomarino c, double t) {
    for (double s = 0; s < t; s += DT) d.avanza(DT, c);
}

// diametro tattico: scostamento laterale quando la prua ha girato di 180 gradi, con la barra tutta a dritta
double diametro_tattico(DinamicaSottomarino d) {
    ComandiSottomarino c;
    c.giri = d.giri();
    c.quota_m = d.quota_m();
    c.barra_deg = DinamicaSottomarino::BARRA_MAX;
    double x = 0.0, girato = 0.0;
    while (girato < 180.0) {
        const double prima = d.rotta();
        d.avanza(DT, c);
        girato += normalizza_180(d.rotta() - prima);
        x += d.velocita_m_s() * std::sin(d.rotta() * 3.14159265358979323846 / 180.0) * DT;   // verso est
    }
    return x;
}

}  // namespace

TEST_CASE("dinamica: parametri dalle classi e dal dossier") {
    const auto d = battello("los_angeles");
    CHECK(d.parametri().dislocamento_t == doctest::Approx(6927.0).epsilon(0.02));
    CHECK(d.parametri().velocita_max_kn == 32.0);
    CHECK(d.parametri().potenza_kW == doctest::Approx(30000 * 0.7457));
    const auto a = battello("alfa");
    CHECK(a.parametri().velocita_max_kn == 41.0);
    CHECK(a.parametri().diametro_tattico_L < d.parametri().diametro_tattico_L);
    // senza potenza pubblicata (Trafalgar): scala del 688 col coefficiente dell'Ammiragliato
    const auto t = parametri_sottomarino("trafalgar");
    CHECK(t.potenza_kW > 10000.0);
    CHECK(t.potenza_kW < 25000.0);
}

TEST_CASE("dinamica: velocita' di regime n Vmax e accelerazione dalla spinta") {
    auto d = battello("los_angeles");
    ComandiSottomarino c;
    c.quota_m = 100.0;
    c.giri = 1.0;
    double t20 = -1.0;
    for (double t = 0; t < 1800.0; t += DT) {
        d.avanza(DT, c);
        if (t20 < 0 && d.velocita_kn() >= 20.0) t20 = t;
    }
    CHECK(d.velocita_kn() == doctest::Approx(32.0).epsilon(0.02));
    // da fermo a 20 nodi in un paio di minuti (non nei 20 s del moto classico, ne' in 10 minuti)
    CHECK(t20 > 60.0);
    CHECK(t20 < 240.0);
    c.giri = d.giri_per_velocita(15.0);
    tieni(d, c, 1800.0);
    CHECK(d.velocita_kn() == doctest::Approx(15.0).epsilon(0.02));
    // indietro: il battello si ferma e poi arretra
    c.giri = DinamicaSottomarino::GIRI_INDIETRO;
    tieni(d, c, 900.0);
    CHECK(d.velocita_kn() < -3.0);
}

TEST_CASE("dinamica: i giri di 1/3, 2/3 e massimi danno le velocita' delle campane; i danni le tagliano, non le scalano") {
    // le campane del posto di comando dell'Enhanced: avanti 1/3 a 5 nodi, 2/3 a 10, massima alla massima della classe
    // (da fermo: a bassa velocita' la resistenza e' poca e ci vuole un'ora per arrivare a regime)
    for (const std::string classe : {"los_angeles", "victor3", "kilo"}) {
        const double massima = battello(classe).parametri().velocita_max_kn;
        for (const double kn : {5.0, 10.0, massima}) {
            auto d = battello(classe);
            ComandiSottomarino c;
            c.quota_m = 100.0;
            c.giri = d.giri_per_velocita(kn);
            tieni(d, c, 3600.0);
            CHECK_MESSAGE(d.velocita_kn() == doctest::Approx(kn).epsilon(0.02), classe, " a ", kn, " nodi");
        }
    }
    // a meta' della massima per i danni: 1/3 resta 5 nodi, la massima si ferma a 16
    LimitiBattello l;
    l.velocita = 0.5;
    for (const auto& [kn, attesa] : {std::pair<double, double>{5.0, 5.0}, {32.0, 16.0}}) {
        auto d = battello("los_angeles");
        ComandiSottomarino c;
        c.quota_m = 100.0;
        c.giri = d.giri_per_velocita(kn);
        for (double t = 0; t < 3600.0; t += DT) d.avanza(DT, c, {}, l);
        CHECK(d.velocita_kn() == doctest::Approx(attesa).epsilon(0.02));
    }
}

TEST_CASE("dinamica: diametro tattico 4-6 lunghezze a 15-20 nodi, l'Alfa stringe di piu'") {
    for (const double kn : {15.0, 20.0}) {
        auto d = battello("los_angeles", kn);
        const double L = d.parametri().lunghezza_m;
        const double dt = diametro_tattico(d);
        CAPTURE(kn);
        CHECK(dt > 4.0 * L);
        CHECK(dt < 6.0 * L);
    }
    auto a = battello("alfa", 20.0);
    auto l = battello("los_angeles", 20.0);
    CHECK(diametro_tattico(a) < 0.75 * diametro_tattico(l));
    // inversione di rotta dell'Alfa a 30 nodi in «decine di secondi»
    auto v = battello("alfa", 30.0);
    ComandiSottomarino c;
    c.giri = v.giri();
    c.quota_m = 100.0;
    c.barra_deg = DinamicaSottomarino::BARRA_MAX;
    double t = 0.0, girato = 0.0;
    while (girato < 180.0 && t < 600.0) {
        const double prima = v.rotta();
        v.avanza(DT, c);
        girato += normalizza_180(v.rotta() - prima);
        t += DT;
    }
    CHECK(t < 90.0);
}

TEST_CASE("dinamica: in accostata con la barra tutta la velocita' cala di circa un terzo") {
    auto d = battello("los_angeles", 20.0);
    ComandiSottomarino c;
    c.giri = d.giri();
    c.quota_m = 100.0;
    c.barra_deg = 35.0;
    tieni(d, c, 900.0);
    CHECK(d.velocita_kn() > 0.55 * 20.0);
    CHECK(d.velocita_kn() < 0.75 * 20.0);
    CHECK(d.rateo_virata_deg_s() > 0.0);   // a dritta
}

TEST_CASE("dinamica: «accostare per» con la barra massima e senza oltrepassare la rotta") {
    auto d = battello("los_angeles", 15.0);
    ComandiSottomarino c;
    c.giri = d.giri();
    c.quota_m = 100.0;
    c.rotta = 270.0;
    c.barra_max_deg = 20.0;
    double barra = 0.0, oltre = 0.0;
    for (double t = 0; t < 600.0; t += DT) {
        d.avanza(DT, c);
        barra = std::max(barra, std::fabs(d.barra_deg()));
        oltre = std::min(oltre, normalizza_180(d.rotta() - 270.0));   // da 0 verso sinistra: oltre se < 0
    }
    CHECK(barra <= 20.0 + 1e-9);
    CHECK(std::fabs(normalizza_180(d.rotta() - 270.0)) < 1.0);
    CHECK(oltre > -5.0);
}

TEST_CASE("dinamica: la quota ordinata con l'assetto entro l'angolo massimo, dz/dt ~ v sin theta") {
    auto d = battello("los_angeles", 15.0, 50.0);
    ComandiSottomarino c;
    c.giri = d.giri();
    c.quota_m = 150.0;
    c.angolo_max_deg = 15.0;
    double assetto = 0.0, discesa = 0.0;
    for (double t = 0; t < 600.0; t += DT) {
        d.avanza(DT, c);
        assetto = std::min(assetto, d.assetto_deg());
        discesa = std::max(discesa, d.rateo_quota_m_s());
    }
    CHECK(d.quota_m() == doctest::Approx(150.0).epsilon(0.02));
    CHECK(assetto >= -15.0 - 1e-9);   // prua in giu' al piu' 15 gradi
    CHECK(assetto < -10.0);
    CHECK(discesa == doctest::Approx(d.velocita_m_s() * std::sin(15.0 * 3.14159265358979323846 / 180.0)).epsilon(0.1));
    // angolo di immersione senza quota: scende finche' dura
    c.quota_m.reset();
    c.angolo_deg = 10.0;
    tieni(d, c, 30.0);
    CHECK(d.assetto_deg() == doctest::Approx(-10.0).epsilon(0.05));
    CHECK(d.quota_m() > 155.0);
    // a battello fermo i piani non governano: la quota non cambia
    auto f = battello("los_angeles", 0.0, 80.0);
    ComandiSottomarino ferma;
    ferma.quota_m = 30.0;
    tieni(f, ferma, 120.0);
    CHECK(f.quota_m() == doctest::Approx(80.0).epsilon(0.01));
}

TEST_CASE("dinamica: peso netto e momento del battello danneggiato contro la velocita'") {
    // 100 t di acqua: da fermo affonda, a 15 nodi i piani e lo scafo ne reggono gran parte
    ForzeIdrostatiche pesante;
    pesante.peso_netto_N = 100e3 * 9.81;
    auto ferma = battello("los_angeles", 0.0, 100.0);
    auto veloce = battello("los_angeles", 15.0, 100.0);
    ComandiSottomarino c;
    c.quota_m = 100.0;
    ComandiSottomarino cv = c;
    cv.giri = veloce.giri();
    for (double t = 0; t < 60.0; t += DT) { ferma.avanza(DT, c, pesante); veloce.avanza(DT, cv, pesante); }
    CHECK(ferma.quota_m() > 100.0 + 30.0);
    CHECK(veloce.quota_m() < ferma.quota_m() - 20.0);
    // momento a prua in su: a battello fermo l'assetto si sposta, a 15 nodi i piani lo tengono
    ForzeIdrostatiche momento;
    momento.momento_assetto_Nm = 6927e3 * 9.81 * 0.5 * std::sin(10.0 * 3.14159265358979323846 / 180.0);
    auto f2 = battello("los_angeles", 0.0, 100.0);
    auto v2 = battello("los_angeles", 15.0, 100.0);
    for (double t = 0; t < 300.0; t += DT) { f2.avanza(DT, c, momento); v2.avanza(DT, cv, momento); }
    CHECK(f2.assetto_deg() == doctest::Approx(10.0).epsilon(0.1));
    CHECK(std::fabs(v2.assetto_deg()) < 2.0);
    CHECK(v2.quota_m() == doctest::Approx(100.0).epsilon(0.02));
    // idroplani in avaria: la quota ordinata non si raggiunge
    LimitiBattello senza_piani;
    senza_piani.idroplani = false;
    auto g = battello("los_angeles", 15.0, 100.0);
    ComandiSottomarino su = cv;
    su.quota_m = 50.0;
    for (double t = 0; t < 300.0; t += DT) g.avanza(DT, su, {}, senza_piani);
    CHECK(g.quota_m() > 95.0);
}

TEST_CASE("dinamica: cavitazione V_inc(z) = V_inc0 sqrt((10,33 + z)/10,33)") {
    auto d = battello("los_angeles", 15.0, 100.0);
    CHECK(d.velocita_cavitazione_kn(0.0) == doctest::Approx(5.0));
    CHECK(d.velocita_cavitazione_kn(100.0) / 5.0 == doctest::Approx(3.27).epsilon(0.01));   // tabella del dossier 02
    CHECK(d.velocita_cavitazione_kn(300.0) / 5.0 == doctest::Approx(5.48).epsilon(0.01));
    CHECK_FALSE(d.cavita());   // 15 nodi a 100 m: sotto i 16,4
    ComandiSottomarino c;
    c.quota_m = 100.0;
    c.giri = d.giri_per_velocita(20.0);
    tieni(d, c, 30.0);
    CHECK(d.cavita());         // i giri cavitano prima che la velocita' arrivi
    auto p = battello("los_angeles", 10.0, 18.0);   // quota periscopica all'elica: 8,6 nodi
    CHECK(p.cavita());
    // a pari giri piu' in basso smette di cavitare
    auto q = battello("los_angeles", 10.0, 60.0);
    CHECK_FALSE(q.cavita());
}

namespace {

std::string missione_dinamica() {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_dinamica.lua";
    const std::string modello = "modello = { massima_kn = 30, virata_gradi_s = 3, accel_kn_s = 1, quota_m_s = 2, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {"
                        "V = function(u) m688.telegrafo(u, 2 / 3) m688.rotta(u, 90) m688.quota(u, 150, 1) m688.salva_modello(u) end"
                        "}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = -20000, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 16, " + modello + " },"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 },"
                        "  quota_m = 100, rotta = 0, velocita_kn = 0, " + modello + " },"
                        "\n}, timer = { { dopo_s = 0, unita = 2, gestore = 'V' } } } } }\n";
    return f.string();
}

}  // namespace

TEST_CASE("dinamica nella missione: solo con la regola di simulazione; timoniere dell'IA e comandi del giocatore") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto file = missione_dinamica();
    // livello classico: nessuna dinamica, il moto e' quello dell'originale (1 nodo al secondo)
    Missione classica(file, 1, false);
    for (int i = 0; i < 9 * 30; ++i) classica.avanza(DT);
    CHECK(classica.dinamica(2) == nullptr);
    const double ordinata = classica.unita().at(2).velocita_ordinata;   // 2/3 di avanti tutta (massima x 256/280)
    CHECK(classica.unita().at(2).velocita == doctest::Approx(ordinata));
    // Enhanced
    Missione m(file, 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    ComandiSottomarino g;
    g.giri = 0.5;
    g.barra_deg = 35.0;
    g.quota_m = 100.0;
    bool comandi = false;
    double v30 = 0.0, assetto = 0.0;
    for (int i = 0; i < 9 * 900; ++i) {
        m.avanza(DT);
        dm.avanza(m, DT);
        if (!comandi && m.con_dinamica()) { m.comandi_sottomarino(0, g); comandi = true; }
        if (i == 9 * 30) v30 = m.unita().at(2).velocita;
        if (const auto* d = m.dinamica(2)) assetto = std::min(assetto, d->assetto_deg());
    }
    REQUIRE(m.dinamica(2) != nullptr);
    REQUIRE(m.dinamica(0) != nullptr);
    const auto& v = m.unita().at(2);
    CHECK(v30 < 0.6 * ordinata);                         // la spinta del Victor non ci arriva in 30 s
    CHECK(v.velocita == doctest::Approx(ordinata).epsilon(0.05));
    CHECK(std::fabs(normalizza_180(v.rotta - 90.0)) < 1.0);
    CHECK(v.quota == doctest::Approx(150.0).epsilon(0.02));
    CHECK(assetto < -5.0);                               // e' sceso con la prua in giu'
    CHECK(assetto >= -20.0 - 1e-9);
    // il giocatore segue i suoi comandi: barra tutta e meta' dei giri, velocita' calata in accostata
    const auto& p = m.unita().at(0);
    CHECK(m.dinamica(0)->barra_deg() == doctest::Approx(35.0));
    CHECK(p.velocita < 0.75 * 16.0);
    CHECK(p.velocita > 0.55 * 16.0);
    CHECK(p.quota == doctest::Approx(100.0).epsilon(0.02));
    std::filesystem::remove(file);
}

TEST_CASE("dinamica nella missione: i giri danno la velocita' ordinata; Ctrl+E come primo ordine sale a 30 gradi") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const auto file = missione_dinamica();
    Missione m(file, 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    StazioniMissione st(m, 1);
    const auto passo = [&] { m.avanza(DT); st.dopo_passo(m); dm.avanza(m, DT); };
    for (int i = 0; i < 9 * 5; ++i) passo();
    REQUIRE(m.con_dinamica());
    REQUIRE(m.dinamica(0) != nullptr);
    // un ordine di giri: la velocita' ordinata del battello e' quella di regime dei giri
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::giri, 0.5, 0.0, 0}));
    CHECK(m.unita().at(0).velocita_ordinata == doctest::Approx(16.0));
    // un'altra partita: Ctrl+E e' il primo ordine di manovra, prima comandava il timoniere
    Missione e(file, 1, false);
    DanniMissione de(classi, M688_DATI, 1, RegolaDanni::simulazione);
    StazioniMissione se(e, 1);
    for (int i = 0; i < 9 * 5; ++i) { e.avanza(DT); se.dopo_passo(e); de.avanza(e, DT); }
    REQUIRE(e.dinamica(0) != nullptr);
    se.consegna(e, {e.tempo(), TipoOrdine::emersione_emergenza, 0.0, 0.0, 0});
    double assetto = 0.0;
    for (int i = 0; i < 9 * 120 && e.unita().at(0).quota > 1.0; ++i) {
        e.avanza(DT); se.dopo_passo(e); de.avanza(e, DT);
        assetto = std::max(assetto, e.dinamica(0)->assetto_deg());
    }
    CHECK(assetto > 25.0);           // il massimo dell'emergenza, non i 15 del timoniere
    CHECK(assetto <= 30.0 + 1e-9);
    std::filesystem::remove(file);
}
