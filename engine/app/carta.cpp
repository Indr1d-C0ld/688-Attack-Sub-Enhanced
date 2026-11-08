#include "carta.hpp"

#include "grafica/etichetta_tasti.hpp"
#include "grafica/simboli_carta.hpp"

#include <algorithm>
#include <map>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/strati_meteo.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

namespace {
constexpr std::uint8_t ROSSO_CHIARO = 12, MAGENTA_CHIARO = 13;   // colori EGA dell'originale
Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }   // spostamenti in pixel delle scritte

// Colore del fondale: terra, poi acqua sempre piu' scura con la profondita'
std::uint8_t colore_fondale(double prof) {
    if (prof <= 0.0) return colori::MARRONE;
    if (prof < 50.0) return colori::CIANO_CHIARO;
    if (prof < 200.0) return colori::CIANO;
    return colori::BLU;
}

// --- grafica VGA/SVGA dell'Enhanced (N1) ---

// Confini delle 16 fasce di profondita' in metri (passo crescente, docs/ricerca/12 §5.6) e delle 7 di quota
constexpr double SOGLIE_ACQUA[15] = {10, 20, 30, 50, 75, 100, 150, 200, 300, 400, 500, 750, 1000, 1500, 2000};
constexpr double SOGLIE_TERRA[6] = {20, 50, 100, 200, 400, 700};
constexpr bool PRINCIPALE[15] = {false, false, false, false, false, true, false, true, false, false, true, false, true, false, true};

// La fascia della profondita' (0-15) e quanto vi si e' dentro (0-1, per la retinatura)
int fascia_acqua(double prof, double& dentro) {
    int b = 0;
    while (b < 15 && prof >= SOGLIE_ACQUA[b]) ++b;
    const double lo = b ? SOGLIE_ACQUA[b - 1] : 0.0, hi = b < 15 ? SOGLIE_ACQUA[b] : lo * 1.5;
    dentro = std::clamp((prof - lo) / (hi - lo), 0.0, 1.0);
    return b;
}
int fascia_terra(double quota) {
    int b = 0;
    while (b < 6 && quota >= SOGLIE_TERRA[b]) ++b;
    return b;
}

struct Rgb {
    double r, g, b;
};
Rgb mescola(Rgb a, Rgb b, double t) { return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t}; }
// colore lungo una serie di punti uguali da 0 a 1
template <std::size_t N>
Rgb lungo(const Rgb (&p)[N], double t) {
    const double x = std::clamp(t, 0.0, 1.0) * static_cast<double>(N - 1);
    const auto i = std::min(static_cast<std::size_t>(x), N - 2);
    return mescola(p[i], p[i + 1], x - static_cast<double>(i));
}
void metti(Palette& p, int i, Rgb c, double k = 1.0) {
    const auto v = [k](double x) { return static_cast<std::uint8_t>(std::clamp(std::lround(x * k), 0L, 255L)); };
    p.imposta(i, v(c.r), v(c.g), v(c.b));
}

// Retinatura ordinata 2x2: le soglie dei quattro pixel di un blocco
constexpr double BAYER[2][2] = {{0.125, 0.625}, {0.875, 0.375}};
}  // namespace

std::uint8_t colore_carta(double profondita) {
    using namespace colori_carta;
    if (!std::isfinite(profondita)) return static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1);
    if (profondita <= 0.0) return static_cast<std::uint8_t>(TERRA + fascia_terra(-profondita));
    double dentro = 0.0;
    return static_cast<std::uint8_t>(ACQUA + fascia_acqua(profondita, dentro) * 3 + 1);
}

void applica_palette_carta(Palette& p, StileCarta stile) {
    using namespace colori_carta;
    // acqua: VGA 1989 dal ciano chiaro al blu notte; carta nautica dagli azzurri saturi al bianco del profondo
    static constexpr Rgb VGA[] = {{176, 236, 244}, {96, 196, 228}, {40, 140, 200}, {24, 84, 160}, {14, 40, 104}, {6, 14, 56}};
    static constexpr Rgb NAUTICA[] = {{110, 176, 232}, {150, 200, 238}, {190, 222, 244}, {220, 236, 248}, {240, 247, 252}};
    for (int b = 0; b < 16; ++b) {
        const Rgb c = stile == StileCarta::nautica ? lungo(NAUTICA, b / 15.0) : lungo(VGA, b / 15.0);
        metti(p, ACQUA + b * 3, c, 0.80);   // versante in ombra (luce da nord-ovest)
        metti(p, ACQUA + b * 3 + 1, c);
        metti(p, ACQUA + b * 3 + 2, mescola(c, {255, 255, 255}, 0.18));
    }
    // terra: camoscio, ocra, bruno; la costa scura
    static constexpr Rgb TERRE[] = {{226, 206, 158}, {212, 184, 124}, {196, 162, 100}, {176, 140, 84}, {156, 120, 72}, {136, 104, 62}, {118, 90, 56}};
    metti(p, COSTA, {70, 46, 22});
    for (int i = 0; i < 7; ++i) metti(p, TERRA + i, TERRE[i]);
    const bool chiara = stile == StileCarta::nautica;
    metti(p, ISOBATA, chiara ? Rgb{110, 140, 175} : Rgb{150, 200, 230});
    metti(p, ISOBATA_PRINCIPALE, chiara ? Rgb{40, 70, 120} : Rgb{230, 245, 255});
    metti(p, SICUREZZA, {220, 0, 140});
    metti(p, RETICOLO, chiara ? Rgb{70, 70, 80} : Rgb{170, 190, 210});
    metti(p, GRIGLIA, chiara ? Rgb{150, 150, 160} : Rgb{90, 120, 150});
    static constexpr Rgb MATITE[] = {{24, 24, 24}, {205, 30, 30}, {30, 60, 205}, {20, 140, 40}, {235, 200, 0}, {250, 250, 250}};
    for (int i = 0; i < 6; ++i) metti(p, MATITA + i, MATITE[i]);
    static constexpr Rgb SIMBOLI[] = {{128, 224, 255}, {255, 128, 128}, {255, 255, 128}, {170, 255, 170}};   // APP-6: amico, ostile, ignoto, neutrale
    // sulla carta chiara le varianti scure (MIL-STD-2525C, «dark»; l'ignoto piu' scuro del giallo 225,220,0, che sul
    // bianco del profondo non si legge): gli attenuati sbiadiscono verso il bianco invece di scurirsi
    static constexpr Rgb SIMBOLI_SCURI[] = {{0, 107, 140}, {200, 0, 0}, {190, 150, 0}, {0, 160, 0}};
    for (int i = 0; i < 4; ++i) {
        metti(p, NTDS + i, chiara ? SIMBOLI_SCURI[i] : SIMBOLI[i]);
        if (chiara) metti(p, NTDS + 4 + i, mescola(SIMBOLI_SCURI[i], {255, 255, 255}, 0.45));
        else metti(p, NTDS + 4 + i, SIMBOLI[i], 0.6);
    }
    metti(p, ZONA, {200, 0, 160});
    metti(p, ZONA + 1, {0, 160, 160});
    metti(p, ZONA + 2, {255, 90, 90});
    metti(p, MAGENTA, {180, 0, 140});
    metti(p, CURSORE, {255, 255, 255});
    metti(p, EVIDENZA, {255, 230, 0});
}

namespace {
// Il fondale dell'Enhanced: un campione ogni blocco di 2x2 pixel; fascia, ombra dal gradiente (luce da nord-ovest),
// retinatura fra fasce vicine, costa e isobate dove la fascia cambia
void disegna_fondale_vga(Tela& t, const VistaCarta& v, const Fondale& fondo, double sicurezza_m) {
    using namespace colori_carta;
    const int cols = v.w / 2 + 2, righe = v.h / 2 + 2;
    std::vector<double> d(static_cast<std::size_t>(cols) * static_cast<std::size_t>(righe));
    const auto at = [&](int c, int r) -> double& {
        return d[static_cast<std::size_t>(std::clamp(r, 0, righe - 1)) * static_cast<std::size_t>(cols) + static_cast<std::size_t>(std::clamp(c, 0, cols - 1))];
    };
    for (int r = 0; r < righe; ++r)
        for (int c = 0; c < cols; ++c) at(c, r) = fondo.profondita(v.a_mondo({v.x + c * 2 + 1.0, v.y + r * 2 + 1.0}));
    const double passo_m = 2.0 * v.metri_per_pixel;
    for (int r = 0; r + 1 < righe; ++r)
        for (int c = 0; c + 1 < cols; ++c) {
            const double p = at(c, r);
            const int x0 = v.x + c * 2, y0 = v.y + r * 2;
            if (!std::isfinite(p)) {   // fuori dai dati: il fondo piu' profondo, senza ombre
                t.rettangolo_pieno(x0, y0, 2, 2, static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1));
                continue;
            }
            if (p <= 0.0) {   // terra, con la costa dove tocca l'acqua
                const bool costa = at(c + 1, r) > 0.0 || at(c - 1, r) > 0.0 || at(c, r + 1) > 0.0 || at(c, r - 1) > 0.0;
                t.rettangolo_pieno(x0, y0, 2, 2, static_cast<std::uint8_t>(costa ? COSTA : TERRA + fascia_terra(-p)));
                continue;
            }
            // ombra: la quota (meno la profondita') sale verso sud-est = versante che guarda la luce
            const double gx = at(c - 1, r) - at(c + 1, r), gy = at(c, r - 1) - at(c, r + 1);
            const double pendenza = (-gx - gy) / (4.0 * passo_m);
            const int ombra = pendenza > 0.01 ? 2 : pendenza < -0.01 ? 0 : 1;
            double dentro = 0.0;
            const int b = fascia_acqua(p, dentro);
            // carta d'epoca: le zone non rilevate col tratteggio diagonale (i fondali li' sono solo interpolati)
            const bool ignoto = fondo.carta_epoca() && !fondo.rilevato(v.a_mondo({x0 + 1.0, y0 + 1.0}));
            for (int j = 0; j < 2; ++j)
                for (int i = 0; i < 2; ++i) {
                    const int bb = b < 15 && dentro > BAYER[j][i] + 0.5 ? b + 1 : b;   // solo verso la fine della fascia
                    const bool tratto = ignoto && (x0 + i + y0 + j) % 6 == 0;
                    t.pixel(x0 + i, y0 + j, static_cast<std::uint8_t>(tratto ? ISOBATA : ACQUA + bb * 3 + ombra));
                }
            // contorno di sicurezza: dove il fondo passa la quota del battello piu' il margine (un tratto doppio)
            if (sicurezza_m > 0.0) {
                const bool basso = p < sicurezza_m;
                const double qd = at(c + 1, r), qs = at(c, r + 1);
                if ((std::isfinite(qd) && (qd < sicurezza_m) != basso) || (std::isfinite(qs) && (qs < sicurezza_m) != basso)) {
                    t.rettangolo_pieno(x0, y0, 2, 2, static_cast<std::uint8_t>(SICUREZZA));
                    continue;
                }
            }
            // isobate: il confine fra due fasce, a destra o sotto
            for (const double q : {at(c + 1, r), at(c, r + 1)}) {
                if (!(q > 0.0) || !std::isfinite(q)) continue;
                double tmp = 0.0;
                const int bq = fascia_acqua(q, tmp);
                if (bq == b) continue;
                const int confine = std::min(b, bq);
                t.pixel(x0, y0, static_cast<std::uint8_t>(PRINCIPALE[confine] ? ISOBATA_PRINCIPALE : ISOBATA));
            }
        }
}

// Segmento sulla tela (Bresenham); con `tratto` > 0 a trattini di quella lunghezza
void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t col, int tratto = 0) {
    int x0 = static_cast<int>(std::lround(a.x)), y0 = static_cast<int>(std::lround(a.y));
    const int x1 = static_cast<int>(std::lround(b.x)), y1 = static_cast<int>(std::lround(b.y));
    const int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (int k = 0; k < 4000; ++k) {
        if (!tratto || (k / tratto) % 2 == 0) t.pixel(x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Cerchio di raggio in metri attorno a un punto, a puntini
void cerchio(Tela& t, const VistaCarta& v, Vec2 c, double raggio_m, std::uint8_t col, int ogni = 2) {
    const Vec2 q = v.a_tela(c);
    const double r = raggio_m / v.metri_per_pixel;
    const int n = std::clamp(static_cast<int>(2.0 * PI * r), 12, 4000);
    for (int k = 0; k < n; k += ogni) {
        const double a = 2.0 * PI * k / n;
        t.pixel(static_cast<int>(q.x + r * std::sin(a)), static_cast<int>(q.y - r * std::cos(a)), col);
    }
}

// Ora del giorno «HHMM» di un istante di missione
std::string ora_hhmm(double ora_inizio_s, double t) {
    const int s = static_cast<int>(std::fmod(std::fmod(ora_inizio_s + t, 86400.0) + 86400.0, 86400.0));
    char b[16];
    std::snprintf(b, sizeof b, "%02d%02d", s / 3600, s / 60 % 60);
    return b;
}

// Gli strumenti dell'Enhanced (N2): tratti fissati, linee di rilevamento, eventi, datum, anelli, CPA e righello
void disegna_strumenti(Tela& t, const Font& f, const VistaCarta& v, const Missione& m, const CartaGiocatore& c, int bersaglio) {
    using namespace colori_carta;
    const auto* g = m.giocatore();
    const double ora = m.tempo();
    const bool vga = c.stile == StileCarta::vga;
    const auto matita = [&](int i) {   // sul blu della VGA il nero diventa bianco
        return static_cast<std::uint8_t>(MATITA + (vga && i == 0 ? 5 : i));
    };
    const auto scrivi = [&](Vec2 q, const std::string& s, std::uint8_t col) { f.scrivi(t, static_cast<int>(q.x), static_cast<int>(q.y), s, col, 1); };
    // anelli attorno al battello: fissi a 1, 2 e 5 nm; con il secondo modo anche il filo del Mk 48 (7,5 nm, docs/11 §1.1)
    if (c.anelli) {
        for (const double nm : {1.0, 2.0, 5.0}) {
            cerchio(t, v, g->pos, nm * MIGLIO, matita(0), 3);
            scrivi(v.a_tela({g->pos.x, g->pos.y + nm * MIGLIO}) + Vec2{3, -9}, std::to_string(static_cast<int>(nm)) + " NM", matita(0));
        }
        if (c.anelli >= 2) {
            cerchio(t, v, g->pos, 7.5 * MIGLIO, matita(1), 2);
            scrivi(v.a_tela({g->pos.x, g->pos.y - 7.5 * MIGLIO}) + Vec2{3, 2}, "FILO 7,5 NM", matita(1));
        }
    }
    // linee di rilevamento: dalla posizione del battello all'istante della misura, 40 nm, con l'ora in fondo
    for (const auto& l : c.lucido.linee()) {
        const Vec2 a = v.a_tela(l.origine);
        const double r = radianti(l.rilevamento), lungh = 40.0 * MIGLIO / v.metri_per_pixel;
        const Vec2 b{a.x + lungh * std::sin(r), a.y - lungh * std::cos(r)};
        const bool vecchia = !c.anni_89 && ora - l.tempo > 600.0;   // nel moderno le linee oltre i 10 minuti sbiadiscono
        linea(t, a, b, vecchia ? static_cast<std::uint8_t>(NTDS + 4 + 2) : matita(l.colore));
        const Vec2 q{a.x + 60.0 * std::sin(r), a.y - 60.0 * std::cos(r)};
        scrivi(q + Vec2{2, -3}, l.fonte + " " + ora_hhmm(c.ora_inizio_s, l.tempo), vecchia ? static_cast<std::uint8_t>(NTDS + 4 + 2) : matita(l.colore));
    }
    // tratti fissati col righello
    for (const auto& tr : c.lucido.tratti()) {
        const Vec2 a = v.a_tela(tr.a), b = v.a_tela(tr.b);
        linea(t, a, b, matita(0));
        scrivi(Vec2{(a.x + b.x) / 2.0 + 4, (a.y + b.y) / 2.0 - 10}, tr.etichetta, matita(0));
    }
    // datum: il cerchio di massimo avanzamento, che si allarga col tempo
    for (const auto& d : c.lucido.datum()) {
        cerchio(t, v, d.pos, raggio_datum(ora - d.tempo, d.velocita_max_m_s), static_cast<std::uint8_t>(ZONA + 2), 2);
        const Vec2 q = v.a_tela(d.pos);
        linea(t, q + Vec2{-4, 0}, q + Vec2{4, 0}, static_cast<std::uint8_t>(ZONA + 2));
        linea(t, q + Vec2{0, -4}, q + Vec2{0, 4}, static_cast<std::uint8_t>(ZONA + 2));
        scrivi(q + Vec2{6, -3}, "DATUM " + d.fonte + " " + ora_hhmm(c.ora_inizio_s, d.tempo), static_cast<std::uint8_t>(ZONA + 2));
    }
    // eventi: un triangolo col numero e l'ora (quattro cifre, come sul DRT)
    for (const auto& e : c.lucido.eventi()) {
        const Vec2 q = v.a_tela(e.pos);
        linea(t, q + Vec2{0, -5}, q + Vec2{-4, 3}, matita(1));
        linea(t, q + Vec2{-4, 3}, q + Vec2{4, 3}, matita(1));
        linea(t, q + Vec2{4, 3}, q + Vec2{0, -5}, matita(1));
        std::string s = "E" + std::to_string(e.numero) + " " + ora_hhmm(c.ora_inizio_s, e.tempo);
        if (e.tipo != Evento::Tipo::segno) s += " " + std::string(nome_evento(e.tipo));
        scrivi(q + Vec2{6, -3}, s, matita(1));
    }
    // CPA/TCPA sul bersaglio con la traccia solida (rotta e velocita' stimate della barra)
    if (c.cpa && bersaglio >= 0) {
        const BarraBersaglio bb = barra_bersaglio(m, bersaglio);
        const auto ct = g->contatti.find(bersaglio);
        if (bb.dati && ct != g->contatti.end()) {
            const Cpa k = cpa(g->pos, vettore(g->rotta, m.metri_al_secondo(g->velocita)), ct->second.pos_stimata,
                              vettore(bb.rotta, m.metri_al_secondo(bb.velocita_kn)));
            if (k.tempo_s > 0.0) {
                linea(t, v.a_tela(g->pos), v.a_tela(k.proprio), static_cast<std::uint8_t>(EVIDENZA), 3);
                linea(t, v.a_tela(ct->second.pos_stimata), v.a_tela(k.altro), static_cast<std::uint8_t>(EVIDENZA), 3);
                linea(t, v.a_tela(k.proprio), v.a_tela(k.altro), static_cast<std::uint8_t>(EVIDENZA));
            }
            const double nm = (c.anni_89 ? distanza_1989(k.distanza_m, c.scala()) : k.distanza_m) / MIGLIO;
            char riga[96];
            if (k.tempo_s > 0.0)
                std::snprintf(riga, sizeof riga, "CPA %.1f NM  TCPA %.0f MIN  RIL. %03.0f°", nm, k.tempo_s / 60.0, k.rilevamento);
            else std::snprintf(riga, sizeof riga, "CPA PASSATO");
            f.scrivi(t, v.x + 8, v.y + v.h - 24, riga, static_cast<std::uint8_t>(EVIDENZA), 1);
        } else {
            f.scrivi(t, v.x + 8, v.y + v.h - 24, "CPA: SERVE LA TRACCIA DEL BERSAGLIO", static_cast<std::uint8_t>(EVIDENZA), 1);
        }
    }
    // righello: estremo A (sul battello o libero) e B; nel moderno anche il cerchio del VRM attorno ad A
    if (c.righello.aperto) {
        const Vec2 a = c.righello.a_battello ? g->pos : c.righello.a, b = c.righello.b;
        const Vec2 qa = v.a_tela(a), qb = v.a_tela(b);
        if (!c.anni_89) cerchio(t, v, a, distanza(a, b), static_cast<std::uint8_t>(CURSORE), 3);
        linea(t, qa, qb, static_cast<std::uint8_t>(CURSORE));
        for (const auto& [q, attivo] : {std::pair{qa, !c.righello.sposta_b}, std::pair{qb, c.righello.sposta_b}})
            t.rettangolo_pieno(static_cast<int>(q.x) - 2, static_cast<int>(q.y) - 2, 5, 5, static_cast<std::uint8_t>(attivo ? EVIDENZA : CURSORE));
        const double kn = m.metri_al_secondo(g->velocita) / NODO;
        const std::string s = testo_misura(misura(a, b, m.metri_al_secondo(g->velocita)), kn, c.anni_89, c.scala());
        t.rettangolo_pieno(static_cast<int>(qb.x) + 6, static_cast<int>(qb.y) - 4, f.larghezza(s, 1) + 4, 10, colori::NERO);
        scrivi(qb + Vec2{8, -3}, s, static_cast<std::uint8_t>(EVIDENZA));
    }
}

// Gradi e primi «62°10'N» o «6°50'W»
std::string grado(double g, bool latitudine) {
    const char lettera = latitudine ? (g >= 0 ? 'N' : 'S') : (g >= 0 ? 'E' : 'W');
    const double a = std::fabs(g);
    int gradi = static_cast<int>(a), primi = static_cast<int>(std::lround((a - gradi) * 60.0));
    if (primi == 60) { ++gradi; primi = 0; }
    char b[24];
    std::snprintf(b, sizeof b, "%d°%02d'%c", gradi, primi, lettera);
    return b;
}

// Reticolo geografico (paralleli e meridiani) dalla georeferenziazione del fondale, con un passo tondo (1, 2, 5, 10,
// 15, 30 primi o 1 grado) che dia qualche linea nella vista; le etichette sui bordi
// `etichette`: i gradi lungo i bordi (spenti quando la cornice coi minuti li scrive gia')
void disegna_reticolo(Tela& t, const Font& f, const VistaCarta& v, const Fondale& fondo, std::uint8_t col, bool etichette = true) {
    double lat0 = 1e9, lat1 = -1e9, lon0 = 1e9, lon1 = -1e9;
    for (const auto& [px, py] : {std::pair{v.x, v.y}, std::pair{v.x + v.w, v.y}, std::pair{v.x, v.y + v.h}, std::pair{v.x + v.w, v.y + v.h}}) {
        const Geografiche g = fondo.geografiche(v.a_mondo({static_cast<double>(px), static_cast<double>(py)}));
        lat0 = std::min(lat0, g.lat);
        lat1 = std::max(lat1, g.lat);
        lon0 = std::min(lon0, g.lon);
        lon1 = std::max(lon1, g.lon);
    }
    const auto passo = [](double ampiezza) {
        for (const double p : {1.0, 2.0, 5.0, 10.0, 15.0, 30.0, 60.0, 120.0})
            if (ampiezza * 60.0 / p <= 6.0) return p / 60.0;
        return 2.0;
    };
    const double plat = passo(lat1 - lat0), plon = passo(lon1 - lon0);
    for (double la = std::ceil(lat0 / plat) * plat; la <= lat1; la += plat) {   // paralleli
        Vec2 prima{};
        for (int k = 0; k <= 64; ++k) {
            const Vec2 q = v.a_tela(fondo.proiettata({la, lon0 + (lon1 - lon0) * k / 64.0}));
            if (k) linea(t, prima, q, col, 2);
            prima = q;
        }
        const Vec2 q = v.a_tela(fondo.proiettata({la, lon0}));
        if (etichette) f.scrivi(t, v.x + 2, static_cast<int>(q.y) - 9, grado(la, true), col, 1);
    }
    for (double lo = std::ceil(lon0 / plon) * plon; lo <= lon1; lo += plon) {   // meridiani
        Vec2 prima{};
        for (int k = 0; k <= 64; ++k) {
            const Vec2 q = v.a_tela(fondo.proiettata({lat0 + (lat1 - lat0) * k / 64.0, lo}));
            if (k) linea(t, prima, q, col, 2);
            prima = q;
        }
        const Vec2 q = v.a_tela(fondo.proiettata({lat0, lo}));
        if (etichette) f.scrivi(t, static_cast<int>(q.x) + 2, v.y + v.h - 10, grado(lo, false), col, 1);
    }
}

// Note della carta d'epoca (ED, PA, acqua scolorita, profondita' minore riportata) e cartiglio dell'edizione
void disegna_note_carta(Tela& t, const Font& f, const VistaCarta& v, const Fondale& fondo, bool urss, bool riga_carta) {
    const auto col = static_cast<std::uint8_t>(colori_carta::MAGENTA);
    for (const auto& a : fondo.annotazioni()) {
        const Vec2 q = v.a_tela(a.pos);
        if (q.x < v.x || q.y < v.y || q.x >= v.x + v.w || q.y >= v.y + v.h) continue;
        std::string s;
        switch (a.tipo) {
            case Annotazione::Tipo::ed: s = "ED"; break;
            case Annotazione::Tipo::pa: s = "PA"; break;
            case Annotazione::Tipo::acqua_scolorita: s = "ACQUA SCOL."; break;
            case Annotazione::Tipo::profondita_minore: s = std::to_string(static_cast<int>(std::lround(a.profondita_m))) + " M"; break;
        }
        if (a.anno) s += " (" + std::to_string(a.anno) + ")";
        t.rettangolo_pieno(static_cast<int>(q.x) - 1, static_cast<int>(q.y) - 1, 3, 3, col);
        simboli::scrivi(t, f, static_cast<int>(q.x) + 4, static_cast<int>(q.y) - 3, s, col, 1);   // con l'alone: sul blu scuro non si legge
    }
    if (!riga_carta) return;   // con la mobilia l'edizione e il tratteggio stanno nel cartiglio (la carta grande)
    char riga[80];
    std::snprintf(riga, sizeof riga, "CARTA %s ED. %d - ZONE TRATTEGGIATE NON RILEVATE", urss ? "VMF" : "NATO", fondo.edizione());
    f.scrivi(t, v.x + v.w - 8 - f.larghezza(riga, 1), v.y + 18, riga, col, 1);   // sotto la riga dell'ingrandimento
}

// Simbolo APP-6/NTDS (moderno) o di matita (1989): telaio per affiliazione (amico tondo, ostile rombo, ignoto
// ottagono al posto del quadrifoglio, neutrale quadrato), solo la meta' inferiore per il subacqueo e la superiore per
// l'aereo, chiusa da un tratto orizzontale
void disegna_simbolo(Tela& t, const Font& f, Vec2 q, const Segno& s, bool anni_89, bool vga) {
    using namespace colori_carta;
    static constexpr int NTDS_DI[] = {1, 2, 0, 3};          // ostile, ignoto, amico, neutrale -> indici NTDS
    static constexpr int MATITA_DI[] = {1, 0, 2, 3};         // rosso, nero, blu, verde
    const int a = static_cast<int>(s.affiliazione);
    std::uint8_t col = static_cast<std::uint8_t>(anni_89 ? MATITA + MATITA_DI[a] : NTDS + NTDS_DI[a]);
    if (anni_89 && vga && MATITA_DI[a] == 0) col = static_cast<std::uint8_t>(MATITA + 5);
    const int x = static_cast<int>(q.x), y = static_cast<int>(q.y), r = 5;
    simboli::Pennello pn;   // col contorno nero del modulo comune (grafica/simboli_carta), come gli altri simboli della carta
    const auto tieni = [&](int dy) {   // la meta' del telaio da disegnare
        if (s.dominio == Segno::Dominio::subacqueo) return dy >= 0;
        if (s.dominio == Segno::Dominio::aereo) return dy <= 0;
        return true;
    };
    if (s.dominio == Segno::Dominio::croce) {
        pn.linea({q.x - 4, q.y - 4}, {q.x + 4, q.y + 4}, col);
        pn.linea({q.x - 4, q.y + 4}, {q.x + 4, q.y - 4}, col);
    } else if (s.dominio != Segno::Dominio::nota) {
        for (int dy = -r; dy <= r; ++dy)
            for (int dx = -r; dx <= r; ++dx) {
                if (!tieni(dy)) continue;
                const int ax = std::abs(dx), ay = std::abs(dy);
                bool bordo = false;
                switch (s.affiliazione) {
                    case Segno::Affiliazione::amico: bordo = std::abs(dx * dx + dy * dy - r * r) <= r; break;
                    case Segno::Affiliazione::ostile: bordo = ax + ay == r + 1; break;
                    case Segno::Affiliazione::neutrale: bordo = std::max(ax, ay) == r; break;
                    case Segno::Affiliazione::ignoto: bordo = (std::max(ax, ay) == r && std::min(ax, ay) <= 2) || ax + ay == r + 3; break;
                }
                if (bordo) pn.pixel(x + dx, y + dy, col);
            }
        if (s.dominio != Segno::Dominio::superficie) pn.orizzontale(x - r, x + r, y, col);
    }
    pn.stendi(t);
    if (!s.testo.empty()) simboli::scrivi(t, f, x + 8, y - 3, s.testo, col, 1);
}

// Zone da evitare dell'utente: il cerchio e un tratteggio rado dentro
void disegna_zona(Tela& t, const Font& f, const VistaCarta& v, const ZonaUtente& z) {
    const auto col = static_cast<std::uint8_t>(colori_carta::ZONA);
    cerchio(t, v, z.centro, z.raggio_m, col, 1);
    const Vec2 q = v.a_tela(z.centro);
    const double r = z.raggio_m / v.metri_per_pixel;
    for (int y = static_cast<int>(q.y - r); y <= static_cast<int>(q.y + r); ++y)
        for (int x = static_cast<int>(q.x - r); x <= static_cast<int>(q.x + r); ++x)
            if ((x + y) % 8 == 0 && (x - q.x) * (x - q.x) + (y - q.y) * (y - q.y) <= r * r) t.pixel(x, y, col);
    f.scrivi(t, static_cast<int>(q.x) - 10, static_cast<int>(q.y - r) - 10, "ZONA", col, 1);
}

// Zone dello spazio d'acqua ricevute dalla radio (R2): l'area d'azione in verde acqua, la zona libera ASW in rosso a
// trattini, le protette (fisse e mobili attorno a un battello amico) in magenta tratteggiato; col nome e la serie
void disegna_zona_radio(Tela& t, const Font& f, const VistaCarta& v, const Missione::ZonaCarta& z) {
    using namespace colori_carta;
    const bool protetta = z.zona.tipo == TipoZona::protetta || z.zona.tipo == TipoZona::protetta_mobile;
    const auto col = static_cast<std::uint8_t>(protetta ? ZONA : z.zona.tipo == TipoZona::azione ? ZONA + 1 : ZONA + 2);
    const int tratto = z.zona.tipo == TipoZona::libera_asw ? 4 : 0;
    Vec2 etichetta{};
    if (!z.vertici.empty()) {
        for (std::size_t i = 0; i < z.vertici.size(); ++i)
            linea(t, v.a_tela(z.vertici[i]), v.a_tela(z.vertici[(i + 1) % z.vertici.size()]), col, tratto);
        etichetta = v.a_tela(z.vertici.front());
        if (protetta) {   // tratteggio rado dentro, sul riquadro che la contiene
            double x0 = 1e18, y0 = 1e18, x1 = -1e18, y1 = -1e18;
            for (const auto& p : z.vertici) {
                const Vec2 q = v.a_tela(p);
                x0 = std::min(x0, q.x); y0 = std::min(y0, q.y); x1 = std::max(x1, q.x); y1 = std::max(y1, q.y);
            }
            for (int y = std::max(static_cast<int>(y0), v.y); y <= std::min(static_cast<int>(y1), v.y + v.h - 1); ++y)
                for (int x = std::max(static_cast<int>(x0), v.x); x <= std::min(static_cast<int>(x1), v.x + v.w - 1); ++x)
                    if ((x + y) % 8 == 0 && dentro_poligono(z.vertici, v.a_mondo({x + 0.5, y + 0.5}))) t.pixel(x, y, col);
        }
    } else if (z.centro) {
        cerchio(t, v, *z.centro, z.raggio_m, col, tratto ? 3 : 1);
        etichetta = v.a_tela({z.centro->x, z.centro->y + z.raggio_m});
    }
    static constexpr const char* NOMI[] = {"AZIONE", "LIBERA ASW", "PROTETTA", "PROTETTA MOBILE"};
    std::string s = std::string(NOMI[static_cast<int>(z.zona.tipo)]) + " " + z.zona.id;
    if (z.zona.serie) s += " (" + std::to_string(z.zona.serie) + ")";
    if (z.zona.dubbia) s += " DUBBIA";   // da un messaggio che non si autentica
    f.scrivi(t, static_cast<int>(etichetta.x) + 3, static_cast<int>(etichetta.y) - 10, s, col, 1);
}

// Regolo di Ekelund: la striscia lungo la rotta stimata con una tacca per linea, e la lettura
void disegna_ekelund(Tela& t, const Font& f, const VistaCarta& v, const CartaGiocatore& c, const Missione& m) {
    using namespace colori_carta;
    const auto linee = c.lucido.linee_di(c.ekelund.fonte);
    char riga[128];
    if (linee.size() < 2) {
        std::snprintf(riga, sizeof riga, "EKELUND %s: SERVONO ALMENO DUE LINEE DI RILEVAMENTO (5)", c.ekelund.fonte.c_str());
        f.scrivi(t, v.x + 8, v.y + v.h - 36, riga, static_cast<std::uint8_t>(EVIDENZA), 1);
        return;
    }
    const Ekelund e = regolo_ekelund(linee, c.ekelund.distanza_m, c.ekelund.rotta, m.metri_al_secondo(c.ekelund.velocita_kn));
    const auto col = static_cast<std::uint8_t>(EVIDENZA);
    linea(t, v.a_tela(e.tacche.front()), v.a_tela(e.tacche.back()), col);
    const Vec2 perp = vettore(c.ekelund.rotta + 90.0, 1.0);
    for (std::size_t i = 0; i < e.tacche.size(); ++i) {
        const Vec2 q = v.a_tela(e.tacche[i]);
        linea(t, {q.x - perp.x * 4, q.y + perp.y * 4}, {q.x + perp.x * 4, q.y - perp.y * 4}, col);
    }
    std::snprintf(riga, sizeof riga, "EKELUND %s  ROTTA %03.0f°  %.1f KN  DIST. %.1f NM  SCARTO %.1f°  (INVIO AL MATE)", c.ekelund.fonte.c_str(),
                  normalizza_360(c.ekelund.rotta), c.ekelund.velocita_kn, c.ekelund.distanza_m / MIGLIO, e.scarto_gradi);
    f.scrivi(t, v.x + 8, v.y + v.h - 36, riga, col, 1);
}

// Pannello tempo-rilevamento a destra: un punto per linea negli ultimi 20 minuti, la retta ai minimi quadrati e il rateo
void disegna_tempo_rilevamento(Tela& t, const Font& f, const VistaCarta& v, const CartaGiocatore& c, double ora) {
    using namespace colori_carta;
    const int px = v.x + v.w - 140, py = v.y + 32, pw = 132, ph = 230;
    t.rettangolo_pieno(px, py, pw, ph, colori::NERO);
    const auto bordo = static_cast<std::uint8_t>(RETICOLO);
    t.linea_orizzontale(px, px + pw - 1, py, bordo);
    t.linea_orizzontale(px, px + pw - 1, py + ph - 1, bordo);
    t.linea_verticale(px, py, py + ph - 1, bordo);
    t.linea_verticale(px + pw - 1, py, py + ph - 1, bordo);
    f.scrivi(t, px + 4, py + 3, "TEMPO-RILEVAMENTO", bordo, 1);
    constexpr double FINESTRA = 20.0 * 60.0;
    std::vector<std::string> fonti;
    for (const auto& l : c.lucido.linee())
        if (ora - l.tempo <= FINESTRA && std::find(fonti.begin(), fonti.end(), l.fonte) == fonti.end()) fonti.push_back(l.fonte);
    if (fonti.empty()) {
        f.scrivi(t, px + 4, py + 16, "NESSUNA LINEA (5)", bordo, 1);
        return;
    }
    // scala dei rilevamenti: attorno all'ultima linea, quanto basta a contenere tutte
    const double base = c.lucido.linee().back().rilevamento;
    double lo = 0.0, hi = 0.0;
    for (const auto& l : c.lucido.linee())
        if (ora - l.tempo <= FINESTRA) {
            const double d = normalizza_180(l.rilevamento - base);
            lo = std::min(lo, d);
            hi = std::max(hi, d);
        }
    lo -= 5.0;
    hi += 5.0;
    const int x0 = px + 6, w = pw - 12, y0 = py + 16, h = ph - 40;
    const auto xy = [&](double ril, double tempo) {
        return Vec2{x0 + (normalizza_180(ril - base) - lo) / (hi - lo) * w, y0 + (ora - tempo) / FINESTRA * h};
    };
    for (int k = 0; k <= 4; ++k) {   // tacche ogni 5 minuti
        const int y = y0 + k * h / 4;
        t.linea_orizzontale(x0, x0 + 3, y, bordo);
        f.scrivi(t, x0 + 5, y - 3, k ? "-" + std::to_string(k * 5) + "'" : "ORA", bordo, 1);
    }
    int riga_testo = 0;
    for (std::size_t i = 0; i < fonti.size(); ++i) {
        std::vector<LineaRilevamento> linee;
        for (const auto& l : c.lucido.linee_di(fonti[i]))
            if (ora - l.tempo <= FINESTRA) linee.push_back(l);
        const std::uint8_t col = static_cast<std::uint8_t>(MATITA + (linee.front().colore == 0 ? 5 : linee.front().colore));
        for (const auto& l : linee) {
            const Vec2 q = xy(l.rilevamento, l.tempo);
            t.rettangolo_pieno(static_cast<int>(q.x) - 1, static_cast<int>(q.y) - 1, 3, 3, col);
        }
        if (const auto r = retta_rilevamento(linee, ora)) {
            const double t0 = linee.front().tempo;
            linea(t, xy(r->rilevamento, ora), xy(r->rilevamento - r->rateo_gradi_min * (ora - t0) / 60.0, t0), col);
            char e[48];
            std::snprintf(e, sizeof e, "%s %+.1f°/MIN", fonti[i].c_str(), r->rateo_gradi_min);
            if (riga_testo < 2) f.scrivi(t, px + 4, py + ph - 22 + riga_testo++ * 10, e, col, 1);
        }
    }
}

// Rosa attorno al battello, come il «bug» del DRT: tacche ogni 10 gradi, cifre ogni 30
// Le mine posate dal giocatore (M2b, nucleo/mine.hpp): il simbolo a cerchio con quattro punte, nel colore dello stato
// (in corsa bianco, armandosi giallo, armata rosso chiaro, inerte grigio, scoppiata una stella), il tipo e l'ora
// d'armamento; la mina mobile in corsa col tratteggio fino al punto di posa, l'armata col cerchio d'innesco. Anche i
// campi minati della sua parte dello scenario (posatore -1, la parte del giocatore: la I17 di trailer-1, 30/10/2026), che
// il comandante conosce dal briefing: lo stesso simbolo, «NOTA» e niente ora d'armamento, armati o no
void disegna_mine(Tela& t, const Font& f, const VistaCarta& v, const Missione& m, int giocatore, double ora_inizio_s) {
    const auto gi = m.unita().find(giocatore);
    const int lato = gi != m.unita().end() ? (gi->second.flag & 0x18) : -1;
    for (const Mina& x : m.campo_minato().mine()) {
        const bool nota = x.posatore < 0 && lato > 0 && x.lato == lato;   // della sua parte, dallo scenario
        if (x.posatore != giocatore && !nota) continue;
        const Vec2 q = v.a_tela(x.pos);
        const int cx = static_cast<int>(q.x), cy = static_cast<int>(q.y);
        if (cx < v.x - 20 || cx > v.x + v.w + 20 || cy < v.y - 20 || cy > v.y + v.h + 20) continue;
        const std::uint8_t col = x.stato == Mina::Stato::in_corsa    ? colori::BIANCO
                                 : x.stato == Mina::Stato::armandosi ? colori::GIALLO
                                 : x.stato == Mina::Stato::armata    ? ROSSO_CHIARO
                                                                     : colori::GRIGIO;
        if (x.stato == Mina::Stato::in_corsa && x.corsa_m > 0.0) {
            const double a = radianti(x.rotta);
            linea(t, q, v.a_tela({x.pos.x + x.corsa_m * std::sin(a), x.pos.y + x.corsa_m * std::cos(a)}), col, 3);
        }
        // simbolo e scritte con l'ombra nera un pixel in basso a destra, per leggerli sul mare chiaro
        auto scritta = [&](int x0, int y0, const std::string& s, std::uint8_t c) {
            f.scrivi(t, x0 + 1, y0 + 1, s, colori::NERO, 1);
            f.scrivi(t, x0, y0, s, c, 1);
        };
        if (x.stato == Mina::Stato::scoppiata) {
            scritta(cx - f.larghezza("*", 1) / 2, cy - 3, "*", colori::GRIGIO);
            continue;
        }
        if (x.stato == Mina::Stato::armata)
            if (const DatiMina* d = m.campo_minato().dati(x.tipo); d && d->raggio_m > 0.0) cerchio(t, v, x.pos, d->raggio_m, col, 3);
        for (int o = 1; o >= 0; --o) {   // il cerchio di raggio 3 e le quattro punte, prima l'ombra
            const std::uint8_t c = o ? colori::NERO : col;
            for (int k = 0; k < 12; ++k) {
                const double a = 2.0 * PI * k / 12.0;
                t.pixel(cx + o + static_cast<int>(std::lround(3.0 * std::sin(a))), cy + o - static_cast<int>(std::lround(3.0 * std::cos(a))), c);
            }
            for (int k = 4; k <= 6; ++k)
                t.pixel(cx + o, cy + o - k, c), t.pixel(cx + o, cy + o + k, c), t.pixel(cx + o - k, cy + o, c), t.pixel(cx + o + k, cy + o, c);
        }
        std::string tipo = x.tipo;
        for (auto& ch : tipo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (nota) tipo += " NOTA";
        else if (x.stato == Mina::Stato::armandosi) tipo += " ARMA " + ora_hhmm(ora_inizio_s, x.armata_t);
        if (!nota && x.stato == Mina::Stato::inerte) tipo += " INERTE";
        scritta(cx + 9, cy - 3, tipo, col);
    }
    // le mine nemiche viste dal BQS-15 (runtime, S06): alla posizione stimata, in rosso chiaro, ormeggiata o sul fondo
    for (const auto& s : m.mine_scoperte()) {
        const Vec2 q = v.a_tela(s.pos);
        const int cx = static_cast<int>(q.x), cy = static_cast<int>(q.y);
        if (cx < v.x - 20 || cx > v.x + v.w + 20 || cy < v.y - 20 || cy > v.y + v.h + 20) continue;
        constexpr std::uint8_t COLORE_MINA = 12;   // rosso chiaro: il magenta e' delle quote d'epoca
        for (int o = 1; o >= 0; --o) {
            const std::uint8_t c = o ? colori::NERO : COLORE_MINA;
            for (int k = 0; k < 12; ++k) {
                const double a = 2.0 * PI * k / 12.0;
                t.pixel(cx + o + static_cast<int>(std::lround(3.0 * std::sin(a))), cy + o - static_cast<int>(std::lround(3.0 * std::cos(a))), c);
            }
            for (int k = 4; k <= 6; ++k)
                t.pixel(cx + o, cy + o - k, c), t.pixel(cx + o, cy + o + k, c), t.pixel(cx + o - k, cy + o, c), t.pixel(cx + o + k, cy + o, c);
        }
        const std::string scritta_mina = std::string("MINA ") + (s.ormeggiata ? "ORM. " : "FONDO ") + ora_hhmm(ora_inizio_s, s.scoperta_t);
        f.scrivi(t, cx + 10, cy - 2, scritta_mina, colori::NERO, 1);
        f.scrivi(t, cx + 9, cy - 3, scritta_mina, COLORE_MINA, 1);
    }
}

void disegna_rosa(Tela& t, const Font& f, Vec2 b, int raggio, std::uint8_t colore, bool alone) {
    for (int g = 0; g < 360; g += 10) {
        const double a = radianti(g);
        const int lunga = g % 30 == 0 ? 6 : 3;
        for (int k = 0; k < lunga; ++k)
            t.pixel(static_cast<int>(b.x + (raggio - k) * std::sin(a)), static_cast<int>(b.y - (raggio - k) * std::cos(a)),
                    colore);
        if (g % 30 == 0) {
            char e[8];
            std::snprintf(e, sizeof e, "%02d", g / 10);
            const int cx = static_cast<int>(b.x + (raggio + 8) * std::sin(a)) - f.larghezza(e, 1) / 2;
            const int cy = static_cast<int>(b.y - (raggio + 8) * std::cos(a)) - 3;
            if (alone) simboli::scrivi(t, f, cx, cy, e, colore, 1);
            else f.scrivi(t, cx, cy, e, colore, 1);
        }
    }
}
}  // namespace

Vec2 VistaCarta::a_tela(Vec2 p) const {
    return {x + w / 2.0 + (p.x - centro.x) / metri_per_pixel, y + h / 2.0 - (p.y - centro.y) / metri_per_pixel};
}

Vec2 VistaCarta::a_mondo(Vec2 q) const {
    return {centro.x + (q.x - x - w / 2.0) * metri_per_pixel, centro.y - (q.y - y - h / 2.0) * metri_per_pixel};
}

VistaCarta vista_carta(const Missione& m, const CartaGiocatore& c) {
    VistaCarta v;
    // con la navigazione realistica la carta sta dove il giocatore crede di essere (nucleo/navigazione)
    if (const auto* g = m.giocatore()) v.centro = m.con_navigazione() ? m.posizione_stimata(g->id) : g->pos;
    v.metri_per_pixel = c.scala() * m.metri_per_unita() / 2.0;
    return v;
}

std::vector<SegnoSuCarta> segni_su_carta(const VistaCarta& v, const Missione& m, const std::vector<SegnoContatto>& segni) {
    std::vector<SegnoSuCarta> r;
    const auto* g = m.giocatore();
    if (!g) return r;
    const Vec2 b = v.a_tela(g->pos);
    auto x_sul_rilevamento = [&](double ril) {   // la X: solo il rilevamento, a 25 pixel originali
        const double a = radianti(ril);
        return Vec2{b.x + 50.0 * std::sin(a), b.y - 50.0 * std::cos(a)};
    };
    for (const auto& s : segni) {
        const Vec2 q = s.traccia() ? v.a_tela(s.pos_stimata) : x_sul_rilevamento(s.rilevamento);
        if (q.x < v.x || q.y < v.y || q.x >= v.x + v.w || q.y >= v.y + v.h) continue;
        r.push_back({s, q});
        if (s.specchio && !s.traccia()) {   // schiera (Enhanced): l'altra X dell'ambiguita', finche' non si risolve
            SegnoContatto sp = s;
            sp.rilevamento = *s.specchio;
            r.push_back({sp, x_sul_rilevamento(*s.specchio)});
        }
    }
    return r;
}

void disegna_meteo_carta(Tela& t, const Font& f, const VistaCarta& v, const Missione& m, const Meteo& meteo, Vec2 battello,
                         bool testa_fuori) {
    constexpr double VICINO_M = 9260.0, CELLE_M = 100000.0, FULMINI_S = 600.0;
    const double tempo_s = m.tempo();
    const StatoMeteo bollettino = meteo.previsione(m.ultimo_bollettino(tempo_s), tempo_s);
    auto noto = [&](Vec2 p) { return distanza(p, battello) <= VICINO_M ? meteo.in(p) : bollettino; };
    StratiMeteo s;
    s.a_tela = [&v](Vec2 p) { return v.a_tela(p); };
    s.a_mondo = [&v](Vec2 q) { return v.a_mondo(q); };
    s.area = {v.x, v.y, v.w, v.h};
    s.passo_px = 192;   // la carta pulita (scelta dell'utente del 29/10/2026): una barba ogni 192 px, il mare nella legenda
    s.campioni_px = 8;
    s.vento = [&](Vec2 p, double& u, double& da) {
        const StatoMeteo st = noto(p);
        u = st.vento_m_s;
        da = st.vento_da_gradi;
    };
    // il clima delle carte del ghiaccio: la deriva e le polynye del vetrino di Dio il battello le sa solo dalla volta che misura
    // (disegna_volta_carta); lo spettatore le vede sulla sua carta (app/replay, polynye_a)
    s.ghiaccio = [&meteo](Vec2 p) { return meteo.in(p).ghiaccio; };
    s.nebbia = [&](Vec2 p) {
        if (distanza(p, battello) <= VICINO_M) return testa_fuori && meteo.in(p).visibilita_m < 1000.0;
        return bollettino.nebbia != Nebbia::nessuna;
    };
    if (testa_fuori)
        for (const auto& c : meteo.celle(tempo_s))
            if (distanza(c.pos, battello) <= CELLE_M) s.celle.push_back({c.pos, c.raggio_m});
    s.battello = battello;
    for (const auto& fs : m.fulmini_sentiti())
        if (fs.tempo <= tempo_s && tempo_s - fs.tempo <= FULMINI_S) s.rilevamenti_fulmini.push_back(fs.rilevamento);
    // matite della carta: bianco per vento, mare e ghiaccio, rosso per le celle; reticolo per la nebbia
    s.colori.vento = static_cast<std::uint8_t>(colori_carta::RETICOLO);   // la barba grigia: non si confonde con le quote
    s.colori.mare = s.colori.ghiaccio = static_cast<std::uint8_t>(colori_carta::MATITA + 5);
    s.colori.celle = static_cast<std::uint8_t>(colori_carta::MATITA + 1);
    s.colori.nebbia = static_cast<std::uint8_t>(colori_carta::RETICOLO);
    disegna_strati_meteo(t, f, s);
}

namespace {
// La legenda del meteo (la carta pulita, scelta dell'utente del 29/10/2026): il vento e il mare che il battello misura,
// una volta sola, nell'angolo in alto a destra di r, coi colori del cartiglio. Restituisce il suo rettangolo
Rettangolo legenda_meteo(Tela& t, const Font& f, Rettangolo r, const Meteo& meteo, Vec2 battello) {
    const auto nero = static_cast<std::uint8_t>(colori_carta::MATITA), bianco = static_cast<std::uint8_t>(colori_carta::MATITA + 5);
    const StatoMeteo st = meteo.in(battello);
    char r1[48], r2[48];
    std::snprintf(r1, sizeof r1, "VENTO %03.0f / %.0f KN", st.vento_da_gradi, st.vento_m_s * 1.943844);
    std::snprintf(r2, sizeof r2, "MARE %d (DOUGLAS)", st.stato_mare());
    const int passo = f.righe() + 3, lw = 104, lh = 3 * passo + 6;
    const Rettangolo l{r.x + r.w - lw - 6, r.y + 6, lw, lh};
    t.rettangolo_pieno(l.x, l.y, l.w, l.h, bianco);
    t.linea_orizzontale(l.x, l.x + l.w - 1, l.y, nero), t.linea_orizzontale(l.x, l.x + l.w - 1, l.y + l.h - 1, nero);
    t.linea_verticale(l.x, l.y, l.y + l.h - 1, nero), t.linea_verticale(l.x + l.w - 1, l.y, l.y + l.h - 1, nero);
    f.scrivi(t, l.x + 4, l.y + 4, "METEO, BOLLETTINO", nero, 1);
    f.scrivi(t, l.x + 4, l.y + 4 + passo, r1, nero, 1);
    f.scrivi(t, l.x + 4, l.y + 4 + 2 * passo, r2, nero, 1);
    return l;
}

FondaleVga& fondale_in_cache(bool urss) {   // una per parte: le due carte del video d'apertura hanno fondale e font diversi
    static FondaleVga cache[2];
    return cache[urss ? 1 : 0];
}
const Font *font_isobate_nato = nullptr, *font_isobate_urss = nullptr;
}  // namespace

std::string cartella_dati_carta;
void imposta_cartella_dati_carta(std::string cartella) { cartella_dati_carta = std::move(cartella); }

// i toponimi del teatro, letti una volta sola
const std::vector<Toponimo>& toponimi_del_teatro(const std::string& teatro) {
    static std::map<std::string, std::vector<Toponimo>> letti;
    auto it = letti.find(teatro);
    if (it == letti.end())
        it = letti.emplace(teatro, cartella_dati_carta.empty() ? std::vector<Toponimo>{}
                                                               : carica_toponimi(file_toponimi(cartella_dati_carta, teatro))).first;
    return it->second;
}

void scrivi_contornato(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t colore, std::uint8_t contorno) {
    f.scrivi(t, x - 1, y, s, contorno, 1);
    f.scrivi(t, x + 1, y, s, contorno, 1);
    f.scrivi(t, x, y - 1, s, contorno, 1);
    f.scrivi(t, x, y + 1, s, contorno, 1);
    f.scrivi(t, x, y, s, colore, 1);
}

void imposta_font_isobate(const Font* nato, const Font* urss) {
    font_isobate_nato = nato;
    font_isobate_urss = urss;
    fondale_in_cache(false).invalida();
    fondale_in_cache(true).invalida();
}

void disegna_carta(Tela& t, const Font& f, const Missione& m, const CartaGiocatore& c, const std::vector<SegnoContatto>& segni,
                   int bersaglio, std::optional<Vec2> cursore, const VistaCarta* area) {
    const VistaCarta v = area ? *area : vista_carta(m, c);
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    // sfondo: il fondale campionato ogni due pixel, o il riempimento uniforme (B)
    // nell'Enhanced la carta della parte del giocatore (d'epoca, se la partita la usa; altrimenti il fondale vero)
    const Fondale* fondo = c.restyle ? m.fondale_carta(c.urss ? Parte::urss : Parte::nato) : m.fondale();
    if (c.restyle && fondo && c.stile != StileCarta::foglio && c.livelli.fondale) {   // Fase 3: la carta del kit
        const GeometriaCarta g{v.x, v.y, v.w, v.h, v.centro, v.metri_per_pixel};
        OpzioniCarta o{c.stile, c.contorno_sicurezza_m, true, c.urss ? font_isobate_urss : font_isobate_nato};
        o.scandagli = c.livelli.mobilia;   // nelle zone rilevate della carta d'epoca, sotto contatti e simboli
        o.passo_tratteggio = 12;            // la carta pulita: il tratteggio delle zone non rilevate piu' leggero
        if (!area) o.tempo_s = m.tempo();   // il debriefing resta esatto
        fondale_in_cache(c.urss).disegna(t, g, *fondo, o);
        t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    } else if (c.restyle && (c.stile == StileCarta::foglio || !c.livelli.fondale)) {   // foglio di plottaggio: carta chiara
        t.rettangolo_pieno(v.x, v.y, v.w, v.h, static_cast<std::uint8_t>(colori_carta::ACQUA + 15 * 3 + 2));
    } else if (c.sfondo_uniforme || !fondo) {
        t.rettangolo_pieno(v.x, v.y, v.w, v.h, colori::CIANO_CHIARO);
    } else {
        for (int py = v.y; py < v.y + v.h; py += 2)
            for (int px = v.x; px < v.x + v.w; px += 2)
                t.rettangolo_pieno(px, py, 2, 2, colore_fondale(fondo->profondita(v.a_mondo({px + 1.0, py + 1.0}))));
    }
    const auto* g = m.giocatore();
    // strati meteo dell'Enhanced (livello spento nel classico): sopra il fondo, sotto rotte e contatti
    if (c.livelli.meteo && g)
        if (const Meteo* me = m.meteo()) {
            disegna_meteo_carta(t, f, v, m, *me, g->pos, g->quota <= 20.0);   // testa fuori fino a 20 m
            t.imposta_ritaglio({v.x, v.y, v.w, v.h});   // gli strati tolgono il loro ritaglio
        }
    if (!g) { t.togli_ritaglio(); return; }
    // navigazione realistica (Enhanced, sulla carta della stazione): tutto quello che viene dai sensori del battello
    // (il battello, i contatti, le scie, le linee di rilevamento, le cadute, le mine viste) si disegna rispetto alla
    // posizione stimata, cioe' spostato dell'errore di navigazione; fondale, punti di rotta, zone, intelligence e segni
    // del lucido restano dove sono. Il debriefing (con `area`) mostra il vero
    Vec2 errore{};
    if (!area && m.con_navigazione()) {
        const Vec2 st = m.posizione_stimata(g->id);
        errore = {st.x - g->pos.x, st.y - g->pos.y};
    }
    VistaCarta vs = v;
    vs.centro = {v.centro.x - errore.x, v.centro.y - errore.y};
    // come l'originale (4C8C): contorno a croce nero, rosso scuro per le X dei contatti
    // nell'Enhanced l'alone nero del modulo comune su tutti i lati (grafica/simboli_carta)
    auto testo = [&](Vec2 q, const std::string& s, std::uint8_t col, std::uint8_t contorno = colori::NERO) {
        const int x = static_cast<int>(q.x) - f.larghezza(s, 1) / 2, y = static_cast<int>(q.y) - 3;
        if (c.restyle) simboli::scrivi(t, f, x, y, s, col, 1);
        else scrivi_contornato(t, f, x, y, s, col, contorno);
    };
    // scie (12EE): un punto per voce, 4 il bersaglio, 8 il proprio battello, 0 gli altri; con TARGET TRACK solo il
    // bersaglio
    if (c.tutte_le_scie || c.scia_bersaglio)
        for (const auto& p : m.scia()) {
            if (c.scia_bersaglio && p.id != bersaglio) continue;
            const std::uint8_t col = p.id == bersaglio ? colori::ROSSO : p.id == g->id ? colori::GRIGIO_SCURO : colori::NERO;
            const Vec2 q = vs.a_tela(p.pos);
            t.rettangolo_pieno(static_cast<int>(q.x), static_cast<int>(q.y), 2, 2, col);
        }
    // punti di rotta: cifre di colore 13
    for (std::size_t i = 0; i < m.punti_di_rotta().size(); ++i)
        testo(v.a_tela(m.punti_di_rotta()[i]), std::to_string(i + 1), MAGENTA_CHIARO);
    // proiezione a N minuti sulla rotta e alla velocita' attuali (il moto dell'originale), e del bersaglio con la traccia
    if (c.proiezione) {
        auto stella = [&](const UnitaClassica& u, std::uint8_t col) {
            const double d = m.metri_al_secondo(u.velocita) * c.minuti() * 60.0, a = radianti(u.rotta);
            testo(vs.a_tela({u.pos.x + d * std::sin(a), u.pos.y + d * std::cos(a)}), "*", col);
        };
        stella(*g, colori::GIALLO);
        for (const auto& s : segni)
            if (s.id == bersaglio && s.traccia())
                if (const auto it = m.unita().find(s.id); it != m.unita().end()) stella(it->second, ROSSO_CHIARO);
    }
    // contatti
    for (const auto& [s, q] : segni_su_carta(vs, m, segni)) {
        const std::string segno = s.traccia() && s.lettera ? std::string(1, s.lettera) : "X";
        if (s.id == bersaglio) t.rettangolo_pieno(static_cast<int>(q.x) - 5, static_cast<int>(q.y) - 5, 11, 11, colori::NERO);
        testo(q, segno, colore_segno(s), segno == "X" ? static_cast<std::uint8_t>(colori::ROSSO) : static_cast<std::uint8_t>(colori::NERO));
    }
    // contatti d'intelligence (Enhanced, sala radio R1): l'ellisse d'incertezza, che cresce con l'eta' del dato, col
    // sensore che l'ha dato e quanti minuti ha
    if (const SalaRadio* r = m.radio(); r && c.livelli.intelligence) {
        const double ora = m.tempo();
        for (const auto& ci : r->intelligence(ora)) {
            const Vec2 c0 = ci.centro(ora);
            const double a = ci.semiasse_rotta(ora), b = ci.semiasse_traverso(ora), ang = radianti(ci.orientamento());
            const Vec2 lungo{std::sin(ang), std::cos(ang)}, largo{std::cos(ang), -std::sin(ang)};
            for (int k = 0; k < 96; ++k) {
                const double th = 2.0 * PI * k / 96.0, ca = a * std::cos(th), sb = b * std::sin(th);
                const Vec2 q = v.a_tela({c0.x + ca * lungo.x + sb * largo.x, c0.y + ca * lungo.y + sb * largo.y});
                t.pixel(static_cast<int>(q.x), static_cast<int>(q.y), ROSSO_CHIARO);
            }
            const Vec2 q = v.a_tela(c0);
            testo(q, "+", ROSSO_CHIARO);
            std::string etichetta = ci.dati.sensore;
            for (auto& ch : etichetta) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            etichetta += " " + std::to_string(static_cast<int>(ci.eta(ora) / 60.0)) + "'";
            simboli::scrivi(t, f, static_cast<int>(q.x) + 6, static_cast<int>(q.y) - 3, etichetta, ROSSO_CHIARO, 1);
        }
    }
    // proprio battello: quadratino di colore 10 bordato di 2
    const Vec2 b = vs.a_tela(g->pos);
    // Fase 3: la mobilia della carta nautica (cornice coi minuti, rosa con la declinazione, scala, cartiglio) sopra il
    // fondale e sotto zone, simboli e contatti; con la cornice il reticolo tiene le linee e lascia a lei i gradi
    const bool mobilia = c.restyle && fondo && c.livelli.mobilia && c.livelli.fondale && c.stile != StileCarta::foglio && fondo->georeferenziato();
    if (c.restyle && fondo) {
        if (fondo->georeferenziato()) disegna_reticolo(t, f, v, *fondo, static_cast<std::uint8_t>(colori_carta::RETICOLO), !mobilia);
        if (mobilia) {
            MobiliaCarta mm;
            mm.cornice = mm.rosa = mm.scala = mm.cartiglio = true;
            // l'intestazione della carta e la scritta della carta d'epoca in alto; in basso il reticolo, o nel briefing
            // (senza intestazione) la sua riga d'aiuto
            mm.riserva_alto = 0;   // la carta grande (scelta dell'utente del 29/10/2026): niente fascia in alto
            mm.zone_non_rilevate = fondo->carta_epoca();
            mm.riserva_basso = c.livelli.intestazione ? 12 : 16;
            mm.declinazione_gradi = declinazione_stima(m.teatro());
            mm.sovietica = c.urss;
            mm.titolo = m.teatro();
            for (auto& ch : mm.titolo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            mm.edizione = fondo->edizione();
            const Font* fm = c.urss ? font_isobate_urss : font_isobate_nato;
            const GeometriaCarta geo{v.x, v.y, v.w, v.h, v.centro, v.metri_per_pixel};
            SpazioMobilia sm = disegna_mobilia(t, geo, *fondo, fm ? *fm : f, mm);
            t.imposta_ritaglio({v.x, v.y, v.w, v.h});
            if (c.livelli.meteo && g && m.meteo()) sm.occupati.push_back(legenda_meteo(t, fm ? *fm : f, sm.interno, *m.meteo(), g->pos));
            // i nomi di mari, stretti, isole, capi, citta' e basi (kit, Natural Earth e aggiunte): sotto zone e simboli
            if (c.livelli.toponimi)
                disegna_toponimi(t, geo, toponimi_del_teatro(m.teatro()), fm ? *fm : f, {sm.interno, sm.occupati});
        }
        if (fondo->carta_epoca()) disegna_note_carta(t, f, v, *fondo, c.urss, !mobilia);
    }
    // senza la mobilia (il foglio di plottaggio) la legenda del meteo sta sotto le due righe in alto
    if (c.restyle && !mobilia && c.livelli.meteo && m.meteo())
        legenda_meteo(t, f, {v.x, v.y + 28, v.w, v.h - 28}, *m.meteo(), g->pos);
    if (c.restyle) {
        for (const auto& z : c.lucido.zone()) disegna_zona(t, f, v, z);
        if (c.livelli.zone)
            for (const auto& z : m.zone_ricevute()) disegna_zona_radio(t, f, v, z);
        for (const auto& sg : c.lucido.segni()) disegna_simbolo(t, f, v.a_tela(sg.pos), sg, c.anni_89, c.stile == StileCarta::vga);
        for (const auto& sg : c.cadute) disegna_simbolo(t, f, vs.a_tela(sg.pos), sg, c.anni_89, c.stile == StileCarta::vga);
    }
    if (c.restyle) disegna_mine(t, f, vs, m, g->id, c.ora_inizio_s);
    if (c.restyle) disegna_strumenti(t, f, vs, m, c, bersaglio);
    if (c.restyle && c.ekelund.aperto) disegna_ekelund(t, f, vs, c, m);
    if (c.restyle && c.tempo_rilevamento) disegna_tempo_rilevamento(t, f, v, c, m.tempo());
    if (c.restyle && c.segni.aperto) {   // modo segno: il cursore, l'anteprima del segno o della zona, l'aiuto
        const Vec2 q = v.a_tela(c.segni.cursore);
        const auto col = static_cast<std::uint8_t>(colori_carta::CURSORE);
        t.linea_orizzontale(static_cast<int>(q.x) - 10, static_cast<int>(q.x) - 4, static_cast<int>(q.y), col);
        t.linea_orizzontale(static_cast<int>(q.x) + 4, static_cast<int>(q.x) + 10, static_cast<int>(q.y), col);
        t.linea_verticale(static_cast<int>(q.x), static_cast<int>(q.y) - 10, static_cast<int>(q.y) - 4, col);
        t.linea_verticale(static_cast<int>(q.x), static_cast<int>(q.y) + 4, static_cast<int>(q.y) + 10, col);
        std::string tipo;
        if (c.segni.tipo == 5) {
            disegna_zona(t, f, v, {c.segni.cursore, c.segni.raggio_zona_m, true});
            char e[32];
            std::snprintf(e, sizeof e, "ZONA %.1f NM", c.segni.raggio_zona_m / MIGLIO);
            tipo = e;
        } else {
            Segno sg;
            sg.affiliazione = c.segni.affiliazione;
            sg.dominio = static_cast<Segno::Dominio>(c.segni.tipo);
            disegna_simbolo(t, f, q, sg, c.anni_89, c.stile == StileCarta::vga);
            tipo = std::string(nome_affiliazione(c.segni.affiliazione)) + " " + nome_dominio(sg.dominio);
        }
        const std::string aiuto = c.segni.scrivendo ? "SCRIVI LA NOTA, INVIO CHIUDE" :
                                  "SEGNO " + tipo + "   TAB AFFILIAZIONE  " + tasti_valore() + " TIPO  INVIO METTE  K TOGLIE" +
                                      (c.segni.tipo == 5 ? "  MAIUSC+SU/GIU RAGGIO" : "");
        f.scrivi(t, v.x + 8, v.y + v.h - 36, aiuto, static_cast<std::uint8_t>(colori_carta::EVIDENZA), 1);
    }
    if (!c.livelli.battello) {
        t.togli_ritaglio();
        return;
    }
    if (c.restyle)   // ciano con l'alone sul blu della VGA, per non confondersi con gli scandagli; a matita nera sulla carta chiara
        disegna_rosa(t, f, b, 70, c.stile == StileCarta::vga ? colori::CIANO_CHIARO : static_cast<std::uint8_t>(colori_carta::MATITA), c.stile == StileCarta::vga);
    if (c.restyle) {
        simboli::disegna_proprio(t, b);   // gli stessi quadratini, con l'alone
    } else {   // il classico: senza contorno, come l'originale (A2C6, E838)
        t.rettangolo_pieno(static_cast<int>(b.x) - 3, static_cast<int>(b.y) - 3, 7, 7, colori::VERDE);
        t.rettangolo_pieno(static_cast<int>(b.x) - 2, static_cast<int>(b.y) - 2, 5, 5, colori::VERDE_CHIARO);
    }
    // l'ellisse d'incertezza della posizione stimata, al 95%, tratteggiata
    std::optional<EllisseErrore> ellisse;
    if (!area && m.con_navigazione()) ellisse = m.ellisse_navigazione(g->id, ELLISSE_95);
    if (ellisse) {   // il modulo comune: tratteggiata, col verde del battello e l'alone
        const Vec2 c0{g->pos.x + errore.x, g->pos.y + errore.y};
        simboli::disegna_ellisse(t, v.a_tela(c0), ellisse->maggiore_m / v.metri_per_pixel, ellisse->minore_m / v.metri_per_pixel,
                                 ellisse->orientamento_deg, simboli::VERDE_PROPRIO);
    }
    if (cursore) {   // SET: croce e coordinate in nm dal battello
        const Vec2 q = v.a_tela(*cursore);
        const int cx = static_cast<int>(q.x), cy = static_cast<int>(q.y);
        t.linea_orizzontale(cx - 8, cx + 8, cy, colori::BIANCO);
        t.linea_verticale(cx, cy - 8, cy + 8, colori::BIANCO);
        char riga[64];
        const Vec2 da{g->pos.x + errore.x, g->pos.y + errore.y};   // dalla posizione che il giocatore crede sua
        std::snprintf(riga, sizeof riga, "%03.0f°  %.1f NM", normalizza_360(rilevamento(da, *cursore)), distanza(da, *cursore) / MIGLIO);
        f.scrivi(t, v.x + 8, v.y + v.h - 12, riga, colori::BIANCO, 1);
    }
    char riga[128];
    std::snprintf(riga, sizeof riga, "INGRANDIMENTO: %d   PROIEZIONE %02d MIN%s", c.fattore(), c.minuti(), c.proiezione ? "" : " (SPENTA)");
    // la carta grande (con la mobilia): ingrandimento e proiezione stanno nel pannello; resta solo l'errore di posizione
    if (mobilia) riga[0] = '\0';
    if (ellisse) {   // il raggio del 95% e il punto nave in corso
        std::string r = riga;
        char x[64];
        std::snprintf(x, sizeof x, "   POSIZIONE +-%.1f NM", ellisse->maggiore_m / MIGLIO);
        r += x;
        if (const auto pn = m.punto_nave_in_corso(); pn && pn->in_corso) {
            std::string tipo = pn->tipo;
            for (auto& ch : tipo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch == '_' ? ' ' : ch)));
            if (pn->attesa_s > 0.0) std::snprintf(x, sizeof x, "   %s FRA %.0f MIN", tipo.c_str(), std::ceil(pn->attesa_s / 60.0));
            else std::snprintf(x, sizeof x, "   %s %.0f S", tipo.c_str(), std::ceil(pn->rimasto_s));
            r += x;
        }
        std::snprintf(riga, sizeof riga, "%s", r.c_str());
    }
    // con la cornice (la carta grande) l'errore di posizione sta sul margine bianco in basso, a destra, a matita nera;
    // senza, la riga resta in alto come prima
    if (c.livelli.intestazione) {
        std::string r = riga;
        while (!r.empty() && r.front() == ' ') r.erase(r.begin());
        if (!mobilia) f.scrivi(t, v.x + 8, v.y + 6, r, colori::BIANCO, 1);
        else if (!r.empty())
            f.scrivi(t, v.x + v.w - 8 - f.larghezza(r, 1), v.y + v.h - 11, r, static_cast<std::uint8_t>(colori_carta::MATITA), 1);
    }
    t.togli_ritaglio();
}

void disegna_carta_museo(Tela& t, const Font& f, Rettangolo r, const Fondale& carta, const Geografiche* punto, double larghezza_km) {
    using namespace colori_carta;
    if (r.w < 8 || r.h < 8 || carta.larghezza() <= 0 || carta.altezza() <= 0) return;
    VistaCarta v;
    v.x = r.x;
    v.y = r.y;
    v.w = r.w;
    v.h = r.h;
    const bool base = punto && carta.georeferenziato();
    if (base) {
        v.centro = carta.proiettata(*punto);
        v.metri_per_pixel = larghezza_km * 1000.0 / r.w;
    } else {   // il teatro intero, nel riquadro
        const Vec2 a = carta.centro_cella(0, 0), b = carta.centro_cella(carta.larghezza() - 1, carta.altezza() - 1);
        v.centro = {(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
        v.metri_per_pixel = std::max(carta.larghezza() * carta.cella_m() / r.w, carta.altezza() * carta.cella_m() / r.h);
    }
    t.imposta_ritaglio(r);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, static_cast<std::uint8_t>(ACQUA + 15 * 3 + 1));
    disegna_fondale_vga(t, v, carta, 0.0);
    // la base: cerchio rosso con la croce, bordato di bianco perche' si legga sulla terra e sul mare
    if (base) {
        const Vec2 q = v.a_tela(v.centro);
        const int cx = static_cast<int>(std::lround(q.x)), cy = static_cast<int>(std::lround(q.y));
        for (const auto& [rr, col] : {std::pair{6.5, MATITA + 5}, std::pair{5.5, MATITA + 1}, std::pair{4.5, MATITA + 5}})
            for (int k = 0; k < 48; ++k) {
                const double a = 2.0 * PI * k / 48;
                t.pixel(static_cast<int>(std::lround(cx + rr * std::cos(a))), static_cast<int>(std::lround(cy + rr * std::sin(a))),
                        static_cast<std::uint8_t>(col));
            }
        for (int d = -3; d <= 3; ++d) {
            t.pixel(cx + d, cy, static_cast<std::uint8_t>(MATITA + 1));
            t.pixel(cx, cy + d, static_cast<std::uint8_t>(MATITA + 1));
        }
    }
    // scala in miglia: il passo tondo piu' lungo che sta in un quarto del riquadro, in basso a sinistra
    double nm = 1.0;
    for (const double n : {1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0})
        if (n * 1852.0 / v.metri_per_pixel <= r.w / 4.0) nm = n;
    const int lp = static_cast<int>(std::lround(nm * 1852.0 / v.metri_per_pixel)), sx = r.x + 4, sy = r.y + r.h - 5;
    const auto nero = static_cast<std::uint8_t>(MATITA), bianco = static_cast<std::uint8_t>(MATITA + 5);
    t.rettangolo_pieno(sx - 1, sy - 2, lp + 3, 5, bianco);
    t.rettangolo_pieno(sx, sy - 1, lp + 1, 3, nero);
    t.rettangolo_pieno(sx + lp / 2, sy, (lp + 1) / 2, 1, bianco);   // la meta' di destra a strisce, come sulle carte
    char txt[16];
    std::snprintf(txt, sizeof txt, "%.0f NM", nm);
    f.scrivi(t, sx + lp + 5, sy - 2, txt, nero, 1);   // in bianco con l'ombra: si legge sul mare e sulla terra
    f.scrivi(t, sx + lp + 4, sy - 3, txt, bianco, 1);
    t.togli_ritaglio();
}

}  // namespace m688
