# E8. Giorno, notte e cielo

*Autore: kit. Revisione del 29/10/2026 (la luce della missione sente il coperto e segue `--lat` e `--lon`). Stato:
rivisto (curatore: kit).*

## 1. A che serve

Decide quanta luce c'è: per la palette della vista (il cielo e il mare che si scuriscono al tramonto), per quello che
le vedette e i periscopi vedono (capitolo C9), per la luce rossa della centrale, per le stelle del punto nave
astronomico (capitolo C12). Il sole sta dove deve stare per quel posto, quel giorno e quell'ora.

## 2. Il modello

### 2.1 Il Sole

`nucleo/sole.hpp`, `elevazione_sole(data e ora UTC, latitudine, longitudine)`: l'algoritmo semplificato della NOAA
(equazione del tempo e declinazione con la serie di Spencer), con un errore di qualche decimo di grado.

Il luogo:
- le missioni dell'Enhanced: il centro della missione (`posa.centro`), cioè il centro della batimetria
  georeferenziata del teatro;
- le missioni rapide e l'editor: il centro dello scenario;
- `--lat` e `--lon` da riga di comando sostituiscono il luogo, per la vista e per la missione
  (`Missione::luogo_del_sole`): spostano il Sole, non la geografia;
- senza nessuno di questi (il classico): 61,5° N, 7° O, alle Fær Øer.

La data e l'ora:
- le originali dell'Enhanced partono dalla data e dall'ora dei loro ordini (`data = { giorno, mese, anno, ora_utc }`
  nel file della sala radio). TORPEX parte il 14 marzo 1989 alle 10Z;
- rapide, editor e scenari della libreria hanno la loro;
- `--data AAAA-MM-GG` e `--ora HH.hh` prevalgono;
- senza nessuno di questi: il 15 ottobre 1989 alle 9 UTC.

### 2.2 La luce ambiente

```
t    = clamp((e + 12) / 22, 0, 1)
luce = t² (3 − 2t)
```

con `e` l'elevazione del Sole in gradi (`luce_ambiente`). Vale zero dal crepuscolo nautico (−12°) in giù, uno dal
pieno giorno (+10°) in su, con un raccordo dolce in mezzo. La luce 0,2, la soglia della «notte» di §2.5, corrisponde a
un Sole circa 5,7° sotto l'orizzonte.

Il coperto toglie luce alla vista (`cielo::luce_sotto_le_nubi`, solo col meteo dell'Enhanced):

```
luce' = luce · clamp(1 − 0,45 · n^1,5 − 0,3 · T, 0,2, 1)
```

- `n`: la copertura delle nubi, da 0 (sereno) a 1 (coperto);
- `T`: il temporale, da 0 a 1.

Il coperto pieno col temporale lascia un quarto della luce; il solo coperto il 55%.

**Illuminanti e lampi.** Gli illuminanti dell'artiglieria (capitolo E4) aggiungono luce sul mare dove guarda il
periscopio. I lampi schiariscono la palette della vista (`Palette::schiarita`):
- ogni colpo dura 30 ms, uno ogni 57 ms; i colpi successivi valgono il 70% del precedente;
- un fulmine ha 1 colpo nel 30% dei casi, altrimenti 2 o più, 3,67 in media (`data/motore/meteo.toml`);
- il lampo è pieno di notte e un quarto di giorno (fattore 1 − 0,75 · luce), e cala con la distanza.

### 2.3 La palette della vista

Ogni scena ha la sua palette di 256 colori, quella del giorno dell'originale (DAY.PAL). La luce agisce solo sugli
indici della vista (cielo, mare, grigi degli sprite, `INDICI_VISTA`): si scuriscono e virano al blu dal giorno alla
notte (`Palette::illuminata`). Gli indici dell'interfaccia restano fermi.

Gli interni dell'Enhanced hanno la loro luce di sala (`grafica/tema_stazioni.hpp`, capitolo E9):
- bianca di giorno;
- rossa di notte sul 688, con la luce sotto quella di un Sole a −6° (circa 0,18);
- blu nella sala sonar del 688.

Lampade e fosfori non cambiano con la luce.

### 2.4 La notte per chi guarda

Il modello dei periscopi (capitolo C9, `fattore_notte` in `nucleo/src/periscopi.cpp`) scala la portata visiva
secondo la luce e il sensore. Con la luce sotto 0,1 resta di notte questa frazione della portata di giorno; fra 0,1 e
0,7 si passa in modo lineare al giorno:

| Sensore | Notte serena | Notte coperta |
|---|---|---|
| occhio | 5% | 2% |
| intensificatore | 30% | 10% |
| bassa luminosità e TV | 25% | 8% |
| camera termica | 60% | 60% |

La copertura conta in modo lineare fra sereno e 8 ottavi; il termico non sente le nubi.

Gli aerei ASW accendono le luci anticollisione e di navigazione, e le spengono in caccia in tempo di guerra
(`nucleo/src/asw_aereo.cpp`, capitolo E5).

### 2.5 La luce nella missione

La missione (`Missione::luce_periscopi`) ricalcola ogni 60 s la luce dal Sole nel suo luogo, alla sua data e alla sua
ora. Il coperto la riduce: `luce · (1 − 0,5 · n)`, con `n` le nubi in ottavi diviso 8 sopra il battello (**STIMA**).
Col cielo coperto la luce del giorno si dimezza; di notte, a luce zero, non cambia niente. È «notte» sotto 0,2: un
crepuscolo coperto arriva prima alla notte. La usano:
- i periscopi del giocatore e dell'IA (§2.4);
- il periscopio di un sottomarino dell'IA, che vede le navi fino a 15 km di giorno e 3 km di notte;
- l'artiglieria (capitolo E4): le navi vedono la piuma del periscopio entro 1 km di giorno e 300 m di notte, e di
  notte sparano gli illuminanti sopra il bersaglio;
- il punto nave astronomico, che vuole la notte o il crepuscolo e il cielo con al più 3 ottavi di nubi (capitolo
  C12).

Senza il modello dei periscopi (il classico) la luce della missione vale sempre 1, il pieno giorno.

L'app tiene la sua luce per gli effetti della partita: il marcatore OMAB sovietico è di notte la fiamma (OMAB-25-8N)
sotto 0,3, di giorno la macchia di fluoresceina.

### 2.6 La Luna

`nucleo/sagoma_notte` (`cielo_notte`, `posizione_luna`) dà dalla data, dal luogo e dalle nubi il cielo della notte
(`CieloNotte`):
- l'illuminamento sul mare in lux;
- se la Luna è su, la sua elevazione e il suo azimut;
- la frazione illuminata e l'angolo del lembo illuminato: 0 in alto, verso lo zenit, e 90 a destra per chi guarda la
  Luna.

Senza data non c'è Luna. Lo usano l'IA e le vedette, per la portata delle sagome, e la vista del periscopio. La vista
ne prende la luce: il massimo fra il crepuscolo equivalente e un adattamento dell'occhio da 0,003 a 0,25 lux, portato
a 0-0,22. Disegna poi il disco con la sua falce o la sua gobba e la scia sul mare (capitolo C9 §2.13): quello che si
vede torna con quello che vede l'IA.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Posizione del Sole | algoritmo semplificato NOAA | alta (qualche decimo di grado) |
| Luce dal crepuscolo nautico al giorno | stima di gioco | **STIMA** |
| Attenuazione delle nubi | stima di gioco | **STIMA** |
| Lampi: tempi dei colpi, colpi per fulmine | `cielo.hpp`, `meteo.toml` (dossier 21) | **STIMA** |
| Portate visive di notte | dossier 20, dossier 21 §6.2 | **STIMA** da tarare |
| Soglia della notte a 0,2, luce rossa sotto −6° | scelte di gioco | — |
| Il coperto dimezza la luce della missione | scelta di gioco | **STIMA** |

## 4. Classico ed Enhanced

Il ciclo del giorno vale in tutti e due per la vista: la palette si scurisce secondo il Sole. Nel classico il luogo e
la data sono quelli di riserva (§2.1), salvo `--data`, `--ora`, `--lat` e `--lon`. Sono solo dell'Enhanced: la data
degli ordini e il centro del teatro, il coperto e i lampi, gli illuminanti, i modi notturni dei periscopi, la luce
della missione per l'IA, l'artiglieria e il punto nave astronomico, la luce rossa e blu delle sale.

## 5. Come si vede in gioco

Volume 1, capitoli 2 (luci di bordo), 4 (periscopi di notte) e 9 (giorno e notte); capitolo 5 §5.2 per il punto nave
astronomico (le stelle al periscopio, nel menu Ctrl+O della carta, «SERVE IL CREPUSCOLO O LA NOTTE» e «CIELO COPERTO»
quando non si può).

## 6. Taratura e prove

- `test/prove_scena.cpp`: «elevazione del Sole», «luce ambiente», «palette illuminata: la vista si scurisce,
  l'interfaccia no».
- `test/prove_meteo_grafica.cpp`: «luce sotto le nubi: il coperto scurisce, il meteo spento no»; «fulmini: canale
  bianco durante i colpi, niente fra un colpo e l'altro, lampo piu' forte di notte»; «nubi: copertura dal sereno al
  coperto, solo sul cielo, cumulonembi piu' scuri».
- `test/prove_periscopi.cpp`: «periscopi: portate visive per testa, piuma, mare, notte e osservatore»; «periscopi
  missione: la notte e la luce dal sole della missione».
- `test/prove_navigazione.cpp`: «navigazione: le condizioni dei punti nave» (l'astronomico vuole la notte e il cielo
  sereno).
- Anteprime: `melchior688 --ora HH.hh --data AAAA-MM-GG --lat GRADI --lon GRADI`, e `anteprima_meteo` (giorno, notte,
  temporale, nebbia).

## 7. Limiti noti e in arrivo

- Il crepuscolo non tinge il cielo d'arancio: la palette scurisce e vira al blu.
- La palette notturna dell'originale (NIGHT.PAL) è esportata ma l'app non la usa: anche il classico scurisce la
  palette del giorno.

## 8. Riferimenti

- Codice: `nucleo/sole.hpp`, `nucleo/src/sole.cpp`, `grafica/cielo.hpp` e `grafica/src/cielo.cpp`
  (`luce_sotto_le_nubi`, `lampo`), `grafica/palette.hpp`, `grafica/tema_stazioni.hpp`, `nucleo/src/periscopi.cpp`
  (`fattore_notte`), `missioni/src/missione_periscopi.cpp` (`luce_periscopi`), `missioni/src/missione_artiglieria.cpp`,
  `missioni/src/missione_navigazione.cpp`, `app/main.cpp` (`componi`, le opzioni), `app/scena_prova.cpp`
  (`tema_consolle`).
- Dati: `data/motore/meteo.toml` (i lampi), `data/motore/periscopi.toml`, i file della sala radio delle missioni
  (`data/missioni/radio/`, la data degli ordini).
- Ricerca: dossier 20 (periscopi), dossier 21 (meteo).
- Volume 2: capitoli C9, C12, E4, E5, E7, E9.
