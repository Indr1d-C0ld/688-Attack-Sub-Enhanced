# Pipeline dei dati ambientali (Python)

Script in `tools/pipeline/`, dati in `data/teatri/<id>/`. Tutto si esegue con l'ambiente
`tools/venv` e la radice del progetto come argomento.

```bash
tools/venv/bin/python -I tools/pipeline/fetch_etopo.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/pipeline/fetch_woa.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/pipeline/georef_surf.py /home/randolph/Claude/melchior-688
```

## 1. Teatri (`tools/pipeline/teatri.yaml`)

Sette teatri con il riquadro geografico del dossier `docs/ricerca/03_teatri_fondali_storia.md`
e le missioni originali che vi si svolgono. Unica eccezione: il Barents, spostato di 0,45° a sud per
includere la baia di Kola (vedi §2b).

## 2. Batimetria: ETOPO 2022 (`fetch_etopo.py`)

Fonte: NOAA NCEI ETOPO 2022 v1, 15″, tramite ERDDAP CoastWatch (`ETOPO_2022_v1_15s`), dati del
governo USA di libero uso. Per ogni teatro:

| File | Contenuto |
|---|---|
| `etopo15s.nc` | Riquadro del teatro a piena risoluzione (15″ ≈ 460 m) |
| `etopo_ampio_60s.nc` | Riquadro allargato di 4° in latitudine e 8° in longitudine, passo 60″, per le ricerche |

Keflavík, Vladivostok e Rybachiy stanno fuori da tutti i riquadri: i loro ritagli a 15″ sono in `data/basi/<base>.nc`
(`fetch_etopo_basi.py`, 5 MB, scaricati col permesso dell'utente il 06/11/2026). Con queste griglie `carte_basi.py` fa
la carta di ogni base per il Museo (`data/motore/basi/<base>.fdl`); la simulazione non le legge.

## 2b. Heightmap per il motore (`build_heightmap.py`)

```bash
tools/venv/bin/python -I tools/pipeline/build_heightmap.py /home/randolph/Claude/melchior-688
```
Si possono indicare uno o più teatri come argomenti aggiuntivi (per esempio `gibilterra`).

**Fonti.** Per i 6 teatri europei la fonte primaria è EMODnet Bathymetry DTM, release corrente,
1/16′ (circa 115 m), dal servizio WCS `https://ows.emodnet-bathymetry.eu/wcs` (copertura
`emodnet__mean`, licenza CC-BY 4.0). La copertura arriva fino a 43°E, quindi include anche il
Barents meridionale, e contiene le quote di terra. Kunashir è fuori copertura e usa ETOPO 2022 15″;
ETOPO riempie anche gli eventuali vuoti di EMODnet.

Il GeoTIFF restituito dal WCS **non ha tag di georeferenziazione**. La griglia si ricostruisce dalla
`DescribeCoverage` (origine 90°N / 70,5°W ai centri dei pixel, passo 1/960°, riga 0 = nord). Il
ritaglio di Gibilterra confrontato con ETOPO dà correlazione 0,9996 e differenza mediana −0,3 m,
che conferma orientamento e allineamento. Il ritaglio viene salvato come `emodnet.nc` con le sue
coordinate.

**Griglia.** Proiezione azimutale equidistante (WGS84) centrata sul riquadro del teatro, celle
quadrate da 200 m (`cella_m` in `teatri.yaml`). Le dimensioni sono quelle del riquadro misurate
lungo i meridiani e i paralleli centrali. Ricampionamento bilineare con sovracampionamento 2×2 e
media (anti-aliasing).

**Uscite** in `data/teatri/<id>/`:

| File | Contenuto |
|---|---|
| `heightmap_u16.png` | PNG in scala di grigi a 16 bit; quota `z [m] = valore × 0,25 − 11000` (da −11000 a +5383 m, passo 0,25 m); riga 0 = nord, colonna 0 = ovest |
| `fonte_u8.png` | Provenienza per cella: 1 = EMODnet, 2 = ETOPO |
| `heightmap.json` | Proiezione (stringa PROJ), centro, cella, dimensioni, formula delle coordinate di cella, codifica con errore massimo verificato, fonti con licenza e attribuzione, statistiche |
| `anteprima.png` | Immagine di controllo (batimetria in blu, terra in ocra) |

Coordinate locali del centro della cella `(col, riga)`:
`x = (col + 0,5) · cella − larghezza · cella / 2`, `y = altezza · cella / 2 − (riga + 0,5) · cella`
(metri, x verso est, y verso nord, origine nel centro della proiezione). Il motore non legge il PNG:
`tools/pipeline/esporta_motore.py` copia gli stessi valori a 16 bit in `data/motore/teatri/<id>.fdl`, compressi
senza perdite (formato `FDL2` in `engine/README.md`), letto da `nucleo/fondale.hpp`.

**Attribuzioni obbligatorie** (da riportare nei crediti del gioco): EMODnet Bathymetry Consortium,
*EMODnet Digital Bathymetry (DTM)*, CC-BY 4.0; NOAA NCEI, *ETOPO 2022*, doi:10.25921/fd45-gt74.

**Risultati** (22/10/2026, celle da 200 m; durata circa 4 minuti con il download, 80 s con i dati in cache):

| Teatro | Celle | Area (nm) | Quote (m) | Terra | Fonte |
|---|---|---|---|---|---|
| Faroe | 1571 × 1672 | 170 × 181 | −2807 … +842 | 1,5% | EMODnet |
| Barents | 1515 × 1562 | 164 × 169 | −477 … +496 | 17,8% | EMODnet |
| Gibilterra | 1805 × 999 | 195 × 108 | −1624 … +2133 | 29,4% | EMODnet |
| Otranto | 1472 × 1666 | 159 × 180 | −2184 … +2631 | 36,9% | EMODnet |
| Kunashir | 1472 × 1278 | 159 × 138 | −6912 … +1659 | 10,4% | ETOPO (15″, bordi a 60″) |
| Celtica | 1671 × 1390 | 180 × 150 | −4966 … −97 | 0% | EMODnet |
| Baltico | 1443 × 1002 | 156 × 108 | −84 … +208 | 32,3% | EMODnet |

Errore massimo di codifica verificato rileggendo ogni PNG: 0,1255 m (atteso 0,125 m per il passo
da 0,25 m, più l'arrotondamento in float32). Foglio di confronto: `extracted/heightmap_teatri.png`.

**Problemi incontrati e risolti:**
- il WCS rifiuta con un errore 500 le richieste grandi (Faroe, circa 3° × 6°): il download avviene a
  tasselli da 1° × 2°, con fino a 4 tentativi. La ricomposizione dei tasselli è identica bit per bit a
  un download unico (verificato su Gibilterra);
- a Kunashir gli angoli della griglia proiettata uscivano dal ritaglio ETOPO a 15″ (0,6% delle celle):
  ora si usa l'area allargata a 60″ come seconda fonte;
- la scala di colore delle anteprime si adatta alla profondità del teatro (99° percentile).

**Riquadro del Barents spostato a sud** (22/10/2026): da 69,2–72,0°N (dossier) a **68,75–71,55°N**,
stessa longitudine (31–39°E), per includere tutta la baia di Kola con i porti della missione 3
"Sub Ops". Verifica sulla heightmap: Murmansk, Severomorsk, Poljarnyj e Gadžievo cadono sulla costa
con acqua entro 1 km; all'imbocco della baia il fondale è di 208–278 m. ETOPO, WOA23, EMODnet e
heightmap del Barents sono stati rigenerati. La georeferenziazione di `SURFC` (affidabilità bassa)
era stata calcolata sul riquadro precedente.

**Baltico allargato (23/10/2026).** Per le missioni ricollocate sulle coste vere (Bornholm, Pomerania, fuga verso il
Kattegat) il riquadro è passato da 54,0–55,8 N, 10,5–15,0 E a **53,9–56,4 N, 10,5–16,2 E**: centro della proiezione
55,15 N 13,35 E, griglia 1817 × 1392 celle (196 × 150 nm), 100% EMODnet. I dati precedenti sono in
`data/teatri/baltico/_riquadro_2026-10-22/`. Le coordinate in metri delle missioni del Baltico dipendono dal centro
della proiezione e vanno rigenerate con `tools/re/missioni_lua.py`.

## 3. Temperatura, salinità e velocità del suono: WOA23 (`fetch_woa.py`)

Fonte: NOAA World Ocean Atlas 2023, periodo `decav` (1955–2022), griglia 0,25°, via OPeNDAP
(solo i ritagli dei teatri, non i file globali da 724 MB l'uno).

| File | Contenuto |
|---|---|
| `woa23_mensile.nc` | `t_an`, `s_an` per mese 1–12, 0–1500 m (57 livelli) |
| `woa23_stagionale.nc` | `t_an`, `s_an` per stagione, 0–5500 m |
| `ssp_mensile.json` | Profilo mediano del teatro per mese: T, S, c (TEOS-10) e c (Mackenzie 1981) |

Velocità del suono con TEOS-10 (`gsw.sound_speed`, da salinità assoluta e temperatura
conservativa). Mackenzie (1981) viene calcolata accanto per controllo: le due differiscono di meno
di 0,1 m/s nei casi verificati.

**Controllo di qualità.** Vicino al fondo la griglia a 0,25° contiene artefatti dell'analisi
oggettiva (per esempio −1,98 °C a 1500 m a Gibilterra, su una sola cella valida). Il profilo del
teatro usa quindi la mediana e scarta i livelli con meno di 3 celle valide. Per il motore servirà
anche un filtro per cella (coerenza verticale) prima di usare i profili punto per punto.

Velocità del suono in superficie e minimo del profilo (m/s):

| Teatro | Gennaio | Agosto | Profondità coperta |
|---|---|---|---|
| Faroe | 1481 / min 1458 | 1494 / min 1459 | 1500 m |
| Barents | 1462 / min 1462 | 1486 / min 1463 | 375 m |
| Gibilterra | 1511 / min 1506 | 1530 / min 1505 | 1400 m |
| Otranto | 1509 / min 1508 | 1539 / min 1511 | 1300 m |
| Kunashir | 1451 / min 1451 | 1502 / min 1456 | 1500 m |
| Celtica | 1497 / min 1497 | 1515 / min 1496 | 1500 m |
| Baltico | 1429 / min 1429 | 1485 / min 1445 | 50 m |

## 4. Mappe originali ↔ teatri reali

### 4.1 Associazione certa (dal codice)

Nei briefing `SCENE*.LZA` la pagina 20 del dialogo contiene i token `0B 00 <i> <v>`, che
riempiono il blocco di configurazione della missione (`DS:B45A`, letto da `FUN_1000_1e11`). Il
byte 14 è l'indice della mappa: `GAME.EXE` carica `SURF('A' + indice).LZW`. L'indice è identico per
tutte le scene dello stesso teatro:

| Mappa | Indice | Teatro | Missioni |
|---|---|---|---|
| SURFA | 0 | Otranto | 6 |
| SURFB | 1 | Baltico | 10 |
| SURFC | 2 | Barents | 3 |
| SURFD | 3 | Gibilterra | 4, 5 |
| SURFE | 4 | Kunashir | 7 |
| SURFF | 5 | Celtica | 8, 9 |
| SURFG | 6 | Faroe | 1, 2 |

Il caricamento è diviso per dischetto: A, B, C e G stanno su DISK1, D, E e F su DISK2.

### 4.2 Verifica statistica indipendente

Prima di trovare la tabella nel codice ho cercato le mappe nei dati ETOPO senza saperne il teatro.
La correlazione di rango (Spearman, invariante a qualsiasi legge monotona livello→profondità) su
più scale, posizioni e orientamenti ha dato:
- `SURFD` = Gibilterra con ρ = 0,97 e margine netto (0,13) sul secondo teatro;
- `SURFA` = Otranto e `SURFE` = Kunashir come migliori assegnazioni.

Le altre mappe, con gradienti morbidi, si adattano a quasi ogni teatro a scala piccola: senza
la tabella del codice l'associazione non sarebbe stata dimostrabile.

### 4.3 Georeferenziazione (`georef_surf.py`)

Con il teatro noto si cerca posizione, scala, orientamento e aspetto del pixel di ogni mappa
nel suo teatro. Risultato in `data/teatri/associazione_surf.json` e verifica visiva in
`extracted/georef_surf_verifica.png`.

Metodo: ricerca grossolana con modello 32×20 su 22 scale (da circa 20 a 250 miglia di
altezza), 2 aspetti del pixel e 4 orientamenti, con il centro entro mezzo grado dal riquadro del
teatro. Poi affinamento a 64×40 attorno ai migliori candidati. Durata circa 14 minuti.

| Mappa | Teatro | ρ | Area (nm) | Riquadro lat / lon | Affidabilità della posizione |
|---|---|---|---|---|---|
| SURFD | Gibilterra | 0,963 | 96 × 127 | 34,97–36,56 N; 7,15–4,54 W | **alta**: stretto, canale e bacini coincidono |
| SURFA | Otranto | 0,907 | 75 × 119 | 39,81–41,06 N; 17,36–19,99 E | media |
| SURFE | Kunashir | 0,750 | 85 × 112 | 43,17–44,59 N; 144,01–146,63 E | media |
| SURFC | Barents | 0,941 | 20 × 26 | — | bassa (scala minima) |
| SURFF | Celtica | 0,865 | 20 × 26 | — | bassa (scala minima) |
| SURFB | Baltico | 0,805 | 22 × 36 | — | bassa (scala minima) |
| SURFG | Faroe | 0,690 | 46 × 61 | — | bassa |

Conclusioni:
- l'orientamento migliore è "diritta" per tutte e sette: l'estrazione con le righe dal basso verso
  l'alto è corretta e le mappe non sono speculari;
- le mappe con affidabilità bassa sono probabilmente stilizzate dai grafici del 1989 (per esempio le
  bande parallele di SURFG). Per il remake l'area di gioco di quei teatri si colloca partendo dalle
  descrizioni del manuale e dei briefing, non dalla mappa originale;
- le nuove mappe del remake verranno comunque costruite dai dati reali (ETOPO, EMODnet, GEBCO);
  le originali servono come riferimento di stile e di area di gioco.

**Ricollocazione di SURFG e SURFB (23/10/2026, `tools/re/ricolloca_surf.py`).** Per le missioni si confrontano
le sole terre emerse, cioè il livello 15 della carta contro la terra di ETOPO, con traslazione e scala e senza
rotazione. Poi si affina sulle unità navali delle missioni.

| Mappa | Jaccard delle terre | Centro | Scala | Area | Riscontro |
|---|---|---|---|---|---|
| SURFG | 0,32 (0,35 con le sole coste) | 61,92 N, 6,90 W | 23,3 m per unità (×0,63) | 129 × 81 nm | l'arcipelago coincide; TORPEX a sud di Suðuroy |
| SURFB | 0,67 (0,70) | 54,91 N, 15,08 E | 45,9 m per unità (×1,24) | 254 × 159 nm | Bornholm, Skåne, Pomerania, Selandia; fuga verso il Kattegat |

Il confronto visivo è in `re/surf/SURF*_ricollocata.png`. Per la missione 10 la heightmap del Baltico è
stata allargata a 53,9–56,4 N e 10,5–16,2 E, con centro della proiezione a 55,15 N e 13,35 E. La
collocazione di SURFB è calcolata su questa versione.

## 5. Blocco di configurazione della missione

Gli altri byte impostati dalla pagina 20 dei briefing (`0B 00 i v`) descrivono la missione.
Esempio (SCENE1A, TORPEX '89, lato 688): `[1]=0x16 [5]=2 [6]=6 [11]=5 [12]=5 [13]=4 [14]=6`.
Il significato dei byte diversi dal 14 è ancora da decodificare: probabilmente dotazione di armi
e parametri d'ambiente.
