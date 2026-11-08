#pragma once
// Posto di comando del giocatore (docs/11_comandi_e_stazioni.md §3.1 e §5, fase C2): le posizioni dei comandi di
// manovra dell'originale e la traduzione delle azioni (tasti, pannelli) in ordini (nucleo/ordini.hpp), con i
// rifiuti dell'equipaggio quando il manuale del 1989 vieta l'ordine.
//
// Posizioni del livello classico (docs/originale/sensori_e_navigazione.md §7):
// - timone: 11 scatti da -5 (tutta a sinistra) a +5, rateo di virata = scatto x 0,72 gradi/s (+-3,6);
// - barra di immersione (timoni orizzontali): 9 scatti da -4 (su) a +4 (giu'), rateo = |scatto| / 4 del modello,
//   verso la superficie o verso la quota massima; a zero rateo 0, il battello tiene la quota (codice 17h);
// - telegrafo: 7 posizioni, INDIETRO (arresto rapido), STOP, 1/3, 2/3, STD, TUTTA, MAX = 0, 0, 1/4, 1/2, 3/4, 1,
//   1,09 della potenza.
// Rifiuti dal manuale: STD, TUTTA e MAX non sopra i 100 ft (r. 2140); periscopio e antenna solo a quota
// periscopica, 20 ft (r. 1941).

#include <cstdint>
#include <functional>
#include <optional>
#include <string_view>

#include "nucleo/ordini.hpp"

namespace m688 {

enum class Azione : std::uint8_t {
    timone_sinistra, timone_dritta, timone_centro,
    barra_su, barra_giu, barra_zero,
    telegrafo_avanti, telegrafo_indietro,
    quota_periscopica, quota_150_piedi, emersione_emergenza,
    pilota_automatico,
    periscopio, antenna,
    impulso_sonar,
    posati,                 // Enhanced: posarsi sul fondo (docs/11 §6)
    immersione_emergenza,   // Enhanced: avanti tutta alla quota di sicurezza (dossier 16, P4)
};

// Messaggi dell'equipaggio quando rifiuta un ordine (data/motore/testi/messaggi_bordo.tsv)
inline constexpr int MSG_TROPPO_IN_ALTO = 210;      // velocita' alta sopra i 100 ft
inline constexpr int MSG_NESSUN_PUNTO = 211;        // pilota automatico senza punti di rotta
inline constexpr int MSG_SOLO_PERISCOPICA = 212;    // periscopio e antenna solo a quota periscopica

struct StatoProprio {
    double tempo = 0.0;     // s di simulazione
    double quota = 0.0;     // m
    bool periscopio = false, antenna = false;
};

struct Risposta {
    std::optional<Ordine> ordine;   // l'ordine da accodare, se l'equipaggio lo esegue
    int rifiuto = 0;                // altrimenti il messaggio con il motivo (0: nessuna azione, nessun messaggio)
};

class PostoDiComando {
public:
    static constexpr int TIMONE_MAX = 5, BARRA_MAX = 4, TELEGRAFO_POSIZIONI = 7;
    static constexpr int TELEGRAFO_INDIETRO = 0, TELEGRAFO_STOP = 1, TELEGRAFO_STD = 4;
    static constexpr double QUOTA_100_PIEDI = 30.48, QUOTA_PERISCOPICA = 6.096, QUOTA_MASSIMA_ORDINE = 624.2;

    Risposta azione(Azione a, const StatoProprio& s);

    int timone() const { return timone_; }
    int barra() const { return barra_; }
    int telegrafo() const { return telegrafo_; }
    bool pilota_automatico() const { return pilota_; }
    void imposta_telegrafo(int posizione);                        // stato iniziale della missione
    // Aggiorna le posizioni da un ordine consegnato (anche da un replay), cosi' il pannello mostra sempre lo stato vero
    void osserva(const Ordine& o);
    void pilota_disinserito() { pilota_ = false; }                // la missione l'ha rifiutato o terminato

    static double rateo_timone(int scatto) { return scatto * 0.72; }
    static double potenza(int posizione);                         // frazione del telegrafo
    static std::string_view nome_telegrafo(int posizione);       // INDIETRO, STOP, 1/3, 2/3, STD, TUTTA, MAX
    static int posizione_da_velocita(double frazione);            // la posizione del telegrafo piu' vicina

private:
    int timone_ = 0, barra_ = 0, telegrafo_ = TELEGRAFO_STOP;
    bool pilota_ = false;
};

// Posto di comando dell'Enhanced (docs/11 §3.1 e §10, fase C6c): gli stessi tasti del classico danno gli ordini del
// modello dinamico (C6a). Timone in gradi a scatti di 5 fino a 35; telegrafo con le campane reali (indietro tutta,
// 2/3, 1/3, stop, avanti 1/3, 2/3, standard, tutta, massima) tradotte in giri dalla classe; barra di immersione come
// quota ordinata a passi di 15 m con angolo massimo di 15 gradi (dottrina, dossier 01). La quota periscopica e' nella
// scala della missione, come quella del classico: periscopio e antenna escono fino a 6,096 m (20 ft), e la chiglia
// della classe la ricava quota_classe (missione_periscopi.cpp); 5,5 m lasciano margine alla dinamica, come il
// comandante dell'IA (28/10/2026: prima erano 18 m e il periscopio non usciva).
class PostoEnhanced {
public:
    static constexpr int BARRA_PASSO = 5, BARRA_MAX = 35, CAMPANE = 9, STOP = 3;
    static constexpr double QUOTA_PASSO_M = 15.0, ANGOLO_MAX = 15.0, QUOTA_PERISCOPICA_M = 5.5, QUOTA_150_PIEDI_M = 45.72;

    // `giri_per_nodi`: i giri di regime per una velocita' (la dinamica della classe); `nodi_massimi`: la massima
    PostoEnhanced(std::function<double(double)> giri_per_nodi, double nodi_massimi);

    // Ordine dell'Enhanced (barra, giri, quota_con_angolo, ...) o nessuno; i rifiuti per i danni li fa la missione
    std::optional<Ordine> azione(Azione a, const StatoProprio& s);
    void imposta(int campana, int barra_gradi, double quota_m) { campana_ = campana; barra_ = barra_gradi; quota_ = quota_m; }
    // Aggiorna le posizioni da un ordine consegnato (anche da un replay): la campana piu' vicina ai giri, ecc.
    void osserva(const Ordine& o);

    int campana() const { return campana_; }
    int barra() const { return barra_; }
    double quota_ordinata() const { return quota_; }
    static std::string_view nome_campana(int c);   // IND. TUTTA ... MASSIMA
    double nodi_campana(int c) const;             // velocita' di regime della campana (negativa indietro)

private:
    std::function<double(double)> giri_per_nodi_;
    double nodi_max_;
    int campana_ = STOP, barra_ = 0;
    double quota_ = 0.0;
};

}  // namespace m688
