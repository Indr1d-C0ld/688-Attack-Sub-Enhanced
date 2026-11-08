# E1. Esplosioni

*Autore: kit. Revisione del 28/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Un'esplosione subacquea è l'evento più vistoso del gioco: la colonna d'acqua di un siluro sotto la chiglia, i
pennacchi delle bombe di profondità, la gobba d'acqua di una carica profonda. Questo capitolo spiega come le
dimensioni, i tempi e le forme di quello che si vede vengono dalla carica e dalla profondità, con le formule della
letteratura sulle esplosioni subacquee. I danni che l'esplosione fa sono nei capitoli B3 (sottomarini) ed E2 (navi);
i suoni nel capitolo C8.

## 2. Il modello

Il codice dei numeri è `nucleo/fenomeni.hpp` (`fenomeno_subacqueo`, `fenomeno_aereo`, fumo e fiamme); la messa in
scena è `grafica/effetti.hpp` (`Effetti::esplosione_subacquea`, `esplosione_aerea`), con particelle nel mondo 3D
ordinate per distanza insieme alle navi.

`W` è la carica in kg di TNT equivalente (massa per efficacia relativa, dossier 07 §1.5), `d` la profondità della
carica in metri.

### 2.1 La bolla

```
Z     = d + 10                      (profondità più l'atmosfera, in metri d'acqua)
A_max = 3,50 · W^(1/3) / Z^(1/3)    (raggio massimo della bolla, m)
T₁    = 2,11 · W^(1/3) / Z^(5/6)    (primo periodo, s; formula di Willis)
T₂    = 0,74 T₁,  T₃ = 0,63 T₁      (periodi successivi)
```

Il **regime** di quello che arriva in superficie dipende da `d / A_max`:

| d / A_max | Regime | Cosa si vede |
|---|---|---|
| sotto 1 | sfiato | la bolla sfonda subito: colonna cava, con la corona scura dei gas in cima |
| 1-1,5 | getto | un getto sottile e alto con una gonna di spruzzi alla base |
| 1,5-3 | pennacchi | la cupola, poi da 2 a 5 pennacchi bianchi inclinati all'impulso della bolla |
| 3-5 | cupola bassa | una cupola bassa, un pennacchio piccolo, la chiazza |
| oltre 5 | profonda | una piccola cupola o solo una gobba, poi la chiazza |

### 2.2 In superficie

- **Anello scuro** (slick): l'onda d'urto arriva in superficie e corre come un anello scuro,
  `r(t) = sqrt((1500 t)² − d²)`, fino a `R = 18 · W^(1/3)` m (dove l'urto in superficie scende sotto circa 2 MPa), in
  meno di 0,3 s.
- **Cupola di spruzzi** (Kolsky), per `0,23 < W^(1/3)/d`:
  ```
  v₀ = 86 · min(W^(1/3)/d, 3,1) − 6,6      (m/s)
  H  = 0,45 · v₀² / 2g
  ```
  alta al più metà del pennacchio, che la buca; il raggio è `max(1,25 d, A_max)`, cioè un diametro di circa 2-3
  volte la profondità. Sale all'apice in `2H / v₀`. Per le cariche sotto 5 kg senza pennacchio la cupola non supera
  `6 · W^(1/3)` m (**STIMA**).
- **Pennacchi** (tabella di Swisdak, `y = d / W^(1/4)`, fino a `y = 9` e per `W ≥ 5 kg`): altezza e raggio massimi
  per `W^(1/3)`, interpolati in tabella; escono all'impulso della bolla (`T₁`) e salgono in `0,8 · sqrt(2H/g)`.
  Oltre `y = 9`, o sotto i 5 kg, resta una gobba: `H = 0,8 · W^(1/3) · sqrt(9/y)` m, nulla sotto mezzo metro.
- **Forma dei getti** (`grafica/src/effetti.cpp`), per regime:
  - sfiato: una colonna cava (Milligan e Young) inclinata fino a 6°, con la corona di gas;
  - getto: un getto sottile inclinato fino a 5°, con la gonna di spruzzi alta un terzo;
  - pennacchi: da 2 a 5 getti che bucano la cupola, inclinati fino a 15°;
  - cupola bassa e profonda: un solo pennacchio largo e basso.
- **Ricaduta, base surge e nebbia**: l'acqua ricade; alla base si allarga l'anello di spruzzi (1,5 volte il raggio
  del pennacchio, nei regimi sfiato, getto e pennacchi); la nebbia resta sospesa per `4 + 0,15 · H` secondi.
- **Chiazza** di schiuma: raggio `max(4 · W^(1/3), 1,2 · R_pennacchio)`, che ribolle per `60 + 10 · W^(1/3)` s;
  **marrone di fango** e per `300 + 10 · W^(1/3)` s se la bolla tocca il fondo (`fondale − d < 2 A_max`).
- **Pulsazioni**: nei regimi a pennacchi e cupola bassa, sussulti successivi a `T₁ + T₂` e `T₁ + T₂ + T₃`, quante
  sono le oscillazioni della bolla prima della superficie (tabella di Swisdak, per `d / W^(1/3)`) meno una, al più
  due. Ogni sussulto è una cupola alta `max(H_cupola, 0,3 H_pennacchio) · 0,37^k`.

Il seme varia numero, forma e inclinazione dei getti, le altezze di ±20% e i raggi di ±15%: due esplosioni uguali non
si somigliano mai del tutto (le misure vere variano fino a ±50%).

**Esempi** (dalle formule, senza la variazione del seme):

| Carica | W (kg) | d (m) | A_max (m) | T₁ (s) | Regime | Cupola (m) | Pennacchio, altezza × raggio (m) | Anello (m) |
|---|---|---|---|---|---|---|---|---|
| Mk 48 sotto la chiglia | 544 | 15 | 9,8 | 1,18 | pennacchi | 37 | 153 × 104 | 147 |
| siluro da 533 mm a 5 m | 300 | 5 | 9,5 | 1,48 | sfiato | 116 | 233 × 60 | 120 |
| Mk 46 | 108 | 8 | 6,4 | 0,90 | getto | 46 | 94 × 61 | 86 |
| bomba di profondità a 15 m | 135 | 15 | 6,1 | 0,74 | pennacchi | 12 | 84 × 61 | 92 |
| bomba di profondità a 50 m | 135 | 50 | 4,6 | 0,36 | profonda | – | gobba di 3 m | 92 |
| mina Mk 67 a 30 m | 150 | 30 | 5,4 | 0,52 | profonda | – | 31 × 17 | 96 |

### 2.3 Nell'aria

Per la testata di un missile o lo scoppio di un deposito (`fenomeno_aereo`):

```
palla di fuoco    D = 3,5 · W^(1/3) m,   durata 0,2 + 0,08 · W^(1/3) s
schegge           v = 25 · W^(1/6) m/s   (deposito: 32 · W^(1/6))
nube              H = 10 · W^(1/3) m     (deposito: il fungo, 45 · W^(1/3))
lampo             clamp(W / 2000, 0,25, 1)
```

Il lampo schiarisce la palette per un fotogramma o due (`Palette::schiarita`). Le schegge sono balistiche (25 per un
missile, 80 per un deposito) e finiscono in mare con uno spruzzo. Esempio: i 165 kg di un Exocet fanno una palla di
19 m; un deposito di 10 t un fungo di quasi 1 km.

### 2.4 Fiamme e fumo

Gli incendi delle navi (capitolo E2) si vedono così:

- **calore**: `Q = F · 0,5 MW` per m² di locale che brucia (**STIMA**: incendio limitato dalla ventilazione);
- **fiamma** (Heskestad): `L = 0,235 · (Q/1000)^0,4 − 1,02 · D` m, con D il diametro del focolare. Il disegno la
  calcola con metà del calore su un focolare grande un decimo del locale, e almeno 1 m;
- **fumo** (Briggs, atmosfera neutra): flusso di galleggiamento `F = g · 0,7 Q / (π ρ c_p T)`, con l'aria a 15 °C; il
  pennacchio sale di `Δh = 1,6 · F^(1/3) · x^(2/3) / u` a distanza x sottovento col vento u (almeno 1,5 m/s), fino
  alla quota finale, che raggiunge a `x_f = 49 F^(5/8)` (sotto `F = 55`) o `119 F^(2/5)`;
- **colore del fumo**: nero per macchine, casse laterali, carico e carburante avio; grigio scuro per i depositi;
  grigio per gli altri locali. Di notte la fiamma lo illumina dal basso;
- **vapore** dove l'acqua spegne il fuoco (locale pieno oltre il 15%) e uno sbuffo quando l'incendio si spegne.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Raggio e periodi della bolla | Willis; Swift e Decius; dossier 07 §1.4 | alta |
| Oscillazioni prima della superficie, pennacchi | Swisdak 1978 (tabelle 12 e cap. 14), dossier 07 §1.4 e §2.4 | alta (una cifra illeggibile presa a metà) |
| Cupola | Kolsky, dossier 07 §2.3 | media |
| Anello scuro, base surge, chiazza | dossier 07 §2.2 e §2.5 | media |
| Regimi per d / A_max | dossier 07 §2.1 | media |
| Palla di fuoco, schegge, nubi | dossier 07 §4.3 e §11.2 | **STIMA** |
| Calore degli incendi | letteratura sugli incendi | **STIMA** |
| Fiamma e fumo | Heskestad; Briggs | alta (le formule), il calore è stima |
| Tetto della cupola sotto 5 kg | | **STIMA** |

## 4. Classico ed Enhanced

Il modello non esiste nell'originale. Gli effetti si disegnano con tutte e due le regole: anche nel livello classico
`DanniMissione` tiene la fisica delle navi colpite per l'immagine, e gli scoppi sui sottomarini arrivano lo stesso agli
effetti (`app/partita.cpp`). Cambia solo chi decide gli esiti: gli script nel classico, la fisica nell'Enhanced
(capitolo E2 §4).

## 5. Come si vede in gioco

Al periscopio (volume 1, capitoli 4 e 7): l'esplosione nel punto vero dello scoppio, alla sua distanza, con la scala e
l'orizzonte del periscopio. Le esplosioni arrivano agli effetti per due vie:
- **le navi con la fisica** (capitolo E2): ogni scoppio di `DanniNave` (siluro, mina, bomba, missile, proietto,
  deposito) si disegna nel punto dello scafo dove è avvenuto, con la sua carica. Uno scoppio nell'aria abbatte gli
  alberi entro `6 + 2,5 · W^(1/3)` m; uno sotto la chiglia abbatte ogni albero con probabilità 1/2;
- **gli altri colpi** (sottomarini, esche): uno scoppio subacqueo alla quota del colpo, con 135 kg per le cariche e i
  razzi, 544 kg per il Mk 48 e 300 kg per tutto il resto. I colpi in superficie e i missili non si disegnano qui.

Nelle minicartoline del debriefing, la fotografia del mondo 4 s dopo il colpo, quando la colonna è su; quelle degli
affondamenti sono scene composte, e fumo e colonna non vengono dalla simulazione (capitolo F1 §2.5). Le esplosioni
contro il nostro battello, viste dall'interno, sono scossa e suono (B3, C8).

## 6. Taratura e prove

`test/prove_effetti.cpp`:
- «fenomeni: Mk 48 sotto la chiglia (544 kg a 8 e 12 m)»: raggi, periodi, regimi, pennacchi di 180-230 m e 150-175 m
  contro i numeri del dossier 07 §2.6;
- «fenomeni: bombe di profondità, razzi e mine»: 135 kg a 15, 30 e 75 m, RBU da 25 kg, mina da 600 kg sul fondo col
  fango, cariche minuscole;
- «fenomeni: nell'aria e incendi»: Exocet, deposito, Briggs e Heskestad (1 MW su 1 m dà una fiamma di 2,7 m);
- le prove degli effetti: «effetti: sequenza di un'esplosione subacquea (Mk 48 a 12 m)», «effetti: carica profonda,
  solo gobba e chiazza; determinismo del seme», «effetti: nave colpita da un missile, fiamme e colonna di fumo
  sottovento», «effetti: disegno, limite del mare, fuoco emissivo e fumo acceso di notte», «danni: la bolla sotto la
  chiglia solleva la nave e la inarca».

Immagini di controllo con `--scoppio KG,PROF,DIST,RIL[,FONDALE]` e `--aerea KG,QUOTA,DIST,RIL[,deposito]`.

## 7. Limiti noti e in arrivo

- Le formule sono per esplosivi convenzionali in acqua libera; l'esplosione contro uno scafo (contatto) si disegna con
  la geometria dello scoppio ma senza il riflesso sullo scafo.
- Le esplosioni nucleari non ci sono.
- Le colonne dei colpi d'artiglieria hanno una loro messa in scena (capitolo E4).
- Gli scoppi sui sottomarini usano tre sole cariche (135, 544 e 300 kg): un Mk 46 o un AT-1 si disegna come un
  siluro da 300 kg, anche se i danni (capitolo B3) contano 108 e 105 kg.
- Gli scoppi sotto le navi si disegnano senza il fondale: la chiazza non diventa mai fango.

## 8. Riferimenti

- Codice: `nucleo/fenomeni.hpp`, `nucleo/src/fenomeni.cpp`, `grafica/effetti.hpp`, `grafica/src/effetti.cpp`,
  `app/partita.cpp` (gli scoppi degli altri colpi, le fotografie del debriefing), `app/main.cpp` (`--scoppio`,
  `--aerea`, il lampo sulla palette).
- Progetto: `docs/10_danni_affondamento_esplosioni.md` §5.3-5.6.
- Ricerca: dossier 07 §1.4-1.5, §2, §4.3, §11.
- Volume 1: cap. 4 e 7. Volume 2: capitoli B3, C8, E2, E4, E9.
