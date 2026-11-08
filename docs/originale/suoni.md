# Suoni in partita dell'originale

Analisi di `GAME.EXE` (`re/unpacked/GAME.EXE`, segmento 1000h salvo dove indicato). Le tabelle in fondo le genera
`tools/re/suoni.py`, che scrive anche i WAV in `extracted/suoni/`; il resto è scritto a mano. Confidenza: **alta** se
letta nel codice, **media** se dedotta dal contesto, **bassa** se stimata.

## 1. Come suona

**Campioni `.T`** (`407E(file, p, ripetizioni)`, riproduzione in `D461`). Confidenza alta.
- Il file viene caricato (`CF14`) e suonato `ripetizioni` volte, con un ritardo per nibble d = s × 3200 / p
  (`A0E6`). s è la velocità della CPU misurata all'avvio (`A008`), quindi **p è la frequenza in Hz**, uguale su ogni
  macchina.
- Formato: PCM a 4 bit senza segno, due campioni per byte, prima il nibble alto. Ogni nibble passa dalla tabella
  `DS:BC52` e va all'altoparlante (porta 61h, `DS:0335` = 0), al Tandy (porta C0h, `DS:13D8` = 1) o al Covox.
- Si suonano 16 × (P − 2) byte, con P in `DS:1027` dopo il caricamento. Quasi certamente è tutto il file,
  arrotondato ai 16 byte (confidenza bassa sull'arrotondamento).
- **È bloccante:**
  - durante il suono il tempo di gioco è fermo (`DS:1F3E` = 0, salvo in partita via modem, `DS:0333`), e il
    programma non fa altro;
  - due suoni nello stesso istante escono quindi uno dopo l'altro, e nessun suono si sovrappone a un altro;
  - il manuale (p. 34) avverte che con la compressione del tempo i suoni rallentano l'aggiornamento delle carte.
- Dopo il suono, se a schermo c'è una vista (`DS:03AA` ≠ 0), la vista viene ridisegnata (`3AC0`).

**Bip dell'altoparlante** (`BD3F(divisore, giri)`). Confidenza alta.
- Usa il canale 2 del PIT. La frequenza è f = 1 193 182 / divisore.
- La durata è un ciclo d'attesa di `giri` iterazioni (4 NOP più LOOP), quindi **dipende dalla CPU**:
  - circa 7,3 µs per giro su un XT a 4,77 MHz;
  - circa 1,8 µs su un AT a 12 MHz.
- Sono clic e bip brevi, da 3 a 30 ms su un XT.

**Interruttore `DS:03E2`** (tasti in `DBFB`, tabella a `DE35`). Confidenza alta: lo confermano anche il manuale e
l'addendum.

| Valore | Tasto | Effetto |
|---|---|---|
| 0 | — | campioni e bip |
| 1 | **Ctrl-N** (0Eh, `DECC`), alterna 0 e 1 | niente campioni; i bip restano. Il manuale lo chiama «Sound On/Off» |
| 2 | **Ctrl-Q** (11h, `DEAB`), alterna 0 e 2 | niente campioni e niente bip («turn off all sound effects», addendum) |

`407E` esce con `DS:03E2` ≠ 0, `BD3F` solo con 2. Ogni pressione dei due tasti fa un bip (3000, 3000).

## 2. Quando suonano i campioni

Sono sette chiamate a `407E`, ciascuna con una sola ripetizione salvo ANALYZE. Durate a ripetizione: EXP1 2,28 s a
5000 Hz e 2,85 s a 4000; sonar1 2,19 s e 1,94 s; TORPRAMP 1,17 s.

| Campione | Hz | Chiamata | Quando | Confidenza |
|---|---|---|---|---|
| EXP1 | 5000 | `8CAE` | **Messaggi 7** «a torpedo has reached its target» e **50** «a missile has reached its target». Arrivano per ogni colpo su un'unità che non sia il giocatore locale, anche fra unità dell'IA, a qualunque distanza. Prima il testo, poi il suono. **Saltato** quando a schermo c'è la vista del periscopio (`DS:03AA` ≠ 0 e `DS:03A0` = 20h, riga sotto): il testo compare lo stesso | alta; periscopio media-alta |
| EXP1 | 5000 | `2AA8` | **Al periscopio** (stazione `1FBB`), quando l'animazione del colpo di un'unità nel campo arriva al **quarto fotogramma** (dettagli sotto) | media |
| EXP1 | 4000 | `8C54` | **Messaggi 19** «underwater explosion» (DETONATE del filo), **20** «damaged by a depth charge», **26** «we've been hit and damaged», **30** «we scraped bottom, hull damaged». Per 20, 26 e 30 prima si tira un guasto (§9.4 di `sensori_e_navigazione.md`). Prima il testo, poi il suono | alta |
| sonar1 | 5500 | `3D4E` | **Ping proprio** accettato (C9h, `3D17`): prima il testo «sending active sonar ping captain», poi il suono. Rifiutato col sonar di tiro guasto: niente suono | alta |
| sonar1 | 6200 | `8DFE` | **Messaggio 8** «active sonar», da `PING` (`2011:1843`). Lo manda qualunque unità che non sia il giocatore locale: navi, sottomarini, SOSUS, l'altro giocatore e anche i siluri del giocatore a ricerca attiva (Spearfish e Mk 50, ogni 10–16 s). Vale **a qualunque distanza** e senza condizioni. Prima il suono, poi il testo. **Ripetuto:** se l'ultimo messaggio passato dal gestore (`DS:03C8`) era già un 8, niente campione e niente testo, solo un bip grave (5000 = 239 Hz, 1500 giri, `8DE5`) | alta |
| TORPRAMP | 6500 | `7A73` | **Lancio del giocatore** accettato da un tubo (`790C`): siluri, missili del tubo 7, Sea Lance ed esche del tubo ausiliario. Suona **una volta sola, al lancio**: niente durante la corsa, niente legato alla distanza, niente per i lanci dell'IA | alta; esche media |
| prop2, propv2, whale2 | per classe | `4049` | **ANALYZE** (`3EE0`, tabella sotto) | alta |

**Esplosione vista dal periscopio** (`2AA8`).
- Il byte `+5E` di un'unità colpita vale 1 al primo colpo, C9h ai successivi e D3h quando è distrutta.
- A ogni ridisegno della vista, per ogni unità disegnata nel campo, `97BD` (chiamata da `3154`) incrementa `+5E`.
  Fanno eccezione:
  - 0, che resta 0;
  - FFh, che resta FFh;
  - C8h, che diventa A0h;
  - CFh, che diventa A1h.
- Il disegno (`322A`) usa il valore di prima dell'incremento. Ricava il fotogramma dell'esplosione così: 1–4 → 0–3,
  C9h–CCh → 0–3, D3h–D6h → 0–3.
- Al fotogramma 3 mette `DS:03D8` = 1. Dopo il ridisegno (`2A95`) la stazione suona EXP1 a 5000 Hz e azzera il flag.
- Quindi:
  - il suono arriva al **quarto ridisegno** della vista dopo il colpo, uno per colpo;
  - se l'unità non è nel campo del periscopio, non arriva;
  - se più unità arrivano al fotogramma 3 nello stesso ridisegno, il suono è uno solo.
- La cadenza dei ridisegni è quella del ciclo della stazione, che dipende dalla CPU: confidenza bassa sul tempo
  reale.

**`DS:03AA` e `DS:03A0`.** Confidenza media-alta.
- `DS:03AA` ≠ 0 vuol dire che una vista occupa la finestra principale:
  - il periscopio alzato, `DS:0194`, che il messaggio 3 azzera;
  - oppure il «sonar imager» (tasto C).
- `DS:03A0` è il tipo di vista, scritto da `13C1`:
  - la stazione Periscopio (`1FBB`, quella dell'ESM `244C`) chiama `13C1(20h)`;
  - gli altri chiamanti passano 10h, e con 10h il disegno dei contatti cambia (`2B94`).
- 20h è quindi **la vista del periscopio**. Il salto di `8C8D` evita il doppio scoppio: al periscopio il suono
  arriva con l'animazione.

**ANALYZE** (`3EE0`).
- **Rifiuti, senza suono:**
  - senza bersaglio, «you must have a target selected»;
  - senza contatto (`990C`);
  - con un aereo, «can't analzye air contacts sir».
- **Altrimenti:** prima il testo, poi un campione scelto dal tipo t (`+4F` del contatto) e dagli indicatori del
  contatto che scrive `990C` (§10.5 di `sensori_e_navigazione.md`). Confidenza alta.
- **Gli indicatori non sono i bit di `+06`.** `990C` li ricompone (`999B–99B1`):
  - 08h vuol dire nave o aereo (`+06 & 6`);
  - 10h vuol dire arma (`+06 & 20h`);
  - 20h vuol dire non sovietico.

  Il suono dipende quindi dalla **categoria**, non dallo schieramento: una nave sovietica suona come una
  americana. Le versioni precedenti di questo file e di `tools/re/suoni.py` leggevano 08h e 10h come schieramento.

| Condizione (in quest'ordine) | Campione | Frequenza | Ripetizioni | Durata a ripetizione |
|---|---|---|---|---|
| 08h: nave (`+06 & 6`; gli aerei sono già rifiutati) | prop2.t | 2500 + 50 t Hz | 4 | 0,96–1,32 s |
| 10h: arma (`+06 & 20h`: siluri) | propv2.t | 5500 + 10 t Hz | 5 | 0,66–0,69 s |
| t = 19h (biologico) | whale2.t | 2500 + (caso mod 4000) Hz, cioè 2500–6499 Hz | 2 | 1,36–3,54 s |
| altrimenti: sottomarini, esche, marcatori | propv2.t | 2000 + 30 t Hz | 4 | 1,20–1,52 s |

- Dopo il suono il grafico dell'analizzatore (`92AF`, `410B`) si anima per 42 scatti di `DS:1F52`. Anche questo è
  bloccante.

## 3. Bip in partita

| Chiamata | Quando | Divisore (Hz) | Giri (durata su XT) | Confidenza |
|---|---|---|---|---|
| `0ED8` | **ogni testo scritto nella barra dei messaggi**: messaggi del gestore e risposte delle stazioni, rifiuti compresi | 3000 (398 Hz) | 2000 (≈ 15 ms) | alta |
| `8DE5` | messaggio 8 ripetuto (sopra), al posto del campione e del testo | 5000 (239 Hz) | 1500 (≈ 11 ms) | alta |
| `47B0` | «sonar imager», `4500` «COMPUTING IMAGE DATA»: uno per ognuna delle 5 passate (SPACE interrompe dopo la seconda) | 2000 (597 Hz) | 1200 (≈ 9 ms) | media |
| `6135` | finestra del testo a macchina (`606E`): uno per carattere | 7000 (170 Hz) | 400 (≈ 3 ms) | media |
| `E0F5` | conferma di un'immissione (Invio o pulsante, in `DBFB`) | 5000 (239 Hz) | 4000 (≈ 29 ms) | media |
| `DEDE`, `DEBF` | tasti Ctrl-D (04h, `DS:BDD2`), Ctrl-N e Ctrl-Q | 3000 (398 Hz) | 3000 (≈ 22 ms) | alta |
| `DEA4` | tasto Ctrl-R (12h, `DS:BD89`, solo con `DS:032F` ≠ 0) | 3000 (398 Hz) | 1500 (≈ 11 ms) | media |

## 4. Cosa non suona

Confidenza alta: sono tutte le chiamate a `407E` e a `BD3F` dell'eseguibile.
- **Siluri:** niente per il siluro in corsa (TORPRAMP è solo il lancio del giocatore) né per i lanci dell'IA.
- **Altri messaggi:** niente per gli scoppi delle cariche su unità dell'IA (ATTACK dà messaggi solo ai giocatori)
  né per i messaggi 36 «we are being pinged hard», 48 «detected a missile launch», 9 «smashed into the bottom» e i
  messaggi di fine missione. Questi hanno solo il bip del testo.
- **Ambiente:** niente macchine, flusso, mare o biologici fuori da ANALYZE.
- **File:** i sei `.T` del disco 2 sono tutti quelli che il gioco carica. Fra le stringhe dell'eseguibile non ce ne
  sono altri.

## 5. Per il motore

- **Il remake non blocca:** il tempo non si ferma e il gioco va avanti mentre il suono esce. La sequenza resta
  quella dell'originale se i suoni vanno su un solo canale, in coda (`Mixer::aggiungi` accoda).
- **La cadenza del periscopio** e la durata dei bip vanno fissate:
  - i bip alla durata su XT;
  - i ridisegni del periscopio a un passo della missione.
  Sono stime.

<!-- tabelle: tools/re/suoni.py -->
## Tabelle generate

Da `tools/re/suoni.py`; i WAV sono in `extracted/suoni/`.

### Effetti

| File | Frequenza | Uso | WAV |
|---|---|---|---|
| EXP1.T | 5000 Hz | esplosione | `exp1_5000hz.wav` |
| EXP1.T | 4000 Hz | esplosione, variante grave | `exp1_4000hz.wav` |
| SONAR1.T | 5500 Hz | impulso sonar | `sonar1_5500hz.wav` |
| SONAR1.T | 6200 Hz | impulso sonar, variante acuta | `sonar1_6200hz.wav` |
| TORPRAMP.T | 6500 Hz | siluro in corsa | `torpramp_6500hz.wav` |

### Firme acustiche di ANALYZE (sala sonar)

Lo stesso campione ricampionato per classe: la frequenza dipende dal tipo d'unità e dallo schieramento del
contatto (qui quella del tipo). Il WAV contiene le ripetizioni che il gioco suona.

| Tipo | Classe | Categoria | Suono | Frequenza | Ripetizioni | WAV |
|---|---|---|---|---|---|---|
| 0x00 | Forrest Sherman Class Destroyer | nave | prop2.t | 2500 Hz | 4 | `analyze_00_prop2.wav` |
| 0x01 | Merchant Ship | nave | prop2.t | 2550 Hz | 4 | `analyze_01_prop2.wav` |
| 0x02 | Cargo Ship | nave | prop2.t | 2600 Hz | 4 | `analyze_02_prop2.wav` |
| 0x05 | Kara Class Guided Missile Cruiser | nave | prop2.t | 2750 Hz | 4 | `analyze_05_prop2.wav` |
| 0x06 | Kirov | nave | prop2.t | 2800 Hz | 4 | `analyze_06_prop2.wav` |
| 0x07 | Krivak Ii | nave | prop2.t | 2850 Hz | 4 | `analyze_07_prop2.wav` |
| 0x08 | Koni Class Frigate | nave | prop2.t | 2900 Hz | 4 | `analyze_08_prop2.wav` |
| 0x09 | Slava Class Cruiser | nave | prop2.t | 2950 Hz | 4 | `analyze_09_prop2.wav` |
| 0x0a | Nimitz Class Aircraft Carrier | nave | prop2.t | 3000 Hz | 4 | `analyze_0a_prop2.wav` |
| 0x0b | Iowa Class Battleship | nave | prop2.t | 3050 Hz | 4 | `analyze_0b_prop2.wav` |
| 0x0c | Kidd Class Guided Missile Destroyer | nave | prop2.t | 3100 Hz | 4 | `analyze_0c_prop2.wav` |
| 0x0d | Oliver Hazard Perry Class Frigate | nave | prop2.t | 3150 Hz | 4 | `analyze_0d_prop2.wav` |
| 0x0e | Sacramento Class Oiler | nave | prop2.t | 3200 Hz | 4 | `analyze_0e_prop2.wav` |
| 0x0f | Cruise Missile Target | nave | prop2.t | 3250 Hz | 4 | `analyze_0f_prop2.wav` |
| 0x10 | Tanker | nave | prop2.t | 3300 Hz | 4 | `analyze_10_prop2.wav` |
| 0x11 | Kamov Ka-25 Hormone Helicoptor | aereo o missile: rifiutato | — | — | — | — |
| 0x12 | Seahawk-60B Lamps Iii Helicoptor | aereo o missile: rifiutato | — | — | — | — |
| 0x13 | Mss | nave | prop2.t | 3450 Hz | 4 | `analyze_13_prop2.wav` |
| 0x14 | Us 688 Class Attack Submarine | sottomarino o esca | propv2.t | 2600 Hz | 4 | `analyze_14_propv2.wav` |
| 0x15 | Soviet Alfa Class Attack Submarine | sottomarino o esca | propv2.t | 2630 Hz | 4 | `analyze_15_propv2.wav` |
| 0x16 | Mark 48 Torpedo | arma | propv2.t | 5720 Hz | 5 | `analyze_16_propv2.wav` |
| 0x17 | Victor Iii Class Fast Attack Sub | sottomarino o esca | propv2.t | 2690 Hz | 4 | `analyze_17_propv2.wav` |
| 0x18 | P3C Orion Aircraft | aereo o missile: rifiutato | — | — | — | — |
| 0x19 | Biological | biologico | whale2.t | 2500–6499 Hz (caso) | 2 | `analyze_19_whale2.wav` |
| 0x1a | Us 688 Class Attack Submarine | sottomarino o esca | propv2.t | 2780 Hz | 4 | `analyze_1a_propv2.wav` |
| 0x1b | Sturgeon Class Ssn676 | sottomarino o esca | propv2.t | 2810 Hz | 4 | `analyze_1b_propv2.wav` |
| 0x1c | Oscar Class (Ssgn) Cruise Missile Sub | sottomarino o esca | propv2.t | 2840 Hz | 4 | `analyze_1c_propv2.wav` |
| 0x1d | Akula Class Fast Attack | sottomarino o esca | propv2.t | 2870 Hz | 4 | `analyze_1d_propv2.wav` |
| 0x1e | Charlie Ii Class Guided Missile Sub | sottomarino o esca | propv2.t | 2900 Hz | 4 | `analyze_1e_propv2.wav` |
| 0x1f | Typhoon Class Ssbn | sottomarino o esca | propv2.t | 2930 Hz | 4 | `analyze_1f_propv2.wav` |
| 0x20 | Kilo Class Diesel Sub | sottomarino o esca | propv2.t | 2960 Hz | 4 | `analyze_20_propv2.wav` |
| 0x21 | Foxtrot Class Diesel Sub | sottomarino o esca | propv2.t | 2990 Hz | 4 | `analyze_21_propv2.wav` |
| 0x22 | Trafalgar Class Fast Attack | sottomarino o esca | propv2.t | 3020 Hz | 4 | `analyze_22_propv2.wav` |
| 0x23 | Tomahawk Missile | aereo o missile: rifiutato | — | — | — | — |
| 0x24 | Sealance Torpedo | arma | propv2.t | 5860 Hz | 5 | `analyze_24_propv2.wav` |
| 0x25 | 533 Mm Torpedo | arma | propv2.t | 5870 Hz | 5 | `analyze_25_propv2.wav` |
| 0x26 | 450 Mm Torpedo | arma | propv2.t | 5880 Hz | 5 | `analyze_26_propv2.wav` |
| 0x27 | Spearfish Torpedo | arma | propv2.t | 5890 Hz | 5 | `analyze_27_propv2.wav` |
| 0x28 | Ss-N-19 Slcm | aereo o missile: rifiutato | — | — | — | — |
| 0x29 | Harpoon Missile | aereo o missile: rifiutato | — | — | — | — |
| 0x2a | Noisemaker | sottomarino o esca | propv2.t | 3260 Hz | 4 | `analyze_2a_propv2.wav` |
| 0x2b | Sonar Target Marker | sottomarino o esca | propv2.t | 3290 Hz | 4 | `analyze_2b_propv2.wav` |
| 0x2c | Mark 50 Torpedo | arma | propv2.t | 5940 Hz | 5 | `analyze_2c_propv2.wav` |
| 0x2d | 650 Mm Torpedo | arma | propv2.t | 5950 Hz | 5 | `analyze_2d_propv2.wav` |
<!-- fine tabelle -->
