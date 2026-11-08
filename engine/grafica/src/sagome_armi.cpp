#include "grafica/sagome_armi.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "grafica/cerimonia.hpp"

namespace m688 {

namespace {

using M = MaterialeArma;

enum class Forma {
    mk48, mk48_adcap, sov_ogiva, sov_piatto, shkval, missile_sov, caps_harpoon, caps_tomahawk, caps_generica, mk67,
    captor, pmr1, esca_adc, esca_mg84, mina_fondo, esca_mg74, esca_moss,
    // i siluri di siluri.toml senza voce di carico.toml e le classi delle missioni (kit, 27/10/2026)
    tigerfish, spearfish, mk46, aviolanciato_sov, umgt1, apr2, set40, balena
};

Forma forma_di(const SagomaArma& s) {
    const std::string& id = s.id;
    if (id == "mk48") return Forma::mk48;
    if (id == "mk48_adcap") return Forma::mk48_adcap;
    if (id == "type53_65k" || id == "type53") return Forma::sov_ogiva;   // type53: la classe delle missioni del 1989
    if (id == "tigerfish") return Forma::tigerfish;
    if (id == "spearfish") return Forma::spearfish;
    if (id == "mk46") return Forma::mk46;
    if (id == "at1" || id == "at2") return Forma::aviolanciato_sov;
    if (id == "umgt1") return Forma::umgt1;
    if (id == "apr2") return Forma::apr2;
    if (id == "set40" || id == "type40") return Forma::set40;            // type40: la classe delle missioni del 1989
    if (id == "biologico" || id == "balena") return Forma::balena;
    if (id == "set65" || id == "saet60m" || id == "uset80" || id == "test71m") return Forma::sov_piatto;
    if (id == "shkval") return Forma::shkval;
    if (id == "vyuga") return Forma::missile_sov;
    if (id == "harpoon") return Forma::caps_harpoon;
    if (id == "tasm" || id == "tlam_c") return Forma::caps_tomahawk;
    if (id == "sea_lance") return Forma::caps_generica;
    if (id == "mk67") return Forma::mk67;
    if (id == "mk60") return Forma::captor;
    if (id == "pmr1" || id == "pmr2") return Forma::pmr1;
    if (id == "mdt") return Forma::mina_fondo;
    if (id == "adc_mk2") return Forma::esca_adc;
    if (id == "mg84") return Forma::esca_mg84;
    if (id == "mg74") return Forma::esca_mg74;
    if (id == "moss_mk70") return Forma::esca_moss;
    switch (s.tipo) {   // armi nuove del catalogo: la forma del loro tipo
        case TipoArmaCarico::siluro: return s.sovietica ? Forma::sov_ogiva : Forma::mk48;
        case TipoArmaCarico::missile: return s.sovietica ? Forma::missile_sov : Forma::caps_generica;
        case TipoArmaCarico::mina: return Forma::mina_fondo;
        case TipoArmaCarico::esca: return s.sovietica ? Forma::esca_mg84 : Forma::esca_adc;
    }
    return Forma::mk48;
}

// la MOSS (10") si disegna nel suo manicotto da 533 mm, come sta nel tubo
double diametro_disegnato(const SagomaArma& s) {
    return forma_di(s) == Forma::esca_moss ? std::max(s.diametro_mm, 533.0) : s.diametro_mm;
}

double lunghezza_forma(Forma f) {
    switch (f) {
        case Forma::sov_ogiva:
        case Forma::sov_piatto: return 7.2;
        case Forma::shkval:
        case Forma::missile_sov: return 8.2;
        case Forma::caps_tomahawk:
        case Forma::caps_generica: return 6.4;
        case Forma::mk67: return 4.1;
        case Forma::captor: return 3.35;
        case Forma::pmr1: return 7.8;
        case Forma::esca_adc: return 1.0;
        case Forma::esca_mg84: return 3.0;
        case Forma::esca_mg74: return 3.9;
        case Forma::esca_moss: return 4.0;
        case Forma::mina_fondo: return 2.8;
        case Forma::tigerfish: return 6.46;
        case Forma::spearfish: return 6.1;
        case Forma::mk46: return 2.59;
        case Forma::aviolanciato_sov: return 4.0;
        case Forma::umgt1: return 3.6;
        case Forma::apr2: return 3.7;
        case Forma::set40: return 4.5;
        case Forma::balena: return 21.0;
        default: return 5.8;
    }
}

// Il diametro di una forma quando la sagoma non lo dice (sagoma_da_id)
double diametro_forma(Forma f, const std::string& id) {
    switch (f) {
        case Forma::mk46: return 324.0;
        case Forma::aviolanciato_sov: return id == "at1" ? 450.0 : 533.0;
        case Forma::umgt1:
        case Forma::set40: return 400.0;
        case Forma::apr2: return 350.0;
        case Forma::balena: return 2600.0;   // l'altezza del corpo, 0,12 della lunghezza
        case Forma::esca_adc: return 76.0;
        case Forma::mk67: return 533.0;
        default: return 533.0;
    }
}

// raggio relativo (0-1) di un'estremita' arrotondata: t = 0 all'attacco, 1 in punta; p alto = piu' piatta
double calotta(double t, double p) { return std::pow(std::max(0.0, 1.0 - std::pow(std::clamp(t, 0.0, 1.0), p)), 1.0 / p); }
double rampa(double u, double a, double b) { return std::clamp((u - a) / (b - a), 0.0, 1.0); }

struct Cella {
    std::int8_t m = -1;
    float tono = 0;
};

// Il tracciatore: una griglia di celle per la sagoma, poi le appendici e il contorno
class Tracciato {
public:
    Tracciato(const SagomaArma& s, double x_poppa, double y_asse, double px_m)
        : s_(s), f_(forma_di(s)), xp_(x_poppa), ya_(y_asse), px_m_(px_m) {
        L_ = lunghezza_disegnata(s) * px_m;
        R_ = std::max(1.0, 0.5 * diametro_disegnato(s) / 1000.0 * px_m);
        x0_ = static_cast<int>(std::floor(xp_)) - 1;
        x1_ = static_cast<int>(std::ceil(xp_ + L_)) + 1;
        y0_ = static_cast<int>(std::floor(ya_ - R_)) - 1;
        y1_ = static_cast<int>(std::ceil(ya_ + R_)) + 1;
        w_ = x1_ - x0_;
        h_ = y1_ - y0_;
        c_.assign(static_cast<std::size_t>(w_ * h_), Cella{});
    }

    void traccia(const PuntoSagoma& punto) {
        corpo();
        appendici();
        if (R_ >= 3.5) contorno();
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                const Cella& k = c_[static_cast<std::size_t>(y * w_ + x)];
                if (k.m >= 0) punto(x0_ + x, y0_ + y, static_cast<M>(k.m), std::clamp(static_cast<double>(k.tono), 0.0, 1.0));
            }
    }

private:
    const SagomaArma& s_;
    Forma f_;
    double xp_, ya_, px_m_, L_ = 0, R_ = 1;
    int x0_, x1_, y0_, y1_, w_, h_;
    std::vector<Cella> c_;

    double u_di(int x) const { return (x + 0.5 - xp_) / L_; }
    int x_di(double u) const { return static_cast<int>(std::floor(xp_ + u * L_)); }
    double m_(double metri) const { return metri / (L_ / px_m_); }   // metri -> frazione della lunghezza
    void metti(int x, int y, M m, double tono) {
        if (x < x0_ || y < y0_ || x >= x1_ || y >= y1_) return;
        auto& k = c_[static_cast<std::size_t>((y - y0_) * w_ + (x - x0_))];
        k.m = static_cast<std::int8_t>(m);
        k.tono = static_cast<float>(tono);
    }
    bool pieno(int x, int y) const {
        if (x < x0_ || y < y0_ || x >= x1_ || y >= y1_) return false;
        return c_[static_cast<std::size_t>((y - y0_) * w_ + (x - x0_))].m >= 0;
    }
    // una fascia di `metri` attorno a u, larga almeno un pixel
    bool fascia(double u, double centro, double metri) const {
        const double mezza = std::max(0.5 / L_, 0.5 * m_(metri));
        return std::fabs(u - centro) <= mezza;
    }
    bool dettagli() const { return R_ >= 2.5; }

    // raggio relativo del corpo lungo la sagoma (u = 0 poppa, 1 prua)
    double raggio(double u) const {
        switch (f_) {
            case Forma::mk48:
            case Forma::mk48_adcap: {
                if (u < 0.05) return 0.80;                                     // mantello del propulsore
                if (u < 0.24) return 0.50 + 0.50 * std::pow(rampa(u, 0.05, 0.24), 0.8);   // cono di poppa
                if (u > 0.90) return calotta((u - 0.90) / 0.10, 2.3);          // ogiva del sonar
                return 1.0;
            }
            case Forma::sov_ogiva:
            case Forma::sov_piatto: {
                if (u < 0.15) return 0.26 + 0.74 * std::pow(rampa(u, 0.0, 0.15), 0.85);
                if (f_ == Forma::sov_ogiva && u > 0.90) return calotta((u - 0.90) / 0.10, 2.0);
                if (f_ == Forma::sov_piatto && u > 0.955) return calotta((u - 0.955) / 0.045, 3.5);
                return 1.0;
            }
            case Forma::shkval: {
                if (u < 0.04) return 0.55;                                     // ugello del razzo
                if (u > 0.985) return 0.42;                                    // disco del cavitatore
                if (u > 0.92) return 1.0 - 0.65 * rampa(u, 0.92, 0.985);
                return 1.0;
            }
            case Forma::missile_sov: {
                if (u < 0.03) return 0.60;
                if (u > 0.86) return 0.15 + 0.85 * calotta(rampa(u, 0.86, 1.0), 1.4);
                return 1.0;
            }
            case Forma::caps_harpoon: {
                if (u < 0.10) return 0.72 + 0.28 * rampa(u, 0.0, 0.10);
                if (u > 0.90) return calotta((u - 0.90) / 0.10, 2.6);
                return 1.0;
            }
            case Forma::caps_tomahawk:
            case Forma::caps_generica: {
                const double e = m_(0.08);
                if (u < e) return 0.88 + 0.12 * (u / e);
                if (u > 1 - e) return 0.88 + 0.12 * ((1 - u) / e);
                return 1.0;
            }
            case Forma::mk67: {
                if (u < 0.03) return 0.35;                                     // mozzo dell'elica
                if (u < 0.15) return 0.40 + 0.60 * std::pow(rampa(u, 0.03, 0.15), 0.85);
                if (u > 0.95) return calotta((u - 0.95) / 0.05, 3.0);          // sezione mina, muso tondo
                return 1.0;
            }
            case Forma::captor: {
                if (u < 0.03) return 0.85 + 0.15 * rampa(u, 0.0, 0.03);
                if (u > 0.93) return calotta((u - 0.93) / 0.07, 2.2);          // calotta da cui esce il Mk 46
                return 1.0;
            }
            case Forma::pmr1: {
                const double e = m_(0.12);
                if (u < e) return calotta((e - u) / e, 5.0);
                if (u > 1 - e) return calotta((u - (1 - e)) / e, 4.0);
                return 1.0;
            }
            case Forma::esca_adc: {
                if (u < 0.10) return 0.95;                                     // anello dell'elica intubata
                if (u > 0.82) return calotta((u - 0.82) / 0.18, 2.0);
                return 0.85;
            }
            case Forma::esca_mg74: {   // siluro elettrico corto: cono di poppa di un metro, ogiva del trasduttore
                const double e = m_(1.0);
                if (u < e) return 0.26 + 0.74 * std::pow(rampa(u, 0.0, e), 0.85);
                if (u > 0.92) return calotta((u - 0.92) / 0.08, 2.2);
                return 1.0;
            }
            case Forma::esca_moss: {   // il corpo del Mk 37 da 10" (0,48 del manicotto)
                constexpr double K = 254.0 / 533.0;
                if (u < 0.03) return K * 0.35;
                if (u < 0.15) return K * (0.40 + 0.60 * std::pow(rampa(u, 0.03, 0.15), 0.85));
                if (u > 0.93) return K * calotta((u - 0.93) / 0.07, 2.4);
                return K;
            }
            case Forma::esca_mg84:
            case Forma::mina_fondo: {
                const double e = std::min(0.12, m_(0.18));
                if (u < e) return calotta((e - u) / e, 3.0);
                if (u > 1 - e) return calotta((u - (1 - e)) / e, 3.0);
                return 1.0;
            }
            case Forma::tigerfish:   // naso «a balena», coda che si stringe dagli ultimi 0,12
                if (u < 0.12) return 0.40 + 0.60 * std::pow(rampa(u, 0.0, 0.12), 0.85);
                if (u > 0.94) return calotta((u - 0.94) / 0.06, 1.8);
                return 1.0;
            case Forma::spearfish:   // come il Tigerfish, ma senza alette: il condotto del pump-jet in coda
                if (u < 0.04) return 0.92;
                if (u < 0.12) return 0.55 + 0.45 * std::pow(rampa(u, 0.04, 0.12), 0.85);
                if (u > 0.94) return calotta((u - 0.94) / 0.06, 1.8);
                return 1.0;
            case Forma::mk46:        // muso smussato, quasi piatto
                if (u < 0.15) return 0.42 + 0.58 * std::pow(rampa(u, 0.0, 0.15), 0.85);
                if (u > 0.95) return calotta((u - 0.95) / 0.05, 3.5);
                return 1.0;
            case Forma::aviolanciato_sov:
            case Forma::set40:       // a sigaro, muso tondo sul trasduttore
                if (u < 0.14) return 0.28 + 0.72 * std::pow(rampa(u, 0.0, 0.14), 0.85);
                if (u > 0.92) return calotta((u - 0.92) / 0.08, 2.0);
                return 1.0;
            case Forma::umgt1:       // l'idrogetto intubato al posto delle eliche
                if (u < 0.05) return 0.78;
                if (u < 0.16) return 0.50 + 0.50 * std::pow(rampa(u, 0.05, 0.16), 0.85);
                if (u > 0.92) return calotta((u - 0.92) / 0.08, 2.2);
                return 1.0;
            case Forma::apr2:        // razzo-siluro: ugello in coda, carenatura ogivale in testa
                if (u < 0.03) return 0.55;
                if (u < 0.08) return 0.70 + 0.30 * rampa(u, 0.03, 0.08);
                if (u > 0.82) return calotta(rampa(u, 0.82, 1.0), 1.5);
                return 1.0;
            case Forma::balena:      // peduncolo stretto, corpo pieno a meta', testa a «V» col rostro
                if (u < 0.05) return 0.20;                                     // le pinne caudali di taglio
                if (u < 0.45) return 0.12 + 0.88 * std::pow(rampa(u, 0.05, 0.45), 1.6);
                if (u > 0.65) return 1.0 - 0.70 * std::pow(rampa(u, 0.65, 1.0), 1.2);
                return 1.0;
        }
        return 1.0;
    }

    // materiale e tono aggiunto del corpo in (u, v): v = -1 in alto, +1 in basso
    void colore(double u, double v, M& m, double& dt) const {
        dt = 0.0;
        const bool es = s_.esercitazione;
        switch (f_) {
            case Forma::mk48:
            case Forma::mk48_adcap: {
                m = es && u < 0.62 ? M::arancione : M::verde;
                if (u < 0.05) m = M::grigio, dt = -0.15;
                if (u > 0.86) m = M::nero;
                if (dettagli())
                    for (const double g : {0.30, 0.62, 0.72, 0.86})
                        if (fascia(u, g, 0.08)) {
                            m = M::bianco;
                            // ADCAP: le fasce con i triangoli neri
                            if (f_ == Forma::mk48_adcap && R_ >= 4 && std::fabs(v) < 0.6 && static_cast<int>((v + 1) * R_) % 3 == 1) m = M::nero;
                        }
                return;
            }
            case Forma::sov_ogiva:
            case Forma::sov_piatto: {
                m = es ? (u > 0.78 ? M::rosso : M::grigio) : M::oliva;
                if (f_ == Forma::sov_piatto && u > 0.985 && std::fabs(v) < 0.55) m = M::nero, dt = 0.1;   // trasduttore
                if (dettagli())
                    for (const double g : {0.40, 0.78})
                        if (fascia(u, g, 0.03)) dt -= 0.35;   // giunti
                return;
            }
            case Forma::shkval:
                m = M::oliva;
                if (u < 0.04 || u > 0.985) m = M::nero;
                if (dettagli() && fascia(u, 0.92, 0.03)) dt = -0.35;
                return;
            case Forma::missile_sov:
                m = M::grigio, dt = 0.1;
                if (u < 0.03) m = M::nero;
                if (u > 0.86) m = M::oliva;
                return;
            case Forma::caps_harpoon:
                m = M::nero, dt = 0.15;
                if (dettagli() && (fascia(u, 0.90, 0.03) || fascia(u, 0.18, 0.03))) m = M::grigio, dt = -0.1;
                if (R_ >= 4 && u > 0.44 && u < 0.56 && v > -0.35 && v < 0.05) m = M::bianco, dt = -0.1;   // scritte
                return;
            case Forma::caps_tomahawk:
            case Forma::caps_generica: {
                m = f_ == Forma::caps_tomahawk ? M::argento : M::grigio;
                if (dettagli() && (u < m_(0.08) || u > 1 - m_(0.08))) dt = -0.25;
                // feritoie della capsula del Tomahawk: due file di asole
                if (f_ == Forma::caps_tomahawk && R_ >= 3) {
                    const double metri = u * L_ / px_m_;
                    const double fase = std::fmod(metri, 0.55);
                    if (u > 0.06 && u < 0.94 && fase < 0.26 && (std::fabs(v + 0.40) < 0.14 || std::fabs(v - 0.30) < 0.14)) m = M::nero, dt = 0.2;
                }
                return;
            }
            case Forma::mk67: {
                m = u > 0.70 ? M::oliva : M::verde_chiaro;
                if (dettagli()) {
                    if (fascia(u, 0.70, 0.05)) m = M::bianco;
                    if (fascia(u, 0.80, 0.10)) m = M::giallo;   // esplosivo
                }
                return;
            }
            case Forma::captor: {
                m = M::kaki;
                if (u < 0.22) dt = -0.15;                       // sezione dell'ancora
                if (u > 0.93) m = M::bronzo;                    // calotta
                if (dettagli())
                    for (const double g : {0.22, 0.45, 0.60})
                        if (fascia(u, g, g == 0.22 ? 0.05 : 0.12)) m = M::nero, dt = 0.1;   // giunto e fasce
                return;
            }
            case Forma::pmr1: {
                m = M::argento, dt = -0.08;
                if (u < 0.14) dt = -0.22;                       // ancora
                if (dettagli()) {
                    for (const double g : {0.14, 0.55})
                        if (fascia(u, g, 0.03)) dt = -0.6;      // giunti fra ancora, strumenti e razzo
                    // fori e ribattini sul fianco
                    const double metri = u * L_ / px_m_;
                    if (R_ >= 4 && std::fabs(v + 0.15) < 0.12 && std::fmod(metri, 0.45) < 0.08 && u > 0.16) m = M::grigio, dt = -0.5;
                }
                return;
            }
            case Forma::esca_adc:
                m = M::bianco;
                if (u < 0.10) m = M::nero;
                if (u > 0.82) m = M::arancione;
                return;
            case Forma::esca_mg84:
                m = M::grigio;
                if (dettagli() && fascia(u, 0.75, 0.10)) m = M::rosso;
                return;
            case Forma::esca_mg74:   // grigio con la fascia rossa dell'esca e il muso nero del trasduttore (STIMA)
                m = M::grigio, dt = 0.05;
                if (u > 0.92) m = M::nero;
                if (fascia(u, 0.62, 0.25)) m = M::rosso;
                if (dettagli() && fascia(u, 0.35, 0.03)) dt = -0.35;   // giunto della batteria
                return;
            case Forma::esca_moss:   // grigio scuro del corpo del Mk 37, muso nero del proiettore (STIMA)
                m = M::grigio, dt = -0.12;
                if (u > 0.93) m = M::nero;
                if (dettagli() && fascia(u, 0.55, 0.05)) m = M::bianco;
                return;
            case Forma::mina_fondo:
                m = M::oliva;
                if (dettagli() && fascia(u, 0.62, 0.12)) m = M::giallo;
                return;
            case Forma::tigerfish:   // verde scuro, ogiva nera; d'esercitazione la testa arancione (STIMA)
            case Forma::spearfish:
                m = es && u > (f_ == Forma::tigerfish ? 0.62 : 0.55) ? M::arancione : M::verde;
                dt = -0.12;
                if (u > 0.94) m = M::nero, dt = 0.0;
                if (f_ == Forma::spearfish && u < 0.04) m = M::grigio, dt = -0.15;   // il condotto
                if (dettagli())
                    for (const double g : {0.35, 0.62})
                        if (fascia(u, g, 0.04)) m = M::bianco, dt = -0.1;
                return;
            case Forma::mk46:        // grigio scuro con la fascia gialla della testata; esercitazione: testa arancione
                m = es && u > 0.62 ? M::arancione : M::grigio;
                if (m == M::grigio) dt = -0.2;
                if (u > 0.95) m = M::nero, dt = 0.0;
                if (dettagli() && fascia(u, 0.75, 0.06) && !es) m = M::giallo, dt = 0.0;
                return;
            case Forma::aviolanciato_sov:
            case Forma::umgt1:
            case Forma::apr2:
            case Forma::set40: {     // grigio chiaro (STIMA), muso nero sul trasduttore (SET-40: dalla fonte)
                const double testa = f_ == Forma::apr2 ? 0.82 : 0.78;
                m = es && u > testa ? M::rosso : M::grigio;
                if (m == M::grigio) dt = 0.1;
                if (f_ != Forma::apr2 && u > 0.92) m = M::nero, dt = 0.05;
                if (f_ == Forma::apr2 && u > 0.82 && !es) dt = -0.12;            // la carenatura di testa
                if ((f_ == Forma::apr2 && u < 0.03) || (f_ == Forma::umgt1 && u < 0.05)) m = M::nero, dt = 0.1;   // ugello, condotto
                if (dettagli())
                    for (const double g : {0.40, testa})
                        if (fascia(u, g, 0.03)) dt -= 0.35;   // giunti
                return;
            }
            case Forma::balena:      // dorso grigio scuro, ventre bianco; l'occhio sopra l'angolo della bocca
                if (v < 0.15) m = M::grigio, dt = -0.30;
                else m = M::bianco, dt = -0.15;
                if (dettagli() && u > 0.84 && u < 0.86 && std::fabs(v - 0.02) < 0.12) m = M::nero, dt = 0.0;
                return;
        }
        m = M::grigio;
    }

    void corpo() {
        for (int x = x0_; x < x1_; ++x) {
            const double u = u_di(x);
            if (u < 0.0 || u > 1.0) continue;
            const double rr = raggio(u) * R_;
            if (rr <= 0.0) continue;
            // la balena ha il dorso quasi dritto e il ventre pieno: sopra l'asse il profilo e' piu' basso
            const double rs = f_ == Forma::balena ? 0.72 * rr : rr;
            const int ya = static_cast<int>(std::floor(ya_ - rs + 0.5)), yb = static_cast<int>(std::ceil(ya_ + rr - 0.5));
            for (int y = ya; y < std::max(yb, ya + 1); ++y) {
                const double v = std::clamp((y + 0.5 - ya_) / std::max(y + 0.5 < ya_ ? rs : rr, 0.5), -1.0, 1.0);
                // luce dall'alto e di fronte, con il riflesso lungo il dorso
                const double nz = std::sqrt(std::max(0.0, 1 - v * v));
                double tono = 0.12 + 0.72 * std::clamp(-0.55 * v + 0.83 * nz, 0.0, 1.0);
                if (v > -0.62 && v < -0.36) tono += 0.18;
                tono *= 0.75 + 0.25 * raggio(u);   // estremita' piu' scure
                M m;
                double dt;
                colore(u, v, m, dt);
                metti(x, y, m, tono + dt);
            }
        }
    }

    // alette di poppa: la dorsale e la ventrale di fianco (trapezio con il bordo d'attacco inclinato), quelle
    // laterali di taglio come una riga sull'asse
    void alette(double ua, double ub, double punta, M m, double tono) {
        const int xa = x_di(ua), xb = x_di(ub);
        for (int x = xa; x <= xb; ++x) {
            const double u = u_di(x);
            const double f = std::clamp((ub - u) / std::max(ub - ua, 1e-6), 0.0, 1.0);
            const double h = (raggio(u) + (punta - raggio(u)) * std::min(1.0, f / 0.45)) * R_;
            for (int y = static_cast<int>(std::floor(ya_ - h + 0.5)); y < static_cast<int>(std::ceil(ya_ + h - 0.5)); ++y) {
                const double v = (y + 0.5 - ya_) / R_;
                if (std::fabs(v) <= raggio(u) && pieno(x, y)) continue;
                metti(x, y, m, tono - 0.1 * (v > 0));
            }
            if (R_ >= 3.0) metti(x, static_cast<int>(std::floor(ya_)), m, tono - 0.2);
        }
    }
    // pale dell'elica viste di fianco: una colonna in bronzo con le pale alterne
    void elica(double u, double apertura) {
        const int x = x_di(u);
        const double h = apertura * R_;
        for (int y = static_cast<int>(std::floor(ya_ - h + 0.5)); y < static_cast<int>(std::ceil(ya_ + h - 0.5)); ++y) {
            const double v = (y + 0.5 - ya_) / std::max(h, 0.5);
            metti(x, y, M::bronzo, 0.75 - 0.45 * std::fabs(v) + (v < 0 ? 0.15 : 0.0));
            if (R_ >= 4 && std::fabs(v) > 0.25) metti(x + (v < 0 ? -1 : 1), y, M::bronzo, 0.3);   // pala in scorcio
        }
    }

    void appendici() {
        switch (f_) {
            case Forma::mk48:
            case Forma::mk48_adcap:
                alette(0.05, 0.12, 0.92, s_.esercitazione ? M::arancione : M::verde, 0.4);
                if (dettagli()) {   // girante dentro il mantello
                    const int x = x_di(0.02);
                    for (int y = static_cast<int>(ya_ - 0.55 * R_); y <= static_cast<int>(ya_ + 0.55 * R_); ++y) metti(x, y, M::nero, 0.15);
                }
                return;
            case Forma::sov_ogiva:
            case Forma::sov_piatto: {
                const M m = s_.esercitazione ? M::grigio : M::oliva;
                alette(0.015, 0.11, 1.0, m, 0.45);
                // timoni dietro le alette, col telaio in punta
                if (dettagli()) {
                    const int xa = x_di(0.0), xb = x_di(0.04);
                    for (int x = xa; x <= xb; ++x) {
                        metti(x, static_cast<int>(std::floor(ya_ - R_ + 0.5)), m, 0.55);
                        metti(x, static_cast<int>(std::ceil(ya_ + R_ - 1.5)), m, 0.3);
                    }
                }
                elica(0.008, 0.62);
                elica(0.024, 0.62);   // due eliche controrotanti
                return;
            }
            case Forma::shkval: alette(0.04, 0.09, 0.8, M::oliva, 0.4); return;
            case Forma::missile_sov: alette(0.04, 0.14, 0.92, M::grigio, 0.35); return;
            case Forma::caps_harpoon: alette(0.01, 0.12, 1.0, M::nero, 0.35); return;
            case Forma::mk67:
                alette(0.03, 0.12, 1.0, M::verde_chiaro, 0.4);
                elica(0.012, 0.75);
                return;
            case Forma::esca_adc: return;
            case Forma::esca_mg74:
                alette(0.015, 0.13, 1.0, M::grigio, 0.45);
                elica(0.008, 0.62);
                elica(0.024, 0.62);   // eliche controrotanti del siluro elettrico
                return;
            case Forma::esca_moss: {
                constexpr double K = 254.0 / 533.0;
                alette(0.03, 0.12, K, M::grigio, 0.35);
                elica(0.012, K * 0.75);
                // il manicotto da 533 mm: due guide lungo il corpo e tre anelli di sostegno, piu' il fondo di poppa
                const int xa = x_di(0.0), xb = x_di(1.0);
                const int yt = static_cast<int>(std::floor(ya_ - 0.94 * R_ + 0.5)), yb = static_cast<int>(std::ceil(ya_ + 0.94 * R_ - 0.5)) - 1;
                for (int x = xa; x <= xb; ++x) {
                    if (!pieno(x, yt)) metti(x, yt, M::grigio, 0.55);
                    if (!pieno(x, yb)) metti(x, yb, M::grigio, 0.25);
                }
                for (const double g : {0.02, 0.22, 0.60, 0.97}) {
                    const int x = x_di(g);
                    for (int y = yt; y <= yb; ++y)
                        if (!pieno(x, y) || g < 0.05) metti(x, y, M::giallo, y < ya_ ? 0.6 : 0.35);
                }
                return;
            }
            case Forma::tigerfish:   // quattro alette rettangolari raccordate, due eliche controrotanti
                alette(0.025, 0.10, 0.95, M::verde, 0.3);
                elica(0.008, 0.62);
                elica(0.022, 0.62);
                return;
            case Forma::mk46:
                alette(0.03, 0.09, 0.85, M::grigio, 0.3);
                elica(0.008, 0.55);
                elica(0.022, 0.55);
                return;
            case Forma::aviolanciato_sov:   // alette coi timoni, eliche coassiali
                alette(0.015, 0.11, 1.0, M::grigio, 0.45);
                elica(0.008, 0.62);
                elica(0.024, 0.62);
                return;
            case Forma::set40:       // alette coi timoni, un'elica (ru.wikipedia)
                alette(0.015, 0.11, 1.0, M::grigio, 0.45);
                elica(0.010, 0.62);
                return;
            case Forma::umgt1: alette(0.06, 0.13, 0.85, M::grigio, 0.4); return;
            case Forma::apr2: alette(0.03, 0.12, 1.0, M::grigio, 0.4); return;
            case Forma::balena: {    // la pinna dorsale piccola e uncinata, a 0,72 della lunghezza dal muso
                const int xa = x_di(0.24), xb = x_di(0.30);
                for (int x = xa; x <= xb; ++x) {
                    const double f = rampa(u_di(x), 0.24, 0.30);
                    const double h = 0.35 * R_ * (f < 0.75 ? f / 0.75 : (1.0 - f) / 0.25);
                    const double top = ya_ - 0.72 * raggio(u_di(x)) * R_;
                    for (int y = static_cast<int>(std::floor(top - h)); y < static_cast<int>(std::ceil(top)); ++y) metti(x, y, M::grigio, 0.15);
                }
                return;
            }
            default: return;
        }
    }

    // contorno scuro come negli sprite: i pixel di bordo si abbassano di tono
    void contorno() {
        std::vector<Cella> c = c_;
        for (int y = 0; y < h_; ++y)
            for (int x = 0; x < w_; ++x) {
                auto& k = c[static_cast<std::size_t>(y * w_ + x)];
                if (k.m < 0) continue;
                const int X = x0_ + x, Y = y0_ + y;
                if (!pieno(X - 1, Y) || !pieno(X + 1, Y) || !pieno(X, Y + 1)) k.tono = std::min(k.tono, 0.08f);
                else if (!pieno(X, Y - 1)) k.tono = std::min(k.tono + 0.1f, 1.0f);   // filo di luce in alto
            }
        c_ = std::move(c);
        // CAPTOR: la finestra sul Mk 46 dentro la capsula (spaccato da tavola tecnica)
        if (f_ == Forma::captor && R_ >= 4.5) {
            const int xa = x_di(0.30), xb = x_di(0.88);
            const double hf = 0.70 * R_;
            for (int x = xa; x <= xb; ++x)
                for (int y = static_cast<int>(std::floor(ya_ - hf)); y <= static_cast<int>(std::ceil(ya_ + hf)) - 1; ++y) {
                    const bool bordo = x == xa || x == xb || y == static_cast<int>(std::floor(ya_ - hf)) || y == static_cast<int>(std::ceil(ya_ + hf)) - 1;
                    metti(x, y, bordo ? M::kaki : M::nero, bordo ? 0.95 : 0.05);
                }
            // il Mk 46 (324 mm) con la prua verso la calotta
            const double r46 = 0.324 / 0.533 * 0.5 * R_ * 1.0;
            for (int x = xa + 2; x <= xb - 1; ++x) {
                const double u = static_cast<double>(x - xa - 2) / std::max(xb - xa - 3, 1);
                const double rr = r46 * (u > 0.88 ? calotta((u - 0.88) / 0.12, 2.0) : u < 0.1 ? 0.6 + 0.4 * u / 0.1 : 1.0);
                for (int y = static_cast<int>(std::floor(ya_ - rr + 0.5)); y < static_cast<int>(std::ceil(ya_ + rr - 0.5)); ++y) {
                    const double v = (y + 0.5 - ya_) / std::max(rr, 0.5);
                    metti(x, y, u > 0.88 ? M::nero : M::verde, 0.25 + 0.6 * (1 - std::fabs(v + 0.3)));
                }
            }
            elica(u_di(xa + 2) - 0.004, 0.42);
        }
    }
};

}  // namespace

std::uint8_t TavolozzaSagome::indice(MaterialeArma m, double tono, int x, int y) const {
    const auto& r = rampe[static_cast<std::size_t>(m)];
    const int n = static_cast<int>(r.size());
    return r[static_cast<std::size_t>(std::clamp(cerimonia::retina(std::clamp(tono, 0.0, 1.0) * (n - 1), x, y, n - 1), 0, n - 1))];
}

const TavolozzaSagome& TavolozzaSagome::fissa() {
    // indici della palette del giorno che INDICI_VISTA non illumina: EGA 0-15, interfaccia 80-95, 160-195, 254
    static const TavolozzaSagome t{{{
        {0, 88, 87},                // nero
        {0, 88, 86, 84, 82, 80, 15},   // grigio
        {87, 85, 83, 82, 81, 80, 15},  // argento
        {84, 82, 80, 15},           // bianco
        {0, 254, 254, 2},           // verde
        {0, 189, 87, 191},          // oliva: oliva scuro, grigio e verde giallo retinati
        {254, 3, 7, 160},           // verde chiaro
        {0, 189, 88, 86, 84},       // kaki
        {6, 92, 93},                // arancione
        {0, 4, 91, 12},             // rosso
        {6, 93, 94, 14},            // giallo
        {6, 188, 186, 185},         // bronzo
        {0, 1, 9},                  // acqua
    }}};
    return t;
}

TavolozzaSagome TavolozzaSagome::ricca(Palette& p, int primo) {
    using RGB = std::array<std::uint8_t, 3>;
    static const std::array<std::vector<RGB>, N_MATERIALI_ARMA> rgb = {{
        {{10, 10, 12}, {34, 36, 40}, {66, 70, 76}},
        {{24, 26, 30}, {52, 56, 62}, {86, 92, 100}, {124, 130, 138}, {168, 174, 180}, {220, 224, 228}},
        {{90, 96, 104}, {150, 156, 164}, {200, 206, 212}, {245, 248, 250}},
        {{150, 150, 145}, {205, 205, 198}, {250, 250, 245}},
        {{10, 34, 20}, {22, 62, 36}, {40, 96, 56}, {78, 136, 90}},
        {{34, 38, 18}, {62, 68, 34}, {96, 102, 56}, {140, 146, 92}},
        {{52, 96, 90}, {100, 160, 150}, {160, 210, 200}},
        {{60, 58, 44}, {100, 96, 74}, {140, 134, 106}, {184, 178, 148}},
        {{110, 36, 6}, {196, 72, 14}, {244, 120, 30}, {255, 182, 90}},
        {{110, 10, 8}, {196, 28, 20}, {250, 86, 64}},
        {{150, 110, 10}, {230, 190, 30}, {255, 240, 120}},
        {{96, 56, 20}, {176, 112, 44}, {236, 184, 96}},
        {{4, 16, 40}, {8, 36, 76}, {18, 64, 118}, {60, 120, 170}},
    }};
    TavolozzaSagome t;
    int i = primo;
    for (std::size_t m = 0; m < rgb.size(); ++m)
        for (const auto& c : rgb[m]) {
            p.imposta(i, c[0], c[1], c[2]);
            t.rampe[m].push_back(static_cast<std::uint8_t>(i++));
        }
    return t;
}

SagomaArma sagoma_da_scheda(const SchedaArma& a, bool esercitazione) {
    SagomaArma s;
    s.id = a.id;
    s.tipo = a.tipo;
    s.lunghezza_m = a.lunghezza_m;
    s.diametro_mm = a.diametro_mm > 0 ? a.diametro_mm : 533;
    s.sovietica = a.nazione == "urss";
    s.esercitazione = esercitazione;
    return s;
}

SagomaArma sagoma_da_id(const std::string& id, bool esercitazione) {
    SagomaArma s;
    s.id = id;
    if (id == "sea_lance" || id == "harpoon" || id == "tasm" || id == "tlam_c") s.tipo = TipoArmaCarico::missile;
    const Forma f = forma_di(s);
    s.diametro_mm = diametro_forma(f, id);
    if (id == "at2") s.lunghezza_m = 5.0;   // 4,8-5,2 m nelle fonti
    s.sovietica = f == Forma::sov_ogiva || f == Forma::sov_piatto || f == Forma::aviolanciato_sov || f == Forma::umgt1 ||
                  f == Forma::apr2 || f == Forma::set40;
    s.esercitazione = esercitazione;
    return s;
}

bool sagoma_nota(const std::string& id) {
    SagomaArma s;
    s.id = id;
    s.tipo = TipoArmaCarico::esca;   // una forma che nessuna voce sconosciuta prende per tipo...
    const Forma a = forma_di(s);
    s.tipo = TipoArmaCarico::mina;   // ...e un'altra: se le due coincidono la forma viene dal nome
    return a == forma_di(s);
}

double lunghezza_disegnata(const SagomaArma& s) {
    const Forma f = forma_di(s);
    const double l = s.lunghezza_m > 0.0 ? s.lunghezza_m : lunghezza_forma(f);
    switch (f) {
        case Forma::caps_harpoon: return std::max(l, 5.8);           // la capsula e' lunga come un siluro
        case Forma::caps_tomahawk:
        case Forma::caps_generica: return std::max(l, 6.25) + 0.15;  // missile col booster e i fondi della capsula
        default: return l;
    }
}

void traccia_sagoma_arma(const SagomaArma& s, double x_poppa, double y_asse, double px_m, const PuntoSagoma& punto) {
    if (px_m <= 0.0) return;
    Tracciato(s, x_poppa, y_asse, px_m).traccia(punto);
}

Rettangolo disegna_sagoma_arma(Tela& t, const SagomaArma& s, Rettangolo r, double px_m, const TavolozzaSagome& tav) {
    const double L = lunghezza_disegnata(s), D = diametro_disegnato(s) / 1000.0;
    if (px_m <= 0.0) px_m = std::min((r.w - 2) / L, (r.h - 2) / std::max(D, 0.05));
    const double x_poppa = r.x + 0.5 * (r.w - L * px_m), y_asse = r.y + 0.5 * r.h;
    int xa = 1 << 30, ya = 1 << 30, xb = -(1 << 30), yb = -(1 << 30);
    traccia_sagoma_arma(s, x_poppa, y_asse, px_m, [&](int x, int y, MaterialeArma m, double tono) {
        if (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h) return;
        t.pixel(x, y, tav.indice(m, tono, x, y));
        xa = std::min(xa, x), ya = std::min(ya, y), xb = std::max(xb, x), yb = std::max(yb, y);
    });
    if (xb < xa) return {r.x + r.w / 2, r.y + r.h / 2, 0, 0};
    return {xa, ya, xb - xa + 1, yb - ya + 1};
}

// --- tubo ---

GeometriaTubo geometria_tubo(Rettangolo r, const SagomaInTubo& s) {
    GeometriaTubo g;
    const double L = s.arma.id.empty() ? 5.8 : lunghezza_disegnata(s.arma);
    const double tubo = std::max(L + 0.5, 6.7);
    const double sinistra = 1.0, destra = 0.6;   // culatta col vassoio, volata col fasciame e il mare
    g.px_m = std::min(r.w / (tubo + sinistra + destra), (r.h - 3) / 0.95);
    const double largo = (tubo + sinistra + destra) * g.px_m;
    g.x_culatta = r.x + 0.5 * (r.w - largo) + sinistra * g.px_m;
    g.x_volata = g.x_culatta + tubo * g.px_m;
    g.y_asse = r.y + 0.5 * r.h;
    const double f = s.stato == FaseTubo::ricarica ? std::clamp(s.ricarica, 0.0, 1.0) : 1.0;
    g.x_poppa_arma = g.x_culatta + 0.2 * g.px_m - (1.0 - f) * (L + 0.4) * g.px_m;
    return g;
}

void disegna_tubo(Tela& t, Rettangolo r, const SagomaInTubo& s, const TavolozzaSagome& tav) {
    const GeometriaTubo g = geometria_tubo(r, s);
    const double Rb = 0.31 * g.px_m, parete = std::max(1.5, 0.08 * g.px_m);
    const auto metti = [&](int x, int y, M m, double tono) {
        if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) t.pixel(x, y, tav.indice(m, tono, x, y));
    };
    const int xc = static_cast<int>(std::lround(g.x_culatta)), xv = static_cast<int>(std::lround(g.x_volata));
    const int yt = static_cast<int>(std::floor(g.y_asse - Rb)), yb = static_cast<int>(std::ceil(g.y_asse + Rb)) - 1;   // vano
    const int pt = static_cast<int>(std::ceil(parete));
    const bool ricarica = s.stato == FaseTubo::ricarica;
    const bool vuoto = s.arma.id.empty() || s.stato == FaseTubo::vuoto;

    // fondo del locale, fasciame esterno e mare oltre la volata
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            if (x > xv + pt) {
                const bool fasciame = x <= xv + pt + std::max(2, static_cast<int>(0.2 * g.px_m));
                metti(x, y, fasciame ? M::grigio : M::acqua, fasciame ? 0.3 + 0.1 * ((y & 3) == 0) : 0.35 + 0.15 * std::sin(0.4 * y + 2 * s.tempo_s));
            } else {
                // costole dello scafo resistente ogni 0,6 m dietro il tubo
                const bool costola = std::fmod(x - r.x + 1000.0 * g.px_m, std::max(4.0, 0.6 * g.px_m)) < std::max(1.0, 0.06 * g.px_m);
                metti(x, y, M::grigio, 0.08 + 0.05 * (y > g.y_asse + Rb + pt) + 0.1 * costola);
            }
        }
    // vassoio di carico davanti alla culatta, coi rulli
    for (int x = r.x; x < xc; ++x) {
        metti(x, yb + 1, M::grigio, 0.6);
        if ((x - r.x) % std::max(3, static_cast<int>(0.4 * g.px_m)) == 0) metti(x, yb + 2, M::grigio, 0.35);
    }
    // vano del tubo: asciutto, o allagato con l'acqua che ondeggia e le bolle
    for (int y = yt; y <= yb; ++y)
        for (int x = xc; x <= xv; ++x) {
            if (s.allagato) metti(x, y, M::acqua, 0.25 + 0.2 * (y - yt) / std::max(1, yb - yt) * 0 + 0.12 * std::sin(0.5 * x - 3 * s.tempo_s));
            else metti(x, y, M::nero, 0.05 + 0.15 * (y > g.y_asse));
        }
    // arma: dalla rastrelliera nel tubo durante la ricarica
    if (!vuoto) {
        traccia_sagoma_arma(s.arma, g.x_poppa_arma, g.y_asse, g.px_m, [&](int x, int y, M m, double tono) {
            if (x < r.x || x >= r.x + r.w) return;
            // sott'acqua il colore passa attraverso un velo d'acqua
            if (s.allagato && x >= xc && x <= xv && ((x + 2 * y) & 3) == 0) return metti(x, y, M::acqua, 0.45);
            metti(x, y, m, tono);
        });
    }
    if (s.allagato)   // bolle che salgono
        for (int k = 0; k < 6; ++k) {
            const double fx = cerimonia::caso(k, 7, 3);
            const double fy = std::fmod(cerimonia::caso(k, 9, 3) + 0.6 * s.tempo_s * (0.5 + fx), 1.0);
            metti(xc + static_cast<int>(fx * (xv - xc)), yb - static_cast<int>(fy * (yb - yt)), M::acqua, 1.0);
        }
    // pareti del tubo tagliate per il lungo: in alto in luce, in basso in ombra
    for (int k = 0; k < pt; ++k)
        for (int x = xc; x <= xv; ++x) {
            metti(x, yt - 1 - k, M::grigio, 0.75 - 0.12 * k + 0.1 * (k == pt - 1));
            metti(x, yb + 1 + k, M::grigio, 0.35 - 0.06 * k);
        }
    // culatta: chiusa sul vano, o aperta e girata di fianco durante la ricarica
    {
        const int sp = std::max(2, static_cast<int>(0.12 * g.px_m));
        const int x0 = ricarica ? xc - std::max(sp + 2, static_cast<int>(Rb * 1.4)) : xc - sp;
        const int x1 = ricarica ? x0 + 1 : xc - 1;
        for (int y = yt - pt - 1; y <= yb + pt + 1; ++y)
            for (int x = x0; x <= x1; ++x) metti(x, y, M::grigio, (ricarica ? 0.55 : 0.7) - 0.25 * (x == x1) + 0.15 * (y < g.y_asse));
        if (!ricarica)   // maniglia gialla
            for (int y = static_cast<int>(g.y_asse) - 1; y <= static_cast<int>(g.y_asse) + 1; ++y) metti(x0 - 1, y, M::giallo, 0.8);
    }
    // porta esterna: chiusa sulla volata o fatta scorrere via
    {
        const int sp = std::max(1, static_cast<int>(0.08 * g.px_m));
        const int ya = s.porta_aperta ? yt - pt - (yb - yt) - 2 : yt - pt, yz = s.porta_aperta ? yt - pt - 2 : yb + pt;
        for (int y = ya; y <= yz; ++y)
            for (int x = xv + 1; x <= xv + sp; ++x) metti(x, y, M::grigio, 0.5 - 0.15 * (x == xv + sp) + 0.1 * (y < g.y_asse));
    }
    // spia: verde carico, ambra in armamento e in ricarica (lampeggia), rossa armato
    const int lato = std::max(2, std::min(8, r.h / 9));
    const int lx = r.x + 2, ly = r.y + 2;
    const bool acceso = std::fmod(s.tempo_s, 1.0) < 0.5;
    M ml = M::grigio;
    double tl = 0.3;
    switch (s.stato) {
        case FaseTubo::carico: ml = M::verde, tl = 1.0; break;
        case FaseTubo::ricarica:
        case FaseTubo::armamento: ml = acceso ? M::giallo : M::grigio, tl = acceso ? 0.9 : 0.3; break;
        case FaseTubo::armato: ml = M::rosso, tl = 1.0; break;
        default: break;
    }
    for (int y = 0; y < lato; ++y)
        for (int x = 0; x < lato; ++x) metti(lx + x, ly + y, ml, tl - 0.3 * (x == lato - 1 || y == lato - 1));
    // mina armata: il segno della mina pronta alla posa (sfera con i cornetti) accanto alla spia
    if (s.stato == FaseTubo::armato && s.arma.tipo == TipoArmaCarico::mina && !vuoto) {
        const double rr = std::max(1.5, lato * 0.6);
        const double cx = lx + lato + 3 + rr + 1, cy = ly + lato * 0.5;
        const double luce = acceso ? 1.0 : 0.6;
        for (int y = static_cast<int>(cy - rr - 2); y <= static_cast<int>(cy + rr + 2); ++y)
            for (int x = static_cast<int>(cx - rr - 2); x <= static_cast<int>(cx + rr + 2); ++x) {
                const double dx = x + 0.5 - cx, dy = y + 0.5 - cy, d = std::hypot(dx, dy);
                const bool corno = d <= rr + 1.6 && (std::fabs(dx) < 0.6 || std::fabs(dy) < 0.6);
                if (d <= rr) metti(x, y, M::giallo, luce * (0.55 + 0.4 * (dy < 0)));
                else if (corno) metti(x, y, M::nero, 0.1);
            }
    }
}

}  // namespace m688
