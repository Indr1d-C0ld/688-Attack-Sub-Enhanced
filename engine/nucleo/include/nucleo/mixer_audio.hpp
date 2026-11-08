#pragma once
// Mixer dell'audio (requisito 27, pezzo R1; dossier docs/ricerca/18_audio_reale.md §5): tre canali (la cuffia del
// sonarista, l'ambiente di bordo, i suoni dell'interfaccia), ciascuno ricampionato alla frequenza d'uscita, con i volumi,
// e la resa finale:
// - piena: virgola mobile, nessuna alterazione;
// - originale: come i campioni dell'EXE del 1989 (PCM a 4 bit senza segno a circa 5,5 kHz, riprodotti dall'altoparlante,
//   dal Tandy o dal Covox senza filtro: docs/01_analisi_tecnica_originale.md §2.6): campionamento e tenuta a 5,5 kHz
//   senza filtro anti-aliasing e 16 livelli;
// - Sound Blaster: 8 bit senza segno (256 livelli) a 22.050 o 11.025 Hz con tenuta (la SB 1.0 non aveva un filtro
//   anti-aliasing), e un passa basso leggero sull'uscita (STIMA: il filtro della scheda non e' verificato).
//
// Il modulo e' puro e deterministico: chi produce i suoni aggiunge blocchi ai canali, chi li riproduce chiede n campioni
// e riceve sempre gli stessi a parita' d'ingresso. Il dispositivo (piattaforma/audio.hpp) e' a parte: senza dispositivo
// il mixer si puo' anche non usare, e nelle prove gira senza audio.

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <vector>

namespace m688 {

enum class Resa : std::uint8_t { piena, originale, sound_blaster };
const char* nome_resa(Resa r);

// musica (roadmap 38, 31/10/2026): l'FM dal vivo (nucleo/musica_fm); si somma dopo la resa, come sulle schede d'epoca,
// dove l'OPL usciva analogico e non dal DAC a 8 bit dei campioni. effetti: allarmi e suoni di bordo di data/suoni/effetti,
// gia' sommati fra loro da chi li suona (la regia di trailer-1), in un flusso continuo: PCM, passa dalla resa
enum class Canale : std::uint8_t { cuffia, bordo, interfaccia, musica, effetti };
inline constexpr std::size_t CANALI_AUDIO = 5;

struct ParametriMixer {
    int campionamento_hz = 48000;            // frequenza d'uscita
    Resa resa = Resa::piena;
    std::array<float, CANALI_AUDIO> volume{1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float generale = 1.0f;
    int sound_blaster_hz = 22050;            // 22.050 o 11.025
    int originale_hz = 5500;                 // la frequenza tipica dei campioni dell'EXE (ping 5500 Hz)
    // rese d'epoca: l'uscita delle schede era accoppiata in alternata, e la continua del livello di riposo (8 su 16:
    // il silenzio a +1/15) spariva. Un passa alto a un polo a `taglio_alternata_hz` (STIMA), partito dal riposo
    bool accoppiamento_alternata = true;
    double taglio_alternata_hz = 10.0;
};

// I produttori stanno sul thread dei fotogrammi, produci() sul filo dell'audio: ogni metodo prende il lucchetto. parametri() la
// legge e imposta() la scrive solo il thread dei fotogrammi
class Mixer {
public:
    explicit Mixer(const ParametriMixer& p = {}) : p_(p) {}
    void imposta(const ParametriMixer& p);
    const ParametriMixer& parametri() const { return p_; }

    // Accoda al canale `campioni` (mono, -1..1) alla frequenza `campionamento_hz`: si ricampionano alla frequenza
    // d'uscita, continui fra un blocco e l'altro (interpolazione lineare con la fase che resta)
    void aggiungi(Canale c, const std::vector<float>& campioni, int campionamento_hz);
    // campioni in attesa nel canale, alla frequenza d'uscita
    std::size_t in_attesa(Canale c) const {
        std::lock_guard blocco(mutex_);
        return coda_[static_cast<std::size_t>(c)].size();
    }
    // Produce n campioni d'uscita: la somma dei canali per i loro volumi, con la resa (la musica dopo, senza resa); i
    // canali vuoti danno silenzio
    std::vector<float> produci(std::size_t n);
    // lo stesso blocco in 16 bit (per un WAV o un dispositivo a interi)
    static std::vector<std::int16_t> a_16_bit(const std::vector<float>& x);
    void svuota();

private:
    struct Ricampionatore {
        double fase = 0.0;       // posizione dentro l'ingresso, in campioni d'ingresso
        float precedente = 0.0f;
        bool iniziato = false;
    };
    struct StatoResa {
        double fase = 1.0;       // per la tenuta: quando arriva a 1 si prende un campione nuovo
        float tenuto = 0.0f;
        float passa_basso = 0.0f;
        float ac_x = 0.0f, ac_y = 0.0f;   // l'accoppiamento in alternata: ingresso e uscita precedenti
        bool a_riposo = false;            // tenuta, passa basso e accoppiamento partiti dal livello di riposo
    };
    void azzera();
    float rendi(float x);
    float alternata(float x);

    mutable std::mutex mutex_;
    ParametriMixer p_;
    std::array<std::deque<float>, CANALI_AUDIO> coda_;
    std::array<Ricampionatore, CANALI_AUDIO> ric_{};
    StatoResa resa_;
};

}  // namespace m688
