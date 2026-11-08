# 24 — Il quadro del fondale: carta + misure, ecoscandaglio e vista dei ritorni (piano)

*Disassemblatore, 28/10/2026. Stato: piano, da approvare. Roadmap 26; specifica in `docs/08_fondali_e_navigazione.md`
§3-§4 e §7; parametri e fonti nel dossier `docs/ricerca/06_fondali_e_navigazione.md` §1 e §7.1-7.2.*

## 1. Che cosa c'è oggi

- **Fondale vero** (`nucleo/fondale`, celle da 200 m): incagli e collisioni si calcolano su questo.
- **Carte d'epoca** NATO e sovietica (`<teatro>_carta_<parte>.fdl`, `tools/pipeline/carta_epoca.py`), con campagne,
  zone non rilevate e annotazioni. La carta della principale le mostra già (opzione «CARTA: D'EPOCA»).
- **Posizione stimata** (`nucleo/navigazione`), agganciata dal runtime, con l'ellisse sulla carta e i punti nave.
- **BQS-15** (`Missione::spazzata_bqs15`): vede solo le mine, con la contro-scoperta.

**Manca:**
- l'ecoscandaglio come sensore, cioè un ping che misura il fondo sotto la chiglia e costa in contro-scoperta;
- i ritorni del fondo e delle pareti del BQS-15, e i sondatori;
- il quadro misurato, che fonde le misure cella per cella;
- gli avvisi dell'equipaggio nati solo dalle misure;
- le viste.

## 2. Modello (mio): tre moduli puri nel nucleo

### 2.1 `nucleo/ecoscandaglio`: il ping verso il basso

`sonda(fondale vero, posizione vera, quota, parametri, caso)` restituisce un `Sondaggio`:
- t;
- posizione stimata e quota propria;
- distanza dalla chiglia al fondo e profondità del fondo, con la sigma;
- se è un falso fondo.

Il modello (dossier 06 §1.2):
- **Impronta:** un cono di 30°, quindi un diametro di 0,54 volte la distanza dal fondo. Si prende il punto più vicino
  nell'impronta: i fianchi dei rilievi si arrotondano e i picchi si anticipano.
- **Errore:** ±1% di scala per la velocità del suono, più un rumore.
- **Falsi fondi, 2%:** lo strato diffondente di giorno a 300-500 m, le bolle e la scia sopra i 20 kn.
- **Potenza:** piena, fino a 6000 m e intercettabile a 15-20 km; bassa, −20 dB, fino a 2000 m e intercettabile a 5-8 km.
- **Modalità:** a comando, oppure periodica ogni N minuti, con N da 1 a 30.
- **Contro-scoperta:** ogni ping è un'`EmissioneAttiva` nuova del catalogo dell'intercettazione (12 kHz, 210 o
  190 dB), così i ricevitori del blocco 2 lo sentono già.

### 2.2 `nucleo/ritorni_fondo`: il BQS-15 sul fondo e i sondatori

`spazza_fondo(fondale vero, posa vera, posa stimata, velocità, parametri, caso)` dà i ritorni del settore di prua:
±60° in azimut, da −30° a +45° in elevazione, settori di 15°.
- **Ritorni:** distanza, azimut ed elevazione del primo eco del terreno lungo ogni raggio, entro la portata: pareti
  3 km, fondo inclinato meno. La risoluzione è di circa 1° per 5 m.
- **Degrado:** −30% della portata a 15 kn, −60% oltre i 20; vicino al fondo la riverberazione.
- **Raggi senza eco:** «niente fino a X m».
- **Sondatori su e giù:** fascio di 10-15°, 1000 m, uno al secondo. Danno la distanza dal fondo e dalla volta, che
  serve per il ghiaccio del punto 4.

Le mine restano a `spazzata_bqs15` com'è; la stessa spazzata chiama anche questa.

**Sovietici** (MG-70, MT-70, MG-24): stessi modelli con le portate ridotte (STIMA, docs/08 §3).

### 2.3 `nucleo/quadro_fondale`: carta + misure

Una griglia per battello, sulla griglia della carta della sua parte (200 m). Per cella:
- dalla carta: `z_carta` e `σ_carta`, dalla spaziatura della campagna e dalle zone non rilevate;
- dalle misure: `z_mis`, `σ_mis`, istante e fonte (ecoscandaglio, BQS-15, sondatore);
- l'ostacolo, se c'è.

**Aggiornamento** (dossier 06 §7.2):
- **Sondaggio:** il centro dell'impronta riceve la misura col filtro di Kalman scalare. Le altre celle dell'impronta
  ricevono il vincolo «fondo almeno a questa profondità». Lungo la rotta si forma un nastro.
- **Ritorni del BQS-15:** sono punti sul terreno; i raggi senza eco liberano le celle con una confidenza minore.
- **Decadimento:** una misura vecchia con la posizione incerta torna lentamente verso la carta.

**Il mosaico che scorre:**
- Ogni misura si archivia con la posa stimata e la sua sigma.
- A un punto nave la correzione si distribuisce in modo lineare sulle misure prese dall'ultimo punto nave, e le celle
  toccate si ricalcolano: è una SLAM semplificata.
- Il quadro tiene le misure grezze (alcune migliaia per ora di sondaggi periodici, poche decine di kB) e rifà solo le
  celle cambiate.

**Letture:**
- `cella(col, riga)`: valore fuso, sigma, fonte, età, e se è «da carta».
- `profilo(da, rotta, lunghezza)`: la sezione verticale di prua, col profilo della carta e quello misurato.
- `ritorni_recenti()`: per la vista dei soli ritorni.
- `avvisi(posa stimata, quota, rotta, velocità)`:
  - «fondo in salita» solo se due sondaggi recenti lo mostrano;
  - «ostacolo a prua» solo da un ritorno del BQS-15 entro il tempo di reazione;
  - «sotto la chiglia X m» dall'ultimo sondaggio;
  - mai dal terreno vero.
- Salvataggio e ricarica.

**La velocità sicura**, portata del BQS-15 divisa per il tempo di reazione (docs/08 §3), è una lettura per
l'avviso, non un vincolo.

### 2.4 Dati

`[ecoscandaglio.*]` e `[bqs15_fondo.*]` in `data/motore/acustica.toml`, generati da `tools/pipeline/acustica.py`:
- per sensore: portate, livelli, settori, falsi fondi, degrado con la velocità, fonte e confidenza (quasi tutto STIMA);
- per classe: quali sensori ha;
- le due emissioni nuove per l'intercettazione.

## 3. Runtime (cadenza-della-scoperta-c): l'aggancio

- Ordini: ping singolo, modo periodico con N minuti, potenza piena o bassa. Il BQS-15 c'è già.
- Un `QuadroFondale` per il giocatore, alimentato dai sondaggi e dalle spazzate, sulla posizione stimata; i punti
  nave chiamano la correzione del mosaico.
- Le emissioni nell'intercettazione e i messaggi dell'equipaggio dagli `avvisi()`.
- L'IA: per simmetria potrebbe sondare con la stessa logica, per esempio prima di scendere vicino al fondo. La lascerei
  a dopo: oggi il suo pilota usa il fondale vero.

## 4. Viste (principale)

- **Carta + misure:** sulla carta d'epoca le celle misurate, col colore della profondità e la trasparenza della
  confidenza; quelle «da carta» tratteggiate; il nastro dei sondaggi lungo la rotta; gli ostacoli.
- **Ecoscandaglio:** la striscia del fathometer, la profondità sotto la chiglia nel tempo come su carta termica, coi
  falsi fondi; i tasti per il ping, il periodo e la potenza.
- **Vista dei soli ritorni:** la prospettiva erede del Contour Imaging Display. Rende solo la griglia fusa e i
  ritorni del BQS-15, con «da carta» dove non si è misurato; più la sezione di prua (carta, ritorni, quota, margine).

## 5. Prove (mie)

- **Ecoscandaglio:**
  - sul fondo piatto misura entro l'1%;
  - su un pinnacolo l'impronta lo anticipa;
  - falsi fondi al 2%;
  - le portate delle due potenze;
  - ripetibile dal seme.
- **BQS-15 sul fondo:**
  - una parete a 2,5 km si vede a 10 kn e non a 25;
  - i raggi liberi;
  - la riverberazione vicino al fondo.
- **Quadro:**
  - il nastro lungo la rotta e la sigma che cala;
  - il vincolo di minimo;
  - il punto nave che sposta il mosaico della correzione giusta;
  - il decadimento;
  - salvataggio e ricarica uguali.
- **Avvisi:**
  - mai dal terreno vero;
  - lo scenario San Francisco: a 33 kn con un sondaggio ogni 15 minuti il monte non si vede, con uno al minuto sì.
- **Classico:** rilievo 0/110, perché è tutto solo nell'Enhanced.

## 6. Ordine, costi, file

**Ordine:** 2.1 l'ecoscandaglio, poi 2.3 il quadro, poi 2.2 il BQS-15 sul fondo. L'aggancio del runtime e le viste
della principale possono partire dopo 2.1, sull'header del quadro che mando prima del codice.

**Costi:**
- ecoscandaglio: S;
- quadro, col mosaico che scorre: M;
- BQS-15 sul fondo: S-M;
- aggancio: S;
- viste: M.

**File:**
- **miei, nuovi:** `nucleo/ecoscandaglio`, `nucleo/ritorni_fondo`, `nucleo/quadro_fondale`, le prove;
- **miei, già esistenti:** `tools/pipeline/acustica.py`;
- **condivisi, da annunciare:** CMake, README;
- **del runtime:** `Missione` (ordini, aggancio, messaggi);
- **della principale:** `app/` (le viste);
- **del kit:** il manuale.

**Dopo:** il punto 4 del giro, le operazioni sotto il ghiaccio (docs/08 §197), usa i sondatori di 2.2 e il quadro.
Lo valuto quando questo è fatto.
