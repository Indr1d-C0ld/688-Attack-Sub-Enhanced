#include "nucleo/volta_ghiaccio.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>

namespace m688 {

namespace {
constexpr std::int64_t RIGA = 1 << 22;   // la chiave di cella: riga x RIGA + colonna, con l'origine spostata
constexpr std::int64_t SPOSTA = 1 << 21;
constexpr char MAGICO[8] = {'M', '6', '8', '8', 'Q', 'V', 'O', 'L'};
constexpr std::uint32_t VERSIONE = 1;
constexpr double SONDAGGI_RECENTI_S = 60.0;   // il sondatore va a 1 Hz: l'ultimo minuto
constexpr double RITORNI_RECENTI_S = 30.0;    // come per il fondo: l'ultima mezza dozzina di spazzate
constexpr double DISCESA_MIN_M = 5.0;         // la volta che scende di piu' in un minuto (STIMA)
constexpr double SETTORE_PRUA_DEG = 30.0;     // come per il fondo

template <typename T>
void scrivi(std::ostream& o, const T& x) {
    o.write(reinterpret_cast<const char*>(&x), sizeof(T));
}
template <typename T>
T leggi_bin(std::istream& in) {
    T x{};
    in.read(reinterpret_cast<char*>(&x), sizeof(T));
    if (!in) throw std::runtime_error("quadro_volta: salvataggio troncato");
    return x;
}

double gauss(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}
}  // namespace

SondaggioAlto sonda_alto(const CampoGhiaccio& g, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double t,
                         const ParametriSondatoreAlto& p, Casuale& caso) {
    SondaggioAlto s;
    s.t = t;
    s.pos = pos_stimata;
    s.quota_m = quota_m;
    if (quota_m <= 0.0 || quota_m > p.portata_m) return s;
    // il pescaggio piu' profondo nel cono: l'impronta sulla volta e' un cerchio di raggio quota x tan(cono / 2)
    const double raggio = quota_m * std::tan(radianti(0.5 * p.cono_deg));
    const double passo = std::max(10.0, 0.5 * g.parametri().cella_m);
    double pescaggio = 0.0;
    PuntoGhiaccio centro = g.in(pos_vera, t);
    pescaggio = centro.pescaggio_m;
    for (double dx = -raggio; dx <= raggio + 1e-9; dx += passo)
        for (double dy = -raggio; dy <= raggio + 1e-9; dy += passo)
            if (dx * dx + dy * dy <= raggio * raggio) pescaggio = std::max(pescaggio, g.in({pos_vera.x + dx, pos_vera.y + dy}, t).pescaggio_m);
    s.pescaggio_vero_m = pescaggio;
    s.eco = true;
    // il ghiaccio sottile e liscio (senza chiglie nel cono) puo' sembrare acqua
    const bool liscio = centro.ghiaccio && pescaggio < 0.9 * p.liscio_m;
    if (!centro.ghiaccio && pescaggio <= 0.0) {
        s.libera = true;
    } else if (liscio && caso.uniforme() < p.p_scambio) {
        s.libera = true;
        s.scambio = true;
    }
    const double vera = s.libera ? quota_m : quota_m - pescaggio;
    s.sigma_m = p.errore_fisso_m + p.errore_rel * vera;
    s.distanza_m = std::max(0.0, vera + s.sigma_m * gauss(caso));
    s.pescaggio_m = s.libera ? 0.0 : std::max(0.0, quota_m - s.distanza_m);
    return s;
}

double portata_chiglia_m(const ParametriBqs15Fondo& p, bool migliorato) {
    return PORTATA_CHIGLIA_M * p.fattore_portata * (migliorato ? CHIGLIA_688I : 1.0);
}

std::vector<RitornoChiglia> spazza_ghiaccio(const CampoGhiaccio& g, Vec2 pos_vera, Vec2 pos_stimata, double quota_m, double rotta,
                                            double velocita_kn, double t, const ParametriBqs15Fondo& p, Casuale& caso,
                                            double portata_chiglia_m, double margine_m, double pescaggio_min_m) {
    std::vector<RitornoChiglia> out;
    const double portata = portata_effettiva(portata_chiglia_m, velocita_kn, p);
    const double tan_max = std::tan(radianti(p.elevazione_max_deg));
    for (double az = -p.settore_az_deg; az <= p.settore_az_deg + 1e-9; az += p.passo_az_deg) {
        const double b = radianti(rotta + az), sb = std::sin(b), cb = std::cos(b);
        bool primo = false;
        for (double h = p.passo_m; h <= portata; h += p.passo_m) {
            const PuntoGhiaccio x = g.in({pos_vera.x + h * sb, pos_vera.y + h * cb}, t);
            if (!x.chiglia && x.pescaggio_m < pescaggio_min_m) continue;
            const double sopra = quota_m - x.pescaggio_m;   // quanto la chiglia sta sopra il battello
            if (sopra > h * tan_max) continue;               // troppo in alto per il ventaglio, ancora
            if (std::hypot(h, sopra) > portata) break;
            const bool ostacolo = x.pescaggio_m >= quota_m - margine_m;
            if (primo && !ostacolo) continue;                // dopo la prima eco si cerca solo un ostacolo piu' in la'
            RitornoChiglia r;
            r.t = t;
            r.origine = pos_stimata;
            r.rilevamento = normalizza_360(rotta + az + p.errore_angolo_deg * gauss(caso));
            r.distanza_m = std::max(1.0, std::hypot(h, sopra) + p.errore_distanza_m * gauss(caso));
            const double el = std::atan2(sopra, h);
            const double orizz = r.distanza_m * std::cos(el);
            r.punto = {pos_stimata.x + orizz * std::sin(radianti(r.rilevamento)), pos_stimata.y + orizz * std::cos(radianti(r.rilevamento))};
            r.pescaggio_m = std::max(0.0, quota_m - r.distanza_m * std::sin(el));
            r.ostacolo = ostacolo;
            out.push_back(r);
            if (ostacolo) break;   // la prima del raggio e, se non era un ostacolo, il primo ostacolo dietro
            primo = true;
        }
    }
    return out;
}

double forza_bersaglio_chiglia_db(double pescaggio_m) {
    constexpr double TS_10M_DB = 0.0, PER_RADDOPPIO_DB = 6.0, TS_MIN_DB = -15.0, TS_MAX_DB = 10.0;
    if (pescaggio_m <= 0.0) return TS_MIN_DB;
    return std::clamp(TS_10M_DB + PER_RADDOPPIO_DB * std::log2(pescaggio_m / 10.0), TS_MIN_DB, TS_MAX_DB);
}

std::vector<EcoChiglia> echi_chiglie(const CampoGhiaccio& g, Vec2 pos, double quota_m, double rotta, double settore_deg,
                                     double portata_m, double elevazione_deg, double t, double passo_m, double pescaggio_min_m) {
    constexpr double PASSO_AZ_DEG = 5.0;
    std::vector<EcoChiglia> out;
    const double tan_el = std::tan(radianti(std::clamp(elevazione_deg, 0.0, 89.0)));
    const double mezzo = 0.5 * std::clamp(settore_deg, 0.0, 360.0);
    for (double az = -mezzo; az <= mezzo + 1e-9; az += PASSO_AZ_DEG) {
        const double b = radianti(rotta + az), sb = std::sin(b), cb = std::cos(b);
        for (double h = passo_m; h <= portata_m; h += passo_m) {
            const Vec2 p{pos.x + h * sb, pos.y + h * cb};
            const PuntoGhiaccio x = g.in(p, t);
            if (!x.chiglia || x.pescaggio_m < pescaggio_min_m) continue;
            const double sopra = quota_m - x.pescaggio_m;   // quanto la chiglia sta sopra l'attivo (negativa: sotto)
            if (std::fabs(sopra) > h * tan_el) continue;   // fuori dal ventaglio d'elevazione
            const double d = std::hypot(h, sopra);
            if (d > portata_m) break;
            out.push_back({p, x.pescaggio_m, forza_bersaglio_chiglia_db(x.pescaggio_m), d});
            break;   // la piu' vicina del raggio copre quelle dietro
        }
    }
    return out;
}

void QuadroVolta::aggiungi(const RitornoChiglia& r) {
    dimentica(r.t);
    ritorni_.push_back(r);
    CellaVolta& c = celle_[chiave(r.punto)];
    ++c.misure;
    c.ultima_t = r.t;
    c.pescaggio_max_m = std::max(c.pescaggio_max_m, r.pescaggio_m);
}

std::int64_t QuadroVolta::chiave(Vec2 p) const {
    const auto c = static_cast<std::int64_t>(std::floor(p.x / cella_)) + SPOSTA, r = static_cast<std::int64_t>(std::floor(p.y / cella_)) + SPOSTA;
    return r * RIGA + c;
}

Vec2 QuadroVolta::centro(std::int64_t k) const {
    const std::int64_t r = k / RIGA - SPOSTA, c = k % RIGA - SPOSTA;
    return {(static_cast<double>(c) + 0.5) * cella_, (static_cast<double>(r) + 0.5) * cella_};
}

void QuadroVolta::aggiungi_spazzata(double t, double portata_m, const std::vector<RitornoChiglia>& ritorni) {
    spazzata_t_ = t;
    spazzata_portata_m_ = portata_m;
    for (const RitornoChiglia& r : ritorni) aggiungi(r);
    dimentica(t);
}

void QuadroVolta::dimentica(double t) {
    std::erase_if(sondaggi_, [t](const SondaggioAlto& s) { return t - s.t > STORIA_S; });
    std::erase_if(ritorni_, [t](const RitornoChiglia& r) { return t - r.t > RITORNI_ARCHIVIO_S; });
}

std::vector<RitornoChiglia> QuadroVolta::ritorni_recenti(double t, double finestra_s) const {
    std::vector<RitornoChiglia> out;
    for (const RitornoChiglia& r : ritorni_)
        if (r.t <= t && t - r.t <= finestra_s) out.push_back(r);
    return out;
}

void QuadroVolta::aggiungi(const SondaggioAlto& s) {
    if (!s.eco) return;
    dimentica(s.t);
    sondaggi_.push_back(s);
    CellaVolta& c = celle_[chiave(s.pos)];
    ++c.misure;
    c.ultima_t = s.t;
    if (s.libera) {
        ++c.libere;
        c.ultima_libera_t = s.t;
    } else {
        c.pescaggio_max_m = std::max(c.pescaggio_max_m, s.pescaggio_m);
    }
}

std::optional<CellaVolta> QuadroVolta::leggi(Vec2 p) const {
    const auto it = celle_.find(chiave(p));
    if (it == celle_.end()) return std::nullopt;
    return it->second;
}

std::vector<QuadroVolta::Candidata> QuadroVolta::polynye(Vec2 p, double raggio_m, double t, double eta_max_s) const {
    std::vector<Candidata> out;
    for (const auto& [k, c] : celle_) {
        if (c.libere == 0 || c.ultima_libera_t < 0.0 || t - c.ultima_libera_t > eta_max_s) continue;
        const Vec2 q = centro(k);
        if (distanza(p, q) > raggio_m) continue;
        out.push_back({q, t - c.ultima_libera_t, c.libere, c.misure});
    }
    std::sort(out.begin(), out.end(), [](const Candidata& a, const Candidata& b) { return a.eta_s < b.eta_s; });
    return out;
}

double QuadroVolta::pescaggio_max(Vec2 a, Vec2 b) const {
    const double d = distanza(a, b);
    const int n = std::max(1, static_cast<int>(std::ceil(d / (0.5 * cella_))));
    double m = 0.0;
    for (int k = 0; k <= n; ++k) {
        const double f = static_cast<double>(k) / n;
        if (const auto c = leggi({a.x + f * (b.x - a.x), a.y + f * (b.y - a.y)})) m = std::max(m, c->pescaggio_max_m);
    }
    return m;
}

std::vector<AvvisoVolta> QuadroVolta::avvisi(Vec2 pos_stimata, double quota_m, double rotta, double velocita_kn, double t,
                                             double reazione_s, double margine_m) const {
    std::vector<AvvisoVolta> out;
    // il sondatore: la volta bassa dall'ultimo, la volta che scende dal primo dell'ultimo minuto all'ultimo
    const SondaggioAlto* ultimo = nullptr;
    const SondaggioAlto* primo = nullptr;
    for (const SondaggioAlto& s : sondaggi_)
        if (s.t <= t && t - s.t <= SONDAGGI_RECENTI_S) {
            if (!primo) primo = &s;
            ultimo = &s;
        }
    if (ultimo && !ultimo->libera) {   // la distanza dalla volta alla quota di adesso
        const double sopra = ultimo->distanza_m + (quota_m - ultimo->quota_m);
        if (sopra < margine_m) out.push_back({AvvisoVolta::Tipo::volta_bassa, sopra, ultimo->t});
    }
    if (ultimo && primo && ultimo != primo && !ultimo->libera && !primo->libera) {
        const double discesa = ultimo->pescaggio_m - primo->pescaggio_m;
        if (discesa > DISCESA_MIN_M) out.push_back({AvvisoVolta::Tipo::volta_in_discesa, discesa, ultimo->t});
    }
    // il BQS-15: la chiglia a prua entro il tempo di reazione; la velocita' sicura dalla portata dell'ultima spazzata
    const double v_m_s = velocita_kn * NODO;
    double ostacolo = std::numeric_limits<double>::infinity(), t_ostacolo = 0.0;
    for (const RitornoChiglia& r : ritorni_) {
        if (r.t > t || t - r.t > RITORNI_RECENTI_S || !r.ostacolo) continue;
        if (std::fabs(normalizza_180(r.rilevamento - rotta)) > SETTORE_PRUA_DEG) continue;
        const double d = distanza(pos_stimata, r.punto);
        if (d < ostacolo) ostacolo = d, t_ostacolo = r.t;
    }
    if (std::isfinite(ostacolo) && ostacolo <= std::max(v_m_s, 1.0) * reazione_s)
        out.push_back({AvvisoVolta::Tipo::chiglia_a_prua, ostacolo, t_ostacolo});
    if (spazzata_t_ >= 0.0 && spazzata_t_ <= t && t - spazzata_t_ <= RITORNI_RECENTI_S) {
        const double vista = std::min(spazzata_portata_m_, std::isfinite(ostacolo) ? ostacolo : spazzata_portata_m_);
        const double sicura_kn = vista / reazione_s / NODO;
        if (velocita_kn > sicura_kn) out.push_back({AvvisoVolta::Tipo::velocita_eccessiva, sicura_kn, t});
    }
    return out;
}

void QuadroVolta::salva(std::ostream& out) const {
    out.write(MAGICO, sizeof(MAGICO));
    scrivi(out, VERSIONE);
    scrivi(out, cella_);
    scrivi(out, spazzata_t_);
    scrivi(out, spazzata_portata_m_);
    scrivi(out, static_cast<std::uint64_t>(celle_.size()));
    for (const auto& [k, c] : celle_) {
        scrivi(out, k);
        for (double v : {c.pescaggio_max_m, c.ultima_t, c.ultima_libera_t}) scrivi(out, v);
        scrivi(out, static_cast<std::int32_t>(c.misure));
        scrivi(out, static_cast<std::int32_t>(c.libere));
    }
    scrivi(out, static_cast<std::uint64_t>(sondaggi_.size()));
    for (const SondaggioAlto& s : sondaggi_) {
        for (double v : {s.t, s.pos.x, s.pos.y, s.quota_m, s.distanza_m, s.pescaggio_m, s.sigma_m, s.pescaggio_vero_m}) scrivi(out, v);
        scrivi(out, static_cast<std::uint8_t>((s.eco ? 1 : 0) | (s.libera ? 2 : 0) | (s.scambio ? 4 : 0)));
    }
    scrivi(out, static_cast<std::uint64_t>(ritorni_.size()));
    for (const RitornoChiglia& r : ritorni_) {
        for (double v : {r.t, r.origine.x, r.origine.y, r.rilevamento, r.distanza_m, r.punto.x, r.punto.y, r.pescaggio_m}) scrivi(out, v);
        scrivi(out, static_cast<std::uint8_t>(r.ostacolo ? 1 : 0));
    }
}

QuadroVolta QuadroVolta::carica(std::istream& in) {
    char m[sizeof(MAGICO)];
    in.read(m, sizeof(m));
    if (!in || std::memcmp(m, MAGICO, sizeof(m)) != 0) throw std::runtime_error("quadro_volta: non e' un quadro della volta");
    if (leggi_bin<std::uint32_t>(in) != VERSIONE) throw std::runtime_error("quadro_volta: versione sconosciuta");
    const double cella = leggi_bin<double>(in);
    if (!(cella > 0.0)) throw std::runtime_error("quadro_volta: cella non valida");
    QuadroVolta q(cella);
    q.spazzata_t_ = leggi_bin<double>(in);
    q.spazzata_portata_m_ = leggi_bin<double>(in);
    for (auto n = leggi_bin<std::uint64_t>(in); n > 0; --n) {
        const auto k = leggi_bin<std::int64_t>(in);
        CellaVolta c;
        c.pescaggio_max_m = leggi_bin<double>(in);
        c.ultima_t = leggi_bin<double>(in);
        c.ultima_libera_t = leggi_bin<double>(in);
        c.misure = leggi_bin<std::int32_t>(in);
        c.libere = leggi_bin<std::int32_t>(in);
        q.celle_[k] = c;
    }
    for (auto n = leggi_bin<std::uint64_t>(in); n > 0; --n) {
        SondaggioAlto s;
        for (double* v : {&s.t, &s.pos.x, &s.pos.y, &s.quota_m, &s.distanza_m, &s.pescaggio_m, &s.sigma_m, &s.pescaggio_vero_m})
            *v = leggi_bin<double>(in);
        const auto f = leggi_bin<std::uint8_t>(in);
        s.eco = f & 1;
        s.libera = f & 2;
        s.scambio = f & 4;
        q.sondaggi_.push_back(s);
    }
    for (auto n = leggi_bin<std::uint64_t>(in); n > 0; --n) {
        RitornoChiglia r;
        for (double* v : {&r.t, &r.origine.x, &r.origine.y, &r.rilevamento, &r.distanza_m, &r.punto.x, &r.punto.y, &r.pescaggio_m})
            *v = leggi_bin<double>(in);
        r.ostacolo = leggi_bin<std::uint8_t>(in) & 1;
        q.ritorni_.push_back(r);
    }
    return q;
}

}  // namespace m688
