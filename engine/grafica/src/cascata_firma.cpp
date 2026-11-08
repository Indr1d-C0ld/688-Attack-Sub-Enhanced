#include "grafica/cascata_firma.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "grafica/font.hpp"
#include "nucleo/analisi_acustica.hpp"
#include "nucleo/casuale.hpp"

namespace m688 {

namespace {

// la scala FOSFORO della stazione (app/sonar_enhanced.cpp)
struct Rgb {
    double r, g, b;
};
constexpr Rgb FOSFORO[] = {{0, 0, 0}, {6, 40, 8}, {18, 100, 22}, {40, 170, 45}, {80, 230, 80}, {190, 255, 180}};

constexpr double DURATA_S = 900.0;                 // la cascata copre un quarto d'ora: meta' piano, poi accelera
constexpr double INIZIO_RAMPA = 0.5, FINE_RAMPA = 0.6;   // frazioni della durata
constexpr double QUOTA_RICEVITORE_M = 100.0, VELOCITA_RICEVITORE_KN = 3.0;   // in ascolto, piano
constexpr double QUOTA_SOTTOMARINO_M = 60.0, VELOCITA_NAVE_KN = 15.0;
constexpr double RILEVAMENTO0 = 35.0;              // il bersaglio entra a 035 e scorre verso destra
constexpr double F_MIN = 10.0, F_MAX = 410.0, BIN_HZ = 0.1;
constexpr double GRANA_DB = 0.53;                  // la grana della stazione (1,5 dB) con l'integrazione piu' lunga (8 misure)
// la distanza: la piu' lontana in cui, nella meta' lenta, spiccano almeno RIGHE_VOLUTE righe di RIGA_VOLUTA_DB (la
// larga banda puo' restare sotto: il LOFAR sente prima, ed e' quello che la cascata mostra)
constexpr double RIGA_VOLUTA_DB = 8.0;
constexpr int RIGHE_VOLUTE = 3;
constexpr double DISTANZE_M[] = {40000, 30000, 22000, 16000, 12000, 9000, 7000, 5000, 3500, 2500, 1800, 1200, 800, 500};

bool passivo(const CatalogoAcustico& c, const std::string& id) {
    const Sensore* s = c.sensore(id);
    return s && s->modo == ModoSensore::passivo;
}

std::string sensore_usato(const CatalogoAcustico& c, const std::string& sensore) {
    if (!sensore.empty()) {
        if (passivo(c, sensore)) return sensore;
        if (passivo(c, sensore + "_passivo")) return sensore + "_passivo";
        const std::string suf = "_attivo";
        if (sensore.size() > suf.size() && sensore.compare(sensore.size() - suf.size(), suf.size(), suf) == 0) {
            const std::string b = sensore.substr(0, sensore.size() - suf.size());
            if (passivo(c, b)) return b;
            if (passivo(c, b + "_passivo")) return b + "_passivo";
        }
    }
    return passivo(c, "bqq5_sfera") ? "bqq5_sfera" : std::string();
}

Ambiente ambiente_medio() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(3);
    a.traffico = 0.5;
    return a;
}

// la velocita' al tempo t (0 = la riga piu' vecchia): lenta, poi la rampa fino a quella data
double velocita_al(const ScenaCascata& s, double t) {
    const double u = std::clamp((t / DURATA_S - INIZIO_RAMPA) / (FINE_RAMPA - INIZIO_RAMPA), 0.0, 1.0);
    return s.velocita_lenta_kn + (s.velocita_kn - s.velocita_lenta_kn) * u;
}
// la strada fatta fino a t (m), integrando la velocita' a passi di 10 s
double percorso_m(const ScenaCascata& s, double t) {
    double d = 0.0;
    for (double x = 0.0; x < t; x += 10.0) d += velocita_al(s, x + std::min(5.0, (t - x) / 2)) * 1852.0 / 3600.0 * std::min(10.0, t - x);
    return d;
}

// la classe al tempo t a distanza r0 iniziale, in moto al traverso
Sorgente sorgente_al(const CatalogoAcustico& c, const std::string& classe, const FirmaRumore& f, const ScenaCascata& s, double r0,
                     double t) {
    UnitaAcustica u;
    u.id = 1;
    u.classe = classe;
    u.categoria = f.nave ? "nave" : "sottomarino";
    const double rotta = RILEVAMENTO0 + 90.0, d = percorso_m(s, t);
    u.pos = Vec2{r0 * std::sin(radianti(RILEVAMENTO0)) + d * std::sin(radianti(rotta)),
                 r0 * std::cos(radianti(RILEVAMENTO0)) + d * std::cos(radianti(rotta))};
    u.quota_m = s.quota_m;
    u.rotta = rotta;
    u.velocita_kn = velocita_al(s, t);
    return sorgente_acustica(c, u);
}

double rilevamento_di(Vec2 p) { return rilevamento(Vec2{}, p); }

// come SonarEnhanced::normalizza_btr: ogni fascio meno la media dei fasci da 2 a 6 di distanza, senza i ciechi
std::vector<float> normalizza(const RigaCascata& r) {
    const int n = static_cast<int>(r.db.size());
    std::vector<float> v(r.db.size(), 0.0f);
    const auto cieco = [&](int i) { return static_cast<std::size_t>(i) < r.cieco.size() && r.cieco[static_cast<std::size_t>(i)]; };
    for (int i = 0; i < n; ++i) {
        if (cieco(i)) continue;
        float s = 0.0f;
        int k = 0;
        for (int d = 2; d <= 6; ++d)
            for (const int j : {(i + d) % n, (i - d + n) % n})
                if (!cieco(j)) {
                    s += r.db[static_cast<std::size_t>(j)];
                    ++k;
                }
        v[static_cast<std::size_t>(i)] = r.db[static_cast<std::size_t>(i)] - (k ? s / static_cast<float>(k) : 0.0f);
    }
    return v;
}

// Il normalizzatore degli schermi LOFAR (a due passi, TPM): ogni bin meno la media dei bin attorno (da 3 a 3 + FINESTRA
// per parte, il centro escluso), cosi' il continuo del bersaglio vicino sparisce e restano le righe strette
std::vector<float> normalizza_spettro(const std::vector<float>& s) {
    constexpr int FINESTRA = 40, BUCO = 3;   // 4 Hz per parte con bin di 0,1 Hz
    const int n = static_cast<int>(s.size());
    std::vector<double> cum(static_cast<std::size_t>(n) + 1, 0.0);
    for (int i = 0; i < n; ++i) cum[static_cast<std::size_t>(i) + 1] = cum[static_cast<std::size_t>(i)] + s[static_cast<std::size_t>(i)];
    const auto somma = [&](int a, int b) {   // [a, b): somma e quanti
        a = std::clamp(a, 0, n);
        b = std::clamp(b, 0, n);
        return b > a ? std::pair{cum[static_cast<std::size_t>(b)] - cum[static_cast<std::size_t>(a)], b - a} : std::pair{0.0, 0};
    };
    std::vector<float> v(s.size());
    for (int i = 0; i < n; ++i) {
        const auto [s1, n1] = somma(i - BUCO - FINESTRA, i - BUCO);
        const auto [s2, n2] = somma(i + BUCO + 1, i + BUCO + 1 + FINESTRA);
        const int m = n1 + n2;
        v[static_cast<std::size_t>(i)] = s[static_cast<std::size_t>(i)] - (m ? static_cast<float>((s1 + s2) / m) : 0.0f);
    }
    return v;
}

// la scala AUTO della stazione: pavimento al 20% dei valori, tetto al 99,5% piu' 2 dB (almeno 8 dB sopra)
void scala_auto(std::vector<float> v, double& pav, double& tetto) {
    if (v.empty()) {
        pav = 2.0;
        tetto = 16.0;
        return;
    }
    const auto q = [&](double p) {
        const auto k = static_cast<std::size_t>(std::clamp(p * static_cast<double>(v.size() - 1), 0.0, static_cast<double>(v.size() - 1)));
        std::nth_element(v.begin(), v.begin() + static_cast<std::ptrdiff_t>(k), v.end());
        return static_cast<double>(v[k]);
    };
    pav = std::round(q(0.2));
    tetto = std::max(std::round(q(0.995) + 2.0), pav + 8.0);
}

}  // namespace

void applica_palette_cascata(Palette& p, int base, int livelli) {
    constexpr int N = static_cast<int>(std::size(FOSFORO));
    for (int i = 0; i < livelli; ++i) {
        const double x = livelli > 1 ? static_cast<double>(i) / (livelli - 1) * (N - 1) : 0.0;
        const int k = std::min(static_cast<int>(x), N - 2);
        const double f = x - k;
        const auto v = [&](double a, double b) { return static_cast<std::uint8_t>(std::clamp(std::lround(a + (b - a) * f), 0L, 255L)); };
        if (i == 0) p.imposta(base, 0, 0, 0);   // sotto il pavimento il fondo resta nero
        else p.imposta(base + i, v(FOSFORO[k].r, FOSFORO[k + 1].r), v(FOSFORO[k].g, FOSFORO[k + 1].g), v(FOSFORO[k].b, FOSFORO[k + 1].b));
    }
}

ScenaCascata scena_cascata(const CatalogoAcustico& c, const std::string& classe, double velocita_kn, const std::string& sensore) {
    ScenaCascata s;
    s.sensore = sensore_usato(c, sensore);
    const FirmaRumore* f = c.firma(classe);
    const Sensore* sn = c.sensore(s.sensore);
    if (!f || !sn) return s;
    s.quota_m = f->nave ? 0.0 : QUOTA_SOTTOMARINO_M;
    s.velocita_kn = velocita_kn > 0.0 ? velocita_kn : f->nave ? VELOCITA_NAVE_KN : velocita_cavitazione_kn(*f, s.quota_m) + 1.5;
    s.cavitazione = cavita(*f, s.velocita_kn, s.quota_m);
    // la meta' lenta: sotto la cavitazione, dove le righe dei macchinari spiccano sul continuo
    s.velocita_lenta_kn = std::max(3.0, 0.5 * s.velocita_kn);
    if (!f->nave) s.velocita_lenta_kn = std::min(s.velocita_lenta_kn, std::max(2.0, 0.8 * velocita_cavitazione_kn(*f, s.quota_m)));
    const Ricevitore ric{Vec2{}, QUOTA_RICEVITORE_M, 0.0, VELOCITA_RICEVITORE_KN, sn};
    const Ambiente amb = ambiente_medio();
    s.distanza_m = DISTANZE_M[std::size(DISTANZE_M) - 1];
    for (const double r0 : DISTANZE_M) {   // dalla piu' lontana: la prima in cui spicca nella meta' lenta
        const Sorgente so = sorgente_al(c, classe, *f, s, r0, DURATA_S * INIZIO_RAMPA / 2);
        const Spettro sp = spettro_fascio({so}, ric, amb, rilevamento_di(so.pos), F_MIN, F_MAX, BIN_HZ);
        const std::vector<float> n = normalizza_spettro(sp.snr_db);
        int righe = 0;   // i picchi sopra la soglia
        for (std::size_t i = 1; i + 1 < n.size(); ++i) righe += n[i] >= RIGA_VOLUTA_DB && n[i] >= n[i - 1] && n[i] > n[i + 1];
        if (righe >= RIGHE_VOLUTE) {
            s.distanza_m = r0;
            break;
        }
    }
    return s;
}

void disegna_cascata_firma(Tela& t, Rettangolo r, const CatalogoAcustico& c, const std::string& classe, double velocita_kn,
                           std::uint64_t seme, const std::string& sensore, const Font* font, int base, int livelli) {
    const auto nero = static_cast<std::uint8_t>(base);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, nero);
    const ScenaCascata sc = scena_cascata(c, classe, velocita_kn, sensore);
    const FirmaRumore* f = c.firma(classe);
    const Sensore* sn = c.sensore(sc.sensore);
    if (!f || !sn || r.w < 16 || r.h < 16) return;
    const int testa = font ? font->altezza_riga(1) + 1 : 0;
    const int bw = std::max(8, r.w * 2 / 5), lw = r.w - bw - 2, h = r.h - testa;
    if (h < 4 || lw < 4) return;
    const Ricevitore ric{Vec2{}, QUOTA_RICEVITORE_M, 0.0, VELOCITA_RICEVITORE_KN, sn};
    const Ambiente amb = ambiente_medio();
    Casuale grana(seme ^ 0xca5cadeull);
    // le righe dalla piu' vecchia (in basso) alla piu' recente (in alto)
    std::vector<std::vector<float>> btr(static_cast<std::size_t>(h)), lofar(static_cast<std::size_t>(h));
    std::vector<std::vector<bool>> ciechi(static_cast<std::size_t>(h));
    std::vector<float> tutti_b, tutti_l;
    double ril0 = 0.0, passo = 5.0;
    for (int k = 0; k < h; ++k) {
        const double tempo = DURATA_S * k / std::max(1, h - 1);
        const Sorgente so = sorgente_al(c, classe, *f, sc, sc.distanza_m, tempo);
        const RigaCascata riga = cascata({so}, ric, amb, 72, 0.0, &grana);
        ril0 = riga.rilevamento0;
        passo = riga.passo;
        const std::vector<float> v = normalizza(riga);
        // BTR: un pixel di colonna per un rilevamento (0-360 su bw), il fascio piu' vicino
        auto& b = btr[static_cast<std::size_t>(h - 1 - k)];
        auto& cb = ciechi[static_cast<std::size_t>(h - 1 - k)];
        b.resize(static_cast<std::size_t>(bw));
        cb.resize(static_cast<std::size_t>(bw));
        for (int x = 0; x < bw; ++x) {
            const double ril = 360.0 * (x + 0.5) / bw;
            const int n = static_cast<int>(v.size());
            const int i = n ? static_cast<int>(std::lround(normalizza_360(ril - ril0) / passo)) % n : 0;
            b[static_cast<std::size_t>(x)] = n ? v[static_cast<std::size_t>(i)] : 0.0f;
            cb[static_cast<std::size_t>(x)] = static_cast<std::size_t>(i) < riga.cieco.size() && riga.cieco[static_cast<std::size_t>(i)];
            if (!cb[static_cast<std::size_t>(x)]) tutti_b.push_back(b[static_cast<std::size_t>(x)]);
        }
        // LOFAR del fascio sul bersaglio, con la grana; per colonna il bin piu' forte (le righe sono strette)
        Spettro sp = spettro_fascio({so}, ric, amb, rilevamento_di(so.pos), F_MIN, F_MAX, BIN_HZ);
        for (auto& x : sp.snr_db) {
            const double u1 = std::max(grana.uniforme(), 1e-12), u2 = grana.uniforme();
            x = static_cast<float>(std::max(0.0, x + GRANA_DB * std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2)));
        }
        const std::vector<float> norm = normalizza_spettro(sp.snr_db);
        auto& l = lofar[static_cast<std::size_t>(h - 1 - k)];
        l.assign(static_cast<std::size_t>(lw), -1e9f);
        const std::size_t nb = norm.size();
        for (std::size_t i = 0; i < nb; ++i) {   // per colonna il bin piu' forte (le righe sono strette)
            const auto x = std::min(static_cast<std::size_t>(lw - 1), i * static_cast<std::size_t>(lw) / std::max<std::size_t>(nb, 1));
            l[x] = std::max(l[x], norm[i]);
        }
        tutti_l.insert(tutti_l.end(), l.begin(), l.end());
    }
    double pb = 0, tb = 0, pl = 0, tl = 0;
    scala_auto(tutti_b, pb, tb);
    // LOFAR: le righe occupano poche colonne, la scala parte dal rumore (la mediana) e satura 12 dB sopra
    if (!tutti_l.empty()) {
        std::nth_element(tutti_l.begin(), tutti_l.begin() + static_cast<std::ptrdiff_t>(tutti_l.size() / 2), tutti_l.end());
        pl = tutti_l[tutti_l.size() / 2];
    }
    tl = pl + 12.0;
    const auto colore = [&](double db, double pav, double tetto) {
        const double u = (db - pav) / (tetto - pav);
        return static_cast<std::uint8_t>(base + std::clamp(static_cast<int>(std::lround(u * (livelli - 1))), 0, livelli - 1));
    };
    const int y0 = r.y + testa, xl = r.x + bw + 2;
    for (int y = 0; y < h; ++y) {
        const auto& b = btr[static_cast<std::size_t>(y)];
        const auto& cb = ciechi[static_cast<std::size_t>(y)];
        for (int x = 0; x < bw; ++x)
            t.pixel(r.x + x, y0 + y, cb[static_cast<std::size_t>(x)] ? nero : colore(b[static_cast<std::size_t>(x)], pb, tb));
        const auto& l = lofar[static_cast<std::size_t>(y)];
        for (int x = 0; x < lw; ++x) t.pixel(xl + x, y0 + y, colore(l[static_cast<std::size_t>(x)], pl, tl));
    }
    if (font) {
        const auto chiaro = static_cast<std::uint8_t>(base + livelli - 1), medio = static_cast<std::uint8_t>(base + livelli / 2);
        font->scrivi(t, r.x + 1, r.y, "BTR", chiaro, 1);
        font->scrivi(t, xl + 1, r.y, "LOFAR", chiaro, 1);
        const std::string hz = "10-410 HZ";
        font->scrivi(t, r.x + r.w - font->larghezza(hz, 1) - 1, r.y, hz, medio, 1);
    }
}

}  // namespace m688
