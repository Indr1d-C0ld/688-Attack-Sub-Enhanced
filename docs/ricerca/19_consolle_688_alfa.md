# 19 — Le postazioni vere del 688 Flight I e dell'Alfa (Pr. 705), per differenziare le stazioni

> Ambientazione: **1989**. I due battelli giocabili: SSN-688 *Los Angeles* Flight I e Pr. 705/705K *Lira* («Alfa»).
> Scopo: il requisito 12 di `docs/00_ROADMAP.md` e la Fase 3. Oggi le stazioni grafiche sono le stesse per i due battelli
> e cambiano solo le funzioni. Qui si raccoglie com'erano davvero le postazioni: disposizione, apparati, colori,
> materiali, luci e caratteri. Poi si propone per ogni stazione come renderla a 640×480 nello stile VGA del gioco.
> Solo ricerca e proposta: il codice delle stazioni è in `app/`, della sessione principale.
> Compilato: 25 ottobre 2026, da due ricerche parallele (688 su fonti USA, Alfa su fonti russe) con la sintesi del kit.

---

## 0. Convenzioni, fonti e avvertenze

**Confidenza.** **alta** = fonte primaria o ufficiale (manuale tecnico NAVSEA, norma MIL, articolo del progettista,
memorie di un comandante con dettagli, museo); **media** = fonte secondaria seria (FAS, Submarine Review, NOVA,
Clancy, Wikipedia con fonti); **bassa** = forum, blog, gioco; **n.v.** = non verificato. **[GIOCO]** = proposta per il
remake; **STIMA** = valore scelto da noi.

**Etichette dell'Alfa.** **[705]** = provato per il Pr. 705/705K; **[GEN]** = vale per i battelli sovietici
dell'epoca, non dimostrato per il 705.

**Due cautele sulle fonti.**
- **688.** Molte foto e descrizioni «dentro un 688» riguardano il **688I** degli anni '90 (USS Springfield nel
  documentario NOVA, USS Miami nel libro di Clancy), oppure i musei con le classi Sturgeon (637) e Permit. Sono gli
  analoghi più vicini e sono etichettati come tali. Nessun interno di 688 è aperto al pubblico.
- **Alfa.** La fonte migliore è il numero speciale dell'almanacco *Тайфун* sul Pr. 705 (2001-2002), con:
  - l'articolo di **В. А. Собакин** sull'automazione, con le didascalie delle foto del posto centrale;
  - le memorie dei comandanti di K-123 (**Богатырёв**), K-493 (**Коляда**) e K-432 (**Баранов**);
  - la descrizione di progetto del 705K.

  Il numero è leggibile su tech.wikireading.ru, capitoli 5510-5526. L'articolo di **Sutyagin** (Submarine Review
  1998) è la fonte occidentale. È utile, ma in due punti contraddice Собакин (§3.2): si segue Собакин.

**Le sigle delle fonti** sono al §7 con gli URL.

**Accesso (25-10-2026).** Non raggiungibili: globalsecurity, DTIC, USNI Proceedings, le pagine dell'esposizione
Smithsonian (solo gli estratti delle ricerche), il Naval Undersea Museum, deepstorm.ru (codifica illeggibile).

---

## 1. Lo stato di oggi

Le stazioni del remake hanno la vista del periscopio in alto (360 righe) e il pannello della stazione in basso (120
righe). Il pannello è grigio scuro, coi pulsanti a riquadro e il testo del font originale. Sonar (cascate), radio,
carta (F6), sala siluri e quadro danni (F3) hanno anche una vista a tutto schermo.

Fra 688 e Alfa cambiano solo i dati: numero dei tubi, quote, velocità, ricariche. Le schermate sono le stesse
(istantanee di `SCENE1A` col 688 e `SCENE2B` con l'Alfa, stazioni 3, 4, 6, 7 e 8).

---

## 2. SSN-688 Flight I, circa 1989

### 2.1 La centrale («control room», anche attack center)

- È al ponte superiore del compartimento prodiero e fa anche da centro d'attacco `[FAS-688]` (media). La sala sonar
  è un locale a parte, attiguo `[SI-637]` (alta, per il 637); a prora della centrale `[NE]` (bassa).
- **Periscopi.** Due, su una pedana rialzata al centro `[NOVA-C]` `[CLANCY]` (media):
  - **Type 2**: d'attacco, testa da 1,4 pollici, solo diurno, progetto degli anni '40 ancora in servizio negli anni
    '90;
  - **Type 18**: di ricerca (Kollmorgen, dal 1975, oltre 40 sui 637 e 688). È stabilizzato, ha la luce bassa, la
    TV a bassa luminosità, la fotocamera e una sezione ESM `[SR-2002]` (media-alta). Riceve anche VLF, LF, VHF e
    UHF `[FAS-SCMP]`.
  - Il Type 2 sta a sinistra e il Type 18 a dritta `[FAS-688]` (media; un articolo sullo Springfield dice il
    contrario). Quale si chiami «numero 1» su un 688: n.v.
- **Posizioni attorno alla pedana** (688I, Clancy; media):
  - l'ufficiale di guardia sta sul davanti della pedana, rivolto ai quadri di stato;
  - il governo è a prora sinistra;
  - la fila delle consolle di tiro è a dritta; la più a poppiera è il **Weapons Control Panel** col pulsante rosso
    di fuoco `[NOVA-C]`;
  - la navigazione si fa su grandi tavoli di plottaggio `[NOVA-C]`. Posizione esatta e nomi (DRT, NAV plot) n.v.
- **Luci** `[NOVA-S]` (media):
  - di giorno la centrale è illuminata in bianco;
  - di notte si «arma al buio» (*rig for black*), per gli occhi dell'ufficiale e perché nessuna luce esca dal
    periscopio alzato;
  - prima delle osservazioni notturne si passa alla luce rossa (*rig for red*) (bassa-media).

### 2.2 Governo e casse (ship control e BCP)

- **Quattro uomini al governo** `[NOVA-C]` `[SR-2004]`:
  - **timoniere** e **pianista** sui due sedili davanti;
  - **ufficiale d'immersione** (Diving Officer of the Watch) dietro di loro;
  - **capo della guardia** (Chief of the Watch) fuori bordo a sinistra, al **Ballast Control Panel**, che segue la
    curva dello scafo accanto al governo.

  La classe Virginia li ridurrà a due.
- **Le colonne di comando**, dal manuale tecnico NAVSEA `[NSTM-561]` (alta):
  - due postazioni, interna ed esterna, rivolte al pannello di governo, ognuna con una **colonna a cloche e un
    volante** («aircraft-type control column»);
  - avanti = immersione, indietro = risalita, circa 6 pollici di corsa per parte, centrata da una molla;
  - volante: 3,5° di volante per 1° di timone, orario = timone a dritta, con un fermo in folle;
  - in emergenza la colonna esterna comanda i timoni orizzontali di poppa e quella interna i **timoni della vela**
    (il Flight I non ha timoni di prora; arrivano col 688I). C'è anche una barra di emergenza per il timone.
  - Indicatori: **quadranti a sincro** per timone, timoni della vela e di poppa, ognuno con un anello di numeri
    illuminati che legge l'angolo a 5° `[NSTM-561]` (alta). Comandi e strumenti «per lo più analogici» `[HB]`.
- **Ballast Control Panel (BCP).** Il capo della guardia ci comanda l'immersione e l'emersione, e tiene assetto e
  spinta pompando acqua fra le casse di compenso `[NOVA-C]`. Sorveglia anche:
  - tutti i portelli e le aperture dello scafo;
  - alberi e antenne, tranne i periscopi (media).
- **Emersione d'emergenza** (*emergency blow*): l'interruttore è sul BCP e lo usa solo il capo della guardia
  `[SI-637]` (alta). Sopra il BCP stanno le valvole meccaniche del soffiaggio (bassa-media).
- **Allarmi:** «le scatole rosse, gialle e verdi sopra il BCP» `[SI-637]` (alta, 637).
- **Aperture dello scafo («albero di Natale»):** lampade rosse = aperto, verdi = chiuso; «green board» = tutto
  chiuso, si può immergere `[COD]` (alta per i sottomarini della guerra). Sul 688 la stessa funzione è una sezione del
  BCP con la stessa logica (media). Disposizione esatta, sinottico delle casse e forma delle lampade: n.v.
- Il BCP del Flight I aveva un indicatore di quota **a barra**, sostituito sul 688I (bassa-media).

### 2.3 Sonar: AN/BQQ-5

**Versioni** `[FI-BQQ5]` (media-alta):

| Versione | Consolle e calcolatori |
|---|---|
| Iniziale (1974) | sfera BQS-13 e trasmettitore BQS-14; **tre consolle OJ-274**, un banco di diagnosi OH-275, un video IP-1124; un UYK-7 |
| 5A | tre OJ-431, due UYK-7 |
| 5B | formatore di fasci per la schiera di scafo; consolle OJ-462 |
| 5C | DIFAR, tre UYK-44, **quattro consolle OJ-544** |
| 5D | per i 688 senza BSY-1, schiera sottile TB-23 (sviluppo concluso nel 1988) |

- **Schermi a colori** solo col 5E del 688I (sviluppo dal 1993). Nel 1989 un Flight I ha quindi 3 o 4 consolle con
  **CRT monocromatici** (media, deduzione dalla fonte). Il colore del fosforo (verde o ambra) è n.v. Per confronto,
  lo schermo TMA Mk 78 dell'era Mk 113 era «ambra o verde» `[LH-113]` (bassa).
- **La sala** (testimone, 698; bassa):
  - «tre postazioni identiche» e la consolle del capo sonar, con la targhetta IBM;
  - le cascate, varie presentazioni su CRT, il calcolatore UYK-7 («JUNK-7»);
  - **luce blu** soffusa, 5-6 sonaristi.

  La luce blu è confermata da NOVA (media, 688I).
- **Divisione e ruoli** `[TWZ]` (bassa-media): 14 sonaristi, metà di guardia; il capo dietro gli operatori; un
  operatore a banda larga (cascata BTR) e uno a banda stretta (pagine LOFAR).
- **Presentazioni** `[APL]` (alta per le definizioni): BTR = energia a larga banda su 360° contro il tempo; LOFAR =
  energia per frequenza contro il tempo a un rilevamento dato. C'era anche il ricevitore d'intercettazione **WLR-9**
  per sonar attivi e siluri (media).
- Disposizione di una consolle OJ (quanti CRT, palla o tastierino, file di tasti funzione): n.v. STIMA: uno o due
  grandi CRT per consolle, con un ripiano di tasti e interruttori.

### 2.4 Tiro e locale siluri

- **Mk 117.** I primi 688, fino all'SSN-699, avevano il Mk 113 Mod 10. Il **Mk 117**, primo sistema «tutto
  digitale», arriva dal Dallas (SSN-700); diversi dei primi battelli vengono convertiti negli anni '80 `[SR-1993]`.
  Nel 1989 quindi Mk 117 su quasi tutti i Flight I (media).
- Il Mk 117 sposta nell'UYK-7 il lavoro del calcolatore d'attacco analogico Mk 75 e lo affida alle **consolle digitali
  Mk 81** `[WIKI-688]`. Gli operatori del Mk 81 «girano gli encoder» (manopole rotative) per costruire la colonna di
  punti della TMA. Sono sommersi da «centinaia di dati» e preimpostazioni delle armi `[SR-1984]` (media-alta).
- **Il Mk 92 è delle fregate FFG-7, non dei sottomarini: non usarlo.**
- Numero delle consolle Mk 81, colore dei CRT e nomi dei pannelli di lancio: n.v. STIMA: 3-4 consolle in fila sul
  fianco destro, più il Weapons Control Panel col pulsante rosso.
- **Locale siluri** `[NOVA-T]` (media):
  - ponte inferiore del compartimento prodiero, **quattro tubi da 21 pollici** al centro, fino a 22 armi sulle
    rastrelliere;
  - l'arma va sul vassoio di carico e si ispeziona; il **pistone di caricamento** la spinge nel tubo e si chiude la
    culatta; movimentazione idraulica.
  - Nel locale c'è un **pannello di controllo dei tubi**: foto del 1984 sul City of Corpus Christi `[F-TOR]` (alta).
  - Colori: n.v.

### 2.5 ESM e radio

- **AN/WLR-8(V)2** `[FAS-WLR8]` `[FI-WLR8]` (media-alta):
  - ricevitore ESM tattico «per la classe 688», con sette sintonizzatori YIG; 500 MHz-18 GHz;
  - misura direzione, frequenza, modulazione, PRF, durata degli impulsi e ampiezza;
  - una consolle unica con **due tracce d'oscilloscopio** commutabili su qualunque sintonizzatore, per la
    presentazione **panoramica** (PAN) o nel tempo; allarme di minaccia e stampa del registro;
  - non tutti i battelli lo avevano: gli apparati si facevano ruotare.

  Fosforo n.v. (probabilmente monocromatico). Con il WLR-8 c'era l'avvisatore WLR-10.
- **Radio** `[FAS-SCMP]` (media-alta; elenco anni '90 con apparati vecchi):
  - due alberi multifunzione BRA-34;
  - ricetrasmettitori satellitari UHF **WSC-3**;
  - ricevitori HF R-1051 e trasmettitore URT-23D;
  - **telescriventi UGC-136**;
  - antenna a filo galleggiante OE-315 e boe UHF a perdere BRT-6;
  - il ricevitore ELF arriva fra il 1989 e il 1995: forse non ancora a bordo.

  Disposizione della sala radio: n.v.

### 2.6 Colori, luci e caratteri

- **Armadi:** nessuna fonte descrive i colori delle consolle 688. Valgono le norme: gli apparati elettronici della
  Marina hanno due mani di smalto grigio secondo MIL-E-15090, il **grigio chiaro degli apparati FS 26307**; il grigio
  nave FS 26270 è solo per l'esterno `[MIL-16400]` (alta).
- **Comandi** (manopole, maniglie): **nero FS 37038** o **grigio FS 26231/36231** `[MIL-1472]` (alta).
- **Lampade** `[MIL-1472]` (alta):

  | Colore | Significato |
  |---|---|
  | rosso lampeggiante | emergenza |
  | rosso | guasto, «no» |
  | giallo | margine, attenzione |
  | verde | in tolleranza, pronto |
  | bianco | stato neutro o selezione |
  | blu | da evitare |

- **Quadranti** `[MIL-16400]` (alta):
  - quelli illuminati hanno di giorno il fondo scuro e i numeri bianchi, e brillano in rosso con la luce interna
    accesa (filtri rossi oltre 590 nm);
  - quelli non illuminati hanno il fondo bianco, i numeri e l'indice neri.
- **Scritte** `[MIL-1472]` (alta): carattere bastoni standard, maiuscolo, secondo MIL-M-18012. Dove conta l'occhio
  adattato al buio, bianco su fondo scuro; sopra 10 lux, nero su chiaro. Sul grigio FS 26231 si scrive in bianco, sul
  grigio FS 26492 in nero. I pannelli retroilluminati (MIL-P-7788) sono grigio scuro o neri con le lettere bianche che
  si accendono.
- **Nella pratica** le incisioni sono le lettere a tratto unico della pantografatrice («Gorton», Futura Medium) in
  maiuscolo con le abbreviazioni (bassa).

---

## 3. Pr. 705/705K «Lira» (Alfa)

### 3.1 I sistemi automatici [705]

Il battello ha due «cervelli» e un terzo per il moto:
- **«Аккорд»** (Akkord), il combattimento;
- **«Ритм»** (Ritm), le macchine;
- **«Боксит»** (Boksit), moto e quota.

| Sistema | Funzione | Fonte |
|---|---|---|
| **БИУС «Аккорд»** (МВУ-111) | informazione e comando del combattimento (capo progettista А. И. Буртов, poi ЦНИИ «Гранит»). Concentra tutto nel posto centrale; prende sonar, navigazione, periscopio e TV, radar; calcola il moto di 3 bersagli; immette i dati nelle armi per via elettrica | `[SOB]` `[FLOT]` `[BAR]`, alta |
| **«Сарган»** (Sargan) | preparazione dei tubi e comando del lancio (ЦНИИ «Аврора») | `[SOB]`, alta |
| **ГАК «Океан»** (Okean) | complesso sonar automatizzato (ЦНИИ «Морфизприбор»), §3.3 | `[SOB]` `[TTX]`, alta |
| **«Сож»** (Sozh) | navigazione autonoma col **plottatore automatico** (автопрокладчик) | `[SOB]` `[MET]`, alta |
| **«Боксит»** (Boksit) | comando del moto: rotta e quota a programma, automatico e a mano, anche da fermo | `[SOB]` `[PRJ]`, alta |
| **«Тан»** (Tan) | assetto automatico in moto | `[PRJ]`, alta |
| **КСУТС «Ритм»** (705) / **«Ритм-200»** (705K) | reattore, rete elettrica, impianti di bordo. Sottosistemi «Гамма», «Тембр», «Такт-Т» e «Мелодия» (sorveglianza e registro, 226 sensori e 279 segnali) | `[SOB]`, alta |
| **«Молния-705»** / **«Булава-705»** | radio / intercettazione radio | `[SOB]`, alta |
| **«Бухта»** / **«Чибис»** | ESM passivo / radar di navigazione, citati insieme come «Бухта-Чибис» | `[SOB]` `[MET]`, alta |
| **«Сигнал»** | periscopio universale | `[MET]`, alta |
| **ТВ-1** | telecamera verticale nella vela, mostrata sul pulto del comandante | `[MET]` `[KOL]`, alta |
| **«Снегирь»** | registratore inviolabile di tutto quello che accade a bordo; video in tutti i compartimenti | `[ZV]`, media |

- **Nomi non confermati per il 705**, da non usare: «Молибден», «Сигма», «Литий», «Радиан».
- **Equipaggio** [705]:
  - 31-32 uomini, quasi tutti ufficiali, nessun commissario politico `[TTX]` `[BAR]`;
  - **8 uomini di guardia** nel posto centrale; Akkord ne avrebbe richiesti 5 `[BOG]`;
  - compartimenti tecnici senza personale, la ronda solo in prontezza 1 e 2 `[PRJ]`;
  - nessun silurista: niente personale della БЧ-3 `[KOL]`.

### 3.2 Il posto centrale (ГКП) [705]

- **Dove:** ponte superiore del compartimento III, il compartimento rifugio, con le paratie sferiche resistenti
  alla quota massima `[PRJ]` `[FLOT]`. La camera di salvataggio per tutto l'equipaggio è nella vela.
- **Forma: a ferro di cavallo** («в форме подковы»). «Le postazioni e i locali tradizionali mancavano»: niente sala
  radio, sala radar, sala sonar o sala carteggio `[SOB]` (alta).
- **Pulti del 705K** `[PRJ]` (alta):
  - comando (ПК), moto (ПД), armi (ПУО);
  - impianto a vapore (ПЭУ), rete elettrica (ПЭЭС), impianti di bordo (ПОКС);
  - informazione (ПИ), navigatore (ПШ);
  - radio, intercettazione radio.
- **Disposizione**, dalle didascalie delle foto di Собакин (alta). Guardando a prora:
  - **lato sinistro, ship e macchine:** i pulti di **Боксит** (moto) e di **Ритм** (macchine); verso poppa, un
    pulto unico **Молния/Булава** (radio ed ESM) con un operatore;
  - **lato dritto, combattimento:** i pulti di **Аккорд**, **Сарган**, gli schermi della situazione esterna di
    **Океан** (sonar) e **Бухта-Чибис** (radar); verso poppa, il posto del navigatore.
- **Pulto del comandante = pulto di Akkord** (`[SOB]`; Баранов: «Il primo pulto è quello del comandante»). Ha:
  - i comandi di Akkord;
  - uno **schermo TV** con la posizione, la situazione subacquea e sotto i ghiacci, lo stato dei compartimenti e
    l'immagine del periscopio;
  - i comandi a distanza del periscopio;
  - il quadro delle radiazioni;
  - gli indicatori di sbandamento, assetto e velocità.
- **Allarmi:** **finestrelle rosse** luminose, per esempio «Сбой ДВУМ» che lampeggia sul pulto del comandante
  `[BAR]`, o il «quadro rosso» «Temperatura oltre 60°» sul pulto degli impianti `[BOG]` (alta).
- **Governo.**
  - Le memorie parlano di «guardia ai timoni» e di un timoniere nostromo `[FLOT]`. Богатырёв dice che quota e rotta
    si tenevano per lo più «dal pulto del moto» (alta).
  - **Due joystick** per un solo timoniere, commutabili su uno: lo dice solo Sutyagin `[SUT]` (media-bassa).
  - **Casse:** una «stazione a pulsanti» accanto al timoniere, tenuta da un ingegnere `[SUT]` (media). Corrisponde
    al pulto ПОКС di Ритм. **Nessuna parete di valvole a volantino** all'americana.
- **Periscopio:** universale «Сигнал». Ci guardavano solo il comandante, il secondo e l'ufficiale di guardia; «uno
  sguardo al periscopio» era il premio delle gare fra le guardie `[KOL]` (alta). Se si guastava, restava il solo sonar.
- **Schermi**, dalle foto (media):
  - **CRT tondi sotto cappe coniche di gomma nera**;
  - cappe quadrate nere;
  - schermi TV rettangolari grigi sul pulto del comandante;
  - un tavolo di plottaggio orizzontale in fondo alla fila del combattimento (Sutyagin).
- **Sinottici:** già i sistemi «Вольфрам» del Pr. 671 avevano sostituito i sinottici luminescenti con **sinottici
  illuminati da lampade di segnalazione** `[SOB]`. Probabile, non provato, che Ритм facesse lo stesso.
- **Conflitto.** Sutyagin descrive una «sala sonar» con tre consolle (attivo, intercettazione, passivo). Собакин, dal
  lato dei progettisti, dice che le sale non c'erano e che gli schermi del sonar stanno nel posto centrale. **Si segue
  Собакин.**

### 3.3 Sonar: «Океан» [705]

- **Composizione** `[TTX]` (alta):
  - stazione passiva di scoperta **«Енисей»**, citata come МГК-1001 (media) o «MGK-100» da Sutyagin; en.wikipedia la
    chiama a torto «ricevitore d'intercettazione»;
  - velocimetro «Жгут», telefono subacqueo «Роса»;
  - stazione antimine **«Луч»**, poi «Винт» e «Тисса»;
  - Меттер aggiunge il solcometro acustico «Мечта» e l'ecoscandaglio per i ghiacci «Север».
- **Nella pratica** (alta):
  - la potenza era alta sulla carta, ma il rumore proprio limitava la portata passiva a quella di «Рубин»
    (Pr. 671);
  - 671RTM e Los Angeles scoprivano più lontano `[KOL]`;
  - **nessun registratore** per fissare i bersagli;
  - modi attivi ИД, УП, ДП, СП, БП `[BAR]`;
  - con l'integrazione del segnale «le tracce del bersaglio sullo schermo ci sono» anche quando non si sente niente
    `[BOG]`.
- Gli schermi sono nel posto centrale, lato dritto, sotto le cappe nere. **Colore del fosforo e forma della
  presentazione** (circolare tipo PPI o a cascata): **n.v.**

### 3.4 Armi e ricarica [705]

- **Tubi:** 6 da 533 mm **pneumoidraulici**, i primi in URSS. Lanciano con l'acqua invece che con l'aria, a qualunque
  quota fino alla massima e ad alta velocità `[TTX]` `[FLOT]` (alta).
- **Carico:** 18-20 siluri (SAET-60A, SET-65), o «Шквал», o il razzo «Вьюга», o 24 (o 36) mine (alta).
- **Ricarica rapida (УБЗ)** `[KOL]` (alta):
  - «il 1° compartimento è completamente vuoto, eppure si ricaricano i tubi»;
  - «УБЗ, immissione elettrica, nessun uomo della БЧ-3: per me era tutto nuovo»;
  - con i siluri d'esercitazione in rastrelliera serviva qualcuno al posto locale del compartimento.

  Sutyagin descrive il meccanismo, coppie di rastrelliere idrauliche che traslano davanti ai tubi più un pistone, e
  un silurista di guardia (media). Contraddice Коляда.
- **Lancio** `[KOL]` (alta): l'aiutante alle armi, «seduto nel posto centrale al suo pulto "Сарган", accendendo e
  premendo pulsanti, può comandare tutte le armi».
  - I dati passavano solo per via elettrica: se Akkord si guastava, **nessuna arma era utilizzabile** `[BAR]`.
  - Akkord preparava i dati per 3 bersagli in sequenza rapida.
- **Lampade di stato per tubo:** n.v. Ragionevole una fila per tubo (pronto, porta aperta, lanciato) [GEN].

### 3.5 Interni, colori e caratteri

- **Dalle foto a colori di un posto centrale del tipo 705** `[WRK]`. La didascalia dice: la sezione prodiera della
  K-64 usata per l'addestramento, con divise successive al 1992. Aspetto media, identificazione bassa-media.
  - pannelli in smalto **crema o avorio**, non grigio-verde;
  - banchi inclinati con i pannelli verticali sopra;
  - file di **piccole lampade tonde colorate** (rosse, ambra, verdi);
  - interruttori a leva e manopole **neri** in bachelite, quadranti tondi **a fondo bianco**;
  - **targhette rosse con le scritte incise in bianco** in cima ai pannelli;
  - cordoni del telefono neri a spirale che pendono dal cielo del locale;
  - poltroncine girevoli a guscio **bianco-crema** con i cuscini **marrone scuro**, su colonna;
  - pavimento blu-grigio scuro.
- **[GEN] Sovietici degli anni '70-'80:**
  - B-396 (Pr. 641B, 1980, museo di Mosca) ha la stessa livrea: crema e avorio, quadranti bianchi, manopole nere,
    scatole grigio-azzurre, cartelli gialli «ВНИМАНИЕ!» con le lettere rosse, numeri dell'ordinata su targhette blu;
    il pulto del sonar è **verde giallino pallido** con un grande CRT tondo;
  - B-413 (Pr. 641) ha i pannelli avorio e i **volantini rossi** delle valvole. Il posto centrale del 705 non ha
    pareti di volantini: è un contrasto utile.
- **Caratteri:** il carattere da disegno sovietico è il ГОСТ 2.304, stretto e spesso inclinato. Che sia quello delle
  targhette del 705: n.v. (bassa). Il requisito 12 chiede caratteri **latini** di stile sovietico (squadrati, a
  stampino), senza cirillico illeggibile.

---

## 4. Che cosa c'è e che cosa manca

| | 688 Flight I | Alfa (Pr. 705) |
|---|---|---|
| Locali | centrale (attack center) + **sala sonar** + sala radio + **manovra** delle macchine a poppa + locale siluri | **un solo posto centrale a ferro di cavallo** con sonar, radio, ESM, radar, navigazione, reattore, rete, impianti |
| Governo | **2 colonne a cloche col volante**, ufficiale d'immersione, 4 uomini | pulto «Боксит»: **rotta e quota a programma**, assetto automatico «Тан»; uno o due joystick (n.v.) |
| Casse | **BCP** del capo della guardia, aperture dello scafo rosso/verde, soffiaggio d'emergenza | **pulsanti** sul pulto degli impianti (ПОКС), niente volantini |
| Sonar | 3-4 consolle OJ, CRT monocromatici, capo sonar dietro, luce blu, BTR e LOFAR | schermi di «Океан» sulla fila di Akkord, CRT tondi sotto cappe nere; portata minore |
| Tiro | fila di consolle Mk 81 con gli encoder (TMA a mano), Weapons Control Panel col pulsante rosso | Akkord calcola 3 bersagli; **pulsanti del pulto «Сарган»**; senza Akkord niente armi |
| Siluri | 4 tubi, siluristi, vassoio e pistone, culatte a mano | 6 tubi pneumoidraulici, **ricarica automatica УБЗ**, compartimento vuoto |
| Periscopi | **due** (Type 2 d'attacco e Type 18 di ricerca con TV ed ESM) | **uno** («Сигнал») più la **telecamera TV-1** sul pulto del comandante |
| Macchine | locale manovra a poppa, con personale | pulti di «Ритм» nel posto centrale, compartimenti senza personale, video |
| Equipaggio | ~13 ufficiali e ~115 sottufficiali e marinai | ~31, quasi tutti ufficiali; 8 di guardia |
| Allarmi | scatole rosse, gialle e verdi sopra il BCP | **finestrelle rosse** luminose con la scritta |
| Colori | grigio chiaro degli apparati, inserti grigio scuro o neri, manopole nere, scritte bianche | crema e avorio, lampade tonde colorate, bachelite nera, targhette rosse |

---

## 5. Come renderle a 640×480 nello stile VGA del gioco [GIOCO]

### 5.1 Principi

- **Si differenzia la grafica, non l'interfaccia.** Le stesse otto stazioni, con gli stessi tasti e le stesse aree del
  mouse dove hanno lo stesso significato. Cambiano:
  - **forma** degli strumenti: quadranti a sincro e colonne contro pulsanti e lampade;
  - **disposizione:** fila di consolle e sale separate contro ferro di cavallo;
  - **presenza:** comandi che su un battello non esistono non compaiono. Sull'Alfa niente volantini del BCP né
    culatte a mano; sul 688 niente programma di quota.
- **Un tema per battello:** palette delle consolle, carattere delle scritte, forma delle lampade e degli schermi,
  cornici, rumori di fondo dei pulsanti (per l'audio, dossier 18).
- **Le funzioni restano quelle del motore.** Dove l'Alfa automatizza (quota a programma, assetto, ricarica), la
  stazione mostra l'automatismo che lavora e il giocatore lo imposta. Dove il 688 è manuale, il giocatore muove i
  comandi.

### 5.2 Palette

**I pannelli in basso** (sotto il periscopio) condividono la palette con la vista. Hanno a disposizione:
- gli indici che la luce non tocca: interfaccia 0-15 e 80-95, fuoco 175-178, 183-188 (toni carne e ambra), 189, 191,
  195, 254;
- gli indici **160-174**, che oggi il codice non usa (da verificare prima di prenderli). Possono ospitare **una rampa
  del tema** di 15 posti, ridefinita alla scelta del battello:

| Tema | Rampa di 15 posti (STIMA RGB) |
|---|---|
| 688 | grigio degli apparati, dal nero dei bordi al grigio chiaro FS 26307 (circa 135, 140, 140), più due grigio-azzurri per i cristalli dei CRT |
| Alfa | crema e avorio (circa 225, 215, 180) verso il marrone dei cuscini e il blu-grigio del pavimento (circa 60, 70, 85) |

Lampade: rosso, ambra e verde EGA (12, 14 e 10; 4, 6 e 2 spente), che di notte non si spengono.

**Le stazioni a tutto schermo** (sonar, radio, carta, sala siluri, danni) ridefiniscono già 96-143 e 199-247. Il tema
vi aggiunge le sue rampe:

| Elemento | 688 | Alfa |
|---|---|---|
| Fosforo dei CRT | **verde** (n.v., STIMA) | **verde giallino** (scelta dell'utente, 25-10-2026; dato storico n.v.) |
| Smalto delle consolle | grigio | crema |
| Lampade | in tre colori | in tre colori |
| Targhette | | rosse |

**La luce della sala** (bianca, rossa per la notte, blu in sala sonar) si fa come la luce della vista:
`Palette::illuminata` sulla rampa del tema. **[GIOCO]** Sul 688 il «rig for red» la sera e al periscopio notturno, la
luce blu nella sala sonar. Sull'Alfa le luci restano bianche: è un locale unico.

### 5.3 Caratteri

I dettagli, con le norme, le foto e le misure in pixel dei font bitmap da disegnare, sono al **§9**.


- **688:** bastoni a tratto unico, maiuscolo, larghezza uniforme, alla «Gorton» delle incisioni MIL-M-18012.
  - Bianco su grigio scuro per le targhette dei pannelli; nero su bianco per i cartelli d'avviso.
  - Sigle all'americana: «STBD», «FWD», «BLOW», «VENT», «HULL OPEN».
  - Si può derivare dal font originale (`data/motore/font/originale.fnt`) assottigliandolo, oppure disegnare un
    5×7 incisi.
- **Alfa:** latino squadrato a stampino (requisito 12), stretto e verticale come il ГОСТ, con i ponti delle lettere
  aperti.
  - Bianco su **targhetta rossa** per i nomi dei pulti; nero su crema per le scritte dei comandi.
  - Sigle dei sistemi traslitterate: «AKKORD», «SARGAN», «OKEAN», «BOKSIT», «RITM», «MOLNIJA».
  - Comandi in italiano o inglese per leggibilità, con la sigla sovietica piccola accanto
    (`PROGRAMMA QUOTA / BOKSIT`).

### 5.4 Strumenti e forme ricorrenti

| Elemento | 688 | Alfa |
|---|---|---|
| Quadranti | tondi a sincro, fondo scuro e numeri bianchi (rossi di notte), anello di numeri illuminati | tondi a fondo bianco, indice nero, cornice di bachelite |
| Lampade | rettangolari a lente, ammassate nei sinottici del BCP | piccole tonde in file orizzontali; **finestrelle rosse** con la scritta per gli allarmi |
| Schermi | rettangolari, cornice grigia, cappuccio | **tondi sotto la cappa conica nera**; TV rettangolare grigia sul pulto del comandante |
| Comandi | manopole a encoder, leve, pulsanti coperti (il rosso di fuoco), colonne a cloche | pulsanti quadrati illuminati, interruttori a leva neri, joystick |

---

## 6. Proposta per stazione [GIOCO]

Coordinate a 640×480. «Vista» = le 360 righe del periscopio o della sala. «Pannello» = le 120 righe in basso. Gli
schizzi sono d'impaginazione: misure indicative.

### 6.1 Centrale (F9)

- **688:** la **pedana dei periscopi** vista dall'ufficiale di guardia. Nella vista, la sala disegnata in prospettiva
  semplice: a sinistra il governo coi due sedili e il BCP curvo, a destra la fila delle consolle di tiro, al centro i
  due periscopi. Ogni postazione è un'area cliccabile che apre la sua stazione, come il menu dell'originale. Nel
  pannello, lo stato rapido.
- **Alfa:** il **ferro di cavallo** visto dall'alto in pianta, o in prospettiva dal pulto del comandante:
  - a sinistra i pulti di Боксит, Ритм e Молния;
  - a destra Аккорд, Сарган, gli schermi di Океан e il posto del navigatore;
  - al centro il periscopio «Сигнал» e lo schermo TV del comandante.

  Un'area per pulto. Gli 8 uomini di guardia sono figurine sedute, coi gusci crema e i cuscini marrone.

```
688 (vista 640x360)                                  ALFA (vista 640x360)
+------------------------------------------------+   +------------------------------------------------+
| [GOVERNO]  2 sedili   |  O O  periscopi  | FILA |   |  [BOKSIT][RITM][RITM]  ( )  [AKKORD][SARGAN] |
| [BCP curvo, allarmi]  |  pedana          | MK81 |   |  [MOLNIJA]   periscopio   [OKEAN][NAVIGAT.] |
| [ ESM ]  [NAV/plot ]  |                  | WCP  |   |  ---- ferro di cavallo, TV comandante ----  |
+------------------------------------------------+   +------------------------------------------------+
```

### 6.2 Manovra (F4)

- **688:** il **pannello di governo** `[NSTM-561]`:
  - tre quadranti a sincro (timone, timoni della vela, timoni di poppa) con l'anello illuminato;
  - quota a barra, velocità, giri;
  - telegrafo, che resta comando del giocatore;
  - a destra il **BCP**: sinottico delle casse, **«albero di Natale»** delle aperture (verde = chiuso, rosso =
    aperto), casse di compenso con le pompe, l'interruttore del **soffiaggio d'emergenza** sotto il coperchio, le
    tre scatole d'allarme.

  Il giocatore muove timone e quota coi tasti di oggi: le colonne si inclinano sullo schermo.
- **Alfa:** il **pulto di Боксит**:
  - modi PROGRAMMA, AUTOMATICO e MANUALE per rotta e quota;
  - la quota e la rotta impostate su contatori a rulli;
  - indicatori di sbandamento, assetto e velocità sul pulto del comandante;
  - l'assetto automatico «Тан» con una lampada ATTIVO;
  - **le casse come pulsanti** illuminati (ПОКС): SOFFIA, SFOGA, COMPENSO, con la finestrella rossa per i guasti;
  - un joystick per il manuale.

  **Nessun volantino, nessun BCP.**

### 6.3 Periscopio (F5)

- **688:** **due periscopi** con un selettore: Type 2 d'attacco, stretto e solo diurno; Type 18 di ricerca, con
  l'ingrandimento doppio, la luce bassa, la TV e l'ESM nella testa. Le funzioni sono già nel motore (stadimetria,
  notte, ESM, S2). La cornice cambia col modello e il Type 18 ha le maniglie con i comandi.
- **Alfa:** un periscopio, **«Сигнал»**, più la **telecamera TV-1** come vista alternativa in bianco e nero
  retinata, sul pulto del comandante. Pulsante «PERISKOP / TV». La vista TV è a grana grossa e senza colore, ma si
  usa sotto la quota periscopica di qualche metro (STIMA da tarare).

### 6.4 Plottaggio (F6)

- **688:** il **tavolo di carteggio** a matita, oggi la carta F6 nello stile «foglio» o «nautica». Fra gli strumenti
  ci sono le consolle Mk 81 con la colonna di punti della TMA (MATE). È il lavoro a mano del gruppo di tiro.
- **Alfa:** il **plottatore automatico «Сож»**: una carta su tavolo orizzontale con il **pennino che traccia da
  solo** la rotta propria (animazione del carrello). I bersagli vengono da Akkord (3 alla volta). Stile «carta»
  sovietica (carta d'epoca URSS), cornice crema.

### 6.5 Armi (F7)

- **688:** due livelli:
  - in centrale le **consolle Mk 81**: encoder rotativi per le impostazioni (rotta, quota, abilitazione, filo), il
    **Weapons Control Panel** con i quattro tubi, le lampade per tubo e il **pulsante rosso coperto**;
  - un tasto apre il **locale siluri**: la sezione dei tubi delle sagome, con la culatta a mano, il vassoio, il
    pistone di carico e i siluristi.
- **Alfa:** il **pulto «Сарган»**:
  - **sei file di pulsanti illuminati**, una per tubo (PRONTO, ALLAGATO, PORTA, FUOCO);
  - la **finestrella rossa «AKKORD GUASTO»**: un guasto di Akkord **blocca tutte le armi**, come era davvero `[BAR]`
    (scelta dell'utente, 25-10-2026);
  - lo schema della **ricarica automatica УБЗ**: il compartimento I vuoto, con le rastrelliere che traslano e le
    armi che entrano da sole. Si usa la sezione del tubo delle sagome, senza figurine.
  - L'impostazione dei 3 bersagli viene da Akkord e il giocatore ne sceglie uno.

```
688 ARMI (pannello)                                   ALFA ARMI (pannello)
+------------------------------------------------+   +------------------------------------------------+
| (o)(o)(o) encoder  | [1][2][3][4] lampade tubo  |   | SARGAN  1 [P][A][D][F]  2 [P][A][D][F] ...   |
| RTA  QUOTA  ABIL.  | [FIRE] coperto, rosso      |   | AKKORD: BERS. 1 2 3   [##] finestrella rossa  |
+------------------------------------------------+   +------------------------------------------------+
```

### 6.6 Sonar (F8)

- **688:** la **sala sonar**:
  - luce blu;
  - tre consolle OJ con i CRT monocromatici verdi (STIMA): cascata BTR, LOFAR e attivo, come le cascate S5a nel
    modo «1989»;
  - la consolle del capo sonar;
  - una riga di interruttori e tasti funzione sotto ogni schermo;
  - il ricevitore WLR-9 dei siluri come lampada e allarme.
- **Alfa:** gli **schermi di «Океан»** sulla fila di Akkord: **CRT tondi sotto le cappe nere**, presentazione n.v.
  Proposta: lo schermo tondo mostra un settore a ventaglio di rilevamenti (Енисей), con a fianco un piccolo schermo
  rettangolare a cascata (modo «1989» sovietico, fosforo da scegliere).
  - **Nessun registratore** `[BOG]`: gli slot del sonar ci sono **solo nel modo moderno**; nel modo «1989»
    dell'Alfa mancano (scelta dell'utente, 25-10-2026).
  - L'integrazione del segnale mostra le «tracce senza suono».
  - La stazione antimine «Луч» ha una lampada.
  - **Portata minore** del 688: è già nei dati.

### 6.7 Stato e danni (F3)

- **688:** il **quadro dei danni** di oggi (spaccato), più il **pannello delle aperture e degli allagamenti**
  (Flood Control Gauge Board del 637 `[SI-637]`) e le scatole d'allarme. Il controllo danni è fatto da squadre di
  uomini, già nel kit.
- **Alfa:** il **pulto di «Ритм»**:
  - **sinottici illuminati da lampade** (reattore «Гамма», rete «Тембр», impianti «Такт-Т»);
  - quadranti a fondo bianco;
  - le **finestrelle rosse** degli allarmi («TEMPERATURA OLTRE 60°» `[BOG]`);
  - lo **schermo video dei compartimenti senza personale**: lo spaccato in bianco e nero retinato, compartimento per
    compartimento.

  Le squadre di controllo danni sono poche (ufficiali), e i compartimenti si sorvegliano da lontano.

### 6.8 Radio ed ESM (F2)

- **688:** due posti.
  - La **sala radio** con i rack: WSC-3, R-1051, URT-23D, la **telescrivente UGC-136** che batte i messaggi (già
    nella stazione radio S4) e la boa BRT-6.
  - La **consolle ESM WLR-8** con le **due tracce d'oscilloscopio**: panoramica in frequenza e nel tempo, allarme di
    minaccia, stampa del registro.
- **Alfa:** **un solo pulto «Молния/Булава»**, con un operatore, sul lato sinistro del posto centrale:
  - radio e intercettazione insieme, la sezione «Бухта» per l'ESM;
  - pulsanti illuminati e un piccolo CRT tondo per l'ESM;
  - i messaggi su una striscia di telescrivente sovietica, con la carta grigia e i caratteri a stampino.

### 6.9 Riepilogo dei comandi presenti e assenti

| Comando | 688 | Alfa |
|---|---|---|
| Colonne di governo e telegrafo | sì | no: modi di Боксит e joystick |
| Quota e rotta a programma | no (autopilota semplice) | **sì** |
| Assetto automatico | no | **sì** («Тан») |
| BCP, albero di Natale, soffiaggio coperto | **sì** | no: pulsanti ПОКС |
| Due periscopi | **sì** | no: uno più la TV-1 |
| Locale siluri a mano | **sì** | no: ricarica automatica УБЗ |
| Armi senza il calcolatore | sì (Mk 81 manuale) | **no**: un guasto di Akkord blocca tutte le armi (deciso) |
| Sala sonar con registratori | sì | no: slot del sonar solo nel modo moderno (deciso) |
| Macchine dal posto centrale | no | **sì** (Ритм) |
| Video dei compartimenti | no | **sì** |

---

## 7. Fonti

**688.**
- `[NSTM-561]` NAVSEA, NSTM S9086-S9-STM-010 cap. 561, *Submarine Steering and Diving*:
  https://navytribe.com/wp-content/uploads/2015/11/navsea-s9086-s9-stm-010_ch-561.pdf
- `[MIL-16400]` MIL-E-16400H (1987): https://www.atmmicrowave.com/wp-content/uploads/mil-e-16400-rev-h.pdf
- `[MIL-1472]` MIL-STD-1472F, *Human Engineering*:
  https://www.denix.osd.mil/soh/denix-files/sites/21/2016/03/02_MIL-STD-1472F-Human-Engineering.pdf
- `[MIL-15090]` MIL-DTL-15090D: https://quicksearch.dla.mil/Transient/8B53A0D49ED04B6C9B09B53578DA8957.pdf
- `[FI-BQQ5]` Forecast International, AN/BQQ-5(V): https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641
- `[FI-WLR8]` Forecast International, AN/WLR-8(V): https://www.forecastinternational.com/archive/disp_old_pdf.cfm?ARC_ID=753
- `[FAS-688]` https://man.fas.org/dod-101/sys/ship/ssn-688.htm; BQQ-5 https://man.fas.org/dod-101/sys/ship/weaps/an-bqq-5.htm
- `[FAS-WLR8]` https://irp.fas.org/program/collect/an-wlr-8.htm
- `[FAS-SCMP]` https://man.fas.org/dod-101/navy/docs/scmp/part06.htm
- `[SR-1984]` Submarine Review 1984, *SSN Fire Control*:
  https://archive.navalsubleague.org/1984/ssn-fire-control-the-need-por-systematic-training
- `[SR-1993]` Submarine Review 1993, *combat control systems*:
  https://archive.navalsubleague.org/1993/a-brief-history-of-the-push-pull-of-submarine-combat-control-systems
- `[SR-2002]` Submarine Review 2002, *periscopes*:
  https://archive.navalsubleague.org/2002/looking-around-a-short-history-of-submarine-periscopes-part-ii
- `[SR-2004]` Submarine Review 2004, *Virginia CCC*:
  https://archive.navalsubleague.org/2004/virginias-command-and-control-center-new-concepts-for-a-new-submarine
- `[NOVA-C]` `[NOVA-S]` `[NOVA-T]` PBS NOVA, *Submarines, Secrets and Spies* (USS Springfield, 688I):
  https://www.pbs.org/wgbh/nova/subsecrets/spriconhiright.html, …/sprisonhiright.html, …/spritorhiright.html
- `[CLANCY]` T. Clancy, *Submarine* (1993, USS Miami 688I)
- `[SI-637]` Smithsonian, *Fast Attacks and Boomers*:
  https://americanhistory.si.edu/subs/operating/attackcenter/index.html, controlstation/
- `[APL]` JHU APL Technical Digest 19-4: https://secwww.jhuapl.edu/techdigest/content/techdigest/pdf/V19-N04/19-04-South.pdf
- `[WIKI-688]` https://en.wikipedia.org/wiki/Los_Angeles-class_submarine
- `[COD]` https://usscod.org/diving.html; https://maritime.org/doc/fleetsub/chap18.php
- Fonti basse:
  - `[NE]` https://naval-encyclopedia.com/cold-war/us/los-angeles-class.php
  - `[LH-113]` https://larryshomeport.com/boats/collections/ftg/mk113/
  - testimone del 698: https://bremertonreunion.net/2023/05/04/698-had-the-oldest/
  - `[TWZ]` https://www.twz.com/35603/
  - `[HB]` https://www.hawaiibusiness.com/my-job-submarine-pilot/
- **Foto di pubblico dominio** (Commons):
  - `File:LA_sub1.jpg`: stazione d'immersione di un Los Angeles, 1981, DN-ST-88-07345; la migliore del governo e del
    BCP dell'epoca;
  - `[F-TOR]` «A crewman monitors the torpedo tube control panel … USS CITY OF CORPUS CHRISTI (SSN 705)», 1984;
  - `File:USS_PARGO_Sturgeon_Class.jpg`: centrale di uno Sturgeon, 1991, col capo della guardia al BCP;
  - `File:Skipjack_control_room.svg`: schema;
  - le categorie «Submarine control rooms» e «Submarine periscopes, control room views» hanno foto di 688
    ammodernati (2004-2011).

**Alfa.**
- `[SOB]` В. А. Собакин, automazione del Pr. 705, *Тайфун* 2001: https://tech.wikireading.ru/5514. Le figure 1 e
  36-42 sono le foto e il disegno del posto centrale, con diritti, solo da consultare.
- `[TTX]` tabelle tattico-tecniche: https://tech.wikireading.ru/5524, /5525; descrizione: /5511
- `[PRJ]` descrizione di progetto del 705K: https://tech.wikireading.ru/5523
- `[BOG]` Богатырёв, K-123: https://tech.wikireading.ru/5519
- `[KOL]` Коляда, K-493: https://tech.wikireading.ru/5520
- `[BAR]` Баранов, K-432: https://tech.wikireading.ru/5521
- `[MET]` Меттер, БИУС «Аккорд»: https://memoclub.ru/2013/09/3-bius-akkord-dlya-pl-proektov-705-lira-nato-alfa/
- `[FLOT]` https://flot.com/blog/historyofNVMU/487.php
- `[ZV]` TV Звезда, Баранов (2021): https://tvzvezda.ru/news/20211219931-7QtEJ.html
- `[SUT]` I. Sutyagin, *Alfa Class: The 1960s Dream Machine*, Submarine Review 1998:
  https://archive.navalsubleague.org/1998/alfa-class-the-1960s-dream-machine-dr-sutyagin
- `[WRK]` foto a colori, forum wrk.ru (licenza ignota, solo da consultare):
  http://www.wrk.ru/forums/attachment.php?item=540897, …=540898
- ru.wikipedia: https://ru.wikipedia.org/wiki/Подводные_лодки_проекта_705(К)_«Лира»
- Altre: techinsider https://www.techinsider.ru/weapon/16141-submarina-istrebitel-proekta-705/; snariad
  https://snariad.ru/submarin/submarin_705/; ot-a-do-ya https://ot-a-do-ya.org/Enc/RI-SSSR-RF/Warships/PLA705.aspx
- **Foto libere di battelli comparabili [GEN]:**
  - B-396 (Pr. 641B), CC BY-SA 4.0: «…Submarine B-396 …, central post P4172401_3180.jpg», «…P4172402_3165.jpg»,
    «…ballast tank control station P4172411_3145.jpg»;
  - «Interior of B-396 Russian museum submarine 10.jpg»: il pulto del sonar verde pallido col CRT tondo;
  - categoria «Interiors of B-413 (submarine, 1968)».

  Se servissero come riferimento d'aspetto da ridisegnare (non da proiettare), vanno tenute nei crediti come
  consultate.

---

## 8. Lacune

**Decise dall'utente il 25-10-2026** (non più lacune):
- fosforo degli schermi sovietici **verde giallino**;
- sull'Alfa gli slot del sonar ci sono **solo nel modo moderno**;
- un guasto di **Akkord blocca tutte le armi**.

**Ancora aperte:**

- **688:**
  - numero e forma delle consolle Mk 81 e del Weapons Control Panel;
  - colore del fosforo di BQQ-5, Mk 81 e WLR-8;
  - disposizione esatta del BCP;
  - quale periscopio è il «numero 1»;
  - lato della sala sonar, posizione della sala radio, dell'ESM e dei tavoli di carteggio.
- **Alfa:**
  - formato degli schermi di «Океан» e di Akkord (il colore è deciso: verde giallino, §5.2);
  - lampade per tubo del pulto «Сарган»;
  - posti a sedere esatti;
  - joystick (solo Sutyagin);
  - caratteri delle targhette;
  - le foto del posto centrale di Собакин e di wrk.ru hanno diritti: si consultano, non si usano.
- Da fare se si vuole più certezza:
  - le pagine dell'esposizione Smithsonian e del Naval Undersea Museum (centrale ricostruita del Greenling) quando
    tornano raggiungibili;
  - deepstorm.ru con la codifica giusta;
  - il libro di Clancy per il 688I (solo come analogo).

---

## 9. Caratteri, schermi e numeri (aggiunta del 25-10-2026)

Richiesta dell'utente: la tipografia delle postazioni, dalle foto vere, e una proposta di font bitmap da disegnare **da
zero**, senza copiare font digitali protetti. Ricerca a parte; le sigle delle fonti sono qui sotto.

### 9.1 Schermi del 688

- **BQQ-5:** monocromatico nel 1989. I colori arrivano con la OJ-544 del 688I negli anni '90 `[FI-BQQ5]` (media,
  deduzione). La foto **«Trident digital sonar system»** (US Navy 1975, pubblico dominio) mostra lo schema della
  famiglia BQQ-5/6: due CRT sovrapposti con le cascate e accanto una **matrice di pulsanti quadrati a funzione
  variabile**. La «Greeneville Sonar Room» (688I, più tarda) ha lo stesso schema: due CRT per consolle, tastierino a
  matrice, spie e pannelli grigio chiaro.
- **Fosfori.** Dalla tabella JEDEC `[PHOS]`:

  | Fosforo | Colore | Uso tipico |
  |---|---|---|
  | P1 | verde, 525 nm | monitor, oscilloscopi |
  | P31 | verde-giallo, persistenza corta | terminali |
  | P39 | verde, persistenza lunga | |
  | P3 | ambra | |
  | P7 | blu con coda gialla | PPI radar |
  | P12, P19 | arancio | radar |

  Il fosforo delle OJ-274/431/462 è n.v. Un verde (P31 o P39) è plausibile. Il TMA Mk 78 dell'era Mk 113 era «ambra
  o verde» `[LH-113]`.
- **Caratteri sugli schermi.**
  - MIL-STD-1472F `[MIL-1472]` (alta):
    - matrice di punti **minimo 5×7, preferita 7×9**;
    - matrici e segmenti **solo maiuscoli**;
    - altezza ≥ 16′ d'arco (20′ preferiti);
    - tratto fra 1/12 e 1/6 dell'altezza;
    - bastoni, con l/1, Z/2 e O/0 distinguibili;
    - colori preferiti verde > giallo > arancio > rosso, niente blu.
  - Le consolle tattiche della Marina dell'epoca (NTDS, OJ-194 dell'AN/UYA-4 con 6 tasti fissi e 18 a funzione
    variabile) disegnavano i simboli **a tratti vettoriali** (media; per i modelli precisi n.v.).
  - I **caratteri Hershey**, vettoriali a tratti dritti per CRT, nascono nel 1967 al Naval Weapons Laboratory di
    Dahlgren `[HERSHEY]`: sono il riferimento estetico dello «stile Marina a tratti».
- **Telescrivente UGC-136:** stampa a matrice di punti, ~120 caratteri al secondo. In Baudot (ITA2) **solo
  maiuscole** `[UGC]`. La matrice è n.v.

### 9.2 Scritte e numeri sui pannelli del 688

- **MIL-M-18012B** `[MIL-18012]` (alta, in vigore fino al 1989):
  - **tipo I** bianco su fondo **nero**, **tipo II** bianco su fondo **grigio**;
  - incisione in **Gorton Extended, Normal o Condensed**; stampa in Futura Demibold/Medium;
  - un tratto fra le lettere, una lettera fra le parole, niente punteggiatura né cornici;
  - la legenda del pannello in alto e più grande;
  - lettere larghe 0,6-1,0 h, numeri 0,4-0,7 h, l'«1» di un solo tratto, tratto ≈ h/6-h/7; altezza 3,2-3,6 mm;
  - comandi d'azione immediata a **strisce gialle-arancio e nere a 45°**.
- MIL-STD-1472F §5.5.5 `[MIL-1472]`: etichette tutte maiuscole; tratto **bianco su scuro h/7-h/8**, retroilluminate
  h/10.
- **Nelle foto di pubblico dominio:**
  - «Defense.gov News Photo 971129-N-5745B-001» (USS Annapolis, 1997): BCP grigio chiaro con **targhette nere a
    scritta bianca** in maiuscole a tratto unico («HYDRAULIC PRESSURE»), un **sinottico a linee nere**, pulsanti con
    la legenda retroilluminata e una finestrella rossa che sembra un **display a LED rossi** (n.v.);
  - «US Navy 040901-N-1348L-005» (USS Toledo, 2004): quadranti analogici sopra il governo e targhette rosse e
    bianche sul BCP.
- **Indicatori numerici:** i Nixie sono superati da VFD e LED a metà anni '70 `[CHM]`. Sui 688 degli anni '80 sono
  plausibili **LED rossi a 7 segmenti**, lampade e **contatori meccanici a tamburo**; il tipo esatto sul pannello di
  governo è n.v. MIL-STD-1472F: 7 segmenti solo per i numeri; LED rossi non vicino alle spie rosse d'allarme.

### 9.3 Scritte e numeri sovietici

- **Foto dei musei** (Commons, CC BY-SA: riferimenti consultati, da ridisegnare e non da copiare):
  - B-396 (Pr. 641B, 1980): «Tango B-396 Sonar Station», «Tango B-396 Central Station 1» (governo «Шпат»),
    «…ballast tank control station P4172411_3145»;
  - B-39 (Pr. 641): «B-39 sonar room 1», «B-39 Leningrad fire control system 1» (attenzione: il museo ha aggiunto
    etichette in inglese);
  - categorie B-307 e B-413.

  Cosa mostrano:
  - consolle **avorio o crema**;
  - **targhette nere rettangolari con le scritte bianche incise**, maiuscole diritte a bastoni, anche su più righe
    («КОНТРОЛЬ», «ПУТЬ ТОРПЕДЫ»);
  - sigle dei blocchi su targhette nere («БК22», «ИЗ-21»);
  - **quadranti a fondo bianco** con le scritte nere («ГЛУБИНОМЕР»);
  - **CRT tondo** del sonar;
  - codici di colore degli impianti: targa **blu con scritta bianca «ВВД»** (aria ad alta pressione), volantini blu
    e rossi;
  - stampini a vernice sulle casse.
  - **Nessun display numerico elettronico: tutto analogico.**
- **ГОСТ 2.304-81** `[GOST-2304]` (alta), carattere da disegno:
  - tipo А (tratto h/14) e tipo Б (**tratto h/10**), diritto o **inclinato di 75°**;
  - comprende **anche l'alfabeto latino**;
  - tipo Б: spazio fra le lettere 2/10 h, fra le parole 6/10 h, passo della riga 17/10 h;
  - larghezze delle maiuscole: strette (Г, Е, С, cifre 2, 3, 5) 5/10 h; normali (Б, В, Д, И, «4») 6/10 h;
    larghe (А, М, Х) 7/10 h; molto larghe (Ж, Ф, Ш) 8/10 h; l'«1» 3/10 h `[GOST-L]`.
- **ГОСТ 26.020-80** `[GOST-26020]` (alta): il carattere **«приборный» per strumenti e apparati
  radioelettronici**, quello giusto per i pannelli.
  - Пр3 diritto semigrassetto (il principale), Пр41 stretto chiaro, Пр5 grassetto.
  - Ingombri, margini inclusi: «А» 0,95 H, «Ш» 1,25 H, cifre 0,70 H; interlinea ≥ H/2.
  - Il ГОСТ 26.008-85 lo applica alle incisioni `[GOST-26008]`.
- **Indicatori numerici:** Nixie ИН-12/ИН-14 (cifra da 18 mm, arancio) e ИН-18; VFD ИВ (blu-verde, nelle calcolatrici
  dal 1971) `[RU-NIXIE]` `[RU-VFD]`. A bordo dei sottomarini: n.v., e nelle foto dei musei non compaiono.
- **Schermi di Akkord e Okean:** generatore di caratteri e fosforo n.v. Il fosforo **verde giallino** è la scelta
  dell'utente (§5.2).

### 9.4 Proposta: quattro font bitmap [GIOCO]

Si disegnano da zero, a 640×480. Riferimento di scala `[CALC]`: su un 14" a 60 cm, 16′ d'arco ≈ 6 px e 20′ ≈ 8 px.
Le maiuscole delle scritte da leggere stanno quindi fra 6 e 9 px, coerenti con le matrici 5×7 e 7×9 delle norme.

| Font | Glifo e cella | Disegno | Colori | Dove |
|---|---|---|---|---|
| **«Fosforo 688»** (terminale) | **5×7 in cella 6×9** (80 colonne = 480 px) e **7×9 in cella 8×12** (80×40 a tutto schermo) | solo maiuscole, zero barrato, tratto di 1 px, **segmenti dritti alla Hershey** con gli angoli a 45° e niente curve | rampa del fosforo verde di 3-4 toni (nucleo, alone, persistenza); variante ambra per la TMA | testi su tutti gli schermi del 688: sonar, Mk 81, WLR-8, sala radio, intestazioni delle cascate |
| **«Incisione 688»** (Gorton) | maiuscole alte **5 px** (minimo) o **7 px**; titolo del pannello 1-2 px più alto | a tratto unico, larghezze 0,6-0,7 h (M e W 6 px su h 7, I e 1 un tratto), 1 px fra le lettere e 4-5 fra le parole; estremità ammorbidite con un mezzo tono, come le lascia la fresa | **bianco su targhetta nera** (tipo I) o nero su grigio chiaro; strisce gialle e nere per i comandi d'emergenza | targhette dei pannelli: BCP, governo, Mk 81, WCP, ESM, rack radio; cartelli d'avviso |
| **«Stampino URSS»** (latino in stile ГОСТ) | **h 10 px** (h 7-8 nei pannelli bassi); tratto 1 px (h/10) o 2 px sulle aste per il semigrassetto Пр3 | proporzioni del tipo Б applicate al latino: E, F, C, S 5 px, normali 6, A, M, X 7, W 8; 2 px fra le lettere, 6 fra le parole, riga ogni 17 px; variante inclinata a 75° (1 px ogni 4 righe); variante a stampino con ponticelli da 1 px su O, A, B, D, P, R, 0, 4, 6, 8, 9 per le marcature dipinte | **bianco su targhetta nera o rossa**, nero su crema; blu con bianco per gli impianti; sugli schermi la stessa base nel **fosforo verde giallino** | tutte le stazioni dell'Alfa (requisito 12): nomi dei pulti, comandi, scritte sugli schermi di Akkord e Okean, striscia della telescrivente |
| **«Cifre»** (numeri) | 7 segmenti **5×9** o **6×11**; tamburi larghi 3/5 h | segmenti di 1-2 px, inclinazione facoltativa di 1 px ogni 4-5 righe; tamburo con la cifra a metà giro tagliata | **688:** LED rossi coi segmenti spenti visibili in rosso scurissimo, tamburi bianchi su nero. **Alfa:** **quadranti analogici e tamburi** (le foto non mostrano display); Nixie arancio solo come estro, n.v. | quota, rotta, velocità, giri e tempi della manovra e del BCP del 688; contatori a tamburo di quota e rotta impostate su Боксит |

Il font originale (`originale.fnt`) resta per i menu e i messaggi di bordo, come nel 1989. I quattro nuovi servono
solo alle stazioni ridisegnate della Fase 3.

### 9.5 Fonti della sezione

- `[MIL-18012]` MIL-M-18012B: https://everyspec.com/MIL-SPECS/MIL-SPECS-MIL-M/MIL-M-18012B_6496/
- `[MIL-1472]` MIL-STD-1472F (§2.6)
- `[PHOS]` https://en.wikipedia.org/wiki/Phosphor (tabella JEDEC)
- `[HERSHEY]` https://en.wikipedia.org/wiki/Hershey_fonts
- `[UGC]` https://islandregister.com/phones/teletype/ugc-504.html
- `[CHM]` Computer History Museum, display numerici (citato nella ricerca)
- `[GOST-2304]` ГОСТ 2.304-81: https://meganorm.ru/Data1/6/6298/index.htm
- `[GOST-L]` larghezze del tipo Б: https://sersalaev.narod.ru/spravochnik/Index-11.htm
- `[GOST-26020]` ГОСТ 26.020-80: https://files.stroyinf.ru/Data2/1/4294846/4294846525.htm
- `[GOST-26008]` ГОСТ 26.008-85: https://meganorm.ru/Index2/1/4294828/4294828445.htm
- `[RU-NIXIE]` https://ru.wikipedia.org/wiki/Газоразрядный_индикатор
- `[RU-VFD]` https://ru.wikipedia.org/wiki/Вакуумно-люминесцентный_индикатор
- `[GORTON]` https://en.wikipedia.org/wiki/Gorton_(typeface)
- **Foto di pubblico dominio** (Commons, consultate):
  - «Trident digital sonar system» (1975);
  - «Greeneville Sonar Room»;
  - «Defense.gov News Photo 971129-N-5745B-001» (Annapolis, 1997);
  - «US Navy 040901-N-1348L-005» (Toledo, 2004);
  - categoria «Submarine sonar rooms».
- **Foto CC BY-SA** (consultate, non proiettate né copiate): B-396 (Mike1979 Russia, CC BY-SA 3.0; Alexxx1979, CC
  BY-SA 4.0), B-39 (BrokenSphere, CC BY-SA 3.0), categorie B-307 e B-413.
- **Lacune:**
  - fosforo delle OJ-274/462;
  - display del pannello di governo del 688;
  - formato dell'USQ-69 e matrice dell'UGC-136;
  - fosfori e generatori di caratteri di Akkord e Okean.

  DTIC e USNI non erano raggiungibili: la tesi NPS ADA056383 resta da leggere.

