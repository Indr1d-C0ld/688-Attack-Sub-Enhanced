# F5. Replay e ripresa

*Autori: disassemblatore (traccia e replay), principale (ripresa). Revisione del 27/10/2026. Stato: rivisto (curatore: kit) per la ripresa, rivisto (curatore: kit)
anche per la traccia e il replay.*

## 1. A che serve

Il replay mostra la partita dall'alto, a missione finita, con la verità o col quadro che il giocatore aveva. La
**ripresa** («Riprendi da qui») rimette il giocatore al comando da un istante della partita: la stessa missione, lo
stesso seme, le stesse opzioni, gli stessi ordini fino a quell'istante, poi decide di nuovo lui. Serve a provare
un'altra scelta nel momento in cui le cose sono andate male. La missione giocata resta agli atti così com'era: la
ripresa non conta per la carriera.

Sotto c'è la **traccia**: mentre si gioca, la partita si registra in un file, con la verità di tutte le unità e il
quadro che il giocatore aveva. Dalla traccia vengono il replay, le statistiche (capitolo F4) e la heatmap; la traccia
legge soltanto, e la partita non cambia.

## 2. Il modello

### 2.1 La traccia e lo schermo del replay

Il codice è in `nucleo/traccia_replay.hpp`, con l'adattatore dalla missione `missioni/traccia_missione.hpp`
(`fotogramma_da`); lo schermo è `app/replay.hpp` (`SchermataReplay`).

**Che cosa si registra** (`RegistratoreTraccia`): un fotogramma ogni 5 s, ogni 2 s se ci sono aerei o armi in acqua, e
uno a ogni evento nuovo del registro (capitolo F1). Ogni fotogramma ha:
- **la verità**: tutte le unità, con posizione, quota, rotta, velocità, stato, alberi alzati (periscopio, antenna,
  schiera), lanciatore e filo;
- **il quadro del giocatore**: i contatti con la lettera, la qualità, il rilevamento, la posizione stimata e la classe
  creduta; le linee di rilevamento nuove della TMA; il bersaglio;
- **i dati dell'interfaccia** (`DatiApp`, che riempie l'app): modo del sonar, sensore, fascio, filtro, stazione,
  silenzio a bordo, bersaglio, soluzione proposta dal MATE;
- **i segnalibri**: gli eventi del registro nel fotogramma in cui compaiono (record `E`), col tipo, i testi,
  l'interesse, se erano noti al giocatore, l'unità e i campi che servono alle statistiche (bersaglio, distanza,
  classi, lato, classificazione).

**Il file `.m688tr`** è autodescritto:
- l'intestazione di testo ha i metadati (missione, origine, codice, comandante, teatro, giocatore, `ripresa_da`) e lo
  schema dei record, coi nomi dei campi, il tipo e la quantizzazione. Un lettore più nuovo legge per nome, ignora i
  campi e i record che non conosce e lascia ai predefiniti quelli che mancano;
- i corpi sono interi quantizzati in varint zigzag, in differenza per unità e per contatto;
- un fotogramma chiave ogni 60; ogni fotogramma porta la sua lunghezza, quindi uno rovinato si salta fino alla chiave
  dopo;
- due ore con una cinquantina di unità stanno in poco più di un megabyte e mezzo (1,63 MB con la versione 2 in una
  prova sintetica di 50 unità; 1800 s di sandbox su S4, 26 kB).

**La versione 2** (27/10/2026, per la sandbox: capitolo F8) aggiunge record nuovi. Le tracce della versione 1 si leggono
ancora, con i campi nuovi ai valori predefiniti, e un lettore della 1 salta i record che non conosce:
- **`V`**: la posizione stimata, l'ellisse intera della navigazione (semiassi e orientamento), l'allerta e le emissioni
  in onda di ogni unità. Si scrive solo quando cambiano (la stima di 10 m, l'ellisse di 5 m o 5°) e nei fotogrammi
  chiave;
- **`A`**: la TMA in corso dell'IA (bersaglio, gambe, soluzione con posizione, rotta e velocità, osservabile) e le
  armi a bordo (armi rimaste, tubi carichi), quando cambiano;
- **`E`**: i segnalibri prendono anche l'arma, la classe creduta e la posizione;
- **`I`** (`EventoIa`): i fatti dell'IA che il registro non ha. Li raccoglie `RaccoltaTraccia`:
  - i contatti nuovi, solidi e persi di tutte le unità;
  - con `Missione::registra_statistiche` (la sandbox) anche il primo istante possibile, i lanci con la stima del
    lanciatore, la soluzione della TMA, le evasioni con la minaccia, le emissioni di tutti, i punti nave dell'IA;
  - gli eventi dei siluri del modello;
- **`K`** (`ContattoUnita`): i contatti di ogni unità, con qualità, rilevamento, posizione stimata, sensore e classe
  creduta. Una riga ai cambi di stato, ai movimenti al più ogni 15 s, e una «perso» dopo 30 s d'assenza;
- **`P`** (`QuadroParte`): il quadro di ogni parte (`Missione::quadro_parte`), scritto ogni 5 s quando cambia.

`Traccia::quadro_unita(osservatore, t)` e `Traccia::quadro_parte(lato, t)` ricostruiscono il quadro di un'unità e di una
parte a un istante, per le letture «unità» e «parte». `aggiungi_fotogramma(traccia, fotogramma)` aggiunge alla traccia
in memoria quello che il lettore ricaverebbe dal file, per la vista dal vivo della sandbox.

**La lettura**: `carica_traccia`, poi `Traccia::istante(t)` interpola posizione, quota e velocità in linea retta e la
rotta sul cerchio; il quadro del giocatore va a gradini, com'era. `segnalibri(traccia, interesse_min, noti)` dà gli
eventi in ordine di tempo, per la timeline.

**Dove sta**: in carriera `replay/<comandante>/`, col nome del file nel fascicolo (`VoceMissione::replay`); fuori
carriera e per le riprese `replay/liberi/`, dove `pulisci_replay_liberi(20)` tiene le ultime 20.

**Lo schermo del replay** (`SchermataReplay`), sulla carta del teatro (il fondale VGA, come l'editor), senza
risimulare. Nel modo replay è nella variante C della rifinitura estetica (capitolo E9 §2.9; 28/10/2026, principale):
la consolle «SALA ANALISI - REPLAY» con la carta grande in uno schermo (circa 618×358 pixel), due riquadri sopra la
carta e la linea del tempo come fascia sottile (`grafica/quadro`). Il replay dal vivo della sandbox tiene la
disposizione di prima (capitolo F8). Che cosa c'è:
- due letture, V per alternarle:
  - **verità**: tutte le unità coi simboli NTDS e APP-6 per parte e dominio, le scie degli ultimi 5 minuti, le armi
    in acqua per tipo col filo verso chi le guida, le emissioni, gli alberi alzati. I simboli, con l'alone, vengono dal
    modulo comune `grafica/simboli_carta` (capitolo E9; volume 1, §5.6);
  - **quadro del giocatore**: il battello, i contatti con la lettera e la posizione stimata (o la sola linea di
    rilevamento), le linee della TMA e la soluzione proposta;
- I e O ingrandiscono; la rotella fa lo stesso, a passi di 1,5×, attorno al punto sotto il puntatore
  (`app/trascina_carta.hpp`), ma non toglie il seguire: mentre la carta segue il battello, al passo dopo torna su di lui.
  Trascinare sposta la carta, che smette di seguire il battello finché non si clicca sulla destra della testata («SEGUI
  IL BATTELLO»); in basso a destra la scala delle 10 miglia;
- in basso a sinistra sulla carta la **legenda** coi simboli veri di `simboli_carta`: «CIO' CHE SAPEVI» nel quadro,
  «TUTTE LE UNITA'» nella verità;
- in alto a destra la **lettura** (larga 256 pixel): gli ultimi 4 eventi e il **profilo verticale** degli ultimi
  10 minuti, cioè la quota del battello e del bersaglio col fondale e i termoclini della missione;
- sotto la carta la **linea del tempo** (`quadro::fascia_tempo`): ogni evento è una tacca, la ripresa una riga verde.
  Il colore (`colore_evento`) dipende dal solo tipo, ed è quello della riga del registro nella vista dal vivo, dove i grigi si
  scrivono col fosforo. Nella lettura i rossi sono in rosso chiaro (indice 12) e tutto il resto col fosforo.
  - **Rosso** (24), ciò che colpisce, fa danno o è un'arma che parte: `lancio_giocatore`, `lancio_nemico`, `lancio_ia`, `colpo`,
    `colpo_esercitazione`, `affondamento`, `artiglieria_salva`, `artiglieria_colpo`, `artiglieria_ia`, `collisione`,
    `collisione_ia`, `incaglio`, `danno_subito`, `scram`, `camera_di_lancio_persa`, `cavo_impigliato`, `avaria_ia`,
    `fuoco_amico`, `fuoco_amico_ia`, `provocazione`, `infrazione_roe`, `mina_posata`, `mina_esplosa`, `esito_tlam`.
  - **Giallo** (27), il pericolo scampato o in corso: `avvistato`, `fix_radiogoniometria`, `evasione_ia`, `ritirata_ia`,
    `emersione_emergenza`, `emersione_forzata`, `siluro_evitato`, `allarme_siluro`, `esca_sedotta`, `esca_ia`, `danno_domato`,
    `posti_di_combattimento`, `artiglieria_vicino`, `quasi_collisione`, `incsea`, `cavo_toccato`, `attacco_simulato`,
    `marcatore_avvistato`, `esposizione_lunga`, `esm_rischio_alto`, `ping_subito`, `ping_sonar_immersione`, `aereo_in_sorvolo`,
    `boa_vicina`, `ingresso_zona`, `acque_territoriali`, `attraversamento_mine`.
  - **Ciano** (5), le scoperte: `scoperta`, `classificazione`, `scoperta_ia`, `ingaggio_ia`, `mina_scoperta`.
  - **Bianco** (2): `inizio` e `fine`.
  - **Grigio** (27), il resto: radio, routine, misure. `intervento` è l'unico a tutta altezza, perché ha già il triangolo col
    colore della famiglia sopra la barra. `classificazione_ia` lo è di proposito: 74 eventi in S5, in ciano coprirebbero le
    scoperte vere.

  Conta il tipo, non il caso: `posti_di_combattimento` è giallo anche per «Condizione normale», che con interesse 20 è un
  punto, `acque_territoriali` anche in uscita, `esito_tlam` è rosso anche se il missile è perso.
  L'altezza dice l'interesse (`EventoMissione::interesse`, 0-100): da 70 tutta la fascia, da 50 due terzi, da 30 un terzo,
  sotto un punto sulla linea. Le più interessanti si disegnano sopra le altre, il cursore dell'ora sopra tutte. Nella
  lettura quadro gli eventi non noti non compaiono; nella vista dal vivo della sandbox ci sono tutti. P gioca e mette in pausa; + e − (anche del tastierino) e Pag Su e Pag Giù cambiano la velocità da
  1× a 64×; le frecce saltano di 30 s; Tab e Maiusc+Tab vanno all'evento dopo e a quello prima; un clic sulla fascia
  salta lì;
- il **piede** comune: ESC ESCI, V LETTURA, INVIO RIPRENDI DA QUI, anche al clic; il codice della partita in fondo
  alla riga d'aiuto;
- gli eventi dell'IA (lanci, scoperte dell'IA) sono sulla timeline, ma nel quadro del giocatore non sono noti.

Per la ripresa conta solo questo: lo schermo del replay, aperto con P dal debriefing, restituisce con Invio l'istante
mostrato (`SchermataReplay::riprendi_da`), ed esce.

### 2.2 Da dove si riprende

| Da | Come | Conferma |
|---|---|---|
| debriefing | R (o il pulsante «R RIPRENDI») sull'evento corrente; spento sulla pagina dell'esito | sì: «RIPRENDERE IL COMANDO DA T+HH:MM:SS?», Invio sì, Esc no |
| replay completo | P dal debriefing, poi Invio all'istante scelto | no: Invio è la scelta |
| riga di comando | `--missione X --ordini FILE --riprendi-da T` (con o senza finestra) | — |

Il replay riaperto dal fascicolo, nell'albo dei comandanti, non riprende: la missione non c'è più.

### 2.3 Che cosa si tiene

`app/main.cpp`, `ripresa_da` e il ciclo della partita. La partita si ricrea da capo con:
- la stessa **missione** e la stessa cartella;
- lo stesso **seme** e la stessa **regola**;
- le stesse **opzioni**: stazioni 1989 o moderne, carta d'epoca o fondale vero, meteo, ASW, sonarista, variazioni
  acustiche, stile della carta;
- la stessa **data e ora d'inizio** e lo stesso luogo del Sole;
- il **carico di partenza** della Sala siluri, che sta nello storico degli ordini e si applica prima del primo passo
  (il briefing e la Sala siluri non si ripetono);
- gli **ordini** dello storico con istante **minore** di T (quello dell'evento o del replay). Gli ordini da T in poi si
  scartano.

Dal file (`ripresa_da_testo`): le righe d'ordine con istante ≥ T si scartano; carico e commenti restano.

Non si tengono: il lucido della carta (linee, segni, eventi, datum), la proposta del MATE, la compressione (riparte da
1×). Il quaderno del sonarista si rilegge dal suo file, come a ogni partita.

### 2.4 La ricostruzione

`app/main.cpp`, `ricostruisci`. La partita si rigioca senza disegnarla:

```
finché tempo di missione < T:
    luce della vista all'ora del passo (per gli effetti)
    Partita::avanza(passo)                      passo = 1/9 s (capitolo A1)
    ogni 200 passi: schermata d'attesa
```

- **Gli ordini** arrivano dallo storico, agli stessi istanti: con lo stesso passo la partita è la stessa (capitolo A1).
- **La ripresa comincia** al primo passo che raggiunge T, cioè T arrotondato in su al passo. L'evento di T è già
  successo: si riprende subito dopo.
- **La cascata del sonar** (cascate, tracker, registratore per l'ACINT) è solo interfaccia ed è la parte più cara:
  resta ferma fino agli ultimi 10 minuti di missione prima di T (600 s), che le ridanno la sua storia recente.
- **L'audio** riparte dall'istante della ripresa: i colpi, le salve e i tuoni già passati non suonano.
- **La schermata d'attesa**: su fondo nero, al centro, «RICOSTRUZIONE DELLA SITUAZIONE» in giallo, «RIPRESA DA
  T+HH:MM:SS», l'ora di missione raggiunta, una barra di 400 pixel che si riempie in verde. Si ridisegna ogni 200 passi,
  cioè ogni 22,2 s di missione rigiocati.
- **Chiudere la finestra** durante la ricostruzione esce dal gioco.

Senza finestra (`--istantanea`) la ricostruzione è la stessa, senza schermata; poi la partita continua fino a
`--secondi`, e il terminale scrive `ripresa da T s (N ordini prima)`.

### 2.5 Dopo la ripresa

- Comanda il giocatore. I suoi ordini nuovi si aggiungono allo storico, dopo quelli rigiocati.
- A fine missione:
  - la traccia del replay si salva fra le ultime 20 libere, mai nella cartella del comandante; porta l'istante della
    ripresa;
  - le **statistiche** non si registrano, né quelle globali né quelle del comandante (`registra_statistiche` esce
    subito con una ripresa);
  - il **debriefing** si apre come sempre, con la scritta «RIPRESA DA T+HH:MM:SS - NON CONTA PER LA CARRIERA» sulla
    carta;
  - niente **esito**, cerimonie, riconoscimenti né albo d'onore: uscendo dal debriefing la partita finisce.
- **Si riprende anche da una ripresa**: R nel suo debriefing chiede di nuovo, con la riga «ANCHE LA NUOVA RIPRESA
  NON CONTA PER LA CARRIERA.». Lo storico è quello della partita ripresa: gli ordini rigiocati più i nuovi.

### 2.6 La carriera della partita d'origine

Prima della prima ripresa la missione va agli atti così com'è (`app/main.cpp`, ciclo del debriefing):
1. il giocatore chiede la ripresa nel debriefing;
2. se la carriera non è ancora chiusa, si chiude adesso con la missione giocata (`chiudi_partita`): punteggio,
   decorazioni, promozione o provvedimento, riconoscimenti, albo d'onore; il fascicolo si salva;
3. la traccia della partita d'origine si salva (nella cartella del comandante se la missione conta);
4. parte la ricostruzione.

Se il giocatore chiede la ripresa prima di aver visto le pagine dell'esito, quelle pagine non si mostrano: il loro
effetto è già nel fascicolo, e il fascicolo le ricorda.

**Perché la ripresa non conta.** La carriera misura una missione giocata una volta, nella nebbia. Chi riprende ha
già visto il debriefing, spesso con la verità: sa dove erano le unità e che cosa hanno fatto. E se la ripresa
contasse, si potrebbe rigiocare il tratto andato male finché non va bene. Per questo la missione d'origine resta agli
atti com'era, e la ripresa vale solo come esercizio (`docs/00_ROADMAP.md`, punto 16: «prima della ripresa la missione
va agli atti così com'è»).

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Ripresa per ricostruzione deterministica dallo storico | requisito 16 della roadmap; capitolo A1 | alta (verificata, §6) |
| Cascata ferma fino agli ultimi 10 minuti | scelta di gioco, misurata | **STIMA**: abbastanza storia per la cascata a schermo |
| Schermata ogni 200 passi | scelta di gioco | — |
| La ripresa fuori carriera e fuori statistiche | regola di progetto (`docs/00_ROADMAP.md` punto 16) | — |

La traccia e lo schermo del replay non hanno parametri da tarare: le cadenze di registrazione (5 s, 2 s con aerei e
armi in acqua, un fotogramma chiave ogni 60) sono scelte di progetto, misurate sul peso del file (nota condivisa
`replay-traccia-proposta.md`, approvata il 26/10/2026).

## 4. Classico ed Enhanced

- **Ripresa dal debriefing e dal replay**: solo nell'Enhanced, perché solo lì ci sono debriefing e traccia.
- **Da riga di comando** (`--ordini FILE --riprendi-da T`) funziona anche nel classico; il carico di partenza vale
  solo nell'Enhanced.
- Il classico del 1989 non aveva niente di simile.

- **La traccia** si registra solo con la regola di simulazione dell'Enhanced. Legge soltanto: il rilievo delle 110
  partite del classico resta identico.

## 5. Come si vede in gioco

- Volume 1, §10.4 «Riprendi da qui e il replay»; §10.2 per il debriefing.
- Il debriefing: il pulsante «R RIPRENDI» e la domanda al centro della carta; la scritta della ripresa in alto a
  sinistra.
- La schermata d'attesa della ricostruzione (§2.4).

- Lo schermo del replay, P dal debriefing o dal fascicolo nell'albo dei comandanti: carta, letture con V, timeline,
  profilo verticale (§2.1; volume 1, §10.4).

## 6. Taratura e prove

- **Verifica della ripresa** (25/10/2026, `docs/00_ROADMAP.md` punto 16): senza finestra, con
  `--ordini FILE --riprendi-da T`, la partita ripresa dopo l'ultimo ordine finisce con la stessa impronta e la stessa
  immagine della partita intera.
- **Costo** (stessa fonte, misurato col vecchio passo di 1/30 s; a 1/9 s i passi sono 3,3 volte meno): 1600 s di
  Enhanced (SCENE1A) si ricostruiscono in 169 s con la cascata ferma fino agli
  ultimi 10 minuti, contro 314 s con la cascata sempre accesa. Il 60% del tempo va allo spettro acustico (sonarista e
  cascata).
- Prove automatiche vicine:
  - `test/prove_replay.cpp`: «replay: stesso seme, ordini e carico ridanno gli eventi del registro fino all'istante
    scelto», cioè gli stessi eventi con gli stessi id, carico compreso;
  - `test/prove_replay_schermo.cpp`: Invio nello schermo del replay dà l'istante (300 s) per la ripresa; Esc no;
  - `test/prove_statistiche.cpp`: una ripresa non entra nelle statistiche;
  - `test/prove_ordini.cpp`: lo storico salvato si rilegge identico.
- La ricostruzione in `app/main.cpp` non ha una prova automatica sua.

- Prove della traccia, `test/prove_traccia_replay.cpp`:
  - «traccia: due ore con 50 unita' stanno in poco piu' di un mega; si rilegge e si interpola»;
  - «traccia: lo schema per nome regge campi rinominati, tipi nuovi e un fotogramma rovinato»;
  - «traccia: una missione vera, il fotogramma dalla missione, il file e le cartelle»;
  - le prove «traccia v2»:
    - andata e ritorno dei record nuovi;
    - una traccia v1 scritta a mano col vecchio schema;
    - la raccolta su una scena dell'Enhanced e su SCENE1A classica ed Enhanced dal primo passo;
    - la vista dal vivo contro il file riletto;
    - il record K e il quadro di un'unità;
    - il record P, anche vuoto.
- Prove dello schermo, `test/prove_replay_schermo.cpp`: il teatro e la carta; gioca e pausa, velocità, salti, eventi,
  clic sulla timeline; gli eventi dell'IA sulla timeline e non noti al giocatore.

## 7. Limiti noti e in arrivo

- **Rigiocare costa**: il tempo cresce con T (circa un decimo del tempo di missione, sulla macchina della misura).
  **In arrivo**: fotografie periodiche dello stato, da cui ripartire senza rigiocare tutto.
- **La storia del sonar** prima degli ultimi 10 minuti manca: cascate, tracker e il registratore dell'ACINT la
  perdono, e con loro i rapporti dei tracker nel registro dei messaggi e le linee dei tracker sulla carta.
- **Il quadro del giocatore nella traccia** della partita ripresa viene dall'interfaccia ricostruita: per la parte
  rigiocata è più povero dell'originale.
- **Lucido e MATE** non si ricostruiscono: la carta riparte pulita.
- **Le pagine dell'esito** della partita d'origine non si vedono se si riprende prima di sfogliarle (§2.6).
- **Il codice partita** mostrato nel debriefing della ripresa è quello della partita d'origine: la ricrea da capo, non
  dall'istante della ripresa.
- **Stessa versione**: la ripresa è esatta con lo stesso eseguibile (capitolo A1).

- **La traccia campiona**: fra due fotogrammi il replay interpola, quindi una virata breve o un'arma che passa in
  meno di 2 s si vedono smussate.
- **I contatti di ogni unità (record K)** sono righe ai cambi, non uno stato a ogni fotogramma. Una posizione stimata
  che si sposta di meno di 100 m in 15 s resta quella dell'ultima riga.
- **Il replay non risimula**: mostra quello che è stato registrato, senza l'acustica né la cascata di quel momento.
  Per riascoltare c'è solo la ripresa.

## 8. Riferimenti

- Codice: `app/main.cpp` (`Ripresa`, `ripresa_da`, `ripresa_da_testo`, `ricostruisci`, schermata d'attesa, ciclo del
  debriefing), `app/debriefing.cpp` (R, conferma, `imposta_ripresa`), `app/replay.cpp` (`riprendi_da`),
  `app/interfaccia.cpp` (`ricostruzione`), `app/partita.cpp` (`ImpostazioniPartita::ripresa_da`),
  `nucleo/ordini.hpp` (`RegistroOrdini`), `nucleo/src/statistiche.cpp` (`registra_statistiche`).
- Progetto: `docs/00_ROADMAP.md` punto 16, `docs/12_schermate_di_missione.md` (M5).
- Capitoli: A1 (passo fisso e storico), F1 (debriefing ed esito), F2 (carriera).

- Codice della traccia e del replay: `nucleo/traccia_replay.hpp`, `nucleo/src/traccia_replay.cpp`,
  `missioni/traccia_missione.hpp`, `app/replay.hpp`, `app/replay.cpp`.
- Progetto: nota condivisa `replay-traccia-proposta.md`.
