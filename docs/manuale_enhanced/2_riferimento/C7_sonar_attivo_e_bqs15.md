# C7. Sonar attivo e BQS-15

*Autore: kit, per il runtime. Revisione del 28/10/2026 (intercettazione e allarme siluro). Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Il sonar attivo emette un impulso e ascolta l'eco. In cambio dà quello che il passivo non dà: la distanza, e quindi
la posizione del bersaglio in un colpo solo. Il prezzo è alto. L'impulso viaggia una volta sola fino a chi ascolta,
mentre l'eco deve fare andata e ritorno: chi è pingato sente il ping molto più lontano di quanto il pinger senta
l'eco. Un ping dice a tutti dove sei. Per questo in partita si usa per l'ultima distanza prima del lancio, quando la
sorpresa non serve più. Lo usano anche le navi dell'IA per tenere sotto un sottomarino scoperto: ogni ping ti avvisa
che ti stanno cercando. Il **BQS-15** del 688 è un attivo ad alta frequenza e a corto raggio per le mine. Acceso,
spazza la prua ogni 8 secondi e mette le mine sulla carta. Anche lui si fa sentire.

## 2. Il modello

L'equazione dell'attivo e la probabilità di scoperta sono nel capitolo C1 (§2.3 e §2.4). Qui si spiegano le parti:
la forza di bersaglio, la riverberazione, il Doppler, la soglia, il ping nella missione, la contro-scoperta e il
BQS-15. Il codice è in `nucleo/src/acustica.cpp` (`valuta`, `forza_bersaglio`, `forza_superficie_db`) e in
`missioni/src/missione.cpp` (`Missione::impulso_sonar_enhanced`, `Missione::intercetta_impulso`); il BQS-15 in
`missioni/src/missione_bqs15.cpp`.

### 2.1 I sonar attivi

Un sensore è attivo se in `data/motore/acustica.toml` ha `modo = "attivo"`. Oltre ai campi di ogni sensore (C1 §2.8)
ha il livello dell'impulso `sl_attivo_db` (dB re 1 µPa a 1 m), la durata `impulso_s` (s) e, per gli impulsi FM con
compressione, `risoluzione_s` (1/B, la durata equivalente dopo la compressione). La banda è una sola, fra quelle del
modello (`BANDE_HZ`: 50, 150, 300, 1000, 3500, 10 000, 25 000 Hz).

| Sensore | Chi lo ha | SL (dB) | Banda (Hz) | Impulso (s) |
|---|---|---|---|---|
| `bqq5_attivo` (BQQ-5, sfera) | Los Angeles, Sturgeon, Trafalgar | 225 | 3500 | 0,5 |
| `sovietico_attivo` | tutti i sottomarini sovietici | 222 | 3500 | 0,5 |
| `sqs53_attivo` (SQS-53) | Kidd, Spruance | 235 | 3500 | 1,0 |
| `sqs56_attivo` (SQS-56) | Oliver Hazard Perry | 222 | 10 000 | 0,5 |
| `titan2_attivo` (MG-332 Titan-2) | Krivak II, Kara, Slava, Kirov, Koni | 228 | 3500 | 0,5 |

Fonte: `data/motore/acustica.toml [sensori.*]` e `[suite]`; la tabella completa, generata, è nel capitolo C1 §2.8.

- Una nave senza suite ma con il sonar dell'originale (`sonar` > 0) riceve `sqs53_attivo` se è occidentale e
  `titan2_attivo` se è sovietica (`Missione::sensori_acustici`).
- Il Nanuchka non ha attivo. Gli aerei, le armi e le esche non pingano col sonar di bordo.
- Gli attivi dell'ASW aereo (boe DICASS, RGB-55A, sonar a immersione AQS-13 e VGS) usano la stessa equazione, con
  guadagno d'antenna dato, eco omnidirezionale e un errore della distanza (`sigma_distanza_m`): sono nel capitolo E5.

### 2.2 La forza di bersaglio

La forza di bersaglio TS (dB) dice quanto un bersaglio rimanda dell'energia che lo colpisce. Dipende dall'aspetto φ,
l'angolo fra la prua del bersaglio e la direzione da cui arriva il ping (0 = di prua, 90 = al traverso):

```
TS(φ) = TS_prua + (TS_traverso − TS_prua) · |sin φ|³ + 4 · exp(−((φ − 90) / 7)²)
```

- `TS_prua`, `TS_traverso`: della firma, `data/motore/acustica.toml [firme.<classe>] ts_prua_db, ts_traverso_db`.
- L'ultimo termine è il lobo speculare: +4 dB esatti al traverso, che calano a +1,5 dB a 7° dal traverso.
- Per le sorgenti della libreria (biologici, sorgenti civili) vale un valore fisso, `ts_db` della voce.

Valori nei dati:

| Bersaglio | TS di prua (dB) | TS al traverso (dB) |
|---|---|---|
| Los Angeles, Sturgeon, Trafalgar, Charlie II, Foxtrot, Kilo | 12 | 25 |
| Victor III, Alfa, Akula (rivestimento anecoico) | 2 | 15 |
| Oscar II, Typhoon | 15 | 30 |
| fregate e cacciatorpediniere | 18-22 | 30-35 |
| Nanuchka, AGI | 15 | 28 |
| Kirov, Iowa, Nimitz, cargo, petroliera | 25 | 40 |
| Sacramento, mercantile | 25 | 38 |
| siluri pesanti (Mk 48, Type 53, generico) | −25 | −20 |
| siluri leggeri (Mk 46, AT-1) | −28 | −23 |
| esca ADC Mk 2 | 10 | 10 |
| biologico | 0 | 10 |

Il rivestimento anecoico toglie 10 dB a tutti gli aspetti (`tools/pipeline/acustica.py`, dossier 02 §1.4). Un
Victor III al traverso dà quindi 19 dB, di prua 2 dB; a 45° circa 6,6 dB. Un 688 nelle stesse condizioni dà 29, 12
e 16,6 dB.

L'esca semovente (MG-74, MOSS) prende la firma del battello che l'ha lanciata, quindi anche la sua TS
(`Missione::sorgente_acustica`, capitolo D2). Un relitto (scheda I10) è «muto»: niente rumore, ma l'eco resta.

### 2.3 La riverberazione

La riverberazione RL è l'eco di tutto il resto: l'acqua, la superficie e il fondo. Nell'equazione il segnale si
confronta col maggiore fra il rumore e RL (C1 §2.3). RL è la somma in potenza di tre parti, calcolate per la banda
del sensore.

**Volume.**

```
RL_volume = SL − 2 TL + Sv + 10 log V          V = r² · ψ · c · τ / 2          ψ = 4π · 10^(−DI/10)
```

- `Sv` = −80 dB: forza di diffusione di volume, a metà fra −90 e −70 (`SV_VOLUME_DB`).
- `r` la distanza orizzontale, `c` = 1500 m/s, `τ` la durata dell'impulso.
- `ψ` l'angolo solido equivalente del fascio, dal guadagno d'antenna DI. Per il BQQ-5 a 3500 Hz il DI è 30,6 dB e
  ψ = 0,011 sr.
- `TL` è la stessa perdita dell'eco: il riverbero di volume e l'eco crescono e calano insieme.

**Superficie** (Chapman e Harris, 1962). Si considera l'anello di superficie che riporta l'eco insieme al bersaglio,
cioè alla stessa distanza obliqua R:

```
Ss = 3,3 β log(θ / 30) − 42,4 log β + 2,6          β = 158 · (v · f^(1/3))^(−0,58)
RL_superficie = SL − 2 TL_sup + Ss + 10 log(R · Φ · c · τ' / 2) − lobo(θ)
```

- `v` il vento in nodi (almeno 1), `θ` l'angolo radente in gradi, limitato fra 0,1 e 30.
- Il tetto di Ss è −20 dB (`SS_MASSIMA_DB`, STIMA).
- `Φ` è la larghezza orizzontale del fascio in radianti (per la sfera 58,4 λ / D: 5,4° per il BQQ-5 a 3500 Hz); per
  un sensore omnidirezionale è tutto il giro.
- `τ'` è la risoluzione dell'impulso: `risoluzione_s` se c'è (FM compresso), altrimenti la durata.
- `TL_sup` è la perdita fino al punto della superficie, alla distanza orizzontale √(R² − z²).
- Il trasduttore sta almeno a 3 m (`QUOTA_MIN_RIVERBERAZIONE_M`): per le navi, la chiglia.

**Fondo** (legge di Lambert). Lo stesso anello, sul fondo:

```
RL_fondo = SL − 2 TL_fondo − 27 + 20 log(h / R) + 10 log(R · Φ · c · τ' / 2) − lobo(θ)
```

- −27 dB è la costante di Lambert (`SB_FONDO_DB`, Mackenzie 1961); `20 log(h / R)` è il `10 log sin²θ` della legge,
  con `h` l'altezza del trasduttore sul fondo.
- Il fondo conta solo se il fondale è noto e se la distanza obliqua supera l'altezza sul fondo.

**Lobo verticale.** Superficie e fondo visti di sbieco cadono fuori dal fascio. Il codice attenua andata e ritorno
con un lobo gaussiano largo quanto il fascio (STIMA):

```
lobo(θ) = 2 · min(30, 3 x²)          x = θ / (Φ_gradi / 2)
```

Sono 3 dB per verso a metà fascio, al più 60 dB in tutto.

**Battello posato.** Se il bersaglio è posato sul fondo (o è un relitto) l'eco si confonde col fondo. RL diventa
almeno la riverberazione di fondo alla distanza del bersaglio, sulla stessa perdita dell'eco:

```
RL ≥ SL − 2 TL − 27 + 10 log(r · Φ · c · τ / 2)
```

Qui conta la durata vera dell'impulso, non la risoluzione. È il motivo per cui posarsi sul fondo nasconde dai sonar
attivi (volume 1, §2.5).

### 2.4 Il Doppler

Un impulso CW separa dalla riverberazione un bersaglio che si muove. La velocità radiale è la differenza fra la
velocità del bersaglio e quella del pinger, proiettate sulla congiungente:

```
v_rad = V_bersaglio · cos(rotta_b − rilevamento) − V_pinger · cos(rotta_p − rilevamento)
```

Con |v_rad| > 2,5 nodi (`DOPPLER_RIVERBERAZIONE_KN`) RL cala di 10 dB (`DOPPLER_GUADAGNO_DB`). Il guadagno è a
gradino: 2,4 nodi non valgono niente, 2,6 valgono tutti i 10 dB. Un bersaglio fermo, o che va al traverso del
pinger, non ne ha.

### 2.5 Rumore e soglia

- Il rumore NL è quello di sempre (ambiente più rumore proprio, C1 §2.1), integrato sulla banda dell'impulso:
  `NL_banda = NL + 10 log(1/τ)`. Per un impulso di 0,5 s sono +3 dB; per 1 s, zero.
- Nel cono d'ombra poppiero (`baffles_gradi` per parte attorno alla poppa: 35° per il BQQ-5, 30° per i sonar delle
  navi, 40° per i sovietici) il DI vale zero e il rumore proprio sale di 15 dB (`baffles_db`), come nel passivo. Con DI
  nullo anche ψ diventa 4π: il riverbero di volume sale di 30 dB. Verso poppa la sfera non vede.
- La soglia dell'attivo è quella di un ricevitore a replica: `DT = 10 log(d / 2) − abilità` (`soglia`). Con d = 14
  sono 8,45 dB, ±3 dB per l'abilità dell'operatore.
- La stessa soglia vale contro il rumore e contro la riverberazione.

### 2.6 Un esempio: eco contro riverbero di volume

Il riverbero di volume e l'eco hanno la stessa perdita, quindi il loro rapporto non dipende dal mare:

```
eco − RL_volume = TS − (Sv + 10 log V)
```

Per il BQQ-5 (impulso di 0,5 s, ψ = 0,011 sr) `Sv + 10 log V` vale −13,8 dB a 1 km, −4,3 dB a 3 km e +6,2 dB a
10 km: cresce di 20 dB per decade. La tabella dà `eco − RL_volume`, da confrontare con la soglia di 8,45 dB:

| Bersaglio | 1 km | 3 km | 10 km |
|---|---|---|---|
| Victor III al traverso (TS 19 dB) | 32,8 | 23,3 | 12,8 |
| Victor III di prua (TS 2 dB), fermo in radiale | 15,8 | 6,3 | −4,2 |
| lo stesso con più di 2,5 nodi radiali | 25,8 | 16,3 | 5,8 |

I valori sono in dB, dalle formule del §2.2-2.4; contano solo il volume. Superficie e fondo, il rumore e la perdita
vera possono soltanto peggiorarli. Un Victor III di prua a 10 km non esce dal riverbero di volume nemmeno col
Doppler. Al traverso sì. Il capitolo C1 (§2.7) dà l'equazione intera dello stesso caso: SE +4,4 dB a 10 km.

### 2.7 Il ping nella missione

Il ping è un evento, non un modo del sonar: ogni impulso è una valutazione sola
(`Missione::impulso_sonar_enhanced`).

1. **Il sensore.** Il primo sensore attivo della suite dell'unità. Se lo scenario dà all'unità un `sonar_speciale`
   (scheda I09, `docs/09_schema_missioni.md` §4), quello prende il suo posto: SL, frequenza (portata alla banda più
   vicina), durata, risoluzione FM, dimensione e tipo d'antenna. Senza sensore attivo non succede niente.
2. **L'eco.** Per ogni altra unità viva si calcola `valuta` con l'ambiente fra le due (meteo, profilo, fondale) e i
   campi di perdita del tracciamento dei raggi (C2). L'impulso vale su tutto l'orizzonte, fuori dal cono d'ombra
   poppiero (§2.5): l'eco torna da ogni unità che supera la soglia, non solo da quelle a prua. Il volume 1 (§3.8)
   parla di chi è «nel fascio»: vale questo capitolo. Gli aerei non danno eco.
3. **Il contatto.** Con SE ≥ 0 il pinger ha un contatto con la posizione:
   - distanza vera con un errore gaussiano dell'1% (σ);
   - rilevamento vero con un errore gaussiano di 1° (σ);
   - quota del bersaglio, esatta;
   - qualità della traccia almeno 800 (solida) e segnale `500 + 25 · SE`.
4. **La contro-scoperta.** Lo stesso impulso va, di sola andata, ai sensori passivi di tutti gli altri (§2.8).

L'eco non ha fluttuazione: la decisione è netta, SE ≥ 0. Il ping lascia anche un evento acustico di 33 s (300 passi
dell'originale, `evento_acustico`): per quel tempo il rumore irradiato del pinger sale di 10 dB
(`Missione::sorgente_acustica`, `transitorio_db`).

Chi pinga:
- **il giocatore**, col pulsante dell'impulso alla stazione Sonar (`TipoOrdine::impulso_sonar`). Nell'Enhanced, come
  l'IA, non ripinga prima che torni l'eco: 10 s fra due ping (`INTERVALLO_PING_S`, l'eco da circa 7,5 km; la simmetria,
  scelta dell'utente del 31/10/2026). Un ping chiesto prima è rifiutato col messaggio 414, «il ping precedente non è
  ancora tornato»; il classico resta senza limiti di cadenza. Col sonar fuori uso l'ordine è rifiutato col messaggio
  63; nell'Enhanced vale il sonar del battello danneggiato (calcolatore e sonar di prua almeno al 30%);
- **gli script** delle missioni, con `m688.impulso_sonar(u)`: le missioni originali (TORPEX, le navi in attacco) e la
  libreria. Nella libreria di comportamenti (`missioni/src/scenario_missione.cpp`) una nave pinga a ogni valutazione
  di un contatto con probabilità 0,05, 0,2, 0,4 o 0,7 secondo l'allerta (`PING`, la «paranoia» dell'editor);
- **le reazioni dell'IA** (`missioni/src/missione_reazioni.cpp`): la nave antisom più vicina che «tiene sotto» un
  sottomarino scoperto in crisi pinga ogni 30 s (`PING_OGNI_S`); in pace, la nave che pedina un sottomarino tenta un
  ping una volta al minuto con la probabilità dell'allerta (`PING_ALLERTA`, gli stessi valori della libreria). Il
  resto è nel capitolo E6;
- **i sonar speciali** con `ogni_s` > 0, da soli a cadenza fissa.

### 2.8 La contro-scoperta

L'impulso arriva a chi ascolta per la sola andata (C1 §2.4):

```
SE_ping = SL_ping − TL − (NL + 10 log B − DI) − 5 log(d / max(B τ, 1))          B = 0,7 f
```

- Nella banda dell'impulso il ricevitore tiene il maggiore fra questo SE e quello del rumore irradiato del pinger.
- `Missione::intercetta_impulso` prova tutti i sensori passivi dell'ascoltatore (sfera, schiera filata,
  intercettatori) e tiene il migliore.
- Un sensore sente l'impulso solo se ha una banda che cade nella stessa banda del modello. Il ping a 3500 Hz lo
  sentono il WLR-9, l'intercettatore sovietico (Enisej), l'SQS-53 passivo e il sonar di prua delle navi sovietiche.
  Le sfere dei sottomarini (bande fino a 1000 Hz) no. Il ping a 10 kHz dell'SQS-56 della Perry lo sentono solo gli
  intercettatori.
- Il WLR-9 e l'Enisej hanno idrofoni a larga apertura, senza guadagno d'antenna (`di_fisso_db = 0`).
- Il contatto che ne nasce ha **solo il rilevamento** (errore gaussiano di 1°), qualità almeno 400, segnale
  `500 + 25 · min(SE, 40)`. Vale per il giocatore e per l'IA allo stesso modo.

Poi:
- il giocatore riceve il messaggio 312 e l'evento `ping_subito` nel registro;
- un'unità dell'IA ostile al pinger alza l'allerta (`evento_allerta`, «ping»);
- un sottomarino dell'IA ostile in pace, entro 2 nm (3704 m), con probabilità 0,7 si allontana di 10 km a 12 nodi al
  più, per 10 minuti, una volta per coppia; non nelle missioni originali;
- un sottomarino dell'IA immerso ostile al pinger riceve lo stimolo d'evasione: «ping vicino» entro 5 km, «ping
  lontano» oltre (capitolo E6).

### 2.9 Il BQS-15

L'AN/BQS-15 è il sonar ad alta frequenza della vela del 688. Il modello fa le mine e, sotto la banchisa, le chiglie del
ghiaccio: ogni 5 s `spazzata_volta` misura le chiglie a prua per la volta del battello (capitolo E11 §2.2), col
sondatore verso l'alto. Ostacoli diversi dal ghiaccio e dalle mine, e il fondo davanti, non li vede. Tutte le cifre sono
STIME DI GIOCO del dossier 06, perché quelle vere sono classificate.

- **L'Alfa del giocatore** ha i sonar da mine sovietici: l'MG-70 e, sui battelli che lo montano, l'MG-24, coi loro
  messaggi (493-495 l'MG-70, 496-498 l'MG-24: acceso, mina, spento), le loro portate (`fattore_portata`) e la loro
  emissione. Funzionano come il BQS-15: lo stesso ordine (Ctrl+M), le stesse spazzate, le stesse scoperte.

- **Chi lo ha:** ogni classe il cui nome comincia con `los_angeles` (`Missione::ha_bqs15`), solo nell'Enhanced
  (servono l'acustica o i siluri dell'Enhanced).
- **Acceso e spento:** ordine `bqs15` (Ctrl+M), funzione `m688.bqs15(u [, acceso])` per gli script. La prima
  spazzata parte subito.
- **La spazzata:** ogni 8 s (`SPAZZATA_S`), il settore di prua di ±60° in azimut (`SETTORE_DEG`) e da −30° a +45° in
  elevazione.
- **La portata:** dalla distanza in tre dimensioni, con un fattore della velocità propria:

  ```
  portata = P · f(v)        P = 1100 m per una mina ormeggiata, 450 m per una mina sul fondo
  f(v) = 1 fino a 5 kn;  1 − 0,03 (v − 5) fino a 15 kn;  0,7 − 0,06 (v − 15) fino a 20 kn;  0,4 oltre
  ```

  Una mina è ormeggiata se sta più di 2 m sopra il fondo (`ORMEGGIO_MIN_M`). A 10 nodi una ormeggiata si vede a
  935 m, a 15 nodi a 770 m, a 25 nodi a 440 m.
- **La misura:** distanza con 5 m d'errore (σ), azimut ed elevazione con 1° (σ). La posizione e la quota stimate si
  aggiornano a ogni spazzata, finché la mina è nel settore. Le mine già scoppiate o in corsa non si vedono.
- **Chi tiene le scoperte:** il giocatore (`mine_scoperte()`) e, dal 30/10/2026, anche l'IA per simmetria. Un
  sottomarino dell'IA col BQS-15 acceso, o coi sonar da mine sovietici MG-70 e MG-24, tiene le sue mine scoperte e il
  pilota le aggira a 300 m (`Missione::evita_mine`, capitolo D4 §2.8).
- **La contro-scoperta:** ogni spazzata è un impulso di 205 dB a 25 kHz per 10 ms. A ogni unità arriva con
  `intercetta_impulso`: a pieno livello se è nel settore di prua (si guarda solo l'azimut), 25 dB sotto (i lobi
  laterali, `LOBI_DB`) se è fuori. A 25 kHz lo sentono solo gli intercettatori (WLR-9, Enisej). Chi lo sente reagisce
  come a un ping (§2.8): allerta, ritirata in pace, stimolo d'evasione. La contro-scoperta c'è solo con l'acustica.

Il calcolo della portata nel dossier 06 §1.1: SL 205 dB, NL 66 dB nella banda di 500 Hz col mare 3, DI 20 dB, DT
10 dB; una mina ormeggiata con TS −15 dB si vede fin dove 2 TL ≤ 134 dB, circa 1,1 km. Il gioco usa la portata
fissa, non l'equazione del sonar.

#### Il BQS-8 degli Sturgeon

Gli Sturgeon non hanno il BQS-15 ma il loro sonar da ghiaccio, l'AN/BQS-8 (dossier 34, del disassemblatore). Il BQS-14
stava solo sulla Lipscomb (SSN 685), il BQS-14A sulla Parche.
- **Che cosa guarda:** dall'orizzonte in su, ±45° a prua, con metà della portata del BQS-15: le chiglie del pack fino
  a circa 1.250 m.
- **Che cosa non vede:** le mine e il fondo (STIMA: nessuna fonte aperta dice che vedesse le mine).
- **Il giocatore** con uno Sturgeon lo accende e lo spegne col tasto del BQS-15 (Ctrl+M): messaggi 490 «BQS-8 acceso»
  e 491 spento. L'avviso della chiglia è il messaggio 487 «Sonar del ghiaccio: chiglia a prua!».
- **L'IA:** lo Sturgeon lo tiene acceso sotto il pack, salvo quando è in silenzio. La sua spazzata è un impulso di
  205 dB a 40 kHz che gli altri possono sentire, come quella del BQS-15 (`missione_ghiaccio.cpp`). Il BQS-8 non sonda il
  fondo: la spazzata del fondo dell'IA (`missione_fondale_ia.cpp`) vale solo per i sonar che guardano sotto l'orizzonte.
- La sua riga è nella tabella del capitolo C1.

### 2.10 Il ricevitore d'intercettazione e l'allarme siluro (blocco 2 del sonar, E)

Solo Enhanced con l'acustica. Il modulo puro è `nucleo/intercettazione.hpp` (disassemblatore). L'aggancio nella
missione è `missioni/src/missione_intercettazione.cpp` (runtime). La pagina della stazione è
`app/pagina_intercettazione` (trailer-1, cablata dalla principale). Il progetto è in `docs/22_sonar_blocco2.md` §2,
la ricerca nel dossier 28 §1-§3.

Il contatto dal ping del §2.8 resta com'è. Il ricevitore d'intercettazione aggiunge quello che un WLR-9 vero fa in più:
- **misura** ogni impulso;
- **lega** gli impulsi in tracce per emettitore;
- **dice** che cosa li emette;
- **accende** l'allarme siluro.

**Chi emette** (`Missione::emetti_impulso`):
- il ping attivo di scafo (`impulso_sonar_enhanced`), anche del sonar speciale degli scenari con la sua frequenza;
- il BQS-15, pieno nel settore di prua e 25 dB sotto fuori;
- le teste attive dei siluri, a ogni loro ping: la cadenza si stringe man mano che la testa si avvicina al bersaglio.

Il catalogo delle emissioni (`[emissioni.*]` di `acustica.toml`) dà a ognuna tipo, frequenza, durata, CW o FM e
cadenza tipica. Per esempio: SQS-53 3,3 kHz, 1,5 s, FM, ogni 25 s; BQQ-5 3,5 kHz, 0,5 s, CW, ogni 60 s; BQS-15 25 kHz,
10 ms, ogni 2 s. Le teste vengono dai transitori `ping_testa_*` della libreria.

**I ricevitori** (`[intercettatori.*]`, tutto **STIMA**; dossier 28 §1.3 e §2):

| Ricevitore | Classi | Bande (kHz) ed errore del rilevamento | Soglia, risoluzione | Misura | Falsi allarmi |
|---|---|---|---|---|---|
| WLR-9 | Los Angeles, Sturgeon, Trafalgar | 1-6 ±12°, 6-15 ±6°, 15-45 ±6°, 45-100 ±8° | 10 dB, 100 Hz | durata | 0,1 all'ora |
| Skat | Victor III, Akula, Oscar, Typhoon | 1,5-12 ±8°, 12-60 ±6° | 10 dB, 100 Hz | durata e CW/FM | 0,5 all'ora |
| Rubikon | Kilo | 1,5-12 ±12°, 12-40 ±10° | 12 dB, 200 Hz | durata | 1 all'ora |
| Rubin | Alfa, Charlie II, Foxtrot | 2-10 ±15°, 10-40 ±15° | 12 dB, 300 Hz | solo frequenza e rilevamento | 2 all'ora |

La soglia è il rapporto segnale/rumore in un canale di 1 kHz, con l'impulso a sola andata e il rumore proprio del
sensore d'intercettazione dell'unità. I falsi allarmi sono intercetti spuri del proprio battello: lo «Skat-3» sovietico
li aveva davvero (Kazakov 2019).

**Le misure.** Ogni intercetto ha:
- la frequenza, col Doppler a sola andata;
- la durata, se il ricevitore la misura;
- il livello sopra la soglia;
- il rilevamento, con l'errore della banda;
- CW o FM, solo per lo Skat.

Gli errori vengono dal seme e dall'istante: la stessa partita dà le stesse misure.

**Le tracce.** Un intercetto va nella traccia della stessa banda con la frequenza entro 400 Hz (o il 3%) e il
rilevamento entro 15° (o tre volte l'errore della banda), se l'ultimo intercetto è recente (2 minuti, o quattro
cadenze). Altrimenti apre una traccia nuova. Una traccia tiene gli ultimi 12 intercetti. Da questi il ricevitore stima:
- la **cadenza** (PRI), come mediana degli intervalli;
- la **deriva del rilevamento**, in gradi al minuto sull'ultimo minuto, col suo errore;
- la **salita del livello**, riportata a 10 s;
- il **Doppler**: + se la frequenza sta sopra quella tipica più vicina della banda, − se sta sotto, oltre la
  risoluzione.

La storia degli ultimi 30 minuti resta per la cascata della pagina.

**La classe dell'emettitore** (`valuta_traccia`):

| Classe | Condizione |
|---|---|
| testa d'arma | da 15 kHz in su, impulso fino a 50 ms, cadenza fino a 3 s (o ancora ignota) |
| ricerca ad alta frequenza (come il BQS-15) | da 15 kHz in su, ma più lungo o più lento |
| sonar a immersione | 8,5-12 kHz, impulso fino a 50 ms |
| boa attiva | da 6 kHz, impulso di almeno 80 ms, cadenza fra 6 e 15 s |
| sonar di scafo | il resto |

Il Rubin non misura la durata: per lui è testa d'arma tutto quello che sta da 15 kHz in su, e sonar di scafo il resto.

**L'allarme** (dai brevetti USN del dossier 28 §3.3, **STIMA**):
- **SILURO IN ACQUA:** una traccia di classe testa d'arma con almeno due intercetti;
- **SILURO IN ACQUISIZIONE:** la stessa traccia mostra tutte e quattro le cose insieme:
  - la cadenza in calo, oltre il 15% sugli ultimi intervalli (fino a 6) e senza risalite; senza la durata questa
    condizione salta;
  - il livello in salita, almeno 3 dB in 10 s;
  - il rilevamento fermo, sotto 2° al minuto, a meno di due volte il suo errore;
  - un Doppler non negativo.

Una testa che si allontana dà SILURO IN ACQUA ma mai l'acquisizione. **Esempio** (README del motore): una Mk 48 a 45
nodi contro il WLR-9 dà SILURO IN ACQUA a 3,9 km e SILURO IN ACQUISIZIONE a 1,2 km.

**Nella missione** (`Missione::passo_intercettazione`), a ogni passo, per ogni unità con un ricevitore della sua classe:
- **gli impulsi:** ascolta quelli degli altri, mai i propri, col livello verso di lei (il fascio del BQS-15 compreso);
- **il giocatore:** quando l'allarme sale riceve il messaggio 311 «Sonar: siluro in acqua!» o il 475 «Sonar: SILURO IN
  ACQUISIZIONE!». Torna solo se l'allarme scende e risale;
- **l'IA:** un sottomarino dell'IA che sente salire l'allarme evade come per un siluro scoperto, da un punto a 3 km sul
  rilevamento della traccia (da poppa, se non ce n'è una). Con l'acquisizione è uno stimolo di siluro vicino. La
  difesa spenta della sandbox lo ferma. È la simmetria: l'IA sente quello che senti tu.

**Nella stazione** (volume 1, §3.8):
- **la targa d'allarme:** con l'allarme acceso, una targa lampeggia in alto in tutte le stazioni, gialla per SILURO IN
  ACQUA e rossa e più veloce per SILURO IN ACQUISIZIONE, con «CTRL+W»;
- **il suono:** tre bip a 900 Hz per SILURO IN ACQUA, toni alternati a 700 e 1100 Hz per due secondi per SILURO IN
  ACQUISIZIONE. Suona solo quando l'allarme sale di livello. È un suono del remake, non documentato a bordo dei 688
  (`InterfacciaMissione::suona_allarme`);
- **la pagina:** Ctrl+W apre o chiude da ogni stazione la pagina dell'intercettazione, sulla stazione Sonar; anche
  Esc la chiude. In alto
  le lampade dell'allarme e il nome del ricevitore. A sinistra la rosa dei rilevamenti delle tracce vive. A destra la
  cascata frequenza-tempo da 1 a 100 kHz, con le bande scritte. In basso la lista delle tracce: rilevamento,
  frequenza, durata, cadenza, deriva, livello, CW/FM, Doppler, classe ed età. Quello che il ricevitore non misura è
  «--»;
- **i comandi della pagina:** frecce su e giù (o un clic sulla riga) scelgono la traccia; frecce sinistra e destra
  (o un clic sui tasti della finestra) cambiano la finestra della cascata (2, 5, 10 o 30 minuti);
- **il modo 1989:** cascata al fosforo con la persistenza della stazione e testo verde.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| TS per aspetto, sin³ φ e lobo di +4 dB a ±7° | dossier 02 §1.4 (forma suggerita, p da 2 a 4) | media (forma); bassa (valori) |
| TS dei sottomarini, 10-15 di prua e 25 al traverso | dossier 02 §1.4 [Urick] | media |
| Anecoico −10 dB | dossier 02 §1.4 (10-20 dB) | bassa: il valore è al limite basso |
| Riverberazione di volume, Sv = −80 dB | dossier 02 §1.3 (−90 … −70) | media |
| Superficie, Chapman e Harris | dossier 02 §1.3 | alta (forma) |
| Tetto di Ss −20 dB, lobo verticale gaussiano | — | **STIMA**: evitano un riverbero eccessivo col vento forte e verso la superficie |
| Fondo, Lambert −27 dB | dossier 02 §1.3 [Mackenzie 1961] | media; non cambia col tipo di fondo |
| Doppler, 2,5 nodi e 10 dB | dossier 02 §1.3 (separazione Doppler del CW) | **STIMA** |
| DT dell'attivo, `10 log(d/2)` | dossier 02 §1.2 [Urick] | alta (forma); **STIMA** d |
| SL e durate dei sonar attivi | dossier 02 §1.5 | bassa: i livelli veri non sono pubblicati |
| Contro-scoperta, sola andata, 1,5-3 volte la portata dell'eco | dossier 02 §5.3 | alta (principio); media (rapporto) |
| BQS-15: 205 dB, 25 kHz, settore, 8 s, portate | dossier 06 §1.1, §1.5 e §7.1 | **STIMA DI GIOCO**: frequenza e livello classificati |
| BQS-15: −30% a 15 kn, −60% oltre i 20 | dossier 06 §1.1 | **STIMA DI GIOCO** |
| BQS-15: lobi laterali −25 dB | dossier 06 §1.5 (−20/−25) | **STIMA** |
| Probabilità dei ping dell'IA per allerta | dossier 24 §8.1 | **STIMA** di gioco |

| Ricevitori d'intercettazione (bande, errori, soglia, risoluzione) | dossier 28 §1.3 (WLR-9) e §2 (Kazakov 2019) | **STIMA**, nessun numero aperto |
| Allarme siluro: classe, cadenza in calo, livello, rilevamento fermo, Doppler | dossier 28 §3.3 (brevetti USN) | **STIMA** |
| Evasione dell'IA da 3 km sul rilevamento | scelta di gioco | — |

## 4. Classico ed Enhanced

Nel classico il sonar attivo non è un sensore ma un **evento acustico** (`docs/originale/sensori_e_navigazione.md`
§4):
- il ping del giocatore (comando 2Dh) e l'istruzione `PING` degli script mettono nella lista degli eventi un evento
  di 300 passi, 33 s;
- l'unico rifiuto è il sonar di tiro guasto (`docs/originale/numeri_eseguibile.md` §3.6): niente limiti di quota, di
  velocità o di cadenza;
- **chi pinga si rivela:** a chiunque lo valuti si somma E = 1000 nella formula di scoperta, che basta a coprire i
  20 nm in ogni direzione. L'originale voleva 1000, 2000 o 5000 secondo la durata residua, ma legge il campo sbagliato
  e vale quasi sempre 1000;
- **il ping illumina la prua:** per chi pinga, ai bersagli entro ±72° dalla prua si somma 2200 (±36°), 1400 (±52°) o
  800 (±72°). Non c'è eco, distanza o riverbero;
- alle altre unità arriva il messaggio 8 «Sonar attivo», a qualunque distanza.

Il BQS-15 nel classico non esiste.

Nell'Enhanced con l'acustica l'evento resta (dura sempre 33 s e alza di 10 dB il rumore del pinger), ma non entra
più nella scoperta: l'eco e l'intercettazione vengono dall'equazione del sonar. Il messaggio 8 lo manda ancora la
funzione degli script `m688.impulso_sonar`, senza condizioni; i ping delle reazioni dell'IA e dei sonar speciali
passano solo dall'intercettazione e dal 312. Senza i dati acustici l'Enhanced torna alla scoperta del classico.

## 5. Come si vede in gioco

- **Il ping proprio:** stazione Sonar, pulsante 2 (volume 1, §3.8; tasti nell'appendice A). Il contatto pingato
  diventa subito solido e va sulla carta con la posizione. Nei locali prodieri si sente il ping del BQQ-5
  (capitolo C8).
- **I ping degli altri:** messaggio 312 «Ci stanno pingando», un contatto col solo rilevamento sul pinger. Alla
  cuffia i ping lontani si sentono come un ticchettio (C8). I ping dell'ASW aereo sono nel capitolo E5 e nel volume
  1, §8.2.
- **Il BQS-15:** Ctrl+M (volume 1, §3.8; appendice A). Messaggi 450 «BQS-15 in trasmissione: chi ascolta ci può
  sentire», 451 «BQS-15: mina a prua» alla prima scoperta di una spazzata, 452 «BQS-15 spento». Sulla carta le mine
  scoperte sono un cerchio rosso chiaro con la croce e la scritta «MINA ORM.» o «MINA FONDO» con l'ora della
  scoperta (`app/carta.cpp`; volume 1, §5.1). Le mine e la loro posa sono nel capitolo D4.
- **Intercettazione** (§2.10): la targa SILURO IN ACQUA o SILURO IN ACQUISIZIONE in tutte le stazioni, Ctrl+W per la
  pagina; messaggi 311 e 475.
- **Registro e debriefing** (capitolo F1): `ping_attivo` per ogni ping proprio, `ping_subito` per ogni ping
  intercettato, `mina_scoperta` per ogni mina vista dal BQS-15. Le reazioni dell'IA scrivono «tiene sotto col sonar
  attivo» e «scoperto dal ping, si allontana».
- **Statistiche e carriera:** «PING ATTIVI» nel report (capitolo F4); i ping subiti contano fra le esposizioni. Il
  terzo gradino dell'achievement «fantasma» vuole zero ping propri e zero transitori (capitolo F3).
- **Script:** `m688.ping_giocatore_entro(u, raggio_m [, ultimi_s])` dice se il giocatore ha pingato vicino a
  un'unità; `m688.mine_scoperte()` quante mine ha visto il BQS-15.

## 6. Taratura e prove

- **Contro-scoperta del BQQ-5** (dossier 02 §5.3): alle Fær Øer d'inverno un Akula sente il ping di un 688 più di 1,5
  volte più lontano di quanto il 688 senta l'eco dell'Akula, e più di 10 volte più lontano del 688 silenzioso.
- **BQS-15 all'intercettazione** (dossier 06 §1.5): nel fascio fra 7 e 12 km sia col WLR-9 sia con l'Enisej; il
  capitolo C1 riporta 9,4 km nel fascio e 6,7 km fuori. La prova calcola il fuori fascio con i lobi a −20 dB, mentre
  la missione usa −25 dB: nel gioco la portata fuori fascio è un po' minore. Prima di una correzione l'impulso era
  sovrastimato di 32 dB.
- **BQS-15 nella missione:** due mine nel settore su tre (la CAPTOR alle spalle resta fuori), posizione entro 40 m e
  quota entro 30 m dalla vera; fuori fascio l'impulso arriva 25 dB sotto, al decimo di dB.

Prove in `test/prove_acustica.cpp`:
- «acustica: equazione del sonar, portate relative, attivo e probabilita'» (l'eco al traverso supera quella di prua
  di oltre 10 dB e cala con la distanza);
- «acustica: intercettazione di un impulso attivo (contro-scoperta)»;
- «acustica: l'impulso del BQS-15 all'intercettazione (WLR-9, Enisej): ~10 km nel fascio, ~6 fuori»;
- «acustica: una sorgente muta (il relitto della I10) non si sente al passivo, ma da' l'eco all'attivo».

In `test/prove_acustica_missione.cpp`:
- «acustica: a 30 km il Victor silenzioso non da' una traccia; il suo ping si', ma solo come rilevamento»;
- «acustica: il sonar attivo speciale dello scenario vede l'eco piu' lontano e pinga da solo (scheda I09)»;
- «acustica: l'antenna del sonar speciale si legge e un nome sconosciuto e' un errore».

In `test/prove_bqs15.cpp`:
- «BQS-15: le mine nel settore di prua, l'evento del registro e chi sente gli impulsi (S06)»;
- «BQS-15: la portata cala con la velocita' e con il fondo; niente senza l'Enhanced»;
- «BQS-15: fuori dal fascio di prua l'impulso arriva coi lobi laterali, 25 dB sotto».

Altre prove:
- `test/prove_acustica_asw.cpp`: «acustica asw: il ping si sente 5-12 volte piu' lontano della scoperta dell'eco
  (§8.3)» e «acustica asw: riverberazione di superficie e di fondo negli attivi»;
- `test/prove_scenario_missione.cpp`: «scenario: foto, zona inviolata, unita' da raggiungere, danni iniziali e sonar
  speciale»;
- `test/prove_libreria_missioni.cpp`: «libreria S05 «Il guardiano del cavo»: il ping trova l'intruso, che fugge allo
  stretto; la nave persa chiude male» e «libreria S03 «Quarantena»: il Foxtrot pingato senza tregua emerge per forza
  (Enhanced); nel classico a 2 ore e mezza»;
- `test/prove_missioni.cpp`: «rapide v2: ping del giocatore vicino a un'unita', lancio di un'unita' ostile»;
- `test/prove_registro.cpp`: «registro: scoperte, contro-scoperta, esposizione e obiettivi della missione»;
- `test/prove_sonar_giocatore.cpp`: «sonar: filtro, impulso attivo e ANALYZE».

Intercettazione (§2.10):
- `test/prove_intercettazione.cpp` (`intercettazione:`): il catalogo dei ricevitori e delle emissioni; il sonar di una
  fregata a 20 km misurato e classificato, senza allarme; una testa che arriva dà SILURO IN ACQUA da oltre 3 km e
  l'acquisizione fra 300 e 1600 m, una che se ne va no; lo Skat misura CW e FM, il Rubin no e ha i falsi allarmi;
  ripetibile dal seme.
- `test/prove_intercettazione_missione.cpp`: il 688 intercetta i ping del Victor e il Victor quelli del 688; senza
  l'acustica (il classico) nessun intercettatore.
- `test/prove_pagina_intercettazione.cpp`: la fregata sulla cascata in B1, la riga della traccia, la scelta e la
  finestra; la testa in acquisizione accende la lampada rossa; il Rubin con i «--», e senza ricevitore la pagina lo
  dice.

## 7. Limiti noti e in arrivo

- L'eco non fluttua: con SE ≥ 0 il contatto c'è sempre, con SE < 0 mai. Il dossier 02 §1.6 suggerisce una
  fluttuazione di 10-60 s anche per l'attivo. Lo stesso vale per l'intercettazione.
- Il fascio dell'attivo è tutto l'orizzonte fuori dal cono d'ombra: non si punta un settore e non c'è una scala delle
  distanze. Il tempo di andata e ritorno non c'è: l'eco arriva nello stesso passo.
- Il contatto del ping (§2.8) ha solo il rilevamento. La classe dell'emettitore la dice la pagina
  dell'intercettazione (§2.10), ma non passa al contatto né alla carta. Il dossier 02 §5.3 la chiede nel contatto.
- Nell'eco il Doppler è un gradino a 2,5 nodi, e la forma d'onda (CW o FM) c'è solo come `risoluzione_s` dei sonar
  speciali e delle boe. L'intercettazione invece misura il Doppler e, con lo Skat, CW o FM.
- Le boe attive e i sonar a immersione dell'ASW aereo non passano ancora dal ricevitore d'intercettazione: il
  catalogo ha le loro emissioni, ma la missione non le emette. Si sentono solo come ping (§2.8, capitolo E5).
- Il ricevitore lega e classifica, ma l'IA usa solo l'allarme siluro: non evita i sonar che intercetta.
- La riverberazione di fondo non cambia col tipo di fondo (−27 dB anche sul fango, che il dossier dà a −35/−40 dB).
  Non c'è lo strato diffondente profondo né la sua migrazione fra giorno e notte.
- La stessa soglia vale contro il rumore e contro il riverbero; il dossier 02 §1.3 dà 10-15 dB contro il clutter.
- Il BQS-15 usa portate fisse, non l'equazione del sonar: non sente il mare, il fondale o il tipo di mina. Le sue
  spazzate non sono eventi acustici: non alzano il rumore del 688 e non contano come ping nel registro, nelle
  statistiche e nell'achievement «fantasma».
- Il BQS-15 dell'IA non serve all'IA: le scoperte le tiene solo il giocatore.
- L'ecoscandaglio e i ritorni del BQS-15 sul fondo e sulle pareti ci sono, nel quadro del fondale del giocatore
  (capitolo C12 §2.11); il ping dell'ecoscandaglio si intercetta (emissione `fathometer`). Il sondatore verso l'alto e
  le chiglie del ghiaccio ci sono dal 29/10/2026 (capitolo E11): i ping del sondatore vanno agli intercettatori come il
  fathometer.
- **Le chiglie al ping dell'IA** (31/10/2026, la simmetria dei sensori, punto 6): anche il ping attivo di una nave o di
  un sottomarino dell'IA prende i falsi echi delle chiglie (`echi_chiglie_ping`), come quello del giocatore.

## 8. Riferimenti

- Codice: `nucleo/acustica.hpp`, `nucleo/src/acustica.cpp` (`valuta`, `forza_bersaglio`, `forza_superficie_db`,
  `soglia`, `larghezza_fascio_deg`; costanti `SV_VOLUME_DB`, `SB_FONDO_DB`, `SS_MASSIMA_DB`,
  `DOPPLER_RIVERBERAZIONE_KN`, `DOPPLER_GUADAGNO_DB`, `QUOTA_MIN_RIVERBERAZIONE_M`, `LOBO_VERTICALE_MAX_DB`);
  `missioni/src/missione.cpp` (`impulso_sonar_enhanced`, `intercetta_impulso`, `sensori_acustici`,
  `sorgente_acustica`, `evento_acustico`); `missioni/src/missione_bqs15.cpp`; `missioni/src/missione_reazioni.cpp`
  (`PING_OGNI_S`, `PING_ALLERTA`); `missioni/src/scenario_missione.cpp` (`PING`, `sonar_speciale`);
  `app/interfaccia.cpp` (pulsante dell'impulso, `bqs15`, `intercettazione`, la targa dell'allarme); `app/carta.cpp`
  (mine scoperte).
- Intercettazione: `nucleo/intercettazione.hpp`, `nucleo/src/intercettazione.cpp`,
  `missioni/src/missione_intercettazione.cpp` (`emetti_impulso`, `passo_intercettazione`), `app/pagina_intercettazione`.
- Dati: `data/motore/acustica.toml` `[sensori.*]`, `[firme.*]`, `[suite]` (da `tools/pipeline/acustica.py`);
  `[intercettatori.*]`, `[emissioni.*]`; `data/motore/tasti_predefiniti.toml` (`bqs15`); `data/motore/testi/messaggi_bordo.tsv`
  (8, 311, 312, 450-452, 475).
- Ricerca: dossier 02 §1.2-1.6 e §5.3; dossier 06 §1.1, §1.5 e §7.1; dossier 24 §5.1 e §8.1; dossier 28 §1-§3.
- Progetto: `docs/22_sonar_blocco2.md` §2.
- Progetto: `docs/09_schema_missioni.md` §4 (`sonar_speciale`); `docs/08_fondali_e_navigazione.md`;
  `docs/17_libreria_missioni.md` (S06, I09); `docs/00_ROADMAP.md` punto 26.
- Originale: `docs/originale/sensori_e_navigazione.md` §4; `docs/originale/numeri_eseguibile.md` §3.6.
- Volume 1: `1_uso/03_sonar.md` §3.8; `1_uso/05_carta_e_tma.md` §5.1; `1_uso/08_nemico.md` §8.2 e §8.4.
