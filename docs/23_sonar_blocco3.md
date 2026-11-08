# 23 — Sonar, blocco 3: DEMON a cascata, righe automatiche, storia lunga, CPA nel MATE, biologici registrati (piano)

*Disassemblatore, 28/10/2026. Stato: approvato dall'utente il 28/10/2026 così com'è, ordine L, G, H, I con M in
parallelo. Per M solo l'elenco con le licenze: ogni scaricamento lo chiede la principale all'utente, voce per voce. Le
viste di G, H e I le fa trailer-1 su file nuovi suoi; quella di L, il MATE, la principale.*

Sono i punti rimasti della nota `sonar-confronto-27-10.md`, dopo i blocchi 1 (`docs/20`) e 2 (`docs/22`):
- **G**, punto 5: il DEMON a cascata;
- **H**, punto 6: le righe trovate in automatico nel modo moderno;
- **I**, punto 7: la storia lunga delle cascate;
- **L**, punto 11: il CPA dalla curva a S nel MATE;
- **M**, punto 9: i biologici e il ghiaccio senza registrazione.

Come sempre il classico non cambia: rilievo 0/110. La scoperta (`valuta`) non cambia: è tutto lavoro del sonarista e
della stazione.

## 1. Che cosa c'è oggi

- **DEMON:** `demon(sorgente, ricevitore, ambiente)` dà l'inviluppo della cavitazione di una sorgente sola (asse, pala,
  armoniche), e `leggi_demon` ne ricava pale e giri. La stazione lo mostra come lettura, non come cascata.
- **Righe:** il LOFAR ha cursori, tracker S-numerati, verniero, libreria e maschera delle righe proprie. Le righe le
  trova l'operatore; nel modo moderno non c'è l'aiuto automatico (ALI/ALT dell'A-RCI).
- **Storie:** la stazione tiene 150 righe per scala dei tempi (x1, x4, x16) di BTR e LOFAR; il registratore (S5d)
  registra, ma non si sfoglia all'indietro per ore.
- **CPA:** `misura_cpa` ricava dalla curva a S di una riga f0, velocità relativa, istante e distanza del CPA, con le
  sigma; `confronta_cpa` confronta il CPA misurato con quello di una soluzione del MATE. Manca il passaggio
  inverso: proporre al MATE una soluzione dal CPA. In più la vista non mostra la coerenza.
- **Biologici e ghiaccio:** capodoglio, foca barbata e fessurazione del ghiaccio sono solo modello, senza
  registrazione. Tricheco, balenottera minore e orca vanno verificati (dossier 09 §8).

## 2. G — Il DEMON a cascata (punto 5)

**Modello (mio):** `demon_fascio(sorgenti, ricevitore, ambiente, rilevamento, f_max, bin)`.
- Dà il DEMON del fascio puntato, come `spettro_fascio`: l'inviluppo 2-8 kHz di tutte le sorgenti che cavitano, pesate
  con la risposta del fascio in quella banda.
- Ci sono anche le navi civili generate, sopra l'innesco, e il proprio battello se cavita.
- Due sorgenti nello stesso fascio danno due serie di righe.

La storia la tiene la vista, come per il LOFAR. Si vede quando il bersaglio cambia giri: le righe d'asse e di pala
salgono o scendono, indizio di manovra.

**Vista (principale):** la cascata DEMON al posto del LOFAR, o accanto, coi cursori armonici d'asse e di pala.

**Costo:** S, pochi giorni miei; S per la vista.

## 3. H — Le righe trovate in automatico, ALI/ALT (punto 6)

**Modello (mio): modulo puro `nucleo/rilevatore_righe.hpp`.**
- Riceve le righe successive del LOFAR di un fascio o di un tracker, cioè gli `Spettro` con la grana della stazione.
- Integra per qualche riga (ALI): un picco va sopra soglia in M righe su N.
- Segue ogni riga nel tempo (ALT): frequenza, deriva in Hz/min, S/N medio, età.
- Dà le righe marcate, con un identificativo stabile per tracker.

**Errori plausibili, tarati sul dossier 09 §3.9 (STIMA):**
- righe false dalla grana, a un tasso per ora;
- righe perse nelle frange d'interferenza (blocco 1) e quando il segnale cala;
- due righe vicine che si fondono o si scambiano;
- la riga propria marcata se la maschera è spenta.

Solo nel modo moderno; il 1989 resta a mano.

**Vista (principale):** le marche sul LOFAR, la lista delle righe con età e deriva, l'invio di una riga alla libreria
o al verniero.
- Le etichette delle marche (`app/righe_automatiche.cpp`, trailer-1, 29/10/2026): quando si toccano si impilano su
  quattro righe al massimo, sulla prima riga libera da sinistra, con la linea a puntini dal triangolino, come i
  segnalibri delle SDR (docs/ricerca/30, n. 8). Prima si disegnano le linee, poi le etichette su un fondo nero, così
  una linea non taglia il testo. Quelle che non trovano posto diventano un «+n» verde sull'ultima etichetta della riga
  più bassa, e il loro triangolino resta cliccabile.

**IA:** il sonarista dell'IA potrebbe usarlo nei modi moderni, ma il suo modello di classificazione funziona già senza.
Lo lascerei fuori, salvo che il runtime lo voglia.

**Costo:** M, una settimana mia; S per la vista.

## 4. I — La storia lunga delle cascate (punto 7)

**Contenitore (mio, puro): `nucleo/storia_cascata.hpp`.**
- Le righe di BTR, LOFAR e DEMON per ore, a più risoluzioni: 1 riga al secondo per 10 minuti, ogni 10 s per 2 ore,
  ogni 60 s per 12 ore.
- Si scende di risoluzione col massimo, come la decimazione di oggi.
- Il LOFAR si quantizza a 0,25 dB in 8 bit: circa 7 MB per 12 ore e 4000 bin.
- Si legge una finestra a qualsiasi istante e risoluzione.
- Si cerca quando è comparsa una riga, o una traccia a un rilevamento.
- Si salva e si ricarica col salvataggio della partita (formato del runtime e della principale, da concordare).

**Vista (principale):** lo scorrimento all'indietro delle cascate, il cursore del tempo, la ricerca «quando è
comparso».

Il registratore esistente resta per l'ascolto.

**Costo:** S-M per me, M per la vista. Se la principale preferisce tenere anche il contenitore nella stazione, va bene:
lo dico solo perché è puro e si prova da solo.

## 5. L — Il CPA dalla curva a S nel MATE (punto 11)

**Modello (mio, in `nucleo/misure_sonar`): `soluzione_da_cpa(tma, misura_cpa)`.**
- Al CPA il rilevamento è perpendicolare alla velocità relativa. Quindi il bersaglio sta lungo il rilevamento di
  quell'istante, alla distanza del CPA, e la velocità relativa è perpendicolare con il verso del rateo di rilevamento.
- Il moto proprio si ricava dalle posizioni registrate nella TMA.
- Ne esce una `SoluzioneTma` completa (distanza, rotta, velocità), con le sigma propagate.
- Funziona solo se il CPA è stato osservato (`cpa_osservato`).

Insieme a `confronta_cpa`, che c'è già, chiude la catena sonar → TMA.

**Vista (principale):** nel MATE il tasto «proposta dal CPA» e l'indicatore di coerenza (i tre scarti entro due
sigma).

**Costo:** S per me, S per la vista. È il pezzo più piccolo e chiude un punto aperto da S5d: lo farei per primo.

## 6. M — Biologici e ghiaccio registrati (punto 9)

**Ricerca (mia):** registrazioni con licenza CC0, PD o CC BY di:
- capodoglio (click e coda);
- foca barbata (trilli);
- fessurazione e pressione del ghiaccio;
- da verificare: tricheco, balenottera minore (*boing* e *bioduck*), orca.

Fonti da guardare:
- NOAA (PMEL, NEFSC: pubblico dominio);
- Wikimedia Commons;
- Freesound coi filtri di licenza;
- le altre (Watkins, Macaulay, DOSITS) solo se la licenza è giusta; spesso non lo è.

Per ognuna: licenza, autore, durata, qualità, frequenza di campionamento. Ne esce un elenco con le scelte. **Niente
download senza il via della principale**, voce per voce. Poi `tools/pipeline/campioni` (R5) le collega alle voci della
libreria, come le 33 di oggi, coi crediti (`CREDITI.md`, `crediti.toml`).

**Costo:** S per la ricerca; S per il collegamento dopo il via.

**Esito (28/10/2026):** ricerca in `docs/ricerca/29_note/registrazioni_biologiche.md`; l'utente ha dato il via a nove
voci. Sette sono nella libreria (vedi il README, «Libreria di campioni»), con le due sorgenti nuove `tricheco` e
`balenottera_minore_pacifica`. Escluse: la coda del capodoglio, perché è la stessa registrazione dei rapid clicks e
non ha una coda riconoscibile; il bioduck, perché non c'è un teatro antartico. Restano al modello le campane del
tricheco e la coda. Si può fare in parallelo a tutto il resto.

## 7. Ordine, prove, file

**Ordine consigliato:**
1. **L, il CPA nel MATE:** piccolo, chiude S5d.
2. **G, il DEMON a cascata:** piccolo, si vede subito.
3. **H, ALI/ALT:** il pezzo più grosso.
4. **I, la storia lunga:** dopo H, perché le righe automatiche si cercano anche nella storia.
5. **M:** in parallelo, come ricerca, coi download a tempo dei via.

**Prove (mie):**
- **L:** su una scena sintetica (bersaglio rettilineo, CPA osservato) la soluzione dal CPA sta entro le sigma dalla
  vera; senza CPA osservato niente; coerente con `confronta_cpa`.
- **G:**
  - il DEMON del fascio ha le righe del bersaglio che cava;
  - è piatto senza cavitazione;
  - due bersagli in un fascio danno due serie;
  - le righe si spostano quando il bersaglio cambia giri.
- **H:**
  - righe vere trovate entro N righe;
  - falsi al tasso previsto sulla grana;
  - la deriva misurata contro quella vera (Doppler);
  - la fusione di due righe vicine;
  - ripetibile.
- **I:**
  - la decimazione col massimo conserva i picchi;
  - una finestra a qualsiasi istante;
  - la ricerca della comparsa di una riga;
  - la memoria entro il previsto;
  - salvataggio e ricarica uguali.
- **M:** la libreria e la cuffia usano le registrazioni nuove, coi crediti; `manuale --controlla` a 0.

**File miei:**
- nuovi: `nucleo/rilevatore_righe`, `nucleo/storia_cascata`;
- `nucleo/analisi_acustica` (`demon_fascio`), `nucleo/misure_sonar` (`soluzione_da_cpa`);
- la pipeline dei campioni;
- le prove.

**Condivisi, da annunciare:** CMake, README.

**Della principale:** le viste (`app/sonar_enhanced`, il MATE).

**Del kit:** il manuale (C5, C13, C8).

**Stima complessiva:** 2-3 settimane mie, più le viste.
