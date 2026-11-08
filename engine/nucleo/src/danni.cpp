#include "nucleo/danni.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <stdexcept>

#include "nucleo/collisioni.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double RHO = 1025.0, G = 9.81, CD = 0.6;
constexpr double PASSO_DINAMICA = 0.1, PASSO_LIVELLI = 0.5, PASSO_LENTO = 1.0;
constexpr double CRESCITA_FUOCO = 0.693 / 120.0;   // l'incendio raddoppia in 2 minuti
constexpr double CARICA_MAX_SCOPPIO_KG = 30000.0;   // uno scoppio solo: al piu' 30 t di TNT (come tools/navi/danni_critici.py)
// lo scoppio secondario contro la stazza (danni critici, fase 4; STIMA, kg di TNT per t di dislocamento): da 2 kg/t la
// nave salta in aria e sparisce (Hauk, Stockholm, Nanuchka); sotto, si spezza con probabilita' (r - 0,1) / 0,6, cosi'
// una fregata quasi sempre e le grandi (Nimitz, Kitty Hawk) spesso restano intere con la falla e il fuoco
constexpr double DISINTEGRA_KG_T = 2.0, SPEZZA_KG_T_0 = 0.1, SPEZZA_KG_T_SCALA = 0.6;
// Le cisterne vuote in zavorra (dossier 07 §5.3): il 20-40% per colpo nella zona del carico, il 5% col gas inerte, danno
// x3 e rottura possibile. La rottura una volta su tre: delle tre petroliere del dicembre 1969 ne affondo' una (STIMA, come
// i residui delle cisterne, 10 minuti di fuoco pieno)
constexpr double P_VAPORI = 0.30, P_VAPORI_INERTE = 0.05, DANNO_VAPORI = 3.0, P_SPEZZA_VAPORI = 1.0 / 3.0, RESIDUI_CISTERNA_S = 600.0;
constexpr double ZETA_H = 0.6, ZETA_R = 0.15, ZETA_P = 0.6;
constexpr double PORTATA_SOFFIO = 10.0;   // m^3/s per cassa: esaurimento d'emergenza in superficie (aria a 1 bar)

// Toglie i vertici quasi allineati (entro `tolleranza` m dalla retta dei vicini): da ~80 a ~20 vertici.
Poligono semplifica(const Poligono& p, double tolleranza = 0.02) {
    if (p.size() < 5) return p;
    Poligono q;
    q.push_back(p.front());
    for (std::size_t i = 1; i + 1 < p.size(); ++i) {
        const auto& a = q.back();
        const auto& c = p[i + 1];
        const auto& b = p[i];
        const double dy = c.y - a.y, dz = c.z - a.z, l = std::hypot(dy, dz);
        const double d = l > 1e-9 ? std::fabs((b.y - a.y) * dz - (b.z - a.z) * dy) / l : std::hypot(b.y - a.y, b.z - a.z);
        if (d > tolleranza) q.push_back(b);
    }
    q.push_back(p.back());
    return q;
}

// Sezione semplificata e simmetrica: si semplifica il fianco di dritta e lo si specchia (la nave intatta resta dritta).
Poligono sezione_simmetrica(const Stazione& s) {
    Poligono fianco;
    for (std::size_t k = 0; k < s.semilarghezze.size(); ++k)
        fianco.push_back({-s.semilarghezze[k], s.z_chiglia + s.dz * static_cast<double>(k)});
    if (fianco.size() < 2) return {};
    fianco = semplifica(fianco);
    Poligono p = fianco;
    for (std::size_t k = fianco.size(); k-- > 0;) p.push_back({-fianco[k].y, fianco[k].z});
    return p;
}

double semilarghezza(const Stazione& s, double z) {
    if (s.semilarghezze.empty()) return 0.0;
    const double k = (z - s.z_chiglia) / s.dz;
    if (k <= 0) return s.semilarghezze.front();
    const auto n = s.semilarghezze.size();
    if (k >= static_cast<double>(n - 1)) return s.semilarghezze.back();
    const auto i = static_cast<std::size_t>(k);
    const double f = k - static_cast<double>(i);
    return s.semilarghezze[i] * (1 - f) + s.semilarghezze[i + 1] * f;
}

std::size_t stazione_vicina(const std::vector<Stazione>& st, double x) {
    std::size_t m = 0;
    for (std::size_t i = 1; i < st.size(); ++i)
        if (std::fabs(st[i].x - x) < std::fabs(st[m].x - x)) m = i;
    return m;
}

// la nave che salta in aria: tutti i corpi a fondo subito (la posa sotto il mare: non si disegna piu')
void disintegra(std::vector<Corpo>& corpi) {
    for (auto& c : corpi) {
        c.affondato = true;
        c.assetto.immersione = 200.0;
        c.v_imm = c.v_sb = c.v_ap = 0.0;
    }
}

double combustibile_per(const std::string& uso, const std::string& tipo) {
    if (uso == "macchine") return 3600;
    if (uso == "deposito") return 1500;
    if (uso == "carico") return tipo == "petroliera" ? 40000 : 7200;
    if (uso == "ala") return 600;
    if (uso == "zavorra") return 0;   // casse zavorra dei sottomarini: aria o acqua, niente da bruciare
    return 1800;
}

// Il carburante nello scafo (danni critici, fase 2): secondi di fuoco pieno per tonnellata nel locale, oltre al
// combustibile del suo uso (STIMA: nafta e gasolio 20 s/t, JP-5 e benzina avio 30, i veicoli del garage 10; al piu'
// 20.000 s). Il carico liquido di petroliere e rifornitrici brucia come quello di una petroliera
double combustibile_carburante(const std::string& tipo, double t) {
    if (t <= 0.0 || tipo.empty()) return 0.0;
    if (tipo.ends_with("_carico")) return 40000.0;
    const double s_t = tipo == "jp5" || tipo == "avio" ? 30.0 : tipo == "veicoli" ? 10.0 : 20.0;
    return std::min(20000.0, s_t * t);
}
// JP-5 e benzina avio fanno un incendio piu' violento: si accende piu' spesso e parte piu' forte
bool carburante_volatile(const std::string& tipo) { return tipo == "jp5" || tipo == "avio" || tipo == "jp5_carico" || tipo == "prodotti_carico"; }
// Quanto salta facilmente il deposito secondo quello che contiene (danni critici, T2; STIMA nella forchetta del 2-5% per
// colpo delle navi moderne, dossier 07): il propellente dei missili e dei razzi piu' dei proietti, i siluri meno, le bombe
// meno di tutti. Nei depositi misti decide il piu' sensibile
double sensibilita_deposito(const std::string& deposito) {
    const auto c_e = [&](const char* s) { return deposito.find(s) != std::string::npos; };
    if (c_e("missili") || c_e("razzi")) return 1.25;
    if (c_e("proietti") || c_e("munizioni")) return 1.0;
    if (c_e("siluri")) return 0.75;
    return c_e("bombe") ? 0.5 : 1.0;
}
// Il fuoco passa al locale vicino secondo quello che c'e' dentro (danni critici, T5; STIMA): 1 e' un locale comune, cioe'
// 1/400 al secondo. Il deposito col propellente prende fuoco dal calore della paratia, meno se e' protetto; il garage aperto
// corre (Atlantic Conveyor); il JP-5 e la benzina avio si accendono prima, la nafta e le macchine un po' prima; le casse
// laterali senza carburante tardano
double peso_propagazione(const CellaScafo& d) {
    if (d.uso == "deposito") return 2.0 * (1.0 - d.protezione);
    if (d.carburante.find("veicoli") != std::string::npos) return 4.0;
    if (d.carburante.find("jp5") != std::string::npos || d.carburante.find("avio") != std::string::npos ||
        carburante_volatile(d.carburante)) return 3.0;
    if (d.carburante_t > 0.0 || d.uso == "macchine") return 1.5;
    return d.uso == "ala" ? 0.5 : 1.0;
}

AssettoScafo in_posa(const Assetto& a) {
    AssettoScafo p;
    p.sbandamento = gradi(a.sbandamento);
    p.assetto = gradi(a.appoppamento);
    p.immersione = a.immersione;
    p.dx = a.dx;
    p.dy = a.dy;
    p.x_perno = a.x_o;
    return p;
}

}  // namespace

double Corpo::massa() const {
    double m = 0;
    for (double v : massa_stazioni) m += v;
    return m;
}

DanniNave::DanniNave(const DatiScafo& dati, std::uint64_t seme) : dati_(dati), caso_(seme) {
    Corpo c;
    c.stazioni = dati.stazioni;
    const double dx = dati.passo;
    // idrostatica intatta
    std::vector<double> vol(c.stazioni.size(), 0.0);
    double V0 = 0, Mx = 0, Mz = 0, It = 0, Aw = 0, Sx = 0;
    for (std::size_t i = 0; i < c.stazioni.size(); ++i) {
        const auto& s = c.stazioni[i];
        c.sezioni.push_back(sezione_simmetrica(s));
        Punto2 b;
        const double A = area_poligono(taglia(c.sezioni.back(), 0, 1, 0), &b);
        vol[i] = A * dx;
        V0 += vol[i]; Mx += vol[i] * s.x; Mz += vol[i] * b.z;
        const double hw = s.semilarghezze.empty() || s.z_chiglia > 0 ? 0.0 : semilarghezza(s, 0.0);
        It += 2.0 / 3.0 * hw * hw * hw * dx; Aw += 2 * hw * dx; Sx += 2 * hw * s.x * dx;
    }
    if (V0 <= 0) throw std::runtime_error("scafo senza volume immerso");
    const double lcb = Mx / V0, zb = Mz / V0, xf = Sx / std::max(Aw, 1e-9);
    double Il = 0;
    for (const auto& s : c.stazioni) {
        const double hw = s.semilarghezze.empty() || s.z_chiglia > 0 ? 0.0 : semilarghezza(s, 0.0);
        Il += 2 * hw * (s.x - xf) * (s.x - xf) * dx;
    }
    massa_nave_ = RHO * V0;
    c.zg = zb + It / V0 - dati.GM;
    // pesi: 75% come la carena, 25% uniformi su un tratto simmetrico attorno al centro di carena (LCG = LCB)
    const double xa = c.stazioni.front().x, xp = c.stazioni.back().x;
    const double sp = std::min(lcb - xa, xp - lcb);
    int nu = 0;
    for (const auto& s : c.stazioni) nu += std::fabs(s.x - lcb) <= sp + 1e-9;
    for (std::size_t i = 0; i < c.stazioni.size(); ++i) {
        const double u = std::fabs(c.stazioni[i].x - lcb) <= sp + 1e-9 ? 1.0 / nu : 0.0;
        c.massa_stazioni.push_back(RHO * (0.75 * vol[i] + 0.25 * V0 * u));
    }
    c.spinta = vol;
    // trave: modulo minimo di regolamento (IACS) per la resistenza della nave
    const double L = dati.lunghezza, B = dati.larghezza, T = -std::min_element(c.stazioni.begin(), c.stazioni.end(),
        [](const Stazione& a, const Stazione& b) { return a.z_chiglia < b.z_chiglia; })->z_chiglia;
    const double cb = std::clamp(V0 / (L * B * T), 0.35, 0.9);
    const double cn = L < 90 ? 0.0412 * L + 4.0 : (L <= 300 ? 10.75 - std::pow((300 - L) / 100, 1.5) : 10.75);
    c.capacita_trave = 300e6 * cn * L * L * B * (cb + 0.7) * 1e-6 * dati.resistenza_trave;
    // dinamica: frequenze proprie della nave intatta per lo smorzamento
    momento_roll_ = massa_nave_ * std::pow(0.4 * B, 2) * 1.2;
    momento_pitch_ = massa_nave_ * std::pow(0.27 * L, 2) * 1.3;
    smorz_h_ = 2 * ZETA_H * std::sqrt(RHO * G * Aw / (2 * massa_nave_));
    smorz_r_ = 2 * ZETA_R * std::sqrt(massa_nave_ * G * std::max(dati.GM, 0.3) / momento_roll_);
    smorz_p_ = 2 * ZETA_P * std::sqrt(RHO * G * Il / momento_pitch_);
    // celle
    const bool militare = dati.tipo == "militare" || dati.tipo == "sottomarino";
    for (const auto& cs : dati.celle) {
        Cella ce;
        ce.dati = cs;
        for (std::size_t i = 0; i < c.stazioni.size(); ++i) {
            const double x = c.stazioni[i].x;
            if (x < cs.x0 || x >= cs.x1 || c.sezioni[i].empty()) continue;
            Poligono p = taglia(taglia(c.sezioni[i], -1, 0, -cs.y0), 1, 0, cs.y1);
            const double A = area_poligono(p);
            if (A <= 1e-6) continue;
            ce.stazioni.push_back(static_cast<int>(i));
            ce.poligoni.push_back(std::move(p));
            ce.volume += A * dx;
        }
        if (ce.stazioni.empty()) continue;
        ce.acqua_stazioni.assign(ce.stazioni.size(), 0.0);
        ce.combustibile = combustibile_per(cs.uso, dati.tipo);
        if (cs.carburante.ends_with("_carico")) ce.combustibile = std::max(ce.combustibile, combustibile_carburante(cs.carburante, cs.carburante_t));
        else ce.combustibile += combustibile_carburante(cs.carburante, cs.carburante_t);
        c.celle.push_back(std::move(ce));
    }
    // zavorra iniziale dei sottomarini: acqua nelle casse, tolta dal peso del battello stazione per stazione, con il
    // baricentro corretto perche' peso e baricentro complessivi (e quindi l'equilibrio) restino quelli intatti
    for (auto& ce : c.celle) ce.acqua = std::min(ce.dati.acqua_iniziale, ce.volume * ce.dati.permeabilita);
    livelli(c);
    // (dove l'acqua supera la massa della stazione, il resto si toglie dalle altre in proporzione)
    double mw = 0, mwz = 0, resto = 0;
    for (const auto& ce : c.celle) {
        if (ce.acqua <= 0) continue;
        for (std::size_t k = 0; k < ce.stazioni.size(); ++k) {
            auto& m = c.massa_stazioni[static_cast<std::size_t>(ce.stazioni[k])];
            const double w = RHO * ce.acqua_stazioni[k];
            resto += std::max(0.0, w - m);
            m = std::max(0.0, m - w);
        }
        mw += RHO * ce.acqua;
        mwz += RHO * ce.acqua * ce.bz;
    }
    if (resto > 0) {
        double tot = 0;
        for (double m : c.massa_stazioni) tot += m;
        for (double& m : c.massa_stazioni) m *= 1.0 - resto / tot;
    }
    if (mw > 0) c.zg = (massa_nave_ * c.zg - mwz) / (massa_nave_ - mw);
    // aperture fra celle (perdite delle paratie) e aperture di coperta
    // perdite delle paratie (porte stagne chiuse sulle navi militari, porte e passaggi sui mercantili), aperture di
    // coperta; cedimento delle paratie trasversali sotto la differenza di livello: oltre l'altezza dello scafo per le
    // navi militari, oltre l'80% per i mercantili, +-20% per paratia (stima di gioco: le paratie si progettano per
    // l'acqua fino alla coperta)
    // sottomarini: portelli chiusi a tenuta (nessuna apertura di coperta), anche dove la cima dello scafo e' sott'acqua
    const double perdita = militare ? 0.002 : 0.02, cedimento = militare ? 1.0 : 0.8;
    const double coperta = dati.tipo == "sottomarino" ? 0.0 : militare ? 0.15 : 0.4;
    for (std::size_t i = 0; i < c.celle.size(); ++i) {
        const auto& a = c.celle[i].dati;
        const auto& st = c.stazioni[static_cast<std::size_t>(c.celle[i].stazioni[c.celle[i].stazioni.size() / 2])];
        const double yc = std::clamp((std::max(a.y0, -B) + std::min(a.y1, B)) / 2, -B / 3, B / 3);
        c.falle.push_back({static_cast<int>(i), st.x, yc, st.coperta(), coperta, true, false});
        for (std::size_t j = i + 1; j < c.celle.size(); ++j) {
            const auto& b = c.celle[j].dati;
            const double y0 = std::max(a.y0, b.y0), y1 = std::min(a.y1, b.y1);
            const bool vicine_x = (std::fabs(a.x1 - b.x0) < 1e-3 || std::fabs(a.x0 - b.x1) < 1e-3) && y1 > y0;
            const bool vicine_y = std::fabs(a.x0 - b.x0) < 1e-3 && (std::fabs(a.y1 - b.y0) < 1e-3 || std::fabs(a.y0 - b.y1) < 1e-3);
            if (!vicine_x && !vicine_y) continue;
            const double x = vicine_x ? (std::fabs(a.x1 - b.x0) < 1e-3 ? a.x1 : a.x0) : (a.x0 + a.x1) / 2;
            const double y = vicine_x ? std::clamp((y0 + y1) / 2, -B / 4, B / 4) : (std::fabs(a.y1 - b.y0) < 1e-3 ? a.y1 : a.y0);
            const auto& sv = c.stazioni[stazione_vicina(c.stazioni, x)];
            // sottomarini: fra le casse zavorra e i compartimenti nessuna porta ne' perdita (lo scafo resistente)
            const bool cassa = dati.tipo == "sottomarino" && (a.uso == "zavorra" || b.uso == "zavorra");
            c.aperture.push_back({static_cast<int>(i), static_cast<int>(j), x, y, sv.z_chiglia + 1.0, cassa ? 0.0 : perdita,
                                  vicine_x ? cedimento * (sv.coperta() - sv.z_chiglia) * caso_.uniforme(0.8, 1.2) : 1e9, false});
        }
    }
    for (const auto& z : dati.sopra_coperta) zone_.push_back({z, 0.0, z.carico == "avio" ? 3000.0 : 1200.0});
    // il sottomarino in superficie: i contenitori dei missili fra i due scafi (Oscar II, Charlie II) come i lanciatori
    // in coperta (danni critici, fase 3)
    for (const auto& z : dati.fuori_resistente)
        if (z.carica_kg > 0) zone_.push_back({z, 0.0, 1200.0});
    corpi_.push_back(std::move(c));
}

void DanniNave::imposta_acqua(std::size_t cella, double m3, std::size_t corpo) {
    auto& c = corpi_.at(corpo);
    auto& ce = c.celle.at(cella);
    ce.acqua = std::clamp(m3, 0.0, ce.volume * ce.dati.permeabilita);
    ce.livello = -1e9;   // da ricalcolare
    livelli(c);
}

void DanniNave::imposta_fuoco(std::size_t cella, double intensita, std::size_t corpo) {
    auto& ce = corpi_.at(corpo).celle.at(cella);
    ce.fuoco = ce.riempimento() < 0.4 ? std::clamp(intensita, 0.0, 1.0) : 0.0;
}

void DanniNave::imposta_carico(std::size_t cella, double carica_kg, double propellente_kg, std::size_t corpo) {
    auto& ce = corpi_.at(corpo).celle.at(cella);
    ce.dati.carica_kg = std::max(0.0, carica_kg);
    ce.dati.propellente_kg = std::max(0.0, propellente_kg);
}

bool DanniNave::esplosione_di_prova(bool coperta) {
    if (coperta) {
        ZonaIncendio* z = nullptr;
        for (auto& k : zone_)
            if (k.dati.carica_kg > 0 && (!z || k.dati.carica_kg > z->dati.carica_kg)) z = &k;
        if (!z) return false;
        esplosione_in_coperta(*z);
        return true;
    }
    int jm = -1;
    auto& c = corpi_.front();
    for (std::size_t j = 0; j < c.celle.size(); ++j)
        if (c.celle[j].dati.uso == "deposito" &&
            (jm < 0 || c.celle[j].dati.carica_kg > c.celle[static_cast<std::size_t>(jm)].dati.carica_kg)) jm = static_cast<int>(j);
    if (jm < 0) return false;
    esplosione_deposito(0, jm);
    return true;
}

double DanniNave::capacita_iniziale() const { return corpi_.empty() ? 0.0 : corpi_.front().capacita_trave; }

void DanniNave::imposta_zavorra(bool gas_inerte) {
    if (dati_.tipo != "petroliera") return;
    p_vapori_ = gas_inerte ? P_VAPORI_INERTE : P_VAPORI;
    for (auto& c : corpi_)
        for (auto& ce : c.celle)
            if (ce.dati.uso == "carico") {
                ce.dati.carburante.clear();
                ce.dati.carburante_t = 0.0;
                ce.combustibile = RESIDUI_CISTERNA_S;
            }
}

void DanniNave::annota(const std::string& s) {
    char t[32];
    std::snprintf(t, sizeof t, "%6.0f s  ", tempo_);
    registro_.push_back(t + s);
}

bool DanniNave::affondata() const {
    return std::all_of(corpi_.begin(), corpi_.end(), [](const Corpo& c) { return c.affondato; });
}

double DanniNave::acqua_imbarcata_t() const {
    double v = 0;
    for (const auto& c : corpi_)
        for (const auto& ce : c.celle) v += std::max(0.0, ce.acqua - ce.dati.acqua_iniziale);
    return v * RHO / 1000.0;
}

namespace {
const Corpo& principale(const std::vector<Corpo>& corpi) {
    return *std::max_element(corpi.begin(), corpi.end(), [](const Corpo& a, const Corpo& b) { return a.massa() < b.massa(); });
}
}  // namespace

double DanniNave::sbandamento_gradi() const { return gradi(principale(corpi_).assetto.sbandamento); }
double DanniNave::appoppamento_gradi() const { return gradi(principale(corpi_).assetto.appoppamento); }
double DanniNave::immersione_m() const { return principale(corpi_).assetto.immersione; }

PosaNave DanniNave::posa() const {
    PosaNave p;
    p.danneggiata = true;
    // segni sullo scafo: falle verso il mare (non le aperture di coperta ne' le valvole) e fuliggine attorno ai locali
    // che hanno bruciato; coordinate del modello intatto, che sono quelle dei corpi
    const double B = dati_.larghezza;
    for (const auto& c : corpi_) {
        for (const auto& f : c.falle) {
            if (f.coperta || f.controallagamento || f.area < 0.02) continue;
            SegnoScafo s;
            s.x = f.x; s.y = f.y; s.z = f.z;
            const double r = std::min(std::sqrt(f.area / PI), 6.0);
            const bool fondo = std::fabs(f.y) < 0.5;
            s.rx = r; s.ry = fondo ? r : 1.5; s.rz = fondo ? 1.5 : r;
            p.segni.push_back(s);
        }
        for (const auto& ce : c.celle) {
            if (ce.annerimento < 0.03 || ce.stazioni.empty()) continue;
            const auto& st = c.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
            SegnoScafo s;
            s.tipo = SegnoScafo::Tipo::fuliggine;
            s.x = (std::max(ce.dati.x0, c.stazioni.front().x) + std::min(ce.dati.x1, c.stazioni.back().x)) / 2;
            s.y = std::clamp((std::max(ce.dati.y0, -B / 2) + std::min(ce.dati.y1, B / 2)) / 2, -B / 2, B / 2);
            s.z = st.coperta();
            s.rx = (std::min(ce.dati.x1, c.stazioni.back().x) - std::max(ce.dati.x0, c.stazioni.front().x)) / 2 + 4.0;
            s.ry = B; s.rz = 9.0;
            s.intensita = ce.annerimento;
            p.segni.push_back(s);
        }
    }
    for (const auto& z : zone_) {
        if (z.annerimento < 0.03) continue;
        SegnoScafo s;
        s.tipo = SegnoScafo::Tipo::fuliggine;
        s.x = (z.dati.x0 + z.dati.x1) / 2; s.y = 0;
        s.z = corpi_.front().stazioni[stazione_vicina(corpi_.front().stazioni, s.x)].coperta() + 3.0;
        s.rx = (z.dati.x1 - z.dati.x0) / 2 + 4.0; s.ry = B; s.rz = 10.0;
        s.intensita = z.annerimento;
        p.segni.push_back(s);
    }
    if (frusta_t0_ >= 0) {   // su per mezzo periodo della bolla, poi giu' con un'oscillazione smorzata a 1,5 Hz
        const double t = tempo_ - frusta_t0_, T = frusta_T1_;
        const double g = t < T ? std::sin(PI * t / T) : -0.6 * std::exp(-(t - T) / 1.5) * std::sin(2 * PI * 1.5 * (t - T) + 0.5);
        if (t < T + 8.0) {
            p.flessione_x = frusta_x_;
            p.flessione_m = frusta_m_ * g;
            p.flessione_larghezza = 0.25 * dati_.lunghezza;
        }
    }
    if (corpi_.size() == 1) {
        p.prodiero = in_posa(corpi_[0].assetto);
        return p;
    }
    p.spezzata = true;
    p.x_rottura = x_rottura_;
    p.poppiero = in_posa(corpi_[0].assetto);
    p.prodiero = in_posa(corpi_[1].assetto);
    return p;
}

// --- acqua nelle celle: pelo libero orizzontale nel mondo -----------------------------------------------------

void DanniNave::livelli(Corpo& c) {
    const double dx = dati_.passo;
    double a, b, cc;
    for (auto& ce : c.celle) {
        std::fill(ce.acqua_stazioni.begin(), ce.acqua_stazioni.end(), 0.0);
        if (ce.acqua <= 1e-6) { ce.livello = -1e9; continue; }
        const double perm = ce.dati.permeabilita;
        const double vg = std::min(ce.acqua / perm, ce.volume);
        std::vector<double> ak(ce.stazioni.size());
        double zmin = 1e9, zmax = -1e9;
        for (std::size_t k = 0; k < ce.stazioni.size(); ++k) {
            c.assetto.coefficienti(c.stazioni[static_cast<std::size_t>(ce.stazioni[k])].x, a, b, cc);
            ak[k] = a;
            for (const auto& q : ce.poligoni[k]) {
                const double z = a + b * q.y + cc * q.z;
                zmin = std::min(zmin, z); zmax = std::max(zmax, z);
            }
        }
        auto volume_sotto = [&](double h) {
            double v = 0;
            for (std::size_t k = 0; k < ce.poligoni.size(); ++k) v += area_poligono(taglia(ce.poligoni[k], b, cc, h - ak[k])) * dx;
            return v;
        };
        // regula falsi (Illinois) sul volume in funzione del livello, partendo dal livello precedente
        double lo = zmin, hi = zmax, flo = -vg, fhi = ce.volume - vg;
        if (ce.livello > -1e8) {
            const double l0 = std::max(zmin, ce.livello - 0.5), l1 = std::min(zmax, ce.livello + 0.5);
            const double f0 = l0 < l1 ? volume_sotto(l0) - vg : 1.0, f1 = l0 < l1 ? volume_sotto(l1) - vg : -1.0;
            if (f0 <= 0 && f1 >= 0) { lo = l0; hi = l1; flo = f0; fhi = f1; }
        }
        double m = hi;
        if (vg < ce.volume * 0.9999) {
            int lato = 0;
            for (int it = 0; it < 25 && hi - lo > 0.002; ++it) {
                m = (fhi - flo) > 1e-12 ? lo - flo * (hi - lo) / (fhi - flo) : (lo + hi) / 2;
                const double fm = volume_sotto(m) - vg;
                if (std::fabs(fm) < 0.002 * std::max(vg, 1.0)) break;
                if (fm < 0) { lo = m; flo = fm; if (lato == -1) fhi /= 2; lato = -1; }
                else { hi = m; fhi = fm; if (lato == 1) flo /= 2; lato = 1; }
            }
        }
        ce.livello = m;
        double sv = 0, sx = 0, sy = 0, sz = 0;
        for (std::size_t k = 0; k < ce.poligoni.size(); ++k) {
            Punto2 bc;
            const double A = area_poligono(taglia(ce.poligoni[k], b, cc, ce.livello - ak[k]), &bc);
            const double v = A * dx;
            ce.acqua_stazioni[k] = v * perm;
            sv += v; sx += v * c.stazioni[static_cast<std::size_t>(ce.stazioni[k])].x; sy += v * bc.y; sz += v * bc.z;
        }
        if (sv > 1e-9) {
            ce.bx = sx / sv; ce.by = sy / sv; ce.bz = sz / sv;
            const double scala = ce.acqua / (sv * perm);   // corregge la discretizzazione
            for (auto& v : ce.acqua_stazioni) v *= scala;
        }
    }
}

// --- dinamica: sussulto, rollio, beccheggio -------------------------------------------------------------------

void DanniNave::passo_dinamica(Corpo& c, double h) {
    const double dx = dati_.passo;
    double a, b, cc;
    double Ox, Oy, Oz;
    c.assetto.mondo(c.assetto.x_o, 0, 0, Ox, Oy, Oz);
    double Fz = 0, tx = 0, tt = 0, massa = 0;
    auto forza = [&](double x, double y, double z, double f) {
        double X, Y, Z;
        c.assetto.mondo(x, y, z, X, Y, Z);
        Fz += f; tx += (Y - Oy) * f; tt += (X - Ox) * f;
    };
    double cima = -1e9;
    for (std::size_t k = 0; k < c.stazioni.size(); ++k) {
        const auto& s = c.stazioni[k];
        c.spinta[k] = 0;
        if (c.sezioni[k].empty()) continue;
        c.assetto.coefficienti(s.x, a, b, cc);
        Punto2 bc;
        const double A = area_poligono(taglia(c.sezioni[k], b, cc, -a), &bc);
        c.spinta[k] = A * dx;
        if (A > 1e-9) forza(s.x, bc.y, bc.z, RHO * G * A * dx);
        forza(s.x, 0, c.zg, -G * c.massa_stazioni[k]);
        massa += c.massa_stazioni[k];
        const double top = s.coperta();
        const double hb = s.semilarghezze.back();
        cima = std::max({cima, a + b * hb + cc * top, a - b * hb + cc * top, a + cc * s.z_chiglia});
    }
    for (const auto& ce : c.celle)
        if (ce.acqua > 1e-6) {
            forza(ce.bx, ce.by, ce.bz, -G * RHO * ce.acqua);
            massa += RHO * ce.acqua;
        }
    if (cima < -2.0) {   // tutto sotto la superficie: affondato
        if (!c.affondato) annota(c.stazioni.size() == corpi_.front().stazioni.size() && corpi_.size() == 1 ? "la nave affonda"
                                                                                                              : "un troncone affonda");
        c.affondato = true;
        return;
    }
    // masse e momenti d'inerzia del corpo (con le masse aggiunte), smorzamento dalle frequenze della nave intatta
    const double L = c.stazioni.back().x - c.stazioni.front().x + dx;
    const double Mh = 2.0 * massa;
    const double Ir = massa * std::pow(0.4 * dati_.larghezza, 2) * 1.2, Ip = massa * std::pow(0.27 * L, 2) * 1.3;
    c.v_imm += (-Fz / Mh - smorz_h_ * c.v_imm) * h;
    c.v_sb += (tx / Ir - smorz_r_ * c.v_sb) * h;
    // forze generalizzate: lo sbandamento ruota attorno all'asse x del mondo; l'assetto attorno all'asse trasversale
    // della nave gia' sbandata, (0, -cos phi, -sin phi): le forze verticali vi danno una coppia ridotta di cos phi
    c.v_ap += (tt * std::cos(c.assetto.sbandamento) / Ip - smorz_p_ * c.v_ap) * h;
    c.assetto.immersione += c.v_imm * h;
    c.assetto.sbandamento = std::remainder(c.assetto.sbandamento + c.v_sb * h, 2 * PI);
    c.assetto.appoppamento = std::clamp(c.assetto.appoppamento + c.v_ap * h, -PI / 2, PI / 2);
    c.assetto.dx += c.v_dx * h;
    c.v_dx *= std::exp(-h / 60.0);
}

// --- allagamento: efflusso attraverso falle e aperture --------------------------------------------------------

void DanniNave::passo_allagamento(Corpo& c, double h) {
    auto superficie = [](const Cella& ce) {   // area del pelo libero stimata (volume / altezza)
        return ce.volume / 8.0;
    };
    for (auto& f : c.falle) {
        if (f.area <= 0 || f.cella < 0) continue;
        auto& ce = c.celle[static_cast<std::size_t>(f.cella)];
        const double z = c.assetto.quota(f.x, f.y, f.z);
        const double fuori = -z, dentro = ce.livello > z ? ce.livello - z : 0.0;
        double dh;
        if (fuori > 0) dh = fuori - dentro;   // positivo: entra
        else if (dentro > 0) dh = -dentro;    // falla sopra il mare: esce l'acqua sopra la falla
        else continue;
        double dv = (dh > 0 ? 1 : -1) * CD * f.area * std::sqrt(2 * G * std::fabs(dh)) * h;
        const double cap = ce.volume * ce.dati.permeabilita;
        dv = std::clamp(dv, -std::min(ce.acqua, std::fabs(dh) * superficie(ce)),
                        std::max(0.0, std::min(cap - ce.acqua, std::fabs(dh) * superficie(ce))));
        ce.acqua = std::clamp(ce.acqua + dv, 0.0, cap);
        if (ce.livello > -1e8) ce.livello += dv / (ce.dati.permeabilita * superficie(ce));   // fino al prossimo calcolo esatto
    }
    for (const auto& ap : c.aperture) {
        if (ap.area <= 0) continue;
        auto& A = c.celle[static_cast<std::size_t>(ap.a)];
        auto& B = c.celle[static_cast<std::size_t>(ap.b)];
        const double z = c.assetto.quota(ap.x, ap.y, ap.z);
        const double da = std::max(0.0, A.livello - z), db = std::max(0.0, B.livello - z);
        if (da <= 0 && db <= 0) continue;
        const double dh = da - db;
        auto& da_cella = dh > 0 ? A : B;
        auto& a_cella = dh > 0 ? B : A;
        double dv = CD * ap.area * std::sqrt(2 * G * std::fabs(dh)) * h;
        dv = std::min({dv, da_cella.acqua, a_cella.volume * a_cella.dati.permeabilita - a_cella.acqua,
                       std::fabs(dh) * std::min(superficie(A), superficie(B)) * 0.5});
        if (dv <= 0) continue;
        da_cella.acqua -= dv;
        a_cella.acqua += dv;
        if (da_cella.livello > -1e8) da_cella.livello -= dv / (da_cella.dati.permeabilita * superficie(da_cella));
        if (a_cella.livello > -1e8) a_cella.livello += dv / (a_cella.dati.permeabilita * superficie(a_cella));
    }
}

// --- trave di scafo ---------------------------------------------------------------------------------------------

void DanniNave::verifica_trave(std::size_t i) {
    auto& c = corpi_[i];
    const std::size_t n = c.stazioni.size();
    // al piu' tre tronconi; per flessione cedono solo tronconi lunghi almeno il 30% della nave
    if (c.affondato || corpi_.size() >= 3 || static_cast<double>(n) < 0.3 * static_cast<double>(dati_.stazioni.size())) return;
    std::vector<double> f(n), m(c.massa_stazioni);   // forza verticale netta (N) e massa (kg) per stazione
    for (std::size_t k = 0; k < n; ++k) f[k] = RHO * G * c.spinta[k] - G * c.massa_stazioni[k];
    for (const auto& ce : c.celle)
        for (std::size_t j = 0; j < ce.stazioni.size(); ++j) {
            const auto k = static_cast<std::size_t>(ce.stazioni[j]);
            f[k] -= G * RHO * ce.acqua_stazioni[j];
            m[k] += RHO * ce.acqua_stazioni[j];
        }
    // carichi d'inerzia (d'Alembert): la nave non e' in equilibrio mentre imbarca acqua; l'accelerazione di sussulto e
    // di beccheggio distribuita sulle masse annulla taglio e momento alle estremita'
    double massa = 0, F = 0, xg = 0;
    for (std::size_t k = 0; k < n; ++k) { massa += m[k]; F += f[k]; xg += m[k] * c.stazioni[k].x; }
    xg /= massa;
    double Mg = 0, I = 0;
    for (std::size_t k = 0; k < n; ++k) {
        const double r = c.stazioni[k].x - xg;
        Mg += f[k] * r; I += m[k] * r * r;
    }
    const double a0 = F / massa, alfa = I > 0 ? Mg / I : 0.0;
    for (std::size_t k = 0; k < n; ++k) f[k] -= m[k] * (a0 + alfa * (c.stazioni[k].x - xg));
    const double ct = std::cos(c.assetto.appoppamento);
    double peggiore = 0;
    std::size_t kk = 0;
    for (std::size_t k = 2; k + 2 < n; ++k) {
        const double xb = (c.stazioni[k - 1].x + c.stazioni[k].x) / 2;
        double M = 0;
        for (std::size_t j = 0; j < k; ++j) M += f[j] * (xb - c.stazioni[j].x) * ct;
        if (std::fabs(M) > peggiore) { peggiore = std::fabs(M); kk = k; }
    }
    if (peggiore > c.capacita_trave) {
        char t[96];
        std::snprintf(t, sizeof t, "la trave cede: momento %.0f MN*m oltre i %.0f", peggiore / 1e6, c.capacita_trave / 1e6);
        spezza(i, kk, t);
    }
}

void DanniNave::verifica_paratie(Corpo& c) {
    for (auto& ap : c.aperture) {
        if (ap.ceduta || ap.cedimento > 1e8) continue;
        const auto& A = c.celle[static_cast<std::size_t>(ap.a)];
        const auto& B = c.celle[static_cast<std::size_t>(ap.b)];
        const double z = c.assetto.quota(ap.x, ap.y, ap.z);
        const double dh = std::fabs(std::max(0.0, A.livello - z) - std::max(0.0, B.livello - z));
        if (dh < ap.cedimento) continue;
        ap.ceduta = true;
        ap.area = std::max(ap.area, 3.0);
        annota("cede la paratia fra " + A.dati.nome + " e " + B.dati.nome);
    }
}

void DanniNave::spezza(std::size_t i, std::size_t k, const std::string& causa) {
    Corpo vecchio = std::move(corpi_[i]);
    const std::size_t n = vecchio.stazioni.size();
    if (k < 1 || k >= n) { corpi_[i] = std::move(vecchio); return; }
    const double x_taglio = (vecchio.stazioni[k - 1].x + vecchio.stazioni[k].x) / 2;
    Corpo parti[2];   // 0 poppa (stazioni < k), 1 prua
    std::vector<int> indice[2];
    for (int p = 0; p < 2; ++p) {
        const std::size_t s0 = p == 0 ? 0 : k, s1 = p == 0 ? k : n;
        auto& c = parti[p];
        c.zg = vecchio.zg;
        c.capacita_trave = vecchio.capacita_trave;
        for (std::size_t s = s0; s < s1; ++s) {
            c.stazioni.push_back(vecchio.stazioni[s]);
            c.sezioni.push_back(vecchio.sezioni[s]);
            c.spinta.push_back(vecchio.spinta[s]);
            c.massa_stazioni.push_back(vecchio.massa_stazioni[s]);
        }
        indice[p].assign(vecchio.celle.size(), -1);
        for (std::size_t j = 0; j < vecchio.celle.size(); ++j) {
            const auto& ce = vecchio.celle[j];
            Cella nuova;
            nuova.dati = ce.dati;
            double vol = 0;
            for (std::size_t q = 0; q < ce.stazioni.size(); ++q) {
                const auto s = static_cast<std::size_t>(ce.stazioni[q]);
                if (s < s0 || s >= s1) continue;
                nuova.stazioni.push_back(static_cast<int>(s - s0));
                nuova.poligoni.push_back(ce.poligoni[q]);
                vol += area_poligono(ce.poligoni[q]) * dati_.passo;
            }
            if (nuova.stazioni.empty()) continue;
            nuova.volume = vol;
            nuova.acqua = ce.acqua * vol / std::max(ce.volume, 1e-9);
            nuova.acqua_stazioni.assign(nuova.stazioni.size(), 0.0);
            nuova.fuoco = ce.fuoco; nuova.combustibile = ce.combustibile;
            indice[p][j] = static_cast<int>(c.celle.size());
            c.celle.push_back(std::move(nuova));
        }
        for (const auto& f : vecchio.falle) {
            const int nc = f.cella >= 0 ? indice[p][static_cast<std::size_t>(f.cella)] : -1;
            if (nc < 0 || (p == 0) != (f.x < x_taglio)) continue;
            Falla g = f; g.cella = nc; c.falle.push_back(g);
        }
        for (const auto& ap : vecchio.aperture) {
            const int a = indice[p][static_cast<std::size_t>(ap.a)], b = indice[p][static_cast<std::size_t>(ap.b)];
            if (a < 0 || b < 0) continue;
            Apertura g = ap; g.a = a; g.b = b; c.aperture.push_back(g);
        }
        // struttura lacerata vicino al taglio (paratie squarciate entro il 25% della lunghezza, fondo aperto entro il
        // 15%) e scafo scosso: porte, passaggi e tubolature di tutte le paratie perdono (tarato sui tempi del dossier
        // 07 §11.4: troncone poppiero in 0,3-5 minuti, prodiero in 2-120)
        for (auto& ap : c.aperture) {
            if (std::fabs(ap.x - x_taglio) < 0.25 * dati_.lunghezza) ap.area = std::max(ap.area, 2.0);
            else ap.area = std::max(ap.area, 0.05);
        }
        for (auto& f : c.falle) if (f.coperta) f.area = std::max(f.area, 0.6);   // boccaporti e prese d'aria divelti
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            const auto& d = c.celle[j].dati;
            const double xc = std::clamp(x_taglio, d.x0, d.x1);
            if (std::fabs(xc - x_taglio) > 0.15 * dati_.lunghezza) continue;
            const auto& st = c.stazioni[static_cast<std::size_t>(c.celle[j].stazioni[c.celle[j].stazioni.size() / 2])];
            c.falle.push_back({static_cast<int>(j), st.x, std::clamp(0.0, d.y0, d.y1), st.z_chiglia, 3.0, false, false});
        }
        // le celle sul taglio sono aperte al mare per tutta la sezione
        const std::size_t sk = p == 0 ? c.stazioni.size() - 1 : 0;
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            const auto& ce = c.celle[j];
            if (std::find(ce.stazioni.begin(), ce.stazioni.end(), static_cast<int>(sk)) == ce.stazioni.end()) continue;
            Punto2 bc;
            const double A = area_poligono(ce.poligoni[static_cast<std::size_t>(std::find(ce.stazioni.begin(), ce.stazioni.end(),
                                                                                          static_cast<int>(sk)) - ce.stazioni.begin())], &bc);
            c.falle.push_back({static_cast<int>(j), x_taglio, bc.y, bc.z, 0.8 * A, false, false});
        }
        // nuova origine nel baricentro del troncone, con la stessa posa nel mondo
        double m = 0, mx = 0;
        for (std::size_t s = 0; s < c.stazioni.size(); ++s) { m += c.massa_stazioni[s]; mx += c.massa_stazioni[s] * c.stazioni[s].x; }
        c.assetto = vecchio.assetto;
        c.assetto.x_o = mx / m;
        double X, Y, Z;
        vecchio.assetto.mondo(c.assetto.x_o, 0, 0, X, Y, Z);
        c.assetto.dx = X - c.assetto.x_o; c.assetto.dy = Y; c.assetto.immersione = -Z;
        c.v_imm = vecchio.v_imm; c.v_sb = vecchio.v_sb; c.v_ap = vecchio.v_ap;
        c.v_dx = p == 0 ? -0.15 : 0.15;
    }
    for (auto& pz : parti)
        if (pz.stazioni.size() < 5) { pz.affondato = true; annota("un troncone piccolo affonda subito"); }
    if (corpi_.size() == 1) x_rottura_ = x_taglio;
    corpi_[i] = std::move(parti[0]);
    corpi_.insert(corpi_.begin() + static_cast<std::ptrdiff_t>(i) + 1, std::move(parti[1]));
    char t[160];
    std::snprintf(t, sizeof t, "nave spezzata a %.0f m dal centro (%s)", x_taglio, causa.c_str());
    annota(t);
    condannata_ = true;
}

// --- esplosioni -------------------------------------------------------------------------------------------------

void DanniNave::urto_meccanico(const DanniUrtoUnita& d) {
    char t[160];
    std::snprintf(t, sizeof t, "collisione: urto su %s a %.0f m (%.1f MJ)%s%s%s%s", nome_parte_urto(d.parte), d.x_m, d.energia_mj,
                  d.duomo_perso ? ", duomo sonar perso" : "", d.eliche_danneggiate ? ", eliche danneggiate" : "",
                  d.timone_bloccato ? ", timone bloccato" : "", d.sovrastrutture ? ", sovrastrutture danneggiate" : "");
    annota(t);
    if (d.falla_nave_m2 <= 0.0 || d.z_m >= 0.0) return;
    std::size_t ic = 0;
    for (std::size_t i = 0; i < corpi_.size(); ++i)
        if (d.x_m >= corpi_[i].stazioni.front().x - dati_.passo && d.x_m <= corpi_[i].stazioni.back().x + dati_.passo) ic = i;
    auto& c = corpi_[ic];
    const auto& st = c.stazioni[stazione_vicina(c.stazioni, d.x_m)];
    const double z = std::max(d.z_m, st.z_chiglia + 0.1);
    const double y = (d.y_m >= 0 ? 1.0 : -1.0) * semilarghezza(st, z);
    // le celle sul fianco lungo lo squarcio (l'area in proporzione al tratto), o la cella al punto
    const double x0 = d.x_m - d.squarcio_m / 2, x1 = d.x_m + d.squarcio_m / 2;
    int migliore = -1;
    double dist = 1e9, tratto = 0.0;
    std::vector<std::pair<int, double>> celle;
    for (std::size_t j = 0; j < c.celle.size(); ++j) {
        const auto& cd = c.celle[j].dati;
        if (y * 0.95 < cd.y0 || y * 0.95 > cd.y1) continue;
        const double dentro = std::min(x1, cd.x1) - std::max(x0, cd.x0);
        if (d.squarcio_m > 0.0 && dentro > 0.0) {
            celle.emplace_back(static_cast<int>(j), dentro);
            tratto += dentro;
        }
        const double dx = d.x_m < cd.x0 ? cd.x0 - d.x_m : d.x_m >= cd.x1 ? d.x_m - cd.x1 : 0.0;
        if (dx < dist) { dist = dx; migliore = static_cast<int>(j); }
    }
    if (celle.empty() && migliore >= 0) celle.emplace_back(migliore, tratto = 1.0);
    for (const auto& [j, l] : celle) {
        const auto& cd = c.celle[static_cast<std::size_t>(j)].dati;
        const double xc = std::clamp(d.x_m, cd.x0, cd.x1);
        c.falle.push_back({j, xc, y, z, d.falla_nave_m2 * l / tratto, false, false});
    }
}

void DanniNave::colpo(const Esplosione& e) {
    std::size_t ic = 0;
    for (std::size_t i = 0; i < corpi_.size(); ++i)
        if (e.x >= corpi_[i].stazioni.front().x - dati_.passo && e.x <= corpi_[i].stazioni.back().x + dati_.passo) ic = i;
    auto& c = corpi_[ic];
    const double W = std::max(e.tnt_kg, 0.1), W3 = std::cbrt(W);
    const std::size_t ks = stazione_vicina(c.stazioni, e.x);
    const auto& st = c.stazioni[ks];
    char t[200];
    auto cella_in = [&](double x, double y, double z) -> int {
        int migliore = -1;
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            const auto& d = c.celle[j].dati;
            if (x >= d.x0 && x < d.x1 && y >= d.y0 && y <= d.y1 && z <= st.coperta() + 0.5) migliore = static_cast<int>(j);
        }
        return migliore;
    };
    const bool sopra = e.z > 0 || e.tipo == Esplosione::Tipo::missile;
    {   // per gli effetti: sul fianco se a contatto, altrimenti nel punto di scoppio
        const double zs = sopra ? std::clamp(e.z, 0.5, st.coperta() - 0.5) : e.z;
        const double ys = sopra || e.contatto ? (e.y >= 0 ? 1.0 : -1.0) * semilarghezza(st, zs) : e.y;
        Scoppio sc{tempo_, e.tipo, 0, 0, 0, W};
        c.assetto.mondo(e.x, ys, zs, sc.x, sc.y, sc.z);
        scoppi_.push_back(sc);
    }
    if (sopra) {
        std::snprintf(t, sizeof t, "colpo sopra il galleggiamento a %.0f m (%.0f kg)", e.x, W);
        annota(t);
        const double lato = e.y >= 0 ? 1.0 : -1.0;
        const double z = std::clamp(e.z, 0.5, st.coperta() - 0.5);
        const double y = lato * semilarghezza(st, z);
        const int jf = cella_in(e.x, y * 0.95, z), j = cella_in(e.x, 0.0, z);   // falla sul fianco, scoppio all'interno
        // il proietto d'artiglieria passa la murata e scoppia dentro: un foro piu' piccolo di quello di un missile (STIMA:
        // (0,35 W^1/3)^2, 0,1 m^2 col 76 mm, 0,3 col 5", 2 col 16")
        const double foro = e.tipo == Esplosione::Tipo::proietto ? std::pow(0.35 * W3, 2) : std::pow(0.7 * W3, 2);
        if (jf >= 0) c.falle.push_back({jf, e.x, y, z, foro, false, false});
        if (j >= 0) {
            auto& ce = c.celle[static_cast<std::size_t>(j)];
            // carburante residuo del missile (dossier 07 §4); il proietto d'artiglieria ne appicca di meno (STIMA)
            const bool proietto = e.tipo == Esplosione::Tipo::proietto;
            // il carburante volatile (JP-5, benzina avio) si accende una volta e mezza piu' spesso e parte piu' forte
            const bool vol = carburante_volatile(ce.dati.carburante) && ce.dati.carburante_t > 0;
            double p_fuoco = (proietto ? (massa_nave_ < 5e6 ? 0.12 : 0.05) : massa_nave_ < 5e6 ? 0.8 : 0.5) * (vol ? 1.5 : 1.0);
            // il greggio della petroliera si accende meno di quanto dica la stazza: 20-40% dei missili (dossier 07 §5.3)
            if (!proietto && dati_.tipo == "petroliera" && ce.dati.uso == "carico") p_fuoco = 0.3;
            if (caso_.uniforme() < p_fuoco) { ce.fuoco = std::max(ce.fuoco, vol ? 0.8 : 0.6); annota("incendio in " + ce.dati.nome); }
            // il deposito salta col 4% dei missili; coi proietti in proporzione alla carica, fino a 25 kg (STIMA: 0,1% col
            // 76 mm, 0,6% col 5", 4% col 16"), secondo il contenuto; la protezione (corazza, paratie, sprinkler) toglie la sua parte
            const double p_deposito = (proietto ? 0.04 * std::min(1.0, W / 25.0) : 0.04) * sensibilita_deposito(ce.dati.deposito) * (1.0 - ce.dati.protezione);
            if (ce.dati.uso == "deposito" && caso_.uniforme() < p_deposito) esplosione_deposito(ic, j);
            // i vapori delle cisterne vuote: coi proietti in proporzione alla carica, come il deposito (STIMA)
            if (in_zavorra() && ce.dati.uso == "carico" && caso_.uniforme() < p_vapori_ * (proietto ? std::min(1.0, W / 25.0) : 1.0))
                esplosione_vapori(ic, j, W);
        }
        for (auto& z2 : zone_)
            if (e.x >= z2.dati.x0 && e.x <= z2.dati.x1 && (e.tipo != Esplosione::Tipo::proietto || caso_.uniforme() < 0.1)) {
                z2.fuoco = std::max(z2.fuoco, 0.5);
                annota("incendio: " + z2.dati.nome);
                // i lanciatori in coperta, senza protezione: come un deposito colpito
                if (z2.dati.carica_kg > 0 && caso_.uniforme() < (e.tipo == Esplosione::Tipo::proietto ? 0.04 * std::min(1.0, W / 25.0) : 0.04))
                    esplosione_in_coperta(z2);
            }
        return;
    }
    // sott'acqua: bolla, falla, rottura della chiglia
    const double A_max = 3.50 * W3 / std::cbrt(-e.z + 10.0);
    const double hb_z = semilarghezza(st, e.z);
    const bool sotto_chiglia = e.z < st.z_chiglia + 0.5 && std::fabs(e.y) <= std::max(hb_z, 1.0);
    const double s = e.contatto ? 0.0 : (sotto_chiglia ? st.z_chiglia - e.z : std::max(0.0, std::fabs(e.y) - hb_z));
    std::snprintf(t, sizeof t, "esplosione subacquea a %.0f m, %.1f m %s (%.0f kg, bolla di %.1f m)", e.x, s,
                  sotto_chiglia ? "sotto la chiglia" : "dal fianco", W, A_max);
    annota(t);
    // sotto la chiglia, senza contatto: l'urto e la bolla sollevano la nave (si vede nelle SINKEX) prima della frusta
    if (sotto_chiglia && !e.contatto && s < 1.5 * A_max) {
        const double v = std::min(3.0, 15.0 * W3 / (s + 5.0));
        for (auto& k : corpi_) if (!k.affondato) k.v_imm -= v;
        // inarcamento (hogging) mentre la bolla si espande, poi insellamento al collasso (dossier 07 §3.3); ampiezza
        // stimata 0,4 A_max (4 m per un Mk 48), solo per il disegno
        frusta_t0_ = tempo_;
        frusta_x_ = e.x;
        frusta_m_ = std::min(0.4 * A_max, 5.0) * (1.0 - s / (1.5 * A_max));
        frusta_T1_ = 2.11 * W3 / std::pow(-e.z + 10.0, 5.0 / 6.0);
    }
    const double L = dati_.lunghezza;
    const double x_mid = (corpi_.front().stazioni.front().x + corpi_.back().stazioni.back().x) / 2;
    // falla: a contatto D = 1,25 W^(1/3); a distanza piu' piccola (fondo squarciato), nulla oltre il raggio della bolla
    if (e.contatto || s < A_max) {
        const double D = 1.25 * W3 * (e.contatto ? 1.0 : std::max(0.3, 1.0 - s / A_max));
        const double area = PI * D * D / 4;
        const double yh = sotto_chiglia ? 0.0 : (e.y >= 0 ? 1.0 : -1.0) * semilarghezza(st, e.z);
        const double zh = sotto_chiglia ? st.z_chiglia : e.z;
        // la falla si divide fra le celle che attraversa (sul fianco o sul fondo)
        std::vector<std::pair<int, double>> parti;
        double totale = 0;
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            const auto& d = c.celle[j].dati;
            const double ov = std::min(d.x1, e.x + D / 2) - std::max(d.x0, e.x - D / 2);
            const bool lato_ok = sotto_chiglia ? (d.y0 <= 0 && d.y1 >= 0) : (yh >= d.y0 && yh <= d.y1) ||
                                 (yh < 0 ? d.y0 < -1e2 : d.y1 > 1e2);
            if (ov > 0 && lato_ok) { parti.push_back({static_cast<int>(j), ov}); totale += ov; }
        }
        for (const auto& [j, ov] : parti) {
            const auto& d = c.celle[static_cast<std::size_t>(j)].dati;
            const double xc = std::clamp(e.x, d.x0 + 0.1, d.x1 - 0.1);
            c.falle.push_back({j, xc, yh, zh, area * ov / totale, false, false});
        }
        // paratie trasversali squarciate entro la falla (fra celle colpite); la paratia interna della cassa laterale
        // colpita sfondata verso il centro
        auto colpita = [&](int j) { return std::any_of(parti.begin(), parti.end(), [j](const auto& pp) { return pp.first == j; }); };
        for (auto& ap : c.aperture) {
            const auto& da = c.celle[static_cast<std::size_t>(ap.a)].dati;
            const auto& db = c.celle[static_cast<std::size_t>(ap.b)].dati;
            const bool trasversale = std::fabs(da.x0 - db.x0) > 1e-3;
            if (trasversale && std::fabs(ap.x - e.x) < D / 2 + 1.0 && (colpita(ap.a) || colpita(ap.b))) ap.area = std::max(ap.area, 0.25 * area);
            else if (trasversale && std::fabs(ap.x - e.x) < 1.5 * D) ap.area = std::max(ap.area, 0.1);   // porte e passaggi deformati
            if (!trasversale && W >= 64 && (colpita(ap.a) != colpita(ap.b)) && e.x >= da.x0 - D / 2 && e.x <= da.x1 + D / 2)
                ap.area = std::max(ap.area, 0.3 * D * D);
        }
        std::snprintf(t, sizeof t, "falla di %.1f m (%.0f m^2) in %zu locali", D, area, parti.size());
        annota(t);
        bool vapori = in_zavorra();   // una sola estrazione per colpo, anche se la falla prende piu' cisterne
        for (const auto& [j, ov] : parti) {
            auto& ce = c.celle[static_cast<std::size_t>(j)];
            if (vapori && (ce.dati.uso == "carico" || ce.dati.uso == "ala")) {   // il fianco sono le cisterne laterali
                vapori = false;
                if (caso_.uniforme() < p_vapori_) { esplosione_vapori(ic, j, W); return; }
            }
            if (ce.dati.uso == "deposito" && caso_.uniforme() < 0.10 * sensibilita_deposito(ce.dati.deposito) * (1.0 - ce.dati.protezione)) { esplosione_deposito(ic, j); return; }
            if ((ce.dati.uso == "macchine" || ce.dati.uso == "ala") && caso_.uniforme() < 0.3) {
                ce.fuoco = std::max(ce.fuoco, 0.3);
                annota("incendio in " + ce.dati.nome);
            }
        }
    }
    // rottura della chiglia (dossier 07 §3.4, §11.3): carica sotto la chiglia nel tratto centrale
    if (s <= 1.5 * A_max && (sotto_chiglia || s < 0.5 * A_max)) {
        const double centro = std::fabs(e.x - x_mid) / L;
        double p;
        if (dati_.dislocamento_pubblicato_t > 20000 || L > 200) p = 0.08 * std::min(1.0, W / 400);
        else if (L < 130) p = std::min(1.0, W / 150);
        else p = 0.7 * std::min(1.0, W / 400);
        if (centro <= 0.25 && sotto_chiglia) {
            if (caso_.uniforme() < p) { spezza(ic, std::max<std::size_t>(ks, 1), "chiglia rotta dalla bolla"); return; }
            c.capacita_trave *= 0.6;
            annota("trave indebolita dalla frusta della bolla");
        } else if (centro > 0.3 && W >= 150 && caso_.uniforme() < 0.5 * std::min(1.0, W / (0.03 * massa_nave_ / 1000.0))) {
            // estremita' asportata: il taglio a mezza falla verso il centro
            const double D = 1.25 * W3;
            const double xt = e.x + (e.x > x_mid ? -D / 2 : D / 2);
            spezza(ic, std::max<std::size_t>(stazione_vicina(c.stazioni, xt), 1), e.x > x_mid ? "prua asportata" : "poppa asportata");
        }
    }
}

void DanniNave::esplosione_deposito(std::size_t ic, int j) {
    auto& c = corpi_[ic];
    auto& ce = c.celle[static_cast<std::size_t>(j)];
    // kg di TNT: il contenuto del locale piu' il propellente a 0,3 (danni critici, fase 2; uno scoppio e' un locale, la
    // catena la fa il fuoco); senza dati la vecchia stima, una frazione del dislocamento
    const double W = ce.dati.carica_kg > 0 ? std::min(ce.dati.carica_kg + 0.3 * ce.dati.propellente_kg, CARICA_MAX_SCOPPIO_KG)
                                           : std::clamp(0.5 * massa_nave_ / 1000.0, 500.0, CARICA_MAX_SCOPPIO_KG);
    char t[160];
    std::snprintf(t, sizeof t, "esplode il deposito %s (%s, circa %.0f kg)", ce.dati.nome.c_str(), ce.dati.deposito.c_str(), W);
    annota(t);
    const auto& st = c.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
    Scoppio sc{tempo_, Esplosione::Tipo::deposito, 0, 0, 0, W};
    c.assetto.mondo(st.x, 0.0, (st.z_chiglia + st.coperta()) / 2, sc.x, sc.y, sc.z);
    scoppi_.push_back(sc);
    const double D = 1.25 * std::cbrt(W);
    c.falle.push_back({j, st.x, 0.0, st.z_chiglia, PI * D * D / 4, false, false});
    ce.fuoco = 1.0;
    for (auto& ap : c.aperture)
        if (std::fabs(ap.x - st.x) < D) ap.area = std::max(ap.area, D * D * 0.5);
    for (auto& o : c.celle)
        if (std::fabs((o.dati.x0 + o.dati.x1) / 2 - st.x) < D * 2) o.fuoco = std::max(o.fuoco, 0.6);
    // la rottura secondo lo scoppio per tonnellata (fase 4)
    const double r = W / std::max(massa_nave_ / 1000.0, 1.0);
    if (r >= DISINTEGRA_KG_T) {
        annota("la nave salta in aria");
        disintegra(corpi_);
    } else if (caso_.uniforme() < std::clamp((r - SPEZZA_KG_T_0) / SPEZZA_KG_T_SCALA, 0.0, 1.0)) {
        spezza(ic, static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2]), "esplosione del deposito");
    } else {
        annota("lo scafo regge lo scoppio: la nave resta intera");
    }
}

void DanniNave::esplosione_vapori(std::size_t ic, int j, double W0) {
    auto& c = corpi_[ic];
    auto& ce = c.celle[static_cast<std::size_t>(j)];
    const double W = std::min(DANNO_VAPORI * W0, CARICA_MAX_SCOPPIO_KG);
    char t[160];
    std::snprintf(t, sizeof t, "esplodono i vapori della cisterna %s (circa %.0f kg)", ce.dati.nome.c_str(), W);
    annota(t);
    const auto& st = c.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
    Scoppio sc{tempo_, Esplosione::Tipo::deposito, 0, 0, 0, W};
    c.assetto.mondo(st.x, 0.0, (st.z_chiglia + st.coperta()) / 2, sc.x, sc.y, sc.z);
    scoppi_.push_back(sc);
    // lo scoppio squarcia il fianco e abbatte le paratie attorno (Kong Haakon VII, dossier 07 §5.3)
    const double D = 1.25 * std::cbrt(W);
    c.falle.push_back({j, st.x, std::clamp(0.0, ce.dati.y0, ce.dati.y1), st.z_chiglia, PI * D * D / 4, false, false});
    ce.fuoco = 1.0;
    for (auto& ap : c.aperture)
        if (std::fabs(ap.x - st.x) < D) ap.area = std::max(ap.area, D * D * 0.5);
    if (caso_.uniforme() < P_SPEZZA_VAPORI)
        spezza(ic, std::max<std::size_t>(static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2]), 1), "esplosione dei vapori");
    else
        annota("lo scafo regge lo scoppio dei vapori: la nave resta intera");
}

void DanniNave::esplosione_in_coperta(ZonaIncendio& z) {
    // i missili dei lanciatori in coperta (danni critici, fase 2): lo scoppio sopra la coperta, il fuoco pieno nella
    // zona; lo scafo sotto non si apre, ma il fuoco passa ai locali sotto (fase 3)
    const double W = std::min(z.dati.carica_kg, CARICA_MAX_SCOPPIO_KG);
    z.dati.carica_kg = 0.0;
    z.fuoco = 1.0;
    char t[160];
    std::snprintf(t, sizeof t, "esplodono i missili di %s (circa %.0f kg)", z.dati.nome.c_str(), W);
    annota(t);
    const double x = (z.dati.x0 + z.dati.x1) / 2;
    for (auto& c : corpi_) {
        if (c.stazioni.empty() || x < c.stazioni.front().x || x > c.stazioni.back().x) continue;
        const auto& st = c.stazioni[stazione_vicina(c.stazioni, x)];
        Scoppio sc{tempo_, Esplosione::Tipo::deposito, 0, 0, 0, W};
        c.assetto.mondo(x, 0.0, st.coperta() + 1.0, sc.x, sc.y, sc.z);
        scoppi_.push_back(sc);
        if (W / std::max(massa_nave_ / 1000.0, 1.0) >= DISINTEGRA_KG_T) {   // una nave piccola salta in aria (fase 4)
            annota("la nave salta in aria");
            disintegra(corpi_);
            return;
        }
        for (auto& v : c.celle)   // la coperta sfondata: il locale sotto lo scoppio prende fuoco
            if (x >= v.dati.x0 && x < v.dati.x1 && v.dati.y0 <= 0.0 && v.dati.y1 >= 0.0 && v.riempimento() < 0.4) {
                if (v.fuoco <= 0) annota("incendio in " + v.dati.nome);
                v.fuoco = std::max(v.fuoco, 0.6);
            }
        break;
    }
}

// --- incendi e controllo danni (ogni secondo) ---------------------------------------------------------------------

void DanniNave::passo_incendi_e_controllo(double h) {
    const double q = dati_.qualita_controllo_danni;
    const bool militare = dati_.tipo == "militare" || dati_.tipo == "sottomarino";
    // energia: almeno un locale macchine asciutto e non in fiamme
    bool macchine = false, ok = false;
    for (const auto& c : corpi_)
        for (const auto& ce : c.celle)
            if (ce.dati.uso == "macchine") { macchine = true; ok |= ce.riempimento() < 0.5 && ce.fuoco < 0.7 && !c.affondato; }
    const bool energia = !macchine || ok;
    if (energia != energia_) annota(energia ? "energia ripristinata" : "perdita di energia: pompe e collettore antincendio fermi");
    energia_ = energia;
    // incendi
    for (std::size_t ic = 0; ic < corpi_.size(); ++ic) {
        auto& c = corpi_[ic];
        for (std::size_t j = 0; j < c.celle.size(); ++j) {
            auto& ce = c.celle[j];
            if (ce.fuoco <= 0) continue;
            ce.annerimento = std::min(1.0, ce.annerimento + ce.fuoco * h / 900.0);   // annerito in un quarto d'ora
            if (ce.riempimento() > 0.4 || c.affondato) { ce.fuoco = 0; continue; }
            if (ce.combustibile > 0) ce.fuoco += CRESCITA_FUOCO * ce.fuoco * (1 - ce.fuoco) * h;
            else ce.fuoco -= 0.01 * h;
            ce.combustibile -= ce.fuoco * h;
            ce.fuoco = std::clamp(ce.fuoco, 0.0, 1.0);
            const auto vicina = [&](std::size_t o) {
                const auto& v = c.celle[o];
                return std::fabs(v.dati.x0 - ce.dati.x1) < 1e-3 || std::fabs(v.dati.x1 - ce.dati.x0) < 1e-3 ||
                       (std::fabs(v.dati.x0 - ce.dati.x0) < 1e-3 && o != j);
            };
            if (ce.fuoco > 0.6)   // propagazione ai locali vicini
                for (std::size_t o = 0; o < c.celle.size(); ++o) {
                    auto& v = c.celle[o];
                    if (vicina(o) && v.fuoco <= 0 && v.combustibile > 0 && v.riempimento() < 0.4 &&
                        caso_.uniforme() < peso_propagazione(v.dati) * h / 400) {
                        v.fuoco = 0.1;
                        annota("l'incendio si estende a " + v.dati.nome);
                    }
                }
            if (ce.dati.uso == "deposito" && ce.fuoco > 0.2) {
                // allagamento d'emergenza dei depositi: 2 minuti (diviso la qualita') con l'energia, il doppio con le
                // valvole a mano; nel frattempo rischio di esplosione
                if (ce.allagata_per_incendio <= 0) ce.allagata_per_incendio = 120.0 / q;
                if (ce.allagata_per_incendio > 0) {
                    ce.allagata_per_incendio -= energia_ ? h : h / 2;
                    if (ce.allagata_per_incendio <= 0) {
                        ce.acqua = std::max(ce.acqua, 0.8 * ce.volume * ce.dati.permeabilita);
                        ce.fuoco = 0;
                        ce.allagata_per_incendio = 0.0;   // zero esatto: le pompe lo lasciano allagato
                        annota("deposito " + ce.dati.nome + " allagato contro l'incendio");
                        continue;
                    }
                }
                // rischio di esplosione: 0,6% al minuto a fuoco pieno (circa il 30% in un'ora senza allagamento)
                // col locale vicino in fiamme il deposito si scalda anche da fuori, e il rischio raddoppia (T5, STIMA)
                bool scaldato = false;
                for (std::size_t o = 0; o < c.celle.size() && !scaldato; ++o) scaldato = vicina(o) && c.celle[o].fuoco > 0.6;
                if (ce.fuoco > 0.5 && caso_.uniforme() < 0.0001 * (scaldato ? 2.0 : 1.0) * sensibilita_deposito(ce.dati.deposito) * ce.fuoco * (1.0 - ce.dati.protezione) * h) {
                    esplosione_deposito(ic, static_cast<int>(j));
                    return;
                }
            }
        }
    }
    for (auto& z : zone_) {
        if (z.fuoco <= 0) {   // il fuoco sale: oltre 0,6 un locale accende la zona sopra di lui (T5), l'hangar prima
            if (z.combustibile <= 0) continue;
            for (const auto& c : corpi_) {
                if (c.affondato || z.fuoco > 0) continue;
                for (const auto& v : c.celle)
                    if (v.fuoco > 0.6 && v.dati.x1 > z.dati.x0 && v.dati.x0 < z.dati.x1 && v.dati.y0 <= 0.0 && v.dati.y1 >= 0.0 &&
                        caso_.uniforme() < (z.dati.carico == "avio" ? 3.0 : 1.0) * h / 400) {
                        z.fuoco = 0.1;
                        annota("l'incendio di " + v.dati.nome + " sale a " + z.dati.nome);
                        break;
                    }
            }
            continue;
        }
        z.annerimento = std::min(1.0, z.annerimento + z.fuoco * h / 900.0);
        if (z.combustibile > 0) z.fuoco += CRESCITA_FUOCO * z.fuoco * (1 - z.fuoco) * h;
        else z.fuoco -= 0.01 * h;
        z.combustibile -= z.fuoco * h;
        z.fuoco = std::clamp(z.fuoco, 0.0, 1.0);
        // il cook-off dei missili nei lanciatori in fiamme, come nei depositi ma senza allagamento
        if (z.dati.carica_kg > 0 && z.fuoco > 0.5 && caso_.uniforme() < 0.0001 * z.fuoco * h) esplosione_in_coperta(z);
        // l'hangar in fiamme: l'elicottero e' perso (fase 3)
        if (z.dati.carico == "avio" && z.fuoco > 0.5 && !z.elicottero_perso) {
            z.elicottero_perso = true;
            annota("elicottero perso nell'incendio di " + z.dati.nome);
        }
        // il fuoco scende sotto coperta: oltre 0,6 passa ai locali sotto la zona, come fra locali vicini (1/400 al secondo)
        if (z.fuoco > 0.6)
            for (auto& c : corpi_) {
                if (c.affondato) continue;
                for (auto& v : c.celle)
                    if (v.dati.x1 > z.dati.x0 && v.dati.x0 < z.dati.x1 && v.dati.y0 <= 0.0 && v.dati.y1 >= 0.0 && v.fuoco <= 0 &&
                        v.combustibile > 0 && v.riempimento() < 0.4 && caso_.uniforme() < peso_propagazione(v.dati) * h / 400) {
                        v.fuoco = 0.1;
                        annota("l'incendio di " + z.dati.nome + " scende in " + v.dati.nome);
                    }
            }
    }
    // squadre del controllo danni, per priorita': falle tamponabili, incendi, pompe, controallagamento; dopo
    // l'abbandono nave nessuno lavora piu'
    double squadre = condannata_ ? 0.0 : dati_.squadre * (energia_ ? 1.0 : 0.7);
    for (auto& c : corpi_) {
        if (c.affondato || condannata_) continue;
        for (auto& f : c.falle) {
            if (squadre <= 0) break;
            if (f.coperta || f.controallagamento || f.area <= 0 || f.area > 1.0) continue;
            if (c.assetto.quota(f.x, f.y, f.z) >= 0) continue;
            f.area = std::max(0.0, f.area - 0.002 * q * h);
            squadre -= 1;
        }
        for (auto& ce : c.celle) {
            if (squadre <= 0) break;
            if (ce.fuoco <= 0) continue;
            ce.fuoco = std::max(0.0, ce.fuoco - 0.0004 * q * (energia_ ? 1.0 : 0.4) * h);
            squadre -= 1;
        }
        for (auto& z : zone_) {
            if (squadre <= 0) break;
            if (z.fuoco <= 0) continue;
            z.fuoco = std::max(0.0, z.fuoco - 0.0004 * q * (energia_ ? 1.0 : 0.4) * h);
            squadre -= 1;
        }
        // pompe: locali con acqua senza falle aperte sotto il mare
        // pompe fisse con l'energia (0,02 m^3/s per 1000 t di dislocamento) e motopompe portatili delle squadre libere
        double portata = (energia_ ? 0.02 * q * massa_nave_ / 1e6 : 0.0) + std::max(0.0, squadre) * 0.016 * q;
        for (std::size_t j = 0; j < c.celle.size() && portata > 0; ++j) {
            auto& ce = c.celle[j];
            if (ce.acqua <= 0 || ce.allagata_per_incendio == 0 || ce.dati.uso == "zavorra") continue;   // casse: si soffiano
            bool aperta = false;
            for (const auto& f : c.falle)
                aperta |= f.cella == static_cast<int>(j) && f.area > 0.01 && !f.controallagamento && c.assetto.quota(f.x, f.y, f.z) < 0;
            if (aperta) continue;
            const double v = std::min(ce.acqua, portata * h);
            ce.acqua -= v;
            portata -= v / h;
        }
        // sottomarini: esaurimento d'emergenza delle casse zavorra integre appena il battello imbarca acqua (dossier 02
        // §9.2); in superficie l'aria delle bombole basta a svuotarle tutte, senza squadre ne' energia
        if (dati_.tipo == "sottomarino") {
            if (!soffiaggio_)
                for (const auto& ce : c.celle)
                    if (ce.acqua > ce.dati.acqua_iniziale + 1.0) {
                        soffiaggio_ = true;
                        annota("esaurimento d'emergenza delle casse zavorra");
                        break;
                    }
            for (std::size_t j = 0; j < c.celle.size() && soffiaggio_; ++j) {
                auto& ce = c.celle[j];
                if (ce.dati.uso != "zavorra" || ce.acqua <= 0) continue;
                bool aperta = false;
                for (const auto& f : c.falle)
                    aperta |= f.cella == static_cast<int>(j) && f.area > 0.01 && c.assetto.quota(f.x, f.y, f.z) < 0;
                if (!aperta) ce.acqua -= std::min(ce.acqua, PORTATA_SOFFIO * h);
            }
        }
        // controallagamento (navi militari con energia): sbandamento oltre 4 gradi
        const double sb = gradi(c.assetto.sbandamento);
        if (militare && energia_ && std::fabs(sb) > 4.0 && std::fabs(sb) < 30.0) {
            const bool aperta = std::any_of(c.falle.begin(), c.falle.end(), [](const Falla& f) { return f.controallagamento && f.area > 0; });
            if (!aperta)   // la cassa laterale piu' vuota sul lato alto (sbandamento sulla dritta: alto a sinistra)
                for (std::size_t j = 0; j < c.celle.size(); ++j) {
                    const auto& ce = c.celle[j];
                    if (ce.dati.uso != "ala" || ce.riempimento() > 0.5) continue;
                    if ((sb > 0) != (ce.dati.y0 > 0)) continue;
                    const auto& st = c.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
                    c.falle.push_back({static_cast<int>(j), st.x, ce.dati.y0 > 0 ? semilarghezza(st, st.z_chiglia + 0.5) : -semilarghezza(st, st.z_chiglia + 0.5),
                                       st.z_chiglia + 0.5, 0.15, false, true});
                    annota("controallagamento di " + ce.dati.nome);
                    break;
                }
        }
        // la valvola si chiude quando il suo lato non e' piu' quello alto di almeno 1,5 gradi
        for (auto& f : c.falle)
            if (f.controallagamento && f.area > 0 && (f.y > 0 ? sb < 1.5 : sb > -1.5)) f.area = 0;
    }
    // abbandono nave
    if (!condannata_) {
        const auto& p = principale(corpi_);
        if (std::fabs(gradi(p.assetto.sbandamento)) > 25 || std::fabs(gradi(p.assetto.appoppamento)) > 8) {
            condannata_ = true;
            annota("abbandono nave");
        }
    }
}

void DanniNave::avanza(double dt) {
    const double fine = tempo_ + dt;
    while (tempo_ < fine - 1e-9) {
        const double h = std::min(PASSO_DINAMICA, fine - tempo_);
        accumulo_ += h;
        const bool livelli_ora = accumulo_ >= PASSO_LIVELLI - 1e-9;
        if (livelli_ora) accumulo_ = 0;
        for (std::size_t i = 0; i < corpi_.size(); ++i) {
            auto& c = corpi_[i];
            if (c.affondato) continue;
            if (livelli_ora) livelli(c);
            passo_allagamento(c, h);
            passo_dinamica(c, h);
        }
        tempo_ += h;
        accumulo_lento_ += h;
        if (accumulo_lento_ >= PASSO_LENTO) {
            accumulo_lento_ -= PASSO_LENTO;
            passo_incendi_e_controllo(PASSO_LENTO);
            for (auto& c : corpi_) if (!c.affondato) verifica_paratie(c);
            for (std::size_t i = 0; i < corpi_.size(); ++i) verifica_trave(i);
        }
    }
}

}  // namespace m688
