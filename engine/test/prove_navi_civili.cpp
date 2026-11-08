#include <doctest.h>

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "nucleo/analisi_acustica.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/navi_civili.hpp"

using namespace m688;

// Navi civili generate per individuo (blocco 1 del sonar, docs/20_sonar_blocco1.md §2)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Sorgente sorgente(const FirmaRumore& f, double kn) {
    Sorgente s;
    s.firma = &f;
    s.nave = true;
    s.velocita_kn = kn;
    return s;
}

double hz_di(const std::vector<RigaEmessa>& righe, TipoRiga tipo, double vicino) {
    double migliore = 0.0;
    for (const auto& r : righe)
        if (r.tipo == tipo && (migliore == 0.0 || std::fabs(r.hz - vicino) < std::fabs(migliore - vicino))) migliore = r.hz;
    return migliore;
}

}  // namespace

TEST_CASE("navi civili: i tipi nel catalogo, la firma tipica e le classi") {
    const auto& c = catalogo();
    REQUIRE(c.civili.size() >= 13);
    for (const auto& [id, t] : c.civili) {
        CAPTURE(id);
        CHECK(t.classe == id);
        REQUIRE(c.firma(id) != nullptr);   // la firma tipica (o quella di sempre) per la libreria e le missioni
        CHECK(c.firma(id)->nave);
    }
    // le firme fatte a mano restano quelle
    CHECK(c.firma("mercantile")->fonte == "01 §3.2 mercantili 175-185");
    CHECK(c.firma("traghetto")->tipo_unita == "traghetto");
    CHECK(c.firma("traghetto")->giri_costanti > 0.0);   // passo variabile
    // le classi del traffico, civili
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    for (const char* k : {"mercantile", "cargo", "petroliera", "peschereccio", "traghetto", "bmrt", "rompighiaccio"}) {
        CAPTURE(k);
        REQUIRE(classi.count(k));
        CHECK(classi.at(k).civile);
        CHECK_FALSE(classi.at(k).modello.empty());
    }
    CHECK_FALSE(classi.at("perry").civile);
}

TEST_CASE("navi civili: lo stesso seme da' la stessa nave, dentro gli intervalli del tipo") {
    const TipoCivile& t = *catalogo().tipo_civile("cargo");
    const IndividuoCivile a = genera_nave_civile(t, 42), b = genera_nave_civile(t, 42), d = genera_nave_civile(t, 43);
    CHECK(a.lunghezza_m == b.lunghezza_m);
    REQUIRE(a.firma.righe.size() == b.firma.righe.size());
    for (std::size_t i = 0; i < a.firma.righe.size(); ++i) CHECK(a.firma.righe[i].hz == b.firma.righe[i].hz);
    CHECK(a.firma.punti == b.firma.punti);
    CHECK(a.lunghezza_m != d.lunghezza_m);
    for (const auto& [id, tc] : catalogo().civili) {
        CAPTURE(id);
        for (std::uint64_t s = 0; s < 50; ++s) {
            const IndividuoCivile n = genera_nave_civile(tc, s * 7919);
            CHECK(n.lunghezza_m >= tc.lunghezza_m.min);
            CHECK(n.lunghezza_m <= tc.lunghezza_m.max);
            CHECK(n.firma.pale >= tc.pale.min);
            CHECK(n.firma.pale <= tc.pale.max);
            CHECK(n.velocita_max_kn > n.velocita_servizio_kn);
            CHECK(n.firma.v_cavitazione_kn >= tc.cavitazione_kn.min);
            CHECK(n.firma.v_cavitazione_kn <= tc.cavitazione_kn.max);
            for (std::size_t i = 1; i < n.firma.punti.size(); ++i) CHECK(n.firma.punti[i].second >= n.firma.punti[i - 1].second);
        }
    }
}

TEST_CASE("navi civili: i livelli di servizio dentro le misure per tipo (ECHO, McKenna, Veirs)") {
    // media su 200 navi della banda larga 5-200 Hz alla velocita' di servizio
    const std::map<std::string, std::pair<double, double>> attesi = {
        {"cargo", {178.0, 192.0}},          // McKenna 2012: container 184-188 dB (20-1000 Hz)
        {"portarinfuse", {176.0, 190.0}},   // McKenna 2012: bulker 184-187
        {"petroliera", {176.0, 190.0}},     // McKenna 2012: greggio 179-182
        {"mercantile", {172.0, 186.0}},     // Veirs 2016: cargo 175 +- 5
        {"traghetto", {165.0, 182.0}},      // Veirs 2016: passeggeri 166 +- 8, piu' veloci
        {"rimorchiatore", {158.0, 178.0}},  // Veirs 2016: 170 +- 5
        {"nave_ricerca", {155.0, 175.0}},   // Veirs 2016: 167 +- 5
        {"peschereccio", {152.0, 175.0}},   // Veirs 2016: 164 +- 9
    };
    for (const auto& [id, intervallo] : attesi) {
        const TipoCivile& t = *catalogo().tipo_civile(id);
        double somma = 0.0;
        for (std::uint64_t s = 0; s < 200; ++s) {
            const IndividuoCivile n = genera_nave_civile(t, s);
            somma += livello_banda_larga(sorgente(n.firma, n.velocita_servizio_kn));
        }
        const double media = somma / 200.0;
        CAPTURE(id);
        MESSAGE(id << ": " << media << " dB a banda larga al servizio");
        CHECK(media >= intervallo.first);
        CHECK(media <= intervallo.second);
    }
    // sotto l'innesco della cavitazione 6 dB in meno, e il DEMON spento
    const IndividuoCivile n = genera_nave_civile(*catalogo().tipo_civile("portarinfuse"), 7);
    const double cis = n.firma.v_cavitazione_kn;
    const double sopra = livello_banda_larga(sorgente(n.firma, cis * 1.01)), sotto = livello_banda_larga(sorgente(n.firma, cis * 0.97));
    CHECK(sopra - sotto > 5.0);
    Ricevitore r;
    r.quota_m = 100.0;
    r.sensore = catalogo().sensore("tb16");
    Ambiente amb;
    amb.fondale_m = 3000.0;
    Sorgente vicina = sorgente(n.firma, cis * 1.2);
    vicina.pos = {3000.0, 0.0};
    const auto d_sopra = leggi_demon(demon(vicina, r, amb));
    vicina.velocita_kn = cis * 0.9;
    const auto d_sotto = leggi_demon(demon(vicina, r, amb));
    REQUIRE(d_sopra.has_value());
    CHECK(d_sopra->pale == n.firma.pale);
    CHECK_FALSE(d_sotto.has_value());
    // le navi da guerra hanno l'innesco nella firma solo per il DEMON (passo B, 29/10/2026: 11 nodi le americane, STIMA):
    // la Perry sotto non ha DEMON, sopra si', con le sue cinque pale
    Sorgente perry;
    perry.firma = catalogo().firma("perry");
    perry.velocita_kn = 9.0;
    perry.pos = {3000.0, 0.0};
    CHECK_FALSE(leggi_demon(demon(perry, r, amb)).has_value());
    perry.velocita_kn = 20.0;
    const auto d_perry = leggi_demon(demon(perry, r, amb));
    REQUIRE(d_perry.has_value());
    CHECK(d_perry->pale == 5);
}

TEST_CASE("navi civili: le righe del diesel lento seguono i giri, quelle col passo variabile restano") {
    // portarinfuse: 2 tempi in presa diretta, le accensioni a cilindri x asse
    const IndividuoCivile b = genera_nave_civile(*catalogo().tipo_civile("portarinfuse"), 3);
    const auto r8 = righe_emesse(sorgente(b.firma, 8.0)), r14 = righe_emesse(sorgente(b.firma, 14.0));
    double cil = 0.0;
    for (const auto& r : b.firma.righe)
        if (r.multiplo_asse > 0.0 && r.tipo == TipoRiga::diesel) { cil = r.multiplo_asse; break; }
    REQUIRE(cil >= 4.0);
    const double asse8 = b.firma.giri_per_nodo * 8.0 / 60.0, asse14 = b.firma.giri_per_nodo * 14.0 / 60.0;
    CHECK(hz_di(r8, TipoRiga::diesel, cil * asse8) == doctest::Approx(cil * asse8));
    CHECK(hz_di(r14, TipoRiga::diesel, cil * asse14) == doctest::Approx(cil * asse14));
    // il rapporto fra accensioni e pala e' fisso (indizio di classificazione, 26 §2.2)
    CHECK(hz_di(r14, TipoRiga::elica, b.firma.pale * asse14) == doctest::Approx(b.firma.pale * asse14));
    // traghetto a passo variabile: asse, pala e motori fermi in frequenza quando cambia la velocita'
    const IndividuoCivile f = genera_nave_civile(*catalogo().tipo_civile("traghetto"), 5);
    const auto a = righe_emesse(sorgente(f.firma, 10.0)), c = righe_emesse(sorgente(f.firma, 20.0));
    REQUIRE(a.size() == c.size());
    for (std::size_t i = 0; i < a.size(); ++i) CHECK(a[i].hz == doctest::Approx(c[i].hz));
    // la rete: 50 o 60 Hz con le armoniche
    const auto rete = hz_di(righe_emesse(sorgente(f.firma, 15.0)), TipoRiga::rete, 50.0);
    CHECK((rete == doctest::Approx(50.0) || rete == doctest::Approx(60.0)));
}

TEST_CASE("navi civili: individui dello stesso tipo si distinguono sul LOFAR") {
    // due portacontainer: righe diverse (cilindri, pale, giri, generatori), la stessa classe per la libreria
    const TipoCivile& t = *catalogo().tipo_civile("cargo");
    int diverse = 0;
    const IndividuoCivile riferimento = genera_nave_civile(t, 1000);
    for (std::uint64_t s = 1001; s < 1021; ++s) {
        const IndividuoCivile n = genera_nave_civile(t, s);
        diverse += std::fabs(n.firma.tonale_hz - riferimento.firma.tonale_hz) > 0.5 || n.firma.pale != riferimento.firma.pale;
    }
    CHECK(diverse >= 15);
}
