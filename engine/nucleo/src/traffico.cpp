#include "nucleo/traffico.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

namespace {

constexpr double FASCIA_S = 600.0;           // gli arrivi sulle corsie, a fasce di 10 minuti
constexpr double GIORNO_S = 86400.0;
constexpr double RACCORDO_M = 3000.0;        // lo scarto laterale gira ai vertici della spezzata su 3 km
constexpr double VICINI_PESCA_M = 15000.0;   // un rimorchio va da un punto della zona a uno entro 15 km
constexpr double RIENTRO_S = 600.0;          // dopo la fuga la nave torna in rotta in 10 minuti
constexpr double SCAVALCA_M = 10000.0;       // una nave fuori dal tetto entra se e' piu' vicina di 10 km della piu' lontana
constexpr int SCAMBI_PER_PASSO = 2;

std::uint64_t mescola(std::uint64_t z) {
    z += 0x9e3779b97f4a7c15ull;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}
std::uint64_t chiave(std::uint64_t a, std::uint64_t b, std::uint64_t c = 0, std::uint64_t d = 0, std::uint64_t e = 0) {
    return mescola(mescola(mescola(mescola(mescola(a) ^ b) ^ c) ^ d) ^ e);
}
double u01(std::uint64_t z) { return static_cast<double>(mescola(z) >> 11) * 0x1.0p-53; }
double normale(std::uint64_t z) {
    const double u1 = std::max(u01(z), 1e-300), u2 = u01(z ^ 0x5bd1e995ull);
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}
int poisson(double lambda, double u) {   // inversa della ripartizione: lambda piccolo (al piu' qualche decina)
    double p = std::exp(-lambda), somma = p;
    int k = 0;
    while (u > somma && k < 1000) {
        ++k;
        p *= lambda / k;
        somma += p;
    }
    return k;
}
const std::string& dalla_miscela(const std::vector<std::pair<std::string, double>>& m, double u) {
    double totale = 0.0;
    for (const auto& [t, w] : m) totale += w;
    double x = u * totale;
    for (const auto& [t, w] : m)
        if ((x -= w) <= 0.0) return t;
    return m.back().first;
}
bool nel_mese(const std::vector<int>& mesi, int mese) { return mesi.empty() || std::find(mesi.begin(), mesi.end(), mese) != mesi.end(); }

Vec2 piu(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
Vec2 meno(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
Vec2 per(Vec2 a, double k) { return {a.x * k, a.y * k}; }
Vec2 unitario(Vec2 a) {
    const double l = std::hypot(a.x, a.y);
    return l > 0.0 ? Vec2{a.x / l, a.y / l} : Vec2{0.0, 1.0};
}

// Il punto a distanza s lungo la spezzata, spostato di `scarto` a destra del verso di percorrenza (con la normale che
// gira dolcemente ai vertici). `inverso`: la si percorre dall'ultimo punto
Vec2 sulla_spezzata(const std::vector<Vec2>& p, const std::vector<double>& cum, double s, bool inverso, double scarto) {
    const double totale = cum.back();
    double x = std::clamp(inverso ? totale - s : s, 0.0, totale);
    std::size_t i = static_cast<std::size_t>(std::upper_bound(cum.begin(), cum.end(), x) - cum.begin());
    i = std::clamp<std::size_t>(i, 1, p.size() - 1);
    const double l = cum[i] - cum[i - 1];
    const double t = l > 0.0 ? (x - cum[i - 1]) / l : 0.0;
    const Vec2 base = piu(p[i - 1], per(meno(p[i], p[i - 1]), t));
    // normale a destra del verso diretto: (dy, -dx)
    const auto normale_di = [&](std::size_t k) {
        const Vec2 d = unitario(meno(p[k], p[k - 1]));
        return Vec2{d.y, -d.x};
    };
    Vec2 n = normale_di(i);
    const double dentro = x - cum[i - 1], fuori = cum[i] - x;
    if (i > 1 && dentro < RACCORDO_M) {
        const Vec2 media = unitario(piu(normale_di(i - 1), n));
        n = unitario(piu(per(media, 1.0 - dentro / RACCORDO_M), per(n, dentro / RACCORDO_M)));
    } else if (i + 1 < p.size() && fuori < RACCORDO_M) {
        const Vec2 media = unitario(piu(normale_di(i + 1), n));
        n = unitario(piu(per(media, 1.0 - fuori / RACCORDO_M), per(n, fuori / RACCORDO_M)));
    }
    return piu(base, per(n, inverso ? -scarto : scarto));
}

std::vector<std::pair<std::string, double>> miscela_da(const toml::table& tab, const std::string& chi, const std::string& percorso) {
    std::vector<std::pair<std::string, double>> out;
    if (const auto* m = tab["miscela"].as_table())
        for (const auto& [k, v] : *m) out.emplace_back(std::string(k.str()), v.value_or(0.0));
    if (out.empty()) throw std::runtime_error(percorso + ": '" + chi + "' senza miscela");
    return out;
}
std::vector<Vec2> punti_da(const toml::table& tab, const char* campo) {
    std::vector<Vec2> out;
    if (const auto* a = tab[campo].as_array())
        for (const auto& x : *a)
            if (const auto* q = x.as_array(); q && q->size() == 2) out.push_back({(*q)[0].value_or(0.0), (*q)[1].value_or(0.0)});
    return out;
}
std::vector<int> mesi_da(const toml::table& tab) {
    std::vector<int> out;
    if (const auto* a = tab["mesi"].as_array())
        for (const auto& x : *a) out.push_back(static_cast<int>(x.value_or(0)));
    return out;
}

}  // namespace

RotteTraffico carica_traffico(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    RotteTraffico out;
    out.teatro = t["teatro"].value_or(std::string{});
    if (const auto* corsie = t["corsie"].as_table())
        for (const auto& [k, v] : *corsie) {
            const auto* tab = v.as_table();
            if (!tab) continue;
            CorsiaTraffico c;
            c.id = std::string(k.str());
            c.nome = (*tab)["nome"].value_or(std::string{});
            c.punti = punti_da(*tab, "punti");
            if (c.punti.size() < 2) throw std::runtime_error(percorso + ": corsia '" + c.id + "' con meno di due punti");
            c.larghezza_m = (*tab)["larghezza_m"].value_or(2000.0);
            c.doppio_senso = (*tab)["doppio_senso"].value_or(true);
            c.navi_giorno = (*tab)["navi_giorno"].value_or(0.0);
            c.miscela = miscela_da(*tab, c.id, percorso);
            if (const auto* a = (*tab)["partenze_h"].as_array())
                for (const auto& x : *a) c.partenze_h.push_back(x.value_or(0.0));
            c.mesi = mesi_da(*tab);
            out.corsie.push_back(std::move(c));
        }
    if (const auto* zone = t["zone"].as_table())
        for (const auto& [k, v] : *zone) {
            const auto* tab = v.as_table();
            if (!tab) continue;
            ZonaPesca z;
            z.id = std::string(k.str());
            z.nome = (*tab)["nome"].value_or(std::string{});
            z.punti = punti_da(*tab, "punti");
            if (z.punti.empty()) throw std::runtime_error(percorso + ": zona '" + z.id + "' senza punti");
            z.navi = (*tab)["navi"].value_or(0.0);
            z.velocita_kn = (*tab)["velocita_kn"].value_or(3.5);
            z.miscela = miscela_da(*tab, z.id, percorso);
            z.mesi = mesi_da(*tab);
            out.zone.push_back(std::move(z));
        }
    return out;
}

std::string file_traffico(const std::string& cartella_dati, const std::string& teatro) {
    return cartella_dati + "/teatri/" + teatro + ".traffico.toml";
}

Traffico::Traffico(const RotteTraffico& rotte, const CatalogoAcustico& catalogo, int mese, std::uint64_t seme_partita,
                   double raggio_m, double margine_m, int tetto)
    : rotte_(rotte), catalogo_(catalogo), mese_(mese), seme_(mescola(seme_partita ^ 0x7AFF1C0ull)), raggio_m_(raggio_m),
      margine_m_(std::max(margine_m, raggio_m)), tetto_(tetto) {
    for (const CorsiaTraffico& c : rotte_.corsie) {
        Geometria g;
        g.cumulata.push_back(0.0);
        for (std::size_t i = 1; i < c.punti.size(); ++i) g.cumulata.push_back(g.cumulata.back() + distanza(c.punti[i - 1], c.punti[i]));
        double v = 1e9;
        for (const auto& [tipo, w] : c.miscela)
            if (const TipoCivile* t = catalogo_.tipo_civile(tipo)) v = std::min(v, t->velocita_servizio_kn.min);
        if (v >= 1e9) throw std::runtime_error("corsia '" + c.id + "' senza tipi civili noti nella miscela");
        g.v_min = 0.85 * v * NODO;
        geometrie_.push_back(std::move(g));
    }
}

void Traffico::da_corsie(std::vector<Candidata>& out, double t, Vec2 centro, double ora_locale) const {
    for (std::size_t ci = 0; ci < rotte_.corsie.size(); ++ci) {
        const CorsiaTraffico& c = rotte_.corsie[ci];
        if (!nel_mese(c.mesi, mese_) || c.navi_giorno <= 0.0) continue;
        const Geometria& g = geometrie_[ci];
        const double lunghezza = g.cumulata.back();
        const double durata_max = lunghezza / g.v_min;
        for (int senso = 0; senso < (c.doppio_senso ? 2 : 1); ++senso) {
            // gli arrivi a un capo: (istante, chiave)
            std::vector<std::pair<double, std::uint64_t>> arrivi;
            if (c.partenze_h.empty()) {
                const double lambda = c.navi_giorno / GIORNO_S * FASCIA_S;
                const long f0 = static_cast<long>(std::floor((t - durata_max) / FASCIA_S)), f1 = static_cast<long>(std::floor(t / FASCIA_S));
                for (long f = f0; f <= f1; ++f) {
                    const std::uint64_t k = chiave(seme_, ci, static_cast<std::uint64_t>(senso), static_cast<std::uint64_t>(f));
                    const int n = poisson(lambda, u01(k));
                    for (int j = 0; j < n; ++j) {
                        const std::uint64_t kj = chiave(k, static_cast<std::uint64_t>(j) + 1);
                        arrivi.emplace_back(static_cast<double>(f) * FASCIA_S + u01(kj ^ 1u) * FASCIA_S, kj);
                    }
                }
            } else {   // linee con l'orario: il giorno locale dalla mezzanotte di oggi
                const double mezzanotte = t - std::fmod(std::max(ora_locale, 0.0), 24.0) * 3600.0;
                const long giorno = std::lround(mezzanotte / GIORNO_S);
                const int giorni = static_cast<int>(std::ceil(durata_max / GIORNO_S)) + 1;
                for (int d = -giorni; d <= 0; ++d)
                    for (std::size_t h = 0; h < c.partenze_h.size(); ++h)
                        arrivi.emplace_back(mezzanotte + d * GIORNO_S + c.partenze_h[h] * 3600.0,
                                            chiave(seme_, ci, static_cast<std::uint64_t>(senso), static_cast<std::uint64_t>(giorno + d), h + 1000));
            }
            for (const auto& [arrivo, k] : arrivi) {
                if (arrivo > t) continue;
                const std::string& tipo = dalla_miscela(c.miscela, u01(k ^ 2u));
                const TipoCivile* tc = catalogo_.tipo_civile(tipo);
                if (!tc) continue;
                const double servizio = tc->velocita_servizio_kn.min + (tc->velocita_servizio_kn.max - tc->velocita_servizio_kn.min) * u01(k ^ 3u);
                const double v = servizio * (0.85 + 0.15 * u01(k ^ 4u));
                const double s = (t - arrivo) * v * NODO;
                if (s > lunghezza) continue;
                // a destra dell'asse a doppio senso, attorno all'asse a senso unico (sigma un quarto della larghezza)
                const double sigma = 0.25 * c.larghezza_m;
                const double scarto = c.doppio_senso ? 0.1 * c.larghezza_m + std::fabs(normale(k ^ 5u)) * sigma : normale(k ^ 5u) * sigma;
                const Vec2 pos = sulla_spezzata(c.punti, g.cumulata, s, senso == 1, scarto);
                if (distanza(pos, centro) > margine_m_) continue;
                const Vec2 avanti = sulla_spezzata(c.punti, g.cumulata, std::min(s + 100.0, lunghezza), senso == 1, scarto);
                Candidata x;
                x.chiave = k;
                x.tipo = tipo;
                x.seme = mescola(k ^ 6u);
                x.pos = pos;
                x.rotta = distanza(pos, avanti) > 1.0 ? rilevamento(pos, avanti) : 0.0;
                x.velocita_kn = v;
                out.push_back(std::move(x));
            }
        }
    }
}

void Traffico::da_zone(std::vector<Candidata>& out, double t, Vec2 centro) {
    for (std::size_t zi = 0; zi < rotte_.zone.size(); ++zi) {
        const ZonaPesca& z = rotte_.zone[zi];
        if (!nel_mese(z.mesi, mese_)) continue;
        auto& barche = rimorchi_[static_cast<int>(zi)];
        const int n = static_cast<int>(std::lround(z.navi));
        const std::size_t np = z.punti.size();
        const auto prossimo = [&](int barca, Rimorchio& r) {   // il rimorchio dopo: verso un punto vicino a caso
            const std::uint64_t k = chiave(seme_, zi + 7777, static_cast<std::uint64_t>(barca), static_cast<std::uint64_t>(r.numero) + 1);
            const Vec2 da = z.punti[static_cast<std::size_t>(r.a)];
            std::vector<int> vicini;
            for (std::size_t i = 0; i < np; ++i)
                if (static_cast<int>(i) != r.a && distanza(da, z.punti[i]) < VICINI_PESCA_M) vicini.push_back(static_cast<int>(i));
            const int a = vicini.empty() ? static_cast<int>(mescola(k) % np) : vicini[mescola(k) % vicini.size()];
            const double v = z.velocita_kn * (0.8 + 0.4 * u01(k ^ 9u)) * NODO;
            r.da = r.a;
            r.a = a;
            r.numero += 1;
            r.inizio_s = r.fine_s;
            r.fine_s = r.inizio_s + std::max(distanza(da, z.punti[static_cast<std::size_t>(a)]), 1000.0) / v;
        };
        if (barche.empty())
            for (int b = 0; b < n; ++b) {   // all'inizio: un punto a caso, a meta' di un rimorchio
                const std::uint64_t k = chiave(seme_, zi + 7777, static_cast<std::uint64_t>(b));
                Rimorchio r;
                r.a = static_cast<int>(mescola(k) % np);
                r.fine_s = -u01(k ^ 1u) * 3600.0;   // dall'inizio della partita: la stessa barca anche in una ripresa
                prossimo(b, r);
                barche.push_back(r);
            }
        for (int b = 0; b < static_cast<int>(barche.size()); ++b) {
            Rimorchio& r = barche[static_cast<std::size_t>(b)];
            while (r.fine_s < t) prossimo(b, r);
            const Vec2 p0 = z.punti[static_cast<std::size_t>(r.da)], p1 = z.punti[static_cast<std::size_t>(r.a)];
            const double u = std::clamp((t - r.inizio_s) / std::max(r.fine_s - r.inizio_s, 1.0), 0.0, 1.0);
            const Vec2 pos = piu(p0, per(meno(p1, p0), u));
            if (distanza(pos, centro) > margine_m_) continue;
            const std::uint64_t k = chiave(seme_, zi + 7777, static_cast<std::uint64_t>(b), 0xF15ull);
            Candidata x;
            x.chiave = k;
            x.tipo = dalla_miscela(z.miscela, u01(k ^ 2u));
            x.seme = mescola(k ^ 6u);
            x.pos = pos;
            x.rotta = distanza(p0, p1) > 1.0 ? rilevamento(p0, p1) : 0.0;
            x.velocita_kn = distanza(p0, p1) / std::max(r.fine_s - r.inizio_s, 1.0) / NODO;
            x.attivita = AttivitaNave::pesca;
            out.push_back(std::move(x));
        }
    }
}

std::vector<Traffico::Candidata> Traffico::candidate(double t, Vec2 centro, double ora_locale) {
    std::vector<Candidata> out;
    da_corsie(out, t, centro, ora_locale);
    da_zone(out, t, centro);
    std::erase_if(out, [&](const Candidata& c) {
        return std::find(perdute_.begin(), perdute_.end(), c.chiave) != perdute_.end() || !catalogo_.tipo_civile(c.tipo);
    });
    return out;
}

Traffico::Stato& Traffico::stato(const Candidata& c) {
    auto it = stati_.find(c.chiave);
    if (it == stati_.end()) {
        Stato s;
        s.indice = prossimo_++;
        s.individuo = genera_nave_civile(*catalogo_.tipo_civile(c.tipo), c.seme);
        it = stati_.emplace(c.chiave, std::move(s)).first;
        chiavi_[it->second.indice] = c.chiave;
    }
    return it->second;
}

NaveTraffico Traffico::nave_da(const Candidata& c, Stato& st, double t, double dt) {
    NaveTraffico n;
    n.indice = st.indice;
    n.tipo = c.tipo;
    const TipoCivile* tc = catalogo_.tipo_civile(c.tipo);
    n.classe = tc ? tc->classe : c.tipo;
    n.lunghezza_m = st.individuo.lunghezza_m;
    n.pescaggio_m = st.individuo.pescaggio_m;
    n.pos = c.pos;
    n.rotta = c.rotta;
    n.velocita_kn = c.velocita_kn;
    n.attivita = c.attivita;
    if (st.fuga_fino_s > t) {   // in fuga: via dal punto, alla velocita' massima
        const Vec2 via = unitario(meno(st.pos_fuga, st.fuga_da));
        st.pos_fuga = piu(st.pos_fuga, per(via, st.individuo.velocita_max_kn * NODO * dt));
        st.scarto = meno(st.pos_fuga, c.pos);
        st.scarto_da_s = t;
        n.pos = st.pos_fuga;
        n.rotta = rilevamento(Vec2{0.0, 0.0}, via);
        n.velocita_kn = st.individuo.velocita_max_kn;
        n.attivita = AttivitaNave::fuga;
        n.fuga_da = st.fuga_da;
        n.fuga_fino_s = st.fuga_fino_s;
    } else if (st.scarto_da_s >= 0.0) {   // il rientro in rotta
        const double k = 1.0 - (t - st.scarto_da_s) / RIENTRO_S;
        if (k > 0.0) n.pos = piu(c.pos, per(st.scarto, k));
        else st.scarto_da_s = -1.0;
    }
    return n;
}

Ricambio Traffico::avanza(double t, double dt, Vec2 centro, double ora_locale) {
    Ricambio r;
    const std::vector<Candidata> cand = candidate(t, centro, ora_locale);
    std::map<std::uint64_t, const Candidata*> per_chiave;
    for (const Candidata& c : cand) per_chiave[c.chiave] = &c;
    // le attive di prima che ci sono ancora (dentro il margine); le altre escono
    std::vector<std::uint64_t> attive;
    for (const NaveTraffico& n : navi_) {
        const std::uint64_t k = chiavi_.at(n.indice);
        if (per_chiave.count(k)) attive.push_back(k);
        else r.uscite.push_back(n.indice);
    }
    // le nuove dentro il raggio, dalla piu' vicina, fino al tetto; poi le piu' vicine scavalcano le piu' lontane
    std::vector<const Candidata*> fuori;
    for (const Candidata& c : cand)
        if (distanza(c.pos, centro) <= raggio_m_ && std::find(attive.begin(), attive.end(), c.chiave) == attive.end()) fuori.push_back(&c);
    std::sort(fuori.begin(), fuori.end(), [&](const Candidata* a, const Candidata* b) {
        const double da = distanza(a->pos, centro), db = distanza(b->pos, centro);
        return da != db ? da < db : a->chiave < b->chiave;
    });
    std::size_t prossima = 0;
    while (static_cast<int>(attive.size()) < tetto_ && prossima < fuori.size()) {
        attive.push_back(fuori[prossima]->chiave);
        r.entrate.push_back(stato(*fuori[prossima++]).indice);
    }
    for (int scambi = 0; scambi < SCAMBI_PER_PASSO && prossima < fuori.size() && !attive.empty(); ++scambi) {
        const auto lontana = std::max_element(attive.begin(), attive.end(), [&](std::uint64_t a, std::uint64_t b) {
            return distanza(per_chiave[a]->pos, centro) < distanza(per_chiave[b]->pos, centro);
        });
        if (distanza(fuori[prossima]->pos, centro) + SCAVALCA_M >= distanza(per_chiave[*lontana]->pos, centro)) break;
        r.uscite.push_back(stati_.at(*lontana).indice);
        *lontana = fuori[prossima]->chiave;
        r.entrate.push_back(stato(*fuori[prossima++]).indice);
    }
    navi_.clear();
    for (std::uint64_t k : attive) navi_.push_back(nave_da(*per_chiave[k], stati_.at(k), t, dt));
    std::sort(navi_.begin(), navi_.end(), [](const NaveTraffico& a, const NaveTraffico& b) { return a.indice < b.indice; });
    return r;
}

const NaveTraffico* Traffico::nave(int indice) const {
    for (const NaveTraffico& n : navi_)
        if (n.indice == indice) return &n;
    return nullptr;
}

const FirmaRumore* Traffico::firma(int indice) const {
    const auto it = chiavi_.find(indice);
    return it == chiavi_.end() ? nullptr : &stati_.at(it->second).individuo.firma;
}

void Traffico::fuggi(int indice, Vec2 da, double fino_s) {
    const auto it = chiavi_.find(indice);
    const NaveTraffico* n = nave(indice);
    if (it == chiavi_.end() || !n) return;
    Stato& st = stati_.at(it->second);
    if (n->attivita != AttivitaNave::fuga) st.pos_fuga = n->pos;   // una fuga nuova parte da dov'e'
    st.fuga_da = da;
    st.fuga_fino_s = fino_s;
}

void Traffico::perduta(int indice) {
    const auto it = chiavi_.find(indice);
    if (it == chiavi_.end()) return;
    perdute_.push_back(it->second);
    std::erase_if(navi_, [&](const NaveTraffico& n) { return n.indice == indice; });
}

}  // namespace m688
