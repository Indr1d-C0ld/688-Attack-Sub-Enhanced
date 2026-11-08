#include <doctest.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "missioni/comandante_ia.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/sandbox_originali.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/rotte.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il piano del comandante IA per le 22 originali nella sandbox (V3, missioni/sandbox_originali.hpp): le voci si
// leggono, stanno nella geometria della missione (partenza dentro il rettangolo, punti in acqua), e con DIAG_ORIGINALE
// una partita col comandante e il piano per la diagnosi.

namespace {

bool c_e() { return fs::exists(M688_DATI "/sandbox_originali.toml") && fs::exists(std::string(M688_MISSIONI) + "/SCENE5A.lua"); }

std::shared_ptr<const Fondale> fondale(const std::string& teatro) {
    const std::string f = std::string(M688_DATI) + "/teatri/" + teatro + ".fdl";
    return fs::exists(f) ? std::make_shared<const Fondale>(Fondale::carica(f)) : nullptr;
}

// una partita come quella di app/partita, col fondale: ordini dal comandante, missione, stazioni, danni
struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    std::unique_ptr<StazioniMissione> st;
    RegistroOrdini ordini;
    std::unique_ptr<ComandanteIa> comandante;
    std::unique_ptr<CatalogoCarico> catalogo;
    Partita(const std::string& id, std::uint64_t seme)
        : m(std::string(M688_MISSIONI) + "/" + id + ".lua", seme), dm(classi, M688_DATI, seme, RegolaDanni::simulazione), ds(classi, M688_DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        if (auto f = fondale(m.teatro())) m.imposta_fondale(f, pescaggi);
        m.senza_giocatore(true);
        m.radio_enhanced();                     // come app/partita e main: la sala radio, il meteo, il sonarista medio
        m.meteo_enhanced(M688_DATI);
        m.imposta_sonarista(Esperienza::medio);
        st = std::make_unique<StazioniMissione>(m, static_cast<std::uint32_t>(seme));
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
        st->usa_danni(&ds);
        OpzioniComandante o;
        if (!std::getenv("DIAG_SENZA_PIANO")) o.piano = piano_originale(M688_DATI, id);   // il confronto col comandante di V2
        if (o.piano && !o.piano->tubi.empty()) {   // il carico del piano prima del primo passo, come main nella sandbox
            catalogo = std::make_unique<CatalogoCarico>(carica_carico(M688_DATI "/carico.toml"));
            SalaSiluri sala(*catalogo, m.giocatore()->classe, m.limiti_carico());
            sala.carico_iniziale();
            for (const auto& x : applica_tubi_piano(sala, *o.piano)) MESSAGE("carico: ", x);
            ordini.imposta_carico(sala.carico());
            st->imposta_carico(m, sala.carico(), *catalogo);
        }
        comandante = std::make_unique<ComandanteIa>(m, o, seme);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            for (const auto& o : comandante->passo(m, *st)) ordini.ordina(o);
            for (const auto& o : ordini.consegna(m.tempo())) {
                if (ds.consegna(m, o)) continue;
                st->consegna(m, o);
            }
            m.avanza(1.0 / 9.0);
            st->dopo_passo(m);
            dm.avanza(m, 1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            if (m.esito()) break;
        }
    }
};

std::vector<std::string> originali() {
    std::vector<std::string> v;
    for (const auto& e : fs::directory_iterator(M688_MISSIONI))
        if (const std::string n = e.path().filename().string(); n.rfind("SCENE", 0) == 0 && e.path().extension() == ".lua")
            v.push_back(e.path().stem().string());
    std::sort(v.begin(), v.end());
    return v;
}

}  // namespace

TEST_CASE("sandbox V3: ogni originale ha il piano, nella geometria della missione" * doctest::skip(!c_e())) {
    const auto ids = originali();
    REQUIRE(ids.size() == 22);
    for (const auto& id : ids) {
        CAPTURE(id);
        const auto p = piano_originale(M688_DATI, id);
        REQUIRE(p);
        CHECK_FALSE(p->fonte.empty());
        Missione m(std::string(M688_MISSIONI) + "/" + id + ".lua", 1);
        const auto f = fondale(m.teatro());
        // la partenza del battello sta dentro il rettangolo da cui non uscire, e l'uscita (o l'ultimo punto) e' raggiungibile
        if (p->limite) CHECK(p->limite->dentro(m.giocatore()->pos));
        // i punti, le aree e la PIM in acqua (il fondale del teatro, dove c'e')
        if (f) {
            std::vector<Vec2> qui = p->punti;
            if (p->area) qui.push_back(p->area->centro);
            if (p->uscita) qui.push_back(p->uscita->centro);
            if (p->pim) qui.insert(qui.end(), p->pim->punti.begin(), p->pim->punti.end());
            for (const Vec2 q : qui) {
                CAPTURE(q.x);
                CAPTURE(q.y);
                CHECK(m.profondita(q) > 15.0);
            }
        }
        if (p->condotta == PianoOriginale::Condotta::scorta) CHECK(p->pim);
        if (p->condotta == PianoOriginale::Condotta::pedinamento) CHECK(p->distanza_m > 0.0);
    }
    CHECK_FALSE(piano_originale(M688_DATI, "PROVA_S5_CONVOGLIO"));   // una missione che non e' originale: nessun piano
}

// TLAM-C (carico M2): chiama il gestore armi.attacco_terra solo se la missione lo dichiara, mai quello del «tomahawk»;
// il MISSILE del gestore non conta un secondo lancio. La 0A con la chiave aggiunta in una copia
TEST_CASE("TLAM-C: il gestore facoltativo armi.attacco_terra, senza doppio conteggio" * doctest::skip(!c_e())) {
    const std::string orig = std::string(M688_MISSIONI) + "/SCENE0A.lua";
    std::string testo;
    {
        std::FILE* f = std::fopen(orig.c_str(), "rb");
        REQUIRE(f);
        char buf[4096];
        for (std::size_t n; (n = std::fread(buf, 1, sizeof buf, f)) > 0;) testo.append(buf, n);
        std::fclose(f);
    }
    const std::string chiave = "tomahawk = { gestore = \"L_0724\" },";
    const auto k = testo.find(chiave);
    REQUIRE(k != std::string::npos);
    const fs::path copia = fs::temp_directory_path() / "m688_tlam_c_SCENE0A.lua";
    {
        std::string t = testo;
        t.insert(k + chiave.size(), " attacco_terra = { gestore = \"L_0724\" },");
        std::FILE* f = std::fopen(copia.string().c_str(), "wb");
        REQUIRE(f);
        std::fwrite(t.data(), 1, t.size(), f);
        std::fclose(f);
    }
    const auto lancia_tlam = [](Missione& m) {
        std::vector<std::string> tubi = m.tubi();
        tubi[0] = "attacco_terra";
        m.imposta_tubi(tubi);
        REQUIRE(m.applica({m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
        m.avanza(1.0);
        int lanci = 0;
        for (const auto& e : m.eventi_missione()) lanci += e.tipo == TipoEvento::lancio_giocatore;
        CHECK(lanci == 1);
        CHECK(m.esito_dettagliato().per_arma.at("tlam_c").lanci == 1);
        CHECK(m.esito_dettagliato().per_arma.count("attacco_terra") == 0);
    };
    SUBCASE("la 0A originale: solo l'evento, il gestore del «tomahawk» non parte") {
        Missione m(orig, 1);
        lancia_tlam(m);
        CHECK(m.variabili().at(4) == 4);
        CHECK(m.lanci_missili().empty());
    }
    SUBCASE("con armi.attacco_terra: il gestore conta il missile (variabile 4) e fa il MISSILE") {
        Missione m(copia.string(), 1);
        lancia_tlam(m);
        CHECK(m.variabili().at(4) == 3);
        CHECK(m.lanci_missili().size() == 1);
    }
    fs::remove(copia);
}

// Il traffico civile (docs/20 §3): con le navi del traffico attorno, ne' il comandante ne' l'IA tattica sparano a una
// nave del traffico senza ROE che lo consentano (l'errore d'identificazione ad A3 non vale per loro, missione_forze.cpp),
// e nessuno script la prende per bersaglio. Tre originali in guerra, due semi, 40 minuti
TEST_CASE("traffico: comandante e IA non sparano alle navi del traffico" * doctest::skip(!c_e())) {
    for (const std::string id : {"SCENE5A", "SCENE8A", "SCENE9A"})
        for (const std::uint64_t seme : {1ull, 2ull}) {
            Partita p(id, seme);
            p.m.usa_traffico(true);
            int navi = 0;
            for (int k = 0; k < 8 && !p.m.esito(); ++k) {
                p.avanza(300.0);
                for (const auto& [uid, u] : p.m.unita()) {
                    if (p.m.del_traffico(uid)) { ++navi; continue; }
                    // le armi no: la testa d'un missile puo' agganciare una nave del traffico che il lanciatore non conosceva
                    // (SCENE8A seme 2: l'Harpoon d'una Perry sull'Alfa e un peschereccio nella scatola), come per il
                    // giocatore (i neutrali nella scatola, docs/26). Quello che l'IA conosce lo tiene fuori missione_missili
                    if (u.flag & 0x20) continue;
                    CHECK_MESSAGE(!p.m.del_traffico(u.bersaglio), id, " seme ", seme, ": l'unita' ", uid, " punta la nave ", u.bersaglio);
                }
            }
            int lanci = 0;
            for (const auto& e : p.m.registro().eventi()) {
                const bool lancio = e.tipo == TipoEvento::lancio_giocatore || e.tipo == TipoEvento::lancio_nemico || e.tipo == TipoEvento::lancio_ia;
                if (!lancio) continue;
                ++lanci;
                CHECK_MESSAGE(!p.m.del_traffico(e.altra), id, " seme ", seme, ": lancio su una nave del traffico (", e.altra, ")");
            }
            MESSAGE(id, " seme ", seme, ": ", std::string(p.m.con_traffico() ? "traffico acceso" : "senza traffico"), ", ", navi, " navi-campione, ",
                    lanci, " lanci");
        }
}

TEST_CASE("DIAG originale" * doctest::skip(!std::getenv("DIAG_ORIGINALE"))) {
    const std::string id = std::getenv("DIAG_ORIGINALE");
    const std::uint64_t seme = std::getenv("DIAG_SEME") ? std::strtoull(std::getenv("DIAG_SEME"), nullptr, 10) : 1;
    const double durata = std::getenv("DIAG_S") ? std::atof(std::getenv("DIAG_S")) : 3600.0;
    Partita p(id, seme);
    double ogni = std::getenv("DIAG_OGNI") ? std::atof(std::getenv("DIAG_OGNI")) : 300.0, prossimo = 0.0;
    while (p.m.tempo() < durata - 0.5 && !p.m.esito()) {   // meno di mezzo secondo: avanza non farebbe nessun passo
        p.avanza(std::max(1.0, std::min(ogni, durata - p.m.tempo())));
        if (const auto* g = p.m.giocatore(); g && p.m.tempo() >= prossimo) {
            prossimo += ogni;
            std::printf("t %6.0f pos %.0f,%.0f q %.1f (%.1f) chiglia %.1f fondo %.1f v %.1f (%.1f) r %.0f (%.0f) fase %s\n", p.m.tempo(), g->pos.x, g->pos.y,
                        g->quota, g->quota_ordinata, p.m.chiglia_vera(*g), p.m.profondita(g->pos), g->velocita, g->velocita_ordinata, g->rotta,
                        g->rotta_ordinata, nome_fase_comandante(p.comandante->stato().fase));
        }
    }
    if (std::getenv("DIAG_ORDINI"))   // i primi ordini del comandante
        for (std::size_t k = 0; k < std::min<std::size_t>(p.ordini.storico().size(), std::atoi(std::getenv("DIAG_ORDINI"))); ++k) {
            const Ordine& o = p.ordini.storico()[k];
            std::printf("ordine %.0f %s a %.2f b %.2f n %d\n", o.tempo, std::string(nome_ordine(o.tipo)).c_str(), o.a, o.b, o.n);
        }
    if (std::getenv("DIAG_ARMI")) {   // i tubi e i contatti alla fine
        const auto& a = p.st->armi();
        for (int k = 1; k <= a.numero_tubi(); ++k)
            std::printf("tubo %d %s stato %d\n", k, std::string(nome_arma(a.arma(k))).c_str(), static_cast<int>(a.stato(k)));
        for (const auto& s : segni_contatti(p.m, false)) {
            const BarraBersaglio b = barra_bersaglio(p.m, s.id);
            std::printf("contatto %d %s ril %.0f traccia %d\n", s.id, b.classe.c_str(), s.rilevamento, s.traccia() ? 1 : 0);
        }
    }
    for (const auto& [u, metri] : p.m.spostamenti()) std::printf("spostata all'avvio: %d di %.0f m\n", u, metri);
    std::printf("storia:");
    for (const auto& [t, s] : p.comandante->storia()) std::printf(" %.0f %s(%s)", t, nome_fase_comandante(s.fase), s.motivo.c_str());
    std::printf("\nRIASSUNTO %s seme %llu: esito %d, t %.0f, ordini %zu\n", id.c_str(), static_cast<unsigned long long>(seme),
                p.m.esito() ? *p.m.esito() : -1, p.m.tempo(), p.ordini.storico().size());
}

// La collocazione all'avvio (Missione::colloca) del battello del giocatore: la profondita' della partenza, l'acqua
// comoda e minima, la componente delle acque aperte. DIAG_COLLOCA=SCENE0A, DIAG_SEMI=8
TEST_CASE("DIAG colloca" * doctest::skip(!std::getenv("DIAG_COLLOCA"))) {
    const std::string scelta = std::getenv("DIAG_COLLOCA");   // "tutte": le 22, una riga per seme
    const int semi = std::getenv("DIAG_SEMI") ? std::atoi(std::getenv("DIAG_SEMI")) : 8;
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    std::map<std::string, double> pescaggi;
    for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
    for (const std::string& id : scelta == "tutte" ? originali() : std::vector<std::string>{scelta})
    for (int s = 1; s <= semi; ++s) {
        Missione prima(std::string(M688_MISSIONI) + "/" + id + ".lua", static_cast<std::uint64_t>(s));
        const Vec2 p0 = prima.giocatore()->pos;
        Missione m(std::string(M688_MISSIONI) + "/" + id + ".lua", static_cast<std::uint64_t>(s));
        const auto f = fondale(m.teatro());
        REQUIRE(f);
        m.imposta_fondale(f, pescaggi);
        const auto* g = m.giocatore();
        const double pc = m.pescaggio(*g);
        int c = 0, r = 0;
        f->cella(p0, c, r);
        std::printf("SPOSTAMENTO %s %d %.0f\n", id.c_str(), s, distanza(p0, g->pos));
        if (scelta == "tutte") continue;
        std::printf("seme %d: partenza %.0f,%.0f fondo %.1f m, pescaggio %.1f (minima %.1f, comoda %.1f); dopo %.0f,%.0f, %.0f m piu' in la'\n",
                    s, p0.x, p0.y, f->profondita(p0), pc, pc + 0.5, pc + 10.0, g->pos.x, g->pos.y, distanza(p0, g->pos));
        for (const double soglia : {pc + 0.5, pc + 10.0}) {
            const AcqueNavigabili a(*f, soglia);
            const int k = a.componente(c, r);
            std::printf("  acqua >= %.1f m: componente %d, %d celle\n", soglia, k, k >= 0 ? a.celle(k) : 0);
        }
    }
}
