# 02 — Acustica subacquea, oceanografia e dottrina (dossier di riferimento)

Progetto: remake realistico di *688 Attack Sub* (1989). Periodo: tarda Guerra Fredda, circa 1985–1991.
Scopo: dare formule con unità, costanti pronte per il codice e tabelle numeriche rappresentative, ognuna con fonte e grado di confidenza.

---

## 0. Convenzioni

- **Livelli** in dB re 1 µPa (pressione) e dB re 1 µPa²/Hz (densità spettrale, o "spectrum level"). I livelli di sorgente sono riferiti a 1 m. Le vecchie fonti USA usano dB re 1 µbar (+100 dB per convertire in re 1 µPa) o re 0,0002 dyn/cm² (+26 dB). **[alta]** (Urick 1983, cap. 1)
- **log** = log₁₀; **ln** = logaritmo naturale. Distanze: m, km, kyd (1000 yd = 914,4 m), nm (1852 m). Velocità: kt (0,5144 m/s).
- **Confidenza:** **[alta]** = formula o valore standard verificato su fonte primaria o secondaria autorevole. **[media]** = valore da letteratura aperta credibile ma non controllato sulla fonte primaria, oppure derivato dall'autore con fisica standard. **[bassa]** = stima plausibile, da fonti divulgative o da giochi, oppure interpolazione dell'autore. Le voci [bassa] **vanno trattate come parametri di taratura del gioco**, non come dati.
- I riferimenti tra parentesi quadre ([U83], [ES310] …) rimandano alla bibliografia (§11).

---

## 1. Equazione del sonar

### 1.1 Forme fondamentali [alta] — [U83] cap. 2; [ES310]; [CS76]

Eccesso di segnale (*Signal Excess*, SE). Si ha rilevazione (al 50% per definizione di DT) quando SE ≥ 0.

| Caso | Equazione |
|---|---|
| Passivo | `SE = SL − TL − (NL − DI) − DT` |
| Attivo, limitato dal rumore | `SE = SL − 2·TL + TS − (NL − DI) − DT` |
| Attivo, limitato dalla riverberazione | `SE = SL − 2·TL + TS − RL − DT_R` |
| Attivo bistatico | `SE = SL − TL₁ − TL₂ + TS_bist − (NL − DI) − DT` |

- **Figura di merito** (FOM), cioè la TL massima ammessa: passivo `FOM = SL − NL + DI − DT`; attivo `FOM = SL + TS − NL + DI − DT`, con rilevazione se `2·TL ≤ FOM`. [ES310]
- Regola pratica dei livelli: in ascolto passivo a banda larga, sia SL sia NL vanno integrati sulla stessa banda W: `NL_banda = NL_spettro + 10·log W`. Se invece SL è un livello spettrale (per Hz), si usa DT riferito a 1 Hz (vedi sotto). **[alta]** [U83]

### 1.2 Termini

**DI, indice di direttività / AG, guadagno d'antenna** [alta] [U83] cap. 3; [Waite]
- In rumore isotropo e con elementi non correlati: `AG ≈ DI`.
- Schiera lineare di N elementi a spaziatura λ/2: `DI ≈ 10·log N`. Schiera lineare continua di lunghezza L (L ≫ λ): `DI ≈ 10·log(2L/λ)`.
- Pistone circolare di diametro D: `DI ≈ 20·log(πD/λ)`.
- Larghezza del fascio a −3 dB, schiera lineare uniforme vista di traverso (*broadside*): `θ₃dB ≈ 50,8°·λ/L`. Verso l'*endfire* (lungo l'asse) il fascio si allarga approssimativamente di `1/sin(angolo dall'asse)`, fino a circa `2·sqrt(λ/L)` rad esattamente in asse. [media]
- Esempio TB‑16 (apertura 240 ft ≈ 73 m, [TB16]): a 100 Hz (λ = 15 m) DI ≈ 10·log(2·73/15) ≈ 9,9 dB e θ₃dB ≈ 10,4°; a 500 Hz DI ≈ 17 dB e θ₃dB ≈ 2,1°. [media, calcolo dell'autore]

**DT, soglia di rilevazione** [alta] [U83] cap. 12. Definita come il rapporto S/N₀ (con N₀ = rumore in 1 Hz) che dà il 50% di Pd per una Pfa fissata.
- Segnale noto, cioè correlatore o *replica* attiva, con impulso di durata t: `DT = 10·log(d/(2t))`.
- Segnale ignoto in rumore (rivelatore di energia, ascolto o *display* passivo a banda larga) su banda W e tempo di integrazione t: `DT = 5·log(d·W/t)`.
- Indice di rilevazione d, ricavato dalle curve ROC per rumore gaussiano: `d ≈ (z_Pfa − z_Pd)²`.

| Pfa | Pd = 0,5 | Pd = 0,9 |
|---|---|---|
| 10⁻² | d ≈ 5,4 | 13 |
| 10⁻⁴ | 13,8 | 25 |
| 10⁻⁶ | 22,6 | 36 |

(calcolo dell'autore con z(10⁻²) = 2,33, z(10⁻⁴) = 3,72, z(10⁻⁶) = 4,75, z(0,9) = −1,28) [media]

- Esempio passivo: W = 1000 Hz, t = 10 s, d = 14 → DT = 5·log(1400) ≈ 15,7 dB (riferito a 1 Hz), cioè S/N in banda ≈ −14 dB. [media, calcolo]
- **Differenziale di riconoscimento (RD):** è il nome storico (anni '50–'60) dello stesso concetto, cioè il S/N in banda che serve all'operatore umano per "riconoscere" il segnale il 50% delle volte. Valori operativi indicativi: ascolto in cuffia a banda larga da −3 a +3 dB in banda; *display* LOFAR da 0 a +6 dB per bin, secondo l'addestramento. **[bassa]** (ordine di grandezza da [U83] e [CS76]: è un parametro di "abilità dell'operatore" da tarare)

**PG, guadagno di elaborazione** [alta] [U83]; [Waite]
- Filtro adattato (impulso FM a larghezza di banda B e durata T): `PG = 10·log(B·T)`.
- Integrazione incoerente di M campioni indipendenti: `≈ 5·log M`.
- Analisi a banda stretta (FFT di durata T_fft, bin `b = 1/T_fft`): rispetto a un rivelatore a banda W, una tonale guadagna `10·log(W/b)` (al netto di instabilità di frequenza e Doppler).
- Doppler. Passivo: `Δf = f₀·v_r/c`, cioè 0,103 Hz/kt a 300 Hz. Attivo (andata e ritorno): `Δf = 2·f₀·v_r/c`, cioè 2,4 Hz/kt a 3,5 kHz. Sono questi i valori che permettono a un impulso CW di separare dalla riverberazione un bersaglio con velocità radiale ≥ 2–3 kt. [alta, fisica]

### 1.3 Riverberazione [alta per le forme, media per i valori] — [U83] capp. 8–9; [Ainslie10]

- Volume: `RL = SL − 2·TL + S_v + 10·log V`, con `V ≈ R²·ψ·(c·τ/2)` (ψ = angolo solido equivalente del fascio combinato).
- Superficie o fondo: `RL = SL − 2·TL + S_s + 10·log A`, con `A ≈ R·Φ·(c·τ/2)` (Φ = larghezza orizzontale equivalente del fascio, rad).
- **S_v** (forza di diffusione di volume): da −90 a −70 dB/m³. Il *Deep Scattering Layer* biologico (3–12 kHz) arriva fino a circa −70 dB; di giorno sta a 300–500 m, di notte risale verso la superficie (migrazione diurna). [media]
- **S_s superficie** (Chapman‑Harris 1962), con θ = radenza in gradi, v = vento in kt, f in Hz [media]:
  `S_s = 3,3·β·log(θ/30) − 42,4·log β + 2,6`, con `β = 158·(v·f^(1/3))^(−0,58)`
- **Fondo, legge di Lambert:** `S_b = 10·log μ + 10·log(sin²θ)`, con 10·log μ ≈ −27 dB (Mackenzie 1961). Valori: sabbia/roccia da −20 a −30 dB, fango da −35 a −40 dB. [media]
- **Rilevazione limitata dalla riverberazione:** per un impulso CW conta la separazione Doppler. Se il bersaglio è fermo e privo di *aspect* favorevole, contro il *clutter* il sistema lavora in pratica con DT_R di 10–15 dB. [bassa]

### 1.4 Forza di bersaglio (TS) per aspetto [media] — [U83] cap. 9 (sottomarini pp. 331–341 ed. 1983, citato in [KR24])

| Bersaglio | Traverso (beam) | Prua/poppa | Aspetti intermedi | Note |
|---|---|---|---|---|
| Sottomarino (SSN/SSK) | +25 dB | +10 … +15 dB | +15 … +20 dB | Picco speculare stretto a ±5–10° dal traverso; la falsatorre contribuisce molto. Misura WWII su un *fleet boat*: 11,5 ± 2 dB al traverso a frequenza alta [DFO49] |
| Nave di superficie (DD/FF) | +25 … +40 dB | +15 … +25 dB | — | Le grandi navi hanno TS maggiore |
| Siluro | −20 dB | −25 dB | — | — |
| Mina | da −10 a −25 dB | — | — | — |
| Balena | 0 … +10 dB | — | — | Possibile falso contatto [bassa] |
| Scia, *knuckle*, nube di bolle | da −10 a +10 dB | — | — | Per pochi minuti; vedi §6 [bassa] |

- Rivestimenti anecoici (piastrelle "Cluster Guard" sovietiche dagli anni '70–'80 su Victor III, Alfa, Akula, Sierra): abbassano la TS attiva di circa 10–20 dB alle frequenze medio‑alte. **[bassa]** ([PM04] ne descrive l'uso, non i valori in dB)
- Implementazione suggerita: `TS(φ) = TS_bow + (TS_beam − TS_bow)·|sin φ|^p`, con p ≈ 2–4, più un lobo speculare gaussiano di +3…+5 dB largo ±7° intorno a φ = 90°. [bassa, modello dell'autore]

### 1.5 Livelli di sorgente dei sonar attivi

- SL da potenza acustica irradiata P (W): `SL = 170,8 + 10·log P + DI_T` (dB re 1 µPa @1 m). Con potenza elettrica P_e ed efficienza η si usa P = η·P_e. **[alta]** [U83] cap. 4
- **Limite di cavitazione del trasduttore:** la soglia sale con la profondità. Pressione di soglia `p_c ≈ p_c0·(1 + h/10)` (h in m), quindi intensità limite `∝ (1 + h/10)²`. Ordine di grandezza in superficie per impulsi lunghi: circa 0,3–1 W/cm². [media] [U83]

| Sonar | Piattaforma | Frequenza | SL (dB re 1 µPa @1 m) | Conf. / fonte |
|---|---|---|---|---|
| AN/SQS‑53 (A/B/C) | CG‑47, DDG‑993, DD‑963 | ~3 kHz (centro) | 235 nominale | [media] [SQS53] |
| AN/SQS‑26 (AXR/BX/CX) | FF‑1052, CG | 3–4 kHz attivo, 1,5 kHz passivo; 240 kW; 18–64 km | ≈ 230–235 | potenza/frequenze [media] [SQS26]; SL [bassa] |
| AN/SQS‑56 | FFG‑7 | ~6,8–8,2 kHz | ≈ 220–225 | [bassa] |
| AN/BQQ‑5 (sfera BQS‑13, modo attivo) | SSN‑688 | ~3,5 kHz | ≈ 220–230 | [bassa] (FAS conferma solo "attivo/passivo a bassa frequenza" [BQQ5]) |
| AN/AQS‑13 (*dipping*) | SH‑3H | ~9–11 kHz | ≈ 215 | [bassa] |
| AN/SSQ‑62 DICASS (boa) | P‑3, S‑3, SH‑60B | 4 frequenze selezionabili, ~6,5–9,5 kHz | ≈ 200–205 | [bassa]; esistenza/funzione [alta] [SSQ62] |
| Testa autocercante Mk 46 / Mk 48 | — | ~20–30 kHz (alta frequenza) | ≈ 200–215 | [bassa] |
| MG‑332 "Titan‑2", MGK‑335 "Platina", "Polinom" (Udaloy) | sovietici | ~3–8 kHz | ≈ 220–235 | [bassa] (nomi da [Friedman]/[PM04]; SL non pubblicati) |

### 1.6 Fluttuazioni e probabilità di rilevazione [media] — [WMS]; [U83] cap. 5
- Il segnale ricevuto fluttua con deviazione standard σ ≈ 3–6 dB (multipath, mare). Modello "λ‑σ" usato nelle analisi operative USN: `Pd = Φ(SE/σ)`, con Φ = funzione di ripartizione normale standard.
- Per evitare rilevazioni "a lampeggio", la fluttuazione va correlata nel tempo, per esempio con un processo AR(1) con costante di tempo 2–10 min per il passivo a bassa frequenza e 10–60 s per l'attivo ad alta frequenza. **[bassa]** (scelta di progetto coerente con il modello a salti λ‑σ di [WMS])

---

## 2. Rumore irradiato

### 2.1 Componenti [alta] — [U83] cap. 10; [Ross76]

1. **Macchinario:** pompe del refrigerante del reattore, turbogeneratori, riduttori, pompe di circolazione, compressori, convertitori. Produce **tonali** (righe strette) stabili, trasmessi allo scafo per via strutturale; dipendono dallo stato dell'impianto più che dalla velocità.
2. **Elica:** rumore di pala non cavitante (righe alla frequenza di pala, *blade rate*, e armoniche); **cavitazione** (larga banda forte sopra circa 100 Hz, modulata alla frequenza d'asse e di pala); *singing*.
3. **Idrodinamico:** flusso turbolento, risonanze di cavità e di aperture (prese a mare, porte dei tubi), appendici.
4. **Transitori:** sbattere di portelli, apertura delle porte dei tubi, allagamento dei tubi, lancio, cambi di velocità delle pompe, impulsi di timone o idroplani. Sono la principale "firma" tattica di un lancio. [media]

### 2.2 Frequenze caratteristiche [alta per le formule]
- Frequenza d'asse: `f_s = RPM/60` (Hz).
- *Blade rate*: `f_BR = Z·RPM/60` (Z = numero di pale), con armoniche `m·f_BR`.
- Velocità dai giri: `V = RPM/TPK` (TPK = *turns per knot*, costante per classe). Il TPK reale delle singole classi **non è pubblico**: va inventato coerentemente, per esempio 688 ≈ 6–8 giri/kt con elica a 7 pale. **[bassa]**
- Rete elettrica di bordo: USN a 60 Hz e 400 Hz; marina sovietica a 50 Hz (più 400 Hz per armi ed elettronica). Righe a 50/60 Hz e armoniche sono il classico indizio di nazionalità al LOFAR. **[media]** (prassi nota; nessuna fonte primaria aperta trovata)
- Eliche: 688 a 7 pale *skewed* [media]; Victor III con elica tandem (due eliche a 4 pale coassiali) [media] [PM04]; Alfa, Akula, Sierra a 7 pale [bassa].

### 2.3 Cavitazione: insorgenza in funzione di profondità e velocità

- Numero di cavitazione [alta, fisica; [Ross76]]: `σ = (p_atm + ρ·g·h − p_v) / (½·ρ·V_tip²)`. La cavitazione nasce quando σ < σ_i (σ_i dipende dal progetto dell'elica). Essendo `V_tip ∝ V_nave` a passo costante:
  `V_inc(h) = V_inc(0)·sqrt((10,33 + h)/10,33)` (h = profondità dell'asse dell'elica in m; p_v trascurabile)

| Profondità elica (m) | 0 | 20 | 50 | 100 | 150 | 200 | 300 |
|---|---|---|---|---|---|---|---|
| Fattore su V_inc(0) | 1,00 | 1,71 | 2,42 | 3,27 | 3,95 | 4,51 | 5,48 |

  [media, calcolo dell'autore]
- Valori di gioco suggeriti per `V_inc(0)`: SSN USA anni '80 ≈ 4–6 kt, quindi a quota periscopica (circa 18 m all'elica) circa 7–10 kt e a 150 m circa 16–24 kt; SSN sovietici di 2ª generazione ≈ 3–4 kt; Alfa ≈ 3–5 kt; Akula ≈ 5–6 kt; navi di superficie ≈ 8–12 kt reali (elica a pochi metri di profondità). **[bassa]** Lo studio di [IIAV14] conferma che all'insorgenza il rumore cresce di "5 dB o più per nodo" [media].
- Effetto acustico: all'insorgenza la larga banda sopra ~100 Hz sale di 10–20 dB in pochi nodi, poi segue circa 50–60·log V. Il picco spettrale scende di frequenza al crescere della cavitazione. Compare la modulazione d'inviluppo (DEMON) a `f_s` e `f_BR`. [media] [U83]; [Ross76]

### 2.4 Scala con la velocità
- **Modello di Ross** per navi cavitanti (f > 100 Hz): `SL(f,V) = SL_ref(f) + 60·log(V/V_ref)`, con V_ref = 10 kt. Esiste una variante con 50·log(V/V_ref). I sei poteri derivano da sorgenti dipolari con forze ∝ V². [media] [Ross76]; [IIAV14]
- Pendenza locale: `dSL/dV ≈ 26/V` dB/kt per 60·log V, cioè circa 2,6 dB/kt a 10 kt e 1,3 dB/kt a 20 kt. La regola empirica "1–2 dB per nodo" per navi e sottomarini a velocità di crociera ne è un caso particolare. [media, calcolo]
- **Sottomarino nucleare a bassa velocità:** la larga banda è dominata dal macchinario, quasi indipendente da V (pendenza circa 0,5 dB/kt) finché il flusso e l'elica non prevalgono (tipicamente sopra 8–12 kt in profondità). Le tonali delle pompe del refrigerante cambiano a gradino al passaggio pompe lente → veloci. [bassa] (logica qualitativa coerente con [U83])

### 2.5 Rumore proprio (*self-noise*) [media] — [U83] cap. 11
- Componenti: rumore di flusso sull'idrofono e sulla cupola (domina a velocità medio‑alte), macchinario proprio, elica propria (domina nei *baffles*), rumore elettrico.
- Modello di gioco: `NL_self(f,V) = max(NL_mach(f), NL_flow0(f) + 60·log(V/V₀))`, con V₀ ≈ 8 kt per il sonar di prua e ≈ 12 kt per la schiera trainata. La schiera trainata TB‑16 è quasi inutilizzabile oltre circa 15–18 kt (vibrazione e cavo); la sfera di prua è "sorda" oltre circa 20–25 kt. **[bassa]**
- **Rumore della nave dentro i *baffles*:** settore posteriore di circa ±30–40° per la sfera (60–80° totali), con NL proprio di 10–20 dB più alto. [bassa] [Baff] (concetto [alta])

### 2.6 Livelli irradiati stimati per il gioco [bassa]
Livello a banda larga totale, 100 Hz–10 kHz, in dB re 1 µPa @1 m. Valori **non pubblicati ufficialmente**; ordinamento relativo coerente con [Cote], [PM04], [ONI97] e con la nota "Alfa molto rumoroso ad alta velocità, simile agli altri SSN sovietici a bassa velocità" ([Alfa705]).

| Classe | Silenzioso (≤ 6 kt, in profondità) | Pattugliamento (10–15 kt) | Massima | Tonali più forti |
|---|---|---|---|---|
| SSN‑688 / 688I | 120–128 | 130–138 | 155–160 (32 kt) | 105–115 |
| SSN‑637 Sturgeon | 125–130 | 135–140 | 155 | 110–118 |
| Victor I / II | 140–145 | 148–152 | 165 | 125–135 |
| Victor III | 128–135 | 138–142 | 158 | 115–122 |
| Alfa (pr. 705) | 135–140 (motori ausiliari) | 145–150 | 170–175 (41 kt) | 120–130 |
| Akula / Sierra | 120–125 | 128–135 | 155 | 105–112 |
| November / Echo / Charlie | 145–155 | 155–160 | 170 | 130–140 |
| Kilo (a batteria) / Foxtrot (a batteria) | 112–118 / 125–130 | — | 140 / 145 | 100 / 110 |
| SSK in *snorkel* (diesel) | 150–160 | — | — | 140 (accensione dei diesel) |
| Typhoon / Delta IV | 128–132 / 125–130 | 135–140 | 160 | 110–118 |
| Fregata o cacciatorpediniere a 18 kt | 165–172 | — | 180–185 (30 kt) | 140–150 |
| Mercantile grande a 15 kt | 175–185 | — | — | 150–160 (pala ed eliche) |

### 2.7 LOFAR, DEMON, cascata (*waterfall*) e identificazione [media] — [U83]; [Cote]; [Waite]

- **LOFAR** (*LOw Frequency Analysis and Recording*): il segnale di ogni fascio passa in FFT (bin tipici da 0,03 a 1 Hz, banda 10 Hz–2 kHz). Il LOFARgram mostra le righe in funzione di frequenza (ascisse) e tempo (ordinate, che scorrono verso il basso): una tonale è una linea verticale. L'integrazione su 1–8 righe di FFT rende le linee deboli visibili come "tracce". [media]
- **BTR / waterfall in rilevamento** (*Bearing‑Time Recorder*): ascisse = rilevamento (0–360° o settori), ordinate = tempo. Ogni pixel è l'energia a banda larga del fascio. Un contatto è una traccia la cui pendenza dà il *bearing rate* (input della TMA, §5). [alta, concetto]
- **DEMON** (*DEMOdulation of Noise*): si filtra il rumore di cavitazione su una banda alta (per es. 2–8 kHz), se ne rileva l'inviluppo e se ne fa la FFT a bassa frequenza (0–100 Hz). Il risultato dà f_s e f_BR, quindi il numero di pale `Z = f_BR/f_s` e la velocità `V = 60·f_s/TPK`. Funziona solo su sorgenti cavitanti (navi e sottomarini veloci). [media]
- **Classificazione:** confronto di un insieme di tonali (frequenza, rapporti armonici, stabilità), del numero di pale e dei transitori con una libreria di firme. Prima si assegna la classe (sottomarino nucleare sovietico, nave a turbina a gas…), poi si identifica il tipo. Lo spostamento Doppler di una tonale stabile dà la componente radiale della velocità relativa, `v_r = c·Δf/f₀`, che entra nella TMA. [media]
- **Storicamente:** fino alla fine degli anni '70 i sottomarini sovietici erano rilevati soprattutto tramite le tonali a banda stretta (SOSUS, P‑3, SSN). Il silenziamento del Victor III e dell'Akula, aiutato dalla rete di spionaggio Walker e dalle fresatrici Toshiba‑Kongsberg, ridusse molto le distanze di scoperta negli anni '80. [media] [Cote] via [NSL‑rev]; [Murphy97]. La "parità acustica" raggiunta dall'URSS negli anni '80 (fase 4 di Cote) è la premessa narrativa ideale per il gioco.

---

## 3. Propagazione

### 3.1 Velocità del suono

**Mackenzie (1981)** — consigliata per il gioco [alta] [M81] (coefficienti verificati su NPL)
```
c(T,S,D) = 1448,96 + 4,591·T − 5,304e-2·T² + 2,374e-4·T³
         + 1,340·(S − 35) + 1,630e-2·D + 1,675e-7·D²
         − 1,025e-2·T·(S − 35) − 7,139e-13·T·D³        [m/s]
T in °C, S in ‰ (psu), D in m. Validità: T 2…30 °C, S 25…40 ‰, D 0…8000 m. σ ≈ 0,07 m/s.
```
- **Coppens (1981)** (T 0–35 °C, S 0–45, D 0–4 km; t = T/10, D in km) [alta] [NPL‑SS]:
  `c = 1449,05 + 45,7t − 5,21t² + 0,23t³ + (1,333 − 0,126t + 0,009t²)(S − 35) + (16,23 + 0,253t)D + (0,213 − 0,1t)D² + [0,016 + 0,0002(S − 35)](S − 35)·t·D`
- **UNESCO / Chen‑Millero (1977)** nella versione Wong‑Zhu (1995): standard internazionale con pressione in bar, coefficienti tabulati in [NPL‑SS]. Da usare solo come riferimento offline. [alta]
- **Derivate utili** (circa 10 °C, 35 ‰): ∂c/∂T ≈ +4,6 m/s/°C a 0 °C e ≈ +3,0 m/s/°C a 20 °C; ∂c/∂S ≈ +1,3 m/s/‰; ∂c/∂z ≈ +0,0163 s⁻¹ (+1,63 m/s ogni 100 m; ES310 dice +1,7). [alta]
- **Raggio di curvatura** in un gradiente lineare g = dc/dz: `R_c = c₀/(|g|·cos θ₀) ≈ c₀/|g|`. Esempio: strato isotermo, g = 0,0163 s⁻¹ → R_c ≈ 92 km. Termoclino con g = −0,1 s⁻¹ → R_c ≈ 15 km. [alta, fisica] [U83] cap. 5

### 3.2 Assorbimento

**Thorp (1967)**, solo per un primo passaggio, valida 0,1–50 kHz [alta]:
`α (dB/km) = 0,11·f²/(1 + f²) + 44·f²/(4100 + f²) + 2,75e-4·f² + 0,003` (f in kHz)

**Ainslie‑McColm (1998)**, consigliata (stessa accuratezza di Francois‑Garrison con meno termini) [alta] [AM98]; [NPL‑ABS]:
```
α = 0,106·(f₁·f²/(f₁² + f²))·exp((pH − 8)/0,56)
  + 0,52·(1 + T/43)·(S/35)·(f₂·f²/(f₂² + f²))·exp(−z/6)
  + 0,00049·f²·exp(−(T/27 + z/17))                              [dB/km]
f₁ = 0,78·sqrt(S/35)·exp(T/26)   [kHz]   (acido borico)
f₂ = 42·exp(T/17)                [kHz]   (solfato di magnesio)
f in kHz, T in °C, S in ‰, z in km, pH ≈ 8 (7,9–8,2)
```
- **Francois‑Garrison (1982)**, riferimento completo, 200 Hz–1 MHz [alta] [FG82]:
  - Boro: `A₁ = (8,86/c)·10^(0,78·pH − 5)`, P₁ = 1, `f₁ = 2,8·sqrt(S/35)·10^(4 − 1245/θ)`.
  - MgSO₄: `A₂ = 21,44·(S/c)·(1 + 0,025T)`, `P₂ = 1 − 1,37e-4·z + 6,2e-9·z²`, `f₂ = 8,17·10^(8 − 1990/θ)/(1 + 0,0018(S − 35))`.
  - Acqua pura: `P₃ = 1 − 3,83e-5·z + 4,9e-10·z²`. Per T ≤ 20 °C: `A₃ = 4,937e-4 − 2,59e-5T + 9,11e-7T² − 1,5e-8T³`; per T > 20 °C: `A₃ = 3,964e-4 − 1,146e-5T + 1,45e-7T² − 6,5e-10T³`.
  - Dove θ = 273 + T, c = 1412 + 3,21T + 1,19S + 0,0167z, z in m.
  - Totale: `α = A₁P₁f₁f²/(f₁² + f²) + A₂P₂f₂f²/(f₂² + f²) + A₃P₃f²` (dB/km).
  - Coefficienti riportati dall'autore e coerenti con le implementazioni MB‑System e R/sonar. **[media]**: verificarli sul PDF [FG82] prima del rilascio.

Valori calcolati in superficie (dB/km) [media, calcolo dell'autore]:

| f | 50 Hz | 100 Hz | 300 Hz | 1 kHz | 3,5 kHz | 10 kHz | 25 kHz | 50 kHz |
|---|---|---|---|---|---|---|---|---|
| A‑M, Atlantico 4 °C, 35 ‰, pH 8 | 0,0003 | 0,0013 | 0,011 | 0,064 | 0,23 | 1,17 | 5,8 | 15,3 |
| A‑M, Mediterraneo 13,5 °C, 38,5 ‰, pH 8,2 | 0,0003 | 0,0012 | 0,010 | 0,080 | 0,28 | 1,03 | 5,1 | 16,6 |
| A‑M, Artico −1,5 °C, 34 ‰ | 0,0005 | 0,0018 | 0,015 | 0,073 | 0,25 | 1,33 | 6,0 | 13,2 |
| Thorp | 0,003 | 0,004 | 0,013 | 0,069 | 0,24 | 1,19 | 6,1 | 17,5 |

Conseguenza tattica: sotto 1 kHz l'assorbimento è trascurabile fino a centinaia di km, ed è per questo che LOFAR e SOSUS lavorano a bassa frequenza. A 25 kHz (siluri, sonar d'evitamento) la portata utile è di pochi km. [alta]

### 3.3 Struttura verticale del profilo (SVP) [alta] — [ES310]; [U83] cap. 5

- **Strato superficiale / strato rimescolato (*mixed layer*):** quasi isotermo, quindi c cresce con la pressione (gradiente positivo). Profondità da 0–30 m (estate, calma) a 100–400 m (inverno, alte latitudini). ES310 indica "fino a 100 m" per tempeste invernali nelle medie latitudini.
- **SLD (*Sonic Layer Depth*):** profondità del massimo di c vicino alla superficie (gradiente positivo sopra, negativo sotto). È il "layer" della dottrina: sopra o sotto il layer.
- **Termoclino stagionale** e **termoclino principale:** gradiente negativo.
- **Asse del canale sonoro profondo (SOFAR):** minimo di c. Circa 1000–1300 m alle medie latitudini; risale verso la superficie alle alte latitudini (Artico: 0–100 m).
- **Strato isotermo profondo:** gradiente positivo dovuto alla sola pressione.
- **Profondità critica:** profondità sotto l'asse in cui c eguaglia il massimo di c sopra l'asse (in superficie o alla SLD). **Eccesso di profondità** (*depth excess*) = fondale − profondità critica. Per le CZ serve un eccesso di almeno 200 m [ES310]; altre fonti operative indicano 300–600 m per CZ "affidabili" [bassa].

### 3.4 Percorsi di propagazione

**(a) Divergenza geometrica** [alta] [ES310]
- Sferica: `TL = 20·log r` (r in m).
- Cilindrica: `TL = 10·log r + 10·log r_t`. Con r_t = 1000 m: `TL = 10·log r + 30`.
- Completa: `TL = 10·log r + 30 + α·r/1000 + A` (A = "anomalia" di trasmissione, che raccoglie tutto il resto).
- Regola mnemonica ES310: 60 dB a 1 km, 70 dB a 10 km, 80 dB a 100 km (cilindrica).

**(b) Condotto superficiale (*surface duct*)** [media] [U83] cap. 6; [Jensen]
- **Frequenza di taglio:** `λ_max ≈ 8,5e-3·H^(3/2)` (λ e H in piedi; Urick). Esempi: H = 30 m → f_min ≈ 590 Hz; H = 100 m → f_min ≈ 95 Hz; H = 200 m → f_min ≈ 35 Hz. Altre formulazioni (Jensen) danno valori circa 2× più alti. **Sotto f_min il condotto non intrappola.**
- **Distanza di salto** del raggio limite: `x_skip ≈ sqrt(8·R_c·H)`. Esempio: H = 100 m, R_c = 92 km → x ≈ 8,6 km.
- **TL nel condotto** (forma AMOS/Urick): `TL = 20·log r` per r < r₀; oltre, `TL = 10·log r₀ + 10·log r + (α + α_L)·r`, con `r₀ ≈ x_skip/8 … x_skip/4` e α_L = perdita di dispersione e fuga, da circa 0,5 a 5 dB per salto secondo frequenza, stato del mare e H. [bassa per α_L]

**(c) Zona d'ombra sotto il layer** [media, geometria]
- Sorgente a z_s < H dentro il layer: il raggio limite diventa orizzontale alla SLD dopo `x₁ ≈ sqrt(2·R_c·(H − z_s))`.
- Ricevitore sotto il layer a z_r: l'ombra comincia circa a `x₁ + sqrt(2·R'_c·(z_r − H))`, con R'_c = c/|g_termoclino|.
- Nell'ombra arriva solo energia diffratta o diffusa, oppure via fondo o CZ: TL di 10–30 dB peggiore della divergenza sferica. Modello di gioco: penalità d'attraversamento del layer `ΔTL_layer = 5…20 dB`, crescente con |g_termoclino| e con f, decrescente al diminuire della frequenza sotto f_min. **[bassa]**

**(d) Zone di convergenza (CZ)** [media]
- Intervalli tipici: 20–35 miglia secondo latitudine e profilo [H61]; 20–30 nm, ripetute (30, 60, 90 nm…) e larghe "poche miglia" [ES310]. Le misure di Urick danno semizone larghe 3–5 miglia con guadagno di convergenza di 5–10 dB rispetto alla divergenza sferica [U65].
- Variazione con la latitudine: CZ più corte con superficie fredda o profilo con asse superficiale; più lunghe in acque calde e profonde. Un fronte termico accorcia o allunga la CZ rispetto alle due masse d'acqua [DTIC‑A048759].
- Valori calcolati dall'autore con Mackenzie su profili climatologici archetipici (raggio con vertice alla SLD, integrale `x = 2∫c/sqrt(c_v² − c²)dz`) [bassa]:

| Profilo | Asse SOFAR | Profondità critica | Fondale | CZ |
|---|---|---|---|---|
| Atlantico NE estate | ~1500 m | ~2800 m | 4500 m | ≈ 35 nm |
| Norvegese inverno | ~900 m | ~2100 m | 3500 m | ≈ 32 nm |
| Norvegese estate | — | — | — | ≈ 27 nm |
| Pacifico 35° N estate | ~1000 m | ~4200 m | 5500 m | ≈ 34 nm |
| Pacifico 35° N inverno (MLD 120 m) | — | — | — | ≈ 40 nm |
| Mediterraneo estate | ~150 m | ~1950 m | 3000 m | ≈ 22 nm |

  - Nessuna CZ dove il fondale non supera la profondità critica: Mare Arabico con superficie a 29 °C su 3500 m, Barents, Golfo Persico, piattaforme, GIUK sulle soglie.
  - Il valore tipico citato di ~30–33 nm (Atlantico medio) è coerente con queste stime.
- **Valori dei teatri del gioco (29/10/2026), al posto degli archetipici.** Profili WOA23 del decennio 1985-1994,
  l'epoca del gioco: il mensile a 0,25° del periodo 1955-2022 corretto con la differenza stagionale «1985-1994 meno
  1955-2022» (`tools/pipeline/fetch_woa_8594.py`, `suono.py --epoca 8594`). Mediana del teatro, velocità TEOS-10.
  - **Metodo:** la distanza è l'integrale del raggio con vertice al massimo di c sopra l'asse
    (`ProfiloSuono::analizza`, lo stesso che usa il motore).
  - **Verifica coi raggi** (`calcola_campo`, guadagno sulla sferica a 100 m di quota): conferma l'ordine di grandezza
    entro ±20-25% e dà 12-18 dB di guadagno di convergenza.
  - **Dove vale:** la zona c'è solo dove il fondale del punto supera di almeno 200 m la profondità critica.
  - **Mesi:** quattro, uno per stagione. Il programma è `tools/re/prove/woa8594/cz.cpp`.
  [media: dati WOA, metodo del motore]

| Teatro | Mese | Asse m / c min m/s | Profondità critica m | Fondale max del teatro m | CZ |
|---|---|---|---|---|---|
| Celtica | febbraio | 1500 / 1494,4 | 2535 | 4966 | ≈ 24,8 nm |
| Celtica | maggio | 1650 / 1495,8 | 2518 | 4966 | ≈ 30,2 nm |
| Celtica | agosto | 1550 / 1495,7 | 3167 | 4966 | ≈ 35,7 nm |
| Celtica | novembre | 1650 / 1496,3 | 2593 | 4966 | ≈ 32,1 nm |
| Faroe | febbraio | 600 / 1457,9 | 2283 | 2806 | ≈ 29,6 nm |
| Faroe | maggio | 700 / 1458,7 | 2272 | 2806 | ≈ 28,0 nm |
| Faroe | agosto | 600 / 1458,7 | 2889 | 2806 | ≈ 29,2 nm (non nel gioco: fondale troppo basso) |
| Faroe | novembre | 700 / 1459,0 | 2543 | 2806 | ≈ 31,9 nm |
| Gibilterra | maggio | 425 / 1505,1 | 828 | 1624 | ≈ 12,9 nm |
| Gibilterra | agosto | 450 / 1504,2 | 1590 | 1624 | ≈ 19,9 nm (non nel gioco: fondale troppo basso) |
| Gibilterra | novembre | 450 / 1506,0 | 1058 | 1624 | ≈ 16,9 nm |
| Kunashir | agosto | 225 / 1456,8 | 2559 | 6912 | ≈ 25,2 nm |
| Kunashir | novembre | 225 / 1458,4 | 1415 | 6912 | ≈ 18,8 nm |
| Otranto | maggio | 75 / 1507,2 | 878 | 2184 | ≈ 18,7 nm |
| Otranto | agosto | 75 / 1508,3 | 2002 | 2184 | ≈ 22,4 nm (non nel gioco: fondale troppo basso) |

  - **Nessuna zona di convergenza** nel Baltico e nel Barents (fondale sotto i 500 m), né in nessun teatro nei mesi in cui
    lo strato superficiale arriva a 400 m o l'asse è in superficie (Kunashir d'inverno, Otranto d'inverno, Gibilterra a
    febbraio).
  - **Tattica estiva a Otranto e Gibilterra (mese predefinito agosto): nessuna zona di convergenza nel gioco.** La
    profondità critica estiva (circa 2000 m a Otranto, 1600 a Gibilterra) è troppo vicina al fondale massimo dei due
    teatri (2184 e 1624 m). Il passivo lontano vive di cammini diretti, del rimbalzo sul fondo e, nello stretto, del
    canale dell'acqua mediterranea a 450 m; in primavera e in autunno le zone tornano (13-19 nm). Con la media 1955-2022
    c'erano, al limite: la scelta dell'epoca del gioco le ha tolte (decisione dell'utente, 29/10/2026).
  - **La Celtica** ha il profilo profondo quasi isotermo: la distanza è sensibile a pochi decimi di m/s, e con la media
    1955-2022 arrivava a 46 nm.
  - **Il confronto completo** fra le due climatologie è nelle note della sessione (woa8594-regressione.md).
- **Larghezza della zona:** circa 5–10% della distanza della CZ. Modello di gioco: `G_CZ(r) = G_max·exp(−((r − k·R_CZ)/w)²)`, con G_max = 5–10 dB, w ≈ 0,04·R_CZ, k = 1, 2, 3; ogni CZ successiva perde altri 3–5 dB. [bassa]

**(e) Rimbalzo sul fondo (*bottom bounce*, BB)** [media] [ES310]; [U83]
- Raggi con angolo > 30° (tipicamente 10–40°) dall'orizzontale. Fondali duri e piatti sono i migliori; il fango morbido è il peggiore [ES310].
- Perdita per riflessione: 2–6 dB per sabbia o roccia, 8–15 dB per fango (1–4 kHz), crescente con frequenza e angolo. Portate BB tipiche 5–20 nm in acqua profonda (modo BB dell'SQS‑26). [bassa per i valori]

**(f) RAP (*Reliable Acoustic Path*)** [media] [CS76]: ricevitore profondo, vicino alla profondità critica, e sorgente vicino alla superficie. Percorso diretto senza ombre fino a circa 20–35 km. Utile nel gioco per un SSN profondo che ascolta una nave.

**(g) Canale SOFAR:** sorgente e ricevitore vicino all'asse danno `TL ≈ 10·log r + 10·log r_t + α·r` con perdite bassissime. È la base del SOSUS [Cote]. Per un sottomarino l'asse è raggiungibile solo dove è poco profondo (alte latitudini e Mediterraneo).

**(h) Specchio di Lloyd (interferenza con l'immagine in superficie)** [media] [U83]; [Jensen]; [Pereira16]
- Sorgente a z_s e ricevitore a z_r, superficie a pressione nulla (R = −1):
  `I/I_dir = 4·sin²(k·z_s·z_r/r)`, con k = 2πf/c, quindi `ΔTL_LM = −10·log(4·sin²(k·z_s·z_r/r))`.
- Per `r ≫ k·z_s·z_r`: ΔTL ≈ −10·log(4(k·z_s·z_r/r)²), cioè **TL cresce come 40·log r** (dipolo).
- Conseguenza: le sorgenti molto superficiali (sottomarino a quota periscopica, nave) a **bassa frequenza** sono attenuate vicino alla superficie e a grande distanza.
- Con mare mosso la riflessione perde coerenza. Coefficiente coerente (Eckart): `R_coh = −exp(−2(k·σ_h·sin θ)²)`, con σ_h = scarto quadratico dell'altezza d'onda. Per il gioco conviene una media incoerente oltre circa SS3.

**(i) Acqua bassa (< 200 m)** [media] — Marsh‑Schulkin, in [U83] cap. 6
- Parametro di salto: `H_ms = sqrt((L + D)/3)` in kyd, con L = layer e D = fondale, entrambi in piedi. R in kyd, k_L = anomalia di near-field (≈ 0–5 dB).
  - R < H_ms: `TL = 20·log R + α·R + 60 − k_L`
  - H_ms < R < 8·H_ms: `TL = 15·log R + α·R + α_T·(R/H_ms − 1) + 5·log H_ms + 60 − k_L`
  - R > 8·H_ms: `TL = 10·log R + α·R + α_T·(R/H_ms − 1) + 10·log H_ms + 64,5 − k_L`
- α_T (attenuazione per perdita sul fondo, dB per salto): da 0,5 a 5 dB secondo fondale, frequenza e stato del mare (tabelle M‑S). [bassa per i valori]

### 3.5 Variazioni stagionali e diurne
- **Effetto pomeriggio (*afternoon effect*)** [alta] [IW42]
  - Riscaldamento solare con vento debole e cielo sereno: si forma un gradiente negativo nei primi metri, che "spezza" il condotto superficiale e accorcia molto le portate verso bersagli superficiali. Le portate restano spesso corte anche dopo il tramonto.
  - Fattori in ordine di importanza: altezza del sole, nuvolosità, vento, differenza aria‑acqua, umidità. Con vento di forza Beaufort ≥ 4 la turbolenza impedisce la stratificazione.
  - Nell'Atlantico occidentale si presentava in media nel ~20% dei casi. I limiti di rifrazione davano portate sicure sotto 2500 yd nel 92% dei casi d'estate e nel 34% d'inverno.
  - Modello di gioco: dalle 10 alle 18 locali, con vento < 11 kt (B ≤ 3) e nubi < 50%, si aggiunge sopra la SLD un microtermoclino di 0,5–2 °C nei primi 5–15 m, che si dissipa in 3–6 h dopo il tramonto o con vento ≥ 11 kt. [bassa per i parametri]
- **Stagioni** [media]: d'inverno lo strato rimescolato è profondo (100–400 m), i condotti superficiali sono buoni e le ombre poche. D'estate il termoclino è superficiale e forte, con condotti sottili (alto f_min), ombre marcate e il layer intorno ai 20–60 m.
- **Mare e vento:** il rimescolamento approfondisce la SLD di circa 1–3 m per ora di vento ≥ 20 kt (modello di gioco). [bassa]

### 3.6 Teatri: profili tipici e caratteristiche [media per i tratti qualitativi, bassa per i numeri]

| Teatro | Fondale | SVP inverno | SVP estate | CZ | Particolarità |
|---|---|---|---|---|---|
| **Atlantico N / GIUK** | Soglie GIUK 400–900 m; bacini 3000–4500 m | Superficie 8–11 °C; MLD 200–400 m | Superficie 12–16 °C; termoclino a 20–50 m | Sì nei bacini (~30–35 nm); **no sulle soglie** | Fronte Islanda‑Faroe molto marcato (salto di 3–5 °C in poche nm); acque mediterranee profonde a 1000 m; stato del mare alto (SS4–6 frequente d'inverno) |
| **Mare di Norvegia** | 3000–3600 m | Acqua atlantica calda (5–7 °C) sopra acqua profonda a −1 °C; asse a 600–1000 m | Superficie 10–12 °C | Sì, ~27–32 nm | Rumore ambiente da tempeste; zona di transito degli SSN verso i bastioni |
| **Mare di Barents** | 200–400 m | Isotermo vicino al congelamento (−1,5 °C): profilo rifrangente verso l'alto, ottimo per le basse frequenze ma con perdite per rugosità superficiale | Superficie 5–8 °C a sud; fronte polare | No (acqua bassa) | Ghiaccio marginale; rumore biologico estivo; bastione dei SSBN della Flotta del Nord |
| **Mediterraneo** | 2000–4000 m; molti bacini e stretti | Isotermo a ~13–14 °C fino al fondo, quindi gradiente positivo dalla superficie: forte condotto, nessun asse profondo | Termoclino forte (25 °C in superficie → 14 °C a 60–100 m); asse a 100–200 m | Solo d'estate (~20–25 nm) | Traffico mercantile intenso; acque profonde calde; salinità 38–39 ‰ |
| **Pacifico N** | 4000–6000 m | Subartico: asse superficiale 50–200 m, alocline. Medie latitudini: asse a ~1000 m | Medie latitudini: superficie 20–25 °C | Sì alle medie latitudini (~33–40 nm) | Mare di Okhotsk = bastione della Flotta del Pacifico (ghiaccio stagionale) |
| **Golfo Persico** | Medio ~35–50 m, massimo ~100 m | 18–21 °C, quasi isotermo | 30–35 °C in superficie, S 40+ ‰; gradiente negativo forte, rifrazione verso il fondo | No | Portate brevissime; gamberi schioccatori, traffico e petroliere: NL alto; vortici di 5–10 km [AA22] |
| **Golfo di Oman / Mare Arabico** | 3000–4000 m | — | Superficie 27–30 °C; intrusione d'acqua del Golfo, calda e salata, a 200–400 m che crea inversioni [AA22] | Marginale o assente | Monsoni da SW (giugno–settembre): SS5–6 |
| **Artico sotto il ghiaccio** | Bacini 3000–4000 m, piattaforme < 100 m | Asse SOFAR in superficie (semi‑canale): tutto rifratto verso l'alto | — | No: ogni raggio rimbalza sotto il ghiaccio | Perdita per rugosità del ghiaccio crescente con f (pessimo sopra ~1 kHz, ottimo a 10–50 Hz); NL molto variabile (§4.6) |

---

## 4. Rumore ambiente

### 4.1 Struttura delle curve di Wenz [alta] [W62]
Il rumore è la somma di almeno tre componenti sovrapposte:
- turbolenza e fluttuazioni di pressione, 1–100 Hz;
- traffico navale lontano, 10–1000 Hz;
- vento (bolle e spruzzi), 50 Hz–20 kHz.

A cui si aggiungono il rumore termico (> 50–100 kHz), la sismica (< 10 Hz), la pioggia e la biologia.

**Modello parametrico di Coates / Stojanovic** (f in kHz, risultati in dB re 1 µPa²/Hz) [media] [Stoj07]; [arXiv‑1801.03641]; implementazione ns‑3:
```
N_turb(f)  = 17 − 30·log f
N_ship(f)  = 40 + 20·(s − 0,5) + 26·log f − 60·log(f + 0,03)      s ∈ [0,1] traffico
N_wind(f)  = 50 + 7,5·w^0,5 + 20·log f − 40·log(f + 0,4)          w = vento (m/s)
N_th(f)    = −15 + 20·log f
NL(f)      = 10·log Σ 10^(N_i/10)
```

Valori calcolati con s = 0,5 (dB re 1 µPa²/Hz) [media, calcolo]:

| f \ vento | 0 m/s | 2,5 m/s | 5 m/s | 10 m/s | 15 m/s | 20 m/s |
|---|---|---|---|---|---|---|
| 50 Hz | 72 | 72 | 72 | 73 | 73 | 75 |
| 100 Hz | 67 | 67 | 68 | 70 | 73 | 76 |
| 300 Hz | 56 | 60 | 63 | 70 | 75 | 79 |
| 1 kHz | 45 | 56 | 61 | 68 | 73 | 78 |
| 3,5 kHz | 37 | 49 | 54 | 61 | 66 | 71 |
| 10 kHz | 29 | 41 | 46 | 53 | 58 | 63 |
| 25 kHz | 22 | 34 | 39 | 46 | 51 | 55 |

Componente traffico da sola (s = 0 / s = 1): 50 Hz 62 / 82 dB; 100 Hz 57 / 77 dB; 300 Hz 45 / 65 dB.

### 4.2 Curve di Knudsen e stato del mare
- **Knudsen** (fit di [KAE48], f in Hz, valida circa 0,5–25 kHz): `NL(f, SS) = 94,5 − (50/3)·log f + 30·log(SS + 1)` (dB re 1 µPa²/Hz).
  - Verifica a 1 kHz: SS0 44,5 / SS1 53,5 / SS2 58,8 / SS3 62,6 / SS4 65,5 / SS5 67,8 / SS6 69,9.
  - I valori tabulati classici di Urick sono 44,5 / 50 / 55 / 61,5 / 64,5 / 66,5 / 68,5. [media]
  - Nota: la fonte [arXiv‑0712.1833] non esplicita l'unità di f, ma solo con f in Hz il valore a SS0 coincide con il classico 44,5 dB.
- **Corrispondenza stato del mare ↔ vento ↔ onda** (WMO/Douglas, indicativa) [media]:

| Stato del mare | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|---|
| Beaufort | 0 | 1–2 | 3 | 4 | 5 | 6 | 7 | 8–9 |
| Vento (kt) | < 1 | 1–6 | 7–10 | 11–16 | 17–21 | 22–27 | 28–33 | 34–47 |
| Hs (m) | 0 | 0–0,1 | 0,1–0,5 | 0,5–1,25 | 1,25–2,5 | 2,5–4 | 4–6 | 6–9 |

- Con venti oltre ~15 m/s e frequenze > 4 kHz il rumore **satura o cala** (le bolle schermano). Le curve di Wenz, monotone, non lo prevedono: nel gioco conviene limitare N_wind a w = 15 m/s. [media] [JASA‑EL23]

### 4.3 Traffico navale [media]
Domina tra 10 e 1000 Hz con picco circa 50–100 Hz. Valori indicativi: traffico leggero, medio e pesante circa 60, 70 e 80 dB a 100 Hz (coerenti con Coates per s = 0 / 0,5 / 1). Il traffico lontano si propaga via SOFAR e CZ, quindi dipende dal bacino: Mediterraneo e Atlantico N sono "pesanti", Artico e Pacifico S "leggeri". Per il gioco s si mappa per cella: rotte mercantili s = 0,8–1; oceano aperto s = 0,3–0,5; Artico sotto il ghiaccio s = 0. [media] [W62]

### 4.4 Pioggia [media] (Wenz; Nystuen, idem in [NRC03])
Aumenta il rumore tra 1 e 30 kHz. La pioggerella e le gocce piccole hanno un picco caratteristico a circa 13–25 kHz (risonanza delle bolle). La pioggia forte alza lo spettro di 10–30 dB in 1–10 kHz, portandolo a circa 65–80 dB re 1 µPa²/Hz a 5 kHz con 50–100 mm/h. Modello di gioco: `N_rain(f, R) = 50 + 10·log(R_mm/h) + 10·log(f/5)` per f < 5 kHz, poi `−10·log` oltre 25 kHz, con un picco di +5 dB a 15 kHz se R < 2 mm/h. **[bassa]**

### 4.5 Biologici
- **Gamberi schioccatori** (Alpheidae): picco a picco circa 190 dB re 1 µPa @1 m, energia principale 2–5 kHz, spettro fino a 200 kHz; colonie dense in acque tropicali e subtropicali poco profonde (< 60 m, latitudine < ~40°). Possono alzare NL di 20–30 dB in 2–20 kHz. Rilevanti nel Golfo Persico e nel Mar Rosso. [alta] [Front21]; [KIOST]
- **Balenottera azzurra:** circa 180–190 dB re 1 µPa @1 m, 10–40 Hz (Cummings & Thompson 1971). **Balenottera comune:** impulsi a 20 Hz di circa 180–190 dB, scoperti dalla Marina USA negli anni '60 (i "mostri dei 20 Hz" di Jezebel). **Megattera:** 150–190 dB, 30 Hz–8 kHz. **Capodoglio:** click di circa 230 dB pp. [media] [NRC03]
- **Uso nel gioco:** falsi contatti LOFAR (tonali e impulsi a 20 Hz), cori di pesci al crepuscolo (100 Hz–1 kHz, +5–15 dB), riverberazione del DSL. [bassa]

### 4.6 Ghiaccio [media] — [GZ94]; [Milne72]
- Sotto il pack stabile e senza vento il rumore può stare 5–10 dB sotto SS0 in acqua libera.
- Sopra ~40 Hz domina la fessurazione termica: forte nelle notti fredde, quando l'aria si raffredda rapidamente. Oltre il 50% del livello viene da eventi entro 6 km e oltre l'80% entro 30 km.
- Sotto ~40 Hz dominano poche creste di pressione attive a decine di km. Al margine del ghiaccio (MIZ) il moto ondoso sui flottanti alza il rumore di +10…+20 dB.
- Il rumore è non gaussiano e impulsivo, il che genera falsi allarmi. Modello di gioco: base pari a SS0 − 5 dB, con impulsi casuali di +10–25 dB per 0,1–2 s secondo un processo di Poisson (tasso più alto con temperatura dell'aria in calo). [bassa per i parametri]

### 4.7 Effetto del meteo sulla portata (esempio) [media, calcolo]
Passaggio da SS2 a SS6 a 1 kHz: +11–13 dB di NL.
- Con divergenza cilindrica (10·log r) la portata scende di un fattore circa 13–20.
- Con divergenza sferica (20·log r) scende di un fattore circa 3,5–4,5.

Le tonali sotto 300 Hz risentono poco del vento (domina il traffico). Quindi **il cattivo tempo degrada soprattutto la larga banda e l'attivo a media frequenza, molto meno il LOFAR a bassa frequenza**: è un'asimmetria tattica realistica.

---

## 5. Rilevazione, classificazione, localizzazione

### 5.1 TMA solo rilevamenti (*bearings-only*)
- **Osservabilità** [alta] [NA81]: con il solo rilevamento e bersaglio a rotta e velocità costanti, la soluzione è **inosservabile** finché la nave propria non manovra (almeno un cambio di rotta o velocità: due "gambe").
- **Regola del 1934** [alta, derivazione]: `Ḃ (°/min) = 1934·V_⊥ (kt) / R (yd)`. Infatti 1 kt = 33,756 yd/min e 33,756 × 57,296 = 1934,1. Equivalenti metrici: `Ḃ (°/min) = 1,77·V_⊥ (kt) / R (km)` oppure `Ḃ (rad/s) = V_⊥/R`. V_⊥ è la velocità relativa trasversale alla linea di vista (LOS).
- **Distanza di Ekelund** [alta] [TMA‑wiki]; [NHHC‑Ekelund] (Ekelund, 1956)
  - Prima gamba con *bearing rate* Ḃ₁ e componente della propria velocità attraverso la LOS OSA₁; manovra; seconda gamba con Ḃ₂ e OSA₂. Se il bersaglio è costante:
    `R = 1934·(OSA₂ − OSA₁) / (Ḃ₁ − Ḃ₂)` (yd, °/min, kt)
  - Wikipedia lo scrive come `r₂ = (SOA² − SOA¹)/(BR¹ − BR²)` in unità coerenti.
  - Limiti: ignora l'accelerazione del rilevamento e usa solo due gambe [US5877998]. È una buona stima quando la manovra è rapida, la gamba breve e la distanza moderata. Va aggiunto rumore realistico su Ḃ (±0,05–0,2 °/min).
- **Strategia delle gambe** [media] [WMS]; [Hervey]
  - Gambe di 3–6 minuti (più lunghe a grande distanza).
  - Manovra di 30–90° verso il lato che massimizza la variazione di OSA. Classica la manovra "per mettere il bersaglio a prua 30–60° dall'altro lato".
  - Evitare di manovrare in direzione del bersaglio a breve distanza.
  - Dopo una manovra la schiera trainata impiega 2–5 minuti a raddrizzarsi (§5.3).
  - Il metodo del "contatto a prora", che mantiene Ḃ ≈ 0 avvicinandosi, è usato per la caccia ma non dà la distanza.
- **Altri metodi manuali:** Spiess (geometrico), *Spears wheel*, *strip plot*, *time‑bearing plot* (pendenza = Ḃ). L'MATE (*Manual Adaptive Target Estimator*) del sistema di controllo del tiro: l'operatore regola a mano distanza, rotta e velocità finché i rilevamenti previsti non coincidono con quelli misurati; è l'ideale per un'interfaccia di gioco. [media]
- **Dati ausiliari:**
  - Doppler di una tonale stabile: `f = f₀·(1 − Ṙ/c)` dà Ṙ.
  - Distanza CZ: un contatto che compare e scompare in un anello di qualche nm implica R ≈ k·R_CZ.
  - Distanza da multipath diretto/riflesso (D/E) con la sfera.
  - Telemetro passivo (PUFFS/BQG‑4 su alcuni sottomarini; *Wide Aperture Array* solo dopo la Guerra Fredda).

  [media]
- **Precisione di rilevamento** [bassa]: sfera BQS‑13 circa 1–2° (alta frequenza) e 2–5° (bassa); TB‑16 fino a ~0,5° al traverso e > 5° vicino all'endfire. Lo scarto RMS del rilevamento si può modellare come `σ_B ≈ θ₃dB / sqrt(SNR_lineare)`, limitato in basso a 0,2°. [media per la forma, bassa per i numeri]

### 5.2 Ambiguità della schiera trainata e *baffles* [alta per il concetto] [Baff]; [TB16]
- La schiera lineare misura l'angolo conico rispetto al proprio asse: ogni contatto ha un'**immagine speculare** simmetrica rispetto all'asse (ambiguità sinistra/destra). Si risolve con una virata di 20–40°: il rilevamento vero resta coerente, quello falso salta.
- Zona cieca o degradata all'endfire anteriore, dove c'è la propria nave (rumore, settore di circa ±20–30°). L'endfire posteriore è più libero.
- TB‑16: schiera di 240 ft rimorchiata con 2400 ft di cavo [TB16]; altre fonti dicono 2600 ft.
- Durante e dopo una virata la schiera è curva e i rilevamenti sono inaffidabili per circa 2–5 minuti. [bassa]
- La schiera trainata **elimina in pratica i baffles**: è il motivo per cui il "Crazy Ivan" diventò meno utile [Baff].

### 5.3 Contro‑scoperta dell'attivo [alta per il principio]
- Il bersaglio (o un terzo) intercetta il ping con perdita solo di sola andata: `SE_int = SL − TL(R) − NL + DI_int − DT_int`, contro `SL − 2·TL + TS …` per l'eco. In acqua profonda la distanza d'intercettazione è tipicamente **1,5–3 volte** la distanza di scoperta attiva. [media]
- Ogni ping rivela inoltre la classe del sonar (frequenza, tipo d'impulso), quindi la classe della piattaforma. Nel gioco un ping deve generare per tutti i ricevitori in portata un contatto con rilevamento immediato e identificazione del tipo di sonar. [alta, concetto]
- Dottrina SSN: l'attivo si usa quasi solo per la distanza finale di tiro (un singolo ping, "one ping only") o sotto il ghiaccio e contro le mine. [media] [Hervey]; [Cote]

---

## 6. Siluri: tattica ed evasione

### 6.1 Armi del periodo
| Arma | Ruolo / guida | Velocità | Portata | Note | Conf. / fonte |
|---|---|---|---|---|---|
| **Mk 48** (1972) | ASW e antinave; filoguidato; autoguida attiva e passiva; riattacco | 55 kt dichiarati da fonti aperte | 34 430 yd (31,5 km) a 55 kt; 44 550 yd (40,7 km) a 40 kt | Profondità ufficiale > 1200 ft (riportata ~3000 ft); testata 650 lb | [media] [Mk48] |
| **Mk 48 ADCAP** (1988) | Come sopra, elettronica e sensori migliori | 55 kt | 42 530 yd (38,9 km) a 55 kt; 54 685 yd (50 km) a 40 kt | Primo lancio reale 23‑07‑1988 (USS Norfolk vs ex Jonas K. Ingram) | [media] [Mk48] |
| **Mk 46** (leggero, da aereo, ASROC o tubo) | Attivo/passivo, ricerca a elica | ~40–45 kt | ~11 km | Testata ~44 kg | [bassa] |
| **Sub Harpoon / Tomahawk** | Antinave (da tubo) | — | 70 nm / 250 nm | Fuori tema acustico | [media] |
| **53‑65K** (URSS) | Antinave, autoguida sulla scia | 45 kt | ~19 km | — | [media] [T53] |
| **65‑76** (650 mm) | Antinave, scia, senza filo | 50 kt / 30–35 kt | 50 / 100 km | Testata ~450–765 kg (le fonti divergono) | [media] [T65] |
| **SET‑65** | ASW, attivo/passivo | 40 kt | ~15 km | — | [media] [navweaps] |
| **TEST‑71** | ASW filoguidato elettrico | ~40 kt | ~20 km | — | [bassa] |
| **USET‑80** | ASW e antinave, attivo/passivo | 45–48 kt | ~18 km | — | [bassa/media] [navweaps]; [GS] |

### 6.2 Sequenza d'attacco e parametri preimpostati [media] — [Mk48]; [Hervey]; [Friedman]
1. Soluzione TMA sufficiente (distanza ±10–20%, rotta ±20°).
2. Preparazione dell'arma: allagamento del tubo, equalizzazione, apertura della porta esterna (transitori udibili). [media]
3. Parametri impostati prima del lancio:
   - modo (attivo, passivo, misto);
   - velocità (bassa per l'avvicinamento silenzioso, alta per l'attacco);
   - **RTE (*run‑to‑enable*)**, cioè la distanza o il tempo dopo cui la testa autocercante si attiva (tipicamente 1–3 km prima della posizione prevista del bersaglio);
   - **soffitto e pavimento** di ricerca (protezione dalla superficie e dal fondo; il soffitto evita le acquisizioni sulla superficie e sulla nave lanciante);
   - quota di ricerca (sopra o sotto il layer);
   - schema di ricerca;
   - protezione anti‑circolare (ACR), che impedisce di ritornare verso il lanciatore.
4. **Filoguida:** il filo si srotola sia dal siluro sia dal sottomarino. Durante la guida il sottomarino deve evitare alta velocità e virate brusche, pena la rottura del filo; i dati della testa tornano lungo il filo, così l'operatore può correggere la rotta, rifiutare un esca e riassegnare il bersaglio [Mk48]. Il filo si taglia al momento dell'acquisizione finale oppure quando il sottomarino deve manovrare per evadere. [media]
5. **Abilitazione (*enable*):** la testa cerca. Portata d'acquisizione tipica della testa ad alta frequenza circa 1–2 km sul sottomarino (attivo) e maggiore in passivo su bersagli rumorosi. [bassa]
6. **Ricerca:**
   - **serpentina (*snake*)**, oscillazione di ±20–45° intorno alla rotta mentre avanza;
   - **circolare o a spirale** dopo il punto di attivazione o dopo la perdita del contatto;
   - **riattacco** dopo un passaggio a vuoto.
   [media]
7. **Spoletta:** a contatto o di prossimità (magnetica o acustica). Contro le navi l'esplosione sotto la chiglia sfrutta la bolla (frattura della trave di scafo).

### 6.3 Contromisure [media/bassa]
- **Rumoristi** (*noisemakers*), cioè cartucce espulse da sottomarini e navi: saturano la testa passiva per 3–10 minuti. Sui sottomarini USA esistono gli ADC (*Acoustic Device Countermeasure*), lanciati dal tubo segnali da 3"; sui sovietici le cartucce MG‑34 e i generatori di bolle tipo GIP. [bassa sui nomi]
- **Esche mobili** che imitano il rumore e l'eco del sottomarino (MG‑74 sovietica, decoy mobili USA da tubo segnali): seducono la testa attiva o passiva. [bassa]
- Esca rimorchiata da nave di superficie **AN/SLQ‑25 "Nixie"** (anni '80). [media]
- ***Knuckle*:** virata con tutta la barra a velocità elevata. La scia turbolenta e la nube di bolle restituiscono un'eco "falsa" persistente (1–3 minuti) e mascherano il rumore. [media, concetto]
- **Cambio di quota attraverso il layer:** la testa che cerca sopra il layer perde il contatto con il bersaglio sotto (vedi la penalità ΔTL_layer). [media]

### 6.4 Dottrina d'evasione (sintesi per l'IA) [media] — [Hervey]; prassi descritta in [PM04]
1. **Allarme** su transitorio di lancio o rumore di eliche ad alto regime (siluro: tonali sopra 1 kHz e rilevamento che varia rapidamente).
2. **Contro‑tiro immediato** (*snap shot*) lungo il rilevamento del lancio, se un tubo è pronto: costringe il lanciatore a manovrare e a rompere il filo.
3. **Virata per mettere il siluro a poppa** (rilevamento relativo circa 150–180°), alla massima velocità ("sprint"), lanciando un'esca o un rumorista **al momento della virata** (il knuckle e l'esca restano dove il bersaglio "era").
4. **Cambio di quota** attraverso il layer, verso l'alto o il basso a seconda del soffitto o pavimento probabile e del fondale.
5. Quando il siluro ha seguito l'esca o ha superato la quota: **rallentare** (5–8 kt) per ridurre il rumore e uscire dalla zona di ricerca.
6. **Esaurimento della corsa:** l'evasione riesce se il siluro esaurisce l'autonomia, cioè se `R₀/(V_t − V_s) > T_rimasto`.

   Esempio [calcolo]: Mk 48 lanciato a 10 km, autonomia a 55 kt circa 22,6 min. Contro un bersaglio a 30 kt in fuga diretta, l'inseguimento finale richiede circa 10 km / 25 kt ≈ 13 min, quindi non si sfugge. Contro i siluri sovietici a 40–45 kt con portata di 15–20 km, uno Los Angeles a 30+ kt può spesso esaurirli.

---

## 7. Dottrina

### 7.1 SSN statunitensi (ASW e antinave)
- **Fasi della lotta antisom secondo Cote** [media] [Cote] via [NSL‑rev]: (1) 1945–50 schiere passive e SSK; (2) 1950–60 rivoluzione nucleare e triade ASW (sottomarini, boe da aerei e navi, SOSUS); (3) 1960–80 "tempi felici" della superiorità acustica USA; (4) anni '80 parità acustica sovietica e Strategia Marittima. Colpi gravi: rete Walker e fresatrice Toshiba a 9 assi.
- **Strategia Marittima degli anni '80** [media] [CNA18]; [USNI89]: gli SSN USA dovevano entrare sotto il ghiaccio artico e nel Mare di Okhotsk per minacciare gli SSBN sovietici nei bastioni, difesi dagli SSN sovietici. L'Akula complicò molto la missione. La Marina USA accettò la teoria dei bastioni solo dopo prove d'intercettazione.
- **Missioni tipiche di uno SSN‑688:**
  - barriera nei passaggi obbligati (GIUK, Capo Nord–Svalbard, stretti giapponesi);
  - pedinamento (*trail*) di SSBN e SSN;
  - supporto diretto al gruppo portaerei (DS), posizionato avanti lungo la direzione della minaccia;
  - attacco ai gruppi navali con Mk 48, Harpoon o Tomahawk;
  - raccolta d'informazioni.

  [media] [PM04]; [Hervey]
- **Sprint‑and‑drift** [media; formula alta]: alternare uno *sprint* veloce, sordo ma che guadagna posizione, a un *drift* lento in ascolto. Velocità d'avanzamento:
  `SOA = (V_s·t_s + V_d·t_d)/(t_s + t_d)`
  Esempio: sprint a 25 kt per 10 min e drift a 5 kt per 20 min danno SOA ≈ 11,7 kt, con il 67% del tempo in ascolto efficace. Lo usano anche le navi di superficie con schiera trainata. [bassa per i valori tipici]
- **Pedinamento:** posizione nel settore poppiero del bersaglio, fuori dai suoi baffles ma vicino al loro margine. Distanza tipica di pedinamento nella letteratura divulgativa: da qualche migliaio di yarde a ~10 nm secondo il rumore del bersaglio. Rischio di collisione nei "Crazy Ivan". [bassa]
- **Antinave:** avvicinamento sotto il layer, quota periscopica solo per la conferma finale, salva da 2–4 siluri su bersagli distinti; filo guidato fino all'acquisizione. [media]

### 7.2 Sottomarini sovietici: tattiche
- **"Crazy Ivan"** [media] [Baff]: virata brusca, spesso di 180° o comunque ampia, a intervalli irregolari, per ascoltare nei propri baffles e mettere un eventuale inseguitore davanti a sé. È pericolosa per entrambi: scoperta dell'inseguitore e collisione per inerzia. Variante "angles and dangles": otto, virate e cambi di quota casuali. L'arrivo della schiera trainata l'ha resa meno necessaria.

  Parametri IA suggeriti [bassa]: intervallo casuale di 15–45 min (più breve in zona sospetta), virata di 90–270° a velocità moderata, eventuale contro‑attacco se in modalità aggressiva.
- **Missioni:**
  - anti‑portaerei: SSGN Oscar, Charlie ed Echo II con missili; SSN Victor con SS‑N‑15/16;
  - protezione dei bastioni;
  - interdizione delle linee di comunicazione (secondaria per dottrina, al contrario della percezione occidentale);
  - barriere di SSK (Foxtrot, Tango, Kilo) negli stretti.

  [media] [CIA‑NS]; [PM04]
- **Alfa (pr. 705):** fino a 41 kt immersa, raggiunti in circa 90 s; profondità operativa ~350–400 m; reattore al piombo‑bismuto; molto rumoroso ad alta velocità, simile agli altri SSN sovietici a bassa velocità (eliche ausiliarie). Il ruolo è l'intercettore: scatto contro siluri o bersagli, poi abbandono della scena. [media] [Alfa705]
- **Silenziamento:** a metà anni '80 tre nuove classi di SSN (Akula, Sierra, Mike), con un probabile salto di silenziosità. [media] [CIA‑NS] Lo studio della Camera dei Rappresentanti riportato in [USNI‑quiet] riferisce che l'Akula migliorato era più silenzioso del 688I, ma non che tutti i sovietici fossero più silenziosi.

### 7.3 Bastioni [media‑alta] — [CIA‑bastion]; [CIA‑NS]; [CNA18]; [NSL‑bastion]
- Con SLBM a testate multiple e portata intercontinentale (SS‑N‑8, SS‑N‑18, SS‑N‑20, SS‑N‑23), gli SSBN potevano colpire gli USA da acque vicine all'URSS. Da qui la scelta di operare la maggior parte degli SSBN in "bastioni" protetti: **Barents e Artico** per la Flotta del Nord, **Okhotsk** per il Pacifico.
- In guerra, gruppi misti di aviazione navale, unità di superficie e sottomarini, più sensori fissi e campi minati, li avrebbero difesi dalle forze ASW NATO.
- Operazioni sotto il ghiaccio artico per migliorare la sopravvivenza; comunicazioni ELF.
- Cautela [NSL‑bastion]: la letteratura sovietica non ammette esplicitamente una "politica dei bastioni"; il concetto è in parte un'inferenza occidentale.
- **Struttura per il gioco (zone concentriche):** zona interna con SSBN, SSN Victor III/Akula e unità ASW di superficie; zona media con aerei Il‑38 e Tu‑142, sensori fissi e mine nei passaggi; zona esterna con SSN e SSK di barriera (Capo Nord–Orso, GIUK in guerra). [bassa, sintesi]

### 7.4 ASW sovietica
- **Valutazione CIA del 1965:** capacità ASW in mare aperto contro sottomarini nucleari "virtually nil", per mancanza di scoperta iniziale; operazioni efficaci contro i convenzionali vicino alle proprie basi. [media] [CIA65] La valutazione del 1972 (in [CIA72]) tratta sonar sovietici e metodi non acustici. Negli anni '80 cresce la ricerca non acustica (13 istituti, esperimenti con radar spaziali) [CIA‑bulletin] [media].
- **KPUG** (*korabel'naja poiskovo‑udarnaja gruppa*, gruppo nave di ricerca e attacco): 2–4 unità ASW, cioè Krivak, Grisha, Udaloy, Kresta II e Kara (Ka‑25/Ka‑27), con sonar di scafo e VDS.
  - Tattica in linea di fronte, con unità spaziate circa 1–1,5 volte la portata del sonar, alternando ricerca attiva e corse veloci.
  - Il contatto si passa all'elicottero o all'aereo e si attacca con RBU (lanciarazzi ASW, 1–6 km), SS‑N‑14 o siluri.

  [media sul concetto e i tipi, bassa sulle spaziature] ([PM04]; Polmar, *Guide to the Soviet Navy*)
- **Aerei:** Il‑38 "May" e Tu‑142 "Bear‑F" con boe RGB e MAD; Be‑12 "Mail"; elicotteri Ka‑25 "Hormone‑A" e Ka‑27 "Helix‑A" con sonar a immersione (serie VGS "Oka") e boe. [media sui tipi] [Friedman]

### 7.5 Difesa di convogli e portaerei (NATO/USN) [media] — [Cote]; [Hervey]
Difesa a strati:
- **Esterna (oltre 100 nm):** allerta SOSUS, P‑3C con boe, SSN in supporto diretto.
- **Intermedia:** fregate con schiera trainata (TACTAS SQR‑18/19 su FF‑1052 e FFG‑7) ed elicotteri LAMPS (SH‑2F, SH‑60B dal 1984).
- **Interna:** sonar attivi di scafo (SQS‑53/26), elicotteri SH‑3H con sonar AQS‑13 vicino alla portaerei, S‑3A Viking dalla portaerei.

Settori di scorta orientati sulla minaccia e manovre di zig‑zag del gruppo.

### 7.6 Mezzi aerei: boe, MAD, Jezebel
- **Boe sonar** [alta/media]:

| Boa | Tipo | Banda | Quote | Durata | Fonte |
|---|---|---|---|---|---|
| **SSQ‑41 LOFAR** | Passiva omnidirezionale | 10 Hz–~6–10 kHz | 60 / 1000 ft | 1–8 h | [bassa] |
| **SSQ‑53 DIFAR** | Passiva direzionale, dà il rilevamento | taglio inferiore 5 Hz (versione D) | 4 quote fino a 1000 ft (D: 90 / 400 / 1000 ft) | 5 opzioni fino a 8 h | [alta] [SSQ53] |
| **SSQ‑47** | Attiva, solo distanza | — | — | — | [bassa] |
| **SSQ‑62 DICASS** | Attiva direzionale: distanza, rilevamento e Doppler | 4 frequenze | — | — | [alta] [SSQ62] |
| **SSQ‑36** | Batitermografo (profilo T fino a 2625 ft): fornisce la SVP al gioco | — | — | — | [alta] [SSQ36] |

- **Schemi di posa:** barriera lineare (spaziatura circa 1,5–2 volte la portata di scoperta della boa), cerchio intorno al *datum*, chevron e campo. Localizzazione tramite incrocio di rilevamenti DIFAR o fix attivo DICASS, poi conferma MAD e attacco Mk 46. [media]
- **Jezebel/Julie:** LOFAR con ascolto passivo; Julie usava cariche esplosive come sorgente attiva. [media]
- **MAD** [media] [MAD‑wiki]: il campo d'anomalia scende come 1/r³, quindi l'aereo deve passare quasi sulla verticale e il sottomarino deve essere poco profondo. Anomalia di ~0,2 nT a 600 m; ~1,65 nT a 1 km e 0,01 nT a 5 km per un sottomarino di 100 m (modello). Portata pratica dei sistemi del periodo (ASQ‑81) circa 300–700 m di distanza obliqua; il MAD‑XR moderno arriva a ~1200 m. Il P‑3 vola a 200–500 ft in passaggi MAD. Il MAD serve alla localizzazione finale, non alla ricerca. [bassa per i numeri del periodo]

### 7.7 EMCON, periscopio, radar, ESM
- **EMCON:** il sottomarino non emette (niente radar, niente attivo, comunicazioni solo in ricezione via VLF e filo galleggiante, oppure ELF per i sovietici); trasmette brevi raffiche (*burst*) solo a quota periscopica. La nave di superficie in EMCON spegne radar e sonar attivo e si affida a ESM e passivo. [media]
- **Esposizione del periscopio:** un equipaggio disciplinato limita l'esposizione a **meno di 10 secondi** nelle fasi d'attacco [alta] [SBIR‑mast]. Prassi tipica: giro rapido d'orizzonte, poi brevi alzate di 3–8 s. A velocità superiore a circa 5–8 kt il "pennacchio" (*feather*) diventa visibile.
- **Radar contro antenne:**
  - **Orizzonte radar** (atmosfera 4/3): `d (nm) = 1,23·(sqrt(h_r ft) + sqrt(h_t ft))`. Esempi: aereo a 1000 ft contro periscopio a 3 ft → 41 nm; a 500 ft → 30 nm; nave con radar a 60 ft → 11,7 nm. [alta per la formula, calcolo]
  - Ogni esposizione d'antenna è rilevabile da un radar aereo; contro brevi esposizioni servono soglie sensibili e, in acque costiere, il clutter satura gli operatori [APL97]. I radar per periscopi richiedono quote di 150–450 m [SBIR‑mast].
  - Portate di gioco [bassa]: periscopio rilevato da un radar MPA a 4–8 nm con SS1–2 e a 1–2 nm con SS4; snorkel a 10–20 nm con SS1–2.
- **ESM:** il segnale radar percorre una sola andata, quindi l'ESM del sottomarino rileva il radar ben oltre la portata a cui quel radar vedrebbe il periscopio (tipicamente 1,5–3 volte o fino all'orizzonte). Dà rilevamento e tipo d'emettitore e permette di immergersi prima della scoperta. [alta per il principio, bassa per i fattori]

---

## 8. Meteo, stato del mare, giorno e notte

| Fattore | Effetto | Conf. |
|---|---|---|
| Stato del mare ≥ 4 | Clutter radar di mare forte: la scoperta radar di periscopi e snorkel crolla. Vedetta: creste che mascherano il pennacchio | [media] [APL97] |
| Stato del mare ≥ 5 | Sottomarino a quota periscopica difficile da tenere (oscillazioni, "broaching" rischioso); conviene ~20–25 m di chiglia o più profondo | [bassa] |
| Stato del mare ≥ 5–6 | Sonar di scafo delle navi degradati da beccheggio, rumore di flusso e bolle sotto la cupola (*quenching*): la nave rallenta a 10–15 kt. Schiere trainate e VDS meno sensibili | [media] |
| Vento > 35–45 kt / SS6+ | Elicotteri ASW: sonar a immersione e appontaggio limitati o impossibili. Operazioni aeree dalla portaerei ridotte | [bassa] |
| Lancio di boe | Possibile anche con mare forte, ma il rumore di superficie riduce la portata delle boe a bassa quota | [media] |
| Pioggia | NL più alto in 1–30 kHz (§4.4). Visibilità ottica ridotta. Clutter radar da pioggia (banda X) | [media] |
| Nebbia | Periscopio visivo inutile; il radar del sottomarino a quota periscopica diventa tentazione e rischio (ESM nemico) | [alta, ovvio] |
| Notte | Avvistamento visivo del periscopio quasi nullo senza luna o bioluminescenza. Periscopio con intensificatore o termico (anni '80) a portate ridotte. Il DSL risale: riverberazione di volume più alta vicino alla superficie | [media] |
| Giorno (pomeriggio) | Effetto pomeriggio (§3.5) | [alta] [IW42] |
| Inverno ad alte latitudini | Strato rimescolato profondo, ghiaccio, notte polare, SS alto: buone portate a bassa frequenza, pessime operazioni aeree | [media] |
| Ghiaccio | Sotto il pack niente periscopio né radar nemico (tranne polinie e canali); il rumore di fessurazione genera falsi allarmi | [media] |

---

## 9. Danni e riparazioni

### 9.1 Allagamento, efflusso da orifizio [alta per la formula]
`Q = C_d·A·sqrt(2·Δp/ρ)`, con `Δp = ρ·g·h`, quindi `v = sqrt(2·g·h)`; C_d ≈ 0,6–0,8 (spigolo vivo ≈ 0,62); ρ = 1025 kg/m³.

Portate calcolate con C_d = 0,62, in m³/s (tra parentesi t/min) [media, calcolo]:

| h (m) | Δp (bar) | v (m/s) | foro Ø 1" | 2" | 5" | 30 cm | 1 m |
|---|---|---|---|---|---|---|---|
| 20 (quota periscopica) | 2,0 | 19,8 | 0,006 (0,4) | 0,025 (1,5) | 0,16 (9,6) | 0,87 (53) | 9,6 (593) |
| 100 | 10,1 | 44,3 | 0,013 (0,8) | 0,056 (3,4) | 0,35 (21) | 1,94 (119) | 21,6 (1327) |
| 200 | 20,1 | 62,6 | 0,019 (1,2) | 0,079 (4,8) | 0,49 (30) | 2,75 (169) | 30,5 (1876) |
| 300 (≈ quota di collaudo del 688) | 30,2 | 76,7 | 0,023 (1,4) | 0,096 (5,9) | 0,60 (37) | 3,36 (207) | 37,4 (2298) |
| 400 | 40,2 | 88,6 | 0,027 (1,7) | 0,111 (6,8) | 0,70 (43) | 3,88 (239) | 43,1 (2653) |

- **Thresher (1963):** la Corte d'Inchiesta ritenne più probabile un allagamento in sala macchine da un orifizio di 2–5 pollici, aggravato dalla perdita di potenza del reattore. A 300 m un foro da 5" fa entrare circa 37 t/min.
  - Le pompe di assetto e di sentina avevano portata bassissima alla quota di collaudo.
  - L'esaurimento d'emergenza delle casse zavorra principali (MBT) poteva compensare solo allagamenti modesti.
  - Ghiaccio nei filtri e nelle valvole di riduzione dell'aria (effetto Joule‑Thomson) ridusse il flusso d'aria.
  - Ne nacque il programma SUBSAFE.

  [media] [USNI21]; [USNI23]
- **Zavorra d'emergenza e legge di Boyle:** il volume d'aria disponibile dalle bombole HP si riduce in proporzione alla pressione assoluta (1 bar in superficie; 11,1 bar a 100 m; 31,2 bar a 300 m; 46,3 bar a 450 m). A 300 m la stessa massa d'aria svuota ~1/31 del volume che svuoterebbe in superficie. L'aria si espande durante la risalita, quindi la manovra è auto‑accelerante una volta avviata. [alta, fisica]

### 9.2 Dottrina di risposta a un allagamento (SSN USA, sintesi) [media] — prassi post‑Thresher; [USNI21]; [PM04]
1. Allarme ("Flooding in …").
2. Chiusura delle valvole di scafo e di quelle di riserva nel compartimento.
3. **Avanti tutta** e **barra piena in salita** sugli idroplani: la portanza dinamica è più rapida della zavorra.
4. **Esaurimento d'emergenza** delle MBT (EMBT).
5. Ridurre la quota il prima possibile: Q ∝ sqrt(h), e risalire riduce sia la portata sia lo sforzo sullo scafo.
6. Isolamento del compartimento.
7. Pompe di sentina e di assetto (efficaci solo a bassa quota).

**Priorità di riparazione** (Damage Control USN; NSTM cap. 079 e 555) [media]: (1) allagamento e integrità stagna; (2) incendio, con maschere EAB perché l'atmosfera chiusa diventa letale in pochi minuti; (3) propulsione e reattore (dopo uno SCRAM: batteria più motore di propulsione d'emergenza EPM a bassa velocità, recupero del reattore in circa 5–20 min [bassa]); (4) governo (idraulica degli idroplani e del timone, comando d'emergenza); (5) sensori, armi, comunicazioni.

### 9.3 Compartimentazione e riserva di spinta [media/bassa]
- Il 688 ha pochi compartimenti stagni principali: compartimento prodiero (controllo, alloggi e locale siluri su 3 ponti), compartimento reattore, sala macchine. Un allagamento grave in sala macchine o a prua oltre circa 100 m è spesso irrecuperabile. **[media]** [PM04]
- Riserva di spinta: SSN USA a scafo singolo circa 10–15%; sovietici a doppio scafo circa 25–30% (più sopravvivenza ai danni). Profondità di collaudo del 688 circa 950 ft (290 m). Profondità di collasso circa 1,5 volte quella di collaudo (regola di progetto spesso citata). Alfa e Sierra con scafo in titanio, più profondi. [media/bassa] [PM04]; [Alfa705]

### 9.4 Esplosioni subacquee e urto [bassa per le soglie]
- **Fattore d'urto dello scafo:** `HSF = sqrt(W)/R` (W = libbre di TNT equivalente, R = piedi). **Fattore d'urto della chiglia:** `KSF = HSF·(1 + sin θ)/2` (θ = angolo d'incidenza dall'orizzontale).
- Soglie di gioco suggerite: KSF 0,05–0,1 guasti a strumenti e luci; 0,1–0,3 danni ad apparecchiature non sospese, perdite da tenute e tubolature, possibile SCRAM del reattore; 0,3–0,6 allagamenti da rotture localizzate; > 0,6–1 rottura dello scafo resistente, perdita del battello.
- Siluro Mk 48 (650 lb) esploso a 10 m dallo scafo (33 ft): HSF ≈ 0,77, quindi letale o quasi. Lo scafo già sollecitato in profondità ha soglie più basse: si scala la soglia × (1 − 0,5·h/h_collaudo). [bassa, modello dell'autore]
- **Colpo vicino** (*near miss*): effetti secondari via onda d'urto e bolla. La bolla sotto la chiglia di una nave causa la frattura della trave di scafo. [media]

---

## 10. Modello consigliato per il gioco

### 10.1 Ambiente (per cella, griglia 5–20 nm, aggiornato ogni 10–60 min di gioco)
- **Batimetria** (ETOPO/GEBCO a 1–2′) e tipo di fondale (roccia, sabbia, fango), con perdita per riflessione BL(θ, f) da tabella.
- **SVP parametrica:** SST, MLD (= SLD), gradiente e spessore del termoclino, profilo profondo per teatro e stagione (da tabella §3.6, idealmente da climatologia WOA). c(z) con Mackenzie. Più un microstrato diurno (effetto pomeriggio).
- **Meteo:** vento w (m/s) → stato del mare; pioggia R (mm/h); nuvolosità; ghiaccio (frazione, tipo); traffico s ∈ [0,1]; indici biologici (gamberi, balene, DSL).

### 10.2 Perdita di trasmissione TL(f, r, z_s, z_r, azimut)
- **Bande di calcolo:** 50, 150, 300, 1000, 3500, 10 000 e 25 000 Hz.
- **Livello A, analitico ed economico** (fallback, unità lontane):
  `TL = 20·log r` (r < r_t) oppure `20·log r_t + 10·log(r/r_t)` (r ≥ r_t), più `α(f)·r/1000`, più `ΔTL_layer(z_s, z_r, f)`, più `ΔTL_LM`, meno `G_CZ(r)`, più BB e ombre.
  - r_t = min(fondale, H_duct) secondo il percorso.
  - α da Ainslie‑McColm.
  - G_CZ solo se l'eccesso di profondità è ≥ 200 m.
  - In acqua bassa: Marsh‑Schulkin.
- **Livello B, consigliato:** *ray tracing* 2D incoerente lungo N radiali (Nx2D, 36–72 azimut), con profilo dipendente dalla distanza (batimetria e fronti).
  - Ventaglio di 100–400 raggi da −30° a +30°, passo adattivo, legge di Snell a gradiente lineare per strati (archi di cerchio esatti).
  - Intensità per fascio gaussiano o per tubo di raggi; perdite di superficie (Eckart) e di fondo (BL).
  - Calcolato in un thread di background **per sorgente rilevante** e cache su griglia (r ≤ 120 nm a passi di 0,25–0,5 nm, z a passi di 10–25 m), ricalcolato quando l'ambiente cambia o la sorgente si sposta di più di 2–5 nm.
  - Riferimento di validazione offline: BELLHOP e KRAKEN (Acoustics Toolbox, Porter) [Jensen].
- **Modi normali:** non consigliati in tempo reale. Utili solo per pre‑calcolare tabelle in acqua bassa (Barents, Golfo Persico).

### 10.3 Sorgenti
- **Larga banda:** `SL_bb(f, V, z) = SL_ref(f; classe) + 60·log(max(V, V_mach)/V_ref) + C(V, z)·ΔCav(f)`.
  - Cavitazione `C = smoothstep(V_inc(z), V_inc(z) + 2 kt, V)`, con `V_inc(z) = V_inc0·sqrt((10,33 + z_elica)/10,33)`.
  - ΔCav(f) = +15 dB per f > 100 Hz.
- **Tonali:** lista per classe {frequenza, livello, condizione (pompe lente/veloci, turbogeneratore, 50/60 Hz, ausiliari), stabilità} più la riga a f_BR = Z·RPM/60 (più forte sotto cavitazione) e la modulazione DEMON.
- **Transitori:** eventi con livello di picco e durata (porta del tubo, lancio, sbattere di un portello, ritiro della schiera…).

### 10.4 Rumore al ricevitore
`NL = 10·log(10^(N_turb/10) + 10^(N_ship/10) + 10^(N_wind/10) + 10^(N_th/10) + 10^(N_rain/10) + 10^(N_bio/10) + 10^(N_ice/10) + 10^(N_self(V, settore)/10))`

Vento limitato a 15 m/s; N_self dipende dalla velocità e dal settore (baffles).

### 10.5 Rilevazione e tracciamento
- Per ogni coppia sensore‑sorgente e banda, ogni 1–4 s:
  - SE con l'equazione §1.1;
  - DT da §1.2 (W, t, d dati dal sistema, più un modificatore di abilità dell'operatore ±3 dB);
  - fluttuazione AR(1) con σ = 3–6 dB;
  - `Pd_istantanea = Φ(SE/σ)`.
- **Stati del contatto:** *rilevato* (SE > 0 su un fascio), poi *tracciato* (k su n rilevazioni, per es. 3 su 5), poi *classificato* (SE_nb > +3 dB su almeno 2 tonali di libreria, oppure DEMON valido), poi *identificato*. Isteresi di −3 dB per perdere il contatto.
- **Errore di rilevamento** `σ_B = θ₃dB/sqrt(SNR)` (≥ 0,2°); ambiguità speculare per la schiera trainata; settore cieco nei baffles.
- **TMA:** filtro di Kalman esteso o stimatore ai minimi quadrati sui rilevamenti (più Doppler se disponibile), con covarianza che resta grande senza manovra propria. Per il giocatore: strumenti MATE, regola del 1934 ed Ekelund (§5.1).

### 10.6 Parametri chiave da tarare (tutti [bassa])
V_inc0 per classe, SL_ref per classe, livelli delle tonali, TS per aspetto e riduzione anecoica, ΔTL_layer, G_CZ, α_L del condotto, σ della fluttuazione, abilità dell'operatore, portata d'acquisizione dei siluri, tempo di raddrizzamento della schiera, intervalli del Crazy Ivan, soglie di urto.

### 10.7 Costanti
- c di riferimento 1500 m/s; ρ = 1025 kg/m³; g = 9,81 m/s²; 1 atm = 10,33 m d'acqua di mare.
- 1 kt = 0,5144 m/s = 33,756 yd/min; 1 nm = 1852 m = 2025,4 yd.
- 1934 (°/min · yd/kt); 170,8 dB (W → SL).
- Mackenzie, Ainslie‑McColm e Coates come sopra.

---

## 11. Bibliografia

**Testi (non tutti consultati direttamente online: le pagine indicate sono citate da fonti secondarie)**
- **[U83]** R. J. Urick, *Principles of Underwater Sound*, 3ª ed., McGraw‑Hill / Peninsula Publishing, 1983. Capitoli: equazioni del sonar, propagazione, rumore ambiente, riverberazione, TS (pp. 331–341 per i sottomarini, come citato in [KR24]), rumore irradiato, rumore proprio, soglia di rilevazione.
- **[Ross76]** D. Ross, *Mechanics of Underwater Noise*, Pergamon, 1976.
- **[Jensen]** F. B. Jensen, W. A. Kuperman, M. B. Porter, H. Schmidt, *Computational Ocean Acoustics*, 2ª ed., Springer, 2011.
- **[Ainslie10]** M. A. Ainslie, *Principles of Sonar Performance Modelling*, Springer, 2010.
- **[Waite]** A. D. Waite, *Sonar for Practising Engineers*, 3ª ed., Wiley, 2002.
- **[WMS]** D. H. Wagner, W. C. Mylander, T. J. Sanders (a cura di), *Naval Operations Analysis*, 3ª ed., Naval Institute Press, 1999.
- **[PM04]** N. Polmar, K. J. Moore, *Cold War Submarines*, Brassey's, 2004.
- **[Hervey]** J. B. Hervey, *Submarines* (Brassey's Sea Power vol. 7) / *Submarine Warfare Today*; non consultato direttamente.
- **[Friedman]** N. Friedman, *The Naval Institute Guide to World Naval Weapon Systems*, edizioni 1989 e 1991.
- **[Coates89]** R. F. W. Coates, *Underwater Acoustic Systems*, Macmillan, 1989.

**Articoli e rapporti**
- **[W62]** G. M. Wenz, "Acoustic ambient noise in the ocean: spectra and sources", *JASA* 34, 1936–1956 (1962). https://tethys.pnnl.gov/publications/acoustic-ambient-noise-ocean-spectra-sources
- **[KAE48]** V. O. Knudsen, R. S. Alford, J. W. Emling, *J. Mar. Res.* 7, 410 (1948); fit della formula in https://arxiv.org/pdf/0712.1833
- **[M81]** K. V. Mackenzie, "Nine‑term equation for sound speed in the oceans", *JASA* 70(3), 807–812 (1981), doi:10.1121/1.386920
- **[NPL‑SS]** NPL, *Speed of sound in sea water – underlying physics* (Mackenzie, Coppens, UNESCO/Wong‑Zhu, Del Grosso). http://resource.npl.co.uk/acoustics/techguides/soundseawater/underlying-phys.html
- **[FG82]** R. E. Francois, G. R. Garrison, *JASA* 72(3), 896–907 e 72(6), 1879–1890 (1982). https://data-ww3.ifremer.fr/BIB/Francois_Garrison_JASA1982.pdf ; implementazione: https://www.ldeo.columbia.edu/res/pi/MB-System/mbweb/html/mbabsorption.html
- **[AM98]** M. A. Ainslie, J. G. McColm, "A simplified formula for viscous and chemical absorption in sea water", *JASA* 103(3), 1671–1672 (1998).
- **[NPL‑ABS]** http://resource.npl.co.uk/acoustics/techguides/seaabsorption/physics.html
- **[Th67]** W. H. Thorp, *JASA* 42, 270 (1967).
- **[ES310]** USNA/FAS, *Fundamentals of Naval Weapons Systems*, "Sonar Propagation". https://man.fas.org/dod-101/navy/docs/es310/SNR_PROP/snr_prop.htm
- **[CS76]** A. B. Coppens, J. V. Sanders, *An Introduction to the Sonar Equations with Applications*, NPS, 1976. https://apps.dtic.mil/sti/pdfs/ADA071133.pdf
- **[H61]** F. E. Hale, "Long‑range sound propagation in the deep ocean", *JASA* 33 (1961). https://labs.sonicfield.org/library/long-range-sound-propagation-in-the-deep-ocean-1959
- **[U65]** R. J. Urick, "Caustics and convergence zones in deep‑water sound transmission", *JASA* 38 (1965). https://labs.sonicfield.org/library/caustics-and-convergence-zones-in-deep-water-sound-transmission
- **[DTIC‑A048759]** Effetto dei fronti sulla distanza delle CZ. https://apps.dtic.mil/sti/pdfs/ADA048759.pdf ; esercitazione Atlantico NE: https://apps.dtic.mil/sti/pdfs/ADC029546.pdf
- **[IW42]** C. O'D. Iselin, A. H. Woodcock, *Preliminary report on the prediction of "afternoon effect"*, WHOI, 1942. https://darchive.mblwhoilibrary.org/handle/1912/2044
- **[Stoj07]** M. Stojanovic, "On the relationship between capacity and distance in an underwater acoustic communication channel", *ACM WUWNet* 2006 / *MC2R* 2007 (modello Coates). Formule in https://arxiv.org/pdf/1801.03641 ; ns‑3: https://www.nsnam.org/docs/release/3.30/doxygen/uan-noise-model-default_8cc_source.html
- **[JASA‑EL23]** Saturazione del rumore da vento oltre 15 m/s. https://doaj.org/article/960aff293ae840afb0af71fb89679f82
- **[NRC03]** National Research Council, *Ocean Noise and Marine Mammals*, 2003. https://www.nationalacademies.org/read/10564/chapter/15
- **[Front21]** Gamberi schioccatori, ~190 dB pp, 2–5 kHz. https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2021.779283/pdf ; **[KIOST]** https://sciwatch.kiost.ac.kr/handle/2020.kiost/28503
- **[GZ94]** Greening & Zakarauskas, modello del rumore artico a due componenti, *Canadian Acoustics*. https://jcaa.caa-aca.ca/index.php/jcaa/article/view/740 ; **[Milne72]** fessurazione termica, AD0745631. https://openpolar.no/Record/ftdtic:AD0745631
- **[Pereira16]** Specchio di Lloyd. https://research-repository.st-andrews.ac.uk/bitstream/10023/15966/1/Pereira_2016_Lloyds_mirror_effect_PMA_070002.pdf
- **[AA22]** Propagazione nel Golfo Persico (vortici, T/S), *Archives of Acoustics* 47(3), 2022. https://acoustics.ippt.pan.pl/index.php/aa/article/view/3029
- **[IIAV14]** Scala del rumore con V⁶ e insorgenza della cavitazione. https://iiav.org/ijav/content/volumes/19_2014_1148751395224895/vol_4/19_4_number_1071241419332454.pdf
- **[DFO49]** Misura della TS di un sottomarino, 1949. https://waves-vagues.dfo-mpo.gc.ca/Library/40679706.pdf ; **[KR24]** TS bistatica di sottomarini (cita Urick pp. 331–341). https://www.koreascience.or.kr/article/JAKO202408737595754.page
- **[NA81]** S. C. Nardone, V. J. Aidala, "Observability criteria for bearings‑only target motion analysis", *IEEE Trans. AES* 17(2), 1981.
- **[TMA‑wiki]** https://en.wikipedia.org/wiki/Target_Motion_Analysis ; **[US4327500]** brevetto per il regolo di Ekelund ; **[US5877998]** TMA ricorsiva ; **[NHHC‑Ekelund]** https://www.history.navy.mil/about-us/leadership/director/directors-corner/in-memoriam/ekelund-obit.html

**Sistemi d'arma e sensori**
- **[Mk48]** https://man.fas.org/dod-101/sys/ship/weaps/mk-48.htm
- **[T53]** https://en.wikipedia.org/wiki/Type_53_torpedo ; **[T65]** https://en.wikipedia.org/wiki/Type_65_torpedo ; **[navweaps]** https://navweaps.com/Weapons/WTRussian_post-WWII.php ; **[GS]** https://www.globalsecurity.org/military/world/russia/t-65-torpedo.htm
- **[SQS53]** https://www.navysbir.com/n24_2/N242-D11.htm ; https://man.fas.org/dod-101/sys/ship/weaps/an-sqs-53.htm ; **[SQS26]** https://en.wikipedia.org/wiki/AN/SQS-26
- **[BQQ5]** https://man.fas.org/dod-101/sys/ship/weaps/an-bqq-5.htm ; **[TB16]** https://man.fas.org/dod-101/sys/ship/weaps/tb-16.htm
- **[SSQ53]** https://man.fas.org/dod-101/sys/ship/weaps/an-ssq-53.htm ; **[SSQ62]**, **[SSQ36]**: dati dal Federal Register 2019 (https://public-inspection.federalregister.gov/2019-27491.pdf) e dalla scheda Ultra (https://www.ultra.group/media/2566/ul6_9659-sonobuoy-ref-card-issue-2021.pdf)
- **[MAD‑wiki]** https://en.wikipedia.org/wiki/Magnetic_anomaly_detector ; **[Baff]** https://en.wikipedia.org/wiki/Baffles_(submarine)
- **[Alfa705]** https://www.globalsecurity.org/military/world/russia/705-dev.htm
- **[SBIR‑mast]** https://www.navysbir.com/n19_2/N192-059.htm ; **[APL97]** Ousborn et al., *JHU APL Tech. Digest* 18(1). https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V18-N01/18-01-Ousbourn.pdf

**Dottrina, intelligence e storia**
- **[Cote]** O. R. Coté Jr., *The Third Battle: Innovation in the U.S. Navy's Silent Cold War Struggle with Soviet Submarines*, Newport Paper 16, NWC Press, 2003. https://apps.dtic.mil/sti/pdfs/ADA421957.pdf (DTIC blocca il download automatico; contenuto preso dalla recensione **[NSL‑rev]** https://archive.navalsubleague.org/?p=4211)
- **[CIA‑NS]** CIA, *Soviet Naval Strategy and Programs Through the 1990s* (seguito della NIE 11‑15‑82/D). https://cia.gov/readingroom/document/cia-rdp87t00126r000600700004-3
- **[CIA‑bastion]** CIA FOIA, documento sui bastioni SSBN. https://www.cia.gov/readingroom/print/1697117 (contenuto visto tramite estratti di ricerca)
- **[CIA65]** OSI, *Trends in Soviet ASW Capabilities*, 1965. https://www.cia.gov/readingroom/node/332883 ; **[CIA72]** *Soviet Antisubmarine Warfare: Current Capabilities and Priorities*, 1972. https://www.cia.gov/readingroom/print/2062281 ; *The Soviet Attack Submarine Force: Evolution and Operations*: https://cia.gov/readingroom/node/2062267 ; **[CIA‑bulletin]** https://www.cia.gov/readingroom/print/1903188 ; valutazione netta SSN/ASW 1988: https://www.cia.gov/readingroom/node/1986685
- **[CNA18]** CNA, CAB‑2018‑U‑017105 (Strategia Marittima, bastioni, Akula). https://www.cna.org/CNA_files/PDF/CAB-2018-U-017105-Final.pdf
- **[NSL‑bastion]** https://archive.navalsubleague.org/?p=868 ; **[USNI89]** "Soviet Maritime Strategy", *Proceedings*, settembre 1989. https://www.usni.org/magazines/proceedings/1989/september/soviet-maritime-strategy
- **[USNI‑quiet]** "What quieting means to the Soviets", *Proceedings*, 1989. https://www.usni.org/magazines/proceedings/1989/april/what-quieting-means-soviets ; **[Murphy97]** "Catch the quiet ones", *Proceedings*, 1997. https://www.usni.org/magazines/proceedings/1997/july/catch-quiet-ones ; **[ONI97]** testimonianza *Worldwide Submarine Challenges*, 1997. https://irp.fas.org/congress/1997_hr/s970408c.htm
- **[USNI21]** "What did the Thresher court of inquiry find", *Proceedings*, agosto 2021. https://www.usni.org/magazines/proceedings/2021/august/what-did-thresher-disaster-court-inquiry-find ; **[USNI23]** "Was Thresher ready for sea?", aprile 2023. https://www.usni.org/magazines/proceedings/2023/april/was-thresher-ready-sea

---

## 12. Lacune note (da colmare)
1. **Livelli irradiati reali** (SL, tonali, V_inc) delle classi sovietiche e del 688: nessuna fonte aperta primaria. La tabella §2.6 è una stima di taratura. Possibili fonti: grafici relativi ONI (fine anni '90) e Polmar & Moore (cap. sul silenziamento), da consultare su carta.
2. **SL e frequenze** dei sonar attivi diversi dall'SQS‑53 (BQQ‑5, SQS‑56, AQS‑13, sonar sovietici): valori [bassa].
3. **Dottrina tattica ufficiale** (NWP 3‑21.x, ATP‑1/ATP‑28 NATO): nessun estratto aperto trovato. Le tattiche di §6–7 vengono da letteratura secondaria.
4. **Coté, *The Third Battle*:** il PDF DTIC non è scaricabile in automatico. Vanno verificati sul testo i dati su Victor III/Akula, trail e SOSUS.
5. ~~**Distanze delle CZ per teatro:** calcolate dall'autore su profili archetipici.~~ *Colmata il 29/10/2026:* §(d), coi profili WOA23 1985-1994 e l'integrale del motore verificato coi raggi. GDEM non ha una distribuzione pubblica dei dati e resta fuori.
6. **MAD ASQ‑81, radar e periscopi, portate ESM:** solo ordini di grandezza.
7. **Soglie di urto e danni:** modello inventato coerente con HSF/KSF. Mancano dati pubblici sui sottomarini.
8. **Coefficienti di Francois‑Garrison e formula di Chapman‑Harris:** riportati dall'autore, da verificare sul PDF originale (Ainslie‑McColm è sufficiente per il gioco).
