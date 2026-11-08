#pragma once
// La pagina SQUADRE del quadro danni dell'Enhanced (F3; punto 4 del piano, kit 06/11/2026): sta al posto del dettaglio
// ingrandito, y 164-312, uguale nel 688 e nell'Alfa. A sinistra i 13 sistemi nell'ordine di riparazione, con l'efficienza e
// la squadra al lavoro; a destra i compartimenti con le porte. I pulsanti stanno su una riga a parte, anche sulla pagina
// normale di F3. I dati li prepara la stazione: qui non si guarda il battello, cosi' il disegno non tocca la partita.

#include <string>
#include <utility>
#include <vector>

#include "grafica/tela.hpp"

namespace m688 {

class Font;
class Spaccato;

struct RigaSquadre {
    std::string nome;          // del sistema, come nome_sistema()
    double efficienza = 1.0;   // 0..1
    double massimo = 1.0;      // fin dove le riparazioni lo possono riportare
    int squadre = 0;           // al lavoro su questo sistema
};

struct PortaOtsek {
    enum class Stato { aperto, in_chiusura, chiuso, comando };   // comando: il posto di comando, non si isola
    std::string nome;          // «C3 MACCHINE», «V TURB.»
    Stato stato = Stato::aperto;
    int conto_s = 0;           // in_chiusura: i secondi alle porte chiuse
    int uomini_dentro = 0;
    double acqua = 0.0, fuoco = 0.0;   // 0..1
    double ossigeno = 0.21;            // frazione: sotto il 12% il fuoco si spegne
    int squadre = 0;                   // al lavoro nel compartimento
};

struct DatiPaginaSquadre {
    bool alfa = false;                // le scritte del pult
    std::vector<RigaSquadre> righe;   // nell'ordine di riparazione
    int cursore = 0;                  // la riga scelta coi pulsanti 2 e 3
    std::vector<PortaOtsek> porte;    // nell'ordine dei compartimenti
    int scelto = 0;                   // il compartimento mostrato (Tab), su cui agisce il pulsante 1
    int squadre = 0, libere = 0;
};

void disegna_pagina_squadre(Tela& t, const Font& f, Rettangolo r, const DatiPaginaSquadre& d);

// I pulsanti, allineati a destra di `destra`, alti 12 px da y: sempre 1 e 5, con la pagina aperta anche 2, 3 e 4. `aree` da' il
// rettangolo e la cifra di ognuno, per il clic; puo' essere nullptr. Restituisce la x del primo pulsante
using AreaPulsante = std::pair<Rettangolo, int>;
int disegna_tasti_squadre(Tela& t, const Font& f, int destra, int y, const DatiPaginaSquadre& d, bool pagina_aperta, std::vector<AreaPulsante>* aree = nullptr);

// Sullo spaccato disegnato in r: i compartimenti chiusi o in chiusura, col conto alla rovescia e gli uomini dentro
void disegna_porte_spaccato(Tela& t, const Font& f, const Spaccato& s, Rettangolo r, const DatiPaginaSquadre& d);

}  // namespace m688
