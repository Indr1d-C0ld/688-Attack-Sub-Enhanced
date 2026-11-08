# D3. Missili e difese

*Autore: kit, per il runtime. Revisione del 05/11/2026 (le difese delle navi contro i missili: allerta, saturazione della
salva, Phalanx, §2.6). Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Decide se un missile lanciato dal battello colpisce una nave, e quanto le fa male. Il 688 porta l'Harpoon antinave e
il Tomahawk, antinave o d'attacco a terra; la Sea Lance antisom è un what-if. Per lanciare servono un bersaglio di
superficie con una traccia solida, abbastanza lontano, e il battello a quota periscopica. Il lancio si fa sentire:
per 100 s chiunque ascolti sa dove si trova il battello. Nel classico il volo non è simulato: l'esito è quello del
1989, un colpo che riesce con una probabilità che dipende dalla nave. Nell'Enhanced l'Harpoon e il Tomahawk antinave
del giocatore volano col piano di tiro e il seeker (§2.9), il TLAM-C ha la sua libreria di missioni (§2.10), e il
colpo fa i danni veri di una testata. All'impatto la nave si difende secondo l'allerta, il numero di missili della salva e
i suoi Phalanx (§2.6); i missili antiaerei e gli inganni non sono oggetti del gioco, li riassume la probabilità della classe.

## 2. Il modello

Il missile non è un oggetto della missione. Il lancio lo controlla la stazione Armi (`nucleo/src/armi_giocatore.cpp`,
`StazioneArmi::lancia`); il volo e l'esito li fa lo script della missione, quello dell'originale (il gestore
dell'arma: `docs/04_vm_missioni.md`); l'Enhanced aggiunge il carico (capitolo D5), la fisica del colpo (capitoli E1 ed
E2), l'effetto del lancio e il registro.

### 2.1 Le armi

Dal catalogo del carico (`data/motore/carico.toml [armi.*]`; la tabella completa è nel capitolo D5):

| Arma | Famiglia | Velocità | Portata | Testata | Dove | In servizio |
|---|---|---|---|---|---|---|
| UGM-84 Harpoon | `harpoon` | 560 kn | 140 km | 221 kg | tubo, in capsula | dal 1981 |
| UGM-109B Tomahawk TASM | `tomahawk` | 475 kn | 460 km | 450 kg (454 nel danno, §2.3) | tubo o pozzo | 1983-1994 |
| UGM-109C Tomahawk TLAM-C | `attacco_terra` | 475 kn | 1250 km | 450 kg | tubo o pozzo | dal 1986 |
| UUM-125 Sea Lance | `sea_lance` | 860 kn | 185 km | 45 kg | tubo | mai (what-if) |

Velocità e portata del catalogo servono alla sala siluri e all'IA per scegliere l'arma; il volo non le usa (§2.3). La
testata del catalogo non la legge nessuno: la carica del colpo la dà `testata_missile` (§2.3).

### 2.2 Il lancio

Il tubo deve essere armato. Poi i controlli, nell'ordine dell'originale (`StazioneArmi::lancia`;
`docs/originale/sensori_e_navigazione.md` §10.2):

| Controllo | Vale per | Rifiuto |
|---|---|---|
| il TLAM-C vuole solo la quota periscopica | TLAM-C | 231 |
| c'è un bersaglio selezionato, e c'è ancora | missili | 227 |
| il bersaglio è una nave | solo Harpoon | 228 |
| il giocatore ha il contatto sul bersaglio | missili | nessun messaggio |
| la traccia è solida (qualità almeno 800) | missili | 229 |
| il bersaglio è ad almeno 5,0 miglia (9,26 km; `DISTANZA_MIN_DECIMI` = 50 decimi) | missili | 230 |
| il battello è a quota periscopica, al più 20 piedi (6,096 m; `QUOTA_PERISCOPICA`) | missili | 231 |
| non ci sono già troppe armi in acqua | solo Sea Lance | 232 |

- I missili, tranne la Sea Lance, non contano fra le armi in acqua (4 sul 688).
- Nell'originale i missili stanno nel tubo 7. Col carico dell'Enhanced un missile può stare in un tubo da siluri, e
  segue le stesse regole del tubo 7; sul Flight II la posizione 7 sono i pozzi verticali (capitolo D5).
- Superati i controlli, la stazione designa il bersaglio e chiama il gestore di lancio della famiglia del tubo.
- **Nell'Enhanced, i missili del catalogo** (`data/motore/missili.toml`, caricato con i siluri dell'Enhanced) hanno
  la loro preparazione per tubo (`Missione::tiro_missile`: PWR, ALN, RDY). Un missile del catalogo pronto, cioè RDY
  (piano di tiro valido e allineato) con la slide valve del suo lato libera, salta i controlli della tabella:
  traccia solida, distanza e quota li sostituisce il piano di tiro. Se non è pronto, il rifiuto è 232 «Non
  ancora» (`SituazioneArmi::missile_tubo`, `StazioneArmi::lancia`). I tubi senza un missile del catalogo seguono la
  tabella. La preparazione, il piano e il volo sono nel §2.9.

### 2.3 Il volo e l'esito

L'Harpoon e il TASM hanno lo stesso gestore. Nelle missioni dell'Enhanced della libreria il Tomahawk antinave usa
quello dell'Harpoon (`tomahawk = { gestore = ... }` nelle missioni convertite). Il gestore fa, nell'ordine:
1. l'istruzione MISSILE (`f_lancio_missile`): un evento acustico di 100 s sul battello (§2.5), il lancio nella lista
   dei lanci (`Missione::registra_lancio_missile`) e l'evento EMCON «lancio di missili» per chi lancia;
2. il messaggio 49 «Missile lanciato»;
3. dopo 20 s (10 s in alcune scene dell'originale) l'istruzione ATTACK_RND sul bersaglio, con il danno 75 (100 in
   alcune scene).

**ATTACK_RND** (`f_attacco_casuale`):

```
riesce se  b ≥ difesa,   b = byte casuale 0-255
P(colpo) = (256 − difesa) / 256
```

`difesa` è il byte `+3D` del modello della nave bersaglio, cioè la sua difesa contro gli attacchi a esito casuale.
Riuscito: il messaggio 50 «Un missile ha raggiunto il bersaglio» e il danno sulla nave. Fallito: niente.

Le probabilità che ne vengono per le classi dell'originale (`docs/originale/prestazioni.md`):

| Bersaglio | Difesa | P(colpo) |
|---|---|---|
| Kara, Kirov, Slava, Krivak II | 90 | 65% |
| Oscar | 128 | 50% |
| Oliver Hazard Perry in SCENE9A e SCENE9B, e nei file di libreria e di prova | 128 | 50% |
| Nimitz, Kidd, Oliver Hazard Perry in SCENE8 e SCENE9C | 15-16 | 94% |
| Iowa | 17 | 93% |
| Koni, Sacramento | 5 | 98% |
| Forrest Sherman, mercantili | 0 | 100% |

Il byte viene dal modello della scena, non dalla classe: la stessa classe può averne due. Nell'Enhanced, per i missili del
modello, queste cifre valgono solo per una nave ai posti di combattimento, con un canale libero e senza Phalanx (§2.6).

Nel gestore il volo non c'è: niente quota, rotta, accensione della testa, scelta del bersaglio in volo, né la
distanza. Un missile lanciato a 10 km e uno a 100 km arrivano dopo gli stessi 20 s. Vale nel classico, per l'IA e per
il TLAM-C. Nell'Enhanced l'Harpoon e il TASM del giocatore volano col modello del §2.9: il gestore gira ancora, ma il
suo ATTACK_RND non fa niente (`Missione::attacco_da_missile`), perché l'esito viene dall'impatto.

**Il danno.** Con la regola di simulazione il colpo passa alla fisica dei danni (`missioni/src/danni_missione.cpp`):
- su una nave con lo scafo del kit: un'esplosione di tipo missile 2,5 m sopra il galleggiamento, su un fianco a caso,
  in un punto a caso entro il 25% della lunghezza dal centro;
- la carica la decide il missile lanciato (`testata_missile`): 454 kg per il Tomahawk antinave (famiglia
  `tomahawk`), 221 kg per l'Harpoon. Il missile è la famiglia dell'ultimo lancio di quel lanciatore
  (`Missione::applica_attacco`, dalla lista dei lanci). Per il giocatore è la famiglia del tubo che parte
  (`Missione::registra_lancio_missile`, durante il lancio dal tubo): anche se Harpoon e Tomahawk antinave hanno lo
  stesso gestore, il missile resta quello del tubo. Per gli altri è la famiglia il cui gestore nel blocco delle armi
  della missione è quello che ha eseguito MISSILE;
- se il missile non si sa (di norma i lanci dell'IA, che girano nei gestori dei loro comportamenti), decide la classe
  di chi lancia:
  221 kg per 688, Sturgeon e Trafalgar (l'Harpoon); 750 kg per Oscar e Kirov (P-700 Granit); 1000 kg per lo Slava
  (P-500 Bazalt); 500 kg per il Charlie II (P-120 Malakhit) e per gli altri;
- su un sottomarino, solo se è in superficie (quota fino a 5 m): 220 kg a 2 m, per ogni missile
  (`DanniSottomarini::colpisci`).

Incendi, falle e affondamento sono nei capitoli E1 ed E2.

### 2.4 TLAM-C e Sea Lance

- **TLAM-C.** Non ha un bersaglio in mare. Il lancio è l'evento `lancio_giocatore` del registro, con l'arma `tlam_c`
  (`lancio_senza_arma` del registro). Non usa mai il gestore del `tomahawk`, che nelle missioni è il TASM antinave.
  Una missione che vuole contare gli attacchi a terra dichiara `attacco_terra = { gestore = "..." }` in `armi`
  (`docs/09` §5, `data/missioni/API.md`): il gestore parte a ogni lancio del giocatore, e un suo
  `m688.lancio_missile(u)` non conta un secondo lancio. Senza, il lancio resta solo nel registro. Nessuna missione
  del gioco lo dichiara oggi (28/10/2026).
- **TLAM-C nell'Enhanced** (col catalogo dei missili): prima del gestore parte il modello del §2.10. Senza una missione
  scelta dalla libreria di bordo il lancio è rifiutato.
- **Sea Lance.** Nell'Enhanced è un what-if, solo con l'opzione del giocatore (capitolo D5). La lancia il gestore
  `sea_lance` della missione. Nelle scene dell'originale quel gestore crea per lo più un siluro (Mk 48 in undici
  scene, Spearfish, Mk 50 o «Sealance Torpedo» nelle altre: `docs/originale/prestazioni.md`). Se la classe è nella
  tabella dei siluri, quel siluro corre con il modello dell'Enhanced (capitolo D1); altrimenti resta classico. Nel
  classico non è un'arma filoguidata.

### 2.5 La contro-scoperta

- **L'evento acustico.** MISSILE apre un evento di 900 passi (100 s) sul battello. Nell'originale un evento rivela
  l'unità a tutti entro 20 miglia, in ogni direzione (`docs/originale/sensori_e_navigazione.md` §4).
- **Il messaggio.** Quando lancia un'altra unità, il giocatore riceve il 48 «Rilevato un lancio di missile».
- **L'effetto.** L'app disegna ogni lancio della lista (`app/partita.cpp`, `Effetti::lancio_missile`,
  `docs/10_danni_affondamento_esplosioni.md` §10.15). Nella lista ci sono i lanci dell'istruzione MISSILE e ogni
  missile del modello del §2.9 (`missione_missili.cpp`): giocatore e IA, da sottomarino e da nave, inerti compresi. Il
  TLAM vi entra con la rotta verso il landfall. Se lo script e il modello lanciano nello stesso istante dalla stessa
  unità, la voce è una sola, con l'arma e la rotta del modello.
  - **da un sottomarino:** capsula o booster che escono dall'acqua, fiammata e fumo;
  - **da una nave** (`da_nave`): niente risalita, capsula o spruzzo; la vampa all'accensione, una nube di fumo che
    avvolge il ponte e la stessa traiettoria del profilo, partendo da 6 m sul mare;
  - **dagli aerei:** ancora nessun effetto.

  Il profilo segue l'inizio dell'id dell'arma (gli id del modello hanno la variante, come `harpoon_84c` o `tlam_c`):
  `harpoon` come l'Harpoon; `tasm`, `tlam` e `tomahawk` come il Tomahawk; `p500`, `p700`, `p1000` e `granit` come il
  Granit.
  Senza un'arma nota decide la classe: Granit per l'Oscar, Malakhit per il Charlie II, Ametist per il Charlie,
  Harpoon per i sottomarini americani, Vodopad per gli altri sottomarini, Malakhit per le navi.
- **I transitori.** Il nucleo ha le sequenze di lancio dei missili per la scena acustica
  (`nucleo/src/transitori_armi.cpp`, `transitori_lancio`): porta del tubo e colpo d'aria, poi la capsula dell'Harpoon
  che emerge dopo 20-60 s, o il booster del Tomahawk; dai pozzi il portello e il booster. La missione le emette per le
  armi che diventano unità (`Missione::passo_transitori_armi`). Il missile del gestore non lo è: per lui oggi vale
  solo l'evento acustico dell'originale. **In arrivo** col volo (roadmap, punto 25).

### 2.6 Le difese delle navi

**Nel classico, negli script e nei lanci senza modello** l'unica difesa è il byte `difesa` del §2.3. Non dipende
dall'allerta della nave né dalla salva: due missili contro un Kirov sono due prove indipendenti, ciascuna al 65%.

**Nell'Enhanced, per ogni missile del modello (§2.9) che arriva su una nave,** le regole sono tre
(`nucleo/difese_navi.hpp`, `ImpegnoDifese::arrivo`). Il giocatore e l'IA passano dallo stesso codice, che non guarda chi ha
lanciato. Missile per missile, nell'ordine d'arrivo:

1. **Il canale.** Se la nave ha un canale libero, il missile lo impegna per 30 s e il byte, scalato con l'allerta, può
   fermarlo. Con tutti i canali impegnati il byte non si tira: è la saturazione.
2. **Il Phalanx.** Se il missile è passato e c'è un affusto libero, l'affusto lo ingaggia una volta e può fermarlo.
3. **Il colpo.** Se è passato anche da lì, il missile colpisce (§2.9).

**L'allerta.** Il byte vale per una nave ai posti di combattimento; sotto, la nave ne usa solo una parte. Il fattore scala il
byte (arrotondato per difetto) e la probabilità del Phalanx:

| Allerta (capitolo E6) | Fattore |
|---|---|
| A0 | 0,5 |
| A1 | 0,75 |
| A2, A3, o forze spente | 1,0 |

- **Quale allerta conta:** quella che la nave poteva avere, cioè il massimo fra il livello della sua forza quando il missile
  è partito e il livello dell'unità all'arrivo. Il lancio è una provocazione e porta subito la forza bersaglio in guerra e
  ad A2 (`passo_forze`), ma la nave non può saperlo. Perciò ogni missile si porta dietro la fotografia dei livelli di tutte
  le forze al lancio (`MissileMissione::forze_al_lancio`): il bersaglio lo sceglie il seeker in volo, al lancio non è noto.
- **Il primo missile sveglia la nave:** un colpo o un missile fermato le danno l'evento `arma`, e il missile dopo trova A3.
  La sorpresa vale una volta sola. Un missile d'esercitazione fermato non dà l'evento.

**I canali e gli affusti.** Un canale si impegna per ogni missile che arriva, anche se poi il byte non lo ferma. Quattro tubi
con la slide (0, 4, 20, 24 s, §2.9) stanno dentro i 30 s, come una salva dell'IA a 4 s; due salve separate da una ricarica
no. Un affusto resta impegnato per il tempo in cui il missile attraversa la portata, 1490 m / velocità: 6,2 s a 240 m/s,
3,3 s a 450 m/s. Un solo affusto contro Harpoon a 4 s ne ingaggia uno sì e uno no; contro P-500 a 4 s li ingaggia tutti.

| Phalanx | Valore |
|---|---|
| probabilità di fermare, Block 0 | 0,35 |
| probabilità di fermare, Block 1 (solo l'Iowa, dal 1988) | 0,45 |
| contro un supersonico, oltre 340 m/s | × 0,5: P-700 e P-500 sì; P-120 (300 m/s), Harpoon e TASM no |

**Le classi** (`data/motore/missili.toml`, `[[difesa_nave]]`, da `tools/pipeline/missili.py`, con fonte e confidenza per riga):

| Classe | Phalanx | Canali | Base dei canali |
|---|---|---|---|
| Perry | 1 | 1 | Mk 13 |
| Knox | 1 (§7) | 1 | BPDMS |
| Spruance | 2 (**STIMA**) | 1 | Sea Sparrow Mk 29 |
| Kidd | 2 | 2 | 2 Mk 26 |
| Ticonderoga | 2 | 2 | 2 Mk 26 o lancio verticale |
| Nimitz | 3 | 3 | 3 Mk 29 |
| Kitty Hawk | 3 (**STIMA**) | 3 | come la Nimitz |
| Iowa | 4 | 1 | nessun missile antiaereo |
| Sacramento | 2 | 1 | Sea Sparrow |
| Krivak II | 0 | 2 | 2 Osa-M |
| Kara | 0 | 4 | 2 Shtorm, 2 Osa-M |
| Kirov, Slava | 0 | 3 | S-300F, 2 Osa-M |
| Udaloy | 0 | 4 | Kinzhal, 4 bersagli |
| Koni, Nanuchka, Grisha III | 0 | 1 | 1 Osa-M |
| Type 22, Type 22 batch 2 | 0 | 2 | 2 Sea Wolf |
| Oslo | 0 | 1 | Sea Sparrow |
| Hauk, Stockholm | 0 | 1 | nessun missile antiaereo |

Un canale per lanciatore, la regola del dossier per l'Osa, con almeno 1 anche senza missili: il byte di una nave senza
missili sono i suoi cannoni e gli inganni. Tutti **STIMA**, tranne il Kinzhal.

**Chi resta fuori.** Una classe fuori tabella, come i mercantili, tira il byte intero senza limiti, in qualunque allerta. Un
sottomarino in superficie anche, e non riceve l'evento `arma` da un missile fermato.

**Gli esiti.** Calcolo esatto delle regole (`tools/re/prove/difese_navi/calcolo_esiti.py`), senza volo né guasti: all'impatto si
arriva con l'affidabilità di 0,95 (§2.9). Un missile solo, P(colpo):

| Nave (byte) | Solo byte | A0 | A1 | A2-A3 | A2-A3, P-500 |
|---|---|---|---|---|---|
| Krivak, Kara, Slava, Kirov (90) | 65% | 82% | 74% | 65% | 65% |
| Nanuchka, Koni (40) | 84% | 92% | 88% | 84% | 84% |
| Perry (15), Nimitz (15), Kidd (16) | 94% | 80% | 70-71% | 61% | 77-78% |
| Perry, Knox (128) | 50% | 62% | 46% | 32% | 41% |
| Spruance (0) | 100% | 82% | 74% | 65% | 82% |
| Iowa (17) | 93% | 75% | 63% | 51% | 72% |
| Sacramento (5) | 98% | 82% | 73% | 64% | 81% |
| Mercantile (0) | 100% | 100% | 100% | 100% | 100% |

Colpi attesi per salva, nave ad A2, solo byte → con le regole:

| Nave (byte) | Harpoon, 2 a 4 s | Harpoon, 4 a 4 s | P-500, 4 a 4 s |
|---|---|---|---|
| Krivak (90) | 1,30 → 1,30 | 2,59 → 3,30 | 2,59 → 3,30 |
| Slava, Kirov (90) | 1,30 → 1,30 | 2,59 → 2,95 | 2,59 → 2,95 |
| Kara (90) | 1,30 → 1,30 | 2,59 → 2,59 | 2,59 → 2,59 |
| Perry (15) | 1,88 → 1,59 | 3,77 → 3,24 | 3,77 → 3,25 |
| Perry (128) | 1,00 → 1,15 | 2,00 → 2,80 | 2,00 → 2,89 |
| Kidd (16) | 1,88 → 1,22 | 3,75 → 2,52 | 3,75 → 3,20 |
| Nimitz (15) | 1,88 → 1,22 | 3,77 → 2,49 | 3,77 → 3,15 |
| Iowa (17) | 1,87 → 1,06 | 3,73 → 2,16 | 3,73 → 3,05 |

In parole: le navi americane, col Phalanx, scendono vicino ai sovietici; le navi sovietiche ai posti di combattimento non
cambiano contro due missili e soffrono contro quattro, quindi concentrare la salva paga; la sorpresa paga poco: due Harpoon su
un Krivak fanno 1,47 colpi da A0 e 1,30 da A2.

### 2.7 I missili nemici

- **Antinave.** Gli script dell'IA usano le stesse istruzioni: MISSILE e, dopo il ritardo, ATTACK_RND sul bersaglio.
  La probabilità è `(256 − difesa) / 256` del bersaglio, il danno quello dello script. Nell'Enhanced la carica è
  quella della classe del lanciatore (§2.3).
- **Il MISSILE degli script nell'Enhanced.** Se la classe ha il missile del modello (`per_classe`), il MISSILE vola
  col modello del §2.9, col piano sulla soluzione dell'unità. Col piano non valido il lancio non parte: niente
  rumore, niente avviso al giocatore, ATTACK_RND muto (`missile_dello_script`, `EsitoMissileScript`). Una classe senza
  il missile del modello tiene il lancio dello script.
- **Antinave senza script** (Enhanced, `Missione::passo_missili_ia`, del runtime; docs/18, voci del 29/10/2026).
  L'IA tira da sé, col modello del volo e lo stesso piano del giocatore: fuori portata, o sotto la distanza minima, il
  missile non parte. All'impatto valgono le difese del §2.6, come per il giocatore.
  - **Chi tira:** le navi col missile antinave della classe (Slava, Kirov, Nanuchka, Perry, Spruance, Kidd, Iowa);
    Oscar e Charlie II; gli SSN occidentali con l'Harpoon dei tubi del loro carico. I sottomarini prima salgono alla
    quota e rallentano alla velocità di lancio, poi riprendono quota e velocità di prima.
  - **Su chi:** un bersaglio di superficie classificato:
    - dal sonarista, dalla vista (il libretto), dal radar identificato all'ESM, o da un contatto lungo al radar. Il
      contatto lungo del solo sonar passivo non basta: con la SQR-19 una Perry teneva per ore un cargo sovietico a
      30 km e gli tirava gli Harpoon (30/10/2026);
    - oppure con una traccia di 5 minuti di un contatto che si comporta da unità militare (oltre i 18 nodi, in
      manovra, o vicino a un'unità militare nota della sua parte);
    - sempre, se la missione dichiara `zona_di_guerra = true`.

    Mai su chi crede neutrale né sui non combattenti. Mai su un contatto che la vista o il sonarista credono un
    mercantile, anche se la classe creduta è sbagliata.
  - **L'area di ricerca libera:** il piano è quello del giocatore (§2.9). Se nell'area di ricerca del seeker, oltre al
    bersaglio, c'è un contatto creduto neutrale o di parte ignota, l'IA non tira e riprova più tardi, come chi vede la
    scatola sulla carta. In SCENE8A l'Harpoon di una Perry contro l'Alfa agganciava un peschereccio del traffico.
  - **Quando:** solo con le armi libere, solo nell'Enhanced con le forze e le armi dell'IA. Mai nelle missioni
    originali; il classico resta con ATTACK_RND.
  - **Quanti:** ogni lanciatore di `missili.toml` (`[[lanciatore]]`) ha due numeri:
  - `salva`, i missili di una salva: 4 per Slava, Kirov, Oscar e Iowa, 3 per la Nanuchka, 2 per gli altri. Un
    sottomarino emerso ne tira sempre uno;
  - `riserva`, i missili per le salve dopo la prima: Oscar 24, Kirov 20, Slava e Iowa 16, Charlie II, Spruance e Kidd
    8, Nanuchka 6, Perry 4. Gli SSN occidentali hanno riserva 0 e tirano dai tubi del loro carico; 0 vuol dire la
    stima del motore per la classe.

  I missili di una salva partono a 4 s l'uno dall'altro, dopo un minuto di reazione (`FRA_MISSILI_SALVA_S`). Dopo l'ultimo ci sono 120 s di
  valutazione del danno (`DOPO_SALVA_S`) prima di un'altra salva. Tutti questi numeri sono **STIMA** di gioco.
- **Contro il battello del giocatore.** Nella fisica dei danni del battello (capitolo B3) un missile conta solo se il
  battello è in superficie.
- **Antisom a distanza.** Nell'originale le armi antisom a distanza delle navi (SS-N-14, ASROC) non volano: un siluro
  nasce in acqua vicino al bersaglio (`docs/originale/comportamenti_ia.md`, nave antisommergibile, punto 3). Quel
  siluro, se la classe è nella tabella, corre col modello dell'Enhanced (capitolo D1).

### 2.8 Nel debriefing

Il registro (`missioni/src/registro_missione.cpp`) mette ogni lancio di missile senza unità fra gli eventi: per il
giocatore `lancio_giocatore` con la famiglia dell'arma (`harpoon`, `tomahawk`); per un'unità ostile `lancio_nemico`,
con l'arma «missile», noto se il giocatore ne ha il contatto. Un lancio già registrato nei 2,5 s prima non si ripete.
Un colpo di ATTACK_RND senza lanciatore va a chi ha lanciato da ultimo contro la vittima, entro 300 s. Il conto dei
lanci per arma va alle statistiche (capitolo F1).

### 2.9 Il piano, il volo e la pagina di tiro (Enhanced)

Solo nell'Enhanced coi missili del catalogo, per i tubi del giocatore con un Harpoon (UGM-84A o 84C, secondo l'anno)
o un TASM. Il TLAM-C ha il suo modello (§2.10). Il progetto è `docs/21_stazione_missili.md`, la ricerca
il dossier 27. Le parti:
- il modulo puro `nucleo/missili.hpp`, di trailer-1: dati, piano, volo, ricerca e aggancio;
- l'aggancio alla missione, del runtime: `missioni/src/missione_missili.cpp`;
- la pagina di F7, della principale: `app/stazione_missili`.

**Due mondi.** Il **piano** usa solo quello che il battello sa:
- la sua posizione **stimata**, con la navigazione incerta del capitolo C12;
- la soluzione del bersaglio con la sua ellisse;
- i contatti che conosce, con la classe creduta.

Il **volo** parte invece dalla posizione **vera** del battello, ma segue la rotta del piano. Lo scarto fra stimata e
vera al lancio sposta tutta la geometria: la tratta sul mare è inerziale e niente la corregge.

**I dati delle classi** (`data/motore/missili.toml`, da `tools/pipeline/missili.py`, confidenza media):

| Missile | Velocità | Portata | Distanza minima | Lancio | Seeker (piccola, media, grande) | Testata | PWR + ALN |
|---|---|---|---|---|---|---|---|
| UGM-84A Harpoon (1981-1997), pop-up | 240 m/s | 111 km | 5,6 km | 20-60 m, al più 10 kn | 12, 18, 23 km | 221 kg | 30 + 30 s |
| UGM-84C Harpoon (1982-1997), radente | 240 m/s | 111 km | 5,6 km | 20-60 m, al più 10 kn | 12, 18, 23 km | 221 kg | 30 + 30 s |
| UGM-109B TASM (1983-1994), con l'ESM | 245 m/s | 460 km | 9,3 km | fino a 60 m, al più 10 kn | 15, 25, 35 km | 454 kg | 600 + 600 s |

**La fonte dei dati** (`Missione::bersaglio_missile`):

| Fonte | Che cosa dà | Errore (1 σ) |
|---|---|---|
| TMA | la soluzione del MATE, mandata dalla stazione; senza, la posizione del contatto o la TMA dell'equipaggio | dal MATE (sotto); senza: 15% in distanza, 1° di traverso |
| periscopio | la distanza dello stadimetro, correggibile a mano a passi di 500 m | 5% in distanza, 0,5° di traverso |
| ESM | solo il rilevamento | solo BOL |
| radio | il rapporto del quadro radio, geografico, con l'ellisse che si gonfia dall'ora della posizione (30 nodi senza il moto) | quella del rapporto, più la navigazione del battello |

Contro un bersaglio del proprio sonar la geometria è relativa e l'errore di navigazione non conta; contro un dato radio
si somma. **Il MATE** manda la sua soluzione ai tubi coi missili quando cambia, al più ogni 10 s, arrotondata (100 m,
1°, mezzo nodo):
- l'errore di traverso viene dallo scarto quadratico medio dei rilevamenti contro la soluzione;
- quello in distanza è il 20% (STIMA, dossier 05 §1.4: errore 10-20%).

Va come ordini, così il replay lo ripete. Nella sandbox no.

**Il piano** (`pianifica`): rotta, tempo di volo, punto previsto, accensione del seeker, area di ricerca e probabilità.
- **RBL:** il missile vola al punto previsto e cerca in una **scatola** sull'ellisse all'arrivo. La mezza profondità è
  1,7, 2,5 o 3,5 σ (piccola, media, grande), con un minimo di 5, 10 o 15 km. La mezza larghezza ha un minimo di 3, 6 o
  10 km.
- **BOL:** il missile cerca a **ventaglio** dal punto d'accensione (SRCH): la semiapertura parte da 15° e si allarga di
  7,5° ogni 10 s, fino a 45°. Finisce all'autodistruzione (DSTR) o alla portata.
- **TASM:** una **serpentina** dal punto previsto, con gambe di 40 km a 25 km l'una dall'altra.
- **Probabilità di successo stimata:** P(il bersaglio è nell'area) × P(è il primo eco preso fra i contatti noti) ×
  affidabilità (0,95). È sulla soluzione stimata, senza le difese delle navi.
- **Avvisi bloccanti:** fuori portata, troppo vicino, quota oltre il massimo, solo BOL (senza la distanza), senza
  bersaglio. Bloccano il lancio.
- **Solo avvisi:** velocità oltre il massimo, sotto la quota minima dell'Harpoon (più guasti in salita), mare oltre il
  massimo del seeker, dato più vecchio di 30 minuti, neutrali nell'area, amici nell'area, rotta del bersaglio ignota.
  Gli amici vengono dalla classe che hai dato al contatto: l'avviso non ti dice di chi è un contatto che non hai
  classificato. Il giocatore può tirare lo stesso; l'IA e il comandante no (capitolo E6, §2.13.1).

**La preparazione** (spie PWR e ALN): l'accensione, poi l'allineamento. Un missile caricato prima della missione parte
già pronto; dopo il primo lancio da un tubo il prossimo si prepara da capo (STIMA di gioco). **RDY** vuole il piano
valido e ALN. **SLIDE:** una sola valvola aperta per lato. Dopo un lancio lo stesso lato aspetta 20 s e l'altro 4 s
(STIMA, docs/21 §3.3: 15-25 e 3-5 s). Sul 688 i tubi 1 e 3 sono a dritta, 2 e 4 a sinistra.

**Il volo** (`Missile`): salita della capsula o booster, crociera alla quota del missile sulla rotta del piano,
accensione del seeker, ricerca, inseguimento, terminale, impatto. Senza aggancio finisce il carburante e cade.
- **Seeker:** il DSQ-28 è un radar a ricerca sequenziale senza classificazione. Prende il **primo eco valido**
  dell'area, dal più vicino, nemico o neutrale che sia (dossier 27 §3). L'eco più grande dell'area si prende sempre;
  uno più piccolo con P 0,4 (`p_acquisizione_scorta`, Becker, STIMA), se no il seeker passa al successivo. Il piano
  stima la stessa cosa, e conta che un eco più grande del bersaglio lo «ruba» con P 0,97. Il lanciatore non è fra gli echi
  (`Missione::passo_missili`, dal 04/11/2026): prima una TASM a serpentina poteva tornare sul battello a galla che l'aveva
  lanciata.
- **Falsi bersagli:** un'unità col bit 0x80 (il «CRUISE MISSILE TARGET» della SCENE0A) non è un eco per nessuno, salvo per
  il missile che l'ha designata (`PianoTiro::contatto`, dal 04/11/2026). Prima i TASM della 0A lo sorvolavano e prendevano
  il traghetto a 28 km.
- **TASM:** l'ESM preferisce chi ha acceso i radar della classe voluta.
- **Mare:** oltre il massimo del seeker la portata cala del 30% e l'aggancio si può perdere.
- **Guasti:** un missile su venti si guasta in salita o in volo (affidabilità 0,95, GAO 1995).
- **L'aggancio:** al primo aggancio di un missile del giocatore arriva il messaggio 479 «Il missile ha agganciato il
  bersaglio», una volta per missile, solo se il giocatore lo può sapere. Lo vede lui, con la testa del periscopio fuori
  e il bersaglio entro l'orizzonte del periscopio (contro una sovrastruttura di 20 m). Oppure glielo riferisce
  un'unità amica che ha il bersaglio in contatto, non un sottomarino in immersione. **STIMA**: nel 1989 il missile non
  ha collegamento col battello.
- **La fine senza impatto:** un missile che finisce il carburante senza aggancio, o si guasta, scrive `fine_missile`
  nel registro (capitolo F1): esaurito o guasto, la classe, i secondi di volo, il punto. Per tutti i lanciatori. Chi
  l'ha lanciato non lo sa: per lui il missile semplicemente non arriva.

**L'impatto** (`Missione::passo_missili`): sulla nave agganciata, che può non essere il bersaglio. Prima le difese: su una
nave le tre regole del §2.6; su un sottomarino in superficie il byte intero, come l'ATTACK_RND (§2.3). Un missile fermato
scoppia 500 m prima e scrive `fine_missile` col codice 2 (`missile_abbattuto`). Se passa, il messaggio 50 e il danno di
attacco: 75 × testata / 221, almeno 30, con
la fisica dei danni dove c'è. Il colpo è di chi ha lanciato quel missile, con l'arma: prima il registro lo dava al +62
della vittima, che lì non vale (`Colpo::lanciatore`; il campo della vittima resta com'era per gli script). I missili in volo
sono unità della missione (categoria arma).

**La pagina di tiro** (stazione Armi, F7; volume 1, §6.4):
- **si apre da sola** quando il tubo scelto ha un missile del catalogo; Tab torna ai siluri;
- **in alto la carta di tiro:** la carta della Navigazione con stima del bersaglio, punto previsto, rotta del missile,
  accensione del seeker, area di ricerca, contatti nell'area (avviso sui neutrali), orizzonte radar del bersaglio,
  datum di 3 miglia sul punto di lancio. Dopo il lancio ci sono i missili in volo come stima dal tempo di volo,
  tratteggiati e marcati «STIMA», mai dalla verità;
- **in basso a sinistra la consolle:** MK 117 FIRE CONTROL sul Flight I, CCS MK 1 sul Flight II, BSY-1 sul 688I. Una
  riga per parametro; le righe che si scelgono sono FONTE, DISTANZA (solo col periscopio), MODO, RICERCA (RBL) o SRCH
  e DSTR (BOL), PREPARAZIONE;
- **in basso a destra la Weapons Launch Console:** per tubo le spie PWR, ALN, RDY, DOOR, SLIDE e il FIRE col coperchio.

| Tasto | Azione |
|---|---|
| frecce su e giù | la riga della consolle |
| frecce sinistra e destra | il valore: la fonte a giro (TMA, periscopio, ESM, radio); la distanza ±500 m; RBL o BOL; piccola, media, grande; SRCH e DSTR a passi di un miglio (0 = predefinita); la preparazione (destra avvia, sinistra spegne) |
| Maiusc+L | alza o abbassa il coperchio del FIRE |
| L | lancia, solo a coperchio alzato (se no «COPERCHIO CHIUSO»); poi il coperchio si riabbassa, e così al cambio di tubo |
| I, O | ingrandisce o allarga la carta: 2, 5, 10, 20, 50, 100, 200 km; all'apertura è automatica |
| C | centra la carta sul bersaglio o sull'area di lancio, a giro |
| Tab | torna ai siluri |
| 1-4, 7, G | tubo e bersaglio, come nella pagina dei siluri |

Col mouse le righe della consolle e il FIRE si cliccano. Il lancio passa da `StazioneArmi::lancia`, che col missile
vuole RDY e la slide libera (§2.2).

### 2.10 Il TLAM-C del modello (Enhanced)

Il runtime l'ha agganciato il 28/10/2026 (`missioni/src/missione_missili.cpp`). Il modulo puro è in `nucleo/missili.hpp`
(`valuta_tlam`, `esito_tlam`); la ricerca nel dossier 27 §4. Nel 1989 tutto si pianificava a terra: il battello parte
con un **pacchetto di missioni** e può solo scegliere quale eseguire, quando e se lanciare.

**La libreria di bordo** è il campo `tlam` della missione, una missione TLAM per elemento:

| Campo | Che cos'è |
|---|---|
| `id` | il numero della missione, quello che l'ordine d'esecuzione cita |
| `bersaglio` | il nome a terra |
| `plp = {x, y}` | il punto di lancio pianificato, in metri sul teatro |
| `landfall = {x, y}` | il primo punto a terra, la prima mappa TERCOM |
| `tot_s` | l'ora sul bersaglio, in secondi dall'inizio della missione |
| `finestra_s` | la tolleranza sull'ora, di norma 600 s |
| `tratta_terra_m` | la tratta sopra terra fino al bersaglio |
| `mezza_mappa_m` | la mezza larghezza della prima mappa, di norma 2,5 miglia (STIMA) |

**L'ordine d'esecuzione** arriva alla radio: un messaggio della missione con `esecuzione_tlam = "<id>"` rende
eseguibile quella missione quando il battello lo riceve (capitolo C11). Senza l'ordine i missili si preparano ma non
partono.

**Il pannello**, sulla posizione **stimata** del battello e sulla sua sigma di navigazione dichiarata (capitolo C12):

| Stato | Condizione |
|---|---|
| VALIDA | entro 10 miglia dal punto di lancio, dentro la finestra |
| RIPIANIFICA | fra 10 e 40 miglia: il CCS ricalcola la tratta sul mare in 10 minuti, che contano per la finestra |
| NON VALIDA | oltre 40 miglia, o la tratta (mare più terra) oltre la portata di 875 km |
| FUORI FINESTRA | l'ora di lancio necessaria per arrivare all'ora sul bersaglio è fuori di più della finestra |

- **Tempo di volo:** salita e booster, più (mare fino al landfall + tratta a terra) / 245 m/s. L'ora di lancio è l'ora
  sul bersaglio meno il tempo di volo.
- **Probabilità di prendere la prima mappa:** `1 − exp(−H² / 2σ²)`, con `H` la mezza mappa. La `σ` somma:
  - l'errore di posizione del battello;
  - la deriva dell'inerziale del missile sul mare, 0,5 miglia all'ora;
  - un decimo di grado di rotta.
- **RDY** vuole lo stato VALIDA o RIPIANIFICA, ALN e l'ordine ricevuto.

**L'esito** usa invece la posizione **vera** e l'errore vero di navigazione (`Missione::lancia_tlam`). Il missile:
- **parte** se lo stato vero non è NON VALIDA;
- **vola** con l'affidabilità del 95% (GAO 1995);
- **prende la prima mappa** con la probabilità vera.

Se si perde, cade sul mare (guasto in volo) o oltre il landfall, in inerziale. È il punto chiave del dossier 27 §4: un
equipaggio «sicuro di sé e sbagliato» vede «VALIDA» e un'alta probabilità e manca il landfall. L'errore del battello
passa uno a uno al missile, e la tratta sul mare non lo corregge. Un punto nave prima del lancio costa esposizione e
compra probabilità.

**La pagina di tiro** (`app/stazione_missili.cpp`, il TLAM nel tubo scelto): due righe, MISSIONE e PREPARAZIONE.
- **MISSIONE:** destra e sinistra danno l'ordine `campo_missile` col campo `missione_tlam`, a giro sulla libreria;
  senza libreria «LIBRERIA VUOTA».
- **Le righe lette**, sulla posizione stimata:
  - STATO con la validità (verde, giallo coi minuti della ripianificazione, rosso) e la distanza dal PLP;
  - NAVIGAZIONE «P TERCOM», verde da 0,8;
  - LANCIO, l'ora e il tempo di volo;
  - ORDINE: ricevuto, in attesa, o non autenticato (I12: un ordine con l'autenticazione sbagliata non arma la
    missione);
  - i CONTROLLI di quota e velocità.

**L'esito** (`Missione::passo_missili`), all'ora d'arrivo:
- **la verità al registro:** l'evento `esito_tlam`, con la missione, il bersaglio, arrivato o no, il tempo di volo, il
  punto e se è arrivato nella finestra. Il debriefing lo scrive «TLAM missione 0521: colpito …» oppure «perso prima
  di …»;
- **al giocatore, il BDA:** alla radio, 1800 s dopo (`BDA_TLAM_S`, **STIMA**), da COMSUBLANT, PRIORITY, SECRET:
  «BDA MISSIONE TLAM n: BERSAGLIO … COLPITO.» o «… NESSUN EFFETTO OSSERVATO SU ….»;
- **l'obiettivo `tlam`** delle missioni conta i TLAM arrivati, nella finestra se richiesta, di una missione o di tutte.

Le prove: `prove_missili.cpp` (le quattro validità, la presa della prima mappa, il «sicuro di sé e sbagliato»),
`prove_missili_missione.cpp` (dalla libreria all'ordine al lancio; l'ordine non autentico), `prove_stazione_missili.cpp`
(la M2: la missione dalla libreria, l'ordine dalla radio, l'area di lancio), `prove_libreria_missioni.cpp` (I12),
`prove_scenario_missione.cpp` (la libreria e l'obiettivo nello scrittore). Girate il 29/10/2026: 10 casi su 10.

**L'IA** che guida il 688 (il comandante dell'IA della sandbox e delle partite senza giocatore) sceglie la prima
missione della libreria, poi aspetta l'ordine (`comandante_ia.cpp`).

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Regole del lancio, ordine dei controlli, 5 miglia, 20 piedi | eseguibile dell'originale (`1000:790C`), `docs/originale/sensori_e_navigazione.md` §10.2 | alta |
| Ritardo, danno e ATTACK_RND | script delle scene, `docs/04_vm_missioni.md`, `docs/originale/prestazioni.md` | alta |
| Difesa delle classi | modelli delle scene dell'originale | alta (come dato del 1989) |
| Evento acustico di 100 s, portata di 20 miglia | `docs/originale/sensori_e_navigazione.md` §4 | alta |
| Prestazioni delle armi del catalogo | dossier 14 §2.2, §2.3, §2.5 | media (Sea Lance bassa) |
| Testate per classe del lanciatore | dossier 07 §4.1; dossier 05 §3.1 | media |
| Salva e riserva del tiro antinave dell'IA, intervalli fra i missili e dopo la salva | `missili.toml`, `missione_missili.cpp` | **STIMA** |
| Punto d'impatto: 2,5 m sopra il galleggiamento, ±25% della lunghezza | nessuna fonte | **STIMA** |
| Dati di Harpoon e TASM (`missili.toml`) | dossier 27, dossier 05 §1-§3, dossier 14 | media |
| Scatola RBL in sigma, ventaglio BOL, serpentina del TASM | dossier 27 §3.1 (Becker, NPS 1986) | **STIMA DI GIOCO** |
| Primo eco valido, P 0,4 per un eco non il più grande, 0,97 nel piano | dossier 27 §3 | **STIMA** |
| Slide valve 20 s e 4 s, preparazione PWR e ALN | dossier 27 §2 | **STIMA** |
| Errore della soluzione del MATE: 20% in distanza | dossier 05 §1.4 | **STIMA** |
| TLAM: validità a 10 e 40 miglia, finestra di 10 minuti, ripianificazione in 10 minuti | dossier 27 §4.2 | **STIMA** |
| TLAM: presa della prima mappa, deriva 0,5 nm/h, mezza mappa 2,5 nm | dossier 27 §4.2, appunti OTH §6.2 | **STIMA** |
| TLAM: portata da sottomarino 875 km, affidabilità 95% | GAO 1995 (dossier 05 §2.4) | media |
| Difese: fattore 0,5 ad A0 | dossier 05 §4.3 (rapporto fra bassa prontezza e allerta, 0,48, arrotondato) | **STIMA** |
| Difese: fattore 0,75 ad A1 | a metà fra A0 e A2 | **STIMA** |
| Difese: canali per classe, 30 s per missile | dossier 05 §4.2 e §4.4 (Osa-M, 2 colpi al minuto); il Kinzhal media | **STIMA** |
| Difese: Phalanx per classe | dossier 01 §4.1, dossier 22, enciclopedia, modello del kit | media; Spruance e Kitty Hawk **STIMA** |
| Difese: probabilità del Phalanx 0,35 e 0,45, × 0,5 contro un supersonico | dossier 05 §4.4 | **STIMA** del dossier |
| Difese: soglia del supersonico 340 m/s | nessuna fonte, separa i missili del catalogo | **STIMA** |
| Difese: Block 1 sull'Iowa dal 1988, le altre mai | dossier 05 §4.1 per l'Iowa; il resto del programma manca | **STIMA** prudente |
| Difese: affusto impegnato 1490 m / velocità | portata efficace del Block 0, dossier 05 §4.1 | calcolo |

Il dossier 05 ha anche la dottrina di salva (§1.6) e la probabilità storica di colpire una nave difesa, circa 0,44 per
Exocet e Harpoon (§4.3). Le regole del §2.6 non sono tarate su quel numero: con 65% all'impatto e 0,95 di affidabilità, il byte
sovietico resta sopra, ed è il dato dell'originale.

## 4. Classico ed Enhanced

Nel classico vale il §2.2 e il §2.3 fino all'ATTACK_RND: il danno toglie 75 punti d'integrità alla nave. Le missioni
originali hanno l'Harpoon nel tubo 7 (in una sola scena il Tomahawk); i gestori delle famiglie Sea Lance e Tomahawk,
nelle scene che non le usano, sono per lo più quelli di un siluro (`docs/originale/prestazioni.md`).

Solo nell'Enhanced:
- il carico (TASM, TLAM-C, pozzi verticali, missili nei tubi da siluri; capitolo D5);
- il danno fisico della testata (§2.3);
- il piano, la preparazione, il volo e la pagina di tiro di Harpoon e TASM del giocatore (§2.9);
- le difese delle navi contro i missili del modello (§2.6);
- l'effetto del lancio e la voce nel registro.

## 5. Come si vede in gioco

Volume 1, §6.4 (i missili) e §6.2 (tubi e lancio).

- **Stazione Armi** (F7): M sceglie il tubo 7 (messaggio 223 «Tubo lanciamissili selezionato»); il secondo clic lo
  arma (messaggi 12-14 «Missile armato»); G sceglie il bersaglio; L lancia. Nell'Enhanced, con un missile del catalogo
  nel tubo scelto, la pagina di tiro (§2.9): carta di tiro, consolle e Weapons Launch Console; Maiusc+L il coperchio,
  L il lancio.
- **Messaggi di rifiuto**: 226 arma non armata; 227 serve un bersaglio; 228 solo unità di superficie; 229 serve la
  distanza; 230 bersaglio troppo vicino; 231 quota periscopica.
- **Dopo il lancio**: 49 missile lanciato; 479 missile agganciato, se lo si può sapere; 50 missile a segno; per i lanci
  altrui il 48.
- **Periscopio**: la capsula o il booster che escono, la fiammata e il fumo, anche per i lanci della pagina di tiro
  e il TLAM; dalle navi la vampa e la nube sul ponte; sulla nave colpita fiamme e fumo (capitolo E1).
- **Debriefing**: i lanci per arma e i colpi (capitolo F1).

## 6. Taratura e prove

Nessuna taratura: le probabilità del classico sono quelle del 1989, i valori del §2.6 vengono dal dossier o sono **STIMA**.

Prove:
- `test/prove_armi_giocatore.cpp`: «armi: controlli dei missili del tubo 7 nell'ordine dell'originale»;
- `test/prove_danni_sottomarini.cpp`: «missioni: lanci di missili registrati per gli effetti»; «effetti: missili
  lanciati da un sottomarino in immersione», con il sottocaso del lancio dal ponte di una nave;
- `test/prove_effetti.cpp`: «effetti: nave colpita da un missile, fiamme e colonna di fumo sottovento»;
- `test/prove_transitori_armi.cpp`: «transitori armi: il lancio dal tubo in sequenza»;
- `test/prove_carico.cpp`: «carico: Flight II, i Tomahawk della dotazione nei pozzi».

Il modello del §2.9:
- `test/prove_missili.cpp` (`missili:`):
  - il catalogo con fonte e confidenza, e le classi per anno;
  - l'ellisse che cresce col moto noto e senza;
  - la scatola RBL sul previsto, che scala con la sigma; fuori portata e troppo vicino bloccano;
  - sotto la quota minima dell'Harpoon un avviso e più guasti;
  - il ventaglio BOL e la serpentina del TASM;
  - i contatti nell'area dal più vicino, l'avviso dei neutrali, la probabilità che scende;
  - la preparazione;
  - il volo sul previsto, e uno scarto di navigazione che sposta tutto;
  - il primo eco valido anche se neutrale, e il TASM che preferisce chi emette;
  - il TLAM;
  - deterministici dal seme.
- `test/prove_missili_missione.cpp`: il tiro del tubo, il lancio e il primo eco; lo stesso seme, lo stesso volo;
  senza piano valido il lancio non parte, e il BOL va col solo rilevamento; il TLAM, dalla libreria all'ordine
  d'esecuzione al lancio (senza missione non parte, senza ordine non è RDY); una Perry vicino allo Slava riferisce
  l'aggancio (479), e col piano a 60 km il missile non trova niente e finisce nel registro (`fine_missile`);
- `data/missioni/prove/PROVA_M2_TLAM.lua`: la scena di prova, con due missioni della libreria nel Mare di
  Barents.
- `test/prove_stazione_missili.cpp`: i comandi della consolle diventano gli ordini giusti; il coperchio del FIRE, lo
  zoom e il centro; la carta di tiro e il pannello sulla scena M1; i contatti nell'area e i missili in volo come
  STIMA.

Le difese del §2.6:
- `test/prove_difese_navi.cpp` (`difese delle navi:`): l'allerta che scala byte e Phalanx; i canali e gli affusti impegnati;
  il supersonico e il Block 1; la classe fuori tabella con la stessa sequenza casuale; le frequenze su 20 000 prove contro
  `calcolo_esiti.py`, entro l'1%; lo stesso seme, lo stesso esito;
- `test/prove_missili_missione.cpp` (`difese delle navi nella missione:`): la saturazione per il giocatore e per l'IA; la salva
  sulla Perry; la provocazione del lancio che non conta; il missile fermato che alza l'allerta, tranne quello
  d'esercitazione; il sottomarino in superficie; le forze spente.
- **Serie prima e dopo** (05/11/2026, 10 semi per scena, binari fissi; `tools/re/prove/difese_navi/nota_fase2.md`): `PROVA_T2_MISSILI`
  da 8 fermati e 12 a segno a 4 e 16, su 20 P-500; `PROVA_M3_ORIZZONTE` con la spia da 40 e 49 a 17 e 55, con 90 P-500 invece di
  108 perché le Perry affondano prima; S5 identica. In tutte e due il bersaglio è solo la Perry: le navi sovietiche sotto una
  salva le vedono solo le prove. Dieci semi mostrano solo gli effetti grossi; le frequenze le controlla la prova.

## 7. Limiti noti e in arrivo

- Il volo c'è nell'Enhanced per i missili antinave del giocatore e dell'IA che hanno il modello (§2.7, §2.9). Il
  classico usa ancora il gestore: tempo di volo fisso, nessuna portata, il bersaglio è sempre quello designato.
- **L'IA e i missili:** le navi non hanno un ricevitore ESM nei dati (`[esm_di]` di emettitori.toml ha solo
  sottomarini). Il sonarista dell'IA può metterci molto a classificare una nave veloce. La TMA a rotta costante può
  stimare il bersaglio troppo vicino, e allora un missile con la distanza minima (il P-120 del Charlie II, 10 km) non
  parte (docs/18, 29/10/2026).
- Il danno dell'impatto resta quello di ATTACK_RND scalato con la testata.
- Le cariche sono costanti del codice (`testata_missile`), non la `testata_kg` del catalogo: il TASM vale 454 kg nel
  danno e 450 nel catalogo.
- Su un sottomarino in superficie ogni missile vale 220 kg, qualunque sia il missile.
- Il lancio del gestore non produce i transitori della capsula e del booster nella scena acustica (§2.5).
- I lanci degli aerei non hanno ancora un effetto al periscopio.
- I missili sovietici ci sono già in `missili.toml`: P-700, P-500, P-120 e P-70. Il P-70 è solo nel catalogo: nessun
  `[[lanciatore]]` lo usa. L'Alfa non ha lanciatori di missili.
- **Le difese delle navi (§2.6) sono tre regole all'impatto, non oggetti:**
  - niente missili antiaerei, chaff o disturbo: il byte li riassume. Niente traccianti né effetti nuovi;
  - niente archi di tiro: tutti gli affusti vedono tutto. Il Phalanx non consuma colpi;
  - il byte è per scena, non per classe. Una Perry col byte 128 (32% di colpi ad A2) è più difesa di una Kidd (61%), perché il
    Phalanx si somma a un byte già alto. Accettato: cambiare i byte toccherebbe una ventina di missioni (04/11/2026);
  - nessun AK-630, per decisione dell'utente (04/11/2026). Un Iowa (51%) è più difeso di un Kirov con 8 AK-630 (65%);
  - la Knox ha il Phalanx nella tabella delle difese, dall'enciclopedia, e in partita ferma i missili come gli altri; il
    modello 3D del kit però non lo disegna, quindi al periscopio non si vede. La Spruance ha 2 Phalanx solo dal modello 3D
    del kit (**STIMA**);
  - l'ATTACK_RND degli script e i lanci senza modello restano col byte intero;
  - il secondo missile di una salva, lanciato dopo la provocazione del primo, ha la fotografia ad A2 anche se il primo si
    guasta in volo: in quel 5% dei casi la sorpresa vale meno;
  - la probabilità del piano resta «senza le difese delle navi» (§2.9), e l'IA non conosce le difese: non adatta le salve.

## 8. Riferimenti

- Codice: `nucleo/armi_giocatore.hpp`, `nucleo/src/armi_giocatore.cpp` (`StazioneArmi::lancia`),
  `missioni/src/missione.cpp` (`f_lancio_missile`, `f_attacco_casuale`, `registra_lancio_missile`, il lancio per
  famiglia in `Missione::applica`), `missioni/src/danni_missione.cpp` (`testata_missile`),
  `missioni/src/danni_sottomarini.cpp` (`DanniSottomarini::colpisci`), `missioni/src/registro_missione.cpp`, `missioni/src/missione_transitori.cpp`,
  `nucleo/src/transitori_armi.cpp`, `app/partita.cpp`.
- Il modello del §2.9: `nucleo/missili.hpp`, `nucleo/src/missili.cpp`, `missioni/src/missione_missili.cpp`
  (`bersaglio_missile`, `tiro_missile`, `lancia_missile_modello`, `passo_missili`), `app/stazione_missili.hpp` e
  `.cpp`, `app/interfaccia.cpp` (la soluzione del MATE ai tubi).
- Le difese del §2.6: `nucleo/difese_navi.hpp`, `nucleo/src/difese_navi.cpp`, `missioni/src/missione_forze.cpp`
  (`livelli_forze`, `allerta_contro_missile`); la proposta e le decisioni in `tools/re/prove/difese_navi/proposta.md`.
- Dati: `data/motore/carico.toml [armi.harpoon]`, `[armi.tasm]`, `[armi.tlam_c]`, `[armi.sea_lance]`;
  `data/motore/missili.toml`, con `[[difesa_nave]]`.
- Ricerca: dossier 05 §1-4; dossier 14 §2.2-2.5; dossier 07 §4.1; dossier 27.
- Progetto: `docs/21_stazione_missili.md`; `docs/00_ROADMAP.md` punto 25; `docs/04_vm_missioni.md`; `docs/10_danni_affondamento_esplosioni.md`
  §10.15; `docs/originale/prestazioni.md`; `docs/originale/sensori_e_navigazione.md` §4 e §10.2.
- Capitoli: D5 carico e ricarica, D1 siluri, E1 esplosioni, E2 danni e affondamento delle navi, B3 danni del battello,
  F1 registro e debriefing.
