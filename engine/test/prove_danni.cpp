#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <functional>
#include <string>

#include "nucleo/casuale.hpp"
#include "nucleo/danni.hpp"

using namespace m688;
using doctest::Approx;

namespace {
// Chiatta a parallelepipedo 100 x 10 x 10 m con 5 m d'immersione, 50 stazioni, 10 locali da 10 m (permeabilita' 1);
// il locale centrale (x 0..10) e' diviso in una cassa di dritta larga 3 m e nel resto.
DatiScafo chiatta(bool cassa = false) {
    DatiScafo d;
    d.tipo = "militare"; d.GM = 1.0; d.lunghezza = 100; d.larghezza = 10; d.passo = 2.0;
    d.squadre = 0;   // nessuna tamponatura: la falla resta aperta
    for (int i = 0; i < 50; ++i) {
        Stazione s;
        s.x = -50 + 2.0 * (i + 0.5); s.z_chiglia = -5; s.dz = 0.25;
        s.semilarghezze.assign(41, 5.0);
        d.stazioni.push_back(s);
    }
    for (int k = 0; k < 10; ++k) {
        CellaScafo c;
        c.nome = "L" + std::to_string(k); c.x0 = -50 + 10.0 * k; c.x1 = c.x0 + 10; c.permeabilita = 1.0;
        if (cassa && k == 5) {
            CellaScafo w = c; w.nome = "L5D"; w.y1 = -2.0; c.y0 = -2.0;
            d.celle.push_back(w);
        }
        d.celle.push_back(c);
    }
    return d;
}
}  // namespace

TEST_CASE("danni: chiatta intatta in equilibrio") {
    DanniNave n(chiatta(), 1);
    CHECK(n.dislocamento_intatto_t() == Approx(100 * 10 * 5 * 1.025).epsilon(0.01));
    n.avanza(300);
    CHECK(std::fabs(n.immersione_m()) < 1e-3);
    CHECK(std::fabs(n.sbandamento_gradi()) < 1e-3);
    CHECK(std::fabs(n.appoppamento_gradi()) < 1e-3);
}

TEST_CASE("danni: locale centrale allagato (metodo della spinta perduta)") {
    DanniNave n(chiatta(), 1);
    // falla sul fondo del locale x 0..10: l'acqua sale al livello del mare; affondamento s = l T / (L - l)
    n.aggiungi_falla({5, 5.0, 0.0, -5.0, 1.0});
    n.avanza(900);
    const double s = 10.0 * 5.0 / 90.0;
    CHECK(n.immersione_m() == Approx(s).epsilon(0.03));
    CHECK(n.acqua_imbarcata_t() == Approx(10 * 10 * (5 + s) * 1.025).epsilon(0.03));
    CHECK(std::fabs(n.sbandamento_gradi()) < 0.1);
    CHECK_FALSE(n.affondata());
}

TEST_CASE("danni: cassa laterale allagata, sbandamento sulla dritta") {
    DanniNave n(chiatta(true), 1);
    n.aggiungi_falla({5, 5.0, -5.0, -4.0, 0.5});   // la cassa di dritta (indice 5)
    n.avanza(600);
    CHECK(n.sbandamento_gradi() > 1.0);            // positivo: dritta in basso
    CHECK(n.sbandamento_gradi() < 15.0);
}

// Casi del dossier docs/ricerca/07_danni_affondamento_esplosioni.md (§7.3, §11.4) sui dati esportati
namespace {
const std::string SCAFI = std::string(M688_DATI) + "/scafi/";
bool ci_sono_scafi() { return std::filesystem::exists(SCAFI + "perry.toml"); }
Esplosione siluro(const DanniNave& n, double fx, double y, double z_rel, bool contatto, double kg) {
    const auto& st = n.corpi().front().stazioni;
    const double x0 = st.front().x, x1 = st.back().x, chiglia = st[st.size() / 2].z_chiglia;
    Esplosione e;
    e.x = x0 + (x1 - x0) * fx; e.y = y; e.z = contatto ? chiglia * z_rel : chiglia - z_rel; e.contatto = contatto; e.tnt_kg = kg;
    return e;
}
}  // namespace

TEST_CASE("danni: navi del dossier" * doctest::skip(!ci_sono_scafi())) {
    SUBCASE("Perry intatta in equilibrio") {
        DanniNave n(DatiScafo::carica(SCAFI + "perry.toml"), 1);
        n.avanza(300);   // assestamento di qualche millimetro: i pesi discreti non centrano esattamente la carena
        CHECK(std::fabs(n.immersione_m()) < 5e-3);
        CHECK(std::fabs(n.sbandamento_gradi()) < 1e-3);
        CHECK(std::fabs(n.appoppamento_gradi()) < 0.05);
    }
    SUBCASE("Mk 48 sotto la chiglia di una fregata: spezzata, tronconi a fondo in minuti") {
        DanniNave n(DatiScafo::carica(SCAFI + "perry.toml"), 2);
        n.colpo(siluro(n, 0.5, 0, 6.0, false, 544));
        CHECK(n.spezzata());
        // la prua va a fondo in 7 minuti; la poppa, col deposito dei Mk 46 (permeabilita' 0,6), resta a candela
        // sull'aria dei locali e va giu' dopo circa 33 minuti
        n.avanza(2400);
        CHECK(n.affondata());
    }
    SUBCASE("siluro a contatto su una corvetta: affonda entro un'ora") {
        DanniNave n(DatiScafo::carica(SCAFI + "koni.toml"), 1);
        n.colpo(siluro(n, 0.42, -1, 0.7, true, 300));
        n.avanza(3600);
        CHECK(n.affondata());
    }
    SUBCASE("portaerei: un siluro non basta") {
        DanniNave n(DatiScafo::carica(SCAFI + "nimitz.toml"), 1);
        n.colpo(siluro(n, 0.42, -1, 0.7, true, 300));
        n.avanza(3600);
        CHECK_FALSE(n.affondata());
        CHECK_FALSE(n.spezzata());
        CHECK(std::fabs(n.sbandamento_gradi()) < 10.0);
    }
    SUBCASE("petroliera carica: due siluri, resta a galla") {
        DanniNave n(DatiScafo::carica(SCAFI + "petroliera.toml"), 1);
        n.colpo(siluro(n, 0.65, -1, 0.7, true, 300));
        n.colpo(siluro(n, 0.35, -1, 0.7, true, 300));
        n.avanza(7200);
        CHECK_FALSE(n.affondata());
    }
    SUBCASE("missile su una fregata: incendio, la nave si salva") {
        DanniNave n(DatiScafo::carica(SCAFI + "perry.toml"), 1);
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile; e.x = 7; e.y = -1; e.z = 2.5; e.tnt_kg = 165;
        n.colpo(e);
        n.avanza(1800);
        double fuoco = 0;
        for (const auto& c : n.corpi()) for (const auto& ce : c.celle) fuoco = std::max(fuoco, ce.fuoco);
        CHECK(fuoco > 0.5);
        n.avanza(5400);
        CHECK_FALSE(n.affondata());
    }
    SUBCASE("deterministica con il seme") {
        auto gioca = [] {
            DanniNave n(DatiScafo::carica(SCAFI + "krivak2.toml"), 7);
            n.colpo(siluro(n, 0.42, -1, 0.7, true, 300));
            n.avanza(1200);
            std::string s;
            for (const auto& r : n.registro()) s += r + "\n";
            return s + std::to_string(n.immersione_m());
        };
        CHECK(gioca() == gioca());
    }
}

// Sottomarini (tools/navi/compartimenti.py, base della fase D6): dati dello scafo resistente e fisica in emersione
TEST_CASE("danni: sottomarini in emersione" * doctest::skip(!std::filesystem::exists(SCAFI + "los_angeles.toml"))) {
    const char* classi[] = {"los_angeles", "victor3", "kilo", "alfa", "akula", "oscar", "charlie2", "typhoon", "foxtrot",
                            "sturgeon", "trafalgar"};
    SUBCASE("scafo resistente dentro lo scafo esterno, quote e volumi coerenti") {
        for (const char* c : classi) {
            INFO(c);
            const auto d = DatiScafo::carica(SCAFI + c + ".toml");
            const auto& s = d.sottomarino;
            REQUIRE(s.presente);
            CHECK(d.tipo == "sottomarino");
            CHECK(s.quota_collasso_m > s.quota_collaudo_m);
            CHECK(s.dislocamento_immersione_t > s.dislocamento_emersione_t);
            double v = 0;
            for (const auto& st : d.stazioni) {
                if (st.r_resistente <= 0) continue;
                v += PI * st.r_resistente * st.r_resistente * d.passo * (st.y_resistente > 0 ? 2 : 1);
                double hmax = 0;
                for (double h : st.semilarghezze) hmax = std::max(hmax, h);
                CHECK(st.y_resistente + st.r_resistente <= hmax + 1e-3);
                CHECK(st.z_resistente - st.r_resistente >= st.z_chiglia - 1e-3);
                CHECK(st.z_resistente + st.r_resistente <= st.coperta() + 1e-3);
            }
            CHECK(v == doctest::Approx(s.volume_resistente_m3).epsilon(0.01));
            CHECK(v < s.volume_esterno_m3);
            // lo scafo resistente c'e' a meta' lunghezza e non alle estremita' (casse zavorra)
            CHECK(d.stazioni[d.stazioni.size() / 2].r_resistente > 0);
            CHECK(d.stazioni.front().r_resistente == 0);
            CHECK(d.stazioni.back().r_resistente == 0);
        }
    }
    SUBCASE("intatti in equilibrio (portelli chiusi: niente aperture di coperta sott'acqua)") {
        for (const char* c : classi) {
            INFO(c);
            DanniNave n(DatiScafo::carica(SCAFI + c + ".toml"), 1);
            n.avanza(300);
            CHECK(std::fabs(n.immersione_m()) < 0.05);
            CHECK(std::fabs(n.appoppamento_gradi()) < 0.3);
            CHECK(n.acqua_imbarcata_t() < 1.0);
        }
    }
    SUBCASE("riserva di spinta tarata su quella pubblicata (immersione / emersione - 1)") {
        for (const char* c : classi) {
            INFO(c);
            if (std::string(c) == "trafalgar") continue;   // scafo singolo: sopra il mare c'e' quasi solo scafo resistente
            const auto d = DatiScafo::carica(SCAFI + c + ".toml");
            double v0 = 0, vt = 0, zavorra = 0;
            for (const auto& st : d.stazioni) {
                const auto p = st.sezione();
                vt += area_poligono(p) * d.passo;
                v0 += area_poligono(taglia(p, 0, 1, 0)) * d.passo;
            }
            for (const auto& ce : d.celle) zavorra += ce.acqua_iniziale;
            CHECK(vt / (v0 - zavorra) - 1 == doctest::Approx(d.sottomarino.riserva_spinta).epsilon(0.05).scale(1.0));
        }
    }
    SUBCASE("Akula colpito da un siluro leggero: esaurisce le casse e resta a galla") {
        DanniNave n(DatiScafo::carica(SCAFI + "akula.toml"), 3);
        n.avanza(60);
        CHECK(n.acqua_imbarcata_t() < 1.0);   // la zavorra iniziale non e' acqua imbarcata
        n.colpo(siluro(n, 0.42, -1, 0.7, true, 44));
        n.avanza(1800);
        CHECK_FALSE(n.affondata());
        bool soffia = false;
        for (const auto& r : n.registro()) soffia |= r.find("esaurimento d'emergenza") != std::string::npos;
        CHECK(soffia);
    }
    SUBCASE("Mk 48 sotto la chiglia: il 688 in emersione si spezza e affonda") {
        DanniNave n(DatiScafo::carica(SCAFI + "los_angeles.toml"), 1);
        n.colpo(siluro(n, 0.5, 0, 5.0, false, 544));
        CHECK(n.spezzata());
        n.avanza(600);
        CHECK(n.affondata());
    }
    SUBCASE("siluro leggero a contatto: affonda lo scafo singolo, il doppio scafo resta a galla (dossier 02 §9.3)") {
        DanniNave singolo(DatiScafo::carica(SCAFI + "los_angeles.toml"), 3);
        singolo.colpo(siluro(singolo, 0.42, -1, 0.7, true, 44));
        singolo.avanza(1800);
        CHECK(singolo.affondata());
        DanniNave doppio(DatiScafo::carica(SCAFI + "kilo.toml"), 3);
        doppio.colpo(siluro(doppio, 0.42, -1, 0.7, true, 44));
        doppio.avanza(1800);
        CHECK_FALSE(doppio.affondata());
        CHECK(doppio.acqua_imbarcata_t() > 100.0);   // la cassa laterale e il compartimento imbarcano acqua
    }
}

TEST_CASE("danni: proietti d'artiglieria sopra il galleggiamento (G3, fra navi)" * doctest::skip(!ci_sono_scafi())) {
    // un 5" (3,5 kg) sulla murata della Perry: falla sopra l'acqua (~0,3 m^2), la nave galleggia; meno incendi di un
    // missile della stessa carica (12% contro 80% per le navi piccole: STIMA)
    int fuochi_proietto = 0, fuochi_missile = 0;
    for (std::uint64_t seme = 1; seme <= 60; ++seme) {
        for (const auto tipo : {Esplosione::Tipo::proietto, Esplosione::Tipo::missile}) {
            DanniNave n(DatiScafo::carica(SCAFI + "perry.toml"), seme);
            Esplosione e;
            e.tipo = tipo;
            e.x = 10.0;
            e.y = 5.0;
            e.z = 3.0;
            e.tnt_kg = 3.5;
            n.colpo(e);
            const double attesa = std::pow((tipo == Esplosione::Tipo::proietto ? 0.35 : 0.7) * std::cbrt(3.5), 2);
            bool falla = false;   // la falla del colpo sul fianco (la Perry ne ha gia' altre in elenco)
            for (const auto& f : n.corpi().front().falle) falla |= std::fabs(f.area - attesa) < 1e-9 && f.z > 0.0;
            CHECK(falla);
            bool fuoco = false;
            for (const auto& c : n.corpi().front().celle) fuoco |= c.fuoco > 0.0;
            (tipo == Esplosione::Tipo::proietto ? fuochi_proietto : fuochi_missile) += fuoco;
            if (tipo == Esplosione::Tipo::proietto) {
                for (int k = 0; k < 120; ++k) n.avanza(1.0);
                CHECK_FALSE(n.affondata());
                CHECK(n.scoppi().back().tipo == Esplosione::Tipo::proietto);
            }
        }
    }
    CHECK(fuochi_proietto < fuochi_missile);
    CHECK(fuochi_proietto <= 20);
    // un Kara (8.000 t) sotto il tiro di una Perry: 40 colpi da 76 mm in due minuti sulla murata sopra l'acqua non lo
    // affondano (dossier 22: per affondare una nave servono di solito molti colpi)
    DanniNave kara(DatiScafo::carica(SCAFI + "kara.toml"), 3);
    Casuale caso(9);
    for (int k = 0; k < 40; ++k) {
        Esplosione e;
        e.tipo = Esplosione::Tipo::proietto;
        e.x = caso.uniforme(-70.0, 70.0);
        e.y = caso.uniforme() < 0.5 ? 9.0 : -9.0;
        e.z = caso.uniforme(1.0, 5.0);
        e.tnt_kg = 0.75;
        kara.colpo(e);
        kara.avanza(3.0);
    }
    for (int k = 0; k < 1200; ++k) kara.avanza(1.0);
    CHECK_FALSE(kara.affondata());
    CHECK_FALSE(kara.condannata());
}

TEST_CASE("danni: depositi con contenuto e protezione, carburante, lanciatori in coperta (danni critici, fase 2)") {
    // il locale centrale e' un deposito con 2 t di esplosivo e 1 t di propellente: lo scoppio vale 2 + 0,3 t
    const auto con_deposito = [](double protezione) {
        DatiScafo d = chiatta();
        for (auto& c : d.celle)
            if (c.nome == "L5") { c.uso = "deposito"; c.deposito = "missili"; c.carica_kg = 2000; c.propellente_kg = 1000; c.protezione = protezione; }
        return d;
    };
    const auto siluro_nel_deposito = [](DanniNave& n) {
        Esplosione e;
        e.x = 5.0; e.y = 5.0; e.z = -3.0; e.tnt_kg = 100.0;
        n.colpo(e);
        for (const auto& s : n.scoppi())
            if (s.tipo == Esplosione::Tipo::deposito) return s.tnt_kg;
        return 0.0;
    };
    int scoppi = 0;
    for (std::uint64_t seme = 1; seme <= 120; ++seme) {
        DanniNave n(con_deposito(0.0), seme);
        if (const double W = siluro_nel_deposito(n); W > 0) {
            CHECK(W == Approx(2300.0));
            ++scoppi;
        }
        DanniNave p(con_deposito(1.0), seme);   // corazzato del tutto: non salta mai
        CHECK(siluro_nel_deposito(p) == 0.0);
    }
    CHECK(scoppi >= 3);   // il 10% dei colpi sott'acqua nel deposito

    // il gasolio nel doppio fondo: 100 t sono 2.000 s di fuoco pieno in piu' del locale
    DatiScafo d = chiatta();
    d.celle[2].carburante = "gasolio"; d.celle[2].carburante_t = 100.0;
    d.celle[3].carburante = "greggio_carico"; d.celle[3].carburante_t = 5000.0;
    DanniNave n(d, 1);
    const auto& celle = n.corpi().front().celle;
    CHECK(celle[2].combustibile == Approx(celle[1].combustibile + 2000.0));
    CHECK(celle[3].combustibile == Approx(40000.0));   // il carico brucia come quello di una petroliera

    // i lanciatori in coperta: 1,5 t di missili che saltano sopra la coperta, una volta sola
    DatiScafo z = chiatta();
    z.sopra_coperta.push_back({"lanciatore", "missili", 0.0, 10.0, 1500.0});
    int saltati = 0;
    for (std::uint64_t seme = 1; seme <= 200; ++seme) {
        DanniNave m(z, seme);
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile;
        e.x = 5.0; e.y = 5.0; e.z = 3.0; e.tnt_kg = 100.0;
        m.colpo(e);
        for (int k = 0; k < 60; ++k) m.avanza(1.0);
        int qui = 0;
        for (const auto& s : m.scoppi())
            if (s.tipo == Esplosione::Tipo::deposito) {
                CHECK(s.tnt_kg == Approx(1500.0));
                CHECK(s.z > 5.0);
                ++qui;
            }
        CHECK(qui <= 1);
        saltati += qui;
        CHECK(m.zone().front().dati.carica_kg == (qui ? 0.0 : 1500.0));
    }
    CHECK(saltati >= 2);   // il 4% dei missili, piu' il cook-off nel primo minuto
}

TEST_CASE("danni: il fuoco dell'hangar scende sotto coperta e brucia l'elicottero (danni critici, fase 3)") {
    // l'hangar copre L5, dove scoppia il missile, e L6: si conta il fuoco che scende in L6 (in L5 lo appicca il colpo)
    DatiScafo z = chiatta();
    z.sopra_coperta.push_back({"hangar", "avio", 0.0, 20.0, 0.0});
    int scesi = 0;
    for (std::uint64_t seme = 1; seme <= 20; ++seme) {
        DanniNave m(z, seme);
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile;
        e.x = 5.0; e.y = 5.0; e.z = 3.0; e.tnt_kg = 100.0;
        m.colpo(e);
        for (int k = 0; k < 900; ++k) m.avanza(1.0);
        CHECK(m.zone().front().elicottero_perso);
        for (const auto& r : m.registro()) scesi += r.find("scende in L6") != std::string::npos;
    }
    CHECK(scesi >= 5);
}

TEST_CASE("danni: il fuoco passa secondo quello che c'e' nel locale vicino (danni critici, T5)") {
    // L5 brucia: quante volte, in 3 minuti su 60 semi, il fuoco passa a L6 secondo il suo contenuto
    const auto passaggi = [](const std::function<void(CellaScafo&)>& riempi) {
        DatiScafo d = chiatta();
        for (auto& c : d.celle)
            if (c.nome == "L6") riempi(c);
        int n = 0;
        for (std::uint64_t seme = 1; seme <= 60; ++seme) {
            DanniNave m(d, seme);
            m.imposta_fuoco(5, 1.0);
            for (int k = 0; k < 180; ++k) m.avanza(1.0);
            for (const auto& r : m.registro()) n += r.find("si estende a L6") != std::string::npos;
        }
        return n;
    };
    const int comune = passaggi([](CellaScafo&) {});
    const int garage = passaggi([](CellaScafo& c) { c.carburante = "gasolio+veicoli"; c.carburante_t = 5.0; });
    const int avio = passaggi([](CellaScafo& c) { c.carburante = "jp5"; c.carburante_t = 5.0; });
    const int deposito = passaggi([](CellaScafo& c) { c.uso = "deposito"; c.deposito = "missili"; c.carica_kg = 100.0; });
    const int corazzato = passaggi([](CellaScafo& c) { c.uso = "deposito"; c.deposito = "proietti"; c.carica_kg = 100.0; c.protezione = 0.85; });
    const int cassa = passaggi([](CellaScafo& c) { c.uso = "ala"; });
    MESSAGE("passaggi su 60: comune ", comune, ", garage ", garage, ", JP-5 ", avio, ", deposito ", deposito, ", corazzato ", corazzato, ", cassa ", cassa);
    CHECK(garage > avio);
    CHECK(avio > deposito);
    CHECK(deposito > comune);
    CHECK(comune > corazzato);
    CHECK(comune > cassa);
}

TEST_CASE("danni: il deposito salta secondo il contenuto (danni critici, T2)") {
    // un missile nel deposito L5, sopra il galleggiamento, su 2000 semi: il 5% dei missili, il 2% delle bombe
    const auto scoppi = [](const std::string& contenuto, double protezione) {
        DatiScafo d = chiatta();
        for (auto& c : d.celle)
            if (c.nome == "L5") { c.uso = "deposito"; c.deposito = contenuto; c.carica_kg = 500.0; c.protezione = protezione; }
        int n = 0;
        for (std::uint64_t seme = 1; seme <= 2000; ++seme) {
            DanniNave m(d, seme);
            Esplosione e;
            e.tipo = Esplosione::Tipo::missile;
            e.x = 5.0; e.y = 0.0; e.z = 3.0; e.tnt_kg = 100.0;
            m.colpo(e);
            for (const auto& s : m.scoppi()) n += s.tipo == Esplosione::Tipo::deposito;
        }
        return n;
    };
    const int missili = scoppi("missili", 0.0), misto = scoppi("siluri+missili", 0.0), proietti = scoppi("proietti", 0.0),
              siluri = scoppi("siluri", 0.0), bombe = scoppi("bombe", 0.0), corazzato = scoppi("missili", 0.8);
    MESSAGE("scoppi su 2000: missili ", missili, ", misto ", misto, ", proietti ", proietti, ", siluri ", siluri, ", bombe ", bombe, ", corazzato ", corazzato);
    CHECK(missili == Approx(100).epsilon(0.35));
    CHECK(misto == missili);
    CHECK(proietti == Approx(80).epsilon(0.35));
    CHECK(siluri == Approx(60).epsilon(0.4));
    CHECK(bombe == Approx(40).epsilon(0.5));
    CHECK(corazzato == Approx(20).epsilon(0.6));
    CHECK(missili > bombe);
}

TEST_CASE("danni: il fuoco sale dal locale alla zona sopra coperta, l'hangar prima dei lanciatori (danni critici, T5)") {
    const auto saliti = [](const std::string& carico) {
        DatiScafo z = chiatta();
        z.sopra_coperta.push_back({"zona", carico, 0.0, 10.0, 0.0});   // sopra L5
        int n = 0;
        for (std::uint64_t seme = 1; seme <= 60; ++seme) {
            DanniNave m(z, seme);
            m.imposta_fuoco(5, 1.0);
            for (int k = 0; k < 180; ++k) m.avanza(1.0);
            CHECK((m.zone().front().fuoco > 0) == [&] {
                for (const auto& r : m.registro())
                    if (r.find("sale a zona") != std::string::npos) return true;
                return false;
            }());
            n += m.zone().front().fuoco > 0;
        }
        return n;
    };
    const int hangar = saliti("avio"), lanciatori = saliti("missili");
    MESSAGE("saliti su 60: hangar ", hangar, ", lanciatori ", lanciatori);
    CHECK(lanciatori > 0);
    CHECK(hangar > lanciatori);
}

TEST_CASE("danni: lo scoppio del deposito contro la stazza (danni critici, fase 4)") {
    // la chiatta di 5000 t: 20 t (4 kg/t) la fanno saltare in aria; 1 t (0,2 kg/t) la spezza di rado
    const auto con_deposito = [](double carica) {
        DatiScafo d = chiatta();
        for (auto& c : d.celle)
            if (c.nome == "L5") { c.uso = "deposito"; c.deposito = "missili"; c.carica_kg = carica; }
        return d;
    };
    const auto scoppia = [](DanniNave& n) {
        Esplosione e;
        e.x = 5.0; e.y = 5.0; e.z = -3.0; e.tnt_kg = 100.0;
        n.colpo(e);
        for (const auto& s : n.scoppi())
            if (s.tipo == Esplosione::Tipo::deposito) return true;
        return false;
    };
    int saltate = 0, spezzate = 0, intere = 0;
    for (std::uint64_t seme = 1; seme <= 200; ++seme) {
        DanniNave grande(con_deposito(20000.0), seme);
        if (scoppia(grande)) {
            ++saltate;
            CHECK(grande.affondata());
            CHECK_FALSE(grande.spezzata());
        }
        DanniNave piccola(con_deposito(1000.0), seme);
        if (scoppia(piccola)) (piccola.spezzata() ? spezzate : intere) += 1;
    }
    CHECK(saltate >= 5);
    CHECK(intere > spezzate);   // p = (0,2 - 0,1) / 0,6 = 17%
}

TEST_CASE("danni: le cisterne in zavorra, i vapori e il gas inerte (danni critici, T7)") {
    const auto nave = [] {
        DatiScafo d = chiatta();
        d.tipo = "petroliera";
        for (auto& c : d.celle)
            if (c.nome == "L5") { c.uso = "carico"; c.carburante = "greggio_carico"; c.carburante_t = 5000.0; }
        return d;
    }();
    const auto colpo = [](Esplosione::Tipo tipo, double kg) {
        Esplosione e;
        e.tipo = tipo;
        e.x = 5.0; e.y = 0.0; e.z = 3.0; e.tnt_kg = kg;
        return e;
    };
    struct Conto { int scoppi = 0, spezzate = 0; };
    const auto conta = [&](const DatiScafo& d, int stato, const Esplosione& e) {   // stato: 0 cariche, 1 zavorra, 2 col gas inerte
        Conto c;
        for (std::uint64_t seme = 1; seme <= 2000; ++seme) {
            DanniNave m(d, seme);
            if (stato) m.imposta_zavorra(stato == 2);
            m.colpo(e);
            for (const auto& s : m.scoppi()) c.scoppi += s.tipo == Esplosione::Tipo::deposito;
            c.spezzate += m.spezzata();
        }
        return c;
    };
    const Esplosione missile = colpo(Esplosione::Tipo::missile, 100.0);
    const Conto cariche = conta(nave, 0, missile), zavorra = conta(nave, 1, missile), inerte = conta(nave, 2, missile);
    MESSAGE("scoppi su 2000: cariche ", cariche.scoppi, ", zavorra ", zavorra.scoppi, " (spezzate ", zavorra.spezzate, "), inerte ", inerte.scoppi);
    CHECK(cariche.scoppi == 0);
    CHECK(cariche.spezzate == 0);
    CHECK(zavorra.scoppi == Approx(600).epsilon(0.15));   // il 30%
    CHECK(inerte.scoppi == Approx(100).epsilon(0.35));    // il 5%
    CHECK(zavorra.spezzate == Approx(zavorra.scoppi / 3.0).epsilon(0.25));
    CHECK(conta(nave, 1, colpo(Esplosione::Tipo::proietto, 2.5)).scoppi == Approx(60).epsilon(0.5));   // il 10% del 30%

    SUBCASE("il danno e' tre volte la testata") {
        for (std::uint64_t seme = 1; seme <= 50; ++seme) {
            DanniNave m(nave, seme);
            m.imposta_zavorra(false);
            m.colpo(missile);
            for (const auto& s : m.scoppi())
                if (s.tipo == Esplosione::Tipo::deposito) CHECK(s.tnt_kg == Approx(300.0));
        }
    }
    SUBCASE("il colpo sott'acqua: una sola estrazione, nella cisterna colpita") {
        Esplosione e;
        e.tipo = Esplosione::Tipo::siluro; e.x = 5.0; e.y = 5.0; e.z = -2.0; e.tnt_kg = 300.0; e.contatto = true;
        CHECK(conta(nave, 0, e).scoppi == 0);
        CHECK(conta(nave, 1, e).scoppi == Approx(600).epsilon(0.15));
    }
    SUBCASE("le cisterne vuote non hanno greggio") {
        DanniNave m(nave, 1);
        m.imposta_zavorra(false);
        CHECK(m.in_zavorra());
        for (const auto& ce : m.corpi().front().celle)
            if (ce.dati.nome == "L5") {
                CHECK(ce.dati.carburante.empty());
                CHECK(ce.combustibile == Approx(600.0));
            }
    }
    SUBCASE("un'altra nave non cambia") {
        DatiScafo d = nave;
        d.tipo = "mercantile";
        DanniNave m(d, 1);
        m.imposta_zavorra(false);
        CHECK_FALSE(m.in_zavorra());
        CHECK(conta(d, 1, missile).scoppi == 0);
    }
    SUBCASE("lo scafo vero della petroliera regge i tronconi") {
        const DatiScafo d = DatiScafo::carica(SCAFI + "petroliera.toml");
        int scoppi = 0;
        for (std::uint64_t seme = 1; seme <= 40; ++seme) {
            DanniNave m(d, seme);
            m.imposta_zavorra(false);
            m.colpo(siluro(m, 0.5, -1, 0.7, true, 300));
            m.avanza(600);
            for (const auto& s : m.scoppi()) scoppi += s.tipo == Esplosione::Tipo::deposito;
        }
        MESSAGE("scoppi su 40 col Mk 48: ", scoppi);
        CHECK(scoppi >= 4);   // attesi 12: il siluro sul fianco trova le cisterne laterali
    }
}

TEST_CASE("danni: le pompe lasciano stare il deposito allagato, anche con una qualita' che non divide i 120 s (T15)") {
    auto dati = DatiScafo::carica(SCAFI + "perry.toml");
    dati.qualita_controllo_danni = 0.7;   // 120 / 0,7: il conto alla rovescia passa sotto zero senza fermarsi sullo zero
    DanniNave n(dati, 1);
    const auto& celle = n.corpi().front().celle;
    std::size_t j = 0;
    while (j < celle.size() && celle[j].dati.uso != "deposito") ++j;
    REQUIRE(j < celle.size());
    n.imposta_fuoco(j, 1.0);
    n.avanza(600);
    REQUIRE(n.scoppi().empty());
    CHECK(celle[j].fuoco == 0.0);
    CHECK(celle[j].allagata_per_incendio == 0.0);
    const double allagato = celle[j].acqua;
    REQUIRE(celle[j].riempimento() > 0.7);
    n.avanza(1800);
    // resta la perdita delle paratie (55 m^3 in 30 minuti); con le pompe se ne andavano 171
    CHECK(allagato - celle[j].acqua < 80.0);
}
