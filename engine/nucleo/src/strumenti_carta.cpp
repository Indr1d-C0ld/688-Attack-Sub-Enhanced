#include "nucleo/strumenti_carta.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace m688 {

Misura misura(Vec2 a, Vec2 b, double velocita_m_s) {
    Misura m;
    m.distanza_m = distanza(a, b);
    m.rilevamento = normalizza_360(rilevamento(a, b));
    m.tempo_s = velocita_m_s > 0.0 ? m.distanza_m / velocita_m_s : 0.0;
    return m;
}

Vec2 vettore(double rotta, double m_s) {
    const double a = radianti(rotta);
    return {m_s * std::sin(a), m_s * std::cos(a)};
}

Cpa cpa(Vec2 p0, Vec2 v0, Vec2 p1, Vec2 v1) {
    // moto relativo dell'altro rispetto al primo: d(t) = dp + dv t, minimo a t = -(dp . dv) / |dv|^2
    const Vec2 dp{p1.x - p0.x, p1.y - p0.y}, dv{v1.x - v0.x, v1.y - v0.y};
    const double vv = dv.x * dv.x + dv.y * dv.y;
    Cpa c;
    c.tempo_s = vv > 1e-12 ? -(dp.x * dv.x + dp.y * dv.y) / vv : 0.0;
    c.proprio = {p0.x + v0.x * c.tempo_s, p0.y + v0.y * c.tempo_s};
    c.altro = {p1.x + v1.x * c.tempo_s, p1.y + v1.y * c.tempo_s};
    c.distanza_m = distanza(c.proprio, c.altro);
    c.rilevamento = normalizza_360(rilevamento(c.proprio, c.altro));
    return c;
}

double distanza_1989(double metri, int scala) {
    const double passo = scala <= 12 ? 0.1 : 0.5;   // la tacca della scala del plotter, in nm
    return std::round(metri / MIGLIO / passo) * passo * MIGLIO;
}

double rilevamento_1989(double gradi) { return normalizza_360(std::round(gradi)); }

std::string testo_misura(const Misura& m, double velocita_kn, bool anni_89, int scala) {
    const double nm = (anni_89 ? distanza_1989(m.distanza_m, scala) : m.distanza_m) / MIGLIO;
    const double ril = anni_89 ? rilevamento_1989(m.rilevamento) : m.rilevamento;
    char b[96];
    if (m.tempo_s > 0.0 && velocita_kn > 0.0)
        std::snprintf(b, sizeof b, "%03.0f°  %.1f NM  %.0f MIN A %.0f KN", ril, nm, std::round(m.tempo_s / 60.0), velocita_kn);
    else std::snprintf(b, sizeof b, anni_89 ? "%03.0f°  %.1f NM" : "%05.1f°  %.2f NM", ril, nm);
    std::string s = b;
    for (auto& ch : s)
        if (ch == '.') ch = ',';   // la virgola dei decimali, come nel resto dell'interfaccia
    return s;
}

bool dentro_poligono(const std::vector<Vec2>& v, Vec2 p) {
    bool dentro = false;
    for (std::size_t i = 0, j = v.size() - 1; i < v.size(); j = i++)
        if ((v[i].y > p.y) != (v[j].y > p.y) && p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x) dentro = !dentro;
    return dentro;
}

double raggio_datum(double eta_s, double velocita_max_m_s) { return std::max(0.0, eta_s) * velocita_max_m_s; }

const char* nome_evento(Evento::Tipo t) {
    switch (t) {
        case Evento::Tipo::segno: return "SEGNO";
        case Evento::Tipo::transitorio: return "TRANSITORIO";
        case Evento::Tipo::avvistamento: return "AVVISTAMENTO";
        case Evento::Tipo::lancio: return "LANCIO";
        case Evento::Tipo::datum: return "DATUM";
        case Evento::Tipo::nota: return "NOTA";
    }
    return "";
}

void LucidoCarta::aggiungi_tratto(Tratto t) {
    tratti_.push_back(std::move(t));
    if (tratti_.size() > static_cast<std::size_t>(MAX_TRATTI)) tratti_.erase(tratti_.begin());
}

void LucidoCarta::aggiungi_linea(LineaRilevamento l) {
    const std::string fonte = l.fonte;
    linee_.push_back(std::move(l));
    const auto n = std::count_if(linee_.begin(), linee_.end(), [&](const LineaRilevamento& x) { return x.fonte == fonte; });
    if (n > MAX_LINEE_PER_FONTE)   // via la piu' vecchia della stessa fonte
        linee_.erase(std::find_if(linee_.begin(), linee_.end(), [&](const LineaRilevamento& x) { return x.fonte == fonte; }));
}

const LineaRilevamento* LucidoCarta::ultima_linea(const std::string& fonte) const {
    for (auto it = linee_.rbegin(); it != linee_.rend(); ++it)
        if (it->fonte == fonte) return &*it;
    return nullptr;
}

int LucidoCarta::evento(Evento::Tipo tipo, Vec2 pos, double tempo, std::string testo) {
    eventi_.push_back({prossimo_evento_, tipo, pos, tempo, std::move(testo)});
    if (eventi_.size() > static_cast<std::size_t>(MAX_EVENTI)) eventi_.erase(eventi_.begin());
    return prossimo_evento_++;
}

void LucidoCarta::aggiungi_datum(Datum d) {
    datum_.push_back(std::move(d));
    if (datum_.size() > static_cast<std::size_t>(MAX_DATUM)) datum_.erase(datum_.begin());
}

void LucidoCarta::pulisci() {
    tratti_.clear();
    linee_.clear();
    eventi_.clear();
    datum_.clear();
    segni_.clear();
    zone_.clear();
}

void LucidoCarta::aggiungi_segno(Segno s) {
    if (s.testo.size() > static_cast<std::size_t>(MAX_TESTO)) s.testo.resize(static_cast<std::size_t>(MAX_TESTO));
    segni_.push_back(std::move(s));
    if (segni_.size() > static_cast<std::size_t>(MAX_SEGNI)) segni_.erase(segni_.begin());
}

void LucidoCarta::aggiungi_zona(ZonaUtente z) {
    zone_.push_back(z);
    if (zone_.size() > static_cast<std::size_t>(MAX_ZONE)) zone_.erase(zone_.begin());
}

std::vector<LineaRilevamento> LucidoCarta::linee_di(const std::string& fonte) const {
    std::vector<LineaRilevamento> v;
    for (const auto& l : linee_)
        if (l.fonte == fonte) v.push_back(l);
    std::stable_sort(v.begin(), v.end(), [](const LineaRilevamento& a, const LineaRilevamento& b) { return a.tempo < b.tempo; });
    return v;
}

const char* nome_affiliazione(Segno::Affiliazione a) {
    switch (a) {
        case Segno::Affiliazione::ostile: return "OSTILE";
        case Segno::Affiliazione::ignoto: return "IGNOTO";
        case Segno::Affiliazione::amico: return "AMICO";
        case Segno::Affiliazione::neutrale: return "NEUTRALE";
    }
    return "";
}

const char* nome_dominio(Segno::Dominio d) {
    switch (d) {
        case Segno::Dominio::subacqueo: return "SUBACQUEO";
        case Segno::Dominio::superficie: return "SUPERFICIE";
        case Segno::Dominio::aereo: return "AEREO";
        case Segno::Dominio::nota: return "NOTA";
        case Segno::Dominio::croce: return "CROCE";
    }
    return "";
}

Ekelund regolo_ekelund(const std::vector<LineaRilevamento>& linee, double distanza_m, double rotta, double velocita_m_s) {
    Ekelund e;
    if (linee.empty()) return e;
    const LineaRilevamento& ultima = linee.back();
    const Vec2 dir = vettore(ultima.rilevamento, 1.0), moto = vettore(rotta, velocita_m_s);
    const Vec2 p0{ultima.origine.x + distanza_m * dir.x, ultima.origine.y + distanza_m * dir.y};
    double somma = 0.0;
    for (const auto& l : linee) {
        const double dt = ultima.tempo - l.tempo;
        const Vec2 p{p0.x - moto.x * dt, p0.y - moto.y * dt};
        e.tacche.push_back(p);
        const double scarto = normalizza_180(rilevamento(l.origine, p) - l.rilevamento);
        somma += scarto * scarto;
    }
    e.scarto_gradi = std::sqrt(somma / static_cast<double>(linee.size()));
    return e;
}

std::optional<RettaRilevamento> retta_rilevamento(const std::vector<LineaRilevamento>& linee, double t) {
    if (linee.size() < 2) return std::nullopt;
    // rilevamenti «srotolati» attorno al primo, per non saltare a 0/360
    const double base = linee.front().rilevamento;
    double st = 0.0, sb = 0.0, stt = 0.0, stb = 0.0;
    const double n = static_cast<double>(linee.size());
    for (const auto& l : linee) {
        const double b = base + normalizza_180(l.rilevamento - base), x = (l.tempo - t) / 60.0;
        st += x;
        sb += b;
        stt += x * x;
        stb += x * b;
    }
    const double den = n * stt - st * st;
    if (std::fabs(den) < 1e-12) return std::nullopt;
    RettaRilevamento r;
    r.rateo_gradi_min = (n * stb - st * sb) / den;
    r.rilevamento = normalizza_360((sb - r.rateo_gradi_min * st) / n);   // il valore all'istante t (x = 0)
    return r;
}

}  // namespace m688
