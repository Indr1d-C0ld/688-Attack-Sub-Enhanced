#pragma once
// Strumenti di plottaggio della carta (docs/11_comandi_e_stazioni.md §11, pezzi N2-N3; ricerca in
// docs/ricerca/12_strumenti_plotting_e_carta.md §2.4 e §5.2): geometria pura e lo stato del «lucido» sopra la carta.
// Ogni strumento ha la forma 1989 (letture arrotondate alla tacca della scala, solo cio' che il plotter segna) e la
// moderna (letture esatte, linee continue, eventi automatici). E' solo interfaccia: non cambia la simulazione.

#include <optional>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

// --- geometria ---

// Distanza, rilevamento vero da a verso b, e il tempo per percorrerla alla velocita' data (0: niente tempo)
struct Misura {
    double distanza_m = 0.0, rilevamento = 0.0, tempo_s = 0.0;
};
Misura misura(Vec2 a, Vec2 b, double velocita_m_s = 0.0);

// Punto di minima distanza fra due moti rettilinei uniformi (posizione e velocita' in m e m/s): distanza al CPA,
// tempo al CPA (negativo se e' gia' passato), dove si trova ciascuno e il rilevamento dal primo al secondo al CPA
struct Cpa {
    double distanza_m = 0.0, tempo_s = 0.0, rilevamento = 0.0;
    Vec2 proprio, altro;
};
Cpa cpa(Vec2 p0, Vec2 v0, Vec2 p1, Vec2 v1);

// Velocita' come vettore (m/s) dalla rotta (gradi veri) e dal modulo
Vec2 vettore(double rotta, double m_s);

// Letture del modo 1989: la distanza arrotondata alla tacca della scala in uso (0,1 nm da vicino, 0,5 nm da lontano),
// il rilevamento al grado, i minuti interi
double distanza_1989(double metri, int scala);
double rilevamento_1989(double gradi);

// Testo di una misura come la scrive il plotter: «247°  6,3 NM  31 MIN A 12 KN» (il tempo solo con la velocita')
std::string testo_misura(const Misura& m, double velocita_kn, bool anni_89, int scala);

// Punto dentro un poligono (regola dei raggi; i vertici in ordine, chiuso implicitamente)
bool dentro_poligono(const std::vector<Vec2>& vertici, Vec2 p);

// Raggio del cerchio di massimo avanzamento di un datum: quanto puo' aver fatto un bersaglio alla velocita' massima
double raggio_datum(double eta_s, double velocita_max_m_s);

// --- il lucido: cio' che la squadra di plottaggio ha segnato ---

struct Tratto {                       // righello fissato con Invio: un tratto di matita con la sua etichetta
    Vec2 a, b;
    std::string etichetta;
};
struct LineaRilevamento {             // dalla posizione del battello all'istante della misura
    Vec2 origine;
    double rilevamento = 0.0, tempo = 0.0;
    std::string fonte;                // "S2" (tracker del sonar) o la lettera del bersaglio
    int colore = 0;                   // 0-5: le matite
};
struct Evento {                       // MARK: ora e posizione, con un tipo
    enum class Tipo : unsigned char { segno, transitorio, avvistamento, lancio, datum, nota };
    int numero = 0;
    Tipo tipo = Tipo::segno;
    Vec2 pos;
    double tempo = 0.0;
    std::string testo;
};
struct Datum {                        // contatto perso: dove puo' essere ora
    Vec2 pos;
    double tempo = 0.0, velocita_max_m_s = 0.0;
    std::string fonte;
};
const char* nome_evento(Evento::Tipo t);

// Simbolo sul lucido (N2): forme APP-6/NTDS per affiliazione e dominio (la meta' inferiore del telaio per il
// subacqueo, la superiore per l'aereo), una nota sola o una croce di matita; con una scritta di 24 caratteri al massimo
struct Segno {
    enum class Affiliazione : unsigned char { ostile, ignoto, amico, neutrale };
    enum class Dominio : unsigned char { subacqueo, superficie, aereo, nota, croce };
    Vec2 pos;
    Affiliazione affiliazione = Affiliazione::ignoto;
    Dominio dominio = Dominio::subacqueo;
    std::string testo;
    double tempo = 0.0;
};
const char* nome_affiliazione(Segno::Affiliazione a);
const char* nome_dominio(Segno::Dominio d);

// Zona dell'utente (N4): un cerchio da evitare (esclusione) o un'area d'interesse
struct ZonaUtente {
    Vec2 centro;
    double raggio_m = MIGLIO;
    bool esclusione = true;
};

// Regolo di Ekelund sul plot geografico (N3, docs/ricerca/12 §5.2 punto 9): il bersaglio sta sull'ultima linea a
// `distanza_m` dall'origine, con rotta e velocita' stimate; all'istante di ogni linea precedente doveva trovarsi
// v (t0 - tk) indietro sulla rotta: quelle sono le tacche. Lo scarto e' quanto le tacche cadono fuori dalle loro linee
// (gradi, media quadratica). `linee`: della stessa fonte, in ordine di tempo; servono almeno due linee.
struct Ekelund {
    std::vector<Vec2> tacche;          // una per linea, l'ultima e' il punto sull'ultima linea
    double scarto_gradi = 0.0;
};
Ekelund regolo_ekelund(const std::vector<LineaRilevamento>& linee, double distanza_m, double rotta, double velocita_m_s);

// Rateo di rilevamento ai minimi quadrati (gradi al minuto, positivo in senso orario) e rilevamento al tempo `t`, sulle
// linee date (dal plot tempo-rilevamento); vuoto con meno di due punti
struct RettaRilevamento {
    double rateo_gradi_min = 0.0, rilevamento = 0.0;
};
std::optional<RettaRilevamento> retta_rilevamento(const std::vector<LineaRilevamento>& linee, double t);

class LucidoCarta {
public:
    static constexpr int MAX_LINEE_PER_FONTE = 12, MAX_EVENTI = 60, MAX_TRATTI = 30, MAX_DATUM = 8;

    void aggiungi_tratto(Tratto t);
    void togli_tratto() { if (!tratti_.empty()) tratti_.pop_back(); }
    // una linea di rilevamento; per ogni fonte restano le ultime MAX_LINEE_PER_FONTE
    void aggiungi_linea(LineaRilevamento l);
    // l'ultima linea della fonte, se c'e' (per sapere quando segnarne un'altra)
    const LineaRilevamento* ultima_linea(const std::string& fonte) const;
    int evento(Evento::Tipo tipo, Vec2 pos, double tempo, std::string testo = {});   // il numero dell'evento
    void togli_evento() { if (!eventi_.empty()) eventi_.pop_back(); }
    void aggiungi_datum(Datum d);
    // simboli, note e zone dell'utente
    void aggiungi_segno(Segno s);
    void togli_segno() { if (!segni_.empty()) segni_.pop_back(); }
    Segno* ultimo_segno() { return segni_.empty() ? nullptr : &segni_.back(); }
    void aggiungi_zona(ZonaUtente z);
    void togli_zona() { if (!zone_.empty()) zone_.pop_back(); }
    // le linee di una fonte, in ordine di tempo
    std::vector<LineaRilevamento> linee_di(const std::string& fonte) const;
    void pulisci();                   // via tratti, linee, eventi, datum, simboli e zone

    const std::vector<Tratto>& tratti() const { return tratti_; }
    const std::vector<LineaRilevamento>& linee() const { return linee_; }
    const std::vector<Evento>& eventi() const { return eventi_; }
    const std::vector<Datum>& datum() const { return datum_; }
    const std::vector<Segno>& segni() const { return segni_; }
    const std::vector<ZonaUtente>& zone() const { return zone_; }
    static constexpr int MAX_SEGNI = 40, MAX_ZONE = 12, MAX_TESTO = 24;

private:
    std::vector<Tratto> tratti_;
    std::vector<LineaRilevamento> linee_;
    std::vector<Evento> eventi_;
    std::vector<Datum> datum_;
    std::vector<Segno> segni_;
    std::vector<ZonaUtente> zone_;
    int prossimo_evento_ = 1;
};

}  // namespace m688
