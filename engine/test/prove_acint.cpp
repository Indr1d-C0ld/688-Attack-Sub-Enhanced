// Registratore dei contatti e rapporto ACINT (S5d).
#include <doctest.h>

#include <cmath>

#include "nucleo/acint.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Ambiente faroe() {
    static const ClimaTeatro c = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente a;
    a.profilo = &c.profilo(2);
    a.fondale_m = 3000.0;
    a.vento_m_s = 7.0;
    a.traffico = c.traffico;
    return a;
}

// un contatto seguito dalla schiera per `minuti`: un campione ogni 30 s con le righe del fascio
void segui(Registratore& reg, int id, const char* classe, const char* categoria, double kn, double distanza_m, int minuti,
           double extra_hz = 0.0) {
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    UnitaAcustica u;
    u.id = id;
    u.classe = classe;
    u.categoria = categoria;
    u.quota_m = std::string(categoria) == "nave" ? 0.0 : 150.0;
    u.velocita_kn = kn;
    u.pos = {distanza_m, 0.0};
    const Sorgente s = sorgente_acustica(catalogo(), u);
    const auto righe = righe_osservate(spettro_fascio({s}, r, faroe(), 90.0, 10.0, 410.0, 0.1));
    for (int k = 0; k <= minuti * 2; ++k) {
        CampioneContatto c;
        c.t = 600.0 + 30.0 * k;
        c.rilevamento = 90.0 - 0.05 * k;
        c.livello_db = 8.0;
        c.righe = righe;
        if (extra_hz > 0.0 && k % 3 != 0) c.righe.push_back({extra_hz, 9.0});   // presente due volte su tre
        c.sensore = "tb16";
        reg.registra(id, c);
    }
}

}  // namespace

TEST_CASE("acint: registratore con la storia lunga dimezzata, righe stabili") {
    Registratore reg;
    CHECK(reg.vuoto());
    for (int i = 0; i < 5000; ++i) {
        CampioneContatto c;
        c.t = i * 5.0;
        c.righe = {{100.0 + 0.001 * (i % 7), 10.0}};
        if (i % 5 == 0) c.righe.push_back({237.0, 6.0});   // una volta su cinque
        reg.registra(7, c);
    }
    const auto* s = reg.storia(7);
    REQUIRE(s != nullptr);
    CHECK(s->size() <= Registratore::MASSIMO);
    CHECK(s->front().t == doctest::Approx(0.0));      // l'inizio resta
    CHECK(s->back().t == doctest::Approx(24995.0));   // e la fine
    CHECK(reg.contatti() == std::vector<int>{7});
    const auto stabili = righe_stabili(*s);
    REQUIRE(stabili.size() == 1);                     // la riga dei 237 Hz e' sotto la presenza minima
    CHECK(stabili[0].hz == doctest::Approx(100.003).epsilon(1e-4));
    CHECK(stabili[0].presenza == doctest::Approx(1.0));
    CHECK(stabili[0].deriva_hz == doctest::Approx(0.006).epsilon(0.01));
    CHECK(righe_stabili(*s, 0.004, 0.15).size() == 2);
    reg.dimentica(7);
    CHECK(reg.vuoto());
}

TEST_CASE("acint: classificazione dalla storia e righe nuove per la libreria") {
    Registratore reg;
    segui(reg, 3, "charlie2", "sottomarino", 8.0, 3000.0, 20, 333.3);
    const VoceAcint v = classifica_contatto(3, *reg.storia(3), catalogo());
    CHECK(v.classe == "charlie2");
    CHECK(v.sottomarino);
    CHECK(v.confidenza >= Confidenza::probsub);
    CHECK(v.t_ultimo - v.t_primo == doctest::Approx(1200.0));
    bool nuova = false;
    for (const auto& r : v.nuove) nuova |= std::fabs(r.hz - 333.3) < 0.5;
    CHECK(nuova);   // la riga che il Charlie della libreria non ha
    REQUIRE_FALSE(v.testo_it.empty());
    CHECK(v.testo_it[0].find("S3") == 0);
    CHECK(v.testo_it[0].find(nome_confidenza(v.confidenza)) != std::string::npos);
    CHECK(v.testo_en[0].find("bearing") != std::string::npos);
}

TEST_CASE("acint: una nave e' NONSUB, un contatto senza righe POSSUB; rapporto di fine missione") {
    Registratore reg;
    // la nave: le righe della sua voce di libreria, come le leggerebbe un buon operatore da vicino
    const auto perry = voce_libreria(catalogo(), "perry");
    REQUIRE(perry.has_value());
    for (int k = 0; k <= 30; ++k) {
        CampioneContatto c;
        c.t = 30.0 * k;
        for (const auto& r : perry->righe) c.righe.push_back({r.hz, 10.0 + r.db_relativo});
        reg.registra(1, c);
    }
    segui(reg, 2, "charlie2", "sottomarino", 8.0, 3000.0, 20);
    CampioneContatto vuoto;
    vuoto.t = 100.0;
    reg.registra(9, vuoto);
    vuoto.t = 400.0;
    reg.registra(9, vuoto);
    const VoceAcint nave = classifica_contatto(1, *reg.storia(1), catalogo());
    CHECK_FALSE(nave.sottomarino);
    CHECK(nave.confidenza == Confidenza::nonsub);
    const VoceAcint ignoto = classifica_contatto(9, *reg.storia(9), catalogo());
    CHECK(ignoto.classe.empty());
    CHECK(ignoto.confidenza == Confidenza::possub);
    const RapportoAcint r = rapporto_acint(reg, catalogo(), 60.0);
    CHECK(r.voci.size() == 3);
    CHECK(r.testo_it.front().find("RAPPORTO ACINT: 3 contatti") == 0);
    CHECK(rapporto_acint(reg, catalogo(), 1000.0).voci.size() == 1);   // solo il Charlie e' stato seguito tanto
    CHECK(std::string(nome_confidenza(Confidenza::certsub)) == "CERTSUB");
}

TEST_CASE("acint: una riga sola non da' una classe, anche se concorda con due righe del catalogo") {
    // la riga a 120 Hz del 688, l'unica che il sonarista medio non maschera in M1: prima bastava per «kidd» su ogni
    // contatto, perche' concordava con due righe della voce e il minimo contava le coppie
    const Classificazione k = classifica_righe({{120.0, 15.0}}, std::nullopt, catalogo());
    CHECK(k.classe.empty());
    // due righe distinte bastano ancora
    bool una_classe = false;
    for (const auto& [classe, f] : catalogo().firme) {
        std::vector<RigaOsservata> due;
        for (const auto& r : f.righe)
            if (r.multiplo_asse == 0 && due.size() < 2 && (due.empty() || std::fabs(due.front().hz - r.hz) > 5.0)) due.push_back({r.hz, 15.0});
        if (due.size() == 2 && !classifica_righe(due, std::nullopt, catalogo()).classe.empty()) { una_classe = true; break; }
    }
    CHECK(una_classe);
}
