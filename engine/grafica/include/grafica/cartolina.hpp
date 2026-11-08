#pragma once
// Minicartoline del debriefing (M4, docs/12): la scena saliente di un evento ridisegnata dal mondo all'istante
// dell'evento (navi, modelli danneggiati, effetti) con il periscopio, da un punto di vista scelto per inquadrarla,
// in un riquadro piccolo (128 x 96). Il punto di vista si ricava dal suggerimento del registro (EventoMissione::
// inquadratura, missioni/registro_missione.hpp): soggetto, secondo, distanza, elevazione, azimut dalla prua. Per gli
// eventi che non si vedono, un'icona d'epoca con i nomi del registro: cascata, sonar, radio (telescrivente), esm,
// radiogoniometro, danni, zona, mina. Tutto nella palette della vista (il debriefing la tiene).

#include <cstdint>
#include <string_view>

#include "grafica/effetti.hpp"
#include "grafica/font.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tela.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

struct PuntoDiVista {
    Vec2 occhio;                    // da dove si guarda (m)
    double rilevamento = 0.0;       // verso dove (gradi veri)
    Vec2 bersaglio;                 // il punto da tenere al centro del riquadro
    bool alto_ingrandimento = true; // 6x (8 gradi); 1,5x per le viste dall'alto
    double quota_occhio_m = 2.0;
    double luce = 1.0;              // 0 notte, 1 giorno
    double visibilita_m = 30000.0;
    double altezza_onde_m = 1.0;
    double tempo_s = 0.0;           // onde e animazioni
    double alzo = 0.0;              // frazione dell'altezza: >0 alza l'inquadratura (colonne, missili)
};

// Un punto visto da `distanza_m`, guardando dal rilevamento `da_gradi` (gradi veri dal punto verso l'occhio), con
// l'occhio alto `elevazione_gradi` sopra l'orizzonte del punto (0: a pelo d'acqua, come il periscopio)
PuntoDiVista inquadra(Vec2 punto, double distanza_m, double da_gradi, double tempo_s, double elevazione_gradi = 0.0);
// Una nave del mondo vista dal lato `lato_gradi` (dalla prua: 90 = traverso di dritta), a una distanza che la fa
// stare nel riquadro con un po' d'aria (`lunghezza_m` dalla classe)
PuntoDiVista inquadra_nave(const Nave& n, double lunghezza_m, double tempo_s, double lato_gradi = 90.0);
// Dal suggerimento del registro: il soggetto (e il secondo da tenere in campo, o nullptr), la distanza della camera,
// l'elevazione sopra l'orizzonte (negativa: si guarda a pelo d'acqua) e l'azimut dalla prua del soggetto
PuntoDiVista dal_registro(const Nave& soggetto, const Nave* secondo, double distanza_m, double elevazione_gradi,
                          double azimut_gradi, double tempo_s);

// La scena nel riquadro r (pensato per 128 x 96): il periscopio ad alto ingrandimento disegna la vista su una tela di
// appoggio, se ne prende il centro attorno al bersaglio e si riduce di 2 col colore piu' frequente in ogni 2x2
void disegna_cartolina(Tela& t, Rettangolo r, const Periscopio& p, const Mondo& mondo, const PuntoDiVista& q,
                       const Effetti* effetti = nullptr);

enum class Icona {
    cascata,          // scoperta passiva: cascata con la traccia del contatto
    sonar,            // ping, contatto attivo: schermo circolare con il lampo
    telescrivente,    // «radio»: messaggio ricevuto o perso, foglio con le righe battute
    esm,              // radar ESM a rischio alto: oscilloscopio polare con la linea di rilevamento
    radiogoniometro,  // fix del nemico dopo una trasmissione: rosa con i rilevamenti che si incrociano
    danni,            // danni subiti: sagoma del battello con i compartimenti colpiti
    zona,             // zona protetta: carta quadrettata con la zona tratteggiata e il battello
    mina,             // mina posata: sezione del fondale con la mina ormeggiata sul cavo e l'ancora
    aereo,            // aereo ASW in sorvolo: il quadrimotore visto da sotto col pungiglione del MAD
    boa,              // sonoboa: galleggiante con l'antenna che trasmette, cavo e idrofono sott'acqua
    elicottero,       // elicottero in hover col sonar a immersione: anello di spruzzi, cavo, ping dal trasduttore
    marcatore,        // marcatore in mare (Mk 58): due candele e il fumo, di notte
    periscopio,       // periscopio esposto: la testa fuori dall'acqua col suo baffo, il mare mosso
};
// Dai nomi del registro ("radio", "esm", "radiogoniometro", "danni", "zona", "sonar", "cascata", "mina", "aereo",
// "boa", "elicottero", "marcatore", e gli eventi dell'ASW aereo "aereo_in_sorvolo", "boa_vicina",
// "ping_sonar_immersione", "marcatore_avvistato", "attacco_simulato"); sconosciuti: sonar
Icona icona_da_nome(std::string_view nome);
// Icona animata (tempo_s) con una didascalia facoltativa nel foglio della telescrivente
void disegna_icona(Tela& t, const Font& f, Rettangolo r, Icona i, double tempo_s, std::uint32_t seme,
                   std::string_view testo = {});

}  // namespace m688
