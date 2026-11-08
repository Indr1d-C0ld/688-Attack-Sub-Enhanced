// Il ricevitore d'intercettazione nella missione dell'Enhanced (blocco 2 del sonar, E; docs/22_sonar_blocco2.md §2;
// modulo puro nucleo/intercettazione.hpp). Il collegamento:
// - chi emette registra l'impulso (emetti_impulso): il sonar attivo di scafo (impulso_sonar_enhanced), il BQS-15 col
//   suo settore e i lobi laterali, le teste dei siluri alla loro cadenza (passo_transitori_armi), le boe attive e i
//   sonar a immersione dell'ASW aereo (missione_asw.cpp, emetti_impulso_da);
// - a fine passo ogni unita' con un intercettatore della classe (WLR-9, Skat, Rubin) ascolta gli impulsi degli altri,
//   col livello verso di lei, col Ricevitore del suo sensore d'intercettazione;
// - l'allarme che sale: al giocatore il messaggio (311 siluro in acqua, 475 in acquisizione); l'IA
//   evade (stimola_evasione) dalla direzione della traccia, la simmetria.
// La scoperta del contatto dal ping (intercetta_impulso) resta quella di prima.

#include <cmath>
#include <memory>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr int MSG_SILURO_IN_ACQUA = 311;
constexpr int MSG_SILURO_ACQUISIZIONE = 475;   // nel blocco 470-479 della navigazione (testo in esporta_motore.py)
constexpr double DA_TRACCIA_M = 3000.0;      // l'IA evade da un punto a 3 km sul rilevamento della traccia (STIMA)

}  // namespace

const RicevitoreIntercettazione* Missione::intercettatore(int id) const {
    const auto it = intercettatori_.find(id);
    return it == intercettatori_.end() ? nullptr : it->second.get();
}

const RicevitoreIntercettazione* Missione::intercettatore_giocatore() const {
    const auto* g = giocatore();
    return g ? intercettatore(g->id) : nullptr;
}

void Missione::emetti_impulso(int id, const std::string& chiave, double sl_db, double f_hz, double settore_deg, double lobi_db) {
    if (!catalogo_int_) return;
    const auto it = unita_.find(id);
    if (it == unita_.end()) return;
    const UnitaClassica& u = it->second;
    EmissioneMissione x;
    x.e.emettitore = id;
    x.e.chiave = chiave;
    x.e.pos = u.pos;
    x.e.quota_m = u.quota;
    x.e.rotta = u.rotta;
    x.e.velocita_kn = std::fabs(u.velocita);
    x.e.t = tempo_;
    x.e.sl_db = sl_db;
    x.e.f_hz = f_hz;
    x.settore_deg = settore_deg;
    x.lobi_db = lobi_db;
    emissioni_attive_.push_back(std::move(x));
}

void Missione::emetti_impulso_da(int id, Vec2 pos, double quota_m, const std::string& chiave, double sl_db) {
    if (!catalogo_int_) return;
    EmissioneMissione x;
    x.e.emettitore = id;
    x.e.chiave = chiave;
    x.e.pos = pos;
    x.e.quota_m = quota_m;   // la boa e il trasduttore sono fermi: niente Doppler
    x.e.t = tempo_;
    x.e.sl_db = sl_db;
    emissioni_attive_.push_back(std::move(x));
}

void Missione::passo_intercettazione(double dt) {
    if (!catalogo_int_) { emissioni_attive_.clear(); return; }
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (u.flag & 0x20) || (u.flag & 3) == 3) continue;
        const DatiIntercettatore* d = catalogo_int_->intercettatore_di(u.classe);
        if (!d) continue;
        auto& rp = intercettatori_[id];
        if (!rp) rp = std::make_unique<RicevitoreIntercettazione>(*d, *catalogo_int_, seme_ ^ (0x1A7E5ull + static_cast<std::uint64_t>(id) * 0x9E37ull));
        // gli impulsi degli altri, col livello verso di lei (il fascio del BQS-15)
        std::vector<EmissioneAttiva> v;
        for (const auto& x : emissioni_attive_) {
            if (x.e.emettitore == id) continue;
            EmissioneAttiva e = x.e;
            if (x.settore_deg < 180.0 && std::fabs(normalizza_180(rilevamento(e.pos, u.pos) - e.rotta)) > x.settore_deg) e.sl_db -= x.lobi_db;
            v.push_back(std::move(e));
        }
        const Sensore* sn = nullptr;   // il sensore d'intercettazione della classe, per il rumore proprio
        for (const Sensore* s : catalogo_->sensori_di(u.classe))
            if (s->id.rfind("wlr", 0) == 0 || s->id.rfind("intercettazione", 0) == 0) sn = s;
        rp->ascolta(v, ricevitore_acustico(u, sn), ambiente_acustico(u.pos, u.pos), tempo_, dt);
        // l'allarme che sale
        const Allarme a = rp->allarme();
        Allarme& prima = allarmi_intercettazione_[id];
        if (static_cast<int>(a) > static_cast<int>(prima)) {
            if (u.giocatore) {
                conti_.allarmi_siluro.push_back({tempo_, a == Allarme::siluro_in_acqua ? 1 : 2});   // docs/26
                if (a == Allarme::siluro_in_acqua) messaggio_sonar(MSG_SILURO_IN_ACQUA);
                else messaggio_sonar(MSG_SILURO_ACQUISIZIONE);
            } else if (forze_ && !difesa_spenta_ && u.categoria == "sottomarino") {
                double ril = u.rotta + 180.0;   // senza traccia: da poppa
                for (const auto& t : rp->tracce())
                    if (t.allarme == a) ril = t.rilevamento;
                const double r = radianti(ril);
                stimola_evasione(id, a == Allarme::siluro_in_acquisizione ? StimoloEvasione::siluro_vicino : StimoloEvasione::siluro,
                                 {u.pos.x + DA_TRACCIA_M * std::sin(r), u.pos.y + DA_TRACCIA_M * std::cos(r)}, 0.0, -1);
            }
        }
        prima = a;
    }
    emissioni_attive_.clear();
}

}  // namespace m688
