# A1. Il motore

*Autore: principale. Revisione del 29/10/2026 (codice partita versione 2, col GPS). Stato: rivisto (curatore: kit).*

## 1. A che serve

Il motore fa avanzare il mondo a passi fissi, sempre uguali, qualunque sia il ritmo dello schermo. Tastiera, mouse e
pannelli non toccano la simulazione: producono **ordini** con l'istante in cui valgono, e il motore li consegna
all'inizio del passo. Con lo stesso seme, le stesse opzioni e gli stessi ordini la partita è la stessa, passo per
passo. Da qui vengono il **codice partita** (rigiocare la stessa partita), il **replay**, il **«Riprendi da qui»**
(capitolo F5) e la **regressione** che tiene fermo il classico a ogni modifica.

## 2. Il modello

### 2.1 Il passo fisso

L'orologio è in `nucleo/orologio.hpp` (`Orologio`). L'app ne crea uno col passo predefinito (`app/main.cpp`).

```
passo_µs = round(passo · 10⁶)
accumulato_µs += round(min(dt_reale, 0,25) · compressione · 10⁶)
n = accumulato_µs / passo_µs        (divisione intera)
```

- **Passo delle missioni**: 1/9 s, la cadenza della VM dell'originale (`PASSO_S` di `missione.cpp`). È lo stesso passo
  delle prove e del rilievo delle 110 partite, quindi il gioco con la finestra dà gli stessi esiti del rilievo.
  L'orologio conta in microsecondi interi (111 111 µs); alla missione passa 1/9 s esatto.
- **Disegno ed effetti**: il disegno interpola posizioni e rotte fra gli ultimi due passi (`Mondo::interpolato`, con
  `alfa = accumulato / passo`), così il moto è fluido a ogni ritmo di fotogrammi; l'interpolazione è solo per gli occhi.
  Fumo, fuoco, spruzzi e
  colonne avanzano a ogni fotogramma, a sottopassi di al più 1/30 s (`Partita::anima_effetti`), e restano fluidi.
- **Scena di prova** (senza missione): 1/30 s.
- **Accumulo in microsecondi interi**: in virgola mobile `0,25 − 0,2 + 0,05` non fa 0,1 e un passo si perderebbe.
- **Fotogramma lento**: il tempo reale di un fotogramma conta al più 0,25 s.
- **Compressione**: da 1× a 64×, raddoppiata o dimezzata con Pag Su e Pag Giù (`main.cpp`). A 64× un fotogramma di
  0,25 s chiede 144 passi.
- **Tetto**: al più 600 passi per fotogramma (66,7 s di missione a 1/9 s). Oltre, la macchina non tiene il passo: l'accumulo si
  azzera e quel tempo simulato si perde. La partita resta deterministica, perché gli ordini portano l'istante di
  missione, non quello reale.
- **Pausa** (P): zero passi.

### 2.2 Il sottopasso della missione

La missione (`missioni/src/missione.cpp`, `Missione::avanza`) divide ogni `dt` in sottopassi di al più
`PASSO_S = 1/9 s`, il passo della VM dell'originale (9 al secondo). Con il passo dell'app (1/9 s) ogni passo è un
solo sottopasso. L'ordine dentro il sottopasso:

1. contatore `DS:5100` dell'originale (da 18 a 1, uno per sottopasso);
2. moto delle unità (`navigazione`), poi il tempo avanza;
3. siluri, mine, avarie e rimorchi dell'IA, sonar speciali, esche mobili, transitori delle armi;
4. **visita**: un'unità per sottopasso, a rotazione (affondamenti e controlli del giocatore);
5. pilota automatico, **timer degli script** in ordine di tempo e d'inserimento;
6. impatti delle armi (nell'Enhanced su ogni coppia), meteo;
7. **sensori**: un solo osservatore per sottopasso, a rotazione come nell'originale; sonarista dell'equipaggio e
   dell'IA, avvisi del sonar, paesaggio sonoro;
8. radio, periscopi, elettronica, collisioni, batterie dei diesel, BQS-15, artiglieria, ASW aereo ed evasione;
9. storia dei contatti, **registro degli eventi** (capitolo F1), uscita delle unità distrutte.

### 2.3 Il passo della partita

`app/partita.cpp`, `Partita::avanza(dt)`, una volta per passo:

1. gli ordini con istante ≤ tempo di missione escono dal registro (§2.5) e vanno al posto di comando,
   all'interfaccia, alle stazioni (che rifiutano quelli dei sistemi guasti), ai danni dell'Alfa (sfoghi, Tan), poi
   alla missione;
2. `Missione::avanza(dt)`;
3. stazioni dopo il passo: TMA (capitolo C13), siluri, filo;
4. un fotogramma della traccia del replay, quando serve (solo Enhanced, capitolo F5);
5. interfaccia dopo il passo (tracker del sonar, linee sulla carta);
6. danni delle navi (`DanniMissione`) e dei sottomarini (`DanniSottomarini`, solo Enhanced), raccolti nel registro;
7. mondo da disegnare, effetti visivi, avvisi composti (artiglieria, collisioni, unità dell'IA), tracce del debriefing
   ogni 20 s, fotografie per le minicartoline.

### 2.4 Il seme e il determinismo

**Il generatore** è `Casuale` (`nucleo/casuale.hpp`): xoshiro256** con i semi da splitmix64. Dà la stessa sequenza su
ogni piattaforma e compilatore, a differenza delle distribuzioni di `<random>`. Il nucleo e la missione non leggono
mai l'orologio di sistema (verificato sui sorgenti di `nucleo/src` e `missioni/src`).

**Dal seme della partita** nascono flussi separati, ognuno col seme mescolato a una costante:
- la missione: variante iniziale con le probabilità esatte dell'originale, dadi degli script e dell'IA;
- l'acustica (fluttuazioni, errori dei rilevamenti), il sonarista, il meteo (col teatro, la missione e la data);
- i danni delle navi e dei sottomarini;
- le stazioni (lo scarto dei rilevamenti della TMA senza acustica);
- gli effetti visivi.

**Da dove viene il seme:**

| Avvio | Seme |
|---|---|
| riga di comando (`--missione`) | `--seme N`, predefinito 1 |
| menu, missione originale o della libreria | nuovo a ogni partita (`seme_nuovo`) |
| codice partita (P nella scelta, o `--codice`) | quello del codice |
| missione rapida | quello del codice della rapida (capitolo F6) |
| editor, «Prova in mare» | quello dell'editor (capitolo F7) |

`seme_nuovo` (`app/codice_partita.cpp`) mescola due letture di `std::random_device` e l'orologio monotono con il
finalizzatore a 64 bit di MurmurHash3, e tiene 31 bit, quanti ne porta il codice partita. Non dà mai 0, che
nella carriera vuol dire «nessuno».

### 2.5 Lo storico degli ordini

`nucleo/ordini.hpp`, `RegistroOrdini`. Un ordine è `{istante, tipo, a, b, n, testo}`; i tipi sono 54 (manovra,
sensori, armi, radio, periscopi, Alfa). Il significato dei parametri è quello dei comandi dell'originale.

- **Consegna**: all'inizio del passo escono gli ordini con istante ≤ tempo, in ordine di istante e, a parità, di
  inserimento.
- **Ordine in ritardo**: un istante già passato vale al passo in cui arriva. Lo storico registra l'istante effettivo,
  così il replay lo consegna allo stesso passo. La storia non si riscrive.
- **Carico di partenza** della Sala siluri: sta nello storico e si applica prima del primo passo.
- **Testo**: una riga per ordine, `istante nome a b n [testo]`, coi numeri a 17 cifre (si rilegge identico); il carico
  in testa su righe `carico ...`; `#` per i commenti. `--registra FILE` salva lo storico all'uscita, `--ordini FILE` lo
  ripete.
- **Regola**: stesso seme + stesse opzioni + stesso storico + stesso passo = stessa partita.

### 2.6 L'impronta della partita

`Mondo::impronta()` (`nucleo/src/mondo.cpp`) riassume lo stato in 64 bit, per verificare il determinismo:

```
h = FNV-1a a 64 bit sugli 8 byte di: x, y, rotta, velocità di ogni unità del mondo da disegnare; poi il tempo
```

Le prove senza finestra la stampano: `istantanea e1.bmp dopo 600 s simulati (impronta 04e232a67bbd1059)`. Da non
confondere con l'impronta del file della missione nel codice partita (§2.7) e con quella SHA-256 dei pacchetti
(capitolo F7).

### 2.7 Il codice partita

`app/codice_partita.{hpp,cpp}`. Ricrea la stessa partita di una missione originale, della libreria, dell'editor o
importata. Le missioni rapide hanno il loro codice di 13 caratteri, che le genera (capitolo F6).

90 bit in 18 caratteri dell'alfabeto di Crockford (`0123456789ABCDEFGHJKMNPQRSTVWXYZ`), a gruppi di sei:
`XXXXXX-XXXXXX-XXXXXX`.

| Campo | Bit | Contenuto |
|---|---|---|
| versione | 3 | 2 (`VERSIONE_CODICE_PARTITA`) |
| origine | 2 | originale, libreria, editor, importata |
| impronta | 32 | del file della missione (sotto) |
| seme | 31 | della partita |
| GPS | 1 | l'opzione GPS «1990+» (capitolo C12 §2.6) |
| stazioni 1989 | 1 | modo 1989 o moderno |
| carta d'epoca | 1 | carta d'epoca o fondale vero |
| meteo realistico | 1 | meteo dinamico o semplificato |
| ASW | 2 | facile, normale, realistico, minaccia |
| sonarista | 2 | nessuno, recluta, medio, esperto |
| navigazione realistica | 1 | navigazione perfetta o realistica |
| senza traffico | 1 | 1: la partita è senza traffico civile (capitolo E10). Preso dalla riserva il 28/10/2026: i codici di prima lo leggono 0, cioè col traffico |
| controllo | 12 | CRC-12 dei 78 bit precedenti |

- **Impronta della missione**: FNV-1a a 32 bit dell'id, di un byte nullo e del file senza i ritorni di Windows (`\r`).
  Cambia se la missione cambia. Uno 0 diventa 1.
- **CRC-12**: polinomio x¹² + x¹¹ + x³ + x² + x + 1. Vede ogni errore entro 12 bit di fila: un carattere sbagliato
  (5 bit) e due caratteri vicini scambiati (10 bit).
- **Lettura**: maiuscole e minuscole uguali, trattini e spazi ignorati, O vale 0, I e L valgono 1.
- **La versione 1** (fino al 29/10/2026) aveva il seme a 32 bit e niente GPS: il bit del GPS l'ha preso al seme. I
  codici v1 si leggono ancora, col GPS spento. Il primo carattere porta versione e origine, e così si riconoscono a
  occhio: i v1 cominciano con 4-7, i v2 con 8-B (per esempio `8XGQT4-…`).
- **Quando c'è**: solo nell'Enhanced, per le missioni non rapide, con un seme di al più 31 bit e un file leggibile. Il
  classico non ne ha.
- **Ricerca della missione**: fra le originali (`data/missioni`), la libreria, le missioni dell'editor e le importate,
  per impronta. Con origine diversa da «originale» data, ora e centro del teatro vengono dallo scenario.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Passo di 1/9 s per le missioni, separato dal disegno | cadenza della VM dell'originale (`docs/originale/sensori_e_navigazione.md` §3.1) | alta |
| VM a 9 passi al secondo, un osservatore per passo | `docs/originale/sensori_e_navigazione.md` §3.1, dal disassemblato | alta |
| xoshiro256** e splitmix64 | Blackman e Vigna, generatori pubblicati | alta |
| FNV-1a (32 e 64 bit) | Fowler, Noll, Vo, costanti pubblicate | alta |
| CRC-12 e alfabeto di Crockford | polinomio CRC-12 classico; Base32 di D. Crockford | alta |
| Tetto di 600 passi per fotogramma, 0,25 s per fotogramma | scelta di progetto | **STIMA** (contro la «spirale della morte») |

## 4. Classico ed Enhanced

Tutti e due usano lo stesso orologio, lo stesso `Missione::avanza` e lo stesso storico degli ordini. Cambia quello che
gira dentro il passo.

| | Classico | Enhanced |
|---|---|---|
| Avvio | riga di comando, `--missione X` | menu, o `--regola simulazione` |
| Seme | `--seme`, predefinito 1 | nuovo a ogni partita dal menu |
| Impatti delle armi | quelli dell'originale (l'osservatore del passo) | ogni coppia a ogni passo |
| Scoperta | formula additiva dell'originale | equazione del sonar (capitolo C1) |
| Moto dei sottomarini | cinematica dell'originale | dinamica (capitolo B1) |
| Danni | regola classica delle navi | fisica delle navi e dei sottomarini |
| Radio, meteo, elettronica, siluri, ASW, periscopi, artiglieria, collisioni, batterie | no | sì |
| Traccia del replay, TMA, codice partita, coda dei messaggi | no | sì |

Nell'Enhanced questi modelli si accendono alla fine del primo passo, nel primo `DanniMissione::avanza`
(`missioni/src/danni_missione.cpp`). La sala radio e il meteo nascono prima, perché il briefing li legge.

## 5. Come si vede in gioco

- **Il tempo**: Pag Su e Pag Giù la compressione, P la pausa. Nell'Enhanced la compressione torna a 1× da sola a ogni
  evento importante (capitolo F1). Volume 1, §2.7.
- **Il codice partita**: in fondo al briefing, nella pagina dell'esito del debriefing (E), nell'albo d'onore, nei dati
  del replay; sul terminale all'avvio (`codice partita: ...`). P nella scelta della missione, in ogni scheda, lo fa
  digitare. Volume 1, §11.4.
- **Riprendi da qui**: capitolo F5; volume 1, §10.4.
- **Riga di comando** per le verifiche: `--istantanea FILE --secondi N` (senza finestra, stampa l'impronta),
  `--ordini`, `--registra`, `--seme`, `--codice`, `--riprendi-da T`, `--salva-traccia`. Le opzioni della partita si
  danno anche da riga di comando, come nel menu: per esempio `--navigazione realistica` e `--gps` (capitolo C12).
- **Misurare gli scatti:** la variabile d'ambiente `MELCHIOR_REGISTRO_TEMPI=file` scrive una riga per fotogramma, nel film e
  in partita, coi tempi in ms; le colonne sono nell'intestazione del file (`app/registro_tempi.hpp`). Tiene le righe in
  memoria e le scrive a buffer pieno e all'uscita, in aggiunta: un'uscita forzata può perdere le ultime. Spento costa un
  confronto, e le partite non lo vedono.
- **Il video d'apertura si prepara in anticipo** (`CodaFilm`, `app/video_apertura.hpp`): un filo prepara fotogrammi e
  suono, e quello che si mostra è legato ai campioni già dati, quindi immagine e suono non si scollano. Parte con 3 s
  pronti, cioè qualche secondo di nero all'avvio (5-7 misurati), poi va a 30 fotogrammi al secondo fissi. La coda arriva
  a 20 s, fino a 190 MB in più, oltre ai circa 340 MB delle due partite del film.

## 6. Taratura e prove

Prove automatiche:
- `test/prove_menu_modo.cpp` (bersaglio `prove_menu`, a parte da `prove` perché si lega a SDL): il modo, le schede del
  classico, il modulo e l'eliminazione con tasti e clic. `--menu-prova FILE` gira il menu vero senza mani (con
  `SDL_VIDEODRIVER=dummy`); le prove da capo a fondo sono in `tools/re/prove/menu/e2e/`;
- `test/prove_nucleo.cpp`: «orologio a passo fisso», «generatore casuale riproducibile», «simulazione
  deterministica» (due mondi uguali, 3000 passi, la stessa impronta);
- `test/prove_ordini.cpp`: consegna per istante e per arrivo, ordine in ritardo, storico salvato che si rilegge identico
  e ridà le stesse consegne, carico nello storico;
- `test/prove_codice_partita.cpp`: andata e ritorno, un carattere sbagliato o due scambiati non passano, l'impronta
  trova la missione e cambia col file, i codici della versione 1 si leggono ancora, senza GPS;
- `test/prove_registro.cpp`: «registro: deterministico, e il classico non cambia»;
- `test/prove_replay.cpp`: stesso seme, ordini e carico ridanno gli eventi del registro con gli stessi id.

La regressione sta in `tools/regressione/`. Ogni script prende il binario come argomento e lavora su una copia,
perché la build comune può ricompilare mentre gira.

| Script | Cosa fa | Deve dare |
|---|---|---|
| `casi.sh` | 7 partite senza finestra: impronta e md5 dell'istantanea | le righe di `riferimento.txt` |
| `rilievo_classico.sh` | le 110 partite del classico | «0 partite diverse su 110» |
| `suite.sh` | tutte le prove del motore | nessuna prova rossa |

I sette casi di `casi.sh`:

| Caso | Partita |
|---|---|
| c1 | SCENE1A, classico, 600 s, stazione Manovra |
| c2 | SCENE2A, classico, 900 s, tubo 1 caricato a 1 s e lanciato a 40 s |
| e1 | SCENE1A, Enhanced, 600 s, stazione Sonar |
| e2 | SCENE4A, Enhanced, 900 s, gli stessi ordini di c2 |
| e3 | SCENE1A, Enhanced, 30 s, urto di prova di 200 kg a 20 m, 90° |
| e4 | SCENE1B, Enhanced, 500 s, periscopio puntato sull'unità 2 |
| p1 | scena di prova, siluro sotto la chiglia, 20 s |

**Le 110 partite** (`engine/strumenti/rilievo_missioni.cpp`): le 22 scene originali per 5 semi, 3600 s ciascuna, col
giocatore passivo, a sottopassi di 1/9 s come la VM. Per partita: esito e quando, armi a segno, attacchi riusciti su
tentati, unità distrutte, prima scoperta del giocatore e da parte del giocatore. `confronta_rilievo.py` le confronta
con `engine/strumenti/riferimento/rilievo_missioni.tsv` (24/10/2026). Nel riferimento: 142 armi a segno, 117 attacchi
riusciti su 170, 79 unità distrutte; 62 partite finiscono con un esito entro l'ora.

## 7. Limiti noti e in arrivo

- **Cadenza corretta il 26/10/2026.** Fino ad allora l'app avanzava le missioni a 1/30 s. Ogni passo era un sottopasso
  della VM, e sensori, visite e contatori a passi correvano 3,3 volte più spesso che nell'originale: TORPEX col seme 1
  finiva prima dei 600 s con l'esito 2, invece che a 900 s con l'esito 5 come nel rilievo. Ora l'app va a 1/9 s e gli
  esiti coincidono.
- **Stessa versione**: il determinismo vale con lo stesso eseguibile. Seni, coseni e radici della libreria matematica
  possono cambiare con il compilatore, e il codice partita non porta la versione del motore.
- **L'impronta** guarda solo posizione, rotta e velocità delle unità del mondo da disegnare, e il tempo. Uno stato
  interno diverso (danni, contatti) si vede solo quando cambia il moto.
- **Due flussi casuali hanno un seme fisso**, non quello della partita: i rumori delle avarie dell'IA e gli errori del
  BQS-15 (`missioni/include/missioni/missione.hpp`). Sono deterministici, ma uguali in ogni partita.
- **Il primo passo dell'Enhanced** (0,111 s) gira con le regole del classico (§4).
- Un seme oltre 31 bit (`--seme`) non ha codice partita.
- **In arrivo**: fotografie periodiche dello stato, per rendere più rapidi replay e ripresa (`docs/00_ROADMAP.md`,
  punto 16).

## 8. Riferimenti

- Codice: `nucleo/orologio.hpp`, `nucleo/casuale.hpp`, `nucleo/ordini.hpp` e `nucleo/src/ordini.cpp`,
  `nucleo/mondo.hpp` e `nucleo/src/mondo.cpp` (`impronta`, `interpolato`); `app/main.cpp` (orologio, compressione,
  seme, codice partita), `app/partita.cpp` (`Partita::avanza`), `app/codice_partita.{hpp,cpp}`;
  `missioni/src/missione.cpp` (`Missione::avanza`), `missioni/src/danni_missione.cpp` (accensione dell'Enhanced).
- Regressione: `tools/regressione/casi.sh`, `riferimento.txt`, `rilievo_classico.sh`, `suite.sh`;
  `engine/strumenti/rilievo_missioni.cpp`, `confronta_rilievo.py`, `riferimento/rilievo_missioni.tsv`.
- Progetto: `engine/README.md`, `docs/00_ROADMAP.md`, `docs/11_comandi_e_stazioni.md` §2 (ordini).
- Originale: `docs/originale/sensori_e_navigazione.md` §3.1 (cadenza della VM).
