#pragma once
// Statistiche tecniche di una partita sandbox (docs/19_sandbox.md §4; nota condivisa sandbox-statistiche-contratto.md):
// dalla traccia v2 del replay (nucleo/traccia_replay), per parte e per unita'. Catene di scoperta, tempi di reazione,
// TMA, armi ed esche, esposizione, navigazione, perdite e scambio. Lo spettatore vede tutto: qui si usa la verita'.
//
// Le fonti nella traccia:
// - i campioni delle unita' (posizione vera e stimata, alberi, emissioni, lanciatore, filo);
// - il registro M1 (record E): scoperte, classificazioni, allerta, TMA, lanci, colpi, affondamenti, esche, evasioni;
// - i fatti della raccolta (record I): i contatti di tutte le unita' e gli eventi dei siluri; con
//   Missione::registra_statistiche (la sandbox) anche R1-R6 del runtime: il primo istante possibile, i lanci con la
//   stima, le evasioni con la minaccia, le emissioni di tutti, i punti nave dell'IA.
// Quello che una traccia non ha (le tracce senza R1-R6, le v1) resta a -1. Il modulo e' puro. Le partite sandbox sono
// fuori carriera: il vetrino non registra le statistiche cumulate (nucleo/statistiche), e qui non se ne scrivono.

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/traccia_replay.hpp"

namespace m688 {

// la parte di un'unita' dal flag dell'originale (+06 & 18h): 0x08 NATO, 0x10 Patto di Varsavia, 0x18 la parte del luogo
// (M7, solo nell'Enhanced), 0 neutrali
int parte_da_flag(int flag);
bool parti_ostili(int a, int b);   // il luogo e' ostile a NATO e Patto (la traccia non ha la situazione delle coppie)

struct CatenaScoperta {            // una per coppia (osservatore, bersaglio) di parti diverse
    int osservatore = -1, bersaglio = -1;
    std::string sensore;           // del primo contatto
    double t_possibile = -1.0, t_contatto = -1.0, t_solido = -1.0, t_classificato = -1.0;
    double distanza_m = 0.0;       // vera, al primo contatto
    bool classificato = false, classe_giusta = false;
    double ritardo_s() const { return t_possibile >= 0.0 && t_contatto >= 0.0 ? t_contatto - t_possibile : -1.0; }
};

struct ReazioneUnita {             // dal primo contatto su un ostile (-1: non e' successo)
    int unita = -1, su = -1;
    double t_scoperta = -1.0;
    double allerta_s = -1.0;       // all'allerta che sale
    double ingaggio_s = -1.0;      // alla prima gamba di TMA o all'ingaggio
    double lancio_s = -1.0;        // al primo lancio
    double minaccia_t = -1.0;      // il primo siluro ostile sentito
    double evasione_s = -1.0;      // da quel siluro all'evasione, all'esca o alla ritirata
};

struct TmaUnita {
    int unita = -1, bersaglio = -1, gambe = 0;
    double soluzione_s = -1.0;     // dal primo contatto (o dalla prima gamba) alla soluzione
    bool rinuncia = false;
    // al lancio, contro la verita': il giocatore dalla soluzione proposta della stazione; l'IA con R3
    std::optional<double> errore_distanza_m, errore_rotta_gradi, errore_velocita_kn;
};

// un'arma in acqua (siluro, missile con la sua unita'), oppure un attacco senza un'unita' d'arma (arma = -1): le cariche
// di profondita' e i razzi ASW (ATTACK), un colpo per attacco, a segno se ha fatto danno
struct ArmaLanciata {
    enum class Esito : std::uint8_t { a_segno, uccisione, esca, mancata, esaurita, in_corsa };
    int arma = -1, lanciatore = -1, bersaglio = -1;
    std::string classe;
    double t_lancio = 0.0, durata_s = 0.0, filo_s = 0.0;
    Esito esito = Esito::mancata;
    int vittima = -1;              // chi ha colpito davvero (anche non il bersaglio)
};
const char* nome_esito(ArmaLanciata::Esito e);

struct EsposizioneUnita {
    int unita = -1;
    double periscopio_s = 0.0, antenna_s = 0.0, schiera_s = 0.0, radar_s = 0.0;
    int ping = 0, radio = 0;
    int contro_scoperte = 0;       // contatti nuovi su di lei via ESM o mentre emetteva
};

struct NavigazioneUnita {
    int unita = -1;
    double errore_medio_m = 0.0, errore_max_m = 0.0;   // stima meno vera, media nel tempo
    int punti_nave = 0, punti_mancati = 0;
};

struct StatParte {
    int parte = 0;
    int unita = 0, perse = 0, inflitte = 0;
    double scambio = 0.0;          // inflitte / perse (inflitte se perse = 0)
    int scoperte = 0, scoperte_subite = 0;
    double ritardo_medio_s = -1.0, distanza_media_m = -1.0;
    int lanci = 0, a_segno = 0, esche = 0, esche_efficaci = 0;
    std::map<std::string, std::pair<int, int>> pk;   // per classe d'arma: a segno, lanciate
    double reazione_media_s = -1.0;                  // dalla scoperta all'allerta
    double ingaggio_medio_s = -1.0, soluzione_media_s = -1.0, errore_lancio_medio_m = -1.0;
    double lancio_medio_s = -1.0, evasione_media_s = -1.0;   // dalla scoperta al lancio; dal siluro all'evasione
    double gambe_medie = -1.0;                                // della TMA
    // esposizione: medie su tutte le unita' della parte (anche quelle senza alberi), somme
    double periscopio_medio_s = -1.0, antenna_media_s = -1.0, radar_medio_s = -1.0;
    int ping = 0, radio = 0, contro_scoperte = 0;
    // navigazione: le unita' che navigano a stima
    double errore_nav_medio_m = -1.0, errore_nav_max_m = -1.0;
    int punti_nave = 0;
};

struct StatisticheSandbox {
    double durata_s = 0.0;
    int esito = -1;                // il codice dell'evento «fine» del registro (-1: la partita non e' finita)
    std::string esito_testo;
    int giocatore = -1;            // dal metadato della traccia
    bool complete = true;          // falso con una traccia senza i fatti della raccolta (versione 1, classico)
    std::vector<CatenaScoperta> catene;
    std::vector<ReazioneUnita> reazioni;
    std::vector<TmaUnita> tma;
    std::vector<ArmaLanciata> armi;
    std::vector<EsposizioneUnita> esposizione;
    std::vector<NavigazioneUnita> navigazione;
    std::map<int, StatParte> parti;
};
StatisticheSandbox statistiche_sandbox(const Traccia& t);

// Per le serie (V3; nota sandbox-v3-aggregazione-contratto.md): i numeri di StatParte su piu' partite, per parte e per
// nome (scambio, perse, ritardo_medio_s...): media, deviazione campionaria, estremi e quantili per interpolazione
// lineare. I valori mancanti (-1) non entrano; n dice quante partite li avevano
struct ValoreSerie {
    double media = 0.0, dev = 0.0, min = 0.0, max = 0.0;
    double q10 = 0.0, mediana = 0.0, q90 = 0.0;
    int n = 0;
};
struct EsitoSerie {
    int volte = 0;
    std::string testo;
    double frequenza = 0.0;        // volte / partite
};
struct RiassuntoSerie {
    int partite = 0;
    std::map<int, EsitoSerie> esiti;                                   // codice -> volte (-1: non finite)
    std::map<int, std::map<std::string, ValoreSerie>> parti;           // parte -> nome di numeri_parte -> valore
    std::map<int, std::map<std::string, std::pair<int, int>>> pk;      // parte -> classe d'arma -> a segno, lanciate
};
// Il vettore non si riordina: la partita i resta la i (lo strumento lega le barre alle tracce)
RiassuntoSerie riassunto_partite(const std::vector<StatisticheSandbox>& v);
std::map<int, std::map<std::string, ValoreSerie>> riassunto_serie(const std::vector<StatisticheSandbox>& v);   // = .parti
std::map<std::string, double> numeri_parte(const StatParte& p);   // i nomi di riassunto_serie, i mancanti a -1

}  // namespace m688
