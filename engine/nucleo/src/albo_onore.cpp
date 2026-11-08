#include "nucleo/albo_onore.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include <toml.hpp>

#include "nucleo/albo.hpp"

namespace m688 {

namespace {

constexpr double SENZA_AIUTI = 1.0 - 1e-9;   // realismo 1: nessun aiuto acceso

template <class T>
int inserisci(std::vector<T>& v, const T& x, double (*punti)(const T&), int massimo) {
    // dopo le voci con punteggio maggiore o uguale: a parita' resta prima la piu' vecchia
    const auto it = std::find_if(v.begin(), v.end(), [&](const T& y) { return punti(y) < punti(x); });
    const int pos = static_cast<int>(it - v.begin());
    if (pos >= massimo) return 0;
    v.insert(it, x);
    if (static_cast<int>(v.size()) > massimo) v.resize(static_cast<std::size_t>(massimo));
    return pos + 1;
}

double punti_missione(const VoceOnoreMissione& v) { return v.punteggio; }
double punti_carriera(const VoceOnoreCarriera& v) { return v.punti; }

std::string s(const toml::table& t, const char* k) { return t[k].value_or(std::string{}); }

}  // namespace

std::filesystem::path percorso_albo_onore() { return percorso_albo().parent_path() / "albo_onore.toml"; }

AlboOnore leggi_albo_onore(const std::filesystem::path& p) {
    AlboOnore a;
    if (!std::filesystem::exists(p)) return a;
    try {
        const toml::table t = toml::parse_file(p.string());
        if (const auto* m = t["missioni"].as_array())
            for (const auto& n : *m)
                if (const auto* r = n.as_table()) {
                    VoceOnoreMissione v;
                    v.punteggio = (*r)["punteggio"].value_or(0.0);
                    v.missione = s(*r, "missione");
                    v.origine = s(*r, "origine");
                    v.codice = s(*r, "codice");
                    v.data = s(*r, "data");
                    v.data_reale = s(*r, "data_reale");
                    v.comandante = s(*r, "comandante");
                    v.nome = s(*r, "nome");
                    v.marina = s(*r, "marina");
                    v.grado = s(*r, "grado");
                    a.missioni.push_back(std::move(v));
                }
        if (const auto* c = t["carriere"].as_array())
            for (const auto& n : *c)
                if (const auto* r = n.as_table()) {
                    VoceOnoreCarriera v;
                    v.punti = (*r)["punti"].value_or(0.0);
                    v.missioni = (*r)["missioni"].value_or(0);
                    v.comandante = s(*r, "comandante");
                    v.nome = s(*r, "nome");
                    v.marina = s(*r, "marina");
                    v.grado = s(*r, "grado");
                    v.decorazione = s(*r, "decorazione");
                    v.stato = s(*r, "stato");
                    v.data = s(*r, "data");
                    a.carriere.push_back(std::move(v));
                }
    } catch (const std::exception&) {   // un file rovinato non ferma il gioco
        return {};
    }
    return a;
}

void salva_albo_onore(const AlboOnore& a, const std::filesystem::path& p) {
    std::filesystem::create_directories(p.parent_path());
    toml::array m, c;
    for (const auto& v : a.missioni)
        m.push_back(toml::table{{"punteggio", v.punteggio}, {"missione", v.missione}, {"origine", v.origine},
                                {"codice", v.codice}, {"data", v.data}, {"data_reale", v.data_reale},
                                {"comandante", v.comandante}, {"nome", v.nome}, {"marina", v.marina}, {"grado", v.grado}});
    for (const auto& v : a.carriere)
        c.push_back(toml::table{{"punti", v.punti}, {"missioni", v.missioni}, {"comandante", v.comandante},
                                {"nome", v.nome}, {"marina", v.marina}, {"grado", v.grado},
                                {"decorazione", v.decorazione}, {"stato", v.stato}, {"data", v.data}});
    toml::table t;
    t.insert("versione", 1);
    t.insert("missioni", m);
    t.insert("carriere", c);
    const auto tmp = p.string() + ".tmp";
    {
        std::ofstream o(tmp);
        if (!o) throw std::runtime_error("impossibile scrivere " + tmp);
        o << "# albo d'onore: le migliori missioni (senza aiuti) e le migliori carriere chiuse\n" << t << "\n";
    }
    std::filesystem::rename(tmp, p);
}

int proponi_missione(AlboOnore& a, const VoceOnoreMissione& v, bool ripresa, double realismo) {
    if (ripresa || realismo < SENZA_AIUTI) return 0;
    return inserisci(a.missioni, v, &punti_missione, ONORE_MISSIONI);
}

int proponi_carriera(AlboOnore& a, const Fascicolo& f, const RegoleCarriera* r) {
    if (f.in_servizio()) return 0;
    std::erase_if(a.carriere, [&](const VoceOnoreCarriera& v) { return v.comandante == f.id; });
    VoceOnoreCarriera v;
    for (const auto& m : f.missioni) v.punti += m.punti;
    v.missioni = static_cast<int>(f.missioni.size());
    v.comandante = f.id;
    v.nome = f.nome;
    v.marina = std::string(nome_marina(f.marina));
    v.grado = f.grado;
    v.stato = f.stato;
    v.data = f.data;
    int precedenza = 1 << 30;
    for (const auto& k : f.decorazioni) {
        const Decorazione* d = r ? r->decorazione(k.decorazione) : nullptr;
        const int p = d ? d->precedenza : 99;
        if (d && d->tipo == "distintivo") continue;   // dolphins e command at sea non sono decorazioni d'onore
        if (p < precedenza) precedenza = p, v.decorazione = d ? d->nome : k.decorazione;
    }
    return inserisci(a.carriere, v, &punti_carriera, ONORE_CARRIERE);
}

int registra_missione_onore(const VoceOnoreMissione& v, bool ripresa, double realismo, const std::filesystem::path& p) {
    if (ripresa || realismo < SENZA_AIUTI) return 0;
    AlboOnore a = leggi_albo_onore(p);
    const int pos = proponi_missione(a, v, ripresa, realismo);
    if (pos) salva_albo_onore(a, p);
    return pos;
}

int registra_carriera_onore(const Fascicolo& f, const RegoleCarriera* r, const std::filesystem::path& p) {
    if (f.in_servizio()) return 0;
    AlboOnore a = leggi_albo_onore(p);
    const auto prima = a.carriere.size();
    const int pos = proponi_carriera(a, f, r);
    if (pos || a.carriere.size() != prima) salva_albo_onore(a, p);   // anche una voce tolta perche' scesa fuori
    return pos;
}

std::vector<ImpresaStorica> carica_storici(const std::string& percorso) {
    const toml::table t = toml::parse_file(percorso);
    std::vector<ImpresaStorica> out;
    if (const auto* tab = t["storici"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) {
                ImpresaStorica i;
                i.id = std::string(k.str());
                i.marina = s(*r, "marina");
                i.anno = (*r)["anno"].value_or(0);
                i.battello = s(*r, "battello");
                i.comandante = s(*r, "comandante");
                i.impresa_it = s(*r, "impresa_it");
                i.impresa_en = s(*r, "impresa_en");
                i.decorazione_it = s(*r, "decorazione_it");
                i.fonte = s(*r, "fonte");
                i.confidenza = s(*r, "confidenza");
                out.push_back(std::move(i));
            }
    std::stable_sort(out.begin(), out.end(), [](const ImpresaStorica& a, const ImpresaStorica& b) { return a.anno < b.anno; });
    return out;
}

}  // namespace m688
