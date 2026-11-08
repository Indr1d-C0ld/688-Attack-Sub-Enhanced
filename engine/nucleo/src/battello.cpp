#include "nucleo/battello.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>

#include "nucleo/collisioni.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double RHO = 1025.0, G = 9.81, P_ATM = 101325.0;
constexpr double CD = 0.62;                 // spigolo vivo, come la tabella del dossier 02 §9.1
constexpr double PASSO = 0.1;               // s
constexpr double FRAZIONE_PARATIE = 0.3;    // paratie: cedono a 0,3 volte la pressione di collaudo (stima)
constexpr double AREA_PARATIA = 1.0;        // m^2 di passaggio di una paratia ceduta (stima)
constexpr double ARIA_CASSE = 2.5;          // aria delle bombole: 2,5 volte le casse a 1 bar (stima, docs/10 §10.3)
constexpr double TEMPO_ESAURIMENTO = 30.0;  // s per svuotare tutte le casse in superficie (stima)
constexpr double CD_VERTICALE = 1.0;        // resistenza al moto verticale sulla pianta (cilindro di traverso)
// equipaggio (stime di gioco, docs/10 §10.3): perdite tamponabili fino a ~2,4" di diametro, piu' lente in profondita';
// pompe che a 200 m non rendono piu'; un sistema si ripara da zero in 15 minuti per squadra; SCRAM di 5-20 minuti
// con il motore d'emergenza al 15% della velocita' massima (dossier 02 §9.2)
constexpr double AREA_TAMPONABILE = 0.003, TAMPONATURA = 5e-5;   // m^2, m^2/s a quota zero
constexpr double PORTATA_POMPE = 0.03, QUOTA_POMPE = 200.0;     // m^3/s, m
constexpr double RIPARAZIONE = 1.0 / 900.0;                      // efficienza al secondo per squadra
constexpr double VELOCITA_EMERGENZA = 0.15;
constexpr double SFOGO_S = 15.0;   // sfoghi: una cassa piena d'aria si svuota in 15 s (STIMA 10-20 s, dossier 19 §3.2)
constexpr double PORTATA_ASSETTO = 0.05;   // m^3/s dalle valvole delle casse d'assetto (stima: 3 t al minuto)
// incendi ed esplosioni interne (stime di gioco, docs/10 §10.13)
constexpr double CRESCITA_FUOCO = 0.693 / 120.0;   // raddoppia in 2 minuti, come sulle navi (D3)
constexpr double OSSIGENO_MINIMO = 0.12;           // sotto, il fuoco si spegne
constexpr double CONSUMO_OSSIGENO = 0.15;          // m^3 di frazione d'ossigeno al secondo a fuoco pieno (1000 m^3: dal 21% al 12% in 10 min)
constexpr double FUMO_PIENO_S = 180.0;             // s di fuoco pieno per riempire di fumo 1000 m^3
constexpr double VENTILAZIONE_S = 600.0;           // costante di tempo del fumo che si dirada a fuoco spento
constexpr double ARIA_NEL_FUOCO = 5.0;             // m^3 normali/s che una tubolatura rotta da' all'incendio
constexpr double SPEGNIMENTO = 6e-4;               // intensita' al secondo per squadra (nel fumo la meta')
constexpr double TENUTE_S = 900.0;                 // s di fuoco forte per bruciare una tenuta dello scafo
constexpr double AREA_TENUTA = 0.002;              // m^2 (circa 2 pollici)
constexpr double SOVRAPPRESSIONE_PARATIE = 0.45;   // un'esplosione fa cedere una paratia oltre 0,45 x la sua resistenza
                                                   // (tarata sul Kursk: 2 t nel compartimento 1 sfondano fino al 4)
constexpr double SQUARCIO_KG = 50.0;               // oltre, l'esplosione interna squarcia lo scafo resistente
// danni critici, fase 3 (STIMA, kit 31/10/2026)
constexpr double CARICA_MAX_KG = 30000.0;          // uno scoppio solo: al piu' 30 t, come le navi (danni.cpp)
constexpr double REAZIONE_LIQUIDO_KG = 100.0;      // il propellente liquido di un missile che reagisce nel silo (K-219, 1986):
                                                   // fa saltare il portello e squarcia il silo, non e' una detonazione
constexpr double REAZIONE_S = 600.0;               // con l'acqua nel silo, reagisce in ~10 minuti (K-219: una perdita di ore)
constexpr double BATTERIA_S = 7200.0;              // la batteria di riserva di un nucleare: 2 ore al motore d'emergenza
// isolamento di un compartimento (STIMA, docs/10 §10.19)
constexpr double SGOMBERO_S = 60.0;                // fuori tutti, poi le porte
constexpr double CONTAGIO_ISOLATO = 0.25;          // il fuoco passa meno ai vicini, ma passa (Komsomolets, 1989)
constexpr double OSSIGENO_RIAPERTO = 0.16;         // riaprendo a fuoco vivo
constexpr int UOMINI_MACCHINE = 19, UOMINI_LOCALI = 10;   // di guardia, sui 129 del 688 (dossier 01); reattore e batterie vuoti

double combustibile_per(const std::string& uso) {   // s di fuoco pieno
    if (uso == "macchine") return 3600;
    if (uso == "deposito") return 1500;
    if (uso == "batterie") return 2400;
    if (uso == "reattore") return 1200;
    return 1800;
}

// Poligoni dello scafo resistente in una stazione: uno o due cerchi (scafi affiancati a +-y)
std::vector<Poligono> cerchi(const Stazione& s) {
    std::vector<Poligono> out;
    if (s.r_resistente <= 0) return out;
    for (const double y : s.y_resistente > 0 ? std::vector<double>{-s.y_resistente, s.y_resistente} : std::vector<double>{0.0}) {
        Poligono p;
        for (int k = 0; k < 32; ++k) {
            const double a = 2 * PI * k / 32;
            p.push_back({y + s.r_resistente * std::cos(a), s.z_resistente + s.r_resistente * std::sin(a)});
        }
        out.push_back(std::move(p));
    }
    return out;
}

// Area di un poligono fra y0 e y1
double area_fra(const Poligono& p, double y0, double y1) { return area_poligono(taglia(taglia(p, -1, 0, -y0), 1, 0, y1)); }

}  // namespace

const char* nome_sistema(Sistema s) {
    static const char* NOMI[] = {"calcolatore del sonar", "schiera", "periscopio", "sonar di tiro", "motori", "antenna",
                                 "idroplani", "timone", "propulsione", "aria ad alta pressione", "pompe", "sonar di prua",
                                 "navigatore inerziale"};
    const auto i = static_cast<std::size_t>(s);
    return i < std::size(NOMI) ? NOMI[i] : "?";
}

const char* chiave_sistema(Sistema s) {
    static const char* CHIAVI[] = {"calcolatore_sonar", "schiera", "periscopio", "sonar_tiro", "motori", "antenna",
                                   "idroplani", "timone", "propulsione", "aria", "pompe", "sonar_prua", "inerziale"};
    static_assert(std::size(CHIAVI) == static_cast<std::size_t>(Sistema::numero));
    const auto i = static_cast<std::size_t>(s);
    return i < std::size(CHIAVI) ? CHIAVI[i] : "?";
}

std::optional<Sistema> sistema_da_chiave(std::string_view chiave) {
    for (int i = 0; i < static_cast<int>(Sistema::numero); ++i)
        if (chiave == chiave_sistema(static_cast<Sistema>(i))) return static_cast<Sistema>(i);
    return std::nullopt;
}

BattelloDanni::BattelloDanni(const DatiScafo& dati, std::uint64_t seme) : dati_(dati), caso_(seme ^ 0xd6d6u), caso_inerziale_(seme ^ 0x1e7a1u) {
    if (!dati.sottomarino.presente) throw std::runtime_error("BattelloDanni: lo scafo non e' di un sottomarino");
    const double dx = dati.passo;
    std::vector<std::pair<double, double>> y_comp;   // intervallo di y di ogni compartimento (per le paratie)
    for (const auto& cs : dati.celle) {
        double v = 0, vx = 0;
        for (const auto& s : dati.stazioni) {
            if (s.x < cs.x0 || s.x >= cs.x1) continue;
            double resistente = 0;
            for (const auto& c : cerchi(s)) resistente += area_fra(c, cs.y0, cs.y1);
            const double a = cs.uso == "zavorra" ? std::max(0.0, area_fra(s.sezione(), cs.y0, cs.y1) - resistente) : resistente;
            v += a * dx; vx += a * dx * s.x;
        }
        if (v < 1.0) continue;   // fuori dallo scafo resistente (silos del Typhoon) o cassa vuota
        if (cs.uso == "zavorra") {
            casse_.push_back({cs.nome, vx / v, v * cs.permeabilita, 0.0, false});
        } else {
            CompartimentoBattello c;
            c.nome = cs.nome; c.uso = cs.uso; c.x0 = cs.x0; c.x1 = cs.x1; c.xc = vx / v;
            c.volume = v * cs.permeabilita;
            c.carica_kg = cs.carica_kg;
            c.propellente_kg = cs.propellente_kg;
            c.protezione = cs.protezione;
            if (cs.carburante == "propellente_liquido") c.liquido_t = cs.carburante_t;
            // uno scafo senza i dati dei danni critici: la vecchia stima, 300 kg nel locale siluri
            if (cs.uso == "deposito" && c.carica_kg <= 0 && c.propellente_kg <= 0 && c.liquido_t <= 0) c.carica_kg = 300.0;
            comp_.push_back(c);
            y_comp.push_back({cs.y0, cs.y1});
        }
    }
    if (comp_.empty()) throw std::runtime_error("BattelloDanni: nessun compartimento nello scafo resistente");
    // i contenitori fra i due scafi a tratti di 5 m, con la carica per lunghezza: salta solo quello vicino allo scoppio
    for (const auto& z : dati.fuori_resistente) {
        if (z.carica_kg <= 0 || z.x1 <= z.x0) continue;
        const int n = std::max(1, static_cast<int>(std::lround((z.x1 - z.x0) / 5.0)));
        const double l = (z.x1 - z.x0) / n;
        for (int i = 0; i < n; ++i) contenitori_.push_back({z.x0 + i * l, z.x0 + (i + 1) * l, z.carica_kg / n});
    }
    // paratie fra compartimenti contigui (stesso scafo: intervalli di y che si sovrappongono)
    const double p_paratia = FRAZIONE_PARATIE * RHO * G * dati.sottomarino.quota_collaudo_m;
    for (std::size_t i = 0; i < comp_.size(); ++i)
        for (std::size_t j = i + 1; j < comp_.size(); ++j) {
            const bool contigui = std::fabs(comp_[i].x1 - comp_[j].x0) < 1e-3 || std::fabs(comp_[i].x0 - comp_[j].x1) < 1e-3;
            const bool stesso_scafo = std::min(y_comp[i].second, y_comp[j].second) > std::max(y_comp[i].first, y_comp[j].first);
            if (contigui && stesso_scafo) paratie_.push_back({static_cast<int>(i), static_cast<int>(j), p_paratia, false});
        }
    double v = 0, vx = 0;
    for (const auto& c : comp_) { v += c.volume; vx += c.volume * c.xc; }
    x_riferimento_ = vx / v;
    for (const auto& s : dati.stazioni) {
        double hb = 0;
        for (double h : s.semilarghezze) hb = std::max(hb, h);
        area_pianta_ += 2 * hb * dx;
    }
    double casse = 0;
    for (const auto& c : casse_) casse += c.volume;
    aria_bombole_ = ARIA_CASSE * casse;
    portata_aria_ = casse / TEMPO_ESAURIMENTO;
    // quota di collasso vera: quella prevista (1,5 volte il collaudo) per 1,1-1,8, in media +45% come nella regola di
    // Rule (dossier 07 §8.2: Thresher 730 m contro 590 previsti, x 1,24; K-8 615 contro 380, x 1,62)
    quota_collasso_ = dati.sottomarino.quota_collasso_m * caso_.uniforme(1.1, 1.8);
    for (auto& c : comp_) {
        nucleare_ |= c.uso == "reattore";
        c.combustibile = combustibile_per(c.uso);
    }
    // priorita' delle riparazioni (dossier 02 §9.2): propulsione, governo, aria e pompe, sensori, comunicazioni
    priorita_ = {Sistema::propulsione, Sistema::motori, Sistema::idroplani, Sistema::timone, Sistema::aria, Sistema::pompe,
                 Sistema::sonar_prua, Sistema::calcolatore_sonar, Sistema::sonar_tiro, Sistema::schiera, Sistema::periscopio,
                 Sistema::inerziale, Sistema::antenna};
}

void BattelloDanni::imbarca(int compartimento, double m3) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size()) || m3 <= 0.0 || perso_) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.imploso) return;
    c.acqua = std::min(c.volume * 0.98, c.acqua + m3);
    // la prima volta, il rapporto (anche se le pompe ce la fanno)
    const bool riferito = std::any_of(eventi_.begin(), eventi_.end(), [&](const EventoBattello& e) {
        return e.tipo == EventoBattello::Tipo::allagamento && e.compartimento == compartimento;
    });
    if (!riferito) evento(EventoBattello::Tipo::allagamento, compartimento, "acqua dalla canna del periscopio nel compartimento " + c.nome);
}

int BattelloDanni::compartimento_vicino(double x) const {
    int k = 0;
    for (std::size_t i = 0; i < comp_.size(); ++i)
        if (std::fabs(comp_[i].xc - x) < std::fabs(comp_[static_cast<std::size_t>(k)].xc - x)) k = static_cast<int>(i);
    return k;
}

void BattelloDanni::danneggia(Sistema s, double perdita) {
    auto& st = sistemi_[static_cast<std::size_t>(s)];
    if (perdita <= 0 || st.efficienza <= 0) return;
    st.efficienza = std::max(0.0, st.efficienza - perdita);
    if (perdita > 0.7) st.massimo = std::min(st.massimo, std::max(0.5, 1.3 - perdita));   // danno grave: parte resta
    char t[96];
    std::snprintf(t, sizeof t, "guasto: %s al %.0f%%", nome_sistema(s), 100 * st.efficienza);
    evento(EventoBattello::Tipo::guasto, -1, t);
    eventi_.back().sistema = static_cast<int>(s);
}

void BattelloDanni::guasto_sistema(Sistema s, double perdita, const std::string& testo) {
    const std::size_t prima = eventi_.size();
    danneggia(s, perdita);
    if (!testo.empty() && eventi_.size() > prima) eventi_.back().testo = testo + " (" + eventi_.back().testo + ")";
}

double BattelloDanni::urto(double tnt_kg, double distanza_m, double angolo_rad, double x_m, double quota_m) {
    // fattore d'urto dello scafo e della chiglia (dossier 02 §9.4), con la soglia ridotta in profondita'
    const double hsf = std::sqrt(tnt_kg * 2.20462) / (std::max(distanza_m, 0.5) * 3.28084);
    const double ksf = hsf * (1 + std::sin(std::fabs(angolo_rad))) / 2;
    const double riduzione = std::max(0.25, 1 - 0.5 * std::max(quota_m, 0.0) / dati_.sottomarino.quota_collaudo_m);
    const double k = ksf / riduzione;
    if (k < 0.05 || perso_) return k;
    char t[96];
    std::snprintf(t, sizeof t, "scossa (fattore d'urto %.2f)", k);
    evento(EventoBattello::Tipo::scossa, -1, t);
    eventi_.back().intensita = k;
    // sistemi: piu' probabili e piu' gravi con l'urto
    const double p = std::clamp((k - 0.05) / 0.8, 0.0, 1.0);
    for (int i = 0; i < static_cast<int>(Sistema::inerziale); ++i)
        if (caso_.uniforme() < p) danneggia(static_cast<Sistema>(i), caso_.uniforme(0.2, 1.0) * std::min(1.0, k / 0.4));
    // l'inerziale: i giroscopi soffrono gli urti piu' degli altri sistemi (STIMA: probabile gia' da 0,1, certo da 0,35);
    // col suo generatore, perche' le sequenze degli altri sistemi non cambino
    const double p_inerziale = std::clamp((k - 0.03) / 0.32, 0.0, 1.0);
    if (caso_inerziale_.uniforme() < p_inerziale)
        danneggia(Sistema::inerziale, caso_inerziale_.uniforme(0.3, 1.0) * std::min(1.0, k / 0.3));
    // SCRAM del reattore (0,1-0,3: possibile)
    if (nucleare_ && k >= 0.1 && fine_scram_ < tempo_ && caso_.uniforme() < std::min(0.8, (k - 0.1) / 0.3 * 0.8)) {
        fine_scram_ = tempo_ + caso_.uniforme(300.0, 1200.0);
        evento(EventoBattello::Tipo::scram, -1, "SCRAM del reattore: motore d'emergenza");
    }
    // perdite da tenute e tubolature (0,1-0,3: 1-2"), allagamenti da rotture localizzate (0,3-0,6: 2-5" e oltre)
    const int c = compartimento_vicino(x_m);
    if (k >= 0.1) {
        const int n = k < 0.2 ? 1 : 2;
        for (int i = 0; i < n; ++i) {
            int dove = c;
            if (i > 0) dove = std::clamp(c + (caso_.uniforme() < 0.5 ? -1 : 1), 0, static_cast<int>(comp_.size()) - 1);
            const double pollici = k < 0.3 ? caso_.uniforme(1.0, 2.0) : caso_.uniforme(2.0, 5.0) * std::min(2.0, k / 0.3);
            aggiungi_falla(dove, PI * std::pow(pollici * 0.0254 / 2, 2));
        }
    }
    if (k >= 0.3) {
        for (std::size_t i = 0; i < casse_.size(); ++i)
            if (!casse_[i].squarciata && std::fabs(casse_[i].xc - x_m) < 15.0 && caso_.uniforme() < (k - 0.3) / 0.3)
                squarcia_cassa(static_cast<int>(i));
        danno_scafo_ = std::min(1.0, danno_scafo_ + 0.5 * (k - 0.3));
    }
    // incendi elettrici e di tubolature dall'urto (stima: fino al 60% oltre 0,6)
    if (k >= 0.1 && caso_.uniforme() < std::clamp((k - 0.1) / 0.5, 0.0, 0.6)) incendio(c, 0.05);
    // oltre 0,6-1: rottura dello scafo resistente
    if (k >= 0.6 && caso_.uniforme() < std::clamp((k - 0.6) / 0.4, 0.0, 1.0)) {
        danno_scafo_ = 1.0;
        aggiungi_falla(c, caso_.uniforme(0.3, 1.0));
        evento(EventoBattello::Tipo::collasso, c, "rottura dello scafo resistente");
    }
    // i contenitori dei missili fra i due scafi (Oscar II, Charlie II) entro 5 m dallo scoppio: le testate saltano per
    // simpatia col 50% al piu', oltre un urto di 0,3 (STIMA); il loro scoppio fuori dallo scafo resistente e' un altro urto,
    // che puo' far saltare i contenitori accanto
    double W = 0.0, xw = 0.0;
    if (k >= 0.3)
        for (auto& ct : contenitori_)
            if (ct.carica_kg > 0.0 && std::fabs((ct.x0 + ct.x1) / 2 - x_m) < 5.0 && caso_.uniforme() < std::min(0.5, (k - 0.3) / 0.7)) {
                W += ct.carica_kg;
                xw = (ct.x0 + ct.x1) / 2;
                ct.carica_kg = 0.0;
            }
    if (W > 0.0) {
        char e[112];
        std::snprintf(e, sizeof e, "esplodono i missili nei contenitori fra i due scafi (%.0f kg)", W);
        evento(EventoBattello::Tipo::esplosione, compartimento_vicino(xw), e, W);
        urto(W, 1.0 + dati_.sottomarino.intercapedine_m, 0.0, xw, quota_m);
    }
    return k;
}

void BattelloDanni::urto_meccanico(const DanniUrtoUnita& d) {
    if (perso_) return;
    char t[128];
    std::snprintf(t, sizeof t, "collisione: urto sul%s %s (%.1f MJ)", d.parte == ParteUrto::eliche ? "le" : d.parte == ParteUrto::alberi ||
                  d.parte == ParteUrto::timoni ? "" : "la", nome_parte_urto(d.parte), d.energia_mj);
    evento(EventoBattello::Tipo::scossa, -1, t);
    eventi_.back().intensita = std::clamp(d.scossa_g / 2.0, 0.05, 1.0);
    for (const auto& [s, p] : d.sistemi) guasto_sistema(s, p, "collisione");
    if (d.falla_resistente_m2 > 0.0) {   // nel compartimento al punto; uno squarcio lungo la divide fra i suoi estremi
        const int c0 = compartimento_vicino(d.x_m - d.squarcio_m / 2), c1 = compartimento_vicino(d.x_m + d.squarcio_m / 2);
        if (c0 == c1) aggiungi_falla(c0, d.falla_resistente_m2);
        else {
            aggiungi_falla(c0, d.falla_resistente_m2 / 2);
            aggiungi_falla(c1, d.falla_resistente_m2 / 2);
        }
    }
    if (d.falla_cassa_m2 > 0.3) {   // la cassa piu' vicina al punto (entro 15 m) e quelle lungo lo squarcio non tengono l'aria
        int k = -1;
        for (std::size_t i = 0; i < casse_.size(); ++i) {
            const double dx = std::fabs(casse_[i].xc - d.x_m);
            if (casse_[i].squarciata) continue;
            if (dx < d.squarcio_m / 2) squarcia_cassa(static_cast<int>(i));
            else if (dx < 15.0 && (k < 0 || dx < std::fabs(casse_[static_cast<std::size_t>(k)].xc - d.x_m))) k = static_cast<int>(i);
        }
        if (k >= 0 && !casse_[static_cast<std::size_t>(k)].squarciata) squarcia_cassa(k);
    }
    danno_scafo_ = std::min(1.0, danno_scafo_ + d.ammaccatura);
}

int BattelloDanni::compartimento_di(Sistema s) const {
    const int n = static_cast<int>(comp_.size());
    auto primo = [&](const char* uso) {
        for (int i = 0; i < n; ++i) if (comp_[static_cast<std::size_t>(i)].uso == uso) return i;
        return -1;
    };
    const int prua = 0, poppa = n - 1;
    const int locali = primo("locali"), centrale = locali >= 0 && locali <= n / 2 ? locali : prua;
    const int macchine = primo("macchine") >= 0 ? primo("macchine") : poppa;
    const int reattore = primo("reattore") >= 0 ? primo("reattore") : macchine;
    switch (s) {
        case Sistema::sonar_prua: case Sistema::calcolatore_sonar: case Sistema::sonar_tiro: return prua;
        case Sistema::periscopio: case Sistema::antenna: case Sistema::idroplani: case Sistema::timone:
        case Sistema::inerziale: return centrale;
        case Sistema::propulsione: return reattore;
        case Sistema::motori: case Sistema::aria: case Sistema::pompe: return macchine;
        case Sistema::schiera: return poppa;
        default: return prua;
    }
}

std::vector<int> BattelloDanni::vicini(int c) const {
    std::vector<int> v;
    for (const auto& p : paratie_) {
        if (p.a == c) v.push_back(p.b);
        if (p.b == c) v.push_back(p.a);
    }
    return v;
}

void BattelloDanni::incendio(int compartimento, double intensita, bool aria) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size()) || perso_) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.imploso || c.acqua > 0.5 * c.volume) return;
    const bool nuovo = c.fuoco <= 0.0;
    c.fuoco = std::clamp(std::max(c.fuoco, intensita), 0.0, 1.0);
    if (nuovo) evento(EventoBattello::Tipo::incendio, compartimento, "incendio nel compartimento " + c.nome);
    if (aria && !c.aria_nell_incendio) {
        c.aria_nell_incendio = true;
        evento(EventoBattello::Tipo::aria_nell_incendio, compartimento,
               "una tubolatura dell'aria ad alta pressione alimenta l'incendio nel compartimento " + c.nome);
    }
}

void BattelloDanni::esplosione_interna(int compartimento, double tnt_kg) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size()) || perso_ || tnt_kg <= 0) return;
    char t[96];
    std::snprintf(t, sizeof t, "esplosione nel compartimento %s (%.0f kg)", comp_[static_cast<std::size_t>(compartimento)].nome.c_str(), tnt_kg);
    evento(EventoBattello::Tipo::esplosione, compartimento, t, tnt_kg);
    // sovrappressione (gamma - 1) E / V nell'aria dei compartimenti raggiunti: le paratie cedono a cascata
    const double energia = 4.184e6 * tnt_kg;
    std::vector<int> raggiunti{compartimento};
    double volume = std::max(comp_[static_cast<std::size_t>(compartimento)].aria(), 1.0);
    for (bool cresce = true; cresce;) {
        cresce = false;
        const double p = 0.4 * energia / volume;
        for (auto& pa : paratie_) {
            const bool in_a = std::count(raggiunti.begin(), raggiunti.end(), pa.a) > 0;
            const bool in_b = std::count(raggiunti.begin(), raggiunti.end(), pa.b) > 0;
            if (pa.ceduta || in_a == in_b || p <= SOVRAPPRESSIONE_PARATIE * pa.resistenza_Pa) continue;
            pa.ceduta = true;
            const int altro = in_a ? pa.b : pa.a;
            evento(EventoBattello::Tipo::paratia_ceduta, altro, "l'esplosione sfonda la paratia fra " +
                   comp_[static_cast<std::size_t>(pa.a)].nome + " e " + comp_[static_cast<std::size_t>(pa.b)].nome);
            raggiunti.push_back(altro);
            volume += std::max(comp_[static_cast<std::size_t>(altro)].aria(), 1.0);
            cresce = true;
            break;
        }
    }
    // compartimenti raggiunti: incendio e fumo; quello dell'esplosione e' sventrato (equipaggio perduto)
    for (const int r : raggiunti) {
        auto& c = comp_[static_cast<std::size_t>(r)];
        c.fumo = 1.0;
        if (r == compartimento) c.distrutto = true;
        if (!c.imploso && c.acqua < 0.5 * c.volume) {
            if (c.fuoco <= 0.0) evento(EventoBattello::Tipo::incendio, r, "incendio nel compartimento " + c.nome);
            c.fuoco = 1.0;
        }
        for (int i = 0; i < static_cast<int>(Sistema::numero); ++i)
            if (compartimento_di(static_cast<Sistema>(i)) == r) danneggia(static_cast<Sistema>(i), r == compartimento ? 0.9 : 0.5);
    }
    // oltre 50 kg lo scafo resistente si squarcia (diametro della falla ~ W^(1/3) m)
    if (tnt_kg > SQUARCIO_KG) {
        aggiungi_falla(compartimento, PI / 4 * std::pow(std::cbrt(tnt_kg), 2));
        danno_scafo_ = std::min(1.0, danno_scafo_ + tnt_kg / 2000.0);
    }
}

void BattelloDanni::carico_deposito(int compartimento, double tnt_kg) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size())) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.uso != "deposito") return;
    const double nuovo = std::max(0.0, tnt_kg);
    if (c.carica_kg > 0.0) c.propellente_kg *= nuovo / c.carica_kg;
    c.carica_kg = nuovo;
}

void BattelloDanni::passo_incendi(double h, double quota) {
    (void)quota;
    for (std::size_t i = 0; i < comp_.size(); ++i) {
        auto& c = comp_[i];
        const int ic = static_cast<int>(i);
        // batterie allagate: cloro
        if (c.uso == "batterie" && c.acqua > 0.05 * c.volume && !c.imploso) {
            if (!c.cloro) evento(EventoBattello::Tipo::cloro, ic, "cloro dalle batterie allagate nel compartimento " + c.nome);
            c.cloro = true;
            c.fumo = std::min(1.0, c.fumo + h / 300.0);
        }
        // i silos a propellente liquido: con l'acqua dentro (o col fuoco) il propellente esce e reagisce col comburente,
        // una volta (K-219, 3/10/1986: il portello del silo 6 salta, fumo d'acido nitrico, tre morti)
        if (c.liquido_t > 0.0 && !c.reagito && !c.imploso && (c.acqua > 0.02 * c.volume || c.fuoco > 0.5) &&
            caso_.uniforme() < h / REAZIONE_S) {
            c.reagito = true;
            evento(EventoBattello::Tipo::incendio, ic, "il propellente liquido di un missile reagisce nel silo, compartimento " + c.nome);
            esplosione_interna(ic, REAZIONE_LIQUIDO_KG);
            c.cloro = true;   // il fumo tossico dell'acido nitrico, come il cloro delle batterie
        }
        if (c.fuoco <= 0.0 || c.imploso) {   // a fuoco spento: ventilazione e ossigeno che torna
            if (c.isolato && !c.imploso) continue;   // finche' non si riapre
            if (!(c.uso == "batterie" && c.acqua > 0.05 * c.volume)) c.fumo = std::max(0.0, c.fumo - c.fumo * h / VENTILAZIONE_S);
            c.ossigeno += (0.21 - c.ossigeno) * h / 1800.0;
            continue;
        }
        if (c.acqua > 0.5 * c.volume) c.fuoco = 0.0;   // l'acqua lo spegne
        // tubolatura dell'aria ad alta pressione rotta: ossigeno e fuoco finche' c'e' aria nelle bombole
        // (a porte chiuse il ramo dell'aria e' chiuso: se non e' gia' rotto non si rompe)
        if (c.fuoco > 0.7 && !c.aria_nell_incendio && caso_.uniforme() < h / 1200.0 && !c.isolato) incendio(ic, c.fuoco, true);
        if (c.aria_nell_incendio && aria_bombole_ > 0.0) {
            aria_bombole_ = std::max(0.0, aria_bombole_ - ARIA_NEL_FUOCO * h);
            c.ossigeno = 0.21;
            c.combustibile += 0.5 * h;
        }
        const double o2 = std::clamp((c.ossigeno - OSSIGENO_MINIMO) / 0.06, 0.0, 1.0);
        if (c.combustibile > 0.0 && o2 > 0.0) c.fuoco += CRESCITA_FUOCO * c.fuoco * (1 - c.fuoco) * o2 * h;
        else c.fuoco -= 0.01 * h;
        c.combustibile -= c.fuoco * h;
        c.ossigeno = std::max(0.0, c.ossigeno - CONSUMO_OSSIGENO * c.fuoco * h / std::max(c.volume, 1.0));
        c.fumo = std::min(1.0, c.fumo + c.fuoco * h * (1000.0 / std::max(c.volume, 1.0)) / FUMO_PIENO_S);
        // sistemi del compartimento
        for (int s = 0; s < static_cast<int>(Sistema::numero); ++s)
            if (compartimento_di(static_cast<Sistema>(s)) == ic) {
                auto& st = sistemi_[static_cast<std::size_t>(s)];
                st.efficienza = std::max(0.0, st.efficienza - c.fuoco * h / 600.0);
            }
        // fuoco forte: tenute dello scafo che bruciano, compartimenti vicini che prendono fuoco
        if (c.fuoco > 0.8) {
            c.fuoco_forte += h;
            if (std::fmod(c.fuoco_forte, TENUTE_S) < h) aggiungi_falla(ic, AREA_TENUTA);
            for (const int v : vicini(ic))
                if (comp_[static_cast<std::size_t>(v)].fuoco <= 0.0 &&
                    caso_.uniforme() < (c.isolato || comp_[static_cast<std::size_t>(v)].isolato ? CONTAGIO_ISOLATO : 1.0) * h / 900.0) incendio(v, 0.1);
        }
        // testate e propellente nell'incendio del deposito (0,6% al minuto a fuoco pieno, come i depositi delle navi,
        // meno la protezione): salta il carico del locale, il propellente a 0,3, al piu' 30 t; un deposito vuoto non salta
        if (c.uso == "deposito" && c.fuoco > 0.5) {
            const double W = std::min(c.carica_kg + 0.3 * c.propellente_kg, CARICA_MAX_KG);
            if (W > 0.0 && caso_.uniforme() < c.fuoco * 0.006 / 60.0 * (1.0 - c.protezione) * h) {
                c.carica_kg = c.propellente_kg = 0.0;
                esplosione_interna(ic, W);
            }
        }
        // idrogeno delle batterie
        if (c.uso == "batterie" && c.fuoco > 0.5 && caso_.uniforme() < h / 1800.0) esplosione_interna(ic, 30.0);
        if (c.fuoco <= 0.005) {
            c.fuoco = 0.0;
            c.aria_nell_incendio = false;
            evento(EventoBattello::Tipo::incendio_spento, ic, "incendio spento nel compartimento " + c.nome);
        }
    }
    // fumo attraverso le paratie cedute
    for (const auto& p : paratie_)
        if (p.ceduta) {
            auto& a = comp_[static_cast<std::size_t>(p.a)];
            auto& b = comp_[static_cast<std::size_t>(p.b)];
            const double m = (a.fumo + b.fumo) / 2, k = std::min(1.0, h / 60.0);
            a.fumo += (m - a.fumo) * k;
            b.fumo += (m - b.fumo) * k;
        }
}

void BattelloDanni::priorita(Sistema s) {
    priorita_.erase(std::remove(priorita_.begin(), priorita_.end(), s), priorita_.end());
    priorita_.insert(priorita_.begin(), s);
}

int BattelloDanni::uomini_in(const CompartimentoBattello& c) const {
    if (senza_personale_ || c.uso == "reattore" || c.uso == "batterie") return 0;
    return c.uso == "macchine" ? UOMINI_MACCHINE : UOMINI_LOCALI;
}

bool BattelloDanni::isolabile(int compartimento) const {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size()) || perso_) return false;
    const auto& c = comp_[static_cast<std::size_t>(compartimento)];
    return compartimento != compartimento_di(Sistema::inerziale) && !c.imploso && !c.distrutto;
}

void BattelloDanni::isola(int compartimento) {
    if (!isolabile(compartimento)) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.senza_squadre()) return;
    if (senza_personale_) {
        c.isolato = true;
        evento(EventoBattello::Tipo::isolamento, compartimento, "compartimento " + c.nome + " isolato");
    } else {
        c.porte_s = tempo_ + SGOMBERO_S;
        evento(EventoBattello::Tipo::sgombero, compartimento, "sgombero del compartimento " + c.nome + ": porte chiuse fra un minuto");
    }
}

void BattelloDanni::chiudi_subito(int compartimento) {
    if (!isolabile(compartimento)) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.porte_s < 0.0) return;
    c.dentro = static_cast<int>(std::ceil(uomini_in(c) * std::max(c.porte_s - tempo_, 0.0) / SGOMBERO_S));   // escono a ritmo costante
    c.porte_s = -1.0;
    c.isolato = true;
    char t[96];
    std::snprintf(t, sizeof t, "compartimento %s isolato, %d uomini dentro", c.nome.c_str(), c.dentro);
    evento(EventoBattello::Tipo::isolamento, compartimento, t);
}

void BattelloDanni::riapri(int compartimento) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size())) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (!c.senza_squadre()) return;
    if (c.isolato && c.fuoco > 0.0) {
        c.ossigeno = std::max(c.ossigeno, OSSIGENO_RIAPERTO);
        for (const int v : vicini(compartimento)) {
            auto& w = comp_[static_cast<std::size_t>(v)];
            c.fumo = w.fumo = (c.fumo + w.fumo) / 2;
        }
    }
    c.isolato = false;
    c.porte_s = -1.0;
    c.dentro = 0;
    evento(EventoBattello::Tipo::riapertura, compartimento, "compartimento " + c.nome + " riaperto");
}

// Le porte alla fine dello sgombero; chi e' rimasto dentro muore quando il compartimento diventa letale
void BattelloDanni::passo_isolamento() {
    for (std::size_t i = 0; i < comp_.size(); ++i) {
        auto& c = comp_[i];
        if (c.porte_s >= 0.0 && tempo_ >= c.porte_s) {
            c.porte_s = -1.0;
            c.isolato = true;
            evento(EventoBattello::Tipo::isolamento, static_cast<int>(i), "compartimento " + c.nome + " isolato");
        }
        if (c.dentro > 0 && (c.fumo >= 0.95 || c.ossigeno < OSSIGENO_MINIMO || c.acqua > 0.5 * c.volume || c.imploso || c.distrutto)) {
            char t[96];
            std::snprintf(t, sizeof t, "perduti i %d uomini chiusi nel compartimento %s", c.dentro, c.nome.c_str());
            evento(EventoBattello::Tipo::caduti, static_cast<int>(i), t);
            eventi_.back().uomini = c.dentro;
            caduti_ += c.dentro;
            c.dentro = 0;
        }
    }
}

LimitiBattello BattelloDanni::limiti() const {
    LimitiBattello l;
    auto e = [this](Sistema s) { return sistema(s).efficienza; };
    l.scram = fine_scram_ >= tempo_;
    l.velocita = std::min(e(Sistema::motori), e(Sistema::propulsione));
    if (l.scram) l.velocita = std::min(l.velocita, VELOCITA_EMERGENZA);
    l.energia = !(nucleare_ && l.scram && batteria_ <= 0.0);
    if (!l.energia) l.velocita = 0.0;   // reattore fermo e batteria scarica: niente motore d'emergenza
    if (!senza_personale_)   // macchine chiuse e abbandonate: si va col motore d'emergenza
        for (const auto& c : comp_)
            if (c.isolato && c.uso == "macchine") l.velocita = std::min(l.velocita, VELOCITA_EMERGENZA);
    l.timone = e(Sistema::timone) >= 0.3;
    l.idroplani = e(Sistema::idroplani) >= 0.3;
    l.periscopio = e(Sistema::periscopio) >= 0.5;
    l.antenna = e(Sistema::antenna) >= 0.5;
    l.schiera = e(Sistema::schiera) > 0.0;
    l.sonar = e(Sistema::calcolatore_sonar) >= 0.3 && e(Sistema::sonar_prua) >= 0.3;
    l.inerziale = e(Sistema::inerziale) >= 0.5;
    return l;
}

void BattelloDanni::equipaggio(double h, double quota) {
    squadre_.clear();
    squadre_disponibili_ = 0;
    if (perso_) return;
    const double q = dati_.qualita_controllo_danni;
    // squadre in proporzione alla parte abitabile del battello (non allagata, non sventrata, non piena di fumo)
    auto abitabile = [](const CompartimentoBattello& c) {
        return !c.imploso && !c.distrutto && c.acqua < 0.5 * c.volume && c.fumo < 0.95 && !c.senza_squadre();
    };
    double v_tot = 0, v_ok = 0;
    for (const auto& c : comp_) { v_tot += c.volume; if (abitabile(c)) v_ok += c.volume; }
    int squadre = static_cast<int>(std::lround(std::max(1, dati_.squadre) * v_ok / std::max(v_tot, 1.0)));
    squadre_disponibili_ = std::max(squadre, 0);
    auto lavoro = [&](SquadraBattello::Compito c, int comp, int n) {
        SquadraBattello l;
        l.compito = c;
        l.compartimento = comp;
        l.numero = n;
        squadre_.push_back(l);
        return &squadre_.back();
    };
    if (squadre <= 0) return;
    // 1. allagamento: tamponatura delle perdite piccole, piu' lenta con la pressione
    for (std::size_t i = 0; i < falle_.size() && squadre > 0; ++i) {
        auto& f = falle_[i];
        if (f.area <= 0 || f.area > AREA_TAMPONABILE || comp_[static_cast<std::size_t>(f.compartimento)].senza_squadre()) continue;
        f.area -= TAMPONATURA * q / (1 + quota / 100.0) * h;
        --squadre;
        lavoro(SquadraBattello::Compito::tamponare, f.compartimento, 1)->falla = static_cast<int>(i);
        if (f.area <= 0) {
            f.area = 0;
            evento(EventoBattello::Tipo::falla_tamponata, f.compartimento,
                   "falla tamponata nel compartimento " + comp_[static_cast<std::size_t>(f.compartimento)].nome);
        }
    }
    // 2. incendi: tutte le squadre libere, divise fra i focolai; nel fumo (maschere EAB) rendono la meta', su un fuoco
    //    pieno meno di un terzo
    int focolai = 0;
    for (const auto& c : comp_) focolai += c.fuoco > 0.0 && !c.imploso && !c.distrutto && !c.senza_squadre();
    for (auto& c : comp_) {
        if (squadre <= 0 || focolai == 0) break;
        if (c.fuoco <= 0.0 || c.imploso || c.distrutto || c.senza_squadre()) continue;
        const int k = std::max(1, squadre / focolai);
        // un incendio esteso non si spegne con gli estintori: si isola il compartimento e si aspetta che consumi l'ossigeno
        c.fuoco = std::max(0.0, c.fuoco - k * SPEGNIMENTO * q * (1.0 - 0.5 * c.fumo) * (1.0 - 0.7 * c.fuoco) * h);
        squadre -= k;
        lavoro(SquadraBattello::Compito::spegnere, static_cast<int>(&c - comp_.data()), k);
    }
    // pompe: compartimenti senza falle aperte, solo a bassa quota; senza energia sono ferme
    double portata = limiti().energia ? PORTATA_POMPE * sistema(Sistema::pompe).efficienza * std::clamp(1 - quota / QUOTA_POMPE, 0.0, 1.0) * h : 0.0;
    for (std::size_t i = 0; i < comp_.size() && portata > 0; ++i) {
        auto& c = comp_[i];
        if (c.acqua <= 0 || c.imploso) continue;
        bool aperta = false;
        for (const auto& f : falle_) aperta |= f.compartimento == static_cast<int>(i) && f.area > 0;
        if (aperta) continue;
        const double v = std::min(c.acqua, portata);
        c.acqua -= v;
        portata -= v;
        if (h > 0) lavoro(SquadraBattello::Compito::pompare, static_cast<int>(i), 0)->portata = v / h;
    }
    // casse d'assetto: si imbarca dalle valvole, si scarica con le pompe (a bassa quota)
    if (assetto_ < assetto_obiettivo_) assetto_ = std::min(assetto_obiettivo_, assetto_ + PORTATA_ASSETTO * h);
    else if (assetto_ > assetto_obiettivo_)
        assetto_ = std::max(assetto_obiettivo_, assetto_ - std::max(PORTATA_POMPE * sistema(Sistema::pompe).efficienza *
                                                                       std::clamp(1 - quota / QUOTA_POMPE, 0.0, 1.0), 0.002) * h);
    // il reattore e l'energia (danni critici, fase 3)
    if (nucleare_) {
        // SCRAM anche da incendio e da allagamento del compartimento del reattore: resta fermo finche' brucia oltre 0,5 o
        // ha acqua oltre il 10%, e riparte 5 minuti dopo (STIMA)
        const int ir = compartimento_di(Sistema::propulsione);
        const auto& r = comp_[static_cast<std::size_t>(ir)];
        const bool fuoco = r.fuoco > 0.5, acqua = r.acqua > 0.1 * r.volume;
        if (fuoco || acqua) {
            if (fine_scram_ < tempo_)
                evento(EventoBattello::Tipo::scram, ir, std::string("SCRAM del reattore: ") + (fuoco ? "incendio" : "acqua") +
                       " nel compartimento " + r.nome + ", motore d'emergenza");
            fine_scram_ = std::max(fine_scram_, tempo_ + 300.0);
        }
        // la batteria di riserva: allagata, bruciata o sventrata e' persa; durante lo SCRAM regge il motore d'emergenza
        // e le pompe per 2 ore
        for (const auto& c : comp_)
            if (c.uso == "batterie" && batteria_ > 0.0 && (c.acqua > 0.05 * c.volume || c.fuoco > 0.7 || c.distrutto || c.imploso)) {
                batteria_ = 0.0;
                evento(EventoBattello::Tipo::guasto, static_cast<int>(&c - comp_.data()), "batteria di riserva perduta nel compartimento " + c.nome);
            }
        if (fine_scram_ >= tempo_) batteria_ = std::max(0.0, batteria_ - h / BATTERIA_S);
        const bool energia = limiti().energia;
        if (!energia && !senza_energia_) evento(EventoBattello::Tipo::guasto, ir, "reattore fermo e batteria scarica: niente energia, motore e pompe fermi");
        senza_energia_ = !energia;
    }
    // reattore di nuovo critico
    if (fine_scram_ >= 0 && tempo_ >= fine_scram_) {
        fine_scram_ = -1.0;
        evento(EventoBattello::Tipo::riparazione, -1, "reattore di nuovo critico: propulsione ripristinata");
    }
    // l'inerziale sta nel comando: con l'acqua oltre il 20% del locale si ferma (giroscopi ed elettronica bagnati)
    const auto& comando = comp_[static_cast<std::size_t>(compartimento_di(Sistema::inerziale))];
    const bool inerziale_bagnato = comando.acqua > 0.2 * comando.volume;   // e finche' resta bagnato non si ripara
    if (const double e = sistemi_[static_cast<std::size_t>(Sistema::inerziale)].efficienza; inerziale_bagnato && e > 0.0)
        guasto_sistema(Sistema::inerziale, e, "acqua nel compartimento " + comando.nome);
    // 2-5. sistemi, nell'ordine delle priorita'
    for (const Sistema s : priorita_) {
        if (squadre <= 0) break;
        if (s == Sistema::inerziale && inerziale_bagnato) continue;
        auto& st = sistemi_[static_cast<std::size_t>(s)];
        if (st.efficienza >= st.massimo || !abitabile(comp_[static_cast<std::size_t>(compartimento_di(s))])) continue;
        st.efficienza = std::min(st.massimo, st.efficienza + RIPARAZIONE * q * h);
        --squadre;
        lavoro(SquadraBattello::Compito::riparare, compartimento_di(s), 1)->sistema = static_cast<int>(s);
        // col compartimento in fiamme il fuoco erode il sistema e la squadra lo riporta su a ogni passo: si riferisce solo
        // la riparazione di un guasto riferito, non ogni giro
        bool riferire = comp_[static_cast<std::size_t>(compartimento_di(s))].fuoco <= 0.0;
        if (!riferire)
            for (auto e = eventi_.rbegin(); e != eventi_.rend(); ++e)
                if (e->sistema == static_cast<int>(s)) {
                    riferire = e->tipo == EventoBattello::Tipo::guasto;
                    break;
                }
        if (st.efficienza >= st.massimo && riferire) {
            char t[96];
            std::snprintf(t, sizeof t, "riparato: %s al %.0f%%", nome_sistema(s), 100 * st.efficienza);
            evento(EventoBattello::Tipo::riparazione, -1, t);
            eventi_.back().sistema = static_cast<int>(s);
        }
    }
}

void BattelloDanni::evento(EventoBattello::Tipo t, int comp, const std::string& testo, double tnt) {
    EventoBattello e;
    e.tempo = tempo_; e.tipo = t; e.compartimento = comp; e.tnt_kg = tnt; e.testo = testo;
    eventi_.push_back(std::move(e));
}

void BattelloDanni::aggiungi_falla(int compartimento, double area_m2) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size())) return;
    falle_.push_back({compartimento, area_m2});
    evento(EventoBattello::Tipo::allagamento, compartimento, "allagamento nel compartimento " + comp_[static_cast<std::size_t>(compartimento)].nome);
}

void BattelloDanni::squarcia_cassa(int cassa) {
    if (cassa < 0 || cassa >= static_cast<int>(casse_.size())) return;
    auto& c = casse_[static_cast<std::size_t>(cassa)];
    if (c.squarciata) return;
    c.squarciata = true;
    evento(EventoBattello::Tipo::cassa_squarciata, -1, "cassa zavorra " + c.nome + " squarciata");
}

void BattelloDanni::esaurisci() {
    if (soffia_ || aria_bombole_ <= 0) return;
    soffia_ = true;
    evento(EventoBattello::Tipo::esaurimento, -1, "esaurimento d'emergenza delle casse zavorra");
}

void BattelloDanni::ferma_esaurimento() { soffia_ = false; }

void BattelloDanni::sfoga() {
    soffia_ = false;
    sfoga_ = true;
}

void BattelloDanni::varia_peso(double kg, double x_m) {
    peso_variabile_kg_ += kg;
    momento_variabile_Nm_ -= kg * G * (x_m - x_riferimento_);   // peso a prua: prua giu'
}

double BattelloDanni::peso_variabile_N(double quota_m) const {
    return G * (peso_variabile_kg_ + compressione_t_100m_ * 1000.0 * (std::max(quota_m, 0.0) - quota_neutra_) / 100.0);
}

bool BattelloDanni::guasto_casse() const {
    if (sistema(Sistema::aria).efficienza < 0.5 || sistema(Sistema::pompe).efficienza < 0.5) return true;
    return std::any_of(casse_.begin(), casse_.end(), [](const CassaBattello& c) { return c.squarciata; });
}

double BattelloDanni::pressione_aria(const CompartimentoBattello& c) const {
    const double aria = c.aria();
    return aria > 1e-9 ? P_ATM * c.volume / aria : 1e12;
}

double BattelloDanni::acqua_imbarcata_m3() const {
    double w = 0;
    for (const auto& c : comp_) w += c.acqua;
    return w;
}

ForzeIdrostatiche BattelloDanni::forze(double quota_m) const {
    const double p = P_ATM + RHO * G * std::max(quota_m, 0.0);
    ForzeIdrostatiche f;
    f.peso_netto_N = RHO * G * assetto_ + peso_variabile_N(quota_m);   // casse d'assetto: al centro, nessun momento
    f.momento_assetto_Nm = momento_variabile_Nm_;
    for (const auto& c : comp_) {
        f.peso_netto_N += RHO * G * c.acqua;
        f.momento_assetto_Nm -= RHO * G * c.acqua * (c.xc - x_riferimento_);   // acqua a prua: prua giu'
    }
    for (const auto& c : casse_) {
        const double aria = std::min(c.volume, c.aria_Nm3 * P_ATM / p);
        f.peso_netto_N -= RHO * G * aria;
        f.momento_assetto_Nm += RHO * G * aria * (c.xc - x_riferimento_);
    }
    return f;
}

ForzeIdrostatiche BattelloDanni::avanza(double dt, const StatoMoto& moto) {
    const double fine = tempo_ + dt;
    while (tempo_ < fine - 1e-9) {
        const double h = std::min(PASSO, fine - tempo_);
        const double p_mare = P_ATM + RHO * G * std::max(moto.quota_m, 0.0);
        const double pieno = 1.0 - P_ATM / p_mare;   // frazione allagata quando l'aria arriva alla pressione del mare
        // collasso e implosioni dei compartimenti ancora asciutti, uno dopo l'altro
        if (!perso_ && moto.quota_m > quota_collasso_m()) {
            perso_ = true;
            prossima_implosione_ = tempo_;
            char t[96];
            std::snprintf(t, sizeof t, "collasso dello scafo resistente a %.0f m", moto.quota_m);
            evento(EventoBattello::Tipo::collasso, -1, t);
        }
        if (perso_ && tempo_ >= prossima_implosione_) {
            std::vector<int> asciutti;
            for (std::size_t i = 0; i < comp_.size(); ++i)
                if (!comp_[i].imploso && comp_[i].aria() > 0.01 * comp_[i].volume) asciutti.push_back(static_cast<int>(i));
            if (!asciutti.empty()) {
                const int k = asciutti[static_cast<std::size_t>(caso_.uniforme() * static_cast<double>(asciutti.size())) % asciutti.size()];
                auto& c = comp_[static_cast<std::size_t>(k)];
                const double e = std::max(0.0, p_mare - pressione_aria(c)) * c.aria();   // J: p V
                c.imploso = true;
                c.acqua = c.volume;
                evento(EventoBattello::Tipo::implosione, k, "implosione del compartimento " + c.nome, e / 4.184e6);
                prossima_implosione_ = tempo_ + caso_.uniforme(1.0, 30.0);
            } else {
                prossima_implosione_ = 1e30;
            }
        }
        // falle: il mare contro l'aria del compartimento, finche' la pressione si pareggia (o l'acqua esce in risalita)
        for (const auto& f : falle_) {
            auto& c = comp_[static_cast<std::size_t>(f.compartimento)];
            if (c.imploso || f.area <= 0) continue;
            const double dp = p_mare - pressione_aria(c);
            const double q = CD * f.area * std::sqrt(2 * std::fabs(dp) / RHO) * h;
            const double equilibrio = c.volume * std::max(pieno, 0.0);
            c.acqua = dp > 0 ? std::min(c.acqua + q, std::max(equilibrio, c.acqua)) : std::max(c.acqua - q, std::min(equilibrio, c.acqua));
        }
        // paratie: cedono oltre la differenza di pressione; poi l'acqua passa finche' le pressioni si pareggiano
        for (auto& pa : paratie_) {
            auto& a = comp_[static_cast<std::size_t>(pa.a)];
            auto& b = comp_[static_cast<std::size_t>(pa.b)];
            const double dp = pressione_aria(a) - pressione_aria(b);
            if (!pa.ceduta && std::fabs(dp) > pa.resistenza_Pa && !a.imploso && !b.imploso) {
                pa.ceduta = true;
                evento(EventoBattello::Tipo::paratia_ceduta, dp > 0 ? pa.b : pa.a, "cede la paratia fra " + a.nome + " e " + b.nome);
            }
            if (!pa.ceduta) continue;
            const double t = (b.volume * a.acqua - a.volume * b.acqua) / (a.volume + b.volume);   // da a verso b per pareggiare
            const double q = std::min(std::fabs(t), CD * AREA_PARATIA * std::sqrt(2 * std::fabs(dp) / RHO) * h);
            if (t > 0) { a.acqua -= q; b.acqua += q; } else { a.acqua += q; b.acqua -= q; }
        }
        // casse: l'aria in eccesso esce dagli ombrinali (risalita); quella delle casse squarciate se ne va
        for (auto& c : casse_) c.aria_Nm3 = c.squarciata ? 0.0 : std::min(c.aria_Nm3, c.volume * p_mare / P_ATM);
        // esaurimento: l'aria delle bombole va alle casse integre in proporzione all'acqua che hanno ancora
        if (soffia_) {
            double acqua = 0;
            for (const auto& c : casse_) if (!c.squarciata) acqua += std::max(0.0, c.volume - c.aria_Nm3 * P_ATM / p_mare);
            const double n = std::min({portata_aria_ * h, aria_bombole_, acqua * p_mare / P_ATM});
            if (acqua > 1e-6 && n > 0) {
                for (auto& c : casse_)
                    if (!c.squarciata) c.aria_Nm3 += n * std::max(0.0, c.volume - c.aria_Nm3 * P_ATM / p_mare) / acqua;
                aria_bombole_ -= n;
            }
            if (aria_bombole_ <= 1e-6) {
                aria_bombole_ = 0;
                soffia_ = false;
                evento(EventoBattello::Tipo::aria_finita, -1, "aria delle bombole esaurita");
            }
        }
        // sfoghi aperti: l'aria esce dalle casse principali (una cassa piena in SFOGO_S), poi si richiudono
        if (sfoga_) {
            double resta = 0.0;
            for (auto& c : casse_) resta += c.aria_Nm3 = std::max(0.0, c.aria_Nm3 - c.volume * p_mare / P_ATM * h / SFOGO_S);
            if (resta <= 0.0) sfoga_ = false;
        }
        // Tan: l'obiettivo delle casse d'assetto compensa il peso variabile e l'acqua imbarcata
        if (tan_ && sistema(Sistema::pompe).efficienza < 0.5) tan_ = false;
        if (tan_)
            assetto_obiettivo_ = std::clamp(-(peso_variabile_N(moto.quota_m) / G + RHO * acqua_imbarcata_m3()) / RHO,
                                            -RISERVA_ASSETTO_M3, RISERVA_ASSETTO_M3);
        passo_incendi(h, std::max(moto.quota_m, 0.0));
        passo_isolamento();
        equipaggio(h, std::max(moto.quota_m, 0.0));
        tempo_ += h;
    }
    return forze(moto.quota_m);
}

double rateo_verticale_regime(double peso_netto_N, double area_pianta_m2, double velocita_m_s, int piani) {
    const double portanza = 0.5 * RHO * velocita_m_s * velocita_m_s * 0.03 * area_pianta_m2 * 0.8;
    const double f = piani > 0 ? peso_netto_N - portanza
                   : piani < 0 ? peso_netto_N + portanza
                   : peso_netto_N > 0 ? std::max(0.0, peso_netto_N - portanza) : std::min(0.0, peso_netto_N + portanza);
    return (f >= 0 ? 1.0 : -1.0) * std::sqrt(std::fabs(f) / (0.5 * RHO * CD_VERTICALE * std::max(area_pianta_m2, 1.0)));
}

}  // namespace m688
