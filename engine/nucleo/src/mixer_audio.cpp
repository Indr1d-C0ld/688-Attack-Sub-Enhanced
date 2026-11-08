#include "nucleo/mixer_audio.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double PASSA_BASSO_SB = 0.45;   // frazione della frequenza di tenuta (STIMA)
constexpr double PI_MIXER = 3.14159265358979323846;

float quantizza(float x, int livelli) {   // senza segno su `livelli` gradini, poi di nuovo in -1..1
    const float u = std::clamp((x + 1.0f) * 0.5f, 0.0f, 1.0f);
    const float q = std::round(u * static_cast<float>(livelli - 1)) / static_cast<float>(livelli - 1);
    return q * 2.0f - 1.0f;
}

}  // namespace

const char* nome_resa(Resa r) {
    switch (r) {
        case Resa::piena: return "piena";
        case Resa::originale: return "originale";
        case Resa::sound_blaster: return "Sound Blaster";
    }
    return "?";
}

void Mixer::imposta(const ParametriMixer& p) {
    std::lock_guard blocco(mutex_);
    if (p.campionamento_hz != p_.campionamento_hz) azzera();   // le code sono alla frequenza d'uscita
    if (p.resa != p_.resa) resa_ = StatoResa{};
    p_ = p;
}

void Mixer::svuota() {
    std::lock_guard blocco(mutex_);
    azzera();
}

void Mixer::azzera() {
    for (auto& c : coda_) c.clear();
    ric_ = {};
    resa_ = StatoResa{};
}

void Mixer::aggiungi(Canale c, const std::vector<float>& campioni, int campionamento_hz) {
    if (campioni.empty() || campionamento_hz <= 0) return;
    std::lock_guard blocco(mutex_);
    const std::size_t k = static_cast<std::size_t>(c);
    auto& coda = coda_[k];
    auto& r = ric_[k];
    if (campionamento_hz == p_.campionamento_hz) {   // stessa frequenza: si copia
        coda.insert(coda.end(), campioni.begin(), campioni.end());
        r.precedente = campioni.back();
        r.iniziato = true;
        return;
    }
    // interpolazione lineare sulla sequenza s[-1] = l'ultimo del blocco prima, s[0..n-1] = questo blocco; la posizione
    // avanza di fs_in / fs_out e quello che resta oltre s[n-1] passa al blocco dopo
    const double passo = static_cast<double>(campionamento_hz) / p_.campionamento_hz;
    if (!r.iniziato) {
        r.precedente = campioni.front();
        r.fase = 0.0;
        r.iniziato = true;
    }
    const long n = static_cast<long>(campioni.size());
    const auto s = [&](long i) { return i < 0 ? r.precedente : campioni[static_cast<std::size_t>(i)]; };
    while (r.fase < static_cast<double>(n - 1)) {
        const long i = static_cast<long>(std::floor(r.fase));
        const double t = r.fase - static_cast<double>(i);
        coda.push_back(static_cast<float>(s(i) + (s(i + 1) - s(i)) * t));
        r.fase += passo;
    }
    r.fase -= static_cast<double>(n);
    r.precedente = campioni.back();
}

float Mixer::rendi(float x) {
    switch (p_.resa) {
        case Resa::piena: return x;
        case Resa::originale:
        case Resa::sound_blaster: {
            const int fs_h = p_.resa == Resa::originale ? p_.originale_hz : p_.sound_blaster_hz;
            const int livelli = p_.resa == Resa::originale ? 16 : 256;
            if (!resa_.a_riposo) {   // la scheda accesa e muta: il livello di riposo, che l'alternata porta a 0
                resa_.tenuto = resa_.passa_basso = resa_.ac_x = quantizza(0.0f, livelli);
                resa_.a_riposo = true;
            }
            // campionamento e tenuta alla frequenza della scheda, senza filtro (l'aliasing e' il timbro d'epoca)
            resa_.fase += static_cast<double>(fs_h) / p_.campionamento_hz;
            if (resa_.fase >= 1.0) {
                resa_.fase -= std::floor(resa_.fase);
                resa_.tenuto = quantizza(x, livelli);
            }
            if (p_.resa == Resa::originale) return alternata(resa_.tenuto);
            // Sound Blaster: un passa basso a un polo sull'uscita
            const double fc = PASSA_BASSO_SB * fs_h;
            const float a = static_cast<float>(1.0 - std::exp(-2.0 * PI_MIXER * fc / p_.campionamento_hz));
            resa_.passa_basso += a * (resa_.tenuto - resa_.passa_basso);
            return alternata(resa_.passa_basso);
        }
    }
    return x;
}

// Con un numero pari di livelli nessun gradino e' lo zero: il silenzio cade sul livello di riposo, +1/15 (o +1/255)
// sopra. L'accoppiamento in alternata lo toglie; parte dal riposo (rendi), cosi' il silenzio iniziale esce 0 esatto
float Mixer::alternata(float x) {
    if (!p_.accoppiamento_alternata) return x;
    const float r = static_cast<float>(std::exp(-2.0 * PI_MIXER * p_.taglio_alternata_hz / p_.campionamento_hz));
    resa_.ac_y = x - resa_.ac_x + r * resa_.ac_y;
    resa_.ac_x = x;
    return resa_.ac_y;
}

std::vector<float> Mixer::produci(std::size_t n) {
    std::vector<float> out(n, 0.0f), fm(n, 0.0f);
    std::lock_guard blocco(mutex_);
    for (std::size_t c = 0; c < CANALI_AUDIO; ++c) {
        auto& coda = coda_[c];
        auto& dove = c == static_cast<std::size_t>(Canale::musica) ? fm : out;
        const float v = p_.volume[c] * p_.generale;
        const std::size_t m = std::min(n, coda.size());
        for (std::size_t i = 0; i < m; ++i) dove[i] += v * coda[i];
        coda.erase(coda.begin(), coda.begin() + static_cast<long>(m));
    }
    // i campioni con la resa d'epoca; la musica FM dopo, analogica (sulle schede l'OPL non passava dal DAC)
    for (std::size_t i = 0; i < n; ++i) out[i] = std::clamp(rendi(std::clamp(out[i], -1.0f, 1.0f)) + fm[i], -1.0f, 1.0f);
    return out;
}

std::vector<std::int16_t> Mixer::a_16_bit(const std::vector<float>& x) {
    std::vector<std::int16_t> out(x.size());
    for (std::size_t i = 0; i < x.size(); ++i)
        out[i] = static_cast<std::int16_t>(std::lround(std::clamp(x[i], -1.0f, 1.0f) * 32767.0f));
    return out;
}

}  // namespace m688
