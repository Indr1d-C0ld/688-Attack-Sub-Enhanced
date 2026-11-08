#pragma once
// Siluri dell'Enhanced (docs/11_comandi_e_stazioni.md §3.3 e §10, fase C6b): impostazioni reali, corsa, ricerca,
// aggancio, inseguimento, riacquisizione, rifiuto delle esche, filo e spegnimento. Esca ADC Mk 2.
//
// Fonti: docs/ricerca/01_piattaforme_e_armi.md §6.1-6.2 (prestazioni), docs/ricerca/02_acustica_oceanografia_dottrina.md
// §6.2-6.4 (sequenza d'attacco, parametri, schemi di ricerca, evasione), docs/ricerca/05_missili_e_contromisure.md §5
// (ADC Mk 2, matrice d'efficacia delle esche, contro-contromisure). Dove le fonti aperte non danno numeri (testa,
// filo, anti-circolare) i valori sono STIME DI GIOCO, segnate come tali nella tabella delle classi (data/motore/siluri.toml, generata da
// tools/pipeline/siluri.py).
//
// Il modulo e' puro: non conosce la missione. Ogni passo riceve le sorgenti attorno (unita', esche, superficie) e
// restituisce lo stato e gli eventi. Il livello classico non lo usa.
//
// Unita': metri, secondi, m/s, gradi bussola (0 = nord, orario), quote positive verso il basso.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/acustica.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

// --- impostazioni prima del lancio (dossier 02 §6.2, punto 3) ---
enum class ModoTesta { passivo, attivo, misto };     // misto: passivo in ricerca, attivo per l'aggancio finale
enum class VelocitaSiluro { bassa, alta };            // bassa per avvicinarsi in silenzio, alta per l'attacco
enum class SchemaRicerca { serpentina, circolare, dritto };

struct ImpostazioniSiluro {
    double rotta = 0.0;                         // rotta di corsa iniziale (gradi): il rilevamento della soluzione
    ModoTesta modo = ModoTesta::passivo;
    VelocitaSiluro velocita = VelocitaSiluro::alta;
    double rte_m = 2000.0;                      // run-to-enable: distanza di corsa prima che la testa si attivi
    double quota_corsa_m = 60.0;                // quota fino all'attivazione
    double quota_ricerca_m = 100.0;             // quota della ricerca (sopra o sotto lo strato)
    double soffitto_m = 15.0;                   // la testa ignora cio' che sta piu' in alto (superficie, propria nave)
    double pavimento_m = 600.0;                 // e cio' che sta piu' in basso; il siluro non scende oltre
    SchemaRicerca schema = SchemaRicerca::serpentina;
    bool anti_circolare = true;                 // ACR: niente ritorno verso il lanciatore
};

// Campi impostabili da un ordine (nucleo/ordini: imposta_siluro). Valori: rotta e quote in gradi e metri; modo 0
// passivo, 1 attivo, 2 misto; velocita' 0 bassa, 1 alta; schema 0 serpentina, 1 circolare, 2 dritto; anti_circolare 0/1
enum class CampoSiluro { rotta, modo, velocita, rte, quota_corsa, quota_ricerca, soffitto, pavimento, schema, anti_circolare };
std::optional<CampoSiluro> campo_siluro(const std::string& nome);   // "rotta", "modo", "rte", "soffitto", ...
bool applica(ImpostazioniSiluro& imp, CampoSiluro campo, double valore);   // false se il valore non e' ammesso

// --- comandi via filo (dossier 02 §6.2 punto 4; 05 §5.6). Niente detonazione nell'Enhanced (decisione 24/10/2026) ---
enum class ComandoFiloTipo { rotta, quota, velocita, modo, abilita, rifiuta, assegna, spegni, taglia };
struct ComandoFilo {
    ComandoFiloTipo tipo = ComandoFiloTipo::rotta;
    double valore = 0.0;                        // rotta (gradi), quota (m)
    VelocitaSiluro velocita = VelocitaSiluro::alta;
    ModoTesta modo = ModoTesta::passivo;
    int sorgente = -1;                          // rifiuta / assegna: id della sorgente (come la riporta la testa)
};

// --- dati della classe d'arma (tabella in siluro.cpp) ---
struct DatiSiluro {
    std::string classe;                          // chiave (mk48, mk48_adcap, uset80, test71m, ...)
    std::string nome;
    double velocita_alta_kn = 45.0, velocita_bassa_kn = 30.0;
    double portata_alta_m = 15000.0, portata_bassa_m = 25000.0;   // autonomia alle due velocita'
    double quota_max_m = 500.0;
    double virata_gradi_s = 6.0;
    double testata_kg = 300.0;
    double spoletta_m = 8.0;                     // raggio della spoletta di prossimita' (o contatto)
    double spoletta_scafo_m = -1.0;              // la stessa dallo scafo della classe (Enhanced); < 0: spoletta_m
    bool filoguidato = false;
    double filo_siluro_m = 0.0;                  // filo che svolge il siluro
    double filo_battello_m = 0.0;                // filo che svolge il lanciatore
    bool attivo = true, passivo = true;          // modi della testa possibili
    bool segui_scia = false;                     // antinave a scia: ignora le esche acustiche, solo navi in moto
    double acquisizione_attiva_m = 1500.0;       // portata dell'attivo su un sottomarino (TS di riferimento)
    double acquisizione_passiva_m = 3000.0;      // portata del passivo su una sorgente di riferimento (130 dB)
    double semiapertura_gradi = 45.0;            // cono della testa attorno alla prua
    // probabilita' che testa e spoletta funzionino (1: sempre); un siluro guasto corre ma non vede niente (taratura A6:
    // la Pk storica dell'AT-1 comprende i guasti)
    double affidabilita = 1.0;
    double fondale_minimo_m = 0.0;   // aviolancio: in acqua piu' bassa urta il fondo nel tuffo d'ingresso (0: nessuno)
    std::string fonte;                           // riferimento ai dossier e confidenza
    int in_servizio_dal = 0;                     // anno d'entrata in servizio (il remake e' nel 1989: dossier 01 §1)
};

// --- esca acustica ADC Mk 2 (dossier 05 §5.1, 5.3) ---
struct DatiEsca {
    std::string classe = "adc_mk2";
    double livello_db = 150.0;                   // sorgente: piu' rumorosa del battello (STIMA)
    double durata_s = 600.0;                     // batteria termica (6-45 min nelle fonti: STIMA 10 min)
    double quota_m = 60.0;                       // staziona alla quota impostata (0: quella del lanciatore)
    // esche semoventi (MG-74, MOSS Mk 70: dossier 05 §5.1, 5.2, 7): corrono da un tubo con eco e rumore del battello
    double velocita_kn = 0.0;                    // 0: staziona (ADC Mk 2)
    double quota_max_m = 0.0;                    // 0: nessun limite
    double eco_db = 10.0;                        // TS del ripetitore per l'attivo
    bool imita_rumore = false;                   // suona come il lanciatore alla sua velocita'
    double virata_ogni_s = 0.0, virata_gradi = 0.0;   // manovra programmata: virata alternata (0: dritta)
    int in_servizio_dal = 0;
    bool mobile() const { return velocita_kn > 0.0; }
};
const DatiEsca& dati_adc_mk2();   // i valori predefiniti di DatiEsca (quelli di [esche.adc_mk2])
// Distanza (m) di un punto dallo scafo di un bersaglio, 0 se e' dentro: un cilindro lungo `lunghezza_m` sull'asse
// della rotta, centrato sulla posizione, con diametro `larghezza_m`; per una nave la carena dalla superficie fino a
// `pescaggio_m` (larghezza e pescaggio non noti: 10 m). Per la spoletta del siluro dell'Enhanced: misurata dallo scafo,
// non dal centro, scoppia anche passando sotto la prua o lungo il fianco di un battello di 100 m
double distanza_dallo_scafo(Vec2 p, double quota, Vec2 centro, double quota_scafo, double rotta_deg, double lunghezza_m,
                            double larghezza_m, bool nave, double pescaggio_m);
// La rotta programmata di un'esca semovente `t` secondi dopo il lancio sulla rotta `rotta0`: dritta per il primo
// intervallo, poi alternata di +-virata_gradi/2 attorno a rotta0 (zig-zag «come un sottomarino»)
double rotta_esca_mobile(const DatiEsca& d, double rotta0, double t);

// Tabella delle classi: data/motore/siluri.toml ([siluri.<classe>] e [esche.<classe>], campi come in DatiSiluro e
// DatiEsca). Lancia std::runtime_error se il file manca o una classe e' incompleta.
struct TabellaSiluri {
    std::map<std::string, DatiSiluro> siluri;
    std::map<std::string, DatiEsca> esche;
    const DatiSiluro* siluro(const std::string& classe) const;   // nullptr se la classe non c'e'
    const DatiEsca* esca(const std::string& classe) const;
};
TabellaSiluri carica_siluri(const std::string& percorso);

// --- sorgenti viste dalla testa in un passo ---
// SorgenteAcustica (le sorgenti viste dalla testa) sta in nucleo/acustica.hpp, con sorgente_per_siluri().

// --- stato e eventi ---
enum class FaseSiluro { corsa, ricerca, verifica, inseguimento, riacquisizione, spento, esaurito, esploso };
const char* nome_fase(FaseSiluro f);

struct RilevamentoTesta {                        // dati della testa che tornano lungo il filo
    int sorgente = -1;
    double rilevamento = 0.0;                    // gradi veri dal siluro
    double distanza_m = -1.0;                    // solo con l'attivo
    bool stazionaria = false;                    // Doppler quasi nullo (sospetta esca)
};

struct EventoSiluro {
    enum class Tipo { attivazione, aggancio, perdita, esca_sedotta, esca_rifiutata, filo_rotto, spento, esaurito,
                      esplosione, ping };
    Tipo tipo;
    double tempo = 0.0;
    int sorgente = -1;
    Vec2 pos;
    double quota = 0.0;
};

// Stato del battello che lancia, per i vincoli del filo
struct Lanciatore {
    Vec2 pos;
    double velocita = 0.0;                       // m/s
    double rateo_virata = 0.0;                   // gradi/s (modulo)
};

class Siluro {
public:
    Siluro(const DatiSiluro& dati, const ImpostazioniSiluro& imp, Vec2 pos, double quota, int lanciatore,
           std::uint64_t seme);

    // Un passo di dt secondi. `sorgenti`: unita' ed esche attorno; `strati`: quote dei termoclini (m).
    void avanza(double dt, const std::vector<SorgenteAcustica>& sorgenti, const std::vector<double>& strati,
                const std::optional<Lanciatore>& lanciatore);
    void comanda(const ComandoFilo& c);          // via filo; ignorato se il filo e' tagliato o rotto

    // --- stato ---
    const DatiSiluro& dati() const { return dati_; }
    const ImpostazioniSiluro& impostazioni() const { return imp_; }
    FaseSiluro fase() const { return fase_; }
    bool attivo() const { return fase_ != FaseSiluro::spento && fase_ != FaseSiluro::esaurito && fase_ != FaseSiluro::esploso; }
    Vec2 pos() const { return pos_; }
    double quota() const { return quota_; }
    double rotta() const { return rotta_; }
    double velocita() const { return velocita_; }                // m/s
    double percorso_m() const { return percorso_; }
    double autonomia_m() const;                                  // percorso che resta alla velocita' attuale
    int bersaglio() const { return bersaglio_; }
    bool filo() const { return filo_; }
    double filo_siluro_svolto_m() const { return filo_siluro_; }
    double filo_battello_svolto_m() const { return filo_battello_; }
    double filo_siluro_frazione() const;         // frazione svolta del rocchetto del siluro (0..1)
    double filo_battello_frazione() const;       // e di quello del battello
    double filo_fuori_limite_s() const { return fuori_limite_s_; }   // da quanto il battello e' fuori dai limiti
    bool filo_a_rischio() const;                 // un rocchetto oltre FILO_QUASI o il battello fuori dai limiti
    const std::vector<RilevamentoTesta>& testa() const { return testa_; }   // ultimo passo
    std::vector<EventoSiluro> eventi();          // eventi dall'ultima chiamata (si svuota)
    bool ping_in_corso() const { return ping_; } // la testa emette in attivo (udibile: evento acustico)
    bool guasto() const { return guasto_; }      // testa o spoletta guaste dal lancio (affidabilita)
    // fondale sotto il siluro (la missione, a ogni passo; senza: acqua profonda): un'eco quasi ferma a meno di
    // VICINO_FONDO_M dal fondo si perde nella riverberazione del fondo
    void imposta_fondale(double m) { fondale_m_ = m; }

    // Portate della testa su una sorgente, per le prove e per l'interfaccia (m; 0 = non la vede)
    double portata_su(const SorgenteAcustica& s, const std::vector<double>& strati) const;

    // --- costanti del modello (STIME DI GIOCO, documentate in engine/README.md) ---
    static constexpr double SERPENTINA_GRADI = 30.0;           // ampiezza della serpentina (dossier: 20-45)
    static constexpr double SERPENTINA_PERIODO_S = 60.0;
    static constexpr double VERIFICA_S = 4.0;                  // conferma prima dell'aggancio (ping o ascolti)
    static constexpr double DOPPLER_ESCA_M_S = 2.0;            // sotto: eco «stazionaria», sospetta esca
    static constexpr double PERDITA_S = 6.0;                   // senza contatto oltre: riacquisizione
    static constexpr double ACR_DISTANZA_M = 3000.0;           // anti-circolare: si spegne se punta al lanciatore entro
    static constexpr double ACR_SEMIAPERTURA_GRADI = 45.0;     //   questa distanza e questo angolo
    static constexpr double FILO_VELOCITA_MAX_KN = 12.0;       // il lanciatore oltre rompe il filo dopo FILO_TOLLERANZA_S
    static constexpr double FILO_VIRATA_MAX_GRADI_S = 2.0;
    static constexpr double FILO_TOLLERANZA_S = 5.0;
    static constexpr double FILO_QUASI = 0.8;                  // avviso al giocatore: rocchetto all'80%
    static constexpr double STRATO_FATTORE = 0.5;              // portata della testa attraverso uno strato
    // battello fermo sul fondo in acque basse (dossier 17 §6.7: Pk <= 0,1, Falkland): Doppler zero nella riverberazione
    // del fondo. Entro 10 m dal fondo e sotto DOPPLER_ESCA_M_S la testa attiva lo vede a un terzo della portata e alla
    // verifica lo scarta come fondo 7 volte su 10 (STIMA, taratura A6.1)
    static constexpr double VICINO_FONDO_M = 10.0, FONDO_FATTORE = 0.33, FONDO_RIFIUTO = 0.7;
    // disturbo: un'esca (noisemaker) entro ACCECAMENTO_M nel fascio della testa attiva riavvia la verifica e fa perdere
    // l'aggancio a un bersaglio vero con P_RIAVVIO_S al secondo (dossier 05 §5.3: effetto «medio» del noisemaker
    // contro l'attivo, il jammer riavvia l'algoritmo di verifica [TWZ]; valori STIMA, taratura A6.1)
    static constexpr double ACCECAMENTO_M = 1000.0, P_RIAVVIO_S = 0.25;
    static constexpr double AFFONDAMENTO_M_S = 1.0;            // siluro spento o esaurito: affonda

private:
    void cambia_fase(FaseSiluro f);
    void evento(EventoSiluro::Tipo t, int sorgente = -1);
    void ascolta(const std::vector<SorgenteAcustica>& sorgenti, const std::vector<double>& strati, double dt);
    void controlla_filo(double dt, const std::optional<Lanciatore>& l);
    double velocita_di(VelocitaSiluro v) const;
    bool vede(const SorgenteAcustica& s, const std::vector<double>& strati, double& distanza, double& rilev) const;
    bool sul_fondo(const SorgenteAcustica& s) const;   // eco quasi ferma vicino al fondo
    bool attraversa_strato(double q1, double q2, const std::vector<double>& strati) const;
    void governa(double dt, const SorgenteAcustica* bersaglio);

    DatiSiluro dati_;
    ImpostazioniSiluro imp_;
    Casuale casuale_;
    bool guasto_ = false;
    double fondale_m_ = 1e9;
    FaseSiluro fase_ = FaseSiluro::corsa;
    Vec2 pos_;
    double quota_ = 0.0, rotta_ = 0.0, velocita_ = 0.0, quota_ordinata_ = 0.0, rotta_ordinata_ = 0.0;
    VelocitaSiluro passo_velocita_;
    ModoTesta modo_;
    double percorso_ = 0.0, tempo_ = 0.0, tempo_fase_ = 0.0, tempo_senza_contatto_ = 0.0;
    double energia_ = 1.0;                       // frazione di autonomia rimasta
    double rotta_ricerca_ = 0.0;                 // asse della serpentina o centro del cerchio
    int lanciatore_ = -1, bersaglio_ = -1, candidato_ = -1;
    std::vector<int> rifiutate_;
    bool filo_ = false, filo_tagliato_ = false;
    double filo_siluro_ = 0.0, filo_battello_ = 0.0, fuori_limite_s_ = 0.0;
    std::optional<Vec2> lanciatore_pos_;
    bool rotta_da_filo_ = false, ping_ = false;
    std::vector<RilevamentoTesta> testa_;
    std::vector<EventoSiluro> eventi_;
};

// Esca ADC Mk 2: staziona alla quota impostata, rumorosa, Doppler zero, per la sua durata
class Esca {
public:
    Esca(Vec2 pos, double quota, double tempo_lancio, const DatiEsca& dati = dati_adc_mk2());
    bool attiva(double tempo) const { return tempo >= inizio_ && tempo < inizio_ + dati_.durata_s; }
    SorgenteAcustica sorgente(int id) const;
    Vec2 pos() const { return pos_; }
    double quota() const { return quota_; }
private:
    DatiEsca dati_;
    Vec2 pos_;
    double quota_, inizio_;
};

}  // namespace m688
