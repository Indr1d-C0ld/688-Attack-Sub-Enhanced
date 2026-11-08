#pragma once
// Stato interno dei periscopi dell'Enhanced nella missione (docs/14_periscopi.md, P2): lo usano soltanto missione.cpp
// e missione_periscopi.cpp. L'interfaccia pubblica e' in missione.hpp (periscopi_enhanced, posto_periscopi, teste_ia...).

#include <map>
#include <set>

#include "missioni/missione.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/periscopi.hpp"

namespace m688 {

// avvisi al giocatore (fascia 400-409, ceduta dal principale; testi in tools/pipeline/esporta_motore.py)
inline constexpr int MSG_PERI_SCELTO = 400, MSG_PERI_SU = 401, MSG_PERI_INSERVIBILE = 402, MSG_PERI_DANNEGGIATO = 403,
                     MSG_PERI_NON_SALE = 404, MSG_PERI_ESPOSTI = 405, MSG_PERI_NEMICO = 406, MSG_PERI_FOTO = 407,
                     MSG_PERI_AFFIORA = 408,   // la vela affiora col mare grosso (meteo, M2)
                     MSG_PERI_FULMINE = 409;   // il sonar sente un fulmine sul mare: transitorio senza contatto

// la quota periscopica della missione (20 ft, quella dell'originale) e' la quota periscopica della classe
inline constexpr double QUOTA_PERISCOPICA_MISSIONE_M = 6.096;

struct Missione::StatoPeriscopiMissione {
    CatalogoPeriscopi dati;
    PostoPeriscopi posto;
    Casuale caso{0x9E21};                    // il flusso dei periscopi (solo con la regola di simulazione)
    std::map<int, PeriscopioIa> ia;          // sottomarini dell'IA con i periscopi della classe
    std::set<int> considerati;
    double prossimo = 0.0;                   // s di missione del prossimo secondo di vista
    double luce = 1.0, ultima_luce = -1e18;
    bool flag_nostro = false;                // il flag del periscopio l'ha alzato il posto
    bool inservibile_avvisato = false, esposti_avvisato = false;
    std::set<int> nemici_visti;              // teste dell'IA gia' annunciate (fino a quando tornano sotto)
    bool avaria = false;                     // il sistema «periscopio» dei danni sotto 0,5
    double teste_tempo[2] = {-1.0, -1.0};    // artiglieria (G3): l'ultimo colpo gia' applicato alle teste d'attacco e di ricerca
    bool comando_bloccato = false;           // l'ultimo comando dato a un periscopio bloccato (per l'avviso 404)
};

}  // namespace m688
