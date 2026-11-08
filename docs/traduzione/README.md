# Traduzione italiana

Stato al 23/10/2026.

| Testo | File | Stato |
|---|---|---|
| Glossario e regole di stile | `glossario.md` | in uso, da estendere |
| Messaggi di bordo (57) | `messaggi_bordo.json` | **tradotti**; 9 con variante breve per la barra scorrevole |
| Altri testi di GAME.EXE (114): messaggi diretti alla barra, stato dei tubi, danni, pannello di stato, etichette, telescrivente, messaggi di sistema | `messaggi_exe.json` | **tradotti**; 16 con forma breve, tutte entro 50 caratteri |
| Briefing, esiti, rapporti (22 scene, 505 paragrafi unici) | `scene/SCENE*.json` (per il motore), `scene/SCENE*.md` (revisione bilingue) | **tradotti** |
| Interfaccia: stazioni, selezione missione, dialoghi, aiuto in linea, telescrivente (319 stringhe) | `interfaccia_it.json` | **tradotte**; 42 con forma breve `it_breve`, spazi verificati in `verifica_spazi.md` |
| Manuale originale (9 parti, 507 blocchi, circa 21.100 parole) | `manuale/capitolo_00..08.md`, `manuale/manuale_IT.md`, `manuale/memoria_it.json` | **tradotto**, appendici comprese |

## Come si rigenera

```bash
tools/venv/bin/python -I tools/re/lza_testi.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/re/trad_unisci.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/re/trad_stato.py /home/randolph/Claude/melchior-688
```

```bash
tools/venv/bin/python -I tools/re/manuale_testi.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/re/manuale_figure.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/re/manuale_unisci.py /home/randolph/Claude/melchior-688
```

- `lza_testi.py` estrae dai dialoghi `SCENE*.LZA` le pagine (ordini, esiti 1–19, rapporto finale),
  ricompone le righe in paragrafi e crea la memoria di traduzione `memoria_en.json`. Ogni paragrafo
  ha un id stabile: le prime 10 cifre dello SHA-1 del testo inglese.
- La prosa si traduce a mano nei file `lotti/lotto_*.json` (id → italiano).
- `trad_unisci.py` traduce le intestazioni con le regole del glossario, unisce tutto e si ferma se
  manca qualcosa. `trad_stato.py` elenca i paragrafi ancora da tradurre.
- `manuale_testi.py` pulisce `docs/originale/manuale_EN.txt` (intestazioni e numeri di pagina) e lo
  divide in blocchi tipizzati (capitolo, titolo, sottotitolo, testo, elenco, figura ASCII, didascalia),
  con la stessa regola degli id. La traduzione è in `manuale/lotti/m_*.json`; `manuale_unisci.py`
  produce i Markdown per capitolo, si ferma se manca un blocco e segnala i termini vietati dal glossario.

## Manuale originale

- Capitolo 0 è frontespizio e sommario; i capitoli 1–7 sono quelli dell'originale. Le figure ASCII
  hanno le etichette tradotte; le tabelle del capitolo 7 (sottomarini, siluri, missili, navi, aerei,
  tasti) sono ricomposte in tabelle Markdown, con ★ per le armi usate dal 688 o dall'Alfa.
- La copia digitale (BBS P-80 Systems, `docs/originale/manuale_EN.txt`) termina dopo i comandi da
  tastiera. Appendice A (cavi null-modem), Glossario, Indice analitico e crediti sono trascritti dalla
  scansione del manuale stampato su Internet Archive
  ([688attacksub_201909](https://archive.org/details/688attacksub_201909), pagine 70–75) in
  `docs/originale/manuale_EN_appendici.txt` e formano il capitolo 8. Lo schema dei cavi è letto pin per
  pin dall'immagine; assistenza tecnica, garanzia e avvertenze legali non sono tradotte.
- La copia BBS è confrontata con la stampa in [manuale/confronto_stampa.md](manuale/confronto_stampa.md):
  dalla stampa sono state aggiunte la riga della fregata Koni, la classe Victor III e le figure 6.2–6.4. Sono
  state corrette sei omissioni della BBS, tra cui il filo di guida "tranciato" nel capitolo 5.
- Le 33 figure della stampa (copertina, figure numerate, disegni) sono in `extracted/manuale_stampa/figure/`
  (`tools/re/manuale_figure.py`, elenco in `manuale/figure.json`) e sostituiscono nei Markdown le figure
  ASCII, che restano richiudibili con le etichette tradotte.
- Glossario e indice sono riordinati sui termini italiani, con l'originale inglese tra parentesi; i
  numeri di pagina restano quelli della stampa.
- Le didascalie [NOME BATTELLO] (46) restano in inglese: sono i marcatori dei paragrafi usati dalla
  protezione anticopia SEND CODE.
- Note del traduttore aggiunte: Tivat, Kunashir, stretti danesi, quota controllata con i timoni,
  ESM, autonomia dei nucleari, curvatura del suono e termoclino permanente, Mk 32 (è il lanciatore),
  testate a carica cava, Type 65 da 650 mm, Sea Lance cancellato nel 1990, velocità di Slava e Iowa.
- Il manuale verrà ampliato dall'edizione Enhanced (simulazione, navigazione, equipaggiamenti,
  riferimenti e foto): le note "edizione Enhanced" segnano già dove il remake cambierà le regole.

## Spazi dell'interfaccia

`tools/re/cls_layout.py` decodifica i layout delle stazioni (`.CLS`) e ne ricava ogni testo con posizione,
finestra e riquadro; `tools/re/trad_spazi.py` misura le stringhe italiane con il font originale e le confronta
con lo spazio disponibile. Il rapporto è in [verifica_spazi.md](verifica_spazi.md), i ridisegni con i testi
italiani in `re/cls/<FILE>_it.png`.

```bash
tools/venv/bin/python -I tools/re/cls_layout.py /home/randolph/Claude/melchior-688
```
```bash
tools/venv/bin/python -I tools/re/trad_spazi.py /home/randolph/Claude/melchior-688
```

- **Etichette e pulsanti**: 199 testi. 41 non entravano nella forma piena; con le forme brevi ne resta uno. Le forme brevi
  (`it_breve`): BERS., SELEZ., IMP., CANC., OFF, MAX, PUNTI DI ROTTA, LIV. CAVITAZIONE e così via. Resta
  *TUTTA* sul telegrafo di macchina, 4 pixel oltre la linea del pannello: va spostata la linea nel layout
  640×480. *STD* resta la sigla originale, perché *MEDIA* non entra.
- **Messaggi nelle finestre** (dialoghi e aiuto): 40 paragrafi. Con il ritorno a capo automatico del remake
  entrano tutti.
- **Correzioni emerse dal ridisegno**: nel pannello di governo le due righe *PERISCOPE / DEPTH* ora sono
  *QUOTA / PERISCOPICA* (prima "PERISCOPIO / QUOTA"). In *@AUTO FUNCTION* la chiocciola era un byte del
  layout, non il comando del grassetto. Aggiunte le sigle senza vocali sfuggite all'inventario: *CRS:* → RTA:,
  *DPT:* → QTA:, *STD*, *150 FT*.
- Il remake a 640×480 ha il doppio dei pixel in orizzontale e un font nuovo. La forma piena (`it`) resta
  quindi per i layout ridisegnati, la forma breve per quelli che restano fedeli all'originale.

## Barra dei messaggi e testi lunghi

Nell'originale (`FUN_1000_0ed8`) il messaggio di bordo scorre da destra a sinistra in un riquadro di
circa 302 pixel (circa 50 caratteri), accanto al ritratto dell'ufficiale che parla (`CREW1–8.ENC`).
Il testo che supera la larghezza resta tagliato. Regola del remake: **nessun testo viene troncato**,
ma scorre per intero o va a capo. Per gli spazi fissi esiste la variante `it_breve`, entro 50
caratteri anche con "Compagno comandante".

## Scelte e note di traduzione

- **Paragrafi troncati nell'originale**, completati in modo minimo:
  - `33044cd67a` "REMEMBER, YOU HAVE MISSILES" → "Ricordi che ha a disposizione anche i missili.";
  - `5cc8f46354` "…AND ONLY THE KILLING BLOW" → "…e conta solo il colpo di grazia.".
- **Refusi dell'originale** non riprodotti: "HELICOPTOR", "FOREST SHERMAN", "POSSITION", "YOUR OFF",
  "WERE YOU THINK YOU WHERE GOING", "ELECTOMAGNETIC", "TROLLER" (peschereccio), "Trivat" (Tivat).
- **Battute** rese con un equivalente: "deep... water" → "nei guai fino al collo... d'acqua"; "bears in
  tow" → "orsi al seguito"; "win one for the Gipper" resta "Facciamolo per il Gipper", con una nota
  nel manuale (allusione a Reagan).
- **Easter egg** mantenuti: le firme degli ammiragli sono i nomi degli autori (Ratcliff, Grace, Breen);
  "registratore giapponese" allude al caso Toshiba-Kongsberg.
- **Errori di fatto** da segnalare con una nota del traduttore nel manuale: missione 7 (Kunashir collega
  il Mare di Okhotsk al Pacifico) e missione 10 (stretti danesi non percorribili in immersione).
- `DIALOG.CLS` contiene messaggi della versione dimostrativa ("… versione commerciale …"): tradotti
  ma non usati nella versione completa.
