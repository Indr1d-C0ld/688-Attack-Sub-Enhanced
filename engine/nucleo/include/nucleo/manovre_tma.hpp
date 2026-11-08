#pragma once
// Le manovre di TMA dell'IA (fase 4 delle interazioni fra IA, docs/18_interazioni_ia.md; dossier docs/ricerca/02 §5.1-5.2
// e 24 §5.2). Dal solo rilevamento la distanza non si ricava finche' chi ascolta non manovra (Ekelund): servono almeno
// due gambe con una componente diversa della velocita' propria attraverso la linea di vista. La dottrina:
// - gambe di 3-6 minuti, piu' lunghe da lontano;
// - il bersaglio a un mascone (45 gradi per la NATO), poi sull'altro: la virata fra le gambe e' di 30-90 gradi e fa
//   cambiare l'OSA;
// - vicino al bersaglio non si accosta verso di lui: il bersaglio va oltre il traverso, cosi' la gamba nuova apre;
// - dopo ogni virata la schiera si raddrizza (2-5 minuti) prima di contare la gamba;
// - basta quando la TMA (nucleo/tma_ia.hpp, risolvi_tma) e' osservabile e la distanza e' nota entro la sigma relativa
//   della dottrina: da li' decide chi comanda l'unita' (attacco, pedinamento, ritirata).
// I numeri sono in data/motore/interazioni.toml ([tma.*], tools/pipeline/interazioni.py), per marina e per classe.
// Il modulo e' puro e deterministico: non muove l'unita', dice rotta e velocita' della gamba.

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>

#include "nucleo/casuale.hpp"

#include "nucleo/geometria.hpp"
#include "nucleo/tma_ia.hpp"

namespace m688 {

struct DottrinaTma {
    std::string marina;
    double gamba_min_s = 180.0, gamba_max_s = 360.0;
    double distanza_gamba_lunga_m = 20000.0;   // da qui in la' la gamba e' la piu' lunga
    double mascone_deg = 45.0;                 // il bersaglio a tanti gradi dalla prua, a dritta o a sinistra
    double mascone_vicino_deg = 100.0;         // lo stesso sotto vicino_m: oltre il traverso, per aprire
    double virata_min_deg = 30.0, virata_max_deg = 90.0;   // fra due gambe (da vicino il massimo non vale: si apre)
    double vicino_m = 4000.0;
    double velocita_kn = 6.0;                  // la velocita' di TMA (piano, per sentire)
    double velocita_max_kn = 10.0;             // oltre la schiera e il sonar sentono male
    double sigma_relativa = 0.15;              // la soluzione basta con sigma_distanza / distanza sotto questa
    double raddrizzamento_s = 120.0;           // dopo una virata, con la schiera filata
    double tolleranza_rotta_deg = 5.0;         // la virata e' finita entro tanti gradi dalla rotta della gamba
    double margine_fondo_m = 30.0;             // una gamba verso un fondo a meno di tanto sotto la quota si rifiuta
    double tenuta_s = 600.0;                   // senza rilevamenti da tanto: rinuncia
    int gambe_max = 8;                         // senza soluzione dopo tante gambe: rinuncia
    int gambe_max_specchio = 3;                // lo stesso col solo rilevamento a specchio irrisolto
    double variazione_gamba = 0.3;             // la durata varia a caso di +-meta' di tanto (x max-min), dal seme
    std::string fonte, confidenza;
};

struct CatalogoTma {
    std::map<std::string, std::string> marine;          // usa, uk -> nato; urss -> vmf
    std::map<std::string, DottrinaTma> dottrine;        // nato, vmf
    std::map<std::string, std::map<std::string, double>> classi;   // classe -> campi che cambiano
    std::map<std::string, std::pair<std::string, std::string>> fonti_classi;   // classe -> (fonte, confidenza)
};
// [tma.*] di interazioni.toml; lancia se manca o se una marina punta a una dottrina che non c'e'
CatalogoTma carica_tma(const std::string& percorso);
// La dottrina della marina (usa, uk, urss...; sconosciuta: nato), con le differenze della classe sopra
DottrinaTma dottrina_tma(const CatalogoTma& c, const std::string& marina, const std::string& classe = {});

struct SituazioneTma {
    double t = 0.0;                            // s di missione
    Vec2 pos;
    double rotta = 0.0, velocita_kn = 0.0;     // dell'unita' che fa la TMA
    double rilevamento = 0.0;                  // l'ultimo misurato verso il bersaglio (gradi veri)
    std::optional<StimaTma> stima;             // risolvi_tma, se c'e'
    bool schiera = false;                      // la schiera e' filata: dopo le virate si aspetta il raddrizzamento
    double quota_m = 0.0;
    double fondale_m = 0.0;                    // sotto l'unita' (0: ignoto)
    // il fondale in un punto (la carta che l'unita' usa): con questo la gamba si controlla lungo la rotta; senza, vale
    // solo fondale_m
    std::function<double(Vec2)> fondale;
    std::optional<double> t_ultimo_rilevamento;   // vuoto: adesso
    bool specchio_irrisolto = false;           // la schiera ha dato il rilevamento e il suo specchio, ancora da risolvere
};

struct DecisioneTma {
    enum class Fase { gamba, virata, raddrizzamento, soluzione };
    Fase fase = Fase::virata;
    double rotta = 0.0, velocita_kn = 0.0;     // da dare all'unita' (fuori dalla soluzione)
    bool nuova_gamba = false;                  // la rotta e' cambiata in questo passo
    bool soluzione_pronta = false;             // osservabile e dentro la sigma relativa: le gambe finiscono
    int gambe = 0;                             // gambe fatte finora
    double prossima_s = 0.0;                   // l'istante della prossima decisione attesa
    // la TMA non va avanti: contatto perso oltre la tenuta, specchio irrisolto dopo gambe_max_specchio, nessuna
    // soluzione dopo gambe_max. Chi comanda l'unita' torna alla pattuglia
    enum class Rinuncia { no, contatto_perso, specchio, gambe };
    Rinuncia rinuncia = Rinuncia::no;
    bool fondo_rifiutato = false;              // in questo passo un lato e' stato scartato per il fondale
};
const char* nome_fase_tma(DecisioneTma::Fase f);

class ManovreTma {
public:
    // `seme`: per la coppia (unita', contatto), cosi' il lato della prima gamba a parita' e la durata delle gambe sono
    // ripetibili
    ManovreTma(DottrinaTma d, std::uint64_t seme) : d_(std::move(d)), caso_(seme) {}
    // Un passo: si chiama a ogni nuova soluzione (o piu' spesso: e' idempotente fra un cambio e l'altro)
    DecisioneTma passo(const SituazioneTma& s);
    void azzera() { avviata_ = false; }       // contatto perso o bersaglio nuovo
    const DottrinaTma& dottrina() const { return d_; }

private:
    double rotta_gamba(double rilevamento, double distanza_m, int lato) const;
    double durata_gamba(double distanza_m);
    bool fondo_buono(const SituazioneTma& s, double rotta, double durata_s) const;

    DottrinaTma d_;
    Casuale caso_;
    double durata_ = 0.0;                      // della gamba in corso, decisa all'inizio
    bool avviata_ = false;
    DecisioneTma::Fase fase_ = DecisioneTma::Fase::virata;
    int lato_ = 1;                             // +1: il bersaglio a dritta (rotta = rilevamento - mascone); -1 a sinistra
    double rotta_ = 0.0, fine_raddrizzamento_ = 0.0, inizio_gamba_ = 0.0;
    int gambe_ = 0;
};

}  // namespace m688
