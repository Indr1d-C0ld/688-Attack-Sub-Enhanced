#pragma once
// Carta di navigazione dell'Enhanced in stile VGA/SVGA alla 688(I) (roadmap Fase 3, «Carta di navigazione piu' bella»;
// docs/11_comandi_e_stazioni.md §11 N1): il fondale disegnato pixel per pixel, con la profondita' interpolata, la
// retinatura di Bayer 4x4 fra le fasce, l'ombreggiatura da nord-ovest su fondali e terra, la terra in tinte di quota
// per luce (indici 48-79), la costa liscia con la fascia chiara del bassofondo, le isole piccole che non spariscono,
// le isobate al pixel (principali doppie) coi numeri, le zone non rilevate della carta d'epoca tratteggiate e il
// contorno di sicurezza.
//
// Lo usa app/carta.cpp (sessione principale) al posto di disegna_fondale_vga, solo nell'Enhanced e negli stili «VGA
// 1989» e «carta nautica»; il classico e il «foglio» restano come sono. La palette dell'acqua (96-143), della costa,
// delle isobate e del contorno di sicurezza resta quella di app/carta.cpp (applica_palette_carta): gli stessi indici
// sono qui sotto. Il rilievo della terra sta negli indici 48-79 (applica_palette_rilievo).
//
// Il disegno si tiene in cache con un margine attorno alla vista: finche' il centro si sposta meno del margine la
// cache si trasla; si rifa' quando cambiano la scala, la carta, lo stile o il contorno di sicurezza.

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carta_giocatore.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

class Font;

namespace colori_carta_vga {
// gli stessi di app/carta.hpp (colori_carta)
inline constexpr int ACQUA = 96;   // 16 fasce di profondita' x 3 ombre (scura, media, chiara)
inline constexpr int COSTA = 199, ISOBATA = 207, ISOBATA_PRINCIPALE = 208, SICUREZZA = 209;
inline constexpr int RETICOLO = 210, MATITA = 212, MAGENTA = 229;   // matite: MATITA nero, MATITA + 5 bianco
// il rilievo della terra: 8 tinte di quota x 4 luci (ombra piena, ombra, media, luce)
inline constexpr int RILIEVO = 48, N_TINTE = 8, N_LUCI = 4;
}  // namespace colori_carta_vga

// La geometria della vista: la stessa di VistaCarta (app/carta.hpp)
struct GeometriaCarta {
    int x = 0, y = 0, w = LARGHEZZA_TELA, h = 360;   // area della tela
    Vec2 centro;                                     // metri
    double metri_per_pixel = 1.0;
    Vec2 a_tela(Vec2 p) const { return {x + w / 2.0 + (p.x - centro.x) / metri_per_pixel, y + h / 2.0 - (p.y - centro.y) / metri_per_pixel}; }
    Vec2 a_mondo(Vec2 q) const { return {centro.x + (q.x - x - w / 2.0) * metri_per_pixel, centro.y - (q.y - y - h / 2.0) * metri_per_pixel}; }
};

struct OpzioniCarta {
    StileCarta stile = StileCarta::vga;
    double sicurezza_m = 0.0;            // contorno di sicurezza (0: nessuno)
    bool rilievo = true;                 // ombreggiatura di fondali e terra
    const Font* font_isobate = nullptr;  // i numeri delle isobate principali (nullptr: senza numeri)
    // scandagli: profondita' sparse in metri, nello strato del fondale (sotto contatti e simboli), col font delle isobate;
    // sulla carta d'epoca solo nelle zone rilevate (dove la carta «sa»), lontano da costa e numeri delle isobate
    bool scandagli = false;
    // il passo del tratteggio diagonale delle zone non rilevate, in pixel (proposta A del kit, 29/10/2026: 12, piu'
    // leggero; 6 e' quello di oggi)
    int passo_tratteggio = 6;
    // il tempo della missione, per il contorno di sicurezza che segue la quota: con un tempo, mentre la quota cambia il contorno puo'
    // restare indietro di 10 m e un secondo dopo che si e' fermata e' esatto. Negativo: sempre esatto
    double tempo_s = -1.0;
};

// La fascia di profondita' della carta (0-15, le soglie delle isobate) e il suo indice con l'ombra (0 scura, 1 media,
// 2 chiara): per chi colora sulla carta una profondita' propria (il quadro del fondale, app/vista_fondale)
int fascia_acqua_vga(double profondita_m);
int fascia_acqua_vga(double profondita_m, double& dentro);   // e quanto vi si e' dentro (0-1), per retinare come la carta
inline std::uint8_t indice_acqua(int fascia, int ombra) {
    return static_cast<std::uint8_t>(colori_carta_vga::ACQUA + std::clamp(fascia, 0, 15) * 3 + std::clamp(ombra, 0, 2));
}

// Le tinte della terra (camoscio, ocra, bruno; nella «carta nautica» piu' chiare) per 4 luci negli indici 48-79
void applica_palette_rilievo(Palette& p, StileCarta stile);

// Il fondale della carta con la sua cache
class FondaleVga {
public:
    // Disegna il fondale nell'area di g (ritaglio compreso); ricompone solo quando serve
    void disegna(Tela& t, const GeometriaCarta& g, const Fondale& f, const OpzioniCarta& o);
    void invalida() { valida_ = false; }
    bool ricomposto() const { return ricomposto_; }          // l'ultimo disegna ha ricomposto la cache
    double tempo_ricomposizione_ms() const { return ms_; }   // l'ultima ricomposizione
    static constexpr int MARGINE = 48;                        // pixel di cache attorno alla vista

private:
    void ricomponi(const GeometriaCarta& g, const Fondale& f, const OpzioniCarta& o);
    std::vector<std::uint8_t> pix_;
    int cw_ = 0, ch_ = 0;            // dimensioni della cache (vista + 2 margini)
    GeometriaCarta g_;               // la vista della ricomposizione
    const Fondale* fondale_ = nullptr;
    int edizione_ = -1;
    OpzioniCarta o_;
    bool valida_ = false, ricomposto_ = false;
    double sicurezza_vista_ = 0.0, sicurezza_cambiata_s_ = 0.0;   // l'ultimo contorno chiesto, e da quando
    double ms_ = 0.0;
};

// --- mobilia della carta (sopra il fondale, fuori dalla cache): ogni pezzo si accende da solo ---
struct MobiliaCarta {
    bool cornice = false, rosa = false, scala = false, cartiglio = false;
    int riserva_alto = 0, riserva_basso = 0;   // righe dell'area lasciate libere (intestazione, scritte della stazione)
    double declinazione_gradi = 0.0;           // positiva a est (declinazione_stima); l'anello magnetico della rosa
    int anno_declinazione = 1990;
    bool sovietica = false;                    // cartiglio e scritte all'uso sovietico (traslitterato)
    std::string titolo;                        // il nome della carta (il teatro)
    int edizione = 0;                          // anno dell'edizione (Fondale::edizione() della carta d'epoca; 0: nessuno)
    bool zone_non_rilevate = false;            // la carta d'epoca: nel cartiglio la riga del tratteggio (la carta grande,
                                               // scelta dell'utente del 29/10/2026, non ha piu' la riga sopra la carta)
};
// Lo spazio che la mobilia lascia ai toponimi: l'interno della cornice e i pezzi da non coprire (rosa, scala, cartiglio)
struct SpazioMobilia {
    Rettangolo interno;
    std::vector<Rettangolo> occupati;
};
// Tutti i pezzi accesi, ai loro posti: cornice sui bordi, rosa in basso a destra, scala e cartiglio in basso a sinistra
SpazioMobilia disegna_mobilia(Tela& t, const GeometriaCarta& g, const Fondale& f, const Font& font, const MobiliaCarta& m);
// La cornice coi minuti di latitudine e longitudine: barrette bianche e nere sui quattro bordi (il passo cresce con la
// scala: 1, 2, 5, 10, 15, 30, 60 primi), le etichette ogni cinque barrette; serve una carta georeferenziata
void cornice_minuti(Tela& t, const GeometriaCarta& g, const Fondale& f, const Font& font, int riserva_alto, int riserva_basso);
// La rosa dei venti (magenta, come sulle carte nautiche): anello vero coi gradi, anello magnetico ruotato della
// declinazione, la scritta della declinazione e dell'anno
void rosa_dei_venti(Tela& t, int cx, int cy, int r, double declinazione_gradi, int anno, bool sovietica, const Font& font);
// La scala grafica in miglia (a tratti bianchi e neri) con l'origine in (x, y); restituisce la larghezza occupata
int scala_grafica(Tela& t, int x, int y, double metri_per_pixel, bool sovietica, const Font& font);
// Il cartiglio: ente, titolo, edizione e unita' degli scandagli; l'angolo in basso a sinistra in (x, y)
Rettangolo cartiglio(Tela& t, int x, int y, const MobiliaCarta& m, const Font& font);
// Declinazione magnetica del teatro nel 1990, gradi a est (STIMA dal modello IGRF/WMM per il centro del teatro, da
// verificare): baltico, barents, celtica, faroe, gibilterra, kunashir, otranto; 0 per un teatro sconosciuto
double declinazione_stima(std::string_view teatro);

// --- toponimi (data/motore/teatri/<teatro>.toponimi.toml, da tools/pipeline/toponimi.py: Natural Earth e aggiunte) ---
struct Toponimo {
    std::string testo, en, tipo;   // tipo: mare, oceano, golfo, baia, stretto, canale, secca, isola, isole, penisola,
                                   // capo, costa, regione, citta, base...
    bool acqua = false;            // nome d'acqua: blu e inclinato, come sulle carte nautiche
    int rango = 5;                 // 1 il piu' importante: decide da che scala compare
    Vec2 pos;                      // metri sul piano del teatro
};
// Il file del teatro: <cartella_dati>/teatri/<teatro>.toponimi.toml
std::string file_toponimi(const std::string& cartella_dati, std::string_view teatro);
// I toponimi del file (vuoto se manca o non si legge: la carta resta senza nomi)
std::vector<Toponimo> carica_toponimi(const std::string& percorso);
// Il rango massimo che si scrive alla scala data: piu' si ingrandisce, piu' nomi
int rango_massimo_toponimi(double metri_per_pixel);
struct OpzioniToponimi {
    Rettangolo area;                   // dove si scrive (l'interno della cornice; w = 0: tutta la vista)
    std::vector<Rettangolo> occupati;  // da non coprire (la mobilia, le scritte della stazione)
    bool inglese = false;              // il nome inglese (per la versione in inglese)
};
// Scrive i nomi in ordine di rango, senza sovrapporli fra loro ne' ai rettangoli occupati: i mari maggiori in maiuscolo
// spaziato, le acque in blu inclinate, la terra in nero diritta; citta' e basi col loro segno. Le lettere che il font non
// ha si scrivono con la loro riserva latina (Font::caratteri). Restituisce quanti nomi ha scritto.
int disegna_toponimi(Tela& t, const GeometriaCarta& g, const std::vector<Toponimo>& nomi, const Font& font,
                     const OpzioniToponimi& o);
// Testo col contorno di un pixel (l'alone), leggibile su ogni fondo
void testo_con_alone(Tela& t, int x, int y, std::string_view testo, const Font& f, std::uint8_t colore, std::uint8_t alone,
                     int scala = 1);

}  // namespace m688
