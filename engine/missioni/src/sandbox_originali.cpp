// Il piano del comandante IA per le missioni originali nella sandbox: vedi missioni/sandbox_originali.hpp.

#include "missioni/sandbox_originali.hpp"

#include <filesystem>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

const char* nome_condotta(PianoOriginale::Condotta c) {
    switch (c) {
        case PianoOriginale::Condotta::transito: return "transito";
        case PianoOriginale::Condotta::caccia: return "caccia";
        case PianoOriginale::Condotta::scorta: return "scorta";
        case PianoOriginale::Condotta::pedinamento: return "pedinamento";
        case PianoOriginale::Condotta::ricognizione: return "ricognizione";
        case PianoOriginale::Condotta::minamento: return "minamento";
        case PianoOriginale::Condotta::evasione: return "evasione";
        case PianoOriginale::Condotta::attacco_terra: return "attacco_terra";
    }
    return "?";
}

std::optional<PianoOriginale> piano_originale(const std::string& cartella_dati, const std::string& id_missione) {
    const std::string percorso = cartella_dati + "/sandbox_originali.toml";
    if (!std::filesystem::exists(percorso)) return std::nullopt;
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const auto* tab = t[id_missione].as_table();
    if (!tab) return std::nullopt;
    const auto errore = [&](const std::string& cosa) { return std::runtime_error(percorso + ": " + id_missione + ": " + cosa); };
    const auto punto = [&](const toml::node& n) {
        const auto* a = n.as_array();
        if (!a || a->size() != 2 || !(*a)[0].value<double>() || !(*a)[1].value<double>()) throw errore("punto non [x, y]");
        return Vec2{*(*a)[0].value<double>(), *(*a)[1].value<double>()};
    };
    const auto punti = [&](const toml::node_view<const toml::node> n) {
        std::vector<Vec2> v;
        if (const auto* a = n.as_array())
            for (const auto& p : *a) v.push_back(punto(p));
        return v;
    };
    const auto cerchio = [&](const char* campo) -> std::optional<PianoOriginale::Cerchio> {
        const auto* c = (*tab)[campo].as_table();
        if (!c) return std::nullopt;
        const auto x = (*c)["x"].value<double>(), y = (*c)["y"].value<double>(), r = (*c)["r"].value<double>();
        if (!x || !y || !r) throw errore(std::string(campo) + " senza x, y o r");
        return PianoOriginale::Cerchio{{*x, *y}, *r};
    };

    PianoOriginale p;
    const std::string c = (*tab)["condotta"].value_or(std::string{});
    bool nota = false;
    for (int k = 0; k <= static_cast<int>(PianoOriginale::Condotta::attacco_terra); ++k)
        if (c == nome_condotta(static_cast<PianoOriginale::Condotta>(k))) p.condotta = static_cast<PianoOriginale::Condotta>(k), nota = true;
    if (!nota) throw errore("condotta '" + c + "' sconosciuta");
    const std::string a = (*tab)["armi"].value_or(std::string("roe"));
    if (a == "libere") p.armi = PianoOriginale::Armi::libere;
    else if (a == "roe") p.armi = PianoOriginale::Armi::roe;
    else if (a == "solo_difesa") p.armi = PianoOriginale::Armi::solo_difesa;
    else if (a == "classi") p.armi = PianoOriginale::Armi::classi;
    else throw errore("armi '" + a + "' sconosciute");
    p.fonte = (*tab)["fonte"].value_or(std::string{});
    if (const auto* v = (*tab)["classi"].as_array())
        for (const auto& k : *v)
            if (const auto s = k.value<std::string>()) p.classi.push_back(*s);
    p.quota_m = (*tab)["quota_m"].value_or(0.0);
    p.velocita_kn = (*tab)["velocita_kn"].value_or(0.0);
    p.seminare = (*tab)["seminare"].value_or(false);
    p.punti = punti((*tab)["punti"]);
    p.area = cerchio("area");
    p.uscita = cerchio("uscita");
    if (const auto* l = (*tab)["limite"].as_table()) {
        const auto x0 = (*l)["x0"].value<double>(), y0 = (*l)["y0"].value<double>(), x1 = (*l)["x1"].value<double>(),
                   y1 = (*l)["y1"].value<double>();
        if (!x0 || !y0 || !x1 || !y1 || *x0 >= *x1 || *y1 >= *y0) throw errore("limite non { x0, y0, x1, y1 } (ovest, nord, est, sud)");
        p.limite = PianoOriginale::Rettangolo{*x0, *y0, *x1, *y1};
    }
    if (const auto* m = (*tab)["pim"].as_table()) {
        PianoOriginale::Pim pim;
        pim.punti = punti((*m)["punti"]);
        pim.velocita_kn = (*m)["velocita_kn"].value_or(0.0);
        if (pim.punti.empty() || pim.velocita_kn <= 0.0) throw errore("pim senza punti o velocita_kn");
        p.pim = pim;
    }
    p.avanti_m = (*tab)["avanti_m"].value_or(0.0);
    p.distanza_m = (*tab)["distanza_m"].value_or(0.0);
    p.ping_m = (*tab)["ping_m"].value_or(0.0);
    p.attesa_s = (*tab)["attesa_s"].value_or(0.0);
    if (const auto* v = (*tab)["tubi"].as_array())
        for (const auto& k : *v)
            if (const auto s = k.value<std::string>()) p.tubi.push_back(*s);
    if (p.condotta == PianoOriginale::Condotta::scorta && !p.pim) throw errore("scorta senza pim");
    if (p.condotta == PianoOriginale::Condotta::transito && p.punti.empty() && !p.uscita) throw errore("transito senza punti ne' uscita");
    return p;
}

std::vector<std::string> applica_tubi_piano(SalaSiluri& s, const PianoOriginale& p) {
    std::vector<std::string> mancate;
    std::vector<std::size_t> tubi;   // gli indici delle posizioni "tubo" del locale
    for (std::size_t i = 0; i < s.carico().tipi.size(); ++i)
        if (s.carico().tipi[i] == "tubo") tubi.push_back(i);
    for (std::size_t k = 0; k < p.tubi.size(); ++k) {
        const std::string& arma = p.tubi[k];
        const int tubo = static_cast<int>(k) + 1;
        EsitoCarico e;
        if (k < tubi.size()) {
            const std::string prima = s.carico().posizioni[tubi[k]];
            if (prima == arma) continue;
            if (!prima.empty()) s.togli(Posto::tubo, prima, tubo);
            // l'arma dalla rastrelliera, se ce n'e' (il deposito della missione l'ha gia' contata)
            if (const auto r = s.carico().rastrelliera.find(arma); r != s.carico().rastrelliera.end() && r->second > 0)
                s.togli(Posto::rastrelliera, arma);
            e = s.metti(Posto::tubo, arma, tubo);
            if (!e.fatto) e = s.metti(Posto::pozzi, arma);   // il Flight II: i missili nei pozzi
            if (!prima.empty()) {
                if (e.fatto) s.metti(Posto::rastrelliera, prima);   // il siluro tolto, se c'e' posto
                else s.metti(Posto::tubo, prima, tubo);              // niente da fare: il tubo torna com'era
            }
        } else {
            e = s.metti(Posto::pozzi, arma);
        }
        if (!e.fatto) mancate.push_back(arma + " nel tubo " + std::to_string(tubo) + ": " + e.motivo);
    }
    return mancate;
}

}  // namespace m688
