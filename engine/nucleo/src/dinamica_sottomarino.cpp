#include "nucleo/dinamica_sottomarino.hpp"

#include <algorithm>
#include <cmath>
#include <map>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double G = 9.81, GRADI = 3.14159265358979323846 / 180.0;
constexpr double SHP = 0.7457;              // kW per shp
constexpr double RENDIMENTO = 0.7;          // potenza all'asse che diventa spinta utile
constexpr double MASSA_AGGIUNTA = 1.1;      // in avanzo
constexpr double PERDITA_ACCOSTATA = 1.5;   // resistenza in piu' con la barra tutta
constexpr double RATEO_GIRI = 0.05;         // frazione dei giri al secondo (turbina)
constexpr double RATEO_BARRA = 5.0;         // gradi/s
constexpr double V_PIANI = 8.0;             // m/s a cui gli idroplani hanno piena autorita'
constexpr double MOMENTO_PIANI = 15.0;      // gradi di momento di raddrizzamento che i piani compensano a V_PIANI
constexpr double ATM_ACQUA = 10.33;         // m d'acqua per un'atmosfera

struct Tabella {
    double dislocamento_t, velocita_max_kn, potenza_shp, diametro_L, v_cav0_kn;
};

// dossier 01 §2 e §8.1 (dislocamento in immersione, velocita', potenza all'asse); diametro tattico e insorgenza della
// cavitazione dal dossier 01 §2.1 e §3.3 e dal dossier 02 §2.3 (stime). Potenza 0: non pubblicata (scala del 688)
const std::map<std::string, Tabella> TABELLA = {
    {"los_angeles", {6927, 32, 30000, 5.0, 5.0}},
    {"sturgeon", {4714, 26, 15000, 5.0, 4.5}},
    {"trafalgar", {5300, 32, 0, 5.0, 5.5}},     // pump-jet: 9-10 kn a quota periscopica
    {"alfa", {3180, 41, 40000, 3.0, 4.0}},      // accelerazione e virata eccezionali
    {"victor3", {7250, 30, 31000, 5.0, 4.5}},
    {"akula", {12770, 33, 43000, 5.0, 5.5}},
    {"oscar", {19400, 32, 98000, 6.0, 4.0}},
    {"charlie2", {5500, 24, 15000, 5.0, 3.5}},
    {"typhoon", {48000, 26, 100000, 6.0, 4.0}},
    {"kilo", {3075, 20, 5500, 4.5, 3.5}},
    {"foxtrot", {2515, 15, 5400, 5.0, 3.0}},
};

}  // namespace

ParametriSottomarino parametri_sottomarino(const std::string& classe, const ClasseNave* c, const DatiSottomarino* s) {
    ParametriSottomarino p;
    const auto it = TABELLA.find(classe);
    const Tabella t = it != TABELLA.end() ? it->second : Tabella{p.dislocamento_t, p.velocita_max_kn, 0, 5.0, 4.5};
    p.dislocamento_t = s && s->presente && s->dislocamento_immersione_t > 0 ? s->dislocamento_immersione_t : t.dislocamento_t;
    p.velocita_max_kn = c && c->velocita_max_nodi > 0 ? c->velocita_max_nodi : t.velocita_max_kn;
    if (c && c->lunghezza_m > 0) p.lunghezza_m = c->lunghezza_m;
    if (c && c->larghezza_m > 0) p.larghezza_m = c->larghezza_m;
    p.diametro_tattico_L = t.diametro_L;
    p.v_cavitazione0_kn = t.v_cav0_kn;
    // senza dato: la potenza del 688 in scala con dislocamento^(2/3) e velocita'^3 (coefficiente dell'Ammiragliato)
    p.potenza_kW = t.potenza_shp > 0 ? t.potenza_shp * SHP
                                     : 30000 * SHP * std::pow(p.dislocamento_t / 6927.0, 2.0 / 3.0) * std::pow(p.velocita_max_kn / 32.0, 3.0);
    return p;
}

DinamicaSottomarino::DinamicaSottomarino(const ParametriSottomarino& p, double rotta, double quota_m, double velocita_kn)
    : p_(p), u_(velocita_kn * NODO), rotta_(normalizza_360(rotta)), z_(quota_m) {
    vmax_ = p_.velocita_max_kn * NODO;
    c_ = RENDIMENTO * p_.potenza_kW * 1000.0 / (vmax_ * vmax_ * vmax_);
    massa_ = p_.dislocamento_t * 1000.0 * MASSA_AGGIUNTA;
    raggio_ = p_.diametro_tattico_L * p_.lunghezza_m / 2.0;
    area_ = p_.area_pianta_m2 > 0 ? p_.area_pianta_m2 : 0.8 * p_.lunghezza_m * p_.larghezza_m;
    raddrizzamento_ = p_.dislocamento_t * 1000.0 * G * p_.bg_m;
    n_ = giri_per_velocita(velocita_kn);
}

double DinamicaSottomarino::velocita_kn() const { return u_ / NODO; }

void DinamicaSottomarino::imposta_rotta(double rotta) { rotta_ = normalizza_360(rotta); r_ = 0.0; }

void DinamicaSottomarino::imposta_velocita(double kn) { u_ = kn * NODO; n_ = giri_per_velocita(kn); }

double DinamicaSottomarino::giri_per_velocita(double kn) const {
    return std::clamp(kn / p_.velocita_max_kn, GIRI_INDIETRO, 1.0);
}

double DinamicaSottomarino::velocita_cavitazione_kn(double quota_m) const {
    return p_.v_cavitazione0_kn * std::sqrt((ATM_ACQUA + std::max(0.0, quota_m)) / ATM_ACQUA);
}

bool DinamicaSottomarino::cavita() const { return std::fabs(n_) * p_.velocita_max_kn > velocita_cavitazione_kn(); }

void DinamicaSottomarino::avanza(double dt, const ComandiSottomarino& c, const ForzeIdrostatiche& f, const LimitiBattello& l) {
    // giri: la turbina insegue l'ordine, entro la velocita' consentita dai danni (SCRAM: motore d'emergenza). Il limite
    // taglia i giri, non li scala: sotto il limite l'ordine da' la sua velocita' di regime
    const double lim = std::clamp(l.velocita, 0.0, 1.0);
    const double giri = std::clamp(c.giri, std::max(GIRI_INDIETRO, -lim), lim);
    n_ += std::clamp(giri - n_, -RATEO_GIRI * dt, RATEO_GIRI * dt);

    // barra: il timoniere accosta per la rotta ordinata smorzando il rateo, oppure tiene l'angolo ordinato
    if (l.timone) {
        double voluta = c.barra_deg;
        if (c.rotta) {
            const double massima = std::clamp(c.barra_max_deg, 0.0, BARRA_MAX);
            voluta = std::clamp(normalizza_180(*c.rotta - rotta_) - 8.0 * r_, -massima, massima);
        }
        voluta = std::clamp(voluta, -BARRA_MAX, BARRA_MAX);
        barra_ += std::clamp(voluta - barra_, -RATEO_BARRA * dt, RATEO_BARRA * dt);
    }

    // avanzo
    const double spinta = c_ * vmax_ * vmax_ * n_ * std::fabs(n_) * (n_ < 0.0 ? 0.7 : 1.0);
    const double frazione_barra = barra_ / BARRA_MAX;
    const double resistenza = c_ * u_ * std::fabs(u_) * (1.0 + PERDITA_ACCOSTATA * frazione_barra * frazione_barra);
    u_ += (spinta - resistenza) / massa_ * dt;

    // imbardata: il rateo di regime e' u / R alla barra tutta
    const double r_regime = u_ / raggio_ * frazione_barra / GRADI;   // gradi/s
    const double tau = 1.5 * p_.lunghezza_m / std::max(std::fabs(u_), 0.5);
    r_ += (r_regime - r_) * std::min(1.0, dt / tau);
    rotta_ = normalizza_360(rotta_ + r_ * dt);

    // beccheggio: autorita' dei piani col quadrato della velocita'; il momento dei danni sposta l'assetto quanto i piani
    // non compensano
    const double autorita = l.idroplani ? std::min(1.0, std::fabs(u_) / V_PIANI) : 0.0;
    const double compensato = autorita * autorita * raddrizzamento_ * std::sin(MOMENTO_PIANI * GRADI);
    const double m = f.momento_assetto_Nm;
    const double residuo = (m >= 0 ? 1.0 : -1.0) * std::max(0.0, std::fabs(m) - compensato);
    const double disturbo = std::asin(std::clamp(residuo / raddrizzamento_, -1.0, 1.0)) / GRADI;
    double voluto = disturbo;
    if (autorita > 0.0) {
        const double massimo = std::clamp(c.angolo_max_deg, 0.0, 30.0);
        // quota ordinata: mezzo grado di assetto per metro di scarto, prua in su se troppo in basso
        const double comando = c.quota_m ? 0.5 * (z_ - *c.quota_m) : -c.angolo_deg;
        voluto += std::clamp(comando, -massimo, massimo);
    }
    const double rateo_assetto = 0.2 + 3.0 * autorita;   // gradi/s: idrostatico a battello fermo
    theta_ += std::clamp((std::clamp(voluto, -60.0, 60.0) - theta_) / 4.0 * dt, -rateo_assetto * dt, rateo_assetto * dt);

    // quota: componente verticale della velocita' lungo l'asse inclinato piu' la deriva del peso netto contro la
    // resistenza verticale della pianta (modello del kit a velocita' 0: la portanza dell'angolo d'attacco qui e' gia'
    // l'assetto, che il timoniere della quota alza quando il battello e' pesante; contarla anche li' la darebbe due volte)
    w_ = -u_ * std::sin(theta_ * GRADI) + rateo_verticale_regime(f.peso_netto_N, area_, 0.0, 0);
    z_ += w_ * dt;
    if (z_ < 0.0) { z_ = 0.0; if (w_ < 0.0) w_ = 0.0; }   // in superficie galleggia
}

}  // namespace m688
