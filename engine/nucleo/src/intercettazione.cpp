#include "nucleo/intercettazione.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

namespace {

constexpr double C_ACQUA = 1500.0;
constexpr double BANDA_RICEZIONE_HZ = 1000.0;    // il canale del ricevitore (dossier 28 §1.3: soglia in 1 kHz)
constexpr std::size_t INTERCETTI_PER_TRACCIA = 12;
constexpr double STORIA_S = 1800.0;
// la logica dell'allarme (dossier 28 §3.3, STIMA dai brevetti)
constexpr double TESTA_F_MIN_HZ = 15000.0, TESTA_DURATA_MAX_S = 0.05, TESTA_PRI_MAX_S = 3.0;
constexpr double ACQUISIZIONE_CALO = 0.15, ACQUISIZIONE_SALITA_DB = 3.0, ACQUISIZIONE_DERIVA_DEG_MIN = 2.0;

std::uint64_t mescola(std::uint64_t z) {
    z += 0x9e3779b97f4a7c15ull;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}
double u01(std::uint64_t z) { return static_cast<double>(mescola(z) >> 11) * 0x1.0p-53; }
double normale(std::uint64_t z) {
    const double u1 = std::max(u01(z), 1e-300), u2 = u01(z ^ 0x5bd1e995ull);
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}
double potenza(double db) { return std::pow(10.0, db / 10.0); }

TipoEmissione tipo_da_testo(const std::string& t) {
    if (t == "scafo") return TipoEmissione::scafo;
    if (t == "immersione") return TipoEmissione::immersione;
    if (t == "boa") return TipoEmissione::boa;
    if (t == "testa") return TipoEmissione::testa;
    if (t == "ricerca_hf") return TipoEmissione::ricerca_hf;
    return TipoEmissione::ignoto;
}

double mediana(std::vector<double> v) {
    if (v.empty()) return -1.0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

}  // namespace

const char* nome_tipo_emissione(TipoEmissione t) {
    switch (t) {
        case TipoEmissione::scafo: return "sonar di piattaforma";
        case TipoEmissione::immersione: return "sonar a immersione";
        case TipoEmissione::boa: return "boa attiva";
        case TipoEmissione::testa: return "testa d'arma";
        case TipoEmissione::ricerca_hf: return "sonar ad alta frequenza";
        case TipoEmissione::ignoto: return "ignoto";
    }
    return "?";
}

const char* nome_allarme(Allarme a) {
    switch (a) {
        case Allarme::nessuno: return "nessuno";
        case Allarme::siluro_in_acqua: return "SILURO IN ACQUA";
        case Allarme::siluro_in_acquisizione: return "SILURO IN ACQUISIZIONE";
    }
    return "?";
}

const DatiIntercettatore* CatalogoIntercettazione::intercettatore_di(const std::string& classe) const {
    for (const auto& [id, d] : intercettatori)
        if (std::find(d.classi.begin(), d.classi.end(), classe) != d.classi.end()) return &d;
    return nullptr;
}

const DatiEmissione* CatalogoIntercettazione::emissione(const std::string& id) const {
    const auto it = emissioni.find(id);
    return it == emissioni.end() ? nullptr : &it->second;
}

CatalogoIntercettazione carica_intercettazione(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    CatalogoIntercettazione out;
    if (const auto* tab = t["intercettatori"].as_table())
        for (const auto& [k, v] : *tab) {
            const auto* x = v.as_table();
            if (!x) continue;
            DatiIntercettatore d;
            d.id = std::string(k.str());
            if (const auto* bande = (*x)["bande"].as_array())
                for (const auto& b : *bande)
                    if (const auto* a = b.as_array(); a && a->size() == 3)
                        d.bande.push_back({(*a)[0].value_or(0.0), (*a)[1].value_or(0.0), (*a)[2].value_or(10.0)});
            if (d.bande.empty()) throw std::runtime_error(percorso + ": intercettatore '" + d.id + "' senza bande");
            d.soglia_db = (*x)["soglia_db"].value_or(10.0);
            d.risoluzione_hz = (*x)["risoluzione_hz"].value_or(100.0);
            d.misura_durata = (*x)["misura_durata"].value_or(true);
            d.misura_fm = (*x)["misura_fm"].value_or(false);
            d.falsi_allarmi_h = (*x)["falsi_allarmi_h"].value_or(0.0);
            if (const auto* c = (*x)["classi"].as_array())
                for (const auto& s : *c) d.classi.push_back(s.value_or(std::string{}));
            d.fonte = (*x)["fonte"].value_or(std::string{});
            out.intercettatori.emplace(d.id, std::move(d));
        }
    if (const auto* tab = t["emissioni"].as_table())
        for (const auto& [k, v] : *tab) {
            const auto* x = v.as_table();
            if (!x) continue;
            DatiEmissione e;
            e.id = std::string(k.str());
            e.tipo = tipo_da_testo((*x)["tipo"].value_or(std::string{}));
            e.f_hz = (*x)["f_hz"].value_or(3500.0);
            e.durata_s = (*x)["durata_s"].value_or(0.5);
            e.fm = (*x)["fm"].value_or(false);
            e.pri_s = (*x)["pri_s"].value_or(20.0);
            out.emissioni.emplace(e.id, std::move(e));
        }
    // le teste dei siluri dai loro transitori: il centro della banda dell'impulso e la sua durata
    if (const auto* tab = t["transitori"].as_table())
        for (const auto& [k, v] : *tab) {
            const std::string id(k.str());
            const auto* x = v.as_table();
            if (!x || id.rfind("ping_testa_", 0) != 0) continue;
            DatiEmissione e;
            e.id = id;
            e.tipo = TipoEmissione::testa;
            e.pri_s = 1.25;
            if (const auto* comp = (*x)["componenti"].as_array(); comp && !comp->empty())
                if (const auto* c0 = (*comp)[0].as_table()) {
                    e.f_hz = 0.5 * ((*c0)["f_lo"].value_or(25000.0) + (*c0)["f_hi"].value_or(35000.0));
                    e.durata_s = (*c0)["durata_s"].value_or(0.01);
                }
            out.emissioni.emplace(e.id, std::move(e));
        }
    return out;
}

RicevitoreIntercettazione::RicevitoreIntercettazione(const DatiIntercettatore& dati, const CatalogoIntercettazione& catalogo,
                                                     std::uint64_t seme)
    : dati_(dati), catalogo_(catalogo), seme_(mescola(seme ^ 0x1A7E2CE7ull)) {}

std::vector<Intercetto> RicevitoreIntercettazione::ascolta(const std::vector<EmissioneAttiva>& emissioni, const Ricevitore& r,
                                                           const Ambiente& amb, double t, double dt) {
    std::vector<Intercetto> nuovi;
    const auto banda_di = [&](double f) {
        for (std::size_t b = 0; b < dati_.bande.size(); ++b)
            if (f >= dati_.bande[b].f_lo_hz && f < dati_.bande[b].f_hi_hz) return static_cast<int>(b);
        return -1;
    };
    for (const EmissioneAttiva& e : emissioni) {
        const DatiEmissione* d = catalogo_.emissione(e.chiave);
        const double f = e.f_hz > 0.0 ? e.f_hz : d ? d->f_hz : 0.0;
        const int b = banda_di(f);
        if (b < 0) continue;
        const double dist = std::max(distanza(r.pos, e.pos), 1.0);
        const double tl = perdita_trasmissione(f, dist, e.quota_m, r.quota_m, amb).tl_db;
        double nl = potenza(rumore_ambiente(f, amb));
        if (r.sensore) nl += potenza(rumore_proprio(f, *r.sensore, r.velocita_kn, false));
        const double se = e.sl_db - tl - (10.0 * std::log10(nl) + 10.0 * std::log10(BANDA_RICEZIONE_HZ)) - dati_.soglia_db;
        if (se < 0.0) continue;
        // il Doppler a sola andata: la velocita' con cui l'emettitore si avvicina, meno quella con cui ci allontaniamo
        const Vec2 u{(r.pos.x - e.pos.x) / dist, (r.pos.y - e.pos.y) / dist};   // dall'emettitore al ricevitore
        const double ve = e.velocita_kn * NODO * (std::sin(radianti(e.rotta)) * u.x + std::cos(radianti(e.rotta)) * u.y);
        const double vr = r.velocita_kn * NODO * (std::sin(radianti(r.rotta)) * u.x + std::cos(radianti(r.rotta)) * u.y);
        const std::uint64_t k = mescola(seme_ ^ (static_cast<std::uint64_t>(e.emettitore) * 0x9E3779B97F4A7C15ull) ^
                                        static_cast<std::uint64_t>(std::llround(e.t * 1000.0)));
        Intercetto x;
        x.t = e.t;
        x.banda = b;
        x.f_hz = f * (1.0 + (ve - vr) / C_ACQUA) + 0.5 * dati_.risoluzione_hz * normale(k ^ 1u);
        x.durata_s = dati_.misura_durata && d ? d->durata_s * (1.0 + 0.05 * normale(k ^ 2u)) : -1.0;
        x.livello_db = se;
        x.rilevamento = normalizza_360(rilevamento(r.pos, e.pos) + 0.5 * dati_.bande[static_cast<std::size_t>(b)].errore_deg * normale(k ^ 3u));
        x.fm = dati_.misura_fm && d ? (d->fm ? 1 : 0) : -1;
        x.emettitore = e.emettitore;
        nuovi.push_back(x);
    }
    // i falsi allarmi: impulsi del proprio battello presi per intercetti (Kazakov 2019, «Rubin», «Skat-3»)
    if (dati_.falsi_allarmi_h > 0.0 && dt > 0.0) {
        const std::uint64_t k = mescola(seme_ ^ 0xFA15Eull ^ static_cast<std::uint64_t>(std::llround(t * 10.0)));
        if (u01(k) < dati_.falsi_allarmi_h * dt / 3600.0) {
            const std::size_t b = static_cast<std::size_t>(mescola(k ^ 1u) % dati_.bande.size());
            Intercetto x;
            x.t = t;
            x.banda = static_cast<int>(b);
            x.f_hz = dati_.bande[b].f_lo_hz + (dati_.bande[b].f_hi_hz - dati_.bande[b].f_lo_hz) * u01(k ^ 2u);
            x.durata_s = dati_.misura_durata ? 0.005 + 0.2 * u01(k ^ 3u) : -1.0;
            x.livello_db = 1.0 + 4.0 * u01(k ^ 4u);
            x.rilevamento = 360.0 * u01(k ^ 5u);
            x.spurio = true;
            nuovi.push_back(x);
        }
    }
    std::sort(nuovi.begin(), nuovi.end(), [](const Intercetto& a, const Intercetto& b) { return a.t < b.t; });
    for (const Intercetto& x : nuovi) {
        storia_.push_back(x);
        lega(x, t);
    }
    while (!storia_.empty() && storia_.front().t < t - STORIA_S) storia_.pop_front();
    // le tracce mute da tempo si chiudono
    std::erase_if(tracce_, [&](const TracciaIntercetto& tr) { return t - tr.ultimo_t > std::max(300.0, 6.0 * std::max(tr.pri_s, 0.0)); });
    return nuovi;
}

void RicevitoreIntercettazione::lega(const Intercetto& x, double t) {
    TracciaIntercetto* scelta = nullptr;
    for (TracciaIntercetto& tr : tracce_) {
        if (tr.intercetti.back().banda != x.banda) continue;
        const double df = std::fabs(tr.f_hz - x.f_hz), db = std::fabs(normalizza_180(tr.rilevamento - x.rilevamento));
        const double finestra = std::max(120.0, 4.0 * std::max(tr.pri_s, 0.0));
        const double err = dati_.bande[static_cast<std::size_t>(x.banda)].errore_deg;
        if (df < std::max(400.0, 0.03 * x.f_hz) && db < std::max(15.0, 3.0 * err) && x.t - tr.ultimo_t < finestra) {
            scelta = &tr;
            break;
        }
    }
    if (!scelta) {
        tracce_.push_back({});
        scelta = &tracce_.back();
        scelta->id = prossima_++;
    }
    scelta->intercetti.push_back(x);
    if (scelta->intercetti.size() > INTERCETTI_PER_TRACCIA) scelta->intercetti.erase(scelta->intercetti.begin());
    scelta->ultimo_t = std::max(scelta->ultimo_t, x.t);
    valuta_traccia(*scelta);
    (void)t;
}

void RicevitoreIntercettazione::valuta_traccia(TracciaIntercetto& tr) const {
    const auto& v = tr.intercetti;
    const Intercetto& ultimo = v.back();
    double f = 0.0;
    const std::size_t n3 = std::min<std::size_t>(3, v.size());
    for (std::size_t i = v.size() - n3; i < v.size(); ++i) f += v[i].f_hz;
    tr.f_hz = f / static_cast<double>(n3);
    tr.rilevamento = ultimo.rilevamento;
    tr.livello_db = ultimo.livello_db;
    tr.fm = ultimo.fm;
    std::vector<double> durate, intervalli;
    for (const Intercetto& x : v)
        if (x.durata_s >= 0.0) durate.push_back(x.durata_s);
    tr.durata_s = mediana(durate);
    for (std::size_t i = 1; i < v.size(); ++i) intervalli.push_back(v[i].t - v[i - 1].t);
    const bool cadenza = dati_.misura_durata && !intervalli.empty();   // senza la durata il ricevitore non tiene la cadenza
    tr.pri_s = cadenza ? mediana(std::vector<double>(intervalli.end() - static_cast<long>(std::min<std::size_t>(3, intervalli.size())), intervalli.end())) : -1.0;
    // la deriva del rilevamento: la retta sui rilevamenti dell'ultimo minuto (minimi quadrati), con la sua incertezza
    // dall'errore della banda; 0 se l'arco e' sotto i 10 s
    tr.deriva_deg_min = 0.0;
    tr.incertezza_deriva_deg_min = 0.0;
    {
        double st = 0.0, sb = 0.0, n = 0.0;
        for (const Intercetto& x : v)
            if (ultimo.t - x.t <= 60.0) { st += x.t; sb += normalizza_180(x.rilevamento - ultimo.rilevamento); n += 1.0; }
        double stt = 0.0, stb = 0.0;
        const double tm = n > 0.0 ? st / n : 0.0, bm = n > 0.0 ? sb / n : 0.0;
        double arco = 0.0;
        for (const Intercetto& x : v)
            if (ultimo.t - x.t <= 60.0) {
                const double dt = x.t - tm;
                stt += dt * dt;
                stb += dt * (normalizza_180(x.rilevamento - ultimo.rilevamento) - bm);
                arco = std::max(arco, ultimo.t - x.t);
            }
        if (n >= 3.0 && arco >= 10.0 && stt > 0.0) {
            tr.deriva_deg_min = stb / stt * 60.0;
            tr.incertezza_deriva_deg_min = 0.5 * dati_.bande[static_cast<std::size_t>(ultimo.banda)].errore_deg / std::sqrt(stt) * 60.0;
        }
    }
    // la salita del livello: rispetto all'intercetto di circa 10 s prima, riportata a 10 s
    tr.salita_db_10s = 0.0;
    for (auto it = v.rbegin() + 1; it != v.rend(); ++it)
        if (ultimo.t - it->t >= 8.0) {
            tr.salita_db_10s = (ultimo.livello_db - it->livello_db) / (ultimo.t - it->t) * 10.0;
            break;
        }
    // il Doppler: la frequenza contro quella tipica piu' vicina della stessa banda nel catalogo
    tr.doppler = 0;
    double vicina = -1.0;
    for (const auto& [id, e] : catalogo_.emissioni) {
        const auto& bb = dati_.bande[static_cast<std::size_t>(ultimo.banda)];
        if (e.f_hz < bb.f_lo_hz || e.f_hz >= bb.f_hi_hz) continue;
        if (vicina < 0.0 || std::fabs(e.f_hz - tr.f_hz) < std::fabs(vicina - tr.f_hz)) vicina = e.f_hz;
    }
    if (vicina > 0.0 && std::fabs(tr.f_hz - vicina) > dati_.risoluzione_hz) tr.doppler = tr.f_hz > vicina ? 1 : -1;
    // la classe
    const double f0 = tr.f_hz;
    if (tr.durata_s >= 0.0) {
        if (f0 >= TESTA_F_MIN_HZ && tr.durata_s <= TESTA_DURATA_MAX_S && (tr.pri_s < 0.0 || tr.pri_s <= TESTA_PRI_MAX_S))
            tr.classe = TipoEmissione::testa;
        else if (f0 >= TESTA_F_MIN_HZ)
            tr.classe = TipoEmissione::ricerca_hf;
        else if (f0 >= 8500.0 && f0 < 12000.0 && tr.durata_s <= TESTA_DURATA_MAX_S)
            tr.classe = TipoEmissione::immersione;
        else if (f0 >= 6000.0 && tr.durata_s >= 0.08 && tr.pri_s >= 6.0 && tr.pri_s <= 15.0)
            tr.classe = TipoEmissione::boa;
        else
            tr.classe = TipoEmissione::scafo;
    } else {   // il ricevitore vecchio: solo la frequenza
        tr.classe = f0 >= TESTA_F_MIN_HZ ? TipoEmissione::testa : TipoEmissione::scafo;
    }
    // l'allarme
    tr.allarme = Allarme::nessuno;
    if (tr.classe != TipoEmissione::testa || v.size() < 2) return;
    tr.allarme = Allarme::siluro_in_acqua;
    // la cadenza in calo: sugli ultimi intervalli (fino a 6) il calo supera il 15%, senza risalite oltre 0,1 s
    bool calo = false;
    if (cadenza && intervalli.size() >= 3) {
        const std::size_t m = intervalli.size(), da = m - std::min<std::size_t>(6, m);
        bool senza_risalite = true;
        for (std::size_t i = da + 1; i < m; ++i) senza_risalite = senza_risalite && intervalli[i] <= intervalli[i - 1] + 0.1;
        calo = senza_risalite && intervalli[m - 1] < (1.0 - ACQUISIZIONE_CALO) * intervalli[da];
    }
    const bool vicino = tr.salita_db_10s >= ACQUISIZIONE_SALITA_DB;
    // fermo: la deriva sotto 2 gradi al minuto, a meno del suo errore (due sigma)
    const bool fermo = std::fabs(tr.deriva_deg_min) < ACQUISIZIONE_DERIVA_DEG_MIN + 2.0 * tr.incertezza_deriva_deg_min;
    if ((cadenza ? calo : true) && vicino && fermo && tr.doppler >= 0) tr.allarme = Allarme::siluro_in_acquisizione;
}

Allarme RicevitoreIntercettazione::allarme() const {
    Allarme a = Allarme::nessuno;
    for (const TracciaIntercetto& tr : tracce_)
        if (static_cast<int>(tr.allarme) > static_cast<int>(a)) a = tr.allarme;
    return a;
}

}  // namespace m688
