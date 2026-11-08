#include "grafica/cartolina.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

#include "grafica/cerimonia.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

double caso(int a, int b, int c) {
    std::uint32_t h = static_cast<std::uint32_t>(a) * 73856093U ^ static_cast<std::uint32_t>(b) * 19349663U ^
                      static_cast<std::uint32_t>(c) * 83492791U;
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return h / 4294967296.0;
}

// colori dell'interfaccia (palette del giorno): EGA 0-15, grigi 144-158
constexpr int NERO = 0, VERDE = 2, CIANO = 3, ROSSO = 4, MAGENTA = 5, MARRONE = 6, GRIGIO = 7, GRIGIO_SCURO = 8, VERDE_CHIARO = 10,
              CIANO_CHIARO = 11, ROSSO_CHIARO = 12, GIALLO = 14, BIANCO = 15, GRIGI = 144;

}  // namespace

PuntoDiVista inquadra(Vec2 punto, double distanza_m, double da_gradi, double tempo_s, double elevazione_gradi) {
    PuntoDiVista q;
    const double a = radianti(da_gradi), el = radianti(std::clamp(elevazione_gradi, 0.0, 30.0));
    const double orizz = distanza_m * std::cos(el);
    q.occhio = {punto.x + orizz * std::sin(a), punto.y + orizz * std::cos(a)};
    q.quota_occhio_m = std::max(2.0, distanza_m * std::sin(el));
    q.rilevamento = normalizza_360(da_gradi + 180.0);
    q.bersaglio = punto;
    q.alto_ingrandimento = elevazione_gradi < 1.2;   // il campo stretto copre solo 4,5 gradi in altezza
    q.tempo_s = tempo_s;
    return q;
}

PuntoDiVista inquadra_nave(const Nave& n, double lunghezza_m, double tempo_s, double lato_gradi) {
    // il riquadro prende 3,2 gradi del campo ad alto ingrandimento: la nave ne occupa circa i quattro quinti
    const double campo = radianti(CAMPO_ALTO_GRADI) * (256.0 / LARGHEZZA_TELA);
    const double d = std::max(500.0, std::max(lunghezza_m, 40.0) / (0.78 * std::tan(campo)));
    return inquadra(n.pos, d, n.rotta + lato_gradi, tempo_s);
}

PuntoDiVista dal_registro(const Nave& soggetto, const Nave* secondo, double distanza_m, double elevazione_gradi,
                          double azimut_gradi, double tempo_s) {
    Vec2 centro = soggetto.pos;
    double d = std::max(distanza_m, 300.0);
    if (secondo) {   // tutti e due in campo: il centro fra i due, abbastanza lontano da contenerli
        centro = {(soggetto.pos.x + secondo->pos.x) / 2, (soggetto.pos.y + secondo->pos.y) / 2};
        const double campo = radianti(elevazione_gradi < 1.2 ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI) * (256.0 / LARGHEZZA_TELA);
        d = std::max(d, distanza(soggetto.pos, secondo->pos) / (0.8 * std::tan(campo)));
    }
    return inquadra(centro, d, soggetto.rotta + azimut_gradi, tempo_s, std::max(0.0, elevazione_gradi));
}

Icona icona_da_nome(std::string_view n) {
    if (n == "radio") return Icona::telescrivente;
    if (n == "esm") return Icona::esm;
    if (n == "radiogoniometro") return Icona::radiogoniometro;
    if (n == "danni") return Icona::danni;
    if (n == "zona") return Icona::zona;
    if (n == "cascata") return Icona::cascata;
    if (n == "mina") return Icona::mina;
    if (n == "aereo" || n == "aereo_in_sorvolo") return Icona::aereo;
    if (n == "boa" || n == "boa_vicina") return Icona::boa;
    if (n == "elicottero" || n == "ping_sonar_immersione") return Icona::elicottero;
    if (n == "marcatore" || n == "marcatore_avvistato" || n == "attacco_simulato" || n == "sus") return Icona::marcatore;
    if (n == "periscopio") return Icona::periscopio;
    return Icona::sonar;
}

void disegna_cartolina(Tela& t, Rettangolo r, const Periscopio& p, const Mondo& mondo, const PuntoDiVista& q,
                       const Effetti* effetti) {
    static Tela appoggio;   // la vista intera, poi se ne prende il centro
    Nave occhio;
    occhio.id = -1000000;   // non e' nel mondo
    occhio.pos = q.occhio;
    occhio.rotta = q.rilevamento;
    VistaPeriscopio v;
    v.rilevamento = q.rilevamento;
    v.alto_ingrandimento = q.alto_ingrandimento;
    v.quota_occhio_m = q.quota_occhio_m;
    v.luce = q.luce;
    v.visibilita_m = q.visibilita_m;
    v.altezza_onde_m = q.altezza_onde_m;
    v.tempo_s = q.tempo_s;
    v.reticolo = false;   // una scena vista da fuori, non dall'occhio del periscopio (l'utente, 07/11/2026)
    appoggio.togli_ritaglio();
    appoggio.riempi(0);
    p.disegna(appoggio, mondo, occhio, v, effetti);
    appoggio.togli_ritaglio();
    // ritaglio 2x attorno all'orizzonte, un po' sopra: le navi e le colonne stanno sopra la linea
    const int cw = 2 * r.w, ch = 2 * r.h;
    const int x0 = (LARGHEZZA_TELA - cw) / 2;
    // riga del bersaglio: sotto l'occhio di (quota / distanza) radianti; a pelo d'acqua e' l'orizzonte
    const double dist = std::max(1.0, distanza(q.occhio, q.bersaglio));
    const double riga = ALTEZZA_VISTA / 2.0 + (q.quota_occhio_m / dist) * Periscopio::pixel_per_radiante(q.alto_ingrandimento);
    const double oriz = std::max(Periscopio::riga_orizzonte(v), riga);
    const int y0 = std::clamp(static_cast<int>(oriz - ch * (0.62 + q.alzo)), 0, ALTEZZA_VISTA - ch);
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) {
            // il colore piu' frequente nel blocco 2x2 (a parita' quello in alto a sinistra): i contorni restano netti
            std::array<std::uint8_t, 4> c{appoggio.at(x0 + 2 * x, y0 + 2 * y), appoggio.at(x0 + 2 * x + 1, y0 + 2 * y),
                                          appoggio.at(x0 + 2 * x, y0 + 2 * y + 1), appoggio.at(x0 + 2 * x + 1, y0 + 2 * y + 1)};
            std::uint8_t migliore = c[0];
            int conta = 0;
            for (int i = 0; i < 4; ++i) {
                int n = 0;
                for (int j = 0; j < 4; ++j) n += c[static_cast<std::size_t>(j)] == c[static_cast<std::size_t>(i)];
                if (n > conta) { conta = n; migliore = c[static_cast<std::size_t>(i)]; }
            }
            t.pixel(r.x + x, r.y + y, migliore);
        }
}

void disegna_icona(Tela& t, const Font& f, Rettangolo r, Icona i, double tm, std::uint32_t seme, std::string_view testo) {
    const int s = static_cast<int>(seme);
    auto metti = [&](int x, int y, int c) {
        if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) t.pixel(x, y, static_cast<std::uint8_t>(c));
    };
    switch (i) {
        case Icona::cascata: {
            // righe di rumore che scendono, la traccia del contatto che deriva, la scala dei rilevamenti in basso
            const int scorre = static_cast<int>(tm * 6.0);
            const double x_traccia = 0.62 + 0.08 * std::sin(seme * 1.3);
            for (int y = r.y; y < r.y + r.h - 10; ++y) {
                const int riga = (y - r.y) - scorre;
                const double xt = r.x + r.w * (x_traccia - 0.0012 * riga);
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double n = caso(x, riga, s);
                    const double d = std::fabs(x - xt);
                    int c = n > 0.93 ? VERDE_CHIARO : n > 0.6 ? VERDE : NERO;
                    if (d < 1.5 && caso(x, riga, s + 1) < 0.85) c = d < 0.7 ? BIANCO : VERDE_CHIARO;
                    else if (d < 3 && n > 0.4) c = VERDE_CHIARO;
                    metti(x, y, c);
                }
            }
            for (int y = r.y + r.h - 10; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) metti(x, y, (x - r.x) % 16 == 0 && y < r.y + r.h - 6 ? VERDE_CHIARO : GRIGI + 2);
            break;
        }
        case Icona::sonar: {
            // schermo circolare: anelli, il fascio che ruota, il lampo del contatto
            const int cx = r.x + r.w / 2, cy = r.y + r.h / 2, R = std::min(r.w, r.h) / 2 - 3;
            const double fascio = std::fmod(tm * 1.4, 2 * PI);
            const double ab = 0.8 + seme % 5, db = 0.6 * R;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double dx = x - cx, dy = y - cy, d = std::hypot(dx, dy);
                    if (d > R + 2) { metti(x, y, GRIGI + 3); continue; }
                    if (d > R) { metti(x, y, GRIGI + 9); continue; }
                    const double a = std::atan2(dx, -dy);
                    const double dietro = std::fmod(fascio - a + 4 * PI, 2 * PI);   // quanto e' passato il fascio
                    int c = NERO;
                    if (dietro < 0.9) c = dietro < 0.08 ? VERDE_CHIARO : (caso(x, y, s) < 0.6 * (1 - dietro / 0.9) ? VERDE : NERO);
                    if (std::fabs(std::fmod(d, R / 3.0)) < 0.6 || std::fabs(dx) < 0.5 || std::fabs(dy) < 0.5) c = c == NERO ? GRIGI + 3 : c;
                    const double bx = cx + db * std::sin(ab), by = cy - db * std::cos(ab);
                    const double lampo = std::fmod(fascio - ab + 4 * PI, 2 * PI);
                    if (std::hypot(x - bx, y - by) < 2.6 && lampo < 2.5) c = lampo < 0.6 ? BIANCO : VERDE_CHIARO;
                    metti(x, y, c);
                }
            break;
        }
        case Icona::telescrivente: {
            // foglio che esce dal rullo: righe battute, l'ultima a meta'
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    int c = (y - r.y) < 12 ? GRIGI + 4 + ((y - r.y) % 4 == 0 ? 3 : 0) : (caso(x, y, s) < 0.06 ? GRIGI + 13 : BIANCO);
                    if ((y - r.y) >= 12 && (x - r.x) < 6) c = (y % 6 == 0) ? GRIGI + 9 : GRIGI + 12;   // fori di trascinamento
                    metti(x, y, c);
                }
            if (!testo.empty()) {
                std::string riga;
                int y = r.y + 16;
                std::string_view resto = testo;
                const std::size_t mostrati = static_cast<std::size_t>(tm * 18.0);
                std::size_t scritti = 0;
                while (!resto.empty() && y < r.y + r.h - 10) {
                    std::size_t n = resto.size();
                    while (n > 0 && f.larghezza(resto.substr(0, n), 1) > r.w - 12) n = resto.substr(0, n - 1).find_last_of(' ') + 1;
                    if (n == 0) n = 1;
                    const auto parte = resto.substr(0, std::min(n, mostrati > scritti ? mostrati - scritti : 0));
                    f.scrivi(t, r.x + 9, y, parte, NERO, 1);
                    scritti += n;
                    resto = resto.substr(n);
                    y += 11;
                }
            } else {
                for (int k = 0; k < 6; ++k) {
                    const int y = r.y + 18 + k * 11;
                    const int lung = k == 5 ? 30 : 70 + static_cast<int>(caso(k, 0, s) * (r.w - 90));
                    for (int x = r.x + 9; x < r.x + 9 + lung; ++x)
                        if (caso(x / 4, k, s) < 0.8 && (x / 4) % 2 == 0) {
                            metti(x, y, NERO);
                            metti(x, y + 1, NERO);
                        }
                }
            }
            break;
        }
        case Icona::esm: {
            // oscilloscopio polare: reticolo, la linea di rilevamento che pulsa, le sigle della minaccia
            const int cx = r.x + r.w / 2, cy = r.y + r.h / 2, R = std::min(r.w, r.h) / 2 - 3;
            const double rl = 0.9 + (seme % 7) * 0.7;
            const bool acceso = std::fmod(tm, 0.8) < 0.5;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double dx = x - cx, dy = y - cy, d = std::hypot(dx, dy);
                    int c = d > R ? GRIGI + 3 : NERO;
                    if (d <= R && (std::fabs(d - R) < 0.8 || std::fabs(d - R / 2.0) < 0.5)) c = CIANO;
                    const double a = std::atan2(dx, -dy);
                    const double scarto = std::fabs(normalizza_180(gradi(a - rl)));
                    if (d <= R && d > 4 && scarto < 1.5 && acceso) c = d > R * 0.7 ? GIALLO : ROSSO_CHIARO;
                    else if (d <= R && d > 4 && scarto < 4 && acceso && caso(x, y, s) < 0.4) c = ROSSO;
                    metti(x, y, c);
                }
            f.scrivi(t, r.x + 3, r.y + 2, "ESM", CIANO_CHIARO, 1);
            break;
        }
        case Icona::radiogoniometro: {
            // rosa dei venti su fondo scuro, due rilevamenti dalle stazioni costiere che si incrociano sul battello
            const int cx = r.x + r.w / 2, cy = r.y + r.h / 2, R = std::min(r.w, r.h) / 2 - 4;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double d = std::hypot(x - cx, y - cy);
                    int c = d > R ? GRIGI + 3 : NERO;
                    if (std::fabs(d - R) < 0.7 || (d < R && (std::abs(x - cx) == 0 || std::abs(y - cy) == 0) && static_cast<int>(d) % 3 == 0)) c = VERDE;
                    metti(x, y, c);
                }
            const double bx = cx + R * 0.35 * std::cos(seme * 0.7), by = cy - R * 0.25;
            for (const double a0 : {2.4, 4.0}) {
                const double sx = cx + R * std::cos(a0), sy = cy + R * std::sin(a0);
                for (int k = 0; k <= 60; ++k) {
                    const double u = k / 60.0 * 1.3;
                    if (std::fmod(u * 20 - tm * 3, 2.0) < 1.2) metti(static_cast<int>(sx + (bx - sx) * u), static_cast<int>(sy + (by - sy) * u), GIALLO);
                }
                for (int yy = -1; yy <= 1; ++yy)
                    for (int xx = -1; xx <= 1; ++xx) metti(static_cast<int>(sx) + xx, static_cast<int>(sy) + yy, CIANO_CHIARO);
            }
            const bool acceso = std::fmod(tm, 0.6) < 0.35;
            for (int yy = -2; yy <= 2; ++yy)
                for (int xx = -2; xx <= 2; ++xx)
                    if (std::abs(xx) + std::abs(yy) <= 2) metti(static_cast<int>(bx) + xx, static_cast<int>(by) + yy, acceso ? ROSSO_CHIARO : ROSSO);
            f.scrivi(t, r.x + 3, r.y + 2, "DF", CIANO_CHIARO, 1);
            break;
        }
        case Icona::danni: {
            // sagoma del battello di profilo, compartimenti, quello colpito che lampeggia, acqua che sale
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) metti(x, y, GRIGI + 2);
            const int cy = r.y + r.h / 2 + 4, L = r.w - 16, x0 = r.x + 8, H = r.h / 4;
            const int colpito = static_cast<int>(seme % 4);
            for (int x = 0; x < L; ++x) {
                const double u = static_cast<double>(x) / L;
                const double h = H * std::sqrt(std::max(0.0, 1 - std::pow(2 * u - 1, 8.0)));
                const int comp = std::min(3, static_cast<int>(u * 4));
                for (int y = static_cast<int>(-h); y <= h; ++y) {
                    int c = GRIGI + 6;
                    if (std::abs(y) >= static_cast<int>(h) - 1) c = GRIGI + 12;
                    else if (x % (L / 4) == 0) c = GRIGI + 9;
                    else if (comp == colpito) {
                        const bool lampo = std::fmod(tm * 2.0, 1.0) < 0.5;
                        c = y > h - 2 * h * (0.4 + 0.1 * std::sin(tm * 2)) ? CIANO : (lampo ? ROSSO_CHIARO : ROSSO);
                    }
                    metti(x0 + x, cy + y, c);
                }
                if (u > 0.52 && u < 0.64)   // vela
                    for (int y = static_cast<int>(-h) - 8; y < -h; ++y) metti(x0 + x, cy + y, GRIGI + 10);
            }
            f.scrivi(t, r.x + 3, r.y + 2, "DANNI", GIALLO, 1);
            break;
        }
        case Icona::periscopio: {
            // cielo chiaro, mare che scurisce verso il basso, la testa del periscopio col suo baffo che avanza a sinistra
            const int mare = r.y + r.h / 2;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const int onda = static_cast<int>(1.5 * std::sin(x * 0.25 + tm * 2.0));
                    metti(x, y, y < mare + onda ? (caso(x / 3, y / 2, s) < 0.15 ? CIANO : 1) : (caso(x, y, s) < 0.75 - 0.5 * (y - mare) / r.h ? CIANO : 1));
                }
            const int px = r.x + r.w / 2, alto = mare - std::max(10, r.h / 3);
            for (int y = alto; y <= mare; ++y)
                for (int x = px - 1; x <= px + 1; ++x) metti(x, y, GRIGI + 5);
            for (int x = px - 3; x <= px + 2; ++x)
                for (int y = alto - 2; y < alto + 2; ++y) metti(x, y, GRIGI + 9);
            for (int k = 1; k < r.w / 3; ++k) {   // il baffo
                const int largo = 1 + k / 12;
                for (int y = -largo; y <= largo; ++y)
                    if (caso(k, y, s + static_cast<int>(tm * 4.0)) < 0.7) metti(px + 1 + k, mare + y + 1, BIANCO);
            }
            break;
        }
        case Icona::zona: {
            // carta quadrettata, zona protetta tratteggiata in magenta come sulla carta (ZONA), il battello che ci entra
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    int c = (x - r.x) % 16 == 0 || (y - r.y) % 16 == 0 ? CIANO : 1;   // blu della carta
                    const double dx = x - (r.x + r.w * 0.6), dy = y - (r.y + r.h * 0.45);
                    const bool dentro = dx * dx / 900.0 + dy * dy / 500.0 < 1.0;
                    if (dentro && (x + y) % 6 < 2) c = MAGENTA;
                    if (std::fabs(dx * dx / 900.0 + dy * dy / 500.0 - 1.0) < 0.06) c = MAGENTA;
                    metti(x, y, c);
                }
            const double avanza = std::fmod(tm * 0.15, 1.0);
            const int bx = r.x + static_cast<int>(r.w * (0.15 + 0.35 * avanza)), by = r.y + static_cast<int>(r.h * (0.75 - 0.25 * avanza));
            for (int k = 0; k < 30; ++k) metti(bx - k, by + k / 2, BIANCO);   // scia
            for (int yy = -2; yy <= 2; ++yy)
                for (int xx = -3; xx <= 3; ++xx) metti(bx + xx, by + yy, GIALLO);
            break;
        }
        case Icona::mina: {
            // sezione dell'acqua: blu che scurisce col fondo, fondale bruno, la mina sferica con gli urtanti sul cavo
            const int fondo = r.y + r.h - 14;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double v = static_cast<double>(y - r.y) / r.h;
                    const int sabbia = fondo + static_cast<int>(3 * std::sin(x * 0.09 + seme));
                    int c = y >= sabbia ? (caso(x, y, s) < 0.3 ? GRIGI + 4 : MARRONE) : (v < 0.08 ? CIANO : (caso(x, y, s) < 0.5 - 0.4 * v ? 1 : NERO));
                    if (y < sabbia && caso(x / 2, (y + static_cast<int>(tm * 8)) / 2, s + 2) > 0.985) c = CIANO;   // particelle
                    metti(x, y, c);
                }
            const double ondeggia = 2.0 * std::sin(tm * 1.2);
            const int mx = r.x + r.w / 2 + static_cast<int>(ondeggia), my = r.y + r.h / 2 - 6, R = std::max(5, r.h / 9);
            for (int y = my + R; y < fondo - 2; ++y)   // cavo d'ormeggio
                metti(r.x + r.w / 2 + static_cast<int>(ondeggia * (fondo - y) / (fondo - my)), y, GRIGI + 9);
            for (int y = fondo - 4; y < fondo + 1; ++y)   // ancora (il carrello)
                for (int x = r.x + r.w / 2 - 5; x <= r.x + r.w / 2 + 5; ++x) metti(x, y, GRIGI + 6);
            for (int y = -R; y <= R; ++y)
                for (int x = -R; x <= R; ++x) {
                    const double d = std::hypot(x, y);
                    if (d > R) continue;
                    metti(mx + x, my + y, GRIGI + std::clamp(static_cast<int>(10 - 5 * (x + y + R) / (2.0 * R)), 2, 12));
                }
            for (int k = 0; k < 6; ++k) {   // urtanti
                const double a = k * PI / 3 + 0.3;
                for (int q = R; q <= R + 3; ++q) metti(mx + static_cast<int>(q * std::cos(a)), my + static_cast<int>(q * std::sin(a)), GRIGI + 12);
            }
            f.scrivi(t, r.x + 3, r.y + 2, "MINA", GIALLO, 1);
            break;
        }
        case Icona::aereo: {
            // cielo retinato, il quadrimotore visto da sotto che passa (ali, gondole, coda, pungiglione del MAD)
            const int mare = r.y + r.h - std::max(8, r.h / 6);
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const double v = static_cast<double>(y - r.y) / (mare - r.y);
                    int c = y >= mare ? (caso(x, y, s) < 0.25 ? CIANO : 1) : (cerimonia::bayer(x, y) < 0.75 - 0.6 * v ? 9 : CIANO_CHIARO);
                    metti(x, y, c);
                }
            const double S = std::min(r.w, r.h) * 0.30;   // semiapertura in pixel
            const double cx = r.x + r.w * (0.25 + 0.5 * std::fmod(0.08 * tm + 0.37 * (seme % 3), 1.0)), cy = r.y + (mare - r.y) * 0.55;
            auto pieno = [&](double x0, double y0, double x1, double y1, int c) {
                for (int y = static_cast<int>(std::floor(std::min(y0, y1))); y <= static_cast<int>(std::ceil(std::max(y0, y1))); ++y)
                    for (int x = static_cast<int>(std::floor(std::min(x0, x1))); x <= static_cast<int>(std::ceil(std::max(x0, x1))); ++x) metti(x, y, c);
            };
            // la prua verso destra: fusoliera orizzontale, ali verticali (pianta vista da sotto)
            pieno(cx - 0.95 * S, cy - 0.07 * S, cx + 0.85 * S, cy + 0.07 * S, GRIGI + 4);   // fusoliera
            for (int x = static_cast<int>(cx + 0.85 * S); x <= cx + S; ++x) metti(x, static_cast<int>(cy), GRIGI + 3);   // muso
            pieno(cx - 1.25 * S, cy - 0.02 * S, cx - 0.95 * S, cy + 0.02 * S, GRIGI + 2);   // pungiglione del MAD
            for (int y = static_cast<int>(cy - S); y <= cy + S; ++y) {   // ala dritta, rastremata
                const double k = std::fabs(y - cy) / S;
                pieno(cx + 0.05 * S - 0.12 * S * k, y, cx + 0.22 * S - 0.06 * S * k, y, GRIGI + (y < cy ? 5 : 3));
            }
            for (int y = static_cast<int>(cy - 0.38 * S); y <= cy + 0.38 * S; ++y) {   // piani di coda
                const double k = std::fabs(y - cy) / (0.38 * S);
                pieno(cx - 0.9 * S + 0.05 * S * k, y, cx - 0.78 * S - 0.03 * S * k, y, GRIGI + 4);
            }
            for (const double g : {-0.62, -0.3, 0.3, 0.62})   // gondole dei motori
                pieno(cx + 0.12 * S, cy + g * S - 0.035 * S, cx + 0.38 * S, cy + g * S + 0.035 * S, GRIGI + 2);
            f.scrivi(t, r.x + 3, r.y + 2, "SORVOLO", GIALLO, 1);
            break;
        }
        case Icona::boa:
        case Icona::elicottero: {
            // sezione del mare: cielo, onde, acqua che scurisce in profondita'
            const int pelo = r.y + r.h * 2 / 5;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) {
                    const int onda = pelo + static_cast<int>(1.5 * std::sin(0.35 * x + 2.0 * tm));
                    const double v = static_cast<double>(y - pelo) / (r.y + r.h - pelo);
                    int c = y < onda ? (cerimonia::bayer(x, y) < 0.5 ? CIANO_CHIARO : 9) : (y == onda ? BIANCO : (caso(x, y, s) < 0.55 - 0.45 * v ? 1 : NERO));
                    metti(x, y, c);
                }
            const int cx = r.x + r.w / 2;
            const int fondo_cavo = std::min(r.y + r.h - 8, pelo + r.h / 3);
            if (i == Icona::boa) {
                const int dx = static_cast<int>(1.5 * std::sin(0.8 * tm));
                for (int y = pelo + 2; y < fondo_cavo; ++y) metti(cx + dx * (y - pelo) / (fondo_cavo - pelo), y, GRIGI + 8);   // cavo
                for (int y = fondo_cavo; y < fondo_cavo + 4; ++y)   // idrofono
                    for (int x = -1; x <= 1; ++x) metti(cx + dx + x, y, GRIGI + 11);
                for (int y = pelo - 3; y <= pelo + 1; ++y)   // galleggiante
                    for (int x = -3; x <= 3; ++x) metti(cx + x, y, y < pelo ? GRIGI + 12 : GRIGI + 9);
                for (int y = pelo - 3 - r.h / 6; y < pelo - 3; ++y) metti(cx, y, GRIGI + 4);   // antenna
                // onde radio dall'antenna, che si allargano
                const int ya = pelo - 3 - r.h / 6;
                for (int k = 0; k < 3; ++k) {
                    const double raggio = std::fmod(6.0 * tm + 5.0 * k, 15.0) + 3.0;
                    for (int a = -40; a <= 40; a += 4) {
                        const double ang = a * PI / 180.0;
                        for (const int lato : {-1, 1})
                            metti(cx + lato * static_cast<int>(raggio * std::cos(ang)), ya + static_cast<int>(raggio * std::sin(ang)), raggio < 10 ? GIALLO : GRIGI + 10);
                    }
                }
                f.scrivi(t, r.x + 3, r.y + 2, "BOA", GIALLO, 1);
            } else {
                // elicottero in hover sopra il pelo, il disco del rotore, l'anello di spruzzi, il cavo e i ping
                const int hy = r.y + (pelo - r.y) / 3 + 3, L = std::max(18, r.w / 3);
                for (int y = hy - 4; y <= hy + 4; ++y)   // fusoliera, col muso arrotondato
                    for (int x = cx - L / 2; x <= cx + L / 4; ++x)
                        if (std::abs(y - hy) <= 4 - (x > cx + L / 6 ? 2 : 0)) metti(x, y, GRIGI + (y < hy - 1 ? 6 : y > hy + 2 ? 2 : 4));
                for (int x = cx - L; x <= cx - L / 2; ++x) metti(x, hy - 1 + (cx - L / 2 - x) / 8, GRIGI + 4);   // trave di coda
                for (int y = hy - 6; y <= hy - 1; ++y) metti(cx - L, y, GRIGI + 4);
                const bool lampeggia = std::fmod(tm * 6.0, 1.0) < 0.5;
                for (int x = cx - L; x <= cx + L / 2 + 6; ++x)   // disco del rotore puntinato
                    if (cerimonia::bayer(x, hy - 6) < 0.5 || lampeggia) metti(x, hy - 6, GRIGI + 9);
                for (int y = hy + 5; y < fondo_cavo; ++y) metti(cx, y, y < pelo ? GRIGI + 3 : GRIGI + 8);   // cavo
                for (int y = fondo_cavo; y < fondo_cavo + 5; ++y)
                    for (int x = -2; x <= 2; ++x) metti(cx + x, y, GRIGI + 11);   // trasduttore
                for (int x = r.x; x < r.x + r.w; ++x) {   // spruzzi del flusso del rotore
                    const int d = std::abs(x - cx);
                    if (d > L / 2 && d < L + 6 && caso(x, static_cast<int>(tm * 10), s) < 0.6)
                        for (int y = pelo - 1 - static_cast<int>(3 * caso(x, 1, s + 4)); y < pelo; ++y) metti(x, y, BIANCO);
                }
                const double fase = std::fmod(tm, 3.0);   // un ping ogni 3 s: l'anello che si allarga dal trasduttore
                if (fase < 1.8) {
                    const double raggio = 4.0 + 16.0 * fase;
                    for (int a = 0; a < 360; a += 3) {
                        const double ang = a * PI / 180.0;
                        const int y = fondo_cavo + 2 + static_cast<int>(raggio * 0.7 * std::sin(ang));
                        if (y > pelo + 1) metti(cx + static_cast<int>(raggio * std::cos(ang)), y, fase < 0.9 ? CIANO_CHIARO : CIANO);
                    }
                }
                f.scrivi(t, r.x + 3, r.y + 2, "DIP", GIALLO, 1);
            }
            break;
        }
        case Icona::marcatore: {
            // di notte: mare nero, le due candele del Mk 58 che tremolano, il riflesso e il fumo piegato dal vento
            const int pelo = r.y + r.h * 3 / 5;
            for (int y = r.y; y < r.y + r.h; ++y)
                for (int x = r.x; x < r.x + r.w; ++x) metti(x, y, y < pelo ? (caso(x, y, s) < 0.004 ? GRIGI + 10 : NERO) : (caso(x, y, s) < 0.12 ? 1 : NERO));
            const int cx = r.x + r.w / 2;
            for (int k = 0; k < 40; ++k) {   // fumo: sale e piega sottovento
                const double u = std::fmod(k / 40.0 + 0.05 * tm, 1.0);
                const int fx = cx + static_cast<int>(u * u * r.w * 0.45 + 2 * std::sin(k + tm));
                const int fy = pelo - 4 - static_cast<int>(u * (pelo - r.y) * 0.8);
                const int R = 1 + static_cast<int>(4 * u);
                for (int y = -R; y <= R; ++y)
                    for (int x = -R; x <= R; ++x)
                        if (x * x + y * y <= R * R && caso(fx + x, fy + y, s + k) < 0.45) metti(fx + x, fy + y, u < 0.25 ? 6 : GRIGI + 4);
            }
            for (const int dx : {-3, 3}) {   // fiamme e riflesso sull'acqua
                const int h = 4 + static_cast<int>(3 * caso(dx, static_cast<int>(tm * 12), s));
                for (int y = 0; y < h; ++y) metti(cx + dx, pelo - 1 - y, y < 2 ? BIANCO : y < 4 ? GIALLO : ROSSO_CHIARO);
                metti(cx + dx - 1, pelo - 2, GIALLO);
                metti(cx + dx + 1, pelo - 2, GIALLO);
                for (int y = pelo + 1; y < pelo + 10; y += 2)
                    if (caso(cx + dx, y + static_cast<int>(tm * 8), s) < 0.7) metti(cx + dx + (y % 4 == 1 ? 1 : 0), y, y < pelo + 5 ? GIALLO : 6);
            }
            f.scrivi(t, r.x + 3, r.y + 2, "MARCATORE", GIALLO, 1);
            break;
        }
    }
    (void)MARRONE;
    (void)GRIGIO;
    (void)GRIGIO_SCURO;
}

}  // namespace m688
