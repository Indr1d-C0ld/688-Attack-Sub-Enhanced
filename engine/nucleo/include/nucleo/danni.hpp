#pragma once
// Fisica dei danni delle navi di superficie (docs/10_danni_affondamento_esplosioni.md §2, fase D3; numeri dal
// dossier docs/ricerca/07_danni_affondamento_esplosioni.md e da docs/ricerca/02_acustica_oceanografia_dottrina.md §9).
//
// Una nave e' uno o piu' corpi rigidi (piu' di uno dopo una rottura della trave). Ogni corpo ha le sue stazioni,
// le sue celle stagne e il suo assetto:
// - spinta per stazioni sulle sezioni dello scafo, a qualunque assetto (anche capovolto);
// - acqua imbarcata come peso nelle celle, con il pelo libero orizzontale nel mondo: le superfici libere riducono
//   la stabilita' da sole;
// - flussi dall'efflusso Q = Cd A sqrt(2 g dh) attraverso falle, aperture fra celle (perdite delle paratie, paratie
//   squarciate) e aperture di coperta (allagamento quando la coperta va sott'acqua);
// - dinamica di sussulto, rollio e beccheggio con masse aggiunte e smorzamento;
// - trave di scafo: momento flettente contro la resistenza stimata dal modulo minimo di regolamento; oltre, la
//   nave si spezza e i due tronconi proseguono per conto loro;
// - esplosioni: falla a contatto D = 1,25 W^(1/3), rottura della chiglia per i siluri sotto la chiglia con la
//   regola tarata del dossier 07 (§3.4, §11.3), prua o poppa asportate fuori dal tratto centrale;
// - incendi per locale (crescita, propagazione, combustibile), esplosione dei depositi, energia elettrica;
// - controllo danni: tamponature, pompe, lotta agli incendi, allagamento dei depositi, controallagamento;
// - sottomarini in emersione: zavorra iniziale nelle casse (stesso peso e stesso baricentro del battello) ed
//   esaurimento d'emergenza delle casse integre quando il battello imbarca acqua.
// Tutto il caso viene dal seme: stessa nave, stessi colpi, stesso affondamento.

#include <cstdint>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/scafo.hpp"

namespace m688 {

struct DanniUrtoUnita;   // nucleo/collisioni.hpp

struct Esplosione {
    enum class Tipo { siluro, mina, missile, bomba, deposito, proietto };   // proietto: artiglieria fra navi (G3), meno incendi
    Tipo tipo = Tipo::siluro;
    double x = 0.0, y = 0.0, z = -5.0;   // punto di scoppio, coordinate della nave intatta (m)
    double tnt_kg = 300.0;               // TNT equivalente (per la bolla, sott'acqua)
    bool contatto = true;                // a contatto con lo scafo; altrimenti a distanza (spoletta d'influenza)
};

// Esplosione avvenuta, per gli effetti grafici (grafica/effetti.hpp): posizione nel riferimento della nave (coordinate
// del modello dopo l'assetto del corpo colpito, z = quota sul mare) all'istante dello scoppio.
struct Scoppio {
    double tempo = 0.0;
    Esplosione::Tipo tipo = Esplosione::Tipo::siluro;
    double x = 0.0, y = 0.0, z = 0.0;
    double tnt_kg = 0.0;
};

struct Falla {           // verso il mare (scafo squarciato, apertura di coperta, valvola di allagamento)
    int cella = -1;
    double x = 0, y = 0, z = 0, area = 0;   // m, m^2 (coordinate del corpo)
    bool coperta = false;                   // apertura di coperta (non si tampona)
    bool controallagamento = false;
};

struct Apertura {        // fra due celle: perdite di paratia, paratia squarciata, porta aperta
    int a = -1, b = -1;
    double x = 0, y = 0, z = 0, area = 0;
    double cedimento = 1e9;   // m: differenza di livello oltre cui la paratia cede (trasversali)
    bool ceduta = false;
};

struct Cella {
    CellaScafo dati;
    std::vector<int> stazioni;            // indici delle stazioni del corpo
    std::vector<Poligono> poligoni;       // sezione della cella in ogni stazione (y, z)
    double volume = 0.0;                  // geometrico, m^3
    double acqua = 0.0;                   // m^3 di acqua imbarcata (al piu' volume * permeabilita')
    double livello = -1e9;                // quota del pelo libero nel mondo (-1e9: vuota)
    double bx = 0, by = 0, bz = 0;        // baricentro dell'acqua (corpo)
    std::vector<double> acqua_stazioni;   // m^3 per stazione (per la trave)
    double fuoco = 0.0, combustibile = 0.0;   // intensita' 0..1, s di fuoco pieno rimasti
    double allagata_per_incendio = -1.0;  // s: deposito allagato dal controllo danni (in corso)
    double annerimento = 0.0;             // 0-1: fuliggine sullo scafo attorno al locale (cresce col fuoco, non cala)
    double riempimento() const { return volume > 0 ? acqua / (volume * dati.permeabilita) : 0.0; }
};

struct ZonaIncendio {    // sopra coperta (hangar, lanciatori)
    ZonaFuoco dati;
    double fuoco = 0.0, combustibile = 0.0;
    double annerimento = 0.0;
    bool elicottero_perso = false;   // hangar: l'elicottero bruciato (danni critici, fase 3)
};

struct Corpo {
    std::vector<Stazione> stazioni;
    std::vector<Poligono> sezioni;         // sezione chiusa di ogni stazione
    std::vector<double> spinta;            // m^3 immersi per stazione (ultimo passo)
    std::vector<double> massa_stazioni;    // kg della nave per stazione
    double zg = 0.0;                       // quota del baricentro della nave (corpo)
    double capacita_trave = 0.0;           // N*m
    std::vector<Cella> celle;
    std::vector<Falla> falle;
    std::vector<Apertura> aperture;
    Assetto assetto;
    double v_imm = 0, v_sb = 0, v_ap = 0, v_dx = 0;
    bool affondato = false;
    double massa() const;
};

class DanniNave {
public:
    DanniNave(const DatiScafo& dati, std::uint64_t seme);

    void colpo(const Esplosione& e);
    // Urto meccanico di una collisione (nucleo/collisioni.hpp, valuta_urto): falla sotto il galleggiamento nella cella
    // al punto, e nel registro le parti perse (duomo, eliche, timone, sovrastrutture)
    void urto_meccanico(const DanniUrtoUnita& d);
    void aggiungi_falla(const Falla& f, std::size_t corpo = 0) { corpi_.at(corpo).falle.push_back(f); }   // danni a mano
    // Acqua in una cella (m^3, entro il volume per la permeabilita'): per un sottomarino che emerge danneggiato e passa
    // dal modello d'immersione (nucleo/battello.hpp) a questa fisica con l'acqua che ha gia' a bordo
    void imposta_acqua(std::size_t cella, double m3, std::size_t corpo = 0);
    // Lo stesso per il fuoco (intensita' 0-1) e per il carico vivo del deposito (kg di esplosivo e di propellente) del
    // battello che emerge (danni critici, fase 4)
    void imposta_fuoco(std::size_t cella, double intensita, std::size_t corpo = 0);
    void imposta_carico(std::size_t cella, double carica_kg, double propellente_kg, std::size_t corpo = 0);
    // Petroliera in zavorra (danni critici, T7; dossier 07 §5.3): le cisterne vuote non hanno greggio da bruciare o da
    // versare, ma i vapori si accendono col colpo nella zona del carico. Massa e immersione restano quelle della nave
    // carica. Le altre navi non cambiano
    void imposta_zavorra(bool gas_inerte);
    bool in_zavorra() const { return p_vapori_ > 0.0; }
    // Per le scene di prova e le istantanee (melchior688 --colpo deposito|coperta): fa saltare subito il deposito con
    // la carica piu' grande, o il lanciatore in coperta piu' carico; false se la nave non ne ha
    bool esplosione_di_prova(bool coperta);
    void avanza(double dt);                     // s di simulazione

    const std::vector<Corpo>& corpi() const { return corpi_; }
    const std::vector<ZonaIncendio>& zone() const { return zone_; }
    const DatiScafo& dati() const { return dati_; }
    double tempo() const { return tempo_; }
    bool affondata() const;                     // tutti i corpi sotto la superficie
    bool condannata() const { return condannata_; }   // perdita prevista: abbandono nave
    // L'equipaggio abbandona la nave per una decisione esterna (la missione la da' per perduta): stop al controllo danni.
    void abbandona() { if (!condannata_) { condannata_ = true; annota("abbandono nave (ordinato)"); } }
    bool spezzata() const { return corpi_.size() > 1; }
    bool energia() const { return energia_; }
    double acqua_imbarcata_t() const;           // oltre la zavorra iniziale dei sottomarini
    double dislocamento_intatto_t() const { return massa_nave_ / 1000.0; }
    // assetto del corpo principale (quello piu' pesante), gradi e metri
    double sbandamento_gradi() const;
    double appoppamento_gradi() const;
    double immersione_m() const;
    PosaNave posa() const;                      // per il disegno (grafica/modello.hpp): assetti, falle e fuliggine
    const std::vector<std::string>& registro() const { return registro_; }   // eventi con il tempo
    const std::vector<Scoppio>& scoppi() const { return scoppi_; }

private:
    void passo_dinamica(Corpo& c, double h);
    void passo_allagamento(Corpo& c, double h);
    void livelli(Corpo& c);
    void passo_incendi_e_controllo(double h);
    void verifica_trave(std::size_t i);
    void verifica_paratie(Corpo& c);
    void spezza(std::size_t i, std::size_t k, const std::string& causa);
    void esplosione_deposito(std::size_t corpo, int cella);
    void esplosione_in_coperta(ZonaIncendio& zona);   // i missili di un lanciatore in coperta (carica_kg della zona)
    void esplosione_vapori(std::size_t corpo, int cella, double W);   // la cisterna vuota, W kg la testata che l'accende
    void annota(const std::string& s);
    double capacita_iniziale() const;

    DatiScafo dati_;
    Casuale caso_;
    std::vector<Corpo> corpi_;
    std::vector<ZonaIncendio> zone_;
    std::vector<std::string> registro_;
    std::vector<Scoppio> scoppi_;
    double tempo_ = 0.0, accumulo_ = 0.0, accumulo_lento_ = 0.0;
    double massa_nave_ = 0.0, momento_roll_ = 0.0, momento_pitch_ = 0.0;
    double smorz_h_ = 0, smorz_r_ = 0, smorz_p_ = 0;
    bool energia_ = true, condannata_ = false, soffiaggio_ = false;
    double x_rottura_ = 0.0;
    double p_vapori_ = 0.0;   // per colpo nella zona del carico: 0 la nave carica (imposta_zavorra)
    // frusta della bolla sotto la chiglia, per il disegno: istante, punto, ampiezza dell'inarcamento, periodo della bolla
    double frusta_t0_ = -1.0, frusta_x_ = 0.0, frusta_m_ = 0.0, frusta_T1_ = 1.0;
};

}  // namespace m688
