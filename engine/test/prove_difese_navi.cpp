#include <doctest.h>

#include <set>
#include <string>
#include <vector>

#include "nucleo/classi.hpp"
#include "nucleo/difese_navi.hpp"
#include "nucleo/missili.hpp"

using namespace m688;

// Le tre regole delle difese delle navi, il modulo puro (nucleo/difese_navi; missione_missili.cpp le chiama all'impatto):
// allerta, saturazione, Phalanx. I valori attesi delle frequenze vengono dal calcolo esatto, senza il motore:
// tools/re/prove/difese_navi/calcolo_esiti.py (valori_prova.py li stampa).

namespace {

constexpr double HARPOON = 240.0, P500 = 450.0;
constexpr int ANNO = 1989;

DifesaClasse classe(int phalanx, int canali, int block1_dal = 0) {
    DifesaClasse d;
    d.phalanx = phalanx;
    d.canali = canali;
    d.block1_dal = block1_dal;
    return d;
}

// Un generatore con il primo tiro scelto: il byte (`r`, 0-255) e, se si chiede, il secondo (`u`, 0-1). Si cerca un seme: la
// prova guarda solo la regola, non la sequenza
Casuale con_tiri(int r, double u = -1.0) {
    for (std::uint64_t s = 1;; ++s) {
        Casuale prova(s);
        if (static_cast<int>(prova.prossimo() >> 56) != r) continue;
        if (u >= 0.0 && (prova.uniforme() < u) != (u > 0.5)) continue;
        return Casuale(s);
    }
}

constexpr int TIRO_SEMPRE_FERMA = 0, TIRO_MAI_FERMA = 255;

struct Salva { double colpi = 0.0, almeno_uno = 0.0; };

// come la missione: il primo missile con l'allerta di partenza, gli altri ad A3 (il primo l'ha svegliata)
Salva frequenze(const DifesaClasse& d, int byte, double velocita, const std::vector<double>& tempi, std::optional<int> allerta, int prove = 20000) {
    Casuale caso(0xD1FE5A);
    int colpi = 0, con_colpo = 0;
    for (int k = 0; k < prove; ++k) {
        ImpegnoDifese impegno;
        int qui = 0;
        for (std::size_t i = 0; i < tempi.size(); ++i)
            if (impegno.arrivo(&d, byte, i == 0 ? fattore_allerta(allerta) : 1.0, tempi[i], velocita, ANNO, caso) == EsitoDifesa::colpo) ++qui;
        colpi += qui;
        if (qui) ++con_colpo;
    }
    return {static_cast<double>(colpi) / prove, static_cast<double>(con_colpo) / prove};
}

const std::vector<double> UNO = {0.0}, DUE = {0.0, 4.0}, QUATTRO = {0.0, 4.0, 8.0, 12.0}, GIOCATORE = {0.0, 4.0, 20.0, 24.0};

}  // namespace

TEST_CASE("difese delle navi: l'allerta scala il byte; A2, A3 e forze spente lo lasciano intero") {
    CHECK(fattore_allerta(0) == 0.5);
    CHECK(fattore_allerta(1) == 0.75);
    CHECK(fattore_allerta(2) == 1.0);
    CHECK(fattore_allerta(3) == 1.0);
    CHECK(fattore_allerta(std::nullopt) == 1.0);
    CHECK(byte_in_allerta(90, 0.5) == 45);
    CHECK(byte_in_allerta(90, 0.75) == 67);   // 67,5 in giu'
    CHECK(byte_in_allerta(90, 1.0) == 90);
    const DifesaClasse senza = classe(0, 2);
    {   // un tiro di 50 ferma col byte 90 ai posti di combattimento, non ad A0 (45)
        ImpegnoDifese i;
        Casuale c = con_tiri(50);
        CHECK(i.arrivo(&senza, 90, 1.0, 0.0, HARPOON, ANNO, c) == EsitoDifesa::fermato_byte);
    }
    {
        ImpegnoDifese i;
        Casuale c = con_tiri(50);
        CHECK(i.arrivo(&senza, 90, 0.5, 0.0, HARPOON, ANNO, c) == EsitoDifesa::colpo);
    }
}

TEST_CASE("difese delle navi: con i canali impegnati il byte non conta, e dopo 30 s il canale e' libero") {
    const DifesaClasse due = classe(0, 2);
    ImpegnoDifese i;
    Casuale c1 = con_tiri(TIRO_SEMPRE_FERMA), c2 = con_tiri(TIRO_SEMPRE_FERMA), c3 = con_tiri(TIRO_SEMPRE_FERMA), c4 = con_tiri(TIRO_SEMPRE_FERMA), c5 = con_tiri(TIRO_SEMPRE_FERMA);
    CHECK(i.arrivo(&due, 90, 1.0, 0.0, HARPOON, ANNO, c1) == EsitoDifesa::fermato_byte);
    CHECK(i.arrivo(&due, 90, 1.0, 4.0, HARPOON, ANNO, c2) == EsitoDifesa::fermato_byte);
    CHECK(i.arrivo(&due, 90, 1.0, 8.0, HARPOON, ANNO, c3) == EsitoDifesa::colpo);   // saturi: anche col tiro piu' basso
    CHECK(i.arrivo(&due, 90, 1.0, 29.9, HARPOON, ANNO, c4) == EsitoDifesa::colpo);
    CHECK(i.arrivo(&due, 90, 1.0, 30.0, HARPOON, ANNO, c5) == EsitoDifesa::fermato_byte);   // il primo si e' liberato
}

TEST_CASE("difese delle navi: un affusto contro Harpoon a 4 s ne ingaggia uno si' e uno no, contro P-500 tutti; due affusti tutti") {
    // uno e' impegnato 6,2 s da un Harpoon e 3,3 s da un P-500; il byte non ferma nessuno. Il tiro sotto Pk ferma
    const auto tiri = [](const DifesaClasse& d, double v) {
        ImpegnoDifese i;
        std::vector<EsitoDifesa> e;
        for (int k = 0; k < 4; ++k) {
            Casuale c = con_tiri(TIRO_MAI_FERMA, 0.0);   // secondo tiro: il piu' basso possibile, ogni ingaggio ferma
            e.push_back(i.arrivo(&d, 90, 1.0, 4.0 * k, v, ANNO, c));
        }
        return e;
    };
    using E = std::vector<EsitoDifesa>;
    const auto F = EsitoDifesa::fermato_phalanx, C = EsitoDifesa::colpo;
    CHECK(tiri(classe(1, 8), HARPOON) == E{F, C, F, C});
    CHECK(tiri(classe(1, 8), P500) == E{F, F, F, F});
    CHECK(tiri(classe(2, 8), HARPOON) == E{F, F, F, F});
    CHECK(tiri(classe(0, 8), HARPOON) == E{C, C, C, C});
}

TEST_CASE("difese delle navi: il Pk del Phalanx, Block 1 dall'anno della classe, meta' contro un supersonico, scalato con l'allerta") {
    const DifesaClasse perry = classe(1, 1), iowa = classe(4, 1, 1988);
    CHECK(pk_phalanx(perry, 1989, HARPOON, 1.0) == doctest::Approx(0.35));
    CHECK(pk_phalanx(perry, 1989, 300.0, 1.0) == doctest::Approx(0.35));   // P-120: sotto la soglia
    CHECK(pk_phalanx(perry, 1989, P500, 1.0) == doctest::Approx(0.175));
    CHECK(pk_phalanx(iowa, 1987, HARPOON, 1.0) == doctest::Approx(0.35));
    CHECK(pk_phalanx(iowa, 1988, HARPOON, 1.0) == doctest::Approx(0.45));
    CHECK(pk_phalanx(iowa, 1989, P500, 1.0) == doctest::Approx(0.225));
    CHECK(pk_phalanx(perry, 1989, HARPOON, 0.5) == doctest::Approx(0.175));
    CHECK(pk_phalanx(perry, 1989, HARPOON, 0.75) == doctest::Approx(0.2625));
}

TEST_CASE("difese delle navi: niente secondo tiro senza Phalanx libero o se il byte ha gia' fermato") {
    const auto dopo = [](const DifesaClasse& d, int primo) {   // il prossimo numero del generatore dopo un arrivo
        Casuale c = con_tiri(primo);
        Casuale riferimento = c;
        ImpegnoDifese i;
        i.arrivo(&d, 90, 1.0, 0.0, HARPOON, ANNO, c);
        riferimento.prossimo();
        return c.prossimo() == riferimento.prossimo();   // vero: e' stato estratto solo il byte
    };
    CHECK(dopo(classe(0, 2), TIRO_MAI_FERMA));
    CHECK(dopo(classe(1, 2), TIRO_SEMPRE_FERMA));
    CHECK_FALSE(dopo(classe(1, 2), TIRO_MAI_FERMA));
}

TEST_CASE("difese delle navi: una classe fuori tabella resta come prima: stessi esiti, stessa sequenza, nessun limite") {
    Casuale di_prima(0x688), oggi(0x688);
    ImpegnoDifese i;
    for (int k = 0; k < 1000; ++k) {
        const int r = static_cast<int>(di_prima.prossimo() >> 56);
        const auto e = i.arrivo(nullptr, 90, 0.5, k * 0.1, HARPOON, ANNO, oggi);   // il fattore non conta
        CHECK((e == EsitoDifesa::fermato_byte) == (r < 90));
        CHECK((e == EsitoDifesa::colpo) == (r >= 90));
    }
    CHECK(di_prima.prossimo() == oggi.prossimo());
}

// i valori di calcolo_esiti.py, entro l'1%
TEST_CASE("difese delle navi: le frequenze su 20000 salve sono quelle del calcolo esatto") {
    const auto controlla = [](const DifesaClasse& d, int byte, double v, const std::vector<double>& t, std::optional<int> al, double colpi, double almeno) {
        const Salva s = frequenze(d, byte, v, t, al);
        CHECK(s.colpi == doctest::Approx(colpi).epsilon(0.01));
        CHECK(s.almeno_uno == doctest::Approx(almeno).epsilon(0.01));
    };
    const DifesaClasse krivak = classe(0, 2), perry = classe(1, 1), kidd = classe(2, 2), iowa = classe(4, 1, 1988), spruance = classe(2, 1);
    // un missile, per allerta
    controlla(krivak, 90, HARPOON, UNO, 0, 0.8242, 0.8242);
    controlla(krivak, 90, HARPOON, UNO, 2, 0.6484, 0.6484);
    controlla(perry, 15, HARPOON, UNO, 0, 0.8024, 0.8024);
    controlla(perry, 15, HARPOON, UNO, 2, 0.6119, 0.6119);
    controlla(perry, 128, HARPOON, UNO, 2, 0.3250, 0.3250);
    controlla(kidd, 16, HARPOON, UNO, 2, 0.6094, 0.6094);
    controlla(kidd, 16, P500, UNO, 2, 0.7734, 0.7734);
    controlla(iowa, 17, HARPOON, UNO, 2, 0.5135, 0.5135);
    controlla(spruance, 0, HARPOON, UNO, std::nullopt, 0.65, 0.65);
    // le salve: la saturazione e la sorpresa
    controlla(krivak, 90, HARPOON, DUE, 0, 1.4727, 0.9382);
    controlla(krivak, 90, HARPOON, DUE, 2, 1.2969, 0.8764);
    controlla(krivak, 90, HARPOON, QUATTRO, 2, 3.2969, 1.0);
    controlla(krivak, 90, HARPOON, GIOCATORE, 2, 3.2969, 1.0);
    controlla(perry, 128, HARPOON, QUATTRO, 2, 2.8000, 1.0);
    controlla(perry, 128, P500, QUATTRO, 2, 2.8875, 0.9969);
    controlla(kidd, 16, HARPOON, QUATTRO, 2, 2.5187, 0.9813);
    controlla(kidd, 16, P500, QUATTRO, 2, 3.1969, 0.9984);
    controlla(iowa, 17, HARPOON, QUATTRO, 2, 2.1635, 0.9557);
}

TEST_CASE("difese delle navi: stesso seme, stesso esito") {
    const DifesaClasse d = classe(2, 2);
    const auto giro = [&d] {
        Casuale c(77);
        ImpegnoDifese i;
        std::vector<EsitoDifesa> e;
        for (int k = 0; k < 200; ++k) e.push_back(i.arrivo(&d, 90, k % 3 ? 1.0 : 0.5, k * 2.0, HARPOON, ANNO, c));
        return e;
    };
    CHECK(giro() == giro());
}

#ifdef M688_DATI
// la tabella vera: data/motore/missili.toml, [[difesa_nave]]
TEST_CASE("difese delle navi: la tabella delle classi nel catalogo dei missili") {
    const CatalogoMissili cat = carica_missili(M688_DATI "/missili.toml");
    const auto c = [&cat](const std::string& k) { return cat.difesa(k); };
    REQUIRE(c("perry"));
    CHECK(c("perry")->phalanx == 1);
    CHECK(c("perry")->canali == 1);
    REQUIRE(c("iowa"));
    CHECK(c("iowa")->phalanx == 4);
    CHECK(c("iowa")->block1_dal == 1988);
    REQUIRE(c("udaloy"));
    CHECK(c("udaloy")->phalanx == 0);
    CHECK(c("udaloy")->canali == 4);
    REQUIRE(c("spruance"));
    CHECK(c("spruance")->phalanx == 2);
    CHECK(c("spruance")->confidenza == "STIMA");
    CHECK(c("cargo") == nullptr);
    CHECK(c("nessuna") == nullptr);
    // solo la Iowa ha il Block 1; ogni riga ha almeno un canale, la fonte e una classe vera
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    std::set<std::string> col_phalanx;
    for (const auto& [k, d] : cat.difese) {
        CAPTURE(k);
        CHECK(classi.count(k) == 1);
        CHECK(d.canali >= 1);
        CHECK_FALSE(d.fonte.empty());
        CHECK_FALSE(d.confidenza.empty());
        if (d.block1_dal) CHECK(k == "iowa");
        if (d.phalanx) col_phalanx.insert(k);
    }
    CHECK(col_phalanx == std::set<std::string>{"iowa", "kidd", "kitty_hawk", "knox", "nimitz", "perry", "sacramento", "spruance", "ticonderoga"});
}
#endif
