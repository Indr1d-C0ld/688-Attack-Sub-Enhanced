# Sensori, eventi acustici, navigazione e comandi del giocatore nell'originale

Verifica sulla VM, del 23/10/2026, delle approssimazioni del runtime classico del motore (`engine/missioni/`).
Le fonti sono il disassemblato di `GAME.EXE` scompattato (`re/unpacked/GAME.EXE`, segmento 2011h):
- `0C23–1180`: coppia osservatore/bersaglio;
- `083E`: moto;
- `0533–0691`: navigazione e arrivo;
- `1F3C–21AC`: dispatcher dei comandi;
- `0793` e `17C9–1872`: eventi acustici.

Correggono e completano `docs/04_vm_missioni.md` §1.1 e §2.2.

## 1. Unità di distanza

- **Sensori e navigazione misurano in quarti di unità.** La differenza delle coordinate a 32 bit (parte
  intera in `+18`/`+1C`) viene spostata a sinistra di 2 bit e se ne prende la word alta (`0C5C–0C9B`,
  `05D9–0622`). Quindi in questi confronti **1 nm = 200**, non 50.
- Le soglie che nel documento precedente erano in "unità" vanno divise per 4: per esempio 4000 = 1000
  unità = **20 nm**, non 80.
- `IF_TGT_NEAR` invece confronta le parti intere (`1C4F`): lì 50 = 1 nm, come già convertito nelle missioni.

## 2. Flag `+54`: schiera, periscopio, antenna

| Bit | Opcode | Significato | Prova |
|---|---|---|---|
| `01h` | `F54_1` (30h) | **Schiera trainata filata**: ascolto su ±90° invece di ±30°; `+3B` += 2 | gestori 34h/35h: filata dopo 30 tick (60 s) e messaggio 17 «towed array deployed», recuperata dopo 20 tick (40 s) e messaggio 18 |
| `02h` | `F54_2` (2Fh) | **Periscopio alzato** | comandi 33h/32h da `1000:25A9` («periscope raised/lowered», richiede quota ≤ A0h) |
| `04h` | `F54_4` (5Ch) | **Antenna alzata** | comandi 3Fh/3Eh da `1000:68D1` («antenna has been raised/lowered») |

Nelle missioni convertite le funzioni ora si chiamano `m688.schiera_trainata`, `m688.periscopio` e
`m688.antenna`, con argomento 0 o 1. Prima si chiamavano `sonar_attivo`, `flag_sonar_2` e `flag_sonar_4`.
Il campo del modello è `schiera_trainata`. Per le unità dell'IA il bit 0 vuol dire semplicemente
«ascolto a ±90°»: lo hanno quasi tutte le navi da guerra e i sottomarini sovietici.

**Difetto dell'originale.** `F54_1 0` toglie sempre 2 a `+3B`, anche se la schiera era già dentro. Due
comandi di recupero di fila, per esempio 35h e poi 36h, abbassano la sensibilità di 4.

**Il sonar attivo non è un flag ma un evento acustico (§4).**

## 3. Modello di scoperta (`2011:0C23–1180`)

`si` è l'osservatore e `di` il bersaglio; `|dx|`, `|dy|` sono in quarti di unità.

### 3.1 Cadenza (`07BC–0839`, `1350–137B`)

Riletta il 24/10/2026 (analisi dell'impatto, §9.8); la prima lettura («ogni unità valuta almeno un bersaglio a
ogni passo», con `DS:1F56` recupero degli interrupt persi) era sbagliata.

- **La lista delle unità** va dalla più recente alla più vecchia: `0116` mette la nuova unità in testa
  (`DS:1F58`), riporta lì la ricerca dell'osservatore (`DS:1F5C`) e la rotazione delle visite (`DS:1F5E`), le dà
  se stessa come primo bersaglio (`+12`) e incrementa `DS:1F56`. `003C`, quando un'unità esce, rimanda alla
  testa `DS:1F5C`, `DS:1F5E` e i `+12` che puntavano a lei.
- **Un solo osservatore per passo** (9 al secondo). `1350` parte da `DS:1F5C` e scorre la lista:
  - salta le unità con il flag 40h (non combattenti: mercantili, petroliere, biologici, il Sacramento, i bersagli
    delle esercitazioni e alcune navi da guerra, per esempio Slava e Koni) e quelle dell'IA che
    affondano (`+55` ≠ 0, id ≥ 2);
  - alle altre scende il conto `+05`; la prima che arriva a zero osserva e il conto riparte da `+04`, il byte
    dell'opcode `TYPE` (2011:13BF scrive i due byte). Un'unità osserva quindi una volta ogni `+04` giri: 1 per
    quasi tutte e per le armi, 2, 3 o 20 per alcune (con 0, una volta ogni 256);
  - **difetto:** se arriva in fondo alla lista senza trovarne una, osserva la testa senza alcun controllo, che
    prende come `+04` il byte dell'ultima unità esaminata.
- **I bersagli** (`07FA–0826`): l'osservatore valuta `DS:1F56` bersagli (almeno 1) dal suo cursore `+12`, a giro
  sulla lista, saltando se stesso senza contarlo; poi il cursore resta sul successivo. `DS:1F56` è il numero
  di unità create nella missione (parte da 0 in `1000:9150`, non cala mai), quindi l'osservatore passa in
  rassegna tutte le altre, e quando ne sono uscite dal gioco (siluri, esche) qualcuna anche due volte o più.
- La ricerca successiva parte dall'unità che segue l'osservatore, o dalla testa dopo l'ultima. Se l'impatto
  toglie l'osservatore (`0C23` restituisce ≠ 0) il passo finisce lì.
- L'osservatore giocatore locale chiama `12EE` ogni 30 osservazioni (`DS:47BA`); non serve al runtime.

**Effetto.** Con E osservatori pronti, N unità in gioco e C create, ogni osservatore osserva circa ogni E passi
e valuta ciascun bersaglio C/(N − 1) volte: una coppia viene valutata circa C/((N − 1)·E) volte per passo.
Prima il runtime dava a ogni unità un bersaglio per passo (circa 1/(N − 1)). Le non combattenti non scoprono
più nulla; le altre scoprono tanto più spesso quante più non combattenti ci sono. Nelle 22 scene, con la
disposizione più probabile, osservano da 2 a 8 unità su 6–11, e ciascuna valuta i suoi bersagli da 1,3 volte
(SCENE8A, SCENE8B) a 4 volte (SCENE1B, SCENE5B) più spesso di prima. Gli esiti misurati sono in
`engine/README.md`, «Danni nelle missioni».

**Nel motore** (`Missione::passo_sensori`, 24/10/2026): come sopra, compreso il difetto della testa; la lista è
per id decrescente (le unità create hanno id crescenti), `tipo` delle unità iniziali viene dal convertitore
(`docs/09_schema_missioni.md` §4). Fra due armi l'osservatore non valuta nulla (`0C25`: né colpo né scoperta; sulle
110 partite del rilievo non cambia niente). La collisione delle armi si controlla nelle stesse coppie (§9.8), salvo
nella regola di simulazione. Anche la
rotazione delle visite (`02F3`, `DS:1F5E`, §9.2) segue la stessa lista e riparte dalla testa come l'originale.

### 3.2 Scoperta automatica (segnale 6000, durata ≥ 1400)

- **Proprio lanciatore**: il bersaglio ha l'osservatore in `+0C`. È il caso delle armi del lanciatore.
- **Osservatore nave o aereo** (flag & 6):
  - **bersaglio in superficie** (quota ≤ 10 ottavi di piede) entro **1800 = 9 nm** per asse;
  - **bersaglio con periscopio o antenna alzati**: è l'intercettazione radar ed ESM.
    - La portata è 900 = **4,5 nm** con il periscopio e +450 con l'antenna, fino a **6,75 nm**.
    - Non dà la scoperta automatica: dà un bonus intero alla qualità della traccia a ogni valutazione
      (`DS:476A`), pari a (85 − (|dx|+|dy|)/32)/4, e un contatto con segnale 550 quando la scoperta passiva
      (§3.3) è possibile ma fallisce: G ed E nulli (`0EDC`) o T non basta (`1003–1016`). Senza sonar
      dell'osservatore, senza rumore del bersaglio o oltre 20 nm non c'è contatto; dentro la portata passiva
      vale il segnale passivo, anche se minore di 550. Con bonus 0 (livello < 4) niente contatto garantito.
      Nel motore così dal 24/10/2026; prima ogni segnale con l'intercettazione saliva a 550.
    - Al livello `DS:0331` ≠ 0 il bonus è diviso per 4 (il ramo `IF_STANDARD`).
- **Osservatore sottomarino con il periscopio alzato**: navi e aerei entro **9 nm** per asse, a vista.

### 3.3 Scoperta passiva

Non c'è scoperta se il sonar dell'osservatore `S` = `+3B` vale 0, se il rumore del bersaglio `N` = `+3A` vale 0,
oppure se `|dx|` o `|dy|` ≥ 4000 (**20 nm**). Altrimenti:

```
rel = |rotta osservatore − rilevamento del bersaglio|        (0..4500 = 0..180°)
G   = senza schiera: floor((1000 − rel) / 256)   se rel < 1000, cioè 3/2/1 entro ±9°/±20°/±30°, poi 0
      con schiera:   2·floor((2500 − rel) / 256) se rel < 2500, cioè fino a 18, non nullo entro ±90°
E   = somma degli eventi acustici (§4)
se G = 0 ed E = 0: nessuna scoperta (resta solo l'intercettazione radar/ESM del §3.2)
f   = 3 se la velocità del bersaglio supera avanti tutta, altrimenti 1
Nb  = (1 + floor(8 · v_bersaglio / avanti_tutta_bersaglio) · f) · N
      (difetto DS:1F64: se il bersaglio sta cambiando velocità, +2E ≠ 0, Nb = 1)
Nt  = (Nb + max(0, 40 − floor(32 · v_osservatore / avanti_tutta_osservatore))) · S
per ogni termoclino fra le due quote: G = G / 2, Nt = Nt / 4    (E non si attenua)
T   = G + Nt + E                                                 (portata in unità di gioco)
contatto se T > |dx| e T > |dy|, con dx e dy in unità; segnale = T − (|dx| + |dy|) / 2
```

Il termine `40 − velocità propria` è la **capacità d'ascolto dell'osservatore**: conta più del rumore del
bersaglio. Un osservatore fermo sente qualsiasi bersaglio nel settore di prua fino a 10–17 nm.

### 3.4 Qualità e durata della traccia (record contatto, pool `DS:2C52`)

- **Contatto nuovo.** Se il segnale è > 500, la qualità parte da (segnale − 500)/4. Altrimenti la qualità
  è 0 e parte il **lampeggio** (sotto). In ogni caso il contatto nuovo passa subito anche dall'aggiornamento,
  con differenza di segnale 0 (`10CB–10D7`): con segnale > 500 ha durata 1 e qualità + 1 + bonus.
- **Scoperta automatica** (§3.2): un contatto che c'è già prende durata 1400 *prima* dell'aggiornamento
  (`0DA4`, un'assegnazione, non un massimo), quindi 1401; un contatto nuovo parte da 0 come gli altri.
- **Aggiornamento con segnale > 500.**
  - La durata `+14` cresce di 1; con il periscopio dell'osservatore alzato vale almeno 800.
  - La qualità `+0E` cresce di (differenza)/4 + 1 col segno della differenza fra segnale e segnale
    precedente (`114C–1160`: con segnale in calo scende di |differenza|/4 + 1), mai sotto 0, poi si
    aggiunge il bonus radar/ESM. La somma è telescopica: qualità ≈ (segnale − 500)/4 + numero di valutazioni.
  - **Il limite a 1200 non funziona:** `1170–1175` riscrive lo stesso valore. La qualità può superare 1200.
  - Il lampeggio non cambia: resta com'era.
  - Seguono la posizione stimata e la scelta del bersaglio (§8).
- **Aggiornamento con segnale ≤ 500** (`10DF–1134`). Se varia di più di 5, la qualità torna a 0 e il
  lampeggio riparte; poi un passo del lampeggio, e la valutazione **finisce**: niente posizione stimata e
  niente scelta del bersaglio.

**Lampeggio dei contatti deboli** (`+06` parola, `+12` byte; `10A5–112E`). Il byte basso di `+06` è un conto
alla rovescia, quello alto un conto in salita; `+12` ha il bit 80h (nascosto) e 7 bit di conto.
- **Partenza:** n = (550 − segnale)/8 + 1, `+06` = n (conto in salita 0), `+12` = n | 80h.
- **Passo** (ogni valutazione con segnale ≤ 500, anche quella che parte):
  - con 80h il conto di `+12` scende; quando passa sotto zero `+12` prende il conto in salita, senza 80h
    (visibile), poi il conto alla rovescia scende di 1 e quello in salita sale di 1;
  - senza 80h il conto scende; quando passa sotto zero `+12` prende il conto alla rovescia con 80h
    (nascosto), oppure 0 se il conto alla rovescia è a zero: da lì il contatto resta visibile.
- **Effetto:** fasi nascoste di n, n, n − 1, …, 2 valutazioni alternate a fasi visibili di 1, 2, 3, …, poi il
  contatto si vede sempre. Con segnale vicino a 0 (n ≈ 69) servono circa n² ≈ 4800 valutazioni; un
  bersaglio in movimento fa variare il segnale di più di 5 e il lampeggio riparte, quindi un contatto debole
  resta quasi sempre nascosto. Con segnale > 500 `+12` resta congelato: un contatto nato debole e poi
  rafforzato può restare nascosto finché la qualità non arriva a 800.
- Lo legge solo l'interfaccia (`990C`, §10.5); l'IA non ne tiene conto.
- `IF_TRACK_GOOD` vuole qualità ≥ 800, quindi da 600 a 800 valutazioni con segnale sopra 500. Un evento
  acustico o la scoperta automatica portano il segnale a migliaia e la traccia diventa subito solida.

**Nel motore** (`Missione::valuta`, 24/10/2026): come sopra. Il lampeggio è in `Contatto::conto_giu`,
`conto_su` e `lampeggio` (`lampeggio_da_capo`, `passo_lampeggio`); `Contatto::visibile()` è il test di `990C`
e serve alla carta e alla barra. Prima di questa data il contatto debole aveva un conto alla rovescia semplice
che nessuno leggeva, la valutazione con segnale ≤ 500 aggiornava la posizione stimata e faceva avanzare la
scelta del bersaglio, il contatto nuovo saltava il primo aggiornamento e la qualità cresceva di 1 anche con
segnale in calo.

### 3.5 Portate tipiche (bersaglio in prua, nessuno strato salvo dove indicato)

| Caso | Portata T (unità) | Contatto entro (nm, per asse) | Traccia che cresce entro (nm, un asse) |
|---|---|---|---|
| Alfa ferma ascolta il 688 fermo | 831 | 16,6 | 13,2 |
| Alfa ferma, 688 a 8,5 kn (1/4) | 1000 | 20,0 | 20,0 |
| Alfa a 19 kn (1/2), 688 fermo | 543 | 10,9 | 1,7 |
| Alfa ferma, 688 fermo, uno strato in mezzo | 208 | 4,2 | — |
| Alfa ferma, 688 fermo, due strati | 51 | 1,0 | — |
| Forrest Sherman a 1,8 kn (telegrafo 20), 688 fermo | 502 | 10,0 | 0,1 |
| Forrest Sherman a 1,8 kn, 688 a 8,5 kn | 634 | 12,7 | 5,4 |
| Perry (sonar 8) ferma, 688 fermo | 386 | 7,7 | — |
| Kirov a 2,1 kn, 688 fermo | 810 | 16,2 | 12,4 |
| 688 fermo ascolta un'Alfa ferma | 867 | 17,3 | 14,7 |
| 688 fermo ascolta un mercantile a 13 kn | 1000 | 20,0 | 20,0 |
| 688 a 33,8 kn ascolta un'Alfa ferma | 291 | 5,8 | — |

«Traccia che cresce» vuol dire segnale > 500, cioè distanza < 2·(T − 500) con l'altro asse a 0.

### 3.6 Confronto con il runtime del motore

| Motore (23/10/2026) | Originale |
|---|---|
| portata 30 × rumore × fattore di velocità × sonar/18 unità | T = G + S·(Nb + 40 − ⌊32 v_oss/v_tutta⌋) + E. È additivo: il termine d'ascolto dell'osservatore pesa più del rumore. Per il 688 fermo ascoltato da un'Alfa ferma dà 831 unità (16,6 nm), contro le 180 della stima |
| scoperta su tutto l'orizzonte | **solo nel settore di prua**: ±30°, oppure ±90° con la schiera (bit 0 di `+54`). Fuori settore servono un evento acustico o la scoperta automatica |
| strato: × 0,4 | per strato G/2 e termine di rumore /4; gli eventi non si attenuano |
| 1350 unità con il sonar attivo | 1350 sono **quarti** di unità (6,75 nm), e sono il radar/ESM delle navi e degli aerei contro periscopio (900) e antenna (+450) alzati. Il sonar attivo è l'evento `PING` (§4) |
| limite | 20 nm per asse (4000 quarti) |
| qualità | come §3.4: cresce di circa 1 per valutazione con segnale > 500, il segnale si somma, nessun limite reale a 1200 |

## 4. Eventi acustici (lista `DS:4772`)

Ci sono al massimo 12 eventi, ciascuno con l'id dell'unità che lo emette e una durata in passi (`DS:478A`)
che scende di 1 a ogni passo (`0793`).

| Opcode / comando | Nome (prima) | Evento | Durata | Messaggio agli altri giocatori |
|---|---|---|---|---|
| `2A` `PING` | `KILLED`, `m688.distrutta` | **impulso del sonar attivo** | 300 passi (33 s) | 8 «active sonar»; se l'unità è la 0, variabile di scena 0 + 1 |
| comando 2Dh (IMPULSO BERSAGLIO) | — | ping del giocatore (stessa routine `1843`) | 300 passi | 8 |
| `5A` `MISSILE` | `LOST`, `m688.persa` | **lancio di missile** | 900 passi (100 s) | 30h «detected a missile launch» |
| `73` `TRANSIENT` | `MARK_100`, `m688.registra_evento` | **rumore transitorio** (lancio di siluro, ecc.) | 100 passi (11 s) | — |

Le funzioni Lua ora si chiamano `m688.impulso_sonar`, `m688.lancio_missile` e `m688.rumore_transitorio`.
Gli usi negli script lo confermano:
- **TORPEX:** il SOSUS chiama `PING` tre volte di fila, cioè i «tre impulsi sonar» con cui il briefing
  chiude l'esercitazione.
- **Navi in attacco di TORPEX:** ripetono `PING` ogni 5 tick, cioè 10 s (SCENE1A `L_0484`/`L_04a0`, `AT +5`).
- **Siluri a ricerca attiva:** Spearfish e Mk 50 chiamano `PING` ogni 10–16 s dopo l'abilitazione.
- **Boa marcatrice:** non usa `PING`. Ha un rumore costante di 255, ascoltato dal sonar passivo.
- **Missili:** il gestore dell'Harpoon chiama `MISSILE` con il messaggio 49.

Effetto sul modello di scoperta (`0E5F–0ECC`):
- **L'evento è del bersaglio.** E += 1000, 2000 o 5000. La traccia è subito solida: chi fa un ping o lancia
  un missile si rivela a tutti entro 20 nm, in ogni direzione.
- **L'evento è dell'osservatore.** Per i bersagli entro ±72° dalla prua, E += 2200 (±36°), 1400 (±52°) o
  800 (±72°). Il ping illumina il settore di prua: è proprio la descrizione del manuale (IMPULSO
  BERSAGLIO, cap. 5).
- **Difetto.** La scelta fra 1000, 2000 e 5000 dovrebbe dipendere dalla durata residua, ma legge la word
  successiva della lista degli id (`[bx+2]`). In pratica vale quasi sempre 1000, che basta comunque a
  coprire i 20 nm.

## 5. Navigazione e arrivo (`2011:0533–0691`)

Modo di navigazione in `+4E` (bit 2–3): 4 = `GOTO_XY`/`GOTO_TGT`, 8 = `PURSUE`, 0Ch = `PURSUE2`.

- **`PURSUE`.** Finché la qualità della traccia è < 800 accosta solo sul rilevamento e non controlla
  l'arrivo. Poi naviga verso la posizione stimata del contatto (`+08`/`+0A`) e, se **chi insegue** non è una
  nave o un aereo, ne uguaglia la quota stimata (`057F–05A2`, corretto il 24/10/2026: prima si leggeva come
  condizione sul bersaglio). Un siluro quindi sale alla quota di una nave e la finestra d'impatto di 100 ft
  basta (§9.8).
- **Soglia `s` = `+50` in quarti di unità per asse.** L'arrivo scatta quando valgono **almeno 2 su 3**
  condizioni: |Δx| ≤ s, |Δy| ≤ s, |quota − M| ≤ s.
- **Difetto della terza condizione.** M dovrebbe essere la quota della destinazione, ma è la word
  `DS:(s + 1Eh)` dell'immagine. Con i valori statici:

| Soglia nelle missioni | Metri per asse (scala di gioco) | Usi | Word `DS:(s+1Eh)` | Effetto della terza condizione |
|---|---|---|---|---|
| 0 (`insegui`) | 0 | 22 | 14392 | mai: il siluro insegue finché colpisce o finisce la corsa |
| 15 | 139 | 1 | 46 | solo tra 4 e 7,6 piedi |
| 20 | 185 | 34 | 8300 | mai |
| 30 | 278 | 247 | 29537 | mai |
| 40 | 370 | 48 | 2573 | solo tra 317 e 327 piedi: lì basta un asse |
| 60 (`vai_al_bersaglio`) | 556 | 2 | 28018 | mai |
| 120 | 1111 | 1 | 0 | sempre per le unità sopra i 15 piedi (le navi): basta **un** asse entro 30 unità |

- **Accostata** (`0705–0790`, ricavato il 24/10/2026): vale per ogni unità che naviga verso un punto, armi
  comprese.
  - Se la rotta da prendere differisce di oltre 75 unità (3°) da quella attuale e non c'è già una velocità
    salvata in `+5C`, la velocità (l'ordinata se sta accelerando, altrimenti l'attuale) viene salvata in `+5C` (al
    più avanti tutta: da MAX si torna a TUTTA) e l'ordinata scende a un quarto di avanti tutta. Al giocatore
    locale arriva il messaggio **47** «Riduco la velocità per accostare».
  - Entro 3° la velocità salvata torna ordinata e `+5C` si azzera, senza messaggio.
- **`DEPTH_TO`** (`175D`): la quota ordinata è limitata da `+5A`; se il battello è già a quella quota non cambia
  nulla; il rateo è in 1/256 di `+38`, quindi **0 vuol dire nessun cambio di quota**. Così i timoni orizzontali a
  zero (codice 17h) tengono la quota.
- **`PURSUE2`** (0Ch) rovescia i confronti: l'arrivo scatta quando almeno 2 condizioni su 3 sono ≥ s.
  Serve a staccarsi dal bersaglio.
- **Nel motore.** La soglia in metri è `s / 4 × metri_per_unita` per asse, con entrambi gli assi
  richiesti; per s = 120 sulle navi basta uno solo dei due assi.

## 6. Moto: fattore 3,46

- A ogni passo la posizione cresce di `velocità grezza × sin/cos(rotta) / 65536` unità, con 9 passi al secondo.
- 1 kn mostrato (350 grezzi) vale quindi 0,048065 unità al secondo. In tempo reale sarebbero
  1852/3600/37,04 = 0,01389 unità al secondo: il moto è **3,46 volte** il vero.
- Ratei di virata, quota e accelerazione restano in gradi/s, m/s e kn/s.
- **Per la versione classica** una velocità in nodi va convertita in metri al secondo come
  `kn × 0,048065 × metri_per_unita`. Il valore è già calcolato in ogni missione come
  `posa.moto_classico_m_s_per_nodo`:

| Posa | Metri per unità | m/s per nodo (classico) | Rapporto con il vero (0,5144) |
|---|---|---|---|
| scala di gioco (Barents, Celtica) | 37,04 | 1,780 | 3,46 |
| Fær Øer ricollocata | 23,3 (×0,63) | 1,12 | 2,2 |
| Baltico ricollocato | 45,9 (×1,24) | 2,21 | 4,3 |
| Gibilterra (georef) | 23,0 / 27,8 | circa 1,2 | circa 2,4 |

- Senza il fattore i tempi del gioco (15 minuti di TORPEX, corse dei siluri, timer) non tornano con le
  distanze. Il Mk 48, per esempio, percorre 13,5 nm di gioco in 280 s solo con il fattore.

## 7. Comandi del giocatore e parametri dei gestori

Il dispatcher scrive i parametri negli argomenti della prima istruzione del gestore, o della seconda per
3Dh. Nelle missioni convertite i nomi in `gestori_giocatore` sono stati corretti: **`quota` e `timone`
erano scambiati**.

| Codice | Gestore (`gestori_giocatore`) | Prima chiamata | Parametri scritti (unità Lua) |
|---|---|---|---|
| 01h–07h | `telegrafo` | `telegrafo(u, f)` | "arresto_rapido", 0, 0,25, 0,5, 0,75, 1, 1,09375 |
| 08h–12h | `timone` (DS:639F) | `rateo_virata(u, g)` | nell'originale (codice − 0Dh) × 2 × 0,36 °/s, con il **positivo a sinistra** perché le rotte sono angoli matematici (`numeri_eseguibile.md` §3.3): 12h, il primo elemento del timone dove porta il tasto R, è tutta la barra a **sinistra**, 08h tutta a **dritta**, 0Dh barra al centro. Nelle missioni Lua (dal 24/10/2026) g è in convenzione bussola, cioè −(codice − 0Dh) × 2 × 0,36: da +3,6 (tutta a dritta) a −3,6 (tutta a sinistra). Poi `rotta(u, nil)`: virata libera |
| 13h–1Bh | `quota` (DS:639D) | `quota(u, m, r)` | m = 624,2 (in giù fino alla quota massima) se codice ≥ 17h, altrimenti 0 (in su); r = \|codice − 17h\| / 4 (17h = timoni orizzontali a zero) |
| 37h | `quota` | `quota(u, 6,096, 0,75)` | quota periscopica (20 ft) |
| 40h | `quota` | `quota(u, 45,72, 1)` | quota standard 150 ft |
| 41h | `quota` | `quota(u, 0, 4)` | emersione rapida |
| 2Ch | `punto_di_rotta` | `vai_a(u, x, y, …)` | punto di rotta (coordinate di gioco → metri con la posa) |
| 2Bh | `pilota_automatico_off` | — | — |
| 32h / 33h | `periscopio` | `periscopio(u, v)` | v = 0 abbassa, 1 alza |
| 3Eh / 3Fh | `antenna` | `antenna(u, v)` | v = 0 abbassa, 1 alza |
| 34h / 35h / 36h | `schiera_cala` / `schiera_recupera` / `schiera_persa` | — | filata in 60 s, recuperata in 40 s; 36h = schiera persa per danno o velocità |
| 38h | `designa_bersaglio` | `bersaglio(u, id)` | id del contatto designato |
| 39h | `posizione` | `posizione(u, x, y)` | sincronizzazione della posizione (modem) |
| 1Dh–23h | `carica_tubo_1..7` | — | — |
| 24h–29h | `armi.<tipo>.gestore` | — | lancio per tipo d'arma |
| 3Ah–3Ch, 3Dh | `guida_e0..e2` | — | guida a filo; 3Dh scrive X, Y nella seconda istruzione di `guida_e2` |
| 42h / 43h | `f0` / `f1` | — | — |
| 2Dh | (nessuno) | — | ping del sonar attivo: evento acustico (§4) |
| 31h | (nessuno) | — | messaggio di testo via modem |

Il bit 7 del codice indica il secondo giocatore (`DS:5CBD`).

## 8. Scelta del bersaglio e script dell'unità (`2011:1178–12E6`, `1316`, `19F0`)

**L'originale non sceglie il contatto con la qualità migliore.** Ogni contatto ha un conto alla rovescia
(byte `+16` del record contatto).

- **Contatto nuovo.** Nasce con conto 1.
- **`ENGAGE n`** (`m688.ingaggia`, 19F0h):
  - aggancia il bersaglio corrente in `+60`;
  - porta il suo conto a *n*; negli script n vale 10, 15, 20, 30 o 40, e raramente 2 o 5;
  - imposta lo script del bersaglio (`+10`) e quello del bersaglio perso (`+56`).
- **`BEHAVIOR`** imposta solo `+10`.

Quando l'osservatore aggiorna un contatto con segnale > 500 (§3.4; con segnale ≤ 500 la valutazione finisce
prima):
1. Se non ha uno script `+10`, non succede niente.
2. Se ha un contatto agganciato `+60`, conta soltanto quello.
3. **Armi** (flag 20h): il conto scende solo se la qualità è > 800, e un'arma non sceglie come bersaglio
   un'altra arma.
4. Il conto scende di 1. Quando arriva a 0, il contatto diventa il bersaglio `+58` e lo script `+10` viene
   eseguito **una volta**, con un pacchetto di tipo 2 nella coda degli eventi. Poi `+10` e `+60` si azzerano.

Quindi:
- **`+10` non è periodico.** È lo script «al bersaglio»: scatta alla prima valutazione di ogni contatto
  nuovo, oppure dopo *n* aggiornamenti del contatto agganciato con `ENGAGE`. Gli script lo riarmano da soli
  con `ENGAGE`/`BEHAVIOR`; i controlli periodici veri usano i timer (`AT`).
- **Senza filtro di schieramento.** Il bersaglio può essere amico, neutrale o un biologico, perciò gli
  script controllano `IF_TGT_ENEMY`, `IF_TGT_F20` e simili. Un blocco che filtrava per schieramento e per
  durata ≥ 1400 (`11B7–11E6`) è codice morto, senza riferimenti.
- **Ordine.** Fra più contatti nuovi vince l'ultimo che arriva a 0 nel giro di valutazione: lo script
  vede quello. A ogni scatto il bersaglio precedente viene sostituito.

**Contatto perso** (`1258–12E6`). Quando una valutazione non dà scoperta, l'osservatore perde subito il
contatto: basta che il bersaglio esca dal settore di prua. Fanno eccezione le unità con `+0C` non nullo,
cioè quelle lanciate da un'altra unità: tengono il contatto con segnale 1 finché la durata è ≤ 450 o `+62` ≤ 1.
- Se il contatto perso era il bersaglio `+58`, `+58` torna a 0.
- Se era il contatto agganciato `+60` o quello inseguito (`+14`, `PURSUE`), l'originale azzera anche la
  navigazione verso di esso e lo script `+10`, ed **esegue lo script `+56`**, che quindi vuol dire
  «bersaglio perso».

**Posizione stimata del contatto.** Con qualità ≥ 800 i campi `+08`, `+0A` e `+0C` del contatto prendono a
ogni aggiornamento la posizione e la quota vere del bersaglio (`11EE`). Sotto 800 restano all'ultimo valore.
Il rilevamento `+04` si aggiorna sempre.

**Cadenza** (§3.1). I conti alla rovescia di `ENGAGE` vanno in valutazioni del contatto agganciato, e
l'osservatore ne fa C/(N − 1) a ogni sua osservazione, una ogni E passi (E osservatori pronti, N unità in gioco, C
create). Per esempio `ENGAGE 30` con 8 unità, di cui 4 non combattenti e nessuna uscita dal gioco, fa scattare
lo script dopo circa 30 × 4 / (8/7) ≈ 105 passi, 12 s.

Nelle missioni Lua il gestore si chiama ora `gestori.bersaglio` (prima `periodico`).

## 9. Fondale, incagli ed esito 1 (`2011:02F3–0533`, `22FC`, `1000:8A33–8D91`)

### 9.1 Da dove viene il fondale

- **È la carta SURF stessa.** `2011:22FC` prende la cella della carta originale sotto l'unità:
  - colonna = x / 32 e riga = y / 32, quindi 320 × 200 celle da 32 unità (0,64 nm, 1185 m alla scala di gioco);
  - righe compresse in RLE e 2 celle per byte, cioè 4 bit per cella;
  - risultato: il livello 0–15. **Fuori dalla carta vale 15, cioè terra.**
- **Profondità del fondo per livello** (tabella `DS:6622`, in ottavi di piede): F(L) = (14 − L) × 1170.

| Livello | 0 | 4 | 8 | 10 | 11 | 12 | 13 | 14–15 |
|---|---|---|---|---|---|---|---|---|
| Fondo (ft) | 2047 | 1462 | 877 | 585 | 439 | 292 | 146 | 0 |
| Fondo (m) | 624 | 446 | 267 | 178 | 134 | 89 | 44,6 | 0 |

- **Zona vietata.** Un'unità **immersa** (quota ≥ 10 ottavi, cioè 1,25 ft) sopra una cella di livello ≥ 13 è
  «a terra»: l'originale non lascia navigare in immersione dove il fondo è a 146 ft o meno. In superficie
  (quota < 10) è a terra solo dal livello 14.

### 9.2 Quando si controlla e su chi

- **Cadenza.** A ogni passo `02F3` esamina **una** unità, a rotazione sulla lista da `DS:1F5E`. Ogni unità viene
  quindi visitata circa ogni N passi. La rotazione va dalla più recente alla più vecchia e torna in testa quando
  nasce un'unità (`0116`) o esce quella da visitare (`003C`; l'uscita a fine affondamento, `035A`, non fa
  avanzare). Nel motore (`Missione::visita`) è così dal 24/10/2026; prima andava per id crescente.
- **Periodo dei controlli.** Il controllo vero si fa ogni `+4C` visite, il periodo impostato da `P_4C`; nelle
  missioni Lua è `modello.periodo_controlli`. Vale 1 quasi sempre e 3, 5 o 20 per alcune navi. I falsi
  bersagli (flag 80h) sono esclusi.
- **Solo le unità con il bit 0 di `+4E` controllano il fondale.** Il bit lo imposta `NAVFLAG`
  (`m688.navigazione`) e `CLEAR` lo azzera; nelle missioni Lua è `modello.controllo_fondale`. Ce l'hanno:
  - il 688, l'Alfa (salvo quella di TORPEX), Victor III, Akula, Oscar, Typhoon, Charlie II, Trafalgar;
  - i siluri Type 53 e parte dei Mk 48 e degli Spearfish.
- **Non ce l'hanno:** tutte le navi, gli elicotteri, i Foxtrot, i biologici, le boe e il bersaglio dei
  missili. **Le navi dell'originale ignorano il fondale e possono passare sopra la terra.**

### 9.3 Cosa succede (tra parentesi il messaggio al giocatore locale / all'altro giocatore)

**Giocatori** (id 0 e 1):

| Condizione | Effetto |
|---|---|
| sopra terra o acqua bassa (livello ≥ 13 immerso, ≥ 14 in superficie) | messaggio **9** «smashed into the bottom» (22 all'altro): **fine missione, esito 1**. Non c'è danno né arresto: la missione finisce |
| quota > F(L) (sotto il fondo), integrità ≥ 40 | integrità **−40**; quota portata subito a F(L) − 25 ft e rateo di quota a 0; sonar −2 e rumore +2 (`0C10`); messaggio **30** «we scraped bottom, hull damaged!» (46 all'altro). Il messaggio 30 danneggia inoltre, con probabilità 1/2, un sistema a caso fra 6, se non è già guasto (§9.4) |
| quota > F(L), integrità < 40 | messaggio **9**: fine missione, **esito 1**. La sottrazione di 40 fa traboccare il byte d'integrità (difetto), ma la missione è già finita |
| quota > F(L) mentre l'unità sta già affondando (`+55` ≠ 0) | messaggio **9**: esito 1 |

**Unità dell'IA**, solo con `controllo_fondale`:
- **Sotto il fondo:** quota ordinata F(L) − 25 ft e risalita al rateo pieno. **Nessun danno.** Vale anche per i
  siluri.
- **Sopra terra o acqua bassa, unità non arma:** si ferma del tutto (velocità, componenti del moto, ratei a 0).
  Non subisce danni e riparte solo se lo script le dà un nuovo ordine. Al controllo successivo si riferma.
- **Sopra terra o acqua bassa, arma:** è distrutta (`2011:003C`). Se è del giocatore locale compare il
  messaggio 38 «a torpedo was lost».

### 9.4 Danni ai sistemi, distruzione e allagamento

- **Danni ai sistemi** (`1000:8B39`). Li causano i messaggi 20 (bomba di profondità), 26 (colpiti e danneggiati)
  e 30 (fondo strisciato). Con probabilità 1/2 si guasta un sistema scelto a caso fra 6, se non è già guasto
  (tabella `DS:CC86`):

| Sistema | Effetto del guasto |
|---|---|
| sonar computer | `DS:13D6` = 0 |
| towed array | schiera persa: comando 36h |
| periscope | periscopio abbassato (32h) |
| targeting sonar | solo il guasto |
| engines | comandi di telegrafo forzati |
| antenna | antenna abbassata (3Eh) |

- **Distruzione** (`0B45`, `1CC1`). Quando un colpo supera l'integrità, `+07` va a 0 e parte il conto `+55`:
  - **IA:** 18 visite, poi l'unità viene tolta;
  - **giocatori:** 30 visite. Alla fine, se il battello è in superficie (quota < 20 ottavi, 2,5 ft) resta a
    galla come relitto (bit 80h di `+54`, integrità 1). Altrimenti compare il messaggio **25** «game over»
    (29 all'altro).
- **Allagamento.** Un relitto (bit 80h) che scende sotto i 2,5 ft dà il messaggio **54** «submarine flooded
  and destroyed!!!» e poi 25.

### 9.5 Esiti decisi dal programma (`1000:8A33`, tabella dei 19 messaggi a `CS:8AD5`)

| Messaggio | Esito (`DS:03E0`) | Testo nelle schede |
|---|---|---|
| 9 | **1** | incagliato (CODICE n-1-ACP) |
| 25, 54 | **2** | distrutto |
| 22 | 3 | l'altro giocatore si è incagliato (scene C) |
| 29 | 4 | l'altro giocatore è distrutto (scene C) |
| ≥ 100 dagli script | messaggio − 100 | gli altri esiti |

- Con `DS:0333` e `DS:032F` entrambi ≠ 0 gli esiti 1–4 diventano 15–18. `DS:0333` è legato al modem e
  `DS:032F` è il **lato** del giocatore (0 = 688, 1 = Alfa: byte 0 del blocco della scena, corretto il 24/10/2026
  in `numeri_eseguibile.md` §1.1), quindi vale per la partita a due quando si gioca l'Alfa.
- **L'esito 1 non lo imposta mai uno script.** Nessuna missione emette `messaggio(101)`: è sempre il
  messaggio 9 del controllo del fondale.

### 9.6 Altri controlli nello stesso giro (solo giocatori)

- **Strati termici.** Attraversare una quota di `LAYERS` dà il messaggio 31 «we crossed a thermal layer sir».
- **Limite di velocità.** Sopra i 100 ft (quota < 800 ottavi) la velocità è al massimo metà di avanti tutta,
  un quarto con il periscopio alzato. Oltre il limite la velocità ordinata viene ridotta e compare il
  messaggio 45 «adjusting engine speed sir».
- **Periscopio e antenna.** Sotto i 20 ft (quota > A0h) si abbassano da soli, con i messaggi 3 e 37.
- **Dove e per chi** (`0387–0452`, verificato il 24/10/2026): a ogni visita (`02F3`) di un giocatore (id 0 e 1). Gli
  strati termici solo per il giocatore locale, con la quota della visita precedente in `DS:1F46`; i messaggi solo
  al giocatore locale. Nel motore: `Missione::controlli_giocatore`.

### 9.7 Per il motore

Per una versione fedele su heightmap reale, con D = profondità del fondo:
- **Zona vietata in immersione:** D ≤ 44,6 m (146 ft, livello 13). Per il giocatore è esito 1; le unità
  dell'IA con `controllo_fondale` si fermano.
- **Sotto il fondo:** quota > D. Il giocatore perde 40 di integrità, risale a D − 7,6 m e ha i danni del
  §9.4, oppure esito 1 sotto 40. L'IA risale senza danni.
- **In superficie:** è terra solo con D ≤ 0.
- **Navi senza controllo del fondale:** nell'originale lo ignorano. Nel remake conviene comunque impedirgli la
  terra, perché è un limite dell'originale e non una scelta di gioco.

### 9.8 Impatto delle armi (`2011:0C23–0CF4`, `0B2D–0C10`, `0AD4`, `0B18`, `003C`)

Ricavato il 24/10/2026 dal disassemblato di `re/unpacked/GAME.EXE` (`tools/re/disasm16.py`,
`tools/re/righe_listato.py`). Confidenza alta salvo dove indicato.

**Dove.** La collisione sta nella routine delle coppie osservatore/bersaglio (`0C23`), prima del modello di
scoperta: nessuna routine propria per le armi.

**Regola** (`si` osservatore, `di` bersaglio):
1. Se entrambi sono armi (flag 20h) si esce: **le armi non si colpiscono fra loro**.
2. Distanze per asse in quarti di unità (|Δx|, |Δy|) e quota grezza in ottavi di piede.
3. Impatto se valgono tutte (disuguaglianze strette): **|Δquota| < 800** (100 ft, 30,5 m), **|Δx| < 20 e
   |Δy| < 20** (5 unità, 185 m per asse alla scala di gioco: un quadrato, fino a 262 m in diagonale), e nessuno
   dei due è il collegato (`+0C`) dell'altro.
4. Se l'osservatore è l'arma, la vittima non è un aereo (`+06 & 3` = 3), un'altra arma o un falso bersaglio
   (80h: esche, biologici, bersagli dei missili). Se l'osservatore non è un'arma e il bersaglio sì, l'osservatore
   non deve essere un aereo; poi si scambiano i ruoli.
5. **Qualsiasi unità**, non solo il bersaglio dell'arma (`+58` e `+14` non si leggono): **fuoco amico e
   neutrali vengono colpiti**, anche chi sta già affondando (l'arma si consuma e basta).
6. Il taglio del filo esegue `LINK 0`, che azzera `+0C` e lascia `+62`: da lì **il siluro può colpire il
   proprio lanciatore** (difetto; Mk 48 a +95 tick, 190 s; il 533 mm di SCENE1A a +30 tick).

**L'arma** (`0B2D`) esce subito dal gioco con `003C`: via tutti i suoi timer (anche la fine corsa), i
contatti e i riferimenti altrui; se era del giocatore locale scende `DS:03DE` (armi in acqua). Nessun evento
acustico, nessun messaggio 38. `DS:6503` = lanciatore (`+62`).

**Fuoco amico** (`0AD4`): lanciatore giocatore (id < 2) ancora in gioco e vittima non ostile (XOR di
`+06 & 18h` ≠ 18h, neutrali compresi): `DS:1F66`/`1F68` (riga in più nel rapporto) e, per il giocatore locale,
messaggio **67** «we just hit a friendly.....sir?». Il colpo si applica lo stesso.

**Danno** (`0B45`): `+3D` dell'arma (Mk 48 75, 533 mm 100), **dimezzato se `DS:5100` < 8**: il contatore scende
da 18 a 1 a ogni passo (`01E1`), quindi in 7 passi su 18. Poi integrità − danno:
- **≥ 0, sopravvive:** `+07` = resto; `+5E` = 1 al primo colpo, C9h ai successivi; `0C10` su tutte le vittime
  (sensibilità `+3B` − 2, minimo 0; rumore `+3A` + 2). Messaggi: vittima giocatore locale **26** (col 50% un
  guasto, §9.4); altro giocatore **28** e **7**; IA **7** (50 se l'arma è di categoria 3, ramo morto con i dati).
- **< 0 con `+55` ≠ 0, stava già affondando:** niente; 7 (o 50) se la vittima non è il giocatore locale.
- **< 0 con `+55` = 0, distrutta:** contatore delle uccisioni `0B18` (`var[10 + lanciatore]` + 1 se il
  lanciatore ha id < 10: SCENE1C e SCENE1B lo usano per il duello); `+55` = 18, `+07` = 0, `+5E` = D3h. IA:
  messaggio 7. Giocatore: `+55` = 30 (1 se è già un relitto), niente 7 e niente `0C10`; locale **26** più **53**
  «emergency surface!!!» se non è un relitto; altro giocatore **28**.

**Il 7 arriva per qualunque colpo** su un'unità che non sia il giocatore locale, anche fra unità dell'IA. Il
gestore di 7 e 50 (`1000:8C7F`) mostra il testo e suona `EXP1.T`, salvo con `DS:03AA` ≠ 0 e schermo `DS:03A0`
= 20h (probabilmente il periscopio alzato; confidenza media).

**Durante il conto `+55`** (`02F3` visita un'unità per passo a rotazione, quindi 18 visite ≈ 2·N s con N unità):
niente navigazione, fondale e controlli, ma il moto (`083E`) continua con gli ultimi ordini; l'unità resta
rilevabile (il suo rumore dipende dalla velocità, che non cambia); le unità dell'IA non osservano più; i timer
continuano (confidenza media). A fine conto l'IA esce con `003C`; il giocatore in superficie resta relitto (§9.4).

**Cariche e missili sono opcode, non oggetti.**
- **`ATTACK` (54h, `1CC1`):** la word della portata è letta e **scartata**; il byte (0: `+3D`) è una soglia:
  riesce se `DS:5100` > soglia, cioè con probabilità (18 − b)/18 (mai con b ≥ 18: con 19 in tre usi, per esempio
  SCENE8A, è un difetto). **Danno fisso 30** sul bersaglio `+58`. Messaggi: **20** alla vittima giocatore locale
  (più `0C10` se sopravvive, 53 se distrutta e non relitto), **56** all'altro giocatore, nulla per l'IA. Se
  sopravvive **`+5E` non cambia** (`IF_HIT` non vede i colpi non letali); un'arma distrutta ha `+55` = 2.
- **`ATTACK_RND` (6Ah, `1C96`):** riesce se un byte casuale (`1ACA`) è ≥ `+3D` del modello corrente; la vittima è
  l'unità corrente `DS:475A`, cioè chi esegue lo script (il bersaglio del missile, a cui `SELECT_LINKED` ha
  passato il timer). Messaggio **50** sempre, `+5E` = C9h, poi il danno del byte.
- **Difetto:** `ATTACK` e `ATTACK_RND` chiamano `0B18` con un `DS:6503` vecchio: l'uccisione va al lanciatore
  dell'ultimo siluro andato a segno (o al giocatore 0).

**Fine corsa e filo** (script): la fine corsa è un timer dell'arma (Mk 48 `AT +140`, 280 s) verso uno script con
il messaggio 24 e l'**opcode 37h**, che **toglie dal gioco l'unità corrente `DS:475A`** (`19CB` → `003C`; nei
listati storici `SELECT_LAST`). `DS:475A` è chi esegue lo script (`0289`), salvo selezioni successive
(`SELECT_TGT`, `SELECT_LINKED`). **`SELECT_LINKED` (6Bh, `1DCD`)** seleziona il **bersaglio** `+58` (non il
collegato) e prima ne scrive il `+62` con l'unità corrente. `LAUNCH` (`18B7`) copia nel siluro il contatto del
lanciatore con qualità 1200 (traccia solida).

**Ritorno del colpo.** `0B2D` finisce sempre con ax = 1, e `07BC` a ax ≠ 0 chiude il passo (`0818`): dopo un colpo,
chiunque fosse l'arma, il cursore `+12` dell'osservatore e `DS:1F5C` non avanzano. Se l'osservatore ha nella sua
lista un contatto rimasto senza bersaglio (un'unità uscita, `003C` ne azzera il puntatore), `0C49` lo libera
(`127B`: azzera `+58`, `+60`/`+10` o `+14` se erano quel contatto, mette in coda lo script `+56`) e quella
valutazione salta scoperta e colpo. Il motore lo riproduce dal 24/10/2026 (§10.7).

**Nel motore** (`engine/missioni/src/missione.cpp`, 24/10/2026): nel livello classico (predefinito,
`Missione::impatti_dell_originale`) collisione solo nelle coppie dell'osservatore del passo, con le due direzioni del
punto 4 e la fine del passo dopo un colpo; nella regola di simulazione su ogni coppia arma/unità a ogni passo, con
le esclusioni dei punti 1, 3 e 4. Sulle 110 partite del rilievo le due regole danno 178 e 177 armi a segno, ma
cambiano 20 partite e 2 esiti (`engine/README.md`, «Danni nelle missioni»); danno con il contatore `DS:5100`; conto delle visite; opcode 37h `togli_unita`;
`SELECT_LINKED`, `ATTACK` e `ATTACK_RND` come sopra; i colpi vanno alla fisica dei danni (`engine/README.md`,
«Danni nelle missioni»).

**Non stabilito:** a cosa servano C9h e D3h di `+5E` fuori dalla VM; lo schermo `DS:03A0` = 20h; se l'interfaccia
mostra diversamente un'unità che affonda; la cadenza effettiva dei controlli in emulazione.

## 10. Armi, schiera e quadro del giocatore (numeri per le fasi C3–C5)

Fonti:
- stazione Armi: `1000:740E–76E7`, lancio `790C`, tubo successivo `7AF1`, guida `7B4C`;
- telegrafo e schiera: `6F60–7070`, `3D9D–3E4F`, posizione di velocità `72A2`;
- contatti per l'interfaccia: `990C`; carta `6132`; periscopio `4777`; barra del bersaglio `8213`; ANALYZE `5443`;
- inizializzazione: `1000:4022–4075`;
- script dei tubi e delle armi delle 22 missioni.

### 10.1 Tubi: dati, armamento in due tempi, ricarica

**Stato in DS**, per i tubi 0–6:

| Indirizzo | Contenuto |
|---|---|
| `02D1+i` | stato del tubo: 0 vuoto, 2 in armamento, 3 armato e pronto. Nel disegno lo stato 1 ha il colore 7, il 2 il colore 14, il 3 il colore 12 della palette |
| `02D8+i` | tipo d'arma del tubo (0 Mk 48, 1 Type 53, 2 Sea Lance, 3 Tomahawk, 4 Harpoon, 5 esca) |
| `02DF+t` | scorte rimaste per tipo d'arma t |
| `02E5` | tubo selezionato |

I tubi sono 7 − `DS:032F` (il lato): 7 sul 688 e 6 sull'Alfa. Stato iniziale, contatore delle armi in acqua,
tubo selezionato (`7AF1`) e messaggi 10–16 in dettaglio: `numeri_eseguibile.md` §1.

**Armamento in due tempi** (`7606–76A5`), con i tasti R (tubo 1), A (tubo 5, il primo ausiliario), M (tubo 7, i missili) o con il mouse:
1. **Il primo clic seleziona il tubo.** Compare un messaggio della tabella `DS:01F6` secondo il tipo d'arma:
   «torpedo tube selected» per Mk 48 e Type 53, «missile tube selected» per Sea Lance, Tomahawk e Harpoon,
   «auxilary tube selected» per le esche.
2. **Il secondo clic sullo stesso tubo cambia stato.**
   - Tubo vuoto: va a 2 «weapon being armed». Se ci sono scorte manda il comando 1Dh + tubo e toglie una
     scorta; se non ce ne sono resta vuoto e compare «we are out of that weapon type».
   - Tubo in armamento o armato: torna a 0 «tube cleared» e l'arma rientra nelle scorte.
3. **Il gestore `carica_tubo_n` della missione** aspetta **10 tick (20 s) in tutte le 22 scene e per ogni
   arma**: `AT +10`, che nei file Lua è `dopo(u, 20, …)`, già in secondi. Poi manda il messaggio 10–16 del tubo. Il programma lo trasforma nel messaggio del tipo d'arma, mette
   il tubo a 3 «weapon armed and ready to fire» e seleziona il primo tubo armato con la stessa arma.

**Ricarica.** Dopo un lancio il tubo **si ricarica da solo**: va a 2, toglie una scorta e manda 1Dh + tubo;
senza scorte resta vuoto. Ci vogliono altri 20 s. Intanto si può lanciare subito da un altro tubo armato.

### 10.2 Lancio (`790C`)

Controlli, in quest'ordine:
1. Il tubo deve essere armato (stato 3), altrimenti «weapon not armed captain».
2. **Solo per il tubo 7**, quello dei missili (vale anche per il Sea Lance, che nelle missioni 9 sta lì):
   - serve un bersaglio selezionato, altrimenti «you must have a target selected»;
   - l'**Harpoon** si lancia solo contro le navi (categoria 2): «only fire missles at surface vessels»;
   - serve un contatto con **traccia solida** (bit 01h, §10.5), altrimenti «need range information to launch».
     Senza contatto il lancio viene rifiutato in silenzio;
   - la **distanza** dev'essere almeno **5,0 nm** (`DS:0386` > 49 decimi), altrimenti «target too close to
     launch missile at». È l'ultima distanza mostrata nella barra del bersaglio, cioè la distanza **vera**;
   - serve la **quota periscopica**, quota ≤ 160 ottavi (20 ft), altrimenti «you must be at periscope depth
     sir!».
3. **Limite dei siluri in acqua.** Il contatore `DS:03DE` sale a ogni lancio dai tubi 1–6 e del Sea Lance (non
   dei missili del tubo 7). Il limite è `DS:0AFC`. Raggiunto il limite compare «can't launch another yet,
   captain».

   | Battello | Limite |
   |---|---|
   | 688 | 4 (2 su un PC lento, con indice di velocità `DS:0FE8` < 70) |
   | Alfa | 6 (3 su un PC lento) |

   Il contatore scende quando viene tolta un'unità con `+62` uguale all'id del giocatore. `+62` lo imposta
   `LINK` (`198A`) con l'id del lanciatore: siluri, esche e ogni oggetto collegato al giocatore. Anche le esche
   occupano quindi un posto, fino a quando scadono (160 s, 60 in alcune scene). **Non esiste un intervallo
   minimo fra i lanci:** i «circa 10 s» del manuale non sono nel codice.
4. **Lancio:**
   - suono `TORPRAMP.T`;
   - comando 38h con il bersaglio selezionato, che diventa il bersaglio `+58` del giocatore e quindi quello che
     `LAUNCH` copia nell'arma;
   - comando 24h + tipo, cioè il gestore di lancio della missione;
   - ricarica automatica (§10.1).

### 10.3 Guida a filo (`7B4C`, gestori `guida_e0..e2`)

- **Condizioni.** Il contatto selezionato dev'essere un'arma **guidata**: bit 40h del §10.5, cioè un'unità con
  `+0C` = il giocatore e tipo 16h (Mk 48) o 25h (Type 53). Altrimenti compare «no weapon selected sir!» o «not
  a guided weapon sir!». Spearfish, Mk 50 e Sea Lance non sono mai guidati.
- **Esecuzione.** Il programma manda 38h con l'arma, che diventa il bersaglio del giocatore, e poi il comando:

| Comando | Gestore | Effetto negli script |
|---|---|---|
| 3Ah DETONATE | `guida_e0` | seleziona l'arma, messaggio 19 «underwater explosion», toglie l'unità. Niente danni ad altri: gli effetti li fa il colpo, non la detonazione |
| 3Bh CERCA | `guida_e1` | messaggio 41 «search program started», marca i contatti, dopo 2 tick torna al comportamento di ricerca dell'arma |
| 3Ch DIRECT, poi 3Dh X, Y | `guida_e2` | «select waypoint captain» sulla carta; il punto finisce negli argomenti di `vai_a` del gestore; l'arma ci va a tutta forza |

- **Taglio del filo.** È **a tempo**, nel gestore di lancio dell'arma: dopo `filo_s` secondi viene eseguito
  `LINK 0` (`collega(u, 0)`), `+0C` torna a 0 e l'arma non è più guidata. Arriva il messaggio 23 «guidance
  cable cut captain».

| Arma | Velocità | Corsa | Filo | Filo in nm di gioco | Corsa in nm di gioco |
|---|---|---|---|---|---|
| Mk 48 | 50,3 kn (54,9 in una scena) | 280 s | 190 s | 9,2 | 13,5 |
| Type 53 | 45,7 kn (50,3 in una scena) | 240 s (280 in una) | 170 s (190 in una) | 7,5 | 10,5 |

  I «7,5 nm» del manuale valgono per il Type 53; il filo del Mk 48 dura di più.

### 10.4 Schiera trainata (`3D9D–3E4F`, `6F60–6FE0`)

**Stato** in `DS:03C6`: 1 dentro (all'inizio), 0 filata, 2 persa. In `DS:03C4` c'è la manovra in corso: 1 la sta
filando, 2 la sta recuperando.

**Filarla:**
- Si può solo se la posizione di velocità (`72A2`: velocità ordinata × 4 / avanti tutta) è 0–2, cioè **sotto i
  3/4 di avanti tutta**. Altrimenti compare «cannot deploy towed array captain».
- Durante la manovra compaiono «still deploying/retrieving towed array». Con la schiera persa: «we cut the towed
  array captain».
- I tempi sono quelli degli script: filata in 30 tick (60 s, `AT +30`), recuperata in 20 tick (40 s, `AT +20`).

**Limite di velocità** (`6F8F–6FE0`):
- Con la schiera filata, o mentre la si fila o la si recupera, i comandi di telegrafo **3/4, TUTTA, MAX e
  l'arresto rapido** chiedono una conferma.
- Se il comandante conferma, la schiera viene tagliata: stato 2, comando 36h, «towed array cut captain».
  Altrimenti il comando è annullato.
- La velocità massima con la schiera è quindi metà di avanti tutta, il «2/3» del telegrafo del manuale. Vale
  uguale per l'Alfa: il codice non distingue.

**Danni.** Il guasto «towed array» del §9.4 porta la schiera allo stato 2 e manda 36h.

### 10.5 Contatti per l'interfaccia (`990C`) e quadro del giocatore

`990C` scorre i contatti del giocatore (`+08`) e per ognuno scrive 12 byte: indicatori, x, y e quota stimate
(`+08`, `+0A`, `+0C` del contatto), rilevamento (`+04`), id e lettera (`+13`). I contatti con qualità < 800
nascosti dal lampeggio (bit 80h di `+12`, §3.4; `996F–997B`) non compaiono. Gli indicatori:

| Bit | Condizione | Uso |
|---|---|---|
| 01h e 02h | qualità ≥ 800 (traccia solida) | **posizione nota**: lettera sulla carta, distanza e missili |
| 02h | durata `+14` > 800 | colore «classificato» |
| 04h | durata ≥ 1400 | **classe nota** (nome nella barra), colori pieni |
| 08h | nave o aereo (flag & 6) | colore |
| 10h | arma (flag 20h) | colore |
| 20h | schieramento non sovietico (flag 10h assente) | — |
| 40h | siluro filoguidato proprio (`+0C` = il giocatore, tipo Mk 48 o Type 53) | guida a filo, giallo |

**Filtro** (`DS:13D6`, comando FILTRO del sonar):
- Acceso, `990C` passa solo le armi, cioè i siluri; gli aerei no. Vale per la carta e per la barra.
- La vista del periscopio lo spegne per sé.
- Il guasto «sonar computer» lo spegne (§9.4).

**Carta (plottaggio):**
- **Qualità < 800:** una **«X»** di colore 14 a 25 pixel dal battello sul rilevamento. Si conosce solo la
  direzione.
- **Qualità ≥ 800:** la **lettera** del contatto alla posizione stimata, che si aggiorna solo con traccia
  solida (§8). Il colore:
  - 13 (traccia solida ma durata ≤ 800);
  - con durata > 800: 1 (sottomarino o ignoto) o 2 (nave o aereo);
  - con durata ≥ 1400: 4 se è un'arma, poi + 8 (colori chiari);
  - 14 (giallo) se è il proprio siluro filoguidato.

**Periscopio.** Stessa regola, «X» o lettera, per i contatti entro ±66 pixel dal rilevamento della vista.

**Barra del bersaglio** (`8213`):
- durata ≤ 800: «UNKNOWN CONTACT»;
- durata > 800: «UNKNOWN SURFACE CONTACT» o «UNKNOWN SUBMERGED CONTACT», secondo la quota **vera**;
- durata ≥ 1400: il nome della classe (`DS:019A`).

Con traccia solida si aggiungono **distanza, rotta, velocità e quota calcolate dalla posizione vera** del
bersaglio, non da quella stimata. La distanza compare in decimi di nm entro 20 nm per asse.

**ANALYZE** (`5443`):
- servono un bersaglio selezionato e il contatto;
- gli aerei vengono rifiutati: «can't analzye air contacts sir»;
- la firma dipende da categoria e tipo (`docs/originale/suoni.md`).

**ESM.** Il livello ESM del periscopio non è un modello a parte: sono le intercettazioni del §3.2. Lì l'osservatore
è la nave o l'aereo che scopre l'antenna o il periscopio alzati del giocatore.

### 10.6 Profondità e motori

- **Niente quota di schiacciamento.** La quota massima `+5A` (1500 ft per il 688, 1800 per l'Alfa) limita solo
  la quota ordinata, in `DEPTH_TO` (`175E`) e in `MATCH_DEPTH` (`1E59`). Nessun codice dà danni per la
  profondità.
- **Motori guasti** (§9.4, flag `DS:CC8E`):
  - MAX e l'arresto rapido vengono rifiutati, con il messaggio della tabella a `DS:028C`;
  - al momento del guasto, se la posizione di velocità è MAX diventa TUTTA, e se è indietro diventa STOP.

  Il «75% operativo» del manuale è solo l'indicazione del quadro di stato: nel codice non c'è un limite al 75%.
- **Sopra i 100 ft**, solo per i giocatori: velocità massima metà di avanti tutta, un quarto con il periscopio
  alzato (§9.6). L'antenna non conta.

### 10.7 Contatti: ordine, lettere, orfani e scia (`2011:1043–10CF`, `0C33–0C51`, `127B`, `12EE`)

Riletto il 24/10/2026; confidenza alta.

- **Ordine.** Ogni unità ha la sua lista di contatti (`+08`), e un contatto nuovo va **in coda** (`10CB`): la
  lista è in ordine di creazione.
- **Pool.** I record (21 byte, `DS:2C52`) sono 300, ma il contatore `DS:2C54` si ferma a 299: con il pool pieno
  non nasce nessun contatto, e quella valutazione finisce lì (`1043–1053`).
- **Lettere** (`1056–1083`), solo per gli osservatori con id 0 e 1:
  - un contatto nuovo prende la lettera più bassa, da «A», che nessun altro record della lista usa;
  - le altre unità hanno sempre «A», che non serve a nulla.
- **Orfani:**
  - quando un'unità esce dal gioco, `003C` azzera il bersaglio dei contatti che la riguardano, ma i record
    restano nelle liste;
  - `0C33–0C51` cerca il contatto del bersaglio dall'inizio della lista; se prima trova un orfano, lo libera con
    `127B` (come un contatto perso: bersaglio `+58`, aggancio `+60`/`+10` o inseguimento `+14` azzerati, script
    `+56` in coda) e la valutazione salta scoperta e colpo;
  - un orfano in fondo alla lista resta quindi finché l'osservatore non valuta un bersaglio senza contatto, per
    esempio un'unità nuova;
  - l'interfaccia lo ignora (`990C`, `993E–9947`), ma la sua lettera resta occupata fino ad allora.
- **Scia del giocatore** (`12EE`, chiamata da `07E2–07F7`):
  - ogni 30 osservazioni del giocatore locale (`DS:47BA`) si registrano id, x e y in un anello di 150 posti
    (`DS:47BE`, indice `DS:47BC`), azzerato all'avvio (`1000:9639`);
  - con circa E osservatori, un punto ogni 30·E/9 s (con 5 osservatori, uno ogni 17 s);
  - la carta li disegna come pixel (`1000:4F61–5047`), colore 8 per il giocatore locale e 4 per l'altro, solo con
    `DS:03CA` o `DS:03CE` ≠ 0;
  - con `DS:03CE` ≠ 0 disegna solo la scia di `DS:0368` (probabilmente la partita a due).
- **Comandi dall'interfaccia.** `1000:981D(codice)` mette il comando in coda (`DS:6016`, evento di tipo 2, con
  80h per il giocatore 1) verso il dispatcher; è così che partono 34h–36h della schiera (§10.4).

**Nel motore** (`engine/missioni/src/missione.cpp`, 24/10/2026):
- `Contatto::ordine` e `Contatto::lettera`, il limite del pool e la pulizia degli orfani nella coppia dei
  sensori (`Missione::contatto_orfano`, poi `perdi_contatto`), come sopra;
- un orfano resta in `UnitaClassica::contatti` finché non viene liberato: la carta e la barra della C4 devono
  saltare i contatti la cui unità non è più in gioco (`Missione::trova` nullo), come `990C`;
- la scia è in `Missione::scia()` (`PuntoScia`: id e posizione, anello di 150 punti, contatore DS:47BA in
  `passo_sensori`, contatti solidi in `valuta`); le voci vuote non si contano.

