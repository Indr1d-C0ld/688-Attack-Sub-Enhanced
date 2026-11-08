# 3. L'edizione Enhanced

*Dalla presentazione del progetto (`docs/presentazione/progetto_en.md`) e dalla roadmap (`docs/00_ROADMAP.md`).*

## 3.1 Che cos'è

Un remake non commerciale di *688 Attack Sub*, scritto da zero come programma nuovo, in due strati:

- **il classico**: le 22 missioni originali con le regole originali, ricavate dall'eseguibile del DOS e dagli script
  delle missioni, non imitate a occhio. I risultati si confrontano con partite di riferimento: gli stessi ordini e lo
  stesso seme devono dare la stessa partita;
- **l'Enhanced**: le stesse missioni e gli stessi battelli, più quello che un computer di casa del 1989 non poteva fare.
  Il suono si propaga secondo la fisica, il sonar e la guerra elettronica funzionano come sui battelli veri, il meteo e
  il mare vengono dai dati climatici; poi i danni, la carriera coi gradi e le medaglie vere, l'editor e una libreria di
  missioni nuove dagli episodi della Guerra fredda.

È un progetto amatoriale, non affiliato a Electronic Arts. Gli asset del gioco originale (grafica, font, testi, file delle
missioni) non si ridistribuiscono: il gioco li legge dalla copia del giocatore.

## 3.2 I requisiti

L'autore del progetto li ha fissati all'inizio (roadmap, «Requisiti del progetto»), e da allora guidano ogni scelta:

1. tutto in **italiano**: testi, voci di bordo, interfacce, manuali;
2. **solo giocatore singolo**: il codice del modem non si porta;
3. **meccaniche riscritte** in modo realistico e profondo, sulle fonti ufficiali, tecniche, accademiche e storiche dei
   due blocchi;
4. **sonar, radar, oceanografia** veri, e un'**IA** matura, ispirata alla dottrina;
5. l'**estrazione** di tutti gli asset originali;
6. la **manualistica**: la traduzione del manuale originale e questo manuale;
7. il **meteo dinamico**;
8. il **giorno e la notte** con l'astronomia vera, i **periscopi** rifatti, tutte le **stazioni** realistiche;
9. l'**armamento** coerente col teatro e con l'anno;
10. un **editor** di missione libero, le missioni rapide e la libreria;
11. i **danni e le riparazioni** realistici;
12. l'**Alfa** senza cirillico illeggibile: caratteri latini con un font «sovietico» a stampino;

e così via fino al trentunesimo, la simmetria fra il giocatore e l'IA (volume 1, capitolo 1). Due principi li attraversano
tutti: **il classico resta identico**, e **l'estetica è quella VGA a 256 colori** dell'epoca, col suono AdLib e Sound
Blaster.

## 3.3 Come si lavora

Il progetto è sviluppato con Claude Code (Anthropic), come una piccola squadra di sessioni d'intelligenza artificiale in
parallelo, coordinate da un progettista umano, l'autore del progetto: decide la direzione, approva ogni proposta di
ricerca prima che si scriva il codice, giudica i risultati.

- **La sessione principale** coordina: tiene la roadmap e l'elenco dei punti aperti, assegna il lavoro in modo che due
  sessioni non tocchino gli stessi file, approva le interfacce fra i moduli e collega i pezzi alle schermate.
- **Quattro sessioni specialistiche** lavorano insieme:
  - reverse engineering, acustica e sonar: il disassemblato dell'originale, la macchina virtuale delle missioni, il
    modello del suono, le stazioni sonar, il carico, la carriera, i riconoscimenti, il replay, le statistiche;
  - il runtime delle missioni: la simulazione, la sala radio, l'ASW aereo, le collisioni, l'artiglieria, le mine, la
    dottrina dell'IA;
  - il kit grafico: i modelli 3D dalle foto, i danni e l'affondamento delle navi, gli effetti, le consolle dei due
    battelli, l'enciclopedia e questo manuale;
  - missioni e media: il generatore delle missioni rapide, l'editor, la libreria, i trailer.
- **Prima i contratti.** Quando una funzione attraversa più sessioni (l'artiglieria: dati, tiro, danni, grafica, audio,
  schermate), la sua interfaccia si scrive e si approva prima del codice, e ogni pezzo si consegna su quella.
- **Prima la ricerca.** Ogni funzione importante parte da un dossier scritto, con le fonti e la confidenza di ogni
  numero: ventiquattro finora. I valori che non hanno una fonte sono segnati **STIMA** nel codice e nei dati, per
  trovarli e sostituirli dopo.
- **La regressione come regola.** Ogni modifica si controlla con le prove automatiche, con il rilievo di 110 partite del
  classico (che deve restare identico) e con le impronte della simulazione e delle schermate.

## 3.4 Il reverse engineering

- Gli eseguibili del DOS sono stati decompressi (EXEPACK) e analizzati con **Ghidra 12**, attraverso un server MCP che dà a
  un agente l'accesso funzione per funzione, con **Capstone** per il disassemblato a 16 bit e script **Python** per i
  formati dei dati.
- La macchina virtuale delle missioni è stata ricostruita, e le 22 missioni convertite in **Lua** leggibile, che il
  motore nuovo esegue con la semantica originale.
- Ogni asset è stato estratto e convertito: sprite, palette, font, schermate, la musica OPL2, i campioni a 4 bit. La base
  dati delle navi, le portate dei sensori, le tabelle dei danni e i messaggi sono mappati, con gli indirizzi.
- Il manuale originale (circa 21.000 parole) e tutti i testi del gioco sono tradotti in italiano.
- Il confronto con l'originale che gira in **DOSBox Staging** tiene onesto il classico.

## 3.5 Le fonti

- **Intelligence declassificata**: le stime della CIA sulle forze navali e antisommergibile sovietiche, i rapporti tecnici
  del DTIC, le pubblicazioni NATO.
- **Riferimenti navali**: Polmar e Moore, Friedman, i *Jane's* dell'epoca, il Naval History and Heritage Command, la
  Federation of American Scientists, GlobalSecurity, NavWeaps, e le fonti in russo (deepstorm.ru, militaryrussia.ru,
  Apalkov, Pavlov).
- **Acustica**: Urick, *Principles of Underwater Sound*; Ross, *Mechanics of Underwater Noise*; Waite, *Sonar for
  Practising Engineers*; Coté, *The Third Battle*.
- **Oceano e meteo**: EMODnet Bathymetry e NOAA ETOPO 2022; World Ocean Atlas 2023; Copernicus ERA5 (1981-1990).
- **Immagini e suoni**: foto della US Navy e del Dipartimento della Difesa in pubblico dominio, file di Wikimedia Commons
  sotto CC0, CC BY o CC BY-SA, campioni audio sotto CC0, pubblico dominio o CC BY: ognuno coi suoi crediti.

Ogni numero dei file di dati porta la sua fonte e la sua confidenza. I crediti delle foto stanno nel Museo del gioco;
quelli completi nel file `LICENZE/CREDITI.txt` del pacchetto.

## 3.6 Il software

- **Motore**: C++20, SDL3, una tela indicizzata 640×480 a 256 colori, CMake e Ninja, doctest per le prove, Lua 5.4 per le
  missioni, toml++ per i dati, ymfm (Aaron Giles) per emulare il chip FM della musica originale.
- **Pipeline dei dati**: Python 3.13 con NumPy, SciPy, Pillow, xarray, netCDF4 e gsw (le equazioni TEOS-10 dell'acqua di
  mare). Trasforma i dati della ricerca, la batimetria, il clima e i modelli 3D nei formati del motore.
- **Reverse engineering**: Ghidra con il JDK, il server MCP per Ghidra, Capstone, DOSBox Staging.
- **Media**: FFmpeg per i trailer; FluidSynth con il soundfont MuseScore General per la musica.

## 3.7 Le tappe del lavoro

Una cronologia breve dei blocchi di lavoro, dalle date dei documenti di progetto (`docs/`). Ogni blocco ha il suo
piano prima del codice.

| Data (2026) | Blocco | Documento |
|---|---|---|
| 22-23/10 | il reverse engineering dell'originale, le decisioni tecniche, il motore nuovo | `00_ROADMAP.md`, `01`-`04` |
| 24-25/10 | i danni e gli affondamenti, il meteo e il mare, l'artiglieria, la libreria di missioni | `10`, `15`, `16`, `17` |
| 26/10 | le interazioni fra le unità dell'IA e la simmetria col giocatore | `18` |
| 27/10 | la sandbox, il vetrino dell'IA | `19` |
| 28/10 | la stazione di tiro dei missili, il quadro del fondale e l'ecoscandaglio | `21`, `24` |
| 29/10 | il libretto di riconoscimento e il telemetro; achievement, statistiche e carriera; le luci di navigazione | `25`, `26`, `27` |
| 30-31/10 | la banchisa e la vita sotto il ghiaccio; i fari della costa | volume 2, E11 e C9 |
| 31/10 | il vetrino di Dio (lo spettatore interviene); il video d'apertura | `28`, `29` |
| 31/10-03/11 | i danni critici: depositi con la loro carica, carburante, sottomarini col carico vivo, navi che saltano in aria | volume 2, E2 e B3 |

## 3.8 Nota legale

*688 Attack Sub* è un marchio dei rispettivi proprietari. Questo è un progetto amatoriale e non commerciale, non affiliato
a Electronic Arts. Nessun asset originale è distribuito: il gioco li legge dalla copia del giocatore. I dati e i media di
terzi si usano secondo le loro licenze, elencate nei crediti.
