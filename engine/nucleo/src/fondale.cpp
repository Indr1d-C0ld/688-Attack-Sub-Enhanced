#include "nucleo/fondale.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>

#include "nucleo/inflate.hpp"

namespace m688 {

namespace {

// Lettura little endian dal contenuto di un blocco
struct Cursore {
    const std::vector<std::uint8_t>& d;
    std::size_t i = 0;
    const std::string& percorso;
    void controlla(std::size_t n) const {
        if (i + n > d.size()) throw std::runtime_error(percorso + ": blocco troncato");
    }
    template <class T> T leggi() {
        controlla(sizeof(T));
        T v;
        std::memcpy(&v, d.data() + i, sizeof(T));
        i += sizeof(T);
        return v;
    }
    std::string testo(std::size_t n) {
        controlla(n);
        std::string s(reinterpret_cast<const char*>(d.data() + i), n);
        i += n;
        return s;
    }
};

// Ellissoide WGS84 e problemi geodetici di Vincenty (1975)
constexpr double WGS84_A = 6378137.0, WGS84_F = 1.0 / 298.257223563, WGS84_B = WGS84_A * (1.0 - WGS84_F);

double termine_a(double u2) { return 1 + u2 / 16384 * (4096 + u2 * (-768 + u2 * (320 - 175 * u2))); }
double termine_b(double u2) { return u2 / 1024 * (256 + u2 * (-128 + u2 * (74 - 47 * u2))); }
double delta_sigma(double b, double sin_s, double cos_s, double cos2sm) {
    return b * sin_s * (cos2sm + b / 4 * (cos_s * (-1 + 2 * cos2sm * cos2sm) -
                                          b / 6 * cos2sm * (-3 + 4 * sin_s * sin_s) * (-3 + 4 * cos2sm * cos2sm)));
}

// Punto a distanza s (m) e azimut az (rad) da (lat1, lon1) (rad)
Geografiche diretto(double lat1, double lon1, double az, double s) {
    const double f = WGS84_F;
    const double tan_u1 = (1 - f) * std::tan(lat1), cos_u1 = 1 / std::sqrt(1 + tan_u1 * tan_u1), sin_u1 = tan_u1 * cos_u1;
    const double sin_a1 = std::sin(az), cos_a1 = std::cos(az);
    const double sigma1 = std::atan2(tan_u1, cos_a1);
    const double sin_a = cos_u1 * sin_a1, cos2a = 1 - sin_a * sin_a;
    const double u2 = cos2a * (WGS84_A * WGS84_A - WGS84_B * WGS84_B) / (WGS84_B * WGS84_B);
    const double A = termine_a(u2), B = termine_b(u2);
    double sigma = s / (WGS84_B * A), cos2sm = 0, sin_s = 0, cos_s = 1;
    for (int i = 0; i < 100; ++i) {
        cos2sm = std::cos(2 * sigma1 + sigma);
        sin_s = std::sin(sigma);
        cos_s = std::cos(sigma);
        const double nuovo = s / (WGS84_B * A) + delta_sigma(B, sin_s, cos_s, cos2sm);
        const bool fermo = std::fabs(nuovo - sigma) < 1e-13;
        sigma = nuovo;
        if (fermo) break;
    }
    cos2sm = std::cos(2 * sigma1 + sigma);
    sin_s = std::sin(sigma);
    cos_s = std::cos(sigma);
    const double t = sin_u1 * sin_s - cos_u1 * cos_s * cos_a1;
    const double lat2 = std::atan2(sin_u1 * cos_s + cos_u1 * sin_s * cos_a1, (1 - f) * std::sqrt(sin_a * sin_a + t * t));
    const double lambda = std::atan2(sin_s * sin_a1, cos_u1 * cos_s - sin_u1 * sin_s * cos_a1);
    const double C = f / 16 * cos2a * (4 + f * (4 - 3 * cos2a));
    const double L = lambda - (1 - C) * f * sin_a * (sigma + C * sin_s * (cos2sm + C * cos_s * (-1 + 2 * cos2sm * cos2sm)));
    return {gradi(lat2), normalizza_180(gradi(lon1 + L))};
}

// Distanza (m) e azimut iniziale (rad) da (lat1, lon1) a (lat2, lon2) (rad)
void inverso(double lat1, double lon1, double lat2, double lon2, double& s, double& az) {
    const double f = WGS84_F;
    const double L = radianti(normalizza_180(gradi(lon2 - lon1)));
    const double u1 = std::atan((1 - f) * std::tan(lat1)), u2 = std::atan((1 - f) * std::tan(lat2));
    const double sin_u1 = std::sin(u1), cos_u1 = std::cos(u1), sin_u2 = std::sin(u2), cos_u2 = std::cos(u2);
    double lambda = L, sin_l = 0, cos_l = 1, sin_s = 0, cos_s = 1, sigma = 0, cos2a = 1, cos2sm = 0;
    for (int i = 0; i < 200; ++i) {
        sin_l = std::sin(lambda);
        cos_l = std::cos(lambda);
        const double p = cos_u2 * sin_l, q = cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos_l;
        sin_s = std::sqrt(p * p + q * q);
        if (sin_s == 0) {   // punti coincidenti
            s = 0;
            az = 0;
            return;
        }
        cos_s = sin_u1 * sin_u2 + cos_u1 * cos_u2 * cos_l;
        sigma = std::atan2(sin_s, cos_s);
        const double sin_a = cos_u1 * cos_u2 * sin_l / sin_s;
        cos2a = 1 - sin_a * sin_a;
        cos2sm = cos2a != 0 ? cos_s - 2 * sin_u1 * sin_u2 / cos2a : 0;
        const double C = f / 16 * cos2a * (4 + f * (4 - 3 * cos2a));
        const double prima = lambda;
        lambda = L + (1 - C) * f * sin_a * (sigma + C * sin_s * (cos2sm + C * cos_s * (-1 + 2 * cos2sm * cos2sm)));
        if (std::fabs(lambda - prima) < 1e-13) break;
    }
    const double uu = cos2a * (WGS84_A * WGS84_A - WGS84_B * WGS84_B) / (WGS84_B * WGS84_B);
    const double A = termine_a(uu), B = termine_b(uu);
    s = WGS84_B * A * (sigma - delta_sigma(B, sin_s, cos_s, cos2sm));
    az = std::atan2(cos_u2 * sin_l, cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos_l);
}

}  // namespace

const char* nome_parte(Parte p) { return p == Parte::nato ? "nato" : "urss"; }

std::string percorso_carta(const std::string& dati, const std::string& teatro, Parte p) {
    return dati + "/teatri/" + teatro + "_carta_" + nome_parte(p) + ".fdl";
}

Fondale Fondale::carica(const std::string& percorso) {
    std::ifstream f(percorso, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + percorso);
    char firma[4];
    std::uint16_t w = 0, h = 0;
    float cella = 0, scala = 0, offset = 0;
    f.read(firma, 4);
    f.read(reinterpret_cast<char*>(&w), 2);
    f.read(reinterpret_cast<char*>(&h), 2);
    f.read(reinterpret_cast<char*>(&cella), 4);
    f.read(reinterpret_cast<char*>(&scala), 4);
    f.read(reinterpret_cast<char*>(&offset), 4);
    if (!f || std::memcmp(firma, "FDL2", 4) != 0 || w == 0 || h == 0 || !(cella > 0))
        throw std::runtime_error(percorso + ": non e' un fondale FDL2 (riesportare con tools/pipeline/esporta_motore.py)");
    // valori compressi (tools/pipeline/esporta_motore.py): scarto dalla predizione a piano sinistra + sopra - diagonale,
    // byte bassi poi byte alti, DEFLATE
    std::uint32_t nz = 0;
    f.read(reinterpret_cast<char*>(&nz), 4);
    std::vector<std::uint8_t> z(nz);
    f.read(reinterpret_cast<char*>(z.data()), static_cast<std::streamsize>(nz));
    if (!f) throw std::runtime_error(percorso + ": fondale troncato");
    const std::size_t n = static_cast<std::size_t>(w) * h;
    std::vector<std::uint8_t> piani;
    try {
        piani = interno::inflate(z.data(), z.size(), 2 * n);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(percorso + ": " + e.what());
    }
    std::vector<std::uint16_t> v(n);
    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t x = i % w;
        const unsigned a = x > 0 ? v[i - 1] : 0, b = i >= w ? v[i - w] : 0, d = x > 0 && i >= w ? v[i - w - 1] : 0;
        v[i] = static_cast<std::uint16_t>(a + b - d + (piani[i] | (piani[n + i] << 8)));
    }
    Fondale fd(w, h, cella, scala, offset, std::move(v));
    // blocchi facoltativi: "TAG4", u32 lunghezza, contenuto (formato in tools/pipeline/carta_epoca.py, scrivi_fdl);
    // quelli sconosciuti si saltano
    for (;;) {
        char tag[4];
        f.read(tag, 4);
        if (f.gcount() == 0) break;
        std::uint32_t lun = 0;
        f.read(reinterpret_cast<char*>(&lun), 4);
        std::vector<std::uint8_t> b(lun);
        f.read(reinterpret_cast<char*>(b.data()), static_cast<std::streamsize>(lun));
        if (!f) throw std::runtime_error(percorso + ": blocco troncato");
        Cursore c{b, 0, percorso};
        const std::string t(tag, 4);
        if (t == "GEO1") {
            const double lat = c.leggi<double>(), lon = c.leggi<double>();
            fd.imposta_centro({lat, lon});
        } else if (t == "RIL1") {
            const int edizione = c.leggi<std::uint16_t>();
            std::vector<Campagna> campagne(c.leggi<std::uint16_t>());
            for (auto& k : campagne) {
                k.anno = c.leggi<std::uint16_t>();
                k.spaziatura_m = c.leggi<float>();
                k.spaziatura_bacini_m = c.leggi<float>();
                k.spostamento_m = c.leggi<float>();
                k.ente = c.testo(c.leggi<std::uint8_t>());
            }
            const std::uint32_t nc = c.leggi<std::uint32_t>();
            c.controlla(nc);
            std::vector<std::uint8_t> per_cella;
            try {
                per_cella = interno::inflate(b.data() + c.i, nc, n);
            } catch (const std::runtime_error& e) {
                throw std::runtime_error(percorso + ": " + e.what());
            }
            if (per_cella.size() != n) throw std::runtime_error(percorso + ": campagne per cella incomplete");
            for (const auto k : per_cella)
                if (k > campagne.size()) throw std::runtime_error(percorso + ": campagna inesistente");
            fd.imposta_carta(edizione, std::move(campagne), std::move(per_cella));
        } else if (t == "ANN1") {
            std::vector<Annotazione> a(c.leggi<std::uint16_t>());
            for (auto& k : a) {
                const int tipo = c.leggi<std::uint8_t>();
                if (tipo > 3) throw std::runtime_error(percorso + ": annotazione sconosciuta");
                k.tipo = static_cast<Annotazione::Tipo>(tipo);
                k.pos.x = c.leggi<float>();
                k.pos.y = c.leggi<float>();
                k.profondita_m = c.leggi<float>();
                k.anno = c.leggi<std::uint16_t>();
            }
            fd.annotazioni_ = std::move(a);
        }
    }
    return fd;
}

Fondale::Fondale(int larghezza, int altezza, double cella_m, double scala, double offset, std::vector<std::uint16_t> valori)
    : w_(larghezza), h_(altezza), cella_(cella_m), scala_(scala), offset_(offset), valori_(std::move(valori)) {
    if (w_ <= 0 || h_ <= 0 || valori_.size() != static_cast<std::size_t>(w_) * h_)
        throw std::invalid_argument("fondale: dimensioni e valori non corrispondono");
}

bool Fondale::dentro(Vec2 p) const {
    return std::fabs(p.x) <= w_ * cella_ / 2.0 && std::fabs(p.y) <= h_ * cella_ / 2.0;
}

bool Fondale::cella(Vec2 p, int& col, int& riga) const {
    if (!dentro(p)) return false;
    col = std::clamp(static_cast<int>(std::floor((p.x + w_ * cella_ / 2.0) / cella_)), 0, w_ - 1);
    riga = std::clamp(static_cast<int>(std::floor((h_ * cella_ / 2.0 - p.y) / cella_)), 0, h_ - 1);
    return true;
}

Vec2 Fondale::centro_cella(int col, int riga) const {
    return {(col + 0.5) * cella_ - w_ * cella_ / 2.0, h_ * cella_ / 2.0 - (riga + 0.5) * cella_};
}

double Fondale::quota(Vec2 p) const {
    if (!dentro(p)) return -std::numeric_limits<double>::infinity();
    // centro della cella (col, riga): x = (col + 0,5) * cella - L/2, y = H/2 - (riga + 0,5) * cella
    const double c = std::clamp((p.x + w_ * cella_ / 2.0) / cella_ - 0.5, 0.0, w_ - 1.0);
    const double r = std::clamp((h_ * cella_ / 2.0 - p.y) / cella_ - 0.5, 0.0, h_ - 1.0);
    const int c0 = std::min(static_cast<int>(c), std::max(w_ - 2, 0)), r0 = std::min(static_cast<int>(r), std::max(h_ - 2, 0));
    const int c1 = std::min(c0 + 1, w_ - 1), r1 = std::min(r0 + 1, h_ - 1);
    const double fc = c - c0, fr = r - r0;
    const double alto = z(c0, r0) * (1 - fc) + z(c1, r0) * fc, basso = z(c0, r1) * (1 - fc) + z(c1, r1) * fc;
    return alto * (1 - fr) + basso * fr;
}

void Fondale::imposta_centro(Geografiche c) {
    centro_ = c;
    geo_ = true;
}

Geografiche Fondale::geografiche(Vec2 p) const {
    if (!geo_) throw std::logic_error("fondale senza centro della proiezione (blocco GEO1)");
    const double s = std::hypot(p.x, p.y);
    if (s == 0) return centro_;
    return diretto(radianti(centro_.lat), radianti(centro_.lon), std::atan2(p.x, p.y), s);
}

Vec2 Fondale::proiettata(Geografiche g) const {
    if (!geo_) throw std::logic_error("fondale senza centro della proiezione (blocco GEO1)");
    double s = 0, az = 0;
    inverso(radianti(centro_.lat), radianti(centro_.lon), radianti(g.lat), radianti(g.lon), s, az);
    return {s * std::sin(az), s * std::cos(az)};
}

void Fondale::imposta_carta(int edizione, std::vector<Campagna> campagne, std::vector<std::uint8_t> per_cella,
                            std::vector<Annotazione> annotazioni) {
    if (per_cella.size() != valori_.size()) throw std::invalid_argument("carta: una campagna per cella");
    edizione_ = edizione;
    campagne_ = std::move(campagne);
    campagna_cella_ = std::move(per_cella);
    if (!annotazioni.empty()) annotazioni_ = std::move(annotazioni);
}

int Fondale::campagna_cella(int col, int riga) const {
    if (campagna_cella_.empty() || col < 0 || riga < 0 || col >= w_ || riga >= h_) return -1;
    return static_cast<int>(campagna_cella_[static_cast<std::size_t>(riga) * w_ + col]) - 1;
}

int Fondale::campagna(Vec2 p) const {
    int col = 0, riga = 0;
    return cella(p, col, riga) ? campagna_cella(col, riga) : -1;
}

}  // namespace m688
