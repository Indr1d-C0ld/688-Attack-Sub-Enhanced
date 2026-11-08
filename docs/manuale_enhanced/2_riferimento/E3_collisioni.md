# E3. Collisioni

*Autori: kit (danni), runtime (rilevazione nella missione). Revisione del 29/10/2026 (gli effetti nella missione).
Stato: rivisto (curatore: kit).*

## 1. A che serve

Durante i pedinamenti della Guerra fredda i sottomarini si sono urtati più volte, fra loro e con le navi. Questo
capitolo spiega come il gioco decide se due unità si toccano, quanta energia si scambiano, quale parte di ciascuno
scafo la assorbe e che danni ne vengono: dalle piastrelle strappate al duomo perso, dalla vela piegata allo scafo
resistente aperto. Vale solo nell'Enhanced.

## 2. Il modello

Due parti, con un contratto fra loro:
- **la rilevazione** (runtime, `missioni/src/missione_collisioni.cpp`): le forme delle unità, il contatto, l'urto con
  la sola geometria del mondo;
- **i danni** (kit, `nucleo/collisioni.hpp`, `valuta_urto`): dall'urto alle energie, alla parte colpita, ai danni di
  ciascuna unità, che applicano `BattelloDanni::urto_meccanico` (capitolo B3), `DanniNave::urto_meccanico` (capitolo
  E2) e `DanniSottomarini::collisione`.

### 2.1 Le forme e il contatto

- **Sottomarino**: una scatola lunga quanto lo scafo e alta quanto il diametro, con la chiglia alla quota vera, più la
  vela nella sua posizione e con la sua altezza (`vela_x_m`, `altezza_vela_m` di `classi.toml`, misurate sui modelli
  3D), larga 3,5 m. Senza i dati la vela è lunga il 15% dello scafo, centrata al 20% verso prua, alta 0,6 diametri.
  A quota periscopica la quota della missione (6,1 m del classico) si traduce nella quota vera della classe (18,3 m sul
  688), altrimenti la vela uscirebbe dall'acqua; fra la superficie e la quota periscopica si interpola dal pescaggio.
- **Nave**: un prisma lungo e largo quanto lo scafo, da 8 m sopra l'acqua al pescaggio (duomo del sonar compreso).
- **Contatto**: test degli assi separatori nel piano più la sovrapposizione delle quote, fra scafi e vele. La normale
  è l'asse della sovrapposizione minore: se è quella in quota, l'urto è verticale (normale in su o in giù). Solo coppie
  con almeno un sottomarino, entro 3 km; esclusi aerei, armi, esche e biologici.
- **Un urto per contatto**: un altro dopo 2 s di contatto continuo; nessuno nel primo secondo (unità che partono già
  sovrapposte). L'urtante è quello che va verso l'altro più in fretta.
- **Masse**: il dislocamento di `scafi/<classe>.toml` (in emersione o in immersione per i sottomarini, quello pubblicato
  per le navi); senza scafo, stimato dalle dimensioni.

### 2.2 L'energia

La velocità relativa si divide in una componente lungo la normale al contatto (`v_n`) e una sul piano (`v_t`):

```
m'   = m · (1 + k)                         (massa con la massa aggiunta nella direzione della normale)
μ    = m₁' m₂' / (m₁' + m₂')
E_n  = ½ μ v_n²                            (energia che deforma)
E_t  = 0,3 · ½ μ v_t²                      (strisciamento: graffia, non sfonda)
```

Masse aggiunte `k`: sottomarino 0,02 assiale e 0,95 trasversale (di fianco e in verticale); nave 0,075
longitudinale, 0,4 laterale, 1 verticale. Contro un ostacolo fisso (oggi solo nelle prove, come il monte
del San Francisco) `μ` è la massa di chi urta. Un urto è **radente**
sotto 25° fra la velocità relativa e il piano di contatto.

### 2.3 Dove va l'energia

L'energia normale si divide fra i due scafi secondo la rigidezza della parte colpita: metà ciascuno a pari rigidezza,
altrimenti il 70% al più cedevole. Contro un ostacolo fisso la prende tutta l'unità. Le rigidezze:
- sottomarino: la vela media (alta quella da ghiaccio del Typhoon), il resto bassa;
- nave: la prua alta, il fianco e la chiglia media (bassa sotto le 3000 t: struttura leggera), il resto bassa.

In ciascuno scafo l'energia passa verso l'interno, parte dopo parte, finché non è assorbita:

| Parte | Capacità (**STIMA**) |
|---|---|
| duomo, timoni, eliche, alberi | 2 MJ |
| casing e casse | 12 MJ × D/10 (× 1,5 sui doppi scafi) |
| vela | 10 MJ (50 la vela da ghiaccio del Typhoon); ne passa allo scafo il 30% di quel che avanza |
| zona prodiera del sottomarino | 1 GJ per D = 10 m, × (D/10)² (taratura sul San Francisco) |
| scafo resistente | 40 MJ × D/10 × max(0,25; 1 − 0,5 h / h_collaudo) |
| lamiera della nave sotto il galleggiamento | 1,5 MJ × (Δ / 1000 t)^(2/3); la prua rinforzata × 3 |

`D` è il diametro dello scafo in metri, `h` la quota, `Δ` il dislocamento. Ogni capacità varia di ±10% col seme. Le
**parti a lama** (la vela, gli alberi, i timoni e le eliche di un sottomarino; la prua, i timoni e le eliche di una
nave; l'ostacolo fisso) concentrano l'urto: la lamiera dell'altro regge 0,3 volte. Sulla vela no: la vela distribuisce
il carico sullo scafo resistente.

### 2.4 La falla

Il volume d'acciaio deformato dà la falla (Minorsky, più una regola per l'energia oltre la capacità):

```
R_T = max((E − 32,7) / 47,2,  (E − capacità) / 80)   [m³ d'acciaio, E in MJ]
area = R_T / spessore
```

Lo spessore è 35 mm per lo scafo resistente, `8 mm + 0,5 mm · Δ^(1/3)` (Δ in t) per la lamiera di una nave. Sotto la
capacità non c'è falla. Una lama che striscia a più di 1 m/s apre uno **squarcio** lungo `v_t × 1 s` (fra 2 m e il 40%
dello scafo), che può attraversare più compartimenti (Ehime Maru).

### 2.5 I danni a ciascuna unità

`DanniUrtoUnita`:
- **sottomarini**:
  - perdite dei sistemi della parte colpita: sonar di prua e di tiro col duomo e con la prua; periscopio e antenna con
    la vela (molto di più con gli alberi alzati) e con gli alberi; i timoni della vela sul 688 e sullo Sturgeon; timone
    e timoni orizzontali coi timoni; propulsione con le eliche e con la poppa;
  - l'antenna rimorchiata filata, persa se l'urto è a poppa, sulle eliche o sui timoni;
  - falla nello scafo resistente nel compartimento al punto, se l'energia arriva oltre il casing; casse e casing che
    non tengono più l'aria (oltre 0,3 m² di falla);
  - ammaccatura `a` fra 0,2 e 0,8 appena l'energia arriva allo scafo resistente: la quota di collasso scende di
    `0,5 · a` (capitolo B3 §2.6);
  - l'elica dell'altro incide lo scafo resistente oltre i 5 MJ: falla da 0,01 a 0,1 m² (Sceptre, 1981);
  - piastrelle anecoiche strappate, `20 · E_t + 2 · E_n` m² fino a 300, con 1 dB di rumore in più ogni 40 m² (al più
    6);
  - un'elica colpita: `g = clamp(E / 3, 0,2; 0,7)`, propulsione persa per `g`, rumore `3 + 14 (g − 0,2)` dB;
  - in superficie anche la falla di una nave vicino al galleggiamento, che vale per lo scafo resistente;
- **navi**: falla sotto il galleggiamento nelle celle sul fianco lungo lo squarcio (o nella cella al punto), duomo
  perso (oltre 0,5 MJ), eliche danneggiate (`g = clamp(E / 4, 0,2; 0,6)`, rumore `3 + 12,5 (g − 0,2)` dB), timone
  bloccato (oltre 0,3 MJ), sovrastrutture danneggiate (oltre 0,05 MJ);
- **per tutti**: velocità massima ridotta (un'elica piegata: `1 − g`), **scossa** a bordo (`Δv / (0,5 s · g)`),
  rollio provocato;
- **navi senza scafo** per i danni: perdono integrità per `falla/20 m² + 0,05 duomo + 0,1 eliche + 0,05 timone + 0,05
  sovrastrutture`.

Nella missione (`DanniSottomarini::collisione`, `DanniMissione`) i danni passano a `BattelloDanni::urto_meccanico` o,
per un battello emerso danneggiato e per le navi, a `DanniNave::urto_meccanico`. Poi gli effetti:
- **il rumore** in più di un sottomarino si somma urto dopo urto fino a +12 dB (`DanniSottomarini::rumore_urto_db`) e
  entra nella sua sorgente della scena acustica (`urto_db`): dopo lo speronamento il battello si sente di più;
- **il duomo perso** di una nave le toglie i sonar di scafo (`Missione::perdi_sonar_scafo`): restano la schiera
  rimorchiata e il VDS;
- **l'elica piegata** limita per sempre la velocità massima a `1 − g` della massima (`NaveColpita::velocita_urto`, o
  l'ordine del motore per le navi senza scafo);
- **il timone bloccato** non lascia più accostare la nave.

### 2.6 Il suono

- **Il colpo**, a `170 dB re 1 µPa a 1 m` per 1 MJ, più 10 dB per decade d'energia: sordo (10-3000 Hz, lo scafo che
  risuona), o squillante (50-8000 Hz) se c'entra una lama o un'appendice.
- **Lo stridio**: l'energia tangente sulla durata dello strisciamento, 160 dB per 1 MJ in 1 s, fra 130 e 180.
- **Le pale**: se c'entra un'elica, i colpi delle pale per 3 s, 20 dB sotto il colpo.
- **A bordo** (`AmbienteBordo::urto`, `nucleo/bordo_audio.cpp`): lo schianto per via strutturale (115 dB a 1 MJ
  assorbito, fra 90 e 135), lo stridio, lo scossone; l'urto fra altri arriva dall'acqua, attenuato con la distanza e
  dallo scafo, e non si sente in plancia. L'interfaccia (`app/interfaccia.cpp`) lo chiama per ogni urto nuovo: lo
  schianto, e lo stridio se striscia, quando c'entra il battello; altrimenti dall'acqua, alla distanza dell'urto. Solo
  nell'Enhanced, e solo a compressione del tempo fino a 8×.

Il colpo, lo stridio e le pale sono transitori della scena acustica (`acustica_urto.hpp`; voci `urto_colpo`,
`urto_lamiera`, `urto_stridio`, `urto_elica` di `acustica.toml`), al punto e alla quota del contatto. Un urto a 10 nodi
fra un 688 e un Victor III si sente alla sfera di un battello a 10 km. Dopo l'urto le due unità si conoscono:
contatto reciproco con la posizione.

### 2.7 Avvisi, quasi collisioni, anticollisione e Crazy Ivan

- **Avvisi dell'equipaggio** (`Partita::allarmi_collisione`, solo Enhanced), sui contatti del giocatore che possono
  incrociarne la quota (una nave se siamo entro 30 m sotto il suo pescaggio, un sottomarino entro 60 m di quota):
  - 431 rilevamento costante: il contatto passerà entro 300 m fra meno di 3 minuti, ed è entro 3 km;
  - 432 allarme collisione: il contatto è entro 500 m.
- **Quasi collisione** (solo per il giocatore): passaggio entro 200 m fra immersi, o entro 500 m a quota periscopica
  con una nave, senza urto. Si registra al minimo della distanza, con le quote, chi sapeva dell'altro e la qualità del
  contatto, quando ci si allontana oltre 1,2 volte la soglia. Messaggio 434 se l'altro era in contatto, 433 se no.
- **Anticollisione dell'IA** (`missioni/src/missione_difesa.cpp`): un sottomarino dell'IA che ha in contatto un'unità
  entro 1000 m e 30 m di quota, con un passaggio previsto sotto i 150 m, cambia quota di 40 m (sotto se può) e accosta
  via per 2 minuti.
- **Immersione d'emergenza** (messaggio 430): la quota di sicurezza è almeno 150 piedi (45,7 m) e porta la cima della
  vela 10 m sotto la chiglia delle navi entro 5 km, senza scendere oltre la quota massima meno 10 m né a meno di 15 m
  dal fondo.
- **I cavi delle navi**: le navi che filano una schiera (capitolo C6 §2.10: la SQR-19 di Perry, Spruance e
  Ticonderoga, la SQR-18A della Knox dietro il pesce del VDS, la 2031Z della Type 22 Batch 2) trainano il cavo, che segue
  la scia. Dal 30/10/2026, con l'acustica, la quota lungo il cavo è la forma vera del modello della schiera
  (`Missione::profilo_cavo`); senza, quella fissa della SQR-19 (1700 m di cavo e 242 m di schiera, fino a 360 m). Il cavo entro 300 m fa scattare l'avviso 435. Se il cavo passa entro mezzo diametro più 1 m
  dal mozzo dell'elica, il battello s'impiglia (436): propulsione −85%, al più 2 nodi; la nave perde la schiera e ci
  scopre. Se tocca solo lo scafo, il messaggio è 437 (capitolo C6 §2.7).
- **Crazy Ivan**: i sottomarini sovietici dell'IA, immersi oltre 30 m, sopra 3 nodi, non agganciati e non in evasione,
  ogni 30-120 minuti e 5-15 minuti dopo un'accostata di oltre 30°, virano di 120-180° con 2 nodi in più, nel 30% dei
  casi cambiando quota di 20-40 m, ascoltano per 2-5 minuti a 4 nodi e tornano in navigazione.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Energia d'urto con le masse aggiunte | dossier 16 §2.1 | alta (meccanica) |
| Masse aggiunte | dossier 16 §2.1 | media |
| Minorsky | Minorsky 1959, dossier 16 §2.3 | media (navi), bassa per i sottomarini |
| Capacità delle parti | tarate sui casi storici | **STIMA** |
| Lama × 0,3 | | **STIMA** |
| Livelli acustici | dossier 16 §2.6 | **STIMA** |
| Soglie delle quasi collisioni, immersione d'emergenza, anticollisione | dossier 16 §4.3-4.6 | media |
| Cavo della SQR-19 | dossier 04 §2.2 e §3 | media; l'impiglio **STIMA** |
| Crazy Ivan | dossier 03 §C4 | media |

## 4. Classico ed Enhanced

Solo Enhanced: `Missione::collisioni_enhanced` parte solo con la regola di simulazione. Nel classico le unità si
attraversano come nel 1989.

## 5. Come si vede in gioco

- la scossa (la vista che sobbalza) e lo scossone a bordo;
- lo schianto e lo stridio a bordo quando urta il battello; l'urto fra altre unità si sente dall'acqua, più debole con
  la distanza;
- al periscopio la vista s'inclina col rollio provocato dall'urto (`rollio_gradi`, sopra mezzo grado) e torna:
  un'oscillazione di 7 s smorzata in circa 10 s, finita entro 30 s (`InterfacciaMissione::rollio_attuale`, **STIMA**).
  Solo nell'Enhanced;
- gli avvisi 430-437 (§2.7) e i rapporti dell'equipaggio sui danni (capitolo B3), con la stazione Stato;
- al sonar il colpo, lo stridio e le pale, anche dell'urto fra altri;
- nel registro e nel debriefing (volume 1, cap. 10 §10.2) gli eventi della collisione (radente o no, con l'energia),
  della quasi collisione, dell'urto fra due unità dell'IA, del cavo impigliato o toccato, e la manovra
  d'anticollisione dell'IA (`missioni/src/registro_missione.cpp`); fra i riconoscimenti conta la quasi collisione con
  un'unità che non avevamo in contatto.

Il volume 1 ne parla al cap. 8 §8.5 e, per l'immersione d'emergenza, al cap. 2.

## 6. Taratura e prove

`test/prove_collisioni.cpp`, sui casi storici del dossier 16, ognuno con la geometria e le velocità ricostruite:

| Caso | Che cosa controlla |
|---|---|
| Grayling e Delta IV (1993) | radente di prua |
| Tautog e K-108 (1970) | il Victor che scende sulla vela |
| Baton Rouge e Kostroma (1992) | il Sierra che risale sotto la poppa |
| Vanguard e Triomphant (2009) | due SSBN lenti e obliqui |
| Sceptre e K-211 (1981) | di prua sulle eliche di un SSBN fermo |
| Greeneville ed Ehime Maru (2001) | l'emersione d'emergenza sotto una nave piccola, lo squarcio |
| Hartford e New Orleans (2009) | preso al traverso a quota periscopica |
| S-178 e Refrizherator-13 (1981) | in superficie a 25° |
| San Francisco (2005) | contro un monte sottomarino a 33 nodi |
| Mar Nero (1988) | l'urto voluto, radente, sopra il galleggiamento |

Più le energie del dossier §2.1, il determinismo, i pescherecci che galleggiano e che la vela di un 688 allaga
(«collisioni: energie del dossier 16 §2.1», «collisioni: deterministico; i danni arrivano ai modelli», «collisioni: i
pescherecci nuovi (agi, peschereccio) galleggiano; la vela di un 688 li allaga»).

Altre prove:
- `test/prove_collisioni_missione.cpp`: speronamento, quasi collisione, classico che si attraversa, Crazy Ivan,
  immersione d'emergenza sotto le chiglie, il cavo della schiera della Perry nell'elica del 688 («S10: …»); «dopo lo
  speronamento il Victor fa piu' rumore»; «la nave col duomo perso non sente piu', col timone bloccato non accosta»;
- `test/prove_acustica_urto.cpp`: «urto: le sorgenti della scena», «urto: due sottomarini che si toccano a 10 nodi si
  sentono a 10 km», «urto: a bordo, il nostro scafo e quello degli altri»;
- `test/prove_difesa.cpp`: «difesa: due sottomarini in rotta di collisione, l'IA cambia quota e accosta».

## 7. Limiti noti e in arrivo

- Le navi non si urtano fra loro; i siluri non hanno forma. L'urto fra una nave e un sottomarino dell'IA invece c'è
  (la fregata della S16 che investe l'Alfa, 30/10/2026), con le stesse regole di quello col giocatore.
- La vela contro le chiglie del ghiaccio usa questo modello, con la chiglia come ostacolo fisso (capitolo E11 §2.6).
- Alcuni risultati di `valuta_urto` non hanno ancora effetto nella missione:
  - il rollio provocato non muove il battello: inclina solo la vista al periscopio (§5);
  - il rumore in più delle eliche danneggiate di una nave resta nel registro dei danni: la sua firma non cambia.
- Le reti dei pescherecci a strascico sono una sorgente acustica e prendono l'antenna rimorchiata (capitolo C6), ma
  non il battello.
- **In arrivo** (runtime e principale): l'urto voluto in tempo di pace, le conseguenze per la carriera e l'incidente
  politico, le reti dei pescherecci, l'urto fra navi.

## 8. Riferimenti

- Codice: `nucleo/collisioni.hpp`, `nucleo/src/collisioni.cpp`, `nucleo/acustica_urto.hpp`,
  `nucleo/src/acustica_urto.cpp`, `nucleo/src/bordo_audio.cpp` (`AmbienteBordo::urto`),
  `missioni/src/missione_collisioni.cpp` (`passo_collisioni`, `passo_cavi`, `chiglia_vera`,
  `quota_immersione_emergenza`), `missioni/src/missione_difesa.cpp` (anticollisione dell'IA),
  `missioni/src/danni_sottomarini.cpp` e `missioni/src/danni_missione.cpp` (i danni applicati),
  `missioni/src/registro_missione.cpp` (gli eventi), `app/partita.cpp` (`allarmi_collisione`).
- Dati: `data/motore/classi.toml` (forme e vele), `data/motore/scafi/<classe>.toml` (masse, collaudo, doppio scafo),
  `data/motore/acustica.toml` (`[transitori.urto_*]`), `data/motore/testi/messaggi_bordo.tsv` (430-437).
- Ricerca: dossier 16 (collisioni), dossier 03 §C4 (Crazy Ivan), dossier 04 (la SQR-19).
- Volume 1: cap. 8 §8.5. Volume 2: capitoli B3, C6, E2, E6.
