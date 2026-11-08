# 25 — Il libretto di riconoscimento e il telemetro (piano)

Richiesta dell'utente del 29/10/2026: un libretto da consultare al periscopio, con le sagome delle unità conosciute
viste da più angoli, i dati tecnici noti e l'equipaggiamento in dotazione, per l'identificazione visiva; insieme alle
meccaniche del telemetro e a tutto quello che è collegato. Solo nell'Enhanced: il classico resta quello dell'originale.

## 1. Perché è realistico

- I sottomarini americani avevano i manuali di riconoscimento dell'ONI e i fogli del CNO, con le sagome delle unità
  sovietiche di profilo, di prua e di tre quarti, l'armamento visibile, i radar e l'**altezza della testa d'albero**.
  Gli equipaggi sovietici avevano gli album delle navi NATO (GRU e Stato maggiore della Marina).
- Al periscopio il riconoscimento serve a tre cose:
  - **chi è**, per le regole d'ingaggio: una nave da guerra, un mercantile, un neutrale;
  - **quanto è lontano**: il telemetro misura l'angolo sotteso dall'albero, e la distanza si ricava dall'altezza letta
    nel libretto. Una classe sbagliata dà una distanza sbagliata;
  - **dove va**: l'angolo sulla prora (AOB) si stima confrontando la sagoma con le viste del libretto, e dà la rotta
    del bersaglio alla TMA.
- I dati del libretto sono **quelli noti nel 1989**, con le incertezze dell'intelligence di allora. Non sono la
  verità del gioco: un'altezza d'albero può essere sbagliata di qualche metro, un'unità rimodernata può avere un radar
  diverso dalla tavola.

## 2. Cosa c'è già

- **Telemetro a immagine sdoppiata** sul 688 (Type 2 e Type 18; docs/14 §P4, `InterfacciaMissione::misura_stadimetro`):
  - H apre l'immagine fantasma, tarata a 3000 m;
  - le parentesi quadre la spostano finché la testa d'albero tocca il galleggiamento;
  - H legge la distanza.

  L'altezza sul quadrante oggi la sceglie il gioco: quella della classe vera se il contatto è «noto» da 1400 s,
  altrimenti 30 m. Sull'Alfa c'è il reticolo.
- **Altezze d'albero per classe**, dai modelli 3D (`main.cpp`, `imposta_alberi`).
- **Il museo** (`app/museo`, `nucleo/enciclopedia`): schede tecniche, testi, immagini dei modelli e foto.
- **Gli emettitori radar per classe** (intercettazione e ESM, docs/22): Top Sail, Head Net e gli altri.

## 3. Cosa si fa

### 3.1 Il libretto (pagina alla stazione Periscopio)

- **Dove si apre:** con un tasto nuovo («libretto»), alla stazione Periscopio, accanto alla vista. Il libretto occupa
  il terzo destro, mentre la vista resta a sinistra con la sua bussola. Si sfoglia a periscopio abbassato o alzato.
- **Le tavole:** una per classe, divise in tre sezioni: navi da guerra (NATO, Patto, altri), navi civili (per tipo),
  sottomarini (la vela, gli alberi). Ogni tavola ha:
  - le sagome di **profilo (90°)**, di **tre quarti (45° e 135°)** e di **prua (0°)**, rese dai modelli 3D nella
    grafica d'epoca: inchiostro su carta, come le tavole ONI;
  - i **dati noti nel 1989**:
    - dislocamento e lunghezza;
    - **altezza della testa d'albero** (quella dell'intelligence, con l'incertezza);
    - armi e sensori visibili;
    - radar ed emettitori (per l'ESM);
    - i **segni distintivi** in una riga («due fumaioli a poppa del ponte, lanciatore binato a prua»);
  - per i sottomarini solo la vela e gli alberi: è quello che si vede.
- **Il filtro:** per parte e per tipo, e una ricerca per «cosa vedo» (fumaioli, lanciatori, ponte di volo) che
  restringe le tavole.

### 3.2 L'identificazione visiva

- Dal libretto, **IDENTIFICA**: il giocatore attribuisce la classe al contatto designato. È un ordine al motore, che
  il registro e il replay vedono.
- **Il contatto prende la classe dichiarata dal giocatore,** che può essere sbagliata: la classificazione visiva è
  diversa da quella acustica del sonarista e può contraddirla. Il debriefing confronta le due con la verità.
- **Le conseguenze:**
  - le regole d'ingaggio leggono la classe dichiarata. Colpire un neutrale identificato male resta un errore del
    comandante, e il registro lo dice;
  - il **telemetro** usa l'altezza della tavola scelta.

### 3.3 Il telemetro

- **L'altezza sul quadrante** la imposta il giocatore: dal libretto con IDENTIFICA, oppure a mano col tasto (passi
  di 1 m), entro i limiti del periscopio (`periscopi.toml`).
- **Il gioco non sceglie più al posto suo:** il vecchio «noto da 1400 s» resta solo come aiuto, «il
  timoniere legge la tavola», nella difficoltà più facile.
- **L'errore della distanza** viene da due fonti:
  - l'allineamento del giocatore (oggi);
  - l'altezza sbagliata, cioè una classe sbagliata o l'incertezza della tavola.
- **Sull'Alfa** il reticolo: si sceglie l'altezza e si contano le divisioni (docs/14 §P4).

### 3.4 L'angolo sulla prora (AOB)

- Il giocatore stima l'AOB confrontando la sagoma con le viste del libretto, e lo imposta a passi di 10° dritta o
  sinistra. Con rilevamento e distanza fa un'**osservazione completa** al periscopio.
- **L'osservazione va alla TMA:** la rotta del bersaglio = rilevamento + 180° − AOB, con la sua incertezza. Nel MATE
  la proposta si vede e si confronta.

### 3.5 L'IA, per la simmetria

L'identificazione visiva dell'IA (avvistamento, classe) segue le stesse regole:
- è più incerta con la distanza, la foschia, la notte e l'aspetto di prua;
- un'unità dell'IA può scambiare una fregata per un mercantile;
- vale anche fra unità dell'IA.

### 3.6 Il manuale

Il kit scrive il capitolo del libretto e del telemetro, con le tavole d'esempio e i tasti nell'appendice.

## 4. Chi fa cosa (stima)

| Chi | Lavoro | Costo |
|---|---|---|
| Kit | le tavole: sagome dai modelli 3D a 0°, 45°, 90° e 135° in stile ONI; la pagina del libretto in `grafica`; i dati per classe in `data/motore/libretto.toml` (altezza d'albero dell'intelligence con l'incertezza, segni distintivi), per le 40-60 classi del gioco | M-L |
| Runtime | l'ordine `identifica_visiva` (contatto, classe), la classe dichiarata nel contatto, nel registro e nelle ROE; l'osservazione AOB alla TMA; l'identificazione visiva dell'IA con l'errore (simmetria) | M |
| Disassembler | la TMA con l'osservazione della rotta (AOB) e il suo peso; il confronto fra classificazione acustica e visiva nel debriefing | S-M |
| Principale | la pagina del libretto alla stazione Periscopio, i tasti, IDENTIFICA, il quadrante del telemetro, l'AOB e il suo invio | M |
| Trailer-1 | una missione d'addestramento al riconoscimento per la libreria, se approvata | S-M |

**Ordine:** prima i dati e le tavole (kit), poi la pagina e IDENTIFICA col telemetro (principale e runtime), poi
l'AOB e la TMA, infine l'IA.

## 5. Decisioni dell'utente (29/10/2026)

- Il libretto si apre **accanto alla vista**, nel terzo destro.
- L'aiuto automatico («il timoniere legge la tavola») resta **solo nella difficoltà più facile**.
- **L'AOB va subito alla TMA**, insieme al resto.

Le domande erano:

1. **Dove si apre il libretto:** accanto alla vista, nel terzo destro (consigliato: si confronta la sagoma con quello
   che si vede), oppure a tutta pagina.
2. **L'aiuto automatico** («il timoniere legge la tavola»): solo nella difficoltà più facile (consigliato), oppure
   mai.
3. **L'AOB e la TMA:** subito, insieme al resto (consigliato), oppure dopo, in una seconda fase.

## 6. Stato (29/10/2026)

Fatto:
- **Kit:** le 37 tavole (`data/motore/libretto.toml`), le sagome e la pagina (`grafica/libretto`),
  `strumenti/libretto_tavole` per le prove a vista.
- **Runtime:** gli ordini `identifica_visiva` e `osserva_aob`, la classe dichiarata nel registro e nelle ROE, la
  difficoltà ASW per l'aiuto del telemetro, l'identificazione visiva dell'IA (`missioni/src/missione_libretto.cpp`).
- **Principale** (`app/interfaccia.cpp`), solo nell'Enhanced coi periscopi veri:
  - **Ctrl+Z** apre e chiude il libretto nel terzo destro della vista (da ogni stazione porta al Periscopio; Esc lo
    chiude). **Tab** e **Maiusc+Tab** sfogliano le tavole;
  - **Invio** = IDENTIFICA sul bersaglio designato: la classe va al motore, l'altezza d'albero della tavola al
    quadrante del telemetro;
  - **[ ]** cambiano l'altezza del quadrante a mano, 1 m (Maiusc 5 m), entro i limiti del periscopio; poi la tavola
    non vale più;
  - per le otto classi con l'albero oltre il quadrante (39,62 m: Sacramento, Nimitz, Iowa, Kirov, portacontainer,
    Spruance, Kidd, Slava) IDENTIFICA mette sul quadrante il secondo riferimento della tavola (tetto della plancia o
    fumaiolo), e la riga e la barra dicono a cosa mirare («PLANCIA 25 M»);
  - senza identificazione il quadrante sta a 30 m; a difficoltà ASW facile «il timoniere legge la tavola» della classe
    vera se il contatto è noto da 1400 s;
  - **Z X** l'AOB a passi di 10° (sinistra, dritta); **Spazio** lo manda alla TMA a periscopio alzato, ±10° ad alto
    ingrandimento e ±15° a basso;
  - sotto la pagina la riga «AOB 30 DR  ALBERO 29 M  IDENT.» («?» dopo i metri: presunta o a mano; IDENT.: la tavola
    aperta è la classe dichiarata); col libretto aperto le righe dell'ESM si accorciano a sinistra della pagina;
  - nel MATE l'ultima osservazione al periscopio sul bersaglio con la rotta che ne esce e lo scarto dalla soluzione;
    nella barra del bersaglio «A VISTA <classe>».

- **Filtro delle tavole** (grafica/libretto del kit): F apre e chiude la scheda; le frecce scelgono la riga (parte,
  tipo, etichette «cosa vedo») e il valore; Invio o Spazio cambiano il valore, Backspace torna a TUTTE, Esc chiude.
  Chiusa la scheda, Tab sfoglia solo le tavole filtrate e la testata dice il filtro («PATTO / FREGATA / ... 1/2»).
- **Missioni d'addestramento:** approvate LIB-I13 «Sagome nello Stretto» e LIB-I14 «Levante» (nebbia), con armi da
  esercitazione (trailer-1; servono l'obiettivo «identificare» e la misura del telemetro come ordine, del runtime).

Fatto il 31/10/2026 (kit): le etichette «cosa vedo» che distinguono i sottomarini. Alla posizione, all'altezza e ai
segni della vela si aggiungono corta o lunga e squadrata o spiovente a poppa, misurate sui modelli; la pipeline
controlla che nessun sottomarino abbia le stesse etichette di un altro (manuale C9 §2.16).

Fatto il 31/10/2026 (kit): il telemetro a reticolo dell'Alfa col libretto. H mette il cursore sulla scala verticale,
[ ] lo spostano a decimi di divisione, H legge la distanza dall'altezza del quadrante (manuale C9 §2.12).
