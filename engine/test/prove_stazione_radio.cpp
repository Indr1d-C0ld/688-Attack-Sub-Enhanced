#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/radio.hpp"
#include "stazione_radio.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Stazione Radio (app/stazione_radio.hpp; docs/11_comandi_e_stazioni.md §11, S4): archivio, ordini di missione,
// a capo del testo, ordini radio_* tradotti da StazioniMissione.

namespace {

ParametriRadio senza_fondo() {
    ParametriRadio p;
    p.fondo_ogni_s = 1e12;
    return p;
}

MessaggioRadio messaggio(Precedenza p, const char* oggetto) {
    MessaggioRadio m;
    m.precedenza = p;
    m.oggetto = oggetto;
    m.testo = {"Nuova zona di pattugliamento.", "NEW PATROL AREA."};
    m.caratteri = 300;
    return m;
}

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/acustica.toml"); }

}  // namespace

TEST_CASE("stazione radio: archivio dal piu' recente, ordini di missione a parte, scelta ritagliata") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, senza_fondo());
    MessaggioRadio ordini = messaggio(Precedenza::immediate, "ORDINI");
    r.ordini_di_missione(ordini);
    r.programma(messaggio(Precedenza::routine, "WX"), 0.0);
    r.programma(messaggio(Precedenza::priority, "INTEL"), 0.0);
    StatoAntenne su;
    su.quota_m = 10.0;
    su.velocita_kn = 5.0;
    su.albero = true;
    for (double t = 0.0; t < 600.0; t += 1.0) r.avanza(t, 1.0, su);
    StazioneRadio s;
    const auto tutti = s.archivio(r);
    REQUIRE(tutti.size() == r.ricevuti().size());
    REQUIRE(tutti.size() >= 3);
    CHECK(tutti.front() == &r.ricevuti().back());   // il piu' recente per primo
    s.commuta_ordini();
    const auto ordini_soli = s.archivio(r);
    REQUIRE_FALSE(ordini_soli.empty());
    CHECK(ordini_soli.back()->messaggio.serie == 0);
    // PREC. oltre la fine si ferma all'ultimo quando si disegna; SUCC. non va sotto lo zero
    s.successivo();
    CHECK(s.scelto() == 0);
    for (int i = 0; i < 20; ++i) s.precedente();
    CHECK(s.scelto() == 20);
}

TEST_CASE("stazione radio: il tipo di trasmissione gira sui sei, previsione meteo compresa") {
    StazioneRadio s;
    CHECK(s.tipo() == TipoTrasmissione::contatto);
    for (int i = 0; i < 4; ++i) s.tipo_successivo();
    CHECK(s.tipo() == TipoTrasmissione::richiesta);
    s.tipo_successivo();
    CHECK(s.tipo() == TipoTrasmissione::previsione);
    s.tipo_successivo();
    CHECK(s.tipo() == TipoTrasmissione::contatto);
}

TEST_CASE("stazione radio: a capo sulle parole e sui ritorni" * doctest::skip(!fs::exists(std::string(M688_DATI) + "/font/originale.fnt"))) {
    const Font f = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const std::string testo = "UNO DUE TRE QUATTRO CINQUE SEI SETTE OTTO NOVE DIECI\nUNDICI";
    const auto righe = StazioneRadio::a_capo(f, testo, f.larghezza("UNO DUE TRE QUATTRO", 1));
    REQUIRE(righe.size() >= 4);
    for (const auto& r : righe) CHECK(f.larghezza(r, 1) <= f.larghezza("UNO DUE TRE QUATTRO", 1));
    CHECK(righe.back() == "UNDICI");
    CHECK(righe.front() == "UNO DUE TRE QUATTRO");
}

TEST_CASE("stazione radio: trasmettere senza antenna e' rifiutato con il motivo; il filo si cala" * doctest::skip(!ci_sono_dati())) {
    const auto file = fs::temp_directory_path() / "m688_prova_stazione_radio.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(file) << "return { id = 'PROVA', teatro = 'faroe', script = {}, varianti = { { probabilita = 1, unita = {\n"
                           "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                           "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " << modello << " },"
                           "\n}, timer = {} } } }\n";
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(file.string(), 1, false);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    StazioniMissione st(m);
    for (int i = 0; i < 9; ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); }
    REQUIRE(m.radio() != nullptr);
    CHECK_FALSE(st.consegna(m, {m.tempo(), TipoOrdine::radio_trasmetti, 0, 0, 0}));
    REQUIRE_FALSE(st.messaggi().empty());
    CHECK(st.messaggi().back().codice == MSG_SERVE_ALBERO);
    CHECK_FALSE(st.consegna(m, {m.tempo(), TipoOrdine::radio_ssixs}));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::radio_filo, 0, 0, 1}));
    CHECK((m.radio()->filo_in_filata() || m.radio()->filo()));
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::radio_filo, 0, 0, 0}));
    // boa SLOT: la missione di prova non ne ha, il rifiuto col motivo
    CHECK(m.boe_slot() == 0);
    CHECK_FALSE(st.consegna(m, {m.tempo(), TipoOrdine::radio_slot, 600.0}));
    CHECK(st.messaggi().back().codice == MSG_NESSUNA_BOA);
    // verifica dell'autenticazione degli ordini di missione (serie 0 non si verifica; una serie qualsiasi passa alla sala)
    CHECK(st.consegna(m, {m.tempo(), TipoOrdine::radio_verifica, 0, 0, 1}));
    CHECK_FALSE(m.radio()->filo());
    fs::remove(file);
}
