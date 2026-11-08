#include "nucleo/albo.hpp"

#include <algorithm>
#include <fstream>
#include <map>

#include <toml.hpp>

namespace m688 {

namespace {

// albo.toml: il comandante in carica per marina e i codici delle rapide gia' contati per comandante
struct FileAlbo {
    std::map<std::string, std::string> in_carica;
    std::map<std::string, std::vector<std::string>> codici;
};

FileAlbo leggi_file_albo() {
    FileAlbo out;
    try {
        const toml::table t = toml::parse_file(percorso_albo().string());
        if (const auto* c = t["in_carica"].as_table())
            for (const auto& [k, v] : *c)
                if (const auto s = v.value<std::string>()) out.in_carica[std::string(k.str())] = *s;
        if (const auto* c = t["codici"].as_table())
            for (const auto& [k, v] : *c)
                if (const auto* a = v.as_array())
                    for (const auto& x : *a)
                        if (const auto s = x.value<std::string>()) out.codici[std::string(k.str())].push_back(*s);
    } catch (const std::exception&) {
    }
    return out;
}

std::map<std::string, std::string> leggi_in_carica() { return leggi_file_albo().in_carica; }

void scrivi_file_albo(const FileAlbo& a) {
    std::filesystem::create_directories(percorso_albo().parent_path());
    toml::table c;
    for (const auto& [k, v] : a.in_carica) c.insert(k, v);
    toml::table codici;
    for (const auto& [k, v] : a.codici) {
        toml::array x;
        for (const auto& s : v) x.push_back(s);
        codici.insert(k, x);
    }
    toml::table t;
    t.insert("versione", 1);
    t.insert("in_carica", c);
    t.insert("codici", codici);
    const auto tmp = percorso_albo().string() + ".tmp";
    {
        std::ofstream f(tmp);
        if (!f) throw std::runtime_error("impossibile scrivere " + tmp);
        f << "# albo dei comandanti: il comandante in carica per marina e i codici delle rapide gia' contati\n" << t << "\n";
    }
    std::filesystem::rename(tmp, percorso_albo());
}

}  // namespace

std::filesystem::path percorso_albo() { return cartella_fascicoli().parent_path() / "albo.toml"; }

std::vector<VoceAlbo> leggi_albo() {
    const auto carica = leggi_in_carica();
    std::vector<VoceAlbo> out;
    for (const auto& id : elenco_fascicoli()) {
        if (static_cast<int>(out.size()) >= ALBO_MAX) break;
        try {
            VoceAlbo v;
            v.f = leggi_fascicolo(percorso_fascicolo(id));
            for (const auto& m : v.f.missioni) v.punti += m.punti;
            const auto it = carica.find(std::string(nome_marina(v.f.marina)));
            v.in_carica = it != carica.end() && it->second == v.f.id && caricabile(v.f);
            out.push_back(std::move(v));
        } catch (const std::exception&) {   // un fascicolo illeggibile non ferma l'albo
        }
    }
    return out;
}

std::optional<std::string> comandante_in_carica(Marina m) {
    const auto carica = leggi_in_carica();
    const auto it = carica.find(std::string(nome_marina(m)));
    if (it == carica.end()) return std::nullopt;
    try {
        const Fascicolo f = leggi_fascicolo(percorso_fascicolo(it->second));
        if (f.marina == m && caricabile(f)) return f.id;
    } catch (const std::exception&) {
    }
    return std::nullopt;
}

void imposta_in_carica(Marina m, const std::string& id) {
    FileAlbo a = leggi_file_albo();
    a.in_carica[std::string(nome_marina(m))] = id;
    scrivi_file_albo(a);
}

bool codice_giocato(const std::string& id, const std::string& codice) {
    const FileAlbo a = leggi_file_albo();
    const auto it = a.codici.find(id);
    return it != a.codici.end() && std::find(it->second.begin(), it->second.end(), codice) != it->second.end();
}

void registra_codice(const std::string& id, const std::string& codice) {
    FileAlbo a = leggi_file_albo();
    auto& v = a.codici[id];
    if (std::find(v.begin(), v.end(), codice) == v.end()) v.push_back(codice);
    scrivi_file_albo(a);
}

bool caricabile(const Fascicolo& f) { return f.in_servizio(); }

std::string id_libero(const std::string& id) {
    if (!std::filesystem::exists(percorso_fascicolo(id))) return id;
    for (int k = 2;; ++k)
        if (const std::string x = id + "_" + std::to_string(k); !std::filesystem::exists(percorso_fascicolo(x))) return x;
}

void elimina_dall_albo(const std::string& id) {
    std::error_code ec;
    std::filesystem::remove(percorso_fascicolo(id), ec);
    FileAlbo a = leggi_file_albo();
    bool cambiato = a.codici.erase(id) > 0;
    for (auto it = a.in_carica.begin(); it != a.in_carica.end();)
        if (it->second == id) it = a.in_carica.erase(it), cambiato = true;
        else ++it;
    if (cambiato) scrivi_file_albo(a);
}

}  // namespace m688
