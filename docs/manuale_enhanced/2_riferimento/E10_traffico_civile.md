# E10. Traffico civile

*Autore: disassemblatore (rotte e modello); la presenza nella missione è del runtime. Revisione del 28/10/2026.
Stato: rivisto (curatore: kit).*

## 1. A che serve

Il mare dei teatri non è vuoto. Ci sono:
- mercantili e petroliere sulle rotte;
- traghetti di linea all'orario;
- pescherecci sui banchi;
- d'inverno, i rompighiaccio.

Ogni nave è un contatto vero, con la sua firma, per te e per l'IA.

Il primo lavoro del sonarista vero è tenere il bersaglio fra le navi, e qui diventa anche tuo:
- **sulla cascata:** il BTR è pieno di tracce di mercantili, e il silenzioso va cercato fra loro;
- **sul LOFAR:** due navi dello stesso tipo hanno righe diverse;
- **nella tattica:** un mercantile che passa vicino copre, e una petroliera è un bersaglio da non sbagliare.

## 2. Il modello

### 2.1 Le rotte dei teatri

Il file `data/motore/teatri/<teatro>.traffico.toml` (scritto da `tools/pipeline/traffico.py`) ha, sul piano del
teatro:
- **corsie:** la spezzata, la larghezza, il senso unico o doppio, le navi al giorno per senso, la miscela dei tipi,
  i mesi, e per i traghetti di linea le ore delle partenze;
- **zone di pesca:** i punti d'acqua buona, quanti pescherecci, la velocità al lavoro, la miscela, i mesi.

Il pipeline controlla le corsie sul fondale:
- l'acqua dev'essere almeno il pescaggio più fondo dei tipi più un metro (meno nei porti del Baltico);
- i porti si agganciano all'acqua utile più vicina, e i tratti che toccano terra si instradano col percorso d'acqua
  più breve (A* sulla griglia del fondale).

Le zone sono punti ogni 3 km fra le profondità della zona.

| Teatro | Mercantili | Traghetti e linee | Pesca | Altro |
|---|---|---|---|---|
| Gibilterra | lo schema di separazione dello stretto, i ventagli verso l'Atlantico e il Mediterraneo: 140 al giorno | Algeciras-Ceuta, Algeciras-Tangeri, Tarifa-Tangeri, Malaga-Melilla | Golfo di Cadice, Alboran, Marocco | — |
| Otranto | l'asse dell'Adriatico con i rami verso lo Ionio e Messina, il costiero pugliese, Durazzo | Brindisi e Bari verso la Grecia (fitti d'estate), Dubrovnik, Bar | Fossa Sud-Adriatica, piattaforma pugliese, Leuca, lampare dell'Epiro | — |
| Celtica | i flussi atlantici di Ouessant, Irlanda-Iberia | — | il ciglio della piattaforma (Gran Sol); il tonno bianco d'estate | Ouessant è a est del riquadro |
| Baltico | la Route T, Kiel, Lubecca, Rostock, il Sund verso Bornholm e Stettino | Puttgarden-Rødby, il Grande Belt, Gedser, Trelleborg, Mukran-Klaipėda, Bornholm | Arkona e Bornholm, baia di Kiel | il rompighiaccio da gennaio a marzo |
| Barents | dal Golfo di Kola verso il Mar Bianco, la Rotta del Mare del Nord, Vardø, Svalbard | — | i banchi del Murman, la zona grigia | il rompighiaccio da novembre a giugno |
| Faroe | il varco verso l'Islanda, la Norvegia | Tórshavn-Scozia, Lerwick, Seyðisfjörður (d'estate) | piattaforma, Banco delle Faroe, pesce azzurro da gennaio ad aprile | i pescherecci sovietici in transito |
| Kunashir | poche navi sovietiche per Ekaterina e Shikotan, i costieri giapponesi di Nemuro | — | saury (agosto-novembre), kombu (settembre), granchio, merluzzo, salmone | — |

### 2.2 Le navi

`nucleo/traffico.hpp`, classe `Traffico`. Il traffico è una funzione del tempo e del seme della partita: le stesse
navi nella stessa partita, anche in una ripresa.

**Corsie.**
- Ogni corsia è un nastro: gli arrivi a ogni capo sono un processo di Poisson con la portata della corsia, a fasce di
  10 minuti con un seme fisso. I traghetti di linea partono invece alle ore locali dell'orario.
- Ogni nave ha il suo tipo dalla miscela, la velocità di servizio del suo individuo (85-100%) e uno scarto laterale:
  - a doppio senso sta a destra dell'asse, da un decimo della larghezza in su (σ un quarto della larghezza);
  - a senso unico è attorno all'asse.
- Ai vertici della spezzata lo scarto gira su 3 km.

**Zone di pesca.** Ogni peschereccio rimorchia da un punto della zona a uno vicino, entro 15 km, alla velocità della
zona (±20%). I rimorchi partono dall'inizio della partita, ognuno a metà di un rimorchio: la stessa barca anche in
una ripresa.

**I mesi e l'ora.** Corsie e zone con i mesi valgono solo in quei mesi: il mese è quello della missione (la sua data,
o quello di riserva del teatro). L'ora locale degli orari è l'ora solare: l'ora UTC della missione più la longitudine
del centro diviso 15.

**Nella scena.**
- `Traffico::avanza` tiene le navi entro 100 km dal giocatore, al più 40, le più vicine. Le altre restano nel rumore
  del traffico lontano (capitolo C3).
- Escono quelle oltre 110 km, e quelle scavalcate da una nave più vicina di 10 km (due per passo). Una nave che esce
  può rientrare col suo indice.
- La firma di ogni nave è l'individuo del capitolo C4, 2.10.
- **Fuga:** vicino a un'esplosione la missione chiama `fuggi`. La nave si allontana alla velocità massima del suo
  individuo, e finita la fuga torna in rotta in 10 minuti.
- **Perdita:** colpita, affondata o incagliata, la nave esce dal traffico per sempre (`perduta`), e il resto lo fa la
  missione come per ogni nave.

### 2.3 Nella missione

`missioni/src/missione_traffico.cpp` (`Missione::passo_traffico`), col contratto di `docs/20_sonar_blocco1.md` §3. Il
modulo decide, la missione applica.

**Quando c'è.**
- Solo nell'Enhanced, con la regola di simulazione e l'acustica.
- Acceso di norma in tutte le partite. Si spegne con `--traffico no` da riga di comando, o con la riga «TRAFFICO
  CIVILE» delle regolazioni della sandbox (capitolo F8 §2.2). La scelta va nel codice partita e nel codice sandbox, col
  bit che era di riserva (capitolo A1 §2.7): i codici di prima la leggono accesa.
- Mai nelle scene di prova: le missioni `PROVA_*`, quelle in `prove/` e quelle della cartella `prove` (`main.cpp`).
- Una missione lo spegne sempre con la chiave `traffico = false`.
- Un teatro senza il file `<teatro>.traffico.toml` non ha traffico.

**Le unità.** Una volta al secondo `Traffico::avanza` gira sulla posizione vera del giocatore, col tempo del mondo
(col moto classico le navi vanno come le altre unità). Poi:
- per ogni entrata un'unità vera: id da 10000, categoria nave, la classe del tipo (`ClasseNave::civile`), neutrale e
  non combattente (`flag` 42h), senza sonar. Le classi civili (cargo, mercantile, petroliera, peschereccio,
  portarinfuse, petroliera_costiera, traghetto, rimorchiatore, rompighiaccio, nave_ricerca, bmrt,
  peschereccio_costiero, cianciolo) hanno in `acustica.toml` una suite dichiarata vuota: anche una missione che desse
  loro `sonar` > 0 non le fa ripiegare sull'SQS-53. Le loro firme sono quelle tipiche del tipo civile, generate al
  caricamento del catalogo dall'id (FNV-1a);
- per ogni uscita l'unità si toglie: non è un affondamento;
- a ogni passo posizione, rotta e velocità si copiano dalle navi del modulo. Le navi del traffico non passano dal
  passo dell'IA: niente pilota, niente aggiramento delle secche;
- una nave colpita, affondata, distrutta o incagliata esce dal traffico (`Traffico::perduta`) e resta un'unità come le
  altre;
- un'esplosione (un colpo del passo) entro 10 km fa fuggire le navi vicine per 30 minuti (`FUGA_M`, `FUGA_S`,
  **STIMA**).

**I sensori.** Le navi sono solo sorgenti:
- la firma è quella dell'individuo (`UnitaAcustica::firma`, capitolo C4 §2.10); le sentono il giocatore e l'IA;
- ogni osservatore valuta il traffico un turno su quattro (`TRAFFICO_OGNI_TURNI`), per il costo;
- oltre 30 km la perdita è quella del livello A, senza il campo di raggi (`TRAFFICO_CAMPO_M`, capitolo C2). Ora il gradino si
  vede anche sulla cascata: una nave che passa i 30 km perde in un colpo quello che aveva sopra il fondo, e la sua striscia
  finisce (SCENE5A, in 50 minuti: 8 navi, da 6-17 dB a meno di 1,5 dB). È lo stesso gradino del sensore. Il contatto però non
  cade insieme alla striscia: la qualità della traccia scende piano (1500 a 30,0 km, 1436 a 30,6 km);
- il radar di navigazione civile è acceso come quello di un mercantile (`emettitori.toml`, capitolo C10);
- i pescherecci a strascico del traffico tirano la rete come gli altri (capitolo C6 §2.9);
- le navi del traffico emettono, ma non scoprono niente.

**Le regole d'ingaggio.** Le classi civili sono della categoria ROE «mercantili».
- Gli script delle missioni originali non le vedono: nessuno le sceglie come bersaglio.
- L'IA non le attacca per l'errore d'identificazione ad allerta 3 che vale per i neutrali dichiarati dalla missione
  (capitolo E6): con 40 navi attorno sarebbe una strage. Il rischio resta all'arma: un missile in ricerca può
  agganciare la nave sbagliata (capitolo D3).
- In guerra con i neutrali, o con le ROE della sandbox che li liberano, si possono attaccare come gli altri.
- Possono urtare un sottomarino (capitolo E3); le navi fra loro non si urtano.

**Il replay.** La traccia scrive le navi neutrali che il giocatore non ha a contatto al più ogni 25 s, e subito se
cambia lo stato (colpita, distrutta, alberi): `RegistratoreTraccia::PASSO_NEUTRALI_S` in `nucleo/traccia_replay.hpp`.
Così il replay le interpola ancora (sotto i 30 s) e il file resta piccolo.

## 3. Fonti e confidenza

| Che cosa | Fonte | Confidenza |
|---|---|---|
| Schemi di separazione di Gibilterra (forma) e di Ouessant (1980-2003, 2003) | appunti 26 §1.1, §3.1 (IMO SN/Circ.232; Mavor 1980) | media; i vertici anni '80 sono **STIMA** |
| Portate di Gibilterra (140 al giorno nel 1979-80), di Ouessant, del Baltico (HELCOM 2008 riportato agli anni '80) | appunti 26 §1.2, §3.1, §4.2 | media per i dati, **STIMA** il riporto |
| Corsie di Otranto, Barents, Faroe, Kunashir | appunti 26 §2, §5, §6, §7: geografia e archi di cerchio massimo | **STIMA** |
| Traghetti degli anni '80 | appunti 26 §1.4, §2.4, §4.3, §6.4 | bassa-media per le linee, **STIMA** le frequenze |
| Zone e stagioni di pesca | appunti 26 §1.3, §2.3, §3.3, §4.4, §5.3, §6.3, §7.3 | media per le stagioni, **STIMA** i numeri |
| Tetto di 40 navi, 100 km, 10 km per scavalcare | scelta di gioco (costo) | — |
| Fuga entro 10 km da un'esplosione, per 30 minuti | scelta di gioco | **STIMA** |
| Un turno su quattro ai sensori, livello A oltre 30 km | scelta di gioco (costo) | — |

## 4. Classico ed Enhanced

Il classico non ha traffico: ci sono solo le navi della missione. Il traffico è solo dell'Enhanced, con la regola di
simulazione e l'acustica.

## 5. Come si vede in gioco

- **Sonar:** tracce di mercantili sulla cascata e righe diverse sul LOFAR. Le navi si classificano con la libreria:
  i tipi civili sono voci con gli indizi. L'analizzatore di firma riconosce di rado il traffico per classe: da 3 a 4 casi
  su 78 fra 3 e 20 km, con una media di 3-5 righe viste. Oltre 30 km il traffico resta al livello A (capitolo C2 §7).
- **Carta, periscopio e radar:** i contatti neutrali. I tipi nuovi hanno il modello 3D del tipo più vicino.
- **ESM:** il radar di navigazione dei civili («radar di navigazione civile», `nav_civ`).
- **Opzioni:** `--traffico si|no`, la riga «TRAFFICO CIVILE» della sandbox («NAVI CIVILI NEI TEATRI» o «NESSUNO»), il
  codice partita (volume 1, capitolo 11).
- **Replay:** le navi del traffico ci sono, rade se il giocatore non le ha a contatto.

Volume 1: capitolo 3 (il traffico civile sul sonar) e capitolo 11 (il codice partita).

## 6. Taratura e prove

`test/prove_traffico.cpp` (`traffico:`):
- «allo stato stazionario le navi sono quante dice la portata, ripetibili»;
- «le navi scorrono, a destra nel doppio senso, e i pescherecci restano sul banco»;
- «il tetto tiene le più vicine, le uscite e le entrate seguono il giocatore»;
- «i traghetti di linea partono all'ora, e le navi fuggono e si perdono»;
- «i file dei teatri si caricano e le corsie hanno tipi noti».

`test/prove_traffico_missione.cpp` (`traffico nella missione:`):
- «spento di norma, acceso con le navi vere sotto il tetto»;
- «la chiave traffico = false lo spegne»;
- «lo stesso seme, le stesse navi».

Altre prove:
- `test/diag_originali.cpp`: «traffico: comandante e IA non sparano alle navi del traffico» (tre originali in guerra,
  due semi, 40 minuti);
- `test/prove_traccia_replay.cpp`: «traccia: le navi neutrali senza contatto ogni 25 s, subito se cambia lo stato
  (blocco 1 del sonar)»;
- `test/prove_navi_civili.cpp` (`navi civili:`): i tipi e le firme degli individui (capitolo C4).

Il pipeline stampa un avviso («ATTENZIONE») se una corsia non trova acqua utile, o se una zona ha meno di 5 punti; il
file si scrive comunque.

## 7. Limiti noti e in arrivo

- Le navi non si evitano fra loro: lo scarto laterale le separa, ma due che si sorpassano allo stesso scarto possono
  sovrapporsi. Non si urtano, perché la missione non guarda le collisioni fra navi (capitolo E3).
- Le navi del traffico non evitano il giocatore né le secche: seguono la corsia, validata sul fondale dal pipeline.
- L'ora degli orari è l'ora solare del centro del teatro, non l'ora legale del porto.
- Niente ora del giorno per la pesca: le lampare di notte e il rientro in porto la sera non ci sono. Niente ghiaccio
  che chiude le rotte, a parte i mesi dei rompighiaccio.
- Le rotte sono una per corsia: i ventagli fuori dagli stretti sono corsie separate.
- La scelta della missione non ha una voce per il traffico: fuori dalla sandbox si spegne solo da riga di comando.

## 8. Riferimenti

- Codice: `nucleo/traffico.hpp`, `nucleo/src/traffico.cpp`, `nucleo/navi_civili.hpp`;
  `missioni/src/missione_traffico.cpp` (`passo_traffico`), `missioni/src/missione.cpp` (la firma, i sensori, gli
  script, le reti), `missioni/src/missione_forze.cpp` (`armi_libere_ia`), `nucleo/traccia_replay.hpp`
  (`PASSO_NEUTRALI_S`), `app/main.cpp` (`--traffico`, le scene di prova), `app/codice_partita.hpp`,
  `app/regolazioni_sandbox.cpp`.
- Dati: `data/motore/teatri/<teatro>.traffico.toml` (da `tools/pipeline/traffico.py`), `[civili.*]` di
  `data/motore/acustica.toml` (da `tools/pipeline/civili.py`), le classi civili in `data/motore/classi.toml`, il
  radar in `data/motore/emettitori.toml`.
- Ricerca: dossier 26 (`docs/ricerca/26_navi_civili_traffico.md`, appunti `26_note/rotte_traffico.md`); dossier 03,
  dossier 09 §5.7.
- Progetto: `docs/20_sonar_blocco1.md` §3.
- Volume 2: capitoli A1, C2, C3, C4, C6, C10, D3, E3, E6, F8.
