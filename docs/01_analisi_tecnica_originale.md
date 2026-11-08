# 688 Attack Sub (EA, 1989): analisi tecnica dell'originale

Stato al 22/10/2026. Documento vivo: viene aggiornato a ogni scoperta.

## 1. Contenuto della distribuzione

| File | Dimensione | Ruolo |
|---|---|---|
| `688.BAT` | 351 | Lancia `GAME n` in base all'ERRORLEVEL restituito da `TITLE.EXE` (1–6) |
| `TITLE.EXE` | 16.869 | Schermata titolo e selezione. Turbo C 2.0 + EXEPACK; contiene simboli di debug (moduli `TITLE.C`, `DECODE.ASM`, `FBLOAD.ASM`, `SETEGA.ASM`, `HERC_CGA.ASM`, `PALETTE.ASM`) |
| `GAME.EXE` | 104.572 | Motore di gioco. Turbo C 2.0 (Borland TLINK) + **Microsoft EXEPACK** |
| `CALLS.EXE` | 64.861 | Driver audio digitalizzato (Tandy/PC speaker, campioni vocali). EXEPACK |
| `DISK1.MLB`, `DISK2.MLB` | 157K / 342K | Archivi risorse (55 + 150 voci) |
| `688.PAK/PAL`, `ALFA.PAK/PAL`, `TITLE.PAK/PAL` | 64.000 / 768 | Schermate VGA 320×200×256 e relative palette |
| `MAIN.EGA` | 5.807 | Schermata dei titoli per EGA (immagine ENC 319×199, 16 colori, palette EGA standard), caricata da `TITLE.EXE` al posto di `TITLE.PAK`; estratta in `extracted/png/MAIN_EGA.png` |
| `MODEM.688` | 18 | Stringa di init modem `ATV1E0X3` |
| `PROFILE.688` | 19 | Configurazione salvata |

Compilatore riconosciuto dalla firma `01 00 FB 20 72 6A` (TLINK 2.x) e dalla stringa
`Turbo-C - Copyright (c) 1988 Borland Intl.` a `DS:0004`. Il segmento dati (DGROUP) di
`GAME.EXE` decompresso è `0x1306`.

## 2. Formati ricostruiti (con strumenti funzionanti)

### 2.1 Archivi `.MLB` (`tools/re/mlb.py`)
```
u16 count, u16 ?(=6)
count × { u32 offset, u32 size, char name[13] }   // nome 8.3 terminato da NUL, padding sporco
```

### 2.2 Eseguibili EXEPACK (`tools/re/unexepack.py`)
Decompressore EXEPACK scritto da zero: decodifica all'indietro i comandi `0xB0` (fill) e
`0xB2` (copy), ricostruisce la tabella di rilocazione (16 sezioni da 64K) e produce un MZ
pulito in `re/unpacked/`. Senza questo passo Ghidra analizzava codice ancora compresso.

### 2.3 Immagini `.PAK` + `.PAL`
Bitmap grezza 320×200 a 8 bpp (mode 13h). Palette 256×RGB a **8 bit** (non i 6 bit VGA).

### 2.4 Sprite `.ENC` (`tools/re/assets.py`)
```
u16 larghezza, u16 altezza-1
RLE su byte: ctrl & 0x80 -> ripeti il byte seguente (ctrl & 0x7F) + 3 volte
             altrimenti  -> copia ctrl + 1 byte letterali
2 pixel per byte (nibble alto = pixel sinistro), righe dal basso verso l'alto
```
I nibble sono indici relativi a un **banco di palette**:
- banco 0: i 16 colori EGA, per l'interfaccia (pulsanti, finestre, frecce);
- banco 144: rampa di 16 grigi, per le foto digitalizzate monocromatiche (navi, elicotteri,
  missili, fumo, equipaggio). Il nibble 15 delle sagome è trasparente.

Le navi hanno 5 viste (0–4, probabilmente prua, ¾, fianco, ¾ poppa, poppa); la Nimitz ne ha 8.

### 2.5 Compressione `.LZW` / `.LZA` (`tools/re/lzw.py`)
Ricostruita dall'assembly a `2283:000D` di `GAME.EXE`:
```
u16 numero_codici, u16 lunghezza_output, poi flusso di bit LSB-first
primo codice a 8 bit, poi 9..12 bit (larghezza++ quando next >= limite)
nessun codice di clear: il dizionario (3 array: suffisso, prefisso lo/hi) si
reinizializza da solo quando next raggiunge 0x1000
```
Verificato su tutti i 16 `.LZW` e i 21 `.LZA`: lunghezze identiche a quelle dichiarate.
L'output di un `.LZW` è un'immagine in formato ENC.

| Risorsa | Contenuto |
|---|---|
| `MAIN.LZW` | Centrale di comando del 688 (320×200) |
| `ALFA.LZW` | Centrale di comando dell'Alfa |
| `SURFA..SURFG.LZW` | **Mappe batimetriche dei 7 teatri** (320×200, 16 livelli; livello alto = acqua bassa o costa). La lettera è scelta in `FUN_1000_1f01` come `'A' + indice_mappa`; associazione certa ai teatri in `docs/02_pipeline_dati.md` §4 |
| `END0.LZW` | Scrivania di fine missione |
| `END1..4.LZW` | Cartoline e lettere di fine partita (205×134) |
| `TARGETS.LZW` | **Non usato.** LZW + lista di comandi `.CLS` con 41 pagine, una per tipo d'unità: il nome della classe stampato a telescrivente. Nessun eseguibile lo carica. È il residuo di un elenco precedente, coerente con gli sprite: tipo 08h «NANUCHA CLASS MISSILE CORVETTE» (sprite NUCHKA, mentre il nome nel gioco è Koni), 0Fh «BROADSWORD CLASS BRITISH FRIGATE», 28h «SS-N-19 SLCM» |
| `SCENE*.LZA` | **Briefing e messaggi di missione**: dialogo a pagine (`u8 n` + offset u16; ogni pagina `[flag][u16 puntatore]`) con markup a token: `1D 00 <testo> 00` riga, `0C 00 nn` colore, `0B 00 i v` scrive il byte `i` del blocco di configurazione della missione (`DS:B45A`). Il byte 14 è l'indice della mappa. Testo estratto in `docs/originale/briefing_EN.txt` (`tools/re/briefings.py`) |

Scene: `SCENE<n><lato>`, con `n` = missione (0 = missione 10) e lato `A` = 688, `B` = Alfa, `C` = variante.

Corrispondenza mappe ↔ teatri, certa dal codice: A Otranto, B Baltico, C Barents, D Gibilterra,
E Kunashir, F Piattaforma Celtica, G Faroe (dettagli e verifica statistica in `docs/02_pipeline_dati.md`).

### 2.6 Audio `.T`
PCM senza segno a 4 bit, 2 campioni per byte, prima il nibble alto.
- `SONAR1` (ping), `PROP2`/`PROPV2` (eliche), `TORPRAMP` (siluro), `EXP1` (esplosione), `WHALE2` (balena).
- **Riproduzione** (`GAME.EXE` 1000:D461): ciclo d'attesa per nibble, con tabella di conversione `DS:BC52`
  verso l'altoparlante (porta 61h), il Tandy (C0h) o il Covox (LPT1/LPT2). Il ritardo è s × 3200 / p, con s
  = velocità della CPU misurata all'avvio (1000:A008). Quindi **p è la frequenza in Hz**, indipendente
  dalla CPU: EXP1 5000/4000, sonar1 5500/6200, TORPRAMP 6500.
- **ANALYZE della sala sonar** ricampiona prop2/propv2/whale2 con una frequenza che dipende da tipo d'unità
  e categoria del contatto (navi 2500 + 50t, armi 5500 + 10t, sottomarini ed esche 2000 + 30t Hz): è la firma per classe dell'originale. Anche il
  grafico dell'analizzatore è una firma fissa (1000:92AF): 6 barre che scrivono in binario tipo + 1, nella
  zona di frequenza della categoria (navi, sottomarini, biologici, siluri).
- WAV alle frequenze del codice e tabella delle firme in `extracted/suoni/` e `docs/originale/suoni.md`
  (`tools/re/suoni.py`). I vecchi `extracted/wav/` erano a 8 kHz ipotetici.

### 2.7 Musica dei titoli (`CALLS.EXE`)
`CALLS.EXE` è il lettore della musica dei titoli, con tre arrangiamenti diversi nello stesso file
(segmento dati 00D3h). Il modo lo sceglie chi lo lancia.
- **0, Tandy/PCjr**: SN76496 sulla porta C0h. Due voci a onda quadra con inviluppi di volume a tabella,
  glissando e vibrato; una voce di rumore, che usa il suo valore come byte di controllo del rumore;
  campioni a 4 bit sul canale 0, scrivendone l'attenuazione dall'interrupt del timer. Sequencer al
  ritorno verticale (circa 60 Hz).
- **1, Covox**: una voce campionata. 5 strumenti PCM a 4 bit (1,7–16 KB), nota = divisore del timer;
  sequencer alla sincronia verticale (70 Hz).
- **2, AdLib**: 9 voci OPL2 a 72,8 Hz (timer con divisore 4000h). Sequencer a pattern con trasposizione,
  strumenti a 9 byte, glissando, vibrato, rilascio e percussioni (non usate dal brano). Frequenze
  temperate: la nota 48 è il Do a 261 Hz.

Estratti in `extracted/musica/`:
- AdLib: `titolo_adlib.vgm` (registri OPL2, riproducibile con i lettori VGM), `.mid` (note, 9 tracce) e
  `.wav`, reso con ymfm. Durata 177 s, poi il brano si chiude con il comando 84h.
- Covox: `titolo_covox.wav`, 78 s.
- Tandy: `titolo_tandy.vgm` (scritture sullo SN76496, compreso il canale campionato) e `titolo_tandy.wav`,
  155 s. Il WAV viene da un emulatore del chip scritto per il progetto (`tools/re/tandy_musica.py`), perché
  ymfm non copre lo SN76496.

Strumenti: `tools/re/adlib_musica.py` (emulazione del sequencer 1000:08DF), `tools/re/covox_musica.py`,
`tools/re/tandy_musica.py`.

Controlli:
- Il cromagramma dei WAV concorda con le note del sequencer: AdLib 0,62 (−0,02 con le note spostate di 3
  semitoni), Tandy 0,60 (−0,20).
- Le tabelle delle frequenze sono temperate: AdLib entro 7 cent, Tandy entro 27 cent (periodi interi).
- Le durate si basano su due assunzioni: 70 Hz per il Covox su VGA e 60 Hz per il Tandy 1000.

## 3. Formati ancora da decodificare

| Formato | Indizi | Prossimo passo |
|---|---|---|
| `SCENE*.TOK` | **Decodificato** (22/10/2026): bytecode di una VM a 118 opcode (`2011:137C`, gestori a `DS:6505`), tutti con nome, argomenti ed effetto. Disassemblatore `tools/re/tok_disasm.py`: 100% dei byte dei 22 file decodificati, 96% raggiunti dal flusso. Unità, script per unità, timer, eventi del giocatore, messaggi ed esiti documentati in `docs/04_vm_missioni.md`; listati in `re/tok/`. Scala verificata (1 nm = 50 unità, 1 kt = 350, 1 ft = 8, 1 tick = 2 s), modello di scoperta dei sensori con termoclini, schede automatiche delle 22 scene in `re/tok/schede/` (`tools/re/tok_schede.py`) | **Convertite** in `data/missioni/*.lua` (schema `docs/09_schema_missioni.md`, `tools/re/missioni_lua.py`) |
| `SCENE*.LZA` | **Decodificato**: dialoghi a pagine (ordini, esiti, rapporto) e blocco di configurazione della missione (§2.5). Byte 0 lato (0 = 688, 1 = Alfa); 1–6 dotazione per tipo d'arma; 7–13 tipo d'arma dei 7 tubi (0 Mk 48, 1 Type 53, 2 Sea Lance, 3 Tomahawk, 4 Harpoon, 5 esca); 14 mappa | Testi tradotti in `docs/traduzione/scene/` |
| `*.CLS` | Liste di comandi grafiche compilate, eseguite dall'interprete 1000:BB44/BBBD: `u8 n` + n offset delle voci + offset della base; voce = numero di argomenti, offset del codice, tipi a 2 bit. Pagina 0 = schermo, voci = pagine richiamate dal gioco e macro del file. Comandi: 1..31 primitive del gioco (tabella DS:97A2: rettangoli, linee, pixel, cornici, testo, cursore, finestra di testo, aree attive con tasto rapido, immagini `.ENC`), 80h+k macro; argomenti letterali o presi dalla chiamante (±letterale). Font unico proporzionale a DS:9D5C (14 byte per carattere, `@` = grassetto) | **Decodificato** (`tools/re/cls_layout.py`; listati e ridisegni in `re/cls/`, verificati sulle figure del manuale) |
| `HELICOP.ENC` | **Decodificato** (23/10/2026). Non è un'animazione: è la base dati della protezione anticopia, mascherata da un nome qualsiasi. LZW (6398 byte), poi una lista di comandi nel formato `.CLS` con 89 pagine. Il gioco ne esegue una a caso (1000:EF73) e la stampa a telescrivente in sala radio (primitiva 29, 1000:606E): «ESTABLISHING SATELLITE UPLINK…», battello, frammento del manuale con «???»; tre comandi 11 scrivono la risposta. Le 89 voci, verificate tutte sul manuale (83 sulla copia BBS, 4 sulla stampa), sono in `docs/originale/protezione_codici.md` (`tools/re/protezione.py`) | Le immagini dell'elicottero sono `SEAHAWK0–4.ENC` |
| `MAIN.EGA` | 5.807 byte | **Decodificato** (immagine ENC; vedi §1) | — |
| Database unità | **Risolto.** Non c'è una tabella delle prestazioni: i modelli stanno negli script delle scene e le armi nei gestori di lancio (`docs/04_vm_missioni.md` §8, `docs/originale/prestazioni.md`). In `GAME.EXE` ci sono i nomi (DS:019A) e i prefissi degli sprite per tipo (DS:10E2: FOREST, MERCH, CARGO, SMK, SFX2, KARA, KIROV, KRIVAK, NUCHKA, SLAVA, NIMITZ, IOWA, KIDD, OHP, SAC, TARGET, TANKER, HORMON, SEAHAWK, MISSILE). HORMON (Ka-25) è citato, ma nei dischi non c'è nessun file HORMON | — |

## 4. Dati di gioco trovati nelle stringhe

- **10 missioni**: TORPEX '89, Shake 'Em, Sub Ops, Mumar Cadaver, Escape, Goulash,
  Cat Walk, Surprise Party, Homecoming, Hit and Run.
- **41 tipi di entità** nel database, tra cui Forrest Sherman, Kara, Kirov, Krivak II, Koni,
  Slava, Nimitz, Iowa, Kidd, Perry, Sacramento, Ka-25 Hormone, SH-60B, P-3C, 688, Alfa,
  Victor III, Sturgeon, Oscar, Akula, Charlie II, Typhoon, Kilo, Foxtrot, Trafalgar, Mk48,
  Mk50, Spearfish, Sea Lance, SS-N-19, Harpoon, Tomahawk, noisemaker, boa marcatrice.
- **~70 messaggi di bordo** (ad esempio "we crossed a thermal layer sir",
  "towed array deployed sir", "guidance cable cut captain"): base per il nuovo sistema di
  comunicazioni dell'equipaggio in italiano.
- **Sistemi danneggiabili**: sonar computer, towed array, periscope, targeting sonar,
  engines, antenna.

## 5. Strumenti installati (`tools/`)

| Strumento | Uso |
|---|---|
| Ghidra 12.2 + JDK Temurin 25 | Analisi e decompilazione headless (`re/ghidra_proj`, output in `re/decomp/*.c`) |
| Python venv: Pillow, NumPy, Capstone, pdfplumber | Script di estrazione e disassemblaggio x86 a 16 bit |
| REA 6.7.0 (MCP) + Ghidra 12.1.4 + JDK 21 + Node 24 | Analisi guidata funzione per funzione; dettagli in `docs/03_strumenti_re.md` |
| `tools/re/*.py` | `mlb.py`, `unexepack.py`, `lzw.py`, `assets.py`, sonde varie |

Comando per rigenerare tutti gli asset:
```bash
tools/venv/bin/python -I tools/re/assets.py /home/randolph/Claude/melchior-688
```
