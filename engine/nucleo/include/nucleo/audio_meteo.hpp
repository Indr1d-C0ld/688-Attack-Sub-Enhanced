#pragma once
// Audio del maltempo (pezzo M5 di docs/15_meteo_e_mare.md; numeri e fonti in docs/ricerca/21_note/e_audio_meteo.md,
// citato «21e»). Il vento e la pioggia sono gia' nel rumore ambiente (rumore_ambiente con Ambiente.vento_m_s e
// pioggia_mm_h): li riempie la missione dal meteo, qui non si duplicano. Qui ci sono:
// - i fulmini al sonar: i colpi di un lampo nube-mare come transitori «fulmine_mare» della scena acustica (la cuffia li
//   suona a campione esatto, nucleo/ascolto.hpp; a bordo il tuono e il resto del maltempo sono in nucleo/bordo_audio.hpp);
// - le scariche in sala radio: fondo, crepitio lontano, crash dei lampi e AGC del ricevitore, in campioni.
//
// Il modulo e' puro e deterministico: lo stesso seme da' gli stessi campioni, comunque si spezzino i passi.

#include <cstdint>
#include <vector>

#include "nucleo/acustica_asw.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

// --- fulmini al sonar ---

// I colpi di un lampo che tocca il mare, uno per Transitorio «fulmine_mare» (data/motore/acustica.toml): il primo a
// l.t, i successivi a intervalli lognormali di mediana `intervallo_s` (sigma_ln 0,7, cioe' 20-180 ms al 90%) e 6 +- 3 dB
// sotto il primo (21e §5.1, STIMA sulla dispersione e sull'ampiezza). Id da `id_base` in su; vuoto se il lampo resta
// nella nube. Il ritardo in acqua (distanza / 1500 m/s) non c'e', come per gli altri transitori della scena.
std::vector<Transitorio> transitori_lampo(const Lampo& l, int id_base, std::uint64_t seme, double intervallo_s = 0.057);

// --- scariche in sala radio (21e §4 e §5.5) ---

enum class BandaRadio : std::uint8_t { hf_ssb, hf_am, vlf };
const char* nome_banda_radio(BandaRadio b);

struct ParametriScariche {
    int campionamento_hz = 22050;
    BandaRadio banda = BandaRadio::hf_ssb;   // SSB 300-2700 Hz, AM 200-4000 Hz, VLF 300 Hz-11 kHz (monitor audio)
    double fondo_dbfs = -30.0;               // il fruscio del ricevitore senza temporali, in uscita
    double riferimento_agc = 0.5;            // l'AGC tiene l'inviluppo sotto questo valore (-6 dBFS)
    bool soppressore = false;                // noise blanker: spegne per 1,5 ms gli impulsi oltre +12 dB sul fondo
    std::uint64_t seme = 1;
};

class ScaricheRadio {
public:
    explicit ScaricheRadio(const ParametriScariche& p = {});
    void imposta(const ParametriScariche& p);   // banda, fondo e soppressore dal prossimo campione; il seme no
    const ParametriScariche& parametri() const { return p_; }

    // L'ambiente delle scariche: le celle vive (Meteo::celle) rispetto a chi riceve, e la notte (il crepitio lontano
    // in HF e' 20-60 impulsi al secondo di notte, 3-10 di giorno; i tweek in VLF solo di notte). Una cella entro 50 km
    // alza il fondo di 10 dB, che torna a zero a 300 km (21e §5.5, STIMA sulla distanza)
    void ambiente(const std::vector<CellaTemporale>& celle, Vec2 pos, bool notte);
    // Un lampo (Meteo::lampi), nube-mare o nella nube: il crash parte subito (la luce), piu' forte se vicino (+35 dB
    // sul fondo entro 50 km, +20 a 300 km, +7 a 1000 km e oltre). I lampi oltre 3000 km si ignorano
    void lampo(const Lampo& l, Vec2 pos);

    // `dt_s` di audio: il fondo, il crepitio e i crash, piu' il segnale demodulato (la telescrivente, la voce) se c'e',
    // attraverso l'AGC (attacco 1 ms, rilascio 0,6 s): dopo un crash forte il segnale sparisce e risale. `segnale`, se
    // c'e', deve avere almeno i campioni del passo (gli altri si ignorano)
    std::vector<float> passo(double dt_s, const std::vector<float>* segnale = nullptr);
    double guadagno_agc() const { return guadagno_; }
    double aumento_fondo_db() const { return aumento_db_; }
    std::size_t crash_in_corso() const { return crash_.size(); }
    void azzera();

private:
    struct Crash {
        std::uint64_t inizio = 0, fine = 0;   // campioni
        double picco = 0.0;                   // ampiezza degli impulsi piu' forti, unita' d'uscita
        bool tweek = false;                   // VLF di notte: il «ping» che scende a 1,7 kHz
        int resta = 0;                        // campioni ancora dell'impulso in corso
        double ampiezza = 0.0;                // e il suo rms
    };
    struct Biquad {
        double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
        double operator()(double x);
    };
    void filtri();
    double impulso_crash(Crash& c, std::uint64_t k);

    ParametriScariche p_;
    Casuale caso_;
    std::uint64_t campione_ = 0;
    double resto_ = 0.0;
    bool notte_ = false;
    double aumento_db_ = 0.0;
    std::vector<Crash> crash_;
    Biquad passa_alto_, passa_basso_, passa_alto2_, passa_basso2_;
    double inviluppo_ = 0.0, guadagno_ = 1.0, potenza_media_ = 0.0;
    int spento_ = 0;                          // campioni del soppressore ancora da spegnere
    struct Glissando { std::uint64_t inizio = 0; double durata_s = 0.0, f0 = 0.0, f1 = 0.0, ampiezza = 0.0, fase = 0.0; bool whistler = false; };
    std::vector<Glissando> glissandi_;
};

}  // namespace m688
