#include "nucleo/achievement.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include <toml.hpp>

#include "nucleo/albo.hpp"

namespace m688 {

namespace {

AmbitoAchievement ambito_da_nome(const std::string& s, const std::string& id) {
    if (s == "missione") return AmbitoAchievement::missione;
    if (s == "somma") return AmbitoAchievement::somma;
    if (s == "carriera") return AmbitoAchievement::carriera;
    throw std::runtime_error("achievement '" + id + "': ambito sconosciuto '" + s + "'");
}

bool confronta(double a, const std::string& op, double b) {
    if (op == ">=") return a >= b - 1e-9;
    if (op == ">") return a > b + 1e-9;
    if (op == "<=") return a <= b + 1e-9;
    if (op == "<") return a < b - 1e-9;
    if (op == "==") return std::abs(a - b) <= 1e-9;
    if (op == "!=") return std::abs(a - b) > 1e-9;
    return false;
}

bool vale(const CondizioneAchievement& c, const Misure& m) {
    if (!c.testo.empty()) {
        const auto it = m.chiavi.find(c.misura);
        const bool uguale = it != m.chiavi.end() && it->second == c.testo;
        return c.op == "==" ? uguale : !uguale;
    }
    return confronta(m[c.misura], c.op, c.valore);
}

}  // namespace

const std::set<std::string>& misure_achievement_note() {
    static const std::set<std::string> s = {
        "riuscita", "completezza", "successo", "battello_perso", "avversario", "affondati_ostili",
        "sottomarini_affondati", "navi_affondate", "doppietta", "lanci", "lanci_a_segno", "tutti_a_segno",
        "siluri_evitati", "colpi_subiti", "distanza_min_siluro_m", "mine_posate", "mine_inerti", "fantasma",
        "trasmissioni", "serie_copiate", "serie_perse", "rapporti_mancati", "avvistamenti", "boe_vicine", "ping_attivi",
        "sottomarino_vicino", "esposizione_periscopio_s", "quasi_collisione_ignota", "classificazioni_giuste",
        "classificazioni_errate", "anticipo_medio_s", "danni_domati", "margine_collasso", "foto_periscopio", "ordini",
        "ordini_mancati", "ordini_rispettati", "incaglio", "armi_a_bordo", "originale", "rapida", "editor", "libreria",
        "artico", "pace", "crisi", "guerra", "usn", "vmf", "realismo", "qualifica_sommergibilista", "grado_ordine",
        "decorazioni_personali", "decorazione_massima", "missioni_carriera", "pensione", "uno",
        // docs/26 §2 (29/10/2026; achievement_missione.cpp, tools/pipeline/achievement.py)
        "visive_giuste", "telemetro_entro_5", "aob_entro_10", "prima_la_vista", "affondati_missile_oth",
        "salve_neutrale_illeso", "siluri_evitati_allarme", "sotto_chiglia_20_s", "navigazione_incerta", "gps",
        "errore_finale_m", "non_autentici_respinti", "campi_minati_attraversati", "mare_max", "colpi_artiglieria_subiti",
        "righe_nuove_acint",
        // secondo giro (31/10/2026): ghiaccio, notte, soccorso, pedinamento, mine, elicotteri, VDS, libreria storica
        "emersioni_ghiaccio", "urti_ghiaccio", "sotto_banchisa_s", "visive_fanali_giuste", "visive_luci_spente",
        "tutti_salvati", "in_coda_s", "entro_500yd_s", "collisioni", "mine_scoperte", "mine_aggirate",
        "sorvoli_elicottero", "navi_traino_vicine", "libreria_storica"};
    return s;
}

const std::set<std::string>& chiavi_achievement_note() {
    // classi_giuste ha piu' valori, separati da '|' (vale solo per `distinti`)
    static const std::set<std::string> s = {"missione", "teatro", "marina", "codice", "classi_giuste"};
    return s;
}

int DefAchievement::livello(double v) const {
    int k = 0;
    for (const double soglia : livelli) k += v >= soglia - 1e-9;
    return k == 0 ? 0 : 3 - static_cast<int>(livelli.size()) + k;   // una soglia sola: l'oro
}

const DefAchievement* CatalogoAchievement::cerca(const std::string& id) const {
    const auto it = std::find_if(achievement.begin(), achievement.end(), [&](const DefAchievement& d) { return d.id == id; });
    return it == achievement.end() ? nullptr : &*it;
}

CatalogoAchievement carica_achievement(const std::string& percorso) {
    const toml::table t = toml::parse_file(percorso);
    CatalogoAchievement out;
    const auto* tab = t["achievement"].as_table();
    if (!tab) throw std::runtime_error(percorso + ": senza [achievement]");
    // l'ordine del file: toml++ tiene le chiavi ordinate, quindi si rilegge l'ordine dalle posizioni d'origine
    std::vector<std::pair<std::size_t, std::string>> ordine;
    for (const auto& [k, v] : *tab) ordine.emplace_back(v.source().begin.line, std::string(k.str()));
    std::sort(ordine.begin(), ordine.end());
    for (const auto& [riga, id] : ordine) {
        (void)riga;
        const toml::table& r = *(*tab)[id].as_table();
        DefAchievement d;
        d.id = id;
        d.nome_it = r["nome_it"].value_or(id);
        d.nome_en = r["nome_en"].value_or(d.nome_it);
        d.descrizione_it = r["descrizione_it"].value_or(std::string{});
        d.descrizione_en = r["descrizione_en"].value_or(std::string{});
        d.categoria = r["categoria"].value_or(std::string{});
        d.ambito = ambito_da_nome(r["ambito"].value_or(std::string{}), id);
        d.misura = r["misura"].value_or(std::string{"uno"});
        d.distinti = r["distinti"].value_or(std::string{});
        d.icona = r["icona"].value_or(id);
        d.nascosto = r["nascosto"].value_or(false);
        if (const auto* a = r["livelli"].as_array())
            for (const auto& x : *a) d.livelli.push_back(x.value<double>().value_or(0.0));
        if (d.livelli.size() != 1 && d.livelli.size() != 3)
            throw std::runtime_error(percorso + ": achievement '" + id + "' con " + std::to_string(d.livelli.size()) + " livelli");
        if (!misure_achievement_note().count(d.misura))
            throw std::runtime_error(percorso + ": achievement '" + id + "' con la misura sconosciuta '" + d.misura + "'");
        if (!d.distinti.empty() && !chiavi_achievement_note().count(d.distinti))
            throw std::runtime_error(percorso + ": achievement '" + id + "' con la chiave sconosciuta '" + d.distinti + "'");
        if (const auto* a = r["condizioni"].as_array())
            for (const auto& x : *a) {
                const auto* c = x.as_array();
                if (!c || c->size() != 3) throw std::runtime_error(percorso + ": achievement '" + id + "': condizione malformata");
                CondizioneAchievement k;
                k.misura = (*c)[0].value_or(std::string{});
                k.op = (*c)[1].value_or(std::string{});
                if (const auto s = (*c)[2].value<std::string>()) k.testo = *s;
                else k.valore = (*c)[2].value<double>().value_or(0.0);
                const bool chiave = !k.testo.empty();
                if (chiave ? !chiavi_achievement_note().count(k.misura) : !misure_achievement_note().count(k.misura))
                    throw std::runtime_error(percorso + ": achievement '" + id + "': misura sconosciuta '" + k.misura + "'");
                static const std::set<std::string> OP = {">=", ">", "<=", "<", "==", "!="};
                if (!OP.count(k.op) || (chiave && k.op != "==" && k.op != "!="))
                    throw std::runtime_error(percorso + ": achievement '" + id + "': operatore '" + k.op + "'");
                d.condizioni.push_back(std::move(k));
            }
        out.achievement.push_back(std::move(d));
    }
    return out;
}

int StatoAchievement::livello(const std::string& id) const {
    const auto it = p.find(id);
    return it == p.end() ? 0 : it->second.livello;
}

double StatoAchievement::avanzamento(const DefAchievement& d) const {
    const auto it = p.find(d.id);
    if (it == p.end()) return 0.0;
    return d.distinti.empty() ? it->second.somma : static_cast<double>(it->second.chiavi.size());
}

std::vector<Sblocco> valuta_achievement(const CatalogoAchievement& c, const Misure& m, StatoAchievement& s,
                                        const std::string& data, const std::string& missione) {
    std::vector<Sblocco> out;
    for (const DefAchievement& d : c.achievement) {
        if (!std::all_of(d.condizioni.begin(), d.condizioni.end(), [&](const CondizioneAchievement& k) { return vale(k, m); }))
            continue;
        ProgressoAchievement& p = s.p[d.id];
        double v = m[d.misura];
        if (d.ambito == AmbitoAchievement::somma) {
            if (d.distinti.empty()) {
                p.somma += v;
                v = p.somma;
            } else if (const auto k = m.chiavi.find(d.distinti); k != m.chiavi.end() && !k->second.empty()) {
                for (std::size_t a = 0; a <= k->second.size();) {   // i valori separati da '|'
                    const std::size_t b = std::min(k->second.find('|', a), k->second.size());
                    if (b > a) p.chiavi.insert(k->second.substr(a, b - a));
                    a = b + 1;
                }
                v = static_cast<double>(p.chiavi.size());
            } else {
                v = static_cast<double>(p.chiavi.size());
            }
        }
        int l = d.livello(v);
        if (d.ambito == AmbitoAchievement::carriera)   // quello che il fascicolo aveva gia' prima non si guadagna qui
            if (const auto b = m.prima.find(d.misura); b != m.prima.end() && d.livello(b->second) >= l) l = 0;
        if (l > p.livello) {
            out.push_back({d.id, l, p.livello});
            p.livello = l;
            p.data = data;
            p.missione = missione;
        }
        if (p.livello == 0 && p.somma == 0.0 && p.chiavi.empty()) s.p.erase(d.id);   // niente da ricordare
    }
    return out;
}

// --- file ---

std::filesystem::path percorso_achievement() { return percorso_albo().parent_path() / "achievement.toml"; }

namespace {

StatoAchievement leggi_stato(const toml::table& t) {
    StatoAchievement s;
    for (const auto& [k, v] : t) {
        const auto* r = v.as_table();
        if (!r) continue;
        ProgressoAchievement p;
        p.livello = (*r)["livello"].value_or(0);
        p.somma = (*r)["somma"].value_or(0.0);
        p.data = (*r)["data"].value_or(std::string{});
        p.missione = (*r)["missione"].value_or(std::string{});
        if (const auto* a = (*r)["chiavi"].as_array())
            for (const auto& x : *a)
                if (const auto c = x.value<std::string>()) p.chiavi.insert(*c);
        s.p[std::string(k.str())] = std::move(p);
    }
    return s;
}

toml::table scrivi_stato(const StatoAchievement& s) {
    toml::table t;
    for (const auto& [id, p] : s.p) {
        toml::table r;
        r.insert("livello", p.livello);
        if (p.somma != 0.0) r.insert("somma", p.somma);
        if (!p.chiavi.empty()) {
            toml::array a;
            for (const auto& c : p.chiavi) a.push_back(c);
            r.insert("chiavi", a);
        }
        if (!p.data.empty()) r.insert("data", p.data);
        if (!p.missione.empty()) r.insert("missione", p.missione);
        t.insert(id, r);
    }
    return t;
}

}  // namespace

FileAchievement leggi_achievement(const std::filesystem::path& p) {
    FileAchievement out;
    if (!std::filesystem::exists(p)) return out;
    try {
        const toml::table t = toml::parse_file(p.string());
        if (const auto* g = t["globali"].as_table()) out.globali = leggi_stato(*g);
        if (const auto* c = t["comandanti"].as_table())
            for (const auto& [k, v] : *c)
                if (const auto* r = v.as_table()) out.comandanti[std::string(k.str())] = leggi_stato(*r);
    } catch (const std::exception&) {   // un file rovinato non ferma la partita: si riparte da zero
        return {};
    }
    return out;
}

void salva_achievement(const FileAchievement& f, const std::filesystem::path& p) {
    std::filesystem::create_directories(p.parent_path());
    toml::table c;
    for (const auto& [id, s] : f.comandanti) c.insert(id, scrivi_stato(s));
    toml::table t;
    t.insert("versione", 1);
    t.insert("globali", scrivi_stato(f.globali));
    t.insert("comandanti", c);
    const auto tmp = p.string() + ".tmp";
    {
        std::ofstream o(tmp);
        if (!o) throw std::runtime_error("impossibile scrivere " + tmp);
        o << "# achievement: i livelli raggiunti, globali e per comandante (id del fascicolo)\n" << t << "\n";
    }
    std::filesystem::rename(tmp, p);
}

EsitoAchievement registra_achievement(const CatalogoAchievement& c, const Misure& m, const std::string& comandante,
                                      bool conta_comandante, bool ripresa, const std::string& data,
                                      const std::string& missione, const std::filesystem::path& p) {
    EsitoAchievement e;
    if (ripresa) return e;
    FileAchievement f = leggi_achievement(p);
    e.globali = valuta_achievement(c, m, f.globali, data, missione);
    if (conta_comandante && !comandante.empty()) e.comandante = valuta_achievement(c, m, f.comandanti[comandante], data, missione);
    salva_achievement(f, p);
    return e;
}

}  // namespace m688
