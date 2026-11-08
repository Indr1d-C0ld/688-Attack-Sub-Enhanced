#pragma once
// Stazione Sonar del giocatore nel livello classico (docs/11_comandi_e_stazioni.md §3.2, fase C4): schiera trainata,
// filtro, impulso attivo e ANALYZE. Come la stazione Armi sta nel programma e non negli script
// (docs/originale/sensori_e_navigazione.md §10.4-§10.5, `3D9D-3E4F`, `6F60-7070`): gli script fanno solo i timer
// della schiera (`schiera_cala` 60 s e messaggio 17, `schiera_recupera` 40 s e messaggio 18) e il taglio
// (`schiera_persa`, comando 36h).
//
// Stessa regola della stazione Armi: lo stato dipende solo dagli ordini consegnati e dai messaggi della missione.

#include "nucleo/ordini.hpp"

namespace m688 {

enum class StatoSchiera : std::uint8_t { filata = 0, dentro = 1, persa = 2 };          // DS:03C6
enum class ManovraSchiera : std::uint8_t { nessuna = 0, filando = 1, recuperando = 2 };  // DS:03C4

// Messaggi della stazione (testi dell'EXE, data/motore/testi/messaggi_bordo.tsv)
inline constexpr int MSG_PING = 240, MSG_ANCORA_IN_FILATURA = 241, MSG_ANCORA_IN_RECUPERO = 242;
inline constexpr int MSG_SCHIERA_GIA_TAGLIATA = 243, MSG_NON_SI_FILA = 244;
inline constexpr int MSG_PASSA_ALTO = 245, MSG_PASSA_BANDA = 246;
inline constexpr int MSG_ANALISI = 247, MSG_NO_ANALISI_AEREI = 248, MSG_SCHIERA_TAGLIATA = 249;
inline constexpr int MSG_SONAR_SERVE_BERSAGLIO = 250, MSG_CONTATTO_PERSO = 251;
inline constexpr int MSG_SCHIERA_FILATA = 17, MSG_SCHIERA_RECUPERATA = 18;   // dagli script

struct SituazioneSonar {
    double velocita_ordinata = 0.0, avanti_tutta = 1.0;   // del giocatore (posizione di velocita', 72A2)
    bool bersaglio = false;                               // c'e' un bersaglio selezionato
    bool contatto = false;                                // e il giocatore ne ha il contatto
    bool aereo = false;                                   // il bersaglio e' un aereo
};

class StazioneSonar {
public:
    // --- consegna degli ordini (schiera_cala, schiera_recupera, schiera_taglia, filtro_sonar, impulso_sonar) ---
    bool riguarda(const Ordine& o) const;
    EsitoStazione consegna(const Ordine& o, const SituazioneSonar& s);
    // Messaggi 17 e 18 degli script: schiera filata o recuperata
    void messaggio_missione(int codice);

    // Telegrafo con la schiera fuori o in manovra: 3/4, TUTTA, MAX e l'arresto rapido chiedono conferma; se il
    // comandante conferma, prima dell'ordine del telegrafo va il taglio (`taglio`), altrimenti l'ordine si annulla
    bool chiede_conferma(const Ordine& telegrafo) const;
    static Ordine taglio(double tempo) { return {tempo, TipoOrdine::schiera_taglia}; }

    // ANALYZE (3EE0): bersaglio selezionato, contatto ancora esistente di qualsiasi qualita', niente aerei; la firma
    // acustica e' del suono (docs/originale/suoni.md)
    int analizza(const SituazioneSonar& s) const;

    // Guasti del §9.4: «sonar computer» spegne il filtro, «towed array» porta la schiera a persa (il comando 36h lo
    // manda chi applica il guasto)
    void guasto_calcolatore() { filtro_ = false; }

    StatoSchiera schiera() const { return schiera_; }
    ManovraSchiera manovra() const { return manovra_; }
    bool schiera_esposta() const { return schiera_ == StatoSchiera::filata || manovra_ != ManovraSchiera::nessuna; }
    bool filtro() const { return filtro_; }   // DS:13D6: passa alto, solo le armi sulla carta e nella barra
    static int posizione_velocita(double velocita_ordinata, double avanti_tutta);   // 72A2: x 4 / avanti tutta

private:
    StatoSchiera schiera_ = StatoSchiera::dentro;
    ManovraSchiera manovra_ = ManovraSchiera::nessuna;
    bool filtro_ = false;
};

}  // namespace m688
