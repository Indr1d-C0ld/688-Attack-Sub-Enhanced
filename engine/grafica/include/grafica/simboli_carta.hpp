#pragma once
// I simboli delle viste dall'alto, uguali in ogni schermata: carta della stazione, briefing, debriefing, replay, vista
// dal vivo della sandbox ed editor (proposta del kit kit_simboli_coerenza.md, 27/10/2026). Fuori dalle lettere dei
// contatti della stazione (il codice di classificazione del 1989, che resta com'e'), la parte la dicono il colore NTDS
// e la forma APP-6:
// - amico: azzurro, cerchio;
// - ostile: rosa, rombo;
// - ignoto: giallo, ottagono;
// - neutrale: verde, quadrato.
// Il giallo NTDS vuol dire solo «ignoto». Il battello del giocatore e' il quadratino verde della stazione, con la scia
// e l'ellisse di navigazione nel suo verde; la croce della soluzione TMA e' quella della stazione, bianca sulla VGA e
// a matita sulla carta chiara.
//
// Di serie ogni simbolo e ogni scritta ha l'alone nero di un pixel (gli otto vicini), come nei simulatori d'epoca:
// risalta sul mare scuro, sul mare chiaro e sulla terra (richiesta dell'utente, 27/10/2026). Chi disegna cose sue
// sulla carta (le X dei contatti, le cadute, i punti di rotta) passa dal Pennello o da scrivi(), e ha lo stesso alone.
//
// I colori sono indici della palette della carta: gli stessi posti di colori_carta (app/carta.hpp), che
// applica_palette_carta riempie; le prove controllano che coincidano. Il modulo non dipende dall'app.

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/tela.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

class Font;

namespace simboli {

// i posti della palette della carta (colori_carta in app/carta.hpp)
inline constexpr int MATITA = 212;     // 6 matite: nero, rosso, blu, verde, giallo, bianco
inline constexpr int NTDS = 218;       // amico, ostile, ignoto, neutrale; da 222 gli stessi attenuati
inline constexpr int CURSORE = 230, EVIDENZA = 231;

// L'alone di serie (vero); le anteprime «prima e dopo» lo spengono
void imposta_alone(bool si);
bool alone();

// Raccoglie i pixel di un disegno e li stende sulla tela con l'alone: prima il nero sugli otto vicini liberi, poi il
// disegno. Cosi' l'alone non copre mai il simbolo stesso, e due tratti vicini non si cancellano a vicenda.
class Pennello {
public:
    void pixel(int x, int y, std::uint8_t c);
    void orizzontale(int x0, int x1, int y, std::uint8_t c);
    void verticale(int x, int y0, int y1, std::uint8_t c);
    void rettangolo(int x, int y, int w, int h, std::uint8_t c);
    void linea(Vec2 a, Vec2 b, std::uint8_t c, int tratto = 0);   // tratto > 0: tratteggiata
    void arco(Vec2 centro, double r, double da_gradi, double a_gradi, std::uint8_t c);   // gradi matematici, 0 = est
    // stende sulla tela; con_alone come alone() se non si dice
    void stendi(Tela& t) const { stendi(t, alone()); }
    void stendi(Tela& t, bool con_alone) const;
    bool vuoto() const { return p_.empty(); }

private:
    struct Punto {
        int x, y;
        std::uint8_t c;
    };
    std::vector<Punto> p_;
};

// Una scritta della carta (etichette, lettere dei contatti, intelligence) con l'alone come i simboli
void scrivi(Tela& t, const Font& f, int x, int y, std::string_view testo, std::uint8_t colore, int scala = 1);

enum class Parte : std::uint8_t { amico, ostile, ignoto, neutrale };   // nell'ordine dei posti NTDS
// Il dominio APP-6: superficie col telaio intero; subacqueo la meta' inferiore; aereo la superiore, l'elicottero con
// le pale; terra col telaio intero sopra una base; ignoto (nemmeno la categoria e' nota) col telaio intero e il «?»
// dentro. «Categoria nota, classe ignota» e' la Parte::ignoto col dominio noto.
enum class Dominio : std::uint8_t { superficie, subacqueo, aereo, elicottero, terra, ignoto };
// Le cose in acqua che non sono unita' con un telaio
enum class Oggetto : std::uint8_t { siluro, missile, esca, mina, boa, biologico };
enum class Emissione : std::uint8_t { ping, radar, radio };

// Il colore NTDS della parte; attenuato: la copia sbiadita (scie, ellissi, unita' di sfondo, dati vecchi)
std::uint8_t indice_parte(Parte p, bool attenuato = false);
// Un dato di eta_s secondi: sbiadito oltre ETA_SBIADITA_S (10 minuti, come i rilevamenti della carta)
inline constexpr double ETA_SBIADITA_S = 600.0;
inline std::uint8_t indice_parte_eta(Parte p, double eta_s) { return indice_parte(p, eta_s > ETA_SBIADITA_S); }

// Dai nomi della simulazione (categoria e classe di UnitaClassica, della traccia del replay, di VoceParte) al simbolo:
// - e_oggetto: le categorie che non hanno un telaio (arma, boa, biologico, esca, mina): si disegnano con
//   disegna_oggetto; tutte le altre, anche «nessuna» e le sconosciute, sono unita';
// - dominio_da_categoria: sottomarino, nave, aereo (gli elicotteri per classe), terra o base; vuota, «nessuna» o
//   sconosciuta: ignoto;
// - oggetto_da_classe: boa, biologico, esca ed esca_mobile, le mine (mk67, mk60, pmr1, pmr2, mdt), i missili
//   (harpoon, tasm, tlam_c, tomahawk, sea_lance, p700, vyuga); il resto e' un siluro.
bool e_oggetto(std::string_view categoria);
Dominio dominio_da_categoria(std::string_view categoria, std::string_view classe = {});
Oggetto oggetto_da_classe(std::string_view categoria, std::string_view classe);
// La matita della parte nello stile 1989 della stazione: rosso ostile, nero ignoto (bianco sulla VGA), blu amico,
// verde neutrale
std::uint8_t indice_matita(Parte p, bool vga);
// Il verde del battello del giocatore (quadratino, scia, ellisse di navigazione)
inline constexpr std::uint8_t VERDE_PROPRIO = 10, VERDE_PROPRIO_SCURO = 2;
// La croce della soluzione TMA: bianca sulla carta VGA, matita nera sulla carta chiara
std::uint8_t indice_croce_tma(bool vga);
// Il rapporto fra il semiasse dell'ellisse al 95% e la sigma (chi quadro a 2 gradi di liberta')
inline constexpr double K_ELLISSE_95 = 2.4477;

// Ogni funzione disegna da sola col suo alone; le varianti col Pennello aggiungono al disegno di chi chiama, che lo
// stende una volta sola (un gruppo di simboli che si toccano, con un alone unico).
//
// Il telaio APP-6 di raggio 5 attorno a q (gli stessi pixel dei segni a matita della carta)
void disegna_simbolo_unita(Pennello& pn, Vec2 q, Parte p, Dominio d, std::uint8_t colore);
void disegna_simbolo_unita(Tela& t, Vec2 q, Parte p, Dominio d, std::uint8_t colore);
inline void disegna_simbolo_unita(Tela& t, Vec2 q, Parte p, Dominio d) { disegna_simbolo_unita(t, q, p, d, indice_parte(p)); }
// Il battello del giocatore: il quadratino 7x7 verde scuro con dentro il 5x5 verde chiaro; evidenza: l'anello di
// raggio 8 dell'unita' seguita (vista dal vivo)
void disegna_proprio(Pennello& pn, Vec2 q, bool evidenza = false);
void disegna_proprio(Tela& t, Vec2 q, bool evidenza = false);
// Siluro e missile: una freccina lungo la rotta (gradi, 0 = nord, verso le y decrescenti), il missile con la coda
// doppia; esca: un rombo vuoto; mina: il cerchio con quattro punte (come le mine della carta, l'alone al posto
// dell'ombra); boa: il galleggiante 3x3 con l'antenna; biologico: la coda di balena, sempre grigia (non ha parte)
void disegna_oggetto(Pennello& pn, Vec2 q, Oggetto o, std::uint8_t colore, double rotta_gradi = 0.0);
void disegna_oggetto(Tela& t, Vec2 q, Oggetto o, std::uint8_t colore, double rotta_gradi = 0.0);
// Il filo di un'arma guidata, tratteggiato, verso chi la guida
void disegna_filo(Pennello& pn, Vec2 arma, Vec2 guida, std::uint8_t colore);
void disegna_filo(Tela& t, Vec2 arma, Vec2 guida, std::uint8_t colore);
// La X rossa sopra un'unita' distrutta o persa, dopo il simbolo: sta dentro il suo alone e non ne ha uno suo (un alone
// fra i tratti della X e del telaio farebbe un grumo nero)
void disegna_distrutta(Pennello& pn, Vec2 q);
void disegna_distrutta(Tela& t, Vec2 q);
// Le emissioni attorno all'unita', animate col tempo t_s: l'anello del ping che si allarga (bianco, poi grigio), il
// ventaglio del radar che gira (ciano chiaro), il fulmine della radio (giallo)
void disegna_emissione(Pennello& pn, Vec2 q, Emissione e, double t_s);
void disegna_emissione(Tela& t, Vec2 q, Emissione e, double t_s);
// Gli angoli dell'unita' scelta (vista dal vivo, replay, editor): un quadrato di 18 pixel, in EVIDENZA
void disegna_scelta(Pennello& pn, Vec2 q);
void disegna_scelta(Tela& t, Vec2 q);
// La linea di rilevamento di un contatto senza posizione, tratteggiata; specchio: quella dell'ambiguita' della
// schiera, a tratti piu' lunghi
void disegna_rilevamento(Pennello& pn, Vec2 da, Vec2 a, std::uint8_t colore, bool specchio = false);
void disegna_rilevamento(Tela& t, Vec2 da, Vec2 a, std::uint8_t colore, bool specchio = false);
// La croce della soluzione TMA, 7x7
void disegna_croce_tma(Pennello& pn, Vec2 q, std::uint8_t colore);
void disegna_croce_tma(Tela& t, Vec2 q, std::uint8_t colore);
// L'ellisse tratteggiata coi semiassi in pixel, orientamento del semiasse maggiore in gradi da nord; per l'ellisse
// al 95% da una sigma: semiasse = K_ELLISSE_95 * sigma
void disegna_ellisse(Pennello& pn, Vec2 centro, double maggiore_px, double minore_px, double orientamento_gradi,
                     std::uint8_t colore);
void disegna_ellisse(Tela& t, Vec2 centro, double maggiore_px, double minore_px, double orientamento_gradi,
                     std::uint8_t colore);

// --- il vetrino di Dio (docs/28 §6): i segni degli interventi sulla carta ---
enum class FamigliaIntervento : std::uint8_t { forze, guasti, ambiente, comunicazioni, inganno };
// Il colore di base della famiglia: forze magenta chiaro (13), guasti marrone (6), ambiente ciano chiaro (11),
// comunicazioni bianco (15), inganno magenta scuro (5)
std::uint8_t indice_famiglia(FamigliaIntervento f);
// Dal gruppo del catalogo degli interventi (FORZE, GUASTI, AMBIENTE, COMUNICAZIONI, INGANNO, anche in minuscolo);
// un gruppo ignoto e' inganno, il punto interrogativo
FamigliaIntervento famiglia_da_gruppo(const std::string& gruppo);
// Il segno di 9x9 pixel centrato in q: la bandierina (forze), la chiave inglese (guasti), l'onda (ambiente), gli archi
// radio (comunicazioni), il punto interrogativo nel rombo (inganno, 11x11); programmato: in piu' gli angoli di un
// quadrato di 13x13 (l'intervento a tempo, non ancora applicato). Il colore di solito e' indice_famiglia(f)
void disegna_intervento(Pennello& pn, Vec2 q, FamigliaIntervento f, std::uint8_t colore, bool programmato = false);
void disegna_intervento(Tela& t, Vec2 q, FamigliaIntervento f, std::uint8_t colore, bool programmato = false);
// Il falso contatto e la falsa flag: l'anellino dove l'unita' e' davvero, la linea tratteggiata e la doppia punta
// dove la si crede
void disegna_doppia_freccia(Pennello& pn, Vec2 vera, Vec2 creduta, std::uint8_t colore);
void disegna_doppia_freccia(Tela& t, Vec2 vera, Vec2 creduta, std::uint8_t colore);
// La portata ridotta di un sensore guasto: la portata piena a puntini radi in grigio, la ridotta tratteggiata nel
// colore, e otto trattini fra le due che puntano verso il centro (raggi in pixel)
void disegna_portata_ridotta(Pennello& pn, Vec2 centro, double piena_px, double ridotta_px, std::uint8_t colore);
void disegna_portata_ridotta(Tela& t, Vec2 centro, double piena_px, double ridotta_px, std::uint8_t colore);

// Una voce della legenda: il simbolo vero e la parola accanto
struct VoceLegenda {
    enum class Tipo : std::uint8_t { unita, proprio, oggetto, distrutta, emissione, croce_tma, scia } tipo = Tipo::unita;
    Parte parte = Parte::amico;
    Dominio dominio = Dominio::superficie;
    Oggetto oggetto = Oggetto::siluro;
    Emissione emissione = Emissione::ping;
    std::uint8_t colore = 0;   // croce TMA e scia; 0: quello della parte
    std::string testo;
};
// La legenda in colonna da (x, y), una riga ogni 14 pixel col simbolo nei primi 14; restituisce l'altezza occupata
int disegna_legenda(Tela& t, const Font& f, int x, int y, std::span<const VoceLegenda> voci, std::uint8_t colore_testo);

}  // namespace simboli
}  // namespace m688
