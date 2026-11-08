#pragma once
// Emettitori radar, EMCON ed ESM dell'Enhanced (punto 23 della roadmap: «ESM ripensato»). Numeri dal dossier
// docs/ricerca/11_emettitori_radar_esm.md §6, dati in data/motore/emettitori.toml (tools/pipeline/emettitori.py).
//
// Il modulo e' puro: non conosce la missione.
// - Radar: banda, periodo di scansione, altezza, R50 (distanza a cui un giro vede l'albero con probabilita' 0,5) per
//   periscopio, snorkel e nave, col mare. Un giro vede l'albero con p = F_op F_vel / (1 + (R / R50)^4), entro
//   l'orizzonte radar 1,23 nm (sqrt(h_r ft) + sqrt(h_t ft)) (dossier 02 §7.7, 11 §6.4); l'esposizione si accumula.
// - EMCON dell'IA: silenzio, navigazione, ricerca, intermittente (aerei: acceso e spento a caso) e ingaggio, con gli
//   eventi che la fanno salire (scoperti, minaccia aerea, contatto sonar, albero visto, lancio di missili) e il ritorno
//   allo stato di partenza dopo 20 minuti senza allarmi (dossier 11 §6.2).
// - ESM del battello (WLR-8 sul 688, Bukhta per i sovietici): a sola andata, entro l'orizzonte per il fattore del
//   radar, un intercetto per passaggio del fascio; rilevamento con l'errore della banda che cala con i passaggi,
//   identificazione al terzo passaggio (firma unica 0,9, condivisa 0,6, agilita' di frequenza 0,3, un'identita' sbagliata del gruppo con 0,1),
//   distanza stimata con il 40% d'errore, ampiezza da 1 a 5, tendenza oltre 3 dB/min e rischio di contro-scoperta
//   dal rapporto fra la distanza stimata e la R50 del radar contro il nostro albero (dossier 11 §6.3).

#include <cstdint>
#include <deque>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

enum class RuoloRadar : std::uint8_t { navigazione, superficie, ricerca_aerea, asw_aereo, tiro, testa_missile };

struct Radar {
    std::string id, nome, banda, gruppo;
    RuoloRadar ruolo = RuoloRadar::navigazione;
    double ghz = 9.0, periodo_s = 3.0;
    double altezza_m = 20.0;               // 0: la quota della piattaforma (aerei, missili)
    double r50_periscopio_mare2_m = 0.0, r50_periscopio_mare4_m = 0.0, r50_snorkel_mare2_m = 0.0;
    double contro_nave_m = 0.0;            // 0: fino all'orizzonte
    double fattore_esm = 1.2;              // portata d'intercetto: l'orizzonte per questo fattore
    bool puntato = false;                  // tiro e teste: illumina solo il bersaglio
    bool firma_unica = false;              // l'ESM lo riconosce senza ambiguita'
    bool agilita_frequenza = false;        // salti di frequenza impulso per impulso (Korshun): identificazione 0,3
};

struct RicevitoreEsm {
    std::string id, nome;
    double ghz_min = 0.05, ghz_max = 20.0;
    double p_intercetto = 0.95, p_intercetto_basse = 0.8;   // per passaggio, in E-J e in B-D
    double errore_ij_gradi = 5.0, errore_eh_gradi = 8.0, errore_bd_gradi = 12.0;
    double altezza_m = 1.5;
};

struct Intermittenza {                     // aerei in ricerca: acceso per [on], spento per [off] (s, a caso)
    double on_min = 6.0, on_max = 18.0, off_min = 120.0, off_max = 480.0;
};

struct CatalogoElettronico {
    std::map<std::string, Radar> radar;
    std::map<std::string, RicevitoreEsm> esm;
    std::map<std::string, std::vector<std::string>> suite;   // classe -> radar di bordo
    std::map<std::string, std::string> esm_di_classe;         // classe -> ricevitore
    std::map<std::string, Intermittenza> intermittenza;       // classe d'aereo -> ciclo
    // [antenne.<classe>]: la quota dell'antenna sul mare misurata sul modello del kit, per classe e radar
    std::map<std::pair<std::string, std::string>, double> antenne;
    const Radar* trova_radar(const std::string& id) const;
    // l'altezza dell'antenna di `r` sulla classe: quella del modello se c'e', se no quella del radar
    double altezza_antenna(const std::string& classe, const Radar& r) const;
    std::vector<const Radar*> radar_di(const std::string& classe) const;
    const RicevitoreEsm* esm_di(const std::string& classe) const;
    const Intermittenza* intermittenza_di(const std::string& classe) const;
};
CatalogoElettronico carica_elettronica(const std::string& percorso);   // std::runtime_error se manca o e' incompleto

// --- scoperta radar dell'albero ---
enum class Esposizione : std::uint8_t { periscopio, albero, snorkel, emerso };
double orizzonte_radar_m(double altezza_radar_m, double altezza_bersaglio_m);
double altezza_esposta_m(Esposizione e);
// R50 contro l'esposizione col mare (Douglas, dossier 21 §6.1): sotto 2 meglio, fra 2 e 4 lineare, da 5 in su periscopio
// 0 (i radar per periscopi 0,4, 0,2, 0,1) e snorkel 0,3 del mare 2;
// l'albero (antenna e periscopio insieme, circa 1 m2) come un periscopio con (1 / 0,5)^(1/4); emerso: 0,6 della nave
double r50_m(const Radar& r, Esposizione e, int stato_mare);
// probabilita' che un giro veda l'albero; f_op: 0,3 non allertato, 0,7 allertato, 0,9 con un dato noto
// `pioggia_mm_h` (Enhanced col meteo): attenuazione in banda X ed eco della pioggia (dossier 21 §6.2)
// `altezza_vela_m` > 0 (emerso): la vela vera della classe sull'acqua per l'orizzonte, al posto dei 6 m fissi
double probabilita_giro(const Radar& r, double distanza_m, double altezza_radar_m, Esposizione e, int stato_mare,
                        double f_op, double velocita_kn, double pioggia_mm_h = 0.0, double altezza_vela_m = -1.0);
// Enhanced coi periscopi (docs/14, P1; nucleo/periscopi: sagoma_testa): la testa vera al posto dell'esposizione
// «periscopio». La R50 del periscopio scala con la radice quarta della sezione efficace (0,5 m2 a 1,5 m e' il
// riferimento), l'orizzonte con l'altezza della testa, il pennacchio con la piuma (1: quello del dossier 11)
struct TestaEsposta {
    double sezione_m2 = 0.0;               // efficace (gia' per l'altezza esposta)
    double altezza_m = 0.0;
    double piuma = 1.0;
};
double probabilita_giro(const Radar& r, double distanza_m, double altezza_radar_m, const TestaEsposta& testa,
                        int stato_mare, double f_op, double velocita_kn, double pioggia_mm_h = 0.0);

// --- EMCON dell'IA ---
enum class StatoEmcon : std::uint8_t { silenzio, navigazione, ricerca, intermittente, ingaggio };
enum class EventoEmcon : std::uint8_t { scoperti, minaccia_aerea, contatto_sonar, albero_visto, lancio_missili };
const char* nome_emcon(StatoEmcon s);

// stato di partenza (dossier 11 §6.2): navi USA in navigazione, sovietiche in ricerca, aerei intermittenti, sottomarini
// e armi in silenzio, mercantili col radar di navigazione
StatoEmcon emcon_di_partenza(const std::string& categoria, bool sovietico, bool aereo, bool mercantile);

class Emcon {
public:
    Emcon(StatoEmcon partenza, std::optional<Intermittenza> ciclo, std::uint64_t seme);
    void evento(EventoEmcon e, double t);
    void avanza(double t);
    bool emette(const Radar& r) const;     // il radar e' acceso adesso
    StatoEmcon stato() const { return stato_; }
    StatoEmcon partenza() const { return partenza_; }

private:
    StatoEmcon partenza_, stato_;
    std::optional<Intermittenza> ciclo_;
    Casuale caso_;
    double prossimo_cambio_ = -1.0, ultimo_allarme_ = -1e18;
    bool acceso_ = false, continuo_ = false;
};

// --- ESM del battello ---
struct EmissioneVista {                    // un radar acceso, con quello che serve all'ESM
    int unita = -1;
    const Radar* radar = nullptr;
    Vec2 pos;
    double altezza_m = 20.0;               // dell'antenna radar sul mare
    bool puntato_su_di_noi = false;        // tiro o testa sul battello
    bool aereo = false;
};

enum class Rischio : std::uint8_t { sicuro, attenzione, probabile, scoperto };
enum class Tendenza : std::uint8_t { stabile, avvicinamento, allontanamento };
const char* nome_rischio(Rischio r);

struct Intercettazione {
    int unita = -1;                        // l'emettitore vero (per correlare col contatto, non da mostrare)
    std::string radar;                     // id vero
    std::string identita;                  // quello che dice l'operatore: il nome, una classe generica o un errore
    bool identificato = false;
    std::string banda;
    double rilevamento = 0.0;              // gradi veri, media dei passaggi
    double errore_gradi = 0.0;             // 1 sigma della media
    double distanza_stimata_m = 0.0;
    int ampiezza = 1;                      // 1 appena sopra soglia ... 5 vicino
    Tendenza tendenza = Tendenza::stabile;
    Rischio rischio = Rischio::sicuro;
    bool aereo = false, asw_aereo = false;
    int passaggi = 0;
    double prima = 0.0, ultima = 0.0;      // s di missione
};

struct EventoEsm {
    enum class Tipo : std::uint8_t { nuovo, aereo_asw, rischio_alto, illuminato, perso };
    Tipo tipo = Tipo::nuovo;
    double tempo = 0.0;
    int unita = -1;
    std::string radar;
};

class EsmBattello {
public:
    EsmBattello(const CatalogoElettronico& c, const RicevitoreEsm& r, std::uint64_t seme);
    // `albero`: antenna ESM fuori dall'acqua; `esposizione` e `stato_mare` per il rischio
    void avanza(double t, double dt, Vec2 pos, double quota_m, bool albero, Esposizione esposizione, int stato_mare,
                const std::vector<EmissioneVista>& emissioni);
    const std::vector<Intercettazione>& intercettazioni() const { return lista_; }
    int livello_originale() const;         // 0-85 come DS:476C: l'emettitore piu' vicino
    std::vector<EventoEsm> eventi();

private:
    struct Traccia {
        Intercettazione i;
        double somma_sin = 0.0, somma_cos = 0.0, somma_log_distanza = 0.0;   // log10 della distanza, media mobile
        std::deque<std::pair<double, double>> livelli;   // (tempo, dB relativi)
        bool rischio_avvisato = false, illuminato_avvisato = false, identita_decisa = false;
    };
    const CatalogoElettronico* catalogo_;
    RicevitoreEsm ricevitore_;
    Casuale caso_;
    std::map<std::pair<int, std::string>, Traccia> tracce_;
    std::vector<Intercettazione> lista_;
    std::vector<EventoEsm> eventi_;
    double gauss();
};

}  // namespace m688
