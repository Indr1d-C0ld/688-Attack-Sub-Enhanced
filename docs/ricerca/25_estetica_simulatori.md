# 25 — L'estetica delle schermate nei simulatori d'epoca (dossier di riferimento)

Progetto: remake di *688 Attack Sub* (EA 1989). Scopo: studiare **com'erano disegnate** le schermate attorno alla
missione e le stazioni nei simulatori DOS degli anni '90:
- cornici e bordi, materiali e texture, luce e smussi;
- tipografia, pulsanti e stati, palette e dithering a 256 colori, transizioni, icone.

Da qui una proposta di linguaggio comune per le nostre schermate (roadmap 33, rifinitura estetica; richiesta
dell'utente, «con calma»). Solo ricerca: nessun codice. Il classico non cambia. Compilato il 27-28 ottobre 2026 dal
kit.

Il dossier 15 studia come quelle schermate erano **organizzate** (flusso, punteggi, carriera); qui si parla solo di
**aspetto**, e non si ripete il 15. Le note di ricerca complete, titolo per titolo e con tutte le fonti, sono in
`25_note/`:
- `a_dynamix_sierra.md`: *A-10*, *Red Baron*, *Aces of the Pacific*, *Aces over Europe*, *Aces of the Deep*,
  *Nova 9*;
- `b_ea_origin.md`: *688 Attack Sub*, *Seawolf*, *Chuck Yeager*, *U.S. Navy Fighters*, *ATF*, *Strike Commander*,
  *Longbow*, *688(I)*;
- `c_microprose_altri.md`:
  - *Red Storm Rising*, *Silent Service II*, *Task Force 1942*, *F-117A*, *F-15 III*, *Gunship 2000*, *Fleet
    Defender*, *B-17*;
  - *Silent Hunter*, *Wolfpack*, *Falcon 3.0*, *Harpoon*, *Tornado*;
  - la parte tecnica.

Il «prima» delle nostre schermate è l'inventario di trailer-1, `schermate_prima/` (59 istantanee, `indice.md`).

---

## 0. Convenzioni e limiti

- **Confidenza** come nel dossier 15 §0:
  - **[alta]** fonte primaria: il manuale letto nel testo, i file originali del progetto (`re/cls`,
    `extracted/png`), una recensione d'epoca letta per intero;
  - **[media]** fonte secondaria credibile o dato coerente fra più fonti;
  - **[bassa]** forum, ricordo, deduzione.
- **STIMA** = valore proposto per il nostro gioco; **PROPOSTA** = scelta da discutere; «estratto» = letto solo nel
  riassunto di un motore di ricerca. Le sigle fra parentesi quadre rimandano alla §11 e alle note.
- **Nessuna immagine scaricata.** Le schermate degli altri giochi sono descritte a parole da manuali e recensioni.
- **Il limite maggiore.** Manuali e recensioni dicono **che cosa** c'è in una schermata, quasi mai **come è
  disegnata**: luce, smussi, font, rampe e dissolvenze sono scritti raramente. MobyGames (didascalie delle
  schermate), GameSpot e Giant Bomb rifiutano la lettura automatica. Dove un dettaglio non è scritto, il dossier lo
  dice; non si inventa. Per colmare queste lacune serve vedere le schermate (§10).
- **Eccezione:** di *688 Attack Sub* abbiamo i file originali, quindi la sua estetica è documentata pixel per
  pixel [alta].

---

## 1. Il punto di partenza: *688 Attack Sub* (1989)

Dai file originali (`SELECT.CLS`, `DIALOG.CLS`, `RADIO.CLS`, immagini di `TITLE.PAK`, `688.PAK`, `ALFA.PAK`, `END*`)
[RE-SELECT], [RE-CLS], [RE-PNG]; dettagli in `25_note/b_ea_origin.md` §1.

**Due strati.** I fondali VGA sono immagini 320×200 a 256 colori con la palette propria, e sono foto o dipinti
sfumati senza retinatura. L'interfaccia invece è **disegnata a comandi** (rettangoli, linee, testo) con i soli 16
colori di base [alta].

**Il pannello avvitato** è il segno della casa:
- **la lamiera:** tutto lo schermo è grigio chiaro, senza texture;
- **i pannelli incassati:** bordo di un pixel, nero in alto, grigio scuro a sinistra, bianco a destra e in basso,
  angoli smussati di un pixel. La luce viene dall'alto a sinistra e il pannello sembra affondato nella lamiera;
- **le viti:** ai quattro angoli interni una **testa di vite** di 3×3 pixel, chiara in alto, scura sotto e a destra;
- **la targa rivettata** col titolo al centro in alto [alta].

**Il testo inciso:**
- bianco con un'**ombra nera a +1,+1**, tutto maiuscolo, in un solo font bitmap squadrato con due misure (maiuscole
  piccole 5×5, maiuscole grandi 7×7);
- il grassetto è la lettera ridisegnata spostata di un pixel;
- il **colore dell'ombra porta un significato:** blu per il 688, rosso per l'Alfa [alta].

**Il pulsante a spia** è un tasto quadrato 11×10 in rilievo (filo bianco in alto a sinistra, grigio scuro in basso a
destra) con dentro una **lente colorata incassata**. Il colore dice il gruppo:
- verde: avanti;
- rosso: uscire;
- magenta: il modem;
- ciano: livello e telefono;
- blu: rispondere.

Nelle finestre di dialogo c'è un triplo filo nero, bianco e nero, e i tre tasti verde, blu e rosso. L'OK è un tasto
giallo con la scritta nera [alta].

**Il colore dice la parte:** blu USA, rosso URSS, nei pulsanti e nelle ombre. Il fulmine bianco segna il modem
[alta].

**Il menu come luogo:**
- la **CONN** è un'illustrazione a tutto schermo della centrale, diversa per il 688 (acciaio, blu) e per l'Alfa
  (verde petrolio, fosforo, una tazza rossa);
- non ci sono pulsanti disegnati: le stazioni sono **zone calde** e il cursore prende l'**icona della stazione**
  [alta].

**L'esito è una scrivania:** legno visto dall'alto, calendario, buste, telefono rosso. Nel mezzo c'è una **cartolina**
o un biglietto: San Francisco, Mosca, il biglietto di condoglianze della Navy, quello col ritratto di Lenin [alta].

**L'equipaggio:** foto digitalizzate di persone vere, quasi in bianco e nero, «one of the best-looking games» [alta]
[COMP111].

**Le transizioni:** nessuna. Dal titolo al gioco c'è uno schermo vuoto, perché il `.BAT` lancia un altro eseguibile
[bassa]. F10 apre il «boss key», un finto prompt DOS [alta].

---

## 2. Dynamix e Sierra (1989-1995)

Sintesi di `25_note/a_dynamix_sierra.md` (sei manuali letti per intero, CGW e *Compute!*).

- **Il materiale cambia con l'epoca del gioco:**
  - *Red Baron* ha un'interfaccia **art déco** che richiama l'inizio del Novecento;
  - in *Aces of the Pacific* l'art déco cede al **«cold gray steel»** di metà secolo, con **fotografie
    digitalizzate** nelle schermate di assegnazione, esito e valutazione [media] [CGW92]. È l'unica indicazione
    esplicita sui materiali di casa.
- **La fotografia come materia prima:** attori in costume e servizi fotografici per *A-10*, foto d'archivio della
  Smithsonian per gli *Aces* [alta] [M-A10], [M-AOTP].
- **Documenti come schermate:**
  - foglio di campagna e scheda di squadriglia da sfogliare (NEXT, PREVIOUS);
  - comunicati in telescrivente;
  - il **Photolog**, album della carriera;
  - il **titolo di giornale** col nome del giocatore;
  - il **giornale di bordo come libro**: un clic sulla pagina sinistra o destra lo sfoglia (*Aces of the Deep*) [alta].
- **Liste a barra evidenziata** con la descrizione accanto e spesso un **ritratto** (l'asso in alto a destra);
  **pulsanti a ciclo** («ogni pressione mostra la combinazione successiva»); opzioni come **pulsanti dentro il
  testo** che aprono un menu modale [alta].
- **Pulsanti testuali in maiuscolo con un verbo:** BEGIN MISSION, DECLINE MISSION, ACCEPT, CANCEL [alta per le
  diciture; l'aspetto non è scritto].
- **La mappa al centro:**
  - scorre, e il nome compare al passaggio del cursore, senza clic;
  - il cursore circonda l'icona con un'evidenziatura;
  - il bersaglio ha un contorno rosso;
  - c'è la foto di ricognizione [alta] [M-AOE].
- **L'interfaccia nell'ambiente** (*Nova 9*, *Aces of the Deep*):
  - la plancia o la centrale da attraversare, con le etichette al passaggio;
  - **cursori d'azione** che cambiano forma;
  - **pulsanti grigi** in fila lungo il bordo basso, che cambiano con la schermata [alta] [M-CAOD], [M-N9].
- **Colori funzionali e costanti** [alta]:
  - rosso per nemico, bersaglio, pronto al fuoco;
  - verde per amico e sicuro;
  - giallo per il transito;
  - grigio per inattivo.

  La **spia**: Fire **rosso vivo** quando si può sparare, **scuro** in ricarica; frecce **grigio chiaro** se
  disabilitate.
- **Il danno come disegno tecnico:** un blueprint del battello con le zone colpite cerchiate in rosso (*Aces of the
  Deep*) [alta].
- **I contatti che invecchiano** sbiadiscono dal nero al grigio sulla carta [alta] [M-CAOD].
- **Non scritto:** font, luce, smussi, rampe, dithering, transizioni dei menu. Gli effetti di palette sono documentati
  solo in volo (rosso da ferito, bianco col sole) [media].

---

## 3. Electronic Arts e Origin (1991-1997)

Sintesi di `25_note/b_ea_origin.md`.

- ***Chuck Yeager's Air Combat* (1991):**
  - «bottoni» d'opzione col **predefinito fra frecce >testo<**, lettera iniziale come tasto rapido;
  - menu a tendina in volo;
  - il **ritratto severo** di Yeager che commenta;
  - un replay a videoregistratore [alta e media].
- ***U.S. Navy Fighters* (1994):** una **grammatica unica** per tutte le schermate di preparazione [alta] [USNF-MAN]:
  - barra dei menu a tendina in alto;
  - conferma con il **segno blu**, **Cancel verde**;
  - **evidenziatura gialla** della scelta;
  - pulsanti di testo a ciclo, interruttori e manopole;
  - **portablocchi a pagine** per il fascicolo e il briefing, con intestazione dattiloscritta;
  - **lucine rosse** per scegliere, oggetti da trascinare;
  - i **danni** si leggono dai trattori gialli attorno all'aereo.
- ***Strike Commander* (Origin 1993):** **luoghi** invece dei menu [alta] [SC-MAN].
  - Nell'hangar ogni oggetto è un comando: porte, jeep, camion, aereo, persone. In ufficio il libro mastro e il
    catalogo con la calcolatrice; in camerata il letto.
  - Si torna indietro cliccando il bordo dello schermo.
  - I **pulsanti non disponibili sono «flat»**, piatti invece che in rilievo: lo stato disabilitato più chiaro che si
    trovi scritto.
  - Crediti «Menus and fonts» a una persona sola: un'identità tipografica voluta.
- ***Longbow* (Origin 1996):** le «wrapper screens» sono modellate in Alias su SGI; il punteggio sulla **lavagna**;
  per riprovare si clicca il **casco** sul tavolo [alta] [LB-GUIDE].
- ***Seawolf* (1994)** e ***688(I)* (1997):** le stazioni diventano menu con la **foto o la clip** del marinaio
  (*Seawolf*), poi schermi-stazione pieni di dati (*688(I)*). Le cascate sono pixel verdi vivi su un campo
  scuro di rumore [alta e media].
- **Non scritto:** font, cornici e stati dei pulsanti di *Seawolf*, *Longbow*, *ATF* e *688(I)*; le transizioni,
  salvo la dissolvenza dall'introduzione alla base di *Longbow*.

---

## 4. MicroProse e gli altri (1988-1996)

Sintesi di `25_note/c_microprose_altri.md` (sette manuali letti per intero).

- **Luoghi veri:**
  - la Ready Room di *F-117A*, con porte e bacheca;
  - l'**hangar** di *F-15 III*, con armadietti, lavagna, televisore e cestino;
  - l'ufficio di *Gunship 2000* [alta e media].
- **Foto-realismo dichiarato:**
  - per *F-15 III* Barb Bents impose uno stile «photo-realistic» coerente, e un **modello CAD** di una base aerea fece
    da set di tutte le schermate di contorno [alta] [F15III-MAN];
  - *Silent Service II* fotografa modelli in scala [media].
- **Il cursore fa da etichetta:** in *F-15 III* «legge» il nome del punto attivo e cambia forma; la scena resta
  pulita, senza pulsanti sopra [alta].
- **Il colore è un codice spiegato in appendice:** le tabelle dei colori di *F-117A*, il colore per modo grafico in
  *SS II*, il dithering come ripiego dichiarato in CGA [alta].
- **Pulsanti testuali:**
  - *Fleet Defender*: con la **lettera-scorciatoia evidenziata**, selezione gialla, pulsanti che si «depress» [alta];
  - *Falcon 3.0*: **spie rosse** per gli stati, in una sala operativa di monitor [alta];
  - *Harpoon*: una **scrivania da Macintosh** con menu a tendina, dialoghi e voci in grigio se non disponibili [alta].
- **Sobrietà:** la grafica deve «servire il gioco», la regola di Sid Meier riferita dal direttore artistico Haire
  [media] [SH-MH]; animazioni brevi e disattivabili.
- **Il dopo-missione è un replay** (*F-15 III*, *Fleet Defender*) [alta].
- ***Silent Hunter* (SSI 1996)** e ***Tornado*** portano la SVGA e gli sfondi digitalizzati nelle schermate di
  contorno; *Silent Hunter* i filmati dei veterani [media].

---

## 5. La tecnica a 256 colori

Da `25_note/c_microprose_altri.md` §3, con le ricadute per noi.

- **La palette è una risorsa:**
  - 256 voci da 18 bit (6 per canale);
  - le **dissolvenze** si fanno cambiando la palette durante il ritorno verticale, verso il nero o verso un colore
    [alta come tutorial d'epoca] [PCGPE-VGA];
  - si lavora **per intervalli d'indici**: lo stencil di *Deluxe Paint* protegge un blocco [media] [WP-DP].
  - **Per noi:**
    - la tela 640×480 a 8 bit ha già blocchi assegnati (`grafica/palette.hpp`): le fasce 16-79, 96-158 e 196-247 sono
      illuminate dalla luce della vista (`INDICI_VISTA`); restano fissi 0-15 (EGA), 80-95 (interfaccia), 159-195
      (effetti e altro), 248-253 (liberi, pochi), 254-255;
    - la carta e le consolle ridefiniscono 96-247 quando coprono la vista;
    - le sagome ricche usano 96-143.

    Un linguaggio comune deve avere **un blocco suo**, fisso in tutte le schermate di contorno (§8.2).
- **Il color cycling** (Mark Ferrari, *Loom*, *Monkey Island*): un'immagine sola e una palette che ruota un blocco
  di voci anima acqua, luci, pioggia [media] [EG-CC], [WP-CC]. Nei simulatori del nostro elenco non è documentato.
  - **Per noi:** spie che lampeggiano, il cursore della telescrivente, l'acqua di un pontile, la spazzata di uno
    schermo; un intervallo di voci per effetto.
- **Le rampe** (R. Schlitter): 8-9 gradini, la luminosità che non parte dal nero, la saturazione massima al centro,
  la tinta che si scalda verso i chiari, i grigi ricavati desaturando il centro [media] [SLY-1].
  - **I metalli** non sono documentati per questi giochi. STIMA: ombre grigio-blu fredde, riflessi quasi bianchi
    caldi, e un **salto netto di contrasto** fra due gradini vicini, che distingue il metallo dalla pietra.
- **Il dithering:**
  - **ordinato** (Bayer) per tutto quello che si anima o scorre, perché il colore di un pixel non dipende dai vicini;
  - **a diffusione dell'errore** solo per le immagini fisse [media] [JY-DITH].
  - Il kit usa già il Bayer in ritratti, sagome e cerimonie (`cerimonia::retina`).
- **La luce dall'alto a sinistra:**
  - è la convenzione delle linee guida Apple e quella di *688* (§1) [alta];
  - in rilievo: chiaro sopra e a sinistra, scuro sotto e a destra;
  - premuto: smusso invertito e scritta spostata di un pixel in basso;
  - disabilitato: niente smusso, colori spenti [bassa per la regola generale, alta per Apple e *688*].
- **La leggibilità del testo:**
  - su sfondi illustrati vince lo **sfondo pieno** dietro il testo, o un **contorno di due pixel** [media]
    [ONT-TXT];
  - i font bitmap nascono senza antialias, quindi l'ombra è un pixel netto.
  - Per noi: il contorno nero di un pixel della carta (`simboli::scrivi`) è già questo.

---

## 6. Tabella comparativa

| Elemento | *688* (1989) | Dynamix/Sierra | EA/Origin (1991-97) | MicroProse e altri |
|---|---|---|---|---|
| **Cornici** | pannelli incassati di 1 px su lamiera, viti agli angoli, targa rivettata | art déco (*Red Baron*), acciaio grigio (*Aces*) | portablocco, catalogo, lavagna: oggetti più che cornici | scena fotografica (hangar, ready room) o finestre da Macintosh (*Harpoon*) |
| **Materiali** | lamiera grigia; legno della scrivania; cartoline | foto d'archivio, telescriventi, giornali, libro di bordo | set 3D e attori, carta dattiloscritta | foto di modelli, CAD, cabine fotografate |
| **Luce** | alto a sinistra, smussi di 1 px | non scritto | non scritto (piatto = disabilitato) | non scritto; Apple: alto a sinistra |
| **Tipografia** | un font squadrato, maiuscolo, inciso con ombra +1,+1; ombra colorata per la parte | maiuscolo con verbi d'azione (resa non scritta) | crediti «Menus and fonts» (Origin); dattiloscritto nei portablocchi | lettera-scorciatoia evidenziata; etichetta del cursore |
| **Pulsanti** | tasto a spia con lente colorata per gruppo | testuali, a ciclo, dentro il testo; grigi lungo il bordo basso; Fire rosso o scuro | predefinito fra frecce; blu avanti, verde Cancel, giallo scelta; oggetti come pulsanti | spie rosse (*Falcon*), selezione gialla (*Fleet Defender*), voci in grigio (*Harpoon*) |
| **Palette** | 16 colori per l'interfaccia, 256 per i fondali | VGA 256, gradienti senza bande; effetti in volo | VGA 256, poi SVGA | colore come codice in appendice; dithering come ripiego |
| **Transizioni** | nessuna (schermo vuoto) | non scritte per i menu | dissolvenza dall'introduzione (*Longbow*) | dissolvenze di palette (tecnica d'epoca) |
| **Icone e cursori** | icona della stazione sul cursore, mirino, fulmine del modem | cursori d'azione, libro e cinepresa nel manuale in linea | freccia (*Yeager*) | cursore-etichetta, forme in cabina |

**Le due scuole.** Dynamix, Origin e il CONN di *688* mettono le schermate **in un luogo** o **in un documento**
(base, hangar, scrivania, libro, giornale). *USNF*, *Fleet Defender* e *Harpoon* usano una **grammatica di
pulsanti** uguale ovunque. I giochi migliori tengono tutte e due: il luogo per l'atmosfera, la grammatica per non
perdersi.

---

## 7. Le nostre schermate oggi (il «prima»)

Dall'inventario di trailer-1 (`schermate_prima/indice.md`), con i dati tecnici:
- le cornici del kit (`grafica/cornici`) le usano davvero solo esito, bacheca, editor (un pannello) e le consolle
  delle stazioni (`grafica/sale`);
- le altre schermate disegnano righe e rettangoli a mano;
- quasi tutto è scritto in `originale.fnt` a scala 1. La scala 2 c'è solo per i titoli di esito, bacheca e albo, e i
  font del kit (incisione, stampino, cifre, fosforo) stanno solo nelle consolle e sulle isobate.

**I sette problemi trasversali**, a cui la proposta deve rispondere:
1. **Due famiglie:** le schermate con le cornici (stazioni, menu, briefing, sala siluri, esito, bacheca, albo) e quelle
   di **testo nudo su nero** (replay, statistiche, editor, regolazioni, sandbox, serie, debriefing), che sembrano
   strumenti di sviluppo.
2. **Muri di testo** senza gerarchia: briefing, museo, sala radio, regolazioni (31 righe uguali), tabelle.
3. **Spazio vuoto non progettato:** le statistiche della sandbox (due terzi neri), la metà alta della sala radio,
   l'albo vuoto.
4. **Colori delle testate senza regola:** giallo, arancio, bianco, ciano, verde.
5. **Righe d'aiuto in tre forme:** pulsanti veri, testo semplice, pulsanti a metà.
6. **Stati vuoti senza disegno:** albo senza carriere, cumulate a zero, bacheca «non ancora presa».
7. **Coppie che sembrano la stessa schermata:** sonar moderno e 1989, carta VGA e nautica.

**Quello che va già bene e si tiene:**
- le consolle delle stazioni (grigie per il 688, crema per l'Alfa);
- la carta da fascicolo dell'esito, i medaglioni della bacheca, le minicartoline;
- i simboli della carta con l'alone nero.

---

## 8. Proposta: un linguaggio comune (PROPOSTA)

### 8.1 I principi, validi in tutte e tre le varianti

Ognuno risponde a uno dei sette problemi della §7.

1. **Una cornice per ogni schermata, mai testo nudo su nero** (problema 1). Ogni schermata di contorno ha la sua
   cornice e i suoi pannelli. Anche gli strumenti (replay, statistiche, editor, sandbox) stanno dentro una consolle o
   un documento. Le stazioni restano come sono.
2. **Tre livelli di testo** (problema 2):
   - **titolo:** scala 2, oppure un font del kit (`incisione688_7` per la US Navy, `stampino_urss_10` per la VMF),
     inciso con l'ombra;
   - **intestazione di gruppo:** scala 1, maiuscolo, su una fascia;
   - **testo:** scala 1 su un fondo pieno, cioè carta, vetro o fascia scura, mai sul disegno.

   I testi lunghi si spezzano in riquadri con un titolo ciascuno. Le righe ripetute uguali (15 volte «DELLA
   MISSIONE») diventano gruppi, e il valore di serie si scrive in grigio.
3. **Una griglia per lo spazio** (problema 3): testata 24 px, piede 28 px, margini 8 px, colonne a 1/3 o 1/2. Quello
   che avanza non resta nero: c'è il materiale (lamiera, legno, vetro) o un riquadro «che cosa mostra questa pagina»
   col disegno.
4. **Una regola per i colori dei titoli** (problema 4):
   - il titolo della schermata è sempre nel colore della **marina** del giocatore: oro o bianco per la US Navy,
     rosso o crema per la VMF;
   - le intestazioni di gruppo sono sempre di un colore neutro;
   - il ciano solo per i valori che si possono cambiare, il giallo solo per la riga scelta.

   Le parti restano NTDS (azzurro, rosa, giallo, verde), come i simboli.
5. **Un piede uguale dappertutto** (problema 5):
   - i comandi principali come **pulsanti veri**, al più quattro, col tasto scritto sul pulsante («ESC INDIETRO»,
     «INVIO AVVIA»);
   - gli altri tasti in **una riga d'aiuto** sopra, in grigio, sempre nella stessa forma.

   È la grammatica di *USNF*: avanti, indietro e predefinito sempre allo stesso posto e dello stesso colore.
6. **Gli stati vuoti hanno un disegno** (problema 6), come in *Aces of the Deep* e *Strike Commander*, dove il vuoto
   è una scena:
   - l'albo senza carriere è uno schedario con la cartella «NUOVO COMANDANTE»;
   - le cumulate a zero sono la carta del teatro con «NESSUNA MISSIONE ANCORA»;
   - la bacheca vuota mostra i medaglioni grigi, come «da conquistare», con il primo evidenziato.
7. **Le coppie si devono distinguere a colpo d'occhio** (problema 7):
   - il sonar del 1989 ha il **fosforo** (§9), quello moderno lo schermo pulito;
   - la carta nautica ha la **carta** (bianco del profondo, matite) e la mobilia, la VGA il blu;
   - le due pagine dell'esito hanno due materiali: rapporto su carta, cerimonia su velluto.
8. **Luce dall'alto a sinistra, smussi di uno o due pixel**, come *688* (§1, §5):
   - in rilievo: chiaro sopra e a sinistra, scuro sotto e a destra;
   - **premuto:** smusso invertito e testo spostato di un pixel;
   - **evidenziato:** la lente o la fascia si accende;
   - **disabilitato:** piatto, senza smusso, testo grigio scuro (*Strike Commander*);
   - **predefinito:** un filo d'oro attorno.
9. **Il colore dice la parte e il gruppo**, come le spie di *688*:
   - verde per avanti, rosso per uscire o rifiutare, ambra per le scelte, blu per il 688, rosso per l'Alfa;
   - una tabella fissa, scritta nel manuale come le appendici dei colori di MicroProse.
10. **Transizioni brevi:** dissolvenza di palette di 150-250 ms verso il nero fra due schermate (la tecnica d'epoca,
    §5). Spenta con la compressione del tempo e nelle istantanee, così la regressione non cambia.
11. **Il cursore fa da etichetta** sulle scene illustrate (la CONN, una sala): il nome del punto attivo appare al
    passaggio (*F-15 III*, *Aces*), invece di pulsanti disegnati sopra.

### 8.2 La palette dell'interfaccia (vale per tutte le varianti)

Un **blocco fisso di 48 posti** per cornici, pannelli, testo, pulsanti e spie. Sono i posti 80-95 dell'interfaccia
più 32 da trovare fra quelli liberi delle schermate di contorno (STIMA: 248-253 e un blocco nelle rampe della vista,
che fuori dal mare non si vede). Contiene:
- **lamiera:** 6 grigi da freddo a caldo;
- **legno:** 5 gradini;
- **carta:** 4 gradini;
- **spie:** verde, rosso, ambra, blu, ciano, ciascuno spento e acceso;
- **oro** per il filo del predefinito;
- **il colore della marina:** US Navy e VMF.

Le schermate che ridefiniscono la palette (carta, sonar, rilievo) non toccano questo blocco. Così una cornice è
uguale dappertutto.

### 8.3 Variante A: «Lamiera di bordo» (l'eredità diretta del 1989)

- **L'idea:** tutto il gioco parla la lingua di `SELECT.CLS`, ridisegnata a 640×480.
- **Cornici:** pannelli incassati di 2 pixel su lamiera grigia, viti agli angoli, targhe rivettate per i titoli.
  Lamiera grigio-blu per la US Navy, crema per la VMF, come le consolle già fatte.
- **Sfondi:** lamiera con una texture leggerissima, cioè graffi e dithering ordinato a due toni, mai a tinta unita.
- **Pulsanti:** **tasti a spia** con la lente colorata e la scritta incisa accanto, come il 1989. Accesi, la lente si
  illumina; premuti, scendono di un pixel.
- **Testo lungo** (briefing, museo, regole) su **vetro nero** di consolle, cioè un pannello incassato scuro col testo
  chiaro, come uno schermo di bordo.
- **Briefing e debriefing:** fogli di carta (ordini, rapporto) fermati sulla lamiera con due viti o una clip.
- **Strumenti** (replay, statistiche, sandbox, serie, editor): una «consolle d'analisi», cioè lamiera con uno o più
  schermi incassati.
- **Pro e contro:**
  - **pro:** coerente con le stazioni che ci sono già; il meno caro (cornici e pannelli esistono quasi tutti); il più
    riconoscibile per chi ricorda il 1989;
  - **contro:** il più freddo e uniforme; carriera e cerimonie, che oggi sono legno e carta, vanno ricondotte o
    tenute come eccezione.
- **Costo:** basso. Si estende `grafica/cornici` (pannello a viti, targa, tasto a spia, vetro) e le schermate lo
  chiamano.

### 8.4 Variante B: «La base» (luoghi e documenti, alla Dynamix e Origin)

- **L'idea:** ogni gruppo di schermate è un **luogo della base**, e l'informazione arriva come **documento d'epoca**.
  - **Scelta della missione:** lo **schedario delle missioni** su una scrivania. Le schede (originali, rapide,
    editor, sandbox) sono linguette di cartelline; la missione scelta ha la sua cartella aperta, col timbro.
  - **Briefing:** la **sala briefing**: la carta appesa, gli ordini come **telescrivente** su carta a modulo
    continuo, la colonna dei livelli come pannello di interruttori.
  - **Sala siluri:** com'è, una sala.
  - **Debriefing ed esito:** la **scrivania del 1989** (legno, telefono, cartoline), allargata. Le minicartoline sono
    fotografie sulla scrivania, il rapporto su carta.
  - **Albo, fascicolo, bacheca:** l'**ufficio del comandante**: schedario, fascicoli di cuoio, bacheca di
    sughero o vetrina.
  - **Strumenti:** la **sala d'analisi**: plotter, rotoli di carta, uno schermo grande.
- **Pulsanti:** gli oggetti della scena sono pulsanti, con l'etichetta del cursore; nel piede, sempre, i pulsanti
  testuali della §8.1 punto 5, per chi non vuole cercare.
- **Pro e contro:**
  - **pro:** la più ricca e d'atmosfera, quella dei simulatori che l'utente ricorda (*Aces*, *Strike Commander*,
    *F-15 III*); dà un disegno naturale agli stati vuoti e allo spazio;
  - **contro:** la più cara (molte illustrazioni, una per luogo e per marina); rischio di scene affollate e di
    oggetti che non si capiscono; gli strumenti tecnici stanno male dentro un luogo.
- **Costo:** alto. Servono le illustrazioni dei luoghi, procedurali o dipinte, per US Navy e VMF.

### 8.5 Variante C: «Il quadro di comando» (consolle e schermi, alla *Falcon 3.0* e *688(I)*)

- **L'idea:** tutto è uno **schermo di consolle**: le schermate di contorno come le stazioni, ma più grandi.
  - Una cornice di consolle attorno a tutto lo schermo, con la targhetta del posto («SALA BRIEFING», «ANALISI»).
  - Dentro, **uno o più schermi** a fondo scuro con testo e grafici a colori di fosforo (verde, ambra o bianco) e
    le fasce delle intestazioni.
  - Pulsanti a **spia** lungo i bordi, come i tasti funzione di una consolle reale, con la scritta sopra.
- **Carta e documenti** (ordini, rapporto, fascicolo) appaiono **sullo schermo** come documenti stampati, cioè
  carta chiara dentro lo schermo.
- **Pro e contro:**
  - **pro:** la più coerente per gli strumenti (statistiche, serie, sandbox, editor sono già schermi); la gerarchia
    viene dalle cornici degli schermi; costo medio;
  - **contro:** la più fredda; carriera e cerimonie perdono il calore di legno, carta e velluto. Va tenuta
    un'eccezione «di terra» per esito, bacheca e albo, che oggi sono la parte migliore.
- **Costo:** medio. Una consolle-cornice parametrica, gli schermi, le spie.

### 8.6 Che cosa cambia per gruppo di schermate

| Gruppo | A: Lamiera di bordo | B: La base | C: Quadro di comando |
|---|---|---|---|
| Scelta della missione e regolazioni | pannelli a viti, tasti a spia, righe raggruppate | schedario con cartelline e linguette | consolle con lo schermo dell'elenco e i tasti funzione |
| Briefing | foglio degli ordini sulla lamiera, livelli come interruttori | sala briefing, telescrivente su carta | schermo degli ordini e schermo della carta affiancati |
| Sala siluri | com'è (consolle) | com'è (sala) | com'è (consolle) |
| Debriefing ed esito | carta del rapporto fissata sulla lamiera | scrivania del 1989 con le foto | rapporto sullo schermo, cerimonie «di terra» |
| Albo, fascicolo, bacheca | lamiera con le cartelle (eccezione: legno) | ufficio del comandante | eccezione «di terra», com'è oggi |
| Replay, statistiche, sandbox, serie | consolle d'analisi con schermi incassati | sala d'analisi (plotter, schermo grande) | schermi nella consolle: il caso naturale |
| Museo | lamiera con la vetrina della voce | biblioteca o archivio | schermo d'archivio |
| Editor | consolle di lavoro | tavolo di pianificazione | consolle di lavoro |
| Stazioni | come oggi | come oggi | come oggi |

**Le tavole** (`25_tavole/`, indice in `25_tavole/indice.md`): le regolazioni della sandbox e la scelta della
missione nelle tre varianti, più la combinazione consigliata, coi dati veri dell'inventario.

**Il parere del kit:** la **A** come base, perché è la nostra eredità, costa poco e unifica subito le due famiglie.
Ci vanno le eccezioni **di B** dove la carta e il legno già funzionano (esito, cerimonie, fascicolo, bacheca) e il
**quadro di comando di C** per gli strumenti. È una A «con i documenti». La scelta è dell'utente.

---

## 9. Le stazioni del sonar (osservazioni del disassemblatore)

Dalla nota condivisa `sonar-confronto-27-10.md`; il rendering è del kit, i dati del disassemblatore.

- **I livelli delle cascate.** Oggi le cascate usano 32 indici (200-231), quindi le scale hanno gradini visibili;
  una cascata SDR ne ha 256 continui. Tre strade:
  1. **Più indici quando la stazione copre la vista**, come fa la carta: il sonar si prende un blocco di 128-150
     voci (STIMA: 96-247, che fuori dalla vista non servono) per una sola rampa. **Costo:** basso, perché la
     ridefinizione della palette c'è già; nessun cambio al motore né alla regressione.
  2. **La 1 con il dithering ordinato fra due gradini vicini:** Bayer 4×4 sulla parte frazionaria del livello in
     dB. Si arriva a circa 16 sotto-livelli percepiti per gradino, anche con pochi indici; la grana resta ferma
     perché il dithering ordinato non tremola (§5). **Costo:** basso. Combinata con la 1 dà una scala senza gradini.
  3. **La stazione in truecolor:** una seconda tela RGB solo per il sonar, composta da `piattaforma` al momento di
     presentare. **Costo:** alto:
     - una tela nuova e un percorso nuovo nella finestra;
     - le istantanee e la regressione a 8 bit che non la vedono;
     - lo stile a 256 colori che si rompe proprio dove si guarda di più.

  **Proposta del kit:** la 1 più la 2. Poi si guarda il risultato accanto a una cascata SDR prima di pensare alla 3.
- **Il modo 1989: il fosforo.**
  - **Persistenza:** ogni pixel tiene un'intensità che decade, `v = max(nuovo, vecchio · k)` con k fra 0,85 e 0,95
    a fotogramma, STIMA.
  - **Bagliore:** un alone tenue attorno ai punti più accesi.
  - **Grana:** un rumore fermo, dithering ordinato.
  - **Righe del CRT:** una riga su due più scura, alla risoluzione doppia.
  - Una rampa verde di 24-32 indici fino al quasi bianco al centro della traccia. I caratteri sono quelli del BQQ-5
    (dossier 09 §2.6; il font `fosforo688` c'è già).

  Così moderno e 1989 non sembrano più la stessa schermata (§7, problema 7).
- **L'impaginazione** del dossier 09 §4.3:
  - BTR a sinistra;
  - a destra A-scan, LOFAR e DEMON;
  - in basso contatti e slot;
  - la libreria a pagina intera.

  Tracce dell'A-scan senza scalini (linee con l'antialias sulle rampe, o il punto doppio). Scale leggibili
  (frequenza, tempo, dB) col font delle consolle. Tracker e annotazioni coi simboli comuni di `grafica/simboli_carta`.

---

## 10. Lacune e riferimenti visivi da chiedere

Le fonti scritte non dicono come erano **disegnate** cornici, pulsanti, font e transizioni di quasi tutti i titoli
(§0). Per le bozze per schermata servirebbero riferimenti visivi salvati. **Nessuno è stato scaricato.** Se l'utente
è d'accordo, il kit propone di guardare, uno per titolo, le schermate di questi menu:

| Titolo | Schermate | Da dove |
|---|---|---|
| *Aces of the Pacific*, *Aces of the Deep* | menu principale, scelta della missione, briefing, base, fascicolo | MobyGames (pagina delle schermate del titolo), o il gioco installato |
| *Red Baron* | Aerodrome Menu, Pilot Record (lo stile art déco) | MobyGames |
| *F-15 Strike Eagle III* | hangar, briefing, debriefing (il set CAD foto-realistico) | MobyGames |
| *U.S. Navy Fighters* | Choose Activity, Pilot Service Record (il portablocco) | MobyGames |
| *Strike Commander* | hangar, ufficio, opzioni (pulsanti piatti e in rilievo) | MobyGames |
| *Falcon 3.0* | sala operativa, pulsanti a spia | MobyGames o DOS Days |
| *688(I) Hunter/Killer* | centrale, sonar (le cascate) | MobyGames |

Basterebbero 2-3 immagini per titolo, tenute solo come riferimento nella cartella del dossier, non nel gioco.

**Altre lacune:** le rampe dei metalli di questi giochi; i nomi e le misure dei font; le transizioni dei menu, tranne
la dissolvenza di *Longbow*.

---

## 11. Fonti

Consultate il 27-28 ottobre 2026. Qui le principali; l'elenco completo, con le sigle usate nelle note, è in fondo a
`25_note/a_dynamix_sierra.md`, `b_ea_origin.md` e `c_microprose_altri.md`.

### Progetto
- `[RE-SELECT]` listato e ridisegno di `SELECT.CLS`: `re/cls/SELECT.CLS.txt`, `SELECT.CLS.png`; `[RE-CLS]` gli
  altri `.CLS` in `re/cls/`; `[RE-PNG]` immagini estratte in `extracted/png/`; `[RE-DOC]`
  `docs/01_analisi_tecnica_originale.md`; `[MAN-688]` `docs/originale/manuale_EN.txt`.
- `schermate_prima/indice.md` (inventario di trailer-1); `docs/ricerca/15_schermate_simulatori.md`; nota condivisa
  `sonar-confronto-27-10.md` (disassemblatore); `docs/ricerca/09_sonar_sistemi_e_interfacce.md`.

### Manuali
- `[M-A10]` *A-10 Tank Killer*: https://archive.org/details/A-10_Tank_Killer_1990_Dynamix
- `[M-RB]` *Red Baron*: https://archive.org/details/Red_Baron_-_Manual
- `[M-AOTP]` *Aces of the Pacific*: https://www.dosdays.co.uk/media/games/aotp/aotp_manual.pdf
- `[M-AOE]` *Aces over Europe*: https://www.dosdays.co.uk/media/games/aceseur/aoe_manual.pdf
- `[M-CAOD]` *Command: Aces of the Deep*: https://archive.org/details/Command_Aces_Of_The_Deep_-_Manual
- `[M-N9]` *Nova 9*: https://archive.org/details/Nova_9_-_Manual
- `[SC-MAN]` *Strike Commander*: https://archive.org/details/StrikeCommanderManual
- `[USNF-MAN]` *U.S. Navy Fighters Gold*: https://archive.org/details/u.-s.-navy-fighters-gold
- `[LB-GUIDE]` *Jane's AH-64D Longbow* Official Guide: https://archive.org/details/Janes_AH-64D_Longbow_Guide
- `[F15III-MAN]` *F-15 Strike Eagle III*: https://archive.org/stream/microprose-f-15-strike-eagle-iii-manual/Microprose_F15_Strike_Eagle_III_Manual_EN_djvu.txt
- `[F117-MAN]` *F-117A*: https://archive.org/stream/f117Astealthfightermanual/F-117AStealthFighter-Manual_djvu.txt
- `[FD-MAN]` *Fleet Defender*: https://www.dosdays.co.uk/media/games/f14/Fleet_Defender_Manual.pdf
- `[F3-MAN]` *Falcon 3.0*: https://dosdays.co.uk/media/games/falcon3/Falcon%203.0.PDF
- `[HP-MAN]` *Harpoon Challenger Pak*: https://archive.org/stream/Harpoon_Challenger_Pak_Manual_1991/Harpoon%20Challenger%20Pak%20Manual%20%281991%29_djvu.txt
- `[SS2-MAN]` *Silent Service II* (manuale tedesco): https://archive.org/stream/silent-service-ii-dos-german-manual/Silent-Service-II_dos_german-Manual_djvu.txt

### Recensioni e interviste
- `[CGW92]` *Computer Gaming World* n. 92 (1992), anteprima di *Aces of the Pacific*: https://archive.org/details/Computer_Gaming_World_Issue_92
- `[CGW125]` CGW n. 125 (1994), *Aces of the Deep*: https://archive.org/details/Computer_Gaming_World_Issue_125
- `[CGW59]` CGW n. 59 (1989), *688 Attack Sub*: https://archive.org/details/Computer_Gaming_World_Issue_59
- `[COMP111]` *Compute!* n. 111 (1989), *688 Attack Sub*: https://archive.org/details/1989-08-compute-magazine
- `[CGW119]` CGW n. 119 (1994), *Seawolf*: https://archive.org/details/Computer_Gaming_World_Issue_119
- `[CGW159]` CGW n. 159 (1997), *688(I)*: https://archive.org/details/Computer_Gaming_World_Issue_159
- `[SH-MH]` *Spillhistorie*, intervista a Michael Haire: https://spillhistorie.no/qas-with-michael-haire-former-art-director-at-microprose/

### Tecnica
- `[PCGPE-VGA]` Boone, «Programming the VGA Registers» (1994): https://bearstrong.net/tekst97/data/programmering/pcgpe10/vgaregs/
- `[WP-DP]` *Deluxe Paint*: https://en.wikipedia.org/wiki/Deluxe_Paint ; `[WP-CC]` *Color cycling*: https://en.wikipedia.org/wiki/Color_cycling
- `[EG-CC]` Effect Games, «Old School Color Cycling with HTML5» (2011): http://www.effectgames.com/effect/article-Old_School_Color_Cycling_with_HTML5.html
- `[SLY-1]` R. Schlitter, «Pixelblog 1 – Color Palettes»: https://www.slynyrd.com/blog/2018/1/10/pixelblog-1-color-palettes
- `[JY-DITH]` J. Yliluoma, «Arbitrary-palette positional dithering algorithm»: https://bisqwit.iki.fi/story/howto/dither/jy/
- `[MAC-HIG]` Apple, *Macintosh Human Interface Guidelines*, «Use a Consistent Light Source» (vedi `c_microprose_altri.md`)
- `[ONT-TXT]` R. Chen, «A sample of desktop icon text effects» (2004): https://devblogs.microsoft.com/oldnewthing/?p=37203
