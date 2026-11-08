# 25/B — Electronic Arts e Origin: l'aspetto delle schermate attorno alla missione

Nota di ricerca per la rifinitura estetica dell'interfaccia del remake (640×480, 256 colori): cornici, pulsanti,
titoli e sfondi. Gruppo B: i titoli Electronic Arts e Origin, dal nostro *688 Attack Sub* (1989) a *688(I)* (1997).
Il dossier generale sulle schermate d'epoca è `../15_schermate_simulatori.md`: questa nota parla solo di **aspetto**
(materiali, luce, caratteri, stati dei pulsanti) e non di flusso o punteggi.

**Convenzioni.**
- Le sigle fra parentesi quadre rimandano alle fonti in fondo.
- La confidenza:
  - **[alta]**: manuale letto; dati ricavati dai file originali del progetto (`re/cls`, `extracted/png`); recensione
    d'epoca letta per intero nella scansione OCR (il recensore descrive ciò che ha visto).
  - **[media]**: fonte secondaria credibile (Wikipedia, retrospettive, blog che rigioca il titolo), o dato coerente fra
    più fonti.
  - **[bassa]**: forum, ricordo, deduzione mia.
- «estratto» vuol dire che ho letto solo il riassunto del motore di ricerca.
- Le citazioni sono brevi e fra virgolette; il resto è riassunto con parole mie.

**Limiti della ricerca.** MobyGames e GameSpot rispondono 403 agli strumenti di lettura, quindi **non ho letto le
didascalie delle schermate di MobyGames** né le recensioni di GameSpot. Non ho scaricato immagini dal web. Per *688
Attack Sub* ho invece guardato le immagini e i listati **già estratti nel progetto** dai file originali (`TITLE.PAK`,
`688.PAK`, `ALFA.PAK`, `END*.ENC`, `SELECT.CLS`, `DIALOG.CLS`): sono fonte primaria. Dove una cosa non l'ho trovata
scritta, lo dico nel testo e la raccolgo nel §10.

---

## 1. *688 Attack Sub* (EA, 1989; John W. Ratcliff, Paul Grace, Randall Breen)

È il nostro punto di partenza. Il gioco è diviso in due eseguibili: `TITLE.EXE` mostra il titolo e la scelta,
`688.BAT` lancia poi `GAME n` [alta] [RE-DOC]. Grafica da Hercules a VGA; in VGA i fondali sono immagini 320×200 a
256 colori (`TITLE.PAK`, `688.PAK`, `ALFA.PAK`), mentre **i pannelli dell'interfaccia sono disegnati a comandi**
(rettangoli, linee, pixel, testo) con i 16 colori di base, dagli script `.CLS` [alta] [RE-DOC].

- **Titolo.**
  - Una foto digitalizzata di un sottomarino che emerge di prua in uno sbuffo di spuma, su mare e cielo azzurri
    [alta] [RE-PNG].
  - In alto «ELECTRONIC ARTS®» in rosso. Il titolo «688 ATTACK SUB» è un grande carattere **con grazie**, bianco
    con un contorno scuro. Sotto, «BY JOHN W. RATCLIFF» in rosso [alta] [RE-PNG].
  - In basso, in bianco e senza grazie, «PRESS A KEY TO BEGIN MISSION SELECTION», poi la riga del copyright in rosso
    col marchio EA [alta] [RE-PNG].
  - La musica del titolo suona solo con AdLib, Covox o Tandy [alta] [MAN-688]. Una retrospettiva ricorda la schermata
    come la sola immagine del sottomarino che riemerge [media] (estratto) [RFR-688].
- **Scelta della missione (MISSION SELECTION).** Fonte: `SELECT.CLS`, pagina 0 [alta] [RE-SELECT].
  - **Fondo e materiale:** tutto lo schermo è grigio chiaro (colore 7): una lamiera neutra, senza texture né
    dithering.
  - **Cornici:** tre pannelli, cioè la targa del titolo, l'elenco delle missioni e le colonne dei lati. Ogni pannello
    ha un **bordo incassato** di un pixel: lato alto nero, lato sinistro grigio scuro, lati destro e basso bianchi.
    Gli angoli sono smussati di un pixel. La luce viene dunque **dall'alto a sinistra** e il pannello sembra
    affondato nella lamiera.
  - **Viti:** ai quattro angoli interni di ogni cornice c'è una **testa di vite** di 3×3 pixel: un punto bianco
    sopra, grigio scuro a sinistra, nero a destra e sotto. È il motivo «pannello avvitato» di tutte le stazioni.
  - **Testo:** in rilievo, bianco con un'**ombra nera a +1,+1**. Il titolo «MISSION SELECTION» sta in una targa
    rivettata, al centro in alto. Le dieci missioni sono numerate da «1.» a «0.»: il numero è anche il tasto
    rapido.
  - **Caratteri:** il gioco ha un solo font bitmap proporzionale, alto 13 righe. Le maiuscole sono piccole (5×5),
    mentre gli spazi delle minuscole contengono **maiuscole grandi (7×7)**: scrivendo «mission  selection» in
    minuscolo si ottiene il titolo in maiuscolo grande. È un disegno squadrato, di gusto tecnico. «@» accende il
    grassetto, ottenuto ridisegnando ogni lettera spostata di 1 pixel [alta] [RE-DOC].
  - **Pulsanti di lato:** a destra di ogni missione c'è «688» in bianco con l'ombra **blu**, accanto a un pulsante
    quadrato con la lente blu. «ALFA» (o «700» per Torpex) ha l'ombra **rossa** e la lente rossa. Dove un lato non
    esiste, la casella resta vuota. Il colore dice **la parte**: blu per gli USA, rosso per i sovietici.
  - **Icone:** un **fulmine** bianco a zig-zag segna le sei missioni giocabili via modem (1, 2, 5, 7, 8, 9). Sono le
    stesse sei che cita Compute! [alta] [RE-SELECT], [COMP111].
- **Pannello di configurazione (CONFIGURATION PANEL).** Fonte: `SELECT.CLS`, pagina 2 [alta] [RE-SELECT].
  - Stesso grigio, stessa targa rivettata per il titolo, poi **sei pannelli avvitati** che raggruppano le voci: le
    uscite (BEGIN GAME, MISSION SELECTION, EXIT TO DOS); il suono (DEFAULT SOUND, COVOX LPT1/LPT2); il modem (NO
    MODEM, COM1/COM2, con i fulmini); SET PLAY LEVEL (STANDARD, BEGINNER); SET TELEPHONE NUMBER (undici cifre con le
    frecce UP/DN); ORIGINATE, AUTO-ANSWER, MANUAL ANSWER.
  - **Pulsante a spia:** ogni voce ha un tasto quadrato 11×10 in rilievo, con il filo esterno bianco in alto a
    sinistra e grigio scuro in basso a destra. Dentro c'è una lente colorata il cui bordo interno alto e sinistro è
    nel tono scuro dello stesso colore, come una lampadina incassata.
  - **Colori per gruppo:** verde e rosso per le uscite (BEGIN GAME verde chiaro, le altre due rosse), verde per il
    suono, magenta per il modem, ciano per il livello e il telefono, blu per la risposta.
  - Che la lente chiara segni la voce scelta e quella scura le altre è una mia **deduzione** dal listato, perché ogni
    area attiva porta un secondo colore (per esempio 2 o 4 accanto alla lente 10 o 12) [bassa].
  - Le scelte si salvano come configurazione predefinita [alta] [MAN-688].
- **Menu principale = la CONN.** Il manuale chiama la CONN il menu principale del gioco [alta] [MAN-688].
  - È un'illustrazione a tutto schermo della centrale [alta] [RE-PNG]. Sul 688 la sala ha pareti di quadri e
    strumenti, una colonna del periscopio cromata al centro e il tavolo della carta (verde e azzurra) in primo piano.
    I marinai in tuta blu sono uno al telefono e uno in cuffia. Toni blu, acciaio e azzurro, con sfumature morbide
    sui cilindri.
  - L'Alfa ha un'altra sala: verdi petrolio, schermi verde fosforo, uniformi kaki, una tazza rossa in primo piano
    [alta] [RE-PNG].
  - La versione EGA (`MAIN.EGA`) è la stessa scena in 16 colori, con la retinatura al posto delle sfumature [alta]
    [RE-PNG], [RE-DOC].
  - **Nessun pulsante disegnato:** le stazioni sono **zone calde dell'immagine**. Passandoci sopra, il cursore prende
    l'**icona della stazione** [alta] [MAN-688].
- **Stazioni (cruscotti).**
  - Stesso lessico della scelta: pannelli grigi con cornice incassata e viti agli angoli, etichette in rilievo
    bianche con l'ombra nera (TARGET, SEND CODE, MSG REVIEW, ESM LEVEL...) [alta] [RE-CLS].
  - Nella sala radio i tasti PREV e NEXT sono riquadri blu con la scritta ciano, TRANSMIT ha una spia rossa, e il
    livello ESM è una barra a segmenti su un pozzetto nero [alta] [RE-CLS].
  - Sull'Alfa le scritte dei comandi sono in **pseudo-cirillico** [alta] [CGW59].
  - Una barra di stato in alto (TARGET, BEARING, RANGE, COURSE, SPEED, DEPTH) e una in basso («SPACE TO PAUSE»)
    incorniciano ogni stazione [alta] [MAN-688].
- **Equipaggio.**
  - Gli uomini che rispondono agli ordini sono **foto digitalizzate di persone vere**. Compute! ne fa il primo
    motivo di vanto: «one of the best-looking games I've seen» [alta] [COMP111].
  - CGW aggiunge che i volti «look like you would expect them to» [alta] [CGW59].
  - Il ritratto estratto è una foto quasi in bianco e nero, a mezzobusto, con la cuffia [alta] [RE-PNG].
- **Briefing.** Gli ordini arrivano come testo nella sala radio, dopo il codice d'accesso. Il manuale dice che tutte
  le missioni cominciano e finiscono nella sala radio con il messaggio del CINC [alta] [MAN-688]. Non c'è una
  schermata di briefing illustrata.
- **Debriefing.**
  - L'esito è **una scrivania vista dall'alto**: legno, un calendario, buste azzurre, un telefono rosso, una matita,
    un tagliacarte. Nel riquadro nero al centro compare la carta dell'esito [alta] [RE-PNG].
  - Le carte (205×134) sono quattro:
    - il biglietto di condoglianze «With Sympathy and Honor from the U.S. Navy», rosa, in corsivo, con l'aquila e la
      bandiera;
    - la cartolina di **San Francisco** col Golden Gate, cioè la licenza a terra;
    - quella sovietica col ritratto di Lenin, «Comrade, The Navy Regrets To Inform You...»;
    - la cartolina «Greetings from Moscow» [alta] [RE-PNG].
  - CGW conferma la licenza meritata e la notifica di morte «laying on a desk» [alta] [CGW59].
  - Le missioni sono a esito secco: lode o ira dei superiori, senza punti né medaglie [alta] [COMP111].
- **Finestre di dialogo.**
  - Fondo grigio, **triplo filo nero, bianco, nero**, il titolo in rilievo bianco con l'ombra grigio scuro («DO YOU
    WANT TO...»).
  - Tre tasti a spia **verde** (PLAY AGAIN), **blu** (CONTINUE PLAY) e **rosso** (EXIT TO DOS) [alta] [RE-CLS].
  - L'OK delle finestre minori è un tasto giallo con la scritta nera [alta] [RE-PNG].
- **Cursore e icone.**
  - Sulle zone calde il cursore cambia colore e/o forma [alta] [MAN-688].
  - Per mirare (carta, periscopio) diventa un mirino a croce [alta] [MAN-688].
  - Le icone delle stazioni, le frecce e le cifre grandi sono **caratteri del font** dal codice 80h in poi [alta]
    [RE-DOC].
- **Palette e retinatura.**
  - Fondali VGA a 256 colori con palette propria; l'interfaccia usa i 16 colori di base [alta] [RE-DOC].
  - La retinatura c'è solo nelle versioni EGA e CGA dei fondali; in VGA le immagini sono sfumate [alta] [RE-PNG].
  - CGW giudica la grafica «spectacular in EGA and VGA» [alta] [CGW59].
- **Transizioni.** Nessuna fonte ne descrive. Dal titolo alla partita il gioco passa da un eseguibile all'altro
  attraverso il `.BAT`, quindi c'è un salto a schermo vuoto [bassa: deduzione da RE-DOC]. Il tasto F10 sostituisce lo
  schermo con un **finto prompt DOS** (il «boss key») [alta] [MAN-688].
- **Aiuto.** F1 sovrappone pagine di aiuto in testo, che si sfogliano con la barra spaziatrice [alta] [MAN-688],
  [RE-CLS].

## 2. *SSN-21 Seawolf* (EA, 1994; John W. Ratcliff e Paul Grace)

- **Introduzione.** È un'animazione «remarkably well-rendered»: una nave all'orizzonte lancia missili, poi un
  missile da sottomarino esce dall'acqua, fa un arco e distrugge la nave, con il suono stereo [alta] [CGW119].
- **Menu e scelta della missione.** Ci sono 33 missioni, giocabili in campagna o una per volta [alta] [CGW119]. Le
  recensioni lette **non descrivono** l'aspetto del menu né quello della scelta.
- **Stazioni.**
  - Rispetto a *688* il cambio è netto: «the familiar motif of individual stations is gone». Al loro posto ci sono
    **piccoli menu con la foto del membro dell'equipaggio** responsabile [alta] [CGW118].
  - I tasti funzione aprono navigazione, sonar, armi, comunicazioni e la «conn», che qui è il pannello delle
    preferenze. Ogni sistema è un menu di comandi più una **clip video digitalizzata** del marinaio [alta]
    [CGW119].
  - Le clip sono a bassa risoluzione ma «add loads of atmosphere» [alta] [CGW119].
- **Cruscotto principale.**
  - Il piano di battaglia è di solito una **carta navale vista dall'alto**, «crisp and easily read».
  - Un **riquadro di stato** sproporzionato si sposta nei quattro angoli della carta e copre i contatti [alta]
    [CGW119].
  - Le altre viste sono il periscopio (lodato), quella subacquea e una telecamera remota che ruota di 360° [alta]
    [CGW119].
- **Tono generale.** «state-of-the-art graphics», musica di The Fat Man [alta] [CGW118]. Home of the Underdogs la
  trova un bel passo avanti rispetto all'originale [media] [HOTU-SW].
- **Lacune.** Non ho trovato descrizioni di cornici, caratteri, briefing, debriefing né transizioni.

## 3. *Chuck Yeager's Air Combat* (EA, 1991; Brent Iverson)

- **Menu principale.** Le voci sono missione storica (WWII, Corea, Vietnam), Create Mission e Test Flight [alta]
  [COMP133].
  - Ogni voce è un **«bottone» d'opzione**: il tasto Tab sposta la freccia del cursore da un bottone all'altro e la
    barra la attiva.
  - **Il bottone predefinito è segnato da frecce ai lati**, nella forma >«testo»<, e Invio lo sceglie anche se il
    cursore è altrove.
  - Ogni voce ha la lettera iniziale come tasto rapido; Esc torna indietro di uno schermo [media] [YAC-DOC].
  - In volo ci sono **menu a tendina** con voci spuntabili, ricordate fra una sessione e l'altra [media] [YAC-DOC].
- **Scelta della missione.** La didascalia OCR di una schermata in CGW mostra la «MISSION SELECTION» come pagina di
  tre missioni: il titolo («Double Bounce», «Final Exam», «B-29 Intercept»), una riga di descrizione, un bottone di
  pagina precedente [alta] [CGW85]. I tasti 1, 2 e 3 scelgono le tre missioni visibili [media] [YAC-DOC].
- **Briefing e schede.** Nell'area di test e di briefing tattico ci sono «beautiful bit-mapped images» degli aerei,
  mentre in volo gli aerei sono poligoni pieni [alta] [CGW85]. Ogni cockpit è un'immagine VGA disegnata a mano [alta]
  [COMP133].
- **Statistiche (debriefing).** Una finestra «CUSTOM MISSION STATS» elenca aerei abbattuti, colpi sparati,
  percentuale di colpi a segno, tempo e stato dell'aereo, con un bottone «Done» [alta] [CGW85]. Yeager loda o
  rimprovera a voce [alta] [COMP133]. Non ci sono medaglie né promozioni [media] [PCP].
- **Volto di Yeager.** Una **finestra col ritratto del generale** compare con i suoi consigli [alta] [CGW85]. PC
  Pilot ne ricorda il «stern visage» [media] [PCP].
- **Film (replay).** Una schermata di playback a videoregistratore, con avanti, indietro e rallentatore, più una
  «box view» con gli aerei come punti in un cubo [alta] [CGW85], [media] [YAC-DOC].
- **Palette.** VGA «beautiful». L'orizzonte è **retinato** (CGW lo loda, ma nota i bordi seghettati in virata)
  [alta] [CGW85], [COMP133].
- **Lacune.** Non ho trovato descrizioni scritte di cornici, caratteri o transizioni.

## 4. *U.S. Navy Fighters* (EA, 1994; Brent Iverson) e *Marine Fighters* (1995)

Il manuale (edizione Gold) descrive il lessico dei comandi in modo molto preciso [alta] [USNF-MAN].

- **Menu principale (Choose Activity Screen).** È il «main menu»: missione singola, missione rapida, editor Pro,
  replay, nuova campagna, continua, configurazione, schede dei veicoli, stato di servizio dei piloti [alta]
  [USNF-MAN].
- **Lessico dei comandi, uguale in tutte le schermate di preparazione** [alta] [USNF-MAN]:
  - una **barra dei menu a tendina** in alto, alla maniera del Macintosh, con il menu «?» per uscire a DOS;
  - i bottoni di conferma con un **segno di tratteggio blu** («blue hash mark», tasto Invio);
  - i bottoni **verdi «Cancel»** (tasto Esc);
  - i **text button** che fanno ruotare le opzioni a ogni clic (sinistro avanti, destro indietro);
  - **interruttori e manopole** che agiscono a seconda del punto cliccato, con le scritte accanto alle posizioni;
  - oggetti da **trascinare**, come i waypoint e le armi.
- **Scelta della missione.** Un «Page switch» sfoglia le missioni e il clic sposta l'**evidenziatura gialla** sulla
  missione [alta] [USNF-MAN].
- **Fascicolo del pilota (Select Pilot / Pilot Service Record).**
  - È un **portablocco** (clipboard) a pagine, con nome, grado, nominativo e campagne.
  - Si sceglie la **foto del volto** («Prev Pic»/«Next Pic»), la **nose art** e la **tail art**; le medaglie stanno
    nelle pagine successive [alta] [USNF-MAN].
- **Briefing.**
  - Prima di ogni giornata c'è un **briefing video** dell'ufficiale informazioni e prima di ogni missione uno del
    comandante; la barra spaziatrice li salta [alta] [USNF-MAN].
  - Su CD ci sono più di 40 minuti di video; alcune missioni hanno chiacchiere fra piloti nella **ready room** [alta]
    [CGW127].
  - Poi viene la **Mission Brief Screen**, di nuovo un portablocco a pagine con l'intestazione dattiloscritta
    (nave, data, ora, meteo, SITUATION) [alta] [USNF-MAN].
- **Carta.** La Mission Map, con il pannello «Object Details» del waypoint scelto [alta] [USNF-MAN], e la carta del
  teatro per pianificare e assegnare le risorse [alta] [CGW127].
- **Hangar e armamento.**
  - Nella scelta dell'aereo ogni macchina ha uno stato («Fully Operational»).
  - Nel carico delle armi si scelgono aria-aria o aria-suolo cliccando le **lucine rosse** accanto alla manopola
    «Weapon Type», poi si trascinano le armi sui piloni [alta] [USNF-MAN].
- **Debriefing e riparazioni.** Dopo ogni missione la schermata Aircraft Repair mostra il ponte. **I danni si vedono
  dai «mules» gialli** (i trattori di servizio) attorno all'aereo: più sono, più l'aereo è danneggiato [alta]
  [USNF-MAN].
  - *Marine Fighters* aggiunge una schermata riassuntiva **«Tour of Duty»** con percentuali di tiro, abbattimenti e
    decorazioni [media] [WP-USNF].
- **Risoluzioni.** Il gioco va da 320×200 a 1024×768 [alta] [CGW127]. Non c'è cruscotto in volo: solo HUD e
  finestre a comparsa.
- **Lacune.** Non ho trovato descrizioni scritte di materiali, caratteri o transizioni dei menu.

## 5. *Jane's ATF: Advanced Tactical Fighters* (Jane's/EA, 1996; Andre Gagnon, Brent Iverson)

- **Menu principale.** Dopo un video d'apertura «exhilarating» si arriva a una schermata principale «nearly
  identical to USNF's» [alta] [CGW143]. Wikipedia conferma lo stesso motore e gli stessi menu [media] [WP-ATF].
- **Schede.** C'è una nuova **Player Aircraft Reference** con video e dati di Jane's [alta] [CGW143]. Per un altro
  recensore l'enciclopedia è il pezzo migliore e la grafica «a mixed bag» [media] [WP-ATF].
- **Cruscotti.** La grafica del cockpit serve solo a reggere lo specchietto; gli strumenti sono **fino a sei finestre
  a comparsa** [alta] [CGW143].
- **Lacune.** Come per *USNF*: niente sull'aspetto di cornici e caratteri.

## 6. *Strike Commander* (Origin, 1993; Chris Roberts)

Le schermate fuori dal volo sono **luoghi**, non menu [alta] [SC-MAN].

- **Avvio.** Dopo una breve animazione c'è il menu d'avvio: nuova partita, missione d'addestramento, oggetti in 3D,
  carica, continua [alta] [SC-MAN].
- **Fascicolo del pilota.** Il pilota nasce compilando la **«Mercenary Defense License Application»**, un modulo con
  cognome, nome e nominativo [alta] [SC-MAN].
- **Hub: l'hangar della base Wildcat.** Ogni elemento della scena è un comando [alta] [SC-MAN]:
  - la porta destra porta alla **camerata** (salvataggi);
  - la porta sinistra porta all'**ufficio** di Virgil;
  - la jeep va al bar Selim's;
  - il camion carica la squadriglia per le missioni all'estero;
  - l'F-16 sulla soglia avvia briefing e volo;
  - i piloti in piedi si cliccano per parlarci.
- **Oggetti come pulsanti** [alta] [SC-MAN]:
  - in camerata, una **pilota che dorme** in branda carica la partita, il **letto vuoto** la salva;
  - in ufficio, il **libro mastro aperto** mostra i conti, il **catalogo con la calcolatrice** gli acquisti, la
    lavagna degli abbattimenti la classifica;
  - nel catalogo le pagine si girano cliccando gli angoli in alto, ogni acquisto genera una **ricevuta** sullo
    schermo e la calcolatrice tiene i totali;
  - al bar ci si accorda **stringendo la mano** del procacciatore.
- **Ritorno senza pulsante.** Si torna indietro **cliccando lungo il bordo alto o basso dello schermo** [alta]
  [SC-MAN].
- **Briefing e armamento.** Il briefing è parlato, poi viene il carico delle armi: l'F-16 nell'hangar (o davanti alla
  tenda) e le **rastrelliere** di armi da cliccare, caricate a coppie. Si vola cliccando l'abitacolo [alta] [SC-MAN].
- **Pannelli delle opzioni.**
  - Sono gli unici con pulsanti veri: difficoltà rookie/veteran/ace, dettaglio, telecamere, comandi, suono.
  - **I pulsanti non disponibili appaiono «flat»**, cioè piatti invece che in rilievo [alta] [SC-MAN]. È lo stato
    «disabilitato» più chiaro che ho trovato scritto.
  - Il pannello del dettaglio ha una **finestrella d'anteprima** che mostra l'effetto delle scelte [alta] [SC-MAN].
- **Stile.**
  - Gli intermezzi a terra hanno «the quality of a graphic novel» [alta] [COMP157].
  - Nei crediti si leggono «3-D studio sets and meshes», «Backgrounds», «Character animations» e **«Menus and fonts»
    (Bruce Lemmons)**, e un cast di attori per i personaggi [alta] [SC-MAN].
  - Quindi i set sono modellati in 3D e i personaggi presi da attori; il dettaglio del procedimento non è scritto
    [media].
  - Poche scene di intermezzo sono doppiate e c'è molto testo da leggere [alta] [CGW109].
- **Palette.** VGA a 256 colori. Il motore RealSpace usa ombreggiatura Gouraud, texture e «dithered colors» [alta]
  [SC-MAN], [media] [FS-SC].
- **Lacune.** Non ho trovato descritte la forma dei pulsanti, le transizioni (oltre all'animazione iniziale) né il
  cursore.

## 7. *Jane's AH-64D Longbow* (Origin, 1996; Andy Hollis) e *Longbow 2* (1997)

- **Chi ha disegnato i menu.** È documentato bene nella guida ufficiale di Origin [alta] [LB-GUIDE]:
  - le **«wrapper screens»** (briefing, debriefing, scelta della missione, interfaccia del videoregistratore, piano di
    missione, lettore dei filmati) sono state programmate da Steve Muchow;
  - il direttore artistico Paul Stankiewicz ha fatto le introduzioni e «many of the other screens», modellando in
    **Alias su SGI** per dare un aspetto realistico e comune;
  - Carol Roberts e Gary McElhaney hanno lavorato ad altre schermate (Roberts ha fatto il logo ORIGIN Skunkworks) e
    Brian Smith ad arte e animazioni delle schermate;
  - i modelli 3D dei mezzi servono sia alle animazioni sia alle texture del volo.
- **Video.** Una piccola ripresa con attori («newscaster») è stata integrata nella grafica 3D perché non sembrasse
  «campy» [alta] [LB-GUIDE]. Nella campagna gli intermezzi video danno i notiziari e scene come **il comandante che
  consegna la promozione** [alta] [CGW144].
- **Menu principale.** L'introduzione «fades into» una base, che fa da interfaccia per spostarsi fra le sezioni del
  gioco [media] [PIX-LB]. Il blogger la chiama «naval base»: forse è una svista per una base dell'esercito. SimHQ
  parla di un menu animato [bassa] (estratto) [SIMHQ-LB]. **Non ho trovato una descrizione degli edifici o degli
  oggetti cliccabili.**
- **Fascicolo del pilota.** Si crea il pilota scegliendo il nominativo da una lista fissa [media] [PIX-LB]. Il gioco
  tiene progressi, promozioni e medaglie [alta] [CGW144].
- **Briefing.** Una carta della zona e un testo; il blogger nota che manca la voce [media] [PIX-LB]. Gli ordini
  d'operazione (Op Orders) sono scritti in stile militare [alta] [LB-GUIDE].
- **Debriefing.**
  - Il punteggio appare **sulla lavagna («chalkboard»)** del debriefing [alta] [LB-GUIDE].
  - Dopo uno schianto si riprova **cliccando il casco di volo** nell'area del debriefing: un oggetto della scena come
    pulsante [alta] [LB-GUIDE].
- **Replay.** I comandi da videoregistratore permettono di rivedere e di riprendere il volo in qualsiasi punto [alta]
  [CGW144].
- **Palette.** VGA e SVGA (VESA) [media] [DOSDAYS-LB].
- ***Longbow 2* (Windows 95).** Il piano di missione è «exceptionally full-featured», ma l'interfaccia è scomoda e i
  debriefing «rather terse» [alta] [CGW164]. Le missioni singole si aprono **cliccando il portablocco** nel «Mission
  Planning Center» [alta] [LB2-GUIDE].
- **Lacune.** Non ho trovato scritti materiali, cornici, caratteri o stati dei pulsanti.

## 8. *688(I) Hunter/Killer* (Sonalysts/Jane's, 1997; Windows 95)

- **Cruscotti.**
  - Il giocatore sta al centro della **centrale**, che riunisce più posti di guardia: governo, sonar, radar, radio ed
    ESM, periscopio, carte, TMA [alta] [CGW159].
  - Il sonar a cascata mostra un contatto come una riga di pixel vivi; GameRevolution precisa «brighter green pixels
    against a black or dark green field of noise» [alta] [CGW159], [media] [GR-688I].
- **Schede.** Lo **Ship's Information Book** e un estratto di *Jane's Fighting Ships* [alta] [CGW159].
- **Menu.** Ci sono addestramento, missioni singole, campagna (Caraibi, Mediterraneo, Golfo, Pacifico del nord) ed
  editor [alta] [CGW159]. **Le recensioni lette non descrivono l'aspetto dei menu, del briefing né del debriefing.**
- **Grafica.** Navi con ombreggiatura Gouraud e texture, eliche che girano e scie [media] [GR-688I]. Requisiti:
  SVGA con DirectX [alta] [CGW159].
- **Evoluzione dello stile.** Rispetto a *688* (1989) la **sala** resta il centro dell'interfaccia, ma diventa una
  serie di **schermi-stazione** pieni di dati, e i ritratti fotografici spariscono. Gli ordini li ripete il secondo a
  voce [media] [GR-688I]. Il confronto è mio [bassa].

---

## 9. Lo stile di casa EA/Origin (sintesi)

1. **Il pannello avvitato** (*688*): lamiera grigia, cornici incassate di un pixel, viti agli angoli. È il segno più
   riconoscibile del nostro originale e costa pochissimi pixel [alta].
2. **La luce viene dall'alto a sinistra** in tutto *688*: filo bianco in alto a sinistra e scuro in basso a destra
   sui tasti in rilievo; l'inverso sulle cornici incassate; ombra del testo a +1,+1 [alta].
3. **Il testo inciso**: bianco con l'ombra scura, tutto maiuscolo, un solo font squadrato in due misure (titoli e
   etichette). Il colore dell'ombra può portare un significato: blu per i 688, rosso per gli Alfa [alta].
4. **Il colore dice la parte e il gruppo**: blu per gli USA e rosso per i sovietici; nel pannello di configurazione
   ogni gruppo ha la sua tinta di spia [alta].
5. **Il pulsante a spia**: un tasto quadrato in rilievo con una lente colorata, invece del bottone con la scritta
   dentro. Lo stato si legge dal colore della lente [alta per la forma, bassa per il significato chiaro/scuro].
6. **Le icone sono poche e funzionali**: il fulmine del modem, le icone delle stazioni come forme del cursore, il
   mirino [alta].
7. **Il menu come luogo illustrato**: la CONN di *688*, l'hangar di *Strike Commander*, la base di *Longbow*. Si
   clicca su porte, oggetti e persone [alta per 688 e Strike, media per Longbow].
8. **L'oggetto come pulsante**: la cartolina sulla scrivania (*688*), il libro mastro, la ricevuta, la stretta di
   mano, il letto vuoto (*Strike*), il casco e la lavagna (*Longbow*), il portablocco (*USNF*, *Longbow 2*) [alta].
9. **Carta e cancelleria per i documenti**: i portablocchi a pagine (*USNF*), il modulo di licenza (*Strike*), le
   cartoline e i biglietti (*688*), gli ordini in stile militare (*Longbow*) [alta].
10. **Il volto umano**: le foto digitalizzate dell'equipaggio (*688*), le clip video del marinaio (*Seawolf*), il
    ritratto di Yeager, i video di briefing (*USNF*), il comandante che promuove (*Longbow*) [alta].
11. **Il carattere nazionale nella grafica**: lo pseudo-cirillico dell'Alfa, Lenin e Mosca sulle cartoline, l'aquila
    sul biglietto della Navy [alta].
12. **Gli stati dei pulsanti si dicono per contrasto, non per testo**: piatto vuol dire non disponibile (*Strike*);
    l'evidenziatura gialla (*USNF*); le frecce >«testo»< sul predefinito (*Yeager*) [alta].
13. **Dalla VGA dipinta al rendering**: dal 1989 al 1991 immagini disegnate a mano o digitalizzate con la palette
    propria; dal 1993 set 3D (3D Studio, poi Alias su SGI) e attori filmati [alta].
14. **Il debriefing è povero e personale**: lode o rimprovero di un superiore, una carta, una lavagna, più che
    tabelle. *Marine Fighters* è l'eccezione, con il «Tour of Duty» [alta e media].
15. **Una sola grammatica dei comandi per tutto il gioco** (*USNF*): lo stesso tasto blu per «avanti», lo stesso
    verde per «Cancel», la stessa barra dei menu in ogni schermata di preparazione [alta].

## 10. Che cosa non ho trovato scritto

- Le **transizioni** (dissolvenze, tende, scorrimenti) non sono descritte per nessun titolo, tranne la dissolvenza
  dall'introduzione alla base di *Longbow* [media] [PIX-LB].
- I **cursori** sono descritti solo per *688* (cambia colore o forma, icona della stazione, mirino) e per *Yeager* (la
  freccia).
- **Cornici, caratteri e stati dei pulsanti** di *Seawolf*, *Longbow*, *Longbow 2*, *ATF* e *688(I)*: nessuna fonte
  letta li descrive.
- **Le didascalie di MobyGames** non sono leggibili (errore 403). Andranno lette a mano, oppure si potranno guardare le
  schermate dai giochi installati, se il progetto li ha.

---

## Fonti

**Primarie del progetto (file originali di *688 Attack Sub*)**
- `[MAN-688]` *688 Attack Sub*, manuale (testo in inglese): `/home/randolph/Claude/melchior-688/docs/originale/manuale_EN.txt`
- `[RE-DOC]` Analisi tecnica dell'originale: `/home/randolph/Claude/melchior-688/docs/01_analisi_tecnica_originale.md`
- `[RE-SELECT]` Listato e ridisegno di `SELECT.CLS` (scelta della missione e configurazione): `/home/randolph/Claude/melchior-688/re/cls/SELECT.CLS.txt`, `SELECT.CLS.png`
- `[RE-CLS]` Listati delle altre pagine (`RADIO.CLS`, `DIALOG.CLS`, `HELP.CLS`...): `/home/randolph/Claude/melchior-688/re/cls/`
- `[RE-PNG]` Immagini estratte (`TITLE`, `688`, `ALFA`, `MAIN` EGA, `END0`–`END4`, `CREW*`, `DIALOG*`): `/home/randolph/Claude/melchior-688/extracted/png/`

**Manuali e guide (OCR archive.org e altri)**
- `[SC-MAN]` *Strike Commander* manual (Origin, 1993): https://archive.org/details/StrikeCommanderManual
- `[USNF-MAN]` *U.S. Navy Fighters Gold* PC manual (EA, 1995): https://archive.org/details/u.-s.-navy-fighters-gold
- `[LB-GUIDE]` *Jane's AH-64D Longbow* Official Guide (Origin/Incan Monkey God, 1996), con le interviste e i crediti: https://archive.org/details/Janes_AH-64D_Longbow_Guide
- `[LB2-GUIDE]` *The Official Guide to Jane's Longbow 2* (Prima/Incan Monkey God, 1997): https://archive.org/details/Janes_Longbow_2_Guide
- `[YAC-DOC]` *Chuck Yeager's Air Combat*, scheda di riferimento dei comandi (testo da textfiles.com): https://mirror.cyberbits.eu/textfiles.com/piracy/SOFTDOCS/yeager.txt
- `[PCP]` Steve Smith, *PC Pilot*, pagina su *Yeager's Air Combat*: https://www.flightsimbooks.com/pcpilot/page81.php

**Recensioni d'epoca**
- `[CGW59]` H. E. Dille, «Silent Running», *Computer Gaming World* n. 59, maggio 1989, pp. 32–33: https://archive.org/details/Computer_Gaming_World_Issue_59
- `[COMP111]` Richard Sheffield, «688 Attack Sub», *Compute!* n. 111, agosto 1989, p. 64: https://archive.org/details/1989-08-compute-magazine
- `[CGW85]` recensione di *Chuck Yeager's Air Combat*, *Computer Gaming World* n. 85, agosto 1991, pp. 16–17 e 22 (nome dell'autore illeggibile nell'OCR): https://archive.org/details/Computer_Gaming_World_Issue_85
- `[COMP133]` Denny Atkin, *Chuck Yeager's Air Combat*, *Compute!* n. 133, settembre 1991, p. 106: https://atarimagazines.com/compute/issue133/106_Chuck_Yeagers_Air_C.php
- `[CGW109]` Tom «KC» Basham, *Strike Commander*, *Computer Gaming World* n. 109, agosto 1993, pp. 130–131: https://archive.org/details/Computer_Gaming_World_Issue_109
- `[COMP157]` *Strike Commander*, *Compute!* n. 157, ottobre 1993, p. 103: https://atarimagazines.com/compute/issue157/103_REVIEWS_STRIKE_COMMANDER.php
- `[CGW118]` scheda di *Seawolf*, *Computer Gaming World* n. 118, maggio 1994: https://archive.org/details/Computer_Gaming_World_Issue_118
- `[CGW119]` Robert L. Hayes Jr., «Leagues Of Its Own», *Computer Gaming World* n. 119, giugno 1994, p. 77 e seguenti: https://archive.org/details/Computer_Gaming_World_Issue_119
- `[CGW127]` Denny Atkin, «Bird On A Three Wire» (*U.S. Navy Fighters*), *Computer Gaming World* n. 127, febbraio 1995, pp. 154–155: https://archive.org/details/Computer_Gaming_World_Issue_127
- `[CGW143]` Denny Atkin, «Plane Jane's» (*ATF*), *Computer Gaming World* n. 143, giugno 1996: https://archive.org/details/Computer_Gaming_World_Issue_143
- `[CGW144]` Denny Atkin, «Chopping Up The Competition» (*AH-64D Longbow*), *Computer Gaming World* n. 144, luglio 1996: https://archive.org/details/Computer_Gaming_World_Issue_144
- `[CGW159]` Kevin Turner, «Deep Sim» (*688(I) Hunter/Killer*), *Computer Gaming World* n. 159, ottobre 1997: https://archive.org/details/Computer_Gaming_World_Issue_159
- `[CGW164]` Robin G. Kim, recensione di *Longbow 2* (titolo poco leggibile nell'OCR, «Longbow & Resurrection»?), *Computer Gaming World* n. 164, marzo 1998: https://archive.org/details/Computer_Gaming_World_Issue_164

**Secondarie**
- `[WP-USNF]` Wikipedia, *U.S. Navy Fighters*: https://en.wikipedia.org/wiki/U.S._Navy_Fighters
- `[WP-ATF]` Wikipedia, *Jane's ATF: Advanced Tactical Fighters*: https://en.wikipedia.org/wiki/Jane%27s_ATF:_Advanced_Tactical_Fighters
- `[GR-688I]` GameRevolution, «Dive! Dive! Dive! Review» (1/9/1997): https://www.gamerevolution.com/?p=33088
- `[HOTU-SW]` Home of the Underdogs, *SSN-21 Seawolf*: https://www.homeoftheunderdogs.net/game.php?id=1039
- `[PIX-LB]` Pix's Origin Adventures, «Day 184 – AH-64D Longbow» (23/2/2009): https://www.pixsoriginadventures.co.uk/day-184-ah-64d-longbow/
- `[PIX-LB2]` Pix's Origin Adventures, «Day 219 – Longbow 2» (8/11/2010): https://www.pixsoriginadventures.co.uk/day-219-longbow-2
- `[DOSDAYS-LB]` DOS Days, *Longbow*: https://dosdays.co.uk/topics/Games/game_longbow.php
- `[FS-SC]` Fabien Sanglard, «Reverse Engineering Strike Commander»: https://fabiensanglard.net/reverse_engineering_strike_commander/
- `[SIMHQ-LB]` SimHQ, «Second Look» su *Longbow* (solo estratto, la pagina risponde 403): https://www.simhq.net/_air10/air_320a.html
- `[RFR-688]` Retro Freak Reviews, *688 Attack Sub* (solo estratto): https://retrofreakreviews.com/2017/04/03/688-attack-sub-review/
