#include "nucleo/armi_ia.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>

#include <toml.hpp>

namespace m688 {

namespace {

std::string s(const toml::table& t, const char* k) { return t[k].value_or(std::string{}); }

bool in_servizio(const SchedaArma& a, int anno) {
    return a.in_servizio_dal > 0 && a.in_servizio_dal <= anno && (a.ritirata_nel == 0 || anno < a.ritirata_nel);
}
bool caricabile(const SchedaArma& a) { return !a.what_if && !a.nucleare; }

constexpr double PORTATA_UTILE = 0.9;        // un'arma «arriva» se la distanza sta sotto il 90% della portata
constexpr double PRIMI_ANNI = 2.0;           // il tetto massimo_primo_anno vale nei primi due anni di servizio

}  // namespace

CatalogoArmiIa carica_armi_ia(const std::string& percorso) {
    const toml::table t = toml::parse_file(percorso);
    CatalogoArmiIa c;
    if (const auto* tab = t["armi_ia"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) {
                DatiArmiIa d;
                d.classe = std::string(k.str());
                d.nazione = s(*r, "nazione");
                d.tubi_533 = (*r)["tubi_533"].value_or(0);
                d.tubi_650 = (*r)["tubi_650"].value_or(0);
                if (const auto* a = (*r)["armi"].as_table())
                    for (const auto& [v, x] : *a) d.armi[std::string(v.str())] = x.value_or(0);
                if (const auto* a = (*r)["tubi"].as_array())
                    for (const auto& x : *a) d.tubi.push_back(x.value_or(std::string{}));
                d.fonte = s(*r, "fonte");
                d.confidenza = s(*r, "confidenza");
                c.classi[d.classe] = std::move(d);
            }
    if (const auto* tab = t["filo_ia"].as_table())
        for (const auto& [k, n] : *tab)
            if (const auto* r = n.as_table()) {
                DottrinaFilo d;
                d.correzione_s = (*r)["correzione_s"].value_or(d.correzione_s);
                d.soglia_deg = (*r)["soglia_deg"].value_or(d.soglia_deg);
                d.taglio_dopo_aggancio_s = (*r)["taglio_dopo_aggancio_s"].value_or(d.taglio_dopo_aggancio_s);
                d.distanza_taglio_m = (*r)["distanza_taglio_m"].value_or(d.distanza_taglio_m);
                d.rifiuta_esche = (*r)["rifiuta_esche"].value_or(d.rifiuta_esche);
                d.scarto_esca_deg = (*r)["scarto_esca_deg"].value_or(d.scarto_esca_deg);
                d.attivo_su_perdita = (*r)["attivo_su_perdita"].value_or(d.attivo_su_perdita);
                d.fonte = s(*r, "fonte");
                d.confidenza = s(*r, "confidenza");
                c.filo[std::string(k.str())] = d;
            }
    for (const char* e : {"recluta", "medio", "esperto"})
        if (!c.filo.count(e)) throw std::runtime_error(percorso + ": manca [filo_ia." + e + "]");
    return c;
}

const DottrinaFilo& dottrina_filo(const CatalogoArmiIa& c, Esperienza e) { return c.filo.at(nome_esperienza(e)); }

CatalogoInterazioni carica_interazioni(const std::string& percorso) { return {carica_tma(percorso), carica_armi_ia(percorso)}; }

int CaricoIa::totale(const std::string& variante) const {
    const auto it = riserva.find(variante);
    return static_cast<int>(std::count(tubi.begin(), tubi.end(), variante)) + (it == riserva.end() ? 0 : it->second);
}

std::string variante_dell_anno(const CatalogoCarico& cat, const std::string& variante, const std::string& nazione, int anno) {
    const SchedaArma* a = cat.arma(variante);
    if (!a || !caricabile(*a)) return {};
    if (in_servizio(*a, anno)) return variante;
    // la predefinita della famiglia per la nazione, poi la piu' recente in servizio della stessa famiglia: della
    // nazione, altrimenti di un'altra (il Trafalgar prima del Tigerfish Mod 1 col Mk 48)
    if (const std::string p = cat.predefinita(a->famiglia, nazione); !p.empty() && p != variante)
        if (const SchedaArma* b = cat.arma(p); b && caricabile(*b) && in_servizio(*b, anno)) return p;
    std::string migliore;
    int anno_migliore = -1;
    bool stessa = false;
    for (const auto& [id, b] : cat.armi) {
        if (b.famiglia != a->famiglia || b.tipo != a->tipo || !caricabile(b) || !in_servizio(b, anno)) continue;
        const bool nostra = b.nazione == nazione;
        if ((nostra && !stessa) || (nostra == stessa && b.in_servizio_dal > anno_migliore)) {
            migliore = id;
            anno_migliore = b.in_servizio_dal;
            stessa = nostra;
        }
    }
    return migliore;
}

CaricoIa carico_ia(const CatalogoCarico& cat, const CatalogoArmiIa& ia, const std::string& classe, int anno,
                   const std::string& tipo_missione) {
    CaricoIa out;
    std::string nazione;
    std::vector<std::string> tubi;
    std::map<std::string, int> riserva;
    if (const LocaleSiluri* l = cat.locale(classe); l && cat.carichi.count(l->id)) {
        const auto& per_tipo = cat.carichi.at(l->id);
        std::string tipo = tipo_missione;
        if (!per_tipo.count(tipo)) {
            const auto p = cat.predefiniti.find(l->id);
            tipo = p != cat.predefiniti.end() ? p->second : std::string{};
        }
        if (!per_tipo.count(tipo)) return out;
        const CaricoStandard& cs = per_tipo.at(tipo);
        nazione = l->nazione;
        tubi = cs.tubi;
        riserva = cs.rastrelliera;
        for (const auto& [v, n] : cs.pozzi) riserva[v] += n;
        out.fonte = "carico.toml [carichi." + l->id + "." + tipo + "]";
    } else if (const auto it = ia.classi.find(classe); it != ia.classi.end()) {
        const DatiArmiIa& d = it->second;
        nazione = d.nazione;
        tubi = d.tubi;
        riserva = d.armi;
        for (const auto& v : d.tubi)   // [armi_ia] conta anche le armi nei tubi
            if (auto r = riserva.find(v); r != riserva.end() && r->second > 0) --r->second;
        out.tubi_650_vuoti = d.tubi_650;
        out.fonte = "interazioni.toml [armi_ia." + classe + "]: " + d.fonte;
    } else {
        return out;
    }
    // le varianti dell'anno, col tetto dei primi anni (l'ADCAP nel 1989: al piu' 6)
    std::map<std::string, int> usate;
    const auto dell_anno = [&](const std::string& v) {
        std::string w = variante_dell_anno(cat, v, nazione, anno);
        if (const SchedaArma* a = cat.arma(w); a && a->massimo_primo_anno > 0 && anno < a->in_servizio_dal + PRIMI_ANNI &&
                                               usate[w] >= a->massimo_primo_anno) {
            // oltre il tetto: la variante precedente della famiglia
            w = variante_dell_anno(cat, cat.predefinita(a->famiglia, nazione), nazione, anno);
        }
        if (!w.empty()) ++usate[w];
        return w;
    };
    for (const auto& v : tubi) out.tubi.push_back(v.empty() ? std::string{} : dell_anno(v));
    for (const auto& [v, n] : riserva)
        for (int k = 0; k < n; ++k)
            if (const std::string w = dell_anno(v); !w.empty()) ++out.riserva[w];
    return out;
}

std::optional<SceltaArmaIa> scegli_arma_ia(const CaricoIa& carico, const CatalogoCarico& cat, BersaglioIa tipo,
                                           double distanza_m, bool filo_possibile) {
    const char* contro = tipo == BersaglioIa::sottomarino ? "sottomarino" : "superficie";
    std::optional<SceltaArmaIa> out;
    std::tuple<bool, int, bool, double> meglio{false, 0, false, 0.0};
    for (std::size_t i = 0; i < carico.tubi.size(); ++i) {
        const SchedaArma* a = cat.arma(carico.tubi[i]);
        if (!a || a->stelle(contro) <= 0) continue;
        const double portata = a->portata_km * 1000.0;
        const bool in_portata = portata <= 0.0 || distanza_m <= PORTATA_UTILE * portata;
        const bool filo = a->guida.find("filo") != std::string::npos;
        const std::tuple<bool, int, bool, double> chiave{in_portata, a->stelle(contro), filo && filo_possibile, portata};
        if (!out || chiave > meglio) {
            meglio = chiave;
            out = SceltaArmaIa{a->id, a->siluro, a->tipo, a->famiglia, static_cast<int>(i), in_portata, filo};
        }
    }
    return out;
}

std::optional<Vec2> GuidaFiloIa::mira(const SituazioneFilo& s) const {
    if (s.bersaglio_stimato) return s.bersaglio_stimato;
    if (!s.rilevamento || !s.siluro) return std::nullopt;
    // sulla linea del rilevamento, alla distanza dal lanciatore a cui sara' il siluro fra una correzione: il siluro
    // stringe verso la linea senza tornare indietro
    const double avanti = distanza(s.pos_lanciatore, s.siluro->pos()) + s.siluro->velocita() * d_.correzione_s;
    const double r = radianti(*s.rilevamento);
    return Vec2{s.pos_lanciatore.x + avanti * std::sin(r), s.pos_lanciatore.y + avanti * std::cos(r)};
}

std::vector<ComandoFilo> GuidaFiloIa::passo(const SituazioneFilo& s) {
    std::vector<ComandoFilo> out;
    if (tagliato_ || !s.siluro) return out;
    const Siluro& sil = *s.siluro;
    filo_ = sil.filo();
    if (!filo_ || !sil.attivo()) return out;
    const auto taglia = [&] {
        out.push_back(ComandoFilo{ComandoFiloTipo::taglia});
        tagliato_ = true;
    };
    // il lanciatore deve evadere o il filo sta per cedere: si taglia, il siluro continua con la testa
    if (s.lanciatore_evade || sil.filo_a_rischio()) {
        taglia();
        return out;
    }
    const std::optional<Vec2> bersaglio = mira(s);
    bool perso = false;
    for (const EventoSiluro& e : s.eventi) {
        if (e.tipo == EventoSiluro::Tipo::aggancio && !aggancio_t_) aggancio_t_ = s.t;
        if (e.tipo == EventoSiluro::Tipo::perdita) {
            aggancio_t_.reset();
            perso = true;
        }
    }
    // esche: un'eco stazionaria lontana dalla stima si rifiuta (una volta per sorgente)
    if (d_.rifiuta_esche && bersaglio)
        for (const RilevamentoTesta& r : sil.testa())
            if (r.stazionaria && r.sorgente >= 0 && !rifiutate_.count(r.sorgente) &&
                std::fabs(normalizza_180(r.rilevamento - rilevamento(sil.pos(), *bersaglio))) > d_.scarto_esca_deg) {
                ComandoFilo c{ComandoFiloTipo::rifiuta};
                c.sorgente = r.sorgente;
                out.push_back(c);
                rifiutate_.insert(r.sorgente);
                if (sil.bersaglio() == r.sorgente) aggancio_t_.reset();
            }
    // agganciato: si guida fino al taglio, poi decide la testa
    if (aggancio_t_) {
        const bool vicino = bersaglio && distanza(sil.pos(), *bersaglio) < d_.distanza_taglio_m;
        if (s.t - *aggancio_t_ >= d_.taglio_dopo_aggancio_s || vicino) taglia();
        return out;
    }
    if (!bersaglio) return out;
    const double voluta = rilevamento(sil.pos(), *bersaglio);
    // il bersaglio si e' sottratto: subito sulla stima nuova, in attivo e veloce
    if (perso && d_.attivo_su_perdita) {
        ComandoFilo m{ComandoFiloTipo::modo};
        m.modo = ModoTesta::attivo;
        ComandoFilo v{ComandoFiloTipo::velocita};
        v.velocita = VelocitaSiluro::alta;
        ComandoFilo r{ComandoFiloTipo::rotta};
        r.valore = voluta;
        out.insert(out.end(), {m, v, r});
        prossima_correzione_ = s.t + d_.correzione_s;
        return out;
    }
    if (s.t >= prossima_correzione_) {
        prossima_correzione_ = s.t + d_.correzione_s;
        if (std::fabs(normalizza_180(voluta - sil.rotta())) > d_.soglia_deg) {
            ComandoFilo r{ComandoFiloTipo::rotta};
            r.valore = voluta;
            out.push_back(r);
        }
    }
    return out;
}

VincoliFilo GuidaFiloIa::vincoli() const {
    VincoliFilo v;
    v.attivi = filo_ && !tagliato_;
    return v;
}

}  // namespace m688
