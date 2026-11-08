#pragma once
// Registratore dei contatti e ACINT (S5d; dossier docs/ricerca/09_sonar_sistemi_e_interfacce.md §2.8-2.9, §3.8):
// la storia acustica di ogni contatto (righe, livello, rilevamento, DEMON nel tempo), le righe stabili, il confronto
// con la libreria per la classificazione con la scala NATO (NONSUB, POSSUB, PROBSUB, CERTSUB) e il rapporto ACINT di
// fine missione: per contatto la durata, la classificazione, le righe e quelle che la libreria non conosceva.
//
// Il modulo e' puro: la stazione o la missione registrano, qui si legge. Deterministico.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/analisi_acustica.hpp"

namespace m688 {

struct CampioneContatto {
    double t = 0.0;
    double rilevamento = 0.0;            // gradi veri
    double livello_db = 0.0;             // eccesso di segnale o S/N del tracker
    std::vector<RigaOsservata> righe;    // righe del fascio in quel momento
    std::optional<LetturaDemon> demon;
    std::string sensore;
};

class Registratore {
public:
    static constexpr std::size_t MASSIMO = 4320;   // campioni per contatto (6 ore a uno ogni 5 s)
    // i campioni oltre il massimo dimezzano la storia piu' vecchia (uno ogni due), cosi' la missione resta tutta
    void registra(int contatto, const CampioneContatto& c);
    const std::vector<CampioneContatto>* storia(int contatto) const;
    std::vector<int> contatti() const;   // in ordine di prima registrazione
    void dimentica(int contatto);
    bool vuoto() const { return storie_.empty(); }

private:
    std::map<int, std::vector<CampioneContatto>> storie_;
    std::vector<int> ordine_;
};

// Una riga presente in una parte apprezzabile della storia: frequenza media, S/N medio, presenza (0..1), deriva
// (massimo - minimo, Doppler e cambi di giri)
struct RigaStabile {
    double hz = 0.0, snr_db = 0.0, presenza = 0.0, deriva_hz = 0.0;
    int campioni = 0;
};
// Raggruppa le righe dei campioni entro la tolleranza relativa e tiene quelle presenti almeno in `presenza_min`
std::vector<RigaStabile> righe_stabili(const std::vector<CampioneContatto>& storia, double tolleranza_rel = 0.004,
                                       double presenza_min = 0.3);

enum class Confidenza { nonsub, possub, probsub, certsub };
const char* nome_confidenza(Confidenza c);   // "NONSUB", "POSSUB", "PROBSUB", "CERTSUB"

// Classificazione di un gruppo di righe (con le pale del DEMON) contro la libreria: la migliore ipotesi del confronto per
// rapporti con almeno due righe concordi, la scala NATO dalla sua confidenza e dalle righe (STIMA DI GIOCO: CERTSUB con
// confidenza 0,8 e 5 righe, PROBSUB con 0,55 e 3, POSSUB per un sottomarino meno sicuro o senza classe, NONSUB se la
// migliore non e' un sottomarino). Le prime tre ipotesi.
struct Classificazione {
    std::string classe;                          // "" se nessuna
    bool sottomarino = false;
    Confidenza confidenza = Confidenza::possub;
    std::vector<ConfrontoRapporti> ipotesi;
};
Classificazione classifica_righe(const std::vector<RigaOsservata>& righe, std::optional<int> pale, const CatalogoAcustico& c);

struct VoceAcint {
    int contatto = -1;
    double t_primo = 0.0, t_ultimo = 0.0;
    double rilevamento_primo = 0.0, rilevamento_ultimo = 0.0;
    double livello_massimo_db = 0.0;
    std::vector<RigaStabile> righe;
    std::optional<int> pale;                     // dal DEMON, la lettura piu' frequente
    std::vector<ConfrontoRapporti> ipotesi;      // le prime tre della libreria
    std::string classe;                          // la migliore ("" se nessuna)
    bool sottomarino = false;                    // la migliore e' un sottomarino
    Confidenza confidenza = Confidenza::nonsub;
    std::vector<RigaStabile> nuove;              // righe stabili che la classe migliore non spiega: ACINT
    std::vector<std::string> testo_it, testo_en;
};
struct RapportoAcint {
    std::vector<VoceAcint> voci;
    std::vector<std::string> testo_it, testo_en;   // l'intestazione e le voci, una riga ciascuna
};
// La classificazione di un contatto dalla sua storia (righe stabili e pale contro la libreria, confronto per rapporti)
VoceAcint classifica_contatto(int contatto, const std::vector<CampioneContatto>& storia, const CatalogoAcustico& c);
// Il rapporto di fine missione su tutti i contatti registrati per almeno `durata_min_s`
RapportoAcint rapporto_acint(const Registratore& r, const CatalogoAcustico& c, double durata_min_s = 60.0);

}  // namespace m688
