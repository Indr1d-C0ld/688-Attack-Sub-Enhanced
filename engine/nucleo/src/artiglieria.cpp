#include "nucleo/artiglieria.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

constexpr double QUOTA_SUPERFICIE_M = 1.0;      // fino a qui il battello e' in superficie (scafo fuori)
constexpr double PD_FALLITA = 0.03;             // spoletta a percussione che non funziona: tonfo (STIMA)
constexpr double RIMBALZO_MIN_M = 50.0, RIMBALZO_MAX_M = 300.0;   // il proietto riparte e ricade (22 §9.4, STIMA)
constexpr double VT_MIN_M = 3.0, VT_MAX_M = 15.0;                 // scoppio di prossimita' sopra l'acqua (22 §3.5)
constexpr double AVVERTIMENTO_MIN_M = 300.0, AVVERTIMENTO_MAX_M = 500.0;   // 22 §9.2
constexpr double PLANCIA_M = 1.5;               // la cima della falsatorre: la plancia con le vedette
// le schegge di uno scoppio radente vanno avanti lungo la traiettoria: una testa fino a 50 m oltre lo scoppio, sulla
// linea di tiro, e' presa come una a r_eff (STIMA, tarata sulle probabilita' per colpo di 22 §3.6)
constexpr double SCHEGGE_AVANTI_M = 50.0;

double normale(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-300), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

double interpola(const std::vector<PuntoVolo>& v, double d, double PuntoVolo::*campo, double a_zero) {
    double d0 = 0.0, y0 = a_zero;
    for (const auto& p : v) {
        if (d <= p.distanza_m) {
            const double f = p.distanza_m > d0 ? (d - d0) / (p.distanza_m - d0) : 0.0;
            return y0 + f * (p.*campo - y0);
        }
        d0 = p.distanza_m;
        y0 = p.*campo;
    }
    if (v.size() < 2) return y0;   // oltre l'ultima riga: la pendenza delle ultime due
    const auto& a = v[v.size() - 2];
    const auto& b = v.back();
    return b.*campo + (d - b.distanza_m) * (b.*campo - a.*campo) / std::max(b.distanza_m - a.distanza_m, 1.0);
}

Vec2 piu(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
Vec2 per(Vec2 a, double k) { return {a.x * k, a.y * k}; }
double scalare(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
Vec2 direzione(double rotta_deg) { const double r = radianti(rotta_deg); return {std::sin(r), std::cos(r)}; }

// L'intervallo di s (lungo la linea di tiro u, a scarto laterale c) dentro il rettangolo di centro `centro`, assi lungo
// la rotta `a`, semilati hl e hw: vuoto se lo1 > hi1
std::pair<double, double> dentro_rettangolo(Vec2 u, Vec2 n, double c, Vec2 centro, Vec2 a, double hl, double hw) {
    const Vec2 b{a.y, -a.x};
    double lo = -std::numeric_limits<double>::infinity(), hi = std::numeric_limits<double>::infinity();
    for (const auto& [asse, h] : {std::pair<Vec2, double>{a, hl}, {b, hw}}) {
        const double k = scalare(u, asse), x0 = c * scalare(n, asse) - scalare(centro, asse);
        if (std::fabs(k) < 1e-12) {
            if (std::fabs(x0) > h) return {1.0, 0.0};
            continue;
        }
        double s1 = (-h - x0) / k, s2 = (h - x0) / k;
        if (s1 > s2) std::swap(s1, s2);
        lo = std::max(lo, s1);
        hi = std::min(hi, s2);
    }
    return {lo, hi};
}

struct Contatto {
    ParteEsposta parte = ParteEsposta::nessuna;
    double s = std::numeric_limits<double>::infinity(), quota = 0.0;
};

// Il primo contatto del proietto con la sagoma (coordinate relative al centro dello scafo): la traiettoria scende
// lungo u a scarto laterale c con l'angolo theta e tocca l'acqua in s_imp; vale solo per s >= s_da (rimbalzo)
Contatto primo_contatto(const SagomaEsposta& g, Vec2 u, Vec2 n, double s_imp, double c, double tan_theta, double raggio_proietto,
                        double s_da = -std::numeric_limits<double>::infinity()) {
    Contatto best;
    const auto prova = [&](ParteEsposta parte, double a, double b, double altezza) {
        const double lo = std::max({a, s_imp - altezza / tan_theta, s_da}), hi = std::min(b, s_imp);
        if (lo > hi || lo >= best.s) return;
        best = {parte, lo, (s_imp - lo) * tan_theta};
    };
    for (const auto& al : g.alberi) {
        const Vec2 rel{al.pos.x - g.pos.x, al.pos.y - g.pos.y};
        const double s0 = scalare(rel, u), c0 = scalare(rel, n), r = al.diametro_m / 2.0 + raggio_proietto;
        const double dc = c - c0;
        if (std::fabs(dc) > r) continue;
        const double w = std::sqrt(r * r - dc * dc);
        prova(al.parte, s0 - w, s0 + w, al.altezza_m);
    }
    const Vec2 a = direzione(g.rotta_deg);
    if (g.falsatorre_altezza_m > 0.0) {
        const auto [lo, hi] = dentro_rettangolo(u, n, c, per(a, g.falsatorre_avanti_m), a, g.falsatorre_lunghezza_m / 2.0 + raggio_proietto,
                                                g.falsatorre_larghezza_m / 2.0 + raggio_proietto);
        prova(ParteEsposta::falsatorre, lo, hi, g.falsatorre_altezza_m);
        if (best.parte == ParteEsposta::falsatorre && g.in_superficie && best.quota >= g.falsatorre_altezza_m - PLANCIA_M)
            best.parte = ParteEsposta::plancia;
    }
    if (g.in_superficie && g.bordo_libero_m > 0.0) {
        const auto [lo, hi] = dentro_rettangolo(u, n, c, {0.0, 0.0}, a, g.lunghezza_m / 2.0 + raggio_proietto,
                                                g.larghezza_m / 2.0 + raggio_proietto);
        prova(ParteEsposta::scafo, lo, hi, g.bordo_libero_m);
    }
    return best;
}

// La parte piu' vicina a uno scoppio (orizzontale sull'acqua, `quota` sopra) entro `r_eff`: teste e alberi come
// cilindri, falsatorre e scafo come scatole
std::pair<ParteEsposta, double> parte_vicina(const SagomaEsposta& g, Vec2 punto, double quota, double r_eff, Vec2 u) {
    ParteEsposta migliore = ParteEsposta::nessuna;
    double dmin = std::numeric_limits<double>::infinity();
    const auto considera = [&](ParteEsposta p, double orizzontale, double altezza) {
        const double d = std::hypot(std::max(0.0, orizzontale), std::max(0.0, quota - altezza));
        if (d < dmin) { dmin = d; migliore = p; }
    };
    for (const auto& al : g.alberi) {   // il cono delle schegge in avanti: davanti allo scoppio la distanza si accorcia
        const Vec2 rel{al.pos.x - punto.x, al.pos.y - punto.y};
        const double avanti = scalare(rel, u), lato = std::fabs(rel.x * u.y - rel.y * u.x);
        const double lungo = avanti > 0.0 ? avanti * r_eff / SCHEGGE_AVANTI_M : -avanti;
        considera(al.parte, std::hypot(lato, lungo) - al.diametro_m / 2.0, al.altezza_m);
    }
    const Vec2 a = direzione(g.rotta_deg), b{a.y, -a.x};
    const auto scatola = [&](ParteEsposta p, Vec2 centro, double hl, double hw, double altezza) {
        const Vec2 rel{punto.x - centro.x, punto.y - centro.y};
        const double dx = std::max(0.0, std::fabs(scalare(rel, a)) - hl), dy = std::max(0.0, std::fabs(scalare(rel, b)) - hw);
        considera(p, std::hypot(dx, dy), altezza);
    };
    if (g.falsatorre_altezza_m > 0.0)
        scatola(g.in_superficie ? ParteEsposta::plancia : ParteEsposta::falsatorre, piu(g.pos, per(a, g.falsatorre_avanti_m)),
                g.falsatorre_lunghezza_m / 2.0, g.falsatorre_larghezza_m / 2.0, g.falsatorre_altezza_m);
    if (g.in_superficie && g.bordo_libero_m > 0.0)
        scatola(ParteEsposta::scafo, g.pos, g.lunghezza_m / 2.0, g.larghezza_m / 2.0, g.bordo_libero_m);
    if (dmin > r_eff) return {ParteEsposta::nessuna, -1.0};
    return {migliore, dmin};
}

}  // namespace

// --- dati ---

double DatiPezzo::tempo_volo_s(double d) const {
    if (volo.empty()) return d / std::max(v0_m_s, 1.0);
    return std::max(0.0, interpola(volo, d, &PuntoVolo::volo_s, 0.0));
}
double DatiPezzo::caduta_gradi(double d) const { return std::clamp(interpola(volo, d, &PuntoVolo::caduta_gradi, 0.0), 0.05, 80.0); }
double DatiPezzo::velocita_arrivo_m_s(double d) const { return std::max(50.0, interpola(volo, d, &PuntoVolo::velocita_m_s, v0_m_s)); }

const DatiPezzo* TabellaArtiglieria::pezzo(const std::string& id) const {
    const auto it = pezzi.find(id);
    return it == pezzi.end() ? nullptr : &it->second;
}

std::vector<const DatiPezzo*> TabellaArtiglieria::pezzi_di(const std::string& classe) const {
    std::vector<const DatiPezzo*> v;
    if (const auto it = classi.find(classe); it != classi.end())
        for (const auto& id : it->second)
            if (const DatiPezzo* p = pezzo(id)) v.push_back(p);
    return v;
}

SagomaClasse TabellaArtiglieria::sagoma(const std::string& classe) const {
    if (const auto it = sagome.find(classe); it != sagome.end()) return it->second;
    if (const auto it = sagome.find("_predefinita"); it != sagome.end()) return it->second;
    return {};
}

TabellaArtiglieria carica_artiglieria(const std::string& percorso) {
    TabellaArtiglieria out;
    if (!std::filesystem::exists(percorso)) return out;
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const auto numero = [&](const toml::table& tab, const std::string& id, const char* campo) {
        const auto v = tab[campo].value<double>();
        if (!v) throw std::runtime_error(percorso + ": pezzo '" + id + "' senza " + campo);
        return *v;
    };
    if (const auto* pezzi = t["pezzi"].as_table())
        for (auto&& [chiave, nodo] : *pezzi) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            DatiPezzo p;
            p.id = std::string(chiave.str());
            p.nome = (*tab)["nome"].value_or(p.id);
            p.calibro_mm = numero(*tab, p.id, "calibro_mm");
            p.canne = (*tab)["canne"].value_or(1);
            p.cadenza_min = numero(*tab, p.id, "cadenza_min");
            p.colpi_pronti = (*tab)["colpi_pronti"].value_or(80);
            p.ricarica_s = (*tab)["ricarica_s"].value_or(0.0);
            p.deposito = (*tab)["deposito"].value_or(0);
            p.carica_kg = numero(*tab, p.id, "carica_kg");
            p.v0_m_s = numero(*tab, p.id, "v0_m_s");
            p.altezza_m = (*tab)["altezza_m"].value_or(p.altezza_m);
            p.depressione_gradi = (*tab)["depressione_gradi"].value_or(p.depressione_gradi);
            p.portata_minima_m = (*tab)["portata_minima_m"].value_or(0.0);
            p.portata_superficie_m = numero(*tab, p.id, "portata_superficie_m");
            p.portata_periscopio_m = (*tab)["portata_periscopio_m"].value_or(0.0);
            p.reazione_combattimento_s = (*tab)["reazione_combattimento_s"].value_or(p.reazione_combattimento_s);
            p.reazione_condizione3_s = (*tab)["reazione_condizione3_s"].value_or(p.reazione_condizione3_s);
            p.sigma_direzione_mrad = (*tab)["sigma_direzione_mrad"].value_or(p.sigma_direzione_mrad);
            p.sigma_distanza_m = (*tab)["sigma_distanza_m"].value_or(p.sigma_distanza_m);
            p.sigma_distanza_frazione = (*tab)["sigma_distanza_frazione"].value_or(p.sigma_distanza_frazione);
            p.r_eff_m = (*tab)["r_eff_m"].value_or(p.r_eff_m);
            const std::string s = (*tab)["spoletta"].value_or(std::string("pd"));
            p.spoletta = s == "vt" ? Spoletta::vt : s == "raffica" ? Spoletta::raffica : Spoletta::pd;
            p.raffica_colpi = (*tab)["raffica_colpi"].value_or(0);
            p.raffica_s = (*tab)["raffica_s"].value_or(0.0);
            p.fonte = (*tab)["fonte"].value_or(std::string{});
            p.confidenza = (*tab)["confidenza"].value_or(std::string{});
            if (const auto* v = (*tab)["volo"].as_array())
                for (const auto& riga : *v)
                    if (const auto* r = riga.as_array(); r && r->size() >= 4)
                        p.volo.push_back({(*r)[0].value_or(0.0), (*r)[1].value_or(0.0), (*r)[2].value_or(0.0), (*r)[3].value_or(0.0)});
            std::sort(p.volo.begin(), p.volo.end(), [](const PuntoVolo& a, const PuntoVolo& b) { return a.distanza_m < b.distanza_m; });
            out.pezzi.emplace(p.id, std::move(p));
        }
    if (const auto* classi = t["classi"].as_table())
        for (auto&& [chiave, nodo] : *classi)
            if (const auto* tab = nodo.as_table())
                if (const auto* v = (*tab)["pezzi"].as_array()) {
                    auto& l = out.classi[std::string(chiave.str())];
                    for (const auto& x : *v)
                        if (const auto id = x.value<std::string>()) l.push_back(*id);
                }
    if (const auto* sagome = t["sagome"].as_table())
        for (auto&& [chiave, nodo] : *sagome)
            if (const auto* tab = nodo.as_table()) {
                SagomaClasse s;
                s.bordo_libero_m = (*tab)["bordo_libero_m"].value_or(s.bordo_libero_m);
                s.falsatorre_lunghezza_m = (*tab)["falsatorre_lunghezza_m"].value_or(s.falsatorre_lunghezza_m);
                s.falsatorre_larghezza_m = (*tab)["falsatorre_larghezza_m"].value_or(s.falsatorre_larghezza_m);
                s.falsatorre_altezza_m = (*tab)["falsatorre_altezza_m"].value_or(s.falsatorre_altezza_m);
                s.falsatorre_avanti_m = (*tab)["falsatorre_avanti_m"].value_or(s.falsatorre_avanti_m);
                out.sagome[std::string(chiave.str())] = s;
            }
    return out;
}

const char* nome_parte(ParteEsposta p) {
    switch (p) {
        case ParteEsposta::nessuna: return "nessuna";
        case ParteEsposta::periscopio_attacco: return "periscopio_attacco";
        case ParteEsposta::periscopio_ricerca: return "periscopio_ricerca";
        case ParteEsposta::albero_esm: return "albero_esm";
        case ParteEsposta::albero_radio: return "albero_radio";
        case ParteEsposta::radar: return "radar";
        case ParteEsposta::snorkel: return "snorkel";
        case ParteEsposta::falsatorre: return "falsatorre";
        case ParteEsposta::plancia: return "plancia";
        case ParteEsposta::scafo: return "scafo";
    }
    return "?";
}

const char* nome_esito(ImpattoArtiglieria::Esito e) {
    using E = ImpattoArtiglieria::Esito;
    switch (e) {
        case E::acqua: return "acqua";
        case E::rimbalzo: return "rimbalzo";
        case E::tonfo: return "tonfo";
        case E::aria: return "aria";
        case E::diretto: return "diretto";
        case E::schegge: return "schegge";
    }
    return "?";
}

SagomaEsposta sagoma_sottomarino(const SagomaClasse& c, int unita, Vec2 pos, double rotta_deg, double velocita_m_s,
                                 double lunghezza_m, double larghezza_m, double quota_m, double vela_affiorata_m,
                                 const std::vector<AlberoAlzato>& alberi) {
    SagomaEsposta g;
    g.unita = unita;
    g.pos = pos;
    g.rotta_deg = rotta_deg;
    g.velocita_m_s = velocita_m_s;
    g.lunghezza_m = lunghezza_m;
    g.larghezza_m = larghezza_m;
    g.falsatorre_lunghezza_m = c.falsatorre_lunghezza_m;
    g.falsatorre_larghezza_m = c.falsatorre_larghezza_m;
    g.falsatorre_avanti_m = c.falsatorre_avanti_m;
    g.in_superficie = quota_m <= QUOTA_SUPERFICIE_M;
    double base = 0.0;   // gli alberi partono dall'acqua, o dalla cima della falsatorre in superficie
    if (g.in_superficie) {
        g.bordo_libero_m = c.bordo_libero_m;
        g.falsatorre_altezza_m = c.falsatorre_altezza_m;
        base = c.falsatorre_altezza_m;
    } else if (vela_affiorata_m > 0.0) {
        g.falsatorre_altezza_m = std::min(vela_affiorata_m, c.falsatorre_altezza_m);
    }
    const Vec2 vela = piu(pos, per(direzione(rotta_deg), c.falsatorre_avanti_m));
    for (const auto& a : alberi)
        if (a.altezza_m > 0.0) g.alberi.push_back({a.parte, vela, (g.in_superficie ? base : 0.0) + a.altezza_m, a.diametro_m});
    return g;
}

Vec2 punto_di_mira(const SagomaEsposta& g) {
    if (!g.in_superficie && !g.alberi.empty())
        return std::max_element(g.alberi.begin(), g.alberi.end(), [](const PezzoEsposto& a, const PezzoEsposto& b) {
                   return a.altezza_m < b.altezza_m;
               })->pos;
    return piu(g.pos, per(direzione(g.rotta_deg), g.falsatorre_avanti_m));
}

// --- tiro ---

double CorrezioniTiro::fattore() const {
    double k = fattore_difficolta;
    if (stato_mare >= 5) k *= 0.3;
    else if (stato_mare == 4) k *= 0.6;
    if (velocita_nave_kn > 20.0 || in_accostata) k *= 0.5;
    if (primo_colpo) k *= 0.5;
    if (notte) k *= illuminante || radar_tiro ? 0.8 : 0.3;
    return std::clamp(k, 0.02, 4.0);
}

std::vector<ImpattoArtiglieria> spara(const DatiPezzo& p, Vec2 nave, Vec2 stimata, Vec2 velocita_stimata_m_s,
                                      const SagomaEsposta& vera, double tempo_s, const CorrezioniTiro& c, Casuale& caso,
                                      bool avvertimento) {
    std::vector<ImpattoArtiglieria> out;
    // la mira: il bersaglio stimato portato avanti per il tempo di volo (due passi bastano)
    double tv = p.tempo_volo_s(distanza(nave, stimata));
    Vec2 mira = piu(stimata, per(velocita_stimata_m_s, tv));
    const double r = std::max(distanza(nave, mira), 1.0);
    tv = p.tempo_volo_s(r);
    mira = piu(stimata, per(velocita_stimata_m_s, tv));
    const Vec2 u{(mira.x - nave.x) / r, (mira.y - nave.y) / r}, n{u.y, -u.x};
    const double theta = p.caduta_gradi(r), tan_theta = std::tan(radianti(theta));
    ImpattoArtiglieria base;
    base.tiratore = -1;
    base.bersaglio = vera.unita;
    base.pezzo = p.id;
    base.calibro_mm = p.calibro_mm;
    base.carica_kg = p.carica_kg;
    base.r_eff_m = p.r_eff_m;
    base.tempo_sparo_s = tempo_s;
    base.tempo_impatto_s = tempo_s + tv;
    base.angolo_caduta_gradi = theta;
    base.velocita_m_s = p.velocita_arrivo_m_s(r);
    if (avvertimento) {   // davanti o accanto, mai sul bersaglio
        ImpattoArtiglieria i = base;
        i.avvertimento = true;
        const double scarto = caso.uniforme(AVVERTIMENTO_MIN_M, AVVERTIMENTO_MAX_M) * (caso.uniforme() < 0.5 ? -1.0 : 1.0);
        i.punto = piu(mira, per(n, scarto));
        out.push_back(i);
        return out;
    }
    const double k = std::sqrt(c.fattore());
    const double sd = p.sigma_direzione_mrad / 1000.0 * r / k, sr = (p.sigma_distanza_m + p.sigma_distanza_frazione * r) / k;
    // il bersaglio vero all'impatto (centro dello scafo, teste e alberi spostati insieme)
    SagomaEsposta g = vera;
    const Vec2 spostamento = per(direzione(vera.rotta_deg), vera.velocita_m_s * tv);
    g.pos = piu(g.pos, spostamento);
    for (auto& al : g.alberi) al.pos = piu(al.pos, spostamento);
    const double raggio_proietto = p.calibro_mm / 2000.0;
    const auto colpo = [&](Vec2 punto, double t_impatto) {
        // relativo al bersaglio: s lungo il tiro, c di traverso
        const Vec2 rel{punto.x - g.pos.x, punto.y - g.pos.y};
        const double s_imp = scalare(rel, u), cl = scalare(rel, n);
        std::vector<ImpattoArtiglieria> v;
        ImpattoArtiglieria i = base;
        i.tempo_impatto_s = t_impatto;
        const auto diretto = [&](const Contatto& x) {
            i.esito = ImpattoArtiglieria::Esito::diretto;
            i.parte = x.parte;
            i.distanza_parte_m = 0.0;
            i.punto = piu(g.pos, piu(per(u, x.s), per(n, cl)));
            i.quota_m = x.quota;
            v.push_back(i);
        };
        if (const Contatto x = primo_contatto(g, u, n, s_imp, cl, tan_theta, raggio_proietto); x.parte != ParteEsposta::nessuna) {
            diretto(x);
            return v;
        }
        i.punto = punto;
        if (p.spoletta == Spoletta::vt && g.in_superficie) {   // scoppio sopra l'acqua prima del punto di caduta
            const double h = caso.uniforme(VT_MIN_M, VT_MAX_M);
            i.esito = ImpattoArtiglieria::Esito::aria;
            i.quota_m = h;
            i.punto = piu(punto, per(u, -h / tan_theta));
        } else if (p.spoletta != Spoletta::raffica && caso.uniforme() < PD_FALLITA) {
            i.esito = ImpattoArtiglieria::Esito::tonfo;
        } else if (p.spoletta == Spoletta::pd && theta < ANGOLO_RIMBALZO_GRADI && caso.uniforme() < 0.5) {
            i.esito = ImpattoArtiglieria::Esito::rimbalzo;   // il tonfo e il proietto che riparte
            v.push_back(i);
            const double l = caso.uniforme(RIMBALZO_MIN_M, RIMBALZO_MAX_M);
            ImpattoArtiglieria j = base;
            j.tempo_impatto_s = t_impatto + l / std::max(0.5 * i.velocita_m_s, 50.0);
            j.velocita_m_s = 0.5 * i.velocita_m_s;
            if (const Contatto x = primo_contatto(g, u, n, s_imp + l, cl, tan_theta, raggio_proietto, s_imp); x.parte != ParteEsposta::nessuna) {
                i = j;
                diretto(x);
                return v;
            }
            j.punto = piu(punto, per(u, l));
            const auto [parte, d] = parte_vicina(g, j.punto, 0.0, p.r_eff_m, u);
            j.esito = parte == ParteEsposta::nessuna ? ImpattoArtiglieria::Esito::acqua : ImpattoArtiglieria::Esito::schegge;
            j.parte = parte;
            j.distanza_parte_m = d;
            v.push_back(j);
            return v;
        }
        if (i.esito != ImpattoArtiglieria::Esito::tonfo) {
            const auto [parte, d] = parte_vicina(g, i.punto, i.quota_m, p.r_eff_m, u);
            if (parte != ParteEsposta::nessuna) {
                i.esito = i.esito == ImpattoArtiglieria::Esito::aria ? ImpattoArtiglieria::Esito::aria : ImpattoArtiglieria::Esito::schegge;
                i.parte = parte;
                i.distanza_parte_m = d;
            }
        }
        v.push_back(i);
        return v;
    };
    if (p.raffica()) {   // N proietti con la sigma della raffica: il centro per gli effetti, poi quelli che toccano
        const int nc = std::max(1, p.raffica_colpi);
        ImpattoArtiglieria centro = base;
        centro.raffica = true;
        centro.colpi = nc;
        const double dr0 = normale(caso) * sr, dc0 = normale(caso) * sd;   // l'errore di puntamento della raffica
        centro.punto = piu(mira, piu(per(u, dr0), per(n, dc0)));
        out.push_back(centro);
        for (int k2 = 0; k2 < nc; ++k2) {
            const double t = base.tempo_impatto_s + (nc > 1 ? p.raffica_s * k2 / (nc - 1) : 0.0);
            const Vec2 punto = piu(centro.punto, piu(per(u, normale(caso) * sr * 0.5), per(n, normale(caso) * sd * 0.5)));
            for (auto& x : colpo(punto, t))
                if (x.parte != ParteEsposta::nessuna) out.push_back(x);
        }
        return out;
    }
    const Vec2 punto = piu(mira, piu(per(u, normale(caso) * sr), per(n, normale(caso) * sd)));
    return colpo(punto, base.tempo_impatto_s);
}

std::vector<EventoArtiglieria> eventi_di(const ImpattoArtiglieria& i, Vec2 nave, bool partenza) {
    std::vector<EventoArtiglieria> v;
    EventoArtiglieria e;
    e.calibro_mm = i.calibro_mm;
    e.tiratore = i.tiratore;
    e.bersaglio = i.bersaglio;
    if (partenza) {
        e.tempo_s = i.tempo_sparo_s;
        e.tipo = TipoEventoArtiglieria::partenza;
        e.pos = nave;
        e.carica_kg = 0.0;
        v.push_back(e);
    }
    e.tempo_s = i.tempo_impatto_s;
    e.pos = i.punto;
    e.quota_m = 0.0;
    e.carica_kg = i.carica_kg;
    using E = ImpattoArtiglieria::Esito;
    if (i.raffica) {
        e.tipo = TipoEventoArtiglieria::raffica;
        e.colpi = i.colpi;
        e.intervallo_s = 0.013;   // AK-630 (acustica_artiglieria.hpp)
        v.push_back(e);
        return v;
    }
    const bool albero = i.parte != ParteEsposta::nessuna && i.parte != ParteEsposta::falsatorre &&
                        i.parte != ParteEsposta::plancia && i.parte != ParteEsposta::scafo;
    switch (i.esito) {
        case E::acqua:
        case E::schegge: e.tipo = TipoEventoArtiglieria::scoppio_superficie; break;
        case E::rimbalzo: e.tipo = TipoEventoArtiglieria::rimbalzo; e.carica_kg = 0.0; break;
        case E::tonfo: e.tipo = TipoEventoArtiglieria::tonfo; e.carica_kg = 0.0; break;
        case E::aria: e.tipo = TipoEventoArtiglieria::scoppio_aria; e.quota_m = i.quota_m; break;
        case E::diretto:
            e.tipo = albero ? TipoEventoArtiglieria::colpo_testa : TipoEventoArtiglieria::scoppio_aria;
            e.quota_m = i.quota_m;
            break;
    }
    v.push_back(e);
    if (i.esito == E::schegge && albero) {   // le schegge sulla testa: il colpo che si sente a bordo
        EventoArtiglieria t = e;
        t.tipo = TipoEventoArtiglieria::colpo_testa;
        t.carica_kg = 0.0;
        v.push_back(t);
    }
    return v;
}

}  // namespace m688
