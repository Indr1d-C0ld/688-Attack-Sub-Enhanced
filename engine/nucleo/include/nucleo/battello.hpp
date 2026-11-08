#pragma once
// Sottomarino danneggiato in immersione (docs/10_danni_affondamento_esplosioni.md §10, fase D6a; numeri dai dossier
// docs/ricerca/02_acustica_oceanografia_dottrina.md §9 e 07_danni_affondamento_esplosioni.md §8).
//
// Il modulo non muove il battello: dalla quota e dalla velocita' che riceve calcola il peso netto e il momento
// d'assetto prodotti dall'acqua imbarcata e dall'aria nelle casse; chi integra il moto (la dinamica della Fase 2,
// o l'adattatore del livello cinematico, rateo_verticale_regime) li usa. In superficie il battello passa alla
// fisica delle navi (DanniNave), con la stessa acqua.
// - Compartimenti: lo scafo resistente fra le paratie (raggio per stazione da scafi/<classe>.toml), con la loro
//   aria a 1 bar compressa dall'acqua che entra (legge di Boyle): l'allagamento si ferma quando l'aria arriva alla
//   pressione del mare.
// - Falle: Q = Cd A sqrt(2 dp / rho), con dp fra il mare alla quota del battello e l'aria del compartimento.
// - Paratie: cedono oltre una differenza di pressione (una frazione della pressione di collaudo); poi l'acqua passa.
// - Casse zavorra principali: in immersione piene d'acqua (battello intatto neutro). L'esaurimento d'emergenza
//   manda l'aria delle bombole a portata limitata; l'aria occupa nelle casse il volume p_atm/p dei suoi m^3 normali
//   e si espande in risalita (dossier 02 §9.1). Una cassa squarciata non tiene l'aria.
// - Collasso: alla quota di collasso della classe per un fattore casuale dal seme fra 1,1 e 1,8 (dossier 07 §8.2),
//   abbassata dai danni allo scafo resistente; i compartimenti ancora asciutti implodono uno dopo l'altro (1-30 s),
//   con l'energia p V (dossier 07 §9.4).
// - Urto (fase D6b): fattore d'urto KSF = HSF (1 + sin theta) / 2, HSF = sqrt(W lb) / R ft, con la soglia ridotta in
//   profondita' (x (1 - 0,5 h / h_collaudo)); le fasce del dossier 02 §9.4 danno luci e scosse, guasti graduati dei
//   sistemi, SCRAM del reattore, perdite, falle, casse squarciate e rottura dello scafo resistente.
// - Equipaggio: tamponatura delle perdite piccole, pompe (efficaci solo a bassa quota), riparazione dei sistemi con
//   le priorita' del dossier 02 §9.2 (allagamento, propulsione, governo, sensori, comunicazioni), che il giocatore
//   puo' cambiare; dopo uno SCRAM motore d'emergenza a bassa velocita' fino al recupero del reattore (5-20 min).
// - Incendi (docs/10 §10.13): fuoco, fumo e ossigeno per compartimento. Il fuoco raddoppia in 2 minuti finche' c'e'
//   ossigeno e combustibile; in un compartimento chiuso consuma l'ossigeno e si spegne sotto il 12%, salvo che una
//   tubolatura dell'aria ad alta pressione rotta lo alimenti (Komsomolets, 1989). Brucia le tenute dello scafo (piccole
//   perdite), passa ai compartimenti vicini, danneggia i sistemi del compartimento; il fumo rallenta le squadre (EAB).
// - Esplosioni interne: testate nell'incendio del locale siluri, idrogeno delle batterie, siluri di prova (Kursk,
//   2000). Squarciano lo scafo resistente, fanno cedere le paratie a cascata secondo la sovrappressione (energia per
//   volume d'aria dei compartimenti raggiunti) e appiccano l'incendio. Batterie allagate: cloro (fumo tossico).
// - Isolare un compartimento (docs/10 §10.19): le paratie sono gia' chiuse, quindi non ferma l'acqua e non cambia l'ossigeno
//   che il fuoco consuma. Le squadre escono e non rientrano, il fuoco passa meno ai vicini, il ramo dell'aria si chiude se
//   non e' gia' rotto; chi resta dentro muore se il compartimento diventa letale prima che si riapra. Solo per ordine.
// Tutto il caso viene dal seme.

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "nucleo/casuale.hpp"
#include "nucleo/scafo.hpp"

namespace m688 {

struct DanniUrtoUnita;   // nucleo/collisioni.hpp

struct StatoMoto {
    double quota_m = 0.0;        // quota dell'asse dello scafo sotto il mare (m, positiva verso il basso)
    double velocita_m_s = 0.0;   // velocita' sull'acqua
};

struct ForzeIdrostatiche {
    double peso_netto_N = 0.0;        // positivo: il battello e' pesante e scende
    double momento_assetto_Nm = 0.0;  // positivo: prua in su (come AssettoScafo::assetto)
};

struct CompartimentoBattello {
    std::string nome, uso;
    double x0 = 0, x1 = 0, xc = 0;   // m, coordinate del kit (x verso prua)
    double volume = 0.0;             // m^3 allagabili (volume dello scafo resistente per la permeabilita')
    double acqua = 0.0;              // m^3
    bool imploso = false;
    // incendio (D6, docs/10 §10.13): intensita' 0..1, fumo 0..1 (1 = irrespirabile senza maschera), ossigeno (frazione)
    double fuoco = 0.0, fumo = 0.0, ossigeno = 0.21;
    double combustibile = 0.0;       // s di fuoco pieno che restano
    bool aria_nell_incendio = false; // tubolatura dell'aria ad alta pressione rotta che alimenta il fuoco
    bool distrutto = false;          // sventrato da un'esplosione interna: equipaggio perduto
    bool cloro = false;              // batterie allagate: cloro gia' riferito
    double fuoco_forte = 0.0;        // s di fuoco oltre 0,8 (tenute dello scafo che bruciano)
    // il carico del deposito (danni critici, fase 3): kg di esplosivo delle testate a bordo (dallo scafo, poi il carico
    // vivo con carico_deposito), propellente solido dei missili e dei siluri, protezione (0-1: allagamento rapido,
    // sprinkler, silos separati); i silos a propellente liquido (R-29R, R-27 del K-219) in tonnellate, che con l'acqua
    // o col fuoco reagiscono una volta
    double carica_kg = 0.0, propellente_kg = 0.0, protezione = 0.0, liquido_t = 0.0;
    bool reagito = false;
    // isolamento (BattelloDanni::isola): porte_s e' l'istante in cui le porte si chiudono (< 0: nessuno sgombero in corso),
    // dentro gli uomini rimasti chiusi
    bool isolato = false;
    double porte_s = -1.0;
    int dentro = 0;
    bool senza_squadre() const { return isolato || porte_s >= 0.0; }
    double aria() const { return volume - acqua; }
};

struct CassaBattello {
    std::string nome;
    double xc = 0.0, volume = 0.0;   // m^3
    double aria_Nm3 = 0.0;           // aria nella cassa, in m^3 normali (a 1 bar)
    bool squarciata = false;
};

struct FallaBattello {
    int compartimento = -1;
    double area = 0.0;   // m^2
};

// Lavoro del controllo danni nell'ultimo passo (BattelloDanni::squadre): dove, cosa e quante squadre. Le pompe
// sono macchine: pompare non toglie squadre (numero 0), ma si riferisce perche' il quadro danni lo mostri.
struct SquadraBattello {
    enum class Compito { tamponare, spegnere, riparare, pompare };
    Compito compito = Compito::tamponare;
    int compartimento = -1;
    int numero = 1;          // squadre su questo lavoro
    int falla = -1;          // tamponare: indice in falle()
    int sistema = -1;        // riparare: indice di Sistema
    double portata = 0.0;    // pompare: m^3/s
};

struct ParatiaBattello {
    int a = -1, b = -1;
    double resistenza_Pa = 0.0;   // differenza di pressione oltre cui cede
    bool ceduta = false;
};

// Sistemi del battello: i sei dell'originale (docs/originale/sensori_e_navigazione.md §9.4), poi quelli dell'Enhanced
enum class Sistema {
    calcolatore_sonar, schiera, periscopio, sonar_tiro, motori, antenna,
    idroplani, timone, propulsione, aria, pompe, sonar_prua,
    inerziale,   // il navigatore inerziale (DMINS o WSN-3 sul 688, Soz sull'Alfa): giu', la navigazione passa alla stima
    numero
};
const char* nome_sistema(Sistema s);
// La chiave del sistema nei dati e nelle missioni (danni_iniziali.sistemi): «sonar_prua», «aria»...; la fonte unica dei nomi
const char* chiave_sistema(Sistema s);
std::optional<Sistema> sistema_da_chiave(std::string_view chiave);

struct StatoSistema {
    double efficienza = 1.0;   // 0..1 (il «% operativo» della stazione Stato)
    double massimo = 1.0;      // fin dove le riparazioni lo possono riportare (danni gravi: parte resta)
};

struct LimitiBattello {
    double velocita = 1.0;   // frazione della velocita' massima (motori, propulsione, SCRAM)
    bool timone = true, idroplani = true, periscopio = true, antenna = true, schiera = true, sonar = true;
    bool inerziale = true;   // il navigatore inerziale funziona (efficienza almeno 0,5): se no, la stima col solcometro
    bool scram = false;      // reattore fermo: motore d'emergenza
    bool energia = true;     // c'e' corrente: il reattore, o la batteria di riserva durante lo SCRAM (fase 3)
};

struct EventoBattello {
    enum class Tipo { allagamento, paratia_ceduta, esaurimento, aria_finita, collasso, implosione,
                      scossa, guasto, scram, riparazione, falla_tamponata,
                      incendio, esplosione, incendio_spento, cloro, aria_nell_incendio, cassa_squarciata,
                      sgombero, isolamento, riapertura, caduti };
    double tempo = 0.0;
    Tipo tipo = Tipo::allagamento;
    int compartimento = -1;
    double tnt_kg = 0.0;      // implosioni: energia p V in kg di TNT
    double intensita = 0.0;   // scosse: fattore d'urto efficace (luci che tremolano, vista che sobbalza)
    int sistema = -1;         // guasti e riparazioni
    int uomini = 0;           // caduti: quanti
    std::string testo;
};

// Casse di compensazione sotto il neutro: l'acqua che si puo' espellere per alleggerire (m^3, STIMA)
inline constexpr double RISERVA_ASSETTO_M3 = 15.0;

class BattelloDanni {
public:
    BattelloDanni(const DatiScafo& dati, std::uint64_t seme);

    void aggiungi_falla(int compartimento, double area_m2);
    // Acqua che entra da fuori senza una falla nello scafo (la canna di un periscopio col vetro rotto, G3): m^3 nel
    // compartimento, fino a riempirlo; le pompe la tolgono come quella delle falle tamponate
    void imbarca(int compartimento, double m3);
    void squarcia_cassa(int cassa);
    void esaurisci();            // esaurimento d'emergenza delle casse (Ctrl+E nell'Enhanced)
    void ferma_esaurimento();
    // Esplosione a distanza dallo scafo: TNT equivalente per l'urto (kg), distanza dal punto piu' vicino dello
    // scafo (m), angolo d'incidenza dall'orizzontale (rad, pi/2 = da sotto o da sopra), x del punto piu' vicino
    // (coordinate del kit). Restituisce il fattore d'urto efficace (KSF diviso per la riduzione con la quota).
    double urto(double tnt_kg, double distanza_m, double angolo_rad, double x_m, double quota_m);
    // Urto meccanico di una collisione (nucleo/collisioni.hpp, valuta_urto): scossa, sistemi, falla nello scafo
    // resistente nel compartimento al punto, casse squarciate vicino al punto, ammaccatura (quota di collasso piu' bassa)
    void urto_meccanico(const DanniUrtoUnita& d);
    void priorita(Sistema s);    // il giocatore porta un sistema in cima alle riparazioni
    const std::vector<Sistema>& ordine_riparazioni() const { return priorita_; }
    // Isolare un compartimento (docs/10 §10.19). isola: le squadre escono subito e le porte si chiudono dopo lo sgombero
    // (SGOMBERO_S; senza personale subito); chiudi_subito, durante lo sgombero: chi non e' uscito resta dentro; riapri: chi
    // e' dentro torna fuori, e a fuoco vivo l'ossigeno risale e il fumo si mescola coi vicini. Il posto di comando e i
    // compartimenti perduti non si isolano (isolabile).
    bool isolabile(int compartimento) const;
    void isola(int compartimento);
    void chiudi_subito(int compartimento);
    void riapri(int compartimento);
    // Compartimenti tecnici senza personale e macchine comandate dal posto centrale (Alfa, dossier 19 §3.1-3.2): si
    // chiudono subito, nessuno resta dentro, il motore non ne risente
    void senza_personale(bool si) { senza_personale_ = si; }
    int caduti() const { return caduti_; }   // uomini morti nei compartimenti isolati
    // Incendio in un compartimento (intensita' iniziale 0..1); con `aria` una tubolatura dell'aria ad alta pressione
    // rotta lo alimenta da subito
    void incendio(int compartimento, double intensita, bool aria = false);
    // Esplosione dentro un compartimento (kg di TNT): scafo squarciato oltre 50 kg, paratie a cascata, incendio
    void esplosione_interna(int compartimento, double tnt_kg);
    // Il carico vivo di un deposito (kg di esplosivo delle armi a bordo: la sala siluri del giocatore, la dotazione
    // dell'IA); sostituisce quello dello scafo. Il propellente scala nella stessa proporzione
    void carico_deposito(int compartimento, double tnt_kg);
    double batteria() const { return batteria_; }   // carica della batteria di riserva, 0-1
    // Compartimento che ospita un sistema (sonar a prua, governo in centrale, propulsione nel reattore o nelle macchine)
    int compartimento_di(Sistema s) const;
    int compartimento_a(double x_m) const { return compartimento_vicino(x_m); }   // il piu' vicino al punto x
    // Casse d'assetto e di compensazione (per posarsi sul fondo, docs/11 §6): acqua da imbarcare in piu' (m^3, verso
    // l'obiettivo: dalle valvole a 0,05 m^3/s; in uscita con le pompe, che a 200 m non rendono piu'). 0 = neutro.
    // Sotto lo 0 si espelle la riserva delle casse di compensazione (fino a -RISERVA_ASSETTO_M3, con le pompe).
    void assetto(double m3) { assetto_obiettivo_ = std::max(-RISERVA_ASSETTO_M3, m3); }
    double acqua_assetto_m3() const { return assetto_; }
    double assetto_obiettivo_m3() const { return assetto_obiettivo_; }
    // Casse dell'Alfa (dossier 19 §3.1-3.2 e §6.2; ПОКС e Tan dell'Enhanced):
    // - sfoga: apre gli sfoghi delle casse principali; ferma l'esaurimento e l'aria esce in ~15 s (STIMA), poi si
    //   richiudono;
    // - compensa: +- m^3 d'acqua all'obiettivo delle casse d'assetto (alle portate di assetto());
    // - peso variabile: le armi lanciate (varia_peso: kg, negativi se escono, al punto x) e la compressione dello scafo
    //   con la quota (abilita_compressione: t in piu' ogni 100 m sotto la quota a cui il battello e' stato messo in
    //   assetto, in meno sopra); zero per difetto, cosi' gli altri battelli restano neutri come prima;
    // - Tan, l'assetto automatico: porta l'obiettivo delle casse d'assetto a compensare il peso variabile e l'acqua
    //   imbarcata (entro la riserva, alla portata delle valvole e delle pompe); si disinserisce da solo con le pompe
    //   sotto il 50%;
    // - guasto delle casse (la finestrella): aria o pompe sotto il 50%, o una cassa squarciata.
    void sfoga();
    bool sfogo() const { return sfoga_; }
    void compensa(double m3) { assetto(assetto_obiettivo_ + m3); }
    void varia_peso(double kg, double x_m);
    void abilita_compressione(double t_per_100m, double quota_neutra_m = 0.0) {
        compressione_t_100m_ = std::max(0.0, t_per_100m);
        quota_neutra_ = std::max(0.0, quota_neutra_m);
    }
    void tan(bool si) { tan_ = si; }
    bool tan() const { return tan_; }
    bool guasto_casse() const;
    double peso_variabile_N(double quota_m) const;
    ForzeIdrostatiche avanza(double dt, const StatoMoto& moto);   // s di simulazione

    const std::vector<CompartimentoBattello>& compartimenti() const { return comp_; }
    const std::vector<CassaBattello>& casse() const { return casse_; }
    const std::vector<ParatiaBattello>& paratie() const { return paratie_; }
    const std::vector<EventoBattello>& eventi() const { return eventi_; }
    ForzeIdrostatiche forze(double quota_m) const;
    double acqua_imbarcata_m3() const;
    double aria_bombole_Nm3() const { return aria_bombole_; }
    double quota_collasso_m() const { return quota_collasso_ * (1.0 - 0.5 * danno_scafo_); }
    const std::vector<StatoSistema>& sistemi() const { return sistemi_; }
    const StatoSistema& sistema(Sistema s) const { return sistemi_[static_cast<std::size_t>(s)]; }
    LimitiBattello limiti() const;
    double danno_scafo() const { return danno_scafo_; }   // 0..1: abbassa la quota di collasso
    const std::vector<FallaBattello>& falle() const { return falle_; }
    // controllo danni: lavori dell'ultimo passo e squadre disponibili (in proporzione alla parte abitabile)
    const std::vector<SquadraBattello>& squadre() const { return squadre_; }
    int squadre_disponibili() const { return squadre_disponibili_; }
    double area_pianta_m2() const { return area_pianta_; }
    bool esaurimento() const { return soffia_; }
    bool perso() const { return perso_; }
    double tempo() const { return tempo_; }
    void imposta_tempo(double t) { tempo_ = t; }   // orologio degli eventi (es. il tempo di missione alla creazione)
    // Un sistema rotto da fuori (la missione: il periscopio piegato dalla velocita', docs/14 P2): il guasto nel
    // rapporto, col testo della causa davanti
    void guasto_sistema(Sistema s, double perdita, const std::string& testo);

private:
    void evento(EventoBattello::Tipo t, int comp, const std::string& testo, double tnt = 0.0);
    double pressione_aria(const CompartimentoBattello& c) const;
    void danneggia(Sistema s, double perdita);
    void equipaggio(double h, double quota);
    int compartimento_vicino(double x) const;
    void passo_incendi(double h, double quota);
    void passo_isolamento();
    int uomini_in(const CompartimentoBattello& c) const;   // di guardia nel compartimento (STIMA)
    std::vector<int> vicini(int c) const;   // compartimenti oltre le paratie di c

    DatiScafo dati_;
    Casuale caso_;
    Casuale caso_inerziale_;   // gli urti sull'inerziale: un flusso suo, cosi' gli altri sistemi restano come prima
    std::vector<CompartimentoBattello> comp_;
    std::vector<CassaBattello> casse_;
    std::vector<FallaBattello> falle_;
    std::vector<ParatiaBattello> paratie_;
    std::vector<SquadraBattello> squadre_;
    int squadre_disponibili_ = 0;
    std::vector<EventoBattello> eventi_;
    double x_riferimento_ = 0.0, area_pianta_ = 0.0;
    double aria_bombole_ = 0.0, portata_aria_ = 0.0;   // m^3 normali, m^3 normali/s
    double quota_collasso_ = 0.0;
    double tempo_ = 0.0, prossima_implosione_ = 0.0;
    bool soffia_ = false, perso_ = false;
    std::vector<StatoSistema> sistemi_ = std::vector<StatoSistema>(static_cast<std::size_t>(Sistema::numero));
    std::vector<Sistema> priorita_;   // ordine delle riparazioni dei sistemi
    double danno_scafo_ = 0.0, fine_scram_ = -1.0;
    double assetto_ = 0.0, assetto_obiettivo_ = 0.0;   // m^3 nelle casse d'assetto oltre il neutro
    bool sfoga_ = false, tan_ = false;
    double peso_variabile_kg_ = 0.0, momento_variabile_Nm_ = 0.0, compressione_t_100m_ = 0.0, quota_neutra_ = 0.0;
    bool nucleare_ = false;
    // energia (fase 3): la batteria di riserva dei nucleari si scarica durante lo SCRAM; allagata o bruciata, e' persa
    double batteria_ = 1.0;
    bool senza_energia_ = false;   // gia' riferito
    // i contenitori dei missili fra i due scafi (Oscar II, Charlie II): x0, x1, kg di esplosivo che restano
    struct Contenitori { double x0 = 0, x1 = 0, carica_kg = 0; };
    std::vector<Contenitori> contenitori_;
    bool senza_personale_ = false;
    int caduti_ = 0;
};

// Adattatore cinematico (decisione 2 del §10.3): velocita' verticale di regime (m/s, positiva verso il basso) di
// un battello con il peso netto dato, dalla resistenza al moto verticale sulla sua pianta, con la portanza che
// piani e scafo danno alla velocita' v (stima: 0,5 rho v^2 S Cl con S = 3 % della pianta, Cl = 0,8: sul 688 circa
// 100 t a 10 m/s, docs/10 §10.3). piani: +1 barra tutta in salita (dottrina dopo un allagamento, dossier 02 §9.2),
// 0 per tenere la quota (la portanza si oppone al peso netto, in un verso o nell'altro), -1 tutta in discesa.
double rateo_verticale_regime(double peso_netto_N, double area_pianta_m2, double velocita_m_s = 0.0, int piani = 0);

}  // namespace m688
