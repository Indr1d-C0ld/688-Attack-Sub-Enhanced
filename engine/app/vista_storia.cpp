#include "vista_storia.hpp"

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

std::string ora_del_giorno(double s) {
    const double x = std::fmod(std::fmod(s, 86400.0) + 86400.0, 86400.0);
    char b[16];
    std::snprintf(b, sizeof b, "%02d:%02d:%02dZ", static_cast<int>(x / 3600.0), static_cast<int>(std::fmod(x, 3600.0) / 60.0),
                  static_cast<int>(std::fmod(x, 60.0)));
    return b;
}

}  // namespace

std::string VistaStoria::relativo(double secondi) {
    const int s = std::max(0, static_cast<int>(std::lround(secondi)));
    char b[32];
    if (s >= 3600) std::snprintf(b, sizeof b, "-%d H %d MIN", s / 3600, (s % 3600) / 60);
    else if (s >= 60) std::snprintf(b, sizeof b, "-%d MIN %d S", s / 60, s % 60);
    else std::snprintf(b, sizeof b, "-%d S", s);
    return b;
}

// --- comandi ---

void VistaStoria::segui_cursore() {
    const double finestra = (righe_ > 0 ? righe_ : 60) * passo();
    if (cursore_s_ < indietro_s_) indietro_s_ = cursore_s_;
    if (cursore_s_ > indietro_s_ + finestra - passo()) indietro_s_ = cursore_s_ - finestra + passo();
    indietro_s_ = std::max(0.0, indietro_s_);
}

void VistaStoria::cursore(int verso, bool grosso) {
    if (verso == 0) return;
    const double durata = livelli_.back().durata_s;
    cursore_s_ = std::clamp(cursore_s_ + (verso > 0 ? 1.0 : -1.0) * passo() * (grosso ? 10.0 : 1.0), 0.0, durata);
    segui_cursore();
}

void VistaStoria::cursore_a(double t, double ora) {
    cursore_s_ = std::clamp(ora - t, 0.0, livelli_.back().durata_s);
    segui_cursore();
}

void VistaStoria::risoluzione(int verso) {
    livello_ = std::clamp(livello_ + (verso > 0 ? 1 : -1), 0, static_cast<int>(livelli_.size()) - 1);
    segui_cursore();
}

void VistaStoria::adesso() {
    indietro_s_ = cursore_s_ = 0.0;
    comparsa_t_.reset();
}

std::optional<double> VistaStoria::cerca_comparsa(const StoriaCascata& s, double x, double ora, bool gradi) {
    livelli_ = s.parametri().livelli;
    std::optional<double> t = s.comparsa(x);
    if (gradi)   // il BTR non fa il giro a 0/360: una traccia a cavallo di 0 gradi si cerca anche dall'altra parte
        for (const double altro : {x + 360.0, x - 360.0})
            if (const auto u = s.comparsa(altro); u && (!t || *u < *t)) t = u;
    if (!t) return std::nullopt;
    comparsa_t_ = t;
    cursore_s_ = std::max(0.0, ora - *t);
    // la risoluzione che arriva fin li', poi la finestra col cursore a tre quarti
    while (livello_ < static_cast<int>(livelli_.size()) - 1 && livelli_[static_cast<std::size_t>(livello_)].durata_s < cursore_s_) ++livello_;
    indietro_s_ = std::max(0.0, cursore_s_ - 0.75 * (righe_ > 0 ? righe_ : 60) * passo());
    segui_cursore();
    return t;
}

// --- geometria e disegno ---

double VistaStoria::t_a(Rettangolo r, const StoriaCascata& s, int y, double ora) const {
    const double p = s.parametri().livelli[static_cast<std::size_t>(livello_)].passo_s;
    return ora - indietro_s_ - (y - (r.y + ALTO_TESTO)) * p;
}

int VistaStoria::y_di(Rettangolo r, const StoriaCascata& s, double t, double ora) const {
    const double p = s.parametri().livelli[static_cast<std::size_t>(livello_)].passo_s;
    return r.y + ALTO_TESTO + static_cast<int>(std::floor((ora - indietro_s_ - t) / p));
}

void VistaStoria::disegna(Tela& t, const Font& f, Rettangolo r, const StoriaCascata& s, double ora, const StileStoria& st,
                          std::vector<AreaAttiva>* aree) const {
    aree_ = aree;
    livelli_ = s.parametri().livelli;
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
    const Rettangolo c{r.x, r.y + ALTO_TESTO, r.w, r.h - ALTO_TESTO};
    righe_ = std::max(1, righe_ > 0 ? std::min(righe_, c.h) : c.h);   // il rettangolo piu' basso, per i comandi
    const double p = passo();
    const int testo = st.modo_1989 ? colori::VERDE_CHIARO : colori::BIANCO;

    // la finestra: dalla piu' vecchia, una riga per pixel, la piu' recente in alto
    const double a = ora - indietro_s_, da = a - c.h * p;
    const FinestraStoria w = s.finestra(da, a, livello_);
    const double x0 = st.x1 > st.x0 ? st.x0 : s.f0(), x1 = st.x1 > st.x0 ? st.x1 : s.f0() + s.df() * static_cast<double>(s.bin());
    const auto u = [&](float db) { return cascate::frazione(db, st.pavimento_db, st.tetto_db); };
    std::optional<cascate::Fosforo> fosforo;
    if (st.fosforo) fosforo.emplace(c.w, c.h);
    const auto y_di_t = [&](double tk) { return static_cast<int>(std::floor((a - tk) / p - 1e-9)); };   // il passo che comincia a t
    std::vector<float> px(static_cast<std::size_t>(c.w));
    std::vector<char> pieno(static_cast<std::size_t>(c.w));
    for (std::size_t k = 0; k < w.righe(); ++k) {
        const int y = y_di_t(w.t[k]);
        // la riga vale fino alla successiva: al passo di 1 s le righe della stazione (ogni 2 s) non lasciano buchi;
        // un buco vero (piu' di 4 s, o di un passo) resta nero
        int y_alto = y;
        if (k + 1 < w.righe()) y_alto = std::max(y_di_t(w.t[k + 1]) + 1, y - std::max(1, static_cast<int>(std::ceil(4.0 / p))) + 1);
        if (y < 0 || y_alto >= c.h) continue;
        for (int x = 0; x < c.w; ++x) {
            // i bin che cadono nel pixel: il massimo
            const double ua = x0 + (x1 - x0) * x / c.w, ub = x0 + (x1 - x0) * (x + 1) / c.w;
            int ia = static_cast<int>(std::floor((ua - s.f0()) / s.df())), ib = static_cast<int>(std::floor((ub - s.f0()) / s.df()));
            ib = std::max(ib, ia + 1);
            float v = 0.0f;
            bool dentro = false;
            for (int i = std::max(0, ia); i < std::min(ib, static_cast<int>(w.bin)); ++i) {
                v = std::max(v, w.valore(k, static_cast<std::size_t>(i)));
                dentro = true;
            }
            px[static_cast<std::size_t>(x)] = v;
            pieno[static_cast<std::size_t>(x)] = dentro;
        }
        if (!st.gradi) {   // lo spettro e' l'SNR grezzo: lo stesso normalizzatore a finestra divisa del LOFAR dal vivo
            std::vector<float> somma(px.size() + 1, 0.0f);
            std::vector<int> quanti(px.size() + 1, 0);
            for (std::size_t x = 0; x < px.size(); ++x) {
                somma[x + 1] = somma[x] + (pieno[x] ? px[x] : 0.0f);
                quanti[x + 1] = quanti[x] + (pieno[x] ? 1 : 0);
            }
            const auto tra = [&](int da_x, int a_x, float& s_, int& n_) {   // la somma dei pixel pieni in [da_x, a_x]
                da_x = std::max(da_x, 0);
                a_x = std::min(a_x, c.w - 1);
                if (da_x > a_x) return;
                s_ += somma[static_cast<std::size_t>(a_x + 1)] - somma[static_cast<std::size_t>(da_x)];
                n_ += quanti[static_cast<std::size_t>(a_x + 1)] - quanti[static_cast<std::size_t>(da_x)];
            };
            std::vector<float> norm(px.size());
            for (int x = 0; x < c.w; ++x) {
                float s_ = 0.0f;
                int n_ = 0;
                tra(x - 24, x - 4, s_, n_);
                tra(x + 4, x + 24, s_, n_);
                norm[static_cast<std::size_t>(x)] = px[static_cast<std::size_t>(x)] - (n_ ? s_ / static_cast<float>(n_) : 0.0f);
            }
            px = std::move(norm);
        }
        for (int x = 0; x < c.w; ++x) {
            if (!pieno[static_cast<std::size_t>(x)]) continue;
            const double q = u(px[static_cast<std::size_t>(x)]);
            for (int yy = std::max(y_alto, 0); yy <= std::min(y, c.h - 1); ++yy) {
                if (fosforo) fosforo->deponi(x, yy, q);
                else if (q > 0.0) t.pixel(c.x + x, c.y + yy, cascate::indice(q, c.x + x, c.y + yy, st.scala));
            }
        }
    }
    if (fosforo) fosforo->componi(t, c.x, c.y, st.scala, cascate::Fosforo::Effetti{});
    area(c.x, c.y, c.w, c.h, "storia_tempo");

    // la comparsa trovata: una linea bianca tratteggiata
    if (comparsa_t_) {
        const int y = y_di(r, s, *comparsa_t_, ora);
        if (y >= c.y && y < c.y + c.h)
            for (int x = c.x; x < c.x + c.w; x += 3) t.pixel(x, y, colori::BIANCO);
    }
    // il cursore del tempo: una linea gialla a puntini, fuori diretta
    const bool diretta = in_diretta();
    if (!diretta) {
        const int y = y_di(r, s, t_cursore(ora), ora);
        if (y >= c.y && y < c.y + c.h)
            for (int x = c.x; x < c.x + c.w; x += 2) t.pixel(x, y, colori::GIALLO);
        t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, colori::GIALLO);   // il bordo: non e' la diretta
        t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, colori::GIALLO);
        t.linea_verticale(r.x, r.y, r.y + r.h - 1, colori::GIALLO);
        t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, colori::GIALLO);
    }

    // la riga del tempo, in alto
    char b[48];
    std::snprintf(b, sizeof b, "1 RIGA = %.0f S", p);
    std::string riga = diretta ? std::string("DIRETTA  ") + b : std::string("STORIA  ") + b + "  " +
                                     (st.ora_utc_s >= 0.0 ? ora_del_giorno(st.ora_utc_s - cursore_s_) + "  " : std::string()) +
                                     relativo(cursore_s_);
    if (comparsa_t_) riga += "  COMPARSA " + relativo(ora - *comparsa_t_);
    f.scrivi(t, r.x + 2, r.y + 1, riga, static_cast<std::uint8_t>(diretta ? testo : colori::GIALLO), 1);
    if (!diretta) {
        const int wa = f.larghezza("ADESSO", 1);
        f.scrivi(t, r.x + r.w - 3 - wa, r.y + 1, "ADESSO", colori::GRIGIO, 1);
        area(r.x + r.w - 5 - wa, r.y, wa + 4, ALTO_TESTO, "storia_adesso");
    }
    t.imposta_ritaglio(prima);
    aree_ = nullptr;
}

}  // namespace m688
