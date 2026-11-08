#pragma once
// Navigazione e incertezza della posizione propria (roadmap 26; docs/08 §6; dossier docs/ricerca/06 §2, §6 e §7.3).
// Solo in Enhanced: nel classico la posizione propria e' esatta e questo modulo non si usa.
//
// Un battello sa dov'e' solo per stima: l'inerziale deriva (DMINS 1,5 nm/giorno, WSN-3 0,5, Medvedica 1,5-2, Soz e
// Sigma 3), con l'oscillazione di Schuler (84 min, 0,15 nm) sopra, e la deriva raddoppia oltre 80°N; a inerziale
// guasto (o sui diesel) restano solcometro e girobussola, e l'errore e' la corrente non nota del teatro (0,2-1 kn).
// I punti nave la riportano a posto, ma costano: il Transit vuole l'albero fuori a quota periscopica e lento, e
// passa ogni 40-100 minuti; il radar e il batimetrico emettono; l'astronomico vuole il cielo sereno e la notte;
// l'Omega si riceve col filo ma e' grossolano e ambiguo di una corsia; il batimetrico eredita l'errore della carta.
// I dati (data/motore/navigazione.toml, da tools/pipeline/navigazione.py) dicono per ogni punto nave precisione,
// condizioni e costo, con fonte e confidenza. Vale per il giocatore e per l'IA allo stesso modo, per classe.
//
// NavigazioneStimata e' il navigatore di un battello: un filtro di Kalman a 4 stati (errore di posizione ed errore di
// velocita' per asse, l'ultimo un processo di Gauss-Markov) che sa quanto si fida della stima (la covarianza, quindi
// l'ellisse), e insieme l'errore vero simulato con lo stesso modello. Il filtro non conosce l'errore vero: lo vede
// solo attraverso i punti nave, che hanno il loro rumore e, quelli sulla carta, un errore sistematico che il filtro
// non sa. La stima e' la posizione vera piu' l'errore: chi la usa passa la posizione vera (la sa il mondo).
// Unita': metri e secondi dentro, miglia e nodi nei dati. x verso est, y verso nord (come nucleo/geometria.hpp).
// Il modulo e' puro e deterministico dal seme.

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

struct SistemaNavigazione {
    std::string id, nome, fonte, confidenza;
    double deriva_nm_giorno = 0.0;   // 0: nessun inerziale (stima con solcometro e girobussola)
    double schuler_nm = 0.0;
    double correlazione_h = 12.0;
    bool inerziale() const { return deriva_nm_giorno > 0.0; }
};

struct DefPuntoNave {
    std::string id, nome, marina = "tutte", opzione, fonte, confidenza;   // marina: tutte | nato | vmf
    double sigma_min_nm = 0.0, sigma_max_nm = 0.0;   // precisione a 1 sigma per asse
    double per_kn = 0.0;                              // nm in piu' per nodo d'incertezza della velocita' propria
    double durata_s = 0.0;                            // l'esposizione
    double quota_max_m = 1e9, velocita_max_kn = 1e9;
    bool albero = false, filo = false, emissione = false, cielo = false, notte = false;
    double costa_km = 0.0;                            // > 0: serve la costa entro questa distanza
    std::vector<std::string> teatri;                  // la copertura; vuoto = ovunque
    int anno_dal = 0;
    double attesa_40_min = 0.0, attesa_60_min = 0.0, attesa_70_min = 0.0, scartati = 0.0;   // satelliti
    double corsia_nm = 0.0;
    double allungamento = 1.0;                        // rapporto fra la sigma lungo la direzione debole e quella attraverso
    double frazione_isobate = 0.0;
    bool carta = false;                               // eredita l'errore della carta
    int fari_min = 0;                                 // > 0: servono tanti fari accesi in vista (il punto nave dai fari)
    bool satellite() const { return attesa_40_min > 0.0; }
};

struct TeatroNavigazione {
    double latitudine = 0.0, corrente_kn = 0.0;
};

struct CatalogoNavigazione {
    std::map<std::string, SistemaNavigazione> sistemi;
    std::map<std::string, std::string> classi;        // classe (o classe_variante) -> sistema
    std::map<std::string, DefPuntoNave> punti;
    std::map<std::string, TeatroNavigazione> teatri;
    std::string sistema_predefinito = "stimata";
    double schuler_min = 84.4, latitudine_polare = 80.0, fattore_polare = 2.0;

    // Il sistema della classe: prima il nome intero, poi togliendo i suffissi "_x" (los_angeles_flight3 -> los_angeles),
    // altrimenti il predefinito
    const SistemaNavigazione& sistema_di(const std::string& classe) const;
    const SistemaNavigazione& stima() const { return sistemi.at("stimata"); }   // a inerziale guasto
    const DefPuntoNave* punto(const std::string& id) const;
    // La corrente non nota del teatro (kn); quella di riserva se il teatro manca
    double corrente_kn(const std::string& teatro, double riserva = 0.5) const;
};
// Lancia se il file manca o e' incoerente (un sistema di classe che non c'e', il predefinito o la stimata che mancano)
CatalogoNavigazione carica_navigazione(const std::string& percorso);

// Le condizioni del battello al momento di tentare un punto nave
struct CondizioniPuntoNave {
    std::string teatro, marina = "usn";   // usn, rn, ... (le occidentali) | vmf
    int anno = 1989;
    bool gps = false;                     // l'opzione di partita «1990+»
    double quota_m = 0.0, velocita_kn = 0.0;
    bool albero = false;                  // un albero fuori (periscopio o antenna)
    bool filo = false;                    // l'antenna a filo galleggiante filata
    bool sereno = true, notte = false;    // notte o crepuscolo
    double distanza_costa_km = 1e9;
    double errore_velocita_kn = 0.0;      // l'incertezza della velocita' propria (NavigazioneStimata::sigma_velocita_kn)
    double qualita = 0.5;                 // 0 il fix migliore, 1 il peggiore della forchetta sigma_min..sigma_max
    double spaziatura_isobate_nm = 0.0;   // batimetrico: 0 = la forchetta come gli altri
    double direzione_debole_deg = 0.0;    // dove il fix e' meno preciso: l'isobata per il batimetrico
    double errore_carta_nm = 0.0;         // la carta degradata (0 = carta buona)
    int fari_in_vista = 0;                // fari accesi oltre l'orizzonte della loro luce e dentro la portata
};

enum class MotivoPuntoNave { nessuno, marina, anno, opzione, copertura, quota, velocita, albero, filo, cielo, notte, costa, fari };

struct ValutazionePuntoNave {
    bool possibile = false;
    MotivoPuntoNave motivo = MotivoPuntoNave::nessuno;   // la prima condizione che manca
    double sigma_nm = 0.0;                // per asse, attraverso la direzione debole
    double sigma_debole_nm = 0.0;         // lungo la direzione debole
    double direzione_debole_deg = 0.0;
    double sistematico_nm = 0.0;          // l'errore della carta che il filtro non sa
    double corsia_nm = 0.0;
    double durata_s = 0.0;
    bool emissione = false, albero = false;
};
ValutazionePuntoNave valuta_punto_nave(const DefPuntoNave& d, const CondizioniPuntoNave& c);

// Satelliti (Transit, Parus): l'attesa media fra due passaggi alla latitudine (minuti; interpolata fra 40, 60 e 70)
// e il primo passaggio utile dall'istante `t_s` in poi (secondi), deterministico dal seme. Il tempo si divide in
// finestre lunghe un'attesa media: in ognuna passa un satellite in un istante a caso, e una quota `scartati` dei
// passaggi non serve. -1 per un punto nave che non e' un satellite.
double attesa_satellite_min(const DefPuntoNave& d, double latitudine_deg);
double prossimo_passaggio_s(const DefPuntoNave& d, double t_s, double latitudine_deg, std::uint64_t seme);

struct EllisseErrore {
    double maggiore_m = 0.0, minore_m = 0.0;   // semiassi
    double orientamento_deg = 0.0;             // rilevamento vero dell'asse maggiore, 0-180
};
inline constexpr double ELLISSE_95 = 2.4477;   // semiassi a 1 sigma -> regione del 95% (chi quadro a 2 gradi)

class NavigazioneStimata {
public:
    // Il navigatore della classe nel teatro. `sigma_iniziale_nm`: quanto e' buona la posizione di partenza (per asse)
    NavigazioneStimata(const CatalogoNavigazione& cat, const std::string& classe, const std::string& teatro,
                       std::uint64_t seme, double sigma_iniziale_nm = 0.1);
    // Lo stesso con i pezzi scelti a mano: `stima` e' il sistema a inerziale guasto, `corrente_kn` la corrente non nota
    NavigazioneStimata(const SistemaNavigazione& sistema, const SistemaNavigazione& stima, double corrente_kn,
                       std::uint64_t seme, double sigma_iniziale_nm = 0.1, double schuler_min = 84.4,
                       double latitudine_polare = 80.0, double fattore_polare = 2.0);

    // `inerziale_ok` falso: inerziale guasto (o spento), si naviga a stima. Senza inerziale il flag non conta
    void avanza(double dt_s, double latitudine_deg, bool inerziale_ok = true);
    // Il punto nave valutato possibile (gli altri non fanno niente): aggiorna la stima e ritorna di quanto si e'
    // spostata (m). Con la corsia (Omega) il ricevitore conta le corsie dalla stima: se questa e' fuori di piu' di mezza
    // corsia, il fix cade nella corsia sbagliata e il filtro ci crede
    Vec2 punto_nave(const ValutazionePuntoNave& v);

    Vec2 stimata(Vec2 vera) const { return {vera.x + errore().x, vera.y + errore().y}; }
    Vec2 errore() const;                     // stimata - vera (m): lo sa il gioco, non chi naviga
    EllisseErrore ellisse(double k = 1.0) const;
    double sigma_m() const;                  // l'errore radiale atteso (rms), m
    double sigma_velocita_kn() const;        // l'incertezza della velocita' (rms radiale), kn
    bool a_inerziale() const { return ins_; }
    double tempo_s() const { return t_; }

private:
    using Mat4 = std::array<double, 16>;
    double sigma_bias(bool ins) const;       // per asse, m/s
    double correlazione_s(bool ins) const;
    void cambia_modo(bool ins);
    Vec2 schuler() const;

    SistemaNavigazione s_;
    double correlazione_stima_s_, corrente_ms_, schuler_s_, lat_polare_, fattore_polare_;
    Casuale rng_;
    bool ins_ = false;
    double t_ = 0.0, fattore_ = 1.0;
    // vero: errore di posizione, deriva dell'inerziale, corrente (m, m/s)
    std::array<double, 2> e_{}, b_ins_{}, c_{};
    std::array<double, 2> b_hat_{};          // la deriva che il navigatore stima e corregge
    Mat4 p_{};                               // covarianza del filtro: [ex, ey, bx, by]
    double schuler_fase_ = 0.0, schuler_dir_ = 0.0, carta_dir_ = 0.0, corsia_dir_ = 0.0;
};

}  // namespace m688
