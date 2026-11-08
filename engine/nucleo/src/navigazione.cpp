#include "nucleo/navigazione.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

constexpr double NM = 1852.0, KN = 1852.0 / 3600.0, GIORNO_S = 86400.0;

std::string s(const toml::table& t, const char* k) { return t[k].value_or(std::string{}); }
double d(const toml::table& t, const char* k, double riserva = 0.0) { return t[k].value_or(riserva); }
bool b(const toml::table& t, const char* k) { return t[k].value_or(false); }

double gauss(Casuale& r) {
    const double u = 1.0 - r.uniforme(), v = r.uniforme();
    return std::sqrt(-2.0 * std::log(u)) * std::cos(2.0 * PI * v);
}

// FNV-1a: il seme dei passaggi dipende dal punto nave, uguale su ogni piattaforma
std::uint64_t impronta(const std::string& x) {
    std::uint64_t h = 1469598103934665603ull;
    for (const unsigned char c : x) h = (h ^ c) * 1099511628211ull;
    return h;
}

Vec2 versore(double rilevamento_deg) { return {std::sin(radianti(rilevamento_deg)), std::cos(radianti(rilevamento_deg))}; }

// indici della covarianza [ex, ey, bx, by]
constexpr int E[2] = {0, 1}, B[2] = {2, 3};
double& at(std::array<double, 16>& m, int r, int c) { return m[static_cast<std::size_t>(r * 4 + c)]; }
double at(const std::array<double, 16>& m, int r, int c) { return m[static_cast<std::size_t>(r * 4 + c)]; }

std::array<double, 16> per(const std::array<double, 16>& a, const std::array<double, 16>& b) {
    std::array<double, 16> o{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) {
            double x = 0.0;
            for (int k = 0; k < 4; ++k) x += at(a, r, k) * at(b, k, c);
            at(o, r, c) = x;
        }
    return o;
}
std::array<double, 16> trasposta(const std::array<double, 16>& a) {
    std::array<double, 16> o{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) at(o, r, c) = at(a, c, r);
    return o;
}

}  // namespace

const SistemaNavigazione& CatalogoNavigazione::sistema_di(const std::string& classe) const {
    for (std::string c = classe; !c.empty();) {
        if (const auto it = classi.find(c); it != classi.end()) return sistemi.at(it->second);
        const auto u = c.rfind('_');
        if (u == std::string::npos) break;
        c.resize(u);
    }
    return sistemi.at(sistema_predefinito);
}

const DefPuntoNave* CatalogoNavigazione::punto(const std::string& id) const {
    const auto it = punti.find(id);
    return it == punti.end() ? nullptr : &it->second;
}

double CatalogoNavigazione::corrente_kn(const std::string& teatro, double riserva) const {
    const auto it = teatri.find(teatro);
    return it == teatri.end() ? riserva : it->second.corrente_kn;
}

CatalogoNavigazione carica_navigazione(const std::string& percorso) {
    const toml::table t = toml::parse_file(percorso);
    CatalogoNavigazione c;
    c.sistema_predefinito = s(t, "sistema_predefinito");
    c.schuler_min = d(t, "schuler_min", c.schuler_min);
    c.latitudine_polare = d(t, "latitudine_polare", c.latitudine_polare);
    c.fattore_polare = d(t, "fattore_polare", c.fattore_polare);
    if (const auto* tab = t["sistemi"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) {
                SistemaNavigazione x;
                x.id = std::string(k.str());
                x.nome = s(*r, "nome");
                x.deriva_nm_giorno = d(*r, "deriva_nm_giorno");
                x.schuler_nm = d(*r, "schuler_nm");
                x.correlazione_h = d(*r, "correlazione_h", x.correlazione_h);
                x.fonte = s(*r, "fonte");
                x.confidenza = s(*r, "confidenza");
                c.sistemi[x.id] = x;
            }
    if (const auto* tab = t["classi"].as_table())
        for (const auto& [k, n] : *tab) c.classi[std::string(k.str())] = n.value_or(std::string{});
    if (const auto* tab = t["teatri"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) c.teatri[std::string(k.str())] = {d(*r, "latitudine"), d(*r, "corrente_kn")};
    if (const auto* tab = t["punti"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) {
                DefPuntoNave p;
                p.id = std::string(k.str());
                p.nome = s(*r, "nome");
                p.marina = s(*r, "marina");
                p.opzione = s(*r, "opzione");
                p.fonte = s(*r, "fonte");
                p.confidenza = s(*r, "confidenza");
                p.sigma_min_nm = d(*r, "sigma_min");
                p.sigma_max_nm = d(*r, "sigma_max");
                p.per_kn = d(*r, "per_kn");
                p.durata_s = d(*r, "durata_s");
                p.quota_max_m = d(*r, "quota_max_m", p.quota_max_m);
                p.velocita_max_kn = d(*r, "velocita_max_kn", p.velocita_max_kn);
                p.albero = b(*r, "albero");
                p.filo = b(*r, "filo");
                p.emissione = b(*r, "emissione");
                p.cielo = b(*r, "cielo");
                p.notte = b(*r, "notte");
                p.costa_km = d(*r, "costa_km");
                if (const auto* a = (*r)["teatri"].as_array())
                    for (const auto& x : *a) p.teatri.push_back(x.value_or(std::string{}));
                p.anno_dal = static_cast<int>(d(*r, "anno_dal"));
                p.attesa_40_min = d(*r, "attesa_40");
                p.attesa_60_min = d(*r, "attesa_60");
                p.attesa_70_min = d(*r, "attesa_70");
                p.scartati = d(*r, "scartati");
                p.corsia_nm = d(*r, "corsia_nm");
                p.allungamento = d(*r, "allungamento", 1.0);
                p.frazione_isobate = d(*r, "frazione_isobate");
                p.carta = b(*r, "carta");
                p.fari_min = static_cast<int>(d(*r, "fari_min", 0.0));
                for (const auto& te : p.teatri)
                    if (!c.teatri.count(te)) throw std::runtime_error("navigazione: teatro sconosciuto " + te + " in " + p.id);
                c.punti[p.id] = std::move(p);
            }
    for (const auto& [classe, sis] : c.classi)
        if (!c.sistemi.count(sis)) throw std::runtime_error("navigazione: sistema sconosciuto " + sis + " per " + classe);
    if (!c.sistemi.count(c.sistema_predefinito)) throw std::runtime_error("navigazione: manca il sistema predefinito");
    if (!c.sistemi.count("stimata") || c.stima().inerziale()) throw std::runtime_error("navigazione: manca la stimata");
    return c;
}

ValutazionePuntoNave valuta_punto_nave(const DefPuntoNave& p, const CondizioniPuntoNave& c) {
    ValutazionePuntoNave v;
    using M = MotivoPuntoNave;
    const bool sovietico = c.marina == "vmf";
    if ((p.marina == "nato" && sovietico) || (p.marina == "vmf" && !sovietico)) v.motivo = M::marina;
    else if (c.anno < p.anno_dal) v.motivo = M::anno;
    else if (!p.opzione.empty() && !(p.opzione == "gps" && c.gps)) v.motivo = M::opzione;
    else if (!p.teatri.empty() && std::find(p.teatri.begin(), p.teatri.end(), c.teatro) == p.teatri.end()) v.motivo = M::copertura;
    else if (c.quota_m > p.quota_max_m) v.motivo = M::quota;
    else if (c.velocita_kn > p.velocita_max_kn) v.motivo = M::velocita;
    else if (p.albero && !c.albero) v.motivo = M::albero;
    else if (p.filo && !c.filo && !c.albero) v.motivo = M::filo;
    else if (p.cielo && !c.sereno) v.motivo = M::cielo;
    else if (p.notte && !c.notte) v.motivo = M::notte;
    else if (p.costa_km > 0.0 && c.distanza_costa_km > p.costa_km) v.motivo = M::costa;
    else if (p.fari_min > 0 && c.fari_in_vista < p.fari_min) v.motivo = M::fari;
    if (v.motivo != M::nessuno) return v;
    v.possibile = true;
    double sigma = p.sigma_min_nm + std::clamp(c.qualita, 0.0, 1.0) * (p.sigma_max_nm - p.sigma_min_nm);
    if (p.frazione_isobate > 0.0 && c.spaziatura_isobate_nm > 0.0)
        sigma = std::max(p.sigma_min_nm, p.frazione_isobate * c.spaziatura_isobate_nm);
    sigma += p.per_kn * std::max(0.0, c.errore_velocita_kn);
    v.sigma_nm = sigma;
    v.sigma_debole_nm = sigma * std::max(1.0, p.allungamento);
    v.direzione_debole_deg = c.direzione_debole_deg;
    v.sistematico_nm = p.carta ? std::max(0.0, c.errore_carta_nm) : 0.0;
    v.corsia_nm = p.corsia_nm;
    v.durata_s = p.durata_s;
    v.emissione = p.emissione;
    v.albero = p.albero;
    return v;
}

double attesa_satellite_min(const DefPuntoNave& p, double latitudine_deg) {
    if (!p.satellite()) return -1.0;
    const double l = std::abs(latitudine_deg);
    if (l <= 40.0) return p.attesa_40_min;
    if (l <= 60.0) return p.attesa_40_min + (p.attesa_60_min - p.attesa_40_min) * (l - 40.0) / 20.0;
    if (l <= 70.0) return p.attesa_60_min + (p.attesa_70_min - p.attesa_60_min) * (l - 60.0) / 10.0;
    return p.attesa_70_min;
}

double prossimo_passaggio_s(const DefPuntoNave& p, double t_s, double latitudine_deg, std::uint64_t seme) {
    const double w = attesa_satellite_min(p, latitudine_deg) * 60.0;
    if (w <= 0.0) return -1.0;
    const std::uint64_t base = seme ^ impronta(p.id);
    for (auto k = static_cast<std::int64_t>(std::floor(std::max(0.0, t_s) / w));; ++k) {
        Casuale r(base ^ (static_cast<std::uint64_t>(k) * 0x9E3779B97F4A7C15ull));
        const double t = (static_cast<double>(k) + r.uniforme()) * w;
        if (t >= t_s && r.uniforme() >= p.scartati) return t;
    }
}

NavigazioneStimata::NavigazioneStimata(const CatalogoNavigazione& cat, const std::string& classe,
                                       const std::string& teatro, std::uint64_t seme, double sigma_iniziale_nm)
    : NavigazioneStimata(cat.sistema_di(classe), cat.stima(), cat.corrente_kn(teatro), seme, sigma_iniziale_nm,
                         cat.schuler_min, cat.latitudine_polare, cat.fattore_polare) {}

NavigazioneStimata::NavigazioneStimata(const SistemaNavigazione& sistema, const SistemaNavigazione& stima,
                                       double corrente_kn, std::uint64_t seme, double sigma_iniziale_nm,
                                       double schuler_min, double latitudine_polare, double fattore_polare)
    : s_(sistema), correlazione_stima_s_(stima.correlazione_h * 3600.0), corrente_ms_(corrente_kn * KN),
      schuler_s_(schuler_min * 60.0), lat_polare_(latitudine_polare), fattore_polare_(fattore_polare), rng_(seme),
      ins_(sistema.inerziale()) {
    const double s0 = sigma_iniziale_nm * NM, sb = sigma_bias(ins_);
    for (std::size_t i = 0; i < 2; ++i) {
        e_[i] = s0 * gauss(rng_);
        b_ins_[i] = sigma_bias(true) * gauss(rng_);
        c_[i] = sigma_bias(false) * gauss(rng_);
        at(p_, E[i], E[i]) = s0 * s0;
        at(p_, B[i], B[i]) = sb * sb;
    }
    schuler_fase_ = 2.0 * PI * rng_.uniforme();
    schuler_dir_ = 360.0 * rng_.uniforme();
    carta_dir_ = 360.0 * rng_.uniforme();
    corsia_dir_ = 360.0 * rng_.uniforme();
}

double NavigazioneStimata::correlazione_s(bool ins) const {
    return ins ? s_.correlazione_h * 3600.0 : correlazione_stima_s_;
}

// L'errore di velocita' per asse. A inerziale: tale che l'errore radiale dopo un giorno sia la deriva del sistema
// (per un Gauss-Markov stazionario la varianza della posizione per asse e' 2 s^2 tau^2 (T/tau - 1 + e^-T/tau)). A
// stima: la corrente del teatro e' la velocita' radiale rms
double NavigazioneStimata::sigma_bias(bool ins) const {
    if (!ins) return corrente_ms_ / std::sqrt(2.0);
    if (!s_.inerziale()) return 0.0;
    const double tau = correlazione_s(true), x = GIORNO_S / tau;
    const double g = 2.0 * tau * tau * (x - 1.0 + std::exp(-x));
    return s_.deriva_nm_giorno * NM / std::sqrt(2.0 * g);
}

void NavigazioneStimata::cambia_modo(bool ins) {
    ins_ = ins;
    const double sb = sigma_bias(ins);
    b_hat_ = {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (i >= 2 || j >= 2) at(p_, i, j) = (i == j) ? sb * sb : 0.0;
}

void NavigazioneStimata::avanza(double dt, double latitudine_deg, bool inerziale_ok) {
    if (dt <= 0.0) return;
    if (const bool ins = s_.inerziale() && inerziale_ok; ins != ins_) cambia_modo(ins);
    fattore_ = (ins_ && std::abs(latitudine_deg) > lat_polare_) ? fattore_polare_ : 1.0;
    const double f = fattore_;
    // il processo attivo, esatto per qualunque dt: b' = -b/tau + w, e' = f (b - b_hat)
    const double tau = correlazione_s(ins_), sb = sigma_bias(ins_);
    const double x = dt / tau, phi = std::exp(-x), a = -tau * std::expm1(-x), u = -tau * std::expm1(-2.0 * x) / 2.0;
    const double q = 2.0 * sb * sb / tau;
    const double qbb = sb * sb * (1.0 - phi * phi);
    const double qeb = x < 1e-4 ? q * f * dt * dt / 2.0 : q * f * tau * (a - u);
    const double qee = x < 1e-4 ? q * f * f * dt * dt * dt / 3.0 : q * f * f * tau * tau * (dt - 2.0 * a + u);
    auto& attivo = ins_ ? b_ins_ : c_;
    auto& altro = ins_ ? c_ : b_ins_;
    const double tau2 = correlazione_s(!ins_), sb2 = sigma_bias(!ins_), phi2 = std::exp(-dt / tau2);
    for (std::size_t i = 0; i < 2; ++i) {
        const double g1 = gauss(rng_), g2 = gauss(rng_);
        const double wb = std::sqrt(qbb) * g1;
        const double l21 = qbb > 0.0 ? qeb / std::sqrt(qbb) : 0.0;
        const double we = l21 * g1 + std::sqrt(std::max(0.0, qee - l21 * l21)) * g2;
        e_[i] += f * a * (attivo[i] - b_hat_[i]) + we;
        attivo[i] = phi * attivo[i] + wb;
        b_hat_[i] *= phi;
        altro[i] = phi2 * altro[i] + sb2 * std::sqrt(1.0 - phi2 * phi2) * gauss(rng_);
    }
    Mat4 F{}, Q{};
    for (int i = 0; i < 2; ++i) {
        at(F, E[i], E[i]) = 1.0;
        at(F, E[i], B[i]) = f * a;
        at(F, B[i], B[i]) = phi;
        at(Q, E[i], E[i]) = qee;
        at(Q, E[i], B[i]) = at(Q, B[i], E[i]) = qeb;
        at(Q, B[i], B[i]) = qbb;
    }
    p_ = per(per(F, p_), trasposta(F));
    for (std::size_t k = 0; k < 16; ++k) p_[k] += Q[k];
    t_ += dt;
}

Vec2 NavigazioneStimata::schuler() const {
    if (!ins_ || s_.schuler_nm <= 0.0) return {};
    const double v = s_.schuler_nm * NM * fattore_ * std::sin(2.0 * PI * t_ / schuler_s_ + schuler_fase_);
    const Vec2 u = versore(schuler_dir_);
    return {v * u.x, v * u.y};
}

Vec2 NavigazioneStimata::errore() const {
    const Vec2 sc = schuler();
    return {e_[0] + sc.x, e_[1] + sc.y};
}

Vec2 NavigazioneStimata::punto_nave(const ValutazionePuntoNave& v) {
    if (!v.possibile) return {};
    const Vec2 u = versore(v.direzione_debole_deg), w{u.y, -u.x};
    const double sa = v.sigma_debole_nm * NM, st = v.sigma_nm * NM;
    // dove cade il fix rispetto alla posizione vera: rumore, errore della carta, corsia
    const double ga = gauss(rng_), gt = gauss(rng_);
    const Vec2 carta = versore(carta_dir_);
    double mx = sa * ga * u.x + st * gt * w.x + v.sistematico_nm * NM * carta.x;
    double my = sa * ga * u.y + st * gt * w.y + v.sistematico_nm * NM * carta.y;
    const Vec2 err = errore();
    if (v.corsia_nm > 0.0) {
        const double l = v.corsia_nm * NM;
        for (const double dir : {corsia_dir_, corsia_dir_ + 90.0}) {
            const Vec2 c = versore(dir);
            const double j = std::round((err.x * c.x + err.y * c.y) / l);
            mx += j * l * c.x;
            my += j * l * c.y;
        }
    }
    const double z[2] = {err.x - mx, err.y - my};   // stimata - fix
    // R, S = P_ee + R, K = P H' S^-1
    const double r00 = sa * sa * u.x * u.x + st * st * w.x * w.x, r11 = sa * sa * u.y * u.y + st * st * w.y * w.y;
    const double r01 = sa * sa * u.x * u.y + st * st * w.x * w.y;
    const double s00 = at(p_, 0, 0) + r00, s11 = at(p_, 1, 1) + r11, s01 = at(p_, 0, 1) + r01;
    const double det = s00 * s11 - s01 * s01;
    if (det <= 0.0) return {};
    const double i00 = s11 / det, i11 = s00 / det, i01 = -s01 / det;
    double k[4][2];
    for (int r = 0; r < 4; ++r) {
        k[r][0] = at(p_, r, 0) * i00 + at(p_, r, 1) * i01;
        k[r][1] = at(p_, r, 0) * i01 + at(p_, r, 1) * i11;
    }
    double xh[4];
    for (int r = 0; r < 4; ++r) xh[r] = k[r][0] * z[0] + k[r][1] * z[1];
    // P = (I - KH) P (I - KH)' + K R K' (Joseph: resta simmetrica e positiva)
    Mat4 A{};
    for (int r = 0; r < 4; ++r) {
        at(A, r, r) = 1.0;
        at(A, r, 0) -= k[r][0];
        at(A, r, 1) -= k[r][1];
    }
    p_ = per(per(A, p_), trasposta(A));
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            at(p_, r, c) += k[r][0] * (r00 * k[c][0] + r01 * k[c][1]) + k[r][1] * (r01 * k[c][0] + r11 * k[c][1]);
    e_[0] -= xh[0];
    e_[1] -= xh[1];
    b_hat_[0] += xh[2];
    b_hat_[1] += xh[3];
    return {-xh[0], -xh[1]};
}

EllisseErrore NavigazioneStimata::ellisse(double k) const {
    double a = at(p_, 0, 0), dd = at(p_, 1, 1), c = at(p_, 0, 1);
    if (ins_ && s_.schuler_nm > 0.0) {   // l'oscillazione di Schuler, che il filtro non segue: varianza A^2/2 sulla sua direzione
        const double amp = s_.schuler_nm * NM * fattore_, v = amp * amp / 2.0;
        const Vec2 u = versore(schuler_dir_);
        a += v * u.x * u.x;
        dd += v * u.y * u.y;
        c += v * u.x * u.y;
    }
    const double m = (a + dd) / 2.0, r = std::sqrt((a - dd) * (a - dd) / 4.0 + c * c);
    EllisseErrore el;
    el.maggiore_m = k * std::sqrt(std::max(0.0, m + r));
    el.minore_m = k * std::sqrt(std::max(0.0, m - r));
    const double th = 0.5 * std::atan2(2.0 * c, a - dd);   // dall'asse x (est) verso y (nord)
    el.orientamento_deg = std::fmod(normalizza_360(90.0 - gradi(th)), 180.0);
    return el;
}

double NavigazioneStimata::sigma_m() const {
    const EllisseErrore el = ellisse();
    return std::hypot(el.maggiore_m, el.minore_m);
}

double NavigazioneStimata::sigma_velocita_kn() const {
    return std::sqrt(std::max(0.0, at(p_, 2, 2) + at(p_, 3, 3))) / KN;
}

}  // namespace m688
