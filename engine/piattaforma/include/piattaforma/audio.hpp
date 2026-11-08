#pragma once
// Uscita audio SDL3 (requisito 27, pezzo R1; dossier docs/ricerca/18_audio_reale.md §5.1): un flusso mono in virgola
// mobile verso il dispositivo predefinito, alimentato dal mixer (nucleo/mixer_audio.hpp). SDL sceglie il driver a
// run time (pipewire, pulseaudio, alsa, sndio): la build non ha bisogno di ALSA.
//
// Senza dispositivo o senza audio (SDL_INIT_AUDIO che fallisce, sessione senza suono) apri() restituisce nullptr e il
// gioco prosegue in silenzio: nessuna eccezione, nessun messaggio d'errore oltre a `motivo`.

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

struct SDL_AudioStream;

namespace m688 {

class UscitaAudio {
public:
    // Apre il dispositivo predefinito con un flusso mono float a `campionamento_hz`. nullptr se non si puo'; in `motivo`
    // (se dato) il perche'
    static std::unique_ptr<UscitaAudio> apri(int campionamento_hz = 48000, std::string* motivo = nullptr);
    ~UscitaAudio();
    UscitaAudio(const UscitaAudio&) = delete;
    UscitaAudio& operator=(const UscitaAudio&) = delete;

    // Accoda campioni (mono, -1..1) da suonare dopo quelli gia' in coda
    void accoda(const std::vector<float>& campioni);
    // Campioni ancora da suonare: il chiamante accoda quanto basta a tenerne in coda un poco (per esempio 0,1 s)
    std::size_t in_coda() const;
    // Quanti campioni accodare adesso per avere `obiettivo_s` secondi in coda (0 se ce ne sono gia' abbastanza)
    std::size_t da_accodare(double obiettivo_s) const;
    void volume(float v);                 // guadagno dell'uscita, 0..1 (oltre al volume generale del mixer)
    void pausa(bool p);                   // ferma o riprende il dispositivo (il menu, la finestra in secondo piano)
    void svuota();                        // butta la coda (cambio di scena)
    // I campioni li chiede il dispositivo, su un filo suo, quando gli servono: `produttore` ne da' n (mono, -1..1) e non deve
    // aspettare il disco. Niente chiamate a questa uscita tenendo un lucchetto che anche il produttore prende: SDL lo chiama col
    // lucchetto del flusso preso. Una volta sola, prima che serva il suono
    void tira(std::function<std::vector<float>(std::size_t)> produttore);
    // Per il registro dei tempi. Tirati: i campioni dati finora. In ritardo: quelli che il dispositivo ha aspettato oltre il
    // doppio del suo periodo. Filo: la chiamata piu' lunga dall'ultima lettura, in secondi
    std::uint64_t tirati() const { return tirati_.load(std::memory_order_relaxed); }
    std::uint64_t in_ritardo() const { return in_ritardo_.load(std::memory_order_relaxed); }
    double filo_massimo_s() { return static_cast<double>(filo_max_ns_.exchange(0, std::memory_order_relaxed)) * 1e-9; }
    int campioni_del_dispositivo() const;   // la grandezza del buffer del dispositivo, in campioni (0: non si sa)
    int campionamento_hz() const { return fs_; }
    const std::string& driver() const { return driver_; }

private:
    UscitaAudio() = default;
    SDL_AudioStream* flusso_ = nullptr;
    int fs_ = 48000;
    std::string driver_;
    std::function<std::vector<float>(std::size_t)> produttore_;
    std::atomic<std::uint64_t> tirati_{0}, in_ritardo_{0}, filo_max_ns_{0};
    std::uint64_t ultimo_ns_ = 0;
};

}  // namespace m688
