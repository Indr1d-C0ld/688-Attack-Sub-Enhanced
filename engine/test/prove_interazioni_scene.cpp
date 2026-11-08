#include <doctest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

#include "../app/data_missione.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Le scene del dossier 24 §8.9 come missioni di prova (data/missioni/prove, scritte da strumenti/scene_interazioni.cpp;
// piano docs/18_interazioni_ia.md, fase 5). La prima prova controlla che i file siano letti, verificati e giocabili su
// piu' semi; le altre portano le attese del dossier e sono marcate «da completare» (doctest::may_fail): passano solo
// quando il runtime avra' le reazioni delle fasi 2-4 (allerta, messaggi fra unita', zone, tabelle di reazione, ROE per
// parte). Quando una passa stabilmente su piu' semi, si toglie may_fail (S5 e S7 dal 27/10/2026, con la fase 4). Le attese giocano fino a 90 minuti per scena (circa due
// minuti in tutto): girano solo con M688_PROVE_LUNGHE nell'ambiente.

namespace {

const std::string DATI = M688_DATI;
const fs::path PROVE = fs::path(M688_MISSIONI) / "prove";
const char* const SCENE[] = {"PROVA_S1_KPUG",     "PROVA_S1G_KPUG_GUERRA", "PROVA_S2_PEDINAMENTO", "PROVA_S3_BASTIONE", "PROVA_S3A_BASTIONE_ALFA",
                             "PROVA_S4_DUE_SSN", "PROVA_S5_CONVOGLIO",    "PROVA_S6_USCITA",      "PROVA_S7_PORTAEREI",
                             "PROVA_S8_LUOGO",    "PROVA_M1_ANTINAVE", "PROVA_M2_TLAM",   // i missili accesi dal 28/10/2026
                             "PROVA_M3_ORIZZONTE",   // l'esperimento della spia, dal 04/11/2026
                             "PROVA_M4_QUADRO",   // il quadro radio ai sottomarini, dal 05/11/2026
                             "PROVA_T1_EMERSIONE", "PROVA_T2_MISSILI", "PROVA_T3_FUOCO", "PROVA_T4_ELICOTTERO",   // le scene del trailer, dal 29/10/2026
                             "PROVA_T5_VOLTA",     "PROVA_T6_POLYNYA",  "PROVA_T7_MARGINE", "PROVA_T8_FANALI",   // del trailer n. 3, dal 30/10/2026
                             "PROVA_T9_COSTA"};   // la costa al periscopio, dal 30/10/2026
bool c_e() { return fs::exists(PROVE / "PROVA_S7_PORTAEREI.lua") && fs::exists(DATI + "/missioni_rapide.toml"); }
bool attese() { return c_e() && std::getenv("M688_PROVE_LUNGHE"); }

std::string leggi(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

// una partita nell'Enhanced pieno, col giocatore fermo ad ascoltare; ogni 10 s annota il primo contatto utilizzabile
// (traccia solida o posizione) di ogni unita' su ogni altra, le armi in acqua (chi, su chi, quando) e le unita' colpite
struct Partita {
    struct Arma {
        double t;
        int da, su;
    };
    std::map<std::string, ClasseNave> classi = carica_classi(DATI + "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    std::map<std::pair<int, int>, double> visto, sentito;   // usabile; qualsiasi contatto
    std::map<int, Arma> armi;
    std::map<int, double> colpita;

    Partita(const std::string& id, std::uint64_t seme)
        : m((PROVE / (id + ".lua")).string(), seme), dm(classi, DATI, seme, RegolaDanni::simulazione), ds(classi, DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        // la data della scena, come in partita (app/data_missione.hpp): mese, profilo del suono e sole sono quelli che si giocano
        if (const auto d = data_della_missione(PROVE, id)) m.imposta_data_ora(d->giorno, d->mese, d->anno, d->ore_utc);
        m.radio_enhanced();
        if (const auto* g = m.giocatore()) {
            ds.prepara(m, g->id);
            auto* u = m.trova(g->id);
            u->velocita_ordinata = 0.0;
            u->velocita_salvata = 0.0;
        }
    }
    void gioca(double secondi) {
        while (m.tempo() < secondi && !m.esito()) {
            for (int k = 0; k < 90; ++k) {
                m.avanza(1.0 / 9.0);
                dm.avanza(m, 1.0 / 9.0);
                ds.avanza(m, 1.0 / 9.0);
                m.raccogli_danni(&ds, &dm);
            }
            for (const auto& [id, u] : m.unita()) {
                if (u.flag & 0x20) {
                    if (!armi.count(id)) armi[id] = {m.tempo(), u.lanciatore, u.bersaglio};
                    continue;
                }
                if (u.colpo && !colpita.count(id)) colpita[id] = m.tempo();
                for (const auto& [altro, c] : u.contatti) {   // il contatto che l'IA puo' usare (come scoperta_ia), e il primo
                    if ((c.qualita >= 800.0 || c.posizione_nota) && !visto.count({id, altro})) visto[{id, altro}] = m.tempo();
                    if (!sentito.count({id, altro})) sentito[{id, altro}] = m.tempo();
                }
            }
        }
    }
    double quando_vede(int chi, int cosa) const {   // -1: mai
        const auto it = visto.find({chi, cosa});
        return it == visto.end() ? -1.0 : it->second;
    }
    double quando_sente(int chi, int cosa) const {   // il primo contatto di qualsiasi qualita' (-1: mai)
        const auto it = sentito.find({chi, cosa});
        return it == sentito.end() ? -1.0 : it->second;
    }
};

}  // namespace

TEST_CASE("scene delle interazioni: i file si leggono, si verificano, si riscrivono uguali e si giocano su due semi" * doctest::skip(!c_e())) {
    const DatiScenario d = carica_dati_scenario(DATI);
    for (const char* id : SCENE) {
        CAPTURE(id);
        const std::string testo = leggi(PROVE / (std::string(id) + ".lua"));
        const auto s = leggi_scenario(testo);
        REQUIRE(s.has_value());
        CHECK(s->id == id);
        CHECK(scrivi_missione_lua(*s, d) == testo);
        const Fondale f = Fondale::carica(DATI + "/teatri/" + s->teatro + ".fdl");
        for (const auto& a : verifica(*s, f, d)) CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
        for (std::uint64_t seme = 1; seme <= 2; ++seme) {
            Partita p(id, seme);
            p.gioca(180.0);
            CHECK(p.m.errori().empty());
            CHECK(p.m.tempo() >= 180.0);
        }
    }
}

// --- le attese del dossier, da completare ---

// il «VDS a t=0» del dossier e' il primo contatto, non la traccia solida, che viene per integrazione: entro 1 minuto il
// primo contatto del Krivak di destra, entro 6 la traccia solida di tutti e tre (runtime, 27-28/10: dopo la nuova
// acustica, su sei semi fra 150 e 350 s)
TEST_CASE("S1: il VDS sente lo Sturgeon subito, i tre del KPUG lo tengono entro 6 minuti" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        CAPTURE(seme);
        Partita p("PROVA_S1_KPUG", seme);
        p.gioca(360.0);
        const double vds = p.quando_sente(3, 2);
        CHECK(vds >= 0.0);
        CHECK(vds <= 60.0);
        for (int nave = 3; nave <= 5; ++nave) {
            CAPTURE(nave);
            CHECK(p.quando_vede(nave, 2) >= 0.0);
        }
    }
}

// il primo lancio, dopo PRIMO_ATTACCO_S (780 s) sul contatto, entro i 26 minuti del dossier (15-25, con un minuto di tolleranza).
// Lo scarto dai 15 minuti e il colpo, che arriva molto dopo, sono limiti in E6 §7
TEST_CASE("S1G: in guerra il KPUG lancia sullo Sturgeon dopo il PROBSUB, fra 13 e 26 minuti" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        CAPTURE(seme);
        Partita p("PROVA_S1G_KPUG_GUERRA", seme);
        p.gioca(1560.0);
        double primo = -1.0;
        for (const auto& [id, a] : p.armi)
            if (a.su == 2 && (primo < 0.0 || a.t < primo)) primo = a.t;
        REQUIRE(primo >= 0.0);
        CHECK(primo >= 780.0);
        CHECK(primo <= 1560.0);
    }
}

// il P-3 entra a 45 minuti e va dove la Perry col TACTAS ha il Victor: si conta dal suo cue, cioe' dal piu' tardo fra
// l'entrata e il contatto della Perry; 35 minuti per transito, datum e contatto (runtime, 27/10: 18-31 nei tre semi, poi
// il seme 3 senza contatto in 80 minuti). Ferma su tre semi dal 28/10/2026, con la TMA corretta e la schiera vera:
// 1010-1750 s dal cue
TEST_CASE("S2: il P-3 prende il contatto del Victor entro 35 minuti dal cue della Perry" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        CAPTURE(seme);
        Partita p("PROVA_S2_PEDINAMENTO", seme);
        p.gioca(4800.0);
        const double perry = p.quando_vede(3, 2);
        REQUIRE(perry >= 0.0);
        const double cue = std::max(2700.0, perry), p3 = p.quando_sente(5, 2);   // l'aereo: il contatto delle boe
        CHECK(p3 >= 0.0);
        CHECK(p3 - cue <= 2100.0);
        CHECK(p.armi.empty());   // in pace nessuno lancia
    }
}

TEST_CASE("S3A: Il-38, KPUG o Victor trovano l'intruso entro 90 minuti" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {   // stabile da quando l'intruso entra a 25 km dal KPUG (27/10/2026)
        CAPTURE(seme);
        Partita p("PROVA_S3A_BASTIONE_ALFA", seme);
        p.gioca(5400.0);
        bool trovato = false;
        for (int u = 3; u <= 8; ++u) trovato = trovato || p.quando_vede(u, 2) >= 0.0;
        CHECK(trovato);
    }
}

// il dossier: «il Trafalgar sente un contatto debole sul confine, non classificato» e non lancia nella fascia; il
// contatto e' di qualsiasi qualita' (quando_sente), non la traccia solida. Ferma su tre semi dal 28/10/2026, coi due SSN
// a 2 e 4 km dal confine
TEST_CASE("S4: il Trafalgar sente un contatto sul confine entro 10 minuti e non lancia" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        CAPTURE(seme);
        Partita p("PROVA_S4_DUE_SSN", seme);
        p.gioca(600.0);
        CHECK(p.quando_sente(2, 0) >= 0.0);
        for (const auto& [id, a] : p.armi) CHECK(a.su != 0);
    }
}

TEST_CASE("S5: dopo il colpo le scorte cercano almeno 5 minuti prima di colpire il Victor" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {   // stabile dalla fase 4 di docs/18 (27/10/2026)
        CAPTURE(seme);
        Partita p("PROVA_S5_CONVOGLIO", seme);
        p.gioca(1200.0);
        double primo = -1.0;
        for (int m = 2; m <= 7; ++m)
            if (p.colpita.count(m) && (primo < 0.0 || p.colpita[m] < primo)) primo = p.colpita[m];
        REQUIRE(primo >= 0.0);   // la scena: un mercantile e' colpito
        if (p.colpita.count(12)) CHECK(p.colpita[12] - primo >= 300.0);
    }
}

// l'SSN aspetta sul fango della scarpata a ~49 km dalla rotta d'uscita (runtime, 28/10): prima del messaggio delle 30
// minuti non sente il Typhoon; dopo, nelle due ore del vetrino, lo trova a ~73 minuti sui semi 1 e 2, non sul 3
TEST_CASE("S6: l'SSN che aspetta trova il Typhoon solo dopo il messaggio della nave spia" * doctest::skip(!attese())) {
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        CAPTURE(seme);
        Partita p("PROVA_S6_USCITA", seme);
        p.gioca(1800.0);
        CHECK(p.quando_vede(4, 2) < 0.0);
    }
}

TEST_CASE("S7: senza la copia dell'ordine di guerra il Victor non lancia; il Kara si'" * doctest::skip(!attese())) {
    // ferma dal 06/11/2026: prima il Victor lanciava alla guerra se aveva la soluzione (seme 3), ora fino alla copia sta in
    // R_pedina_vicino, che non lancia
    for (std::uint64_t seme = 1; seme <= 3; ++seme) {
        Partita p("PROVA_S7_PORTAEREI", seme);
        p.gioca(4200.0);
        CAPTURE(seme);
        bool kara = false;
        for (const auto& [id, a] : p.armi) {
            CHECK(a.da != 8);
            kara = kara || a.da == 9;
        }
        CHECK(kara);
    }
}

TEST_CASE("S8: la Koni del luogo e' in crisi col 688 e in pace col Foxtrot; a 15 minuti la guerra solo con gli USA" * doctest::skip(!c_e())) {
    Partita p("PROVA_S8_LUOGO", 1);
    p.gioca(10.0);
    REQUIRE(p.m.unita().count(2));
    CHECK((p.m.unita().at(2).flag & 0x18) == 0x18);
    CHECK(p.m.situazione_fra(0x18, 0x08) == Situazione::crisi);
    CHECK(p.m.situazione_fra(0x18, 0x10) == Situazione::pace);
    CHECK_FALSE(p.m.armi_libere_ia(2, 3).value_or(true));
    p.gioca(960.0);
    CHECK(p.m.situazione_fra(0x18, 0x08) == Situazione::guerra);
    CHECK(p.m.situazione_fra(0x08, 0x10) == Situazione::pace);   // fra il 688 e il Foxtrot resta la pace
    CHECK_FALSE(p.m.armi_libere_ia(2, 3).value_or(true));
    CHECK(p.m.errori().empty());
}
