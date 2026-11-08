# C1. L'equazione del sonar

*Autore: disassemblatore. Revisione del 28/10/2026 (schiera sovietica). Stato: rivisto (curatore: kit).*

## 1. A che serve

Decide chi sente chi. Ogni volta che un'unità ascolta un'altra, il gioco confronta il rumore che arriva con il rumore
di fondo e con la capacità del sensore. Il margine che resta è l'**eccesso di segnale** (SE). Con un SE positivo il
contatto nasce e la traccia cresce; con un SE negativo il contatto si indebolisce e si perde. Il modello è uguale per
il giocatore e per l'IA, e per i sottomarini e per le navi. È il cuore della caccia: spiega perché un 688 lento non lo
sente nessuno, perché un'Alfa a 20 nodi si sente a cento miglia, e perché verso poppa si è sordi.

## 2. Il modello

Il codice è in `nucleo/acustica.hpp` e `nucleo/src/acustica.cpp` (`valuta`); la missione lo chiama in
`missioni/src/missione.cpp` (`Missione::ascolta_con_acustica`). Tutti i livelli sono in dB: le sorgenti e i rumori
re 1 µPa a 1 m, spettrali (per Hz) alla frequenza della banda.

### 2.1 Passivo

```
SE = SL − TL − (NL − DI) − DT − penalità
```

- `SL`: livello della sorgente alla frequenza della banda (capitolo C4).
- `TL`: perdita di trasmissione fra sorgente e ricevitore (capitolo C2).
- `NL`: rumore di fondo nel punto del ricevitore. È la somma in potenza del rumore ambiente (capitolo C3) e del
  rumore proprio del battello che ascolta:

  ```
  NL_proprio = max(NL_macchina, NL_flusso + 60 log(V / V₀)) − 20 log(max(f, 100) / 1000) [+ baffles] [+ 30 se sordo]
  ```

  `V` è la velocità propria in nodi; `V₀` e i due livelli sono del sensore (tabella in fondo). Sopra 1 kHz il rumore
  proprio scende di 20 dB per decade. Nel mare 5 e oltre si aggiunge il mare sui sensori vicino alla superficie
  (capitolo C3).
- `DI`: guadagno d'antenna, dalla dimensione e dalla frequenza:

  ```
  sfera e scafo:  DI = 20 log(π D / λ)
  schiera:        DI = 10 log(2 L / λ)          λ = 1500 / f
  ```

  Nel cono d'ombra poppiero (i **baffles**) il DI vale 0 e il rumore proprio sale di `baffles_db` (15 dB per il
  BQQ-5): lo scafo e l'elica schermano i fasci.
- `DT`: la soglia, cioè il rapporto segnale/rumore che serve nella banda:

  ```
  DT = 5 log(d / (W t)) − abilità       W = 0,7 f (un'ottava),  t = integrazione del sensore
  ```

  `d` è l'indice di rivelazione del sensore (14 per i sonar principali: probabilità di falso allarme 10⁻⁴ e di
  scoperta 0,5). `abilità` è l'abilità dell'operatore, ±3 dB.
- `penalità`: 30 dB se la schiera è cieca, cioè in virata oltre il suo limite o con la sorgente nell'endfire
  anteriore. L'endfire sono le due direzioni lungo l'asse della schiera; quello anteriore guarda verso il battello.
  Dopo una virata la penalità è di 10 dB, che calano a zero nel tempo di raddrizzamento.

Il sensore prova tutte le sue bande (`bande_hz`) e tiene la migliore.

### 2.2 LOFAR: le righe

I sensori con `lofar_bin_hz` ascoltano anche le righe tonali della firma: la rete di bordo, le pompe, le turbine, le
pale (capitolo C4). Una riga sta in un bin stretto `b` e si integra per un tempo lungo `T`:

```
SE_riga = L_riga − TL − (NL + 10 log b − DI) − DT_riga
DT_riga = 5 log(d / (b T)) − abilità
```

- TB-16: b = 0,05 Hz, T = 120 s; sfera BQQ-5: 0,1 Hz e 60 s; sovietici di terza generazione: 0,2 Hz e 60 s.
- Se una riga dà più della banda larga, la scoperta viene da quella (`Rilevazione::lofar`).
- Il miglior SE delle righe resta comunque in `se_tonale_db`: con +3 dB le righe si leggono, ed è l'indizio per la
  classificazione (capitolo C5).

### 2.3 Attivo

```
SE = SL − 2 TL + TS − max(NL_banda − DI, RL) − DT
NL_banda = NL + 10 log(1 / τ)          DT = 10 log(d / 2) − abilità
```

- `SL`: livello del ping del sensore (`sl_attivo_db`), `τ` la durata dell'impulso.
- `TS`: forza di bersaglio dall'aspetto: `TS = TS_prua + (TS_traverso − TS_prua) |sin φ|³`, più un lobo di +4 dB a
  ±7° dal traverso.
- `RL`: la **riverberazione**, somma in potenza di tre parti:
  - volume: `SL − 2 TL − 80 + 10 log(r² ψ c τ / 2)`, con ψ dal DI;
  - superficie: la forza di diffusione di Chapman e Harris col vento, sull'anello che torna insieme all'eco;
  - fondo: legge di Lambert, −27 dB.
  Un bersaglio con più di 2,5 nodi di velocità radiale esce dalla riverberazione di 10 dB (il Doppler). Un battello
  posato sul fondo si confonde con la riverberazione del fondo.

### 2.4 Il ping intercettato

Chi ascolta con un intercettatore (WLR-9 sul 688, gli intercettatori sovietici) sente il ping degli altri molto più
lontano di quanto il ping senta lui: la strada è una sola, non andata e ritorno.

```
SE_ping = SL_ping − TL − (NL + 10 log B − DI) − 5 log(d / (B τ))      B = 0,7 f
```

È un rivelatore d'energia sulla durata dell'impulso, nella banda di un'ottava.

### 2.5 Probabilità e fluttuazione

- La probabilità di scoperta per un SE dato è `P = Φ(SE / σ)`, la normale cumulata (`probabilita_rilevazione`).
- Il segnale non è fermo: per ogni coppia (chi ascolta, chi è ascoltato) c'è una **fluttuazione** lenta, un processo
  AR(1) con tempo di correlazione di 5 minuti e σ = 4 dB (`Fluttuazione`; costanti `TAU_PASSIVO_S`, `SIGMA_DB` in
  `missione.cpp`). Così un contatto al limite appare e scompare per minuti, non a ogni passo.

| SE (dB) | −4 | 0 | +4 | +8 |
|---|---|---|---|---|
| P con σ = 4 dB | 0,16 | 0,50 | 0,84 | 0,98 |

### 2.6 Dal segnale al contatto

Nella missione (`Missione::ascolta_con_acustica`):

1. la cadenza è quella dell'originale: un osservatore per passo, a rotazione (capitolo A1);
2. per l'osservatore si prende il sensore migliore della sua suite (la schiera solo se filata);
3. al SE si somma la fluttuazione della coppia;
4. la qualità della traccia integra il SE nel tempo: `qualità += 0,55 · clamp(SE, −10, 20) · dt` (al più 1500).
   Con +10 dB stabili la traccia diventa solida (qualità 800) in circa due minuti e mezzo;
5. con SE negativo la qualità scende; a zero il contatto si perde;
6. il rilevamento misurato ha l'errore del sensore (capitolo C5); la schiera dà anche il rilevamento a specchio.

Le armi proprie, il radar e la vista contano come +30 dB, con la posizione; l'intercettazione ESM come +5 dB, solo
il rilevamento.

### 2.7 Un esempio

Fær Øer a febbraio, vento 7 m/s. Il 688 ascolta a 150 m e 5 nodi; un Victor III a 5 nodi e 150 m al traverso. I numeri
vengono dal motore (`tools/manuale/esempi_acustica.cpp`).

| Distanza | Sensore | Banda | SL | TL | NL | DI | DT | SE |
|---|---|---|---|---|---|---|---|---|
| 3 km | TB-16 | riga a 250 Hz | 118,0 | 64,7 | 65,8 | 13,9 | 1,8 | **+12,6** |
| 3 km | sfera BQQ-5 | 1 kHz | 103,3 | 64,9 | 64,1 | 19,7 | −13,5 | **+7,6** |
| 10 km | TB-16 | riga a 250 Hz | 118,0 | 73,7 | 65,8 | 13,9 | 1,8 | **+3,6** |
| 10 km | sfera BQQ-5 | 1 kHz | 103,3 | 74,3 | 64,1 | 19,7 | −13,5 | **−1,9** |
| 20 km | TB-16 | riga a 250 Hz | 118,0 | 80,3 | 65,8 | 13,9 | 1,8 | **−3,0** |

- A 10 km la schiera sente il Victor dalla riga della rete di bordo; la sfera no.
- Oltre i 3 km il suono viaggia nello strato superficiale (il condotto, capitolo C2).
- Lo stesso Victor pingato dal BQQ-5 attivo a 10 km dà SE +4,4 dB, e il Victor sente il ping da molto più lontano.

Portate al traverso, stesso ambiente, TB-16 (fra parentesi la sfera):

| Bersaglio | Portata |
|---|---|
| Los Angeles a 5 kn | 6,1 km (1,8) |
| Akula a 5 kn | 4,2 km (1,8) |
| Kilo a 3 kn | 4,1 km (2,5) |
| Victor III a 5 kn | 13,6 km (8,1) |
| Akula a 15 kn | 26,1 km (15,0) |
| Charlie II a 5 kn | 70 km (33) |
| Alfa a 20 kn | 175 km (135) |

Questi esempi usano il livello A, senza raggi: non dipendono dalla griglia, che nella missione arriva a 300 km (capitolo C2 §7).

### 2.8 I sensori

<!-- generato: sensori_sonar -->
| Sensore | Modo | Antenna (m) | Bande (Hz) | Integrazione (s), d | Rumore proprio: macchina / flusso a V₀ (dB, kn) | Velocità max (kn) | Settore cieco | LOFAR: bin (Hz), T (s) |
|---|---|---|---|---|---|---|---|---|
| BQQ-5, sfera (passivo) | passivo | sfera 4,6 | 150, 300, 1000 | 10, 14 | 45 / 50 a 8 | 25 | baffles ±35°, +15 dB | 0,10, 60 |
| TB-16 (schiera) | passivo | lineare 73,0 | 50, 150, 300, 1000 | 30, 14 | 35 / 45 a 12 | 17 | endfire ±25°; virata oltre 0,3 °/s | 0,05, 120 |
| BQQ-5, sfera (attivo) | attivo, SL 225 dB, τ 0,5 s | sfera 4,6 | 3500 | —, 14 | 45 / 50 a 8 | 25 | baffles ±35° | — |
| WLR-9 (intercettatore) | passivo | scafo 0,3 | 3500, 10000, 25000 | 1, 14 | 50 / 52 a 8 | 30 | — | — |
| MGK-400/503 (terza generazione) | passivo | sfera 4,0 | 150, 300, 1000 | 10, 14 | 50 / 52 a 8 | 25 | baffles ±40°, +15 dB | 0,20, 60 |
| attivo dei sottomarini sovietici | attivo, SL 222 dB, τ 0,5 s | sfera 4,0 | 3500 | —, 14 | 50 / 52 a 8 | 25 | baffles ±40° | — |
| intercettatore sovietico (Enisej) | passivo | scafo 0,3 | 3500, 10000, 25000 | 1, 14 | 55 / 55 a 8 | 30 | — | — |
| schiera sovietica (Skat, Pelamida) | passivo | lineare 80,0 | 50, 150, 300 | 10, 100 | 50 / 58 a 10 | 14 | endfire ±25°; virata oltre 0,3 °/s | 0,10, 120 |
| MG-200 Kerch (seconda generazione) | passivo | sfera 3,0 | 300, 1000 | 10, 14 | 56 / 55 a 7 | 22 | baffles ±45°, +15 dB | 1,00, 30 |
| csu83 | passivo | sfera 1,6 | 300, 1000, 3500, 10000 | 10, 14 | 45 / 50 a 8 | 18 | baffles ±40°, +15 dB | 0,20, 60 |
| csu83_attivo | attivo, SL 215 dB, τ 0,1 s | sfera 1,6 | 10000 | —, 14 | 45 / 50 a 8 | 18 | baffles ±40° | — |
| fas3 | passivo | scafo 12,0 | 1000, 3500 | 20, 14 | 45 / 50 a 8 | 15 | baffles ±30°, +10 dB | 0,10, 60 |
| SQS-53 (passivo) | passivo | sfera 5,0 | 1000, 3500 | 10, 14 | 60 / 56 a 10 | 30 | baffles ±30°, +15 dB | 1,00, 30 |
| SQS-53 (attivo) | attivo, SL 235 dB, τ 1,0 s | sfera 5,0 | 3500 | —, 14 | 60 / 56 a 10 | 30 | baffles ±30° | — |
| sqr19 | passivo | lineare 242,0 | 50, 150, 300, 1000 | 30, 14 | 40 / 48 a 12 | 25 | endfire ±25°; virata oltre 0,3 °/s | 0,05, 120 |
| type2016 | passivo | sfera 4,0 | 1000, 3500 | 10, 14 | 60 / 56 a 10 | 30 | baffles ±30°, +15 dB | 1,00, 30 |
| type2016_attivo | attivo, SL 228 dB, τ 0,5 s | sfera 4,0 | 10000 | —, 14 | 60 / 56 a 10 | 30 | baffles ±30° | — |
| type2031z | passivo | lineare 500,0 | 50, 150, 300 | 30, 14 | 46 / 50 a 12 | 20 | endfire ±25°; virata oltre 0,3 °/s | 0,05, 120 |
| sqs35_vds | passivo | sfera 4,8 | 3500, 10000 | 10, 14 | 50 / 52 a 10 | 20 | — | — |
| sqs35_vds_attivo | attivo, SL 220 dB, τ 0,5 s | sfera 4,8 | 10000 | —, 14 | 50 / 52 a 10 | 20 | — | — |
| sqr18a | passivo | lineare 200,0 | 50, 150, 300, 1000 | 30, 14 | 42 / 50 a 12 | 20 | endfire ±25°; virata oltre 0,3 °/s | 0,10, 120 |
| polinom | passivo | sfera 7,0 | 300, 1000, 3500 | 10, 14 | 64 / 58 a 10 | 30 | baffles ±30°, +15 dB | 0,50, 60 |
| polinom_attivo | attivo, SL 232 dB, τ 1,0 s | sfera 7,0 | 3500 | —, 14 | 64 / 58 a 10 | 30 | baffles ±30° | — |
| polinom_vds | passivo | sfera 4,0 | 1000, 3500 | 10, 14 | 54 / 54 a 10 | 20 | — | 0,50, 60 |
| polinom_vds_attivo | attivo, SL 226 dB, τ 1,0 s | sfera 4,0 | 3500 | —, 14 | 54 / 54 a 10 | 20 | — | — |
| sonar di prua delle navi sovietiche | passivo | sfera 4,0 | 1000, 3500 | 10, 14 | 64 / 58 a 10 | 30 | baffles ±30°, +15 dB | 1,00, 30 |
| MG-332 Titan-2 (attivo) | attivo, SL 228 dB, τ 0,5 s | sfera 4,0 | 3500 | —, 14 | 62 / 58 a 10 | 30 | baffles ±30° | — |
| shelon_vds | passivo | sfera 1,5 | 3500, 10000 | 10, 14 | 50 / 55 a 3 | 3 | — | — |
| shelon_vds_attivo | attivo, SL 215 dB, τ 0,1 s | sfera 1,5 | 10000 | —, 14 | 50 / 55 a 3 | 3 | — | — |
| SQS-56 (attivo) | attivo, SL 222 dB, τ 0,5 s | sfera 2,5 | 10000 | —, 14 | 60 / 56 a 10 | 30 | baffles ±30° | — |
| vega_vds | passivo | sfera 2,5 | 3500, 10000 | 10, 14 | 54 / 55 a 10 | 18 | — | — |
| vega_vds_attivo | attivo, SL 222 dB, τ 0,5 s | sfera 2,5 | 3500 | —, 14 | 54 / 55 a 10 | 18 | — | — |
| sqs36 | passivo | sfera 3,0 | 3500, 10000 | 10, 14 | 60 / 56 a 10 | 25 | baffles ±30°, +15 dB | — |
| sqs36_attivo | attivo, SL 218 dB, τ 0,5 s | sfera 3,0 | 10000 | —, 14 | 60 / 56 a 10 | 25 | baffles ±30° | — |
| sa950 | passivo | sfera 1,5 | 10000, 25000 | 10, 14 | 60 / 58 a 10 | 25 | baffles ±30°, +15 dB | — |
| sa950_attivo | attivo, SL 212 dB, τ 0,2 s | sfera 1,5 | 25000 | —, 14 | 60 / 58 a 10 | 25 | baffles ±30° | — |
| salmon_vds | passivo | sfera 1,5 | 3500, 10000 | 10, 14 | 50 / 54 a 10 | 18 | — | — |
| salmon_vds_attivo | attivo, SL 215 dB, τ 0,3 s | sfera 1,5 | 10000 | —, 14 | 50 / 54 a 10 | 18 | — | — |
<!-- fine: sensori_sonar -->

**La schiera sovietica** (Skat-KS del Victor III, Pelamida dell'Akula; 28/10/2026, disassemblatore): 200 m di
schiera ma 80 di apertura efficace, 10 s d'integrazione con l'indice di rilevazione 100, e un rumore proprio di 50 / 58
dB, più alto di quello della TB-16. È tutta una **STIMA** (dossier 04 §3 e §6.2), tarata perché il vantaggio di
scoperta fra le classi del dossier 24 §5.1 resti entro 4 dB. La prima versione aveva 200 m d'apertura, 30 s, indice 14 e
38 / 48 dB. L'isolamento delle righe del proprio battello per ogni sensore è nel capitolo C5
§2.9.

Quali sensori ha ogni classe:

<!-- generato: suite_sonar -->
| Classe | Sensori |
|---|---|
| Los Angeles (SSN-688, Flight I) | BQQ-5, sfera (passivo), TB-16 (schiera), BQQ-5, sfera (attivo), WLR-9 (intercettatore) |
| Sturgeon (SSN-637) | BQQ-5, sfera (passivo), TB-16 (schiera), BQQ-5, sfera (attivo), WLR-9 (intercettatore) |
| Trafalgar (Royal Navy) | BQQ-5, sfera (passivo), TB-16 (schiera), BQQ-5, sfera (attivo), WLR-9 (intercettatore) |
| Victor III (Progetto 671RTM) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej), schiera sovietica (Skat, Pelamida) |
| Akula I (Progetto 971) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej), schiera sovietica (Skat, Pelamida) |
| Oscar II (Progetto 949A) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej), schiera sovietica (Skat, Pelamida) |
| Typhoon (Progetto 941) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej), schiera sovietica (Skat, Pelamida) |
| Kilo (Progetto 877) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Alfa (Progetto 705) | MG-200 Kerch (seconda generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Charlie II (Progetto 670M) | MG-200 Kerch (seconda generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Foxtrot (Progetto 641) | MG-200 Kerch (seconda generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Victor I (Progetto 671) | MG-200 Kerch (seconda generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Delta III (Progetto 667BDR) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Sierra I (Progetto 945) | MGK-400/503 (terza generazione), attivo dei sottomarini sovietici, intercettatore sovietico (Enisej) |
| Ohio (SSBN-726) | BQQ-5, sfera (passivo), TB-16 (schiera), BQQ-5, sfera (attivo), WLR-9 (intercettatore) |
| Type 206 (U 13-30) | csu83, csu83_attivo, fas3, WLR-9 (intercettatore) |
| Ticonderoga (CG-47) | SQS-53 (passivo), SQS-53 (attivo), sqr19 |
| Type 22 Broadsword (Batch 1) | type2016, type2016_attivo |
| Type 22 Boxer (Batch 2) | type2016, type2016_attivo, type2031z |
| Knox (FF-1052) | SQS-53 (passivo), SQS-53 (attivo), sqs35_vds, sqs35_vds_attivo, sqr18a |
| Udaloy (Progetto 1155) | polinom, polinom_attivo, polinom_vds, polinom_vds_attivo |
| Grisha III (Progetto 1124M) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo), shelon_vds, shelon_vds_attivo |
| Oliver Hazard Perry (FFG-7) | SQS-53 (passivo), SQS-56 (attivo), sqr19 |
| Kidd (DDG-993) | SQS-53 (passivo), SQS-53 (attivo) |
| Spruance (DD-963) | SQS-53 (passivo), SQS-53 (attivo), sqr19 |
| Krivak II (Progetto 1135M) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo), vega_vds, vega_vds_attivo |
| Kara (Progetto 1134B) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo), vega_vds, vega_vds_attivo |
| Slava (Progetto 1164) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo) |
| Kirov (Progetto 1144) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo) |
| Koni (Progetto 1159) | sonar di prua delle navi sovietiche, MG-332 Titan-2 (attivo) |
| Nanuchka I (Progetto 1234) | sonar di prua delle navi sovietiche |
| Ugra (Progetto 1886) | sonar di prua delle navi sovietiche |
| Nimitz (CVN-68) |  |
| Kitty Hawk (CV-63) |  |
| Iowa (BB-61) |  |
| Sacramento (AOE-1) |  |
| Hauk (motocannoniera norvegese) |  |
| Portacontainer (Cpl. Louis J. Hauge Jr., ex Estelle Maersk) |  |
| Mercantile (Ravenstvo, tipo Leninskiy Komsomol) |  |
| Petroliera (Sea Isle City, 1981) |  |
| Peschereccio d'altura (Atlantic Peace, 1987) |  |
| Portarinfuse |  |
| Petroliera costiera e chimichiera |  |
| Traghetto ro-ro e passeggeri |  |
| Rimorchiatore d'altura |  |
| Rompighiaccio diesel-elettrico |  |
| Nave oceanografica |  |
| Grande peschereccio sovietico (BMRT) |  |
| Peschereccio costiero (palangaro, reti da posta) |  |
| Cianciolo |  |
| BQM-74C Chukar (bersaglio) |  |
| Oslo (F300) | sqs36, sqs36_attivo |
| Stockholm (K11) | sa950, sa950_attivo, salmon_vds, salmon_vds_attivo |
| Nacken (A14) | csu83, csu83_attivo, fas3, WLR-9 (intercettatore) |
<!-- fine: suite_sonar -->

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Forma dell'equazione, passivo e attivo | dossier 02 §1.1 [Urick] | alta |
| DT del rivelatore d'energia, `5 log(d/(W t))`, d = 14 | dossier 02 §1.2 [Urick] | alta (forma); **STIMA** il valore di d |
| DI di sfera e schiera | dossier 02 §1.2 | alta (forma); media le dimensioni |
| Rumore proprio `60 log(V/V₀)`, livelli di macchina e flusso | dossier 02 §2.5 | **STIMA**: tarati perché un 688 a 5 nodi senta un Victor III a circa 10 km |
| Baffles: 15 dB e DI nullo | dossier 02 §2.5 | **STIMA** |
| Bin e tempi del LOFAR | dossier 02 §1.2, §2.7 | media |
| Riverberazione di volume −80 dB, fondo −27 dB | dossier 02 §1.3 [Mackenzie 1961] | media |
| Superficie: Chapman e Harris | dossier 02 §1.3 [Chapman-Harris 1962] | alta |
| Forza di bersaglio per aspetto, lobo di +4 dB | dossier 02 §1.4 | media |
| Fluttuazione: 5 minuti, 4 dB | dossier 02 §1.6 (2-10 minuti, 3-6 dB) | media |
| Qualità della traccia, 0,55 per dB e per secondo | scelta di gioco | **STIMA**: due minuti e mezzo per una traccia solida a +10 dB |
| Ping intercettato | dossier 24 §5.1 | alta (forma) |

Tutti i valori di taratura dei sensori sono STIMA DI GIOCO (dossier 02 §10.6): le prestazioni vere sono classificate.

## 4. Classico ed Enhanced

Nel classico la scoperta è la formula additiva dell'originale (`docs/originale/sensori_e_navigazione.md` §3.3):

```
T = G + S · (Nb + 40 − ⌊32 v_oss / v_tutta⌋) + E
```

- `G` è il guadagno del settore di prua (±30°, ±90° con la schiera), `S` il sonar dell'osservatore, `Nb` il rumore
  del bersaglio con la sua velocità, `E` gli eventi acustici.
- Il contatto c'è se T supera la distanza per asse, al più 20 nm. Ogni strato fra le quote dimezza G e divide per 4
  il rumore.
- Un osservatore fermo sente quasi tutto nel settore di prua fino a 10-17 nm.

L'equazione del sonar vale solo nell'Enhanced, con la regola di simulazione. La cadenza (chi osserva a ogni passo)
resta quella del classico, perché gli script e l'IA dipendono da quei tempi.

## 5. Come si vede in gioco

- **Stazione sonar** (volume 1, cap. 3): i contatti nascono, lampeggiano finché il SE è negativo e diventano solidi
  con la qualità.
- **Messaggi**: 310 nuovo contatto, 311 siluro in acqua, 312 ci stanno pingando, 313 stiamo cavitando, 314 schiera
  cieca in virata; 340 e 341 classificato sottomarino o nave, 344 il contatto cavita, 345 transitorio, 346 biologico,
  347 nuova riga al LOFAR.
- **Velocità propria**: andare piano è il modo di sentire. Oltre la velocità massima di un sensore (TB-16 17 nodi,
  BQQ-5 25) il sensore è sordo (+30 dB di rumore proprio).
- **Baffles**: un contatto a poppa scompare. Si accosta per guardarsi dietro (volume 1, cap. 3).

## 6. Taratura e prove

- **Ordine di grandezza** (dossier 02 §10.6): alle Fær Øer d'inverno, con la TB-16 a 5 nodi, un 688 o un Akula
  silenziosi si sentono a 4-6 km, un Victor III a 13, un Charlie II a 70.
- **Intercettazione del ping**: l'impulso del BQS-15 si intercetta a 9,4 km nel fascio e a 6,7 km fuori (dossier
  24 §5.1). Prima di una correzione era sovrastimato di 32 dB.
- **Vantaggio della coppia** (dossier 24 §5.1): le differenze di scoperta fra le classi del 1989 escono dal motore
  entro 4 dB dal dossier, senza ritocchi.

Prove in `test/prove_acustica.cpp`:
- «equazione del sonar, portate relative, attivo e probabilita'»;
- «fluttuazione AR(1) stazionaria con la sigma data»;
- «schiera trainata cieca in virata e nell'endfire, errore del rilevamento»;
- «intercettazione di un impulso attivo (contro-scoperta)»;
- «LOFAR sulle tonali»;
- «l'impulso del BQS-15 all'intercettazione»;
- «il vantaggio di scoperta fra le classi del 1989 esce dal motore».

Poi `test/prove_acustica_missione.cpp` (la scoperta nella missione) e `test/prove_acustica_asw.cpp` (l'asimmetria
fra 688 e Alfa, almeno 5 volte).

## 7. Limiti noti e in arrivo

- Un solo sensore per volta dà la scoperta, il migliore: i segnali di sensori diversi non si sommano.
- Il DT non cambia con la probabilità di falso allarme scelta: è fisso per sensore.
- I falsi contatti nascono dal paesaggio sonoro e dal sonarista (capitolo C5), non da un rumore casuale sopra soglia.
- Il paesaggio sonoro (biologici, pesca, traffico lontano) esiste solo nel disco di 100 km attorno al giocatore. Gli
  ascoltatori dell'IA dentro il disco sentono le stesse istanze, ogni 10 s ed entro 30 km, e le passano al loro
  sonarista, che può sbagliarle (fase 2 delle interazioni fra IA; `Missione::passo_paesaggio`). Fuori dal disco l'IA
  sente solo le unità: il mare lontano dal giocatore è più vuoto di quello vicino.

## 8. Riferimenti

- Codice: `nucleo/acustica.hpp`, `nucleo/src/acustica.cpp` (`valuta`, `soglia`, `indice_direttivita`,
  `rumore_proprio`, `probabilita_rilevazione`, `Fluttuazione`); `missioni/src/missione.cpp`
  (`ascolta_con_acustica`).
- Dati: `data/motore/acustica.toml` `[sensori.*]` e `[suite]` (da `tools/pipeline/acustica.py`).
- Esempi: `tools/manuale/esempi_acustica.cpp`.
- Ricerca: dossier 02 §1, §2.5, §10; dossier 24 §5.
- Originale: `docs/originale/sensori_e_navigazione.md` §3.
