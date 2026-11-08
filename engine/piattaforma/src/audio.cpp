#include "piattaforma/audio.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

namespace m688 {

std::unique_ptr<UscitaAudio> UscitaAudio::apri(int campionamento_hz, std::string* motivo) {
    const auto no = [&](const char* perche) -> std::unique_ptr<UscitaAudio> {
        if (motivo) *motivo = std::string(perche) + ": " + SDL_GetError();
        return nullptr;
    };
    if (!SDL_WasInit(SDL_INIT_AUDIO) && !SDL_InitSubSystem(SDL_INIT_AUDIO)) return no("audio non inizializzato");
    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = campionamento_hz;
    SDL_AudioStream* f = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!f) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return no("nessun dispositivo audio");
    }
    std::unique_ptr<UscitaAudio> u(new UscitaAudio());
    u->flusso_ = f;
    u->fs_ = campionamento_hz;
    if (const char* d = SDL_GetCurrentAudioDriver()) u->driver_ = d;
    SDL_ResumeAudioStreamDevice(f);
    return u;
}

UscitaAudio::~UscitaAudio() {
    if (flusso_) {
        SDL_DestroyAudioStream(flusso_);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

void UscitaAudio::accoda(const std::vector<float>& campioni) {
    if (!flusso_ || campioni.empty()) return;
    SDL_PutAudioStreamData(flusso_, campioni.data(), static_cast<int>(campioni.size() * sizeof(float)));
}

std::size_t UscitaAudio::in_coda() const {
    if (!flusso_) return 0;
    const int byte = SDL_GetAudioStreamQueued(flusso_);
    return byte > 0 ? static_cast<std::size_t>(byte) / sizeof(float) : 0;
}

std::size_t UscitaAudio::da_accodare(double obiettivo_s) const {
    const auto obiettivo = static_cast<std::size_t>(std::max(0.0, std::round(obiettivo_s * fs_)));
    const std::size_t c = in_coda();
    return c >= obiettivo ? 0 : obiettivo - c;
}

void UscitaAudio::volume(float v) {
    if (flusso_) SDL_SetAudioStreamGain(flusso_, std::clamp(v, 0.0f, 1.0f));
}

void UscitaAudio::pausa(bool p) {
    if (!flusso_) return;
    if (p) SDL_PauseAudioStreamDevice(flusso_);
    else SDL_ResumeAudioStreamDevice(flusso_);
}

void UscitaAudio::svuota() {
    if (flusso_) SDL_ClearAudioStream(flusso_);
}

void UscitaAudio::tira(std::function<std::vector<float>(std::size_t)> produttore) {
    if (!flusso_) return;
    produttore_ = std::move(produttore);
    SDL_SetAudioStreamGetCallback(flusso_, [](void* dato, SDL_AudioStream* flusso, int mancano, int) {
        if (mancano <= 0) return;
        auto* u = static_cast<UscitaAudio*>(dato);
        const std::uint64_t da = SDL_GetTicksNS();
        const std::size_t n = static_cast<std::size_t>(mancano) / sizeof(float);
        const std::vector<float> x = u->produttore_(n);
        if (!x.empty()) SDL_PutAudioStreamData(flusso, x.data(), static_cast<int>(x.size() * sizeof(float)));
        const std::uint64_t a = SDL_GetTicksNS();
        u->tirati_.fetch_add(x.size(), std::memory_order_relaxed);
        const std::uint64_t durata = a - da;
        if (durata > u->filo_max_ns_.load(std::memory_order_relaxed)) u->filo_max_ns_.store(durata, std::memory_order_relaxed);
        const std::uint64_t periodo = static_cast<std::uint64_t>(n) * 1000000000ull / static_cast<std::uint64_t>(u->fs_);
        if (u->ultimo_ns_ && da - u->ultimo_ns_ > 2 * periodo)
            u->in_ritardo_.fetch_add((da - u->ultimo_ns_ - 2 * periodo) * static_cast<std::uint64_t>(u->fs_) / 1000000000ull, std::memory_order_relaxed);
        u->ultimo_ns_ = da;
    }, this);
}

int UscitaAudio::campioni_del_dispositivo() const {
    if (!flusso_) return 0;
    SDL_AudioSpec spec{};
    int campioni = 0;
    return SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(flusso_), &spec, &campioni) ? campioni : 0;
}

}  // namespace m688
