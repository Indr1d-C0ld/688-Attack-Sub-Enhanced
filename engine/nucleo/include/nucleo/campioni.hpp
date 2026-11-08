#pragma once
// Libreria di campioni registrati (pezzi R4 e R5 del dossier docs/ricerca/18_audio_reale.md §2 e §8): le tessiture
// sbiancate di data/suoni/campioni/ (tools/pipeline/campioni_analisi.py: spettro piatto, senza le righe stabili, solo
// nella banda valida) coi metadati di data/motore/campioni.toml. Il colore lo rimette il modello (CuffiaSonar):
// livello per banda, perdita, fascio e righe restano del modello, il campione da' solo la grana nel tempo.
//
// Il modulo e' puro: carica e sceglie in modo deterministico.

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/acustica.hpp"

namespace m688 {

struct Campione {
    std::string id, voce;                    // voce: id della sorgente della libreria o classe della firma
    int campionamento_hz = 22050;
    double fattore = 1.0;                    // accelerazione della fonte (PMEL): le frequenze vere sono /fattore
    double tessitura_rms = 0.25;
    double f_min_hz = 0.0, f_max_hz = 0.0;   // banda valida, Hz veri
    double demon_hz = 0.0, demon_db = 0.0;   // modulazione d'inviluppo (Hz veri)
    std::vector<float> campioni;
    // inviluppo nel tempo (dB, ogni passo_s del file) in tre bande di Hz veri, misurato sull'originale
    std::vector<std::vector<float>> inviluppo_tempo;
    double passo_s = 0.25;
    // dall'inviluppo, al caricamento (prepara_inviluppo): per banda le somme prefisse del lineare (n + 1 valori) e la
    // media, cosi' modulazione_campione costa O(1) per finestra
    std::vector<std::vector<double>> prefisso_lineare;
    std::vector<double> media_lineare;
    std::vector<std::pair<double, double>> bande_hz;
    std::string licenza, stato, attribuzione;
    double durata_s() const { return static_cast<double>(campioni.size()) / campionamento_hz; }   // del file
    double durata_vera_s() const { return durata_s() * fattore; }
};

struct LibreriaCampioni {
    std::vector<Campione> campioni;
    std::map<std::string, std::vector<std::size_t>> per_voce;
    // uno dei campioni della voce, scelto dal seme dell'istanza (nullptr se la voce non ne ha)
    const Campione* scegli(const std::string& voce, std::uint64_t seme) const;
    bool vuota() const { return campioni.empty(); }
};

// data/motore/campioni.toml; i file relativi a `radice_dati` (data/). Una libreria vuota se il file manca;
// std::runtime_error se un WAV indicato non si legge
LibreriaCampioni carica_campioni(const std::string& percorso_toml, const std::string& radice_dati);
// WAV PCM a 16 bit (mono, o la media dei canali): false se non lo e'
bool leggi_wav(const std::string& percorso, std::vector<float>& campioni, int& campionamento_hz);

// La scelta fra i campioni di una voce (lo stesso indice per la cuffia e per l'attivita' del modello)
std::size_t indice_campione(std::uint64_t seme, std::size_t quanti);
// Il seme di un'istanza: il suo, o l'id + 1
inline std::uint64_t seme_istanza(const Sorgente& s) { return s.seme ? s.seme : static_cast<std::uint64_t>(s.id) + 1; }
// Dove sta la lettura nel file (s del file) al tempo di gioco: uno scarto dell'istanza piu' il tempo vero / fattore,
// ad anello. La cuffia legge li', e l'attivita' del modello segue l'inviluppo di li'
double posizione_campione_s(const Campione& c, std::uint64_t seme, double tempo_s);
// Le somme prefisse e le medie dell'inviluppo (carica_campioni la chiama; chi riempie inviluppo_tempo a mano anche)
void prepara_inviluppo(Campione& c);
// L'inviluppo nel tempo nella banda di `f_hz` mediato sulla finestra [t - w, t], rispetto alla sua media su tutto il
// campione (1 = come la media; al piu' 30, almeno 0,001)
double modulazione_campione(const Campione& c, std::uint64_t seme, double tempo_s, double finestra_s, double f_hz);
// Collega le voci del catalogo ai loro campioni (SorgenteLibreria::campioni) e calcola l'attivita' media delle
// componenti. Da chiamare insieme a CuffiaSonar::imposta_campioni, cosi' cascata, scoperta e cuffia seguono lo stesso
// tratto; la libreria deve vivere quanto il catalogo. Senza, il catalogo resta quello del modello
void collega_campioni(CatalogoAcustico& c, const LibreriaCampioni& l);

// La voce della libreria per una sorgente della scena: l'id della sorgente della libreria, o la classe della firma
// (mercantili, cargo e petroliere: «mercantile»; il Ka-25 col materiale del Ka-27, rotori coassiali); "" se non ce n'e'
std::string voce_campione(const Sorgente& s);

}  // namespace m688
