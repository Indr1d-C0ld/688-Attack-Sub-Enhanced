// Il quadro del fondale del giocatore nell'Enhanced (docs/24 §3; i moduli sono nucleo/ecoscandaglio, ritorni_fondo e
// quadro_fondale, del disassemblatore). Qui il collegamento:
// - un QuadroFondale sulla carta della parte del giocatore (quella d'epoca, o il fondale vero con la carta «vera»);
// - l'ecoscandaglio: a comando o periodico ogni N minuti, a potenza piena o bassa; il ping sul fondale vero dalla
//   posizione vera, la misura sulla stimata con la sigma della navigazione;
// - il BQS-15 acceso: ogni spazzata da' anche i ritorni del fondo e delle pareti (spazza_fondo);
// - i punti nave correggono il mosaico (missione_navigazione.cpp chiama quadro_g_->punto_nave);
// - ogni ping e' un'emissione per i ricevitori d'intercettazione (contro-scoperta);
// - gli avvisi dell'equipaggio dalle sole misure: messaggi 480-485 (testi in tools/pipeline/esporta_motore.py), uno
//   per avviso quando diventa vero.
// Il classico non lo usa (serve l'acustica). L'IA ha il suo quadro (missione_fondale_ia.cpp); il suo pilota resta sul
// fondale vero, il quadro le da' solo limiti piu' prudenti.

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double AVVISI_OGNI_S = 2.0;
constexpr double POCA_ACQUA_M = 100.0, POCHISSIMA_ACQUA_M = 30.0;   // sotto la chiglia (STIMA)
constexpr int MSG_POCA_ACQUA = 480, MSG_POCHISSIMA_ACQUA = 481, MSG_FONDO_SALE = 482, MSG_OSTACOLO = 483,
              MSG_TROPPO_VELOCE = 484, MSG_NESSUN_ECO = 485;
// i tipi degli avvisi detti, oltre a quelli del quadro (AvvisoFondo::Tipo): le due soglie dell'acqua sotto la chiglia
constexpr int AVVISO_POCA = 100, AVVISO_POCHISSIMA = 101;

}  // namespace

void Missione::dati_fondale(const std::string& cartella_dati) {
    // l'ecoscandaglio e i sonar del fondo servono anche ai sottomarini dell'IA (missione_fondale_ia.cpp)
    const std::string f = cartella_dati + "/acustica.toml";
    try { par_eco_ = carica_ecoscandaglio(f); } catch (const std::exception&) { return; }
    acustica_toml_ = f;
    const auto* g = giocatore();
    if (!g || g->categoria != "sottomarino") return;
    try { par_bqs_fondo_ = bqs15_fondo_di_classe(f, g->classe); } catch (const std::exception&) {}
}

bool Missione::prepara_quadro_fondale() {
    if (quadro_g_) return true;
    if (fondale_tentato_ || !par_eco_ || !fondale_) return false;
    fondale_tentato_ = true;
    const auto* g = giocatore();
    if (!g || g->categoria != "sottomarino") return false;   // il quadro e' del giocatore sottomarino (l'IA ha il suo)
    // la carta della sua parte: impostata dopo il caricamento (partita.cpp), quindi al primo passo e non prima
    const Parte parte = (g->flag & 0x18) == 0x10 ? Parte::urss : Parte::nato;
    quadro_g_.emplace(*fondale_carta(parte));
    caso_fondale_.emplace(seme_ ^ 0xEC05CA11ull);
    scala_eco_ = scala_ecoscandaglio(*par_eco_, *caso_fondale_);
    return true;
}

const QuadroFondale* Missione::quadro_fondale(int id) const {
    const auto* g = giocatore();
    if (g && g->id == id) return quadro_g_ ? &*quadro_g_ : nullptr;
    const auto it = fondale_ia_.find(id);   // i sottomarini dell'IA (missione_fondale_ia.cpp)
    return it != fondale_ia_.end() ? &it->second.quadro : nullptr;
}

std::optional<Missione::StatoEcoscandaglio> Missione::ecoscandaglio(int id) const {
    const auto* g = giocatore();
    if (!g || g->id != id || !par_eco_) return std::nullopt;
    return StatoEcoscandaglio{prog_eco_.periodo_min(), potenza_eco_, ultimo_sondaggio_};
}

bool Missione::ordine_ecoscandaglio(const Ordine& o) {
    if (!par_eco_ || !catalogo_) return false;
    switch (o.tipo) {
        case TipoOrdine::ecoscandaglio_ping: prog_eco_.singolo(); return true;
        case TipoOrdine::ecoscandaglio_periodo:
            if (o.a < 0.0) return false;
            prog_eco_.periodico(o.a <= 0.0 ? 0.0 : std::clamp(o.a, 1.0, 30.0));
            return true;
        case TipoOrdine::ecoscandaglio_potenza: potenza_eco_ = o.n != 0 ? PotenzaEco::bassa : PotenzaEco::piena; return true;
        default: return false;
    }
}

void Missione::spazzata_fondo(const UnitaClassica& u) {
    if (!par_bqs_fondo_ || !prepara_quadro_fondale()) return;
    const double sigma = sigma_navigazione_m(u.id).value_or(0.0);
    for (const RitornoFondo& r : spazza_fondo(*fondale_, u.pos, posizione_stimata(u.id), u.quota, u.rotta, std::fabs(u.velocita), tempo_,
                                              *par_bqs_fondo_, *caso_fondale_))
        quadro_g_->aggiungi(r, sigma);
}

void Missione::passo_quadro_fondale() {
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa || !catalogo_ || !prepara_quadro_fondale()) return;
    const Vec2 stimata = posizione_stimata(g->id);
    if (prog_eco_.passo(tempo_)) {
        const double ora = std::fmod(std::fmod((ora_utc_s_ + tempo_) / 3600.0 + longitudine_ / 15.0, 24.0) + 24.0, 24.0);
        const bool giorno = ora >= 6.0 && ora < 18.0;   // lo strato diffondente sale di notte (STIMA: 6-18 ora locale)
        Sondaggio s = sonda(*fondale_, g->pos, stimata, g->quota, std::fabs(g->velocita), tempo_, potenza_eco_, giorno, scala_eco_,
                            *par_eco_, *caso_fondale_);
        quadro_g_->aggiungi(s, sigma_navigazione_m(g->id).value_or(0.0));
        if (catalogo_int_) emissioni_attive_.push_back({emissione(s, g->id, g->rotta, std::fabs(g->velocita), *par_eco_), 180.0, 0.0});
        if (!s.eco && (!ultimo_sondaggio_ || ultimo_sondaggio_->eco)) messaggio_di_bordo(g->id, MSG_NESSUN_ECO);   // una volta per tratto
        ultimo_sondaggio_ = std::move(s);
    }
    if (tempo_ < prossimi_avvisi_fondo_) return;
    prossimi_avvisi_fondo_ = tempo_ + AVVISI_OGNI_S;
    // gli avvisi dalle misure: ognuno detto una volta quando diventa vero, di nuovo dopo che e' tornato falso
    std::set<int> attivi;
    for (const AvvisoFondo& a : quadro_g_->avvisi(stimata, g->quota, g->rotta, std::fabs(g->velocita), tempo_)) {
        int tipo = static_cast<int>(a.tipo), codice = 0;
        switch (a.tipo) {
            case AvvisoFondo::Tipo::sotto_chiglia:
                if (a.valore >= POCA_ACQUA_M) continue;
                tipo = a.valore < POCHISSIMA_ACQUA_M ? AVVISO_POCHISSIMA : AVVISO_POCA;
                codice = a.valore < POCHISSIMA_ACQUA_M ? MSG_POCHISSIMA_ACQUA : MSG_POCA_ACQUA;
                break;
            case AvvisoFondo::Tipo::fondo_in_salita: codice = MSG_FONDO_SALE; break;
            case AvvisoFondo::Tipo::ostacolo_a_prua: codice = MSG_OSTACOLO; break;
            case AvvisoFondo::Tipo::velocita_eccessiva: codice = MSG_TROPPO_VELOCE; break;
        }
        attivi.insert(tipo);
        if (!avvisi_fondo_attivi_.count(tipo)) messaggio_di_bordo(g->id, codice);
    }
    avvisi_fondo_attivi_ = std::move(attivi);
}

}  // namespace m688
