# 688 Attack Sub, edizione Enhanced: il manuale

Il manuale dell'edizione Enhanced è un compendio: chi gioca deve trovarci qualunque informazione sulla simulazione,
da come si alza il periscopio alla formula che decide se un contatto si sente. È diviso in quattro volumi.

| Volume | Cartella | Per chi | Contenuto |
|---|---|---|---|
| **1. Manuale d'uso** | `1_uso/` | chi gioca | le stazioni, i comandi e le procedure, capitolo per capitolo come si usano in mare |
| **2. Riferimento tecnico** | `2_riferimento/` | chi vuole capire | ogni modello della simulazione: formule, parametri, fonti, confidenza, limiti, differenze col classico |
| **3. Storia e unità** | `3_storia/` | chi vuole sapere | il gioco del 1989, la Guerra fredda sotto il mare, i teatri, gli episodi, ogni unità e arma con la sua storia |
| **4. Tattiche operative** | `4_tattiche/` | chi vuole vincere | come si combatte coi modelli dell'Enhanced: le regole pratiche, il perché, i numeri misurati col motore e una scena per provarle (indice in `4_tattiche/00_indice.md`) |

Accanto c'è la traduzione italiana del manuale originale (`docs/traduzione/manuale`), che resta il riferimento per il
classico.

## Regole comuni

- **Lingua e termini:** italiano, col glossario della traduzione (`docs/traduzione/glossario.md`). Un termine nuovo
  si aggiunge al glossario prima di usarlo.
- **Classico ed Enhanced:** ogni funzione dice dove vale. Il classico è il gioco del 1989 e resta identico.
- **In arrivo:** funzione decisa e in lavorazione, non ancora nel gioco. Si verifica nel codice o con le istantanee
  che quello che si descrive esista davvero.
- **STIMA:** valore scelto per il gioco dove le fonti tacciono; si dice sempre.
- **Tabelle di dati:** non si copiano a mano. Le genera `tools/pipeline/manuale.py` dai file di `data/motore`, fra i
  segni `<!-- generato: NOME -->` e `<!-- fine: NOME -->`, così non si separano mai dal gioco (volume 2, schema per
  gli autori).
- **Tasti:** ogni tasto nuovo o cambiato va nell'appendice A del volume 1. I tasti dei comandi vengono da
  `data/motore/tasti_predefiniti.toml`; quelli delle schermate e delle stazioni si spiegano in
  `tools/pipeline/tasti_schermate.toml`, e `manuale.py --controlla` segnala ogni tasto del codice che lì manca.
- **Figure:** in `figure/`, condivise fra i volumi. Schermate prese senza finestra (`--istantanea`, `--schermata`),
  tavole del kit grafico, foto storiche solo a licenza libera, con i crediti di `docs/CREDITI.md`.

## Curatela

Cura il manuale la sessione del kit grafico, che scrive anche i capitoli delle sue aree e fa le figure. I capitoli
delle altre aree li scrivono le sessioni che le conoscono, sullo schema comune di `2_riferimento/SCHEMA_AUTORI.md`.
