#include "missioni/pacchetto_missione.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>

#include "lua_isolato.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/fondale.hpp"
#include "sha256.hpp"
#include "testo_lua.hpp"

namespace m688 {

namespace {

using testo_lua::esatto;
using testo_lua::stringa;

const std::string ZERI(64, '0');
constexpr const char* INIZIO = "  pacchetto = {";

std::string leggi_file(const std::filesystem::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + p.string());
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

std::string breve(const std::string& s) { return sha256::esadecimale(s).substr(0, 16); }

std::vector<std::string> classi_di(const ScenarioMissione& s) {
    std::set<std::string> v;
    for (const auto& u : s.unita) v.insert(u.giocatore ? "giocatore_" + u.classe : u.classe);
    return {v.begin(), v.end()};
}

// Le impronte delle dipendenze: il fondale del teatro, i modelli classici delle classi, i kit del giocatore
std::string impronta_teatro(const std::string& cartella, const std::string& teatro) {
    return breve(leggi_file(cartella + "/teatri/" + teatro + ".fdl"));
}
std::string impronta_classi(const std::vector<std::string>& classi, const DatiScenario& d) {
    std::string t;
    for (const auto& c : classi) {
        const auto it = d.modelli.find(c);
        if (it == d.modelli.end()) continue;
        const auto& m = it->second;
        t += c + ":" + esatto(m.massima) + "," + esatto(m.virata) + "," + esatto(m.accel) + "," + esatto(m.quota_m_s) + "," +
             esatto(m.quota_max) + "," + std::to_string(m.integrita) + "," + std::to_string(m.rumore) + "," + std::to_string(m.sonar) +
             "," + (m.difesa ? std::to_string(*m.difesa) : "-") + "," + (m.schiera ? "1" : "0") + (m.fondale ? "1" : "0") + "," +
             std::to_string(m.periodo) + ";";
        const std::string base = c.rfind("giocatore_", 0) == 0 ? c.substr(10) : c;
        if (const auto p = d.pescaggi.find(base); p != d.pescaggi.end()) t += esatto(p->second) + ";";
    }
    return breve(t);
}
std::string impronta_kit(const DatiScenario& d) {
    return breve(d.kit[0].gestori + d.kit[0].armi + d.kit[0].script + d.kit[1].gestori + d.kit[1].armi + d.kit[1].script);
}

// Il testo senza il valore dell'impronta (azzerato) e il valore; nullopt se non e' un pacchetto
struct Impronta {
    std::string azzerato, valore;
};
std::optional<Impronta> impronta_di(const std::string& testo) {
    const auto riga = testo.find(std::string("\n") + INIZIO);
    if (riga == std::string::npos) return std::nullopt;
    const auto fine_riga = testo.find('\n', riga + 1);
    const std::string chiave = "impronta = \"sha256:";
    const auto k = testo.find(chiave, riga);
    if (k == std::string::npos || k > fine_riga || k + chiave.size() + 64 > testo.size()) return std::nullopt;
    Impronta i;
    i.valore = testo.substr(k + chiave.size(), 64);
    i.azzerato = testo;
    i.azzerato.replace(k + chiave.size(), 64, ZERI);
    return i;
}

// Il file della missione senza la riga del pacchetto
std::string senza_pacchetto(const std::string& testo) {
    const auto riga = testo.find(std::string("\n") + INIZIO);
    if (riga == std::string::npos) return testo;
    const auto fine = testo.find('\n', riga + 1);
    return testo.substr(0, riga + 1) + testo.substr(fine + 1);
}

std::set<std::string> funzioni_m688(const std::string& testo) {
    static const std::regex chiamata(R"(m688\.([A-Za-z_][A-Za-z0-9_]*))");
    std::set<std::string> v;
    for (auto it = std::sregex_iterator(testo.begin(), testo.end(), chiamata); it != std::sregex_iterator(); ++it) v.insert((*it)[1]);
    return v;
}

bool id_valido(const std::string& id) {
    if (id.empty() || id.size() > 64 || id.rfind("SCENE", 0) == 0 || id.rfind("RAPIDA_", 0) == 0) return false;
    return std::all_of(id.begin(), id.end(), [](char c) { return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'; });
}

struct Pacchetto {
    int formato_pacchetto = 0, formato_missione = 0, formato_editor = 0, libreria = 0;
    std::string autore, creato, teatro, impronta_teatro, impronta_classi, impronta_kit;
    std::vector<std::string> classi;
};

std::string campo(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    std::string v = lua_type(L, -1) == LUA_TSTRING ? lua_tostring(L, -1) : "";
    lua_pop(L, 1);
    return v;
}
int numero(lua_State* L, int t, const char* k) {
    lua_getfield(L, t, k);
    const int v = lua_isinteger(L, -1) ? static_cast<int>(lua_tointeger(L, -1)) : 0;
    lua_pop(L, 1);
    return v;
}

std::optional<Pacchetto> leggi_pacchetto(const std::string& testo) {
    lua_isolato::Stato stato;
    lua_State* L = stato.get();
    stato.esegui(testo, "=pacchetto");
    if (!lua_istable(L, -1)) return std::nullopt;
    lua_getfield(L, -1, "pacchetto");
    if (!lua_istable(L, -1)) return std::nullopt;
    const int p = lua_gettop(L);
    Pacchetto x;
    x.formato_pacchetto = numero(L, p, "formato_pacchetto");
    x.formato_missione = numero(L, p, "formato_missione");
    x.formato_editor = numero(L, p, "formato_editor");
    x.libreria = numero(L, p, "libreria");
    x.autore = campo(L, p, "autore");
    x.creato = campo(L, p, "creato");
    lua_getfield(L, p, "dipendenze");
    if (lua_istable(L, -1)) {
        const int d = lua_gettop(L);
        x.teatro = campo(L, d, "teatro");
        x.impronta_teatro = campo(L, d, "impronta_teatro");
        x.impronta_classi = campo(L, d, "impronta_classi");
        x.impronta_kit = campo(L, d, "impronta_kit");
        lua_getfield(L, d, "classi");
        if (lua_istable(L, -1))
            for (lua_Integer i = 1; lua_rawgeti(L, -1, i) == LUA_TSTRING; ++i) {
                x.classi.push_back(lua_tostring(L, -1));
                lua_pop(L, 1);
            }
    }
    return x;
}

// Funzioni che le regole compilate possono usare oltre a quelle del file rigenerato
const std::set<std::string> FUNZIONI_REGOLE = {"seleziona", "posizione", "rotta_ora", "velocita_ora", "cue_asw", "nessun_comportamento",
                                               "comportamento", "vai_a", "salva_modello", "messaggio", "variabile", "variabile_piu",
                                               "variabile_meno", "variabile_uguale", "variabile_maggiore", "variabile_minore",
                                               "unita_colpita", "unita_esiste", "in_rettangolo", "dopo"};

}  // namespace

std::string esporta_pacchetto(const ScenarioMissione& s, const DatiScenario& d, const std::string& cartella, const std::string& creato) {
    const std::string missione = scrivi_missione_lua(s, d);
    const auto classi = classi_di(s);
    std::string blocco = std::string(INIZIO) + " formato_pacchetto = " + std::to_string(FORMATO_PACCHETTO) + ", formato_missione = 1, formato_editor = " +
                         std::to_string(FORMATO_EDITOR) + ", libreria = " + std::to_string(s.libreria) + ", autore = " + stringa(s.autore) +
                         ", creato = " + stringa(creato) + ",\n";
    // la riga del pacchetto e' una sola: le dipendenze sulla stessa riga
    blocco.pop_back();
    blocco += " dipendenze = { teatro = " + stringa(s.teatro) + ", impronta_teatro = " + stringa(impronta_teatro(cartella, s.teatro)) + ", classi = { ";
    for (const auto& c : classi) blocco += stringa(c) + ", ";
    blocco += "}, impronta_classi = " + stringa(impronta_classi(classi, d)) + ", impronta_kit = " + stringa(impronta_kit(d)) +
              " }, impronta = \"sha256:" + ZERI + "\" },\n";
    const std::string formato = "  formato = 1,\n";
    const auto k = missione.find(formato);
    if (k == std::string::npos) throw std::logic_error("pacchetto: il file della missione non ha il formato");
    std::string t = missione.substr(0, k + formato.size()) + blocco + missione.substr(k + formato.size());
    const std::string h = sha256::esadecimale(t);
    const auto z = t.find("sha256:" + ZERI);
    t.replace(z + 7, 64, h);
    return t;
}

EsitoImportazione importa_pacchetto(const std::string& testo, const DatiScenario& d, const std::string& cartella,
                                    const std::filesystem::path& destinazione) {
    EsitoImportazione e;
    const auto no = [&](const std::string& perche) {
        e.problemi.push_back(perche);
        e.importato = false;
        return e;
    };
    // 1. l'impronta, prima di eseguire qualunque cosa
    const auto imp = impronta_di(testo);
    if (!imp) return no("non e' un pacchetto dell'editor: manca il blocco 'pacchetto'");
    if (sha256::esadecimale(imp->azzerato) != imp->valore) return no("impronta sbagliata: il file e' rovinato o e' stato modificato");
    // 2. i formati
    std::optional<Pacchetto> p;
    try {
        p = leggi_pacchetto(testo);
    } catch (const std::exception& x) {
        return no(std::string("il pacchetto non si carica: ") + x.what());
    }
    if (!p) return no("non e' un pacchetto dell'editor: manca il blocco 'pacchetto'");
    if (p->formato_pacchetto < 1 || p->formato_pacchetto > FORMATO_PACCHETTO)
        return no("formato del pacchetto " + std::to_string(p->formato_pacchetto) + ": serve una versione piu' nuova del gioco");
    if (p->formato_missione != 1) return no("formato della missione " + std::to_string(p->formato_missione) + " sconosciuto");
    if (p->formato_editor > FORMATO_EDITOR) return no("formato dell'editor " + std::to_string(p->formato_editor) + ": serve una versione piu' nuova del gioco");
    e.autore = p->autore;
    // 3. lo scenario, nell'ambiente isolato
    std::optional<ScenarioMissione> s;
    try {
        s = leggi_scenario(testo);
    } catch (const std::exception& x) {
        return no(std::string("lo scenario non si legge: ") + x.what());
    }
    if (!s) return no("il pacchetto non contiene lo scenario dell'editor");
    e.id = s->id;
    e.titolo = s->titolo[0];
    if (!id_valido(s->id)) return no("id della missione non valido: " + s->id);
    // 4. le dipendenze
    if (!std::filesystem::exists(cartella + "/teatri/" + s->teatro + ".fdl")) return no("teatro sconosciuto: " + s->teatro);
    const auto classi = classi_di(*s);
    std::string mancanti;
    for (const auto& c : classi)
        if (!d.modelli.count(c)) mancanti += (mancanti.empty() ? "" : ", ") + c;
    if (!mancanti.empty()) return no("classi sconosciute: " + mancanti);
    if (p->impronta_teatro != impronta_teatro(cartella, s->teatro))
        e.problemi.push_back("il fondale del teatro e' diverso da quello di chi l'ha creata: le unita' potrebbero essere sulla terra");
    if (p->impronta_classi != impronta_classi(classi, d)) e.problemi.push_back("i modelli delle classi sono diversi da quelli di chi l'ha creata");
    if (p->impronta_kit != impronta_kit(d)) e.problemi.push_back("i comandi del giocatore sono di un'altra versione: la missione si rigenera con questi");
    // 5. il file rigenerato dallo scenario, e le funzioni m688 del pacchetto
    std::string missione;
    try {
        missione = scrivi_missione_lua(*s, d);
    } catch (const std::exception& x) {
        return no(std::string("lo scenario non si scrive: ") + x.what());
    }
    std::set<std::string> conosciute = funzioni_m688(missione);
    conosciute.insert(FUNZIONI_REGOLE.begin(), FUNZIONI_REGOLE.end());
    std::string sconosciute;
    for (const auto& f : funzioni_m688(testo))
        if (!conosciute.count(f)) sconosciute += (sconosciute.empty() ? "" : ", ") + f;
    if (!sconosciute.empty()) return no("funzioni m688 sconosciute: " + sconosciute + " (il pacchetto e' stato modificato a mano)");
    if (senza_pacchetto(testo) != missione)
        e.problemi.push_back("il codice del pacchetto e' diverso da quello che scrive questo gioco: si usa quello rigenerato");
    if (const Fondale f = Fondale::carica(cartella + "/teatri/" + s->teatro + ".fdl"); true)
        for (const auto& a : verifica(*s, f, d))
            if (a.gravita == AvvisoScenario::Gravita::errore) e.problemi.push_back("da correggere nell'editor: " + a.testo);
    // 6. la missione nelle importate
    std::error_code ec;
    std::filesystem::create_directories(destinazione, ec);
    e.file = destinazione / (s->id + ".lua");
    std::ofstream(e.file, std::ios::binary) << missione;
    if (!std::filesystem::exists(e.file)) return no("impossibile scrivere " + e.file.string());
    e.scenario = std::move(s);
    e.importato = true;
    return e;
}

std::filesystem::path cartella_missioni_importate() { return cartella_fascicoli().parent_path() / "missioni_importate"; }
std::filesystem::path cartella_da_importare() { return cartella_missioni_importate() / "da_importare"; }
std::filesystem::path cartella_missioni_esportate() { return cartella_fascicoli().parent_path() / "missioni_esportate"; }

std::vector<EsitoImportazione> importa_cartella(const std::filesystem::path& da, const std::filesystem::path& a, const DatiScenario& d,
                                                const std::string& cartella) {
    std::vector<std::filesystem::path> file;
    std::error_code ec;
    for (const auto& x : std::filesystem::directory_iterator(da, ec))
        if (x.is_regular_file() && x.path().extension() == ".m688m") file.push_back(x.path());
    std::sort(file.begin(), file.end());
    std::vector<EsitoImportazione> v;
    for (const auto& f : file) {
        EsitoImportazione e;
        try {
            e = importa_pacchetto(leggi_file(f), d, cartella, a);
        } catch (const std::exception& x) {
            e.problemi.push_back(x.what());
        }
        e.sorgente = f.filename().string();
        const auto dove = da / (e.importato ? "importati" : "rifiutati");
        std::filesystem::create_directories(dove, ec);
        std::filesystem::rename(f, dove / f.filename(), ec);
        if (!e.importato) {
            std::ofstream r(dove / (f.stem().string() + ".txt"), std::ios::binary);
            for (const auto& x : e.problemi) r << x << "\n";
        }
        v.push_back(std::move(e));
    }
    return v;
}

}  // namespace m688
