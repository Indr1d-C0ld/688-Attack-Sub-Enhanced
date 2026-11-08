#pragma once
// Strati meteo sulla carta dell'Enhanced (docs/15_meteo_e_mare.md, M3; dossier 21 §5.3 e §9, nota D A4: alla Harpoon
// II). Disegna sopra la carta gia' fatta:
// - le barbe del vento (convenzione WMO: l'asta punta da dove viene il vento; mezza barba 5 nodi, barba 10, bandiera 50);
// - lo stato del mare in cifre Douglas al piede di ogni barba;
// - le celle temporalesche (cerchio tratteggiato col fulmine al centro);
// - il ghiaccio (tratteggio obliquo dove la copertura supera la meta', col bordo);
// - i banchi di nebbia noti (puntinato).
// La carta e' del chiamante (app/carta): proiezione, area e colori arrivano da fuori; il meteo da funzioni (il giocatore
// vede quello noto, non la verita'). Senza funzioni lo strato corrispondente non si disegna.

#include <functional>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

struct CellaSuCarta {
    Vec2 pos;                          // metri
    double raggio_m = 10000.0;
};

struct ColoriStrati {                  // indici della palette della carta
    std::uint8_t vento = 14, mare = 14, celle = 4, ghiaccio = 15, nebbia = 7;
};

struct StratiMeteo {
    std::function<Vec2(Vec2)> a_tela;                       // metri -> pixel
    std::function<Vec2(Vec2)> a_mondo;                      // pixel -> metri
    Rettangolo area{0, 0, 640, 480};
    int passo_px = 64;                                      // griglia delle barbe
    int campioni_px = 4;                                    // passo di campionamento di ghiaccio e nebbia
    std::function<void(Vec2, double&, double&)> vento;      // m/s, gradi da cui viene
    std::function<int(Vec2)> stato_mare;                    // Douglas 0-9
    std::function<double(Vec2)> ghiaccio;                   // copertura 0-1
    std::function<bool(Vec2)> nebbia;                       // banco di nebbia noto
    std::vector<CellaSuCarta> celle;
    // fulmini sentiti al sonar: rilevamenti veri (gradi) dal battello, segnati come raggi tratteggiati col fulmine in punta
    Vec2 battello;
    std::vector<double> rilevamenti_fulmini;
    ColoriStrati colori;
};

void disegna_strati_meteo(Tela& t, const Font& font, const StratiMeteo& s);
// Una barba del vento con l'asta lunga `lunghezza` pixel dal punto (x, y) verso da dove viene il vento
void disegna_barba(Tela& t, double x, double y, double vento_kn, double da_gradi, int lunghezza, std::uint8_t colore);

}  // namespace m688
