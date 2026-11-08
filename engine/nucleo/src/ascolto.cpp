#include "nucleo/ascolto.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>

namespace m688 {

namespace {

constexpr double SOGLIA_UDIBILE_HZ = 50.0;   // sotto, le righe si traslano di ottave (opzione)
constexpr double RIGHE_MIN_HZ = 5.0;         // la riga piu' bassa che si trasla
constexpr double BATTITO = 0.4;              // profondita' della modulazione d'elica per chi cava (STIMA)
constexpr double PICCO_MAX = 0.98;           // sotto il fondo scala

using Complesso = std::complex<double>;

// FFT iterativa radix-2 (inversa se `inversa`), in place; n potenza di due
void fft(std::vector<Complesso>& a, bool inversa) {
    const std::size_t n = a.size();
    for (std::size_t i = 1, j = 0; i < n; ++i) {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }
    for (std::size_t len = 2; len <= n; len <<= 1) {
        const double ang = 2.0 * PI / static_cast<double>(len) * (inversa ? 1.0 : -1.0);
        const Complesso w(std::cos(ang), std::sin(ang));
        for (std::size_t i = 0; i < n; i += len) {
            Complesso wk(1.0, 0.0);
            for (std::size_t j = 0; j < len / 2; ++j) {
                const Complesso u = a[i + j], v = a[i + j + len / 2] * wk;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                wk *= w;
            }
        }
    }
    if (inversa)
        for (auto& x : a) x /= static_cast<double>(n);
}

double potenza(double db) { return std::pow(10.0, db / 10.0); }

// la banda d'uscita utile per una tessitura (sotto Nyquist, con margine)
double f_hi_out_campione(double fs) { return 0.45 * fs; }

// velocita' radiale (m/s, positiva in avvicinamento), come nello spettro del fascio
double avvicinamento(const Sorgente& s, const Ricevitore& r) {
    const double d = distanza(r.pos, s.pos);
    if (d < 1.0) return 0.0;
    const Vec2 u{(s.pos.x - r.pos.x) / d, (s.pos.y - r.pos.y) / d};
    const double rs = radianti(s.rotta), rr = radianti(r.rotta);
    return r.velocita_kn * NODO * (std::sin(rr) * u.x + std::cos(rr) * u.y) - s.velocita_kn * NODO * (std::sin(rs) * u.x + std::cos(rs) * u.y);
}

void scrivi16(std::ofstream& f, std::uint16_t v) { f.put(static_cast<char>(v & 0xff)); f.put(static_cast<char>(v >> 8)); }
void scrivi32(std::ofstream& f, std::uint32_t v) { scrivi16(f, static_cast<std::uint16_t>(v & 0xffff)); scrivi16(f, static_cast<std::uint16_t>(v >> 16)); }

}  // namespace

Suono sintetizza(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio,
                 const ParametriAscolto& p) {
    Suono out;
    out.campionamento_hz = p.campionamento_hz;
    const double fs = p.campionamento_hz;
    const std::size_t n = static_cast<std::size_t>(std::max(0.0, std::round(p.durata_s * fs)));
    if (n == 0 || !r.sensore || p.f_max_hz <= p.f_min_hz) return out;
    const double k = std::max(p.accelerazione, 1e-3);
    const double f_hi_out = std::min(p.f_max_hz, 0.49 * fs);
    // lo spettro nel dominio della sorgente (le frequenze ascoltate divise per l'accelerazione)
    const double lo = p.f_min_hz / k, hi = f_hi_out / k;
    if (hi <= lo) return out;
    const double bin = std::max(0.05, (hi - lo) / 2000.0);
    const Spettro sp = spettro_continuo(sorgenti, r, amb, rilevamento_fascio, lo, hi, bin, true);   // le righe a parte
    const Sensore& sn = *r.sensore;
    const double v = r.sul_fondo ? 0.0 : r.velocita_kn;
    const auto rumore = [&](double f) {   // rumore nel fascio, spettrale, meno il guadagno d'antenna (dB re 1 uPa^2/Hz)
        return 10.0 * std::log10(potenza(rumore_ambiente(f, amb)) + potenza(rumore_proprio_ricevitore(f, r, v, false))) - indice_direttivita(sn, f);
    };
    const auto snr = [&](double f) {
        if (sp.snr_db.empty()) return 0.0;
        const double x = std::clamp((f - sp.f0_hz) / sp.df_hz - 0.5, 0.0, static_cast<double>(sp.snr_db.size() - 1));
        const std::size_t i = static_cast<std::size_t>(x);
        const std::size_t j = std::min(i + 1, sp.snr_db.size() - 1);
        const double t = x - static_cast<double>(i);
        return sp.snr_db[i] + t * (sp.snr_db[j] - sp.snr_db[i]);
    };
    // rumore colorato: sintesi in frequenza con fasi a caso, simmetria hermitiana
    std::size_t m = 1;
    while (m < n) m <<= 1;
    const double df = fs / static_cast<double>(m);
    Casuale caso(p.seme);
    std::vector<Complesso> x(m, Complesso(0.0, 0.0));
    double varianza_fondo = 0.0;   // del solo rumore, per la scala
    for (std::size_t j = 1; j < m / 2; ++j) {
        const double f_out = static_cast<double>(j) * df;
        if (f_out < p.f_min_hz || f_out > f_hi_out) continue;
        const double f = f_out / k;
        const double fondo = potenza(rumore(f)) / k;              // per Hz d'uscita
        const double totale = fondo * potenza(snr(f));
        varianza_fondo += fondo * df;
        const double a = std::sqrt(totale * df * static_cast<double>(m) * static_cast<double>(m) / 2.0);
        const double fase = 2.0 * PI * caso.uniforme();
        x[j] = std::polar(a, fase);
        x[m - j] = std::conj(x[j]);
    }
    fft(x, true);
    std::vector<double> y(n);
    for (std::size_t i = 0; i < n; ++i) y[i] = x[i].real();
    // battito dell'elica: il primo che cava nel fascio, alla frequenza di pala letta dal DEMON
    for (const Sorgente& s : sorgenti) {
        if (!s.firma) continue;
        if (const auto d = leggi_demon(demon(s, r, amb))) {
            const double fp = d->pala_hz * k;
            for (std::size_t i = 0; i < n; ++i) y[i] *= 1.0 + BATTITO * std::sin(2.0 * PI * fp * static_cast<double>(i) / fs);
            break;
        }
    }
    // le righe: sinusoidi alla frequenza ricevuta, potenza = rumore nel bin x S/N della riga
    // con la traslazione d'ottava anche le righe sotto la banda (da 5 Hz)
    const double lo_righe = p.ottava_su ? std::min(lo, RIGHE_MIN_HZ) : lo;
    for (const RigaRicevuta& rr : righe_ricevute(sorgenti, r, amb, rilevamento_fascio, lo_righe, hi, bin, true)) {
        if (rr.snr_db <= 0.0) continue;
        double f_out = rr.hz * k;
        if (p.ottava_su)
            while (f_out < SOGLIA_UDIBILE_HZ) f_out *= 2.0;
        if (f_out < p.f_min_hz || f_out > f_hi_out) continue;
        const double pot = potenza(rumore(rr.hz)) * bin * potenza(rr.snr_db);
        const double a = std::sqrt(2.0 * pot), fase = 2.0 * PI * caso.uniforme();
        for (std::size_t i = 0; i < n; ++i) y[i] += a * std::sin(2.0 * PI * f_out * static_cast<double>(i) / fs + fase);
    }
    // scala: il fondo a rumore_dbfs (rms), senza saturare
    double g = varianza_fondo > 0.0 ? std::pow(10.0, p.rumore_dbfs / 20.0) / std::sqrt(varianza_fondo) : 1.0;
    double picco = 0.0;
    for (double z : y) picco = std::max(picco, std::fabs(z) * g);
    if (picco > PICCO_MAX) {
        out.guadagno_db = 20.0 * std::log10(PICCO_MAX / picco);
        g *= PICCO_MAX / picco;
    }
    out.campioni.resize(n);
    for (std::size_t i = 0; i < n; ++i)
        out.campioni[i] = static_cast<std::int16_t>(std::lround(std::clamp(y[i] * g, -1.0, 1.0) * 32767.0));
    return out;
}

// --- motore di sintesi a blocchi ---

namespace {

constexpr double RAMPA_RIGHE_S = 0.03;      // costante di tempo delle ampiezze di righe e battito
constexpr double SCALA_FOTOGRAMMA = 0.2;    // la scala del fondo segue per fotogrammi (circa mezzo secondo)
constexpr double LIMITE_RIPRESA = 1e-5;     // ripresa del limitatore per campione
constexpr double AGGIORNAMENTO_S = 0.25;    // la scena (spettro, righe, DEMON) si rilegge 4 volte al secondo (STIMA)

double rumore_fascio_db(double f, const Ricevitore& r, const Ambiente& amb) {
    const Sensore& sn = *r.sensore;
    const double v = r.sul_fondo ? 0.0 : r.velocita_kn;
    return 10.0 * std::log10(potenza(rumore_ambiente(f, amb)) + potenza(rumore_proprio_ricevitore(f, r, v, false))) - indice_direttivita(sn, f);
}

}  // namespace

GeneratoreSpettrale::GeneratoreSpettrale(int campionamento_hz, std::uint64_t seme, std::size_t fotogramma)
    : fs_(campionamento_hz), seme_(seme), caso_(seme), n_(fotogramma) {}

void GeneratoreSpettrale::azzera() {
    caso_ = Casuale(seme_);
    ampiezze_.clear();
    coda_.clear();
    scale_coda_.clear();
    sovrapposto_.clear();
    righe_.clear();
    scala_bersaglio_ = scala_ = 0.0;
    limite_ = 1.0;
    battito_hz_ = battito_ = battito_bersaglio_ = fase_battito_ = 0.0;
}

void GeneratoreSpettrale::imposta_rumore(const std::vector<double>& densita, double scala) {
    // ampiezza del bin per l'IFFT di n_ punti: la varianza di un fotogramma e' la somma di densita' x df
    const double df = df_hz();
    ampiezze_.assign(n_ / 2, 0.0);
    for (std::size_t j = 1; j < n_ / 2 && j < densita.size(); ++j)
        if (densita[j] > 0.0) ampiezze_[j] = std::sqrt(densita[j] * df * static_cast<double>(n_) * static_cast<double>(n_) / 2.0);
    scala_bersaglio_ = scala;
}

void GeneratoreSpettrale::imposta_righe(const std::vector<RigaSintesi>& righe) {
    for (auto& [chiave, x] : righe_) x.bersaglio = 0.0;
    for (const RigaSintesi& r : righe) {
        Riga& x = righe_[r.chiave];
        x.hz = r.hz;
        x.bersaglio = r.ampiezza;
    }
}

void GeneratoreSpettrale::imposta_battito(double hz, double profondita) {
    if (profondita > 0.0) battito_hz_ = hz;
    battito_bersaglio_ = profondita;
}

void GeneratoreSpettrale::fotogramma() {
    // un fotogramma di rumore colorato con fasi a caso, sotto la radice della finestra di Hann: due fotogrammi
    // sovrapposti per meta' sommano la potenza a 1, quindi il rumore resta stazionario
    std::vector<Complesso> x(n_, Complesso(0.0, 0.0));
    for (std::size_t j = 1; j < n_ / 2; ++j) {
        const double fase = 2.0 * PI * caso_.uniforme();
        if (ampiezze_.size() <= j || ampiezze_[j] <= 0.0) continue;
        x[j] = std::polar(ampiezze_[j], fase);
        x[n_ - j] = std::conj(x[j]);
    }
    fft(x, true);
    scala_ = scala_ <= 0.0 ? scala_bersaglio_ : scala_ + SCALA_FOTOGRAMMA * (scala_bersaglio_ - scala_);
    if (sovrapposto_.size() != n_ / 2) sovrapposto_.assign(n_ / 2, 0.0f);
    for (std::size_t i = 0; i < n_; ++i) {
        const double w = std::sqrt(0.5 * (1.0 - std::cos(2.0 * PI * static_cast<double>(i) / static_cast<double>(n_))));
        const float v = static_cast<float>(x[i].real() * w);
        if (i < n_ / 2) {
            coda_.push_back(sovrapposto_[i] + v);
            scale_coda_.push_back(static_cast<float>(scala_));
        } else {
            sovrapposto_[i - n_ / 2] = v;
        }
    }
}

std::vector<float> GeneratoreSpettrale::genera(std::size_t n, const std::vector<double>* aggiunta) {
    while (coda_.size() < n) fotogramma();
    const double fs = fs_;
    const double alfa = 1.0 - std::exp(-1.0 / (RAMPA_RIGHE_S * fs));
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        double y = coda_[i];
        battito_ += alfa * (battito_bersaglio_ - battito_);
        fase_battito_ += 2.0 * PI * battito_hz_ / fs;
        y *= 1.0 + battito_ * std::sin(fase_battito_);
        for (auto& [chiave, x] : righe_) {
            x.ampiezza += alfa * (x.bersaglio - x.ampiezza);
            x.fase += 2.0 * PI * x.hz / fs;
            y += x.ampiezza * std::sin(x.fase);
        }
        y *= scale_coda_[i];
        if (aggiunta) y += (*aggiunta)[i];
        if (std::fabs(y * limite_) > PICCO_MAX) limite_ = PICCO_MAX / std::fabs(y);
        else limite_ = std::min(1.0, limite_ * (1.0 + LIMITE_RIPRESA));
        out[i] = static_cast<float>(std::clamp(y * limite_, -1.0, 1.0));
    }
    coda_.erase(coda_.begin(), coda_.begin() + static_cast<long>(n));
    scale_coda_.erase(scale_coda_.begin(), scale_coda_.begin() + static_cast<long>(n));
    fase_battito_ = std::fmod(fase_battito_, 2.0 * PI);
    for (auto it = righe_.begin(); it != righe_.end();) {   // le righe spente e gia' a zero se ne vanno
        it->second.fase = std::fmod(it->second.fase, 2.0 * PI);
        if (it->second.bersaglio <= 0.0 && it->second.ampiezza < 1e-12) it = righe_.erase(it);
        else ++it;
    }
    return out;
}

// --- cuffia in tempo reale ---

CuffiaSonar::CuffiaSonar(const ParametriAscolto& p) : p_(p), g_(p.campionamento_hz, p.seme) {}

void CuffiaSonar::imposta(const ParametriAscolto& p) {
    const bool frequenza = p.campionamento_hz != p_.campionamento_hz || p.seme != p_.seme;
    p_ = p;
    pronto_ = false;   // filtri, ottava e accelerazione al prossimo passo
    if (frequenza) {
        g_ = GeneratoreSpettrale(p.campionamento_hz, p.seme);
        azzera();
    }
}

void CuffiaSonar::azzera() {
    g_.azzera();
    resto_ = 0.0;
    dal_aggiornamento_ = 0.0;
    pronto_ = false;
    campione_ = 0;
    schiocchi_.clear();
    attivi_.clear();
    prossimo_seme_ = 1;
    lettori_.clear();
}

void CuffiaSonar::prepara(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio) {
    pronto_ = true;
    const double fs = p_.campionamento_hz, k = std::max(p_.accelerazione, 1e-3);
    const double f_hi_out = std::min(p_.f_max_hz, 0.49 * fs);
    const double lo = p_.f_min_hz / k, hi = f_hi_out / k;
    std::vector<double> densita(g_.bin(), 0.0);
    std::vector<GeneratoreSpettrale::RigaSintesi> righe;
    if (!r.sensore || hi <= lo) {
        g_.imposta_rumore(densita, 0.0);
        g_.imposta_righe(righe);
        g_.imposta_battito(0.0, 0.0);
        return;
    }
    // lo spettro alla risoluzione del fotogramma: basta per colorare il rumore, e la potenza delle righe non dipende
    // dal bin (il loro S/N e' riferito al rumore nello stesso bin)
    const double df = g_.df_hz();
    const double bin = std::max(0.05, df / k);
    // i transitori impulsivi fuori dallo spettro: diventano schiocchi (sotto, quando la scala del fondo e' nota)
    std::vector<const Sorgente*> lampi;
    for (const Sorgente& s : sorgenti)
        if (impulsivo(s)) lampi.push_back(&s);
    // R5: le sorgenti con un campione, fuori dallo spettro del generatore anche loro (tranne fuori dalla banda valida)
    std::vector<std::pair<const Sorgente*, const Campione*>> registrate;
    if (libreria_ && !libreria_->vuota())
        for (const Sorgente& s : sorgenti)
            if (!impulsivo(s) && (s.fonte || s.firma))
                if (const Campione* c = libreria_->scegli(voce_campione(s), seme_istanza(s)))
                    registrate.emplace_back(&s, c);
    const auto registrata = [&](const Sorgente* s) {
        return std::any_of(registrate.begin(), registrate.end(), [&](const auto& x) { return x.first == s; });
    };
    // il continuo senza i transitori impulsivi e senza le sorgenti registrate; le righe senza i soli impulsivi (le righe
    // delle sorgenti registrate restano quelle del modello)
    std::vector<Sorgente> senza_lampi, solo_righe;
    if (!lampi.empty() || !registrate.empty())
        for (const Sorgente& s : sorgenti) {
            if (std::find(lampi.begin(), lampi.end(), &s) != lampi.end()) continue;
            solo_righe.push_back(s);
            if (!registrata(&s)) senza_lampi.push_back(s);
        }
    const std::vector<Sorgente>& scena = lampi.empty() && registrate.empty() ? sorgenti : senza_lampi;
    const std::vector<Sorgente>& scena_righe = lampi.empty() && registrate.empty() ? sorgenti : solo_righe;
    const Spettro sp = spettro_continuo(scena, r, amb, rilevamento_fascio, lo, hi, bin, true);   // le righe a parte
    const auto snr = [&](double f) {
        if (sp.snr_db.empty()) return 0.0;
        const double x = std::clamp((f - sp.f0_hz) / sp.df_hz - 0.5, 0.0, static_cast<double>(sp.snr_db.size() - 1));
        const std::size_t i = static_cast<std::size_t>(x);
        const std::size_t j = std::min(i + 1, sp.snr_db.size() - 1);
        return sp.snr_db[i] + (x - static_cast<double>(i)) * (sp.snr_db[j] - sp.snr_db[i]);
    };
    double varianza_fondo = 0.0;
    for (std::size_t j = 1; j < g_.bin(); ++j) {
        const double f_out = static_cast<double>(j) * df;
        if (f_out < p_.f_min_hz || f_out > f_hi_out) continue;
        const double f = f_out / k;
        const double fondo = potenza(rumore_fascio_db(f, r, amb)) / k;
        varianza_fondo += fondo * df;
        densita[j] = fondo * potenza(snr(f));
    }
    const double lo_righe = p_.ottava_su ? std::min(lo, RIGHE_MIN_HZ) : lo;
    for (const RigaRicevuta& rr : righe_ricevute(scena_righe, r, amb, rilevamento_fascio, lo_righe, hi, bin, true)) {
        if (rr.snr_db <= 0.0) continue;
        double f_out = rr.hz * k;
        if (p_.ottava_su)
            while (f_out < SOGLIA_UDIBILE_HZ) f_out *= 2.0;
        if (f_out < p_.f_min_hz || f_out > f_hi_out) continue;
        righe.push_back({{rr.sorgente, std::lround(rr.hz_emessa * 1000.0)}, f_out,
                         std::sqrt(2.0 * potenza(rumore_fascio_db(rr.hz, r, amb)) * bin * potenza(rr.snr_db))});
    }
    const double scala = varianza_fondo > 0.0 ? std::pow(10.0, p_.rumore_dbfs / 20.0) / std::sqrt(varianza_fondo) : 0.0;
    // R5: lo spettro di ciascuna sorgente registrata; dentro la banda del campione lo porta la tessitura (guadagni per
    // bin), fuori resta nel generatore
    for (auto& [id, l] : lettori_) l.visto = false;
    for (const auto& [s, c] : registrate) {
        const Spettro sp_s = spettro_continuo({*s}, r, amb, rilevamento_fascio, lo, hi, bin, true);
        const auto snr_s = [&](double f) {
            if (sp_s.snr_db.empty()) return 0.0;
            const double x = std::clamp((f - sp_s.f0_hz) / sp_s.df_hz - 0.5, 0.0, static_cast<double>(sp_s.snr_db.size() - 1));
            const std::size_t i = static_cast<std::size_t>(x);
            const std::size_t j = std::min(i + 1, sp_s.snr_db.size() - 1);
            return sp_s.snr_db[i] + (x - static_cast<double>(i)) * (sp_s.snr_db[j] - sp_s.snr_db[i]);
        };
        Lettore& l = lettori_[s->id];
        const bool nuovo = l.c != c;
        if (nuovo) l = Lettore{}, l.c = c;
        l.visto = true;
        // il passo di lettura: tempo vero, accelerazione, Doppler; le navi al battito d'elica del modello (al piu' x1,3)
        double m = 1.0;
        if (s->firma && c->demon_hz > 0.0)
            if (const double fp = frequenza_pala_hz(*s); fp > 0.0) m = std::clamp(fp / c->demon_hz, 0.77, 1.3);
        const double dop = 1.0 + avvicinamento(*s, r) / 1500.0;
        l.passo = c->campionamento_hz / (c->fattore * fs) * k * dop * m;
        const double lunghezza = static_cast<double>(c->campioni.size());
        if (s->fonte && lunghezza > 0.0) {   // la posizione del tempo di gioco (con lo scarto dell'istanza)
            const double voluta = posizione_campione_s(*c, seme_istanza(*s), s->tempo_s) * c->campionamento_hz;
            double d = std::fabs(voluta - std::fmod(l.pos, lunghezza));
            d = std::min(d, lunghezza - d);
            if (nuovo || d > 0.5 * c->campionamento_hz) l.pos = voluta;
        }
        const double f_lo_tex = c->f_min_hz * k * dop * m, f_hi_tex = std::min(c->f_max_hz * k * dop * m, f_hi_out_campione(fs));
        const double banda = std::max(f_hi_tex - f_lo_tex, 1.0);
        l.guadagni.assign(g_.bin() + 1, 0.0);
        for (std::size_t j = 1; j < g_.bin(); ++j) {
            const double f_out = static_cast<double>(j) * df;
            if (f_out < p_.f_min_hz || f_out > f_hi_out) continue;
            const double f = f_out / k;
            const double sorgente = potenza(rumore_fascio_db(f, r, amb)) / k * std::max(potenza(snr_s(f)) - 1.0, 0.0);
            if (f_out >= f_lo_tex && f_out <= f_hi_tex && scala > 0.0)
                l.guadagni[j] = std::sqrt(sorgente * scala * scala * banda / (c->tessitura_rms * c->tessitura_rms));
            else
                densita[j] += sorgente;   // fuori dalla banda valida del campione: il rumore del modello
        }
    }
    // il vento e la pioggia del rumore ambiente con le loro tessiture (le chiavi -1 e -2): la loro parte nel fascio esce
    // dal generatore dentro la banda del campione e la porta la tessitura ricolorata
    if (libreria_ && !libreria_->vuota() && scala > 0.0)
        for (const auto& [chiave, voce] : {std::pair{-1, "vento"}, std::pair{-2, "pioggia"}}) {
            const Campione* c = libreria_->scegli(voce, static_cast<std::uint64_t>(-chiave));
            if (!c) continue;
            std::vector<double> parte(g_.bin() + 1, 0.0);
            bool c_e = false;
            for (std::size_t j = 1; j < g_.bin(); ++j) {
                const double f_out = static_cast<double>(j) * df;
                if (f_out < p_.f_min_hz || f_out > f_hi_out) continue;
                const double f = f_out / k;
                const ComponentiRumore cr = componenti_rumore_ambiente(f, amb);
                const double db = chiave == -1 ? cr.vento : cr.pioggia;
                if (db <= -299.0) continue;
                parte[j] = potenza(db - indice_direttivita(*r.sensore, f)) / k;
                c_e = true;
            }
            if (!c_e) continue;
            Lettore& l = lettori_[chiave];
            if (l.c != c) l = Lettore{}, l.c = c;
            l.visto = true;
            l.passo = c->campionamento_hz / (c->fattore * fs) * k;
            const double f_lo_tex = c->f_min_hz * k, f_hi_tex = std::min(c->f_max_hz * k, f_hi_out_campione(fs));
            const double banda = std::max(f_hi_tex - f_lo_tex, 1.0);
            l.guadagni.assign(g_.bin() + 1, 0.0);
            for (std::size_t j = 1; j < g_.bin(); ++j) {
                const double f_out = static_cast<double>(j) * df;
                if (parte[j] <= 0.0 || f_out < f_lo_tex || f_out > f_hi_tex) continue;
                l.guadagni[j] = std::sqrt(parte[j] * scala * scala * banda / (c->tessitura_rms * c->tessitura_rms));
                densita[j] = std::max(densita[j] - parte[j], 0.0);
            }
        }
    for (auto it = lettori_.begin(); it != lettori_.end();) it = it->second.visto ? std::next(it) : lettori_.erase(it);
    g_.imposta_rumore(densita, scala);
    g_.imposta_righe(righe);
    for (const Sorgente* s : lampi) {
        const bool attivo = livello_totale_fonte(*s) > 0.0;
        bool& prima = attivi_[s->id];
        if (attivo && !prima && scala > 0.0) {
            const Componente& c0 = s->fonte->componenti.front();
            const bool lampo = s->fonte->categoria == "meteo";
            const double km = distanza(r.pos, s->pos) / 1000.0;
            const double f_lo = std::max(c0.f_lo, 5.0), f_hi = std::max(c0.f_hi, f_lo * 1.5);
            // il S/N al centro della banda (il fulmine a 150 Hz), sopra la densita' del fondo d'uscita, su tutta la banda
            const double fc = lampo ? 150.0 : (c0.picco_hz > 0.0 ? c0.picco_hz : std::sqrt(f_lo * f_hi));
            const double snr_db = snr_impulso_db(*s, r, amb, rilevamento_fascio, fc);
            const double densita_fc = potenza(rumore_fascio_db(fc, r, amb)) / k * scala * scala;
            Schiocco x;
            x.inizio = campione_;
            x.scala_t = 1.0 / k;
            x.caso = Casuale(p_.seme ^ (0xd1b54a32d192ed03ull * prossimo_seme_++));
            if (lampo) {   // 2 km -> 1 kHz, 10 -> 500 Hz, 30 -> 300 (21e §5.2)
                x.ampiezza = std::sqrt(densita_fc * potenza(snr_db) * 300.0 * k);
                x.taglio_hz = std::min(std::clamp(1000.0 * std::pow(std::max(km, 0.5) / 2.0, -0.43), 300.0, 1000.0) * k, 0.45 * fs);
                x.acuto = km < 3.0;
                x.coda = 0.25;
            } else {       // l'assorbimento: il passa basso dove la perdita supera di 6 dB quella al centro
                double taglio = f_hi;
                const double tl0 = perdita_trasmissione(fc, std::max(km * 1000.0, 1.0), s->quota_m, r.quota_m, amb).tl_db;
                for (double f = fc * 2.0; f < f_hi; f *= 2.0)
                    if (perdita_trasmissione(f, std::max(km * 1000.0, 1.0), s->quota_m, r.quota_m, amb).tl_db > tl0 + 6.0) { taglio = f; break; }
                x.ampiezza = std::sqrt(densita_fc * potenza(snr_db) * (std::min(taglio, f_hi) - f_lo) * k);
                x.taglio_hz = std::min(taglio * k, 0.45 * fs);
                x.basso_hz = f_lo;
                x.tau_s = std::max(c0.durata_s / 3.0, 0.003);
            }
            // il rumore filtrato riportato a rms 1 (lo stesso filtro su un tratto di prova)
            Casuale prova(11);
            const double a = 1.0 - std::exp(-2.0 * PI * x.taglio_hz / fs), h = 1.0 - std::exp(-2.0 * PI * x.basso_hz * k / fs);
            double f1 = 0.0, f2 = 0.0, f3 = 0.0, q = 0.0;
            for (int i = 0; i < 8192; ++i) {
                f1 += a * ((2.0 * prova.uniforme() - 1.0) - f1);
                f2 += a * (f1 - f2);
                f3 += h * (f2 - f3);
                if (i >= 1024) q += (f2 - f3) * (f2 - f3);
            }
            x.norma = 1.0 / std::sqrt(std::max(q / 7168.0, 1e-30));
            if (snr_db > -10.0) schiocchi_.push_back(x);
        }
        prima = attivo;
    }
    double battito_hz = 0.0, battito = 0.0;
    for (const Sorgente& s : sorgenti) {
        if (!s.firma || lettori_.count(s.id)) continue;   // la tessitura registrata ha gia' il suo battito
        if (const auto d = leggi_demon(demon(s, r, amb))) {
            battito_hz = d->pala_hz * k;
            battito = BATTITO;
            break;
        }
    }
    g_.imposta_battito(battito_hz, battito);
}

std::vector<float> CuffiaSonar::passo(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                                      double rilevamento_fascio, double dt_s) {
    // la scena si rilegge ogni AGGIORNAMENTO_S di audio (contati in campioni, quindi uguale comunque si spezzino i passi)
    const double fs = p_.campionamento_hz;
    if (!pronto_ || dal_aggiornamento_ >= AGGIORNAMENTO_S * fs) {
        prepara(sorgenti, r, amb, rilevamento_fascio);
        dal_aggiornamento_ = 0.0;
    }
    const double esatti = std::max(0.0, dt_s) * fs + resto_;
    const std::size_t n = static_cast<std::size_t>(std::floor(esatti));
    resto_ = esatti - static_cast<double>(n);
    dal_aggiornamento_ += static_cast<double>(n);
    // R5: le tessiture entrano nel generatore prima del limitatore, come il resto della scena
    std::vector<double> tessiture;
    if (!lettori_.empty()) {
        tessiture.assign(n, 0.0);
        for (auto& [id, l] : lettori_) {
            while (l.coda.size() < n) fotogramma_lettore(l);
            for (std::size_t i = 0; i < n; ++i) tessiture[i] += l.coda[i];
            l.coda.erase(l.coda.begin(), l.coda.begin() + static_cast<std::ptrdiff_t>(n));
        }
    }
    std::vector<float> out = g_.genera(n, tessiture.empty() ? nullptr : &tessiture);
    if (!schiocchi_.empty()) {
        for (std::size_t i = 0; i < n; ++i) {
            double v = 0.0;
            for (Schiocco& s : schiocchi_) v += campione_schiocco(s, campione_ + i);
            out[i] = static_cast<float>(std::tanh(static_cast<double>(out[i]) + v));   // come il limitatore
        }
        schiocchi_.erase(std::remove_if(schiocchi_.begin(), schiocchi_.end(), [&](const Schiocco& s) {
            return static_cast<double>(campione_ + n - s.inizio) / fs > std::max(1.5, 8.0 * s.tau_s) * s.scala_t;
        }), schiocchi_.end());
    }
    campione_ += n;
    return out;
}

void CuffiaSonar::fotogramma_lettore(Lettore& l) {
    // un fotogramma della tessitura: letta al passo (interpolazione cubica, ad anello), sotto la radice di Hann,
    // filtrata coi guadagni e risommata per meta' (analisi e sintesi sotto la radice di Hann: guadagno 1)
    const std::size_t n = 2 * g_.bin();
    const std::vector<float>& x = l.c->campioni;
    const double lunghezza = static_cast<double>(x.size());
    std::vector<Complesso> a(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double t = std::fmod(l.pos + static_cast<double>(i) * l.passo, lunghezza);
        const std::size_t k = static_cast<std::size_t>(t), m = x.size();
        const double u = t - static_cast<double>(k);
        // Catmull-Rom sui quattro vicini (la lineare spegne gli acuti di 1-2 dB)
        const double p0 = x[(k + m - 1) % m], p1 = x[k], p2 = x[(k + 1) % m], p3 = x[(k + 2) % m];
        const double v = p1 + 0.5 * u * (p2 - p0 + u * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3 + u * (3.0 * (p1 - p2) + p3 - p0)));
        a[i] = Complesso(v * std::sqrt(0.5 * (1.0 - std::cos(2.0 * PI * static_cast<double>(i) / static_cast<double>(n)))), 0.0);
    }
    l.pos = std::fmod(l.pos + static_cast<double>(n / 2) * l.passo, lunghezza);
    fft(a, false);
    for (std::size_t j = 0; j <= n / 2; ++j) {
        const double g = j < l.guadagni.size() ? l.guadagni[j] : 0.0;
        a[j] *= g;
        if (j > 0 && j < n / 2) a[n - j] *= g;
    }
    fft(a, true);
    if (l.sovrapposto.size() != n / 2) l.sovrapposto.assign(n / 2, 0.0f);
    for (std::size_t i = 0; i < n; ++i) {
        const double w = std::sqrt(0.5 * (1.0 - std::cos(2.0 * PI * static_cast<double>(i) / static_cast<double>(n))));
        const float v = static_cast<float>(a[i].real() * w);
        if (i < n / 2) l.coda.push_back(l.sovrapposto[i] + v);
        else l.sovrapposto[i - n / 2] = v;
    }
}

bool impulsivo(const Sorgente& s) {
    if (!s.fonte || s.fonte->componenti.empty() || s.fonte->ritmo.processo != Processo::singolo) return false;
    const auto& f = *s.fonte;
    const Componente& c = f.componenti.front();
    return (f.categoria == "meteo" || f.categoria == "arma" || f.categoria == "scafo") && f.sottocategoria != "ping" &&
           c.tipo == TipoComponente::impulso && c.durata_s <= 0.5;
}

double CuffiaSonar::campione_schiocco(Schiocco& s, std::uint64_t k) const {
    // 21e §5.2: attacco 1,5 ms, caduta 40 ms, coda di multipercorso di 250 ms a -12 dB; lo schiocco acuto di 8 ms
    const double fs = p_.campionamento_hz;
    const double t = static_cast<double>(k - s.inizio) / fs / s.scala_t;
    const double b = 2.0 * s.caso.uniforme() - 1.0;
    const double a = 1.0 - std::exp(-2.0 * PI * s.taglio_hz / fs), h = 1.0 - std::exp(-2.0 * PI * s.basso_hz / s.scala_t / fs);
    s.f1 += a * (b - s.f1);
    s.f2 += a * (s.f1 - s.f2);
    s.f3 += h * (s.f2 - s.f3);
    const double inviluppo = std::min(1.0, t / 0.0015) * (std::exp(-t / s.tau_s) + s.coda * std::exp(-t / 0.25));
    double v = s.norma * (s.f2 - s.f3) * inviluppo;
    if (s.acuto && t < 0.05) v += 0.5 * (b - s.f1) * std::min(1.0, t / 0.0005) * std::exp(-t / 0.008);
    return s.ampiezza * v;
}

bool scrivi_wav(const std::string& percorso, const Suono& s) {
    std::ofstream f(percorso, std::ios::binary);
    if (!f) return false;
    const std::uint32_t dati = static_cast<std::uint32_t>(s.campioni.size() * 2);
    f.write("RIFF", 4);
    scrivi32(f, 36 + dati);
    f.write("WAVEfmt ", 8);
    scrivi32(f, 16);
    scrivi16(f, 1);   // PCM
    scrivi16(f, 1);   // mono
    scrivi32(f, static_cast<std::uint32_t>(s.campionamento_hz));
    scrivi32(f, static_cast<std::uint32_t>(s.campionamento_hz) * 2);
    scrivi16(f, 2);
    scrivi16(f, 16);
    f.write("data", 4);
    scrivi32(f, dati);
    for (const std::int16_t c : s.campioni) scrivi16(f, static_cast<std::uint16_t>(c));
    return static_cast<bool>(f);
}

double ampiezza_a(const Suono& s, double hz) {
    const std::size_t n = s.campioni.size();
    if (n == 0) return 0.0;
    const double w = 2.0 * PI * hz / s.campionamento_hz, c = 2.0 * std::cos(w);
    double s1 = 0.0, s2 = 0.0;
    for (const std::int16_t x : s.campioni) {
        const double s0 = x / 32767.0 + c * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    const double re = s1 - s2 * std::cos(w), im = s2 * std::sin(w);
    return 2.0 * std::hypot(re, im) / static_cast<double>(n);
}

}  // namespace m688
