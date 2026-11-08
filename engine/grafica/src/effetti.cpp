#include "grafica/effetti.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/fenomeni.hpp"

namespace m688 {

namespace {

constexpr double G = 9.81;
constexpr int BAYER[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

double soglia(int x, int y) { return (BAYER[y & 3][x & 3] + 0.5) / 16.0; }

std::uint32_t mescola(std::uint32_t a, std::uint32_t b) {
    std::uint32_t h = a * 0x9e3779b1u ^ (b + 0x7f4a7c15u + (a << 6) + (a >> 2));
    h ^= h >> 15; h *= 0x2c1b3c6du; h ^= h >> 12;
    return h;
}

// materiali leggeri: dopo la salita restano sospesi e vanno col vento (velocita' verticale di equilibrio)
bool leggera(Materia m) {
    return m == Materia::fumo || m == Materia::vapore || m == Materia::nebbia || m == Materia::corona || m == Materia::illuminante;
}
double verticale_equilibrio(Materia m) {
    switch (m) {
        case Materia::vapore: return 1.0;
        case Materia::nebbia: return -1.0;
        case Materia::corona: return 0.3;
        case Materia::illuminante: return -4.0;   // sotto il paracadute (STIMA: 4 m/s)
        default: return 0.0;
    }
}

// coordinate della nave (x a prua, y a sinistra, z in alto, dopo l'assetto del corpo) -> mondo
struct Punto { double x = 0, y = 0, z = 0; };
Punto nel_mondo(const Nave& n, double X, double Y, double Z) {
    const double r = radianti(n.rotta), s = std::sin(r), c = std::cos(r);
    return {n.pos.x + X * s - Y * c, n.pos.y + X * c + Y * s, Z};
}
Punto nel_mondo(const Nave& n, const Corpo& c, double x, double y, double z) {
    double X, Y, Z;
    c.assetto.mondo(x, y, z, X, Y, Z);
    return nel_mondo(n, X, Y, Z);
}

// tono del fumo dal combustibile (dossier 07 §4.3): gasolio e greggio neri, locali e munizioni grigi
double tono_fumo(const std::string& uso, const std::string& carico) {
    if (uso == "macchine" || uso == "ala" || uso == "carico" || carico == "avio") return 0.05;
    if (uso == "deposito") return 0.25;
    return 0.4;
}

}  // namespace

Effetti::Effetti(std::uint64_t seme) : caso_(seme) { imposta_vento(250.0, 7.0); }

void Effetti::imposta_vento(double da_gradi, double velocita) {
    vento_ = std::max(velocita, 0.0);
    const double a = radianti(da_gradi + 180.0);   // verso cui soffia
    vento_x_ = vento_ * std::sin(a);
    vento_y_ = vento_ * std::cos(a);
}

Particella& Effetti::nuova(Materia m) {
    particelle_.emplace_back();
    auto& p = particelle_.back();
    p.materia = m;
    p.seme = static_cast<std::uint32_t>(caso_.prossimo() >> 32);
    return p;
}

void Effetti::segui(int id, const DanniNave* danni, const Modello* modello) {
    Seguita s;
    s.id = id;
    s.danni = danni;
    s.modello = modello;
    s.corpi = danni->corpi().size();
    for (const auto& c : danni->corpi()) s.affondati.push_back(c.affondato);
    if (modello) {
        // alberi: radici "albero*" e "palo*" dalla piu' bassa; ciascuna porta con se' le parti sottili che stanno sopra
        // la sua base entro la sua impronta (alberetti, pennoni, antenne), non le strutture
        struct Scatola { double x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9, z0 = 1e9, z1 = -1e9; };
        std::vector<Scatola> sc(modello->parti().size());
        for (const auto& t : modello->triangoli()) {
            if (t.parte >= sc.size()) continue;
            auto& b = sc[t.parte];
            for (const auto& v : t.v) {
                b.x0 = std::min<double>(b.x0, v[0]); b.x1 = std::max<double>(b.x1, v[0]);
                b.y0 = std::min<double>(b.y0, v[1]); b.y1 = std::max<double>(b.y1, v[1]);
                b.z0 = std::min<double>(b.z0, v[2]); b.z1 = std::max<double>(b.z1, v[2]);
            }
        }
        const auto& nomi = modello->parti();
        auto comincia = [](const std::string& n, const char* p) { return n.rfind(p, 0) == 0; };
        auto struttura = [&](const std::string& n) {
            for (const char* p : {"scafo", "carena", "tuga", "casa", "plancia", "fumaiolo", "timoneria", "ponte",
                                  "passerella", "tubolature", "hangar", "sovrastruttura", "torretta"})
                if (comincia(n, p)) return true;
            return false;
        };
        std::vector<int> radici;
        for (std::size_t i = 0; i < nomi.size(); ++i)
            if ((comincia(nomi[i], "albero") || comincia(nomi[i], "palo")) && sc[i].z1 > sc[i].z0) radici.push_back(static_cast<int>(i));
        std::sort(radici.begin(), radici.end(), [&](int a, int b) { return sc[static_cast<std::size_t>(a)].z0 < sc[static_cast<std::size_t>(b)].z0; });
        std::vector<bool> preso(nomi.size(), false);
        for (int r : radici) {
            if (preso[static_cast<std::size_t>(r)]) continue;
            const auto& b = sc[static_cast<std::size_t>(r)];
            Albero a;
            a.px = (b.x0 + b.x1) / 2; a.py = (b.y0 + b.y1) / 2; a.pz = b.z0;
            double cima = b.z1;
            for (std::size_t i = 0; i < nomi.size(); ++i) {
                const auto& c = sc[i];
                if (preso[i] || struttura(nomi[i]) || c.z1 < c.z0) continue;
                const bool sopra = static_cast<int>(i) == r || c.z0 >= b.z0 + 0.25 * (b.z1 - b.z0);
                const bool dentro = c.x1 >= b.x0 - 1 && c.x0 <= b.x1 + 1 && c.y1 >= b.y0 - 1 && c.y0 <= b.y1 + 1;
                if (!sopra || !dentro) continue;
                preso[i] = true;
                a.parti.push_back(static_cast<int>(i));
                cima = std::max(cima, c.z1);
            }
            a.altezza = std::max(cima - a.pz, 1.0);
            s.alberi.push_back(std::move(a));
        }
    }
    seguite_.push_back(std::move(s));
}

void Effetti::abbatti(Albero& a, double dx, double dy) {
    if (a.cade || a.a_terra) return;
    const double n = std::hypot(dx, dy);
    if (n < 1e-6) { const double t = caso_.uniforme(0, 2 * PI); dx = std::cos(t); dy = std::sin(t); }
    else { dx /= n; dy /= n; }
    a.ax = -dy; a.ay = dx;   // la cima va verso (dx, dy)
    a.cade = true;
    a.angolo = 0.05;
    a.omega = caso_.uniforme(0.2, 0.5);
}

void Effetti::decora(int id, PosaNave& posa) const {
    for (const auto& s : seguite_) {
        if (s.id != id) continue;
        for (const auto& a : s.alberi) {
            if (!a.cade && !a.a_terra) continue;
            for (int p : a.parti) posa.parti.push_back({p, a.px, a.py, a.pz, a.ax, a.ay, a.angolo});
        }
    }
}

// --- piccoli insiemi di particelle --------------------------------------------------------------------------------

void Effetti::spruzzi(double x, double y, double z, int n, double alto, double largo) {
    for (int i = 0; i < n; ++i) {
        auto& p = nuova(Materia::spruzzo);
        const double a = caso_.uniforme(0, 2 * PI), r = largo * std::sqrt(caso_.uniforme());
        p.x = x + r * std::cos(a); p.y = y + r * std::sin(a); p.z = p.z0 = z;
        p.H = alto * caso_.uniforme(0.4, 1.0);
        p.ta = 0.8 * std::sqrt(2 * p.H / G) + 0.05;
        const double v = 0.25 * std::sqrt(G * p.H);
        p.vx = v * std::cos(a) * caso_.uniforme(); p.vy = v * std::sin(a) * caso_.uniforme();
        p.v_limite = 15;
        p.r = 0.4 + 0.06 * p.H; p.dr = 0.1 + 0.02 * p.H;
        p.vita = p.ta + std::sqrt(2 * (p.H + z) / G) + 0.6;
        p.opacita = 0.85;
    }
}

void Effetti::bolle(double x, double y, double raggio, int n, bool fango, double alto) {
    for (int i = 0; i < n; ++i) {
        const double a = caso_.uniforme(0, 2 * PI), r = raggio * std::sqrt(caso_.uniforme());
        const double px = x + r * std::cos(a), py = y + r * std::sin(a);
        auto& b = nuova(fango ? Materia::fango : Materia::schiuma);   // bolla che scoppia: piccola cupola
        b.x = px; b.y = py;
        b.H = alto * caso_.uniforme(0.3, 1.0);
        b.ta = 0.3 + 0.15 * b.H;
        b.v_limite = 5;
        b.r = 0.4 + 0.25 * alto; b.dr = 0.2;
        b.vita = b.ta + 1.0;
        b.opacita = 0.8;
        auto& c = nuova(fango ? Materia::fango : Materia::schiuma);   // e la schiuma che lascia
        c.x = px; c.y = py; c.piatta = true;
        c.r = 1.0 + alto; c.dr = 0.3; c.r_max = 2.0 + 3.0 * alto;
        c.vita = caso_.uniforme(8, 16);
        c.opacita = 0.75;
    }
}

void Effetti::emersione_violenta(Vec2 pos, double rotta, double velocita, double angolo, double lunghezza, double fuori,
                                 double ricaduta) {
    const double r = radianti(rotta), ax = std::sin(r), ay = std::cos(r);   // verso la prua
    const double nx = ay, ny = -ax;                                           // di traverso (dritta)
    const double L = std::max(lunghezza, 10.0), fu = std::clamp(fuori, 0.05, 0.6);
    const double hb = fu * L * std::sin(radianti(std::clamp(angolo, 0.0, 60.0)));   // quota della prua al culmine
    const double s0 = L / 2 - fu * L;                                          // dal centro: punto che resta in acqua
    // punto a `s` m dal centro lungo lo scafo, dopo `t` s: il battello continua ad avanzare mentre emerge
    auto lungo = [&](double s, double t = 0.0) { return Vec2{pos.x + ax * (s + velocita * t), pos.y + ay * (s + velocita * t)}; };
    // 1. acqua che cola dalla prua mentre esce: dalla punta verso il perno, sollevata e ricadente ai lati
    const int n = std::clamp(static_cast<int>(8 * fu * L), 40, 320);
    const double t_salita = std::max(0.3 * ricaduta, 0.5);
    for (int i = 0; i < n; ++i) {
        const double u = caso_.uniforme();                      // 0 al perno, 1 alla punta
        const Vec2 c = lungo(s0 + u * fu * L, (1 - u) * t_salita);
        const double lato = caso_.uniforme() < 0.5 ? -1.0 : 1.0, d = caso_.uniforme(1.0, 4.0);
        auto& p = nuova(Materia::spruzzo);
        p.x = c.x + lato * d * nx; p.y = c.y + lato * d * ny;
        p.H = (1.0 + 0.6 * hb * u) * caso_.uniforme(0.5, 1.0);
        p.ta = 0.8 * std::sqrt(2 * p.H / G) + 0.05;
        const double v = caso_.uniforme(0.5, 2.0) + 0.1 * velocita;
        p.vx = lato * v * nx + 0.3 * velocita * ax; p.vy = lato * v * ny + 0.3 * velocita * ay;
        p.v_limite = 15;
        p.r = 0.6 + 0.02 * L; p.dr = 0.15;
        p.ritardo = (1 - u) * t_salita;
        p.vita = p.ta + std::sqrt(2 * p.H / G) + 0.8;
        p.opacita = 0.85;
    }
    // 2. colonna alla prua: l'acqua sollevata dalla punta che sfonda la superficie
    const Vec2 prua = lungo(L / 2);
    for (int i = 0; i < 40; ++i) {
        auto& p = nuova(Materia::spruzzo);
        const double a = caso_.uniforme(0, 2 * PI), rho = 3.0 * std::sqrt(caso_.uniforme());
        p.x = prua.x + rho * std::cos(a); p.y = prua.y + rho * std::sin(a);
        p.H = (2.0 + 0.9 * hb) * caso_.uniforme(0.6, 1.0);
        p.ta = std::max(t_salita, 0.8 * std::sqrt(2 * p.H / G));
        p.vx = 0.15 * velocita * ax + caso_.uniforme(-1.0, 1.0); p.vy = 0.15 * velocita * ay + caso_.uniforme(-1.0, 1.0);
        p.v_limite = 20;
        p.r = 1.0 + 0.03 * L; p.dr = 0.2;
        p.vita = p.ta + std::sqrt(2 * p.H / G) + 1.0;
        p.opacita = 0.9;
    }
    // 3. schiuma bianca lungo lo scafo: resta qualche minuto
    for (int i = 0; i < 6; ++i) {
        auto& p = nuova(Materia::schiuma);
        const Vec2 c = lungo(-L / 2 + L * (i + 0.5) / 6, 0.5 * ricaduta);
        p.x = c.x; p.y = c.y; p.piatta = true;
        p.r = 3.0 + 0.05 * L; p.dr = 0.3; p.r_max = 0.12 * L + 6.0;
        p.vita = caso_.uniforme(180.0, 300.0); p.opacita = 0.85;
    }
    // 4. il colpo della prua sul mare: spruzzi laterali, nebbia e schiuma, con il ritardo della ricaduta
    const Vec2 colpo = lungo(s0 + 0.6 * fu * L, ricaduta);
    const double Hc = 2.0 + 0.5 * hb, largo = 0.1 * L;
    for (int i = 0; i < 80; ++i) {
        auto& p = nuova(Materia::spruzzo);
        const double lato = caso_.uniforme() < 0.5 ? -1.0 : 1.0, lungo_s = caso_.uniforme(-0.5, 0.5) * fu * L;
        p.x = colpo.x + ax * lungo_s + lato * caso_.uniforme(1.0, 3.0) * nx;
        p.y = colpo.y + ay * lungo_s + lato * caso_.uniforme(1.0, 3.0) * ny;
        p.H = Hc * caso_.uniforme(0.4, 1.0);
        p.ta = 0.8 * std::sqrt(2 * p.H / G) + 0.05;
        const double v = largo / std::max(p.ta, 0.3) * caso_.uniforme(0.4, 1.0);
        p.vx = lato * v * nx; p.vy = lato * v * ny;
        p.v_limite = 20;
        p.r = 0.8 + 0.02 * L; p.dr = 0.2;
        p.ritardo = ricaduta + caso_.uniforme(0.0, 0.2);
        p.vita = p.ta + std::sqrt(2 * p.H / G) + 1.0;
        p.opacita = 0.9;
        if (i % 8 == 0) {
            const double x = p.x, y = p.y, h = p.H, ta = p.ta;   // `p` non vale piu' dopo la prossima nuova()
            auto& m = nuova(Materia::nebbia);
            m.x = x; m.y = y; m.z = 0.6 * h;
            m.r = 2.0 + 0.03 * L; m.dr = 0.4; m.vz = -0.5;
            m.ritardo = ricaduta + ta;
            m.vita = caso_.uniforme(12.0, 25.0);
            m.opacita = 0.45;
        }
    }
    const std::size_t primo = particelle_.size();
    bolle(colpo.x, colpo.y, 0.15 * L, 12, false, 1.0 + 0.1 * hb);
    for (std::size_t k = primo; k < particelle_.size(); ++k) particelle_[k].ritardo = ricaduta;
}

namespace {
// Profilo di lancio (STIME dal dossier 05: §1.2 Harpoon, §2.2 Tomahawk, §3.1-3.4 sovietici, §6.1 aspetto)
struct TrattoVolo {
    double durata, angolo0, angolo1, v0, v1;   // s, gradi sull'orizzonte, m/s
    double quota_crociera = -1;                 // >= 0: l'angolo insegue questa quota (crociera)
    bool fumo = false, fiamma = false;
};
struct ProfiloLancio {
    double salita_m_s;          // risalita sott'acqua (capsula o missile spinto)
    double angolo_sott_acqua;   // gradi: la deriva in avanti prima di uscire e' quota / tan(angolo)
    bool capsula;               // Harpoon: la capsula affiora, espelle i tappi e poi accende il booster
    double spruzzo_m, fumo_r, fiamma_r, corpo_r;
    std::vector<TrattoVolo> tratti;
};
ProfiloLancio profilo_lancio(const std::string& m) {
    if (m == "harpoon")   // capsula a 2-5 m/s; booster 2,5-3 s con un getto di fumo lungo 50-150 m a 20-40 gradi,
                          // poi turbogetto senza fumo che accelera a 240 m/s e scende a 5-15 m
        return {3.0, 45.0, true, 3.0, 1.2, 0.8, 0.3,
                {{3.0, 35, 25, 20, 80, -1, true, true}, {8.0, 0, 0, 80, 240, 10}, {10.0, 0, 0, 240, 240, 10}}};
    if (m == "tomahawk")   // booster Mk 106 acceso sott'acqua, 12 s, salita ripida; sgancio e crociera bassa
        return {12.0, 55.0, false, 6.0, 1.5, 1.0, 0.3,
                {{12.0, 55, 10, 25, 150, -1, true, true}, {15.0, 0, 0, 150, 220, 30}}};
    if (m == "granit")   // grande (10 m, 7 t), contenitori inclinati, booster e salita verso 14-17 km
        return {15.0, 50.0, false, 10.0, 3.0, 2.5, 0.6,
                {{4.0, 60, 55, 20, 250, -1, true, true}, {20.0, 55, 35, 250, 600}}};
    if (m == "vodopad")   // missile-siluro: esce ripido e va in alto sulla parabola
        return {20.0, 70.0, false, 6.0, 1.8, 1.2, 0.4,
                {{6.0, 65, 45, 20, 300, -1, true, true}, {15.0, 45, 20, 300, 280}}};
    // Malakhit e Ametist (P-120, P-70): lancio umido da contenitori a 38,5 gradi, boost di 7 s, crociera a ~60 m
    return {12.0, 38.5, false, 8.0, 2.2, 1.6, 0.5,
            {{7.0, 40, 5, 20, 250, -1, true, true}, {12.0, 0, 0, 250, 300, 60}}};
}
}  // namespace

double Effetti::lancio_missile(Vec2 pos, double rotta, double quota, const std::string& missile, bool da_nave) {
    const ProfiloLancio pr = profilo_lancio(missile);
    const double r = radianti(rotta), ax = std::sin(r), ay = std::cos(r);
    if (da_nave) {   // dal ponte: la nube del booster che avvolge la nave, poi la stessa traiettoria da quota metri
        for (int i = 0; i < 10; ++i) {
            auto& v = nuova(Materia::fumo);
            v.x = pos.x + caso_.uniforme(-6.0, 6.0); v.y = pos.y + caso_.uniforme(-6.0, 6.0); v.z = std::max(quota, 1.0);
            v.tono = 0.85; v.r = 2.0 + pr.fumo_r; v.dr = 1.0; v.r_max = 6.0 * pr.fumo_r + 8.0; v.vz = 0.8;
            v.ritardo = 0.05 * i; v.vita = caso_.uniforme(15.0, 30.0); v.opacita = 0.75;
        }
        auto& lampo = nuova(Materia::fuoco);   // la vampa all'accensione
        lampo.x = pos.x; lampo.y = pos.y; lampo.z = std::max(quota, 1.0); lampo.r = 2.0 * pr.fiamma_r + 1.0; lampo.dr = 2.0;
        lampo.calore = 1.0; lampo.opacita = 1.0; lampo.vita = 0.6;
    }
    const double q = da_nave ? 0.0 : std::max(quota, 0.0);
    const double avanti = q / std::tan(radianti(pr.angolo_sott_acqua));
    const Vec2 uscita{pos.x + ax * avanti, pos.y + ay * avanti};
    const double t0 = da_nave ? 0.0 : std::clamp(q / pr.salita_m_s, 1.0, 30.0);   // dal lancio all'uscita
    double s = 0, z = da_nave ? std::max(quota, 1.0) : 0.5, t = 0;
    double t_motore = t0;
    if (!da_nave) {
    // affioramento: ribollio, spruzzo; vapore del booster che brucia sott'acqua
    const std::size_t primo = particelle_.size();
    bolle(uscita.x, uscita.y, 2.0 + 0.2 * pr.spruzzo_m, 6, false, 0.5 + 0.1 * pr.spruzzo_m);
    for (std::size_t k = primo; k < particelle_.size(); ++k) particelle_[k].ritardo = std::max(t0 - 0.5, 0.0);
    if (pr.capsula) {   // la capsula sporge inclinata per 1-3 s, poi i due tappi saltano via
        auto& c = nuova(Materia::detrito);
        c.x = uscita.x; c.y = uscita.y; c.z = 1.0; c.r = 0.35; c.propulsa = true;
        c.ritardo = t0; c.vita = 2.5;
        t_motore = t0 + 1.5;
        for (int i = 0; i < 2; ++i) {
            auto& tp = nuova(Materia::detrito);
            tp.x = uscita.x; tp.y = uscita.y; tp.z = 1.5; tp.r = 0.2;
            const double a = caso_.uniforme(0, 2 * PI);
            tp.vx = 3 * std::cos(a); tp.vy = 3 * std::sin(a); tp.vz = caso_.uniforme(5.0, 8.0);
            tp.v_limite = 20; tp.ritardo = t_motore - 0.3; tp.vita = 3.0;
        }
    }
    for (int i = 0; i < 25; ++i) {   // acqua sollevata all'uscita
        auto& p = nuova(Materia::spruzzo);
        const double a = caso_.uniforme(0, 2 * PI), rho = 2.0 * std::sqrt(caso_.uniforme());
        p.x = uscita.x + rho * std::cos(a); p.y = uscita.y + rho * std::sin(a);
        p.H = pr.spruzzo_m * caso_.uniforme(0.5, 1.0);
        p.ta = 0.8 * std::sqrt(2 * p.H / G) + 0.05;
        p.vx = caso_.uniforme(-2.0, 2.0) + 3 * ax; p.vy = caso_.uniforme(-2.0, 2.0) + 3 * ay;
        p.v_limite = 15; p.r = 0.6 + 0.08 * pr.spruzzo_m; p.dr = 0.2;
        p.ritardo = t_motore + caso_.uniforme(0.0, 0.3);
        p.vita = p.ta + std::sqrt(2 * p.H / G) + 0.8; p.opacita = 0.85;
    }
    for (int i = 0; i < 6; ++i) {   // il getto del booster sul mare: vapore e nebbia bianca
        auto& v = nuova(Materia::vapore);
        v.x = uscita.x + caso_.uniforme(-3.0, 3.0); v.y = uscita.y + caso_.uniforme(-3.0, 3.0); v.z = 1.0;
        v.r = 1.5 + pr.fumo_r; v.dr = 0.8; v.r_max = 4.0 * pr.fumo_r + 4.0; v.vz = 0.5;
        v.ritardo = t_motore + 0.1 * i; v.vita = caso_.uniforme(6.0, 12.0); v.opacita = 0.7;
    }
    }
    // traiettoria: passi di 0,05 s; corpo (segmenti di 0,25 s), fiamma e fumo lungo i tratti
    constexpr double DT = 0.05, SEG = 0.25;
    double t_seg = 0, s_seg = 0, z_seg = z, t_fumo = 0;
    auto punto = [&](double ss) { return Vec2{uscita.x + ax * ss, uscita.y + ay * ss}; };
    for (const auto& tr : pr.tratti) {
        for (double u = 0; u < tr.durata - 1e-9; u += DT) {
            const double k = u / tr.durata, v = tr.v0 + (tr.v1 - tr.v0) * k;
            double ang = tr.angolo0 + (tr.angolo1 - tr.angolo0) * k;
            if (tr.quota_crociera >= 0) ang = std::clamp(gradi(std::atan2(tr.quota_crociera - z, 4.0 * v)), -12.0, 12.0);
            s += v * std::cos(radianti(ang)) * DT;
            z = std::max(z + v * std::sin(radianti(ang)) * DT, 0.5);
            t += DT;
            if (tr.fumo && t - t_fumo >= 0.1) {   // scia di fumo bianco denso che resta sospesa e si allarga
                t_fumo = t;
                const Vec2 c = punto(s);
                auto& f = nuova(Materia::fumo);
                f.x = c.x; f.y = c.y; f.z = z; f.tono = 0.9;
                f.r = pr.fumo_r; f.dr = 0.5 + 0.2 * pr.fumo_r; f.r_max = 6.0 * pr.fumo_r + 4.0;
                f.ritardo = t_motore + t; f.vita = caso_.uniforme(25.0, 50.0); f.opacita = 0.8;
            }
            if (t - t_seg >= SEG - 1e-9) {   // segmento del corpo (e della fiamma) da t_seg a t
                const Vec2 a0 = punto(s_seg), a1 = punto(s);
                const double d = t - t_seg;
                auto segmento = [&](Materia m, double raggio) -> Particella& {
                    auto& p = nuova(m);
                    p.x = a0.x; p.y = a0.y; p.z = z_seg; p.propulsa = true;
                    p.vx = (a1.x - a0.x) / d; p.vy = (a1.y - a0.y) / d; p.vz = (z - z_seg) / d;
                    p.r = raggio; p.ritardo = t_motore + t_seg; p.vita = d;
                    return p;
                };
                segmento(Materia::detrito, pr.corpo_r);
                if (tr.fiamma) { auto& f = segmento(Materia::fuoco, pr.fiamma_r); f.calore = 1.0; f.opacita = 1.0; }
                t_seg = t; s_seg = s; z_seg = z;
            }
        }
        if (tr.fiamma) {   // fine del booster: il cilindro si sgancia e ricade (Tomahawk, Granit, sovietici)
            const Vec2 c = punto(s);
            auto& b = nuova(Materia::detrito);
            b.x = c.x; b.y = c.y; b.z = z; b.r = 0.8 * pr.corpo_r;
            b.vx = 0.3 * tr.v1 * ax; b.vy = 0.3 * tr.v1 * ay; b.vz = 0.0; b.v_limite = 60;
            b.ritardo = t_motore + t; b.vita = std::sqrt(2 * z / G) + 10.0;
        }
    }
    return t_motore;
}

void Effetti::piuma(int id, Vec2 pos, double rotta, double velocita, double altezza, double dt) {
    const double v = std::max(velocita, 0.0);
    if (v < 0.5 || altezza <= 0) return;   // fermo: solo l'albero, niente scia
    const double r = radianti(rotta), ax = std::sin(r), ay = std::cos(r);
    // onda di prua dell'albero: alta circa v^2/2g (0,9 m a 8 nodi, 1,9 m a 12); STIMA
    const double h = std::min(1.2 * v * v / (2 * G) + 0.1, altezza + 1.5);
    double& acc = piume_[id];
    acc += (3.0 + 2.5 * v) * dt;
    const int n = static_cast<int>(acc);
    acc -= n;
    for (int i = 0; i < n; ++i) {
        auto& p = nuova(Materia::spruzzo);   // spruzzo davanti all'albero, buttato ai lati e indietro
        const double lato = caso_.uniforme() < 0.5 ? -1.0 : 1.0;
        p.x = pos.x + 0.3 * ax; p.y = pos.y + 0.3 * ay;
        p.H = h * caso_.uniforme(0.5, 1.0);
        p.ta = 0.8 * std::sqrt(2 * p.H / G) + 0.05;
        const double lat = caso_.uniforme(0.3, 0.8) * v;
        p.vx = lato * lat * ay - 0.2 * v * ax; p.vy = -lato * lat * ax - 0.2 * v * ay;
        p.v_limite = 8; p.r = 0.15 + 0.04 * v; p.dr = 0.1;
        p.vita = p.ta + std::sqrt(2 * p.H / G) + 0.3; p.opacita = 0.8;
        if (i % 2 == 0) {   // e la scia di schiuma: resta dove passa, piu' lunga piu' si va veloci
            const double x = pos.x - 0.5 * ax, y = pos.y - 0.5 * ay;
            auto& f = nuova(Materia::schiuma);
            f.x = x; f.y = y; f.piatta = true;
            f.r = 0.3 + 0.08 * v; f.dr = 0.05 + 0.04 * v; f.r_max = 0.6 + 0.35 * v;
            f.vita = 4.0 + 1.5 * v; f.opacita = std::min(0.9, 0.3 + 0.1 * v);
        }
    }
}

void Effetti::segno_superficie(int tipo, Vec2 pos, double durata, double intensita) {
    const double k = std::clamp(intensita, 0.05, 1.0), vita = std::max(durata, 10.0);
    if (tipo == 0) {   // aria che risale: una macchia di schiuma che dura, con bolle all'inizio
        auto& p = nuova(Materia::schiuma);
        p.x = pos.x; p.y = pos.y; p.piatta = true;
        p.r = 5.0 + 25.0 * k; p.dr = 0.05; p.r_max = 10.0 + 60.0 * k;
        p.vita = vita; p.opacita = 0.8;
        bolle(pos.x, pos.y, 5.0 + 20.0 * k, 4 + static_cast<int>(16 * k), false, 0.5 + k);
    } else if (tipo == 1) {   // nafta: chiazze scure e piatte che si allargano e derivano col vento
        for (int i = 0; i < 1 + static_cast<int>(3 * k); ++i) {
            auto& p = nuova(Materia::nafta);
            p.x = pos.x + caso_.uniforme(-20.0, 20.0); p.y = pos.y + caso_.uniforme(-20.0, 20.0); p.piatta = true;
            p.r = 10.0 + 30.0 * k; p.dr = (50.0 + 400.0 * k) / vita; p.r_max = 60.0 + 450.0 * k;
            p.vx = 0.03 * vento_x_; p.vy = 0.03 * vento_y_;
            p.vita = vita; p.opacita = 0.8;
        }
    } else {   // rottami alla deriva
        for (int i = 0; i < 3 + static_cast<int>(17 * k); ++i) {
            auto& p = nuova(Materia::detrito);
            const double a = caso_.uniforme(0, 2 * PI), r = (10.0 + 40.0 * k) * std::sqrt(caso_.uniforme());
            p.x = pos.x + r * std::cos(a); p.y = pos.y + r * std::sin(a);
            p.galleggia = true;
            p.vx = 0.03 * vento_x_ + caso_.uniforme(-0.2, 0.2); p.vy = 0.03 * vento_y_ + caso_.uniforme(-0.2, 0.2);
            p.r = caso_.uniforme(0.3, 0.9);
            p.vita = vita; p.opacita = 1.0;
        }
    }
}

// --- esplosioni -------------------------------------------------------------------------------------------------

void Effetti::esplosione_subacquea(Vec2 pos, double profondita, double W, double fondale) {
    const FenomenoSubacqueo f = fenomeno_subacqueo(W, profondita, fondale);
    const double d = f.d;
    const double t_urto = d / 1500.0;
    // variazione dal seme (le misure reali variano fino a +-50%; qui +-20%)
    const double kH = caso_.uniforme(0.8, 1.2), kR = caso_.uniforme(0.85, 1.15);

    {   // anello scuro dell'urto in superficie: un lampo di un decimo di secondo
        auto& p = nuova(Materia::anello_scuro);
        p.x = pos.x; p.y = pos.y; p.piatta = true;
        p.H = d; p.r = 0.5; p.r_max = f.R_slick;
        p.ritardo = t_urto;
        p.vita = std::max(f.t_slick - t_urto, 0.03) + 0.05;
        p.opacita = 0.9;
    }

    auto cupola = [&](double H, double R, double t0, double ta, int n, double opacita) {
        if (H < 0.3 || R <= 0) return;
        const double C = d * d / (d * d + R * R);
        for (int i = 0; i < n; ++i) {
            const double rho = R * std::sqrt(caso_.uniforme()), a = caso_.uniforme(0, 2 * PI);
            const double h = H / (1 - C) * (d * d / (d * d + rho * rho) - C);
            if (h < 0.3) continue;
            auto& p = nuova(Materia::spruzzo);
            p.x = pos.x + rho * std::cos(a); p.y = pos.y + rho * std::sin(a);
            p.H = h;
            p.ta = ta * caso_.uniforme(0.85, 1.15);
            const double v = 0.12 * 2 * h / p.ta;
            p.vx = v * std::cos(a); p.vy = v * std::sin(a);
            p.v_limite = 35;
            p.r = std::max(0.8, 0.04 * H + 0.02 * R); p.dr = 0.08 * p.r;
            p.ritardo = t0;
            p.vita = p.ta + std::sqrt(2 * h / G) + 2.0;
            p.opacita = opacita;
        }
    };
    if (f.H_cupola > 0.3)
        cupola(f.H_cupola * kH, f.R_cupola * kR, t_urto, f.t_apice_cupola, std::clamp(static_cast<int>(30 + 1.5 * f.H_cupola), 30, 300), 0.85);
    if (f.H_gobba > 0) cupola(f.H_gobba, std::max(0.6 * d, 2.0 * f.A_max), t_urto, 1.0 + 0.1 * f.H_gobba, 24, 0.6);
    // pulsazioni successive: sussulti piu' piccoli della cupola
    double tp = f.T1;
    for (int k = 1; k <= f.pulsazioni; ++k) {
        tp += k == 1 ? f.T2 : f.T3;
        const double H = std::max(f.H_cupola, 0.3 * f.H_pennacchio) * std::pow(0.37, k);
        cupola(H, 0.6 * std::max(f.R_cupola, f.A_max), tp, 2 * H / std::max(f.v0_cupola * 0.5, 5.0), 40, 0.7);
    }

    // pennacchi: colonne di particelle che salgono all'impulso della bolla e ricadono
    auto pennacchio = [&](double cx, double cy, double H, double Rb, double Rt, double inclinazione, double dir,
                          int n, bool cava, bool corona) {
        const double t0 = f.t_pennacchio;
        const double ta_cima = 0.8 * std::sqrt(2 * H / G);
        for (int i = 0; i < n; ++i) {
            const double u = caso_.uniforme();
            const double h = H * (0.15 + 0.85 * std::pow(u, 0.7));
            const double rho_h = Rb + (Rt - Rb) * std::pow(h / H, 1.5);
            const double a = caso_.uniforme(0, 2 * PI);
            const double rho = rho_h * (cava ? caso_.uniforme(0.7, 1.0) : std::sqrt(caso_.uniforme()));
            const double ox = rho * std::cos(a) + h * std::tan(inclinazione) * std::cos(dir);
            const double oy = rho * std::sin(a) + h * std::tan(inclinazione) * std::sin(dir);
            const bool marrone = f.fango && caso_.uniforme() < 0.5;
            auto& p = nuova(marrone ? Materia::fango : Materia::spruzzo);
            p.x = cx + 0.2 * ox; p.y = cy + 0.2 * oy;
            p.H = h;
            p.ta = 0.8 * std::sqrt(2 * h / G) + 0.05;
            p.vx = 0.8 * ox / p.ta; p.vy = 0.8 * oy / p.ta;
            p.v_limite = 25;
            // dimensioni tali che le particelle coprano la sagoma 2-3 volte, non di piu' (costo del disegno)
            p.r = 0.03 * H + 0.025 * Rt; p.dr = 0.04 * H / ta_cima;
            p.r_max = 0.06 * H + 0.05 * Rt;
            p.ritardo = t0 + caso_.uniforme(0, 0.25);
            p.vita = p.ta + std::sqrt(2 * h / G) + h / 25.0 + 0.5;
            p.opacita = 0.95;
            const double ta = p.ta;   // `p` non vale piu' dopo la prossima nuova()
            if (i % 6 == 0) {   // nebbia sospesa dopo la ricaduta
                auto& m = nuova(Materia::nebbia);
                m.x = cx + ox; m.y = cy + oy; m.z = h * caso_.uniforme(0.5, 1.0);
                m.r = 0.05 * H + 0.04 * Rt; m.dr = 0.4;
                m.ritardo = t0 + ta;
                m.vita = f.durata_nebbia * caso_.uniforme(0.7, 1.1);
                m.vz = -1.0;
                m.opacita = 0.5;
            }
        }
        if (corona)   // gas dello sfiato: nube grigio-nera in cima
            for (int i = 0; i < 30; ++i) {
                auto& p = nuova(Materia::corona);
                const double h = H * caso_.uniforme(0.7, 1.0), a = caso_.uniforme(0, 2 * PI), rho = Rt * 0.6 * std::sqrt(caso_.uniforme());
                p.x = cx + rho * std::cos(a) + h * std::tan(inclinazione) * std::cos(dir);
                p.y = cy + rho * std::sin(a) + h * std::tan(inclinazione) * std::sin(dir);
                // compare in cima alla colonna quando questa ha fatto il 30% della salita, e sale con lei
                const double ta = 0.8 * std::sqrt(2 * h / G) + 0.05;
                p.z = p.z0 = 0.51 * h;
                p.H = 0.49 * h; p.ta = 0.7 * ta;
                p.r = 0.06 * H + 0.1 * Rt; p.dr = 0.8;
                p.ritardo = t0 + 0.3 * ta;
                p.vita = p.ta + 0.6 * f.durata_nebbia;
                p.opacita = 0.9;
            }
    };
    const double H = f.H_pennacchio * kH, R = f.R_pennacchio * kR;
    if (H > 0) {
        const int n = std::clamp(static_cast<int>(60 + 1.2 * H), 60, 420);
        switch (f.regime) {
            case RegimeSubacqueo::sfiato: {   // colonna cava (Milligan e Young) con la corona di gas
                const double Dc = 3.71 * std::cbrt(W) * std::pow(std::max(d / std::cbrt(W), 0.08), 0.166);
                pennacchio(pos.x, pos.y, H, 0.5 * Dc, R, caso_.uniforme(0, radianti(6)), caso_.uniforme(0, 2 * PI), n, true, true);
                break;
            }
            case RegimeSubacqueo::getto:      // getto sottile e alto con la gonna di spruzzi
                pennacchio(pos.x, pos.y, H, 0.1 * R, 0.35 * R, caso_.uniforme(0, radianti(5)), caso_.uniforme(0, 2 * PI), n * 2 / 3, false, false);
                pennacchio(pos.x, pos.y, 0.35 * H, 0.4 * R, R, 0, 0, n / 2, false, false);
                break;
            case RegimeSubacqueo::pennacchi: {   // da 2 a 5 getti che bucano la cupola, inclinati fino a 15 gradi
                const int k = 2 + static_cast<int>(caso_.uniforme() * 4);
                for (int j = 0; j < k; ++j) {
                    const double a = caso_.uniforme(0, 2 * PI), off = 0.3 * R * caso_.uniforme();
                    const double hk = j == 0 ? H : H * caso_.uniforme(0.65, 1.0);
                    pennacchio(pos.x + off * std::cos(a), pos.y + off * std::sin(a), hk, 0.15 * R, R * caso_.uniforme(0.35, 0.6),
                               caso_.uniforme(0, radianti(15)), a, n / k + 20, false, false);
                }
                break;
            }
            default:   // cupola bassa o profonda: un solo pennacchio largo e basso
                pennacchio(pos.x, pos.y, H, 0.4 * R, R, caso_.uniforme(0, radianti(5)), caso_.uniforme(0, 2 * PI), n / 2, false, false);
                break;
        }
        if (f.R_base > 0)   // base surge: anello basso di nebbia che si allarga
            for (int i = 0; i < 50; ++i) {
                auto& p = nuova(Materia::nebbia);
                const double a = caso_.uniforme(0, 2 * PI), v = (f.R_base - 0.5 * R) / 8.0;
                p.x = pos.x + 0.5 * R * std::cos(a); p.y = pos.y + 0.5 * R * std::sin(a);
                p.z = caso_.uniforme(0, 0.08 * H);
                p.vx = v * std::cos(a); p.vy = v * std::sin(a);
                p.r = 0.05 * H + 3; p.dr = 1.0;
                p.ritardo = f.t_pennacchio + f.t_salita + 0.6 * std::sqrt(2 * H / G);
                p.vita = caso_.uniforme(12, 16);
                p.opacita = 0.55;
            }
    }
    {   // chiazza di schiuma (o di fango) e acqua "gassata" che ribolle
        auto& p = nuova(f.fango ? Materia::fango : Materia::schiuma);
        p.x = pos.x; p.y = pos.y; p.piatta = true;
        const double r0 = 0.5 * std::max({R, f.R_cupola, 2 * f.A_max});
        p.r = r0; p.r_max = std::max(f.R_chiazza * kR, r0); p.dr = (p.r_max - r0) / 10.0;
        p.ritardo = 0.8 * f.T1;
        p.vita = f.durata_chiazza;
        p.opacita = 0.85;
        Emettitore e;
        e.x = pos.x; e.y = pos.y; e.raggio = 0.7 * p.r_max; e.t0 = tempo_ + f.T1;
        e.durata = 0.5 * f.durata_chiazza; e.ritmo = std::clamp(3.0 * p.r_max / 30.0, 1.0, 12.0); e.fango = f.fango;
        emettitori_.push_back(e);
    }
}

void Effetti::esplosione_aerea(Vec2 pos, double z, double W, bool deposito) {
    const FenomenoAereo f = fenomeno_aereo(W, deposito);
    lampo_ = std::max(lampo_, f.lampo);
    const double D = f.D_palla;
    for (int i = 0; i < 25; ++i) {   // palla di fuoco
        auto& p = nuova(Materia::fuoco);
        const double a = caso_.uniforme(0, 2 * PI), r = 0.15 * D * std::sqrt(caso_.uniforme());
        p.x = pos.x + r * std::cos(a); p.y = pos.y + r * std::sin(a); p.z = p.z0 = z + caso_.uniforme(-0.1, 0.2) * D;
        p.H = 0.4 * D; p.ta = f.durata_palla;
        p.r = 0.2 * D; p.dr = 0.35 * D / f.durata_palla; p.r_max = 0.5 * D * caso_.uniforme(0.8, 1.1);
        p.vita = f.durata_palla * caso_.uniforme(0.8, 1.2);
        p.calore = 1.0;
        p.opacita = 1.0;
    }
    if (deposito)   // fiammata verticale
        for (int i = 0; i < 40; ++i) {
            auto& p = nuova(Materia::fuoco);
            p.x = pos.x + caso_.uniforme(-0.1, 0.1) * D; p.y = pos.y + caso_.uniforme(-0.1, 0.1) * D; p.z = p.z0 = z;
            p.H = f.H_nube * caso_.uniforme(0.1, 0.3); p.ta = 1.2;
            p.r = 0.04 * f.H_nube; p.dr = 0.02 * f.H_nube;
            p.vita = 1.6 * caso_.uniforme(0.8, 1.2);
            p.calore = 1.0; p.opacita = 1.0;
        }
    const int n_schegge = deposito ? 80 : 25;
    for (int i = 0; i < n_schegge; ++i) {   // schegge e rottami balistici
        auto& p = nuova(Materia::detrito);
        const double v = f.v_schegge * caso_.uniforme(0.3, 1.0);
        const double el = radianti(caso_.uniforme(10, 80)), az = caso_.uniforme(0, 2 * PI);
        p.x = pos.x; p.y = pos.y; p.z = z;
        p.vx = v * std::cos(el) * std::sin(az); p.vy = v * std::cos(el) * std::cos(az); p.vz = v * std::sin(el);
        p.v_limite = 60;
        p.r = caso_.uniforme(0.3, 0.8);
        p.vita = 40;
        p.opacita = 1.0;
        p.calore = deposito && caso_.uniforme() < 0.3 ? 0.8 : 0.0;   // frammenti in fiamme
    }
    const int n_fumo = deposito ? 120 : 30;
    for (int i = 0; i < n_fumo; ++i) {   // sbuffo del missile o fungo del deposito
        auto& p = nuova(Materia::fumo);
        const double q = deposito ? std::sqrt(caso_.uniforme(0.04, 1.0)) : caso_.uniforme(0.3, 1.0);
        const double h = f.H_nube * q;
        const double a = caso_.uniforme(0, 2 * PI);
        const double rho = (deposito ? 0.35 * f.H_nube * q * q * q : 0.2 * f.H_nube) * std::sqrt(caso_.uniforme());
        p.x = pos.x; p.y = pos.y; p.z = p.z0 = z;
        p.H = h;
        p.ta = 2 * h / (deposito ? 40.0 : 15.0);
        p.vx = rho * std::cos(a) / p.ta; p.vy = rho * std::sin(a) / p.ta;
        p.r = deposito ? 0.04 * f.H_nube + 0.08 * f.H_nube * q * q : 0.15 * f.H_nube; p.dr = deposito ? 0.5 : 0.15;
        p.r0 = p.r;
        p.ritardo = 0.5 * f.durata_palla;
        p.vita = deposito ? caso_.uniforme(600, 900) : caso_.uniforme(90, 150);
        p.tono = deposito ? caso_.uniforme(0.0, 0.35) : caso_.uniforme(0.1, 0.4);
        p.calore = 0.6;
        p.opacita = 0.9;
    }
}

// --- navi seguite -------------------------------------------------------------------------------------------------

void Effetti::affondamento(const Seguita& s, const Nave& n, const Corpo& c) {
    const double L = c.stazioni.back().x - c.stazioni.front().x;
    const Punto P = nel_mondo(n, c, c.assetto.x_o, 0, 0);
    Emettitore e;   // ribollio dell'aria che esce dallo scafo
    e.x = P.x; e.y = P.y; e.raggio = 0.3 * L; e.t0 = tempo_; e.durata = 180; e.ritmo = 6;
    emettitori_.push_back(e);
    spruzzi(P.x, P.y, 0, 25, 12, 0.2 * L);
    {   // vortice di schiuma che si allarga e si spegne
        auto& p = nuova(Materia::schiuma);
        p.x = P.x; p.y = P.y; p.piatta = true;
        p.r = 0.2 * L; p.dr = 0.5 * L / 60.0; p.r_max = 0.7 * L;
        p.vita = 240; p.opacita = 0.9;
    }
    // il carico liquido che bruciava (petroliere, rifornitrici: il fuoco nelle cisterne o la fuliggine che ha lasciato,
    // perche' l'acqua che sale le spegne prima che il troncone vada giu') resta a bruciare sulla chiazza per mezz'ora
    // (danni critici, fase 4; STIMA)
    bool brucia = false;
    if (!s.danni->in_zavorra()) for (const auto& ce : c.celle)
        brucia |= (ce.fuoco > 0.2 || ce.annerimento > 0.1) && (ce.dati.uso == "carico" || ce.dati.carburante.ends_with("_carico")) &&
                  (s.danni->dati().tipo == "petroliera" || ce.dati.carburante.ends_with("_carico"));
    if (brucia) {
        Emettitore f;
        f.x = P.x; f.y = P.y; f.raggio = 0.6 * L; f.t0 = tempo_; f.durata = 1800; f.ritmo = 12; f.fuoco = true;
        emettitori_.push_back(f);
    }
    for (int i = 0; i < 4; ++i) {   // chiazza di nafta: macchie scure e piatte che si allargano per ore
        auto& p = nuova(Materia::nafta);
        p.x = P.x + caso_.uniforme(-0.3, 0.3) * L; p.y = P.y + caso_.uniforme(-0.3, 0.3) * L; p.piatta = true;
        p.r = 0.2 * L; p.dr = 1.2 * L / 1800.0; p.r_max = caso_.uniforme(1.0, 2.0) * L;
        p.vx = 0.03 * vento_x_; p.vy = 0.03 * vento_y_;
        p.vita = 7200; p.opacita = 0.8;
    }
    const int n_detriti = static_cast<int>(L / 6.0);
    for (int i = 0; i < n_detriti; ++i) {   // rottami e zattere alla deriva
        auto& p = nuova(Materia::detrito);
        const double a = caso_.uniforme(0, 2 * PI), r = 0.4 * L * std::sqrt(caso_.uniforme());
        p.x = P.x + r * std::cos(a); p.y = P.y + r * std::sin(a);
        p.galleggia = true;
        p.vx = 0.03 * vento_x_ + caso_.uniforme(-0.2, 0.2); p.vy = 0.03 * vento_y_ + caso_.uniforme(-0.2, 0.2);
        p.r = caso_.uniforme(0.4, 1.2);
        p.tono = caso_.uniforme() < 0.2 ? 1.0 : 0.0;   // zattere chiare
        p.vita = 3600; p.opacita = 1.0;
    }
}

void Effetti::segui_nave(Seguita& s, const Nave& n, double dt) {
    const DanniNave& d = *s.danni;
    const auto& corpi = d.corpi();
    const double B = d.dati().larghezza;

    // alberi in caduta: asta rigida incernierata alla base, theta'' = 3 g / (2 h) sin(theta), fino a coricarsi
    for (auto& a : s.alberi) {
        if (!a.cade) continue;
        a.omega += 1.5 * G / a.altezza * std::sin(a.angolo) * dt;
        a.angolo += a.omega * dt;
        if (a.angolo < 1.45) continue;
        a.angolo = 1.45;
        a.cade = false;
        a.a_terra = true;
        // la cima finisce fuori bordo: spruzzo dove tocca l'acqua
        const double cx = a.px + a.altezza * a.ay, cy = a.py - a.altezza * a.ax;   // direzione (dx, dy) = (ay, -ax)
        if (std::fabs(cy) > B / 2 && !corpi.empty()) {
            const Corpo* c = &corpi.front();
            for (const auto& k : corpi) if (cx >= k.stazioni.front().x && cx <= k.stazioni.back().x) c = &k;
            const Punto P = nel_mondo(n, *c, cx, cy, 0);
            spruzzi(P.x, P.y, 0, 12, 6, 2.0);
        }
    }

    for (; s.scoppi < d.scoppi().size(); ++s.scoppi) {
        const Scoppio& sc = d.scoppi()[s.scoppi];
        const Punto P = nel_mondo(n, sc.x, sc.y, sc.z);
        // alberi: abbattuti da uno scoppio vicino (lontano dallo scoppio) o dalla frusta della bolla sotto la chiglia
        const bool aereo = sc.tipo == Esplosione::Tipo::missile || sc.tipo == Esplosione::Tipo::deposito || sc.z >= 0;
        for (auto& a : s.alberi) {
            if (aereo) {
                const double r = std::hypot(a.px - sc.x, a.py - sc.y);
                if (r < 6.0 + 2.5 * std::cbrt(sc.tnt_kg) && sc.z > a.pz - 12.0) abbatti(a, a.px - sc.x, a.py - sc.y);
            } else if (std::fabs(sc.y) < B / 2 && sc.z < -2.0 && caso_.uniforme() < 0.5) {
                abbatti(a, 0.0, caso_.uniforme() < 0.5 ? 1.0 : -1.0);
            }
        }
        if (sc.tipo == Esplosione::Tipo::missile) esplosione_aerea({P.x, P.y}, std::max(P.z, 1.0), sc.tnt_kg, false);
        else if (sc.tipo == Esplosione::Tipo::deposito) esplosione_aerea({P.x, P.y}, std::max(P.z, 2.0), sc.tnt_kg, true);
        else if (P.z < 0) esplosione_subacquea({P.x, P.y}, -P.z, sc.tnt_kg);
        else esplosione_aerea({P.x, P.y}, P.z, sc.tnt_kg, false);
    }
    if (corpi.size() != s.corpi) {   // rottura: spruzzi e schiuma dove lo scafo si apre, alberi vicini abbattuti
        if (corpi.size() > s.corpi && !corpi.empty()) {
            const auto& c = corpi.front();
            const double xr = c.stazioni.back().x;
            const Punto P = nel_mondo(n, c, xr, 0, 0);
            spruzzi(P.x, P.y, 0, 40, 15, 0.6 * B);
            bolle(P.x, P.y, B, 20, false, 2.0);
            for (auto& a : s.alberi)
                if (std::fabs(a.px - xr) < 0.15 * d.dati().lunghezza) abbatti(a, 0.0, caso_.uniforme() < 0.5 ? 1.0 : -1.0);
        }
        s.corpi = corpi.size();
        s.affondati.clear();
        for (const auto& c : corpi) s.affondati.push_back(c.affondato);
    }
    for (std::size_t i = 0; i < corpi.size(); ++i)
        if (corpi[i].affondato && !s.affondati[i]) {
            s.affondati[i] = true;
            affondamento(s, n, corpi[i]);
        }

    auto emetti = [&](const std::string& chiave, double ritmo) {
        double& a = s.accumulo[chiave];
        a += ritmo * dt;
        const int k = static_cast<int>(a);
        a -= k;
        return k;
    };

    for (std::size_t i = 0; i < corpi.size(); ++i) {
        const Corpo& c = corpi[i];
        if (c.affondato) continue;
        const std::string pre = std::to_string(i) + ":";
        // orlo di schiuma sul galleggiamento e spruzzi dove lo scafo si muove in fretta
        const std::size_t ns = c.stazioni.size(), passo = std::max<std::size_t>(1, ns / 30);
        for (std::size_t k = 0; k < ns; k += passo) {
            const auto& sez = c.sezioni[k];
            if (sez.size() < 3) continue;
            const double x = c.stazioni[k].x;
            double a, b, cc;
            c.assetto.coefficienti(x, a, b, cc);
            const double vel = std::fabs(c.v_imm) + std::fabs(c.v_sb) * B / 2 + std::fabs(c.v_ap) * std::fabs(x - c.assetto.x_o);
            for (std::size_t e = 0; e < sez.size(); ++e) {
                const auto& u = sez[e];
                const auto& v = sez[(e + 1) % sez.size()];
                const double qu = a + b * u.y + cc * u.z, qv = a + b * v.y + cc * v.z;
                if ((qu < 0) == (qv < 0)) continue;
                const double t = qu / (qu - qv);
                const Punto P = nel_mondo(n, c, x, u.y + (v.y - u.y) * t, u.z + (v.z - u.z) * t);
                Particella o;
                o.materia = Materia::schiuma;
                o.x = P.x; o.y = P.y; o.z = 0.3;
                o.r = 0.8 + 0.4 * vel;
                o.opacita = std::clamp(0.3 + 0.5 * vel, 0.0, 1.0);
                o.eta = 0.5; o.vita = 1.0;
                o.seme = static_cast<std::uint32_t>(k * 31 + e);
                orli_.push_back(o);
                if (vel > 1.2 && caso_.uniforme() < (vel - 1.2) * 2.0 * dt) spruzzi(P.x, P.y, 0, 3, std::min(20.0, 2.0 * vel), 2.0);
            }
        }
        // celle: aria che esce sopra le falle, fiamme, fumo e vapore
        double Q_tot = 0, qx = 0, qy = 0, qz = 0, tono = 0, span = 0;
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            const Cella& ce = c.celle[j];
            const std::string chiave = pre + ce.dati.nome;
            double& acqua_prec = s.acqua[chiave];
            const double portata = dt > 0 ? (ce.acqua - acqua_prec) / dt : 0.0;
            acqua_prec = ce.acqua;
            if (portata > 0.3 && ce.riempimento() < 0.98)
                for (const auto& fa : c.falle) {
                    if (fa.cella != static_cast<int>(j)) continue;
                    const Punto P = nel_mondo(n, c, fa.x, fa.y, fa.z);
                    if (P.z > 0.5) break;
                    const int k = emetti(chiave + "/aria", std::min(portata * 0.08, 12.0));
                    if (k > 0) bolle(P.x, P.y, 1.0 + std::sqrt(fa.area), k, false, std::min(3.0, 0.3 * std::sqrt(portata)));
                    break;
                }
            // il carico che esce dalle falle di una cisterna in fiamme brucia sul mare attorno alla falla, mentre la nave
            // galleggia (petroliere e rifornitrici; danni critici, fase 4, STIMA: una chiazza di 20-40 m)
            if (ce.fuoco > 0.3 && !d.in_zavorra() && (ce.dati.carburante.ends_with("_carico") || (ce.dati.uso == "carico" && d.dati().tipo == "petroliera")))
                for (const auto& fa : c.falle) {
                    if (fa.cella != static_cast<int>(j) || fa.coperta) continue;
                    const Punto P = nel_mondo(n, c, fa.x, fa.y, fa.z);
                    if (P.z > 0.5) continue;   // la falla sopra l'acqua non perde a mare
                    const double R = std::min(20.0 + 4.0 * std::sqrt(fa.area), 40.0);
                    const int k = emetti(chiave + "/chiazza", 6.0 * ce.fuoco);
                    for (int q = 0; q < k; ++q) {
                        const double a = caso_.uniforme(0, 2 * PI), r = R * std::sqrt(caso_.uniforme());
                        auto& p = nuova(Materia::fuoco);
                        p.x = P.x + r * std::cos(a); p.y = P.y + r * std::sin(a); p.z = p.z0 = 0.3;
                        p.H = caso_.uniforme(3.0, 8.0); p.ta = 0.4 + 0.04 * p.H; p.vita = 1.1 * p.ta;
                        p.r = 0.2 * p.H + 0.8; p.dr = 0.6; p.calore = 1.0; p.opacita = 1.0;
                    }
                    break;
                }
            double& fuoco_prec = s.fuoco[chiave];
            const auto& sx = c.stazioni;
            const double x0 = std::max(ce.dati.x0, sx.front().x), x1 = std::min(ce.dati.x1, sx.back().x);
            if (x1 <= x0) { fuoco_prec = ce.fuoco; continue; }
            const double xc = (x0 + x1) / 2;
            std::size_t kk = 0;
            for (std::size_t q = 0; q < sx.size(); ++q) if (std::fabs(sx[q].x - xc) < std::fabs(sx[kk].x - xc)) kk = q;
            const double yc = std::clamp((std::max(ce.dati.y0, -B / 2) + std::min(ce.dati.y1, B / 2)) / 2, -0.4 * B, 0.4 * B);
            const Punto P = nel_mondo(n, c, xc, yc, sx[kk].coperta());
            const double area = (x1 - x0) * std::max(std::min(ce.dati.y1, B / 2) - std::max(ce.dati.y0, -B / 2), 1.0);
            if (ce.fuoco > 0.02 && P.z > 0.3) {
                const double Q = ce.fuoco * 5e5 * area;   // incendio limitato dalla ventilazione: ~0,5 MW/m^2
                const double L = std::max(altezza_fiamma(0.5 * Q, std::sqrt(4 * 0.1 * area / PI)), 1.0);
                const int k = emetti(chiave + "/fiamme", 8.0 * std::sqrt(ce.fuoco));
                for (int q = 0; q < k; ++q) {
                    auto& p = nuova(Materia::fuoco);
                    const double lungo = caso_.uniforme(-0.4, 0.4) * (x1 - x0);
                    const double r = radianti(n.rotta);
                    p.x = P.x + lungo * std::sin(r) + caso_.uniforme(-1.5, 1.5);
                    p.y = P.y + lungo * std::cos(r) + caso_.uniforme(-1.5, 1.5);
                    p.z = p.z0 = P.z;
                    p.H = L * caso_.uniforme(0.5, 1.1);
                    p.ta = 0.4 + 0.04 * L;
                    p.vita = 1.1 * p.ta;
                    p.r = 0.2 * L + 0.8; p.dr = 0.6;
                    p.calore = 1.0; p.opacita = 1.0;
                }
                Q_tot += Q; qx += Q * P.x; qy += Q * P.y; qz += Q * (P.z + 0.7 * L);
                tono += Q * tono_fumo(ce.dati.uso, "");
                span = std::max(span, x1 - x0);
                if (ce.riempimento() > 0.15) {   // acqua sul fuoco: vapore
                    const int kv = emetti(chiave + "/vapore", 2.0 * ce.fuoco);
                    for (int q = 0; q < kv; ++q) {
                        auto& p = nuova(Materia::vapore);
                        p.x = P.x + caso_.uniforme(-3, 3); p.y = P.y + caso_.uniforme(-3, 3); p.z = P.z;
                        p.vz = 4.0; p.r = 2.0; p.dr = 1.0; p.vita = caso_.uniforme(8, 14); p.opacita = 0.7;
                    }
                }
            } else if (fuoco_prec > 0.15 && ce.fuoco <= 0.02) {   // incendio spento: sbuffo di vapore
                for (int q = 0; q < 10; ++q) {
                    auto& p = nuova(Materia::vapore);
                    p.x = P.x + caso_.uniforme(-4, 4); p.y = P.y + caso_.uniforme(-4, 4); p.z = std::max(P.z, 0.5);
                    p.vz = caso_.uniforme(3, 6); p.r = 2.5; p.dr = 1.2; p.vita = caso_.uniforme(10, 20); p.opacita = 0.75;
                }
            }
            fuoco_prec = ce.fuoco;
        }
        if (i == 0 || corpi.size() == 1)   // zone sopra coperta (hangar, lanciatori): sul corpo principale
            for (const auto& z : d.zone()) {
                if (z.fuoco <= 0.02) continue;
                const double xc = std::clamp((z.dati.x0 + z.dati.x1) / 2, c.stazioni.front().x, c.stazioni.back().x);
                std::size_t kk = 0;
                for (std::size_t q = 0; q < c.stazioni.size(); ++q)
                    if (std::fabs(c.stazioni[q].x - xc) < std::fabs(c.stazioni[kk].x - xc)) kk = q;
                const Punto P = nel_mondo(n, c, xc, 0, c.stazioni[kk].coperta() + 2.0);
                if (P.z < 0.3) continue;
                const double area = (z.dati.x1 - z.dati.x0) * 0.6 * B, Q = z.fuoco * 1e6 * area;
                const double L = std::max(altezza_fiamma(0.5 * Q, std::sqrt(4 * 0.1 * area / PI)), 1.0);
                const int k = emetti(pre + z.dati.nome + "/fiamme", 8.0 * std::sqrt(z.fuoco));
                for (int q = 0; q < k; ++q) {
                    auto& p = nuova(Materia::fuoco);
                    p.x = P.x + caso_.uniforme(-3, 3); p.y = P.y + caso_.uniforme(-3, 3); p.z = p.z0 = P.z;
                    p.H = L * caso_.uniforme(0.5, 1.1); p.ta = 0.4 + 0.04 * L; p.vita = 1.1 * p.ta;
                    p.r = 0.2 * L + 0.8; p.dr = 0.6; p.calore = 1.0; p.opacita = 1.0;
                }
                Q_tot += Q; qx += Q * P.x; qy += Q * P.y; qz += Q * (P.z + 0.7 * L);
                tono += Q * tono_fumo("", z.dati.carico);
                span = std::max(span, z.dati.x1 - z.dati.x0);
            }
        // una colonna di fumo per corpo, dal baricentro dei focolai, con il calore di tutti
        if (Q_tot > 0) {
            const int k = emetti(pre + "fumo", 1.0);
            for (int q = 0; q < k; ++q) {
                auto& p = nuova(Materia::fumo);
                p.x = qx / Q_tot + caso_.uniforme(-0.5, 0.5) * span;
                p.y = qy / Q_tot + caso_.uniforme(-0.5, 0.5) * span;
                p.z = p.z0 = qz / Q_tot;
                p.F = flusso_galleggiamento(Q_tot);
                p.r = p.r0 = std::max(3.0, 0.25 * std::sqrt(span * B));
                p.dr = 0.3;
                p.vx = caso_.uniforme(-0.5, 0.5); p.vy = caso_.uniforme(-0.5, 0.5);
                p.vita = caso_.uniforme(600, 800);
                p.opacita = 0.55 + 0.35 * std::min(1.0, Q_tot / 2e8);
                p.tono = std::clamp(tono / Q_tot + caso_.uniforme(-0.05, 0.1), 0.0, 1.0);
                p.calore = std::min(1.0, Q_tot / 5e7);
            }
        }
    }
}

// --- passo ------------------------------------------------------------------------------------------------------

void Effetti::avanza(double dt, const Mondo& mondo) {
    tempo_ += dt;
    avanza_artiglieria(mondo);
    lampo_ = std::max(0.0, lampo_ - dt / 0.08);   // un fotogramma o due
    orli_.clear();
    for (auto& s : seguite_)
        for (const auto& n : mondo.navi())
            if (n.id == s.id) { segui_nave(s, n, dt); break; }
    for (auto& e : emettitori_) {
        const double t = tempo_ - e.t0;
        if (t < 0 || t > e.durata) continue;
        e.accumulo += e.ritmo * std::exp(-2.0 * t / e.durata) * dt;   // il ribollio si spegne
        const int k = static_cast<int>(e.accumulo);
        e.accumulo -= k;
        if (e.fuoco) {   // fiamme basse sparse sulla chiazza, una su tre col fumo nero
            for (int q = 0; q < k; ++q) {
                const double a = caso_.uniforme(0, 2 * PI), r = e.raggio * std::sqrt(caso_.uniforme());
                auto& p = nuova(Materia::fuoco);
                p.x = e.x + r * std::cos(a); p.y = e.y + r * std::sin(a); p.z = p.z0 = 0.3;
                p.H = caso_.uniforme(3.0, 9.0); p.ta = 0.4 + 0.04 * p.H; p.vita = 1.1 * p.ta;
                p.r = 0.2 * p.H + 0.8; p.dr = 0.6; p.calore = 1.0; p.opacita = 1.0;
                if (q % 3 == 0) {
                    auto& f = nuova(Materia::fumo);
                    f.x = p.x; f.y = p.y; f.z = p.H; f.vz = 2.5;
                    f.tono = 0.05; f.r = 4.0; f.dr = 1.5; f.r_max = 30.0; f.vita = caso_.uniforme(30.0, 50.0); f.opacita = 0.8;
                }
            }
        } else if (k > 0) {
            bolle(e.x, e.y, e.raggio, k, e.fango, 1.5);
        }
    }
    emettitori_.erase(std::remove_if(emettitori_.begin(), emettitori_.end(),
                                     [this](const Emettitore& e) { return tempo_ - e.t0 > e.durata; }), emettitori_.end());

    const std::size_t prima = particelle_.size();   // le particelle nate durante il passo partono dal prossimo
    std::vector<Particella> cadute;                  // frammenti finiti in mare: spruzzi dopo il ciclo
    for (std::size_t i = 0; i < prima; ++i) {
        Particella& p = particelle_[i];
        if (p.ritardo > 0) { p.ritardo -= dt; continue; }
        p.eta += dt;
        if (p.materia == Materia::anello_scuro) {   // r = sqrt((c t)^2 - d^2), con t dall'esplosione
            p.r = std::min(p.r_max, std::sqrt(std::pow(p.H + 1500.0 * p.eta, 2) - p.H * p.H));
            continue;
        }
        p.r = std::min(p.r_max, p.r + p.dr * dt);
        if (p.piatta) { p.x += p.vx * dt; p.y += p.vy * dt; continue; }
        if (p.propulsa) { p.x += p.vx * dt; p.y += p.vy * dt; p.z += p.vz * dt; continue; }
        if (p.F > 0) {   // fumo di un incendio: pennacchio di Briggs piegato dal vento
            p.x += (vento_x_ + p.vx) * dt; p.y += (vento_y_ + p.vy) * dt;
            p.percorso += std::max(vento_, 1.5) * dt;
            const double h = risalita_fumo(p.F, vento_, p.percorso);
            p.z = p.z0 + h;
            p.r = std::max(p.r, p.r0 + 0.45 * h);
            continue;
        }
        if (p.ta > 0 && p.eta < p.ta) {   // salita prescritta
            const double s = p.eta / p.ta;
            p.z = p.z0 + p.H * (1 - (1 - s) * (1 - s));
            p.vz = 2 * p.H / p.ta * (1 - s);
            p.x += p.vx * dt; p.y += p.vy * dt;
            continue;
        }
        if (p.galleggia && p.z <= 0) {   // rottami alla deriva
            p.z = 0;
            p.x += p.vx * dt; p.y += p.vy * dt;
            continue;
        }
        if (leggera(p.materia)) {   // sospesa: va col vento e si assesta
            const double k = std::min(1.0, 0.4 * dt);
            p.vx += (vento_x_ - p.vx) * k; p.vy += (vento_y_ - p.vy) * k;
            p.vz += (verticale_equilibrio(p.materia) - p.vz) * k;
        } else if (p.materia != Materia::fuoco) {   // balistica con velocita' limite
            p.vz = std::max(p.vz - G * dt, -p.v_limite);
            if (p.materia != Materia::detrito) {
                const double k = std::min(1.0, 0.3 * dt);
                p.vx += (vento_x_ - p.vx) * k; p.vy += (vento_y_ - p.vy) * k;
            }
        }
        p.x += p.vx * dt; p.y += p.vy * dt; p.z += p.vz * dt;
        if (p.z < 0) {
            if (p.materia == Materia::detrito) {   // un frammento cade in mare: piccolo spruzzo
                if (p.galleggia) { p.z = 0; continue; }
                cadute.push_back(p);
                p.eta = p.vita;
            } else if (leggera(p.materia)) {
                p.z = 0; p.vz = 0;
            } else {
                p.eta = p.vita;   // l'acqua ricade in mare
            }
        }
    }
    for (const auto& p : cadute) spruzzi(p.x, p.y, 0, 2, caso_.uniforme(2, 6), 1.0);
    avanza_asw(dt);
    particelle_.erase(std::remove_if(particelle_.begin(), particelle_.end(), [](const Particella& p) { return p.eta >= p.vita; }),
                      particelle_.end());
}

// --- ASW aereo (docs/13 A4; dossier 17 §4.3, §8.1) ------------------------------------------------------------

void Effetti::aerei_asw(const std::vector<AereoAsw>& aerei) { aerei_ = aerei; }

void Effetti::boe_asw(const std::vector<Sonoboa>& boe, double t_missione) {
    boe_ = boe;
    t_boe_ = t_missione;
}

void Effetti::marcatore_asw(MarcatoreAsw tipo, Vec2 pos, double durata_s) {
    MarcatoreAcceso m;
    m.tipo = tipo;
    m.x = pos.x; m.y = pos.y;
    m.t0 = tempo_;
    m.durata = std::max(durata_s, 1.0);
    marcatori_.push_back(m);
    if (tipo == MarcatoreAsw::omab_giorno) {   // la fluoresceina si allarga e resta finche' il marcatore dura
        auto& c = nuova(Materia::colorante);
        c.x = pos.x; c.y = pos.y; c.piatta = true;
        c.r = 2.0; c.dr = 0.08; c.r_max = 25.0;
        c.vita = m.durata; c.opacita = 0.9;
    }
    spruzzi(pos.x, pos.y, 0, 3, 1.5, 0.5);   // il tonfo
}

double Effetti::arma_paracadute(Vec2 pos, double quota_m, double rotta, double velocita, bool sovietica) {
    // discesa col paracadute: Mk 46 ~15 m/s, AT-1M e UMGT-1 col paracadute piu' grande ~10 m/s (STIMA)
    const double vd = sovietica ? 10.0 : 15.0, h = std::max(quota_m, 5.0), t = h / vd;
    const double r = radianti(rotta), ax = std::sin(r), ay = std::cos(r);
    const double vh = 0.12 * std::max(velocita, 0.0);   // il paracadute frena subito: resta poca velocita' in avanti
    auto cade = [&](Materia m, double z, double raggio, double tono) {
        auto& p = nuova(m);
        p.x = pos.x; p.y = pos.y; p.z = z;
        p.vx = vh * ax; p.vy = vh * ay; p.vz = -vd;
        p.propulsa = true;
        p.r = raggio; p.tono = tono;
        p.vita = (z - 0.2) / vd; p.opacita = 1.0;
    };
    cade(Materia::detrito, h, 0.25, 0.15);                                   // il siluro
    cade(Materia::detrito, h + (sovietica ? 4.0 : 3.0), sovietica ? 1.8 : 0.9, 0.95);   // la calotta
    const double xi = pos.x + vh * ax * t, yi = pos.y + vh * ay * t;
    const std::size_t prima = particelle_.size();
    spruzzi(xi, yi, 0, 10, 4.0, 1.5);   // tonfo
    auto& f = nuova(Materia::schiuma);
    f.x = xi; f.y = yi; f.piatta = true;
    f.r = 1.5; f.dr = 0.3; f.r_max = 5.0; f.vita = 25.0; f.opacita = 0.8;
    auto& c = nuova(Materia::schiuma);   // il paracadute sull'acqua
    c.x = xi + 2.0 * ax; c.y = yi + 2.0 * ay; c.piatta = true;
    c.r = sovietica ? 1.8 : 1.0; c.vita = 120.0; c.opacita = 0.9;
    for (std::size_t i = prima; i < particelle_.size(); ++i) particelle_[i].ritardo = t;
    return t;
}

Particella& Effetti::appeso(Materia m, double x, double y, double z, double r) {
    orli_.emplace_back();
    auto& p = orli_.back();
    p.materia = m;
    p.x = x; p.y = y; p.z = z; p.r = r;
    p.eta = 0.5; p.vita = 1.0;   // opacita' piena: rifatta a ogni passo
    p.seme = static_cast<std::uint32_t>(orli_.size() * 2654435761u);
    return p;
}

void Effetti::avanza_asw(double dt) {
    for (const auto& a : aerei_) {
        const double r = radianti(a.rotta), ax = std::sin(r), ay = std::cos(r);
        // luci: lampeggiante rosso (~1 lampo al secondo), rossa a sinistra, verde a dritta, bianca in coda
        if (a.luci) {
            const double fase = std::fmod(tempo_ + 0.37 * a.id, 1.1);
            if (fase < 0.12) appeso(Materia::luce, a.pos.x, a.pos.y, a.quota_m + 1.0, 0.2).tono = 0.0;
            const double m = 0.5 * a.apertura_m;
            appeso(Materia::luce, a.pos.x - m * ay, a.pos.y + m * ax, a.quota_m, 0.15).tono = 0.0;
            appeso(Materia::luce, a.pos.x + m * ay, a.pos.y - m * ax, a.quota_m, 0.15).tono = 0.5;
            appeso(Materia::luce, a.pos.x - 0.5 * a.apertura_m * ax, a.pos.y - 0.5 * a.apertura_m * ay, a.quota_m, 0.15).tono = 1.0;
        }
        // anello di spruzzi del flusso del rotore: forte a 12 m sotto il disco grande del Sea King, piu' debole a 30 m
        // sotto il Ka-27 (STIMA dal rapporto fra quota e diametro)
        if (a.elicottero && a.hover && a.rotore_m > 0) {
            const double forza = std::clamp(1.4 - a.quota_m / (2.2 * a.rotore_m), 0.0, 1.0);
            const double anello = 0.55 * a.rotore_m + 0.2 * a.quota_m;
            // col vento l'anello si sposta sottovento (fino a un quarto del raggio a 15 m/s, STIMA)
            const double sotto = radianti(a.vento_da_gradi + 180.0), spinta = 0.25 * anello * std::clamp(a.vento_m_s / 15.0, 0.0, 1.0);
            const double cx = a.pos.x + spinta * std::sin(sotto), cy = a.pos.y + spinta * std::cos(sotto);
            double& acc = spruzzi_acc_[a.id];
            acc += 24.0 * forza * dt;
            const int n = static_cast<int>(acc);
            acc -= n;
            for (int i = 0; i < n; ++i) {
                const double ang = caso_.uniforme(0, 2 * PI), c = std::cos(ang), sn = std::sin(ang);
                const double rr = anello * caso_.uniforme(0.85, 1.1);
                auto& p = nuova(Materia::spruzzo);
                p.x = cx + rr * c; p.y = cy + rr * sn;
                p.H = (0.5 + 1.8 * forza) * caso_.uniforme(0.5, 1.0);
                p.ta = 0.5;
                const double v = caso_.uniforme(6.0, 11.0) * (0.5 + 0.5 * forza);
                p.vx = v * c; p.vy = v * sn;
                p.v_limite = 3; p.r = caso_.uniforme(0.35, 0.7); p.dr = 0.6;
                p.vita = p.ta + 1.4; p.opacita = 0.45;
                if (i % 4 == 0) {   // e la schiuma che l'aria spinge in fuori
                    auto& f = nuova(Materia::schiuma);
                    f.x = p.x; f.y = p.y; f.piatta = true;
                    f.vx = 0.5 * p.vx; f.vy = 0.5 * p.vy;
                    f.r = 1.2; f.dr = 0.4; f.r_max = 3.0; f.vita = 3.5; f.opacita = 0.5;
                }
            }
        }
        // cavo e trasduttore: il filo scende dalla pancia; il trasduttore e' appeso finche' non tocca l'acqua
        const double pancia = std::max(a.quota_m - 1.0, 0.0);
        const double prima = cavo_prec_.count(a.id) ? cavo_prec_[a.id] : a.cavo_m;
        // col vento il cavo pende sottovento di 5-15 gradi (nota D §B6.4, STIMA): spezzato in tratti verticali
        const double inclina = a.vento_m_s > 0.5 ? std::tan(radianti(std::clamp(0.8 * a.vento_m_s, 5.0, 15.0))) : 0.0;
        const double sotto_x = std::sin(radianti(a.vento_da_gradi + 180.0)), sotto_y = std::cos(radianti(a.vento_da_gradi + 180.0));
        const double filato = std::min(a.cavo_m, pancia);
        if (a.cavo_m > 0.05 && inclina == 0.0) {
            auto& c = appeso(Materia::cavo, a.pos.x, a.pos.y, pancia, 0.02);
            c.H = filato;
        } else if (a.cavo_m > 0.05) {
            constexpr int TRATTI = 6;
            for (int k = 0; k < TRATTI; ++k) {
                const double z = pancia - filato * k / TRATTI, off = inclina * filato * (k + 0.5) / TRATTI;
                appeso(Materia::cavo, a.pos.x + off * sotto_x, a.pos.y + off * sotto_y, z, 0.02).H = filato / TRATTI;
            }
        }
        if (a.cavo_m > 0.05 && a.cavo_m < pancia)
            appeso(Materia::detrito, a.pos.x + inclina * filato * sotto_x, a.pos.y + inclina * filato * sotto_y, pancia - a.cavo_m, 0.3).tono = 0.1;
        if (prima <= pancia && a.cavo_m > pancia) {   // il trasduttore entra in acqua: tonfo
            spruzzi(a.pos.x, a.pos.y, 0, 6, 2.0, 0.8);
        } else if (prima > pancia && a.cavo_m <= pancia && a.cavo_m > 0.05) {   // esce: acqua che cola e schiuma
            spruzzi(a.pos.x, a.pos.y, 0.5, 4, 0.8, 0.4);
            bolle(a.pos.x, a.pos.y, 1.0, 2, false, 0.6);
        }
        cavo_prec_[a.id] = a.cavo_m;
        // MAD rimorchiato: il «bird» pende sotto l'elicottero fermo e resta indietro in volo
        if (a.mad_m > 0) {
            const double th = std::atan(std::max(a.velocita, 0.0) / 15.0);
            const double bx = a.pos.x - a.mad_m * std::sin(th) * ax, by = a.pos.y - a.mad_m * std::sin(th) * ay;
            const double bz = std::max(3.0, pancia - a.mad_m * std::cos(th));
            for (int k = 1; k < 10; ++k) {
                const double f = k / 10.0;
                appeso(Materia::detrito, a.pos.x + (bx - a.pos.x) * f, a.pos.y + (by - a.pos.y) * f, pancia + (bz - pancia) * f, 0.12).tono = 0.1;
            }
            appeso(Materia::detrito, bx, by, bz, 0.5).tono = 0.8;
        }
    }
    // boe: espulsione, discesa, tonfo, colorante, galleggiante con l'antenna
    const double t = t_boe_;
    for (const auto& b : boe_) {
        const bool usa = b.tipo.rfind("ssq", 0) == 0;
        const double discesa = std::max(b.t_impatto - b.t_posa, 0.0);
        const double h0 = 30.0 * discesa;   // DatiBoa::caduta_m_s
        if (t >= b.t_posa && t < b.t_posa + 3.0 && boe_espulse_.insert(b.id).second) {   // cartuccia d'espulsione
            auto& p = nuova(Materia::fumo);
            p.x = b.pos.x; p.y = b.pos.y; p.z = p.z0 = h0;
            p.r = 0.4; p.dr = 0.8; p.r_max = 3.0; p.tono = 0.9; p.vita = 4.0; p.opacita = 0.6;
        }
        if (t >= b.t_posa && t < b.t_impatto && discesa > 0) {   // col paracadute
            const double z = h0 * (b.t_impatto - t) / discesa;
            appeso(Materia::detrito, b.pos.x, b.pos.y, z, 0.12).tono = 0.15;
            appeso(Materia::detrito, b.pos.x, b.pos.y, z + 1.2, 0.35).tono = 0.95;
        }
        if (t >= b.t_impatto && boe_cadute_.insert(b.id).second && t - b.t_impatto < 900.0) {
            if (t - b.t_impatto < 5.0) spruzzi(b.pos.x, b.pos.y, 0, 5, 2.5, 0.8);   // tonfo (solo se e' adesso)
            if (usa) {   // colorante delle boe USA: si allarga per un quarto d'ora
                auto& c = nuova(Materia::colorante);
                c.x = b.pos.x; c.y = b.pos.y; c.piatta = true;
                c.r = 1.5; c.dr = 0.05; c.r_max = 12.0;
                c.eta = t - b.t_impatto; c.r = std::min(c.r_max, c.r + c.dr * c.eta);
                c.vx = b.deriva.x; c.vy = b.deriva.y;
                c.vita = 900.0; c.opacita = 0.85;
            }
        }
        if (t >= b.t_impatto && t < b.t_fine) {   // galleggiante con l'antenna di 1-1,5 m
            const Vec2 q = posizione_boa(b, t);
            appeso(Materia::detrito, q.x, q.y, 0.15, 0.2).tono = 0.75;
            appeso(Materia::cavo, q.x, q.y, 1.4, 0.01).H = 1.2;
        }
    }
    // marcatori: fiamme rifatte a ogni passo, fumo che sale e va col vento
    for (auto& m : marcatori_) {
        const double e = tempo_ - m.t0;
        if (e > m.durata) continue;
        const double fine = std::clamp((m.durata - e) / 30.0, 0.0, 1.0);   // si spegne negli ultimi 30 s
        auto fiamma = [&](double dx, double calore) {
            auto& f = appeso(Materia::fuoco, m.x + dx, m.y, 0.35, 0.35 * fine + 0.05);
            f.calore = calore;
            f.seme = static_cast<std::uint32_t>(m.t0 * 1000.0 + dx * 10.0);
        };
        int fumi = 0;
        switch (m.tipo) {
            case MarcatoreAsw::mk25: fiamma(0.0, 0.8); fumi = 1; break;
            case MarcatoreAsw::mk58: fiamma(-0.6, 0.85); fiamma(0.6, 0.85); fumi = 2; break;
            case MarcatoreAsw::omab_notte: fiamma(0.0, 1.0); break;
            case MarcatoreAsw::omab_giorno: break;
        }
        m.accumulo += fumi * fine * dt;
        const int k = static_cast<int>(m.accumulo);
        m.accumulo -= k;
        for (int i = 0; i < k; ++i) {
            auto& p = nuova(Materia::fumo);
            p.x = m.x + caso_.uniforme(-0.5, 0.5); p.y = m.y + caso_.uniforme(-0.5, 0.5); p.z = p.z0 = 1.2;
            p.calore = 0.8;   // di notte la fiamma lo illumina dal basso
            p.H = caso_.uniforme(2.5, 5.0); p.ta = 3.0;   // un pennacchio bianco basso che va col vento
            p.r = 0.5; p.dr = 0.25; p.r_max = 3.5;
            p.tono = 0.95; p.vita = 22.0; p.opacita = 0.6;
        }
    }
    marcatori_.erase(std::remove_if(marcatori_.begin(), marcatori_.end(), [this](const MarcatoreAcceso& m) { return tempo_ - m.t0 > m.durata; }),
                     marcatori_.end());
}

// --- disegno ----------------------------------------------------------------------------------------------------

void Effetti::disegna(Tela& t, const Particella& p, double x, double y, double px_m, int limite, double luce,
                      double quota_occhio, double distanza, double foschia) const {
    if (p.ritardo > 0) return;
    // trasparenza: comparsa rapida, dissolvenza nell'ultimo 40% della vita; il fumo si diluisce allargandosi
    double a = p.opacita * std::min(1.0, p.eta / (p.materia == Materia::fumo ? 1.0 : 0.08) + 0.05);
    a *= std::clamp((p.vita - p.eta) / (0.4 * p.vita), 0.0, 1.0);
    if (p.materia == Materia::fumo && p.r0 > 0) a *= std::min(1.0, std::pow(p.r0 / p.r, 0.35));
    a *= 1.0 - foschia;   // la foschia toglie contrasto: resta lo sfondo, che all'orizzonte e' gia' del suo colore
    if (a < 0.03) return;
    const int ox = static_cast<int>(p.seme & 3u), oy = static_cast<int>((p.seme >> 2) & 3u);   // fase della retinatura

    if (p.piatta) {   // sul mare: ellisse schiacciata dalla quota dell'occhio (di solito una riga)
        const double w = p.r * px_m, hh = w * quota_occhio / std::max(distanza, 1.0);
        const bool sottile = hh < 1.0;   // di solito: una sola riga, quella del pelo dell'acqua
        const int y0 = sottile ? static_cast<int>(std::floor(y)) : static_cast<int>(std::floor(y - hh));
        const int y1 = sottile ? y0 : static_cast<int>(std::ceil(y + hh));
        const int x0 = static_cast<int>(std::floor(x - w)), x1 = static_cast<int>(std::ceil(x + w));
        for (int py = y0; py <= y1; ++py) {
            if (py >= limite || py < 0 || py >= t.altezza()) continue;
            const double fy = sottile ? 0.0 : (py + 0.5 - y) / hh;
            for (int px = x0; px <= x1; ++px) {
                if (px < 0 || px >= t.larghezza()) continue;
                const double fx = (px + 0.5 - x) / std::max(w, 0.5);
                const double d2 = fx * fx + fy * fy;
                if (d2 > 1.0) continue;
                double aa = a * std::min(1.0, 2.5 * (1 - d2));
                if (p.materia == Materia::anello_scuro && !sottile) aa *= std::min(1.0, 4.0 * d2);   // dall'alto si vede il bordo; di taglio, tutta la riga
                if (aa <= soglia(px + ox, py + oy)) continue;
                const std::uint8_t c0 = t.at(px, py);
                if (p.materia == Materia::colorante) {   // fluoresceina: verde chiaro al centro, piu' scura sul bordo
                    if (aa * std::clamp(2.0 * luce, 0.0, 1.0) <= soglia(px + oy, py + ox)) continue;
                    t.pixel(px, py, static_cast<std::uint8_t>(d2 < 0.45 ? colori::VERDE_CHIARO : colori::VERDE));
                } else if (p.materia == Materia::anello_scuro || p.materia == Materia::nafta) {   // scurisce il mare
                    if (c0 >= 96 && c0 <= 143) t.pixel(px, py, static_cast<std::uint8_t>(std::max(96, c0 - (p.materia == Materia::nafta ? 3 : 5))));
                } else {
                    t.pixel(px, py, static_cast<std::uint8_t>(p.materia == Materia::fango ? colori::FANGO_CHIARO : colori::GRIGI + 13));
                }
            }
        }
        return;
    }

    if (p.materia == Materia::luce) {   // un punto che non si spegne; di notte con l'alone, di giorno solo da vicino
        if (luce > 0.5 && distanza > 3000.0) return;
        const int px = static_cast<int>(std::floor(x)), py = static_cast<int>(std::floor(y));
        const int c = p.tono < 0.25 ? 12 : p.tono < 0.75 ? colori::VERDE_CHIARO : colori::BIANCO;
        const int alone = p.tono < 0.25 ? colori::ROSSO : p.tono < 0.75 ? colori::VERDE : colori::GRIGIO;
        auto punto = [&](int qx, int qy, int col) {
            if (qy < limite && qx >= 0 && qy >= 0 && qx < t.larghezza() && qy < t.altezza()) t.pixel(qx, qy, static_cast<std::uint8_t>(col));
        };
        if (luce < 0.4 && distanza < 8000.0)
            for (const auto& [dx, dy] : {std::pair{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) punto(px + dx, py + dy, alone);
        punto(px, py, c);
        return;
    }
    if (p.materia == Materia::tracciante || p.materia == Materia::illuminante) {
        const int px = static_cast<int>(std::floor(x)), py = static_cast<int>(std::floor(y));
        auto punto = [&](int qx, int qy, int col) {
            if (qy < limite && qx >= 0 && qy >= 0 && qx < t.larghezza() && qy < t.altezza()) t.pixel(qx, qy, static_cast<std::uint8_t>(col));
        };
        if (p.materia == Materia::tracciante) {   // il punto rosso-arancio e il tratto verso dove viene
            punto(px, py, colori::FUOCO[3]);
            punto(px + (p.vx > 0 ? -1 : 1), py + (p.vz > 0 ? 1 : 0), colori::FUOCO[1]);
            return;
        }
        // l'illuminante: il nucleo bianco, l'alone giallo di notte (piu' largo quanto piu' e' buio)
        const int alone = luce < 0.2 ? 3 : luce < 0.5 ? 2 : 1;
        for (int dy = -alone; dy <= alone; ++dy)
            for (int dx = -alone; dx <= alone; ++dx)
                if (dx * dx + dy * dy <= alone * alone) punto(px + dx, py + dy, dx * dx + dy * dy <= 1 ? colori::BIANCO : colori::FUOCO[5]);
        return;
    }
    if (p.materia == Materia::cavo) {   // filo di 1 pixel, retinato se e' troppo sottile per vedersi pieno
        const int px = static_cast<int>(std::floor(x));
        if (px < 0 || px >= t.larghezza()) return;
        const double copertura = std::clamp(0.3 + 0.06 * px_m, 0.3, 1.0) * a;
        const int y0 = std::max(static_cast<int>(std::floor(y)), 0);
        const int y1 = std::min({static_cast<int>(std::ceil(y + p.H * px_m)), limite - 1, t.altezza() - 1});
        for (int py = y0; py <= y1; ++py)
            if (copertura > soglia(px + ox, py + oy)) t.pixel(px, py, static_cast<std::uint8_t>(colori::GRIGI + 3));
        return;
    }
    const double rp = p.r * px_m;
    const double scintilla = static_cast<double>(mescola(p.seme, static_cast<std::uint32_t>(tempo_ * 12.0)) & 0xffu) / 255.0;
    // indice di palette continuo (poi retinato): fy -1 in cima al disco, +1 in fondo; luce dall'alto
    auto colore = [&](double fy) -> double {
        const double alto = 0.5 - 0.5 * fy;   // 1 in cima
        switch (p.materia) {
            case Materia::fuoco: {
                const double c = std::clamp(p.calore * (1.0 - 0.8 * p.eta / p.vita) + 0.3 * (scintilla - 0.5) - 0.15 * fy, 0.0, 0.999);
                return colori::FUOCO[static_cast<int>(c * 6)];
            }
            case Materia::fumo: {
                const double l = p.calore * std::exp(-p.eta / 6.0) * (1.0 - 2.0 * luce) * (0.5 + 0.5 * fy);   // a poche decine di metri dalle fiamme
                if (l > 0.35) return colori::FUMO_ACCESO_CHIARO;
                if (l > 0.12) return colori::FUMO_ACCESO;
                return colori::GRIGI + 1 + p.tono * 10 + 2.5 * alto;
            }
            case Materia::vapore: return colori::GRIGI + 11 + 3.0 * alto;
            case Materia::spruzzo: return colori::GRIGI + 10.5 + 3.5 * alto;
            case Materia::tinta: return alto > 0.35 ? colori::FANGO_CHIARO : colori::FANGO;   // acqua tinta: illuminata, non brilla di notte
            case Materia::nebbia: return colori::GRIGI + 10 + 2.5 * alto;
            case Materia::corona: return colori::GRIGI + 2 + 3.0 * alto;
            case Materia::fango: return alto > 0.5 ? colori::FANGO_CHIARO : colori::FANGO;
            case Materia::detrito:
                if (p.calore > 0) return colori::FUOCO[3 + static_cast<int>(scintilla * 2.99)];
                return p.tono > 0.5 ? colori::GRIGI + 12 : colori::GRIGI + 1 + 2.0 * alto;
            case Materia::schiuma: return colori::GRIGI + 12 + 2.5 * alto;
            default: return colori::GRIGI + 8;
        }
    };
    auto indice = [&](double c, int px, int py) {
        const double base = std::floor(c);
        const int i = static_cast<int>(base) + ((c - base) > soglia(px + 1, py + 2) ? 1 : 0);
        // le rampe dei grigi non sconfinano nel trasparente (159) ne' nel nero
        return static_cast<std::uint8_t>(i >= colori::GRIGI && i < colori::GRIGI + 15 ? std::clamp(i, colori::GRIGI + 1, colori::GRIGI + 14) : i);
    };
    if (rp < 0.75) {   // piu' piccola di un pixel: un punto con la copertura dell'area
        const int px = static_cast<int>(std::floor(x)), py = static_cast<int>(std::floor(y));
        if (py >= limite) return;
        const double aa = a * std::max(rp / 0.75, 0.25) * std::max(rp / 0.75, 0.25);
        if (aa <= soglia(px + ox, py + oy) && p.materia != Materia::fuoco) return;   // le fiamme si vedono sempre
        t.pixel(px, py, indice(colore(0), px, py));
        return;
    }
    // bordo frastagliato: il raggio varia col rumore (reticolo di ~1/3 del raggio, fermo rispetto alla particella);
    // dentro il 55% del raggio il disco e' pieno e il rumore non serve
    const bool frastagliata = p.materia != Materia::detrito && rp >= 2.5;
    const double cella = std::max(rp / 3.0, 1.5);
    const double rr = frastagliata ? rp * 1.15 : rp;
    constexpr int NODI = 16;
    std::array<float, NODI * NODI> nodi{};
    const int nc = frastagliata ? std::min(static_cast<int>(std::ceil(rr / cella)) + 1, NODI / 2 - 1) : 0;
    if (frastagliata)
        for (int j = -nc; j <= nc + 1; ++j)
            for (int i = -nc; i <= nc + 1; ++i)
                nodi[static_cast<std::size_t>((j + nc) * NODI + i + nc)] = static_cast<float>(
                    (mescola(mescola(p.seme, static_cast<std::uint32_t>(i)), static_cast<std::uint32_t>(j)) & 0xffffu) / 65535.0);
    const double rr_n2 = (rr / rp) * (rr / rp), inv_rp = 1.0 / rp, inv_cella = 1.0 / cella;
    const Rettangolo& rit = t.ritaglio();
    const int ya = std::max(static_cast<int>(std::floor(y - rr)), rit.y);
    const int yb = std::min({static_cast<int>(std::ceil(y + rr)), limite - 1, rit.y + rit.h - 1});
    std::array<double, NODI> riga_rumore{};   // rumore della riga ai nodi del reticolo (interpolato in v)
    for (int py = ya; py <= yb; ++py) {
        const double fy = (py + 0.5 - y) * inv_rp, fy2 = fy * fy;
        if (fy2 > rr_n2) continue;
        // colore della riga (due indici vicini, scelti per pixel dalla retinatura) e soglie delle 4 colonne
        const double c = colore(fy), base = std::floor(c), frazione = c - base;
        const int ib = static_cast<int>(base);
        const bool grigio = ib >= colori::GRIGI && ib < colori::GRIGI + 15;
        auto stringi = [&](int i) { return static_cast<std::uint8_t>(grigio ? std::clamp(i, colori::GRIGI + 1, colori::GRIGI + 14) : i); };
        std::uint8_t col[4];
        double opa[4];
        for (int k = 0; k < 4; ++k) {
            col[k] = frazione > soglia(k + 1, py + 2) ? stringi(ib + 1) : stringi(ib);
            opa[k] = soglia(k + ox, py + oy);
        }
        if (frastagliata) {
            const double v = std::clamp((py + 0.5 - y) * inv_cella + nc, 0.0, 2 * nc + 0.999);
            const auto j = static_cast<std::size_t>(v);
            double sv = v - static_cast<double>(j);
            sv = sv * sv * (3 - 2 * sv);
            for (std::size_t i = 0; i <= static_cast<std::size_t>(2 * nc + 1); ++i) {
                const double q0 = nodi[j * NODI + i], q1 = nodi[(j + 1) * NODI + i];
                riga_rumore[i] = q0 + (q1 - q0) * sv;
            }
        }
        // solo l'arco della riga dentro il disco; scrittura senza salti (la retinatura li renderebbe imprevedibili)
        const double mezza = std::sqrt(rr_n2 - fy2) * rp;
        const int xa = std::max(static_cast<int>(std::floor(x - mezza)), rit.x);
        const int xb = std::min(static_cast<int>(std::ceil(x + mezza)), rit.x + rit.w - 1);
        std::uint8_t* dst = t.riga(py);
        for (int px = xa; px <= xb; ++px) {
            const double fx = (px + 0.5 - x) * inv_rp;
            const double d2 = fx * fx + fy2;
            double aa = a;
            if (d2 > 0.47) {   // vicino al bordo: sfumatura e frastagliatura
                double d = std::sqrt(d2);
                if (frastagliata) {
                    const double u = std::clamp((px + 0.5 - x) * inv_cella + nc, 0.0, 2 * nc + 0.999);
                    const auto i = static_cast<std::size_t>(u);
                    double su = u - static_cast<double>(i);
                    su = su * su * (3 - 2 * su);
                    d -= 0.45 * (riga_rumore[i] + (riga_rumore[i + 1] - riga_rumore[i]) * su - 0.5) + 0.1;
                }
                aa = d >= 1.0 ? 0.0 : aa * std::min(1.0, 3.0 * (1 - d * d));
            }
            const int k = px & 3;
            dst[px] = aa > opa[k] ? col[k] : dst[px];
        }
    }
}

}  // namespace m688
