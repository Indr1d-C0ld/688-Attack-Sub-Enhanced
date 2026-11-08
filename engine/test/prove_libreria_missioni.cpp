#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/mine.hpp"
#include "nucleo/elettronica.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/radio.hpp"
#include "nucleo/siluro.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Libreria di missioni pronte (data/missioni/libreria, docs/17_libreria_missioni.md): i file aggiornati con lo
// scrittore, i pacchetti che si importano, 30 minuti nei due livelli, e per ogni missione la vittoria e la sconfitta
// preparate (giocatore spostato con il comando «posizione», il punto di rotta del kit)

namespace {

const std::string DATI = M688_DATI;
const fs::path LIBRERIA = fs::path(M688_MISSIONI) / "libreria";
bool c_e() { return fs::exists(LIBRERIA / "LIB_I08_OMBRA_LUNGA.lua") && fs::exists(DATI + "/missioni_rapide.toml"); }

std::string leggi(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

const DatiScenario& dati() {
    static const DatiScenario d = carica_dati_scenario(DATI);
    return d;
}

ScenarioMissione scenario(const std::string& id) { return *leggi_scenario(leggi(LIBRERIA / (id + ".lua"))); }

std::string scrivi(const ScenarioMissione& s, const std::string& nome) {
    const auto f = fs::temp_directory_path() / (nome + ".lua");
    std::ofstream(f, std::ios::binary) << scrivi_missione_lua(s, dati());
    return f.string();
}

const ZonaScenario& zona(const ScenarioMissione& s, const std::string& id) {
    for (const auto& z : s.zone)
        if (z.id == id) return z;
    throw std::logic_error("zona " + id);
}

void avanza_fino(Missione& m, double t) {
    while (m.tempo() < t && !m.esito()) m.avanza(2.0);
}

const Obiettivo* obiettivo(const EsitoDettagliato& e, const std::string& id) {
    for (const auto& o : e.obiettivi)
        if (o.id == id) return &o;
    return nullptr;
}

struct Enhanced {
    std::map<std::string, ClasseNave> classi = carica_classi(DATI + "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    Enhanced(const std::string& file, std::uint64_t seme)
        : m(file, seme), dm(classi, DATI, seme, RegolaDanni::simulazione), ds(classi, DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    }
    void avanza(double s) {
        constexpr double DT = 1.0 / 9.0;
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(DT);
            dm.avanza(m, DT);
            ds.avanza(m, DT);
            m.raccogli_danni(&ds, &dm);
        }
    }
};

// le unita' dell'IA che non vedono e non sparano (le vittorie preparate: conta la logica della missione, non la caccia)
// le unita' dell'IA sorde: cupola e schiera spente (Missione::sonar_ia). Coi soli ciechi() un neutrale sente e classifica
// come tutti, dal 31/10/2026 (simmetria, punto 1: il sonarista dell'IA senza filtro sulla parte vera)
void sordi(Missione& m) {
    for (const auto& [id, u] : m.unita())
        if (!u.giocatore) m.sonar_ia(id, false, false);
}

void ciechi(ScenarioMissione& t) {
    t.aerei_asw.clear();
    for (auto& u : t.unita)
        if (!u.giocatore) { u.parte = "neutrale"; u.flag &= ~0x18; u.comportamento = ""; }
}

int quanti(const Missione& m, TipoEvento t) {
    return static_cast<int>(std::count_if(m.eventi_missione().begin(), m.eventi_missione().end(), [t](const EventoMissione& e) { return e.tipo == t; }));
}

}  // namespace

TEST_CASE("libreria: i file sono aggiornati, i pacchetti si importano, 30 minuti nei due livelli" * doctest::skip(!c_e())) {
    int n = 0;
    for (const auto& e : fs::directory_iterator(LIBRERIA)) {
        if (e.path().extension() != ".lua") continue;
        ++n;
        const std::string id = e.path().stem().string();
        CAPTURE(id);
        const std::string testo = leggi(e.path());
        const auto s = leggi_scenario(testo);
        REQUIRE(s);
        CHECK(scrivi_missione_lua(*s, dati()) == testo);   // rifare la libreria (strumento libreria_missioni) non cambia niente
        for (const auto& a : verifica(*s, Fondale::carica(DATI + "/teatri/" + s->teatro + ".fdl"), dati()))
            CHECK_MESSAGE(a.gravita != AvvisoScenario::Gravita::errore, a.testo);
        const auto pacchetto = LIBRERIA / (id + ".m688m");
        REQUIRE(fs::exists(pacchetto));
        const auto dest = fs::temp_directory_path() / "m688_prova_libreria";
        const auto imp = importa_pacchetto(leggi(pacchetto), dati(), DATI, dest);
        CHECK(imp.importato);
        CHECK(imp.problemi.empty());
        fs::remove_all(dest);
        Missione m(e.path().string(), 1);
        for (int i = 0; i < 900; ++i) m.avanza(2.0);
        CHECK(m.errori().empty());
        CHECK(m.non_implementate().empty());
        Enhanced x(e.path().string(), 1);
        x.avanza(1800.0);
        CHECK(x.m.errori().empty());
        CHECK(x.m.non_implementate().empty());
    }
    CHECK(n >= 2);
}

TEST_CASE("libreria I08 «Ombra lunga»: vittoria inosservati, sconfitta se scoperti" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_I08_OMBRA_LUNGA");
    const auto& rientro = zona(s, "rientro").cerchio;
    const Vec2 nimitz0 = s.unita.at(1).pos;
    const double rotta_gruppo = s.unita.at(1).rotta;
    // il giocatore fermo davanti al gruppo, sulla sua rotta e con la prua verso di lui: il gruppo gli passa sopra
    const auto davanti = [&](ScenarioMissione& t, double metri) {
        t.unita[0].pos = {nimitz0.x + metri * std::sin(radianti(rotta_gruppo)), nimitz0.y + metri * std::cos(radianti(rotta_gruppo))};
        t.unita[0].rotta = normalizza_360(rotta_gruppo + 180.0);
        t.unita[0].velocita = 0.0;
    };
    {   // vittoria: un gruppo che non vede (neutrale, senza ASW); classificata la Nimitz, poi al rientro
        ScenarioMissione t = s;
        t.aerei_asw.clear();
        for (auto& u : t.unita)
            if (!u.giocatore) { u.parte = "neutrale"; u.flag &= ~0x18; u.comportamento = ""; }
        davanti(t, 4000.0);
        Missione m(scrivi(t, "lib_i08_vittoria"), 1);
        avanza_fino(m, 2800.0);   // classificata, e dopo l'allarme dei 45 minuti
        REQUIRE(m.comando("posizione", {rientro.x, rientro.y}));
        avanza_fino(m, 3000.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        REQUIRE(obiettivo(e, "portaerei"));
        CHECK(obiettivo(e, "portaerei")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: incontro al gruppo a 30 nodi (fermo, nel classico, non lo sente nessuno): scoperto, poi al rientro
        ScenarioMissione t = s;
        davanti(t, 12000.0);
        t.unita[0].velocita = 30.0;
        Missione m(scrivi(t, "lib_i08_sconfitta"), 1);
        avanza_fino(m, 2800.0);
        REQUIRE(m.comando("posizione", {rientro.x, rientro.y}));
        avanza_fino(m, 3000.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        REQUIRE(obiettivo(e, "occulto"));
        CHECK(obiettivo(e, "occulto")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria I10 «Corsa al relitto»: col presidio la nave devia, senza arriva sul relitto" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_I10_CORSA_AL_RELITTO");
    const auto& relitto = zona(s, "relitto").cerchio;
    {   // vittoria: l'Alfa sul relitto da subito; la nave devia, il soccorso arriva, esito 40
        Missione m(scrivi(s, "lib_i10_vittoria"), 1);
        m.avanza(2.0);
        REQUIRE(m.comando("posizione", {relitto.x + 1000.0, relitto.y}));
        avanza_fino(m, 9400.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        REQUIRE(m.unita().count(4));
        CHECK(distanza(m.unita().at(4).pos, {relitto.x, relitto.y}) > relitto.r);   // la nave non e' sul relitto
        CHECK(m.errori().empty());
    }
    {   // sconfitta: l'Alfa lontano; la nave arriva sul relitto, esito 41 (fallimento)
        Missione m(scrivi(s, "lib_i10_sconfitta"), 1);
        m.avanza(2.0);
        REQUIRE(m.comando("posizione", {relitto.x + 60000.0, relitto.y}));
        avanza_fino(m, 9400.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

// --- storiche (docs/17 §2b) ---

TEST_CASE("libreria S01 «Vestfjord»: l'arancione colpito bonifica il fiordo, la portaerei all'imboccatura chiude male" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S01_VESTFJORD");
    {   // vittoria: il Los Angeles fermo davanti all'arancione (che qui corre a 12 nodi e si fa sentire), classificato e
        // affondato prima che il gruppo entri
        ScenarioMissione t = s;
        t.unita[1].velocita = 12.0;
        t.unita[1].comportamento = "";
        t.unita[0].pos = {t.unita[1].punti[0].x + 2500.0, t.unita[1].punti[0].y};
        t.unita[0].rotta = 270.0;
        t.unita[0].velocita = 0.0;
        Missione m(scrivi(t, "lib_s01_vittoria"), 1);
        const auto classificato = [&] { const auto* o = obiettivo(m.esito_dettagliato(), "classificato"); return o && o->stato == Obiettivo::Stato::raggiunto; };
        while (m.tempo() < 2400.0 && !classificato()) m.avanza(2.0);
        REQUIRE(classificato());
        m.distruggi(2);
        avanza_fino(m, 2600.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        REQUIRE(obiettivo(e, "arancione"));
        CHECK(obiettivo(e, "arancione")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: la portaerei anticipa, entra e arriva all'imboccatura con l'arancione vivo
        Missione m(scrivi(s, "lib_s01_sconfitta"), 1);
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.tempo() > 2700.0);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S02 «Nave mineraria»: zitti si vince, scoperti arriva il Victor" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S02_NAVE_MINERARIA");
    {   // vittoria: il Los Angeles fermo e la Krivak che non vede (il sonar del classico, come l'originale, sente anche un
        // battello fermo: il suo rumore non scende mai a zero); a 1 h 50' il braccio e' a bordo
        ScenarioMissione t = s;
        t.unita[0].velocita = 0.0;
        t.unita[4].parte = "neutrale";
        t.unita[4].flag &= ~0x18;
        t.unita[4].comportamento = "";
        Missione m(scrivi(t, "lib_s02_vittoria"), 1);
        avanza_fino(m, 6700.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(m.unita().at(4).pos.x != t.unita[3].pos.x);   // il rimorchiatore e' entrato
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "occulto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: a 30 nodi incontro alla Krivak appena entrata (prua contro prua): l'allarme fa entrare il Victor
        ScenarioMissione t = s;
        const Vec2 krivak = t.unita[4].pos;
        t.unita[0].rotta = normalizza_360(t.unita[4].rotta + 180.0);
        t.unita[0].velocita = 30.0;
        Missione m(scrivi(t, "lib_s02_sconfitta"), 1);
        avanza_fino(m, 2402.0);
        REQUIRE(m.comando("posizione", {krivak.x + 6000.0 * std::sin(radianti(t.unita[4].rotta)), krivak.y + 6000.0 * std::cos(radianti(t.unita[4].rotta))}));
        avanza_fino(m, 6700.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(m.variabili().count(11));
        CHECK(distanza(m.unita().at(6).pos, t.unita[1].pos) < 50000.0);   // il Victor e' entrato
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "occulto")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S04 «Campane d'edera»: due ore sul cavo poi fuori; senza il lavoro l'uscita non chiude" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S04_CAMPANE_D_EDERA");
    ciechi(s);
    s.unita[0].velocita = 0.0;
    const auto& cavo = zona(s, "cavo").cerchio;
    const auto& uscita = zona(s, "uscita").cerchio;
    {   // vittoria
        Missione m(scrivi(s, "lib_s04_vittoria"), 1);
        m.avanza(2.0);
        REQUIRE(m.comando("posizione", {cavo.x, cavo.y}));
        avanza_fino(m, 7300.0);
        CHECK(m.variabili().at(300) >= 720);
        CHECK(m.variabili().at(301) == 1);
        CHECK(distanza(m.unita().at(5).pos, s.unita[4].pos) < 30000.0);   // la nave sul cavo e' entrata
        REQUIRE(!m.esito());
        REQUIRE(m.comando("posizione", {uscita.x, uscita.y}));
        avanza_fino(m, 7400.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "lavoro")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "uscita")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: un'ora sola sul cavo, poi fuori: niente chiusura, e a tempo scaduto il lavoro manca
        Missione m(scrivi(s, "lib_s04_sconfitta"), 1);
        m.avanza(2.0);
        REQUIRE(m.comando("posizione", {cavo.x, cavo.y}));
        avanza_fino(m, 3700.0);
        REQUIRE(m.comando("posizione", {uscita.x, uscita.y}));
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "lavoro")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

// «Tappo» nell'Enhanced: il Los Angeles tre chilometri a nord del campo, sei Mk 67 verso sud con corse diverse
struct PosaTappo {
    ScenarioMissione s;
    Enhanced x;
    CatalogoCarico cat = carica_carico(DATI + "/carico.toml");
    StazioniMissione st;
    RegistroOrdini ordini;
    static ScenarioMissione preparato() {
        ScenarioMissione t = scenario("LIB_S07_TAPPO");
        ciechi(t);
        const auto campo = t.obiettivi.at(0).cerchio.value();
        t.unita[0].pos = {campo.x, campo.y + 3000.0};
        t.unita[0].quota = 20.0;
        t.unita[0].velocita = 0.0;
        return t;
    }
    PosaTappo() : s(preparato()), x(scrivi(s, "lib_s07_enhanced"), 1), st((x.m.siluri_enhanced(DATI), x.m)) {}
    void avanza(double secondi) {
        constexpr double DT = 1.0 / 9.0;
        for (long i = 0; i < std::lround(secondi * 9.0); ++i) {
            for (const auto& o : ordini.consegna(x.m.tempo())) st.consegna(x.m, o);
            x.m.avanza(DT);
            x.dm.avanza(x.m, DT);
            x.ds.avanza(x.m, DT);
            x.m.raccogli_danni(&x.ds, &x.dm);
            st.dopo_passo(x.m);
        }
    }
    void posa(int tubi, double corsa0) {   // carica `tubi` Mk 67 e le posa una per tubo
        SalaSiluri sala(cat, "los_angeles", x.m.limiti_carico());
        for (int k = 1; k <= tubi; ++k) REQUIRE(sala.metti(Posto::tubo, "mk67", k).fatto);
        st.imposta_carico(x.m, sala.carico(), cat);
        for (int k = 1; k <= tubi; ++k) {
            ordini.ordina({x.m.tempo(), TipoOrdine::imposta_siluro, 180.0, static_cast<double>(CampoSiluro::rotta), k});
            ordini.ordina({x.m.tempo(), TipoOrdine::imposta_siluro, corsa0 + 250.0 * k, static_cast<double>(CampoSiluro::rte), k});
            ordini.ordina({x.m.tempo(), TipoOrdine::carica_tubo, 0, 0, k});
            avanza(1.0);
            if (st.armi().stato(k) == StatoTubo::in_armamento) st.armi().messaggio_missione(9 + k);
            REQUIRE(st.armi().stato(k) == StatoTubo::armato);
            ordini.ordina({x.m.tempo(), TipoOrdine::lancia, 0.0, 0.0, k});
            avanza(2.0);
        }
    }
    void campo_posato() {
        avanza(1.0);
        posa(4, 2000.0);
        posa(2, 3000.0);
        avanza(600.0);   // 3,5 km a 20 nodi
    }
};

TEST_CASE("libreria S07 «Tappo»: sei Mk 67 nel campo e via (Enhanced); senza mine la ritirata non basta" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S07_TAPPO");
    const auto& ritirata = zona(s, "ritirata").cerchio;
    {   // sconfitta (classico, dove le mine non ci sono): al punto di ritirata senza il campo
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_s07_sconfitta"), 1);
        avanza_fino(m, 1900.0);
        REQUIRE(m.comando("posizione", {ritirata.x, ritirata.y}));
        avanza_fino(m, 2000.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "campo")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // vittoria (Enhanced): il campo posato non chiude la missione, la ritirata si'
        PosaTappo p;
        REQUIRE(Fondale::carica(DATI + "/teatri/barents.fdl").profondita(p.s.unita[0].pos) > 30.0);
        p.campo_posato();
        CHECK(quanti(p.x.m, TipoEvento::mina_posata) == 6);
        REQUIRE(!p.x.m.esito());
        REQUIRE(p.x.m.comando("posizione", {ritirata.x, ritirata.y}));
        while (p.x.m.tempo() < 2000.0 && !p.x.m.esito()) p.avanza(10.0);
        REQUIRE(p.x.m.esito());
        CHECK(*p.x.m.esito() == 40);
        const auto e = p.x.m.esito_dettagliato();
        REQUIRE(obiettivo(e, "campo"));
        CHECK(obiettivo(e, "campo")->fatti == 6);
        CHECK(obiettivo(e, "campo")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(p.x.m.errori().empty());
    }
}

TEST_CASE("libreria S08 «Valona»: il Foxtrot convinto esce con gli altri; lasciato solo resta nella baia" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S08_VALONA");
    const auto& ripensamento = zona(s, "ripensamento").cerchio;
    {   // vittoria: dopo la svolta l'Alfa raggiunge il Foxtrot che torna indietro
        Missione m(scrivi(s, "lib_s08_vittoria"), 1);
        avanza_fino(m, 1300.0);
        REQUIRE(m.comando("posizione", {ripensamento.x, ripensamento.y}));
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "fuori")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: l'Alfa lontano; quattro su cinque fuori, il tempo scade
        Missione m(scrivi(s, "lib_s08_sconfitta"), 1);
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        CHECK(m.variabili().at(300) == 4);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "fuori")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S09 «Aport»: nell'area senza fix, o scoperti dalla fregata" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S09_APORT");
    const auto& area = zona(s, "area").cerchio;
    {   // vittoria
        Missione m(scrivi(s, "lib_s09_vittoria"), 1);
        avanza_fino(m, 60.0);
        REQUIRE(m.comando("posizione", {area.x, area.y}));
        avanza_fino(m, 200.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "occulto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: a 30 nodi incontro alla fregata della schiera, poi nell'area
        ScenarioMissione t = s;
        const Vec2 perry = t.unita[2].pos;
        t.unita[0].pos = {perry.x + 8000.0, perry.y};
        t.unita[0].rotta = 270.0;
        t.unita[0].velocita = 30.0;
        Missione m(scrivi(t, "lib_s09_sconfitta"), 1);
        avanza_fino(m, 600.0);
        REQUIRE(m.comando("posizione", {area.x, area.y}));
        avanza_fino(m, 800.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "occulto")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S11 «Tenete il confine»: il Foxtrot rimorchiato arriva all'incontro; oltre la linea e' finita" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S11_TENETE_IL_CONFINE");
    s.unita[0].velocita = 0.0;
    {   // vittoria: l'Alfa fermo fuori dalle 12 miglia
        Missione m(scrivi(s, "lib_s11_vittoria"), 1);
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "a_casa")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // Enhanced (M6): incagliato fino a 20 minuti, poi a rimorchio del rimorchiatore fino all'incontro
        Enhanced x(scrivi(s, "lib_s11_enhanced"), 1);
        x.avanza(600.0);
        CHECK(x.m.incagliata_ia(2));
        const Vec2 scogli = x.m.unita().at(2).pos;
        x.avanza(700.0);
        CHECK(x.m.rimorchiatore_di(2) == 9);
        CHECK(quanti(x.m, TipoEvento::rimorchio) >= 1);
        while (x.m.tempo() < s.durata_s && !x.m.esito()) x.avanza(30.0);
        CHECK(distanza(x.m.unita().at(2).pos, scogli) > 10000.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(x.m.errori().empty());
    }
    {   // sconfitta: dentro le acque territoriali
        const auto& acque = zona(s, "acque").cerchio;
        Missione m(scrivi(s, "lib_s11_sconfitta"), 1);
        avanza_fino(m, 100.0);
        REQUIRE(m.comando("posizione", {acque.x, acque.y - acque.r * 0.8}));
        avanza_fino(m, 200.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S12 «Il gruppo cacciatore»: una fregata affondata e fuori; senza, il tempo scade" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_S12_GRUPPO_CACCIATORE");
    const auto& uscita = zona(s, "uscita").cerchio;
    {   // vittoria: l'Alfa gia' all'uscita, la fregata lenta affondata
        Missione m(scrivi(s, "lib_s12_vittoria"), 1);
        avanza_fino(m, 60.0);
        REQUIRE(m.comando("posizione", {uscita.x, uscita.y}));
        avanza_fino(m, 1300.0);
        REQUIRE(!m.esito());
        m.distruggi(3);
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "fregate")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: nessuna fregata affondata
        Missione m(scrivi(s, "lib_s12_sconfitta"), 1);
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        CHECK(m.esito_dettagliato().categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

// --- inedite sbloccate dalle rapide v2 ---

// il giocatore fermo dietro l'unita' `id` (che gli sta a prua, a `metri`): lo riporta li' ogni 30 s per `secondi`
static void tieni_a_prua(Missione& m, int id, double metri, double secondi) {
    const double fino = m.tempo() + secondi;
    while (m.tempo() < fino && !m.esito() && m.unita().count(id)) {
        const auto& g = *m.giocatore();
        const Vec2 b = m.unita().at(id).pos;
        REQUIRE(m.comando("posizione", {b.x - metri * std::sin(radianti(g.rotta)), b.y - metri * std::cos(radianti(g.rotta))}));
        for (int i = 0; i < 15 && !m.esito(); ++i) m.avanza(2.0);
    }
}

TEST_CASE("libreria I01 «La balena di ferro»: 40 minuti sulla balena e il pedinatore seminato; lontani, niente" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I01_BALENA_DI_FERRO");
    ciechi(s);   // l'Akula che non vede: si semina da solo
    s.unita[0].velocita = 0.0;
    {   // vittoria
        Missione m(scrivi(s, "lib_i01_vittoria"), 1);
        m.avanza(2.0);
        tieni_a_prua(m, 2, 2500.0, 3000.0);
        CHECK(m.variabili().count(300));   // la svolta e' scattata (a 15 minuti di contatto o a 50)
        avanza_fino(m, 8000.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "balena")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "ascolto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "seminato")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: fermi e lontani, nessun contatto
        Missione m(scrivi(s, "lib_i01_sconfitta"), 1);
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "ascolto")->stato != Obiettivo::Stato::raggiunto);
        CHECK(e.categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria I02 «Il pescatore di Teriberka»: il Typhoon nel golfo e lo straniero classificato; il Typhoon perso" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I02_PESCATORE_DI_TERIBERKA");
    ciechi(s);
    s.unita[0].velocita = 0.0;
    {   // vittoria: dieci minuti davanti al Los Angeles, poi il Typhoon arriva da solo
        Missione m(scrivi(s, "lib_i02_vittoria"), 1);
        m.avanza(2.0);
        tieni_a_prua(m, 4, 2000.0, 900.0);
        CHECK(obiettivo(m.esito_dettagliato(), "straniero")->stato == Obiettivo::Stato::raggiunto);
        REQUIRE(m.comando("posizione", {s.unita[0].pos.x, s.unita[0].pos.y}));
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: il Typhoon affondato
        Missione m(scrivi(s, "lib_i02_sconfitta"), 1);
        avanza_fino(m, 600.0);
        m.distruggi(2);
        avanza_fino(m, 1500.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria I02: se il Los Angeles tiene il Typhoon, il Typhoon accosta verso il golfo a 12 nodi" * doctest::skip(!c_e())) {
    const ScenarioMissione s = scenario("LIB_I02_PESCATORE_DI_TERIBERKA");
    const std::string testo = leggi(LIBRERIA / "LIB_I02_PESCATORE_DI_TERIBERKA.lua");
    CHECK(testo.find("su_unita(u, 4, m688.contatto_da, 2, 2)") != std::string::npos);
    Missione m(scrivi(s, "lib_i02_accosta"), 1);
    // il Typhoon passa davanti al Los Angeles: prima o poi lo sente (la variabile della regola c'e' da subito, a 0)
    const auto scattata = [&] { return m.variabili().count(200) && m.variabili().at(200) == 1; };
    for (int i = 0; i < 2700 && !scattata() && !m.esito(); ++i) m.avanza(2.0);
    if (scattata()) {
        for (int i = 0; i < 60; ++i) m.avanza(2.0);
        CHECK(m.unita().at(2).velocita > 10.5);
    } else {
        MESSAGE("il Los Angeles non ha tenuto il Typhoon per due minuti: accostata non provata");
    }
    CHECK(m.errori().empty());
}

TEST_CASE("libreria I06 «Il ponte di Ekaterina»: due intrusi affondati; il primo nel bastione chiude male" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I06_PONTE_DI_EKATERINA");
    ciechi(s);
    s.unita[0].velocita = 0.0;
    {   // vittoria: il primo affondato, entra il secondo, affondato anche lui; a 90 minuti il gruppo della Nimitz
        Missione m(scrivi(s, "lib_i06_vittoria"), 1);
        avanza_fino(m, 600.0);
        m.distruggi(2);
        avanza_fino(m, 1200.0);
        REQUIRE(m.variabili().count(300));
        CHECK(distanza(m.unita().at(3).pos, s.unita[2].pos) < 5000.0);   // il secondo e' entrato
        m.distruggi(3);
        avanza_fino(m, 5600.0);
        CHECK(distanza(m.unita().at(6).pos, s.unita[5].pos) < 20000.0);   // la Nimitz e' entrata
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "intrusi")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: nessuno lo ferma, il primo Los Angeles arriva nel bastione
        Missione m(scrivi(s, "lib_i06_sconfitta"), 1);
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria I09 «Sotto il bastione»: la nave nuova classificata e fuori; sopra lo strato scatta la barriera" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I09_SOTTO_IL_BASTIONE");
    const auto& uscita = zona(s, "uscita").cerchio;
    {   // vittoria
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 0.0;
        Missione m(scrivi(t, "lib_i09_vittoria"), 1);
        m.avanza(2.0);
        tieni_a_prua(m, 2, 2500.0, 1500.0);
        REQUIRE(obiettivo(m.esito_dettagliato(), "foto")->stato == Obiettivo::Stato::raggiunto);
        REQUIRE(m.comando("posizione", {uscita.x, uscita.y}));
        avanza_fino(m, m.tempo() + 100.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "fuori")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
    {   // il sonar nuovo: a 30 m nelle trenta miglia, dopo 40 minuti, allarme e barriera
        ScenarioMissione t = s;
        t.unita[0].quota = 30.0;
        t.unita[0].velocita = 0.0;
        t.unita[0].pos = {zona(s, "portata").cerchio.x, zona(s, "portata").cerchio.y + 20000.0};
        Missione m(scrivi(t, "lib_i09_sonar"), 1);
        avanza_fino(m, 2300.0);
        CHECK_FALSE(m.variabili().count(300));
        avanza_fino(m, 2500.0);
        CHECK(m.variabili().count(11));
        CHECK(m.variabili().count(300));
        CHECK(distanza(m.unita().at(5).pos, t.unita[4].pos) < 10000.0);   // la barriera esce
        CHECK(m.errori().empty());
    }
    {   // sconfitta: nessuna foto, il tempo scade
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 0.0;
        Missione m(scrivi(t, "lib_i09_sconfitta"), 1);
        avanza_fino(m, s.durata_s + 10.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        CHECK(m.esito_dettagliato().categoria != EsitoDettagliato::Categoria::successo);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria: l'Alfa ha due esche semoventi MG-74 nel deposito, il 688 del 1989 nessuna MOSS" * doctest::skip(!c_e())) {
    const CatalogoCarico cat = carica_carico(DATI + "/carico.toml");
    int alfa = 0, l688 = 0;
    for (const auto& e : fs::directory_iterator(LIBRERIA)) {
        if (e.path().extension() != ".lua") continue;
        const ScenarioMissione s = *leggi_scenario(leggi(e.path()));
        CAPTURE(s.id);
        Missione m(e.path().string(), 1);
        SalaSiluri sala(cat, s.alfa ? "alfa" : "los_angeles", m.limiti_carico());
        if (s.alfa) {
            ++alfa;
            const auto mg74 = sala.disponibile("mg74");
            CHECK_FALSE(mg74.negata);
            CHECK(mg74.deposito == 2);
        } else {
            ++l688;
            const auto moss = sala.disponibile("moss_mk70");
            CHECK((moss.negata || moss.deposito == 0));
        }
    }
    CHECK(alfa >= 8);
    CHECK(l688 >= 6);
}

// --- E6: radio e meteo fissato nella libreria ---

TEST_CASE("libreria S05 «Il guardiano del cavo»: il ping trova l'intruso, che fugge allo stretto; la nave persa chiude male" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S05_GUARDIANO_DEL_CAVO");
    s.unita[0].velocita = 0.0;
    const Vec2 la = s.unita[3].pos;
    {   // vittoria: a un chilometro e mezzo dall'intruso, un ping; poi lo si ascolta mentre scappa
        Missione m(scrivi(s, "lib_s05_vittoria"), 1);
        m.radio_enhanced();
        m.avanza(2.0);
        REQUIRE(m.comando("posizione", {la.x, la.y - 1500.0}));
        avanza_fino(m, 60.0);
        REQUIRE(m.applica(Ordine{m.tempo(), TipoOrdine::impulso_sonar}));
        avanza_fino(m, 120.0);
        CHECK(m.unita().at(4).velocita > 5.0);   // fugge
        CHECK(m.radio()->in_coda("CACCIA"));      // e la radio cambia gli ordini
        tieni_a_prua(m, 4, 2000.0, 900.0);
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        const auto e = m.esito_dettagliato();
        CHECK(obiettivo(e, "cacciato")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "nave")->stato == Obiettivo::Stato::raggiunto);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: la nave di soccorso affondata
        Missione m(scrivi(s, "lib_s05_sconfitta"), 1);
        avanza_fino(m, 300.0);
        m.distruggi(2);
        avanza_fino(m, 1200.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria I02 e S01: la radio della missione (ordine falso, ordine vero con le ROE, la svolta di S01)" * doctest::skip(!c_e())) {
    {
        Missione m((LIBRERIA / "LIB_I02_PESCATORE_DI_TERIBERKA.lua").string(), 1);
        m.radio_enhanced();
        REQUIRE(m.radio());
        CHECK(m.radio()->ordini().front()->messaggio.oggetto == "RYBAK");
        CHECK(m.radio()->roe_trasmesse(0.0)[CategoriaRoe::sottomarini] == StatoRoe::trattenuto);
        avanza_fino(m, 4300.0);
        std::set<std::string> in_onda;
        for (const auto& v : m.radio()->debriefing().trasmessi) in_onda.insert(v.messaggio.id);
        CHECK((in_onda.count("FALSO") || m.radio()->in_coda("FALSO")));
        CHECK((in_onda.count("VERO") || m.radio()->in_coda("VERO")));
        while (m.tempo() < 6300.0) m.avanza(2.0);   // anche dopo l'esito: il Typhoon puo' essere gia' nel golfo
        double vero = -1.0;
        for (const auto& v : m.radio()->debriefing().trasmessi)
            if (v.messaggio.id == "VERO") vero = v.trasmesso;
        REQUIRE(vero > 0.0);
        CHECK(m.radio()->roe_trasmesse(vero + 1.0)[CategoriaRoe::sottomarini] == StatoRoe::libero);
        CHECK(m.errori().empty());
    }
    {
        Missione m((LIBRERIA / "LIB_S01_VESTFJORD.lua").string(), 1);
        m.radio_enhanced();
        avanza_fino(m, 2600.0);
        CHECK_FALSE(m.radio()->in_coda("ANTICIPO"));
        avanza_fino(m, 2760.0);
        CHECK(m.radio()->in_coda("ANTICIPO"));
        CHECK(m.errori().empty());
    }
}

// --- sbloccate dalle mine dell'IA, dalla batteria dei diesel e dal cavo della schiera (26/10/2026) ---

namespace {

Vec2 polare_prova(Vec2 c, double rotta, double d) { return {c.x + d * std::sin(radianti(rotta)), c.y + d * std::cos(radianti(rotta))}; }

// l'Enhanced con le mine (siluri dell'Enhanced) e la radio; il giocatore portato a `metri` dietro l'unita' `id`, sulla
// sua rotta, ogni 30 s; con `ping` un impulso del sonar attivo ogni minuto (la caccia che non lascia ricaricare)
struct EnhancedPieno : Enhanced {
    EnhancedPieno(const std::string& file, std::uint64_t seme) : Enhanced(file, seme) {
        m.siluri_enhanced(DATI);
        m.radio_enhanced();
    }
    void insegui(int id, double metri, double fino, bool ping) {
        double prossimo_ping = 0.0;
        while (m.tempo() < fino && !m.esito() && m.unita().count(id) && !m.unita().at(id).distrutta) {
            const auto& b = m.unita().at(id);
            REQUIRE(m.comando("posizione", {b.pos.x - metri * std::sin(radianti(b.rotta)), b.pos.y - metri * std::cos(radianti(b.rotta))}));
            if (ping && m.tempo() >= prossimo_ping) {
                REQUIRE(m.applica(Ordine{m.tempo(), TipoOrdine::impulso_sonar}));
                prossimo_ping = m.tempo() + 60.0;
            }
            avanza(30.0);
        }
    }
};

}  // namespace

TEST_CASE("libreria S03 «Quarantena»: il Foxtrot pingato senza tregua emerge per forza (Enhanced); nel classico a 2 ore e mezza" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S03_QUARANTENA");
    s.aerei_asw.clear();
    s.unita[0].velocita = 0.0;
    {   // Enhanced: la batteria al 6%, la caccia non lo lascia andare a snorkel: si trascina fino all'emersione forzata
        EnhancedPieno x(scrivi(s, "lib_s03_enhanced"), 1);
        x.avanza(1.0);
        REQUIRE(x.m.batteria(2) == doctest::Approx(0.06).epsilon(0.02));
        x.insegui(2, 1500.0, s.durata_s, true);
        CAPTURE(x.m.batteria(2));
        CHECK(x.m.emersione_forzata(2));
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(x.m.tempo() < 9000.0);   // prima della regola del classico
        CHECK(x.m.variabili()[300] == 1);
        const auto e = x.m.esito_dettagliato();
        CHECK(obiettivo(e, "emerso")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "non_sparare")->stato != Obiettivo::Stato::fallito);
        CHECK(x.m.errori().empty());
    }
    {   // Enhanced, perso: senza nessuno che lo tenga (le unita' americane neutrali e lontane, il giocatore lontano) va a
        // snorkel e ricarica. Un neutrale sentito puo' tenerlo giu' (teme di essere scoperto: decisione della principale,
        // 29/10/2026), quindi i neutrali vanno lontano anche loro
        ScenarioMissione t = s;
        for (auto& u : t.unita)
            if (!u.giocatore && u.parte == "usa") { u.parte = "neutrale"; u.flag &= ~0x18; u.comportamento = ""; }
        EnhancedPieno x(scrivi(t, "lib_s03_snorkel"), 1);
        x.avanza(1.0);
        const Vec2 lontano = polare_prova(x.m.unita().at(2).pos, 45.0, 60000.0);
        REQUIRE(x.m.comando("posizione", {lontano.x, lontano.y}));
        for (const auto& [id, u] : x.m.unita())   // gli ex americani a 80 km dal Foxtrot, oltre l'ascolto
            if (!u.giocatore && id != 2 && (u.flag & 0x18) == 0 && u.categoria != "arma")
                if (auto* v = x.m.trova(id)) { v->pos = polare_prova(x.m.unita().at(2).pos, 225.0, 80000.0); v->velocita = v->velocita_ordinata = 0.0; }
        x.avanza(1800.0);
        CHECK(x.m.snorkel(2));
        CHECK(x.m.batteria(2) > 0.06);
        CHECK_FALSE(x.m.emersione_forzata(2));
    }
    {   // classico: niente batteria, il Foxtrot «emerge» con la regola se il giocatore l'ha tenuto due ore
        Missione m(scrivi(s, "lib_s03_classico"), 1);
        m.avanza(2.0);
        CHECK(m.batteria(2) < 0.0);
        tieni_a_prua(m, 2, 2000.0, 9100.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 44);   // l'esito suo, col suo testo (dal 31/10/2026)
        CHECK(m.tempo() >= 9000.0);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: a 60 km dal Foxtrot, il tempo scade senza che emerga
        Missione m(scrivi(s, "lib_s03_sconfitta"), 1);
        m.avanza(2.0);
        const Vec2 lontano = polare_prova(m.unita().at(2).pos, 45.0, 60000.0);
        REQUIRE(m.comando("posizione", {lontano.x, lontano.y}));
        avanza_fino(m, s.durata_s + 60.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        CHECK(m.variabili()[300] == 0);
        CHECK(m.esito_dettagliato().categoria != EsitoDettagliato::Categoria::successo);
    }
}

TEST_CASE("libreria S06 «Petroliere»: la mina colpisce la prima, arriva l'ordine, il posamine riparte e posa; fermato, finisce bene" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S06_PETROLIERE");
    s.aerei_asw.clear();
    {   // Enhanced: la mk67 sulla rotta colpisce la prima petroliera; l'ordine flash supera quello di riserva
        EnhancedPieno x(scrivi(s, "lib_s06_enhanced"), 1);
        CHECK(x.m.campo_minato().mine().size() == 1);
        while (x.m.tempo() < 4000.0 && !x.m.esito() && quanti(x.m, TipoEvento::mina_esplosa) == 0) x.avanza(10.0);
        REQUIRE(quanti(x.m, TipoEvento::mina_esplosa) == 1);
        CAPTURE(x.m.tempo());
        x.avanza(120.0);
        CHECK(x.m.variabili()[200] == 1);   // «riparte»
        CHECK(x.m.radio()->in_coda("INTERCETTARE"));
        CHECK(x.m.unita().at(8).velocita > 3.0);
        // lasciato fare, sulla rotta delle altre due posa altre tre mk67
        while (x.m.tempo() < 7000.0 && !x.m.esito() && quanti(x.m, TipoEvento::mina_posata) < 3) x.avanza(10.0);
        CHECK(quanti(x.m, TipoEvento::mina_posata) == 3);
        CHECK(x.m.errori().empty());
    }
    {   // fermato: il posamine affondato dopo l'ordine chiude la missione
        EnhancedPieno x(scrivi(s, "lib_s06_fermato"), 1);
        while (x.m.tempo() < 4000.0 && !x.m.esito() && quanti(x.m, TipoEvento::mina_esplosa) == 0) x.avanza(10.0);
        x.avanza(60.0);
        x.m.distruggi(8);
        x.avanza(60.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(obiettivo(x.m.esito_dettagliato(), "petroliere")->stato != Obiettivo::Stato::fallito);
        CHECK(x.m.errori().empty());
    }
    {   // classico: nessuna mina; l'ordine arriva a 90 minuti e il convoglio esce dallo stretto
        Missione m(scrivi(s, "lib_s06_classico"), 1);
        m.radio_enhanced();
        const auto trasmesso = [&](const std::string& id) {
            const auto& t = m.radio()->debriefing().trasmessi;
            return std::any_of(t.begin(), t.end(), [&](const auto& v) { return v.messaggio.id == id; });
        };
        avanza_fino(m, 5300.0);
        CHECK_FALSE(trasmesso("INTERCETTARE_TARDI"));
        CHECK_FALSE(trasmesso("INTERCETTARE"));
        avanza_fino(m, s.durata_s);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 44);   // l'esito suo, col suo testo (dal 31/10/2026)
        CHECK(m.errori().empty());
    }
}

TEST_CASE("libreria S10 «Il cavo della fregata»: mezz'ora entro 5 km dalla schiera; da lontano niente; classificati, bruciati; "
          "sulla scia, il cavo chiude male (Enhanced)" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S10_CAVO_DELLA_FREGATA");
    // l'Alfa tenuto `metri` dietro la Perry (id 2 nel file) per `secondi`, nell'Enhanced col cavo della schiera
    const auto dietro = [](Enhanced& x, double metri, double secondi) {
        const double fino = x.m.tempo() + secondi;
        while (x.m.tempo() < fino && !x.m.esito()) {
            const auto& g = *x.m.giocatore();
            const Vec2 b = x.m.unita().at(2).pos;
            REQUIRE(x.m.comando("posizione", {b.x - metri * std::sin(radianti(g.rotta)), b.y - metri * std::cos(radianti(g.rotta))}));
            x.avanza(30.0);
        }
    };
    {   // vittoria: 3 km dietro la Perry che non vede, entro 5 km dal cavo: alla mezz'ora la firma e' sul nastro
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 0.0;
        Enhanced x(scrivi(t, "lib_s10_vittoria"), 1);
        sordi(x.m);
        x.avanza(2.0);
        dietro(x, 3000.0, 2400.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(x.m.tempo() < 2100.0);   // 1800 s di registrazione, a passi di 10 s
        const auto e = x.m.esito_dettagliato();
        CHECK(obiettivo(e, "firma")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(e, "occulto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(x.m.errori().empty());
    }
    {   // da lontano (9 km dietro la Perry, oltre i 5 km dal cavo) la firma non si registra: era la vittoria da fermi
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 0.0;
        Enhanced x(scrivi(t, "lib_s10_lontano"), 1);
        sordi(x.m);
        x.avanza(2.0);
        dietro(x, 9000.0, 2400.0);
        CHECK_FALSE(x.m.esito());
        CHECK(x.m.errori().empty());
    }
    {   // classificati: l'Alfa che tiene gli ordini di partenza (era la vittoria da fermi) e' sentito dalle Perry; quando
        // una lo classifica almeno POSSUB, o il contatto diventa lungo, l'operazione e' bruciata
        Enhanced x(scrivi(s, "lib_s10_classificati"), 1);
        while (x.m.tempo() < 3600.0 && !x.m.esito()) x.avanza(10.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 42);
        CHECK(x.m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(obiettivo(x.m.esito_dettagliato(), "occulto")->stato == Obiettivo::Stato::fallito);
        CHECK(x.m.errori().empty());
    }
    {   // sconfitta: l'Alfa fermo a 1 km sulla rotta della Perry, sotto il cavo che scende verso la schiera: il cavo gli arriva
        // addosso. La forma vera della SQR-19 (nucleo/schiera, docs/ricerca/33): con lo sprint and drift la Perry tiene la
        // schiera fra 180 e 280 m, e il cavo attraversa in diagonale le quote sopra
        ScenarioMissione t = s;
        const auto& p = t.unita[1];
        t.unita[2].pos = polare_prova(p.pos, p.rotta + 90.0, 20000.0);   // la seconda Perry lontana
        t.unita[0].pos = polare_prova(p.pos, p.rotta, 1055.0);
        t.unita[0].rotta = p.rotta;
        t.unita[0].velocita = 0.0;
        t.unita[0].quota = 150.0;
        t.unita[1].velocita = 8.0;
        // la prova misura il cavo: senza le regole della classificazione, che a 1 km arriva prima
        std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id.rfind("classificati", 0) == 0; });
        Enhanced x(scrivi(t, "lib_s10_cavo"), 1);
        x.m.usa_reazioni(false);   // la prova misura l'urto col cavo: senza la Perry che pedina l'Alfa scoperto (fase 4)
        x.avanza(1.0);
        x.m.usa_crazy_ivan(false);
        while (x.m.tempo() < 1200.0 && !x.m.esito()) x.avanza(10.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 41);
        CHECK(x.m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria I04 «La porta di Ercole»: a Cadice e' fatta; oltre 20 nodi il P-3C; nel canale le CAPTOR (Enhanced)" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I04_PORTA_DI_ERCOLE");
    const auto& cadice = zona(s, "cadice").cerchio;
    const auto& campo = zona(s, "campo").cerchio;
    {   // vittoria: a Cadice (il comando «posizione»)
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i04_vittoria"), 1);
        avanza_fino(m, 60.0);
        REQUIRE(m.comando("posizione", {cadice.x, cadice.y}));
        avanza_fino(m, 120.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(obiettivo(m.esito_dettagliato(), "cadice")->stato == Obiettivo::Stato::raggiunto);
        CHECK(m.errori().empty());
    }
    {   // a 25 nodi: il SOSUS fa entrare l'Orion da Rota
        ScenarioMissione t = s;
        t.unita[0].velocita = 25.0;
        Missione m(scrivi(t, "lib_i04_sosus"), 1);
        const Vec2 canale = {zona(s, "campo").cerchio.x, zona(s, "campo").cerchio.y};
        m.avanza(2.0);
        CHECK(distanza(m.unita().at(7).pos, canale) > 500000.0);   // l'Orion aspetta fuori dal teatro
        avanza_fino(m, 120.0);
        CHECK(m.variabili()[201] == 1);
        CHECK(distanza(m.unita().at(7).pos, canale) < 200000.0);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: nel canale le CAPTOR; il sonarista le annuncia, e una lancia sul battello
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].pos = {campo.x - 400.0, campo.y};
        t.unita[0].velocita = 4.0;
        t.unita[0].rotta = 0.0;
        t.unita[0].quota = 150.0;
        EnhancedPieno x(scrivi(t, "lib_i04_captor"), 1);
        CHECK(x.m.campo_minato().mine().size() == 5);
        x.avanza(30.0);   // il primo controllo della regola del campo
        while (x.m.tempo() < 1800.0 && !x.m.esito() && quanti(x.m, TipoEvento::mina_esplosa) == 0) x.avanza(10.0);
        CHECK(x.m.variabili()[200] == 1);
        CHECK(quanti(x.m, TipoEvento::mina_esplosa) >= 1);
        CHECK(x.m.errori().empty());
    }
    {   // sconfitta: fermi a Gibilterra, il tempo scade
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 0.0;
        Missione m(scrivi(t, "lib_i04_sconfitta"), 1);
        avanza_fino(m, s.durata_s + 60.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() != 40);
        CHECK(m.esito_dettagliato().categoria != EsitoDettagliato::Categoria::successo);
    }
}

TEST_CASE("libreria I07 «Lupo ferito»: al punto d'incontro e' fatta; oltre 12 nodi l'asse; l'Oscar lancia se nessuno lo ferma" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I07_LUPO_FERITO");
    const auto& incontro = zona(s, "incontro").cerchio;
    REQUIRE(s.danni_iniziali);
    CHECK(s.carico.dotazione.at(0).numero == 2);   // due Mk 48
    {   // vittoria: al punto d'incontro (il comando «posizione»), senza superare i 12 nodi
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i07_vittoria"), 1);
        avanza_fino(m, 120.0);
        CHECK(m.variabili()[301] == 1);   // le variabili d'autore stanno da 300
        REQUIRE(m.comando("posizione", {incontro.x, incontro.y}));
        avanza_fino(m, 240.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(obiettivo(m.esito_dettagliato(), "incontro")->stato == Obiettivo::Stato::raggiunto);
        CHECK(m.errori().empty());
    }
    {   // a 16 nodi: l'asse vibra e il secondario dei 12 nodi e' perso
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 16.0;
        Missione m(scrivi(t, "lib_i07_veloce"), 1);
        avanza_fino(m, 120.0);
        CHECK(m.variabili()[301] == 0);
        CHECK(m.errori().empty());
    }
    {   // nessuno ferma l'Oscar: si rivela a 90 minuti e lancia un'ora dopo
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i07_oscar"), 1);
        avanza_fino(m, 5500.0);
        CHECK(m.variabili()[302] == 1);
        CHECK(m.variabili()[303] == 0);   // l'Oscar di riserva non conta come affondato prima di entrare
        avanza_fino(m, 9100.0);
        CHECK(m.variabili()[302] == 0);
        CHECK_FALSE(m.esito().has_value());   // il lancio non chiude la missione
        CHECK(m.errori().empty());
    }
    {   // senza rapporto radio il secondario dell'Oscar segnalato resta a 0 (la condizione e' m688.trasmesso, Enhanced)
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i07_rapporto"), 1);
        avanza_fino(m, 6000.0);
        CHECK(m.variabili()[304] == 0);
        REQUIRE(obiettivo(m.esito_dettagliato(), "rapporto"));
        CHECK(m.errori().empty());
    }
    {   // Enhanced: i danni di partenza ci sono e la missione gira
        Enhanced x((LIBRERIA / "LIB_I07_LUPO_FERITO.lua").string(), 1);
        x.avanza(60.0);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria S13 «Outlaw Shark»: il TASM sulla nave bersaglio chiude bene; senza lancio in un'ora il secondario cade" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S13_OUTLAW_SHARK");
    CHECK(s.esercitazione);   // tutte le armi inerti
    REQUIRE(s.unita.size() >= 2);
    CHECK(s.unita[1].classe == "kara");   // Top Sail e Head Net C: i radar che cerca l'ESM del TASM
    {   // vittoria: la nave bersaglio «colpita»
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_s13_vittoria"), 1);
        avanza_fino(m, 300.0);
        CHECK(m.variabili()[301] == 1);
        m.distruggi(2);
        avanza_fino(m, 600.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);   // l'obiettivo «colpire» conta i colpi del giocatore, non distruggi()
        CHECK(m.errori().empty());
    }
    {   // nessun lancio: dopo un'ora dal primo dato (10 minuti) il secondario cade; a tempo scaduto la prova non e' riuscita
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_s13_tardi"), 1);
        avanza_fino(m, 4300.0);
        CHECK(m.variabili()[301] == 0);
        avanza_fino(m, s.durata_s + 60.0);
        REQUIRE(m.esito());
        CHECK(obiettivo(m.esito_dettagliato(), "bersaglio")->stato != Obiettivo::Stato::raggiunto);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: il dato dell'E-2 arriva alla sala radio come contatto d'intelligence sulla nave bersaglio
        Enhanced x((LIBRERIA / "LIB_S13_OUTLAW_SHARK.lua").string(), 1);
        x.m.registra_statistiche(true);   // il registro dei radar accesi (R5), prima del primo passo
        x.m.radio_enhanced();
        x.m.applica({x.m.tempo(), TipoOrdine::quota_periscopica});   // a quota periscopica con l'antenna
        x.avanza(240.0);
        x.m.applica({x.m.tempo(), TipoOrdine::antenna, 0, 0, 1});
        x.avanza(700.0);
        REQUIRE(x.m.radio());
        bool sul_bersaglio = false;
        for (const auto& c : x.m.radio()->intelligence(x.m.tempo()))
            for (const int u : c.dati.unita) sul_bersaglio = sul_bersaglio || u == 2;
        CHECK(sul_bersaglio);
        // l'EMCON di partenza «ricerca»: i simulatori del bersaglio accendono Top Sail e Head Net C, che cerca l'ESM del
        // TASM (in pace partirebbe in navigazione, coi radar di ricerca spenti: verifica del disassemblatore, 28/10)
        REQUIRE(x.m.emcon(2).has_value());
        CHECK(*x.m.emcon(2) >= StatoEmcon::ricerca);
        bool top_sail = false, head_net = false;
        for (const auto& e : x.m.emissioni())
            if (e.unita == 2 && e.tipo == "radar") {
                top_sail = top_sail || e.dettaglio == "top_sail";
                head_net = head_net || e.dettaglio == "head_net_c";
            }
        CHECK(top_sail);
        CHECK(head_net);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria I05 «Il disertore»: nelle acque italiane l'asilo; la Ekvator affondata; il Kilo lancia e le ROE si liberano" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I05_IL_DISERTORE");
    REQUIRE(s.unita.size() >= 4);
    const auto& acque = zona(s, "acque_italiane").cerchio;
    const auto& agguato = zona(s, "agguato").cerchio;
    {   // vittoria: la Ekvator gia' sul bordo delle dodici miglia
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[1].pos = {acque.x + (acque.r + 800.0) * std::sin(radianti(135.0)), acque.y + (acque.r + 800.0) * std::cos(radianti(135.0))};
        Missione m(scrivi(t, "lib_i05_vittoria"), 1);
        avanza_fino(m, 900.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(m.errori().empty());
    }
    {   // sconfitta: la Ekvator affondata
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i05_sconfitta"), 1);
        avanza_fino(m, 60.0);
        m.distruggi(2);
        avanza_fino(m, 200.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 41);
        CHECK(m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::fallimento);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: la Ekvator nel punto dell'agguato; il Kilo lancia su di lei con l'attacco forzato e le ROE si liberano
        ScenarioMissione t = s;
        t.unita[1].pos = {agguato.x, agguato.y};
        EnhancedPieno x(scrivi(t, "lib_i05_agguato"), 1);
        while (x.m.tempo() < 900.0 && !x.m.esito() && x.m.variabili()[302] == 0) x.avanza(10.0);
        CHECK(x.m.variabili()[302] == 1);   // il Kilo ha lanciato
        bool siluro = false;
        for (const auto& [id, u] : x.m.unita()) siluro = siluro || ((u.flag & 0x20) && u.lanciatore == 4);
        CHECK(siluro);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria I03 «Acque basse, nervi tesi»: novanta minuti nell'area; a un'ora il siluro inerte; l'esca fa scattare la crisi" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I03_ACQUE_BASSE");
    const auto& area = zona(s, "esercitazione").cerchio;
    {   // novanta minuti nell'area (il conto dei controlli ogni 10 s)
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].pos = {area.x, area.y};
        t.unita[0].velocita = 0.0;
        Missione m(scrivi(t, "lib_i03_area"), 1);
        avanza_fino(m, 5000.0);
        CHECK(m.variabili()[302] == 0);
        avanza_fino(m, 5600.0);
        CHECK(m.variabili()[302] == 1);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: a un'ora il Kilo tira sul 688 un siluro d'esercitazione (l'attacco forzato senza soluzione)
        ScenarioMissione t = s;
        ciechi(t);
        EnhancedPieno x(scrivi(t, "lib_i03_siluro"), 1);
        x.avanza(3620.0);
        bool siluro = false;
        for (const auto& [id, u] : x.m.unita()) siluro = siluro || ((u.flag & 0x20) && u.lanciatore == 7);
        CHECK(siluro);
        CHECK(x.m.errori().empty());
    }
    {   // chi scappa oltre i 12 nodi entro 5 minuti dal siluro si fa scoprire: allarme e crisi
        ScenarioMissione t = s;
        ciechi(t);
        t.unita[0].velocita = 16.0;
        Missione m(scrivi(t, "lib_i03_fuga"), 1);
        avanza_fino(m, 3500.0);
        CHECK(m.variabili()[11] == 0);
        avanza_fino(m, 3800.0);
        CHECK(m.variabili()[11] == 1);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: il fondo toccato fa fallire il secondario; con l'allarme dei sovietici e' l'incidente diplomatico
        ScenarioMissione t = s;
        ciechi(t);
        // a 15 nodi contro una secca: l'urto orizzontale danneggia lo scafo (il registro lo annota come incaglio)
        const Fondale f = Fondale::carica(DATI + "/teatri/baltico.fdl");
        // da 54,55N 11,40E (26 m) verso nord: a 5 km il fondale sale a 11 m, a 10 km a 2
        t.unita[0].pos = f.proiettata({54.55, 11.40});
        REQUIRE(f.profondita(t.unita[0].pos) > 20.0);
        t.unita[0].rotta = 0.0;
        t.unita[0].quota = 15.0;
        t.unita[0].velocita = 15.0;
        Enhanced x(scrivi(t, "lib_i03_fondo"), 1);
        x.avanza(600.0);
        CHECK(std::any_of(x.m.eventi_missione().begin(), x.m.eventi_missione().end(),
                          [](const EventoMissione& e) { return e.tipo == TipoEvento::incaglio; }));
        CHECK(x.m.variabili()[306] == 0);
        CHECK_FALSE(x.m.esito().has_value());
        x.m.variabili()[11] = 1;   // l'allarme
        x.avanza(30.0);
        REQUIRE(x.m.esito().has_value());
        CHECK(*x.m.esito() == 41);
        CHECK(x.m.errori().empty());
    }
    {   // senza incaglio il secondario resta compiuto
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i03_pulito"), 1);
        avanza_fino(m, 120.0);
        CHECK(m.variabili()[306] == 1);
    }
}

TEST_CASE("libreria I12 «Finestra di lancio»: l'ordine falso non vale, quello vero si'; l'obiettivo sui TLAM della 0521" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I12_FINESTRA_DI_LANCIO");
    REQUIRE(s.tlam.size() == 2);
    REQUIRE(s.navigazione);
    ScenarioMissione t = s;
    ciechi(t);
    EnhancedPieno x(scrivi(t, "lib_i12_ordini"), 1);
    REQUIRE(x.m.libreria_tlam().size() == 2);
    x.m.applica({x.m.tempo(), TipoOrdine::quota_periscopica});   // a quota periscopica con l'antenna, per il broadcast
    x.avanza(400.0);
    REQUIRE(x.m.applica({x.m.tempo(), TipoOrdine::antenna, 0, 0, 1}));
    x.m.applica({x.m.tempo(), TipoOrdine::imposta_missile, 1.0, static_cast<double>(CampoMissile::missione_tlam), 1});   // la 0522
    x.avanza(4000.0 - x.m.tempo());   // il falso va in onda a 3600 s e arriva col broadcast (~3730 s), il vero a 4800 (~4930)
    CHECK_FALSE(x.m.tlam_eseguibile("0522"));   // l'autenticazione non torna
    auto tiro = x.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->tlam);
    CHECK(tiro->tlam->id == "0522");
    CHECK(tiro->ordine_non_autentico);
    CHECK_FALSE(tiro->rdy);
    x.avanza(5100.0 - x.m.tempo());
    CHECK(x.m.tlam_eseguibile("0521"));
    const Obiettivo* o = obiettivo(x.m.esito_dettagliato(), "tlam_0521");
    REQUIRE(o);
    CHECK(o->stato == Obiettivo::Stato::in_corso);
    CHECK(x.m.errori().empty());
}

TEST_CASE("libreria I11 «Il primo eco»: lo Slava colpito chiude bene; i rapporti del P-3 sul gruppo; il P-500 sul 688 in superficie" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I11_IL_PRIMO_ECO");
    REQUIRE(s.unita.size() >= 12);
    CHECK(s.unita[1].classe == "slava");
    REQUIRE(s.radio);
    CHECK(s.radio->messaggi.size() == 4);
    for (const auto& m : s.radio->messaggi) CHECK(m.contatto == std::vector<int>{2, 3, 4});
    const auto sc = std::find_if(s.obiettivi.begin(), s.obiettivi.end(), [](const ObiettivoScenario& o) { return o.tipo == "scatola"; });
    REQUIRE(sc != s.obiettivi.end());
    CHECK(sc->massima == "media");
    {   // vittoria: lo Slava colpito
        ScenarioMissione t = s;
        ciechi(t);
        Missione m(scrivi(t, "lib_i11_vittoria"), 1);
        avanza_fino(m, 300.0);
        m.distruggi(2);
        avanza_fino(m, 600.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 40);
        CHECK(m.errori().empty());
    }
    {   // Enhanced: il 688 emerso a 30 km dallo Slava (il P-500 vuole almeno 20 km; l'orizzonte radar fra il top sail a 33 m
        // e la vela a 5,7 m e' di 33,5 km: a 35 km, coi dati del 31/10/2026, non c'era contatto); lo Slava lo vede e gli tira
        // il P-500 del modello
        ScenarioMissione t = s;
        t.aerei_asw.clear();
        t.unita[0].pos = {t.unita[1].pos.x - 30000.0 * std::sin(radianti(t.unita[1].rotta + 90.0)),
                          t.unita[1].pos.y - 30000.0 * std::cos(radianti(t.unita[1].rotta + 90.0))};
        t.unita[0].quota = 0.0;
        t.unita[0].velocita = 4.0;
        EnhancedPieno x(scrivi(t, "lib_i11_p500"), 1);
        std::string arma;
        for (int k = 0; k < 60 && arma.empty(); ++k) {   // col mare 4 il contatto lampeggia: mezzo minuto visibile in ~6 minuti
            x.avanza(10.0);
            for (const auto& [id, u] : x.m.unita())
                if (u.categoria == "arma" && u.lanciatore == 2) arma = u.classe;
        }
        MESSAGE("lo Slava sul 688 emerso: ", arma.empty() ? std::string("niente") : arma, " a ", x.m.tempo(), " s");
        CHECK(arma.rfind("p500", 0) == 0);
        CHECK(x.m.errori().empty());
    }
}

// «scoprire» conta anche la classificazione giusta del sonarista e l'identificazione visiva giusta (collaudo del
// 29/10/2026: col sonarista nuovo da lontano arriva solo il rilevamento, e la sola durata classica non bastava a I03 e
// S05); una classificazione sbagliata non conta
TEST_CASE("libreria: «scoprire» conta il libretto al periscopio (I03) e il sonarista sul Los Angeles che cavita (S05), non le classi sbagliate" * doctest::skip(!c_e())) {
    {   // I03 di giorno: le unita' dell'esercitazione identificate a vista; prima sbagliate (non contano), poi giuste
        ScenarioMissione s = scenario("LIB_I03_ACQUE_BASSE");
        s.aerei_asw.clear();
        const auto& area = zona(s, "esercitazione").cerchio;   // il 688 in mezzo all'esercitazione, fermo
        s.unita[0].pos = {area.x, area.y};
        s.unita[0].velocita = 0.0;
        EnhancedPieno x(scrivi(s, "lib_i03_libretto"), 1);
        x.m.imposta_sonarista(Esperienza::medio);
        const std::vector<int> esercitazione = {2, 3, 4, 5, 6, 7};
        std::vector<int> visti;
        for (int k = 0; k < 120 && visti.size() < 4; ++k) {
            x.avanza(15.0);
            visti.clear();
            for (const int id : esercitazione)
                if (x.m.giocatore()->contatti.count(id)) visti.push_back(id);
        }
        INFO("contatti del giocatore: " << x.m.giocatore()->contatti.size());
        REQUIRE(visti.size() >= 4);
        const auto stato = [&] { return obiettivo(x.m.esito_dettagliato(), "classificate")->stato; };
        const int prima = obiettivo(x.m.esito_dettagliato(), "classificate")->fatti;
        for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, "cargo")));   // tutte sbagliate
        x.avanza(2.0);
        CHECK(obiettivo(x.m.esito_dettagliato(), "classificate")->fatti == prima);
        for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, x.m.unita().at(id).classe)));
        x.avanza(2.0);
        CHECK(stato() == Obiettivo::Stato::raggiunto);
        CHECK(x.m.errori().empty());
    }
    {   // S05: il Los Angeles che corre a 25 nodi a 30 m cavita; il sonarista medio lo classifica, e l'intruso e' scoperto.
        // Nella missione fugge a 12 nodi a 130 m e non cavita: non si classifica, e «intruso» e' diventato un contatto
        ScenarioMissione s = scenario("LIB_S05_GUARDIANO_DEL_CAVO");
        s.aerei_asw.clear();
        s.regole.clear();
        for (auto& o : s.obiettivi)   // nella missione «intruso» e' un contatto; qui si prova «scoprire»
            if (o.id == "intruso") o.tipo = "scoprire";
        int la = -1;
        for (auto& u : s.unita)
            if (u.classe == "los_angeles" && !u.giocatore) {
                la = u.id;
                u.quota = 30.0;
                u.velocita = 25.0;
                u.punti = {Vec2{u.pos.x, u.pos.y - 60000.0}};   // verso sud
                u.rotta = 180.0;
            }
        REQUIRE(la >= 0);
        EnhancedPieno x(scrivi(s, "lib_s05_sonarista"), 1);
        x.m.imposta_sonarista(Esperienza::medio);   // il sonarista dell'equipaggio, come dal menu (di serie medio)
        x.insegui(la, 2500.0, 1800.0, false);
        bool sonarista = false;
        for (const auto& e : x.m.eventi_missione())
            sonarista = sonarista || (e.tipo == TipoEvento::classificazione && e.codice == 411 && e.unita == la && e.giusta);
        CHECK(sonarista);
        CHECK(obiettivo(x.m.esito_dettagliato(), "intruso")->stato == Obiettivo::Stato::raggiunto);
        CHECK(x.m.errori().empty());
    }
}

// I13 «Sagome nello Stretto» e I14 «Levante» (docs/25 §4, docs/17 §2d): le navi da guerra identificate a vista col
// libretto, le distanze lette col telemetro; giuste vince, sbagliate no. Col Levante bastano 5 su 9 e il 15%, e una
// collisione chiude in fallimento
TEST_CASE("libreria I13 e I14: identificare e telemetro, giusti raggiunti e sbagliati no; col Levante soglie piu' larghe" * doctest::skip(!c_e())) {
    for (const bool levante : {false, true}) {
        CAPTURE(levante);
        ScenarioMissione s = scenario(levante ? "LIB_I14_LEVANTE" : "LIB_I13_SAGOME_NELLO_STRETTO");
        CHECK(s.esercitazione);   // siluri d'esercitazione
        const auto identifica = std::find_if(s.obiettivi.begin(), s.obiettivi.end(), [](const ObiettivoScenario& o) { return o.id == "identifica"; });
        REQUIRE(identifica != s.obiettivi.end());
        CHECK(identifica->unita.size() == 9u);
        CHECK(identifica->minimo == (levante ? 5 : 7));
        CHECK(std::any_of(s.regole.begin(), s.regole.end(), [](const RegolaScenario& r) { return r.id == "collisione"; }) == levante);
        s.aerei_asw.clear();
        EnhancedPieno x(scrivi(s, levante ? "lib_i14" : "lib_i13"), 1);
        // il 688 fermo a quota periscopica: il traffico gli passa vicino; si aspettano i contatti sulle navi da guerra
        std::vector<int> visti;
        for (int k = 0; k < 160 && static_cast<int>(visti.size()) < identifica->minimo; ++k) {
            x.avanza(15.0);
            visti.clear();
            for (const int id : identifica->unita)
                if (x.m.giocatore()->contatti.count(id)) visti.push_back(id);
        }
        REQUIRE(static_cast<int>(visti.size()) >= identifica->minimo);
        const auto stato = [&](const std::string& id) { return obiettivo(x.m.esito_dettagliato(), id)->stato; };
        // sbagliate: niente
        for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, "peschereccio")));
        const auto lettura = [&](int id, double fattore) {
            const double vera = distanza(x.m.giocatore()->pos, x.m.unita().at(id).pos);
            REQUIRE(x.m.applica(ordine_misura_telemetro(x.m.tempo(), id, vera * fattore, 30.0)));
        };
        for (int k = 0; k < 3; ++k) lettura(visti[static_cast<std::size_t>(k)], 1.2);   // 20%: fuori in tutte e due
        x.avanza(2.0);
        CHECK(stato("identifica") == Obiettivo::Stato::in_corso);
        CHECK(stato("telemetro") == Obiettivo::Stato::in_corso);
        // giuste: raggiunti. Il 12% basta solo col Levante (15%), non di giorno (10%). I contatti di adesso: uno di
        // passaggio puo' essersi perso
        for (int k = 0; k < 80; ++k) {
            visti.clear();
            for (const int id : identifica->unita)
                if (x.m.giocatore()->contatti.count(id)) visti.push_back(id);
            if (static_cast<int>(visti.size()) >= identifica->minimo) break;
            x.avanza(15.0);
        }
        REQUIRE(static_cast<int>(visti.size()) >= identifica->minimo);
        for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, x.m.unita().at(id).classe)));
        for (int k = 0; k < 3; ++k) lettura(visti[static_cast<std::size_t>(k)], 1.12);
        x.avanza(2.0);
        CHECK(stato("identifica") == Obiettivo::Stato::raggiunto);
        CHECK(stato("telemetro") == (levante ? Obiettivo::Stato::raggiunto : Obiettivo::Stato::in_corso));
        if (!levante) {
            for (int k = 0; k < 3; ++k) lettura(visti[static_cast<std::size_t>(k)], 0.95);
            x.avanza(2.0);
            CHECK(stato("telemetro") == Obiettivo::Stato::raggiunto);
        }
        CHECK(x.m.errori().empty());
    }
}


// I15 «Fanali» (docs/17 §2d, docs/27): la notte del riconoscimento. La Kara e la nave spia a luci spente, un mercantile
// in avaria senza governo, sei pescherecci in pesca; il 688 a quota periscopica non si fa vedere a 9 miglia nel buio. Le rotte dai fanali
// sono primarie: giuste raggiunte, sbagliate di 60 gradi no; una collisione chiude in fallimento
TEST_CASE("libreria I15: di notte le luci per unita', la Kara buia non vede il periscopio, identificare e rotte dai fanali" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I15_FANALI");
    CHECK(s.esercitazione);
    CHECK(s.ora_utc == doctest::Approx(23.5));
    const auto ob = [&](const std::string& id) {
        return std::find_if(s.obiettivi.begin(), s.obiettivi.end(), [&](const ObiettivoScenario& o) { return o.id == id; });
    };
    REQUIRE(ob("identifica") != s.obiettivi.end());
    CHECK(ob("identifica")->minimo == 5);
    REQUIRE(ob("rotta") != s.obiettivi.end());
    CHECK(ob("rotta")->primario);
    REQUIRE(ob("telemetro") != s.obiettivi.end());
    CHECK_FALSE(ob("telemetro")->primario);
    CHECK(ob("telemetro")->errore == doctest::Approx(0.15));
    CHECK(ob("foto_agi") == s.obiettivi.end());
    CHECK(ob("tiro") == s.obiettivi.end());
    CHECK(std::any_of(s.regole.begin(), s.regole.end(), [](const RegolaScenario& r) { return r.id == "collisione"; }));
    CHECK(std::any_of(s.regole.begin(), s.regole.end(), [](const RegolaScenario& r) { return r.id == "scia"; }));
    std::map<std::string, int> per_classe;
    int avaria = -1, pescherecci = 0;
    for (const auto& u : s.unita) {
        per_classe.emplace(u.classe, u.id);
        if (u.luci == "non_governa") avaria = u.id;
        pescherecci += u.classe == "peschereccio";
    }
    CHECK(pescherecci == 6);
    REQUIRE(avaria >= 0);
    const int kara = per_classe.at("kara"), agi = per_classe.at("agi"), kidd = per_classe.at("kidd"), pesca = per_classe.at("peschereccio");
    s.aerei_asw.clear();
    EnhancedPieno x(scrivi(s, "lib_i15"), 1);
    x.avanza(1.0);
    REQUIRE(x.m.luce_periscopi() < 0.2);   // mezzanotte
    CHECK(x.m.stato_luci(kara) == luci::Stato::spente);
    CHECK(x.m.stato_luci(agi) == luci::Stato::spente);
    CHECK(x.m.stato_luci(kidd) == luci::Stato::moto);
    CHECK(x.m.stato_luci(pesca) == luci::Stato::pesca);
    CHECK(x.m.stato_luci(avaria) == luci::Stato::non_governa);
    // il 688 fermo a quota periscopica; si aspettano i contatti sulle navi da guerra. Le navi dell'Eskadra, a 9 miglia e
    // meno, non lo vedono: di notte la scoperta automatica vale solo entro la portata della sagoma e dei fanali
    const auto& identifica = *ob("identifica");
    std::vector<int> visti;
    for (int k = 0; k < 160 && static_cast<int>(visti.size()) < identifica.minimo; ++k) {
        x.avanza(15.0);
        visti.clear();
        for (const int id : identifica.unita)
            if (x.m.giocatore()->contatti.count(id)) visti.push_back(id);
        for (const int id : {kara, per_classe.at("krivak2"), per_classe.at("nanuchka")}) {
            const auto& c = x.m.unita().at(id).contatti;
            if (distanza(x.m.unita().at(id).pos, x.m.giocatore()->pos) > 3000.0) CHECK_FALSE((c.count(0) && c.at(0).sensore == "vista"));
        }
    }
    REQUIRE(static_cast<int>(visti.size()) >= identifica.minimo);
    const auto stato = [&](const std::string& id) { return obiettivo(x.m.esito_dettagliato(), id)->stato; };
    // l'AOB vero: il rilevamento del 688 dalla nave meno la sua rotta (+ a dritta)
    const auto aob = [&](int id) {
        const auto& b = x.m.unita().at(id);
        const Vec2 g = x.m.giocatore()->pos;
        double a = std::atan2(g.x - b.pos.x, g.y - b.pos.y) * 180.0 / 3.14159265358979323846 - b.rotta;
        while (a > 180.0) a -= 360.0;
        while (a < -180.0) a += 360.0;
        return a;
    };
    const auto osserva = [&](int id, double scarto) {
        double a = aob(id) + scarto;
        if (a > 180.0) a -= 360.0;
        if (a < -180.0) a += 360.0;
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::osserva_aob, a, 10.0, id}));
    };
    // sbagliate: niente
    for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, "peschereccio")));
    for (int k = 0; k < 3; ++k) osserva(visti[static_cast<std::size_t>(k)], 60.0);
    x.avanza(2.0);
    CHECK(stato("identifica") == Obiettivo::Stato::in_corso);
    CHECK(stato("rotta") == Obiettivo::Stato::in_corso);
    // giuste: raggiunti
    for (const int id : visti) REQUIRE(x.m.applica(ordine_identifica_visiva(x.m.tempo(), id, x.m.unita().at(id).classe)));
    for (int k = 0; k < 3; ++k) osserva(visti[static_cast<std::size_t>(k)], 5.0);
    x.avanza(2.0);
    CHECK(stato("identifica") == Obiettivo::Stato::raggiunto);
    CHECK(stato("rotta") == Obiettivo::Stato::raggiunto);
    CHECK(x.m.errori().empty());
}

// I16 «Il bastione bianco» (docs/17 §2e): sotto il pack di marzo a Okhotsk. Il ghiaccio viene dal meteo del teatro (ERA5,
// col margine negli stretti): il Pacifico libero, il bastione sotto la banchisa. Il pedinamento accende la campana
// dell'ELF; sotto la banchisa niente antenne, quindi niente rapporto; in una polynya, a quota periscopica con l'antenna,
// il rapporto passa; poi il rientro nel Pacifico chiude con l'esito 40. Una chiglia attraversata a 20 m e' un urto.
// Per la prova il pedinamento dura mezzo minuto, col 688 messo a 600 m dal Delta, e niente aereo
TEST_CASE("libreria I16: il pack dal meteo, la campana dopo il pedinamento, il rapporto solo in acqua libera, il rientro" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I16_BASTIONE_BIANCO");
    CHECK(s.teatro == "kunashir");
    CHECK(s.mese == 3);
    CHECK(s.meteo_realistico);   // il ghiaccio dai punti ERA5: col meteo fisso non c'e' il margine
    CHECK_FALSE(s.meteo_fisso);
    REQUIRE(s.navigazione);
    CHECK(s.navigazione->realistica_di_serie);
    REQUIRE(s.radio);
    const auto msg = [&](const std::string& id) {
        return std::find_if(s.radio->messaggi.begin(), s.radio->messaggi.end(), [&](const MessaggioScenario& m) { return m.id == id; });
    };
    REQUIRE(msg("campana") != s.radio->messaggi.end());
    CHECK(msg("campana")->precedenza == "flash");   // il campanello dell'ELF: arriva sotto il pack col filo
    CHECK(msg("campana")->su_richiesta);
    REQUIRE(msg("intel") != s.radio->messaggi.end());
    int delta = -1, victor = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "delta3") delta = u.id;
        if (u.classe == "victor3") victor = u.id;
    }
    REQUIRE(delta >= 0);
    REQUIRE(victor >= 0);
    CHECK(msg("intel")->contatto == std::vector<int>{delta});
    for (auto& o : s.obiettivi)
        if (o.id == "seguito") o.minuti = 0.5;
    s.aerei_asw.clear();
    EnhancedPieno x(scrivi(s, "lib_i16"), 1);
    x.m.usa_meteo(Missione::ModoMeteo::realistico);
    x.avanza(2.0);
    const auto* g = x.m.ghiaccio();
    REQUIRE(g);
    const int io = x.m.giocatore()->id;
    CHECK_FALSE(x.m.sotto_banchisa(io));   // la partenza, dal lato del Pacifico: acqua libera
    const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
    const auto stato = [&](const std::string& id) { return obiettivo(x.m.esito_dettagliato(), id)->stato; };
    // il 688 a 600 m dal Delta, nel bastione: sotto la banchisa; mezzo minuto di contatto e la campana
    const auto& d = x.m.unita().at(delta);
    REQUIRE(x.m.comando("posizione", {d.pos.x + 600.0, d.pos.y}));
    for (int k = 0; k < 40 && var(1) == 0; ++k) {
        const auto& dd = x.m.unita().at(delta);
        REQUIRE(x.m.comando("posizione", {dd.pos.x + 600.0, dd.pos.y}));
        x.avanza(15.0);
    }
    CHECK(var(1) == 1);
    CHECK(stato("seguito") == Obiettivo::Stato::raggiunto);
    // sotto la banchisa il rapporto non parte
    REQUIRE(x.m.sotto_banchisa(io));
    CHECK_FALSE(x.m.radio_trasmetti(TipoTrasmissione::contatto).fatta);
    x.avanza(2.0);
    CHECK(var(2) == 0);
    CHECK(stato("rapporto") == Obiettivo::Stato::in_corso);
    // una polynya larga (3 x 3 celle aperte) entro 30 km: a quota periscopica con l'antenna il rapporto passa
    const Vec2 qui = x.m.giocatore()->pos;
    std::optional<Vec2> polynya;
    for (double r = 0.0; r < 30000.0 && !polynya; r += 200.0)
        for (double a = 0.0; a < 360.0 && !polynya; a += 5.0) {
            const Vec2 q{qui.x + r * std::sin(a * 3.14159265358979323846 / 180.0), qui.y + r * std::cos(a * 3.14159265358979323846 / 180.0)};
            bool aperta = true;
            for (int i = -1; i <= 1 && aperta; ++i)
                for (int j = -1; j <= 1 && aperta; ++j) {
                    const PuntoGhiaccio p = g->in({q.x + 200.0 * i, q.y + 200.0 * j}, x.m.tempo());
                    aperta = !p.ghiaccio || p.aperta;
                }
            if (aperta) polynya = q;
        }
    REQUIRE(polynya);
    REQUIRE(x.m.comando("posizione", {polynya->x, polynya->y}));
    REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
    for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) {   // gli alberi salgono solo a quota periscopica (20 ft)
        REQUIRE(x.m.comando("posizione", {polynya->x, polynya->y}));
        x.avanza(10.0);
    }
    CHECK_FALSE(x.m.sotto_banchisa(io));
    REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
    x.avanza(5.0);
    CHECK(x.m.radio_trasmetti(TipoTrasmissione::contatto).fatta);
    for (int k = 0; k < 24 && var(2) == 0; ++k) x.avanza(5.0);   // la trasmissione conta quando e' finita
    CHECK(var(2) == 1);
    CHECK(stato("rapporto") == Obiettivo::Stato::raggiunto);
    // il rientro nel Pacifico chiude la missione
    const auto pacifico = std::find_if(s.zone.begin(), s.zone.end(), [](const ZonaScenario& z) { return z.id == "pacifico"; });
    REQUIRE(pacifico != s.zone.end());
    REQUIRE(x.m.comando("posizione", {pacifico->cerchio.x, pacifico->cerchio.y}));
    for (int k = 0; k < 12 && !x.m.esito(); ++k) x.avanza(5.0);   // le regole si controllano a intervalli
    CHECK(var(3) == 1);
    REQUIRE(x.m.esito());
    CHECK(*x.m.esito() == 40);
    CHECK(x.m.errori().empty());
}

TEST_CASE("libreria I16: sotto il pack una chiglia attraversata a 20 m e' un urto della vela" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_I16_BASTIONE_BIANCO");
    s.aerei_asw.clear();
    EnhancedPieno x(scrivi(s, "lib_i16_urto"), 1);
    x.m.usa_meteo(Missione::ModoMeteo::realistico);
    x.avanza(2.0);
    const auto* g = x.m.ghiaccio();
    REQUIRE(g);
    // una chiglia di almeno 12 m nel bastione, e il 688 che la attraversa a 20 m
    int delta = -1;
    for (const auto& u : s.unita)
        if (u.classe == "delta3") delta = u.id;
    REQUIRE(delta >= 0);
    const Vec2 b = x.m.unita().at(delta).pos;
    std::optional<Vec2> chiglia;
    for (double dy = -10000.0; dy <= 10000.0 && !chiglia; dy += 200.0)
        for (double dx = -10000.0; dx <= 10000.0 && !chiglia; dx += 200.0) {
            const PuntoGhiaccio p = g->in({b.x + dx, b.y + dy}, x.m.tempo());
            if (p.chiglia && p.pescaggio_m >= 12.0) chiglia = Vec2{b.x + dx, b.y + dy};
        }
    REQUIRE(chiglia);
    REQUIRE(x.m.comando("posizione", {chiglia->x - 400.0, chiglia->y}));
    REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota, 20.0, 0.0}));
    for (int k = 0; k < 60 && std::fabs(x.m.giocatore()->quota - 20.0) > 1.0; ++k) {
        REQUIRE(x.m.comando("posizione", {chiglia->x - 400.0, chiglia->y}));
        x.avanza(5.0);
    }
    const std::size_t prima = x.m.urti().size();
    REQUIRE(x.m.comando("posizione", {chiglia->x + 400.0, chiglia->y}));   // di qua e di la' della chiglia in un passo
    x.avanza(1.0);
    CHECK(x.m.urti().size() > prima);
    CHECK(x.m.giocatore()->quota > 20.0);   // resta sotto la chiglia
}

// I16, l'Il-38 sul ghiaccio (regola del runtime: PercezioneAsw::acqua_libera): un datum al margine, dal lato del
// Pacifico, e uno nel bastione sotto il pack. In tutti e due le boe stanno in acqua libera: nel pack di primo anno ci
// sono polynye e canali (circa il 15% nei 10 km), e le boe vanno nella piu' vicina; l'aereo rinuncia («datum sul pack»)
// solo se tutto il datum cade sul ghiaccio
TEST_CASE("libreria I16: l'Il-38 posa le boe solo in acqua libera, anche col datum sotto il pack" * doctest::skip(!c_e())) {
    for (const bool pack : {false, true}) {
        CAPTURE(pack);
        ScenarioMissione s = scenario("LIB_I16_BASTIONE_BIANCO");
        REQUIRE(s.aerei_asw.size() == 1u);
        const auto bastione = std::find_if(s.zone.begin(), s.zone.end(), [](const ZonaScenario& z) { return z.id == "bastione"; });
        REQUIRE(bastione != s.zone.end());
        const Vec2 datum = pack ? Vec2{bastione->cerchio.x, bastione->cerchio.y} : s.aerei_asw[0].zona;
        const int aereo = s.aerei_asw[0].unita;
        RegolaScenario r;
        r.id = "datum_prova";
        CondizioneScenario c;
        c.tipo = CondizioneScenario::Tipo::tempo;
        c.valore = 10.0;
        r.quando = {c};
        AzioneScenario a;
        a.tipo = AzioneScenario::Tipo::cue_asw;
        a.punto = datum;
        a.raggio = 4000.0;
        r.allora = {a};
        s.regole.push_back(r);
        EnhancedPieno x(scrivi(s, pack ? "lib_i16_asw_pack" : "lib_i16_asw_margine"), 1);
        x.m.usa_meteo(Missione::ModoMeteo::realistico);
        bool rinuncia = false;
        for (int k = 0; k < 60 && !rinuncia; ++k) {
            x.avanza(30.0);
            if (const auto* ctl = x.m.controllore_asw(aereo))
                for (const auto& p : ctl->storia()) rinuncia = rinuncia || p.motivo == "datum sul pack";
            if (!pack && x.m.boe().size() >= 4u) break;
        }
        const auto* g = x.m.ghiaccio();
        REQUIRE(g);
        int sul_ghiaccio = 0;
        for (const auto& b : x.m.boe()) {
            const PuntoGhiaccio p = g->in(b.pos, b.t_impatto);
            sul_ghiaccio += p.ghiaccio && !p.aperta;
        }
        CHECK(sul_ghiaccio == 0);
        MESSAGE("boe posate: " << x.m.boe().size() << std::string(rinuncia ? ", datum sul pack" : ""));
        CHECK((rinuncia || x.m.boe().size() >= 1u));   // sul pack: boe nelle polynye, o la rinuncia
        if (!pack) CHECK(x.m.boe().size() >= 4u);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria S15 «L'ancoraggio»: i fanali di fonda; alla bomba a mano la squadra salpa coi fanali di moto; il rapporto e "
          "il largo; tenuto, la caccia col sonar attivo senza sparare" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S15_ANCORAGGIO");
    CHECK(s.teatro == "otranto");
    CHECK(s.situazione == "crisi");
    int kara = -1, guardia = -1, charlie = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "kara") kara = u.id;
        if (u.classe == "krivak2" && u.luci == "spente") guardia = u.id;
        if (u.classe == "charlie2") charlie = u.id;
    }
    REQUIRE(kara >= 0);
    REQUIRE(guardia >= 0);
    REQUIRE(charlie >= 0);
    const auto largo = std::find_if(s.zone.begin(), s.zone.end(), [](const ZonaScenario& z) { return z.id == "largo"; });
    REQUIRE(largo != s.zone.end());
    {   // la vittoria: l'ancoraggio coi fanali di fonda, la partenza coi fanali di moto, il rapporto, il largo
        ScenarioMissione t = s;
        ciechi(t);   // nessuno lo cerca: la prova guarda la svolta
        t.aerei_asw.clear();
        t.unita[0].rotta = std::fmod(t.unita[0].rotta + 180.0, 360.0);   // lontano dall'ancoraggio, a 3 nodi: i timoni portano
        t.unita[0].velocita = 3.0;                                        // a quota periscopica solo con un po' d'abbrivio
        EnhancedPieno x(scrivi(t, "lib_s15"), 1);
        x.avanza(2.0);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        CHECK(x.m.stato_luci(kara) == luci::Stato::fonda);   // ferma: i fanali di fonda da soli
        CHECK(x.m.stato_luci(guardia) == luci::Stato::spente);
        CHECK(x.m.unita().at(charlie).quota < 1.0);           // in superficie, affiancato
        x.avanza(3600.0);
        CHECK(var(1) == 1);                                   // la bomba a mano
        x.avanza(600.0);                                      // a 3720 s la squadra salpa
        CHECK(x.m.unita().at(kara).velocita > 5.0);
        CHECK(x.m.stato_luci(kara) == luci::Stato::moto);
        CHECK(x.m.unita().at(charlie).quota > 20.0);          // il Charlie si immerge (l'azione «quota»)
        // il rapporto di contatto a quota periscopica con l'antenna, poi il largo
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
        for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) x.avanza(10.0);
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
        x.avanza(5.0);
        CHECK(x.m.radio_trasmetti(TipoTrasmissione::contatto).fatta);
        for (int k = 0; k < 24 && var(2) == 0; ++k) x.avanza(5.0);
        CHECK(var(2) == 1);
        REQUIRE(x.m.comando("posizione", {largo->cerchio.x, largo->cerchio.y}));
        for (int k = 0; k < 12 && !x.m.esito(); ++k) x.avanza(5.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(obiettivo(x.m.esito_dettagliato(), "rapporto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(obiettivo(x.m.esito_dettagliato(), "occulto")->stato == Obiettivo::Stato::raggiunto);
        CHECK(x.m.errori().empty());
    }
    {   // tenuto: il 688 a 10 nodi a 1,5 km dalla Krivak di guardia; l'allarme, i ping su di lui, nessun lancio (crisi)
        ScenarioMissione t = s;
        t.aerei_asw.clear();
        const auto& gu = *std::find_if(t.unita.begin(), t.unita.end(), [&](const UnitaScenario& u) { return u.id == guardia; });
        t.unita[0].pos = polare_prova(gu.pos, 0.0, 1500.0);
        t.unita[0].quota = 40.0;
        t.unita[0].velocita = 10.0;
        EnhancedPieno x(scrivi(t, "lib_s15_tenuto"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        for (int k = 0; k < 180 && (x.m.tempo() < 10.0 || var(5) == 1); ++k) x.avanza(10.0);
        CHECK(var(5) == 0);
        x.avanza(600.0);
        CHECK(quanti(x.m, TipoEvento::ping_subito) > 0);
        CHECK(quanti(x.m, TipoEvento::lancio_nemico) == 0);
        CHECK(quanti(x.m, TipoEvento::lancio_ia) == 0);
        CHECK_FALSE(x.m.esito());
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria I17 «La soglia di Ekaterina»: lo sbarramento amico col varco; sotto il pack l'americano vede le PMR-2 col "
          "BQS-15, le aggira e passa dal varco; il rapporto e il Delta" *
          doctest::skip(!c_e() || !fs::exists(LIBRERIA / "LIB_I17_SOGLIA_DI_EKATERINA.lua"))) {   // fuori dalla libreria per ora
    ScenarioMissione s = scenario("LIB_I17_SOGLIA_DI_EKATERINA");
    CHECK(s.teatro == "kunashir");
    CHECK(s.meteo_realistico);
    REQUIRE(s.mine.size() >= 20);
    for (const auto& m : s.mine) {
        CHECK(m.tipo == "pmr2");
        CHECK(m.lato == "urss");
        CHECK(m.sicura);   // in sicurezza in tempo di pace: inerte, ma il BQS-15 la vede
    }
    int la = -1;
    for (const auto& u : s.unita)
        if (u.classe == "los_angeles" && !u.giocatore) la = u.id;
    REQUIRE(la >= 0);
    const auto zona_di = [&](const std::string& id) { return *std::find_if(s.zone.begin(), s.zone.end(), [&](const ZonaScenario& z) { return z.id == id; }); };
    const ZonaScenario sud = zona_di("sud");
    // l'Alfa lontano e fermo, perche' l'americano non lo senta e vada per la sua strada; il viaggio a sud subito
    ScenarioMissione t = s;
    t.aerei_asw.clear();
    t.unita[0].pos = polare_prova(t.unita[0].pos, 300.0, 40000.0);
    t.unita[0].velocita = 0.0;
    for (auto& r : t.regole)
        if (r.id == "verso_il_pacifico") r.quando[0].valore = 60.0;
    EnhancedPieno x(scrivi(t, "lib_i17"), 1);
    x.m.usa_meteo(Missione::ModoMeteo::realistico);
    const double lat_sbarramento = s.mine.front().pos.y;
    double passa_x = 0.0;
    bool passato = false, sotto = false;
    for (int k = 0; k < 600 && !passato; ++k) {
        const Vec2 prima = x.m.unita().at(la).pos;
        x.avanza(10.0);
        const Vec2 dopo = x.m.unita().at(la).pos;
        sotto = sotto || x.m.sotto_banchisa(la);
        if (prima.y > lat_sbarramento && dopo.y <= lat_sbarramento) passato = true, passa_x = dopo.x;
    }
    REQUIRE(passato);
    CHECK(sotto);
    const auto* viste = x.m.mine_scoperte_ia(la);
    REQUIRE(viste);
    CHECK(!viste->empty());   // il BQS-15 sotto la banchisa (disassemblatore, 30/10/2026)
    // passa lontano dalle mine (piu' di 300 m da ognuna): dal varco
    double vicina = 1e18;
    for (const auto& m : s.mine) vicina = std::min(vicina, std::fabs(m.pos.x - passa_x));
    MESSAGE("il Los Angeles passa lo sbarramento a " << vicina << " m dalla mina piu' vicina, a " << x.m.tempo() << " s");
    CHECK(vicina > 250.0);
    CHECK(distanza(x.m.unita().at(la).pos, Vec2{sud.cerchio.x, sud.cerchio.y}) < 20000.0);
    CHECK(x.m.errori().empty());
}

TEST_CASE("libreria S14 «Il pedinatore in attesa»: l'SSBN in superficie coi fanali, l'immersione al punto, la firma entro 5 km; "
          "trovati dalla Knox, la firma non conta" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S14_PEDINATORE_IN_ATTESA");
    CHECK(s.teatro == "celtica");
    CHECK(s.alfa);
    int ssbn = -1, knox = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "ohio") ssbn = u.id;
        if (u.classe == "knox") knox = u.id;
    }
    REQUIRE(ssbn >= 0);
    REQUIRE(knox >= 0);
    {   // la vittoria: nessuno lo cerca; dopo l'immersione l'Alfa 3 km dietro l'SSBN per mezz'ora
        ScenarioMissione t = s;
        ciechi(t);
        t.aerei_asw.clear();
        // la Knox, anche neutrale, sente l'Alfa tenuto a 3 km dall'SSBN: le regole del «trovati» le prova il blocco dopo
        std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id == "classificati" || r.id == "tenuti"; });
        EnhancedPieno x(scrivi(t, "lib_s14"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        x.avanza(2000.0);   // a 25 minuti entra, in superficie
        REQUIRE(x.m.unita().count(ssbn));
        CHECK(x.m.unita().at(ssbn).quota < 1.0);
        CHECK(x.m.stato_luci(ssbn) == luci::Stato::moto);
        for (int k = 0; k < 300 && x.m.unita().at(ssbn).quota < 50.0; ++k) x.avanza(10.0);
        CHECK(x.m.unita().at(ssbn).quota >= 50.0);   // l'azione «quota» al punto d'immersione
        CHECK(var(4) == 0);                          // prima dell'immersione la firma non conta
        while (x.m.tempo() < 9000.0 && !x.m.esito()) {
            const auto& g = *x.m.giocatore();
            const Vec2 b = x.m.unita().at(ssbn).pos;
            REQUIRE(x.m.comando("posizione", {b.x - 3000.0 * std::sin(radianti(g.rotta)), b.y - 3000.0 * std::cos(radianti(g.rotta))}));
            x.avanza(30.0);
        }
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(obiettivo(x.m.esito_dettagliato(), "firma")->stato == Obiettivo::Stato::raggiunto);
        CHECK(x.m.errori().empty());
    }
    {   // trovati: l'Alfa nella rotta della Knox; la granata, l'allarme, e la firma non conta piu'
        ScenarioMissione t = s;
        t.aerei_asw.clear();
        EnhancedPieno x(scrivi(t, "lib_s14_trovati"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        for (int k = 0; k < 400 && (x.m.tempo() < 10.0 || var(5) == 1); ++k) x.avanza(10.0);
        CHECK(var(5) == 0);
        CHECK(var(6) == 1);
        CHECK_FALSE(x.m.esito());
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria S16 «La scala»: scoperto, il segnale di emergere, le cariche di segnalazione, l'attacco al quindicesimo "
          "minuto; emerso, scortato fuori; la foto e l'uscita" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S16_SCALA");
    CHECK(s.teatro == "faroe");
    REQUIRE(s.situazione_luogo);
    CHECK(s.situazione_luogo->urss == "crisi");
    int guardia = -1, appoggio = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "oslo" && guardia < 0) guardia = u.id;   // le Oslo col Terne dal 30/10/2026
        if (u.classe == "cargo") appoggio = u.id;
    }
    REQUIRE(guardia >= 0);
    REQUIRE(appoggio >= 0);
    const auto con = [&](ScenarioMissione t, const std::string& nome, auto&& prova) {
        t.aerei_asw.clear();
        EnhancedPieno x(scrivi(t, nome), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        prova(x, var);
    };
    {   // scoperto e ostinato: la scala fino all'attacco; le fregate gli vengono addosso e in guerra sparano
        ScenarioMissione t = s;
        const auto& g = *std::find_if(t.unita.begin(), t.unita.end(), [&](const UnitaScenario& u) { return u.id == guardia; });
        t.unita[0].pos = polare_prova(g.pos, 0.0, 1500.0);
        t.unita[0].velocita = 12.0;   // rumoroso: le fregate lo tengono, e la scala non si ferma
        con(t, "lib_s16_scala", [&](EnhancedPieno& x, auto&& var) {
            for (int k = 0; k < 120 && var(7) == 0; ++k) x.avanza(10.0);
            REQUIRE(var(7) == 1);   // scoperto: il segnale di emergere
            const double t0 = x.m.tempo();
            const int eventi0 = static_cast<int>(x.m.eventi_missione().size());
            while (x.m.tempo() < t0 + 1000.0 && var(7) == 1) x.avanza(10.0);
            CHECK(var(7) == 2);     // dopo 15 minuti, l'ultimo gradino
            CHECK(x.m.tempo() - t0 > 850.0);
            CHECK(var(10) == 0);
            // dalla scoperta le fregate gli vengono addosso e pingano; in guerra sparano solo a portata. L'Alfa tenuto a 1 km
            // dalla fregata di guardia per 30 minuti, dentro la fascia del Terne (425-1600 m): le Oslo tirano (dal 30/10/2026,
            // quando il runtime ha fatto nemico del luogo chi e' in crisi o in guerra con lui; prima non tiravano mai)
            // Tre salve quasi a contatto possono affondarlo (il kit, 31/10/2026: e' il modello dei danni, e dipende dal
            // caso): finita la missione o perso il battello, il conto si chiude li'
            for (int k = 0; k < 30; ++k) {
                if (x.m.esito() || !x.m.giocatore() || x.m.giocatore()->distrutta) break;
                const Vec2 q = x.m.unita().at(guardia).pos;
                REQUIRE(x.m.comando("posizione", {q.x, q.y + 1000.0}));
                x.avanza(60.0);
            }
            const auto terne = std::count_if(x.m.colpi().begin(), x.m.colpi().end(),
                                             [](const Colpo& c) { return c.tipo == Colpo::Tipo::attacco && c.classe_lanciatore == "oslo"; });
            MESSAGE("S16: salve del Terne sull'Alfa a 1 km in 30 minuti: ", terne);
            CHECK(terne > 0);
            CHECK(static_cast<int>(x.m.eventi_missione().size()) > eventi0);
            CHECK(quanti(x.m, TipoEvento::ping_subito) > 0);
            CHECK(x.m.errori().empty());
        });
    }
    {   // emerso dopo il segnale: scortato fuori, esito «richiamato»
        ScenarioMissione t = s;
        const auto& g = *std::find_if(t.unita.begin(), t.unita.end(), [&](const UnitaScenario& u) { return u.id == guardia; });
        t.unita[0].pos = polare_prova(g.pos, 0.0, 1500.0);
        t.unita[0].velocita = 3.0;
        con(t, "lib_s16_emersi", [&](EnhancedPieno& x, auto&& var) {
            for (int k = 0; k < 120 && var(7) == 0; ++k) x.avanza(10.0);
            REQUIRE(var(7) == 1);
            REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::emersione_emergenza}));
            for (int k = 0; k < 60 && !x.m.esito(); ++k) x.avanza(10.0);
            REQUIRE(x.m.esito());
            CHECK(*x.m.esito() == 43);
            CHECK(x.m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::richiamato);
        });
    }
    {   // la vittoria, senza nessuno che cerchi: la foto da meno di 2 km e il largo
        ScenarioMissione t = s;
        ciechi(t);
        std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id.rfind("classificati", 0) == 0 || r.id.rfind("tenuti", 0) == 0; });
        con(t, "lib_s16_foto", [&](EnhancedPieno& x, auto&& var) {
            x.avanza(2.0);
            const Vec2 a = x.m.unita().at(appoggio).pos;
            REQUIRE(x.m.comando("posizione", {a.x, a.y - 1200.0}));
            REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
            for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) {
                REQUIRE(x.m.comando("posizione", {a.x, a.y - 1200.0}));
                x.avanza(10.0);
            }
            REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::periscopio, 0.0, 0.0, 1}));
            x.avanza(20.0);   // il periscopio sale
            CHECK(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::foto_periscopio, 0.0, 0.0, 0}));   // a nord, sulla nave appoggio
            x.avanza(10.0);
            const auto largo = std::find_if(s.zone.begin(), s.zone.end(), [](const ZonaScenario& z) { return z.id == "largo"; });
            REQUIRE(x.m.comando("posizione", {largo->cerchio.x, largo->cerchio.y}));
            for (int k = 0; k < 12 && !x.m.esito(); ++k) x.avanza(5.0);
            REQUIRE(x.m.esito());
            CHECK(*x.m.esito() == 40);
            CHECK(x.m.errori().empty());
            (void)var;
        });
    }
}

TEST_CASE("libreria S17 «Sotto la prora»: il gruppo rientra a 20 minuti e accosta a 40; un'ora sulla portaerei e il rapporto; "
          "a meno di 1000 m dalla prora l'obiettivo cade" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S17_SOTTO_LA_PRORA");
    CHECK(s.teatro == "kunashir");
    CHECK(s.meteo_realistico);
    int cv = -1;
    for (const auto& u : s.unita)
        if (u.classe == "kitty_hawk") cv = u.id;
    REQUIRE(cv >= 0);
    ScenarioMissione t = s;
    ciechi(t);
    t.aerei_asw.clear();
    EnhancedPieno x(scrivi(t, "lib_s17"), 1);
    const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
    x.avanza(600.0);
    CHECK(distanza(x.m.unita().at(cv).pos, x.m.giocatore()->pos) > 60000.0);   // ancora fuori dal teatro
    x.avanza(700.0);                                                           // a 20 minuti rientra
    CHECK(distanza(x.m.unita().at(cv).pos, x.m.giocatore()->pos) < 60000.0);
    CHECK(var(6) == 1);
    // un'ora 5 km a poppa della portaerei, con la svolta di mezzo
    const auto dietro = [&](double metri, double secondi) {
        const double fino = x.m.tempo() + secondi;
        while (x.m.tempo() < fino && !x.m.esito()) {
            const auto& c = x.m.unita().at(cv);
            REQUIRE(x.m.comando("posizione", {c.pos.x - metri * std::sin(radianti(c.rotta)), c.pos.y - metri * std::cos(radianti(c.rotta))}));
            x.avanza(30.0);
        }
    };
    dietro(5000.0, 3700.0);
    CHECK(obiettivo(x.m.esito_dettagliato(), "tenuta")->stato == Obiettivo::Stato::raggiunto);
    CHECK(var(6) == 1);   // sempre a poppa: la prora non e' mai stata a meno di 1000 m
    REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
    for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) dietro(5000.0, 10.0);   // gli alberi escono a quota periscopica
    REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
    x.avanza(5.0);
    CHECK(x.m.radio_trasmetti(TipoTrasmissione::contatto).fatta);
    for (int k = 0; k < 24 && !x.m.esito(); ++k) x.avanza(5.0);
    REQUIRE(x.m.esito());
    CHECK(*x.m.esito() == 40);
    CHECK(x.m.errori().empty());
    {   // sulla prora: 600 m davanti alla portaerei, a 100 m (sotto la chiglia), l'obiettivo «prora» cade
        EnhancedPieno y(scrivi(t, "lib_s17_prora"), 1);
        y.avanza(1300.0);
        const auto& c = y.m.unita().at(cv);
        REQUIRE(y.m.comando("posizione", {c.pos.x + 600.0 * std::sin(radianti(c.rotta)), c.pos.y + 600.0 * std::cos(radianti(c.rotta))}));
        y.avanza(30.0);
        const auto it = y.m.variabili().find(306);
        CHECK((it == y.m.variabili().end() ? 0 : it->second) == 0);
    }
}

TEST_CASE("libreria S18 «L'arcipelago»: le mine comandate in sicurezza; la foto della base e il largo; scoperti nelle acque "
          "interne, l'arcipelago chiuso e la mina che scoppia sotto il 688" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S18_ARCIPELAGO");
    CHECK(s.teatro == "baltico");
    REQUIRE(s.situazione_luogo);
    CHECK(s.situazione_luogo->usa == "crisi");
    REQUIRE(s.mine.size() >= 20);
    for (const auto& m : s.mine) {
        CHECK(m.comandata);
        CHECK(m.gruppo == "ingresso");
    }
    int ormeggiata = -1, interna = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "stockholm" && u.velocita == 0.0) ormeggiata = u.id;
        if (u.classe == "stockholm" && u.velocita > 0.0 && interna < 0 && u.ruolo.find("interne") != std::string::npos) interna = u.id;
    }
    REQUIRE(ormeggiata >= 0);
    REQUIRE(interna >= 0);
    REQUIRE(s.aerei_asw.size() == 1);   // l'elicottero antisom e' l'Hkp 4 col sonar a immersione (dal 30/10/2026)
    CHECK(std::find_if(s.unita.begin(), s.unita.end(), [&](const UnitaScenario& u) { return u.id == s.aerei_asw[0].unita; })->classe == "hkp4");
    const auto zona_di = [&](const std::string& id) { return *std::find_if(s.zone.begin(), s.zone.end(), [&](const ZonaScenario& z) { return z.id == id; }); };
    {   // la vittoria, senza nessuno che cerchi: 3 km a sud della corvetta ormeggiata, la foto verso nord, il largo
        ScenarioMissione t = s;
        ciechi(t);
        t.aerei_asw.clear();
        std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id.rfind("classificati", 0) == 0 || r.id.rfind("tenuti", 0) == 0; });
        EnhancedPieno x(scrivi(t, "lib_s18"), 1);
        x.avanza(60.0);   // le mine comandate posate: restano in sicurezza
        const Vec2 b = x.m.unita().at(ormeggiata).pos;
        const auto tieni = [&] { REQUIRE(x.m.comando("posizione", {b.x, b.y - 2500.0})); };
        tieni();
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
        for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) { tieni(); x.avanza(10.0); }
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::scegli_periscopio, 0.0, 0.0, 1}));   // il Type 18, che fotografa
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::periscopio, 0.0, 0.0, 1}));
        x.avanza(20.0);
        CHECK(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::foto_periscopio, 0.0, 0.0, 0}));
        x.avanza(10.0);
        const ZonaScenario largo = zona_di("largo");
        REQUIRE(x.m.comando("posizione", {largo.cerchio.x, largo.cerchio.y}));
        for (int k = 0; k < 12 && !x.m.esito(); ++k) x.avanza(5.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(quanti(x.m, TipoEvento::mina_esplosa) == 0);
        CHECK(x.m.errori().empty());
    }
    {   // scoperti nelle acque interne: chiusa l'area, le mine alla stazione; sopra una mina, lo scoppio
        ScenarioMissione t = s;
        t.aerei_asw.clear();
        const auto& in = *std::find_if(t.unita.begin(), t.unita.end(), [&](const UnitaScenario& u) { return u.id == interna; });
        t.unita[0].pos = polare_prova(in.pos, 180.0, 1500.0);
        t.unita[0].velocita = 10.0;
        EnhancedPieno x(scrivi(t, "lib_s18_chiusa"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        for (int k = 0; k < 120 && (x.m.tempo() < 20.0 || var(7) == 1); ++k) x.avanza(10.0);
        REQUIRE(var(7) == 0);   // l'arcipelago chiuso
        CHECK(var(5) == 0);
        // il 688 sopra una mina dell'ingresso, a 15 m: la stazione la fa scoppiare dopo 10-30 s
        const Vec2 mina = s.mine[s.mine.size() / 2].pos;
        for (int k = 0; k < 8 && quanti(x.m, TipoEvento::mina_esplosa) == 0; ++k) {
            REQUIRE(x.m.comando("posizione", {mina.x, mina.y}));
            x.avanza(10.0);
        }
        CHECK(quanti(x.m, TipoEvento::mina_esplosa) > 0);
        CHECK(x.m.errori().empty());
    }
}

TEST_CASE("libreria S19 «Pietra santa»: 40 minuti sotto la coda entro 4 km; le 12 miglia e i 500 m" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S19_PIETRA_SANTA");
    CHECK(s.teatro == "barents");
    int ssbn = -1;
    for (const auto& u : s.unita)
        if (u.classe == "delta3") ssbn = u.id;
    REQUIRE(ssbn >= 0);
    ScenarioMissione t = s;
    ciechi(t);
    t.aerei_asw.clear();
    std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id.rfind("classificati", 0) == 0; });
    EnhancedPieno x(scrivi(t, "lib_s19"), 1);
    x.m.usa_crazy_ivan(false);   // la prova misura la firma: niente accostate dell'SSBN sul 688 tenuto a poppa
    const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
    x.avanza(20.0);
    while (x.m.tempo() < 3000.0 && !x.m.esito()) {
        const auto& b = x.m.unita().at(ssbn);
        REQUIRE(x.m.comando("posizione", {b.pos.x - 2000.0 * std::sin(radianti(b.rotta)), b.pos.y - 2000.0 * std::cos(radianti(b.rotta))}));
        x.avanza(30.0);
    }
    REQUIRE(x.m.esito());
    CHECK(*x.m.esito() == 40);
    CHECK(var(6) == 1);
    CHECK(var(7) == 1);
    CHECK(x.m.errori().empty());
    {   // dentro le 12 miglia, e a 400 m dall'SSBN: i due obiettivi cadono
        EnhancedPieno y(scrivi(t, "lib_s19_limiti"), 1);
        y.avanza(20.0);
        const auto dodici = std::find_if(s.zone.begin(), s.zone.end(), [](const ZonaScenario& z) { return z.id == "acque_territoriali"; });
        REQUIRE(dodici != s.zone.end());
        REQUIRE(y.m.comando("posizione", {dodici->cerchio.x, dodici->cerchio.y + dodici->cerchio.r - 2000.0}));
        y.avanza(20.0);
        const auto& b = y.m.unita().at(ssbn);
        REQUIRE(y.m.comando("posizione", {b.pos.x + 400.0, b.pos.y}));
        y.avanza(20.0);
        const auto v = [&](int i) { const auto it = y.m.variabili().find(300 + i); return it == y.m.variabili().end() ? 0 : it->second; };
        CHECK(v(6) == 0);
        CHECK(v(7) == 0);
    }
}

TEST_CASE("libreria S20 «Cinquecento iarde»: la firma piu' in fretta da vicino; sotto le 500 iarde l'obiettivo cade; il "
          "nuovo schema dopo un'ora" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S20_CINQUECENTO_IARDE");
    CHECK(s.teatro == "celtica");
    int ssbn = -1;
    for (const auto& u : s.unita)
        if (u.classe == "delta3") ssbn = u.id;
    REQUIRE(ssbn >= 0);
    ScenarioMissione t = s;
    ciechi(t);
    t.aerei_asw.clear();
    std::erase_if(t.regole, [](const RegolaScenario& r) { return r.id.rfind("classificati", 0) == 0; });
    const auto tieni = [&](EnhancedPieno& x, double metri, double secondi) {
        const double fino = x.m.tempo() + secondi;
        while (x.m.tempo() < fino && !x.m.esito()) {
            const auto& b = x.m.unita().at(ssbn);
            REQUIRE(x.m.comando("posizione", {b.pos.x - metri * std::sin(radianti(b.rotta)), b.pos.y - metri * std::cos(radianti(b.rotta))}));
            x.avanza(30.0);
        }
    };
    {   // 1,5 km a poppa: 30 a ogni controllo, la firma in dieci minuti
        EnhancedPieno x(scrivi(t, "lib_s20_vicino"), 1);
        x.m.usa_crazy_ivan(false);
        x.avanza(20.0);
        tieni(x, 1500.0, 1500.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(x.m.tempo() < 900.0);
        CHECK(x.m.errori().empty());
    }
    {   // 4 km a poppa: 10 a ogni controllo, ci vuole mezz'ora
        EnhancedPieno x(scrivi(t, "lib_s20_lontano"), 1);
        x.m.usa_crazy_ivan(false);
        x.avanza(20.0);
        tieni(x, 4000.0, 2400.0);
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == 40);
        CHECK(x.m.tempo() > 1700.0);
    }
    {   // a 400 m l'obiettivo delle 500 iarde cade; dopo un'ora l'SSBN scende a 130 m
        EnhancedPieno y(scrivi(t, "lib_s20_schema"), 1);
        y.avanza(20.0);
        const auto& b = y.m.unita().at(ssbn);
        REQUIRE(y.m.comando("posizione", {b.pos.x + 400.0, b.pos.y}));
        y.avanza(20.0);
        const auto it = y.m.variabili().find(307);
        CHECK((it == y.m.variabili().end() ? 0 : it->second) == 0);
        const auto& g0 = *y.m.giocatore();
        REQUIRE(y.m.comando("posizione", {g0.pos.x + 30000.0, g0.pos.y}));   // lontano: la firma non sale
        y.avanza(3800.0);
        CHECK(y.m.unita().at(ssbn).quota > 95.0);       // scende (fino a dove il fondo lo lascia: l'IA tiene il margine)
        CHECK(y.m.unita().at(ssbn).velocita < 4.5);     // e rallenta (da 5 verso 3 nodi, a scatti di telegrafo)
    }
}

TEST_CASE("libreria S25 «Ombra del boomer»: in coda all'SSBN fino all'emergenza; il rapporto, il rimorchio che non tiene, il "
          "richiamo; sotto le 1000 iarde la versione sovietica" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S25_OMBRA_DEL_BOOMER");
    CHECK(s.teatro == "celtica");
    int ssbn = -1, cargo = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "delta3") ssbn = u.id;
        if (u.classe == "mercantile" && u.parte == "urss") cargo = u.id;
    }
    REQUIRE(ssbn >= 0);
    REQUIRE(cargo >= 0);
    ScenarioMissione t = s;
    ciechi(t);
    const auto tieni = [&](EnhancedPieno& x, double metri, auto&& fino) {   // a poppa dell'SSBN finche' fino() e' falso
        while (!fino() && !x.m.esito() && x.m.tempo() < s.durata_s) {
            const auto& b = x.m.unita().at(ssbn);
            REQUIRE(x.m.comando("posizione", {b.pos.x - metri * std::sin(radianti(b.rotta)), b.pos.y - metri * std::cos(radianti(b.rotta))}));
            x.avanza(30.0);
        }
    };
    {   // la partita intera: 2,5 km di poppa, l'emergenza dopo le due ore; il rapporto da 12 km; il rimorchio, il richiamo
        EnhancedPieno x(scrivi(t, "lib_s25"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        x.m.usa_crazy_ivan(false);
        x.avanza(20.0);
        tieni(x, 2500.0, [&] { return var(10) >= 1; });
        MESSAGE("S25: l'emergenza a ", x.m.tempo(), " s");
        REQUIRE(var(10) >= 1);
        CHECK(x.m.tempo() >= 7200.0);
        CHECK(x.m.tempo() < 12700.0);
        CHECK(var(4) == 1);
        CHECK(var(8) == 1);
        // lontano dalle navi che arrivano (da sud-est): 12 km a nord-ovest dell'SSBN, a quota periscopica, il rapporto
        const Vec2 b = x.m.unita().at(ssbn).pos;
        const Vec2 qui = polare_prova(b, 315.0, 12000.0);
        REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
        for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) {
            REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
            x.avanza(10.0);
        }
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
        x.avanza(5.0);
        CHECK(x.m.radio_trasmetti(TipoTrasmissione::situazione).fatta);
        x.avanza(20.0);
        CHECK(var(6) == 1);
        for (int k = 0; k < 400 && var(10) < 2 && !x.m.esito(); ++k) {   // l'SSBN emerge e si ferma; il mercantile arriva e lo prende
            REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
            x.avanza(30.0);
        }
        MESSAGE("S25: il rimorchio a ", x.m.tempo(), " s");
        CHECK(var(10) == 2);
        CHECK(x.m.in_avaria(ssbn));
        CHECK(x.m.rimorchiatore_di(ssbn) == cargo);
        while (!x.m.esito() && x.m.tempo() < s.durata_s) {
            REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
            x.avanza(30.0);
        }
        REQUIRE(x.m.esito());
        MESSAGE("S25: esito ", *x.m.esito(), " a ", x.m.tempo(), " s");
        CHECK(*x.m.esito() == 40);
        CHECK(var(10) == 3);
        CHECK(var(7) == 1);
        CHECK(x.m.rimorchiatore_di(ssbn) < 0);
        const auto e = x.m.esito_dettagliato();
        CHECK(obiettivo(e, "riferito")->stato == Obiettivo::Stato::raggiunto);
        CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
        CHECK(x.m.errori().empty());
    }
    {   // a 800 m dall'SSBN prima dell'emergenza: l'obiettivo delle 1000 iarde cade (poi la versione sovietica)
        EnhancedPieno y(scrivi(t, "lib_s25_vicino"), 1);
        y.avanza(20.0);
        const auto& b = y.m.unita().at(ssbn);
        REQUIRE(y.m.comando("posizione", {b.pos.x + 800.0, b.pos.y}));
        y.avanza(20.0);
        const auto it = y.m.variabili().find(308);
        CHECK((it == y.m.variabili().end() ? 0 : it->second) == 0);
    }
}

// Chi passa cambia col seme e con i cambi all'ASW (04/11/2026, il radar degli aerei; misure in tools/re/prove/s22_esca e T12 par. 5).
// Qui due semi soli, e l'esito che segue i liberi: seme 1 a 20 nodi, seme 2 a 4 nodi. Quello che vale per il seme 2 non vale per tutti:
// a 4 nodi la Kidd tiene un Victor, tardi, nei semi 8, 14, 21 e 30. Il giocatore va dritto: lo scatto vicino a un Victor inseguito,
// misurato a parte, non libera i Victor ed e' stato tolto dal briefing (05/11/2026)
TEST_CASE("libreria S22 «La rotta lunga»: seme 1 a 20 nodi: Kidd e Los Angeles tengono l'Alfa; seme 2 a 4 nodi: la Kidd non tiene i battelli sovietici; "
          "l'esito segue i battelli liberi" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S22_ROTTA_LUNGA");
    CHECK(s.teatro == "celtica");
    CHECK(s.alfa);
    int victor = 0, cacciatori = 0;
    for (const auto& u : s.unita) {
        victor += u.classe == "victor3";
        cacciatori += u.parte == "usa" && u.classe != "ohio";
    }
    CHECK(victor == 4);
    CHECK(cacciatori == 5);
    constexpr int ALFA = 0, KIDD = 9, LA_1 = 10, LA_2 = 15;
    const auto gioca = [&](double nodi, std::uint64_t seme, const std::string& nome, const auto& controlla) {   // il giocatore tiene rotta e velocita' per 4 ore
        ScenarioMissione t = s;
        t.unita[0].velocita = nodi;
        EnhancedPieno x(scrivi(t, nome), seme);
        x.avanza(s.durata_s);
        const auto it = x.m.variabili().find(307);
        const int liberi = it == x.m.variabili().end() ? 0 : it->second;
        MESSAGE("S22 a ", nodi, " nodi, seme ", seme, ": liberi ", liberi, ", esito ", x.m.esito() ? *x.m.esito() : -1);
        CHECK(x.m.errori().empty());
        REQUIRE(x.m.esito());
        CHECK(*x.m.esito() == (liberi >= 3 ? 40 : 41));
        const auto tiene = [&](int cacciatore, int battello) { return x.m.contatto_cumulato_s(cacciatore, battello) >= 70.0 * 60.0; };
        controlla(tiene);
        return *x.m.esito();
    };
    // i due semi coprono oggi i due esiti; se un cambio li sposta, il controllo sull'esito resta vero ma qui si vede
    const int di_corsa = gioca(20.0, 1, "lib_s22_esca", [&](const auto& tiene) {
        CHECK(tiene(KIDD, ALFA));
        CHECK(tiene(LA_1, ALFA));
        CHECK(tiene(LA_2, ALFA));
    });
    const int piano = gioca(4.0, 2, "lib_s22_nascosto", [&](const auto& tiene) {
        for (const auto& u : s.unita)
            if (u.parte == "urss") CHECK_MESSAGE(!tiene(KIDD, u.id), u.id);
    });
    if (di_corsa == piano) MESSAGE("S22: i due semi danno lo stesso esito, ", di_corsa, ": uno dei due rami non e' piu' coperto");
}
TEST_CASE("libreria S23 «Acqua a due gradi»: la posizione data in tempo salva tutti; senza, l'Il-38 e il peschereccio tardi; "
          "l'aiuto norvegese chiesto" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S23_ACQUA_A_DUE_GRADI");
    CHECK(s.teatro == "barents");
    CHECK(s.alfa);
    int battello = -1, bmrt = -1, kv = -1;
    for (const auto& u : s.unita) {
        if (u.classe == "akula") battello = u.id;
        if (u.classe == "bmrt") bmrt = u.id;
        if (u.classe == "oslo") kv = u.id;
    }
    REQUIRE(battello >= 0);
    REQUIRE(bmrt >= 0);
    REQUIRE(kv >= 0);
    const auto trasmetti = [&](EnhancedPieno& x, Vec2 qui, TipoTrasmissione tipo) {   // a quota periscopica, l'antenna, la trasmissione
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::quota_periscopica}));
        for (int k = 0; k < 60 && x.m.giocatore()->quota > 6.5; ++k) {
            REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
            x.avanza(10.0);
        }
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
        x.avanza(5.0);
        CHECK(x.m.radio_trasmetti(tipo).fatta);
    };
    const auto fino_alla_fine = [&](EnhancedPieno& x, Vec2 qui) {
        while (!x.m.esito() && x.m.tempo() < s.durata_s) {
            REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
            x.avanza(30.0);
        }
    };
    {   // l'Alfa a 3 km in 50 minuti, il rapporto: il peschereccio arriva prima che il battello vada giu'
        EnhancedPieno x(scrivi(s, "lib_s23_pronto"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        x.avanza(3000.0);
        const Vec2 qui = polare_prova(x.m.unita().at(battello).pos, 180.0, 3000.0);
        REQUIRE(x.m.comando("posizione", {qui.x, qui.y}));
        x.avanza(10.0);
        CHECK(var(2) == 0);   // a 3 km: non ancora entro 2
        trasmetti(x, qui, TipoTrasmissione::situazione);
        x.avanza(20.0);
        CHECK(var(3) == 1);
        fino_alla_fine(x, qui);
        REQUIRE(x.m.esito());
        MESSAGE("S23 pronto: esito ", *x.m.esito(), " a ", x.m.tempo(), " s, salvati ", var(5));
        CHECK(*x.m.esito() == 40);
        CHECK(var(5) == 40);
        CHECK(x.m.salvati().salvati == 40);   // m688.salvati: l'achievement «tutti salvati» e la carriera
        CHECK(x.m.salvati().dispersi == 40);
        CHECK(var(4) == 1);
        CHECK(x.m.unita().at(battello).quota < 1.0);   // e' rimasto in superficie (la nave arriva prima dell'affondamento)
        CHECK(x.m.errori().empty());
    }
    {   // nessun rapporto: l'Il-38 da' la posizione mezz'ora dopo, il peschereccio arriva tardi
        EnhancedPieno x(scrivi(s, "lib_s23_tardi"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        x.avanza(10.0);
        fino_alla_fine(x, polare_prova(x.m.unita().at(battello).pos, 180.0, 25000.0));   // l'Alfa resta lontano
        MESSAGE("S23 senza rapporto: esito ", x.m.esito() ? *x.m.esito() : -1, " a ", x.m.tempo(), " s, salvati ", var(5));
        CHECK(var(3) == 0);
        CHECK(var(5) == 15);   // come nel 1989: la nave arriva quasi un'ora dopo
        CHECK(x.m.salvati().salvati == 15);
        CHECK((!x.m.unita().count(battello) || x.m.unita().at(battello).persa));   // affondato fuori dalla vista: «togli unita'»
    }
    {   // nessun rapporto, ma l'aiuto norvegese chiesto subito: la nave arriva prima del peschereccio
        EnhancedPieno x(scrivi(s, "lib_s23_norvegia"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        x.avanza(9010.0);
        const Vec2 qui = polare_prova(x.m.unita().at(battello).pos, 180.0, 20000.0);
        trasmetti(x, qui, TipoTrasmissione::richiesta);
        x.avanza(20.0);
        CHECK(var(12) == 1);
        fino_alla_fine(x, qui);
        MESSAGE("S23 con la Norvegia: esito ", x.m.esito() ? *x.m.esito() : -1, " a ", x.m.tempo(), " s, salvati ", var(5));
        CHECK(var(5) > 15);   // chiedere aiuto ne salva piu' che aspettare il peschereccio (35 col seme 1)
    }
}
TEST_CASE("libreria S24 «Quota zero»: lo Sturgeon in coda all'Alfa; col Crazy Ivan lo si sente e lo si classifica; pingato "
          "se ne va; l'urto chiude la missione" * doctest::skip(!c_e())) {
    ScenarioMissione s = scenario("LIB_S24_QUOTA_ZERO");
    CHECK(s.teatro == "kunashir");
    CHECK(s.alfa);
    CHECK(s.esercitazione);
    int st = -1;
    for (const auto& u : s.unita)
        if (u.classe == "sturgeon") st = u.id;
    REQUIRE(st >= 0);
    {   // l'Alfa tiene la rotta col Crazy Ivan ogni 5 minuti: lo Sturgeon si mette in coda, vicino, e si sente; classificato
        // e pingato, se ne va
        EnhancedPieno x(scrivi(s, "lib_s24"), 1);
        const auto var = [&](int i) { const auto it = x.m.variabili().find(300 + i); return it == x.m.variabili().end() ? 0 : it->second; };
        double minima = 1e9;
        for (int k = 0; k < 160 && x.m.contatto_cumulato_s(0, st) < 120.0; ++k) {   // 3 gradi al secondo per un minuto
            if (k % 20 == 0) REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::timone, 3.0}));
            if (k % 20 == 4) REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::timone, 0.0}));
            x.avanza(15.0);
            minima = std::min(minima, distanza(x.m.unita().at(st).pos, x.m.giocatore()->pos));
        }
        MESSAGE("S24: due minuti di contatto a ", x.m.tempo(), " s, lo Sturgeon fino a ", minima, " m");
        CHECK(minima < 1500.0);
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::timone, 0.0}));
        CHECK(obiettivo(x.m.esito_dettagliato(), "sentito")->stato == Obiettivo::Stato::raggiunto);   // il primo passo
        CHECK(var(2) == 0);
        REQUIRE(x.m.applica(ordine_classifica(x.m.tempo(), st, "sturgeon")));   // la classificazione a mano, giusta
        x.avanza(30.0);
        CHECK(var(2) == 1);
        REQUIRE(x.m.applica(Ordine{x.m.tempo(), TipoOrdine::impulso_sonar}));
        x.avanza(30.0);
        CHECK(var(3) == 1);
        for (int k = 0; k < 120 && var(4) == 0; ++k) x.avanza(30.0);
        MESSAGE("S24: allontanato a ", x.m.tempo(), " s, a ", distanza(x.m.unita().at(st).pos, x.m.giocatore()->pos), " m");
        CHECK(var(4) == 1);
        CHECK_FALSE(x.m.esito());
        CHECK(x.m.errori().empty());
    }
    {   // l'urto: l'Alfa nella posizione e alla quota dello Sturgeon, esito 41
        EnhancedPieno y(scrivi(s, "lib_s24_urto"), 1);
        y.avanza(900.0);
        const UnitaClassica b = y.m.unita().at(st);
        UnitaClassica* g = y.m.trova(0);
        REQUIRE(g);
        g->pos = b.pos;
        g->quota = g->quota_ordinata = b.quota;
        for (int k = 0; k < 20 && !y.m.esito(); ++k) y.avanza(1.0);
        REQUIRE(y.m.esito());
        CHECK(*y.m.esito() == 41);
    }
}
