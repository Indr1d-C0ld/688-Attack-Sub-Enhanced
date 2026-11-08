#include "confronto_ab.hpp"

#include <toml.hpp>

#include <cctype>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <sstream>

#include "nucleo/statistiche_sandbox.hpp"

namespace m688 {

namespace {

struct VoceNumero {
    const char* nome;
    const char* etichetta;
    const char* unita;
    double scala;
};
// come app/schermata_serie: i numeri di numeri_parte con l'etichetta e l'unita' mostrata
const VoceNumero VOCI[] = {
    {"unita", "UNITA'", "", 1.0},
    {"perse", "PERSE", "", 1.0},
    {"inflitte", "INFLITTE", "", 1.0},
    {"scambio", "SCAMBIO", "", 1.0},
    {"scoperte", "SCOPERTE FATTE", "", 1.0},
    {"scoperte_subite", "SCOPERTE SUBITE", "", 1.0},
    {"ritardo_medio_s", "RITARDO DI SCOPERTA", "s", 1.0},
    {"distanza_media_m", "DISTANZA DI SCOPERTA", "km", 0.001},
    {"reazione_media_s", "SCOPERTA - ALLERTA", "s", 1.0},
    {"ingaggio_medio_s", "SCOPERTA - INGAGGIO", "s", 1.0},
    {"lancio_medio_s", "SCOPERTA - LANCIO", "s", 1.0},
    {"gambe_medie", "GAMBE DI TMA", "", 1.0},
    {"soluzione_media_s", "TEMPO ALLA SOLUZIONE", "s", 1.0},
    {"errore_lancio_medio_m", "ERRORE AL LANCIO", "m", 1.0},
    {"lanci", "LANCI", "", 1.0},
    {"a_segno", "A SEGNO", "", 1.0},
    {"esche", "ESCHE", "", 1.0},
    {"esche_efficaci", "ESCHE EFFICACI", "", 1.0},
    {"evasione_media_s", "MINACCIA - EVASIONE", "s", 1.0},
    {"periscopio_medio_s", "PERISCOPIO ALZATO", "s", 1.0},
    {"antenna_media_s", "ANTENNA ALZATA", "s", 1.0},
    {"radar_medio_s", "RADAR ACCESO", "s", 1.0},
    {"ping", "PING", "", 1.0},
    {"radio", "TRASMISSIONI RADIO", "", 1.0},
    {"contro_scoperte", "CONTRO-SCOPERTE", "", 1.0},
    {"errore_nav_medio_m", "ERRORE DI NAVIGAZIONE", "m", 1.0},
    {"errore_nav_max_m", "ERRORE NAV. MASSIMO", "m", 1.0},
    {"punti_nave", "PUNTI NAVE", "", 1.0},
    {"durata_s", "DURATA", "min", 1.0 / 60.0},
};
const VoceNumero* voce(const std::string& nome) {
    for (const auto& v : VOCI)
        if (nome == v.nome) return &v;
    return nullptr;
}

std::string stringa(const std::string& s) {   // fra virgolette, per il TOML
    std::string o = "\"";
    for (const char c : s) {
        if (c == '"' || c == '\\') o += '\\';
        if (c == '\n') o += "\\n";
        else o += c;
    }
    return o + "\"";
}
std::string numero(double v) {
    if (!std::isfinite(v)) return "-1";
    char b[32];
    std::snprintf(b, sizeof b, "%.6g", v);
    return b;
}
std::string orologio(double s) {
    const int x = static_cast<int>(std::max(0.0, s));
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
    return b;
}

std::string riga_numero(const ConfrontoNumero& n) {
    return "{ coppie = " + std::to_string(n.coppie) + ", mediana_a = " + numero(n.mediana_a) + ", mediana_b = " + numero(n.mediana_b) +
           ", differenza = " + numero(n.differenza) + ", basso = " + numero(n.basso) + ", alto = " + numero(n.alto) +
           ", sale = " + std::to_string(n.sale) + ", scende = " + std::to_string(n.scende) + ", uguale = " + std::to_string(n.uguale) + " }";
}
std::string riga_wilson(const IntervalloWilson& w) {
    return "{ k = " + std::to_string(w.k) + ", n = " + std::to_string(w.n) + ", p = " + numero(w.p) + ", basso = " + numero(w.basso) +
           ", alto = " + numero(w.alto) + " }";
}

ConfrontoNumero leggi_numero(const toml::table& t) {
    ConfrontoNumero n;
    n.coppie = static_cast<int>(t["coppie"].value_or(0));
    n.mediana_a = t["mediana_a"].value_or(0.0);
    n.mediana_b = t["mediana_b"].value_or(0.0);
    n.differenza = t["differenza"].value_or(0.0);
    n.basso = t["basso"].value_or(0.0);
    n.alto = t["alto"].value_or(0.0);
    n.sale = static_cast<int>(t["sale"].value_or(0));
    n.scende = static_cast<int>(t["scende"].value_or(0));
    n.uguale = static_cast<int>(t["uguale"].value_or(0));
    return n;
}
IntervalloWilson leggi_wilson(const toml::table* t) {
    IntervalloWilson w;
    if (!t) return w;
    w.k = static_cast<int>((*t)["k"].value_or(0));
    w.n = static_cast<int>((*t)["n"].value_or(0));
    w.p = (*t)["p"].value_or(0.0);
    w.basso = (*t)["basso"].value_or(0.0);
    w.alto = (*t)["alto"].value_or(0.0);
    return w;
}

}  // namespace

std::string etichetta_numero(const std::string& nome) {
    if (const VoceNumero* v = voce(nome)) return v->etichetta;
    std::string s = nome;
    for (auto& c : s) c = c == '_' ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
std::string unita_numero(const std::string& nome) {
    const VoceNumero* v = voce(nome);
    return v ? v->unita : "";
}
double scala_numero(const std::string& nome) {
    const VoceNumero* v = voce(nome);
    return v ? v->scala : 1.0;
}
const char* nome_parte_confronto(int p) { return p == 0x08 ? "NATO" : p == 0x10 ? "PATTO" : p == 0x18 ? "LUOGO" : "NEUTRALI"; }

DatiConfronto confronta_tracce(const std::vector<Traccia>& a, const std::vector<Traccia>& b, const std::vector<std::string>& nomi_a,
                               const std::vector<std::string>& nomi_b, std::uint64_t da, const ParametriBootstrap& par) {
    if (a.size() != b.size()) throw std::runtime_error("confronto: le due serie hanno un numero diverso di partite");
    DatiConfronto d;
    d.da = da;
    if (!a.empty()) d.missione = a.front().missione;
    std::vector<StatisticheSandbox> sa, sb;
    for (std::size_t i = 0; i < a.size(); ++i) {
        sa.push_back(statistiche_sandbox(a[i]));
        sb.push_back(statistiche_sandbox(b[i]));
        if (!sa.back().esito_testo.empty()) d.testi_esiti[sa.back().esito] = sa.back().esito_testo;
        if (!sb.back().esito_testo.empty()) d.testi_esiti[sb.back().esito] = sb.back().esito_testo;
    }
    d.confronto = confronta_serie(sa, sb, par);
    if (!b.empty())   // le righe degli interventi applicati, dalla prima partita di B
        for (const RecordIntervento& x : b.front().interventi)
            if (x.fase == RecordIntervento::FASE_APPLICATO) d.interventi.push_back(orologio(x.t) + "  " + x.riga_it);
    for (std::size_t i = 0; i < a.size(); ++i) {
        CoppiaConfronto c;
        c.seme = da + i;
        c.traccia_a = i < nomi_a.size() ? nomi_a[i] : std::string();
        c.traccia_b = i < nomi_b.size() ? nomi_b[i] : std::string();
        c.esito_a = sa[i].esito;
        c.esito_b = sb[i].esito;
        const Divergenza v = punto_divergenza(a[i], b[i]);
        c.divergono = v.divergono;
        c.t_s = v.t_s;
        c.unita = v.unita;
        c.come = v.come;
        c.t_intervento_s = v.t_intervento_s;
        c.dopo_intervento_s = v.dopo_intervento_s;
        c.indice_evento = v.indice_evento;
        if (v.evento_a) c.evento_a = v.evento_a->testo_it.empty() ? v.evento_a->tipo : v.evento_a->testo_it, c.t_evento_a = v.evento_a->tempo;
        if (v.evento_b) c.evento_b = v.evento_b->testo_it.empty() ? v.evento_b->tipo : v.evento_b->testo_it, c.t_evento_b = v.evento_b->tempo;
        d.coppie.push_back(std::move(c));
    }
    return d;
}

void scrivi_confronto(const std::filesystem::path& p, const DatiConfronto& d) {
    std::ofstream f(p);
    if (!f) throw std::runtime_error("confronto: non si scrive " + p.string());
    f << "# il confronto di un esperimento A/B del vetrino di Dio (app/confronto_ab.hpp)\n[confronto]\n";
    f << "titolo = " << stringa(d.titolo) << "\nmostra = " << stringa(d.mostra) << "\nipotesi = " << stringa(d.ipotesi) << "\n";
    f << "missione = " << stringa(d.missione) << "\ncodice = " << stringa(d.codice) << "\ndata = " << stringa(d.data) << "\n";
    f << "da = " << d.da << "\ncoppie = " << d.coppie.size() << "\ncartella_a = " << stringa(d.cartella_a)
      << "\ncartella_b = " << stringa(d.cartella_b) << "\n\n[interventi]\nrighe = [";
    for (std::size_t i = 0; i < d.interventi.size(); ++i) f << (i ? ", " : "") << stringa(d.interventi[i]);
    f << "]\n\n";
    for (const auto& [k, e] : d.confronto.esiti) {
        const auto t = d.testi_esiti.find(k);
        f << "[esiti.\"" << k << "\"]\ntesto = " << stringa(t != d.testi_esiti.end() ? t->second : std::string()) << "\na = " << riga_wilson(e.a)
          << "\nb = " << riga_wilson(e.b) << "\ndifferenza = " << numero(e.differenza) << "\nbasso = " << numero(e.basso)
          << "\nalto = " << numero(e.alto) << "\nsolo_a = " << e.solo_a << "\nsolo_b = " << e.solo_b << "\nentrambe = " << e.entrambe
          << "\nnessuna = " << e.nessuna << "\n\n";
    }
    for (const auto& [parte, m] : d.confronto.parti) {
        f << "[numeri.\"" << parte << "\"]\n";
        for (const auto& [nome, n] : m) f << nome << " = " << riga_numero(n) << "\n";
        f << "\n";
    }
    f << "[generali]\n";
    for (const auto& [nome, n] : d.confronto.generali) f << nome << " = " << riga_numero(n) << "\n";
    for (const auto& c : d.coppie) {
        f << "\n[[coppia]]\nseme = " << c.seme << "\ntraccia_a = " << stringa(c.traccia_a) << "\ntraccia_b = " << stringa(c.traccia_b)
          << "\nesito_a = " << c.esito_a << "\nesito_b = " << c.esito_b << "\ndivergono = " << (c.divergono ? "true" : "false")
          << "\nt_s = " << numero(c.t_s) << "\nunita = " << c.unita << "\ncome = " << stringa(c.come) << "\nt_intervento_s = " << numero(c.t_intervento_s)
          << "\ndopo_intervento_s = " << numero(c.dopo_intervento_s) << "\nindice_evento = " << c.indice_evento
          << "\nevento_a = " << stringa(c.evento_a) << "\nevento_b = " << stringa(c.evento_b) << "\nt_evento_a = " << numero(c.t_evento_a)
          << "\nt_evento_b = " << numero(c.t_evento_b) << "\n";
    }
    if (!f) throw std::runtime_error("confronto: non si scrive " + p.string());
}

DatiConfronto leggi_confronto(const std::filesystem::path& p) {
    toml::table t;
    try {
        t = toml::parse_file(p.string());
    } catch (const std::exception& e) {
        throw std::runtime_error("confronto: " + p.string() + ": " + e.what());
    }
    DatiConfronto d;
    const auto s = [&](const char* k) { return t["confronto"][k].value_or(std::string{}); };
    d.titolo = s("titolo"), d.mostra = s("mostra"), d.ipotesi = s("ipotesi"), d.missione = s("missione"), d.codice = s("codice");
    d.data = s("data");
    d.cartella_a = t["confronto"]["cartella_a"].value_or(std::string("A"));
    d.cartella_b = t["confronto"]["cartella_b"].value_or(std::string("B"));
    d.da = static_cast<std::uint64_t>(t["confronto"]["da"].value_or(std::int64_t{1}));
    if (const toml::array* r = t["interventi"]["righe"].as_array())
        for (const auto& x : *r)
            if (const auto v = x.value<std::string>()) d.interventi.push_back(*v);
    if (const toml::table* es = t["esiti"].as_table())
        for (const auto& [k, v] : *es) {
            const toml::table* e = v.as_table();
            if (!e) continue;
            const int codice = std::stoi(std::string(k.str()));
            ConfrontoEsito c;
            c.a = leggi_wilson((*e)["a"].as_table());
            c.b = leggi_wilson((*e)["b"].as_table());
            c.differenza = (*e)["differenza"].value_or(0.0);
            c.basso = (*e)["basso"].value_or(0.0);
            c.alto = (*e)["alto"].value_or(0.0);
            c.solo_a = static_cast<int>((*e)["solo_a"].value_or(0));
            c.solo_b = static_cast<int>((*e)["solo_b"].value_or(0));
            c.entrambe = static_cast<int>((*e)["entrambe"].value_or(0));
            c.nessuna = static_cast<int>((*e)["nessuna"].value_or(0));
            d.confronto.esiti[codice] = c;
            if (const auto testo = (*e)["testo"].value<std::string>(); testo && !testo->empty()) d.testi_esiti[codice] = *testo;
        }
    if (const toml::table* nu = t["numeri"].as_table())
        for (const auto& [k, v] : *nu)
            if (const toml::table* m = v.as_table())
                for (const auto& [nome, n] : *m)
                    if (const toml::table* x = n.as_table()) d.confronto.parti[std::stoi(std::string(k.str()))][std::string(nome.str())] = leggi_numero(*x);
    if (const toml::table* g = t["generali"].as_table())
        for (const auto& [nome, n] : *g)
            if (const toml::table* x = n.as_table()) d.confronto.generali[std::string(nome.str())] = leggi_numero(*x);
    if (const toml::array* cs = t["coppia"].as_array())
        for (const auto& x : *cs) {
            const toml::table* c = x.as_table();
            if (!c) continue;
            CoppiaConfronto k;
            k.seme = static_cast<std::uint64_t>((*c)["seme"].value_or(std::int64_t{0}));
            k.traccia_a = (*c)["traccia_a"].value_or(std::string{});
            k.traccia_b = (*c)["traccia_b"].value_or(std::string{});
            k.esito_a = static_cast<int>((*c)["esito_a"].value_or(-1));
            k.esito_b = static_cast<int>((*c)["esito_b"].value_or(-1));
            k.divergono = (*c)["divergono"].value_or(false);
            k.t_s = (*c)["t_s"].value_or(-1.0);
            k.unita = static_cast<int>((*c)["unita"].value_or(-1));
            k.come = (*c)["come"].value_or(std::string{});
            k.t_intervento_s = (*c)["t_intervento_s"].value_or(-1.0);
            k.dopo_intervento_s = (*c)["dopo_intervento_s"].value_or(-1.0);
            k.indice_evento = static_cast<int>((*c)["indice_evento"].value_or(-1));
            k.evento_a = (*c)["evento_a"].value_or(std::string{});
            k.evento_b = (*c)["evento_b"].value_or(std::string{});
            k.t_evento_a = (*c)["t_evento_a"].value_or(-1.0);
            k.t_evento_b = (*c)["t_evento_b"].value_or(-1.0);
            d.coppie.push_back(std::move(k));
        }
    d.confronto.coppie = static_cast<int>(d.coppie.size());
    return d;
}

std::string rapporto_confronto(const DatiConfronto& d, const std::vector<std::string>& immagini) {
    std::ostringstream o;
    const auto pct = [](double p) {
        char b[16];
        std::snprintf(b, sizeof b, "%.0f%%", std::fabs(p) < 0.005 ? 0.0 : p * 100.0);
        return std::string(b);
    };
    const auto val = [](double v, const std::string& nome) {
        char b[32];
        std::snprintf(b, sizeof b, "%.1f", v * scala_numero(nome));
        const std::string u = unita_numero(nome);
        return std::string(b) + (u.empty() ? "" : " " + u);
    };
    o << "# " << (d.titolo.empty() ? "Esperimento A/B del vetrino di Dio" : d.titolo) << "\n\n";
    if (!d.mostra.empty()) o << d.mostra << "\n\n";
    o << "- **Missione:** " << d.missione << (d.codice.empty() ? "" : " (codice " + d.codice + ")") << "\n";
    o << "- **Disegno:** a coppie, " << d.coppie.size() << " semi da " << d.da << " a " << d.da + (d.coppie.empty() ? 0 : d.coppie.size() - 1)
      << ". A senza interventi, B con gli interventi: la stessa partita fino al primo intervento.\n";
    if (!d.data.empty()) o << "- **Data:** " << d.data << "\n";
    if (!d.ipotesi.empty()) o << "- **Ipotesi:** " << d.ipotesi << "\n";
    o << "\n## Gli interventi (B)\n\n";
    if (d.interventi.empty()) o << "Nessun intervento applicato nella prima partita di B.\n";
    for (const auto& r : d.interventi) o << "- " << r << "\n";
    o << "\n## Gli esiti\n\nFrequenza in A e in B con l'intervallo di Wilson al 95%; la differenza B - A con l'intervallo di "
         "Newcombe per dati appaiati. Le coppie: l'esito solo in A, solo in B, in entrambe.\n\n";
    o << "| Esito | A | B | B - A (95%) | Solo A | Solo B | Entrambe |\n|---|---|---|---|---|---|---|\n";
    for (const auto& [k, e] : d.confronto.esiti) {
        const auto t = d.testi_esiti.find(k);
        o << "| " << (t != d.testi_esiti.end() ? t->second : k < 0 ? std::string("non finita") : "esito " + std::to_string(k)) << " | "
          << pct(e.a.p) << " (" << pct(e.a.basso) << "-" << pct(e.a.alto) << ") | " << pct(e.b.p) << " (" << pct(e.b.basso) << "-"
          << pct(e.b.alto) << ") | " << (e.differenza >= 0 ? "+" : "") << pct(e.differenza) << " (" << pct(e.basso) << " ; " << pct(e.alto)
          << ")" << (e.significativo() ? " **" : "") << (e.significativo() ? "significativa**" : "") << " | " << e.solo_a << " | " << e.solo_b
          << " | " << e.entrambe << " |\n";
    }
    o << "\n## I numeri per parte\n\nLa mediana in A e in B; la differenza delle mediane con l'intervallo bootstrap sulle coppie "
         "(2000 ricampionamenti, seme fisso); quante coppie salgono, scendono, restano uguali. In grassetto le differenze il cui "
         "intervallo non comprende lo zero, da " << COPPIE_MINIME << " coppie in su.\n";
    if (static_cast<int>(d.coppie.size()) < COPPIE_MINIME)
        o << "\n**Poche coppie (" << d.coppie.size() << "):** con meno di " << COPPIE_MINIME
          << " il bootstrap non distingue le differenze dal caso; i numeri sono indicativi.\n";
    for (const auto& [parte, m] : d.confronto.parti) {
        o << "\n### " << nome_parte_confronto(parte) << "\n\n| Numero | A | B | B - A (95%) | Sale | Scende | Uguale |\n|---|---|---|---|---|---|---|\n";
        for (const auto& [nome, n] : m) {
            if (n.coppie == 0) continue;
            const std::string dif = (n.differenza >= 0 ? "+" : "") + val(n.differenza, nome) + " (" + val(n.basso, nome) + " ; " + val(n.alto, nome) + ")";
            o << "| " << etichetta_numero(nome) << " | " << val(n.mediana_a, nome) << " | " << val(n.mediana_b, nome) << " | "
              << (significativo(n) ? "**" + dif + "**" : dif) << " | " << n.sale << " | " << n.scende << " | " << n.uguale << " |\n";
        }
    }
    o << "\n## Le coppie\n\nIl punto di divergenza: il primo istante in cui la verita' delle due partite si separa, e quanto "
         "dopo il primo intervento; il primo evento del registro che cambia.\n\n";
    o << "| Seme | Esito A | Esito B | Divergenza | Dopo l'intervento | Come | Primo evento diverso (A / B) |\n|---|---|---|---|---|---|---|\n";
    for (const auto& c : d.coppie) {
        const auto testo = [&](int k) {
            const auto t = d.testi_esiti.find(k);
            return t != d.testi_esiti.end() ? t->second : k < 0 ? std::string("-") : std::to_string(k);
        };
        o << "| " << c.seme << " | " << testo(c.esito_a) << " | " << testo(c.esito_b) << " | " << (c.divergono ? orologio(c.t_s) : std::string("mai"))
          << " | " << (c.dopo_intervento_s >= 0 ? std::to_string(static_cast<int>(c.dopo_intervento_s)) + " s" : std::string("-")) << " | "
          << (c.come.empty() ? "-" : c.come) << " | " << (c.evento_a.empty() ? "-" : c.evento_a) << " / " << (c.evento_b.empty() ? "-" : c.evento_b)
          << " |\n";
    }
    if (!immagini.empty()) {
        o << "\n## Le pagine del confronto\n\n";
        for (const auto& i : immagini) o << "![" << i << "](" << i << ")\n\n";
    }
    o << "\n---\nGenerato da melchior688 (app/confronto_ab, nucleo/confronto_serie).\n";
    return o.str();
}

}  // namespace m688
