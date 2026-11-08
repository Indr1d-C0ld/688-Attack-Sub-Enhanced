# 12 — Strumenti di plottaggio e aspetto della carta (dossier per il remake di *688 Attack Sub*)

Progetto: remake realistico di *688 Attack Sub* (EA 1989). Ambientazione 1989; modo moderno opzionale.
Scopo: base di ricerca per i **piccoli strumenti della carta** (stazione F6, `engine/app/carta.cpp`) per annotare,
misurare e segnare, realistici per l'attrezzatura del tempo, e per il **successivo restyle grafico** della carta in uno
stile VGA/SVGA dettagliato e colorato che ricordi il gioco del 1989 e il suo seguito *688(I) Hunter/Killer* (1997).
Solo ricerca: nessun codice. Compilato il 24 ottobre 2026.

---

## 0. Convenzioni, rimandi e punto di partenza

**Confidenza** (come `02` §0): **[alta]** fonte primaria o autorevole verificata, oppure geometria standard;
**[media]** fonte aperta credibile ma secondaria, oppure calcolo dell'autore su dati pubblicati; **[bassa]** forum,
giochi, ricordo non riverificato, interpolazione. **STIMA** = valore proposto per il gioco; **PROPOSTA** = scelta di
design. «estratto» = pagina letta solo attraverso il riassunto di una ricerca (accesso diretto negato, 403/402).
Le sigle tra parentesi quadre rimandano alla §7; quelle già definite in `02` ([TMA‑wiki], [NHHC‑Ekelund], [NA81]),
`06` ([BOW26]), `09` ([Amick20], [GUPPY], [GI10], [WP-BQQ10], [GD-BYG1], [MP-BSY1]) e `10` ([AAP6HAV], [COTE]) valgono
anche qui con lo stesso significato.

**Epoca** di ogni voce: **1989** = 688 Flight I/II con Mk 117 o CCS Mk 1, 688I con BSY‑1 (marginale nel 1989,
`01` §1), Alfa e Victor III; **moderno** = BYG‑1, ECDIS‑N/VMS, dagli anni '90–2000 in poi; **storico** = precedente
(TDC, Mk 101, GUPPY), utile solo come origine dei metodi manuali.

**Non ripetuto qui:** TMA solo rilevamenti, regola del 1934, Ekelund e strategia delle gambe (`02` §5.1); Doppler e
curva a S al CPA, consegna dal sonar alla TMA, tracker S1… e lessico POSSUB/PROBSUB (`09` §2.7, §2.9, §3.6, §3.10);
contatti d'intelligence con ellisse che invecchia e zone dello spazio d'acqua (`10` §4.1, §4.3); carta d'epoca
degradata, isobate, annotazioni ED/PA, incertezza della propria posizione e punti nave (`docs/08`, `06`).

### 0.1 Cosa fa già la carta (F6)

Da `docs/11_comandi_e_stazioni.md` §8–§11, `engine/app/carta.cpp`, `nucleo/carta_giocatore.hpp`, `nucleo/tma.hpp`:
- **vista**: 640×360 pixel della tela (640×480, 256 colori indicizzati), centrata sul battello; scala S da 2 a 52 con
  I e O come l'originale (S = 2 → 37 m/pixel, 640 px ≈ 12,8 nm; S = 52 → 963 m/pixel, 640 px ≈ 333 nm);
- **fondale**: campionato ogni 2 pixel dal `Fondale` vero del teatro e colorato in 4 classi EGA (terra marrone, < 50 m
  ciano chiaro, < 200 m ciano, oltre blu); B dà lo sfondo uniforme;
- **contatti**: X sul rilevamento a 25 pixel originali (e la X speculare della schiera), lettere con traccia solida,
  bersaglio evidenziato; **scie** dell'anello di 150 punti (1 ALL TRACKS, 2 TARGET TRACK); **punti di rotta** 1–9 (W,
  K, cursore con frecce o clic); **proiezione** a N minuti (J, `[` `]`) del battello e del bersaglio con una «*»;
- **MATE** (T, solo Enhanced): linee di rilevamento del bersaglio, posizioni della soluzione agli stessi istanti,
  striscia degli scarti ±5°, soluzione, scarto medio e rateo degli ultimi 2 minuti; `Tma` tiene fino a 720
  rilevamenti (uno ogni 5 s) e sa calcolare rateo ai minimi quadrati e distanza di Ekelund su due gambe;
- **intelligence** (R1): ellisse d'incertezza che cresce con l'età, centro, sensore e minuti;
- **cursore di SET**: rilevamento e distanza dal battello (`%03.0f°  %.1f NM`).

**Tasti già presi alla carta:** I O (zoom), W K (punti), J `[` `]` (proiezione), B (sfondo), T (MATE), 1 2 3 (scie e
proiezione), G (TARGET), frecce e Maiuscolo (cursore e MATE), Tab, Invio, Esc. **Globali** che la carta non deve
rubare senza toglierli come fa il Sonar (`interfaccia.cpp`, «resto» dei comandi): L, S, C, D, Z, X, Y (armi e filo),
R A M (tubi), U N H, P, V, F2–F9, tastierino, `+` `-`. **Liberi oggi:** E, F, Q, i pulsanti 4–9 della stazione,
Maiuscolo+lettera, la rotella del mouse alla carta.

**Osservazione.** La carta disegna oggi il **fondale vero**. Per `docs/08` §2 e §5 la carta del giocatore dovrebbe
essere la **carta d'epoca degradata** più le misure; il restyle (§5.6) è il momento naturale per farlo.

---

## 1. Le presentazioni («plot») del gruppo di tiro

### 1.1 Catalogo

| Presentazione | Cosa mostra | A che serve | Epoca | Conf. / fonte |
|---|---|---|---|---|
| **Plot geografico** (geo plot, *navigational plot*) | posizioni vere del proprio battello, contatti, punti nave, armi, su scala fissa | il quadro d'insieme dell'avvicinamento; controllo indipendente delle soluzioni | storico → 1989 → moderno (elettronico) | [media] [TFCM] (estratto): era la stazione «più importante», l’unica con il quadro intero; [media] [GUPPY]; [media] [NSL‑WWII] |
| **DRT** (*Dead Reckoning Tracer*) | lo stesso plot geografico, con il punto del proprio battello mosso da girobussola e solcometro sotto un piano di vetro | stima la propria posizione e la traccia; TMA grafica | dagli anni '40 fino alla fine della Guerra Fredda (superficie e sottomarini) | [alta] [RADFIVE]; [alta] [STU13] |
| **Plot tempo‑rilevamento** (*time‑bearing*, anche espanso o «a striscia») | rilevamento (ascisse) contro tempo (ordinate), un punto per osservazione, poi una curva liscia | filtra i rilevamenti cattivi; la pendenza è il rateo; mostra manovre del bersaglio | storico → 1989 (carta) → moderno (elettronico) | [alta] [US5029110]; [media] [NSL‑Patton08]; [media] [NSL‑Walsh01]; uomini migliori del calcolatore sulle curve complesse [media] [NSMRL716] (`09`) |
| **Plot rateo‑rilevamento contro rilevamento** (semilogaritmico) | rateo in ordinata (log) contro rilevamento | curva simmetrica concava il cui picco è il CPA | moderno (insegnato dagli anni '90–2000) | [media] [NSL‑Walsh01] |
| **Strisce di velocità di Ekelund** | regolo trasparente graduato in distanza percorsa a una velocità del bersaglio, fatto scorrere lungo l'ultimo rilevamento e inclinato finché le tacche cadono sui rilevamenti precedenti | dà angolo sulla prora, rotta e distanza | dagli anni '50 (GUPPY) | [media] [GUPPY]; formula `02` §5.1 |
| **Distanza di Ekelund** (formula) | distanza da due gambe | taglio rapido della distanza | anni '50 → moderno, anche «corretta per il tempo» | [alta] [TMA‑wiki]; [media] [NPS‑TMA] (indice, estratto) |
| **MATE / pila di punti** (*dot stack*) | per ogni rilevamento, lo scarto fra rilevamento misurato e previsto dalla soluzione, impilato nel tempo | se la pila è dritta e centrata su due gambe la soluzione è buona | sistemi digitali anni '70–'90 (Mk 113/117, CCS); «metodo della Guerra Fredda» | [media] [NSL‑Walsh01]; MATE come strumento manuale [media] `02` §5.1 |
| **Plot di valutazione dei contatti** (CEP, *Contact Evaluation Plot*) | nella Royal Navy: rilevamenti a mano o a plotter su carta tempo × rilevamento 360°, con prora propria, periscopio, annotazioni; negli USA il termine indica il grande plot di carta della centrale | il punto in cui si fondono i sensori e il comando legge il quadro | anni '70–'80, automatizzato dal 1987 circa | [alta] [US5029110]; [media] [NSL‑Patton08] |
| **Plot del moto relativo** (*maneuvering board*, «moboard») | posizioni relative al battello proprio, su un foglio polare standard | CPA, rotta e velocità relativa; manovre | storico → moderno (carta di riserva) | [alta] [STU13]; [media] [NSL‑Walsh01] |
| **Ventaglio di rilevamenti** (LOB, linee di rilevamento) | dal punto del battello all'ora della misura, una linea per rilevamento | sul geo plot i rilevamenti in deriva sembrano «raggi di una ruota» | storico → moderno | [media] [GUPPY] |
| **Scie** proprie e dei contatti, con segni di tempo | posizioni successive con l'ora | ricostruire la geometria, verificare la soluzione | sempre | [alta] [RADFIVE]; [alta] [MSC530] |
| **Plot tempo‑frequenza** (Doppler) | frequenza di una riga stabile nel tempo | velocità radiale e CPA dalla curva a S | 1989 (LOFAR) → moderno | `09` §3.6 |
| **Datum, cerchio di massimo avanzamento, area di pericolo siluri** | ultima posizione nota, cerchio che cresce con la velocità massima, area da cui un siluro può arrivare | ricerca dopo la perdita del contatto; autodifesa | 1989 (ASW di superficie) → moderno | [alta] [STU13]; `10` §4.1 |

**Il plot tempo‑rilevamento su carta (RN, anni '70–'80).** Il brevetto britannico del 1987 descrive il CEP manuale:
gli operatori dettavano i rilevamenti a voce e un uomo li tracciava a mano su fogli tempo contro rilevamento; con il
crescere dei dati non si stava più al passo. Il plotter automatico che lo sostituisce usa **quattro penne di colore
diverso** per quattro funzioni (prora propria; dati, annotazioni ed errori; tracce dei contatti; tempo, margini e
scale dei rilevamenti), fino a quattro ingressi sonar su 360°, rilevamenti dei tracker ogni 30 s o 2 min, tagli del
periscopio, e scale dei tempi di **2, 5 o 10 minuti per pollice**, con il cambio di scala annotato sul foglio.
[alta] [US5029110]. È il riferimento più concreto, e aperto, per l'aspetto di un plot «di carta» del 1989.

**Il DRT (riferimento per la «sensazione» del plot geografico).** Il manuale USN dell'operatore radar (anni '40)
descrive [alta] [RADFIVE]:
- il calcolatore della stima scompone rotta (girobussola) e velocità (solcometro) in componenti nord‑sud ed est‑ovest
  e muove un carrello («bug») sotto il vetro; il carrello porta una piccola **rosa con cerchi di distanza** e una luce
  che proietta **un punto luminoso** sul vetro: il plotter ricalca il punto a matita;
- non si scrive sul vetro: si usa **carta da lucido** in rotolo o in fogli fissati agli angoli, oppure plastica
  trasparente smerigliata;
- scale: **200 yarde per pollice**, oppure da 1/4 a 16 miglia per pollice in tre fasce colorate; per la caccia
  subacquea si resta a 200 o 500 yd/pollice;
- la luce può lampeggiare 15 s al minuto, per marcare i minuti sulla traccia;
- **segni di tempo**: ora e minuto a quattro cifre la prima volta, poi solo i minuti con un esponente per i quarti di
  minuto fino all'ora successiva;
- simboli: punto = proprio battello, **X** = contatto ignoto o nemico, cerchio = amico; rotta e velocità del contatto
  in un riquadro collegato da una linea all'ultima posizione; lettere cerchiate all'inizio di una traccia;
- velocità: **regola dei tre minuti** (in 3 minuti si percorrono tante centinaia di yarde quanti sono i nodi), o
  nomogramma, o compasso a punte tarato; se il DRT si guasta si prolunga la rotta segnando 100/3 yd al minuto per
  nodo.

Il DRT e il tavolo a proiezione **NC‑2** (un proiettore centrale per il proprio battello e due proiettori di bersaglio)
erano i plotter elettromeccanici delle centrali di superficie fino agli anni '80, con il difetto di dover seguire a
mano i punti luminosi [media] [NPS‑CIC] (estratto); [media] [Haida] (estratto); [alta] [STU13]. A bordo dei
sottomarini di epoca GUPPY c'era il **plotter Mk 19** per la TMA [media] [GUPPY]. Per il 688 del 1989 non ho trovato
una fonte aperta che dica quale tavolo di plottaggio fosse installato in centrale. [bassa]

### 1.2 Chi tiene cosa (gruppo di inseguimento e di tiro)

**USA, schema generale [media salvo dove indicato]:**
- **Comandante / ufficiale d'attacco** (*approach officer*): decide manovre e lancio; in alcune organizzazioni il
  comandante delega e il secondo fa da supervisore distaccato [media] [USNI87] (estratto).
- **Coordinatore del tiro** (*fire control coordinator*): costruisce la soluzione dalla TMA. [bassa] [STS‑e5]
- **Coordinatore del plot** (*plot coordinator*): spesso ha il quadro tattico migliore di tutto il gruppo [media]
  [USNI87] (estratto); riceve dal supervisore sonar la migliore stima della velocità del bersaglio [media] [GUPPY].
- **Operatori della direzione del tiro** (tecnici FT): alle console digitali (MATE, pila di punti). Dagli anni '80
  sono i sottufficiali a far girare i sistemi, gli ufficiali giovani hanno ruoli minori [media] [NSL‑Patton08].
- **Plotter del geo plot** e **plotter del tempo‑rilevamento**: due gruppi distinti sugli stessi rilevamenti [media]
  [GUPPY]; il geo plotter tiene la traccia propria e di tutti i bersagli su carta conservata per giorni [media] [GUPPY].
- **Supervisore sonar**: soluzione indipendente di controllo, collegamento con il comando [media] [Amick20]; il
  «tracker anziano» segue i contatti dalla scoperta al tiro e informa ufficiale di guardia e gruppo di tiro [bassa]
  [STS‑e5].
- **Ufficiale di rotta e timonieri di carteggio** (quartermaster): carta nautica, punti nave, stima. [bassa] (prassi
  generale, nessuna fonte aperta specifica per il 688)

**Superficie, per confronto (utile per le zone e i simboli).** Nella centrale ASW di una nave USA il plotter «sud»
tiene traccia propria e contatti sonar con **datum, errore del datum, cerchio di massimo avanzamento, area di pericolo
siluri** e ora d'arrivo; il plotter «nord» gli aerei e le navi d'appoggio con le boe e i canali; si usa anche una
**griglia XY** riferita a un punto noto (DLRP) per scambiare posizioni. [alta] [STU13]

**Royal Navy, anni 2010.** Uno studio sul comando di un SSN durante l'inseguimento in immersione descrive la squadra
in centrale come un sistema distribuito: chi guida la manovra, gli operatori TMA, il sonar e i plot condivisi.
[bassa] [Soton18] (solo estratto: il PDF non è accessibile).

**URSS.** Sull'Alfa la BIUS **«Akkord»** concentrava per la prima volta nella flotta sovietica il comando al posto
centrale: impiego delle armi, raccolta ed elaborazione delle informazioni tattiche, manovra di combattimento,
**riproduzione della situazione esterna**, navigazione, comando a distanza degli impianti; equipaggio di circa 30
uomini quasi tutti ufficiali; affidabilità dell'elettronica scarsa [media] [RU705]. Il Victor III e l'Akula avevano
BIUS di generazione successiva (il nome «Omnibus» ricorre in letteratura secondaria) [bassa]. Il calcolo del
navigatore sovietico (la «prokladka» sulla carta) e il planšet di manovra sono l'equivalente di geo plot e moboard
[bassa]: non ho trovato fonti aperte su come fosse disegnato il quadro tattico dell'Akkord.

### 1.3 Dal 1989 al moderno

| Elemento | 1989 | Moderno | Conf. / fonte |
|---|---|---|---|
| Calcolatore della direzione del tiro | Mk 113 → Mk 117 sul 688, calcolatore **AN/UYK‑7** (poi UYK‑43 da metà anni '80); il Mk 117 discende dal Mk 113, a sua volta un Mk 101 digitalizzato con molte parti analogiche | — | [media] [ARCK‑UYK7]; [media] [NSL‑Mk113] (estratto) |
| Tomahawk dai tubi | integrato nel Mk 117 invece di un sistema separato | — | [media] [USNI01] (estratto) |
| Sistema di combattimento | CCS Mk 1 (688 con Tomahawk); BSY‑1 integrato sul 688I dal 1988 | CCS Mk 2, poi **BYG‑1**: fusione dei sensori, TMA, gestione di molti contatti, controllo delle armi | [media] [MP-BSY1]; [alta] [GD-BYG1] |
| TMA | pila di punti e MATE alla console, plot di carta in parallelo (insegnati ancora anni dopo per il loro valore didattico) | algoritmi automatici, presentazione dell'incertezza; metodi nuovi per le distanze brevi | [media] [NSL‑Walsh01]; [media] [NSL‑Patton08]; [media] [NPS‑Prouty07] (estratto) |
| Navigazione | carta nautica su carta, DRT o equivalente, inerziale (`06` §2) | **ECDIS‑N** su software VMS (Sperry/Northrop Grumman); approvato per i sottomarini nel 2009 (VMS 8.3, con navigazione alle alte latitudini e sotto il ghiaccio); su Virginia Block III come mezzo primario; aggiornamenti su Virginia, Seawolf e Los Angeles dal 2022 | [media] [NG‑VMS] (estratti) |

**Regola mnemonica dell'epoca.** «Un nodo a un miglio fa un grado al minuto»: è la regola del 1934 arrotondata
(1934/2025 ≈ 0,96 °/min per kn a 1 nm), da cui si ricava la formula di Ekelund; gli istruttori la usavano per esercizi
di calcolo mentale, insieme a una scorciatoia per il rilevamento reciproco (+200 −20, oppure −200 +20) e a una stima
della velocità radiale dallo spostamento di frequenza. [media] [NSL‑Patton08]; calcolo [alta]

---

## 2. Strumenti fisici ed elettronici

### 2.1 Strumenti fisici (carta, DRT, CEP)

| Strumento | Uso | Resa possibile nel gioco | Conf. / fonte |
|---|---|---|---|
| **Compasso a punte** (*dividers*) | prende una distanza fra due punti e la riporta sulla scala | righello che legge distanza con arrotondamento alla tacca della scala | [alta] [BOW] |
| **Righe parallele** | trasportano una direzione dalla rosa dei venti a un punto | lettura del rilevamento rispetto alla rosa disegnata | [alta] [WP‑parallel]; [BOW] |
| **Plotter da navigazione** (Weems, rotante o «parallel plotter») | righello con goniometro: rilevamento rispetto a un meridiano; i modelli a rullo non vanno bene sui bordi e sulle pieghe | stesso strumento della riga parallela, in un solo gesto | [media] [Starpath] |
| **Goniometro a tre bracci** | punto nave da due angoli orizzontali fra tre punti cospicui | punto nave al periscopio (`docs/08` §6) | [alta] [USNI1904] |
| **Goniometro a moto parallelo** del DRT | rilevamenti diretti o reciproci dalla traccia propria all'ora della misura | linee di rilevamento dalla posizione del battello all'ora giusta, non da quella attuale | [alta] [RADFIVE] |
| **Foglio di plottaggio** (plotting sheet) e **foglio di manovra** (moboard) | carta senza costa con meridiani da numerare; rosa polare per il moto relativo | sfondo «senza fondale» (B) come foglio di plottaggio; pannello di moto relativo | [alta] [STU13]; [BOW] |
| **Matita grassa** (*grease pencil*) e lucido (*overlay*) | segni su plastica, cancellabili; lucido sovrapposto per la navigazione batimetrica (si «avanza» l'isobata) | strato dei segni del giocatore, cancellabile in blocco | [media] (prassi); avanzamento del contorno [alta] `06` §4 |
| **Scale velocità‑tempo‑distanza**, nomogramma, regola dei 3 minuti | tempi e distanze senza calcolatrice | lettura del tempo a velocità attuale sul righello | [alta] [RADFIVE] |
| **Rosa dei venti** (vera e magnetica) | riferimento per le direzioni | rosa attorno al battello come il «bug» del DRT | [alta] [RADFIVE]; [Chart1] |
| **Regolo di Ekelund** (striscia trasparente) | vedi §1.1 | strumento manuale di soluzione sul geo plot (§5.2, n. 9) | [media] [GUPPY] |
| **Ruota di Spears**, **plot di Spiess** | metodi geometrici storici per la TMA | solo come nota | [media] `02` §5.1 |

### 2.2 Funzioni elettroniche: ECDIS (norma IMO MSC.530(106), 2022)

La norma vigente per l'ECDIS mercantile (che la Marina USA adotta come base per l'ECDIS‑N) elenca funzioni che sono
esattamente gli strumenti richiesti per la carta [alta] [MSC530]:
- **scala** a passi (valori di scala o portate in miglia); orientamento **nord in alto** sempre disponibile, moto vero
  obbligatorio, ricentratura automatica quando il battello si avvicina al bordo a una distanza scelta (§5.7, §8);
- **profondità di sicurezza** e **contorno di sicurezza** scelti dal navigante (30 m se non scelto; se il contorno
  non c'è nei dati si usa il successivo più profondo e lo si segnala); il contorno di sicurezza è evidenziato su tutti
  gli altri; gli scandagli uguali o minori della profondità di sicurezza sono messi in risalto (§5.8–5.9);
- **interrogazione** di un punto col cursore (oggetti cartografici lì) e lettura delle coordinate di qualunque punto
  (§5.6, §11.4.16);
- **pianificazione**: aggiunta, cancellazione e spostamento di punti di rotta in forma grafica e numerica; rotte
  alternative; avviso se una rotta passa più vicino di una distanza scelta al contorno di sicurezza, a un'area
  vietata o con condizioni speciali, o a un pericolo isolato; **limite di deviazione laterale** (*cross‑track*) con
  allarme (§11.3);
- **sorveglianza della rotta**: allarme se entro un tempo o una distanza scelti si passerà più vicino del previsto al
  contorno di sicurezza o a un'area speciale; avviso di pericolo sulla rotta e velocità attuali; avviso prima di un
  **punto critico**; allarme di fuori rotta (§11.4.3–11.4.12);
- **segni di tempo lungo la traccia** a mano o automatici a intervalli fra **1 e 120 minuti**; **punti, linee di
  rilevamento elettroniche libere (EBL), anelli di distanza variabili e fissi (VRM)** e altri simboli (§11.4.15);
- **linee di posizione** manuali (rilevamento e distanza) con il punto risultante come origine della stima (§11.4.18);
- **registrazione**: traccia, prora e velocità ogni minuto per 12 ore, traccia intera con segni di tempo al massimo
  ogni 4 ore, non modificabile (§11.5);
- **calcoli**: distanza e azimut veri fra due punti, punto da distanza e azimut, lossodromia e ortodromia; le misure
  disegnate devono essere precise almeno quanto la risoluzione dello schermo (§12);
- **elementi di navigazione** (App. 3): traccia passata con segni di tempo (primaria e secondaria), vettore della
  rotta e velocità effettive, VRM/EBL, cursore, **evento** (punto stimato DR e punto stimato EP con ora), punto nave
  con ora, linea di posizione e linea trasferita, corrente, **evidenziazione di un pericolo**, **linea di sicurezza**
  (*clearing line*), rotta e velocità pianificate, punto di rotta, distanza da percorrere, posizione pianificata con
  data e ora, punto di inizio accostata;
- **aree con condizioni speciali** (App. 4), fra cui area militare d'esercitazione, **corridoio di transito dei
  sottomarini**, **aree da evitare definite dall'utente**;
- **categorie di presentazione** (App. 2): base non rimovibile (costa, contorno di sicurezza, pericoli isolati, scala,
  freccia del nord, unità, modo), presentazione standard, altre informazioni a richiesta (scandagli, cavi, reticolo,
  nomi…);
- **colori e simboli** IHO (S‑52), con la possibilità di mostrare il battello in scala vera o come simbolo (§9).

**Radar e ARPA (MSC.192(79), 2004).** I tracciatori calcolano **CPA e TCPA** da distanza e rilevamento misurati, con
precisioni tabellate a regime; la revisione ha migliorato il requisito sul CPA. [media] [MSC192] (estratti; numeri non
letti). Per il gioco basta la geometria (§2.4).

### 2.3 Console di direzione del tiro anni '80: cosa sappiamo

Le fonti aperte non descrivono i comandi delle console Mk 117 o CCS Mk 1 (cursori, misure, tasti). Sappiamo che la
TMA si faceva con la pila di punti e il MATE, che il quadro tattico era su un plot (di carta o elettronico) e che il
sistema digitale girava su UYK‑7/UYK‑43 [media] (§1.3). **Tutto ciò che un gioco mette su una console del 1989 oltre
al MATE è STIMA**: è ragionevole un cursore a sfera o a manopole che legge rilevamento e distanza dal battello (come
il cursore di SET di oggi) e la lettura di rotta e velocità della soluzione; non un ECDIS.

### 2.4 Geometria pronta per il codice [alta, geometria piana; la carta è azimutale equidistante, §4.4]

- **Righello due punti:** distanza `d = |B − A|`; rilevamento `atan2(Δest, Δnord)` in gradi bussola; tempo a velocità
  v: `d/v`. Regola dei 6 minuti: in 6 min si percorrono v/10 miglia.
- **CPA/TCPA:** posizione relativa `r = P_c − P_p`, velocità relativa `w = V_c − V_p`;
  `TCPA = −(r·w)/|w|²` (se ≤ 0 il CPA è passato), `CPA = |r + w·TCPA|`; rilevamento al CPA dalla direzione di
  `r + w·TCPA`. Con `|w|` ≈ 0 il CPA non è definito (moto parallelo).
- **Linea di rilevamento** presa all'istante t: parte da `P_p(t)` (dalla scia o dai `Rilevamento::proprio` della `Tma`)
  e va lungo il rilevamento vero per una lunghezza scelta (fino al bordo).
- **Cerchio di massimo avanzamento:** raggio `v_max·(t − t_datum)` attorno al datum; con l'errore del datum
  `σ` si somma: `R = σ + v_max·Δt` (forma di `10` §4.1 per un'area isotropa).
- **Distanza di Ekelund e rateo:** già in `Tma::distanza_ekelund` e `Tma::rateo`.
- **Errore di stima della propria posizione** (`docs/08` §6): ogni segno messo «sul battello» eredita la posizione
  **stimata**, non quella vera.

---

## 3. Simbologia

### 3.1 Simboli standard di contatto

| Sistema | Amico | Ostile | Ignoto / in attesa | Neutrale | Dimensione | Conf. / fonte |
|---|---|---|---|---|---|---|
| **APP‑6 / MIL‑STD‑2525** (NATO, dal 1986; con l'affiliazione da APP‑6A = 2525A) | cornice blu | rombo rosso | quadrifoglio giallo | quadrato verde | aria: cornice aperta in basso (metà superiore); superficie: cornice chiusa; **subacqueo: cornice aperta in alto** (metà inferiore) | [alta] [APP6‑wiki] |
| **NTDS** (schermi tattici della Marina USA) | blu | rosso (anche «sospetto») | giallo (anche «in attesa») | verde | forme semplificate per superficie, subacqueo, aria; modificatore «?» per presunto amico, in attesa e sospetto | colori [alta] [NTDS‑NPS]; forme (cerchio amico, rombo ostile, quadrato ignoto; metà inferiore per il subacqueo) [media], da verificare su una specifica |
| **APP‑6 del 1986**, versione monocromatica | bordo singolo | bordo doppio | — | — | — | [alta] [APP6‑wiki] |
| **DRT anni '40** | cerchio | X (anche ignoto) | X | — | punto per il proprio battello | [alta] [RADFIVE] |

Lettura per il gioco: la **metà inferiore** della forma = subacqueo è una convenzione comune a NTDS e APP‑6, ed è
leggibile anche a 9×9 pixel. Su schermo i colori sono della cornice; l'icona interna è nera o del colore della cornice.
[alta] [APP6‑wiki]; [media] [NATO‑CA] (estratto)

### 3.2 Come si annotano i contatti sui plot manuali

- **Designazioni:** «Sierra» + numero per il sonar (S1, S2…), «Master» quando più sensori tengono lo stesso
  contatto, Victor, Romeo, Echo per visivo, radar ed ESM: convenzione diffusa ma senza fonte ufficiale aperta [bassa]
  `09` §2.7. Il gioco originale usa **lettere** per i contatti con posizione e **X** per i soli rilevamenti [alta]
  [M688].
- **Segni di tempo:** ogni posizione o rilevamento porta l'ora; prima ora e minuti a quattro cifre, poi solo i
  minuti [alta] [RADFIVE]; l'ECDIS mette i segni automatici da 1 a 120 minuti [alta] [MSC530].
- **Rotta e velocità del contatto** in un riquadro accanto all'ultima posizione, unito da una linea [alta] [RADFIVE].
- **Datum** con l'ora e il suo errore, cerchio di massimo avanzamento, area di pericolo siluri, ora d'arrivo
  all'area [alta] [STU13].
- **Colori sui plot di carta:** il plotter automatico britannico assegna un colore di penna a ciascuna di quattro
  funzioni [alta] [US5029110]. Le carte nautiche riservano il **magenta** agli elementi di navigazione (luci, rose,
  aree regolamentate) [alta] [Chart1]. Un codice colore delle matite grasse nei sottomarini USA non è documentato in
  fonti aperte [bassa]: PROPOSTA in §5.6.
- **Abbreviazioni** classiche: C e S (rotta e velocità), OF/CF (fuoco aperto e cessato) [alta] [RADFIVE]; per i passi
  di classificazione POSSUB, PROBSUB, CERTSUB [bassa] `09` §2.9.

---

## 4. Riferimenti visivi per il restyle

### 4.1 La carta di *688 Attack Sub* (1989)

Dal manuale (`docs/originale/manuale_EN.txt`, «Top‑down map») e dal disassemblato (`numeri_eseguibile.md` §3.2) [alta]
[M688]; [NUM‑ESE]:
- **riquadro piccolo** (168×92 pixel su 320×200) presente in tutte le stazioni, centrato sul battello; dieci livelli
  di zoom (S da 2 a 52), scritta «ZOOM FACTOR n»;
- **tinte di profondità**: più scuro = più profondo (convenzione **batimetrica**, non nautica); in MCGA e VGA ci sono
  più livelli fra chiaro e scuro; il colore più chiaro è acqua da attraversare solo in emersione; si va da 0 a oltre
  2000 piedi;
- **contatti**: X a metà strada fra battello e bordo, qualunque sia lo zoom; lettere scure se non identificate,
  chiare se identificate; le «ovali» del contour imaging hanno le stesse lettere;
- **punti di rotta** come cifre di colore 13 (magenta chiaro), proiezione come «*» gialla (14) e rossa chiara (12),
  battello come quadratino verde chiaro (10) bordato di verde (2); scie a punti nei colori 4, 8, 0;
- **HUD** sopra la carta: bussola in alto con un triangolo bianco sulla prora, a destra un indicatore verticale con la
  quota propria, la quota del fondo sotto la chiglia e gli strati termici come tacche più lunghe; in alto a sinistra
  la direzione del periscopio;
- tasti della carta del 688: P F S (viste del contour imaging), I H O (zoom e HUD), C B T (contour, fondale, carta);
  l'Alfa ha solo SCALE su/giù, CONTOUR e HUD.

Ciò che ne resta come «firma» visiva: blu a bande nette che scuriscono con la profondità, simboli fatti di lettere,
colori EGA pieni, HUD sovrapposto. [alta]

### 4.2 La carta di *688(I) Hunter/Killer* (Sonalysts/EA, 1997)

- Le stazioni principali sono Sonar, TMA, Armi e Navigazione; la **navigazione** è la più semplice: una carta
  **ingrandibile e scorrevole** con le migliori posizioni note delle navi e una presentazione separata della **quota
  sotto la chiglia**; la TMA è la stazione più difficile, con un aiuto automatico per i principianti. [media] [GR97]
- Sviluppatore Sonalysts, già autore di simulazioni per il Dipartimento della Difesa; uscita 27 giugno 1997, Windows
  95. [media] [WP688I]
- Dal ricordo (da verificare su una copia del gioco, per esempio l'edizione GOG): simboli NTDS colorati per
  affiliazione con vettori di velocità, mare in toni di blu con terra in colore caldo, griglia, pannelli grigi da
  console tutto attorno, TMA con pila di punti. **[bassa]**. Non ho trovato manuali o descrizioni aperte delle
  schermate.

### 4.3 Convenzioni delle carte nautiche vere

- **Carta USA (Chart No. 1)** [alta] [Chart1]: azzurri per i possibili pericoli, cioè **acque basse** e ostacoli
  sommersi; **bianco** per le acque profonde ritenute sgombre; **camoscio** (*buff*) la terra sulle carte NOAA,
  grigio su quelle NGA; **verde** la fascia intertidale; **magenta** per luci, rose dei venti, aree regolamentate. Su
  molte carte l'azzurro è a due toni: più scuro = più basso [media] [Chart1].
- **ECDIS (S‑52)**: tre tavolozze (**giorno, crepuscolo, notte**); il contorno di sicurezza ha linea spessa e un salto
  netto di tinta; due tinte (più basso o più profondo del contorno di sicurezza) o quattro (con i contorni «basso» e
  «profondo»; profondo predefinito 30 m in un'implementazione); gli scandagli più profondi della profondità di
  sicurezza in grigio, gli altri in nero. [alta] [MSC530]; [media] [IHO‑DIPWG]; [media] [ESRI‑ENC]
- **Isobate**: sulle carte metriche internazionali la serie usuale è 2, 5, 10, 20, 30, 50, 100, 200, 500, 1000, 2000 m e
  poi ogni 1000 [media] (prassi S‑4, testo non riletto); le carte USA del periodo erano spesso in **braccia e piedi**
  [media]. I sottomarini del 1960 seguivano le isobate da 25, 50 e 100 braccia [media] `06` [NSL84].
- **Carte batimetriche** (GEBCO e simili): la convenzione opposta, più scuro = più profondo, con molte tinte; è quella
  del gioco del 1989.
- **Rilievo ombreggiato**: luce convenzionale da nord‑ovest (azimut 315°, 45° d'altezza) [alta, prassi cartografica].
- **Colore su carte elettroniche**: luminanza, illuminazione dell'ambiente, sfondo, ingombro e codici di colore sono
  i temi delle linee guida IHO del 1990 [bassa] [KG90] (solo sommario).

### 4.4 Che cosa supportano i dati del gioco

- **Batimetria** (`data/motore/teatri/<teatro>.fdl`): 7 teatri da circa 1470–1820 × 1000–1670 celle da **200 m**
  (circa 300 km di lato), quota a 0,25 m; proiezione **azimutale equidistante** centrata sul teatro (WGS84), centro
  registrato in `data/teatri/<teatro>/heightmap.json` ma **non** nel file del motore. Fonti EMODnet o ETOPO 2022 per
  cella (`fonte_u8.png`). [alta, verificato sui file]
- **Che cosa si può disegnare:** tinte per fasce, **isobate** con un algoritmo a quadrati marcianti sulle celle,
  ombreggiatura dal gradiente, terra con tinte per altezza (la quota è positiva sulla terra), reticolo geografico (con
  il centro della proiezione esportato nel motore), griglia XY da un punto di riferimento, annotazione della **fonte
  del rilievo** per zona (EMODnet = rilievo fitto, ETOPO = grossolano: un equivalente semplice della «zona di
  confidenza» delle carte elettroniche). [media, PROPOSTA]
- **Risoluzione:** a S = 2 un pixel è 37 m, cioè una cella da 200 m copre circa 5 pixel; da S ≈ 12 in su un pixel
  copre più celle. A scale grandi servono interpolazione (già in `Fondale::quota`) e isobate lisce; a scale piccole il
  campionamento ogni 2 pixel basta.
- **Carta contro realtà:** la carta d'epoca degradata (`docs/08` §5) andrà generata dalla stessa griglia; il restyle
  deve disegnare **quella** (con ED, PA, anno del rilievo) e sopra le misure (ecoscandaglio, BQS‑15).
- **Unità:** il gioco originale usa piedi per la quota; la carta USA del 1989 in braccia e piedi, quella sovietica in
  metri: un dettaglio d'epoca a costo zero. [media]

---

## 5. Proposte per il gioco

### 5.1 Principi (PROPOSTA)

1. **Gli strumenti misurano la carta, non la verità.** Le posizioni del battello sono quelle **stimate**; i contatti
   sono quelli del giocatore (rilevamenti, soluzioni MATE, tracce solide); il fondale è la carta d'epoca. La
   «verità» (V) resta diagnosi.
2. **Due modi, come il sonar** (`--sonar 1989|moderno`): **1989** = «matita grassa» (segni a mano, letture
   arrotondate alla tacca, niente allarmi, linee di rilevamento solo quando si marcano); **moderno** = ECDIS‑N e BYG‑1
   (letture esatte, EBL/VRM, CPA/TCPA continui con allarmi, segni di tempo automatici, registro eventi). Proposta:
   un solo interruttore d'epoca `--epoca 1989|moderno` che governi sonar e carta.
3. **Tutto finisce nel replay e nel debriefing:** segni, eventi e note sono dati deterministici della partita
   (non stato dell'interfaccia usa e getta), come le annotazioni del sonar (`09` §3.2).
4. **Il classico resta identico**: gli strumenti esistono solo con `--regola simulazione` (o almeno non cambiano il
   rilievo del classico), come il MATE.
5. **Pochi tasti nuovi**: un selettore di strumento sui pulsanti 4–9 (come il pannello Plotting dell'originale) e
   mouse diretto; i tasti delle armi non si toccano.

### 5.2 Strumenti in ordine di valore

| # | Strumento | Uso in partita | 1989 | Moderno | Costo |
|---|---|---|---|---|---|
| 1 | **Righello / EBL‑VRM** | distanza, rilevamento e tempo a velocità attuale fra due punti; dal battello: anello e linea che lo seguono | compasso e righe parallele: lettura a 0,1 nm e 1°, nessuna linea che segue il battello; si può fissare come tratto di matita | EBL e VRM centrati sul battello o liberi, lettura esatta, anche in kyd | basso |
| 2 | **Linee di rilevamento dai tracker** S1… con segni di tempo | il ventaglio che diventa «raggi di ruota»; incrocio con le linee prese da altre posizioni (triangolazione dopo la manovra) | il giocatore **marca** il rilevamento (o un plotter IA lo fa ogni 2 min, STIMA); linee nere con l'ora | ventaglio continuo per ogni tracker, colori per contatto, linee che sbiadiscono con l'età; X speculari della schiera | basso (i dati ci sono: `Tma`, tracker S5a) |
| 3 | **Segno d'evento con ora** («MARK») | segnare ora e posizione stimata: transitorio udito, lancio, ping intercettato, avvistamento, contatto perso (datum) | a mano (E); ora a quattro cifre come sul DRT | anche automatici (lancio, contatto perso, ping), registro eventi consultabile | basso |
| 4 | **Simboli e note** su un lucido | marcare amico/ostile/ignoto, campo minato sospetto, «qui S3 ha accelerato» | simboli disegnati a mano (forme semplici, colori di matita), testo breve; «pulisci il lucido» | simboli NTDS, note con ora, strati accendibili, esportabili nel debriefing | medio (serve l'inserimento di testo) |
| 5 | **CPA/TCPA** verso un contatto o lungo un tratto di rotta | evitare collisioni e baffles, calcolare quando si passa al traverso, preparare il tiro | solo su richiesta, con il moboard (pannello del moto relativo); arrotondato | continuo per il bersaglio e i contatti con soluzione; allarme sotto una soglia (STIMA 1 nm o 2 kyd) | basso |
| 6 | **Cerchi di distanza** | anelli fissi (1, 2, 5 nm) e **anelli d'arma**: corsa del Mk 48 alla velocità scelta, lunghezza del filo, portata del missile; anello CZ stimato | anelli a compasso attorno a un punto scelto | anelli dinamici dal battello e dai tubi; anello di scoperta stimata dal modello acustico (solo se l'operatore lo «calcola») | basso |
| 7 | **Proiezione della stima** con segni di tempo | oggi una «*» a N minuti: diventa una linea con tacche ogni minuto per il battello e per ogni contatto con soluzione (vettore di velocità) | linea a matita fino a N minuti | vettori per tutti i contatti, proiezione lungo la rotta pianificata (punti di rotta) | basso |
| 8 | **Datum e cerchio di massimo avanzamento** | contatto perso: dove può essere ora; si allarga col tempo come l'ellisse d'intelligence | cerchio a compasso ridisegnato a mano a ogni intervallo | automatico al contatto perso, con area di pericolo siluri | basso |
| 9 | **Regolo di Ekelund sul geo plot** | soluzione manuale: striscia graduata alla velocità stimata (dal DEMON o dal conteggio dei giri), fatta scorrere sull'ultimo rilevamento e ruotata finché le tacche cadono sulle linee precedenti → rotta e distanza | sì: è lo strumento d'epoca, gemello geografico del MATE | sì, come aiuto; con il MATE condivide la soluzione | medio |
| 10 | **Pannello tempo‑rilevamento** (striscia laterale) | punti per tracker, retta di regressione, rateo; selezione di due gambe → distanza di Ekelund (`Tma`); in moderno anche rateo contro rilevamento con il picco al CPA | carta a striscia, scala 2/5/10 min per «pollice» come il CEP | elettronico, con filtro dei punti cattivi | medio |
| 11 | **Zone** | aree da evitare dell'utente (cerchio o poligono), zone dello spazio d'acqua dalla radio (`10` §4.3), corridoio di transito; contorno di sicurezza **legato alla quota**: quota ordinata + margine (STIMA 30 m) | contorni a matita, nessun allarme | allarmi ECDIS: rotta pianificata o attuale che si avvicina al contorno di sicurezza o a una zona (§2.2) | medio |
| 12 | **Livelli e reticolo** | accendere e spegnere fondale, isobate, scie, linee, segni, zone, intelligence; reticolo lat/lon e griglia XY | foglio di plottaggio (B) + rosa | categorie base/standard/tutto come l'ECDIS | basso |

**Dettagli d'uso:**
- **Righello (1):** con il mouse si trascina dal punto A al punto B (il clic aggancia il battello, un contatto, un
  punto di rotta o un segno se vicino entro 6 pixel); da tastiera si apre sul battello e le frecce muovono l'estremo B,
  Tab passa all'estremo A, Invio lo fissa come tratto permanente con la sua etichetta (`247°  6,3 NM  31 MIN A 12 KN`),
  K toglie l'ultimo tratto. In modo 1989 la lettura compare dopo 2–5 s di gioco («il plotter misura», STIMA) ed è
  arrotondata alla tacca della scala in uso (0,1 nm fino a S = 12, 0,5 nm oltre).
- **Linee di rilevamento (2):** partono dalla posizione stimata del battello **all'istante della misura**; con la
  manovra le linee nuove si incrociano con le vecchie e il giocatore «vede» la distanza. È la lezione della TMA resa
  visibile senza spiegazioni. Con la schiera le due linee (vera e speculare) restano finché la virata non risolve.
- **Evento (3):** E mette l'evento all'istante e alla posizione stimata del battello; Maiuscolo+E apre la scelta del
  tipo (transitorio, avvistamento, lancio, datum, nota). Gli eventi automatici del modo moderno sono gli stessi che
  il sonar già annota sulle cascate (`09` §3.2), così le due stazioni raccontano la stessa storia.
- **CPA (5):** sul bersaglio scelto (G) e sulla soluzione in uso (MATE o traccia solida): punto di CPA su entrambe le
  tracce, linea del moto relativo, `CPA 1,8 NM  TCPA 14 MIN  RIL 312°`. Senza soluzione nessun numero.
- **Anelli d'arma (6):** dal tubo selezionato e dalle impostazioni del siluro (C6b): corsa utile alla velocità scelta e
  RTE come secondo anello (dove il siluro comincia a cercare); il filo dell'originale era di 7,5 nm (`docs/11` §1.1).
- **Ekelund (9):** la striscia ha tante tacche quanti sono gli intervalli fra le linee marcate, distanziate di
  `v·Δt`; frecce sinistra e destra la ruotano (rotta del bersaglio), su e giù la fanno scorrere lungo l'ultima linea
  (distanza); `[` `]` cambiano la velocità a passi di 0,5 kn. Quando le tacche cadono sulle linee, Invio passa rotta,
  velocità e distanza al MATE. È lo stesso problema del MATE presentato in forma geografica, come nei sottomarini
  degli anni '50 [media] [GUPPY].

### 5.3 Tasti e mouse (PROPOSTA)

| Tasto (alla carta) | Comando nuovo (nome rimappabile) | Note |
|---|---|---|
| **4** | `carta_righello` | apre e chiude il righello; il secondo 4 lo chiude |
| **5** | `carta_linee` | linee di rilevamento: spente, bersaglio, tutti i tracker |
| **6** | `carta_cerchi` | anelli: spenti, fissi, d'arma |
| **7** | `carta_segni` | modo segno: il cursore piazza simboli e note; Tab cambia simbolo |
| **8** | `carta_cpa` | lettura CPA/TCPA sul bersaglio (nel 1989: apre il moboard) |
| **9** | `carta_livelli` | ciclo dei livelli (base, standard, tutto) |
| **E** | `carta_evento` | evento con ora alla posizione stimata; Maiuscolo+E sceglie il tipo |
| **F** | `carta_aggancia` | aggancia l'estremo attivo al battello, al bersaglio o al segno più vicino |
| **Q** | `carta_ekelund` | regolo di Ekelund (libero oggi: nell'originale Q era la compressione del tempo alla carta, ora su PagSu/PagGiù) |
| **K** | (esistente) | nel modo di uno strumento toglie l'ultimo elemento di quello strumento, altrimenti l'ultimo punto di rotta come oggi |
| **Maiusc+K** | `carta_pulisci` | pulisce il lucido (segni e tratti), con conferma Invio/Esc |
| **`[` `]`** | (esistenti) | nel modo di uno strumento cambiano il suo valore (raggio, intervallo delle tacche, velocità della striscia), altrimenti i minuti della proiezione |
| Frecce, Maiuscolo, Tab, Invio, Esc | (esistenti) | cursore, più veloce, estremo o simbolo successivo, conferma, annulla |
| **Mouse** | aree della carta | sinistro: piazza o trascina (righello, segno); destro: annulla l'operazione in corso; rotella: zoom (I/O) alla carta |

Il testo delle note richiede un **modo d'inserimento** che prenda tutti i tasti (come la scelta del bersaglio prende le
lettere): Invio chiude, Esc annulla, al massimo 24 caratteri nel font dell'originale. I pulsanti 4–9 andranno anche
nel pannello disegnato della stazione, come ALL TRACKS, TARGET TRACK e PLOT PROJ dell'originale.

### 5.4 Modo 1989 e modo moderno a confronto

| Aspetto | 1989 (matita grassa, DRT, Mk 117) | Moderno (ECDIS‑N, BYG‑1) |
|---|---|---|
| Fondo | carta d'epoca su «carta» color camoscio o foglio di plottaggio bianco; isobate in braccia/piedi (USA) o metri (URSS) | tavolozza S‑52 giorno/crepuscolo/notte; contorno di sicurezza dinamico (quota + margine) |
| Segni | matite: nero, rosso, blu, verde, giallo (§5.6); tratto leggermente irregolare (1 pixel di tremolio, STIMA) | simboli NTDS, colori pieni, etichette nitide |
| Letture | arrotondate, con ritardo del plotter (STIMA 2–5 s) | esatte, immediate |
| Linee di rilevamento | solo quelle marcate (giocatore o plotter IA ogni 2 min) | continue per tracker |
| Tempo | segni a mano; evento con ora a quattro cifre | segni automatici ogni 1–120 min, registro eventi |
| Allarmi | nessuno: è l'equipaggio a parlare («fondo in salita», solo da misure: `docs/08` §7) | CPA, contorno di sicurezza, zone, fuori rotta |
| Soluzione | MATE, regolo di Ekelund, tempo‑rilevamento | MATE con pila di punti, rateo contro rilevamento, aiuto automatico come opzione (come *688(I)*) |
| Luce | «luce rossa» di notte (tavolozza rossa per la centrale, STIMA estetica) | tavolozza notte S‑52 |

### 5.5 Collegamenti con le altre stazioni

- **Sonar:** tracker S1… → linee di rilevamento e pannello tempo‑rilevamento; misure Doppler e CPA (`09` §3.6) → anelli
  e CPA; annotazioni del sonar → eventi della carta.
- **Radio:** intelligence (già disegnata) e zone R2 nello stesso livello «zone», con il numero di serie del messaggio
  che le ha portate.
- **Navigazione e fondali:** punti nave, ellisse della propria posizione, mosaico misurato (`docs/08` §4); i segni sul
  battello ereditano l'errore di stima.
- **Armi:** anelli d'arma dalle impostazioni del tubo; l'evento «lancio» con la traccia del siluro e del filo.
- **Debriefing:** la carta finale con segni, eventi e, sotto, la verità sovrapponibile.

### 5.6 Restyle VGA/SVGA (PROPOSTA)

**Vincoli:** tela 640×480 a 256 colori indicizzati; la carta occupa la vista (640×360); i colori 0–15 sono EGA
dell'interfaccia; la tavolozza del giorno ha la rampa del mare **96–143** (48 colori), i grigi 144–158, gli effetti
175–198, la rampa del cielo **199–247**, che il sonar già ridefinisce (200–231) quando la sua stazione copre la vista
(`grafica/palette.hpp`, `Palette::imposta`). Alla carta né mare né cielo si vedono: entrambe le rampe sono
riutilizzabili mentre F6 è attiva, purché `Palette::illuminata` non le scurisca per la luce ambiente (o le scurisca
apposta, per la «luce rossa»).

**Ripartizione proposta degli indici a F6:**

| Indici | Uso |
|---|---|
| 0–15 | interfaccia EGA invariata (testo, pannelli, colori dell'originale per le lettere) |
| 96–143 (48) | **acqua: 16 fasce di profondità × 3 livelli d'ombreggiatura** (luce da NO) |
| 199–206 (8) | terra: camoscio → ocra → bruno per altezza, più la linea di costa scura |
| 207–209 (3) | isobate: secondaria, principale, **contorno di sicurezza** (spesso, contrastato) |
| 210–211 (2) | reticolo geografico e griglia XY |
| 212–217 (6) | matite: nero (o blu notte), rosso, blu, verde, giallo, bianco |
| 218–221 (4) | NTDS pieni: blu amico, rosso ostile, giallo ignoto, verde neutrale |
| 222–225 (4) | NTDS attenuati (contatti vecchi o persi, linee che sbiadiscono) |
| 226–228 (3) | zone: tratteggio d'esclusione (magenta), zona d'azione, intelligence |
| 229–231 (3) | magenta di carta (punti di rotta, rosa, aree regolamentate), cursore, evidenza |
| 232–247 (16) | riserva: seconda tavolozza (notte o luce rossa) per scambio rapido |

**Due rampe d'acqua a scelta** (16 fasce ciascuna; le soglie sono STIMA e si adattano al teatro come la tavolozza
dell'anteprima di `build_heightmap.py`, che scala sul 99° percentile della profondità):
- **«VGA 1989» (batimetrica, più scuro = più profondo):** dal ciano chiaro sotto i 20 m al blu notte oltre i 2000 m,
  fasce a passo crescente (10, 20, 30, 50, 75, 100, 150, 200, 300, 400, 500, 750, 1000, 1500, 2000 m e oltre);
  è la firma del gioco originale «con più livelli in VGA» [alta] [M688].
- **«Carta nautica» (più chiaro = più profondo):** bianco o azzurro pallidissimo oltre il contorno profondo, azzurri via
  via più saturi verso il basso fondale, verde intertidale, camoscio la terra [alta] [Chart1]; [alta] [MSC530]. È più
  leggibile per il pericolo e si sposa con il contorno di sicurezza legato alla quota.

**Altri accorgimenti:**
- **isobate** a quadrati marcianti sulla carta d'epoca, con etichetta della profondità ogni circa 200 pixel di linea, in
  piedi/braccia o metri secondo la parte; serie per scala: fine (10–20–50–100–200 m) da vicino, rada (200–500–1000–
  2000 m) da lontano;
- **retinatura ordinata 2×2** fra due fasce vicine per gradienti morbidi senza consumare indici (stile VGA dell'epoca);
- **ombreggiatura** dal gradiente della batimetria (3 livelli), più marcata sulla terra;
- **simboli** 9×9 pixel con tratto di 1 pixel: forme NTDS/APP‑6 (metà inferiore per il subacqueo), lettera o S‑numero
  accanto in piccolo; nel modo classico restano X e lettere dell'originale; vettore di velocità come segmento con
  tacche ai minuti;
- **rosa** attorno al battello (come il «bug» del DRT) con tacche ogni 10° e cifre ogni 30°, accendibile;
- **reticolo** lat/lon (serve esportare nel `.fdl` o in un file accanto il centro della proiezione azimutale
  equidistante, già in `heightmap.json`): ogni 1' a S = 2, ogni 5' o 10' a scale medie, ogni 1° oltre;
- **cornice** da console attorno alla vista nello stile delle stazioni ridisegnate (grigi 144–158), con i pulsanti 1–9
  e le letture (scala, epoca, livelli, ora DTG);
- **annotazioni di carta d'epoca**: «ED», «PA», anno del rilievo, «fondo non rilevato» sulle zone da ETOPO
  (`docs/08` §4–5), in magenta o nero di carta;
- **SVGA**: se una versione futura avrà una tela più grande, la stessa ripartizione funziona; le dimensioni dei simboli
  vanno legate ai pixel della tela, non alla scala.

---

## 6. Lacune note

1. **Console Mk 117 / CCS Mk 1**: comandi, cursori e misure disponibili all'operatore; nome e tipo del tavolo di
   plottaggio della centrale del 688 nel 1989. Nessuna fonte aperta trovata.
2. **Organizzazione ufficiale** del gruppo di tiro dei SSN USA (bill di posto di combattimento, PQS): solo estratti
   ([USNI87], [STS‑e5]); i titoli dei ruoli vanno trattati come plausibili.
3. **Convenzioni colore delle matite grasse** a bordo dei sottomarini: non documentate.
4. **Lato sovietico**: come l'Akkord e le BIUS del Victor III mostravano il quadro tattico; metodi manuali della TMA
   sovietica; simboli dei plot sovietici. Possibili fonti cartacee: Apal'kov; pagine di deepstorm.ru.
5. **688(I) Hunter/Killer**: aspetto esatto della carta e dei simboli; servono schermate da una copia propria o dal
   manuale del gioco.
6. **Tinte e soglie S‑52** (valori RGB dei token di profondità) e serie delle isobate S‑4: lette solo in forma
   indiretta.
7. **Numeri di MSC.192(79)** (precisione di CPA e TCPA): non letti.
8. Il **«dot stack»** è descritto solo da un ufficiale in un articolo del 2001; manca una descrizione tecnica
   dell'interfaccia (scale, colori).

---

## 7. Fonti

### Web (consultate il 24 ottobre 2026)

- `[TFCM]` *Submarine Torpedo Fire Control Manual* (USN, II guerra mondiale), Historic Naval Ships Association / Maritime
  Park: https://legacy.maritime.org/doc/attack/index.php (indice e sezioni sulla stazione di navigazione, estratto)
- `[NSL‑WWII]` «A comparison of Second World War fire control methodology in U.S. tanks and U.S. submarines», NSL
  Archive, 2008: https://archive.navalsubleague.org/2008/a-comparison-of-second-world-war-fire-control-methodology-in-u-s-tanks-and-u-s-submarines (estratto)
- `[RADFIVE]` USN, *Radar Operator's Manual* (RADFIVE), cap. sul DRT, HyperWar: https://ibiblio.org/hyperwar/USN/ref/RADFIVE/RADFIVE-2.html
- `[STU13]` USN, *Division Officer Training*, foglio sulla guerra antisom in centrale (DRT, plotter nord e sud, datum,
  FOC, TDA, griglia XY), FAS: https://man.fas.org/dod-101/navy/docs/swos/stu2/STU13.html
- `[US5029110]` Brevetto US 5 029 110, «Automatic data plotter» (UK Secretary of State for Defence; priorità 1987,
  concesso 1991): https://patents.google.com/patent/US5029110A/en
- `[NSL‑Patton08]` J. Patton, «One knot at one mile equals one degree a minute», NSL Archive, ottobre 2008:
  https://archive.navalsubleague.org/?p=4772
- `[NSL‑Walsh01]` J. Walsh, «New kind of target motion analysis», NSL Archive, gennaio 2001:
  https://archive.navalsubleague.org/?p=3882
- `[NSMRL716]` Naval Submarine Medical Research Laboratory, studio su plot tempo‑rilevamento umani e curve
  matematiche: https://apps.dtic.mil/sti/pdfs/AD0759721.pdf (403; estratto) — stessa sigla di `09`
- `[NPS‑TMA]` Tesi NPS sulle procedure di TMA (Ekelund corretto per il tempo, plot relativo e geografico):
  https://apps.dtic.mil/sti/pdfs/ADA289557.pdf (403; indice da estratto)
- `[NPS‑Prouty07]` J. Prouty, *Displaying uncertainty: a comparison between submarine subject matter experts*, NPS 2007:
  https://apps.dtic.mil/sti/pdfs/ADA467112.pdf (403; estratto)
- `[NPS‑CIC]` Tesi NPS, *A microcomputer based shipboard surface‑subsurface contact plotter system*:
  https://calhoun.nps.edu/entities/publication/627ab876-97d2-438b-b4d6-915368806da1 (estratto)
- `[Haida]` HMCS Haida, «Operations room» (tavolo NC2 con proiettori di bersaglio): https://www.jproc.ca/haida/opsroom.html (estratto)
- `[USNI87]` «Bringing tactics to the surface», *Proceedings*, ottobre 1987:
  https://www.usni.org/magazines/proceedings/1987/october/bringing-tactics-surface (403; estratto)
- `[USNI01]` «Build it and they will come», *Proceedings*, aprile 2001 (Mk 117 e Tomahawk):
  https://www.usni.org/magazines/proceedings/2001/april/build-it-and-they-will-come (estratto)
- `[NSL‑Mk113]` NSL Archive, storia dei sistemi di tiro Mk 101/113/117: https://archive.navalsubleague.org/?p=2022 (estratto)
- `[ARCK‑UYK7]` Naval Undersea Museum, scheda AN/UYK‑7: https://collections.arck-project.org/view/ARCK3D0000000872 (estratto)
- `[STS‑e5]` HonestMOS, scheda di carriera STS (pagina personale, informale): https://www.honestmos.com/playbook/navy/STS/e5 (estratto)
- `[Soton18]` Roberts et al., «Go deeper, go deeper», Univ. di Southampton, 2018:
  https://eprints.soton.ac.uk/419007/ (403; estratto)
- `[MSC530]` IMO, Risoluzione MSC.530(106), *Performance standards for ECDIS*, 7 novembre 2022:
  https://wwwcdn.imo.org/localresources/en/KnowledgeCentre/IndexofIMOResolutions/MSCResolutions/MSC.530(106).pdf
- `[MSC192]` IMO, Risoluzione MSC.192(79), norme per i radar: https://www.transportstyrelsen.se/globalassets/global/sjofart/dokument/resolution_msc_192_79.pdf ; commento sui requisiti CPA: https://transnav.eu/html,1073.html (estratti)
- `[IHO‑DIPWG]` IHO DIPWG, «Safety depth contour and safety depth»:
  https://legacy.iho.int/mtg_docs/com_wg/DIPWG/DIPWG4/TSMAD24-DIPWG4-09.9A_Safety_Depth_Contour_and_Safety_Depth.pdf (estratto)
- `[ESRI‑ENC]` Esri, *EncMarinerSettings* (due o quattro tinte, tavolozze giorno/crepuscolo/notte):
  https://developers.arcgis.com/qt/cpp/api-reference/esri-arcgisruntime-encmarinersettings.html (estratto)
- `[KG90]` R. Kaufmann, S. J. Glavin, «General guidelines for the use of colour on electronic charts», *International
  Hydrographic Review* LXVII(2), 1990: https://journals.lib.unb.ca/index.php/ihr/article/view/23304 (solo sommario)
- `[Chart1]` NOAA/NGA, *U.S. Chart No. 1*, testo sui colori (copia non ufficiale): https://es.scribd.com/document/76651795/ChartNo1-LoRes (estratto)
- `[NG‑VMS]` ECDIS‑N e VMS sui sottomarini: https://www.shipandoffshore.net/news/news/northrop-grumman-submarine-navigation-system-upgrade-approved-by-us-navy.html ;
  https://www.edrmagazine.eu/us-navy-surface-ships-and-submarines-to-be-equipped-with-new-system ;
  https://www.militaryaerospace.com/rf-analog/article/16713312/navy-will-convert-fleet-to-paperless-navigation (estratti)
- `[APP6‑wiki]` Wikipedia, *NATO Joint Military Symbology*: https://en.wikipedia.org/wiki/NATO_Joint_Military_Symbology
- `[NTDS‑NPS]` NPS Savage, esempi di simboli NTDS (colori per affiliazione, modificatore «?»):
  https://savage.nps.edu/Savage/Tools/Symbology/NTDSExternProtoExamples.html
- `[NATO‑CA]` *NATO Map Symbol (Canadian) Programmed Instruction Package*, 2000:
  https://army.ca/wiki/images/8/80/NATO_Map_Symbol_(Canadian)_Programmed_Instruction_Package_Jan_2000.pdf (estratto)
- `[RU705]` Wikipedia russa, *Подводные лодки проекта 705* (BIUS Akkord, equipaggio):
  https://ru.wikipedia.org/wiki/Подводные_лодки_проекта_705
- `[GR97]` Game Revolution, recensione di *688(I) Hunter/Killer*, 1 settembre 1997: https://www.gamerevolution.com/?p=33088
- `[WP688I]` Wikipedia, *688(I) Hunter/Killer*: https://en.wikipedia.org/wiki/688(I)_Hunter/Killer
- `[WP‑parallel]` Wikipedia, *Parallel rulers*: https://en.wikipedia.org/wiki/Parallel_rulers (estratto)
- `[Starpath]` Starpath, strumenti di plottaggio: https://www.starpath.com/tools.htm (estratto, fonte commerciale)
- `[USNI1904]` «Navigator's protractor», *Proceedings*, gennaio 1904:
  https://www.usni.org/magazines/proceedings/1904/january/navigators-protractor (estratto)
- `[BOW]` N. Bowditch, *The American Practical Navigator* (NGA Pub. 9), capitoli sugli strumenti e sulla stima;
  testo non riletto in questa sessione: prassi standard.

### Interni

- `[M688]` Manuale del gioco originale, `docs/originale/manuale_EN.txt` («Top‑down map», «Map Display Control»).
- `[NUM‑ESE]` `docs/originale/numeri_eseguibile.md` §3.2 (carta dell'originale dal disassemblato).
- `docs/11_comandi_e_stazioni.md` §8–§11; `docs/08_fondali_e_navigazione.md`; `engine/app/carta.cpp`;
  `engine/nucleo/include/nucleo/{carta_giocatore,tma,fondale,mappa_tasti}.hpp`; `engine/grafica/include/grafica/palette.hpp`;
  `data/teatri/<teatro>/heightmap.json`; `tools/pipeline/build_heightmap.py`.
- Dossier `02` §5.1, `06`, `09` §2.7–§3.10, `10` §4.1 e §4.3.
