#pragma once
// Sottomarini colpiti nelle missioni, regola di simulazione (Enhanced): fase D6c di
// docs/10_danni_affondamento_esplosioni.md §10 (decisioni 1, 2, 5, 8, 9; divisione del lavoro nel §10.5).
//
// Ogni sottomarino colpito (o che esaurisce le casse) riceve il suo BattelloDanni (nucleo/battello.hpp):
// - i colpi della regola d'impatto dell'originale (Missione::colpi()) diventano un urto: Mk 48 d'influenza sotto lo
//   scafo, siluri da 533 mm a contatto sul fianco, cariche e razzi a qualche decina di metri, missili solo su un
//   battello in superficie;
// - a ogni passo il peso netto dell'acqua imbarcata e dell'aria nelle casse diventa il rateo di quota dell'unita'
//   (rateo_quota(id): rateo_verticale_regime con i piani nel verso della quota ordinata) e i limiti dei sistemi la
//   velocita' massima (OrdineMotore::velocita_max_kn). Il rateo di quota entra nel moto di Missione con
//   l'aggancio del punto 3 del §10.5 (turno di missione.cpp concordato fra le sessioni);
// - Ctrl+E del giocatore (esaurisci(id)) e' l'esaurimento d'emergenza delle casse; l'IA lo tenta dopo un
//   allagamento sopra i 150 m nel 30-70% dei casi (dossier 07 §8.4);
// - il collasso (BattelloDanni::perso) distrugge l'unita' (Missione::distruggi);
// - messaggi del motore 270-289: rapporti dell'equipaggio del giocatore; rumori di rottura, implosioni ed
//   emersioni d'emergenza dei sottomarini nemici entro 20 nm.
// - esiti (fase D6e, dossier 07 §8): perso per collasso, con tutti i compartimenti allagati, o posato sul fondo
//   senza poter risalire (10 minuti pesante sul fondo con l'aria finita); emerso danneggiato: fermo in superficie
//   (al piu' 3 nodi);
//   fuori combattimento quando l'integrita' dell'originale e' finita e il battello e' menomato (emerso danneggiato,
//   propulsione sotto il 30% o allagato), cosi' gli script non lo bersagliano all'infinito.
// - segni in superficie (dossier 07 §8.3) dove il battello si perde: rottura profonda (oltre 300 m) -> nafta e
//   pochi rottami dopo 10-60 minuti; rottura vicina alla superficie o sul fondo -> aria, nafta e rottami abbondanti
//   subito. Gasolio dei diesel: chiazza piu' grande e piu' lunga. Il disegno lo fanno gli effetti dell'app.
// - battello emerso danneggiato: passa alla fisica delle navi (nucleo/danni.hpp, DanniNave) con l'acqua che ha a
//   bordo, compartimenti e casse per nome, e le sue falle sul fondo delle celle: puo' sbandare, appruarsi, capovolgersi
//   e affondare in superficie. Resta in superficie (non si immerge piu'); se affonda e' perso. sincronizza() passa la
//   posa al disegno (sprite dal modello 3D con le appendici di poppa).
// Le stazioni leggono sistemi(id) e limiti(id). Il livello classico non lo usa: restano i guasti dell'originale.

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/artiglieria.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/collisioni.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

// Messaggi del motore per i sottomarini (testi in tools/pipeline/esporta_motore.py, MESSAGGI_MOTORE)
namespace msg_sottomarino {
constexpr int ALLAGAMENTO = 270, GUASTO = 271, SCRAM = 272, REATTORE = 273, RIPARATO = 274, TAMPONATA = 275,
              PARATIA = 276, ESAURIMENTO = 277, ARIA_FINITA = 278, ROTTURA = 279, RUMORI_ROTTURA = 280,
              IMPLOSIONI = 281, FERMI_SUL_FONDO = 282, EMERSIONE_NEMICA = 283, INCENDIO = 284, ESPLOSIONE = 285,
              INCENDIO_SPENTO = 286, CLORO = 287, ARIA_NELL_INCENDIO = 288, ESPLOSIONE_NEMICA = 289;
}

// Emersione violenta di un sottomarino dell'IA che esaurisce le casse (E1, docs/11 §11; docs/10 §10.14): la prua esce
// dall'acqua con un angolo e per una parte della lunghezza che dipendono dalla velocita' di risalita, dalla velocita'
// e dall'assetto all'arrivo, poi ricade con un colpo, beccheggia, rolla e si assesta (in tutto circa 40 s).
struct EmersioneViolenta {
    int unita = -1;
    double inizio = 0.0;           // s di missione: arrivo in superficie
    Vec2 pos;                      // centro del battello
    double rotta = 0.0;            // gradi bussola
    double velocita = 0.0;         // m/s sull'acqua
    double salita = 0.0;           // m/s verso l'alto all'arrivo
    double angolo = 0.0;           // gradi: assetto della prua al culmine
    double fuori = 0.0;            // frazione della lunghezza fuori dall'acqua al culmine
    double lunghezza = 0.0;        // m
    double salita_s = 0.0, caduta_s = 0.0;   // durata della salita della prua e della sua ricaduta
    double ricaduta() const { return inizio + salita_s + caduta_s; }   // il colpo della prua sul mare
    double fine() const { return ricaduta() + 30.0; }
    double altezza_prua() const;   // m: quota della prua al culmine
};
// Posa del battello al tempo t (s di missione) durante l'emersione violenta
PosaNave posa_emersione(const EmersioneViolenta& e, double t);

// Artiglieria sul battello (G3, docs/16_artiglieria.md; dossier 22 §5.3 e §9.5): le teste dei periscopi, gli alberi, la
// falsatorre, la plancia e lo scafo colpiti dai colpi diretti e dalle schegge entro r_eff (Missione::impatti_artiglieria).
enum class GuastoTesta : std::uint8_t { intatta, degradata, bloccata, cieca };
const char* nome_guasto_testa(GuastoTesta g);
struct TestaColpita {
    GuastoTesta guasto = GuastoTesta::intatta;   // 22 §5.3: vetro rotto (cieca, 50%), testa piegata (bloccata, 30%),
                                                 // graffi e schegge (degradata, 20%); col colpo diretto
    double incrinato = 0.0, spento = 0.0;        // per l'oculare (ParametriOculare, G4): vetro e immagine
    std::uint32_t seme = 0;                      // le crepe dal punto d'impatto
    double tempo = -1.0;                         // s di missione del colpo (< 0: mai colpita)
    bool allaga_canna = false;                   // vetro rotto: la canna si allaga piano, va abbassata e chiusa
};
struct DanniArtiglieriaBattello {
    TestaColpita attacco, ricerca;               // periscopio d'attacco e di ricerca (Type 18: con l'ESM nella testa)
    bool esm = true, radio = true, radar = true, snorkel = true;   // alberi in funzione
    bool timoni_falsatorre_bloccati = false;     // 688 Flight I e Sturgeon: i timoni della falsatorre
    int feriti_plancia = 0;                      // vedette e ufficiale di guardia (in superficie)
    double plancia_colpita = -1.0;               // s di missione dell'ultimo colpo sulla plancia: il comandante non ha
                                                 // piu' la vista diretta finche' non si scende
    bool non_si_immerge = false;                 // un 16" nello scafo (22 §9.5): falla certa, il battello resta su
    int casse_squarciate = 0;                    // casse zavorra a prua e a poppa squarciate dai colpi (kit, 31/10/2026)
    int colpi = 0, schegge = 0;                  // colpi diretti e scoppi vicini sul battello
    double ultimo = -1.0;                        // s di missione dell'ultimo
};

// Ultima scossa di un battello (D6d: luci che tremolano e vista che sobbalza; la decadenza la fa l'app)
struct ScossaBattello {
    double tempo = -1.0;       // s di missione; < 0: nessuna scossa
    double intensita = 0.0;    // fattore d'urto efficace (0,05 scossa lieve ... oltre 0,6 rottura dello scafo)
};

// Segno in superficie di un sottomarino perso (dossier 07 §8.3), per gli effetti dell'app
struct SegnoSuperficie {
    enum class Tipo { aria, nafta, rottami };
    Tipo tipo = Tipo::nafta;
    int unita = -1;
    Vec2 pos;
    double inizio = 0.0, durata = 0.0;   // s di missione
    double intensita = 0.0;              // 0..1: estensione della chiazza, quantita' di rottami o di bolle
};

struct SottomarinoColpito {
    int unita = -1;
    std::string classe;
    std::unique_ptr<BattelloDanni> danni;
    std::size_t eventi_visti = 0;
    bool quota_fisica = false;        // il peso netto conta: la quota la decide la fisica
    double rateo_quota = 0.0;         // m/s, positivo verso il basso (vale con quota_fisica)
    bool distrutto = false, rotto_annunciato = false, implosioni_annunciate = false, emersione_tentata = false;
    int lanciatore = -1;
    // esiti (D6e)
    double sul_fondo_pesante = 0.0;   // s passati sul fondo con il battello pesante
    std::unique_ptr<DanniNave> superficie;   // emerso danneggiato: la fisica delle navi
    std::size_t falle_passate = 0;           // falle del modello d'immersione gia' passate alla fisica delle navi
    double incendio_forte = 0.0;             // s con un incendio oltre 0,6 (l'IA emerge dopo 5 minuti)
    double quota_prec = -1.0, salita = 0.0;  // quota del passo prima e velocita' di risalita (m/s, media mobile)
    int emersione = -1;                      // indice in emersioni(), se il battello e' emerso violentemente
    Vec2 pos;                         // ultima posizione e quota (i segni restano li')
    double quota = 0.0;
    bool emerso_danneggiato = false, segni_fatti = false, fondo_annunciato = false;
    double rumore_urto_db = 0.0;      // rumore irradiato in piu' dalle collisioni (eliche, piastrelle; al piu' +12)
    // le casse dell'Alfa del giocatore (dossier 19): le armi gia' pesate e lo stato della lampada e della finestrella
    bool casse_alfa = false, tan_prec = false, guasto_casse_prec = false;
    std::set<int> lanci_pesati;
    std::size_t mine_pesate = 0;
    double carico_dato = -1.0;        // kg del carico vivo dato l'ultima volta (< 0: mai): un deposito saltato resta vuoto
};

// I kg d'esplosivo delle armi a bordo, nel locale siluri (danni critici, fase 3): le testate dei siluri e delle mine, dei
// missili l'esplosivo_kg di missili.toml per l'anno (lo stesso del danno del colpo), le esche niente; un missile che
// missili.toml non ha (o senza il catalogo) la frazione d'alto esplosivo del kit, 0,45 della testata (STIMA). Il giocatore:
// i tubi non vuoti (non i pozzi verticali, fuori dallo scafo resistente) e la rastrelliera della stazione Armi; l'IA: tubi
// e riserva del suo carico, meno quello che sta nei pozzi della classe
class StazioneArmi;
struct CaricoIa;
struct CatalogoCarico;
struct CatalogoMissili;
double esplosivo_arma(const CatalogoCarico& cat, const std::string& variante, const CatalogoMissili* missili = nullptr, int anno = 1989);
double esplosivo_a_bordo(const StazioneArmi& armi, const CatalogoCarico& cat, const CatalogoMissili* missili = nullptr, int anno = 1989);
double esplosivo_a_bordo(const CaricoIa& carico, const CatalogoCarico& cat, const std::string& classe,
                         const CatalogoMissili* missili = nullptr, int anno = 1989);

class DanniSottomarini {
public:
    // `classi` e `cartella_dati` per gli scafi (ClasseNave::scafo); il seme rende ripetibili colpi e danni
    DanniSottomarini(const std::map<std::string, ClasseNave>& classi, std::string cartella_dati, std::uint64_t seme);

    // Dopo Missione::avanza: colpi nuovi sui sottomarini, fisica, quota e limiti dei battelli colpiti, messaggi
    void avanza(Missione& m, double dt);
    // Esaurimento d'emergenza delle casse (Ctrl+E del giocatore nell'Enhanced), anche a battello intatto
    void esaurisci(const Missione& m, int unita);
    // Rateo di quota dalla fisica (m/s, positivo verso il basso), o nullopt se la quota la decidono ancora barra e script
    std::optional<double> rateo_quota(int unita) const;

    const std::vector<std::unique_ptr<SottomarinoColpito>>& sottomarini() const { return sub_; }
    const SottomarinoColpito* sottomarino(int unita) const;
    // Per le stazioni: sistemi del battello (nullptr se non e' mai stato colpito: tutto al 100%) e limiti
    const std::vector<StatoSistema>* sistemi(int unita) const;
    LimitiBattello limiti(int unita) const;
    const std::vector<Messaggio>& messaggi() const { return messaggi_; }
    const std::vector<SegnoSuperficie>& segni() const { return segni_; }   // anche futuri: inizio > tempo attuale
    // Emersioni violente (E1), in ordine di tempo: l'app ne fa gli spruzzi (Effetti::emersione_violenta)
    const std::vector<EmersioneViolenta>& emersioni() const { return emersioni_; }
    // Nel mondo da disegnare (dopo Missione::sincronizza): la posa dei battelli emersi danneggiati
    void sincronizza(Mondo& mondo) const;
    // Per la stazione Stato e danni (D6d):
    // - prepara: crea il modello di un battello intatto (di solito il giocatore all'avvio), cosi' compartimenti e
    //   sistemi si possono mostrare prima del primo colpo; a battello intatto non cambia nulla nel moto;
    // - rapporti: gli eventi del battello in ordine di tempo (testo gia' in italiano, sistema e compartimento come
    //   indici di Sistema e di compartimenti(id)); vuoto se il battello non ha modello;
    // - scossa: l'ultima scossa (istante e fattore d'urto efficace);
    // - compartimenti: nome, uso, x0 e x1 lungo lo scafo (coordinate del kit, x verso prua), volume e acqua (m^3),
    //   imploso; nullptr se il battello non ha modello.
    void prepara(const Missione& m, int unita);
    // Urto di prova su un battello (istantanee e prove dell'interfaccia: es. il giocatore colpito all'avvio), come un
    // colpo della missione: TNT per l'urto, distanza dallo scafo, angolo d'incidenza, punto lungo lo scafo dal seme.
    void urto(const Missione& m, int unita, double tnt_kg, double distanza_m, double angolo_rad);
    // Collisione (nucleo/collisioni.hpp): i danni dell'esito ai sottomarini con la fisica fra le due unita' dell'urto
    // (la nave che urta un sottomarino la fa DanniNave / DanniMissione: qui solo i sottomarini). Crea il modello se
    // il battello era intatto; se e' emerso danneggiato la falla va alla fisica delle navi.
    void collisione(const Missione& m, const Urto& u, const EsitoUrto& e);
    double rumore_urto_db(int unita) const;   // 0 se mai urtato
    // Artiglieria (G3): i colpi diretti e le schegge sul battello arrivano da Missione::impatti_artiglieria() dentro
    // avanza; qui lo stato delle teste e degli alberi (nullptr se mai colpito) per la vista (G4: oculare spento e
    // incrinato) e per i messaggi (G6). colpo_artiglieria applica un impatto (prove e istantanee).
    const DanniArtiglieriaBattello* artiglieria(int unita) const;
    void colpo_artiglieria(const Missione& m, const ImpattoArtiglieria& i);
    // Casse d'assetto (posarsi sul fondo, docs/11 §6): appesantisce il battello di `tonnellate` (le imbarca in circa un
    // minuto ogni 3 t), alleggerisci le scarica con le pompe. Il peso passa alla dinamica con OrdineMotore.
    void appesantisci(const Missione& m, int unita, double tonnellate);
    void alleggerisci(int unita);
    // Incendio ed esplosione interna di prova (scenari e verifica storica; docs/10 §10.13)
    void incendio(const Missione& m, int unita, int compartimento, double intensita, bool aria = false);
    void esplosione_interna(const Missione& m, int unita, int compartimento, double tnt_kg);
    const std::vector<EventoBattello>& rapporti(int unita) const;
    ScossaBattello scossa(int unita) const;
    const std::vector<CompartimentoBattello>* compartimenti(int unita) const;
    // fuori combattimento (come DanniMissione::menomata per le navi): emerso danneggiato, propulsione o allagamento
    static bool menomato(const SottomarinoColpito& s);
    // L'Alfa del giocatore (dossier 19 §3.1-3.2 e §6.2; solo Enhanced, classe alfa): gli ordini sfoga, compensa (a =
    // m^3, + imbarca, - espelle) e tan (n = 1 inserito, 0 disinserito) alle sue casse (BattelloDanni). Falso se
    // l'ordine non e' di questi, se il battello non e' un Alfa, o se non si puo': Tan con le pompe sotto il 50%,
    // compensa con Tan inserito (compensa lui). Le armi lanciate alleggeriscono la prua (ARMA_KG) e lo scafo si
    // comprime con la quota (COMPRESSIONE_ALFA_T_100M). Messaggi: 443 Tan inserito, 444 disinserito, 445 fuori servizio,
    // 446 avaria alle casse (la finestrella si accende), 449 sfoghi aperti.
    bool consegna(const Missione& m, const Ordine& o);
    bool tan(int unita) const;
    bool guasto_casse(int unita) const;
    // Il carico vivo dei depositi (danni critici, fase 3): kg d'esplosivo delle armi a bordo (esplosivo_a_bordo), divisi
    // fra i locali siluri come la carica dello scafo; il deposito lo segue, tranne quello saltato (vuoto mentre il carico
    // dato non lo era), che resta vuoto. Senza modello del battello niente: lo prende quando nasce. L'IA lo riceve da
    // avanza (Missione::esplosivo_ia), il giocatore dalle stazioni (StazioniMissione::dopo_passo)
    void carico_vivo(int unita, double kg);

private:
    void applica_carico(SottomarinoColpito& s, double kg);
    bool ha_fisica(const UnitaClassica& u) const;
    SottomarinoColpito& prendi(const UnitaClassica& u, double tempo);
    void colpisci(SottomarinoColpito& s, const UnitaClassica& u, const Colpo& c);
    void annuncia(const Missione& m, int unita, int codice, bool solo_giocatore);
    void pesa_lanci(const Missione& m, SottomarinoColpito& s);   // l'Alfa: armi e mine uscite, la lampada, la finestrella
    void danni_iniziali(const Missione& m);   // Missione::danni_iniziali sul giocatore, al primo passo
    bool iniziali_fatti_ = false;
    void lascia_segni(SottomarinoColpito& s, double tempo);
    void in_superficie(SottomarinoColpito& s);

    const std::map<std::string, ClasseNave>& classi_;
    std::string dati_;
    Casuale caso_;
    std::vector<std::unique_ptr<SottomarinoColpito>> sub_;
    std::map<std::string, DatiScafo> scafi_;
    std::vector<Messaggio> messaggi_;
    std::vector<SegnoSuperficie> segni_;
    std::vector<EmersioneViolenta> emersioni_;
    std::size_t guasti_visti_ = 0;   // Missione::guasti_richiesti gia' applicati
    std::size_t colpi_visti_ = 0;
    std::size_t impatti_visti_ = 0;   // Missione::impatti_artiglieria gia' applicati
    std::size_t urti_visti_ = 0;      // Missione::urti gia' applicati
    std::map<int, DanniArtiglieriaBattello> artiglieria_;
    double tempo_ = 0.0;   // ultimo tempo di missione (per la posa in sincronizza)
};

}  // namespace m688
