# Strumenti di reverse engineering

Tutto è installato in `tools/` senza permessi di amministratore. Versioni fissate e checksum
verificati il 22/10/2026.

| Componente | Versione | Percorso | Uso |
|---|---|---|---|
| Ghidra | 12.2 | `tools/ghidra/ghidra_12.2_PUBLIC` | Analisi headless e dump completo del decompilato (`re/ghidra_proj`, `re/decomp/`) |
| JDK Temurin | 25.0.5 | `tools/jdk` | Richiesto da Ghidra 12.2 |
| Ghidra | 12.1.4 (SHA-256 `ddac49f9…d4db`, verificato con le note di rilascio) | `tools/ghidra/ghidra_12.1.4_PUBLIC` | Motore di REA (che accetta solo la serie 12.1) |
| JDK Temurin | 21.0.13 (checksum verificato) | `tools/jdk21` | Richiesto da Ghidra 12.1.4 |
| Node.js | 24.22.1 LTS (checksum verificato) | `tools/node` | Esecuzione di REA |
| REA (`rea-agents`) | **6.7.0** (fissata con `package-lock.json`) | `tools/rea` | Server MCP e riga di comando per l'analisi guidata |
| Python venv | 3.13 + Pillow, NumPy, Capstone, xarray, netCDF4, gsw, SciPy | `tools/venv` | Script `tools/re/` e `tools/pipeline/` |
| ymfm (Aaron Giles, BSD-3) | commit `81aec25` del 27/07/2026 (tarball SHA-256 `9bcf001b…f443f`) | `tools/ymfm` | Emulazione OPL2: `tools/ymfm/vgmrender` rende i VGM in WAV (compilato con `g++ -O2 -std=c++17 -I src examples/vgmrender/*.cpp src/*.cpp`) |
| Lua (PUC-Rio, MIT) | **5.4.7** (tarball SHA-256 `9fbf5e28…bf1e30`, da lua.org) | `tools/lua` | `tools/lua/src/lua` e `luac`, compilati con `make linux`: verifica delle missioni `data/missioni/*.lua` (`tools/re/verifica_missioni.lua`); la stessa versione prevista per il motore |

## REA (morluto/rea)

Server MCP che dà all'agente accesso funzione per funzione a Ghidra: procedure, pseudo-C,
assembly, chiamanti e chiamati, stringhe, byte, rinomine e commenti, con le prove dietro ogni
risultato. Supporta esplicitamente gli eseguibili DOS MZ a 16 bit (`MzLoader`,
`x86:LE:16:Real Mode`, segmento di caricamento `0x1000`): gli stessi parametri del nostro
progetto Ghidra, quindi gli indirizzi coincidono (lineare = segmento × 16 + offset).

### Registrazione MCP
File `.mcp.json` nella radice del progetto (ambito di progetto): avvia `tools/node/bin/node`
con `tools/rea/node_modules/rea-agents/scripts/rea.mjs mcp`, con `GHIDRA_INSTALL_DIR`,
`JAVA_HOME` e `PATH` impostati sulle installazioni locali. Claude Code chiede di approvare il
server al primo avvio di una sessione nel progetto. Gli strumenti diventano disponibili solo
dopo il riavvio della sessione.

Non è stato eseguito `rea setup`, che modificherebbe la configurazione utente
(`~/.claude.json`, `~/.claude/skills`). La skill di REA si può aggiungere in seguito, se utile.

### Uso da riga di comando
Prima esportare l'ambiente:
```bash
export PATH=/home/randolph/Claude/melchior-688/tools/node/bin:$PATH GHIDRA_INSTALL_DIR=/home/randolph/Claude/melchior-688/tools/ghidra/ghidra_12.1.4_PUBLIC JAVA_HOME=/home/randolph/Claude/melchior-688/tools/jdk21
```
Controllo di prontezza:
```bash
/home/randolph/Claude/melchior-688/tools/rea/node_modules/.bin/rea doctor --provider ghidra --json
```
Decompilare una funzione per indirizzo lineare (esempio: decompressore LZW, `2283:000D`):
```bash
/home/randolph/Claude/melchior-688/tools/rea/node_modules/.bin/rea function /home/randolph/Claude/melchior-688/re/unpacked/GAME.EXE 0x2283d --provider ghidra --json
```
Da riga di comando ogni operazione avvia Ghidra (circa 35 s). Via MCP la sessione resta aperta.

### Regole d'uso nel progetto
- Analizzare sempre le copie decompresse in `re/unpacked/`: gli originali sono compressi con EXEPACK.
- REA usa progetti Ghidra temporanei, cancellati alla chiusura della sessione. Rinomine e commenti
  vanno salvati come snapshot (`snapshot_path` su `open_binary` / `close_binary`) in `re/rea/`, e
  le scoperte stabili riportate in `docs/01_analisi_tecnica_originale.md`.
- Aggiornamenti solo a mano e verificati: niente `rea update`. Per aggiornare, cambiare la versione in
  `tools/rea/package.json`, reinstallare con `npm install --ignore-scripts --save-exact`, rieseguire
  `rea doctor` e aggiornare questa pagina.

### Verifica eseguita
- `rea doctor --provider ghidra`: healthy (Ghidra 12.1.4, decompilatore nativo, JDK 21).
- `rea inspect` e `rea function 0x2283d` su `re/unpacked/GAME.EXE`: formato `dos-mz` riconosciuto,
  funzione `FUN_2283_000d` con corpo `0x2283D–0x228E1`, pseudo-C e assembly.
- Server MCP: `initialize` e `tools/list` rispondono (REA 6.7.0, 207 strumenti).
- `npm audit`: 0 vulnerabilità note al momento dell'installazione.
