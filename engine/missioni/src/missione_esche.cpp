#include <algorithm>
#include <string>

#include "missioni/missione.hpp"
#include "nucleo/acustica.hpp"

// Esche semoventi dell'Enhanced (dossier 05 §5.1-5.3 e §7): l'MG-74 sovietica dal tubo da 533 e la MOSS Mk 70 degli
// SSBN (sul 688 del 1989 e' un what-if). Corrono a quota fissa con una manovra programmata, imitano rumore ed eco del
// battello e, muovendosi, superano la verifica Doppler della testa che smaschera l'ADC Mk 2 stazionaria. Sono unita'
// come le esche dell'originale (flag 193: falso bersaglio, mai un colpo), mosse dal moto classico.

namespace m688 {

namespace {
constexpr double VIRATA_ESCA_GRADI_S = 3.0;   // STIMA: come un siluro lento
constexpr double QUOTA_ESCA_M_S = 2.0;
}  // namespace

int Missione::lancia_esca_mobile(int lanciatore, const std::string& classe, double rotta, double quota) {
    const DatiEsca* d = siluri_attivi_ ? tabella_siluri_.esca(classe) : nullptr;
    const UnitaClassica* b = trova(lanciatore);
    if (!d || !d->mobile() || !b || b->distrutta || b->persa) return -1;
    UnitaClassica e;
    e.id = prossimo_id_++;
    e.classe = classe;
    e.categoria = "arma";
    e.flag = 193;   // come le esche dell'IA: falso bersaglio, nessun impatto
    e.pos = b->pos;
    double q = quota >= 0.0 ? quota : d->quota_m > 0.0 ? d->quota_m : b->quota;
    if (d->quota_max_m > 0.0) q = std::min(q, d->quota_max_m);
    // parte gia' alla quota impostata, entro la sua massima (STIMA: il tubo la spinge fuori come un siluro)
    e.quota = e.quota_ordinata = std::max(q, 1.0);
    e.quota_max = d->quota_max_m > 0.0 ? d->quota_max_m : b->quota_max;
    e.rateo_quota_modello = QUOTA_ESCA_M_S;
    e.rotta = e.rotta_ordinata = normalizza_360(rotta);
    e.virata = VIRATA_ESCA_GRADI_S;
    e.velocita = e.velocita_ordinata = e.avanti_tutta = d->velocita_kn;
    e.integrita = e.integrita_max = 2;
    e.rumore = b->rumore;
    e.collegata = e.lanciatore = b->id;
    ++create_;
    unita_[e.id] = e;
    esche_mobili_[e.id] = {d, tempo_, e.rotta, b->classe};
    return e.id;
}

const DatiEsca* Missione::esca_mobile(int id) const {
    const auto it = esche_mobili_.find(id);
    return it == esche_mobili_.end() ? nullptr : it->second.dati;
}

bool Missione::contatto_falso(int id) const {
    const auto it = esche_mobili_.find(id);
    return it != esche_mobili_.end() && !it->second.dati;
}

// Il falso contatto (D3, docs/28 §3.5; nota d3-contatto-falso-contratto.md): pinga col sonar attivo della classe imitata,
// come un battello vero, per gli intercettatori e i sensori passivi di chi ascolta (l'impulso di sola andata). Nessuna
// eco: chi pinga e' una sorgente sola, senza scafo che ascolti
void Missione::ping_falso(UnitaClassica& u, const EscaMobile& x) {
    const Sensore* sa = nullptr;
    for (const Sensore* s : catalogo_->sensori_di(x.classe_lanciatore))
        if (s->modo == ModoSensore::attivo && s->antenna != Antenna::lineare) { sa = s; break; }
    if (!sa) return;
    emetti_impulso_da(u.id, u.pos, u.quota, sa->id, sa->sl_attivo_db);
    auto sp = sorgente_acustica(u);
    if (!sp) return;
    sp->ping_db = sa->sl_attivo_db;
    sp->ping_hz = sa->bande_hz.empty() ? 3500.0 : sa->bande_hz.front();
    sp->ping_s = sa->impulso_s;
    for (auto& [idb, b] : unita_)
        if (idb != u.id && !b.distrutta && !b.persa) intercetta_impulso(u, b, *sp, ambiente_acustico(u.pos, b.pos));
}

void Missione::passo_esche_mobili() {
    for (auto it = esche_mobili_.begin(); it != esche_mobili_.end();) {
        UnitaClassica* u = trova(it->first);
        EscaMobile& x = it->second;
        const double t = tempo_ - x.inizio;
        if (!x.dati) {   // il falso contatto: rotta, velocita' e quota fisse; la fine la da' l'intervento (fine_intervento)
            if (!u || u->distrutta || u->persa) {
                it = esche_mobili_.erase(it);
                continue;
            }
            u->rotta_ordinata = x.rotta0;
            u->velocita_ordinata = x.velocita_kn;
            if (x.ping_s > 0.0 && catalogo_ && tempo_ >= x.prossimo_ping - 1e-9) {
                x.prossimo_ping = tempo_ + x.ping_s;
                ping_falso(*u, x);
            }
            ++it;
            continue;
        }
        if (!u || u->distrutta || u->persa || t >= x.dati->durata_s) {   // batteria finita: esce dal gioco
            if (u && !u->persa) togli(*u);
            it = esche_mobili_.erase(it);
            continue;
        }
        u->rotta_ordinata = rotta_esca_mobile(*x.dati, x.rotta0, t);
        u->velocita_ordinata = x.dati->velocita_kn;
        ++it;
    }
}

}  // namespace m688
