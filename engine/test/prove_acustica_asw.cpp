// Acustica dell'ASW aereo (docs/13_asw_aereo.md, pezzo A2): sensori di boe e sonar a immersione, finestra di Snell,
// transitori, soglia delle boe sovietiche e l'asimmetria del dossier 17 §8.3.
#include <doctest.h>

#include <cmath>

#include "nucleo/acustica_asw.hpp"

using namespace m688;

namespace {
constexpr double KYD_M = 914.4;
}  // namespace

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}
const DatiAsw& dati() {
    static const DatiAsw d = carica_asw_aereo(M688_DATI "/asw_aereo.toml");
    return d;
}

Ambiente oceano(int mare = 3) {   // acqua profonda isoterma: la «buona propagazione» del dossier
    Ambiente a;
    a.fondale_m = 4000.0;
    a.vento_m_s = vento_per_stato_del_mare(mare);
    return a;
}

Sorgente sottomarino(const std::string& classe, Vec2 pos, double quota, double v, double rotta = 45.0, int id = 1) {
    UnitaAcustica u;
    u.id = id;
    u.classe = classe;
    u.categoria = "sottomarino";
    u.pos = pos;
    u.quota_m = quota;
    u.velocita_kn = v;
    u.rotta = rotta;
    return sorgente_acustica(catalogo(), u);
}

// la distanza massima a cui se(r) >= 0, a passi del 3% fra 50 m e 200 km (con la riverberazione la curva non e'
// monotona: niente bisezione)
template <class F> double portata(F se) {
    double ultima = 0.0;
    for (double r = 50.0; r < 200000.0; r *= 1.03)
        if (se(r) >= 0.0) ultima = r;
    return ultima;
}

}  // namespace

TEST_CASE("acustica asw: i sensori di boe e sonar a immersione ci sono tutti") {
    const auto& c = catalogo();
    for (const auto& [id, b] : dati().boe)
        if (!b.sensore.empty()) CHECK_MESSAGE(c.sensore(b.sensore) != nullptr, id);
    for (const auto& [id, s] : dati().sonar_immersione) {
        CHECK_MESSAGE(c.sensore(s.sensore) != nullptr, id);
        CHECK_MESSAGE(c.sensore(s.sensore + "_passivo") != nullptr, id);
        CHECK_MESSAGE(c.transitorio(transitorio_ping(s.sensore)) != nullptr, id);
    }
    for (const auto& [id, m] : dati().mezzi) CHECK_MESSAGE(c.transitorio(transitorio_aereo(id)) != nullptr, id);
    CHECK(c.sensore("ssq41b")->omni);
    CHECK(c.sensore("ssq53b")->sigma_rilevamento_deg == doctest::Approx(7.0));   // DIFAR: 5-9 gradi in mare
    CHECK(c.sensore("rgb2")->sigma_rilevamento_deg == doctest::Approx(6.0));     // 5-7 gradi
    CHECK(c.sensore("ssq62b")->modo == ModoSensore::attivo);
    CHECK(indice_direttivita(*c.sensore("ssq77a"), 300.0) == doctest::Approx(15.0));   // dato, non dalle dimensioni
    // i transitori stanno fuori dal paesaggio sonoro
    CHECK(c.sorgente("tonfo_boa") == nullptr);
    CHECK(c.transitorio("tonfo_boa") != nullptr);
}

TEST_CASE("acustica asw: ricevitori di boa e di sonar a immersione") {
    const auto& c = catalogo();
    Sonoboa b;
    b.id = 7001;
    b.tipo = "ssq53b";
    b.pos = {1000.0, 2000.0};
    b.deriva = {0.5, 0.0};   // 1 kn verso est
    b.quota_m = 27.4;
    b.t_impatto = 100.0;
    const Ricevitore r = ricevitore_boa(b, *dati().boa("ssq53b"), c, 200.0);
    CHECK(r.pos.x == doctest::Approx(1050.0));
    CHECK(r.quota_m == doctest::Approx(27.4));
    CHECK(r.rotta == doctest::Approx(90.0));
    CHECK(r.velocita_kn == doctest::Approx(0.5 / NODO));
    REQUIRE(r.sensore != nullptr);
    CHECK(r.sensore->id == "ssq53b");
    CHECK(ricevitore_boa(b, *dati().boa("ssq36"), c, 200.0).sensore == nullptr);   // il batitermografo non ascolta

    const DatiSonarImmersione& s = *dati().sonar("aqs13b");
    const Ricevitore d = ricevitore_dip({0, 0}, 100.0, s, c, true);
    CHECK(d.quota_m == doctest::Approx(100.0 - s.hover_m));
    CHECK(d.sensore->id == "aqs13b");
    CHECK(ricevitore_dip({0, 0}, 100.0, s, c, false).sensore->id == "aqs13b_passivo");
    CHECK(quota_trasduttore_m(5.0, 12.0) == doctest::Approx(0.0));   // ancora fuori dall'acqua
}

TEST_CASE("acustica asw: DIFAR da' il rilevamento, l'omni solo il contatto, l'attivo la distanza") {
    const auto& c = catalogo();
    const Ambiente amb = oceano(2);
    // al traverso della boa: l'anecoico del Victor III a prua lascia poca eco
    const Sorgente v3 = sottomarino("victor3", {2000.0, 1000.0}, 100.0, 12.0, rilevamento({2000.0, 1000.0}, {0, 0}) + 90.0);
    Casuale caso(42);
    Ricevitore dif{{0, 0}, 27.4, 0.0, 0.0, c.sensore("ssq53b")};
    const auto x = rivelazione(valuta(v3, dif, amb), *dif.sensore, dif, v3, 7001, 10.0, true, caso);
    REQUIRE(x.has_value());
    REQUIRE(x->rilevamento.has_value());
    CHECK(std::fabs(normalizza_180(*x->rilevamento - rilevamento({0, 0}, v3.pos))) < 4.0 * 7.0);
    CHECK_FALSE(x->distanza_m.has_value());
    CHECK(x->sottomarino);
    CHECK(x->sorgente == 1);
    CHECK(x->livello_db >= 0.0);

    Ricevitore omni{{0, 0}, 18.3, 0.0, 0.0, c.sensore("ssq41b")};
    const auto y = rivelazione(valuta(v3, omni, amb), *omni.sensore, omni, v3, 7002, 10.0, true, caso);
    REQUIRE(y.has_value());
    CHECK_FALSE(y->rilevamento.has_value());

    // la DICASS: un 688 che le viene incontro di traverso (il Doppler lo separa dalla riverberazione)
    const Vec2 dove{1500.0, 500.0};
    const Sorgente la = sottomarino("los_angeles", dove, 100.0, 10.0, rilevamento(dove, {0, 0}) + 60.0);
    Ricevitore dicass{{0, 0}, 121.9, 0.0, 0.0, c.sensore("ssq62b")};
    const auto z = rivelazione(valuta(la, dicass, amb), *dicass.sensore, dicass, la, 7003, 10.0, true, caso);
    REQUIRE(z.has_value());
    REQUIRE(z->distanza_m.has_value());
    CHECK(*z->distanza_m == doctest::Approx(distanza({0, 0}, dove)).epsilon(0.1));
    CHECK(z->rilevamento.has_value());

    // ripetibile dal seme
    Casuale a(7), b(7);
    const auto p = rivelazione(valuta(v3, dif, amb), *dif.sensore, dif, v3, 1, 0.0, true, a);
    const auto q = rivelazione(valuta(v3, dif, amb), *dif.sensore, dif, v3, 1, 0.0, true, b);
    CHECK(*p->rilevamento == *q->rilevamento);
}

TEST_CASE("acustica asw: portate delle boe come il dossier 17 §4") {
    const auto& c = catalogo();
    const Ambiente amb = oceano(2);
    const auto contro = [&](const char* boa, double quota, const char* classe, double v) {
        Ricevitore r{{0, 0}, quota, 0.0, 0.0, c.sensore(boa)};
        return portata([&](double x) { return valuta(sottomarino(classe, {x, 0}, 100.0, v), r, amb).se_db; });
    };
    // RGB-NM: 0,2-5 km, ~2,7 km contro un bersaglio rumoroso (CIA)
    CHECK(contro("rgb_nm", 20.0, "alfa", 12.0) == doctest::Approx(2700.0).epsilon(0.5));
    CHECK(contro("rgb_nm", 20.0, "los_angeles", 6.0) < 500.0);
    // RGB-1: SSK a 6-8 kn, 1,7-4 km (prove del 1965)
    const double rgb1 = contro("rgb1", 35.0, "foxtrot", 7.0);
    CHECK(rgb1 > 1200.0);
    CHECK(rgb1 < 6000.0);
    // la VLAD sente piu' lontano della DIFAR (+10-15 dB contro il traffico)
    CHECK(contro("ssq77a", 152.4, "victor3", 6.0) > 2.0 * contro("ssq53b", 27.4, "victor3", 6.0));
    // un 688 lento e silenzioso si sente solo da vicino
    CHECK(contro("ssq53b", 27.4, "los_angeles", 6.0) < 2000.0);
}

TEST_CASE("acustica asw: la boa sovietica trasmette solo un segnale che dura") {
    SogliaBoa s;
    CHECK_FALSE(s.passa(100.0, true, 12.0, 5.0));    // vale da 95 s: 5 s
    CHECK_FALSE(s.passa(105.0, true, 12.0, 5.0));    // 10 s
    CHECK(s.passa(110.0, true, 12.0, 5.0));          // 15 s: oltre i 12
    CHECK_FALSE(s.passa(115.0, false, 12.0, 5.0));   // giu': si riparte da capo
    CHECK_FALSE(s.passa(120.0, true, 12.0, 5.0));

    // nell'ascolto delle boe: un Alfa veloce vicino a una RGB-NM
    const auto& c = catalogo();
    Sonoboa b;
    b.id = 3001;
    b.tipo = "rgb_nm";
    b.quota_m = 20.0;
    b.t_attiva = 0.0;
    b.t_fine = 1e9;
    const std::vector<SorgenteAsw> sorgenti = {{sottomarino("alfa", {800.0, 0.0}, 100.0, 20.0, 45.0, 5), true}};
    const AmbienteFra amb = [](Vec2, Vec2) { return oceano(2); };
    Casuale caso(1);
    StatoAscoltoBoe stato;
    CHECK(ascolta_boe({b}, dati(), c, sorgenti, amb, 10.0, 2, caso, stato).empty());
    CHECK(ascolta_boe({b}, dati(), c, sorgenti, amb, 15.0, 2, caso, stato).empty());
    const auto r = ascolta_boe({b}, dati(), c, sorgenti, amb, 20.0, 2, caso, stato);
    REQUIRE(r.size() == 1);
    CHECK(r[0].boa_o_aereo == 3001);
    CHECK(r[0].sorgente == 5);
    CHECK_FALSE(r[0].rilevamento.has_value());
    // oltre mare 3 la RGB-NM e' inutile
    StatoAscoltoBoe altro;
    for (double t : {10.0, 15.0, 20.0, 25.0}) CHECK(ascolta_boe({b}, dati(), c, sorgenti, amb, t, 4, caso, altro).empty());
    // una boa guasta o non ancora stabilizzata non ascolta
    Sonoboa g = b;
    g.guasta = true;
    StatoAscoltoBoe terzo;
    for (double t : {10.0, 15.0, 20.0, 25.0}) CHECK(ascolta_boe({g}, dati(), c, sorgenti, amb, t, 2, caso, terzo).empty());
    // il vetrino di Dio (D1): il processore dell'aereo guasto attenua l'eccesso di segnale; spento non sente niente
    StatoAscoltoBoe quarto, quinto;
    for (double t : {10.0, 15.0}) {
        ascolta_boe({b}, dati(), c, sorgenti, amb, t, 2, caso, quarto, nullptr, -6.0);
        CHECK(ascolta_boe({b}, dati(), c, sorgenti, amb, t, 2, caso, quinto, nullptr, -1e9).empty());
    }
    const auto attenuata = ascolta_boe({b}, dati(), c, sorgenti, amb, 20.0, 2, caso, quarto, nullptr, -6.0);
    REQUIRE(attenuata.size() == 1);
    CHECK(attenuata[0].livello_db == doctest::Approx(r[0].livello_db - 6.0).epsilon(0.02));
    CHECK(ascolta_boe({b}, dati(), c, sorgenti, amb, 20.0, 2, caso, quinto, nullptr, -1e9).empty());
}

TEST_CASE("acustica asw: finestra di Snell e sorvolo") {
    const auto& c = catalogo();
    // P-3 a 91 m contro un 688 a 150 m: ~200 m di semilarghezza, ~3-4 s di ascolto a 200 kt
    const double x = finestra_snell_m(91.44, 150.0, 3);
    CHECK(x == doctest::Approx(0.18 * 91.44 + 1.2 * 150.0));
    CHECK(2.0 * x / (200.0 * NODO) == doctest::Approx(3.8).epsilon(0.1));
    // a quota periscopica il mare mosso allunga la finestra a 10-30 s
    const double y = finestra_snell_m(91.44, 18.0, 3);
    CHECK(2.0 * y / (200.0 * NODO) >= 10.0);
    CHECK(2.0 * y / (200.0 * NODO) <= 30.0);
    CHECK(probabilita_sorvolo(18.0) == doctest::Approx(0.95));
    CHECK(probabilita_sorvolo(150.0) == doctest::Approx(0.8));

    AereoAcustico p3;
    p3.id = 50;
    p3.classe = "p3c";
    p3.quota_m = 91.44;
    p3.velocita_kn = 200.0;
    const Ambiente amb = oceano(3);
    Ricevitore sfera{{100.0, 0.0}, 150.0, 0.0, 5.0, c.sensore("bqq5_sfera")};
    const auto dentro = sorgente_aerea(c, p3, sfera.pos, sfera.quota_m, 100.0);
    REQUIRE(dentro.has_value());
    CHECK(dentro->id == 50);
    CHECK(dentro->quota_m < 2.0);
    const double se = valuta(*dentro, sfera, amb).se_db;
    CHECK(se > 0.0);   // sulla verticale si sente
    CHECK_FALSE(sorgente_aerea(c, p3, {400.0, 0.0}, 150.0, 100.0).has_value());   // fuori dalla finestra: niente
    // il doppio della quota: -6 dB
    AereoAcustico alto = p3;
    alto.quota_m = 2.0 * 91.44;
    const auto lontano = sorgente_aerea(c, alto, sfera.pos, sfera.quota_m, 100.0);
    REQUIRE(lontano.has_value());
    CHECK(valuta(*lontano, sfera, amb).se_db == doctest::Approx(se - 20.0 * std::log10(2.0)).epsilon(0.02));
    CHECK(transitorio_aereo("ignoto").empty());

    // elicottero in hover: rumore di superficie entro 0,5-1 nm
    AereoAcustico h = p3;
    h.classe = "sh60b";
    h.quota_m = 12.0;
    h.hover = true;
    REQUIRE(sorgente_hover(c, h, 100.0).has_value());
    h.hover = false;
    CHECK_FALSE(sorgente_hover(c, h, 100.0).has_value());
    h.hover = true;
    const double hover = portata([&](double r) {
        sfera.pos = {r, 0.0};
        return valuta(*sorgente_hover(c, h, 100.0), sfera, amb).se_db;
    });
    CHECK(hover > 0.5 * 1852.0);
    CHECK(hover < 1.5 * 1852.0);
}

TEST_CASE("acustica asw: transitori nel tempo e a distanza") {
    const auto& c = catalogo();
    Transitorio t;
    t.id = 900001;
    t.tipo = "tonfo_boa";
    t.quota_m = 1.0;
    t.t0 = 500.0;
    CHECK_FALSE(sorgente_transitoria(c, t, 499.0).has_value());
    REQUIRE(sorgente_transitoria(c, t, 501.0).has_value());
    CHECK_FALSE(transitorio_finito(c, t, 504.0));
    CHECK(transitorio_finito(c, t, 506.0));
    CHECK_FALSE(sorgente_transitoria(c, t, 520.0).has_value());
    // l'impulso entra nella finestra che lo contiene e non nella successiva
    const auto s = sorgente_transitoria(c, t, 501.0, 2.0);
    CHECK(livello_totale_fonte(*s) > 100.0);
    CHECK(livello_totale_fonte(*sorgente_transitoria(c, t, 504.0, 2.0)) == doctest::Approx(0.0));

    // il tonfo di una boa si sente a 1-3 nm (§8.3)
    const Ambiente amb = oceano(3);
    std::vector<Ricevitore> r688 = {{{0, 0}, 150.0, 0.0, 5.0, c.sensore("bqq5_sfera")},
                                    {{0, 0}, 150.0, 0.0, 5.0, c.sensore("wlr9")}};
    const auto quanto = [&](const char* tipo, double finestra) {
        Transitorio x = t;
        x.tipo = tipo;
        return portata([&](double r) {
            x.pos = {r, 0.0};
            return ascolta_transitorio(*sorgente_transitoria(c, x, t.t0 + 1.0, finestra), r688, amb).se_db;
        });
    };
    const double tonfo = quanto("tonfo_boa", 2.0);
    CHECK(tonfo > 1.0 * 1852.0);
    CHECK(tonfo < 6.0 * 1852.0);
    CHECK(quanto("tonfo_siluro", 2.0) > tonfo);
    CHECK(quanto("sus_mk84", 2.0) > 1852.0);   // i toni a 3 kHz all'intercettatore
    // ascolto col rilevamento e il sensore
    Transitorio x = t;
    x.pos = {1000.0, 1000.0};
    const auto a = ascolta_transitorio(*sorgente_transitoria(c, x, t.t0 + 1.0), r688, amb);
    CHECK(a.sentito);
    CHECK(a.rilevamento == doctest::Approx(45.0));
    CHECK(a.errore_deg > 0.0);
    CHECK_FALSE(a.sensore.empty());

    // SSQ-47B: un ping ogni 10 s per tutta la vita della boa
    Transitorio p;
    p.id = 900002;
    p.tipo = "ping_ssq47b";
    p.t0 = 0.0;
    double sentiti = 0.0;
    for (double tt = 1.0; tt <= 100.0; tt += 1.0)
        if (livello_totale_fonte(*sorgente_transitoria(c, p, tt, 1.0)) > 100.0) sentiti += 1.0;
    CHECK(sentiti == doctest::Approx(10.0).epsilon(0.2));
}

TEST_CASE("acustica asw: il ping si sente 5-12 volte piu' lontano della scoperta dell'eco (§8.3)") {
    // Il dossier 17 §8.3 calcola 7-12x senza dire il ricevitore. Il WLR-9 e' senza guadagno d'antenna (idrofoni a
    // larga apertura, 26/10/2026: prima aveva per errore i 16 dB di un'antenna di 0,3 m a 10 kHz): ~6x
    const auto& c = catalogo();
    const Ambiente amb = oceano(3);
    for (const char* sonar : {"aqs13b", "ssq62b"}) {
        CAPTURE(sonar);
        Ricevitore attivo{{0, 0}, 60.0, 0.0, 0.0, c.sensore(sonar)};
        const double eco = portata([&](double r) { return valuta(sottomarino("los_angeles", {r, 0}, 150.0, 5.0), attivo, amb).se_db; });
        Transitorio ping;
        ping.id = 900003;
        ping.tipo = transitorio_ping(sonar);
        ping.quota_m = 60.0;
        Ricevitore wlr9{{0, 0}, 150.0, 0.0, 5.0, c.sensore("wlr9")};
        const double intercettazione = portata([&](double r) {
            wlr9.pos = {r, 0.0};
            return ascolta_transitorio(*sorgente_transitoria(c, ping, 0.5, 1.0), {wlr9}, amb).se_db;
        });
        CHECK(eco > 2000.0);
        CHECK(intercettazione / eco >= 5.0);
        CHECK(intercettazione / eco <= 12.0);
    }
}

TEST_CASE("acustica asw: riga di pala infrasonora (Ross, 60 log V) e RGB-75") {
    const auto& c = catalogo();
    const Sorgente a10 = sottomarino("los_angeles", {0, 0}, 150.0, 10.0);
    Sorgente a20 = sottomarino("los_angeles", {0, 0}, 150.0, 20.0), cava = a20;
    a20.cavitazione = false;
    cava.cavitazione = true;
    CHECK(livello_pala_db(cava) == doctest::Approx(livello_pala_db(a20) + 10.0));
    CHECK(frequenza_pala_hz(a10) == doctest::Approx(7.0));   // 7 pale x 6 giri/kn x 10 kn / 60
    CHECK(livello_pala_db(a20) - livello_pala_db(a10) == doctest::Approx(60.0 * std::log10(2.0)).epsilon(0.01));
    Sorgente posato = a10;
    posato.sul_fondo = true;
    CHECK(livello_pala_db(posato) == doctest::Approx(livello_pala_db(a10) - 10.0));
    CHECK(livello_pala_db(sottomarino("trafalgar", {0, 0}, 150.0, 10.0)) < -200.0);   // pump-jet: niente riga
    // la RGB-75 (2-60 Hz) sente le pale: vicino a bassa velocita', lontano veloce (dossier 17 §4.2)
    const Ambiente amb = oceano(1);
    const auto rgb75 = [&](const char* classe, double v) {
        Ricevitore r{{0, 0}, 150.0, 0.0, 0.0, c.sensore("rgb75")};
        return portata([&](double x) { return valuta(sottomarino(classe, {x, 0}, 150.0, v), r, amb).se_db; });
    };
    CHECK(rgb75("los_angeles", 4.5) <= 1000.0);
    CHECK(rgb75("los_angeles", 12.0) > 2.0 * rgb75("los_angeles", 6.0));
    CHECK(rgb75("alfa", 12.0) >= 5000.0);   // i battelli rumorosi da lontano
    Ricevitore r{{0, 0}, 150.0, 0.0, 0.0, c.sensore("rgb75")};
    const Rilevazione x = valuta(sottomarino("los_angeles", {8000.0, 0}, 150.0, 20.0), r, amb);
    CHECK(x.lofar);
    CHECK(x.banda_hz < 60.0);   // la riga di pala, non la tonale dei macchinari (fuori dalla gamma della boa)
    // i sensori di bordo senza gamma non prendono le pale (sotto la loro banda): la sfera resta sulla tonale
    Ricevitore sfera{{0, 0}, 150.0, 0.0, 5.0, c.sensore("bqq5_sfera")};
    CHECK(valuta(sottomarino("victor3", {8000.0, 0}, 150.0, 20.0), sfera, amb).banda_hz >= 100.0);
}

TEST_CASE("acustica asw: riverberazione di superficie e di fondo negli attivi") {
    const auto& c = catalogo();
    // Chapman-Harris: piu' vento e piu' angolo, piu' diffusione
    CHECK(forza_superficie_db(3500.0, 10.0, 5.0) > forza_superficie_db(3500.0, 3.0, 5.0));
    CHECK(forza_superficie_db(3500.0, 7.0, 10.0) > forza_superficie_db(3500.0, 7.0, 1.0));
    CHECK(forza_superficie_db(3500.0, 20.0, 30.0) <= -20.0);
    // AQS-13B contro un 688 (§6.4): 3-5 kyd di solito (il sottomarino sotto lo strato, il trasduttore sopra), 8-12 kyd
    // con buona propagazione, meno col mare grosso
    static const ClimaTeatro faroe = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    const auto aqs = [&](int mare, double quota_sub, bool strato) {
        Ambiente a = oceano(mare);
        if (strato) {
            a.profilo = &faroe.profilo(2);   // febbraio: strato a 250 m
            a.traffico = faroe.traffico;
        }
        Ricevitore r{{0, 0}, 60.0, 0.0, 0.0, c.sensore("aqs13b")};
        return portata([&](double x) { return valuta(sottomarino("los_angeles", {x, 0}, quota_sub, 5.0), r, a).se_db; });
    };
    const double tipico = aqs(3, 300.0, true) / KYD_M, buono = aqs(3, 150.0, true) / KYD_M, isoterma = aqs(3, 150.0, false) / KYD_M;
    CHECK(tipico >= 3.0);
    CHECK(tipico <= 6.0);
    CHECK(buono >= 8.0);
    CHECK(buono <= 12.0);
    CHECK(isoterma >= 8.0);
    CHECK(aqs(5, 300.0, true) < aqs(3, 300.0, true));
    // in acqua bassa la riverberazione del fondo accorcia gli attivi delle navi
    const auto sqs53 = [&](double fondo) {
        Ambiente a = oceano(3);
        a.fondale_m = fondo;
        Ricevitore r{{0, 0}, 0.0, 0.0, 12.0, c.sensore("sqs53_attivo")};
        return portata([&](double x) { return valuta(sottomarino("victor3", {x, 0}, std::min(100.0, fondo - 20.0), 8.0), r, a).se_db; });
    };
    CHECK(sqs53(150.0) < 0.7 * sqs53(3000.0));
}
