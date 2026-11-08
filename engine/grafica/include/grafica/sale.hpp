#pragma once
// Viste e pannelli delle stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md §6): il 688 Flight I e
// l'Alfa (Pr. 705) composti con gli strumenti di grafica/consolle, i colori di grafica/tema_stazioni e i font delle
// stazioni. Viste 640x360 (ALTEZZA_VISTA), pannelli nel rettangolo che riceve (di norma 640 x 90-120).
//
// I dati arrivano in struct semplici che la stazione riempie dai suoi stati (StatoManovra, StatoArmi...); nessuna
// dipendenza da app/. Le funzioni che sull'Alfa non esistono ancora nel motore (Boksit a programma, assetto Tan,
// pulsanti ПОКС, guasto delle casse, Akkord a tre bersagli) si disegnano dai campi coi valori predefiniti.
//
// Scritte: il 688 con le targhette in inglese d'epoca (MIL-M-18012), l'Alfa con lo stampino latino traslitterato. I
// messaggi e gli aiuti del gioco (in italiano) restano fuori dai pannelli: li scrive la stazione nella sua barra.
//
// Aree cliccabili: per ogni comando un rettangolo col nome del comando di nucleo/mappa_tasti ("telegrafo_avanti",
// "lancia", "tubo_1"...); comando vuoto = area solo informativa (evidenza al passaggio del mouse, didascalia).

#include <cstdint>
#include <string>
#include <vector>

#include "grafica/consolle.hpp"
#include "grafica/font.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

// I font del battello, caricati una volta (data/motore/font)
struct FontConsolle {
    Font targa, piccolo, schermo, schermo_grande, cifre, tamburo;
    static FontConsolle carica(const std::string& dati, BattelloTema b);   // std::runtime_error se manca un file
};

// I posti (pulti) delle due centrali: per l'evidenza e le didascalie
enum class Pulto : std::uint8_t {
    // 688
    governo, bcp, periscopio_attacco, periscopio_ricerca, tiro_mk81, wcp, sonar, esm, radio, carteggio, locale_siluri,
    // Alfa
    boksit, tan, ritm, molnija, akkord, sargan, okean, signal, tv1, navigatore, sozh, ubz,
};
struct AreaPulto {
    Pulto pulto = Pulto::governo;
    Rettangolo r;
    std::string comando;   // nome di nucleo/mappa_tasti; vuoto: area solo informativa
};

// --- Manovra (F4) ---
struct DatiGoverno {
    double rotta = 0, rotta_ordinata = 0;              // gradi
    double quota_m = 0, quota_ordinata_m = 0, quota_max_m = 450;
    double velocita_kn = 0, giri = 0;
    double barra_gradi = 0;                            // timone, a dritta positivo (-35..35)
    double piani_gradi = 0;                            // timoni orizzontali, a scendere positivo (-30..30)
    double assetto_gradi = 0, sbandamento_gradi = 0;
    int telegrafo = 1;                                 // come StatoManovra (classico, 7 posizioni)
    int campana = 3;                                   // campane dell'Enhanced 0-8 (PostoEnhanced: 3 = STOP)
    bool pilota = false, cavita = false, cambia_quota = false, periscopio = false, antenna = false;
    int punti_rotta = 0;
    // 688: albero di Natale (true = aperta), casse di compenso 0-1, soffiaggio d'emergenza
    std::vector<bool> aperture;
    double compenso_prua = 0.5, compenso_poppa = 0.5;
    bool soffiaggio = false, coperchio_soffiaggio = false;
    // Alfa: modi di Boksit, assetto automatico Tan, pulsanti ПОКС, guasto delle casse
    enum class Modo : std::uint8_t { manuale, automatico, programma };
    Modo modo_rotta = Modo::manuale, modo_quota = Modo::manuale;
    bool tan = false;
    bool soffia = false, sfoga = false, compensa = false, guasto_casse = false;
    int compensa_verso = 0;                            // compensa: +1 imbarca (URAV+), -1 espelle (URAV-)
    int tappa = 0, tappe = 0;                          // Boksit a programma: la tappa in corso (1..tappe) e quante
    bool radar = false;                                // il radar del battello acceso (688 BPS-15, Alfa la sua RLS)
    // Enhanced sul 688 (spie accanto alla quota): la quota periscopica (-1 nessuna spia, 0 spenta, 1 accesa) e la schiera
    // trainata (-1 nessuna spia, 0 a posto, 1 sul fondo, 2 danneggiata, 3 persa)
    int quota_periscopica = -1, schiera = -1;
};
void pannello_governo(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiGoverno& d,
                      std::vector<AreaPulto>* aree = nullptr);

// --- Armi (F7) ---
struct DatiArmi {
    struct Tubo {
        std::string arma;                   // la variante (id del carico, come StatoArmi); vuota: tubo vuoto
        bool pronto = false, allagato = false, porta = false, filo = false;
        double ricarica = -1.0;             // 0-1 in corso, < 0 nessuna
        SagomaInTubo sezione;               // la sezione del tubo della sala siluri (facoltativa)
    };
    std::vector<Tubo> tubi;
    int selezionato = 0;                    // indice in tubi
    std::vector<std::string> schermo;       // righe brevi per lo schermo Mk 81 o Akkord (impostazioni, filo)
    bool fuoco_coperchio = false;           // 688: il coperchio del FIRE alzato
    bool akkord_guasto = false;             // Alfa: blocca tutte le armi
    int bersagli_akkord = 0, bersaglio_scelto = -1;   // Alfa: fino a 3
    std::vector<std::string> slot_akkord;   // Alfa: per slot una riga breve (lettera del contatto e distanza, «B 4,2 NM»)
};
void pannello_armi(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiArmi& d,
                   std::vector<AreaPulto>* aree = nullptr);

// --- Stato (F3) dell'Alfa: il pulto di Ritm (il 688 tiene lo spaccato) ---
struct DatiStato {
    struct Sinottico {
        std::string nome;                   // «GAMMA» (reattore), «TEMBR» (rete), «TAKT-T» (impianti)
        std::vector<ColoreLampada> lampade;
        std::vector<bool> accese;
    };
    std::vector<Sinottico> sinottici;
    std::vector<std::string> allarmi;       // finestrelle accese
    double potenza_reattore = 0, temperatura = 0;   // 0-1, gradi
};
void pannello_stato_alfa(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiStato& d,
                         std::vector<AreaPulto>* aree = nullptr);

// --- Radio ed ESM (F2) ---
struct PulsanteStazione {
    std::string etichetta, comando;         // la scritta sul pulsante, il comando di nucleo/mappa_tasti
    bool acceso = false;
};
struct DatiRadioEsm {
    std::vector<std::string> righe_telescrivente;
    std::vector<float> panoramica;          // ESM: livello per frequenza 0-1 (la traccia dell'oscilloscopio)
    std::vector<std::string> intercettazioni;   // righe brevi accanto alla panoramica (rilevamento, banda, identita')
    bool minaccia = false;
    // i pulsanti della sala radio (filo, SSIXS, slot, trasmissioni...): se ci sono, a destra in una griglia di 3 colonne
    // (al piu' 9) e telescrivente ed ESM piu' stretti
    std::vector<PulsanteStazione> pulsanti;
};
void pannello_radio_esm(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiRadioEsm& d,
                        std::vector<AreaPulto>* aree = nullptr);

// --- Viste ---
// La centrale (F9): la pedana dei periscopi del 688 in prospettiva o il ferro di cavallo dell'Alfa, un'area per pulto
void vista_centrale(Tela& t, const TemaStazione& tema, const FontConsolle& f, double tempo_s,
                    std::vector<AreaPulto>* aree = nullptr);
// Lo schermo di Okean (Alfa): il ventaglio dei rilevamenti nel CRT tondo; livelli 0-1 su 360 gradi, il fascio puntato
void schermo_okean(Tela& t, int cx, int cy, int r, const std::vector<float>& livello_per_rilevamento, double fascio,
                   const FontConsolle& f);
// La cornice del periscopio: 688 Type 2 d'attacco (1) o Type 18 di ricerca (2) con le maniglie; Alfa Signal o la TV-1
void cornice_periscopio(Tela& t, const TemaStazione& tema, const FontConsolle& f, bool tv, int periscopio);
// La sala sonar del 688 con la luce blu attorno alle cascate (bassa priorita'); il rettangolo libero per le cascate
Rettangolo cornice_sala_sonar(Tela& t, const TemaStazione& tema, const FontConsolle& f);
// Il plottatore Sozh dell'Alfa: la cornice crema e il carrello col pennino alla posizione (bassa priorita')
void cornice_sozh(Tela& t, Rettangolo carta, const TemaStazione& tema, const FontConsolle& f, double pennino_x, double pennino_y);

}  // namespace m688
