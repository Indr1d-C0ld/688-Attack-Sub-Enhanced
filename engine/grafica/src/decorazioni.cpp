#include "grafica/decorazioni.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <stdexcept>

#include "nucleo/geometria.hpp"

#include <toml.hpp>

namespace m688 {

using namespace cerimonia;

namespace {

using F = FormaMedaglia;
using M = Marina;

const std::map<std::string, FormaMedaglia>& forme() {
    static const std::map<std::string, FormaMedaglia> m = {
        {"nessuna", F::nessuna}, {"distintivo", F::distintivo}, {"attestato", F::attestato}, {"croce_patente", F::croce_patente},
        {"stella_capovolta", F::stella_capovolta}, {"medaglione", F::medaglione}, {"stella_con_corona", F::stella_con_corona},
        {"stella", F::stella}, {"legione", F::legione}, {"ottagono", F::ottagono}, {"cuore", F::cuore},
        {"stella_raggiata", F::stella_raggiata}, {"esagono", F::esagono}, {"quadrato", F::quadrato}, {"tondo", F::tondo},
        {"stella_oro", F::stella_oro}, {"ordine_lenin", F::ordine_lenin}, {"bandiera_rossa", F::bandiera_rossa},
        {"stella_rossa", F::stella_rossa}, {"servizio_patria", F::servizio_patria}, {"stella_otto", F::stella_otto},
        {"ordine_usakov", F::ordine_usakov}};
    return m;
}

std::array<std::uint8_t, 3> esadecimale(const std::string& s) {
    if (s.size() != 7 || s[0] != '#') throw std::runtime_error("decorazioni: colore non valido " + s);
    std::array<std::uint8_t, 3> c{};
    for (int i = 0; i < 3; ++i) c[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(std::stoi(s.substr(static_cast<std::size_t>(1 + 2 * i), 2), nullptr, 16));
    return c;
}

// --- distanze con segno (pixel) ---
struct P {
    double x, y;
};
double sd_cerchio(P p, double r) { return std::hypot(p.x, p.y) - r; }
double sd_poligono(const std::vector<P>& v, P p) {
    double d = (p.x - v[0].x) * (p.x - v[0].x) + (p.y - v[0].y) * (p.y - v[0].y), s = 1;
    for (std::size_t i = 0, j = v.size() - 1; i < v.size(); j = i, ++i) {
        const P e{v[j].x - v[i].x, v[j].y - v[i].y}, w{p.x - v[i].x, p.y - v[i].y};
        const double k = std::clamp((w.x * e.x + w.y * e.y) / (e.x * e.x + e.y * e.y), 0.0, 1.0);
        const P b{w.x - e.x * k, w.y - e.y * k};
        d = std::min(d, b.x * b.x + b.y * b.y);
        const bool c1 = p.y >= v[i].y, c2 = p.y < v[j].y, c3 = e.x * w.y > e.y * w.x;
        if ((c1 && c2 && c3) || (!c1 && !c2 && !c3)) s = -s;
    }
    return s * std::sqrt(d);
}
// stella a n punte: raggi esterno e interno, rotazione (0 = una punta in alto)
std::vector<P> stella(int n, double re, double ri, double rot = 0.0) {
    std::vector<P> v;
    for (int k = 0; k < 2 * n; ++k) {
        const double a = rot + PI * k / n, r = k % 2 == 0 ? re : ri;
        v.push_back({r * std::sin(a), -r * std::cos(a)});
    }
    return v;
}
std::vector<P> regolare(int n, double r, double rot = 0.0) {
    std::vector<P> v;
    for (int k = 0; k < n; ++k) {
        const double a = rot + 2 * PI * k / n;
        v.push_back({r * std::sin(a), -r * std::cos(a)});
    }
    return v;
}

// --- rilievo della medaglia: altezza e materiale per pixel, poi la luce ---
class Rilievo {
public:
    Rilievo(int w, int h) : w_(w), h_(h), alt_(static_cast<std::size_t>(w) * h, 0.0), mat_(static_cast<std::size_t>(w) * h, -1) {}
    // forma con il bordo smussato: altezza base + rilievo sul bordo largo `smusso` pixel
    void forma(const std::function<double(P)>& sd, double base, double rilievo, double smusso, int rampa, double cx, double cy,
               bool solo_sopra = false) {
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const double d = sd({x + 0.5 - cx, y + 0.5 - cy});
                if (d > 0) continue;
                const std::size_t i = static_cast<std::size_t>(y) * w_ + x;
                if (solo_sopra && mat_[i] < 0) continue;
                alt_[i] = base + rilievo * std::min(-d / smusso, 1.0);
                mat_[i] = rampa;
            }
    }
    // incisione: abbassa (o alza) l'altezza lungo un tratto, senza cambiare materiale
    void segno(const std::function<double(P)>& sd, double delta, double cx, double cy) {
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const std::size_t i = static_cast<std::size_t>(y) * w_ + x;
                if (mat_[i] < 0) continue;
                if (sd({x + 0.5 - cx, y + 0.5 - cy}) < 0) alt_[i] += delta;
            }
    }
    void disegna(Tela& t, int ox, int oy) const {
        const double lx = -0.55, ly = -0.65, lz = 0.85;   // luce da sinistra in alto
        const double ln = std::sqrt(lx * lx + ly * ly + lz * lz);
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const std::size_t i = static_cast<std::size_t>(y) * w_ + x;
                const int m = mat_[i];
                if (m < 0) continue;
                auto a = [&](int xx, int yy) {
                    xx = std::clamp(xx, 0, w_ - 1);
                    yy = std::clamp(yy, 0, h_ - 1);
                    const std::size_t j = static_cast<std::size_t>(yy) * w_ + xx;
                    return mat_[j] < 0 ? alt_[i] - 1.2 : alt_[j];
                };
                const double gx = (a(x + 1, y) - a(x - 1, y)) / 2, gy = (a(x, y + 1) - a(x, y - 1)) / 2;
                const double nx = -gx, ny = -gy, nz = 1.0, nn = std::sqrt(nx * nx + ny * ny + nz * nz);
                const double l = (nx * lx + ny * ly + nz * lz) / (nn * ln);
                const double spec = std::pow(std::max(0.0, l), 12.0);
                const int n = numero(m);
                double tono = (0.15 + 0.7 * std::max(0.0, l)) * (n - 1) + spec * 0.9 * (n - 1) * 0.5;
                // bordo verso il vuoto: un filo scuro
                bool bordo = false;
                for (const auto& [dx, dy] : {std::pair{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
                    const int xx = x + dx, yy = y + dy;
                    if (xx < 0 || yy < 0 || xx >= w_ || yy >= h_ || mat_[static_cast<std::size_t>(yy) * w_ + xx] < 0) bordo = true;
                }
                if (bordo) tono = std::min(tono, 0.6);
                t.pixel(ox + x, oy + y, static_cast<std::uint8_t>(m + retina(tono, ox + x, oy + y, n - 1)));
            }
    }
    static int numero(int rampa) {
        switch (rampa) {
            case ORO: return N_ORO;
            case ARGENTO: return N_ARGENTO;
            case BRONZO: return N_BRONZO;
            case SMALTO: return N_SMALTO;
            case SMALTO_BLU: return N_SMALTO_BLU;
            case BIANCO: return N_BIANCO;
            case VERDE: return N_VERDE;
            case VIOLA: return N_VIOLA;
            default: return 1;
        }
    }

private:
    int w_, h_;
    std::vector<double> alt_;
    std::vector<int> mat_;
};

// nastro a strisce in un poligono (o rettangolo), con le pieghe in ombra retinata
void nastro(Tela& t, const DisegnoDecorazione& d, int x0, int y0, int w, int h, const std::function<bool(int, int)>& dentro,
            bool pieghe) {
    double tot = 0;
    for (const auto& s : d.nastro) tot += s.larghezza;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            if (!dentro(x, y)) continue;
            const double u = d.orizzontale ? (y + 0.5) / h * tot : (x + 0.5) / w * tot;
            double acc = 0;
            int col = d.nastro.front().colore;
            for (const auto& s : d.nastro) {
                acc += s.larghezza;
                col = s.colore;
                if (u < acc) break;
            }
            if (d.stelle && ((x / 3 + y / 3) % 3 == 0) && x % 3 == 1 && y % 3 == 1) col = BIANCO_N;
            if (pieghe) {
                // ombra ai bordi e lungo una piega diagonale: punti neri retinati
                const double e = std::min(x, w - 1 - x) / std::max(1.0, 0.12 * w);
                const double piega = std::fabs((x - 0.5 * w) - 0.25 * (y - 0.5 * h)) / std::max(1.0, 0.04 * w);
                const double ombra = std::max(e < 1 ? 0.45 * (1 - e) : 0.0, piega < 1 ? 0.25 * (1 - piega) : 0.0) + (y > h - 3 ? 0.3 : 0.0);
                if (bayer(x0 + x, y0 + y) < ombra) col = NERO_N;
            }
            t.pixel(x0 + x, y0 + y, static_cast<std::uint8_t>(col));
        }
}

void stellina(Tela& t, int cx, int cy, int r, int col) {
    for (int y = -r; y <= r; ++y)
        for (int x = -r; x <= r; ++x)
            if (std::abs(x) + std::abs(y) <= r || (r >= 2 && (x == 0 || y == 0) && std::abs(x) + std::abs(y) <= r + 1))
                t.pixel(cx + x, cy + y, static_cast<std::uint8_t>(col));
}

}  // namespace

TavolaDecorazioni TavolaDecorazioni::carica(const RegoleCarriera& regole, const std::string& percorso) {
    toml::table g;
    try {
        g = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    TavolaDecorazioni t;
    for (const auto& [id, dc] : regole.decorazioni) {
        const auto* tg = g[id].as_table();
        if (!tg) { t.solo_carriera_.push_back(id); continue; }
        DisegnoDecorazione d;
        d.id = id;
        d.nome = dc.nome_it.empty() ? dc.nome : dc.nome_it;
        d.tipo = dc.tipo;
        d.marina = dc.marina == "vmf" ? Marina::vmf : Marina::usn;
        d.precedenza = dc.precedenza;
        d.orizzontale = dc.orizzontale;
        d.stelle = dc.motivo == "stelle_bianche";
        for (const auto& r : dc.righe) {
            const auto c = esadecimale(r.colore);
            std::size_t k = 0;
            while (k < t.colori_.size() && t.colori_[k] != c) ++k;
            if (k == t.colori_.size()) {
                if (t.colori_.size() < static_cast<std::size_t>(N_NASTRO)) t.colori_.push_back(c);
                else {   // posti finiti: il colore piu' vicino
                    double best = 1e18;
                    for (std::size_t j = 0; j < t.colori_.size(); ++j) {
                        double dd = 0;
                        for (int q = 0; q < 3; ++q) dd += std::pow(t.colori_[j][static_cast<std::size_t>(q)] - c[static_cast<std::size_t>(q)], 2.0);
                        if (dd < best) { best = dd; k = j; }
                    }
                }
            }
            d.nastro.push_back({NASTRO + static_cast<int>(k), static_cast<double>(r.larghezza)});
        }
        const std::string forma = (*tg)["forma"].value_or(std::string("nessuna"));
        const auto f = forme().find(forma);
        if (f == forme().end()) throw std::runtime_error(percorso + ": forma sconosciuta " + forma + " (" + id + ")");
        d.forma = f->second;
        const std::string metallo = (*tg)["metallo"].value_or(std::string("bronzo"));
        if (metallo == "oro") d.metallo = ORO;
        else if (metallo == "argento") d.metallo = ARGENTO;
        else if (metallo == "bronzo") d.metallo = BRONZO;
        else throw std::runtime_error(percorso + ": metallo sconosciuto " + metallo + " (" + id + ")");
        d.classe = static_cast<int>((*tg)["classe"].value_or(0));
        d.a_vite = (*tg)["a_vite"].value_or(false);
        const std::string lettera = (*tg)["lettera"].value_or(std::string());
        d.lettera = lettera.empty() ? 0 : lettera[0];
        t.d_.push_back(std::move(d));
    }
    for (const auto& [id, v] : g)
        if (!regole.decorazioni.count(std::string(id.str()))) t.solo_grafica_.push_back(std::string(id.str()));
    std::stable_sort(t.d_.begin(), t.d_.end(), [](const DisegnoDecorazione& a, const DisegnoDecorazione& b) {
        return a.marina != b.marina ? a.marina < b.marina : a.precedenza < b.precedenza;
    });
    return t;
}

void TavolaDecorazioni::imposta_palette(Palette& p) const {
    for (std::size_t i = 0; i < colori_.size(); ++i) p.imposta(NASTRO + static_cast<int>(i), colori_[i][0], colori_[i][1], colori_[i][2]);
}

const DisegnoDecorazione* TavolaDecorazioni::decorazione(std::string_view id) const {
    for (const auto& d : d_)
        if (d.id == id) return &d;
    return nullptr;
}

void TavolaDecorazioni::disegna_nastrino(Tela& t, int x, int y, int w, int h, const VoceBarretta& n) const {
    const auto* d = decorazione(n.decorazione);
    if (!d || w <= 0 || h <= 0 || d->nastro.empty()) return;
    nastro(t, *d, x, y, w, h, [](int, int) { return true; }, false);
    // dispositivi: stelline d'oro per i conferimenti successivi (d'argento per cinque), «V», lettera
    const int extra = std::max(0, n.numero - 1);
    const int argento = extra / 5, oro = extra % 5;
    const bool v = n.dispositivo == "v";
    const int r = std::max(1, h / 4);
    int k = 0;
    const int tot = argento + oro + (v ? 1 : 0) + (d->lettera ? 1 : 0);
    auto posto = [&]() { return x + w / 2 + (2 * k++ - (tot - 1)) * (2 * r + 2) / 2; };
    if (d->lettera) {
        const int cx = posto();
        for (int yy = y + 1; yy < y + h - 1; ++yy) t.pixel(cx - 1, yy, ORO + 6);
        for (const int yy : {y + 1, y + h / 2, y + h - 2})
            for (int xx = cx - 1; xx <= cx + 1; ++xx) t.pixel(xx, yy, ORO + 6);
    }
    if (v) {
        const int cx = posto();
        for (int i = 0; i < h - 1; ++i) {
            t.pixel(cx - (h - 1 - i) / 2, y + i, BRONZO + 4);
            t.pixel(cx + (h - 1 - i) / 2, y + i, BRONZO + 4);
        }
    }
    for (int i = 0; i < argento; ++i) stellina(t, posto(), y + h / 2, r, ARGENTO + 5);
    for (int i = 0; i < oro; ++i) stellina(t, posto(), y + h / 2, r, ORO + 6);
}

Rettangolo misura_barretta(std::size_t n, int per_fila, int w, int h) {
    if (n == 0 || per_fila <= 0) return {};
    const int file = static_cast<int>((n + per_fila - 1) / per_fila);
    const int colonne = static_cast<int>(std::min<std::size_t>(n, static_cast<std::size_t>(per_fila)));
    return {0, 0, colonne * w, file * h};
}

Rettangolo TavolaDecorazioni::disegna_barretta(Tela& t, int cx, int y_basso, const std::vector<VoceBarretta>& tutte_, int per_fila, int w,
                                               int h) const {
    std::vector<VoceBarretta> ns;   // solo i nastrini (i distintivi stanno sull'uniforme)
    for (const auto& v : tutte_) {
        const auto* d = decorazione(v.decorazione);
        if (d && !d->nastro.empty() && d->forma != F::distintivo) ns.push_back(v);
    }
    const auto m = misura_barretta(ns.size(), per_fila, w, h);
    if (m.w == 0) return m;
    const int n = static_cast<int>(ns.size());
    const int file = (n + per_fila - 1) / per_fila;
    const int prima = n - (file - 1) * per_fila;   // la fila in alto, incompleta, con le decorazioni piu' alte
    int i = 0;
    for (int f = 0; f < file; ++f) {
        const int quanti = f == 0 ? prima : per_fila;
        const int y = y_basso - (file - f) * h;
        const int x0 = cx - quanti * w / 2;
        for (int k = 0; k < quanti; ++k, ++i) disegna_nastrino(t, x0 + k * w, y, w, h, ns[static_cast<std::size_t>(i)]);
    }
    return {cx - m.w / 2, y_basso - m.h, m.w, m.h};
}

void TavolaDecorazioni::disegna_medaglia(Tela& t, Rettangolo r, std::string_view id) const {
    const auto* d = decorazione(id);
    if (!d) return;
    const int W = r.w, H = r.h;
    const double cx = W / 2.0;
    // solo nastrino: ingrandito con la cornice dorata
    if (d->forma == F::nessuna) {
        const int nw = static_cast<int>(W * 0.86), nh = std::max(8, static_cast<int>(nw * 10.0 / 35.0));
        const int x0 = r.x + (W - nw) / 2, y0 = r.y + (H - nh) / 2;
        for (int y = -3; y < nh + 3; ++y)
            for (int x = -3; x < nw + 3; ++x)
                if (x < 0 || y < 0 || x >= nw || y >= nh) t.pixel(x0 + x, y0 + y, static_cast<std::uint8_t>(ORO + (x + y < 0 || y < 0 ? 6 : 2)));
        VoceBarretta n;
        n.decorazione = d->id;
        disegna_nastrino(t, x0, y0, nw, nh, n);
        return;
    }
    if (d->forma == F::attestato) {
        // foglio incorniciato con le righe e il sigillo d'oro
        const int fw = static_cast<int>(W * 0.82), fh = static_cast<int>(std::min(H * 0.9, fw * 1.3));
        const int x0 = r.x + (W - fw) / 2, y0 = r.y + (H - fh) / 2;
        for (int y = 0; y < fh; ++y)
            for (int x = 0; x < fw; ++x) {
                const int b = std::min({x, y, fw - 1 - x, fh - 1 - y});
                int col = b < 3 ? ORO + (b == 1 ? 6 : 2) : CARTA + retina(3.6 + 0.6 * caso(x, y, 4) - 0.8 * (b < 6), x0 + x, y0 + y, N_CARTA - 1);
                if (b >= 8 && y > fh * 0.2 && y < fh * 0.62 && (y - static_cast<int>(fh * 0.2)) % 7 == 0 && x > fw * 0.12 && x < fw * 0.88 && caso(x / 5, y, 2) < 0.85)
                    col = CARTA + 0;
                t.pixel(x0 + x, y0 + y, static_cast<std::uint8_t>(col));
            }
        Rilievo sig(30, 30);
        sig.forma([](P p) { return sd_poligono(stella(16, 13, 11), p); }, 0.0, 2.0, 2.0, d->marina == M::vmf ? SMALTO : ORO, 15, 15);
        sig.forma([](P p) { return sd_cerchio(p, 8); }, 2.0, 1.0, 1.5, d->marina == M::vmf ? SMALTO : ORO, 15, 15);
        sig.disegna(t, x0 + fw / 2 - 15, y0 + static_cast<int>(fh * 0.72));
        return;
    }
    const bool sovietica = d->marina == M::vmf;
    const bool con_nastro = !d->a_vite && d->forma != F::stella_oro && d->forma != F::distintivo && !d->nastro.empty();
    // nastro: USA un drappo rettangolare con la barretta, URSS il pentagono piegato; la Stella d'oro una barretta rossa
    // misure: drappo USA alto 1,3 volte la larghezza, pentagono sovietico alto quanto largo; il gruppo e' centrato
    const double k_nastro = con_nastro ? (sovietica ? 1.25 * 1.15 : 1.35 * 1.3 + 0.1) : 0.0;
    const double rm = std::min(W * 0.42, (H - 8) / (2.0 + k_nastro + (con_nastro ? 0.15 : 0.0)));   // raggio della medaglia
    const double alto_gruppo = (2.0 + k_nastro + (con_nastro ? 0.15 : 0.0)) * rm;
    const double y_gruppo = (H - alto_gruppo) / 2.0;
    const double my = con_nastro ? y_gruppo + alto_gruppo - rm : H / 2.0 + (d->forma == F::stella_oro ? H * 0.08 : 0);
    if (con_nastro) {
        const int nw = static_cast<int>(sovietica ? rm * 1.25 : rm * 1.35);
        const int nh = static_cast<int>(sovietica ? nw * 1.15 : nw * 1.3 + 4);
        const int x0 = r.x + static_cast<int>(cx) - nw / 2, y0 = r.y + static_cast<int>(y_gruppo);
        if (sovietica) {
            const double punta = 0.5 * nw;   // pentagono: rettangolo e due lati che scendono a punta
            nastro(t, *d, x0, y0, nw, nh, [&](int x, int y) {
                const double sotto = y + 0.5 - (nh - punta);
                return sotto <= 0 || std::fabs(x + 0.5 - 0.5 * nw) <= (punta - sotto) * (0.5 * nw) / punta;
            }, true);
        } else {
            nastro(t, *d, x0, y0 + 4, nw, nh - 4, [](int, int) { return true; }, true);
            for (int x = x0 - 2; x < x0 + nw + 2; ++x)   // barretta di sospensione
                for (int y = y0; y < y0 + 4; ++y) t.pixel(x, y, static_cast<std::uint8_t>(ORO + (y == y0 ? 7 : y == y0 + 3 ? 1 : 4)));
        }
        // anello
        for (int k = 0; k < 40; ++k) {
            const double a = k * 2 * PI / 40;
            t.pixel(r.x + static_cast<int>(cx + 3 * std::cos(a)), r.y + static_cast<int>(my - rm - 2 + 3 * std::sin(a)),
                    static_cast<std::uint8_t>(d->metallo + (std::sin(a) < 0 ? 4 : 1)));
        }
    } else if (d->forma == F::stella_oro) {
        // barretta rossa con la cornice dorata
        const int bw = static_cast<int>(rm * 0.9), bh = static_cast<int>(rm * 0.38);
        const int x0 = r.x + static_cast<int>(cx) - bw / 2, y0 = r.y + static_cast<int>(my - rm * 1.05 - bh);
        for (int y = 0; y < bh; ++y)
            for (int x = 0; x < bw; ++x) {
                const bool bordo = x < 2 || y < 2 || x >= bw - 2 || y >= bh - 2;
                t.pixel(x0 + x, y0 + y, static_cast<std::uint8_t>(bordo ? ORO + (y < 2 || x < 2 ? 6 : 2) : SMALTO + 3));
            }
    }

    // corpo della medaglia
    const int L = static_cast<int>(2 * rm + 8);
    Rilievo rl(L, L);
    const double c = L / 2.0;
    const int me = d->metallo;
    auto cerchio = [](double rr) { return [rr](P p) { return sd_cerchio(p, rr); }; };
    auto poligono = [](std::vector<P> v) { return [v](P p) { return sd_poligono(v, p); }; };
    const double R = rm;
    switch (d->forma) {
        case F::croce_patente: {
            for (int k = 0; k < 4; ++k) {
                const double a = k * PI / 2;
                auto ruota = [a](P p) { return P{p.x * std::cos(a) + p.y * std::sin(a), -p.x * std::sin(a) + p.y * std::cos(a)}; };
                const std::vector<P> braccio = {{0, -0.16 * R}, {0.92 * R, -0.42 * R}, {R, 0}, {0.92 * R, 0.42 * R}, {0, 0.16 * R}};
                rl.forma([=](P p) { return sd_poligono(braccio, ruota(p)); }, 0.0, 3.0, 2.5, me, c, c);
            }
            rl.forma(cerchio(0.3 * R), 2.5, 1.5, 2.0, me, c, c);   // la caravella nel tondo
            rl.segno(poligono({{-0.18 * R, 0.05 * R}, {0.18 * R, 0.05 * R}, {0.1 * R, 0.14 * R}, {-0.1 * R, 0.14 * R}}), 1.5, c, c);
            rl.segno(poligono({{-0.02 * R, -0.2 * R}, {0.02 * R, -0.2 * R}, {0.02 * R, 0.05 * R}, {-0.02 * R, 0.05 * R}}), 1.5, c, c);
            for (int k = 0; k < 4; ++k) {   // alloro fra le braccia
                const double a = PI / 4 + k * PI / 2;
                rl.forma(cerchio(0.12 * R), 0.5, 1.5, 1.5, me, c + 0.45 * R * std::cos(a), c + 0.45 * R * std::sin(a));
            }
            break;
        }
        case F::stella_capovolta: {
            rl.forma(poligono(stella(5, R, 0.45 * R, PI)), 0.0, 3.0, 3.0, BRONZO, c, c + 0.05 * R);
            for (int k = 0; k < 5; ++k) {   // trifogli sulle punte
                const double a = PI + k * 2 * PI / 5;
                rl.forma(cerchio(0.12 * R), 2.0, 1.0, 1.5, BRONZO, c + 0.86 * R * std::sin(a), c + 0.05 * R - 0.86 * R * std::cos(a));
            }
            rl.forma(cerchio(0.3 * R), 2.5, 1.5, 2.0, BRONZO, c, c + 0.05 * R);
            rl.forma(cerchio(0.22 * R), 3.0, 0.5, 1.0, ORO, c, c + 0.05 * R);
            break;
        }
        case F::medaglione:
            rl.forma(cerchio(R), 0.0, 2.0, 2.0, ORO, c, c);
            rl.forma(cerchio(0.72 * R), 1.5, 0.6, 1.0, SMALTO_BLU, c, c);
            rl.forma(cerchio(0.5 * R), 2.0, 1.5, 2.0, ORO, c, c);
            rl.forma(poligono(stella(5, 0.25 * R, 0.1 * R)), 3.0, 1.0, 1.0, BIANCO, c, c - 0.82 * R);
            break;
        case F::stella_con_corona:
            rl.forma(poligono(stella(5, R, 0.42 * R)), 0.0, 3.0, 4.0, ORO, c, c);
            rl.forma([&](P p) { return std::fabs(sd_cerchio(p, 0.3 * R)) - 0.06 * R; }, 2.5, 1.0, 1.0, ORO, c, c);
            rl.forma(poligono(stella(5, 0.16 * R, 0.07 * R)), 3.0, 1.0, 1.0, ARGENTO, c, c);
            break;
        case F::stella:
            rl.forma(poligono(stella(5, R, 0.42 * R)), 0.0, 3.0, 4.0, me, c, c);
            rl.forma(poligono(stella(5, 0.22 * R, 0.1 * R)), 3.0, 1.0, 1.0, me, c, c);
            break;
        case F::legione: {
            rl.forma([&](P p) { return std::fabs(sd_cerchio(p, 0.62 * R)) - 0.14 * R; }, 0.0, 2.0, 2.0, VERDE, c, c);   // corona
            rl.forma(poligono(stella(5, R, 0.5 * R)), 1.0, 2.0, 2.0, SMALTO, c, c);
            rl.forma(poligono(stella(5, 0.9 * R, 0.42 * R)), 2.0, 1.5, 2.0, BIANCO, c, c);
            for (int k = 0; k < 5; ++k) {   // bracci forcuti: una tacca in punta
                const double a = k * 2 * PI / 5;
                rl.forma(cerchio(0.1 * R), -5.0, 0.0, 1.0, SMALTO, c + 0.92 * R * std::sin(a), c - 0.92 * R * std::cos(a), true);
            }
            rl.forma(cerchio(0.28 * R), 3.0, 0.8, 1.0, SMALTO_BLU, c, c);
            for (int k = 0; k < 13; ++k) {
                const double a = k * 2 * PI / 13;
                rl.forma(cerchio(0.035 * R), 4.0, 0.2, 0.5, BIANCO, c + 0.18 * R * std::sin(a), c - 0.18 * R * std::cos(a));
            }
            break;
        }
        case F::ottagono:
            rl.forma(poligono(regolare(8, R, PI / 8)), 0.0, 2.0, 2.0, me, c, c);
            rl.forma(poligono({{-0.08 * R, -0.5 * R}, {0.08 * R, -0.5 * R}, {0.08 * R, 0.4 * R}, {-0.08 * R, 0.4 * R}}), 2.0, 1.5, 1.0, me, c, c);
            rl.forma([&](P p) { return std::max(std::fabs(sd_cerchio({p.x, p.y + 0.1 * R}, 0.42 * R)) - 0.06 * R, -p.y - 0.0); }, 2.0, 1.5, 1.0, me, c, c);
            rl.forma(poligono({{-0.45 * R, -0.35 * R}, {0.45 * R, -0.35 * R}, {0.2 * R, -0.12 * R}, {-0.2 * R, -0.12 * R}}), 2.5, 1.5, 1.0, me, c, c);   // aquila
            break;
        case F::cuore: {
            auto cuore = [](double s) {
                return [s](P q) {
                    const P p{std::fabs(q.x) / s, (0.85 * s - q.y) / s * 1.0};   // y in su
                    if (p.y + p.x > 1.0) return (std::hypot(p.x - 0.25, p.y - 0.75) - std::sqrt(2.0) / 4.0) * s;
                    const double a = std::hypot(p.x, p.y - 1.0), b = std::hypot(p.x - 0.5 * std::max(p.x + p.y, 0.0), p.y - 0.5 * std::max(p.x + p.y, 0.0));
                    return std::sqrt(std::min(a * a, b * b)) * (p.x - p.y > 0 ? 1 : -1) * s;
                };
            };
            rl.forma(cuore(1.05 * R), 0.0, 2.0, 2.0, ORO, c, c);
            rl.forma(cuore(0.86 * R), 1.0, 1.5, 2.5, VIOLA, c, c + 0.04 * R);
            rl.forma(cerchio(0.24 * R), 2.0, 1.5, 2.0, ORO, c, c - 0.05 * R);   // profilo di Washington
            rl.forma(poligono({{-0.35 * R, -1.0 * R}, {0.35 * R, -1.0 * R}, {0.25 * R, -0.78 * R}, {-0.25 * R, -0.78 * R}}), 1.0, 1.5, 1.0, ORO, c, c);
            break;
        }
        case F::stella_raggiata:
            rl.forma(poligono(stella(13, R, 0.72 * R)), 0.0, 2.0, 2.0, me, c, c);
            rl.forma(cerchio(0.52 * R), 1.5, 1.0, 1.5, me, c, c);
            rl.forma(poligono({{-0.5 * R, -0.1 * R}, {0.5 * R, -0.1 * R}, {0.12 * R, 0.15 * R}, {0.1 * R, 0.4 * R}, {-0.1 * R, 0.4 * R}, {-0.12 * R, 0.15 * R}}), 2.5, 1.5, 1.0, me, c, c);
            break;
        case F::esagono:
            rl.forma(poligono(regolare(6, R, PI / 6)), 0.0, 2.0, 2.5, me, c, c);
            rl.forma(poligono(regolare(6, 0.72 * R, PI / 6)), 1.0, 0.8, 1.0, me, c, c);
            rl.forma(poligono({{-0.45 * R, -0.15 * R}, {0.45 * R, -0.15 * R}, {0.12 * R, 0.1 * R}, {0.1 * R, 0.4 * R}, {-0.1 * R, 0.4 * R}, {-0.12 * R, 0.1 * R}}), 2.0, 1.5, 1.0, me, c, c);
            break;
        case F::quadrato:
            rl.forma([&](P p) { const double dx = std::fabs(p.x) - 0.8 * R, dy = std::fabs(p.y) - 0.8 * R;
                                return std::hypot(std::max(dx, 0.0), std::max(dy, 0.0)) + std::min(std::max(dx, dy), 0.0) - 0.15 * R; },
                     0.0, 2.0, 2.0, me, c, c);
            rl.forma(poligono({{-0.07 * R, -0.55 * R}, {0.07 * R, -0.55 * R}, {0.07 * R, 0.45 * R}, {-0.07 * R, 0.45 * R}}), 2.0, 1.5, 1.0, me, c, c);
            rl.forma([&](P p) { return std::max(std::fabs(sd_cerchio({p.x, p.y + 0.05 * R}, 0.45 * R)) - 0.07 * R, -p.y); }, 2.0, 1.5, 1.0, me, c, c);
            break;
        case F::tondo:
            rl.forma(cerchio(R), 0.0, 2.0, 2.0, me, c, c);
            rl.forma([&](P p) { return std::fabs(sd_cerchio(p, 0.86 * R)) - 0.03 * R; }, 2.0, 0.6, 0.8, me, c, c);
            if (sovietica) {
                rl.forma(poligono(stella(5, 0.4 * R, 0.17 * R)), 1.5, 1.5, 1.5, d->id.rfind("servizio_imp", 0) == 0 ? SMALTO : me, c, c - 0.1 * R);
                rl.forma([&](P p) { return std::max(std::fabs(sd_cerchio(p, 0.62 * R)) - 0.06 * R, -p.y); }, 1.5, 1.0, 1.0, me, c, c);
            } else {
                rl.forma(poligono({{-0.25 * R, -0.45 * R}, {0.25 * R, -0.45 * R}, {0.35 * R, 0.4 * R}, {-0.35 * R, 0.4 * R}}), 1.5, 1.0, 1.5, me, c, c);
            }
            break;
        case F::stella_oro: {
            // stella d'oro a facce: ogni punta ha due facce, una in luce e una in ombra
            const auto v = stella(5, R, 0.4 * R);
            rl.forma(poligono(v), 0.0, 0.0, 1.0, ORO, c, c);
            for (int k = 0; k < 10; ++k) {
                const P a{0, 0}, b = v[static_cast<std::size_t>(k)], e = v[static_cast<std::size_t>((k + 1) % 10)];
                rl.forma(poligono({a, b, e}), k % 2 == 0 ? 2.5 : 0.5, 0.0, 1.0, ORO, c, c);
            }
            break;
        }
        case F::ordine_lenin:
            rl.forma(cerchio(R), 0.0, 1.5, 2.0, ORO, c, c);
            for (int k = 0; k < 18; ++k) {   // spighe
                const double a = PI * (0.65 + 0.7 * k / 17.0) * (k % 2 ? 1 : -1);
                rl.forma(cerchio(0.1 * R), 2.0, 1.0, 1.0, ORO, c + 0.82 * R * std::sin(a), c - 0.82 * R * std::cos(a));
            }
            rl.forma(cerchio(0.6 * R), 1.5, 0.5, 1.0, ARGENTO, c, c);   // disco di platino
            rl.forma([&](P p) { return sd_cerchio({p.x * 1.3, p.y}, 0.38 * R); }, 2.5, 1.5, 2.0, ARGENTO, c + 0.04 * R, c);   // profilo
            rl.forma(poligono({{-0.9 * R, -0.25 * R}, {-0.45 * R, -0.45 * R}, {-0.4 * R, -0.3 * R}, {-0.85 * R, -0.1 * R}}), 3.0, 0.5, 1.0, SMALTO, c, c);   // bandiera
            rl.forma(poligono(stella(5, 0.18 * R, 0.08 * R)), 3.0, 1.0, 1.0, SMALTO, c, c - 0.85 * R);
            break;
        case F::bandiera_rossa:
            rl.forma([&](P p) { return std::max(std::fabs(sd_cerchio(p, 0.78 * R)) - 0.18 * R, -p.y - 0.2 * R); }, 0.0, 2.0, 1.5, BIANCO, c, c);   // alloro
            rl.forma(poligono(stella(5, 0.78 * R, 0.33 * R)), 1.5, 2.0, 2.5, SMALTO, c, c + 0.08 * R);
            rl.forma(cerchio(0.22 * R), 3.0, 1.0, 1.0, ORO, c, c + 0.08 * R);   // falce e martello
            rl.forma(poligono({{-0.75 * R, -0.95 * R}, {0.7 * R, -0.95 * R}, {0.5 * R, -0.62 * R}, {-0.6 * R, -0.62 * R}}), 2.0, 1.0, 1.0, SMALTO, c, c);   // bandiera
            rl.forma(poligono({{-0.6 * R, -0.86 * R}, {0.55 * R, -0.86 * R}, {0.5 * R, -0.8 * R}, {-0.55 * R, -0.8 * R}}), 3.0, 0.3, 0.6, ORO, c, c);
            break;
        case F::stella_rossa: {
            const auto v = stella(5, R, 0.42 * R);
            rl.forma(poligono(v), 0.0, 0.0, 1.0, SMALTO, c, c);
            for (int k = 0; k < 10; ++k) {   // costole delle punte smaltate
                rl.forma(poligono({{0, 0}, v[static_cast<std::size_t>(k)], v[static_cast<std::size_t>((k + 1) % 10)]}),
                         k % 2 == 0 ? 2.0 : 0.8, 0.0, 1.0, SMALTO, c, c);
            }
            rl.forma(cerchio(0.3 * R), 2.5, 1.5, 1.5, ARGENTO, c, c);
            rl.forma(poligono({{-0.05 * R, -0.2 * R}, {0.06 * R, -0.2 * R}, {0.1 * R, 0.2 * R}, {-0.08 * R, 0.2 * R}}), 4.0, 0.5, 0.8, ARGENTO, c, c);   // soldato
            rl.forma(poligono({{-0.24 * R, 0.24 * R}, {0.24 * R, 0.24 * R}, {0.2 * R, 0.29 * R}, {-0.2 * R, 0.29 * R}}), 3.5, 0.3, 0.5, SMALTO, c, c);
            break;
        }
        case F::servizio_patria: {
            rl.forma(poligono(stella(4, R, 0.3 * R, PI / 4)), 0.0, 2.0, 2.0, SMALTO_BLU, c, c);   // stella dietro, smaltata
            rl.forma(poligono(stella(4, 0.9 * R, 0.28 * R)), 1.5, 2.0, 2.0, d->classe == 3 ? ARGENTO : ORO, c, c);
            rl.forma(cerchio(0.36 * R), 3.0, 1.0, 1.5, d->classe == 1 ? ORO : ARGENTO, c, c);
            rl.forma(poligono(stella(5, 0.2 * R, 0.09 * R)), 4.0, 0.8, 1.0, SMALTO, c, c);
            break;
        }
        case F::stella_otto:
            rl.forma(poligono(stella(8, R, 0.55 * R)), 0.0, 2.5, 3.0, ORO, c, c);
            rl.forma(cerchio(0.42 * R), 2.0, 1.0, 1.5, ARGENTO, c, c);
            rl.forma(poligono(stella(5, 0.25 * R, 0.1 * R)), 3.0, 1.0, 1.0, SMALTO, c, c);
            break;
        case F::ordine_usakov: {
            rl.forma(poligono(stella(5, R, 0.48 * R)), 0.0, 2.5, 3.0, ARGENTO, c, c);
            rl.forma(cerchio(0.48 * R), 2.0, 1.0, 1.5, SMALTO_BLU, c, c);
            rl.forma([&](P p) { return std::fabs(sd_cerchio(p, 0.4 * R)) - 0.05 * R; }, 3.0, 0.5, 0.8, ORO, c, c);   // catena
            rl.forma(poligono({{-0.05 * R, -0.3 * R}, {0.05 * R, -0.3 * R}, {0.05 * R, 0.25 * R}, {-0.05 * R, 0.25 * R}}), 3.5, 0.8, 0.8, ARGENTO, c, c);   // ancora
            rl.forma([&](P p) { return std::max(std::fabs(sd_cerchio({p.x, p.y - 0.05 * R}, 0.22 * R)) - 0.04 * R, -(p.y - 0.05 * R)); }, 3.5, 0.8, 0.8, ARGENTO, c, c);
            break;
        }
        case F::distintivo: {
            if (d->id == "dolphins") {
                // il sommergibile di prua con la vela fra due delfini, sulle onde
                rl.forma([&](P p) { return sd_cerchio({p.x / 1.9, p.y * 1.0}, 0.32 * R); }, 0.0, 2.0, 2.0, ORO, c, c);
                rl.forma(poligono({{-0.12 * R, -0.62 * R}, {0.12 * R, -0.62 * R}, {0.14 * R, -0.2 * R}, {-0.14 * R, -0.2 * R}}), 0.5, 2.0, 1.5, ORO, c, c);
                for (const double sx : {-1.0, 1.0}) {
                    std::vector<P> delfino;
                    for (int k = 0; k <= 10; ++k) {
                        const double u = k / 10.0;
                        delfino.push_back({sx * (0.5 + 0.48 * u) * R, (-0.12 - 0.25 * std::sin(u * PI) + 0.3 * u * u) * R});
                    }
                    for (int k = 10; k >= 0; --k) {
                        const double u = k / 10.0;
                        delfino.push_back({sx * (0.5 + 0.48 * u) * R, (0.08 - 0.18 * std::sin(u * PI) + 0.32 * u * u) * R});
                    }
                    rl.forma(poligono(delfino), 0.0, 2.0, 1.5, ORO, c, c);
                }
                rl.forma([&](P p) { return std::fabs(p.y - 0.42 * R - 0.06 * R * std::sin(p.x / R * 9)) - 0.05 * R + std::max(0.0, std::fabs(p.x) - 0.9 * R); }, 0.0, 1.0, 1.0, ORO, c, c);
            } else if (d->id == "command_at_sea") {
                rl.forma([&](P p) { return sd_cerchio({p.x / 1.4, p.y}, 0.55 * R); }, 0.0, 2.0, 2.0, ORO, c, c);
                rl.forma(poligono(stella(5, 0.4 * R, 0.17 * R)), 2.0, 1.5, 1.5, ARGENTO, c, c);
            } else {
                // sagoma di sommergibile di profilo (pattuglia di combattimento USA, «Comandante di sottomarino» VMF)
                rl.forma([&](P p) { return sd_cerchio({p.x / 3.0, p.y * 1.0}, 0.24 * R); }, 0.0, 2.0, 1.5, d->metallo, c, c);
                rl.forma(poligono({{-0.25 * R, -0.5 * R}, {0.15 * R, -0.5 * R}, {0.22 * R, -0.12 * R}, {-0.3 * R, -0.12 * R}}), 0.0, 2.0, 1.5, d->metallo, c, c);
                if (d->marina == M::vmf) rl.forma(poligono(stella(5, 0.3 * R, 0.13 * R)), 2.5, 1.5, 1.2, SMALTO, c + 0.6 * R, c - 0.45 * R);
                else rl.forma([&](P p) { return std::max(std::fabs(p.y - 0.5 * R) - 0.1 * R, std::fabs(p.x) - 0.7 * R); }, 0.0, 1.5, 1.0, d->metallo, c, c);
            }
            break;
        }
        case F::attestato:
        case F::nessuna: break;
    }
    rl.disegna(t, r.x + static_cast<int>(cx - c), r.y + static_cast<int>(my - c));
}

}  // namespace m688
