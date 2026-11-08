# Kit delle navi: modello 3D + proiezione multipla da foto calibrate

Metodo adottato per gli sprite delle unità al periscopio. Ogni classe ha un modello 3D assemblato da
parti parametriche, in scala esatta. Le foto d'epoca, con la fotocamera calibrata, vengono
proiettate sul modello. Il risultato sono sprite a 16 grigi della palette originale, a ogni angolo
sulla prua e da entrambi i lati. Verificato sulla fregata Oliver Hazard Perry.

## Strumenti (`tools/navi/`)

| File | Contenuto |
|---|---|
| `kit.py` | Mesh e parti: `scafo` (generatore parametrico), `tuga`/`scatola`, `asta`, `cilindro`, `sfera`, `traliccio` (albero con gambe, controventi a X, piattaforme), `pennone`, `antenna_radar` (graticcio), `lanciatore`, `cannone`, `ciws`, `radome`, `fumaiolo`, `imbarcazione`; parti sovietiche e parti NATO e generali (elenco nell'intestazione del file, vedi «Consolidamento del kit») |
| `maschera_manuale.py` | Maschere ritoccate a mano: per luminosità (poligoni + soglie) o `metodo: colore` (navi chiare contro cielo e mare blu) |
| `triangola.py` | Triangolazione multi-foto di elementi della nave (aggiustamento a fascio semplificato) |
| `camera.py` | Fotocamera a foro stenopeico; calibrazione da punti di riferimento (minimi quadrati con limiti fisici) e affinamento sulla sagoma (IoU, Nelder-Mead) |
| `raster.py` | Sagome rapide (PIL) e G-buffer con z-buffer e baricentriche corrette in prospettiva |
| `foto.py` | Maschera nave/sfondo, cancellatura delle scritte, livelli, calibrazione, mappa di profondità per le occlusioni, uso speculare |
| `costruisci.py` | Costruzione di una classe dal suo YAML: modello, calibrazione, sprite, verifiche |
| `foglio_confronto.py` | Fogli di confronto, aspetto continuo, scena di prova al periscopio |
| `classi/perry.yaml` | Prima classe descritta |

```bash
tools/venv/bin/python -I tools/navi/costruisci.py /home/randolph/Claude/melchior-688 tools/navi/classi/perry.yaml 15
```
L'ultimo argomento è il passo in gradi (15 per le prove, 5 per la produzione).

## Convenzioni

Coordinate **destrorse** in metri: x verso prua (origine a metà lunghezza), **y verso sinistra**,
z verso l'alto (galleggiamento a z = 0). Vista da dritta, la prua appare a destra. Nel YAML un
valore `{prua: d}` indica un punto a d metri dalla prua (x = L/2 − d). I prototipi in
`tools/museo/` usavano un sistema sinistrorso: il kit lo sostituisce.

Angolo sulla prua (AOB): 0° = prua verso l'osservatore, 180° = poppa. Lato `dritta` = osservatore
sulla dritta della nave.

## Formato di una classe (YAML)

- `scafo`: `L`, `B`, `bordo` (quota della coperta lungo lo scafo, come coppie distanza dalla prua /
  quota), `slancio` (arretramento del dritto al galleggiamento), `finezza_prua`, `specchio`,
  `svasatura`, `pienezza` (esponenti della forma al galleggiamento e in coperta).
- `parti`: elenco di parti del kit con `tipo`, `nome` e parametri.
- `moduli_parti` (facoltativo): file Python, relativi alla cartella della classe, che definiscono un
  dizionario `PARTI` di parti aggiuntive (per esempio `classi/nimitz_parti.py`). Permettono di aggiungere
  parti specifiche di una classe senza modificare `kit.py`, e di costruire più classi in parallelo.
  Le parti che risultano utili a più classi vanno poi spostate in `kit.py`.
- `foto`: per ogni foto:
  - `file`, `nave`, `qualita` (peso), `specchiabile` (usata anche riflessa per il lato opposto);
  - `orizzonte_y` e `soglia_cielo`: sopra l'orizzonte la nave è più scura del cielo; opzionali
    `soglia_mare`/`mare_fino_a_y` se anche contro il mare la separazione è affidabile;
  - `riquadro` e `escludi`: limitano la zona affidabile;
  - `cancella`: riquadri delle scritte (numeri di scafo, nomi) e `margine_cancella`;
  - `soglia_schiuma`: sotto l'orizzonte i pixel più chiari di questa soglia non vengono proiettati;
  - `maschera_file`: maschera ritoccata a mano (vedi Krivak II, versione 3), che sostituisce soglie e sagoma;
  - `fotocamera_iniziale`: posizione, punto mirato e focale approssimativi;
  - `orizzonte_punti` (facoltativo): pixel `[u, v]` sulla linea dell'orizzonte marino; vincolano quota ed
    elevazione della fotocamera (l'orizzonte vero sta sqrt(2h/R) sotto l'orizzontale). Da usare quando i
    punti della nave sono quasi tutti sul piano di simmetria;
  - `punti`: punti di riferimento `[[x, y, z], [u, v]]` (almeno 5, non tutti sul piano di simmetria
    se possibile).

## Come funziona

1. **Calibrazione.** Prima i punti di riferimento (minimi quadrati con limiti fisici: distanza ≥ 80 m,
   elevazione tra −5° e 60°, e un vincolo debole verso distanza e focale iniziali, necessario quando
   i punti sono quasi complanari). Poi l'affinamento che massimizza la coincidenza tra sagoma del
   modello e nave nella foto, solo nella zona affidabile (contro il cielo).
2. **Maschera finale.** Sopra l'orizzonte la separazione dal cielo; sotto, la sagoma del modello
   proiettata con la fotocamera calibrata, meno la schiuma.
3. **Resa di ogni vista.** G-buffer dal punto di vista del periscopio (occhio a 2 m, 4 km). Per ogni
   pixel e ogni foto si calcolano:
   - la posizione nella foto;
   - la visibilità dalla fotocamera della foto (mappa di profondità: una foto non dipinge
     superfici che dal suo punto di vista erano nascoste);
   - il peso: orientamento della superficie verso la fotocamera, al quadrato, per la qualità;
     zero se la foto lì non mostra la nave.

   Le foto si mescolano con un piccolo contributo dell'ombreggiatura del modello, che resta il
   ripiego dove nessuna foto vede la superficie. Livelli di ogni foto uniformati (media e deviazione
   dei pixel della nave).
4. **Sprite.** Sovracampionamento 3×3, quantizzazione con retinatura ordinata sui grigi 144–158 di
   `DAY.PAL`, 159 trasparente.

## Verifica sulla Perry (22/10/2026)

Uscite in `extracted/navi/perry/`: `sprite/aob_<gradi>_{dritta,sinistra}.png`, `verifica.json`,
`calibrazione_<foto>.png` (rosso = solo modello, verde = solo foto, giallo = contorno del modello),
`fonti_dominanti.png`, `confronto.png`, `aspetto_continuo_{dritta,sinistra}.png`, `scena_periscopio.png`.

Modello: 2.250 triangoli, 18 parti. Costruzione ogni 15°: circa 42 s.

| Foto | Nave | Errore sui punti | Coincidenza della sagoma |
|---|---|---|---|
| Fianco sinistro | USS Nicholas, 1984 | 0,7 px | 0,843 |
| Prua | USS Simpson, 1985 | 3,2 px | 0,479 (zona contro il cielo: sartiame e ringhiere sottili non modellati) |
| Poppa 3/4 | USS Elrod, 1985 | 15,3 px | 0,796 |

Valori dopo la ricostruzione con le partenze multiple introdotte con il Krivak II. Rispetto alla prima
costruzione l'errore sulla foto di prua scende da 7,1 a 3,2 px; le altre coincidenze sono invariate
(differenze di ±0,005).

Le fotocamere ricostruite sono plausibili: fianco a circa 1,5 km sul lato sinistro; prua a circa
370 m davanti e 22 m di quota; poppa 3/4 a circa 300 m a poppavia sinistra. Sei sorgenti in totale:
3 foto più le loro versioni speculari.

### Problemi trovati e risolti durante la verifica
- **Fotocamera degenere**: con punti tutti sul piano di simmetria (foto di fianco) la calibrazione
  libera finiva a pochi metri dalla nave con focale 33 pixel. Risolto con la parametrizzazione
  attorno a un punto mirato, i limiti fisici e il vincolo debole.
- **Errori di annotazione**: nella poppa 3/4 avevo scambiato gli spigoli di dritta e di sinistra;
  il controllo visivo li ha mostrati subito (coincidenza da 0,28 a 0,80 dopo la correzione).
- **Geometria**: il pennone dell'albero principale è a circa 28 m ed è largo circa 12 m (misurato
  sulla foto di prua); lo scafo ha una forte svasatura a prua (nuovo parametro `pienezza`).
- **Lato dritto senza foto**: tutte le foto sono riprese da sinistra o da prua; risolto con l'uso
  speculare delle foto (scritte cancellate).
- **Schiuma** proiettata sullo scafo e **residui delle scritte**: soglia della schiuma; cancellatura
  con margine e riempimento interpolato.

### Limiti noti
- Albero e pennoni sono semplificati: sartiame, ringhiere e antenne a frusta non sono modellati.
- Resta un leggero residuo del numero cancellato sulla prua di dritta (foto della Simpson riflessa).
- Le foto vengono da tre navi diverse della stessa classe (FFG-47, 55, 56): differenze di dettaglio
  minime, numeri di scafo cancellati. Nel gioco il numero di ciascuna nave si potrà dipingere a parte.
- Il tono complessivo dipende dalla scelta dei livelli; da tarare con la palette e il ciclo giorno/notte.

## Come aggiungere una classe

1. Trovare 2–4 foto di pubblico dominio (`tools/museo/commons_cerca.py`): una di fianco a livello del
   mare è indispensabile, una di prua e una di poppa (anche oblique o un po' rialzate) migliorano gli
   angoli estremi.
2. Scrivere `classi/<classe>.yaml`: scafo e parti con misure dal dossier, dai disegni di riconoscimento
   e dalla foto di fianco calibrata (scala in m/pixel da prua e poppa).
3. Annotare 5–7 punti di riferimento per foto (cima d'albero, dritto di prua, spigoli dello specchio,
   radome, spigoli di plancia o hangar), facendo attenzione a dritta e sinistra.
4. Costruire con passo 15, controllare `calibrazione_*.png` e `verifica.json`; correggere e ripetere.
5. Costruire con passo 5 per la produzione.

Stima: circa 1–2 ore per classe dopo questo kit, soprattutto per scegliere le foto e annotare i punti.

## Seconda classe: Krivak II (Progetto 1135M), prova con foto di ricognizione (22/10/2026)

Classe: `tools/navi/classi/krivak2.yaml`; uscite in `extracted/navi/krivak2/` (con `dettaglio.png`).
Modello: 1.572 triangoli, 22 parti, misure dalla foto di fianco calibrata (scala 0,058 m/pixel;
lunghezza misurata 123,6 m contro 123,5 ufficiali). Particolarità: ponte a gradini (8,4 m a prua,
circa 5 m al centro, cassero di poppa a 2,4 m), Rastrub (SS-N-14) a prua, due torri da 100 mm a
poppa, alloggiamento del VDS.

| Foto (pubblico dominio, US Navy) | Ripresa | Errore sui punti | Note |
|---|---|---|---|
| Pytlivyy 1986, fianco dritto, 2800 px | ~10° dall'alto, ~3 km | 3,8 px | quasi tutta la nave sotto l'orizzonte |
| Rezvyy 1983, prua di dritta, 2850 px | ~3°, ~250 m | 12,0 px | |
| Gordelivyy 1985, poppa di sinistra, 2860 px | ~13° dall'alto, ~270 m | 11,3 px | nessun cielo: calibrazione solo sui punti |

Metadati delle foto in `extracted/navi/krivak2/foto/fonti.json`.

### Cosa è cambiato nel kit con questa prova
- **Calibrazione a partenze multiple** (azimut ±40°, elevazioni 5–45°, distanza ×0,5–2): necessaria
  per le foto aeree, dove la stima iniziale è lontana dal vero.
- **Riconoscimento del mare** sotto l'orizzonte (`soglie_mare_sigma`): statistiche del mare riga per
  riga attorno alla nave. Dove il modello non coincide con la nave la foto mostra il mare: quei pixel
  non vengono proiettati, e resta l'ombreggiatura. Prima le parti fuori posto apparivano trasparenti.
- **`esponente_peso`** (predefinito 2): un valore più alto fa prevalere la foto meglio orientata
  (Krivak: 6). Effetto modesto.
- **Diagnosi punto per punto**: un errore di 44 pixel sulla foto aerea è sparito togliendo un punto
  identificato male (uno "spigolo del gradino" che era un altro spigolo). Conviene sempre controllare
  gli scarti dei singoli punti prima di cercare altre cause.

### Esito e limiti
Il metodo regge anche con le foto di ricognizione: le tre fotocamere si calibrano, ogni foto lavora
dove deve (fianchi, prua, poppa) e gli sprite hanno dettaglio fotografico e alberi a traliccio veri.
La qualità però è inferiore alla Perry:
- le foto sono riprese dall'alto e la nave sta quasi tutta contro il mare, quindi la maschera dipende
  dal modello e la precisione del modello conta molto di più;
- il modello è ancora schematico (sovrastrutture del Krivak molto articolate, lanciatori Osa-M,
  Head Net a doppio paraboloide semplificata), quindi la tessitura alterna foto e ombreggiatura
  ("a macchie");
- le tre foto sono di navi e anni diversi.

### Per le classi sovietiche, in produzione
1. Geometria più fedele: partire dai disegni di riconoscimento d'epoca (profilo e pianta) invece che
   dalla sola foto, e aggiungere al kit parti specifiche (lanciatori Osa-M a scomparsa, paraboloidi
   doppi, torri binate, RBU).
2. Quando possibile, almeno una foto di fianco ripresa da bassa quota (ce ne sono nei fondi NARA:
   da cercare per nave).
3. Ritocco manuale delle maschere sotto l'orizzonte per le foto principali (pochi minuti per foto).
4. Stima: 2–4 ore per classe sovietica, contro 1–2 per le classi USA.

### Krivak II, versione 2: rifinitura (22/10/2026)

**Disegno di riconoscimento.** Unico disegno di pubblico dominio trovato: il profilo US Navy del 1987
(rivista *All Hands*), `extracted/navi/krivak2/disegni/profilo_1987.png` (742 × 170 px, metadati in
`fonti.json`). Verifica sulle foto calibrate:
- con le quote e le posizioni del disegno gli errori di calibrazione **peggiorano** (14 px complessivi
  contro circa 10), soprattutto sulla foto aerea, indipendente dalla foto di fianco (18,4 contro 11,3 px);
- il fattore di scala verticale non si lascia determinare (`tools/navi/scala_verticale.py`, errore
  piatto tra 0,9 e 1,5);
- con le quote del disegno la torre AK-100 poppiera risulterebbe alta 1,8 m sul ponte (reale circa 3 m).

Conclusione: il profilo è una sagoma di riconoscimento stilizzata, utile per sapere **quali parti ci sono
e com'è fatta la sagoma** (da qui gli Osa-M prodiero e poppiero, assenti nella prima versione), non per
misurare.

**Triangolazione da più foto** (`tools/navi/triangola.py`): stima insieme le fotocamere delle foto e la
posizione 3D degli elementi annotati in più foto. Ancore: lunghezza fuori tutto e galleggiamento; vincolo
debole σ = 3 m verso la stima iniziale (con σ = 15 m la geometria complessiva diventa instabile per i
pochi punti, ma gli elementi principali restano vicini alle misure dalla foto ed escludono i valori del
disegno). Risultati: dritto in coperta 8,5 m; piede del dritto a 8,2 m dalla prua; Head Net 26,5 m a
60,7 m; traliccio poppiero 28,7 m a 66,2 m; Rastrub tra 18,1 e 32,5 m con sommità a circa 9,9 m;
torri X e Y a 95,3 e 103,6 m, sommità 8,6 e 7,2 m. Errori: 4,3 / 11,1 / 5,2 px.

**Parti sovietiche aggiunte al kit:** `tubo`, `lanciatore_quadruplo` (Rastrub/SS-N-14),
`lanciatore_scomparsa` (Osa-M), `lanciarazzi_rbu` (RBU-6000/1000, fascio di canne inclinato),
`paraboloide`, `head_net` (due riflettori contrapposti), `direttore_tiro` (Pop Group, Owl Screech),
`torre_singola` (AK-100), `lanciasiluri_quadruplo`; trasformazione `Mesh.ruota_y`.

**Foto di fianco da bassa quota:** non trovata. Nella categoria Commons "Bessmennyy class frigate"
(nome russo del Krivak II) le altre foto del periodo sono riprese aeree (Bessmennyy 1989, Razitel'nyy 1986,
Rezkiy 1990) o lontane (Kerch & Pytlivyy 1991). Restano da esplorare i fondi NARA non pubblicati su Commons.

**Risultati** (prima versione conservata in `extracted/navi/krivak2_v1/`; confronto in
`extracted/navi/krivak2/confronto.png` e `dettaglio_v1_v2.png`):

| | Prima versione | Versione 2 |
|---|---|---|
| Triangoli | 1.572 | 2.978 |
| Fianco: coincidenza della sagoma (alberi) | 0,403 | 0,474 |
| Prua: coincidenza della sagoma | 0,333 | 0,395 |
| Poppa aerea: errore sui punti | 11,3 px | 5,3 px |
| Fianco: errore sui punti | 3,8 px (5 punti) | 6,2 px (9 punti, con Rastrub e torri) |

**Limiti rimasti:** la tessitura resta "a macchie". Le foto sono riprese dall'alto e la nave sta quasi
tutta contro il mare, quindi foto e ombreggiatura si alternano dove il modello non coincide esattamente.
Lanciasiluri quadrupli non inseriti (posizione non verificabile sulle foto disponibili). Il passo
successivo utile sarebbe un ritocco manuale delle maschere sulla foto di fianco.

### Krivak II, versione 3: maschera ritoccata a mano sulla foto di fianco (22/10/2026)

**Strumento** `tools/navi/maschera_manuale.py` con specifica YAML (esempio:
`tools/navi/classi/krivak2_maschera_fianco.yaml`):
- `scafo`: poligono tracciato a mano (linea di coperta con il gradino a poppa, dritto di prua,
  galleggiamento con il bagnasciuga nero), dentro cui è nave ciò che è più scuro di `soglia_scafo`
  (esclude schiuma e onda di prua);
- `linea_coperta` e `inviluppo`: sopra la coperta, dentro un poligono largo attorno a sovrastrutture e
  alberi, è nave ciò che è più scuro di `soglia_sovrastrutture` (il mare alle spalle è chiaro);
- `includi`/`escludi` per le correzioni puntuali, `area_minima` e `buco_massimo` per la pulizia.

Produce la maschera e un'immagine di controllo (nave in rosso). Nella classe, `maschera_file` sulla foto
sostituisce soglie, riconoscimento del mare e sagoma del modello, sia nella calibrazione sia nella
proiezione. Tempo di tracciamento per la foto di fianco: circa 20 minuti, compresi due ritocchi.

Ritocchi necessari durante il tracciamento:
- a prua, chiazze di mare scuro sopra la coperta: inviluppo ristretto all'altezza del Rastrub (salvo
  l'asta di bandiera), soglia delle sovrastrutture da 160 a 140, `area_minima` 120;
- bordo grigio dell'onda di prua proiettato come scia chiara sulla murata: `soglia_scafo` da 175 a 135
  (la murata sta attorno a 81).

**Risultati** (versione 2 conservata in `extracted/navi/krivak2_v2/`; confronto a vista in
`extracted/navi/krivak2/dettaglio_v2_v3.png`, foglio completo in `confronto.png`):
- la calibrazione della foto di fianco ora si misura sull'intera sagoma: coincidenza 0,70 (prima
  misurabile solo sugli alberi sopra l'orizzonte, 0,47);
- parte della nave con dettaglio fotografico, prima e dopo: 30° 74→77%, 60° 76→79%, 90° 78→81%,
  120° 68→72%, 150° 56→59% (misurata con la maschera a soglia 175);
- a vista: murata uniforme, scomparse le macchie di mare scuro al galleggiamento e la scia dell'onda
  di prua.

**Cosa non risolve:** sulle sovrastrutture l'aspetto "a macchie" resta, perché viene in buona parte dal
dettaglio reale della foto (sovrastrutture molto articolate) e dal piccolo disallineamento residuo tra
modello e foto (errore sui punti 5,6 px). Ridurlo richiede un modello più dettagliato delle sovrastrutture,
non una maschera migliore. Restano anche un'ombra ondulata scura a prua vicino al galleggiamento e il
fatto che le foto di prua e di poppa usano ancora la maschera automatica.

## Classi costruite in parallelo (23/10/2026)

Ogni classe ha il suo rapporto in `extracted/navi/<classe>/RAPPORTO.md` (foto, misure, metriche,
problemi, limiti) e il foglio `confronto.png` con gli sprite originali del 1989.

| Classe | Foto | Errore punti (px) | Coincidenza della sagoma | Note |
|---|---|---|---|---|
| Kidd (DDG-993) | 5 (Kidd 1987–90, Chandler 1988–91) | 1,4–6,0 | 0,78–0,80 sulle foto con maschera | nessuna foto esattamente al traverso; alberetto poppiero modificato tra 1988 e 1990 |
| Nimitz (CVN-68) | 5 (Roosevelt 1986, Eisenhower 1982–87, Vinson 1982) | 1,5–10,7 | 0,93 di fianco, 0,87 di poppa (maschere a mano) | ponte di volo, isola, elevatori, sponsoni e 23 aerei in coperta come parti di classe |
| Iowa (BB-61) | 4 (tutte della Iowa, 1984–88) | 2,2–9,8 | 0,87 di fianco, 0,84 di poppa (maschere a mano) | torri triple, impianti binati da 5", lanciatori corazzati Tomahawk; quote ricavate proiettando sulla linea d'asse con la fotocamera calibrata; foto di prua del 1988 (non 1985) |
| Sacramento (AOE-1) | 4 (Sacramento 1988, Camden 1987, Seattle 1983) | 1,2–11,2 | 0,77–0,85 (maschere a mano per colore) | gru di rifornimento e bracci misurati come linee; nave chiara più luminosa di cielo e mare: maschere per colore (ora `maschera_manuale.py`, `metodo: colore`) |

### Correzioni al codice condiviso nate da questo lavoro
- **Altezza della vista**: era fissa a 46 m e tagliava alberi e antenne più alti di circa 41 m (Nimitz:
  albero a 63 m). Ora è `max(46 m, quota massima del modello × 1,08 + 2 m)`. La scala in pixel per metro
  è la stessa per tutte le classi, quindi le dimensioni relative restano corrette.
- **Limiti di calibrazione** (`camera.LIMITI`): punto mirato entro ±200 m in lunghezza e tra −10 e 100 m
  di quota (prima ±90 e −10/40, troppo stretti per le navi lunghe: la calibrazione si bloccava a 35–50 px).
- **Partenze multiple** riportate dentro i limiti fisici (`foto.py`, `triangola.py`): con una fotocamera
  iniziale a meno di 160 m il calcolo si fermava.
- **`verifica.json`**: parametri della fotocamera salvati con 5 decimali (con 2 decimali, a focali di
  ~54.000 px, lo scarto era di circa 5 px).
- **`fonti_dominanti.png`**: un colore distinto per ogni foto, con legenda a campioni.
- **Maschera calcolata sulla foto pulita** (scritte cancellate): prima i numeri di scafo lasciavano un
  "fantasma" negli sprite.
- **Chiusura sotto il dritto slanciato** nel generatore di scafi: prima restava una fessura visibile nelle
  viste di prua (Perry, Krivak II, Iowa). Perry e Krivak II ricostruite.

### Da migliorare nel codice condiviso (segnalazioni degli agenti)
- **Soglia del cielo** solo per navi più scure del cielo: servirebbe anche il caso opposto (navi chiare
  contro un cielo scuro). In parte coperto dalle maschere per colore.
- **Specchio di poppa solo verticale**: le poppe arrotondate (Iowa) lasciano un triangolo scuro a 120–180°.
- **Antenne rotanti** (SPS-48, SPS-49, Head Net): l'orientamento fisso produce macchie nella tessitura;
  da valutare un'animazione o un orientamento medio.
- **Fotocamere basse puntate verso l'alto** (Sacramento): l'elevazione minima di −5° resta stretta per
  riprese da sotto il ponte di un'altra nave. L'agente aveva ritagliato il cielo dalle foto come ripiego.

## Consolidamento del kit (23/10/2026)

Tre soluzioni inventate separatamente dagli agenti sono ora nel codice condiviso. Verifica: le mesh
delle sei classi (Perry, Krivak II, Kidd, Iowa, Nimitz, Sacramento) hanno lo stesso hash di prima, e le
maschere di Krivak II e Sacramento sono identiche bit per bit; le calibrazioni non cambiano perché il
nuovo vincolo agisce solo se la foto dichiara `orizzonte_punti`.

1. **Maschere per colore** in `maschera_manuale.py` (`metodo: colore`), dalla Sacramento: dentro
   l'inviluppo è nave ciò che è poco blu (B − (R+G)/2 < `soglia_blu`), con le opzioni per cieli non blu
   e per la schiuma. `classi/sacramento_maschera.py` è stato rimosso; la specifica della Sacramento
   usa lo strumento condiviso. Un file YAML può contenere più specifiche separate da `---`.
2. **Vincolo d'orizzonte** (`camera.scarto_orizzonte`, opzione `orizzonte_punti` delle foto), dalla
   Iowa: usato in `calibra_punti`, `calibra_sagoma` e in `triangola.py`; `verifica.json` riporta
   `errore_orizzonte_px`. Prova sulla foto di fianco della Iowa senza il punto virtuale dell'agente:
   senza vincolo la fotocamera finisce a −97 m, con tre pixel dell'orizzonte torna a 40 m (l'agente,
   con il punto virtuale, aveva 46 m), errore dei punti 2,9 px e coincidenza 0,858.
3. **Parti riutilizzabili in `kit.py`**: da Kidd `lanciatore_mk26`, `cannone_mk45`, `sps48`, `mack`,
   `harpoon` (Mk 141), `albero_palo`; da Iowa `prisma_xz`, `torre_trinata`, `impianto_binato`, `abl`,
   `fumaiolo_cappello`, `direttore_mk38`, `direttore_mk37`; da Nimitz `sponsone`, `sea_sparrow`,
   `aereo`, `aerei`, `antenna_piana`; da Sacramento `chiusura_dritto`, `stazioni_rifornimento`,
   `fumaiolo_cilindrico`, `direttore_mk91`, `gru`. Restano nei moduli delle classi solo le parti
   davvero proprie (scafo profilato, ponte di volo ed elevatori della Nimitz) e due varianti che
   ridefiniscono una parte del kit con lo stesso nome: `harpoon` della Iowa (contenitori su supporto
   inclinato) e `albero_palo` della Sacramento (con pennone). `classi/kidd_parti.py` è stato rimosso.

## Lotto sovietico (23/10/2026)

Cinque classi costruite in parallelo con il kit consolidato, passo di 15° (produzione a 5° fatta dopo il
secondo consolidamento, vedi sotto). Foto tutte di pubblico dominio (US Navy/DoD); dettagli in `extracted/navi/<classe>/RAPPORTO.md`.

| Classe | Configurazione | Foto proiettate | Errore dei punti (px) | Coincidenza | Note principali |
|---|---|---|---|---|---|
| Kara (Pr. 1134B) | 1977–1986 (Nikolayev, Petropavlovsk, Tashkent) | 4 | 3,8–9,9 | 0,67–0,85 | rivisto il 23/10: la foto di poppa (Tashkent 1985) è un teleobiettivo da ~6 km, calibrazione coerente; resta la coincidenza 0,67 (maschera automatica su cielo velato) e i dettagli delle sovrastrutture |
| Kirov (Pr. 1144) | Kirov capoclasse, Flotta del Nord, fino al 1990 | 3 | 2,7–7,7 | 0,88–0,91 | prima classe con `orizzonte_punti`; poppa dal Frunze 1990 (tessitura AK-130 sugli AK-100) |
| Slava (Pr. 1164) | Slava, con foto anche di Chervona Ukraina e Marshal Ustinov | 6 | 3,2–8,7 | 0,77–0,87 | Bazalt triangolati su 7 foto; nessuna foto di poppa vera |
| Koni (Pr. 1159T) | export "Koni II" (Algeria, Cuba) senza missili | 4 | 3,1–5,6 | 0,84–0,86 | nessuna foto di unità libiche; varianti 1159 e 1159TR descritte nel rapporto; riferimento di confronto: sprite KRIVAK |
| Nanuchka I (Pr. 1234) | Nanuchka I | 3 | 2,5–7,5 | 0,77–0,85 | foto di fianco di un Nanuchka III: sovrastrutture triangolate da due foto del Nanuchka I; gli sprite NUCHKA del 1989 non somigliano a un Nanuchka |

Nessuna delle cinque classi ha una foto di poppa vera da bassa quota di pubblico dominio (sul Kirov e sul
Koni le viste di poppa vengono da un'unità gemella o dall'ombreggiatura del modello).

### Segnalazioni degli agenti del lotto sovietico
Raccolte e in gran parte risolte nel secondo consolidamento (sezione seguente). Restano aperte:
- **Antenne rotanti** con orientamento fisso (Top Sail, Top Pair, Head Net, SPS-48).
- **Specchio di poppa solo verticale** e poppe arrotondate.
- **Foto di poppa**: per nessuna classe sovietica esiste una foto di poppa da bassa quota di pubblico dominio.

## Secondo consolidamento del kit (23/10/2026)

1. **Scafo** (`kit.scafo`), tutto facoltativo:
   - `profilo_prua: [[d, z], ...]`: dritto tabulato (curvo o concavo) al posto di quello rettilineo di `slancio`;
   - `prua_segue_dritto: true`: la larghezza delle sezioni cresce dalla distanza dal dritto alla stessa quota,
     quindi il piede di prua resta fine invece che tozzo (Koni: a 5 m dalla prua, al galleggiamento, da 1,4 m a 0);
   - `mat_specchio`: materiale dello specchio di poppa.
2. **Materiali di qualsiasi parte** (`costruisci.py`): `materiali: {da: a}` ricolora una parte; `mat` su una
   parte che non lo prevede vale `materiali: {sovrastruttura: mat}`.
3. **Maschere** (`maschera_manuale.py`):
   - `metodo: cielo`: nave chiara contro cielo grigio, anche più chiara del cielo, per scarto dal cielo della propria
     riga; sostituisce i tre script `kirov_maschera_prua.py`, `kirov_maschera_poppa.py` e `iowa_maschera_prua.py`
     (risultati identici per il Kirov, 2 pixel su 275.000 per la Iowa);
   - orizzonte inclinato (`orizzonte: [[x0, y0], [x1, y1]]`) per i metodi colore e cielo; `inviluppo` facoltativo;
     `chiusura` morfologica.
4. **Maschera automatica** (`foto.py`):
   - orizzonte inclinato (`orizzonte`);
   - `scarto_cielo: soglia` per navi più chiare o più scure del cielo;
   - con `maschera_file` valgono anche `escludi` (zone tolte dalla calibrazione, es. parti di un'altra variante)
     e `maschera_calibrazione` (maschera usata solo per calibrare).
5. **Calibrazione** (`camera.py`, `foto.py`):
   - limiti generali più larghi (distanza fino a 60 km, focale fino a 250.000 px);
   - per foto `limiti_fotocamera` ({azimut: [a, b] e elevazione: [a, b] in gradi, distanza: [a, b] in m,
     focale: [a, b] in px, mira_x, mira_z, rollio}) e
     `prior_fotocamera` (vincolo debole: elevazione, sigma_el, distanza, sigma_logD);
   - **avviso** quando un parametro finisce su un limite (stampato e salvato in `verifica.json`,
     `limiti_raggiunti`);
   - coincidenza `null` quando non è misurabile.
6. **Triangolazione** (`triangola.py`): `prior_camera` e `limiti_fotocamera` per foto, scarti di ogni punto
   con i sospetti segnalati, avvisi sui limiti, foto relative alla radice del progetto (secondo argomento
   facoltativo, altrimenti dedotta).
7. **Ombreggiatura di ripiego** più chiara: luce ambiente 0,55 invece di 0,30 (facce non viste da nessuna foto
   a ~0,25 invece di ~0,14 contro il livello medio 0,45 delle foto); per classe `luce_ambiente`.
8. **Parti spostate nel kit**: `tronco` (dal Kara; il Kirov tiene la propria versione con altra firma),
   `ak630`, `ak230`, `ak130`.
9. `commons_cerca.py`: campo `grande` con la miniatura da 1920 px; `costruisci.py` non stampa più l'avviso di
   `nanmean` sui blocchi vuoti.
10. Per gli agenti in parallelo: ogni agente usa una **sottocartella propria** della cartella temporanea.

Le mesh delle 11 classi restano identiche (tutte le novità dello scafo sono facoltative). Le maschere
già prodotte con lo strumento condiviso restano identiche. Gli sprite cambiano per la nuova ombreggiatura
di ripiego e, dove la calibrazione era bloccata sul limite della focale, per i limiti più larghi.
`classi/nimitz_sprite_alti.py` è superato dall'altezza automatica della vista.

## Produzione a 5° (23/10/2026)

Tutte le 11 classi (Perry, Krivak II, Kidd, Iowa, Nimitz, Sacramento, Kara, Kirov, Slava, Koni, Nanuchka) prodotte
con il kit del secondo consolidamento: 74 sprite per classe (da 0° a 180° ogni 5°, dritta e sinistra), 814 in
tutto, in `extracted/navi/<classe>/sprite/`, con fogli di confronto e aspetto continuo rigenerati. Calibrazioni
identiche a quelle della costruzione a 15°. Tempi su 24 core, classi in parallelo: da 110 s (Nanuchka) a
560 s (Nimitz). Unico avviso sui limiti: la foto di prua dello Slava (ripresa esattamente di prua: distanza al
minimo e punto mirato al massimo, una degenerazione attesa per quella geometria).
Comando, per una classe: `tools/venv/bin/python -I tools/navi/costruisci.py <radice> tools/navi/classi/<classe>.yaml 5`.

## Navi civili e Spruance (23/10/2026)

Quattro classi costruite in parallelo con il kit del secondo consolidamento, già prodotte a 5° ed esportate
per il motore (`data/motore/`). Una nave reale rappresentativa per tipo; dettagli nei `RAPPORTO.md`.

| Classe | Nave modellata | Tipo nell'originale | Foto proiettate | Errore dei punti (px) | Coincidenza | Note |
|---|---|---|---|---|---|---|
| Spruance (DD-963) | allestimento originale con ASROC Mk 16 (Leftwich, Caron, Ingersoll, Oldendorf 1982–87) | 0x00, al posto del Forrest Sherman | 5 | 3,7–6,3 | 0,56–0,84 | scansione della foto Leftwich stirata in verticale dell'11% (corretta su una copia) |
| Petroliera | Sea Isle City (1981, 231,8 × 44 m), Earnest Will 1987 | 0x10 | 1 (più la riflessa) | 3,3 | 0,90 | in zavorra; poppa tonda con un'estensione locale di `kit.scafo` |
| Mercantile | Ravenstvo (1963), tipo Leninskiy Komsomol, progetto 567 | 0x01 | 1 (più la riflessa) | 3,0 | 0,88 | in zavorra; poppa a incrociatore con uno scafo proprio |
| Cargo | Cpl. Louis J. Hauge Jr. (ex Estelle Mærsk, 1979), portacontainer/ro-ro MSC | 0x02 | 5 | 2,0–3,9 | 0,83–0,85 | nessuna foto di fianco a livello del mare; container solo nella foto del 1990 |

## Terzo consolidamento del kit (23/10/2026)

Dalle segnalazioni degli agenti di Spruance e navi civili. Tutto facoltativo: le mesh delle altre classi e le
34 maschere prodotte restano identiche (hash e confronto bit per bit); la calibrazione della Perry non cambia.

1. **Poppe** (`kit.scafo`):
   - `specchio_alto`: specchio a V, stretto al galleggiamento (`specchio`) e largo in coperta;
   - `lunghezza_poppa`: tratto su cui lo scafo si restringe verso lo specchio (prima fisso a 30 m);
   - `poppa_tonda: {lunghezza, pendenza, soglia}`: poppa che si stringe verso il galleggiamento fino all'asse
     (dalla petroliera, che prima sostituiva `kit.scafo` dal proprio modulo: mesh identica);
   - `scafo_composto`: poppa a incrociatore con la volta sopra il timone (dal mercantile).
2. **Scafo sostituibile**: `scafo: {generatore: <parte>, ...}` costruisce lo scafo con un'altra parte (es.
   `scafo_composto`) invece di `kit.scafo`, senza ridurre quello del kit a una lamina invisibile. Il mercantile
   lo usa: stessa mesh, meno 792 triangoli invisibili.
3. **Calibrazione** (per foto):
   - `aspetto_pixel`: altezza vera / altezza nella scansione (es. 0,90 per una scansione stirata dell'11%). La foto
     e le maschere si ricampionano in altezza e tutte le coordinate annotate si riscalano; si annota sempre sulla
     foto originale. Verificato sulla Leftwich: stesso risultato della copia corretta a mano (5,50 px, 0,680);
   - `assetto_nave: {beccheggio, sbandamento}` in gradi: il vincolo d'orizzonte vale anche con la nave che beccheggia;
   - `verticali: [[[u1, v1], [u2, v2]], ...]`: segmenti tracciati lungo alberi e spigoli verticali; devono puntare
     al punto di fuga delle verticali e vincolano il rollio delle foto quasi orizzontali. `verifica.json` riporta
     `errore_verticali_px`.
4. **Triangolazione** (`triangola.py`): elementi `simmetrico_di: altro` (stessa x e quota, y opposta, nessuna
   incognita propria); gli stessi `verticali`, `assetto_nave`, `aspetto_pixel` delle classi; fotocamere salvate in
   JSON (`uscita_fotocamere`, predefinito `<file>_fotocamere.json`) nel formato `fotocamera_iniziale`.
5. **Ombreggiatura di ripiego per classe**: `peso_ombra` (predefinito 0,12) e `albedo: {materiale: valore}`.
6. **Maschere**: `banda_orizzonte: k` nel metodo `cielo` (la banda sfocata dell'orizzonte non conta come nave);
   nuovo `contorno_modello.py` che proietta sulla foto calibrata il contorno del modello (o di alcune parti, es.
   `--parti scafo`) e lo stampa come poligono per `includi`/`inviluppo`.
7. **Avviso sui limiti** anche per l'azimut quando una foto ne restringe i limiti.
8. **Oggetti in volo** (aerei, elicotteri, missili): `vista: {volante: true, elevazioni: [0, 10, 25], centro: [x, y, z]}`.
   Il modello sta tutto sopra z = 0 (nessun taglio al galleggiamento) e ha il centro in `centro`; per ogni
   elevazione (l'angolo sotto cui l'osservatore vede l'oggetto sopra l'orizzonte, cioè dal basso) si producono gli
   sprite `aob_XXX_lato_eNN.png` (senza suffisso per l'elevazione 0), su tela quadrata, con l'ancora nel centro del
   modello. Le navi restano identiche (verificato bit per bit su uno sprite della Perry).

Applicazioni: cargo con **specchio a V** (`specchio` 0,25 → `specchio_alto` 0,88), verificato sovrapponendo i
contorni alla foto di poppa della Baugh (lo specchio verticale finiva fuori dallo scafo, in acqua); mercantile con
il generatore `scafo_composto`. Restano aperti: antenne rotanti con orientamento fisso; nessuna foto di poppa
da bassa quota per le classi sovietiche.

## Unità in volo e sottomarini (23/10/2026)

Quattro lotti in parallelo, già prodotti a 5° ed esportati per il motore. Gli oggetti in volo hanno 222 sprite
(3 elevazioni); i sottomarini 74. Dettagli nei `RAPPORTO.md`.

| Lotto | Classi | Errore dei punti (px) | Coincidenza | Note principali |
|---|---|---|---|---|
| Elicotteri | SH-60B (5 foto), Ka-25PL (4) | 2,0–21,6 | 0,55–0,90 | rotori come nuvola di quadratini (la retinatura dei rotori del 1989); foto del Ka-25 di dritta da un museo (CC0) |
| Aerei | P-3C, Il-38, Tu-142MK (3 foto ciascuno) | 2,5–18,5 | 0,48–0,87 | dischi delle eliche opachi (da rendere semitrasparenti); nessuna foto di coda o di muso per Il-38 e Tu-142 |
| Sottomarini | Los Angeles (3), Victor III (1), Kilo (3) | 2,7–17,7 | 0,64–0,98 | nuova parte `scafo_sottomarino` (in `classi/sottomarini_parti.py`); toni grigio medio invece che neri |
| Missili | Tomahawk (2), Harpoon (1), P-700 (nessuna: solo ombreggiatura), BQM-74C (1) | 6,4–11,9 | 0,76–0,89 | sprite minuscoli (Tomahawk 20×4 px di fianco); licenza delle foto Convair di affidabilità media |

## Quarto consolidamento del kit (23/10/2026)

Dalle segnalazioni dei lotti di elicotteri, aerei, sottomarini e missili. Le navi restano identiche (hash delle mesh
di 13 classi, uno sprite della Perry bit per bit); le 71 maschere dello strumento condiviso restano identiche.

1. **Materiale `retinato`** per eliche e rotori in movimento: niente tessitura dalle foto (solo ombreggiatura), escluso
   dalla calibrazione e dalle occlusioni, e disegnato a scacchiera sopra cio' che sta dietro (due passate con la
   stessa inquadratura, alternate pixel per pixel: come i rotori retinati degli sprite del 1989). `elica` del kit
   lo usa per default (prima i dischi delle eliche erano opachi).
2. **`escludi_parti: [nomi]`** per foto: le parti indicate non entrano nella calibrazione ne' ricevono tessitura da
   quella foto (al posto di doppie maschere e di centinaia di rettangoli `escludi`).
3. **Tono per classe**: `tono` (livello medio, predefinito 0,45) e `contrasto` (0,17) a cui si portano le foto e
   l'ombreggiatura di ripiego; i sottomarini usano 0,22 e 0,12 (scafi neri, prima grigio medio).
4. **`affondamento`** per foto (m): la nave in quella foto sta piu' bassa (o piu' alta, se negativo) del modello.
5. **Oggetti in volo**: limiti predefiniti di elevazione (−80/+80°) e rollio (±60°) e partenze multiple anche dal
   basso; `assetto_nave: {quota_mare: z}` per il vincolo d'orizzonte quando il modello non ha il mare a z = 0;
   `solo_punti: true` per calibrare sui soli punti.
6. **Maschere**: nel metodo `cielo`, `colore: true` (distanza nel colore RGB, per livree colorate) e
   `fondo: polinomio` con `grado` (cieli vignettati o a gradiente). `contorno_modello.py --maschera f.png
   [--erodi k]` scrive la sagoma proiettata come maschera (anche per sagome a piu' pezzi).
7. **Foglio di confronto**: per gli oggetti in volo `elevazioni.png` (una riga per elevazione e lato, scala fissa 2x)
   e scena di prova con l'oggetto sopra l'orizzonte.
8. **Parti generiche nel kit**, in quattro moduli registrati in `kit.PARTI` (`kit_sottomarini`, `kit_missili`,
   `kit_aerei`, `kit_elicotteri`); i moduli delle classi ne riesportano i nomi originali. Nomi in conflitto con
   suffisso: `pinna_sottomarino`, `pinna_missile`, `fusoliera_aereo`, `fusoliera_elicottero`, `ellissoide_aereo`,
   `ellissoide_elicottero`, `corpo_aereo`.

Restano aperti: triangolazione con foto aeree senza vincoli espliciti (usare `prior_camera`/`limiti_fotocamera`),
antenne rotanti con orientamento fisso, foto di poppa a bassa quota delle unita' sovietiche.

## Opera viva e modelli per il motore (23/10/2026)

Per le navi danneggiate il motore genera gli sprite al volo dal modello 3D (`docs/10_danni_affondamento_esplosioni.md`
§5.1). Due strumenti nuovi, che non toccano calibrazione e sprite precalcolati:
- **`tools/navi/opera_viva.py`:** carena sotto il galleggiamento, eliche con assi, timoni, cupola del sonar e bulbo
  per le 15 navi di superficie; per gli 11 sottomarini, scafo e appendici senza il taglio al galleggiamento piu' le
  eliche e le appendici di poppa sommerse (24/10/2026, sotto).
  - La carena parte dai vertici a z = 0 della parte `scafo`, quindi segue qualunque generatore (`scafo`,
    `scafo_composto`, poppe tonde). Scende alla chiglia con sezioni a superellisse, piu' a V verso le estremita',
    con piede di prua e specchio immerso.
  - I dati per classe (`OPERA_VIVA`: pescaggio dello scafo, eliche, timoni, cupola, bulbo) sono stime da schede
    pubbliche: affidabilita' media per pescaggi ed eliche, bassa per le forme.
  - `costruisci.modello_intero(cl)` restituisce il modello con l'opera viva; `costruisci_mesh` resta quello di
    sempre.
- **`tools/navi/esporta_modello.py`** (in blocco: `tools/navi/esporta_modelli.sh`) scrive
  `extracted/navi/<classe>/modello.mdl`.
  - Usa le fotocamere gia' calibrate di `verifica.json` (`Foto.usa_fotocamera`), senza nuova calibrazione.
  - Cuoce in una tessitura (texel di 0,15 m) il valore delle foto e la loro quota rispetto all'ombreggiatura di
    ripiego.
  - **Una tessitura per ciascuna faccia di ogni triangolo**, come negli sprite, dove una foto colora solo la
    faccia rivolta all'osservatore. Senza questa separazione la foto di un fianco "dipingeva" anche l'altro
    attraverso la tolleranza di profondita' (circa 15 m a quella distanza). Le facce che nessuna foto vede hanno un
    texel solo.
  - **Verifica:** con `--verifica` ridisegna tre viste dal modello cotto e le confronta con gli sprite
    (`verifica_modello.png`).
  - **Risultato sulle 26 classi:** sagoma 0,98–1,00, scarto medio 0,05–0,31 livelli di grigio, il 96–100% dei
    pixel entro un livello.
  - **Peso** (formato `MDL2`, 24/10/2026): 0,5–4,3 MB a classe, 39 MB in tutto contro i 203 MB del primo formato
    (`MDL1`); `data/motore` scende da 330 a 172 MB. Vedi sotto.
- **Codice condiviso:**
  - `Foto._finalizza` separa la parte finale della calibrazione (maschera, profondita', livelli);
  - `costruisci.cfg_foto` prepara la configurazione di una foto;
  - in `ALBEDO` entrano `carena` (0,30) ed `elica` (0,40).
- **Limiti:** le navi non hanno chiglie di rollio ne' skeg; le appendici di poppa dei sottomarini sono stime.

### Formato MDL2 e appendici di poppa dei sottomarini (24/10/2026)

- **Peso dei modelli.** Quasi tutto il peso era nella tessitura: Nimitz 31 MB su 32, cargo 26 su 27. Il 70% dei
  texel era vuoto: spazi dell'impacchettamento a scaffali, meta' di ogni rettangolo fuori dal triangolo, parti di
  facce che nessuna foto vede.
  - **`MDL2`** (formato nel docstring di `esporta_modello.py`), senza perdite rispetto alla tessitura a 8 bit:
    record dei triangoli per piani di byte, quota delle foto intera, valore delle foto solo nei texel con quota
    > 0 come scarto dalla predizione MED (LOCO-I) dei vicini visti. Tutti e tre compressi con DEFLATE grezzo.
  - Nimitz 31,8 -> 3,2 MB, cargo 26,4 -> 3,5 MB, Perry 6,3 -> 1,1 MB, sottomarini 2,4–5,7 -> 0,5–1,5 MB.
  - Dove la quota arrotondata e' 0 anche il valore si scrive 0 (il formato non lo salva); la verifica usa le
    tessiture del file. Scarti invariati entro 0,01 livelli.
  - **Motore:** `Modello::carica` usa un decompressore DEFLATE proprio (`engine/nucleo/include/nucleo/inflate.hpp`, niente
    zlib), poi rifa' la predizione. Caricamento 0,1–0,3 s; in memoria la tessitura resta intera.
  - **Sprite precalcolati** (`SPR2`, `tools/pipeline/esporta_motore.py`): indici di ogni sprite compressi con lo
    stesso DEFLATE, 107 -> 11 MB per le 35 classi. Pixel identici a quelli di `SPR1` (controllati sprite per
    sprite); i filtri per riga o per colonna comprimono peggio.
    I 35 insiemi si caricano in 0,75 s in tutto. Con modelli e sprite `data/motore` passa da 330 a 79 MB.
  - **Fondali** (`FDL2`, 24/10/2026): stesso DEFLATE dopo la predizione a piano dei valori a 16 bit, 32 -> 11,8 MB
    per i 7 teatri, valori identici. `data/motore` scende a 49 MB.
  - Andata e ritorno controllata su tutte le 26 classi: il valore letto dal motore ridà esattamente gli scarti
    del file, triangoli e quote coincidono.
  - **Scartate**, perche' cambiano l'aspetto:
    - texel piu' grande (0,2 m: Perry da 0,22 a 0,28 livelli di scarto, 96% entro un livello nella vista a 150);
    - texel adattato allo scorcio delle facce nelle viste degli sprite: le coperte, quasi invisibili a nave
      dritta, si vedono quando la nave sbanda o si capovolge;
    - texel adattato alla risoluzione delle foto: quasi tutte sono gia' sotto 0,15 m/pixel sulla nave.
- **Appendici di poppa dei sottomarini** (`opera_viva.coda_sottomarino`, dati in `OPERA_VIVA[...]['coda']`).
  I YAML descrivono solo la parte emersa: ai sottomarini mancavano timone inferiore e piani poppieri.
  - Coda a croce (+) per tutte le 11 classi: nessuna ha la coda a X.
  - Le parti nuove sono `timone_inferiore`, `timoni_poppieri` e, sul 688, `piastre_timoni`: le piastre
    verticali alle estremita' dei piani.
  - **Pianta:** quella del timone superiore del YAML riflessa rispetto all'asse dello scafo (bordo d'attacco,
    corde e freccia alla stessa distanza dall'asse); radici sull'asse, dentro lo scafo.
  - **Asse:** misurato sullo scafo intero a meta' corda (fondo dello scafo piu' il semiasse verticale).
  - Estremita' del timone inferiore e semiapertura dei piani dall'asse, in metri:

    | classe | inferiore | piani |
    |---|---|---|
    | 688 | 5,2 | 5,6 (piastre di 2,4 m) |
    | Victor III | 5,0 | 5,8 |
    | Kilo | 3,2 | 4,6 |
    | Alfa | 5,0 | 5,5 |
    | Akula | 6,0 | 7,5 |
    | Oscar II | 7,0 | 9,5 |
    | Charlie II | 5,0 | 5,5 |
    | Typhoon | 8,0 | 12,5 |
    | Foxtrot | 3,5 | 4,5 |
    | Sturgeon | 4,8 | 5,0 |
    | Trafalgar | 5,5 | 5,5 |

  - Eccezioni alla pianta riflessa:
    - **Kilo:** il timone superiore e' la coda del cassero, quindi la pianta e' data a parte (`coda['timone']`).
    - **Oscar II:** il timone superiore e' una carenatura lunga 17 m; pianta propria (corda 8 m, radice a 132 m
      dalla prua, davanti alle due eliche).
    - **Typhoon:** piani propri a poppa delle due eliche (radice a 164 m, corda 7 m); timone inferiore riflesso.
  - **Affidabilita' bassa:** le schede `RAPPORTO.md` danno solo il timone superiore. Le misure sono stime dalle
    proporzioni delle foto in bacino, piani larghi circa quanto lo scafo.
  - Sono tutte sotto il galleggiamento: gli sprite a nave intatta non cambiano. Si vedono quando il sottomarino
    si apprua, si capovolge o si spezza (fase D6).

## Sezioni e compartimentazione per la fisica dei danni (23/10/2026)

`tools/navi/compartimenti.py` taglia il modello con l'opera viva in 60 stazioni (semilarghezze ogni 0,25 m) e
scrive `extracted/navi/<classe>/scafo.toml` per le 15 navi di superficie.
- **Compartimentazione:** dalla tabella `CLASSI` (stime di gioco), con depositi, macchine, stive e cisterne alle
  distanze dalla prua dei lanciatori, dei cannoni e dei fumaioli del modello. Il GM intatto e il dislocamento
  pubblicato sono per il controllo.
- **Taratura dell'opera viva:** `opera_viva.py` tara la pienezza delle sezioni sul dislocamento pubblicato
  (`D` in `OPERA_VIVA`). Senza taratura le navi militari spostavano dal 20 al 50% in piu'. Dopo la taratura le
  sezioni militari sono piuttosto a V, perche' compensano un galleggiamento del kit un po' pieno.
- **Larghezza:** quella al galleggiamento si ricava dalle sezioni, perche' alcuni YAML hanno uno scafo fittizio
  (la Nimitz ha `B: 0.1`).
- **Velocita':** il taglio delle stazioni e' vettoriale (numpy). Con i 15–37 mila triangoli dei sottomarini il
  ciclo in Python impiegava minuti; i file delle navi di superficie restano identici.

### Sottomarini (24/10/2026, base della fase D6)

`compartimenti.py` scrive `scafo.toml` anche per gli 11 sottomarini (tabella `SOTTOMARINI`).
- **Sezioni:** quelle dello scafo esterno del modello intero, quindi con le appendici di poppa, ma senza il
  cassero, che e' una sovrastruttura allagabile e non da' spinta. In emersione il motore li tratta come navi a
  sezioni tonde.
- **Compartimenti:**
  - 688, Sturgeon e Trafalgar: casse zavorra a prua e a poppa, compartimento prodiero (siluri, centrale,
    alloggi), reattore, macchine (dossier 02 §9.3).
  - Sovietici: dal numero di compartimenti noto (Kilo 6, Alfa 6, Victor III 7, Foxtrot 7 con i siluri di
    poppa, Akula 8, Charlie II 8, Oscar II 10). Le posizioni sono stimate, con la centrale sotto la vela.
  - Typhoon: modulo dei siluri, silos fra i due scafi, centrale, reattori, macchine e modulo di poppa.
  - Nei doppi scafi l'intercapedine ai fianchi di ogni compartimento e' una cella di zavorra per lato.
- **Scafo resistente per stazione** (raggio, quota, distanza dall'asse di simmetria):
  - negli scafi singoli coincide con lo scafo esterno fra le casse zavorra;
  - nei doppi e' lo scafo esterno meno un'intercapedine, tarata perche' lo scafo resistente sposti il 90% del
    minore fra dislocamento in emersione della geometria e dislocamento pubblicato; risulta 0,4–1,2 m;
  - Typhoon: due scafi da 7,2 m a ±6,4 m, quanto consente la larghezza con i silos in mezzo, piu' i moduli di
    prua e di poppa.
- **Quote:** collaudo dal dossier 01; collasso 1,5 volte il collaudo dove manca (regola del dossier 02 §9.3).
- **Controllo stampato da `compartimenti.py`:**

  | | emersione geom./pubbl. (t) | riserva geom./pubbl. | intercapedine (m) |
  |---|---|---|---|
  | 688 | 6126 / 6082 | 0,15 / 0,14 | 0 (singolo) |
  | Sturgeon | 4279 / 3698 | 0,17 / 0,28 | 0 (singolo) |
  | Trafalgar | 4085 / 4700 | 0,23 / 0,12 | 0 (singolo) |
  | Victor III | 4291 / 4950 | 0,45 / 0,47 | 0,82 |
  | Kilo | 2812 / 2325 | 0,53 / 0,32 | 1,21 |
  | Alfa | 2344 / 2300 | 0,69 / 0,38 | 1,00 |
  | Akula | 10197 / 8140 | 0,21 / 0,57 | 1,11 |
  | Oscar II | 18394 / 14700 | 0,42 / 0,32 | 0,65 |
  | Charlie II | 4564 / 4300 | 0,35 / 0,23 | 0,42 |
  | Typhoon | 38866 / 23200 | 0,35 / 1,07 | 1,00 (diametri dati) |
  | Foxtrot | 3254 / 1983 | 0,28 / 0,27 | 1,04 |

- **Taratura della riserva di spinta** (`tara_riserva`, 24/10/2026). La riserva della geometria viene dal
  galleggiamento delle foto e si scostava molto da quella pubblicata. Il galleggiamento resta quello delle foto
  e degli sprite: si tara solo cio' che usa la fisica.
  - **Geometria con troppa riserva** (Kilo, Alfa, Oscar II, Charlie II, Trafalgar): sopra il galleggiamento c'e'
    scafo leggero allagabile (raccordi, estremita' senza casse). Le semilarghezze sopra z = 0 si restringono di
    un fattore, raccordato sui primi 0,5 m perche' il galleggiamento non cambi, mai dentro lo scafo resistente.
    Fattori: Kilo 0,46, Alfa 0,15, Oscar II 0,72, Charlie II 0,44.
  - **Geometria con poca riserva** (Akula, Typhoon, Sturgeon): nelle foto il battello e' basso sull'acqua, con le
    casse zavorra in parte piene. L'acqua iniziale (`acqua_iniziale_m3`) sta nelle celle di zavorra in proporzione
    al loro volume sotto il galleggiamento: Akula 2192 t (casse piene), Typhoon 13 561 t, Sturgeon 368 t.
  - Il Typhoon ha ora celle proprie per i due scafi resistenti, con lo spazio fra i due come zavorra (i silos dei
    missili dove ci sono).
  - **Motore:** l'acqua iniziale si toglie dal peso del battello stazione per stazione, con il baricentro
    corretto, quindi l'equilibrio intatto non cambia. Le pompe non la toccano e non conta come acqua imbarcata.
    Quando il battello imbarca acqua l'equipaggio esaurisce le casse integre (10 m³/s per cassa, dossier 02
    §9.2).
  - Fra casse zavorra e compartimenti non ci sono perdite di paratia: e' lo scafo resistente.

  | | riserva geometrica | dopo la taratura | pubblicata |
  |---|---|---|---|
  | 688, Victor III, Foxtrot | 0,15 / 0,45 / 0,28 | invariata | 0,14 / 0,47 / 0,27 |
  | Kilo | 0,53 | 0,32 | 0,32 |
  | Alfa | 0,69 | 0,38 | 0,38 |
  | Akula | 0,21 | 0,55 | 0,57 |
  | Oscar II | 0,42 | 0,32 | 0,32 |
  | Charlie II | 0,35 | 0,23 | 0,23 |
  | Typhoon | 0,35 | 1,07 | 1,07 |
  | Sturgeon | 0,17 | 0,27 | 0,28 |
  | Trafalgar | 0,23 | 0,20 | 0,12 |

  Il Trafalgar resta sopra la riserva pubblicata, che e' bassa e incerta (dossier 01: 4500–4800 / 5200–5300 t):
  con lo scafo singolo, sopra il mare c'e' quasi solo scafo resistente.
- **Effetto in emersione:**
  - un Mk 48 sotto la chiglia spezza e affonda tutti i sottomarini, tranne il Typhoon;
  - 300 kg a contatto affondano tutti tranne il Typhoon;
  - un siluro leggero a contatto affonda 688, Sturgeon, Trafalgar, Alfa e Charlie II; Kilo, Victor III, Akula,
    Oscar II, Foxtrot e Typhoon restano a galla esaurendo le casse.
- **Limiti** (affidabilita' bassa):
  - lo scafo resistente del Typhoon sposta solo il 45% del dislocamento pubblicato in emersione;
  - celle a tutta sezione (o a fasce di y): lo scafo resistente come confine circolare dell'allagamento e'
    lavoro di D6.

### Lezioni ricorrenti
- Su Commons le foto esattamente al traverso e a livello del mare sono rare anche per le navi USA: spesso
  la migliore è a 45–55° dalla prua.
- Le maschere tracciate a mano rendono la calibrazione misurabile sull'intera sagoma e vanno usate sulle
  foto principali.
- Gli errori più costosi sono stati di identificazione dei punti (lato sinistro e lato dritto, spigoli
  simili): controllare sempre gli scarti dei singoli punti.
- Le parti asimmetriche (isola della Nimitz, elevatori) impediscono di usare la foto riflessa per l'altro
  lato: in quei casi servono foto vere di entrambi i lati.
- Con più agenti in parallelo il server di Commons limita la frequenza (errore 429): servono pause lunghe
  e il download degli originali uno alla volta.
- Con più agenti in parallelo ciascuno deve lavorare in una sottocartella propria della cartella temporanea.
- Quando la foto di fianco ritrae un'altra variante (Nanuchka III invece di I), conviene triangolare le parti
  diverse dalle foto della variante giusta e mascherare sulla foto di fianco solo le parti comuni.

## Sagome delle armi e dei tubi (25/10/2026)

`engine/grafica/sagome_armi.hpp` disegna le armi di `carico.toml` di fianco, in scala e con la prua a destra,
per la sala siluri, il pannello Armi e le rastrelliere dello spaccato. L'aspetto viene dalle foto e dalle
schede che seguono; le voci senza foto sono STIMA.

| Arma | Aspetto disegnato | Fonte |
|---|---|---|
| Mk 48, Mk 48 ADCAP | Ogiva nera e sezioni verde scuro, con fasce bianche ai giunti (sull'ADCAP fasce coi triangoli neri). Propulsore a idrogetto intubato, con quattro alette davanti. In esercitazione la metà poppiera è arancione. | Commons: «Mk 48 torpedo maintenance 1982», «Mk 48 torpedo on retriever boat 1982», «Pasadena (SSN-752) weapons»; nuclearcompanion.com, *Mark 48 Torpedo* (gruppi e propulsore) |
| SET-65, 53-65K, SAET-60M, USET-80, TEST-71M | Verde oliva. Coda conica con alette a croce, timoni e due eliche controrotanti. Ogiva tonda sul 53-65K, muso piatto col trasduttore sugli autoguidati. In esercitazione testa rossa e corpo grigio. | Commons: «53-65K torpedo at Park Patriot», memoriale di Kaliningrad («Торпеда и мина-ракета»), «53-65K torpedo MW» |
| Mk 67 SLMM | Corpo del Mk 37 da 485 mm, verde chiaro, con alette a croce ed elica singola; davanti la sezione mina. La sezione in oliva con la fascia gialla dell'esplosivo è STIMA (MIL-STD-709). | Wikipedia, *Submarine Launched Mobile Mines*; globalsecurity, *Mk 67*; Commons: «Mark 37 Torpedo» |
| Mk 60 CAPTOR | Capsula kaki da 3,35 m con fasce scure, sezione dell'ancora a poppa e calotta dorata a prua. Una finestra da tavola tecnica mostra il Mk 46 dentro. | Wikipedia, *Mark 60 CAPTOR*; Commons: «Mk.60 Captor», «CAPTOR mine being loaded aboard sub» |
| PMR-1, PMR-2 | Mina-razzo ancorata «Kal'mar»: cilindro grigio chiaro da 7,8 m con fori e ribattini. Tre parti: ancora, sezione strumenti col cavo, razzo. | ru.wikipedia, *ПМР-1*; allmines.net, *ПМР-1* (534 mm, 7800 mm, 1700 kg); Commons, memoriale di Kaliningrad |
| MDT | Mina di fondo corta, oliva con fascia gialla: STIMA. | Nessuna foto trovata |
| Harpoon (UGM-84) | Capsula nera dal muso tondo con il coperchio separato e la coda alettata. Lunga quanto un Mk 48 (5,8 m, STIMA). | designation-systems.net, *AGM/RGM/UGM-84*; globalsecurity, *AGM-84* («the same size and general shape of a blunt nosed torpedo») |
| Tomahawk (TASM, TLAM-C) | Capsula d'acciaio lucido con due file di feritoie, lunga 6,4 m (missile e booster da 6,25 m più i fondi). | Commons: «Defense.gov News Photo 990331-N-1556A-001» (sala siluri dell'USS Norfolk) |
| Sea Lance | Capsula grigia generica (arma «what-if»). | — |
| Shkval, Vyuga | Shkval: corpo oliva con il disco del cavitatore e l'ugello. Vyuga: corpo grigio con l'ogiva oliva. Entrambi STIMA. | — |
| ADC Mk 2, MG-84 | ADC Mk 2: cilindro bianco da 3 pollici con l'anello dell'elica e l'ogiva arancione. MG-84: grigio con fascia rossa. Entrambi STIMA. | navysbir.com, N191-023 (diametro dell'ADC) |

**Palette.** La tavolozza fissa usa solo indici che `INDICI_VISTA` non illumina (0–15, 80–95, 160–195,
254), quindi funziona anche di notte nel pannello Armi. La tavolozza ricca ridefinisce i posti 96–143 per le
schermate a tutto schermo.

## Aerei ed elicotteri dell'ASW aereo (25/10/2026)

Pezzo A4 di docs/13_asw_aereo.md. Gli sprite di s3a, s3b, sh3h, sh60f, ka27 e be12 sostituiscono i segnaposto
di `tools/pipeline/esporta_motore.py`. Sono tutti oggetti in volo, prodotti a 5° su tre elevazioni.
- SH-60F: la cellula e le foto calibrate dell'SH-60B, senza radome APS-124, lanciatore di boe e MAD.
- S-3A e S-3B, SH-3H, Ka-27PL, Be-12: ricostruiti sulle foto (seconda passata dello stesso giorno), con la
  geometria corretta sulle misure lette nelle foto. Ogni RAPPORTO.md dà foto, licenze e scarti; le foto scartate
  sono indicate col motivo.
  - S-3A: due foto della US Navy (7,4 e 27,8 px). L'S-3B riusa modello e foto.
  - SH-3H: due foto della US Navy (10,4 e 16,1 px). Mozzo più alto e galleggianti più avanti del primo modello.
  - Ka-27PL: una foto della US Navy, specchiata (2,3 px). Rotori coassiali a 1,4 m, derive alte 2,5 m, radome
    ventrale lungo.
  - Be-12: una foto del museo di Monino, CC BY-SA 2.0 di Alan Wilson, specchiata (8,6 px); lo sprite è CC BY-SA.
    Corretti il radome a ditale in cima al muso, le gondole lunghe sopra il ginocchio, l'anedro dell'ala esterna,
    le derive ellittiche a ±5,9 m e il boma del MAD.
- Il nuovo `tools/navi/maschera_colore_modello.py` fa la maschera di una foto d'aereo dalla sagoma del modello
  calibrato, senza i dischi dei rotori. Ci interseca il fondo «non blu» quando il colore separa; altrimenti usa la
  sola sagoma ristretta.
- Lezione: le foto da vicino col grandangolo, o con l'aereo inclinato verso l'obiettivo, calibrano bene solo con una
  fotocamera iniziale realistica e con `limiti_fotocamera` su distanza e focale. Altrimenti la calibrazione scappa
  verso un teleobiettivo lontano.
