#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;

// Ricarica realistica dell'Enhanced (dossier docs/ricerca/14_carico_armi.md §1.5 e §5.4)

namespace {

const std::vector<std::string> TUBI_688 = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};

StazioneArmi stazione(int mk48 = 6) { return StazioneArmi(TUBI_688, {{"mk48", mk48}, {"harpoon", 1}, {"esca", 6}}, 7, 4); }

// un tubo carico armato e lanciato (il messaggio 10 + tubo - 1 arriva dallo script dopo 20 s)
void arma_e_lancia(StazioneArmi& s, int tubo, double t) {
    REQUIRE(s.stato(tubo) == StatoTubo::carico);
    s.consegna({t, TipoOrdine::carica_tubo, 0, 0, tubo}, {});
    REQUIRE(s.stato(tubo) == StatoTubo::in_armamento);
    s.messaggio_missione(9 + tubo);
    REQUIRE(s.stato(tubo) == StatoTubo::armato);
    s.consegna({t, TipoOrdine::lancia, 0, 0, tubo}, {});
}

}  // namespace

TEST_CASE("ricarica: senza attivazione la stazione e' quella del livello classico") {
    StazioneArmi s = stazione();
    CHECK_FALSE(s.ricarica_realistica());
    const EsitoStazione e = s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 1}, {});
    CHECK(s.stato(1) == StatoTubo::in_armamento);   // dritto all'armamento, come nell'originale
    CHECK(e.alla_missione.size() == 1);
    CHECK(s.avanza(100.0).messaggi.empty());
    CHECK(s.consegna({0, TipoOrdine::silenzio_a_bordo, 0, 0, 1}, {}).messaggio == 0);
}

TEST_CASE("ricarica: tubi carichi alla partenza, armamento, disarmo e scorte") {
    StazioneArmi s = stazione();
    s.attiva_ricarica(parametri_ricarica("los_angeles"));
    for (int t = 1; t <= 4; ++t) CHECK(s.stato(t) == StatoTubo::carico);
    CHECK(s.stato(7) == StatoTubo::carico);    // l'Harpoon
    CHECK(s.stato(5) == StatoTubo::vuoto);     // le esche hanno i loro lanciatori
    CHECK(s.scorte(Arma::mk48) == 2);
    CHECK(s.scorte(Arma::harpoon) == 0);
    const auto tutte = s.scorte_tutte();
    REQUIRE(tutte.size() == 3);
    CHECK(tutte[0] == std::pair<Arma, int>{Arma::mk48, 2});
    // carico -> armamento (comando alla missione) -> disarmato: l'arma resta nel tubo
    EsitoStazione e = s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 2}, {});
    CHECK(s.stato(2) == StatoTubo::in_armamento);
    CHECK(e.alla_missione.size() == 1);
    e = s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 2}, {});
    CHECK(s.stato(2) == StatoTubo::carico);
    CHECK(e.messaggio == MSG_DISARMATO);
    CHECK(s.messaggio_missione(11) == 0);      // la fine dell'armamento dello script si sopprime
    CHECK(s.scorte(Arma::mk48) == 2);
    // le esche come nel classico: dritte all'armamento
    s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 5}, {});
    CHECK(s.stato(5) == StatoTubo::in_armamento);
}

TEST_CASE("ricarica: dopo il lancio, 300 s sul 688 con cinque colpi, poi l'armamento da solo") {
    StazioneArmi s = stazione();
    s.attiva_ricarica(parametri_ricarica("los_angeles"));
    s.avanza(0.0);
    arma_e_lancia(s, 1, 0.0);
    CHECK(s.stato(1) == StatoTubo::in_ricarica);   // ricarica automatica, senza annuncio
    CHECK(s.scorte(Arma::mk48) == 1);
    CHECK(s.secondi_ricarica(1) == doctest::Approx(300.0));
    int colpi = 0;
    for (double t = 1.0; t < 300.0; t += 1.0) {
        const EsitoRicarica r = s.avanza(t);
        colpi += r.transitori;
        CHECK(r.messaggi.empty());
    }
    CHECK(s.secondi_ricarica(1) == doctest::Approx(1.0));
    const EsitoRicarica fine = s.avanza(300.0);
    colpi += fine.transitori;
    CHECK(colpi == 5);                               // uno ogni 60 s di lavoro
    REQUIRE(fine.messaggi.size() == 1);
    CHECK(fine.messaggi[0] == MSG_RICARICATO);       // tubo 1 ricaricato
    CHECK(s.stato(1) == StatoTubo::in_armamento);    // si arma da solo: 20 s dello script
    REQUIRE(fine.alla_missione.size() == 1);
    CHECK(fine.alla_missione[0].tipo == TipoOrdine::carica_tubo);
    CHECK(fine.alla_missione[0].n == 1);
}

TEST_CASE("ricarica: due alla volta, la terza in coda; annullamento ed esaurimento") {
    StazioneArmi s = stazione(3);   // tre Mk 48 in tutto: alla partenza vanno nei tubi 1-3
    s.attiva_ricarica(parametri_ricarica("los_angeles"));
    CHECK(s.stato(4) == StatoTubo::vuoto);   // solo tre Mk 48: il quarto tubo resta vuoto
    s.avanza(0.0);
    // lanciato il tubo 1, la ricarica automatica non parte (scorte a zero) e il tubo 4 non si ricarica
    arma_e_lancia(s, 1, 0.0);
    CHECK(s.stato(1) == StatoTubo::vuoto);           // niente scorte: la ricarica automatica non parte
    CHECK(s.consegna({0, TipoOrdine::carica_tubo, 0, 0, 4}, {}).messaggio == MSG_ARMA_ESAURITA);
    // con tre Mk 48 in rastrelliera: tre ricariche chieste a mano
    StazioneArmi q = StazioneArmi(TUBI_688, {{"mk48", 3}}, 7, 4);
    ParametriRicarica p = parametri_ricarica("los_angeles");
    p.tubi_carichi_alla_partenza = false;
    q.attiva_ricarica(p);
    q.avanza(0.0);
    CHECK(q.consegna({0, TipoOrdine::carica_tubo, 0, 0, 1}, {}).messaggio == MSG_RICARICA_INIZIATA);
    CHECK(q.consegna({0, TipoOrdine::carica_tubo, 0, 0, 2}, {}).messaggio == MSG_RICARICA_INIZIATA);
    CHECK(q.consegna({0, TipoOrdine::carica_tubo, 0, 0, 3}, {}).messaggio == MSG_RICARICA_IN_CODA);
    CHECK(q.ricariche_in_corso() == 2);
    CHECK(q.in_coda(3));
    q.avanza(150.0);
    CHECK(q.secondi_ricarica(3) == doctest::Approx(300.0));   // in coda non avanza
    q.avanza(300.0);
    CHECK(q.stato(1) == StatoTubo::in_armamento);
    CHECK_FALSE(q.in_coda(3));                                  // un posto libero: parte
    q.avanza(450.0);
    CHECK(q.secondi_ricarica(3) == doctest::Approx(150.0));
    // annullata: l'arma torna in rastrelliera
    CHECK(q.consegna({0, TipoOrdine::carica_tubo, 0, 0, 3}, {}).messaggio == MSG_RICARICA_ANNULLATA);
    CHECK(q.stato(3) == StatoTubo::vuoto);
    CHECK(q.scorte(Arma::mk48) == 1);
}

TEST_CASE("ricarica: in silenzio dura il doppio e non fa transitori") {
    StazioneArmi s = stazione();
    s.attiva_ricarica(parametri_ricarica("los_angeles"));
    s.avanza(0.0);
    arma_e_lancia(s, 1, 0.0);
    CHECK(s.consegna({0, TipoOrdine::silenzio_a_bordo, 0, 0, 1}, {}).messaggio == MSG_SILENZIO);
    CHECK(s.silenzio());
    CHECK(s.secondi_ricarica(1) == doctest::Approx(600.0));
    int colpi = 0;
    for (double t = 10.0; t <= 590.0; t += 10.0) colpi += s.avanza(t).transitori;
    CHECK(colpi == 0);
    CHECK(s.stato(1) == StatoTubo::in_ricarica);
    CHECK(s.avanza(600.0).messaggi.size() == 1);
    CHECK(s.consegna({0, TipoOrdine::silenzio_a_bordo, 0, 0, 0}, {}).messaggio == MSG_FINE_SILENZIO);
    CHECK(s.consegna({0, TipoOrdine::silenzio_a_bordo, 0, 0, 0}, {}).messaggio == 0);   // gia' finito
}

TEST_CASE("ricarica: tempi per classe (STIMA del dossier 14)") {
    CHECK(parametri_ricarica("los_angeles").secondi == 300.0);
    CHECK(parametri_ricarica("alfa").secondi == 75.0);
    CHECK(parametri_ricarica("sturgeon").secondi == 420.0);
    CHECK(parametri_ricarica("trafalgar").secondi == 240.0);
    CHECK(parametri_ricarica("victor3").secondi == 240.0);
    CHECK(parametri_ricarica("ignota").secondi == 300.0);
    for (const char* c : {"los_angeles", "alfa", "sturgeon"}) CHECK(parametri_ricarica(c).contemporanee == 2);
    CHECK(std::string(nome_ordine(TipoOrdine::silenzio_a_bordo)) == "silenzio_a_bordo");
    CHECK(ordine_da_nome("silenzio_a_bordo") == TipoOrdine::silenzio_a_bordo);
}

namespace {
bool c_e(const char* scena) { return std::filesystem::exists(std::string(M688_MISSIONI) + "/" + scena); }

struct Partita {
    Missione m;
    StazioniMissione st;
    RegistroOrdini ordini;
    Partita(const char* scena, bool enhanced) : m(std::string(M688_MISSIONI) + "/" + scena, 1), st(m) {
        if (enhanced) m.siluri_enhanced(M688_DATI);
        avanza(1.0 / 9.0);
    }
    void avanza(double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    }
    void clic(int tubo) {
        if (st.armi().selezionato() != tubo)
            for (const auto& o : st.armi().clic_tubo(tubo, m.tempo()).alla_missione) ordini.ordina(o);
        for (const auto& o : st.armi().clic_tubo(tubo, m.tempo()).alla_missione) ordini.ordina(o);
    }
    bool messaggio(int codice) const {
        for (const auto& x : st.messaggi()) if (x.codice == codice) return true;
        return false;
    }
};
}  // namespace

TEST_CASE("ricarica: nella missione (688, Enhanced): carichi, armati in 20 s, lancio e ricarica di 300 s" * doctest::skip(!c_e("SCENE1A.lua"))) {
    Partita p("SCENE1A.lua", true);
    REQUIRE(p.st.armi().ricarica_realistica());
    CHECK(p.st.armi().stato(1) == StatoTubo::carico);
    CHECK(p.st.armi().scorte(Arma::mk48) == 18);   // 22 della dotazione, 4 nei tubi
    p.clic(1);
    p.avanza(21.0);
    REQUIRE(p.st.armi().stato(1) == StatoTubo::armato);
    for (const auto& [id, u] : p.m.unita())   // nessuno a tiro
        if (id != p.m.giocatore()->id) p.m.trova(id)->pos.x += 400000.0;
    p.ordini.ordina(p.st.armi().lancio(p.m.tempo()));
    p.avanza(1.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::in_ricarica);
    CHECK(p.st.armi().secondi_ricarica(1) == doctest::Approx(299.0).epsilon(0.01));
    p.avanza(299.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::in_armamento);
    CHECK(p.messaggio(MSG_RICARICATO));
    CHECK_FALSE(p.messaggio(MSG_RICARICA_INIZIATA));   // la ricarica automatica non si annuncia
    p.avanza(21.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::armato);
    CHECK(p.m.errori().empty());
}

TEST_CASE("ricarica: nella missione (Alfa, Enhanced): 75 s col caricatore automatico" * doctest::skip(!c_e("SCENE5B.lua"))) {
    Partita p("SCENE5B.lua", true);
    REQUIRE(p.m.giocatore()->classe == "alfa");
    CHECK(p.st.armi().parametri_ricarica().secondi == 75.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::carico);
    p.ordini.ordina({p.m.tempo(), TipoOrdine::silenzio_a_bordo, 0, 0, 1});
    p.avanza(1.0);
    CHECK(p.messaggio(MSG_SILENZIO));
    CHECK(p.st.armi().silenzio());
}

TEST_CASE("ricarica: nel livello classico la missione non cambia" * doctest::skip(!c_e("SCENE1A.lua"))) {
    Partita p("SCENE1A.lua", false);
    CHECK_FALSE(p.st.armi().ricarica_realistica());
    CHECK(p.st.armi().stato(1) == StatoTubo::vuoto);   // come nell'originale: tubi vuoti all'avvio
    CHECK(p.st.armi().scorte(Arma::mk48) == 22);
    p.clic(1);
    p.avanza(21.0);
    CHECK(p.st.armi().stato(1) == StatoTubo::armato);
}
