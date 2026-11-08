#include "nucleo/audio_meteo.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double PI_M = 3.14159265358979323846;
constexpr double QUOTA_LAMPO_M = 1.0;            // il colpo nasce alla superficie (taratura in sorgenti_asw.py)
constexpr double SIGMA_INTERVALLO = 0.7;         // lognormale degli intervalli fra i colpi (21e §5.1, STIMA)
constexpr double CALO_COLPI_DB = 6.0, SIGMA_COLPI_DB = 3.0;

double normale(Casuale& c) {   // Box-Muller
    const double u1 = std::max(c.uniforme(), 1e-300), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI_M * u2);
}
double ampiezza_db(double db) { return std::pow(10.0, db / 20.0); }
// il crepitio lontano: impulsi lognormali (sigma 6 dB) sopra il fondo del ricevitore, con la mediana tarata sul Vd
// delle tabelle CCIR 322 (21e §4.2: 5-9 dB in HF con la banda SSB, 11-16 in VLF); di giorno lo strato D assorbe
// (STIMA). Si prova con tools/re/prove/taratura/scariche.cpp
double CREPITIO_DB[2][2] = {{15.0, 24.0}, {30.0, 36.0}};   // [hf, vlf][giorno, notte]; in VLF Vd ~7, sotto le tabelle

// 21e §5.5: il crash rispetto al fondo, secondo la distanza
double picco_crash_db(double distanza_m) {
    const double km = std::max(distanza_m / 1000.0, 1.0);
    if (km <= 50.0) return 35.0;
    if (km <= 300.0) return 35.0 - 15.0 * std::log10(km / 50.0) / std::log10(6.0);
    if (km <= 1000.0) return 20.0 - 13.0 * std::log10(km / 300.0) / std::log10(1000.0 / 300.0);
    return 7.0;
}

}  // namespace

std::vector<Transitorio> transitori_lampo(const Lampo& l, int id_base, std::uint64_t seme, double intervallo_s) {
    std::vector<Transitorio> out;
    if (!l.nube_mare || l.colpi < 1) return out;
    Casuale c(seme ^ (0x9e3779b97f4a7c15ull * static_cast<std::uint64_t>(id_base + 1)));
    double t = l.t;
    for (int k = 0; k < l.colpi; ++k) {
        Transitorio x;
        if (k > 0) {
            t += intervallo_s * std::exp(SIGMA_INTERVALLO * normale(c));
            x.guadagno_db = std::clamp(-CALO_COLPI_DB + SIGMA_COLPI_DB * normale(c), -15.0, 0.0);
        }
        x.id = id_base + k;
        x.tipo = "fulmine_mare";
        x.pos = l.pos;
        x.quota_m = QUOTA_LAMPO_M;
        x.t0 = t;
        x.seme = c.prossimo() | 1u;
        out.push_back(x);
    }
    return out;
}

const char* nome_banda_radio(BandaRadio b) {
    switch (b) {
        case BandaRadio::hf_ssb: return "HF SSB";
        case BandaRadio::hf_am: return "HF AM";
        case BandaRadio::vlf: return "VLF";
    }
    return "?";
}

double ScaricheRadio::Biquad::operator()(double x) {
    const double y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return y;
}

ScaricheRadio::ScaricheRadio(const ParametriScariche& p) : p_(p), caso_(p.seme) { filtri(); }

void ScaricheRadio::imposta(const ParametriScariche& p) {
    const bool cambia_banda = p.banda != p_.banda || p.campionamento_hz != p_.campionamento_hz;
    const std::uint64_t seme = p_.seme;
    p_ = p;
    p_.seme = seme;
    if (cambia_banda) filtri();
}

void ScaricheRadio::azzera() {
    caso_ = Casuale(p_.seme);
    campione_ = 0;
    resto_ = 0.0;
    crash_.clear();
    glissandi_.clear();
    inviluppo_ = potenza_media_ = 0.0;
    guadagno_ = 1.0;
    spento_ = 0;
    filtri();
}

void ScaricheRadio::filtri() {
    // passa banda del demodulatore: due passa alto e due passa basso del secondo ordine (Butterworth, RBJ)
    const double fs = p_.campionamento_hz;
    double lo = 300.0, hi = 2700.0;
    if (p_.banda == BandaRadio::hf_am) { lo = 200.0; hi = 4000.0; }
    else if (p_.banda == BandaRadio::vlf) { lo = 300.0; hi = 11000.0; }
    hi = std::min(hi, 0.45 * fs);
    const auto biquad = [&](double f, bool alto) {
        const double w = 2.0 * PI_M * f / fs, cw = std::cos(w), al = std::sin(w) / (2.0 * std::sqrt(0.5));
        const double a0 = 1.0 + al;
        Biquad b;
        if (alto) { b.b0 = (1.0 + cw) / 2.0; b.b1 = -(1.0 + cw); b.b2 = (1.0 + cw) / 2.0; }
        else { b.b0 = (1.0 - cw) / 2.0; b.b1 = 1.0 - cw; b.b2 = (1.0 - cw) / 2.0; }
        b.b0 /= a0; b.b1 /= a0; b.b2 /= a0;
        b.a1 = -2.0 * cw / a0;
        b.a2 = (1.0 - al) / a0;
        return b;
    };
    passa_alto_ = passa_alto2_ = biquad(lo, true);
    passa_basso_ = passa_basso2_ = biquad(hi, false);
}

void ScaricheRadio::ambiente(const std::vector<CellaTemporale>& celle, Vec2 pos, bool notte) {
    notte_ = notte;
    double vicina_km = 1e9;
    for (const CellaTemporale& c : celle)
        vicina_km = std::min(vicina_km, std::max(0.0, distanza(c.pos, pos) - c.raggio_m) / 1000.0);
    aumento_db_ = vicina_km <= 50.0 ? 10.0 : vicina_km < 300.0 ? 10.0 * (300.0 - vicina_km) / 250.0 : 0.0;
}

void ScaricheRadio::lampo(const Lampo& l, Vec2 pos) {
    const double d = distanza(l.pos, pos);
    if (d > 3.0e6) return;
    Crash c;
    c.inizio = campione_;
    // durata lognormale di mediana 0,5 s (Aiya 1977, 21e §4.3); i lampi nella nube un po' piu' deboli (STIMA)
    const double durata = std::clamp(0.5 * std::exp(0.5 * normale(caso_)), 0.1, 2.0);
    c.fine = c.inizio + static_cast<std::uint64_t>(durata * p_.campionamento_hz);
    c.picco = ampiezza_db(p_.fondo_dbfs + picco_crash_db(d) - (l.nube_mare ? 0.0 : 3.0));
    c.tweek = p_.banda == BandaRadio::vlf && notte_ && caso_.uniforme() < 0.5;
    crash_.push_back(c);
    if (c.tweek) {   // 21e §4.4 e §5.5: da 8-10 kHz a 1,7 kHz in 20-40 ms, poi la coda
        Glissando g;
        g.inizio = c.inizio;
        g.durata_s = caso_.uniforme(0.02, 0.04);
        g.f0 = caso_.uniforme(8000.0, 10000.0);
        g.f1 = 1700.0;
        g.ampiezza = 0.3 * c.picco;
        glissandi_.push_back(g);
    }
}

double ScaricheRadio::impulso_crash(Crash& c, std::uint64_t k) {
    // dentro il crash ~150 impulsi al secondo di ~1 ms, lognormali (sigma 6 dB) col 10% entro 6 dB dal picco (Aiya):
    // l'ampiezza e' l'rms dell'impulso rispetto al fondo
    const double fs = p_.campionamento_hz;
    if (c.resta > 0) {
        --c.resta;
        return c.ampiezza * normale(caso_);
    }
    if (caso_.uniforme() >= 150.0 / fs) return 0.0;
    const double t = static_cast<double>(k - c.inizio) / fs, durata = static_cast<double>(c.fine - c.inizio) / fs;
    const double inviluppo = std::min(1.0, t / 0.02) * (t > 0.7 * durata ? std::max(0.0, (durata - t) / (0.3 * durata)) : 1.0);
    c.ampiezza = inviluppo * c.picco * ampiezza_db(-13.7 + 6.0 * normale(caso_));
    c.resta = static_cast<int>(caso_.uniforme(0.5e-3, 1.5e-3) * fs);
    return c.ampiezza * normale(caso_);
}

std::vector<float> ScaricheRadio::passo(double dt_s, const std::vector<float>* segnale) {
    const double fs = p_.campionamento_hz;
    const double esatti = std::max(0.0, dt_s) * fs + resto_;
    const std::size_t n = static_cast<std::size_t>(std::floor(esatti));
    resto_ = esatti - static_cast<double>(n);
    std::vector<float> out(n);
    double lo = 300.0, hi = 2700.0;
    if (p_.banda == BandaRadio::hf_am) { lo = 200.0; hi = 4000.0; }
    else if (p_.banda == BandaRadio::vlf) { lo = 300.0; hi = 11000.0; }
    hi = std::min(hi, 0.45 * fs);
    const double frazione = 2.0 * (hi - lo) / fs;           // la parte di banda che il filtro lascia al rumore bianco
    const double fondo_base = ampiezza_db(p_.fondo_dbfs);   // rms in uscita senza temporali
    const double g_fondo = fondo_base * ampiezza_db(aumento_db_) / std::sqrt(frazione);
    const double impulso = 1.0 / frazione;                   // un delta di 1 da' un picco ~ 1 x rms del fondo base
    // crepitio lontano (scariche di tutto il mondo via ionosfera, 21e §5.5): impulsi al secondo
    const double crepitio = p_.banda == BandaRadio::vlf ? 100.0 : notte_ ? 40.0 : 6.0;
    // crash di temporali lontani fuori dalla missione, al minuto (STIMA)
    const double lontani = (p_.banda == BandaRadio::vlf ? 4.0 : 2.0) * (notte_ ? 1.0 : 0.25) / 60.0;
    const double whistler = p_.banda == BandaRadio::vlf && notte_ ? 5.0 / 3600.0 : 0.0;   // all'ora, medie latitudini
    const double a_att = 1.0 - std::exp(-1.0 / (0.001 * fs)), a_ril = 1.0 - std::exp(-1.0 / (0.6 * fs));
    const double a_media = 1.0 - std::exp(-1.0 / (0.05 * fs));
    for (std::size_t i = 0; i < n; ++i) {
        const std::uint64_t k = campione_ + i;
        double u = g_fondo * normale(caso_);
        if (caso_.uniforme() < crepitio / fs)
            u += impulso * fondo_base * ampiezza_db(CREPITIO_DB[p_.banda == BandaRadio::vlf][notte_] + 6.0 * normale(caso_));
        if (caso_.uniforme() < lontani / fs) {
            Crash c;
            c.inizio = k;
            c.fine = k + static_cast<std::uint64_t>(std::clamp(0.5 * std::exp(0.5 * normale(caso_)), 0.1, 2.0) * fs);
            c.picco = ampiezza_db(p_.fondo_dbfs + picco_crash_db(caso_.uniforme(1.0e6, 3.0e6)));
            crash_.push_back(c);
        }
        for (Crash& c : crash_)
            if (k >= c.inizio && k < c.fine) u += impulso_crash(c, k) / std::sqrt(frazione);
        double y = passa_basso2_(passa_basso_(passa_alto2_(passa_alto_(u))));
        // tweek e whistler: toni che scendono, gia' nella banda
        if (whistler > 0.0 && caso_.uniforme() < whistler / fs) {
            Glissando g;
            g.inizio = k;
            g.durata_s = caso_.uniforme(0.5, 2.0);
            g.f0 = 6000.0;
            g.f1 = 500.0;
            g.ampiezza = 2.0 * fondo_base;
            g.whistler = true;
            glissandi_.push_back(g);
        }
        for (Glissando& g : glissandi_) {
            if (k < g.inizio) continue;
            const double t = static_cast<double>(k - g.inizio) / fs;
            double f = g.f1, a = 0.0;
            if (g.whistler) {   // dispersione: t proporzionale a 1/sqrt(f)
                const double d = g.durata_s / (1.0 / std::sqrt(g.f1) - 1.0 / std::sqrt(g.f0));
                const double x = t / d + 1.0 / std::sqrt(g.f0);
                f = 1.0 / (x * x);
                a = g.ampiezza * std::min(1.0, t / 0.05) * (t < g.durata_s ? 1.0 : 0.0);
            } else {            // tweek: discesa esponenziale, poi la coda risonante a 1,7 kHz con le armoniche
                f = t < g.durata_s ? g.f0 * std::pow(g.f1 / g.f0, t / g.durata_s) : g.f1;
                a = g.ampiezza * (t < g.durata_s ? 1.0 : std::exp(-(t - g.durata_s) / 0.04));
            }
            g.fase += 2.0 * PI_M * f / fs;
            y += a * std::sin(g.fase);
            if (!g.whistler && t >= g.durata_s) y += a * ampiezza_db(-10.0) * (std::sin(2.0 * g.fase) + std::sin(3.0 * g.fase));
        }
        // il soppressore: spegne per 1,5 ms quello che supera di 12 dB il fondo medio
        potenza_media_ += a_media * (y * y - potenza_media_);
        if (p_.soppressore) {
            const double soglia = 4.0 * std::max(std::sqrt(potenza_media_), fondo_base);
            if (std::fabs(y) > soglia) spento_ = static_cast<int>(0.0015 * fs);
        }
        double x = y + (segnale && i < segnale->size() ? static_cast<double>((*segnale)[i]) : 0.0);
        if (spento_ > 0) { --spento_; x = 0.0; }
        // AGC: attacco 1 ms, rilascio 0,6 s (21e §5.5, STIMA)
        const double ax = std::fabs(x);
        inviluppo_ += (ax > inviluppo_ ? a_att : a_ril) * (ax - inviluppo_);
        guadagno_ = std::min(1.0, p_.riferimento_agc / std::max(inviluppo_, 1e-9));
        out[i] = static_cast<float>(std::tanh(x * guadagno_));
    }
    campione_ += n;
    crash_.erase(std::remove_if(crash_.begin(), crash_.end(), [&](const Crash& c) { return c.fine <= campione_; }), crash_.end());
    glissandi_.erase(std::remove_if(glissandi_.begin(), glissandi_.end(), [&](const Glissando& g) {
        return static_cast<double>(campione_ - g.inizio) / fs > g.durata_s + (g.whistler ? 0.0 : 0.3);
    }), glissandi_.end());
    return out;
}

}  // namespace m688
