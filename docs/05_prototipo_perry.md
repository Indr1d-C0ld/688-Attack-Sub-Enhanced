# Prototipo grafico: fregata Oliver Hazard Perry

Obiettivo: decidere lo stile delle unità a 640×480 prima di estenderlo alle oltre 30 classi.
Confronta gli sprite originali del 1989, foto reali d'epoca trattate e un modello 3D di prova.

## Risultati (`extracted/prototipo_perry/`)

| File | Contenuto |
|---|---|
| `confronto.png` | Sprite originali 1989, foto trattate, modello di prova e modello con foto proiettata, nelle 5 viste dell'originale (viste da dritta, prua a destra) |
| `aspetto_continuo_foto.png` | Modello con foto proiettata, lato sinistro, ogni 15° di angolo sulla prua |
| `scena_periscopio_foto.png` | Prova di scena al periscopio a 640×480 con la palette originale (valori indicativi) |
| `museo/<vista>_epoca16.png` | Foto trattate: 16 grigi della palette originale (indici 144–158), retinatura Floyd–Steinberg |
| `museo/<vista>_svga64.png` | Foto trattate: 64 grigi, retinatura ordinata leggera |
| `periscopio/aob_<gradi>_{dritta,sinistra}.png` | Sprite del modello di prova da 0° a 180°, ogni 15°, sui 16 grigi; indice 159 trasparente |
| `periscopio_foto/aob_<gradi>_{dritta,sinistra}.png` | Sprite del modello misurato con foto proiettata, renderizzati direttamente da ciascun lato |
| `periscopio_foto/coincidenza_*.png`, `verifica.json` | Controllo della sagoma di fianco contro la foto (rosso = solo modello, verde = solo foto) |
| `foto/` + `foto/fonti.json` | Foto sorgente (960 px) con titolo, pagina, licenza, data e autore |

Strumenti in `tools/museo/`: `commons_cerca.py` (ricerca su Wikimedia Commons con i metadati di
licenza, senza scaricare), `digitalizza.py` (trattamento d'epoca), `perry3d.py` (primitive, modello
di prova e resa), `maschera_foto.py` (maschera nave/sfondo e calibrazione della foto di fianco),
`perry3d_foto.py` (modello misurato e proiezione della foto), `confronto_perry.py` (fogli di confronto).

## Fonti delle foto

Sei fotografie ufficiali del 1984–85 (cantieri Bath Iron Works e Todd Pacific per la US Navy;
Defense Visual Information Center), conservate dai National Archives (NARA) e pubblicate su
Wikimedia Commons come **pubblico dominio** (opere del governo federale USA). Elenco completo in
`foto/fonti.json`.

| Vista | Nave | Data | Adatta al periscopio? |
|---|---|---|---|
| Fianco sinistro | USS Nicholas (FFG-47) | 1984 | **Sì**, ripresa a livello del mare |
| Poppa 3/4 | USS Elrod (FFG-55) | 19/3/1985 | Sì, quasi a livello del mare |
| Prua | USS Simpson (FFG-56) | 25/6/1985 | Parzialmente, ripresa leggermente dall'alto |
| Prua 3/4 | USS Simpson (FFG-56) | 25/6/1985 | No, ripresa dall'alto (fiume Kennebec) |
| Poppa | USS Elrod (FFG-55) | 19/3/1985 | No, ripresa aerea |
| Fianco destro | USS Crommelin (FFG-37) | 15/3/1985 | No, dal ponte di una portaerei |

Tutte le navi sono a scafo lungo (LAMPS III), coerenti con il 1989.

## Modello 3D di prova

Scafo con cavallino, svasatura, prua affilata e specchio di poppa, più circa 18 primitive per le
sovrastrutture: lanciatore Mk13, plancia con finestre, radome Mk92, albero a traliccio, antenna
SPS-49, cannone Mk75, STIR, fumaiolo, hangar, Phalanx. In totale 1.266 triangoli.
- Dimensioni dello scafo dal dossier: 138,1 × 13,7 m.
- Posizioni e altezze delle sovrastrutture sono **stime dalla foto di fianco**: albero a circa
  31–34 m sul galleggiamento. Vanno verificate sui piani d'epoca prima dell'uso definitivo,
  perché l'altezza dell'albero serve allo stadimetro.

Resa: rasterizzatore software con z-buffer, occhio a 2 m sul mare, distanza 4 km (proiezione
quasi ortografica, come al periscopio), parti sotto il galleggiamento escluse, normali orientate
verso l'osservatore. Sole alle spalle e in alto (parametro, legabile all'ora del giorno).
Sovracampionamento 3×3, quantizzazione sui 16 grigi della palette originale con retinatura
ordinata. Tempo: circa 4 s per 13 viste.

## Valutazione

- **Foto per il Museo e i briefing**: risultato già convincente in entrambe le varianti; stile
  coerente con le foto digitalizzate dell'originale, con il doppio della risoluzione.
- **Foto per il periscopio**: solo 2 viste su 6 sono riprese abbastanza in basso. Coprire tutte
  le angolazioni con foto reali non è realistico, ed è anche il limite dell'originale, fermo a 5 viste.
- **Modello 3D**: geometria, scala e aspetto continuo sono corretti, e con l'aspetto continuo l'angolo
  sulla prua si stima davvero. Con sole primitive piatte però è molto meno ricco delle foto e anche
  degli sprite originali, che sembrano foto di modellini in scala.

## Opzioni per la produzione

1. **Foto per il Museo, modello 3D dettagliato per il periscopio.** Più geometria (traliccio
   vero, ringhiere, antenne, imbarcazioni) e ombreggiatura migliore. Costo: alcune ore di
   modellazione per classe.
2. **Modello 3D texturizzato dalle foto (consigliata).** Proiettare sulle superfici del modello le
   foto riprese a livello del mare (fianco, poppa di 3/4) e usare la geometria solo per forma,
   scala e angolazione. Unisce la ricchezza della foto all'aspetto continuo. Serve almeno una buona
   foto di fianco per classe, quasi sempre disponibile.
3. **Solo foto, più viste specchiate.** Il più rapido e fedele all'originale, ma resta a poche
   viste fisse e dipende dalla disponibilità di foto a livello del mare.

## Seconda prova: modello misurato con foto proiettata (strada consigliata)

**Calibrazione della foto di fianco** (USS Nicholas, `foto/fianco_sinistro_calibrazione.json`):
prua x = 70, poppa x = 860, galleggiamento y = 435 nell'immagine a 960 px, quindi 0,175 m/pixel.
La maschera nave/sfondo usa una soglia contro il cielo sopra l'orizzonte e una contro il mare tra
orizzonte e galleggiamento. Altezza massima misurata: 35,8 m (cima delle antenne).

**Geometria misurata sulla foto.** Rispetto al modello di prova sono cambiati: albero principale
(da circa 50 a circa 64 m dalla prua), antenna SPS-49 (a proravia dell'albero, non a poppavia),
radome Mk92, lanciatore Mk13, cannone da 76 mm (a circa 83 m dalla prua), fumaiolo, hangar;
inoltre il dritto di prua slanciato (9,2 m al galleggiamento) e il bordo libero (8,9 m a prua,
5,8 m al centro).

**Controllo automatico della sagoma**, dalla proiezione laterale del modello confrontata con la
maschera della foto nelle stesse coordinate:

| Modello | Coincidenza (IoU) | Pixel solo modello | Pixel solo foto |
|---|---|---|---|
| Prova (posizioni a occhio) | 0,714 | 10.067 | 6.630 |
| Misurato | **0,877** | 4.862 | 1.673 |

Le differenze residue sono piccoli dettagli: ringhiere, antenne a frusta, schiuma, traliccio
riempito nella maschera.

**Proiezione della foto.** Proiezione laterale lungo l'asse trasversale, con livelli estesi sui pixel
della nave. Peso della foto |n_y|^1,5 (massimo sulle superfici rivolte di fianco); il resto usa
l'ombreggiatura del modello, con albedo tarato sul tono medio della foto. Dove la foto mostra cielo,
le superfici dei tralicci diventano trasparenti, quindi gli alberi mostrano i vuoti tra le aste.
La foto è del lato sinistro: il lato dritto usa una copia con il numero di scafo cancellato, che
altrimenti apparirebbe rovesciato (nel gioco si potrà ridipingere il numero di ciascuna nave).

**Correzione di convenzione.** Il sistema del modello (x prua, y dritta, z alto) è sinistrorso: la
prima versione del renderer produceva immagini speculari. Corretto in entrambi i renderer: dal lato
dritto la prua appare a destra.

**Esito.** Dettaglio fotografico vero (portelli, ringhiere, imbarcazioni, numero di scafo), tralicci
trasparenti, scala esatta e aspetto continuo. Molto più vicino allo spirito degli sprite 1989 rispetto
alla sola geometria. Limite: tra 0° e circa 10° e tra circa 170° e 180° la foto di fianco non dà
informazione e resta l'ombreggiatura.

**Per la produzione:**
1. proiezione multipla: foto di fianco sulle facce laterali, foto di prua e di poppa (anche
   leggermente elevate) sulle facce frontali, con fusione pesata sull'orientamento;
2. kit di parti riutilizzabili (generatore di scafi, alberi a traliccio, radar, lanciatori, cannoni,
   hangar) per ridurre la modellazione per classe;
3. sprite ogni 5° per lato (72 viste per classe) preparati in anticipo; giorno, notte e foschia
   tramite palette;
4. controllo della sagoma automatico con soglia minima (per esempio IoU ≥ 0,85) per ogni classe.

Stima: il kit richiede qualche giorno di lavoro; poi circa 1–2 ore per classe, soprattutto per
scegliere, calibrare e mascherare le foto.
