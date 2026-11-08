#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/radio.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Sotto il ghiaccio nella missione (opzione A; missione_ghiaccio.cpp) e l'IA che sonda il fondo
// (missione_fondale_ia.cpp): il campo dal meteo della missione, il quadro della volta del 688 e del Victor, l'urto della
// vela con le chiglie, l'IA che scende sotto le chiglie misurate e che riempie il suo quadro del fondale; niente di
// tutto questo senza l'Enhanced o senza il meteo.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 e un Victor III verso nord nel Barents di gennaio, col pack fisso della missione (copertura 1)
std::string scena(double quota_688, double quota_victor, double ghiaccio = 1.0) {
    const auto f = fs::temp_directory_path() / "m688_prova_ghiaccio_missione.lua";
    std::ofstream(f) << "return { id = 'PROVA_GHIACCIO', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, meteo = { fisso = { ghiaccio = " << ghiaccio << " } },"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 50000, y = 50000 },"
                        "  quota_m = " << quota_688 << ", rotta = 0, velocita_kn = 10, " << SUB << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 80000, y = 50000 },"
                        "  quota_m = " << quota_victor << ", rotta = 0, velocita_kn = 8, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    // l'Enhanced lo accende DanniMissione con la regola di simulazione (acustica e meteo), il classico con la classica
    Partita(const std::string& file, bool enhanced = true, double fondale_m = 600.0)
        : m(file, 1, false), dm(classi, M688_DATI, 1, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica), ds(classi, M688_DATI, 1) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -fondale_m, std::vector<std::uint16_t>(200 * 200, 0)), pescaggi);
        if (enhanced) m.collisioni_enhanced(classi, M688_DATI);
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
    int urti_ghiaccio(int id) const {
        return static_cast<int>(std::count_if(m.urti().begin(), m.urti().end(), [&](const Missione::UrtoMissione& u) {
            return u.urto.a.unita == id && u.urto.b.unita < 0;
        }));
    }
};

bool c_e() { return fs::exists(M688_DATI "/acustica.toml") && fs::exists(M688_DATI "/meteo.toml"); }

}  // namespace

TEST_CASE("ghiaccio nella missione: il pack dal meteo, la volta del 688 col BQS-15 e del Victor con l'MG-70" *
          doctest::skip(!c_e())) {
    Partita p(scena(60.0, 60.0));
    p.avanza(1.0);   // l'Enhanced si accende al primo passo (DanniMissione)
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
    int sotto = 0, passi = 0;
    for (int k = 0; k < 40; ++k) {   // 20 minuti
        p.avanza(30.0);
        ++passi;
        sotto += p.m.sotto_banchisa(0);
    }
    REQUIRE(p.m.ghiaccio() != nullptr);
    MESSAGE("il 688 sotto la banchisa in ", sotto, " passi su ", passi);
    CHECK(sotto > passi / 2);   // il pack quasi pieno: l'acqua libera e' il 10%
    const QuadroVolta* q = p.m.quadro_volta(0);
    REQUIRE(q);
    CHECK(q->celle() > 20);
    // il Victor sonda la volta finche' non e' in silenzio (i ping del BQS-15 del 688 lo mettono in allarme)
    const QuadroVolta* qv = p.m.quadro_volta(3);
    REQUIRE(qv);
    MESSAGE("celle della volta: il 688 ", q->celle(), ", il Victor ", qv->celle());
    CHECK(qv->celle() > 0);
    // a 60 m nessuna chiglia arriva alla vela
    CHECK(p.urti_ghiaccio(0) == 0);
}

TEST_CASE("fondo dell'IA nella missione: il Victor con 120 m sotto la chiglia sonda e riempie il suo quadro" * doctest::skip(!c_e())) {
    Partita p(scena(60.0, 60.0, 0.0), true, 180.0);   // senza ghiaccio, 180 m d'acqua
    for (int k = 0; k < 20; ++k) p.avanza(30.0);
    const QuadroFondale* qf = p.m.quadro_fondale(3);
    REQUIRE(qf);
    MESSAGE("il Victor: ", qf->sondaggi().size(), " sondaggi e ", qf->ritorni().size(), " ritorni del sonar del fondo nel suo quadro");
    CHECK(qf->sondaggi().size() > 0);
    // la quota prudente non sale mai sopra i 20 m
    CHECK(p.m.unita().at(3).quota_ordinata >= 20.0);
}

TEST_CASE("ghiaccio nella missione: a 20 m la vela urta le chiglie; il battello resta sotto e ha i danni" * doctest::skip(!c_e())) {
    Partita p(scena(20.0, 60.0));
    double quota_min_dopo = 1e9;
    for (int k = 0; k < 40 && p.urti_ghiaccio(0) == 0; ++k) p.avanza(30.0);
    const int urti = p.urti_ghiaccio(0);
    MESSAGE("urti col ghiaccio in ", p.m.tempo(), " s: ", urti);
    REQUIRE(urti > 0);
    const auto it = std::find_if(p.m.urti().begin(), p.m.urti().end(), [](const Missione::UrtoMissione& u) { return u.urto.b.unita < 0; });
    REQUIRE(it != p.m.urti().end());
    MESSAGE("il primo: ", it->esito.energia_normale_mj, " MJ sulla ", std::string(nome_parte_urto(it->esito.a.parte)));
    CHECK(it->esito.energia_normale_mj > 0.0);
    CHECK(it->esito.a.parte == ParteUrto::vela);   // il punto sulla vela, a prua del centro
    // dopo l'urto il battello sta sotto la chiglia (la quota cresce) e non e' distrutto
    p.avanza(5.0);
    quota_min_dopo = p.m.unita().at(0).quota;
    CHECK(quota_min_dopo > 20.0);
    CHECK_FALSE(p.m.unita().at(0).distrutta);
}

TEST_CASE("ghiaccio nella missione: il Victor a 25 m vede le chiglie e scende sotto" * doctest::skip(!c_e())) {
    Partita p(scena(150.0, 25.0));
    double piu_fondo = 25.0;
    for (int k = 0; k < 40; ++k) {
        p.avanza(30.0);
        piu_fondo = std::max(piu_fondo, p.m.unita().at(3).quota_ordinata);
    }
    MESSAGE("la quota ordinata piu' fonda del Victor: ", piu_fondo, " m; urti col ghiaccio: ", p.urti_ghiaccio(3));
    CHECK(piu_fondo > 30.0);
}

TEST_CASE("ghiaccio nella missione: niente senza ghiaccio, niente nel classico" * doctest::skip(!c_e())) {
    {
        Partita p(scena(20.0, 60.0, 0.0));   // il meteo senza ghiaccio
        p.avanza(120.0);
        CHECK_FALSE(p.m.sotto_banchisa(0));
        CHECK(p.urti_ghiaccio(0) == 0);
        CHECK(p.m.quadro_volta(0) == nullptr);
    }
    {
        Partita p(scena(20.0, 60.0), false);   // il classico
        p.avanza(60.0);
        CHECK(p.m.ghiaccio() == nullptr);
        CHECK(p.m.quadro_fondale(3) == nullptr);
    }
}

TEST_CASE("ghiaccio e radio: sotto la banchisa il filo non riceve il VLF, l'ELF si'") {
    SalaRadio alfa(LatoRadio::urss, 1);   // l'Alfa ha l'ELF
    REQUIRE(alfa.elf());
    StatoAntenne s;
    s.quota_m = 60.0;
    s.velocita_kn = 5.0;
    alfa.cala_filo(0.0);
    for (double t = 0.0; t < 200.0; t += 1.0) alfa.avanza(t, 1.0, s);
    CHECK(alfa.ricezione(Banda::vlf) == Ricezione::si);
    CHECK(alfa.ricezione(Banda::elf) == Ricezione::si);
    s.ghiaccio = true;
    alfa.avanza(200.0, 1.0, s);
    CHECK(alfa.ricezione(Banda::vlf) == Ricezione::no);
    CHECK(alfa.ricezione(Banda::hf) == Ricezione::no);
    CHECK(alfa.ricezione(Banda::elf) == Ricezione::si);
}

TEST_CASE("ghiaccio nella missione: sotto la banchisa l'antenna e il periscopio non salgono (489)" *
          doctest::skip(!c_e())) {
    Partita p(scena(60.0, 150.0));
    p.avanza(1.0);
    p.m.periscopi_enhanced(M688_DATI);   // dopo il primo passo: acustica e meteo gia' accesi
    for (int k = 0; k < 60 && !p.m.sotto_banchisa(0); ++k) p.avanza(10.0);   // fuori da un'eventuale polynya
    REQUIRE(p.m.sotto_banchisa(0));
    const auto avvisi = [&] {
        return std::count_if(p.m.messaggi().begin(), p.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 489; });
    };
    const auto prima = avvisi();
    CHECK_FALSE(p.m.applica({p.m.tempo(), TipoOrdine::antenna, 0.0, 0.0, 1}));
    CHECK(avvisi() == prima + 1);
    CHECK((p.m.unita().at(0).flag54 & 4) == 0);
    REQUIRE(p.m.posto_periscopi());
    p.m.applica({p.m.tempo(), TipoOrdine::periscopio, 0.0, 0.0, 1});
    p.avanza(1.0);
    CHECK_FALSE(p.m.posto_periscopi()->stato().comandato_su);
    CHECK(avvisi() == prima + 2);
    p.avanza(30.0);
    CHECK_FALSE(p.m.posto_periscopi()->stato().su);
}

TEST_CASE("ghiaccio e siluro: una chiglia sotto il soffitto inganna a volte la testa attiva; sopra il soffitto no") {
    const TabellaSiluri tab = carica_siluri(M688_DATI "/siluri.toml");
    const DatiSiluro* mk48 = tab.siluro("mk48");
    REQUIRE(mk48);
    // la chiglia come la passa la missione (missione_ghiaccio.cpp): ferma, muta, eco dal pescaggio, alla quota del pescaggio
    const auto chiglia = [](double pescaggio) {
        SorgenteAcustica x;
        x.id = -1000123;
        x.pos = {0.0, 700.0};
        x.quota = pescaggio;
        x.livello_db = 60.0;
        x.forza_bersaglio_db = forza_bersaglio_chiglia_db(pescaggio);
        x.lunghezza_m = 30.0;
        x.esca = true;
        return x;
    };
    const auto prova = [&](double soffitto, double pescaggio, std::uint64_t seme) {
        ImpostazioniSiluro imp;
        imp.rotta = 0.0;
        imp.modo = ModoTesta::attivo;
        imp.rte_m = 200.0;
        imp.quota_corsa_m = imp.quota_ricerca_m = 25.0;
        imp.soffitto_m = soffitto;
        Siluro s(*mk48, imp, {0.0, 0.0}, 25.0, 0, seme);
        std::vector<SorgenteAcustica> v{chiglia(pescaggio)};
        bool vista = false;
        for (int i = 0; i < 9 * 60 && s.attivo(); ++i) {
            s.avanza(1.0 / 9.0, v, std::vector<double>{}, std::nullopt);
            for (const auto& e : s.eventi())
                vista = vista || ((e.tipo == EventoSiluro::Tipo::esca_sedotta || e.tipo == EventoSiluro::Tipo::esca_rifiutata) && e.sorgente == -1000123);
        }
        return vista;
    };
    int sotto = 0, sopra = 0;
    for (std::uint64_t seme = 1; seme <= 10; ++seme) {
        sotto += prova(5.0, 20.0, seme);    // soffitto abbassato, cresta di 20 m: la testa la considera
        sopra += prova(15.0, 8.0, seme);    // soffitto di serie, cresta di 8 m: sopra il soffitto, invisibile
    }
    MESSAGE("la testa considera la chiglia: ", sotto, " su 10 sotto il soffitto, ", sopra, " su 10 sopra");
    CHECK(sotto > 5);
    CHECK(sopra == 0);
}

TEST_CASE("meteo del ghiaccio: il punto ERA5 comanda, il margine del pack c'e' (Curili a marzo: Okhotsk coperto, Pacifico libero)" *
          doctest::skip(!c_e())) {
    const DatiMeteo dati = carica_meteo(M688_DATI "/meteo.toml");
    const TeatroMeteo& T = dati.teatri.at("kunashir");
    REQUIRE_FALSE(T.punti.empty());
    // l'origine sul punto piu' coperto e su quello del Pacifico: le posizioni dei punti nel riferimento della missione
    const PuntoMeteo* pacifico = nullptr;
    const PuntoMeteo* coperto = nullptr;
    for (const auto& p : T.punti) {
        if (p.nome == "kuril_pac") pacifico = &p;
        if (!coperto || p.ghiaccio_pct[2] > coperto->ghiaccio_pct[2]) coperto = &p;
    }
    REQUIRE(pacifico);
    REQUIRE(coperto);
    REQUIRE(coperto->ghiaccio_pct[2] > 50.0);
    Meteo m(dati, "kunashir", 3, 12.0, 7);
    const double lat0 = 44.0, lon0 = 146.5;
    m.imposta_origine(lat0, lon0);
    m.avanza(0.0);
    const auto pos = [&](const PuntoMeteo& p) {
        return Vec2{(p.lon - lon0) * 111320.0 * std::cos(lat0 * PI / 180.0), (p.lat - lat0) * 111320.0};
    };
    const double g_pac = m.in(pos(*pacifico)).ghiaccio, g_cop = m.in(pos(*coperto)).ghiaccio;
    MESSAGE("marzo alle Curili: teatro ", m.stato().ghiaccio, ", Pacifico (", pacifico->nome, ") ", g_pac, ", ", coperto->nome, " ", g_cop);
    CHECK(g_pac < 0.25);   // il Pacifico libero, anche se la media del teatro e' alta
    CHECK(g_cop > 0.5);
    CHECK(m.in(pos(*pacifico)).hs_m() > m.in(pos(*coperto)).hs_m());   // dove il ghiaccio manca il mare torna
}

TEST_CASE("ghiaccio nella missione: sotto il pack il ping attivo del 688 da' falsi echi delle chiglie, fuori dai contatti; in acqua libera no" *
          doctest::skip(!c_e())) {
    for (const double g : {1.0, 0.0}) {
        CAPTURE(g);
        Partita p(scena(40.0, 150.0, g));
        p.avanza(1.0);
        const auto contatti_prima = p.m.unita().at(0).contatti.size();
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::impulso_sonar, 0.0, 0.0, 0}));
        p.avanza(1.0);
        const auto& echi = p.m.echi_falsi_attivo();
        MESSAGE("ghiaccio ", g, ": ", echi.size(), " falsi echi dal ping");
        if (g > 0.0) {
            REQUIRE_FALSE(echi.empty());
            for (const auto& e : echi) {
                CHECK(e.eccesso_db >= 0.0);
                CHECK(e.distanza_m <= 8000.0 * 1.05);
                CHECK(e.rilevamento >= 0.0);
                CHECK(e.rilevamento < 360.0);
            }
        } else {
            CHECK(echi.empty());
        }
        // i contatti veri non crescono per le chiglie (il Victor a 30 km non c'entra)
        CHECK(p.m.unita().at(0).contatti.size() <= contatti_prima + 1);
    }
}

// Sotto la banchisa il BQS-15 dell'IA resta acceso (la I17 della libreria, 30/10/2026): un SSN dell'IA a 130 m sotto il pack
// vede una PMR-2 sovietica ormeggiata a 300 m, lontana dal fondo e di un campo che non conosce, e la aggira. In acqua
// libera, con lo stesso campo sconosciuto, il BQS-15 resta spento e ci passa sopra
// Il BQS-8 degli Sturgeon (docs/ricerca/34): il sonar da ghiaccio senza le mine. Lo Sturgeon dell'IA a 25 m sotto il pack vede
// le chiglie e scende sotto, come il Victor con l'MG-70; con una PMR-2 sulla rotta non la vede e ci passa sopra (il BQS-15
// dei 688 la aggira, la prova qui sopra). Lo Sturgeon del giocatore accende il BQS-8 con l'ordine del BQS-15
namespace {
std::string scena_sturgeon(bool giocatore, double quota, bool mina) {
    const auto f = fs::temp_directory_path() / "m688_prova_ghiaccio_sturgeon.lua";
    std::ofstream o(f);
    o << "local script = { V = function(u) m688.vai_a(u, 40000, 0, 6, '') m688.salva_modello(u) end }\n"
         "return { id = 'PROVA_GHIACCIO_STURGEON', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12,"
         " paesaggio_sonoro = false, meteo = { fisso = { ghiaccio = 1 } }, script = script,";
    if (mina) o << " mine = { { tipo = 'pmr2', x = 1500, y = 0, lato = 'urss', armata = false } },";
    o << " varianti = { { probabilita = 1, unita = {\n";
    if (giocatore)
        o << "{ id = 0, classe = 'sturgeon', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -1000, y = 0 },"
             "  quota_m = " << quota << ", rotta = 90, velocita_kn = 6, " << SUB << " },\n";
    else
        o << "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -30000, y = -30000 },"
             "  quota_m = 150, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
             "{ id = 3, classe = 'sturgeon', categoria = 'sottomarino', flag = 9, posizione = { x = -1000, y = 0 },"
             "  quota_m = " << quota << ", rotta = 90, velocita_kn = 6, " << SUB << " },\n";
    o << "}, timer = { { dopo_s = 1, unita = " << (giocatore ? 0 : 3) << ", gestore = 'V' } } } } }\n";
    return f.string();
}
}  // namespace

TEST_CASE("ghiaccio nella missione: lo Sturgeon dell'IA col BQS-8 vede le chiglie e scende, ma non vede le mine" * doctest::skip(!c_e())) {
    {   // le chiglie: come il Victor
        Partita p(scena_sturgeon(false, 25.0, false));
        double piu_fondo = 25.0;
        for (int k = 0; k < 40; ++k) {
            p.avanza(30.0);
            piu_fondo = std::max(piu_fondo, p.m.unita().at(3).quota_ordinata);
        }
        const QuadroVolta* q = p.m.quadro_volta(3);
        REQUIRE(q);
        MESSAGE("lo Sturgeon dell'IA: quota ordinata piu' fonda ", piu_fondo, " m, celle della volta ", q->celle());
        CHECK(q->celle() > 0);
        CHECK(piu_fondo > 30.0);
    }
    {   // la PMR-2 a 300 m sulla rotta, in 400 m d'acqua: il BQS-8 non la vede
        Partita p(scena_sturgeon(false, 130.0, true), true, 400.0);
        double vicina = 1e9;
        for (int k = 0; k < 900; ++k) {
            p.avanza(1.0);
            const auto& mine = p.m.campo_minato().mine();
            if (!mine.empty()) vicina = std::min(vicina, distanza(p.m.unita().at(3).pos, mine.front().pos));
        }
        const auto* s = p.m.mine_scoperte_ia(3);
        MESSAGE("lo Sturgeon dell'IA e la PMR-2: piu' vicino ", vicina, " m, mine viste ", s ? s->size() : 0u);
        CHECK((!s || s->empty()));
        CHECK(vicina < 100.0);
    }
}

TEST_CASE("ghiaccio nella missione: lo Sturgeon del giocatore accende il BQS-8, che riempie la volta e non le mine" * doctest::skip(!c_e())) {
    const auto gioca = [](bool acceso) {
        Partita p(scena_sturgeon(true, 60.0, true), true, 400.0);
        p.avanza(1.0);
        const bool accettato = p.m.applica({p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, acceso ? 1 : 0});
        p.avanza(600.0);
        const QuadroVolta* q = p.m.quadro_volta(0);
        const std::size_t celle = q ? q->celle() : 0u;
        return std::pair<bool, std::size_t>{accettato, celle};
    };
    const auto [acceso, celle] = gioca(true);
    const auto [spento, celle_spento] = gioca(false);
    MESSAGE("lo Sturgeon del giocatore: celle della volta col BQS-8 ", celle, ", senza ", celle_spento);
    CHECK(acceso);
    CHECK(spento);
    CHECK(celle > celle_spento);   // le spazzate sulle chiglie si aggiungono al sondatore alto
}

TEST_CASE("ghiaccio nella missione: sotto il pack il BQS-15 dell'IA vede la PMR-2 e la aggira" * doctest::skip(!c_e())) {
    const auto gioca = [](double ghiaccio) {
        const auto f = fs::temp_directory_path() / "m688_prova_ghiaccio_pmr2.lua";
        std::ofstream(f) << "local script = { V = function(u) m688.vai_a(u, 4000, 0, 6, '') m688.salva_modello(u) end }\n"
                            "return { id = 'PROVA_GHIACCIO_PMR2', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, meteo = { fisso = { ghiaccio = " << ghiaccio << " } }, script = script,"
                            " mine = { { tipo = 'pmr2', x = 1500, y = 0, lato = 'urss', armata = false } },"
                            " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -30000, y = -30000 }, quota_m = 100, rotta = 90, velocita_kn = 0, " << SUB << " },\n"
                            "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = -1000, y = 0 },"
                            "  quota_m = 130, rotta = 90, velocita_kn = 6, " << SUB << " },\n"
                            "}, timer = { { dopo_s = 1, unita = 3, gestore = 'V' } } } } }\n";
        Partita p(f.string(), true, 400.0);
        double vicina = 1e9;
        for (int k = 0; k < 900; ++k) {   // un quarto d'ora
            p.avanza(1.0);
            const auto& mine = p.m.campo_minato().mine();
            if (!mine.empty()) vicina = std::min(vicina, distanza(p.m.unita().at(3).pos, mine.front().pos));
        }
        const auto* s = p.m.mine_scoperte_ia(3);
        const bool sotto = p.m.sotto_banchisa(3);
        fs::remove(f);
        MESSAGE("ghiaccio ", ghiaccio, ": sotto il pack ", sotto, ", piu' vicino alla PMR-2 ", vicina, " m, mine viste ", s ? s->size() : 0u);
        return std::pair<double, std::size_t>{vicina, s ? s->size() : 0u};
    };
    const auto [sotto, viste] = gioca(1.0);
    CHECK(viste == 1);
    CHECK(sotto > 200.0);   // al largo
    const auto [libera, viste_libera] = gioca(0.0);
    CHECK(viste_libera == 0);   // il campo e' sconosciuto e il fondo lontano: il BQS-15 e' spento
    CHECK(libera < 100.0);
}

TEST_CASE("ghiaccio nella missione: sotto il pack la chiglia confonde l'eco del bersaglio anche al ping dell'IA; in acqua libera no (simmetria, punto 6)" *
          doctest::skip(!c_e())) {
    // un Victor III a 60 m che pinga ogni 12 s un 688 dell'IA a 3 km, a 20 m fra le chiglie: nessun contatto fantasma; una
    // chiglia piu' forte nello stesso fascio e nella stessa cella darebbe la sua posizione e la sua quota, in acqua libera mai
    int confuse[2] = {0, 0}, echi[2] = {0, 0};
    for (const int i : {0, 1}) {
        const double g = i == 0 ? 1.0 : 0.0;
        const auto f = fs::temp_directory_path() / "m688_prova_ghiaccio_eco_ia.lua";
        std::ofstream(f) << "return { id = 'PROVA_GHIACCIO_ECO_IA', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, meteo = { fisso = { ghiaccio = " << g << " } },"
                            " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 20000, y = 20000 },"
                            "  quota_m = 200, rotta = 0, velocita_kn = 3, " << SUB << " },\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 80000, y = 50000 },"
                            "  quota_m = 60, rotta = 0, velocita_kn = 4, " << SUB << " },\n"
                            "{ id = 4, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 80000, y = 53000 },"
                            "  quota_m = 20, rotta = 90, velocita_kn = 4, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f.string());
        p.avanza(1.0);
        for (int n = 0; n < 50; ++n) {   // un ping ogni 12 s (l'IA non ripinga prima di 10), letto subito
            p.m.impulso_sonar_enhanced(3);
            const auto& cc = p.m.unita().at(3).contatti;
            if (const auto k = cc.find(4); k != cc.end() && k->second.ultima_scoperta == p.m.tempo() && k->second.sensore.find("attivo") != std::string::npos) {
                ++echi[i];
                if (std::fabs(k->second.quota_stimata - p.m.unita().at(4).quota) > 2.0) ++confuse[i];
            }
            p.avanza(12.0);
        }
        // nessun contatto fantasma: il Victor conosce solo unita' vere
        for (const auto& [id, c] : p.m.unita().at(3).contatti) CHECK(p.m.unita().count(id) == 1);
        fs::remove(f);
    }
    MESSAGE("echi del 688 al ping del Victor: pack ", echi[0], " (confusi con una chiglia ", confuse[0], "), acqua libera ", echi[1], " (", confuse[1], ")");
    CHECK(echi[0] >= 10);
    CHECK(echi[1] >= 10);
    // le chiglie rendono 0-5 dB, il 688 a 3 km 27: la confusione e' dei bersagli al limite della scoperta (rara qui)
    CHECK(confuse[1] == 0);
}
