#include "nucleo/collisioni.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "nucleo/casuale.hpp"
#include "nucleo/scafo.hpp"

namespace m688 {

namespace {

constexpr double G = 9.81;
constexpr double ATTRITO = 0.3;           // strisciamento: frazione dell'energia tangenziale che si dissipa sugli scafi
constexpr double T_CONTATTO_S = 0.5;      // durata dell'urto per la scossa (16 §4.2: 0,2-1 s, STIMA)
constexpr double LAMA = 0.3;              // capacita' della lamiera davanti a una parte a lama (STIMA)
constexpr double CAP_APPENDICE = 2.0;     // MJ: duomo, eliche, timoni, alberi (16 §4.2: 1-3)
constexpr double CAP_VELA = 10.0, CAP_VELA_GHIACCIO = 50.0;
constexpr double SPESSORE_RESISTENTE_M = 0.035;

// La forma di un'unita' (16 §4.1) dai dati della classe
struct Forma {
    bool fisso = false, sottomarino = false, doppio = false, ghiaccio = false, piani_vela = false;
    double L = 100.0, D = 10.0, T = 8.0;   // lunghezza; diametro (sottomarini) o larghezza; pescaggio
    double vela = 0.0, vela_x0 = 0.0, vela_x1 = 0.0;
    double duomo = 0.0, duomo_x0 = 0.0, duomo_x1 = 0.0;
    double collaudo = 300.0;
};

Forma forma(const CorpoUrto& c, const std::map<std::string, ClasseNave>& classi, const std::string& dati) {
    Forma f;
    f.sottomarino = c.sottomarino;
    f.fisso = c.massa_t <= 0.0;
    const auto it = classi.find(c.classe);
    if (it == classi.end()) return f;
    const ClasseNave& k = it->second;
    if (k.lunghezza_m > 0) f.L = k.lunghezza_m;
    if (k.larghezza_m > 0) f.D = k.larghezza_m;
    if (k.pescaggio_m > 0) f.T = k.pescaggio_m;
    f.vela = k.altezza_vela_m;
    f.vela_x0 = k.vela_x_m[0];
    f.vela_x1 = k.vela_x_m[1];
    f.duomo = k.duomo_m;
    f.duomo_x0 = k.duomo_x_m[0];
    f.duomo_x1 = k.duomo_x_m[1];
    f.ghiaccio = c.classe == "typhoon";   // vela rinforzata per il ghiaccio (16 §4.2)
    f.piani_vela = c.classe == "los_angeles" || c.classe == "sturgeon";   // timoni di vela (688 Flight I, Sturgeon)
    if (c.sottomarino && !k.scafo.empty()) {
        try {
            const DatiScafo s = DatiScafo::carica(dati + "/" + k.scafo);
            f.doppio = s.sottomarino.scafo == "doppio";
            if (s.sottomarino.quota_collaudo_m > 0) f.collaudo = s.sottomarino.quota_collaudo_m;
        } catch (const std::exception&) {
        }
    }
    return f;
}

// Il punto di contatto e la normale nelle coordinate dello scafo
struct Locale {
    double x = 0, y = 0, z = 0;    // kit: prua, sinistra, su dal galleggiamento (dei sottomarini: in superficie)
    double zk = 0;                 // su dalla chiglia
    double n[3] = {0, 0, 0};       // normale nel riferimento dello scafo (avanti, sinistra, su)
};

Locale locale(const CorpoUrto& c, const Forma& f, const Urto& u) {
    Locale l;
    const double r = radianti(c.rotta_gradi);
    const double fx = std::sin(r), fy = std::cos(r), px = -std::cos(r), py = std::sin(r);   // avanti e sinistra
    const double dx = u.punto.x - c.pos.x, dy = u.punto.y - c.pos.y;
    l.x = std::clamp(dx * fx + dy * fy, -f.L / 2, f.L / 2);
    l.y = dx * px + dy * py;
    l.zk = c.quota_m - u.quota_punto_m;
    l.z = f.sottomarino ? l.zk - f.T : -u.quota_punto_m;
    l.n[0] = u.normale[0] * fx + u.normale[1] * fy;
    l.n[1] = u.normale[0] * px + u.normale[1] * py;
    l.n[2] = u.normale[2];
    return l;
}

double massa_aggiunta(const Forma& f, const Locale& l) {
    const double ax = l.n[0] * l.n[0], lat = l.n[1] * l.n[1], ver = l.n[2] * l.n[2];
    if (f.sottomarino) return 0.02 * ax + 0.95 * (lat + ver);   // ellissoide L/D 8-11 (Newman)
    return 0.075 * ax + 0.4 * lat + 1.0 * ver;                   // Minorsky; longitudinale e verticale STIMA
}

ParteUrto parte(const CorpoUrto& c, const Forma& f, const Locale& l) {
    const double meta = f.L / 2, x = l.x;
    if (f.fisso) return ParteUrto::fianco;
    if (f.sottomarino) {
        const double D = f.D, zk = l.zk;
        const bool sulla_vela = f.vela > 0 && x >= f.vela_x0 - 1.0 && x <= f.vela_x1 + 1.0;
        if (sulla_vela && zk > D - 0.3) {
            if ((c.periscopio_alzato || c.alberi_alzati) && zk > D + f.vela + 0.2) return ParteUrto::alberi;
            return ParteUrto::vela;
        }
        if (x > meta - 0.06 * f.L) return ParteUrto::duomo;
        if (x > meta - 0.15 * f.L) return ParteUrto::prua;
        if (x < -meta + 0.03 * f.L) return ParteUrto::eliche;
        if (x < -meta + 0.10 * f.L)
            return zk > 0.85 * D || zk < 0.15 * D || std::fabs(l.y) > 0.45 * D ? ParteUrto::timoni : ParteUrto::poppa;
        if (zk > 0.8 * D) return ParteUrto::dorso;
        if (zk < 0.2 * D) return ParteUrto::chiglia;
        return ParteUrto::fianco;
    }
    const double chiglia = -(f.T - f.duomo);   // la chiglia dello scafo, sopra il fondo del duomo
    if (l.z > 0.3) return x > meta - 0.10 * f.L ? ParteUrto::prua : ParteUrto::sovrastrutture;
    if (f.duomo > 0 && x >= f.duomo_x0 - 1.0 && x <= f.duomo_x1 + 1.0 && l.z < chiglia + 0.3) return ParteUrto::duomo;
    if (x > meta - 0.10 * f.L) return ParteUrto::prua;
    if (x < -meta + 0.06 * f.L && l.z < 0.4 * chiglia) return std::fabs(l.y) < 0.1 * f.D ? ParteUrto::timoni : ParteUrto::eliche;
    if (l.z < chiglia + 0.5) return ParteUrto::chiglia;
    return ParteUrto::fianco;
}

// rigidezza della parte (16 §4.2): 1 bassa, 2 media, 3 alta; l'ostacolo fisso non cede
int rigidezza(const Forma& f, ParteUrto p, double massa_t) {
    if (f.fisso) return 10;
    if (f.sottomarino) return p == ParteUrto::vela ? (f.ghiaccio ? 3 : 2) : 1;
    switch (p) {
        case ParteUrto::prua: return 3;
        case ParteUrto::fianco: case ParteUrto::chiglia: return massa_t < 3000.0 ? 1 : 2;   // pescherecci, mercantili piccoli
        default: return 1;
    }
}

// le parti a lama concentrano l'urto sulla lamiera dell'altro
bool a_lama(const Forma& f, ParteUrto p) {
    if (f.fisso) return true;
    if (f.sottomarino) return p == ParteUrto::vela || p == ParteUrto::timoni || p == ParteUrto::eliche || p == ParteUrto::alberi;
    return p == ParteUrto::prua || p == ParteUrto::timoni || p == ParteUrto::eliche;
}

void perdita(DanniUrtoUnita& d, Sistema s, double p) {
    p = std::clamp(p, 0.0, 1.0);
    if (p > 0.01) d.sistemi.emplace_back(s, p);
}

// falla da energia oltre la capacita' (16 §2.2: Minorsky sopra i 33 MJ, sotto in proporzione), m^2
double falla(double e, double capacita, double spessore) {
    if (e <= capacita) return 0.0;
    const double rt = std::max((e - 32.7) / 47.2, (e - capacita) / 80.0);
    return rt / spessore;
}

// lo scafo di una nave (o di un sottomarino in superficie) sotto il galleggiamento: lamiera e falla
void lamiera_nave(DanniUrtoUnita& d, const CorpoUrto& c, double e, double lama, double var) {
    if (d.z_m >= 0.0 || c.massa_t <= 0.0) return;
    const double cap = 1.5 * std::pow(std::cbrt(c.massa_t / 1000.0), 2) * lama * var;
    const double spessore = 0.008 + 0.0005 * std::cbrt(c.massa_t);
    d.falla_nave_m2 += falla(e, cap, spessore);
}

void danni_nave(DanniUrtoUnita& d, const CorpoUrto& c, const Forma& f, double lama, Casuale& caso) {
    const double e = d.energia_mj, var = caso.uniforme(0.9, 1.1);
    switch (d.parte) {
        case ParteUrto::sovrastrutture:
            d.sovrastrutture = e > 0.05;
            return;
        case ParteUrto::duomo:
            d.duomo_perso = e > 0.5;
            lamiera_nave(d, c, e - CAP_APPENDICE * var, lama, var);
            return;
        case ParteUrto::eliche: {
            const double g = std::clamp(e / 4.0, 0.2, 0.6);
            d.eliche_danneggiate = true;
            d.velocita_max = std::min(d.velocita_max, 1.0 - g);
            d.rumore_db += 3.0 + 12.5 * (g - 0.2);
            lamiera_nave(d, c, e - CAP_APPENDICE * var, lama, var);
            return;
        }
        case ParteUrto::timoni:
            d.timone_bloccato = e > 0.3;
            lamiera_nave(d, c, e - CAP_APPENDICE * var, lama, var);
            return;
        case ParteUrto::prua:
            lamiera_nave(d, c, e, lama * 3.0, var);   // prua rinforzata
            return;
        default:
            lamiera_nave(d, c, e, lama, var);
            (void)f;
            return;
    }
}

void danni_sottomarino(DanniUrtoUnita& d, const CorpoUrto& c, const Forma& f, double lama, bool contro_elica, Casuale& caso) {
    const double e = d.energia_mj, s = f.D / 10.0, var = caso.uniforme(0.9, 1.1);
    const double cap_casing = 12.0 * s * (f.doppio ? 1.5 : 1.0) * lama * var;
    // lo scafo resistente; la vela distribuisce il carico (niente lama), e il grosso dell'energia lo prendono la vela
    // che si piega e il rollio
    const double cap_res = 40.0 * s * std::max(0.25, 1.0 - 0.5 * std::max(c.quota_m, 0.0) / f.collaudo) *
                           (d.parte == ParteUrto::vela ? 1.0 : lama) * var;
    double resto = 0.0;   // l'energia che arriva allo scafo resistente
    auto casing = [&](double x) {
        if (x <= 0.0) return 0.0;
        const double ec = std::min(x, cap_casing);
        if (ec > 0.5) d.falla_cassa_m2 += std::min(6.0, 0.25 * ec);
        return x - cap_casing;
    };
    switch (d.parte) {
        case ParteUrto::duomo: case ParteUrto::prua: {
            const double ed = std::min(e, CAP_APPENDICE);
            const double k = d.parte == ParteUrto::duomo ? 1.0 : 0.5;
            perdita(d, Sistema::sonar_prua, k * (ed - 0.3) / 1.2);
            perdita(d, Sistema::sonar_tiro, 0.6 * k * (ed - 0.3) / 1.2);
            double x = e - CAP_APPENDICE;
            if (x > 0) d.falla_cassa_m2 += std::clamp(0.25 * x, 0.1, 10.0);   // casse prodiere
            resto = x - 1000.0 * s * s;   // la zona prodiera fino alla paratia (taratura San Francisco: niente dispersione)
            break;
        }
        case ParteUrto::vela: {
            const bool su = c.periscopio_alzato || c.alberi_alzati;
            const double p = su ? (e - 0.2) / 1.5 : (e - 1.0) / 4.0;   // alberi spezzati o vela piegata sugli alberi
            perdita(d, Sistema::periscopio, p);
            perdita(d, Sistema::antenna, p);
            if (f.piani_vela) perdita(d, Sistema::idroplani, (e - 1.0) / 5.0);
            resto = 0.3 * (e - (f.ghiaccio ? CAP_VELA_GHIACCIO : CAP_VELA) * var);   // la vela scarica sullo scafo
            break;
        }
        case ParteUrto::alberi:
            perdita(d, Sistema::periscopio, (e - 0.05) / 0.5);
            perdita(d, Sistema::antenna, (e - 0.05) / 0.5);
            break;
        case ParteUrto::eliche: {
            const double g = std::clamp(e / 3.0, 0.2, 0.7);
            d.velocita_max = std::min(d.velocita_max, 1.0 - g);
            d.rumore_db += 3.0 + 14.0 * (g - 0.2);
            perdita(d, Sistema::propulsione, g);
            resto = casing(e - CAP_APPENDICE);
            break;
        }
        case ParteUrto::timoni:
            perdita(d, Sistema::timone, (e - 0.3) / 1.5);
            perdita(d, Sistema::idroplani, (e - 0.3) / 1.5);
            resto = casing(e - CAP_APPENDICE);
            break;
        default:
            resto = casing(e);
            if (d.parte == ParteUrto::poppa) perdita(d, Sistema::propulsione, (e - 5.0) / 30.0);
            break;
    }
    // la schiera filata presa da chi passa a poppa
    if (c.schiera_filata && (d.parte == ParteUrto::poppa || d.parte == ParteUrto::eliche || d.parte == ParteUrto::timoni))
        perdita(d, Sistema::schiera, 1.0);
    // lo scafo resistente: ammaccatura appena l'energia ci arriva, falla oltre la capacita'
    if (resto > 0.0) {
        d.ammaccatura = std::clamp(0.2 + 0.6 * resto / std::max(cap_res, 1.0), 0.2, 0.8);
        d.falla_resistente_m2 = falla(resto, cap_res, SPESSORE_RESISTENTE_M);
    }
    // l'elica dell'altro incide lo scafo resistente (Sceptre 1981)
    if (contro_elica && e > 5.0) {
        d.falla_resistente_m2 = std::max(d.falla_resistente_m2, 0.01 + 0.09 * std::clamp((e - 5.0) / 20.0, 0.0, 1.0));
        d.ammaccatura = std::max(d.ammaccatura, 0.2);
    }
}

}  // namespace

const char* nome_parte_urto(ParteUrto p) {
    switch (p) {
        case ParteUrto::prua: return "prua";
        case ParteUrto::duomo: return "duomo";
        case ParteUrto::fianco: return "fianco";
        case ParteUrto::dorso: return "dorso";
        case ParteUrto::chiglia: return "chiglia";
        case ParteUrto::vela: return "vela";
        case ParteUrto::alberi: return "alberi";
        case ParteUrto::poppa: return "poppa";
        case ParteUrto::eliche: return "eliche";
        case ParteUrto::timoni: return "timoni";
        case ParteUrto::schiera: return "schiera";
        case ParteUrto::sovrastrutture: return "sovrastrutture";
    }
    return "?";
}

EsitoUrto valuta_urto(const Urto& u, const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati,
                      std::uint64_t seme) {
    EsitoUrto r;
    Casuale caso(seme);
    const Forma fa = forma(u.a, classi, cartella_dati), fb = forma(u.b, classi, cartella_dati);
    const Locale la = locale(u.a, fa, u), lb = locale(u.b, fb, u);
    // velocita' relativa di a rispetto a b, nel mondo
    auto vel = [](const CorpoUrto& c, double v[3]) {
        const double ang = radianti(c.rotta_gradi);
        v[0] = c.velocita_m_s * std::sin(ang);
        v[1] = c.velocita_m_s * std::cos(ang);
        v[2] = -c.rateo_verticale_m_s;
    };
    double va[3], vb[3], vr[3];
    vel(u.a, va);
    vel(u.b, vb);
    const double* n = u.normale;
    double vn = 0.0;
    for (int i = 0; i < 3; ++i) {
        vr[i] = va[i] - vb[i];
        vn += vr[i] * n[i];
    }
    double vt2 = 0.0;
    for (int i = 0; i < 3; ++i) vt2 += std::pow(vr[i] - vn * n[i], 2);
    r.v_n_m_s = std::max(vn, 0.0);
    r.v_t_m_s = std::sqrt(vt2);
    r.angolo_gradi = gradi(std::atan2(r.v_n_m_s, r.v_t_m_s));
    r.radente = r.angolo_gradi < 25.0;
    // massa ridotta con le masse aggiunte nella direzione della normale
    const double ma = fa.fisso ? 0.0 : u.a.massa_t * 1000.0 * (1.0 + massa_aggiunta(fa, la));
    const double mb = fb.fisso ? 0.0 : u.b.massa_t * 1000.0 * (1.0 + massa_aggiunta(fb, lb));
    double mu = 0.0;
    if (ma > 0 && mb > 0) mu = ma * mb / (ma + mb);
    else mu = std::max(ma, mb);   // contro l'ostacolo fisso: tutta la massa di chi urta
    r.massa_ridotta_t = mu / 1000.0;
    r.energia_normale_mj = 0.5 * mu * r.v_n_m_s * r.v_n_m_s / 1e6;
    r.energia_tangente_mj = ATTRITO * 0.5 * mu * vt2 / 1e6;
    // parti colpite e ripartizione per rigidezza
    r.a.parte = parte(u.a, fa, la);
    r.b.parte = parte(u.b, fb, lb);
    const int ra = rigidezza(fa, r.a.parte, u.a.massa_t), rb = rigidezza(fb, r.b.parte, u.b.massa_t);
    double quota_a = 0.5;
    if (fb.fisso) quota_a = 1.0;
    else if (fa.fisso) quota_a = 0.0;
    else if (ra != rb) quota_a = ra < rb ? 0.7 : 0.3;
    for (auto* d : {&r.a, &r.b}) {
        const bool e_a = d == &r.a;
        const Locale& l = e_a ? la : lb;
        const double q = e_a ? quota_a : 1.0 - quota_a;
        d->energia_mj = q * r.energia_normale_mj;
        d->strisciamento_mj = 0.5 * r.energia_tangente_mj;
        d->x_m = l.x;
        d->y_m = l.y;
        d->z_m = l.z;
    }
    // danni di ciascuno, con la lama dell'altro
    for (int k = 0; k < 2; ++k) {
        const CorpoUrto& c = k == 0 ? u.a : u.b;
        const Forma& f = k == 0 ? fa : fb;
        const Forma& fo = k == 0 ? fb : fa;
        DanniUrtoUnita& d = k == 0 ? r.a : r.b;
        const ParteUrto po = k == 0 ? r.b.parte : r.a.parte;
        if (f.fisso) continue;
        const double lama = a_lama(fo, po) ? LAMA : 1.0;
        if (!f.sottomarino) {
            danni_nave(d, c, f, lama, caso);
        } else {
            danni_sottomarino(d, c, f, lama, fo.sottomarino && po == ParteUrto::eliche, caso);
            if (c.in_superficie) {   // la falla vicino al galleggiamento come in una nave; e' anche quella dello scafo resistente
                lamiera_nave(d, c, d.energia_mj, lama, caso.uniforme(0.9, 1.1));
                d.falla_resistente_m2 = std::max(d.falla_resistente_m2, d.falla_nave_m2);
            }
            // piastrelle anecoiche strappate dallo strisciamento e dall'urto; firma piu' alta
            d.piastrelle_m2 = std::clamp(20.0 * d.strisciamento_mj + 2.0 * d.energia_mj, 0.0, 300.0);
            d.rumore_db += std::min(6.0, d.piastrelle_m2 / 40.0);
        }
        // una lama che striscia apre uno squarcio lungo lo scafo
        if (a_lama(fo, po) && r.v_t_m_s > 1.0 && (d.falla_nave_m2 > 0.0 || d.falla_cassa_m2 > 0.0 || d.falla_resistente_m2 > 0.0))
            d.squarcio_m = std::clamp(r.v_t_m_s * 1.0, 2.0, 0.4 * f.L);
        // scossa e rollio dall'impulso lungo la normale
        const double m = k == 0 ? ma : mb;
        if (m > 0) {
            const double dv = mu * r.v_n_m_s / m;
            d.scossa_g = dv / (T_CONTATTO_S * G);
            const Locale& l = k == 0 ? la : lb;
            const double rg = 0.38 * f.D, gm = f.sottomarino ? 0.5 : 1.0;
            const double braccio_z = f.sottomarino ? l.zk - f.D / 2 : l.z;
            const double omega = dv * (std::fabs(l.n[1]) * std::fabs(braccio_z) + std::fabs(l.n[2]) * std::fabs(l.y)) / (rg * rg);
            d.rollio_gradi = std::min(90.0, gradi(omega / (std::sqrt(G * gm) / rg)));
        }
    }
    // acustica: il colpo e lo stridio
    const double e = r.energia_normale_mj + r.energia_tangente_mj;
    r.livello_sorgente_db = std::clamp(170.0 + 10.0 * std::log10(std::max(e, 1e-3)), 150.0, 195.0);
    const double contatto = 0.3 * std::min(fa.fisso ? 60.0 : fa.L, fb.fisso ? 60.0 : fb.L);
    r.durata_strisciamento_s = r.v_t_m_s > 0.5 ? std::clamp(contatto / r.v_t_m_s, 0.3, 20.0) : 0.3;
    return r;
}

double perdita_integrita_urto(const DanniUrtoUnita& d) {
    return std::clamp(d.falla_nave_m2 / 20.0 + (d.duomo_perso ? 0.05 : 0.0) + (d.eliche_danneggiate ? 0.1 : 0.0) +
                          (d.timone_bloccato ? 0.05 : 0.0) + (d.sovrastrutture ? 0.05 : 0.0),
                      0.0, 1.0);
}

}  // namespace m688
