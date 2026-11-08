#include "grafica/libretto.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>

#include "grafica/cerimonia.hpp"
#include "grafica/lamiera.hpp"

#include <stdexcept>
#include <toml.hpp>

namespace m688::libretto {

namespace {

constexpr std::uint8_t INCHIOSTRO = 0, PENNA = 1, ROSSO = 4, GRIGIO = 8, CARTA = 15;

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }
int largo(const Font& f, std::string_view s) { return f.larghezza(s, 1, spazio(f), f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, spazio(f), f.formato() != 2);
}

// lo sprite centrato nel rettangolo, l'ancora sul galleggiamento a `base` pixel dal fondo
void incolla(Tela& t, const Sprite& s, Rettangolo r, int base) {
    const int x0 = r.x + (r.w - s.w) / 2, y0 = r.y + r.h - base - static_cast<int>(std::lround(s.ancora_y));
    for (int y = 0; y < s.h; ++y)
        for (int x = 0; x < s.w; ++x) {
            const std::uint8_t c = s.pix[static_cast<std::size_t>(y * s.w + x)];
            const int px = x0 + x, py = y0 + y;
            if (c != TRASPARENTE && px >= r.x && py >= r.y && px < r.x + r.w && py < r.y + r.h) t.pixel(px, py, c);
        }
}

// il galleggiamento: un filo sottile sotto la sagoma, come nelle tavole
void galleggiamento(Tela& t, Rettangolo r, int base) {
    const int y = r.y + r.h - base;
    for (int x = r.x + 2; x < r.x + r.w - 2; ++x)
        if (x % 3 != 0) t.pixel(x, y, GRIGIO);
}

std::string numero(double v) {
    char b[32];
    std::snprintf(b, sizeof b, "%.0f", v);
    std::string s = b, out;   // le migliaia col punto: 3.500
    for (std::size_t i = 0; i < s.size(); ++i) {
        out += s[i];
        const std::size_t resto = s.size() - 1 - i;
        if (resto && resto % 3 == 0) out += '.';
    }
    return out;
}

std::string elenco(const std::vector<std::string>& v) {
    std::string s;
    for (const auto& x : v) s += (s.empty() ? "" : ", ") + x;
    return s;
}

std::vector<std::string> lista(const toml::node_view<const toml::node>& n) {
    std::vector<std::string> v;
    if (const auto* a = n.as_array())
        for (const auto& x : *a)
            if (const auto s = x.value<std::string>()) v.push_back(*s);
    return v;
}

// il vocabolario «cosa vedo» di tools/pipeline/libretto.py (VEDO), nel suo ordine, con le parole della carta
struct Parola {
    const char* id;
    const char* testo;
};
constexpr Parola VOCABOLARIO[] = {
    {"ponte_volo", "PONTE DI VOLO"}, {"hangar", "HANGAR"},          {"lanciatori", "LANCIATORI"},
    {"torri", "TORRI"},              {"un_fumaiolo", "UN FUMAIOLO"}, {"piu_fumaioli", "PIU' FUMAIOLI"},
    {"nessun_fumaiolo", "NESSUN FUMAIOLO"}, {"radomi", "RADOMI"},    {"gru_bighi", "GRU E BIGHI"},
    {"container", "CONTAINER"},      {"plancia_prua", "PLANCIA A PRUA"}, {"plancia_centro", "PLANCIA AL CENTRO"},
    {"plancia_poppa", "PLANCIA A POPPA"}, {"portali", "PORTALI"},    {"vela", "VELA"},
    {"vela_prua", "VELA A PRUA"},    {"vela_centro", "VELA AL CENTRO"}, {"vela_arretrata", "VELA ARRETRATA"},
    {"vela_alta", "VELA ALTA"},      {"vela_bassa", "VELA BASSA"},   {"vela_raccordata", "VELA RACCORDATA"},
    {"timoni_vela", "TIMONI SULLA VELA"}, {"bulbo_timone", "BULBO SUL TIMONE"},
    {"vela_corta", "VELA CORTA"},    {"vela_lunga", "VELA LUNGA"},   {"vela_squadrata", "VELA SQUADRATA"},
    {"vela_spiovente", "VELA SPIOVENTE A POPPA"},
    {"turboeliche", "TURBOELICHE"},  {"reattori", "REATTORI"},       {"quattro_motori", "QUATTRO MOTORI"},
    {"due_motori", "DUE MOTORI"},    {"ali_a_freccia", "ALI A FRECCIA"}, {"anfibio", "ANFIBIO"},
    {"coda_mad", "CODA DEL MAD"},    {"rotore_singolo", "ROTORE SINGOLO"}, {"rotori_coassiali", "ROTORI COASSIALI"},
    {"rotori_tandem", "ROTORI IN TANDEM"},
};

bool ha(const std::vector<std::string>& v, const std::string& x) { return std::find(v.begin(), v.end(), x) != v.end(); }

// il testo troncato coi puntini se supera `w` pixel
std::string tronca(const Font& f, std::string s, int w) {
    if (largo(f, s) <= w) return s;
    while (!s.empty() && largo(f, s + "..") > w) s.pop_back();
    while (!s.empty() && (s.back() == ' ' || s.back() == '/')) s.pop_back();
    return s + "..";
}

// la carta della pagina: la copertina scura attorno e l'ombra a destra e in basso
void carta(Tela& t, Rettangolo r) {
    t.rettangolo_pieno(r.x + 3, r.y + 3, r.w, r.h, INCHIOSTRO);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, GRIGIO);
    t.rettangolo_pieno(r.x + 2, r.y + 2, r.w - 4, r.h - 4, CARTA);
}

}  // namespace

std::vector<Voce> carica(const std::string& percorso) {
    toml::table lib;
    try {
        lib = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error("libretto: " + percorso + ": " + std::string(e.description()));
    }
    std::vector<Voce> out;
    for (const auto& [chiave, nodo] : lib) {
        const auto* t = nodo.as_table();
        if (!t) continue;
        Voce v;
        v.classe = std::string(chiave.str());
        v.nome = (*t)["nome"].value_or(std::string());
        v.sottotitolo = (*t)["sottotitolo"].value_or(std::string());
        v.sezione = (*t)["sezione"].value_or(std::string());
        v.parte = (*t)["parte"].value_or(std::string());
        v.tipo = (*t)["tipo"].value_or(std::string());
        v.sottomarino = (*t)["sottomarino"].value_or(false);
        v.dislocamento_t = (*t)["dislocamento_t"].value_or(0.0);
        v.lunghezza_m = (*t)["lunghezza_m"].value_or(0.0);
        v.albero_m = (*t)["albero_m"].value_or(0.0);
        v.albero_sigma_m = (*t)["albero_sigma_m"].value_or(0.0);
        v.riferimento_nome = (*t)["riferimento_nome"].value_or(std::string());
        v.riferimento_m = (*t)["riferimento_m"].value_or(0.0);
        v.riferimento_sigma_m = (*t)["riferimento_sigma_m"].value_or(0.0);
        v.segni = (*t)["segni"].value_or(std::string());
        v.armi = lista((*t)["armi"]);
        v.sensori = lista((*t)["sensori"]);
        v.emettitori = lista((*t)["emettitori"]);
        v.emettitori_id = lista((*t)["emettitori_id"]);
        v.vede = lista((*t)["vede"]);
        v.voce_museo = (*t)["voce_museo"].value_or(std::string());
        v.ordine = (*t)["ordine"].value_or(0);
        out.push_back(std::move(v));
    }
    std::stable_sort(out.begin(), out.end(), [](const Voce& a, const Voce& b) { return a.ordine < b.ordine; });   // la tabella e' in ordine alfabetico
    return out;
}

Sprite sagoma(const Modello& m, double aob, double scala_px_m, const std::array<double, 15>& rampa, bool solo_vela) {
    Sprite s = genera_sprite(m, PosaNave{}, aob, true, rampa, scala_px_m);
    // i sottomarini: solo cio' che sta sopra il dorso dello scafo, un terzo dell'altezza (almeno 3 m) sopra il mare
    const double dorso_m = std::max(3.0, 0.35 * m.quota_max_m());
    const int taglio = solo_vela ? static_cast<int>(std::lround(s.ancora_y - dorso_m * scala_px_m)) : s.h;
    std::vector<std::uint8_t> out(s.pix.size(), TRASPARENTE);
    const auto vuoto = [&](int x, int y) {
        return x < 0 || y < 0 || x >= s.w || y >= s.h || y >= taglio || s.pix[static_cast<std::size_t>(y * s.w + x)] == TRASPARENTE;
    };
    for (int y = 0; y < s.h && y < taglio; ++y)
        for (int x = 0; x < s.w; ++x) {
            if (vuoto(x, y)) continue;
            const int k = s.pix[static_cast<std::size_t>(y * s.w + x)] - 144;   // 0 il grigio piu' scuro, 14 il piu' chiaro
            const bool bordo = vuoto(x - 1, y) || vuoto(x + 1, y) || vuoto(x, y - 1) || vuoto(x, y + 1);
            // l'inchiostro pieno; dove la luce batte forte il retino, piu' rado quanto piu' e' chiaro
            const double soglia = k <= 9 ? 1.0 : k <= 11 ? 0.75 : k <= 13 ? 0.5 : 0.25;
            const bool nero = bordo || cerimonia::bayer(x, y) < soglia;
            out[static_cast<std::size_t>(y * s.w + x)] = nero ? INCHIOSTRO : CARTA;
        }
    s.pix = std::move(out);
    if (solo_vela) {   // si tiene solo la larghezza della vela e degli alberi, cosi' la sagoma si centra su di loro
        int x0 = s.w, x1 = -1;
        for (int y = 0; y < s.h; ++y)
            for (int x = 0; x < s.w; ++x)
                if (s.pix[static_cast<std::size_t>(y * s.w + x)] != TRASPARENTE) x0 = std::min(x0, x), x1 = std::max(x1, x);
        if (x1 >= x0) {
            Sprite c = s;
            c.w = x1 - x0 + 1;
            c.pix.assign(static_cast<std::size_t>(c.w * c.h), TRASPARENTE);
            for (int y = 0; y < s.h; ++y)
                for (int x = 0; x < c.w; ++x) c.pix[static_cast<std::size_t>(y * c.w + x)] = s.pix[static_cast<std::size_t>(y * s.w + x0 + x)];
            c.ancora_x = s.ancora_x - static_cast<float>(x0);
            c.ancora_y = static_cast<float>(taglio);   // la vela poggia sulla linea: il dorso fa da galleggiamento
            return c;
        }
    }
    return s;
}

bool pagina(Tela& t, Rettangolo r, const Font& f, const Font& piccolo, const Voce& v, const std::array<const Sprite*, 4>& sagome,
            int numero_pagina, int pagine, const Filtro* filtro) {
    carta(t, r);
    const int x0 = r.x + 7, w = r.w - 14;
    const int pp = piccolo.righe() + 1;
    bool entra = true;
    int y = r.y + 6;
    // la testata: la sezione, il numero della tavola
    const std::string num = std::to_string(numero_pagina) + "/" + std::to_string(pagine);
    if (filtro && filtro->attivo()) {   // la striscia del filtro al posto della sezione
        striscia(t, {x0 - 2, y - 2, w + 4, piccolo.righe() + 3}, piccolo, *filtro, num);
    } else {
        scrivi(t, piccolo, x0, y, v.sezione, INCHIOSTRO);
        scrivi(t, piccolo, x0 + w - largo(piccolo, num), y, num, INCHIOSTRO);
    }
    y += pp + 1;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 4;
    std::string nome = v.nome;   // il nome che non entra si tronca: la pagina non si allarga
    while (nome.size() > 1 && largo(f, nome) > w) nome.pop_back();
    scrivi(t, f, x0, y, nome, INCHIOSTRO);
    y += f.righe() + 2;
    std::string sotto = v.sottotitolo;
    while (sotto.size() > 1 && largo(piccolo, sotto) > w) sotto.pop_back();
    scrivi(t, piccolo, x0, y, sotto, INCHIOSTRO);
    y += pp + 2;
    // il profilo grande, poi le tre viste piccole con l'angolo
    const Rettangolo rp{x0, y, w, 44};
    galleggiamento(t, rp, 4);
    if (sagome[0]) incolla(t, *sagome[0], rp, 4);
    y += rp.h + 2;
    const int cw = w / 3;
    const char* angoli[] = {"0", "45", "135"};
    for (int i = 0; i < 3; ++i) {
        const Rettangolo rv{x0 + i * cw, y, cw - 2, 32};
        galleggiamento(t, rv, 3);
        if (sagome[static_cast<std::size_t>(i + 1)]) incolla(t, *sagome[static_cast<std::size_t>(i + 1)], rv, 3);
        const std::string a = std::string(angoli[i]) + " GR.";
        scrivi(t, piccolo, rv.x + (rv.w - largo(piccolo, a)) / 2, rv.y + rv.h + 1, a, GRIGIO);
    }
    scrivi(t, piccolo, x0, rp.y, "90 GR.", GRIGIO);
    y += 32 + pp + 2;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 4;
    // i numeri: dislocamento e lunghezza; l'albero nel riquadro, perche' e' quello che serve al telemetro
    std::string dati;
    if (v.dislocamento_t > 0) dati += "DISL. " + numero(v.dislocamento_t) + " T   ";
    if (v.lunghezza_m > 0) dati += "LUNGH. " + numero(v.lunghezza_m) + " M";
    scrivi(t, piccolo, x0, y, dati, INCHIOSTRO);
    y += pp + 2;
    char alb[64];
    // l'incertezza col decimale solo se serve, e la virgola: «+- 2», «+- 0,5»
    const auto incertezza = [](double s) {
        if (std::abs(s - std::round(s)) < 0.05) return numero(s);
        char b[16];
        std::snprintf(b, sizeof b, "%.1f", s);
        std::string out = b;
        for (auto& c : out) if (c == '.') c = ',';
        return out;
    };
    // gli aerei e gli elicotteri (dal 31/10/2026): niente albero ne' telemetro, solo i segni per riconoscerli
    if (v.tipo != "aereo" && v.tipo != "elicottero") {
        std::snprintf(alb, sizeof alb, "ALBERO %.0f M +- %s", v.albero_m, incertezza(v.albero_sigma_m).c_str());
        const int wa = largo(f, alb) + 8;
        t.rettangolo_pieno(x0, y, wa, f.righe() + 5, INCHIOSTRO);
        scrivi(t, f, x0 + 4, y + 3, alb, CARTA);
        // sotto, a che cosa serve: e' l'altezza che il telemetro prende quando si identifica questa classe
        y += f.righe() + 7;
        if (v.sovietico) {   // il telemetro a reticolo non ha quadrante: l'altezza va al calcolo, le divisioni le conta l'occhio
            for (const auto& riga : lamiera::a_capo(piccolo, "CON IDENTIFICA VA AL TELEMETRO: CONTA LE DIVISIONI DEL RETICOLO", w))
                scrivi(t, piccolo, x0, y, riga, INCHIOSTRO), y += pp;
        } else if (v.riferimento_nome.empty()) {
            for (const auto& riga : lamiera::a_capo(piccolo, "CON IDENTIFICA VA AL TELEMETRO", w))
                scrivi(t, piccolo, x0, y, riga, INCHIOSTRO), y += pp;
        } else {   // l'albero non sta nel quadrante: al telemetro va il riferimento, nel riquadro a filo
            scrivi(t, piccolo, x0, y, "OLTRE IL QUADRANTE, AL TELEMETRO", INCHIOSTRO);
            y += pp + 1;
            std::snprintf(alb, sizeof alb, "%s %.0f M +- %s", v.riferimento_nome.c_str(), v.riferimento_m, incertezza(v.riferimento_sigma_m).c_str());
            std::string rif = alb;
            while (rif.size() > 1 && largo(piccolo, rif) > w - 8) rif.pop_back();
            const int wr = largo(piccolo, rif) + 8, hr = piccolo.righe() + 5;
            t.linea_orizzontale(x0, x0 + wr - 1, y, INCHIOSTRO), t.linea_orizzontale(x0, x0 + wr - 1, y + hr - 1, INCHIOSTRO);
            t.linea_verticale(x0, y, y + hr - 1, INCHIOSTRO), t.linea_verticale(x0 + wr - 1, y, y + hr - 1, INCHIOSTRO);
            scrivi(t, piccolo, x0 + 4, y + 3, rif, INCHIOSTRO);
            y += hr + 1;
        }
    }
    y += 3;
    // le righe di testo, a capo nella larghezza: l'etichetta in nero, il testo a seguire
    const auto blocco = [&](const std::string& etichetta, const std::string& testo, std::uint8_t c) {
        if (testo.empty()) return;
        bool prima = true;
        for (const auto& riga : lamiera::a_capo(piccolo, etichetta + " " + testo, w)) {
            if (y + pp > r.y + r.h - 16) {
                entra = false;
                return;
            }
            if (prima) {
                scrivi(t, piccolo, x0, y, etichetta, INCHIOSTRO);
                scrivi(t, piccolo, x0 + largo(piccolo, etichetta + " "), y, riga.substr(std::min(riga.size(), etichetta.size() + 1)), c);
            } else {
                scrivi(t, piccolo, x0, y, riga, c);
            }
            prima = false;
            y += pp;
        }
        y += 1;
    };
    blocco("SEGNI:", v.segni, INCHIOSTRO);
    blocco("ARMI:", elenco(v.armi), INCHIOSTRO);
    blocco("SENSORI:", elenco(v.sensori), INCHIOSTRO);
    blocco("RADAR:", elenco(v.emettitori), PENNA);
    // il piede: il rimando al museo e la classifica
    const int yp = r.y + r.h - 13;
    t.linea_orizzontale(x0, x0 + w - 1, yp - 3, GRIGIO);
    if (v.sovietico) {
        scrivi(t, piccolo, x0, yp, "ALBOM SILUETOV", GRIGIO);
    } else if (!v.voce_museo.empty()) {   // «MUSEO: KRIVAK2», senza la sezione, e mai sopra la classifica
        std::string m = "MUSEO: " + v.voce_museo.substr(v.voce_museo.find('.') == std::string::npos ? 0 : v.voce_museo.find('.') + 1);
        for (auto& c : m) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        while (m.size() > 1 && largo(piccolo, m) > w - largo(piccolo, v.classifica) - 6) m.pop_back();
        scrivi(t, piccolo, x0, yp, m, GRIGIO);
    }
    scrivi(t, piccolo, x0 + w - largo(piccolo, v.classifica), yp, v.classifica, ROSSO);
    return entra;
}

void in_stile_sovietico(std::vector<Voce>& voci) {
    // le sezioni del libretto e il loro posto nell'album: prima il nemico
    static const std::pair<const char*, const char*> SEZIONI[] = {
        {"NAVI DA GUERRA - NATO", "KORABLI VMS SSHA I NATO"}, {"SOTTOMARINI - NATO", "PODVODNYE LODKI NATO"},
        {"AEREI ED ELICOTTERI - NATO", "SAMOLETY I VERTOLETY NATO"},
        {"NAVI DA GUERRA - URSS", "KORABLI VMF SSSR"}, {"SOTTOMARINI - URSS", "PODVODNYE LODKI SSSR"},
        {"AEREI ED ELICOTTERI - URSS", "SAMOLETY I VERTOLETY SSSR"},
        {"NAVI CIVILI", "GRAZHDANSKIE SUDA"}};
    constexpr int N = static_cast<int>(std::size(SEZIONI));
    const auto posto = [](const std::string& sezione) {
        for (int i = 0; i < N; ++i)
            if (sezione == SEZIONI[i].first) return i;
        return N;
    };
    std::stable_sort(voci.begin(), voci.end(), [&](const Voce& a, const Voce& b) { return posto(a.sezione) < posto(b.sezione); });
    for (std::size_t i = 0; i < voci.size(); ++i) {
        Voce& v = voci[i];
        if (const int p = posto(v.sezione); p < N) v.sezione = SEZIONI[p].second;
        v.classifica = "SOV. SEKRETNO";
        v.sovietico = true;
        v.ordine = static_cast<int>(i) + 1;
    }
}

std::vector<int> filtra(const std::vector<Voce>& voci, const Filtro& filtro) {
    std::vector<int> out;
    for (std::size_t i = 0; i < voci.size(); ++i) {
        const Voce& v = voci[i];
        if (!filtro.parte.empty() && v.parte != filtro.parte) continue;
        if (!filtro.tipo.empty() && v.tipo != filtro.tipo) continue;
        if (!std::all_of(filtro.vede.begin(), filtro.vede.end(), [&](const std::string& e) { return ha(v.vede, e); })) continue;
        out.push_back(static_cast<int>(i));
    }
    return out;
}

std::vector<std::string> parti(const std::vector<Voce>& voci) {
    std::vector<std::string> out;
    for (const auto& v : voci)
        if (!v.parte.empty() && !ha(out, v.parte)) out.push_back(v.parte);
    return out;
}

std::vector<std::string> tipi(const std::vector<Voce>& voci, const std::string& parte) {
    std::vector<std::string> out;
    for (const auto& v : voci)
        if (!v.tipo.empty() && (parte.empty() || v.parte == parte) && !ha(out, v.tipo)) out.push_back(v.tipo);
    return out;
}

std::vector<std::string> etichette(const std::vector<Voce>& voci) {
    std::vector<std::string> presenti, out;
    for (const auto& v : voci)
        for (const auto& e : v.vede)
            if (!ha(presenti, e)) presenti.push_back(e);
    for (const auto& p : VOCABOLARIO)
        if (ha(presenti, p.id)) out.push_back(p.id);
    std::vector<std::string> altre;   // fuori dal vocabolario: in fondo, in ordine alfabetico
    for (const auto& e : presenti)
        if (!ha(out, e)) altre.push_back(e);
    std::sort(altre.begin(), altre.end());
    out.insert(out.end(), altre.begin(), altre.end());
    return out;
}

std::string testo_maiuscolo(const std::string& s) {
    std::string out = s;
    for (auto& c : out) c = c == '_' ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return out;
}

std::string testo_etichetta(const std::string& etichetta) {
    for (const auto& p : VOCABOLARIO)
        if (etichetta == p.id) return p.testo;
    return testo_maiuscolo(etichetta);
}

int righe_filtro(const std::vector<Voce>& voci) { return 2 + static_cast<int>(etichette(voci).size()); }

Filtro scegli(const std::vector<Voce>& voci, Filtro filtro, int riga, int passo) {
    // TUTTE e' il posto 0, poi le voci dell'elenco; il passo gira in tondo
    const auto scorri = [passo](const std::vector<std::string>& elenco, const std::string& ora) -> std::string {
        if (passo == 0) return {};
        const int n = static_cast<int>(elenco.size()) + 1;
        const auto it = std::find(elenco.begin(), elenco.end(), ora);
        const int i = it == elenco.end() ? 0 : static_cast<int>(it - elenco.begin()) + 1;
        const int j = ((i + passo) % n + n) % n;
        return j == 0 ? std::string() : elenco[static_cast<std::size_t>(j - 1)];
    };
    if (riga == 0) {
        filtro.parte = scorri(parti(voci), filtro.parte);
        if (!filtro.tipo.empty() && !ha(tipi(voci, filtro.parte), filtro.tipo)) filtro.tipo.clear();
    } else if (riga == 1) {
        filtro.tipo = scorri(tipi(voci, filtro.parte), filtro.tipo);
    } else {
        const auto el = etichette(voci);
        const int k = riga - 2;
        if (k >= 0 && k < static_cast<int>(el.size())) {
            const std::string& e = el[static_cast<std::size_t>(k)];
            const auto it = std::find(filtro.vede.begin(), filtro.vede.end(), e);
            if (it == filtro.vede.end()) filtro.vede.push_back(e);
            else filtro.vede.erase(it);
        }
    }
    return filtro;
}

void striscia(Tela& t, Rettangolo r, const Font& piccolo, const Filtro& filtro, const std::string& destra) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, INCHIOSTRO);
    std::string s;
    if (!filtro.parte.empty()) s += testo_etichetta(filtro.parte);
    if (!filtro.tipo.empty()) s += (s.empty() ? "" : " / ") + testo_etichetta(filtro.tipo);
    for (const auto& e : filtro.vede) s += (s.empty() ? "" : " / ") + testo_etichetta(e);
    const int y = r.y + (r.h - piccolo.righe() + 1) / 2;
    const int wd = destra.empty() ? 0 : largo(piccolo, destra) + 6;
    // «FILTRO:» davanti se ci sta tutto; se no la fascia nera basta a dirlo, e il posto va alle scelte
    if (s.empty()) s = "FILTRO: TUTTE LE TAVOLE";
    else if (largo(piccolo, "FILTRO: " + s) <= r.w - 4 - wd) s = "FILTRO: " + s;
    scrivi(t, piccolo, r.x + 2, y, tronca(piccolo, s, r.w - 4 - wd), CARTA);
    if (!destra.empty()) scrivi(t, piccolo, r.x + r.w - 2 - largo(piccolo, destra), y, destra, CARTA);
}

void scheda_filtro(Tela& t, Rettangolo r, const Font& f, const Font& piccolo, const std::vector<Voce>& voci, const Filtro& filtro,
                   int riga, const std::string& aiuto) {
    carta(t, r);
    const int x0 = r.x + 7, w = r.w - 14;
    const int pp = piccolo.righe() + 3;   // una riga della scheda
    int y = r.y + 6;
    // la testata: il titolo e quante tavole passano
    const int passano = static_cast<int>(filtra(voci, filtro).size());
    const std::string quante = std::to_string(passano) + (passano == 1 ? " TAVOLA" : " TAVOLE");
    scrivi(t, f, x0, y, tronca(f, "FILTRO DELLE TAVOLE", w - largo(piccolo, quante) - 6), INCHIOSTRO);
    scrivi(t, piccolo, x0 + w - largo(piccolo, quante), y + f.righe() - piccolo.righe(), quante, passano ? INCHIOSTRO : ROSSO);
    y += f.righe() + 3;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 4;
    // la riga in negativo dove sta il cursore
    const auto fondo = [&](int k, int yy) {
        if (k != riga) return INCHIOSTRO;
        t.rettangolo_pieno(x0 - 2, yy - 2, w + 4, pp, INCHIOSTRO);
        return CARTA;
    };
    const auto scelta = [&](int k, const char* nome, const std::string& valore, const char* tutte) {
        const std::uint8_t c = fondo(k, y);
        scrivi(t, piccolo, x0, y, nome, c);
        const std::string v = "< " + (valore.empty() ? std::string(tutte) : testo_etichetta(valore)) + " >";
        scrivi(t, piccolo, x0 + 44, y, tronca(piccolo, v, w - 44), valore.empty() && c == INCHIOSTRO ? GRIGIO : c);
        y += pp;
    };
    scelta(0, "PARTE", filtro.parte, "TUTTE");
    scelta(1, "TIPO", filtro.tipo, "TUTTI");
    y += 1;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 4;
    // le etichette: la casella, il nome, quante tavole resterebbero con lei. Se non entrano tutte l'elenco scorre col
    // cursore, e un'ultima riga dice quante ne restano sotto
    const auto el = etichette(voci);
    const int fondo_y = r.y + r.h - 6 - (aiuto.empty() ? 0 : 3 * (piccolo.righe() + 1) + 4);
    const int n = static_cast<int>(el.size());
    int posti = (fondo_y - y - pp) / pp, primo = 0;
    if (n > posti) {
        posti = std::max(1, posti - 1);
        primo = std::clamp(riga - 2 - posti / 2, 0, n - posti);
    }
    const int ultimo = std::min(n, primo + posti);
    if (primo > 0) {
        scrivi(t, piccolo, x0, y, "COSA VEDO", GRIGIO);
        const std::string sopra = "ALTRE " + std::to_string(primo) + " SOPRA";
        scrivi(t, piccolo, x0 + w - largo(piccolo, sopra), y, sopra, GRIGIO);
    } else {
        scrivi(t, piccolo, x0, y, "COSA VEDO (TUTTO QUELLO SEGNATO)", GRIGIO);
    }
    y += pp;
    for (int i = primo; i < ultimo; ++i) {
        const auto k = static_cast<std::size_t>(i);
        const bool segnata = ha(filtro.vede, el[k]);
        Filtro prova = filtro;
        if (!segnata) prova.vede.push_back(el[k]);
        const int resta = static_cast<int>(filtra(voci, prova).size());
        std::uint8_t c = fondo(static_cast<int>(k) + 2, y);
        const std::uint8_t testo = c == INCHIOSTRO && resta == 0 ? GRIGIO : c;
        const int lato = piccolo.righe();
        t.linea_orizzontale(x0, x0 + lato - 1, y, c), t.linea_orizzontale(x0, x0 + lato - 1, y + lato - 1, c);
        t.linea_verticale(x0, y, y + lato - 1, c), t.linea_verticale(x0 + lato - 1, y, y + lato - 1, c);
        if (segnata) t.rettangolo_pieno(x0 + 2, y + 2, lato - 4, lato - 4, c == CARTA ? CARTA : PENNA);   // la crocetta a penna
        scrivi(t, piccolo, x0 + lato + 5, y, testo_etichetta(el[k]), testo);
        const std::string quante_qui = std::to_string(resta);
        scrivi(t, piccolo, x0 + w - largo(piccolo, quante_qui), y, quante_qui, testo);
        y += pp;
    }
    if (ultimo < n) {
        const std::string sotto = "ALTRE " + std::to_string(n - ultimo) + " SOTTO";
        scrivi(t, piccolo, x0 + w - largo(piccolo, sotto), y, sotto, GRIGIO);
    }
    // il piede: i tasti, a capo nella larghezza, al massimo tre righe
    if (!aiuto.empty()) {
        const auto righe = lamiera::a_capo(piccolo, aiuto, w);
        const int n_righe = std::min<int>(3, static_cast<int>(righe.size()));
        int yp = r.y + r.h - 6 - n_righe * (piccolo.righe() + 1);
        t.linea_orizzontale(x0, x0 + w - 1, yp - 3, GRIGIO);
        int scritte = 0;
        for (const auto& rr : righe) {
            if (scritte++ == n_righe) break;
            scrivi(t, piccolo, x0, yp, rr, GRIGIO);
            yp += piccolo.righe() + 1;
        }
    }
}

namespace {

// il colore EGA di una luce sul nero della notte
std::uint8_t colore_luce(luci::Colore c) {
    switch (c) {
        case luci::Colore::rosso: return 12;
        case luci::Colore::verde: return 10;
        case luci::Colore::giallo:
        case luci::Colore::ambra: return 14;
        case luci::Colore::luci_coperta: return 7;
        default: return 15;
    }
}

enum class Vista { prora, dritta, poppa, angolo };

// Uno schema: il riquadro di notte con lo scafo, gli alberi e i fanali accesi visti da quella parte. Con Vista::angolo
// chi guarda sta a `aob` gradi dalla prua (positivi a dritta): la nave si vede di scorcio
void schema(Tela& t, Rettangolo b, const luci::Configurazione& c, luci::Stato stato, Vista vista, double tempo_s, double aob = 0.0) {
    t.rettangolo_pieno(b.x, b.y, b.w, b.h, INCHIOSTRO);
    // le misure dello scafo dai fanali: da poppa (coronamento) a prua (fanale di fonda), la coperta, la larghezza
    // (e la lunghezza della classe: i sottomarini non hanno fanali a prua)
    double x0 = -c.lunghezza_m / 2, x1 = c.lunghezza_m / 2, coperta = 2, mezza = 2, alto = 4;
    for (const auto& f : c.fanali) {
        x0 = std::min(x0, f.pos_m[0]), x1 = std::max(x1, f.pos_m[0]);
        mezza = std::max(mezza, std::abs(f.pos_m[1]));
        alto = std::max(alto, f.pos_m[2]);
        if (f.ruolo == "coronamento") coperta = std::max(1.5, f.pos_m[2] - 1.0);
    }
    // l'asse orizzontale di chi guarda, nel riferimento della nave: u = x sin(aob) + y cos(aob) (di prora la sinistra
    // della nave e' a destra di chi guarda, di dritta la prua e' a destra)
    const double rel_vista = vista == Vista::prora ? 0.0 : vista == Vista::dritta ? 90.0 : vista == Vista::poppa ? 180.0 : aob;
    const double su = std::sin(rel_vista * PI / 180.0), cu = std::cos(rel_vista * PI / 180.0);
    // di scorcio la scala e' quella del fianco, uguale per tutti gli angoli: si vede lo scarto fra gli alberi crescere
    const double lungo = vista == Vista::angolo ? x1 - x0 : std::max(std::abs(su) * (x1 - x0), 2 * mezza);
    const double s = std::min((b.w - 12) / std::max(lungo, 1.0), (b.h - 10) / std::max(alto, 1.0));
    const int base = b.y + b.h - 5, cx = b.x + b.w / 2;
    // dove cade un punto del modello: di fianco la prua a destra; di prora la sinistra della nave a destra di chi guarda
    const auto px = [&](const std::array<double, 3>& p) {
        const double u = (p[0] - (x0 + x1) / 2) * su + p[1] * cu;
        return cx + static_cast<int>(std::lround(u * s));
    };
    const auto py = [&](double z) { return base - static_cast<int>(std::lround(z * s)); };
    t.linea_orizzontale(b.x + 1, b.x + b.w - 2, base, PENNA);   // il mare
    // lo scafo in grigio scuro, e gli alberi fino ai fanali alti
    {   // lo scafo: la sua impronta vista da chi guarda
        const int a = std::min({px({x0, mezza, 0}), px({x0, -mezza, 0}), px({x1, mezza, 0}), px({x1, -mezza, 0})});
        const int b2 = std::max({px({x0, mezza, 0}), px({x0, -mezza, 0}), px({x1, mezza, 0}), px({x1, -mezza, 0})});
        t.rettangolo_pieno(a, py(coperta), std::max(2, b2 - a), base - py(coperta), GRIGIO);
    }
    for (const auto& f : c.fanali) {
        if (f.ruolo == "albero" || f.ruolo == "albero_poppiero" || f.ruolo == "pesca_alto" || f.ruolo == "faro_ambra")
            t.linea_verticale(px({f.pos_m[0], 0, 0}), py(f.pos_m[2]), py(coperta), GRIGIO);
        if (c.tipo == "sottomarino" && f.ruolo == "albero" && std::abs(su) > 0.5) {   // la vela
            const int a = px({f.pos_m[0] - 4, 0, 0}), b2 = px({f.pos_m[0] + 4, 0, 0});
            t.rettangolo_pieno(a, py(f.pos_m[2] - 0.3), std::max(2, b2 - a), py(coperta) - py(f.pos_m[2] - 0.3), GRIGIO);
        }
    }
    // i fanali accesi in quello stato e visibili da questo rilevamento relativo
    const double rel = rel_vista;
    struct Punto { int x, y; double z; std::uint8_t col; bool coperta; };
    std::vector<Punto> punti;
    for (const luci::Fanale* f : luci::accesi(c, stato, tempo_s))
        if (luci::nel_settore(*f, rel))
            punti.push_back({px(f->pos_m), py(f->pos_m[2]), f->pos_m[2], colore_luce(f->colore), f->colore == luci::Colore::luci_coperta});
    // i fanali uno sopra l'altro (rosso-bianco-rosso, verde su bianco) a questa scala si toccano: dall'alto in giu',
    // ognuno almeno 4 pixel sotto quello che ha sopra nella stessa colonna, come negli schemi del regolamento
    std::sort(punti.begin(), punti.end(), [](const Punto& p1, const Punto& p2) { return p1.z > p2.z; });   // dal piu' alto
    for (std::size_t i = 0; i < punti.size(); ++i)
        for (std::size_t j = 0; j < i; ++j)
            if (!punti[i].coperta && !punti[j].coperta && std::abs(punti[i].x - punti[j].x) <= 2 && punti[i].y - punti[j].y < 4 &&
                punti[i].y - punti[j].y > -4)
                punti[i].y = punti[j].y + 4;
    for (const auto& p : punti) {
        if (p.coperta) {
            t.pixel(p.x, p.y, p.col);
            continue;
        }
        t.rettangolo_pieno(p.x - 1, p.y - 1, 3, 3, p.col);   // il punto, col nucleo bianco per le luci colorate
        if (p.col != 15) t.pixel(p.x, p.y, 15);
    }
}

}  // namespace

bool pagina_luci(Tela& t, Rettangolo r, const Font& f, const Font& piccolo, const std::map<std::string, luci::Configurazione>& configurazioni,
                 int numero_pagina, int pagine) {
    carta(t, r);
    const int x0 = r.x + 7, w = r.w - 14;
    const int pp = piccolo.righe() + 1;
    bool tutte = true;
    int y = r.y + 6;
    const std::string num = std::to_string(numero_pagina) + "/" + std::to_string(pagine);
    scrivi(t, piccolo, x0, y, "LUCI DI NAVIGAZIONE", INCHIOSTRO);
    scrivi(t, piccolo, x0 + w - largo(piccolo, num), y, num, INCHIOSTRO);
    y += pp + 1;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 3;
    scrivi(t, f, x0, y, tronca(f, "COLREG 1972", w), INCHIOSTRO);
    y += f.righe() + 3;
    struct Schema {
        const char* classe;
        luci::Stato stato;
        Vista vista;
        const char* titolo;
        const char* testo;
    };
    const Schema schemi[] = {
        {"mercantile", luci::Stato::moto, Vista::prora, "DA PRORA", "VERDE E ROSSO: VIENE A TE"},
        {"mercantile", luci::Stato::moto, Vista::dritta, "DA DRITTA", "VERDE; BIANCO BASSO A PRUA"},
        {"mercantile", luci::Stato::moto, Vista::poppa, "DA POPPA", "UN BIANCO SOLO, BASSO"},
        {"mercantile", luci::Stato::fonda, Vista::dritta, "ALLA FONDA", "ALTO A PRUA, BASSO A POPPA"},
        {"peschereccio", luci::Stato::pesca, Vista::dritta, "IN PESCA", "VERDE SU BIANCO (ROSSO: ALTRE)"},
        {"mercantile", luci::Stato::manovra_limitata, Vista::dritta, "MANOVRA LIMITATA", "ROSSO, BIANCO, ROSSO"},
        {"mercantile", luci::Stato::non_governa, Vista::dritta, "SENZA GOVERNO", "DUE ROSSI: NON GOVERNA"},
        {"los_angeles", luci::Stato::moto, Vista::dritta, "SOTTOMARINO USA", "AMBRA: 3 LAMPI, 3 S DI BUIO"},
    };
    const int colonna = (w - 6) / 2, alto_box = 34;
    const int riga = pp + alto_box + 2 * pp + 6;
    for (std::size_t k = 0; k < std::size(schemi); ++k) {
        const auto& v = schemi[k];
        const int bx = x0 + static_cast<int>(k % 2) * (colonna + 6), by = y + static_cast<int>(k / 2) * riga;
        scrivi(t, piccolo, bx, by, tronca(piccolo, v.titolo, colonna), INCHIOSTRO);
        const Rettangolo b{bx, by + pp, colonna, alto_box};
        const auto it = configurazioni.find(v.classe);
        if (it == configurazioni.end()) {
            tutte = false;
            t.rettangolo_pieno(b.x, b.y, b.w, b.h, GRIGIO);
        } else {
            schema(t, b, it->second, v.stato, v.vista, 0.2);   // a 0,2 s il faro ambra e' acceso
        }
        int yy = b.y + b.h + 2;
        int n = 0;
        for (const auto& rr : lamiera::a_capo(piccolo, v.testo, colonna)) {
            if (n++ == 2) break;
            scrivi(t, piccolo, bx, yy, rr, INCHIOSTRO);
            yy += pp;
        }
    }
    // il piede: la regola e da dove vengono gli schemi
    const int yp = r.y + r.h - 13;
    t.linea_orizzontale(x0, x0 + w - 1, yp - 3, GRIGIO);
    scrivi(t, piccolo, x0, yp, tronca(piccolo, "COLREG, REGOLE 21-30", w), GRIGIO);
    return tutte;
}

bool pagina_luci_aob(Tela& t, Rettangolo r, const Font& f, const Font& piccolo,
                     const std::map<std::string, luci::Configurazione>& configurazioni, int numero_pagina, int pagine) {
    carta(t, r);
    const int x0 = r.x + 7, w = r.w - 14;
    const int pp = piccolo.righe() + 1;
    int y = r.y + 6;
    const std::string num = std::to_string(numero_pagina) + "/" + std::to_string(pagine);
    scrivi(t, piccolo, x0, y, "LUCI DI NAVIGAZIONE", INCHIOSTRO);
    scrivi(t, piccolo, x0 + w - largo(piccolo, num), y, num, INCHIOSTRO);
    y += pp + 1;
    t.linea_orizzontale(x0, x0 + w - 1, y, INCHIOSTRO);
    y += 3;
    scrivi(t, f, x0, y, tronca(f, "AOB DALLE LUCI", w), INCHIOSTRO);
    y += f.righe() + 4;
    // i laterali: quale colore si vede, da che parte guarda la nave
    struct Riga {
        std::vector<std::uint8_t> punti;
        const char* testo;
    };
    const Riga righe[] = {{{10}, "SOLO VERDE: AOB A DRITTA"},
                          {{12}, "SOLO ROSSO: AOB A SINISTRA"},
                          {{12, 10}, "ROSSO E VERDE: DI PRORA"},
                          {{15}, "UN BIANCO SOLO: DI POPPA"}};
    for (const auto& rr : righe) {
        t.rettangolo_pieno(x0, y - 1, 16, pp + 1, INCHIOSTRO);
        int px = x0 + 4;
        for (const auto col : rr.punti) {
            t.rettangolo_pieno(px, y + pp / 2 - 2, 3, 3, col);
            px += 6;
        }
        scrivi(t, piccolo, x0 + 20, y, tronca(piccolo, rr.testo, w - 20), INCHIOSTRO);
        y += pp + 3;
    }
    y += 2;
    t.linea_orizzontale(x0, x0 + w - 1, y, GRIGIO);
    y += 4;
    // i due bianchi d'albero del mercantile, visti a 0, 20, 45 e 90 gradi a dritta
    bool c_e = true;
    scrivi(t, piccolo, x0, y, tronca(piccolo, "I DUE BIANCHI D'ALBERO, A DRITTA", w), INCHIOSTRO);
    y += pp + 2;
    const int colonna = (w - 6) / 2, alto_box = 34;
    const double angoli[] = {0.0, 20.0, 45.0, 90.0};
    const char* didascalie[] = {"0: IN LINEA", "20 DR: VICINI", "45 DR: APERTI", "90 DR: LONTANI"};
    const auto it = configurazioni.find("mercantile");
    for (int k = 0; k < 4; ++k) {
        const int bx = x0 + (k % 2) * (colonna + 6), by = y + (k / 2) * (alto_box + pp + 6);
        const Rettangolo b{bx, by, colonna, alto_box};
        if (it == configurazioni.end()) {
            c_e = false;
            t.rettangolo_pieno(b.x, b.y, b.w, b.h, GRIGIO);
        } else {
            schema(t, b, it->second, luci::Stato::moto, Vista::angolo, 0.0, angoli[k]);
        }
        scrivi(t, piccolo, bx, by + alto_box + 2, tronca(piccolo, didascalie[k], colonna), INCHIOSTRO);
    }
    y += 2 * (alto_box + pp + 6) + 2;
    // la regola: lo scarto fra i due bianchi su quanto distano gli alberi
    double l = 0.0;
    if (it != configurazioni.end()) {
        double xa = 0, xp = 0;
        bool a = false, p = false;
        for (const auto& fa : it->second.fanali) {
            if (fa.ruolo == "albero") xa = fa.pos_m[0], a = true;
            if (fa.ruolo == "albero_poppiero") xp = fa.pos_m[0], p = true;
        }
        if (a && p) l = std::abs(xa - xp);
    }
    char riga[96];
    std::snprintf(riga, sizeof riga, "AOB = ARCSEN(D / L): D LO SCARTO FRA I DUE BIANCHI, L FRA GLI ALBERI (MERCANTILE %.0f M).",
                  l > 0 ? l : 100.0);
    std::string testo = riga;
    testo += " IL BIANCO BASSO E' A PRUA: DA QUELLA PARTE VA LA NAVE.";
    for (const auto& rr : lamiera::a_capo(piccolo, testo, w)) {
        if (y + pp > r.y + r.h - 16) break;
        scrivi(t, piccolo, x0, y, rr, INCHIOSTRO);
        y += pp;
    }
    const int yp = r.y + r.h - 13;
    t.linea_orizzontale(x0, x0 + w - 1, yp - 3, GRIGIO);
    scrivi(t, piccolo, x0, yp, tronca(piccolo, "Z X L'AOB, SPAZIO ALLA TMA", w), GRIGIO);
    return c_e;
}

}  // namespace m688::libretto
