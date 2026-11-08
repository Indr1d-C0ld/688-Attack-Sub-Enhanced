#pragma once
// Carta del giocatore nel livello classico (docs/11_comandi_e_stazioni.md §4, fase C4): scala, zoom e proiezione
// della stazione Navigazione dell'originale (docs/originale/numeri_eseguibile.md §3.2, `4C8C`, tasti comuni `1234`).
// E' solo stato dell'interfaccia: non cambia la simulazione, quindi non passa per gli ordini.

#include <vector>

#include "nucleo/geometria.hpp"
#include "nucleo/strumenti_carta.hpp"

namespace m688 {

// Stile della carta nell'Enhanced (N1, docs/ricerca/12 §5.6): batimetrica «VGA 1989» (piu' scuro = piu' profondo),
// «carta nautica» (piu' chiaro = piu' profondo, come le carte vere) o foglio di plottaggio uniforme
enum class StileCarta : unsigned char { vga, nautica, foglio };

inline constexpr int MSG_MASSIMO_INGRANDIMENTO = 252, MSG_MINIMO_INGRANDIMENTO = 253;

class CartaGiocatore {
public:
    // Scala S in unita' di gioco per pixel della carta originale (320x200): da 2 a 52, all'avvio 6 (20 x 11 nm)
    int scala() const { return scala_; }
    int fattore() const { return scala_ / 2; }     // «ZOOM FACTOR n»
    int ingrandisci();                              // I: messaggio di rifiuto o 0
    int riduci();                                   // O
    // TIME PROJECTION: da 1 a 99 minuti, all'avvio 10, in giro
    int minuti() const { return minuti_; }
    void minuti_piu() { minuti_ = minuti_ % 99 + 1; }
    void minuti_meno() { minuti_ = minuti_ == 1 ? 99 : minuti_ - 1; }
    bool proiezione = false;                        // PLOT PROJ (CAh, DS:15AE)
    bool tutte_le_scie = false;                     // ALL TRACKS (C8h, DS:03CA)
    bool scia_bersaglio = false;                    // TARGET TRACK (C9h, DS:03CE): con entrambe accese vince questa
    bool sfondo_uniforme = false;                   // B: riempimento di colore 11 senza fondale
    // Enhanced: la grafica VGA/SVGA (fondale a fasce con ombre, isobate, terra, rosa); B cambia lo stile
    bool restyle = false;
    StileCarta stile = StileCarta::vga;
    void stile_successivo() { stile = static_cast<StileCarta>((static_cast<int>(stile) + 1) % 3); }
    // Strumenti dell'Enhanced (N2): modo 1989 (letture arrotondate, linee segnate dal plotter ogni 2 minuti) o moderno
    bool anni_89 = false;
    bool urss = false;                              // la carta della parte del giocatore: sovietica con l'Alfa (N5)
    double ora_inizio_s = 9.0 * 3600.0;            // ora UTC dell'inizio della missione, per le ore sui segni
    struct Righello {
        bool aperto = false;
        Vec2 a, b;                                  // metri
        bool a_battello = true;                     // l'estremo A segue il battello (EBL/VRM centrati sul battello)
        bool sposta_b = true;                       // l'estremo che si muove con le frecce
    } righello;
    int linee = 0;                                  // linee di rilevamento: 0 spente, 1 bersaglio, 2 anche i tracker
    int anelli = 0;                                 // 0 spenti, 1 fissi (1, 2, 5 nm), 2 anche il filo del siluro
    bool cpa = false;                               // CPA/TCPA sul bersaglio con una soluzione
    LucidoCarta lucido;
    // artiglieria (docs/16, G6): le cadute viste dal battello, croci ostili a parte dai segni del giocatore
    std::vector<Segno> cadute;
    // simboli, note e zone (7): un cursore; Tab l'affiliazione, [ ] il tipo (subacqueo, superficie, aereo, nota,
    // croce, zona), Invio mette il segno e apre la scritta, K toglie l'ultimo
    struct ModoSegni {
        bool aperto = false, scrivendo = false;
        Vec2 cursore;
        Segno::Affiliazione affiliazione = Segno::Affiliazione::ostile;
        int tipo = 0;                               // 0-4: Segno::Dominio; 5: zona da evitare
        double raggio_zona_m = MIGLIO;
    } segni;
    // regolo di Ekelund (Q): sulle linee di una fonte; frecce rotta e distanza, [ ] velocita', Invio passa al MATE
    struct RegoloEkelund {
        bool aperto = false;
        std::string fonte;
        double distanza_m = 5000.0, rotta = 90.0, velocita_kn = 10.0;
    } ekelund;
    bool tempo_rilevamento = false;                 // pannello tempo-rilevamento (9)
    double contorno_sicurezza_m = 0.0;              // quota + margine: l'isobata di sicurezza (0: spenta)
    // livelli accendibili (la carta del briefing, Fase 4a): fondale, zone della radio, intelligence, battello e rosa,
    // riga dell'ingrandimento
    struct Livelli {
        bool fondale = true, zone = true, intelligence = true, battello = true, intestazione = true;
        bool meteo = false;   // strati meteo dell'Enhanced (docs/15, M3): vento, mare, celle, ghiaccio, nebbia; spento nel classico
        bool mobilia = false; // Fase 3: cornice coi minuti, rosa, scala, cartiglio e scandagli (grafica/carta_vga); spenta nel classico
        bool toponimi = true; // con la mobilia: i nomi di mari, stretti, isole, capi, citta' e basi (grafica/carta_vga)
    } livelli;

private:
    int scala_ = 6, passo_ = 2;                     // passo: DS:03C0
    int minuti_ = 10;                               // DS:15AC
};

}  // namespace m688
