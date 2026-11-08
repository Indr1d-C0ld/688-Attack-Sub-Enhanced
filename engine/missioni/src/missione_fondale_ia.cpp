#include "missioni/missione.hpp"

#include <algorithm>
#include <cmath>

// L'IA che sonda il fondo (docs/24 §3; nucleo/sondaggio_ia): ogni sottomarino dell'IA nell'Enhanced ha il suo
// QuadroFondale sulla carta della sua parte, ci mette i suoi sondaggi e le spazzate del BQS-15 e ne ricava la quota e la
// velocita' prudenti, con le stesse regole d'emissione del giocatore (ogni ping va agli intercettatori). Le spazzate
// del sonar del fondo (il BQS-15 dei Los Angeles, l'MG-70 e l'MG-24 dei sovietici) le fa questo passo nel quadro
// dell'unita': l'impulso del BQS-15 lo emette gia' spazzata_bqs15, quello dei sovietici si emette qui. Primo passo: il
// fondale vero resta ai piloti e agli incagli; dal quadro vengono solo limiti piu' prudenti agli ordini.

namespace m688 {

namespace {
constexpr double PASSO_S = 5.0;              // la decisione ogni 5 s
constexpr double QUOTA_MIN_IA_M = 20.0;      // il limite di quota non porta mai piu' su di cosi'
constexpr double VELOCITA_MIN_IA_KN = 4.0;   // ne' la velocita' sotto questa
constexpr double SPAZZATA_S = 5.0;           // le spazzate del sonar del fondo, come il BQS-15 (missione_bqs15.cpp)
constexpr double CAMPO_NOTO_M = 5000.0;      // un campo minato noto cosi' vicino: il sonar da mine acceso (STIMA)
}  // namespace

void Missione::passo_fondale_ia() {
    if (!catalogo_ || !par_eco_ || !fondale_ || tempo_ < prossimo_fondale_ia_) return;
    prossimo_fondale_ia_ = tempo_ + PASSO_S;
    const auto* g = giocatore();
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (g && id == g->id) || u.categoria != "sottomarino" || u.quota < 5.0) continue;
        auto it = fondale_ia_.find(id);
        if (it == fondale_ia_.end()) {
            const Parte parte = (u.flag & 0x18) == 0x10 ? Parte::urss : Parte::nato;
            const Fondale* carta = fondale_carta(parte);
            if (!carta) continue;
            FondaleIa n{QuadroFondale(*carta), Casuale(seme_ ^ 0xF0DA1Aull ^ (static_cast<std::uint64_t>(id) * 0x9E37ull)), 0.0, -1e9, std::nullopt};
            n.scala = scala_ecoscandaglio(*par_eco_, n.caso);
            try { n.bqs = bqs15_fondo_di_classe(acustica_toml_, u.classe); } catch (const std::exception&) {}
            it = fondale_ia_.emplace(id, std::move(n)).first;
        }
        FondaleIa& f = it->second;
        const bool sovietico = (u.flag & 0x18) == 0x10;
        SituazioneFondo s;
        s.pos_stimata = posizione_stimata(id);
        s.sigma_posizione_m = sigma_navigazione_m(id).value_or(0.0);
        s.quota_m = u.quota;
        s.rotta = u.rotta;
        s.velocita_kn = std::fabs(u.velocita);
        s.t = tempo_;
        s.ultimo_sondaggio_t = f.ultimo_ping;
        s.sotto_ghiaccio = sotto_banchisa(id);
        // un campo minato noto entro CAMPO_NOTO_M: le mine della sua parte, o quelle che ha gia' visto (runtime, 30/10/2026:
        // il BQS-15 dell'IA si accende nei campi, e il pilota aggira le mine viste; missione_bqs15.cpp)
        for (const Mina& m : campo_minato_.mine()) {
            if (m.stato == Mina::Stato::scoppiata || distanza(m.pos, u.pos) > CAMPO_NOTO_M) continue;
            const auto v = mine_scoperte_ia_.find(id);
            const bool vista = v != mine_scoperte_ia_.end() &&
                               std::any_of(v->second.begin(), v->second.end(), [&](const MinaScoperta& x) { return x.mina == m.id; });
            if (m.lato == (u.flag & 0x18) || vista) { s.campo_minato = true; break; }
        }
        // il livello d'emissione dall'allerta: tranquillo naviga, in allarme riduce, a contatto col nemico tace
        double ostile = 1e9;
        for (const auto& [cid, c] : u.contatti)
            if (const auto* b = trova(cid); b && !b->distrutta && c.posizione_nota && ostili(u, *b)) ostile = std::min(ostile, distanza(u.pos, c.pos_stimata));
        s.ostile_vicino_m = ostile;
        const int allerta = allerta_unita(id).value_or(0);
        s.livello = allerta >= 3 || ostile < 15000.0 ? LivelloEmissioni::silenzio
                    : allerta >= 1               ? LivelloEmissioni::ridotto
                                                 : LivelloEmissioni::navigazione;
        const DecisioneFondo d = decidi_fondo(f.quadro, s, dottrina_fondo(sovietico));
        if (d.sonda) {
            const double ora = std::fmod(std::fmod((ora_utc_s_ + tempo_) / 3600.0 + longitudine_ / 15.0, 24.0) + 24.0, 24.0);
            Sondaggio x = sonda(*fondale_, u.pos, s.pos_stimata, u.quota, s.velocita_kn, tempo_, d.potenza, ora >= 6.0 && ora < 18.0, f.scala,
                                *par_eco_, f.caso);
            f.quadro.aggiungi(x, s.sigma_posizione_m);
            if (catalogo_int_) emissioni_attive_.push_back({emissione(x, id, u.rotta, s.velocita_kn, *par_eco_), 180.0, 0.0});
            f.ultimo_ping = tempo_;
        }
        // il sonar del fondo: il BQS-15 acceso o spento (le mine e l'impulso in missione_bqs15.cpp); la spazzata del fondo
        // e delle pareti nel suo quadro, qui
        const bool los_angeles = ha_bqs15(u.classe);
        if (los_angeles) bqs15(id, d.bqs15);
        // solo un sonar che guarda anche sotto l'orizzonte sonda fondo e pareti: il BQS-8 degli Sturgeon guarda in alto, ed e'
        // della volta (missione_ghiaccio.cpp); col suo ventaglio corto qui frenava lo Sturgeon a 4 nodi (docs/ricerca/34)
        if (d.bqs15 && f.bqs && f.bqs->elevazione_min_deg < 0.0 && tempo_ + 1e-9 >= f.prossima_spazzata) {
            f.prossima_spazzata = tempo_ + SPAZZATA_S;
            for (const RitornoFondo& r : spazza_fondo(*fondale_, u.pos, s.pos_stimata, u.quota, u.rotta, s.velocita_kn, tempo_, *f.bqs, f.caso))
                f.quadro.aggiungi(r, s.sigma_posizione_m);
            // l'MG-70, l'MG-24 e il BQS-8 degli Sturgeon nel settore di prua (emissioni.<emissione> di acustica.toml; livello
            // STIMA)
            const bool bqs8 = f.bqs->emissione == "bqs8";
            if (!los_angeles) emetti_impulso(id, f.bqs->emissione, bqs8 ? 205.0 : 210.0, bqs8 ? 40000.0 : 20000.0, f.bqs->settore_az_deg, 20.0);
            // e le mine nel settore, come il BQS-15 (missione_bqs15.cpp, runtime): il pilota le aggira. Il BQS-8 degli Sturgeon
            // non le vede (docs/ricerca/34)
            if (!los_angeles && f.bqs->mine) rileva_mine(u, mine_scoperte_ia_[id], 1.0, caso_mine_ia_);
        }
        // i limiti prudenti: solo verso il prudente, mai sopra QUOTA_MIN_IA_M ne' sotto VELOCITA_MIN_IA_KN
        // (il limite di 20 m non deve mai far scendere chi sta piu' su, per esempio a quota periscopica in acqua bassa)
        if (const double q = std::max(QUOTA_MIN_IA_M, d.quota_max_m); q < u.quota_ordinata) u.quota_ordinata = q;
        if (d.velocita_max_kn < std::fabs(u.velocita_ordinata))
            u.velocita_ordinata = std::copysign(std::max(VELOCITA_MIN_IA_KN, d.velocita_max_kn), u.velocita_ordinata);
    }
}

}  // namespace m688
