#pragma once
// Il comandante IA del battello del giocatore, per la sandbox (docs/19_sandbox.md §5, V1; nota condivisa
// sandbox-v1-runtime-contratto.md). Comanda col solo `Ordine` del giocatore (nucleo/ordini.hpp), che main mette nel
// RegistroOrdini come i tasti: replay e codice partita vanno come sempre, e nel replay il comandante non serve.
//
// Vede quello che vede il giocatore, mai la verita':
// - i contatti come li mostra la carta (segni_contatti, barra_bersaglio: la classe solo se nota);
// - la posizione stimata del battello (Missione::posizione_stimata, con la navigazione incerta);
// - la stazione armi, gli obiettivi della missione e le ROE ricevute dalla sala radio.
// La parte di un contatto la deduce dalla classe nota (la parte delle unita' della missione di quella classe, come
// l'IA: Missione::lato_creduto); le proprie armi le riconosce.
//
// La condotta, coi moduli dell'IA:
// - transito verso l'area dell'obiettivo e pattuglia con gli sprint and drift;
// - TMA a gambe (nucleo/manovre_tma) col solo rilevamento, con la propria soluzione (nucleo/tma_ia, risolvi_tma); se
//   dopo 5 minuti o due gambe non c'e', un impulso del sonar attivo (al piu' ogni 2 minuti) per la distanza;
// - attacco con la soluzione o la posizione, secondo le ROE: designazione, impostazioni del siluro, lancio;
// - la guida a filo dei suoi siluri con la dottrina dell'IA del suo livello (GuidaFiloIa), coi comandi via filo del
//   giocatore; mentre guida resta sotto i 12 nodi e accosta piano;
// - missili (Harpoon, TASM) sulle navi con la posizione, oltre la portata dei siluri;
// - mine: con l'obiettivo `minare` va nell'area e posa dai tubi con le mine;
// - difesa da un siluro in arrivo: fuga a tutta forza, cambio di quota, esca semovente di traverso ed esca fissa;
// - radio: il rapporto di contatto al primo ostile dell'obiettivo (quota periscopica, antenna), e l'ascolto del
//   broadcast alle finestre;
// - punto nave a quota periscopica con la navigazione incerta, quando la stima e' cresciuta;
// - ritirata se colpito o senza armi.
// Con il piano di una missione originale (V3, missioni/sandbox_originali.hpp) area, condotta e armi vengono dal piano:
// - transito sui punti fino all'uscita, seminando chi pedina (scatto, cambio di quota oltre lo strato, deriva);
// - scorta: la stazione davanti alla rotta pianificata del convoglio (PIM);
// - pedinamento: la TMA sul contatto della classe del piano, poi entro la distanza (con l'impulso del sonar vicino);
// - evasione: scatto e deriva per liberarsi di chi pedina;
// - attacco_terra: i Tomahawk da quota periscopica sul bersaglio dei missili, poi l'uscita;
// - l'attesa sul posto, il rettangolo da cui non uscire, le armi libere, secondo le ROE, solo in difesa o sulle classi.
// Deterministico dal seme. Gli stati passano al registro (evento comandante_ia) con Missione::nota_comandante.

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "missioni/missione.hpp"
#include "missioni/sandbox_originali.hpp"
#include "nucleo/armi_ia.hpp"
#include "nucleo/manovre_tma.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/rotte.hpp"
#include "nucleo/sonarista.hpp"
#include "nucleo/tma_ia.hpp"

namespace m688 {

class StazioniMissione;

struct OpzioniComandante {
    Esperienza livello = Esperienza::medio;   // V1: decide quanti rilevamenti servono prima di fidarsi della soluzione
    std::optional<PianoOriginale> piano;      // V3: le originali (piano_originale); dove c'e' vince sugli obiettivi
};

struct StatoComandante {
    enum class Fase : std::uint8_t {
        transito, pattuglia, tma, attacco, difesa, punto_nave, ritirata, radio, mine,
        scorta, pedinamento, seminare, attesa, attacco_terra   // V3: le condotte del piano delle originali
    };
    Fase fase = Fase::transito;
    std::string obiettivo;   // id dell'obiettivo della missione seguito
    int bersaglio = -1;      // il contatto della TMA o dell'attacco
    int gambe = 0;
    std::string motivo;      // perche' e' in questa fase (per lo spettatore)
};
const char* nome_fase_comandante(StatoComandante::Fase f);

class ComandanteIa {
public:
    ComandanteIa(const Missione& m, OpzioniComandante o, std::uint64_t seme);
    // una volta al secondo di missione (piu' spesso non fa niente): gli ordini da consegnare come quelli del giocatore.
    // La missione non la cambia: le annota solo gli stati per il registro
    std::vector<Ordine> passo(Missione& m, const StazioniMissione& st);
    const StatoComandante& stato() const { return stato_; }
    const std::vector<std::pair<double, StatoComandante>>& storia() const { return storia_; }

private:
    struct Contatto {
        int id = -1;
        double rilevamento = 0.0;
        bool posizione = false;
        Vec2 pos;
        bool ostile = false, sottomarino = false, arma = false, nostra = false, classificato = false;
        std::string classe;
        CategoriaRoe categoria = CategoriaRoe::navi;
    };
    std::vector<Contatto> contatti(const Missione& m) const;
    bool armi_libere(const Missione& m, const Contatto& c) const;
    void cambia(double t, StatoComandante::Fase f, const std::string& motivo, Missione& m);
    void rotta(std::vector<Ordine>& v, double t, double rotta_deg, double velocita_kn, const UnitaClassica& g);
    void quota(std::vector<Ordine>& v, double t, double quota_m);
    void guida_fili(const Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const std::vector<Contatto>& cc);
    bool radio(Missione& m, std::vector<Ordine>& v, double t, const UnitaClassica& g);   // vero: occupa il passo
    // la TMA sul contatto: i rilevamenti e le soluzioni; `dove` la posizione (del contatto o della soluzione). Vero se ha
    // dato gli ordini di una gamba (la posizione non c'e' ancora): il passo e' occupato
    bool tma(Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const Contatto& c, const UnitaClassica& g, bool ping,
             std::optional<Vec2>& dove);
    // V3: la condotta del piano dopo attacchi, punto nave e mine; vero se occupa il passo
    bool condotta(Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const std::vector<Contatto>& cc, const UnitaClassica& g);
    bool attacco_terra(Missione& m, std::vector<Ordine>& v, double t, const std::vector<Contatto>& cc, const UnitaClassica& g,
                       const StazioniMissione& st);
    bool semina(Missione& m, std::vector<Ordine>& v, double t, const std::vector<Contatto>& cc, const UnitaClassica& g, bool sempre,
                std::optional<double> lungo_rotta);
    Vec2 posizione_pim(double t, double avanti_m, double* rotta = nullptr) const;
    bool della_lista(const Contatto& c) const;
    double quota_massima() const;   // la quota piu' profonda con la chiglia sopra il fondo
    Vec2 verso(const Missione& m, double t, Vec2 io, Vec2 meta);   // la prossima tappa per acqua navigabile

    OpzioniComandante o_;
    Casuale caso_;
    StatoComandante stato_;
    std::vector<std::pair<double, StatoComandante>> storia_;
    int lato_ = 0x08;
    Vec2 partenza_;
    std::optional<Vec2> area_;
    double raggio_area_m_ = 0.0;
    std::vector<std::string> classi_obiettivo_;
    double prossimo_ = 0.0, prossima_rotta_ = 0.0, fine_fase_ = 0.0;
    double rotta_data_ = -1.0, velocita_data_ = -1.0, quota_data_ = -1.0;
    int gamba_pattuglia_ = 0;
    std::map<int, Tma> tma_;
    std::map<int, double> ultimo_ril_, ultima_soluzione_;
    std::map<int, StimaTma> stime_;
    std::map<int, int> lanci_su_;
    std::optional<ManovreTma> manovre_;
    double ultimo_lancio_ = -1e18, inizio_tma_ = -1.0, ultimo_ping_ = -1e18;
    std::set<int> minacce_, fuggite_;           // le armi nemiche viste; quelle da cui e' gia' fuggito
    std::map<int, GuidaFiloIa> fili_;           // i suoi siluri a filo
    std::size_t eventi_siluri_visti_ = 0;
    VincoliFilo vincoli_;                        // di questo passo
    std::set<int> riferiti_;                     // i contatti gia' riferiti per radio
    std::optional<int> da_riferire_;
    double ultimo_rapporto_ = -1e18;
    bool minare_ = false, ascolto_ = false, missile_lanciato_ = false;
    int mine_posate_ = 0;
    std::map<int, double> ricarica_chiesta_;     // tubo -> quando e' stata chiesta l'ultima ricarica
    bool colpito_ = false, iniziato_ = false, antenna_su_ = false;
    std::size_t colpi_visti_ = 0;
    // V3, il piano delle originali
    double quota_crociera_ = 120.0, velocita_crociera_ = 0.0;
    Vec2 io_;
    const DinamicaSottomarino* dinamica_ = nullptr;   // del battello, nell'Enhanced (di questo passo)
    bool dinamica_vista_ = false;
    double ultimo_periscopio_ = -1e18, quota_voluta_ = -1.0, fondo_ = 0.0, rotta_deriva_ = 0.0;
    double pescaggio_ = 8.0, chiglia_periscopica_ = 18.3;
    const Missione* missione_ = nullptr;   // di questo passo, per le secche lungo la rotta
    int lato_secche_ = 0;
    std::shared_ptr<const AcqueNavigabili> acque_;   // per la chiglia in superficie, una volta
    std::vector<Vec2> percorso_;
    std::optional<Vec2> meta_rotta_;
    double ripiano_ = 0.0;
    double ultima_antenna_ = -1e18, ultimo_ascolto_ = 0.0;   // il primo ascolto dopo ASCOLTO_OGNI_S
    std::size_t punto_ = 0;                   // il prossimo punto del transito
    bool verso_uscita_ = false, uscito_ = false, missili_terra_finiti_ = false, ping_fatto_ = false;
    double inizio_terra_ = -1.0, fine_scatto_ = -1.0, fine_deriva_ = -1.0, prossimo_scatto_ = 0.0, rotta_scatto_ = 0.0;
    int designato_ = -1;                      // pedinamento: il contatto designato
    std::optional<Vec2> ultima_vista_;        // pedinamento: l'ultima posizione stimata del contatto
};

}  // namespace m688
