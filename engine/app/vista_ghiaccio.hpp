#pragma once
// La banchisa per la vista del periscopio dal campo vero del ghiaccio (docs/28 §3.3): la copertura dove sta l'occhio e lo
// spostamento del pack sono quelli del campo, cosi' il disegno della volta resta attaccato al campo anche dopo un cambio di
// deriva o sopra una polynya del vetrino di Dio. Senza cambi e' la copertura del meteo e deriva per tempo, bit per bit.
// Spenta senza il campo o senza il giocatore.
#include "grafica/oculare.hpp"
#include "missioni/missione.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/dinamica_sottomarino.hpp"

namespace m688 {

inline oculare::GhiaccioVista vista_ghiaccio(const Missione& m) {
    oculare::GhiaccioVista v;
    const CampoGhiaccio* cg = m.ghiaccio();
    const auto* g = m.giocatore();
    if (!cg || !g) return v;
    const double t = m.tempo();
    v.attivo = true;
    v.copertura = m.copertura_ghiaccio(g->pos);
    v.cella_m = cg->parametri().cella_m;
    v.deriva_m = cg->spostamento(t);
    v.campione = [cg, t](Vec2 p) {
        const PuntoGhiaccio x = cg->in(p, t);
        oculare::CampioneGhiaccio c;
        c.ghiaccio = x.ghiaccio;
        c.nilas = x.nilas;
        c.spessore_m = x.spessore_m;
        c.cresta_m = x.chiglia ? x.pescaggio_m / 4.5 : 0.0;
        return c;
    };
    return v;
}

// La telecamera verticale dell'Alfa: la quota del battello e la copertura sopra di lui (sotto una polynya aperta, 0)
struct DatiTelecamera {
    double quota_m = 0.0, ghiaccio = 0.0;
};
inline DatiTelecamera dati_telecamera(const Missione& m) {
    DatiTelecamera c;
    const auto* g = m.giocatore();
    if (!g) return c;
    const DinamicaSottomarino* d = m.dinamica(g->id);
    c.quota_m = d ? d->quota_m() : g->quota;
    if (m.ghiaccio()) c.ghiaccio = m.copertura_ghiaccio(g->pos);
    return c;
}

}  // namespace m688
