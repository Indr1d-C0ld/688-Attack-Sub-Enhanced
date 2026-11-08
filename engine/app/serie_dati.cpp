#include "serie_dati.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

std::string stringa(const std::string& s) {   // una stringa TOML tra virgolette
    std::string o = "\"";
    for (const char c : s) {
        if (c == '"' || c == '\\') o += '\\';
        if (c == '\n') {
            o += "\\n";
            continue;
        }
        o += c;
    }
    return o + "\"";
}

std::string numero(double v) {
    if (!std::isfinite(v)) return "-1";
    char b[32];
    std::snprintf(b, sizeof b, "%.6g", v);
    std::string s = b;
    if (s.find_first_of(".en") == std::string::npos) s += ".0";   // un float anche quando e' intero
    return s;
}

double leggi_numero(const toml::node* n, double predefinito = -1.0) {
    if (!n) return predefinito;
    if (const auto v = n->value<double>()) return *v;
    return predefinito;
}

}  // namespace

// 0x18: la parte del luogo (M7, schieramento = 'luogo')
const char* nome_parte_serie(int parte) { return parte == 0x08 ? "nato" : parte == 0x10 ? "patto" : parte == 0x18 ? "luogo" : "neutrali"; }

int parte_serie(const std::string& nome) {
    if (nome == "nato") return 0x08;
    if (nome == "patto") return 0x10;
    if (nome == "neutrali") return 0;
    if (nome == "luogo") return 0x18;
    return -1;
}

DatiSerie dati_serie(const std::vector<StatisticheSandbox>& partite, const std::vector<std::string>& tracce, std::uint64_t da) {
    DatiSerie d;
    d.da = da;
    d.semi = static_cast<int>(partite.size());
    d.riassunto = riassunto_partite(partite);
    for (std::size_t i = 0; i < partite.size(); ++i) {
        const StatisticheSandbox& s = partite[i];
        d.durata_s = std::max(d.durata_s, s.durata_s);
        PartitaSerie p;
        p.seme = da + i;
        p.esito = s.esito;
        p.esito_testo = s.esito_testo;
        p.traccia = i < tracce.size() ? tracce[i] : std::string();
        for (const auto& [parte, v] : s.parti) p.numeri[parte] = numeri_parte(v);
        d.partite.push_back(std::move(p));
    }
    return d;
}

void scrivi_serie(const std::filesystem::path& p, const DatiSerie& d) {
    std::ostringstream o;
    o << "# Serie della sandbox (strumenti/serie_sandbox; app/serie_dati.hpp per il formato)\n\n[serie]\n";
    o << "missione = " << stringa(d.missione) << "\ncodice = " << stringa(d.codice) << "\nda = " << d.da << "\nsemi = " << d.semi
      << "\ndurata_s = " << numero(d.durata_s) << "\ndata = " << stringa(d.data) << "\n\n[esiti]\n";
    for (const auto& [codice, e] : d.riassunto.esiti)
        o << stringa(std::to_string(codice)) << " = { volte = " << e.volte << ", testo = " << stringa(e.testo)
          << ", frequenza = " << numero(e.frequenza) << " }\n";
    for (const auto& [parte, voci] : d.riassunto.parti) {
        o << "\n[parti." << nome_parte_serie(parte) << "]\n";
        for (const auto& [nome, v] : voci)
            o << nome << " = { media = " << numero(v.media) << ", dev = " << numero(v.dev) << ", min = " << numero(v.min) << ", max = "
              << numero(v.max) << ", q10 = " << numero(v.q10) << ", mediana = " << numero(v.mediana) << ", q90 = " << numero(v.q90)
              << ", n = " << v.n << " }\n";
    }
    for (const auto& [parte, armi] : d.riassunto.pk) {
        o << "\n[pk." << nome_parte_serie(parte) << "]\n";
        for (const auto& [arma, v] : armi) o << stringa(arma) << " = { a_segno = " << v.first << ", lanciate = " << v.second << " }\n";
    }
    for (const PartitaSerie& x : d.partite) {
        o << "\n[[partita]]\nseme = " << x.seme << "\nesito = " << x.esito << "\nesito_testo = " << stringa(x.esito_testo)
          << "\ntraccia = " << stringa(x.traccia) << "\n";
        for (const auto& [parte, numeri] : x.numeri) {
            o << nome_parte_serie(parte) << " = { ";
            bool primo = true;
            for (const auto& [nome, v] : numeri) {
                o << (primo ? "" : ", ") << nome << " = " << numero(v);
                primo = false;
            }
            o << " }\n";
        }
    }
    std::ofstream f(p, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile scrivere " + p.string());
    f << o.str();
}

DatiSerie leggi_serie(const std::filesystem::path& p) {
    toml::table t;
    try {
        t = toml::parse_file(p.string());
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(p.string() + ": " + std::string(e.description()));
    }
    DatiSerie d;
    const auto serie = t["serie"];
    d.missione = serie["missione"].value_or(std::string());
    d.codice = serie["codice"].value_or(std::string());
    d.data = serie["data"].value_or(std::string());
    d.da = static_cast<std::uint64_t>(serie["da"].value_or(std::int64_t{1}));
    d.semi = static_cast<int>(serie["semi"].value_or(std::int64_t{0}));
    d.durata_s = serie["durata_s"].value_or(0.0);
    if (const auto* esiti = t["esiti"].as_table())
        for (const auto& [k, v] : *esiti) {
            EsitoSerie e;
            e.volte = static_cast<int>((*v.as_table())["volte"].value_or(std::int64_t{0}));
            e.testo = (*v.as_table())["testo"].value_or(std::string());
            e.frequenza = (*v.as_table())["frequenza"].value_or(0.0);
            d.riassunto.esiti[std::stoi(std::string(k.str()))] = e;
        }
    d.riassunto.partite = d.semi;
    if (const auto* parti = t["parti"].as_table())
        for (const auto& [nome_parte, voci] : *parti) {
            const int parte = parte_serie(std::string(nome_parte.str()));
            if (parte < 0 || !voci.as_table()) continue;
            for (const auto& [nome, v] : *voci.as_table()) {
                const toml::table* x = v.as_table();
                if (!x) continue;
                ValoreSerie s;
                s.media = leggi_numero(x->get("media"));
                s.dev = leggi_numero(x->get("dev"));
                s.min = leggi_numero(x->get("min"));
                s.max = leggi_numero(x->get("max"));
                s.q10 = leggi_numero(x->get("q10"));
                s.mediana = leggi_numero(x->get("mediana"));
                s.q90 = leggi_numero(x->get("q90"));
                s.n = static_cast<int>((*x)["n"].value_or(std::int64_t{0}));
                d.riassunto.parti[parte][std::string(nome.str())] = s;
            }
        }
    if (const auto* pk = t["pk"].as_table())
        for (const auto& [nome_parte, armi] : *pk) {
            const int parte = parte_serie(std::string(nome_parte.str()));
            if (parte < 0 || !armi.as_table()) continue;
            for (const auto& [arma, v] : *armi.as_table())
                d.riassunto.pk[parte][std::string(arma.str())] = {static_cast<int>((*v.as_table())["a_segno"].value_or(std::int64_t{0})),
                                                                static_cast<int>((*v.as_table())["lanciate"].value_or(std::int64_t{0}))};
        }
    if (const auto* partite = t["partita"].as_array())
        for (const auto& n : *partite) {
            const toml::table* x = n.as_table();
            if (!x) continue;
            PartitaSerie q;
            q.seme = static_cast<std::uint64_t>((*x)["seme"].value_or(std::int64_t{0}));
            q.esito = static_cast<int>((*x)["esito"].value_or(std::int64_t{-1}));
            q.esito_testo = (*x)["esito_testo"].value_or(std::string());
            q.traccia = (*x)["traccia"].value_or(std::string());
            for (const auto& [k, v] : *x) {
                const int parte = parte_serie(std::string(k.str()));
                if (parte < 0 || !v.as_table()) continue;
                for (const auto& [nome, val] : *v.as_table()) q.numeri[parte][std::string(nome.str())] = leggi_numero(&val);
            }
            d.partite.push_back(std::move(q));
        }
    return d;
}

}  // namespace m688
