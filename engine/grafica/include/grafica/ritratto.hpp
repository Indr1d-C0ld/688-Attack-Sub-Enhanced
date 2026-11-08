#pragma once
// Ritratto del comandante in uniforme (M4 della Fase 4a, dossier docs/ricerca/13 §4), nello stile dei ritratti VGA a
// 256 colori dei simulatori MicroProse: testa e petto, come la foto ufficiale. La testa e' un volume in 3D (cranio,
// mascella, zigomi, orbite, occhi con le palpebre, naso, labbra, orecchie, collo, capelli e berretto) calcolato a
// distanze con segno e illuminato come in studio (luce principale da sinistra in alto con ombre morbide, schiarita da
// destra, controluce, occlusione ambientale), poi ridotto alle rampe di cerimonia.hpp con la retinatura di Bayer.
// L'uniforme e' dipinta a forme ombreggiate. Il volto viene da un seme (lineamenti, pettinatura, capelli, occhi,
// carnagione, baffi) e invecchia con gli anni; grado, nastrini e distintivi sono parametri della carriera.
//   US Navy: Service Dress Blue, dolphins d'oro sopra la barretta, Command at Sea sul petto destro finche' comanda;
//            berretto con le foglie di quercia d'oro dal capitano di fregata. I galloni delle maniche, fuori
//            dall'inquadratura, si mostrano a parte con disegna_insegna_grado (come la promozione in F-19).
//   VMF: tuzurka nera con le controspalline (strisce e stelle del grado), barretta a sinistra, distintivo
//        «Comandante di sottomarino» e rombo del diploma a destra; berretto con il «granchio».

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"

namespace m688 {

struct Volto {
    std::uint32_t seme = 1;    // lineamenti: mascella, naso, sopracciglia, orecchie, pettinatura
    int eta = 40;              // capelli brizzolati dopo i 40, rughe dopo i 45
    std::array<std::uint8_t, 3> capelli{70, 45, 30};
    bool baffi = false;
    double carnagione = 0.2;                       // 0 chiara, 1 olivastra
    std::array<std::uint8_t, 3> occhi{90, 120, 150};
};

enum class Uniforme { servizio, kaki };   // kaki: solo US Navy (briefing e debriefing), con i gradi al colletto

struct ParametriRitratto {
    Marina marina = Marina::usn;
    // US Navy: 1-10 per O-1 ... O-10; VMF: 1-12 nell'ordine del dossier 13 §1.5 (6 = capitano di 2° rango)
    int grado = 5;
    Uniforme uniforme = Uniforme::servizio;
    bool berretto = false;
    bool qualificato = true;   // dolphins (US Navy)
    bool comando = false;      // Command at Sea / «Comandante di sottomarino»
    bool diploma = true;       // rombo della scuola superiore (VMF)
    std::vector<VoceBarretta> nastrini;   // in ordine di precedenza (Carriera::barretta); i distintivi si saltano
    Volto volto;
    double girata_gradi = -999.0;     // rotazione della testa; -999 = scelta dal seme (4-12 gradi)
    bool solo_testa = false;          // prova dei lineamenti: niente uniforme
};

// Rampe della cerimonia per la marina, capelli, carnagione e occhi del volto, colori dei nastrini della tavola
void prepara_palette_ritratto(Palette& p, const ParametriRitratto& r, const TavolaDecorazioni* tavola = nullptr);
// Ritratto nel rettangolo (proporzioni di riferimento 176 x 232, scalato), sul fondale dello studio. Senza tavola niente
// barretta. Un ritratto grande costa qualche decimo di secondo: si disegna una volta per schermata.
void disegna_ritratto(Tela& t, Rettangolo r, const ParametriRitratto& p, const TavolaDecorazioni* tavola, bool fondale = true);
// Ritratto di gruppo dell'equipaggio schierato (decorazioni di reparto: NUC, PUC, MUC; encomio e Bandiera Rossa al
// battello), nello stile della foto ufficiale: la vela del battello all'ormeggio dietro, tre file sul ponte e sulla
// banchina, gli ufficiali al centro della prima fila col comandante in mezzo. Marinai USN con il berretto bianco e il
// colletto alla marinara, VMF con la bustina nera e la maglia a righe; teste in 3D come il ritratto, ognuna col suo
// volto. Il ritratto cambia con la missione: i feriti con la benda o il braccio al collo, i caduti mancano dalla fila e
// un nastro nero di lutto attraversa l'angolo della foto. Funziona in qualunque riquadro (236 x 310 come il ritratto
// del comandante, o piu' largo).
struct ParametriEquipaggio {
    Marina marina = Marina::usn;
    std::uint32_t seme = 1;
    int uomini = 18;            // in posa a missione senza perdite (ufficiali compresi)
    int caduti = 0;
    int feriti = 0;
    std::string battello;       // numero sulla vela (vuoto: niente scritta)
    int grado_comandante = 5;   // per le foglie di quercia sul berretto
};
void disegna_equipaggio(Tela& t, const Font& f, Rettangolo r, const ParametriEquipaggio& p);

// Insegna del grado in grande: polsino con i galloni e la stella (US Navy), controspallina (VMF)
void disegna_insegna_grado(Tela& t, Rettangolo r, Marina m, int grado);

}  // namespace m688
