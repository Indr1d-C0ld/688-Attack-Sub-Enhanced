# 35 — Le classi delle missioni S16-S18: Oslo, Hauk, Stockholm, Hkp 4, Kitty Hawk, Näcken

Disassemblatore, 30/10/2026. Il kit le modella; la principale ha chiesto l'acustica. Queste voci sono in
`tools/pipeline/acustica.py`, l'Hkp 4 anche in `asw_aereo.py` e `sorgenti_asw.py`.

**Le fonti.**
- Solo pagine aperte, senza file scaricati.
- Confidenza: **alta** quando le fonti concordano, **media** con una sola fonte, **bassa** quando è debole o in disaccordo.
- Le scelte di gioco sono **STIMA**.
- Le reti di bordo nordiche non si sono trovate. Nel gioco sono a 60 Hz come le altre NATO, perché a 50 Hz l'analizzatore
  le leggerebbe sovietiche: è la scelta della Type 206 (dossier 32 §5).

## 1. Le classi

| Classe | Scafo e macchine | Sonar | Fonte | Confidenza |
|---|---|---|---|---|
| Oslo (fregata norvegese) | 1.760 t, 96,6 m; turbina a vapore, 2 caldaie, 1 asse, 20.000 shp, 25 nodi | SQS-36 di scafo e Terne III; dal 1987-91 il TSM 2633 di scafo e VDS (la Oslo nel febbraio 1991) | Navypedia «no_es_oslo»; Wikipedia, Oslo-class | media |
| Hauk (motocannoniera) | 148 t, 36,5 m; due diesel veloci, 34 nodi (kit) | nessun sonar antisom | kit; trailer-1 | media |
| Stockholm (corvetta svedese, 1985) | 372 t, 50 m; CODAG con 2 MTU 16V396 e una Allison 570, 3 assi, 32 nodi | Simrad SA950 di scafo e VDS TSM 2643 «Salmon» | Navypedia «sw_cf_stockholm»; seaforces | media |
| Hkp 4 (Vertol 107 navale) | due rotori in tandem; cacciatorpediniere e basi a terra | sonar a immersione HS-73, portata 5,6 km; un radar di prua | aeroseum (vertolensvanner), avrosys | bassa |
| Kitty Hawk (portaerei convenzionale) | 81.780 t; 8 caldaie, turbine Westinghouse, 4 assi, 280.000 shp, 32 nodi | nessuno: l'America (CV-66) fu la sola con un sonar, l'SQS-23, tolto nei primi anni '80 | Wikipedia, Kitty Hawk-class | alta |
| Näcken (A14, sottomarino svedese) | 980/1.125 t, 49,5 m; un diesel MTU 16V652 e un motore elettrico, 1 asse; 20 nodi in immersione, 300 m | CSU-83 (DBQS-21 e FAS-3-1) | Navypedia «sw_ss_nacken» | media |

Il Näcken ha in più l'**AIP Stirling**: due V4-275, 200 hp, 14 giorni in immersione. Fu montata nel 1988-89 sul solo
Näcken (Navypedia; Wikipedia). Il motore non la modella: resta il diesel a snorkel.

## 2. Nel gioco (STIMA salvo dove detto)

| Classe | Firma | Righe | Suite |
|---|---|---|---|
| Oslo | 140/155/165 dB a 5/15/25 nodi, come la Knox più piccola | turbina 98 Hz, riduttore 196, pompe 29; 5 pale, 1 asse | `sqs36` e `sqs36_attivo` (13 kHz, la famiglia dell'SQS-35) |
| Hauk | 130/150/163 dB a 5/20/34 | diesel 30 e 150 Hz; 3 pale, 2 assi | nessuna, e `sonar = 0` nel modello della S16 |
| Stockholm | 135/150/163 dB a 5/15/32 | MTU 31,7 e 63,3 Hz, turbina 140; 4 pale, 3 assi | `sa950` di scafo e il VDS `salmon_vds` (cavo 150 m, fino a 120 m) |
| Kitty Hawk | 158/173/183 dB a 5/15/32, sopra la Nimitz (caldaie, ventilatori, pompe) | turbina 80, riduttore 160, pompe 32, ausiliario 48; 5 pale, 4 assi | nessuna: va messo `sonar = 0` nel modello dello scenario, se no il motore le dà l'SQS-53 di ripiego |
| Näcken | 108/114/125/132 dB a 4,5/8/15/20, come il 206 più grande; snorkel +22,5 | ausiliario 65, diesel 25, 12,5 e 75 (solo a snorkel); 5 pale | come il 206: i numeri del passivo di 2a generazione per il CSU-83 |

**L'Hkp 4:**
- il mezzo ASW `hkp4`: niente boe, il sonar `hs73`, due Tp 42 (§5; prima il Mk 46 al suo posto);
- il transitorio `elicottero_hkp4`: i due rotori a 3 pale, circa 13 Hz;
- il ping `ping_hs73` a circa 10 kHz;
- la prova: sul datum cala l'HS-73 e pinga, senza posare boe.

## 3. Cosa non si è trovato

- Le frequenze e i livelli di SQS-36, Terne, SA950, Salmon e HS-73.
- Le reti di bordo norvegesi e svedesi.
- Il rumore irradiato di tutte le classi.
- Quali Oslo avessero già il TSM 2633 nel 1989.
- Le prestazioni del Tp 42 e i pesi delle bombe svedesi (§5).

## 4. Il CSU-83 / DBQS-21 della Type 206 e del Näcken

Fonti: Military Periscope, CSU-83 e DSQS-21 (anteprima): «primary active/passive classification and tracking sonar»
delle U-Boot tedesche. Il dettaglio viene da Watts 2000, citato in una pagina giapponese derivata da Wikipedia (Weblio):
**confidenza bassa-media**. Lo si provò sui 206 dal 1985 e lo si montò sui 206A con la conversione.

| Parte | Dato | Nel gioco (STIMA dove non detto) |
|---|---|---|
| Cilindro di prua, passivo | 0,3-12 kHz | `csu83`: 1,6 m, bande 300-10.000 Hz, LOFAR a 0,2 Hz, il rumore proprio di un SSK a batteria |
| Attivo | 8 kHz, settori di 30° | `csu83_attivo`: nella banda dei 10 kHz, SL 215 |
| Schiera di fianco FAS 3-1 | 2-8 kHz, LOFAR e DEMON | `fas3`: un'antenna di scafo con 15 dB di guadagno dato (le «lineari» del motore sono quelle rimorchiate) |
| Intercettatore | 1-100 kHz | `wlr9`, l'intercettatore occidentale |

Prima la Type 206 e il Näcken usavano i numeri del passivo sovietico di seconda generazione.

## 5. Le armi svedesi

- **Il Tp 42:**
  - 400 mm, del 1975, prodotto dalla FFV (Jane's, in sintesi); il Tp 422 è la versione per gli elicotteri antisom, che lo
    sganciano senza paracadute (Military Periscope, anteprima);
  - velocità, portata, testata e acquisizione non sono pubblicate: nel gioco 30/20 nodi, 10/20 km, 45 kg, acquisizione a
    500/800 m, fino a 200 m, fondale minimo 15 m, affidabilità 0,8, spoletta a contatto, tutto STIMA;
  - la testa attiva a 40 kHz (STIMA), per l'intercettazione;
  - l'Hkp 4 ne porta due.
- **Le bombe dell'Hkp 4** (la «Sjunkbomb 51», fino a 6, «deboli»: aeroseum, e un forum che cita cmano-db):
  - nel gioco dal 30/10/2026, col runtime: `sjunkbomb51` in missione_asw.cpp, accanto alle PLAB, con 25 kg di carica (STIMA);
  - il secondo carico dell'Hkp 4, «bombe», con 6 bombe; il predefinito resta coi 2 Tp 42 (di una coppia, uno coi siluri e
    uno con le bombe);
  - la prova: col carico delle bombe l'Hkp 4 sul sottomarino localizzato sgancia le Sjunkbomb 51.
- **L'Elma della Stockholm** (lanciagranate antisom, 4 da 9 tubi): resta «a dado» come gli RBU sovietici
  (missione_antisom_navi.cpp). Nel 1982 a Hårsfjärden la caccia lanciò bombe di profondità (almeno 18 secondo l'UPI, 47
  secondo una fonte di parte).

## 6. Le navi senza sonar

**Il problema** (trailer-1): nel classico la Nimitz ha `sonar = 4` nel modello, e nell'Enhanced una nave senza suite prende
l'SQS-53 di ripiego.

**La regola:**
- in acustica.toml le classi senza sonar vero hanno una **suite dichiarata vuota**: nimitz, kitty_hawk, iowa, sacramento,
  hauk, e i civili cargo, mercantile, petroliera, peschereccio;
- il ripiego resta per le classi senza voce, e per l'AGI, che ascoltava davvero.

Ha effetto con la riga del runtime in Missione::sensori_acustici. Il dato del classico non cambia.
