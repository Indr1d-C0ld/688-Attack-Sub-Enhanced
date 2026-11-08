#pragma once
// Registro degli eventi di missione ed esito dettagliato (M1, docs/12_schermate_di_missione.md; requisito 16 di
// docs/00_ROADMAP.md): la base del debriefing evento per evento alla F-19, della carriera (M3) e delle statistiche.
//
// Il registro osserva soltanto: non usa il generatore casuale e non cambia lo stato, quindi il livello classico resta
// identico e lo stesso seme con gli stessi comandi (e gli stessi passi) da' gli stessi eventi con gli stessi id. Si
// riempie alla fine di ogni passo di 2 s della missione, prima che le unita' uscite si tolgano, con alcuni ganci alla
// fonte (radio, regole d'ingaggio, trasmissioni, classificazioni del giocatore); danni subiti, emersioni d'emergenza e
// quota di collasso li raccoglie Missione::raccogli_danni dopo DanniSottomarini::avanza e DanniMissione::avanza.
//
// Obiettivi: la chiave `obiettivi` della missione (rapide, editor), altrimenti data/missioni/obiettivi/<ID>.lua
// (le 22 originali), altrimenti la forma generica (sopravvivere); `categorie_esiti` da' la categoria dei codici
// d'esito dell'originale. Formato in docs/09_schema_missioni.md §5.2.

#include <algorithm>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "nucleo/geometria.hpp"
#include "nucleo/radio.hpp"

struct lua_State;

namespace m688 {

class Missione;
class DanniSottomarini;
class DanniMissione;
struct UnitaClassica;

enum class TipoEvento : std::uint8_t {
    inizio, fine,                    // fine: esito della missione (codice >= 100 dell'originale)
    scoperta,                        // il giocatore ha un contatto nuovo (la prima volta per unita')
    classificazione,                 // durata del contatto oltre 800 (categoria) e oltre 1400 (classe), o il giocatore
    lancio_giocatore,                // siluro, missile, esca, boa; missile del modello: livello = taglia della scatola + 1
                                     // (1 piccola, 2 media, 3 grande), codice = modo (0 RBL, 1 BOL)
    lancio_nemico,                   // arma di un'unita' ostile al giocatore (verso chiunque)
    colpo,                           // a segno (Missione::colpi), anche sul giocatore
    affondamento,                    // unita' distrutta (regola classica, fisica dei danni, urto col fondo)
    emersione_emergenza,             // E1 di un sottomarino dell'IA
    siluro_evitato,                  // arma ostile diretta sul giocatore finita senza colpirlo
    esca_sedotta,                    // un siluro del modello ha preso un'esca
    danno_subito,                    // rapporti di DanniSottomarini sul giocatore
    danno_domato,                    // incendio spento o falla tamponata
    scram,                           // reattore del giocatore fermo
    camera_di_lancio_persa,          // il compartimento dei tubi allagato o esploso: le armi a bordo sono perse
    messaggio_ricevuto, messaggio_perso, compito_scaduto, sessione_mancata,
    ordine_ricevuto,                 // messaggio radio che porta un obiettivo (`da_radio` nel file degli obiettivi)
    trasmissione,                    // il giocatore (o una sua boa) ha trasmesso
    infrazione_roe,                  // lancio contro le regole d'ingaggio o in una zona protetta
    ingresso_zona,                   // zona protetta conosciuta
    fix_radiogoniometria,            // il nemico ci ha localizzati (o rilevati) dopo una trasmissione
    avvistato,                       // un'unita' ostile ha la posizione del giocatore (prima volta o dopo 10 minuti)
    esm_rischio_alto,                // 336 e 337
    ping_attivo,                     // ping del giocatore
    ping_subito,                     // ping nemico che arriva al giocatore (312)
    transitorio_proprio,             // cavitazione del giocatore (313)
    boa_vicina,                      // boa ostile entro 2 nm dal giocatore
    incaglio,                        // urto col fondo del giocatore
    fuoco_amico,                     // il giocatore ha colpito un amico o un neutrale
    mina_posata,                     // M2b: una mina arrivata sul fondo (livello 1: inerte, fondale inadatto)
    mina_esplosa,                    // M2b: una mina e' scoppiata sotto un bersaglio o ha lanciato il suo siluro
    // ASW aereo dell'Enhanced (docs/13_asw_aereo.md, A3): dagli avvisi 382-385
    aereo_in_sorvolo,                // un aereo e' passato nella finestra di Snell sopra il giocatore
    ping_sonar_immersione,           // il giocatore sente il ping di un sonar a immersione
    marcatore_avvistato,             // un fumogeno o un marcatore aereo in vista del periscopio
    attacco_simulato,                // SUS in pace: attacco simulato contro il giocatore
    // periscopi dell'Enhanced (docs/14_periscopi.md, P2)
    foto_periscopio,                 // foto del Type 18: valore = rilevamento vero, livello = indice dell'ingrandimento
    esposizione_lunga,               // la testa fuori oltre 30 s in una guardata: valore = secondi
    // il sonarista dell'equipaggio (S5d): le sue classificazioni sono `classificazione` col codice 411
    scambio_tracce,                  // due tracce si sono scambiate in un incrocio (verita'): unita' = ora, altra = prima
    // artiglieria navale dell'Enhanced (docs/16_artiglieria.md, G2): unita' = la nave che spara
    artiglieria_salva,               // una nave apre il fuoco sul giocatore (codice 1: d'avvertimento)
    artiglieria_colpo,               // colpo diretto o schegge sul giocatore, uno per proietto: classe_arma = pezzo, valore = calibro mm, codice = sparo
    artiglieria_vicino,              // il primo colpo caduto entro 50 m dal giocatore: valore = distanza
    // collisioni dell'Enhanced (docs/ricerca/16_collisioni.md): unita' = l'altra
    collisione,                      // urto del giocatore: valore = energia normale (MJ), livello 1 se radente
    quasi_collisione,                // passaggio ravvicinato: valore = distanza fra gli scafi, codice 1 se lo sapevamo
    emersione_forzata,               // M6: un diesel dell'IA emerge con la batteria scarica (unita')
    cavo_impigliato,                 // S10: il cavo della schiera di una nave nell'elica del giocatore (unita' = la nave)
    cavo_toccato,                    // S10: lo scafo del giocatore tocca il cavo (unita' = la nave)
    mina_scoperta,                   // S06: il BQS-15 vede una mina (pos stimata, valore = distanza, livello 1 ormeggiata)
    avaria_ia,                       // M6: un'unita' dell'IA perde la propulsione (unita')
    rimorchio,                       // M6: unita' = il rimorchiato, altra = il rimorchiatore; livello 1 alla fine
    acque_territoriali,              // il giocatore nelle acque di un altro paese (codice = indice), livello 1 all'uscita
    colpo_esercitazione,             // un'arma da esercitazione a segno: unita' = la vittima, altra = il lanciatore
    // le interazioni fra unita' dell'IA (docs/18, fase 5: lo spettatore). noto: l'equipaggio del giocatore ha un
    // contatto visibile su chi agisce (lancio, salve, urti); scoperte, ingaggi ed evasioni sono decisioni interne
    lancio_ia,                       // un lancio di un'unita' dell'IA: unita' = lanciatore, altra = bersaglio, arma
    scoperta_ia,                     // il primo contatto visibile di un'IA su un'unita' ostile: unita' = chi scopre
    ingaggio_ia,                     // un'IA aggancia un'unita' ostile (ENGAGE): unita' = chi attacca, altra = bersaglio
    evasione_ia,                     // una manovra d'evasione nuova di un sottomarino dell'IA (valore = quante finora)
    artiglieria_ia,                  // una nave apre il fuoco su un'unita' che non e' il giocatore: altra = bersaglio
    collisione_ia,                   // urto fra due unita' dell'IA: valore = energia normale (MJ)
    classificazione_ia,              // il sonarista di un'IA classifica: unita' = chi ascolta, altra = la sorgente vera,
                                     // classe_creduta, giusta; livello = confidenza (1 POSSUB, 2 PROBSUB, 3 CERTSUB)
    // le forze dell'Enhanced (docs/18 fase 3): noto falso, decisioni della forza
    allerta_forza,                   // l'allerta cambia: codice = lato (8 usa, 16 urss, 0 neutrali), valore = di prima,
                                     // livello = nuovo, classe_arma = causa; unita' = chi l'ha vissuto (la sua
                                     // formazione), -1 la forza intera
    messaggio_ia,                    // un rapporto di contatto fra unita' dell'IA (fase 3 B): unita' = chi riferisce,
                                     // altra = su chi; codice = canale (0 formazione, 1 aereo-nave, 2 teatro,
                                     // 3 broadcast ai sottomarini, 4 al giocatore), valore = ritardo s, giusta = arrivato
    provocazione,                    // in crisi, un atto ostile porta in guerra due parti: unita' = chi l'ha fatto,
                                     // altra = su chi; codice = lato provocato, livello = lato che provoca; classe_arma
                                     // = lancio, colpo, salva
    fuoco_amico_ia,                  // un'IA attacca un'unita' della sua parte (fase 3 C): unita' = chi, altra = su chi,
                                     // classe_arma = lancio, colpo, salva
    // la difesa dell'IA (docs/18 fase 4 A)
    esca_ia,                         // un sottomarino dell'IA lancia un'esca: unita' = il battello, arma = l'esca,
                                     // classe_arma = adc_mk2 o mg74; noto se l'equipaggio ha un contatto sul battello
    ritirata_ia,                     // un'IA si allontana: unita' = chi, altra = da chi (il lanciatore, il siluro,
                                     // l'altra unita', chi ha pingato); classe_arma = colpito, siluro, collisione, ping
    // le armi dell'IA (docs/18 fase 4 B)
    tma_ia,                          // le gambe di TMA di un sottomarino dell'IA: unita' = chi, altra = il bersaglio,
                                     // livello = gambe fatte; codice 0 gamba nuova, 1 soluzione, 2 rinuncia
    reazione_ia,                     // una reazione per situazione (fase 4 C): unita' = chi reagisce, altra = la causa
                                     // (l'attaccato, l'affondato, il contatto); classe_arma = reazione (datum,
                                     // tiene_sotto, allontana, ascolta, copre, cue, pedina); classe_creduta = ruolo (asw, nave,
                                     // sottomarino, aereo); codice 0 scoperto, 1 attaccato, 2 affondato, 3 il pedinamento
                                     // del giocatore fatto dallo script
    incsea,                          // pace (fase 4 D): due navi di parti diverse sotto i 1000 yd: unita', altra; valore = m
    punto_nave,                      // navigazione incerta: classe_arma = tipo del punto nave; giusta = fatto; valore =
                                     // spostamento della stima (m); distanza_m = sigma dopo (m); codice = MotivoPuntoNave
                                     // se non fatto
    comandante_ia,                   // sandbox: il comandante IA del battello cambia fase: classe_arma = fase (transito,
                                     // pattuglia, tma, attacco, difesa, punto_nave, ritirata), classe_creduta = motivo,
                                     // altra = il contatto su cui lavora
    esito_tlam,                      // un TLAM all'ora d'arrivo, la verita' (docs/21 §11.8): classe_arma = missione,
                                     // classe_creduta = bersaglio, giusta = arrivato, valore = s di volo, pos = landfall o
                                     // caduta; codice 1 se l'arrivo e' nella finestra (tot +- finestra_s); il
                                     // giocatore lo sa solo dal BDA alla radio, piu' tardi
    fine_missile,                    // un missile antinave che finisce senza impatto (docs/21 §11.9): unita' = il
                                     // lanciatore, arma = il missile, classe_arma = la classe; codice 0 esaurito
                                     // (combustibile o ricerca finita), 1 guasto in volo; valore = s di volo, pos = dove;
                                     // noto falso (senza collegamento col missile nel 1989: semplicemente non arriva).
                                     // Codice 2: fermato dalla difesa della nave (T13 del kit): altra = la nave, pos e
                                     // quota (m sul mare) dello scoppio in aria, valore = kg d'esplosivo (0 inerte)
    misura_telemetro,                // una lettura del telemetro del giocatore (docs/25 §3.3): unita' = il contatto, valore =
                                     // la distanza letta, distanza_m = la vera, misura_m = l'altezza d'albero usata
    // docs/26 (achievement e statistiche), del giocatore; noto falso dove porta la verita'
    osservazione_aob,                // un AOB al periscopio: unita' = il contatto, valore = errore in gradi dalla rotta vera
    ping_ecoscandaglio,              // un ping dell'ecoscandaglio (quadro del fondale)
    allarme_siluro,                  // l'allarme dell'intercettatore che sale: codice 1 siluro in acqua, 2 in acquisizione
    attraversamento_mine,            // un campo di mine ostili attraversato col BQS-15 acceso, senza danni
    verifica_radio,                  // il gruppo d'autenticazione verificato: giusta = autentico (falso: riconosciuto non autentico)
    luci,                            // le luci di navigazione d'un'unita' cambiano (docs/27): valore = lo stato (luci::Stato)
    punto_nave_ia,                   // un punto nave d'un sottomarino dell'IA (navigazione incerta): unita' = chi, valore = lo
                                     // spostamento della stima (m), distanza_m = la sigma dopo, classe_arma = il tipo; noto falso
    sonar_traino,                    // una nave dell'IA fila o recupera il VDS o la schiera (docs/ricerca/33): unita' = la nave,
                                     // livello 1 filato (in quota) o 0 recuperato, valore = la quota (m; < 0 non ancora),
                                     // classe_arma = il sistema; noto falso (il giocatore non lo vede)
    intervento,                      // il vetrino di Dio (docs/28): unita' = quella toccata, codice = id dell'intervento,
                                     // classe_arma = il tipo, livello 1 applicato, 2 finito, 3 fallito; noto falso
                                     // (lo spettatore lo vede, le parti no)
    posti_di_combattimento,          // il battello del giocatore va ai posti di combattimento (livello 1) o torna alla
                                     // condizione normale (0); classe_arma = la causa (siluro, lancio, contatto, normale)
    numero
};
const char* nome_evento(TipoEvento t);   // chiavi stabili ("lancio_giocatore", ...) per file e prove

enum class Lato : std::uint8_t { nessuno, ostile, neutrale, amico };

// Fotografia di un'unita' coinvolta, per disegnare senza replay: resta anche dopo che l'unita' e' uscita dal gioco
struct PosaEvento {
    int unita = -1;
    std::string classe, categoria;
    Vec2 pos;
    double quota = 0.0, rotta = 0.0, velocita_kn = 0.0;
};

// Suggerimento per la minicartolina (M4): l'unita' da inquadrare e da dove; le cose che non si vedono hanno un'icona
struct Inquadratura {
    bool visibile = true;
    std::string icona;               // non visibili: "radio", "esm", "radiogoniometro", "danni", "zona", "sonar", "mina"
    int soggetto = -1;               // l'unita' da inquadrare (la sua posa e' in EventoMissione::pose)
    int secondo = -1;                // l'altra da tenere in campo, o -1
    double distanza_m = 0.0;         // della camera dal soggetto
    double elevazione_deg = 0.0;     // sopra l'orizzonte; negativa: sott'acqua
    double azimut_deg = 0.0;         // da dove guarda, rispetto alla prua del soggetto (90: dal traverso di dritta)
};

struct EventoMissione {
    int id = 0;                      // progressivo, stabile nel replay: il «fatto» per motivazioni e inchieste
    TipoEvento tipo = TipoEvento::inizio;
    double tempo = 0.0;              // s di missione
    Vec2 pos;                        // dove succede (unita' principale, punto dello scoppio, il giocatore)
    double quota = 0.0;
    int giocatore = 0;               // 0 o 1
    int unita = -1;                  // principale: il contatto, la vittima, chi lancia, chi ci avvista
    int altra = -1;                  // il bersaglio di un lancio, il lanciatore di un colpo
    int arma = -1;
    std::string classe, categoria, classe_arma;   // dell'unita' principale (classe e categoria) e dell'arma
    Lato lato = Lato::nessuno;       // dell'unita' principale rispetto al giocatore (lanci: del bersaglio)
    std::optional<CategoriaRoe> categoria_roe;
    std::optional<GiudizioRoe::Esito> giudizio;   // lanci del giocatore con la sala radio
    double distanza_m = 0.0;         // dal giocatore all'unita' principale
    // classificazioni
    std::string classe_creduta;      // quella che il giocatore (o il sonar) ha dato
    int livello = 0;                 // 1 categoria (nave, sottomarino), 2 classe
    bool giusta = true;
    bool visiva = false;             // classificazione a vista dal libretto (docs/25 §3.2), non dal sonar
    double misura_m = 0.0;           // misura_telemetro: l'altezza d'albero usata sul quadrante
    double tempo_scoperta = -1.0;    // prima scoperta del contatto (anticipo)
    int codice = 0;                  // messaggio collegato (340, 328, codice d'esito...), serie radio, o 0
    double valore = 0.0;             // gravita' del danno (0..1), dB, errore del fix in m... secondo il tipo
    std::string testo_it, testo_en;  // descrizione breve gia' composta
    int interesse = 0;               // 0..100, per i momenti salienti
    bool noto = true;                // il giocatore lo sapeva allora (verita' sovrapponibile del debriefing)
    std::vector<PosaEvento> pose;    // giocatore, unita', altra, arma, quando ci sono
    Inquadratura inquadratura;
};

// Replay fino a un evento (minicartoline, M4 e M5): con lo stesso seme, lo stesso storico degli ordini (carico
// compreso, RegistroOrdini::carico) e lo stesso passo fisso dall'inizio, gli eventi tornano con gli stessi id e le
// stesse pose. Si avanza finche' RegistroMissione::evento(id) c'e': succede entro passi_per_evento(e, passo) passi
// (l'evento dell'osservazione alla fine del suo passo; quelli degli ordini, consegnati prima del passo, uno dopo)
long passi_per_evento(const EventoMissione& e, double passo);

// Interesse di un evento (funzione pura: la carriera e le statistiche possono ricalcolarlo). `primo`: il primo evento
// del suo tipo nella missione (la prima scoperta, il primo siluro, la prima contro-scoperta)
int interesse(const EventoMissione& e, bool primo);

struct Obiettivo {
    enum class Tipo : std::uint8_t { affondare, proteggere, raggiungere, scoprire, non_essere_scoperto, sopravvivere,
                                     variabile, esito, roe, minare,   // minare: mine posate in un cerchio
                                     contatto,     // contatto: minuti di contatto sonar cumulativi con le unita'
                                     foto,         // foto: le unita' nel campo di una foto al periscopio (P2)
                                     zona_inviolata,     // nessuna unita' di una parte nel cerchio
                                     colpire,            // colpire (anche con armi da esercitazione), senza affondare
                                     tlam,               // i TLAM della missione `missione` arrivati (nella finestra)
                                     scatola,            // nessun missile lanciato con una scatola di ricerca oltre `massima`
                                     identificare,       // le unita' con l'ultima identificazione visiva del giocatore giusta
                                     telemetro,          // unita' diverse lette al telemetro con l'errore relativo entro `errore`
                                     rotta_stimata };    // unita' diverse con un AOB al periscopio entro `errore` gradi dalla rotta vera
    enum class Stato : std::uint8_t { in_corso, raggiunto, parziale, fallito };
    std::string id, testo_it, testo_en;
    Tipo tipo = Tipo::sopravvivere;
    bool primario = true;
    double peso = 1.0;
    Stato stato = Stato::in_corso;
    int fatti = 0, richiesti = 1;
    double quota() const { return richiesti > 0 ? std::min(1.0, static_cast<double>(fatti) / richiesti) : 0.0; }
    std::vector<int> unita;          // le unita' dell'obiettivo, risolte all'avvio (id o classi)
    std::string da_radio;            // ordine arrivato via radio: id del messaggio (vuoto: ordini di partenza)
    int serie = -1;                  // la serie che l'ha portato (da_radio ricevuto), -1 altrimenti
};

struct ContoArma { int lanci = 0, colpi = 0; };
struct ContoBersaglio {
    std::string classe;
    Lato lato = Lato::nessuno;
    int colpi = 0;                   // del giocatore
    bool affondato = false, dal_giocatore = false;
};

struct EsitoDettagliato {
    enum class Categoria : std::uint8_t { in_corso, successo, successo_parziale, fallimento, battello_perso, richiamato,
                                          fuoco_amico };
    std::optional<int> codice;       // esito dell'originale (n di messaggio 100 + n), se c'e'
    std::string testo_it;            // testo_esito
    Categoria categoria = Categoria::in_corso;
    std::vector<Obiettivo> obiettivi;
    // 0..1: obiettivi attivi raggiunti (parziali in proporzione, primari doppi), meno fino a meta' per i vincoli violati
    // (proteggere, non essere scoperti, ROE); sopravvivere e' fuori (battello_perso)
    double completezza = 0.0;
    double riuscita = 0.0;           // 0..1: gli obiettivi attivi primari
    bool ordini_rispettati = true;   // nessuna infrazione e nessuna zona protetta
    double durata_s = 0.0;
    // armi e bersagli
    int lanci = 0, lanci_a_segno = 0, siluri_evitati = 0, colpi_subiti = 0;
    std::map<std::string, ContoArma> per_arma;            // per classe d'arma, del giocatore
    std::map<int, ContoBersaglio> per_bersaglio;          // unita' colpite dal giocatore o affondate
    int affondati_ostili = 0, affondati_neutrali = 0, affondati_amici = 0;   // dal giocatore
    int neutrali_colpiti = 0, amici_colpiti = 0, fuoco_senza_ordine = 0, zone_vietate = 0;
    // sensori e classificazioni
    int classificazioni_giuste = 0, classificazioni_errate = 0;
    double anticipo_medio_s = 0.0;   // dalla prima scoperta alla classificazione, medio
    // esposizione: conti per tipo (i pesi sono della carriera)
    int ping_subiti = 0, fix_subiti = 0, avvistamenti = 0, esm_rischio_alto = 0, transitori = 0, boe_vicine = 0, ping_attivi = 0;
    int sorvoli = 0, ping_immersione = 0, marcatori_avvistati = 0, attacchi_simulati = 0;   // ASW aereo
    int foto_periscopio = 0, esposizioni_lunghe = 0;   // periscopi dell'Enhanced
    int classificazioni_sonarista = 0, errori_sonarista = 0, scambi_tracce = 0;   // il sonarista (non nel giudizio del giocatore)
    double esposizione_periscopio_s = 0.0;             // secondi con la testa del periscopio fuori (Enhanced)
    double tempo_esposto_s = 0.0;    // albero su o in superficie
    // radio e ordini
    int ordini = 0, ordini_eseguiti = 0, rapporti_richiesti = 0, rapporti_inviati = 0;
    // mine del giocatore (M2b)
    int mine_posate = 0, mine_inerti = 0, mine_esplose = 0;
    int serie_copiate = 0, serie_perse = 0, sessioni_mancate = 0, compiti_scaduti = 0;
    std::vector<GiudizioRoe> infrazioni;
    std::vector<std::pair<double, std::string>> ingressi_zone;
    // danni e pericolo
    double danno_massimo = 0.0;      // 0..1
    bool incaglio = false, danno_nucleare = false;
    int danni_domati = 0;
    bool battello_perso = false, equipaggio_salvo = true;
    int feriti = 0, morti = 0;
    bool comandante_ferito = false;
    bool stimati = true;             // feriti e morti: stima (finche' la D6 non ha l'equipaggio, valgono 0)
    double distanza_min_siluro_m = 1e9;   // fra il giocatore e un'arma ostile in acqua
    double tempo_contatto_asw_s = 0.0;    // un'unita' ostile antisommergibile ha un contatto sul giocatore
    double margine_collasso = 1.0;        // minimo di 1 - quota / quota di collasso
};
const char* nome_categoria_esito(EsitoDettagliato::Categoria c);   // "successo", ... (chiavi di categorie_esiti)

class RegistroMissione {
public:
    const std::vector<EventoMissione>& eventi() const { return eventi_; }
    // Gli obiettivi come li legge il comandante IA della sandbox (missioni/comandante_ia.hpp): il tipo, le classi delle
    // unita' (quelle che il briefing nomina) e l'area, se c'e'
    struct AreaObiettivo {
        std::string id;
        Obiettivo::Tipo tipo = Obiettivo::Tipo::sopravvivere;
        bool primario = true;
        std::vector<std::string> classi;
        std::optional<Vec2> centro;
        double raggio_m = 0.0;
    };
    std::vector<AreaObiettivo> aree_obiettivi() const;
    const EventoMissione* evento(int id) const;   // nullptr se non c'e' (ancora)
    // i momenti del debriefing riepilogativo: al massimo n, i piu' interessanti distanziati di almeno spazio_s (a
    // parita' il primo), sempre compresi scoperte di sottomarini, colpi, affondamenti, danni gravi e fine; in ordine
    // di tempo
    std::vector<const EventoMissione*> momenti_salienti(int n = 12, double spazio_s = 60.0) const;
    EsitoDettagliato esito(const Missione& m) const;

private:
    friend class Missione;
    struct DefObiettivo {
        Obiettivo o;
        std::vector<int> unita;
        std::vector<std::string> classi;
        int minimo = -1;             // -1: tutte
        bool colpire = true;         // proteggere: false = non colpire nemmeno
        std::optional<Vec2> centro;
        double raggio = 0.0, entro_s = -1.0;
        int indice = 0;
        std::optional<int> uguale, almeno, al_piu;
        std::vector<int> codici, varianti;
        double minuti = 0.0;         // contatto: minuti cumulativi per unita'; zona_inviolata: tolleranza
        double distanza_m = -1.0;    // foto: distanza massima; raggiungere un'unita': la distanza da lei
        Lato parte = Lato::ostile;   // zona_inviolata: la parte che non deve entrare (rispetto al giocatore)
        std::string missione;        // tlam: la missione della libreria (vuota: qualunque)
        bool finestra = true;        // tlam: conta solo l'arrivo entro tot +- finestra
        int scatola_max = 2;         // scatola: la taglia massima permessa (0 piccola, 1 media, 2 grande)
        double errore = 0.0;         // telemetro: errore relativo massimo (0,1 = 10%); rotta_stimata: gradi
        bool da_tma = false;         // rotta_stimata: le stime della TMA del giocatore, anche ripetute sulla stessa unita'
    };
    struct InfoUnita {
        std::string classe, categoria;
        int flag = 0;
        Lato lato = Lato::nessuno;
        CategoriaRoe roe = CategoriaRoe::navi;
        bool arma = false;
        PosaEvento ultima;
        // armi: dirette sul giocatore e da chi
        int lanciatore = -1;
        bool minaccia = false, agganciato = false;
        double distanza_min_m = 1e18;
    };
    // osservazione (Missione::avanza) e ganci
    void carica(lua_State* L, int tabella, const std::string& file_missione, const std::string& id);
    void osserva(const Missione& m, double dt);
    void radio(const Missione& m, const EventoRadio& e);
    void giudizio(const Missione& m, const GiudizioRoe& r);
    void trasmissione(const Missione& m, TipoTrasmissione t, const EsitoTrasmissione& e, Vec2 da, bool boa);
    void classifica(const Missione& m, int giocatore, int contatto, const std::string& classe, bool visiva = false);
    void danni(const Missione& m, const DanniSottomarini* sub, const DanniMissione* navi);

    void ingresso(const Missione& m, const std::string& zona);
    void lancio_senza_arma(const Missione& m, const std::string& classe_arma);   // TLAM-C: nessuna unita' in mare
    void esito_tlam(const Missione& m, const std::string& missione, const std::string& bersaglio, bool arrivato, double volo_s, Vec2 pos,
                    bool in_finestra = true);
    void fine_missile(const Missione& m, int lanciatore, int missile, const std::string& classe, bool guasto, double volo_s, Vec2 pos);
    // il missile fermato dalla difesa della nave: fine_missile col codice 2, lo scoppio in aria (Effetti::esplosione_aerea)
    void missile_abbattuto(const Missione& m, int lanciatore, int missile, const std::string& classe, int nave, Vec2 pos,
                           double quota_m, double esplosivo_kg);
    // mine (M2b): arrivata sul fondo (o inerte) e scoppiata o lanciato il suo siluro
    void mina_posata(const Missione& m, int posatore, const std::string& tipo, Vec2 pos, double quota, bool inerte);
    void mina_esplosa(const Missione& m, int posatore, int bersaglio, const std::string& tipo, Vec2 pos, double quota,
                      bool siluro, double carica_kg);

    // tempo < 0: quello della missione
    EventoMissione& aggiungi(const Missione& m, TipoEvento t, int unita = -1, int altra = -1, int arma = -1, double tempo = -1.0);
    void componi(EventoMissione& e) const;            // testi e inquadratura
    void in_terza_persona(EventoMissione& e) const;   // la sandbox: i testi del battello col suo nome
    void aggiorna_unita(const Missione& m);
    PosaEvento posa(int id) const;
    Lato lato_di(const Missione& m, const UnitaClassica& u) const;
    double quota_collasso(const UnitaClassica& g) const;

    std::vector<EventoMissione> eventi_;
    std::vector<DefObiettivo> definizioni_;
    std::map<int, std::string> categorie_;            // codice d'esito -> categoria
    bool obiettivi_dichiarati_ = false, avviato_ = false;
    std::map<int, InfoUnita> info_;
    int giocatore_ = -1;
    bool terza_persona_ = false;   // sandbox: lo spettatore guarda il battello da fuori
    std::set<int> scoperti_, classificati_, identificati_, affondati_, colpiti_dal_giocatore_;
    std::set<int> classificati_a_mano_;   // i contatti che il giocatore ha gia' classificato dalla stazione (il primo tentativo)
    std::map<int, double> prima_scoperta_;
    std::map<int, double> avvistato_;                 // unita' ostile -> ultima volta con la posizione del giocatore
    std::set<int> asw_con_contatto_;
    std::size_t colpi_visti_ = 0, sonar_visti_ = 0, esm_visti_ = 0, msg_visti_ = 0, siluri_visti_ = 0, asw_visti_ = 0, foto_viste_ = 0, lunghe_viste_ = 0, sonarista_visti_ = 0, scambi_visti_ = 0;
    std::size_t rapporti_visti_ = 0, emersioni_viste_ = 0, lanci_visti_ = 0;
    std::size_t impatti_art_visti_ = 0, eventi_art_visti_ = 0;
    std::size_t urti_visti_ = 0, quasi_visti_ = 0, forzate_viste_ = 0, cavi_visti_ = 0, mine_scoperte_viste_ = 0, avarie_viste_ = 0, rimorchi_visti_ = 0, acque_viste_ = 0, esercitazioni_viste_ = 0, classificazioni_ia_viste_ = 0, telemetro_visti_ = 0, aob_visti_ = 0, eco_visti_ = 0, allarmi_visti_ = 0, mine_att_viste_ = 0, verifiche_viste_ = 0, luci_visti_ = 0, punti_nave_ia_visti_ = 0, traini_visti_ = 0;
    std::map<int, int> interventi_visti_;   // id dell'intervento -> l'ultimo livello registrato (1, 2, 3)
    std::size_t posti_visti_ = 0, soffiate_viste_ = 0;
    std::set<int> salve_aperte_;                      // navi che hanno gia' aperto il fuoco
    std::set<std::pair<int, int>> scoperte_ia_, salve_ia_;   // fase 5: coppie gia' riferite
    std::map<int, int> agganci_ia_, evasioni_ia_;     // l'ultimo agganciato, le manovre d'evasione gia' riferite
    // «ci stanno pingando» per episodio (principale, 30/10/2026: 3393 righe in due ore nella S15): l'evento dell'episodio
    // in corso (indice) e l'ultimo ping; il conteggio in valore
    long episodio_ping_ = -1;
    double ultimo_ping_t_ = -1e18;
    double cadenza_ia_ = 0.0;
    std::size_t cambi_allerta_visti_ = 0, provocazioni_viste_ = 0, messaggi_ia_visti_ = 0, fuochi_amici_visti_ = 0;   // fase 3
    std::size_t esche_ia_viste_ = 0, ritirate_ia_viste_ = 0, tma_ia_viste_ = 0, reazioni_ia_viste_ = 0, incsea_viste_ = 0, punti_nave_visti_ = 0, note_comandante_viste_ = 0;   // fase 4, navigazione, sandbox
    bool artiglieria_vicina_ = false;
    std::set<int> armi_a_segno_, finite_, boe_vicine_;
    std::map<int, int> ultimo_colpitore_;             // vittima -> lanciatore dell'ultimo colpo
    std::set<std::size_t> raggiunti_;                 // definizioni `raggiungere` raggiunte
    struct Fotografata { int unita = -1; double tempo = 0.0, distanza_m = 0.0; };
    std::vector<Fotografata> fotografate_;            // ogni unita' nel campo di ogni foto del giocatore
    std::map<std::pair<std::size_t, int>, double> dentro_zona_s_;   // zona_inviolata: secondi dentro per unita'
    std::set<std::size_t> violate_;                   // definizioni `zona_inviolata` violate
    std::set<int> colpiti_esercitazione_;             // unita' colpite da un'arma da esercitazione del giocatore
    std::optional<GiudizioRoe> giudizio_in_attesa_;   // giudicato prima che l'arma compaia
    double ultimo_acustico_ = -1e18;                  // fine dell'ultimo evento acustico del giocatore gia' visto
    bool incaglio_ = false;
    int prossimo_id_ = 0;
    bool fine_ = false, scram_ = false, camera_persa_ = false;
    std::set<std::string> ordini_ricevuti_;           // id dei messaggi radio ricevuti che portano obiettivi
    std::map<std::string, int> serie_ordini_;
    // accumuli
    double esposto_s_ = 0.0, asw_s_ = 0.0, distanza_min_siluro_ = 1e9, margine_collasso_ = 1.0;
    double quota_collasso_danni_ = -1.0;              // dal modello dei danni, se c'e'
    double danno_massimo_ = 0.0;
    bool battello_perso_ = false, equipaggio_salvo_ = true;
    int rapporti_richiesti_ = 0, rapporti_inviati_ = 0, danni_domati_ = 0;
    std::vector<GiudizioRoe> giudizi_;
    std::vector<std::pair<double, std::string>> ingressi_;
    std::map<std::string, ContoArma> per_arma_;
};

}  // namespace m688
