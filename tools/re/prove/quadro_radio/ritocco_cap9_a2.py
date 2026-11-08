# Riscrive il capitolo 9 di proposta.md con le scelte dell'utente del 06/11 (filo da A2 in su). Un solo giro: si ferma se e' gia' fatto
import pathlib
Q=pathlib.Path(__file__).parent; P=Q/'proposta.md'; t=P.read_text()
assert 'Scelte dell\'utente (06/11)' not in t
i94=t.index('### 9.4 Valori attesi'); i95=t.index('### 9.5 Che cosa fa scartare')
(Q/'attesi_primo_giro_06_11.md').write_text(
 "# Attesi del primo giro di proposta.md §9.4: RITIRATI\n\n"
 "Scritti il 06/11 con il filo sempre calato e sulle basi di ottobre. La principale li ha ritirati lo stesso giorno, prima di qualunque codice o misura: "
 "le basi non valgono più e il filo ora è solo da A2 in su. Non sono attese: restano per sapere da dove si è partiti. Gli attesi veri si fissano come dice §9.4.\n\n"
 + t[i94:i95].split('\n',1)[1].strip()+"\n")
testa=t[:t.index('## 9. Filo, ELF e rapporti')]
nota6="Decisioni 1 e 9 riaperte dall'utente il 06/11: le scelte sono in §9.6.\n\n"
sez6=testa.index('## 7. Che cosa resta fuori'); testa=testa[:sez6]+nota6+testa[sez6:]
nuovo=r'''## 9. Filo, ELF e rapporti dei sottomarini (06/11, scritto prima di compilare)

L'utente ha riaperto le decisioni 1 e 9 di §6 (punti 2 e 3 del piano) e il 06/11 ha scelto come in §9.6. Restano ferme le altre sette. La regola del rilevamento resta fuori (§8).
Il codice aspetta il via della principale, dopo la portata del sonar e il passo di chiusura.

**Giudizio in anticipo:**
- **A2 non è un'eccezione: è il minimo di partenza di quasi tutte le missioni** (§9.3). «Filo da A2 in su» limita il filo solo in S08 e S11, fra quelle misurate. Altrove vale come «filo sempre». C'è una domanda nuova in §9.6.
- **Il filo cambia molto le copie e poco il dato usato.** Chi ha un contatto solido o un aggancio continua a non usare il dato.
- **Il vero cambiamento è un altro:** dove parte in A2 il battello non sale quasi più, quindi non mostra più l'albero. Sparisce il costo di salire.
- **Il campanello ELF si vedrà pochissimo.** Vale solo per l'URSS, solo per i FLASH, e serve solo sotto i 120 m o sopra i 12 nodi.
- **I rapporti localizzati si vedranno poco,** e quasi solo in S08 e S11, dove i battelli restano sotto A2 e continuano a salire.

### 9.1 Punto 2: filo galleggiante ed ELF

Le stesse regole del giocatore (`nucleo/src/radio.cpp:160-172`, `:256-262`, `radio.hpp:255-261`), con le costanti di `ParametriRadio`, ma il filo c'è solo da A2 in su.

| Voce | Regola per il battello dell'IA |
|---|---|
| Quando | **Da `allerta_unita(id) >= 2`** (scelta 1). Chi parte in A2 lo fila dal primo istante. Chi ci arriva lo fila allora: 120 s senza cambiare la manovra. Niente sotto la banchisa (STIMA: la scelta di filarlo subito; il giocatore la fa a mano) |
| Sotto A2 | Niente filo: **il codice di oggi, salite comprese.** Chi torna sotto A2 (la discesa, 2700 s senza eventi) lo ritira in 120 s (STIMA) e torna alla salita di §3.2. L'allerta regolata della sandbox vince: una forza ad A0 in guerra è sorpresa e non ha il filo |
| Ricezione | quota ≤ 120 m; fino a 8 nodi sempre, fino a 12 al 50%, oltre niente; cieco 120 s dopo una virata oltre 0,5°/s |
| Copia | 80 s continui nei limiti prima del ciclo, come ad albero. Se fallisce, ritenta ai cicli dopo per un'ora dal fix (`missione_forze.cpp:709`) |
| Rottura | oltre 20 nodi per più di 60 s: 30% al minuto. **Rotto resta rotto:** il battello torna alla salita di §3.2 |
| Finestra | col filo nei limiti non sale: conta come ascolto. **Oltre 120 m o 12 nodi** sale a 100 m (STIMA) e rallenta a 8 nodi, solo alle finestre e alle condizioni di §3.2. Finché il filo non è filato vale la regola di oggi |
| Pace | riceve col filo se è in A2, ma non sale e non usa il dato (decisione 2). Il filo si vede solo in ROE e falsi ordini |
| ROE e falsi ordini | **anche dal filo** (scelta 3): filato, ≤ 120 m, ≤ 12 nodi, non cieco. Senza dado e senza gli 80 s, come arrivano oggi ad albero. **Conseguenza nel vetrino:** nelle missioni di pace in A2 un battello a 100 m riceve subito un ordine che oggi non gli arriverebbe mai, perché in pace non sale |
| ELF, solo URSS | a un FLASH in onda, con filo o albero, quota ≤ 300 m e velocità ≤ 15 nodi: il campanello apre la finestra subito. Riceve sempre al ciclo. USA niente, come il giocatore (`radio.cpp:89`). Sotto A2 servirebbe l'albero, dove il battello ascolta già: in pratica vale solo da A2 in su |
| Numeri casuali | il 50% tra 8 e 12 nodi e la rottura usano `F.caso_radio`. **Sotto A2 nessun dado nuovo** |
| Comandante IA | senza filo (scelta 5): sale ogni 1800 s (`comandante_ia.cpp:50`). L'asimmetria va scritta nel manuale |

**Che cosa resta com'è:** dato non sufficiente per un siluro (decisione 5), limite di 30 km, contatto o aggancio che bloccano la caccia, regole dei missili.

**Righe da toccare:**

| File | Dove | Che cosa |
|---|---|---|
| `forze_missione.hpp` | `RadioSottomarino` | stato del filo, cecità, tempo oltre la velocità di rottura, inizio dell'ascolto, quota della salita, campanello |
| `missione_forze.cpp` | `:621-626` | `copia` accetta anche il filo |
| | `:720` | il FLASH suona il campanello ai battelli URSS nei limiti |
| | `:1323` | l'ascolto è albero o filo; lo stato del filo segue `a` di `:1328` |
| | `:1358`, `:1370`, `:1400` | la quota della salita non è più fissa |
| | `:1390-1394` | col filo non sale; oltre i limiti sale a 100 m; il campanello apre la finestra |
| | `:1083`, `missione_interventi.cpp:543` | ROE e falso ordine anche col filo (scelta 3) |
| `missione.hpp` | `:845` | `usa_filo_sottomarini(bool)`, accanto all'interruttore del quadro |

Niente header del nucleo. `missione.hpp` è condiviso: va annunciato e costringe a ricompilare chi lo include.

**Prove:** le scene del quadro stanno a 100 m e ricevono solo ad albero: le 25 chiamate di `prove_forze.cpp` girano col filo spento e restano come sono.
Prove nuove: in A1 niente filo e salita di oggi; in A2 filato dopo 120 s; ritirato alla discesa; allerta regolata; riceve a 100 m e non a 130; limiti di 8 e 12 nodi; cieco dopo la virata; rottura; sotto la banchisa niente; 80 s di copia; non sale col filo; sale a 100 m da 200 m; rotto torna ad albero; ROE dal filo senza dado; campanello solo URSS e solo FLASH; interruttore spento e originali identici.
Circa 15 prove e 15 guasti nuovi.

### 9.2 Punto 3: il battello che riferisce trasmette

| Oggi | Con la regola |
|---|---|
| riferisce fino a 20 m, senza albero (`:619`, `:632`) | solo a quota d'antenna e fuori dalla banchisa, come chi trasmette dal giocatore (`radio.cpp:341`). **Non sale apposta** (scelta 2) |
| nessuna radiogoniometria | ogni trasmissione passa da `radiogoniometria_ia` (`:1159`) |
| il fix arriva con 1852 m di errore (`:741`, `:596`) | **il rapporto porta il suo errore** (scelta 4) |

- **Quale regola scatta:** URSS, raffica HF di 0,72 s: fix al 34%, con errore di 65 km a 1 sigma e ritardo di 1-3 h. USA, UHF di 1,5 s (STIMA, da 300 caratteri): 24% di rilevamento se un nemico con ESM è entro 50 km, se no 2% di fix con errore di 185 km e ritardo di 3-6 h.
- **Una trasmissione sola:** più contatti nello stesso giro valgono un lancio di dado, con al più una trasmissione ogni 60 s per battello (STIMA). Passa solo chi invia davvero al teatro (`:683`).
- **Numeri casuali:** per il rapporto `radiogoniometria_ia` usa `F.caso_radio`, così `F.caso` non si sposta. Il falso ordine resta com'è. Serve un parametro in `missione.hpp:1658`.
- **Dove arriva il fix:** al teatro della parte nemica come `sconosciuto`, con il suo giro: navi, sottomarini, aerei ASW e, se è di quella parte, il giocatore.
- **L'errore:** un campo in `RapportoContatto` (`missione.hpp:931`), 1852 m se non è un fix. Lo leggono il rapporto al giocatore (`:741`), il datum dell'aereo ASW (`:596`) e il raggio del quadro. Vale anche per il fix del falso ordine.
- **Interruttore:** solo col quadro acceso: nelle originali e con l'interruttore spento resta `riceve`.

**Righe:** `missione_forze.cpp:596`, `:632`, `:683-695`, `:741`, `:1159-1176`, `:1191`; `missione.hpp:931`, `:1658`. Prove nuove, circa 6: a 15 m senza albero non riferisce; ad albero sì; URSS e USA con le probabilità del modulo su 200 semi; nessun nuovo dado su `F.caso`; il fix porta l'errore; originali identiche. Circa 5 guasti.

### 9.3 Che cosa dicono le tracce già giocate

**Tutti i numeri di questo paragrafo sono di ottobre, sul codice del 05/11.** Spiegano il ragionamento e non sono attese. Vengono da `stima_filo.py` e `allerta_tracce.py`, che legge l'allerta dalle tracce (`engine/build_radio/misura_allerta/esito.txt`).

**L'allerta di partenza è la `paranoia` della missione, e non scende sotto quella** (`missione_forze.cpp:186`, `:982`).

| Partenza | Missioni con sottomarini USA o URSS nella libreria (27) |
|---|---|
| Guerra o crisi, A2 | I02, I05, I06, I07, S03, S07, S15; le scene M4 e S5 |
| Crisi, A1 | **solo S08 e S11** |
| Pace, A2 o più | 16 missioni, S22 compresa: nessuno sale comunque |
| Pace, A1 | S01, S23 |

Nelle partite rapide la decide chi le crea. In S08 i Foxtrot non hanno mai toccato A2, e i Los Angeles di S08 e S11 ci arrivano solo dopo un evento.

**Dove stanno le 62 salite delle 18 partite:**

| Dove | Salite | Con il filo da A2 |
|---|---|---|
| I06 14, S03 14, I07 10, S07 4, S15 4, I05 3, I02 2; S11 1: in A2 o più | **52** | spariscono, salvo chi supera i limiti o ha rotto il filo |
| S08 7, S11 3: sotto A2 | **10** | restano come sono |

Solo **6** delle 62 sono in A3. Il dato usato, 2 volte, è tutto in A2 (I07).

**Tempo nei limiti del filo.** La colonna A2+ è la parte di partita in cui il battello lo avrebbe.

| Scena e battelli | A2+ | Filo utile fino a 8 nodi | Fino a 12 nodi | Oltre 120 m | Oggi ad albero |
|---|---|---|---|---|---|
| I02, Typhoon | tutta | 0% | 100% | 0% | 6-7% |
| I02, Los Angeles | tutta | 21-66% | 88-100% | 0-11% | 0% |
| I05, Kilo | tutta | 95-98% | 95-98% | 1-4% | 11% |
| I06, Los Angeles | tutta | 23-52% | 65-100% | 0-34% | 3-5% |
| I07, Oscar | tutta | 53-70% | 55-75% | 24-43% | 8-10% |
| S03, Foxtrot | tutta | 81-100% | 81-100% | 0-18% | 5-11% |
| S07, Akula | tutta | 68-89% | 78-90% | 0% | 2-8% |
| S08, Foxtrot URSS | **mai** | 5-53% | 100% | 0% | 37-100% |
| S08, Los Angeles | 56-83% | 61-62% | 100% | 0% | 0% |
| S11, Foxtrot | **mai** | 100% | 100% | 0% | 19-100% |
| S11, Los Angeles | **0-50%** | 100% | 100% | 0% | 19-100% |
| S15, Charlie II | tutta | 39-42% | 100% | 0% | 39-44% |
| M4, Victor | tutta | 17-22% | 60-67% | 32-39% | 0% |
| S5, Victor (semi 1, 3, 7) | tutta | 9-59% | 9-64% | 32-90% | 0-40% |

Le colonne dei limiti contano tutta la partita, non la sola parte in A2+.

**Il filo si rompe proprio dove si combatte.** Il Victor passa i 20 nodi per 810-1530 s in 7 partite M4 su 8 e in tutte e 3 le S5: oltre i 60 s, a 30% al minuto, il filo non regge.

**Rapporti di battelli: 27 in 18 partite.**

| Missione | Rapporti, sommati sui 2 semi | Battelli | Con il filo da A2 |
|---|---|---|---|
| S03 | 18 | Foxtrot, che salgono per l'ascolto | calano molto: non salgono più |
| S11 | 6 | Los Angeles | restano, finché è sotto A2 |
| S08 | 2 | Foxtrot | restano |
| S15 | 1 | Charlie II | calano |
| le altre cinque | 0 | | |

Fuori dalla libreria: M4 0 nelle 4 partite guardate; S5 25 nel solo seme 7, 0 nei semi 1 e 3.

**I dati che possono servire sono pochi.** In guerra le missioni sono I06, I07 e S07. I06 riceve solo rapporti su aerei, che non si cacciano. I07 e S07 ricevono rapporti sul Los Angeles del giocatore; oggi la S07 non copia niente.

**L'età del dato, col filo:** arriva al primo ciclo, in media circa 375 s dopo il teatro. Le navi riferite dagli USA hanno 1275-2175 s, spesso entro i 1824 s del limite; quelle riferite dall'URSS 1725-3075 s, quasi sempre oltre. I sottomarini classificati, FLASH, hanno 825-1275 s, sempre entro i 2736 s.

**Il fix serve a poco a chi lo riceve.** Dopo 1 ora `r` vale almeno 39 km, e con l'errore del fix (scelta 4) molto di più: oltre i 30 km della caccia e oltre i 10 km dei missili. Lo usano un aereo ASW, come datum, e il giocatore, che lo vede sulla carta.

### 9.4 Valori attesi: il ragionamento, e come si fissano i numeri

La tabella del primo giro, con il filo sempre calato e le basi di ottobre, è **ritirata** e sta in `attesi_primo_giro_06_11.md`. Le basi si sposteranno ancora con la portata del sonar e con le batterie: più contatti da lontano vogliono dire più rapporti e più battelli bloccati dal contatto. Non provo a indovinare di quanto.

**Da qui i numeri si fissano così, sempre prima di scrivere codice:**
1. Corsa sul codice di allora, dopo la portata e il passo di chiusura, con il binario di misura (`sonda_quadro.py`), e lo stesso codice per tutta la corsa.
2. `allerta_tracce.py` e uno script nuovo, `attesi_a2.py`, ricavano i numeri con le regole qui sotto. **Le regole e i margini sono fissati ora.**
3. Scrivo i numeri in questo paragrafo e li passo alla principale. Se un'altra sessione cambia le partite prima del codice, si rifà il passo. Dopo il codice non si riscrivono.

**Che cosa ci si aspetta, in parole:**
- **Dove si parte in A2** il filo vale come sempre calato: tante copie in più, quasi nessuna salita, rapporti in calo.
- **Dove si parte in A1, senza eventi,** la partita è quella di P3. Un battello che non tocca A2 non cambia di una riga.
- **In pace** il filo non si vede senza interventi del vetrino: nessuno sale e nessuno usa il dato.
- **M4 e S5:** Victor in A3 quasi tutta la partita, spesso oltre i 20 nodi. Il filo si rompe presto e poi vale la salita di oggi. Poco effetto.
- **Il dato usato** resta poco: serve un battello senza contatti, con un dato ostile non scaduto, entro i 30 km.

**Le regole.** Bracci: **P3** solo rapporti, **P2+P3** con filo ed ELF, sempre contro la base. Tutte su battelli USA e URSS dell'IA, sommati sui semi. Si arrotonda verso il margine più largo.

| Voce | Che cosa si legge nella base | Atteso P3 | Atteso P2+P3 |
|---|---|---|---|
| Salite ad albero, 18 partite | `Sb` sotto A2; `Sa` in A2+; `Sr` in A2+ di chi prima ha passato 20 nodi per oltre 60 s | uguali alla base | fra `Sb` e `Sb + Sr + 25% Sa` |
| Gruppi di copie con dati ostili, 18 partite | `G` nella base; `C` cicli in A2+ con un rapporto ostile al teatro e 80 s nei limiti, filo non rotto, contando 1/2 tra 8 e 12 nodi | uguali | **almeno `G + 50% C`** |
| Dato usato, 18 partite | `U` nella base; `Uc` cicli di `C` col battello libero (righe `QUADRO finestra`), dato non scaduto ed entro 30 km | fra `U` e `U + 1` | fra `U` e `U + 150% Uc` |
| Rapporti di battelli, 18 partite | `Rb` sotto A2 ad albero; `Ra` in A2+ ad albero; `R20` fino a 20 m senza albero | fra 70% e 130% di `Rb + Ra` | fra 50% di `Rb` e 130% di `Rb` + 25% di `Ra` |
| Fix sui battelli, 18 partite | trasmissioni di ciascuna parte, stimate dai rapporti attesi | intervallo binomiale al 95% con le probabilità di §9.2 | lo stesso |
| Contatti al giocatore da un fix | fix col ritardo che cade prima della fine, della parte giusta | al più il massimo binomiale | lo stesso |
| Campanelli ELF | `E` FLASH a battelli URSS in A2+, entro 300 m e 15 nodi, che il filo non riceve | | fra 0 ed `E`. **Se `E` è 0 l'ELF non si scrive** |
| Giocatore affondato, 18 partite | `A` nella base | al più `A + 2` | al più `A + 2` |
| S5: Victor affondato, 10 semi | `V` per variante | al più `V + 3` | al più `V + 3` |
| M4 B e C, S5 con la spia: partite col dato usato | `Uc` per variante | come la base | al più le partite con `Uc` > 0 |

**Quelli che non dipendono dalla base:**

| Controllo | Atteso |
|---|---|
| **Partita in cui nessun battello tocca A2** (in ottobre: S11 seme 2) | stessa impronta fra P2+P3 e P3 |
| Partite identiche fra P3 e base | quelle in cui nessun battello riferisce: in ottobre M4 A e S5 semi 1 e 3 |
| Lanci sul solo dato radio | 0 |
| Salite per ciclo | al più 1 |
| M3 | identica |
| S22, 12 semi | identica, per i semi che restano in pace e dove nessun battello riferisce nella base. Lo dice la corsa |
| Rilievo classico | 0/110 |

**Cancello sulla carta:** se la corsa dà `C` minore di `G`, le copie non possono raddoppiare. Lo dico alla principale prima di scrivere il filo.

### 9.5 Che cosa fa scartare

**P2, filo ed ELF, si scarta se manca uno di questi:**
1. **Effetto, sulle partite che partono in A2:** gruppi di copie con dati ostili almeno `G + 50% C` e almeno il doppio della base; salite in A2+ al più un terzo della base. Se non è così, il filo non fa quello che dice.
2. **Nessun danno:** nessun lancio sul solo dato; salite per ciclo al più 1; giocatore affondato al più `A + 2` su 18; Victor della S5 al più `V + 3` su 10; nessuna partita chiaramente peggiorata.
3. **Controlli:** rilievo 0/110; originali e interruttori spenti identici; M3 identica; S22 e partite che restano sotto A2 come in §9.4; suite senza nuove prove fallite; guasti tutti rossi.

**Il solo ELF si toglie, lasciando il filo,** se in tutta la serie non apre neanche una finestra: la regola costerebbe righe e manuale senza mostrarsi.

**P3 si scarta** solo per il punto 2 o il punto 3, o se un fix manda aerei ASW a vuoto al punto di peggiorare chiaramente una missione. Non si scarta per lo scarso effetto: chiude un'asimmetria (decisione 9) e costa poco.

### 9.6 Scelte dell'utente (06/11) e una domanda nuova

L'utente ha accettato tutti i consigli.

| # | Scelta | Che cosa vuol dire |
|---|---|---|
| 1 | **Filo solo da A2 in su** | §9.1. Sotto A2 il codice di oggi |
| 2 | **Niente salita apposta per riferire** | §9.2. Oggi non lo fa, e romperebbe il silenzio proprio di chi ha un contatto |
| 3 | **ROE e falsi ordini anche dal filo** | §9.1. Costa due righe e cambia i tempi del vetrino |
| 4 | **Il rapporto porta il suo errore** | §9.2. Oggi il giocatore vedrebbe 20 km dopo 1 ora su un fix con 65 km di errore |
| 5 | **Comandante IA senza filo, per ora** | Cambiarlo sposterebbe tutte le serie dell'utente. L'asimmetria va nel manuale |
| 6 | **Prima i rapporti, poi il filo dopo il cancello di circa 1 ora** | §9.7 |

**Domanda nuova, per la principale.** Quando ho proposto «filo solo in A2 e A3» come alternativa, e scritto «il resto del tempo si sale come oggi», non avevo guardato le allerte. In realtà A2 è il minimo di partenza di 23 missioni su 27 e di 7 delle 9 misurate: **in ottobre 52 salite su 62 erano già in A2 o più.** L'utente può aver scelto pensando a un limite che quasi non c'è. Va bene così?

| Possibilità | Effetto, in ottobre | Giudizio |
|---|---|---|
| **A2 in su, come scelto** | l'albero sparisce quasi del tutto in 7 missioni su 9, resta in S08 e S11 | **Consiglio questa.** È coerente: A2 è il posto di combattimento (`interazioni.toml`), e porta tutte le copie in più |
| A3 soltanto | tocca solo 6 salite su 62; I06 e S03 non lo vedrebbero quasi mai; Victor quasi sempre in A3, ma lì il filo si rompe | Tiene l'albero, ma il filo quasi non si vede e costa lo stesso. Tanto vale non farlo |

Le righe e il costo sono gli stessi. Per scegliere la seconda dopo i numeri non serve rifare la corsa.

### 9.7 Costo reale

**L'ordine:** corsa di base e numeri; P3 con le sue serie; prima versione del filo; cancello; prove, guasti e serie del filo. **Il cancello viene dopo la prima versione del filo e prima di prove e guasti,** così, se non funziona, si spende poco.

| Passo | Macchina (misura del 05/11, nice 19) |
|---|---|
| **Corsa di base, nuova:** 18 partite, M4 e S5 a 10 semi, M3 e S22 a 12 semi | **circa 5 h** |
| P3: S03, S08, S11, S15, 8 partite, più i semi di S5 dove il Victor riferisce | circa 60 min |
| Cancello: S07 semi 1 e 2, I07 seme 1, S11 con il seme in cui il Los Angeles passa ad A2 | circa 1 h. Se S07 non copia dati ostili, ci si ferma e si riferisce |
| P2+P3: 18 partite | circa 130 min |
| P2+P3: M4 (3 varianti) e S5 (2), 10 semi | circa 115 min |
| M3 e S22, 12 semi | circa 60 min |
| Suite e rilievo | 60-120 min |
| Guasti, circa 50 | circa 45 min |
| **Totale** | **circa 13-14 h** |

Rispetto al primo giro ci sono 3-5 ore in più, per la corsa di base. Si possono togliere circa 60 min se la portata o la chiusura lasciano M3 e S22 sullo stesso binario; M4 e S5 no, perché servono le righe `QUADRO`.
Con la macchina più carica i tempi crescono. Disco: 60 GB liberi il 06/11, la serie ne usa pochi.

**Lavoro di sessione:** numeri e `attesi_a2.py` circa 1 h; P3 con l'errore del rapporto circa 2 h; P2 con ELF e la regola di A2 circa 5 h, compresi prove e guasti; poi la lettura delle serie, circa 1 h. **Circa 9 h.** Il testo per il manuale va al kit.

**Senza ELF** si risparmia circa 1 ora di lavoro e nessuna ora di macchina.
'''
P.write_text(testa+nuovo)
