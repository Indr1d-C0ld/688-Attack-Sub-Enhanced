# A3. Le missioni originali

*Autore: disassemblatore. Revisione del 26/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Le 22 missioni del 1989 tornano come erano. Nel gioco originale una missione non è un elenco di dati: è un
**programma** per una piccola macchina virtuale dentro `GAME.EXE`, che crea le unità, le fa navigare e combattere,
gestisce i comandi del giocatore, i timer, i messaggi di bordo e l'esito. Il rifacimento ha ricostruito la macchina,
ha tradotto ogni scena in Lua istruzione per istruzione e la fa girare con le stesse regole: la stessa cadenza, gli
stessi conti alla rovescia, lo stesso modo in cui un siluro colpisce. Per questo le missioni originali danno gli stessi
esiti di allora.

## 2. Il modello

### 2.1 La macchina virtuale

Ogni scena è un file `SCENEnX.TOK` in bytecode (`docs/04_vm_missioni.md`). L'interprete:
- carica il file in un buffer di 5 KB e legge un byte alla volta: `0` termina, altrimenti chiama uno dei **118
  opcode**, e ogni gestore legge da sé i suoi argomenti;
- i salti sono offset dall'inizio del file; `CALL` esegue una sottoroutine fino a `END`;
- le **unità** sono record di 100 byte in una lista collegata: si riempie un *modello* (posizione, quota, rotta,
  velocità, tipo, flag, prestazioni) e `NEW_UNIT` ne fa una copia con un id nuovo;
- ogni unità ha tre **script**: all'arrivo (`+0E`, dopo `GOTO_XY`, `PURSUE`, `LAUNCH`), al bersaglio (`+10`, quando un
  contatto diventa bersaglio) e alla reazione (`+56`, quando lo perde);
- i **timer** (`AT tempo, etichetta`) stanno in una lista ordinata; 1 tick = 2 s;
- le **variabili di scenario** sono una tabella di parole; la casualità è un generatore congruenziale a 8 bit, e
  `IF_RND p` salta con probabilità (p + 1) / 256;
- `MSG c` mette un messaggio di bordo in coda; i codici da 100 in su sono l'**esito** di fine missione (la pagina
  «CODE: n-x-ACP» del briefing).

**Le unità di misura** del record (`docs/04_vm_missioni.md` §2): 1 miglio = 50 unità di gioco; la quota in ottavi di
piede; la rotta in 9000 parti di giro; la velocità in 1/350 di nodo. Il moto è accelerato di circa 3,5 volte, come
volle il progettista (Paul Grace): le distanze di combattimento sono «dell'ordine di 15 miglia». Nel rifacimento, sulle
carte vere, il fattore k va da 2 a 4,3 secondo la posa della missione (`moto_classico_m_s_per_nodo`, capitolo A2).

**La cadenza**: un tick ha 18 passi di simulazione, cioè 9 passi al secondo di gioco. Il motore avanza a passi fissi
di 1/9 s (`PASSO_S` in `missioni/src/missione.cpp`), perché le regole che contano i passi (la visita dei sensori, i
contatori) corrano come nel 1989.

### 2.2 La validazione del bytecode

Su tutti i 22 file (42 208 byte):
- **ogni byte si decodifica** in un'istruzione valida, coperta una volta sola, senza sovrapposizioni, con tutte le
  etichette all'inizio di un'istruzione;
- il flusso, partito dall'offset 0 e da tutte le etichette (salti, `CALL`, timer, gestori, script delle unità),
  raggiunge 14 324 istruzioni, il 96% dei byte; il resto sono blocchi orfani, avanzi degli autori;
- 14 opcode sono implementati nell'eseguibile ma mai usati nelle missioni.

Due cose trovate nel codice:
- **`IF_TAMPERED`** (0x75) confronta 45 byte della memoria DOS con valori attesi: è un controllo anti-manomissione.
  In SCENE1A, se fallisce, il battello del giocatore è sabotato (integrità 10, sensori al minimo) col messaggio
  «adjusting engine speed»;
- **il difetto di `IF_VARCMP`** (0x65): a parità salta a metà dell'istruzione dopo. In SCENE1C fa eseguire una quota
  senza senso, poi `END`, e l'esito della missione non arriva.

### 2.3 La conversione in Lua

`tools/re/missioni_lua.py` traduce ogni scena in `data/missioni/<SCENA>.lua` (formato in
`docs/09_schema_missioni.md`):
- ogni etichetta del bytecode diventa una funzione; le condizioni diventano `if m688.<condizione>(u, …) then goto
  L_xxxx end`, i salti `goto`, `CALL` una chiamata;
- ogni opcode è una funzione dell'**API `m688`** con un nome italiano e le grandezze convertite (`m688.dopo` è `AT`,
  `m688.caso` è `IF_RND`, `m688.attacca` è `ATTACK`); l'elenco con l'opcode d'origine e gli usi è generato in
  `data/missioni/API.md`;
- lo script d'inizializzazione si esegue in modo simbolico (`tok_schede.py`) e ne escono le **varianti iniziali** con
  le probabilità esatte: in tutte le 22 scene sommano a 1 (SCENE9A ha 576 combinazioni);
- le unità si **posano sul teatro vero**: la carta SURF dell'originale si ricolloca sulle coste reali
  (`ricolloca_surf.py`), con la scala in metri per unità; le navi partono in acqua di almeno il loro pescaggio più 3
  m, i sottomarini con almeno 10 m sotto la chiglia. Le correzioni sono piccole (al più 2,5 km, una volta 12,6 km);
- le rotte sono convertite in gradi bussola e le virate col positivo a dritta;
- le armi hanno il loro modello di corsa (Mk 48: 50,3 nodi in gioco, 280 s di corsa, 190 s di filo, danno 75).

Le lettere delle scene: A col giocatore sul 688, B sull'Alfa, C probabilmente la partita a due via modem (entrambi i
sottomarini in testa e gli script con `IF_PLAYER1`).

<!-- generato: carriera_originali -->
| Missione | Situazione | Avversario | Difficoltà | Operazione speciale | Artico |
|---|---|---|---|---|---|
| Colpisci e fuggi (SCENE0A) | guerra | regolare | 4 | sì | — |
| TORPEX '89 (SCENE1A) | pace | impreparato | 1 | — | — |
| TORPEX '89 (SCENE1B) | pace | impreparato | 1 | — | — |
| TORPEX '89 (SCENE1C) | pace | impreparato | 1 | — | — |
| Scrollatevelo di dosso (SCENE2A) | crisi | veterano | 2 | — | — |
| Scrollatevelo di dosso (SCENE2B) | crisi | veterano | 2 | — | — |
| Scrollatevelo di dosso (SCENE2C) | crisi | veterano | 2 | — | — |
| Operazioni subacquee (SCENE3B) | pace | regolare | 2 | — | sì |
| Mumar Cadaver (SCENE4A) | crisi | regolare | 3 | sì | — |
| Fuga (SCENE5A) | crisi | veterano | 3 | — | — |
| Fuga (SCENE5B) | crisi | veterano | 3 | — | — |
| Fuga (SCENE5C) | crisi | veterano | 3 | — | — |
| Gulasch (SCENE6A) | crisi | regolare | 3 | — | — |
| Passerella (SCENE7A) | guerra | veterano | 4 | — | — |
| Passerella (SCENE7B) | guerra | veterano | 4 | — | — |
| Passerella (SCENE7C) | guerra | veterano | 4 | — | — |
| Festa a sorpresa (SCENE8A) | guerra | regolare | 4 | — | sì |
| Festa a sorpresa (SCENE8B) | guerra | regolare | 4 | — | sì |
| Festa a sorpresa (SCENE8C) | guerra | regolare | 4 | — | sì |
| Ritorno a casa (SCENE9A) | guerra | elite | 5 | — | — |
| Ritorno a casa (SCENE9B) | guerra | elite | 5 | — | — |
| Ritorno a casa (SCENE9C) | guerra | elite | 5 | — | — |
<!-- fine: carriera_originali -->

La tabella completa delle scene, con forze, combinazioni ed esiti, è in `docs/04_vm_missioni.md` §7.

### 2.4 I gestori del giocatore

Anche il sottomarino del giocatore è guidato dal bytecode. Un dispatcher traduce ogni comando nell'esecuzione di un
gestore dello script e **scrive i parametri del comando negli argomenti della prima istruzione** del gestore: il
telegrafo (in 1/256 della velocità massima), il timone (−10…+10), i timoni orizzontali (quota e rateo), la quota
periscopica, i sette tubi, il lancio per tipo d'arma, il periscopio, l'antenna, la schiera, il bersaglio designato, la
guida delle armi filoguidate.

Nel Lua sono i `gestori_giocatore` della missione (`telegrafo`, `quota`, `timone`, `carica_tubo_1..7`,
`schiera_cala`…). I nove gestori con parametri sono `function(u, p)`: dove l'originale scriveva il parametro, c'è
`par(p, i, valore originale)`. Il motore li chiama con `Missione::comando` (il dispatcher) e `Missione::applica` (gli
ordini del giocatore). La tabella completa dei codici è in `docs/originale/sensori_e_navigazione.md` §7.

### 2.5 La regola d'impatto

Ricavata dal disassemblato il 24/10/2026 (`docs/originale/sensori_e_navigazione.md` §9.8). Nell'originale non c'è
una routine per le armi: la collisione sta nella stessa routine delle coppie osservatore e bersaglio, prima della
scoperta.
- **Le armi non si colpiscono fra loro.**
- Impatto se la quota differisce meno di 100 piedi (30,5 m) e la distanza meno di 5 unità per asse (185 m nella scala
  di gioco: un quadrato, fino a 262 m in diagonale), e nessuno dei due è collegato all'altro.
- La vittima non è un aereo, un'altra arma o un falso bersaglio (esche, biologici, bersagli dei missili).
- **Qualsiasi unità**, non solo il bersaglio: fuoco amico e neutrali vengono colpiti, anche chi sta già affondando.
- Tagliato il filo, il siluro **può colpire il proprio lanciatore** (un difetto dell'originale).
- Il danno è quello dell'arma (Mk 48 75, 533 mm 100), **dimezzato in 7 passi su 18**; poi l'integrità scende del
  danno. Sotto zero l'unità è distrutta e conta l'uccisione al lanciatore.
- L'arma esce subito dal gioco. Le cariche e i missili non sono oggetti ma opcode (`ATTACK`, `ATTACK_RND`), con le
  loro probabilità e un danno fisso o casuale.

### 2.6 Nel motore

`engine/missioni/` (`Missione`): il runtime del livello classico con tutte le funzioni `m688`, Lua 5.4 in un ambiente
isolato. `melchior688 --missione <SCENA>` gioca una scena. Con la regola di simulazione dell'Enhanced le stesse
missioni girano con la fisica nuova (scoperta, siluri, danni), ma la cadenza degli osservatori, i timer e gli script
restano quelli dell'originale.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Interprete, opcode, lunghezze degli argomenti | disassemblato di `GAME.EXE` (`tools/re/vm_paths.py`, `tok_disasm.py`) | alta (verificate su tutti i byte) |
| Unità di misura del record | valori dei file e tabelle del manuale originale | alta |
| Cadenza: 18 passi per tick, tick di 2 s | gestore dell'interrupt del timer | alta |
| Regola d'impatto e danno | disassemblato, `docs/originale/sensori_e_navigazione.md` §9.8 | alta, salvo dove indicato |
| Lettera C come partita via modem | contenuto delle scene | media (da confermare nel codice di selezione) |
| Posa sul teatro vero | `ricolloca_surf.py`, heightmap del teatro | media (Jaccard delle terre 0,3 circa) |

## 4. Classico ed Enhanced

Questo capitolo è il classico: le missioni originali girano come nel 1989. La **regressione delle 110 partite** (22
scene per 5 semi, capitolo A1) tiene fermi gli esiti: ogni modifica del motore deve lasciare 0 partite diverse.
L'Enhanced cambia la fisica sotto gli stessi script (capitoli C1, D1, B3), non gli script.

## 5. Come si vede in gioco

- **Scelta della missione**: le 22 scene, col lato (688 o Alfa) e il teatro.
- **Briefing**: il testo dell'originale tradotto; le varianti iniziali sono quelle del bytecode, con le loro
  probabilità.
- **Messaggi di bordo**: gli stessi codici dell'originale (7 colpito, 26 siamo colpiti, 53 emersione d'emergenza, 67
  fuoco amico…); i codici da 100 sono l'esito.
- **Esito**: la pagina «CODE: n-x-ACP» dell'originale.

## 6. Taratura e prove

- `tools/re/missioni_lua.py --verifica`: le unità navali in acqua sulla heightmap; `tools/re/verifica_missioni.lua`
  carica ogni file con un `m688` finto, controlla che ogni script citato esista e che le probabilità sommino a 1, ed
  esegue ogni script 200 volte con condizioni casuali. Stato: 22 file, 0 errori.
- `tools/lua/src/luac -p data/missioni/*.lua` (la sintassi) e `tools/re/prova_gestori_parametrici.lua` (i gestori
  con e senza parametri).
- Nel motore: le prove fanno girare le 22 scene per 15 minuti senza errori né funzioni mancanti
  (`test/prove_missioni.cpp`), e `tools/regressione/rilievo_classico.sh` rifà le 110 partite.

## 7. Limiti noti e in arrivo

- Il livello BEGINNER dell'originale (`DS:0331`) e il significato esatto dei parametri `+3A-+3D` sono ipotesi da
  verificare.
- La partita a due via modem (scene C) non c'è nel rifacimento.
- I difetti dell'originale si tengono (il filo tagliato che colpisce il lanciatore, `IF_VARCMP`): correggerli
  cambierebbe gli esiti.
- Nell'angolo della carta originale le unità che entrano più tardi restano «parcheggiate» col timer, come allora.

## 8. Riferimenti

- Documenti: `docs/04_vm_missioni.md` (la VM, gli opcode, le schede), `docs/09_schema_missioni.md` (il formato Lua),
  `docs/originale/sensori_e_navigazione.md` (scoperta, gestori, impatto), `docs/originale/numeri_eseguibile.md`.
- Strumenti: `tools/re/vm_paths.py`, `tok_disasm.py`, `tok_ops.py`, `tok_schede.py`, `missioni_lua.py`,
  `ricolloca_surf.py`, `verifica_missioni.lua`.
- Dati: `data/missioni/SCENE*.lua`, `data/missioni/API.md`.
- Codice: `engine/missioni/` (`Missione`, `missioni/src/missione.cpp`).
