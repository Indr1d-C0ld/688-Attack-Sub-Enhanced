# Volume 4 — Tattiche operative

*Indice del 31/10/2026 (piano di revisione del manuale, roadmap 39). Curatore: kit.* Il volume dice come si combatte
con i modelli dell'Enhanced: che cosa funziona, perché, e quanto. Riprende il capitolo 6 del manuale originale
(strati termici, esche, le zone A/B/C, il cono d'ombra), la dottrina dei dossier (02 §6-7, 03 §C4, 24) e i volumi 2 e
3, e lo riscrive sui modelli del gioco.

## Come è fatto ogni capitolo

- **Le regole pratiche**, in poche righe numerate.
- **Il perché**, col rimando al capitolo del volume 2 che ha il modello.
- **Un esempio numerico** preso dal gioco: i numeri si misurano col motore su più semi (`scene_interazioni --semi N`,
  le serie della sandbox, le sonde di `tools/re/prove`), non si stimano. Ogni capitolo dice come sono stati misurati.
- **Una scena per provarlo**: un vetrino della sandbox o una missione della libreria.

Le tabelle copiate dal volume 2 si rigenerano come lì (`tools/pipeline/manuale.py`), così non si separano dal gioco.
Le tattiche valgono nell'Enhanced; dove il classico si comporta diversamente lo si dice.

## Capitoli

| Capitolo | Contenuto | Base nel volume 2 | Stato |
|---|---|---|---|
| [T1 Principi](T1_principi.md) | il ciclo scoperta, classificazione, localizzazione, attacco; rumore contro velocità e quota; l'esposizione | B1, C1, C4, F4 | bozza curata |
| [T2 L'ambiente come arma](T2_ambiente_come_arma.md) | strato e termoclino, zona d'ombra, convergenza, canale di superficie, fondale, frange, rumore del mare e del traffico, meteo per teatro e mese | C2, C3, E7, E10 | bozza curata |
| [T3 Cercare](T3_cercare.md) | sprint and drift; la schiera e lo specchio; il cono sordo; sopra o sotto lo strato; ESM e periscopio, il radar che tradisce | C1, C6, C9, C10 | bozza curata |
| [T4 Classificare](T4_classificare.md) | LOFAR e DEMON, la libreria e gli slot, il sonarista; il libretto e il periscopio; come ti classifica l'IA | C4, C5, C9 | bozza curata |
| [T5 La TMA in pratica](T5_tma.md) | le gambe, la regola del 1934, Ekelund, il MATE, l'AOB al periscopio; che cosa sa di te la TMA dell'IA | C13, E6 §2.5 | bozza curata |
| [T6 L'attacco col 688](T6_attacco_688.md) | il Mk 48 (RTE, soffitto, pavimento, filo); le salve; Harpoon e TLAM; il punto di lancio e la ritirata | D1, D3, D5 | bozza curata |
| [T7 Combattere con l'Alfa](T7_alfa.md) | la velocità e la quota come armi; niente schiera; i siluri sovietici; il reticolo; il Bukhta | vol. 1 §2.6, C9, C10, D1 | bozza curata |
| [T8 Difesa ed evasione](T8_difesa_ed_evasione.md) | le zone A/B/C rifatte con autonomia, rateo di virata, testa e anti-circolare; esche ADC e MG-74; il siluro sotto il soffitto della testa; il Crazy Ivan visto da dietro | D1, D2, E6 §2.12, E3 | bozza curata |
| [T9 Contro aerei e navi](T9_aerei_e_navi.md) | boe, dip, MAD; quota e strato; EMCON; i cannoni e gli illuminanti; l'elicottero imbarcato | E5, E4, C10 | bozza curata |
| [T10 Pace e crisi](T10_pace_e_crisi.md) | pedinare ed essere pedinati, il cono sordo, l'INCSEA, le ROE, l'identificazione a vista, i fanali, le acque territoriali | E6 §2.11, vol. 3 §6.7, C9 | bozza curata |
| [T11 Acque difficili](T11_acque_difficili.md) | fondale e incaglio, mine e BQS-15, navigazione incerta e punti nave, sotto il ghiaccio | C12, D4, E11 | bozza curata |
| [T12 Casi di studio](T12_casi_di_studio.md) | le 24 missioni storiche della libreria per lezione, due episodi da vicino; tre originali commentate col replay | A3, F7, vol. 3 cap. 10 | bozza curata |

Le bozze di T2, T4 e T11 sono del disassemblatore, quelle di T1, T6 e T7 del kit; T3 del disassemblatore e del kit, T8 e T9 del runtime, T10 del runtime e del kit, T12 di trailer-1; la curatela (rilettura, rimandi,
stile) è del kit.
