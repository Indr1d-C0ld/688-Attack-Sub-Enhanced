#include "grafica/simboli_carta.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <unordered_set>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"

namespace m688::simboli {

namespace {

bool alone_ = true;

std::int64_t chiave(int x, int y) { return (static_cast<std::int64_t>(x) << 32) ^ static_cast<std::uint32_t>(y); }

// un passo di d pixel lungo la rotta (0 = nord, in alto)
Vec2 lungo(Vec2 q, double rotta, double d) { return {q.x + d * std::sin(radianti(rotta)), q.y - d * std::cos(radianti(rotta))}; }

}  // namespace

void imposta_alone(bool si) { alone_ = si; }
bool alone() { return alone_; }

// --- pennello ---

void Pennello::pixel(int x, int y, std::uint8_t c) { p_.push_back({x, y, c}); }
void Pennello::orizzontale(int x0, int x1, int y, std::uint8_t c) {
    for (int x = std::min(x0, x1); x <= std::max(x0, x1); ++x) pixel(x, y, c);
}
void Pennello::verticale(int x, int y0, int y1, std::uint8_t c) {
    for (int y = std::min(y0, y1); y <= std::max(y0, y1); ++y) pixel(x, y, c);
}
void Pennello::rettangolo(int x, int y, int w, int h, std::uint8_t c) {
    for (int j = 0; j < h; ++j) orizzontale(x, x + w - 1, y + j, c);
}
void Pennello::linea(Vec2 a, Vec2 b, std::uint8_t c, int tratto) {
    const int n = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y)));
    for (int i = 0; i <= n; ++i) {
        if (tratto > 0 && (i / tratto) % 2) continue;
        const double s = n ? static_cast<double>(i) / n : 0.0;
        pixel(static_cast<int>(std::lround(a.x + (b.x - a.x) * s)), static_cast<int>(std::lround(a.y + (b.y - a.y) * s)), c);
    }
}
void Pennello::arco(Vec2 centro, double r, double da, double a, std::uint8_t c) {
    const int n = std::max(16, static_cast<int>(r * 7.0 * std::fabs(a - da) / 360.0) + 8);   // un punto per pixel
    for (int i = 0; i <= n; ++i) {
        const double g = radianti(da + (a - da) * i / n);
        pixel(static_cast<int>(std::lround(centro.x + r * std::cos(g))), static_cast<int>(std::lround(centro.y - r * std::sin(g))), c);
    }
}

void Pennello::stendi(Tela& t, bool con_alone) const {
    if (con_alone) {
        std::unordered_set<std::int64_t> pieni;
        pieni.reserve(p_.size() * 2);
        for (const Punto& q : p_) pieni.insert(chiave(q.x, q.y));
        std::unordered_set<std::int64_t> fatti;
        for (const Punto& q : p_)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    const std::int64_t k = chiave(q.x + dx, q.y + dy);
                    if (pieni.count(k) || !fatti.insert(k).second) continue;
                    t.pixel(q.x + dx, q.y + dy, static_cast<std::uint8_t>(colori::NERO));
                }
    }
    for (const Punto& q : p_) t.pixel(q.x, q.y, q.c);
}

void scrivi(Tela& t, const Font& f, int x, int y, std::string_view testo, std::uint8_t colore, int scala) {
    if (alone_) testo_con_alone(t, x, y, testo, f, colore, static_cast<std::uint8_t>(colori::NERO), scala);
    else f.scrivi(t, x, y, testo, colore, scala, f.formato() == 2 ? f.spazio_predefinito() : 1, false);
}

// --- colori ---

std::uint8_t indice_parte(Parte p, bool attenuato) {
    return static_cast<std::uint8_t>(NTDS + (attenuato ? 4 : 0) + static_cast<int>(p));
}

std::uint8_t indice_matita(Parte p, bool vga) {
    switch (p) {
        case Parte::amico: return MATITA + 2;                     // blu
        case Parte::ostile: return MATITA + 1;                    // rosso
        case Parte::neutrale: return MATITA + 3;                  // verde
        case Parte::ignoto: return vga ? MATITA + 5 : MATITA;     // nero, bianco sul blu della VGA
    }
    return MATITA;
}

bool e_oggetto(std::string_view categoria) {
    // un elenco e non un'esclusione: «nessuna» (unita' senza categoria nota, missione.hpp) e le categorie nuove
    // restano unita', col telaio dell'ignoto
    for (const std::string_view c : {"arma", "boa", "biologico", "esca", "mina"})
        if (categoria == c) return true;
    return false;
}

Dominio dominio_da_categoria(std::string_view categoria, std::string_view classe) {
    if (categoria == "sottomarino") return Dominio::subacqueo;
    if (categoria == "nave") return Dominio::superficie;
    if (categoria == "elicottero") return Dominio::elicottero;
    if (categoria == "aereo") {
        for (const std::string_view e : {"sh60b", "sh60f", "sh3h", "ka25", "ka27"})
            if (classe == e) return Dominio::elicottero;
        return Dominio::aereo;
    }
    if (categoria == "terra" || categoria == "base") return Dominio::terra;
    return Dominio::ignoto;
}

Oggetto oggetto_da_classe(std::string_view categoria, std::string_view classe) {
    if (categoria == "boa" || classe.substr(0, 3) == "boa") return Oggetto::boa;
    if (categoria == "biologico" || classe == "biologico") return Oggetto::biologico;
    if (classe.substr(0, 4) == "esca") return Oggetto::esca;
    for (const std::string_view m : {"mk67", "mk60", "pmr1", "pmr2", "mdt", "mina"})
        if (classe == m) return Oggetto::mina;
    for (const std::string_view m : {"harpoon", "tasm", "tlam_c", "tomahawk", "sea_lance", "p700", "vyuga", "missile"})
        if (classe == m) return Oggetto::missile;
    return Oggetto::siluro;
}

std::uint8_t indice_croce_tma(bool vga) { return static_cast<std::uint8_t>(vga ? CURSORE : MATITA); }

// --- simboli ---

void disegna_simbolo_unita(Pennello& pn, Vec2 q, Parte p, Dominio d, std::uint8_t col) {
    const int x = static_cast<int>(q.x), y = static_cast<int>(q.y), r = 5;
    const bool sotto = d == Dominio::subacqueo, sopra = d == Dominio::aereo || d == Dominio::elicottero;
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx) {
            if ((sotto && dy < 0) || (sopra && dy > 0)) continue;   // la meta' del telaio
            const int ax = std::abs(dx), ay = std::abs(dy);
            bool bordo = false;
            switch (p) {
                case Parte::amico: bordo = std::abs(dx * dx + dy * dy - r * r) <= r; break;
                case Parte::ostile: bordo = ax + ay == r + 1; break;
                case Parte::neutrale: bordo = std::max(ax, ay) == r; break;
                case Parte::ignoto: bordo = (std::max(ax, ay) == r && std::min(ax, ay) <= 2) || ax + ay == r + 3; break;
            }
            if (bordo) pn.pixel(x + dx, y + dy, col);
        }
    if (sotto || sopra) pn.orizzontale(x - r, x + r, y, col);
    if (d == Dominio::elicottero) {   // le pale sopra il telaio
        pn.orizzontale(x - 3, x + 3, y - r - 3, col);
        pn.pixel(x, y - r - 2, col);
    }
    if (d == Dominio::terra) pn.orizzontale(x - r - 1, x + r + 1, y + r + 2, col);   // la base
    if (d == Dominio::ignoto) {   // il «?» dentro il telaio
        pn.orizzontale(x - 1, x + 1, y - 3, col);
        pn.pixel(x - 2, y - 2, col), pn.pixel(x + 2, y - 2, col), pn.pixel(x + 1, y - 1, col);
        pn.pixel(x, y, col), pn.pixel(x, y + 2, col);
    }
}

void disegna_proprio(Pennello& pn, Vec2 q, bool evidenza) {
    const int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
    pn.rettangolo(x - 3, y - 3, 7, 7, VERDE_PROPRIO_SCURO);
    pn.rettangolo(x - 2, y - 2, 5, 5, VERDE_PROPRIO);
    if (evidenza) pn.arco(q, 8.0, 0.0, 360.0, static_cast<std::uint8_t>(EVIDENZA));
}

void disegna_oggetto(Pennello& pn, Vec2 q, Oggetto o, std::uint8_t col, double rotta) {
    const int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
    switch (o) {
        case Oggetto::siluro:
        case Oggetto::missile: {
            const Vec2 punta = lungo(q, rotta, 3.0), coda = lungo(q, rotta, -3.0);
            pn.linea(coda, punta, col);
            pn.linea(punta, lungo(punta, rotta + 145.0, 2.5), col);
            pn.linea(punta, lungo(punta, rotta - 145.0, 2.5), col);
            if (o == Oggetto::missile) {   // la coda doppia
                pn.linea(coda, lungo(coda, rotta + 145.0, 2.0), col);
                pn.linea(coda, lungo(coda, rotta - 145.0, 2.0), col);
            }
            return;
        }
        case Oggetto::esca:
            for (int dy = -2; dy <= 2; ++dy)
                for (int dx = -2; dx <= 2; ++dx)
                    if (std::abs(dx) + std::abs(dy) == 2) pn.pixel(x + dx, y + dy, col);
            return;
        case Oggetto::mina:   // il cerchio di raggio 3 e le quattro punte
            for (int k = 0; k < 12; ++k) {
                const double a = 2.0 * PI * k / 12.0;
                pn.pixel(x + static_cast<int>(std::lround(3.0 * std::sin(a))), y - static_cast<int>(std::lround(3.0 * std::cos(a))), col);
            }
            for (int k = 4; k <= 6; ++k) pn.pixel(x, y - k, col), pn.pixel(x, y + k, col), pn.pixel(x - k, y, col), pn.pixel(x + k, y, col);
            return;
        case Oggetto::boa:   // il galleggiante e l'antenna
            pn.rettangolo(x - 1, y - 1, 3, 3, col);
            pn.verticale(x, y - 4, y - 2, col);
            return;
        case Oggetto::biologico: {   // la coda di balena
            const auto g = static_cast<std::uint8_t>(colori::GRIGIO);
            pn.pixel(x - 3, y - 2, g), pn.pixel(x + 3, y - 2, g);
            pn.pixel(x - 2, y - 1, g), pn.pixel(x - 1, y - 1, g), pn.pixel(x + 1, y - 1, g), pn.pixel(x + 2, y - 1, g);
            pn.verticale(x, y - 1, y + 2, g);
            return;
        }
    }
}

void disegna_filo(Pennello& pn, Vec2 arma, Vec2 guida, std::uint8_t col) { pn.linea(arma, guida, col, 1); }

void disegna_distrutta(Pennello& pn, Vec2 q) {
    const auto c = static_cast<std::uint8_t>(colori::ROSSO);
    pn.linea({q.x - 4, q.y - 4}, {q.x + 4, q.y + 4}, c);
    pn.linea({q.x - 4, q.y + 4}, {q.x + 4, q.y - 4}, c);
}

void disegna_emissione(Pennello& pn, Vec2 q, Emissione e, double t_s) {
    switch (e) {
        case Emissione::ping: {
            const double fase = std::fmod(t_s * 0.8, 1.0);
            pn.arco(q, 7.0 + 14.0 * fase, 0.0, 360.0, static_cast<std::uint8_t>(fase < 0.6 ? colori::BIANCO : colori::GRIGIO));
            return;
        }
        case Emissione::radar: {
            const double giro = std::fmod(t_s * 60.0, 360.0);
            const auto c = static_cast<std::uint8_t>(colori::CIANO_CHIARO);
            pn.arco(q, 11.0, giro, giro + 50.0, c);
            pn.linea(q, {q.x + 11.0 * std::cos(radianti(giro)), q.y - 11.0 * std::sin(radianti(giro))}, c);
            return;
        }
        case Emissione::radio: {
            const auto c = static_cast<std::uint8_t>(colori::GIALLO);
            pn.linea({q.x + 4, q.y - 12}, {q.x + 2, q.y - 9}, c);
            pn.linea({q.x + 2, q.y - 9}, {q.x + 5, q.y - 8}, c);
            pn.linea({q.x + 5, q.y - 8}, {q.x + 3, q.y - 5}, c);
            return;
        }
    }
}

void disegna_scelta(Pennello& pn, Vec2 q) {
    const auto c = static_cast<std::uint8_t>(EVIDENZA);
    for (const int sx : {-1, 1})
        for (const int sy : {-1, 1}) {
            pn.linea({q.x + 9 * sx, q.y + 9 * sy}, {q.x + 5 * sx, q.y + 9 * sy}, c);
            pn.linea({q.x + 9 * sx, q.y + 9 * sy}, {q.x + 9 * sx, q.y + 5 * sy}, c);
        }
}

void disegna_rilevamento(Pennello& pn, Vec2 da, Vec2 a, std::uint8_t col, bool specchio) { pn.linea(da, a, col, specchio ? 3 : 2); }

void disegna_croce_tma(Pennello& pn, Vec2 q, std::uint8_t col) {
    const int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
    pn.orizzontale(x - 3, x + 3, y, col);
    pn.verticale(x, y - 3, y + 3, col);
}

void disegna_ellisse(Pennello& pn, Vec2 c, double a, double b, double orientamento, std::uint8_t col) {
    const double ang = radianti(orientamento);
    const Vec2 asse{std::sin(ang), -std::cos(ang)}, traverso{std::cos(ang), std::sin(ang)};   // sullo schermo, nord in alto
    for (int k = 0; k < 120; k += 2) {   // tratteggiata: un punto si', uno no
        const double th = 2.0 * PI * k / 120.0, ca = a * std::cos(th), sb = b * std::sin(th);
        pn.pixel(static_cast<int>(std::lround(c.x + ca * asse.x + sb * traverso.x)),
                 static_cast<int>(std::lround(c.y + ca * asse.y + sb * traverso.y)), col);
    }
}

// le versioni che disegnano da sole, ciascuna col suo alone
void disegna_simbolo_unita(Tela& t, Vec2 q, Parte p, Dominio d, std::uint8_t c) {
    Pennello pn;
    disegna_simbolo_unita(pn, q, p, d, c);
    pn.stendi(t);
}
void disegna_proprio(Tela& t, Vec2 q, bool evidenza) {
    Pennello pn;
    disegna_proprio(pn, q, evidenza);
    pn.stendi(t);
}
void disegna_oggetto(Tela& t, Vec2 q, Oggetto o, std::uint8_t c, double rotta) {
    Pennello pn;
    disegna_oggetto(pn, q, o, c, rotta);
    pn.stendi(t);
}
void disegna_filo(Tela& t, Vec2 arma, Vec2 guida, std::uint8_t c) {
    Pennello pn;
    disegna_filo(pn, arma, guida, c);
    pn.stendi(t);
}
void disegna_distrutta(Tela& t, Vec2 q) {
    Pennello pn;
    disegna_distrutta(pn, q);
    pn.stendi(t, false);
}
void disegna_emissione(Tela& t, Vec2 q, Emissione e, double t_s) {
    Pennello pn;
    disegna_emissione(pn, q, e, t_s);
    pn.stendi(t);
}
void disegna_scelta(Tela& t, Vec2 q) {
    Pennello pn;
    disegna_scelta(pn, q);
    pn.stendi(t);
}
void disegna_rilevamento(Tela& t, Vec2 da, Vec2 a, std::uint8_t c, bool specchio) {
    Pennello pn;
    disegna_rilevamento(pn, da, a, c, specchio);
    pn.stendi(t);
}
void disegna_croce_tma(Tela& t, Vec2 q, std::uint8_t c) {
    Pennello pn;
    disegna_croce_tma(pn, q, c);
    pn.stendi(t);
}
void disegna_ellisse(Tela& t, Vec2 c, double a, double b, double orientamento, std::uint8_t col) {
    Pennello pn;
    disegna_ellisse(pn, c, a, b, orientamento, col);
    pn.stendi(t);
}

// --- il vetrino di Dio: i segni degli interventi ---

namespace {

// i segni a 11x11 (quelli da 9x9 hanno vuote la prima e l'ultima riga e colonna), '#' acceso; gli archi radio delle
// comunicazioni si tracciano
using Segno = std::array<const char*, 11>;
constexpr Segno BANDIERINA = {"...........", "...##......", "...####....", "...######..", "...####....",
                              "...##......", "...#.......", "...#.......", "...#.......", "..###......",
                              "..........."};
constexpr Segno CHIAVE = {"...........", "......#.#..", "......#.##.", "......###..", ".....##....",
                          "....##.....", "...##......", "..##.......", ".##........", ".#.........",
                          "..........."};
constexpr Segno ONDA = {"...........", "...........", "...##...##.", "..#..#.#...", ".#....#....",
                        "...........", "...##...##.", "..#..#.#...", ".#....#....", "...........",
                        "..........."};
constexpr Segno ROMBO = {".....#.....", "....#.#....", "...#.###...", "..#.....#..", ".#....#..#.",
                         "#....#....#", ".#.......#.", "..#..#..#..", "...#...#...", "....#.#....",
                         ".....#....."};

const Segno& segno_di(FamigliaIntervento f) {
    switch (f) {
        case FamigliaIntervento::forze: return BANDIERINA;
        case FamigliaIntervento::guasti: return CHIAVE;
        case FamigliaIntervento::ambiente: return ONDA;
        case FamigliaIntervento::comunicazioni: break;   // gli archi si tracciano (disegna_intervento)
        case FamigliaIntervento::inganno: break;
    }
    return ROMBO;
}

}  // namespace

std::uint8_t indice_famiglia(FamigliaIntervento f) {
    switch (f) {
        case FamigliaIntervento::forze: return 13;
        case FamigliaIntervento::guasti: return 6;
        case FamigliaIntervento::ambiente: return 11;
        case FamigliaIntervento::comunicazioni: return 15;
        case FamigliaIntervento::inganno: break;
    }
    return 5;
}

FamigliaIntervento famiglia_da_gruppo(const std::string& gruppo) {
    std::string g;
    for (const char c : gruppo) g += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    if (g == "FORZE") return FamigliaIntervento::forze;
    if (g == "GUASTI") return FamigliaIntervento::guasti;
    if (g == "AMBIENTE") return FamigliaIntervento::ambiente;
    if (g == "COMUNICAZIONI") return FamigliaIntervento::comunicazioni;
    return FamigliaIntervento::inganno;
}

void disegna_intervento(Pennello& pn, Vec2 q, FamigliaIntervento f, std::uint8_t colore, bool programmato) {
    const int x0 = static_cast<int>(std::lround(q.x)) - 5, y0 = static_cast<int>(std::lround(q.y)) - 5;
    if (f == FamigliaIntervento::comunicazioni) {   // il punto dell'antenna in basso a sinistra e due archi radio
        const Vec2 c{x0 + 2.0, y0 + 8.0};
        pn.rettangolo(x0 + 2, y0 + 7, 2, 2, colore);
        pn.arco(c, 3.6, 0.0, 90.0, colore);
        pn.arco(c, 6.6, 0.0, 90.0, colore);
    } else {
        const Segno& s = segno_di(f);
        for (int j = 0; j < 11; ++j)
            for (int i = 0; i < 11; ++i)
                if (s[static_cast<std::size_t>(j)][i] == '#') pn.pixel(x0 + i, y0 + j, colore);
    }
    if (programmato)   // gli angoli di un quadrato di 13x13, a tre pixel per lato (tratti piu' fitti fanno un grumo con l'alone)
        for (const int ax : {x0 - 1, x0 + 11})
            for (const int ay : {y0 - 1, y0 + 11}) {
                const int sx = ax < x0 ? 1 : -1, sy = ay < y0 ? 1 : -1;
                pn.orizzontale(ax, ax + 2 * sx, ay, colore);
                pn.verticale(ax, ay, ay + 2 * sy, colore);
            }
}

void disegna_doppia_freccia(Pennello& pn, Vec2 vera, Vec2 creduta, std::uint8_t colore) {
    pn.arco(vera, 2.0, 0.0, 360.0, colore);
    const double dx = creduta.x - vera.x, dy = creduta.y - vera.y, d = std::hypot(dx, dy);
    if (d < 6.0) return;
    const Vec2 u{dx / d, dy / d}, n{-u.y, u.x};
    pn.linea({vera.x + 3.0 * u.x, vera.y + 3.0 * u.y}, creduta, colore, 2);
    for (const double indietro : {0.0, 4.0}) {   // la doppia punta: due «V» una dietro l'altra
        const Vec2 p{creduta.x - indietro * u.x, creduta.y - indietro * u.y};
        pn.linea(p, {p.x - 3.0 * u.x + 3.0 * n.x, p.y - 3.0 * u.y + 3.0 * n.y}, colore);
        pn.linea(p, {p.x - 3.0 * u.x - 3.0 * n.x, p.y - 3.0 * u.y - 3.0 * n.y}, colore);
    }
}

void disegna_portata_ridotta(Pennello& pn, Vec2 c, double piena, double ridotta, std::uint8_t colore) {
    const auto grigio = static_cast<std::uint8_t>(colori::GRIGIO);
    if (piena > 0.0) {
        const int n = std::max(24, static_cast<int>(2.0 * PI * piena / 6.0));   // un puntino ogni 6 pixel
        for (int k = 0; k < n; ++k) {
            const double a = 2.0 * PI * k / n;
            pn.pixel(static_cast<int>(std::lround(c.x + piena * std::cos(a))), static_cast<int>(std::lround(c.y + piena * std::sin(a))), grigio);
        }
    }
    if (ridotta > 0.0) {
        const int n = std::max(32, static_cast<int>(2.0 * PI * ridotta));
        for (int k = 0; k < n; ++k) {
            if ((k / 3) % 2) continue;   // tratti di tre pixel
            const double a = 2.0 * PI * k / n;
            pn.pixel(static_cast<int>(std::lround(c.x + ridotta * std::cos(a))), static_cast<int>(std::lround(c.y + ridotta * std::sin(a))), colore);
        }
    }
    if (piena - ridotta >= 8.0)
        for (int k = 0; k < 8; ++k) {   // i trattini verso il centro, a meta' fra le due
            const double a = PI / 8.0 + k * PI / 4.0, m = 0.5 * (piena + ridotta);
            const Vec2 u{std::cos(a), std::sin(a)};
            pn.linea({c.x + (m + 2.0) * u.x, c.y + (m + 2.0) * u.y}, {c.x + (m - 2.0) * u.x, c.y + (m - 2.0) * u.y}, colore);
        }
}

void disegna_intervento(Tela& t, Vec2 q, FamigliaIntervento f, std::uint8_t colore, bool programmato) {
    Pennello pn;
    disegna_intervento(pn, q, f, colore, programmato);
    pn.stendi(t);
}
void disegna_doppia_freccia(Tela& t, Vec2 vera, Vec2 creduta, std::uint8_t colore) {
    Pennello pn;
    disegna_doppia_freccia(pn, vera, creduta, colore);
    pn.stendi(t);
}
void disegna_portata_ridotta(Tela& t, Vec2 c, double piena, double ridotta, std::uint8_t colore) {
    Pennello pn;
    disegna_portata_ridotta(pn, c, piena, ridotta, colore);
    pn.stendi(t);
}

int disegna_legenda(Tela& t, const Font& f, int x, int y, std::span<const VoceLegenda> voci, std::uint8_t colore_testo) {
    int riga = y;
    for (const VoceLegenda& v : voci) {
        const Vec2 q{x + 6.0, riga + 6.0};
        const std::uint8_t col = v.colore ? v.colore : indice_parte(v.parte);
        Pennello pn;
        switch (v.tipo) {
            case VoceLegenda::Tipo::unita: disegna_simbolo_unita(pn, q, v.parte, v.dominio, col); break;
            case VoceLegenda::Tipo::proprio: disegna_proprio(pn, q); break;
            case VoceLegenda::Tipo::oggetto: disegna_oggetto(pn, q, v.oggetto, col, 90.0); break;
            case VoceLegenda::Tipo::distrutta:
                disegna_simbolo_unita(pn, q, Parte::ostile, Dominio::superficie, indice_parte(Parte::ostile));
                break;
            case VoceLegenda::Tipo::emissione:
                if (v.emissione == Emissione::radio) disegna_emissione(pn, {q.x - 3, q.y + 8}, v.emissione, 0.0);
                else if (v.emissione == Emissione::radar) {   // un quarto del ventaglio, che stia nella riga
                    const auto c = static_cast<std::uint8_t>(colori::CIANO_CHIARO);
                    pn.arco({q.x - 5, q.y + 4}, 9.0, 20.0, 70.0, c);
                    pn.linea({q.x - 5, q.y + 4}, {q.x - 5 + 9.0 * std::cos(radianti(20.0)), q.y + 4 - 9.0 * std::sin(radianti(20.0))}, c);
                } else pn.arco(q, 5.0, 0.0, 360.0, static_cast<std::uint8_t>(colori::BIANCO));
                break;
            case VoceLegenda::Tipo::croce_tma: disegna_croce_tma(pn, q, col); break;
            case VoceLegenda::Tipo::scia:
                for (int k = -5; k <= 5; k += 2) pn.pixel(static_cast<int>(q.x) + k, static_cast<int>(q.y), col);
                break;
        }
        pn.stendi(t);
        if (v.tipo == VoceLegenda::Tipo::distrutta) disegna_distrutta(t, q);
        scrivi(t, f, x + 16, riga + 3, v.testo, colore_testo);
        riga += 14 + (v.tipo == VoceLegenda::Tipo::unita && v.dominio == Dominio::elicottero ? 3 : 0);
    }
    return riga - y;
}

}  // namespace m688::simboli
