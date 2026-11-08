# 21-D — Meteo e stato del mare: briefing, grafica, animazioni

Fa parte del dossier 21, su meteo e stato del mare dinamici e realistici, trattati come **un unico sistema**: il vento genera il mare, e il mare governa sensori, moto delle navi e vista.

Questa nota copre cinque temi:

- **(A)** come il meteo arrivava ai comandanti (1985-1990) e come lo mostravano i giochi;
- **(A-bis)** il meteo via radio ai sottomarini;
- **(B)** come appare il meteo dal periscopio e in mare, per la vista 3D→VGA;
- **(B-bis)** i fulmini;
- **(B6)** lo stato del mare per le **animazioni** del kit di sprite 2D/VGA: creste, moto delle navi, elicotteri, periscopio che va sotto.

**Etichette.**

- Affidabilità: **alta** = manuale ufficiale o documento primario; **media** = fonte secondaria seria; **bassa** = forum, gioco, aneddoto.
- `[GIOCO]` = proposta di design per il remake.
- `STIMA` = numero inventato o ricavato da noi.
- `n.v.` = non verificato o non trovato.

Le sigle tra parentesi quadre rimandano all'elenco delle fonti in fondo (§F).

Nota di metodo: ho letto direttamente l'OCR dei manuali di Gunship, F-19, Red Storm Rising (ed. tedesca Amiga), 688 Attack Sub, Harpoon II, Dangerous Waters e Silent Hunter 5, presi da archive.org. Dove cito un manuale l'affidabilità è quindi **alta** sul *gioco*, non sulla realtà.

---

## 0. In sintesi

1. **Prodotti reali USA anni '70-'80** `[AG76]` `[BOW]` `[WPFN]`. Il manuale *Aerographer's Mate 3 & 2* del 1976 conferma i bollettini radio regolari alla flotta:
   - avvisi di vento e di **mare grosso** (*high sea warnings*);
   - **previsioni per le aree operative di flotta** (le «OPAREA forecast»);
   - carte analizzate e prognostiche, meteo e oceanografiche;
   - supporto su misura **a richiesta** delle navi.

   L'ossatura era:
   - la FNOC (Monterey) faceva i modelli numerici: atmosfera, onde, strato misto oceanico dal 1981;
   - i centri meteo-oceanografici di flotta facevano i prodotti su misura:
     - **OTSR** (rotta ottima) con le **raccomandazioni di deviazione**;
     - **WEAX** (previsione di meteo e stato del mare lungo la rotta);
     - gli avvisi per vento: *small craft* 18-33 kn, *gale* 34-47 kn, temporale T1;
     - le condizioni di prontezza per i cicloni tropicali, da **COR 5 a COR 1**.
2. **Codici aviazione.** Negli USA del 1985-1990 **non** si usavano METAR e TAF: si usavano i formati **SA** (osservazione oraria) e **FT** (previsione d'aeroporto). Il passaggio domestico a METAR/TAF avvenne il **1° luglio 1996** `[AVW]` (alta-media). Fuori dagli USA, e nei messaggi internazionali, il METAR esisteva già.
3. **Meteo al sottomarino in immersione.** È plausibile ma **n.v.** nei dettagli che arrivasse nel broadcast VLF/LF a 50 baud: sono testi brevi. Un SSN in EMCON non chiede previsioni. Le chiede solo in trasferimento in superficie o fuori dall'EMCON. Il servizio dedicato **SUBWEAX** (24/7 alla FNMOC) è del **2008**, quindi fuori epoca `[WPFN]`.
4. **Giochi d'epoca.** Il meteo era quasi assente o molto astratto:
   - **688 Attack Sub**: nessuna voce meteo nel manuale `[M688]`;
   - **Red Storm Rising**: «rumore di superficie» per tipo di ghiaccio, nessun meteo `[MRSR]`;
   - **F-19**: solo giorno e notte, con le telecamere che vedono meno di notte `[MF19]`;
   - **Gunship**: vento, temperatura e peso massimo al decollo nel briefing `[MGS]`;
   - **Silent Service II**: livelli di visibilità e «murk» notturno `[RSS2]`.
5. **Il modello di riferimento è Harpoon II** `[MH2]`:
   - celle di alta e bassa pressione che si muovono;
   - fronti che creano nubi e precipitazioni (3 livelli ciascuno di nebbia, pioggia e neve);
   - vento che genera lo stato del mare 0-9;
   - **ogni punto di stato del mare toglie circa il 15% alla portata del sonar passivo**;
   - il sonar attivo è «quasi inutile» sopra lo stato del mare 5.
6. **Periscopio con mare grosso.** Le fonti reali dicono che:
   - le onde coprono la testa e rendono l'osservazione impossibile; si risale in superficie o si va a fondo `[BEN]` `[DEPTH]`;
   - in mare di poppa la quota periscopica può diventare ingovernabile `[IMAR]`;
   - le guarnizioni che perdono inzuppano chi guarda (CDR Fraser, «GET ME DOWN!») `[NSLF]`.

   Nei giochi: Silent Hunter III disegna l'**acqua che cola dalla lente** quando il periscopio esce `[CXW]`; Silent Hunter 5 ha **gocce di pioggia che sbavano la vista** `[GS5]`; in Cold Waters la «zona scura in basso» è **acqua sopra la lente** `[CWST]`.
7. **Fulmini.** In media una scarica ha **3-4 colpi di ritorno** (media 3,67; un terzo è a colpo singolo). Fra un colpo e l'altro passano **~57 ms** (media geometrica). Le scariche multiple durano in tutto **~370 ms** `[NHESS]` `[ERR]`. Di notte il lampo si vede a **~100 miglia**, il tuono si sente a ~10 miglia (NWS) `[NWSHL]`. In mare aperto i fulmini sono più rari che sulla terra `[RMETS]`.
8. **Mare e animazioni** (B6). Mare da vento con creste aguzze e onda lunga con cime arrotondate vanno separati `[AG76]`. I criteri visivi Beaufort sono quelli RMetS: cavalloni sparsi a 3, strisce di schiuma a 7, spruzzaglia a 8, mare bianco a 10 `[RMB]`. Per il moto delle navi, periodo di rollio T ≈ C·B/√GM `[JNAV]` e tabella per classe (STIMA). Per gli elicotteri: anello di spruzzo, muso al vento, cavo inclinato; il limite di vento dalle flashcard è bassa. Per i giochi passati: «waveriding» in DW `[MDW]`, acqua sulla lente in SH3 e Cold Waters, solo strati sulla mappa in Harpoon II.
9. **Immagini di riferimento.** La §B5 ne elenca 30 su Commons con licenza. La serie Beaufort NWS è di pubblico dominio, ma è stata fotografata sui Grandi Laghi.

---

## A. Briefing e previsioni

### A1. Prodotti meteo-oceanografici della US Navy, 1985-1990

**Organizzazione** (alta/media)

- **Fleet Numerical Oceanography Center** (FLENUMOCEANCEN, Monterey). Così si chiamò dal 1979 al 1993, poi FNMOC. Era il calcolatore centrale della *Naval Environmental Data Network*: analisi e previsioni numeriche per le forze navali `[WPFN]` `[ONR87]`.
  - Nel **1981** installò il primo sistema operativo al mondo di **previsione dello strato misto oceanico**, «per la guerra antisommergibile» `[ONR87]`. È questo a legare meteo, vento, termoclino e portata del sonar.
  - Aveva i modelli d'onda spettrali: SWOM negli anni '70, poi GSOWM; la previsione globale d'onda arrivava a 72 h `[APL]` `[WWM]`.
- **NAVOCEANCOM** (Naval Oceanography Command) dirigeva i centri regionali e le **Fleet Weather Centrals / Facilities**:
  - a Norfolk, Rota, Pearl Harbor, Guam (con il JTWC), Suitland;
  - dal 1976 c'era il **Joint Ice Center** Navy-NOAA a Suitland, per le carte del ghiaccio `[ICE]`.

  Le funzioni sono documentate; l'elenco esatto dei centri nel 1989 è **n.v.**
- **La struttura nel 1976, prima di NAVOCEANCOM**, secondo il *Aerographer's Mate 3 & 2* (NAVEDTRA, 1976) `[AG76]` (alta):
  - la **FLENUMWEACEN** di Monterey era il centro dei calcoli e il nodo della rete NEDN;
  - **quattro Fleet Weather Centrals**: Guam, Pearl Harbor, Norfolk, Rota. Ognuna seguiva un bacino oceanico e produceva i **bollettini ambientali di flotta** e il supporto su richiesta;
  - la **FLEWEAFAC Suitland** si occupava delle regioni polari e del **ghiaccio marino**.

  Bollettini regolari via CW, RATT (telescrivente radio) e radiofax:
  1. avvisi di vento (aree oceaniche);
  2. **avvisi di mare grosso**;
  3. *small craft warnings*;
  4. avvisi di mareggiata;
  5. **«Fleet operating area forecasts»**;
  6. analisi e carte prognostiche, meteo e oceanografiche;
  7. ricaduta nucleare;
  8. meteo per l'aviazione;
  9. maltempo locale;
  10. foto satellitari delle nubi.

  Lo stesso manuale dice che le carte della **SST** servono per le **previsioni della portata del sonar** per l'ASW e per prevedere la **nebbia** (differenza di temperatura aria-mare). Dal 1978-79 i centri passano sotto il **Naval Oceanography Command** e cambiano nome: le date esatte del cambio sono n.v.; i nomi FLEWEACEN/NAVOCEANCOMCEN sono media.
- **TESS** (Tactical Environmental Support System), in sviluppo a metà anni '80: doveva «prevedere l'influenza dell'ambiente sulle prestazioni di sensori e armi» `[TRB85]`. A bordo delle portaerei esisteva una versione di TESS; ma le **portate sonar previste** (ESR, *effective sonar range*) erano un prodotto standard dei briefing ASW `[DTIC-ESR]` (media).

**Prodotti per navi in mare** (media; le definizioni vengono da materiale di studio moderno della Navy, ma sono coerenti con Bowditch)

| Prodotto | Che cos'è | Note d'epoca | Fonte |
|---|---|---|---|
| **OTSR** (Optimum Track Ship Routing) | Rotta consigliata sulla base delle previsioni a 3-10 giorni e dei limiti di vento e mare della nave. Va chiesta **72 h prima** di partire | Nata nel **1958** (fonte commerciale, media-bassa); negli anni '50 la Navy già instradava navi evitando il maltempo | `[BOW]` `[OWS]` `[BRS]` |
| **Route Surveillance** | Conferma che la nave è seguita per tutto il viaggio | Si attiva dopo il MOVREP (rapporto di movimento) | `[BRS]` |
| **Divert Recommendation** | Messaggio di deviazione quando vento o mare previsti superano i limiti dell'unità | È il vero «avviso di burrasca» personalizzato | `[BRS]` |
| **WEAX** (Enroute Weather Forecast) | «Previsione di meteo e **stato del mare**» su misura lungo la rotta, in un punto o in un porto | Il termine esisteva negli anni '80: **n.v.** (probabile) | `[BRS]` |
| **Small Craft / Gale / Storm Warning** | Vento sostenuto: 18-33 kn per *small craft*, 34-47 kn per *gale*; *storm* da 48 kn (STIMA basata sulla convenzione NWS; la pagina citata non la riporta) | Le soglie coincidono con quelle NWS | `[BRS]` |
| **Thunderstorm T1 / Severe T1** | Temporale entro 10 NM o previsto entro 1 h; il «severe» aggiunge raffiche >50 kn, grandine >¾", trombe | Utile anche per i fulmini (§B-bis) | `[BRS]` |
| **Tropical Cyclone COR 5…1** | Venti distruttivi possibili entro 96, 72, 48, 24 o 12 h | Nel Pacifico occidentale li emette la FWC/JTWC di Guam | `[BRS]` `[JTWC]` |
| **Carte del ghiaccio / ice forecasts** | Per i sottomarini contavano distribuzione, dimensione e **pescaggio delle creste di pressione** | Previsioni del ghiaccio per i sottomarini dal 1957; modello dinamico-termodinamico sui dati FNOC negli anni '80 | `[ICE]` `[ARC]` |
| **Critical Factors Chart (CFC)** | Riassunto in una slide «soprattutto per la comunità sommergibilisti» | Moderno: per gli anni '80 **n.v.** | `[BRS]` |

**Codici e formati del periodo** (alta-media)

- **USA domestico fino al 30/6/1996**: osservazioni **SA** (Surface Aviation, orarie) e previsioni **FT** (terminal forecast). METAR/TAF solo per lo scambio internazionale: la conversione SA→METAR internazionale fu completata nel luglio 1993 `[AVW]`. In un briefing di NAS del 1989 sarebbe dunque comparso un SA, non un METAR.
- **Europa e NATO** usavano già METAR e TAF (codici OMM).
- **Stato del mare**: codice OMM 3700, gradi 0-9, con altezza d'onda (vedi tabella in A4) `[SSC]`.
- **Aviazione militare**: il modulo **DD 175-1** (*Flight Weather Briefing*) riporta:
  - dati di decollo: temperatura, punto di rugiada, altitudine di densità, vento al suolo, avvisi locali;
  - venti e temperature in quota;
  - nubi al livello di volo;
  - **oscuramenti della visibilità**;
  - **plafond minimo**, top massimo delle nubi, quota dello zero termico;
  - carte di temporali, turbolenza e ghiaccio;
  - previsioni per destinazione e alternati (vento, plafond, visibilità) `[DD175]`.

  Alba, tramonto e illuminazione lunare nel DD 175-1: **n.v.** Nei bollettini militari moderni ci sono (es. Space Force: alba, BMNT, sorgere della luna accanto al blocco stato del mare) `[PSFB]`.

### A2. Il meteo via radio ai sottomarini, 1985-1990 (aggiunta richiesta)

Vedi anche `08_comunicazioni_radio.md` per stazioni, frequenze e 50 baud.

**Cosa è documentato**

- Il VLF/LF arriva al battello in immersione (antenna filare galleggiante o boa) ed è **solo terra→battello** `[FASVLF]` (alta).
- Nel 1976 il meteo per la flotta passava «via normal Navy Communications channels», a orari fissi; il supporto su misura c'era «upon request» al centro competente `[AG76]` (alta). Che lo stesso valesse per il broadcast VLF ai sottomarini **non è detto** (n.v.).
- Il broadcast moderno è descritto come una sequenza continua di messaggi in ordine di priorità, che normalmente dura due ore `[FASVLF]` (media). Il battello lo copia a orari prefissati.
- Che il broadcast USA degli anni '80 contenesse **bollettini meteo, avvisi di burrasca o previsioni del ghiaccio** è **n.v.**: non ho trovato fonti primarie.
  - Plausibilità: alta per gli avvisi di ciclone tropicale e per le carte del ghiaccio nelle operazioni artiche, perché sono sicurezza della navigazione.
  - I messaggi meteo sono testi brevi, compatibili con 50 baud.
- **SUBWEAX**, il supporto meteo 24/7 dedicato ai sottomarini, fu assegnato alla FNMOC solo nel **2008** `[WPFN]` (media). Prima esisteva un supporto, ma meno centralizzato (n.v. nei dettagli).
- Le **Fleet Weather Centers** oggi forniscono l'OTSR a unità «di superficie e subacquee» `[BWIRE]` (media, moderno). Per gli SSN in trasferimento negli anni '80 è plausibile ma n.v.
- Prima del maltempo esistono forme di «uscita in massa» dai porti e piani di prontezza (COR) `[CNIC]`. Un battello in porto li riceve come una nave qualsiasi.

**Il battello poteva chiedere una previsione trasmettendo?**

- In **EMCON** (pattugliamento, sorveglianza, guerra): **no**. Trasmettere a quota periscopica è l'unico momento in cui si viene localizzati, e non lo si fa per una previsione. Questo vale per il gioco e per la logica di `08_comunicazioni_radio.md` (alta come principio).
- **Quando sarebbe accettabile** (STIMA ragionata, n.v. come procedura):
  1. in **trasferimento in superficie** in uscita o in rientro, quando si trasmettono comunque i MOVREP e il battello è sotto OTSR;
  2. in **esercitazione** in tempo di pace;
  3. **insieme a un'altra trasmissione obbligatoria** (rapporto di situazione, contatto), aggiungendo la richiesta allo stesso «burst». Negli anni '80 c'era il satellitare **SSIXS** (sistema di scambio dati via satellite per sottomarini), che riduceva l'esposizione. Era in uso, ma i dettagli sono n.v. qui (media);
  4. per **emergenza o avaria**: un battello in superficie con l'uomo in plancia ha bisogno di sapere quanto peggiorerà il mare.
- **Navi di superficie**: OTSR, Route Surveillance, **Divert Recommendation**, WEAX su richiesta, avvisi *gale* e *storm*. Per i mercantili c'erano anche i bollettini radiofax HF della NWS e della Navy `[NWSFAX]` (media).

**Lato sovietico**

- In tempo di guerra la meteorologia del Nord era militarizzata: l'ufficio regionale di Murmansk divenne l'**Управление гидрометслужбы Северного флота**, con i previsori **presso lo stato maggiore della flotta** `[AARI]` (media). Per gli anni '80 è plausibile la stessa struttura, ma **n.v.**
- Dal 1972 l'organo centrale per navigazione e oceanografia è il **ГУНиО МО**; la voce di Wikipedia cita il supporto «гидрометеорологическое» tra le sue funzioni, senza dettagli `[GUNIO]` (media).
- Ai sottomarini si comunicava per **sedute di collegamento programmate** (сеансы связи). La stazione ELF «Зевс» manda solo segnali semplici, come «emergi per comunicare» `[IZ]` (media-bassa).
- Che i broadcast circolari ai battelli contenessero il meteo è **n.v.**
- [GIOCO] Per la parte sovietica si usino gli stessi meccanismi, con nomi russi: «метеосводка», «штормовое предупреждение», «прогноз ГАУ (гидроакустических условий)». Il prodotto acustico va presentato come «прогноз дальности обнаружения» (previsione della portata di scoperta).

### A3. Cosa c'era in un briefing del comandante

Gli elementi vengono dalle fonti sopra. Il modello di messaggio sotto è **inventato** (STIMA), in stile d'epoca, ed è da usare nel gioco.

Elementi tipici (media, ricomposti da DD 175-1, WEAX, OTSR e bollettini militari):

1. **Situazione sinottica**: sistemi di pressione, fronti e loro movimento.
2. **Vento**: direzione e intensità, raffiche, tendenza.
3. **Mare**: stato del mare in codice OMM, altezza e periodo dell'onda, **onda lunga** (direzione e altezza).
4. **Visibilità**, con nebbia, foschia, precipitazioni.
5. **Plafond e nubi**: importante per aerei ASW ed elicotteri, ed entra in gioco per gli aerei nemici.
6. **Precipitazioni**: tipo e intensità; temporali con fulmini.
7. **Ghiaccio**:
   - limite del pack, concentrazione, creste di pressione;
   - per le navi, il **ghiaccio sulle sovrastrutture** per spruzzi gelati.
8. **Astronomia**: alba, tramonto, crepuscoli nautici (BMNT/EENT, inizio del crepuscolo nautico al mattino e fine alla sera), sorgere e tramonto della luna, **frazione illuminata**.
9. **Oceanografia e acustica**:
   - SST (temperatura del mare in superficie), profondità dello strato (MLD), presenza del canale di superficie;
   - convergenza e zone d'ombra;
   - **portate di scoperta previste** per i sonar propri e nemici, per tipo di bersaglio;
   - **rumore ambiente** (dipende da vento e mare: vedi Harpoon II e Dangerous Waters in A4).
10. **Tendenza** a 12/24/48 h e avvisi in vigore (*gale*, T1, COR).

**Esempio di messaggio** [GIOCO] (formato inventato, maiuscole e abbreviazioni d'epoca):

```
FM FLEWEACEN NORFOLK
TO USS CHICAGO
SUBJ: WEAX/ACOUSTIC SUMMARY OPAREA NORWEGIAN SEA
1. SYNOPSIS: LOW 988MB 67N 02E MOVG NE 15KT. COLD FRONT TRAILS SW.
2. VALID 121200Z-131200Z:
   A. WIND: SW 25-30KT GUSTS 40 BCMG W 35KT AFT FROPA 130300Z.
   B. SEAS: STATE 5 (8-10FT) INCR STATE 6 (12-15FT). SWELL W 10FT.
   C. VIS: 3-5NM IN RAIN, LCL 1NM IN SHWRS. SNOW SHWRS AFT FROPA.
   D. CIG: 800-1500FT OVC.
   E. ICE: NIL. ICING ON SUPERSTRUCTURE POSS AFT 130600Z.
3. ASTRO 12 DEC: SR 0956Z SS 1258Z. MOON 23 PCT, MR 0311Z.
4. ACOUSTIC: SLD 180FT. SFC DUCT FAIR. AMBIENT NOISE HIGH (WIND).
   PREDICTED PASSIVE DET RANGE VS SSN: 4-6KYD LAYER, 2-3KYD XLAYER.
5. GALE WARNING IN EFFECT.
```

Tutti i numeri qui sono STIMA, da tarare col dossier acustico 02.

**Tabella stato del mare (codice OMM 3700)** `[SSC]` (alta per i valori):

| Codice | Descrizione | Onda (m) | Beaufort indicativo (STIMA, la relazione non è fissa) |
|---|---|---|---|
| 0 | calmo, vitreo | 0 | 0 |
| 1 | calmo, increspato | 0-0,1 | 1 |
| 2 | poco mosso | 0,1-0,5 | 2-3 |
| 3 | mosso | 0,5-1,25 | 4 |
| 4 | molto mosso | 1,25-2,5 | 5 |
| 5 | agitato | 2,5-4 | 6-7 |
| 6 | molto agitato | 4-6 | 8-9 |
| 7 | grosso | 6-9 | 10 |
| 8 | molto grosso | 9-14 | 11 |
| 9 | tempestoso | >14 | 12 |

### A4. Il meteo nei simulatori: briefing e partita

| Gioco | Meteo nel briefing | Meteo in partita, effetti | Cambia durante la missione? | Fonte (affidabilità) |
|---|---|---|---|---|
| **688 Attack Sub** (EA, 1989) | Nessuna voce meteo nel manuale | Nessun meteo: periscopio e sonar non dipendono da vento o mare. Il livello BEGINNER rende il periscopio «meno visibile al radar» | No | `[M688]` (alta). È il punto da cui partiamo |
| **Red Storm Rising** (MicroProse, 1988) | Scelta del luogo dello scontro: mare aperto, ghiaccio alla deriva, sotto il pack, bassi fondali | «Rumore di superficie»: il **pack è il più silenzioso**, mare aperto e bassi fondali sono nella media, il **ghiaccio alla deriva è molto rumoroso**. La vista dipende dall'**altezza dell'albero** (curvatura terrestre), con una tabella quota/portata | No | `[MRSR]` (alta) |
| **Gunship** (MicroProse, 1986) | Il briefing riporta **vento** («5 kts from East») e **temperatura**. Opzione realtà: meteo *Easy* (niente vento, temperatura fissa) o *Realistic* («highly variable weather») | Il peso massimo al decollo dipende da regione e meteo: con il caldo si porta meno. Il laser rende meno con pioggia, neve, nebbia e fumo. Il vento conta in decollo e atterraggio | n.v. | `[MGS]` (alta) |
| **F-19 Stealth Fighter** (1988) | Nessun meteo nel briefing: bersagli, radar, SAM, basi, piano di volo | Solo giorno e notte: la telecamera è TV di giorno e **FLIR di notte**, portata **80-100 km, meno di notte** | No | `[MF19]` (alta) |
| **F-117A** (1991) | n.v. (manuale PDF troppo grande da analizzare) | n.v. | n.v. | — |
| **Silent Service II** (1990) | Il **Captain's Log** mostra ora del giorno, **visibilità** e fondale | Livelli di visibilità «realistically depicted»; di notte si scruta «through the murk» | n.v. | `[RSS2]` (media, recensione d'epoca) |
| **Harpoon (Classic)** | Finestra *Weather Report* con ora del giorno | Centri di tempesta casuali a 17-23 kn, durata ~72 h, rivalutati ogni 12 h, applicati ogni 5 minuti di gioco:<br>• raggio ~330 NM, con fasce a 25, 50, 100 e 150 NM;<br>• precipitazione 0-3: degrada la vista, il radar, l'IR e (solo al livello 3) l'ESM;<br>• lo stato del mare entra nel sonar | **Sì** | `[HGF]` (bassa, forum 2005) |
| **Harpoon II** (1994-96) | Mappa con strati meteo a tacche colorate: vento e stato del mare, nubi (sereno, sparso, parzialmente nuvoloso, coperto), precipitazioni (**3 livelli ciascuno di nebbia, pioggia e neve**) | Celle di alta e bassa pressione generate a inizio scenario sui valori climatici della regione e della stagione:<br>• le celle si muovono; i bordi danno nubi e precipitazioni; il vento dà lo stato del mare 0-9;<br>• il **radar e l'occhio** dipendono da precipitazioni e visibilità;<br>• **ogni punto di stato del mare: −15% circa di portata passiva**, di più alle alte frequenze;<br>• attivo «quasi inutile» sopra SS5;<br>• la portata di identificazione visiva dipende da meteo, ora, dimensione e quota | **Sì** (celle in movimento) | `[MH2]` (alta) |
| **Dangerous Waters** (2005) | Lo stato del mare è fissato nell'editor della missione; esistono «regioni di vento» e correnti | L'onda di uno stato del mare alto **genera rumore di fondo**. Il modello di rilevamento usa anche lo stato del mare. Il tracciatore VLAD è consigliato quando il mare alza il rumore. I modelli 3D «cavalcano» le onde | Non dinamico (n.v.) | `[MDW]` (alta) |
| **Sub Command / Fleet Command** (Sonalysts) | n.v. | n.v. (stesso motore di DW, probabile stato del mare nell'editor) | n.v. | — |
| **Cold Waters** (2017) | n.v. | Rumore ambiente dovuto a **stato del mare, pioggia, bassi fondali e ghiaccio** (intervista allo sviluppatore). Al periscopio la parte scura in basso è **acqua sulla lente**; con mare grosso a 50 ft si vede poco e i giocatori salgono a 45 ft | n.v. | `[RPS]` (media), `[CWST]` (bassa) |
| **Silent Hunter III** (2005) | Il rapporto meteo lo dà l'ufficiale di guardia (dettaglio n.v.) | Pioggia e nebbia fino a **~400 m di visibilità**: è il miglior riparo dagli aerei. Il mare calmo rende il periscopio più visibile. Opzione realismo sulla **stabilità del periscopio col mare grosso**. Animazione dell'**acqua che cola dalla lente** alzando il periscopio. Parametri di tempesta per battello in `uboat.cfg` (*StormConditions*). GWX ritocca visibilità e scoperta | **Sì** | `[MOBY3]` (media); `[HGW]` `[CXW]` `[Q23]` (bassa) |
| **Silent Hunter 5** (2010) | n.v. | **Gocce di pioggia che sbavano la vista** al periscopio. Cannone e mitragliere «solo con bel tempo»: col mare grosso l'equipaggio può finire in mare. Nebbia volumetrica e nubi | **Sì** | `[GS5]` (media), `[MSH5]` (alta) |
| **Falcon 4.0 / BMS** | La mappa di briefing mostra (strumenti di terzi) tipo di meteo, base delle nubi, visibilità, pressione e temperatura in ogni punto | Meteo per celle di circa 1°×1°, con condizioni, venti e pressione. Si possono importare dati reali NOAA GFS (F4Wx, 16 giorni). Gli sviluppatori ammettono che il meteo cambiava **troppo in fretta** e che la pressione saltava al confine fra regioni | **Sì** | `[FBMS]` `[FLW]` (bassa) |

**Cosa ha funzionato, e cosa piaceva** (sintesi critica; bassa-media, da forum e recensioni)

- Il **meteo come risorsa tattica** è il tratto più apprezzato: in Silent Hunter III ci si nasconde nella nebbia e nella pioggia per sfuggire agli aerei `[Q23]`. Il meteo diventa una decisione («aspetto il fronte?»), non decorazione.
- **Harpoon II** è il modello più leggibile: si vedono strati meteo sulla mappa e una regola semplice (−15% per punto di stato del mare). Il giocatore capisce *perché* il sonar sente meno.
- **Gunship** mostra l'idea più MicroProse: **due righe nel briefing** (vento e temperatura) con una conseguenza concreta (il peso al decollo), e la possibilità di rinunciare («sick call»).
- I **difetti segnalati**:
  - meteo che cambia troppo in fretta o a scalini (Falcon BMS `[FBMS]`);
  - meteo che impedisce di giocare (in Cold Waters il periscopio è inservibile, in SH il cannone non si usa);
  - in Cold Waters, effetti visivi che dipendono da limiti di rendering e non dalla fisica `[CWST]`.

**Proposte** [GIOCO]

1. Il briefing ha un **pannello «METEO E MARE»** su una sola schermata, in stile minicartolina F-19, con 6 righe:
   - vento;
   - stato del mare e onda lunga;
   - visibilità e precipitazione;
   - plafond;
   - astronomia (luna in %);
   - **acustica**: profondità dello strato e portate previste come una barretta per tipo di bersaglio.

   Sotto c'è la **tendenza**, con una freccia e l'ora: per esempio «PEGGIORA 0300Z». Vedi anche la memoria «Briefing e debriefing»: un evento per volta.
2. **Messaggio radio in missione**: se il battello copia il broadcast all'ora prevista, arriva un **aggiornamento WEAX o un GALE WARNING**, solo con evento probabile e solo in finestra di copia. Il meteo cambia così in modo narrativo e plausibile.
3. Gli **effetti** sono pochi e leggibili, alla Harpoon II:
   - lo stato del mare alza il rumore ambiente e taglia la portata passiva (circa −15% per punto, da tarare);
   - il sonar attivo degrada fortemente sopra SS5;
   - visibilità e precipitazioni tagliano la portata visiva al periscopio e il radar nemico sul periscopio. Il **mare mosso nasconde la piuma**, quello calmo la rivela `[Q23]`;
   - il mare grosso rende difficile tenere la quota periscopica (§B1).
4. Il meteo **evolve lentamente**: un fronte che passa in 6-12 h di gioco (STIMA). Si evita l'errore di Falcon.

---

## B. Grafica: periscopio e mare

### B1. Il periscopio col mare grosso: cosa raccontano le fonti

- **Le onde sulla testa.** In un racconto sul tempo di guerra, un battello britannico in burrasca da nord non può restare a quota periscopica, perché le onde che lavano il periscopio rendono impossibile osservare: naviga in superficie `[BEN]` (media-bassa, libro del 1943).
- **Tenere la quota.** Gli studi della US Navy sul controllo della quota periscopica dicono che:
  - le forze dovute alle onde sono molto grandi;
  - i segnali di profondità e assetto sono rumorosi;
  - l'obiettivo è stare entro ~1 ft dalla quota e ½° dall'assetto ordinati `[DEPTH]` (media).

  Vicino alla superficie la forza verticale tende verso l'alto, cioè verso l'**affioramento** (*broaching*) `[UTAS]` (media). In mare grosso, specie **di poppa**, il controllo a quota periscopica può diventare impossibile e si scende finché il tempo migliora `[IMAR]` (media).

  Tre criteri per giudicare un battello fra le onde: **immersione della testa del periscopio, affioramento della vela, collisione** `[NPS]` (media).
- **Acqua addosso a chi guarda.** CDR G. K. Fraser (NSL, 1995) racconta di un periscopio con le guarnizioni che perdevano. L'acqua «inevitably cascaded» sull'ufficiale a ogni uso a quota periscopica, e il comandante indossava una cerata col cappuccio prima di alzarlo. In avvicinamento si alzava il periscopio quanto bastava per vedere il bersaglio «sopra le creste» `[NSLF]` (media). È un dettaglio sonoro e visivo per la plancia, non per l'oculare.
- **Lunghezza fissa.** Sul Type 18 del 688 l'esposizione si regola cambiando quota, non abbassando il tubo: la carenatura fa da cuscinetto superiore. È un'affermazione di forum `[CWST]` (bassa), coerente con `20_periscopi.md`.
- **Finestre riscaldate e antiappannamento.** Il CK34 britannico ha la finestra riscaldata (vedi `20_periscopi.md`, Jane's). Per il Type 18 e per i periscopi sovietici è **n.v.** Esistono brevetti di lastre riscaldanti per asciugare dall'interno vetri appannati, su periscopi non navali `[PATDE]`. Un tubo pressurizzato ad azoto secco contro l'appannamento interno è **n.v.** (probabile).
- **Vibrazioni.** Il periscopio di un battello in moto vibra, per la corsa e per le onde; a velocità non bassissima l'osservazione diventa impossibile (brevetto olandese anni '60) `[PATNL]` (media).
- **Il gergo** «dunking the scope» o «taking water over the scope»: **n.v.** come formula attestata. Il concetto è documentato (testa sommersa = periscopio cieco) `[WWI]`.

**Come appare (sintesi per l'artista).** Le prime quattro voci sono deduzioni coerenti con le fonti (media); le ultime due sono STIMA.

1. A ogni cresta che passa la **metà inferiore del campo diventa verde-grigia, opaca, con bolle**, poi l'acqua scende: si «riaffiora».
2. Dopo l'immersione della testa restano **colature verticali** sul vetro e **gocce** che rifrangono: punti chiari con un bordo scuro.
3. Gli **spruzzi** delle creste vicine passano come macchie bianche sfocate.
4. Con la pioggia si hanno **gocce che si allargano e sbavano** (SH5 `[GS5]`); con la neve, fiocchi che si posano sul vetro e si sciolgono.
5. Il **rollio e il beccheggio** fanno oscillare l'orizzonte nel reticolo. La frequenza dipende dal periodo d'onda: 6-12 s con mare 4-6 (STIMA).
6. Di notte e con la nebbia l'orizzonte **sparisce**: il cielo e il mare diventano un'unica rampa di grigi.

**Proposte** [GIOCO]

- **Ciclo d'immersione della testa.** Con stato del mare ≥4 e quota ≥ periscopica, la probabilità che la testa vada sotto per ogni onda cresce con lo stato del mare e con la quota. Sotto si passa a un overlay «subacqueo» per 0,3-1,5 s (STIMA). Il giocatore può **salire di qualche piede** per vedere meglio, ma aumenta la piuma, il rischio di affioramento e la scoperta. È la decisione chiave.
- **Overlay su VGA 320×200** (in palette, senza alpha):
  1. Una **maschera d'acqua** che sale dal basso, in 4-6 fotogrammi, con 2-3 indici di una rampa verde-acqua e dithering ordinato 4×4 sul confine.
  2. Una **tabella di sprite di colature**, 8-12 strisce verticali di 1-2 px, con indice chiaro al centro e scuro ai bordi, che scendono a 20-40 px/s.
  3. **Gocce**: 3×3 o 5×5 px, che prendono il colore del pixel sottostante spostato di 1 px (lente) con un bordo scuro, come 8-16 sprite riciclati.
  4. Una **tabella di lookup per l'appannamento** (256→256) che sposta ogni indice verso un grigio di foschia: si applica a tutto il riquadro.
- **Riscaldamento della finestra**: niente appannamento sui periscopi che lo hanno. Il CK34 è certo; per Type 18 e russi va deciso. Appannamento breve dopo l'immersione della testa in acque fredde (STIMA).

### B2. Fenomeni in mare e come resi in 256 colori

| Fenomeno | Come appare dal mare (media, descrizione) | Ricetta VGA [GIOCO] |
|---|---|---|
| **Foschia** | Orizzonte attenuato, colori desaturati verso un bianco-azzurro; le navi lontane sono silhouette grigie senza dettaglio | **Nebbia per distanza**: per ogni fascia di distanza, una tabella di lookup che sposta l'indice verso il colore di foschia (8-16 livelli). Orizzonte mescolato con 3-4 righe di dithering |
| **Nebbia d'avvezione** (aria calda su mare freddo: Grand Banks, Mare di Barents d'estate) | Muro grigio-bianco uniforme, visibilità <1 km; il sole appare come un disco pallido; alberi e sovrastrutture spariscono dall'alto | Lookup di nebbia aggressiva, cielo e mare uniti in una sola rampa; la nave emerge a 500-1000 m (STIMA) prima come forma piatta |
| **Fumo di mare artico** (*sea smoke*, aria gelida su acqua più calda) | Filamenti e colonne di vapore che salgono dalla superficie, bassi; sopra il cielo è spesso limpido. Le navi spuntano con lo scafo nascosto e gli alberi visibili | **Il contrario della nebbia**: una fascia bassa, a 5-15 righe dall'orizzonte, con animazione di colonne di dithering bianco che salgono e si perdono. Il cielo resta nitido |
| **Pioggia** | Visibilità 2-10 km (rovesci: <1 km); cortine grigie che pendono dalle nubi (*virga*, scrosci) visibili da lontano; mare punteggiato | Strisce diagonali di 1 px su 2-3 indici grigi, che si spostano a ogni fotogramma; gocce sulla lente (B1); lookup di foschia media; cortine come colonne verticali di dithering sul cielo |
| **Neve** | Visibilità fortemente ridotta, luce piatta e senza ombre, fiocchi in moto obliquo col vento | Particelle da 1-2 px, lente, con deriva sinusoidale; lookup verso un bianco-grigio; contrasto ridotto |
| **Nubi di tempesta** (cumulonembo, fronte) | Base scura e uniforme, a volte con nube a mensola; sotto, cortina di pioggia; il mare sotto è **più scuro** del resto | Cielo a rampa verticale scura in alto; banda della base più scura; il mare riflette il cielo con 2-3 indici più cupi |
| **Cavalloni e schiuma** (per Beaufort) | Da 3-4 Bft creste bianche sparse; da 6 molte creste, spruzzi; da 7 strisce di schiuma nella direzione del vento; da 9-10 mare bianco di schiuma, spruzzi che riducono la visibilità; a 12 aria piena di spruzzi (didascalia NWS Beaufort 12 `[BF12]`) | Sprite di creste (3-8 px di bianco più 1 di grigio) piazzati sulle creste delle onde, con densità proporzionale al Beaufort; da 7 Bft strisce orizzontali di schiuma; da 9 lookup di «spruzzo» che sbianca le ultime righe sopra l'orizzonte |
| **Spruzzi su nave** | Nuvole bianche che esplodono a prua quando la nave «picchia», acqua verde sul ponte | Sprite animato di 3-4 fotogrammi sulla prua dei modelli quando il beccheggio supera una soglia |
| **Ghiaccio sulle sovrastrutture** | Strutture bianche e ingrossate, profili arrotondati | [GIOCO] Variante bianca della palette dei modelli navali per le navi in clima artico con vento ≥7 Bft e temperatura <−2 °C (STIMA) |

Tecniche d'epoca (media, conoscenza generale del 256 colori, confermata in parte da `[VGAP]` `[FIRE]`):

- **rampe di palette**: 8-32 indici per cielo, mare e foschia;
- **lookup di fusione** (tabelle 256×N, come nei motori di Doom per la luce);
- **dithering ordinato** (Bayer) per sfumature e confini;
- **rotazione della palette** per animare l'acqua;
- **flash di palette** per esplosioni e fulmini (B-bis).

In MicroProse F-19 il cielo di notte è una tinta piatta e la profondità è data dalla telecamera FLIR: questo è **n.v.** come descrizione dettagliata della resa.

**Proposta** [GIOCO]

Nel periscopio VGA la palette si divide in blocchi fissi:

| Blocco | Indici |
|---|---|
| cielo | 24 |
| mare | 24 |
| schiuma e spruzzo | 8 |
| foschia | 16 |
| navi (scale grigie e colori) | 64 |
| reticolo e UI | 16 |
| lampo | 8 (riservati) |
| riserva | il resto |

Il meteo cambia **la palette** (ogni fascia oraria e ogni stato del cielo ha la sua) e **la lookup di foschia**, non i pixel. È la tecnica più economica e la più fedele all'estetica d'epoca.

### B3. Come lo resero i giochi navali

- **Silent Hunter III**: animazione dell'acqua che cola dalla lente quando il periscopio emerge `[CXW]`; nebbia e pioggia a ~400 m `[Q23]`; periscopio instabile col mare grosso come opzione `[MOBY3]`.
- **Silent Hunter 5**: gocce di pioggia che sbavano la vista; nubi, ombre, nebbia volumetrica; corretto un difetto per cui il disco del sole si vedeva sotto le nubi spesse `[GS5]`.
- **Cold Waters**: acqua sulla lente come banda scura in basso; telecamere separate sopra e sotto la superficie `[CWST]`.
- **Dangerous Waters**: i modelli «cavalcano» le onde secondo lo stato del mare `[MDW]`.
- **Silent Service II**: diversi livelli di visibilità e «murk» notturno; navi digitalizzate da foto di modellini `[RSS2]`.
- **Red Storm Rising**: nessuna resa del meteo; vista limitata dall'altezza dell'albero sull'orizzonte `[MRSR]`.

### B-bis. Fulmini in mare (aggiunta richiesta)

**Fisica utile alla grafica**

| Aspetto | Valore | Fonte (affidabilità) |
|---|---|---|
| Tipi | **Nella nube** (intra-cloud: la nube si illumina dall'interno, «lampo a lenzuolo») e **nube-suolo o nube-mare** (canale visibile e ramificato). Quelli nella nube sono la maggioranza | `[NSSL]` (alta per i tipi; la proporzione è n.v.) |
| Colpi per scarica | Media **3,67**; **circa un terzo a colpo singolo** | `[NHESS]` (media-alta, 1.174 scariche in 4 Paesi) |
| Intervallo fra i colpi | Media geometrica **57 ms** (Florida, 199 intervalli); coerente con 40-60 ms | `[ERR]` (media) |
| Durata della scarica multipla | Media **~371 ms** | `[NHESS]` |
| Visibilità del lampo | Di notte **~100 miglia** (fino a 150+ in condizioni buone); il «lampo di calore» è un temporale lontano, spesso riflesso sulle nubi alte | `[NWSHL]` (alta per la definizione; media per le distanze) |
| Tuono | Si sente a **~10 miglia** (NWS) o fino a 25 (NSSL): per il gioco usare 10-15 | `[NWSHL]` `[NSSL]` |
| Frequenza in mare | Fulmini più rari sull'oceano che sulla terra. A parità di temporale, i temporali sulla terra tropicale producono circa il doppio dei lampi | `[RMETS]` `[HARV]` `[NASA]` (media) |
| Canale | Il canale scalda l'aria a ~50.000 °F | `[NSSL]` |

**Aspetto in mare** (media per i principi; le tinte sono STIMA)

- **Di giorno**: il lampo nube-mare si vede come una linea bianca sottile sotto la base scura; l'illuminazione d'ambiente cambia poco. Quello nella nube è quasi invisibile.
- **Al crepuscolo**: il canale è bianco-violaceo; la nube s'illumina a chiazze dall'interno (tinte rosate e arancio per la luce residua); il mare sotto riflette una striscia luminosa.
- **Di notte**:
  - un solo lampo illumina **tutto**: il profilo della nube, la superficie del mare con le creste, le navi in silhouette;
  - dopo, il buio è più profondo per l'abbagliamento;
  - i lampi lontani sotto l'orizzonte appaiono come **bagliori diffusi sulle nubi**, senza tuono;
  - col mare grosso le creste bianche si «congelano» per un istante.
- **Sequenza tipica**:
  1. primo colpo, il più luminoso;
  2. 2-3 colpi successivi sullo stesso canale a ~50 ms;
  3. eventuale **corrente continua** (bagliore persistente di qualche decimo di secondo);
  4. il tutto dura ~0,2-0,5 s.

  Il «tremolio» del lampo sono proprio i colpi multipli (media, coerente con `[NHESS]`).

**Come lo resero i giochi**

- VGA d'epoca (MicroProse, EA navali): **n.v.** Nessuna fonte trovata su fulmini in 688, RSR, Silent Service II, F-19 o F-117A. Il «flash di palette» (portare per pochi fotogrammi gli indici del cielo verso il bianco e poi ripristinarli) è la tecnica standard del 256 colori `[VGAP]` (media), ma non ho un esempio citabile di un simulatore navale.
- Silent Hunter III, IV e 5, Cold Waters: fulmini nelle tempeste **n.v.** in questa ricerca. Nei simulatori professionali i parametri di fulmine stanno insieme a pioggia, neve, nebbia e stato del mare, a riprova che sono una voce standard `[AECH]` (media).

**Ricetta [GIOCO]** (VGA modo 13h a 70 Hz, ~14 ms per fotogramma)

1. **Scelta**: un temporale attivo ha un ritmo di 1-4 scariche al minuto (STIMA). Il 30% è a colpo singolo; il resto ha 2-5 colpi, con media ~3,7.
2. **Impulsi**: ogni colpo dura 1-2 fotogrammi; fra un colpo e l'altro passano 3-4 fotogrammi (~45-60 ms); ogni colpo successivo ha il 60-80% della luminosità del primo.
3. **Palette**:
   - si interpolano verso il bianco-azzurro gli indici di cielo (100% → 0 in 4-6 fotogrammi dopo l'ultimo colpo), di mare e creste (60%) e di navi (40%), e il blocco riservato «lampo»;
   - di giorno si scala tutto al 20-30%.
4. **Sprite del canale**:
   - si disegna solo per le scariche nube-mare entro ~15 km;
   - è una spezzata di 1-2 px nel blocco «lampo», con 1-3 rami, ed è visibile per la durata dei colpi;
   - ne bastano 6-10 varianti pre-disegnate, con offset casuale.
5. **Lontani**: oltre ~20 km, nessun canale; solo un **bagliore dietro una sagoma di nube**, cioè un flash locale su una porzione del cielo, e **niente tuono** se si è oltre ~15 km.
6. **Tuono**: ritardo di **3 s/km**; rombo lungo e cupo da lontano, schiocco secco da vicino.
7. **Periscopio**: ogni lampo **rivela le navi in silhouette** per un istante; con le gocce sulla lente, ogni goccia brilla.
8. **Effetto di gioco** (STIMA, da discutere nella nota sull'acustica):
   - i fulmini aggiungono **impulsi al rumore ambiente** (transitori);
   - di notte possono far vedere per un istante un bersaglio a portata visiva;
   - l'**ESM e la radio HF** sentono le scariche come disturbi.

### B5. Immagini di riferimento con licenza (Wikimedia Commons; solo elenco, nulla è stato scaricato)

Licenze ricavate dai metadati Commons (API `extmetadata`) il 25/10/2026. Prima dell'uso la licenza va ricontrollata sulla pagina del file.

| # | Titolo del file | Licenza | Cosa mostra | Utile per |
|---|---|---|---|---|
| 1 | [Soviet Charlie II class submarine at periscope depth in 1986.jpg](https://commons.wikimedia.org/wiki/File:Soviet_Charlie_II_class_submarine_at_periscope_depth_in_1986.jpg) | PD (US Navy) | Charlie II sovietico a quota periscopica, foto di un aereo USA, 1986 | Periscopio e piuma, epoca giusta |
| 2 | [HMAS Rankin at periscope depth.jpg](https://commons.wikimedia.org/wiki/File:HMAS_Rankin_at_periscope_depth.jpg) | PD (US Navy, 2004) | Collins a quota periscopica | Vela e periscopi sopra l'onda |
| 3 | [HMS Daring photograhed throught the search periscope of HMS Superb. MOD 45147431.jpg](https://commons.wikimedia.org/wiki/File:HMS_Daring_photograhed_throught_the_search_periscope_of_HMS_Superb._MOD_45147431.jpg) | OGL v1.0 | Vista **attraverso il periscopio di ricerca** di un SSN britannico | Resa reale dell'immagine al periscopio |
| 4 | [Arctic sea ice seen through periscope of USS Seadragon (SSN-584) in September 1960 (USN 1050054).jpg](https://commons.wikimedia.org/wiki/File:Arctic_sea_ice_seen_through_periscope_of_USS_Seadragon_(SSN-584)_in_September_1960_(USN_1050054).jpg) | PD | Ghiaccio artico visto dal periscopio | Missioni artiche |
| 5 | [USS John Hood (DD-655) and USS Snowden (DE-246) photographed through a submarine periscope (USN 1042008).jpg](https://commons.wikimedia.org/wiki/File:USS_John_Hood_(DD-655)_and_USS_Snowden_(DE-246)_photographed_through_a_submarine_periscope_(USN_1042008).jpg) | PD | Due navi viste dal periscopio | Composizione, grana |
| 6 | [USS Carpenter (DDE-825) seen trough periscope c1959.jpg](https://commons.wikimedia.org/wiki/File:USS_Carpenter_(DDE-825)_seen_trough_periscope_c1959.jpg) | PD | Cacciatorpediniere visto dal periscopio | Composizione |
| 7 | [US Navy 031000-N-XXXXB-002 … USS Honolulu (SSN 718) … Polar bear ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_031000-N-XXXXB-002_As_seen_through_the_periscope_of_the_Los_Angeles-class_fast_attack_submarine_USS_Honolulu_(SSN_718),_a_young_Polar_bear_investigates_the_open_water_around_the_submarine%27s_rudder.jpg) | PD | Vista dal periscopio di un **688** in Artico | Colori del periscopio di un 688 |
| 8 | [US Navy 990331-N-1556A-006 … USS Norfolk (SSN 714) ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_990331-N-1556A-006_performs_a_visual_inspection_through_the_boats_periscope_of_the_surface_area_around_the_boat.jpg) | PD | Ufficiale di guardia al periscopio su un 688 | Ambiente della plancia |
| 9 | [80-G-244725 (26465760571).jpg](https://commons.wikimedia.org/wiki/File:80-G-244725_(26465760571).jpg) | PD | Scia di periscopio e snorkel vista dall'aria, 1944 | Piuma e scia |
| 10 | [USS Waldron (DD-699) underway in heavy seas on 30 September 1953 (NH 96833).jpg](https://commons.wikimedia.org/wiki/File:USS_Waldron_(DD-699)_underway_in_heavy_seas_on_30_September_1953_(NH_96833).jpg) | PD | Cacciatorpediniere col piede di prua fuori dall'acqua | Beccheggio e onda |
| 11 | [US Navy 050402-N-6363M-027 Waves crash over the bow … USS Barry (DDG 52) ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_050402-N-6363M-027_Waves_crash_over_the_bow_of_the_Arleigh-Burke-class_guided_missile_destroyer_USS_Barry_(DDG_52)_as_she_plows_through_heavy_seas_while_underway_in_the_Mediterranean_Sea.jpg) | PD | Onde che coprono la prua | Sprite di spruzzo |
| 12 | [US Navy 031203-N-0000S-002 … USS Cole (DDG 67) encounters heavy seas ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_031203-N-0000S-002_The_guided_missile_destroyer_USS_Cole_(DDG_67)_encounters_heavy_seas_while_transiting_across_the_Atlantic.jpg) | PD | Mare grosso in Atlantico | Mare 6-7 |
| 13 | [USS Essex (CVA-9) underway in heavy seas, 12 January 1960 (NH 98517).jpg](https://commons.wikimedia.org/wiki/File:USS_Essex_(CVA-9)_underway_in_heavy_seas,_12_January_1960_(NH_98517).jpg) | PD | Portaerei con spruzzi a prua | Grande nave col mare grosso |
| 14 | [Royal Navy Ship in Rough Weather MOD 45154618.jpg](https://commons.wikimedia.org/wiki/File:Royal_Navy_Ship_in_Rough_Weather_MOD_45154618.jpg) | OGL v1.0 | Type 45 col mare grosso, Atlantico meridionale | Mare grosso, colori moderni |
| 15 | [Wasser über Deck und Luken WNA.png](https://commons.wikimedia.org/wiki/File:Wasser_%C3%BCber_Deck_und_Luken_WNA.png) | CC BY-SA 3.0 (Buonasera) | Nord Atlantico d'inverno, acqua sul ponte, 1958 | Acqua verde |
| 16 | [Wea00800,1.jpg](https://commons.wikimedia.org/wiki/File:Wea00800,1.jpg) | PD (NOAA) | Petroliera col mare grosso e onda anomala, Golfo di Biscaglia | Onda enorme |
| 17 | [Wea00820 - Flickr - NOAA Photo Library.jpg](https://commons.wikimedia.org/wiki/File:Wea00820_-_Flickr_-_NOAA_Photo_Library.jpg) | PD (NOAA) | Nave NOAA in tempesta al largo del North Carolina | Tempesta |
| 18 | [US Navy 110415-N-7293M-635 … USS Ponce (LPD 15) emerges from a low-lying fog ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_110415-N-7293M-635_The_amphibious_transport_dock_ship_USS_Ponce_(LPD_15)_emerges_from_a_low-lying_fog_in_the_Mediterranean_Sea.jpg) | PD | Nave che esce da una **nebbia bassa** | Nebbia in mare |
| 19 | [Wea03496 - Flickr - NOAA Photo Library.jpg](https://commons.wikimedia.org/wiki/File:Wea03496_-_Flickr_-_NOAA_Photo_Library.jpg) | PD (NOAA) | Nave vista attraverso pioggia e foschia durante un temporale | Pioggia e visibilità |
| 20 | [Sea smoke on Lake Superior (54250372824).jpg](https://commons.wikimedia.org/wiki/File:Sea_smoke_on_Lake_Superior_(54250372824).jpg) | PD (USFWS) | **Fumo di mare** su acqua libera con aria gelida | Fumo di mare artico (ma è un lago) |
| 21 | [Sea smoke Gloucester Massachusetts 2024-12.png](https://commons.wikimedia.org/wiki/File:Sea_smoke_Gloucester_Massachusetts_2024-12.png) | CC BY-SA 4.0 | Fumo di mare sull'oceano (Atlantico) | Fumo di mare in mare vero |
| 22 | [Beaufort scale 0.jpg](https://commons.wikimedia.org/wiki/File:Beaufort_scale_0.jpg) … 1, 2, 3, 4, 5, 6, 8, 9, 11, [Beaufort scale 12.jpg](https://commons.wikimedia.org/wiki/File:Beaufort_scale_12.jpg) | PD (NWS NOAA) | **Serie Beaufort 0-12**: tutti i file da *Beaufort scale 0.jpg* a *12.jpg* esistono (verificato con l'API). Foto di ~400 px; quelle da 0 a 4 sono **sui Grandi Laghi** (NWS Great Lakes); per le altre l'origine esatta è n.v. | Densità di creste e schiuma per Beaufort |
| 23 | [Sea state code height.png](https://commons.wikimedia.org/wiki/File:Sea_state_code_height.png) | CC0 | Grafico dei codici OMM dello stato del mare e altezze | Tabella A3 |
| 24 | [Whitecap.png](https://commons.wikimedia.org/wiki/File:Whitecap.png) | CC BY-SA 3.0 (Ardhuin) | Cresta frangente in primo piano | Sprite della cresta |
| 25 | [Lightning NOAA.jpg](https://commons.wikimedia.org/wiki/File:Lightning_NOAA.jpg) | PD (NOAA NSSL, C. Clark) | Più scariche nube-suolo e nube-nube di notte | Forma del canale e ramificazioni |
| 26 | [US Navy 021102-N-0000W-001 Lightning fills the horizon ….jpg](https://commons.wikimedia.org/wiki/File:US_Navy_021102-N-0000W-001_Lightning_fills_the_horizon,_and_lights_up_the_flight_deck_and_air_wing_aircraft_parked_on_the_ship%E2%80%99s_forward_elevator_and_bow.jpg) | PD | Fulmini all'orizzonte che **illuminano il ponte** di una portaerei | Luce d'ambiente del lampo in mare |
| 27 | [US Navy 040504-N-1280S-006 A rare lightning storm … USS George Washington (CVN 73).jpg](https://commons.wikimedia.org/wiki/File:US_Navy_040504-N-1280S-006_A_rare_lightning_storm_as_seen_from_the_signal_bridge_at_night_aboard_the_aircraft_carrier_USS_George_Washington_(CVN_73).jpg) | PD | Temporale notturno visto da una nave | Lampo di notte in mare |
| 28 | [Lightning storm over ocean at night.jpg](https://commons.wikimedia.org/wiki/File:Lightning_storm_over_ocean_at_night.jpg) | CC BY-SA 4.0 (bigwavephoto) | Temporale sull'oceano di notte | Lampo e riflesso sul mare |
| 29 | [Lightning Sunset Bass Strait.jpg](https://commons.wikimedia.org/wiki/File:Lightning_Sunset_Bass_Strait.jpg) / [Lightning at the beach. (34458790905).jpg](https://commons.wikimedia.org/wiki/File:Lightning_at_the_beach._(34458790905).jpg) | CC BY 2.0 | Lampo al tramonto sullo stretto di Bass; temporale sull'Atlantico (il secondo è un **composito di 4 scatti**) | Lampo al crepuscolo |
| 30 | [Waterspout S Florida.jpg](https://commons.wikimedia.org/wiki/File:Waterspout_S_Florida.jpg) | CC BY 4.0 | Tromba marina | Evento estremo (opzionale) |

Lacune: non ho trovato su Commons foto con licenza di una **vista dal periscopio col mare grosso, con l'acqua sulla lente**: **n.v.** Si possono cercare nei filmati DVIDS e NARA, che sono di pubblico dominio, oppure ricostruire dalle immagini 3, 7 e 10-12. Anche una foto di **fumo di mare artico con una nave militare** è **n.v.**

---

## B6. Stato del mare e animazioni del kit di sprite (estensione)

### B6.1 Mare da vento e onda lunga

Il manuale della Navy del 1976 distingue `[AG76]` (alta):

- il **mare da vento** (*sea waves*): onde generate dal vento locale, creste **aguzze e angolose**, corte, con molte onde piccole sovrapposte, altezze irregolari e direzioni non allineate;
- l'**onda lunga** (*swell*): onde uscite dalla zona di generazione, **basse, con la cima arrotondata**, quasi tutte della stessa altezza, con creste che si seguono a lungo. Uscendo dalla zona l'altezza cala e il periodo cresce.

Si osservano separatamente, ed esistono anche due sistemi di onda lunga insieme.

La **scala Douglas** ha due parti `[DOUG]` (media):

- il **mare da vento**, 0-9: è il codice OMM della tabella in A3;
- l'**onda lunga**, classificata per lunghezza (corta 0-100 m, media 100-200 m, lunga >200 m) e altezza (bassa 0-2 m, moderata 2-4 m, alta >4 m), più il grado «confusa».

L'OMM ha adottato solo la parte del mare da vento.

Fisica di base (alta come formule; i valori di gioco sono STIMA):

- **acque profonde**: lunghezza d'onda **L ≈ 1,56·T²** (m), velocità di cresta **c ≈ 1,56·T** (m/s), con T il periodo in s;
- **mare completamente sviluppato** (Pierson-Moskowitz): periodo di picco **Tp ≈ 0,73·U** (s, con U il vento in m/s); altezza significativa **Hs ≈ 0,21·U²/g**.

  Esempio: con 20 kn (10 m/s) si ha Tp ≈ 7 s e Hs ≈ 2 m; questo è coerente con Beaufort 5 = 2 m `[RMB]`.

### B6.2 Aspetto per stato del mare, per gli animatori

I criteri visivi vengono dalla scala Beaufort della RMetS `[RMB]` (alta); periodi, lunghezze e densità sono STIMA da B6.1. Ogni riga rimanda alla foto NWS della serie Beaufort su Commons (PD), elencata in B5 al n. 22. Le foto da 0 a 4 sono state scattate sui Grandi Laghi.

| SS (OMM) | Bft | Onda (m) | Periodo (s) | Creste e schiuma | Spruzzo | Animazione [GIOCO] | Foto |
|---|---|---|---|---|---|---|---|
| 0 | 0 | 0 | — | Mare a specchio | — | Superficie statica, solo riflessi; rotazione lenta della palette | Bft 0 |
| 1 | 1 | 0-0,1 | 1-2 | Increspature, nessuna cresta | — | Tremolio dei riflessi con 2 indici | Bft 1 |
| 2 | 2-3 | 0,1-0,5 | 2-4 | Ondine; a Bft 3 **cavalloni sparsi** | — | Onda di 1-2 px; 1 cresta bianca ogni ~200×20 px di mare | Bft 2, 3 |
| 3 | 4 | 0,5-1,25 | 4-6 | Onde che si allungano, **cavalloni frequenti** | — | Creste bianche da 3-5 px, che vivono 0,5-1 s e si spengono in 3 fotogrammi (bianco → grigio → mare) | Bft 4 |
| 4 | 5 | 1,25-2,5 | 6-8 | Onde moderate, **molti cavalloni** | Qualche spruzzo | Linea dell'orizzonte frastagliata di 1-2 px; creste ×2 | Bft 5 |
| 5 | 6-7 | 2,5-4 | 7-10 | **Creste schiumose ovunque**; da Bft 7 **strisce di schiuma** nella direzione del vento | Spruzzo frequente | Strisce orizzontali di schiuma (righe di 6-20 px in grigio chiaro) che scorrono con la cresta. La testa del periscopio va sotto ogni tanto | Bft 6, 7 |
| 6 | 8-9 | 4-6 | 9-12 | Da Bft 8 le creste **si sfrangiano in spruzzaglia** (*spindrift*); a 9 le creste si rovesciano | Lo spruzzo **riduce la visibilità** | Sprite di «fumo bianco» staccato dalla cresta sottovento (4 fotogrammi); lookup di foschia bassa sulle prime righe sopra l'orizzonte; orizzonte seghettato di 3-6 px | Bft 8, 9 |
| 7 | 10 | 6-9 | 11-14 | **Superficie in gran parte bianca** | Molto spruzzo | Mare a chiazze bianche (dithering 50% sulle zone di cresta); gli scafi piccoli spariscono dietro le onde | Bft 10 |
| 8 | 11 | 9-14 | 13-16 | Mare coperto di schiuma | Visibilità molto ridotta | Rampa del mare spostata verso il bianco; navi visibili solo sulle creste | Bft 11 |
| 9 | 12 | >14 | 15+ | Aria piena di schiuma e spruzzi | Visibilità pessima | Quasi solo schiuma e foschia; periscopio di fatto inutilizzabile | Bft 12 |

**L'onda lunga è un livello separato** [GIOCO]: un'ondulazione lenta (T 10-16 s), con creste lunghe e arrotondate, **senza schiuma**, che si somma al mare da vento. Ha due effetti:

- fa beccheggiare e rollare le navi anche quando c'è poco vento;
- fa salire e scendere l'orizzonte nel periscopio.

Il briefing la dà a parte (es. «SWELL W 10FT», A3).

### B6.3 Il moto delle navi visto dal periscopio

**Fonti e principi**

- Periodo naturale di rollio: **T ≈ C·B/√GM**, con B la larghezza e GM l'altezza metacentrica; C è un coefficiente sperimentale, circa 0,7-0,8 in s/√m (STIMA sul valore) `[JNAV]` (alta per la forma). Più la nave è stabile (GM alto), più il rollio è rapido e «a scatti»; più è larga e poco stabile, più è lento.
- Un mercantile ha rollato fino a 44° col periodo stimato di ~10 s, in risonanza con un'onda lunga di ~10 s `[BRIT]` (media). I cacciatorpediniere della Seconda guerra mondiale arrivarono a sbandamenti di 48-67° nelle tempeste `[USNI51]` (media): sono casi estremi.
- Per le operazioni di volo, i limiti di ponte (SHOL) si fissano spesso intorno a un **rollio massimo di ~20°**, con limiti molto più stretti al momento dell'appontaggio `[ICAS]` (media). Esistono criteri NATO di tenuta al mare, STANAG 4154, in valori RMS di rollio e beccheggio per compito; le soglie esatte sono n.v. `[VIK]`.
- **Fuga dell'elica** (*propeller racing*): a poppa alta l'elica esce in tutto o in parte dall'acqua e i giri schizzano; si riduce la potenza `[USCG]` `[TRBP]` (media). È visibile sulle unità piccole.
- **Emersione della cupola sonar e martellamento della prua** (*slamming*) su fregate e cacciatorpediniere in velocità con mare grosso: **n.v.** come numeri; la plausibilità fisica è alta. Effetto [GIOCO]: con mare 6+ i sonar di scafo nemici degradano.

**Valori per l'animazione** (tutti **STIMA**, derivati dalla formula del rollio e dal rapporto fra lunghezza d'onda e lunghezza della nave). La regola: **una nave beccheggia molto quando la lunghezza d'onda si avvicina alla sua lunghezza**, e rolla molto quando il periodo d'incontro si avvicina al suo periodo di rollio.

| Classe (esempio) | L × B (m) | Rollio naturale (s) | Beccheggio (s) | SS3 | SS5 | SS7 |
|---|---|---|---|---|---|---|
| Motovedetta, corvetta piccola (Osa, Grisha) | 40-70 × 8-10 | 5-7 | 3-4 | rollio ±3°, beccheggio ±1-2° | rollio ±10-15°, beccheggio ±4-6°, **prua che si pianta**, spruzzo su tutto, **eliche fuori** | ingovernabile o al riparo; nel periscopio sparisce dietro le onde |
| Fregata o cacciatorpediniere (Krivak, Udaloy, FFG-7, Spruance) | 120-170 × 13-17 | 9-12 | 5-7 | ±1-2° / ±0,5-1° | ±5-10° / ±2-4°, spruzzo sopra il castello a prua | ±15-25° / ±5-8°, **acqua verde sul castello**, prua che esce e ribatte |
| Incrociatore (Kirov, Slava) | 185-250 × 20-29 | 12-15 | 6-8 | quasi fermo | ±3-6° / ±1-3° | ±10-15° / ±3-5°, spruzzo alto a prua |
| Portaerei (Kiev, Kuznetsov, Nimitz) | 270-330 × 30-40 (linea di galleggiamento) | 15-20 | 8-10 | fermo | ±1-3° / ±0,5-1° | ±3-6° / ±1-3°; spruzzo sul ponte di volo a prua `[USN-ESSEX]` |
| Mercantile, petroliera | 150-300 | 12-25 (dipende dal carico) | 7-10 | fermo | rollio lento e ampio se c'è onda lunga al traverso | rischio di **rollio parametrico** e di risonanza |

Le ampiezze sono **singole**, nella condizione peggiore; con mare di prua il rollio è minore e il beccheggio maggiore.

**Come appare nel periscopio** [GIOCO]

- **Rollio**: la sagoma ruota intorno alla linea di galleggiamento. Lo sprite si gira con 3-5 pose precalcolate per lato (0°, 5°, 10°, 20°), oppure l'albero si sposta di qualche pixel.
- **Beccheggio**: la prua sale e scende. Con mare SS5+ sulle navi piccole e medie si vede la **chiglia di prua fuori** e subito dopo uno **sprite di spruzzo** a prua (3-4 fotogrammi bianchi).
- **Moto verticale (sussulto)**: tutta la nave sale e scende sull'onda col periodo d'incontro. Con le onde alte la nave **sparisce dietro la cresta più vicina**: si maschera la parte bassa della nave quando la cresta in primo piano è più alta.
- **Periodo d'incontro**: **Te = T / (1 + V·cos μ / c)**, con c ≈ 1,56·T la velocità dell'onda, V la velocità della nave e μ = 0 per mare di prua (formula standard di tenuta al mare, alta). Si accorcia contro le onde, si allunga col mare di poppa. È questo il ritmo dell'animazione.
- **Eliche fuori** (navi <80 m, SS5+): 2 fotogrammi di poppa sollevata con spruzzo bianco. Sul piano sonoro: picchi di giri e rumore di cavitazione intermittente (vedi il dossier audio 18).
- **Visto da quota periscopica**: la testa è a 1-2 m dall'acqua, quindi con mare 4+ **l'orizzonte stesso ondeggia** e le creste vicine coprono le navi lontane per buona parte del tempo. La portata visiva «utile» va ridotta in proporzione al tempo in cui la nave è nascosta (STIMA).

### B6.4 Elicotteri in hovering col vento

**Fonti**

- Le foto su Commons (PD, US Navy) mostrano SH-3 e SH-60F che calano l'AQS-13 a bassa quota. Si vede bene l'**anello di spruzzo** sollevato dal rotore sull'acqua:
  - [AQS-13 Dipping sonar.jpg](https://commons.wikimedia.org/wiki/File:AQS-13_Dipping_sonar.jpg);
  - [SH-3H HS-15 lowers AQS-13 sonar 1979.JPEG](https://commons.wikimedia.org/wiki/File:SH-3H_HS-15_lowers_AQS-13_sonar_1979.JPEG);
  - [SH-3H HS-4 deploys sonar during RIMPAC 1986.JPEG](https://commons.wikimedia.org/wiki/File:SH-3H_HS-4_deploys_sonar_during_RIMPAC_1986.JPEG);
  - [SH-60F Seahawk dipping sonar.jpg](https://commons.wikimedia.org/wiki/File:SH-60F_Seahawk_dipping_sonar.jpg);
  - [SH-3D Sea King of HS-84 with AQS-13 sonar in 1983.JPEG](https://commons.wikimedia.org/wiki/File:SH-3D_Sea_King_of_HS-84_with_AQS-13_sonar_in_1983.JPEG) (sonar trainato in volo).
- Il **cavo** non è mai fermo: vibra, oscilla e rimbalza. Con un verricello veloce il sonar sale e scende a circa 5 m/s `[PAT-CA]` (media).
- Dalle flashcard di addestramento ASW `[ASWF]` (bassa):
  - i requisiti del modo «cable angle hover» (l'automatismo che tiene il cavo verticale): velocità al suolo entro ±5 kn, quota entro 10 ft;
  - un hovering accoppiato a **70 ft** sotto 4 kn;
  - un limite di vento intorno a **17-21 kn** per le immersioni del sonar in addestramento.
- Il controllo dell'angolo del cavo nel vento e nella turbolenza è un problema studiato `[DELFT]` (media). Per il Ka-27 sovietico (VGS-3): quota e cavo **n.v.**

**Animazione** [GIOCO]

- **Assetto**: muso al vento. Col vento il disco del rotore si inclina controvento, quindi la fusoliera sta **a muso leggermente basso** (2-5°, STIMA) e con un poco di sbandamento se il vento è al traverso. Raffiche: oscillazioni di ±1-2° e di qualche metro di quota, ogni 2-4 s (STIMA).
- **Cavo**: in assenza di automatismo (anni '80 senza «cable angle hover»: n.v. per epoca e modello) il cavo devia dalla verticale verso **sottovento o sotto corrente**, di 5-15° (STIMA). Con l'automatismo resta quasi verticale e l'elicottero «insegue» il sonar.
- **Superficie**: anello di spruzzo e increspatura circolare sotto il rotore (diametro ~1-1,5 rotori, STIMA), spostato sottovento. Con mare SS5+ l'anello si confonde con la schiuma.
- **Limite di gioco**: sopra ~20 kn di vento o con mare 5-6, gli elicotteri nemici **non immergono il sonar** o lo fanno con meno precisione (STIMA, coerente con `[ASWF]`). È un vantaggio del maltempo per il sottomarino, da collegare al meteo.

### B6.5 Il periscopio che va sotto e riemerge

Riassunto operativo di B1, in fotogrammi a 70 Hz (STIMA):

1. **Cresta in arrivo** (frame 0-3): la parte bassa del campo si oscura dal basso verso l'alto con un bordo seghettato e una rampa verde-grigia.
2. **Sotto** (da 0,3 a 1,5 s): il campo è tutto verde-grigio scuro con macchie di bolle (sprite di 2-3 indici); niente reticolo di distanza, perché lo stadimetro è inutilizzabile.
3. **Uscita** (frame 0-6): l'acqua scende dall'alto in fasce; restano **colature** (che scendono per 1-2 s) e **gocce**, che poi spariscono in 3-5 s (o subito, se la finestra è riscaldata e asciutta, STIMA).
4. **Frequenza**: dipende dallo stato del mare, dalla quota (più alta = meno immersioni ma più piuma), dalla rotta rispetto alle onde (con mare di poppa la quota è più difficile da tenere `[IMAR]`) e dalla velocità.

### B6.6 Come lo animavano i giochi

| Gioco | Mare e onde | Moto delle navi | Periscopio sotto e sopra | Elicotteri | Fonte |
|---|---|---|---|---|---|
| **688 Attack Sub** (1989) | Nessuna voce sullo stato del mare nel manuale | n.v. (le navi sembrano sprite fermi: va controllato nell'EXE) | n.v. | n.v. | `[M688]` |
| **Red Storm Rising** (1988) | Rumore di superficie per tipo di ghiaccio o mare; niente stato del mare animato | n.v. | n.v. | n.v. | `[MRSR]` |
| **Harpoon I e II** | Solo mappa 2D: tacche colorate di vento e stato del mare, nubi, precipitazioni | Nessuna animazione | — | Effetti sui sensori, nessuna resa visiva | `[MH2]` (alta) |
| **Dangerous Waters** (2005) | Stato del mare fissato in missione | **«Waveriding»**: le piattaforme 3D cavalcano le onde secondo lo stato del mare. Se si disattiva, restano dritte e possono affondare nell'onda o galleggiarci sopra | n.v. | n.v. | `[MDW]` (alta) |
| **Silent Hunter III / GWX** | Acqua 3D, meteo che cambia, onde col vento | Le navi beccheggiano e rollano con l'acqua 3D (**bassa, n.v. nei dettagli**); i parametri di tempesta sono in `uboat.cfg` | Acqua che cola dalla lente quando il periscopio esce; opzione «stabilità del periscopio» col mare grosso | n.v. | `[CXW]` `[MOBY3]` `[HGW]` |
| **Silent Hunter 5** | Nebbia volumetrica, nubi | n.v. | Gocce di pioggia che sbavano | — | `[GS5]` |
| **Cold Waters** (2017) | Lo stato del mare entra nel rumore | n.v. | Banda scura dell'acqua sulla lente; telecamere separate sopra e sotto la superficie | n.v. | `[CWST]` `[RPS]` |

Lezioni [GIOCO]:

- DW insegna che **il moto delle navi va legato allo stato del mare della missione**, non messo come decorazione a parte.
- SH3 insegna che **l'acqua sulla lente vende la sensazione** più di ogni onda.
- Harpoon II insegna a mostrare lo stato del mare **anche sulla carta** (tacche o colori), così il giocatore collega mare e sonar.

---

## C. Proposte riassunte per il remake [GIOCO]

1. **Pannello METEO E MARE** nel briefing (A4, punto 1), con tendenza e messaggio inventato in stile WEAX (A3).
2. **Aggiornamenti in missione** solo nelle finestre di copia del broadcast (A2): GALE WARNING, WEAX, COR. **Niente richieste** del giocatore in EMCON. La richiesta si può fare in superficie o in trasferimento, con un costo di scoperta.
3. **Effetti di sistema** (tarare con il dossier acustico):
   - stato del mare → rumore ambiente → portata passiva (circa −15% per punto, Harpoon II); attivo degradato sopra SS5;
   - precipitazioni → radar e vista;
   - ghiaccio → rumore (pack silenzioso, ghiaccio alla deriva rumoroso, come in RSR).
4. **Quota periscopica col mare grosso**: immersione ciclica della testa, rischio di affioramento, scelta «salgo di 5 ft» con più piuma; il mare mosso nasconde la piuma.
5. **Grafica**:
   - palette e lookup di foschia per fascia oraria e per cielo;
   - overlay d'acqua, colature, gocce, appannamento;
   - fumo di mare come fascia bassa;
   - fulmini con 3-4 impulsi a ~50 ms e flash di palette, canale solo da vicino, bagliori lontani senza tuono.
6. **Evoluzione lenta**: fronti in 6-12 h di gioco; niente salti al confine fra celle.
7. **Mare come sistema** (B6). Il vento genera il mare da vento (Pierson-Moskowitz, con ritardo di ore); l'onda lunga è un secondo livello indipendente. Lo stato del mare guida tutto insieme:
   - creste e schiuma, per Beaufort;
   - il moto delle navi, per classe, con il periodo d'incontro;
   - le eliche fuori e gli spruzzi a prua;
   - l'immersione della testa del periscopio;
   - i limiti degli elicotteri col sonar a immersione;
   - il rumore ambiente.

---

## F. Fonti

| Sigla | Fonte | URL | Affidabilità |
|---|---|---|---|
| WPFN | Wikipedia, *Fleet Numerical Meteorology and Oceanography Center* (nomi, 1979, SUBWEAX 2008) | https://en.wikipedia.org/wiki/Fleet_Numerical_Meteorology_and_Oceanography_Center | media |
| ONR87 | *Naval Research Reviews* vol. 39 n. 1 (1987): FNOC, strato misto 1981 | https://www.onr.navy.mil/media/document/naval-research-reviews-vol-39-no-1-1987 | alta |
| APL | Zambresky, *JHU APL Tech Digest* 8(1): previsione d'onda globale FNOC | https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V08-N01/08-01-Zambresky.pdf | alta |
| WWM | Wikipedia, *Wind wave model* (SWOM) | https://en.wikipedia.org/wiki/Wind_wave_model | media |
| TRB85 | Wells e Wargelin 1985, *Programs and products of the Naval Oceanographic Office* (TESS) | https://trid.trb.org/View/429857 | media (solo scheda) |
| DTIC-ESR | Rapporto declassificato su ASW in acque basse e previsione della portata sonar | https://apps.dtic.mil/sti/pdfs/AD0374921.pdf | media |
| BOW | Bowditch, *American Practical Navigator*, cap. 41 *Weather Routing* | https://thenauticalalmanac.com/2019_Bowditch-_American_Practical_Navigator/Volume-_1/12-%20Part%2010-%20Marine%20Meteorology/Chapter%2041-%20Weather%20Routing.pdf | alta |
| OWS | Ocean Weather Services, storia dell'OTSR (1958) | https://gcaptain.com/optimum-ship-routing | media-bassa |
| BRS | Flashcard METOC della Navy (WEAX, OTSR, avvisi, COR, CFC) | https://brainscape.com/flashcards/114-metoc-2825540/packs/4700306 | bassa-media (materiale di studio) |
| BWIRE | Comunicato: FWC e OTSR a unità di superficie e subacquee | https://www.businesswire.com/news/home/20241107376881/en | media (moderno) |
| JTWC | DTIC sui compiti FWC/JTWC Guam e CPHC | https://apps.dtic.mil/sti/pdfs/ADA399632.pdf | media |
| CNIC | Navy CNIC, *Issuance of Destructive Weather Warnings* | https://cnrse.cnic.navy.mil/Installations/NS-Mayport/About/Hurricane-Information/Terms/Issuance-of-Destructive-Weather-Warnings | alta (moderno) |
| ICE | *Arctic* (UCalgary): storia delle previsioni del ghiaccio per i sottomarini, Joint Ice Center 1976 | https://journalhosting.ucalgary.ca/index.php/arctic/article/download/64464/48399/183657 | media |
| ARC | DTIC, manuale del modello del ghiaccio su dati FNOC | https://apps.dtic.mil/sti/pdfs/ADA114343.pdf | media |
| AVW | AvWeb, FAQ METAR/TAF (passaggio dell'1/7/1996) | https://avweb.com/features/frequently-asked-questions-about-metar-and-taf/ | media |
| DD175 | Modulo DD 175-1 in bianco (CNATRA) | https://www.cnatra.navy.mil/ebrief/documents/01_Training_Air_Wing_One/flight%20plan/COLUMN%202/MISC/FILLABLE%20BLANK%20DD-175-1%20WX%20BRF%20FORM.pdf | alta |
| PSFB | Bollettino meteo di Patrick SFB (astronomia e stato del mare) | https://www.patrick.spaceforce.mil/Portals/14/PSFBMWP%2004OCT22%201600L.pdf | media |
| SSC | Commons, *Sea state code height.png* (codice OMM) | https://commons.wikimedia.org/wiki/File:Sea_state_code_height.png | media |
| FASVLF | FAS, *Very Low Frequency* | https://nuke.fas.org/guide/usa/c3i/vlf.htm | media |
| NWSFAX | NWS, radiofax marino (archivio) | https://webharvest.gov/peth04/20041106010112/http://www.nws.noaa.gov/om/marine/radiofax.htm | alta |
| AARI | AARI, storia della Гидрометслужба Северного флота | https://data.aari.ru/publicat/rpr/sources/33/RPR-33p_44-46.pdf | media |
| GUNIO | ru.wikipedia, ГУНиО МО | https://ru.wikipedia.org/wiki/Главное_управление_навигации_и_океанографии | media |
| IZ | Izvestija sulle comunicazioni con i sottomarini («Зевс») | https://iz.ru/news/692774 | media-bassa |
| M688 | Manuale di *688 Attack Sub* (OCR) | https://archive.org/details/688attacksub_201909 | alta |
| MRSR | Manuale di *Red Storm Rising*, ed. tedesca Amiga (OCR) | https://archive.org/details/manualzilla-id-6749727 | alta |
| MGS | Manuale di *Gunship* C64 (OCR) | https://archive.org/details/Gunship_1986_Microprose_Software_6901_Instructions | alta |
| MF19 | Manuale di *F-19 Stealth Fighter* (OCR) | https://archive.org/details/f19stealthfightermanual | alta |
| MH2 | Manuale di *Harpoon II Admiral's Edition* (OCR), sezioni *Weather Data* e *Technical Notes: Weather* | https://archive.org/details/harpoon-ii-admirals-edition-1996 | alta |
| MDW | Manuale di *Dangerous Waters* (OCR): sea state, waveriding, rumore | https://archive.org/details/DANGEROUS_WATERS | alta |
| MSH5 | Manuale di *Silent Hunter 5* (OCR): cannone solo con bel tempo | https://archive.org/details/silenthunter5manual | alta |
| HGF | HarpGamer, *Weather modeling in Harpoon Classic* (2005) | https://harpgamer.com/harpforum/topic/107-weather-modeling-in-harpoon-classic | bassa |
| RSS2 | Recensione di *Silent Service II* (textfiles) | https://mirror.cyberbits.eu/textfiles.com/games/REVIEWS/silserv.rev | media |
| RPS | Intervista sviluppatori di Cold Waters (CombatAce / RPS) | https://combatace.com/forums/topic/90480-rock-paper-shotgun-interview-with-cold-waters/ | media |
| CWST | Steam, discussione sul periscopio di Cold Waters | https://steamcommunity.com/app/541210/discussions/0/1470840994968959760 | bassa |
| MOBY3 | MobyGames, *Silent Hunter III* | https://www.mobygames.com/game/17098 | media |
| Q23 | Quarter to Three, diari di Silent Hunter III (400 m, mare calmo) | https://www.quartertothree.com/fp/2011/04/05/silent-hunter-iii-its-1940-do-you-know-where-your-dj18-is/ | bassa |
| HGW | HarpGamer, AAR SH3 / note GWX e *StormConditions* | https://harpgamer.com/harpforum/topic/773-silent-hunter-iii-aars-of-the-harpgamer-wolfpack/ | bassa |
| CXW | CodeWeavers, SH3: crash all'animazione dell'acqua che cola dalla lente | https://codeweavers.com/compatibility/crossover/forum/silent-hunter-iii?msg=47282 | bassa |
| GS5 | GameSpot, *Silent Hunter 5 First Impressions* (gocce che sbavano) | https://www.gamespot.com/articles/silent-hunter-5-first-impressions/1100-6216064/ | media |
| FBMS | Forum Falcon BMS (meteo che cambia troppo in fretta) | https://forum.falcon-bms.com/post/224855 | bassa |
| FLW | Falcon Lounge, strumenti meteo BMS | https://falcon-lounge.com/falcon-bms-essentials/tools/weather-tools-falcon-bms | bassa |
| BEN | M. Bennett, *Up Periscope!* (1943) | https://www.perlego.com/book/3019968/up-periscope-pdf | media-bassa |
| NSLF | CDR G. K. Fraser, «GET ME DOWN!», *Submarine Review* NSL, aprile 1995 | https://archive.navalsubleague.org/?p=2765 | media |
| DEPTH | DTIC, studio sul controllo di quota periscopica | https://apps.dtic.mil/sti/pdfs/ADA288546.pdf | media |
| UTAS | Effetti di risucchio vicino alla superficie (UTAS) | https://figshare.utas.edu.au/articles/conference_contribution/Characterisation_of_suction_effects_on_a_submarine_body_operating_near_the_free_surface/23101247/1 | media |
| NPS | NPS, *Assessment of shallow water near surface response of submersible vehicles* | https://calhoun.nps.edu/entities/publication/cbe37109-8ec5-49c9-bce8-7441712580fc | media |
| IMAR | IMarEST, rapporto storico sul controllo in mare di poppa | https://library.imarest.org/record/7881/files/v02b4p13a.pdf | media |
| PATDE | Brevetto tedesco, lastra riscaldante antiappannamento | https://patents.google.com/patent/EP0017272B1/de | media |
| PATNL | Brevetto sulle vibrazioni del periscopio (anni '60) | https://patents.google.com/patent/US3352618 | media |
| WWI | Mimetizzazione dei periscopi nella Prima guerra mondiale (periscopio sommerso = cieco) | https://mobicore.dissgea.unipd.it/s/WarWon/item/6974 | media |
| NSSL | NOAA NSSL, *Severe Weather 101: Lightning* | https://www.nssl.noaa.gov/education/svrwx101/lightning/ | alta |
| NHESS | *NHESS* 21, 1909 (2021): molteplicità e durata delle scariche | https://nhess.copernicus.org/articles/21/1909/2021/nhess-21-1909-2021.html | media-alta |
| ERR | EarthRef, intervalli fra i colpi in Florida (57 ms) | https://earthref.org/ERR/57849 | media |
| NWSHL | NWS, *Heat lightning* | https://www.weather.gov/safety/lightning-heat | alta |
| RMETS | RMetS, *What happens when lightning strikes the ocean* | https://www.rmets.org/metmatters/what-happens-when-lightning-strikes-ocean | media |
| HARV | Harvard SITN, sale marino e meno fulmini sull'oceano | https://sitn.hms.harvard.edu/tag/storms | media |
| NASA | NASA NTRS, tassi di lampi terra e oceano | https://ntrs.nasa.gov/citations/19990108786 | media |
| AECH | Aechelon, parametri meteo dei simulatori (fulmine, pioggia, nebbia, stato del mare) | https://docs.aechelon.com/aechelon-products-documentation/Working-version-Branch-TBD/Unrestricted/weather-control-packet-support | media |
| VGAP | Devlog CyberVGA (palette per luce e flash) | https://expfunction.itch.io/cybervga/devlog/1014737/cybervga-is-live-on-itchio | bassa |
| FIRE | GameDev.net, *Simple fire effect* (rampe di palette in modo 13h) | https://www.gamedev.net/articles/programming/graphics/simple-fire-effect-r222/ | media |
| BF12 | Commons, *Beaufort scale 12 notext.jpg* (didascalia NWS) | https://commons.wikimedia.org/wiki/File:Beaufort_scale_12_notext.jpg | alta (PD) |
| AG76 | NAVEDTRA, *Aerographer's Mate 3 & 2* (1976), OCR: FLEWEACEN, bollettini di flotta, mare da vento e onda lunga, SST e portata sonar | https://archive.org/details/aerographersmate00nava | alta |
| RMB | RMetS, *The Beaufort Wind Scale* (criteri del mare e altezze d'onda) | https://www.rmets.org/metmatters/beaufort-wind-scale | alta |
| DOUG | Wikipedia, *Douglas sea scale* (mare da vento e onda lunga) e Jackson Parton | https://en.wikipedia.org/wiki/Douglas_sea_scale ; https://jacksonparton.com/the-douglas-sea-state-scale | media |
| JNAV | *Journal of Navigation*, rollio e alette di rollio (T = C·B/√GM) | https://www.cambridge.org/core/journals/journal-of-navigation/article/roll-motion-of-a-ship-and-the-roll-stabilising-effect-of-bilge-keels/9DFED419AC43FC6E9DDC27F08F8319B9 | alta (forma) |
| BRIT | Britannia P&I, *Risk Watch* 2010 (rollio di 44°, periodo di ~10 s) | https://britanniapandi.com/wp-content/uploads/2025/03/riskwatchvol17No206-2010.pdf | media |
| USNI51 | USNI *Proceedings*, maggio 1951, «Extraordinary roll» | https://www.usni.org/magazines/proceedings/1951/may/extraordinary-roll | media |
| ICAS | ICAS 2002, limiti di ponte per elicotteri (rollio ~20°) | https://icas.org/ICAS_ARCHIVE/ICAS2002/PAPERS/564.PDF | media |
| VIK | Viking Systems, criteri STANAG 4154 (trimarano) | https://viking-systems.net/wp-content/uploads/2021/10/seakeeping_assessment_of_large_trimaran_for_naval_aircraft_operations.pdf | media |
| USCG | Guida d'esame USCG, *Heavy Weather Deck Handling* (fuga dell'elica) | https://open-exam-prep.com/study-guides/uscg-master-100/deck-maneuvering/heavy-weather-handling | media-bassa |
| TRBP | TRID, protezione del motore per l'emersione dell'elica | https://trid.trb.org/View/79576 | media |
| USN-ESSEX | Commons, USS Essex col mare grosso (1960) | https://commons.wikimedia.org/wiki/File:USS_Essex_(CVA-9)_underway_in_heavy_seas,_12_January_1960_(NH_98517).jpg | PD |
| PAT-CA | Brevetto US 9989358, sensore dell'angolo del cavo del verricello | https://patents.google.com/patent/US9989358 | media |
| ASWF | Flashcard ASW (cable angle hover, 70 ft, vento 17-21 kn) | https://brainscape.com/flashcards/asw-14176474/packs/21629195 | bassa |
| DELFT | TU Delft, tesi sul controllo del cavo del sonar a immersione in hovering | https://repository.tudelft.nl/file/File_c657add4-f4f6-43f0-8823-531e3b85500e | media |

**Lacune principali.**

- La sezione meteo del manuale di F-117A.
- Il contenuto meteo reale del broadcast ai sottomarini USA e URSS nel 1985-1990.
- Il gergo «dunking the scope».
- Finestre riscaldate su Type 18 e periscopi sovietici.
- Fulmini nei giochi navali VGA e in SH3, SH4, SH5 e Cold Waters.
- Struttura dell'idrometeo della VMF negli anni '80.
- Sub Command e Fleet Command.
- Periodi di rollio misurati per le singole classi (FFG-7, Spruance, Krivak, Udaloy).
- Le soglie numeriche dello STANAG 4154.
- Quota e cavo del Ka-27 col sonar a immersione.
- L'esistenza del «cable angle hover» sugli SH-3H e SH-60F negli anni '80.
- Come 688 Attack Sub disegna le navi nel periscopio: va verificato nell'EXE o in emulazione.
