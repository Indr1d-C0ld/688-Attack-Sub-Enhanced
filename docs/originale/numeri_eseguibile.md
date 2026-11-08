# Numeri dall'eseguibile: risposte puntuali per il motore

File della sessione laterale di reverse engineering. Le sintesi vanno poi in `docs/originale/sensori_e_navigazione.md`
e in `docs/11_comandi_e_stazioni.md`, a cura della sessione principale. Fonte: `re/unpacked/GAME.EXE`
(segmento 1000 per l'interfaccia, 2011 per la simulazione) e `re/decomp/GAME.EXE.c`.

## 1. Armi del giocatore: dettagli per la fase C3 (24/10/2026)

### 1.1 Stato iniziale di tubi e scorte

- **I tubi partono tutti vuoti**, con stato `02D1+i` = 0. Li azzerano due inizializzazioni:
  - il caricamento della scena (`1000:3FF0–4008`);
  - il reset della partita (`1000:4070–4075`), che azzera anche i guasti `DS:CC86+2i`.

  Nessun codice e nessuno script li arma all'avvio: il giocatore deve armarli (20 s ciascuno).
- **Le scorte partono dalla dotazione intera.** `FUN_1000_1E11` copia il blocco di configurazione della scena:

  | Byte | Destinazione | Contenuto |
  |---|---|---|
  | 1–6 | `02DF+t` | scorte per tipo d'arma |
  | 7–13 | `02D8+i` | tipo d'arma dei 7 tubi |
  | 0 | `DS:032F` | lato: 0 = 688, 1 = Alfa |
  | 14 | — | mappa |

  Le armi nei tubi non si sottraggono, perché all'inizio i tubi sono vuoti: ogni armamento toglie una scorta, ogni
  svuotamento la restituisce.
- **Correzione al §9.5 di `sensori_e_navigazione.md`.** `DS:032F` è il **lato** (byte 0 del blocco), non l'indice
  della lettera della scena. I tubi sono 7 − `DS:032F`, quindi 7 sul 688 e 6 sull'Alfa. Anche il limite dei siluri
  in acqua e il «+14» degli esiti dipendono dal lato.

### 1.2 Contatore dei siluri in acqua (`DS:03DE`)

- **Sale** solo nel lancio dalla stazione Armi (`1000:7676`), e solo se il lancio «conta»: tubi 1–6, comprese le
  esche dei tubi ausiliari, oppure tubo 7 con il Sea Lance (tipo 2). I missili del tubo 7 (Tomahawk e Harpoon) non
  contano.
- **Scende** in `2011:004A`, dentro la routine che toglie un'unità (`2011:003C`), quando `+62` dell'unità è uguale
  all'id del giocatore locale (`DS:0364`).
  - È un `dec word` **senza limite a 0**: il valore potrebbe diventare negativo.
  - Il controllo del lancio è un'uguaglianza, `03DE == 0AFC`. Con il contatore negativo si avrebbero quindi lanci
    in più.
- **Nelle 22 missioni però non succede**, perché i conti tornano:
  - **siluri e Sea Lance:** i gestori di lancio creano l'unità con `LINK 1`, che mette in `+62` l'id del
    lanciatore. Contano al lancio e scalano quando l'unità sparisce (colpo, fine corsa, DETONATE);
  - **esche:** `collega(u, 1)` e poi `collega(u, 0)` lasciano `+62` uguale al giocatore e tolgono `+0C`. Contano e
    scalano dopo 160 s (60 in alcune scene);
  - **missili:** i gestori di Harpoon e Tomahawk non creano un'unità. Fanno solo `MISSILE`, il messaggio 49 e un
    `ATTACK_RND` ritardato sull'unità collegata, quindi non c'è niente da scalare;
  - `CLEAR` rimette `+62` a FFFFh, quindi i modelli costruiti dopo un'arma non ereditano il proprietario.
- **Per il motore:** riprodurre il contatore come nell'originale, senza limite a 0. In pratica conviene comunque
  tenerlo ≥ 0, perché il caso negativo nasce solo da script nuovi che creano oggetti collegati al giocatore senza
  passare dalla stazione Armi.

### 1.3 Tubo selezionato dopo il lancio e dopo l'armamento (`1000:7AF1`)

```
t0 = tipo d'arma del tubo selezionato (02D8 + 02E5) all'ingresso
ripeti al massimo N volte (N = 7 − lato):
    se il tubo 02E5 è armato (stato 3) con il tipo t0: fermati
    02E5 = (02E5 + 1) mod N
```

- **Dopo un lancio** il tubo appena usato è in armamento (stato 2) o vuoto. La selezione passa al **primo tubo
  successivo, in giro circolare, armato con la stessa arma**. Se non ce n'è nessuno, dopo N passi torna **sullo
  stesso tubo**.
- **Quando un tubo finisce l'armamento** (messaggi 10–16, §1.4), la funzione viene chiamata di nuovo.
  - Se il tubo selezionato è già armato, la selezione resta lì.
  - Altrimenti passa al primo tubo armato con il tipo d'arma del tubo selezionato. Anche quello appena armato va
    bene, se ha lo stesso tipo.

### 1.4 Messaggi 10–16 e messaggio 5

- **Conversione.** Il gestore `carica_tubo_n` manda il messaggio 9 + n, cioè 10–16 per i tubi 1–7. Il ciclo
  principale (`1000:8A33`) controlla lo stato del tubo:
  - **stato 2 (in armamento):** il tubo passa a 3, viene chiamata `7AF1` e il messaggio diventa **10 + tipo d'arma**
    del tubo:

    | Tipo | Messaggio | Testo |
    |---|---|---|
    | 0 Mk 48 | 10 | «torpedo armed» |
    | 1 Type 53 | 11 | «torpedo armed» |
    | 2 Sea Lance | 12 | «missile armed» |
    | 3 Tomahawk | 13 | «missile armed» |
    | 4 Harpoon | 14 | «missile armed» |
    | 5 esca | 15 | «noisemaker ready to launch» |

    Il messaggio 16 è vuoto e non viene mai prodotto. Anche il registro dei messaggi recenti mostra il messaggio
    convertito.
  - **altro stato:** il messaggio viene soppresso e il tubo non cambia. Succede per esempio se il tubo è stato
    svuotato prima della fine dell'armamento: il timer dello script non si ferma, ma il suo messaggio non ha
    effetto. Se il tubo viene riarmato, il gestore fa prima `CANCEL` del vecchio timer e poi ne parte uno nuovo
    da 20 s.
- **Il messaggio 5 «weapon armed and ready to fire» non compare mai.** Nessuno script lo emette (nelle 22 missioni
  non c'è `messaggio(5)`), e la simulazione non lo mette in coda. Lo stesso testo, allo stesso indirizzo `DS:0848`,
  è la voce 3 della tabella di stato dei tubi `DS:01F6`, ma la stazione Armi usa solo le voci 0, 2 e 4–9.

### 1.5 Tubo selezionato all'avvio (`DS:02E5`)

- **Vale 0, cioè il tubo 1.** È il valore statico dell'immagine di `GAME.EXE`: nessuna inizializzazione lo scrive.
- Lo modificano solo tre punti: la selezione con il primo clic (`1000:7694`) e i due passi di `7AF1` (`7B15`
  incremento, `7B21` ritorno a 0).
- Quindi nell'originale il tubo 1 è già selezionato all'avvio, e **il primo R lo arma subito** (secondo tempo del
  §10.1). Il motore che parte dal tubo 1 si comporta come l'originale.
- Fra una missione e l'altra non viene azzerato. Ogni missione però ricarica `GAME.EXE`, quindi riparte da 0.

## 2. TARGET (tasto G) per la fase C4 (`1000:802A`)

### 2.1 Dove esiste

Ci sono cinque chiamate, in quattro stazioni:

| Stazione | Funzione | Layout | Chiamata |
|---|---|---|---|
| Periscopio | `1FBB` | `peri.cls` | `802A(0)` |
| Sonar | `3BF7` | `sonar.cls` | `802A(0)` |
| Navigazione, cioè la carta | `4874` | `nav.cls` | `802A(0)` |
| Armi | `740E` | `wcr.cls` | `802A(0)`, e `802A(1)` per «SELEZ.», la scelta dell'arma guidata |

Manovra (`steer.cls`), Radio e Stato non hanno TARGET.

### 2.2 Come sceglie: non sceglie da solo

1. **Prende i contatti disegnati nell'ultima vista.** Sono nella lista `DS:08CC`, con per ognuno:
   - x e y sullo schermo (`08F8`, `08FA`);
   - il carattere, cioè la lettera oppure «X» (`08FC`);
   - l'id dell'unità (`08FD`) e il colore (`08FF`).

   La lista la riempie chi disegna:
   - **al periscopio**, la vista (`4777`): i contatti entro ±66 pixel dal rilevamento della vista;
   - **nelle altre stazioni**, la carta (`6132`): i contatti dentro il riquadro della carta, x 9–167 e y 26–112.
     Fine disegno = `DS:08F0` = 1.

   Se la vista non è ancora disegnata compare «updating image first» e TARGET non parte.
2. **Mette in pausa il tempo** (`DS:1F3E` = 0, salvo in partita via modem) e mostra «select target».
3. **Fa di ogni contatto una zona selezionabile** (`ED9E`), un riquadro di 8 × 8 pixel intorno al segno. Il tasto
   associato è la **lettera** del contatto, oppure, per le «X», un codice 80h + indice, selezionabile solo con il
   mouse.
   - Con `802A(1)` (SELEZ.) restano solo i contatti con lettera e colore 14, cioè i propri siluri filoguidati.
     Se non ce ne sono compare «no guided weapons displayed».
4. **Scelta.** Il giocatore preme una lettera o fa clic su un segno, e `DS:0368` (il bersaglio) diventa l'id di
   quell'unità.
   - **Anche una «X» si può scegliere**: si ha un bersaglio senza traccia solida, quindi senza distanza e senza
     missili (§10.2).
   - Annullando, `DS:0368` = −1.
   - Si esce con Invio, Esc o un tasto di stazione (F1–F9). Poi compare «target acquired» o «no target acquired»
     e il tempo riparte.
5. **Non c'è una scelta automatica** del più vicino o di quello con la traccia migliore. Fra contatti con la
   stessa lettera nella lista (raro) vince l'ultimo.

**Per il motore:** G apre la selezione sui contatti della vista corrente. Sono quelli del periscopio, se si è al
periscopio; altrimenti quelli della carta della stazione. La selezione avviene con la lettera o con il mouse, e il
tempo resta fermo finché il giocatore sceglie.

## 3. Fase C4: sensori, carta e barra del bersaglio

### 3.1 Stazione Sonar (`1000:3BF7`, `sonar.cls`)

**Comandi** (tabella `CS:3CC7`):

| Codice | Comando | Note |
|---|---|---|
| C8h | FILTRO | — |
| C9h | impulso attivo | — |
| CBh | schiera | — |
| CDh | ANALIZZA | — |
| G | TARGET | §2 |
| B, C, F, H, I, O, P, S, T | tasti comuni a tutte le stazioni | `1000:1234` |
| X | esce | — |

Ogni comando controlla prima il guasto del sistema che usa (§9.4 di `sensori_e_navigazione.md`) e, se c'è, mostra
il messaggio di danno.

**Schiera** (CBh, `3D86–3E4F`):
- **Rifiuto:** con il guasto della schiera (`DS:CC88`) compare «the towed array is gone».
- **A schermo** (`447D`):
  - sul 688 c'è un riquadro di testo a (E8h, 99h) che mostra l'**azione possibile**, cioè la tabella `DS:0294`
    indicizzata da `DS:03C6`: «DEPLOY» con la schiera dentro (1), «RETRIEVE» con la schiera filata (0), «IS GONE»
    con la schiera persa (2);
  - sull'Alfa è una spia a (112h, 93h), accesa con la schiera filata.
  - Il riquadro cambia **subito alla pressione**, perché `DS:03C6` cambia prima della fine della manovra.
    Durante la manovra compaiono i messaggi «still deploying/retrieving towed array» e, al recupero,
    «retrieving towed array».
- **Per i contatti conta solo il bit 0 di `+54`**, che impostano gli script:
  - in **filatura** non conta finché lo script, dopo 60 s, non esegue `schiera_trainata(1)`;
  - in **recupero** conta ancora per 40 s, fino a `schiera_trainata(0)`.
- **Per il telegrafo invece** la schiera conta per tutta la manovra (`DS:03C4` ≠ 0, §10.4).

**FILTRO** (C8h, `3E5E–3E99`):
- **Rifiuto:** con il computer del sonar guasto (`DS:CC86`) compare «signal analzyer damaged».
- **Effetto:** commuta `DS:13D6`.
  - Con 1 compare «high pass filter activated sir» e passano solo le armi (§10.5).
  - Con 0 compare «band pass filter activated sir».
- **A schermo:**
  - sul 688 c'è un riquadro a (4Dh, 99h) con la tabella `DS:13DA`: «BAND PASS» o «HIGH PASS»;
  - sull'Alfa è una spia a (112h, 84h).
- **All'avvio** è spento (0), e il guasto del computer lo rimette a 0.

**Impulso attivo** (C9h, `3D17–3D54`):
- **Rifiuto:** con il sonar di tiro guasto (`DS:CC8C`) compare «targeting sonar is damaged».
- **Altrimenti:** «sending active sonar ping captain», comando 2Dh (l'evento acustico di 300 passi, §4) e
  suono `sonar1.t`.
- **Non ci sono altri limiti:** né tempo minimo fra due impulsi, né quota, né velocità. Un nuovo impulso
  durante i 33 s del precedente aggiunge un altro evento nella lista: ce ne stanno al massimo 12.

**ANALIZZA** (CDh, `3EE0`):
- **Rifiuto:** con il computer del sonar guasto compare «signal analzyer damaged».
- **Richiede:**
  - un bersaglio selezionato, altrimenti «you must have a target selected»;
  - che il contatto esista ancora, **con qualsiasi qualità, anche una «X»**, altrimenti «contact has been lost»;
  - che non sia un aereo: «can't analzye air contacts sir».
- **Esegue:**
  1. «performing analysis captain»;
  2. suona il campione della categoria a una frequenza che dipende dal tipo (formule in
     `docs/originale/suoni.md`): nave 2500 + 50·tipo, sottomarino 2000 + 30·tipo, biologico casuale, arma
     5500 + 10·tipo. Durante il suono **il tempo è in pausa**, salvo via modem;
  3. disegna le 16 barre dell'analizzatore (`92AF`, la firma = tipo + 1 in binario) con un tremolio casuale
     (`410B`), per **42 interrupt del timer**. Sono circa 2,3 s di gioco, mentre la simulazione continua.
- **La firma rivela il tipo vero** anche senza traccia solida. Il nome della classe nella barra del bersaglio
  invece richiede la durata ≥ 1400 (§10.5).

### 3.2 Carta (stazione Navigazione `1000:4874`, `nav.cls`; disegno `4C8C`; tasti comuni `1234`)

**Riquadro della carta**, uguale in tutte le stazioni: x 8–176 e y 28–120 pixel, cioè 168 × 92. Le coordinate di
gioco diventano pixel con `px = (x − X0)/S + 8` e `py = 120 − (y − Y0)/S`.

**Scala S**, in unità di gioco per pixel (`2F01:0C90` per x e `0C94` per y, sempre uguali):
- all'avvio vale **6**, cioè 168 × 92 pixel = 20 × 11 nm;
- **I** ingrandisce e **O** riduce (tasti comuni, validi in tutte le stazioni con la carta), con passo `DS:03C0`
  (all'avvio 2):
  - **I:** S −= passo. Sotto 2 si ferma a 2 con «cannot zoom further in captain!». Poi il passo vale 2 se S < 11,
    altrimenti scende di 2;
  - **O:** se S > 40 compare «cannot zoom further out captain». Altrimenti il passo vale 2 se S < 11, altrimenti
    sale di 2, e S += passo.
  - In pratica S va da 2 a 52: 6, 8, 10, 12, 16, 22, 30, 40, 52. Con S = 52 la carta copre circa 175 × 96 nm.
- Ogni zoom mostra «ZOOM FACTOR n», con n = S/2.

**Centro** (`4CDE–4D90`). A ogni disegno la carta si centra sul **proprio battello**:
- X0 = x − 84·S e Y0 = y − 46·S;
- vicino ai bordi della mappa il centro si sposta, perché la carta resta dentro la mappa (da 0 a 32 ×
  larghezza della carta SURF). Il battello allora non è più al centro.

Non c'è un centro fisso né uno scorrimento a mano.

**Sfondo.** È la carta SURF, un campione ogni S unità sulle celle da 32.
- **B** (tasto comune): la prima volta imposta `DS:038E`, le volte successive commuta `DS:0394`. Con `0394` = 1
  lo sfondo è un riempimento uniforme (colore 11) senza fondale.
- **C** passa al «sonar imager» (`DS:03AA` = 1), l'immagine del fondo al posto della carta; **T** torna alla
  carta. Con l'imager si possono anche orientare le viste: F a 0°, P a 90°, S a 270°.

**Pulsanti del pannello Plotting:**

| Pulsante | Codice | Variabile | Effetto |
|---|---|---|---|
| ALL TRACKS | C8h | `DS:03CA` | spia; mostra le scie di tutti i contatti |
| TARGET TRACK | C9h | `DS:03CE` | spia; mostra la scia del solo bersaglio |
| PLOT PROJ | CAh | `DS:15AE` | spia; mostra la proiezione |

- **Scie** (buffer di 150 punti a `DS:47BE`, routine `12EE`). Con entrambe le spie conta TARGET TRACK. I colori
  sono 4 per il bersaglio, 8 per il proprio battello e 0 per gli altri.
- **Proiezione** (PLOT PROJ):
  - **«*» di colore 14** alla posizione del battello fra N minuti, sulla rotta e alla velocità attuali (`+20`,
    `+22`). La distanza è **velocità grezza × N × 540 / 65536** unità, cioè il moto dell'originale: 540 passi al
    minuto, fattore 3,46 compreso;
  - con un bersaglio a **traccia solida**, un secondo **«*» di colore 12** calcolato dalla rotta e dalla
    velocità **vere** del bersaglio (`99F1`).
- **TIME PROJECTION:** **N** in `DS:15AC` vale 10 all'avvio (valore statico) e va da 1 a 99. Le frecce o i
  tasti **«=»** (+1) e **«-»** (−1) lo cambiano, in giro: dopo 99 si torna a 1. Due cifre a (FAh, 71h).

**Punti di rotta** (lista X `2F01:0C96`, Y `2F01:0CB4`, numero `DS:03B2`, al massimo **9**):
- **SET / W** (`4B34`):
  - con 9 punti compare «cannot set any more waypoints captain»; con l'imager attivo «cannot set waypoint from
    sonar imager»; con la carta non ancora disegnata «updating image first»;
  - altrimenti `7BC2(0)` mette **in pausa il tempo**, salvo via modem, e mostra «select waypoint captain». Si
    sceglie con un cursore dentro il riquadro, con il mouse o con le frecce (accelerano ogni 4 ripetizioni); le
    coordinate compaiono sotto la carta. Invio conferma, Esc annulla («selection cancelled»);
  - il punto si aggiunge **in fondo** alla lista. Il primo manda 2Ch (X, Y) e accende il pilota automatico:
    «waypoint set and autopilot turned on». I successivi danno solo «waypoint set captain».
- **CLEAR / K** (`4B81`):
  - toglie l'**ultimo** punto: «waypoint cleared captain»;
  - se non ne restano, spegne il pilota (2Bh); con la lista vuota compare «no waypoints set captain».
- **Arrivo su un punto.** Il messaggio 4 «we've reached a waypoint» chiama `5634`: il primo punto esce dalla
  lista, gli altri scalano di un posto e parte 2Ch verso il nuovo primo. Dopo l'ultimo il pilota si spegne.
- **Disegno:** cifre «1»–«9» di colore 13 alla posizione di ogni punto.
- **AUTO PILOT / «.»** (`49A9`): spia `DS:0198`. Spegne il pilota (2Bh) oppure lo riaccende verso il primo
  punto (2Ch).
- **TIME / Q** (`4A59`): compressione del tempo, finché non si preme Invio. Il fattore è `DS:0F4C` (6000 nel modo
  di prova `DS:03E8`). Via modem non c'è: «time compression not in modem game».

**Proprio battello:** un quadratino di colore 10 bordato di colore 2 alla propria posizione.

**Contorno dei simboli** (`57CC(testo, x, y, colore, contorno)`; confidenza alta, decompilato e disassemblato).
- **Come disegna:** ogni carattere della carta esce cinque volte:
  - quattro nel colore del contorno, spostato di un pixel a sinistra, a destra, in alto e in basso: una croce, non le
    diagonali;
  - una nel suo colore, al centro.
- **I colori del contorno** (`4C8C`):
  - nero (colore 0): lettere dei contatti, cifre dei punti di rotta, «*» della proiezione;
  - colore 4, rosso scuro (170, 0, 0): le «X», gialle (colore 14).
- **Senza contorno:**
  - il proprio battello: riempimento di colore 10 (`A2C6`) e cornice di colore 2 (`E838`);
  - i punti delle scie, pixel singoli (`AEE7`).
- **Al periscopio** (`29F4`, `2A0F`) la regola è la stessa: «X» gialla col contorno rosso scuro, lettere bianche
  (colore 15) col contorno nero.

**Contatti:** «X» o lettera come nel §10.5 di `sensori_e_navigazione.md`. Le X stanno a 25 pixel dal battello sul
rilevamento, indipendentemente dalla scala.

### 3.3 Convenzione degli angoli (importante per il motore e per le missioni Lua)

**Rotte e rilevamenti dell'originale sono angoli matematici**: 0 = est, crescenti in senso antiorario, 9000 = 360°.
Non sono rotte bussola. Le prove:
- **moto** (`2011:0A6A`): incremento x = v·cos(r), incremento y = v·sin(r). `21AD` è il coseno, perché la tabella
  `2F01:000A` vale 255 a 0° e 0 a 90°, e `2206` è il seno. y cresce verso nord;
- **rilevamento** (`2011:2265`, arcotangente con i quadranti): 0 lungo +x (est), 2250 lungo +y (nord), 4500 a
  ovest;
- **visualizzazione** (`1000:925C`): bussola = 450 − r/25, riportato a 0–359, cioè **bussola = 90° − r**;
- **rateo di virata `+36`** positivo = rotta crescente = **virata a sinistra** (antioraria).
  - L'indicatore del timone sta a x = 65h − (+36/2) × 10: va a sinistra con il rateo positivo.
  - Il comando 12h (+10, il primo elemento del timone, dove porta il tasto R) è tutta la barra a sinistra; 08h
    (−10) è tutta a destra.
  - Il §7 di `sensori_e_navigazione.md` va quindi corretto: −3,6 °/s è tutta a **destra**, +3,6 °/s tutta a
    **sinistra**.

**Corretto il 24/10/2026.** Fino ad allora le missioni Lua copiavano il numero senza convertirlo (`rotta = r ×
360/9000`). Ora il convertitore applica la tabella qui sotto, e il runtime ferma la virata come l'originale (§4).

| Campo | Conversione oggi | Conversione giusta in bussola |
|---|---|---|
| `rotta` delle unità, `rotta_ora` (HEAD_NOW), `rotta` (HEAD_TO), `accosta_a` (TURN_TO) | r × 0,04 | (90 − r × 0,04) mod 360 |
| `virata_relativa` (TURN_REL) | r × 0,04, positivo = sinistra | −r × 0,04 se si vuole positivo = destra |
| `rateo_virata` (TURN_RATE) | r × 0,36 °/s, positivo = sinistra | idem, con il segno cambiato |

Valori speciali come −1 = «nessuna rotta» non cambiano. In tutto sono circa 380 chiamate più le rotte iniziali.
Rilevamenti e coni di scoperta sono coerenti nell'originale, perché rotta e rilevamento usano la stessa
convenzione. Il motore deve solo usarne una sola: se calcola i rilevamenti in bussola, deve convertire anche le
rotte delle missioni.

### 3.4 Barra del bersaglio (`1000:8150–8320`, `5840`)

Si aggiorna a ogni giro dell'interfaccia. Ogni campo viene ridisegnato solo quando il suo valore cambia (cache in
`DS:0370–038A`; `DS:0378` forza il ridisegno).

**Riga del proprio battello**, sempre: rotta in gradi bussola, velocità in decimi di nodo (`+22`/35), quota in
piedi (`+1E`/8), posizione x e y in unità e orologio (`DS:50FE`, tick).

**Riga del bersaglio**, se c'è un bersaglio (`DS:0368` ≥ 0) e il contatto esiste:

| Campo | Posizione x | Valore | Condizione |
|---|---|---|---|
| nome | (30h, 6) | lettera + «UNKNOWN CONTACT», «UNKNOWN SURFACE/SUBMERGED CONTACT» o il nome della classe (§10.5) | sempre |
| rilevamento | 31h–4Eh | (rotta propria − rilevamento del contatto)/25, riportato a 0–359: **relativo, in senso orario dalla prua** | sempre |
| distanza | 71h–8Dh | √(dx² + dy²)/5 sulle posizioni **vere**, cioè decimi di nm | traccia solida e \|dx\|, \|dy\| < 1000 unità |
| rotta | B6h–CEh | rotta **vera** del bersaglio in bussola (`+20`; per un oggetto collegato la rotta del capo, `+44`) | **sempre**, anche senza traccia solida |
| velocità | F1h–101h | velocità vera/35 (decimi di nodo) + rumore | traccia solida, come la distanza |
| quota | 114h–137h | quota vera/8 (piedi) + rumore | traccia solida, come la distanza |

**Rumore:**
- velocità: scarto casuale fra −v/16 e +v/16 (`DS:1E02`);
- quota: scarto casuale fra −50 e +49 ft (`DS:1E04`), ma non per i bersagli in superficie;
- lo scarto si estrae di nuovo solo quando il valore da mostrare cambia. A valore costante resta fisso.

**Senza traccia solida** restano nome, rilevamento e rotta (vera). Distanza, velocità e quota non si aggiornano:
restano gli ultimi valori.

**Quando cambiano gli indicatori del contatto** (`990C`), `5840` cancella i cinque campi e scrive «CONTACT HAS
BEEN LOST» finché la barra non viene riscritta.

**Se il contatto sparisce**, il bersaglio si deseleziona da solo (`DS:0368` = −1).

**Modo di prova `DS:03E8`, via modem:** mostra anche x, y, quota, rotta e velocità dell'altro giocatore e la coda
dei comandi.

### 3.5 ESM al periscopio (`1000:244C`; livello `DS:476C`, scritto da `2011:0D60`)

**Cosa misura.** È l'intercettazione radar ed ESM del §3.2 di `sensori_e_navigazione.md`, vista dal giocatore. Il
livello si scrive quando:
- l'osservatore è una **nave o un aereo**, di qualsiasi schieramento;
- il bersaglio è **il giocatore**;
- il giocatore ha il **periscopio alzato** (portata 900 quarti di unità per asse, 4,5 nm) o anche l'**antenna**
  (+450, fino a 6,75 nm).

In quel caso `DS:476C` = **85 − (\|dx\| + \|dy\|)/32**, con dx e dy in quarti di unità: da 85, vicinissimo, a circa
1 al limite. Più l'emettitore è vicino, più il livello è alto.

**Comportamento:**
- vale l'**ultima** intercettazione, non la massima né una somma. Il livello resta finché un'altra valutazione non
  lo riscrive. Non c'è decadimento;
- torna a 0 quando periscopio e antenna sono **entrambi abbassati**, e (curiosamente) **ogni volta che un'unità
  viene tolta** dalla simulazione (`2011:003C`);
- all'avvio vale 0.

**A schermo:**
- **688:** 9 riquadri a passo di 9 pixel. Il riquadro k si accende se 9k < livello, quindi i riquadri accesi sono
  circa ⌈livello/9⌉, fino a 9. I colori vengono dalla tabella `DS:029B`: 9, 9, 9, 10, 10, 10, 14, 14, 12 dal
  basso livello all'alto, cioè azzurro, verde, giallo, rosso chiaro; il decimo valore, 12, lo usa solo l'Alfa; spenti hanno il colore 8;
- **Alfa:** 10 riquadri a passo di 11.

**Per il motore:** l'ESM non misura i radar nemici in generale. È la spia «ti stanno vedendo»: si accende solo
quando una nave o un aereo nel raggio ha il periscopio o l'antenna del giocatore sotto radar. Quello stesso evento
dà il bonus di qualità della traccia del §3.2, cioè il giocatore sta per essere scoperto.

### 3.6 Conferma del taglio della schiera e condizioni del ping

**Finestra di conferma** (`1000:8342(28h, 28h, 13h)`, chiamata da `6FAB`):
- **Testo:** è nella **pagina 19 di `DIALOG.CLS`**, non nella tabella delle frasi dell'EXE: «GOING AT THAT SPEED
  WILL CAUSE THE / TOWED ARRAY TO BE LOST.  ARE YOU SURE / YOU WISH TO CUT THE TOWED ARRAY?». La finestra è a
  (28h, 28h), su un riquadro di 220 × 73 pixel.
- **Pulsanti:** due immagini, `dialog1.enc` = **CANCEL** a sinistra (x 20–100) e `dialog0.enc` = **OK** a destra
  (x 120–180). Sono zone attive con i codici 65 e 66.
  - `8342` restituisce l'indice del pulsante scelto: CANCEL = 0, il comando viene annullato; OK = 1, la schiera
    viene tagliata (stato 2, comando 36h, «towed array cut captain») e poi parte il comando di telegrafo.
- **Tasti:** la scelta passa da `DBFB`, la stessa selezione delle zone usata da tutta l'interfaccia: clic del
  mouse, oppure frecce o Tab e poi Invio.
  - Il cursore parte sull'**ultima zona registrata, cioè OK**: Invio subito conferma il taglio.
  - Non c'è uscita con Esc: il ciclo continua finché non si sceglie uno dei due pulsanti.
- **Tempo:** **fermo** durante la domanda (`DS:1F3E` = 0, salvo via modem); poi torna com'era.

**Quali comandi la chiedono.** Solo il gestore del telegrafo della stazione Manovra (`6F60–6FE0`: pulsanti e tasti
del telegrafo, codici interni 8Bh–91h). Le altre vie che cambiano la velocità non controllano la schiera, e la
simulazione non la taglia mai per la velocità:
- **pilota automatico** (`2011:0705–0790`): rallenta a 1/4 per accostare e poi torna alla velocità salvata (al più
  avanti tutta), scrivendo direttamente la velocità ordinata `+32`. Nessun controllo;
- **guasto dei motori** (`1000:8BE0–8C05`): manda il telegrafo 2 (STOP) o 6 (TUTTA) senza controllo. Con la
  schiera fuori si può quindi finire a TUTTA senza perderla;
- **tastierino:** nell'originale non c'è un telegrafo comune a tutte le stazioni (i tasti comuni di `1234` non lo
  comprendono). Il telegrafo si usa solo dalla Manovra, quindi passa sempre dalla domanda.

**Per il motore:** se il tastierino comanda il telegrafo da ogni stazione, per fedeltà la domanda va fatta anche
lì. Il pilota automatico e i guasti invece non la fanno.

**Ping del sonar attivo** (`3D17–3D54`):
- l'**unica condizione** è il guasto del sonar di tiro (`DS:CC8C`, «targeting sonar is damaged»). La schiera non
  conta, la quota e la velocità nemmeno, né un tempo minimo;
- è l'unico punto dell'EXE che manda 2Dh (`3D3E`). Nel dispatcher della simulazione 2Dh chiama `1843` senza altri
  controlli (§4 di `sensori_e_navigazione.md`).

## 4. Angoli da convertire (supporto al §3.3)

Tutti gli angoli dell'originale sono matematici: r in 9000esimi, 0 = est, antiorario. Bussola = (90 − r × 0,04) mod
360; un angolo relativo o un rateo positivo vuol dire **verso sinistra**.

**Valori nelle missioni Lua:**

| Dove | Opcode | Semantica nell'originale | Conversione in bussola |
|---|---|---|---|
| campo `rotta` delle unità (dallo stato iniziale) | `HEAD_NOW` 06h nell'inizializzazione | rotta assoluta | (90 − r·0,04) mod 360 |
| `rotta_ora(r)` | 06h | rotta attuale e ordinata = r, rateo 0 | come sopra |
| `accosta_a(r)` | `TURN_TO` 19h | rotta ordinata = r, rateo = +3E. Il rateo è **sempre positivo**, quindi l'accostata va **sempre a sinistra** anche quando sarebbe più breve a destra: è una stranezza dell'originale | come sopra; per fedeltà l'accostata resta sempre a sinistra |
| `rotta(r)` | `HEAD_TO` 24h | solo rotta ordinata `+34`. Il rateo resta quello che era; −1 (FFFFh) = nessuna rotta ordinata, virata libera | come sopra; `nil` resta `nil` |
| `virata_relativa(d)` | `TURN_REL` 63h | rotta ordinata = rotta + d; il verso più breve lo sceglie `1E9E` | −d·0,04 se si vuole il positivo a destra |
| `rateo_virata(g)` | `TURN_RATE` 20h | rateo `+36` con segno, positivo = sinistra | −g·0,36 °/s se si vuole il positivo a destra |
| `modello_virata(v)` | `P_TURN` 09h | modulo del rateo standard `+3E` | nessuna (è un modulo) |
| `imposta_24`, `imposta_2a` | `SET_24` 17h, `SET_2A` 18h | componenti x e y della velocità | nessuna (sono cartesiane); nelle 22 missioni non sono usate |

**Non vanno convertiti**, perché la simulazione li calcola dalle posizioni e il motore li calcola nella sua
convenzione: `TURN_TO_TGT` (61h), `TURN_AWAY` (71h), `GOTO_XY`, `GOTO_TGT`, `PURSUE`, `PURSUE2`, `LAUNCH`
(il siluro prende rilevamento e posizione dal contatto). **Nessuna condizione `IF_*` confronta angoli**: si
confrontano distanze per asse, coordinate (`IF_IN_RECT`, `IF_X_GT`), qualità e durate. Gli script non leggono mai un
rilevamento come numero.

**Parametri dei comandi del giocatore** (gestori, §7 di `sensori_e_navigazione.md`):
- **timone 08h–12h:** l'originale scrive in `rateo_virata` il valore (codice − 0Dh) × 2, cioè da −10 per 08h a +10
  per 12h, con il positivo a sinistra. Il primo elemento del timone (12h, dove porta il tasto R) è tutta la barra a
  sinistra, 08h tutta a destra. In convenzione bussola il motore deve passare −(codice − 0Dh) × 2 × 0,36 °/s, oppure
  invertire il segno nel gestore;
- **punti di rotta (2Ch, 3Dh):** sono coordinate x e y, senza angoli. Il pilota (`2011:0705–0790`) calcola la rotta
  verso il punto con `2265`, coerente;
- **quota, telegrafo, periscopio, antenna, schiera:** niente angoli.

**Nell'interfaccia:**
- la rotta mostrata è bussola = 90 − r (`925C`);
- il rilevamento della barra è (rotta − rilevamento) mod 360, cioè il relativo in senso orario dalla prua;
- le «X» della carta stanno sul rilevamento matematico: x = 25·cos, y = −25·sin, con lo schermo che ha y verso il
  basso.

## 5. Guasti dei sistemi del giocatore (`1000:8B39–8C36`)

### 5.1 Generatore casuale

- **È il `rand()` di Borland C** (`1000:FE22`):
  - seme a 32 bit in `DS:BFB0`;
  - a ogni chiamata seme = seme × 015A4E35h + 1, e il risultato è (seme >> 16) & 7FFFh.
- **Non è quello della VM.** `RANDOM` e `IF_RND` usano il byte `DS:CFD9`, moltiplicato per B6BFh.
- **Inizializzazione** (`1000:1668`, `C7FD`), all'ingresso nella scelta della missione:
  1. `DS:CFD9` = secondi XOR centesimi dell'orologio DOS (INT 21h, 2Ch);
  2. `srand(DS:CFD9)`, quindi solo 256 sequenze possibili;
  3. `DS:CFD9` = `rand() % 256` diventa il seme della VM.
- Lo stesso `rand()` serve anche al rumore della barra del bersaglio, al tremolio di ANALIZZA, alla firma dei
  biologici e alla scelta casuale del danno.
- **Nel guasto:**
  1. `rand() % 2`: se esce 0, nessun guasto;
  2. altrimenti `rand() % 6` sceglie il sistema.

### 5.2 Sistema già guasto

**Non succede nulla.** Se il sistema estratto è già guasto (`DS:CC86 + 2i` ≠ 0), la funzione esce senza
riprovare. Più i sistemi sono già guasti, più è facile che un colpo non ne guasti di nuovi.

Indici: 0 sonar computer (`CC86`), 1 towed array (`CC88`), 2 periscope (`CC8A`), 3 targeting sonar (`CC8C`), 4
engines (`CC8E`), 5 antenna (`CC90`). Si guastano i messaggi 20 (bomba di profondità), 26 (colpiti) e 30 (fondo).

### 5.3 Effetti

- **Annuncio.** Dopo il guasto il flag va a 1 e `DS:03E4` = indice + 1. Al giro successivo dell'interfaccia
  (`13C1`) compare il messaggio del sistema, cioè la voce 3Bh + indice della tabella `DS:020A`:
  - «signal analzyer damaged»;
  - «the towed array is gone»;
  - «the periscope has been damaged»;
  - «targeting sonar is damaged»;
  - «maximum engine speed not available»;
  - «the antenna has been damaged».
- **Periscopio** (indice 2):
  - se è alzato viene **abbassato**: comando 32h e `DS:0194` = 0;
  - poi il pulsante del periscopio (stazione Periscopio, `225C`) è **rifiutato sempre**, sia per alzare sia per
    abbassare, con «the periscope has been damaged».
- **Antenna** (indice 5):
  - se è alzata viene **abbassata** (comando 3Eh, `DS:0196` = 0);
  - poi il pulsante è rifiutato con «the antenna has been damaged», alla stazione Periscopio (`21D2`) e alla Radio
    (`5DB5`).
  - Non c'è nessun 41h legato all'antenna: 41h è l'emersione rapida.
- **Sonar computer** (indice 0): filtro spento (`DS:13D6` = 0); FILTRO e ANALIZZA vengono rifiutati (§3.1).
- **Sonar di tiro** (indice 3): l'impulso attivo viene rifiutato. Nient'altro.
- **Schiera** (indice 1): se è fuori o in manovra diventa persa (stato 2, comando 36h); il pulsante dice «IS GONE».
- **Motori** (indice 4):
  - al momento del guasto, se la posizione di velocità è indietro diventa STOP (comando 2), se è MAX diventa TUTTA
    (comando 6);
  - poi MAX e l'arresto rapido vengono rifiutati con «she canna stand the strain jim» (voce `DS:028C`);
  - non ci sono altri limiti: TUTTA resta disponibile.

### 5.4 Quadro di stato (stazione Stato, `status.cls`)

Il tasto S scorre 7 voci:
- **voci 0–5:** il nome del sistema (tabella `DS:0F80`: «sonar computer», «towed array», «periscope», «targeting
  sonar», «engines», «antenna») seguito da «system ok» oppure dal suo messaggio di danno del §5.3;
- **voce 6:** «SHIPS SYSTEMS n PERCENT OPERATIONAL», con **n = integrità `+07` × 100 / integrità iniziale** (`DS:0F48`,
  copiata al caricamento) e al minimo 1.
  - Il «75%» del manuale è questo: l'integrità dello **scafo**, non i motori.
  - Dopo il messaggio 53 «captain, emergency surface!!!!!» la voce diventa **«HULL BREACHED»** (`DS:0F44` = 1).
    Il messaggio 53 parte quando l'integrità del giocatore arriva a 0 e inizia il conto di 30 visite del §9.4 di
    `sensori_e_navigazione.md`.
- **Immagini:** il diagramma dei danni usa `DAMAGE0–5.ENC`.

## 6. Scie della carta e selezione delle X (24/10/2026)

### 6.1 Scie (`2011:07E2–07F7`, `1207–1220`, `12EE`; disegno `1000:4C8C`)

**Buffer.** `DS:47BE` contiene **150 voci da 6 byte** (id, x, y in unità intere), cioè 900 byte. È circolare: l'indice
`DS:47BC` sale di 6 e torna a 0 a 900. All'avvio (`1000:9087`) l'indice è a 0 e il buffer azzerato, quindi le voci
vuote valgono (id 0, 0, 0).

**Chi viene registrato, e quando.**
- **Contatore** `DS:47BA`: parte da 30. Scende di 1 **solo nel turno di osservazione del giocatore locale**
  (`07E2`): ricordo che c'è un osservatore per passo, a rotazione (`07BC–0839`).
- Quando arriva a 0 torna a 30 e si registra la **posizione del giocatore**: id, x, y di `+18` e `+1C`.
- Nello **stesso turno**, mentre il contatore vale ancora 30, il giocatore valuta i suoi bersagli. Per ogni
  contatto aggiornato con **qualità ≥ 800** (`11EE`) si registra la **posizione vera** dell'unità (`1210–1220`: id,
  x, y del bersaglio, non la stima).
- **Cadenza:** un punto ogni 30 turni del giocatore. Con N osservatori a rotazione sono circa 30 × N passi, per
  esempio 8 unità = 240 passi ≈ 27 s di gioco. Il 688 e i contatti solidi condividono le 150 voci, quindi con
  molti contatti la scia è più corta.

**Disegno** (pulsanti ALL TRACKS e TARGET TRACK del §3.2):
- per ogni voce un **pixel** (`AEE7`) in x = (x − X0)/S + 8, y = 120 − (y − Y0)/S, se cade dentro il riquadro
  (x 8–176, y 28–120);
- con TARGET TRACK si disegnano solo le voci con id = bersaglio (`DS:0368`);
- colori: 4 il bersaglio, 8 il proprio battello, 0 le altre unità.
- **Difetto:** le voci ancora vuote valgono id 0 in (0, 0). Se il giocatore è l'unità 0, appaiono come
  scia propria nell'angolo sud-ovest della mappa, visibili solo con la carta su quell'angolo.

**Nel motore** (24/10/2026): `Missione::scia()` restituisce i punti scritti (al più 150, nell'ordine dell'anello;
pieno, il più vecchio segue l'ultimo scritto), con la posizione in metri. Il difetto delle voci vuote non c'è: un
anello non ancora pieno ha solo i punti registrati.

### 6.2 Selezione delle X e dei contatti col mouse (`1000:802A`, zone `ED9E`, scelta `DBFB`)

**Zone.** Per ogni voce della lista dei contatti disegnati (`DS:08CC`, §2.2) TARGET crea una zona attiva con
`ED9E(0Dh, x−2, y−2, x+5, y+5, 0Fh, codice, k+1)`:
- è un riquadro di **8 × 8 pixel**, da (x−2, y−2) a (x+5, y+5), attorno al punto in cui è scritto il segno;
- prima di creare la zona, x è limitato a 9–167 e y a 26–112, cioè al riquadro della carta. Con l'imager attivo e
  `DS:B458` > 1 si sommano (8, 1Bh);
- **codice:** la **lettera** del contatto per le tracce solide, oppure **80h + indice** nella lista per le «X»;
- **gruppo:** k + 1, il numero d'ordine.

**Quali contatti entrano nella lista:**
- le lettere solo se sono **dentro** il riquadro della carta;
- le «X» **sempre**, anche quando il segno non è disegnato perché cade fuori. La zona finisce sul bordo, alla
  posizione limitata.

**Scelta** (`DBFB`, tabella dei tasti a `CS:DE35`):
- **mouse:** il cursore sceglie la zona che contiene il punto (`C975` → `E340`). Se più zone si sovrappongono, come
  le «X» con rilevamenti vicini, vince la **prima registrata**, cioè quella più avanti nella lista dei contatti
  (`E340` scorre dall'inizio);
- **lettera:** sceglie la zona con quel codice (`E3B6`; minuscole come maiuscole). Le «X» hanno codice ≥ 80h e
  **non si possono digitare**;
- **Tab / Maiusc+Tab e frecce:** passano da una zona all'altra, **comprese le «X»**; **Invio** conferma;
- **cifre 0–9:** saltano al gruppo con quel numero (`E7F8`);
- Esc o un tasto di stazione chiudono. Per l'esito, l'uscita e il tempo fermo vale il §2.2.

**Dalla zona al bersaglio** (`8013–8027`):
- codice < 80h: si cerca nella lista la voce con quella lettera, e **vince l'ultima** se ce n'è più d'una;
- codice ≥ 80h: si prende la voce (codice − 80h).

`DS:0368` diventa l'id dell'unità di quella voce.

## 7. Dove girano gli script e i salti su se stessi (25/10/2026)

### 7.1 Gli script girano nell'interrupt del timer (`2011:0162–0261`)

**L'ISR del timer** (`2011:0162`) fa queste cose, in ordine:

1. manda subito l'EOI al PIC (`out 20h`) e fa **`sti`**;
2. chiama il vecchio int 08h quando `DS:1F40` va in riporto;
3. un interrupt su due (`DS:1F44`), se il gioco non è in pausa (`DS:1F3E`, scritto solo dal codice principale, per
   esempio durante i suoni dell'analizzatore), fa un passo della simulazione:
   - ogni 18 passi (`DS:5100`) il tick: i timer `AT` scaduti (lista `DS:50FA`) diventano messaggi 6 nella coda `DS:5FE9`;
   - poi il moto (`083E`) e la scoperta (`0793`);
   - infine **un** messaggio della coda (`0262`, tabella `DS:661E`). È qui che partono gli script:
     - il messaggio «esegui» (`027A`) copia i 100 byte dell'unità nel modello `DS:2BEE` (`000C`);
     - poi la VM (`1640` → `137C`) esegue fino a `END`, opcode 0.

**La VM non cede mai il turno:** il ciclo `137C–1397` legge un opcode e chiama il gestore (`DS:6505`), senza limiti.
È l'unico riferimento alla tabella in tutto l'eseguibile; lo usa anche l'avvio della scena (`1000:97A8` con `5105`).

**Non ci sono guardie contro la rientranza.** Se uno script gira a lungo, gli interrupt successivi rientrano
nell'ISR e fanno altri passi completi (moto, scoperta, timer, messaggi), mentre lo script esterno resta fermo nel
suo ciclo. Il codice principale (schermo e tastiera) non riprende finché lo script esterno non finisce.

### 7.2 `IF_TGT_NEAR` che salta su se stesso (SCENE9A `04F7`, SCENE9B `05F1`)

`53 c8 00 f7 04`, cioè `IF_TGT_NEAR 200, 04F7`. Il gestore (`1C4F`):

- prende il bersaglio dal modello (`+58` → `+2`);
- confronta `|dx|` e `|dy|` con 200 (4 nm per asse);
- se vale, salta (`si = 5105h + etichetta`).

La decodifica è giusta: l'istruzione salta davvero su se stessa. Nell'originale quindi è **un'attesa attiva**:
lo script resta lì finché il bersaglio, che i passi annidati muovono davvero, non esce dal quadrato di 4 nm attorno
alla posizione che l'unità aveva nel modello all'ingresso. Poi prosegue: `UFLAGS_INC`, la quota a 1200 piedi,
la virata, il missile.

**Difetti dell'originale durante l'attesa:**

- l'interfaccia è ferma;
- i messaggi eseguiti dai passi annidati ricaricano il modello `DS:2BEE` e l'id `DS:5CBD` con un'altra unità.
  Il confronto e i comandi dopo l'attesa possono quindi finire sull'unità sbagliata.

**Nel remake** (`tools/re/missioni_lua.py`) un IF che salta su se stesso diventa: «se la condizione vale, timer di
0,05 s sulla stessa etichetta e fine». L'etichetta diventa una funzione, e si riprova al passo successivo con un
modello fresco. Prima dell'attesa i due script hanno solo condizioni, quindi non si perde niente del modello.

Gli unici due casi nelle 22 scene sono questi. Lo dice `tools/re/prove/cicli_vm.py`, che cerca i cicli del
bytecode senza uscita. L'unico altro ciclo che trova, in SCENE9C `0414–0444`, sta in codice non raggiunto.
