# 10 — La sala radio in partita: perché esiste e cosa cambia (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. SSN-688 *Los Angeles* Flight I contro SSN sovietici, soprattutto Pr.705 *Alfa*.
> Scopo: capire **come la radio decide una pattuglia vera** e tradurlo in meccaniche di gioco che cambiano le scelte
> del giocatore. Viene **prima** della scrittura dei messaggi delle missioni (`docs/11_comandi_e_stazioni.md` §11, S4).
> Estende il dossier tecnico `docs/ricerca/08_comunicazioni_radio.md` (bande, VLF/ELF, TACAMO, antenne, SSIXS,
> precedenze, radiogoniometria): qui non si ripetono quei numeri, si rimanda alle sue sezioni (es. «08 §9.5»).
> Compilato: 24 ottobre 2026.

---

## 0. Convenzioni e un problema di scala

**Confidenza** (stesse regole del dossier 08 §0):
- **[alta]**: fonte ufficiale o declassificata, oppure ≥2 fonti indipendenti serie concordi.
- **[media]**: una sola fonte secondaria seria.
- **[bassa]**: fonte debole, stima del compilatore, o dato **non riverificato** in questa sessione (*n.v.*).

**STIMA** = valore proposto per il gioco, non un dato storico. **PROPOSTA** = scelta di design.

**Sigle delle fonti** tra parentesi quadre. Quelle già definite nel dossier 08 (`[FASVLF]`, `[NSL99]`, `[NRTRAF]`,
`[FASSSIXS]`, `[SCMP]`, `[WPBOR]`, `[NSA399]`, `[WPZEVS]`, `[RUSNAVY]`…) mantengono lo stesso significato; le nuove
sono nella §8. «estratto» = letto solo dal riassunto di una ricerca, pagina non aperta per intero.

**Il problema della scala dei tempi.** La pattuglia vera vive su cicli di ore e giorni: broadcast VLF di **2 ore**
(08 §2.2), copia del broadcast «un paio di volte al giorno» in transito `[NSL05]`, ordini che arrivavano in **12–24
ore** `[NSL05]`. Le missioni del gioco durano molto meno: TORPEX finisce a 900 s, SHAKE 'EM a 1200 s, MUMAR CADAVER a
2700 s, SURPRISE PARTY intorno ai 4000 s (timer di fine in `data/missioni/SCENE*.lua`). Con i parametri di oggi del
nucleo (`ciclo_s = 7200`, finestre ogni 12 ore) in quasi tutte le missioni **non andrebbe in onda nemmeno un ciclo
intero**: la sala radio resterebbe decorativa.

**PROPOSTA di base: orologio radio compresso per missione.** Il rapporto fra i tempi resta quello vero, la scala no:
- ciclo del broadcast di missione **10–15 min** (STIMA), con le stesse ripetizioni per precedenza del nucleo (3/3/2/1);
- età delle informazioni d'intelligence **10–30 min** invece di ore;
- sessioni obbligatorie sovietiche ogni **10–20 min** invece di due al giorno.

Ha un precedente diretto: *Fast Attack* (Sierra, 1996) manda un broadcast **ogni 15 minuti** e dichiara che i dati
arrivano vecchi di 15–29 minuti `[FAMAN]` [alta, sul gioco]. Il nucleo lo permette già: `ParametriRadio::ciclo_s`
esiste, manca solo che la missione lo imposti (§5).

---

## 1. Da cosa dipende per radio una pattuglia di SSN (anni '80)

### 1.1 Il ritmo: copiare il broadcast e il costo di salire

- **Ricevere è continuo, trasmettere è raro.** Il battello in immersione può solo ricevere (VLF/LF col filo
  galleggiante, ELF dove c'è); per parlare deve esporre un'antenna a quota periscopica (08 §1, §4.4) [alta].
- **Ritmo tipico.** In transito il battello copia il broadcast VLF un paio di volte al giorno. Si collega in modo
  attivo solo se gli viene ordinato o se ha qualcosa d'importante da dire `[NSL05]` [media].
- **Latenza del comando.** Gli ordini potevano metterci **12–24 ore** ad arrivare. Era accettabile perché le missioni
  erano in gran parte indipendenti `[NSL05]` [media].
- **Il costo vero è la manovra, non il tempo radio.** Per un ciclo di comando stretto il battello deve rallentare e
  restare a quota periscopica con l'antenna alzata. Perde agilità; l'autore chiama la mancanza di comunicazioni a
  velocità e quota «perhaps the submarines most critical shortcoming» `[NSL05]` [media].
- **Prima dei sistemi moderni** una sosta per le comunicazioni durava «di solito un'ora o più»; prima dell'SSIXS si
  poteva restare ore a quota periscopica per copiare il VLF a 50 baud `[NSL99]` [media]. Le soste dovevano essere
  **brevi e rare** a quota periscopica, oppure **lente e solo in ricezione** in profondità `[NSL99]` [alta].
- **Il filo galleggiante** obbliga ad andare piano e dritti (08 §3.4) [alta]. Un articolo divulgativo ricorda che il
  cavo limita l'agilità del battello `[SCIAM]` [media].

**Lettura per il gioco.** La scelta che la radio impone non è «leggo o non leggo il messaggio». È: **rallento e salgo
(perdo posizione, contatto, tempo, rischio di essere visto) oppure resto giù e combatto con ordini e dati vecchi?**

### 1.2 Tipi di messaggio in arrivo

| Tipo | Contenuto | Precedenza tipica | Effetto sulla pattuglia | Fonte | Conf. |
|---|---|---|---|---|---|
| Ordine d'operazione / compito | Area, missione, durata, vincoli | IMMEDIATE / PRIORITY | Definisce cosa si fa; gli aggiornamenti cambiano area o bersaglio | `[NSL05]` `[WPSCORP]` | media |
| Notifica di movimento (*submarine notice*) e zone dello spazio d'acqua | Rotta pianificata con zona mobile protetta (*moving haven*), aree d'azione | PRIORITY | Dove si può sparare su contatti subacquei e dove no | `[AAP6HAV]` `[AAP6SAA]` | alta (definizioni NATO) |
| Cambio delle regole d'ingaggio | Stato delle armi, categorie di bersagli autorizzate | IMMEDIATE / FLASH | Cambia chi si può attaccare | prassi; `[WPWT]` per gli stati delle armi | media |
| Intelligence e contatti (SOSUS, P-3, satelliti, HF/DF) | Area di probabilità, rotta e velocità stimate, età del dato | PRIORITY / IMMEDIATE | Porta il battello verso un'area di ricerca, non su un bersaglio | `[COTE]` `[WPSOSUS]` | alta |
| Puntamento su un bersaglio (*cueing*) | Vettore d'intercettazione, area SOSUS | IMMEDIATE | SOSUS indirizza gli SSN a intercettare | `[COTE]` | alta |
| Avviso di minaccia | Unità ASW nemiche in arrivo, MPA in zona | FLASH / IMMEDIATE | Cultura: si va giù e ci si allontana dal *datum* subito, senza chiedere conferme | `[NSL05]` | media |
| Richiamo, deviazione | Interrompere, cambiare missione, rientrare | IMMEDIATE | Ramo di missione | `[WPSCORP]` (deviazione) | media |
| Meteo e ambiente | Stato del mare, ghiaccio, previsioni | ROUTINE | Esposizione dell'albero, rumore ambiente | prassi | bassa (*n.v.*) |
| Compiti speciali (forze speciali, sorveglianza) | Appuntamenti, finestre | PRIORITY | Vincoli di tempo e luogo | prassi | bassa (*n.v.*) |
| Emergency Action Message | Ordini nucleari (forze strategiche) | FLASH OVERRIDE | Per un SSN quasi solo ascolto; cornice di guerra generale | 08 §2.3, §5.1 | media |

**Formato dei dati di contatto.** I dati SOSUS arrivavano alla flotta in un messaggio a formato rigido, **RAINFORM**,
che nascondeva la fonte ed era spesso difficile da interpretare (sostituito nel 1997) `[WPSOSUS]` [media].
Per il gioco: un messaggio di contatto è un blocco di numeri (ora, posizione, ellisse, rotta, velocità, classe
stimata), da leggere e riportare sulla carta. È proprio il lavoro che la sala radio deve dare.

**Precisione dei dati SOSUS.** SOSUS indicava **aree di probabilità**, a volte molto grandi, soprattutto lontano dagli
sbarramenti. L'unità tattica doveva poi cercare e riacquisire il contatto. I falsi allarmi erano frequenti: mercantili,
fauna marina `[COTE]` [alta]. I fasci di un array erano larghi circa 2–5°; due array davano una posizione
«approssimata» per triangolazione `[WPSOSUS]` [media]. Numeri sulle dimensioni delle aree: non trovati in fonti aperte.

### 1.3 Gestione dello spazio d'acqua (WSM) e prevenzione dell'interferenza reciproca (PMI)

- **Due concetti.** La PMI evita collisioni sommerse fra battelli amici, schiere trainate e altri ostacoli `[MHSPMI]`
  (estratto) [media]. La WSM evita che un sottomarino amico venga attaccato per errore; è una misura di guerra. Nella
  Seconda guerra mondiale due sottomarini USA andarono persi per fuoco amico `[USNI10]` (estratto) [media].
- **Definizioni NATO (AAP-6):**
  - *submarine action area*: area della WSM con uno o più sottomarini amici; lì **solo loro** possono usare armi
    antisommergibile `[AAP6SAA]` [alta];
  - *ASW free area*: area senza sottomarini amici; armi antisommergibile **senza restrizioni** `[AAP6ASWFA]` [alta];
  - *submarine haven*: area fissa per operazioni o addestramento, **oppure zona mobile** istituita da una notifica
    di movimento (*submarine notice*) attorno ai battelli in transito; si estende per un numero dato di miglia
    avanti, dietro e ai lati della rotta pianificata `[AAP6HAV]` [alta].
- **Chi decide.** Le *Submarine Operating Authorities* (SUBOPAUTH) ricevono gli avvisi delle attività pianificate e
  deconflittano i movimenti subacquei; una procedura WSM permanente esiste dagli anni '60 `[HYDRO]` (estratto) [media].
- **Il nodo delle comunicazioni.** Pianificare i movimenti è difficile proprio perché i sottomarini comunicano poco
  `[SYSWSM]` (estratto, fonte commerciale) [bassa]. Un battello che ha perso l'aggiornamento della propria area
  non sa dove può sparare.

**Lettura per il gioco.** Zone sulla carta con regole d'arma, che si muovono nel tempo e arrivano **per radio**. Se
il messaggio si perde, la carta del giocatore è sbagliata. È il modo più naturale di legare radio e fuoco amico.

### 1.4 Messaggi persi: numeri di serie e recupero

Il dossier 08 §5.3 copre la numerazione progressiva, il ciclo che ripete i messaggi e la richiesta via SSIXS. Due
aggiunte:
- **Precedente storico molto leggibile.** Gli U-boot della Seconda guerra mondiale ricevevano messaggi numerati in
  sequenza e ripetuti più volte al giorno. Le ripetizioni notturne si potevano copiare in immersione fino a circa 30 m.
  **Ogni battello era responsabile** di recuperare i numeri mancanti `[NOVAUB]` (estratto) [media].
- **Responsabilità.** Nella prassi della flotta è il destinatario che deve accorgersi del buco e chiedere la
  ripetizione (08 §5.3, `[NRTRAF]`) [alta per le navi]. Per il gioco: **il comando giudica con gli ordini trasmessi,
  non con quelli ricevuti** (PROPOSTA, §4.2).

### 1.5 Rapporti in uscita e loro costo

| Rapporto | Quando | Costo | Fonte | Conf. |
|---|---|---|---|---|
| Rapporto di contatto | Contatto importante, soprattutto se serve aiuto o se cambia il quadro | Salire a quota periscopica; emissione intercettabile e localizzabile | 08 §7, `[NSL05]` | alta |
| Rapporto d'attacco / di situazione | Dopo un'azione, o su ordine | Come sopra | 08 §9.7 | bassa (*n.v.*) |
| Conferma di ricezione | Se l'ordine la chiede | Come sopra, ma breve | 08 §9.7 | bassa |
| Rapporto di fine pattuglia | Al rientro, a voce e per iscritto | Nessuno (in porto) | prassi | bassa |
| Boa SLOT (*Submarine-Launched One-way Transmitter*) | Messaggio registrato, lanciato in immersione; trasmette **dopo un ritardo** | Una boa; nessuna antenna esposta; il punto di trasmissione si allontana dal battello | `[WPSLOT]` `[NSL4604]` | media |

- **Le emissioni radio** sono tra i segnali del sottomarino più facili da rilevare e localizzare; devono essere brevi
  e a bassa probabilità d'intercettazione `[NSL05]` [media]. Modello DF del gioco: 08 §9.5, già nel nucleo.
- **SLOT.** Durante la Guerra fredda le boe SLOT mandavano messaggi registrati fino a **5 minuti** agli aerei di
  pattugliamento vicini, su canali VHF delle boe sonore `[NSL4604]` (estratto) [media]. L'AN/BRT-1 (Sippican) costava
  572 dollari nel 1982 `[WPSLOT]` [media]. I modelli moderni hanno ritardi programmabili fino a 99 ore `[WPSLOT]`;
  il ritardo dell'AN/BRT-1 è *n.v.* Per il gioco è un'alternativa con altri compromessi: niente albero, messaggio
  ritardato, sola andata, solo verso un aereo amico entro portata VHF.
- **Il problema SSN–aerei.** Le tattiche sottomarino–aereo erano sempre state ostacolate da problemi di
  comunicazione e d'identificazione amico/nemico `[COTE]` [alta]. Nel 1987 si scriveva che gli SSN di scorta ai
  gruppi portaerei restavano spesso fuori posizione perché non comunicavano in tempo reale. Dopo un contatto dovevano
  salire alla quota di comunicazione o usare una boa `[USNI87]` (estratto) [media].

### 1.6 EMCON e cultura di comando

- **Silenzio come regola** (08 §4.4) [alta].
- **Comando per negazione.** Nella dottrina della Maritime Strategy i comandanti dei gruppi portaerei usavano il
  *command by negation*: azioni pianificate prima e intervento solo per cambiarle `[NSL05]` [media]. Per un SSN
  significa ordini iniziali larghi, aggiornati raramente.
- **Riflesso condizionato.** Ricevuto un avviso di minaccia, il comandante andava subito in profondità e si
  allontanava dal *datum*, senza chiedere conferma `[NSL05]` [media]. Un FLASH di minaccia deve quindi spingere
  all'azione immediata, non a una domanda di chiarimento.
- **Indipendenza.** La prassi USA preferiva il sottomarino «lupo solitario»; quella sovietica le operazioni
  sostenute da aerei, navi, satelliti e altri sottomarini `[NSL85]` [media].

### 1.7 Coordinamento con unità amiche

- **SOSUS → aereo → SSN.** SOSUS forniva le aree di probabilità agli aerei di pattugliamento. Quando un SSN perdeva il
  bersaglio, la squadra SOSUS/aerei poteva riacquisirlo e restituirglielo `[COTE]` [alta].
- **Strati alternati.** Negli sbarramenti tipo GIUK aerei e sottomarini lavoravano in strati diversi. Le aree avanzate
  erano solo degli SSN, quelle arretrate degli aerei `[COTE]` [alta]. È già una separazione dello spazio d'acqua.
- **SOSUS riduce il bisogno di cooperazione stretta.** Spesso sottomarini e aerei lavoravano in modo indipendente
  `[COTE]` [alta].
- **Strategia avanzata anni '80.** In guerra gli SSN USA sarebbero entrati nelle zone delle SSBN sovietiche (Barents)
  per costringere i sovietici a difenderle; i fondali del Barents favorivano la propagazione per riflessione sul fondo
  `[COTE]` [alta]. Nel Barents il satellite UHF è marginale (08 §4.3): **lì la radio costa di più**.

### 1.8 Casi storici utili come modelli di scenario

| Caso | Cosa insegna | Fonte | Conf. |
|---|---|---|---|
| USS *Scorpion*, maggio 1968 | Deviata mentre rientrava per osservare un gruppo sovietico vicino alle Azzorre. Una lunga trasmissione fu ricevuta solo da una stazione in Grecia e inoltrata. Dichiarata in ritardo all'arrivo mancato | `[WPSCORP]` | alta |
| K-129, marzo 1968 | Doveva fare rapporto al passaggio del 180° meridiano e all'arrivo in zona. Dopo **due rapporti mancati** il comando la chiamò via broadcast, poi con messaggi urgenti, poi la dichiarò dispersa | `[WPK129]` | alta |
| Foxtrot verso Cuba, 1962 | Trasmissioni a raffica intercettate e localizzate dagli USA, senza decifrarle. La procedura USA di emersione passata a Mosca **non arrivò mai ai battelli** | `[NSA399]` | alta |
| U-boot, 1939–45 | Numeri di serie, ripetizioni notturne, recupero a carico del battello | `[NOVAUB]` | media |

Il caso *Scorpion* è il modello del **compito urgente che cambia la pattuglia**. Il K-129 è il modello della
**sessione obbligatoria mancata** (§2). Cuba 1962 mostra il **messaggio vitale che non arriva**.

---

## 2. Lato sovietico (per giocare l'Alfa)

### 2.1 Cosa sappiamo

- **Controllo centralizzato e «controllo» come cuore della dottrina.** I comandi di teatro seguivano i sottomarini e
  le unità di superficie, smistavano l'intelligence e tenevano aggiornati gli SSN soprattutto via VLF. Potevano dare
  direttive per tenerli nel piano generale `[NSL85]` [media].
- **Comunicazioni scarse e quasi solo in ricezione**, ma con canali ridondanti. La rigida aderenza alla dottrina
  riduceva il traffico necessario `[NSL85]` [media].
- **Tracciamento centrale.** I battelli si sarebbero affidati al plottaggio centrale più che al proprio `[NSL85]`
  [media]. Per il gioco: **l'Alfa riceve più dati dal comando** e ha meno libertà.
- **Decisioni assistite da calcolatore.** Gorškov vedeva i sistemi automatici come strumenti che suggeriscono, con la
  decisione finale all'ufficiale; il commissario politico scoraggiava gli scostamenti dalla dottrina `[NSL85]`
  [media; saggio occidentale].
- **Ma non tutto è rigido.** Nel 1982 un articolo dei Proceedings notava che *Morskoj Sbornik* (1981) e Gorškov
  (1980) chiedevano più iniziativa ai comandanti `[USNI82]` (estratto) [media]. Un articolo sovietico del 1983
  (Karmenok), tradotto nel 1987, distingue il controllo operativo da terra da quello tattico a bordo. Lamenta rapporti
  sul nemico arrivati troppo tardi e collegamenti con terra spesso scadenti `[KARM]` [alta come fonte sovietica
  tradotta; riguarda il 1941–45].
- **Sessioni di comunicazione a orari e luoghi prefissati.** Trasmissioni brevi a raffica, a orari concordati, da
  posizioni precise; gli analisti USA ne vedevano ora e origine ma non il contenuto `[NI129]` (estratto, fonte
  divulgativa) [bassa]. Il K-129 doveva fare rapporto a punti di controllo (§1.8) `[WPK129]` [alta]. Lo Yankee saliva a
  quota periscopica due volte al giorno (08 §6.4) [media].
- **Conseguenza di una sessione mancata.** Per il K-129: due rapporti mancati, poi chiamate via broadcast e urgenti,
  poi dichiarazione di dispersione e ricerche `[WPK129]` [alta].
- **Mezzi.** ZEVS (82 Hz) nella Flotta del Nord, raffica «Akula» da 0,72 s, relè Tu-142MR (08 §6) [media].
- **Ricevuta (квитанция) degli ordini.** Che ogni ordine importante richiedesse una ricevuta trasmessa è prassi
  citata nella memorialistica russa, ma **non trovata** in fonti aperte verificate in questa sessione [bassa, *n.v.*].

### 2.2 Asimmetria proposta (PROPOSTA)

| Aspetto | USA (688) | URSS (Alfa) |
|---|---|---|
| Ordini iniziali | Larghi, per negazione | Dettagliati, con punti di controllo |
| Quantità di dati dal comando | Pochi, vecchi | Di più: plottaggio centrale, aerei Il-38, stazioni costiere |
| Rapporti obbligatori | Nessuno, salvo richiesta esplicita | Sessioni a orari fissi; rapporto di contatto obbligatorio |
| Uso delle armi | Secondo le ROE in vigore | Per alcuni bersagli serve **autorizzazione** chiesta e ricevuta |
| Sessione mancata | Nessuna conseguenza diretta, solo ordini persi | Dopo 1 sessione: chiamata «доложите обстановку»; dopo 2: il comando agisce (ramo di missione) |
| Trasmissione | UHF breve o HF; boa SLOT | Raffica 0,7 s: P_fix bassa per singola raffica (08 §9.5) |
| Ricezione in profondità | VLF col filo (lento); ELF forse | ZEVS: campanello anche a 100–160 m (08 §6.1) |

Per il giocatore sovietico la radio è **un guinzaglio che dà anche informazioni**. Per quello americano è **una
finestra cara su un quadro che invecchia**.

---

## 3. Riferimenti di game design

### 3.1 Come altri giochi hanno usato le comunicazioni

| Gioco | Cosa fa la radio | Cosa ha funzionato | Problemi | Fonte | Conf. |
|---|---|---|---|---|---|
| *Fast Attack* (Sierra / Software Sorcery, 1996) | Broadcast ogni 15 min (dai :05). Ricezione via satellite alzando il periscopio almeno 5 s prima dell'ora, oppure col filo VLF lento per tutto il quarto d'ora, con ripetizioni. Tabella velocità–quota del filo (0 kn: 200 ft … 8 kn: 120 ft); a più di 15 kn il filo si rompe. Messaggi con nuovi ordini, dati di bersaglio e posizioni: la carta si aggiorna da sola. Campo **NEXT BROADCAST** a schermo. Condizioni d'allarme e di fuoco (Warning White/Yellow/Red; Weapons Tight/Free) negli ordini; disobbedire costa la carriera | La radio decide quota e velocità; il prossimo broadcast è sempre visibile; i dati sono dichiaratamente vecchi; le ROE sono parte degli ordini | Il manuale avverte di non usare i dati radio per la soluzione di tiro: «never rely on radio-received information to calculate a firing solution» | `[FAMAN]` pp. 23, 44–46 | alta (sul gioco) |
| *Command: Modern Operations* (CMANO v1.12, 2017; CMO) | Opzione «comunicazioni realistiche dei sottomarini»: sotto i **40 m** il battello esce dalla rete; il giocatore non lo controlla più e vede solo l'ultima posizione riportata. Si può richiamarlo col campanello ELF; risale (evitando i nemici) e al rientro condivide i contatti | Rende tangibile l'isolamento; il campanello ELF è un'azione del giocatore; resoconto chiaro di cosa è cambiato al rientro | È un'**opzione**, non la regola: segno che molti giocatori la vogliono disattivabile | `[CMODEV]` `[CMO112]` | alta (sul gioco) |
| *Silent Hunter III* (Ubisoft, 2005) e modifiche (GWX) | Rapporti radio del comando (BdU) su convogli con posizione, rotta e velocità stimate; ordini di zona di pattuglia; rapporti di contatto del giocatore | Il rapporto del comando crea la scelta (inseguire il convoglio o il mercantile vicino?); intercettazione con stima incerta: si aspetta, con tensione | Nelle modifiche realistiche trasmettere aumenta la probabilità di attacco aereo nemico; un rapporto di contatto aumenta quella d'intervento aereo amico (valori di configurazione da forum, 25 e 50) | `[QT3SH3]` `[GWXCFG]` (estratto) | media (rapporti), bassa (valori GWX) |
| *Red Storm Rising* (MicroProse, 1988) | Contatti da sensori propri e da aerei alleati, satelliti, SOSUS; i risultati spostano il fronte | Il quadro strategico dà senso alla missione | Non risulta una vera sala radio (*n.v.*) | `[WPRSR]` | media |
| *Cold Waters* (Killerfish, 2017) | Erede dichiarato di RSR; campagna dinamica | — | Meccaniche radio in missione: **non trovate** nelle fonti lette (*n.v.*) | `[WPCW]` | bassa |
| *688(I) Hunter/Killer* (Sonalysts/EA, 1997), *Sub Command* (2001), *Dangerous Waters* (2005) | Stazioni integrate; alberi ESM e di comunicazione; editor di missioni con script | Integrazione fra stazioni lodata dalla critica | Gestione dei messaggi in missione: **non verificata** (manuali non accessibili in questa sessione) | `[WP688I]` | bassa |
| *Harpoon* (Bond, regole da tavolo e versioni per computer) | Gestione di gruppi, sensori, EMCON | — | Regole radio dei sottomarini **non trovate**; trovata solo una regola di collisione a quota periscopica (errata 1991) | `[HARPERR]` | bassa |

### 3.2 Lezioni

1. **Mostrare sempre il prossimo broadcast** e quanto manca (*Fast Attack*). Senza, la radio diventa un'attesa al
   buio [alta].
2. **I dati radio sono vecchi e lo devono sembrare**: un'area, non un punto (*Fast Attack*, *Silent Hunter III*)
   [alta].
3. **Il rapporto del comando deve creare una scelta** fra due opportunità, non dare la risposta (*Silent Hunter III*)
   [media].
4. **Trasmettere deve avere un prezzo e un premio** (modifiche GWX: aerei nemici contro aiuto amico) [media].
5. **Il realismo pieno va reso opzionale**, oppure dosato (*CMO*) [media].
6. **Le ROE negli ordini funzionano** se il giudizio finale le usa (*Fast Attack*: la carriera) [alta sul gioco].
7. **Noia probabile** (inferenza del compilatore, nessuna recensione trovata che la misuri) [bassa]: traffico senza
   effetto, attese obbligatorie senza contatti, messaggi da leggere per intero senza sintesi, salire «perché il
   gioco lo chiede» senza scegliere quando.

---

## 4. Meccaniche proposte (in ordine di valore per il gioco)

### 4.0 Principi (PROPOSTA)

- **Ogni messaggio di missione cambia una decisione.** Il traffico di fondo resta solo per i numeri di serie e si
  piega in una riga («3 messaggi di servizio»).
- **3–6 messaggi significativi per missione**, non di più.
- **Sempre visibili** nella sala radio e nella barra di stato: prossimo broadcast, ricezione attuale (piena, metà,
  nessuna), **ordini in vigore** (ROE e zone) con il numero di serie che li ha stabiliti, serie mancanti.
- **Ricezione automatica** quando le antenne lo permettono. Il giocatore sceglie quota, velocità, filo e albero, non
  clicca «ricevi».
- **Perdere è recuperabile ma costa**: ripetizione al ciclo dopo, oppure richiesta SSIXS esponendosi (già nel
  nucleo).
- **La compressione del tempo si ferma** al campanello ELF, a un FLASH ricevuto e un minuto prima di un broadcast
  annunciato (11_comandi §3).
- **Livelli**: «radio semplificata» (tutto si riceve a qualunque quota, niente DF), «realistica» (nucleo pieno).
  Il livello classico ignora tutto (come oggi).

### 4.1 Contatti d'intelligence con ellisse che invecchia (priorità 1)

**Cosa.** Un messaggio porta un contatto: unità vera (o falsa), ora dell'osservazione, ellisse d'errore, rotta e
velocità stimate, sensore (SOSUS, P-3, satellite, HF/DF). Alla ricezione compare sulla carta un **contatto
d'intelligence**: un'area che cresce col tempo (cerchio di massimo avanzamento, *furthest-on circle*, termine della
navigazione [media]), allungata lungo la rotta stimata.

**Decisione.** Salgo a copiare l'aggiornamento (area più piccola, ma mi espongo e perdo tempo) o vado avanti con
l'area vecchia che si allarga? È la scelta di §1.1.

**Modello (STIMA).** Posizione pubblicata = posizione vera all'ora dell'osservazione + errore gaussiano (σ = `errore_m`).
Semiassi al tempo t: lungo la rotta `a = errore_m + σ_v · (t − t_oss)`, di traverso `b = errore_m + v · sin(σ_rotta) ·
(t − t_oss)`. Valori tipo: SOSUS σ 5–15 km, P-3 1–3 km, HF/DF 20–60 km (08 §9.5). Un falso allarme (biologico,
mercantile) è possibile come nella realtà `[COTE]`.

**Motore.** Esiste già il simmetrico per il nemico: `contatto_da_radiogoniometria` dà alle unità avversarie un
contatto con `sensore = "radiogoniometria"` e `pos_stimata`. Per il giocatore serve un contatto con
`sensore = "intelligence"`, qualità bassa, posizione nota ma con ellisse. Nuovo campo `contatto` in `MessaggioRadio`
(o gestito dalla missione al ricevere, via `nuovi()`).

**Dati di missione.** `contatto = { unita = {4,5}, eta_s = 900, errore_m = 8000, sensore = "sosus", rotta_err = 20,
velocita_err_kn = 3, falso = nil }`.

**Rischio di noia.** Basso: è informazione che si usa subito.

### 4.2 Ordini in vigore e regole d'ingaggio che cambiano (priorità 2)

**Cosa.** Stato delle armi per categoria di bersaglio: **libero** (tutto ciò che non è identificato amico),
**stretto** (solo bersagli identificati ostili), **trattenuto** (solo autodifesa o su ordine). Sono gli stati della
difesa aerea `[WPWT]` [media]; *Fast Attack* li usa in forma semplificata `[FAMAN]`. Le categorie (STIMA):
sottomarini, navi da guerra, mercantili/neutrali, aerei.

**Decisione.** Con ROE «stretto» bisogna classificare prima di sparare (sonar, periscopio). Un messaggio che allenta o
stringe le regole cambia il piano. Se il giocatore non l'ha copiato, opera con regole vecchie.

**Giudizio (PROPOSTA).** Al lancio il motore registra le ROE **trasmesse** e quelle **ricevute**:
- violazione delle ROE ricevute → infrazione piena;
- violazione di ROE trasmesse almeno un ciclo intero prima ma non copiate → infrazione («dovevi copiare il
  broadcast»: §1.4);
- trasmesse da meno di un ciclo → solo nota nel rapporto finale.
Il rapporto di fine missione elenca le serie perse e cosa contenevano. Così il giocatore impara a temere i buchi.

**Motore e codici.** Esito: le pagine d'esito esistono già (codici ≥ 100, `c − 100` = pagina «CODE n-x», 04 §3); le
missioni originali hanno pagine di rimprovero adatte (es. 4-5, 3-3). Nuovo avviso «ORDINI IN VIGORE CAMBIATI» (§6).

**Dati.** `roe = { navi = "stretto", sottomarini = "libero", neutrali = "trattenuto" }` nello stato iniziale e nei
messaggi.

### 4.3 Zone dello spazio d'acqua e zone protette mobili (priorità 3)

**Cosa.** Aree sulla carta: area d'azione del sottomarino (lì si spara su sommergibili), area libera ASW, zona
protetta mobile attorno alla rotta di un battello amico (`[AAP6HAV]`). Le zone arrivano e cambiano per radio.

**Decisione.** Sparare su un contatto subacqueo dentro una zona protetta è un rischio di fuoco amico. Un messaggio
perso lascia una carta sbagliata. Con un amico nella zona, la zona dice anche **dove sarà il nemico che lo pedina**.

**Motore.** Poligoni o corridoi con orario (punto della rotta pianificata + larghezza + anticipo/ritardo). Il motore
controlla il lancio contro un contatto subacqueo in zona, come per le ROE. Nessuna unità nuova: si usano le unità
amiche già nelle missioni (es. il Charlie II in SCENE3B).

**Dati.** `zone = { { id = "HAVEN1", tipo = "haven_mobile", unita = 7, avanti_m = 9000, dietro_m = 9000, lato_m = 4000,
dal_s = 300 } }`; un messaggio può aggiungere, spostare o togliere una zona.

### 4.4 Rapporto di contatto: supporto contro esposizione (priorità 4)

**Cosa.** Il giocatore trasmette un rapporto di contatto (`TipoTrasmissione::contatto`, già nel nucleo). In cambio
la missione dà **supporto**: un aggiornamento più preciso dei bersagli, il permesso di attaccare, un aereo amico che
tiene il contatto, un bersaglio che «passa» ad altri (punteggio). Il costo è la radiogoniometria del nucleo: un ESM
nemico entro 50 km ha subito il rilevamento; altrimenti, dopo il ritardo, un fix con errore (`radio_trasmetti`).

**Variante SLOT (PROPOSTA).** Lancio di una boa dal segnalatore: trasmette dopo un ritardo scelto (5–30 min, STIMA)
dal punto di lancio; solo verso un aereo amico in portata VHF (STIMA 30–50 km); niente albero; il fix nemico cade
**sulla boa**. Dotazione 2–4 boe (STIMA). Fonti: `[WPSLOT]` `[NSL4604]`.

**Decisione.** Chiedere aiuto e rivelare la zona, o tenere il silenzio. Con un ESM vicino conviene la boa.

**Dati.** `rapporti = { contatto = { gestore = "R_SUPPORTO", ritardo_s = 600, una_volta = true } }`.

**Codici.** Esistono 324 (trasmissione) e il contatto nemico da radiogoniometria. Nuovi: «SUPPORTO IN ARRIVO»,
«BOA SLOT LANCIATA» (§6).

### 4.5 Compito urgente con scadenza (priorità 5)

**Cosa.** Un FLASH o IMMEDIATE con un compito valido fino a un'ora: intercettare un gruppo, essere in un punto,
osservare (modello *Scorpion*, §1.8). Se il messaggio arriva tardi (perso al primo passaggio, copiato alla
ripetizione) l'occasione si riduce o sparisce. Con l'ELF (sovietici sempre, USA come opzione) il **campanello**
avvisa in profondità che c'è un FLASH (già nel nucleo: `campanello`).

**Decisione.** Risalire subito al campanello (e rompere il contatto in corso) o finire quello che si sta facendo.

**Dati.** `scade_s`, `obiettivo = { tipo = "zona", ... }`, `gestore_scaduto`.

### 4.6 Sessioni obbligatorie e ricevute: il guinzaglio sovietico (priorità 6)

**Cosa.** Per l'Alfa: sessioni a orari fissi con tolleranza (nucleo: ±30 min, da comprimere). In sessione:
copiare, e se richiesto trasmettere la ricevuta o il rapporto (raffica 0,7 s). Sessione mancata: il comando chiede
notizie; due mancate: ramo di missione (scorta inviata, ordine di rientro, esito «rifiuta gli ordini»: pagina 3-3 di
SUB OPS) — modello K-129 `[WPK129]`.

**Per gli USA** solo le finestre «consigliate» che già esistono: nessuna punizione, ma è lì che il comando concentra
il traffico importante (PROPOSTA).

**Dati.** `finestre = { ogni_s = 900, primo_s = 600, tolleranza_s = 120, obbligatorie = true, ricevuta = true,
saltate_max = 2, gestore_saltate = "R_DISPERSO" }`.

### 4.7 Richiamo, deviazione e rami di missione (priorità 7)

**Cosa.** Un messaggio cambia l'esito possibile: «interrompere e rientrare», «bersaglio cambiato», «operazione
annullata». Se il giocatore non lo copia e continua, il giudizio usa la regola di §4.2.

**Uso.** Raramente: una missione su tre al massimo, altrimenti il giocatore smette di fidarsi degli ordini iniziali.

**Motore.** Il gestore del messaggio può accodare un esito (`m688.messaggio(u, 100 + k)`) o cambiare variabili di
scena lette da altri gestori del file radio.

### 4.8 Ricezione parziale, messaggi corrotti e inganno (priorità 8)

**Cosa.**
- **Corrotto.** Con ricezione al 50% (filo fra 8 e 12 nodi) il nucleo oggi conta «una volta su due». Proposta: se
  una messa in onda è ricevuta a metà, il testo arriva con **gruppi mancanti** (`?????`). I numeri del contatto
  possono mancare: si sa che c'è un messaggio, non dove sia il bersaglio. Alla ripetizione si completa.
- **Inganno.** `autentico = false` esiste già (il gruppo d'autenticazione non torna). Il giocatore deve controllare
  con la tabella del giorno; il motore lo segnala solo se il giocatore lo chiede (azione «verifica»), altrimenti a fine
  missione. Che l'inganno radio contro i sottomarini fosse una pratica reale non è documentato in fonti aperte:
  **finzione di design** [bassa]. Da usare al massimo in una missione per lato.

### 4.9 Avvisi di minaccia (priorità 9)

**Cosa.** FLASH: «P-3 / Il-38 in zona», «ricerca con boe a nord di X». Risposta da manuale: giù e via dal *datum*
`[NSL05]`. Le unità esistono già nelle missioni (elicotteri, aerei); l'avviso anticipa l'ESM.

**Rischio.** Se l'avviso è sempre vero e sempre utile diventa un aiuto gratuito. Meglio legarlo a un costo (arriva solo
a chi ascolta) e a un'incertezza (area, ora).

### 4.10 Meteo e ambiente (priorità 10)

ROUTINE con lo stato del mare e il rumore ambiente previsti: incide sull'esposizione dell'albero (dossier 02 §7.7) e
sul sonar. Utile in una campagna futura; poco nelle missioni brevi.

### 4.11 Riepilogo

| # | Meccanica | Decisione che crea | Peso per il realismo | Costo d'implementazione | Già nel nucleo |
|---|---|---|---|---|---|
| 1 | Contatti con ellisse che invecchia | salire per aggiornare o no | alto | medio | no (simmetrico del DF sì) |
| 2 | ROE e ordini in vigore | classificare prima di sparare; temere i buchi | alto | medio | in parte (`ordine`, `ordini()`) |
| 3 | Zone dello spazio d'acqua | dove si può sparare su sommergibili | alto | medio | no |
| 4 | Rapporto di contatto e boa SLOT | aiuto contro esposizione | medio-alto | basso (DF c'è) | in gran parte |
| 5 | Compito con scadenza + campanello | interrompere o no | medio | basso | campanello sì |
| 6 | Sessioni sovietiche e ricevute | il guinzaglio dell'Alfa | medio (solo URSS) | basso | finestre sì |
| 7 | Richiamo e rami | obbedire e perdere, o disobbedire | medio | basso | via gestori |
| 8 | Corrotti e inganno | verificare | basso-medio | medio | `autentico` sì |
| 9 | Avvisi di minaccia | giù e via | medio | basso | sì (messaggi) |
| 10 | Meteo | — | basso | basso | sì (messaggi) |

---

## 5. Formato del contenuto radio per missione (bozza Lua)

### 5.1 Compatibilità

Oggi `data/missioni/radio/<ID>.lua` restituisce una **lista** di messaggi (`docs/09_schema_missioni.md` §5.1); il
motore la legge con `lua_rawgeti` da 1 in su (`Missione::radio_enhanced`). Le chiavi con nome nella stessa tabella
sono quindi **ignorate dal motore di oggi**: si possono aggiungere senza rompere nulla. I file delle missioni
convertite non si toccano; i gestori nuovi stanno nel file radio (va aggiunto il caricamento nel registro degli
script, PROPOSTA).

### 5.2 Campi proposti

Tabella principale (chiavi con nome):

| Campo | Contenuto |
|---|---|
| `formato_radio` | 2 (1 = sola lista di oggi) |
| `parametri` | sovrascrive `ParametriRadio`: `ciclo_s`, `fondo_ogni_s`, `elf` (sì/no), ripetizioni |
| `stato_iniziale` | `roe` per categoria; `zone` iniziali |
| `finestre` | sessioni: `ogni_s`, `primo_s`, `tolleranza_s`, `obbligatorie`, `ricevuta`, `saltate_max`, `gestore_saltate` |
| `zone` | definizioni delle zone dello spazio d'acqua (fisse o mobili) |
| `rapporti` | cosa succede quando il giocatore trasmette un rapporto: `contatto`, `situazione`, `attacco` → gestore, ritardo |
| `slot` | numero di boe, ritardi ammessi, portata VHF |
| `gestori` | funzioni Lua `function(u) … end` richiamabili da messaggi, finestre e rapporti |

Ogni messaggio (parte lista), oltre ai campi di oggi (`dopo_s`, `precedenza`, `da`, `a`, `oggetto`, `classifica`,
`testo`, `gestore`, `autentico`):

| Campo | Contenuto |
|---|---|
| `id` | nome per i riferimenti (`annulla`, condizioni) |
| `se` | condizione dichiarativa per metterlo in onda: `{ unita_colpita = 4 }`, `{ ricevuto = "INT1" }`, `{ trasmesso = "contatto" }`, `{ variabile = { 3, ">=", 2 } }` |
| `contatto` | §4.1: `unita`, `eta_s`, `errore_m`, `sensore`, `rotta_err`, `velocita_err_kn`, `falso` |
| `roe` | §4.2: nuove regole per categoria |
| `zone` | §4.3: aggiungi, sposta, togli |
| `scade_s` | §4.5: dopo, il compito non vale più; `gestore_scaduto` |
| `annulla` | id del messaggio superato («disregard my …») |
| `ricevuta` | il comando vuole conferma (trasmissione breve) |
| `ripetizioni` | sovrascrive quelle della precedenza |

### 5.3 Esempio A — SCENE4A *MUMAR CADAVER* (USA, Gibilterra, limite 2700 s)

Briefing originale: affondare quattro petroliere libiche nello stretto; le fregate Koni «solo se necessario». Unità:
petroliere id 4–7, mercantile neutrale id 8, Koni id 9–10, due Foxtrot id 2–3. Pagine d'esito: 4-5 (rimprovero),
4-6 (successo), 4-7 (petroliere intatte), 4-8/4-9 (risultato parziale), pagina d'emergenza per il mercantile
affondato. Teatro a 36°N: satellite disponibile (08 §4.3).

```lua
-- data/missioni/radio/SCENE4A.lua (bozza: testi da tradurre, valori STIMA)
return {
  formato_radio = 2,
  parametri = { ciclo_s = 600, fondo_ogni_s = 300, elf = false },
  stato_iniziale = {
    roe = { mercantili = "libero",      -- le petroliere libiche: bersaglio della missione
            navi = "trattenuto",        -- Koni: solo se necessario (briefing originale)
            sottomarini = "stretto",    -- Foxtrot: solo se identificati
            neutrali = "trattenuto" },
  },
  rapporti = {
    contatto = { gestore = "R_AGGIORNA", ritardo_s = 480, una_volta = true },
  },
  slot = { boe = 2, ritardo_min_s = 300, ritardo_max_s = 1800, portata_vhf_m = 40000 },

  -- 1. dato P-3 sulle petroliere: area da cercare, non un bersaglio
  { id = "INT1", dopo_s = 180, precedenza = "priority", da = "CTF 69", oggetto = "TANKER GROUP POSIT",
    classifica = "SECRET",
    testo = { it = "P-3 ALLE 1112Z: QUATTRO PETROLIERE IN COLONNA ... ROTTA 270 VEL 12 ...",
              en = "P-3 AT 1112Z: FOUR TANKERS IN COLUMN ... COURSE 270 SPEED 12 ..." },
    contatto = { unita = { 4, 5, 6, 7 }, eta_s = 900, errore_m = 3000, sensore = "p3",
                 rotta_err = 15, velocita_err_kn = 2 } },

  -- 2. avviso: un mercantile neutrale naviga con il gruppo (prepara l'esito d'emergenza originale)
  { id = "NEU1", dopo_s = 420, precedenza = "immediate", da = "CINCLANTFLT", oggetto = "NEUTRAL IN COMPANY",
    testo = { it = "MERCANTILE DI BANDIERA NEUTRALE IN COMPAGNIA DEL GRUPPO. CLASSIFICARE PRIMA DEL LANCIO.",
              en = "NEUTRAL FLAG MERCHANT IN COMPANY WITH GROUP. CLASSIFY BEFORE LAUNCH." },
    roe = { neutrali = "trattenuto", mercantili = "stretto" } },

  -- 3. le Koni ricevono l'ordine di attaccare: ROE allentate, ma solo dopo il primo colpo
  { id = "ROE2", se = { unita_colpita = { 4, 5, 6, 7 } }, precedenza = "flash", da = "CINCLANTFLT",
    oggetto = "ROE CHANGE",
    testo = { it = "INTERCETTATO ORDINE ALLE KONI DI CACCIA ANTISOM. ARMI LIBERE SULLE KONI.",
              en = "KONI UNITS ORDERED TO PROSECUTE. WEAPONS FREE ON KONI." },
    roe = { navi = "libero" } },

  -- 4. compito a scadenza: se le petroliere superano lo stretto, interrompere
  { id = "RICH", dopo_s = 2100, precedenza = "immediate", da = "CINCLANTFLT", oggetto = "BREAK OFF",
    se = { variabile = { 3, "<", 2 } },          -- meno di due petroliere colpite (variabile da verificare)
    testo = { it = "SE IL GRUPPO HA SUPERATO 5-30W INTERROMPERE E RIENTRARE. L'AVIAZIONE PROSEGUE.",
              en = "IF GROUP PAST 5-30W BREAK OFF AND RTB. AIR WILL PROSECUTE." },
    gestore = "R_RIENTRO", scade_s = 600 },

  gestori = {
    -- premio del rapporto di contatto: dato piu' fresco e piu' preciso sulle petroliere rimaste
    R_AGGIORNA = function(u) --[[ nuovo contatto: eta_s = 120, errore_m = 1000 ]] end,
    -- richiamo ignorato oltre la scadenza: pagina 4-5 «WHERE DO YOU THINK YOU WERE GOING?»
    R_RIENTRO = function(u) --[[ se dopo scade_s il giocatore e' ancora a est di 5-30W: m688.messaggio(u, 105) ]] end,
  },
}
```

**Cosa cambia per il giocatore.**
- INT1 dice dove cercare, ma l'area cresce di circa 6 km ogni 15 minuti (12 kn ± 2). Conviene ricopiare? A 36°N
  basta una richiesta SSIXS di 3 minuti con l'albero, ma le Koni hanno ESM: il nucleo le avvisa se sono entro 50 km.
- NEU1 è il messaggio che evita l'esito peggiore. Perso NEU1, il giocatore spara su «quattro petroliere» e una è il
  neutrale.
- ROE2 arriva solo dopo il primo colpo, quando il giocatore è più impegnato e più profondo. Senza ROE2, affondare una
  Koni resta una violazione degli ordini iniziali.
- Il rapporto di contatto dà un dato fresco, ma un fix sulla propria posizione alle Koni.

### 5.4 Esempio B — SCENE3B *SUB OPS* (URSS, Alfa, Barents vicino a Poljarnyj)

Briefing originale: cercare intrusi lungo la costa; trovato l'intruso, avvicinarsi, accendere il sonar attivo e
seguirlo fuori dall'area; altrimenti rientrare. Unità: Alfa id 0, Trafalgar intruso id 2, Slava id 3, Koni id 4,
mercantili id 5–6, Charlie II id 7. Pagine d'esito: 3-3 (ha lasciato il posto, «rifiuta i miei ordini»), 3-6
(l'intruso ha seguito fuori uno dei nostri sottomarini), 3-7 (un nostro sottomarino distrutto), 3-8 (intruso
scacciato), pagina d'emergenza per un'unità sovietica affondata. A 70°N il satellite non conta; c'è lo ZEVS.

```lua
-- data/missioni/radio/SCENE3B.lua (bozza: testi da tradurre, valori STIMA)
return {
  formato_radio = 2,
  parametri = { ciclo_s = 600, fondo_ogni_s = 400, elf = true },   -- ZEVS: campanello in profondita'
  stato_iniziale = {
    roe = { sottomarini = "trattenuto", navi = "trattenuto", neutrali = "trattenuto" },  -- solo sonar attivo
  },
  finestre = { ogni_s = 600, primo_s = 300, tolleranza_s = 90, obbligatorie = true, ricevuta = true,
               saltate_max = 2, gestore_saltate = "R_SILENZIO" },
  zone = {
    -- il Charlie II esce in mare: zona protetta mobile sulla sua rotta (attiva quando parte)
    { id = "HAVEN_K", tipo = "haven_mobile", unita = 7, avanti_m = 8000, dietro_m = 6000, lato_m = 3000,
      attiva = false },
  },
  rapporti = {
    contatto = { gestore = "R_ISTRUZIONI", ritardo_s = 240 },      -- il comando risponde con ordini precisi
  },

  -- 1. stazione costiera / Il-38: un contatto subacqueo dubbio vicino all'isola Kil'din
  { id = "INT1", dopo_s = 240, precedenza = "immediate", da = "KP SF", oggetto = "KONTAKT",
    testo = { it = "STAZIONE COSTIERA: CONTATTO SUBACQUEO PROBABILE, ORE 0940, AREA ...",
              en = "COASTAL STATION: PROBABLE SUBMERGED CONTACT, 0940, AREA ..." },
    contatto = { unita = { 2 }, eta_s = 600, errore_m = 9000, sensore = "costiera", rotta_err = 60,
                 velocita_err_kn = 4 } },

  -- 2. avviso di movimento del Charlie II: dove sara' l'amico, e quindi chi lo pedina
  { id = "MOV1", dopo_s = 540, precedenza = "priority", da = "KP SF", oggetto = "DVIZHENIE PL",
    testo = { it = "PL PR.670M ESCE ALLE 1015 PER ROTTA ... ZONA PROTETTA 4 MIGLIA. NON IMPIEGARE ARMI.",
              en = "SSGN PR.670M SAILS 1015 VIA ROUTE ... HAVEN 4 NM. NO WEAPONS." },
    zone = { attiva = { "HAVEN_K" } } },

  -- 3. FLASH col campanello ZEVS: un Il-38 ha boe in acqua sul contatto, il comando vuole un rapporto
  { id = "FL1", se = { ricevuto = "INT1" }, dopo_s = 1200, precedenza = "flash", da = "KP SF",
    oggetto = "DOLOZHIT",
    testo = { it = "IL-38 CONFERMA SOMMERGIBILE STRANIERO ... RIFERIRE POSIZIONE ALLA PROSSIMA SESSIONE.",
              en = "IL-38 CONFIRMS FOREIGN SUBMARINE ... REPORT POSITION NEXT SESSION." },
    contatto = { unita = { 2 }, eta_s = 300, errore_m = 2500, sensore = "il38" }, ricevuta = true },

  gestori = {
    -- risposta al rapporto: ordine esplicito (centralizzato) di seguire e premere col sonar attivo
    R_ISTRUZIONI = function(u) --[[ messaggio IMMEDIATE: «SEGUIRE, SONAR ATTIVO, NON IMPIEGARE ARMI» ]] end,
    -- due sessioni mancate: il comando chiama, poi manda la Koni a cercare l'Alfa; esito 3-3 se non risponde
    R_SILENZIO = function(u) --[[ prima: messaggio «доложите обстановку»; poi: m688.messaggio(u, 103) ]] end,
  },
}
```

**Cosa cambia per il giocatore.**
- Ogni 10 minuti l'Alfa deve fare sessione: salire o trovarsi con il filo pronto, copiare, mandare una raffica.
  Ogni sessione è un momento in cui il Trafalgar può sentirlo salire. Il guinzaglio ha un costo tattico vero.
- MOV1 è il messaggio chiave: dice **dove passerà il Charlie II**, quindi dove il Trafalgar andrà a pedinarlo
  (pagina d'esito 3-6). Perso MOV1, il giocatore cerca male e rischia di scambiare il Charlie per l'intruso dentro la
  zona protetta.
- Il FLASH arriva col campanello dello ZEVS anche in profondità: il giocatore sa che c'è qualcosa e sceglie quando
  salire.
- Il rapporto di contatto ottiene ordini precisi: più aiuto, meno libertà.

---

## 6. Codici dei messaggi di bordo: esistenti e proposti

**Esistenti** (classico, `re/tok/messaggi.txt`): 0x03 periscopio abbassato, 0x20 «siamo stati scoperti», 0x25 antenna
abbassata; esiti ≥ 100 (`c − 100` = pagina «CODE n-x», 04 §3).

**Esistenti Enhanced** (fascia runtime, `engine/missioni/src/missione.cpp`): 315 in arrivo, 316 FLASH, 317 IMMEDIATE,
318 ricevuto, 319 perso, 320 campanello ELF, 321 filo rotto, 322 filo senza segnale, 323 satellite assente,
324 trasmissione, 325 SSIXS.

**Proposti** (fascia del runtime 310–339; da concordare con la sessione che la gestisce, che è sua):

| Codice | Avviso | Meccanica |
|---|---|---|
| 326 | Ordini in vigore cambiati (ROE o zone) | §4.2, §4.3 |
| 327 | Contatto d'intelligence riportato sulla carta | §4.1 |
| 328 | Sessione di comunicazione mancata: il comando chiede notizie | §4.6 |
| 329 | Gruppo d'autenticazione errato (dopo verifica) | §4.8 |
| 330 | Ordine superato o scaduto | §4.5, `annulla` |
| 331 | Supporto in arrivo (aereo amico, dato aggiornato) | §4.4 |
| 332 | Boa SLOT lanciata / trasmessa | §4.4 |
| 333 | Lancio contro ROE o in zona protetta (avviso dell'ufficiale in seconda prima del fuoco, solo a livello facile) | §4.2, §4.3 |

Le pagine d'esito originali bastano per i rami radio delle missioni esaminate. Per le infrazioni di ROE serve una riga
nuova nel rapporto finale (Enhanced), non una pagina nuova.

---

## 7. Lacune e domande aperte

- **Contenuto reale del broadcast degli SSN anni '80**: tipi e frequenza dei messaggi, formati (*submarine notice*,
  rapporti di contatto). Fonti aperte solo indirette; il resto è prassi ricostruita [bassa].
- **Dimensioni reali delle aree SOSUS e ritardi di trasmissione**: non trovati.
- **Ricevuta sovietica degli ordini** e frequenza delle sessioni di un SSN (non SSBN): *n.v.*
- **Giochi**: messaggistica di *688(I)*, *Sub Command*, *Dangerous Waters*, *Cold Waters* e regole radio di *Harpoon*
  non verificate sui manuali. Da controllare se servono (manuali in possesso dell'utente?).
- **Noia**: nessuna recensione trovata che la misuri sulla radio; le lezioni del §3.2 su questo punto sono inferenze.
- **Valore di `variabile 3` in SCENE4A** e delle altre variabili di scena citate negli esempi: da verificare sugli
  script prima di scrivere i file veri.
- **Decisioni per l'utente**: ciclo compresso (10 o 15 min?); livello «radio semplificata» sì o no; giudizio sulle ROE
  trasmesse ma non copiate (severo come in §4.2 o solo nota?); inganno sì o no.

---

## 8. Fonti

### Web (consultate il 24 ottobre 2026)
- `[NSL05]` J.H. Patton, «Comms at Speed and Depth: How, For Whom and When?», Naval Submarine League, gennaio 2005: https://archive.navalsubleague.org/?p=4420
- `[COTE]` O.R. Cote Jr., *The Third Battle: Innovation in the U.S. Navy's Silent Cold War Struggle with Soviet Submarines*, Newport Paper 16, Naval War College, 2003 (versione 2000 letta su FAS): https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm ; PDF: https://apps.dtic.mil/sti/pdfs/ADA421957.pdf
- `[WPSOSUS]` Wikipedia, *SOSUS* (RAINFORM, fasci, centri di valutazione): https://en.wikipedia.org/wiki/SOSUS
- `[AAP6SAA]` Militerm/Sõnaveeb, *submarine action area* (AAP-6, 2016): https://sonaveeb.ee/search/unif/dlall/mil/SAA
- `[AAP6ASWFA]` Militerm/Sõnaveeb, *antisubmarine warfare free area* (AAP-6, 2016): https://sonaveeb.ee/search/unif/dlall/mil/ASWFA
- `[AAP6HAV]` Militerm/Sõnaveeb, *submarine haven* (AAP-6, 2016): https://sonaveeb.ee/search/unif/dlall/mil/submarine%20haven
- `[USNI10]` *Proceedings*, giugno 2010, «Preventing Undersea Mishaps» (estratto; pagina 403): https://www.usni.org/magazines/proceedings/2010/june/preventing-undersea-mishaps
- `[MHSPMI]` Military Health System, glossario, *Prevention of Mutual Interference* (estratto): https://www.health.mil/Reference-Center/Glossary-Terms/2018/06/22/Prevention-of-Mutual-Interference
- `[HYDRO]` Hydro International, «The Prevention of Mutual Interference Within the Subsea Littoral» (estratto; pagina 403): https://www.hydro-international.com/content/article/the-prevention-of-mutual-interference-within-the-subsea-littoral
- `[SYSWSM]` Systematic, «Waterspace management: the critical capability…» (estratto; fonte commerciale): https://systematic.com/us/industries/defense/news-knowledge/blog/waterspace-management-the-critical-capability-for-modern-maritime-command-and-control/
- `[USNI87]` *Proceedings*, ottobre 1987, «Nobody Asked Me» (estratto): https://www.usni.org/magazines/proceedings/1987/october/nobody-asked-me
- `[USNI82]` *Proceedings*, ottobre 1982, «Their Command and Control» (estratto): https://www.usni.org/magazines/proceedings/1982/october/their-command-and-control
- `[NSL85]` Naval Submarine League, gennaio 1985, saggio sul comando e controllo dei sottomarini sovietici: https://archive.navalsubleague.org/?p=902
- `[KARM]` G. Karmenok, «Control of Submarines in Operations on Enemy Sea Lanes», *Morskoj Sbornik* 1983, trad. NSL aprile 1987: https://archive.navalsubleague.org/?p=691
- `[NI129]` *The National Interest*, «When Russia's K-129 Submarine Sank, the CIA Saw Opportunity» (estratto; divulgativo): https://nationalinterest.org/blog/reboot/when-russia’s-k-129-submarine-sank-cia-saw-opportunity-175222
- `[WPK129]` Wikipedia, *Soviet submarine K-129 (1960)*: https://en.wikipedia.org/wiki/Soviet_submarine_K-129_(1960)
- `[WPSCORP]` Wikipedia, *USS Scorpion (SSN-589)*: https://en.wikipedia.org/wiki/USS_Scorpion_(SSN-589)
- `[NOVAUB]` NOVA, intervista a un sopravvissuto di U-boot (estratto): https://dipsy.pbs.org/wgbh/nova/lostsub/survivor2.html
- `[WPSLOT]` Wikipedia, *SLOT buoy*: https://en.wikipedia.org/wiki/SLOT_buoy
- `[NSL4604]` Naval Submarine League, articolo sulle boe di comunicazione (estratto): https://archive.navalsubleague.org/?p=4604
- `[SCIAM]` *Scientific American*, articolo su Deep Siren e comunicazioni con i sottomarini (estratto): https://www.scientificamerican.com/article/navy-satellite-deep-siren/
- `[WPWT]` Wikipedia, *Weapons Tight* (stati delle armi): https://en.wikipedia.org/wiki/Weapons_Tight
- `[FAMAN]` Sierra, *Fast Attack — Tactics Training*, manuale, 1996, pp. 23, 44–46: https://api.regvault.org/api/v1/game/dos/192e468b/manual
- `[CMODEV]` Slitherine, intervista agli sviluppatori di *Command: Modern Operations* (comunicazioni interrotte, sottomarini sotto 40 m, campanello ELF): https://www6.slitherine.com/news/command-modern-operations-developers-interview-the-new-simulation-editor-features
- `[CMO112]` Wargamer.fr, *Command MANO: sortie de Chains of War et v1.12* (estratto): https://www.wargamer.fr/command-mano-sortie-de-chains-of-war-et-v1-12/
- `[QT3SH3]` Quarter to Three, diario di *Silent Hunter III*, 6 aprile 2011: https://www.quartertothree.com/fp/?p=4906
- `[GWXCFG]` Discussione Steam su *Silent Hunter III* e configurazione GWX degli attacchi aerei (estratto di ricerca; URL esatto da riverificare fra): https://steamcommunity.com/app/15210/discussions/0/828934423990792141 e https://steamcommunity.com/app/15210/discussions/0/1696043806553898790
- `[WPRSR]` Wikipedia, *Red Storm Rising (video game)*: https://en.wikipedia.org/wiki/Red_Storm_Rising_(video_game)
- `[WPCW]` Wikipedia, *Cold Waters (video game)*: https://en.wikipedia.org/wiki/Cold_Waters_(video_game)
- `[WP688I]` Wikipedia, *688(I) Hunter/Killer*: https://en.wikipedia.org/wiki/688(I)_Hunter/Killer
- `[HARPERR]` Errata di *Harpoon* di L. Bond, 1991 (collisioni a quota periscopica; estratto): https://lasr.cs.ucla.edu/tek/games/cz/v6
- Già nel dossier 08 e riusate qui: `[FASVLF]`, `[NSL99]`, `[NRTRAF]`, `[FASSSIXS]`, `[SCMP]`, `[WPBOR]`, `[NSA399]`, `[WPZEVS]`, `[RUSNAVY]`.

### Rimandi interni
- `docs/ricerca/08_comunicazioni_radio.md`: §2.2 ciclo, §3.4 filo, §4.3 satellite alle alte latitudini, §5.3 messaggi persi, §6 lato sovietico, §9 proposta numerica.
- `docs/11_comandi_e_stazioni.md` §11 (riga S4 e direzione dell'utente), §3 (compressione del tempo).
- `engine/nucleo/include/nucleo/radio.hpp` (`SalaRadio`, `MessaggioRadio`, `ParametriRadio`, `trasmetti`, `campanello`).
- `engine/missioni/src/missione.cpp` (`radio_enhanced`, `radio_trasmetti`, `contatto_da_radiogoniometria`, `passo_radio`, codici 315–325).
- `docs/09_schema_missioni.md` §5.1 (file radio per missione); `docs/04_vm_missioni.md` §3 (esiti ≥ 100).
- `docs/originale/briefing_EN.txt` (briefing e pagine d'esito di SCENE3B e SCENE4A); `re/tok/messaggi.txt`.
