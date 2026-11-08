#pragma once
// Il pannello degli interventi del «vetrino di Dio» (docs/28_vetrino_dio.md §6, roadmap 36): nel vetrino dal vivo lo
// spettatore sceglie il gruppo, l'intervento, l'unita' o il punto sulla carta e i parametri, e il pannello restituisce
// la richiesta con la sua riga di testo. Chi lo usa (la vista dal vivo) la passa al chiamante, che la applica alla
// missione prima del passo. Il catalogo viene dal motore (missioni/interventi.hpp, il contratto della D1 col runtime:
// nota condivisa runtime-interventi-contratto.md), tradotto nelle voci del pannello da catalogo_pannello.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "missioni/interventi.hpp"
#include "nucleo/geometria.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

// Un parametro di un intervento: il valore va da `minimo` a `massimo` a passi di `passo`
struct ParametroPannello {
    std::string nome;          // stabile (va nella richiesta)
    std::string testo;         // per lo spettatore, in maiuscolo
    double minimo = 0.0, massimo = 1.0, passo = 1.0, valore = 0.0;
    std::string unita;         // «NODI», «M», «%»; vuoto: nessuna
    std::vector<std::string> scelte;   // non vuoto: un elenco di voci (il valore e' l'indice)
    std::vector<std::string> etichette;   // come si leggono le scelte (vuoto: le scelte in maiuscolo)
    bool come_testo = false;   // la scelta va nei testi della richiesta (classe, parte, sensore), non nei numeri
    bool facoltativo = false;  // un valore sotto il minimo: «INVARIATO», fuori dalla richiesta (la scelta vuota: «NESSUNO»)
};

// Una voce del catalogo
struct VocePannello {
    std::string gruppo;        // FORZE, GUASTI, AMBIENTE, COMUNICAZIONI, INGANNO
    std::string tipo;          // stabile: unita_nuova, togli, ordine, sensore...
    std::string testo;         // per lo spettatore
    bool vuole_unita = false, vuole_punto = false;
    std::vector<ParametroPannello> parametri;
    bool vuole_raggio = false;     // il parametro «raggio_m» va nel raggio della richiesta
    bool ha_durata = true;         // falso: niente durata (togliere un'unita')
    std::vector<std::string> categorie;   // le unita' che si possono scegliere (vuoto: tutte)
    bool punto_facoltativo = false;       // il punto si puo' saltare con Invio (il «vai a» dell'ordine)
    bool vuole_bersaglio = false;         // il parametro «bersaglio» del motore (un id): un passo con la lista delle unita'
};

// Un'unita' che si puo' scegliere, come la vede lo spettatore (la verita')
struct UnitaPannello {
    int id = -1;
    std::string nome;          // «SPRUANCE #3»
    std::string parte;         // NATO, PATTO, NEUTRALI
    std::string categoria;     // nave, sottomarino, aereo
};

// Quello che lo spettatore ha chiesto
struct RichiestaIntervento {
    std::string tipo;
    int unita = -1;
    std::optional<Vec2> punto;
    double raggio_m = 0.0;     // 0: tutto il teatro
    std::map<std::string, double> parametri;
    std::map<std::string, std::string> testi;   // classe, parte, comportamento, sensore
    double durata_s = 0.0;     // 0: per sempre
    double tempo = 0.0;        // l'istante della partita in cui va applicato
    bool subito = true;        // «fra 0 minuti»: al prossimo passo (Intervento::tempo -1)
    std::string riga;          // «41:00 SONAR DI SPRUANCE #3 A 50% PER 20 MIN»
};

// Il catalogo del motore nelle voci del pannello: i testi in maiuscolo, i testi liberi senza scelte saltati (il
// gestore), la classe con le scelte di `classi` (chiave, nome); vuole_raggio aggiunge il parametro «raggio_m»; il
// parametro «bersaglio» (la false flag) diventa un passo con la lista delle unita'
std::vector<VocePannello> catalogo_pannello(const std::vector<VoceCatalogoIntervento>& c,
                                            const std::vector<std::pair<std::string, std::string>>& classi);
// La richiesta come intervento per Missione::intervieni (la riga la compone la missione)
Intervento intervento_di(const RichiestaIntervento& q);

class PannelloInterventi {
public:
    explicit PannelloInterventi(std::vector<VocePannello> catalogo) : catalogo_(std::move(catalogo)) {}

    void apri(double tempo_partita, std::vector<UnitaPannello> unita);
    void chiudi() { aperto_ = false; }
    bool aperto() const { return aperto_; }

    // il punto sulla carta, quando il pannello lo aspetta (un clic: lo passa la vista)
    bool aspetta_punto() const { return aperto_ && passo_ == Passo::punto; }
    void punto_scelto(Vec2 p);
    // falso: nel passo del punto il pannello non scrive «CLIC SULLA CARTA» (lo scrive chi ha la carta, come la pagina
    // degli interventi programmati sulla sua cartina)
    void istruzione_punto(bool si) { istruzione_punto_ = si; }

    // Tab e Maiuscolo+Tab scorrono le voci, Invio sceglie o conferma (sul punto facoltativo: lo salta), Esc torna
    // indietro (dal primo passo chiude), le frecce destra e sinistra cambiano il parametro scelto. Restituisce la
    // richiesta alla conferma
    std::optional<RichiestaIntervento> comandi(const Comandi& c, double dt);

    // il pannello nel rettangolo r (sopra la carta della vista dal vivo)
    void disegna(Tela& t, const Font& f, Rettangolo r) const;

    // la riga di testo di una richiesta (per il registro dello spettatore e la linea del tempo)
    std::string riga(const RichiestaIntervento& q) const;

private:
    enum class Passo { gruppo, tipo, unita, bersaglio, punto, parametri };

    std::vector<std::string> gruppi() const;
    std::vector<int> tipi_del_gruppo() const;   // indici nel catalogo
    const VocePannello& tipo_scelto() const { return catalogo_[static_cast<std::size_t>(tipo_)]; }
    std::vector<int> unita_adatte() const;      // indici in unita_, secondo le categorie del tipo
    std::vector<int> bersagli() const;          // indici in unita_: tutte, meno l'unita' scelta
    int voci() const;                           // quante voci ha il passo corrente
    Passo dopo(Passo p) const;                  // il passo seguente per il tipo scelto (unita', punto, parametri)
    RichiestaIntervento richiesta() const;

    std::vector<VocePannello> catalogo_;
    std::vector<UnitaPannello> unita_;
    bool aperto_ = false;
    Passo passo_ = Passo::gruppo;
    int cursore_ = 0;
    std::string gruppo_;
    int tipo_ = 0, unita_scelta_ = -1, bersaglio_scelto_ = -1;
    std::optional<Vec2> punto_;
    std::vector<ParametroPannello> valori_;   // i parametri del tipo, coi valori scelti, piu' durata e «fra»
    double tempo_ = 0.0, ripeti_ = 0.0;
    bool istruzione_punto_ = true;
};

}  // namespace m688
