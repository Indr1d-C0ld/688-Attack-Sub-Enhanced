#include "nucleo/armi_giocatore.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {
// per Arma; "" al posto di nessuna
constexpr std::array<std::string_view, 10> NOMI = {"mk48", "type53", "sea_lance", "tomahawk", "harpoon", "esca", "",
                                                   "mina", "attacco_terra", "esca_mobile"};
const std::string VUOTA;
}  // namespace

Arma arma_da_nome(std::string_view nome) {
    for (std::size_t i = 0; i < NOMI.size(); ++i)
        if (!nome.empty() && nome == NOMI[i]) return static_cast<Arma>(i);
    return Arma::nessuna;
}

std::string_view nome_arma(Arma a) { return NOMI[static_cast<std::size_t>(a)]; }

bool e_siluro(Arma a) { return a == Arma::mk48 || a == Arma::type53; }
bool e_missile(Arma a) {
    return a == Arma::sea_lance || a == Arma::tomahawk || a == Arma::harpoon || a == Arma::attacco_terra;
}

StazioneArmi::StazioneArmi(const std::vector<std::string>& tubi, const std::map<std::string, int>& dotazione,
                           int numero_tubi, int limite_in_acqua)
    : numero_tubi_(std::clamp(numero_tubi, 0, TUBI_MAX)), limite_(limite_in_acqua) {
    arma_.fill(Arma::nessuna);
    for (int t = 1; t <= numero_tubi_ && indice(t) < tubi.size(); ++t) arma_[indice(t)] = arma_da_nome(tubi[indice(t)]);
    for (const auto& [nome, n] : dotazione)
        if (const Arma a = arma_da_nome(nome); a != Arma::nessuna) scorte_[static_cast<std::size_t>(a)] = n;
}

int StazioneArmi::scorte(Arma a) const { return a == Arma::nessuna ? 0 : scorte_[static_cast<std::size_t>(a)]; }

std::vector<std::pair<Arma, int>> StazioneArmi::scorte_tutte() const {
    std::vector<std::pair<Arma, int>> out;
    for (std::size_t i = 0; i < NOMI.size(); ++i) {
        const Arma a = static_cast<Arma>(i);
        if (a == Arma::nessuna) continue;
        const bool nei_tubi = std::find(arma_.begin(), arma_.begin() + numero_tubi_, a) != arma_.begin() + numero_tubi_;
        if (scorte_[i] > 0 || nei_tubi) out.emplace_back(a, scorte_[i]);
    }
    return out;
}

ParametriRicarica parametri_ricarica(const std::string& classe) {
    ParametriRicarica p;   // 688 e le classi senza dati: 300 s, due alla volta
    if (classe == "alfa") p.secondi = 75.0;                                    // caricatore automatico
    else if (classe == "sturgeon") p.secondi = 420.0;                          // prima del carrello idraulico (1976)
    else if (classe == "trafalgar" || classe == "victor3") p.secondi = 240.0;
    return p;
}

void StazioneArmi::attiva_ricarica(const ParametriRicarica& p) {
    if (ricarica_attiva_) return;
    ricarica_attiva_ = true;
    parametri_ = p;
    if (!p.tubi_carichi_alla_partenza || con_carico_) return;
    for (int t = 1; t <= numero_tubi_; ++t) {   // in pattugliamento i tubi sono carichi, non allagati (dossier 14 §3.4)
        if (arma(t) == Arma::nessuna || ausiliario(t) || stato(t) != StatoTubo::vuoto) continue;
        if (preleva(t)) stato_[indice(t)] = StatoTubo::carico;
    }
}

bool StazioneArmi::in_coda(int tubo) const {
    return valido(tubo) && std::find(coda_.begin(), coda_.end(), tubo) != coda_.end();
}

int StazioneArmi::ricariche_in_corso() const {
    int n = 0;
    for (int t = 1; t <= numero_tubi_; ++t) n += stato(t) == StatoTubo::in_ricarica && !in_coda(t);
    return n;
}

double StazioneArmi::secondi_ricarica(int tubo) const {
    if (!valido(tubo) || stato(tubo) != StatoTubo::in_ricarica) return 0.0;
    return std::max(0.0, parametri_.secondi - lavoro_[indice(tubo)]) * (silenzio_ ? parametri_.fattore_silenzio : 1.0);
}

EsitoRicarica StazioneArmi::avanza(double tempo) {
    EsitoRicarica e;
    const double dt = std::max(0.0, tempo - tempo_);
    tempo_ = tempo;
    if (!ricarica_attiva_) return e;
    const double passo = dt / (silenzio_ ? parametri_.fattore_silenzio : 1.0);
    for (int t = 1; t <= numero_tubi_; ++t) {
        if (stato(t) != StatoTubo::in_ricarica || in_coda(t)) continue;
        const std::size_t i = indice(t);
        lavoro_[i] += passo;
        if (!silenzio_) {   // i colpi del carrello e della culatta
            rumore_[i] += passo;
            for (; rumore_[i] >= parametri_.colpo_ogni_s; rumore_[i] -= parametri_.colpo_ogni_s) ++e.transitori;
        }
        if (lavoro_[i] + 1e-9 < parametri_.secondi) continue;
        e.messaggi.push_back(MSG_RICARICATO + t - 1);
        stato_[i] = StatoTubo::carico;
        if (parametri_.arma_quando_carico && !akkord_guasto()) {   // con Akkord guasto la ricarica finisce, l'arma no   // si allaga, si equalizza e si apre: l'armamento dell'originale
            stato_[i] = StatoTubo::in_armamento;
            e.alla_missione.push_back({tempo, TipoOrdine::carica_tubo, 0.0, 0.0, t});
        }
    }
    while (!coda_.empty() && ricariche_in_corso() < parametri_.contemporanee) coda_.erase(coda_.begin());
    return e;
}

EsitoStazione StazioneArmi::clic_tubo(int tubo, double tempo) {
    EsitoStazione e;
    if (!valido(tubo) || arma(tubo) == Arma::nessuna) return e;
    if (tubo != selezionato_) {   // primo clic: tabella DS:01F6 secondo il tipo d'arma
        selezionato_ = tubo;
        e.messaggio = e_siluro(arma(tubo)) ? MSG_TUBO_SILURI : e_missile(arma(tubo)) ? MSG_TUBO_MISSILI : MSG_TUBO_AUSILIARIO;
        return e;
    }
    if (stato(tubo) == StatoTubo::vuoto && scorta_tubo(tubo) == 0) {   // il programma non manda nemmeno il comando
        e.messaggio = MSG_ARMA_ESAURITA;
        return e;
    }
    e.alla_missione.push_back({tempo, TipoOrdine::carica_tubo, 0.0, 0.0, tubo});
    return e;
}

bool StazioneArmi::riguarda(const Ordine& o) const {
    switch (o.tipo) {
        case TipoOrdine::carica_tubo:
        case TipoOrdine::lancia:
        case TipoOrdine::designa_bersaglio:
        case TipoOrdine::seleziona_siluro:
        case TipoOrdine::guida_punto:
        case TipoOrdine::guida_cerca:
        case TipoOrdine::guida_detona:
        case TipoOrdine::imposta_siluro:
        case TipoOrdine::silenzio_a_bordo:
        case TipoOrdine::scegli_arma_tubo:
        case TipoOrdine::akkord_designa:
        case TipoOrdine::akkord_scegli: return true;
        default: return false;
    }
}

EsitoStazione StazioneArmi::consegna(const Ordine& o, const SituazioneArmi& s) {
    if (bloccato(o)) return {{}, MSG_AKKORD_GUASTO};
    switch (o.tipo) {
        case TipoOrdine::akkord_designa: return designa_akkord(o);
        case TipoOrdine::akkord_scegli: {
            if (!akkord_ || o.n < 1 || o.n > SLOT_AKKORD || slot_[static_cast<std::size_t>(o.n - 1)] < 0) return {};
            scelto_ = o.n;
            bersaglio_ = slot_[static_cast<std::size_t>(o.n - 1)];
            return {{{o.tempo, TipoOrdine::designa_bersaglio, 0.0, 0.0, bersaglio_}}, 0};
        }
        case TipoOrdine::carica_tubo: return carica(o.n, o.tempo);
        case TipoOrdine::lancia: return lancia(o.n, o.tempo, s);
        case TipoOrdine::designa_bersaglio:   // TARGET: il bersaglio selezionato, che il lancio ripete con 38h
            if (akkord_ && o.n >= 0) return designa_akkord(o);   // sull'Alfa lo prende Akkord in uno slot
            bersaglio_ = o.n;
            if (o.n < 0) return {};            // Esc nella scelta: nessun bersaglio (DS:0368 = -1), niente comando
            return {{o}, 0};
        case TipoOrdine::seleziona_siluro:    // SELEZ.: solo la scelta sul pannello, 38h parte con il comando di guida
            arma_guidata_ = o.n;
            return {};
        case TipoOrdine::guida_punto:
        case TipoOrdine::guida_cerca:
        case TipoOrdine::guida_detona: return guida(o, s);
        case TipoOrdine::silenzio_a_bordo: {   // Enhanced; per ora riguarda solo le ricariche e i loro transitori
            EsitoStazione e;
            if (!ricarica_attiva_) return e;
            const bool prima = silenzio_;
            silenzio_ = o.n != 0;
            if (silenzio_ != prima) e.messaggio = silenzio_ ? MSG_SILENZIO : MSG_FINE_SILENZIO;
            return e;
        }
        case TipoOrdine::scegli_arma_tubo: return scegli_arma_tubo(o.n, static_cast<int>(std::lround(o.a)));
        case TipoOrdine::imposta_siluro:   // Enhanced: solo lo stato della stazione, il modello lo riceve al lancio
            if (valido(o.n) && applica(impostazioni_[indice(o.n)], static_cast<CampoSiluro>(std::lround(o.b)), o.a)) {   // siluro.hpp
                impostato_[indice(o.n)] = true;
                campi_[indice(o.n)] |= 1u << static_cast<unsigned>(std::lround(o.b));
            }
            return {};
        default: return {{o}, 0};
    }
}

// Secondo clic su un tubo (7606-76A5): vuoto -> in armamento (1Dh + tubo, una scorta in meno); in armamento o
// armato -> libero, l'arma torna nelle scorte
EsitoStazione StazioneArmi::carica(int tubo, double tempo) {
    EsitoStazione e;
    if (!valido(tubo) || arma(tubo) == Arma::nessuna) return e;
    if (ricarica_attiva_ && !ausiliario(tubo)) return carica_enhanced(tubo, tempo, false);
    auto& st = stato_[indice(tubo)];
    if (st != StatoTubo::vuoto) {
        st = StatoTubo::vuoto;
        restituisci(tubo);
        e.messaggio = MSG_TUBO_LIBERO;
        return e;
    }
    if (!preleva(tubo)) {
        e.messaggio = MSG_ARMA_ESAURITA;
        return e;
    }
    st = StatoTubo::in_armamento;
    e.alla_missione.push_back({tempo, TipoOrdine::carica_tubo, 0.0, 0.0, tubo});
    e.messaggio = MSG_IN_ARMAMENTO;
    return e;
}

// Enhanced: vuoto -> ricarica (o in coda); in ricarica -> annullata; carico -> armamento; in armamento o armato ->
// disarmato (l'arma resta nel tubo). Le ricariche automatiche dopo un lancio non annunciano l'inizio.
EsitoStazione StazioneArmi::carica_enhanced(int tubo, double tempo, bool automatica) {
    EsitoStazione e;
    const std::size_t i = indice(tubo);
    auto& st = stato_[i];
    switch (st) {
        case StatoTubo::vuoto: {
            if (!preleva(tubo)) {
                e.messaggio = MSG_ARMA_ESAURITA;
                return e;
            }
            if (pozzo(tubo)) {   // i pozzi verticali: la cella successiva e' gia' carica
                st = StatoTubo::carico;
                return e;
            }
            const bool libero = ricariche_in_corso() < parametri_.contemporanee;
            st = StatoTubo::in_ricarica;
            lavoro_[i] = rumore_[i] = 0.0;
            if (!libero) coda_.push_back(tubo);
            if (!automatica) e.messaggio = libero ? MSG_RICARICA_INIZIATA : MSG_RICARICA_IN_CODA;
            return e;
        }
        case StatoTubo::in_ricarica:
            std::erase(coda_, tubo);
            st = StatoTubo::vuoto;
            restituisci(tubo);
            e.messaggio = MSG_RICARICA_ANNULLATA;
            return e;
        case StatoTubo::carico:
            st = StatoTubo::in_armamento;
            e.alla_missione.push_back({tempo, TipoOrdine::carica_tubo, 0.0, 0.0, tubo});
            e.messaggio = MSG_IN_ARMAMENTO;
            return e;
        case StatoTubo::in_armamento:
        case StatoTubo::armato:
            st = StatoTubo::carico;   // il messaggio di fine armamento dello script si sopprime
            e.messaggio = MSG_DISARMATO;
            return e;
    }
    return e;
}

// Lancio (790C): controlli nell'ordine dell'originale, poi 38h con il bersaglio selezionato, 24h + tipo (il gestore
// di lancio della missione) e la ricarica automatica del tubo
EsitoStazione StazioneArmi::lancia(int tubo, double tempo, const SituazioneArmi& s) {
    EsitoStazione e;
    if (!valido(tubo) || stato(tubo) != StatoTubo::armato) {
        e.messaggio = MSG_NON_ARMATA;
        return e;
    }
    const Arma a = arma(tubo);
    const std::uint8_t modello = tubo >= 1 && tubo < static_cast<int>(s.missile_tubo.size()) ? s.missile_tubo[static_cast<std::size_t>(tubo)] : 0;
    if (e_missile(a) && modello) {   // Enhanced: il missile del catalogo, pronto o no secondo il suo modello
        if (modello < 2) { e.messaggio = MSG_NON_ANCORA; return e; }
    } else if (a == Arma::attacco_terra) {   // TLAM-C: nessun bersaglio in mare, solo la quota periscopica
        if (s.quota > QUOTA_PERISCOPICA + 1e-6) { e.messaggio = MSG_QUOTA_PERISCOPICA; return e; }
    } else if (e_missile(a)) {   // il tubo 7 dei missili (anche il Sea Lance), o un tubo da siluri col carico
        if (bersaglio_ < 0 || !s.bersaglio_presente) { e.messaggio = MSG_SERVE_BERSAGLIO; return e; }
        if (a == Arma::harpoon && !s.bersaglio_nave) { e.messaggio = MSG_SOLO_SUPERFICIE; return e; }
        if (!s.contatto) return e;   // senza contatto: rifiuto in silenzio
        if (!s.traccia_solida) { e.messaggio = MSG_SERVE_DISTANZA; return e; }
        const int decimi = static_cast<int>(s.distanza_m / 1852.0 * 10.0);
        if (decimi < DISTANZA_MIN_DECIMI) { e.messaggio = MSG_TROPPO_VICINO; return e; }
        if (s.quota > QUOTA_PERISCOPICA + 1e-6) { e.messaggio = MSG_QUOTA_PERISCOPICA; return e; }
    }
    // DS:03DE: tubi 1-6 e Sea Lance; le mine (M2b) non sono armi in acqua: le posa il campo minato della missione
    const bool conta = (!e_missile(a) || a == Arma::sea_lance) && a != Arma::mina;
    if (conta && in_acqua_ >= limite_) {
        e.messaggio = MSG_NON_ANCORA;
        return e;
    }
    if (conta) ++in_acqua_;
    if (bersaglio_ >= 0) e.alla_missione.push_back({tempo, TipoOrdine::designa_bersaglio, 0.0, 0.0, bersaglio_});
    e.alla_missione.push_back({tempo, TipoOrdine::lancia, 0.0, 0.0, tubo});
    lanciata_[indice(tubo)] = variante_[indice(tubo)];
    stato_[indice(tubo)] = StatoTubo::vuoto;   // ricarica automatica: come un secondo clic sul tubo vuoto
    const EsitoStazione r = ricarica_attiva_ && !ausiliario(tubo) ? carica_enhanced(tubo, tempo, true) : carica(tubo, tempo);
    e.alla_missione.insert(e.alla_missione.end(), r.alla_missione.begin(), r.alla_missione.end());
    selezionato_ = tubo;
    tubo_successivo();
    return e;
}

// Guida a filo (7B4C): l'arma selezionata dev'essere un proprio Mk 48 o Type 53 ancora collegato; 38h con l'arma,
// poi il comando
EsitoStazione StazioneArmi::guida(const Ordine& o, const SituazioneArmi& s) {
    EsitoStazione e;
    if (arma_guidata_ < 0 || !s.arma_presente) { e.messaggio = MSG_NESSUNA_ARMA; return e; }
    if (!s.arma_guidata) { e.messaggio = MSG_NON_GUIDATA; return e; }
    e.alla_missione.push_back({o.tempo, TipoOrdine::seleziona_siluro, 0.0, 0.0, arma_guidata_});
    e.alla_missione.push_back(o);
    if (o.tipo == TipoOrdine::guida_punto) e.messaggio = MSG_ARMA_DIRETTA;
    return e;
}

int StazioneArmi::messaggio_missione(int codice) {
    const int tubo = codice - 9;
    if (codice < 10 || codice > 16 || !valido(tubo)) return 0;
    if (stato(tubo) != StatoTubo::in_armamento) return 0;   // tubo liberato prima della fine del timer
    stato_[indice(tubo)] = StatoTubo::armato;
    tubo_successivo();
    if (arma(tubo) == Arma::mina) return MSG_MINA_ARMATA;
    if (arma(tubo) == Arma::attacco_terra) return MSG_ARMATO + static_cast<int>(Arma::tomahawk);
    if (arma(tubo) == Arma::esca_mobile) return MSG_ARMATO + static_cast<int>(Arma::esca);   // «esca pronta»
    return MSG_ARMATO + static_cast<int>(arma(tubo));   // 10 + tipo d'arma (ciclo principale, 1000:8A33)
}

// 7AF1: dal tubo selezionato, in giro, il primo armato con l'arma del tubo selezionato all'ingresso; dopo N passi
// senza trovarlo torna dov'era
void StazioneArmi::tubo_successivo() {
    if (!valido(selezionato_)) return;
    const Arma t0 = arma(selezionato_);
    for (int k = 0; k < numero_tubi_; ++k) {
        if (stato(selezionato_) == StatoTubo::armato && arma(selezionato_) == t0) return;
        selezionato_ = selezionato_ % numero_tubi_ + 1;
    }
}

// 2011:004A scala DS:03DE senza limite a 0; qui resta >= 0, perche' il caso negativo nasce solo da oggetti collegati al
// giocatore senza passare dalla stazione, che le 22 missioni non creano (docs/originale/numeri_eseguibile.md §1.2)
void StazioneArmi::unita_lanciate(const std::set<int>& in_gioco) {
    for (const int id : lanciate_)
        if (!in_gioco.count(id) && in_acqua_ > 0) --in_acqua_;
    lanciate_ = in_gioco;
}

// --- Akkord (Alfa) ---

bool StazioneArmi::bloccato(const Ordine& o) const {
    if (!akkord_guasto()) return false;
    switch (o.tipo) {
        case TipoOrdine::lancia:
        case TipoOrdine::imposta_siluro:
        case TipoOrdine::guida_punto:
        case TipoOrdine::guida_cerca:
        case TipoOrdine::guida_detona:
        case TipoOrdine::akkord_designa:
        case TipoOrdine::akkord_scegli: return true;
        case TipoOrdine::designa_bersaglio: return o.n >= 0;
        case TipoOrdine::carica_tubo:   // la ricarica (УБЗ, meccanica) si', l'armamento no
            return valido(o.n) && (stato(o.n) == StatoTubo::carico || !ricarica_attiva_);
        default: return false;
    }
}

EsitoStazione StazioneArmi::designa_akkord(const Ordine& o) {
    EsitoStazione e;
    if (!akkord_ || o.n < 0) return e;
    int k = -1;
    for (int i = 0; i < SLOT_AKKORD; ++i)
        if (slot_[static_cast<std::size_t>(i)] == o.n) k = i;   // c'e' gia': lo si sceglie
    for (int i = 0; i < SLOT_AKKORD && k < 0; ++i)
        if (slot_[static_cast<std::size_t>(i)] < 0) k = i;       // il primo libero
    if (k < 0) {                                                // altrimenti il meno recente
        k = 0;
        for (int i = 1; i < SLOT_AKKORD; ++i)
            if (designato_il_[static_cast<std::size_t>(i)] < designato_il_[static_cast<std::size_t>(k)]) k = i;
    }
    const auto j = static_cast<std::size_t>(k);
    if (slot_[j] != o.n) {
        slot_[j] = o.n;
        designato_il_[j] = o.tempo;
        e.messaggio = MSG_AKKORD_DESIGNATO;
    }
    scelto_ = k + 1;
    bersaglio_ = o.n;
    e.alla_missione.push_back({o.tempo, TipoOrdine::designa_bersaglio, 0.0, 0.0, o.n});
    return e;
}

int StazioneArmi::imposta_guasto_akkord(bool guasto) {
    if (!akkord_ || guasto == guasto_akkord_) return 0;
    guasto_akkord_ = guasto;
    if (guasto)   // i tubi armati o in armamento tornano carichi: i dati di tiro non arrivano piu'
        for (int t = 1; t <= numero_tubi_; ++t)
            if (stato(t) == StatoTubo::armato || stato(t) == StatoTubo::in_armamento) stato_[indice(t)] = StatoTubo::carico;
    return guasto ? MSG_AKKORD_GUASTO : MSG_AKKORD_RIPRISTINATO;
}

void StazioneArmi::aggiorna_akkord(const std::set<int>& presenti) {
    for (int i = 0; i < SLOT_AKKORD; ++i) {
        int& s = slot_[static_cast<std::size_t>(i)];
        if (s < 0 || presenti.count(s)) continue;
        s = -1;
        if (scelto_ == i + 1) scelto_ = 0;
    }
}

// --- scorte e carico M2 ---

std::map<std::string, int>& StazioneArmi::fonte(int tubo) {
    return pozzo(tubo) ? pozzi_ : ausiliario(tubo) ? esche_ : rastrelliera_;
}

const std::map<std::string, int>& StazioneArmi::fonte(int tubo) const {
    return pozzo(tubo) ? pozzi_ : ausiliario(tubo) ? esche_ : rastrelliera_;
}

int StazioneArmi::scorta_tubo(int tubo) const {
    if (!con_carico_) return scorte(arma(tubo));
    int n = 0;
    for (const auto& [id, k] : fonte(tubo))
        if (const auto f = famiglia_.find(id); f != famiglia_.end() && f->second == arma(tubo)) n += k;
    return n;
}

bool StazioneArmi::preleva(int tubo) {
    const Arma a = arma(tubo);
    if (a == Arma::nessuna) return false;
    int& scorta = scorte_[static_cast<std::size_t>(a)];
    if (!con_carico_) {
        if (scorta == 0) return false;
        --scorta;
        return true;
    }
    auto& f = fonte(tubo);
    std::string& v = variante_[indice(tubo)];
    if (const auto it = f.find(v); it == f.end() || it->second == 0) {   // la stessa variante, altrimenti un'altra
        v.clear();                                                         // della stessa famiglia
        for (const auto& [id, k] : f)
            if (k > 0 && famiglia_[id] == a) { v = id; break; }
        if (v.empty()) return false;
    }
    --f[v];
    if (scorta > 0) --scorta;
    return true;
}

void StazioneArmi::restituisci(int tubo) {
    const Arma a = arma(tubo);
    if (a == Arma::nessuna) return;
    ++scorte_[static_cast<std::size_t>(a)];
    if (con_carico_ && !variante_[indice(tubo)].empty()) ++fonte(tubo)[variante_[indice(tubo)]];
}

const std::string& StazioneArmi::variante(int tubo) const { return valido(tubo) ? variante_[indice(tubo)] : VUOTA; }
const std::string& StazioneArmi::lanciata(int tubo) const { return valido(tubo) ? lanciata_[indice(tubo)] : VUOTA; }

int StazioneArmi::indice_variante(const std::string& id) const {
    const auto it = std::find(varianti_.begin(), varianti_.end(), id);
    return it == varianti_.end() ? -1 : static_cast<int>(it - varianti_.begin());
}

void StazioneArmi::imposta_carico(const CaricoPartenza& c, const CatalogoCarico& cat, const ParametriRicarica& p) {
    con_carico_ = true;
    rastrelliera_ = c.rastrelliera;
    pozzi_ = c.pozzi;
    esche_ = c.esche;
    scorte_.fill(0);
    std::set<std::string> tutte;
    const auto famiglia = [&](const std::string& id) {
        const SchedaArma* s = cat.arma(id);
        const Arma a = s ? arma_da_nome(s->famiglia) : Arma::nessuna;
        if (a != Arma::nessuna) { famiglia_[id] = a; tutte.insert(id); }
        return a;
    };
    for (const auto* m : {&rastrelliera_, &pozzi_, &esche_})
        for (const auto& [id, k] : *m)
            if (const Arma a = famiglia(id); a != Arma::nessuna) scorte_[static_cast<std::size_t>(a)] += k;
    // il tubo vuoto prende la famiglia piu' numerosa della rastrelliera (la ricarica prende quella)
    Arma principale = Arma::nessuna;
    int piu = 0;
    std::map<Arma, int> per_famiglia;
    for (const auto& [id, k] : rastrelliera_)
        if (const Arma a = famiglia_.count(id) ? famiglia_[id] : Arma::nessuna; a != Arma::nessuna && (per_famiglia[a] += k) > piu) {
            piu = per_famiglia[a];
            principale = a;
        }
    for (int t = 1; t <= numero_tubi_; ++t) {
        const std::size_t i = indice(t);
        const std::string tipo = i < c.tipi.size() ? c.tipi[i] : std::string{};
        const std::string id = i < c.posizioni.size() ? c.posizioni[i] : std::string{};
        stato_[i] = StatoTubo::vuoto;
        variante_[i] = lanciata_[i] = {};
        pozzo_[i] = tipo == "pozzi";
        arma_[i] = Arma::nessuna;
        if (tipo == "tubo") {
            if (!id.empty() && famiglia(id) != Arma::nessuna) {
                arma_[i] = famiglia_[id];
                variante_[i] = id;
                stato_[i] = StatoTubo::carico;   // nel tubo alla partenza, da armare
            } else {
                arma_[i] = principale;
            }
        } else if (tipo == "esca") {
            arma_[i] = Arma::esca;
            variante_[i] = id;
        } else if (tipo == "pozzi" && !id.empty() && famiglia(id) != Arma::nessuna) {
            arma_[i] = famiglia_[id];
            variante_[i] = id;
            if (preleva(t)) stato_[i] = StatoTubo::carico;   // la prima cella
        }
    }
    varianti_.assign(tutte.begin(), tutte.end());
    attiva_ricarica(p);
}

// Cambio d'arma di un tubo da siluri (ordine scegli_arma_tubo)
EsitoStazione StazioneArmi::scegli_arma_tubo(int tubo, int k) {
    EsitoStazione e;
    if (!con_carico_ || !valido(tubo) || pozzo(tubo) || ausiliario(tubo) || k < 0 || k >= static_cast<int>(varianti_.size()))
        return e;
    const std::string& id = varianti_[static_cast<std::size_t>(k)];
    const Arma nuova = famiglia_[id];
    if (nuova == Arma::esca) return e;
    const std::size_t i = indice(tubo);
    if (id == variante_[i] && stato(tubo) != StatoTubo::vuoto) return e;
    switch (stato(tubo)) {
        case StatoTubo::in_armamento:
        case StatoTubo::armato:
            e.messaggio = MSG_PRIMA_DISARMARE;
            return e;
        case StatoTubo::vuoto:
            arma_[i] = nuova;
            variante_[i] = id;
            e.messaggio = MSG_ARMA_SCELTA;
            return e;
        case StatoTubo::in_ricarica:
        case StatoTubo::carico: {
            if (rastrelliera_[id] == 0) {
                e.messaggio = MSG_ARMA_NON_IN_RASTRELLIERA;
                return e;
            }
            const bool in_ricarica = stato(tubo) == StatoTubo::in_ricarica;
            restituisci(tubo);   // quella che c'era (o che si stava caricando) torna in rastrelliera
            arma_[i] = nuova;
            variante_[i] = id;
            preleva(tubo);
            if (!in_ricarica) {   // lo scambio e' una ricarica intera, con il suo posto fra le contemporanee
                stato_[i] = StatoTubo::in_ricarica;
                if (ricariche_in_corso() > parametri_.contemporanee) coda_.push_back(tubo);
            }
            lavoro_[i] = rumore_[i] = 0.0;
            e.messaggio = in_coda(tubo) ? MSG_RICARICA_IN_CODA : MSG_CAMBIO_ARMA;
            return e;
        }
    }
    return e;
}

}  // namespace m688
