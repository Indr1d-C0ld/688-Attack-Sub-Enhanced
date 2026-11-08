#include "effetti_bordo.hpp"

#include <cmath>
#include <vector>

#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/geometria.hpp"
#include "regia_audio.hpp"

namespace m688 {

namespace {

// i codici dei messaggi (privati dei loro file: missione.cpp, missione_ghiaccio.cpp, app/interfaccia.hpp)
constexpr int MSG_RADIO_FLASH = 316, MSG_RADIO_IMMEDIATE = 317, MSG_RADIO_RICEVUTO = 318, MSG_RADIO_ELF = 320;
constexpr int MSG_ESM_AEREO = 335, MSG_ESM_RISCHIO = 336, MSG_ESM_ILLUMINATI = 337;
constexpr int MSG_VOLTA_BASSA = 486, MSG_CHIGLIA = 487, MSG_ALBERI_GHIACCIO = 489;
constexpr int MSG_IMMERSIONE_EMERGENZA = 430, MSG_RICARICA_INIZIATA = 348;
constexpr double SUPERFICIE_M = 2.0, IMMERSO_M = 10.0;   // l'immersione dalla superficie: sotto i 10 m dopo i 2

EffettiDiBordo* attivo = nullptr;

}  // namespace

EffettiDiBordo* effetti_di_bordo() { return attivo; }
void imposta_effetti_di_bordo(EffettiDiBordo* e) { attivo = e; }

void EffettiDiBordo::allarme(const char* usn, const char* vmf, double massimo_s) { r_.effetto(urss_ ? vmf : usn, 1.0f, massimo_s); }

// l'ESM: due bip brevi a 1.500 Hz, come l'avviso dell'intercettatore (sintetizzati: non c'e' un campione)
void EffettiDiBordo::bip_esm() {
    constexpr int FS = RegiaAudio::EFFETTI_HZ;
    std::vector<float> v;
    for (int k = 0; k < 2; ++k) {
        const int n = static_cast<int>(0.08 * FS);
        for (int i = 0; i < n; ++i) {
            const double inv = std::min({1.0, i / (0.004 * FS), (n - i) / (0.004 * FS)});
            v.push_back(static_cast<float>(0.4 * inv * std::sin(2.0 * PI * 1500.0 * i / FS)));
        }
        v.insert(v.end(), static_cast<std::size_t>(0.06 * FS), 0.0f);
    }
    r_.suono(std::move(v), FS);
}

void EffettiDiBordo::aggiorna(const Missione& m) {
    const UnitaClassica* g = m.giocatore();
    if (!g) return;
    if (!iniziato_) {   // la chiamata generale col fischietto del nostromo (non sulla VMF)
        iniziato_ = true;
        if (!urss_) r_.effetto("fischietto_chiamata", 0.7f);
        alberi_ = g->flag54;
        in_superficie_ = g->quota < SUPERFICIE_M;
        eventi_visti_ = m.eventi_missione().size();   // una ripresa: niente allarmi per il passato
        messaggi_visti_ = m.messaggi().size();
        return;
    }
    // periscopio e antenna: l'idraulica quando cambiano
    if ((g->flag54 & 0x02) != (alberi_ & 0x02)) r_.effetto(g->flag54 & 0x02 ? "periscopio_su" : "periscopio_giu", 0.8f);
    if ((g->flag54 & 0x04) && !(alberi_ & 0x04)) r_.effetto("antenna_su", 0.8f);
    alberi_ = g->flag54;
    // l'immersione dalla superficie: lo sfogo delle casse e l'allarme d'immersione
    if (g->quota < SUPERFICIE_M) {
        in_superficie_ = true;
    } else if (in_superficie_ && g->quota > IMMERSO_M) {
        in_superficie_ = false;
        r_.effetto("sfogo_casse", 0.8f);
        allarme("allarme_immersione", "revun", 2.0);
    }
    // gli eventi del registro
    const auto& ev = m.eventi_missione();
    for (; eventi_visti_ < ev.size(); ++eventi_visti_) {
        const EventoMissione& e = ev[eventi_visti_];
        switch (e.tipo) {
            case TipoEvento::lancio_giocatore: r_.effetto("tubo_lancio"); break;
            case TipoEvento::posti_di_combattimento:   // livello 1: ai posti (0: di nuovo normale, in silenzio)
                if (e.livello == 1) gq_ = true, allarme("posti_combattimento_gong", "campane_combattimento");
                break;
            case TipoEvento::danno_subito: {
                using T = EventoBattello::Tipo;
                const auto t = static_cast<T>(e.codice);
                if (t == T::allagamento || t == T::paratia_ceduta || t == T::cassa_squarciata) allarme("allarme_collisione", "revun");
                else if (t == T::incendio || t == T::esplosione || t == T::aria_nell_incendio) allarme("allarme_chimico", "campane_combattimento");
                else if (t == T::cloro) allarme("allarme_chimico", "revun");
                break;
            }
            case TipoEvento::scram: allarme("allarme_propulsione", "revun"); break;
            case TipoEvento::emersione_emergenza:   // la soffiata del giocatore: l'aria alle casse (quelle dell'IA non si sentono a bordo)
                if (e.unita == g->id) r_.effetto("soffiata_emergenza");
                break;
            case TipoEvento::collisione:
                if (e.unita == g->id || e.altra == g->id) allarme("allarme_collisione", "revun");
                break;
            case TipoEvento::mina_scoperta:
                if (e.giocatore == 0) r_.effetto("tono_sonar_mine", 0.7f);
                break;
            default: break;
        }
    }
    // i messaggi della missione al giocatore: la radio, l'ESM, il ghiaccio
    const auto& ms = m.messaggi();
    for (; messaggi_visti_ < ms.size(); ++messaggi_visti_) {
        const Messaggio& x = ms[messaggi_visti_];
        if (x.unita != g->id) continue;
        switch (x.codice) {
            case MSG_RADIO_FLASH:
                if (!urss_) r_.effetto("campanello_flash");
                r_.effetto("telescrivente", 0.6f, 3.0);
                break;
            case MSG_RADIO_IMMEDIATE:
            case MSG_RADIO_RICEVUTO: r_.effetto("telescrivente", 0.6f, 2.0); break;
            case MSG_RADIO_ELF: r_.effetto("vlf_naturale", 0.5f, 2.0); break;
            case MSG_ESM_AEREO:
            case MSG_ESM_RISCHIO:
            case MSG_ESM_ILLUMINATI: bip_esm(); break;
            case MSG_VOLTA_BASSA:
            case MSG_CHIGLIA:
            case MSG_ALBERI_GHIACCIO: r_.effetto("tono_sonar_mine", 0.7f); break;
            default: break;
        }
    }
}

void EffettiDiBordo::messaggio_equipaggio(int codice) {
    if (codice == MSG_IMMERSIONE_EMERGENZA) allarme("tromba_immersione_h8", "revun", urss_ ? 4.0 : 0.0);   // l'«aooga» intero
    else if (codice == MSG_RICARICA_INIZIATA) r_.effetto("ricarica_tubo", 0.8f);
    else r_.effetto("bip_messaggio", 0.5f);
}

}  // namespace m688
