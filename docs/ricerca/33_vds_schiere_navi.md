# 33 — Il sonar a profondità variabile (VDS) e le schiere rimorchiate delle navi

Disassemblatore, 30/10/2026. L'ha chiesto la principale. Il motore metteva ogni sensore della nave alla quota dello
scafo, e il «Horse Tail» del Polinom era reso alla buona col solo rumore proprio più basso (dossier 31 §4).

**Le fonti.**
- Solo pagine aperte, senza file scaricati, con citazioni brevi.
- Confidenza: **alta** quando le fonti concordano, **media** con una sola fonte, **bassa** quando è debole o in disaccordo.
- Le scelte di gioco sono **STIMA**. Mancano quasi tutti i numeri dei verricelli e delle velocità di traino.

## 1. I sistemi

| Sistema | Navi nel gioco | Cosa si sa | Confidenza |
|---|---|---|---|
| AN/SQS-35 IVDS | Knox | Attivo a circa 13 kHz, con un modo passivo «listen». Il pesce si cala da poppa «fino a 600 piedi (180 m)». Montato su 25 delle 46 unità (Wikipedia, Knox-class e AN/SQS-35). | alta |
| AN/SQR-18A TACTAS | Knox | Il pesce dell'SQS-35 «fu poi modificato per trainare» la SQR-18A (Wikipedia, Knox-class); FAS la dice installata sulle FF-1052. Lunghezza, cavo e velocità non trovati. | media |
| AN/SQR-19 TACTAS | Spruance, Perry, Ticonderoga | Fa parte dell'SQQ-89. Una rivista navale brasiliana dà 1.800 m di cavo, quota massima 335 m e mare fino a 4; FAS dice «circa un miglio dietro la nave». Le date di montaggio per unità non si sono trovate. | media |
| MGK-355 «Polinom», «Horse Tail» | Udaloy | Due parti, «Horse Jaw» di prua e «Horse Tail» rimorchiata (Wikipedia, Udaloy-class). La ru.wikipedia descrive solo la cupola di prua, circa 30 × 5,1 m. | media |
| MG-325 «Vega» | Kara, Krivak II | Sul Kara: «a profondità variabile», a poppa, «può scendere a 100 m» (Wikipedia, Kara-class). La ru.wikipedia del progetto 1135 la dà «rimorchiata». | media |
| MG-345 «Bronza» | Krivak III, non nel gioco | La ru.wikipedia dà sul progetto 11351 una «ГАС «Бронза» rimorchiata»; la en.wikipedia chiama Bronza il sonar di prua. **Nel gioco è un VDS (STIMA, decisione della principale).** | bassa |
| «Shelon» | Grisha III | Calabile, a nave ferma (dossier 31 §5). | media-bassa |
| Type 2031Z | Type 22 Batch 2 | Fuori: la classe del gioco unisce i due lotti. | — |

**Schiere sovietiche lunghe sulle navi di superficie nel 1989: nessuna trovata.** La Zvezda della Neustrashimy arriva
dopo il 1990, e l'«Ox Tail» della en.wikipedia (Krivak) è un VDS. I sovietici hanno VDS, non schiere.

## 2. Il modello (nucleo/schiera, un solo modello)

Il pesce del VDS è una **«schiera corta» al cavo**, con la stessa legge di Pode del cavo delle schiere (dossier 04
§1.5). Il peso del pesce e del depressore si spalma sul cavo come peso equivalente (STIMA). Ne seguono:
- la quota sale con la velocità: a 5 nodi il pesce pende, a 15 nodi risale;
- la quota ordinata si raggiunge filando il cavo (`cavo_per_quota_m`), al massimo fino alla quota d'equilibrio col cavo
  intero e alla quota massima del sistema;
- il tempo del cavo, cioè il ritardo, è quello delle schiere;
- le velocità per filare e per recuperare (v_manovra) e quella di traino (v_max), oltre la quale c'è il danno, come per
  le schiere dei sottomarini.

I numeri, tutti **STIMA** salvo le quote massime:

| Sistema | Cavo | Peso equivalente | Quota massima | Filare | Traino max | A 10 kn |
|---|---|---|---|---|---|---|
| sqs35_vds | 250 m, d 4 cm | 280 N/m | 180 m (alta) | 12 kn | 20 kn | circa 180 m |
| vega_vds | 150 m | 150 N/m | 100 m (media) | 12 kn | 18 kn | circa 80 m |
| polinom_vds | 300 m | 250 N/m | 200 m | 12 kn | 20 kn | circa 200 m |
| bronza_vds | 200 m | 200 N/m | 150 m | 12 kn | 18 kn | — |
| shelon_vds | 200 m | 400 N/m | 150 m | 0,5 kn | 3 kn | calabile da ferma |
| sqr18a | 1.500 m, trainata dal pesce | 10 N/m | 360 m | 12 kn | 20 kn | — |

Il rumore proprio:
- il pesce sta lontano dalle macchine: niente via strutturale (isolamento 45 dB, circa la divergenza su 200 m d'acqua) e il
  macchinario 10 dB sotto il sonar di prua (STIMA);
- il flusso è quello del pesce;
- sordo oltre la velocità di traino;
- la cupola di prua del Polinom torna al rumore delle navi sovietiche, ora che il VDS è a parte.

La quota contro lo strato non chiede codice nuovo: la propagazione usa già la quota del Ricevitore.

## 3. L'IA della nave (missione_vds.cpp), con le regole del giocatore (simmetria)

- **Filare:** la nave fila schiera e VDS sotto la velocità per filare e con l'acqua che basta. Il cavo della schiera è
  quello che non tocca il fondo (cavo_sicuro_m, come i sottomarini dell'IA).
- **La quota del VDS:**
  - con lo strato (la nave ha il batitermografo), 30 m sotto lo strato;
  - senza strato, due terzi della quota massima;
  - in ogni caso mai entro 30 m dal fondo (STIMA).
- **Recupero prima di accelerare:**
  - col cavo fuori la nave non supera la velocità di traino;
  - se lo script vuole di più, la nave recupera alla velocità per filare e accelera solo col cavo dentro;
  - con un siluro in acqua contro di lei accelera subito, e il traino veloce porta il rischio di danno.
- **Sprint and drift:**
  - in deriva per 5 minuti a 6 nodi, oppure ferma per lo Shelon, poi in scatto per 10 minuti;
  - la velocità dello scatto tiene la media voluta dallo script, quindi la nave arriva dove deve quando deve;
  - se lo scatto supererebbe la velocità di traino o l'avanti tutta, niente ciclo.
- **Il registro:** «filato» e «recuperato», con la quota, per il debriefing e il replay. Il giocatore non vede il VDS
  nemico.

## 4. Cosa non si è trovato

- Le velocità di traino e di messa a mare di tutti i sistemi.
- I cavi del VDS, i pesi dei pesci e le frequenze sovietiche.
- Le unità con la SQR-19 nel 1989: nel gioco l'hanno tutta la classe Perry, Spruance e Ticonderoga, e il VDS tutta la
  classe Knox (STIMA).
