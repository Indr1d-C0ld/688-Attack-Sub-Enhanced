#pragma once
// Il sonarista dell'equipaggio (S5d; dossier docs/ricerca/09_sonar_sistemi_e_interfacce.md §2.7-2.9, §3.9): rileva,
// traccia e classifica i contatti con errori plausibili secondo l'esperienza. Ritardo fra il segnale e il rapporto,
// tempo d'analisi (30-45 s per il passo 2), classificazioni sbagliate (la seconda ipotesi, un biologico preso per
// sottomarino), rilevamenti rumorosi, tracce perse quando il segnale cala e scambi di tracker negli incroci.
// I numeri sono STIMA DI GIOCO motivata nel dossier 09 §3.9.
//
// Il modulo e' puro e deterministico: la missione (o la stazione) gli passa a ogni passo cio' che i sensori del
// battello sentono, con la verita' per gli errori, e il proprio Casuale&. Esce la lista delle tracce «Sierra» e i
// rapporti al comandante.

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "nucleo/acint.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

enum class Esperienza : std::uint8_t { recluta, medio, esperto };
const char* nome_esperienza(Esperienza e);
std::optional<Esperienza> esperienza_da_nome(std::string_view s);   // recluta | medio | esperto

// Il sonarista di un'unita' dell'IA (fase 2 delle interazioni fra IA, punto C; nota condivisa
// ia-acustica-fase2-proposta.md): lo stesso Sonarista del giocatore, uno per unita'. Il livello viene dall'avversario
// della parte (0 impreparato -> recluta, 1 regolare -> medio, 2 veterano e 3 elite -> esperto), e la chiave d'unita'
// `sonarista` lo sovrascrive (un nome sconosciuto o vuoto non conta). Il seme e' distinto per unita'.
Esperienza esperienza_ia(int avversario, std::string_view sovrascrittura = {});
std::uint64_t seme_sonarista_ia(std::uint64_t seme_missione, int unita);

struct ProfiloSonarista {
    double soglia_db = 1.0;              // eccesso di segnale che serve per accorgersene
    double ritardo_s = 30.0;             // da quando il segnale e' sopra soglia al rapporto
    double analisi_s = 45.0;             // dal rapporto alla classificazione
    double errore_classe = 0.15;         // probabilita' di scegliere la seconda ipotesi della libreria
    double biologico_come_sub = 0.10;    // probabilita' di chiamare POSSUB un biologico
    double sigma_extra_deg = 1.0;        // rumore in piu' sul rilevamento del sensore
    double tenuta_s = 120.0;             // senza segnale per tanto: traccia persa
    double scambio = 0.2;                // probabilita' di scambiare due tracce che si incrociano
    double soglia_righe_db = 3.0;        // S/N nel bin di una riga per vederla (l'esperto ne vede di piu' deboli; STIMA)
    // blocco 2 del sonar, F: la parte delle righe del proprio battello che l'operatore toglie col catalogo del rumore
    // proprio (la recluta ne lascia alcune, e le scambia per un contatto: dossier 09 §3.9; STIMA). Il medio e l'esperto
    // le tolgono tutte: le vede in ogni fascio, anche in quelli vuoti (29/10/2026: col 5% lasciato dal medio, un
    // contatto solo prendeva la classe delle proprie righe, I07 del collaudo della libreria)
    double maschera_proprie = 1.0;
    // le righe uguali in piu' fasci nello stesso istante (sentite su due o piu' sorgenti diverse) sono del proprio
    // battello o della rete comune: l'operatore le toglie, ognuna con questa probabilita' (scelta per frequenza e per
    // partita, come la maschera). 29/10/2026: con le sole 2 righe proprie rimaste alla maschera, 60 e 300 Hz, ogni
    // contatto di SCENE1B diventava «portarinfuse»; la recluta che non ne riconosceva nessuna sbagliava 8 classi su 9.
    // Solo la recluta: il medio e l'esperto hanno la maschera piena, e la regola toglieva anche le righe vere di due
    // contatti con la stessa rete (collaudo della libreria, 29/10/2026)
    double riconosce_comuni = 0.0;
    // il falso contatto (D3): la probabilita' di accorgersi che la firma non torna, per (1 - fedelta'), alla
    // classificazione e a ogni rivalutazione (STIMA)
    double smaschera_falsi = 0.6;
};
ProfiloSonarista profilo_sonarista(Esperienza e);
// La maschera delle righe proprie: l'operatore toglie la riga `hz` se la sua estrazione (dalla frequenza e dal seme) sta
// sotto `maschera` (ProfiloSonarista::maschera_proprie). Nella stessa partita lo stesso equipaggio sbaglia sempre le
// stesse righe; fra partite diverse cambiano (il seme viene dal generatore della partita, Sonarista::passo)
bool riga_propria_tolta(double hz, std::uint64_t seme, double maschera);

// Cio' che i sensori del battello sentono di una sorgente in questo passo (il migliore dei sensori)
struct AscoltoSonar {
    int sorgente = -1;                   // id dell'unita' (verita')
    double rilevamento = 0.0;            // vero
    double se_db = -100.0;               // eccesso di segnale
    double errore_deg = 1.0;             // errore del sensore sul rilevamento (errore_rilevamento_deg)
    std::vector<RigaOsservata> righe;    // righe del fascio sopra 0 dB (la soglia la mette l'esperienza)
    // blocco 2 del sonar, F: le frequenze delle righe del proprio battello che sono fra le righe (righe_ricevute con
    // sorgente -3): l'operatore le toglie secondo la sua esperienza (ProfiloSonarista::maschera_proprie)
    std::vector<double> proprie;
    std::optional<int> pale;             // dal DEMON
    bool biologico = false;              // verita': la sorgente e' un biologico
    // passo B: le serie del DEMON del fascio sulla sorgente (leggi_demon_fascio), per la classificazione a orecchio
    // quando non ci sono righe utili (classifica_a_orecchio); vuote: niente
    std::vector<LetturaDemon> demon;
    bool sottomarino = false;            // verita': la sorgente e' un sottomarino (per una categoria detta sbagliata)
    // verita': la classe della sorgente (chiave della libreria). Se c'e', errata dice se la classificazione e' davvero
    // sbagliata (la classe, o senza classe la categoria); vuota: errata solo per gli sbagli voluti, come prima
    std::string classe_vera;
    // un transitorio dello scafo della sorgente (portello, attrezzo, pompa...: missione_scafo del runtime) sentito
    // nell'ultimo secondo: dice «sottomarino» subito (POSSUB a un contatto nuovo, PROBSUB a una traccia che c'e')
    bool transitorio_scafo = false;
    // il falso contatto del vetrino di Dio (docs/28 §3.5, D3): la verita' (per errata e per lo smascheramento), quanto la
    // firma imita la classe (UnitaAcustica::fedelta) e se un ping di chi ascolta ha trovato un'eco d'esca su di lui
    bool falso = false;
    double fedelta = 1.0;
    bool eco_esca = false;
};

// La classificazione a orecchio e col DEMON (passo B; STIMA di gioco): per i sensori senza righe utili l'operatore sente le
// eliche. Da la categoria (nave o sottomarino) e, per le navi, il tipo generico; mai la classe.
// - sottomarino con sette pale o piu' (le eliche dei sottomarini), con sei a 1,8 Hz d'asse o piu' (la cargo a sei pale
//   gira piano), o a giri d'asse alti oltre 2,6 Hz, 156 al minuto (un'elica profonda deve girare forte per cavitare);
//   altrimenti nave, che cavita anche a pochi giri perche' l'elica e' vicina alla superficie. Sbaglia, come un operatore:
//   un Victor che cavita piano vicino alla superficie sembra una nave;
// - le navi: «nave da guerra veloce» da 1,8 Hz d'asse in su, «mercantile» sotto 1,5 Hz, nessun tipo in mezzo.
// Senza serie del DEMON non c'e' niente da dire (valida falso)
struct ClassificazioneUdito {
    bool valida = false;
    bool sottomarino = false;
    std::string tipo_it, tipo_en;        // vuoti: nessun tipo
};
ClassificazioneUdito classifica_a_orecchio(const std::vector<LetturaDemon>& serie);

struct TracciaSonar {
    enum class Stato : std::uint8_t { in_analisi, classificata, persa };
    int numero = 0;                      // Sierra <numero>
    int sorgente = -1;                   // la sorgente che la traccia segue davvero (dopo uno scambio, l'altra)
    Stato stato = Stato::in_analisi;
    double rilevamento = 0.0;            // misurato, con l'errore
    double se_db = 0.0;
    double t_primo = 0.0, t_visto = 0.0;
    std::string classe;                  // "" finche' non classificata, o senza classe
    Confidenza confidenza = Confidenza::possub;
    bool errata = false;                 // verita': la classificazione e' sbagliata
    std::string tipo;                    // a orecchio, per le navi: «nave da guerra veloce», «mercantile»; vuoto se no
    double t_classificata = 0.0;         // l'ultima classificazione (o rivalutazione: RIVALUTA_S)
};

// classificato vale anche per la rivalutazione di una traccia gia' classificata («riclassificato»: solo in meglio)
struct RapportoSonar {
    enum class Tipo : std::uint8_t { nuovo, classificato, perso, ritrovato };
    Tipo tipo = Tipo::nuovo;
    int numero = 0;
    double t = 0.0;
    std::string testo_it, testo_en;
};

class Sonarista {
public:
    explicit Sonarista(Esperienza e = Esperienza::medio, const CatalogoAcustico* catalogo = nullptr)
        : profilo_(profilo_sonarista(e)), catalogo_(catalogo) {}
    Sonarista(const ProfiloSonarista& p, const CatalogoAcustico* catalogo) : profilo_(p), catalogo_(catalogo) {}

    // un passo all'istante t (i passi possono essere di qualunque durata; i tempi contano dall'istante)
    std::vector<RapportoSonar> passo(double t, const std::vector<AscoltoSonar>& ascolti, Casuale& caso);

    const std::vector<TracciaSonar>& tracce() const { return tracce_; }
    const TracciaSonar* traccia_di(int sorgente) const;   // la traccia che segue la sorgente, se c'e'
    const ProfiloSonarista& profilo() const { return profilo_; }
    // un profilo nuovo senza perdere le tracce (l'allerta della fase 3 moltiplica i falsi contatti dell'IA): vale dal
    // prossimo passo
    void profilo(const ProfiloSonarista& p) { profilo_ = p; }
    int scambi() const { return scambi_; }   // verita', per il debriefing
    // il seme della maschera delle righe proprie: preso al primo passo da una copia del generatore (la sua sequenza non
    // cambia); 0 prima del primo passo
    std::uint64_t seme_maschera() const { return seme_maschera_; }

private:
    ProfiloSonarista profilo_;
    const CatalogoAcustico* catalogo_ = nullptr;
    std::map<int, double> in_attesa_;        // sorgente -> da quando e' sopra soglia
    std::map<int, double> scafo_sentito_;    // sorgente -> l'ultimo transitorio di scafo sentito
    std::vector<TracciaSonar> tracce_;
    std::map<std::pair<int, int>, bool> vicine_;   // coppie di tracce gia' vicine (un incrocio conta una volta)
    int prossimo_ = 1, scambi_ = 0;
    std::uint64_t seme_maschera_ = 0;
    bool seme_preso_ = false;
};

}  // namespace m688
