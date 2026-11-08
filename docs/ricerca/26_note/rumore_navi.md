# Rumore irradiato delle navi civili — dati per generare firme LOFAR/DEMON (1985-1991)

Scopo: generare la firma acustica di **singole** navi civili per tipo (spettro continuo + righe tonali + modulazione DEMON), con valori plausibili per il periodo 1985-1991.

Etichette di affidabilità:
- **[alta]**: numero letto direttamente nella fonte primaria.
- **[media]**: fonte secondaria, oppure fonte primaria letta solo in parte (figure, abstract).
- **[bassa]**: fonte debole o dati incoerenti fra fonti.
- **STIMA**: ricavato da me con le regole fisiche (sezione 2) o dalla pratica navale generale, senza fonte puntuale.

Unità: SL = livello di sorgente in dB re 1 µPa @ 1 m (banda indicata). Densità spettrale in dB re 1 µPa²/Hz @ 1 m.

---

## 1. Modelli dello spettro continuo

### 1.1 JOMOPANS-ECHO (MacGillivray & de Jong 2021) — modello consigliato come base

Fonte: A. MacGillivray, C. de Jong, "A Reference Spectrum Model for Estimating Source Levels of Marine Shipping Based on Automated Identification System Data", *J. Mar. Sci. Eng.* 9(4):369, 2021, doi:10.3390/jmse9040369, open access CC BY.
URL: https://www.mdpi.com/2077-1312/9/4/369 (PDF: https://mdpi-res.com/d_attachment/jmse/jmse-09-00369/article_deploy/jmse-09-00369.pdf)

Il modello è tarato su 1862 misure di SL (programma ECHO, porto di Vancouver, navi moderne). La sorgente è un **monopolo a 6 m di profondità fissa**. Le formule sotto sono lette dal PDF (eq. 2, 9, 10, 11) [alta].

**Densità spettrale di sorgente** (f in Hz, V in nodi, l = lunghezza):

```
L_Sf(f,V,l,C) = L_Sf0(f,C) + 60·log10(V/V_C) + 20·log10(l/l0)        [dB re 1 µPa² m²/Hz]
l0 = 300 ft (91,44 m), con l nella stessa unità
```

**Spettro base** (tutte le classi):

```
L_Sf0(f,C) = K − 20·log10(f1) − 10·log10( (1 − f/f1)² + D² )
f1 = 480 · (1 kn / V_C)       [Hz]
K = 191 dB;  D = 3 per tutte le classi, tranne Cruise: D = 4
```

L'andamento risultante è piatto sotto f1, con un leggero massimo vicino a f1 e una pendenza di −20 dB/decade sopra f1.

**Picco aggiuntivo sotto 100 Hz, solo per le classi cargo** (container, car carrier, bulker, tanker):

```
L_Sf0(f<100, Cargo) = K_LF − 40·log10(f1_LF) + 10·log10(f) − 10·log10( (1 − (f/f1_LF)²)² + D_LF² )
K_LF = 208 dB;  f1_LF = 600 · (1 kn / V_C)  [Hz]
D_LF = 0,8 (container, bulker);  D_LF = 1,0 (car carrier, tanker)
```

Il testo dice che l'eq. 11 "aggiunge un picco" e la indica come valida per f < 100 Hz. Non è chiaro se sotto 100 Hz sostituisca l'eq. 10 o vada sommata in potenza; il foglio Excel di riferimento (File S1) non l'ho letto. Ho provato la lettura "sostituisce": per un container a 18 kn lungo 294 m dà circa 178 dB nella banda di 1/10 di decade a 63 Hz, coerente con la Fig. 5 del paper. A 100 Hz il salto fra le due formule è di circa 1 dB. [media]

**Da densità spettrale a banda di 1/10 di decade (≈ 1/3 d'ottava)**: L_banda = L_Sf + 10·log10(0,231·f_c).
**Incertezza** statistica del modello: deviazione standard circa 6 dB per banda (20 Hz-20 kHz); scarto interquartile 8 dB [alta]. Sotto 20 Hz lo scarto cresce, perché il modello non contiene le righe di pala. Utile come dispersione da nave a nave per il generatore.

**Tabella 1 del paper — classi** [alta]

| Classe | Tipo AIS | V_C (kn) | Navi uniche | n misure | l media (m) | f1 = 480/V_C (Hz) |
|---|---|---|---|---|---|---|
| Fishing (pesca) | 30 | 6,4 | 10 | 21 | 32 | 75 |
| Tug (rimorchiatore) | 31, 32, 52 | 3,7 | 67 | 173 | 28 | 130 |
| Naval (militare) | 35 | 11,1 | 9 | 19 | 79 | 43 |
| Recreational (diporto) | 36, 37 | 10,6 | 7 | 15 | 45 | 45 |
| Government/Research (governo/ricerca) | 51, 53, 55 | 8,0 | 2 | 2 | 58 | 60 |
| Cruise (crociera) | 60-69, l > 100 m | 17,1 | 23 | 54 | 268 | 28 |
| Passenger (passeggeri/traghetto) | 60-69, l ≤ 100 m | 9,7 | 2 | 6 | 52 | 49 |
| Bulker (portarinfuse) | 70, 75-79 con V ≤ 16 kn | 13,9 | 360 | 850 | 211 | 35 |
| Containership (portacontainer) | 71-74; 70, 75-79 con V > 16 kn | 18,0 | 195 | 440 | 294 | 27 |
| Vehicle carrier (car carrier) | n/d | 15,8 | 65 | 141 | 194 | 30 |
| Tanker (cisterna) | 80-89 | 12,4 | 53 | 129 | 186 | 39 |
| Other (altro) | altri tipi | 7,4 | 6 | 12 | 81 | 65 |
| Dredger (draga) | 33 | 9,5 | 7 | 52 | 128 | — |

Per la draga che lavora (V < 3 kn) il modello usa il valore calcolato a 14 kn. Le colonne V_C e la lunghezza media sono [alta]; la colonna f1 è calcolata da me.

Avvertenze per il 1985-1991: le classi Government/Research (2 navi) e Passenger (2 navi) sono statisticamente deboli [bassa]. Le navi ECHO sono del 2000-2020. Per gli anni '80 suggerisco di usare la classe del modello e di aggiungere a parte le righe tonali (sezione 2) e il carattere della propulsione (turbina o diesel, elica a passo fisso o variabile).

**Esempi di andamento in banda a 63 Hz** (Fig. 5, valori letti dal grafico) [media]:
- container (8-24 kn): 160-185 dB;
- bulker (5-20 kn): 155-185 dB;
- tanker (9-16 kn): 165-180 dB;
- rimorchiatore (3-15 kn): 130-175 dB;
- car carrier: 150-180 dB;
- crociera: 155-180 dB.

### 1.2 RANDI 3.1 (riportato in MacGillivray & de Jong 2021, eq. 4-6) [alta]

```
L_Sf(f,V,l) = L_Sf0(f) + 60·log10(V/12 kn) + 20·log10(l/300 ft) + df·dl + 3,0 dB
L_Sf0(f) = −10·log10( 10^(−1,06·log10 f − 14,34) + 10^(3,32·log10 f − 21,425) )   per f < 500 Hz
L_Sf0(f) = 173,2 − 18·log10 f                                                     per f ≥ 500 Hz
dl = l^1,15 / 3643,0   (l in ft)
df = 8,1 dB (f ≤ 28,4 Hz);   df = 22,3 − 9,77·log10 f (28,4 < f ≤ 191,6 Hz);   df = 0 oltre
```

RANDI si basa su dati della Seconda guerra mondiale e una sorgente "media" a 6 m. È adatto proprio al "sapore" di un sonar degli anni '80: era il modello USN dell'epoca. Rispetto ai dati ECHO sovrastima sotto 250 Hz e sottostima sopra.

### 1.3 Wales & Heitmeyer 2002 (WH02) — spettro medio del naviglio mercantile [alta]

Fonte: S.C. Wales, R.M. Heitmeyer, "An ensemble source spectra model for merchant ship-radiated noise", *JASA* 111(3):1211-1231, 2002. Formula e deviazione standard dal riepilogo in MacGillivray & de Jong 2021, eq. 7-8.

Il modello viene da 54 navi mercantili (Mediterraneo e Atlantico orientale) ed è valido fra 30 e 1200 Hz. Non dipende da velocità e lunghezza: gli autori non trovarono correlazione.

```
L_Sf,WH02(f) = 230,0 − 10·log10( f^3,594 ) + 10·log10( (1 + (f/340)²)^0,917 )   [dB re 1 µPa²/Hz @ 1 m]
σ(f) = 5,3 dB (f < 150 Hz);  5,3 − 0,0088·(f − 150) dB (150-400 Hz);  3,1 dB (f ≥ 400 Hz)
```

Uso consigliato: WH02 come "nave mercantile anonima media" e σ(f) come dispersione gaussiana fra navi.

### 1.4 Ross (1976) — scala con velocità e dislocamento [media]

Fonte: D. Ross, *Mechanics of Underwater Noise*, Pergamon 1976. Formula letta in una fonte secondaria: arXiv 2402.02647 (2024), eq. 4, https://arxiv.org/pdf/2402.02647

```
NL_S(f; v, DT) = 112 + 50·log10(v/10 kn) + 15·log10(DT) + 20 − 20·log10(f)    [dB re 1 µPa²/Hz @ 1 m]
v in nodi, DT = dislocamento in tonnellate; valida per f > 100 Hz; navi con elica a passo fisso
```

- Ross stesso sconsiglia la formula per navi oltre circa 30 000 t (riportato da McKenna et al. 2012, p. 99) [alta].
- Con cavitazione pienamente sviluppata il livello scala fra V⁵ e V⁶ (Ross 1976, citato da ITTC 2014; vedi 4). Vicino all'innesco la dipendenza è più ripida.
- Esiste anche la forma con la velocità periferica della pala: L(>100 Hz) ≈ 175 + 60·log10(U_tip/25 m/s) + 10·log10(B/4). La cito a memoria e non l'ho verificata in una fonte aperta: [bassa].

### 1.5 McKenna, Ross, Wiggins, Hildebrand 2012 — misure per tipo di nave [alta]

Fonte: M.F. McKenna et al., "Underwater radiated noise from modern commercial ships", *JASA* 131(1):92-103, 2012.
URL: https://www.cetus.ucsd.edu/docs/publications/McKennaJASA2012.pdf

Misure: 29 navi, canale di Santa Barbara, aprile 2009, idrofono a 570 m di profondità, distanza al CPA circa 3 km. SL calcolato con propagazione sferica, banda 20-1000 Hz (Tabella I).

| Tipo | L (m) | Anno costr. | GT (×10³) | Potenza (×10³ hp) | Velocità (m/s, kn) | SL 20-1000 Hz (dB) |
|---|---|---|---|---|---|---|
| Container (6 navi) | 294-298 | 1993-2005 | 53-55 | 42-67 | 10,6-11,2 (20,6-21,8 kn) | 184,2-188,1 |
| Car carrier (4) | 173-199 | 1984-2006 | 33-61 | 10,7-16,5 | 7,8-9,1 (15-17,7 kn) | 178,1-182,2 |
| Bulk carrier (5) | 167-229 | 1997-2007 | 16-43 | 9,1-13,3 | 7,1-7,4 (13,8-14,4 kn) | 184,2-187,4 |
| Open hatch cargo (4) | 190-213 | 1986-2007 | 20-37 | 9-14,1 | 6,7-7,3 (13-14,2 kn) | 178,8-183,8 |
| Chemical/product tanker (4) | 148-182 | 1985-2004 | 10,8-30 | 6,9-12,9 | 4,6-8,0 (9-15,6 kn) | 176,6-184,9 |
| Crude oil tanker (3) | 229-243 | 2000-2006 | 37-57 | 13-18,4 | 6,5-7,5 (12,6-14,6 kn) | 179,4-182,1 |
| Product tanker (3) | 180-228 | 2005-2007 | 28-42 | 12,6-18,4 | 7,1-8,0 (13,8-15,6 kn) | 178,5-182,7 |

**Forma spettrale per tipo** (Fig. 4, bande d'ottava; valori letti dal grafico) [media]:

| Tipo | Ottava 31,5 Hz | Ottava 125 Hz | Ottava 1 kHz | Carattere |
|---|---|---|---|---|
| Container (10,8 m/s) | ≈179 | ≈177 | ≈174 | Quasi piatto. Massimo in 1/3 d'ottava a 30-60 Hz (≈176); righe di pala sotto 100 Hz |
| Car carrier (8,5 m/s) | ≈172 | ≈173 | ≈168 | Piatto, circa 6 dB sotto i container |
| Bulk carrier (7,2 m/s) | ≈171 | ≈183 | ≈170 | **Picco netto a circa 100 Hz** (1/3 d'ottava ≈178 dB a 80-100 Hz) |
| Open hatch (7,0 m/s) | ≈168 | ≈175 | ≈168 | Picco a circa 63 Hz, più basso |
| Tanker (prodotti/greggio/chimici, 6,5-7,5 m/s) | ≈174 | ≈174 | ≈165-169 | Energia concentrata sotto 100 Hz, cala oltre; i chimichieri sono un po' più forti sopra 500 Hz |

Altri risultati, tutti [alta]:
- **Asimmetria prua/poppa**: a basse frequenze il livello di poppa supera quello di prua di 5-10 dB. Utile per la firma in funzione dell'angolo di vista.
- Spettri in banda di 1 Hz: container circa 165 dB/Hz a 30-40 Hz; bulker circa 165 dB/Hz a circa 100 Hz.
- La relazione SL/velocità non è unica: va trattata per tipo. Container e car carrier aumentano con velocità e dimensione; bulker e tanker no.
- Rumore di fondo del sito: 80 dB/Hz a 40 Hz, 68 dB/Hz a 95 Hz, 63 dB/Hz a 800 Hz.

### 1.6 Veirs, Veirs & Wood 2016 — statistiche per classe, incluse le classi minori [alta]

Fonte: S. Veirs, V. Veirs, J.D. Wood, "Ship noise extends to frequencies used for echolocation by endangered killer whales", *PeerJ* 4:e1657, 2016.
URL: https://pmc.ncbi.nlm.nih.gov/articles/PMC4800784 (anche https://peerj.com/articles/1657)

Misure: Haro Strait 2011-2013, 2809 passaggi, SL a banda larga 11,5 Hz-40 kHz (la Tabella 2 dice 20 Hz-40 kHz). Valori: media ± deviazione standard.

| Classe | N | Velocità (kn) | SL banda larga (dB) |
|---|---|---|---|
| Tutte | 2809 | 14,1 ± 3,9 | 173 ± 7 |
| Container | 529 | 19,2 ± 1,9 | 178 ± 4 |
| Car carrier | 187 | 16,9 ± 1,8 | 176 ± 3 |
| Cargo (general) | 307 | 14,4 ± 2,3 | 175 ± 5 |
| Tanker | 148 | 13,8 ± 1,4 | 174 ± 4 |
| Bulk carrier | 965 | 13,7 ± 1,5 | 173 ± 5 |
| Tug | 337 | 8,2 ± 2,3 | 170 ± 5 |
| Research | 14 | 11,1 ± 1,8 | 167 ± 5 |
| Passenger | 49 | 14,4 ± 4,5 | 166 ± 8 |
| Fishing | 65 | 9,1 ± 2,2 | 164 ± 9 |
| Miscellaneous | 41 | 11,2 ± 5,8 | 163 ± 9 |
| Military | 113 | 11,1 ± 3,1 | 161 ± 10 |
| Pleasure craft | 41 | 12,4 ± 4,9 | 159 ± 9 |

- La mediana degli spettri ha il massimo vicino a **50 Hz** per tutte le classi. Le navi grandi arrivano a 153-159 dB/Hz (container 159 dB/Hz; quantile 95 % = 165), le piccole a 134-141 dB/Hz.
- Pendenza alle alte frequenze: circa −10 dB/decade fra circa 700 Hz e 5 kHz, circa −20 dB/decade sopra 5 kHz. Vicino a 20 kHz tutte le classi hanno livelli simili.
- Dipendenza dalla velocità **debole** in questo dataset: circa +1,8 dB per m/s in media (circa +1 dB/kn), da +0,2 a +3,4 dB per m/s secondo la classe.

### 1.7 Altri riferimenti di livello

- Hildebrand 2009, *MEPS* 395:5-20, Tab. 1 (https://www.cetus.ucsd.edu/docs/publications/HildebrandMEPS2009.pdf) [alta]:
  - nave cargo di 173 m a 16 kn: SL 192 dB, banda a −10 dB fra 40 e 100 Hz (è l'Overseas Harriette);
  - livelli spettrali di picco fra 10 e 50 Hz: da 195 dB/Hz per superpetroliere veloci (> 20 kn) a 140 dB/Hz per piccoli pescherecci (NRC 2003);
  - negli anni '80 la stazza lorda mondiale rimase quasi costante: il calo delle petroliere compensò la crescita di bulker e container.
- ITTC 2014 (27th ITTC Specialist Committee on Hydrodynamic Noise, https://www.ittc.info/media/6091/sc-noise.pdf) [alta]: SL a banda larga delle navi commerciali da circa 165 dB (navi piccole) a 195 dB (grandi), con il massimo fra 10 e 125 Hz.
- Rompighiaccio (DOSITS, da Erbe & Farmer 2000, *JASA* 108:1332, e Roth et al. 2013, *JASA* 133:1971; https://dosits.org/galleries/audio-gallery/anthropogenic-sounds/icebreaker) [media]:
  - *Henry Larsen*: sistema a bolle 194 dB a banda larga (fino a circa 5 kHz); cavitazione dell'elica 203 dB;
  - USCGC *Healy*: 190-200 dB in bande d'ottava a 10, 50 e 100 Hz durante il ramming;
  - rompendo il ghiaccio il livello sale di circa 10 dB fra 20 Hz e 2 kHz; in retromarcia +10-15 dB a 50 e 100 Hz.
- Variabilità entro la stessa classe: 20-40 dB (Simard 2016 e Joy 2019, citati in Erbe et al. 2019, *Front. Mar. Sci.* 6:606, https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2019.00606/full) [media].

---

## 2. Struttura tonale (righe LOFAR) e modulazione (DEMON)

### 2.1 Arveson & Vendittis 2000 — M/V *Overseas Harriette* (portarinfuse)

Fonte: P.T. Arveson, D.J. Vendittis, "Radiated noise characteristics of a modern cargo ship", *JASA* 107(1):118-129, 2000. Il testo integrale è a pagamento: ho letto abstract e riepiloghi (IOGP USRD https://usrd.iogp.org/resource/radiated-noise-characteristics-of-a-modern-cargo-ship/; ITTC 2014; Frontiers 2024 https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2024.1252901/full; MARIN/DOSITS 2024).

- Nave: portarinfuse di 173 m, 25 515 t di dislocamento, costruita nel 1978 da Hitachi Zosen (registro MARAD), **diesel lento 2 tempi a presa diretta**, una sola linea d'asse, elica a passo fisso [alta]. Numero di pale 4 secondo il database di Southampton, citato in un riepilogo di ricerca; il sito risponde 403 e non l'ho verificato direttamente [bassa].
- **Frequenza di pala (BR) fondamentale: 9 Hz a 16 kn, SL 174 dB re 1 µPa @ 1 m** [alta].
  - Con 4 pale l'asse girerebbe a 2,25 Hz (135 giri/min) [STIMA].
  - Con 5 pale: 1,8 Hz (108 giri/min) [STIMA].
- Righe tonali forti da tre sorgenti: diesel-generatore di servizio (SSDG), frequenza di scoppio del motore principale, armoniche di pala dovute alla cavitazione [alta].
- **SSDG**: una didascalia della figura 7 parla di una riga a 24 Hz pari a 4 volte la rotazione dell'SSDG, quindi SSDG a 6 Hz (360 giri/min). Riepiloghi secondari riportano 6 Hz con armoniche, la 24 Hz a 179 dB e la 30 Hz a 168 dB, indipendenti dalla velocità [media].
  - Interpretazione [STIMA]: un generatore 4 tempi a 8 cilindri a 360 giri/min scoppia a 8·360/120 = 24 Hz, cioè la 4ª armonica della rotazione. 360 giri/min con 20 poli dà 60 Hz di rete.
- Con la velocità [alta]: a 8 kn lo spettro è dominato dalle macchine (SSDG); a 16 kn dalla cavitazione dell'elica.
- Esponenti di velocità per banda letti sui dati Arveson e Vendittis (Bosschers, MARIN, seminario DOSITS 2024, https://dosits.org/wp-content/uploads/2024/04/Saturn_Webinar_Ship_URN_v2.1_JB_sharing.pdf) [media]:
  - "gobba" di cavitazione a bassa frequenza: circa 70·log10 V;
  - altre bande fra circa 20·log10 V e 70·log10 V;
  - livello ad alte frequenze circa 50-60·log10 V;
  - sopra la gobba la pendenza spettrale è circa −10·log10 f.
- Irradiazione di tipo dipolo alle basse frequenze (effetto Lloyd mirror) [alta].
- La frequenza di scoppio del motore principale **non** è nelle fonti aperte. Per un motore Hitachi-B&W o Sulzer da 6-7 cilindri a 110-135 giri/min sarebbe 11-16 Hz [STIMA].

### 2.2 Regole generali (fisica) per generare le righe

| Riga | Formula | Note | Fonte |
|---|---|---|---|
| Rotazione asse (SR) | f_SR = N_elica/60 | 1-3 Hz per i mercantili | — |
| Pala (BR) e armoniche | f_m = m·Z·N_elica/60 | Z = numero di pale | ITTC 2014 [alta] |
| Scoppio, 2 tempi | f_F = N_cil·N_motore/60 | Righe anche a multipli interi della rotazione dell'albero motore | [alta, fisica] |
| Scoppio, 4 tempi | f_F = N_cil·N_motore/120 | Righe a multipli **interi e semi-interi** della rotazione dell'albero (ordini 0,5) | ITTC 2014 [alta] |
| Ingranamento del riduttore | f_G = denti·giri/60 | Righe fino a circa 1 kHz e oltre | ITTC 2014 [alta] |
| Rete elettrica | f_rete = poli·giri/120 | 50 o 60 Hz, armonica 2ª (100/120 Hz, magnetostrizione), 3ª | [alta, fisica] |
| Pompe centrifughe | giranti·giri/60 | Righe anche ad alta frequenza | ITTC 2014 [alta] |
| Modulazione DEMON | f_SR e f_BR | Il rumore di cavitazione è modulato in ampiezza alla frequenza di pala | Erbe et al. 2019 [alta] |

- Con un'elica a pale fisse e motore a presa diretta, BR e scoppio sono legati in rapporto fisso. Per esempio 6 cilindri e 4 pale danno f_F/f_BR = 6/4 = 1,5, costante a ogni velocità. **Indizio di classificazione utile in gioco.**
- Diesel lento 2 tempi a presa diretta (ITTC 2014) [alta]:
  - 70-140 giri/min, montato rigido sullo scafo;
  - vibrazioni importanti sotto 100 Hz;
  - i giri, e con loro tutte le righe, variano con la velocità.
- Diesel medio 4 tempi con riduttore (ITTC 2014) [alta]:
  - 300-1000 giri/min, montaggio rigido o elastico;
  - banda bassa 6-150 Hz (masse e scoppi) più una banda larga alta (valvole, ingranaggi, pistoni);
  - livelli di vibrazione **più alti** dei diesel lenti, perché scalano circa con (potenza/peso)².
- **Elica a passo variabile (CPP)** [STIMA, pratica comune]: l'asse gira spesso a velocità costante, quindi SR, BR e scoppio restano **fissi** al variare della velocità e cambia solo l'ampiezza.
  - Eurofleets2 2014, nave *Johan Hjort*: con la CPP fuori dal punto di progetto (passo e giri cambiati a 9,7 kn) compare un forte rumore a banda larga dell'elica intorno a 100 Hz [alta].
- Diesel-elettrico (ITTC 2014) [alta]:
  - generatori a giri costanti, di solito su supporti elastici, con righe **indipendenti dalla velocità**;
  - in più le righe dei motori elettrici e dei convertitori (sincro- o ciclo-convertitore).
- Turbina a vapore con riduttore (ITTC 2014) [alta]:
  - righe di turbina e riduttore legate ai giri dell'asse, fino a circa 1 kHz;
  - nessuna riga di scoppio; la turbina gira molto più veloce (decine di Hz).
- Elica "cantante" (singing): riga stretta fra 180 e 1800 Hz (Frontiers 2024) [alta].

### 2.3 Velocità dei generatori e frequenza di rete

Formula: giri = 120·f/poli [alta, fisica].

| Poli | 60 Hz (giri/min) | 50 Hz (giri/min) |
|---|---|---|
| 4 | 1800 | 1500 |
| 6 | 1200 | 1000 |
| 8 | 900 | 750 |
| 10 | 720 | 600 |
| 12 | 600 | 500 |
| 20 | 360 | 300 |

- Per il generatore 4 tempi la frequenza di scoppio è N_cil·giri/120. Esempio [STIMA]: 6 cilindri a 720 giri/min danno 36 Hz; 8 cilindri a 900 giri/min danno 60 Hz e si confondono con la rete.
- I sistemi di bordo più comuni sono 440 V trifase a 60 Hz e 380 V trifase a 50 Hz (Università di Malta, https://um.edu.mt/library/oar/handle/123456789/102079) [media].
- Chi usa cosa [STIMA, bassa]:
  - USA, navi NATO e la gran parte dei mercantili costruiti in Giappone, Corea e USA: 60 Hz. Molte navi d'altura costruite in Europa negli anni '70-'80 adottavano anch'esse 440 V/60 Hz.
  - Navi sovietiche e del blocco orientale, cabotaggio europeo e traghetti nordici: tipicamente 50 Hz (380/400 V).
  - Le navi in turbina usano turbo-generatori; possono esserci in più generatori asse (shaft generator, diffusi dagli anni '80, con frequenza resa costante da convertitore o da CPP a giri fissi).

---

## 3. Propulsione e caratteristiche tipiche per tipo (flotta 1985-1991)

Legenda colonne: L = lunghezza; Vs = velocità di servizio; Z = numero di pale; N = giri asse/min.

| Tipo | L (m) | Vs (kn) | Z | N (giri/min) | Motore | Cilindri | Assi | Generatori | Classe J-E |
|---|---|---|---|---|---|---|---|---|---|
| Portacontainer | 150-290 | 18-23 (SL-7: 33) | 5-6 (4 nelle piccole) | 80-110 | Diesel lento 2T presa diretta (B&W L-GFCA/MC, Sulzer RLB/RTA); alcune in turbina a vapore | 6-12 | 1 (SL-7: 2) | 3-4 DG 4T 720-1200 giri/min, o turbo-gen. | Containership |
| Portarinfuse | 150-300 | 13-15 | 4-5 | 80-140 | Diesel lento 2T presa diretta | 4-8 | 1 | 2-3 DG | Bulker |
| Cargo generale / multipurpose | 90-180 | 13-17 | 4 | 110-250 (oppure CPP a giri costanti) | Diesel lento 2T, o medio 4T con riduttore e CPP | 5-9 (2T), 6-12 (4T) | 1 | 2-3 DG | Bulker / Other |
| Petroliera VLCC/ULCC | 300-380 | 14-16 | 5 (4-6) | 75-90 | **Turbina a vapore con riduttore** (maggioranza delle unità anni '70); diesel lento nelle nuove dal 1979-80 | — / 7-9 | 1 | Turbo-gen. + DG | Tanker |
| Petroliera prodotti/chimica/costiera | 60-180 | 12-15 | 4 | 100-200 (2T); CPP 150-250 | Diesel lento 2T o medio 4T | 5-8 / 6-12 | 1 | 2-3 DG | Tanker |
| Traghetto / RoRo / passeggeri | 100-200 | 17-23 | 4 (CPP) | 130-250, costanti | 2-4 diesel medi 4T, 400-750 giri/min, riduttore, **CPP** | 6-16 (in linea o V) | **2** (alcuni 3) | 3-4 DG + generatori asse | Passenger/Cruise |
| Rimorchiatore portuale | 25-35 | 10-13 (lavoro 0-6) | 4 (in mantello Kort) | 150-350 | 2 diesel medi/veloci (es. EMD 645 2T a 900 giri/min) con riduttore; oppure Voith-Schneider / Z-drive | 8-16 | 2 | 1-2 DG piccoli | Tug |
| Rimorchiatore d'altura | 50-80 | 14-17 | 4 (CPP, Kort) | 150-250 | 2-4 diesel medi, riduttore, CPP | 8-16 | 2 | 2-3 DG | Tug/Other |
| Rompighiaccio diesel-elettrico | 120-135 | 14-20 | 4 | 120-180 | Diesel-generatori + motori di propulsione | 9-12 (per gruppo) | 3 | Fino a 9 DG | Government |
| Rompighiaccio nucleare | 148 | 18-21 | 4 (passo fisso) | 120-180 | Turbo-elettrico (2 reattori) | — | 3 | 5 turbo-gen. ausiliari | Government |
| Nave oceanografica | 50-100 | 10-14 | 4-5 | 100-250 | Diesel-elettrico (DC) o diesel medio + CPP | 6-12 | 1-2 | 2-4 DG | Government/Research |
| Peschereccio a rampa (occidentale) | 40-70 | 11-14 (pesca 3-5) | 4 (CPP, spesso Kort) | 150-250, costanti | 1 diesel medio 4T, riduttore, CPP | 6-12 | 1 | 1-2 DG + generatore asse | Fishing |
| BMRT sovietico (*Pulkovskiy Meridian*, Proekt 1288) | 104 | 16 (pesca 3-5) | 4 (CPP in Kort, Ø 3,7 m) | 146 | 2 diesel medi 4T, 520 giri/min, 2×3500 CV, riduttore | 6 + 6 | 1 | DG + generatore asse | Fishing |
| BMRT sovietico più vecchio (Proekt 394/*Mayakovskiy*, *Atlantik*) | 84-85 | 12-13 | 4 | 200-300 | Diesel medio con riduttore, 2300-3200 kW | 6-8 | 1 | DG | Fishing |
| Cianciolo (purse seiner) | 30-70 | 12-15 | 4 (CPP) | 200-350 | Diesel medio/veloce, CPP, eliche di manovra (bow thruster) | 6-12 | 1 | DG | Fishing |
| Palangaro / reti da posta | 15-45 | 9-12 | 3-4 | 300-600 | Diesel veloce con riduttore | 6-12 | 1 | 1 DG | Fishing |

Fonti e affidabilità per riga:

- **Portacontainer**
  - Econship della US Lines (Daewoo, 1984): Hyundai/Sulzer **7RLB90**, 28 000 bhp a 102 giri/min; in servizio 18 kn all'85 % MCR, cioè 25 200 bhp a **98 giri/min** (Maritime Reporter 12/1984, https://magazines.marinelink.com/Magazines/MaritimeReporter/198412/page/12) [media]. Righe derivate [STIMA]: asse 1,63 Hz, scoppio 7·98/60 = 11,4 Hz, BR 8,2 Hz (5 pale) o 9,8 Hz (6 pale).
  - SL-7 di Sea-Land (1972-73, nel periodo di gioco USNS classe *Algol*): 2 turbine GE da 120 000 hp in tutto, 2 assi, oltre 33 kn, 288 m (https://en.wikipedia.org/wiki/Algol-class_vehicle_cargo_ship) [media].
  - Sulzer RTA (anni '80): l'RTA84 scende fino a 65 giri/min minimi (55 per la versione RTA84M); l'RTA76 ha da 4 a 9 cilindri; nel 1988 il più venduto era l'RTA58 (Maritime Reporter 10/1984 e 7/1988; TRID) [media].
  - Pale [bassa]: 4-6 nei mercantili, 5-6 nelle grandi portacontainer (blog vibrationdata e simili, https://blog.vibrationdata.com/?p=150472).
- **Portarinfuse e tanker presa diretta**: 70-140 giri/min (ITTC 2014) [alta]. Pale e cilindri sono STIMA [media].
- **VLCC**
  - Per circa 25 anni dopo la guerra la turbina a vapore con riduttore fu "quasi l'unica scelta" per le grandi petroliere; dopo le crisi petrolifere molte furono convertite a diesel (Maritime Reporter 5/1994, https://magazines.marinelink.com/Magazines/MaritimeReporter/199405/page/28; TRID 147514, 160027, 161801) [media].
  - Esempio *Mobil Hawk* (1976): turbina GE da 36 515 shp, elica a 90 giri/min; dopo la conversione 2 Pielstick PC4V da 12 cilindri con riduttore, elica a 86 giri/min [media].
  - Negli anni 1985-91 la flotta VLCC è ancora in buona parte a turbina [STIMA, media].
- **Traghetti**
  - *Herald of Free Enterprise* (1980): 3 Sulzer 12ZV40/48 da 6000 kW, 22 kn, 132 m, eliche a passo variabile, 3 assi (https://en.wikipedia.org/wiki/MS_Herald_of_Free_Enterprise) [media].
  - *Silja Serenade* (1990): 4 Wärtsilä-Vasa 9R46, 32 500 kW in tutto, 23 kn, 203 m [media].
  - Righe derivate [STIMA]: un 12ZV40/48 (4T, circa 560-600 giri/min) scoppia a circa 56-60 Hz; un 9R46 a circa 500 giri/min scoppia a 9·500/120 = 37,5 Hz.
- **Rimorchiatori statunitensi**
  - 2 EMD 12-645 o 16-645 con riduttore Falk (rapporti 4,08-4,71:1), eliche a passo fisso a 4 pale spesso in mantello Kort, 3000-4200 hp (tugboatinformation.com, es. https://tugboatinformation.com/tug.cfm?id=10537) [media].
  - Righe [STIMA]: l'EMD 645 marino gira a 900 giri/min. Albero 15 Hz; scoppio 2T 12·900/60 = 180 Hz (16 cilindri: 240 Hz); elica 900/4,48 ≈ 200 giri/min, cioè 3,35 Hz; BR 13,4 Hz.
  - In Europa erano diffusi Voith-Schneider e Schottel (Z-drive) [STIMA].
- **Rompighiaccio**
  - Classe *Yermak* (Wärtsilä 1974-76): *Krasin* con **9 Wärtsilä-Sulzer 12ZH40/48 da 3385 kW**, 3 motori DC Strömberg da 8820 kW, 3 eliche a passo fisso, 19,8 kn in servizio, 135 m (https://en.wikipedia.org/wiki/Krasin_(1976_icebreaker)) [media].
  - *Kapitan Khlebnikov* (1981, classe *Kapitan Sorokin*): 6 diesel-generatori per 18 MW in tutto, 3 motori DC, 3 assi, eliche a 4 pale di 4,3 m, 122,5 m [media].
  - Classe *Arktika*: 2 reattori OK-900A, propulsione turbo-elettrica (corrente continua), **3 assi, eliche a passo fisso a 4 pale, 120-180 giri/min**, 3×18 MW, 148 m, circa 19-21 kn; 5 turbine ausiliarie per circa 10 MW elettrici (https://en.wikipedia.org/wiki/Arktika-class_icebreaker) [media].
  - Rete a 50 Hz probabile sulle unità sovietiche [STIMA]. Classi diesel: *Moskva* (1959-69), *Kapitan Belousov* (anni '50); classe nucleare *Taymyr* (1989-90) (https://en.wikipedia.org/wiki/Icebreakers_of_Russia) [media].
- **Navi oceanografiche**
  - *Johan Hjort* (1990): diesel, riduttore, CPP, sotto 1 kHz domina il motore (Eurofleets2 D11.1, 2014, https://www.eurofleets.eu/download/EUROFLEETS2_WP11_D11.1_031214_V1.3.pdf) [alta].
  - ICES CRR 209 (Mitson 1995) raccomanda diesel-elettrico con motori DC ed elica a passo fisso grande e lenta a 5 pale; limite fissato a **11 kn** (dalla stessa fonte) [alta].
  - Formula del limite ICES: 135 − 1,66·log10 f (dB re 1 µPa/Hz @ 1 m, 1 Hz-1 kHz), sopra 1 kHz circa 130 − 22·log10(f/1 kHz). Il primo tratto è [media] (jcaa.caa-aca.ca); il secondo è [bassa] perché non verificato.
  - R/V *Revelle*: innesco della cavitazione appena sopra 10,6 kn (UNOLS ARRV white paper 2008, https://www.unols.org/sites/default/files/ARRV_URNLimitsWhitePaper-July09.pdf) [alta].
  - Ecoscandagli di bordo (righe attive ad alta frequenza, utili come "transiente" o identificativo):
    - SeaBeam (dal 1977): 12 kHz, 16 fasci (NOAA, https://www.fisheries.noaa.gov/inport/help/components/instruments/1325) [media];
    - registratori di profondità (PDR) a 12 kHz e sub-bottom profiler a 3,5 kHz [STIMA, media];
    - ecoscandagli da pesca Simrad EK400 a 38 e 120 kHz [STIMA, media];
    - multibeam moderno EM122: 11,5-12,5 kHz, 245 dB (Hildebrand 2009) [alta, ma fuori periodo].
- **BMRT *Pulkovskiy Meridian*** (Proekt 1288): costruito dal 1974 (circa 113 unità), 2×3500 CV a 520 giri/min, CPP a 4 pale Ø 3,7 m a 146,3 giri/min in mantello Kort, 16 kn, 104,5 m (annunci korabel.ru, es. https://www.korabel.ru/board/detail/42116.html; shipstamps.co.uk) [media].
  - Righe [STIMA]: asse 2,44 Hz, BR 9,75 Hz (fissa, perché CPP), scoppio 6·520/120 = 26 Hz per motore (2 motori leggermente sfasati possono dare battimenti), albero motore 8,67 Hz.
  - BMRT più piccoli: 83,6-84,7 m, 2323-3170 kW (FAO, https://www.fao.org/4/y4884e/y4884e06.htm) [media].
- **Pescherecci occidentali, ciancioli, palangari**: valori di propulsione STIMA [bassa].

---

## 4. Innesco della cavitazione

- Meccanismo: la depressione cresce con V². Sotto la velocità di innesco (CIS, cavitation inception speed) non c'è cavitazione; sopra, la lunghezza della cavità cresce circa con V² (Bosschers, MARIN 2024) [alta].
- CIS ottenibili: da 8-10 kn fino a 17-19 kn per navi progettate per stare senza cavitazione ad alta velocità. I fattori principali sono la forma della carena, la posizione dell'elica e il carico dell'elica (Kongsberg, https://www.kongsberg.com/maritime/services/hydrodynamics--collaboration--references/propeller-design--cavitation/cavitation-inception-speed--cis) [media].
- Esempi misurati:
  - nave oceanografica *Revelle*: CIS circa 10,6-11 kn;
  - ARRV (rompighiaccio da ricerca): CIS di progetto oltre 8 kn (UNOLS 2008) [alta];
  - un caso di dati di vibrazione a bordo: CIS circa 8 kn (risultato di ricerca, fonte non letta per intero) [bassa];
  - piccoli gommoni (RIB): 5-6 kn (UCL) [media].
- *Overseas Harriette*: macchine dominanti a 8 kn, cavitazione dominante a 16 kn [alta]. Quindi la CIS cade fra 8 e 16 kn.
- **Regola per il gioco** [STIMA, media]: i mercantili degli anni '80 (eliche ottimizzate per il rendimento, non per il silenzio) cavitano alla velocità di servizio. Valori suggeriti di CIS:

| Tipo | CIS suggerita |
|---|---|
| Portarinfuse e tanker carichi | 7-10 kn |
| Portacontainer | 9-12 kn |
| Traghetti con CPP | 8-12 kn (dipende molto dal passo: fuori progetto cavitazione sul dorso o sulla faccia anche a bassa velocità) |
| Pescherecci al traino | 3-5 kn (elica molto caricata, cavitano spesso già in pesca) |
| Rimorchiatori a tiro pieno (bollard) | cavitano anche da fermi |
| Navi da ricerca "silenziose" | 10-11 kn (solo dopo ICES 209, quindi rare prima del 1995) |

- Sopra la CIS il livello sale circa con 60·log10 V (RANDI, JOMOPANS) oppure fra V⁵ e V⁶ (Ross). Appena sopra l'innesco la salita è più ripida, fino a circa 70·log10 V nella gobba a bassa frequenza [media]. Sotto la CIS: righe di macchina più rumore di flusso, con dipendenza dalla velocità debole.
- Rompighiaccio: forte cavitazione nel ramming e in retromarcia, 190-203 dB (sezione 1.7).

---

## 5. Ricetta proposta per il generatore (sintesi)

1. **Continuo**: JOMOPANS-ECHO con la classe, la velocità e la lunghezza della nave. Per l'anno 1985-91 aggiungere un offset "d'epoca":
   - +0…+3 dB sotto 100 Hz per le navi a diesel lento [STIMA];
   - −3 dB sopra 1 kHz per le navi a turbina [STIMA];
   - scarto casuale per nave gaussiano con σ = 5-6 dB, fisso per tutta la vita della nave.
2. **Cavitazione**: se V > CIS, applicare la scala di velocità della sezione 4. Se V < CIS, portare il continuo sopra 100 Hz a −10/−15 dB e lasciare dominanti le righe di macchina [STIMA].
3. **Righe**:
   - SR, BR e armoniche fino alla 5ª-6ª;
   - scoppio del motore principale e armoniche (2T o 4T, regole della sezione 2.2);
   - 1-3 diesel-generatori a giri fissi (sezione 2.3), con rete a 50 o 60 Hz secondo la bandiera;
   - riduttore per i diesel medi e le turbine.
   - Livelli: BR 170-178 dB a 16 kn per i grandi mercantili (OH: 174); righe SSDG fino a 179 dB (OH: 24 Hz) [media]. Gli altri livelli di riga sono STIMA da tarare a orecchio rispetto al continuo (+10…+20 dB sopra la densità spettrale locale).
4. **DEMON**: modulazione d'ampiezza del rumore di cavitazione a f_BR, con componenti a f_SR. Profondità di modulazione 20-60 % [STIMA]. Le CPP a giri costanti danno una linea DEMON fissa; le presa diretta danno una linea proporzionale alla velocità.
5. **Aspetto**: poppa +5…+10 dB rispetto alla prua alle basse frequenze (McKenna 2012) [alta]. Dipolo/Lloyd mirror con sorgente a circa 6 m (oppure al 50 % del pescaggio).

---

## 6. Lacune non colmate

- **Arveson & Vendittis 2000, testo integrale**: mancano il numero di cilindri e il modello del motore principale, la frequenza di scoppio misurata, i giri dell'asse, il numero di pale (4 solo da una fonte non verificata), la tabella completa delle righe con i livelli e le armoniche di pala.
- **Ross 1976**: la formula con la velocità periferica della pala non è verificata su fonte aperta. Il report DTIC ADA126292 (confronto di Ross con la flotta reale, livelli medi 167 e 161 dB a 50 e 100 Hz, circa 6 dB sotto Ross; noto solo dal riassunto di ricerca) non era accessibile (403).
- **Gray & Greeley 1980** (modello statistico della BR per la flotta mercantile): solo l'abstract, senza numeri.
- **ICES CRR 209**: manca la formula esatta del tratto sopra 1 kHz.
- **Wales & Heitmeyer 2002**: letti solo formula e σ (dal riepilogo JOMOPANS), non le distribuzioni per nave.
- **JOMOPANS-ECHO**: non è chiaro se l'eq. 11 sostituisca l'eq. 10 o vi si sommi (serve il foglio Excel S1). Mancano anche i dettagli dell'Appendice A (correzione della profondità).
- **Statistiche di flotta 1985-91**: mancano la quota di VLCC a turbina, la distribuzione del numero di pale per tipo e la percentuale di navi a 50 o 60 Hz per bandiera o cantiere. Sono tutte STIMA.
- **Pescherecci occidentali, ciancioli, palangari, rimorchiatori europei, navi oceanografiche degli anni '80** (*Knorr*, *Akademik Kurchatov*): nessuna misura acustica né scheda di propulsione verificata. In JOMOPANS la classe Research ha solo 2 navi.
- **Classe *Yermak***: i giri dei diesel-generatori 12ZH40/48 non sono verificati (probabilmente 500-560 giri/min). Rete delle unità sovietiche non verificata.
- **Livelli delle righe** (scoppio, generatori, riduttori) per tipo di nave: le fonti aperte non danno tabelle. Solo l'*Overseas Harriette* ha valori (BR 174 dB, SSDG 179 e 168 dB).
- **Rumore dei verricelli e della rete a strascico** (pesca attiva) e del bow thruster: nessun dato numerico raccolto.
