#include "nucleo/carriera.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <tuple>

#include <toml.hpp>

namespace m688 {

namespace {

constexpr std::array<const char*, 4> AVVERSARI = {"impreparato", "regolare", "veterano", "elite"};
constexpr std::array<const char*, 12> MESI_IT = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio",
                                                 "agosto", "settembre", "ottobre", "novembre", "dicembre"};
constexpr std::array<const char*, 12> MESI_EN = {"January", "February", "March", "April", "May", "June", "July",
                                                 "August", "September", "October", "November", "December"};

double limita(double x, double a, double b) { return std::clamp(x, a, b); }

double valore(const std::map<std::string, double>& m, const char* k, double d) {
    const auto it = m.find(k);
    return it == m.end() ? d : it->second;
}

std::pair<int, int> anno_mese(const std::string& d) {
    int a = 1989, m = 6;
    if (std::sscanf(d.c_str(), "%d-%d", &a, &m) != 2) return {1989, 6};
    return {a, std::clamp(m, 1, 12)};
}

std::string data(int anno, int mese) {
    char b[16];
    std::snprintf(b, sizeof b, "%04d-%02d", anno, mese);
    return b;
}

// "{chiave}" -> valore
std::string riempi(std::string s, const std::map<std::string, std::string>& v) {
    for (const auto& [k, x] : v)
        for (std::size_t p; (p = s.find("{" + k + "}")) != std::string::npos;) s.replace(p, k.size() + 2, x);
    return s;
}

std::string maiuscola(std::string s) {
    if (!s.empty() && s[0] >= 'a' && s[0] <= 'z') s[0] = static_cast<char>(s[0] - 'a' + 'A');
    return s;
}

std::uint32_t fnv1a(const std::string& s) {
    std::uint32_t h = 2166136261u;
    for (const unsigned char c : s) h = (h ^ c) * 16777619u;
    return h;
}

std::string slug(const std::string& s) {
    std::string out;
    for (const unsigned char c : s) {
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) out += static_cast<char>(c);
        else if (c >= 'A' && c <= 'Z') out += static_cast<char>(c - 'A' + 'a');
        else if (!out.empty() && out.back() != '_') out += '_';
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    return out.empty() ? "comandante" : out;
}

std::vector<std::string> stringhe(const toml::node* n) {
    std::vector<std::string> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a) out.push_back(v.value_or(std::string{}));
    return out;
}

std::vector<double> numeri(const toml::node* n) {
    std::vector<double> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a) out.push_back(v.value_or(0.0));
    return out;
}

std::map<std::string, double> tabella_numeri(const toml::node* n) {
    std::map<std::string, double> out;
    if (const auto* t = n ? n->as_table() : nullptr)
        for (auto&& [k, v] : *t)
            if (const auto x = v.value<double>()) out[std::string(k.str())] = *x;
    return out;
}

std::vector<Soglia> soglie(const toml::node* n, const char* campo_punti) {
    std::vector<Soglia> out;
    if (const auto* a = n ? n->as_array() : nullptr)
        for (const auto& v : *a)
            if (const auto* t = v.as_table())
                out.push_back({(*t)[campo_punti].value_or(0.0), (*t)["decorazione"].value_or(std::string{}),
                               (*t)["dispositivo"].value_or(std::string{}), (*t)["condizione"].value_or(std::string{})});
    std::sort(out.begin(), out.end(), [](const Soglia& a, const Soglia& b) { return a.punti < b.punti; });
    return out;
}

}  // namespace

std::string_view nome_situazione(Situazione s) {
    switch (s) {
        case Situazione::pace: return "pace";
        case Situazione::crisi: return "crisi";
        case Situazione::guerra: return "guerra";
    }
    return "pace";
}

std::optional<Situazione> situazione_da_nome(std::string_view s) {
    for (const Situazione x : {Situazione::pace, Situazione::crisi, Situazione::guerra})
        if (s == nome_situazione(x)) return x;
    return std::nullopt;
}

int avversario_da_nome(std::string_view s) {
    for (std::size_t i = 0; i < AVVERSARI.size(); ++i)
        if (s == AVVERSARI[i]) return static_cast<int>(i);
    return -1;
}

const Grado* RegoleCarriera::grado(Marina m, const std::string& id) const {
    const auto it = gradi.find(std::string(nome_marina(m)));
    if (it == gradi.end()) return nullptr;
    for (const auto& g : it->second)
        if (g.id == id) return &g;
    return nullptr;
}

const Decorazione* RegoleCarriera::decorazione(const std::string& id) const {
    const auto it = decorazioni.find(id);
    return it == decorazioni.end() ? nullptr : &it->second;
}

RegoleCarriera carica_carriera(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    RegoleCarriera r;
    const auto reg = t["regole"];
    r.mesi_per_missione = reg["mesi_per_missione"].value_or(r.mesi_per_missione);
    r.mesi_al_comando = reg["mesi_per_missione_al_comando"].value_or(r.mesi_al_comando);
    r.media_su = reg["media_su"].value_or(r.media_su);
    r.missioni_fra_commissioni = reg["missioni_fra_commissioni"].value_or(r.missioni_fra_commissioni);
    r.mancate_per_congedo = reg["mancate_per_congedo"].value_or(r.mancate_per_congedo);
    r.anni_pensione = reg["anni_pensione"].value_or(r.anni_pensione);
    r.soglia_mediocre = reg["soglia_mediocre"].value_or(r.soglia_mediocre);
    r.mediocri_per_trasferimento = reg["mediocri_per_trasferimento"].value_or(r.mediocri_per_trasferimento);
    r.zampolit = reg["zampolit"].value_or(r.zampolit);
    r.rendimenti = numeri(reg["rendimenti"].node());
    r.ripetizione = numeri(reg["ripetizione"].node());
    if (r.rendimenti.empty()) r.rendimenti = {1.0};
    if (r.ripetizione.empty()) r.ripetizione = {1.0};
    r.data_inizio = reg["data_inizio"].value_or(r.data_inizio);
    r.anno_nomina_breve_usn = reg["anno_nomina_breve_usn"].value_or(r.anno_nomina_breve_usn);
    r.anno_nomina_breve_vmf = reg["anno_nomina_breve_vmf"].value_or(r.anno_nomina_breve_vmf);
    r.punteggio = tabella_numeri(t["punteggio"].node());
    r.esposizione = tabella_numeri(t["esposizione"].node());
    r.situazione = tabella_numeri(t["moltiplicatori"]["situazione"].node());
    r.avversario = tabella_numeri(t["moltiplicatori"]["avversario"].node());
    r.realismo_min = t["moltiplicatori"]["realismo_min"].value_or(r.realismo_min);
    if (const auto* gradi = t["gradi"].as_table())
        for (auto&& [m, nodo] : *gradi) {
            auto& v = r.gradi[std::string(m.str())];
            if (const auto* tab = nodo.as_table())
                for (auto&& [id, g] : *tab)
                    v.push_back({std::string(id.str()), g.as_table()->at_path("sigla").value_or(std::string{}),
                                 g.as_table()->at_path("nome").value_or(std::string{}),
                                 g.as_table()->at_path("nome_it").value_or(std::string{}),
                                 g.as_table()->at_path("nato").value_or(std::string{}),
                                 g.as_table()->at_path("ordine").value_or(0)});
            std::sort(v.begin(), v.end(), [](const Grado& a, const Grado& b) { return a.ordine < b.ordine; });
        }
    if (const auto* fasi = t["fasi"].as_table())
        for (auto&& [m, nodo] : *fasi)
            if (const auto* a = nodo.as_array())
                for (const auto& v : *a)
                    if (const auto* f = v.as_table()) {
                        Fase x;
                        x.grado = (*f)["grado"].value_or(std::string{});
                        x.incarico = (*f)["incarico"].value_or(std::string{});
                        x.incarico_en = (*f)["incarico_en"].value_or(std::string{});
                        x.uscita = (*f)["uscita"].value_or(std::string{"missioni"});
                        x.qualifica = (*f)["qualifica"].value_or(std::string{});
                        x.missioni = (*f)["missioni"].value_or(0);
                        x.media_minima = (*f)["media_minima"].value_or(0.0);
                        x.in_zona = (*f)["in_zona"].value_or(0.0);
                        x.sotto_zona = (*f)["sotto_zona"].value_or(0.0);
                        r.fasi[std::string(m.str())].push_back(std::move(x));
                    }
    if (const auto* dec = t["decorazioni"].as_table())
        for (auto&& [id, nodo] : *dec) {
            const auto* d = nodo.as_table();
            if (!d) continue;
            Decorazione x;
            x.id = std::string(id.str());
            x.marina = (*d)["marina"].value_or(std::string{});
            x.nome = (*d)["nome"].value_or(x.id);
            x.nome_it = (*d)["nome_it"].value_or(x.nome);
            x.tipo = (*d)["tipo"].value_or(std::string{"personale"});
            x.precedenza = (*d)["precedenza"].value_or(99);
            if (const auto* righe = (*d)["righe"].as_array())
                for (const auto& riga : *righe)
                    if (const auto* p = riga.as_array(); p && p->size() == 2)
                        x.righe.push_back({(*p)[0].value_or(std::string{}), static_cast<int>((*p)[1].value_or(std::int64_t{0}))});
            x.orizzontale = (*d)["orizzontale"].value_or(false);
            x.motivo = (*d)["motivo"].value_or(std::string{});
            x.con = (*d)["con"].value_or(std::string{});
            x.successiva = (*d)["successiva"].value_or(std::string{});
            x.grado_max = (*d)["grado_max"].value_or(std::string{});
            x.dispositivi = stringhe((*d)["dispositivi"].node());
            x.anni = (*d)["anni"].value_or(0);
            x.confidenza = (*d)["confidenza"].value_or(std::string{});
            x.fonte = (*d)["fonte"].value_or(std::string{});
            r.decorazioni[x.id] = std::move(x);
        }
    if (const auto* s = t["soglie"].as_table())
        for (auto&& [m, nodo] : *s)
            if (const auto* sit = nodo.as_table())
                for (auto&& [nome, lista] : *sit)
                    r.soglie[std::string(m.str()) + "." + std::string(nome.str())] = soglie(&lista, "punti");
    if (const auto* s = t["reparto"].as_table())
        for (auto&& [m, lista] : *s) r.reparto[std::string(m.str())] = soglie(&lista, "punti");
    if (const auto* s = t["fine_comando"].as_table())
        for (auto&& [m, lista] : *s) r.fine_comando[std::string(m.str())] = soglie(&lista, "media");
    if (const auto* s = t["coraggio"].as_table())
        for (auto&& [m, v] : *s) r.coraggio[std::string(m.str())] = v.value_or(std::string{});
    if (const auto* a = t["giudizi"]["livelli"].as_array())
        for (const auto& v : *a)
            if (const auto* l = v.as_table())
                r.livelli.push_back({(*l)["punti"].value_or(0.0), (*l)["it"].value_or(std::string{}), (*l)["en"].value_or(std::string{})});
    std::sort(r.livelli.begin(), r.livelli.end(), [](const auto& a, const auto& b) { return a.punti > b.punti; });
    if (const auto* voci = t["giudizi"]["voci"].as_table())
        for (auto&& [k, v] : *voci)
            if (const auto* x = v.as_table())
                r.voci[std::string(k.str())] = {(*x)["forte_it"].value_or(std::string{}), (*x)["debole_it"].value_or(std::string{}),
                                                (*x)["forte_en"].value_or(std::string{}), (*x)["debole_en"].value_or(std::string{})};
    if (const auto* mot = t["motivazioni"].as_table())
        for (auto&& [m, nodo] : *mot)
            if (const auto* x = nodo.as_table())
                for (auto&& [k, v] : *x) {
                    auto& dest = r.testi[std::string(m.str())];
                    if (const auto* sotto = v.as_table())
                        for (auto&& [k2, v2] : *sotto) dest[std::string(k.str()) + "." + std::string(k2.str())] = v2.value_or(std::string{});
                    else
                        dest[std::string(k.str())] = v.value_or(std::string{});
                }
    if (const auto* f = t["formule"].as_table())
        for (auto&& [k, v] : *f) {
            const auto x = stringhe(&v);
            if (x.size() == 2) r.formule[std::string(k.str())] = {x[0], x[1]};
        }
    if (const auto* o = t["missioni_originali"].as_table())
        for (auto&& [k, v] : *o)
            if (const auto* x = v.as_table())
                r.originali[std::string(k.str())] = {situazione_da_nome((*x)["situazione"].value_or(std::string{})).value_or(Situazione::pace),
                                                     std::max(0, avversario_da_nome((*x)["avversario"].value_or(std::string{"regolare"}))),
                                                     (*x)["difficolta"].value_or(3), (*x)["operazione_speciale"].value_or(false),
                                                     (*x)["artico"].value_or(false), {}};
    if (const auto* o = t["missioni_originali"].as_table())
        for (auto&& [k, v] : *o)
            if (const auto* a = v.as_table() ? (*v.as_table())["esiti_fuoco_senza_ordine"].as_array() : nullptr)
                for (const auto& c : *a) r.originali[std::string(k.str())].esiti_fuoco.push_back(static_cast<int>(c.value_or(std::int64_t{0})));
    for (const char* m : {"usn", "vmf"})
        if (r.fasi[m].empty() || r.gradi[m].empty()) throw std::runtime_error(percorso + ": gradi o fasi mancanti per " + m);
    return r;
}

// --- punteggio (dossier 13 §2.3.1) ---

Punteggio punteggio_missione(const EsitoPerCarriera& e, const ContestoMissione& c, const RegoleCarriera& r) {
    const auto P = [&](const char* k, double d) { return valore(r.punteggio, k, d); };
    Punteggio p;
    p.riuscita = P("riuscita", 400) * limita(e.riuscita, 0, 1);
    const int cl = e.classificazioni_giuste + e.classificazioni_errate;
    const double classificazioni = cl == 0 ? 0.5 : static_cast<double>(e.classificazioni_giuste) / cl;
    const double rapporti = e.rapporti_richiesti == 0 ? 1.0 : std::min(1.0, static_cast<double>(e.rapporti_inviati) / e.rapporti_richiesti);
    p.completezza = P("completezza", 150) * (P("peso_completezza", 0.5) * limita(e.completezza, 0, 1)
                                             + P("peso_classificazioni", 0.3) * classificazioni + P("peso_rapporti", 0.2) * rapporti);
    double esposizione = 0.0;
    for (const auto& [k, n] : e.esposizioni) {
        // le luci di navigazione accese emerso di notte: in pace sono la regola (COLREG), pesano solo in crisi e in guerra
        if (k == "luci" && c.situazione == Situazione::pace) continue;
        esposizione += valore(r.esposizione, k.c_str(), 0.0) * n;
    }
    p.discrezione = P("discrezione", 150) * std::max(0.0, 1.0 - esposizione / std::max(1.0, P("esposizione_piena", 100)));
    const double ordini = e.ordini == 0 ? 1.0 : std::min(1.0, static_cast<double>(e.ordini_eseguiti) / e.ordini);
    const int serie = e.serie_copiate + e.serie_perse;
    const double copiate = serie == 0 ? 1.0 : static_cast<double>(e.serie_copiate) / serie;
    // il fuoco senza provocazione dell'esito della missione (originali senza sala radio) vale come un'infrazione
    const int fuoco = std::max(e.fuoco_senza_ordine, c.fuoco_senza_ordine ? 1 : 0);
    const int infrazioni = std::max(e.infrazioni_roe, c.fuoco_senza_ordine ? 1 : 0);
    // le acque territoriali straniere: in pace e in crisi; nelle operazioni speciali entrarci e' l'ordine
    const bool acque_vietate = c.situazione != Situazione::guerra;
    const int entrate = acque_vietate && !c.operazione_speciale ? e.acque_territoriali : 0;
    const double incidente = acque_vietate && e.scoperto_in_acque ? P("acque_scoperto", 150) * (c.operazione_speciale ? 0.5 : 1.0) : 0.0;
    p.ordini = limita(P("ordini", 100) * (P("peso_ordini", 0.6) * ordini + P("peso_serie", 0.4) * copiate)
                      - P("infrazione_roe", 30) * infrazioni - P("fuoco_senza_ordine_ordini", 50) * fuoco
                      - P("acque_territoriali", 30) * entrate, 0, P("ordini", 100));
    const double colpi = e.lanci == 0 ? P("senza_lanci", 0.7) : std::min(1.0, static_cast<double>(e.colpi) / e.lanci);
    p.efficienza = limita(P("efficienza", 100) * (P("peso_colpi", 0.7) * colpi + P("peso_carico", 0.3) * (1.0 - limita(e.scarto_carico, 0, 1)))
                          - P("arma_incoerente", 10) * e.armi_incoerenti, 0, P("efficienza", 100));
    p.danni = e.battello_perso ? P("battello_perso", -300)
                               : std::max(P("danni_min", -300), -(P("danno_scafo", 150) * limita(e.danni, 0, 1) + P("ferito", 20) * e.feriti
                                                                  + P("morto", 50) * e.morti + P("arma_persa", 10) * e.armi_perse));
    const double violazioni = -(P("neutrale", 150) * e.neutrali_colpiti + P("amico", 200) * e.amici_colpiti
                                + P("fuoco_senza_ordine", 100) * fuoco + P("zona_vietata", 50) * e.zone_vietate + incidente);
    p.squalifica = violazioni <= P("squalifica_sotto", -400);
    p.violazioni = std::max(P("violazioni_min", -200), violazioni);
    p.m_situazione = valore(r.situazione, std::string(nome_situazione(c.situazione)).c_str(), 1.0);
    const int avv = std::clamp(std::min(c.avversario, c.avversario_calcolato), 0, 3);
    p.m_avversario = valore(r.avversario, AVVERSARI[static_cast<std::size_t>(avv)], 1.0);
    p.m_realismo = limita(c.realismo, r.realismo_min, 1.0);
    const double f_siluro = limita(1.0 - e.distanza_min_siluro_m / P("siluro_vicino_m", 5000), 0, 1);
    const double f_asw = limita(e.tempo_contatto_asw_s / P("contatto_asw_s", 1800), 0, 1);
    const double f_collasso = limita(1.0 - e.margine_collasso / P("margine_collasso", 0.2), 0, 1);
    p.pericolo = P("pericolo_max", 0.25) * std::max({f_siluro, f_asw, f_collasso});
    p.moltiplicatore = p.m_situazione * p.m_avversario * p.m_realismo * (1.0 + p.pericolo);
    const double somma = p.riuscita + p.completezza + p.discrezione + p.ordini + p.efficienza + p.danni + p.violazioni;
    p.totale = p.squalifica ? 0.0 : std::max(0.0, somma * p.moltiplicatore);

    // giudizio: il livello, la voce piu' forte e la piu' debole
    std::string livello_it = "insufficiente", livello_en = "unsatisfactory";
    for (const auto& l : r.livelli)
        if (p.totale >= l.punti) { livello_it = l.it; livello_en = l.en; break; }
    const std::array<std::pair<const char*, double>, 5> voci = {{{"riuscita", p.riuscita / P("riuscita", 400)},
                                                                  {"completezza", p.completezza / P("completezza", 150)},
                                                                  {"discrezione", p.discrezione / P("discrezione", 150)},
                                                                  {"ordini", p.ordini / P("ordini", 100)},
                                                                  {"efficienza", p.efficienza / P("efficienza", 100)}}};
    const auto forte = std::max_element(voci.begin(), voci.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
    const auto debole = std::min_element(voci.begin(), voci.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
    std::vector<std::string> it, en;
    if (const auto v = r.voci.find(forte->first); v != r.voci.end() && forte->second >= 0.6) { it.push_back(v->second.forte_it); en.push_back(v->second.forte_en); }
    if (const auto v = r.voci.find(debole->first); v != r.voci.end() && debole->second < 0.6 && debole != forte) { it.push_back(v->second.debole_it); en.push_back(v->second.debole_en); }
    const auto unisci = [](const std::string& livello, const std::vector<std::string>& parti) {
        std::string s = maiuscola(livello);
        for (std::size_t i = 0; i < parti.size(); ++i) s += (i == 0 ? ": " : "; ") + parti[i];
        return s + ".";
    };
    p.giudizio_it = unisci(livello_it, it);
    p.giudizio_en = unisci(livello_en, en);
    return p;
}

// --- carriera ---

Fascicolo Carriera::nuovo(const RegoleCarriera& r, Marina m, const std::string& nome, bool breve, Indulgenza ind, bool aperta) {
    Fascicolo f;
    const std::string mar(nome_marina(m));
    f.nome = nome;
    f.id = slug(nome) + (aperta ? "_aperta" : "");
    f.marina = m;
    f.carriera_breve = breve;
    f.aperta = aperta;
    f.indulgenza = ind;
    f.seme = fnv1a(nome) | 1u;
    f.data = r.data_inizio;
    f.provenienza = m == Marina::usn ? "USNA" : "VVMU «Frunze»";
    const auto& fasi = r.fasi.at(mar);
    const auto& gradi = r.gradi.at(mar);
    const int anno = anno_mese(f.data).first;
    f.anno_nomina = anno;
    if (breve) {   // dal comando: il grado col quarto posto nella scala (O-5, kapitan 2-go ranga)
        const std::string comando = gradi.size() > 4 ? gradi[4].id : gradi.back().id;
        for (std::size_t i = 0; i < fasi.size(); ++i)
            if (fasi[i].grado == comando) f.fase = static_cast<int>(i);
        f.anno_nomina = m == Marina::usn ? r.anno_nomina_breve_usn : r.anno_nomina_breve_vmf;
        const int anni = anno - f.anno_nomina;
        for (int i = 0; i < f.fase; ++i) {   // gli incarichi gia' fatti, spaziati sugli anni di servizio
            const int dal = f.anno_nomina + anni * i / std::max(1, f.fase), al = f.anno_nomina + anni * (i + 1) / std::max(1, f.fase);
            f.incarichi.push_back({fasi[static_cast<std::size_t>(i)].grado, fasi[static_cast<std::size_t>(i)].incarico, "", data(dal, 6), data(al, 5)});
            if (!fasi[static_cast<std::size_t>(i)].qualifica.empty()) f.qualifiche.push_back({fasi[static_cast<std::size_t>(i)].qualifica, data(al, 5)});
        }
        const auto dai = [&](const std::string& d, int quando) {
            if (r.decorazione(d)) f.decorazioni.push_back({d, "", 1, data(quando, 6), "", false, {}, {}, {}});
        };
        if (m == Marina::usn) {   // nominato prima del 1974: la National Defense; schieramenti e un merito da secondo
            if (f.anno_nomina <= 1974) dai("national_defense", f.anno_nomina);
            for (int k = 0; k < 3; ++k) dai("sea_service_deployment", f.anno_nomina + 3 + 4 * k);
            dai("navy_achievement", f.anno_nomina + 6);
            dai("navy_commendation", f.anno_nomina + 13);
        } else {
            dai("distinzione_2", f.anno_nomina + 4);
            if (anno >= 1988) dai("giubilare_70", 1988);
            if (anni >= 10) dai("servizio_impeccabile_3", f.anno_nomina + 10);
        }
    }
    f.grado = fasi[static_cast<std::size_t>(f.fase)].grado;
    f.incarichi.push_back({f.grado, fasi[static_cast<std::size_t>(f.fase)].incarico, "", f.data, ""});
    if (breve) {
        const std::string d = m == Marina::usn ? "command_at_sea" : "comandante_sottomarino";
        f.decorazioni.push_back({d, "", 1, f.data, "", false, {}, {}, {}});
    }
    return f;
}

const Fase* Carriera::fase() const {
    const auto it = regole_->fasi.find(std::string(nome_marina(f_.marina)));
    if (it == regole_->fasi.end() || f_.fase < 0 || f_.fase >= static_cast<int>(it->second.size())) return nullptr;
    return &it->second[static_cast<std::size_t>(f_.fase)];
}

bool Carriera::conta(const ContestoMissione& c) const {
    if (!f_.in_servizio() || c.ripresa) return false;
    if (c.origine == "originale") return true;
    if (c.origine == "rapida") return c.seme != 0 && !c.modificata;
    if (c.origine == "editor" || c.origine == "libreria") return f_.aperta && c.blocco_carriera;
    return false;
}

double Carriera::peso(const ContestoMissione& c) const {
    const auto n = std::count_if(f_.missioni.begin(), f_.missioni.end(), [&](const VoceMissione& v) {
        return v.missione == c.missione && (c.origine != "rapida" || v.seme == c.seme);
    });
    const auto& r = regole_->rendimenti;
    return r[std::min(static_cast<std::size_t>(n), r.size() - 1)];
}

double Carriera::media() const {
    const int n = std::min(static_cast<int>(f_.missioni.size()), regole_->media_su);
    if (n == 0) return 0.0;
    double s = 0.0;
    for (int i = 0; i < n; ++i) s += f_.missioni[f_.missioni.size() - 1 - static_cast<std::size_t>(i)].punti;
    return s / n;
}

int Carriera::possedute(const std::string& d) const {
    return static_cast<int>(std::count_if(f_.decorazioni.begin(), f_.decorazioni.end(), [&](const Conferimento& k) { return k.decorazione == d; }));
}

int Carriera::anni_servizio() const { return anno_mese(f_.data).first - f_.anno_nomina; }

void Carriera::avanza_calendario(int mesi) {
    auto [a, m] = anno_mese(f_.data);
    m += mesi;
    a += (m - 1) / 12;
    m = (m - 1) % 12 + 1;
    f_.data = data(a, m);
}

Conferimento Carriera::conferisci(const std::string& d, const std::string& dispositivo, const ContestoMissione& c,
                                  const EsitoPerCarriera& e, bool postuma, bool con_motivazione) {
    Conferimento k;
    k.decorazione = d;
    k.dispositivo = dispositivo;
    k.numero = possedute(d) + 1;
    k.data = f_.data;
    k.missione = c.missione;
    k.postuma = postuma;
    k.fatti = e.eventi_chiave;
    if (con_motivazione) std::tie(k.motivazione_it, k.motivazione_en) = motivazione(k, c, e);
    f_.decorazioni.push_back(k);
    return k;
}

std::optional<std::string> Carriera::personale(const ContestoMissione& c, const EsitoPerCarriera& e, const Punteggio& p,
                                               double punti, std::string& dispositivo) const {
    if (p.squalifica || p.violazioni < 0) return std::nullopt;   // nessuna decorazione con violazioni (§3.7)
    const std::string mar(nome_marina(f_.marina));
    const auto it = regole_->soglie.find(mar + (c.situazione == Situazione::guerra ? ".guerra" : ".pace"));
    std::optional<std::string> scelta;
    if (it != regole_->soglie.end()) {
        const Grado* mio = grado();
        for (auto s = it->second.rbegin(); s != it->second.rend(); ++s) {
            if (s->condizione == "pericolo" && (p.pericolo < 0.8 * valore(regole_->punteggio, "pericolo_max", 0.25) || e.infrazioni_roe > 0)) continue;
            if (s->condizione == "speciale" && !c.operazione_speciale) continue;
            std::string d = s->decorazione;
            const Decorazione* dec = regole_->decorazione(d);
            if (!dec) continue;
            if (const Grado* g = regole_->grado(f_.marina, dec->grado_max); g && mio && mio->ordine > g->ordine) continue;
            while (dec && possedute(d) > 0 && !dec->successiva.empty()) {   // le classi: dalla 3ª alla 1ª
                d = dec->successiva;
                dec = regole_->decorazione(d);
            }
            const auto& rip = regole_->ripetizione;
            const double fattore = rip[std::min(static_cast<std::size_t>(possedute(d)), rip.size() - 1)];
            if (punti + 1e-9 >= s->punti * fattore) {
                scelta = d;
                dispositivo = s->dispositivo;
                break;
            }
        }
    }
    // incendio o falla domati fuori dalla guerra, o un soccorso riuscito in pace (31/10/2026): la decorazione al
    // coraggio, se e' piu' alta
    if ((e.salvataggio_a_bordo && c.situazione != Situazione::guerra) || (e.soccorso && c.situazione == Situazione::pace))
        if (const auto cr = regole_->coraggio.find(mar); cr != regole_->coraggio.end())
            if (const Decorazione* d = regole_->decorazione(cr->second);
                d && (!scelta || d->precedenza < regole_->decorazione(*scelta)->precedenza)) {
                scelta = d->id;
                dispositivo.clear();
            }
    return scelta;
}

void Carriera::chiudi_carriera(EsitoCarriera& out, const std::string& stato) {
    f_.stato = stato;
    if (!f_.incarichi.empty() && f_.incarichi.back().al.empty()) f_.incarichi.back().al = f_.data;
    out.fine_carriera = true;
}

void Carriera::automatiche(EsitoCarriera& out, const ContestoMissione& c, const EsitoPerCarriera& e) {
    const bool guerra = c.situazione == Situazione::guerra;
    const auto dai = [&](const std::string& d) {
        if (regole_->decorazione(d)) out.automatiche.push_back(conferisci(d, "", c, e, false, false));
    };
    const Grado* g = grado();
    const bool al_comando = g && g->ordine >= 4;
    if (f_.marina == Marina::usn) {
        const auto contate = f_.missioni.size();
        if (contate == 1 || (contate > 1 && (contate - 1) % 3 == 0)) dai("sea_service_deployment");
        if (c.operazione_speciale && !guerra) dai("navy_expeditionary");
        if (c.artico && possedute("arctic_service") == 0) dai("arctic_service");
        if (guerra && e.attaccato && possedute("combat_action_ribbon") == 0) dai("combat_action_ribbon");
        if (guerra && al_comando && possedute("submarine_combat_patrol") == 0) dai("submarine_combat_patrol");
        if (guerra && e.comandante_ferito) dai("purple_heart");
    } else {
        for (const char* d : {"servizio_impeccabile_3", "servizio_impeccabile_2", "servizio_impeccabile_1"})
            if (const Decorazione* x = regole_->decorazione(d); x && anni_servizio() >= x->anni && possedute(d) == 0) dai(d);
    }
    // fine del comando: dopo sei missioni al comando, secondo la media (§3.7 punto 4)
    if (al_comando && !f_.fine_comando_data && f_.missioni_nella_fase >= 6) {
        const auto it = regole_->fine_comando.find(std::string(nome_marina(f_.marina)));
        double s = 0.0;
        int n = 0;
        for (auto v = f_.missioni.rbegin(); v != f_.missioni.rend() && n < f_.missioni_nella_fase; ++v, ++n) s += v->punti;
        const double m = n ? s / n : 0.0;
        if (it != regole_->fine_comando.end())
            for (auto x = it->second.rbegin(); x != it->second.rend(); ++x)
                if (m >= x->punti) {
                    out.automatiche.push_back(conferisci(x->decorazione, "", c, e, false, true));
                    break;
                }
        f_.fine_comando_data = true;
    }
}

void Carriera::commissione(EsitoCarriera& out, const Fase& fa, const ContestoMissione& c, const EsitoPerCarriera& e) {
    const int n = f_.missioni_nella_fase;
    if (f_.prossima_commissione == 0) f_.prossima_commissione = std::max(1, fa.missioni - 1);   // la prima e' sotto zona
    if (n < f_.prossima_commissione) return;
    const bool sotto = n < fa.missioni;
    double m = media();
    if (f_.marina == Marina::vmf) {   // la caratteristica del zampolit: ordini e ROE, non i colpi
        double o = 0.0;
        int k = 0;
        for (auto v = f_.missioni.rbegin(); v != f_.missioni.rend() && k < regole_->media_su; ++v, ++k) o += v->ordini;
        if (k) m *= 1.0 + regole_->zampolit * (2.0 * o / k - 1.0);
    }
    // VMF: promozione anticipata per una decorazione dalla Stella Rossa in su ricevuta nel grado
    bool decorato = false;
    if (f_.marina == Marina::vmf && !f_.incarichi.empty())
        for (const auto& k : f_.decorazioni)
            if (const Decorazione* d = regole_->decorazione(k.decorazione); d && d->tipo == "personale" && d->precedenza <= 6 && k.data >= f_.incarichi.back().dal)
                decorato = true;
    Commissione x{f_.data, f_.grado, {}, m};
    bool promosso = false;
    if (f_.blocco_promozioni > 0) {
        x.esito = "bloccato";
        --f_.blocco_promozioni;
        f_.prossima_commissione = n + regole_->missioni_fra_commissioni;
    } else if (sotto) {
        if (m >= fa.sotto_zona || decorato) { x.esito = "anticipata"; promosso = true; }
        else { x.esito = "non_selezionato"; f_.prossima_commissione = fa.missioni; }   // sotto zona non conta
    } else if (m >= fa.in_zona) {
        x.esito = "selezionato";
        promosso = true;
    } else {
        x.esito = "non_selezionato";
        ++f_.mancate;
        f_.prossima_commissione = n + regole_->missioni_fra_commissioni;
        if (f_.mancate >= regole_->mancate_per_congedo) x.esito = "congedo";   // «up or out»
    }
    f_.commissioni.push_back(x);
    out.commissione = x;
    if (x.esito == "congedo") {
        chiudi_carriera(out, anni_servizio() >= regole_->anni_pensione ? "pensione" : "congedo");
        return;
    }
    if (!promosso) return;
    if (!fa.qualifica.empty() && !f_.ha_qualifica(fa.qualifica)) {
        f_.qualifiche.push_back({fa.qualifica, f_.data});
        out.qualifica = f_.qualifiche.back();
        if (regole_->decorazione(fa.qualifica)) out.automatiche.push_back(conferisci(fa.qualifica, "", c, e, false, false));   // dolphins
    }
    const auto& fasi = regole_->fasi.at(std::string(nome_marina(f_.marina)));
    if (f_.fase + 1 >= static_cast<int>(fasi.size())) return;
    if (!f_.incarichi.empty()) f_.incarichi.back().al = f_.data;
    ++f_.fase;
    const Fase& nuova = fasi[static_cast<std::size_t>(f_.fase)];
    f_.grado = nuova.grado;
    f_.missioni_nella_fase = 0;
    f_.prossima_commissione = 0;
    f_.mancate = 0;
    f_.incarichi.push_back({f_.grado, nuova.incarico, c.battello, f_.data, ""});
    out.promozione = f_.grado;
    if (const Grado* g = grado(); g && g->ordine == 4) {   // il primo comando: il distintivo
        const std::string d = f_.marina == Marina::usn ? "command_at_sea" : "comandante_sottomarino";
        if (possedute(d) == 0) out.automatiche.push_back(conferisci(d, "", c, e, false, false));
    }
}

void Carriera::promuovi(EsitoCarriera& out, const ContestoMissione& c, const EsitoPerCarriera& e) {
    const Fase* fa = fase();
    if (!fa) return;
    ++f_.missioni_nella_fase;
    const int n = f_.missioni_nella_fase;
    Fase come_commissione = *fa;   // le uscite semplici sono una commissione senza soglie
    if (fa->uscita == "fine") return;
    if (fa->uscita == "missioni" || fa->uscita == "qualifica") {
        if (n < fa->missioni) return;
        double s = 0.0;
        const int k = std::min(n, static_cast<int>(f_.missioni.size()));
        for (int i = 0; i < k; ++i) s += f_.missioni[f_.missioni.size() - 1 - static_cast<std::size_t>(i)].punti;
        if (fa->uscita == "qualifica" && k > 0 && s / k < fa->media_minima) return;
        come_commissione.in_zona = come_commissione.sotto_zona = 0.0;
        f_.prossima_commissione = n;
        const auto commissioni = f_.commissioni.size();
        commissione(out, come_commissione, c, e);
        if (f_.commissioni.size() > commissioni) f_.commissioni.pop_back();   // non e' una commissione vera
        out.commissione.reset();
        return;
    }
    commissione(out, *fa, c, e);
}

EsitoCarriera Carriera::chiudi_missione(const ContestoMissione& c, const EsitoPerCarriera& e) {
    EsitoCarriera out;
    out.punteggio = punteggio_missione(e, c, *regole_);
    out.stato = f_.stato;
    if (!conta(c)) return out;
    out.contata = true;
    out.peso = peso(c);
    out.punti = out.punteggio.totale * out.peso;
    const Grado* g = grado();
    avanza_calendario(g && g->ordine >= 4 ? regole_->mesi_al_comando : regole_->mesi_per_missione);

    VoceMissione v;
    v.missione = c.missione;
    v.origine = c.origine;
    v.seme = c.seme;
    v.data = f_.data;
    v.grado = f_.grado;
    v.punteggio = out.punteggio.totale;
    v.peso = out.peso;
    v.punti = out.punti;
    v.ordini = out.punteggio.ordini / std::max(1.0, valore(regole_->punteggio, "ordini", 100));
    v.giudizio_it = out.punteggio.giudizio_it;
    v.giudizio_en = out.punteggio.giudizio_en;
    v.pari = 6 + static_cast<int>(f_.seme % 7);
    v.graduatoria = 1 + static_cast<int>(std::lround((1.0 - limita(out.punti / 1000.0, 0, 1)) * (v.pari - 1)));
    f_.missioni.push_back(v);

    // provvedimenti (§2.3.4): prima la morte, poi le colpe gravi, poi l'inchiesta e i richiami
    const bool usn = f_.marina == Marina::usn;
    const bool morte = e.battello_perso && !e.equipaggio_salvo;
    std::optional<Provvedimento> pr;
    bool fatale = false;
    std::string stato_fine;
    if (morte && f_.indulgenza != Indulgenza::arcade) {
        stato_fine = "morto";
    } else if (e.neutrali_colpiti > 0 || e.amici_colpiti > 0) {
        pr = Provvedimento{f_.data, usn ? (e.infrazioni_roe > 0 ? "corte_marziale" : "rimozione") : "degradazione",
                           e.amici_colpiti > 0 ? "unita' amica colpita" : "unita' neutrale colpita", c.missione};
        fatale = true;
        stato_fine = "rimosso";
    } else if (morte) {   // arcade: il battello perso e' solo un malus
        pr = Provvedimento{f_.data, "malus", "battello perso", c.missione};
    } else if (e.battello_perso) {   // perso con l'equipaggio salvo: commissione d'inchiesta
        const bool colpa = out.punti < regole_->soglia_mediocre || out.punteggio.violazioni < 0;
        pr = Provvedimento{f_.data, colpa ? (usn ? "rimozione" : "congedo") : "inchiesta",
                           colpa ? "battello perso per colpa" : "battello perso per fatalita'", c.missione};
        fatale = colpa;
        if (colpa) stato_fine = usn ? "rimosso" : "congedo";
    } else if (e.incaglio || e.danno_nucleare || c.richiamato || ((e.fuoco_senza_ordine > 0 || c.fuoco_senza_ordine) && c.situazione != Situazione::guerra)
               || (e.scoperto_in_acque && c.situazione != Situazione::guerra && !c.operazione_speciale)) {
        const bool fuoco = e.fuoco_senza_ordine > 0 || c.fuoco_senza_ordine;
        pr = Provvedimento{f_.data, usn ? "richiamo" : "biasimo",
                           e.incaglio ? "incaglio" : e.danno_nucleare ? "danno al reattore" : fuoco ? "fuoco senza ordine"
                           : c.richiamato ? "richiamato dalla missione" : "incidente diplomatico nelle acque territoriali",
                           c.missione};
        f_.blocco_promozioni = 2;
    }
    // esiti mediocri ripetuti: trasferimento a terra
    if (!fatale && stato_fine.empty()) {
        f_.mediocri_di_fila = out.punti < regole_->soglia_mediocre ? f_.mediocri_di_fila + 1 : 0;
        if (f_.mediocri_di_fila >= regole_->mediocri_per_trasferimento) {
            pr = Provvedimento{f_.data, "trasferimento", "esiti mediocri ripetuti", c.missione};
            fatale = true;
            stato_fine = "trasferito";
            f_.mediocri_di_fila = 0;
        }
    }
    if (fatale && f_.indulgenza == Indulgenza::arcade) {   // nessuna rimozione, solo il malus
        pr->tipo = "malus";
        fatale = false;
        stato_fine.clear();
    } else if (fatale && f_.indulgenza == Indulgenza::indulgente && !f_.seconda_possibilita_usata) {
        pr->motivo += " (" + pr->tipo + " sospesa)";
        pr->tipo = "seconda_possibilita";
        f_.seconda_possibilita_usata = true;
        f_.blocco_promozioni = 2;
        fatale = false;
        stato_fine.clear();
    }
    if (pr) {
        f_.provvedimenti.push_back(*pr);
        out.provvedimento = pr;
        if (pr->tipo == "degradazione" && f_.fase > 0) {   // VMF: un grado in meno, poi la rimozione
            --f_.fase;
            f_.grado = regole_->fasi.at("vmf")[static_cast<std::size_t>(f_.fase)].grado;
        }
    }
    const bool malus = pr && pr->tipo == "malus";

    // decorazioni: una personale e una di reparto per missione; postuma solo per un'azione valorosa
    if (!fatale && !malus) {
        std::string disp;
        if (const auto d = personale(c, e, out.punteggio, out.punti, disp); d && (!morte || out.punti >= 600)) {
            out.personale = conferisci(*d, disp, c, e, morte, true);
            if (const Decorazione* x = regole_->decorazione(*d); x && !x->con.empty())   // l'Eroe con l'Ordine di Lenin
                out.automatiche.push_back(conferisci(x->con, "", c, e, morte, false));
        }
        if (!morte && out.punteggio.violazioni >= 0 && !out.punteggio.squalifica && e.morti == 0)
            if (const auto it = regole_->reparto.find(std::string(nome_marina(f_.marina))); it != regole_->reparto.end())
                for (auto s = it->second.rbegin(); s != it->second.rend(); ++s) {
                    if (s->condizione == "guerra_speciale" && c.situazione != Situazione::guerra && !c.operazione_speciale) continue;
                    if (out.punti >= s->punti) {
                        out.reparto = conferisci(s->decorazione, "", c, e, false, true);
                        break;
                    }
                }
    }
    if (!stato_fine.empty() && (fatale || stato_fine == "morto")) {
        chiudi_carriera(out, stato_fine);
    } else {
        automatiche(out, c, e);
        promuovi(out, c, e);
    }
    out.stato = f_.stato;
    return out;
}

std::vector<VoceBarretta> Carriera::barretta() const {
    std::map<std::string, VoceBarretta> per_id;
    for (const auto& k : f_.decorazioni) {
        const Decorazione* d = regole_->decorazione(k.decorazione);
        if (!d || d->tipo == "distintivo" || d->righe.empty()) continue;
        auto& v = per_id[k.decorazione];
        v.decorazione = k.decorazione;
        v.numero = std::max(v.numero, k.numero);
        if (!k.dispositivo.empty()) v.dispositivo = k.dispositivo;
    }
    std::vector<VoceBarretta> out;
    for (auto& [_, v] : per_id) out.push_back(v);
    std::sort(out.begin(), out.end(), [&](const VoceBarretta& a, const VoceBarretta& b) {
        return regole_->decorazione(a.decorazione)->precedenza < regole_->decorazione(b.decorazione)->precedenza;
    });
    return out;
}

// --- motivazioni (dossier 13 §5.1-5.2) ---

std::pair<std::vector<std::string>, std::vector<std::string>> Carriera::motivazione(const Conferimento& k, const ContestoMissione& c,
                                                                                    const EsitoPerCarriera& e) const {
    std::vector<std::string> it, en;
    const Decorazione* d = regole_->decorazione(k.decorazione);
    const auto t = regole_->testi.find(std::string(nome_marina(f_.marina)));
    if (!d || t == regole_->testi.end()) return {it, en};
    const auto& T = t->second;
    const auto testo = [&](const std::string& chiave) {
        const auto x = T.find(chiave);
        return x == T.end() ? std::string{} : x->second;
    };
    const auto f = regole_->formule.find(k.decorazione);
    const std::string da = f == regole_->formule.end() ? "media" : f->second.first;
    std::string formula = f == regole_->formule.end() ? "servizio" : f->second.second;
    if (f_.marina == Marina::vmf && c.operazione_speciale && formula != "merito") formula = "speciale";
    // la decorazione al coraggio per un soccorso in pace: la motivazione dice il salvataggio, sobria
    if (const auto cr = regole_->coraggio.find(std::string(nome_marina(f_.marina)));
        e.soccorso && c.situazione == Situazione::pace && cr != regole_->coraggio.end() && cr->second == k.decorazione)
        formula = "soccorso";
    const Grado* g = grado();
    const Fase* fa = fase();
    const auto [anno, mese] = anno_mese(k.data.empty() ? f_.data : k.data);
    const std::string p_it = k.postuma ? " (" + testo("postuma_it") + ")" : "", p_en = k.postuma ? " (" + testo("postuma_en") + ")" : "";
    std::map<std::string, std::string> v_it = {
        {"da", testo("conferisce_it." + da)}, {"decorazione", d->nome_it}, {"grado", g ? g->nome_it : f_.grado},
        {"nome", f_.nome + p_it}, {"incarico", fa ? fa->incarico : ""}, {"battello", c.battello}, {"teatro", c.teatro},
        {"periodo", std::string(MESI_IT[static_cast<std::size_t>(mese - 1)]) + " " + std::to_string(anno)},
        {"data", std::string(MESI_IT[static_cast<std::size_t>(mese - 1)]) + " " + std::to_string(anno)},
        {"formula", testo("formula_it." + formula)}};
    std::map<std::string, std::string> v_en = {
        {"da", testo("conferisce_en." + da)}, {"decorazione", d->nome}, {"grado", g ? g->nome : f_.grado},
        {"nome", f_.nome + p_en}, {"incarico", fa ? fa->incarico_en : ""}, {"battello", c.battello},
        {"teatro", c.teatro_en.empty() ? c.teatro : c.teatro_en},
        {"periodo", std::string(MESI_EN[static_cast<std::size_t>(mese - 1)]) + " " + std::to_string(anno)},
        {"data", std::string(MESI_EN[static_cast<std::size_t>(mese - 1)]) + " " + std::to_string(anno)},
        {"formula", testo("formula_en." + formula)}};
    const bool reparto = d->tipo == "reparto";
    if (f_.marina == Marina::usn) {
        if (reparto) {
            it.push_back(riempi(testo("reparto_it"), v_it));
            en.push_back(riempi(testo("reparto_en"), v_en));
        } else {
            it.push_back(riempi(testo("apertura_it") + " " + testo("incarico_it"), v_it));
            en.push_back(riempi(testo("apertura_en") + " " + testo("incarico_en"), v_en));
        }
        if (c.operazione_speciale) {   // motivazione volutamente vaga (il caso dell'Halibut)
            it.push_back(testo("segreto_it"));
            en.push_back(testo("segreto_en"));
        } else {
            for (std::size_t i = 0; i < e.fatti_it.size() && i < 4; ++i) it.push_back(e.fatti_it[i]);
            for (std::size_t i = 0; i < e.fatti_en.size() && i < 4; ++i) en.push_back(e.fatti_en[i]);
        }
        if (!reparto) {
            it.push_back(testo("chiusura_it"));
            en.push_back(testo("chiusura_en"));
        }
    } else {
        it.push_back(testo("intestazione_it"));
        en.push_back(testo("intestazione_en"));
        const std::string chiave = reparto ? "reparto" : k.decorazione == "eroe_urss" ? "eroe" : "dispositivo";
        it.push_back(riempi(testo(chiave + "_it"), v_it));
        en.push_back(riempi(testo(chiave + "_en"), v_en));
        it.push_back(riempi(testo("chiusura_it"), v_it));
        en.push_back(riempi(testo("chiusura_en"), v_en));
        if (c.operazione_speciale) {
            it.push_back(testo("segreto_it"));
            en.push_back(testo("segreto_en"));
        }
    }
    return {it, en};
}

}  // namespace m688
