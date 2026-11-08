#pragma once
// La scritta sulla carta del replay per gli interventi d'AMBIENTE della D4 (docs/28 §7): lo strato, la deriva del ghiaccio
// e la polynya, e se la transizione e' in corso. Vuota per tutti gli altri (trailer-1)
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "missioni/griglia_ghiaccio.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/modifica_profilo.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

inline std::string riga_ambiente(const RecordIntervento& x, double t) {
    if (x.tipo != "strato" && x.tipo != "ghiaccio_deriva" && x.tipo != "polynya") return {};
    const auto chiavi = leggi_chiavi(x.chiavi);
    const auto testo = [&](const char* k) { const auto it = chiavi.find(k); return it == chiavi.end() ? std::string() : it->second; };
    const auto numero = [&](const char* k) { const auto it = chiavi.find(std::string("#") + k); return it == chiavi.end() ? 0.0 : std::strtod(it->second.c_str(), nullptr); };
    const auto cifra = [](double v, const char* formato) {
        char b[32];
        std::snprintf(b, sizeof b, formato, v);
        std::string s = b;
        std::replace(s.begin(), s.end(), '.', ',');
        return s;
    };
    std::string r;
    if (x.tipo == "strato") {
        const std::string m = testo("modifica");
        r = m == "togli"      ? "STRATO TOLTO"
            : m == "rinforza" ? "STRATO X" + cifra(numero("fattore"), "%g")
            : m == "canale"   ? "CANALE A " + cifra(numero("quota_m"), "%.0f") + " M"
                              : "STRATO A " + cifra(numero("quota_m"), "%.0f") + " M";
    } else if (x.tipo == "ghiaccio_deriva") {
        r = "GHIACCIO VERSO " + cifra(numero("rotta"), "%03.0f") + " A " + cifra(numero("velocita_kn"), "%.1f") + " KN";
    } else {
        r = testo("azione") == "chiudi" ? "POLYNYA SI CHIUDE" : "POLYNYA SI APRE";
    }
    const double transizione = numero("transizione_s");
    if (transizione > 0.0 && t - x.t < transizione) r += " IN CORSO";   // il font del gioco non ha il segno di percentuale
    return r;
}

// Il profilo del suono per la scheda dello spettatore (docs/28 §3.3), dai soli record X: quello del mese con sopra gli strati
// cambiati la cui zona contiene il punto, con gli stessi passi di Missione::profilo_in. STIMA: senza l'effetto pomeriggio, che dipende
// dall'ora, dal vento e dalle nubi di ogni istante, e nella traccia non c'e'
inline ProfiloSuono profilo_spettatore(const ProfiloSuono& base, const std::vector<RecordIntervento>& interventi, Vec2 p, double t) {
    ProfiloSuono out = base;
    std::vector<const RecordIntervento*> strati;
    for (const RecordIntervento& x : interventi)
        if (x.tipo == "strato" && x.fase == RecordIntervento::FASE_APPLICATO) strati.push_back(&x);
    std::stable_sort(strati.begin(), strati.end(), [](const RecordIntervento* a, const RecordIntervento* b) { return a->t < b->t; });
    for (const RecordIntervento* x : strati) {
        if (x->ha_punto && x->raggio_m > 0.0 && distanza(p, x->punto) > x->raggio_m) continue;
        const auto chiavi = leggi_chiavi(x->chiavi);
        const auto numero = [&](const char* k, double d) { const auto it = chiavi.find(k); return it == chiavi.end() ? d : std::strtod(it->second.c_str(), nullptr); };
        const auto testo = chiavi.find("modifica");
        const std::string k = testo == chiavi.end() ? std::string() : testo->second;
        const double transizione = numero("#transizione_s", 0.0);
        const double durata = x->durata_s > 0.0 ? std::max(x->durata_s - transizione, 1.0) : 0.0;
        const int passo = static_cast<int>(std::lround(4.0 * fase_cambiamento(t, x->t, transizione, durata)));
        if (passo <= 0) continue;
        ModificaStrato m;
        m.tipo = k == "sposta" ? ModificaStrato::Tipo::sposta : k == "rinforza" ? ModificaStrato::Tipo::rinforza : k == "canale" ? ModificaStrato::Tipo::canale : ModificaStrato::Tipo::togli;
        m.valore = m.tipo == ModificaStrato::Tipo::rinforza ? numero("#fattore", 1.0) : numero("#quota_m", 0.0);
        out = modifica_profilo(out, m, passo / 4.0);
    }
    return out;
}

// Le due righe del profilo sotto la scheda; fra parentesi com'era, se l'intervento l'ha cambiato
inline std::string riga_strato(const ProfiloSuono& ora, const ProfiloSuono& base) {
    const auto dice = [](const ProfiloSuono& p) {
        // senza SLD, o nelle acque polari, dove c sale dalla superficie e la SLD e' solo il tetto della ricerca: nessuno strato da passare
        if (p.sld_m <= 0.0 || !modifica_applicabile(p, ModificaStrato::Tipo::sposta)) return std::string("NESSUNO");
        return std::to_string(static_cast<int>(std::lround(p.sld_m))) + " M";
    };
    const std::string a = dice(ora), b = dice(base);
    return "STRATO " + a + (a == b ? std::string() : " (ERA " + b + ")");
}
inline std::string riga_asse(const ProfiloSuono& ora, const ProfiloSuono& base) {
    const auto dice = [](const ProfiloSuono& p) { return std::to_string(static_cast<int>(std::lround(p.asse_m))) + " M"; };
    const std::string a = dice(ora), b = dice(base);
    return "ASSE " + a + (a == b ? std::string() : " (ERA " + b + ")");
}
// La profondita' mostrata dal grafico: quanto basta per lo strato e per l'unita', non oltre i dati del profilo
inline double scala_suono(const ProfiloSuono& ora, const ProfiloSuono& base, double quota_m) {
    const double dentro = 1.5 * std::max({ora.sld_m, base.sld_m, quota_m, 200.0});
    const double dati = ora.z_m.empty() ? dentro : std::max(50.0, ora.z_m.back());
    return std::min(std::ceil(dentro / 50.0) * 50.0, dati);
}

// La polynya per lo spettatore all'istante t, dai soli record X: serve anche nel replay da file, senza la partita. Stessa fase
// del campo (CampoGhiaccio::forza_zona, Missione): la durata dell'intervento comprende la transizione, allo scadere comincia
// il ritorno, che dura ancora una transizione dopo il record della fine. Il giocatore non la vede: sa solo la volta che misura
struct PolynyaVista {
    Vec2 centro;
    double raggio_m = 0.0;   // la zona intera; aperta fino a raggio_m x fase
    bool apri = true;
    double fase = 0.0;       // 0 spenta, 1 piena
    bool ritorno = false;    // l'intervento e' scaduto e il ghiaccio torna com'era
};

// Il pack sotto le zone forzate: la griglia nel punto a cui il bordo e' arrivato (CampoGhiaccio::copertura_in, senza le zone)
inline double pack_senza_zone(const GrigliaGhiaccio& griglia, const CampoGhiaccio& c, Vec2 p, double t) {
    const Vec2 s = c.spostamento(t), d = c.parametri().deriva_m_s;
    return griglia.in({p.x - (s.x - d.x * t), p.y - (s.y - d.y * t)});
}

// La zona del campo da un record della polynya
inline ZonaGhiaccio zona_polynya(const RecordIntervento& x) {
    const auto chiavi = leggi_chiavi(x.chiavi);
    const auto it = chiavi.find("#transizione_s");
    const auto azione = chiavi.find("azione");
    ZonaGhiaccio z;
    z.centro = x.punto;
    z.raggio_m = x.raggio_m;
    z.apri = azione == chiavi.end() || azione->second != "chiudi";
    z.t0 = x.t;
    z.transizione_s = it == chiavi.end() ? 0.0 : std::strtod(it->second.c_str(), nullptr);
    z.durata_s = x.durata_s > 0.0 ? std::max(x.durata_s - z.transizione_s, 1.0) : 0.0;
    return z;
}

inline std::vector<PolynyaVista> polynye_a(const std::vector<RecordIntervento>& interventi, double t) {
    std::vector<PolynyaVista> v;
    for (const RecordIntervento& x : interventi) {
        if (x.tipo != "polynya" || x.fase != RecordIntervento::FASE_APPLICATO || !x.ha_punto) continue;
        const ZonaGhiaccio z = zona_polynya(x);
        const double fase = fase_cambiamento(t, z.t0, z.transizione_s, z.durata_s);
        if (fase > 0.0) v.push_back({z.centro, z.raggio_m, z.apri, fase, z.durata_s > 0.0 && t > z.t0 + z.transizione_s + z.durata_s});
    }
    return v;
}

// Il campo per la carta dello spettatore: il clima della griglia, con la deriva e le polynye dei record X. Ne conta solo la
// copertura (copertura_in), non le celle: quelle dipendono dal seme, e sulla carta non si vedono. Alla fine di una deriva, come
// la missione: quella dell'ultimo intervento ancora attivo, se no quella dei parametri
inline CampoGhiaccio campo_spettatore(std::shared_ptr<const GrigliaGhiaccio> griglia, const std::vector<RecordIntervento>& interventi) {
    CampoGhiaccio g(0, ParametriGhiaccio{}, [griglia](Vec2 p) { return griglia->in(p); });
    std::vector<const RecordIntervento*> derive;
    for (const RecordIntervento& x : interventi) {
        if (x.tipo == "ghiaccio_deriva") derive.push_back(&x);
        else if (x.tipo == "polynya" && x.fase == RecordIntervento::FASE_APPLICATO && x.ha_punto) g.forza_zona(zona_polynya(x));
    }
    std::stable_sort(derive.begin(), derive.end(), [](const RecordIntervento* a, const RecordIntervento* b) { return a->t < b->t; });
    std::map<int, Vec2> attive;
    for (const RecordIntervento* x : derive) {
        if (x->fase == RecordIntervento::FASE_APPLICATO) {
            const auto chiavi = leggi_chiavi(x->chiavi);
            const auto numero = [&](const char* k) { const auto it = chiavi.find(k); return it == chiavi.end() ? 0.0 : std::strtod(it->second.c_str(), nullptr); };
            const double r = radianti(numero("#rotta")), v = numero("#velocita_kn") * NODO;
            attive[x->id] = {v * std::sin(r), v * std::cos(r)};
            g.cambia_deriva(x->t, attive[x->id]);
        } else if (x->fase == RecordIntervento::FASE_FINITO) {
            attive.erase(x->id);
            g.cambia_deriva(x->t, attive.empty() ? g.parametri().deriva_m_s : attive.rbegin()->second);
        }
    }
    return g;
}

}  // namespace m688
