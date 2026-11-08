#pragma once
// Scafo per la fisica dei danni (data/motore/scafi/<classe>.toml, tools/navi/compartimenti.py): sezioni per
// stazioni, celle stagne, zone di fuoco sopra coperta e dati della nave. Idrostatica per stazioni a qualunque
// assetto, anche a nave capovolta (docs/10_danni_affondamento_esplosioni.md §2.2). Per i sottomarini (tipo
// "sottomarino", base della fase D6) anche scafo resistente per stazione, quote e depositi fuori dallo scafo
// resistente; in emersione la fisica e' quella delle navi, con le sezioni dello scafo esterno.
//
// Coordinate del corpo come il kit: x verso prua, y verso sinistra, z verso l'alto, galleggiamento intatto a z = 0.

#include <string>
#include <vector>

namespace m688 {

struct Punto2 {
    double y = 0.0, z = 0.0;
};
using Poligono = std::vector<Punto2>;

// Area e baricentro (y, z) di un poligono semplice.
double area_poligono(const Poligono& p, Punto2* baricentro = nullptr);
// Parte del poligono con a*y + b*z <= c (taglio di Sutherland-Hodgman su un semipiano).
Poligono taglia(const Poligono& p, double a, double b, double c);

struct Stazione {
    double x = 0.0, z_chiglia = 0.0, dz = 0.25;
    std::vector<double> semilarghezze;   // ogni dz dalla chiglia alla coperta
    // scafo resistente dei sottomarini (0 dove manca): raggio, quota dell'asse e sua distanza dal piano di
    // simmetria (> 0: due scafi affiancati, a +-y_resistente, come nel Typhoon)
    double r_resistente = 0.0, z_resistente = 0.0, y_resistente = 0.0;
    Poligono sezione() const;            // sezione chiusa (dritta, poi sinistra), vuota se manca
    double coperta() const { return z_chiglia + dz * static_cast<double>(semilarghezze.empty() ? 0 : semilarghezze.size() - 1); }
};

struct CellaScafo {
    std::string nome, uso, deposito;   // uso: locali, macchine, deposito, carico, ala; sottomarini: zavorra, reattore, batterie
    double x0 = 0, x1 = 0, y0 = -1e3, y1 = 1e3;
    double permeabilita = 0.95;
    double acqua_iniziale = 0.0;   // m^3 a nave intatta: zavorra dei sottomarini nella condizione delle foto
    // i danni critici (kit, fase 2: tools/navi/danni_critici.py): la carica netta del deposito nella cella (kg di alto
    // esplosivo, 0 = la regola di prima), la sua protezione (0-1), il propellente (kg, deflagra a 0,3 in TNT e brucia),
    // il carburante (tonnellate e tipo: nafta, gasolio, jp5, ... o il carico delle cisterne)
    double carica_kg = 0.0, protezione = 0.0, propellente_kg = 0.0, carburante_t = 0.0;
    std::string carburante;
};

struct ZonaFuoco {   // sopra coperta: hangar, lanciatori in coperta; sottomarini: depositi fuori dallo scafo resistente
    std::string nome, carico;
    double x0 = 0, x1 = 0;
    double carica_kg = 0.0;   // le armi in coperta (kit, fase 2): la carica netta che puo' esplodere col fuoco o un colpo
};

struct DatiSottomarino {   // sezione [sottomarino] (dislocamenti pubblicati, quote dal dossier 01)
    bool presente = false;
    std::string scafo;   // singolo, doppio
    double dislocamento_emersione_t = 0, dislocamento_immersione_t = 0, riserva_spinta = 0;
    double quota_collaudo_m = 0, quota_collasso_m = 0;
    double intercapedine_m = 0, volume_resistente_m3 = 0, volume_esterno_m3 = 0;
};

struct DatiScafo {
    std::string tipo;   // militare, mercantile, petroliera, portacontainer
    double GM = 1.0, dislocamento_pubblicato_t = 0.0, qualita_controllo_danni = 1.0, resistenza_trave = 1.0;
    int squadre = 2;
    double lunghezza = 0.0, larghezza = 0.0, passo = 1.0;
    std::vector<Stazione> stazioni;
    std::vector<CellaScafo> celle;
    std::vector<ZonaFuoco> sopra_coperta;
    DatiSottomarino sottomarino;
    std::vector<ZonaFuoco> fuori_resistente;   // contenitori dei missili fra i due scafi (Oscar II, Charlie II)

    static DatiScafo carica(const std::string& percorso);   // lancia std::runtime_error
};

// Assetto di un corpo rigido: rotazioni attorno all'origine del corpo (x_o, 0, 0) nell'ordine del disegno
// (prima l'assetto longitudinale, poi lo sbandamento: grafica/modello.cpp), poi lo spostamento.
struct Assetto {
    double immersione = 0.0;     // m, positivo verso il basso
    double sbandamento = 0.0;    // rad, positivo sulla dritta
    double appoppamento = 0.0;   // rad, positivo = prua alta (come AssettoScafo::assetto)
    double x_o = 0.0, dx = 0.0, dy = 0.0;

    // quota nel mondo di un punto del corpo: a(x) + b*y + c*z
    double quota(double x, double y, double z) const;
    void coefficienti(double x, double& a, double& b, double& c) const;
    void mondo(double x, double y, double z, double& X, double& Y, double& Z) const;
};

}  // namespace m688
