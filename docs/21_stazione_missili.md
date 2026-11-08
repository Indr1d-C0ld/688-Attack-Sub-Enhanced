# 21 — Stazione di tiro dei missili dell'Enhanced (approvata)

*trailer-1, 28/10/2026. Stato: **approvata dall'utente il 28/10/2026** (decisioni al §10); contratti del modulo del
nucleo al §11. Ricerca: `docs/ricerca/27_missili_impiego.md` (nuovo, con gli appunti in `docs/ricerca/27_note/`) e
`docs/ricerca/05_missili_e_contromisure.md` §1-§3.*

Il classico non cambia: tubo 7, controlli e messaggi 223-232, gestore della missione e ATTACK_RND restano quelli
dell'originale (manuale D3 §2.2-§2.3 e §4).

## 1. Che cosa c'è oggi e che cosa manca

Oggi nell'Enhanced il missile si lancia come nell'originale (manuale D3):
- M sceglie il tubo 7, G il bersaglio, L lancia;
- serve una traccia solida (qualità ≥ 800) su una nave ad almeno 5 miglia, da quota periscopica;
- il volo non c'è: dopo 20 s l'ATTACK_RND decide l'esito con il byte di difesa della classe;
- il TLAM-C è solo un evento del registro.

Manca l'impiego storico (roadmap 25, richiesta dell'utente del 28/10):
- i dati di tiro veri: rilevamento sempre, distanza se c'è, da dove arrivano e quanto valgono;
- RBL e BOL, la distanza d'accensione del seeker, l'area di ricerca;
- il missile che vola e cerca, e che può agganciare la nave sbagliata, anche neutrale;
- i dati oltre l'orizzonte che arrivano dal broadcast;
- le missioni TLAM con l'area di lancio e la navigazione allineata;
- la stessa cosa per l'IA, per la simmetria.

## 2. Principi

1. **Il giocatore decide come un comandante del 1989.** Sceglie bersaglio, fonte dei dati, modo, accensione del
   seeker, ampiezza della ricerca e il momento. Non sceglie ciò che il 688 non poteva scegliere: niente waypoint
   sull'Harpoon, niente aggiornamento in volo, niente bersaglio sulla missione TLAM.
2. **Il missile sa solo ciò che il battello sapeva al lancio.** Parte con la stima della TMA o del rapporto radio,
   non con la verità. Il successo dipende dalla qualità dei dati, dall'età del dato e da chi c'è nell'area di ricerca.
3. **Tutto quello che il giocatore vede è sulla carta di tiro.** Punto previsto, accensione del seeker, area di
   ricerca, contatti nell'area, datum del lancio, orizzonte radar del bersaglio, area di lancio del TLAM.
4. **Simmetria.** L'IA usa lo stesso modello e le stesse regole, con i suoi dati (memoria «simmetria dell'IA»).
5. **Niente di più lento del necessario.** Le attese vere (allineamento, un'ora di preparazione del TLAM) restano,
   ma la compressione del tempo le attraversa e la stazione dice cosa manca.

## 3. La stazione: una pagina di F7

Quando il tubo scelto (o il pozzo, sul Flight II) contiene un missile, **F7 passa da sola alla pagina di tiro dei
missili**; con un siluro resta quella di oggi. Tab torna ai siluri (decisione 1 del §10).

Bozze (640×480, font e colori della stazione Armi di oggi; `tools/schermate/bozza_stazione_missili.py`):
- `docs/ricerca/27_note/f7_missili_antinave.png`: Harpoon in RBL sulla carta di tiro;
- `docs/ricerca/27_note/f7_missili_tlam.png`: missione TLAM-C con l'area di lancio.

### 3.1 Disposizione

| Zona | Contenuto |
|---|---|
| In alto (640×358) | **carta di tiro**: la carta della stazione Navigazione con gli strati del tiro (§4) |
| In basso a sinistra | consolle di tiro: **MK 117 FIRE CONTROL** sul Flight I, **CCS MK 1** sul Flight II, **BSY-1** sul 688I; testo al fosforo, una riga per parametro |
| In basso a destra | **pannello di lancio** (Weapons Launch Console): per tubo o pozzo le spie PWR, ALN, RDY, DOOR, SLIDE; il pulsante FIRE col coperchio |
| Barra | messaggi, come nelle altre stazioni |

### 3.2 Harpoon e TASM: i parametri

Riga per riga nella consolle (le frecce scelgono la riga, sinistra e destra cambiano il valore):

| Riga | Valori | Note |
|---|---|---|
| Bersaglio e fonte | contatto (G come oggi) e **fonte dei dati**: TMA, periscopio, ESM, rapporto radio | la fonte decide distanza ed errore (§5) |
| Rilevamento, distanza, rotta, velocità | dalla fonte; con il periscopio si possono correggere a mano | una distanza mancante obbliga al BOL |
| Modo | **RBL** o **BOL** | RBL: vola alla distanza e cerca nella scatola; BOL: cerca a ventaglio, che si allarga, dall'accensione |
| Ricerca (RBL) | **piccola, media, grande** | la scatola si tara sull'incertezza della soluzione, come nel modello di Becker (NPS 1986): profondità ±1,7, ±2,5 o ±3,5 σ della distanza (minimo 5, 10, 15 km), larghezza la fascia del seeker contro la nave più grande del gruppo (STIME, dossier 27 §3.1) |
| Accensione e fine (BOL) | **SRCH**: distanza d'accensione dal lancio; **DSTR**: distanza d'autodistruzione, o fino alla portata massima | prima = più area, più ECM, più neutrali; dopo = serve un rilevamento buono |
| Terminale | solo informativo | nel 1989 dipende dal missile caricato: UGM-84A pop-up, UGM-84C radente; la scelta del Block 1C sui sottomarini arriva col CCS Mk 2 degli anni '90 |
| TASM | ricerca d'area a serpentina dal punto previsto, con l'ESM | gambe trasversali alla linea di tiro; l'ESM porta il missile verso il radar della classe voluta (§4) |
| Probabilità di successo | calcolata, non impostata | come il CCS vero (APL 1992: «calcola la probabilità di successo»): sulla soluzione **stimata**, con l'avviso dei contatti, anche neutrali, che cadono nella scatola |
| Controlli | quota, velocità, mare, distanza minima | in verde se va bene, in ambra se è fuori; un fuori-limite in rosso blocca il lancio |

### 3.3 La sequenza sul pannello

Il pannello è la **Weapons Launch Console** (WLC): l'ordine di fuoco va dalla consolle di tiro alla WLC e da lì al
pannello dei tubi del locale siluri (dossier 27 §2, dal brevetto USN dei simulatori degli SSN-751). Le spie sono i
prerequisiti; il pulsante si abilita solo con tutte verdi:
1. **PWR**: missile alimentato e riscaldato (Harpoon ~30 s);
2. **ALN**: riferimento d'assetto allineato con l'inerziale del battello e dati accettati (Harpoon 20-40 s; TASM 8-12
   min; TLAM dentro l'ora di preparazione);
3. **RDY**: dati di tiro validi (modo, ricerca, bersaglio dentro la portata, fuori dalla distanza minima);
4. **DOOR**: tubo allagato, equalizzato, porta esterna aperta, come per il siluro, con i transitori;
5. **SLIDE**: la slide valve del lato è libera: **una sola aperta per lato**, così due tubi dello stesso lato partono a
   15-25 s l'uno dall'altro, due di lati opposti a 3-5 s;
6. **FIRE**: Shift+L alza il coperchio, L lancia. Nessuna chiave CO (decisione 2 del §10).

Tempi: dossier 27 §2, STIME dove mancano. Un Harpoon con la soluzione pronta è in tubo in 60-90 s; un tubo già pronto
lancia in 1-2 minuti.

### 3.4 TLAM: i parametri

Nel 1989 tutto si pianifica a terra (CMSA, 24-80 ore; la pianificazione imbarcata arriva nel 1996): il battello parte
con un **pacchetto di missioni** e può riceverne via radio, ma un bersaglio nuovo deciso in mare non si attacca in
giornata (dossier 27 §4).

| Riga | Contenuto |
|---|---|
| Missione | dalla **libreria di bordo**: punto di lancio pianificato (PLP), bersaglio cifrato, ora sul bersaglio e finestra |
| Stato | **VALIDA** (entro 10 mi dal PLP), **RIPIANIFICA** (10-40 mi: il CCS ricalcola la tratta sul mare, +10 min, meno carburante), **NON VALIDA** (oltre, o la tratta passa su terra o zone proibite), **FUORI FINESTRA** (±10 min) — STIME |
| Preparazione | primo missile 15 min, poi 3-5 min ciascuno; si sovrappone al trasferimento; con l'ordine da decodificare circa un'ora in tutto (GAO) |
| Navigazione | l'ellisse della posizione stimata, l'ultimo punto nave e la **probabilità stimata di prendere la prima mappa TERCOM** |
| Ordine | l'ordine di esecuzione ricevuto al broadcast: senza ordine i missili si preparano ma non partono |
| Controlli | quota, velocità |

**Il punto chiave.** Il pannello calcola stato e probabilità sulla posizione **stimata**; l'esito usa quella **vera**.
L'errore del battello passa uno a uno al missile, e la tratta sul mare non lo corregge (il TERCOM non funziona
sull'acqua): un equipaggio «sicuro di sé e sbagliato» vede «VALIDA, 96%» e manca il landfall. Il replay «verità» lo
mostra. Un punto nave prima del lancio costa esposizione e compra probabilità (dossier 27 §4, formula del landfall).

Il comandante decide il percorso fino all'area, **quando** lanciare dentro la finestra, **se** rinviare o rinunciare
per la sicurezza del battello, e dà il fuoco. Non decide il bersaglio, la missione né il numero di missili per
bersaglio. Il TLAM-N resta fuori (dossier 14); se un giorno servisse, solo dopo un ordine autenticato a due persone,
senza PAL (dossier 27 §4).

### 3.5 Tasti

Decisi dalla principale il 28/10/2026, contro la tabella dei tasti di oggi: P è la pausa, R il tubo 1, + e − il
telegrafo, quindi la pagina non ha tasti propri per fonte, modo e preparazione. Sono righe della consolle.

| Tasto | Azione |
|---|---|
| F7 | stazione Armi; con un missile nel tubo scelto apre la pagina di tiro |
| Tab | torna alla pagina dei siluri; la pagina dei missili si riapre da sola riscegliendo un tubo col missile, o con F7 |
| 1-4, 7 | tubo, come oggi |
| G | bersaglio, come oggi |
| frecce su e giù | la riga della consolle: FONTE, DISTANZA (solo col periscopio), MODO, RICERCA (RBL) o SRCH e DSTR (BOL), PREPARAZIONE; sul TLAM MISSIONE e PREPARAZIONE |
| frecce sinistra e destra | il valore della riga: la fonte a giro (TMA, periscopio, ESM, radio); la distanza dello stadimetro ±500 m; RBL o BOL; piccola, media, grande; SRCH e DSTR a passi di un miglio (0 = predefinita); la preparazione: destra avvia, sinistra spegne |
| Maiusc+L | alza o abbassa il coperchio del FIRE |
| L | lancia, solo a coperchio alzato (altrimenti «COPERCHIO CHIUSO»); passa per `StazioneArmi::lancia`, che col missile vuole RDY e la slide libera. Dopo il lancio il coperchio si riabbassa, e così al cambio di tubo |
| I, O | ingrandisce o allarga la carta di tiro: scale fisse di 2, 5, 10, 20, 50, 100, 200 km sulla larghezza (all'apertura è automatica: battello, area di ricerca e missili in volo) |
| C | centra la carta sul bersaglio o sull'area di lancio, a giro |

Col mouse: ogni riga che si sceglie e il FIRE sono aree cliccabili (`StazioneMissili::disegna_pannello`).

## 4. La carta di tiro

Gli strati sopra la carta della stazione Navigazione (stessa proiezione e stessi simboli del kit):

| Strato | Disegno | Quando |
|---|---|---|
| Stima del bersaglio | l'ellisse della TMA, o quella del rapporto radio in un altro colore, che si gonfia dall'ora della posizione | sempre |
| Punto previsto | croce gialla al tempo di volo, col vettore dalla stima | RBL |
| Rotta del missile | linea dal battello | sempre |
| Accensione del seeker | punto giallo sulla rotta | sempre |
| Area di ricerca | RBL: rettangolo sul punto previsto; BOL: ventaglio dal punto d'accensione; TASM: la serpentina | sempre |
| Contatti nell'area | i contatti del giocatore che cadono nell'area, con un avviso sui neutrali | sempre |
| Orizzonte radar del bersaglio | cerchio tratteggiato: dove il missile in crociera radente si fa vedere | antinave |
| Datum del lancio | cerchio di 3 miglia sul punto di lancio: dove arriverà l'ASW | sempre |
| Area di lancio | cerchio della missione, rotta di uscita, landfall e TERCOM | TLAM |
| Missili in volo | il punto stimato di ogni missile, tratteggiato e marcato «STIMA» (dal tempo di volo, non dalla verità; decisione 4 del §10) | dopo il lancio |

Dopo il lancio la carta di tiro e la stazione Navigazione mostrano i missili in volo come stima. L'esito arriva con
i messaggi dell'originale (50 a segno), più un messaggio nuovo «missile ha agganciato» se il bersaglio è entro
l'orizzonte del periscopio o se un rapporto lo riferisce.

## 5. Legami con le altre stazioni

- **TMA (stazione Navigazione).** La soluzione MATE dà distanza, rotta e velocità con la loro qualità. La qualità
  stima l'errore del punto previsto; la consolle suggerisce RBL se l'errore sta nell'area di ricerca, BOL se no.
- **Radio.** I rapporti di contatto che arrivano col broadcast (oggi `RapportoContatto` con l'ellisse r0 + u·t, fase B
  del docs/18) diventano bersagli scelti con F = radio. Nel 1989 la catena è sensori → OSIS → Shore Targeting Terminal
  del comando sommergibili → SSIXS, con un dato nuovo una o due volte l'ora e decine di minuti di ritardo. Il
  messaggio in sala radio ha lo stile OTH-T Gold (righe `CTC`, `POS` con l'ora della posizione, `RMKS`; ricostruzione,
  dossier 27 §4). L'ellisse si gonfia dall'**ora della posizione**: con rotta e velocità note cresce lenta, senza
  cresce col raggio Vmax·t. Un dato con rotta e velocità vale più di uno fresco senza; aspettare il broadcast
  successivo è una scelta vera. Senza designazione esterna il TASM era utile solo sotto le 50 miglia. Gli ordini di
  esecuzione TLAM e le missioni nuove arrivano alla radio.
- **ESM (WLR-8).** Il rilevamento di un radar dà un bersaglio BOL anche senza sonar. Il TASM usa il database degli
  emettitori (`nucleo/elettronica.hpp`, dossier 11): un bersaglio in EMCON è più difficile, un mercantile col radar di
  navigazione acceso attira il seeker passivo.
- **Navigazione incerta (docs/08 §6).** Il missile parte dalla posizione **stimata** del battello. Contro un bersaglio
  del proprio sonar l'errore non conta (la geometria è relativa). Contro un dato OTH, in coordinate geografiche, si
  somma all'ellisse del rapporto: un DMINS a tre giorni dal punto nave sbaglia più del satellite. Per il TLAM decide la
  validità vera e la presa della prima mappa TERCOM. È il motivo, leggibile, per fare un punto nave prima di un tiro
  OTH o TLAM.
- **Traffico neutrale (docs/20 B).** Le navi del traffico sono contatti veri: se cadono nell'area di ricerca il seeker
  le può agganciare. È il modo storico in cui il TASM colpiva i neutrali.
- **Periscopio.** Distanza dallo stadimetro come fonte; la capsula, il booster e l'impatto si vedono già.
- **Contro-scoperta.** Transitori del tubo e della capsula (già nel nucleo), booster visibile, missile al radar dentro
  l'orizzonte, seeker acceso all'ESM nemico, datum.

## 6. Il modello del missile (runtime)

Il missile diventa un'unità della missione (oggi è solo il gestore dello script). In breve; i numeri nel dossier 27:
1. **Lancio:** espulsione, salita della capsula senza motore (Harpoon) o booster sott'acqua (Tomahawk), affioramento,
   boost, discesa alla quota di crociera.
2. **Crociera** sulla rotta del lancio, dalla posizione **stimata**, senza correzioni.
3. **Accensione del seeker** alla distanza impostata.
4. **Ricerca** nell'area del modo. Il seeker DSQ-28 è un radar a ricerca sequenziale senza classificazione: aggancia
   **il primo eco valido nel cancello**, non il bersaglio designato (dossier 27 §3). È questo che fa colpire una scorta
   o un mercantile; il TASM ha in più l'ESM che lo porta verso il radar della classe voluta.
5. **Terminale** e colpo; su una nave il missile passa prima dalle difese: il canale col byte scalato con l'allerta, poi il
   Phalanx (manuale D3 §2.6).
6. **Esaurimento:** senza aggancio il missile finisce il carburante e cade.

Il TLAM resta un evento senza volo sul mare visibile: lancio, tempo di volo stimato fino al bersaglio, esito dal
successo della missione (navigazione e area di lancio, affidabilità 95% del GAO, precisione).

## 7. L'IA

L'IA spara solo con la **sua** soluzione (la sua TMA, con l'errore che avrebbe il giocatore, o un dato esterno con
età ed errore), mai con la verità; stessi tempi, stessi inviluppi, stessa regola del primo eco (dossier 27 §5).
- **Missili antinave sovietici** (Charlie II col Rubikon, Oscar con Legenda e Uspekh tramite il Korall): stesso modello
  di ricerca e aggancio. L'Oscar II lancia fino a 50 m e 5 kn, un missile ogni 5 s, in 3 salve.
- **Vodopad e Veter** (Victor III, Akula): armi a punto fisso. Omnibus calcola il punto d'ammaraggio dalla TMA e lo
  inserisce prima del lancio; nessuna correzione. L'83R vuole un errore di distanza ≤ 1,5 km (≤ 3 km con due missili);
  il Veter accetta dati esterni vecchi al più 10 minuti. Il siluro ammara vicino alla **stima** e gira a sinistra
  finché aggancia. Il giocatore vede la minaccia nella TMA lenta del contatto, nel ping o nel transitorio, e la batte
  cambiando rotta e velocità.
- **Victor III e Akula non hanno ricevitori per Legenda e Uspekh**: la designazione esterna arriva come radiogramma, in
  ritardo, come per il giocatore.
- **Vyuga** sull'Alfa: solo nucleare, solo con regole nucleari.
- **Granat**: solo strategico, contro terra; nel gioco una minaccia di scenario, mai contro il 688.
- **L'IA americana** (Sturgeon, Los Angeles della libreria 2) usa l'Harpoon col modo naturale del sottomarino, BOL dal
  sonar, e la stessa scatola.

## 8. Chi fa cosa e costi

Taglie: **S** fino a 2 giorni, **M** fino a una settimana, **L** oltre.

| Lavoro | Chi | Taglia |
|---|---|---|
| Modello del missile antinave (unità, lancio, crociera, seeker, ricerca, aggancio, esaurimento) | runtime | L |
| Dati di tiro: fonte, errore, previsto, regole di validità; interfaccia per la stazione | runtime | M |
| TLAM: libreria di bordo, area di lancio, preparazione, esito | runtime | M |
| Missioni e ordini TLAM dalla radio; rapporti OTH come bersagli | runtime (sala radio) | S |
| IA: uso simmetrico (antinave, Vodopad) | runtime | M |
| Pagina di F7, consolle, pannello di lancio, tasti | principale | M |
| Carta di tiro: strati sulla carta di Navigazione | principale | M |
| Disegni: pannello di lancio, spie, coperchio del FIRE, simboli della carta di tiro (previsto, area, missile in volo) | kit | S |
| Manuale (D3 e cap. 6), figure | kit | S |
| Database emettitori per il seeker ESM del TASM | disassemblatore (dossier 11 già fatto) | S |
| Missioni della libreria e dell'editor con missili, prove di scena | trailer-1 | M |

Ordine consigliato:
1. Harpoon antinave con TMA e RBL/BOL (il caso più comune); prove sul volo e sull'aggancio sbagliato.
2. Rapporti radio come fonte, TASM oltre l'orizzonte.
3. TLAM con l'area di lancio.
4. IA simmetrica.

## 9. Cosa resta nel classico

Niente cambia: tubo 7, controlli 226-232, gestore, 20 s e ATTACK_RND. La pagina di tiro esiste solo nell'Enhanced.

## 10. Decisioni dell'utente (28/10/2026)

La proposta è approvata, con queste risposte alle quattro domande:
1. **La pagina di tiro si apre da sola** quando il tubo scelto ha un missile; Tab torna ai siluri.
2. **Niente chiave CO:** basta il coperchio del FIRE. La chiave resta solo per un futuro TLAM-N.
3. **L'ora di preparazione del TLAM resta vera sempre**, comprimibile col tempo, anche nella partita rapida.
4. **Il missile in volo si vede sulla carta come stima** tratteggiata, dal tempo di volo, marcata «STIMA».

## 11. Contratti del modulo del nucleo

*trailer-1, 28/10/2026. Da far vedere al runtime (aggancio alla missione) e alla principale (pagina di tiro e carta).*

Divisione decisa dalla principale il 28/10: **trailer-1 scrive il modulo puro** `nucleo/missili`, con le sue prove;
il runtime lo aggancia alla missione quando ha finito l'errore di posizione dell'IA e il traffico; la principale fa la
pagina di F7 e la carta di tiro. Il modulo non conosce la missione, come `nucleo/siluro` e `nucleo/navigazione`: ogni
passo riceve ciò che il mondo sa e restituisce stato ed eventi. Il classico non lo usa.

### 11.1 File

**Miei (nuovi):**
- `engine/nucleo/include/nucleo/missili.hpp`, `engine/nucleo/src/missili.cpp`;
- `engine/test/prove_missili.cpp`;
- `tools/pipeline/missili.py` → `data/motore/missili.toml` (classi di missile, con fonte e confidenza per campo).

**Condivisi, da annunciare quando li tocco:**
- `engine/CMakeLists.txt`: `nucleo/src/missili.cpp` nella libreria del nucleo, `test/prove_missili.cpp` nelle prove;
- `tools/pipeline/esporta_motore.py`: la chiamata a `esporta_missili` e la riga nell'elenco dei file.

**Non miei:** `missioni/*` e `missione.*` (runtime); `app/*` e la stazione Armi (principale); `data/motore/emettitori.toml`
e `nucleo/elettronica` (disassemblatore: il DSQ-28 c'è già, ruolo `testa_missile`); disegni e manuale (kit).

### 11.2 Unità e convenzioni

Come il resto del nucleo: metri, secondi, m/s, gradi bussola (0 = nord, orario), x verso est e y verso nord
(`nucleo/geometria.hpp`). Le ellissi sono `EllisseErrore` di `nucleo/navigazione.hpp`, a 1 σ. Deterministico dal seme.

**Il principio che regge tutto:** il modulo lavora in due mondi. Il **piano** usa solo ciò che il battello sa (la sua
posizione stimata, la soluzione del bersaglio, i contatti che conosce). Il **volo** parte dalla posizione vera, ma
segue la rotta calcolata nel piano: la differenza fra stimata e vera al lancio sposta tutta la geometria, come nel
missile vero.

### 11.3 Dati: `CatalogoMissili`

```cpp
enum class Lancio : std::uint8_t { capsula, booster_subacqueo, pozzo, contenitore };   // Harpoon, Tomahawk da tubo, VLS, sovietici
enum class Terminale : std::uint8_t { radente, pop_up };
enum class Ruolo : std::uint8_t { antinave, attacco_terra, asw };

struct DatiMissile {
    std::string id, nome, fonte, confidenza;   // harpoon_84a, harpoon_84c, tasm, tlam_c, p700, p120, p70, ...
    Ruolo ruolo = Ruolo::antinave;
    Lancio lancio = Lancio::capsula;
    double velocita_m_s, quota_crociera_m, portata_m, distanza_min_m;
    double quota_lancio_max_m, velocita_lancio_max_kn;     // oltre: avviso, poi guasto in salita (dossier 05 §1.3)
    double salita_s, boost_s, discesa_s;                   // capsula 10-30 s, booster 12 s, discesa 10-20 s
    Terminale terminale = Terminale::radente;              // dal missile, non si sceglie (1989)
    double pop_up_m = 0.0;
    // seeker: portata d'acquisizione per dimensione del bersaglio (piccola, media, grande; dossier 27 §3.1)
    std::array<double, 3> acquisizione_m{};
    double semiapertura_gradi = 45.0;
    int mare_max = 5;                                      // DSQ-28: ±45°, efficace fino a forza 5
    double p_acquisizione_scorta = 0.4;                    // Becker: probabilità di prendere un eco che non è il più grande
    bool esm = false;                                      // TASM: ESM passivo sulle classi di radar programmate
    std::vector<std::string> esm_radar;                    // id dei radar che l'ESM cerca (emettitori.toml)
    std::string radar_seeker;                              // id in emettitori.toml (dsq28, granit, ...)
    double quota_ricerca_m = 0.0;                          // TASM: sale a ~100 m per cercare
    double testata_kg;
    double affidabilita = 0.95;                            // GAO 1995: 95% in crociera
    double accensione_s, allineamento_s;                   // PWR e ALN (Harpoon 30 s e 20-40 s; TASM 8-12 min)
    double preparazione_primo_s, preparazione_altri_s;     // TLAM: 15 min, poi 3-5 min ciascuno
    int anno_dal = 0, anno_al = 9999;
};
struct CatalogoMissili {
    std::map<std::string, DatiMissile> missili;
    const DatiMissile* missile(const std::string& id) const;   // nullptr se manca
    const DatiMissile* per_arma(const std::string& arma, int anno) const;       // [[arma]]: harpoon -> 84A o 84C
    const DatiMissile* per_classe(const std::string& classe, int anno) const;   // [[lanciatore]]: oscar -> p700
};
CatalogoMissili carica_missili(const std::string& percorso);   // std::runtime_error se manca o è incompleto
```

Le famiglie del carico (`harpoon`, `tomahawk`, `attacco_terra`) restano quelle di `data/motore/carico.toml`. Una tabella
nel file dice quale classe di missile corrisponde a una famiglia per anno: `harpoon` vale l'84A o l'84C, `tomahawk`
il TASM.

Un'altra tabella, `[[lanciatore]]`, dice quale missile antinave tira una classe dell'IA in un anno. Serve al MISSILE
degli script col modello, passo 4 (runtime, 28/10/2026):
- **Sovietici:** Oscar e Kirov il Granit, Slava il Bazalt, Charlie II e Nanuchka il Malakhit.
- **Americani:** i Los Angeles, gli Sturgeon, i Trafalgar, le Perry, le Spruance, le Kidd e le Iowa l'Harpoon dell'anno.
- **Fuori:**
  - Kara e Krivak: l'SS-N-14 è antisom;
  - Victor III e Akula: il Granat è strategico, Vodopad e Veter sono antisom;
  - Alfa: il Vyuga è solo nucleare.

Le navi usano la stessa classe dell'UGM-84 (STIMA).

### 11.4 Il bersaglio come lo sa il battello

```cpp
enum class FonteDati : std::uint8_t { tma, periscopio, esm, radio };

struct BersaglioStimato {
    FonteDati fonte = FonteDati::tma;
    int contatto = -1;                  // l'id del contatto del giocatore (o dell'IA), per i messaggi
    double t_pos = 0.0;                 // l'ora della posizione (per la radio: quella del rapporto, non dell'arrivo)
    Vec2 pos;                           // posizione stimata a t_pos (geografica: per la radio è nel piano del teatro)
    bool distanza_nota = true;          // falso: solo rilevamento (ESM, TMA inosservabile) -> solo BOL
    double rilevamento = 0.0;           // dal battello, quando la distanza non è nota
    bool moto_noto = false;             // rotta e velocità note
    double rotta = 0.0, velocita_m_s = 0.0;
    EllisseErrore ellisse;              // a t_pos, 1 σ
    double sigma_velocita_m_s = 1.0, sigma_rotta_deg = 10.0, velocita_max_m_s = 15.0;   // per la crescita
    bool relativo = true;               // vero: misurato dal battello (TMA, periscopio, ESM); falso: geografico (radio)
};

// L'ellisse del bersaglio a un istante, dall'ora della posizione (dossier 27 §4.1, appunti OTH §3.2):
// con il moto noto cresce lungo e attraverso la rotta; senza, il raggio cresce come velocita_max * età
EllisseErrore ellisse_a(const BersaglioStimato& b, double t);
Vec2 posizione_a(const BersaglioStimato& b, double t);        // la stima proiettata col moto noto
// Somma di due ellissi indipendenti (covarianze): per un dato geografico vi si aggiunge l'ellisse di navigazione del
// battello; per un dato relativo no (la geometria è relativa)
EllisseErrore somma(const EllisseErrore& a, const EllisseErrore& b);
```

Il runtime costruisce `BersaglioStimato`:
- dalla soluzione MATE (`SoluzioneTma` più la sua qualità, convertita in σ di distanza, rotta e velocità);
- dal periscopio, con lo stadimetro;
- dall'ESM (`Intercettazione`, solo rilevamento);
- dal quadro radio (`ContattoRadio`: posizione, `t_fix`, raggio dell'ellisse, rotta e velocità se ci sono).

La conversione sta nel runtime. Il modulo offre `da_tma(...)` e `da_rapporto(...)` come aiuti puri, se li si vuole.

### 11.5 Il piano di tiro (per la consolle e per la carta)

```cpp
enum class ModoLancio : std::uint8_t { rbl, bol };
enum class Taglia : std::uint8_t { piccola, media, grande };

struct ImpostazioniMissile {
    ModoLancio modo = ModoLancio::rbl;
    Taglia taglia = Taglia::media;      // RBL
    double srch_m = 0.0, dstr_m = 0.0;  // BOL: accensione e autodistruzione dal lancio (0 = fino alla portata)
};

struct StatoLanciatore {
    Vec2 pos;                           // STIMATA (il piano non vede la verità)
    EllisseErrore navigazione;          // l'ellisse di navigazione dichiarata
    double quota_m = 0.0, velocita_kn = 0.0, rotta = 0.0;
    int stato_mare = 3;
    int anno = 1989;
};

struct ContattoNoto {                   // i contatti del battello, per l'avviso e la probabilità
    int id = -1;
    Vec2 pos;                           // stimata
    EllisseErrore ellisse;
    int dimensione = 1;                 // 0 piccola, 1 media, 2 grande (dalla classe creduta; 1 se ignota)
    bool neutrale_creduto = false;
};

enum class Avviso : std::uint8_t { fuori_portata, troppo_vicino, quota, velocita, mare, solo_bol, dato_vecchio,
                                   neutrali_in_area, rotta_ignota };

struct AreaRicerca {                    // in coordinate del teatro, come il battello la crede
    enum class Forma : std::uint8_t { scatola, ventaglio, serpentina } forma;
    std::vector<Vec2> contorno;         // poligono chiuso da disegnare
    std::vector<Vec2> percorso;         // serpentina del TASM; vuoto per gli altri
};

struct PianoTiro {
    bool valido = false;                // nessun avviso bloccante
    std::vector<Avviso> avvisi;         // nell'ordine di gravità; i bloccanti prima
    Vec2 lancio;                        // stimato
    double rotta = 0.0;                 // del missile, vera nel mondo del battello
    double tempo_volo_s = 0.0;          // fino al punto previsto
    Vec2 previsto;                      // la stima del bersaglio all'arrivo (RBL)
    EllisseErrore ellisse_arrivo;       // del bersaglio all'arrivo, già sommata alla navigazione se il dato è geografico
    Vec2 accensione;                    // dove si accende il seeker
    AreaRicerca area;
    double eta_dato_s = 0.0;            // età del dato all'accensione (radio)
    std::vector<int> in_area;           // contatti noti che cadono nell'area, nell'ordine in cui il seeker li vedrebbe
    std::vector<int> neutrali_in_area;  // quelli di in_area creduti neutrali (ContattoNoto::neutrale_creduto): la «N» sulla carta
    double p_successo = 0.0;            // stimata (APL 1992), senza le difese delle navi
    double orizzonte_radar_m = 0.0;     // del bersaglio contro il missile in crociera
};

PianoTiro pianifica(const DatiMissile& m, const ImpostazioniMissile& imp, const BersaglioStimato& b,
                    const StatoLanciatore& l, const std::vector<ContattoNoto>& contatti, double t);
```

- **Scatola RBL:** rettangolo centrato sul previsto e orientato sulla rotta. La profondità vale ±1,7, ±2,5 o ±3,5 σ
  della distanza lungo la rotta (minimo 5, 10, 15 km); la mezza larghezza vale le stesse σ attraverso la rotta
  (minimo 3, 6, 10 km). Becker e appunti 27, STIMA.
- **Ventaglio BOL:** dal punto d'accensione (srch_m), da ±15° e 5 km che crescono di 7,5° e 2,5 km ogni 10 s, fino a
  ±45° e 20 km; finisce a dstr_m o alla portata.
- **Serpentina TASM:** salita a 40 km dal previsto, gambe trasversali lunghe 40 km e spaziate 25 km, finché c'è
  carburante. Il percorso serve alla carta e al volo.
- **p_successo** = P(il bersaglio è nell'area) × P(è il primo eco fra i contatti noti nell'area, dal più vicino; ogni
  eco più vicino «ruba» con p_acquisizione_scorta, o 0,97 se più grande del bersaglio) × affidabilità. Le difese delle
  navi non entrano: la probabilità resta senza di loro (manuale D3 §2.6 e §7).

**Alla vista (principale):** tutto `PianoTiro`; il testo delle righe lo fa la vista. Le costanti per la carta: il raggio
del datum (3 mi) e l'orizzonte radar.

### 11.6 La preparazione (spie PWR, ALN, RDY)

```cpp
enum class FasePreparazione : std::uint8_t { spento, accensione, allineamento, pronto };
class PreparazioneMissile {
public:
    // TLAM: primo = nessun altro TLAM di bordo gia' pronto o in preparazione (15 min), altrimenti preparazione_altri_s
    explicit PreparazioneMissile(const DatiMissile& m, bool primo = true);
    void avvia();                       // la riga PREPARAZIONE della consolle
    void avanza(double dt);
    void spegni();
    FasePreparazione fase() const;
    double residuo_s() const;           // per la fase in corso
    bool pwr() const;                   // le spie
    bool aln() const;
};
// TLAM: i missili si preparano in parallelo dopo il primo (15 min, poi 3-5 min ciascuno)
double preparazione_tlam_s(const DatiMissile& m, int n_missili);
```

DOOR e SLIDE restano del tubo, come oggi (stazione Armi; la slide valve per lato è nuova nel runtime). RDY è
`PianoTiro::valido` più ALN; per il TLAM la missione valida, l'ordine d'esecuzione e ALN.

Il TLAM non ha accensione e allineamento propri. La preparazione del pacchetto dura `preparazione_primo_s` (15 minuti)
per il primo missile di bordo e `preparazione_altri_s` (4 minuti, STIMA) per gli altri: un terzo per l'accensione e due
terzi per l'allineamento (28/10/2026). L'ora resta vera sempre, anche nella partita rapida (decisione 3 del §10).

### 11.7 Il volo

```cpp
enum class FaseMissile : std::uint8_t { salita, boost, crociera, ricerca, inseguimento, terminale, impatto,
                                        esaurito, guasto };

struct EcoNave {                        // la VERITÀ: ogni unità di superficie del mondo, a ogni passo
    int id = -1;
    Vec2 pos;
    double rotta = 0.0, velocita_m_s = 0.0;
    int dimensione = 1;                 // dalla classe (lunghezza): 0 sotto 90 m, 1 fino a 180, 2 oltre (STIMA)
    double altezza_m = 20.0;            // sovrastruttura, per l'orizzonte radar
    std::vector<std::string> radar_accesi;   // id dei radar accesi (per l'ESM del TASM)
};

struct EventoMissile {
    enum class Tipo : std::uint8_t { affioramento, boost, crociera, seeker_acceso, aggancio, perdita, terminale,
                                     impatto, esaurito, guasto };
    Tipo tipo;
    double tempo = 0.0;
    int nave = -1;                      // aggancio, perdita, impatto
    Vec2 pos;
};

class Missile {
public:
    // `piano` è quello del battello; `lancio_vero` la posizione vera del lanciatore: lo scarto sposta tutto il volo
    Missile(const DatiMissile& m, const ImpostazioniMissile& imp, const PianoTiro& piano, Vec2 lancio_vero,
            double quota_lancio_m, double velocita_lancio_kn, int lanciatore, std::uint64_t seme);
    void avanza(double dt, const std::vector<EcoNave>& navi, int stato_mare);
    std::vector<EventoMissile> eventi();     // dall'ultima chiamata (si svuota)

    FaseMissile fase() const;
    bool attivo() const;
    Vec2 pos() const;                   // vera
    double quota() const;
    double rotta() const;
    int bersaglio() const;              // la nave agganciata, -1 senza
    bool seeker_acceso() const;         // per l'ESM delle navi: il radar `radar_seeker` emette
    double percorso_m() const;
};

// La stima del missile per la carta (decisione 4 del §10): dal piano e dal tempo dal lancio, mai dalla verità
struct StimaVolo { Vec2 pos; FaseMissile fase; };
StimaVolo stima_volo(const PianoTiro& p, const ImpostazioniMissile& imp, const DatiMissile& m, double t_dal_lancio);
```

- **Regola d'aggancio:** a ogni passo dopo l'accensione il seeker prende le navi dentro l'area e il settore, entro
  l'orizzonte radar e la portata per dimensione, e con il mare entro il limite. Le esamina **dal più vicino**: la
  prima viene presa con probabilità 1 se è la più grande dell'area, altrimenti con `p_acquisizione_scorta`; se
  scartata, si passa alla successiva. **Nessuna distinzione fra nemiche e neutrali.**
- **TASM con l'ESM:** se una nave ha acceso uno dei radar programmati, il missile vira sul suo rilevamento e
  aggancia l'eco più vicino entro ±5°.
- **Esaurimento:** senza aggancio a dstr_m o a fine carburante; evento `esaurito`.
- **Impatto:** il modulo dà solo `impatto` con la nave. Danno, testata e difese sono della missione (§11.9).

### 11.8 TLAM

```cpp
struct MissioneTlam {                   // dalla libreria di bordo (file della missione)
    std::string id, bersaglio;          // bersaglio: nome del bersaglio a terra (cifrato in consolle)
    Vec2 plp;                           // punto di lancio pianificato
    Vec2 landfall;                      // primo waypoint pianificato (prima mappa TERCOM)
    double tot_s = 0.0, finestra_s = 600.0;
    double tratta_terra_m = 0.0;
    double mezza_mappa_m = 2.5 * MIGLIO;   // H, STIMA
};
enum class ValiditaTlam : std::uint8_t { valida, ripianifica, non_valida, fuori_finestra };
struct StatoTlam {
    ValiditaTlam validita;
    double distanza_plp_m = 0.0;
    double ripianifica_s = 0.0;         // +10 min se ripianifica
    double p_landfall = 0.0;            // presa della prima mappa: 1 - exp(-H² / 2σ²)
    double tempo_volo_s = 0.0;
};
// Due chiamate, stessa funzione: con la stimata e l'ellisse dichiarata (pannello) e con la vera e l'errore vero (esito)
StatoTlam valuta_tlam(const DatiMissile& m, const MissioneTlam& t, Vec2 pos, double sigma_nav_m, double t_ora);
struct EsitoTlam { bool arrivato = false; double t_arrivo = 0.0; Vec2 caduta; };   // caduta: dove finisce se si perde
EsitoTlam esito_tlam(const DatiMissile& m, const MissioneTlam& t, const StatoTlam& vero, double t_lancio,
                     std::uint64_t seme);
```

L'ordine di esecuzione è della missione (radio): senza ordine RDY non si accende.

### 11.9 Che cosa chiede alla missione (runtime)

1. **Al piano:** `BersaglioStimato` dal contatto scelto (§11.4); `StatoLanciatore` con la posizione **stimata** e
   l'ellisse di navigazione; i `ContattoNoto` del battello.
2. **Al lancio:** crea un `Missile` con `lancio_vero`; i transitori del lancio ci sono già (`transitori_lancio`);
   l'evento EMCON «lancio di missili» e il datum come oggi.
3. **A ogni passo:** le `EcoNave` di tutte le unità di superficie (verità), con `dimensione` e `altezza_m` dalla
   classe e i radar accesi da `Emcon`/`elettronica`; lo stato del mare.
4. **Sugli eventi:**
   - `seeker_acceso`: il radar `radar_seeker` emette per l'ESM delle navi (l'emettitore c'è già);
   - `impatto`: su una nave in tabella le tre regole di `nucleo/difese_navi.hpp` (manuale D3 §2.6); sulle classi fuori
     tabella e su un sottomarino in superficie il byte `difesa` intero; se passa, il danno con la testata della classe di missile (`danni_missione`);
   - `esaurito` e `guasto`: nel registro.
5. **TLAM:** la libreria di bordo nel file della missione (un campo `tlam = { ... }` con i campi di `MissioneTlam`);
   l'ordine di esecuzione alla radio; l'esito con l'errore vero.
6. **IA:** gli stessi `pianifica` e `Missile` con la soluzione dell'IA (`tma_ia`); la soglia per sparare è un
   parametro del comportamento (per esempio `p_successo` ≥ 0,4 o l'errore di distanza sotto la soglia del dossier 27
   §5).
7. **Registro e traccia:** la traccia v2 registra i missili come unità (posizione vera), più gli eventi; il replay
   «verità» li mostra.

### 11.10 Che cosa dà alla vista (principale)

- `PianoTiro` per la consolle e la carta (§11.5): geometria, avvisi, probabilità.
- `PreparazioneMissile` per le spie PWR e ALN (§11.6), più DOOR e SLIDE dal tubo.
- `StimaVolo` per i missili in volo, tratteggiati e marcati «STIMA» (§11.7).
- `StatoTlam` per la pagina TLAM (§11.8).
- I nomi degli avvisi e delle fasi (`nome_avviso`, `nome_fase`) per la barra dei messaggi.

### 11.11 Prove (`prove_missili.cpp`)

- Il catalogo si carica, ogni campo ha fonte e confidenza, le classi del 1989 ci sono.
- Ellisse: con il moto noto cresce lenta, senza cresce come velocità massima × età; la somma delle covarianze.
- Piano: la scatola è centrata sul previsto e scala con la σ; il ventaglio BOL si allarga; la serpentina copre l'area;
  fuori portata e troppo vicino bloccano; `in_area` è ordinato dal più vicino.
- Probabilità: sale col dato migliore, scende con i contatti nell'area, è 0 fuori portata.
- Volo: con stimata uguale alla vera il missile arriva sul previsto; uno scarto di navigazione sposta l'impatto dello
  stesso vettore; il primo eco della scatola viene agganciato anche se neutrale; la scatola vuota esaurisce; il TASM
  con l'ESM preferisce la nave che emette.
- TLAM: le quattro validità, P(landfall) uguale alla formula, esito con la stimata «VALIDA» e la vera persa.
- Determinismo dal seme.

### 11.12 Ordine

1. Contratti approvati (principale, runtime).
2. Dati e catalogo; ellisse e piano con le prove (la principale può cominciare la pagina F7 su `PianoTiro`).
3. Volo e aggancio con le prove.
4. TLAM.
5. Aggancio del runtime; poi il disassemblatore per i radar che cerca l'ESM del TASM.

**Nota dell'header (28/10):** il file vero è `nucleo/missili.hpp`, che fa fede. Rispetto agli schizzi qui sopra:
`Lancio` si chiama `UscitaMissile` (campo `uscita`), perché `missione.hpp` ha già una `LancioMissile`; `Ruolo` è
`RuoloMissile`; `BersaglioStimato` ha anche `dimensione`; `PianoTiro` ha in più `distanza_m`, `accensione_m`,
`fine_m`, `p_area` e `p_primo`. Dal 28/10, su richiesta del runtime decisa dalla principale: `quota_lancio_min_m` (20 m
per l'Harpoon, 0 per gli altri; il TLAM non vuole la quota periscopica per lo sparo) e l'avviso non bloccante
`Avviso::quota_bassa`, con +10% di guasto in salita sotto la quota minima (la capsula affiora vicino alla vela).

Stima: una settimana e mezza di lavoro mio per 2-4.

### 11.13 Le scene di prova (spente)

Scritte da `strumenti/scene_interazioni` (lista `scene_missili`) in **`data/missioni/prove/`**, accese dal 28/10/2026
con la stazione cablata: sono nella scelta delle missioni, in `sandbox_scene.sh`, nelle prove delle scene e fra i vetrini.
Fra i vetrini sono «in ritaratura» finché il comandante IA non tira i missili del giocatore.
`scene_interazioni --semi N --scena PROVA_M` le gioca (il giocatore resta fermo).

| Scena | Che cosa prova | Attese con la stazione cablata |
|---|---|---|
| **PROVA_M1_ANTINAVE** (Faroe, guerra, mare 3, giorno) | Uno Slava (grande) a ~38 km per 045 con due Krivak (medi): uno 4 km davanti sulla linea di tiro, uno di lato; un cargo neutrale grande 3 km di traverso allo Slava, dentro la scatola media; un mercantile a 25 km, fuori. Harpoon nei tubi 1, 2 e 7 | Scatola media: il Krivak davanti si prende con P 0,4; poi il primo eco grande fra Slava e cargo, quindi a volte il cargo. Avviso dei neutrali sul pannello. Scatola piccola con la TMA buona: il cargo resta al bordo; con la TMA scarsa cala la probabilità stimata. Colpire il cargo: infrazione delle ROE |
| **PROVA_M2_TLAM** (Barents, guerra, mare 4, notte) | Il 688 a 22 miglia dal PLP della missione 0417, con 1,5 miglia d'incertezza (`navigazione = { sigma_iniziale_nm = 1.5 }`); in libreria la 0417 (Severomorsk) e la 0418 (Olenja guba), nella chiave `tlam`; l'ordine d'esecuzione alla radio dopo 30 minuti, ora sul bersaglio 04:30Z ±10 min; un Krivak di pattuglia verso costa e un peschereccio | La pagina TLAM passa da NON VALIDA a RIPIANIFICA e VALIDA avvicinandosi al PLP. La presa stimata della prima mappa sale dopo un punto nave, che col Krivak vicino costa. Senza punto nave, l'esito sulla posizione vera può perdere il landfall con il pannello a «VALIDA» |

Lo scrittore dell'editor scrive le chiavi `navigazione`, `tlam` ed `esecuzione_tlam` (nel messaggio radio), anche nel
blocco dell'editor: riaprendo e riscrivendo queste missioni non si perdono (28/10/2026; `ScenarioMissione::navigazione`,
`tlam`, `MessaggioScenario::esecuzione_tlam`).
