# Schema per gli autori del volume 2

Ogni capitolo del riferimento tecnico segue lo stesso schema, così il lettore sa dove trovare cosa. Il curatore (la
sessione del kit) rivede i testi, uniforma i termini e fa le figure.

## File

- `2_riferimento/<sigla>_<nome>.md`, per esempio `C1_equazione_del_sonar.md`. La sigla è quella dell'indice.
- In testa: titolo, autore (la sessione), data dell'ultima revisione, stato (bozza, rivisto).

## Le otto sezioni

### 1. A che serve

Un paragrafo per chi gioca: che cosa decide questo modello in partita, e perché conta. Senza formule.

### 2. Il modello

Il cuore del capitolo. Per ogni grandezza:
- che cos'è, con l'unità di misura;
- la formula, scritta in testo semplice o in un blocco di codice (`TL = 20 log r + α r`), con ogni simbolo spiegato
  subito sotto;
- i parametri con i loro valori, e dove stanno: il file di dati (`data/motore/acustica.toml [firme.victor3]`) o la
  costante nel codice (`nucleo/src/danni.cpp`, `p_fuoco`);
- l'ordine dei passi quando conta (che cosa si calcola prima, a che frequenza).

Un esempio numerico per le formule principali: «un Victor III a 5 nodi e 10 km, in Mar di Norvegia a ottobre, sotto
lo strato…».

### 3. Fonti e confidenza

Per ogni parametro importante: la fonte (dossier e paragrafo, `dossier 02 §5.3 [Urick]`) e la confidenza (alta,
media, bassa, **STIMA**). Le STIME dicono perché si è scelto quel valore.

### 4. Classico ed Enhanced

Che cosa vale nel classico (com'era nel 1989, coi numeri ricavati dall'eseguibile) e che cosa solo nell'Enhanced.
Se il modello non esiste nel classico, si dice in una riga.

### 5. Come si vede in gioco

Dove il giocatore incontra il modello: stazioni, pannelli, messaggi di bordo (col numero, `messaggio 447`), effetti,
suoni, debriefing. Rimanda al capitolo del volume 1 dove si spiega l'uso.

### 6. Taratura e prove

I casi su cui il modello è tarato (casi storici, prove fra unità) coi risultati, e le prove automatiche che lo
tengono fermo (`test/prove_danni.cpp`, «danni: proietti d'artiglieria sopra il galleggiamento»).

### 7. Limiti noti e in arrivo

Quello che il modello semplifica o non fa, e quello che è deciso ma non ancora fatto (**in arrivo**).

### 8. Riferimenti

File del codice e dei dati, dossier di ricerca, documenti di progetto (`docs/10`, `docs/16`…).

## Tabelle di dati

Le tabelle che vengono da `data/motore` non si scrivono a mano. Si mette il segnaposto e lo riempie
`tools/pipeline/manuale.py`:

```
<!-- generato: firme_acustiche -->
<!-- fine: firme_acustiche -->
```

Il nome dice quale tabella. Le tabelle di un'area le scrive chi ne scrive i capitoli, in un modulo suo:
`tools/pipeline/manuale_<area>.py` con un dizionario `TABELLE_<AREA>` (nome → funzione che riceve la cartella dei dati
come `pathlib.Path` e restituisce il markdown della tabella). `manuale.py` li importa da solo (oggi `manuale_acustica` e
`manuale_missioni`; per un'area nuova lo si dice al curatore, che aggiunge una riga). Un segnaposto di una tabella che
non esiste ancora resta com'è, con un avviso. Da `manuale.py` si possono riusare `numero` (virgola decimale),
`glossario` (termini e accenti dei testi presi dai dati) e `RADICE`, importandoli dentro le funzioni. Le tabelle piccole di un solo capitolo (tre o quattro righe di esempio) si possono scrivere a
mano, con la fonte.

## Stile

- Italiano, col glossario della traduzione (`docs/traduzione/glossario.md`): quota, rilevamento, antenna rimorchiata,
  timoni orizzontali, esca acustica, boa sonora. Un termine nuovo si propone al curatore prima di usarlo.
- Frasi brevi, una cosa per frase; elenchi per le sequenze; niente enfasi.
- Numeri con la virgola decimale (3,5 kg), unità SI accanto a piedi e nodi dove il gioco li usa.
- Nomi di classi, sistemi e sigle come nel gioco (Los Angeles, Alfa, BQQ-5, Akkord); il cirillico traslitterato come
  nel Museo, con l'originale fra parentesi al primo uso (ПОКС).
- Niente testo copiato dalle fonti: si parafrasa e si cita.
- Le formule si verificano sul codice, non sul documento di progetto: se divergono vale il codice, e la differenza si
  segnala.

## Figure

Si chiedono al curatore, dicendo che cosa mostrare. Il curatore le fa senza finestra (`--istantanea`,
`--schermata`, gli strumenti di `engine/strumenti`) o con le tavole del kit, e le mette in `figure/`. Grafici di
funzioni (perdite di propagazione, curve di Wenz, tempi d'allagamento) si generano da script, coi dati del gioco.
