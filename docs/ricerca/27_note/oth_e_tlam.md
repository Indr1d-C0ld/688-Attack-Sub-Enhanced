# Note 27 — OTH per il TASM e missioni TLAM: dati esterni, invecchiamento, area di lancio, autorità

> Appunti di ricerca (28/10/2026). Completano `05_missili_e_contromisure.md` §1.4, §2.3, §2.4 senza ripeterli:
> lì sono già trattati OUTLAW SHARK (6 minuti di latenza, ingresso a mano e poi diretto in Mk 117/CCS Mk 1),
> la catena OSIS→STT→SSIXS, il tempo di volo di 30 minuti, le statistiche di discriminazione [NG2], la
> pianificazione CMSA fino a 72 h, il «data transport device» e l'ora di preparazione [GAO95].
> Broadcast, SSIXS, VLF e quota periscopica sono in `08_comunicazioni_radio.md` e `10_radio_ruolo_in_partita.md`;
> la deriva DMINS/ESGN e i punti nave sono in `docs/08_fondali_e_navigazione.md` §6.
>
> Ricerca solo su pagine HTML aperte. Molte pagine USNI (403), GlobalSecurity (402) e catalyst.earth (403) non si
> sono aperte: dove un dato viene dal solo estratto del motore di ricerca lo dico. I PDF non sono stati scaricati e
> sono elencati in §7.3 come «da chiedere».
>
> Confidenza: **alta** = documento ufficiale o due fonti serie concordi; **media** = una buona fonte secondaria;
> **bassa** = fonte singola debole, forum, o solo un estratto di ricerca. **STIMA** = proposta di gioco motivata.

---

## 1. Catena OTH per il TASM

### 1.1 Gli anelli (dal sensore al tubo)

| Anello | Che cosa fa | Fonte | Conf. |
|---|---|---|---|
| Sensori ad area vasta | Satelliti ELINT Classic Wizard in orbita bassa (circa 700 miglia, orizzonte di circa 3.500 miglia di diametro), EP-3 e Classic Outboard per la **HULTEC** (correlazione emettitore–scafo: si lega un radar a una nave precisa) | [NSLOS] | media |
| Altri sensori | Radar OTH (copertura «well over a thousand miles»), SOSUS, radiogoniometria costiera, pattugliatori, e perfino pescherecci, mercantili e osservatori a terra | [NSLC2T] [BRF] | media |
| OSIS (FOSIC/FOSIF) | Nodi a terra con **correlazione delle tracce da tutte le fonti** dall'inizio degli anni '80; ricevono gli algoritmi di tracciamento di OUTLAW SHARK. L'OSIS è il processore dei FOSIC/FOSIF | [NSLBA] [GSTAD-e] | media |
| Shore Targeting Terminal (STT) | Al comando operativo dei sottomarini (SUBOPAUTH): **adatta i dati al singolo battello** e li passa a chi può portare le armi sul bersaglio | [NSLOS] | media |
| Trasporto | SSIXS (UHF satellitare) e ISABPS sul VLF; modo interrogazione | già in 08 §4.2 | — |
| Terminali di bordo | Terminali OUTLAW SHARK designati **USQ-81(V)**; il loro acquisto si fermò nel 1980. La funzione di correlazione doveva entrare in **TFCC**, **TWCS** e **CCS** dei sottomarini | [NSLBA] | media |
| Lato flotta (navi, non sottomarini) | **TADIXS** da terra alle navi, **OTCIXS** da nave a nave (dal comandante tattico), **TRE/TRAP** su una banda laterale UHF di FLTSATCOM; la **TFCC** usava il proprio processore (FDDS) per i dati OTH-T del comandante tattico; **JOTS** a bordo della *Forrestal* dal 1987 | [NSLBA] [FASRFS] [GSTAD-e] | media |
| Coordinamento | Il **FOTC** (*Force Over-the-horizon Track Coordinator*) tiene il quadro comune e lo passa ai sistemi di tiro Tomahawk e Harpoon (descrizione del 1997) | [FASOTH] | alta (per il 1997), bassa per il 1989 |

**Che cosa vede il sottomarino.** Il battello non è un nodo di OTCIXS/TADIXS come una nave. Riceve dalla STT un
sottoinsieme già adattato attraverso il broadcast e l'interrogazione SSIXS. Il proprio calcolatore correla poi quel
sottoinsieme con i contatti del sonar e prepara gli schemi di ricerca del TASM [NSLOS] [BRF] (media). Nel 1985 la
proposta della Naval Submarine League era che il **COMSUBLANT** (o un altro comando sommergibilistico a terra)
coordinasse l'impiego del TASM, e che il battello evitasse comunicazioni estese per non perdere la sorpresa [NSLC2T]
(media).

### 1.2 Evoluzione dei tempi

| Periodo | Ritardo e cadenza | Fonte | Conf. |
|---|---|---|---|
| Outlaw Hawk, 1972 (portaerei *Kitty Hawk*) | Ritardo tale da rendere i dati «useless except for post-exercise analysis»; messaggi persi o bloccati nel compartimento SI; formati incompatibili | [NSLBA] | media |
| Valutazione 1978 | Capacità OTH di scoperta, classificazione e designazione «seriously limited», soprattutto nell'identificazione delle navi e nella valutazione dei danni; navi e sottomarini non attrezzati | [GAO81] | alta |
| Epoca OUTLAW SHARK (fine anni '70) | **Circa 20 minuti o più** per consegnare i dati; aggiornamento di un contatto **una o al massimo due volte l'ora**. Picco di 6 minuti (già in 05) | [NSLBA] | media |
| Metà e fine anni '80 (TRE/TRAP) | Ritardo ridotto a **secondi**, «essentially real time», ma per le unità con TRE. Nel 1987 la flotta costruì un *Blue Force Locator* usando solo i rapporti di contatto TRAP/TRE | [NSLBA] | media |
| Dal sensore al tiratore, in generale | Da «diverse ore» a «di solito meno di un'ora» (già in 05) | [NG2] | media |

**Lettura per il 1989.** Per un SSN la latenza utile resta quella della catena STT→SSIXS: minuti nel caso migliore,
decine di minuti di norma, con un dato **nuovo solo una o due volte l'ora** per contatto. Il «tempo reale» del TRE
riguarda le navi e gli aerei attrezzati. Per il sottomarino non ho trovato una conferma (bassa).

### 1.3 Gridlock

Il **gridlock** è una funzione dell'NTDS e del Link 11: stima gli errori di registrazione fra le unità (posizione
propria e allineamento dei sensori). Le correzioni stimate si chiamano **pads**. Un articolo dell'APL descrive un
minicalcolatore fra l'NTDS e il terminale Link 11 che stima e corregge l'errore senza modificare nessuno dei due
sistemi [APLGRID-e] (media; dall'estratto, il PDF è da chiedere). Per il sottomarino il gridlock come procedura di
rete non c'è: **l'errore di registrazione è semplicemente l'errore della sua posizione stimata** (§6.1).

---

## 2. Formato e ingresso dei dati

### 2.1 OTH-T Gold (OTG): che cosa si sa

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Nome | *Operational Specification for Over-The-Horizon Targeting Gold* (OS-OTG). Scopo: trasmettere dati scelti fra sistemi OTH-T e sistemi di supporto, in forma «easily man readable» | [NISPOTG] | alta |
| Revisioni | Rev C del 1° agosto 1997, Change 1 dell'agosto 1998, poi Change 2. Le revisioni degli anni '80 non sono documentate in rete: nel 1989 esisteva una versione precedente | [NISPOTG] [NRLOTG-e] | alta (1997), bassa (contenuto 1989) |
| Struttura | Messaggi a formato di testo (MTF): ogni formato è una **sequenza fissa di set**. Ogni set ha un identificatore di 3–5 lettere (`MSGID`, `CTC`, `POS`…). `MSGID` è il primo set dopo la riga di classifica; il secondo campo di `MSGID` dice il tipo di formato. Il messaggio chiude con `ENDAT` | [NISPOTG] [NRLOTG-e] | media |
| Sintassi | Campi separati da `/`. I campi vuoti si possono lasciare vuoti ma **i separatori restano**. Righe di **69 caratteri al massimo**, nessun campo spezzato fra due righe | [CATOTHG-e] | media (estratto; variante canadese) |
| Rapporto di contatto | Almeno due righe: `CTC` identifica il contatto; `POS` dà **posizione, ora della posizione** e altre informazioni di traccia; `RMKS` per le note | [CATOTHG-e] | media |
| Variante «enhanced» | Formato `XCTC` (*Enhanced Contact Report*): coppie `CTC` + `XPOS`. In un esempio moderno: data-ora della posizione, lat/long, sorgente (`PHOTO`, `NTDS`…), rotta in gradi veri, velocità in nodi | [GHGOLD] | bassa (esempio generato da un programma moderno, non dalla specifica) |
| Uso | US DoD, SACLANT e molte marine NATO | [NISPOTG] | alta |
| Ellisse d'incertezza | **Non trovata** nelle fonti aperte la posizione dei campi dell'ellisse nel set `POS` | — | — |

**Nota.** Il dato importante del formato è che **l'ora della posizione** è distinta dall'ora del messaggio: è
quella a cui si riferisce l'ellisse, e da essa si calcola l'invecchiamento (§3).

### 2.2 Messaggio di gioco (ricostruzione, non copia della specifica)

STIMA. Formato in stile OTG per la sala radio e la carta, coerente con le regole note (set, `/`, 69 caratteri,
`ENDAT`). I campi dell'ellisse e della qualità sono un'invenzione plausibile:

```
MSGID/OTGCTC/STTNORVA/0147/OCT
CTC/T0457/KIROV/CGN/SUR/UR/HOS/ID:PROB
POS/281422Z/OCT/6912.4N/01255.8E/ELP:06.0/03.0/045T/
    COU:320T/SPD:24/SRC:ELINT
RMKS/HULTEC TOP STEER. ESCORTE 2 UDALOY NON CORRELATE
ENDAT
```

| Campo | Significato in gioco |
|---|---|
| `0147` | Numero di serie del messaggio: un buco nella numerazione indica un messaggio perso (08 §5.3) |
| `T0457` | Numero di traccia della STT, stabile fra un aggiornamento e l'altro |
| `ID:PROB` | Qualità dell'identificazione: `POS` (positiva, per esempio da HULTEC), `PROB`, `POSS`, `UNK` |
| `281422Z` | **Ora della posizione**, non della trasmissione |
| `ELP:06.0/03.0/045T` | Semiasse maggiore e minore in miglia (a 2σ o al 90%, da fissare) e orientamento dell'asse maggiore |
| `COU`/`SPD` | Rotta e velocità stimate; assenti se la sorgente è un singolo rilevamento |
| `SRC` | Sorgente: `ELINT` (satellite), `HFDF`, `SOSUS`, `MPA` (pattugliatore), `RADAR`, `VISUAL` |

### 2.3 Ingresso nel sistema di tiro (oltre a quanto già in 05 §1.4)

- Il calcolatore del battello **correla** il dato esterno con i propri contatti e prepara gli **schemi di ricerca**
  del TASM per un attacco immediato [NSLOS] [BRF] (media).
- La piattaforma **pianifica l'intero volo**: può dare rotte a gomito (*dogleg*) per un arrivo simultaneo e scegliere
  il percorso per evitare i neutrali [BRF] [NG2] (media/bassa).
- Il TASM si può anche sparare come «sveglia», per far accendere i radar di navi in silenzio e scoprirne la posizione
  per un secondo attacco [BRF] (bassa).
- **Mira al punto futuro.** La raccomandazione del 1985 era di mirare alla posizione stimata **all'arrivo**, perché gli
  errori restino dentro la spazzata del seeker; un tiro a 200 miglia su un bersaglio puntiforme costringe il missile a
  una lunga ricerca, che toglie sorpresa e lo rende più facile da ingannare o da abbattere. **Senza designazione
  esterna il TASM era utilizzabile solo sotto le 50 miglia** [NSLC2T] (media).
- **STIMA di gioco per l'ingresso:** con CCS Mk 1 o con un Mk 117 aggiornato, una traccia OTG si «aggancia» alla
  soluzione di tiro con un comando (5–10 s). Sugli altri sistemi si copia a mano dalla stampa della radio: 1–2 minuti,
  con una piccola probabilità di errore di battitura (per esempio il 2%, con lat/long sbagliate di una cifra) da
  scoprire confrontando la carta.

### 2.4 Che cosa deve fare il battello per riceverli

Quota periscopica, albero UHF e finestre del broadcast sono già nei dossier 08 e 10. Qui serve solo la conseguenza:
il dato OTH arriva **a pacchetti**, al broadcast o all'interrogazione, e ogni salita espone il battello. Proposta:
la STT ripete la traccia a ogni ciclo del broadcast di missione (10–15 minuti di gioco, doc 10 §1) **solo se ha un
dato nuovo**. Altrimenti il giocatore rivede la stessa posizione che invecchia.

---

## 3. Invecchiamento del dato

### 3.1 Che cosa dicono le fonti

- L'area d'incertezza cresce soprattutto con il **tempo trascorso**. Per armi subsoniche a lungo raggio può arrivare a
  **migliaia di miglia quadrate** [CIMDMO5] (media; analisi moderna, ma la geometria è la stessa).
- Il missile deve **arrivare nell'area e iniziare la ricerca prima che la nave ne esca**; l'analisi giusta è trovare la
  **latenza massima utilizzabile** dalla scoperta all'ingresso del dato [CIMMOS] (media).
- Il TASM a massima portata impiegava quasi 30 minuti, nei quali una nave veloce percorre **15–20 miglia** [HGF]
  (bassa; concorde con 05 §2.3). Il TASM poteva volare schemi di ricerca «expanding box», a serpentina, per coprire
  l'intera zona d'incertezza; la ricerca iniziava a circa 100 m di quota [HGF] (bassa).
- Nel 1975 l'APL sviluppò per il TASM schemi di ricerca in un'area d'incertezza, su richiesta del contrammiraglio
  Locke [NSLOS] [BRF] (media).
- Esiste una tesi NPS su un modello analitico di acquisizione di un'unità di alto valore, con il moto del bersaglio
  durante il volo fino all'inizio della ricerca attiva [NPSBEC] (solo il titolo e l'estratto; testo da chiedere).
- **Numeri ufficiali sull'area d'incertezza del TASM non ne ho trovati.**

### 3.2 Modello di gioco (STIMA)

Età del dato all'accensione del seeker:

`t = t_lat + t_bordo + t_volo`

- `t_lat` = età all'arrivo, cioè l'ora del messaggio meno l'ora della posizione. Proposta: 5–20 minuti dalla STT
  nel caso migliore, 30–60 minuti tipici, fino a 2 ore se la traccia non è stata aggiornata.
- `t_bordo` = dalla ricezione al lancio (decisione, preparazione, avvicinamento): 10–30 minuti.
- `t_volo` = distanza / circa 475 nodi (0,72 Mach, il valore di 05 §2.3). Per 200 miglia sono circa 25 minuti.

**Caso A, rotta e velocità note** (`COU`/`SPD` presenti). Ellisse allungata lungo la rotta:

- lungo la rotta: `σL(t) = √(σ0² + (σv·t)²)`
- di traverso: `σC(t) = √(σ0² + (V·t·σθ)²)`

con `σ0` = errore della sorgente (ELINT da satellite: 2 miglia; HFDF: 5–10; SOSUS: 10–20; MPA: 0,5–1),
`σv` = 2 nodi, `σθ` = 10°.

**Caso B, rotta ignota** (un solo rilevamento o un dato vecchio): cerchio di raggio `r(t) = 2σ0 + Vmax·t`. È la
crescita lineare del raggio dei modelli di ricerca, con area che cresce col quadrato del tempo mentre l'area spazzata
dal missile cresce solo linearmente [ricerca su modelli di bersaglio mobile, vedi §7.2].

**Esempio di riferimento:** Kirov a 24 nodi (massimo 30), sorgente ELINT, `t` = 30 + 15 + 25 = 70 minuti.

| Caso | Semiassi (1σ) o raggio | Area di ricerca (2σ) | Coperta dal TASM? |
|---|---|---|---|
| A | σL ≈ 3,1 mi; σC ≈ 5,3 mi | π·6,2·10,6 ≈ **205 mi²** | sì, ampiamente |
| B | r ≈ 4 + 35 = 39 mi | π·39² ≈ **4.800 mi²** | solo in parte |

**Capacità di ricerca del TASM (STIMA).** Area spazzata = lunghezza della ricerca × larghezza della fascia del seeker.
Con 173 miglia di ricerca (il volo del 1976) e una fascia di 20 miglia (10 per lato contro una grande nave, STIMA) si
coprono circa **3.500 mi²**. La lunghezza disponibile è il carburante meno la distanza fino al centro dell'area;
per il lancio da tubo il bilancio è ridotto del 30% (05 §2.2).

`P(il bersaglio è nell'area spazzata) ≈ min(1, area spazzata / area 2σ) × 0,95`

Poi entra la discriminazione (05 §2.3: il seeker prende la **prima** nave adatta che incontra lungo lo schema).

**Regole che ne seguono, utili al gioco:**

1. **Un dato con rotta e velocità vale molto di più di un dato fresco senza.** Il caso A a 70 minuti è più facile del
   caso B a 30 minuti.
2. **Ogni minuto conta in modo lineare sul raggio** (0,4–0,5 miglia al minuto contro un incrociatore): aspettare il
   prossimo broadcast per avere un dato fresco è una scelta reale.
3. **Più l'area è grande, più il missile cerca a lungo**: più tempo con il radar acceso, più probabilità che il
   bersaglio lo veda e reagisca, e più probabilità di prendere un'altra nave.
4. A schermo: l'ellisse OTG si **gonfia** sulla carta in tempo reale dall'ora della posizione, in un colore diverso
   dalle tracce TMA; il pannello del TASM mostra «età del dato all'accensione» e «copertura stimata».

---

## 4. TLAM: missione, area di lancio, allineamento

### 4.1 Come la missione arriva a bordo

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Chi pianifica | CMSA (Atlantico e Pacifico) con il **TMPC**: base dati, preparazione e **distribuzione** dei dati di volo, sia per il TLAM-N sia per il TLAM-C/D | [FASTMPC] [FASCMSA] | alta |
| Componenti del TMPC | DIWS (immagini), TLAM Planning System, **Mission Distribution System** (MDS: diffonde le missioni alle piattaforme di lancio e agli staff imbarcati), Precision Targeting Workstation | [FASTMPC] | alta (descrizione del 1998) |
| Anticipo | Rotte terrestri preparate **«weeks or months ahead of time»** per un bersaglio prescelto | [NSLGRU] | media (1987) |
| Supporto fisico | Dati di rotta su **nastri o dischi** inseriti nella guida del missile poco prima del lancio. Il «data transport device» è un **grande disco**; in alternativa la radio | [NSLGRU] [FASCMSA] | alta (due fonti) |
| Libreria | Esiste una **libreria di missioni**: il CMSA fa da archivio delle missioni TLAM [USNI90-e]; nei rapporti DOT&E compare una «Master Mission Library» (sistemi di superficie anni '90) [DOTE-e] | [USNI90-e] [DOTE-e] | bassa (solo estratti) |
| Tempo di pianificazione Block II | **24–80 ore** con dati TERCOM/DSMAC disponibili (72 h in Desert Storm, già in 05) | [FASCMSA] | alta |
| Pianificazione imbarcata | L'APS (pianificazione su portaerei) entra in servizio **solo nel 1996**: nel 1989 tutto si pianifica a terra | [FASTMPC] | alta |
| Interfaccia | Il TMPC parla con il TWCS (navi) via OTCIXS/TADIXS-A | [FASTMPC] | alta (anni '90) |
| «MDU» | Il termine *Mission Data Update* **non compare** nelle fonti aperte lette | — | — |

**Conseguenza per il 1989:** un 688 parte con un **pacchetto di missioni** pianificate a terra per bersagli e aree
di lancio prestabiliti. In pattuglia può ricevere per radio aggiornamenti o missioni nuove, ma con giorni di anticipo.
**Un bersaglio terrestre nuovo deciso in mare non è attaccabile in giornata** (alta, segue dai tempi di
pianificazione).

### 4.2 Area di lancio

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Punto di partenza fisso | Il pianificatore traccia la rotta «from a fixed starting point to the target» | [FASCMSA] | alta |
| Punto di lancio pianificato | Con i primi sistemi TERCOM l'intera rotta, **compreso il punto di lancio**, doveva essere pianificata; un lancio da un punto inatteso poteva far perdere il missile | [WPDSMAC] | bassa (affermazione senza nota) |
| Tratta sul mare | La nave **pianifica la tratta sul mare fino al confine terra–mare**, la inserisce e la **unisce** ai dati della tratta terrestre | [NSLGRU] | media |
| Terminologia (anni 2000) | **PLP** (*pre-planned launch position*); **FPPWP** (*first pre-planned waypoint*: qui si separano la tratta sul mare e quella terrestre) | [BRSCARD] | bassa (schede di studio) |
| Prassi osservata | Arrivato l'ordine con le missioni, gli ufficiali tracciano una **rotta di navigazione per portare la nave in posizione** per quelle missioni, tenendo conto anche del vento; poi la nave si riposiziona per i lanci successivi (2003) | [BH03] | media |
| Posizione d'attesa | Nave inviata a una «preplanned launch position» il giorno prima dello strike (2011) | [CIMCHE] | media |
| Dimensioni dell'area | **Non trovate** | — | — |

**Ricostruzione (media):** la missione del CMSA parte dal primo waypoint prepianificato. La tratta sul mare, dal punto
di lancio al waypoint, la calcola il sistema di tiro del battello entro limiti: carburante, rotta d'arrivo sulla prima
mappa TERCOM, ostacoli e zone da evitare. «Area di lancio» vuol dire quindi l'insieme dei punti da cui questa
tratta resta valida, non un cerchio arbitrario. Questo rende più precisa la STIMA di 05 §2.4 (§6.2).

### 4.3 Allineamento e navigazione

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Sequenza a bordo | Accensione, **allineamento dell'inerziale del missile**, trasferimento della missione, lancio | [FASCMSA] | alta |
| Guida del TLAM-N | **TAINS** (inerziale assistito da TERCOM, AN/DPW-23), circa **80 m CEP**; rotta prefissata con waypoint | [DS109] | media |
| TLAM-C | Stesso INS/TERCOM più DSMAC AN/DXQ-1, circa 10 m CEP; Block II con un solo modo d'attacco | [DS109] | media |
| Prime mappe TERCOM | «The first maps had large cells, to make it easy for the missile to locate itself»; le successive sono più piccole e più precise | [NG1] | media |
| Numero e spaziatura delle mappe | Dipendono dalla qualità dei dati del terreno **e dalla precisione dell'inerziale** | [FASFUN] | alta (manuale dell'Accademia navale) |
| Errori iniziali | Gli errori della guida inerziale **esistono già al lancio** e crescono in volo se non li riducono le misure di posizione lungo la rotta (Irani e Christ, APL 1994) | [APLIRANI-e] | media (citazione indiretta; PDF da chiedere) |
| Punto nave prima del lancio | Secondo un sommergibilista, un punto nave preciso **non era considerato essenziale** per il Tomahawk, ma aiutava a spingersi al limite della portata a pieno carburante | [ERF-e] | bassa (forum, solo l'estratto) |
| Quota periscopica | Un racconto divulgativo fa salire il battello a quota periscopica per allineare l'inerziale prima di sparare | [HR-e] | bassa |
| Deriva dell'INS del missile | **Non trovata** (Litton P-1000 non documentato in rete) | — | — |

**Meccanismo (ricostruzione, media).** Il CCS trasferisce all'inerziale del missile posizione, velocità e rotta del
battello presi dal SINS o dall'ESGN. **L'errore di posizione del battello diventa quindi, uno a uno, l'errore iniziale
del missile.** Sulla tratta sul mare non c'è nulla che lo corregga: il TERCOM non funziona sull'acqua [DS109]. La
prima mappa TERCOM al landfall deve essere abbastanza larga da contenere l'errore del battello più la deriva
accumulata sulla tratta sul mare. Da lì in poi il TERCOM azzera l'errore.

### 4.4 Tempi (dettaglio dell'ora già nota)

| Fase | Durata | Fonte | Conf. |
|---|---|---|---|
| Pianificazione a terra | 24–80 h (giorni o mesi prima) | [FASCMSA] [NSLGRU] | alta |
| Preparazione del missile a bordo (accensione, allineamento, missione) | **10–20 minuti** | [NSLGRU] | media (1987) |
| Dall'incarico al lancio, piattaforma con missione già a bordo | **circa 1 ora** (già in 05) | [GAO95] | alta |
| Programmazione dei missili su nave (2003) | «fino a un'ora», con verifica incrociata delle coordinate | [BH03] | media |
| Conto alla rovescia finale (2003) | 10 → 5 → 1 minuto → 30 s; lanci a coppie a circa 15 s l'uno dall'altro | [BH03] | media |
| Volo fino a 1.000 miglia | **2,5–3 ore**; tempo di reazione complessivo **circa 200 minuti** | [NSLGRU] | media |
| Riassegnazione del bersaglio | Il TERCOM non permette di ridestinare un missile in un minuto (era un obiettivo per il futuro) | [NSLGRU] | media |

**STIMA, cronologia di gioco dopo l'ordine di esecuzione:**

| Passo | Durata |
|---|---|
| Decodifica e verifica dell'ordine, scelta delle missioni dalla libreria | 5–10 min |
| Trasferimento nell'area di lancio | variabile (ore) |
| Punto nave a quota periscopica (Transit o Loran, doc 08 §6), facoltativo | 2–5 min più l'attesa del passaggio |
| Preparazione dei missili; si può sovrapporre al trasferimento negli ultimi 20 minuti | 15 min per il primo, poi 3–5 min per ciascuno in parallelo |
| Lancio (vincoli di quota e velocità in 05 §1.3) | — |
| Finestra di lancio rispetto all'ora pianificata | ±10 min (STIMA, per l'arrivo coordinato e il deconflitto con gli altri tiratori) |

---

## 5. Chi decide

| Decisione | Chi | Fonte | Conf. |
|---|---|---|---|
| Scegliere i bersagli terrestri e incaricare | Comandante unificato; il CMSA pianifica (già in 05) | [FASTom] | alta |
| Pianificare la tratta terrestre | CMSA, giorni o mesi prima | [FASCMSA] [NSLGRU] | alta |
| Ordinare il lancio (TLAM-C) | La catena di comando invia l'ordine con missioni e tempi. Nella prassi degli anni 2000: **LSP** (*Launch Sequence Plan*, elenco delle missioni possibili: solo pianificazione) e **«Indigo»** (l'LSP con tempi e missioni reali, che **autorizza** il tiro); per ogni unità, il messaggio d'incarico dice quale parte dello strike le spetta | [BRSCARD] [BH03] | bassa per i nomi nel 1989; media per il principio |
| Coordinare l'impiego del TASM | Proposta del 1985: COMSUBLANT o un comando a terra equivalente | [NSLC2T] | media |
| Che cosa decide il comandante | Il percorso fino all'area di lancio, la quota e il momento entro la finestra, la sicurezza del battello (rinviare se è scoperto), l'abilitazione del sistema e il «fuoco»; il **rinvio** o la **rinuncia** per motivi tattici con rapporto successivo. Nel 2003 era il comandante ad abilitare il sistema e a dare l'ordine finale («battery release») | [BH03] | media (nave di superficie) |
| Che cosa non decide | Il bersaglio terrestre, la missione (rotta, mappe), il numero di missili per bersaglio, l'arma nucleare | segue da [FASCMSA] | alta |
| Sistema di tiro | CCS Mk 1, CCS Mk 2 o AN/BSY-1 sui sottomarini | [FASTom] | alta |

### 5.1 TLAM-N: rilascio nucleare

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| PAL | **La Marina non usava PAL** su SLBM, armi nucleari dell'aviazione navale e missili da crociera lanciati da sottomarini, navi o aerei | [USNI88-e] [BELL] | alta (due fonti concordi) |
| Perché | Rischio di cattura basso in mare, nessuna custodia straniera, comunicazioni difficili con i sottomarini; un PAL si attivava solo per armi a terra | [BELL] | media |
| Controllo procedurale | Sui sottomarini il lancio richiede «an elaborate set of procedures» con altoparlante, diverse chiavi e la partecipazione di gran parte dell'equipaggio | [BELL] | media |
| Autenticazione (SSBN, prima del 1997) | L'**EAM** indica le forze, i bersagli e l'ora d'attacco coordinata. Contiene un valore **SAS** (*sealed authentication system*) che deve coincidere con il SAS custodito a bordo **in controllo a due persone**. Se il formato è giusto, la decodifica è corretta, il SAS coincide e il messaggio ha senso nel contesto, il comandante annuncia sul 1MC che il rilascio è ordinato e il secondo fa lo stesso sul telefono autoalimentato; il personale delle armi deve sentire **entrambi** | [NSLCT] | media (testimone diretto) |
| Applicazione al TLAM-N | Lo stesso autore dice che **procedure simili valgono per il TLAM-N** | [NSLCT] | media |
| Controllo d'uso | Sugli SSBN solo dal 1° ottobre 1997: non riguarda il 1989 | [NSLCT] | media |
| Che cosa fa il comandante | Non decide l'impiego: **verifica** che l'ordine sia autentico, che bersagli e tempi siano corretti, ed esegue | [NSLCT] [GSW80-e] | media |
| Servizio | TLAM-N schierato dal 1984; circa 350 in inventario nel 1990; ritirato da navi e SSN dopo l'annuncio del 27/9/1991 (sbarcato nel 1992) | [NSLSLCM] [NSLHIST] | alta |

**Per il gioco.** Il doc 14 propone di escludere il TLAM-N; resto d'accordo. Se lo si volesse come scenario
speciale: il TLAM-N è **selezionabile solo dopo un EAM autenticato**. È un minigioco di confronto del SAS fatto da
due ufficiali, con il doppio annuncio. Il giocatore **non sceglie il bersaglio** e un messaggio falso o
incoerente va riconosciuto. È anche un modo di rendere visibile l'«eseguire senza decidere».

---

## 6. Legame con la navigazione incerta e proposte di gioco

### 6.1 TASM: l'errore proprio si somma all'ellisse OTH

Le tracce OTG sono in **coordinate geografiche**. Il battello le converte in rilevamento e distanza relativi usando
la **posizione stimata**, quindi l'errore di navigazione (ellisse di doc 08 §6) sposta l'intera area di ricerca:

`Σ_ricerca = Σ_OTG(t) + Σ_nave`  (somma delle covarianze)

Esempio: DMINS (1,5 mi al giorno) a tre giorni dall'ultimo punto nave porta circa 4,5 mi a 1σ, cioè **più dello
stesso errore ELINT**. Il caso A di §3.2 passa da circa 3×5 mi a circa 5,4×7 mi. Per i bersagli presi con il proprio
sonar l'errore proprio **non conta** (la geometria è relativa). È un motivo concreto, e leggibile in gioco, per fare
un punto nave prima di un tiro OTH. L'ESGN dei Flight II (0,5 mi al giorno) qui vale davvero.

### 6.2 TLAM: validità della missione e probabilità di prendere la prima mappa

**Errore al landfall (STIMA):**

`σ_LF = √( σ_nave² + (d_drift · t_mare)² + (D_mare · ε_rotta)² )`

- `σ_nave` = errore di posizione **vero** del battello al lancio (dal modello di doc 08 §6, mai mostrato al giocatore);
- `d_drift` = deriva dell'INS del missile: **0,5 mi/h** (STIMA: dato ignoto; un inerziale da missile anni '80 è peggiore
  di un SINS ma il volo sul mare dura pochi minuti);
- `t_mare` = durata della tratta sul mare (`D_mare` / 475 nodi);
- `ε_rotta` = errore di rotta trasferito: 0,05° con ESGN, 0,1° con DMINS (STIMA).

Con 150 miglia sul mare (19 minuti) i due termini del missile valgono circa 0,16 e 0,13–0,26 mi: **domina l'errore
del battello**, come diceva il sommergibilista [ERF-e].

**Probabilità di aggancio della prima mappa TERCOM:** con un errore circolare gaussiano e una semilarghezza utile `H`
della prima mappa,

`P_LF = 1 − exp( −H² / (2·σ_LF²) )`

Proposta `H` = **2,5 miglia** (STIMA: «prime mappe a celle grandi» [NG1]; nessuna dimensione pubblica).

| σ_LF (1σ) | P_LF | Situazione tipica |
|---|---|---|
| 0,5 mi | ≈ 1,00 | Punto nave Transit o Loran nelle ultime ore |
| 1,0 mi | 0,96 | ESGN a 2 giorni dal punto nave |
| 1,5 mi | 0,75 | DMINS a 1 giorno, oppure punto batimetrico su una carta mediocre |
| 2,0 mi | 0,54 | DMINS a 1,5 giorni |
| 3,0 mi | 0,29 | Inerziale guasto, stima con solcometro e girobussola |

Un missile che manca la prima mappa **continua in inerziale**. Con poche mappe successive (terreno piatto, 05 §2.5)
può ritrovarsi; altrimenti si perde o cade lontano. Proposta: senza aggancio, una seconda possibilità alla mappa
successiva con `H` dimezzato, poi «missile perso» con un punto d'impatto casuale lungo la rotta (danni collaterali da
registrare nel debriefing).

**Validità della missione (STIMA, da mostrare nel pannello Tomahawk):**

| Stato | Condizione | Effetto |
|---|---|---|
| **VALIDA** | Posizione stimata entro 10 mi dalla PLP, rotta d'arrivo alla prima mappa entro ±20° di quella pianificata, carburante con un margine ≥ 10% | Lancio normale |
| **RIPIANIFICA** | Tra 10 e 40 mi dalla PLP | Il CCS ricalcola la tratta sul mare (+10 min); margine di carburante ridotto; avviso «fuori area di lancio» |
| **NON VALIDA** | Oltre 40 mi, rotta d'arrivo fuori tolleranza, oppure la tratta attraversa terra o una zona proibita | Il sistema rifiuta il lancio |
| **FUORI FINESTRA** | Ora di lancio fuori di ±10 min | Lancio possibile solo dopo un'autorizzazione via radio (deconflitto); altrimenti è un'infrazione delle ROE nel debriefing |

**Il punto chiave da rendere in gioco:** il pannello calcola validità e `P_LF` sulla posizione **stimata** e
mostra `P_LF` stimata dall'ellisse dichiarata. Il risultato vero usa l'errore **vero**. Un equipaggio «sicuro di sé e
sbagliato» (punto batimetrico su una carta errata, doc 08 §6) vede «VALIDA, 96%» e manca il landfall. Il replay
«verità» del debriefing mostra perché. Rischio e rimedio sono tutti già nel modello di navigazione: **un punto nave
prima del lancio costa un'esposizione** (quota periscopica, albero, ESM nemico) e **compra probabilità di successo**.

### 6.3 Altre proposte

1. **Libreria delle missioni a bordo** (briefing pre-missione): elenco «Missione 03 – PLP 57°40'N 008°10'E – bersaglio
   cifrato – TOT 0412Z ±10». Il giocatore sceglie il percorso, non il bersaglio.
2. **Ordine di esecuzione** con un nome di codice (stile «Indigo») ricevuto al broadcast: senza ordine i missili si
   possono preparare ma non lanciare.
3. **TASM e dato OTH:** sul pannello l'età del dato all'accensione del seeker (`t`) e la copertura stimata; un
   avviso se il caso è B (rotta ignota).
4. **IA simmetrica** (memoria «simmetria dell'IA»): un SSGN o un Echo II sovietico che usa dati di bersaglio esterni
   subisce lo stesso invecchiamento. Con il Tu-95RTs può però aggiornare il missile in volo (05 §2.3), e la sua area
   resta piccola.

---

## 7. Fonti (consultazione 28/10/2026)

### 7.1 Lette (pagine HTML)

| Sigla | Titolo | URL |
|---|---|---|
| [NSLOS] | J. Holland, *OUTLAW SHARK 1: The Beginning of Third Party Targeting at Sea*, Submarine Review, inverno 2012 (già in 05) | https://archive.navalsubleague.org/?p=5196 |
| [NSLBA] | R. L. Hess, J. Holland, *Before and After Outlaw Shark*, Submarine Review, estate 2012 | https://archive.navalsubleague.org/?p=5205 |
| [NSLC2T] | W.J.R. (W. J. Ruhe), *C2T of the Antiship Tomahawk Missile*, Submarine Review, gennaio 1985 | https://archive.navalsubleague.org/?p=912 |
| [NSLGRU] | W. P. Gruner, *A Weapon System for the Submarine Tactical Land-Attack Mission*, Submarine Review, aprile 1987 | https://archive.navalsubleague.org/1987/a-weapon-system-for-the-submarine-tactical-land-attack-mission |
| [NSLSLCM] | J. Boyes, W. J. Ruhe, *The Submarine SLCM Problem*, Submarine Review, gennaio 1990 | https://archive.navalsubleague.org/1990/the-submarine-slcm-problem |
| [NSLHIST] | W. J. Holland Jr., *A Short History of Submarines in Land Attack*, Submarine Review, aprile 2009 | https://archive.navalsubleague.org/2009/a-short-history-of-submarines-in-land-attack |
| [NSLCT] | M. Lyman (JHU/APL), *Crimson Tide: They Got It All Wrong*, Submarine Review, aprile 1999 | https://archive.navalsubleague.org/?p=3384 |
| [NSLCUR] | D. A. Curran, *A Brief History of the Push-Pull of Submarine Combat Control Systems*, gennaio 1993 (solo per il contesto: l'OTH-T metteva sotto sforzo i calcolatori del CCS) | https://archive.navalsubleague.org/?p=2022 |
| [FASCMSA] | FAS IRP, *Cruise Missile Support Activity* (già in 05) | https://irp.fas.org/agency/dod/usjfcom/cmsa/index.html |
| [FASTMPC] | FAS, *Tomahawk Mission Planning Center / Afloat Planning System* | https://man.fas.org/dod-101/sys/ship/weaps/tmpc-aps.htm |
| [FASTom] | FAS, *BGM-109 Tomahawk* (già in 05) | https://man.fas.org/dod-101/sys/smart/bgm-109.htm |
| [FASTWCS] | FAS, *Tomahawk Weapon Control System* | https://man.fas.org/dod-101/sys/ship/weaps/twcs.htm |
| [FASFUN] | FAS (testo dell'Accademia navale), *Fundamentals of Naval Weapons Systems*, cap. *Guidance and Control*, §16.5.3 | https://man.fas.org/dod-101/navy/docs/fun/part15.htm |
| [FASOTH] | FAS, bilancio FY98 della Marina, PE 0604707N, progetto X0798 *OTH Targeting* | https://irp.fas.org/budget/fy98_navy/0604707n.htm |
| [FASRFS] | FAS, *Ready-for-Sea Handbook*, modulo 8 (sistemi informatici d'intelligence: TADIXS, OTCIXS, OTH-T) | https://irp.fas.org/doddir/navy/rfs/part08.htm |
| [NISPOTG] | NISP, scheda dello standard *us-dod-oth-g* (OS-OTG Rev C) | https://nisp.nw3.dk/standard/us-dod-oth-g.html |
| [GHGOLD] | luk-kop, *OTH-T GOLD Tracks Generator* (esempio di messaggio XCTC/XPOS) | https://github.com/luk-kop/gold-tracks-generator |
| [DS109] | designation-systems.net, BGM/UGM-109 (già in 05) | https://designation-systems.net/dusrm/m-109.html |
| [NG1] [NG2] | Naval Gazing, *Tomahawk Part 1/2* (già in 05) | https://www.navalgazing.net/Tomahawk-Part-1 ; https://www.navalgazing.net/Tomahawk-Part-2 |
| [GAO95] | GAO/NSIAD-95-116 (già in 05) | https://www.govinfo.gov/content/pkg/GAOREPORTS-NSIAD-95-116/html/GAOREPORTS-NSIAD-95-116.htm |
| [GAO81] | GAO C-MASAD-81-11, *Issues Affecting the Navy's Antiship Cruise Missile Programs*, 28/2/1981 (sintesi) | https://legistorm.com/reports/view/gao/9437/Issues_Affecting_the_Navy_s_Antiship_Cruise_Missile_Programs.html ; https://www.gao.gov/products/c-masad-81-11 |
| [WPDSMAC] | Wikipedia, *TERCOM* (redirect da DSMAC), sezione *Disadvantages* | https://en.wikipedia.org/wiki/DSMAC |
| [WPOTCIXS] | Wikipedia, *OTCIXS* (abbozzo) | https://en.wikipedia.org/wiki/OTCIXS |
| [BELL] | S. Bellovin, *Permissive Action Links* (Columbia University) | https://www.cs.columbia.edu/~smb/nsam-160/pal.html |
| [BH03] | USS *Bunker Hill* (CG-52), libro della crociera 2003, p. 11 (racconto giornalistico dello strike del 20/3/2003) | https://www.e-yearbook.com/yearbooks/Bunker_Hill_CG_52_Cruise_Book/2003/Page_11.html |
| [CIMCHE] | M. Chess, *Countdown to Launch*, CIMSEC 2014 (da *Surface Warfare*, 2011) | https://cimsec.org/countdown-to-launch/ |
| [CIMDMO5] | D. Filipoff, *Fighting DMO, Pt. 5*, CIMSEC, 20/3/2023 | https://cimsec.org/fighting-dmo-pt-5-missile-salvo-patterns-and-maximizing-volume-of-fire/ |
| [CIMMOS] | D. Mosier, *A System of Systems Analysis is Needed for Maritime Strike*, CIMSEC, 21/8/2025 | https://cimsec.org/a-system-of-systems-analysis-is-needed-for-maritime-strike/ |
| [BRSCARD] | Brainscape, schede *STRIKE* (PLP, FPPWP, LSP, Indigo, ESP) | https://brainscape.com/flashcards/strike-4383922/packs/6532081 |
| [HGF] | HarpGamer Forum, *BGM-109 Tomahawk intended use* (moderatore CV32) | https://harpgamer.com/harpforum/topic/2580-bgm-109-tomahawk-intended-use/ |
| [BRF] | Bharat Rakshak Forum, messaggio di TSJones del 23/9/2015 che cita NavSource 01/57s1 (TASM, OUTLAW SHARK) | https://forums.bharat-rakshak.com/viewtopic.php?p=1905514 |
| [GLNAVY] | Gulflink, *United States Navy in Desert Shield/Desert Storm*, p. A-16 (Louisville, 19/1/1991) | https://gulflink.health.mil/histories/db/navy/usnavy_098.html |

### 7.2 Note solo da estratti di ricerca (pagina non aperta: 403, 402 o DNS)

| Sigla | Titolo | URL | Motivo |
|---|---|---|---|
| [CATOTHG-e] | PCI Geomatics, *OTHG vector file* (regole del rapporto di contatto CTC/POS) | https://catalyst.earth/catalyst-system-files/professional-help/references/gdb_r/gdb3N257.html | 403 |
| [GSTAD-e] | GlobalSecurity, *TADIXS* (OSIS processore dei FOSIC, FDDS della TFCC) | https://www.globalsecurity.org/intell/systems/tadixs.htm | 402 |
| [GSW80-e] | GlobalSecurity, *W80* (ordine codificato, controllo a due persone, verifica del comandante) | https://www.globalsecurity.org/wmd/systems/w80.htm | 402 |
| [DOTE-e] | DOT&E FY1997/98/2000, *Tomahawk* (Master Mission Library, LCG) | https://www.globalsecurity.org/military/library/budget/fy2000/dot-e/navy/00tomahawk.html | 402 |
| [USNI88-e] | *Who Needs PALs?*, Proceedings, luglio 1988 | https://www.usni.org/magazines/proceedings/1988/july/who-needs-pals | 403 |
| [USNI90-e] | *Tomahawk Equals True Value*, Proceedings, settembre 1990 | https://www.usni.org/magazines/proceedings/1990/september/tomahawk-equals-true-value | 403 |
| [ERF-e] | Early-Retirement.org, messaggio di un ex sommergibilista sul punto nave prima del Tomahawk | https://www.early-retirement.org/goto/post?id=1028347 | 403 |
| [HR-e] | HistoryRise, *How the U.S. Navy deployed the Tomahawk during the Gulf War* | https://historyrise.com/article/how-the-u-s-navy-deployed-the-tomahawk-cruise-missile-during-the-gulf-war/ | non aperta; fonte debole |
| [APLGRID-e] | JHU/APL Technical Digest, articoli di Miller sul gridlock (V02-N04, V11-N1-2) | https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V02-N04/02-04-Miller.pdf | PDF |
| [APLIRANI-e] | G. B. Irani, J. P. Christ, *Image Processing for Tomahawk Scene Matching*, JHU APL Tech. Digest 15(3), 1994 | https://www.jhuapl.edu/techdigest/Content/techdigest/pdf/V15-N03/15-03-Irani.pdf | PDF |
| [NRLOTG-e] | NCTSI, OS-OTG Rev C Change 2 (estratto del primo capitolo) | https://science.nrlmry.navy.mil/atcf/docs/database/new/jotsfile.format.pdf | PDF |
| — | NavSource, pagina 01/57s1 (fonte citata da [BRF]) | https://www.navsource.org/archives/01/57s1.htm | DNS non risolto |
| — | Modelli di ricerca su bersaglio mobile (raggio = velocità massima × tempo; area quadratica contro spazzata lineare): estratti di DTIC ADA273215 e NDIA 2005 (Silvia) | https://apps.dtic.mil/sti/pdfs/ADA273215.pdf ; https://ndia.dtic.mil/wp-content/uploads/2005/psts/silvia.pdf | PDF |

### 7.3 Da chiedere (PDF o pagine bloccate che varrebbe la pena leggere)

1. **OS-OTG** (NCTSI, Rev C Change 2): l'unico modo per avere i campi veri di `POS` e dell'ellisse —
   https://science.nrlmry.navy.mil/atcf/docs/database/new/jotsfile.format.pdf
2. **Irani e Christ 1994** (APL): errori dell'inerziale al lancio, scelta delle scene —
   https://www.jhuapl.edu/techdigest/Content/techdigest/pdf/V15-N03/15-03-Irani.pdf
3. **Miller, gridlock** (APL Tech. Digest 2(4) e 11(1-2)) — https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V02-N04/02-04-Miller.pdf ;
   https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V11-N1-2/11-01-Miller.pdf
4. **GAO C-MASAD-81-11** completo (OTH-DC&T) — https://gao.gov/assets/c-masad-81-11.pdf
5. **DTIC ADA181664** (OTCIXS) — https://apps.dtic.mil/sti/pdfs/ADA181664.pdf
6. **Tesi NPS di K. J. Becker**, *An analytical high value target acquisition model* — https://calhoun.nps.edu/handle/10945/21667
7. **USNI Proceedings** (pagine bloccate, da leggere a mano): L. Cuccias, *OTH Targeting: Out of Sight, Out of Mind*
   (ottobre 1985); *Tomahawk Tactics: The Midway Connection* (giugno 1992); *Who Needs PALs?* (luglio 1988);
   *Tomahawk Equals True Value* (settembre 1990); *Cruise Missile Warfare* (ottobre 1985).
8. **N. Friedman**, *Network-Centric Warfare* (Naval Institute Press, 2009): capitoli su OSIS, OUTLAW SHARK e OTH-T.
   È un libro, non una pagina aperta.

---

## 8. Lacune rimaste

- **Campi del set `POS`** e codifica dell'ellisse nell'OTG del 1989: non trovati; il messaggio in §2.2 è una
  ricostruzione.
- **Latenza STT→SSN nel 1989** e cadenza degli aggiornamenti per i sottomarini: solo valori dell'epoca di OUTLAW
  SHARK e il «tempo reale» del TRE per le navi.
- **Dimensioni dell'area di lancio**, larghezza della prima mappa TERCOM, deriva dell'INS del missile: nessun dato
  aperto; tutto STIMA.
- **Nomi dei messaggi d'incarico del 1989** (LSP e «Indigo» sono attestati solo per gli anni 2000).
- **Ingresso delle missioni via SSIXS** su un 688 nel 1989: possibile per la FAS («radio»), ma non documentato per i
  sottomarini.
- **Larghezza della fascia di ricerca del seeker** del TASM e geometria esatta dell'«expanding box».
