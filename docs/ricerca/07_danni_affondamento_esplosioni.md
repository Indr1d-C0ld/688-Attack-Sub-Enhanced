# 07 — Danni, affondamento ed esplosioni (dossier di riferimento)

Progetto: remake realistico di *688 Attack Sub* (1989). Periodo: tarda Guerra Fredda, circa 1985–1991.
Scopo: dare al motore formule e numeri per (a) l'aspetto delle esplosioni subacquee viste dal periscopio, (b) il modo in cui le navi incassano i colpi e affondano, (c) i rumori che il sonar sente dopo il colpo. Ogni dato ha fonte e grado di fiducia.

Questo documento **non ripete** quanto già scritto in [doc 02](02_acustica_oceanografia_dottrina.md) §9: efflusso da orifizio e portate (§9.1), dottrina d'allagamento dei SSN (§9.2), compartimentazione del 688 (§9.3), fattori d'urto HSF/KSF e relative soglie di gioco (§9.4). Le testate dei siluri e dei razzi ASW stanno in [doc 01](01_piattaforme_e_armi.md) (tabelle armi); i missili antinave e il loro aspetto al lancio in [doc 05](05_missili_e_contromisure.md) §1.8 e §6.

---

## 0. Convenzioni

- **Unità SI.** W = massa di carica in **kg di TNT equivalente**; R = distanza obliqua in m; d = profondità della carica in m; Z = d + 10 (m) = battente idrostatico totale (10 m ≈ pressione atmosferica). Tempi in s (o ms). Dove la fonte usa unità inglesi, la forma originale è riportata accanto.
- **TNT equivalente.** Non esiste un solo "TNT equivalente": un esplosivo alluminizzato vale più del TNT per la **bolla** che per l'**onda d'urto**. Il dossier usa due pesi: W_urto (per pressioni, impulsi, HSF/KSF) e W_bolla = W·RBE (per periodo, raggio, pennacchi). Swisdak stesso prescrive di moltiplicare per l'RBE prima di usare le tabelle di pennacchi e colonne. [alta] [Sw78]
- **Fiducia:** **[alta]** = formula o dato verificato su fonte primaria o secondaria autorevole; **[media]** = letteratura aperta credibile non controllata sulla primaria, o calcolo dell'autore con fisica standard; **[bassa]** = stima, fit dell'autore o fonte debole: **parametro di taratura**, non dato.
- I codici tra parentesi quadre ([Sw78], [Snay56] …) rimandano alla bibliografia (§12).

---

## 1. Esplosioni subacquee: fisica di base

### 1.1 Cronologia di un'esplosione [alta] — [Snay56]; [Reid96]
1. **Detonazione** (µs): la carica diventa gas a pressione altissima. La pressione iniziale nell'acqua è circa metà della pressione di detonazione.
2. **Onda d'urto** (fronte quasi verticale, decadimento in ~1 ms): si propaga a ~1500 m/s e trasporta circa metà dell'energia.
3. **Bolla di gas**: si espande per inerzia ben oltre l'equilibrio, poi collassa. A ogni minimo emette un **impulso di bolla** (*bubble pulse*). Oscilla più volte e **migra verso l'alto a salti**: sale in fretta vicino ai minimi, quasi per nulla quando è grande.
4. **Fenomeni di superficie**: cupola di spruzzi (dall'onda d'urto), poi pennacchi o colonna (dalla bolla), §2.

### 1.2 Onda d'urto: similitudine di Cole [alta] — [Sw78] Tab. 2 (da [Cole48])
Forma generale: `parametro = K · (W^(1/3)/R)^α`. Per il **TNT** (W in kg, R in m):

| Grandezza | K | α | Unità | Validità |
|---|---|---|---|---|
| Pressione di picco P_m | 52,4 | 1,13 | MPa | 3,4–138 MPa |
| Costante di tempo θ/W^(1/3) | 0,084 | −0,23 | ms/kg^(1/3) | idem |
| Impulso I/W^(1/3) | 5,75 | 0,89 | kPa·s/kg^(1/3) | integrato fino a 5θ |
| Densità di flusso d'energia E/W^(1/3) | 84,4 | 2,04 | m·kPa/kg^(1/3) | integrato fino a 5θ |

- **Forma originale (Cole, unità USA):** `P_m = 21 600 · (W^(1/3)/R)^1,13` psi, W in lb, R in ft. La conversione dà 52,4 MPa: coerente. [alta]
- **Profilo temporale:** `p(t) = P_m · e^(−t/θ)` per t < ~θ; poi la coda decade più lentamente. [alta] [Snay56]
- Altre cariche in Tab. 2 di [Sw78]: pentolite 56,5/1,14; **H‑6 59,2/1,19**; **HBX‑1 56,7/1,15**; HBX‑3 50,3/1,14 (P_m). [alta]
- **Esempio (calcolo):** 544 kg TNT (il Mk 48, vedi §1.5) a 20 m: P_m ≈ 52,4 × (8,16/20)^1,13 ≈ 19 MPa, θ ≈ 0,84 ms. A 10 km (propagazione sferica ideale): P ≈ 13 kPa, cioè ≈ 202 dB re 1 µPa. Il colpo si sente a distanze di bacino oceanico (§9). [media, calcolo]

### 1.3 Ripartizione dell'energia [media] — [Reid96]; [Snay56]; [Brain16]
- **≈ 53% nell'onda d'urto e ≈ 47% nella bolla.** Dell'urto, circa 20 punti si perdono vicino alla carica (riscaldamento) e circa 33% resta come energia utile al danno. [Reid96] ([Snay56] arrotonda a 50/50, da Arons e Yennie.)
- **Primo ciclo della bolla** (percentuali dell'energia totale): circa 13% dissipata durante la prima espansione‑contrazione, 17% irradiata col primo impulso di bolla, **circa 17% rimasta** per le pulsazioni successive. [Reid96]; [Brain16]
- Per una bolla che non migra (Arons e Yennie, in [Snay56]): al primo minimo il 25% dell'energia di bolla esce come impulso, il 37,5% si dissipa e il 37,5% resta per il ciclo dopo. Le pulsazioni successive perdono ancora circa il 50–60% a ciclo. [media] [Brain16]
- **Regola di gioco:** energia di bolla al ciclo n ≈ 0,47·E₀·(0,37)^(n−1). Dopo 3 cicli la bolla è energeticamente esaurita. [media]

### 1.4 Bolla: raggio massimo, periodo, migrazione, pulsazioni [alta per le formule]
**Primo periodo (formula di Willis) e raggio massimo** — [Sw78] cap. 10; [Brain16] Tab. 4:
```
T₁    = K · W^(1/3) / Z^(5/6)      [s]   Z = d + 10  [m]
A_max = J · W^(1/3) / Z^(1/3)      [m]
```

| Esplosivo | J (m^(4/3)/kg^(1/3)) | K (s·m^(5/6)/kg^(1/3)) | RBE (rispetto al TNT) | RPBE (rispetto al TNT) |
|---|---|---|---|---|
| TNT | 3,50 | 2,11 | 1,00 | 1,00 |
| Pentolite | 3,52 | 2,11 | 1,00 | 1,02 |
| HBX‑1 | 3,95 | 2,41 | 1,48 | 1,44 |
| HBX‑3 | 4,27 | 2,63 | 1,93 | 1,82 |
| H‑6 | 4,09 | 2,52 | 1,69 | 1,59 |

[alta] [Sw78] Tab. 10

- **Forma originale (Cole, unità USA):** `T = 4,36 · W^(1/3)/(d+33)^(5/6)` s (W in lb, d in ft). [alta] (citata in [USP8264909]) Il raggio in unità USA, ricavato dalla J SI, è circa 13·(W/(d+33))^(1/3) ft. [media, conversione]
- **Validità:** acqua libera, con la bolla ad almeno ~10 raggi dalla superficie e dal fondo. Correzione per la superficie libera (Snay): `T_FS = T·(1 − 0,1·A_max/d)`. [media] [Brain16]; [Sw78] eq. 3 (correzione per il fondo).
- **Pulsazioni successive** (TNT, costanti di Swift e Decius): K₂ = 1,57, K₃ = 1,33; J₂ ≈ 2,36, J₃ ≈ 1,83 (aggiustati). Si usa Z al centro della bolla all'inizio di ogni ciclo. [media] [Brain16]
- **Raggio minimo:** `A_min ≈ 0,113·W^(1/3)` m. [media] [Brain16]
- **Migrazione verticale al primo minimo:** `m ≈ 12,2 · W^(1/2) / Z` m. Tra il primo e il secondo minimo la bolla sale ancora circa m/2. [media] [Brain16]
- **Pressione dell'impulso di bolla:** `P_BP ≈ 2917 · Z^(1/6) · W^(1/3)/R` kPa. Impulso: `I_BP ≈ 85,2 · Z^(−0,4) · W^(2/3)/R` kPa·s. Dura molto più dell'urto ma ha picco circa 10 volte più basso. Esempio: 544 kg a 12 m, R = 20 m: P_BP ≈ 2 MPa contro ≈ 19 MPa dell'urto. [media] [Brain16]
- **Numero di oscillazioni prima che la bolla arrivi in superficie** (TNT 140–900 kg, fondo ininfluente). Si moltiplica W per l'RBE prima di entrare in tabella. [alta] [Sw78] Tab. 12

| d/W^(1/3) (m/kg^(1/3)) | 0,5 | 1,0 | 1,5 | 2,0 | 2,5 | 3,0 | 3,5 | 4,0 | 4,5 | 5,0 | 5,5 | 6,0 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| oscillazioni N | 0,37 | 0,72 | 1,08 | 1,45 | 1,83 | 2,22 | 2,65 | 3,07 | 3,55 | 4,07 | 4,68 | 5,37 |

(N = 0,5 vuol dire che la bolla arriva alla superficie al primo massimo; N = 1 al primo minimo, e così via.)

### 1.5 Esplosivi delle testate del 1989 e TNT equivalenti
| Esplosivo / arma | Composizione | Urto (P_m, E) rispetto al TNT | Bolla (RBE) | Fonte / fiducia |
|---|---|---|---|---|
| TNT | — | 1,00 / 1,00 | 1,00 | [Sw78] [alta] |
| **H‑6** (mine USA Mk 62–64, bombe) | RDX/TNT/Al/cera 45/30/20/5 | **1,13 / 1,37** | **1,69** | [Sw78] Tab. 4‑1 [alta] |
| **HBX‑1** (cariche di profondità, siluri anni '50) | 40/38/17/5 | 1,08 / 1,26 | 1,48 | [Sw78] [alta] |
| HBX‑3 (Mk 37, mina Mk 56) | 31/29/35/5 | 0,96 / 1,08 | 1,93 | [Sw78]; [Brain16] Tab. 1 [alta] |
| **Torpex** (Mk 8 del *Conqueror*, Tigerfish, Mk 9 e Hedgehog della WWII) | RDX/TNT/Al ≈ 42/40/18 | **non tabellato in [Sw78]**; HBX‑1 vale il 98–100% del Torpex | assumere ≈ HBX‑1 (≈ 1,5) | [IMWS] [media] |
| **PBXN‑103** (Mk 46, Mk 48, Mk 48 ADCAP) | AP/Al/legante (PBX alluminizzato) | n.d. | n.d. | — |
| **Mk 48: 295 kg PBXN‑103 ≈ 544 kg TNT** (≈ 650 lb ≈ 1200 lb TNT) | | | | [NW‑US]; [Brain16] Tab. 1 [media] |
| Mk 54: 44 kg PBXN‑103 ≈ 108 kg TNT | | | | [Brain16] [media] |
| DM2A4: 255 kg PBX ≈ 460 kg TNT | | | | [Brain16] [media] |
| Siluri sovietici Tipo 53: 265–400 kg, TNT equivalente ≈ massa | | | | [Brain16] [bassa] |

- Il passaggio ai PBX alluminizzati aumentò l'**area letale** delle armi subacquee di almeno il 50% rispetto alle cariche TNT/RDX/Al precedenti. [media] [IMWS]
- **Nota sull'equivalenza del Mk 48:** il rapporto 544/295 ≈ 1,84 è vicino agli RBE degli alluminizzati (1,5–1,9), quindi descrive quasi certamente la **bolla**, non l'urto. Per il gioco: W_bolla ≈ 1,8·m, W_urto ≈ 1,2·m (stima per analogia con H‑6 e HBX). [bassa]
- **Testate sovietiche:** la composizione pubblica è vaga (miscele TGAG‑5 e simili). Per il gioco: W_bolla ≈ 1,5·m, W_urto ≈ 1,1·m. [bassa]

---

## 2. Fenomeni in superficie (visti dal periscopio)

### 2.1 Sequenza visibile e regimi in funzione della profondità [alta per la sequenza] — [Snay56]; [Reid96]; [Kol49]
La profondità che conta è quella **relativa al raggio massimo della bolla**, d/A_max (oppure d/W^(1/3)). Per l'altezza assoluta della colonna conta anche la massa assoluta. [Snay56]

| d/A_max | Regime | Che cosa si vede | Fonte |
|---|---|---|---|
| ≲ 0,3–0,5 | **Sfiato immediato** (*blow‑out*) | La bolla sfonda subito: colonna cilindrica cava di acqua con in cima una **nube scura di fumo e acqua** (*smoke crown*); nessuna pulsazione | [Snay56] fig. 12–13; [Sw78] cap. 13 |
| ≈ 0,4–0,9 | **Sfiato attraverso la colonna** | Colonna d'acqua, poco gas visibile; la bolla comunica con l'atmosfera e non pulsa più | [Snay56] fig. 13 |
| ≈ 0,9–1,5 | **Non sfiata al primo ciclo** | Getto sottile e alto al centro con una gonna di spruzzi più bassa e larga alla base | [Snay56] fig. 14 |
| ≈ 1,5–3 | **Pennacchi** (*plumes*) | Prima la cupola; poi, **all'istante del primo impulso di bolla**, uno o più getti violenti che bucano la cupola; altri getti quando il gas sfonda. Se la bolla rompe la superficie al 2° o 3° massimo, il pennacchio è più sottile, più alto e **annerito** dai gas carichi di carbonio | [Snay56] fig. 15; [Reid96] §1.3 |
| ≳ 4–8 | **Profonda** | Piccola cupola che svanisce presto; dopo un po' una massa di gas o una chiazza di "acqua gassata" schiumosa; nessun pennacchio | [Snay56] fig. 17 |

- [Reid96]: se la prima espansione non sfonda, il primo pennacchio è **largo e basso**; lo sfondamento avviene di solito al 2° o 3° massimo, perché dopo l'energia è esaurita.
- Le cariche piccole (sotto ~5 kg) non fanno pennacchi vistosi. [alta] [Snay56]
- **Profondità critica di sfiato (regola di gioco):** la bolla sfiata con colonna scura se `d < 1·A_max` (cioè N < ~0,9 in Tab. 12); fa pennacchi bianchi tra 1 e ~3 A_max; sotto ~4–5 A_max lascia solo una cupola bassa e una chiazza. [media, sintesi dell'autore da [Snay56] e [Sw78]]

### 2.2 "Slick" (anello scuro) e "crack" [alta per il fenomeno] — [Kol49]
- **Crack:** nei primi millisecondi compare un **sbiancamento sotto la superficie**. Cresce alla velocità con cui l'onda d'urto raggiunge la superficie: è cavitazione appena sotto il pelo dell'acqua.
- **Anello scuro** (*black ring*, *slick*): attorno alle cupole delle cariche profonde corre un anello che appare scuro. È un **irruvidimento della superficie** (increspature esaltate e "punte" d'acqua) prodotto dall'arrivo dell'urto.
- **Geometria [alta, geometria pura]:** il raggio dell'anello cresce come `r(t) = sqrt((c·t)² − d²)` per t > d/c, con c ≈ 1500 m/s. La velocità apparente all'inizio è "infinita" e poi tende a c. Per d = 50 m l'anello è largo 100 m dopo 0,075 s e 300 m dopo 0,21 s: a occhio è un **lampo di un decimo di secondo**.
- **Estensione visibile [bassa]:** l'anello si vede finché l'urto in superficie supera ~1–2 MPa, cioè fino a `R ≈ W^(1/3)·(52,4/P_soglia)^(1/1,13)`. Con P_soglia = 2 MPa: ≈ 18·W^(1/3) m (≈ 150 m per 544 kg, ≈ 90 m per 135 kg).

### 2.3 Cupola di spruzzi (*spray dome*) [media]
- **Origine:** l'urto riflesso in superficie mette l'acqua in tensione; la superficie si frammenta in aghi e gocce (instabilità di Taylor) che partono verso l'alto. [Snay56]
- **Velocità iniziale al centro (Kolsky et al. 1949):** `v₀ ≈ 66·(W_lb^(1/3)/D − 0,1)` m/s, con W in lb e **D in metri**, entro ±10%, valida per 0,3 < W_lb^(1/3)/D < 4. Misurata con cariche da 1 e 10 lb. [media] [Kol49] (l'abstract trascritto da Crossref riporta "W1/2", ma l'intervallo di validità è dato in W^(1/3)/D; si assume W^(1/3)).
  - **In SI** (W in kg): `v₀ ≈ 86·W^(1/3)/D − 6,6` m/s, valida per 0,23 < W^(1/3)/D < 3,1. [media, conversione]
  - Fisicamente `v₀ ≈ 2·P_m(D)/(ρc)` (velocità di particella raddoppiata alla superficie libera): le due stime concordano entro ~20%. [media, calcolo]
- **Profilo:** in acqua calma la cupola ha forma `H(R) = B·[D²/(D²+R²) − C]` (B e C costanti della carica): la velocità si dimezza circa a R = D. **Diametro visibile della cupola ≈ 2–3·D.** [media] [Kol49]
- **Altezza massima:** non ho trovato una formula empirica pubblica verificata (le formule citate in letteratura cinese e nella rassegna MDPI 2020 non erano accessibili). **Stima di gioco:** `H_dome ≈ 0,4–0,5·v₀²/(2g)`. Il fattore 0,4–0,5 tiene conto di resistenza dell'aria e frantumazione, ed è tarato su Wahoo (9 kt a 150 m, cupola osservata 270 m; v₀ da Kolsky ≈ 113 m/s, balistica 650 m). [bassa] [WP‑UE]
- **Tempi:** la cupola parte a t = D/c (pochi ms). Bikini Baker: cupola visibile dopo 4 ms, salita iniziale ~760 m/s (2500 ft/s), subito frenata. Tempo all'apice ≈ 2·H/v₀ (forte decelerazione); ricaduta in 2–5 s. [media] [GD77]; [bassa] per i tempi convenzionali

### 2.4 Pennacchi e colonna: altezza e raggio massimi [media] — [Sw78] capp. 13–14
**(a) Pennacchi da esplosione subacquea di TNT, sul fondo o lontano dal fondo** (cap. 14). Ingresso: `y = d/W^(1/4)` (m/kg^(1/4)). Uscita: altezza e raggio scalati con W^(1/3).

| y = d/W^(1/4) | 0 | 0,5 | 1,0 | 1,5 | 2,0 | 2,5 | 3,0 | 3,5 | 4,0 | 4,5 | 5,0 | 5,5 | 6,0 | 6,5 | 7,0 | 8,0 | 9,0 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| H_max/W^(1/3) | 29,7 | ≈34–39* | 38,6 | 29,0 | 20,6 | 19,7 | 18,9 | 18,0 | 17,1 | 16,2 | 15,4 | 13,6 | 9,5 | 5,9 | 5,9 | 5,9 | 5,9 |
| R_max/W^(1/3) | 8,3 | 8,3 | 8,3 | 10,0 | 12,8 | 12,8 | 12,8 | 12,8 | 12,8 | 11,6 | 10,6 | 9,5 | 8,5 | 7,4 | 6,4 | 4,3 | 2,3 |

\* cifra illeggibile nella scansione. Si moltiplica W per l'RBE prima di entrare. **Avvertenza della fonte:** le curve servono a stabilire distanze di sicurezza; le dimensioni reali possono variare fino a circa ±50%. Esempio della fonte: 500 kg TNT a 30 m → altezza ≈ 56 m, raggio ≈ 61 m.

**(b) Colonna e getto da esplosione molto bassa** (cap. 13, da Milligan e Young 1954, NAVORD 2987):
- `H_jet/W^(1/3) = 32,4·(d/W^(1/4))^0,1` per 0,0037 < d/W^(1/4) < 0,74
- `H_jet/W^(1/3) = 21,7·(d/W^(1/4))^(−1,24)` per 0,74 < d/W^(1/4) < 1,56
- Diametro della colonna: `D_col/W^(1/3) = 3,71·(d/W^(1/3))^0,166` per 0,08 < d/W^(1/3) < 0,88
- Diametro della "corona di fumo": `S_max/W^(1/3) ≈ 9,0` per 0,04 < d/W^(1/3) < 0,24
- La fonte consiglia di aggiungere 30% a D e 40% a H per la sicurezza. Esempio: 1000 kg TNT a 2 m → getto ≈ 290 m, colonna ≈ 28 m, corona ≈ 90 m. [media]

**(c) Tempi** (non tabellati nelle fonti consultate): il pennacchio parte all'istante del primo impulso di bolla, t ≈ T₁ [alta] [Snay56]. Salita all'apice in ≈ `sqrt(2H/g)` (circa 5 s per 150 m; un po' meno con la resistenza dell'aria), poi ricaduta di 5–10 s. Nebbia e spruzzi restano sospesi 15–40 s, a seconda del vento. [bassa]

### 2.5 Base surge, ricaduta, chiazza, pulsazioni successive
- **Base surge** (anello basso di nebbia che si allarga dalla base della colonna): documentato per cariche grandi e basse; cfr. Milligan e Young, *Scaling of base surge phenomena of shallow underwater explosions* (citato da [Sw78]). Per le cariche convenzionali (≤ 1 t) è modesto: un anello di spruzzo radente di raggio ≈ 1–2 volte il raggio del pennacchio. Nei test nucleari: Baker (23 kt a ~27 m) 5,6 km di diametro e 550 m d'altezza; Wahoo (9 kt a 150 m) 4 km e 300 m. [media per il nucleare] [WP‑UE]; [bassa per il convenzionale]
- **Ricaduta:** l'acqua del pennacchio ricade in gran parte entro R_max; la nave colpita sotto la chiglia ne viene investita (si vede nelle riprese delle SINKEX). [bassa]
- **Chiazza:** dopo lo sfiato resta una chiazza di schiuma bianca o verdastra. Con il fondo vicino (mina) diventa **marrone di fango sollevato**; resta una superficie "a soda" con bolle che salgono per minuti; pesci morti. [bassa] [Snay56] (soda water)
- **Pulsazioni successive viste dall'alto:** con d/A_max ≈ 1,5–4 ogni minimo della bolla vicino alla superficie produce un nuovo **sussulto** della cupola o un nuovo getto. Gli intervalli sono T₁, poi T₂ ≈ 0,75·T₁ (K₂/K₁), poi T₃ ≈ 0,63·T₁, ciascuno più debole (§1.3). Una carica di profondità regolata bene "solleva appena la superficie e solo dopo un po' esplode in uno sfiato". [media] [Brain16]; [WP‑DC]

### 2.6 Valori tipici calcolati (TNT equivalente per la bolla)
Calcolati dall'autore con le formule di §1.4 e le tabelle di §2.4. [media per T₁, A_max e N; bassa per altezze e raggi]

| Caso | W_bolla (kg) | d (m) | A_max (m) | d/A_max | T₁ (s) | N osc. (Tab. 12) | regime | pennacchio H × R (m) | v₀ cupola (m/s) |
|---|---|---|---|---|---|---|---|---|---|
| Siluro pesante USA (Mk 48) sotto chiglia | 544 | 8 | 10,9 | 0,7 | 1,55 | 0,7 | sfiato, colonna scura | ≈ 215 × 90 | ≈ 80 |
| idem | 544 | 12 | 10,2 | 1,2 | 1,31 | 1,1 | getto/pennacchio | ≈ 160 × 105 | ≈ 52 |
| Siluro pesante ≈ 300 kg TNT sotto chiglia | 300 | 8 | 8,9 | 0,9 | 1,27 | 0,9 | sfiato/getto | ≈ 150 × 85 | ≈ 65 |
| idem | 300 | 12 | 8,4 | 1,4 | 1,07 | 1,3 | pennacchi | ≈ 130 × 85 | ≈ 41 |
| Cheonan (stima JIG 250 kg) | 250 | 7 | 8,6 | 0,8 | 1,25 | 0,8 | sfiato | ≈ 155 × 70 | ≈ 71 |
| Carica di profondità Mk 9 (91 kg Torpex ≈ 135 kg) | 135 | 15 | 6,1 | 2,4 | 0,74 | 2,2 | pennacchi | ≈ 85 × 60 | ≈ 23 |
| idem | 135 | 30 | 5,2 | 5,7 | 0,50 | 5,2 | profonda | ≈ 30 × 15 | fuori validità (piccola) |
| idem | 135 | 50–100 | 4,6–3,7 | 11–27 | 0,36–0,22 | > 5,4 | profonda | solo gobba e chiazza | — |
| Carica pesante (Mk 7, 270 kg) | 270 | 15 | 7,7 | 1,9 | 0,93 | 1,7 | pennacchi | ≈ 115 × 85 | ≈ 30 |
| idem | 270 | 50 | 5,8 | 8,7 | 0,45 | > 5,4 | profonda | gobba e chiazza | — |
| Razzo RBU‑6000 (23 kg, W ≈ 25) | 25 | 10 | 3,8 | 2,7 | 0,51 | 2,6 | pennacchi | ≈ 50 × 35 | ≈ 19 |
| idem | 25 | 50–200 | 2,6–1,7 | 19–116 | 0,20–0,07 | > 5,4 | profonda | quasi nulla | — |
| Mina a contatto (≈ 145 kg) | 145 | 7 | 7,2 | 1,0 | 1,05 | 1,0 | getto/sfiato | ≈ 110 × 65 | ≈ 58 |
| Mina da fondo (600 kg) in 20 m d'acqua | 600 | 20 | 9,5 | 2,1 | 1,05 | 1,7 | pennacchi + fango | ≈ 145 × 105 | ≈ 30 |
| idem in 40 m d'acqua | 600 | 40 | 8,0 | 5,0 | 0,68 | 3,8 | profonda | ≈ 50 × 35 | — |

- **Controllo con un caso reale:** ex‑HMAS *Torrens* (1999), Mk 48 sotto chiglia. Con W = 544 kg e d ≈ 10 m le tabelle danno un pennacchio di ≈ 165 m. Una fonte debole parla di "circa 150 m" di acqua e frammenti. [bassa] [Torr‑plume] Coerente.
- **Carica nucleare (solo nota):** Baker (23 kt, 27 m in 55 m d'acqua) colonna di ≈ 1800 m e ≈ 600 m di diametro; Wahoo (9 kt a 150 m) cupola di 270 m e getti di 520 m. Le bolle nucleari non oscillano più dopo ~6 s. [media] [WP‑UE]; [GD77]

### 2.7 Esempi fotografici famosi
| Immagine | Che cosa mostra | Dimensioni stimate | Fonte |
|---|---|---|---|
| *Mark 48 Torpedo testing.jpg* (ex‑HMAS *Torrens*, 14/6/1999, dal *Farncomb*) | Nave sollevata e spezzata a "V" al centro di una colonna bianca | Pennacchio ≈ 150 m [bassa]; nave lunga 112 m | [WP‑Torr]; [Torr‑plume] |
| Ex‑USS *Racine* (LST, RIMPAC 2018), Mk 48 dal USS *Olympia* | Colpo al centro, chiglia spezzata | Video ufficiale USN | [TWZ‑Racine]; [MarEx] |
| Ex‑USS *Ingraham* (FFG‑61, SINKEX al largo delle Hawaii, 2021) | Il Mk 48 ADCAP finale spezza la fregata | Video USN | [IE‑Ingraham] |
| Bombe di profondità della WWII (foto USN) | Colonne bianche "a cavolfiore" a poppa delle scorte | Da ≈ 20 m (regolazioni profonde) a ≈ 60–80 m (basse) [bassa, calcolo §2.6] | [WP‑DC] |

> **Correzione:** l'ex‑USS *Agerholm* (DD‑826) **non** fu affondata da un Mk 48. Fu il bersaglio di un UGM‑109 Tomahawk lanciato dal USS *Guitarro* il 18/7/1982 (dato con "citation needed" su Wikipedia). [media] [WP‑Ager]

---

## 3. Effetti sullo scafo

### 3.1 Meccanismi [alta] — [Reid96] §2; [Snay56]
1. **Urto** (contatto o vicino): squarcio locale, paratie vicine rotte, frammenti; il danno raramente si estende molto all'interno o in senso longitudinale. Oltre la distanza di rottura lo scafo resta stagno ma deformato (lamiere "imbutite"), poi solo elastico.
2. **Flusso incomprimibile della bolla in espansione:** spinge per più tempo dell'urto e può valere **≥ 60%** dell'energia di deformazione plastica.
3. **Impulso di bolla e collasso con getto** (*jet*): se la bolla collassa vicino allo scafo forma un getto d'acqua ad alta velocità che può **forare** lo scafo.
4. **Whipping:** flessione dinamica dell'intera trave di scafo eccitata da urto e pulsazioni (§3.3). Più pericoloso per le navi di superficie che per i sottomarini.
5. **Choc sugli apparati:** KSF e velocità di traslazione di picco: vedi [doc 02 §9.4](02_acustica_oceanografia_dottrina.md).

### 3.2 Esplosione a contatto: dimensioni della falla
**Formule:** in letteratura si cita una regressione di Keil (1956) che lega raggio della falla, carica e spessore della lamiera (forma riportata in modo illeggibile nelle fonti secondarie consultate), e un modello energetico di Rajendran e Narasimhan (2001). **Non verificate.** [bassa]

**Casi documentati** (falla = apertura nel fasciame esterno):

| Nave (anno) | Arma, carica | Punto | Falla | Esito | Fonte |
|---|---|---|---|---|---|
| USS *Saratoga* CV‑3 (gen. 1942) | Siluro da sottomarino giapponese (testata ≈ 400 kg, n.v.) | Sotto la cintura | ≈ 9 × 6 m (30 ft × 20 ft); paratia antisiluro con squarcio di 3 × 1 m | Salva | [WDR‑Sara] [media] |
| USS *Northampton* CA‑26 (1942) | Siluro Type 93 (≈ 490 kg, n.v.) vicino alla linea di galleggiamento | Fianco | Dal sotto‑galleggiamento al ponte di coperta, 7 ordinate; ponti squarciati per 6 m verso l'interno | Affondata | [WDR‑North] [media] |
| ARA *General Belgrano* (1982) | Mk 8 mod 4, 363 kg Torpex | 1° a 10–15 m dalla prua | **Prua asportata** | v. §3.5 | [WP‑Belg] [alta] |
| idem | idem | 2° a ~3/4 della lunghezza, sala macchine poppiera | Squarcio di **20 m nel ponte di coperta** | Affondata | [WP‑Belg] [alta] |
| USS *Samuel B. Roberts* FFG‑58 (1988) | Mina M‑08 (≈ 115 kg TNT, n.v.) | Sala macchine, babordo | **4,5–7 m** (15–23 ft secondo la fonte); chiglia rotta, motori strappati dai supporti | Salva dopo 5 h di lotta | [DANFS‑SBR]; [WP‑SBR] [alta] |
| USS *Tripoli* LPH‑10 (1991) | Mina a contatto LUGM‑145 (≈ 145 kg, n.v.) | Prua dritta, 3 m sotto il galleggiamento | 4,9 × 6 m → rivista **6 × 9 m** (20 ft × 30 ft) | Salva | [USNI91]; [Gulflink] [media] |
| *Bridgeton* (petroliera, 1987) | Mina M‑08 (n.v.) | Prua | ≈ 10 × 5 m in lamiera di 27 mm | Proseguì con mezzi propri | [NM‑Bridg] [bassa] |
| USS *Stark* FFG‑31 (1987) | Exocet AM39, 165 kg (detonato ~1,5 m dentro lo scafo) | Sopra il galleggiamento | ≈ 3 × 4,6 m (15 ft) | Salva | [CSIS]; [WP‑Stark] [alta] |
| HMS *Sheffield* (1982) | Exocet AM39 | 2,4 m sopra il galleggiamento | ≈ 1,2 × 3 m (foro d'ingresso) | Affondata 6 giorni dopo | [WP‑Sheff] [alta] |

**Fit dell'autore per il gioco** (contatto o quasi, sotto il galleggiamento, fasciame di nave militare leggera o mercantile da 8 a 30 mm): diametro equivalente della falla `D_falla ≈ (1,0–1,5)·W^(1/3)` m (W kg TNT). Esempi: 145 kg → 5–8 m; 544 kg → 8–12 m. Sopra il galleggiamento (missile che esplode dentro): `≈ 0,6–0,8·W^(1/3)` sul fasciame, più danni interni. Con W > ~250 kg contro una nave < ~3000 t si passa alla **rottura della trave** (§3.4) più che alla semplice falla. [bassa]

### 3.3 Esplosione sotto la chiglia senza contatto: sollevamento e whipping
**Fisica [alta]** — [Snay56]; [Reid96]; [Brain16]:
- La bolla in espansione **solleva** la zona centrale della nave (*hogging*, inarcamento). Al collasso la nave ricade nel vuoto lasciato dalla bolla (*sagging*, insellamento). Il getto e l'impulso di bolla colpiscono il fondo. Le vibrazioni trasversali della trave intera eccitate così sono il *whipping*: "la resistenza longitudinale di molte navi non basta, e spesso sono state spezzate in due". [Snay56]
- Il caso più severo è **sotto la chiglia, vicino al centro nave**, con frequenza della bolla vicina a un modo flessionale della nave. [Brain16] Il periodo della bolla delle grandi cariche è dello stesso ordine del periodo del modo fondamentale a 2 nodi delle navi piccole: **0,3–1 s**. [media] [Brain16] (che cita la letteratura NSWC)
- La bolla causa soprattutto *sagging* quando la distanza carica‑scafo è tra 0,7 e 2 A_max. [media] [DT23]

**Frequenza propria a 2 nodi (verticale) della trave di scafo:**
- **Todd (1931–32), con la massa aggiunta di Kumai:** `N₂v = β · sqrt(B·D³ / (Δᵢ·L³))` cicli/min, con `Δᵢ = Δ·(1,2 + B/(3·d))`. L, B, D, d in m (lunghezza tra le perpendicolari, larghezza, altezza di costruzione, immersione); Δ in t. **β ≈ 1,0–1,3·10⁵** per cargo e cisterne (verificato dall'autore sui dati di Kumai: nave da 151 m e 10 390 t → 102 cpm = 1,7 Hz; da 114 m e 5376 t → 133 cpm = 2,2 Hz). Lo stesso β cala del 3–5% a pieno carico. [alta per la forma; media per β] [Kumai57]
- **Schlick:** `N = φ·sqrt(I/(Δ·L³))` (I momento d'inerzia della sezione maestra). Costante φ non verificata. [bassa]
- **Kumai (formula col momento d'inerzia, 3,07·10⁶):** citata spesso, ma la costante non è stata confermata sulle fonti consultate. [bassa]
- **Dati di riferimento:** cacciatorpediniere di Hicks, primo modo flessionale (3° modo libero) **1,54 Hz**, 2° flessionale 3,13 Hz [media] [Brain16] Tab. 15. Nave da 215 m e 61 000 t (Zhang e Zong): 2,03 Hz [media] [Brain16] Tab. 16.
- **Stime dell'autore con Todd e β = 1,2·10⁵** [bassa]:

| Nave | f₂ₙ (Hz) | T₂ₙ (s) |
|---|---|---|
| Corvetta classe Pohang (*Cheonan*, 88 m, 1220 t) | ≈ 2,4 | 0,41 |
| Fregata o DE (*Torrens*, 112 m, 2700 t) | ≈ 1,9 | 0,51 |
| Fregata *Perry* (4100 t) | ≈ 1,5 | 0,65 |
| Cacciatorpediniere *Spruance* o *Sovremenny* (8000 t) | ≈ 1,4 | 0,72 |
| Incrociatore *Belgrano* (12 200 t) | ≈ 1,2 | 0,86 |
| Mercantile da 15 000 t | ≈ 1,3 | 0,78 |
| Portaerei classe Nimitz | ≈ 0,8 | 1,3 |
| Superpetroliera (VLCC) carica | ≈ 0,5 | 1,8 |

- **Confronto col periodo della bolla:** un siluro pesante a 8–15 m sotto la superficie ha T₁ ≈ 1–1,5 s (§2.6), più lungo del periodo a 2 nodi di corvette e fregate (0,4–0,7 s). La rottura di queste navi è quindi soprattutto **quasi statica**: sollevamento più insellamento, più getto, più che risonanza pura. La risonanza diventa probabile con cariche più profonde o più piccole (167 kg a 24 m è stata scelta proprio per eccitare una fregata [Brain16]) e con navi grandi (T₂ₙ ≈ 1–2 s). [media, ragionamento dell'autore su [Brain16]]

### 3.4 Rottura della chiglia: criteri pratici [bassa, modello dell'autore]
In fonte aperta non esiste una soglia numerica di rottura della trave per le navi da guerra. Dai casi (§3.5 e §7) si ricava una regola di taratura:
- **Condizione geometrica:** carica sotto la chiglia, entro ±0,25 L dal centro nave, con distanza verticale dalla chiglia s ≤ 1,5·A_max.
- **Rottura certa** se L < 130 m e W_bolla ≥ 150 kg (Cheonan 88 m; Torrens 112 m; Ingraham 138 m; Racine 159 m con Mk 48).
- **Probabile** per 130–200 m con W_bolla ≥ 400 kg.
- **Improbabile** oltre 200 m o 20 000 t: le navi grandi incassano la falla ma la trave regge (Belgrano 185 m: colpita a contatto, perse la prua ma non si spezzò al centro).
- **Effetto della distanza dal centro:** fuori dal tratto ±0,25 L il danno diventa "prua o poppa asportata" invece di "spezzata in due".
- Rapporto utile per il motore: `κ = T₁·f₂ₙ`. Il danno da whipping è massimo per κ ≈ 1 e decresce con |ln κ|. Moltiplicatore di severità consigliato: `exp(−(ln κ)²/0,5)`, sommato a una componente quasi statica proporzionale a W_bolla/Δ. [bassa]

### 3.5 Casi documentati
**ROKS *Cheonan* (PCC‑772), 26/3/2010** — Joint Civilian‑Military Investigation Group (JIG), con esperti di USA, Regno Unito, Australia e Svezia. [alta per i fatti; media per l'arma] [WP‑Cheonan]; [KimG]
- Nave: corvetta classe Pohang, ≈ 1220 t, 88 m. 104 a bordo, **46 morti**.
- Arma secondo il JIG: siluro nordcoreano CHT‑02D, **250 kg TNT equivalente**, esploso **6–9 m sotto la superficie, ~3 m a sinistra** della sala turbine a gas, **senza contatto**.
- Meccanismo secondo il JIG: urto più bolla. La chiglia centrale si piegò verso l'alto e i fondi di prua e poppa si deformarono verso l'alto. Nella replica alle critiche il JIG attribuisce il taglio al **getto d'acqua** del collasso della bolla più che all'urto.
- Esito: **spezzata in due**. Il troncone di poppa affondò in pochi minuti; quello di prua restò a galla capovolto più a lungo e andò alla deriva. Esplosione alle 21:22, nave sparita intorno alle 21:30.
- Dati sismici e idroacustici: magnitudo locale ≈ 1,5 (JIG). Kim, Gitterman e Rodríguez: frequenza di bolla **1,01 Hz** (periodo osservato **0,998 s**) e stima di **≈ 136 kg TNT a ~8 m** in ~44 m d'acqua. Con §1.4: 250 kg a 7 m darebbero T₁ ≈ 1,25 s (1,10 s con la correzione di superficie); 136 kg a 8 m danno 0,98 s. [media] [KimG]
- Contestazioni: tesi alternative (urto contro il fondo o collisione, analisi russa non pubblicata). Il JIG resta l'unico rapporto ufficiale. [media]

**Ex‑HMAS *Torrens* (DE‑53), 14/6/1999** — Mk 48 **Mod 4** (non ADCAP) dal *Farncomb* durante le prove del sistema di combattimento classe Collins. Colpo al centro, chiglia spezzata, nave in **due tronconi**: la poppa affondò per prima, la prua più tardi. 2700 t, 112 m. [alta] [WP‑Torr]; [bassa] tempi

**Ex‑USS *Racine* (LST‑1191), RIMPAC 2018** — dopo i colpi dei missili, un Mk 48 dal USS *Olympia* la colpì al centro, le spezzò la chiglia e la mandò a fondo. [media] [TWZ‑Racine]; [MarEx]

**Ex‑USS *Ingraham* (FFG‑61), SINKEX 2021** — dopo diversi colpi, il Mk 48 ADCAP finale spezzò la nave. [media] [IE‑Ingraham]

***New Carissa* (troncone di poppa, 1999)** — mercantile incagliato. Trainato al largo, resistette a colpi d'artiglieria; fu finito con un Mk 48 ADCAP del USS *Bremerton*. [media] [NAVSEA‑Brem]

**ARA *General Belgrano*, 2/5/1982** — tre Mk 8 mod 4 (363 kg Torpex, a corsa diritta) dal SSN *Conqueror*; due a segno. [alta] [WP‑Belg]
- 1° colpo: 10–15 m dalla prua, fuori dalla cintura e dal controcarena. **Prua asportata**; le paratie interne ressero; il deposito da 40 mm non esplose.
- 2° colpo: a ~3/4 della lunghezza, sala macchine poppiera; squarcio di 20 m nel ponte; niente corrente, niente radio, **porte stagne aperte**.
- 323 morti, quasi tutti per l'esplosione. Sbandamento a sinistra, abbandono **circa 20 minuti dopo** il colpo, affondamento di prua. [alta] Il tempo totale fino alla scomparsa (≈ 1 h) non è confermato da fonte solida. [bassa]

---

## 4. Missili antinave e incendi

### 4.1 Effetto di una testata sopra la linea di galleggiamento [media] — [CSIS]; [WP‑Sheff]; [WP‑Stark]
- **Exocet AM39:** massa al lancio 655 kg, **testata 165 kg**. Normalmente **penetra e poi esplode**; può incendiare il carburante residuo all'impatto. Non ha la potenza per sfondare la compartimentazione di una nave grande. [media] [CSIS]
- Le testate dei missili sovietici (Styx ≈ 500 kg, Bazalt e Granit ≈ 750–1000 kg, Moskit ≈ 300 kg) sono in [doc 05](05_missili_e_contromisure.md) §3.1.
- **Danni tipici di un singolo colpo da 165 kg su una fregata o un cacciatorpediniere da 3000–4800 t:** foro d'ingresso di 1–5 m, distruzione dei locali entro ~10 m, rottura del collettore antincendio e della rete elettrica. **L'incendio**, più dell'esplosione, decide l'esito (Sheffield, Stark). [alta]
- **Testate da 500–750 kg** (Styx, Granit) su navi da 2000–5000 t: un colpo può bastare ad affondare (Eilat: 3 Styx, affondata ~1 h dopo il primo colpo). [alta] [WP‑Eilat]

### 4.2 Carburante residuo del missile e incendi
- **Stark (17/5/1987):** il primo Exocet **non esplose** ma sparse ~135 kg (300 lb) di propellente in fiamme in un alloggio. Il secondo esplose 20–30 s dopo e innescò il resto. In tutto bruciarono ~270 kg (600 lb) di propellente dei due missili. Temperature indicate in "1400 gradi" (unità non specificata). L'alluminio della sovrastruttura si deformò e le scale fusero. Fumo accecante in gran parte della nave; maschere OBA con 22 minuti d'aria invece di 60. Sbandamento massimo di 16°. Incendio domato in **circa 24 ore**. 37 morti. "Sarebbe quasi certamente affondata" senza l'aiuto esterno. [alta] [CSIS]; [WP‑Stark]
- **Sheffield (4/5/1982):** Exocet AM39 a dritta, 2,4 m sopra il galleggiamento, nella zona di sala macchine prodiera e locale ausiliari. L'MOD nel 2015 ha concluso che la testata **esplose**. Rete elettrica e collettore antincendio fuori uso. L'equipaggio combatté il fuoco per **quasi 4 ore**, poi abbandonò la nave. Rimorchiata e alla deriva per **6 giorni**; l'allagamento lento dal foro la sbandò a dritta, finché si **capovolse e affondò il 10/5**. 20 morti. 4820 t. [alta] [WP‑Sheff]
- **Atlantic Conveyor (25/5/1982):** due Exocet. Incendio incontrollabile alimentato dal propellente residuo e dal carico (carburante, munizioni, materiale incendiario). Persi tutti gli elicotteri tranne un Chinook; 12 morti. Bruciò fino a esaurirsi, fu presa a rimorchio, **si spezzò in due e affondò il 28/5**. [alta] [WP‑AC]
- **Tempi tipici d'incendio per il gioco** [media]: fregata o cacciatorpediniere colpito da un missile da 165 kg → incendio grave per 4–24 h; senza collettore antincendio la nave è persa in 4–6 h (Sheffield). Con aiuto esterno si salva (Stark). Mercantile con carico combustibile: brucia per 1–3 giorni, poi affonda o resta un relitto.

### 4.3 Fumo: colore e materiale [bassa per il colore come indicatore]
- Il colore del fumo dipende dal **rapporto vapore/fuliggine**, dalla ventilazione e dalla temperatura più che dal materiale. Qualsiasi incendio può mostrare tutte le tinte dal bianco al nero; nelle strutture compaiono anche il marrone e il giallo. [media] [FE‑smoke]
- **Regole di rendering consigliate** [bassa, sintesi]:

| Sorgente | Colore | Note |
|---|---|---|
| Gasolio, olio combustibile, greggio | **Nero denso**, pennacchio alto e scuro | Fuliggine ≈ 13% della massa bruciata (greggio, fuochi > 2 m) [alta] [NIST‑ISB]; la combustione efficiente del greggio dà in massa circa 75% CO₂, 12% vapore, 10% fuliggine, 3% CO [media] [ISB‑guide] |
| Vernici, cavi, arredi, plastica | Grigio‑nero, a sbuffi | [bassa] |
| Propellente solido di missile | Bianco‑grigio denso (come il booster dell'Harpoon, doc 05 §6.1) | [bassa] |
| Vapore (caldaie squarciate, acqua sul fuoco, acqua di mare su metallo rovente) | **Bianco**, si dissolve in fretta | [media] |
| Esplosione dei depositi munizioni | Lampo, poi **fungo** nero e grigio alto centinaia di metri (Yamato, Hood) | [alta per il fenomeno] |
| Alluminio o magnesio in combustione | Fiamma bianca abbagliante, fumo bianco | [bassa] |
| Gas di sfiato di un'esplosione subacquea bassa | Corona scura grigio‑nera (§2.1) | [alta] [Snay56] |

---

## 5. Petroliere e mercantili

### 5.1 Tanker War 1980–1988: numeri [media] — [CSIS]; [AC]; [Strauss]; [USNI88]
- **Attacchi:** circa **451** in tutto (Iraq 283, Iran 168), secondo la tabella USNI su dati Lloyd's e CDI; le fonti divergono. Tra il 1984 e l'ottobre 1987: 353 attacchi, di cui 217 con missili aria‑superficie. [media]
- **Esiti** (O'Rourke 1988): **340** mercantili danneggiati (alcuni più volte), **11 affondati**, circa **tre dozzine** dichiarati perdita totale (CTL). [media] [AC]
- **Petroliere:** il 61% delle navi attaccate. Delle 239 petroliere colpite, **55 (23%)** affondate o CTL, contro il 39% delle portarinfuse e il 34% dei mercantili generici. [media] [Strauss]
- **Exocet 1984:** 52 colpi su 53 lanci secondo alcuni rapporti, con almeno 2 inesplosi. Su 17 petroliere sopra le 50 000 t colpite nel 1984, 11 ebbero danni seri e 6 lievi, ma **nessuna affondò per un solo Exocet**. Delle 33 navi piccole e medie: 7 affondate, 19 danneggiate gravemente, 7 lievemente. [media] [CSIS]
- **Morti** (1984–87): 116 uccisi, 167 feriti, 37+ dispersi; 216 attacchi senza vittime. [media] [CSIS]

### 5.2 Perché le petroliere cariche sono difficili da affondare [media]
1. **Rapporto tra testata e nave:** 165 kg contro 100 000–400 000 t. Il foro (≈ 20 m² su *Safina al‑Arab*) interessa 1–3 cisterne su 20–30; la riserva di galleggiabilità resta enorme. [CSIS]
2. **Il greggio pesa poco meno dell'acqua:** se allaga una cisterna piena, la nave perde poca spinta (si scambia olio con acqua).
3. **Il greggio non brucia facilmente in massa:** serve vapore e ossigeno; l'olio "smorza" l'effetto della testata se manca l'ossigeno. [CSIS] Sul mare servono 2–3 mm di spessore per accenderlo. [ISB‑guide]
4. **Compartimentazione fitta e senza aperture:** molte cisterne, poche porte.
5. **Cause di perdita reali:** incendio negli alloggi o in sala macchine; nave troppo sofisticata per valere la riparazione (*Safina al‑Arab*, automatizzata, demolita). [CSIS]

**Esempi** [media] [CSIS]:
- *Safina al‑Arab* (357 000 t, carica): un missile aprì un foro di **22 m²** (240 ft²); incendio nelle cisterne 9–11 per **2 giorni**, 10 000 t di greggio bruciate; CTL.
- *Al‑Ahood* (118 000 t, carica): missile negli alloggi vicino alla sala macchine; incendio di **5 giorni**, 34 000 t bruciate.
- *B. T. Investor* (163 000 t): Exocet in una cisterna di sinistra appena sopra il galleggiamento; nessun incendio, danni trascurabili.
- *Bridgeton* (≈ 400 000 t): mina a prua; proseguì con i propri mezzi fino al Kuwait. [WP‑Earnest]

### 5.3 Petroliera in zavorra: vapori ed esplosioni [media]
- Le cisterne vuote ma non inertizzate contengono miscele vapore‑aria spesso **nel campo d'infiammabilità**. Il colpo può provocare **esplosioni di cisterna**, ben più distruttive che su una nave carica.
- Caso civile di riferimento: dicembre 1969, tre petroliere da 210 000 tpl (*Marpessa*, *Mactra*, *Kong Haakon VII*) esplosero durante il lavaggio delle cisterne centrali. La *Marpessa* **affondò**; sulla *Kong Haakon VII* l'esplosione **sollevò l'intero fasciame del ponte**, abbatté le paratie interne e gonfiò lo scafo di ~1,2 m. Causa indicata: elettricità statica dei getti di lavaggio. Ne seguirono gli impianti a gas inerte. [media] [ICS69]; [Nature73]
- Nella Tanker War le navi in zavorra colpite bruciarono soprattutto negli alloggi e in sala macchine (*Esperanza II*, *Buyuk Hun*). [CSIS]
- **Regola di gioco:** petroliera in zavorra senza gas inerte → 20–40% di probabilità di esplosione di cisterna per colpo nella zona del carico (danno ×3, possibile rottura della trave). Con gas inerte (prassi crescente dagli anni '70–'80) → 5%. [bassa]

### 5.4 Greggio che brucia in mare e chiazze [media]
- Velocità di combustione di una pozza: **≈ 3–4 mm/min** quasi indipendente dal tipo (NIST: 0,055–0,062 mm/s); ≈ 1 mm/min per gli oli pesanti. Spessore minimo per bruciare: **2–3 mm**. Il greggio emulsionato fino al 70% d'acqua può ancora accendersi; completamente emulsionato no. [alta] [NIST‑ISB]; [ISB‑guide]
- **Per il motore:** una chiazza da 10 000 t (circa 11 000 m³) spessa 5 cm copre ≈ 0,2 km² e brucia in ≈ 15 min se è tutta accesa. In pratica brucia dove è spessa (vicino alla nave) e si spegne dove si assottiglia, quindi si vede **un anello di fiamme di qualche centinaio di metri attorno alla nave**, che dura ore o giorni finché la nave alimenta la perdita. [bassa, calcolo]
- Colore della chiazza: nera o marrone vicino alla perdita, iridescente (arcobaleno o argento) dove è sottile (< 0,001 mm). [bassa]

### 5.5 Cargo e portacontainer
- **Portarinfuse:** più vulnerabili delle petroliere (39% CTL o affondate). Le stive grandi allagate e un carico denso (minerale) fanno affondare in fretta; alcune affondarono con un solo missile (*Fidelity*, 17 000 t; *Antigoni*, 13 000 t). [media] [CSIS]; [Strauss]
- **Mercantili generici:** 34% CTL o affondati. [media] [Strauss]
- **Portacontainer e ro‑ro:** nessun dato di guerra sistematico del periodo. Unico caso: *Atlantic Conveyor* (portacontainer ro‑ro), perso per incendio. I grandi spazi senza paratie trasversali (ponti garage) favoriscono l'allagamento rapido e il capovolgimento. Possibili riferimenti civili: *Herald of Free Enterprise* 1987, *Estonia* 1994 (non verificati in questo dossier). [bassa]

---

## 6. Navi grandi: portaerei, corazzate, incrociatori nucleari

### 6.1 Portaerei
- **Ex‑USS *America* (CV‑66), 2005:** prima grande portaerei usata per prove distruttive dopo Crossroads (1946). Per circa **4 settimane** (25 giorni in mare) subì **esplosioni subacquee e di superficie con cariche piazzate** che simulavano attacchi, con strumenti di bordo. **Non fu una SINKEX con armi lanciate.** Alla fine fu **autoaffondata** con cariche il 14/5/2005; giace dritta e intera a 5140 m. I risultati restano **classificati**. [alta per i fatti] [WP‑CV66]. Gli articoli divulgativi che parlano di "siluri e missili" non sono affidabili.
- **WWII (USA):**
  - *Hornet* (CV‑8, 1942): 3 bombe, 3 siluri e 2 aerei schiantati. Poi i caccia USA tentarono di affondarla con 9 siluri (quasi tutti inesplosi) e 430 colpi da 127 mm, **senza riuscirci**. Ci vollero 4 siluri Long Lance giapponesi; si capovolse e affondò di poppa. [alta] [WP‑Hornet]
  - *Lexington* (CV‑2, 1942) e *Taihō* (1944): affondate per **vapori di benzina avio** accesi ore dopo il colpo. *Taihō*: un solo siluro, esplosione 6,5 h dopo per vapori diffusi da una ventilazione sbagliata; affondata di poppa 2 h dopo. [alta] [WP‑Taiho]
  - *Shinano* (1944): 4 siluri dell'*Archerfish*. Era incompleta, con porte stagne aperte o mancanti; si capovolse dopo **7 h 42 min**. 65 800–73 000 t. [alta] [WP‑Shinano]
- **Stima sovietica d'epoca (CIA):** **5–7 colpi** di missile (tipo Styx, cioè testate da ~500 kg) per distruggere una portaerei; **un solo colpo** nel ponte hangar potrebbe impedire il lancio degli aerei. [media] [CIA‑carrier] Altre cifre circolanti (12 colpi, 20 missili in salva) non sono verificate. [bassa]

### 6.2 Corazzate
- **Classe Iowa:** protezione subacquea a 4 compartimenti, progettata per resistere a ~317 kg di TNT (≈ 700 lb). Una fonte secondaria segnala un declassamento perché la paratia pesante era troppo rigida. [media] [Navypedia]
- ***Yamato* (1945):** almeno **11 siluri e 6 bombe** in 1 h 46 min, concentrati a sinistra. Controallagamento a dritta fino all'esaurimento, poi capovolgimento in ~3 min. A ~120° di sbandamento esplose un deposito prodiero, con un **fungo** visibile da lontano. 3055 morti. [alta] [WP‑Yamato] (*Musashi* 1944: circa 19 siluri e 17 bombe, dato da verificare.)
- ***Barham* (1941):** 3 siluri ravvicinati (un'unica colonna d'acqua), capovolgimento a sinistra, **esplosione del deposito ~4 min dopo**. [alta] [WP‑Barham]
- ***Royal Oak* (1939):** 4 siluri in due salve; incendio di cordite; sbandamento a 15° poi a 45°; affondata **13 min** dopo la seconda salva. [alta] [WP‑RO]
- ***Hood* (1941):** colpo del *Bismarck* nei depositi poppieri, esplosione, nave spezzata; la prua si alzò quasi verticale; affondata in **~3 min**, 3 superstiti su 1418. [alta] [WP‑Hood]

### 6.3 Incrociatori nucleari e grandi unità moderne
- Nessun incrociatore nucleare è mai stato colpito o usato come bersaglio documentato. Classi *Kirov* (~25 000 t), *Long Beach*, *Virginia*: **nessun dato pubblico**. [—]
- Per analogia: dislocamento 10–25 000 t, molti compartimenti, scafo d'acciaio (Kirov) → comportamento tra l'incrociatore della WWII e la corazzata leggera. Dati in fonte aperta sui reattori danneggiati: nessuno. [bassa]

### 6.4 Quanti colpi servono, e perché [bassa, sintesi]
| Classe di nave | Siluri pesanti (≈ 300 kg, sotto la chiglia) | Missili da 165–250 kg | Missili da 500–1000 kg | Ragione |
|---|---|---|---|---|
| Corvetta o fregata < 4000 t | **1** (trave spezzata) | 1–2 (incendio decisivo) | 1 | Trave corta e leggera, pochi compartimenti, alluminio |
| Cacciatorpediniere o incrociatore 6–12 000 t | 1–2 | 2–4 | 1–2 | Trave più rigida; la compartimentazione regge |
| Mercantile 10–30 000 t | 1–2 | 2–5 | 1–2 | Stive grandi; nessun controllo danni militare |
| Petroliera > 100 000 t carica | 2–4 | raramente affonda (CTL per incendio) | 3+ | Riserva di spinta enorme; greggio ≈ acqua |
| Portaerei 60–90 000 t | 3–6 (WWII: 4 su *Shinano*, che era incompleta) | 5–10 per la messa fuori combattimento; affondamento incerto | 5–7 (stima CIA) | Doppio scafo, protezione subacquea, controllo danni |
| Corazzata Iowa | 4–8 | missione fallita salvo colpi nei depositi | 4+ | Corazza; protezione subacquea per ~317 kg |

---

## 7. Modi di affondare

### 7.1 Fattori che decidono il modo [alta per la fisica; media per l'applicazione]
- **Compartimentazione** (lunghezza allagabile): se l'allagamento resta entro 2–3 compartimenti contigui, la nave galleggia appruata o appoppata. Se supera la riserva di spinta, affonda nel senso del compartimento allagato.
- **Punto d'impatto:**
  - Al centro, sotto la chiglia → spezzata in due (§3.4).
  - Estremità → **prua o poppa asportata**, spesso con nave salva o affondamento lento (Belgrano 1° colpo; Samuel B. Roberts).
  - Fianco con allagamento asimmetrico → **sbandamento e capovolgimento** (Royal Oak, Barham, Coventry, Yamato, Shinano, Indianapolis).
  - Depositi munizioni → **esplosione e affondamento in secondi o minuti** (Hood, Juneau, Barham).
- **Stabilità: GM e superfici libere.** Correzione per superficie libera: `GM_eff = GM − Σ(ρ_liq·i_t)/(ρ_mare·∇)`, con `i_t = l·b³/12` per un compartimento rettangolare largo b e lungo l. **Un compartimento allagato a tutta larghezza riduce GM con il cubo della larghezza**: è la causa tipica del capovolgimento di navi lunghe con locali macchine larghi. [alta, architettura navale] [WP‑FSE]. Anche gli oblò aperti (Royal Oak) e le porte stagne aperte (Belgrano, Shinano) accelerano tutto. [alta]
- **Lunghezza:** le navi corte si spezzano (trave corta e leggera); quelle lunghe e grandi affondano lentamente in assetto o si capovolgono.
- **Carico e stato:** nave appesantita in alto (Indianapolis "top‑heavy") → capovolgimento rapido. Petroliera carica → galleggia; in zavorra → esplosioni (§5.3).
- **Controllo danni ed energia elettrica:** senza corrente le pompe e il collettore antincendio si fermano (Belgrano, Sheffield, Shinano, Yamato). Il ripristino della corrente salvò la Samuel B. Roberts.

### 7.2 Modi e indicazioni per l'animazione
| Modo | Che cosa si vede | Durata tipica | Note per il motore |
|---|---|---|---|
| **A. Affondamento lento in assetto** | La nave si abbassa uniforme, il ponte va a filo d'acqua, sbuffi d'aria dagli osteriggi | da 30 min a giorni | Allagamento distribuito; nave grande o petroliera |
| **B. Appruamento o appoppamento** | L'estremità colpita affonda; l'altra si alza fino a 30–90°; eliche e timone fuori dall'acqua; scivolata finale rapida | 10 min – qualche ora; scivolata finale 30 s – 2 min | Colpo all'estremità; Gustloff (di prua, < 50 min), Taihō (di poppa) |
| **C. Capovolgimento** | Sbandamento progressivo (5° → 15° → 45°), pausa, poi rotazione rapida con l'opera viva in vista; scafo rovesciato che galleggia per minuti o ore, poi affonda | 3–20 min dal colpo; scafo rovesciato 1 min – qualche ora | Allagamento laterale, GM ridotto; Royal Oak 13 min, Coventry 20 min, Barham ~4 min |
| **D. Spezzata in due** | Lampo, colonna d'acqua, nave a "V"; i tronconi affondano separati, la poppa (pesante, con le macchine) di solito per prima; spesso la prua resta a galla verticale o capovolta | poppa 1–5 min; prua minuti – ore | Siluro sotto la chiglia (Cheonan, Torrens, Racine, Ingraham); Juneau 20 s per esplosione di deposito |
| **E. Esplosione dei depositi** | Fiammata verticale, fungo di fumo, frammenti, la nave scompare | 3 s – 4 min | Hood, Juneau, Barham, Yamato (durante il capovolgimento) |
| **F. Brucia e resta a galla** | Fumo nero e alto, poi relitto annerito che va alla deriva; spesso affonda sotto rimorchio giorni dopo | ore – 6 giorni | Sheffield, Atlantic Conveyor, petroliere della Tanker War |
| **G. Prua o poppa asportata, nave salva** | Moncone con paratia di collisione esposta; nave appruata ma galleggiante | — | Samuel B. Roberts, Tripoli; Belgrano (1° colpo da solo non sarebbe bastato) |

### 7.3 Casi storici con tempi
| Nave (anno) | Disl. (t) | Arma e carica | Punto d'impatto | Modo | Tempo all'affondamento | Note | Fonte |
|---|---|---|---|---|---|---|---|
| HMS *Royal Oak* (1939) | ≈ 29 000 | 4 siluri G7e (≈ 280 kg, n.v.) | Prua, poi centro | C | **13 min** dalla 2ª salva | Incendio di cordite, oblò aperti | [WP‑RO] [alta] |
| HMS *Hood* (1941) | 47 430 | Proietto da 380 mm | Depositi poppieri | E + D | **~3 min** | Prua quasi verticale; 3 superstiti | [WP‑Hood] [alta] |
| HMS *Barham* (1941) | ≈ 31 000 | 3 siluri (U‑331) | Centro sinistra | C + E | **~4 min** | Deposito esploso a nave coricata | [WP‑Barham] [alta] |
| USS *Juneau* (1942) | ≈ 6 700 | 1 siluro (I‑26) | Vicino al deposito | E + D | **20 s** | | [WP‑Juneau] [alta] |
| USS *Hornet* (1942) | ≈ 20 000 | 3 siluri + 3 bombe + 2 aerei; poi 9 siluri USA e 4 giapponesi | Vari | C, di poppa | ore | Nave durissima da finire | [WP‑Hornet] [alta] |
| *Taihō* (1944) | ≈ 37 000 | 1 siluro (Albacore) | Prua dritta, cisterne di benzina avio | E (vapori) poi B di poppa | **8,5 h** (esplosione a 6,5 h) | Vapori diffusi dalla ventilazione | [WP‑Taiho] [alta] |
| *Shinano* (1944) | 65 800–73 000 | 4 siluri Mk 14 (Archerfish, a 3 m di profondità) | Dritta, ≈ 4,3 m sotto il galleggiamento | C, di poppa | **7 h 42 min** | Incompleta, porte aperte | [WP‑Shinano] [alta] |
| *Wilhelm Gustloff* (1945) | 25 484 GRT | 3 siluri (S‑13) | Prua, centro, macchine | C poi B di prua | **< 50 min** | Coricata a 40 min | [WP‑Gustl] [alta] |
| *Yamato* (1945) | ≈ 72 800 | ≥ 11 siluri + 6 bombe | Soprattutto sinistra | C + E | **1 h 46 min** dal primo attacco | Fungo | [WP‑Yamato] [alta] |
| USS *Indianapolis* (1945) | 9 950 (std) | 2 siluri Type 95 (I‑58) | Prua e centro, dritta | B di prua + C | **12 min** | Nave appesantita in alto | [WP‑Indy] [alta] |
| INS *Eilat* (1967) | 2 530 | 3 Styx (≈ 500 kg) | Sopra il galleggiamento, centro | A/C (sbandamento) | **~1 h** dal 1° colpo, 2 min dopo il 3° | Primo affondamento con missili | [WP‑Eilat] [alta] |
| INS *Khukri* (1971) | 1 199 | Siluro del *Hangor* (n.v.) | Sotto i depositi di nafta | D/rapido | **~2 min** | 194 morti | [WP‑Khukri] [media] |
| ARA *General Belgrano* (1982) | 12 242 | 2 Mk 8 (363 kg Torpex) | Prua; sala macchine poppiera | G + C, poi di prua | abbandono a **~20 min**; affondamento totale ~1 h [bassa] | Porte stagne aperte | [WP‑Belg] [alta] |
| HMS *Sheffield* (1982) | 4 820 | 1 Exocet AM39 (165 kg) | Dritta, 2,4 m sopra il galleggiamento | F, poi C sotto rimorchio | **6 giorni** | Incendio, collettore antincendio rotto | [WP‑Sheff] [alta] |
| HMS *Coventry* (1982) | 4 820 | 3 bombe (2 esplose) | Sinistra, appena sopra il galleggiamento | C | **< 20 min** | | [WP‑Cov] [alta] |
| *Atlantic Conveyor* (1982) | ≈ 15 000 GRT | 2 Exocet | Fianco, sala macchine | F, poi D sotto rimorchio | **3 giorni** | Si spezzò in due | [WP‑AC] [alta] |
| USS *Stark* (1987) | ≈ 4 100 | 2 Exocet (1 esploso) | Sinistra sotto la plancia | F, salva | — | Sbandamento di 16°, controallagata | [CSIS] [alta] |
| USS *Samuel B. Roberts* (1988) | ≈ 4 100 | Mina M‑08 | Sala macchine | G, salva | — | Chiglia rotta | [WP‑SBR] [alta] |
| *Safina al‑Arab* (1984) | 357 000 tpl | 1 Exocet | Cisterna 11 | F, CTL | — | Incendio di 2 giorni | [CSIS] [media] |
| ex‑HMAS *Torrens* (1999) | 2 700 | Mk 48 Mod 4 (≈ 544 kg eq.) | Sotto la chiglia, centro | D | poppa per prima, prua dopo (minuti, n.v.) | SINKEX | [WP‑Torr] [alta] |
| ROKS *Cheonan* (2010) | ≈ 1 220 | Siluro, 250 kg (JIG) | 6–9 m sotto, 3 m a sinistra, sala turbine | D | poppa in minuti; prua capovolta più a lungo | 46 morti | [WP‑Cheonan] [alta] |

---

## 8. Sottomarini colpiti

### 8.1 Rottura dello scafo resistente e soglie
- **Fattori d'urto e soglie di gioco:** già in [doc 02 §9.4](02_acustica_oceanografia_dottrina.md) (HSF; soglia ridotta con la profondità). Non si ripetono qui.
- **Dati storici WWII:**
  - Carica britannica Mk VII ad amatolo (130 kg): spacca uno scafo resistente da 22 mm a ~6 m, costringe all'emersione a ~12 m.
  - Con Torpex o Minol: ~8 m e ~16 m.
  - In generale, ~100 kg TNT contro un sottomarino da 1000 t: raggio letale 3–4 m, di avaria 8–10 m. Per il Mk 7 USA (270 kg) la rottura dello scafo richiedeva in genere ~5 m.

  [media] [WP‑DC]
- **Siluri moderni contro sottomarini:** spoletta a contatto contro lo scafo (Spearfish), oppure di prossimità acustica o magnetica. È meglio esplodere entro ~1 m dallo scafo. [media] [WP‑Spear]; [Amick20]

### 8.2 Profondità di collasso e implosione [media] — [Rule]; [WP‑Thr]; [NORSAR02]
- **Thresher (1963):** implosione a **~730 m (2400 ft)** secondo l'analisi SOSUS di Rule, contro la profondità di collasso prevista di ~590 m (1950 ft). Durata ~**0,1 s**, alle 09:18:24. Prima, al telefono subacqueo: "minor difficulties … attempting to blow", sibilo d'aria compressa, "900 N", "exceeding test depth". [media]
- **Scorpion (1968):** collasso a **~470 m (1530 ft)**; frequenza dell'impulso di bolla **4,46 Hz**; il compartimento di poppa si **telescopò di ~15 m in avanti in ~0,11 s** (~480 km/h di velocità media). [media] [Rule]
- **K‑8 classe November:** collasso a ~615 m (2020 ft) contro ~380 m previsti. [media] [Rule]
- **Regola empirica di Rule:** la profondità di collasso reale supera quella prevista in media del **44%**. Nessun evento di collasso di un sottomarino o di un suo compartimento ha superato **0,3 s**; la fase di compressione dura ~0,15 s. [media] [Rule]
- **ARA *San Juan* (2017):** implosione a **~390 m** (Rule) o **~470 m** (altre analisi CTBTO), energia **~5–6 t TNT** (5216 kg in un'analisi; 12 500 lb ≈ 5,7 t in Rule), durata 30–40 ms. Le fonti divergono. [bassa]
- **Kursk (2000):** due eventi a **135,8 s** di distanza. Il primo è ~2 magnitudo più piccolo (≈ 100–250 kg TNT, esplosione di un siluro); il secondo ML ≈ 3,5 (≈ 1–3 t TNT, detonazione di più testate). Nello spettro del secondo c'è la firma di bolla (1–6 Hz) e un riverbero d'acqua a ~9 Hz con 115 m di fondale. Se tutta l'energia fosse un'implosione, corrisponderebbe a un volume di ~4200 m³, il 20–26% del battello. [alta] [NORSAR02]
- **Energia di un'implosione (fisica di base):** `E ≈ p·V = ρ·g·h·V`. In TNT equivalente: `W ≈ p·V / 4,184·10⁶` kg. Esempi: compartimento da 1000 m³ a 470 m → ~1,1 t TNT; scafo da 3000 m³ a 730 m → ~5 t. [media, calcolo]

### 8.3 Che cosa resta in superficie
- **Gasolio e olio:** chiazza iridescente o nera che si allarga per ore o giorni (sottomarini diesel‑elettrici; anche lubrificanti e nafta dei nucleari). Criterio classico di "affondamento probabile" nella WWII, ma i comandanti degli U‑Boot **liberavano olio e detriti di proposito** per simulare la distruzione. [media] [Woodii] (fonte secondaria)
- **Bolle d'aria:** grandi bollicine iniziali (rottura delle casse e dello scafo), poi un ribollio che si esaurisce in minuti. [bassa]
- **Detriti:** legno, sughero e isolanti, materassi, indumenti, carte, corpi. Galleggiano i materiali leggeri delle sezioni rotte. [media]
- **Thresher:** il relitto fu localizzato sul fondo (2600 m) nel 1964 su ~13 ha; nessun dato di superficie solido trovato. [media] [WP‑Thr]
- **Per il motore:** probabilità di segni in superficie in funzione della quota di rottura. Per rotture sotto ~300 m: olio sì (dopo 10–60 min), detriti scarsi; per rotture vicino alla superficie: olio, aria e detriti abbondanti. [bassa]

### 8.4 Emersione d'emergenza dopo i danni
- La dottrina (esaurimento d'emergenza, avanti tutta, barra in salita) è in [doc 02 §9.2](02_acustica_oceanografia_dottrina.md).
- **Casi:**
  - *Komsomolets* (K‑278, 7/4/1989): incendio nel 7° compartimento a ~380 m. Emerse, restò a galla ~5 h e affondò; 42 morti su 69. [media] (memoria dell'autore, verificare)
  - USS *San Francisco* (2005): urtò un monte sottomarino a ~160 m a oltre 30 nodi. Emerse con l'esaurimento d'emergenza nonostante le casse prodiere squarciate; 1 morto. [media] (verificare)
  - WWII: U‑Boot e sommergibili costretti all'emersione dalle cariche di profondità (inizio di molti scontri finali col cannone). [media] [WP‑DC]
- **Per il motore:** dopo un danno sopra la soglia "allagamenti" di doc 02 §9.4 → 30–70% di probabilità di emersione d'emergenza riuscita se la quota è < 150 m e la falla è < 10 cm equivalenti. Altrimenti la discesa è incontrollata fino al collasso. [bassa]
- **Nota (25/10/2026, misure di `engine/strumenti/letalita`, docs/10 §10.17):** questa soglia decide quasi da sola chi sopravvive a un siluro leggero. Sotto i 150 m un Victor III o un Foxtrot colpito da un Mk 46 va perso in tutte le prove; sopra, la metà circa riemerge danneggiata. È la **leva** per avere sopravvissuti in profondità, ma la fonte è **[bassa]**: un solo caso noto la sfiora (*San Francisco*, esaurimento riuscito a ~160 m). Da cambiare solo con una fonte migliore, per esempio i casi di emersione dopo un siluro o una carica.

---

## 9. Rumori dopo il colpo (al sonar)

### 9.1 Esplosione [alta per la fisica]
- L'urto è un impulso di ~1 ms con livelli enormi: 300 kg a 10 km danno ~200 dB re 1 µPa di picco (§1.2). **Satura** tutti i ricevitori passivi vicini per una frazione di secondo; il riverbero (superficie e fondo) dura secondi. Con 115 m d'acqua (Kursk) la riverberazione dello strato d'acqua dà massimi a ~9,5 Hz e 15,8 Hz. [alta] [NORSAR02]
- Su lunghe distanze l'esplosione è un "colpo sordo" basso, con code SOFAR: Scorpion fu registrato a 821 nmi da un singolo idrofono alle Canarie. [media] [Rule]

### 9.2 Pulsazioni della bolla
- Dopo l'urto si sentono **1–3 tonfi più bassi** a intervalli T₁, T₂ ≈ 0,75·T₁, T₃ ≈ 0,63·T₁ (per un siluro pesante sotto la chiglia T₁ ≈ 1–1,5 s; per una carica di profondità a 50 m ≈ 0,4 s). Ciascuno ha picco circa 10 volte più basso dell'urto (§1.4). [media]
- Lo spettro mostra la **frequenza di bolla** 1/T₁ come massimo a bassa frequenza: Cheonan 1,01 Hz; Kursk‑2 1–6 Hz. [media] [KimG]; [NORSAR02] Se la bolla sfiata (d < A_max) non ci sono pulsazioni. [alta] [Snay56]

### 9.3 Rottura e "breaking‑up noises"
- Termine standard dei rapporti di pattuglia: "heavy breaking‑up noises" dopo i colpi (es. USS *Sea Dog*, 1945, rapporto di pattuglia con OCR scadente). Gli operatori sonar della WWII ascoltavano **collasso dello scafo, cedimento delle paratie, esplosioni interne**. Il silenzio improvviso di un bersaglio attivo valeva come affondamento "probabile". [media] [SeaDog]; [Woodii]
- **Composizione sonora consigliata** [bassa, sintesi]:
  1. Scricchiolii e gemiti metallici (lamiere e trave che si deformano) per 10 s – qualche minuto.
  2. Sibili e gorgoglii d'aria e acqua (compartimenti che si allagano, aria che esce dagli sfoghi).
  3. Tonfi o colpi secchi quando una paratia o un serbatoio cede.
  4. Esplosioni secondarie (caldaie, munizioni) con i loro impulsi di bolla.
  5. Sibilo del vapore se le caldaie sono aperte.
- **Thresher:** il *Skylark* sentì aria compressa e poi un rumore ad alta energia e bassa frequenza, tipico di un'implosione; poi silenzio. [media] [WP‑Thr]; [SSDS]

### 9.4 Compartimenti che implodono durante l'affondamento
- Una nave di superficie che affonda porta giù sacche d'aria in compartimenti chiusi. Le paratie e i ponti non sono strutture a pressione, quindi cedono già a **qualche decina di metri** con un colpo sordo; i serbatoi e i locali più robusti cedono più in basso. [bassa, fisica]
- **Sottomarino che affonda:** i compartimenti ancora asciutti implodono uno dopo l'altro alla profondità di collasso o oltre, ciascuno in ≤ 0,1–0,3 s, con una sua frequenza di bolla (Scorpion 4,46 Hz). Seguono rumori minori (serbatoi, siluri, bombole) fino al fondo. Per Scorpion: "un colpo, poi dopo ~90 s altri brontolii", 5–8 eventi in circa 3 min 12 s. [media] [Rule]; [Carlson] (la sequenza esatta varia tra le fonti)
- **Per il motore:**
  - Sottomarino: n eventi = numero di compartimenti stagni ancora asciutti (2–3 per il 688; 6–8 per i doppi scafi sovietici, vedi doc 02 §9.3), alla quota di collasso × (1–1,4), a intervalli di 1–30 s mentre il relitto scende a 5–15 m/s. [bassa]
  - Nave di superficie: 3–10 tonfi tra 20 e 300 m di profondità. [bassa]

---

## 10. Comando di detonazione via filo

### 10.1 Fatto documentato [media]
- **Mk 45 ASTOR** (USA, nucleare, filoguidato, anni '60–'70): "la testata veniva fatta esplodere **solo da un segnale inviato lungo il filo**"; non aveva spolette a contatto o d'influenza. È l'unico caso pubblico chiaro di detonazione a comando via filo su un siluro USA. [alta] [NW‑US]
- **Mk 48 (Mod 1/3/4) e ADCAP (Mod 5):** possono operare con o senza filo. Dal Mod 3 c'è la comunicazione bidirezionale TELCOM: sonar e dati di corsa, rotta, velocità e quota verso il battello. Nessuna fonte pubblica cita un **comando di detonazione** per il Mk 48. Le spolette indicate sono di prossimità o a contatto. [media] [NW‑US]; [WP‑Mk48]
- **Funzioni del filo documentate in fonte aperta:** guida a metà corsa, cambio della geometria d'attacco, **spegnimento** (*shutdown*) su ordine dell'operatore; se il filo si rompe, il siluro segue l'ultimo comando ricevuto. Aaron Amick (ex sommergibilista, The War Zone 2020) scrive esplicitamente che un siluro **non può essere "command‑detonated"** e cita la scena di *The Hunt for Red October* come errore cinematografico. [media] [Amick20]
- **Tigerfish (Mk 24), TEST‑71, Spearfish, DM2A4, F21:** le schede pubbliche indicano spoletta a contatto o di prossimità (acustica per il colpo sotto la chiglia); **nessuna menzione di detonazione via filo**. [media] [WP‑Spear]; [WP‑DM2A4]; ricerca negativa su Tigerfish e TEST‑71.

### 10.2 Convenzione dei videogiochi [bassa]
- **Cold Waters** (Killerfish 2017): le funzioni del filo documentate dalle guide dei giocatori sono "taglia filo" e "attiva siluro" (cambiare le impostazioni del tubo con il filo integro le applica subito al siluro). **Non ho trovato** un comando "detona". [bassa] [CW‑guide]
- **Dangerous Waters / Sub Command** (Sonalysts): non verificato. Una funzione di detonazione o autodistruzione è spesso ricordata dai giocatori, ma non l'ho trovata in fonte consultabile. [bassa]
- **Raccomandazione per il remake (ambientazione 1989):** offrire via filo **guida, abilitazione o ricerca, cambio di quota o velocità e spegnimento (shutdown)**, non la detonazione. Se si vuole un "detona", presentarlo come **opzione arcade** o come licenza dichiarata. [bassa, design]

---

## 11. Numeri per il motore

Tutti i valori presuppongono W in kg di **TNT equivalente per la bolla** (W_bolla = massa × RBE: Mk 48 ≈ 1,84; Torpex o HBX ≈ 1,5; H‑6 1,69; TNT 1). Per l'urto e il KSF si usa W_urto (≈ 1,1–1,2 × massa per gli alluminizzati).

### 11.1 Formule da codificare
| Grandezza | Formula | Fiducia |
|---|---|---|
| Picco d'urto | `P = 52,4·(W^(1/3)/R)^1,13` MPa | alta |
| Costante di tempo | `θ = 0,084·W^(1/3)·(W^(1/3)/R)^(−0,23)` ms | alta |
| 1° periodo di bolla | `T₁ = 2,11·W^(1/3)/(d+10)^(5/6)` s; `T₂ = 0,74·T₁`; `T₃ = 0,63·T₁` | alta / media |
| Raggio massimo | `A = 3,50·W^(1/3)/(d+10)^(1/3)` m | alta |
| Migrazione al 1° minimo | `m = 12,2·W^(1/2)/(d+10)` m | media |
| Energia della bolla per ciclo | 47%, 17%, ~7% dell'energia totale | media |
| Velocità iniziale della cupola | `v₀ = 86·W^(1/3)/d − 6,6` m/s (se 0,23 < W^(1/3)/d < 3,1; altrimenti 0 o "esplosione di superficie") | media |
| Altezza della cupola | `H_d = 0,45·v₀²/(2g)`; tempo all'apice ≈ `2·H_d/v₀`; ricaduta 2–5 s | bassa |
| Pennacchio | Tab. §2.4(a) con y = d/W^(1/4); scala con W^(1/3); ±50% casuale | media/bassa |
| Istante del pennacchio | `t = T₁` (impulso di bolla); salita `≈ sqrt(2H/g)·0,8`; ricaduta 5–10 s; nebbia 15–40 s | bassa |
| Regime | d/A < 1 → colonna scura che sfiata; 1–3 → pennacchi bianchi; 3–5 → cupola bassa e chiazza; > 5 → solo gobba e chiazza schiumosa | media |
| Anello "slick" | `r = sqrt((1500·t)² − d²)` fino a `≈ 18·W^(1/3)` m, durata < 0,3 s | alta (geometria) / bassa (estensione) |
| Falla a contatto | `D ≈ 1,25·W^(1/3)` m (±20%), sotto il galleggiamento | bassa |
| Frequenza a 2 nodi | `f = (1,2·10⁵/60)·sqrt(B·D³/(Δ(1,2+B/3d)·L³))` Hz | media |
| Energia d'implosione | `W ≈ ρ·g·h·V/4,184·10⁶` kg TNT | media |

### 11.2 Tabella compatta: aspetto delle esplosioni
| Scenario | W_bolla | d (m) | Regime | Cupola: v₀ / H (m) | Pennacchio H × diam. (m) | Istante del pennacchio (s) | Durata visibile (s) | Fiducia |
|---|---|---|---|---|---|---|---|---|
| Mk 48 sotto chiglia | 544 | 8–12 | sfiato o getto | 50–80 / 60–150 | 160–215 × 180–210 | 1,3–1,5 | 20–40 | bassa |
| Siluro sovietico 53‑65 / USET‑80 | 300–450 | 8–12 | getto o pennacchi | 40–65 / 40–100 | 130–180 × 170–200 | 1,1–1,3 | 20–35 | bassa |
| Tigerfish (Torpex 134–340 kg) | 200–500 | 8–12 | getto o pennacchi | 35–60 | 110–170 × 150–200 | 1,0–1,3 | 20–35 | bassa |
| Carica di profondità (100–200 kg) a 15 m | 135–270 | 15 | pennacchi | 23–30 / 10–20 | 85–115 × 120–170 | 0,7–0,9 | 10–25 | bassa |
| idem a 30 m | 135–270 | 30 | profonda | — | 30–60 × 30–80 | 0,5–0,6 | 5–15 | bassa |
| idem a 50–100 m | 135–270 | 50–100 | profonda | — | gobba di 2–5 m, chiazza Ø 30–60 m | — | 30–120 (chiazza) | bassa |
| Razzo RBU‑6000 a 10 m | 25 | 10 | pennacchi | ≈ 19 / ≈ 8 | ≈ 50 × 70 | 0,5 | 5–10 | bassa |
| RBU a 50+ m | 25 | ≥ 50 | profonda | — | invisibile; chiazza di bolle Ø 10–20 m | — | 20–60 | bassa |
| Mina a contatto | 145 | 5–7 | getto o sfiato | ≈ 60 | ≈ 110 × 130 | ≈ 1 | 15–30 | bassa |
| Mina da fondo in 20 m | 600–1000 | 20 | pennacchi + fango | ≈ 30 | 145–180 × 210–250, acqua marrone | ≈ 1 | 20–40; chiazza marrone per minuti | bassa |
| Missile antinave sopra il galleggiamento | — | — | — | — | lampo e palla di fuoco Ø 10–20 m, fumo nero o grigio | 0 | fumo per ore | bassa |
| Nucleare (nota): 10–20 kt basso | — | 30–50 | colonna | — | 1500–1800 × 600, base surge 4–6 km | — | minuti | media |

### 11.3 Tabella compatta: danni
| Parametro | Valore consigliato | Fiducia |
|---|---|---|
| Rottura in due (siluro sotto chiglia, ±0,25 L dal centro, distanza ≤ 1,5 A) | L < 130 m: certa con W_bolla ≥ 150 kg; 130–200 m: 60–80% con W_bolla ≥ 400 kg; > 200 m: < 10% | bassa |
| Fuori dal centro nave | prua o poppa asportata (lunghezza persa ≈ 10–15 m, 0,05–0,1 L) | bassa |
| Colpo a contatto sul fianco | falla `1,25·W^(1/3)` m; 1–3 compartimenti allagati; 10–30% di probabilità di incendio | bassa |
| Missile da 165 kg | foro 1–5 m; incendio grave 80% su navi militari < 5000 t; 20–40% su petroliere cariche | media/bassa |
| Missile da 500–1000 kg | danni ×3–5; un colpo affonda < 3000 t nel 50–80% dei casi | bassa |
| Probabilità di esplosione dei depositi (colpo nella zona dei depositi) | 10–30% WWII senza allagamento; 2–5% navi moderne con allagamento automatico | bassa |
| Petroliera in zavorra senza gas inerte | 20–40% di esplosione di cisterna per colpo nella zona del carico | bassa |

### 11.4 Tempi tipici di affondamento (per estrarre il tempo nel gioco)
| Tipo di nave | Modo | Tempo (mediana; intervallo) | Base |
|---|---|---|---|
| Corvetta o fregata spezzata in due | D | poppa 2 min (0,3–5); prua 15 min (2–120) | Cheonan, Torrens, Juneau |
| Fregata o cacciatorpediniere, falla laterale grave | C | 15 min (3–30) | Coventry, Royal Oak, Barham |
| Fregata o cacciatorpediniere, incendio da missile | F | 1–6 giorni o salva (50%) | Sheffield, Stark, Glamorgan |
| Incrociatore, 1–2 siluri | B o C | 20–60 min | Belgrano, Indianapolis 12 min |
| Mercantile 10–20 000 t, 1–2 siluri | B o A | 10 min – 2 h | Gustloff < 50 min |
| Portarinfuse con carico denso | B rapida | 2–10 min | stima (Tanker War: 7/33 piccole affondate) |
| Petroliera carica | F, raramente affonda | incendio 2–5 giorni; CTL | Safina al‑Arab, Al‑Ahood |
| Portaerei, 3–4 siluri | C | 4–8 h | Shinano 7 h 42 min |
| Corazzata, 10+ siluri | C + E | 1–2 h di attacco, capovolgimento in ~3 min | Yamato |
| Esplosione dei depositi | E | 3 s – 4 min | Hood 3 min, Juneau 20 s |

---

## 12. Bibliografia

**Testi e rapporti tecnici**
- **[Cole48]** R. H. Cole, *Underwater Explosions*, Princeton University Press 1948 (Dover 1965). Non consultato direttamente: costanti riprese da [Sw78] e [Snay56].
- **[Sw78]** M. M. Swisdak Jr. (a cura di), *Explosion Effects and Properties: Part II – Explosion Effects in Water*, NSWC/WOL TR 76‑116, Naval Surface Weapons Center, 1978. Tab. 2 (similitudine), Tab. 4‑1 (equivalenze), Tab. 10 (bolla), Tab. 12 (oscillazioni), capp. 13–14 (colonne e pennacchi). Copia EPA: https://semspub.epa.gov/work/01/550573.pdf (DTIC blocca il download automatico).
- **[Snay56]** H. G. Snay, "Hydrodynamics of Underwater Explosions", *Symposium on Naval Hydrodynamics*, NAS‑NRC Publ. 515, 1957. https://www.onr.navy.mil/media/document/hydrodynamics-underwater-explosions
- **[Kol49]** H. Kolsky, J. P. Lewis, M. T. Sampson, A. C. Shearman, C. I. Snow, "Splashes from underwater explosions", *Proc. R. Soc. A* 196, 379 (1949), doi:10.1098/rspa.1949.0034. Abstract tramite Crossref: https://api.crossref.org/works/10.1098/rspa.1949.0034
- **[Reid96]** W. D. Reid, *The Response of Surface Ships to Underwater Explosions*, DSTO‑GD‑0109, 1996. https://everyspec.com/USN/NAVY-General/DSTO-GD-0109_42397/ (anche https://apps.dtic.mil/sti/pdfs/ADA326738.pdf)
- **[Brain16]** B. C. V. Brainard, *An Underwater Explosion‑Induced Ship Whipping Analysis Method for use in Early‑Stage Ship Design*, tesi MS, Virginia Tech 2016. https://vtechworks.lib.vt.edu/handle/10919/64508
- **[Kumai57]** T. Kumai, "Analysis of the factor β in Todd's formula…", *Reports of RIAM Kyushu Univ.* 5(17), 1957. https://catalog.lib.kyushu-u.ac.jp/opac_download_md/7160859/0517_p009.pdf
- **[DT23]** studio di *Defence Technology* (2023) su sagging e hogging da bolla. https://doaj.org/article/7c94f13334934d9a8b56b4da152b6d14 (solo abstract)
- **[USP8264909]** brevetto USA 8 264 909 (forma di Cole del periodo, K = 4,36). https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/8264909
- **[GD77]** S. Glasstone, P. J. Dolan, *The Effects of Nuclear Weapons*, 1977, cap. 2 (Baker). https://www.abomb1.org/nukeffct/enw77b2.html ; https://atomicarchive.com/resources/documents/effects/glasstone-dolan/chapter2.html
- **[IMWS]** "The introduction of castable plastic bonded explosives, PBX's, in Navy munitions". https://www.insensitivemunitions.org/history/the-introduction-of-castable-plastic-bonded-explosives-pbxs-in-navy-munitions/
- **[NIST‑ISB]** NIST IR 5266, combustione di greggio in vasche. https://nvlpubs.nist.gov/nistpubs/Legacy/IR/nistir5266.pdf
- **[ISB‑guide]** Linee guida sull'*in‑situ burning*: https://www.nrt.org/sites/2/files/isb%20aug%2092.pdf ; Environment Canada: https://bac-lac.wayback.archive-it.org/web/20060208011348/http:/www2.ec.gc.ca/science/sharedgifs/boct01_e.pdf
- **[FE‑smoke]** "The Art of Reading Smoke: Understanding the Color", *Fire Engineering*. https://www.fireengineering.com/firefighter-training/the-art-of-reading-smoke-understanding-the-color/
- **[ICS69]** ICS, *Third interim report on explosions in very large tankers*. https://trid.trb.org/View/9997 ; **[Nature73]** commento su *Nature* 241, 196 (1973). https://www.nature.com/articles/241196a0

**Inchieste e casi**
- **[WP‑Cheonan]** https://en.wikipedia.org/wiki/ROKS_Cheonan_sinking (sintesi del rapporto JIG del 20/5/2010 e delle tesi alternative). Replica del JIG: https://nautilus.org/supporting-documents/response-to-rush-to-judgment-inconsistencies-in-south-koreas-cheonan-report/
- **[KimG]** S. G. Kim, Y. Gitterman, S. Rodríguez, analisi idroacustica e sismica del Cheonan (*Methods in Oceanography* 2014/15; *Pure Appl. Geophys.* 2013). https://cris.bgu.ac.il/en/publications/underwater-explosion-uwe-analysis-of-the-roks-cheonan-incident-2/
- **[NORSAR02]** J. Schweitzer, "Some results derived from the seismic signals of the accident of the Russian submarine Kursk", NORSAR Sci. Rep. 1‑2002. https://www.norsar.no/getfile.php/1318125-1681727910/norsar.no/R-D/Publications/4877_Schweitzer2002.pdf
- **[Rule]** B. Rule, "Apparent failure mode of the pressure hull of the Israeli submarine Dakar", *Submarine Review* (NSL). https://archive.navalsubleague.org/?p=5598 ; Scorpion: https://archive.navalsubleague.org/?p=6379 ; San Juan: https://www.infodefensa.com/latam/2018/01/11/noticia-retired-analyst-office-naval-intelligence-could-solved-mystery.html
- **[Carlson]** M. Carlson, ricostruzione degli eventi acustici dello Scorpion. https://warfarehistorynetwork.com/?p=39507
- **[CSIS]** A. H. Cordesman, *The Lessons of Modern War, vol. II: The Iran‑Iraq War*, cap. XIV "The Tanker War…" (incl. USS Stark). https://csis-website-prod.s3.amazonaws.com/s3fs-public/legacy_files/files/media/csis/pubs/9005lessonsiraniraqii-chap14.pdf
- **[USNI88]** R. O'Rourke, "The Tanker War", *Proceedings*, maggio 1988. https://www.usni.org/magazines/proceedings/1988/may/tanker-war (non consultato direttamente; cifre tramite [AC] e [CSIS])
- **[AC]** Atlantic Council, *What attacks on shipping mean for the global maritime order*. https://www.atlanticcouncil.org/in-depth-research-reports/report/what-attacks-on-shipping-mean-for-the-global-maritime-order/
- **[Strauss]** Strauss Center, *Strait of Hormuz – Tanker War*. https://www.strausscenter.org/strait-of-hormuz-tanker-war/
- **[CIA‑carrier]** CIA FOIA, stima sovietica dei colpi necessari contro una portaerei. https://www.cia.gov/readingroom/print/2062265
- **[USNI91]** "We still haven't learned", *Proceedings*, luglio 1991 (Tripoli, Princeton). https://www.usni.org/magazines/proceedings/1991/july/we-still-havent-learned ; **[Gulflink]** cronologia USN. https://gulflink.health.mil/histories/db/navy/usnavy_183.html
- **[DANFS‑SBR]** NHHC, *Samuel B. Roberts III (FFG‑58)*. https://www.history.navy.mil/research/histories/ship-histories/danfs/s/samuel-b--roberts--ffg-58--iii.html
- **[WDR‑Sara]**, **[WDR‑North]** BuShips War Damage Reports n. 19 (Saratoga) e n. 41 (Northampton). https://history.navy.mil/research/library/online-reading-room/title-list-alphabetically/w/war-damage-reports/uss-saratoga-cv3-war-damage-report-no-19.html ; https://history.navy.mil/research/library/online-reading-room/title-list-alphabetically/w/war-damage-reports/uss-northampton-ca26-war-damage-report-no-41.html (dimensioni riprese da estratti di ricerca: sito irraggiungibile in automatico)
- **[NAVSEA‑Brem]** NAVSEA, documento d'inattivazione del USS Bremerton (New Carissa). https://www.navsea.navy.mil/Portals/103/Documents/TeamShips/SEA21/InactiveShips/Historic/2018/BREMERTON(SSN698)Final-DOI22Mar2019.pdf
- **[TWZ‑Racine]** https://www.twz.com/22184/watch-uss-recine-get-pummeled-to-death-during-rimpac-2018-sinking-exercise ; **[MarEx]** https://maritime-executive.com/article/video-lst-torpedoed-and-sunk-at-rimpac-2018
- **[IE‑Ingraham]** https://interestingengineering.com/culture/watch-a-torpedo-destroy-ex-uss-ingraham-frigate
- **[Torr‑plume]** archivio speculare di una presentazione con la foto del Mk 48 contro il Torrens ("circa 150 m"; fonte debole). https://wikipedia2006.classicistranieri.com/en/m/a/r/Image%7EMark_48_Torpedo_testing.jpg_f4a9.html
- **[NM‑Bridg]** https://navalminewarfare.com/2026/07/27/on-this-day-24-july-1987/
- **[Navypedia]** https://navypedia.org/ships/usa/us_bb_iowa.htm
- **[SeaDog]** rapporti di pattuglia del USS Sea Dog (SS‑401). https://www.scribd.com/doc/175592261/SS-401-Sea-Dog ; **[SSDS]** *Silent Sentinel*, aprile 2018 (Thresher e Skylark). https://www.ussvisandiego.org/Sentinels/SS_04_2018.pdf
- **[Woodii]** "The Depth Challenge" (valutazione degli affondamenti nella WWII; secondaria). https://chuckwoodii.substack.com/p/the-depth-challenge

**Armi e giochi**
- **[NW‑US]** NavWeaps, *Post‑WWII Torpedoes of the USA* (Mk 45, Mk 48). https://www.navweaps.com/Weapons/WTUS_PostWWII.php
- **[Amick20]** A. Amick, "Modern submarine torpedo attacks are nothing like what you see in the movies", *The War Zone*, 2020. https://www.twz.com/33018/modern-submarine-torpedo-attacks-are-nothing-like-what-you-see-in-the-movies
- **[CW‑guide]** *Cold Waters Tube Settings Guide*. https://www.magicgameworld.com/cold-waters-tube-settings-guide/

**Wikipedia (solo come appoggio)**
- **[WP‑UE]** https://en.wikipedia.org/wiki/Underwater_explosion · **[WP‑DC]** https://en.wikipedia.org/wiki/Depth_charge · **[WP‑Mk48]** https://en.wikipedia.org/wiki/Mark_48_torpedo · **[WP‑Spear]** https://en.wikipedia.org/wiki/Spearfish_torpedo · **[WP‑DM2A4]** https://en.wikipedia.org/wiki/DM2A4 · RBU‑6000: https://en.wikipedia.org/wiki/RBU-6000
- **[WP‑Torr]** https://en.wikipedia.org/wiki/HMAS_Torrens_(DE_53) · **[WP‑Ager]** https://en.wikipedia.org/wiki/USS_Agerholm · **[WP‑Belg]** https://en.wikipedia.org/wiki/ARA_General_Belgrano · **[WP‑Sheff]** https://en.wikipedia.org/wiki/HMS_Sheffield_(D80) · **[WP‑Stark]** https://en.wikipedia.org/wiki/USS_Stark_incident · **[WP‑AC]** https://en.wikipedia.org/wiki/SS_Atlantic_Conveyor · **[WP‑Cov]** https://en.wikipedia.org/wiki/HMS_Coventry_(D118) · **[WP‑Eilat]** https://en.wikipedia.org/wiki/INS_Eilat_(1955) · **[WP‑Khukri]** https://en.wikipedia.org/wiki/INS_Khukri_(F149) · **[WP‑SBR]** https://en.wikipedia.org/wiki/USS_Samuel_B._Roberts_(FFG-58) · **[WP‑Earnest]** https://en.wikipedia.org/wiki/Operation_Earnest_Will
- **[WP‑CV66]** https://en.wikipedia.org/wiki/USS_America_(CV-66) · **[WP‑Hornet]** https://en.wikipedia.org/wiki/USS_Hornet_(CV-8) · **[WP‑Taiho]** https://en.wikipedia.org/wiki/Japanese_aircraft_carrier_Taihō · **[WP‑Shinano]** https://en.wikipedia.org/wiki/Japanese_aircraft_carrier_Shinano · **[WP‑Yamato]** https://en.wikipedia.org/wiki/Japanese_battleship_Yamato · **[WP‑Hood]** https://en.wikipedia.org/wiki/HMS_Hood · **[WP‑Barham]** https://en.wikipedia.org/wiki/HMS_Barham_(04) · **[WP‑RO]** https://en.wikipedia.org/wiki/HMS_Royal_Oak_(08) · **[WP‑Juneau]** https://en.wikipedia.org/wiki/USS_Juneau_(CL-52) · **[WP‑Indy]** https://en.wikipedia.org/wiki/USS_Indianapolis_(CA-35) · **[WP‑Gustl]** https://en.wikipedia.org/wiki/Wilhelm_Gustloff_(ship)
- **[WP‑Thr]** https://en.wikipedia.org/wiki/USS_Thresher_(SSN-593) · **[WP‑FSE]** https://en.wikipedia.org/wiki/Free_surface_effect

---

## 13. Lacune note (da colmare)
1. **Formula empirica dell'altezza e dei tempi della cupola** (Young, NSWC; Cole cap. 10): non trovata in fonte accessibile. La regola `0,45·v₀²/2g` è tarata su un solo punto nucleare (Wahoo).
2. **Tempi dei pennacchi** (salita e ricaduta) per cariche convenzionali: nessuna tabella pubblica trovata; i valori di §2.4(c) sono balistici.
3. **PBXN‑103 e testate sovietiche:** nessuna tabella pubblica di RBE ed equivalenze d'urto. Per il Mk 48 c'è solo "295 kg ≈ 544 kg TNT" (NavWeaps); il Torpex non compare in [Sw78].
4. **Dimensione della falla in funzione di carica e spessore** (Keil 1956; Rajendran e Narasimhan 2001): la formula non è stata letta sull'originale. Il fit `1,0–1,5·W^(1/3)` è dell'autore su 5 casi.
5. **Costanti di Schlick e Kumai (3,07·10⁶)** per la frequenza a 2 nodi: non verificate. È verificata solo la forma di Todd con β ≈ 1,15–1,29·10⁵ (cargo).
6. **Soglie di rottura della trave per le navi militari:** nessun dato pubblico; la regola di §3.4 è una taratura sui casi.
7. **Tempo di affondamento del Belgrano** e tempi dei tronconi del Torrens e del Cheonan: dati discordi o assenti.
8. **War Damage Reports USN** (dimensioni delle falle con testate note): sito NHHC irraggiungibile in automatico; dimensioni prese da estratti.
9. **Firme acustiche dei "breaking‑up noises"** (spettri, durate): solo descrizioni qualitative; nessuna registrazione declassificata analizzata.
10. **Comando di detonazione via filo** per Tigerfish, TEST‑71, 53‑65 e simili: nessuna fonte pubblica. Per Dangerous Waters e Sub Command la presenza del comando non è verificata.
11. **Incrociatori nucleari, Kirov, portaerei moderne:** nessun dato pubblico di vulnerabilità; i risultati della prova sull'ex‑America sono classificati.
12. **Komsomolets e San Francisco** (§8.4): riportati a memoria, da verificare.
