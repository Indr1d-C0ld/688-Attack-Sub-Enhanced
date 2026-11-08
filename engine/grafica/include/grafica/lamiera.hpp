#pragma once
// I pezzi della variante A «Lamiera di bordo» per le schermate di contorno (roadmap 33; docs/ricerca/
// 25_estetica_simulatori.md §8.1 e §8.3, scelta dell'utente del 28/10/2026): la lingua di SELECT.CLS del 1989 a
// 640x480. Lamiera con la grana, pannelli incassati a viti, vetro nero per i valori, targa rivettata per il titolo,
// testo inciso con l'ombra, tasti a spia nei cinque stati, e il piede uguale dappertutto (principio 5): la riga
// d'aiuto in grigio sopra e al piu' quattro tasti, il predefinito col filo d'oro.
// Usa solo i colori fissi 0-15 e gli indici del tema delle stazioni (16-47: smalto, lampade, targhette; applica_tema),
// cioe' quelli delle consolle: una cornice e' uguale dappertutto, e la schermata puo' tenere la carta (96-143,
// 199-231) e il rilievo (48-79). Palette di una schermata: quella del giorno, poi la carta se serve, poi applica_tema.

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688::lamiera {

// Le scritte sulla lamiera (scelta dell'utente del 29/10/2026, le tavole dell'Alfa): chiare con l'ombra sul grigio del
// 688, scure e piatte sul beige dell'Alfa, come le targhette delle sue consolle (KURS, UGROZA). I pezzi che scrivono
// sulla lamiera prendono un ultimo argomento facoltativo: col valore di prima il 688 resta identico
enum class Scritte : std::uint8_t { chiare, scure };
inline Scritte scritte_per(BattelloTema b) { return b == BattelloTema::alfa ? Scritte::scure : Scritte::chiare; }

// Luce dall'alto a sinistra (principio 8): chiaro sopra e a sinistra, scuro sotto e a destra
void smusso(Tela& t, Rettangolo r, std::uint8_t chiaro, std::uint8_t scuro, int spessore = 1);
// La lamiera grigia: due toni dello smalto retinati, un'onda larga, qualche graffio. tono 1..10
void lamiera(Tela& t, Rettangolo r, int tono = 7, std::uint32_t seme = 1);
// Il pannello incassato a 2 pixel con le quattro viti; dentro la lamiera di un tono piu' scuro
void pannello(Tela& t, Rettangolo r, int tono = 8);
// Il vetro nero incassato: il campo di un valore o il fondo di un testo lungo
void vetro(Tela& t, Rettangolo r);
// Il testo inciso: l'ombra a +1,+1, poi il colore
void incisa(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c, std::uint8_t ombra = 0);
// Il testo principale sulla lamiera: bianco inciso con l'ombra nera (chiare), nero senza ombra (scure)
void principale(Tela& t, const Font& f, int x, int y, std::string_view s, Scritte sc = Scritte::chiare);
// Il testo secondario (teatri sotto i nomi, note, intestazioni di colonna): grigio chiaro inciso con l'ombra nera.
// La lamiera e' un grigio medio: lo scuro di prima arrivava a un contrasto di 2,4-3,3, questo con l'ombra si legge
// come il testo principale, che resta bianco (richiesta dell'utente del 28/10/2026)
// (piccola: le maiuscole piccole del font del 1989, per le note sotto una voce)
// Scure: grigio scuro (8) senza ombra
void secondaria(Tela& t, const Font& f, int x, int y, std::string_view s, bool piccola = false, Scritte sc = Scritte::chiare);
// La targa rivettata del titolo, centrata in cx (font d'incisione); restituisce il suo rettangolo
Rettangolo targa(Tela& t, const Font& f, int cx, int y, std::string_view s);

enum class Stato : std::uint8_t { normale, evidenziato, premuto, disabilitato };
// Il tasto a spia: la lente colorata con la scritta. Evidenziato la lente si accende; premuto scende di un pixel;
// disabilitato e' piatto, senza smusso, con la scritta grigia spenta; il predefinito ha il filo d'oro attorno (la
// lampada ambra accesa e spenta del tema: non serve l'oro della cerimonia, che sta nel blocco del rilievo)
void tasto(Tela& t, Rettangolo r, std::string_view testo, ColoreLampada c, Stato s, bool predefinito, const Font& f);

// La spia di una casella (688, Alfa nella scelta della missione): accesa, la lente del colore (0-15) incassata in un
// tasto chiaro; spenta, solo il vano vuoto
void spia(Tela& t, Rettangolo r, bool accesa, std::uint8_t lente);
// Il tasto della tastiera disegnato accanto a un'opzione («TAB», «W»): il cappuccio chiaro con la lettera nera
void tasto_tastiera(Tela& t, Rettangolo r, std::string_view lettera, const Font& f);

struct Tasto {
    std::string testo;   // col tasto davanti: «ESC  INDIETRO», «INVIO  AVVIA»
    ColoreLampada colore = ColoreLampada::ambra;
    Stato stato = Stato::normale;
};
// Il piede: ALTEZZA_PIEDE pixel da y, la riga d'aiuto (vuota: niente fascia) e i tasti su tutta la larghezza
inline constexpr int ALTEZZA_PIEDE = 50;
// Scure: la riga d'aiuto nera su una fascia chiara
void piede(Tela& t, const Font& f, int y, std::string_view aiuto, const std::vector<Tasto>& tasti, int predefinito,
           Scritte sc = Scritte::chiare);

// --- i documenti di B sulla lamiera (gruppo 3: briefing e debriefing, scelte dell'utente del 28/10/2026) ---
// Tutti negli indici 0-47: il bianco e il grigio dello smalto per la carta, il nero, il blu e il rosso EGA per
// l'inchiostro, cosi' la carta del teatro accanto tiene i suoi colori.

// Le righe di un testo a capo per parole nella larghezza w (le parole piu' lunghe restano intere)
std::vector<std::string> a_capo(const Font& f, std::string_view testo, int w);
// SECRET (o la classifica) inciso in rosso a sinistra e il gruppo data-ora inciso a destra, ai lati della targa
// (scure: la classifica in rosso scuro, il gruppo data-ora nero, senza ombra)
void testata_classificata(Tela& t, const Font& f, std::string_view classifica, std::string_view dtg, Scritte sc = Scritte::chiare);
// La carta a modulo continuo della telescrivente: bianca a fasce grigie ogni tre righe di passo_riga, i margini coi
// fori di trascinamento e la perforazione. Restituisce l'area del testo: le righe a y, y + passo_riga... cadono nelle fasce
Rettangolo modulo_continuo(Tela& t, Rettangolo r, int passo_riga);
// Il foglio bianco appoggiato sulla lamiera, con l'ombra: restituisce l'area del testo (8 pixel di margine)
Rettangolo foglio(Tela& t, Rettangolo r);
// Il timbro: il testo nel riquadro a doppio filo del colore; restituisce il riquadro
Rettangolo timbro(Tela& t, const Font& f, int x, int y, std::string_view testo, std::uint8_t colore);
// Una voce del modulo dattiloscritto: l'etichetta in nero, i puntini, il valore «a penna» allineato a destra in w
void voce_modulo(Tela& t, const Font& f, int x, int y, int w, std::string_view etichetta, std::string_view valore,
                 std::uint8_t penna = 1);
// La casella dattiloscritta di un obiettivo: vuota, o con la X a penna
void casella(Tela& t, int x, int y, bool spuntata, std::uint8_t penna = 1);
// La spia di una voce (i livelli del briefing): la lampada del tema accesa o spenta, incassata, col riflesso
void spia_voce(Tela& t, Rettangolo r, ColoreLampada c, bool accesa);
// Un campo della scheda: l'etichetta secondaria e sotto il vetro con le righe colorate. Restituisce l'altezza usata
int campo_vetro(Tela& t, const Font& f, int x, int y, int w, std::string_view etichetta,
                const std::vector<std::pair<std::string, std::uint8_t>>& righe, Scritte sc = Scritte::chiare);

// --- gruppo 4 (museo ed editor, scelte dell'utente del 29/10/2026) ---

// L'elenco nel vetro (l'indice del museo, le liste dell'editor): le voci bianche a passo `passo` da `primo`, la scelta
// sulla lampada ambra accesa con la scritta nera, la tacca dello scorrimento a destra
void elenco_vetro(Tela& t, Rettangolo r, const Font& f, const std::vector<std::string>& voci, int scelta, int passo,
                  int primo = 0);
// La lamiera nei colori della vista (il museo, dove sprite, modelli e foto usano la palette della vista, anche 16-79):
// ogni pixel del tema (16-47) in r diventa il colore piu' vicino fra gli EGA 0-15 e i grigi degli sprite 144-158
// (le lampade fra i soli EGA colorati, cosi' restano rosse, ambra e verdi).
// `p` ha il tema applicato e i grigi del giorno. Si chiama dopo la cornice e prima dell'immagine; poi la palette della
// schermata non applica il tema
void in_colori_vista(Tela& t, Rettangolo r, const Palette& p);

}  // namespace m688::lamiera
