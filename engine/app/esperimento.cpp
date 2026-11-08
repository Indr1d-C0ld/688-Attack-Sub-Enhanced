#include "esperimento.hpp"

#include <fstream>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

std::string testo(const toml::table& t, const char* chiave) { return t[chiave].value_or(std::string{}); }

}  // namespace

Esperimento leggi_esperimento(const std::filesystem::path& p) {
    toml::table t;
    try {
        t = toml::parse_file(p.string());
    } catch (const toml::parse_error& e) {
        throw std::runtime_error("esperimento " + p.string() + ": " + std::string(e.description()));
    }
    const toml::table* es = t["esperimento"].as_table();
    if (!es) throw std::runtime_error("esperimento " + p.string() + ": manca [esperimento]");
    Esperimento e;
    e.titolo = testo(*es, "titolo");
    e.mostra = testo(*es, "mostra");
    e.ipotesi = testo(*es, "ipotesi");
    e.codice = testo(*es, "codice");
    e.impronta_missione = testo(*es, "impronta_missione");
    e.semi = static_cast<int>((*es)["semi"].value_or(int64_t{10}));
    e.da = static_cast<std::uint64_t>((*es)["da"].value_or(int64_t{1}));
    if (e.codice.empty()) throw std::runtime_error("esperimento " + p.string() + ": manca il codice sandbox");
    if (e.semi < 1) e.semi = 1;
    if (const toml::array* v = t["intervento"].as_array())
        for (const toml::node& n : *v) {
            const toml::table* x = n.as_table();
            if (!x) continue;
            Intervento i;
            i.tempo = (*x)["tempo"].value_or(-1.0);
            i.tipo = testo(*x, "tipo");
            i.unita = static_cast<int>((*x)["unita"].value_or(int64_t{-1}));
            if (const toml::table* q = (*x)["punto"].as_table()) i.punto = Vec2{(*q)["x"].value_or(0.0), (*q)["y"].value_or(0.0)};
            i.raggio_m = (*x)["raggio_m"].value_or(0.0);
            i.durata_s = (*x)["durata_s"].value_or(0.0);
            if (const toml::table* q = (*x)["parametri"].as_table())
                for (const auto& [k, w] : *q) i.parametri[std::string(k.str())] = w.value_or(0.0);
            if (const toml::table* q = (*x)["testi"].as_table())
                for (const auto& [k, w] : *q) i.testi[std::string(k.str())] = w.value_or(std::string{});
            if (const toml::table* q = (*x)["innesco"].as_table()) {   // su evento (D2): un evento del registro o una zona
                Innesco n;
                n.evento = testo(*q, "evento");
                n.unita = static_cast<int>((*q)["unita"].value_or(int64_t{-1}));
                n.altra = static_cast<int>((*q)["altra"].value_or(int64_t{-1}));
                if (const toml::table* p = (*q)["punto"].as_table()) n.punto = Vec2{(*p)["x"].value_or(0.0), (*p)["y"].value_or(0.0)};
                n.raggio_m = (*q)["raggio_m"].value_or(0.0);
                if (!n.evento.empty()) i.innesco = std::move(n);
            }
            if (!i.tipo.empty()) e.interventi.push_back(std::move(i));
        }
    return e;
}

void scrivi_esperimento(const std::filesystem::path& p, const Esperimento& e) {
    toml::table es{{"titolo", e.titolo}, {"mostra", e.mostra}, {"codice", e.codice}, {"semi", e.semi},
                   {"da", static_cast<int64_t>(e.da)}};
    if (!e.ipotesi.empty()) es.insert("ipotesi", e.ipotesi);
    if (!e.impronta_missione.empty()) es.insert("impronta_missione", e.impronta_missione);
    toml::array interventi;
    for (const Intervento& i : e.interventi) {
        toml::table x{{"tempo", i.tempo}, {"tipo", i.tipo}};
        if (i.unita >= 0) x.insert("unita", i.unita);
        if (i.punto) x.insert("punto", toml::table{{"x", i.punto->x}, {"y", i.punto->y}});
        if (i.raggio_m > 0.0) x.insert("raggio_m", i.raggio_m);
        x.insert("durata_s", i.durata_s);
        if (!i.parametri.empty()) {
            toml::table q;
            for (const auto& [k, w] : i.parametri) q.insert(k, w);
            x.insert("parametri", std::move(q));
        }
        if (!i.testi.empty()) {
            toml::table q;
            for (const auto& [k, w] : i.testi) q.insert(k, w);
            x.insert("testi", std::move(q));
        }
        if (i.innesco) {
            toml::table q{{"evento", i.innesco->evento}};
            if (i.innesco->unita >= 0) q.insert("unita", i.innesco->unita);
            if (i.innesco->altra >= 0) q.insert("altra", i.innesco->altra);
            if (i.innesco->punto) q.insert("punto", toml::table{{"x", i.innesco->punto->x}, {"y", i.innesco->punto->y}});
            if (i.innesco->raggio_m > 0.0) q.insert("raggio_m", i.innesco->raggio_m);
            x.insert("innesco", std::move(q));
        }
        interventi.push_back(std::move(x));
    }
    toml::table t{{"esperimento", std::move(es)}};
    if (!interventi.empty()) t.insert("intervento", std::move(interventi));
    std::ofstream f(p);
    if (!f) throw std::runtime_error("impossibile scrivere " + p.string());
    f << "# esperimento del vetrino di Dio (melchior688, docs/28)\n" << t << "\n";
}

}  // namespace m688
