#pragma once
// I fari della costa (manuale C9 §2.15, kit 31/10/2026; solo nell'Enhanced): data/motore/fari.toml, da Wikidata (CC0)
// con tools/pipeline/fari.py. Modulo puro: la posizione geografica la porta nel piano della missione chi ha il
// fondale (Fondale::proiettata); la vista del periscopio li disegna come i fanali.

#include <functional>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"
#include "nucleo/luci_navigazione.hpp"

namespace m688 {

struct Faro {
    std::string teatro, id, nome, caratteristica;   // id: l'elemento di Wikidata (Q...), stabile
    double lat = 0.0, lon = 0.0;                     // gradi WGS84
    double quota_m = 20.0;                           // altezza focale sul mare
    double portata_m = 0.0;                          // portata nominale
    luci::Colore colore = luci::Colore::bianco;
    double periodo_s = 0.0;                          // 0: luce fissa
    std::vector<double> lampi;                       // acceso, spento, acceso, ... (s) dentro il periodo
    int anno = 0;                                    // prima accensione, 0 se non si sa
};

// I fari del teatro (tutti se `teatro` e' vuoto); vuoto se il file manca. std::runtime_error se e' guasto
std::vector<Faro> carica_fari(const std::string& percorso, const std::string& teatro = {});
// La luce e' accesa all'istante t (s)? I fari non vanno in fase fra loro: lo sfasamento viene dall'id
bool faro_acceso(const Faro& f, double t);
// La terra fra l'occhio e il faro copre la luce? La quota del terreno (`quota`, positiva sulla terra, come il fondale)
// si campiona lungo la congiungente ogni `passo_m`, saltando gli ultimi 300 m prima del faro (il capo su cui sta); la
// luce e' coperta dove la terra sale sopra la retta fra l'occhio e la lanterna, con la curvatura della Terra (raggio
// efficace 4/3 per la rifrazione)
bool faro_coperto(const std::function<double(Vec2)>& quota, double passo_m, Vec2 occhio, double quota_occhio_m, Vec2 faro,
                  double quota_faro_m);

}  // namespace m688
