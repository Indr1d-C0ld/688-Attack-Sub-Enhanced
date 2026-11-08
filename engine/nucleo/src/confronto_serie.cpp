#include "nucleo/confronto_serie.hpp"

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

double mediana(std::vector<double> v) {
    if (v.empty()) return 0.0;
    const std::size_t m = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + static_cast<long>(m), v.end());
    if (v.size() % 2) return v[m];
    const double alto = v[m];
    std::nth_element(v.begin(), v.begin() + static_cast<long>(m) - 1, v.end());
    return 0.5 * (v[m - 1] + alto);
}

std::uint64_t fnv1a(const std::string& s) {   // un'impronta stabile fra piattaforme (std::hash non lo e')
    std::uint64_t h = 0xcbf29ce484222325ULL;
    for (const unsigned char c : s) h = (h ^ c) * 0x100000001b3ULL;
    return h;
}

double quantile(std::vector<double> v, double q) {   // per interpolazione lineare, come riassunto_partite
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    const double x = q * static_cast<double>(v.size() - 1);
    const auto i = static_cast<std::size_t>(std::floor(x));
    const std::size_t j = std::min(i + 1, v.size() - 1);
    return v[i] + (x - static_cast<double>(i)) * (v[j] - v[i]);
}

}  // namespace

IntervalloWilson wilson(int k, int n, double z) {
    IntervalloWilson w;
    w.k = k;
    w.n = n;
    if (n <= 0) return w;
    const double nn = n, p = k / nn, z2 = z * z;
    const double centro = (p + z2 / (2.0 * nn)) / (1.0 + z2 / nn);
    const double mezzo = z * std::sqrt(p * (1.0 - p) / nn + z2 / (4.0 * nn * nn)) / (1.0 + z2 / nn);
    w.p = p;
    w.basso = std::max(0.0, centro - mezzo);
    w.alto = std::min(1.0, centro + mezzo);
    return w;
}

ConfrontoEsito confronta_esito(const std::vector<bool>& a, const std::vector<bool>& b, double z) {
    ConfrontoEsito r;
    const std::size_t n = std::min(a.size(), b.size());
    for (std::size_t i = 0; i < n; ++i) {
        if (a[i] && b[i]) ++r.entrambe;
        else if (a[i]) ++r.solo_a;
        else if (b[i]) ++r.solo_b;
        else ++r.nessuna;
    }
    const int nn = static_cast<int>(n);
    r.a = wilson(r.entrambe + r.solo_a, nn, z);
    r.b = wilson(r.entrambe + r.solo_b, nn, z);
    if (nn == 0) return r;
    // Newcombe 1998, metodo 10, su theta = pA - pB; poi il segno per B - A
    const double A = r.entrambe, B = r.solo_a, C = r.solo_b, D = r.nessuna;
    const double margini = (A + B) * (C + D) * (A + C) * (B + D);
    double phi = 0.0;
    if (margini > 0.0) {
        double num = A * D - B * C;
        if (num > 0.0) num = std::max(num - nn / 2.0, 0.0);   // la correzione di continuita' di Newcombe
        phi = num / std::sqrt(margini);
    }
    const double p1 = r.a.p, l1 = r.a.basso, u1 = r.a.alto, p2 = r.b.p, l2 = r.b.basso, u2 = r.b.alto;
    const double theta = p1 - p2;
    const double delta = std::sqrt(std::max(0.0, (p1 - l1) * (p1 - l1) - 2.0 * phi * (p1 - l1) * (u2 - p2) + (u2 - p2) * (u2 - p2)));
    const double eps = std::sqrt(std::max(0.0, (u1 - p1) * (u1 - p1) - 2.0 * phi * (u1 - p1) * (p2 - l2) + (p2 - l2) * (p2 - l2)));
    r.differenza = -theta;
    r.basso = std::max(-1.0, -(theta + eps));
    r.alto = std::min(1.0, -(theta - delta));
    return r;
}

ConfrontoNumero confronta_numero(const std::vector<double>& a, const std::vector<double>& b, const ParametriBootstrap& par) {
    ConfrontoNumero r;
    std::vector<double> va, vb;
    for (std::size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        if (a[i] < 0.0 || b[i] < 0.0) continue;   // un mancante toglie la coppia
        va.push_back(a[i]);
        vb.push_back(b[i]);
        if (b[i] > a[i] + 1e-9) ++r.sale;
        else if (b[i] < a[i] - 1e-9) ++r.scende;
        else ++r.uguale;
    }
    r.coppie = static_cast<int>(va.size());
    if (va.empty()) return r;
    r.mediana_a = mediana(va);
    r.mediana_b = mediana(vb);
    r.differenza = r.mediana_b - r.mediana_a;
    if (va.size() < 2) {
        r.basso = r.alto = r.differenza;
        return r;
    }
    // il bootstrap a percentili ricampionando le coppie (si tengono insieme A e B dello stesso seme)
    Casuale caso(par.seme);
    std::vector<double> diff;
    diff.reserve(static_cast<std::size_t>(std::max(1, par.ricampionamenti)));
    std::vector<double> ra(va.size()), rb(va.size());
    for (int k = 0; k < std::max(1, par.ricampionamenti); ++k) {
        for (std::size_t i = 0; i < va.size(); ++i) {
            const auto j = std::min(va.size() - 1, static_cast<std::size_t>(caso.uniforme() * static_cast<double>(va.size())));
            ra[i] = va[j];
            rb[i] = vb[j];
        }
        diff.push_back(mediana(rb) - mediana(ra));
    }
    const double coda = (1.0 - par.livello) / 2.0;
    r.basso = quantile(diff, coda);
    r.alto = quantile(diff, 1.0 - coda);
    return r;
}

// quanto dista t dai campioni di un'unita': 0 se c'e' il campione a t, altrimenti l'intervallo in cui il valore e'
// interpolato (o tenuto, oltre l'ultimo campione: quanto e' passato da quello)
static double intervallo_campioni(const std::vector<std::pair<double, StatoUnita>>& serie, double t) {
    const auto dopo = std::lower_bound(serie.begin(), serie.end(), t, [](const auto& c, double x) { return c.first < x; });
    if (dopo != serie.end() && std::fabs(dopo->first - t) < 1e-6) return 0.0;
    if (dopo == serie.begin()) return dopo == serie.end() ? 0.0 : dopo->first - t;
    if (dopo == serie.end()) return t - std::prev(dopo)->first;
    return dopo->first - std::prev(dopo)->first;
}

Divergenza punto_divergenza(const Traccia& a, const Traccia& b, const SoglieDivergenza& s) {
    Divergenza d;
    for (const RecordIntervento& x : b.interventi)
        if (x.fase == RecordIntervento::FASE_APPLICATO) {
            d.t_intervento_s = x.t;
            break;
        }
    // la verita': agli istanti dei campioni di entrambe, in ordine, il primo in cui un'unita' e' diversa. Un campione in piu'
    // non e' una divergenza, e sposta la cadenza di chi lo ha (prossimo_ = f.t + PASSO_S), quindi dopo non restano istanti in
    // comune: dove un valore e' interpolato vale la soglia piu' quanto la vera puo' scostarsi dalla corda in quell'intervallo
    // (STIMA, trailer-1 e disassemblatore, 04/11/2026). Rotta e stato si confrontano solo fra campioni veri. Un'unita' che
    // manca non diverge se da quest'altra parte compare fra poco, o se dove c'e' esce fra poco: e' lo stesso fatto in due
    // fotogrammi diversi. Fra campioni diversi t_s puo' arrivare qualche secondo dopo
    constexpr double QUOTA_M_S = 5.0, VELOCITA_KN_S = 5.0, PRESENZA_S = 10.0;
    const auto stessa_presenza = [&](const Traccia& dove_c_e, const Traccia& dove_manca, int id, double t) {
        const auto it = dove_manca.unita.find(id);
        if (it != dove_manca.unita.end() && !it->second.empty() && it->second.front().first > t && it->second.front().first <= t + PRESENZA_S) return true;
        const auto& serie = dove_c_e.unita.at(id);
        return std::any_of(serie.begin(), serie.end(), [&](const auto& c) { return c.second.rimossa && c.first > t && c.first <= t + PRESENZA_S; });
    };
    std::set<double> tempi;
    for (const Traccia* t : {&a, &b})
        for (const auto& [id, serie] : t->unita)
            for (const auto& [tempo, u] : serie) tempi.insert(tempo);
    for (const double t : tempi) {
        if (d.divergono) break;
        const IstanteTraccia ia = a.istante(t), ib = b.istante(t);
        std::map<int, const StatoUnita*> ma, mb;
        for (const auto& u : ia.unita) ma[u.id] = &u;
        for (const auto& u : ib.unita) mb[u.id] = &u;
        const auto segna = [&](int id, const char* come) {
            d.divergono = true;
            d.t_s = t;
            d.unita = id;
            d.come = come;
        };
        for (const auto& [id, u] : ma) {
            const auto it = mb.find(id);
            if (it == mb.end()) {
                if (!stessa_presenza(a, b, id, t)) { segna(id, "manca"); break; }
                continue;
            }
            const StatoUnita& v = *it->second;
            const double dt = std::max(intervallo_campioni(a.unita.at(id), t), intervallo_campioni(b.unita.at(id), t));
            const double corda_m = std::max(u->velocita_kn, v.velocita_kn) * NODO * dt;
            if (distanza(u->pos, v.pos) > s.posizione_m + corda_m) { segna(id, "posizione"); break; }
            if (dt == 0.0 && std::fabs(normalizza_180(u->rotta - v.rotta)) > s.rotta_gradi) { segna(id, "rotta"); break; }
            if (std::fabs(u->quota - v.quota) > s.quota_m + QUOTA_M_S * dt) { segna(id, "quota"); break; }
            if (std::fabs(u->velocita_kn - v.velocita_kn) > s.velocita_kn + VELOCITA_KN_S * dt) { segna(id, "velocita'"); break; }
            if (dt == 0.0 && (u->distrutta != v.distrutta || u->persa != v.persa || u->incagliata != v.incagliata || u->sul_fondo != v.sul_fondo)) {
                segna(id, "stato");
                break;
            }
        }
        if (!d.divergono)
            for (const auto& [id, v] : mb)
                if (!ma.count(id) && !stessa_presenza(b, a, id, t)) { segna(id, "compare"); break; }
    }
    if (d.divergono && d.t_intervento_s >= 0.0) d.dopo_intervento_s = d.t_s - d.t_intervento_s;
    // il registro: il primo evento che cambia (tipo, unita', altra, codice o tempo). Gli interventi del vetrino di Dio
    // si saltano: sono dello spettatore, non fatti del mondo, e in B sarebbero sempre il primo evento diverso (trailer-1)
    const auto fatti = [](const Traccia& t) {
        std::vector<const EventoTraccia*> v;
        for (const EventoTraccia& e : t.registro)
            if (e.tipo != "intervento") v.push_back(&e);
        return v;
    };
    const std::vector<const EventoTraccia*> fa = fatti(a), fb = fatti(b);
    const std::size_t n = std::max(fa.size(), fb.size());
    for (std::size_t i = 0; i < n; ++i) {
        const EventoTraccia* ea = i < fa.size() ? fa[i] : nullptr;
        const EventoTraccia* eb = i < fb.size() ? fb[i] : nullptr;
        const bool uguali = ea && eb && ea->tipo == eb->tipo && ea->unita == eb->unita && ea->altra == eb->altra &&
                            ea->codice == eb->codice && std::fabs(ea->tempo - eb->tempo) <= s.tempo_evento_s;
        if (uguali) continue;
        d.indice_evento = static_cast<int>(i);
        if (ea) d.evento_a = *ea;
        if (eb) d.evento_b = *eb;
        if (!d.divergono) {   // la verita' campionata non l'ha visto: il registro dice quando
            d.divergono = true;
            d.t_s = std::min(ea ? ea->tempo : 1e18, eb ? eb->tempo : 1e18);
            d.unita = ea ? ea->unita : eb->unita;
            d.come = "evento";
            if (d.t_intervento_s >= 0.0) d.dopo_intervento_s = d.t_s - d.t_intervento_s;
        }
        break;
    }
    return d;
}

ConfrontoSerie confronta_serie(const std::vector<StatisticheSandbox>& a, const std::vector<StatisticheSandbox>& b,
                               const ParametriBootstrap& par) {
    ConfrontoSerie r;
    const std::size_t n = std::min(a.size(), b.size());
    r.coppie = static_cast<int>(n);
    std::set<int> codici, parti;
    for (std::size_t i = 0; i < n; ++i) {
        codici.insert(a[i].esito);
        codici.insert(b[i].esito);
        for (const auto& [p, x] : a[i].parti) parti.insert(p);
        for (const auto& [p, x] : b[i].parti) parti.insert(p);
    }
    for (const int c : codici) {
        std::vector<bool> va(n), vb(n);
        for (std::size_t i = 0; i < n; ++i) {
            va[i] = a[i].esito == c;
            vb[i] = b[i].esito == c;
        }
        r.esiti[c] = confronta_esito(va, vb);
    }
    for (const int p : parti) {
        std::vector<std::map<std::string, double>> na(n), nb(n);
        std::set<std::string> nomi;
        for (std::size_t i = 0; i < n; ++i) {
            if (const auto it = a[i].parti.find(p); it != a[i].parti.end()) na[i] = numeri_parte(it->second);
            if (const auto it = b[i].parti.find(p); it != b[i].parti.end()) nb[i] = numeri_parte(it->second);
            for (const auto& [k, v] : na[i]) nomi.insert(k);
            for (const auto& [k, v] : nb[i]) nomi.insert(k);
        }
        for (const auto& nome : nomi) {
            std::vector<double> va(n, -1.0), vb(n, -1.0);
            for (std::size_t i = 0; i < n; ++i) {
                if (const auto it = na[i].find(nome); it != na[i].end()) va[i] = it->second;
                if (const auto it = nb[i].find(nome); it != nb[i].end()) vb[i] = it->second;
            }
            ParametriBootstrap q = par;
            q.seme ^= fnv1a(nome) + static_cast<std::uint64_t>(p) * 0x9E3779B97F4A7C15ULL;
            r.parti[p][nome] = confronta_numero(va, vb, q);
        }
    }
    std::vector<double> da(n), db(n);
    for (std::size_t i = 0; i < n; ++i) {
        da[i] = a[i].durata_s;
        db[i] = b[i].durata_s;
    }
    r.generali["durata_s"] = confronta_numero(da, db, par);
    return r;
}

}  // namespace m688
