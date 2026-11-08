#include "nucleo/rilevatore_righe.hpp"

#include <algorithm>
#include <bit>
#include <cmath>

namespace m688 {

namespace {
constexpr double MEDIA_SNR = 0.3, MEDIA_DERIVA = 0.2;   // le medie mobili (STIMA)
}

RilevatoreRighe::RilevatoreRighe(ParametriRilevatore p) : p_(p) {}

void RilevatoreRighe::azzera() {
    storia_.clear();
    prossima_ = 0;
    righe_.clear();
    stati_.clear();
    ultimo_t_ = -1.0;
}

std::vector<RigaAutomatica> RilevatoreRighe::confermate() const {
    std::vector<RigaAutomatica> out;
    for (const RigaAutomatica& r : righe_)
        if (r.confermata) out.push_back(r);
    return out;
}

void RilevatoreRighe::aggiungi(const Spettro& s, double t) {
    const double dt = ultimo_t_ < 0.0 ? 0.0 : t - ultimo_t_;
    ultimo_t_ = t;
    const double bin = s.df_hz > 0.0 ? s.df_hz : 0.1;
    // l'integratore: la media delle ultime righe (si ricomincia se cambia la scala del LOFAR)
    if (!storia_.empty() && (s.f0_hz != f0_ || s.df_hz != df_ || s.snr_db.size() != storia_.front().size())) {
        storia_.clear();
        prossima_ = 0;
    }
    f0_ = s.f0_hz;
    df_ = s.df_hz;
    const std::size_t quante = static_cast<std::size_t>(std::max(1, p_.integrazione));
    if (storia_.size() < quante) storia_.push_back(s.snr_db);
    else storia_[prossima_] = s.snr_db;
    prossima_ = (prossima_ + 1) % quante;
    Spettro media = s;
    std::fill(media.snr_db.begin(), media.snr_db.end(), 0.0f);
    for (const std::vector<float>& r : storia_)
        for (std::size_t i = 0; i < media.snr_db.size(); ++i) media.snr_db[i] += r[i];
    for (float& x : media.snr_db) x /= static_cast<float>(storia_.size());
    // le tracce vive si seguono sulla riga nuova, dove la riga e' adesso (la media resta indietro quando la riga corre,
    // al CPA); se li' non c'e', sulla media. Le tracce nuove nascono solo dalla media: la grana di una riga sola non basta
    const std::vector<RigaOsservata> grezzi = righe_osservate(s, p_.soglia_db), medi = righe_osservate(media, p_.soglia_db);
    std::vector<bool> preso_g(grezzi.size(), false), preso_m(medi.size(), false);
    // le tracce si prendono il picco piu' vicino alla loro previsione, dalle piu' forti (le deboli si scambiano piu' spesso)
    std::vector<std::size_t> ordine(righe_.size());
    for (std::size_t i = 0; i < ordine.size(); ++i) ordine[i] = i;
    std::sort(ordine.begin(), ordine.end(), [&](std::size_t a, std::size_t b) { return righe_[a].snr_db > righe_[b].snr_db; });
    const auto piu_vicino = [](const std::vector<RigaOsservata>& v, const std::vector<bool>& preso, double hz, double tol) {
        int scelto = -1;
        for (std::size_t k = 0; k < v.size(); ++k)
            if (!preso[k] && std::fabs(v[k].hz - hz) <= tol &&
                (scelto < 0 || std::fabs(v[k].hz - hz) < std::fabs(v[static_cast<std::size_t>(scelto)].hz - hz)))
                scelto = static_cast<int>(k);
        return scelto;
    };
    for (std::size_t i : ordine) {
        RigaAutomatica& r = righe_[i];
        Stato& st = stati_[i];
        st.colpi <<= 1;
        const double previsto = r.hz + r.deriva_hz_min * dt / 60.0;
        const double tol = p_.tolleranza_bin * bin + 0.5 * std::fabs(r.deriva_hz_min * dt / 60.0);
        const RigaOsservata* x = nullptr;
        if (const int k = piu_vicino(grezzi, preso_g, previsto, tol); k >= 0) {
            preso_g[static_cast<std::size_t>(k)] = true;
            x = &grezzi[static_cast<std::size_t>(k)];
        } else if (const int km = piu_vicino(medi, preso_m, previsto, tol); km >= 0) {
            preso_m[static_cast<std::size_t>(km)] = true;
            x = &medi[static_cast<std::size_t>(km)];
        }
        if (x == nullptr) continue;
        if (t - st.ultimo_t > 0.0) {
            const double deriva = (x->hz - r.hz) / (t - st.ultimo_t) * 60.0;
            r.deriva_hz_min += MEDIA_DERIVA * (deriva - r.deriva_hz_min);
        }
        r.hz = x->hz;
        r.snr_db += MEDIA_SNR * (x->snr_db - r.snr_db);
        r.ultima_s = t;
        st.ultimo_t = t;
        st.colpi |= 1u;
    }
    // i picchi liberi della media, lontani dalle tracce: tracce nuove, da confermare
    const double fondi = p_.fusione_hz > 0.0 ? p_.fusione_hz : 2.0 * bin;
    const std::size_t vive = righe_.size();
    for (std::size_t k = 0; k < medi.size(); ++k) {
        if (preso_m[k]) continue;
        bool vicino = false;
        for (std::size_t i = 0; i < vive && !vicino; ++i) vicino = std::fabs(righe_[i].hz - medi[k].hz) < fondi;
        if (vicino) continue;
        RigaAutomatica r;
        r.id = prossimo_++;
        r.hz = medi[k].hz;
        r.snr_db = medi[k].snr_db;
        r.nata_s = r.ultima_s = t;
        righe_.push_back(r);
        stati_.push_back({1u, t});
    }
    // la conferma (M delle ultime N), la perdita, la fusione
    const std::uint32_t maschera = p_.n >= 32 ? ~0u : ((1u << p_.n) - 1u);
    for (std::size_t i = 0; i < righe_.size(); ++i)
        if (std::popcount(stati_[i].colpi & maschera) >= p_.m) righe_[i].confermata = true;
    std::vector<bool> via(righe_.size(), false);
    for (std::size_t i = 0; i < righe_.size(); ++i) {
        const double senza = t - righe_[i].ultima_s;
        // le non confermate che non tornano spariscono presto, le confermate dopo perdita_s
        if (senza > p_.perdita_s || (!righe_[i].confermata && (stati_[i].colpi & maschera) == 0u)) via[i] = true;
    }
    for (std::size_t i = 0; i < righe_.size(); ++i)
        for (std::size_t j = i + 1; j < righe_.size(); ++j) {
            if (via[i] || via[j] || std::fabs(righe_[i].hz - righe_[j].hz) >= fondi) continue;
            // resta la piu' vecchia confermata (o la piu' forte): l'altra si fonde in lei
            const bool tieni_i = righe_[i].confermata != righe_[j].confermata ? righe_[i].confermata : righe_[i].snr_db >= righe_[j].snr_db;
            via[tieni_i ? j : i] = true;
        }
    std::size_t w = 0;
    for (std::size_t i = 0; i < righe_.size(); ++i)
        if (!via[i]) {
            righe_[w] = righe_[i];
            stati_[w] = stati_[i];
            ++w;
        }
    righe_.resize(w);
    stati_.resize(w);
}

}  // namespace m688
