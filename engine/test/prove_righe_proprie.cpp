#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/analisi_acustica.hpp"

using namespace m688;

// Le righe del proprio battello sul LOFAR (blocco 2 del sonar, F; docs/22_sonar_blocco2.md §3)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Sorgente il_688(double kn) {
    UnitaAcustica u;
    u.id = 1;
    u.classe = "los_angeles";
    u.categoria = "sottomarino";
    u.quota_m = 150.0;
    u.velocita_kn = kn;
    return sorgente_acustica(catalogo(), u);
}

Ambiente quieto() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = 3.0;
    a.traffico = 0.3;
    return a;
}

double picco(const Spettro& s, double hz, double larga = 0.3) {
    double m = 0.0;
    for (std::size_t i = 0; i < s.snr_db.size(); ++i)
        if (std::fabs(s.frequenza(i) - hz) <= larga) m = std::max(m, static_cast<double>(s.snr_db[i]));
    return m;
}

}  // namespace

TEST_CASE("righe proprie: la sfera le sente in tutti i fasci, di piu' nei baffles; senza proprio niente") {
    const Sorgente io = il_688(10.0);
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 10.0, catalogo().sensore("bqq5_sfera")};
    const Ambiente amb = quieto();
    const Spettro senza = spettro_fascio({}, r, amb, 90.0, 20.0, 420.0, 0.1);
    r.proprio = &io;
    const Spettro prua = spettro_fascio({}, r, amb, 0.0, 20.0, 420.0, 0.1), traverso = spettro_fascio({}, r, amb, 90.0, 20.0, 420.0, 0.1),
                  poppa = spettro_fascio({}, r, amb, 180.0, 20.0, 420.0, 0.1);
    CHECK(picco(senza, 300.0) < 0.01);
    CHECK(picco(traverso, 300.0) > 6.0);   // la tonale della rete: una riga propria ben visibile
    CHECK(picco(prua, 300.0) == doctest::Approx(picco(traverso, 300.0)).epsilon(0.05));   // uguale in ogni fascio
    // nei baffles il rumore e' piu' alto, ma la riga entra 6 dB piu' forte: si vede ancora
    CHECK(picco(poppa, 300.0) > 3.0);
    // nella cuffia e nelle righe ricevute, con la sorgente -3
    bool c_e = false;
    for (const auto& x : righe_ricevute({}, r, amb, 90.0, 20.0, 420.0, 0.1)) c_e = c_e || (x.sorgente == -3 && std::fabs(x.hz - 300.0) < 0.1);
    CHECK(c_e);
    // un sensore senza isolamento dato (l'intercettatore) non le ha
    Ricevitore w{{0.0, 0.0}, 150.0, 0.0, 10.0, catalogo().sensore("wlr9")};
    w.proprio = &io;
    CHECK(righe_proprie(w, amb, 90.0, 0.1).empty());
}

TEST_CASE("righe proprie: cambiano coi giri e col regime delle pompe") {
    const Ambiente amb = quieto();
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("bqq5_sfera")};
    const Sorgente a5 = il_688(5.0), a10 = il_688(10.0), a20 = il_688(20.0);
    const auto frequenze = [&](const Sorgente& s, TipoRiga tipo) {
        r.proprio = &s;
        r.velocita_kn = s.velocita_kn;
        std::vector<double> f;
        for (const RigaPropria& x : righe_proprie(r, amb, 90.0, 0.1))
            if (x.tipo == tipo) f.push_back(x.hz);
        return f;
    };
    // il secondo stadio del riduttore (78 volte l'asse): a 10 kn il doppio che a 5
    const auto r5 = frequenze(a5, TipoRiga::riduttore), r10 = frequenze(a10, TipoRiga::riduttore);
    const auto c_e = [](const std::vector<double>& v, double hz) { return std::any_of(v.begin(), v.end(), [&](double x) { return std::fabs(x - hz) < 0.05; }); };
    CHECK(c_e(r5, 78.0 * 6.0 * 5.0 / 60.0));
    CHECK(c_e(r10, 78.0 * 6.0 * 10.0 / 60.0));
    CHECK(c_e(r10, 168.0));   // la riga fissa del riduttore resta dov'e'
    // le pompe: lente fino a 15 kn (42,3 Hz), veloci sopra (84,6 Hz)
    CHECK(c_e(frequenze(a10, TipoRiga::pompa), 42.3));
    CHECK_FALSE(c_e(frequenze(a10, TipoRiga::pompa), 84.6));
    CHECK(c_e(frequenze(a20, TipoRiga::pompa), 84.6));
    CHECK_FALSE(c_e(frequenze(a20, TipoRiga::pompa), 42.3));
    // il catalogo ha i nomi delle macchine
    for (const RigaPropria& x : righe_proprie(r, amb, 90.0, 0.1)) CHECK_FALSE(x.macchina.empty());
}

TEST_CASE("righe proprie: la schiera le sente solo verso il fuoco anteriore") {
    const Sorgente io = il_688(6.0);
    Ricevitore r{{0.0, 0.0}, 250.0, 0.0, 6.0, catalogo().sensore("tb16")};
    r.proprio = &io;
    r.cavo_filato_m = 790.0;
    const Ambiente amb = quieto();
    // le righe del battello per l'acqua (senza il cavo, che vibra in tutti i fasci)
    const auto del_battello = [&](double fascio) {
        int n = 0, cavo = 0;
        for (const RigaPropria& x : righe_proprie(r, amb, fascio, 0.05)) (x.macchina == "cavo" ? cavo : n)++;
        CHECK(cavo == 2);   // la vibrazione del cavo filato: la fondamentale e la seconda armonica (03/11/2026)
        return n;
    };
    CHECK(del_battello(15.0) > 0);    // 15 gradi dalla prua
    CHECK(del_battello(90.0) == 0);   // al traverso no
    CHECK(del_battello(180.0) == 0);
    // le righe del cavo sono quelle di righe_ricevute con la sorgente -2, quindi la maschera le toglie
    std::vector<double> cavo;
    for (const RigaPropria& x : righe_proprie(r, amb, 90.0, 0.05)) if (x.macchina == "cavo") cavo.push_back(x.hz);
    int ricevute = 0;
    for (const auto& x : righe_ricevute({}, r, amb, 90.0, 5.0, 1000.0, 0.05))
        if (x.sorgente == -2) {
            ++ricevute;
            CHECK(std::any_of(cavo.begin(), cavo.end(), [&](double h) { return std::fabs(h - x.hz) < 0.01; }));
        }
    CHECK(ricevute == 2);
}

TEST_CASE("righe proprie: senza maschera il sonarista si scambia per un 688, con la maschera no") {
    const Sorgente io = il_688(8.0);
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 8.0, catalogo().sensore("bqq5_sfera")};
    r.proprio = &io;
    const Ambiente amb = quieto();
    // nessun contatto: nello spettro ci sono solo le righe proprie
    std::vector<RigaOsservata> oss = righe_osservate(spettro_fascio({}, r, amb, 90.0, 10.0, 410.0, 0.1));
    REQUIRE(oss.size() >= 3);
    const auto ipotesi = confronta_firma(oss, catalogo());
    REQUIRE(!ipotesi.empty());
    MESSAGE("senza maschera: " << ipotesi.front().classe << " " << ipotesi.front().confidenza);
    CHECK(ipotesi.front().classe == "los_angeles");   // l'errore tipico (dossier 09 §3.9)
    // la maschera col catalogo toglie tutto
    std::vector<double> maschera;
    for (const RigaPropria& x : righe_proprie(r, amb, 90.0, 0.1)) maschera.push_back(x.hz);
    togli_righe(oss, maschera);
    CHECK(oss.empty());
}

TEST_CASE("righe proprie: la schiera danneggiata perde 6 dB e la banda sopra 300 Hz") {
    UnitaAcustica u;
    u.id = 2;
    u.classe = "victor3";
    u.categoria = "sottomarino";
    u.quota_m = 200.0;
    u.velocita_kn = 8.0;
    u.pos = {8000.0, 0.0};
    const Sorgente victor = sorgente_acustica(catalogo(), u);
    Ricevitore r{{0.0, 0.0}, 250.0, 0.0, 6.0, catalogo().sensore("tb16")};
    const Ambiente amb = quieto();
    const Rilevazione sana = valuta(victor, r, amb);
    r.schiera_danneggiata = true;
    const Rilevazione rotta = valuta(victor, r, amb);
    CHECK(rotta.se_db < sana.se_db - 3.0);
    CHECK(rotta.banda_hz <= 300.0);
    const Spettro sp = spettro_fascio({victor}, r, amb, 90.0, 200.0, 410.0, 0.1);
    CHECK(picco(sp, 360.0, 50.0) < 0.01);   // sopra 300 Hz (310-410) solo rumore
}
