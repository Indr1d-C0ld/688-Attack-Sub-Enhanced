# R7 — Transitori e armi: numeri per la sintesi e per il modello acustico

Ricerca su fonti aperte, 25 ottobre 2026, per il pezzo audio **R7 «Transitori e armi»** (dossier 18 §8) del remake di
*688 Attack Sub* (1989). Nessun altro file del progetto è stato modificato.

Non ripeto: le formule di Cole per l'onda d'urto e di Willis per la bolla, né gli esplosivi del 1989 (dossier 07 §1); le
lacune del dossier 18 (§3 e §7); i tonfi di boe e siluri aviolanciati (dossier 17 §8.3). Le firme a banda larga attuali dei
siluri restano quelle di `tools/pipeline/acustica.py` (140-160 dB, STIMA): qui le confronto e le completo.

## 0. Convenzioni

- **Livello di sorgente (SL):** dB re 1 µPa a 1 m. **Densità spettrale d'energia (ESD):** dB re 1 µPa²·s/Hz a 1 m.
  **Picco:** valore istantaneo massimo.
- **Affidabilità:** **alta / media / bassa**, accanto a ogni dato.
- **[V]** = letto da me nella fonte (testo, PDF o diapositive). **[S]** = preso da un riassunto del motore di ricerca o da
  una fonte secondaria che riporta il dato. **[D]** = calcolo o deduzione mia da dati verificati.
  **STIMA** = numero di progetto proposto da me, da usare come parametro e non come misura.
- **n.v.** = non verificato o non trovato in fonti aperte.
- Formule standard usate nei calcoli, da manuale e citate a memoria (affidabilità alta per la forma, da ricontrollare
  su copia cartacea):
  - sorgente omnidirezionale: `SL = 170,8 + 10·log10(P_acustica [W])`; per un trasduttore direttivo si aggiunge l'indice DI;
  - assorbimento di Thorp (f in kHz, α in dB/km): `α = 0,11 f²/(1+f²) + 44 f²/(4100+f²) + 2,75·10⁻⁴ f² + 0,003`.
    Dà circa 0,07 dB/km a 1 kHz, 1 dB/km a 10 kHz e 8 dB/km a 30 kHz;
  - risonanza di una bolla d'aria (Minnaert): `f₀ ≈ 3,26·√(1 + d/10) / a` Hz, con a = raggio in m e d = profondità in m;
  - portata passiva grezza: `SL − TL ≥ NL_banda − DI + DT`, con TL = 20 log R + α R (sferica; in acqua bassa o nel
    canale la portata cresce).
- I calcoli sono in `/tmp/claude-1000/s/calc.py`, cartella volatile (vedi la memoria «tmp volatile»): le formule sono
  ripetute nel testo, così il file può andare perso senza danni.

---

## 1. Lancio dal tubo

### 1.1 Come lancia il 688 [media]

| Dato | Valore | Fonte | Aff. |
|---|---|---|---|
| Sistema di espulsione del 688 | **RPES, *Ram Pump Ejection System*** («water ram»); Ohio e Seawolf usano invece l'*Air Turbine Pump* | [Lit99] [S] | media |
| Funzionamento | l'aria alta pressione delle bombole, tramite la valvola di sparo, muove un pistone nel serbatoio d'impulso. Il pistone spinge acqua di mare nel tubo dietro l'arma, attraverso una valvola a cassetto. Accelerazione «oltre 2 g» | testimonianza di un ex sommergibilista su forum, riassunta da ricerca [S]; brevetti generici [USP6220196] | bassa (dettagli 688), media (principio) |
| Rumore del sistema | brevetti USA: il gruppo ram-pump «è una sorgente di rumore significativa»; la conversione rapida dell'energia dell'aria in energia cinetica dell'arma «produce rumore irradiato significativo» | [USP6220196] e brevetti collegati [S] | media |
| Swim-out | **il 688 non ha lo swim-out**: il Mk 48 viene espulso. Il programma STEP (dal 2003) studia lo swim-out per evitare il «più rumoroso» lancio ad aria compressa | [WP-Mk48] (cita National Academies 2000) [V] | media |
| Dove nasce il rumore (prototipo cinese a «cilindro aria-acqua», analogo al water ram) | sorgenti principali: vibrazioni del basamento del cilindro, della parete del cilindro d'acqua e della parete del tubo; forza pulsante assiale del pistone; **urto del pistone sul fondello**. Il getto d'acqua all'uscita **non** è la sorgente principale | [Yang25] [V] | media |
| Getto all'uscita | velocità d'uscita «sicura» 9 m/s → 111 dB; standard 12,5 m/s → 119,4 dB (modello in vasca). Riportato a tubo in scala reale: **≈ 140 dB**, «pari al rumore stazionario di un battello quasi silenzioso, molto minore del rumore di lancio» | [Yang25] [V] (riferimento dei dB non scritto nelle righe lette: lo leggo come re 1 µPa, distanza normalizzata) | media |
| Rumore nel locale all'avvio del pistone | scalino di **+20 dB** quando il pistone parte; poi rumore bianco a banda larga, più forte in alta frequenza | [Yang25] [V] | media |
| Picco d'urto del pistone | vibrazione verticale istantanea del basamento «un ordine di grandezza» sopra la vibrazione periodica di un diesel navale | [Yang25] [V] | media |

### 1.2 Sequenza, livelli e durate (fuori bordo)

Nessuna fonte aperta dà un livello in dB re 1 µPa per un lancio reale. Propongo valori ancorati a due fatti: il getto vale
≈ 140 dB ed è minore del rumore di lancio [Yang25]; l'energia cinetica di un Mk 48 (1660 kg) a 12,5 m/s vale ≈ 130 kJ
in circa 1 s [D].

| Fase | Che cosa si sente | SL proposto | Banda | Durata | Aff. |
|---|---|---|---|---|---|
| Allagamento del tubo (dal serbatoio WRT, interno) | fruscio d'acqua, gorgoglio, colpi di valvola | n.v.; **STIMA 120-135 dB** | 100 Hz-3 kHz | 10-40 s | bassa |
| Equalizzazione della pressione | sibilo, poi un colpo sordo a fine equalizzazione | n.v.; **STIMA 120-130 dB** | 500 Hz-5 kHz (sibilo) | 5-15 s | bassa |
| Apertura della porta esterna (portello e otturatore idraulici) | ronzio idraulico, scorrimento, **«clunk» metallico** di fine corsa | n.v.; **STIMA 135-150 dB** di picco sul clunk | clunk 50 Hz-4 kHz con risonanze del portello 200-800 Hz | 3-8 s (clunk 50-150 ms) | bassa |
| **Sparo** (water ram) | **colpo sordo** (urto del pistone e della colonna d'acqua), poi un fruscio di getto | **STIMA 165-180 dB** di picco. L'energia acustica è 10⁻⁵-10⁻⁴ dell'energia cinetica: 170-182 dB [D] | 20 Hz-5 kHz; massimo 100-500 Hz | colpo 50-100 ms; coda 0,5-1,5 s | bassa |
| Fine corsa del pistone e ritorno (ricarica) | secondo colpo meccanico, poi aria | **STIMA 150-160 dB** | come sopra | il ritorno è pilotato dall'aria come lo sparo [Yang25]; 2-10 s dopo | bassa |
| Avvio del motore del siluro | vedi §2.5 | | | | |

**Portata di scoperta (calcolo, [D] con SL STIMA).** Uso NL_banda = 85-95 dB (banda di 1 kHz con mare 2-4), DI = 15
e DT = 5, con propagazione sferica:

| SL | 150 dB | 160 dB | 170 dB | 180 dB |
|---|---|---|---|---|
| portata | 2-5 km | 5-16 km | 16-42 km | 42-94 km |

Lo sparo di un 688 si sente quindi a **10-40 km** da un sonar passivo attento: è il classico **«launch transient»** che fa
scattare l'allarme siluro. La preparazione del tubo si sente solo a pochi km.

**A bordo.** Nel locale siluri: colpo e sbuffo molto forti, con le orecchie che si stappano se l'aria d'impulso viene
scaricata nel locale. Quest'ultimo dettaglio sul 688 è n.v.: sui battelli della WWII la valvola di scarico mandava l'aria
dentro il locale per non lasciare una bolla. Nel resto del battello si sente un tonfo e un tremito dello scafo. Per i suoni
esterni che arrivano a bordo vale la funzione K(f) del dossier 18_note/bordo.md (≈ 91 dB a 50-250 Hz).

### 1.3 Missili da tubo e da VLS

| Arma | Sequenza | Numeri | Fonte | Aff. |
|---|---|---|---|---|
| **UGM-84 Harpoon** (capsula, dal 1981) | sparo dal tubo come un siluro; la capsula galleggiante «plana» fino alla superficie; all'affioramento si staccano i tappi anteriore e posteriore e parte il missile | booster: 53 kN per **2,9 s** | [Par-84] [V] | media |
| **UGM-109 Tomahawk** da tubo | il missile resta nel contenitore finché non esce dal tubo; poi il contenitore viene espulso e **il booster si accende sott'acqua** per portarlo in superficie | durata del booster n.v. (STIMA circa 10-13 s in tutto, di cui 1-3 s in acqua) | [Par-109] [V]; [NG20] [V] | media (sequenza) |
| Tomahawk da **VLS** (688 dal Providence, 12 tubi nella cassa zavorra prodiera, fuori dallo scafo resistente) | espulsione a gas dal pozzo, poi accensione del booster | dettagli n.v. | [FAS-109] [V]; DTIC sul tubo verticale [S] | media/bassa |

**Livelli (STIMA).** Uso la stessa formula con potenza meccanica del getto = spinta × velocità del getto / 2 ≈ 30-53 MW
ed efficienza acustica 10⁻⁴-10⁻³: un motore a razzo acceso sott'acqua farebbe **SL ≈ 205-218 dB** [D]. Il valore è molto
incerto. Per il gioco propongo **195-210 dB per 1-3 s**: un ruggito a banda larga con bolle, 20 Hz-10 kHz, che finisce con
lo «sfondamento» della superficie. È di gran lunga il transitorio più forte di un lancio. Si sente come un'esplosione
lunga a decine di km. L'Harpoon invece accende il booster **fuori dall'acqua**: in acqua resta solo la capsula, con lo
sparo (come un siluro), la risalita silenziosa per qualche decina di secondi (n.v.) e lo sgancio dei tappi in superficie.
Lo sgancio è un transitorio pirotecnico, STIMA 170-185 dB, molto breve. Il ruggito del booster arriva in acqua solo
attraverso la finestra di Snell (dossier 17).

**VLS (STIMA).** Lo sparo a gas è più morbido del water ram, ma ogni pozzo ha il suo portello, che si apre con un clunk
STIMA 140-150 dB. Segue l'accensione subacquea come sopra. In salva si sente una cadenza regolare di 3-5 s fra un pozzo e
l'altro, coerente con il dossier 05 §4.

---

## 2. Siluri in corsa

### 2.1 Che cosa si sa dei propulsori [media]

| Siluro | Motore | Propulsore | Velocità | Fonte | Aff. |
|---|---|---|---|---|---|
| **Mk 48 / ADCAP** | motore a pistoni a piatto oscillante (*swashplate*), combustione esterna, Otto fuel II; potenza ≈ 500 hp | pompa a getto assiale; nelle prime versioni descritta «con eliche gemelle controrotanti»; nell'ADCAP Mod 6AT circa **15 pale di rotore e 12 di statore** (grafica US Navy 2011) | 55 kn (Mod 0, non ufficiale); 63-65 kn (ADCAP, stima) | [NW-US] [V]; [WP-Mk48] [V]; [NC] (hp) [S] | media (bassa per gli hp) |
| Rumorosità del Mk 48 | «la principale critica tecnica è che è **molto rumoroso**»; la Mod 6 ha ridotto le firme irradiate; l'isolamento del motore è migliorato negli anni '90 | — | — | [NW-US] [V]; [WP-Mk48] [V] | media |
| **Mk 46** | alternativo a combustione esterna **a due velocità** (ricerca lenta, attacco veloce), Otto fuel II | eliche controrotanti (didascalia della Mod 5A) | > 40 kn (45 kn); autonomia 6-8 min | [NW-US] [V]; [WP-Mk46] [V]; [VP4] [V] | media |
| **53-65K** (1969) | turbina a cherosene-ossigeno (il 53-65M a cherosene-perossido) | n.v. (la famiglia 53 usa eliche controrotanti) | 45 kn, 19 km | [NW-RU] [V]; [WP-T53] [V] | media |
| **SET-65** | elettrico, argento-zinco | n.v. | 40 kn, 16 km | [NW-RU] [V] | media |
| **USET-80** | elettrico, argento-zinco | n.v. | 40-50 kn, ~20 km | [NW-RU] [V] | media |
| **TEST-71** | elettrico, argento-zinco; filoguidato | n.v. | 40 kn / 15 km; 35 kn / 25 km | [NW-RU] [V] | media |
| **AT-1** | elettrico, argento-zinco; solo da elicottero | n.v. | 27 kn, 5 km | [NW-RU] [V] | media |
| Righe delle eliche controrotanti (veicolo ad alta velocità, Cina) | lo spettro a righe è fatto **soprattutto dalla frequenza di pala e dalle sue armoniche** | — | — | [Ma02] (solo l'abstract) [V] | media |
| Rumore e velocità | il rumore di flusso cresce con V⁵-V⁷ (≈ V⁶); il rumore di macchina pesa a bassa velocità, quello idrodinamico domina ad alta | — | — | [rhhz24] [S]; [USP6622647] [S] | media |

**Mancano:** giri d'asse, numero di cilindri e di pale (salvo il dato dell'ADCAP), righe del motore e del riduttore,
livelli misurati. Sono tutti n.v. Urick 1983 ha un capitolo sul rumore irradiato e proprio dei siluri, che non ho letto.

### 2.2 Righe proposte (STIMA, da fisica elementare)

**Giri d'asse.** Per un'elica di siluro il rapporto d'avanzo J = V/(n·D) sta fra 1 e 1,5. Con D ≈ 0,40 m (533 mm di
calibro) e V = 28 m/s (55 kn) si ha **n ≈ 45-70 giri/s** [D]. Per il gioco uso n ∝ V, cioè `n = 1,1·V[kn]` giri/s
(STIMA: 50 giri/s a 45 kn, 60 a 55 kn).

| Sigla | Righe di pala | Motore e riduttore | Carattere | Aff. |
|---|---|---|---|---|
| mk48, mk48_adcap | **15·n** (rotore) e armoniche; interazione rotore-statore a 15·n con bande laterali a ±12·n (STIMA). A 55 kn: **≈ 900 Hz, 1,8 kHz, 2,7 kHz** | piatto oscillante a presa diretta: riga di accensione a k·n con k = cilindri (n.v.; STIMA k = 6 → 330 Hz a 55 kn); scarico dei gas di Otto con bolle = sibilo a banda larga 2-10 kHz | ronzio acuto con battimenti e sibilo; il Mk 48 base è più rumoroso dell'ADCAP di 2-5 dB (STIMA) | bassa |
| mk46 | eliche controrotanti: N₁·n e N₂·n, con N₁, N₂ n.v. (STIMA 4 e 6). Più la somma (N₁ + N₂)·n, debole | due velocità: **salto netto** di righe e livello al passaggio ricerca→attacco | più acuto del Mk 48 (asse più veloce, STIMA n = 1,4·V) | bassa |
| type53_65k | eliche controrotanti (STIMA 4 + 4 pale, n ≈ 1,0·V) | turbina veloce con riduttore: **fischio** di passaggio delle palette e di ingranamento a 3-8 kHz (STIMA). Quasi senza scia di bolle: i prodotti del cherosene-ossigeno condensano (principio dei siluri a ossigeno, [D]) | «whine» metallico molto forte; è il più rumoroso | bassa |
| uset80, set65, test71m, at1 | eliche controrotanti, come sopra | motore elettrico: riga di passaggio dei poli e del collettore a p·n (STIMA p = 8-12); niente gas di scarico | ronzio più pulito, 5-8 dB sotto un termico alla stessa velocità (STIMA) | bassa |

### 2.3 Banda larga (confronto con `acustica.py`)

Le tabelle attuali (mk48 140/150/160 dB a 28/40/55 kn, cioè ≈ 72 log V fra 40 e 55 kn) sono coerenti con la legge
V⁵-V⁷ (50-70 log V) [rhhz24] e restano una STIMA. Propongo di aggiungere:

- **forma spettrale** (STIMA): piatta fino a circa 1-2 kHz, poi −6 dB per ottava fino a 20 kHz. Sotto 100 Hz va ridotta di
  −6 dB per ottava, perché un siluro ha poca massa e poche macchine lente;
- **cavitazione con la profondità** (STIMA): a meno di 30-50 m e sopra i 40 kn, +6-10 dB nella banda 2-20 kHz. È lo
  «sfrigolio» che si sente subito dopo un lancio a bassa quota;
- **sibilo di flusso**: compreso nella banda larga, con la legge V⁶. Copre la testa attiva del siluro stesso: per questo
  il siluro **rallenta prima di pingare** [NC] [S], media.

### 2.4 Testa attiva: frequenze, livelli, cadenza

| Dato | Valore | Fonte | Aff. |
|---|---|---|---|
| Banda dei siluri attivi (ipotesi di progetto USN, 1960) | **20-80 kHz**, banda passante del ricevitore ≈ 5 kHz | [Wel60] brevetto US4216534 [V] | media (ipotesi dell'inventore, non scheda d'arma) |
| Siluri passivi | accordati «di solito» a **24,5 kHz**, larghi 5 kHz; il Mk 24 FIDO della WWII ascoltava a 24 kHz | [Wel60] [V]; [WP-Mk24] [S] | media |
| Esempi di temporizzazione (1960) | ping di **17 ms**, ascolto di circa **1 s**, poi il ciclo riparte; tempo minimo di permanenza del bersaglio 3 ms | [Wel60] [V] | media |
| Esempi di temporizzazione (1964) | ping di **2 ms** ogni **1,25 s** in ricerca; se l'eco torna entro circa **0,5 s** (≈ 380 m, «1250 ft») **la cadenza raddoppia** e l'ascolto si dimezza; oltre 1400 ft si oscura | [Hei64] brevetto US4306512 [V] | media («say», valori d'esempio) |
| Mine e siluri a onda continua (1946) | **≈ 60 kHz**; Doppler ≈ 42 Hz per nodo | [Ste46] brevetto US3021807 [V] | media |
| Doppler | le teste con soglia Doppler ignorano bersagli fermi | [Wel60] [V] | alta (principio) |
| Mk 48 | sonar attivo a schiera bidimensionale a fascio elettronico; CBASS (Mod 7) con impulsi a larga banda, agilità e scelta della frequenza; **acquisizione ≈ 4000 yd (3,6 km)** (Mod 0, non ufficiale; il Mk 37 ≈ 1000 yd) | [NW-US] [V]; [WP-Mk48] [V] | media |
| Mk 46 | attivo o passivo/attivo; ricerca a serpentina o a cerchio; acquisizione **1600 yd** | [VP4] [V]; [WP-Mk46] [V] | media |
| SET-65 / TEST-71 / AT-1 | portata d'aggancio 800 m / 800 m / 500-1000 m; USET-80: canale attivo-passivo più **canale di scia** (scattering acustico dalle bollicine della scia) | [NW-RU] [V]; [prof] [S] | media |
| Frequenze di Mk 48, Mk 46 e sovietici | **n.v.** (Mk 50 «25 kHz» solo da forum, bassa) | — | — |
| Livello di sorgente delle teste | **n.v.** | — | — |

**Proposte (STIMA):**

- **Frequenze:** Mk 48 ≈ 25-35 kHz (pesante, portata lunga); ADCAP a larga banda 20-40 kHz; Mk 46 ≈ 30-40 kHz;
  sovietici 15-30 kHz; canale di scia del 53-65K e dell'USET-80 50-150 kHz, verso l'alto. Il WLR-9 e il sonar
  d'intercettazione li ricevono. In cuffia si rendono come un **«ping» trasposto a 1-3 kHz**, perché sopra 20 kHz
  l'orecchio non sente: era il ruolo del ricevitore d'intercettazione con eterodina.
- **Livello:** con SL = 170,8 + 10 log P + DI, P = 100-1000 W e DI = 15-20 dB si ottiene **SL ≈ 205-220 dB** [D].
  Per il gioco: 210 dB per i pesanti, 205 dB per i leggeri.
- **Cadenza:** il ciclo dipende dalla finestra di distanza, `PRI ≈ 1,2·2R/c`.
  - **Ricerca:** finestra pari alla portata d'acquisizione. Mk 48 (3,6 km): PRI ≈ 5,5 s; Mk 46 (1,5 km): ≈ 2,4 s;
    sovietici leggeri (0,8 km): ≈ 1,3 s, in linea con l'1-1,25 s dei brevetti.
  - **Attacco:** la finestra segue il bersaglio. A 500 m ≈ 0,8 s, a 200 m ≈ 0,3 s, a 100 m ≈ 0,16 s: è l'accelerazione
    dei ping in chiusura. Sotto 380 m conviene passare al doppio impulso, come in [Hei64].
  - **Impulsi:** 2-20 ms, onda continua in ricerca e FM in attacco (la scelta CW/FM dello Stingray è [S]).

### 2.5 Avvio del motore (*start-up*)

| Dato | Valore | Aff. |
|---|---|---|
| Termici (Otto fuel II) | accensione della camera di combustione dopo l'uscita dal tubo; i giri salgono in 1-3 s (STIMA). Si sente uno **«pop»** d'accensione, poi un fischio crescente fino al regime | n.v.; STIMA |
| Turbina del 53-65K | accensione del combustore, poi la turbina sale in fischio in circa 1-2 s (STIMA) | n.v.; STIMA |
| Elettrici | nessuno scoppio: rampa del motore e delle eliche in circa 1 s (STIMA) | n.v.; STIMA |
| Livello | pop d'accensione STIMA 150-160 dB per 20-50 ms; poi la banda larga di §2.3 che sale col quadrato dei giri fino al valore di regime | bassa |

---

## 3. Esplosioni

La fisica di base è nel dossier 07 §1. Qui aggiungo lo **spettro** e **ciò che arriva lontano**.

### 3.1 Valori di sorgente calcolati (TNT equivalente, coefficienti di Cole e Willis del dossier 07) [D, alta per le formule]

L'onda d'urto esponenziale `p = P_m·e^(−t/θ)` ha ESD piatta `2·(P_m θ)²` sotto la frequenza d'angolo
`f_c = 1/(2πθ)` e scende di −6 dB per ottava sopra. La bolla aggiunge energia a f_b = 1/T₁ e alle sue armoniche. È la
«smerlatura» (*scalloping*) dello spettro vista dal CTBTO [Ver19]. P_m e θ «a 1 m» sono valori nominali di sorgente:
le formule valgono a distanze maggiori.

| Carica (TNT eq.) | P_m a 1 m | θ a 1 m | f_c | ESD sotto f_c | T₁ e f_b a 30 / 100 / 300 m | Picco a 1 / 10 / 50 km |
|---|---|---|---|---|---|---|
| 20 kg (bomba di profondità leggera) | 284 dB | 0,18 ms | 880 Hz | 212 dB | 0,27 s · 3,8 Hz / 0,11 s · 8,8 Hz / 0,048 s · 21 Hz | 216 / 194 / 178 dB |
| 100 kg (Mk 54 ≈ 108 kg; bomba di profondità media) | 289 dB | 0,27 ms | 580 Hz | 221 dB | 0,45 s · 2,2 Hz / 0,20 s · 5,1 Hz / 0,082 s · 12 Hz | 222 / 199 / 183 dB |
| 300 kg (testata sovietica da 533 mm) | 293 dB | 0,36 ms | 440 Hz | 227 dB | 0,65 s · 1,5 Hz / 0,28 s · 3,6 Hz / 0,12 s · 8,4 Hz | 225 / 203 / 187 dB |
| 544 kg (Mk 48, bolla) | 295 dB | 0,42 ms | 380 Hz | 231 dB | 0,80 s · 1,3 Hz / 0,34 s · 2,9 Hz / 0,15 s · 6,9 Hz | 227 / 205 / 189 dB |

Il picco «a 50 km» usa Cole fuori dal campo di validità; in acqua reale conta la propagazione. È un ordine di grandezza.

### 3.2 Struttura nel tempo (per la sintesi)

| Elemento | Valore | Fonte | Aff. |
|---|---|---|---|
| Urto | salita quasi istantanea, ritorno al fondo «in pochi ms», con coda più lenta dell'esponenziale | [DDC23] [V] | alta |
| Fase negativa | sotto la pressione idrostatica mentre la bolla si espande | [DDC23] [V] | alta |
| Impulsi di bolla | salita e discesa «graduali (pochi ms)» contro meno di un ms dell'urto; non simmetrici | [DDC23] [V] | alta |
| Secondo impulso di bolla | picco circa **1/5** del primo | [DOSITS-E] [V] | alta |
| Esempio SUS | 0,82 kg a 18,3 m: urto → primo impulso di bolla ≈ 0,12 s (≈ 8 Hz); energia «larga ma dominata dalle basse frequenze» | [DOSITS-E] [V] | alta |
| Periodi successivi | T₂ ≈ 0,74·T₁ e T₃ ≈ 0,63·T₁ (da K₂ = 1,57 e K₃ = 1,33 di [Brain16] nel dossier 07, a Z costante) | [D] | media |
| Bolla che rompe la superficie | se l'esplosione è a pochi raggi di bolla dalla superficie, la serie di impulsi s'interrompe di colpo | [DDC23] [V] | alta |
| Picchi misurati (carica non dichiarata, acqua di 20 m) | 247 dB a 28 m; 238 dB a 68,5 m; la riflessione di superficie taglia l'impulso con cavitazione a circa −100 kPa | [DDC23] [V] | alta |

### 3.3 Che cosa si sente lontano

| Caso | Distanza | Che cosa arriva | Fonte | Aff. |
|---|---|---|---|---|
| Cariche da 1,8 kg (4 lb), 1944 | fino a **1667 km** (900 nmi); a 4000 ft di profondità si prevedevano 10 000 miglia | impulso nel canale SOFAR | [DOSITS-H] [V]; [EW45] [S] | alta |
| ARA San Juan e granata di taratura (108 kg a 33 m) | 6000-7760 km | arrivi brevi di **10-30 s**, banda **1-80 Hz**, forte dispersione; ritardo cepstrale della bolla della granata 0,454 s (teoria 0,454 s) | [Ver19] [V] | alta |
| Kursk-2 (siluri e testate, 12-8-2000) | registrato fino a **5000 km** (sismico) | spettro con modulazione di bolla fra **1 e 6 Hz**; massimi a 9,5 e 15,8 Hz dalla risonanza dello strato d'acqua (115 m) | [Sch02] [V]; [SciD01] [S] | alta |
| A decine di km (STIMA) | 10-50 km | picco 180-205 dB per cariche di 20-544 kg (tabella 3.1), cioè 80-100 dB sopra il fondo. Sopra i 5-10 kHz il suono è tagliato dall'assorbimento (Thorp: 1 dB/km a 10 kHz). L'impulso si allarga a 0,1-2 s per i percorsi di superficie e di fondo. **Si sente come un tuono sordo**, con 1-4 «rimbalzi» di bolla separati da T₁. A bordo resta intorno a 110 dB re 20 µPa con K ≈ 91 dB (18_note/bordo.md): si sente chiaramente | [D] | media |

### 3.4 Kursk: il caso di testate vere

| Dato | Valore | Fonte | Aff. |
|---|---|---|---|
| Due eventi | 07:28:26,6 e 07:30:42,4 UTC; distanza media **135,76 s**; magnitudo locale **1,5** e **3,5** | [Sch02] Tab. 6.6.1 [V] | alta |
| Energia | Kursk-1 ≈ 250 kg TNT («un siluro moderno»); Kursk-2 ≈ 5 t (LANL), stime da 1 a 7 t | [RSC] [S]; [SciD01] [S]; [SP00] [S] | media |
| Natura di Kursk-2 | primi movimenti **negativi** a ARCES e FINES: sorgente complessa, forse un'implosione iniziale. Il volume equivalente collassato è ≈ 4200 m³, il 20-26% del battello | [Sch02] [V] | alta (dato), media (interpretazione) |
| Spostamento fra i due eventi | ≈ 145 m in 135,8 s (1,1-1,3 m/s) | [Sch02] [V] | alta |

---

## 4. Scafi che cedono, implosioni e affondamento

### 4.1 Sottomarini persi: dati acustici

| Battello | Quota di collasso | Energia (TNT eq.) | Durata e bolla | Rilevamento | Fonte | Aff. |
|---|---|---|---|---|---|---|
| **USS Thresher** (10-4-1963) | ≈ **2400 ft (732 m)**; prevista 1950 ft | ≈ **22 000 lb (≈ 10 t)** | collasso in **47 ms**; bolla ≈ 5,4 Hz [D, Willis] | SOSUS: Array Fox (≈ 30 nmi, stazione di Shelburne) più Argentia e Antigua per il punto (ellisse 4 × 8 nmi). Rilevati lo scram del reattore alle 09:11 e una **soffiata delle casse di 30 s** fra le 09:13,5 e le 09:14 | [Mil18] [V]; [Bry18] [V]; [Rule15] [V] | media (analisi di Rule, dati ancora classificati) |
| **USS Scorpion** (22-5-1968) | **1530 ft (466 m)**; prevista 1050 ft | **13 200 lb (≈ 6 t)** | fase di compressione < **0,112 s**; bolla **4,46 Hz** (Willis con 6 t a 466 m dà 4,45 Hz [D]: l'energia di Rule viene dalla bolla) | tre siti sui due lati dell'Atlantico (AFTAC); distanze n.v. | [Rule-Sc] [V]; [Ring10] [V]; [Rule15] [V] | media |
| — precursori | **due esplosioni interne a 0,5 s di distanza** (18:20:44Z), ≤ 20-22 lb TNT ciascuna; collasso 21 min 50 s dopo (18:42:34Z) | | | | [Rule-Sc] [V] | media |
| — dopo il collasso | **17 eventi in ≈ 200 s**: strutture più resistenti che cedono più in basso, fra cui i tubi lanciasiluri a 3370-4570 ft (1030-1390 m). Relitto a 11 100 ft | | | | [Ring10] [V] | media |
| **ARA San Juan** (15-11-2017) | 1275 ft (388 m) nel 2018; **468 m** nella revisione del 2019 | 12 500 lb (5,67 t) nel 2018; **5216 kg** nel 2019 | **40 ms** (2018), 30 ms (2019); bolla **4,68 Hz** (Willis con 5216 kg a 468 m dà 4,67 Hz [D]) | CTBTO HA10 (Ascensione, 6000 km) e HA04 (Crozet, 7760 km); località a 3,5 km dal relitto | [BAT18] [V]; [Infod19] [V]; [Ver19] [V]; [Nie18] [V] | media (Rule), alta (CTBTO) |
| — sequenza 2019 | esplosione d'idrogeno nel locale batterie a ≈ 1 h 40 min dall'immersione, senza falla; poi discesa incontrollata da 40 m a **1,1 m/s** (≈ 6,5 min fino al collasso [D]) | | | | [Infod19] [V] | media |
| — CTBTO | segnale **impulsivo**, ampiezza **maggiore** della granata da 108 kg; ritardo del primo picco cepstrale **314-354 ms**, che cambia con la stazione: forse **due implosioni** vicinissime nello spazio e nel tempo | | | | [Ver19] [V] | alta |

**Discrepanza [D].** Con 5,2-5,7 t a 388-468 m, Willis dà T₁ = 0,21-0,26 s. Il ritardo cepstrale del CTBTO (0,31-0,35 s)
richiederebbe 10-15 t a 388 m. L'interpretazione «a due eventi» del CTBTO rende il dato non direttamente confrontabile.
Per il gioco tengo la formula di Rule, che riproduce le bolle dichiarate.

**Regola per il gioco (STIMA calibrata su Rule).** L'energia d'implosione vale `E = p·V_aria` (p = pressione alla quota,
V = volume non allagato). Scorpion: 4,7 MPa × ~3000 m³ ≈ 14 GJ ≈ 3,4 t TNT; Rule dà 6 t. San Juan: 3,9 MPa × ~2000 m³
≈ 1,9 t; Rule dà 5,7 t [D]. Quindi si prende **W_eq ≈ (2-3)·p·V / 4,184 MJ/kg** per la bolla, poi Willis per f_b.
L'urto è più debole che in un'esplosione della stessa energia: STIMA −10/−15 dB sul picco (vedi §4.2).

### 4.2 Firma di un'implosione

| Dato | Valore | Fonte | Aff. |
|---|---|---|---|
| Profilo di pressione | prima **un calo** (la struttura che cede verso l'interno), poi **un picco positivo** alla fine del collasso; le strutture fragili cedono più energia di quelle duttili | [URI-imp] [S]; [DTIC71] (serbatoi d'acciaio, OCR scadente) [S] | media |
| Sfera di vetro da 10" a 600 ft (183 m), Dabob Bay 1999 | picco **9·10¹¹ µPa a 1 m (239 dB)**, durata **1 ms**, energia **50-50 000 Hz**, livello di sorgente in bassa frequenza ≈ **200 dB** | [PH00] [S] | media |
| Efficienza | sfere implose a 4000 m convertono in suono oltre il 10% dell'energia disponibile (TNT ≈ 1% alla stessa quota) | brevetto US4300654 [S] | media/bassa |
| Durata della distruzione interna | collasso ≈ 0,15 s; nessun collasso di sottomarino ha mostrato una bolla più lunga di 0,3 s | [Rule15] [V] | media |
| Titan (18-6-2023, ≈ 3300 m) | colpo registrato da un registratore NOAA a **≈ 900 miglia (1450 km)**: fruscio costante interrotto da un «boom» | [NBC25] [V] | media |

**Sintesi (STIMA):** calo di pressione di 5-30 ms, poi uno schiocco secco di 1-5 ms (banda fino a 10-20 kHz vicino,
schiacciata sotto 1 kHz lontano), poi 2-4 pulsazioni di bolla a f_b ≈ 4-7 Hz che si smorzano (−14 dB a ciclo). Seguono
crepitii e altri collassi minori: «17 eventi in 200 s» per lo Scorpion.

### 4.3 Allagamento e rumori di rottura

| Dato | Valore | Fonte | Aff. |
|---|---|---|---|
| Allagamento grave | rumore a **banda larga** e **risonanze a banda stretta**; un LOFARgram d'allagamento di Rule mostrava **oltre 100 risonanze forti**, rilevabili a **più di 700 nmi** | [Bry18] [V] | media |
| Thresher | l'Array Fox a 30 nmi sentì le pompe e la soffiata, «non un allagamento» | [Bry18] [V] | media |
| WWII | «breaking-up noises» sentiti dai sommergibili dopo gli attacchi (USS Sea Dog: «forti rumori di rottura» dopo tre colpi; USS Seal 1942) | [SeaDog] (OCR) [S]; [Seal] [S] | bassa (testo), alta (fenomeno) |
| Testimonianze raccolte | cricchiolii, scoppi di caldaie udibili a orecchio nudo | dossier 18_note/bordo.md §0 | media |
| **MOL Comfort** (2013, portacontainer spezzata) | dopo l'affondamento di ciascuna metà, una **raffica di 5-10 min** di segnali brevi, impulsivi, a banda larga; analisi su 6-80 Hz; HA08 a 2352 e 2971 km | [Metz25] [V] | alta |
| **SS El Faro** (2015, nave a vapore, fondale ≈ 4600 m) | un **impulso a banda larga di 10 s** a H10 (Ascensione, ≈ 7500 km [D]); origine stimata 11:44:41 UTC, **5 min dopo** la fine del VDR, entro 25 km dal relitto | [Metz21] [V] | alta |
| Velocità di discesa | San Juan 10-13 kn secondo Rule 2018; Scorpion: da 466 m a ~1390 m in ≤ 200 s, quindi ≥ 4,7 m/s [D] | [BAT18] (dato non nella pagina letta: preso da un riassunto) [S]; [Ring10] [D] | media/bassa |
| Bolle d'aria che sfuggono | risonanza di Minnaert: raggio 1 cm → 330 Hz in superficie e 1,1 kHz a 100 m; raggio 10 cm → 33 Hz in superficie e 108 Hz a 100 m | [D] | alta (formula) |

**Interpretazione mia [D] di El Faro:** l'evento a 5 min (≈ 15 m/s di discesa per arrivare al fondo, oppure un collasso
durante la discesa) è un buon modello per «compartimento che implode» o «urto sul fondo».

**Urto sul fondo:** n.v. Una nave di 2000 t a 5-15 m/s porta 25-225 MJ; con efficienza acustica 10⁻⁵-10⁻⁴ in circa 1 s
viene **SL ≈ 185-200 dB**, con banda sotto 200 Hz e una coda di frane di sedimento (STIMA, bassa).

---

## 5. Transitori del proprio battello

Non ho trovato livelli aperti. Esiste un metodo coreano di misura per bande dei transitori di nave (periscopio, timone,
porta del tubo) [Choi10], ma senza numeri nell'abstract. Gli ancoraggi sono pochi:

- una soffiata delle casse di 30 s fu sentita dal SOSUS a ≈ 30 nmi (Thresher) [Bry18];
- per un ex comandante un urto metallico «si sente per miglia» e un sonarista lo classifica subito come sottomarino [McD23];
- i dossier 09 e 01 propongono 120-150 dB e +10-30 dB sul fondo del battello per pochi secondi (STIMA).

| Transitorio | SL proposto (STIMA) | Componenti | Durata | Portata indicativa ([D] con la tabella di §1.2) | Aff. |
|---|---|---|---|---|---|
| Portello sbattuto o attrezzo caduto | 135-150 dB | urto 50 Hz-5 kHz, risonanze dello scafo 200-1500 Hz con smorzamento di 50-300 ms | 0,1-0,5 s | 2-15 km | bassa |
| Piani di profondità o timone (colpo idraulico) | 120-135 dB | «thunk» più ronzio della pompa idraulica (riga 50-400 Hz) | 0,5-3 s | < 2-5 km | bassa |
| Pompa d'esaurimento o di zavorra (avvio, arresto, cambio) | +6/+12 dB sulla riga (40 log r, dossier 18 §3); transitorio d'avvio 130-140 dB | riga di rotazione e armoniche, più cavitazione all'avvio | avvio 1-3 s, poi stazionario | come le righe | media (legge), bassa (valori) |
| TDU, espulsione dei rifiuti | 140-155 dB | colpo d'acqua (come un piccolo water ram), bolle, borsa zavorrata che cade | 0,5-2 s | 5-20 km | bassa |
| Porta esterna del tubo | 135-150 dB | vedi §1.2 | 3-8 s | 3-15 km | bassa |
| Sparo | 165-180 dB | vedi §1.2 | 0,5-1,5 s | 10-40 km | bassa |
| Soffiata d'emergenza delle casse | **165-180 dB** | ruggito d'aria e bolle 20 Hz-5 kHz | 10-60 s (30 s per il Thresher) | 30-100 km; > 55 km dimostrato col SOSUS | media (rilevamento), bassa (SL) |
| Cambio di regime (ordine di macchina) | +5/+15 dB in rampa | salto delle righe legate ai giri; scrosci di cavitazione se l'elica accelera di colpo | 10-60 s | come la banda larga | media |
| Scricchiolii dello scafo al cambio di quota | 120-135 dB | schiocchi metallici 1-5 kHz, a raffiche | 0,05 s ciascuno, raffiche di 5-30 s | < 5 km | bassa |

---

## 6. Proposta per la sintesi e per il catalogo

### 6.1 Schema di una voce

`id · SL [dB re 1 µPa @1 m] · banda (f_bassa-f_alta, pendenze) · impulso (salita, decadimento) · tonali (f, livello
relativo) · durata · ritmo (singolo / Poisson λ / sequenza) · varianza (± dB, ± %) · fonte e affidabilità`

### 6.2 Voci di transitori

| id | SL | Banda e pendenze | Impulso | Tonali e risonanze | Durata | Ritmo | Var. |
|---|---|---|---|---|---|---|---|
| `tubo_allaga` | 128 | 100 Hz-3 kHz, picco 400 Hz, −6 dB/ott. | salita 0,5 s | gorgoglio, bolle di Minnaert 200-800 Hz | 25 s | singolo | ±4 dB, ±30% |
| `tubo_equalizza` | 125 | 500 Hz-5 kHz (sibilo) | rampa | — | 10 s, colpo finale 80 ms | singolo | ±3 dB |
| `porta_tubo` | 145 (clunk), 125 (moto) | 50 Hz-4 kHz | clunk 2 ms di salita, 150 ms di decadimento | ronzio idraulico 120-180 Hz; risonanze del portello 300-700 Hz | 5 s | apertura e, più tardi, chiusura | ±4 dB |
| `sparo_ram` | 172 | 20 Hz-5 kHz, picco 100-500 Hz | colpo 5 ms di salita, 80 ms di decadimento; coda di getto 1 s a −15 dB | risonanza del tubo ≈ c/(4L) ≈ 50-60 Hz (L ≈ 7 m) [D] | 1,2 s | singolo, più `ram_ritorno` a +3/+8 s | ±4 dB |
| `ram_ritorno` | 155 | come sopra | colpo più morbido | — | 0,5 s | dopo lo sparo | ±4 dB |
| `avvio_siluro` | 155 | 1-10 kHz | pop di 30 ms, poi rampa in 2 s fino alla firma di corsa | righe del siluro che salgono da 0 a n | 2 s | dopo lo sparo +0,5/+2 s | — |
| `capsula_harpoon` | 172 sparo; 178 tappi | 20 Hz-10 kHz | tappi: colpo pirotecnico di 1 ms | — | sparo 1 s; risalita 20-60 s in silenzio (n.v.); tappi 50 ms | sequenza | — |
| `booster_sub` (Tomahawk) | 200 | 20 Hz-10 kHz, piatto | salita 50 ms | bolle; battimento a 5-15 Hz di volume pulsante | 2 s in acqua, poi taglio | singolo, o salva a 3-5 s | ±5 dB |
| `vls_portello` | 145 | 50 Hz-4 kHz | clunk | — | 3 s | per pozzo | ±4 dB |
| `portello_sbatte` | 142 | 50 Hz-5 kHz | 1 ms, decadimento 200 ms | risonanze dello scafo 200-1500 Hz | 0,3 s | Poisson, λ legato a disciplina e allarme | ±6 dB |
| `attrezzo_cade` | 138 | 200 Hz-8 kHz | 0,5 ms | «tintinnio» 1-4 kHz | 0,2 s | Poisson raro | ±6 dB |
| `piani_colpo` | 128 | 50 Hz-2 kHz | 10 ms | pompa idraulica 150-400 Hz | 1,5 s | legato alle manovre | ±4 dB |
| `pompa_avvio` | 135 | riga più banda larga | rampa di 1-3 s | riga di rotazione e armoniche (giri da tabella) | 2 s, poi stazionario | legato all'assetto | ±3 dB |
| `tdu` | 148 | 30 Hz-4 kHz | colpo d'acqua di 50 ms | bolle | 1,5 s | a orari (cambi di guardia) | ±5 dB |
| `soffiata_emergenza` | 175 | 20 Hz-5 kHz, picco 100-300 Hz | rampa di 1 s | bolle grosse 20-100 Hz | 30 s | singolo | ±4 dB |
| `scafo_scricchiola` | 128 | 1-5 kHz | 0,5 ms | — | 0,05 s ciascuno | raffiche di 5-30 s al cambio di quota (> 30 m) | ±6 dB |
| `esplosione` | da Cole (§3.1) | urto: piatto fino a f_c, poi −6 dB/ott.; bolla: righe a f_b, 2f_b… | urto θ; bolle con salita e discesa di pochi ms | f_b da Willis | urto più 2-4 bolle con T₂ ≈ 0,74 T₁ e T₃ ≈ 0,63 T₁ | singolo | — |
| `implosione_scafo` | W_eq = (2-3)·p·V/4,184 MJ, poi Cole −12 dB sul picco | come `esplosione` | calo di 5-30 ms, poi schiocco di 1-5 ms | f_b 4-7 Hz | 0,15 s di collasso più 3 bolle | seguita da 10-20 `cedimento_minore` in 200 s | ±3 dB |
| `cedimento_minore` | 175-195 | 10 Hz-2 kHz | schiocco | f_b più alta (volumi piccoli, quota maggiore) | 0,1-0,5 s | Poisson in 200 s, quota crescente | ±6 dB |
| `rottura_nave` | 150-170 | 50 Hz-3 kHz | strappi e colpi | risonanze di lamiera 100-800 Hz | 0,2-2 s ciascuno | raffica di 5-10 min (MOL Comfort) | ±6 dB |
| `caldaia_esplode` | come 10-50 kg TNT a 5-20 m | come `esplosione` (bolla vicina alla superficie) | | | 1-2 s | 0-2 per nave a vapore | — |
| `allagamento` | 140-160 | banda larga 50 Hz-5 kHz | rampa | **molte risonanze strette** (> 100 linee nel LOFAR, [Bry18]) | da minuti a ore | continuo, ±6 dB a scatti | — |
| `bolle_aria` | 130-150 | bolle di Minnaert, raggio 0,5-10 cm (f da 33 Hz a 650 Hz in superficie, ×√(1+d/10) in quota) | colpi da 5 ms | — | 10-120 s | Poisson denso | — |
| `urto_fondo` | 190 | 5-200 Hz | salita 50 ms | — | 1-2 s più frana di 5-20 s | singolo | ±6 dB |

### 6.3 Righe per le firme dei siluri (da unire a `acustica.py`)

Con `n = k_n·V[kn]` giri/s (STIMA) e la banda larga di `acustica.py` invariata:

| Sigla | k_n | Righe (frequenza, livello rispetto alla banda larga a 1 kHz in 1 Hz) | Sibilo e cavitazione | Avvio | Testa attiva (STIMA) | Ping: ricerca / attacco |
|---|---|---|---|---|---|---|
| mk48 | 1,1 | 15n +12 dB; 30n +6; 45n +3; bande laterali ±12n −6; accensione 6n +6 (n.v.) | sibilo dei gas 2-10 kHz +3 dB; cavitazione a meno di 50 m sopra 40 kn: +8 dB | pop più rampa di 2 s | 30 kHz, 210 dB | PRI 5,5 s (3,6 km) → 0,16 s a 100 m; impulso 10 ms CW / 20 ms FM |
| mk48_adcap | 1,1 | come il mk48 −3 dB; accensione −6 (isolamento migliore) | come sopra −2 dB | idem | 20-40 kHz FM larga banda, 210 dB | come il mk48 |
| mk46 | 1,4 | N₁ = 4, N₂ = 6 (STIMA): 4n +10, 6n +10, 10n +4 | +3 dB 2-10 kHz | due regimi: ricerca a 28 kn, attacco a 45 kn con un salto in 1 s | 35 kHz, 205 dB | PRI 2,4 s → 0,16 s |
| type53_65k | 1,0 | 4n +10, 8n +6; fischio di turbina e riduttore 4-6 kHz +12 dB | nessuna scia di gas (ossigeno, [D]) | scoppio del combustore più fischio in 1,5 s | canale di scia 100 kHz verso l'alto (non sentito dal sottomarino come ping diretto) | ping di scia ogni 0,3-0,5 s (STIMA) |
| uset80 | 1,1 | 4n +8, 4n×2 +4; motore p·n con p = 10: +6 dB | nessuno | rampa elettrica di 1 s | 25 kHz, 210 dB, più canale di scia | PRI 1,3-2,4 s → 0,2 s |
| set65 / test71m | 1,1 | come l'uset80 | nessuno | idem | 25 kHz, 205 dB | PRI 1,3 s → 0,2 s |
| at1 | 1,3 | come l'uset80 −3 dB | nessuno | dopo il tonfo in acqua (dossier 17 §8.3) | 30 kHz, 200 dB | PRI 1,3 s → 0,2 s |

Per il giocatore, nel rendere un ping si conserva la cadenza reale e si traspone la frequenza in banda udibile
(1-3 kHz). L'intercettazione WLR-9 riceve la frequenza vera.

---

## 7. Fonti

Le URL elencate sono quelle che ho aperto (dove [V]) o quelle date dal motore di ricerca (dove [S]).

- [BAT18] Buenos Aires Times, «US Navy intelligence report: ARA San Juan imploded in 40 milliseconds», 12-01-2018 (riporta Le Monde e ONI) — https://batimes.com.ar/news/argentina/us-navy-intelligence-report-ara-san-juan-imploded-in-40-milliseconds.phtml — media.
- [Bry18] J. B. Bryant, «Declassify the Thresher Data», *USNI Proceedings*, luglio 2018; ripubblicato dalla Naval Historical Foundation il 22-08-2019 — https://navyhistory.org/?p=21348 — media (cita il libro di Rule del 2017).
- [Choi10] J.-Y. Choi, J.-S. Oh, P.-H. Lee, «A Calculation Method of Source Level of Underwater Transient Noise by Frequency Band», *J. KIMST* 13(4), 2010 — https://www.koreascience.or.kr/journal/GSGGBW/v13n4.page — media (solo l'abstract).
- [DDC23] D. R. Dall'Osto, P. H. Dahl, N. R. Chapman, «The Sound from Underwater Explosions», *Acoustics Today* 19(1), 2023 — https://acousticstoday.org/wp-content/uploads/2023/02/The-Sound-from-Underwater-Explosions-David-R.-DallOsto-Peter-H.-Dahl-and-N.-Ross-Chapman.pdf — alta.
- [DOSITS-E] DOSITS (URI/GSO), «Explosive Sound Sources» — https://dosits.org/galleries/technology-gallery/basic-technology/explosive-sound-sources/ — alta.
- [DOSITS-H] DOSITS, «History of the SOFAR Channel» — https://dosits.org/science/movement/sofar-channel/history-of-the-sofar-channel/ — alta.
- [DTIC71] rapporto DTIC sui serbatoi d'acciaio imploduti in mare profondo (ADA000807, OCR scadente, non aperto: 403) — https://apps.dtic.mil/sti/pdfs/ADA000807.pdf — media/bassa.
- [EW45] M. Ewing, J. L. Worzel, *Long Range Sound Transmission*, rapporto provvisorio n. 1, 1944-45, WHOI — https://darchive.mblwhoilibrary.org/entities/publication/976cacef-a9ef-5db4-b3b4-e15232e4a267 — alta (scheda).
- [FAS-109] FAS, «BGM-109 Tomahawk» — https://man.fas.org/dod-101/sys/smart/bgm-109.htm — media.
- [Hei64] M. O. Heinrich, US 4 306 512 «Homing torpedo system», depositato il 13-08-1964, concesso nel 1981, US Navy — https://patents.google.com/patent/US4306512 — media.
- [Infod19] G. Porfilio, «The ARA San Juan crew died in 30 milliseconds due to a hydrogen explosion», Infodefensa, 09-10-2019 (rapporto finale di B. Rule, 4-8-2019) — https://www.infodefensa.com/latam/2019/10/09/noticia-milliseconds-hydrogen-explosion.html — media.
- [Lit99] J. Little (NUWC), articolo sul sistema di espulsione elastomerico («water balloon»), ripreso da FAS e GlobalSecurity, ~1999 — https://man.fas.org/dod-101/sys/ship/docs/water_balloon.htm — media (letto dal riassunto).
- [Ma02] 马徐琨 (Ma Xukun), «浅析水下高速航行体对转螺旋桨辐射噪声线谱建模», *声学学报* 2002(6) — https://www.jac.ac.cn/article/doi/10.15949/j.cnki.0371-0025.2002.06.005 — media (solo l'abstract).
- [McD23] NewsNation, intervista al cap. Brad McDonald (USN, a riposo), giugno 2023 — https://digital-release.newsnationnow.com/?p=2319213 — bassa.
- [Metz21] D. Metz, P. L. Nielsen, M. Zampolli, G. Haralabus, «Remote detection of hydroacoustic signals potentially associated with the sinking of SS El Faro», CTBT SnT 2021, poster P1.3-331 — https://conferences.ctbto.org/event/7/contributions/879/ — alta.
- [Metz25] D. Metz, T. C. A. Oliveira, «Acoustic detection of a maritime accident: the case of MOL Comfort», CTBT SnT 2025, poster P1.3-170 — https://conferences.ctbto.org/event/30/contributions/5604/ — alta.
- [Mil18] Military.com, «55 Years After Thresher Disaster, Navy Still Keeps Secrets on Sub Loss», 9-4-2018 (da J. Bryant) — https://www.military.com/daily-news/2018/04/09/55-years-after-thresher-disaster-navy-still-keeps-secrets-sub-loss.html — media.
- [NBC25] NBC News, «Recording captured chilling sound of Titan submersible's final moment», 12-02-2025 — https://www.nbcnews.com/news/us-news/recording-captured-chilling-sound-titan-submersibles-final-moment-rcna191914 — media.
- [NC] Nuclear Companion, «Mark 48 Torpedo» — https://nuclearcompanion.com/data/mark-48-torpedo/ — bassa.
- [NG20] Naval Gazing, «Tomahawk Part 1», 20-5-2020 — https://www.navalgazing.net/Tomahawk-Part-1 — media/bassa.
- [Nie18] P. Nielsen et al. (CTBTO), «Analysis of hydro-acoustic and seismic signals… ARA San Juan», EGU2018-18559 — https://meetingorganizer.copernicus.org/EGU2018/EGU2018-18559.pdf — alta.
- [NW-RU] NavWeaps, «Russia / USSR Torpedoes since World War II» — https://navweaps.com/Weapons/WTRussian_post-WWII.php — media.
- [NW-US] NavWeaps, «USA Torpedoes since World War II» — http://www.navweaps.com/Weapons/WTUS_PostWWII.php — media.
- [Par-84] A. Parsch, «AGM/RGM/UGM-84 Harpoon», Directory of U.S. Military Rockets and Missiles — https://designation-systems.net/dusrm/m-84.html — media.
- [Par-109] A. Parsch, «AGM/BGM/RGM/UGM-109 Tomahawk» — https://designation-systems.net/dusrm/m-109.html — media.
- [PH00] J. J. Pulli (BBN), P. Harben (LLNL), «Hydroacoustic Calibration With Imploding Glass Spheres», 2000 (DTIC ADA516258; copia LDEO SRR 2000) — https://apps.dtic.mil/sti/html/tr/ADA516258/index.html — media (abstract letto tramite riassunto; DTIC risponde 403).
- [prof] Profil, articolo sull'USET-80 e il canale di scia, 2026 — https://profile.ru/news/protection/army/rossijskim-torpedam-uvelichili-dalnost-i-tochnost-obnaruzheniya-881915/amp/ — bassa.
- [rhhz24] 舰船科学技术 46(5), 2024, rumore di flusso del modello SUBOFF ∝ V⁶ — https://html.rhhz.net/jckxjsgw/html/73759.htm — media.
- [Ring10] J. Ring, riassunto (aprile 2010) della rianalisi di B. Rule 2008-09 — https://www.usna63.org/tradition/history/Scorpion.htm — media.
- [RSC] Royal Society of Chemistry, materiale didattico sulla sismologia del Kursk — https://edu.rsc.org/download?ac=507928 — media/bassa.
- [Rule-Sc] B. Rule, lettera sulla perdita dello Scorpion, ripubblicata da Station HYPO il 22-05-2025 — https://stationhypo.com/2025/05/22/why-uss-scorpion-ssn-589-was-lost-on-22-may-1968/ — media.
- [Rule15] B. Rule, «Apparent Failure Mode of the Pressure-Hull of the Israeli (INS) Submarine Dakar», Naval Submarine League, novembre 2015 — https://archive.navalsubleague.org/?p=5598 — media.
- [Sch02] J. Schweitzer, «Some results derived from the seismic signals of the accident of the Russian submarine Kursk», NORSAR Sci. Rep. 1-2002, §6.6 — https://www.norsar.no/getfile.php/1318125-1681727910/norsar.no/R-D/Publications/4877_Schweitzer2002.pdf — alta.
- [SciD01] ScienceDaily, 22-01-2001, analisi LANL del Kursk (Koper et al.) — https://www.sciencedaily.com/releases/2001/01/010122094229.htm — media.
- [SeaDog] rapporto di pattuglia dell'USS Sea Dog (SS-401), copia Scribd con OCR scadente — https://www.scribd.com/doc/175592261/SS-401-Sea-Dog — bassa.
- [Seal] Pigboats, USS Seal (SS-183) — https://archive23.pigboats.com/subs/183.html — bassa.
- [SP00] StrategyPage, 21-10-2000 — https://www.strategypage.com/htmw/htsub/articles/20001021.aspx — bassa.
- [Ste46] C. K. Stedman, US 3 021 807 «Homing system for torpedo», depositato l'1-05-1946 — https://patents.google.com/patent/US3021807 — media.
- [URI-imp] A. Shukla et al., «Underwater implosion mechanics: experimental and computational overview», URI — https://digitalcommons.uri.edu/mcise_facpubs/529/ — media (riassunto).
- [USP4300654] brevetto «Undersea implosion device» — https://patents.justia.com/patent/4300654 — media/bassa.
- [USP6220196] brevetto «Water discharge device for a submerged launching system» — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6220196 — media.
- [USP6622647] brevetto «Active noise cancellation for a torpedo seeker head» — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6622647 — media.
- [Ver19] J. Vergoz, Y. Cansi, Y. Cano, P. Gaillard (CEA/DAM), «Analysis of hydroacoustic signals associated to the loss of the Argentinian ARA San Juan submarine», CTBT SnT 2019 (diapositive) — https://ctnw.ctbto.org/ctnw/event/4799/slides/6823D7459CC44B72B11B3A98BF76B075 ; abstract: https://conferences.ctbto.org/event/17/contributions/3354/ — alta. L'articolo su *Pure Appl. Geophys.* (doi 10.1007/s00024-020-02625-7) non l'ho aperto (rimanda al login).
- [VP4] VP-4 Association, «Mk 46 torpedo» — https://www.vp4association.com/aircraft-information-2/32-2/mk-46-torpedo/ — media/bassa.
- [Wel60] W. E. Wells, US 4 216 534 «Decoying acoustic homing torpedoes», depositato il 28-06-1960, pubblicato nel 1980, US Navy — https://patents.google.com/patent/US4216534 — media.
- [WP-Mk24], [WP-Mk46], [WP-Mk48], [WP-T53] Wikipedia, voci «Mark 24 mine», «Mark 46 torpedo», «Mark 48 torpedo», «Type 53 torpedo» (consultate il 25-10-2026) — media/bassa.
- [Yang25] 杨琼方, 张晓平, 张明敏, «水下平台发射噪声声源排序的理论分析和试验验证», *振动工程学报* (ISSN 1004-4523), 2025 — https://zdxb.nuaa.edu.cn/zdgcxb/article/pdf/202502019 — media.
- Dossier del progetto: 07 §1 (Cole e Willis, [Brain16], [Sw78]), 17 §8.3, 18 §3, §7 e §8, 18_note/bordo.md, 09 (transitori 120-150 dB), 05 §4 (salve di missili), 01 (WLR-9).

## 8. Punti n.v.

1. **Livelli misurati di qualunque transitorio di lancio** (allagamento, porta, sparo, ritorno del ram) di un 688 o di
   un sovietico: n.v. Il solo dato numerico è il getto ≈ 140 dB di un prototipo cinese [Yang25].
2. **Giri, cilindri e pale** del Mk 48 (salvo le 15 + 12 pale dell'ADCAP Mod 6AT), del Mk 46 e dei sovietici; righe del
   motore e del riduttore; livelli irradiati misurati di qualunque siluro: n.v. Urick 1983 (capitoli sul rumore dei
   siluri) è da leggere su carta.
3. **Frequenze e livelli delle teste attive** di Mk 48, ADCAP, Mk 46, SET-65, USET-80, TEST-71 e AT-1: n.v. Esistono solo
   le bande «di progetto» 20-80 kHz e 24,5 kHz dei brevetti del 1960 e i 60 kHz di quello del 1946.
4. **Cadenze reali dei ping** delle armi del 1989: n.v. Gli esempi sono di brevetti del 1960-64 (1-1,25 s, raddoppio
   sotto ~380 m).
5. Durata del booster del Tomahawk e tempo di risalita della capsula dell'Harpoon: n.v.
6. **Livello del motore a razzo acceso sott'acqua:** n.v. (stima 195-218 dB molto incerta).
7. Livelli dei transitori di bordo (portelli, piani, pompe, TDU, scricchiolii): n.v. Ci sono solo stime di progetto e
   il rilevamento SOSUS della soffiata del Thresher a ≈ 30 nmi.
8. Energia e quota di collasso pubblicate solo dall'analista Rule: i dati SOSUS del Thresher sono ancora classificati.
   Il ritardo cepstrale del CTBTO per il San Juan (0,31-0,35 s) non torna con la bolla di Rule (0,21-0,26 s).
9. Distanze dei siti AFTAC che sentirono lo Scorpion: n.v.
10. Livelli assoluti dei «breaking-up noises», dell'urto sul fondo e degli allagamenti: n.v. Ci sono solo durate e bande
    dalle stazioni CTBTO (MOL Comfort 5-10 min, El Faro 10 s, 1-80 Hz a migliaia di km).
11. Il rapporto DTIC del 1971 sui serbatoi d'acciaio imploduti (ADA000807) e [PH00] non si sono aperti (403): i loro
    numeri vengono da riassunti.
12. Scarico dell'aria d'impulso del 688 nel locale siluri: n.v.
