#pragma once
// Strumenti delle consolle per le stazioni della Fase 3 (dossier docs/ricerca/19_consolle_688_alfa.md §2.6, §3.5, §5.4):
// i pezzi con cui grafica/sale compone viste e pannelli del 688 Flight I e dell'Alfa (Pr. 705). Funzioni pure sulla
// Tela, a indici della palette del tema (grafica/tema_stazioni.hpp, indici 16-47) piu' il nero e il bianco EGA.
//
// | Elemento   | 688 (MIL-16400, MIL-1472)                                  | Alfa                                          |
// |------------|------------------------------------------------------------|-----------------------------------------------|
// | quadranti  | fondo scuro, numeri chiari (rossi con la luce rossa)       | fondo bianco, indice nero, cornice di bachelite |
// | lampade    | rettangolari a lente, in file nei sinottici                | piccole tonde in file; finestrelle rosse      |
// | schermi    | CRT rettangolare, cornice grigia e cappuccio               | CRT tondo sotto la cappa conica nera          |
// | comandi    | encoder, pulsante rosso coperto, colonna a cloche          | pulsanti quadrati illuminati, levette nere    |
// | scritte    | incisione bianca su targhetta nera                         | stampino bianco su targhetta rossa            |
//
// Le scritte usano i font delle stazioni (data/motore/font: incisione688_*, stampino_urss_*, fosforo688_*, cifre_*):
// chi chiama passa il Font. Niente stato: ogni funzione disegna nel rettangolo che riceve e non esce dal ritaglio.

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

class Font;

// --- cornici e targhette ---
// Il pannello: smalto con lo smusso (luce dall'alto a sinistra) e, sul 688, le quattro viti negli angoli
void pannello(Tela& t, Rettangolo r, BattelloTema b, int tono = 7);
// Targhetta col nome: incisione bianca su nero (688) o stampino bianco su rosso (Alfa); il testo centrato.
// Restituisce il rettangolo occupato (largo quanto il testo piu' i margini, se r.w e' 0)
Rettangolo targhetta(Tela& t, Rettangolo r, std::string_view testo, const Font& f);

// --- quadranti ---
enum class StileQuadrante : std::uint8_t { scuro_688, bianco_alfa };
struct ScalaQuadrante {
    double minimo = 0.0, massimo = 360.0;
    double da_gradi = 0.0, a_gradi = 360.0;   // arco della scala sul quadrante (0 = in alto, orario)
    int tacche = 36;                          // tacche corte lungo l'arco
    int numeri = 12;                          // valori scritti (0: nessuno); con l'arco intero l'ultimo coincide col primo
    double fattore_numeri = 0.1;              // il valore scritto e' valore x fattore (bussola: 0, 3, 6... per 0-360)
};
// Il quadrante di raggio r centrato in (cx, cy) con l'indice sul valore (e un secondo indice facoltativo, per il
// comandato: rotta ordinata, quota ordinata); i numeri col font, se c'e'
void quadrante(Tela& t, int cx, int cy, int r, double valore, const ScalaQuadrante& s, StileQuadrante stile,
               const Font* f = nullptr, const double* comandato = nullptr);
// Barra verticale graduata (quota, livello delle casse): riempita fino a frazione 0-1, con la tacca del comandato
void barra(Tela& t, Rettangolo r, double frazione, StileQuadrante stile, double comandato = -1.0, int tacche = 10);

// --- lampade e allarmi ---
// Lampada rettangolare a lente (688): accesa col riflesso, spenta la lente colorata; con la scritta incisa sotto
void lampada_rettangolare(Tela& t, Rettangolo r, ColoreLampada c, bool accesa);
// Piccola lampada tonda (Alfa) di raggio r
void lampada_tonda(Tela& t, int cx, int cy, int r, ColoreLampada c, bool accesa);
// Finestrella d'allarme sovietica: vetro rosso con la scritta, accesa (rosso vivo, scritta chiara) o spenta (scura)
void finestrella(Tela& t, Rettangolo r, std::string_view testo, bool accesa, const Font& f);

// --- comandi ---
// Pulsante illuminato (quadrato sull'Alfa, rettangolare sul 688): la scritta sul vetro colorato, acceso o spento,
// premuto (smusso rovesciato)
void pulsante(Tela& t, Rettangolo r, std::string_view testo, ColoreLampada c, bool acceso, bool premuto, const Font& f);
// Pulsante coperto (FIRE del 688, soffiaggio d'emergenza): il coperchio rosso a strisce chiuso o alzato, il pulsante
void pulsante_coperto(Tela& t, Rettangolo r, bool coperchio_alzato, bool premuto);
// Manopola a encoder (688) di raggio r con la linea d'indice ad angolo (0 = in alto, orario)
void manopola(Tela& t, int cx, int cy, int r, double angolo_gradi);
// Levetta nera (Alfa) su o giu'
void levetta(Tela& t, int cx, int cy, bool su);

// --- numeri ---
// Contatori a tamburo (bianco su nero): le cifre del testo; `rotolamento` 0-1 fa salire l'ultima cifra verso la
// successiva (mezza cifra tagliata, come sui contatori meccanici). Font: incisione688_7 o stampino
void tamburo(Tela& t, int x, int y, std::string_view cifre, double rotolamento, const Font& f);
// Display a 7 segmenti (LED del 688): segmenti accesi del colore, spenti in rosso scurissimo. Font: cifre_*
void sette_segmenti(Tela& t, int x, int y, std::string_view testo, ColoreLampada c, const Font& f, int scala = 1);

// --- schermi ---
// CRT rettangolare (688): cornice grigia smussata, cappuccio in alto, vetro spento; restituisce l'area del fosforo
Rettangolo crt_rettangolare(Tela& t, Rettangolo r);
// CRT tondo (Alfa) sotto la cappa conica nera: lo schermo di raggio r in (cx, cy), la cappa intorno; restituisce il
// quadrato che contiene lo schermo (il disco e' dentro)
Rettangolo crt_tondo(Tela& t, int cx, int cy, int r);
// Testo sul fosforo: alone, tratto e nucleo dalla rampa 40-47 (font fosforo688_* o stampino_urss_*)
void testo_fosforo(Tela& t, int x, int y, std::string_view testo, const Font& f, int scala = 1);
// Righe di testo sul vetro: ognuna va a capo a parole intere dentro il vetro (le parole piu' lunghe si spezzano), le
// righe che non ci stanno si tagliano, e il disegno resta nel rettangolo. Restituisce le righe scritte
int testo_fosforo_nel_vetro(Tela& t, Rettangolo vetro, const std::vector<std::string>& righe, const Font& f);
// Le righe a capo per una larghezza in pixel (la stessa regola), per chi deve sapere quante sono
std::vector<std::string> a_capo(const std::string& testo, const Font& f, int larghezza);

// --- governo e casse (688) ---
// Colonna a cloche vista di fronte: il volante girato (barra -1..1, a dritta positivo) e la colonna spinta o tirata
// (piani -1..1, giu' positivo: il volante scende sullo schermo)
void cloche(Tela& t, Rettangolo r, double barra, double piani);
// «Albero di Natale» delle aperture dello scafo: per apertura un segno, verde a barretta (chiusa) o rosso tondo
// (aperta), in `colonne` colonne
void albero_di_natale(Tela& t, Rettangolo r, const std::vector<bool>& aperte, int colonne);

}  // namespace m688
