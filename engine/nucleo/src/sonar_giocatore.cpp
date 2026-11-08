#include "nucleo/sonar_giocatore.hpp"

#include <cmath>

namespace m688 {

int StazioneSonar::posizione_velocita(double velocita_ordinata, double avanti_tutta) {
    return avanti_tutta > 0.0 ? static_cast<int>(std::floor(velocita_ordinata * 4.0 / avanti_tutta + 1e-9)) : 0;
}

bool StazioneSonar::riguarda(const Ordine& o) const {
    switch (o.tipo) {
        case TipoOrdine::schiera_cala:
        case TipoOrdine::schiera_recupera:
        case TipoOrdine::schiera_taglia:
        case TipoOrdine::filtro_sonar:
        case TipoOrdine::impulso_sonar: return true;
        default: return false;
    }
}

EsitoStazione StazioneSonar::consegna(const Ordine& o, const SituazioneSonar& s) {
    EsitoStazione e;
    // durante una manovra o con la schiera persa, filare e recuperare danno solo il messaggio (6F60-6FE0)
    auto occupata = [&]() {
        if (schiera_ == StatoSchiera::persa) e.messaggio = MSG_SCHIERA_GIA_TAGLIATA;
        else if (manovra_ == ManovraSchiera::filando) e.messaggio = MSG_ANCORA_IN_FILATURA;
        else if (manovra_ == ManovraSchiera::recuperando) e.messaggio = MSG_ANCORA_IN_RECUPERO;
        return e.messaggio != 0;
    };
    switch (o.tipo) {
        case TipoOrdine::schiera_cala:
            if (occupata() || schiera_ == StatoSchiera::filata) return e;
            if (posizione_velocita(s.velocita_ordinata, s.avanti_tutta) > 2) { e.messaggio = MSG_NON_SI_FILA; return e; }
            manovra_ = ManovraSchiera::filando;   // 34h: il gestore della missione la fila in 60 s
            e.alla_missione.push_back(o);
            return e;
        case TipoOrdine::schiera_recupera:
            if (occupata() || schiera_ == StatoSchiera::dentro) return e;
            manovra_ = ManovraSchiera::recuperando;   // 35h: 40 s
            e.alla_missione.push_back(o);
            return e;
        case TipoOrdine::schiera_taglia:   // 36h, dopo la conferma del comandante o per un guasto
            if (!schiera_esposta()) return e;
            schiera_ = StatoSchiera::persa;
            manovra_ = ManovraSchiera::nessuna;
            e.alla_missione.push_back(o);
            e.messaggio = MSG_SCHIERA_TAGLIATA;
            return e;
        case TipoOrdine::filtro_sonar:
            filtro_ = o.n != 0;
            e.messaggio = filtro_ ? MSG_PASSA_ALTO : MSG_PASSA_BANDA;
            return e;
        case TipoOrdine::impulso_sonar:   // 2Dh: evento acustico di 300 passi
            e.alla_missione.push_back(o);
            e.messaggio = MSG_PING;
            return e;
        default: return {{o}, 0};
    }
}

void StazioneSonar::messaggio_missione(int codice) {
    if (codice == MSG_SCHIERA_FILATA && manovra_ == ManovraSchiera::filando) {
        schiera_ = StatoSchiera::filata;
        manovra_ = ManovraSchiera::nessuna;
    } else if (codice == MSG_SCHIERA_RECUPERATA && manovra_ == ManovraSchiera::recuperando) {
        schiera_ = StatoSchiera::dentro;
        manovra_ = ManovraSchiera::nessuna;
    }
}

bool StazioneSonar::chiede_conferma(const Ordine& t) const {
    if (t.tipo != TipoOrdine::telegrafo || !schiera_esposta()) return false;
    return t.n == 1 || t.a >= 0.75 - 1e-9;   // arresto rapido, 3/4, TUTTA, MAX
}

int StazioneSonar::analizza(const SituazioneSonar& s) const {
    if (!s.bersaglio) return MSG_SONAR_SERVE_BERSAGLIO;
    if (!s.contatto) return MSG_CONTATTO_PERSO;   // basta un contatto qualsiasi, anche una X (3EE0)
    if (s.aereo) return MSG_NO_ANALISI_AEREI;
    return MSG_ANALISI;
}

}  // namespace m688
