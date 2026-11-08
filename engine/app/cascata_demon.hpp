#pragma once
// Il DEMON a cascata della stazione sonar dell'Enhanced (blocco 3 del sonar, G; docs/23_sonar_blocco3.md §2): l'inviluppo
// della cavitazione del fascio puntato (demon_fascio), una riga per passo della stazione, coi cursori armonici d'asse e di
// pala. Si vede quando il bersaglio cambia giri: le righe salgono o scendono, indizio di manovra.
// - In alto l'A-scan dell'ultima riga, sotto la cascata (frequenza in orizzontale come il LOFAR, la riga nuova in alto),
//   in fondo la lettura dei cursori: «ASSE 2,40 HZ  144 GIRI  PALE 7  PALA 16,8 HZ» (giri al minuto).
// - I cursori: l'asse e le sue armoniche fino alla pala in giallo, la pala e le sue in ciano. Nel modo 1989 si mettono a mano; nel
//   moderno «aggancia» li porta sulla serie trovata (leggi_demon_fascio), e di nuovo sull'altra se nel fascio ce n'e' piu'
//   d'una. La lettura va all'analizzatore di firma (ANALIZZA).
// - Le scale dei tempi della stazione (x1, x4, x16): le righe lente sono il massimo di 4 o 16 righe, come il LOFAR.
//
// Solo vista: la storia e i cursori sono suoi, la missione non li vede. Il posto (il terzo basso della colonna destra,
// sotto il LOFAR, dossier 09 §4.3) e i tasti (il quinto modo «DEMON» del cursore: sinistra e destra l'asse fine, con
// Maiusc grosso; su e giu' le pale; F aggancia o la serie successiva) sono della stazione (principale).

#include <array>
#include <deque>
#include <optional>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "scena_prova.hpp"

namespace m688 {

// Lo stile della stazione sonar, come per l'intercettazione, con la scala dei colori e quella dei tempi
struct StileDemon {
    bool modo_1989 = false;                      // cursori a mano: niente serie trovate in automatico
    bool fosforo = false;                        // SonarEnhanced::con_fosforo()
    cascate::Scala scala = cascate::SCALA_LUNGA;
    double pavimento_db = 0.0, tetto_db = 20.0;  // la scala dei colori, dB sopra il rumore
    int scala_tempi = 1;                         // 1, 4 o 16, quella della stazione
    const char* nome = "DEMON";                  // le scritte: sull'Okean dell'Alfa traslitterate, «DEMOD» e «GC»
    const char* unita = "HZ";
};

class CascataDemon {
public:
    // --- la storia: una riga per passo della stazione (lo Spettro di demon_fascio sul fascio puntato) ---
    void aggiungi(const Spettro& riga);
    void svuota();                                // cambio di fascio o di bersaglio: anche le serie trovate
    const std::deque<Spettro>& righe(int scala_tempi = 1) const { return righe_[indice_scala(scala_tempi)]; }

    // --- i cursori armonici ---
    void sposta_asse(int verso, bool fine);       // a passi di 0,1 Hz (fine) o 1 Hz; il primo tocco mette l'asse a 1 Hz
    void pale(int verso);                         // 3-9: la pala = pale x asse
    // Moderno (la stazione lo chiama solo li'): l'asse e le pale sulla prima serie trovata nell'ultima riga; se e' gia'
    // agganciato, sulla serie successiva. Falso se non c'e' nessuna serie
    bool aggancia();
    bool agganciato() const { return agganciato_; }
    double asse_hz() const { return asse_hz_; }
    int pale_n() const { return pale_; }
    // la lettura per l'analizzatore di firma: asse, pala, pale; nullopt senza cursori
    std::optional<LetturaDemon> lettura() const;
    // le serie trovate nell'ultima riga (una per sorgente distinguibile)
    std::vector<LetturaDemon> serie() const;

    // --- disegno nel rettangolo che passa la stazione (circa 316x90). Con `aree`: «demon_asse» sulla cascata (il clic
    // mette l'asse alla frequenza: frequenza_a) e «demon_serie» sull'A-scan (il clic come F) ---
    void disegna(Tela& t, const Font& f, Rettangolo r, const StileDemon& s, std::vector<AreaAttiva>* aree = nullptr) const;
    double frequenza_a(Rettangolo r, int x) const;   // la frequenza alla x della cascata
    int x_di(Rettangolo r, double hz) const;         // e il contrario

    static constexpr int RIGHE_MAX = 300;
    static constexpr int ALTO_ASCAN = 16, ALTO_TESTO = 11;   // l'A-scan in alto, la riga della lettura in basso
    static constexpr int PALE_MIN = 3, PALE_MAX = 9;
    static constexpr double PASSO_FINE_HZ = 0.1, PASSO_GROSSO_HZ = 1.0;
    static int indice_scala(int s) { return s >= 16 ? 2 : s >= 4 ? 1 : 0; }

private:
    double f_max() const;                         // dall'ultima riga (100 Hz senza righe)
    std::array<std::deque<Spettro>, 3> righe_;
    std::array<Spettro, 3> acc_;
    std::array<int, 3> acc_n_{};
    double asse_hz_ = 0.0;                        // 0: nessun cursore
    int pale_ = 5;
    bool agganciato_ = false;
    int serie_ = 0;                               // la serie agganciata
};

}  // namespace m688
