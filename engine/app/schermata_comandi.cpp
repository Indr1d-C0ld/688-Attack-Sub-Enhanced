#include "schermata_comandi.hpp"

#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {
std::string testo_di(const toml::table& t, const char* k) { return t[k].value_or(std::string{}); }

// I tasti di un comando, come cappucci separati da ", "
std::string tasti_di(const std::string& comando, const MappaTasti& tasti) {
    const auto v = tasti.tasti(comando);
    if (v.empty()) return "(NESSUN TASTO)";
    std::string out;
    for (const std::string& k : v) {
        // i doppioni si lasciano al manuale: = per il +, il tastierino quando c'e' lo stesso tasto fuori
        if (k == "=" || (k.rfind("Tastierino", 0) == 0 && k != v.front())) continue;
        if (!out.empty()) out += ", ";
        out += k;
        // le quadre non hanno un tasto loro sulla tastiera italiana: finestra.cpp le riconosce anche con AltGr
        if (k == "[") out += ", AltGr+è";
        if (k == "]") out += ", AltGr++";
    }
    return out;
}

// {comando:NOME} col suo tasto
std::string con_tasti(std::string s, const MappaTasti& tasti) {
    for (std::size_t a; (a = s.find("{comando:")) != std::string::npos;) {
        const std::size_t b = s.find('}', a);
        if (b == std::string::npos) break;
        s.replace(a, b - a + 1, tasti_di(s.substr(a + 9, b - a - 9), tasti));
    }
    return s;
}
}  // namespace

TestoComandi::TestoComandi(const std::string& cartella_dati) {
    const toml::table t = toml::parse_file(cartella_dati + "/comandi_schermata.toml");
    const toml::array* schede = t["scheda"].as_array();
    if (!schede) throw std::runtime_error("comandi_schermata.toml: nessuna scheda");
    for (const auto& x : *schede) {
        const toml::table& s = *x.as_table();
        Scheda sc;
        sc.etichetta = testo_di(s, "etichetta");
        sc.stazione = s["stazione"].value_or(0);
        if (const auto* a = s["consiglio"].as_array())
            for (const auto& c : *a) sc.consigli.push_back({testo_di(*c.as_table(), "solo"), testo_di(*c.as_table(), "testo")});
        if (const auto* a = s["gruppo"].as_array())
            for (const auto& g : *a) {
                Gruppo gr;
                gr.titolo = testo_di(*g.as_table(), "titolo");
                gr.solo = testo_di(*g.as_table(), "solo");
                if (const auto* r = (*g.as_table())["riga"].as_array())
                    for (const auto& q : *r) {
                        Riga ri;
                        ri.solo = testo_di(*q.as_table(), "solo");
                        ri.cosa = testo_di(*q.as_table(), "cosa");
                        if (const auto* v = (*q.as_table())["tasti"].as_array())
                            for (const auto& k : *v) ri.tasti.push_back(k.value_or(std::string{}));
                        gr.righe.push_back(std::move(ri));
                    }
                sc.gruppi.push_back(std::move(gr));
            }
        schede_.push_back(std::move(sc));
    }
}

bool riga_per(const std::string& solo, bool enhanced, bool alfa) {
    if (solo == "enhanced") return enhanced;
    if (solo == "classico") return !enhanced;
    if (solo == "alfa") return enhanced && alfa;
    if (solo == "688") return enhanced && !alfa;
    return true;
}

std::string scrivi_tasti_voci(const std::vector<std::string>& voci, const MappaTasti& tasti) {
    std::string out;
    for (const auto& v : voci) {
        const std::size_t p = v.find(':');
        const std::string tipo = v.substr(0, p), nome = p == std::string::npos ? std::string{} : v.substr(p + 1);
        std::string pezzo;
        if (tipo == "comando") {
            if (tasti.tasti(nome).empty()) continue;   // tolto dalla mappa (i tubi 2, 3, 4, 6 di partenza)
            pezzo = tasti_di(nome, tasti);
        }
        else if (tipo == "pulsanti") {
            const std::size_t t = nome.find('-');
            pezzo = tasti_di("pulsante_" + nome.substr(0, t), tasti) + "-" + tasti_di("pulsante_" + nome.substr(t + 1), tasti);
        } else pezzo = nome;
        if (!out.empty()) out += ", ";
        out += pezzo;
    }
    return out.empty() ? std::string("(NESSUN TASTO)") : out;
}

DatiSchermataComandi TestoComandi::dati(bool enhanced, bool alfa, const MappaTasti& tasti) const {
    DatiSchermataComandi d;
    d.alfa = alfa;
    for (const Scheda& s : schede_) {
        SchedaComandi sc;
        sc.etichetta = s.etichetta;
        for (const Consiglio& c : s.consigli)
            if (riga_per(c.solo, enhanced, alfa)) sc.consigli.push_back(con_tasti(c.testo, tasti));
        for (const Gruppo& g : s.gruppi) {
            if (!riga_per(g.solo, enhanced, alfa)) continue;
            GruppoComandi gc;
            gc.titolo = g.titolo;
            for (const Riga& r : g.righe)
                if (riga_per(r.solo, enhanced, alfa)) gc.righe.push_back({scrivi_tasti_voci(r.tasti, tasti), r.cosa});
            if (!gc.righe.empty()) sc.gruppi.push_back(std::move(gc));
        }
        d.schede.push_back(std::move(sc));
    }
    return d;
}

int TestoComandi::scheda_di(int stazione) const {
    int ovunque = 0;
    for (std::size_t i = 0; i < schede_.size(); ++i) {
        if (schede_[i].stazione == stazione) return static_cast<int>(i);
        if (schede_[i].stazione == 0) ovunque = static_cast<int>(i);
    }
    return ovunque;
}

}  // namespace m688
