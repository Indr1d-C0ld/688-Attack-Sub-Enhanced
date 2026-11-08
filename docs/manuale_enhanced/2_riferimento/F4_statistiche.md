# F4. Statistiche

*Autore: disassemblatore. Revisione del 27/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Racconta come si è giocato, con i numeri: quanto tempo a quota periscopica, quanto si è cavitato, quando e come il
nemico ci ha trovati, quante classificazioni giuste, dove si è passato il tempo sulla carta. Per una missione, e
sommate su tutte le missioni di un comandante o del giocatore, con i primati. Tutto si ricalcola dalla traccia del
replay (capitolo F5), quindi anche una missione vecchia ha il suo report.

## 2. Il modello

Il codice è in `nucleo/statistiche.hpp`; le schermate in `app/statistiche.hpp`.

### 2.1 Il report di una missione

`report_missione(traccia, catalogo)` legge la verità campionata e gli eventi del registro dalla traccia. Fra due
campioni distanti più di 30 s non si integra.

| Gruppo | Che cosa |
|---|---|
| tempi | in superficie (sotto 2 m), a quota periscopica (2-7,1 m), sopra la velocità di cavitazione; durata e distanza percorsa |
| quota e velocità | media e massima |
| esposizioni | secondi col periscopio, con l'antenna, con la schiera fuori; trasmissioni radio; ping |
| armi | lanci e colpi per classe d'arma; per ogni unità colpita la classe, i colpi, se è affondata |
| scoperta nemica | la prima volta che il nemico ci ha trovati (avvistati, un fix radiogoniometrico dopo una trasmissione, un ping subito): quando, chi, a che distanza; e quante volte in tutto |
| i nostri contatti | quanti, quanti classificati, giusti ed errati, il tempo medio dalla scoperta alla classificazione. Quelle del sonarista dell'equipaggio (messaggio 411) a parte |
| livello irradiato | la banda larga del battello (capitolo C4) ogni minuto, dalla firma della classe sulla velocità e sulla quota campionate; la media in energia e il massimo |
| heatmap | i secondi passati in ogni cella di 1 km |
| periscopio (29/10/2026) | identificazioni visive giuste ed errate (`visive_giuste`, `visive_errate`); misure del telemetro e l'errore medio in percentuale; osservazioni dell'AOB e l'errore medio in gradi (eventi `misura_telemetro`, `osservazione_aob`) |
| missili | lanciati, a segno, neutrali nella scatola (i `lancio_giocatore` col livello della scatola) |
| fondo e navigazione | ping dell'ecoscandaglio; punti nave riusciti, l'errore medio e massimo della posizione stimata (eventi `ping_ecoscandaglio`, `punto_nave`) |
| intercettatore | allarmi di siluro, e i siluri evitati entro 10 minuti da un allarme |
| mine | scoperte, campi attraversati col sonar da mine |
| radio | gruppi d'autenticazione verificati, non autentici riconosciuti |
| urti | collisioni, quasi collisioni, colpi d'artiglieria subiti |
| ghiaccio (31/10/2026) | secondi sotto la banchisa, urti col ghiaccio, emersioni nella banchisa, avvisi delle chiglie |
| notte | identificazioni giuste coi soli fanali e con la sagoma, giuste ed errate a luci spente, secondi con le luci proprie accese |
| pedinamento | il tratto in coda più lungo a un sottomarino, la distanza minima da un sottomarino straniero, le volte che un sottomarino ci ha classificati |
| mine (31/10/2026) | ormeggiate e di fondo, per sensore (BQS-15, MG-70, MG-24), aggirate |
| ASW aereo | sorvoli di elicotteri e di aerei, siluri aerei evitati |
| soccorso | gli uomini salvati (`m688.salvati` della missione) |

I colpi d'artiglieria subiti si contano per sparo dal 05/11/2026: una raffica vale uno (capitolo E4). I totali salvati
prima, del giocatore e di ogni comandante, restano com'erano e contano ogni proietto; le partite nuove sommano con la regola
nuova. Una traccia vecchia, riletta, conta ancora ogni evento.

Le voci del 31/10/2026 che non sono eventi del registro (il tempo sotto la banchisa, le luci, il pedinamento, il
soccorso) arrivano nella traccia come metadati `conti.<nome>`, scritti a fine missione dai contatori della missione
(`Missione::ContiGiocatore`, del runtime). Una traccia più vecchia non li ha, e quelle voci restano a zero.

Le classificazioni giuste ed errate seguono le regole dell'esito (capitolo F1): conta la classe data dal giocatore, o
la categoria.

### 2.2 Le cumulate

`StatisticheCumulate`, in `statistiche.toml` accanto ai riconoscimenti:
- **per comandante**: le missioni che contano per la carriera (`Carriera::conta`, capitolo F2);
- **globali**: tutte le missioni non riprese;
- una missione ripresa da un istante (capitolo F5) non conta per nessuno;
- una partita sandbox (il «vetrino», capitolo F8) non conta per nessuno: è fuori carriera, e il vetrino non registra
  le cumulate. Ha le sue statistiche tecniche per parte e per unità (`nucleo/statistiche_sandbox`, capitolo F8), con
  l'aggregazione delle serie.

`accumula(cumulate, report)` somma tempi, distanze, esposizioni, armi per classe, affondati per classe, le scoperte
nemiche per modo (quante e a che distanza media), le classificazioni, e le voci nuove della tabella di §2.1: gli errori
medi del telemetro, dell'AOB e della navigazione pesati sul numero delle misure; per il pedinamento il primato (il
tratto in coda più lungo, la distanza minima da un sottomarino), non la somma. Tiene i **primati**: quota e velocità massime, la
missione più lunga, la prima scoperta nemica più lontana e più vicina. La heatmap si tiene per teatro, a celle di 2 km.
`registra_statistiche` legge, aggiunge e salva il file.

## 3. Fonti e confidenza

Le statistiche contano quello che è successo: non hanno parametri da tarare. Le soglie di quota (superficie sotto 2 m,
quota periscopica fino a 7,1 m) sono quelle del gioco: la quota è nella scala della missione, dove periscopio e antenna
escono fino a 6,096 m (20 piedi), più il metro di tolleranza delle altre prove; il livello irradiato è quello della firma (capitolo C4, con le
sue confidenze).

## 4. Classico ed Enhanced

Solo dove c'è la traccia del replay, cioè con la regola di simulazione. Il classico non ha statistiche.

## 5. Come si vede in gioco

Volume 1, §10.6:
- **della missione** (S nel debriefing): a sinistra la quota nel tempo con le fasce di superficie e di quota
  periscopica e i termoclini, il livello irradiato coi segni dei ping e delle trasmissioni, le armi; a destra la
  heatmap sulla carta del teatro, la prima scoperta nemica e i tempi;
- **cumulate** (dall'albo dei comandanti): totali, medie per missione, rilevamenti, primati, armi; a destra la heatmap
  di un teatro alla volta (Tab, o un clic sulla carta).

Le schermate non hanno percentuali: il font del gioco non ha il segno.

## 6. Taratura e prove

Prove:
- `test/prove_statistiche.cpp`: «il report di una missione dalla traccia», «le cumulate, il comandante e i globali,
  le riprese, il file», «una missione vera, dalla traccia che si registra mentre si gioca», «una raffica vale un colpo
  d'artiglieria; una traccia di prima del 05/11/2026, senza gli spari, conta ogni evento»;
- `test/prove_statistiche_schermo.cpp`: le due schermate.

## 7. Limiti noti e in arrivo

- Le statistiche vedono solo quello che la traccia campiona: un fotogramma ogni 5 s (2 s per aerei e armi in acqua) e
  uno a ogni evento. Una manovra più breve non lascia segno nei tempi.
- Le cumulate non si ricalcolano se cambiano le regole: si sommano i report di quando la missione è finita.

## 8. Riferimenti

- Codice: `nucleo/statistiche.hpp`, `nucleo/src/statistiche.cpp`, `app/statistiche.hpp`, `app/statistiche.cpp`.
- Dati: la traccia del replay (`.m688tr`, capitolo F5); `statistiche.toml` accanto ai fascicoli.
- Progetto: nota condivisa `statistiche-proposta.md` (approvata il 26/10/2026).
