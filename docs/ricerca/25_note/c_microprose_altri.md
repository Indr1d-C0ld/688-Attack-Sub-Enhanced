# 25c — L'estetica delle schermate: MicroProse e gli altri simulatori navali e aerei

Nota di ricerca per la rifinitura grafica dell'interfaccia del remake di *688 Attack Sub* (cornici, pulsanti, titoli,
sfondi delle schermate attorno alla missione). Gruppo **C**: MicroProse e il confronto (SSI, NovaLogic, Spectrum
HoloByte, Three-Sixty, Digital Integration). Dynamix ed EA sono coperti dagli altri due ricercatori. Solo ricerca,
nessun codice. Compilata il 28 ottobre 2026.

---

## 0. Metodo, convenzioni, limiti

- **Non si ripete** il flusso delle schermate, i punteggi, le decorazioni né le promozioni: sono già in
  `ricerca/15_schermate_simulatori.md` (§1–§5), di cui riuso le sigle delle fonti dove coincidono.
- **Confidenza**: **[alta]** = manuale letto nel testo (OCR di archive.org o testo del PDF) o altra fonte primaria
  (note dei progettisti, crediti); **[media]** = recensione d'epoca o fonte secondaria credibile letta per intero;
  **[bassa]** = forum, ricordo, deduzione mia. **«estratto»** = letto solo nel riassunto del motore di ricerca.
  **PROPOSTA** = indicazione per il remake, non un fatto.
- **Nessuna immagine guardata o copiata**: tutte le descrizioni vengono da testi (manuali, recensioni, didascalie). Due
  manuali in PDF (*Fleet Defender*, *Falcon 3.0*) sono stati letti estraendone il testo; dei manuali OCR di archive.org
  si è letto il testo integrale.
- **Limite principale, da dire subito**: i manuali descrivono le schermate **per funzione** (dove cliccare, che cosa
  cambia), quasi mai per **materiale** (legno, metallo, rivetti) o per **tipografia**. Le recensioni d'epoca parlano
  della grafica 3D in volo molto più che delle schermate di contorno («wrapper screens», come le chiama il manuale di
  *F-15 III*). Dove una voce richiesta (cornici, font, transizioni) non è documentata, lo scrivo come **[lacuna]**
  invece di inventarla.

---

## 1. MicroProse

### 1.1 *Red Storm Rising* (1988; C64, poi PC EGA, Atari ST, Amiga)

Fonti: manuale [RSR-MAN] **[alta]**; recensione *Zzap!* (estratto) [ZZAP-RSR].

- **Stazioni (Attack Center)**: lo schermo è diviso in tre aree fisse: un *Navigation Display* sempre presente, un
  *Primary Display* con otto scelte e un *Secondary Display* con cinque; in alto le sigle dei sensori attivi, un
  indicatore di volume acustico e una zona dei rapporti verbali (p. 11) **[alta]**. È un'impaginazione «a cruscotto»
  con riquadri fissi, non a finestre.
- **Il colore come dato**: i contatti nascono come simboli «dim» (attenuati) e **cambiano colore** quando la soluzione
  migliora; persa la traccia resta una posizione stimata più scura; la parola CONTACT cambia colore se il contatto è
  vivo o perso (pp. 13–14) **[alta]**. Le condizioni dell'acqua si sovrappongono come **zone più scure con meno
  punti**: una texture a puntini usata come informazione (p. 13) **[alta]**.
- **Barre**: l'analisi sonar usa barre orizzontali in tre sezioni, con quella sinistra scura per la ricezione scarsa
  (p. 17) **[alta]**. Fra le missioni, una **barra rossa e blu** mostra l'andamento della guerra (p. 33) **[alta]**.
- **Carta strategica**: i simboli cambiano **colore o forma** con l'età dell'avvistamento (p. 32) **[alta]**.
- **Periscopio**: la parte bassa dello schermo è il «video» della vista (p. 19) **[alta]**.
- **Pulsanti e stati**: si sceglie **evidenziando** un'opzione con il *Controller* e confermando con il *Selector*
  (pp. 8, 32) **[alta]**; l'aspetto dell'evidenziazione non è descritto **[lacuna]**.
- **Chi l'ha disegnato**: «Original Computer Graphics» di Murray Taylor con Max Remington; grafica del manuale di
  Taylor con Barbara Bents e Jackie Ross (crediti) **[alta]**.
- **Giudizio d'epoca**: su C64 niente 3D vero, ma carte animate, grafici e periscopio [ZZAP-RSR] (estratto)
  **[bassa]**.
- Cornici, materiali, font, transizioni, palette dei singoli modi grafici: **[lacuna]**.

### 1.2 *Silent Service II* (MPS Labs 1990; DOS, poi Amiga e Atari ST)

Fonti: manuale tedesco [SS2-MAN] **[alta]**; recensioni [TXT-SS2], [STN-SS2], [AR-SS2], [COMP-SS2], [ACE-SS2]
**[media]**; [WP-SS2] **[media]**.

- **Modi grafici e palette**: l'appendice tecnica elenca VGA/MCGA a 256 colori (640 K di memoria, definita «sehr
  beeindruckend»), EGA a 16, Tandy 1000 a 16 («quasi come EGA»), CGA a 4 colori (con il consiglio di sostituire la
  scheda) [SS2-MAN] **[alta]**. La recensione testuale conferma 320×200 a 256 colori e giudica la VGA
  «simply stunning» [TXT-SS2] **[media]**.
- **Tabella dei colori della carta per ciascun modo** [SS2-MAN] (OCR disordinato: l'associazione riga-colore è una mia
  lettura, quindi **[media]**): proprio battello **giallo con prua nera**; nemico avvistato **bianco**, al radar
  **grigio**, al sonar **nero**, tutti con prua nera; scia **azzurro pallido**; acque profonde **blu scuro**, basse
  **azzurro**. In CGA le acque diventano azzurro con **puntinatura nera** (profonde) o **bianca** (basse): il
  **dithering come ripiego** quando mancano i colori, documentato dal manuale stesso.
- **Carta**: quattro scale; il battello è una linea con la scia che ne dice rotta e velocità; le acque più chiare sono
  più basse (p. 68) **[alta]**.
- **Stazioni**: una **striscia d'informazioni** in basso, comune a carta, ponte e periscopio (profondità, velocità,
  rotta, rilevamento, tubi carichi, luce del TDC) (p. 67) **[alta]**; **icone di stato nell'angolo in basso a destra**
  di quasi tutte le viste (tempo, suono, animazione, velocità dei siluri, radar, periscopio) (p. 75) **[alta]**.
- **Strumenti**: il «Weihnachtsbaum» (albero di Natale) con i portelli aperti in **rosso** e chiusi in **verde**; la
  livella a bolla; il carburante in tre colonne, scure per la nafta e chiare per l'acqua; nella pagina dei danni i
  sistemi colpiti **si illuminano** [SS2-MAN] **[alta]**.
- **Immagini delle navi**: fotografie **digitalizzate di modelli in scala** di ogni tipo giapponese, ricostruiti da
  uno storico; senza memoria sufficiente le figure spariscono [TXT-SS2] **[media]**. Sull'Amiga a 16 colori le foto
  sono **in scala di grigi** e finiscono per sembrare «grey blobs» tutte uguali [AR-SS2] **[media]**. Il libro di
  riconoscimento è una **pagina del libro** sullo schermo, con didascalie «Abbildung» (p. 63) **[alta]**.
- **Menu (versione Atari ST)**: sfondo con un'**immagine in scala di grigi** e testo delle scelte **in rosso**,
  difficile da leggere su un televisore [STN-SS2] **[media, vale per la ST]**.
- **Transizioni**: brevi animazioni (siluro che esce dal tubo, colonne d'acqua) **disattivabili**; pause di
  caricamento a ogni cambio di vista [STN-SS2], [TXT-SS2] **[media]**.
- **Chi l'ha disegnato**: artista Kim Biscoe [WP-SS2] **[media]**. Il manuale ha «clear diagrams and solid
  illustrations» e una carta del Pacifico a colori in poster [COMP-SS2] **[media]**.
- Schermata d'encomio o rimprovero, albo d'onore, cornici, font: **[lacuna]**.

### 1.3 *Task Force 1942* (MPS Labs 1992)

Fonti: [WP-TF42] **[media]**; altre estratte.

- Motore 3D derivato da quello di *F-117A* [WP-F117], [WP-TF42] **[media]**; artista accreditato Max D. Remington III
  [WP-TF42] **[media]**.
- *Computer Gaming World* (1993) lodò **grafica** e profondità e criticò **l'interfaccia** e l'IA tattica [WP-TF42]
  **[media]**.
- Il libro di riconoscimento di circa 100 pagine con l'illustrazione di ogni nave serviva da protezione dalla copia
  (estratto) **[bassa]**. Un appassionato ricorda come notevoli le scene notturne con proiettili traccianti e navi in
  fiamme (estratto) **[bassa]**.
- Manuale non trovato in testo: cornici, schermate di contorno, pulsanti **[lacuna]**.

### 1.4 *F-117A Nighthawk Stealth Fighter 2.0* (MPS Labs 1991)

Fonti: manuale e note dei progettisti [F117-MAN] **[alta]**; [DD-F117], [WP-F117] **[media]**.

- **Scelta dichiarata dai progettisti**: il «front and end» del gioco fu **ridisegnato completamente** per rendere più
  facile la navigazione e per dare spazio a «beautiful 256-color graphics» (note, pp. 179–180) **[alta]**. Le
  schermate d'apertura e di chiusura sono di **Kim Biscoe e Barbara Bents**; la programmazione di quelle schermate di
  Ed Fletcher; crediti «Computer Graphics: Kim Biscoe, Barbara Bents, and Max Remington III with Chris Soares and Todd
  Brizzi» **[alta]**. Tutti gli oggetti 3D furono **ricolorati** (e alcuni ricostruiti) per passare dai 16 colori EGA
  di *F-19* ai 256 **[alta]**.
- **Albo**: si presenta come **«Squadron Bulletin Board»** con l'intestazione «Pilots on Alert»; i nomi d'esempio sono
  quelli dei progettisti con grado e soprannome (scherzo interno); **cursore a freccia**, e i nomi **si evidenziano**
  al passaggio (pp. 27–28) **[alta]**.
- **Ready Room**: una stanza con **porte** verso le altre stanze; la bacheca **sulla parete di fondo** riporta
  all'albo; si clicca la porta con la freccia (p. 30) **[alta]**. Pulsante 2, tasto destro o Esc **riportano sempre**
  alla Ready Room **[alta]**.
- **Sala briefing**: carta con T, P, S, L, menu a destra; un **riquadro d'informazioni** compare sui siti; cerchi di
  portata **pieni** per i radar Doppler e **tratteggiati** per quelli a impulsi (pp. 33–34) **[alta]**.
- **Hangar**: l'aereo «topped off» di carburante e armato, poi la domanda sì/no (p. 35) **[alta]**.
- **Il colore come codice, con appendice**: un «Display Colors Summary» elenca rettangoli neri, marroni e bianchi,
  ovali bianchi e rossi, barre rosse, rosa, gialle, azzurre e bianche, aerei gialli, rosso chiaro e rosso scuro,
  voci «gray-boxed» e «color-boxed» (p. 176) **[alta]**. Il colore nei menu è funzione prima che decorazione.
- **Modi**: EGA, MCGA e VGA [DD-F117] **[media]**; grafica giudicata «beautiful in 1991» [DD-F117] **[media]**.
- Aspetto di debriefing, medaglie, giornale; font; transizioni: non descritti nel testo letto **[lacuna]**.

### 1.5 *F-15 Strike Eagle III* (MPS Labs 1992)

Fonti: manuale e note del produttore [F15III-MAN] **[alta]**; [WP-F15III] **[media]**.

- **Lo stile di casa dichiarato**: «veteran artist» **Barb Bents** diresse le *wrapper screens* e definì «a consistent
  photo-realistic style» per tutto il prodotto, disegnando lei stessa quasi tutte le schermate e animazioni; **Frank
  Vivirito**, con strumenti CAD, costruì un **modello virtuale di una base aerea** come **palcoscenico** di tutte le
  schermate di contorno (note del produttore, pp. 199–200) **[alta]**. Il team provò la Super VGA e la abbandonò:
  «we were wrong about SVGA, it just was not there yet» **[alta]**.
- **Crediti grafici**: Art Direction Barbara Bents; Art di Bents, Vivirito, Edward Bendett, Andy Lakin, Erroll
  Roberts, Murray Taylor e altri; *Interface Design and Programming* di Detmar Peterke **[alta, OCR]**.
- **Home Screen = hangar**: in primo piano il comandante con le **carte della campagna spiegate su un tavolo**
  (THEATERS); sullo sfondo l'F-15 parcheggiato (QUICK START), un **edificio di pietra marrone** (REALITY), la fila di
  armadietti (ROSTER), la **porta sulla parete sinistra** (BRIEFING) **[alta]**.
- **Cursore parlante**: passando su un punto attivo il cursore **«legge»** il nome della funzione (QUICK START,
  THEATERS, REALITY, ROSTER, MISSION, DECLINE MISSION, TAKE OFF, HOME, MENU) **[alta]**. È l'etichetta che fa da
  pulsante: niente bottoni disegnati sulla scena.
- **Barra dei menu nascosta**: compare portando il cursore al bordo superiore (MENU) **[alta]**.
- **Albo = spogliatoio**: otto **armadietti** da far scorrere; chiusi dicono SELECT, aperti MEDALS o NEW PILOT; il
  numero sull'armadietto è il punteggio; dentro la **scatola** di nastrini e medaglie sullo scaffale, tuta, tuta anti-g,
  sacca personale; al nuovo pilota un casco; al pilota «recuperato» una **piccola medaglia-ricordo** **[alta]**.
- **Sala briefing**: **lavagna bianca** con primo piano a schermo intero, carta a sinistra, **televisore** con i
  filmati del bersaglio, **cestino** per rifiutare (DECLINE MISSION) **[alta]**.
- **Armamento**: stazione corrente **evidenziata**, finestra d'informazioni sull'arma, carico in alto a destra, schema
  del ventre dell'F-15; confermato il carico, i pannelli **vengono cancellati** per mostrare l'aereo armato (una
  transizione per sottrazione); il cursore sul tettuccio dice TAKE OFF **[alta]**.
- **Cursori in cabina**: oltre alla freccia, un cursore a **«captain's bars»** (i gradi da capitano) che diventa il
  simbolo ACQ sul radar, poi **mirino a croce**, **rombo** in modo aria-suolo, **X** nelle zone non valide **[alta]**.
- **Debriefing**: **replay animato** degli eventi principali in ordine cronologico, poi punteggio e decorazioni
  **[alta]**.
- Materiali (oltre a lavagna, pietra, armadietti), font, palette: **[lacuna]**. Accoglienza: grafica lodata, lentezza
  criticata [WP-F15III] **[media]**.

### 1.6 *Gunship 2000* (MicroProse 1991)

Fonti: [COMP-G2K], [CSAR-G2K1], [CSAR-G2K2], [PCP-G2K], [WP-G2K] **[media]**.

- **Menu = ufficio del «duty clerk»** del comando di brigata [PCP-G2K] **[media]**: una **grafica animata con punti
  attivi**, con un impiegato alla scrivania che **scrive davvero** e, se cliccato, fa un breve cenno del capo; lo
  schermo di un computer nella stanza retrostante ha effetti non documentati [COMP-G2K], [CSAR-G2K1] **[media]**. Per
  *Compute!* la schermata «just barely looks like a selection screen»: la scelta è mimetizzata nella scena.
- **Briefing**: si sente il **fruscio delle pagine** sfogliate [COMP-G2K] **[media]**: la carta è suggerita anche col
  suono.
- **Debriefing**: un commentatore; le **immagini fisse** sono «well drawn but nothing really spectacular»
  [CSAR-G2K1] **[media]**.
- **VGA**: «a beautiful game in flight and out of it» [COMP-G2K] **[media]**; per CGW (1991) grafica mediocre e terreno
  senza texture [WP-G2K] **[media]**.
- **Cabine** disegnate **da fotografie** di elicotteri veri, omettendo gli interruttori inutili per non affollare lo
  schermo [WP-G2K] **[media]**.
- **Palette per il tempo atmosferico**: sull'Amiga la visibilità cambia **usando palette diverse** [CSAR-G2K1]
  **[media]**.
- Artisti: Michael Reis, Max D. Remington III, Michael Gibson [WP-G2K] **[media]**. Cornici, font, pulsanti:
  **[lacuna]**.

### 1.7 *Fleet Defender* (MicroProse 1994)

Fonti: manuale [FD-MAN] **[alta]**; [COMP-FD1], [COMP-FD2], [DD-FD], [WP-FD] **[media]**.

- **Menu a pulsanti con la lettera evidenziata**: SCRAMBLE [S], CAMPAIGN [C], TOP TEN [T], QUIT [Q]; il mouse può
  **premere** (*depress*) qualunque pulsante; i pulsanti di generazione delle missioni sono **«blue lower case
  buttons»** (pulsanti blu in minuscolo) [FD-MAN] **[alta]**. La lettera-scorciatoia è parte della grafica.
- **Selezione**: teatro e scenario scelti con le frecce sono **evidenziati in giallo** [FD-MAN] **[alta]**.
- **Albo**: un **«Squadron Roster board»** (tabellone dei dieci squadroni); i nomi si scrivono su una **«name plate»**
  di tre righe (nome, nominativo, cognome) [FD-MAN] **[alta]**.
- **Decorazioni**: una scatola delle medaglie; una **fenice** viene aggiunta alle medaglie per ogni resurrezione; lo
  schermo dei caduti si chiama **«Roll of the Honored Dead»** [FD-MAN] **[alta]**.
- **Debriefing**: replay della missione con gli eventi principali in ordine cronologico, punteggio e decorazioni
  [FD-MAN] **[alta]**; dalla versione 1.2 tenendo premuto il pulsante la lista scorre da sola [DD-FD] **[media]**.
- **Grafica**: solo VGA [DD-FD]; una didascalia di DOS Days dice «VGA 640 x 480 in 256-colours» **[media, da
  verificare]**; primo simulatore MicroProse con **texture** su aerei, terreno e acqua [DD-FD] **[media]**; i bitmap
  degli aerei si possono modificare (per esempio con lo stemma dello squadrone) [COMP-FD1] **[media]**.
- **Sobrietà**: le schermate di preparazione sono «streamlined» e il gioco parte senza animazioni d'apertura
  [COMP-FD1] **[media]**; ma DOS Days elenca un'animazione d'apertura (OPEN.FLC) [DD-FD]: **contraddizione** fra le
  fonti, forse saltabile.
- **Crediti**: Lead Artist Terrence Hodge; Artists Mike Reis, Murray Taylor; 3D Artist Max Remington [FD-MAN]
  **[alta, OCR]**.

### 1.8 *B-17 Flying Fortress* (Vektor Grafix/MicroProse 1992)

Fonti: [DD-B17], [THC-B17], [STF-B17], [WP-B17] **[media]**.

- **Modo**: VGA/MCGA 320×200 a 256 colori [DD-B17] **[media]**.
- **Illustrazione, non fotografia**: una retrospettiva loda le «lovely incidental illustrations and 2D cutaways» di
  Dean Betton e Mark Griffiths (gli artisti accreditati) [THC-B17], [WP-B17] **[media]**; le **sezioni** del
  bombardiere mostrano le postazioni dell'equipaggio, ma non cambiano mai dopo i danni (nessun posto vuoto)
  [THC-B17] **[media]**.
- **Foto di gruppo color seppia** dell'equipaggio, che non perde i caduti [THC-B17] **[media]**.
- **Personalizzazione**: nome e **nose art** del bombardiere [DD-B17], [WP-B17] **[media]**; schermata «Select Your
  Bomber» raggiunta dal Roster, con un pulsante *Load Save* [DD-B17] **[media]**; **filmati** nel briefing [WP-B17]
  **[media]**.
- Su Atari ST, «a mixture of static screens and filled 3D vectors» [STF-B17] **[media]**. Un lettore segnala una
  medaglia disegnata con il nastrino sbagliato [THC-B17] (commento) **[bassa]**.
- Cornici, font, pulsanti: **[lacuna]**.

---

## 2. Gli altri

### 2.1 *Silent Hunter* (Aeon/SSI 1996)

Fonti: [GR-SH], [TLM-SH], [GF-SH] **[media]**; [VGG-SH] (estratto).

- **SVGA** e un solo ambiente: la **centrale** con il periscopio; pulsanti e interruttori «finely detailed in SVGA» e
  facili da usare; nessun membro dell'equipaggio visibile [GR-SH] **[media]**. Per un repertorio la grafica è in 2D,
  già datata all'uscita (estratto) **[bassa]**.
- **Stati**: siluri tolti dal caricamento automatico che restano **in grigio**; ago del carburante con il **segno
  rosso**; un **quadratino rosso lampeggiante** nel periscopio quando un bersaglio fotografico è nel reticolo;
  **riquadro d'informazioni** che compare passando il mouse su un convoglio [TLM-SH] **[media]**.
- **Transizioni**: alla perdita del battello lo schermo trema, il periscopio cade e **sfuma nel blu**, poi un filmato
  [GR-SH] **[media]**; i filmati sono file Smacker (.smk) [TLM-SH] **[media]**.
- **Scelta della pattuglia**: una carta della regione [GF-SH] **[media]**. Carriera, fascicolo, cornici: **[lacuna]**.

### 2.2 *Wolfpack* (NovaLogic/Brøderbund 1990)

Fonti: [WP-WP], [STF-WP] **[media]**.

- CGW giudicò falsa la promessa di grafica **VGA oltre la schermata d'apertura** [WP-WP] **[media]**: menu e stazioni
  in pratica a pochi colori.
- Su Atari ST: una **sola schermata** affollata, l'interno di un sottomarino con carta, velocità e profondità; ogni
  battello sulla carta è **un pixel**; una **piccola icona** distingue il mezzo comandato [STF-WP] **[media]**.
- Nel 1994 CGW preferiva *Silent Service II* per la grafica [WP-WP] **[media]**. L'editor permette di scrivere il
  testo dei briefing [STF-WP] **[media]**. Cornici, font: **[lacuna]**.

### 2.3 *Falcon 3.0* (Sphere/Spectrum HoloByte 1991)

Fonti: manuale, 3ª edizione (1995) [F3-MAN] **[alta]**.

- **War Room** come menu: tre zone (carta del mondo, area dello squadrone, **televisori**); una **spia rossa accesa**
  accanto a una zona dice che si può entrare; spenta, manca un passo; i pulsanti FORM, SQUAD, DEACTIVATE, THEATER,
  COMMIT sono **«lit»** (accesi) solo quando servono **[alta]**. È lo **stato del pulsante reso come luce** di un
  pannello.
- **Monitor come pulsanti**: lo schermo grande in alto a sinistra esce in DOS, quello in alto a destra porta
  all'*Instant Action*; in basso, da sinistra, CONFIG, ACMI, COMMS, RED FLAG; lo stemma dello squadrone si sceglie con
  **frecce verdi** dentro il monitor SQUAD; un **telefono** porta alla Ready Room **[alta]**.
- **Ready Room**: una **lavagna bianca** in alto a sinistra («Sierra Hotel», i migliori piloti), lo schedario con un
  **telefono sopra** per uscire, l'elenco dei piloti su un **monitor video**, il pulsante *Change Face* per i volti
  **[alta]**.
- **Debriefing**: rotta disegnata sulla carta e un **display video a destra** che racconta gli eventi; poi
  un'**animazione della cerimonia** se qualcuno è decorato **[alta]**.
- **Crediti**: Art di Matt Carlström, Dan Guerra, Chuck Butler, Louis Sremac, Zachary Fuller; regia delle animazioni
  Carlström e Fuller **[alta]**. Font e cornici: **[lacuna]**.

### 2.4 *Harpoon* (Three-Sixty Pacific 1989) e *Harpoon II* (1994)

Fonti: manuale *Harpoon Challenger Pak* [HP-MAN] **[alta]**; [ITM-HP2], [WP-HP2] **[media]**; CGW (estratto).

- **Modi**: EGA 640×350 a 16 colori, EGA 320×200, CGA e Tandy [HP-MAN] **[alta]**.
- **Interfaccia da scrivania, non da stanza**: **menu a tendina** (PC, Game, Orders, Settings, Reports, Misc) da aprire
  e trascinare col mouse; voci non disponibili **«dimmed»** in EGA 640×350 e marcate in altro modo negli altri modi;
  **finestre di dialogo** con **radio button** (cerchietto pieno), **caselle** con la X, campi di testo e pulsanti;
  **scroll box** con barra di selezione, e quella attiva ha una **cornice gialla** («Selection Border») (pp. 47–49)
  **[alta]**.
- **Colore di schieramento**: BLUE, RED e NEUTRAL (in **giallo**); un **rombo colorato** d'incertezza attorno al
  contatto; il non identificato chiuso in un **quadrato rosso** [HP-MAN] **[alta]**.
- *Harpoon II*: un **sistema di finestre** proprio, con più carte ridimensionabili «come in Windows»; carta tattica a
  **linee e testo blu** con **simboli NTDS rossi**; il colore quasi solo nella **barra degli strumenti e nelle icone**
  [ITM-HP2] **[media; testo giapponese mal codificato]**. CGW approvò l'interfaccia simile a Windows (estratto)
  **[bassa]**; la *Deluxe Multimedia Edition* (1995) aggiunse animazioni che a CGW parvero poca cosa [WP-HP2]
  **[media]**.

### 2.5 *Tornado* (Digital Integration/Spectrum HoloByte 1993)

Fonti: [COMP-TOR], [CSAR-TOR] **[media]**; [WP-TOR] (da `15`).

- **Pianificatore in Super VGA**, giudicato «striking»; vista 3D delle zone con una telecamera telecomandata; display
  di cabina **bitmap**; esterni a poligoni pieni [COMP-TOR] **[media]**.
- Versione Amiga AGA: menu con **«256-colour digitised backdrops»** (sfondi digitalizzati), musica nei menu; nebbia e
  nuvole che passano dal **grigio sporco al bianco** [CSAR-TOR] **[media]**. Carte stampate in A3 coerenti con la carta
  mobile; manuale con disegni al tratto [CSAR-TOR] **[media]**.
- Font, cornici, pulsanti del pianificatore: **[lacuna]**.

---

## 3. Sezione tecnica: come si facevano le interfacce a 256 colori

### 3.1 La tela d'epoca

- La modalità 13h dà **320×200 a 256 colori**, un byte per pixel, **pixel non quadrati** su un monitor 4:3; ogni voce
  di palette ha **18 bit** (6 per canale, quindi 262 144 colori possibili) e si cambia dai registri del DAC alle porte
  3C8h/3C9h, non dalla memoria video [WP-13H] **[media]**.
- I simulatori del 1991–92 lavorano lì (*B-17*, *Silent Service II*, *F-117A*) [DD-B17], [SS2-MAN]; la Super VGA arriva
  prima nelle schermate di contorno (*Tornado* 1993, *Silent Hunter* 1996) che in volo; MicroProse la provò e la
  rinviò con *F-15 III* [F15III-MAN] **[alta]**.
- PROPOSTA: la nostra tela 640×480 ha pixel quadrati; gli asset d'epoca raddoppiati da 320×200 vanno **corretti
  d'aspetto** o accettati come stilizzazione, ma va deciso una volta per tutte.

### 3.2 La palette come risorsa e le dissolvenze

- La palette si carica scrivendo l'indice 0 sulla porta 3C8h e poi i 768 byte su 3C9h; una **dissolvenza** si fa
  sottraendo a ogni ciclo una unità dai valori non nulli e ricaricando la palette **durante il ritorno verticale**
  (60–70 volte al secondo), mandando **128 colori per ritorno** o anche solo 64; per dissolvere verso un colore
  diverso dal nero basta confrontare con un valore d'arrivo [PCGPE-VGA] (Boone, 1994) **[alta come tutorial d'epoca]**.
- Il **cambio di palette** serve anche come effetto di scena: visibilità e meteo resi con palette diverse in *Gunship
  2000* [CSAR-G2K1]; nebbia che vira dal grigio al bianco in *Tornado* [CSAR-TOR] **[media]**.
- Il **«Deluxe Paint» stencil** protegge un elenco di indici di palette dal pennello [WP-DP] **[media]**: lavorare per
  **intervalli d'indici** era la norma, e un'interfaccia che deve convivere con scene diverse si tiene un **blocco di
  voci riservato** (PROPOSTA per noi: le prime 32 voci fisse per cornici, testo e stati, il resto alla scena)
  **[bassa: pratica comune, non documentata per questi titoli]**.

### 3.3 Il color cycling e Mark Ferrari

- Il *color cycling* ruota i colori di un **blocco di voci contigue** della palette per animare acqua e luci senza
  ridisegnare pixel; *Deluxe Paint* lo gestiva per intervalli [WP-DP], [WP-CC] **[media]**.
- **Mark Ferrari** (fondali di *Loom* e di parte di *Monkey Island*) animava pioggia, neve, onde, nebbia, cascate con
  un'**unica immagine e una palette da 256 colori**, senza livelli; i suoi file sono ILBM di *Deluxe Paint* con i dati
  di ciclo [EG-CC] **[media]**. La sua analogia: le **luci di una insegna da teatro**, che non si muovono ma si accendono
  in sequenza [WP-CC] **[media]**. Il suo intervento al GDC 2016 tratta color cycling e *palette shifting* per
  animazioni di fondo (estratto) [GDC-MF] **[bassa]**.
- Nei titoli di questo gruppo il color cycling **non è documentato** **[lacuna]**. PROPOSTA: usarlo dove è naturale
  per noi: la **spazzata** di uno schermo, le **spie** che lampeggiano, l'**acqua** del pontile nella base, la
  telescrivente (cursore), con un intervallo di voci per effetto.

### 3.4 Le rampe di colore (anche per i metalli)

- Una rampa ben fatta (Raymond Schlitter, *Pixelblog 1*): **9 gradini**, luminosità crescente in modo regolare che di
  solito non parte dal nero, saturazione **massima al centro** e mai a 0 o 100, **spostamento di tinta positivo** fino a
  circa **20°** per gradino (i chiari si scaldano); i **grigi** si ottengono duplicando e desaturando il centro della
  palette, per farli convivere con i colori vivi [SLY-1] **[media]**. Luce di norma dall'alto, primaria a destra o a
  sinistra [SLY-6] **[media]**.
- **Metalli**: nessuna fonte trovata che descriva le rampe dei metalli di questi giochi **[lacuna]**. Deduzione
  **[bassa]** e PROPOSTA: grigio-blu freddo nelle ombre, quasi bianco caldo nei riflessi, con un **salto di contrasto**
  netto fra due gradini vicini (è ciò che distingue il metallo dalla pietra); per l'ottone della Marina USA una rampa
  bruno-oliva che sale verso il giallo; per la VMF un grigio-verde più spento.

### 3.5 Dithering ordinato contro dithering a errore

- **Diffusione dell'errore** (Floyd-Steinberg e varianti): ottima per le immagini fisse, ma muovendo un solo pixel
  genera un «cono» di artefatti tremolanti; **ordinato** (Bayer e simili): il colore di un pixel dipende **solo da quel
  pixel**, quindi è adatto alle animazioni; con palette irregolari il Bayer classico rende male (Yliluoma, 2011)
  [JY-DITH] **[media]**. Già nel 1999 il dithering deterministico era raccomandato per animazioni compresse a
  differenze (estratto) [GIMP-DITH] **[bassa]**.
- **Nei giochi del gruppo**: *Silent Service II* ricorre alla **puntinatura** solo in CGA [SS2-MAN] **[alta]**; *Red
  Storm Rising* usa **punti più radi** come codice per l'acqua che assorbe [RSR-MAN] **[alta]**; *Deluxe Paint II*
  offre riempimenti a motivo e a gradiente [WP-DP] **[media]**.
- PROPOSTA: per cornici, pulsanti e sfondi delle schermate usare il **dithering ordinato a 2×2 o 4×4** solo per
  passaggi di tono larghi (cielo, legno lontano, ombra di un pannello); niente diffusione dell'errore su ciò che si
  anima o scorre.

### 3.6 Gli smussi e la luce dall'alto a sinistra

- Le linee guida Apple per le icone: la luce viene **sempre dall'angolo in alto a sinistra**, le ombre portate cadono
  **in basso a destra**, e va usata in modo coerente in tutta l'interfaccia [MAC-HIG] **[alta per Apple]**. La
  convenzione dell'interfaccia di sistema «alle 10» è ricordata anche da altri (estratto) [JW-LIGHT] **[bassa]**.
- Convenzione dei pulsanti in pixel art (fonte di qualità incerta, estratto) **[bassa]**: in rilievo = **pixel chiari
  sopra e a sinistra, scuri sotto e a destra**, di un pixel; premuto = **smusso invertito** e scritta **spostata di un
  pixel in basso**; disattivato = smusso tolto e colori desaturati.
- **Nei giochi del gruppo** lo stato del pulsante è reso più spesso come **luce o colore** che come rilievo: spie
  rosse e pulsanti «lit» (*Falcon 3.0*), selezione **gialla** (*Fleet Defender*, *Harpoon*), voci **dimmed** o
  **in grigio** (*Harpoon*, *Silent Hunter*), etichetta del cursore (*F-15 III*) [F3-MAN], [FD-MAN], [HP-MAN],
  [TLM-SH], [F15III-MAN] **[alta/media]**. *Fleet Defender* parla di pulsanti che si «depress» col mouse [FD-MAN]
  **[alta]**, quindi uno stato premuto esisteva, ma non è descritto.
- PROPOSTA: smusso di **1 pixel** per i pulsanti piccoli (etichette 8×8), di **2 pixel** per i pannelli e le cornici
  (il secondo pixel più tenue), luce fissa in alto a sinistra in tutte le schermate; e in più la **spia** (colore) per
  gli stati logici, come *Falcon 3.0*.

### 3.7 Font bitmap e ombra di un pixel

- Nessuna fonte sui **font** di questi giochi (nome, misura, maiuscolo) **[lacuna]**; il manuale di *Fleet Defender*
  mostra solo che le **lettere-scorciatoia** sono evidenziate nei pulsanti [FD-MAN] **[alta]**.
- Leggibilità del testo su sfondi pieni di dettagli: Raymond Chen (2004), provando effetti per le etichette delle icone
  di Windows, trovò leggibili soprattutto lo **sfondo pieno** dietro il testo e un **contorno di 2 pixel**, e del tutto
  inutile lo XOR [ONT-TXT] **[media]**. I font bitmap nascono **senza antialias**, quindi l'ombra va resa come pixel
  netto (estratto) **[bassa]**.
- PROPOSTA: testo sulle schermate di contorno su **fascia piena** (carta, targhetta) quando è lungo; **ombra di un
  pixel in basso a destra** solo per titoli e intestazioni su sfondi illustrati; maiuscolo per targhette e timbri,
  maiuscolo e minuscolo per i testi da leggere.

### 3.8 Foto digitalizzate, modelli e pre-render

- Tre strade documentate: **foto digitalizzate di modelli in scala** (*Silent Service II*) [TXT-SS2]; **modello CAD**
  di una base aerea come set delle schermate e **stile foto-realistico** coerente (*F-15 III*) [F15III-MAN]
  **[alta]**; **cabine da foto** (*Gunship 2000*) [WP-G2K] e **sfondi digitalizzati** (*Tornado* AGA) [CSAR-TOR]
  **[media]**. Il rischio documentato: con pochi colori le foto diventano macchie grigie (*SS II* su Amiga)
  [AR-SS2] **[media]**.
- **Strumenti**: *Deluxe Paint II Enhanced* (1989) per DOS arrivava a 800×600 a 256 colori ed era diffusissimo per la
  grafica dei giochi; *Deluxe Paint Animation* lavorava a 320×200×256 [WP-DP] **[media]**. Quali strumenti usasse
  MicroProse non risulta: l'intervista a Michael Haire, direttore artistico, non ne parla, ma riporta la regola di Sid
  Meier che la grafica doveva **servire il gioco** [SH-MH] **[media]** **[lacuna sugli strumenti]**.

---

## 4. Che cosa distingue MicroProse da Dynamix ed EA

Avvertenza: il lato MicroProse è documentato qui; il lato Dynamix/EA lo coprono gli altri due ricercatori, quindi i
confronti si basano su `15` e su deduzioni **[bassa]** finché le loro note non lo confermano.

1. **Luoghi veri al posto dei menu**: Ready Room con porte e bacheca (*F-117A*), hangar con armadietti, lavagna,
   televisore e cestino (*F-15 III*), ufficio con impiegato (*Gunship 2000*) **[alta/media]**. Dynamix punta di più
   sul **personaggio** che parla (il comandante Cord in *A-10*) [`15` §1.5] **[bassa come confronto]**.
2. **Foto-realismo dichiarato**: per *F-15 III* uno «photo-realistic style» coerente e un set CAD della base
   **[alta]**; foto di modelli in *SS II* e foto di cabine in *Gunship 2000* **[media]**. Ipotesi **[bassa]**: le
   schermate Dynamix sono più **pittoriche**, quelle EA più **grafiche**.
3. **Il colore è un codice, e il manuale lo spiega**: appendici dei colori in *F-117A*, tabella per modo grafico in
   *SS II*, contatti che cambiano colore in *RSR* **[alta]**. La bellezza viene dopo la leggibilità.
4. **Si scende di modo con eleganza**: la stessa schermata è pensata per VGA, EGA, Tandy e CGA, con il dithering come
   ripiego dichiarato **[alta]**.
5. **Il cursore fa da etichetta**: in *F-15 III* il cursore «legge» la funzione e cambia forma in cabina (gradi,
   croce, rombo, X) **[alta]**: la scena resta pulita, senza pulsanti disegnati sopra.
6. **Pulsanti testuali con lettera-scorciatoia e selezione gialla** dove la scena non serve (*Fleet Defender*)
   **[alta]**: tastiera e mouse sono alla pari.
7. **Sobrietà**: la grafica deve «servire il gioco» (Meier, riferito da Haire) [SH-MH]; schermate di preparazione
   «streamlined» (*Fleet Defender*) [COMP-FD1]; animazioni brevi e disattivabili (*SS II*) **[media]**.
8. **Una squadra d'arte ricorrente**: Bents, Biscoe, Remington, Taylor tornano da *RSR* a *Fleet Defender*
   **[alta, crediti]**: da qui la coerenza di casa; Remington è «3-D artist for virtually all» dei giochi MicroProse
   [F117-MAN] **[alta]**.
9. **Il dopo-missione è un replay**, non un quadro: replay animato e cronologico in *F-15 III* e *Fleet Defender*
   **[alta]**, contro la pagina di giornale di *F-19* (`15` §1.1) e, da verificare, i quadri e i ritratti di Dynamix
   **[bassa]**.
10. **Ironia di casa**: i progettisti nell'albo di *F-117A*, la fenice delle resurrezioni in *Fleet Defender*
    **[alta]**: tocchi leggeri dentro una cornice militare seria.
11. **Gli altri del gruppo** si collocano ai lati: *Harpoon* è una **scrivania da Macintosh** (menu a tendina,
    dialoghi, finestre) **[alta]**; *Falcon 3.0* una **sala operativa** di monitor con spie rosse **[alta]**;
    *Silent Hunter* e *Tornado* portano la **SVGA** e gli sfondi digitalizzati, *Silent Hunter* i filmati
    **[media]**.

**Ricadute per il nostro remake** (PROPOSTA, in breve):
- **Sala briefing e albo come luoghi** (bacheca, armadietti o schedario) **e** pulsanti testuali con lettera evidenziata
  nel piede della schermata (§7 di `15`), così valgono sia il mouse che la tastiera.
- **Etichetta del cursore** sui punti attivi delle scene illustrate, come *F-15 III*, al posto dei pulsanti disegnati.
- **Appendice dei colori nel manuale** (vol. 2) e **codici fissi** fra carta, debriefing e stazioni.
- **Smusso a 1–2 pixel, luce in alto a sinistra**, stati logici come **spie**; palette con un blocco riservato
  all'interfaccia; dithering **ordinato** soltanto.
- **Replay** a eventi nel debriefing, come già previsto dal requisito 16.

---

## 5. Fonti

Consultate il 27–28 ottobre 2026. Le sigle già usate in `15` §9 restano uguali.

### Manuali e fonti primarie
- `[F117-MAN]` MicroProse, *F-117A Nighthawk Stealth Fighter 2.0*, manuale (1991), testo OCR: https://archive.org/stream/f117Astealthfightermanual/F-117AStealthFighter-Manual_djvu.txt (pp. 27–35, 176, 178–180, crediti)
- `[F15III-MAN]` MicroProse, *F-15 Strike Eagle III*, manuale (1992), testo OCR: https://archive.org/stream/microprose-f-15-strike-eagle-iii-manual/Microprose_F15_Strike_Eagle_III_Manual_EN_djvu.txt (Home Screen, Roster, Briefing, Arming, Debriefing, note del produttore pp. 199–200, crediti)
- `[RSR-MAN]` MicroProse, *Red Storm Rising*, manuale, testo OCR: https://archive.org/stream/redstormrisingmanual/RedStormRising-Manual_djvu.txt (pp. 8–33, crediti)
- `[SS2-MAN]` MicroProse, *Silent Service II*, manuale tedesco (DOS), testo OCR: https://archive.org/stream/silent-service-ii-dos-german-manual/Silent-Service-II_dos_german-Manual_djvu.txt (pp. 63–76, appendice tecnica)
- `[FD-MAN]` MicroProse, *Fleet Defender*, manuale (1994), PDF di DOS Days: https://www.dosdays.co.uk/media/games/f14/Fleet_Defender_Manual.pdf (crediti; pp. 21–46)
- `[F3-MAN]` Spectrum HoloByte, *Falcon 3.0*, manuale, 3ª ed. (dic. 1995), DOS Days: https://dosdays.co.uk/media/games/falcon3/Falcon%203.0.PDF (crediti; sezioni IV, V, VIII)
- `[HP-MAN]` Three-Sixty Pacific, *Harpoon Challenger Pak*, manuale (1991), testo OCR: https://archive.org/stream/Harpoon_Challenger_Pak_Manual_1991/Harpoon%20Challenger%20Pak%20Manual%20%281991%29_djvu.txt (requisiti; pp. 47–49)

### Recensioni e schede
- `[ZZAP-RSR]` *Zzap!64* (1988) su *Red Storm Rising*, citato solo nel riassunto di una ricerca (estratto; pagina d'origine non individuata, probabilmente https://everygamegoing.com/larticle/Red-Storm-Rising-000/28923)
- `[TXT-SS2]` Recensione di *Silent Service II* (IBM PC), archivio textfiles: https://mirror.cyberbits.eu/textfiles.com/games/REVIEWS/silserv.rev
- `[STN-SS2]` *ST News* vol. 7 n. 1, «Software review: Silent Service II»: https://www.st-news.com/issues/st-news-volume-7-issue-1/reviews/silent-service-ii
- `[AR-SS2]` J. Compton, *Amiga Report*, *Silent Service II*: https://bigbookofamigahardware.com/amigareport/www.amigareport.com/ar507/review7.html
- `[COMP-SS2]` D. Greenberg, *Compute!* n. 146 (nov. 1992), p. A26: https://atarimagazines.com/compute/issue146/A26_Silent_Service_II.php
- `[ACE-SS2]` *ACE* n. 55 (1992), via Every Game Going: https://everygamegoing.com/larticle/silent-service-2/54731
- `[WP-SS2]` https://en.wikipedia.org/wiki/Silent_Service_II
- `[WP-TF42]` https://en.wikipedia.org/wiki/Task_Force_1942
- `[DD-F117]` DOS Days: https://www.dosdays.co.uk/topics/Games/game_f117.php ; `[WP-F117]` https://en.wikipedia.org/wiki/F-117A_Nighthawk_Stealth_Fighter_2.0
- `[WP-F15III]` https://en.wikipedia.org/wiki/F-15_Strike_Eagle_III
- `[COMP-G2K]` P. Olafson, *Compute!* n. 138 (feb.-mar. 1992), p. 100: https://www.atarimagazines.com/compute/issue138/100_Gunship_2000.php
- `[CSAR-G2K1]` P. Dale, comp.sys.amiga.reviews (14/7/1993): https://www.math.uh.edu/~barrett/data/all/Gunship2000
- `[CSAR-G2K2]` S. Koren, comp.sys.amiga.reviews (29/7/1993): https://www.math.uh.edu/~barrett/data/all/Gunship2000_2
- `[PCP-G2K]` S. Smith, *PC Pilot*, flightsimbooks.com: https://www.flightsimbooks.com/pcpilot/page114.php
- `[WP-G2K]` https://en.wikipedia.org/wiki/Gunship_2000
- `[COMP-FD1]` S. A. May, *Compute!* n. 166 (lug. 1994), p. 86: https://www.atarimagazines.com/compute/issue166/86_Fleet_Defender_The_.php
- `[COMP-FD2]` D. Atkin, «Tomcat unleashed», *Compute!* n. 165 (giu. 1994), p. 90: https://www.atarimagazines.com/compute/issue165/90_Tomcat_unleashed.php
- `[DD-FD]` DOS Days, *Fleet Defender*: https://dosdays.co.uk/topics/Games/game_fleet.php ; `[WP-FD]` https://en.wikipedia.org/wiki/Fleet_Defender
- `[DD-B17]` DOS Days: https://dosdays.co.uk/topics/Games/game_b17.php ; `[WP-B17]` https://en.wikipedia.org/wiki/B-17_Flying_Fortress_(video_game)
- `[THC-B17]` Tally-Ho Corner, «Dusty but trusty: B-17 Flying Fortress» (2021): https://tallyhocorner.com/2021/11/dusty-but-trusty-b-17-flying-fortress/
- `[STF-B17]` R. Mead, *ST Format* n. 48 (1993), via Every Game Going: https://everygamegoing.com/larticle/b17-flying-fortress-000/49355
- `[GR-SH]` *Game Revolution*, «Dive, Dive, Sink!» (*Silent Hunter*): https://www.gamerevolution.com/?p=33106
- `[TLM-SH]` T. Martin, FAQ di *Silent Hunter* (1998–2001): https://www.cs.cmu.edu/~tlm/silent.html
- `[GF-SH]` B. Hegarty, *GamesFirst* (1999): https://gamesfirst.com/articles/1999/silent-hunter-review/
- `[VGG-SH]` VideoGameGeek, *Silent Hunter* (estratto): https://videogamegeek.com/videogame/75419/silent-hunter
- `[WP-WP]` https://en.wikipedia.org/wiki/Wolfpack_(video_game)
- `[STF-WP]` N. Jackson, *ST Format* n. 30 (1992), via Every Game Going: https://everygamegoing.com/larticle/wolfpack-000/49486
- `[ITM-HP2]` ITmedia PC USER, rubrica sui giochi retrò, *Harpoon II* (2005), p. 2: https://www.itmedia.co.jp/pcupdate/articles/0511/11/news099_2.html
- `[WP-HP2]` https://en.wikipedia.org/wiki/Harpoon_II
- `[COMP-TOR]` S. A. May, *Compute!* n. 162 (mar. 1994), p. 108: https://www.atarimagazines.com/compute/issue162/108_Tornado.php
- `[CSAR-TOR]` C. J. Coulson, *Tornado* AGA, comp.sys.amiga.reviews (18/10/1994): https://ftp.fau.de/mirrors/aminet/docs/rview/TornadoAGA.txt
- `[WP-TOR]` https://en.wikipedia.org/wiki/Tornado_(1993_video_game)
- `[SH-MH]` *Spillhistorie*, «Q&As with: Michael Haire – Former Art Director at Microprose»: https://spillhistorie.no/qas-with-michael-haire-former-art-director-at-microprose/

### Tecnica
- `[WP-13H]` Wikipedia, *Mode 13h*: https://en.wikipedia.org/wiki/Mode_13h
- `[PCGPE-VGA]` Boone, «Programming the VGA Registers» (marzo 1994, PC-GPE): https://bearstrong.net/tekst97/data/programmering/pcgpe10/vgaregs/
- `[WP-DP]` Wikipedia, *Deluxe Paint*: https://en.wikipedia.org/wiki/Deluxe_Paint
- `[WP-CC]` Wikipedia, *Color cycling*: https://en.wikipedia.org/wiki/Color_cycling
- `[EG-CC]` Effect Games, «Old School Color Cycling with HTML5» (2011): http://www.effectgames.com/effect/article-Old_School_Color_Cycling_with_HTML5.html
- `[GDC-MF]` GDC, presentazione del talk di M. Ferrari «8 Bit & '8 Bitish' Graphics-Outside the Box» (2016) (estratto): https://gdconf.com/news/thimbleweed_park_artist_will_s
- `[SLY-1]` R. Schlitter, «Pixelblog 1 – Color Palettes» (2018): https://www.slynyrd.com/blog/2018/1/10/pixelblog-1-color-palettes
- `[SLY-6]` R. Schlitter, «Pixelblog 6 – Light and Shadow» (2018): https://www.slynyrd.com/blog/2018/6/15/pixelblog-6-light-and-shadow
- `[JY-DITH]` J. Yliluoma, «Arbitrary-palette positional dithering algorithm» (2011): https://bisqwit.iki.fi/story/howto/dither/jy/
- `[GIMP-DITH]` commit GIMP sul dithering deterministico (1999) (estratto): https://git.tsundoku.ne.jp/callisto-debian/gimp/commit/d0127d1809f12a8ef188dc1295c66bf6f52e60df
- `[MAC-HIG]` Apple, *Macintosh Human Interface Guidelines*, «Use a Consistent Light Source»: https://discmaster.textfiles.com/file/31918/Apple%20Developer%20Reference%20Library%20(December%201999)%20(Disk%202).iso/pc/technical%20publications/mac/higuidelines/higuidelines-164.html?html=true
- `[JW-LIGHT]` joshworth.com, sulla sorgente di luce «alle 10» (estratto): https://joshworth.com/jpw/wp-json/wp/v2/posts/368
- `[ONT-TXT]` R. Chen, *The Old New Thing*, «A sample of desktop icon text effects» (25/11/2004): https://devblogs.microsoft.com/oldnewthing/?p=37203
