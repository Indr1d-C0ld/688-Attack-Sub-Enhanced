#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string>

#include "nucleo/battello.hpp"

using namespace m688;

// Sottomarino danneggiato in immersione (docs/10_danni_affondamento_esplosioni.md §10, fase D6a), sui dati esportati
namespace {
const std::string SCAFI_SUB = std::string(M688_DATI) + "/scafi/";
bool ci_sono_sottomarini() { return std::filesystem::exists(SCAFI_SUB + "los_angeles.toml"); }
constexpr double FORO_5 = 3.14159265 * 0.0635 * 0.0635;   // m^2: foro da 5 pollici
double pressione(double quota) { return 101325.0 + 1025.0 * 9.81 * quota; }

// Moto verticale con l'adattatore cinematico: esaurimento dopo 30 s, fino alla superficie o alla perdita
struct Esito {
    bool emerso = false, perso = false;
    double quota = 0.0;
};
Esito risali(BattelloDanni& b, double quota, double velocita, int piani) {
    for (int t = 0; t < 3600; ++t) {
        if (t == 30) b.esaurisci();
        const auto f = b.avanza(1.0, {quota, velocita});
        quota += rateo_verticale_regime(f.peso_netto_N, b.area_pianta_m2(), velocita, piani);
        if (quota <= 5.0) return {true, false, quota};
        if (b.perso()) return {false, true, quota};
    }
    return {false, false, quota};
}
}  // namespace

TEST_CASE("battello: sottomarino danneggiato in immersione" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d688 = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");

    SUBCASE("compartimenti nello scafo resistente e casse zavorra dai dati") {
        for (const char* c : {"los_angeles", "victor3", "kilo", "alfa", "akula", "oscar", "charlie2", "typhoon", "foxtrot",
                              "sturgeon", "trafalgar"}) {
            INFO(c);
            const auto d = DatiScafo::carica(SCAFI_SUB + c + ".toml");
            BattelloDanni b(d, 1);
            double v = 0;
            for (const auto& k : b.compartimenti()) v += k.volume;
            CHECK(v > 0.3 * d.sottomarino.volume_resistente_m3);   // allagabile: volume per la permeabilita'
            CHECK(v < d.sottomarino.volume_resistente_m3);
            CHECK(!b.casse().empty());
            CHECK(!b.paratie().empty());
        }
        BattelloDanni b(d688, 1);
        CHECK(b.compartimenti().size() == 3);   // prodiero, reattore, macchine (dossier 02 §9.3)
        CHECK(b.casse().size() == 2);           // casse di prua e di poppa (scafo singolo)
        // Typhoon: i due scafi resistenti affiancati hanno compartimenti propri e uguali
        BattelloDanni t(DatiScafo::carica(SCAFI_SUB + "typhoon.toml"), 1);
        int coppie = 0;
        for (const auto& a : t.compartimenti())
            for (const auto& c : t.compartimenti())
                if (a.nome != c.nome && a.nome.back() == 'D' && c.nome == a.nome.substr(0, a.nome.size() - 1) + "S") {
                    ++coppie;
                    CHECK(a.volume == doctest::Approx(c.volume).epsilon(0.02));
                }
        CHECK(coppie >= 5);
    }
    SUBCASE("intatto: neutro, niente acqua ne' eventi") {
        BattelloDanni b(d688, 1);
        const auto f = b.avanza(600, {200.0, 5.0});
        CHECK(f.peso_netto_N == 0.0);
        CHECK(f.momento_assetto_Nm == 0.0);
        CHECK(b.eventi().empty());
    }
    SUBCASE("portate della tabella del dossier 02 §9.1 e allagamento fermato dall'aria compressa") {
        for (const auto& [quota, portata] : {std::pair{20.0, 0.16}, std::pair{300.0, 0.60}}) {
            INFO(quota);
            BattelloDanni b(d688, 1);
            b.aggiungi_falla(1, FORO_5);
            b.avanza(10, {quota, 0});
            CHECK(b.acqua_imbarcata_m3() / 10 == doctest::Approx(portata).epsilon(0.05));
        }
        BattelloDanni b(d688, 1);
        b.aggiungi_falla(0, FORO_5);
        b.avanza(4 * 3600, {50.0, 0});
        const auto& c = b.compartimenti()[0];
        CHECK(c.acqua / c.volume == doctest::Approx(1 - 101325.0 / pressione(50)).epsilon(0.01));
        // la paratia regge 0,3 volte la pressione di collaudo (circa 87 m): a 50 m resta in piedi
        for (const auto& p : b.paratie()) CHECK_FALSE(p.ceduta);
        CHECK(b.forze(50).peso_netto_N > 0);
        CHECK(b.forze(50).momento_assetto_Nm < 0);   // acqua nel compartimento prodiero: prua giu'
    }
    SUBCASE("a 300 m le paratie cedono e l'acqua passa nei compartimenti vicini") {
        BattelloDanni b(d688, 1);
        b.aggiungi_falla(1, FORO_5);   // reattore, fra i due compartimenti
        b.avanza(2 * 3600, {300.0, 0});
        int cedute = 0;
        for (const auto& p : b.paratie()) cedute += p.ceduta;
        CHECK(cedute >= 1);
        for (const auto& c : b.compartimenti()) CHECK(c.acqua > 0.5 * c.volume);
    }
    SUBCASE("esaurimento d'emergenza: a 100 m il 688 riemerge, a 400 m con la stessa falla no (Boyle)") {
        BattelloDanni a(d688, 1);
        a.aggiungi_falla(2, FORO_5);
        const auto da100 = risali(a, 100.0, 0.0, 0);
        CHECK(da100.emerso);
        BattelloDanni b(d688, 1);
        b.aggiungi_falla(2, FORO_5);
        const auto da400 = risali(b, 400.0, 0.0, 0);
        CHECK(da400.perso);
        // dottrina (dossier 02 §9.2): avanti tutta e barra in salita, la portanza dei piani e' piu' rapida della zavorra
        BattelloDanni c(d688, 1);
        c.aggiungi_falla(2, FORO_5);
        CHECK(risali(c, 400.0, 10.0, +1).emerso);
    }
    SUBCASE("collasso fra 1,1 e 1,8 volte la quota prevista (Rule), poi i compartimenti asciutti implodono uno dopo l'altro") {
        for (std::uint64_t seme = 1; seme <= 20; ++seme) {
            BattelloDanni b(d688, seme);
            CHECK(b.quota_collasso_m() >= 1.1 * d688.sottomarino.quota_collasso_m);
            CHECK(b.quota_collasso_m() <= 1.8 * d688.sottomarino.quota_collasso_m);
        }
        BattelloDanni b(d688, 3);
        const double qc = b.quota_collasso_m();
        b.avanza(10, {qc - 5, 0});
        CHECK_FALSE(b.perso());
        b.avanza(300, {qc + 20, 0});
        CHECK(b.perso());
        int implosioni = 0;
        double tempo = -1;
        for (const auto& e : b.eventi()) {
            if (e.tipo != EventoBattello::Tipo::implosione) continue;
            ++implosioni;
            CHECK(e.tnt_kg > 100.0);       // p V: centinaia di kg o tonnellate di TNT (dossier 07 §8.2)
            CHECK(e.tempo > tempo);        // una dopo l'altra
            tempo = e.tempo;
        }
        CHECK(implosioni == static_cast<int>(b.compartimenti().size()));
    }
    SUBCASE("casse d'assetto: 2 t imbarcate in meno di un minuto, scaricate con le pompe (lente in profondita')") {
        BattelloDanni b(d688, 1);
        b.assetto(2.0);
        CHECK(b.avanza(60, {50.0, 0.0}).peso_netto_N == doctest::Approx(1025.0 * 9.81 * 2.0));
        CHECK(b.acqua_imbarcata_m3() == 0.0);   // non e' acqua dalle falle
        b.assetto(0.0);
        b.avanza(30, {50.0, 0.0});
        CHECK(b.acqua_assetto_m3() < 2.0);
        b.avanza(120, {50.0, 0.0});
        CHECK(b.acqua_assetto_m3() == 0.0);
        BattelloDanni p(d688, 1);
        p.assetto(2.0);
        p.avanza(60, {300.0, 0.0});
        p.assetto(0.0);
        p.avanza(150, {300.0, 0.0});
        CHECK(p.acqua_assetto_m3() > 1.0);   // a 300 m le pompe quasi non rendono
    }
    SUBCASE("stesso seme, stessa storia") {
        auto storia = [&](std::uint64_t seme) {
            BattelloDanni b(d688, seme);
            b.aggiungi_falla(0, 2 * FORO_5);
            std::string s;
            risali(b, 350.0, 3.0, 0);
            for (const auto& e : b.eventi()) s += std::to_string(e.tempo) + e.testo;
            return s;
        };
        CHECK(storia(7) == storia(7));
    }
}

// Urto e guasti graduati dei sistemi (fase D6b): soglie del fattore d'urto del dossier 02 §9.4
TEST_CASE("battello: urto, guasti dei sistemi e riparazioni" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d688 = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    constexpr double SOTTO = 1.5707963;   // incidenza dal basso
    auto ha = [](const BattelloDanni& b, EventoBattello::Tipo t) {
        for (const auto& e : b.eventi()) if (e.tipo == t) return true;
        return false;
    };

    SUBCASE("fattore d'urto: Mk 48 (650 lb) a 33 ft da sotto, HSF 0,77; soglia ridotta in profondita'") {
        BattelloDanni a(d688, 1), b(d688, 1);
        CHECK(a.urto(294.8, 10.06, SOTTO, 0, 0) == doctest::Approx(0.77).epsilon(0.02));
        const double meta = 0.5 * d688.sottomarino.quota_collaudo_m;   // a meta' della quota di collaudo: soglia x 0,75
        CHECK(b.urto(294.8, 10.06, SOTTO, 0, meta) == doctest::Approx(0.77 / 0.75).epsilon(0.02));
        BattelloDanni c(d688, 1);
        CHECK(c.urto(294.8, 10.06, 0.0, 0, 0) == doctest::Approx(0.77 / 2).epsilon(0.02));   // di fianco: (1 + sin 0) / 2
    }
    SUBCASE("Mk 48 a 10 m sotto lo scafo a 100 m: letale o quasi, anche con la dottrina") {
        int persi = 0;
        for (std::uint64_t seme = 1; seme <= 50; ++seme) {
            BattelloDanni b(d688, seme);
            b.urto(294.8, 10.0, SOTTO, 0, 100);
            CHECK_FALSE(b.falle().empty());
            persi += risali(b, 100.0, 10.0 * b.limiti().velocita, b.limiti().idroplani ? +1 : 0).perso;
        }
        CHECK(persi >= 40);
    }
    SUBCASE("carica da 135 kg a 30 m sul fianco: guasti senza falle a 50 m, perdite a 250 m") {
        int guasti = 0;
        for (std::uint64_t seme = 1; seme <= 50; ++seme) {
            BattelloDanni b(d688, seme);
            b.urto(135, 30, 0.0, 0, 50);
            CHECK(b.falle().empty());
            CHECK(ha(b, EventoBattello::Tipo::scossa));
            for (const auto& e : b.eventi()) guasti += e.tipo == EventoBattello::Tipo::guasto;
            BattelloDanni p(d688, seme);
            p.urto(135, 30, 0.0, 0, 250);
            CHECK_FALSE(p.falle().empty());
            for (const auto& f : p.falle()) CHECK(f.area < 0.003);   // perdite da 1-2"
        }
        CHECK(guasti > 0);
    }
    SUBCASE("riparazioni: i sistemi tornano al massimo, i danni gravi lasciano almeno il 50%") {
        BattelloDanni b(d688, 5);
        b.urto(135, 15, SOTTO, 0, 100);
        double prima = 0;
        for (const auto& s : b.sistemi()) prima += s.efficienza;
        b.avanza(2 * 3600, {60.0, 3.0});
        double dopo = 0;
        for (const auto& s : b.sistemi()) {
            CHECK(s.efficienza == doctest::Approx(s.massimo));
            CHECK(s.massimo >= 0.5);
            dopo += s.efficienza;
        }
        CHECK(dopo > prima + 1.0);
        CHECK(ha(b, EventoBattello::Tipo::riparazione));
    }
    SUBCASE("SCRAM: motore d'emergenza per 5-20 minuti; i diesel non ne hanno") {
        int scram = 0, ripartiti = 0;
        for (std::uint64_t seme = 1; seme <= 30; ++seme) {
            BattelloDanni b(d688, seme);
            b.urto(135, 15, SOTTO, 25, 100);   // sotto il locale siluri: a x = 0 le falle allagano il reattore (fase 3)
            if (!b.limiti().scram) continue;
            ++scram;
            CHECK(b.limiti().velocita <= 0.15);
            b.avanza(299, {100.0, 2.0});
            CHECK(b.limiti().scram);
            b.avanza(1000, {100.0, 2.0});
            // con l'acqua oltre il 10% o il fuoco oltre 0,5 nel compartimento del reattore resta fermo (danni critici, fase 3)
            const auto& r = b.compartimenti()[static_cast<std::size_t>(b.compartimento_di(Sistema::propulsione))];
            if (r.acqua > 0.1 * r.volume || r.fuoco > 0.5) CHECK(b.limiti().scram);
            else {
                CHECK_FALSE(b.limiti().scram);
                ++ripartiti;
            }
        }
        CHECK(scram > 5);
        CHECK(ripartiti >= 3);
        const auto kilo = DatiScafo::carica(SCAFI_SUB + "kilo.toml");
        for (std::uint64_t seme = 1; seme <= 30; ++seme) {
            BattelloDanni b(kilo, seme);
            b.urto(135, 15, SOTTO, 0, 100);
            CHECK_FALSE(ha(b, EventoBattello::Tipo::scram));
        }
    }
    SUBCASE("limiti dai sistemi e priorita' del giocatore") {
        BattelloDanni b(d688, 5);
        b.urto(135, 15, SOTTO, 0, 100);
        const auto l = b.limiti();
        CHECK(l.idroplani == (b.sistema(Sistema::idroplani).efficienza >= 0.3));
        CHECK(l.timone == (b.sistema(Sistema::timone).efficienza >= 0.3));
        CHECK(l.periscopio == (b.sistema(Sistema::periscopio).efficienza >= 0.5));
        // il periscopio in cima alle riparazioni: dopo 5 minuti e' piu' avanti che con l'ordine normale
        BattelloDanni normale(d688, 5), prima(d688, 5);
        normale.urto(135, 15, SOTTO, 0, 100);
        prima.urto(135, 15, SOTTO, 0, 100);
        prima.priorita(Sistema::periscopio);
        normale.avanza(300, {60.0, 3.0});
        prima.avanza(300, {60.0, 3.0});
        CHECK(prima.sistema(Sistema::periscopio).efficienza > normale.sistema(Sistema::periscopio).efficienza);
    }
}

// Verifica storica (docs/10 §10.11; dossier 07 §8, dossier 02 §9): Thresher, San Francisco, Kursk, cariche della WWII
TEST_CASE("battello: casi storici" * doctest::skip(!ci_sono_sottomarini())) {
    auto foro = [](double pollici) { return 3.14159265 * std::pow(pollici * 0.0254 / 2, 2); };
    // moto verticale con l'adattatore cinematico, esaurimento all'istante dato, fondo facoltativo
    auto moto = [](BattelloDanni& b, double quota, double velocita, int piani, double t_esaurimento, double fondo = 1e9) {
        Esito e;
        for (int t = 0; t < 3600; ++t) {
            if (t == static_cast<int>(t_esaurimento)) b.esaurisci();
            const auto f = b.avanza(1.0, {quota, velocita});
            quota = std::min(fondo, quota + rateo_verticale_regime(f.peso_netto_N, b.area_pianta_m2(), velocita, piani));
            if (quota <= 5.0) return Esito{true, false, quota};
            if (b.perso()) return Esito{false, true, quota};
        }
        return Esito{false, false, quota};
    };
    auto quota_collasso = [](const BattelloDanni& b) {
        double q = -1;
        for (const auto& e : b.eventi())
            if (e.tipo == EventoBattello::Tipo::collasso && e.compartimento < 0) std::sscanf(e.testo.c_str(), "collasso dello scafo resistente a %lf", &q);
        return q;
    };

    SUBCASE("Thresher (1963): falla di 5\" in sala macchine a 300 m senza propulsione, esaurimento inutile, implosione") {
        // lo Sturgeon ha la quota di collaudo del Thresher (400 m, collasso previsto 590-600 m); [WP-Thr], [USNI21]
        const auto d = DatiScafo::carica(SCAFI_SUB + "sturgeon.toml");
        for (std::uint64_t seme = 1; seme <= 10; ++seme) {
            BattelloDanni b(d, seme);
            int macchine = 0;
            for (std::size_t i = 0; i < b.compartimenti().size(); ++i)
                if (b.compartimenti()[i].uso == "macchine") macchine = static_cast<int>(i);
            b.aggiungi_falla(macchine, foro(5));
            CHECK(moto(b, 300.0, 0.0, 0, 60).perso);
            // implosione a 1,1-1,8 volte la quota prevista: il Thresher a ~730 m (x 1,24) e' dentro
            const double q = quota_collasso(b);
            CHECK(q >= 1.1 * d.sottomarino.quota_collasso_m - 5.0);
            CHECK(q <= 1.8 * d.sottomarino.quota_collasso_m + 5.0);
        }
    }
    SUBCASE("San Francisco (2005): casse di prua squarciate a 160 m, l'esaurimento basta (dossier 07 §8.4)") {
        BattelloDanni b(DatiScafo::carica(SCAFI_SUB + "los_angeles.toml"), 1);
        b.squarcia_cassa(0);
        CHECK(moto(b, 160.0, 0.0, 0, 10).emerso);
    }
    SUBCASE("Kursk (2000): falle a prua a quota periscopica, sul fondo a 108 m senza collasso, poppa asciutta") {
        // primo scoppio nel compartimento 1, secondo 135,8 s dopo (dossier 07 §8.2, [NORSAR02]); nessun esaurimento
        BattelloDanni b(DatiScafo::carica(SCAFI_SUB + "oscar.toml"), 1);
        b.aggiungi_falla(0, 1.0);
        double quota = 20.0;
        for (int t = 0; t < 4 * 3600; ++t) {
            if (t == 136) { b.aggiungi_falla(0, 3.0); b.aggiungi_falla(1, 3.0); }
            const auto f = b.avanza(1.0, {quota, 0.0});
            quota = std::min(108.0, quota + rateo_verticale_regime(f.peso_netto_N, b.area_pianta_m2(), 0.0, 0));
        }
        CHECK(quota == doctest::Approx(108.0));
        CHECK_FALSE(b.perso());
        const auto& c = b.compartimenti();
        CHECK(c.front().acqua > 0.8 * c.front().volume);
        CHECK(c.back().acqua == 0.0);   // i superstiti del 9° compartimento resistettero ore
    }
    SUBCASE("Kursk (2000) con le esplosioni interne: 150 kg nel compartimento 1, poi 2 t sul fondo 135 s dopo") {
        // [NORSAR02]: primo scoppio 100-250 kg (siluro), secondo 1-3 t (testate), fondale ~108 m; superstiti nel 9°
        BattelloDanni b(DatiScafo::carica(SCAFI_SUB + "oscar.toml"), 1);
        b.esplosione_interna(0, 150.0);
        CHECK(b.paratie().front().ceduta == false);   // il primo scoppio non sfonda la paratia
        double quota = 20.0, fondo = -1;
        for (int t = 0; t < 4 * 3600; ++t) {
            if (t == 135) b.esplosione_interna(0, 2000.0);
            const auto f = b.avanza(1.0, {quota, 0.0});
            quota = std::min(108.0, quota + rateo_verticale_regime(f.peso_netto_N, b.area_pianta_m2(), 0.0, 0));
            if (fondo < 0 && quota >= 108.0) fondo = t;
        }
        CHECK(fondo >= 0);
        CHECK(fondo < 135);              // a fondo prima del secondo scoppio
        CHECK_FALSE(b.perso());          // 108 m: nessun collasso
        const auto& c = b.compartimenti();
        for (int i = 0; i < 4; ++i) CHECK(c[static_cast<std::size_t>(i)].acqua > 0.8 * c[static_cast<std::size_t>(i)].volume);   // prua sventrata
        for (std::size_t i = 5; i < c.size(); ++i) CHECK(c[i].acqua == 0.0);   // dal reattore a poppa: asciutti
        CHECK(c.front().distrutto);
    }
    SUBCASE("incendio in un compartimento chiuso: consuma l'ossigeno e si spegne; l'aria ad alta pressione lo tiene vivo") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "kilo.toml");
        BattelloDanni chiuso(d, 1), aria(d, 1);
        chiuso.incendio(1, 0.5);
        aria.incendio(1, 0.5, true);
        double spento = -1;
        for (int t = 0; t < 1800 && spento < 0; ++t) {
            chiuso.avanza(1.0, {60.0, 0.0});
            if (chiuso.compartimenti()[1].fuoco == 0.0) spento = t;
        }
        CHECK(spento > 0);
        CHECK(spento < 900);
        CHECK(chiuso.compartimenti()[1].ossigeno < 0.14);
        CHECK(chiuso.compartimenti()[1].fumo > 0.8);
        aria.avanza(spento + 60.0, {60.0, 0.0});
        CHECK(aria.compartimenti()[1].fuoco > 0.0);                 // ancora acceso finche' c'e' aria nelle bombole
        CHECK(aria.aria_bombole_Nm3() < chiuso.aria_bombole_Nm3());
        // i sistemi del compartimento bruciano; il fumo fa rapporto
        bool incendio = false, spento_ev = false;
        for (const auto& e : chiuso.eventi()) {
            incendio |= e.tipo == EventoBattello::Tipo::incendio;
            spento_ev |= e.tipo == EventoBattello::Tipo::incendio_spento;
        }
        CHECK(incendio);
        CHECK(spento_ev);
    }
    SUBCASE("cariche della WWII (130 kg): spaccano lo scafo a ~6 m, costringono all'emersione a ~12 m ([WP-DC])") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "foxtrot.toml");
        int rotture6 = 0;
        for (std::uint64_t seme = 1; seme <= 20; ++seme) {
            BattelloDanni a(d, seme);
            a.urto(130, 6.0, 0.785, 0, 60);
            for (const auto& e : a.eventi()) rotture6 += e.testo == "rottura dello scafo resistente";
            BattelloDanni b(d, seme);
            b.urto(130, 12.0, 0.785, 0, 60);
            for (const auto& e : b.eventi()) CHECK(e.testo != "rottura dello scafo resistente");
            CHECK_FALSE(b.falle().empty());
            CHECK(moto(b, 60.0, 3.0 * b.limiti().velocita, b.limiti().idroplani ? +1 : 0, 30).emerso);
        }
        CHECK(rotture6 >= 8);   // circa la meta' (dossier: "spacca a ~6 m")
    }
}

TEST_CASE("battello: squadre di controllo danni al lavoro" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    BattelloDanni b(d, 1);
    CHECK(b.squadre().empty());
    b.aggiungi_falla(2, 0.002);   // tamponabile, nelle macchine
    b.incendio(0, 0.5);
    b.avanza(0.5, {20.0, 3.0});
    REQUIRE(b.squadre_disponibili() >= 1);
    int tamponare = 0, spegnere = 0, impegnate = 0;
    for (const auto& l : b.squadre()) {
        if (l.compito == SquadraBattello::Compito::tamponare) {
            ++tamponare;
            CHECK(l.compartimento == 2);
            CHECK(l.falla == 0);
        }
        if (l.compito == SquadraBattello::Compito::spegnere) {
            ++spegnere;
            CHECK(l.compartimento == 0);
            CHECK(l.numero >= 1);
        }
        if (l.compito != SquadraBattello::Compito::pompare) impegnate += l.numero;
    }
    CHECK(tamponare == 1);
    CHECK(spegnere == 1);
    CHECK(impegnate <= b.squadre_disponibili());
    // falla tamponata: le pompe tolgono l'acqua (senza squadre)
    bool pompe = false;
    for (int i = 0; i < 2400 && !pompe; ++i) {
        b.avanza(0.5, {20.0, 3.0});
        for (const auto& l : b.squadre())
            if (l.compito == SquadraBattello::Compito::pompare) {
                pompe = true;
                CHECK(l.compartimento == 2);
                CHECK(l.numero == 0);
                CHECK(l.portata > 0.0);
            }
    }
    CHECK(pompe);
    // un urto guasta i sistemi: spento il focolaio, le squadre libere li riparano nel compartimento che li ospita
    BattelloDanni c(d, 1);
    c.urto(135, 15, 1.57, c.compartimenti()[2].xc, 20.0);
    bool ripara = false;
    for (int i = 0; i < 600 && !ripara; ++i) {   // prima il piccolo incendio acceso dall'urto
        c.avanza(0.5, {20.0, 3.0});
        for (const auto& l : c.squadre())
            if (l.compito == SquadraBattello::Compito::riparare) {
                ripara = true;
                CHECK(l.sistema >= 0);
                CHECK(l.compartimento == c.compartimento_di(static_cast<Sistema>(l.sistema)));
            }
    }
    CHECK(ripara);
}

TEST_CASE("battello: acqua imbarcata senza falla (la canna del periscopio col vetro rotto, G3)" * doctest::skip(!ci_sono_sottomarini())) {
    BattelloDanni b(DatiScafo::carica(SCAFI_SUB + "los_angeles.toml"), 1);
    const int c = b.compartimento_di(Sistema::periscopio);
    REQUIRE(c >= 0);
    const auto acqua = [&] { return b.compartimenti()[static_cast<std::size_t>(c)].acqua; };
    // l'acqua entra nel compartimento, mai oltre il suo volume, e il rapporto parte al primo ingresso
    b.imbarca(c, 2.0);
    CHECK(acqua() == doctest::Approx(2.0));
    b.imbarca(c, 1e6);
    CHECK(acqua() <= b.compartimenti()[static_cast<std::size_t>(c)].volume);
    CHECK(b.falle().empty());
    int rapporti = 0;
    for (const auto& e : b.eventi()) rapporti += e.tipo == EventoBattello::Tipo::allagamento && e.compartimento == c;
    CHECK(rapporti == 1);
    b.imbarca(-1, 5.0);   // compartimento fuori dai limiti, acqua negativa: niente
    b.imbarca(c, -1.0);
    // 0,005 m^3/s per 10 minuti a quota periscopica (18 m^3 all'ora): le pompe tengono la centrale quasi asciutta
    BattelloDanni p(DatiScafo::carica(SCAFI_SUB + "los_angeles.toml"), 1);
    StatoMoto moto;
    moto.quota_m = 18.0;
    for (int k = 0; k < 600; ++k) {
        p.imbarca(c, 0.005);
        p.avanza(1.0, moto);
    }
    CHECK(p.compartimenti()[static_cast<std::size_t>(c)].acqua < 0.5);
    CHECK(p.falle().empty());
}

TEST_CASE("battello: il navigatore inerziale, urti e acqua nel comando" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d688 = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    // i nomi: una fonte sola, anche per il sistema nuovo
    CHECK(std::string(chiave_sistema(Sistema::inerziale)) == "inerziale");
    CHECK(std::string(nome_sistema(Sistema::inerziale)) == "navigatore inerziale");
    for (int i = 0; i < static_cast<int>(Sistema::numero); ++i)
        CHECK(sistema_da_chiave(chiave_sistema(static_cast<Sistema>(i))) == static_cast<Sistema>(i));
    CHECK_FALSE(sistema_da_chiave("nessuno"));
    // integro: funziona, e sta nel comando come il periscopio
    BattelloDanni b(d688, 1);
    CHECK(b.limiti().inerziale);
    CHECK(b.compartimento_di(Sistema::inerziale) == b.compartimento_di(Sistema::periscopio));
    // urti: soffre piu' degli altri sistemi. Un fattore d'urto di 0,4 lo guasta sempre; 0,02 mai
    int giu = 0;
    for (std::uint64_t seme = 1; seme <= 50; ++seme) {
        BattelloDanni u(d688, seme);
        u.urto(294.8, 20.0, 1.5707963, 0, 0);   // Mk 48 a 20 m sotto: HSF circa 0,39
        giu += u.sistema(Sistema::inerziale).efficienza < 1.0;
    }
    CHECK(giu == 50);
    BattelloDanni lontano(d688, 1);
    lontano.urto(1.0, 200.0, 0.0, 0, 0);
    CHECK(lontano.sistema(Sistema::inerziale).efficienza == 1.0);
    // l'acqua nel comando oltre il 20% lo ferma; i limiti lo dicono
    BattelloDanni a(d688, 1);
    const int c = a.compartimento_di(Sistema::inerziale);
    a.imbarca(c, 0.25 * a.compartimenti()[static_cast<std::size_t>(c)].volume);
    StatoMoto moto;
    moto.quota_m = 18.0;
    a.avanza(1.0, moto);
    CHECK(a.sistema(Sistema::inerziale).efficienza == 0.0);
    CHECK_FALSE(a.limiti().inerziale);
}

TEST_CASE("battello: isolare un compartimento e le priorita' delle squadre (docs/10 §10.19)" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d688 = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    const auto dalfa = DatiScafo::carica(SCAFI_SUB + "alfa.toml");
    const auto avanza = [](BattelloDanni& b, int secondi) { for (int t = 0; t < secondi; ++t) b.avanza(1.0, {100.0, 5.0}); };
    SUBCASE("il posto di comando non si isola") {
        BattelloDanni b(d688, 1);
        CHECK_FALSE(b.isolabile(0));
        CHECK(b.isolabile(1));
        CHECK(b.isolabile(2));
        b.isola(0);
        CHECK_FALSE(b.compartimenti()[0].senza_squadre());
        BattelloDanni a(dalfa, 1);
        a.senza_personale(true);
        CHECK_FALSE(a.isolabile(2));
        for (const int i : {0, 1, 3, 4, 5}) CHECK(a.isolabile(i));
    }
    SUBCASE("688: le squadre escono subito, le porte dopo un minuto, poi il motore al 15%") {
        BattelloDanni b(d688, 1);
        b.incendio(2, 0.8);
        avanza(b, 1);
        bool dentro = false;
        for (const auto& l : b.squadre()) dentro |= l.compartimento == 2 && l.compito == SquadraBattello::Compito::spegnere;
        CHECK(dentro);
        b.isola(2);
        avanza(b, 1);
        for (const auto& l : b.squadre()) CHECK(l.compartimento != 2);
        CHECK(b.squadre_disponibili() == 2);
        CHECK(b.limiti().velocita > 0.15);   // sgombero in corso
        CHECK_FALSE(b.compartimenti()[2].isolato);
        avanza(b, 60);
        CHECK(b.compartimenti()[2].isolato);
        CHECK(b.compartimenti()[2].dentro == 0);
        CHECK(b.limiti().velocita <= 0.15);
        b.riapri(2);
        CHECK_FALSE(b.compartimenti()[2].senza_squadre());
        CHECK(b.caduti() == 0);
    }
    SUBCASE("Alfa: si chiude subito, nessuno dentro, il motore non ne risente") {
        BattelloDanni b(dalfa, 1);
        b.senza_personale(true);
        b.isola(4);
        CHECK(b.compartimenti()[4].isolato);
        CHECK(b.compartimenti()[4].dentro == 0);
        CHECK(b.limiti().velocita == 1.0);
    }
    SUBCASE("a porte chiuse il fuoco si spegne senza ossigeno, l'aria non torna finche' non si riapre") {
        BattelloDanni b(d688, 3);
        b.incendio(2, 0.8);
        b.isola(2);
        avanza(b, 1800);
        const auto& c = b.compartimenti()[2];
        CHECK(c.fuoco == 0.0);
        CHECK(c.ossigeno < 0.13);
        const double fumo = c.fumo;
        avanza(b, 300);
        CHECK(b.compartimenti()[2].ossigeno < 0.13);
        CHECK(b.compartimenti()[2].fumo == fumo);
        const double ossigeno = b.compartimenti()[2].ossigeno;
        b.riapri(2);
        avanza(b, 300);
        CHECK(b.compartimenti()[2].ossigeno > ossigeno);
        CHECK(b.compartimenti()[2].fumo < fumo);
    }
    SUBCASE("riaprire a fuoco vivo: l'ossigeno risale al 16%, il fumo passa ai vicini") {
        BattelloDanni b(d688, 3);
        b.incendio(2, 0.8);
        b.isola(2);
        avanza(b, 600);
        REQUIRE(b.compartimenti()[2].fuoco > 0.0);
        REQUIRE(b.compartimenti()[2].ossigeno < 0.16);
        const double fumo = b.compartimenti()[2].fumo, vicino = b.compartimenti()[1].fumo;
        b.riapri(2);
        CHECK(b.compartimenti()[2].ossigeno == doctest::Approx(0.16));
        CHECK(b.compartimenti()[2].fumo < fumo);
        CHECK(b.compartimenti()[1].fumo > vicino);
    }
    SUBCASE("le porte chiuse in tempo fermano il tubo dell'aria, il contagio resta possibile a un quarto") {
        int tubi[2] = {0, 0}, contagi[2] = {0, 0};
        for (std::uint64_t seme = 1; seme <= 40; ++seme)
            for (const int isola : {0, 1}) {
                BattelloDanni b(dalfa, seme);
                b.senza_personale(true);
                b.incendio(4, 1.0);
                if (isola) b.isola(4);
                avanza(b, 1200);
                for (const auto& e : b.eventi()) {
                    tubi[isola] += e.tipo == EventoBattello::Tipo::aria_nell_incendio;
                    contagi[isola] += e.tipo == EventoBattello::Tipo::incendio && e.compartimento != 4;
                }
            }
        CHECK(tubi[0] > 0);
        CHECK(tubi[1] == 0);
        CHECK(contagi[1] < contagi[0]);
    }
    SUBCASE("contro l'acqua isolare non ferma niente") {
        double acqua[2] = {0.0, 0.0};
        for (const int isola : {0, 1}) {
            BattelloDanni b(d688, 1);
            b.aggiungi_falla(2, 0.02);
            if (isola) b.isola(2);
            avanza(b, 600);
            acqua[isola] = b.compartimenti()[2].acqua;
        }
        CHECK(acqua[1] == doctest::Approx(acqua[0]).epsilon(0.01));
    }
    SUBCASE("chi resta dentro: muore quando il compartimento e' letale, torna fuori se si riapre prima") {
        for (const bool riapre : {false, true}) {
            BattelloDanni b(d688, 1);
            b.incendio(2, 0.8);
            b.isola(2);
            avanza(b, 30);
            b.chiudi_subito(2);
            CHECK(b.compartimenti()[2].isolato);
            CHECK(b.compartimenti()[2].dentro == doctest::Approx(10).epsilon(0.11));   // 19 uomini, meta' sgombero
            const int dentro = b.compartimenti()[2].dentro;
            if (riapre) { avanza(b, 60); b.riapri(2); }
            avanza(b, 1200);
            CHECK(b.caduti() == (riapre ? 0 : dentro));
            int eventi = 0;
            for (const auto& e : b.eventi()) if (e.tipo == EventoBattello::Tipo::caduti) { ++eventi; CHECK(e.uomini == dentro); }
            CHECK(eventi == (riapre ? 0 : 1));
        }
    }
    SUBCASE("le priorita': una squadra sola va sul sistema in cima") {
        auto d = d688;
        d.squadre = 1;
        for (const bool sonar : {false, true}) {
            BattelloDanni b(d, 1);
            b.guasto_sistema(Sistema::timone, 0.5, "prova");
            b.guasto_sistema(Sistema::sonar_prua, 0.5, "prova");
            if (sonar) b.priorita(Sistema::sonar_prua);
            CHECK((b.ordine_riparazioni().front() == Sistema::sonar_prua) == sonar);
            avanza(b, 60);
            CHECK((b.sistema(Sistema::sonar_prua).efficienza > 0.5) == sonar);
            CHECK((b.sistema(Sistema::timone).efficienza > 0.5) == !sonar);
        }
    }
}

TEST_CASE("battello: col fuoco nel compartimento un sistema si dice riparato una volta sola" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    const auto riparati = [](const BattelloDanni& b, Sistema s) {
        int n = 0;
        for (const auto& e : b.eventi()) n += e.tipo == EventoBattello::Tipo::riparazione && e.sistema == static_cast<int>(s);
        return n;
    };
    const int reattore = BattelloDanni(d, 1).compartimento_di(Sistema::propulsione);
    for (const bool guasto : {false, true}) {
        BattelloDanni b(d, 1);
        b.incendio(reattore, 0.1);
        b.incendio(0, 0.1);   // due focolai: delle tre squadre una resta libera per le riparazioni
        if (guasto) b.guasto_sistema(Sistema::propulsione, 0.01, "prova");
        bool fuoco_alla_fine = false;
        int giri = 0;
        for (int t = 0; t < 40; ++t) {
            b.avanza(1.0, {100.0, 5.0});
            for (const auto& l : b.squadre()) giri += l.compito == SquadraBattello::Compito::riparare && l.sistema == static_cast<int>(Sistema::propulsione);
            fuoco_alla_fine = b.compartimenti()[static_cast<std::size_t>(reattore)].fuoco > 0.0;
        }
        CHECK(fuoco_alla_fine);       // la prova guarda il fuoco ancora vivo
        CHECK(giri > 10);             // la squadra lo riporta su piu' volte: e' li' che l'evento si ripeteva
        CHECK(riparati(b, Sistema::propulsione) == (guasto ? 1 : 0));
    }
    // senza fuoco, come sempre
    BattelloDanni b(d, 1);
    b.guasto_sistema(Sistema::timone, 0.1, "prova");
    for (int t = 0; t < 120; ++t) b.avanza(1.0, {100.0, 5.0});
    CHECK(riparati(b, Sistema::timone) == 1);
}

TEST_CASE("battello: danni critici, fase 3 (carico vivo, silos liquidi, SCRAM, batteria, contenitori)" * doctest::skip(!ci_sono_sottomarini())) {
    const auto primo = [](const BattelloDanni& b, const char* uso) {
        for (std::size_t i = 0; i < b.compartimenti().size(); ++i)
            if (b.compartimenti()[i].uso == uso) return static_cast<int>(i);
        return -1;
    };
    SUBCASE("il 688: il carico dallo scafo, poi quello vivo; un deposito vuoto nel fuoco non salta") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
        BattelloDanni b(d, 1);
        const int k = primo(b, "deposito");
        REQUIRE(k >= 0);
        const auto& c = b.compartimenti()[static_cast<std::size_t>(k)];
        CHECK(c.carica_kg == doctest::Approx(5980.0));
        CHECK(c.protezione == doctest::Approx(0.3));
        const double p = c.propellente_kg;
        b.carico_deposito(k, 2990.0);   // meta' delle armi lanciate
        CHECK(c.carica_kg == doctest::Approx(2990.0));
        CHECK(c.propellente_kg == doctest::Approx(p / 2));
        for (std::uint64_t seme = 1; seme <= 20; ++seme) {
            BattelloDanni v(d, seme);
            v.carico_deposito(k, 0.0);
            v.incendio(k, 1.0, true);
            for (int t = 0; t < 1800; ++t) v.avanza(1.0, {100.0, 0.0});
            for (const auto& e : v.eventi()) CHECK(e.tipo != EventoBattello::Tipo::esplosione);
        }
    }
    SUBCASE("il Delta III: l'acqua nel silo del propellente liquido lo fa reagire, una volta (K-219)") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "delta3.toml");
        for (std::uint64_t seme = 1; seme <= 5; ++seme) {
            BattelloDanni b(d, seme);
            int k = -1;
            for (std::size_t i = 0; i < b.compartimenti().size(); ++i)
                if (b.compartimenti()[i].liquido_t > 0) k = static_cast<int>(i);
            REQUIRE(k >= 0);
            b.imbarca(k, 0.05 * b.compartimenti()[static_cast<std::size_t>(k)].volume);
            for (int t = 0; t < 7200; ++t) b.avanza(1.0, {50.0, 0.0});
            CHECK(b.compartimenti()[static_cast<std::size_t>(k)].reagito);
            int scoppi = 0;
            for (const auto& e : b.eventi()) scoppi += e.tipo == EventoBattello::Tipo::esplosione && e.compartimento == k;
            CHECK(scoppi == 1);
        }
    }
    SUBCASE("SCRAM per l'acqua nel reattore; con la batteria allagata niente energia") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "delta3.toml");
        BattelloDanni b(d, 1);
        const int r = b.compartimento_di(Sistema::propulsione);
        b.imbarca(r, 0.2 * b.compartimenti()[static_cast<std::size_t>(r)].volume);
        for (int t = 0; t < 10; ++t) b.avanza(1.0, {50.0, 0.0});
        CHECK(b.limiti().scram);
        CHECK(b.limiti().energia);   // la batteria regge il motore d'emergenza
        CHECK(b.limiti().velocita > 0.0);
        const int bt = primo(b, "batterie");
        REQUIRE(bt >= 0);
        b.imbarca(bt, 0.1 * b.compartimenti()[static_cast<std::size_t>(bt)].volume);
        for (int t = 0; t < 10; ++t) b.avanza(1.0, {50.0, 0.0});
        CHECK(b.batteria() == 0.0);
        CHECK_FALSE(b.limiti().energia);
        CHECK(b.limiti().velocita == 0.0);
    }
    SUBCASE("l'Oscar II: un siluro vicino ai contenitori dei P-700 li puo' far saltare") {
        const auto d = DatiScafo::carica(SCAFI_SUB + "oscar.toml");
        REQUIRE_FALSE(d.fuori_resistente.empty());
        const double x = (d.fuori_resistente.front().x0 + d.fuori_resistente.front().x1) / 2;
        int saltati = 0;
        for (std::uint64_t seme = 1; seme <= 20; ++seme) {
            BattelloDanni b(d, seme);
            b.urto(300.0, 2.0, 0.0, x, 100.0);
            for (const auto& e : b.eventi())
                if (e.tipo == EventoBattello::Tipo::esplosione && e.testo.find("contenitori") != std::string::npos) {
                    ++saltati;
                    CHECK(e.tnt_kg > 0.0);
                    CHECK(e.tnt_kg <= d.fuori_resistente.front().carica_kg);
                }
        }
        CHECK(saltati >= 1);
    }
}
