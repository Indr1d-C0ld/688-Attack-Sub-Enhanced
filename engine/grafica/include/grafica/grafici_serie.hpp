#pragma once
// I grafici della schermata delle serie della sandbox (docs/19 §6; contratto di trailer-1 sandbox-v3-serie-contratto.md):
// la pagina la compone trailer-1 (app/schermata_serie), il kit disegna i pezzi.
// - istogramma: i valori di una statistica su tutte le partite della serie, a classi, con la mediana e la fascia fra
//   il 10° e il 90° percentile;
// - barre_esiti: le frequenze degli esiti, una riga per codice d'esito;
// - riga_dispersione: una statistica in una riga di tabella, coi numeri (media ± dev, min-max) e un grafico piccolo
//   (min-max, p10-p90, mediana, media);
// - barre_partite: una barra per partita in ordine di seme, cliccabile (restituisce i rettangoli).
//
// I valori sono gia' nell'unita' che si scrive (s, m, km o niente): chi chiama converte. Un valore mancante e' NaN:
// istogramma e riassumi lo saltano, barre_partite lascia il posto vuoto. I numeri si scrivono all'italiana (virgola
// decimale). I colori sono indici della palette che la schermata ha caricato: per le parti, simboli::indice_parte
// (con applica_palette_carta). Il resto usa i colori fissi dell'interfaccia (0-15).

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/tela.hpp"

namespace m688 {

class Font;

namespace grafici {

// Media, deviazione standard (del campione), minimo, massimo, mediana e percentili 10 e 90 (interpolazione lineare
// fra i valori ordinati); n = 0 se non ci sono valori validi
struct Riassunto {
    int n = 0;
    double media = 0, dev = 0, min = 0, max = 0, mediana = 0, p10 = 0, p90 = 0;
};
Riassunto riassumi(std::span<const double> valori);

// Un numero da scrivere: interi senza decimali, sotto 10 un decimale, la virgola decimale; con l'unita' dopo uno spazio
std::string numero(double v, std::string_view unita = {});

struct OpzioniIstogramma {
    std::string titolo;               // in alto a sinistra
    std::string unita;                // s, m, km o vuota
    int classi = 10;                  // con valori interi su un intervallo piu' stretto, una classe per intero
    std::uint8_t colore = 15;         // le barre (la parte: simboli::indice_parte)
    std::uint8_t colore_testo = 7;    // assi, numeri, titolo
    std::uint8_t colore_mediana = 15; // la linea della mediana
};
// L'istogramma in r: titolo, barre a classi con l'altezza delle partite, l'asse con min, mediana e max, la fascia
// p10-p90 sotto l'asse e il numero di partite. Senza valori scrive «NESSUN DATO».
void istogramma(Tela& t, const Font& f, Rettangolo r, std::span<const double> valori, const OpzioniIstogramma& o);

struct VoceEsito {
    int codice = 0;
    std::string testo;
    int volte = 0;
};
// Le frequenze degli esiti in r, una riga per voce (al piu' quante ci stanno, righe di 14 pixel): il testo a sinistra,
// la barra proporzionale alle partite, «volte SU totale» in fondo (il font del 1989 non disegna il «%»).
// La voce piu' frequente ha la barra piena, le altre il colore attenuato se dato (0: lo stesso). Il codice si scrive
// davanti al testo solo se non e' negativo (-1, «non finita», e' un codice interno).
void barre_esiti(Tela& t, const Font& f, Rettangolo r, std::span<const VoceEsito> voci, std::uint8_t colore,
                 std::uint8_t colore_testo, std::uint8_t colore_attenuato = 0);

// Una statistica in una riga alta 10 pixel: «media ± dev» a sinistra, il grafico piccolo al centro (la linea min-max,
// la barra p10-p90 nel colore, la tacca della mediana, il punto della media) e «min-max» a destra. La scala del
// grafico va da scala_min a scala_max (le stesse per NATO e Patto, cosi' le due righe si confrontano); se coincidono,
// da min a max del riassunto.
void riga_dispersione(Tela& t, const Font& f, Rettangolo r, const Riassunto& s, std::string_view unita,
                      std::uint8_t colore, std::uint8_t colore_testo, double scala_min = 0.0, double scala_max = 0.0);

// Una barra per partita, in ordine, alte quanto il valore (lo zero e' una tacca, il mancante un puntino grigio);
// la scelta e' bordata di bianco. Restituisce il rettangolo di ogni barra, alto quanto r, per il clic.
std::vector<Rettangolo> barre_partite(Tela& t, Rettangolo r, std::span<const double> valori, std::uint8_t colore,
                                      int scelta = -1);

}  // namespace grafici
}  // namespace m688
