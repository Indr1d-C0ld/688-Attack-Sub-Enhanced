#include <doctest.h>
#include <chrono>
#include <thread>

#include <algorithm>

#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <tuple>

#include "grafica/effetti.hpp"
#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/spaccato.hpp"
#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Sottomarini colpiti nelle missioni con la regola di simulazione (docs/10_danni_affondamento_esplosioni.md §10, D6c)

namespace {

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/scafi/los_angeles.toml"); }

std::string missione_sub(const std::string& unita, const std::string& nome) {
    const auto f = fs::temp_directory_path() / ("m688_prova_sub_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << unita << "\n}, timer = {} } } }\n";
    return f.string();
}

const std::string MODELLO = "modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = ";

// il giocatore (688) a 8 km, un Victor III fermo a 100 m e un siluro che gli arriva da sud alla stessa quota;
// con `bersaglio_giocatore` il giocatore e il Victor si scambiano il posto
std::string scena_bersaglio(const std::string& arma = "mk48", bool bersaglio_giocatore = false, double quota = 100) {
    const std::string q = std::to_string(quota);
    const std::string vicino = "posizione = { x = 0, y = 3000 }, quota_m = " + q + ", rotta = 90",
                      lontano = "posizione = { x = -8000, y = 0 }, quota_m = 60, rotta = 0";
    return "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, "
           + (bersaglio_giocatore ? vicino : lontano) + ", velocita_kn = 0, " + MODELLO + "160 } },"
           "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 9, "
           + (bersaglio_giocatore ? lontano : vicino) + ", velocita_kn = 0, " + MODELLO + "100 } },"
           "{ id = 3, classe = '" + arma + "', categoria = 'arma', flag = 41, posizione = { x = 0, y = 0 },"
           "  quota_m = " + q + ", rotta = 0, velocita_kn = 50, "
           "  modello = { massima_kn = 50, virata_gradi_s = 9, accel_kn_s = 5, quota_m_s = 5, quota_max_m = 500, integrita = 2, difesa = 75 } },";
}

// avanza missione e danni insieme, con la regola di simulazione (impatti su ogni coppia)
void gioca(Missione& m, DanniSottomarini& ds, double secondi) {
    for (int i = 0; i < static_cast<int>(secondi * 9); ++i) {
        m.avanza(1.0 / 9.0);
        ds.avanza(m, 1.0 / 9.0);
    }
}

bool c_e(const std::vector<Messaggio>& v, int codice) {
    for (const auto& x : v) if (x.codice == codice) return true;
    return false;
}

}  // namespace

TEST_CASE("danni dei sottomarini nelle missioni (regola di simulazione)" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");

    SUBCASE("Mk 48 su un Victor III a 100 m: urto sotto lo scafo, battello pesante, nessun rapporto al giocatore") {
        const auto f = missione_sub(scena_bersaglio(), "victor");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        gioca(m, ds, 300);
        REQUIRE(!m.colpi().empty());
        CHECK(m.alla_fisica(2));   // la distruzione la decide il motore, non l'integrita' dell'originale
        const auto* s = ds.sottomarino(2);
        REQUIRE(s != nullptr);
        CHECK(!s->danni->falle().empty());
        CHECK(s->danni->acqua_imbarcata_m3() > 10.0);
        REQUIRE(ds.sistemi(2) != nullptr);
        // la quota la decide la fisica: il Victor non resta a 100 m (scende allagato o risale esaurendo le casse)
        CHECK((m.unita().count(2) == 0 || std::fabs(m.unita().at(2).quota - 100.0) > 10.0 || s->danni->perso()));
        // i rapporti dell'equipaggio sono solo del battello del giocatore: del Victor solo cio' che il sonar sente
        for (const auto& x : ds.messaggi()) CHECK(x.codice >= msg_sottomarino::RUMORI_ROTTURA);
        fs::remove(f);
    }
    SUBCASE("siluro da 533 mm sul 688 del giocatore: rapporti dell'equipaggio e limiti per le stazioni") {
        const auto f = missione_sub(scena_bersaglio("type53", true), "giocatore");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 2);
        gioca(m, ds, 300);
        REQUIRE(!m.colpi().empty());
        REQUIRE(ds.sottomarino(0) != nullptr);
        CHECK(c_e(ds.messaggi(), msg_sottomarino::ALLAGAMENTO));
        CHECK(c_e(ds.messaggi(), msg_sottomarino::GUASTO));
        for (const auto& x : ds.messaggi()) CHECK(x.unita == 0);
        const auto l = ds.limiti(0);
        const auto& st = *ds.sistemi(0);
        CHECK(l.idroplani == (st[static_cast<std::size_t>(Sistema::idroplani)].efficienza >= 0.3));
        if (l.velocita < 1.0 && m.unita().count(0) && !ds.sottomarino(0)->danni->perso())
            CHECK(m.unita().at(0).ordine.velocita_max_kn >= 0.0);   // telegrafo limitato dal motore
        // a 100 m, fermo e senza ordini, un 533 a contatto lo appesantisce: scende (o e' gia' perso)
        CHECK((ds.sottomarino(0)->danni->perso() || m.unita().count(0) == 0 || m.unita().at(0).quota > 100.0));
        fs::remove(f);
    }
    SUBCASE("Ctrl+E del giocatore: esaurimento delle casse a 120 m, il 688 sale fino alla superficie") {
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 120, rotta = 0, velocita_kn = 0, " + MODELLO + "160 } },",
                                    "esaurimento");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.esaurisci(m, 0);
        gioca(m, ds, 10);
        REQUIRE(ds.sottomarino(0) != nullptr);
        CHECK(c_e(ds.messaggi(), msg_sottomarino::ESAURIMENTO));
        REQUIRE(ds.rateo_quota(0).has_value());
        CHECK(*ds.rateo_quota(0) < -0.5);       // leggero: sale (piu' di 0,5 m/s)
        CHECK(ds.limiti(0).velocita == 1.0);    // intatto: nessun limite
        // e arriva davvero in superficie, con la quota ordinata ancora a 120 m (la barra non conta piu')
        double t = 10;
        for (; t < 400 && m.unita().at(0).quota > 0.0; t += 1.0) gioca(m, ds, 1.0);
        CHECK(m.unita().at(0).quota == 0.0);
        CHECK(t < 300);
        CHECK(m.unita().at(0).quota_ordinata == doctest::Approx(120.0));
        fs::remove(f);
    }
    SUBCASE("un sottomarino mai colpito non ha modello: tutto disponibile") {
        const auto f = missione_sub(scena_bersaglio(), "intatto");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        m.avanza(1.0);
        ds.avanza(m, 1.0);
        CHECK(ds.sistemi(0) == nullptr);
        CHECK(ds.limiti(0).idroplani);
        CHECK(ds.messaggi().empty());
        fs::remove(f);
    }
}

// Esiti dei sottomarini colpiti e segni in superficie (fase D6e, dossier 07 §8)
TEST_CASE("sottomarini colpiti: esiti e segni in superficie" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    auto ha_segno = [](const DanniSottomarini& ds, SegnoSuperficie::Tipo t) {
        for (const auto& x : ds.segni()) if (x.tipo == t) return true;
        return false;
    };

    SUBCASE("rottura profonda: nafta e pochi rottami in superficie dopo 10-60 minuti, dove il battello si e' perso") {
        const auto f = missione_sub(scena_bersaglio("mk48", false, 400), "profondo");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        // perso in profondita' (collasso o tutto allagato, prima di risalire): l'istante della perdita
        double perdita = -1;
        for (int t = 0; t < 1800; ++t) {
            gioca(m, ds, 1.0);
            if (perdita < 0 && ds.sottomarino(2) && ds.sottomarino(2)->distrutto) perdita = m.tempo();
        }
        const auto* s = ds.sottomarino(2);
        REQUIRE(s != nullptr);
        REQUIRE(perdita >= 0);   // un Mk 48 sotto lo scafo a 400 m: letale (dossier 02 §9.4)
        REQUIRE(ha_segno(ds, SegnoSuperficie::Tipo::nafta));
        CHECK_FALSE(ha_segno(ds, SegnoSuperficie::Tipo::aria));
        for (const auto& x : ds.segni()) {
            CHECK(x.inizio >= perdita + 600.0 - 2.0);
            CHECK(x.inizio <= perdita + 3600.0 + 2.0);
            CHECK(distanza(x.pos, Vec2{0, 3000}) < 500.0);
        }
        fs::remove(f);
    }
    SUBCASE("a galla: con la fisica perso vuol dire perso, e la missione finisce; senza resta il relitto dell'originale") {
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                                    " quota_m = 0.4, rotta = 0, velocita_kn = 10, " + MODELLO + "160 } }", "perso_a_galla");
        {
            Missione m(f, 1, false);
            m.senza_giocatore(true);
            m.impatti_dell_originale(false);
            DanniSottomarini ds(classi, M688_DATI, 3);
            gioca(m, ds, 2);
            REQUIRE(m.alla_fisica(0));
            m.distruggi(0);
            gioca(m, ds, 300);
            CHECK((!m.unita().count(0) || m.unita().at(0).distrutta));
            CHECK(m.esito().has_value());
        }
        {   // il giocatore umano: stessa fine del comandante IA
            Missione m(f, 1, false);
            m.impatti_dell_originale(false);
            DanniSottomarini ds(classi, M688_DATI, 3);
            gioca(m, ds, 2);
            REQUIRE(m.alla_fisica(0));
            m.distruggi(0);
            gioca(m, ds, 300);
            CHECK((!m.unita().count(0) || m.unita().at(0).distrutta));
            CHECK(m.esito().has_value());
        }
        {
            Missione m(f, 1, false);
            m.impatti_dell_originale(false);
            for (int i = 0; i < 18; ++i) m.avanza(1.0 / 9.0);
            REQUIRE_FALSE(m.alla_fisica(0));
            m.distruggi(0);
            for (int i = 0; i < 2700; ++i) m.avanza(1.0 / 9.0);
            CHECK_FALSE(m.unita().at(0).distrutta);
            CHECK((m.unita().at(0).flag54 & 0x80) != 0);
        }
        {   // emerso danneggiato: il comandante IA va al piu' a 3 nodi come l'IA
            Missione m(f, 1, false);
            m.senza_giocatore(true);
            m.impatti_dell_originale(false);
            DanniSottomarini ds(classi, M688_DATI, 3);
            gioca(m, ds, 2);
            for (const double kg : {10.0, 20.0, 40.0, 80.0, 160.0, 320.0}) {
                if (ds.sottomarino(0) && ds.sottomarino(0)->emerso_danneggiato) break;
                ds.urto(m, 0, kg, 15.0, 1.5);
                gioca(m, ds, 20);
            }
            REQUIRE(ds.sottomarino(0) != nullptr);
            CHECK(ds.sottomarino(0)->emerso_danneggiato);
            REQUIRE(m.unita().count(0));
            CHECK(m.unita().at(0).ordine.velocita_max_kn <= 3.0);
        }
        fs::remove(f);
    }
    SUBCASE("fondale a 150 m: il comandante IA, che ha giocatore = true, tenta le casse come l'IA") {
        const auto f = missione_sub(scena_bersaglio("type53", true, 140), "fondo_comandante");
        const auto soffiate = [&](bool comandante) {
            Missione m(f, 1, false);
            m.senza_giocatore(comandante);
            m.impatti_dell_originale(false);
            m.imposta_fondale(std::make_shared<Fondale>(400, 400, 100.0, 1.0, -150.0, std::vector<std::uint16_t>(400 * 400, 0)));
            DanniSottomarini ds(classi, M688_DATI, 3);
            gioca(m, ds, 1800);
            REQUIRE(ds.sottomarino(0) != nullptr);
            const auto& ev = ds.sottomarino(0)->danni->eventi();
            return std::count_if(ev.begin(), ev.end(), [](const EventoBattello& e) { return e.tipo == EventoBattello::Tipo::esaurimento; });
        };
        CHECK(soffiate(false) == 0);   // il giocatore esaurisce solo se lo ordina
        CHECK(soffiate(true) >= 1);
        fs::remove(f);
    }
    SUBCASE("fondale a 150 m: il giocatore pesante sul fondo riceve il rapporto; perso solo se tutto allagato") {
        const auto f = missione_sub(scena_bersaglio("type53", true, 140), "fondo_giocatore");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        m.imposta_fondale(std::make_shared<Fondale>(400, 400, 100.0, 1.0, -150.0, std::vector<std::uint16_t>(400 * 400, 0)));
        DanniSottomarini ds(classi, M688_DATI, 3);
        gioca(m, ds, 1800);
        REQUIRE(ds.sottomarino(0) != nullptr);
        const auto& b = *ds.sottomarino(0)->danni;
        CHECK(c_e(ds.messaggi(), msg_sottomarino::FERMI_SUL_FONDO));
        CHECK(b.aria_bombole_Nm3() > 0.0);   // il giocatore esaurisce solo se lo ordina
        CHECK_FALSE(b.perso());              // a 150 m nessun collasso
        bool allagato = true;
        for (const auto& c : b.compartimenti()) allagato &= c.aria() < 0.1 * c.volume;
        // perso (affondamento avviato o tolto) solo se tutti i compartimenti sono allagati
        const bool perso = !m.unita().count(0) || m.unita().at(0).affondamento > 0 || m.unita().at(0).distrutta;
        CHECK(perso == allagato);
        if (perso) CHECK(ha_segno(ds, SegnoSuperficie::Tipo::aria));
        fs::remove(f);
    }
    SUBCASE("fondale a 150 m: un Victor III colpito risale esaurendo le casse o resta sul fondo e si perde") {
        int esiti = 0;
        for (std::uint64_t seme = 1; seme <= 4; ++seme) {
            const auto f = missione_sub(scena_bersaglio("type53", false, 140), "fondo_ia");
            Missione m(f, seme, false);
            m.impatti_dell_originale(false);
            m.imposta_fondale(std::make_shared<Fondale>(400, 400, 100.0, 1.0, -150.0, std::vector<std::uint16_t>(400 * 400, 0)));
            DanniSottomarini ds(classi, M688_DATI, seme);
            gioca(m, ds, 3600);
            const auto* s = ds.sottomarino(2);
            REQUIRE(s != nullptr);
            const bool emerso = s->emerso_danneggiato;
            const bool perso = s->distrutto || (m.unita().count(2) && m.unita().at(2).affondamento > 0);
            CHECK((emerso || perso));
            esiti += emerso || perso;
            if (emerso && !perso && m.unita().count(2)) {
                CHECK(m.unita().at(2).quota == 0.0);
                CHECK(m.unita().at(2).ordine.velocita_max_kn <= 3.0);   // fermo in superficie
            }
            if (perso && s->segni_fatti) CHECK(ha_segno(ds, SegnoSuperficie::Tipo::aria));   // perso vicino alla superficie
            CHECK(c_e(ds.messaggi(), msg_sottomarino::EMERSIONE_NEMICA));   // l'IA ha esaurito le casse
            fs::remove(f);
        }
        CHECK(esiti == 4);
    }
}

// Dati per la stazione Stato e danni (fase D6d): rapporti in ordine, ultima scossa, compartimenti
TEST_CASE("sottomarini colpiti: dati per la stazione Stato" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto f = missione_sub(scena_bersaglio("type53", true, 100), "stato");
    Missione m(f, 1, false);
    m.impatti_dell_originale(false);
    DanniSottomarini ds(classi, M688_DATI, 2);
    // prima del colpo: il giocatore preparato all'avvio ha compartimenti e sistemi, tutto asciutto e al 100%
    CHECK(ds.compartimenti(0) == nullptr);
    ds.prepara(m, 0);
    REQUIRE(ds.compartimenti(0) != nullptr);
    CHECK(ds.compartimenti(0)->size() == 3);
    for (const auto& c : *ds.compartimenti(0)) {
        CHECK(c.acqua == 0.0);
        CHECK(c.x1 > c.x0);
    }
    CHECK(ds.rapporti(0).empty());
    CHECK(ds.scossa(0).tempo < 0.0);
    gioca(m, ds, 20);
    CHECK(ds.rapporti(0).empty());   // preparato ma intatto: nessun evento, nessun ordine di quota
    CHECK_FALSE(ds.rateo_quota(0).has_value());
    // il colpo: rapporti in ordine di tempo di missione, l'ultima scossa all'istante del colpo
    gioca(m, ds, 280);
    REQUIRE(!m.colpi().empty());
    const auto& r = ds.rapporti(0);
    REQUIRE(!r.empty());
    for (std::size_t i = 1; i < r.size(); ++i) CHECK(r[i].tempo >= r[i - 1].tempo);
    const auto sc = ds.scossa(0);
    CHECK(sc.intensita > 0.05);
    CHECK(sc.tempo == doctest::Approx(m.colpi().front().tempo).epsilon(0.01));
    double acqua = 0;
    for (const auto& c : *ds.compartimenti(0)) acqua += c.acqua;
    CHECK(acqua > 0.0);
    CHECK(ds.rapporti(2).empty());   // un battello senza modello: nessun rapporto
    fs::remove(f);
}

TEST_CASE("sottomarini: isolare un compartimento, l'ordine del giocatore (docs/10 §10.19)" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    CHECK(ordine_da_nome(nome_ordine(TipoOrdine::isola_compartimento)) == TipoOrdine::isola_compartimento);
    CHECK(ordine_da_nome(nome_ordine(TipoOrdine::priorita_sistema)) == TipoOrdine::priorita_sistema);
    const auto c_e_msg = [](const std::vector<Messaggio>& v, int codice) {
        return std::any_of(v.begin(), v.end(), [&](const Messaggio& x) { return x.codice == codice; });
    };
    SUBCASE("688: rifiuto del comando, sgombero, porte, caduti nell'esito") {
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "isola");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 0}));
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 418));
        CHECK_FALSE((*ds.compartimenti(0))[0].senza_squadre());
        CHECK_FALSE(c_e_msg(ds.messaggi(), 468));
        ds.incendio(m, 0, 2, 0.8);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 2}));
        CHECK((*ds.compartimenti(0))[2].porte_s >= 0.0);
        gioca(m, ds, 30);
        CHECK(c_e_msg(ds.messaggi(), 415));
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 1.0, 0.0, 2}));
        const int dentro = (*ds.compartimenti(0))[2].dentro;
        CHECK(dentro > 0);
        for (int i = 0; i < 1200; ++i) {
            gioca(m, ds, 1);
            m.raccogli_danni(&ds, nullptr);
        }
        CHECK(c_e_msg(ds.messaggi(), 416));
        CHECK(c_e_msg(ds.messaggi(), 419));
        CHECK(ds.sottomarino(0)->danni->caduti() == dentro);
        CHECK(m.esito_dettagliato().morti == dentro);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 2.0, 0.0, 2}));
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 417));
        CHECK(ds.consegna(m, {0.0, TipoOrdine::priorita_sistema, 0.0, 0.0, static_cast<int>(Sistema::sonar_prua)}));
        CHECK(ds.sottomarino(0)->danni->ordine_riparazioni().front() == Sistema::sonar_prua);
        fs::remove(f);
    }
    SUBCASE("un compartimento sventrato non si isola, e lo si dice") {
        const auto f = missione_sub("{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "isola_perso");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        ds.esplosione_interna(m, 0, 4, 30.0);
        REQUIRE((*ds.compartimenti(0))[4].distrutto);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 4}));
        CHECK_FALSE((*ds.compartimenti(0))[4].senza_squadre());
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 468));
        fs::remove(f);
    }
    SUBCASE("l'Alfa si chiude subito, senza conto") {
        const auto f = missione_sub("{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "isola_alfa");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 4}));
        CHECK((*ds.compartimenti(0))[4].isolato);
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 416));
        CHECK_FALSE(c_e_msg(ds.messaggi(), 415));
        fs::remove(f);
    }
}

TEST_CASE("sottomarini: casse d'assetto per posarsi sul fondo" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto f = missione_sub(scena_bersaglio("mk48", false, 100), "assetto");
    Missione m(f, 1, false);
    DanniSottomarini ds(classi, M688_DATI, 1);
    ds.appesantisci(m, 0, 2.0);
    gioca(m, ds, 60);
    // il peso arriva alla dinamica dell'Enhanced con l'ordine del motore
    CHECK(m.unita().at(0).ordine.peso_netto_N == doctest::Approx(2000.0 * 9.81).epsilon(0.01));
    CHECK(ds.rapporti(0).empty());
    ds.alleggerisci(0);
    gioca(m, ds, 180);
    CHECK(m.unita().at(0).ordine.peso_netto_N == doctest::Approx(0.0));
    fs::remove(f);
}

TEST_CASE("sottomarini colpiti: urto di prova per le istantanee" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto f = missione_sub(scena_bersaglio("mk48", false, 100), "urto_prova");
    Missione m(f, 1, false);
    DanniSottomarini ds(classi, M688_DATI, 1);
    ds.prepara(m, 0);
    gioca(m, ds, 20);
    ds.urto(m, 0, 135, 15, 1.5707963);   // carica da 135 kg a 15 m sotto lo scafo: guasti, falle, scossa forte
    CHECK(!ds.rapporti(0).empty());
    CHECK(ds.scossa(0).tempo == doctest::Approx(m.tempo()));
    CHECK(ds.scossa(0).intensita > 0.3);
    gioca(m, ds, 60);
    double acqua = 0;
    for (const auto& c : *ds.compartimenti(0)) acqua += c.acqua;
    CHECK(acqua > 0.0);
    CHECK(c_e(ds.messaggi(), msg_sottomarino::ALLAGAMENTO));
    fs::remove(f);
}

// Battello emerso danneggiato: fisica delle navi in superficie; geometria vera dello scoppio dei siluri del modello
TEST_CASE("sottomarini colpiti: emersione con la fisica delle navi e geometria dello scoppio" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    auto scena_kilo = [&](const std::string& nome) {
        return missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -8000, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 0, " + MODELLO + "160 } },"
                            "{ id = 2, classe = 'kilo', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 3000 },"
                            "  quota_m = 60, rotta = 90, velocita_kn = 0, " + MODELLO + "100 } },", nome);
    };
    SUBCASE("Kilo colpito a 60 m: riemerge, passa alla fisica delle navi, sbanda in superficie, posa nel mondo") {
        const auto f = scena_kilo("emerso");
        Missione m(f, 3, false);
        DanniSottomarini ds(classi, M688_DATI, 3);
        ds.urto(m, 2, 300, 10, 0.0);   // 300 kg a 10 m di fianco
        gioca(m, ds, 1800);
        const auto* s = ds.sottomarino(2);
        REQUIRE(s != nullptr);
        REQUIRE(s->superficie != nullptr);
        CHECK(m.unita().at(2).quota == 0.0);
        CHECK(m.unita().at(2).ordine.velocita_max_kn <= 3.0);
        CHECK(std::fabs(s->superficie->sbandamento_gradi()) > 5.0);   // l'acqua a bordo lo fa sbandare
        Mondo mondo;
        m.sincronizza(mondo);
        ds.sincronizza(mondo);
        REQUIRE(mondo.trova(2) != nullptr);
        CHECK(mondo.trova(2)->posa.danneggiata);
        fs::remove(f);
    }
    SUBCASE("mina a influenza (M2b): la carica vera sostituisce quella dell'arma") {
        const auto f = scena_kilo("mina");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        m.avanza(1.0);
        Colpo c;
        c.tipo = Colpo::Tipo::arma;
        c.tempo = m.tempo(); c.vittima = 2; c.classe_vittima = "kilo"; c.classe_arma = "mk67";
        c.tnt_kg = 150.0; c.influenza = true;
        c.geometria_vera = true; c.distanza_m = 20.0; c.dz_m = 20.0;
        m.registra_colpo(c);
        ds.avanza(m, 1.0);
        REQUIRE(ds.sottomarino(2) != nullptr);
        const double k = ds.scossa(2).intensita;
        CHECK(k == doctest::Approx(std::sqrt(150.0 * 2.20462) / ((20.0 - 4.78 + 1.21) * 3.28084) / (1 - 0.5 * 60.0 / 300.0)).epsilon(0.05));
        fs::remove(f);
    }
    SUBCASE("siluro del modello: distanza e incidenza vere al posto delle stime per arma") {
        const auto f = scena_kilo("geometria");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        m.avanza(1.0);
        Colpo c;
        c.tipo = Colpo::Tipo::arma;
        c.tempo = m.tempo(); c.vittima = 2; c.classe_vittima = "kilo"; c.classe_arma = "mk48";
        c.geometria_vera = true; c.distanza_m = 20.0; c.dz_m = 20.0;   // 20 m sotto il centro: Mk 48 lontano dallo scafo
        m.registra_colpo(c);
        ds.avanza(m, 1.0);
        REQUIRE(ds.sottomarino(2) != nullptr);
        // 544 kg di TNT (295 di PBXN-103) a 20 - 4,78 m dallo scafo (meta' larghezza del Kilo) piu' 1,21 m d'intercapedine
        // fino allo scafo resistente, da sotto, a 60 m (soglia x 0,9): fattore d'urto circa 0,7, non oltre 2 come con la
        // stima d'influenza a 3-8 m
        const double k = ds.scossa(2).intensita;
        CHECK(k == doctest::Approx(std::sqrt(544.0 * 2.20462) / ((20.0 - 4.78 + 1.21) * 3.28084) / (1 - 0.5 * 60.0 / 300.0)).epsilon(0.05));
        fs::remove(f);
    }
    SUBCASE("Mk 46 a contatto della poppa: la distanza e' dallo scafo, non dal centro") {
        const auto f = scena_kilo("poppa");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        m.avanza(1.0);
        const UnitaClassica& v = m.unita().at(2);
        // 34 m a poppavia del centro (il Kilo e' lungo 72,6 m) e 2 m di lato, alla quota della vittima
        const double r = radianti(v.rotta), lungo = -34.0, lato = 2.0;
        Colpo c;
        c.tipo = Colpo::Tipo::arma;
        c.tempo = m.tempo(); c.vittima = 2; c.classe_vittima = "kilo"; c.classe_arma = "mk46";
        c.pos = {v.pos.x + lungo * std::sin(r) + lato * std::cos(r), v.pos.y + lungo * std::cos(r) - lato * std::sin(r)};
        c.geometria_vera = true; c.distanza_m = std::hypot(lungo, lato); c.dz_m = 0.0;
        m.registra_colpo(c);
        ds.avanza(m, 1.0);
        REQUIRE(ds.sottomarino(2) != nullptr);
        // 108 kg di TNT equivalente a 0,5 m piu' l'intercapedine (1,21 m), di lato (KSF = HSF / 2), a 60 m: oltre 0,6,
        // la rottura dello scafo resistente; col centro sarebbero stati 34 - 4,78 m e un fattore d'urto di 0,07
        const double k = ds.scossa(2).intensita;
        CHECK(k == doctest::Approx(std::sqrt(108.0 * 2.20462) / ((0.5 + 1.21) * 3.28084) / 2 / (1 - 0.5 * 60.0 / 300.0)).epsilon(0.05));
        CHECK(k > 1.0);
        fs::remove(f);
    }
}

// Incendi a bordo (docs/10 §10.13): l'IA emerge per un incendio fuori controllo o per il fumo
TEST_CASE("sottomarini: incendi a bordo, casi storici" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    auto scena = [&](const std::string& classe, double quota, const std::string& nome) {
        return missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = -8000, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 0, " + MODELLO + "160 } },"
                            "{ id = 2, classe = '" + classe + "', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 3000 },"
                            "  quota_m = " + std::to_string(quota) + ", rotta = 90, velocita_kn = 6, " + MODELLO + "100 } },", nome);
    };
    auto ultimo = [](const BattelloDanni& b, const char* uso) {
        int k = -1;
        for (std::size_t i = 0; i < b.compartimenti().size(); ++i) if (b.compartimenti()[i].uso == uso) k = static_cast<int>(i);
        return k;
    };
    SUBCASE("Komsomolets (1989): incendio a lampo in sala macchine a 380 m con l'aria ad alta pressione, emerge e galleggia") {
        // il Komsomolets emerse in ~13 minuti e resto' a galla ~5 ore (dossier 07 §8.4); qui un Akula
        const auto f = scena("akula", 380, "komsomolets");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 2);
        ds.incendio(m, 2, ultimo(*ds.sottomarino(2)->danni, "macchine"), 0.6, true);
        double emerso = -1;
        for (int t = 0; t < 3 * 3600; ++t) {
            gioca(m, ds, 1.0);
            if (emerso < 0 && m.unita().at(2).quota <= 0.5) emerso = t;
        }
        CHECK(emerso > 0);
        CHECK(emerso < 20 * 60);
        CHECK_FALSE(ds.sottomarino(2)->distrutto);   // a galla dopo 3 ore
        REQUIRE(ds.sottomarino(2)->superficie != nullptr);
        bool aria = false;
        for (const auto& e : ds.rapporti(2)) aria |= e.tipo == EventoBattello::Tipo::aria_nell_incendio;
        CHECK(aria);
        fs::remove(f);
    }
    SUBCASE("Bonefish (1988): incendio delle batterie a 60 m, il fumo costringe all'emersione") {
        const auto f = scena("kilo", 60, "bonefish");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 2);
        ds.incendio(m, 2, ultimo(*ds.sottomarino(2)->danni, "batterie"), 0.5);
        double emerso = -1;
        for (int t = 0; t < 1800 && emerso < 0; ++t) {
            gioca(m, ds, 1.0);
            if (m.unita().at(2).quota <= 0.5) emerso = t;
        }
        CHECK(emerso > 0);
        CHECK(emerso < 15 * 60);
        fs::remove(f);
    }
    SUBCASE("incendio sul 688 del giocatore: rapporti 284 e 286") {
        const auto f = scena("kilo", 60, "incendio_giocatore");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        ds.incendio(m, 0, 0, 0.1);   // piccolo: le squadre lo spengono
        gioca(m, ds, 600);
        CHECK(c_e(ds.messaggi(), msg_sottomarino::INCENDIO));
        CHECK(c_e(ds.messaggi(), msg_sottomarino::INCENDIO_SPENTO));
        fs::remove(f);
    }
}

// Emersione violenta (E1, docs/10 §10.14): l'IA che esaurisce le casse arriva in superficie veloce, la prua esce con
// un angolo, ricade, rolla e si assesta
TEST_CASE("sottomarini colpiti: emersione violenta dopo l'esaurimento delle casse" * doctest::skip(!ci_sono_dati())) {
    SUBCASE("posa: la prua sale fino all'angolo, ricade, poi oscillazioni che si smorzano") {
        EmersioneViolenta e;
        e.unita = 2; e.inizio = 100; e.lunghezza = 107; e.angolo = 30; e.fuori = 0.3; e.salita_s = 3; e.caduta_s = 2;
        CHECK(e.altezza_prua() == doctest::Approx(0.3 * 107 * 0.5));
        CHECK(posa_emersione(e, 100).prodiero.assetto == doctest::Approx(0.0));
        CHECK(posa_emersione(e, 100).prodiero.immersione == doctest::Approx(4.0));
        CHECK(posa_emersione(e, 103).prodiero.assetto == doctest::Approx(30.0));
        CHECK(posa_emersione(e, 104).prodiero.assetto == doctest::Approx(30.0 * 0.75));
        CHECK(posa_emersione(e, 103).prodiero.x_perno == doctest::Approx(107 / 2.0 - 0.3 * 107));
        double max_tardi = 0;
        for (double t = e.ricaduta() + 25; t < e.fine(); t += 0.5) {
            const auto p = posa_emersione(e, t).prodiero;
            max_tardi = std::max({max_tardi, std::fabs(p.assetto), std::fabs(p.sbandamento), std::fabs(p.immersione)});
        }
        CHECK(max_tardi < 0.5);   // dopo mezzo minuto e' quasi fermo
        bool rollio = false;
        for (double t = e.ricaduta(); t < e.ricaduta() + 10; t += 0.5)
            rollio |= std::fabs(posa_emersione(e, t).prodiero.sbandamento) > 1.0;
        CHECK(rollio);
    }
    SUBCASE("effetto: acqua dalla prua, colpo sul mare con la ricaduta, schiuma che resta minuti") {
        // Victor III (107 m) a 3 m/s verso est: prua fuori per il 35%, 30 gradi, ricaduta dopo 5 s
        Effetti fx(5);
        Mondo mondo;
        const double L = 107, fuori = 0.35, angolo = 30, ricaduta = 5;
        fx.emersione_violenta({0, 0}, 90, 3, angolo, L, fuori, ricaduta);
        const double hb = fuori * L * std::sin(angolo * 3.14159265 / 180);
        double z_prima = 0, z_dopo = 0, y_max = 0;
        bool lontano = false;
        for (int i = 0; i < 30 * 12; ++i) {
            fx.avanza(1.0 / 30, mondo);
            for (const auto& p : fx.particelle()) {
                if (p.ritardo > 0 || p.piatta || p.materia != Materia::spruzzo) continue;
                (fx.tempo() < ricaduta ? z_prima : z_dopo) = std::max(fx.tempo() < ricaduta ? z_prima : z_dopo, p.z);
                // spruzzi del colpo: ai lati dello scafo (asse est-ovest), larghi una decina di metri
                if (fx.tempo() > ricaduta + 0.5) y_max = std::max(y_max, std::fabs(p.y));
                lontano |= p.x > L / 2 + 3 * ricaduta + 15 || p.x < -L / 2 - 15;   // avanza a 3 m/s
            }
        }
        CHECK(z_prima > 0.5 * hb);    // la colonna alla prua sale fin quasi alla prua
        CHECK(z_prima < 1.5 * hb);
        CHECK(z_dopo > 3.0);          // il colpo della prua sul mare
        CHECK(y_max > 5.0);
        CHECK(y_max < 0.3 * L);
        CHECK_FALSE(lontano);         // niente oltre la prua (dove arriva) ne' dietro la poppa
        for (int i = 0; i < 30 * 120; ++i) fx.avanza(1.0 / 30, mondo);
        int schiuma = 0;
        for (const auto& p : fx.particelle()) schiuma += p.materia == Materia::schiuma && p.piatta;
        CHECK(schiuma >= 6);          // dopo due minuti la scia bianca lungo lo scafo resta
    }
    SUBCASE("Victor III con una falla a 100 m che esaurisce: arriva a ~5 m/s, la prua esce e poi la posa nel mondo") {
        const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = -8000, y = 0 }, quota_m = 60, rotta = 0, velocita_kn = 0, " + MODELLO + "160 } },"
                                    "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 3000 },"
                                    "  quota_m = 100, rotta = 90, velocita_kn = 8, " + MODELLO + "100 } },", "emersione_violenta");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.urto(m, 2, 150, 15, 0.0);   // falla leggera: da solo affonderebbe piano
        gioca(m, ds, 0.2);
        ds.esaurisci(m, 2);
        for (int t = 0; t < 300 && ds.emersioni().empty(); ++t) gioca(m, ds, 1.0);
        REQUIRE(ds.emersioni().size() == 1);
        const auto e = ds.emersioni().front();
        CHECK(e.unita == 2);
        CHECK(e.salita > 3.0);          // esaurimento da 100 m: 4-5 m/s all'arrivo
        CHECK(e.angolo >= 20.0);
        CHECK(e.angolo <= 35.0);
        CHECK(e.fuori >= 0.25);
        CHECK(e.fuori <= 0.40);
        CHECK(e.lunghezza > 90.0);
        CHECK(e.inizio <= m.tempo());
        CHECK(distanza(e.pos, Vec2{0, 3000}) < 1000.0);
        CHECK(c_e(ds.messaggi(), msg_sottomarino::EMERSIONE_NEMICA));
        gioca(m, ds, e.inizio + e.salita_s - m.tempo());
        Mondo mondo;
        m.sincronizza(mondo);
        ds.sincronizza(mondo);
        REQUIRE(mondo.trova(2) != nullptr);
        CHECK(mondo.trova(2)->posa.danneggiata);
        CHECK(mondo.trova(2)->posa.prodiero.assetto > 0.8 * e.angolo);   // al culmine
        gioca(m, ds, 120);
        CHECK(ds.emersioni().size() == 1);   // una sola per battello
        m.sincronizza(mondo);
        ds.sincronizza(mondo);
        CHECK(std::fabs(mondo.trova(2)->posa.prodiero.assetto) < 5.0);   // finita: la posa e' quella della fisica delle navi
        fs::remove(f);
    }
    SUBCASE("il giocatore che esaurisce non ha l'emersione violenta (la vede solo chi guarda al periscopio)") {
        const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 3000 }, quota_m = 100, rotta = 90, velocita_kn = 8, " + MODELLO + "160 } },",
                                    "emersione_giocatore");
        Missione m(f, 1, false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.urto(m, 0, 150, 15, 0.0);
        gioca(m, ds, 0.2);
        ds.esaurisci(m, 0);
        gioca(m, ds, 120);
        CHECK(m.unita().at(0).quota < 1.0);
        CHECK(ds.emersioni().empty());
        fs::remove(f);
    }
}

// Effetti visibili al periscopio dei sottomarini (E2, docs/10 §10.15): lancio di missili dall'immersione, piuma
TEST_CASE("effetti: missili lanciati da un sottomarino in immersione") {
    // quota massima del corpo, del fumo e quando compare la fiamma
    struct Esito { double t_uscita = -1, t_fiamma = -1, z_corpo = 0, z_fumo = 0, d_corpo = 0; int fumo = 0; bool tappi = false; };
    auto lancia = [](const std::string& missile, double quota, double secondi) {
        Effetti fx(3);
        Mondo mondo;
        Esito e;
        e.t_uscita = fx.lancio_missile({0, 0}, 0, quota, missile);
        for (int i = 0; i < static_cast<int>(secondi * 20); ++i) {
            fx.avanza(0.05, mondo);
            for (const auto& p : fx.particelle()) {
                if (p.ritardo > 0) continue;
                if (p.materia == Materia::fuoco && e.t_fiamma < 0) e.t_fiamma = fx.tempo();
                if (p.materia == Materia::detrito && p.propulsa) { e.z_corpo = std::max(e.z_corpo, p.z); e.d_corpo = std::max(e.d_corpo, p.y); }
                if (p.materia == Materia::detrito && !p.propulsa && p.r < 0.25) e.tappi = true;
                if (p.materia == Materia::fumo) { e.z_fumo = std::max(e.z_fumo, p.z); ++e.fumo; }
            }
        }
        return e;
    };
    SUBCASE("Harpoon da 40 m: la capsula affiora dopo 10-30 s, tappi, booster di 3 s, crociera radente") {
        const auto e = lancia("harpoon", 40, 60);
        CHECK(e.t_uscita >= 10.0);
        CHECK(e.t_uscita <= 30.0);
        CHECK(e.tappi);
        CHECK(e.t_fiamma >= e.t_uscita);
        CHECK(e.t_fiamma < e.t_uscita + 0.5);
        CHECK(e.z_fumo > 30.0);       // il getto di fumo, lungo 50-150 m a 20-40 gradi
        CHECK(e.z_fumo < 100.0);
        CHECK(e.d_corpo > 2000.0);    // dopo 20 s il missile e' a qualche km
    }
    SUBCASE("Tomahawk: esce gia' spinto, 12 s di fumo, sale a qualche centinaio di metri e torna basso") {
        const auto e = lancia("tomahawk", 30, 40);
        CHECK(e.t_uscita < 5.0);
        CHECK(e.z_corpo > 150.0);
        CHECK(e.z_corpo < 800.0);
        CHECK(e.fumo > 100);
    }
    SUBCASE("Granit dall'Oscar: sale ripido oltre il chilometro") {
        const auto e = lancia("granit", 20, 25);
        CHECK(e.z_corpo > 1000.0);
    }
    SUBCASE("fumo che resta mezzo minuto dopo il booster, missile sparito") {
        Effetti fx(4);
        Mondo mondo;
        const double t0 = fx.lancio_missile({0, 0}, 90, 30, "harpoon");
        for (int i = 0; i < static_cast<int>((t0 + 20) * 20); ++i) fx.avanza(0.05, mondo);
        int fumo = 0, corpo = 0;
        for (const auto& p : fx.particelle()) { fumo += p.materia == Materia::fumo; corpo += p.propulsa && p.materia == Materia::detrito; }
        CHECK(fumo > 10);
        CHECK(corpo >= 1);   // in crociera
        for (int i = 0; i < 20 * 20; ++i) fx.avanza(0.05, mondo);
        corpo = 0;
        for (const auto& p : fx.particelle()) corpo += p.propulsa && p.materia == Materia::detrito;
        CHECK(corpo == 0);   // oltre la fine della traiettoria disegnata
    }
    SUBCASE("dal ponte di una nave: subito, niente spruzzo ne' capsula, la nube sul ponte e il missile che sale") {
        Effetti fx(5);
        Mondo mondo;
        CHECK(fx.lancio_missile({0, 0}, 90, 6.0, "harpoon", true) == 0.0);
        int spruzzo = 0;
        for (const auto& p : fx.particelle()) spruzzo += p.materia == Materia::spruzzo;
        CHECK(spruzzo == 0);
        for (int i = 0; i < 2 * 20; ++i) fx.avanza(0.05, mondo);   // 2 s: il booster brucia
        int fumo = 0, fuoco = 0;
        double z = 0;
        for (const auto& p : fx.particelle()) {
            fumo += p.materia == Materia::fumo;
            fuoco += p.materia == Materia::fuoco;
            if (p.propulsa && p.materia == Materia::detrito) z = std::max(z, p.z);
        }
        CHECK(fumo > 10);
        CHECK(fuoco >= 1);
        CHECK(z > 6.0);   // il corpo sale sopra il ponte
    }
}

TEST_CASE("effetti: piuma del periscopio che cresce con la velocita'") {
    auto scia = [](double nodi) {
        Effetti fx(2);
        Mondo mondo;
        const double v = nodi * 0.514444;
        Vec2 pos{0, 0};
        for (int i = 0; i < 30 * 20; ++i) {   // 30 s verso nord
            fx.piuma(7, pos, 0, v, 2.0, 0.05);
            fx.avanza(0.05, mondo);
            pos.y += v * 0.05;
        }
        double z = 0, coda = 0;
        int schiuma = 0;
        for (const auto& p : fx.particelle()) {
            if (p.materia == Materia::spruzzo) z = std::max(z, p.z);
            if (p.materia == Materia::schiuma) { ++schiuma; coda = std::max(coda, pos.y - p.y); }
        }
        return std::tuple{z, coda, schiuma};
    };
    const auto [z3, c3, s3] = scia(3);
    const auto [z8, c8, s8] = scia(8);
    const auto [z15, c15, s15] = scia(15);
    CHECK(z3 < 0.4);              // a 3 nodi quasi niente
    CHECK(z8 > 0.5);              // a 8 nodi l'onda dell'albero si vede (circa v^2/2g = 0,9 m)
    CHECK(z8 < 1.5);
    CHECK(z15 > z8);
    CHECK(c8 > 3 * c3);           // la scia si allunga con la velocita'
    CHECK(c15 > c8);
    CHECK(c8 > 30.0);             // a 8 nodi alcune decine di metri: la vede anche un elicottero
    Effetti fermo(1);
    fermo.piuma(1, {0, 0}, 0, 0.0, 2.0, 1.0);
    CHECK(fermo.numero() == 0);
}

// Al periscopio un battello immerso non si vede; a quota periscopica solo gli alberi alzati (E2, docs/10 §10.15)
TEST_CASE("periscopio: sottomarini immersi, solo gli alberi alzati" * doctest::skip(!fs::exists(M688_DATI "/modelli/victor3.mdl"))) {
    const std::string d = M688_DATI;
    const auto classi = carica_classi(d + "/classi.toml");
    const auto font = Font::carica(d + "/font/originale.fnt");
    std::map<std::string, InsiemeSprite> sprite;
    std::map<std::string, Modello> modelli;
    for (const std::string c : {"los_angeles", "victor3"}) {
        sprite.emplace(c, InsiemeSprite::carica(d + "/" + classi.at(c).sprite));
        modelli.emplace(c, Modello::carica(d + "/modelli/" + c + ".mdl"));
    }
    Periscopio p(font, classi, sprite);
    p.usa_modelli(modelli, rampa_grigi(Palette::carica(d + "/palette/giorno.pal")));
    VistaPeriscopio v;
    v.alto_ingrandimento = true;
    // pixel diversi dalla vista senza il Victor (600 m a nord, di fianco)
    auto diversi = [&](std::optional<std::pair<double, bool>> victor) {
        Mondo m;
        Nave g; g.id = 0; g.classe = "los_angeles"; g.pos = g.pos_prec = {0, 0};
        m.aggiungi(g);
        Tela vuota, t;
        p.disegna(vuota, m, *m.trova(0), v);
        if (victor) {
            Nave n; n.id = 2; n.classe = "victor3"; n.pos = n.pos_prec = {0, 600}; n.rotta = n.rotta_prec = 90;
            n.quota = victor->first; n.alberi_alzati = victor->second;
            m.aggiungi(n);
        }
        p.disegna(t, m, *m.trova(0), v);
        int k = 0;
        for (int y = 0; y < t.altezza(); ++y)
            for (int x = 0; x < t.larghezza(); ++x) k += t.at(x, y) != vuota.at(x, y);
        return k;
    };
    const int emerso = diversi(std::pair{0.0, false});
    const int alberi = diversi(std::pair{6.0, true});
    const int a_meta = diversi(std::pair{1.5, false});
    CHECK(emerso > 2000);
    CHECK(alberi > 0);                 // periscopio e antenna sporgono
    CHECK(alberi < emerso / 50);       // solo loro
    CHECK(a_meta > alberi);            // a 1,5 m lo scafo e' ancora mezzo fuori
    CHECK(a_meta < emerso);
    CHECK(diversi(std::pair{6.0, false}) == 0);    // alberi giu'
    CHECK(diversi(std::pair{18.0, true}) > 0);     // quota periscopica dell'Enhanced
    CHECK(diversi(std::pair{100.0, true}) == 0);   // in profondita'
}

// Le navi intatte vicine nell'Enhanced (E9 §2.3): dal modello a passi di 1 grado invece degli sprite ogni 5; il classico
// (oculare spento) resta sugli sprite; col filo di lavoro il primo fotogramma tiene lo sprite precalcolato
TEST_CASE("periscopio: le navi intatte vicine dal modello a passi di 1 grado" * doctest::skip(!fs::exists(M688_DATI "/modelli/perry.mdl"))) {
    const std::string d = M688_DATI;
    const auto classi = carica_classi(d + "/classi.toml");
    const auto font = Font::carica(d + "/font/originale.fnt");
    std::map<std::string, InsiemeSprite> sprite;
    std::map<std::string, Modello> modelli;
    for (const std::string c : {"los_angeles", "perry"}) {
        sprite.emplace(c, InsiemeSprite::carica(d + "/" + classi.at(c).sprite));
        modelli.emplace(c, Modello::carica(d + "/modelli/" + c + ".mdl"));
    }
    const auto rampa = rampa_grigi(Palette::carica(d + "/palette/giorno.pal"));
    auto vista = [&](Periscopio& p, double aob, bool oculare) {
        Mondo m;
        Nave g; g.id = 0; g.classe = "los_angeles"; g.pos = g.pos_prec = {0, 0};
        m.aggiungi(g);
        Nave n; n.id = 2; n.classe = "perry"; n.pos = n.pos_prec = {0, 1500}; n.rotta = n.rotta_prec = 180.0 - aob;
        m.aggiungi(n);
        VistaPeriscopio v;
        v.alto_ingrandimento = true;
        if (oculare) {   // l'oculare del 6x (campo di 8 gradi)
            v.oculare.attivo = true;
            v.oculare.campo_gradi = 8.0;
            v.oculare.passo_gradi = 1.0;
            v.oculare.passo_fine_gradi = 0.25;
        }
        Tela t;
        p.disegna(t, m, *m.trova(0), v);
        return t;
    };
    auto diversi = [](const Tela& a, const Tela& b) {
        int k = 0;
        for (int y = 0; y < a.altezza(); ++y)
            for (int x = 0; x < a.larghezza(); ++x) k += a.at(x, y) != b.at(x, y);
        return k;
    };
    Periscopio p(font, classi, sprite);
    p.usa_modelli(modelli, rampa);
    // il classico: 36 e 37 gradi hanno lo stesso sprite (35)
    CHECK(diversi(vista(p, 36.0, false), vista(p, 37.0, false)) == 0);
    // l'Enhanced: un grado si vede, ed e' deterministico
    const Tela a36 = vista(p, 36.0, true), a37 = vista(p, 37.0, true);
    CHECK(diversi(a36, a37) > 20);
    CHECK(diversi(a37, vista(p, 37.0, true)) == 0);
    // col filo di lavoro: il primo fotogramma tiene lo sprite precalcolato, poi arriva quello dal modello
    Periscopio pf(font, classi, sprite);
    pf.usa_modelli(modelli, rampa);
    pf.usa_filo_di_lavoro(true);
    Periscopio ps(font, classi, sprite);
    ps.usa_modelli(modelli, rampa);
    const Tela primo = vista(pf, 37.0, true);
    CHECK(diversi(primo, a37) > 0);
    Tela poi;
    for (int k = 0; k < 400; ++k) {   // al piu' 20 s
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        poi = vista(pf, 37.0, true);
        if (diversi(poi, a37) == 0) break;
    }
    CHECK(diversi(poi, a37) == 0);
}

// Eventi di lancio dei missili per gli effetti (E2): chi, da dove, verso dove, con quale arma
TEST_CASE("missioni: lanci di missili registrati per gli effetti" * doctest::skip(!fs::exists(M688_MISSIONI "/SCENE1A.lua"))) {
    SUBCASE("Harpoon del giocatore dal tubo 7 di SCENE1A: arma dal gestore, rotta sul bersaglio") {
        Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1);
        int g = -1, bersaglio = -1;
        for (const auto& [id, u] : m.unita()) {
            if (u.giocatore) g = id;
            if (u.categoria == "nave" && bersaglio < 0) bersaglio = id;
        }
        REQUIRE(bersaglio >= 0);
        REQUIRE(m.tubi().size() >= 7);
        REQUIRE(m.tubi()[6] == "harpoon");
        CHECK(m.applica({0, TipoOrdine::designa_bersaglio, 0, 0, bersaglio}));
        CHECK(m.applica({0, TipoOrdine::lancia, 0, 0, 7}));
        REQUIRE(m.lanci_missili().size() == 1);
        const auto& l = m.lanci_missili().front();
        CHECK(l.unita == g);
        CHECK(l.arma == "harpoon");
        CHECK(l.categoria_lanciatore == "sottomarino");
        CHECK(l.bersaglio == bersaglio);
        CHECK(l.rotta == doctest::Approx(normalizza_360(rilevamento(m.unita().at(g).pos, m.unita().at(bersaglio).pos))));
        CHECK(l.quota == doctest::Approx(m.unita().at(g).quota));
    }
    SUBCASE("SCENE1B: il 688 alleato lancia dal suo script, senza arma") {
        Missione m(std::string(M688_MISSIONI) + "/SCENE1B.lua", 1);
        for (int t = 0; t < 3600 && m.lanci_missili().empty(); ++t) m.avanza(1.0);
        REQUIRE_FALSE(m.lanci_missili().empty());   // a 152 s con il seme 1
        const auto& l = m.lanci_missili().front();
        CHECK(l.unita == 7);
        CHECK(l.classe_lanciatore == "los_angeles");
        CHECK(l.arma.empty());
        CHECK(l.quota == doctest::Approx(6.096));
    }
}

TEST_CASE("missioni: carte d'epoca delle due parti accanto al fondale vero") {
    const auto f = missione_sub(scena_bersaglio(), "carte");
    Missione m(f, 1, false);
    CHECK(m.fondale_carta(Parte::nato) == nullptr);   // niente fondale, niente carta
    const auto vero = std::make_shared<Fondale>(4, 4, 100.0, 1.0, -150.0, std::vector<std::uint16_t>(16, 0));
    m.imposta_fondale(vero);
    CHECK(m.fondale_carta(Parte::nato) == vero.get());   // «fondale vero»: la carta e' il fondale
    const auto carta = std::make_shared<Fondale>(4, 4, 100.0, 1.0, -140.0, std::vector<std::uint16_t>(16, 0));
    m.imposta_fondale_carta(Parte::urss, carta);
    CHECK(m.fondale_carta(Parte::urss) == carta.get());
    CHECK(m.fondale_carta(Parte::nato) == vero.get());
    CHECK(m.fondale() == vero.get());                    // la simulazione resta sul fondale vero
    CHECK(m.profondita({0, 0}) == doctest::Approx(150.0));
    fs::remove(f);
}

TEST_CASE("spaccato: quadro danni con profilo, acqua, fuoco e squadre" * doctest::skip(!fs::exists(M688_DATI "/modelli/los_angeles.mdl"))) {
    const auto modello = Modello::carica(M688_DATI "/modelli/los_angeles.mdl");
    const auto scafo = DatiScafo::carica(M688_DATI "/scafi/los_angeles.toml");
    Spaccato sp("los_angeles", modello, scafo);
    auto pal = Palette::carica(M688_DATI "/palette/giorno.pal");
    Spaccato::imposta_palette(pal);
    CHECK(pal.rgb(116) == std::array<std::uint8_t, 3>{3, 26, 32});   // locali: verde acqua scuro
    REQUIRE(sp.compartimenti() == 3);
    double x0 = 0, x1 = 0;
    REQUIRE(sp.estensione(2, x0, x1));   // macchine
    CHECK(x0 == doctest::Approx(-39.85));
    const Rettangolo r{4, 20, 632, 108};
    CHECK(sp.x_tela(r, x0) < sp.x_tela(r, x1));
    auto conta = [](const Tela& t, Rettangolo q, int da, int a, int xa = 0, int xb = 1 << 20) {
        int n = 0;
        for (int y = q.y; y < q.y + q.h; ++y)
            for (int x = std::max(q.x, xa); x < std::min(q.x + q.w, xb); ++x) n += t.at(x, y) >= da && t.at(x, y) <= a;
        return n;
    };
    BattelloDanni b(scafo, 1);
    Tela intatto;
    sp.disegna(intatto, r, StatoSpaccato::da(b, 100, 0, 0));
    CHECK(conta(intatto, r, 116, 123) > 1500);   // interni
    CHECK(conta(intatto, r, 132, 135) > 5);      // elica in bronzo
    double p0 = 0, p1 = 0;
    REQUIRE(sp.estensione(0, p0, p1));   // prua
    const int a = sp.x_tela(r, x0), bb = sp.x_tela(r, x1), pa = sp.x_tela(r, p0), pb = sp.x_tela(r, p1);
    CHECK(conta(intatto, r, 199, 206, a + 2, bb - 2) == 0);   // nessuna acqua imbarcata (le casse invece sono piene)...
    CHECK(conta(intatto, r, 235, 238) == 0);     // ...e nessun sistema guasto
    // danni a mano: macchine allagate a meta', fuoco a prua con le squadre, motori guasti
    auto comp = b.compartimenti();
    comp[2].acqua = 0.5 * comp[2].volume;
    comp[0].fuoco = 0.6;
    comp[0].fumo = 0.5;
    std::vector<StatoSistema> sistemi(static_cast<std::size_t>(Sistema::numero));
    sistemi[static_cast<std::size_t>(Sistema::motori)].efficienza = 0.4;
    std::vector<SquadraBattello> squadre(1);
    squadre[0].compito = SquadraBattello::Compito::spegnere;
    squadre[0].compartimento = 0;
    squadre[0].numero = 2;
    StatoSpaccato s = StatoSpaccato::da(b, 100, 0, 0);
    s.compartimenti = &comp;
    s.sistemi = &sistemi;
    s.squadre = &squadre;
    Tela danni;
    sp.disegna(danni, r, s);
    CHECK(conta(danni, r, 199, 206, a, bb) > 200);                    // acqua nelle macchine
    CHECK(conta(danni, r, 199, 206, pa + 2, pb - 2) == 0);            // non a prua
    int fuoco = 0;
    for (const int c : colori::FUOCO) fuoco += conta(danni, r, c, c);
    CHECK(fuoco > 20);
    CHECK(conta(danni, r, 223, 230) > 50);                            // bagliore
    {   // tute ambra dell'antincendio e getto nebulizzato
        StatoSpaccato senza = s;
        senza.squadre = nullptr;
        Tela t2;
        sp.disegna(t2, r, senza);
        CHECK(conta(danni, r, 136 + 4, 136 + 4, pa, pb) > conta(t2, r, 136 + 4, 136 + 4, pa, pb));
    }
    CHECK(conta(danni, r, 235, 238) > 10);                            // contorno dei motori guasti
    // deterministico per lo stesso tempo
    Tela ancora;
    sp.disegna(ancora, r, s);
    bool uguali = true;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) uguali &= ancora.at(x, y) == danni.at(x, y);
    CHECK(uguali);
    {   // la cassa zavorra di prua squarciata: lo squarcio nella lamiera e, in esaurimento, le bolle che ne escono
        auto casse = b.casse();
        REQUIRE_FALSE(casse.empty());
        StatoSpaccato sc = s;
        sc.casse = &casse;
        sc.esaurimento = true;
        Tela integra, rotta;
        sp.disegna(integra, r, sc);
        casse.front().squarciata = true;
        sp.disegna(rotta, r, sc);
        int diversi = 0;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) diversi += integra.at(x, y) != rotta.at(x, y);
        CHECK(diversi > 20);
    }
    // dettaglio del compartimento di prua: riempie il rettangolo, con i locali piu' grandi
    const Rettangolo rd{4, 164, 632, 160};
    sp.disegna_dettaglio(danni, rd, s, -2.85, 41.15);
    CHECK(conta(danni, rd, 116, 123) + conta(danni, rd, 223, 230) > 8000);
}

TEST_CASE("artiglieria sul battello (G3): teste, alberi, falsatorre, plancia e scafo (dossier 22 §5.3 e §9.5)" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, "
                                "posizione = { x = 0, y = 0 }, quota_m = 18, rotta = 0, velocita_kn = 3, " + MODELLO + "160 } },",
                                "artiglieria");
    Missione m(f, 1, false);
    auto impatto = [](ParteEsposta parte, ImpattoArtiglieria::Esito esito, double calibro, double distanza = 0.0) {
        ImpattoArtiglieria i;
        i.tiratore = 9;
        i.bersaglio = 0;
        i.pezzo = "mk45";
        i.calibro_mm = calibro;
        i.carica_kg = 3.5;
        i.tempo_impatto_s = 10.0;
        i.punto = {0.0, 20.0};
        i.esito = esito;
        i.parte = parte;
        i.distanza_parte_m = distanza;
        return i;
    };
    using E = ImpattoArtiglieria::Esito;
    SUBCASE("avvertimenti e colpi in acqua non toccano niente") {
        DanniSottomarini ds(classi, M688_DATI, 1);
        auto a = impatto(ParteEsposta::periscopio_attacco, E::diretto, 127);
        a.avvertimento = true;
        ds.colpo_artiglieria(m, a);
        ds.colpo_artiglieria(m, impatto(ParteEsposta::nessuna, E::acqua, 127));
        ds.colpo_artiglieria(m, impatto(ParteEsposta::periscopio_attacco, E::rimbalzo, 127));
        CHECK(ds.artiglieria(0) == nullptr);
        CHECK(ds.sottomarino(0) == nullptr);
    }
    SUBCASE("colpo diretto sulla testa: cieca 50%, bloccata 30%, degradata 20% (+-10%); il periscopio nei sistemi") {
        int n[4] = {0, 0, 0, 0};
        for (std::uint64_t seme = 1; seme <= 120; ++seme) {
            DanniSottomarini ds(classi, M688_DATI, seme);
            ds.colpo_artiglieria(m, impatto(ParteEsposta::periscopio_attacco, E::diretto, 127));
            const auto* a = ds.artiglieria(0);
            REQUIRE(a != nullptr);
            ++n[static_cast<int>(a->attacco.guasto)];
            CHECK(a->attacco.tempo == 10.0);
            CHECK(a->attacco.incrinato > 0.0);
            CHECK(a->attacco.seme != 0u);
            CHECK(a->ricerca.guasto == GuastoTesta::intatta);
            CHECK(a->esm);   // la testa d'attacco non ha l'ESM
            CHECK(ds.sistemi(0)->at(static_cast<std::size_t>(Sistema::periscopio)).efficienza < 1.0);
            if (a->attacco.guasto == GuastoTesta::cieca) CHECK((a->attacco.allaga_canna && a->attacco.spento > 0.0));
        }
        CHECK(n[0] == 0);
        CHECK(n[3] / 120.0 == doctest::Approx(0.5).epsilon(0.2));
        CHECK(n[2] / 120.0 == doctest::Approx(0.3).epsilon(0.34));
        CHECK(n[1] / 120.0 == doctest::Approx(0.2).epsilon(0.5));
    }
    SUBCASE("il periscopio di ricerca porta via l'ESM; l'albero radio la radio; le schegge lontane spesso mancano") {
        DanniSottomarini ds(classi, M688_DATI, 3);
        ds.colpo_artiglieria(m, impatto(ParteEsposta::periscopio_ricerca, E::diretto, 76));
        ds.colpo_artiglieria(m, impatto(ParteEsposta::albero_radio, E::diretto, 76));
        const auto* a = ds.artiglieria(0);
        REQUIRE(a != nullptr);
        CHECK(a->ricerca.guasto != GuastoTesta::intatta);
        CHECK_FALSE(a->esm);
        CHECK_FALSE(a->radio);
        CHECK(ds.sistemi(0)->at(static_cast<std::size_t>(Sistema::antenna)).efficienza == 0.0);
        CHECK(a->colpi == 2);
        int colpite = 0;
        for (std::uint64_t seme = 1; seme <= 100; ++seme) {
            DanniSottomarini d2(classi, M688_DATI, seme);
            d2.colpo_artiglieria(m, impatto(ParteEsposta::periscopio_attacco, E::schegge, 127, 3.8));   // al bordo di r_eff
            colpite += d2.artiglieria(0)->attacco.guasto != GuastoTesta::intatta;
            CHECK(d2.artiglieria(0)->schegge == 1);
        }
        CHECK(colpite > 5);
        CHECK(colpite < 40);
    }
    SUBCASE("scafo: il 30 mm non buca, il 5\" di rado (5-10%), il 16\" sempre e il battello non si immerge piu'") {
        DanniSottomarini ds(classi, M688_DATI, 5);
        for (int k = 0; k < 50; ++k) ds.colpo_artiglieria(m, impatto(ParteEsposta::scafo, E::diretto, 30));
        CHECK(ds.sottomarino(0)->danni->falle().empty());
        int bucato = 0;
        for (std::uint64_t seme = 1; seme <= 200; ++seme) {
            DanniSottomarini d2(classi, M688_DATI, seme);
            d2.colpo_artiglieria(m, impatto(ParteEsposta::scafo, E::diretto, 127));
            bucato += !d2.sottomarino(0)->danni->falle().empty();
        }
        CHECK(bucato >= 4);    // ~15 attesi su 200
        CHECK(bucato <= 30);
        DanniSottomarini iowa(classi, M688_DATI, 7);
        iowa.colpo_artiglieria(m, impatto(ParteEsposta::scafo, E::diretto, 406));
        CHECK(iowa.artiglieria(0)->non_si_immerge);
        REQUIRE(!iowa.sottomarino(0)->danni->falle().empty());
        CHECK(iowa.sottomarino(0)->danni->falle().front().area >= 0.4);
    }
    SUBCASE("estremita': i colpi a prua e a poppa squarciano le casse zavorra (STIMA 0,4 col 76), al centro no") {
        auto impatto_a = [&](double x, double calibro) {
            auto i = impatto(ParteEsposta::scafo, E::diretto, calibro);
            i.punto = {0.0, x};   // rotta 0: x del kit = y del mondo
            return i;
        };
        int prua = 0, poppa = 0, centro = 0, eventi = 0;
        for (std::uint64_t seme = 1; seme <= 100; ++seme) {
            DanniSottomarini d(classi, M688_DATI, seme);
            d.colpo_artiglieria(m, impatto_a(52.0, 76));   // dentro la cassa di prua del 688 (110 m)
            d.colpo_artiglieria(m, impatto_a(-52.0, 76));  // dentro quella di poppa
            d.colpo_artiglieria(m, impatto_a(5.0, 76));    // sullo scafo resistente
            const auto& casse = d.sottomarino(0)->danni->casse();
            for (const auto& c : casse) {
                if (!c.squarciata) continue;
                (c.xc > 0.0 ? prua : poppa) += 1;
            }
            centro += d.artiglieria(0)->casse_squarciate - static_cast<int>(std::count_if(casse.begin(), casse.end(),
                                                                                         [](const CassaBattello& c) { return c.squarciata; }));
            for (const auto& e : d.rapporti(0)) eventi += e.tipo == EventoBattello::Tipo::cassa_squarciata;
        }
        CHECK(prua > 25);
        CHECK(prua < 55);
        CHECK(poppa > 25);
        CHECK(poppa < 55);
        CHECK(centro == 0);
        CHECK(eventi == prua + poppa);
        // il 16" sempre; il 30 mm di rado
        DanniSottomarini grosso(classi, M688_DATI, 3);
        grosso.colpo_artiglieria(m, impatto_a(52.0, 406));
        CHECK(grosso.artiglieria(0)->casse_squarciate == 1);
    }
    SUBCASE("plancia e falsatorre: feriti, timoni della falsatorre del 688") {
        DanniSottomarini ds(classi, M688_DATI, 11);
        ds.colpo_artiglieria(m, impatto(ParteEsposta::plancia, E::diretto, 30));
        const auto* a = ds.artiglieria(0);
        REQUIRE(a != nullptr);
        CHECK(a->feriti_plancia >= 1);
        CHECK(a->plancia_colpita == 10.0);
        bool timoni = false;
        for (int k = 0; k < 40 && !timoni; ++k) {
            ds.colpo_artiglieria(m, impatto(ParteEsposta::falsatorre, E::diretto, 127));
            timoni = ds.artiglieria(0)->timoni_falsatorre_bloccati;
        }
        CHECK(timoni);
        CHECK(ds.sistemi(0)->at(static_cast<std::size_t>(Sistema::idroplani)).efficienza < 1.0);
    }
    fs::remove(f);
}

TEST_CASE("Alfa, casse (dossier 19): sfoghi, compensazione, peso delle armi, compressione, Tan e guasto" * doctest::skip(!ci_sono_dati())) {
    BattelloDanni b(DatiScafo::carica(M688_DATI "/scafi/alfa.toml"), 1);
    const StatoMoto moto{100.0, 3.0};
    const auto avanza = [&](double s) { for (int i = 0; i < static_cast<int>(s * 10); ++i) b.avanza(0.1, moto); };
    const auto aria = [&] { double a = 0; for (const auto& c : b.casse()) a += c.aria_Nm3; return a; };
    // un soffio, poi gli sfoghi: l'aria esce in una ventina di secondi e l'esaurimento si ferma
    b.esaurisci();
    avanza(20.0);
    REQUIRE(aria() > 10.0);
    CHECK(b.forze(100.0).peso_netto_N < 0.0);
    b.sfoga();
    CHECK_FALSE(b.esaurimento());
    const double soffiata = aria();
    CHECK(b.sfogo());
    avanza(0.1);   // non di colpo
    CHECK(aria() < soffiata);
    CHECK(aria() > 0.0);
    avanza(15.0);
    CHECK(aria() == doctest::Approx(0.0));
    CHECK_FALSE(b.sfogo());
    CHECK(b.forze(100.0).peso_netto_N == doctest::Approx(0.0));
    // una SET-65 che esce dal tubo: piu' leggero e prua su
    double prua = -1e9;
    for (const auto& c : b.compartimenti()) prua = std::max(prua, c.x1);
    b.varia_peso(-1900.0, prua);
    CHECK(b.forze(100.0).peso_netto_N == doctest::Approx(-1900.0 * 9.81));
    CHECK(b.forze(100.0).momento_assetto_Nm > 0.0);
    // la compressione dello scafo: mezza tonnellata ogni 100 m sotto la quota dell'assetto (qui 100 m), meno sopra
    b.abilita_compressione(0.5, 100.0);
    CHECK(b.forze(100.0).peso_netto_N == doctest::Approx(-1900.0 * 9.81));
    CHECK(b.forze(500.0).peso_netto_N == doctest::Approx(100.0 * 9.81));
    CHECK(b.forze(0.0).peso_netto_N == doctest::Approx(-2400.0 * 9.81));
    // COMPENSA a mano, sotto il neutro: le pompe espellono la riserva
    b.compensa(-0.5);
    CHECK(b.assetto_obiettivo_m3() == doctest::Approx(-0.5));
    b.compensa(+2.0);
    avanza(60.0);
    CHECK(b.acqua_assetto_m3() == doctest::Approx(1.5));
    // Tan: l'obiettivo che rende neutro il battello, alle portate delle casse
    b.tan(true);
    avanza(60.0);
    CHECK(std::fabs(b.forze(100.0).peso_netto_N) < 50.0);
    CHECK_FALSE(b.guasto_casse());
    // le pompe sotto il 50%: Tan si disinserisce e la finestrella si accende
    b.guasto_sistema(Sistema::pompe, 0.6, "prova");
    avanza(0.2);
    CHECK_FALSE(b.tan());
    CHECK(b.guasto_casse());
}

TEST_CASE("Alfa, casse: gli ordini sfoga, compensa e tan arrivano al battello del giocatore" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto scena = [](const std::string& classe) {
        return missione_sub("{ id = 0, classe = '" + classe + "', categoria = 'sottomarino', flag = 9, giocatore = true, "
                            "posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "casse_" + classe);
    };
    const auto f = scena("alfa");
    Missione m(f, 1, false);
    DanniSottomarini ds(classi, M688_DATI, 1);
    gioca(m, ds, 1.0);
    REQUIRE(ds.sottomarino(0) != nullptr);   // l'Alfa del giocatore ha le casse dal primo passo
    CHECK(ds.consegna(m, {m.tempo(), TipoOrdine::sfoga}));
    CHECK(c_e(ds.messaggi(), 449));
    CHECK(ds.consegna(m, {m.tempo(), TipoOrdine::tan, 0.0, 0.0, 1}));
    CHECK(ds.tan(0));
    CHECK(c_e(ds.messaggi(), 443));
    CHECK_FALSE(ds.consegna(m, {m.tempo(), TipoOrdine::compensa, 0.5}));   // con Tan compensa lui
    CHECK(ds.consegna(m, {m.tempo(), TipoOrdine::tan, 0.0, 0.0, 0}));
    CHECK(c_e(ds.messaggi(), 444));
    CHECK(ds.consegna(m, {m.tempo(), TipoOrdine::compensa, 0.5}));
    CHECK(ds.sottomarino(0)->danni->assetto_obiettivo_m3() == doctest::Approx(0.5));
    CHECK_FALSE(ds.consegna(m, {m.tempo(), TipoOrdine::telegrafo, 1.0}));   // non sono ordini delle casse
    CHECK_FALSE(ds.guasto_casse(0));
    fs::remove(f);
    // il 688 non ha ne' Tan ne' ПОКС
    const auto g = scena("los_angeles");
    Missione m2(g, 1, false);
    DanniSottomarini ds2(classi, M688_DATI, 1);
    gioca(m2, ds2, 1.0);
    CHECK_FALSE(ds2.consegna(m2, {m2.tempo(), TipoOrdine::tan, 0.0, 0.0, 1}));
    CHECK(ds2.sottomarino(0) == nullptr);   // e nessun modello creato per questo
    fs::remove(g);
}

TEST_CASE("sottomarini: i danni iniziali del giocatore dalla missione (scheda I07)" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    const auto scena = [](const std::string& danni, const std::string& nome) {
        const auto f = fs::temp_directory_path() / ("m688_prova_sub_iniziali_" + nome + ".lua");
        std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, " << danni << " varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                            "  quota_m = 100, rotta = 0, velocita_kn = 5, " << MODELLO << "160 } },\n}, timer = {} } } }\n";
        return f.string();
    };
    const auto f = scena("danni_iniziali = { sistemi = { propulsione = 0.4, schiera = 0 },"
                         " acqua = { { compartimento = 'macchine', m3 = 10 } }, falle = { { compartimento = 'C3', area_m2 = 0.001 } },"
                         " incendi = { { compartimento = 'deposito', intensita = 0.3 } } },", "si");
    Missione m(f, 1, false);
    REQUIRE(m.errori().empty());
    CHECK_FALSE(m.danni_iniziali().vuoti());
    DanniSottomarini ds(classi, M688_DATI, 1);
    gioca(m, ds, 1.0);
    const auto* s = ds.sottomarino(0);
    REQUIRE(s != nullptr);
    const auto& b = *s->danni;
    CHECK(b.sistema(Sistema::propulsione).efficienza == doctest::Approx(0.4).epsilon(0.05));
    CHECK(b.sistema(Sistema::schiera).efficienza == doctest::Approx(0.0));
    CHECK(b.limiti().velocita < 1.0);
    CHECK_FALSE(b.limiti().schiera);
    const auto& comp = b.compartimenti();
    const auto c = [&](const char* uso) { return *std::find_if(comp.begin(), comp.end(), [&](const CompartimentoBattello& x) { return x.uso == uso; }); };
    CHECK(c("macchine").acqua >= 10.0);
    CHECK(b.falle().size() == 1);
    CHECK(c("deposito").fuoco > 0.0);
    CHECK_FALSE(ds.rapporti(0).empty());   // i guasti nel rapporto della stazione Stato
    fs::remove(f);
    // un sistema sconosciuto e' un errore della missione; senza la chiave nessun danno
    const auto g = scena("danni_iniziali = { sistemi = { motore_a_vapore = 0.5 } },", "errore");
    Missione m2(g, 1, false);
    CHECK_FALSE(m2.errori().empty());
    fs::remove(g);
    const auto h = scena("", "no");
    Missione m3(h, 1, false);
    CHECK(m3.danni_iniziali().vuoti());
    DanniSottomarini ds3(classi, M688_DATI, 1);
    gioca(m3, ds3, 1.0);
    CHECK(ds3.sottomarino(0) == nullptr);
    fs::remove(h);
}
