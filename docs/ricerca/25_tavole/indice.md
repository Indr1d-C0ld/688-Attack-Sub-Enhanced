# Le tavole della rifinitura estetica (dossier 25, §8)

Bozze grafiche fuori dal codice del gioco, a 640×480 e salvate a 2× senza filtri, per scegliere fra le varianti
del linguaggio comune. I dati sono quelli veri delle schermate dell'inventario (`../schermate_prima/`), con una sola
eccezione: nelle regolazioni la situazione del Patto è «GUERRA», per far vedere come si legge un valore cambiato.

Le disegna `engine/strumenti/bozze_estetica.cpp` (kit), coi pezzi del kit (`grafica/consolle`, `cornici`,
`cerimonia`), con la palette del giorno e i temi delle consolle. `tools/estetica/bozze.py` le compone; la carta del
teatro è un ritaglio dell'istantanea della scelta della missione. Per rifarle:

```
tools/venv/bin/python tools/estetica/bozze.py .
```

| Tavola | Che cosa mostra |
|---|---|
| `confronto_regolazioni.png` | le regolazioni della sandbox (oggi 31 righe di testo nudo su nero) nelle varianti A, B e C |
| `confronto_scelta.png` | la scelta della missione nelle tre varianti |
| `combinazione_consigliata.png` | il parere del kit. Scelta e regolazioni in A; l'esito come documento, cioè B, com'è già oggi; le statistiche della sandbox come schermi di consolle, cioè C, col secondo schermo che riempie lo spazio vuoto |
| `regolazioni_A.png`, `_B`, `_C`; `scelta_A.png`, `_B`, `_C`; `statistiche_C.png` | le singole schermate a 2× |

**Le varianti** (dossier 25, §8.3-8.5):
- **A, «Lamiera di bordo»:** la lingua di *688* del 1989.
  - Lamiera, pannelli incassati con le viti, targhe rivettate, testo inciso con l'ombra.
  - I valori su vetro nero; i tasti a spia (verde avanti, rosso indietro, ambra le scelte), col filo d'oro sul
    predefinito.
  - La lente blu per il 688 e rossa per l'Alfa.
- **B, «La base»:** luoghi e documenti.
  - Il modulo d'ordine sul portablocco, la scrivania di legno.
  - I valori a penna (blu come da missione, rosso cambiato), l'evidenziatore sulla riga scelta.
  - La cartellina manila con le linguette, il foglio dell'elenco coi timbrini, la foto della carta graffettata, i
    timbri.
- **C, «Il quadro di comando»:** la consolle con la targhetta del posto, i CRT a fosforo verde.
  - Il cursore in negativo, i valori cambiati luminosi e gli altri tenui.
  - I tasti funzione a spia nel piede.
  - Il secondo schermo usa lo spazio che oggi resta nero.

**Che cosa non mostrano ancora:**
- le dissolvenze di palette e le animazioni (spie, color cycling);
- lo stato premuto dei pulsanti;
- la versione VMF di ogni variante (consolle crema, targhette rosse, stampino).
