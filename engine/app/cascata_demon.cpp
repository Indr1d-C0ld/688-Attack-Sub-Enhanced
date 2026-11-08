#include "cascata_demon.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/palette.hpp"

namespace m688 {

namespace {

std::vector<AreaAttiva>* aree_ = nullptr;
void area(int x, int y, int w, int h, std::string comando) {
    if (aree_) aree_->push_back({x, y, w, h, std::move(comando)});
}

// il massimo dei bin che cadono in ogni pixel, da 0 a f_max su `larghezza` pixel
std::vector<float> pixel(const Spettro& s, double f_max, int larghezza) {
    std::vector<float> px(static_cast<std::size_t>(std::max(larghezza, 0)), 0.0f);
    for (std::size_t i = 0; i < s.snr_db.size(); ++i) {
        const int x = static_cast<int>(s.frequenza(i) / f_max * larghezza);
        if (x < 0 || x >= larghezza) continue;
        auto& v = px[static_cast<std::size_t>(x)];
        v = std::max(v, s.snr_db[i]);
    }
    return px;
}

std::string virgola(std::string s) {
    for (char& c : s) if (c == '.') c = ',';
    return s;
}

}  // namespace

// --- la storia ---

void CascataDemon::aggiungi(const Spettro& riga) {
    static constexpr int PER_SCALA[3] = {1, 4, 16};
    for (std::size_t k = 1; k < 3; ++k) {   // scale lente: il massimo di 4 o 16 righe, bin per bin
        if (acc_n_[k] == 0 || acc_[k].snr_db.size() != riga.snr_db.size()) {
            acc_[k] = riga;
            acc_n_[k] = 0;
        } else {
            for (std::size_t i = 0; i < riga.snr_db.size(); ++i) acc_[k].snr_db[i] = std::max(acc_[k].snr_db[i], riga.snr_db[i]);
        }
        if (++acc_n_[k] == PER_SCALA[k]) {
            righe_[k].push_front(acc_[k]);
            acc_n_[k] = 0;
        }
    }
    righe_[0].push_front(riga);
    for (auto& d : righe_)
        while (static_cast<int>(d.size()) > RIGHE_MAX) d.pop_back();
}

void CascataDemon::svuota() {
    for (auto& d : righe_) d.clear();
    acc_n_ = {};
    agganciato_ = false;
    serie_ = 0;
}

double CascataDemon::f_max() const {
    if (righe_[0].empty()) return 100.0;
    const Spettro& s = righe_[0].front();
    return s.f0_hz + s.df_hz * static_cast<double>(s.snr_db.size());
}

// --- i cursori ---

void CascataDemon::sposta_asse(int verso, bool fine) {
    if (verso == 0) return;
    agganciato_ = false;
    if (asse_hz_ <= 0.0) { asse_hz_ = 1.0; return; }
    const double passo = fine ? PASSO_FINE_HZ : PASSO_GROSSO_HZ;
    asse_hz_ = std::clamp(std::round((asse_hz_ + (verso > 0 ? passo : -passo)) / PASSO_FINE_HZ) * PASSO_FINE_HZ, PASSO_FINE_HZ,
                          f_max() / PALE_MIN);
}

void CascataDemon::pale(int verso) {
    if (verso == 0) return;
    agganciato_ = false;
    pale_ = std::clamp(pale_ + (verso > 0 ? 1 : -1), PALE_MIN, PALE_MAX);
}

std::vector<LetturaDemon> CascataDemon::serie() const {
    if (righe_[0].empty()) return {};
    return leggi_demon_fascio(righe_[0].front());
}

bool CascataDemon::aggancia() {
    const auto v = serie();
    if (v.empty()) return false;
    serie_ = agganciato_ ? (serie_ + 1) % static_cast<int>(v.size()) : 0;
    const LetturaDemon& l = v[static_cast<std::size_t>(serie_)];
    pale_ = std::clamp(l.pale, PALE_MIN, PALE_MAX);
    // l'asse dalla pala, che e' la riga piu' forte: un bin di errore diviso per le pale (la riga d'asse ne sbaglia uno)
    asse_hz_ = l.pala_hz > 0.0 ? l.pala_hz / pale_ : l.asse_hz;
    agganciato_ = true;
    return true;
}

std::optional<LetturaDemon> CascataDemon::lettura() const {
    if (asse_hz_ <= 0.0) return std::nullopt;
    return LetturaDemon{asse_hz_, asse_hz_ * pale_, pale_};
}

// --- disegno ---

double CascataDemon::frequenza_a(Rettangolo r, int x) const {
    return std::clamp(static_cast<double>(x - r.x) + 0.5, 0.0, static_cast<double>(r.w)) / r.w * f_max();
}

int CascataDemon::x_di(Rettangolo r, double hz) const { return r.x + static_cast<int>(hz / f_max() * r.w); }

void CascataDemon::disegna(Tela& t, const Font& f, Rettangolo r, const StileDemon& s, std::vector<AreaAttiva>* aree) const {
    aree_ = aree;
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
    const int testo = s.modo_1989 ? colori::VERDE_CHIARO : colori::BIANCO;
    const int tenue = s.modo_1989 ? colori::VERDE : colori::GRIGIO_SCURO;
    const double fm = f_max();
    const auto& storia = righe(s.scala_tempi);
    const Rettangolo ascan{r.x, r.y, r.w, ALTO_ASCAN};
    const Rettangolo casc{r.x, r.y + ALTO_ASCAN + 1, r.w, r.h - ALTO_ASCAN - 1 - ALTO_TESTO};
    const auto u = [&](float db) { return cascate::frazione(db, s.pavimento_db, s.tetto_db); };

    // l'A-scan dell'ultima riga, alto quanto il livello
    t.linea_orizzontale(ascan.x, ascan.x + ascan.w - 1, ascan.y + ascan.h - 1, static_cast<std::uint8_t>(tenue));
    if (!righe_[0].empty()) {
        const auto px = pixel(righe_[0].front(), fm, r.w);
        for (int x = 0; x < r.w; ++x) {
            const int h = static_cast<int>(u(px[static_cast<std::size_t>(x)]) * (ascan.h - 2));
            if (h > 0) t.linea_verticale(r.x + x, ascan.y + ascan.h - 1 - h, ascan.y + ascan.h - 2, static_cast<std::uint8_t>(testo));
        }
    }
    area(ascan.x, ascan.y, ascan.w, ascan.h, "demon_serie");

    // la cascata: la riga nuova in alto, una riga per pixel
    std::optional<cascate::Fosforo> fosforo;
    if (s.fosforo) fosforo.emplace(casc.w, casc.h);
    for (int y = 0; y < casc.h && y < static_cast<int>(storia.size()); ++y) {
        const auto px = pixel(storia[static_cast<std::size_t>(y)], fm, casc.w);
        for (int x = 0; x < casc.w; ++x) {
            const double v = u(px[static_cast<std::size_t>(x)]);
            if (fosforo) fosforo->deponi(x, y, v);
            else if (v > 0.0) t.pixel(casc.x + x, casc.y + y, cascate::indice(v, casc.x + x, casc.y + y, s.scala));
        }
    }
    if (fosforo) fosforo->componi(t, casc.x, casc.y, s.scala, cascate::Fosforo::Effetti{});
    area(casc.x, casc.y, casc.w, casc.h, "demon_asse");

    // le serie trovate (moderno): una tacca sul bordo alto all'asse e alla pala, bianca quella agganciata
    if (!s.modo_1989) {
        const auto v = serie();
        for (std::size_t k = 0; k < v.size(); ++k) {
            const auto col = static_cast<std::uint8_t>(agganciato_ && static_cast<int>(k) == serie_ ? colori::BIANCO : colori::GRIGIO);
            for (const double hz : {v[k].asse_hz, v[k].pala_hz}) {
                const int x = x_di(r, hz);
                t.linea_verticale(x, r.y, r.y + 2, col);
                t.pixel(x - 1, r.y, col);
                t.pixel(x + 1, r.y, col);
            }
        }
    }

    // i cursori armonici sopra tutto: l'asse e le armoniche in giallo (puntini), la pala e le sue in ciano (trattini)
    if (asse_hz_ > 0.0) {
        const auto tratteggio = [&](double hz, std::uint8_t col, int passo, int pieno) {
            const int x = x_di(r, hz);
            for (int y = r.y + 3; y < casc.y + casc.h; ++y)
                if ((y - r.y) % passo < pieno) t.pixel(x, y, col);
        };
        for (int k = 1; k < pale_ && k * asse_hz_ < fm; ++k) tratteggio(k * asse_hz_, colori::GIALLO, 3, 1);   // fino alla pala
        for (int k = 1; k * asse_hz_ * pale_ < fm; ++k) tratteggio(k * asse_hz_ * pale_, colori::CIANO_CHIARO, 4, 2);
    }

    // la lettura dei cursori, in basso
    char buf[96];
    if (asse_hz_ > 0.0)
        std::snprintf(buf, sizeof buf, "ASSE %.2f %s  %.0f GIRI  PALE %d  PALA %.1f %s%s", asse_hz_, s.unita, asse_hz_ * 60.0, pale_,
                      asse_hz_ * pale_, s.unita, agganciato_ ? "  AGG" : "");
    else
        std::snprintf(buf, sizeof buf, "%s 0-%.0f %s%s", s.nome, fm, s.unita, righe_[0].empty() ? "  NESSUN DATO" : "");
    f.scrivi(t, r.x + 2, r.y + r.h - ALTO_TESTO + 2, virgola(buf), static_cast<std::uint8_t>(asse_hz_ > 0.0 ? colori::GIALLO : testo), 1);
    t.imposta_ritaglio(prima);
    aree_ = nullptr;
}

}  // namespace m688
