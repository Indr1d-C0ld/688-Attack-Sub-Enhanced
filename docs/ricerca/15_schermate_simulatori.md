# 15 — Le schermate attorno alla missione nei simulatori d'epoca (dossier di riferimento)

Progetto: remake realistico di *688 Attack Sub* (EA 1989). Scopo: studiare come i simulatori di volo (e navali) di fine
anni '80 e anni '90 organizzavano le schermate **prima e dopo** la missione (albo dei piloti, scelta della missione,
briefing, carico, debriefing, punteggio, promozioni, decorazioni, fine carriera, albo d'onore) e ricavarne proposte
per la **Fase 4** di `docs/00_ROADMAP.md` (sala briefing, Sala siluri, debriefing con le minicartoline, esito,
cerimonia, fascicolo) e per i requisiti 15 e 16. Solo ricerca: nessun codice. Compilato il 24 ottobre 2026.

---

## 0. Convenzioni, rimandi e punto di partenza

- **Confidenza** (come `02` §0): **[alta]** fonte primaria verificata (qui: il manuale del gioco letto nel testo
  integrale, o una guida ufficiale); **[media]** fonte aperta secondaria credibile (enciclopedie, recensioni d'epoca
  riassunte da terzi, siti di retrogiochi), oppure dato coerente fra più fonti; **[bassa]** ricordo non riverificato,
  forum, descrizione del committente, interpolazione. «[alta sul gioco]» = vero per il gioco, non per la realtà.
- **STIMA** = valore proposto per il nostro gioco (taratura); **PROPOSTA** = scelta di progetto da discutere.
  «estratto» = pagina letta solo attraverso il riassunto di un motore di ricerca. Le sigle fra parentesi quadre
  rimandano alla §9. Le fonti sono riassunte con parole mie; le poche citazioni sono brevi e fra virgolette.
- **Nessuna immagine copiata**: le schermate sono descritte a parole dai manuali e dalle recensioni.
- **Non ripetuto qui** (si rimanda): gradi, commissioni, soglie delle decorazioni, motivazioni, ritratto e fascicolo
  (`13`, in particolare §2.3, §3.7, §4.4, §5.4); posti, tubi, ricariche, carichi standard e dati di `carico.toml`
  (`14` §3, §5); ROE, serie radio, orologio radio e `debriefing()` (`docs/11_comandi_e_stazioni.md` R1–R2,
  `ricerca/10`); carta VGA, simboli e zone (N1–N4, `docs/11` §§ carta); pezzi M1–M5 della Fase 4a
  (`docs/12_schermate_di_missione.md`); struttura del file di missione e campo `briefing` (`docs/09_schema_missioni.md` §2).
- **Tela del remake**: 640×480 a 256 colori indicizzati, una palette per scena, asset d'epoca a 320×200 raddoppiati
  (`00_ROADMAP` § Tecnologia). Le misure in pixel delle proposte (§7) sono riferite a questa tela.

### 0.1 Attribuzioni verificate (l'elenco dell'utente contiene qualche svista)

| Titolo | Anno (prima uscita) | Sviluppo | Edizione | Note e conf. |
|---|---|---|---|---|
| *F-15 Strike Eagle* | 1984 (Atari 8 bit) | MicroProse (S. Meier) | MicroProse | [media] [WP-F15] |
| *F-15 Strike Eagle II* | 1989 (DOS) | MPS Labs (Hollis, Meier) | MicroProse | [media] [WP-F15II] |
| *F-15 Strike Eagle III* | dic. 1992 (DOS) | MPS Labs (prod. A. Hollis) | MicroProse | [alta] manuale [F15III-MAN] |
| *F-19 Stealth Fighter* | 1988 (DOS) | MicroProse (S. Meier, A. Hollis) | MicroProse | rifacimento di *Project Stealth Fighter* (C64, 1987) [media] [WP-F19] |
| *F-117A Nighthawk Stealth Fighter 2.0* | 1991 (DOS) | MPS Labs (J. Briggs) | MicroProse | rifacimento di *F-19* [media] [WP-F117] |
| *Falcon 3.0* | 1991 (DOS) | **Sphere Inc.** | **Spectrum HoloByte** (USA), MicroProse (Europa) | non MicroProse [alta] [WP-F3], [F3-MAN] |
| *Falcon 4.0* | dic. 1998 (Windows) | MicroProse Alameda (ex Spectrum HoloByte) | Hasbro Interactive, marchio MicroProse | [media] [WP-F4] |
| *Falcon 4.0: Allied Force* | 2005 | **Lead Pursuit** (sul codice licenziato da Atari) | Graphsim | [media] [WP-F4]; editore da riverificare [bassa] |
| *Lock On: Modern Air Combat* | nov. 2003 | **Eagle Dynamics** | **Ubisoft** (Europa), 1C (Russia) | [media] [WP-LO] |
| *Chuck Yeager's Air Combat* | 1991 (DOS) | Brent Iverson | **Electronic Arts** | [media] [WP-CYAC] |
| *Tornado* | 1993 (DOS, Amiga) | **Digital Integration** | **Spectrum HoloByte** | [media] [WP-TOR] |
| *TFX* | 1993 (DOS) | **Digital Image Design** | **Ocean Software** | [media] [WP-TFX] |
| *X-COM: Interceptor* | giugno 1998 (Windows) | MicroProse Chapel Hill | MicroProse | [media] [WP-XCI] |
| *B-17 Flying Fortress* | 1992 (DOS) | **Vektor Grafix** / MicroProse Leeds (acquisita nel 1992) | MicroProse | [media] [WP-B17] |
| *F-14 Tomcat* | 1988–89 (C64); DOS 1990 | **Dynamix** (K. Ryan, N. Skrepetos) | **Activision** | la versione DOS fu portata da Activision senza Dynamix [media] [C64W-F14], [SH-F14] |
| *A-10 Tank Killer* | 1989 (DOS) | **Dynamix** (D. Slye) | Dynamix | [alta] manuale [A10-MAN] |
| *Eagles* | 1983 (Atari 800), C64 1987 | **Strategic Simulations (SSI)** | SSI | vedi §1.13 [media] [LB-EAG], [MG-EAG] |

Navali: *Silent Service* (MicroProse 1985, S. Meier), *Silent Service II* (MPS Labs/MicroProse 1990), *Red Storm
Rising* (MicroProse 1988, Meier e Hendrick), *Wolfpack* (NovaLogic/Brøderbund 1990), *Aces of the Deep* (Dynamix/
Sierra 1994; *Command: Aces of the Deep* per Windows 1995), *Silent Hunter* (Aeon/SSI 1996), *Harpoon* (Three-Sixty
Pacific 1989, L. Bond), *688(I) Hunter/Killer* (Sonalysts/Electronic Arts 1997) [media] (voci Wikipedia, §9).

In sintesi: della lista dell'utente sono MicroProse (o sue divisioni MPS Labs, Chapel Hill, Leeds, Alameda) *F-15*
I–III, *F-19*, *F-117A*, *B-17*, *X-COM: Interceptor* e *Falcon 4.0*; non lo sono *Falcon 3.0* e *Tornado* (Spectrum
HoloByte, che in Europa usò MicroProse come distributore per *Falcon 3.0*), *Chuck Yeager* (EA), *TFX* (DID/Ocean),
*Lock On* (Eagle Dynamics/Ubisoft), *F-14 Tomcat* (Dynamix/Activision), *A-10* (Dynamix), *Eagles* (SSI).

---

## 1. I simulatori di volo, titolo per titolo

### 1.1 *F-19 Stealth Fighter* (1988)

**Sequenza** [media] [DD-F19], [WP-F19]: introduzione e protezione dalla copia → **albo dei piloti** (più piloti
salvati; un pilota morto non si può più caricare) → scelta del teatro (Libia, Golfo Persico, Capo Nord, Europa
centrale), del livello di conflitto (guerra fredda, limitata, convenzionale), della qualità del nemico e del realismo →
**briefing d'intelligence** → schermata di **carico** (il carico proposto è già adatto agli obiettivi, si può cambiare)
→ volo → **debriefing** con punteggio → eventuale schermata di celebrazione (medaglia, promozione).

**Aspetto**: 320×200 a 16 colori; menu «puliti», comandi a frecce e Invio o joystick [media] [DD-F19]. Le didascalie
documentano schermate di scelta del teatro, briefing, carico, debriefing e celebrazione, senza descriverne la grafica.
L'intestazione «CLASSIFIED» e i dettagli del debriefing sulla carta restano la descrizione del committente
(`00_ROADMAP` requisito 16) [bassa]: non trovata in fonti aperte (come già in `13` §6 punto 7).

**Punteggio e ricompense** [alta sul gioco] [FSB-F19]:
- Valori di base per azione che **cambiano con il tipo di guerra e le ROE**: in guerra fredda pagano soprattutto
  obiettivo primario e secondario, si guadagnano punti per ogni minuto passato dietro le linee, si perdono punti se si
  è avvistati (recuperabili distruggendo il radar che ha avvistato), colpire civili o amici costa moltissimo; in
  guerra convenzionale gli obiettivi militari valgono di più e la scoperta non costa nulla (06_10, 07_2).
- **Moltiplicatore** fino a circa 9: regione, tensione, qualità del nemico, realismo, carburante stimato (distanza),
  tipo di missione; il lancio col paracadute toglie dal 25 al 75 % secondo dove avviene (mare, territorio amico,
  neutrale, nemico).
- **Medaglie** per la sola missione: 250, 500, 850, 1300, 1800 punti (dall'Airman's Medal alla Medal of Honor, con
  ordine diverso fra le versioni); seconda medaglia uguale a più del doppio, terza a più del triplo.
- **Promozioni** con tre minimi insieme: missioni, punteggio totale, media per missione; da 300 punti e media 100
  (1st Lt) a 27 720 punti e media 280 con 99 missioni (Brigadier General) (07_3).

**Esiti**: elicottero di soccorso, giornale con la cattura del pilota, protesta di un alleato o di un neutrale
[media] [WP-F19]: l'esito si racconta con una **pagina di giornale**, non solo con un numero.

**Accoglienza**: *Computer Gaming World* lo nominò simulazione dell'anno; Origins Award anche per la grafica
[media] [WP-F19]. Critiche aperte poche; la prova di riconoscimento degli aerei prima del volo (protezione dalla copia)
era una barriera, ma serviva anche da quiz di cultura (in *F-117A* chi sbaglia vola solo l'addestramento) [media]
[F117-MAN].

### 1.2 *F-117A Nighthawk Stealth Fighter 2.0* (1991)

È il modello dichiarato dal committente per briefing, carico e debriefing. Il manuale si legge integralmente
[alta sul gioco] [F117-MAN]:

- **Preflight**: prova di identificazione di un aereo (sbagliando si va all'addestramento) → **Pilot Roster**: si
  sceglie o crea un pilota (cancellare è definitivo) e si compila una **scheda** con qualità del nemico (Green,
  Regular, Veteran, Elite), abilità di atterraggio (No Crashes, Easy, Realistic) e realismo del velivolo (F-117A
  «Lockheed» a 2 vani, solo notte; «MicroProse» a 4 vani, anche giorno); il manuale avverte che **le scelte più
  difficili alzano il punteggio** (pp. 27–29).
- **Ready Room** come snodo: porte verso le altre stanze, la bacheca riporta all'albo (p. 30). È la **metafora delle
  stanze**: ufficio del comandante, sala briefing, manutenzione, hangar.
- **Ufficio del comandante** (pp. 30–32): nove regioni datate (Golfo Persico 1984 … Corea 1996/97), livello del
  conflitto, tipo di missione (aria-aria, attacco, addestramento senza punteggio).
- **Sala briefing** (pp. 33–34): carta con **T** (decollo), **P** (primario), **S** (secondario), **L** (atterraggio)
  e informazioni al passaggio del cursore; menu con *Mission Brief*, *Radar Sites*, portate radar o portate dei
  missili (alternate con la barra), **Select All / Clear All**, truppe nemiche, **Decline Mission**, **Select
  Weapons**. È esattamente il menu dei livelli accendibili ripreso in `00_ROADMAP` Fase 4.
- **Pianificazione**: si spostano i punti di rotta per «infilare l'ago» fra i radar; il percorso predefinito spesso
  attraversa le zone di scoperta [media] [WP-F117].
- **Manutenzione** (p. 34): si sceglie un vano e poi l'arma; si vedono **carburante stimato e carburante a bordo**;
  vano 1 per il primario, vano 2 per il secondario; in appendice una tabella di **efficacia delle armi** da 1 a 7
  per tipo di bersaglio (pp. 137–141) e una tabella armi con portata, velocità, guida, quota e tecnica d'attacco.
- **Hangar** (p. 35): conferma «pronto?» e decollo, oppure ritorno alla Ready Room.
- **Fine missione** (p. 55): atterraggio e spegnimento, missioni «a colpo unico»; paracadute meglio sul mare, peggio
  su territorio nemico (cattura e processo pubblico).
- **Debriefing** (p. 55): il comandante **ripercorre la missione passo per passo**, valuta ogni evento quando avvenne
  e gli dà un punteggio numerico. Il punteggio dipende dalla riuscita **e dal rispetto delle ROE**, con regole diverse
  per guerra fredda (la scoperta è gravissima: avvistamento visivo peggio del tracciamento radar; amici peggio dei
  neutrali, neutrali peggio dei militari nemici), guerra limitata e convenzionale (pp. 55–56).
- **Decorazioni** (p. 56): AM, DFC, Silver Star, Air Force Cross, Medal of Honor (solo contro l'opposizione più dura);
  **Purple Heart** se si rientra feriti; Combat Readiness Medal quasi a tutti; **nastrini per numero di missioni**
  (5, 15, 30, 60).
- **Gradi e fine carriera** (p. 57): da 2nd Lt a Col, promozione secondo totale **e** media più esperienza; Brigadier
  General solo al congedo. Il manuale: «No pilot is expected to fly more than 99 missions.» Dopo 99: stella di
  generale, scrivania a Washington o invito a lasciare il servizio.

**Accoglienza**: CGW lo trovò coinvolgente e istruttivo, 93 % su *C&VG* [media] [WP-F117]; VGA a 256 colori, «culmine»
dei simulatori MicroProse per una retrospettiva [media] [DD-F117]. La missione nasce da una tabella di 40–50 modelli
d'obiettivo, validata dal codice [media] [GH-F117]: **briefing generato dai dati**, come vuole la nostra roadmap.

### 1.3 *F-15 Strike Eagle* I, II e III

- **I** (1984): teatro e partenza già in volo, armi fisse, niente carriera [media] [WP-F15]. **II** (1989): armi fisse,
  primario e secondario, **promozioni e medaglie secondo il punteggio**; per CGW più gioco che simulazione, molto
  simile a *F-19* [media] [WP-F15II].
- **III** (1992) [alta sul gioco] [F15III-MAN]:
  - **Schermata principale = hangar** con punti attivi (ROSTER, THEATERS, REALITY, BRIEFING, QUICK START, MODEM).
    *Quick Start*: si parte in volo in addestramento, **senza punteggio**.
  - **Albo = otto armadietti personali**; il numero sull'armadietto è il punteggio accumulato; dentro, nastrini e
    decorazioni (in una scatola sullo scaffale), tuta, tuta anti-g, effetti personali. Un'opzione nascosta «recupera
    pilota» resuscita un pilota perso (non i generali).
  - **Teatro**: missione singola o campagna; **realismo**: dodici voci «Standard»/«Authentic», quattro livelli.
  - **Sala briefing**: lavagna con nominativo radio assegnato dal comando, primario e secondario, mezzi di supporto
    (AWACS, J-STARS), rapporto d'intelligence, meteo, carico proposto, **coordinamento delle forze con gli orari**
    (equipaggio in sala 50 min prima, a bordo 30 min prima…); carta della missione a sinistra; un televisore mostra
    **filmati del bersaglio**; il **cestino** rifiuta la missione, e **rifiutarne troppe nuoce alla carriera**.
  - **Armamento**: quattro stazioni, carico simmetrico, «3 × 2», CLEAR STATION, NEXT STATION, MORE (vista da sotto),
    OK; il manuale dice che il peso influisce sulle prestazioni ma non mostra cifre.
  - **Debriefing**: dopo l'atterraggio, **replay animato e cronologico degli eventi principali**, poi punteggio,
    medaglie e premi; END MISSION solo fuori dalla zona dà un punteggio appena ridotto, l'abbandono nessuno.
  - **Decorazioni**: Air Force Commendation, Air Medal, DFC, Silver Star, Air Force Cross, Medal of Honor, più Purple
    Heart; un nastrino di campagna per teatro; **foglie di quercia** bronzo e argento per le ripetizioni.
  - **Fine**: schianto mortale; troppi lanci col paracadute: «ritiro per il bene del servizio»; generale: Pentagono.
    Grafica lodata, lentezza criticata [media] [WP-F15III].

### 1.4 *F-14 Tomcat* (Dynamix/Activision, 1988–89)

Il più «carriera» dei titoli citati [media] [C64W-F14]: ci si arruola (nome nell'elenco degli ensign) → addestramento
primario con punteggio 1–100 che **smista** in caccia, elica o elicotteri → addestramento avanzato con un istruttore
che **assegna il nominativo secondo il rendimento** (da «Crash» a «Mr. Ace»), ali d'oro e promozione → addestramento al
combattimento (vincere 2 su 3) → **turni** di quattro operazioni sulla *Nimitz*, con licenza → fine per scelta, età,
disonore (perdita delle ali), commissione medica o schianto (con l'immagine del **corteo funebre**). Briefing e
debriefing dell'**ammiraglio Hawk** (ordini di fuoco «a volontà» o «solo se attaccati»; giudizio da eccellente a
disastro; medaglia, promozione o perdita delle ali). Registro: grado, età, punteggio, reputazione, missioni, toppe,
medaglie. L'elenco dei gradi riportato dal wiki mescola gradi di Marina e d'Esercito [bassa: probabile errore della
fonte]. Una recensione amatoriale lo dice sottovalutato e stima **una trentina d'ore** per vedere tutto il percorso.

### 1.5 *A-10 Tank Killer* (Dynamix, 1989)

[alta sul gioco] [A10-MAN]: due modi, *Fly One Mission* e *Start Campaign* (un «turno» di missioni in ordine, dove
obiettivi e personaggi **si trascinano** da una missione all'altra: una missione andata male «torna a perseguitarti»).
Il comandante **Cord**, personaggio con ritratto, fa briefing e debriefing; poi *Mission Summary* (esito e punteggio
tattico per numero e tipo di uccisioni) e, in campagna, *Campaign Summary* e *Campaign Decision* («prossimo incarico» o
salva e torna al menu). **Weapons Load**: cinque carichi preimpostati, uno marcato **consigliato** per la missione. I
punteggi ottenuti con munizioni illimitate o invulnerabilità **non entrano nell'albo d'onore**. Telecamere esterne
(vista della vittima, vista dell'ingaggio). Accoglienza: CGW 4/5, divertimento rapido più che realismo [media]
[WP-A10].

### 1.6 *Chuck Yeager's Air Combat* (EA, 1991)

Missioni storiche (WWII, Corea, Vietnam), *Create a Mission* (anche fuori epoca), volo libero [media] [WP-CYAC],
[COMP-CYAC]. Dopo uno scontro storico dice **chi era il pilota vero e com'era finita**, novità poi imitata [media]
[WP-CYAC]. Yeager loda o rimprovera a voce; il **replay** si ferma, si salva, cambia velocità e segue qualunque aereo.
Critiche: missioni isolate, niente campagna né pilota salvato [media] [COMP-CYAC]; niente punti, medaglie o
promozioni, ma il simulatore «più simpatico» [media] [PCP-CYAC].

### 1.7 *Falcon 3.0* (Sphere/Spectrum HoloByte, 1991)

Manuale letto nella terza edizione (dicembre 1995) [alta sul gioco] [F3-MAN]:

- **War Room** come menu principale: monitor e oggetti della stanza sono i punti attivi (carta del mondo, area dello
  squadrone). **Squadron Ready Room** con il registro dello squadrone (missioni, sortite, abbattimenti, bersagli a
  terra, piloti attivi, KIA, MIA) e lo **schedario** per trasferire piloti.
- **Pilot Information**: nome, nominativo, **cambio del volto**, statistiche, tasto **Awards** per le decorazioni; lo
  squadrone parte con **30 piloti**, perde caduti e affaticati, riceve rimpiazzi.
- **Pianificazione in campagna**: briefing assegnato dall'ufficiale di collegamento, poi Edit Flights, Set Waypoints,
  Assign Pilots (con la **fatica** dei piloti), Load Armament. Carta con icone rosse e blu, **cerchi di minaccia**, sei
  modi di visualizzazione (tattico e strategico, con o senza reticolo e scala), **Trace** (percorre la rotta ed elenca
  le minacce incontrate, numerate, in un piccolo monitor), **Recon** (posizioni note delle forze a terra, **vecchie fino
  a una settimana**), zoom fino alla vista 3D.
- **Ogni schermata ha un valore predefinito**: chi non vuole pianificare accetta tutto con Invio.
- **Carico**: per aereo, con *Clear All Weapons*, *Default Load*, *Copy Leader*; un riquadro dice **peso al decollo,
  g massimi e resistenza**; il manuale insiste sul compromesso fra potenza di fuoco, manovrabilità e autonomia.
- **Debriefing** in tre parti: (1) **traccia del volo disegnata sulla carta**, e **a ogni incontro il monitor a destra
  descrive l'evento** (abbattimenti, unità distrutte, gregari persi, schianti; solo gli eventi del proprio volo);
  (2) decorazioni (se nessuno le merita la schermata resta vuota; prima del debriefing c'è un'**animazione della
  cerimonia** solo se qualcuno è decorato); (3) **Mission Summary**: successo o fallimento con **il motivo più
  evidente** scelto da un elenco («non hai distrutto il bersaglio primario», «hai colpito il bersaglio sbagliato»,
  «perdite alleate inaccettabili», «hai finito la missione troppo presto»…), conteggio delle perdite e **stato di ogni
  pilota** (attivo, ferito, deferito alla corte marziale, prigioniero, disperso, caduto; l'asterisco segna
  l'affaticato). Poi la carta dell'occupazione; il debriefing si può **rivedere dall'inizio**.
- **Decorazioni** per accumulo di punti (abbattimenti, concorsi, bersagli primari, missione compiuta): Medal of Honor,
  Air Force Cross, Silver Star, DFC, Bronze Star, Air Medal, Purple Heart.
- **ACMI**: registratore di combattimento consultabile anche dal debriefing (usato per il video *Art of the Kill*).
  Simulazione dell'anno 1992 per CGW, criticata per i **molti difetti** [media] [WP-F3].

### 1.8 *Falcon 4.0* (1998) e *Allied Force* (2005)

**Campagna dinamica** senza trama fissa; il **libro di volo** registra ore, abbattimenti aria-aria e aria-suolo,
decorazioni, nome, **foto** e grado; buona condotta → decorazioni e promozioni; fuoco amico o lancio ingiustificato
col seggiolino → **degradazione o corte marziale** [media] [WP-F4]. In *Allied Force*, secondo un documento di
appassionati [bassa] [BMS-LOG]: punti di missione (pilota rientrato +5, disperso −15, caduto −25, amico abbattuto
−200; arma lanciata −1, a vuoto −6, colpo +4), promozioni a soglie cumulative con **condizioni di campagna** (maggiore
a 801 punti a campagna conclusa, colonnello a 3201 a campagna vinta), medaglie con punti calcolati da realismo,
difficoltà, voto e complessità della missione e con un **realismo minimo** (90 % per l'Air Force Cross); la serie di
missioni consecutive senza perdite dà una medaglia di anzianità a 100 e si azzera con difetti noti da anni.
Accoglienza: 95 % su *PC Gamer*, migliore simulazione del 1998 [media] [WP-F4].

### 1.9 *Tornado* (Digital Integration/Spectrum HoloByte, 1993)

Il **pianificatore** è il cuore: intelligence, bersagli, briefing, punti di rotta e pattuglie, verifica dei carichi,
**carte meteo**, vista 3D dell'area, carta del fronte che **riflette le missioni appena volate**, più voli Tornado da
coordinare [media] [WP-TOR]. *PC Zone* ne fu entusiasta ma **lamentò l'assenza del replay**; CGW criticò combattimento
aria-aria, tastiera e viste [media] [WP-TOR]. Lezione: una pianificazione profonda va bene, ma il giocatore vuole
**rivedere** quello che è successo.

### 1.10 *TFX* (1993) e *X-COM: Interceptor* (1998)

*TFX*: azione immediata, missioni libere e campagna in cinque teatri, carico personalizzabile; CGW lamentò poi la
**mancanza di una campagna** (contraddizione fra le fonti) [media] [WP-TFX]. *X-COM: Interceptor*: gestione di basi e
finanze, **rapporto settimanale** con stato, raccomandazioni e notizie, punti di campagna per missione; tono giudicato
troppo scherzoso; schermate dei piloti non documentate [media] [WP-XCI]. Lezione: il **notiziario fra le missioni**.

### 1.11 *B-17 Flying Fortress* (Vektor Grafix/MicroProse, 1992)

**Gestione dell'equipaggio**: si dà nome e stemma al bombardiere, si scelgono i **dieci uomini** studiandone missioni,
promozioni e medaglie; turno di **25 missioni**; i caduti si sostituiscono [media] [DD-B17], [WP-B17]. Gli uomini
migliorano e vengono promossi; chi gioca passivo o lascia fare all'IA (puntamento, decollo) prende meno medaglie e
**penalità nascoste**, «non particolarmente punitive»; il **compressore del tempo** accorcia i lunghi trasferimenti
[media] [THC-B17]. Critica di una retrospettiva: le ferite e lo stress sono resi in modo blando, **la foto di gruppo
dell'equipaggio non cambia** mai [media] [THC-B17]. CGW lodò l'approccio «per personaggi», criticò la documentazione
[media] [WP-B17].

### 1.12 *Lock On: Modern Air Combat* (Eagle Dynamics/Ubisoft, 2003)

Editor di missione e campagne (il testo del briefing si scrive nel campo *Description*; i risultati della campagna si
leggono con *Results*) [media] [CA-LO]. Il **track** è la **registrazione dei comandi** di tutti i partecipanti,
rieseguita dal simulatore; si rivede da *Show Replay*, si esporta in video e si può **riprendere il controllo** durante
la riproduzione [media] [CA-LO], [DCS-TRK] (estratto). È la stessa architettura del nostro replay deterministico e del
«Riprendi da qui» (requisito 16).

### 1.13 *Eagles*: quale gioco?

Nessun *Eagles* MicroProse. Corrisponde *Eagles* di **SSI** (1983 Atari 800, C64 1987): duelli della Grande Guerra,
tre scenari, fino a venti aerei, fine missione con abbattimenti, durata e **grado** [media] [LB-EAG], [MG-EAG]
(estratto); l'*Eagles* Hewson è uno sparatutto senza legami. Alternative [bassa]: *F-15 Strike Eagle* abbreviato,
*First Eagles* (2006), *War Eagles* (DOS 1989). Da chiedere all'utente; per le schermate porta poco.

---

## 2. I giochi navali e di sottomarini

### 2.1 *Silent Service* (MicroProse, 1985)

[alta sul gioco] [SS-MAN]: tre tipi di scenario (esercitazione, azioni contro convogli storiche con data e battello,
**pattugliamenti di guerra** storici da Midway, Brisbane o Fremantle); quattro livelli d'abilità (Midshipman …
Captain) e **sette opzioni di realismo** accendibili (visibilità limitata, zig-zag dei convogli, siluri difettosi,
riparazioni solo in porto, cacciatorpediniere esperti, ricerca dei convogli, angolo di prora da inserire). Abilità e
realismo danno un **fattore di difficoltà da 1 a 5** che pesa il tonnellaggio. Fine missione: elenco delle navi
affondate e **grado finale** da Ensign a «World's Greatest Submarine Captain»; il **Quartermaster's Log** riassume la
pattuglia in corso; l'**albo d'onore** dei sommergibilisti mette il nome del giocatore accanto ai **tonnellaggi reali**
di battelli storici (es. *Flasher* oltre 100 000 t). CGW: il migliore del suo tempo [media] [WP-SS].

### 2.2 *Silent Service II* (MPS Labs/MicroProse, 1990)

[alta sul gioco] [SS2-MAN] (manuale tedesco): carriera con **base di partenza** (che decide il comando, SubPac o
SubSoWesPac, e quindi le zone), **zona di pattugliamento** sulla carta del Pacifico, battello che prende il nome di un
battello reale della classe, **ordini di navigazione** copiati nel giornale di bordo; il carburante limita la
pattuglia. Al rientro: accoglienza secondo il risultato, **schermata di encomio o di rimprovero**, poi **albo
d'onore** (migliori punteggi e capitani reali con i loro tonnellaggi).
- **Punti**: ogni nave danneggiata o affondata vale secondo la sua forza; l'affondamento dà un **bonus per la scorta**
  (un trasporto truppe ben scortato vale quanto un incrociatore); pesano difficoltà, classe del battello e siluri
  «storici» o perfetti; contano soprattutto i totali di pattuglia e di carriera.
- **Gradi**: si parte capitano di corvetta; una sola promozione in guerra (capitano di fregata); a fine guerra, secondo
  la difficoltà, capitano di vascello o contrammiraglio, che non comandano più battelli.
- **Medaglie per pattuglia secondo i punti, non il tonnellaggio**: BSV < Silver Star < Navy Cross < Medal of Honor;
  **di reparto** NUC < PUC; Asiatic-Pacific Campaign Medal a chi sopravvive a tutta la guerra.
- **Rimozione**: il rimorchio in porto per mancanza di carburante, se ripetuto, allontana dal servizio. CGW: 4,5/5
  [media] [WP-SS2].

### 2.3 *Red Storm Rising* (MicroProse, 1988)

Il precedente più vicino (SSN americano nel Mare di Norvegia) [alta sul gioco] [RSR-MAN]:
- **Epoca** a scelta (1984, 1988, 1992, 1996) che decide armi e minacce: nel 1988 Mk 48 ADCAP e Tomahawk; poi Sea
  Lance, Stinger, Seawolf (pp. 7–8). **Prova di riconoscimento** di una sagoma navale (sbagliando, solo addestramento).
- **Battello** scelto per classe, oppure **assegnato dal «detailer»** del personale secondo il caso e la consistenza
  della flotta atlantica (p. 8). Livelli Introductory … Ultimate (in Ultimate l'identificazione sonar è a mano).
- **Ordini di missione** dopo una breve introduzione, rileggibili dall'Attack Center; carta del teatro dove **i simboli
  cambiano colore e forma con l'età dell'avvistamento** (pp. 9, 31–32).
- **Carico in porto**: «add/remove» un'arma alla volta; si caricano i tubi dal rapporto del secondo; i pozzi verticali
  solo sui Los Angeles migliorati e solo in porto; **partire scarica i tubi**, quindi vanno ricaricati; rifornimento a
  Holy Loch, mentre il nemico intanto avanza (pp. 20–22, 31).
- **Fine dello scontro**: rapporto automatico a COMSUBLANT, esito e nuovi ordini; fra le missioni **notiziario** con un
  **indicatore rosso-blu** dell'andamento della guerra (p. 33).
- **Efficiency Rating**: media del rendimento, pesata su qualità dell'opposizione, tipo di battello, armi disponibili e
  livello; medaglie solo in campagna e solo con ER alto (Commendation, BSV, SS, DSM, NC, CMOH); **niente promozioni
  dopo ogni battaglia**: in guerra il comandante resta al suo grado (p. 6). A fine guerra un **grado postbellico** da
  commander ad ammiraglio, oppure, se vince il Patto, «Tovarishch» e prigione [media] [WP-RSR].

CGW 4,5/5 per l'equilibrio realismo-gioco, con riserve sulla robustezza irrealistica dei battelli [media] [WP-RSR].

### 2.4 *Wolfpack* (NovaLogic/Brøderbund, 1990)

Missioni datate (l'anno sblocca battelli e contromisure alleate), comando del branco o della scorta, **editor di
missioni** (posizioni, ordini e rotte delle due parti), lodato da CGW nell'edizione CD [media] [WP-WP].

### 2.5 *Aces of the Deep* (Dynamix/Sierra, 1994; *Command* 1995)

[alta sul gioco] [AOD-MAN]: nuova carriera con nome, **periodo d'inizio** ed **emblema della torretta**; schermata
d'**assegnazione** (numero e tipo dell'U-boot, base); fino a 20 carriere salvate («in mare» o «in porto»). Menu di
carriera: *War Patrol* (briefing con obiettivi e **impostazioni di realismo** prima di accettare), *View Career*
(pattuglie, tonnellaggio, affondamenti, punteggio, elenco datato, decorazioni), ***Tonnage Leaders*** (classifica
mensile dei comandanti), ***Nightclub*** (voci e notizie fra una pattuglia e l'altra). Siluri: schermata dei tubi e
delle riserve interne ed esterne, carico manuale con tempi, trasbordo dal ponte che impedisce l'immersione fino a due
ore. Fine pattuglia: rientro con richiesta di scorta; **abbandonare la missione cancella il punteggio** già fatto.
**Albo d'onore**: migliori missioni, migliori carriere, **assi storici** con profili; una carriera vi entra quando
finisce (fine guerra, morte, cattura, congedo).

### 2.6 *Silent Hunter* (Aeon/SSI, 1996)

Carriera dal dicembre 1941 al 15 agosto 1945; missioni speciali (ricognizione fotografica, recupero di aviatori);
**medaglie in proporzione al successo di ogni pattuglia**; una pattuglia a vuoto porta un **rimprovero** di COMSUBPAC,
**due di fila la rimozione dal comando** (fine del gioco); confronto del proprio tonnellaggio con quello dei comandanti
reali [media] [WP-SH] e descrizione ufficiale (estratto). Accoglienza: 91 % *PC Gamer* [media].

### 2.7 *Harpoon* (Three-Sixty Pacific, 1989)

Briefing di scenario in **sezioni numerate**: *Background*, *Orders*, *Intelligence Brief*, *Minimum Victory
Conditions* (es. «raggiungere Narvik con almeno un mercantile»); poi ordini (Ctrl-O) e ordine di battaglia (Ctrl-B);
contatti esatti, ad area (rombo d'incertezza) o di solo rilevamento, con **incertezza che cresce col tempo** [alta sul
gioco] [HP-MAN]. CGW «wargame dell'anno» 1990 [media] [WP-HP]. Modello per il **testo** del briefing e per i contatti
d'intelligence invecchiati.

### 2.8 *688(I) Hunter/Killer* (Sonalysts/EA, 1997)

Erede diretto del nostro gioco; quattro tipi di missione fra cui la campagna, più di 30 missioni [media] [GR-688I]; la
pagina Steam dichiara la gestione delle risorse e del carico delle armi [bassa] (`14` [688I-Steam]). La stazione Armi
mostra le armi stivate e ricarica i tubi; una recensione critica le **ricariche troppo rapide** [media] [GR-688I].
Manuale non letto: schermate di briefing, carico e debriefing [lacuna].

### 2.9 Il punto di partenza: *688 Attack Sub* (1989)

*Mission Selection* con le 10 missioni e una colonna per lato (688 o Alfa), pannello di configurazione con BEGINNER o
STANDARD, BEGIN GAME [alta] [ORIG-MAN]; briefing come testo d'ordini; esito come pagina di testo con i toni di carriera
(promozioni, Navy Cross, Medal of Honor, gulag) già analizzati in `13` §0.1 [alta] [ORIG-BR]. Nessun albo, nessun
carico, nessun debriefing a eventi: è tutto da aggiungere nel livello «simulazione».

---

## 3. Punteggi e ricompense a confronto

| Gioco | Base del punteggio | Moltiplicatori | Decorazioni | Promozioni | Fine | Conf. |
|---|---|---|---|---|---|---|
| *F-19* | eventi pesati per tipo di guerra e ROE; tempo dietro le linee | regione, tensione, nemico, realismo, distanza, missione (×≈9) | per missione a soglie fisse; ripetizione ×2, ×3 | missioni + totale + media | morte permanente; 99 missioni | alta s.g. |
| *F-117A* | eventi valutati uno per uno dal CO | nemico, atterraggi, realismo del velivolo | AM…MOH; Purple Heart; nastrini a 5/15/30/60 missioni | totale + media + esperienza | generale, scrivania o congedo | alta s.g. |
| *F-15 III* | obiettivi + bersagli extra − danni collaterali | livelli di realismo | 6 medaglie + Purple Heart, nastrini di teatro, foglie di quercia | da 2nd Lt a generale | troppi lanci: ritiro | alta s.g. |
| *Falcon 3.0* | successo/fallimento con motivo; punti-medaglia accumulati | — | 7, per accumulo | — | stato pilota (KIA, POW, corte marziale) | alta s.g. |
| *Falcon 4 AF* | punti per pilota, aereo, armi, colpi | realismo, difficoltà, complessità | soglie con realismo minimo | soglie + esito della campagna | degradazione, corte marziale | bassa |
| *Silent Service* | tonnellaggio | fattore 1–5 | — | grado finale fino a «WGSC» | — | alta s.g. |
| *Silent Service II* | danno per forza del bersaglio + bonus scorta | difficoltà, classe, siluri | per pattuglia sui punti; di reparto | 1 in guerra, 1 a fine guerra | rimozione per incuria | alta s.g. |
| *Red Storm Rising* | Efficiency Rating medio | opposizione, battello, armi, livello | solo con ER alto | nessuna in guerra; grado postbellico | sconfitta: «Tovarishch» | alta s.g. |
| *Silent Hunter* | successo della pattuglia | — | proporzionali | — | 2 pattuglie vuote: rimozione | media |
| *Aces of the Deep* | tonnellaggio, punteggio | realismo | sì (soglie non lette) | sì | morte, cattura, congedo; abbandono azzera | alta/lacuna |
| *A-10* | punteggio tattico per uccisioni | — | — | — | aiuti esclusi dall'albo | alta s.g. |
| *F-14 Tomcat* | giudizio a parole | — | sì, più toppe | sì; rimproveri rallentano | perdita delle ali, età | media |
| *B-17* | condotta dell'equipaggio | aiuti dell'IA penalizzati (nascosti) | all'equipaggio | all'equipaggio | 25 missioni | media |

Il nostro sistema (`13` §2.3.1 e §3.7) ha già preso da *F-19* il moltiplicatore e le soglie per missione: questo
confronto lo **conferma** e aggiunge quattro idee (PROPOSTA): (1) da *Red Storm Rising* e *Silent Service II*, **poche
promozioni in guerra** e decorazioni per pattuglia (coerente con la carriera compressa di `13`); (2) da *F-117A*,
**nastrini automatici per numero di missioni** (in `13` §3.7 punto 5 come nastrini di servizio); (3) da *Silent Service
II*, **bonus per la scorta** del bersaglio (in `13` è il moltiplicatore M_avversario: va reso per bersaglio, non solo
per missione); (4) da *A-10* e *Falcon 4 AF*, **aiuti visibili** che escludono dall'albo d'onore o dalle decorazioni
più alte, invece delle penalità nascoste di *B-17*.

---

## 4. Cosa funzionava e cosa annoiava

**Funzionava** (da recensioni e manuali):
1. **Metafora delle stanze** (*F-117A* Ready Room, *F-15 III* hangar, *Falcon 3* War Room): orienta senza menu
   astratti e dà atmosfera; ogni stanza è una funzione [alta s.g.].
2. **Livelli della carta accendibili** con «tutto/niente» (*F-117A*) e **Trace** della rotta con le minacce numerate
   (*Falcon 3*): il briefing diventa uno strumento, non un muro di testo [alta s.g.].
3. **Valori predefiniti ovunque** (*Falcon 3*: Invio accetta tutto; *A-10*: carico «consigliato»; *F-19*: carico già
   adatto): il principiante non si blocca, l'esperto personalizza [alta s.g.].
4. **Debriefing a eventi sulla carta** (*F-117A*, *Falcon 3*) e **replay** (*F-15 III*, *Yeager*, *Lock On*); la
   mancanza del replay fu un difetto citato per *Tornado* [media].
5. **Conseguenze persistenti**: morte permanente (*F-19*), stato dei piloti (*Falcon 3*), campagna che ricorda gli
   errori (*A-10*), fronte che si sposta (*Red Storm Rising*, *Falcon 3*).
6. **Confronto con la storia**: albi con i tonnellaggi reali (*Silent Service*, *Silent Hunter*), assi storici (*Aces
   of the Deep*), esito reale dello scontro (*Yeager*): molto apprezzato [media].
7. **Personaggi**: il comandante Cord (*A-10*), l'ammiraglio Hawk (*F-14*), la voce di Yeager: il giudizio ha un volto.
8. **Esito raccontato**: giornali, proteste diplomatiche, cortei funebri (*F-19*, *F-14*).

**Annoiava o fu criticato**:
1. **Quiz di protezione dalla copia** prima del volo (*F-19*, *F-117A*, *RSR*) [media]: da noi niente blocchi; al più
   un **esame di riconoscimento facoltativo** nelle fasi di qualifica della carriera (`13` §2.3.2) e negli achievement.
2. **Rotte predefinite sbagliate** che obbligano a ritoccare (*F-117A*) [media]: le nostre rotte proposte devono
   evitare le zone note (SOSUS, campi minati, zone protette R2), oppure dichiarare perché non lo fanno.
3. **Complessità della pianificazione** e tastiera (*Tornado*, *Falcon 3*) [media]: profondità sì, ma a strati.
4. **Missioni isolate senza carriera** (*Yeager*) [media] e, all'opposto, **carriere lunghissime** (*F-14*, ~30 ore;
   99 missioni di *F-19*) [media]: serve la carriera compressa di `13` §2.3.2 e un «Carriera breve».
5. **Penalità nascoste** (*B-17*) e **difetti nei conteggi** del libro di volo (*Falcon 4*) [media/bassa]: tutto il
   calcolo va mostrato nella schermata d'esito, voce per voce.
6. **Rappresentazione statica delle perdite** (la foto di gruppo di *B-17* che non cambia) [media]: da noi il ritratto
   d'equipaggio e il fascicolo devono **mostrare** feriti e caduti.
7. **Ricariche irrealistiche** (*688(I)*) [media]: già risolto in `14` §5.4.
8. **Abbandono che azzera** (*Aces of the Deep*) e **END MISSION** ridotto (*F-15 III*): regola chiara, ma va detto
   prima, nel briefing.

---

## 5. Tabella comparativa per tipo di schermata

| Schermata | Esempio migliore | Tratti chiave | Adattabile a un sottomarino? |
|---|---|---|---|
| Albo / scelta del personaggio | *F-15 III* (armadietti), *Falcon 3* (volti, nominativo, Awards) | ritratto, punteggio sull'etichetta, decorazioni a un clic, morti non caricabili | **Sì**: «quadro dei comandanti» della squadriglia (USA) o schedario dei *lichnoe delo* (URSS) |
| Opzioni di difficoltà e realismo | *F-117A* (scheda del pilota), *Silent Service* (7 opzioni → fattore 1–5) | si vede che la difficoltà paga | **Sì**: pannello con il moltiplicatore M di `13` calcolato dal vivo |
| Scelta della missione / teatro | *F-117A* (ufficio del CO), *RSR* (epoca, battello assegnato), *SS II* (base e zona) | regione datata, livello di conflitto | **Sì**: teatro, anno, situazione (pace, crisi, guerra) dalla missione; per le rapide anche dal seme |
| Briefing con carta | *F-117A* (livelli, T/P/S/L), *Falcon 3* (Trace, Recon invecchiata), *F-15 III* (lavagna, orari, filmato) | livelli, rifiuto, scelta armi | **Sì**, con livelli navali (zone R2, SOSUS, ASW, ellissi d'intelligence) |
| Testo degli ordini | *Harpoon* (Background, Orders, Intelligence, Victory Conditions), *RSR* (ordini rileggibili) | sezioni fisse | **Sì**: il messaggio della serie 0 con DTG, FM/TO/SUBJ |
| Carico | *F-117A* (vano → arma, carburante stimato), *Falcon 3* (predefinito, copia, peso e resistenza), *A-10* (5 carichi, uno consigliato), *RSR* (add/remove, epoca) | numero di armi, scheda | **Sì, cambiando metrica**: posti e colpi per tipo, pronti nei tubi, non il peso (`14` §3.1) |
| Partenza | *F-117A* hangar, *F-15 III* cupola | conferma «pronto?» | **Sì**: uscita dalla base sul pontile, «In mare» |
| Debriefing a eventi | *Falcon 3* (traccia + monitor degli eventi), *F-117A* (CO valuta evento per evento) | carta, evento, punti | **Sì**: carta con le tracce e minicartoline (`00_ROADMAP`) |
| Replay | *Lock On* (track di comandi, riprendi il controllo), *Yeager* (VCR, segui chiunque), *F-15 III* | timeline | **Sì**: già nel requisito 16 |
| Esito e motivo | *Falcon 3* (motivo più evidente), *A-10* (Mission/Campaign Summary) | successo/fallimento spiegato | **Sì** |
| Cerimonia | *Falcon 3* (animazione solo se qualcuno è decorato), *SS II* (encomio o rimprovero) | medaglia, motivazione | **Sì** (`13` §5.4) |
| Promozione / commissione | *F-19* (tre minimi), *RSR* (niente promozioni in guerra) | soglie | **Sì** (`13` §2.3.3) |
| Esiti narrativi | *F-19* (giornale, protesta), *F-14* (corteo) | pagina illustrata | **Sì**: titolo di giornale d'epoca o comunicato TASS |
| Fine carriera | *F-117A* (99 missioni), *SS II* (fine guerra), *Silent Hunter* (rimozione) | congedo, scrivania | **Sì** (`13` §2.3.4) |
| Albo d'onore | *Silent Service* e *Aces of the Deep* (con dati storici), *A-10* (esclusi gli aiuti) | migliori missioni e carriere | **Sì**, ma senza tonnellaggi di guerra: vedi §7.10 |
| Notiziario fra missioni | *RSR* (barra della guerra), *Aces* (Nightclub), *X-COM I.* (rapporto settimanale) | contesto | **Sì**: rassegna stampa e messaggi di flotta |

---

## 6. Principi per il remake (PROPOSTA)

1. **Tutto si genera dai dati della missione e dal registro della partita** (M1), così le schermate valgono per le 10
   missioni originali, le rapide e quelle dell'editor; i blocchi facoltativi `briefing = { ... }` e `carriera = { ... }`
   aggiungono testi e segni (`docs/09` §2; `13` §2.3.5).
2. **Stanze della base** come snodo (da *F-117A* e *F-15 III*), ma sempre con tasti diretti (F1–F8) e con un percorso
   «Avanti» che attraversa le stanze nell'ordine giusto: chi vuole solo giocare preme Invio e accetta i predefiniti
   (da *Falcon 3*).
3. **Il calcolo si vede**: ogni punto del debriefing e dell'esito ha la sua riga (contro le penalità nascoste).
4. **Classico invariato**: nel livello classico restano *Mission Selection*, configurazione e pagina d'esito
   dell'originale; le schermate nuove con `--regola simulazione` (`docs/12`).
5. **Due marine, due grafiche**: stessa struttura, cornici e lingua dei timbri diverse (US Navy: «SECRET», blu e oro,
   carta intestata del COMSUBLANT o COMSUBPAC; VMF: «SEKRETNO» in caratteri latini stile stampino come da requisito 12,
   rosso e grigio, carta del comando della Flotta del Nord).
6. **Rispetto per il tempo del giocatore**: ogni schermata si salta (Esc → predefinito) e le animazioni si accorciano
   (la cerimonia si vede intera solo la prima volta per ogni tipo di decorazione, STIMA).

---

## 7. Le schermate del remake, una per una (PROPOSTA)

Tela 640×480. Griglia comune (STIMA): **testata** 640×28 (y 0–27: timbro di classifica, titolo, DTG, nome del
battello); **corpo** y 28–447; **piede** 640×32 (y 448–479: pulsanti, sempre negli stessi posti: a sinistra «Indietro»,
a destra «Avanti»). Font bitmap del gioco (8×16 per il testo, 8×8 per le etichette); telescrivente per i testi lunghi
(stesso effetto della radio, requisito 21: si legge tutto, si accelera con la barra).

### 7.0 Flusso

```
Albo dei comandanti ─▶ Base (stanze) ─▶ Scelta della missione ─▶ Briefing ─▶ Sala siluri ─▶ In mare (missione)
       ▲                                                                                        │
       │                                                                                        ▼
  Fascicolo ◀── Cerimonia ◀── Commissione (se dovuta) ◀── Esito e punteggio ◀── Debriefing ◀─────┘
       │                                                      │
       └── Albo d'onore                                       └─▶ Replay / «Riprendi da qui» (requisito 16)
```

Il **notiziario** (rassegna stampa, messaggi di flotta) si apre fra la Base e la Scelta della missione quando la
carriera ha novità (da *RSR*, *Aces of the Deep*, *X-COM I.*).

### 7.1 Albo dei comandanti

- **Contenuto**: fino a 8 carriere (come gli armadietti di *F-15 III*) come **cartellini** con foto piccola (64×80),
  nome, grado, battello, punteggio di carriera, stato (in servizio, a terra, rimosso, congedato, **«still on patrol»**
  per chi è caduto in mare: è l'espressione con cui i sommergibilisti americani ricordano i battelli perduti [media];
  per la VMF «POGIB PRI ISPOLNENII», PROPOSTA). Un cartellino «Nuova carriera» e «Carriera breve» (`13`
  §2.3.2). I caduti si consultano ma non si caricano (da *F-19*); «recupera» solo al livello indulgente (`13` §2.3.4).
- **Layout**: bacheca di sughero (USA) o schedario metallico (URSS) a tutto corpo, 4×2 cartellini da 152×200.
- **Comandi**: frecce e Invio; Canc chiede conferma due volte (definitivo come in *F-117A*).
- **Dati**: fascicoli salvati (M3); ritratto a strati (M4, `13` §4.4).

### 7.2 Base: le stanze

- **Contenuto**: una sola immagine (banchina con il battello ormeggiato e la palazzina del comando; per l'Alfa la base
  di Zapadnaja Lica, `13` §4.4) con **punti attivi**: ufficio del comandante di squadriglia (scelta della missione),
  sala briefing, locale siluri, pontile (In mare), circolo ufficiali (fascicolo, albo d'onore, notiziario).
- **Layout**: corpo 640×420 a 256 colori; etichetta del punto sotto il cursore nella testata.
- **Comandi**: mouse sui punti; tasti F1–F5; Invio = prossima stanza del percorso.
- **Prestito**: *F-117A* Ready Room, *F-15 III* hangar, *Falcon 3* War Room.

### 7.3 Scelta della missione

- **Contenuto**: quattro schede.
  - **Carriera**: i prossimi ordini (da `13` §2.3.2), con teatro, data e situazione.
  - **Missioni originali**: le 10 con la colonna 688/Alfa come l'originale [ORIG-MAN].
  - **Missione rapida**: teatro, stagione, ora, meteo, tipo, forze, paranoia, difficoltà; **codice missione** (seme e
    parametri) da digitare o copiare (requisito 10).
  - **Editor e importate**: elenco dei pacchetti con versione, verifica d'integrità e dipendenze, autore.
  Ogni voce ha il bollino **«Conta per la carriera»** sì/no con la ragione (dichiarata nel blocco `carriera`,
  ricalcolo della difficoltà, rendimenti decrescenti, `13` §2.3.5).
- **Pannello di realismo** a destra (da *F-117A* e *Silent Service*): opposizione, aiuti (verità sulla carta, TMA
  automatica, identificazione automatica…) e **il moltiplicatore M che cambia dal vivo** (M_situazione × M_avversario
  × M_realismo di `13` §2.3.1); un'avvertenza quando un aiuto esclude dall'albo d'onore (da *A-10*).
- **Layout**: elenco a sinistra 400×400, scheda della missione a destra 224×400 (titolo, teatro con miniatura della
  carta 208×120, anno, situazione, durata stimata).
- **Dati**: indice delle missioni, generatore deterministico, verifica dei pacchetti, regole di carriera (M3).

### 7.4 Sala briefing

- **Contenuto**: testata **«SECRET — INTELLIGENCE BRIEFING»** / «SEKRETNO», titolo della missione, DTG. Carta del teatro
  con la grafica N1 e il menu dei livelli. Livelli (PROPOSTA, ciascuno con tasto):
  1. **Ordini e obiettivi**: punti a lettere come in *F-117A*: **P** partenza (o punto d'ingresso nel teatro),
     **O** obiettivo primario, **S** secondario, **R** rientro o punto d'uscita, più i punti di rotta pianificati
     (A, B, C…) modificabili trascinandoli (da *F-117A* e *Falcon 3*);
  2. **Area operativa e zone** dello spazio d'acqua (R2: azione, libera ASW, protette, mobili), con orari;
  3. **Contatti d'intelligence** con ellisse ed **età** (colore che sbiadisce col tempo, da *RSR* e *Harpoon*);
  4. **Basi e radar costieri**, aeroporti ASW;
  5. **Portate delle armi nemiche**: missili antinave, raggio d'azione dei pattugliatori ASW, cortine di boe;
  6. **SOSUS e campi di sorveglianza** (dove la missione li dichiara);
  7. **Fondale e strato**: profondità per fasce, profondità prevista dello strato (dati dei teatri);
  8. **Eventi speciali** (aggiunti dal blocco `briefing`).
  Più «**Seleziona tutto / Pulisci tutto**» (da *F-117A*) e **«Traccia»** (da *Falcon 3*): percorre la rotta
  pianificata ed elenca, numerati, i rischi incontrati (zona di un campo SOSUS, portata di un pattugliatore, acque
  basse, zona protetta) in un riquadro.
- **Testo**: pannello «Ordini» che si apre sopra la carta come una telescrivente: il messaggio della **serie 0** (DTG,
  FM/TO/SUBJ, classifica), seguito dalle sezioni fisse alla *Harpoon*: Situazione, Ordini, Intelligence, **Regole
  d'ingaggio** (stato iniziale R1 per categoria), **Condizioni di successo** e, in piccolo, le regole di fine
  missione (abbandono, rientro, cosa conta per la carriera).
- **Supporto e tempi** (da *F-15 III*): orologio radio (finestre di ascolto, R1), mezzi amici (P-3C, altri SSN), meteo e
  stato del mare, alba e tramonto (requisito 8).
- **Pulsanti** (piede): «Ordini», «Traccia», «Scegli le armi» (→ Sala siluri), «**Rifiuta la missione**» (in carriera
  costa: PROPOSTA un rifiuto gratuito per commissione, poi una nota negativa nelle note caratteristiche, come in *F-15
  III*), «In mare».
- **Layout**: carta 448×412 (x 0–447, y 32–443); menu dei livelli 184×300 a destra (righe da 18 px con casella e
  tasto); sotto il menu un riquadro 184×108 per le informazioni sul punto sotto il cursore (da *F-117A*).
- **Dati**: missione (teatro, posizioni note, obiettivi, `briefing`), radio (serie 0, R1 stato iniziale delle ROE, R2
  zone e finestre), contatti d'intelligence con età ed errore, disegno della carta e simboli (N1–N4), dati dei teatri.
- **Classico**: il briefing è il testo dell'originale; la carta mostra solo teatro e posizioni dichiarate.

### 7.5 Sala siluri

- **Contenuto** (dettagli in `14` §5.3; qui solo cosa prendere dagli altri): a sinistra il **deposito** con le
  quantità (`deposito`, `tipi_negati`, anno di servizio, `14` §5.2); al centro la **pianta** del locale (4 tubi del 688,
  rastrelliere, 12 pozzi sui Flight II; 6 tubi e caricatore dell'Alfa); in basso la **scheda** dell'arma.
- **Prestiti**:
  - da *A-10*: **carichi preimpostati** (i carichi standard per tipo di missione di `14` §5.1), uno marcato
    **«Consigliato»** per questa missione;
  - da *Falcon 3*: «**Carico standard**», «**Svuota**», «**Come l'ultima missione**» (al posto di *Copy Leader*), e un
    riquadro dei totali che al posto di peso e resistenza mostra **posti usati / 25**, **pronti nei tubi**, **ricariche
    possibili prima del primo contatto stimato** (STIMA dai tempi di `14` §5.4);
  - da *F-117A*: la tabella d'**efficacia** dell'arma, qui per bersaglio navale (sottomarino in immersione, nave di
    superficie, bersaglio a terra, posa di mine), con un voto a stelle e la **corrispondenza con gli obiettivi** del
    briefing («nessun bersaglio di superficie previsto»);
  - da *RSR*: **armi datate** (l'anno della missione nega le armi non in servizio) e «aggiungi/togli» uno alla volta;
    la regola «**partire scarica i tubi**» non serve: i tubi alla partenza si scelgono qui;
  - da *F-117A*: la conferma «pronti a partire?» sul pulsante «In mare».
- **Peso**: solo informativo, in piccolo (`14` §3.1).
- **Layout**: deposito 160×300 (x 0–159), pianta 480×300 (x 160–639, y 32–331), scheda 640×112 (y 332–443) con
  sagoma dell'arma 192×64 a sinistra e dati a destra (guida, velocità, portata, testata, posti, anno, fonte e
  confidenza come la libreria del sonar).
- **Comandi**: trascina dal deposito al posto o al tubo; tasti 1–4 (o 1–6) per i tubi; Tab fra le aree.
- **Dati**: M2 (carico.toml, deposito della missione, classi, armi), stato iniziale dei tubi passato alla stazione Armi.

### 7.6 In mare

Transizione saltabile (pontile, rimorchiatori); il registro salva **carico alla partenza** (`14` §5.5) e realismo.

### 7.7 Debriefing

- **Contenuto**: testata «SECRET — PATROL DEBRIEFING» (USA) / «SEKRETNO — RAZBOR POKHODA» (URSS, latino stile
  stampino, PROPOSTA). Carta con le **tracce**: il battello (linea continua), i contatti come li
  conosceva l'equipaggio (tratteggiati, con le soluzioni TMA), le armi (scie e filo); tasto **«Verità»** che sovrappone
  le posizioni reali (requisito 16). **Eventi uno alla volta**, come il comandante di *F-117A* che valuta ogni evento e
  come la traccia di *Falcon 3* che si ferma a ogni incontro: la linea del battello **si disegna fino all'evento**,
  l'evento si accende sulla carta e compare la sua **minicartolina**.
- **Riquadro dell'evento** (a destra): numero (7/23), DTG, descrizione a parole («Contatto S-4 classificato Victor III,
  rilevamento 312, 14 kyd, 9 minuti dopo la prima scoperta»), **punti dell'evento** con la voce di `13` §2.3.1 a cui
  vanno (C, S, O, E, D, V) e **totale progressivo**.
- **Minicartoline** (il pezzo distintivo, da `00_ROADMAP` Fase 4): riquadro illustrato **sulla carta, accanto al punto
  dell'evento**, unito al punto da una linea sottile. La più vicina fonte d'epoca è il monitor degli eventi di
  *Falcon 3* [alta s.g.]; la posizione sulla carta è il ricordo del committente per *F-19*/*F-117A* [bassa]. Regole
  (PROPOSTA, STIMA):
  - dimensione **128×96** (4:3) con cornice di 2 px e didascalia 8×8 sotto; al massimo **una grande** (l'evento
    corrente) e le precedenti ridotte a **francobolli 32×24** lasciati sulla carta come segnaposto cliccabili;
  - posizione: nel quadrante libero più vicino al punto, senza coprire la traccia degli ultimi 5 minuti né gli altri
    francobolli (scelta deterministica, così due debriefing uguali hanno la stessa impaginazione);
  - contenuto per tipo d'evento: **scene visibili** (affondamento, esplosione, emersione d'emergenza nemica, lancio di
    missili, nave avvistata al periscopio, elicottero sopra il battello) disegnate **rigiocando il registro fino a
    quell'istante** e renderizzando la vista del periscopio o una vista esterna con i modelli e gli effetti del kit
    (M4, requisito 17); **eventi non visibili** con un'icona d'epoca: riga sulla cascata (contatto passivo), cerchio
    del ping (sonar attivo nemico), striscia di telescrivente (messaggio ricevuto o perso, con il DTG), ago dell'ESM
    (radar che illumina l'albero), sezione verticale con il termoclino (attraversamento dello strato), spaccato del
    battello con il compartimento colpito (danno subito);
  - **filtro d'interesse**: si mostrano gli eventi sopra una soglia (STIMA: i 12–20 più interessanti, requisito 16);
    «Tutti gli eventi» li elenca senza cartolina;
  - **cache**: le cartoline si disegnano in sottofondo all'apertura del debriefing, una per evento, a 128×96
    direttamente (non riduzione di un 640×480, che perderebbe leggibilità a 256 colori).
- **Pulsanti** (piede): «Evento precedente», «**Evento successivo**» (Invio), «Tutti gli eventi», «Verità»,
  «**Rivedi da qui**» (apre il replay all'istante dell'evento), «**Riprendi il comando da qui**» (requisito 16; non
  conta per la carriera, PROPOSTA), «Esito».
- **Layout**: carta 448×412 a sinistra; riquadro dell'evento 184×200 a destra in alto; sotto, **elenco compatto** degli
  eventi (184×212, righe di 12 px: DTG, icona, punti), dove la riga corrente è evidenziata.
- **Dati**: M1 (registro degli eventi con ora, posizione, unità, descrizione, interesse, voce di punteggio), quadro del
  giocatore e verità (posizioni campionate), radio (`debriefing()`: ROE trasmesse e copiate, serie perse), carico alla
  partenza e consumato (`14` §5.5), danni (`docs/10`), replay deterministico per le cartoline.
- **Classico**: nessun debriefing; solo la pagina d'esito dell'originale.

### 7.8 Esito e punteggio

- **Contenuto**: modulo d'epoca **«PATROL REPORT»** (USA) o **«DONESENIE O POKHODE»** (URSS, latino): in alto la
  **frase d'esito** (quella dell'originale per le 10 missioni, `13` §0.1; generata per le altre), poi la **tabella delle
  voci** R, C, S, O, E, D, V con i punti e il **moltiplicatore** M scomposto (situazione, avversario, realismo, indice di
  pericolo), il **totale P**, e la **nota caratteristica** a parole (`13` §2.3.1). Se la missione è fallita, **il motivo
  più evidente** scelto da un elenco (da *Falcon 3*): «obiettivo primario non raggiunto», «bersaglio sbagliato»,
  «neutrale colpito», «missione abbandonata», «serie radio obbligatorie perse», «battello scoperto prima
  dell'attacco»… con un rimando all'evento del debriefing che lo prova.
- **Equipaggio**: stato degli uomini (feriti, caduti) dal modello dei danni, con la stessa evidenza dello stato dei
  piloti di *Falcon 3*; il ritratto di gruppo **cambia** (contro il difetto di *B-17*).
- **Esito narrativo** (da *F-19*, *F-14*): in alcuni casi una **pagina di giornale** d'epoca (titolo e trafiletto; per
  la VMF un comunicato TASS) o un **messaggio di flotta** al posto della cerimonia: protesta diplomatica dopo un
  neutrale colpito, «battello in ritardo» dopo una perdita, nessuna notizia per le operazioni segrete (la regola
  «niente per la stampa» di `13` §5.2).
- **Pulsanti**: «Debriefing» (torna), «Replay», «Continua» (→ commissione o cerimonia o fascicolo).
- **Layout**: modulo 560×400 centrato su fondo di scrivania; colonne voce / misura / punti; il totale in un riquadro
  timbrato.
- **Dati**: M1 e M3 (formula di `13`), situazione e regole della missione, stato dell'equipaggio.

### 7.9 Commissione, cerimonia e fascicolo

Già proposti in `13` §5.4 (A cerimonia, B barretta, C fascicolo, D commissione, E inchiesta). Aggiunte da questo
confronto (PROPOSTA):
- **Ordine** come *Silent Service II*: esito → **encomio o rimprovero** (sempre, anche senza medaglia: una lettera del
  comandante di squadriglia) → cerimonia solo se c'è una decorazione o una promozione (da *Falcon 3*: se non c'è, la
  schermata non compare) → fascicolo.
- **Decorazioni di reparto** consegnate all'equipaggio schierato (da *SS II* NUC/PUC): stessa cerimonia, ritratto di
  gruppo invece del ritratto del comandante.
- **Nastrini automatici** per numero di pattuglie (da *F-117A*: 5, 15, 30, 60 missioni) tradotti nei nastrini di
  servizio dell'epoca (`13` §3.2 e §3.7 punto 5).
- **Ripetizioni** mostrate come in *F-15 III* (stellina o foglia) e nel fascicolo «aperto» come la scatola delle
  decorazioni nell'armadietto.
- **Layout della cerimonia** (STIMA): ritratto 256×320 a sinistra (x 32–287, y 40–359), medaglia nel riquadro 192×256 a
  destra (x 400–591) con cornice dorata (USA) o rossa (URSS), motivazione in telescrivente 576×80 sotto (y 368–447);
  animazione del nastrino che vola sulla barretta (`13` §5.4 punto 4).
- **Fascicolo**: sezioni di `13` §5.4 C, più (da *Aces of the Deep*) l'**elenco datato** di affondamenti e contatti
  classificati e (da *Falcon 3*) un **registro di squadriglia** con missioni, sortite, battelli persi.

### 7.10 Albo d'onore

- **Contenuto**: «Migliori missioni», «Migliori carriere», «Comandanti storici» (da *Aces of the Deep* e *Silent
  Service*). Nel 1989 non ci sono tonnellaggi di guerra da confrontare: PROPOSTA di sostituire il paragone con
  **schede di imprese reali documentate** (pattugliamenti e record del dossier `13` §3.3 e §3.6: *Parche*, *Halibut*,
  il transito del K-178, ecc.), senza punteggi inventati per persone reali.
- **Regole**: entrano solo le carriere chiuse (come *Aces of the Deep*) e solo le missioni senza aiuti che escludono
  (come *A-10*); le rapide con il loro codice missione, così chiunque può rigiocarle.

### 7.11 Dati richiesti al motore (riepilogo)

| Schermata | Dati | Pezzo |
|---|---|---|
| Albo, fascicolo, albo d'onore | carriere, ritratti, decorazioni, registro di squadriglia | M3, M4 |
| Scelta della missione | indice delle missioni, generatore con seme, pacchetti dell'editor, regole di carriera, moltiplicatore M | M3, editor |
| Briefing | teatro, obiettivi, posizioni note, serie 0, ROE iniziali (R1), zone e finestre (R2), contatti d'intelligence con età, SOSUS/basi/portate, rotta pianificata | missione, R1–R2, N1–N4 |
| Sala siluri | deposito, tipi negati, classi e posti, carichi standard, schede delle armi, efficacia per bersaglio | M2 |
| Debriefing | registro degli eventi con interesse e punti, tracce campionate, quadro del giocatore e verità, `debriefing()` radio, carico consumato, danni, replay per le cartoline | M1, M4, replay |
| Esito | formula di `13` §2.3.1, motivi di fallimento, stato dell'equipaggio, esiti narrativi | M1, M3 |
| Cerimonia e commissione | soglie di `13` §3.7, modelli di motivazione, barretta | M3, M4 |

---

## 8. Lacune note

1. ***F-19* e *F-117A***: intestazioni «CLASSIFIED», disposizione della carta del debriefing e riquadri illustrati
   accanto agli eventi non descritti in fonti aperte (il manuale di *F-117A* dice solo «passo per passo»); manuale DOS
   di *F-19* non letto (troppo grande per la lettura in rete). Va confrontato su una copia propria dei giochi.
2. ***Falcon 3.0***: letta la terza edizione del manuale (1995); ***Falcon 4 AF***: punteggi solo da appassionati.
3. ***688(I)***, ***Silent Hunter***, ***Aces of the Deep*** (pp. 31–36), ***B-17*** (230 pagine): schermate di briefing,
   carico, esito e soglie delle decorazioni non lette; ***X-COM: Interceptor***: piloti non documentati.
4. ***Eagles***: identificazione da confermare con l'utente.
5. **Recensioni d'epoca** lette quasi sempre attraverso Wikipedia: per citazioni dirette servono le scansioni di
   *Computer Gaming World* e delle riviste europee.

---

## 9. Fonti

Consultate il 24 ottobre 2026. Manuali letti nel testo OCR pubblico di archive.org o in PDF pubblici; nessuna immagine riprodotta.

### Progetto
- `[ROAD]` `docs/00_ROADMAP.md`: requisiti 10, 15, 16, 21; Fase 4 (briefing, Sala siluri, debriefing, riferimenti).
- `[D12]` `docs/12_schermate_di_missione.md` (flusso e pezzi M1–M5); `docs/09_schema_missioni.md` §2, §5; `docs/11_comandi_e_stazioni.md` (R1–R2, N1–N4); `ricerca/13_carriera_gradi_decorazioni.md`; `ricerca/14_carico_armi.md`.
- `[ORIG-MAN]` `docs/originale/manuale_EN.txt` (Mission Selection, Configuration Panel); `[ORIG-BR]` `docs/originale/briefing_EN.txt`.

### Simulatori di volo
- `[FSB-F19]` R. G. Sheffield, *The Official F-19 Stealth Fighter Handbook*, flightsimbooks.com: https://www.flightsimbooks.com/f19stealthfighter/06_10_Scoring_System.php ; …/07_2_Scoring.php ; …/07_3_Promotions.php ; …/07_4_Decorations_for_Valor.php
- `[WP-F19]` Wikipedia, *F-19 Stealth Fighter*: https://en.wikipedia.org/wiki/F-19_Stealth_Fighter
- `[DD-F19]` DOS Days, *F-19 Stealth Fighter*: https://www.dosdays.co.uk/topics/Games/game_f19.php
- `[F117-MAN]` MicroProse, *F-117A Nighthawk Stealth Fighter 2.0*, manuale (1991), testo OCR: https://archive.org/stream/f117Astealthfightermanual/F-117AStealthFighter-Manual_djvu.txt (pp. 27–35, 55–57, 137–141)
- `[WP-F117]` Wikipedia: https://en.wikipedia.org/wiki/F-117A_Nighthawk_Stealth_Fighter_2.0
- `[DD-F117]` DOS Days, *F-117A*: https://www.dosdays.co.uk/topics/Games/game_f117.php
- `[GH-F117]` debugcom, *Hacking-F117A* (studio della generazione delle missioni): https://github.com/debugcom/Hacking-F117A
- `[WP-F15]` https://en.wikipedia.org/wiki/F-15_Strike_Eagle_(video_game) ; `[WP-F15II]` https://en.wikipedia.org/wiki/F-15_Strike_Eagle_II ; `[WP-F15III]` https://en.wikipedia.org/wiki/F-15_Strike_Eagle_III
- `[F15III-MAN]` MicroProse, *F-15 Strike Eagle III*, manuale (1992), testo OCR: https://archive.org/stream/microprose-f-15-strike-eagle-iii-manual/Microprose_F15_Strike_Eagle_III_Manual_EN_djvu.txt
- `[C64W-F14]` C64-Wiki, *F-14 Tomcat*: https://c64-wiki.com/wiki/F-14_Tomcat ; `[SH-F14]` Sierra Help Wiki: https://wiki.sierrahelp.com/index.php/F-14_Tomcat
- `[WP-A10]` https://en.wikipedia.org/wiki/A-10_Tank_Killer ; `[A10-MAN]` Dynamix, *A-10 Tank Killer*, manuale, MOCAGH: https://www.mocagh.org/sierra/a10tank-manual.pdf (pp. 13–17)
- `[WP-CYAC]` https://en.wikipedia.org/wiki/Chuck_Yeager%27s_Air_Combat ; `[COMP-CYAC]` *Compute!* n. 133, settembre 1991, p. 106: https://atarimagazines.com/compute/issue133/106_Chuck_Yeagers_Air_C.php ; `[PCP-CYAC]` *PC Pilot* (guida), flightsimbooks.com: https://www.flightsimbooks.com/pcpilot/page81.php
- `[WP-F3]` https://en.wikipedia.org/wiki/Falcon_3.0 ; `[F3-MAN]` Spectrum HoloByte, *Falcon 3.0*, manuale, 3ª ed. (dic. 1995), DOS Days: https://dosdays.co.uk/media/games/falcon3/Falcon%203.0.PDF (pp. 37–40, 121–148)
- `[WP-F4]` https://en.wikipedia.org/wiki/Falcon_4.0 ; `[BMS-LOG]` forum Falcon BMS, «Total score in logbook»: https://forum.falcon-bms.com/post/34972
- `[WP-TOR]` https://en.wikipedia.org/wiki/Tornado_(1993_video_game) ; `[WP-TFX]` https://en.wikipedia.org/wiki/TFX_(video_game)
- `[WP-B17]` https://en.wikipedia.org/wiki/B-17_Flying_Fortress_(video_game) ; `[DD-B17]` DOS Days: https://dosdays.co.uk/topics/Games/game_b17.php ; `[THC-B17]` Tally-Ho Corner, «Dusty but trusty: B-17 Flying Fortress» (2021): https://tallyhocorner.com/2021/11/dusty-but-trusty-b-17-flying-fortress/
- `[WP-XCI]` https://en.wikipedia.org/wiki/X-COM:_Interceptor
- `[WP-LO]` https://en.wikipedia.org/wiki/Lock_On:_Modern_Air_Combat ; `[CA-LO]` CombatAce, *LOMAC final readme*: https://combatace.com/forums/topic/3548-lomac-final-readme/ ; `[DCS-TRK]` forum DCS, natura dei track (estratto): https://forum.dcs.world/topic/84648-something-very-funky-about-mission-data-recording/
- `[LB-EAG]` LaunchBox Games DB, *Eagles* (SSI 1983): https://gamesdb.launchbox-app.com/games/details/144267-eagles ; `[MG-EAG]` MobyGames, *Eagles* (estratto): https://www.mobygames.com/game/50937/eagles/

### Giochi navali
- `[SS-MAN]` MicroProse, *Silent Service*, manuale (Apple), testo OCR: https://archive.org/stream/Silent_Service-Manual/Silent_Service-Manual_djvu.txt ; `[WP-SS]` https://en.wikipedia.org/wiki/Silent_Service_(video_game)
- `[SS2-MAN]` MicroProse, *Silent Service II*, manuale tedesco (DOS), testo OCR: https://archive.org/stream/silent-service-ii-dos-german-manual/Silent-Service-II_dos_german-Manual_djvu.txt (pp. 64–67, 84–85, 101) ; `[WP-SS2]` https://en.wikipedia.org/wiki/Silent_Service_II
- `[RSR-MAN]` MicroProse, *Red Storm Rising*, manuale, testo OCR: https://archive.org/stream/redstormrisingmanual/RedStormRising-Manual_djvu.txt (pp. 5–9, 20–22, 31–33) ; `[WP-RSR]` https://en.wikipedia.org/wiki/Red_Storm_Rising_(video_game)
- `[WP-WP]` https://en.wikipedia.org/wiki/Wolfpack_(video_game)
- `[AOD-MAN]` Sierra/Dynamix, *Command: Aces of the Deep*, manuale, testo OCR: https://archive.org/stream/Command_Aces_Of_The_Deep_-_Manual/Command_Aces_Of_The_Deep_-_Manual_djvu.txt
- `[WP-SH]` https://en.wikipedia.org/wiki/Silent_Hunter_(video_game)
- `[HP-MAN]` Three-Sixty Pacific, *Harpoon Challenger Pak*, manuale (1991), testo OCR: https://archive.org/stream/Harpoon_Challenger_Pak_Manual_1991/Harpoon%20Challenger%20Pak%20Manual%20%281991%29_djvu.txt (pp. 11–16) ; `[WP-HP]` https://en.wikipedia.org/wiki/Harpoon_(video_game)
- `[WP-688I]` https://en.wikipedia.org/wiki/688(I)_Hunter/Killer ; `[GR-688I]` *Game Revolution*, «Dive! Dive! Dive!» (1/9/1997): https://www.gamerevolution.com/?p=33088
