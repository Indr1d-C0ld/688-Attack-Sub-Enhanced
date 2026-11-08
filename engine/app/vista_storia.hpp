#pragma once
// La storia lunga delle cascate della stazione sonar (blocco 3 del sonar, I; docs/23_sonar_blocco3.md §4): lo scorrimento
// all'indietro di LOFAR e BTR dalla StoriaCascata (nucleo/storia_cascata), col cursore del tempo e la ricerca «quando e'
// comparsa». Una sola vista disegnata in piu' rettangoli (il LOFAR con l'asse in Hz, il BTR in gradi veri), che scorrono
// insieme sullo stesso tempo: la riga piu' recente della finestra in alto, come la stazione.
// - La risoluzione e' il livello della storia: 1 riga = 1 s (10 minuti), 10 s (2 ore), 60 s (12 ore).
// - Il cursore del tempo scende e sale di una riga (o di dieci); quando arriva al bordo la finestra scorre con lui.
// - «Quando e' comparsa»: la riga (o la traccia) che c'e' adesso alla frequenza (o al rilevamento) data: la finestra e il
//   cursore vanno all'inizio del suo tratto, alla risoluzione che arriva fin li'.
//
// Solo vista: le StoriaCascata le tiene la stazione (una per il LOFAR del fascio, una per il BTR). Tasti della stazione
// (principale): il modo STORIA del cursore; su e giu' il cursore (Maiusc a passo grosso), [ e ] la risoluzione, F la
// comparsa sulla frequenza del cursore del LOFAR o della riga automatica scelta; uscendo dal modo, adesso().

#include <optional>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "nucleo/storia_cascata.hpp"
#include "scena_prova.hpp"

namespace m688 {

struct StileStoria {
    bool modo_1989 = false, fosforo = false;
    cascate::Scala scala = cascate::SCALA_LUNGA;
    double pavimento_db = 0.0, tetto_db = 20.0;
    double x0 = 0.0, x1 = 0.0;       // la banda nelle unita' della storia (Hz o gradi); x0 == x1: tutta
    bool gradi = false;              // BTR: l'asse in gradi veri (la comparsa prova anche oltre 0/360)
    double ora_utc_s = -1.0;         // l'ora del giorno all'istante `ora` (per «13:42:05Z»); < 0: solo il relativo
};

class VistaStoria {
public:
    // --- comandi ---
    void cursore(int verso, bool grosso);   // +1 giu' (indietro nel tempo), -1 su; una riga o dieci
    void cursore_a(double t, double ora);   // dal clic: l'istante sotto il cursore
    void risoluzione(int verso);            // +1 piu' grossa (10, 60 s), -1 piu' fine
    void adesso();                          // di nuovo in diretta: finestra e cursore sull'ultima riga
    // quando e' comparsa la riga a `x` che c'e' adesso: finestra e cursore all'inizio del suo tratto; nullopt se adesso
    // non c'e'
    std::optional<double> cerca_comparsa(const StoriaCascata& s, double x, double ora, bool gradi = false);
    bool in_diretta() const { return indietro_s_ <= 0.0 && cursore_s_ <= 0.0; }
    double t_cursore(double ora) const { return ora - cursore_s_; }
    int livello() const { return livello_; }
    std::optional<double> comparsa() const { return comparsa_t_; }

    // --- disegno nel rettangolo: in alto la riga del tempo («STORIA  1 RIGA = 10 S  13:42:05Z  -1 H 12 MIN», o
    // «DIRETTA»), sotto la cascata della finestra, il cursore giallo, la comparsa trovata in bianco; fuori diretta un bordo.
    // Con `aree`: «storia_tempo» sulla cascata (clic: cursore_a(t_a(...))) e «storia_adesso» sulla scritta ---
    void disegna(Tela& t, const Font& f, Rettangolo r, const StoriaCascata& s, double ora, const StileStoria& st,
                 std::vector<AreaAttiva>* aree = nullptr) const;
    double t_a(Rettangolo r, const StoriaCascata& s, int y, double ora) const;   // l'istante alla riga y del rettangolo
    int y_di(Rettangolo r, const StoriaCascata& s, double t, double ora) const;   // e il contrario (fuori: fuori dal rettangolo)

    static constexpr int ALTO_TESTO = 10;
    // il testo del tempo relativo: «-1 H 12 MIN», «-4 MIN 30 S»
    static std::string relativo(double secondi);

private:
    double passo() const { return livelli_[static_cast<std::size_t>(livello_)].passo_s; }
    void segui_cursore();                   // la finestra scorre col cursore quando esce
    int livello_ = 0;
    double indietro_s_ = 0.0;               // la riga in alto della finestra: secondi prima di adesso
    double cursore_s_ = 0.0;                // il cursore: secondi prima di adesso
    // i livelli della storia (i parametri dell'ultima disegnata; prima, quelli di serie) e le righe del rettangolo piu'
    // basso disegnato: servono ai comandi, che non vedono la storia
    mutable std::vector<LivelloStoria> livelli_ = ParametriStoria{}.livelli;
    mutable int righe_ = 0;                 // 0: nessun disegno ancora (i comandi contano 60)
    std::optional<double> comparsa_t_;
};

}  // namespace m688
