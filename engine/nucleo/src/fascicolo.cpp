#include "nucleo/fascicolo.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

constexpr int VERSIONE = 1;

template <class T>
toml::array lista(const std::vector<T>& v) {
    toml::array a;
    for (const auto& x : v) a.push_back(x);
    return a;
}

std::vector<std::string> stringhe(const toml::node* n) {
    std::vector<std::string> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a) out.push_back(v.value_or(std::string{}));
    return out;
}

std::vector<int> interi(const toml::node* n) {
    std::vector<int> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a) out.push_back(static_cast<int>(v.value_or(std::int64_t{0})));
    return out;
}

// arrays di tabelle: scrittura e lettura con una funzione per elemento
template <class T, class F>
toml::array tabelle(const std::vector<T>& v, F scrivi) {
    toml::array a;
    for (const auto& x : v) a.push_back(scrivi(x));
    return a;
}

template <class T, class F>
std::vector<T> leggi_tabelle(const toml::node* n, F leggi) {
    std::vector<T> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a)
            if (const auto* t = v.as_table()) out.push_back(leggi(*t));
    return out;
}

std::string testo(const toml::table& t, const char* k) { return t[k].value_or(std::string{}); }

}  // namespace

std::string_view nome_marina(Marina m) { return m == Marina::usn ? "usn" : "vmf"; }

std::optional<Marina> marina_da_nome(std::string_view s) {
    if (s == "usn") return Marina::usn;
    if (s == "vmf") return Marina::vmf;
    return std::nullopt;
}

std::string_view nome_indulgenza(Indulgenza i) {
    switch (i) {
        case Indulgenza::realistica: return "realistica";
        case Indulgenza::indulgente: return "indulgente";
        case Indulgenza::arcade: return "arcade";
    }
    return "realistica";
}

std::optional<Indulgenza> indulgenza_da_nome(std::string_view s) {
    for (const Indulgenza i : {Indulgenza::realistica, Indulgenza::indulgente, Indulgenza::arcade})
        if (s == nome_indulgenza(i)) return i;
    return std::nullopt;
}

bool Fascicolo::ha_qualifica(const std::string& q) const {
    return std::any_of(qualifiche.begin(), qualifiche.end(), [&](const Qualifica& x) { return x.id == q; });
}

std::filesystem::path cartella_fascicoli() {
    if (const char* x = std::getenv("XDG_DATA_HOME"); x && *x) return std::filesystem::path(x) / "melchior688" / "fascicoli";
    const char* casa = std::getenv("HOME");
    return std::filesystem::path(casa ? casa : ".") / ".local" / "share" / "melchior688" / "fascicoli";
}

std::filesystem::path percorso_fascicolo(const std::string& id) { return cartella_fascicoli() / (id + ".toml"); }

std::vector<std::string> elenco_fascicoli() {
    std::vector<std::string> out;
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(cartella_fascicoli(), ec))
        if (e.is_regular_file() && e.path().extension() == ".toml") out.push_back(e.path().stem().string());
    std::sort(out.begin(), out.end());
    return out;
}

void salva_fascicolo(const Fascicolo& f, const std::filesystem::path& p) {
    toml::table t;
    t.insert("versione", VERSIONE);
    t.insert("id", f.id);
    t.insert("nome", f.nome);
    t.insert("marina", std::string(nome_marina(f.marina)));
    t.insert("carriera_breve", f.carriera_breve);
    t.insert("aperta", f.aperta);
    t.insert("indulgenza", std::string(nome_indulgenza(f.indulgenza)));
    t.insert("provenienza", f.provenienza);
    t.insert("seme", static_cast<std::int64_t>(f.seme));
    t.insert("grado", f.grado);
    t.insert("fase", f.fase);
    t.insert("missioni_nella_fase", f.missioni_nella_fase);
    t.insert("prossima_commissione", f.prossima_commissione);
    t.insert("mancate", f.mancate);
    t.insert("blocco_promozioni", f.blocco_promozioni);
    t.insert("mediocri_di_fila", f.mediocri_di_fila);
    t.insert("seconda_possibilita_usata", f.seconda_possibilita_usata);
    t.insert("fine_comando_data", f.fine_comando_data);
    t.insert("data", f.data);
    t.insert("anno_nomina", f.anno_nomina);
    t.insert("stato", f.stato);
    t.insert("incarichi", tabelle(f.incarichi, [](const Incarico& x) {
        return toml::table{{"grado", x.grado}, {"incarico", x.incarico}, {"battello", x.battello}, {"dal", x.dal}, {"al", x.al}};
    }));
    t.insert("qualifiche", tabelle(f.qualifiche, [](const Qualifica& x) { return toml::table{{"id", x.id}, {"data", x.data}}; }));
    t.insert("missioni", tabelle(f.missioni, [](const VoceMissione& x) {
        return toml::table{{"missione", x.missione}, {"origine", x.origine}, {"data", x.data}, {"grado", x.grado},
                           {"seme", static_cast<std::int64_t>(x.seme)}, {"punteggio", x.punteggio}, {"peso", x.peso},
                           {"punti", x.punti}, {"ordini", x.ordini}, {"giudizio_it", x.giudizio_it},
                           {"giudizio_en", x.giudizio_en}, {"graduatoria", x.graduatoria}, {"pari", x.pari},
                           {"replay", x.replay}};
    }));
    t.insert("decorazioni", tabelle(f.decorazioni, [](const Conferimento& x) {
        return toml::table{{"decorazione", x.decorazione}, {"dispositivo", x.dispositivo}, {"numero", x.numero},
                           {"data", x.data}, {"missione", x.missione}, {"postuma", x.postuma},
                           {"fatti", lista(x.fatti)}, {"motivazione_it", lista(x.motivazione_it)},
                           {"motivazione_en", lista(x.motivazione_en)}};
    }));
    t.insert("commissioni", tabelle(f.commissioni, [](const Commissione& x) {
        return toml::table{{"data", x.data}, {"grado", x.grado}, {"esito", x.esito}, {"media", x.media}};
    }));
    t.insert("provvedimenti", tabelle(f.provvedimenti, [](const Provvedimento& x) {
        return toml::table{{"data", x.data}, {"tipo", x.tipo}, {"motivo", x.motivo}, {"missione", x.missione}};
    }));
    std::error_code ec;
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path(), ec);
    std::filesystem::path tmp = p;
    tmp += ".tmp";
    {
        std::ofstream o(tmp, std::ios::trunc);
        if (!o) throw std::runtime_error(p.string() + ": impossibile scrivere il fascicolo");
        o << "# Fascicolo personale del comandante (melchior688)\n" << t << '\n';
        if (!o) throw std::runtime_error(p.string() + ": scrittura del fascicolo interrotta");
    }
    std::filesystem::rename(tmp, p);
}

Fascicolo leggi_fascicolo(const std::filesystem::path& p) {
    toml::table t;
    try {
        t = toml::parse_file(p.string());
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(p.string() + ": " + std::string(e.description()));
    }
    const int v = t["versione"].value_or(0);
    if (v < 1 || v > VERSIONE) throw std::runtime_error(p.string() + ": versione del fascicolo sconosciuta");
    Fascicolo f;
    f.versione = v;
    f.id = testo(t, "id");
    f.nome = testo(t, "nome");
    f.marina = marina_da_nome(testo(t, "marina")).value_or(Marina::usn);
    f.carriera_breve = t["carriera_breve"].value_or(false);
    f.aperta = t["aperta"].value_or(false);
    f.indulgenza = indulgenza_da_nome(testo(t, "indulgenza")).value_or(Indulgenza::realistica);
    f.provenienza = testo(t, "provenienza");
    f.seme = static_cast<std::uint32_t>(t["seme"].value_or(std::int64_t{1}));
    f.grado = testo(t, "grado");
    f.fase = t["fase"].value_or(0);
    f.missioni_nella_fase = t["missioni_nella_fase"].value_or(0);
    f.prossima_commissione = t["prossima_commissione"].value_or(0);
    f.mancate = t["mancate"].value_or(0);
    f.blocco_promozioni = t["blocco_promozioni"].value_or(0);
    f.mediocri_di_fila = t["mediocri_di_fila"].value_or(0);
    f.seconda_possibilita_usata = t["seconda_possibilita_usata"].value_or(false);
    f.fine_comando_data = t["fine_comando_data"].value_or(false);
    f.data = testo(t, "data");
    f.anno_nomina = t["anno_nomina"].value_or(1989);
    f.stato = t["stato"].value_or(std::string{"servizio"});
    f.incarichi = leggi_tabelle<Incarico>(t["incarichi"].node(), [](const toml::table& x) {
        return Incarico{testo(x, "grado"), testo(x, "incarico"), testo(x, "battello"), testo(x, "dal"), testo(x, "al")};
    });
    f.qualifiche = leggi_tabelle<Qualifica>(t["qualifiche"].node(), [](const toml::table& x) { return Qualifica{testo(x, "id"), testo(x, "data")}; });
    f.missioni = leggi_tabelle<VoceMissione>(t["missioni"].node(), [](const toml::table& x) {
        VoceMissione m;
        m.missione = testo(x, "missione");
        m.origine = testo(x, "origine");
        m.data = testo(x, "data");
        m.grado = testo(x, "grado");
        m.seme = static_cast<std::uint64_t>(x["seme"].value_or(std::int64_t{0}));
        m.punteggio = x["punteggio"].value_or(0.0);
        m.peso = x["peso"].value_or(1.0);
        m.punti = x["punti"].value_or(0.0);
        m.ordini = x["ordini"].value_or(0.0);
        m.giudizio_it = testo(x, "giudizio_it");
        m.giudizio_en = testo(x, "giudizio_en");
        m.graduatoria = x["graduatoria"].value_or(1);
        m.pari = x["pari"].value_or(1);
        m.replay = testo(x, "replay");
        return m;
    });
    f.decorazioni = leggi_tabelle<Conferimento>(t["decorazioni"].node(), [](const toml::table& x) {
        Conferimento k;
        k.decorazione = testo(x, "decorazione");
        k.dispositivo = testo(x, "dispositivo");
        k.numero = x["numero"].value_or(1);
        k.data = testo(x, "data");
        k.missione = testo(x, "missione");
        k.postuma = x["postuma"].value_or(false);
        k.fatti = interi(x["fatti"].node());
        k.motivazione_it = stringhe(x["motivazione_it"].node());
        k.motivazione_en = stringhe(x["motivazione_en"].node());
        return k;
    });
    f.commissioni = leggi_tabelle<Commissione>(t["commissioni"].node(), [](const toml::table& x) {
        return Commissione{testo(x, "data"), testo(x, "grado"), testo(x, "esito"), x["media"].value_or(0.0)};
    });
    f.provvedimenti = leggi_tabelle<Provvedimento>(t["provvedimenti"].node(), [](const toml::table& x) {
        return Provvedimento{testo(x, "data"), testo(x, "tipo"), testo(x, "motivo"), testo(x, "missione")};
    });
    return f;
}

}  // namespace m688
