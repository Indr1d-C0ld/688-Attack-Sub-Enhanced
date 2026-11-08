#pragma once
// La missione per la carriera (Fase 4a, M3): il riassunto della partita per il punteggio, ricavato dal registro e
// dall'esito dettagliato della M1 (missioni/registro_missione.hpp), e il contesto della missione (blocco `carriera`
// della missione, o la tabella delle originali di carriera.toml, con avversario e difficolta' ricalcolati dalle unita').
// Legge soltanto: la missione non cambia.

#include <cstdint>
#include <string>

#include "missioni/missione.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/carriera.hpp"

namespace m688 {

struct OpzioniRiassunto {
    const CaricoPartenza* carico = nullptr;     // il carico alla partenza (M2), per lo scarto dal carico standard
    const CatalogoCarico* catalogo = nullptr;
    std::string locale, tipo_missione;          // locale siluri e tipo della missione del carico
    int armi_a_bordo = -1;                      // armi rimaste a bordo (stazione Armi), per la camera di lancio persa
    int fatti_max = 4;                          // fatti per la motivazione, i piu' interessanti
};

EsitoPerCarriera riassumi(const Missione& m, const OpzioniRiassunto& o = {});

// origine: originale | rapida | editor; seme: il seme registrato delle rapide
ContestoMissione contesto_carriera(const Missione& m, const RegoleCarriera& r, const std::string& origine = "originale",
                                   std::uint64_t seme = 0);

// Dalle unita' ostili della missione (sottomarini, navi, aerei, elicotteri): avversario 0-3 e difficolta' 1-5
std::pair<int, int> minaccia_calcolata(const Missione& m);

}  // namespace m688
