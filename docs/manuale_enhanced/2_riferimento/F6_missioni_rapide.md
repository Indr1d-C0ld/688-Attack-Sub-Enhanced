# F6. Missioni rapide

*Autore: trailer-1. Revisione del 29/10/2026 (il codice partita v2 e l'opzione GPS «1990+»). Stato: rivisto.*

## 1. A che serve

La missione rapida è una missione nuova generata in un istante da pochi parametri: teatro, tipo, battello, forze,
difficoltà. Il generatore è deterministico: lo stesso **codice missione** di 13 caratteri dà sempre la stessa
missione, byte per byte. Due giocatori che si scambiano il codice giocano la stessa operazione. Il capitolo spiega come
il generatore sceglie luoghi, forze, obiettivi e testi, e che cosa cambiano i parametri.

## 2. Il modello

Il codice è in `missioni/missione_rapida.hpp` e `missioni/src/missione_rapida.cpp`. I dati sono in
`data/motore/missioni_rapide.toml` e nei kit del giocatore `data/motore/missioni_rapide/kit_usa.lua` e `kit_urss.lua`.
Il generatore non usa lo stato del motore: solo i dati, il fondale del teatro e il generatore casuale di
`nucleo/casuale.hpp`.

### 2.1 I parametri

| Parametro | Valori | Effetto |
|---|---|---|
| teatro | Fær Øer, Barents, Celtica, Gibilterra, Otranto, Kunashir, Baltico, o a caso | la carta, i luoghi, le forze, i tipi possibili |
| stagione | inverno, primavera, estate, autunno, o a caso | il mese (tre per stagione) |
| momento | alba, giorno, tramonto, notte, o a caso | l'ora, dal Sole sul centro del teatro |
| meteo | semplificato o realistico | lo applica il gioco (`Missione::usa_meteo`) |
| tipo | pattuglia, caccia, antinave, scorta, ricognizione, minamento, inserimento, o a caso | lo schema della missione (§2.5) |
| battello | 688 (parte USA) o Alfa (parte URSS) | il giocatore e la parte avversaria |
| forze | 1 poche, 2 medie, 3 molte, o a caso | quante unità avversarie e quanti aerei |
| paranoia | 0 distratta, 1 normale, 2 all'erta, 3 paranoica | l'IA; con 2 o più il gruppo antinave zigzaga |
| difficoltà | 1-5, o a caso | distanze, durate, unità in più, sorprese, informazioni |
| ASW aereo | sì o no | aerei ed elicotteri avversari (Enhanced) |
| carriera | sì o no | la richiesta di contare per il fascicolo |
| seme | 32 bit | tutto il resto |

### 2.2 Il codice missione

Il codice tiene 65 bit in 13 caratteri dell'alfabeto di Crockford in base 32 (`0-9`, `A-Z` senza I, L, O, U). Si
scrive in tre gruppi: `4G0J-BG00-03WB8`.

| Campo | Bit | Note |
|---|---|---|
| versione | 3 | oggi 2 (`VERSIONE_RAPIDE`); la 1 si legge ancora |
| teatro | 3 | 0 a caso, 1-7 nell'ordine della tabella sopra |
| stagione | 3 | 0 a caso, 1-4 |
| momento | 3 | 0 a caso, 1-4 |
| meteo realistico | 1 | |
| tipo | 4 | 0 a caso, 1-7 |
| battello | 1 | 1 = Alfa |
| forze | 2 | 0 a caso, 1-3 |
| paranoia | 2 | 0-3 |
| difficoltà | 3 | 0 a caso, 1-5 |
| ASW aereo | 1 | |
| carriera | 1 | |
| seme | 32 | |
| controllo | 6 | CRC-6 sui 59 bit precedenti |

**Il controllo.** È un CRC-6 col polinomio primitivo `x⁶ + x + 1`. Scopre sempre un carattere sbagliato (un errore di
5 bit) e lo scambio di due caratteri vicini: l'errore è `d(x)(x⁵ + 1)`, che un polinomio primitivo di grado 6 non
divide.

**La lettura** ignora maiuscole, minuscole, spazi e trattini. La O vale zero, la I e la L valgono uno. I codici della
versione 1 si leggono ancora e rigenerano lo stesso file di allora, byte per byte; un codice di una versione che non
c'è si rifiuta.

**Le versioni.**
- 1 (25/10/2026): la prima.
- 2 (29/10/2026): `R_nave` non va più a caccia antisom di una nave di superficie nemica. Con la Kidd solida al radar,
  la Kara faceva ping a ogni giro e illuminava il sottomarino del giocatore. Con gli stessi parametri la missione è la
  stessa della versione 1: il caso si semina coi parametri senza la versione, e cambia solo lo script di `R_nave`.

Il codice dice i parametri **richiesti**, coi campi «a caso». La missione li risolve dal seme (§2.3), quindi non serve
scriverli nel codice.

### 2.3 Dal codice ai parametri risolti

I 59 bit dei dati, letti come numero, sono l'**impronta** dei parametri. Da lì vengono due generatori casuali:

```
r_scelte    = Casuale(splitmix64(impronta XOR 0x7261706964615f31))
r_missione  = Casuale(splitmix64(impronta XOR splitmix64(seme)))
```

- `splitmix64` mescola i bit (la funzione di Vigna); la costante è «rapida_1» in ASCII.
- `r_scelte` risolve i campi a caso, sempre nello stesso ordine e consumando un numero anche se il campo è già
  scelto: teatro, tipo, poi stagione, momento, forze, difficoltà. Così un campo fissato non cambia gli altri.
- `r_missione` genera la missione.

**Teatro a caso.** Si sceglie fra i teatri dove il tipo si gioca con quella parte, col peso del tipo; se anche il tipo
è a caso, ogni teatro con almeno un tipo possibile vale 1. **Tipo a caso**: col peso del tipo nel teatro e per la
parte. Un peso 0 vuol dire che quel tipo lì non si gioca: con un teatro e un tipo fissati e incompatibili la
generazione si rifiuta.

<!-- generato: rapide_tipi_teatri -->
| Teatro | Battello | Pattuglia | Caccia | Antinave | Scorta | Ricognizione | Minamento | Inserimento |
|---|---|---|---|---|---|---|---|---|
| Fær Øer e Mar di Norvegia | 688 | 4 | 4 | 3 | 3 | — | — | — |
| Fær Øer e Mar di Norvegia | Alfa | 3 | 3 | 4 | 1 | 2 | 2 | 1 |
| Mare di Barents, approcci a Kola | 688 | 4 | 5 | 1 | — | 3 | 1 | 1 |
| Mare di Barents, approcci a Kola | Alfa | 4 | 4 | — | 4 | — | — | — |
| Margine della Piattaforma Celtica | 688 | 3 | 4 | 4 | 4 | — | — | — |
| Margine della Piattaforma Celtica | Alfa | 3 | 3 | 5 | — | — | — | — |
| Stretto di Gibilterra | 688 | 4 | 3 | 3 | 3 | 2 | — | — |
| Stretto di Gibilterra | Alfa | 2 | 2 | 4 | — | 3 | 3 | 1 |
| Canale d'Otranto | 688 | 4 | 3 | 3 | 3 | 2 | — | — |
| Canale d'Otranto | Alfa | 2 | 2 | 4 | — | 3 | 3 | 1 |
| Passo di Kunashir e Mare di Okhotsk | 688 | 3 | 4 | 2 | 1 | 4 | 1 | 2 |
| Passo di Kunashir e Mare di Okhotsk | Alfa | 3 | 4 | 2 | 3 | 1 | — | — |
| Mar Baltico sud-occidentale | 688 | 3 | 2 | 3 | — | 3 | 4 | 3 |
| Mar Baltico sud-occidentale | Alfa | 2 | 2 | 3 | 2 | 3 | 4 | 2 |
<!-- fine: rapide_tipi_teatri -->

### 2.4 I passi della generazione

1. **Data e ora.** Anno 1989; il mese fra i tre della stagione, il giorno da 1 a 28. L'ora viene dal Sole sul centro
   del teatro (`nucleo/sole.hpp`): l'alba e il tramonto dall'elevazione che passa per zero, cercata ogni 5 minuti.
   - alba: da mezz'ora prima a un quarto d'ora dopo;
   - tramonto: da un quarto d'ora prima a mezz'ora dopo;
   - giorno: mezzogiorno vero ± 3 ore;
   - notte: mezzanotte vera ± 2,5 ore.
2. **Nome dell'operazione**, da due liste di 24: inglesi per la US Navy (GRAY WIND, COLD HARBOR…), russi traslitterati
   per la VMF (TUMAN, BURJA…).
3. **Il giocatore**: un Los Angeles o un Alfa, a 5 nodi.
4. **Lo schema del tipo** (§2.5): situazione, zona, forze, obiettivi, durata, condizione di fine.
5. **Gli obiettivi comuni**: «Riportare a casa il battello» (sopravvivere) si aggiunge sempre.
6. **I testi**: briefing e obiettivi dai modelli del file di dati, coi segnaposto (`{zona}`, `{forze}`, `{rotta}`…)
   riempiti in italiano e in inglese.
7. **Lo scenario e il file.** Il generatore compila uno scenario dell'editor (capitolo F7) con la libreria dei
   comportamenti 1, e lo scrittore lo trasforma nel file Lua del formato 1. La missione si salva in
   `missioni_rapide/RAPIDA_<codice senza trattini>.lua`, accanto ai fascicoli.

**I luoghi.** Ogni teatro ha una lista di luoghi veri, con coordinate e tipo (base navale, aeroporto, porto, stretto,
mare) e parte (NATO, URSS, neutrale). Il generatore li proietta sulla carta e cerca il punto d'acqua utile più vicino:
prima a caso in un cerchio, poi in cerchi doppi, alla fine sulla griglia di 2 km. «Utile» vuol dire con abbastanza
fondale per chi ci deve andare e ad almeno 4 km dal bordo della carta.

**Quanta acqua serve:**

| Chi | Fondale minimo |
|---|---|
| sottomarini | 120 m (Baltico: 30 m) |
| navi | il pescaggio più 10 m, almeno 15 m |
| il giocatore alla partenza | 80 m (Baltico: 25 m), con la via libera fino alla zona |

**Le forze.** Le classi vengono dalle liste del teatro per ruolo e per parte. Solo classi di `classi.toml`.

<!-- generato: rapide_forze -->
| Teatro | Parte | Unità di pregio | Scorte | Sottomarini | Aerei | Elicotteri |
|---|---|---|---|---|---|---|
| Fær Øer e Mar di Norvegia | NATO | portaerei Nimitz, corazzata Iowa | cacciatorpediniere Spruance, cacciatorpediniere Kidd, fregata Perry | sottomarino Los Angeles, sottomarino Sturgeon, sottomarino Trafalgar | pattugliatore P-3C | elicottero SH-60B |
| Fær Øer e Mar di Norvegia | URSS | incrociatore Kirov, incrociatore Slava, incrociatore Kara | fregata Krivak II, incrociatore Kara | sottomarino Victor III, sottomarino Akula, sottomarino Charlie II, sottomarino Alfa | pattugliatore Tu-142, pattugliatore Il-38 | elicottero Ka-27 |
| Mare di Barents, approcci a Kola | NATO | portaerei Nimitz | cacciatorpediniere Spruance, fregata Perry | sottomarino Los Angeles, sottomarino Sturgeon, sottomarino Trafalgar | pattugliatore P-3C | elicottero SH-60B |
| Mare di Barents, approcci a Kola | URSS | incrociatore Kirov, incrociatore Slava | fregata Krivak II, incrociatore Kara | sottomarino Victor III, sottomarino Akula, sottomarino Alfa, sottomarino Oscar II, sottomarino Charlie II | pattugliatore Il-38, pattugliatore Tu-142, anfibio Be-12 | elicottero Ka-27, elicottero Ka-25 |
| Margine della Piattaforma Celtica | NATO | portaerei Nimitz, corazzata Iowa | cacciatorpediniere Spruance, cacciatorpediniere Kidd, fregata Perry | sottomarino Los Angeles, sottomarino Trafalgar | pattugliatore P-3C, aereo S-3B | elicottero SH-60B, elicottero SH-3H |
| Margine della Piattaforma Celtica | URSS | incrociatore Kirov, incrociatore Slava | fregata Krivak II, incrociatore Kara | sottomarino Victor III, sottomarino Akula, sottomarino Charlie II, sottomarino Oscar II | pattugliatore Tu-142 | elicottero Ka-27 |
| Stretto di Gibilterra | NATO | portaerei Nimitz, corazzata Iowa | cacciatorpediniere Spruance, cacciatorpediniere Kidd, fregata Perry | sottomarino Los Angeles, sottomarino Sturgeon | pattugliatore P-3C | elicottero SH-60B, elicottero SH-3H |
| Stretto di Gibilterra | URSS | incrociatore Kirov, incrociatore Slava, incrociatore Kara | fregata Krivak II, incrociatore Kara, fregata Koni | sottomarino Victor III, sottomarino Charlie II, sottomarino Foxtrot, sottomarino Kilo | pattugliatore Il-38 | elicottero Ka-27, elicottero Ka-25 |
| Canale d'Otranto | NATO | portaerei Nimitz, corazzata Iowa | cacciatorpediniere Spruance, cacciatorpediniere Kidd, fregata Perry | sottomarino Los Angeles, sottomarino Sturgeon | pattugliatore P-3C | elicottero SH-60B, elicottero SH-3H |
| Canale d'Otranto | URSS | incrociatore Kirov, incrociatore Slava, incrociatore Kara | fregata Krivak II, incrociatore Kara, fregata Koni | sottomarino Victor III, sottomarino Charlie II, sottomarino Foxtrot, sottomarino Kilo | pattugliatore Il-38 | elicottero Ka-25 |
| Passo di Kunashir e Mare di Okhotsk | NATO | portaerei Nimitz | cacciatorpediniere Spruance, cacciatorpediniere Kidd, fregata Perry | sottomarino Los Angeles, sottomarino Sturgeon | pattugliatore P-3C | elicottero SH-60B |
| Passo di Kunashir e Mare di Okhotsk | URSS | incrociatore Kirov, incrociatore Slava, incrociatore Kara | fregata Krivak II, incrociatore Kara | sottomarino Victor III, sottomarino Akula, sottomarino Charlie II, sottomarino Oscar II, sottomarino Kilo | pattugliatore Il-38, pattugliatore Tu-142, anfibio Be-12 | elicottero Ka-27, elicottero Ka-25 |
| Mar Baltico sud-occidentale | NATO | fregata Perry | fregata Perry | sottomarino Los Angeles | pattugliatore P-3C | elicottero SH-60B |
| Mar Baltico sud-occidentale | URSS | fregata Krivak II | fregata Koni, corvetta Nanuchka, fregata Krivak II | sottomarino Kilo, sottomarino Foxtrot | anfibio Be-12, pattugliatore Il-38 | elicottero Ka-27 |
<!-- fine: rapide_forze -->

**Gli elementi comuni a più tipi:**
- **Partenza.** Il giocatore parte a una distanza dalla zona che cresce con la difficoltà (±15%), con la prua sulla
  zona, a una quota fra 12 e 45 m.
- **Aerei ASW.** Con l'ASW aereo acceso: uno, due con forze molte. Il primo è un elicottero se c'è una nave che lo
  porta; gli altri partono da un aeroporto della loro parte. Girano su quattro punti attorno alla zona.
- **Neutrali.** Mercantili che attraversano la zona, da 0 a 2 secondo il tipo, a 9-13 nodi.
- **Sorpresa.** Con difficoltà 4 o 5 e forze medie o molte, un sottomarino avversario in più che il briefing non
  nomina.
- **Informazioni.** Il briefing dice quanto sono affidabili: sicure (difficoltà 1-2), buone ma incomplete (3),
  frammentarie (4-5).

### 2.5 Le sette tipologie

Nelle formule `f` sono le forze (1-3) e `d` la difficoltà (1-5); le distanze sono in miglia nautiche.

| Tipo | Situazione | Zona | Forze | Obiettivi | Durata | Fine |
|---|---|---|---|---|---|---|
| **Pattuglia** | pace o crisi (metà e metà) | un luogo di mare o stretto; settore di 6 nm | `1 + f` transiti (uno in più con `d` ≥ 4), 60% sottomarini, a scaglioni lungo la rotta | raggiungere il settore entro metà tempo; scoprire i transiti (uno di meno con `d` < 4); secondari: non farsi scoprire, non attaccare | `2,5 + 0,5 f` ore | quando tutti i transiti sono usciti |
| **Caccia** | guerra (60%) o crisi | mare o stretto, fondale ≥ 150 m | un sottomarino in pattuglia su quattro punti a 8-12 km; `f − 1` navi | guerra: affondarlo; crisi: scoprirlo senza farsi scoprire e senza attaccare | `4,5 − 0,3 d` ore | guerra: il sottomarino affondato; crisi: a tempo |
| **Antinave** | guerra | mare o stretto | un gruppo d'altura (pregio e `max(1, f + d/2 − 1)` scorte) o, nel 40% dei casi, un convoglio con `1 + f` mercantili; zigzag con paranoia ≥ 2 | affondare il pregio, o almeno `round(n · (0,4 + 0,1 d))` mercantili | il tempo del transito più 15 minuti, al massimo 6 ore | il bersaglio affondato o uscito |
| **Scorta** | guerra | mare o stretto | il gruppo amico (pregio, 1-2 scorte, la logistica con forze molte) fino a 12 nodi; `f` sottomarini in agguato lungo la rotta (uno in più con `d` ≥ 4) | proteggere il gruppo; secondario: affondare le minacce | come antinave | il gruppo uscito |
| **Ricognizione** | crisi | una base avversaria; il punto d'osservazione a 6 km verso il largo | `2 + f` unità ferme in rada, pattuglie | arrivare a 2 nm entro il 60% del tempo; scoprire le unità in rada (una di meno con `d` ≤ 2); non farsi scoprire; non attaccare | `4 − 0,2 d` ore | dentro il cerchio, poi fuori a 12 nm |
| **Minamento** | guerra | base, porto o stretto; un punto col fondale giusto per la mina | 1-3 pattuglie | posare almeno 4 mine (6 con `d` ≥ 3, 8 con `d` = 5) nel cerchio di 1500 m; secondario: non farsi scoprire | `4 − 0,2 d` ore | dentro il cerchio, poi fuori a 8 nm |
| **Inserimento** | crisi | la costa avversaria; lo sbarco su 15-40 m d'acqua, il più vicino possibile alla riva | 1-3 pattuglie costiere | sbarco nel cerchio di 1000 m entro il 60% del tempo, poi esfiltrazione a `12 + d` nm verso il largo; secondario: non farsi scoprire | `4 − 0,2 d` ore | sbarco fatto e battello nel cerchio d'esfiltrazione |

**Distanze di partenza** dalla zona: pattuglia `6 + 2d`, caccia `8 + 3d`, antinave `8 + d` (davanti al gruppo, di
lato alla rotta), scorta 4 nm davanti al gruppo, ricognizione `12 + d`, minamento `10 + d`, inserimento `16 + d`.

**Mine** (minamento), scelte dal fondale vicino al luogo:

| Parte | Mina | Fondale | Nel deposito |
|---|---|---|---|
| 688 | Mk 67 | 25-100 m | 10 |
| 688 | Mk 60 CAPTOR | 200-900 m | 6 |
| Alfa | MDT | 20-50 m | 20 |
| Alfa | PMR-2 | 200-1000 m | 6 |

**Carico.** Il 688 imbarca 22 Mk 48, 2 Harpoon e 6 esche; per l'antinave 18 Mk 48, 4 Harpoon, 4 Tomahawk antinave e
6 esche; per il minamento cede siluri alle mine. L'Alfa 18 siluri da 533 mm e 4 esche, o 4 siluri e le mine.
L'inserimento imbarca 6 ospiti (la squadra SEAL o gli incursori).

### 2.6 La carriera

Una rapida conta per il fascicolo solo se è richiesto, con difficoltà almeno 3 e paranoia almeno 1. Allora il file ha
il blocco `carriera`, con la situazione, l'avversario (impreparato, regolare, veterano o d'élite secondo la paranoia), la
difficoltà, l'operazione speciale (l'inserimento) e il teatro artico (Barents). Il gioco conta un codice una volta sola
per comandante (volume 1, cap. 10; capitolo F2).

### 2.7 Un esempio

Il codice `4G0J-BG00-03WB8` (seme 1989, Fær Øer, caccia, 688, forze medie, paranoia 1, difficoltà 3) dà l'«Operazione
COLD HARBOR»:
- 15 ottobre 1989, ore 0655Z;
- il SOSUS segnala un Charlie II presso la Dorsale Islanda-Fær Øer, 15 miglia per 342°;
- forze: il Charlie II e una Krivak II; aerei antisommergibile in zona;
- guerra: trovarlo e affondarlo; 3 ore e 36 minuti (`4,5 − 0,3 · 3`);
- fuori carriera, perché non era richiesta.

## 3. Fonti e confidenza

Le missioni rapide sono uno strumento di gioco, non un modello fisico: i numeri sono scelte di progetto.

| Voce | Fonte | Confidenza |
|---|---|---|
| Luoghi, basi, aeroporti dei teatri | carte nautiche e geografia reale | alta |
| Forze per teatro e parte | ordini di battaglia del 1989, dossier 01 (piattaforme) e 03 (teatri) | media |
| Pesi dei tipi per teatro | plausibilità operativa (niente scorta sovietica in Celtica, minamento nel Baltico) | **STIMA** |
| Durate, distanze, numeri di unità | provati su centinaia di semi per tipo | **STIMA** |
| Modelli del livello classico per classe | le 22 missioni convertite (`tools/rapide/estrai_modelli.lua`); le classi assenti per analogia | alta (originali), **STIMA** (analogie) |
| Comandi e armi del giocatore | i kit estratti dalle missioni convertite (`tools/rapide/estrai_kit.py`) | alta |
| Alba e tramonto | `nucleo/sole.hpp` (algoritmo semplificato della NOAA, qualche decimo di grado) | alta |
| Codice e controllo | Crockford, CRC-6 primitivo | esatto |

## 4. Classico ed Enhanced

Il file generato usa l'API classica e i modelli dell'originale per classe (`[modelli]` del file di dati), quindi si
gioca nei due livelli. Valgono solo nell'Enhanced l'ASW aereo e il minamento. Nel 1989 le missioni rapide non
c'erano.

## 5. Come si vede in gioco

- **Scelta della missione, scheda «Missione rapida»** (R; volume 1, cap. 11.1): i parametri a sinistra coi campi a caso
  risolti fra parentesi, a destra l'operazione con il codice, l'anteprima del fondale e il briefing. Invio la gioca, E
  la apre nell'editor.
- **Il codice** si digita nella scheda (Invio sulla riga del codice, o P da qualunque scheda) o si passa con
  `--codice`. Il codice di una rapida è diverso dal codice partita di 18 caratteri (volume 1, cap. 11.4), che dice
  anche il seme della partita e le opzioni. Una rapida mostra nel briefing, nel debriefing e nell'albo il suo codice
  di 13 caratteri, e la partita usa il seme della rapida.
- **Il briefing** è quello delle originali: ordini, situazione, regole d'ingaggio, tempo, carriera.
- **Il debriefing** e l'albo d'onore segnano l'origine «rapida».

## 6. Taratura e prove

- `test/prove_missione_rapida.cpp`:
  - «il codice va e torna, maiuscole o minuscole, con O, I e L di Crockford»;
  - «il controllo scarta ogni carattere sbagliato e ogni scambio di due vicini»;
  - «stesso codice, stesso file byte per byte; semi diversi, missioni diverse»;
  - «i tipi si giocano dove il teatro li consente, gli altri sono rifiutati»;
  - «50 semi per tipo, caricati e simulati 3 minuti nel classico senza errori»;
  - «alcuni semi per tipo nell'Enhanced, 2 minuti senza errori»;
  - «chiusura dopo la zona (ricognizione, inserimento) e a tempo scaduto»;
  - «la carriera solo se chiesta, con difficolta' 3 e paranoia 1 o piu'».
- Lo strumento `missione_rapida --verifica N [--enhanced]` genera N semi per tipo, li carica e li simula.
- Lo scrittore resta identico byte per byte per le rapide (libreria dei comportamenti 1): le prove dello scenario lo
  controllano.

## 7. Limiti noti e in arrivo

- **Versione congelata.** Una modifica che cambia le missioni generate richiede una nuova versione nel codice
  (`VERSIONE_RAPIDE`); `missioni_rapide.toml` ha la sua (`VERSIONE_DATI_RAPIDE`, oggi 1). Il generatore tiene il
  comportamento delle versioni vecchie, così i loro codici rigenerano gli stessi file (la prova di riferimento della
  versione 1, 40 codici).
- Le liste `ssbn` dei teatri ci sono ma nessun tipo le usa ancora.
- Le rapide usano la libreria dei comportamenti 1: niente parametri per unità, regole o zone dell'editor. Si possono
  aprire nell'editor e arricchire, e allora diventano missioni dell'editor.
- Il meteo fissato e i danni iniziali non sono parametri delle rapide.
- Il codice della rapida non porta le opzioni del menu: stazioni del 1989, carta d'epoca, meteo realistico, ASW,
  sonarista, navigazione realistica, GPS «1990+», traffico civile. Due giocatori con lo stesso codice giocano la stessa
  missione; per la stessa partita devono scegliere le stesse opzioni. Il codice partita v2, che le porta, vale per le
  missioni originali, della libreria, dell'editor e importate.

## 8. Riferimenti

- Codice: `missioni/missione_rapida.hpp`, `missioni/src/missione_rapida.cpp`, `app/scelta_missione.cpp`,
  `strumenti/missione_rapida.cpp`.
- Dati: `data/motore/missioni_rapide.toml`, `data/motore/missioni_rapide/kit_usa.lua`, `kit_urss.lua`.
- Progetto: `docs/09_schema_missioni.md` (formato 1), roadmap punto 10.
- Capitoli collegati: F7 (scenario e scrittore), F2 (carriera), volume 1 cap. 11.
