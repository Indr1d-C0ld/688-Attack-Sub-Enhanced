// Il siluro antisom delle navi dell'IA nell'Enhanced (collaudo della libreria del 29/10/2026, S12 «Gruppo cacciatore»;
// la principale lo ha approvato). L'ATTACK degli script su un sottomarino, che nel classico e' una carica a dado (il
// registro la chiama «carica»), tira l'arma vera della classe col modello del siluro, come il MISSILE degli script col
// missile:
// - le americane il Mk 46: la Perry dai tubi Mk 32, entro la corsa utile; Spruance e Kidd con l'ASROC, che lo posa vicino
//   a dove la nave crede il bersaglio, fino a 9 km;
// - le sovietiche coi tubi da 533 il SET-65, entro la corsa utile; le altre (i lanciarazzi RBU) restano a dado;
// - il bersaglio dove la nave lo crede: la posizione del suo contatto; senza posizione niente siluro; su un sottomarino in
//   superficie nemmeno (la testa cerca sotto: restano i cannoni; 03/11/2026);
// - il siluro e' un'unita' della missione col modello (nuovo_siluro), della parte della nave.
// - la cadenza (secondo collaudo della libreria, 30/10/2026: le Perry tiravano 12 Mk 46 in tre minuti): un'arma alla
//   volta per nave, e la successiva solo dopo l'esito della prima e VERIFICA_S di attesa; i tubi carichi della classe,
//   ricaricati a mano dalla riserva uno ogni `ricarica_s` (STIMA). Senza tubi carichi, senza posizione o fuori portata la
//   nave non tira, e non c'e' piu' il dado delle cariche: chi ha l'arma vera non ha bombe di profondita' (vero).
// - oltre la portata dei tubi, l'elicottero imbarcato (principale, 30/10/2026): SH-60B LAMPS III sulle Perry, Spruance e
//   Ticonderoga, SH-2 Seasprite sulle Knox, Ka-25 sulla Kara, Ka-27 sull'Udaloy e la Kirov. Decolla sul contatto
//   (entro RAGGIO_ELICOTTERO_M, col mare fino a mare_max della macchina, STIMA) e da li' e' un aereo ASW come gli altri
//   (missione_asw.cpp: transito, boe o sonar a immersione, Mk 46, il ghiaccio e il mare del controllore); finito, appontato,
//   e' pronto di nuovo dopo RIFORNIMENTO_S. Se la missione ha gia' un elicottero sulla nave (base_unita, o uno della
//   parte accanto a lei all'inizio) la nave non ne fa un altro, finche' quello non e' appontato. Senza l'ASW dell'Enhanced
//   niente elicottero. Con l'hangar bruciato (perdi_elicottero, da DanniMissione) non decolla piu': quello in volo torna e
//   resta a bordo.
// Senza i siluri del modello (il classico) non cambia niente. Le salve restano quelle della dotazione (salva_asw).

#include <algorithm>
#include <cmath>
#include <map>
#include <string>

#include "missioni/asw_missione.hpp"
#include "missioni/missione.hpp"

namespace m688 {

namespace {

struct ArmaAntisom {
    const char* siluro;
    bool asroc = false;      // il razzo porta il siluro vicino al bersaglio
    double portata_m = 0.0;  // dal lanciatore al punto del bersaglio (STIMA)
    int tubi = 0, riserva = 0;   // carichi alla partenza e di scorta
    double ricarica_s = 0.0;     // un tubo (o una cella del lanciatore) dalla riserva
};

// per classe (STIMA dalle dotazioni del 1989: dossier 01 e kit): la Perry coi due Mk 32 tripli e la scorta di bordo; lo
// Spruance e il Kidd col lanciatore ASROC Mk 16 da 8 e le ricariche; le sovietiche coi tubi da 533 (2 x 4 la Krivak, 2 x 5
// le altre), senza ricariche in mare
const ArmaAntisom* arma_antisom(const std::string& classe) {
    static const std::map<std::string, ArmaAntisom> armi = {
        {"perry", {"mk46", false, 7000.0, 6, 18, 300.0}},     {"spruance", {"mk46", true, 9000.0, 8, 16, 120.0}},
        {"kidd", {"mk46", true, 9000.0, 8, 16, 120.0}},       {"krivak2", {"set65", false, 8000.0, 8, 0, 0.0}},
        {"kara", {"set65", false, 8000.0, 10, 0, 0.0}},       {"kirov", {"set65", false, 8000.0, 10, 0, 0.0}},
        {"slava", {"set65", false, 8000.0, 10, 0, 0.0}},     {"udaloy", {"set65", false, 8000.0, 8, 0, 0.0}},
        {"ticonderoga", {"mk46", true, 9000.0, 8, 12, 120.0}}, {"knox", {"mk46", true, 9000.0, 8, 8, 120.0}},
        // la Type 22 (Batch 1 e 2): due STWS Mk 2 tripli da 324 mm col Mk 46 nel 1989 (lo Stingray dalla nave dopo;
        // disassemblatore, docs/ricerca/38); riserva e ricarica STIMA come la Perry
        {"type22", {"mk46", false, 7000.0, 6, 12, 300.0}},    {"type22b2", {"mk46", false, 7000.0, 6, 12, 300.0}},
    };
    const auto it = armi.find(classe);
    return it == armi.end() ? nullptr : &it->second;
}

constexpr double QUOTA_INGRESSO_M = 15.0;   // il siluro entra in acqua e parte (come quello dell'aereo)
constexpr double QUOTA_EMERSO_M = 2.0;         // fin qui il sottomarino e' in superficie: niente siluro antisom
constexpr double VERIFICA_S = 90.0;         // dopo l'esito: il contatto si riprende prima del colpo seguente (STIMA)
constexpr double RAGGIO_ELICOTTERO_M = 60000.0;   // il contatto per cui decolla (STIMA, dentro il raggio d'azione)
constexpr double RIFORNIMENTO_S = 1800.0;         // appontato: carburante, boe e siluri di nuovo (STIMA)
constexpr double APPONTAGGIO_M = 1500.0;          // in rientro cosi' vicino alla nave: e' a bordo
constexpr double CUE_OGNI_S = 45.0;                // la nave passa il suo contatto all'elicottero in volo (STIMA, 30-60 s)
constexpr double RAGGIO_CUE_ECO_M = 300.0, RAGGIO_CUE_MAX_M = 3000.0;   // il raggio del cue: eco o vista; il tetto
constexpr double SUO_M = 1000.0;                  // un elicottero della parte cosi' vicino all'inizio: e' gia' il suo

struct Elicottero { const char* mezzo; int mare_max; };
// per classe (STIMA: le dotazioni del 1989; lo stato del mare oltre cui non si decolla dal ponte)
const Elicottero* elicottero_di(const std::string& classe) {
    static const std::map<std::string, Elicottero> e = {
        {"perry", {"sh60b", 5}}, {"spruance", {"sh60b", 5}}, {"ticonderoga", {"sh60b", 5}}, {"knox", {"sh2f", 4}},
        {"kara", {"ka25", 4}},   {"udaloy", {"ka27", 5}},    {"kirov", {"ka27", 5}},
        {"type22", {"lynx", 5}}, {"type22b2", {"lynx", 5}},   // il Lynx HAS.3 (MATCH): attivo quando c'e' il mezzo in asw_aereo
    };
    const auto it = e.find(classe);
    return it == e.end() ? nullptr : &it->second;
}

}  // namespace

// il siluro antisom della dotazione della classe (la tabella qui sopra), o nullptr: per il controllo delle armi che gli
// script fanno lanciare alle navi dell'IA (missione.cpp, nuova_unita_da_buffer)
const char* siluro_antisom_della_classe(const std::string& classe) {
    const ArmaAntisom* a = arma_antisom(classe);
    return a ? a->siluro : nullptr;
}

bool Missione::siluro_antisom_nave(int nave, int bersaglio) {
    if (!siluri_attivi_) return false;
    const UnitaClassica* n = trova(nave);
    const UnitaClassica* b = trova(bersaglio);
    if (!n || !b || n->categoria != "nave" || b->categoria != "sottomarino") return false;
    const ArmaAntisom* a = arma_antisom(n->classe);
    if (!a) return false;
    const DatiSiluro* d = tabella_siluri_.siluro(a->siluro);
    if (!d) return false;
    // da qui la nave ha l'arma vera: niente dado, anche quando non tira
    AntisomNave& s = antisom_navi_[nave];
    if (s.tubi < 0) {
        s.tubi = a->tubi;
        s.riserva = a->riserva;
        s.prossima_ricarica = tempo_ + a->ricarica_s;
    }
    while (s.riserva > 0 && s.tubi < a->tubi && tempo_ >= s.prossima_ricarica) {   // la ricarica a mano, un tubo alla volta
        ++s.tubi;
        --s.riserva;
        s.prossima_ricarica += a->ricarica_s;
    }
    if (s.tubi >= a->tubi) s.prossima_ricarica = std::max(s.prossima_ricarica, tempo_ + a->ricarica_s);
    if (s.in_acqua >= 0) {   // l'arma di prima: finche' corre si aspetta, poi la verifica
        const UnitaClassica* w = trova(s.in_acqua);
        if (w && !w->distrutta && !w->persa) return true;
        s.in_acqua = -1;
        s.attesa_fino = tempo_ + VERIFICA_S;
    }
    if (tempo_ < s.attesa_fino || s.tubi <= 0) return true;
    // il vetrino di Dio: i tubi bloccati (tutti o quanti ne ha: la nave ne ha uno per lato, si fermano tutti)
    if (!interventi_.empty() && parametro_attivo(interventi_, stati_interventi_, nave, "tubi", "tubi")) return true;
    const auto k = n->contatti.find(bersaglio);
    if (k == n->contatti.end() || !k->second.posizione_nota) return true;   // senza posizione non tira
    const Vec2 punto = k->second.pos_stimata;
    if (distanza(n->pos, punto) > a->portata_m) {   // oltre i tubi: l'elicottero, se c'e'
        lancia_elicottero(nave, punto, bersaglio);
        return true;
    }
    // un siluro antisom non attacca un sottomarino in superficie: la testa cerca sotto e la spoletta gli passa sotto lo
    // scafo. Contro l'emerso restano i cannoni (missione_artiglieria.cpp; la principale, 03/11/2026)
    if (b->quota <= QUOTA_EMERSO_M) return true;
    UnitaClassica w;
    w.id = prossimo_id_++;
    w.classe = a->siluro;
    w.categoria = "arma";
    w.flag = 0x20 | (n->flag & 0x18);
    w.pos = a->asroc ? punto : n->pos;
    w.quota = w.quota_ordinata = QUOTA_INGRESSO_M;
    w.rotta = w.rotta_ordinata = normalizza_360(rilevamento(n->pos, punto));
    w.lanciatore = nave;
    w.bersaglio = bersaglio;
    w.prossimo_bersaglio = w.id;
    w.velocita = w.velocita_ordinata = d->velocita_alta_kn;
    w.difesa = 40;
    auto& r = unita_[w.id] = w;
    osservatore_ = visitata_ = w.id;   // come nuova_unita_da_buffer: in testa alla lista
    ++create_;
    nuovo_siluro(r, d);
    s.in_acqua = w.id;
    --s.tubi;
    return true;
}

bool Missione::lancia_elicottero(int nave, Vec2 punto, int bersaglio) {
    const UnitaClassica* n = trova(nave);
    const Elicottero* e = n ? elicottero_di(n->classe) : nullptr;
    if (!e || !asw_ || hangar_persi_.count(nave)) return false;
    const DatiMezzo* m = asw_->dati.mezzo(e->mezzo);
    if (!m) return false;   // la macchina senza i dati dell'ASW (asw_aereo.toml)
    AntisomNave& s = antisom_navi_[nave];
    if (!s.elicottero_della_missione) {   // la prima volta: la missione ne ha gia' uno sulla nave?
        bool suo = false;
        for (const auto& c : asw_->configurazioni) suo = suo || c.base_unita == nave;
        for (const auto& [id, u] : unita_)
            if (aereo(u) && !u.distrutta && !u.persa && (u.flag & 0x18) == (n->flag & 0x18) && distanza(u.pos, n->pos) < SUO_M)
                if (const DatiMezzo* x = asw_->dati.mezzo(u.classe); x && x->elicottero) suo = true;
        s.elicottero_della_missione = suo;
    }
    if (*s.elicottero_della_missione || s.elicottero >= 0 || tempo_ < s.elicottero_pronto) return false;
    if (distanza(n->pos, punto) > RAGGIO_ELICOTTERO_M || meteo_in(n->pos).stato_mare() > e->mare_max) return false;
    UnitaClassica h;
    h.id = prossimo_id_++;
    h.classe = e->mezzo;
    h.categoria = "aereo";
    h.flag = (n->flag & 0x18) | 3;
    h.pos = n->pos;
    h.rotta = h.rotta_ordinata = normalizza_360(rilevamento(n->pos, punto));
    h.velocita = h.velocita_ordinata = 0.0;
    h.avanti_tutta = m->velocita_transito_kn * 256.0 / 280.0;
    h.virata = 3.0;
    h.accelerazione = 5.0;
    h.navigazione = true;
    h.meta = punto;
    unita_[h.id] = h;
    osservatore_ = visitata_ = h.id;   // come nuova_unita_da_buffer: in testa alla lista
    ++create_;
    // il controllore lo prende al passo dell'ASW, con questa configurazione: il contatto della nave, la nave per base
    ConfigAereoAsw c;
    c.unita = h.id;
    c.classe = e->mezzo;
    c.base_unita = nave;
    c.zona = punto;
    c.raggio_zona_m = 5000.0;
    c.bersagli = {bersaglio};
    Cue q;
    q.tipo = Cue::Tipo::contatto_nave;
    q.pos = punto;
    q.t = tempo_;
    q.raggio_m = 1000.0;
    q.unita = nave;
    c.cue = {q};
    asw_->configurazioni.push_back(c);
    s.elicottero = h.id;
    return true;
}

// l'hangar bruciato (DanniMissione): l'elicottero a bordo e' perso, quello in volo torna ma non riparte
void Missione::perdi_elicottero(int id) { hangar_persi_.insert(id); }

// la fine del volo degli aerei dell'ASW (principale, 04/11/2026). La base segue la nave madre viva; in rientro, arrivato:
// - sulla nave madre viva l'elicottero apponta. Se era l'ultimo della missione su quella nave, con l'hangar intatto, da li'
//   e' l'elicottero della nave: pronto dopo il rifornimento, riparte con lancia_elicottero;
// - con la nave persa ammara dov'era lei l'ultima volta, e il registro lo dice;
// - con la base fissa, o senza base (il punto di partenza), l'aereo esce di scena.
// Abbattuto l'elicottero della nave, o persa la nave, la nave non ne ha un altro
void Missione::passo_elicotteri_navi() {
    for (auto& [id, a] : asw_->aerei) {
        if (!a.attivo) continue;
        UnitaClassica* u = trova(id);
        if (!u || u->distrutta || u->persa) continue;
        const ConfigAereoAsw* cf = nullptr;
        for (const auto& c : asw_->configurazioni)
            if (c.unita == id || (c.unita < 0 && c.classe == u->classe)) { cf = &c; break; }
        const int madre = cf ? cf->base_unita : -1;
        const UnitaClassica* n = madre >= 0 ? trova(madre) : nullptr;
        const bool viva = n && !n->distrutta && !n->persa;
        if (viva) a.c.sposta_base(n->pos);
        if (a.c.stato() != StatoAsw::rientro || distanza(u->pos, a.c.configurazione().base) >= APPONTAGGIO_M) continue;
        u->persa = true;
        if (madre >= 0 && !viva) reazioni_ia_.push_back({tempo_, id, -1, u->categoria, "ammaraggio", "asw"});
        if (!viva || hangar_persi_.count(madre)) continue;
        bool altri = false;
        for (const auto& c : asw_->configurazioni)
            if (c.base_unita == madre && c.unita != id)
                if (const UnitaClassica* x = trova(c.unita); x && !x->distrutta && !x->persa) altri = true;
        if (altri) continue;
        AntisomNave& s = antisom_navi_[madre];
        s.elicottero_della_missione = false;
        s.elicottero_pronto = tempo_ + RIFORNIMENTO_S;
    }
    for (auto& [id, s] : antisom_navi_) {
        if (s.elicottero < 0) continue;
        UnitaClassica* h = trova(s.elicottero);
        const UnitaClassica* n = trova(id);
        if (!h || h->distrutta || h->persa || !n || n->distrutta || n->persa) {
            if (h && h->distrutta) s.elicottero_pronto = 1e18;
            s.elicottero = -1;
            continue;
        }
        const ControlloreAsw* c = controllore_asw(h->id);
        if (!c) continue;
        if (c->stato() != StatoAsw::rientro) {
            // la nave dirige l'elicottero di continuo (MATCH, disassemblatore): ogni CUE_OGNI_S il suo contatto sul
            // bersaglio, se ne ha la posizione, col raggio dall'errore della sua traccia
            if (tempo_ - s.ultimo_cue < CUE_OGNI_S) continue;
            const auto cf = std::find_if(asw_->configurazioni.begin(), asw_->configurazioni.end(), [&](const ConfigAereoAsw& x) { return x.unita == h->id; });
            if (cf == asw_->configurazioni.end()) continue;
            for (const int b : cf->bersagli) {
                const auto k = n->contatti.find(b);
                if (k == n->contatti.end() || !k->second.posizione_nota) continue;
                double raggio = RAGGIO_CUE_ECO_M;   // eco, radar, vista
                if (const auto t = tma_ia_.find({id, b}); t != tma_ia_.end() && t->second.data && t->second.stima && t->second.stima->osservabile &&
                                                          k->second.pos_stimata.x == t->second.data->x && k->second.pos_stimata.y == t->second.data->y)
                    raggio = std::hypot(t->second.stima->sigma_distanza_m, t->second.stima->sigma_trasversale_m);
                Cue q;
                q.tipo = Cue::Tipo::contatto_nave;
                q.pos = k->second.pos_stimata;
                q.t = tempo_;
                q.raggio_m = std::clamp(raggio, RAGGIO_CUE_ECO_M, RAGGIO_CUE_MAX_M);
                q.unita = id;
                cue_asw(h->id, q);
                s.ultimo_cue = tempo_;
                break;
            }
            continue;
        }
    }
}

}  // namespace m688
