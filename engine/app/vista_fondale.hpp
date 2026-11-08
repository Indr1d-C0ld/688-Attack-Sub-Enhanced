#pragma once
// Le viste del quadro del fondale (docs/24 §4; docs/08 §4; il modello e' nucleo/quadro_fondale del disassemblatore).
// Il battello vede del fondo solo la sua carta e le sue misure, e le viste disegnano solo quelle, mai il fondale vero:
// - sulla carta (Navigazione): le celle misurate col colore della profondita' misurata nella rampa delle isobate,
//   retinate verso la carta secondo la confidenza (dove la misura concorda con la carta non si vede differenza, dove
//   discorda si'); le zone non misurate puntinate a matita («da carta»); il nastro dei sondaggi; gli ostacoli;
// - la striscia dell'ecoscandaglio (BQN-17): la profondita' sotto la chiglia nel tempo come sulla carta termica, coi
//   falsi fondi come li vede l'operatore (non li riconosce), il programma e la potenza;
// - la sezione di prua: la carta a tratti, la stima con la sua fascia di incertezza, i ritorni del BQS-15, la quota
//   propria e il margine;
// - la vista dei soli ritorni in prospettiva, erede del display dell'originale: la griglia fusa davanti al battello,
//   piena dove misurata e a tratti «da carta», coi ritorni del BQS-15.
// Indici: la carta (96-143, 199-231) sulla carta; le viste a fosforo usano il tema delle stazioni (16-47) e 0-15.

#include "carta.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "nucleo/quadro_fondale.hpp"
#include "nucleo/volta_ghiaccio.hpp"

namespace m688 {

// Sopra la carta gia' disegnata in `v` (il ritaglio a v)
void disegna_misure_carta(Tela& t, const VistaCarta& v, const QuadroFondale& q, double tempo);

// Lo stato dell'ecoscandaglio per la striscia (dal runtime: il programma e la potenza)
struct StatoEcoscandaglio {
    double periodo_min = 0.0;        // 0: niente periodico
    bool potenza_piena = true;
    std::string nome = "BQN-17";     // l'ecoscandaglio del battello (l'Alfa: NEL-5, kit 31/10/2026)
};
// La striscia della carta termica in r: gli ultimi `finestra_s` secondi, il presente a destra
void disegna_ecoscandaglio(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, double tempo, const StatoEcoscandaglio& s,
                           double finestra_s = 1800.0);

// La sezione verticale di prua in r per `lunghezza_m` davanti al battello (posizione stimata)
void disegna_sezione_prua(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, Vec2 pos, double rotta, double quota_m,
                          double margine_m, double tempo, double lunghezza_m = 10000.0);

// La vista dei soli ritorni in prospettiva in r: la griglia fusa davanti al battello, vista da dietro e da sopra
void disegna_ritorni_prospettiva(Tela& t, const Font& f, Rettangolo r, const QuadroFondale& q, Vec2 pos, double rotta,
                                 double quota_m, double tempo, double lunghezza_m = 8000.0);

// --- La volta del ghiaccio (bozza del 29/10/2026, docs/ricerca/25_bozze/indice.md §10; QuadroVolta del disassemblatore) ---
// Lo stato del battello per le viste della volta: la posizione stimata, la chiglia vera e la cima della vela
struct BattelloVolta {
    Vec2 pos;
    double rotta = 0.0, velocita_kn = 0.0;
    double chiglia_m = 0.0, cima_vela_m = 0.0;
    double margine_m = 10.0;          // la chiglia che arriva alla cima della vela meno il margine e' un ostacolo
};

// La pagina del fondale sotto il ghiaccio, al posto dei tre schermi di oggi (Ctrl+H), nello stile dei ripetitori:
// - in alto a sinistra la volta a prua in pianta: il settore del BQS-15, le celle del quadro (piu' piene piu' spesse,
//   rosse le chiglie sotto la vela), le polynye candidate in ciano con l'eta', gli avvisi;
// - in alto a destra la carta termica: il sondatore alto sopra, il BQN-17 sotto, la quota del battello;
// - sotto la sezione da 2 km a poppa a 8 km a prua, in due fasce: il ghiaccio ingrandito sopra, il fondo sotto.
// `f` e' il font degli schermi (quello dei ripetitori); `fondale` puo' mancare
void disegna_pagina_volta(Tela& t, const Font& f, const QuadroVolta& volta, const QuadroFondale* fondale, const BattelloVolta& b,
                          double tempo);

// La striscia della volta sulla carta, sempre sotto la banchisa (scelta della principale del 29/10/2026): le celle
// misurate (bianco sottile, grigio spesso, nero le chiglie, ciano l'acqua libera), le chiglie sotto la vela con la croce
// rossa e il pescaggio, le polynye candidate col cerchio e l'eta', la legenda in alto a sinistra
void disegna_volta_carta(Tela& t, const VistaCarta& v, const Font& f, const QuadroVolta& volta, const BattelloVolta& b, double tempo);

// Gli echi falsi delle chiglie dell'ultimo ping attivo (Missione::echi_falsi_attivo) sulla carta: un punto che svanisce
// in `durata_s`, dalla posizione stimata col rilevamento e la distanza misurati; niente lettera e niente traccia
struct EcoCarta { double t = 0.0, rilevamento = 0.0, distanza_m = 0.0, eccesso_db = 0.0; };
void disegna_echi_carta(Tela& t, const VistaCarta& v, const std::vector<EcoCarta>& echi, Vec2 pos, double tempo, double durata_s = 10.0);

}  // namespace m688
