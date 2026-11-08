# La VM delle missioni (file `SCENE*.TOK`)

Stato al 22/10/2026. Le missioni di *688 Attack Sub* non sono dati statici: ogni scena è un
**programma in bytecode** per una piccola macchina virtuale in `GAME.EXE`. Lo stesso bytecode crea
le unità, ne guida il comportamento (IA), gestisce i comandi del giocatore, i timer, le armi, i
messaggi di bordo e l'esito della missione.

Strumenti (tutti in `tools/re/`):

| File | Ruolo |
|---|---|
| `vm_listing.py` | Primo listato grezzo dei gestori (`re/vm_opcodes.asm`, troncato al primo salto) |
| `vm_paths.py` | Esplora tutti i percorsi di ogni gestore e misura quanti byte di argomento consuma; produce `re/vm_opcodes_completo.asm` e `re/vm_lunghezze.json` |
| `tok_ops.py` | Tabella degli opcode: mnemonico, argomenti, effetto, convenzioni sui campi |
| `tok_disasm.py` | Disassemblatore guidato dal flusso; listati in `re/tok/SCENE*.asm` |
| `tok_doc.py` | Genera la tabella degli opcode di questo documento |

```bash
tools/venv/bin/python -I tools/re/vm_paths.py re/unpacked/GAME.EXE re/vm_opcodes_completo.asm re/vm_lunghezze.json
```
```bash
tools/venv/bin/python -I tools/re/tok_disasm.py extracted/raw/DISK1 re/tok re/vm_lunghezze.json
```

Tabelle di supporto estratte da `GAME.EXE`: `re/tok/messaggi.txt` (frasi dei messaggi di bordo per
codice) e `re/tok/tipi_unita.txt` (nomi dei tipi d'unità).

## 1. Architettura dell'interprete

- **Caricamento**: `FUN_1000_1c50` copia `SCENEnX.TOK` a `DS:5105` (buffer da 5 KB).
- **Esecuzione**: `2011:137C` legge un byte in `AL`; `0` termina, altrimenti chiama il gestore
  `word [DS:6505 + 2·(op−1)]` (118 opcode, segmento `2011h`). `SI` punta al bytecode e ogni gestore
  legge da sé i propri argomenti (`lodsb`/`lodsw`); `DI` = **modello dell'unità corrente** a
  `DS:2BEE`.
- **Salti**: `SI = DS:5105 + etichetta`, quindi le etichette sono offset dall'inizio del file.
- **Sottoroutine**: `CALL` (0x01) esegue l'etichetta con un interprete annidato fino a `END`.
- **Unità**: lista collegata a `DS:1F58` di record da 100 byte. `NEW_UNIT` (0x10) inserisce una copia
  del modello con un nuovo id (contatore `DS:5103`, il primo è 0); `STORE` (0x15) riscrive il
  modello nel record dell'unità corrente; `LOAD`/`SELECT_*` fanno l'operazione inversa.
- **Script per unità**: ogni unità ha tre etichette: `+0E` all'arrivo o a fine azione (impostata da
  `GOTO_XY`, `PURSUE`, `LAUNCH`…), `+10` comportamento periodico (`BEHAVIOR`), `+56` reazione.
  Verificato statisticamente: il 100% di questi valori (oltre 900 occorrenze) sono indirizzi di codice.
- **Timer**: `AT tempo, etichetta` (0x14) inserisce un nodo `{tempo, unità, etichetta}` nella lista
  ordinata `DS:50FA`; tempo relativo, o assoluto con il bit 15 (saltato se già passato). `CANCEL` /
  `CANCEL_ALL` rimuovono i timer dell'unità. 1 tick = 2 s.
- **Variabili di scenario**: tabella di word a `DS:64DB` (`VAR_SET`, `VAR_INC`, `IF_VAR_*`).
- **Casualità**: `RANDOM` (0x60) aggiorna il byte `DS:CFD9` (generatore congruenziale × 0xB6BF);
  `IF_RND p, L` salta se `p ≥ casuale`, cioè con probabilità (p+1)/256.
- **Messaggi di bordo**: `MSG c` mette il codice in una coda circolare di 40 elementi (`DS:6045`). Il
  ciclo principale (`FUN_1000_97f8`) li visualizza con la frase al puntatore `DS:0208 + 2c`; i codici
  10–16 vengono rimappati secondo l'arma del tubo; i codici **≥ 100 sono l'esito di fine missione**
  (`c − 100` = pagina "CODE: n-x-ACP" del briefing in `SCENE*.LZA`).

### 1.1 Eventi: come il giocatore comanda il sottomarino

Anche il sottomarino del giocatore è guidato dal bytecode. Il dispatcher a `2011:1F3C–21AC` traduce
un codice di comando (`AL`) nell'esecuzione di un gestore definito dallo script (opcode `HANDLERS_*`)
e **scrive i parametri del comando direttamente negli argomenti della prima istruzione del gestore**.

| Codici | Tabella | Parametri scritti nel gestore |
|---|---|---|
| 1–7 | `HANDLERS_A[1]` (DS:639B) | telegrafo: 999 (arresto rapido), 0, 64, 128, 192, 256, 280 (in 1/256 della velocità massima) |
| 0x08–0x12 | `HANDLERS_A[3]` (DS:639F) | **timone**: `TURN_RATE` (codice − 0x0D) × 2, da −10 a +10 (0x0D = barra al centro), poi `HEAD_TO −1` |
| 0x13–0x1B | `HANDLERS_A[2]` (DS:639D) | **timoni orizzontali**: `DEPTH_TO` 0x4000 (giù, fino alla quota massima) o 0 (su), rateo \|codice − 0x17\| × 64 |
| 0x37, 0x40, 0x41 | `HANDLERS_A[2]` | quota periscopica (`DEPTH_TO` 0xA0, 192), 150 ft (0x4B0, 256), emersione rapida (0, 1024) |
| 0x1D–0x23 | `HANDLERS_B` (7 gestori) | caricamento dei 7 tubi |
| 0x24–0x29 | `HANDLERS_C` (6 gestori) | lancio per tipo d'arma |
| 0x2B | `HANDLERS_A[4]` (DS:63A1) | pilota automatico disinserito |
| 0x2C | `HANDLERS_A[0]` (DS:6397) | waypoint X, Y |
| 0x2D | — | **ping del sonar attivo**: evento acustico come `PING` (0x2A), vedi [sensori_e_navigazione.md](originale/sensori_e_navigazione.md) §4 |
| 0x32–0x33 | `HANDLERS_A[5]` (DS:63A3) | periscopio: byte 0 (giù) o 2 (su) per `F54_2` |
| 0x34, 0x35, 0x36 | `HANDLERS_D[0..2]` (DS:63A7–63AB) | schiera trainata: filare, recuperare, persa |
| 0x38, 0x39 | `HANDLERS_D[3..4]` (DS:63AD–63AF) | bersaglio designato (word); posizione X, Y |
| 0x3E–0x3F | `HANDLERS_D[5]` (DS:63A5) | antenna: byte 0 (giù) o 4 (su) per `F54_4` |
| 0x3A–0x3D | `HANDLERS_E` (DS:63B1–63B5) | guida delle armi filoguidate (0x3D: X, Y nella seconda istruzione) |
| 0x42–0x43 | `HANDLERS_F` (DS:1F6A) | — |

Tabella completa con le conversioni usate nelle missioni Lua in
[docs/originale/sensori_e_navigazione.md](originale/sensori_e_navigazione.md) §7.

## 2. Campi del record d'unità e unità di misura

Le unità di misura sono verificate sui valori dei file e sulle tabelle del manuale originale.

| Campo | Significato | Unità / verifica |
|---|---|---|
| `+02` | id | 0 = giocatore in SCENE1A |
| `+06` | flag: bit `0x18` schieramento (opposto se lo XOR vale `0x18`, neutrale se 0), `0x20`, `0x40` | |
| `+07` | integrità | 0–255; gli attacchi la riducono |
| `+08` | lista dei contatti | |
| `+0E` / `+10` / `+56` | script all'arrivo / di comportamento / di reazione | etichette |
| `+16/+18`, `+1A/+1C` | X, Y a 32 bit (parte intera in `+18`, `+1C`); Y cresce verso nord | **1 nm = 50 unità** (pannello RANGE: √(dx²+dy²)/5 in decimi di nm); **1 pixel della carta = 32 unità** (carta nautica: `coord / 0x20`; riga immagine = 199 − Y/32). Mappa intera 10240 × 6400 unità = 204,8 × 128 nm |
| `+1E` / `+30` | quota attuale / ordinata | **1/8 di piede**: quota massima del 688 = 12000 (1500 ft), dell'Alfa = 14400 (1800 ft), come nel manuale |
| `+20` / `+34` | rotta attuale / ordinata | **9000 = 360°** (2250 = 90°, 4500 = 180°) |
| `+22` / `+32` | velocità attuale / ordinata | **1/350 di nodo** (pannello SPEED: `+22 / 35` in decimi di nodo) |
| `+42` | velocità "avanti tutta" (telegrafo 256); lo sforzo massimo (telegrafo 280) vale × 280/256 | 688: 11840 → 33,8 kt avanti tutta, **37,0 kt** al massimo; Alfa 13440 → **42,0 kt**; Mark 48 17600 → 55 kt; SH-60B 28800 → 90 kt. I valori massimi coincidono con la tabella del manuale |
| `+38`, `+3E`, `+40` | ratei di quota, virata e accelerazione | |
| `+3A–+3D` | parametri dei sensori o di abilità; `+3D` = soglia di probabilità degli attacchi | |
| `+4F` | tipo d'unità | indice della tabella dei nomi `DS:019A` (`re/tok/tipi_unita.txt`) |
| `+58` | contatto bersaglio corrente | |
| `+5A` | quota massima | 1/8 di piede |
| `+5F` | flag di scenario dell'unità | |

### 2.1 Tempo e scala del moto

- **1 tick = 2 s di gioco.** L'orologio a schermo (`FUN_1000_8510`) mostra ore = t/1800, minuti =
  (t/30) mod 60, secondi = 2t mod 60.
- **Ogni tick comprende 18 passi di simulazione.** Il gestore dell'interrupt del timer (`2011:0162`)
  esegue un passo ogni 2 interrupt e conta i passi con `DS:5100`. A ogni passo la posizione a 32 bit
  aumenta di `velocità · sin/cos(rotta) / 65536` (tabella dei seni a 8 bit, `2011:21AD/2206`).
- **La frequenza del timer dipende dalla CPU.** Il divisore del PIT è 2.097.152 / *s* (`FUN_1000_9286`),
  dove *s* (`DS:0FE8`, tra 40 e 256) è un indice di velocità della macchina. Un tick dura quindi da
  circa 0,25 a circa 1,6 s reali.
- **Il moto è accelerato di circa 3,5 volte.** A 1 kt mostrato (350 unità) un'unità percorre 18 × 350 /
  65536 ≈ 0,096 unità per tick, pari a circa 3,5 nm per ora di gioco. È la scelta dichiarata nelle note
  del progettista (Paul Grace): distanza di combattimento "dell'ordine di 15 miglia", portate
  volutamente errate. Esempio: il Mark 48 in 140 tick percorre circa 10–13 nm, come i "10 nm" del manuale.

### 2.2 Modello di scoperta dei sensori (`2011:0CF0–1180`)

Per ogni coppia osservatore/bersaglio:
- **Limiti in unità di posizione, per asse.** Oltre 4000 unità (80 nm) nessuna scoperta. Un bersaglio in
  superficie (quota ≤ 10, cioè 1,25 ft) viene visto automaticamente entro 1800 unità (36 nm), a vista o
  radar. Il sonar attivo (flag `+54`) porta fino a 900 + 450 unità.

> **Correzione (23/10/2026).** Le distanze di questa sezione sono in **quarti di unità** (1 nm = 200):
> 4000 = 20 nm, 1800 = 9 nm. Il 900 + 450 non riguarda il sonar attivo: è l'intercettazione radar/ESM, da
> parte di navi e aerei, del periscopio e dell'antenna alzati (bit 1 e 2 di `+54`; il bit 0 è la schiera
> trainata). Il sonar attivo è un evento acustico. La scoperta passiva esiste solo nel settore di prua
> (±30°, ±90° con la schiera), e la portata è additiva:
> T = G + S·((1 + ⌊8v/v_tutta⌋·f)·N + 40 − ⌊32 v_oss/v_tutta_oss⌋) + E.
> Il modello completo, gli eventi acustici, l'arrivo e i comandi del giocatore sono in
> [docs/originale/sensori_e_navigazione.md](originale/sensori_e_navigazione.md).
- **Segnale = somma di tre termini** (`DS:4764`, `4766`, `4768`):
  - un termine di geometria: rilevamento rispetto alla prua, con settori ciechi;
  - un termine di **rumore del bersaglio**: velocità rispetto alla massima, accelerazione e variazioni di
    quota, moltiplicato per i parametri dei sensori `+3A`/`+3B`;
  - un termine di **eventi** recenti: lanci ed esplosioni, dalla lista `DS:4772`.
- **Termoclini.** Le tre quote impostate con `LAYERS` (0x3A) attenuano il segnale: se osservatore e
  bersaglio stanno da parti opposte di uno strato, il primo termine viene dimezzato e il secondo diviso
  per quattro.
- **Il segnale aggiorna il record contatto** (pool `DS:2C52`, 21 byte): `+04` rilevamento, `+10` livello del
  segnale, `+0E` qualità della traccia (cresce fino a 1200; ≥ 800 = traccia solida, opcode `IF_TRACK_GOOD`),
  `+14` durata del tracciamento (≥ 800 / 1400, `IF_TRACK_MED` / `IF_TRACK_LONG`), `+13` lettera del contatto.

## 3. Tabella degli opcode

Generata da `tools/re/tok_doc.py`. "Byte" è la lunghezza totale, verificata automaticamente contro
il codice dei gestori. "Salto" indica che l'opcode può riposizionare il flusso.

| Op | Mnemonico | Argomenti | Byte | Salto | Gestore | Effetto |
|---|---|---|---|---|---|---|
| `00` | `END` | — | 1 |  | `—` | fine dello script o ritorno da CALL |
| `01` | `CALL` | etichetta | 3 | sì | `2011:1399` | esegue la sottoroutine all'etichetta (interprete annidato fino a END) |
| `02` | `POS` | word, word | 5 |  | `2011:13a1` | posizione X, Y dell'unita' (parte frazionaria azzerata) |
| `03` | `DEPTH_NOW` | word | 3 |  | `2011:13b2` | quota attuale e ordinata in 1/8 di piede (+1E, +30), variazione azzerata |
| `04` | `TYPE` | byte | 2 |  | `2011:13bf` | tipo/classe dell'unita' (+04, byte duplicato) |
| `05` | `FLAGS` | byte | 2 |  | `2011:13c6` | flag dell'unita' (+06): schieramento e proprieta' |
| `06` | `HEAD_NOW` | word | 3 |  | `2011:13cb` | rotta attuale e ordinata = valore (+20, +34), virata azzerata |
| `07` | `SPEED_NOW` | word | 3 |  | `2011:13d8` | velocita' attuale e ordinata in 1/350 di nodo (+22, +32, +2E) |
| `08` | `P_DIVE` | word | 3 |  | `2011:13e3` | parametro di variazione di quota (+38) |
| `09` | `P_TURN` | word | 3 |  | `2011:13e8` | rateo di virata standard (+3E) |
| `0A` | `P_ACCEL` | word | 3 |  | `2011:13ed` | rateo di accelerazione (+40) |
| `0B` | `P_VMAX` | word | 3 |  | `2011:13f2` | velocita' 'avanti tutta' in 1/350 di nodo (+42); sforzo massimo = x 280/256 |
| `0C` | `P_3A` | byte | 2 |  | `2011:13f7` | parametro byte +3A (sensori/abilita') |
| `0D` | `P_3B` | byte | 2 |  | `2011:13fc` | parametro byte +3B (sensori/abilita') |
| `0E` | `P_3C` | byte | 2 |  | `2011:1401` | parametro byte +3C (sensori/abilita') |
| `0F` | `P_SKILL` | byte | 2 |  | `2011:1406` | soglia di probabilita' +3D (confrontata con il numero casuale) |
| `10` | `NEW_UNIT` | — | 1 |  | `2011:140b` | crea un'unita' dal modello corrente con un nuovo id (DS:5103) |
| `11` | `CLEAR` | — | 1 |  | `2011:144c` | azzera lo stato dinamico del modello (contatti, ordini, flag) |
| `12` | `GOTO_XY` | word, word, word, etichetta | 9 |  | `2011:149b` | naviga verso il punto X, Y; soglia d'arrivo +50 in quarti di unità per asse; script all'arrivo (+0E) |
| `13` | `P_4C` | byte | 2 |  | `2011:1494` | parametro byte +4C (duplicato) |
| `14` | `AT` | tempo, etichetta | 5 | sì | `2011:157a` | programma l'esecuzione dell'etichetta dopo/al tempo indicato (timer) |
| `15` | `STORE` | — | 1 |  | `2011:1663` | salva il modello corrente nel record dell'unita' selezionata |
| `16` | `SELECT_CUR` | — | 1 |  | `2011:1420` | riseleziona l'unita' corrente nella lista |
| `17` | `SET_24` | word | 3 |  | `2011:1675` | campo +24 |
| `18` | `SET_2A` | word | 3 |  | `2011:167a` | campo +2A |
| `19` | `TURN_TO` | word | 3 |  | `2011:167f` | rotta ordinata = valore con il rateo standard (+34, +36 = +3E) |
| `1A` | `SPEED_STEP` | word, byte | 4 |  | `2011:168a` | velocita' ordinata (+32) raggiunta a passi (+52 = passo) |
| `1B` | `LOAD` | — | 1 |  | `2011:16a2` | carica nel modello il record puntato da +0A |
| `1C` | `PURSUE` | word, etichetta, etichetta | 7 |  | `2011:1522` | insegue il bersaglio +58 (sul rilevamento finché la traccia è sotto 800, poi sulla posizione stimata); soglia d'arrivo +50 in quarti di unità; script all'arrivo (+0E) e di reazione (+56) |
| `1D` | `PURSUE2` | word, etichetta, etichetta | 7 |  | `2011:154e` | come 0x1C, ma l'arrivo scatta quando la distanza SUPERA la soglia +50 (allontanamento); non uguaglia la quota |
| `1E` | `BEHAVIOR` | etichetta | 3 |  | `2011:16c0` | script del bersaglio (+10): eseguito una volta quando un contatto diventa bersaglio (+58), poi azzerato |
| `1F` | `HANDLERS_A` | gestore, gestore, gestore, gestore, gestore, gestore | 13 |  | `2011:16c5` | gestori di evento: waypoint, telegrafo, timone, quota, 0x2B, 0x32-33 |
| `20` | `TURN_RATE` | word | 3 |  | `2011:1726` | rateo di virata corrente (+36) |
| `21` | `TELEGRAPH` | word | 3 |  | `2011:172b` | velocita' ordinata in 1/256 della massima (999 = arresto rapido, >256 = accelerazione doppia) |
| `22` | `DEPTH_TO` | word, word | 5 |  | `2011:175d` | quota ordinata in 1/8 di piede (limitata da +5A) e rateo in 1/256 di +38 |
| `23` | `MSG_IF_PLAYER` | byte | 2 |  | `2011:1785` | accoda il messaggio di bordo se l'unita' e' il giocatore o lo segue |
| `24` | `HEAD_TO` | word | 3 |  | `2011:1721` | rotta ordinata (+34) |
| `25` | `HANDLERS_B` | gestore, gestore, gestore, gestore, gestore, gestore, gestore | 15 |  | `2011:16de` | gestori di evento 0x1D-0x23 (DS:63B7) |
| `26` | `HANDLERS_C` | gestore, gestore, gestore, gestore, gestore, gestore | 13 |  | `2011:16e8` | gestori di evento 0x24-0x29 (DS:63C5) |
| `27` | `STOP_NAV` | — | 1 |  | `2011:1702` | annulla la navigazione automatica (flag +4E, virata, +5C) |
| `28` | `UNIT_TYPE` | byte | 2 |  | `2011:17bf` | tipo d'unita' (+4F): indice della tabella dei nomi DS:019A |
| `29` | `P_3C2` | byte | 2 |  | `2011:17c4` | campo byte +3C |
| `2A` | `PING` | — | 1 |  | `2011:1843` | impulso del sonar attivo dell'unità corrente: evento acustico per 300 passi (33 s) nella lista DS:4772; gli altri giocatori ricevono il messaggio 8; se l'unità è la 0 incrementa la variabile di scena 0 (prima: `KILLED`) |
| `2B` | `CANCEL` | etichetta | 3 |  | `2011:1873` | annulla i timer dell'unita' corrente diretti all'etichetta |
| `2C` | `CANCEL_ALL` | — | 1 |  | `2011:1877` | annulla tutti i timer dell'unita' corrente |
| `2D` | `LAUNCH` | etichetta, etichetta | 5 |  | `2011:18b7` | prepara nel modello corrente l'arma che NEW_UNIT creera' (non crea l'unita'): soglia d'arrivo 0, script all'arrivo (+0E) e di reazione (+56), inseguimento del bersaglio memorizzato (DS:4762) con il contatto del lanciatore copiato a traccia solida (qualita' 1200) |
| `2E` | `BREAK` | — | 1 |  | `2011:1927` | int 3 (punto di interruzione di debug) |
| `2F` | `F54_2` | byte | 2 |  | `2011:1929` | flag +54, bit 1 (02h): periscopio alzato (comandi 32h/33h) |
| `30` | `F54_1` | byte | 2 |  | `2011:1945` | flag +54, bit 0 (01h): schiera trainata filata, ascolto su ±90°; +3B += 2 (con 0: −2 sempre) |
| `31` | `HANDLERS_D` | gestore, gestore, gestore, gestore, gestore, gestore | 13 |  | `2011:1960` | gestori di evento 0x34-0x39 e 0x3E-0x3F |
| `32` | `NO_BEHAVIOR` | — | 1 |  | `2011:1979` | toglie lo script di comportamento periodico (+10 = 0) |
| `33` | `LINK` | byte | 2 |  | `2011:197f` | collega l'unita' a quella selezionata (+0C, +62) o scollega |
| `34` | `SAVE_TARGET` | — | 1 |  | `2011:1998` | memorizza il bersaglio corrente in DS:4762 |
| `35` | `DEPTH_MAX` | word | 3 |  | `2011:199f` | quota massima in 1/8 di piede (+5A) |
| `36` | `TARGET_ID` | word | 3 |  | `2011:19a4` | seleziona come bersaglio il contatto con l'id indicato (+58) |
| `37` | `SELECT_LAST` | — | 1 |  | `2011:19cb` | **toglie dal gioco** l'unita' corrente DS:475A (chi esegue lo script, salvo selezioni successive): fine corsa, esca esaurita, carica esplosa (`togli_unita`; il nome storico e' sbagliato, `docs/originale/sensori_e_navigazione.md` §9.8) |
| `38` | `HEALTH` | byte | 2 |  | `2011:19d3` | integrita' dell'unita' (+07) |
| `39` | `NAVFLAG` | byte | 2 |  | `2011:19d8` | flag di navigazione +4E (0 = spegne il bit 0) |
| `3A` | `LAYERS` | word, word, word | 7 |  | `2011:19e6` | quote dei tre termoclini (1/8 di piede, DS:1F48): attraversarli attenua il segnale |
| `3B` | `ENGAGE` | byte, etichetta, etichetta | 6 |  | `2011:19f0` | aggancia il bersaglio corrente (+60) e riarma lo scatto dopo n aggiornamenti del contatto (byte); script del bersaglio (+10) e del bersaglio perso (+56) |
| `3C` | `IF_TRACK_LONG` | etichetta | 3 | sì | `2011:1a07` | salta se il contatto bersaglio e' tracciato da molto (+14 >= 1400) |
| `3D` | `IF_TRACK_GOOD` | etichetta | 3 | sì | `2011:1a1d` | salta se la traccia del contatto bersaglio e' solida (+0E >= 800 su 1200) |
| `3E` | `IF_TGT_F20` | etichetta | 3 | sì | `2011:1a33` | salta se l'unita' bersaglio ha il flag 0x20 |
| `3F` | `IF_TGT_ENEMY` | etichetta | 3 | sì | `2011:1a4b` | salta se il bersaglio e' dello schieramento opposto (flag 0x18) |
| `40` | `IF_TGT_F02` | etichetta | 3 | sì | `2011:1a6b` | salta se l'unita' bersaglio ha il flag 0x02 |
| `41` | `IF_TGT_NEUTRAL` | etichetta | 3 | sì | `2011:1a83` | salta se il bersaglio non ha schieramento (flag 0x18 = 0) |
| `42` | `IF_TRACK_MED` | etichetta | 3 | sì | `2011:1a9b` | salta se il contatto bersaglio e' tracciato da un po' (+14 >= 800) |
| `43` | `GOTO` | etichetta | 3 | sì | `2011:1ab1` | salto incondizionato |
| `44` | `IF_RND` | byte, etichetta | 4 | sì | `2011:1ab9` | salta se valore >= numero casuale (DS:CFD9): probabilita' |
| `45` | `IF_ID` | word, etichetta | 5 | sì | `2011:1aec` | salta se l'id dell'unita' corrente = valore |
| `46` | `UFLAGS` | byte | 2 |  | `2011:1b19` | flag di scenario dell'unita' (+5F) = valore |
| `47` | `IF_UFLAGS` | byte, etichetta | 4 | sì | `2011:1b3f` | salta se +5F = valore |
| `48` | `IF_EXISTS` | word, etichetta | 5 | sì | `2011:1b4e` | salta se esiste un'unita' con l'id indicato |
| `49` | `UFLAGS_INC` | — | 1 |  | `2011:1b68` | +5F += 1 |
| `4A` | `IF_ID2` | word, etichetta | 5 | sì | `2011:1b70` | salta se l'id dell'unita' corrente = valore (variante) |
| `4B` | `IF_TGT_ID` | word, etichetta | 5 | sì | `2011:1b7f` | salta se l'id del bersaglio = valore |
| `4C` | `VAR_SET` | variabile, word | 5 |  | `2011:1b98` | variabile di scenario = valore |
| `4D` | `VAR_INC` | variabile | 3 |  | `2011:1ba5` | variabile di scenario += 1 |
| `4E` | `IF_VAR_GT` | variabile, word, etichetta | 7 | sì | `2011:1bb1` | salta se variabile > valore |
| `4F` | `IF_VAR_LT` | variabile, word, etichetta | 7 | sì | `2011:1bc8` | salta se variabile < valore |
| `50` | `IF_VAR_EQ` | variabile, word, etichetta | 7 | sì | `2011:1bdf` | salta se variabile = valore |
| `51` | `VAR_DEC` | variabile | 3 |  | `2011:1c1d` | variabile di scenario -= 1 |
| `52` | `IF_HIT` | word, etichetta | 5 | sì | `2011:1c29` | salta se l'unita' con l'id indicato e' colpita o distrutta (+5E, +55) |
| `53` | `IF_TGT_NEAR` | word, etichetta | 5 | sì | `2011:1c4f` | salta se il bersaglio e' entro la distanza indicata in X e in Y (50 unita' = 1 nm) |
| `54` | `ATTACK` | word s.s., byte | 4 |  | `2011:1cc1` | attacco al bersaglio: la portata e' ignorata; riesce se DS:5100 (18..1) > byte (0 = +3D); danno fisso 30 (§9.8 del documento sull'originale) |
| `55` | `MSG` | byte | 2 |  | `2011:1d6c` | accoda un messaggio di bordo (coda DS:6045); codici >= 100 = esito di fine missione |
| `56` | `IF_TGT_LOCKED` | etichetta | 3 | sì | `2011:1d71` | salta se il bersaglio punta l'ultima unita' trovata |
| `57` | `CONTACTS_MARK` | — | 1 |  | `2011:1c83` | marca tutti i contatti dell'unita' (+16 = 1) |
| `58` | `HANDLERS_E` | gestore, gestore, gestore | 7 |  | `2011:1d93` | gestori di evento 0x3A-0x3C (DS:63B1) |
| `59` | `SELECT_TGT` | — | 1 |  | `2011:1da0` | seleziona come corrente l'unita' bersaglio (se non esiste termina) |
| `5A` | `MISSILE` | — | 1 |  | `2011:17c9` | lancio di missile dell'unità corrente: evento acustico per 900 passi (100 s), messaggio 0x30 agli altri giocatori (prima: `LOST`) |
| `5B` | `RETARGET` | — | 1 |  | `2011:180e` | trasferisce i riferimenti dall'unita' +62 all'ultima trovata |
| `5C` | `F54_4` | byte | 2 |  | `2011:1937` | flag +54, bit 2 (04h): antenna alzata (comandi 3Eh/3Fh) |
| `5D` | `IF_IN_RECT` | word, word, word, word, etichetta | 11 | sì | `2011:1de1` | salta se X >= x0, Y <= y0, X <= x1, Y >= y1 |
| `5E` | `IF_TGT_F03` | etichetta | 3 | sì | `2011:1e08` | salta se l'unita' bersaglio ha i flag 0x03 |
| `5F` | `UFLAGS_DEC` | — | 1 |  | `2011:1b6c` | +5F -= 1 |
| `60` | `RANDOM` | — | 1 |  | `2011:1aca` | nuovo numero casuale in DS:CFD9 |
| `61` | `TURN_TO_TGT` | — | 1 |  | `2011:1e25` | rotta verso il rilevamento del bersaglio |
| `62` | `NAV_ON` | — | 1 |  | `2011:1711` | attiva la navigazione verso il punto (flag +4E = 4) |
| `63` | `TURN_REL` | word | 3 |  | `2011:16f2` | virata relativa: rotta + valore (modulo 9000) |
| `64` | `GOTO_TGT` | word, etichetta | 5 |  | `2011:14c3` | naviga verso la posizione del bersaglio; soglia d'arrivo +50 in quarti di unità per asse, script all'arrivo (+0E) |
| `65` | `IF_VARCMP` | variabile, variabile, etichetta, etichetta | 9 | sì | `2011:1bf6` | confronta due variabili: maggiore -> seconda etichetta; uguali -> prima etichetta + 2 (difetto: riprende a meta' istruzione) |
| `66` | `MATCH_DEPTH` | — | 1 |  | `2011:1e4c` | quota ordinata = quota del bersaglio (limitata) |
| `67` | `MATCH_SPEED` | — | 1 |  | `2011:1e79` | velocita' ordinata = velocita' del bersaglio |
| `68` | `IF_PLAYER1` | etichetta | 3 | sì | `2011:1afb` | salta se il giocatore e' l'unita' 1 (DS:0364 = 1) |
| `69` | `IF_STANDARD` | etichetta | 3 | sì | `2011:1b0a` | salta se DS:0331 = 0, cioe' livello STANDARD (ipotesi: 0331 != 0 = BEGINNER) |
| `6A` | `ATTACK_RND` | byte | 2 |  | `2011:1c96` | colpo sull'unita' corrente DS:475A se un byte casuale e' >= +3D; messaggio 50; danno = byte |
| `6B` | `SELECT_LINKED` | — | 1 |  | `2011:1dcd` | seleziona il bersaglio (+58) dopo averne scritto il +62 con l'unita' corrente |
| `6C` | `IF_X_GT` | word, etichetta | 5 | sì | `2011:1dd2` | salta se X dell'unita' > valore |
| `6D` | `IF_UFLAG_BIT` | etichetta, byte | 4 | sì | `2011:1b30` | salta se +5F AND maschera != 0 (etichetta prima della maschera) |
| `6E` | `UFLAG_SET` | byte | 2 |  | `2011:1b1e` | +5F OR= maschera |
| `6F` | `UFLAG_CLR` | byte | 2 |  | `2011:1b23` | +5F AND= NOT maschera |
| `70` | `CLR_2C` | — | 1 |  | `2011:1b2a` | campo +2C = 0 (arresta la variazione di quota) |
| `71` | `TURN_AWAY` | — | 1 |  | `2011:1e33` | rotta opposta al rilevamento del bersaglio (+180 gradi) |
| `72` | `JUMP_TO_TGT` | — | 1 |  | `2011:14fc` | teletrasporta l'unita' vicino al bersaglio (+45, +45) |
| `73` | `TRANSIENT` | — | 1 |  | `2011:17f1` | rumore transitorio dell'unità corrente (lancio di siluro, manovra): evento acustico per 100 passi (11 s) (prima: `MARK_100`) |
| `74` | `SELECT_ID` | word | 3 |  | `2011:163b` | unita' corrente per i comandi successivi = id (DS:5CBD) |
| `75` | `IF_TAMPERED` | etichetta | 3 | sì | `2011:15d2` | salta se la memoria DOS non corrisponde ai valori attesi (anti-manomissione) |
| `76` | `HANDLERS_F` | gestore, gestore | 5 |  | `2011:15c9` | gestori di evento 0x42 e 0x43 (DS:1F6A) |

## 4. Validazione del disassemblatore

Su tutti i **22 file** `SCENE*.TOK` (42.208 byte):
- **ogni byte si decodifica** in un'istruzione valida, coperto esattamente una volta, con nessuna
  istruzione sovrapposta e tutte le etichette all'inizio di un'istruzione: zero byte di dati, zero
  anomalie;
- il flusso parte dall'offset 0 e da tutte le etichette, cioè salti, `CALL`, timer, gestori di evento e
  script delle unità: raggiunge **14.324 istruzioni (96% dei byte)**;
- il restante 4% (1.565 byte) è **codice orfano**: blocchi completi che finiscono con `END` e che nessuna
  word del file cita. Sono avanzi degli autori, per esempio una variante di rotta in SCENE9C a
  0x021B–0x022A. SCENE5C è coperta al 100% dal flusso.

Le lunghezze degli argomenti sono misurate sul codice da `vm_paths.py`, che tiene conto di
`push si`/`pop si` e delle riassegnazioni di `SI` (i cicli sulle liste collegate non leggono il
bytecode). Le uniche sorprese:
- **0x14 `AT`**, **0x2B `CANCEL`**, **0x2C `CANCEL_ALL`**: i cicli su liste facevano sembrare variabile la
  lunghezza, che invece è fissa (4, 2 e 0 byte);
- **0x75 `IF_TAMPERED`**: confronta 45 byte della memoria DOS (segmento 0, offset 05BF, 08A7, 5A7F, 5B06,
  5DD0, 5E2C) con valori attesi; è un **controllo anti-manomissione**. In SCENE1A, se fallisce, il
  sottomarino del giocatore viene sabotato (integrità 10, sensori al minimo) con il messaggio
  "adjusting engine speed".

14 opcode sono implementati ma **mai usati** nelle missioni originali: 16, 17, 18, 19, 1A, 1B, 1D, 2E,
32, 41, 42, 4A, 4F, 5E.

### 4.1 Difetto dell'originale: `IF_VARCMP` (0x65)

Il gestore confronta due variabili: se la prima è maggiore salta alla seconda etichetta; **se sono
uguali salta alla prima etichetta + 2**. Nei due usi (SCENE1B a 0x0560, SCENE1C a 0x03A8) le due
etichette coincidono: l'autore intendeva "se a ≥ b vai a L". In caso di parità l'interprete riprende
a metà dell'istruzione successiva. Per esempio in SCENE1C a 0x03BC esegue `03 55 6A`, cioè
`DEPTH_NOW 0x6A55`, poi `END`: **l'esito della missione non viene comunicato**. Il disassemblatore
segue l'etichetta scritta e annota il difetto nel listato.

## 5. Esempio: SCENE1A, TORPEX '89 (lato 688), ricostruita dal bytecode

### 5.1 Unità create

| Id | Tipo (`+4F`) | Creazione | Comportamento |
|---|---|---|---|
| 0 | 0x14 US 688 (giocatore) | Una di 3 posizioni a caso: (5760, 1568) rotta 180°; (5568, 1376) 130°; (5184, 2112) 280°. Quota 20 ft, ferma; massimi 37 kt e 1500 ft | comandata dal giocatore tramite gli eventi |
| 1 | 0x19 BIOLOGICAL | (5440, 1888), quota 150 ft, ferma, massimo 1 kt | **una balena**: rumore biologico |
| 2, 3, 4 | 0x00 Forrest Sherman | (5216, 1792), (5312, 1696), (5440, 1600); 10 kt, massimo 25 kt | rotta verso nord-ovest: l'unità 2 va a (5120, 1920), le unità 3 e 4 a (4096, 2944), poi si fermano. Comportamento `L_029B`, descritto sotto |
| 5 | 0x12 SH-60B LAMPS III | (4096, 1696), 90 kt | pattuglia avanti e indietro fra (4096, 1696) e (6688, 2304), con verso iniziale casuale. Comportamento `L_0190` |
| 6 | 0x15 **Alfa sovietico** | **solo nel 50% delle partite** (`IF_RND 0x80`, `var[3] = 1`): (5024, 2176), quota 400 ft, rotta 270°, ferma | comportamento `L_0214`: pedina il giocatore |

Al livello BEGINNER (`IF_STANDARD`, ipotesi) il giocatore riceve integrità 255 e parametri dei
sensori migliori.

### 5.2 Intelligenza delle unità

- **Cacciatorpediniere (`L_029B`)**:
  - se il bersaglio è un'arma in arrivo (flag 0x20) accelerano alla massima (telegrafo 270);
  - se il contatto è il giocatore sospendono la navigazione, accostano verso di lui e rallentano a 10/256
    della velocità per ascoltare, riprendendo la rotta dopo 40 s;
  - se la traccia è solida (`+0E ≥ 800`) e il contatto è vicino (entro 90 unità, cioè 1,8 nm) o tracciato da
    molto (`+14 ≥ 1400`), il sottomarino è
    **scoperto**: il SOSUS emette tre volte "we are being pinged hard sir" e poi "we were detected", e
    l'esercitazione termina con la valutazione.
- **Condivisione dei contatti**: le navi eseguono `CONTACTS_MARK` ogni 24 s, sfalsate di 30, 60 e 90 s
  all'avvio. È una sorta di collegamento dati tattico.
- **Elicottero (`L_0190`)**:
  - quando uno dei cacciatorpediniere 2, 3 o 4 viene colpito, l'elicottero interrompe la pattuglia e vola
    sul punto (`GOTO_TGT`);
  - lì gira in tondo (virata continua, rateo 5) e cerca; se il contatto è il giocatore accosta e rallenta,
    e con un contatto forte scatta la scoperta;
  - dopo 2 minuti torna alla pattuglia.
- **Alfa (`L_0214`)**:
  - **pedina** il giocatore uguagliandone velocità e quota quando è entro 400 unità (8 nm);
  - se un siluro la punta, **evade**: accostata di 180° alla massima per 60 s, poi metà velocità;
  - **se il giocatore ha già lanciato** (`var[1] > 0`, incrementata a ogni lancio) mentre l'Alfa è entro
    400 unità, **risponde con 3 siluri da 533 mm**. Un flag impedisce di ripetere la risposta per 4 minuti.

### 5.3 Timer ed esito

- a +200 s parte un controllo ogni 10 s: se l'Alfa era presente ed è stata distrutta → **esito 3**
  ("…START WORLD WAR THREE?"); se tutti e tre i cacciatorpediniere sono affondati → **esito 8**;
- a +240 s c'è il controllo anti-manomissione;
- a +900 s (15 minuti) arriva "times up captain" e la valutazione in base ai cacciatorpediniere rimasti:
  0 → **esito 8** "OUTSTANDING PERFORMANCE", 1 → **7** "AFTER SINKING TWO SHIPS", 2 → **6** "SINKING JUST ONE
  SHIP… SUBSTANDARD", 3 → **5** "LACKLUSTER (NO SHIPS DAMAGED AT ALL!)";
- **1** (incagliato) e **2** (affondato dai siluri dopo aver provocato il sottomarino sovietico)
  vengono dal motore.

### 5.4 Confronto con il briefing (`docs/originale/briefing_EN.txt`)

| Briefing | Bytecode |
|---|---|
| tre Forrest Sherman radiati diretti a nord-ovest | 3 unità tipo 0x00 da (≈5300, ≈1700) verso (4096, 2944): X diminuisce, Y aumenta |
| autopilota telecomandato, manovre semplici | rotta a waypoint, poi stop; ascolto e accostata verso il contatto |
| un elicottero LAMPS III cercherà il sottomarino | unità 5 SH-60B con pattuglia e ricerca dove una nave viene colpita |
| il SOSUS fa da arbitro e chiude con tre colpi di sonar alla scoperta | messaggi 0x24 ×3 e 0x20, poi la valutazione |
| l'esercitazione termina dopo 15 minuti | `AT +450` (900 s) "times up captain" |
| (non menzionato) | **Alfa sovietico nascosto nel 50% delle partite**, che pedina e reagisce agli attacchi; e una **balena** |

Il briefing tace sull'Alfa, ma le pagine d'esito 2 ("PROVOKED A SOVIET SUB") e 3 ("START WORLD WAR
THREE?") la prevedono: è una sorpresa voluta dai progettisti.

## 6. Questioni aperte

- Risolte il 23/10/2026: scala delle coordinate e del tempo (§2, §2.1) e campi del record contatto (§2.2).
- **Ipotesi da verificare:**
  - i parametri `+3A–+3D` (sensori, abilità, rumore?);
  - ~~i flag `+54`~~: risolto il 23/10/2026 (schiera trainata, periscopio, antenna; `docs/originale/sensori_e_navigazione.md`);
  - il livello BEGINNER (`DS:0331`).
- ~~Verificare la funzione esatta dei termini del segnale~~: risolto il 23/10/2026, formule complete in
  `docs/originale/sensori_e_navigazione.md` §3.
- Il suffisso C delle scene è con ogni probabilità la partita a due giocatori via modem (§7): da confermare
  nel codice di selezione della scena (`FUN_1000_1300` circa, scelta della lettera `'A' + DS:032F`).
- ~~Convertire le schede (§7) nel formato Lua del nuovo motore~~: fatto, `data/missioni/*.lua`
  (`docs/09_schema_missioni.md`).

## 7. Schede di tutte le scene

Generate da `tools/re/tok_schede.py`, che esegue in modo simbolico lo script di inizializzazione:
- segue CALL e salti;
- esplora tutti i rami casuali `IF_RND` con la probabilità esatta (si tiene l'intervallo del numero
  casuale, perché più test senza un nuovo `RANDOM` sono correlati);
- fotografa il modello a ogni `NEW_UNIT`;
- ricostruisce le rotte dalle catene di waypoint e classifica i comportamenti dagli opcode raggiungibili.

Livello STANDARD, giocatore = id 0. In tutte le 22 scene la somma delle probabilità delle combinazioni
vale esattamente 1.

Per ogni scena, in `re/tok/schede/`:
- `SCENE*.json`: unità con presenza, posizioni marginali, timer, variabili, esiti con il testo del
  briefing e configurazione;
- `SCENE*.md`: scheda leggibile;
- `SCENE*.png`: unità e rotte sulla carta SURF originale.

Lettere delle scene: A = giocatore sul 688; B = giocatore sull'Alfa (in TORPEX il secondo 688, come dice il
manuale per la partita via modem); C = **probabile partita a due giocatori via modem**: entrambi i
sottomarini compaiono in testa e gli script usano `IF_PLAYER1`.

| Scena | Missione | Lato | Teatro | Forze (unita' create all'avvio, presenza < 100% tra parentesi) | Combinazioni | Esiti nello script |
|---|---|---|---|---|---|---|
| [SCENE0A](../re/tok/schede/SCENE0A.md) | 10 HIT AND RUN | lato 688 | baltico | Us 688 Ssn, Cruise Missile Target, Soviet Alfa Ssn, Victor Iii Fast Attack Sub, Akula Fast Attack, Kirov, Slava Cruiser, Kara Guided Missile Cruiser, Krivak Ii, Biological | 4 | 3, 4, 5, 6 |
| [SCENE1A](../re/tok/schede/SCENE1A.md) | 1 TORPEX '89 | lato 688 | faroe | Us 688 Ssn, Biological, 3× Forrest Sherman Destroyer, Seahawk-60B Lamps Iii Helicoptor; Soviet Alfa Ssn (50%) | 12 | 3, 5, 6, 7, 8 |
| [SCENE1B](../re/tok/schede/SCENE1B.md) | 1 TORPEX '89 | lato Alfa | faroe | 2× Us 688 Ssn, Biological, 5× Forrest Sherman Destroyer | 4 | 3, 4, 5, 6, 7 |
| [SCENE1C](../re/tok/schede/SCENE1C.md) | 1 TORPEX '89 | due giocatori (ipotesi) | faroe | 2× Us 688 Ssn, 5× Forrest Sherman Destroyer | 4 | 5, 6, 7, 8 |
| [SCENE2A](../re/tok/schede/SCENE2A.md) | 2 SHAKE 'EM | lato 688 | faroe | Us 688 Ssn, Biological, Soviet Alfa Ssn, 3× Merchant Ship, Victor Iii Fast Attack Sub | 4 | 4, 5, 6, 7 |
| [SCENE2B](../re/tok/schede/SCENE2B.md) | 2 SHAKE 'EM | lato Alfa | faroe | Soviet Alfa Ssn, Biological, Us 688 Ssn, 3× Merchant Ship | 2 | 3, 4, 5, 6, 7 |
| [SCENE2C](../re/tok/schede/SCENE2C.md) | 2 SHAKE 'EM | due giocatori (ipotesi) | faroe | Us 688 Ssn, Soviet Alfa Ssn, 3× Merchant Ship, Biological | 2 | 5, 6, 7, 8, 9, 10 |
| [SCENE3B](../re/tok/schede/SCENE3B.md) | 3 SUB OPS | lato Alfa | barents | Soviet Alfa Ssn, Biological, Trafalgar Fast Attack, Slava Cruiser, Koni Frigate, 2× Merchant Ship, Charlie Ii Guided Missile Sub | 48 | 3, 4, 5, 6, 7, 8 |
| [SCENE4A](../re/tok/schede/SCENE4A.md) | 4 MUMAR CADAVER | lato 688 | gibilterra | Us 688 Ssn, Biological, 2× Foxtrot Diesel Sub, 4× Tanker, Merchant Ship, 2× Koni Frigate | 10 | 5, 6, 7, 8, 9 |
| [SCENE5A](../re/tok/schede/SCENE5A.md) | 5 ESCAPE | lato 688 | gibilterra | Us 688 Ssn, 4× Biological, Soviet Alfa Ssn, 2× Victor Iii Fast Attack Sub, Akula Fast Attack, 2× Merchant Ship | 32 | 3, 4, 5, 6, 7, 8, 9, 10 |
| [SCENE5B](../re/tok/schede/SCENE5B.md) | 5 ESCAPE | lato Alfa | gibilterra | Soviet Alfa Ssn, 4× Biological, Us 688 Ssn, 2× Merchant Ship | 8 | 3, 4, 5, 6, 7, 8 |
| [SCENE5C](../re/tok/schede/SCENE5C.md) | 5 ESCAPE | due giocatori (ipotesi) | gibilterra | Us 688 Ssn, Soviet Alfa Ssn, 3× Biological, 2× Merchant Ship | 8 | 5, 6, 7, 8, 9, 10, 11, 12 |
| [SCENE6A](../re/tok/schede/SCENE6A.md) | 6 GOULASH | lato 688 | otranto | Us 688 Ssn, Biological, Soviet Alfa Ssn, 5× Cargo Ship, Kirov, Slava Cruiser | 1 | 3, 4, 5, 7, 8, 9 |
| [SCENE7A](../re/tok/schede/SCENE7A.md) | 7 CAT WALK | lato 688 | kunashir | Us 688 Ssn, Biological, Typhoon Ssbn, 3× Merchant Ship, Seahawk-60B Lamps Iii Helicoptor, Soviet Alfa Ssn | 4 | 5, 6 |
| [SCENE7B](../re/tok/schede/SCENE7B.md) | 7 CAT WALK | lato Alfa | kunashir | Soviet Alfa Ssn, Biological, Typhoon Ssbn, 3× Merchant Ship, Seahawk-60B Lamps Iii Helicoptor, Us 688 Ssn | 4 | 5, 6, 7 |
| [SCENE7C](../re/tok/schede/SCENE7C.md) | 7 CAT WALK | due giocatori (ipotesi) | kunashir | Us 688 Ssn, Soviet Alfa Ssn, Typhoon Ssbn, 3× Merchant Ship, Seahawk-60B Lamps Iii Helicoptor, Biological | 4 | 5, 6, 7, 8, 9, 10 |
| [SCENE8A](../re/tok/schede/SCENE8A.md) | 8 SURPRISE PARTY | lato 688 | celtica | Us 688 Ssn, Biological, Nimitz Aircraft Carrier, Iowa Battleship, Sacramento Oiler, Oliver Hazard Perry Frigate, Kidd Guided Missile Destroyer, Soviet Alfa Ssn | 10 | 3, 4, 5, 6, 7 |
| [SCENE8B](../re/tok/schede/SCENE8B.md) | 8 SURPRISE PARTY | lato Alfa | celtica | Soviet Alfa Ssn, Biological, Nimitz Aircraft Carrier, Iowa Battleship, Sacramento Oiler, Oliver Hazard Perry Frigate, Kidd Guided Missile Destroyer, Us 688 Ssn | 10 | 5, 6, 7 |
| [SCENE8C](../re/tok/schede/SCENE8C.md) | 8 SURPRISE PARTY | due giocatori (ipotesi) | celtica | Us 688 Ssn, Soviet Alfa Ssn, Nimitz Aircraft Carrier, Iowa Battleship, Sacramento Oiler, Oliver Hazard Perry Frigate, Kidd Guided Missile Destroyer, Biological | 10 | 5, 6, 7, 8, 9, 10 |
| [SCENE9A](../re/tok/schede/SCENE9A.md) | 9 HOMECOMING | lato 688 | celtica | Us 688 Ssn, Biological, 5× Cargo Ship, 2× Oliver Hazard Perry Frigate, Soviet Alfa Ssn, Oscar (Ssgn) Cruise Missile Sub | 576 | 5, 6, 7, 8 |
| [SCENE9B](../re/tok/schede/SCENE9B.md) | 9 HOMECOMING | lato Alfa | celtica | Soviet Alfa Ssn, Biological, 5× Cargo Ship, 2× Oliver Hazard Perry Frigate, Us 688 Ssn, Oscar (Ssgn) Cruise Missile Sub | 96 | 5, 6, 7 |
| [SCENE9C](../re/tok/schede/SCENE9C.md) | 9 HOMECOMING | due giocatori (ipotesi) | celtica | Us 688 Ssn, Soviet Alfa Ssn, 5× Cargo Ship, 2× Oliver Hazard Perry Frigate, Biological, Oscar (Ssgn) Cruise Missile Sub | 6 | 5, 7, 8, 9, 10, 11 |

Verifiche visive sulle carte:
- **SCENE1A:** le unità cadono all'estremità sud dell'arcipelago delle Faroe (stilizzato e ruotato
  nella carta SURFG), come dice il briefing, e i cacciatorpediniere risalgono il canale verso nord-ovest;
- **SCENE4A:** il convoglio libico (4 petroliere, 2 Koni, 2 Foxtrot) è all'uscita occidentale dello
  stretto di Gibilterra.


## 8. Prestazioni delle unità e delle armi

Nell'originale non esiste una tabella delle classi in `GAME.EXE`. Ogni scena costruisce il "modello
corrente" con gli opcode `P_*` e lo copia nell'unità con `NEW_UNIT`. Il tipo (`UNIT_TYPE`, `+4F`) serve
solo al nome: la simulazione del segmento 2011 non lo legge, e l'unico altro uso è `1000:99BC`, che
riconosce i siluri. I campi che una scena non imposta restano quelli dell'unità precedente.
`tools/re/prestazioni.py` raccoglie i modelli da tutte le scene e dai gestori di lancio delle armi:
tabelle in [docs/originale/prestazioni.md](originale/prestazioni.md), dati in `prestazioni.json`.

**Conversioni.** Il ciclo di moto (`2011:083E`) gira a ogni passo di simulazione, 18 per tick e quindi 9
per secondo di gioco. Virata (`+3E`) in unità di rotta per passo, cioè 0,36°/s per unità; accelerazione
(`+40`) in 1/350 di nodo per passo; quota (`+38`) in 1/8 di piede per passo, a comando pieno. Distanza:
nm = nodi × tick × 0,00192, perché il moto è accelerato di circa 3,5 volte.

**Sensori e difesa** (`2011:0DC8`, `0EEB–0F90`, `0B45`, `1C96`):
- `+3A` è la rumorosità irradiata. Moltiplica il termine di rumore del bersaglio: velocità / velocità di
  avanti tutta × 8, con il fattore `DS:1F64` che sale da 1 a 3 oltre avanti tutta.
- **Difetto dell'originale.** Quando il bersaglio cambia velocità (`+2E` ≠ 0), il ramo `0EEB–0F17` porta
  il fattore a 4 (5 a meno di 250 ft) e prende la velocità ordinata, ma il salto in `0F17` va a `0F54` e
  scavalca sia la moltiplicazione sia il ×`+3A`. `DS:1F64` non è usato altrove. Così un'unità che accelera
  o rallenta resta con il termine di rumore a 1, quasi inudibile, mentre l'intenzione era il contrario. Il
  remake non lo riproduce.
- `+3B` è la sensibilità del sonar di chi ascolta. Moltiplica 40 − velocità propria × 32 / massima. Se uno
  dei due vale 0 non c'è scoperta: navi mercantili e Koni (`3B` = 0) non sentono nulla.
- `+3D` per le armi è il danno all'impatto, tolto all'integrità (`+07`) e dimezzato in 7 passi su 18 (§9.8). Per
  le unità è la difesa contro gli attacchi a esito casuale (`ATTACK_RND`): il colpo riesce se il numero
  casuale è ≥ `+3D`.

**Risultati principali**
- 688: massima forza 37 kn, 1500 ft, virata 3,6°/s, rumore 6, sonar 18. Alfa: 42 kn, 1800 ft, rumore 12.
  Coincidono con il manuale.
- **Typhoon, Charlie II, Oscar, Victor III e Akula usano il modello dell'Alfa** (subroutine comune, per
  esempio `L_04AA` in SCENE7A): 42 kn e 1800 ft. Il manuale dà 24–35 kn. Cambiano solo nome, rumore e
  integrità. Il Foxtrot ha un modello suo: 20 kn, 600 ft, accelerazione dimezzata.
- Le navi virano a 1,44°/s e accelerano di 0,05 kn/s. La velocità è quella del manuale solo per Kidd e
  Perry; Nimitz e Iowa vanno a 15 kn (il manuale dice 33 e 35).
- **Tipi d'arma** (= gestori HANDLERS_C, codice di lancio 24h + tipo, 1000:7A9F): 0 Mk 48, 1 Type 53,
  2 Sea Lance, 3 Tomahawk, 4 Harpoon, 5 esca acustica. Il tipo di ogni tubo e la dotazione stanno nel blocco
  di configurazione di `SCENE*.LZA` (byte 7–13 e 1–6) e coincidono con le dotazioni del manuale.
  Icone dei tubi (stringhe di glifi del font a DS:02E7) in `re/cls/icone_armi.png`.
- **Armi.** Mk 48 a 50,3 kn per 280 s (13,5 nm). Il filo di guida si taglia a 190 s (9,2 nm): `LINK 0` e
  la testa passa da 17 a 13. Il 533 mm sovietico va a 45,7 kn per 240 s (10,5 nm). Mk 50, Spearfish e
  Sea Lance hanno danno 40 contro 75–100.
- **I missili non sono simulati come oggetti.** Dopo 10–20 s c'è un `ATTACK_RND` sul bersaglio, che riceve il timer da `SELECT_LINKED`, con
  danno 75 (100 in alcune scene), riuscito con probabilità (256 − `3D`) / 256: Kara, Kirov e Slava 65%,
  Perry, Kidd e Nimitz circa 94%.
- **Esca acustica.** Nasce silenziosa e dopo 2 s porta il rumore a 40 (più di qualsiasi nave) e devia i
  riferimenti dei siluri (`RETARGET`); dura 160 s (60 in alcune scene).
- L'elicottero SH-60B va a 70–90 kn (il manuale dice 126), con sonar 18–55 e rumore 0: non si sente.
