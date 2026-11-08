#include "nucleo/carico.hpp"

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

constexpr int SENZA_TETTO = std::numeric_limits<int>::max() / 4;
constexpr int ESCHE_MOBILI_ORIGINALI = 2;   // nelle missioni senza deposito (scelta dell'utente, 26/10/2026)

TipoArmaCarico tipo_da_nome(const std::string& s, const std::string& percorso, const std::string& id) {
    if (s == "siluro") return TipoArmaCarico::siluro;
    if (s == "missile") return TipoArmaCarico::missile;
    if (s == "mina") return TipoArmaCarico::mina;
    if (s == "esca") return TipoArmaCarico::esca;
    throw std::runtime_error(percorso + ": arma '" + id + "' di tipo sconosciuto '" + s + "'");
}

std::vector<std::string> stringhe(const toml::node_view<const toml::node>& n) {
    std::vector<std::string> out;
    if (const auto* a = n.as_array())
        for (const auto& v : *a) out.push_back(v.value_or(std::string{}));
    return out;
}

std::map<std::string, int> conti(const toml::node_view<const toml::node>& n) {
    std::map<std::string, int> out;
    if (const auto* t = n.as_table())
        for (auto&& [k, v] : *t) out[std::string(k.str())] = static_cast<int>(v.value_or(std::int64_t{0}));
    return out;
}

int somma(const std::map<std::string, int>& m) {
    int n = 0;
    for (const auto& [_, k] : m) n += k;
    return n;
}

}  // namespace

const SchedaArma* CatalogoCarico::arma(const std::string& id) const {
    const auto it = armi.find(id);
    return it == armi.end() ? nullptr : &it->second;
}

const LocaleSiluri* CatalogoCarico::locale(const std::string& classe, const std::string& variante) const {
    if (!variante.empty() && variante != "flight1")
        if (const auto it = locali.find(classe + "_" + variante); it != locali.end()) return &it->second;
    const auto it = locali.find(classe);
    return it == locali.end() ? nullptr : &it->second;
}

std::string CatalogoCarico::predefinita(const std::string& famiglia, const std::string& nazione) const {
    const auto n = predefinite.find(nazione);
    if (n == predefinite.end()) return {};
    const auto f = n->second.find(famiglia);
    return f == n->second.end() ? std::string{} : f->second;
}

CatalogoCarico carica_carico(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    CatalogoCarico out;
    if (const auto* armi = t["armi"].as_table())
        for (auto&& [chiave, nodo] : *armi) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            const toml::table& r = *tab;
            SchedaArma a;
            a.id = std::string(chiave.str());
            a.nome = r["nome"].value_or(a.id);
            a.nazione = r["nazione"].value_or(std::string{"usa"});
            a.tipo = tipo_da_nome(r["tipo"].value_or(std::string{"siluro"}), percorso, a.id);
            a.famiglia = r["famiglia"].value_or(std::string{});
            if (a.famiglia.empty()) throw std::runtime_error(percorso + ": arma '" + a.id + "' senza famiglia");
            a.siluro = r["siluro"].value_or(std::string{});
            a.guida = r["guida"].value_or(std::string{});
            a.velocita_kn = r["velocita_kn"].value_or(0.0);
            a.portata_km = r["portata_km"].value_or(0.0);
            a.testata_kg = r["testata_kg"].value_or(0.0);
            a.peso_kg = r["peso_kg"].value_or(0.0);
            a.lunghezza_m = r["lunghezza_m"].value_or(0.0);
            a.diametro_mm = r["diametro_mm"].value_or(533);
            a.posti = r["posti"].value_or(1.0);
            a.pozzo_verticale = r["pozzo_verticale"].value_or(false);
            a.in_servizio_dal = r["in_servizio_dal"].value_or(0);
            a.ritirata_nel = r["ritirata_nel"].value_or(0);
            a.massimo_primo_anno = r["massimo_primo_anno"].value_or(0);
            a.da_famiglia = r["da_famiglia"].value_or(true);
            a.solo_ia = r["solo_ia"].value_or(false);
            a.what_if = r["what_if"].value_or(false);
            a.moderno = r["moderno"].value_or(false);
            a.nucleare = r["nucleare"].value_or(false);
            a.sagoma = r["sagoma"].value_or(std::string{});
            a.fonte = r["fonte"].value_or(std::string{});
            a.confidenza = r["confidenza"].value_or(std::string{});
            a.nota = r["nota"].value_or(std::string{});
            a.efficacia = conti(r["efficacia"]);
            a.efficacia_nota = r["efficacia_nota"].value_or(std::string{});
            a.efficacia_fonte = r["efficacia_fonte"].value_or(std::string{});
            a.efficacia_confidenza = r["efficacia_confidenza"].value_or(std::string{});
            out.armi[a.id] = std::move(a);
        }
    if (const auto* locali = t["locali"].as_table())
        for (auto&& [chiave, nodo] : *locali) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            const toml::table& r = *tab;
            LocaleSiluri l;
            l.id = std::string(chiave.str());
            l.nome = r["nome"].value_or(l.id);
            l.nazione = r["nazione"].value_or(std::string{"usa"});
            l.tubi = r["tubi"].value_or(l.tubi);
            l.calibro_mm = r["calibro_mm"].value_or(l.calibro_mm);
            l.rastrelliera = r["rastrelliera"].value_or(l.rastrelliera);
            l.pozzi = r["pozzi"].value_or(l.pozzi);
            l.lanciatori_esche = r["lanciatori_esche"].value_or(l.lanciatori_esche);
            l.esche = r["esche"].value_or(l.esche);
            l.limite_in_acqua = r["limite_in_acqua"].value_or(l.limite_in_acqua);
            l.posizioni = stringhe(r["posizioni"]);
            l.fonte = r["fonte"].value_or(std::string{});
            if (std::count(l.posizioni.begin(), l.posizioni.end(), "tubo") != l.tubi)
                throw std::runtime_error(percorso + ": locale '" + l.id + "' con posizioni diverse dai tubi");
            out.locali[l.id] = std::move(l);
        }
    if (const auto* pred = t["predefinite"].as_table())
        for (auto&& [nazione, nodo] : *pred)
            if (const auto* tab = nodo.as_table())
                for (auto&& [f, v] : *tab) out.predefinite[std::string(nazione.str())][std::string(f.str())] = v.value_or(std::string{});
    if (const auto* carichi = t["carichi"].as_table())
        for (auto&& [locale, nodo] : *carichi) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            const std::string l(locale.str());
            out.predefiniti[l] = (*tab)["predefinito"].value_or(std::string{});
            for (auto&& [tipo, n2] : *tab) {
                const auto* c = n2.as_table();
                if (!c) continue;
                CaricoStandard s;
                s.tubi = stringhe((*c)["tubi"]);
                s.rastrelliera = conti((*c)["rastrelliera"]);
                s.pozzi = conti((*c)["pozzi"]);
                for (const auto& v : s.tubi)
                    if (!out.arma(v)) throw std::runtime_error(percorso + ": carico " + l + "." + std::string(tipo.str()) + " con l'arma ignota '" + v + "'");
                out.carichi[l][std::string(tipo.str())] = std::move(s);
            }
        }
    return out;
}

double scarto_dal_standard(const CaricoPartenza& c, const CatalogoCarico& cat, const std::string& locale,
                           const std::string& tipo) {
    const auto l = cat.carichi.find(locale);
    if (l == cat.carichi.end()) return 0.0;
    const auto s = l->second.find(tipo);
    if (s == l->second.end()) return 0.0;
    std::map<std::string, int> a, b;
    for (std::size_t i = 0; i < c.posizioni.size() && i < c.tipi.size(); ++i)
        if (c.tipi[i] == "tubo" && !c.posizioni[i].empty()) ++a[c.posizioni[i]];
    for (const auto* m : {&c.rastrelliera, &c.pozzi})
        for (const auto& [id, n] : *m) a[id] += n;
    for (const auto& id : s->second.tubi) ++b[id];
    for (const auto* m : {&s->second.rastrelliera, &s->second.pozzi})
        for (const auto& [id, n] : *m) b[id] += n;
    int diff = 0;
    for (const auto& [id, n] : a) diff += std::abs(n - (b.count(id) ? b.at(id) : 0));
    for (const auto& [id, n] : b)
        if (!a.count(id)) diff += n;
    const int totale = std::max(somma(a), somma(b));
    return totale == 0 ? 0.0 : std::min(1.0, 0.5 * diff / totale);
}

// --- SalaSiluri ---

SalaSiluri::SalaSiluri(const CatalogoCarico& c, const std::string& classe, const LimitiMissione& l, bool what_if_giocatore,
                       bool moderno)
    : catalogo_(&c), locale_(c.locale(classe, l.variante_battello)), limiti_(l), what_if_(what_if_giocatore),
      moderno_(moderno), deposito_dichiarato_(!l.deposito.empty()) {
    if (!locale_) throw std::runtime_error("carico: nessun locale siluri per la classe '" + classe + "'");
    if (limiti_.deposito.empty()) {   // le missioni originali: la dotazione, piu' due esche mobili da tubo (la MG-74
        limiti_.deposito = limiti_.dotazione;   // dell'Alfa; la MOSS del 688, al deposito nel modo moderno)
        limiti_.deposito.emplace("esca_mobile", ESCHE_MOBILI_ORIGINALI);
    }
    svuota();
}

const SchedaArma& SalaSiluri::scheda(const std::string& id) const {
    const auto* a = catalogo_->arma(id);
    if (!a) throw std::out_of_range("carico: arma ignota '" + id + "'");
    return *a;
}

int SalaSiluri::deposito_proprio(const SchedaArma& a) const {
    if (a.id == a.famiglia) return 0;   // la chiave e' quella della famiglia (mk48, harpoon, sea_lance)
    const auto it = limiti_.deposito.find(a.id);
    return it == limiti_.deposito.end() ? 0 : std::max(0, it->second);
}

int SalaSiluri::deposito_famiglia(const std::string& famiglia) const {
    const auto it = limiti_.deposito.find(famiglia);
    return it == limiti_.deposito.end() ? 0 : std::max(0, it->second);
}

int SalaSiluri::caricate(const std::string& id) const {
    int n = 0;   // le posizioni dei pozzi e delle esche ripetono la prima variante: non sono armi in piu'
    for (const int i : indici_tubi()) n += carico_.posizioni[static_cast<std::size_t>(i)] == id;
    for (const auto* m : {&carico_.rastrelliera, &carico_.pozzi, &carico_.esche})
        if (const auto it = m->find(id); it != m->end()) n += it->second;
    return n;
}

int SalaSiluri::dalla_famiglia(const SchedaArma& a) const {
    return std::max(0, caricate(a.id) - deposito_proprio(a));
}

int SalaSiluri::tetto(const SchedaArma& a) const {
    if (a.massimo_primo_anno > 0 && limiti_.anno < a.in_servizio_dal + 2) return a.massimo_primo_anno;
    return SENZA_TETTO;
}

bool SalaSiluri::dichiarata(const SchedaArma& a) const {
    return deposito_proprio(a) > 0 || (a.da_famiglia && deposito_famiglia(a.famiglia) > 0);
}

std::string SalaSiluri::motivo_negata(const SchedaArma& a) const {
    if (a.nucleare) return "nucleare";
    if (a.solo_ia) return "solo IA";
    if (limiti_.tipi_negati.count(a.id) || limiti_.tipi_negati.count(a.famiglia)) return "negata dalla missione";
    if (a.what_if && a.moderno && !moderno_) return "what-if";   // solo il modo moderno la consente
    if (a.what_if && !a.moderno && !limiti_.what_if && !what_if_) return "what-if";
    if (!a.what_if && (a.in_servizio_dal == 0 || a.in_servizio_dal > limiti_.anno))
        return "in servizio dal " + std::to_string(a.in_servizio_dal);
    if (a.what_if && a.in_servizio_dal > limiti_.anno)   // la MOSS esiste dal 1976: what-if solo il battello
        return "in servizio dal " + std::to_string(a.in_servizio_dal);
    if (a.ritirata_nel > 0 && limiti_.anno >= a.ritirata_nel) return "ritirata nel " + std::to_string(a.ritirata_nel);
    if (!dichiarata(a)) return "non al deposito";
    return {};
}

ArmaDisponibile SalaSiluri::disponibile(const std::string& id) const {
    const SchedaArma& a = scheda(id);
    ArmaDisponibile d;
    d.id = id;
    d.caricate = caricate(id);
    d.motivo = motivo_negata(a);
    d.negata = !d.motivo.empty();
    if (d.negata) {
        d.deposito = d.caricate;   // nessuna in piu'
        return d;
    }
    int famiglia = 0;
    if (a.da_famiglia) {
        int altre = 0;   // quelle della stessa famiglia che hanno gia' preso dal suo deposito
        for (const auto& [bid, b] : catalogo_->armi)
            if (bid != id && b.famiglia == a.famiglia && b.da_famiglia) altre += dalla_famiglia(b);
        famiglia = std::min(std::max(0, deposito_famiglia(a.famiglia) - altre), tetto(a));
    }
    d.deposito = deposito_proprio(a) + famiglia;
    return d;
}

std::vector<ArmaDisponibile> SalaSiluri::disponibili() const {
    std::vector<ArmaDisponibile> out;
    for (const auto& [id, a] : catalogo_->armi)
        if (a.nazione == locale_->nazione) out.push_back(disponibile(id));
    return out;
}

int SalaSiluri::posti_totali() const { return locale_->tubi + std::max(0, locale_->rastrelliera - limiti_.ospiti); }

double SalaSiluri::posti_usati() const {
    double n = pronti_nei_tubi();
    for (const auto& [id, k] : carico_.rastrelliera) n += k * scheda(id).posti;
    return n;
}

int SalaSiluri::pozzi_usati() const { return somma(carico_.pozzi); }
int SalaSiluri::esche_caricate() const { return somma(carico_.esche); }

int SalaSiluri::pronti_nei_tubi() const {
    int n = 0;
    for (const int i : indici_tubi()) n += !carico_.posizioni[static_cast<std::size_t>(i)].empty();
    return n;
}

double SalaSiluri::peso_kg() const {
    double kg = 0.0;
    for (const int i : indici_tubi())
        if (const auto& id = carico_.posizioni[static_cast<std::size_t>(i)]; !id.empty()) kg += scheda(id).peso_kg;
    for (const auto* m : {&carico_.rastrelliera, &carico_.pozzi, &carico_.esche})
        for (const auto& [id, k] : *m) kg += k * scheda(id).peso_kg;
    return kg;
}

std::vector<int> SalaSiluri::indici_tubi() const {
    std::vector<int> out;
    for (std::size_t i = 0; i < locale_->posizioni.size(); ++i)
        if (locale_->posizioni[i] == "tubo") out.push_back(static_cast<int>(i));
    return out;
}

int SalaSiluri::indice_pozzi() const {
    const auto it = std::find(locale_->posizioni.begin(), locale_->posizioni.end(), "pozzi");
    return it == locale_->posizioni.end() || locale_->pozzi == 0 ? -1 : static_cast<int>(it - locale_->posizioni.begin());
}

int SalaSiluri::tubo_libero() const {
    const auto t = indici_tubi();
    for (std::size_t k = 0; k < t.size(); ++k)
        if (carico_.posizioni[static_cast<std::size_t>(t[k])].empty()) return static_cast<int>(k) + 1;
    return 0;
}

// le posizioni dei pozzi e delle esche mostrano la prima variante caricata (quella che la stazione arma per prima)
void SalaSiluri::aggiorna_posizione_pozzi() {
    const auto prima = [](const std::map<std::string, int>& m) {
        for (const auto& [id, k] : m)
            if (k > 0) return id;
        return std::string{};
    };
    for (std::size_t i = 0; i < locale_->posizioni.size(); ++i) {
        if (locale_->posizioni[i] == "pozzi") carico_.posizioni[i] = prima(carico_.pozzi);
        else if (locale_->posizioni[i] == "esca") carico_.posizioni[i] = prima(carico_.esche);
    }
}

EsitoCarico SalaSiluri::metti(Posto p, const std::string& arma, int tubo) {
    const SchedaArma* a = catalogo_->arma(arma);
    if (!a) return {false, "arma ignota"};
    if (a->nazione != locale_->nazione) return {false, "arma di un'altra marina"};
    const ArmaDisponibile d = disponibile(arma);
    if (d.negata) return {false, d.motivo};
    if (d.rimaste() <= 0) return {false, "deposito esaurito"};
    const bool esca = a->tipo == TipoArmaCarico::esca;
    switch (p) {
        case Posto::tubo: {
            const auto t = indici_tubi();
            if (esca) return {false, "le esche hanno i loro lanciatori"};
            if (tubo < 1 || tubo > static_cast<int>(t.size())) return {false, "tubo inesistente"};
            auto& pos = carico_.posizioni[static_cast<std::size_t>(t[static_cast<std::size_t>(tubo - 1)])];
            if (!pos.empty()) return {false, "tubo occupato"};
            pos = arma;
            return {true, {}};
        }
        case Posto::rastrelliera: {
            if (esca) return {false, "le esche hanno la loro scorta"};
            double usati = 0.0;
            for (const auto& [id, k] : carico_.rastrelliera) usati += k * scheda(id).posti;
            if (usati + a->posti > std::max(0, locale_->rastrelliera - limiti_.ospiti) + 1e-9) return {false, "posti finiti"};
            ++carico_.rastrelliera[arma];
            return {true, {}};
        }
        case Posto::pozzi:
            if (indice_pozzi() < 0) return {false, "il battello non ha pozzi verticali"};
            if (!a->pozzo_verticale) return {false, "non va nei pozzi"};
            if (pozzi_usati() >= locale_->pozzi) return {false, "pozzi pieni"};
            ++carico_.pozzi[arma];
            aggiorna_posizione_pozzi();
            return {true, {}};
        case Posto::esche:
            if (!esca) return {false, "non e' un'esca"};
            if (esche_caricate() >= locale_->esche) return {false, "scorta delle esche piena"};
            ++carico_.esche[arma];
            aggiorna_posizione_pozzi();
            return {true, {}};
    }
    return {false, "posto sconosciuto"};
}

EsitoCarico SalaSiluri::togli(Posto p, const std::string& arma, int tubo) {
    const auto togli_da = [&](std::map<std::string, int>& m) -> EsitoCarico {
        const auto it = m.find(arma);
        if (it == m.end() || it->second <= 0) return {false, "arma assente"};
        if (--it->second == 0) m.erase(it);
        aggiorna_posizione_pozzi();
        return {true, {}};
    };
    switch (p) {
        case Posto::tubo: {
            const auto t = indici_tubi();
            if (tubo < 1 || tubo > static_cast<int>(t.size())) return {false, "tubo inesistente"};
            auto& pos = carico_.posizioni[static_cast<std::size_t>(t[static_cast<std::size_t>(tubo - 1)])];
            if (pos.empty() || (!arma.empty() && pos != arma)) return {false, "arma assente"};
            pos.clear();
            return {true, {}};
        }
        case Posto::rastrelliera: return togli_da(carico_.rastrelliera);
        case Posto::pozzi: return togli_da(carico_.pozzi);
        case Posto::esche: return togli_da(carico_.esche);
    }
    return {false, "posto sconosciuto"};
}

void SalaSiluri::svuota() {
    carico_ = CaricoPartenza{};
    carico_.posizioni.assign(locale_->posizioni.size(), std::string{});
    carico_.tipi = locale_->posizioni;
}

std::string SalaSiluri::principale() const {
    return catalogo_->predefinita(locale_->nazione == "urss" ? "type53" : "mk48", locale_->nazione);
}

void SalaSiluri::carico_standard(const std::string& tipo) {
    std::string t = tipo.empty() ? limiti_.tipo_missione : tipo;
    const auto cl = catalogo_->carichi.find(locale_->id);
    if (cl == catalogo_->carichi.end()) return carico_predefinito();
    if (!cl->second.count(t)) {
        const auto p = catalogo_->predefiniti.find(locale_->id);
        t = p == catalogo_->predefiniti.end() ? std::string{} : p->second;
    }
    const auto it = cl->second.find(t);
    if (it == cl->second.end()) return carico_predefinito();
    const CaricoStandard& s = it->second;
    svuota();
    // l'arma o, se manca, la variante predefinita della sua famiglia, poi il siluro principale
    const auto prova = [&](Posto p, const std::string& arma, int tubo) {
        const SchedaArma* a = catalogo_->arma(arma);
        for (const std::string& c : {arma, a ? catalogo_->predefinita(a->famiglia, locale_->nazione) : std::string{}, principale()})
            if (!c.empty() && metti(p, c, tubo).fatto) return;
    };
    for (std::size_t k = 0; k < s.tubi.size(); ++k) prova(Posto::tubo, s.tubi[k], static_cast<int>(k) + 1);
    for (const auto& [arma, n] : s.rastrelliera)
        for (int k = 0; k < n; ++k) prova(Posto::rastrelliera, arma, 0);
    for (const auto& [arma, n] : s.pozzi)   // nei pozzi niente ripiego sul siluro: solo armi da pozzo
        for (int k = 0; k < n; ++k)
            if (!metti(Posto::pozzi, arma, 0).fatto) {
                const SchedaArma* a = catalogo_->arma(arma);
                if (a) metti(Posto::pozzi, catalogo_->predefinita(a->famiglia, locale_->nazione), 0);
            }
    const std::string esca = catalogo_->predefinita("esca", locale_->nazione);
    while (!esca.empty() && metti(Posto::esche, esca, 0).fatto) {}
}

void SalaSiluri::carico_predefinito() {
    svuota();
    const auto t = indici_tubi();
    const auto variante = [&](const std::string& famiglia) {
        const std::string v = catalogo_->predefinita(famiglia, locale_->nazione);
        return v.empty() ? famiglia : v;
    };
    // tubi dell'originale: i siluri nei tubi; il missile del tubo 7 nell'ultimo tubo (Flight I) o nei pozzi
    std::vector<std::string> missili;
    for (std::size_t i = 0; i < limiti_.tubi_originali.size(); ++i) {
        const std::string& f = limiti_.tubi_originali[i];
        if (f.empty() || f == "esca") continue;
        const SchedaArma* a = catalogo_->arma(variante(f));
        if (!a) continue;
        if (a->tipo == TipoArmaCarico::missile) missili.push_back(a->id);
        else if (const auto k = std::find(t.begin(), t.end(), static_cast<int>(i)); k != t.end())
            metti(Posto::tubo, a->id, static_cast<int>(k - t.begin()) + 1);
    }
    for (const auto& m : missili) {
        const SchedaArma& a = scheda(m);
        if (a.pozzo_verticale && indice_pozzi() >= 0) {
            while (metti(Posto::pozzi, m, 0).fatto) {}   // i Tomahawk della dotazione tutti nei pozzi
            continue;
        }
        if (t.empty() || disponibile(m).rimaste() <= 0) continue;   // negato (Sea Lance senza what-if) o finito
        const int ultimo = static_cast<int>(t.size());
        if (!carico_.posizioni[static_cast<std::size_t>(t.back())].empty()) togli(Posto::tubo, "", ultimo);
        metti(Posto::tubo, m, ultimo);
    }
    for (const int i : t)   // tubi rimasti vuoti (missioni con meno tubi): il siluro principale
        if (carico_.posizioni[static_cast<std::size_t>(i)].empty())
            metti(Posto::tubo, principale(), static_cast<int>(std::find(t.begin(), t.end(), i) - t.begin()) + 1);
    // il resto della dotazione in rastrelliera, nell'ordine dell'originale: prima i siluri, poi i missili
    std::vector<std::string> famiglie;
    // le esche mobili storiche per prime, cosi' prendono il posto degli ultimi siluri (la MG-74 dell'Alfa); una what-if
    // (la MOSS del 688) resta al deposito: la carica il giocatore, e il carico predefinito e' lo stesso nei due modi
    if (limiti_.deposito.count("esca_mobile")) {
        const std::string v = variante("esca_mobile");
        if (const SchedaArma* a = catalogo_->arma(v); a && !a->what_if)
            while (metti(Posto::rastrelliera, v, 0).fatto) {}
    }
    for (const auto& f : {"mk48", "type53", "harpoon", "tomahawk", "sea_lance", "mina", "attacco_terra"})
        if (limiti_.deposito.count(f)) famiglie.emplace_back(f);
    for (const auto& [k, n] : limiti_.deposito)   // varianti dichiarate per nome
        if (std::find(famiglie.begin(), famiglie.end(), k) == famiglie.end() && k != "esca" && k != "esca_mobile" && catalogo_->arma(k))
            famiglie.push_back(k);
    for (const auto& f : famiglie) {
        const std::string v = catalogo_->arma(f) && catalogo_->arma(f)->famiglia != f ? f : variante(f);
        while (metti(Posto::rastrelliera, v, 0).fatto) {}
    }
    const std::string esca = catalogo_->predefinita("esca", locale_->nazione);
    while (!esca.empty() && metti(Posto::esche, esca, 0).fatto) {}
}

void SalaSiluri::carico_iniziale() {
    if (deposito_dichiarato_) carico_standard();
    else carico_predefinito();
}

std::vector<std::string> SalaSiluri::avvisi() const {
    std::vector<std::string> out;
    const int vuoti = locale_->tubi - pronti_nei_tubi();
    if (vuoti > 0) out.push_back("tubi vuoti: " + std::to_string(vuoti));
    if (const int liberi = static_cast<int>(posti_liberi() - vuoti + 1e-9); liberi > 0)
        out.push_back("posti liberi in rastrelliera: " + std::to_string(liberi));
    if (indice_pozzi() >= 0 && pozzi_usati() < locale_->pozzi)
        out.push_back("pozzi liberi: " + std::to_string(locale_->pozzi - pozzi_usati()));
    if (esche_caricate() < locale_->esche) out.push_back("esche: " + std::to_string(esche_caricate()) + " su " + std::to_string(locale_->esche));
    bool mine = false, what_if = false;
    for (const auto& [id, a] : catalogo_->armi)
        if (caricate(id) > 0) {
            mine = mine || a.tipo == TipoArmaCarico::mina;
            what_if = what_if || a.what_if;
        }
    if (limiti_.tipo_missione == "minamento" && !mine) out.push_back("nessuna mina per il minamento");
    if (what_if) out.push_back("armi what-if a bordo");
    for (const auto& [id, n] : limiti_.deposito)
        if (const SchedaArma* a = catalogo_->arma(id); a && a->id == a->famiglia && n > 0 && !motivo_negata(*a).empty()
                                                       && motivo_negata(*a) != "non al deposito")
            out.push_back(a->nome + ": " + motivo_negata(*a));
    return out;
}

}  // namespace m688
