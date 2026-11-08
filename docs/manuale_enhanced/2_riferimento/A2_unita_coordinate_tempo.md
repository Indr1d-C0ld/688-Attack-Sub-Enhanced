# A2. Unità, coordinate e tempo

*Autore: principale. Revisione del 26/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Dice in che unità il gioco conta, dove stanno le cose e che ora è. Dentro, tutto è in metri, secondi e gradi su un
piano centrato sul teatro. Fuori, il giocatore legge piedi, nodi, miglia e il gruppo data-ora dei messaggi, come a
bordo di un 688 nel 1989. Il capitolo spiega anche una particolarità delle missioni originali: il mondo vi corre più
veloce del vero, perché gli script del 1989 contano su quei tempi.

## 2. Il modello

### 2.1 Le unità di misura

Dentro la simulazione: metri, metri al secondo, secondi, gradi. Le costanti sono in `nucleo/geometria.hpp`.

| Grandezza | Valore | Dove |
|---|---|---|
| nodo | 1852/3600 m/s = 0,51444 m/s | `NODO` |
| miglio nautico | 1852 m | `MIGLIO` |
| iarda | 0,9144 m | `IARDA` |
| piede | 0,3048 m | nei pannelli (`app/scena_prova.cpp`) e nelle costanti della missione |
| raggio terrestre | 6 371 000 m | `RAGGIO_TERRA` |
| raggio per la rifrazione standard | 6 371 km × 7/6 | `RAGGIO_TERRA_RIFRAZIONE` (orizzonte, navi sotto l'orizzonte) |

- **Angoli**: gradi bussola, 0 = nord, crescenti in senso orario. L'originale usava angoli matematici (0 = est,
  antiorario); il convertitore delle missioni scrive tutto in bussola, con virate e ratei positivi a dritta
  (`engine/README.md`).
- **Rilevamenti**: veri, dal battello verso il contatto (`rilevamento(da, a)`).
- **Quote**: metri sotto il mare, positive verso il basso; 0 in superficie. Gli aerei dell'Enhanced hanno una quota
  negativa nel mondo da disegnare (`-quota` metri sopra il mare).
- **Le unità dell'originale**: una mappa di 10 240 × 6 400 unità per teatro, 1 nm di gioco = 50 unità; quote in ottavi
  di piede. Restano in alcune costanti della missione, tradotte in metri: la finestra d'impatto di 100 piedi
  (30,48 m), la superficie a 10 ottavi di piede (0,381 m).
- **Nei pannelli**: il pannello di Manovra legge la quota in piedi (`QUOTA 400 FT`), come gli strumenti
  dell'originale; la barra del bersaglio le distanze in miglia nautiche (`NM`) e le velocità in nodi; la Centrale
  scrive la quota in metri e in piedi insieme (`app/scena_prova.cpp`).

### 2.2 Il piano del teatro

`nucleo/fondale.hpp`. Ogni teatro è una griglia regolare in **proiezione azimutale equidistante** sull'ellissoide
WGS84, col centro nel teatro.

- **Assi**: x verso est, y verso nord, in metri. L'origine (0, 0) è il centro della griglia, che è il centro della
  proiezione: la stessa origine delle posizioni delle missioni.
- **Celle**: 200 m. Quota del terreno `z = valore × scala + offset` (scala 0,25 m, offset −11 000 m), positiva sopra il
  mare.
- **Geografiche**: `Fondale::geografiche` e `Fondale::proiettata` risolvono i problemi diretto e inverso di Vincenty dal
  centro (blocco `GEO1` del file `.fdl`). In questa proiezione distanza e azimut dal centro sono quelli geodetici.
  Scarto da pyproj sotto 1 cm a 230 km (capitolo C12).

I sette teatri (letti dai file `data/motore/teatri/<teatro>.fdl`):

| Teatro | Centro della proiezione | Celle | Estensione |
|---|---|---|---|
| Baltico | 55,15° N 13,35° E | 1817 × 1392 | 363 × 278 km |
| Barents | 70,15° N 35,00° E | 1515 × 1562 | 303 × 312 km |
| Mar Celtico | 48,25° N 9,75° W | 1671 × 1390 | 334 × 278 km |
| Fær Øer | 62,00° N 7,00° W | 1571 × 1672 | 314 × 334 km |
| Gibilterra | 35,90° N 5,50° W | 1805 × 999 | 361 × 200 km |
| Kunashir | 44,45° N 146,65° E | 1472 × 1278 | 294 × 256 km |
| Otranto | 41,00° N 18,75° E | 1472 × 1666 | 294 × 333 km |

### 2.3 La posa delle missioni originali

Le carte del 1989 sono stilizzate. Ogni missione convertita dichiara come si posa sul teatro vero (`posa` nel file
Lua, formato in `docs/09_schema_missioni.md` §3):

- **metodo**: `georef` (la carta originale collocata sul teatro reale), `ricollocata` (le terre della carta
  sovrapposte alla costa vera, con traslazione e scala, senza rotazione), `scala_di_gioco` (1 nm = 50 unità,
  37,04 m per unità, centrata sul teatro);
- **centro**: latitudine e longitudine del centro della carta originale;
- **metri per unità** sui due assi;
- **moto classico**: i metri al secondo di un nodo in questa posa.

Il convertitore porta le posizioni in metri sul piano del teatro. Il motore legge dalla posa i metri per unità (per la
scala della carta), il centro (§2.4) e il moto classico.

**Il moto classico.** L'originale muove le unità circa 3,46 volte più veloci del vero, in scala di gioco
(`docs/originale/sensori_e_navigazione.md` §6). Per tenere i tempi e le distanze degli script, una velocità in nodi
diventa metri al secondo così:

```
v [m/s] = v [kn] · m_nodo          m_nodo = 0,048065 · metri per unità
k = m_nodo / 0,51444               (quante volte il mondo corre più del vero)
```

`m_nodo` è nel file (`moto_classico_m_s_per_nodo`); con due scale diverse sui due assi il convertitore usa la media. Se
manca, il motore prende 0,048065 per i metri per unità dell'asse x (`missioni/src/missione.cpp`).

| Teatro | Missioni | Metodo | Metri per unità | m_nodo (m/s) | k |
|---|---|---|---|---|---|
| Baltico | 0 | ricollocata | 45,930 | 2,2076 | 4,29 |
| Fær Øer | 1, 2 | ricollocata | 23,335 | 1,1216 | 2,18 |
| Barents | 3 | scala di gioco | 37,040 | 1,7803 | 3,46 |
| Gibilterra | 4, 5 | georef | 22,969 × 27,780 | 1,2196 | 2,37 |
| Otranto | 6 | georef | 21,522 × 21,703 | 1,0388 | 2,02 |
| Kunashir | 7 | georef | 20,256 × 24,597 | 1,0779 | 2,10 |
| Mar Celtico | 8, 9 | scala di gioco | 37,040 | 1,7803 | 3,46 |

Esempio: a TORPEX '89 (Fær Øer) una fregata a 10 nodi percorre 11,2 m/s, non 5,1: in un minuto fa 673 m, come in 2,2
minuti di mare vero.

- **Che cosa corre con k**: il moto di tutte le unità (`Missione::navigazione`), i siluri, le mine e le sorgenti del
  paesaggio sonoro (che avanzano con `dt · k`).
- **Che cosa resta vero**: lo spazio (le posizioni sono metri sul teatro vero, le distanze in miglia sono miglia vere),
  i timer degli script, la radio, il meteo, gli urti col fondo e i danni (che usano la velocità vera, nodi × 0,51444).
- **Missioni rapide, della libreria e dell'editor**: posa `rapida`, `m_nodo = 0,514444`, cioè k = 1. Velocità vere
  anche nel classico.

### 2.4 Latitudine e longitudine nella missione

Oltre al piano, la missione tiene un punto geografico: il **centro della posa** (`posa.centro`). Lo usano:
- il Sole della missione, per la luce che vedono le vedette e i periscopi dell'IA (ricalcolata ogni 60 s di missione,
  `missioni/src/missione_periscopi.cpp`);
- il meteo (origine del riquadro e ora locale `UTC + longitudine/15`);
- l'ora locale dell'ASW aereo;
- la latitudine del battello per la radio (copertura del satellite fino a 72°), stimata così:

```
latitudine ≈ latitudine del centro della posa + y / 111 120 m
```

Il centro della posa non coincide con il centro della proiezione (§2.2) in cinque teatri su sette (§7).

### 2.5 Il tempo di missione

- **Tempo di missione**: secondi da 0 all'avvio, contati dal passo fisso (capitolo A1). È `Missione::tempo()`.
- **Data e ora d'inizio** (`DataOra`, data e ore UTC decimali):

| Missione | Data e ora d'inizio |
|---|---|
| originali, Enhanced | quelle degli ordini del loro file radio (`data/missioni/radio/`, `ordini.data`): TORPEX 14/3/1989 alle 10Z; `--data` e `--ora` prevalgono |
| originali, classico | 15/10/1989, 09.00 UTC (`app/main.cpp`); il classico non ha la sala radio né il GDO |
| rapide | quelle del generatore (capitolo F6) |
| editor, libreria, importate | quelle dello scenario |
| codice partita di una missione non originale | quelle dello scenario |

- La partita passa data e ora alla missione prima del primo passo (`Missione::imposta_data_ora`, in
  `app/partita.cpp`). Prevalgono sulla data degli ordini del file radio della missione. Senza (negli strumenti come il
  rilievo delle 110 partite) vale la data del file radio, o il 24/10/1989 alle 00Z.

**Come si legge l'ora:**

| Dove | Forma | Calcolo |
|---|---|---|
| debriefing, ricostruzione della ripresa | `T+01:23:45` | tempo di missione |
| registro dei messaggi (Ctrl+L) | `01:23:45` | tempo di missione |
| carta (segni, linee, datum, mine) | `1423` | ora UTC del giorno, `(ora d'inizio + t) mod 24 h` |
| sala radio, briefing, debriefing | gruppo data-ora (GDO), `241530Z OCT 89` | `SalaRadio::dtg` (sotto) |

**Il gruppo data-ora** (`nucleo/src/radio.cpp`, `SalaRadio::dtg(t)`):

```
s = ora d'inizio [s] + max(t, 0)
finché s ≥ 86 400: s −= 86 400; giorno + 1 (fine mese e anni bisestili gregoriani)
GDO = GG HH MM "Z" MESE AA        (MESE in inglese: JAN ... DEC)
```

Esempio: partenza il 24 ottobre 1989 alle 15.30 UTC, `t = 0` → `241530Z OCT 89` (`test/prove_radio.cpp`). La sala
radio c'è solo nell'Enhanced: senza, il debriefing scrive `T+`.

### 2.6 Il Sole e la luce della vista

`app/main.cpp`, `componi`: la vista al periscopio prende la luce dall'elevazione del Sole (`nucleo/sole.hpp`,
algoritmo semplificato della NOAA) e da `luce_ambiente` (capitolo E8).

```
ora UTC = (ora d'inizio + t / 3600) mod 24        la data resta quella d'inizio
elevazione = elevazione_sole(data, ora UTC, latitudine, longitudine)
```

Il punto del Sole per la vista, in ordine di precedenza:
1. `--lat` e `--lon` dalla riga di comando, che spostano anche il Sole della missione (`Missione::luogo_del_sole`);
2. Enhanced, missione originale: il **centro della proiezione** del teatro (blocco `GEO1`), se il fondale è
   georeferenziato;
3. rapide, editor, libreria e importate: il centro dello scenario;
4. altrimenti (il classico): 61,5° N 7,0° W, la scena di prova a sud delle Fær Øer.

A ogni fotogramma (e a ogni passo durante la ricostruzione della ripresa) la stessa luce va alla partita
(`Partita::imposta_luce`): serve ai marcatori dell'ASW aereo, che di notte (luce sotto 0,3) diventano quelli notturni.
È solo vista: la simulazione ha la sua luce (§2.4).

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Nodo, miglio, iarda, piede | definizioni internazionali | alta |
| Proiezione azimutale equidistante, Vincenty | Vincenty (1975); verificata con pyproj | alta |
| Fondali e centri dei teatri | EMODnet, ETOPO 2022 (capitolo C12) | alta |
| Posa delle carte originali | `docs/09_schema_missioni.md` §3 (`georef` alta per Gibilterra, media per Otranto e Kunashir; `ricollocata` media; `scala_di_gioco` bassa) | vedi a fianco |
| Moto classico, 0,048065 unità al secondo per nodo | `docs/originale/sensori_e_navigazione.md` §6, dal disassemblato | alta |
| Data d'inizio delle originali | gli ordini dei file radio R3 (scritti per l'Enhanced) | **STIMA** di gioco, coerente con gli ordini |
| Posizione del Sole | algoritmo semplificato NOAA | alta (qualche decimo di grado) |

## 4. Classico ed Enhanced

- Unità, piano del teatro, posa e moto classico sono gli stessi nei due modi.
- La data e l'ora d'inizio sono le stesse; il GDO c'è solo nell'Enhanced, con la sala radio.
- Il Sole della vista: nel classico sempre alle Fær Øer (61,5° N 7,0° W), salvo `--lat` e `--lon`; nell'Enhanced sul
  teatro vero.
- Le rapide, la libreria e l'editor hanno velocità vere in tutti e due i modi.

## 5. Come si vede in gioco

- Il pannello e la barra del bersaglio: quota in piedi, distanza in miglia, velocità in nodi (volume 1, cap. 2).
- La carta: miglia vere, ore UTC a quattro cifre sui segni e sulle linee di rilevamento (volume 1, cap. 5).
- La sala radio e il briefing: il GDO degli ordini e delle finestre radio (volume 1, §4.7).
- Il debriefing: l'ora `T+` e il GDO dell'evento (capitolo F1).
- La luce della vista: alba, tramonto e notte sul teatro vero (capitolo E8).

## 6. Taratura e prove

- `test/prove_nucleo.cpp`: «angoli normalizzati», «rilevamenti nautici: 0 nord, 90 est», «angolo sulla prua e lato»,
  «orizzonte con rifrazione standard», «fondale: georeferenziazione azimutale equidistante sull'ellissoide WGS84»
  (contro pyproj in tre teatri).
- `test/prove_radio.cpp`: il GDO all'inizio (`241530Z OCT 89`) e i suoi cambi di giorno.
- La posa delle carte originali: risultati e confronto visivo in `re/surf/` (`docs/09_schema_missioni.md` §3).

## 7. Limiti noti e in arrivo

- **La data delle originali** (corretta il 26/10/2026). Prima partivano tutte il 15/10/1989 alle 09.00 UTC. Ora
  l'Enhanced prende la data e l'ora degli ordini del loro file radio: luce, meteo della stagione e GDO seguono gli
  ordini che si leggono nel briefing.
- **La data non avanza a mezzanotte** per il Sole: l'ora si ripiega su 24 h con la data d'inizio. Il GDO invece cambia
  giorno. In missioni di qualche ora l'effetto è di pochi minuti d'arco sul Sole.
- **Due centri.** La vista usa il centro della proiezione; la simulazione usa il centro della posa (luce dei periscopi,
  meteo, ASW, radio). Differiscono in cinque teatri: Baltico −0,24° di latitudine e +1,73° di longitudine, Fær Øer
  −0,08° e +0,10°, Gibilterra −0,13° e −0,34°, Otranto −0,56° e −0,07°, Kunashir −0,57° e −1,33°; uguali a Barents e
  nel Mar Celtico. Anche la latitudine della radio parte dal centro della posa, non dall'origine del piano: conta solo
  per la copertura del satellite oltre 72°, che nessun teatro supera.
- **Il moto classico** rende le velocità delle originali non fisiche nel piano vero (k da 2,02 a 4,29). Gli strumenti
  che convertono metri al secondo in nodi veri vanno letti con k (capitolo C13, MATE).
- **Il mese del GDO** è in inglese (`OCT`); il glossario della traduzione propone `OTT`.
- **In arrivo**: la posizione incerta del battello (capitolo C12), che separerà la posizione vera da quella stimata.

## 8. Riferimenti

- Codice: `nucleo/geometria.hpp`, `nucleo/fondale.hpp` e `nucleo/src/fondale.cpp`, `nucleo/sole.hpp`,
  `nucleo/src/radio.cpp` (`SalaRadio::dtg`), `missioni/src/missione.cpp` (posa, `metri_al_secondo`,
  `navigazione`, `imposta_data_ora` in `missione.hpp`), `missioni/src/missione_periscopi.cpp`,
  `missioni/src/missione_meteo.cpp`, `app/main.cpp` (`componi`, data e luogo), `app/partita.cpp`, `app/carta.cpp`
  (`ora_hhmm`).
- Dati: `data/motore/teatri/<teatro>.fdl` (blocco `GEO1`), `data/missioni/SCENE*.lua` (`posa`),
  `data/missioni/radio/` (date degli ordini).
- Progetto: `docs/09_schema_missioni.md` §3, `docs/08_fondali_e_navigazione.md`, `engine/README.md`.
- Originale: `docs/originale/sensori_e_navigazione.md` §6 (moto), `docs/originale/numeri_eseguibile.md` §3.3-§4
  (angoli).
- Capitoli: C12 (fondale e carta), E8 (luce), A1 (passo fisso).
