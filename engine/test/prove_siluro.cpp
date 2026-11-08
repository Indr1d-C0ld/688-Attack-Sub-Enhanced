#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <optional>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/siluro.hpp"

using namespace m688;

// Siluri dell'Enhanced (fase C6b; dossier 01 §6.1, 02 §6.2-6.4, 05 §5)

namespace {

constexpr double DT = 1.0 / 9.0;

const TabellaSiluri& tabella() {
    static const TabellaSiluri t = carica_siluri(M688_DATI "/siluri.toml");
    return t;
}
std::vector<std::string> classi_siluro() {
    std::vector<std::string> v;
    for (const auto& [c, d] : tabella().siluri) v.push_back(c);
    return v;
}

const DatiSiluro* dati_siluro(const std::string& classe) { return tabella().siluro(classe); }

SorgenteAcustica sottomarino(int id, Vec2 pos, double quota, double rotta = 0.0, double kn = 0.0, double db = 130.0) {
    SorgenteAcustica s;
    s.id = id; s.pos = pos; s.quota = quota; s.rotta = rotta; s.velocita = kn * NODO; s.livello_db = db;
    s.forza_bersaglio_db = 15.0; s.lunghezza_m = 100.0;
    return s;
}

// muove le sorgenti in moto rettilineo
void muovi(std::vector<SorgenteAcustica>& v, double dt) {
    for (auto& s : v)
        if (!s.esca) s.pos = {s.pos.x + s.velocita * std::sin(radianti(s.rotta)) * dt, s.pos.y + s.velocita * std::cos(radianti(s.rotta)) * dt};
}

struct Esito {
    std::vector<EventoSiluro> eventi;
    bool ha(EventoSiluro::Tipo t, int sorgente = -2) const {
        for (const auto& e : eventi)
            if (e.tipo == t && (sorgente == -2 || e.sorgente == sorgente)) return true;
        return false;
    }
};

Esito corri(Siluro& s, std::vector<SorgenteAcustica>& v, double secondi, const std::vector<double>& strati = {},
            std::optional<Lanciatore> l = std::nullopt) {
    Esito e;
    for (int i = 0; i < static_cast<int>(secondi / DT) && s.attivo(); ++i) {
        s.avanza(DT, v, strati, l);
        muovi(v, DT);
        for (auto& x : s.eventi()) e.eventi.push_back(x);
    }
    return e;
}

ImpostazioniSiluro imp(double rotta, ModoTesta modo = ModoTesta::passivo, double rte = 1000.0) {
    ImpostazioniSiluro i;
    i.rotta = rotta; i.modo = modo; i.rte_m = rte; i.quota_corsa_m = 100.0; i.quota_ricerca_m = 100.0;
    return i;
}

}  // namespace

TEST_CASE("siluro: tabella delle classi con fonti e prestazioni d'epoca") {
    for (const auto& c : classi_siluro()) {
        const auto* d = dati_siluro(c);
        REQUIRE(d != nullptr);
        CHECK(d->velocita_alta_kn >= d->velocita_bassa_kn);
        CHECK(d->portata_bassa_m >= d->portata_alta_m);   // in bassa velocita' va piu' lontano
        CHECK_FALSE(d->fonte.empty());
    }
    const auto* mk48 = dati_siluro("mk48");
    REQUIRE(mk48);
    CHECK(mk48->velocita_alta_kn == doctest::Approx(55.0));   // Almanac of Seapower 1987: 55 kn / ~38 km
    CHECK(mk48->portata_alta_m == doctest::Approx(38000.0));
    CHECK(mk48->filoguidato);
    CHECK(dati_siluro("type53_65k")->segui_scia);
    CHECK(dati_siluro("inesistente") == nullptr);
}

TEST_CASE("siluro: corsa dritta fino all'RTE, poi la testa si attiva") {
    Siluro s(*dati_siluro("mk48"), imp(90.0, ModoTesta::passivo, 3000.0), {0, 0}, 50.0, 0, 1);
    std::vector<SorgenteAcustica> v;
    auto e = corri(s, v, 100.0);   // 55 kn = 28,3 m/s: l'RTE di 3 km arriva dopo ~106 s
    CHECK(s.fase() == FaseSiluro::corsa);
    CHECK(s.rotta() == doctest::Approx(90.0));
    CHECK(s.pos().y == doctest::Approx(0.0).epsilon(1e-6));
    CHECK(s.quota() == doctest::Approx(100.0));   // quota di corsa
    e = corri(s, v, 10.0);
    CHECK(s.fase() == FaseSiluro::ricerca);
    CHECK(e.ha(EventoSiluro::Tipo::attivazione));
    CHECK(s.percorso_m() >= 3000.0);
}

TEST_CASE("siluro: serpentina di +-30 gradi attorno all'asse di ricerca") {
    Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 500.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> v;
    corri(s, v, 25.0);
    REQUIRE(s.fase() == FaseSiluro::ricerca);
    double minimo = 1e9, massimo = -1e9;
    for (int i = 0; i < 9 * 120; ++i) {
        s.avanza(DT, v, {}, std::nullopt);
        const double r = normalizza_180(s.rotta());
        minimo = std::min(minimo, r); massimo = std::max(massimo, r);
    }
    CHECK(massimo == doctest::Approx(Siluro::SERPENTINA_GRADI).epsilon(0.1));
    CHECK(minimo == doctest::Approx(-Siluro::SERPENTINA_GRADI).epsilon(0.1));
}

TEST_CASE("siluro: aggancio passivo, inseguimento con anticipo e spoletta di prossimita'") {
    Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 1000.0), {0, 0}, 100.0, 0, 7);
    std::vector<SorgenteAcustica> v{sottomarino(5, {300, 4000}, 150.0, 90.0, 8.0, 130.0)};
    const auto e = corri(s, v, 600.0);
    CHECK(e.ha(EventoSiluro::Tipo::attivazione));
    CHECK(e.ha(EventoSiluro::Tipo::aggancio, 5));
    CHECK(e.ha(EventoSiluro::Tipo::esplosione, 5));
    CHECK(s.fase() == FaseSiluro::esploso);
    CHECK(s.bersaglio() == 5);
}

TEST_CASE("siluro: soffitto e strato; il segui-scia vede solo le navi in moto") {
    Siluro s(*dati_siluro("mk48"), imp(0.0), {0, 0}, 100.0, 0, 1);
    auto nave = sottomarino(9, {0, 2000}, 0.0, 90.0, 15.0, 160.0);
    nave.nave = true;
    double d = 0, r = 0;
    (void)d; (void)r;
    // la testa del Mk 48 ignora cio' che sta sopra il soffitto (15 m): la nave non diventa un bersaglio
    std::vector<SorgenteAcustica> v{nave};
    auto e = corri(s, v, 120.0);
    CHECK_FALSE(e.ha(EventoSiluro::Tipo::aggancio));
    // lo strato dimezza la portata
    Siluro p(*dati_siluro("mk48"), imp(0.0), {0, 0}, 100.0, 0, 1);
    const auto sub = sottomarino(1, {0, 1000}, 300.0);
    CHECK(p.portata_su(sub, {200.0}) == doctest::Approx(0.5 * p.portata_su(sub, {})));
    // 53-65K: segue la scia di una nave in moto, non vede un sottomarino immerso
    Siluro w(*dati_siluro("type53_65k"), imp(0.0, ModoTesta::passivo, 500.0), {0, 0}, 10.0, 0, 1);
    CHECK(w.portata_su(nave, {}) > 0.0);
    CHECK(w.portata_su(sub, {}) == 0.0);
}

TEST_CASE("siluro: l'esca ADC Mk 2 seduce il passivo; l'attivo la rifiuta piu' spesso (Doppler zero, eco puntuale)") {
    // sottomarino che scappa a 6 kn e un'esca piu' rumorosa lasciata sulla sua rotta, fra lui e il siluro
    auto prova = [](ModoTesta modo, std::uint64_t seme) {
        Siluro s(*dati_siluro("mk48"), imp(0.0, modo, 500.0), {0, 0}, 100.0, 0, seme);
        Esca esca({0, 2500}, 100.0, 0.0);
        std::vector<SorgenteAcustica> v{sottomarino(5, {0, 3200}, 100.0, 0.0, 6.0, 125.0), esca.sorgente(50)};
        const auto e = corri(s, v, 400.0);
        return std::pair<bool, bool>{e.ha(EventoSiluro::Tipo::esca_sedotta, 50), e.ha(EventoSiluro::Tipo::esca_rifiutata, 50)};
    };
    int sedotti_passivo = 0, rifiutati_attivo = 0, sedotti_attivo = 0;
    for (std::uint64_t seme = 1; seme <= 40; ++seme) {
        sedotti_passivo += prova(ModoTesta::passivo, seme).first;
        const auto a = prova(ModoTesta::attivo, seme);
        sedotti_attivo += a.first;
        rifiutati_attivo += a.second;
    }
    CHECK(sedotti_passivo >= 30);                  // passivo: seduzione forte (p rifiuto 0,1)
    CHECK(rifiutati_attivo > sedotti_passivo / 4);  // attivo: rifiuto medio-alto (p 0,7)
    CHECK(sedotti_attivo < sedotti_passivo);
    // il segui-scia e' immune
    Siluro w(*dati_siluro("type53_65k"), imp(0.0, ModoTesta::passivo, 500.0), {0, 0}, 10.0, 0, 1);
    CHECK(w.portata_su(Esca({0, 1000}, 10.0, 0.0).sorgente(1), {}) == 0.0);
}

TEST_CASE("siluro: anti-circolare, si spegne se torna verso il lanciatore") {
    // ricerca circolare vicino al lanciatore fermo: con l'ACR si spegne, senza l'ACR no
    auto i = imp(0.0, ModoTesta::passivo, 500.0);
    i.schema = SchemaRicerca::circolare;
    Siluro s(*dati_siluro("mk48"), i, {0, 0}, 100.0, 0, 1);
    auto lanciatore = sottomarino(0, {0, -200}, 100.0);
    lanciatore.lanciatore = true;
    std::vector<SorgenteAcustica> v{lanciatore};
    const auto e = corri(s, v, 300.0);
    CHECK(e.ha(EventoSiluro::Tipo::spento));
    CHECK(s.fase() == FaseSiluro::spento);
    i.anti_circolare = false;
    Siluro n(*dati_siluro("mk48"), i, {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> w{lanciatore};
    const auto f = corri(n, w, 300.0);
    CHECK_FALSE(f.ha(EventoSiluro::Tipo::spento));
}

TEST_CASE("siluro: filo, comandi, vincoli del lanciatore e taglio") {
    Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 20000.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> v;
    Lanciatore lento{{0, 0}, 6.0 * NODO, 0.5};
    corri(s, v, 30.0, {}, lento);
    CHECK(s.filo());
    s.comanda({ComandoFiloTipo::rotta, 45.0});
    corri(s, v, 30.0, {}, lento);
    CHECK(s.rotta() == doctest::Approx(45.0));        // la rotta via filo vale anche prima dell'attivazione
    CHECK(s.filo_battello_svolto_m() > 0.0);
    // il lanciatore accelera a 18 kn: dopo la tolleranza di 5 s il filo si rompe
    Lanciatore veloce{{0, 0}, 18.0 * NODO, 0.5};
    auto e = corri(s, v, 10.0, {}, veloce);
    CHECK(e.ha(EventoSiluro::Tipo::filo_rotto));
    CHECK_FALSE(s.filo());
    s.comanda({ComandoFiloTipo::rotta, 180.0});       // senza filo i comandi non arrivano
    corri(s, v, 20.0, {}, veloce);
    CHECK(s.rotta() == doctest::Approx(45.0));
    // taglio volontario
    Siluro t(*dati_siluro("mk48"), imp(0.0), {0, 0}, 100.0, 0, 1);
    t.comanda({ComandoFiloTipo::taglia});
    CHECK_FALSE(t.filo());
    // i dati della testa tornano lungo il filo: rilevamento della sorgente
    Siluro h(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 100.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> w{sottomarino(5, {600, 1800}, 100.0, 0.0, 0.0, 130.0)};   // entro i 2,4 km del passivo in alta
    h.avanza(DT, w, {}, lento);
    corri(h, w, 10.0, {}, lento);
    REQUIRE_FALSE(h.testa().empty());
    CHECK(h.testa()[0].sorgente == 5);
    CHECK(h.testa()[0].distanza_m < 0.0);              // passivo: niente distanza
}

TEST_CASE("siluro: spegnimento via filo, si ferma e affonda senza esplodere") {
    Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 500.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> v{sottomarino(5, {0, 1500}, 100.0)};
    Lanciatore l{{0, -100}, 3.0, 0.0};
    corri(s, v, 20.0, {}, l);
    s.comanda({ComandoFiloTipo::spegni});
    auto e = s.eventi();
    REQUIRE_FALSE(e.empty());
    CHECK(e.back().tipo == EventoSiluro::Tipo::spento);
    const double q = s.quota();
    const Vec2 p = s.pos();
    for (int i = 0; i < 9 * 60; ++i) s.avanza(DT, v, {}, l);
    CHECK(s.fase() == FaseSiluro::spento);
    CHECK(s.velocita() == 0.0);
    CHECK(s.pos().y == doctest::Approx(p.y));
    CHECK(s.quota() == doctest::Approx(q + 60.0 * Siluro::AFFONDAMENTO_M_S).epsilon(0.01));
    CHECK(s.eventi().empty());   // nessuna esplosione
}

TEST_CASE("siluro: perdita del contatto e riacquisizione a cerchio; esaurimento a fine autonomia") {
    // un bersaglio che sparisce (sotto lo strato e lontano) fa passare il siluro in riacquisizione
    Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::passivo, 500.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> v{sottomarino(5, {0, 3000}, 100.0, 0.0, 0.0, 130.0)};
    auto e = corri(s, v, 40.0);
    REQUIRE(e.ha(EventoSiluro::Tipo::aggancio, 5));
    v[0].pos = {20000, 20000};
    e = corri(s, v, 20.0);
    CHECK(e.ha(EventoSiluro::Tipo::perdita, 5));
    CHECK(s.fase() == FaseSiluro::riacquisizione);
    // corsa fino all'esaurimento: in bassa velocita' l'autonomia e' maggiore
    Siluro a(*dati_siluro("mk46"), imp(0.0, ModoTesta::passivo, 100000.0), {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> vuoto;
    e = corri(a, vuoto, 2000.0);
    CHECK(e.ha(EventoSiluro::Tipo::esaurito));
    CHECK(a.percorso_m() == doctest::Approx(dati_siluro("mk46")->portata_alta_m).epsilon(0.01));
}

TEST_CASE("siluro: anno di servizio, impostazioni da ordine e avviso del filo") {
    // 1989: lo Spearfish non e' ancora in servizio, l'ADCAP si' (IOC 1988), il Mk 48 Mod 4 dal 1980
    CHECK(dati_siluro("spearfish")->in_servizio_dal > 1989);
    CHECK(dati_siluro("mk48_adcap")->in_servizio_dal <= 1989);
    CHECK(dati_siluro("mk48")->in_servizio_dal <= 1989);
    for (const auto& c : classi_siluro()) CHECK(dati_siluro(c)->in_servizio_dal > 1900);
    // applica: valori ammessi e rifiutati
    ImpostazioniSiluro i;
    CHECK(applica(i, *campo_siluro("modo"), 2));
    CHECK(i.modo == ModoTesta::misto);
    CHECK(applica(i, *campo_siluro("rte"), 3500));
    CHECK(i.rte_m == 3500.0);
    CHECK(applica(i, *campo_siluro("rotta"), 370));
    CHECK(i.rotta == doctest::Approx(10.0));
    CHECK_FALSE(applica(i, *campo_siluro("modo"), 5));
    CHECK_FALSE(applica(i, *campo_siluro("soffitto"), i.pavimento_m + 1));
    CHECK_FALSE(campo_siluro("detonazione"));
    // avviso del filo: battello fuori dai limiti prima della rottura, poi rocchetto oltre l'80%
    Siluro s(*dati_siluro("mk48"), i, {0, 0}, 100.0, 0, 1);
    std::vector<SorgenteAcustica> v;
    s.avanza(DT, v, {}, Lanciatore{{0, 0}, 3.0, 0.0});
    CHECK_FALSE(s.filo_a_rischio());
    s.avanza(DT, v, {}, Lanciatore{{0, 0}, 15.0 * NODO, 0.0});
    CHECK(s.filo_a_rischio());
    CHECK(s.filo());
    Siluro t(*dati_siluro("mk48"), i, {0, 0}, 100.0, 0, 1);
    Lanciatore l{{0, 0}, 3.0, 0.0};
    while (t.filo() && t.filo_siluro_frazione() < Siluro::FILO_QUASI && t.attivo()) t.avanza(1.0, v, {}, l);
    CHECK(t.filo_siluro_frazione() >= Siluro::FILO_QUASI);
    CHECK(t.filo_a_rischio());
}

// --- aggancio nel runtime delle missioni (Missione::siluri_enhanced) ---

namespace {

// Il giocatore (0, 0) lancia al secondo 1 un Mk 48 sull'Alfa a 4 km per 000 (rotta 090, 5 nodi, 100 m), con il fine
// corsa dello script a 30 s: nel livello classico l'arma esce li', con il modello lo script non gira
std::string missione_lancio() {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_siluro_lancio.lua";
    const std::string modello = "modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = 160 }";
    std::ofstream(f) << "local script = {}\n"
        "script.L = function(u)\n"
        "  m688.bersaglio(u, 2) m688.memorizza_bersaglio(u) m688.modello_azzera(u)\n"
        "  m688.modello_classe(u, 'mk48') m688.modello_flag(u, 41) m688.lancia(u, 'F', 'R')\n"
        "  m688.modello_avanti_tutta(u, 50) m688.modello_difesa(u, 75) m688.modello_integrita(u, 20)\n"
        "  m688.telegrafo(u, 1) m688.collega(u, 1) m688.nuova_unita(u) m688.dopo(u, 30, 'F')\n"
        "end\n"
        "script.F = function(u) m688.togli_unita(u) end\n"
        "script.R = function(u) end\n"
        "return { id = 'PROVA', teatro = 'prova', script = script, varianti = { { probabilita = 1, unita = {\n"
        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
        "  posizione = { x = 0, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 0, " << modello << " },\n"
        "{ id = 1, classe = 'cargo', categoria = 'nave', flag = 10, posizione = { x = -30000, y = -30000 },"
        "  quota_m = 0, rotta = 0, velocita_kn = 0, " << modello << " },\n"
        "{ id = 2, classe = 'alfa', categoria = 'sottomarino', flag = 18, posizione = { x = 0, y = 4000 },"
        "  quota_m = 100, rotta = 90, velocita_kn = 5, " << modello << " },\n"
        "}, timer = { { dopo_s = 1, unita = 0, gestore = 'L' } } } } }\n";
    return f.string();
}

int arma(const Missione& m) {
    for (const auto& [id, u] : m.unita())
        if (u.categoria == "arma") return id;
    return -1;
}

}  // namespace

TEST_CASE("siluro nella missione: classico invariato, modello con esplosione sul bersaglio") {
    const auto f = missione_lancio();
    {   // livello classico: l'arma esce col fine corsa dello script e non e' del modello
        Missione m(f, 1, false);
        m.avanza(2.0);
        const int s = arma(m);
        REQUIRE(s >= 0);
        CHECK(m.siluro(s) == nullptr);
        CHECK_FALSE(m.con_siluri_enhanced());
        m.avanza(30.0);
        CHECK(m.unita().count(s) == 0);
        CHECK(m.eventi_siluri().empty());
    }
    {   // Enhanced: lo script di fine corsa non gira, la testa aggancia e la spoletta colpisce l'Alfa
        Missione m(f, 1, false);
        m.siluri_enhanced(M688_DATI);
        m.avanza(2.0);
        const int s = arma(m);
        REQUIRE(s >= 0);
        REQUIRE(m.siluro(s) != nullptr);
        // lanciato da script senza le impostazioni della stazione: il contatto del lanciatore ha solo il rilevamento
        // (passivo), quindi la distanza d'armamento tipica e la quota del lanciatore (docs/18 fase 1)
        CHECK(m.siluro(s)->impostazioni().rte_m == doctest::Approx(2000.0));
        CHECK(m.siluro(s)->impostazioni().quota_ricerca_m == doctest::Approx(60.0));
        CHECK(m.unita().at(s).collegata == 0);   // filoguidato
        m.avanza(30.0);
        REQUIRE(m.unita().count(s) == 1);
        for (int i = 0; i < 9 * 400 && m.colpi().empty(); ++i) m.avanza(1.0 / 9.0);
        REQUIRE(m.colpi().size() == 1);
        CHECK(m.colpi().front().vittima == 2);
        CHECK(m.colpi().front().arma == s);
        // geometria vera dello scoppio: entro la spoletta del Mk 48 (8 m) piu' il margine dello scafo di un
        // sottomarino (5 m) dal centro della vittima
        CHECK(m.colpi().front().geometria_vera);
        CHECK(m.colpi().front().distanza_m <= 8.0 + 5.0 + 1e-6);
        CHECK(m.colpi().front().distanza_m > 0.0);
        CHECK(std::fabs(m.colpi().front().dz_m) <= m.colpi().front().distanza_m + 1e-9);
        CHECK(m.siluro(s) == nullptr);
        bool aggancio = false, esplosione = false;
        for (const auto& e : m.eventi_siluri()) {
            CHECK(e.siluro == s);
            CHECK(e.lanciatore == 0);
            aggancio |= e.evento.tipo == EventoSiluro::Tipo::aggancio;
            esplosione |= e.evento.tipo == EventoSiluro::Tipo::esplosione;
        }
        CHECK(aggancio);
        CHECK(esplosione);
    }
    std::filesystem::remove(f);
}

TEST_CASE("siluro nella missione: senza siluri.toml i siluri restano classici") {
    const auto f = missione_lancio();
    Missione m(f, 1, false);
    m.siluri_enhanced("/cartella/che/non/esiste");
    CHECK_FALSE(m.con_siluri_enhanced());
    REQUIRE_FALSE(m.errori().empty());
    m.avanza(2.0);
    CHECK(m.siluro(arma(m)) == nullptr);
    std::filesystem::remove(f);
}

TEST_CASE("siluro nella missione: impostazioni del giocatore, filo e spegnimento") {
    const auto f = missione_lancio();
    Missione m(f, 1, false);
    m.siluri_enhanced(M688_DATI);
    ImpostazioniSiluro imp;
    imp.rotta = 180.0;   // lontano dall'Alfa
    imp.modo = ModoTesta::passivo;
    imp.rte_m = 8000.0;
    m.imposta_siluro_giocatore(imp);
    m.avanza(2.0);
    const int s = arma(m);
    REQUIRE(m.siluro(s) != nullptr);
    CHECK(m.siluro(s)->impostazioni().rotta == doctest::Approx(180.0));
    CHECK(m.siluro(s)->impostazioni().modo == ModoTesta::passivo);
    m.avanza(20.0);
    CHECK(m.unita().at(s).rotta == doctest::Approx(180.0).epsilon(0.01));
    CHECK(m.unita().at(s).pos.y < -100.0);
    CHECK(m.comanda_siluro(s, {ComandoFiloTipo::rotta, 200.0}));
    m.avanza(10.0);
    CHECK(m.unita().at(s).rotta == doctest::Approx(200.0).epsilon(0.01));
    CHECK_FALSE(m.comanda_siluro(0, {ComandoFiloTipo::spegni}));   // il giocatore non e' un siluro
    CHECK(m.comanda_siluro(s, {ComandoFiloTipo::spegni}));
    m.avanza(1.0);
    CHECK(m.unita().count(s) == 0);   // spento: esce dal gioco senza colpi
    CHECK(m.colpi().empty());
    std::filesystem::remove(f);
}

TEST_CASE("siluro: battello fermo sul fondo in acque basse, la testa attiva lo vede poco e lo scarta come fondo") {
    // dossier 17 §6.7: Pk <= 0,1 contro un battello fermo sul fondo (Falkland); STIMA nel modello della testa
    const auto agganci = [](double fondale) {
        int n = 0;
        for (std::uint64_t seme = 1; seme <= 40; ++seme) {
            Siluro s(*dati_siluro("mk46"), imp(0.0, ModoTesta::attivo, 0.0), {0, 0}, 30.0, 0, seme);
            s.imposta_fondale(fondale);
            std::vector<SorgenteAcustica> v{sottomarino(7, {0, 600}, 40.0)};   // fermo a 40 m
            n += corri(s, v, 60.0).ha(EventoSiluro::Tipo::aggancio, 7);
        }
        return n;
    };
    Siluro s(*dati_siluro("mk46"), imp(0.0, ModoTesta::attivo, 0.0), {0, 0}, 40.0, 0, 1);
    std::vector<SorgenteAcustica> vuoto;
    corri(s, vuoto, 1.0);
    const SorgenteAcustica fermo = sottomarino(7, {0, 600}, 40.0);
    const double libera = s.portata_su(fermo, {});
    s.imposta_fondale(45.0);
    CHECK(s.portata_su(fermo, {}) == doctest::Approx(Siluro::FONDO_FATTORE * libera));
    SorgenteAcustica in_moto = fermo;
    in_moto.velocita = 5.0 * NODO;   // in moto il Doppler lo separa dal fondo
    CHECK(s.portata_su(in_moto, {}) == doctest::Approx(libera));
    const int profondo = agganci(1e9), basso = agganci(45.0);
    CAPTURE(profondo);
    CAPTURE(basso);
    CHECK(profondo >= 0.9 * 40 * dati_siluro("mk46")->affidabilita - 4);   // le teste guaste non agganciano
    CHECK(basso <= profondo / 2);
    CHECK(dati_siluro("mk46")->fondale_minimo_m == doctest::Approx(40.0));
    CHECK(dati_siluro("at1")->fondale_minimo_m == doctest::Approx(60.0));   // dossier 17 §6.7
    CHECK(dati_siluro("mk48")->fondale_minimo_m == doctest::Approx(0.0));
}

TEST_CASE("siluro: il noisemaker nel fascio della testa attiva riavvia la verifica e fa perdere l'aggancio") {
    // dossier 05 §5.3: contro l'attivo l'effetto del noisemaker e' «medio» (il jammer riavvia la verifica)
    const auto colpi = [](bool esca) {
        int n = 0;
        for (std::uint64_t seme = 1; seme <= 40; ++seme) {
            Siluro s(*dati_siluro("mk46"), imp(0.0, ModoTesta::attivo, 0.0), {0, 0}, 100.0, 0, seme);
            std::vector<SorgenteAcustica> v{sottomarino(7, {0, 900}, 100.0, 0.0, 12.0)};   // si allontana a 12 kn
            if (esca) {
                SorgenteAcustica e = sottomarino(8, {0, 500}, 100.0);
                e.esca = true;
                e.livello_db = 150.0;
                e.forza_bersaglio_db = 0.0;
                e.lunghezza_m = 0.0;
                v.push_back(e);
            }
            const Esito x = corri(s, v, 90.0);
            n += x.ha(EventoSiluro::Tipo::esplosione, 7) || (x.ha(EventoSiluro::Tipo::aggancio, 7) && !x.ha(EventoSiluro::Tipo::perdita, 7));
        }
        return n;
    };
    const int senza = colpi(false), con = colpi(true);
    CAPTURE(senza);
    CAPTURE(con);
    CHECK(con < senza);
}

TEST_CASE("siluro: l'esca semovente (MG-74) inganna anche l'attivo, che smaschera l'ADC Mk 2 stazionaria") {
    TabellaSiluri t = carica_siluri(M688_DATI "/siluri.toml");
    const DatiEsca* mg74 = t.esca("mg74");
    REQUIRE(mg74 != nullptr);
    CHECK(mg74->mobile());
    CHECK(mg74->imita_rumore);
    CHECK(mg74->quota_max_m == doctest::Approx(250.0));
    CHECK_FALSE(t.esca("adc_mk2")->mobile());
    // la manovra programmata: dritta, poi alternata attorno alla rotta del lancio
    CHECK(rotta_esca_mobile(*mg74, 90.0, 10.0) == doctest::Approx(90.0));
    const double a = rotta_esca_mobile(*mg74, 90.0, mg74->virata_ogni_s + 1.0);
    const double b = rotta_esca_mobile(*mg74, 90.0, 2.0 * mg74->virata_ogni_s + 1.0);
    CHECK(std::fabs(normalizza_180(a - b)) == doctest::Approx(mg74->virata_gradi));
    CHECK(std::fabs(normalizza_180(a - 90.0)) == doctest::Approx(0.5 * mg74->virata_gradi));
    auto prova = [&](bool mobile, std::uint64_t seme) {
        Siluro s(*dati_siluro("mk48"), imp(0.0, ModoTesta::attivo, 500.0), {0, 0}, 100.0, 0, seme);
        SorgenteAcustica e = Esca({0, 2500}, 100.0, 0.0).sorgente(50);
        if (mobile) { e.velocita = mg74->velocita_kn * NODO; e.forza_bersaglio_db = mg74->eco_db; }
        std::vector<SorgenteAcustica> v{sottomarino(5, {0, 3200}, 100.0, 0.0, 6.0, 125.0), e};
        return corri(s, v, 400.0).ha(EventoSiluro::Tipo::esca_sedotta, 50);
    };
    int ferma = 0, semovente = 0;
    for (std::uint64_t seme = 1; seme <= 40; ++seme) { ferma += prova(false, seme); semovente += prova(true, seme); }
    CAPTURE(ferma);
    CAPTURE(semovente);
    CHECK(semovente > ferma + 8);   // senza il rifiuto Doppler (0,5) resta quello dell'eco puntuale (0,2)
}

TEST_CASE("siluro: la distanza dallo scafo misura dal cilindro dello scafo, non dal centro") {
    // un 688 (110 x 10 m) a 100 m con la prua a nord: sotto la prua, lungo il fianco, oltre la poppa
    const auto sub = [](double x, double y, double q) { return distanza_dallo_scafo({x, y}, q, {0, 0}, 100.0, 0.0, 110.0, 10.0, false, 0.0); };
    CHECK(sub(0, 50, 100) == doctest::Approx(0.0));            // dentro, a 50 m dal centro verso prua
    CHECK(sub(8, 50, 100) == doctest::Approx(3.0));            // a 3 m dal fianco, all'altezza della prua
    CHECK(sub(0, 50, 110) == doctest::Approx(5.0));            // 5 m sotto la chiglia
    CHECK(sub(0, 65, 100) == doctest::Approx(std::hypot(10.0, 0.0) - 5.0).epsilon(0.01));   // 10 m oltre la prua
    CHECK(sub(30, 0, 100) == doctest::Approx(25.0));
    // l'asse segue la rotta: a est il battello e' lungo x
    CHECK(distanza_dallo_scafo({50, 0}, 100.0, {0, 0}, 100.0, 90.0, 110.0, 10.0, false, 0.0) == doctest::Approx(0.0));
    // una nave (pescaggio 6 m, 15 m di larghezza): sotto la carena conta solo la quota oltre il pescaggio
    const auto nave = [](double x, double q) { return distanza_dallo_scafo({x, 20}, q, {0, 0}, 0.0, 0.0, 130.0, 15.0, true, 6.0); };
    CHECK(nave(0, 4) == doctest::Approx(0.0));
    CHECK(nave(0, 14) == doctest::Approx(8.0));
    CHECK(nave(10, 4) == doctest::Approx(2.5));
    // senza dati dello scafo: 10 m di larghezza e pescaggio
    CHECK(distanza_dallo_scafo({0, 0}, 15.0, {0, 0}, 0.0, 0.0, 0.0, 0.0, true, 0.0) == doctest::Approx(5.0));
}

TEST_CASE("siluro dell'IA: si lancia sul contatto del lanciatore, non sulla posizione vera (docs/18 fase 1)") {
    // lo stesso lancio di prima, ma dall'unita' 1 sull'Alfa (2) a nord del giocatore; il contatto del lanciatore deciso a
    // mano, e il lancio al primo passo, prima che i sensori lo aggiornino. L'unita' 1 e' un sottomarino in superficie: dal
    // 31/10/2026 una nave dell'IA tira solo il siluro antisom della sua dotazione, e il cargo non ne ha
    auto f = missione_lancio();
    {
        std::ifstream in(f);
        std::string s((std::istreambuf_iterator<char>(in)), {});
        const std::string da = "{ dopo_s = 1, unita = 0, gestore = 'L' }", a = "{ dopo_s = 0.05, unita = 1, gestore = 'L' }";
        s.replace(s.find(da), da.size(), a);
        const std::string cargo = "classe = 'cargo', categoria = 'nave'";
        s.replace(s.find(cargo), cargo.size(), "classe = 'los_angeles', categoria = 'sottomarino'");
        s.replace(s.find("posizione = { x = -30000, y = -30000 }"), 38, "posizione = { x = 0, y = -2000 }");
        std::ofstream(f) << s;
    }
    const auto lancio = [&](const std::optional<Contatto>& k) {
        Missione m(f, 1, false);
        m.siluri_enhanced(M688_DATI);
        m.avanza(0.01);   // il registro comincia a osservare (le armi del suo primo sguardo non sono lanci)
        if (k) m.trova(1)->contatti[2] = *k;
        m.avanza(0.2);
        const int s = arma(m);
        REQUIRE(s >= 0);
        REQUIRE(m.siluro(s) != nullptr);
        const auto& ev = m.registro().eventi();   // lo spettatore: il lancio del cargo nel registro (docs/18 fase 5)
        CHECK(std::any_of(ev.begin(), ev.end(), [&](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia && e.unita == 1 && e.arma == s; }));
        return m.siluro(s)->impostazioni();
    };
    // il cargo a (0, -2000), l'Alfa vero a (0, 4000): per 000, 6 km
    const auto vera = lancio(std::nullopt);   // nessun contatto: alla cieca, sulla verita'
    CHECK(vera.rotta == doctest::Approx(0.0).epsilon(0.01));
    Contatto stimato;
    stimato.posizione_nota = true;
    stimato.pos_stimata = {3000.0, 4000.0};   // stimato 3 km piu' a est
    stimato.quota_stimata = 200.0;
    stimato.rilevamento = 26.6;
    stimato.qualita = 1200.0;
    const auto su_stima = lancio(stimato);
    CHECK(su_stima.rotta == doctest::Approx(rilevamento({0.0, -2000.0}, {3000.0, 4000.0})).epsilon(0.01));
    CHECK(su_stima.rte_m == doctest::Approx(0.6 * std::hypot(3000.0, 6000.0)).epsilon(0.01));
    CHECK(su_stima.quota_ricerca_m == doctest::Approx(200.0));
    Contatto rilevato;   // solo il rilevamento, sbagliato di 10 gradi
    rilevato.rilevamento = 10.0;
    rilevato.qualita = 1200.0;
    const auto su_rilevamento = lancio(rilevato);
    CHECK(su_rilevamento.rotta == doctest::Approx(10.0));
    CHECK(su_rilevamento.rte_m == doctest::Approx(2000.0));
    std::filesystem::remove(f);
}
