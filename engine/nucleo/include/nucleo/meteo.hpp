#pragma once
// Meteo e mare dell'Enhanced (docs/15_meteo_e_mare.md, pezzo M1; contratto del dossier docs/ricerca/21_meteo_e_mare.md
// §11, citato per paragrafo). Vale solo con la regola di simulazione: il classico non lo usa.
//
// Un solo stato fisico (§2) da cui derivano tutte le grandezze: regime sinottico semi-Markov per teatro, vento AR(1)
// con la Weibull del mese, mare da vento che rilassa verso Pierson-Moskowitz col fetch, swell separato, nubi e
// precipitazione per regime, visibilita' di Koschmieder, nebbia, temperature, pressione con la tendenza, ghiaccio,
// spruzzo gelante, eventi rari di Poisson, celle temporalesche coi lampi (§4).
//
// Il modulo e' puro e deterministico: un generatore per processo (aggiungerne uno non cambia gli altri), passo orario
// interno interpolato al secondo, lo stesso seme ridà lo stesso meteo. Dati da data/motore/meteo.toml
// (tools/pipeline/meteo.py, pezzo M0).

#include <array>
#include <cstdint>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// --- la scala unica (§8 punto 2): Douglas da Hs, Beaufort dal vento ---

int classe_douglas(double hs_m);         // 0-9 dai limiti di WMO 3700
double centro_douglas(int stato_mare);   // Hs al centro della classe (0,875 m a mare 3)
int beaufort(double vento_m_s);          // 0-12

// --- stato ---

enum class Regime : std::uint8_t { anticiclonico, prefrontale, settore_caldo, fronte_freddo, postfrontale, bloccato,
                                   levante, poniente, vendaval, cut_off, bora_chiara, bora_scura, scirocco, maestrale,
                                   etesio, temporale_autunnale, monsone, ciclone_esplosivo, flusso_nebbioso, calma };
enum class Nebbia : std::uint8_t { nessuna, avvezione, fumo_di_mare, radiazione, precipitazione };
const char* nome_regime(Regime r);   // le chiavi di meteo.toml
std::optional<Regime> regime_da_nome(const std::string& s);
const char* nome_nebbia(Nebbia n);

struct StatoMeteo {
    Regime regime = Regime::anticiclonico;
    double vento_m_s = 5.0, vento_da_gradi = 270.0, raffica_m_s = 7.0;
    double hs_vento_m = 0.5, tp_vento_s = 4.0;                    // mare da vento
    double hs_swell_m = 0.0, tp_swell_s = 0.0, swell_da_gradi = 270.0;
    double nubi_ottavi = 4.0, plafond_m = 1500.0;
    double pioggia_mm_h = 0.0, neve_mm_h = 0.0;
    double visibilita_m = 30000.0;                                // Koschmieder, contrasto 0,02
    Nebbia nebbia = Nebbia::nessuna;
    double strato_nebbia_m = 0.0;                                 // spessore (fumo di mare)
    double lampi_al_minuto = 0.0;                                 // 0: niente temporale
    double t_aria_c = 10.0, t_mare_c = 10.0;
    double pressione_hpa = 1013.0, tendenza_hpa_3h = 0.0;
    double ghiaccio = 0.0;                                        // frazione nel punto
    double spruzzo_gelante_cm_h = 0.0;                            // Overland
    std::string evento;                                           // l'evento raro in corso ("polar_low", ...) o ""
    double hs_m() const;                                          // sqrt(vento^2 + swell^2)
    int stato_mare() const;                                       // Douglas da hs_m()
    double schiuma() const;                                       // Monahan: frazione delle creste
};

// Il meteo «semplificato» dell'opzione di realismo e del confronto: mare 3, sereno, vento di 5 m/s, visibilita' 30 km
StatoMeteo meteo_semplificato();

// Una scarica: colpi a ~57 ms l'uno dall'altro (§2); nube_mare se tocca il mare (sonar, scoppio)
struct Lampo {
    Vec2 pos;
    double t = 0.0;
    int colpi = 1;
    bool nube_mare = false;
};

// Una cella temporalesca in moto (per la carta, il radar e i lampi)
struct CellaTemporale {
    Vec2 pos;                                                     // centro al tempo richiesto
    double raggio_m = 10000.0;
    double inizio = 0.0, fine = 0.0;                              // s di missione
    double lampi_al_minuto = 2.0;
};

// --- dati: data/motore/meteo.toml ---

struct GeneraleMeteo {
    std::vector<double> douglas_limiti_m, douglas_centri_m, beaufort_limiti_m_s;
    double hs_pm_coeff = 0.22, tp_pm_coeff = 5.0;
    std::array<double, 2> tau_crescita_h{4.0, 8.0}, tau_decadimento_h{12.0, 24.0}, raffica_fattore{1.3, 1.5};
    std::array<double, 3> cem_cd{0.001, 1.1, 0.035};
    std::array<double, 2> cem_fetch{0.0413, 0.751}, cem_durata{0.00523, 1.5};
    double koschmieder = 3.912;
    std::array<double, 2> estinzione_nebbia{144.7, 0.88}, estinzione_pioggia{2.24, 0.75}, estinzione_neve{10.36, 0.7776},
        acqua_pioggia{0.072, 0.88};
    int spruzzi_da_beaufort = 9;
    std::array<double, 2> schiuma_monahan{3.84e-6, 3.41};
    double overland_tf_c = -1.7;
    std::vector<double> overland_classi_ppr, overland_classi_cm_h;
    std::vector<double> previsione_orizzonti_h, previsione_errore_fronti_h, previsione_sigma_vento_m_s, previsione_sigma_hs_m;
    double previsione_burrasche_in_ritardo = 0.25;
    std::vector<std::string> previsione_mai;
    std::vector<double> bollettini_ore_utc;
    double lampi_colpi_media = 3.67, lampi_singolo = 0.3, lampi_intervallo_ms = 57.0;
    std::array<double, 2> lampi_al_minuto{1.0, 4.0};
    double lampi_canale_km = 15.0, lampi_bagliore_km = 150.0, tuono_km = 16.0, tuono_ritardo_s_km = 3.0;
    std::array<double, 2> celle_raggio_km{5.0, 15.0}, celle_vita_h{1.0, 3.0};
    double celle_radar_nm = 20.0;
    double nebbia_vento_max_m_s = 10.0;
    std::array<double, 2> nebbia_avvezione_vento_m_s{2.0, 8.0};
    double fumo_di_mare_delta_c = 9.0;
    std::array<double, 2> fumo_di_mare_strato_m{5.0, 50.0}, nebbia_visibilita_km{0.1, 0.4};
    double nebbia_notte = 1.2, nebbia_pomeriggio = 0.8, neve_sotto_c = 1.0;
    std::array<double, 2> costa_nm{5.0, 25.0};
};

struct MeseMeteo {                       // climatologia ERA5 del punto centrale
    double vento_media_m_s = 8.0, vento_p10_m_s = 3.0, vento_p50_m_s = 8.0, vento_p90_m_s = 14.0;
    double weibull_k = 2.0, weibull_c_m_s = 9.0;
    std::array<double, 8> rosa_pct{};    // da N, NE, E, SE, S, SW, W, NW
    double bft7_pct = 0.0, bft8_pct = 0.0, raffica_fattore = 1.4;
    double hs_media_m = 2.0, hs_p90_m = 3.5, tp_media_s = 8.0;
    std::array<double, 10> douglas_pct{};
    double ghiaccio_pct = 0.0, t_aria_c = 8.0, t_mare_c = 8.0;
    double nubi_pct = 70.0, coperto_pct = 50.0, sereno_pct = 5.0, nubi_basse_pct = 40.0;
    double precipitazione_pct = 20.0, intensita_mm_h = 0.5, precipitazione_forte_pct = 0.5, neve_pct = 0.0;
    double ur97_pct = 1.0, fumo_di_mare_pct = 0.0, spruzzo_moderato_pct = 0.0, spruzzo_forte_pct = 0.0;
    double pressione_hpa = 1013.0;
};

struct StagioneMeteo {
    double tau_vento_h = 18.0, tau_onda_h = 30.0, ritardo_onda_h = 4.0;
    // classi di VENTO della nota A §6.3 (calmo < 5,5 m/s, moderato, forte 10,8-17,2, burrasca): Markov a 6 h (riga ->
    // colonna, %; nan senza casi) e quote del tempo
    std::array<std::array<double, 4>, 4> markov6h_pct{};
    std::array<double, 4> quote_classi_pct{};
    double burrasche_bft8_per_stagione = 0.0, durata_bft8_h = 0.0, episodi_bft7_per_stagione = 0.0, durata_bft7_h = 0.0;
    double intervallo_minimi_giorni = 5.0, diurno_ampiezza_m_s = 0.3;
    int diurno_ora_massimo = 14;
    std::map<std::string, double> regimi;                  // quota del tempo per regime
};

struct RegimeMeteo {
    double vento_fattore = 1.0;          // della media del mese
    double da_gradi = -1.0;              // -1: dalla rosa del mese
    double nubi_pct = 50.0, pioggia_ora = 0.1, intensita_mm_h = 0.5, visibilita_km = 20.0;
    std::array<double, 2> anomalia_t_c{}; // inverno, estate
    double durata_h = 24.0, temporale_ora = 0.0;
    Nebbia nebbia = Nebbia::nessuna;
    std::vector<std::string> prossimi;
    double incanalamento = 1.0, esordio_h = 0.0;
    bool polvere = false;
    std::vector<double> sequenza_da_gradi;   // ciclone esplosivo: prima da SE, poi da NW
};

struct EventoMeteo {
    std::vector<int> mesi;
    double tasso_al_mese = 0.0;
    std::array<double, 2> durata_h{6.0, 12.0};
    std::vector<double> vento_m_s, hs_m;  // [min, max] (polar low: [media, sigma])
    double raggio_km = 0.0, traslazione_m_s = 0.0;
    std::vector<double> esordio_h;
    bool prevista = true;
    double aria_max_c = 99.0, per_giorno_di_temporale = 0.0;
    int mese_massimo = 0;
};

struct PuntoMeteo {                      // un punto ERA5 del riquadro, per mese
    std::string nome;
    double lat = 0.0, lon = 0.0;
    std::array<double, 12> vento_media_m_s{}, hs_media_m{}, ghiaccio_pct{}, ur97_pct{}, t_aria_c{};
};

struct TeatroMeteo {
    double correzione_burrasche = 1.0, incanalamento = 1.0;
    std::array<double, 8> fetch_km{};    // per settore da cui viene il vento; 0 = illimitato
    std::array<double, 4> foschia_km{30.0, 30.0, 20.0, 30.0};   // per stagione
    std::array<double, 2> swell_hs_m{}, swell_tp_s{}, swell_da_gradi{};
    std::array<std::array<double, 2>, 4> temporali_giorni{};    // per stagione: giorni [min, max]
    std::array<double, 4> brezza_m_s{};
    double lat = 0.0, lon = 0.0;         // centro
    std::vector<int> inverni_rigidi;
    std::array<MeseMeteo, 12> mesi{};
    std::map<std::string, StagioneMeteo> stagioni;   // inverno, primavera, estate, autunno
    std::map<std::string, RegimeMeteo> regimi;
    std::map<std::string, EventoMeteo> eventi;
    std::vector<PuntoMeteo> punti;
};

struct DatiMeteo {
    GeneraleMeteo generale;
    std::map<std::string, TeatroMeteo> teatri;
    const TeatroMeteo* teatro(const std::string& id) const;
};
DatiMeteo carica_meteo(const std::string& percorso);   // data/motore/meteo.toml; std::runtime_error se manca

// --- il generatore ---

class Meteo {
public:
    // `mese` 1-12, `ora_locale` all'inizio (ciclo diurno), `seme` = hash di teatro, data e missione (la missione).
    // Senza il teatro nei dati: il meteo semplificato, fisso
    Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme);
    ~Meteo();
    Meteo(const Meteo&) = delete;
    Meteo& operator=(const Meteo&) = delete;
    // Il punto (0, 0) della missione e gli assi (x est, y nord, metri): per in(pos). Predefinito: il centro del teatro
    void imposta_origine(double lat, double lon);
    // L'anno (Baltico: ghiaccio negli inverni rigidi). Prima del primo avanza
    void imposta_anno(int anno);

    void avanza(double t);                                        // s di missione; passo orario interno, interpolato
    const StatoMeteo& stato() const { return stato_; }            // nel centro del riquadro, al tempo dell'ultimo avanza
    StatoMeteo in(Vec2 pos) const;                                // con i punti ERA5, gli stretti, il ghiaccio, le celle
    std::vector<Lampo> lampi(double t0, double t1) const;         // deterministici nell'intervallo, dalle celle
    std::vector<CellaTemporale> celle(double t) const;            // le celle vive al tempo t
    // Il bollettino emesso a `emessa_t` per `valida_t` (s di missione), con l'errore del 1989 (§5.1); niente polar
    // low, getti, temporali singoli e trombe marine
    StatoMeteo previsione(double emessa_t, double valida_t) const;
    void fissa(const StatoMeteo& s);                              // editor, missione rapida, opzione «fisso»
    // Il vetrino di Dio (docs/28): fermo in tutto il teatro, ma in(pos) tiene il ghiaccio dei punti, se il meteo non era gia' fisso
    void fissa_sopra(const StatoMeteo& s);
    void libera() { fisso_.reset(); ghiaccio_dei_punti_ = false; }   // di nuovo il meteo del teatro (il vetrino di Dio)
    bool fisso() const { return fisso_.has_value(); }
    double tempo() const { return t_; }
    const std::string& teatro() const { return id_teatro_; }
    int mese() const { return mese_; }

private:
    struct Processi;                                              // generatori e stati AR(1), uno per processo
    struct Ora {
        StatoMeteo vero, base;                                    // base: senza gli eventi rari (per la previsione)
    };
    const DatiMeteo* dati_ = nullptr;
    const TeatroMeteo* teatro_ = nullptr;
    std::string id_teatro_;
    int mese_ = 1;
    double ora_locale_ = 12.0;
    std::uint64_t seme_ = 0;
    double lat0_ = 0.0, lon0_ = 0.0;
    int anno_ = 1989;
    bool taratura_ = false;                                       // la corsa breve che tara il mare: non si tara
    double t_ = 0.0;
    StatoMeteo stato_;
    std::optional<StatoMeteo> fisso_;
    bool ghiaccio_dei_punti_ = false;                             // fissa_sopra
    // generati in avanti a richiesta (anche dalla previsione): mutabili
    mutable std::unique_ptr<Processi> processi_;
    mutable std::deque<Ora> ore_;                                 // dall'ora prima_ora_ in poi
    mutable long prima_ora_ = 0;
    mutable std::vector<CellaTemporale> celle_;                   // pos: al tempo 0 (il moto e' lineare)
    mutable std::vector<Vec2> velocita_celle_;                    // m/s, parallela a celle_
    Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme, bool taratura);
    Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme, bool taratura, int);
    void genera_fino(long ora) const;
    const Ora& ora(long h) const;                                 // generata se serve; h >= prima_ora_
    StatoMeteo interpola(double t, bool base) const;
};

}  // namespace m688
