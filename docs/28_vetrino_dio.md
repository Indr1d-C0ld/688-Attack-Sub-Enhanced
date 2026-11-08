# Il «vetrino di Dio»: lo spettatore interviene (roadmap 36)

**Proposta di trailer-1, 31/10/2026, da approvare.** Nasce dall'idea dell'utente del 31/10: nel vetrino (docs/19) lo
spettatore non guarda soltanto, ma interviene dall'esterno, come nei «simulatori di Dio» e di città degli anni '90.
Aggiunge una nave, guasta un sonar, fa salire il mare, falsifica un ordine radio, poi guarda cosa succede. E lo fa in
modo quasi scientifico: lo stesso scenario con e senza l'intervento, su molti semi, e il confronto fra le due serie.

Si appoggia su quello che il vetrino ha già (docs/19, V1-V3):
- la partita senza giocatore, col comandante IA del battello;
- le regolazioni per parte (situazione, ROE, livello, allerta, carico), gli interruttori dell'IA;
- le letture «verità», «parte» e «unità», il registro, il replay e il debriefing dello spettatore;
- le statistiche tecniche (R1-R6) e le serie su N semi (`serie_sandbox`, la schermata delle serie);
- il codice sandbox.

E sulle regole delle missioni (docs/17, scenario_missione): condizioni e azioni a tempo o su evento, già scritte in Lua,
già nel registro e nel replay. Un intervento è, in pratica, **una regola aggiunta a partita in corso**.

---

## 1. Principi

1. **Solo Enhanced e solo nel vetrino.** Il classico non cambia, e nelle partite col giocatore non ci sono interventi.
2. **Il mondo resta fisico.** Un intervento è un fatto del mondo (un'avaria, una nave in più, un ordine radio, un
   rumore in acqua), non un potere sull'IA. Le unità dell'IA lo scoprono solo coi loro sensori e coi loro canali, come
   tutto il resto (docs/18, simmetria). Nessuna unità «sa» che c'è stato un intervento.
3. **Lo spettatore sa tutto.** Le viste per lo spettatore mostrano gli interventi, anche quelli che l'IA non ha
   percepito; il registro dello spettatore li separa dagli eventi del mondo.
4. **Deterministico.** Lo stesso scenario, le stesse regolazioni, lo stesso seme e lo stesso elenco di interventi danno
   la stessa partita. L'elenco entra nel replay e in un file che si può dare a un amico.
5. **Ogni intervento si dice in una riga.** «A 40 minuti, il sonar della Spruance di testa a metà portata per 20
   minuti.» È la riga che compare nel registro, nella linea del tempo, nel file e nel confronto delle serie.
6. **Fuori carriera,** come il vetrino.

---

## 2. Quando scatta un intervento

Ogni intervento ha un **innesco**:
- **subito**, col tasto, nella partita in corso (si registra l'istante esatto, al passo);
- **a tempo**: «a 40 minuti»;
- **su evento**, con le condizioni delle regole: «quando la Krivak classifica l'Alfa», «quando parte il primo siluro»,
  «quando l'SSBN entra nella zona». Le condizioni sono quelle dell'editor (docs/17 §1, F7), più qualcuna per lo
  spettatore (un'unità qualsiasi colpita, la prima scoperta fra due parti);
- con una **durata** facoltativa: il guasto per 20 minuti, il disturbo per un'ora, poi si torna come prima.

Gli interventi programmati (a tempo e su evento) si preparano nella schermata delle regolazioni, prima della partita:
sono quelli che servono agli esperimenti (§5). Quelli subito si fanno dal vivo, e si possono salvare come programmati
per la volta dopo.

---

## 3. Il catalogo degli interventi

Per ognuno: che cosa cambia nel motore, che cosa vede lo spettatore, che cosa resta coerente. La difficoltà è quella
**data dal runtime il 31/10/2026**: **c** = c'è già (un'azione delle regole o una funzione dell'API), **n** = nuovo
piccolo, **N** = nuovo grosso, coi giorni di lavoro del runtime.

### 3.1 Forze

| Intervento | Nel motore | Lo spettatore vede | Coerenza | Diff. |
|---|---|---|---|---|
| **Inserire un'unità** (classe, parte, posizione, rotta, comportamento) | un'unità nuova, come le riserve (`entra`) ma creata dove si vuole | l'unità compare sulla carta con un alone «inserita» | entra nei contatti solo quando qualcuno la sente; nel replay è nell'elenco | n (0,5 g) |
| **Togliere un'unità** | `togli` (l'opcode 37h, dalla S23) | sparisce con un segno «tolta» | per l'IA è un contatto perso, non una distruzione | c |
| **Ordinare** (vai a, quota, velocità, comportamento) a un'unità dell'IA | le azioni `vai_a`, `quota`, `comportamento` | la rotta nuova e la riga nel registro | l'ordine scavalca la dottrina finché dura; poi l'unità torna alla sua IA | c |
| **ROE e situazione per parte** a partita in corso | l'azione `situazione` (anche fra due parti, M7) e le ROE della radio | la carta delle parti cambia colore; riga nel registro | le unità le ricevono come un ordine radio della loro parte: chi è sotto e non ascolta il broadcast le sa dopo | n (0,5 g: si leggono vive, ma vanno avvisate allerta ed EMCON) |
| **Allerta di una forza** | l'allerta per forza esiste (fasi 2-4); va solo esposta | il livello A0-A3 sulla scheda della forza | sale e scende poi come sempre | n (0,5 g: c'è `evento_allerta`, manca l'ingresso dall'esterno) |
| **Cambiare parte a un'unità** (un neutrale che diventa ostile) | la parte è letta in molti posti: contatti, reazioni, ROE | il simbolo cambia colore | le altre unità la trattano secondo quello che sanno di lei, non secondo la parte nuova | N (2 g: i bit di parte sono facili, ma vanno rifatti contatti, TMA, forze, coppie e nemici del luogo) |

### 3.2 Guasti

| Intervento | Nel motore | Lo spettatore vede | Coerenza | Diff. |
|---|---|---|---|---|
| **Macchine** (propulsione persa, velocità massima ridotta) | `avaria` (M6) per la perdita; la riduzione è nuova | l'unità rallenta; segno «guasto» | i rumori di riparazione si sentono, come oggi | c / n |
| **Sonar o radar** spento o a metà portata | un fattore sul sensore dell'unità, per un tempo | la portata ridotta disegnata attorno all'unità | l'unità perde i contatti che non sente più, coi tempi veri | sonar e schiera spenti c (`Missione::sonar_ia`); degradato n; radar n (0,5 g) |
| **Armi** (tubi bloccati, lanciatore fuori uso, siluri inerti) | i tubi e i lanciatori ci sono; serve lo stato «guasto»; inerti c'è (`esercitazione`) | segno sulle armi dell'unità | il comandante IA prova e non può: nel registro «tubo fuori uso» | n (0,5 g: il carico dell'IA ha i tubi, basta svuotarli o bloccarli) |
| **Timone o piani bloccati** | il rateo di virata o di quota a zero, o bloccato su un valore | traiettoria e quota nel profilo | la collisione col fondo o con altri è possibile, ed è il punto | n (1 g, dalla dinamica e dai limiti del battello) |
| **Danno a un compartimento** (allagamento, incendio) | i danni dei sottomarini e delle navi ci sono (kit) | il diagramma dei danni nella scheda | si comporta come un colpo senza scoppio: nessuna arma nel registro | n |

### 3.3 Ambiente

| Intervento | Nel motore | Lo spettatore vede | Coerenza | Diff. |
|---|---|---|---|---|
| **Meteo e mare** (stato del mare, vento, visibilità, nubi) a partita in corso | il meteo fisso c'è all'avvio; a partita in corso va propagato a sonar, rumore ambiente, onde, vista | i numeri del meteo e la carta | cambia per tutti nello stesso istante, nella zona scelta o in tutto il teatro | n (1 g: il meteo realistico avanza da sé, quello fisso va reso modificabile) |
| **Notte e giorno** (spostare l'ora) | sole e luna dalla data e dall'ora (la luna c'è dal 31/10) | il cielo nelle viste | salto d'ora per tutti: le sagome e i fanali si ricalcolano | n (1 g) |
| **Strato termico** (più forte, più profondo, assente) | il profilo del suono del teatro | il profilo nella scheda | per tutti: è il mare che cambia, non un sensore | n-N (1-2 g: profili e raggi sono in cache per mese) |
| **Ghiaccio** (pack che avanza, polynya che si apre) | il campo di ghiaccio è un dato del teatro (M2) | la carta del ghiaccio | un battello sotto il pack resta sotto: va controllato chi si trova intrappolato | N (3 g: il campo è procedurale, dal seme all'avvio) |

### 3.4 Comunicazioni

| Intervento | Nel motore | Lo spettatore vede | Coerenza | Diff. |
|---|---|---|---|---|
| **Disturbo della radio di una parte** (messaggi persi, ritardati, rapporti che non arrivano) | la sala radio e il quadro radio esistono; serve un filtro per parte e per tempo | segno «disturbo» sulla linea del tempo; i messaggi persi nel registro dello spettatore | l'IA agisce su quello che riceve: un rapporto perso è un contatto che il comando non conosce | n (1 g) |
| **Falso ordine radio** (ROE, rotta, rientro) a una parte | i messaggi non autentici ci sono per il giocatore (`autentico = false`); per l'IA va deciso chi ci crede | il messaggio falso in rosso | l'IA lo segue secondo una probabilità per livello, o lo verifica (docs/18): la verifica è un'emissione radio, quindi rischiosa | al giocatore c; all'IA N (2 g: l'IA non legge ordini, sarebbero finti messaggi fra unità nelle forze) |
| **Falso rapporto di contatto** (un datum inventato) | l'azione `cue_asw` c'è; per le navi un rapporto radio nel quadro della parte | il datum falso sulla carta | la parte manda unità e aerei dove non c'è niente: è il consumo di tempo e carburante che si misura | c / n |

### 3.5 Inganno

| Intervento | Nel motore | Lo spettatore vede | Coerenza | Diff. |
|---|---|---|---|---|
| **Falso contatto acustico** (una sorgente che suona come una classe, senza unità) | una sorgente acustica mobile, come un'esca semovente con la firma di una classe | la sorgente con un segno «falso» | chi la sente la tratta come un contatto vero, finché la TMA o la classificazione non la smentiscono | c / n (0,5 g: ci sono le sorgenti del paesaggio sonoro senza unità) |
| **False flag** (un'azione attribuita a un'altra parte: un siluro, una mina, un colpo) | serve l'attribuzione: chi viene «creduto» il lanciatore. Il siluro parte da un punto, e le parti deducono il colpevole da quello che hanno sentito e visto | la freccia «vera» e quella «creduta» | la reazione segue la deduzione, non la verità: una parte può dichiarare la guerra alla parte sbagliata | N (2-3 g: l'attribuzione tocca registro, forze e reazioni) |
| **Incidente provocato** (uno scoppio in acqua, una collisione, un siluro «di nessuno», una mina che salta) | lo scoppio c'è (mine, esplosioni), la collisione pure; il siluro senza lanciatore è nuovo | il fatto e la sua causa vera | chi lo sente reagisce come a un'arma vera: allerta, evasione, rapporti | n (0,5 g ciascuno; il siluro con lanciatore -1 va gestito nei registri) |
| **Spia** (una parte riceve per un tempo il quadro vero dell'altra) | il quadro di una parte c'è (`quadro_parte`); va dato a un'altra | segno «spia» sulla linea del tempo | rompe la simmetria apposta: è l'esperimento «che cosa vale l'informazione» | n |

---

## 4. La coerenza: registro, replay, codice

- **Registro:** un evento nuovo `intervento` (chi, che cosa, innesco, durata), nel registro dello spettatore con un
  colore suo. Non entra nei registri delle parti: l'IA non sa che c'è stato.
- **Traccia e replay:** l'elenco degli interventi entra nella traccia (un record suo); il replay rifà la partita dagli
  ordini del comandante IA **più** gli interventi, allo stesso passo. È la stessa strada degli ordini del giocatore
  oggi.
- **Il file dell'esperimento:** il codice sandbox (30 caratteri) non basta per un elenco. Propongo un file `.m688v`
  (vetrino), in testo TOML: la missione con la sua impronta, le regolazioni, il seme o la serie di semi, gli interventi
  programmati. Dal codice sandbox si ritrova la partita senza interventi; dal file, quella con.
- **Determinismo:** gli interventi subito si registrano col passo esatto; un intervento su evento scatta nel passo in
  cui la condizione diventa vera, come le regole delle missioni. **Vincolo del runtime:** un intervento che consuma
  numeri casuali va applicato sempre nello stesso punto del passo, se no il replay diverge.

---

## 5. Gli esperimenti

L'idea dell'utente è studiare le conseguenze «in modo quasi scientifico». Quindi:

- **Il disegno a coppie.** Ogni seme si gioca due volte, senza e con l'intervento. Fino all'istante dell'intervento le
  due partite sono identiche; da lì divergono. Il confronto a coppie elimina il rumore del seme.
- **Le serie del V3.** `serie_sandbox` gioca la stessa missione su N semi; qui due serie, A senza e B con, sugli stessi
  semi. Le statistiche sono quelle di §4 di docs/19: esiti, catene di scoperta, tempi di reazione, TMA, armi e Pk,
  esposizione, perdite.
- **Il confronto:**
  - per gli esiti, la differenza delle frequenze con l'intervallo di confidenza (Wilson);
  - per i tempi e i numeri (minuti alla prima scoperta, lanci, perdite), la differenza delle mediane con un intervallo
    bootstrap, e quante coppie cambiano in un senso o nell'altro;
  - il **punto di divergenza**: in quanti secondi le due partite di una coppia prendono strade diverse, e il primo evento
    che cambia.
- **La schermata del confronto:** la schermata delle serie con due colonne, A e B, le differenze in evidenza, gli
  istogrammi sovrapposti, e un clic su una coppia apre i due replay affiancati, fermi al punto di divergenza.
- **Esperimenti pronti,** come i vetrini pronti: «il sonar della scorta guasto», «il falso ordine di rientro», «la nave
  spia che vede tutto», «la false flag nello Stretto». Ognuno con una riga che dice cosa mostra.

Quanti semi: con 20 coppie si vede una differenza di esito di circa 30 punti; con 50 di circa 20 (STIMA da rifare coi
numeri veri delle serie).

---

## 6. L'interfaccia

- **Il pannello degli interventi** (tasto **G** nella vista dal vivo, perché I e O fanno lo zoom della carta): le
  cinque categorie, poi l'intervento, poi l'unità o la zona (un clic sulla carta), poi i parametri (portata, durata,
  ora). L'ultima voce, «quando: fra», a 0 lo applica subito, altrimenti lo programma a tempo (su evento: dopo la D1).
  Backspace annulla l'ultimo.
- **La linea del tempo** in basso: gli interventi fatti e quelli programmati, con la riga di testo al passaggio del
  mouse; gli eventi importanti del mondo sopra (scoperte, lanci, colpi). Nel replay si può saltare a un intervento.
- **I segni sulla carta:** un segno per famiglia (forze, guasti, ambiente, comunicazioni, inganno) accanto all'unità o
  sulla zona; i falsi contatti e le false flag con la doppia freccia «vera» e «creduta»; le portate ridotte dei sensori
  guasti.
- **Nella schermata delle regolazioni,** una pagina «interventi programmati», e il tasto per giocare la serie A/B.
- Lo stile resta quello del vetrino (640×480, 256 colori, cornici del kit).

---

## 7. Le fasi e chi fa cosa

| Fase | Che cosa | runtime | disassemblatore | kit | trailer-1 | principale |
|---|---|---|---|---|---|---|
| **D1, gli interventi di base** | forze (inserire, togliere, ordini, ROE, allerta), guasti (macchine, sensori, armi, timone), meteo e ora; subito e a tempo; registro, traccia, replay | 5-6 g: unità create, ROE e allerta dall'esterno (1,5), sonar degradato e radar (1), tubi (0,5), timone e piani (1), meteo e ora a partita in corso (2), record della traccia | 1 g: i sensori guasti nell'acustica, il meteo a partita in corso nel sonar | 1 g: segni e pannello | 2 g: pannello, linea del tempo, segni sulla carta nel vetrino dal vivo, elenco nel replay | 1 g: tasto I, aggancio |
| **D2, gli esperimenti** | interventi su evento; file `.m688v`; serie A/B a coppie; schermata del confronto; esperimenti pronti | 1 g: inneschi su evento (le condizioni ci sono) | 1-2 g: confronto statistico (Wilson, bootstrap, punto di divergenza) nel riassunto delle serie | 1 g: grafici del confronto | 3 g: serie A/B, schermata del confronto, replay affiancati, esperimenti pronti | 1 g: file `.m688v` nel menu |
| **D3, comunicazioni e inganno** | disturbo radio, falsi ordini e rapporti, falsi contatti, incidenti provocati, spia | 5-6 g: disturbo radio (1), falsi ordini all'IA come finti messaggi fra unità (2), falso contatto (0,5), incidenti (1,5), spia (0,5) | 1-2 g: la sorgente falsa nell'acustica e nell'intercettazione | 1 g | 1 g: pannello, segni, esperimenti pronti | 0,5 g |
| **D4, false flag e cambi di parte** | l'attribuzione creduta, le reazioni che ne seguono, il cambio di parte di un'unità, il ghiaccio che si muove, lo strato termico | 8-10 g: false flag (2-3), cambio di parte (2), ghiaccio che si muove (3), strato (1-2) | 1 g | 1 g | 1 g: doppia freccia, esperimenti pronti | 0,5 g |
| **Ogni fase** | suite, rilievo classico (0/110), una serie A/B per ogni esperimento pronto, il manuale (cap. 12 e F8) | | | manuale | | |

Le stime sono in giorni di lavoro di una sessione e sono **STIMA**; quelle del runtime sono sue (31/10/2026). In tutto
circa 20-24 giorni del runtime, 4-6 del disassemblatore, 4 del kit, 7 di trailer-1, 3 della principale.

---

## 8. Domande aperte per l'utente

1. Gli interventi subito devono poter **tornare indietro** («annulla l'ultimo»)? Col determinismo è facile: si
   rigioca dall'inizio fino all'istante, senza quell'intervento.
2. Il **falso ordine radio** all'IA: ci crede sempre, ci crede secondo il livello, o lo verifica col rischio di emettere?
3. Le **false flag** e il **cambio di parte** (D4) toccano molte parti del motore: in questo piano o a parte?
4. Gli esperimenti A/B: bastano esito e statistiche, o serve anche un **rapporto scritto** dell'esperimento, da
   esportare?

---

## Stato

- 31/10/2026: proposta di trailer-1, con le stime del runtime, da portare all'utente.
- 31/10/2026: **approvato dall'utente, si parte dalla D1.** Risposte alle domande del §8:
  1. annulla l'ultimo intervento «subito»: sì (consiglio della principale, adottato): si rigioca dall'inizio senza
     quell'intervento; nel file dell'esperimento restano solo gli interventi tenuti;
  2. il falso ordine radio all'IA: **secondo il livello** (scelta dell'utente): un comandante esperto lo mette in dubbio
     più spesso e può chiedere conferma emettendo, col rischio di farsi scoprire;
  3. false flag e cambio di parte: restano la D4 di questo piano (consiglio adottato), da rivedere dopo la D3;
  4. rapporto scritto dell'esperimento A/B: sì (consiglio adottato), un testo esportabile con ipotesi, interventi, semi,
     numeri, intervalli e grafici.
- 31/10/2026: D1, primo aggancio (trailer-1): il pannello (`app/pannello_interventi`) legge `catalogo_interventi()`
  del runtime; nella vista dal vivo G lo apre e Backspace annulla l'ultimo; gli interventi escono in
  `RichiesteVivo::interventi` (tempo -1: subito) e `annulla_ultimo`, che main passa a `Missione::intervieni`.
- 31/10/2026: D1, i segni (trailer-1), dai record X della traccia: sulla linea del tempo del vivo e del replay un
  triangolo magenta per intervento (rosso se fallito), la durata in una riga sopra, i programmati col triangolo vuoto;
  sulla carta la lettera della famiglia accanto all'unità, il «vai a» tratteggiato, la zona in cerchio, l'unità tolta
  con una croce per due minuti, i programmati col punto in un rombo; nel replay G salta all'intervento seguente
  (Maiuscolo+G al precedente). Nel registro del vivo le righe «intervento» del runtime.
- 31/10/2026: D2, la parte di trailer-1 (forma del `.m688v` e dell'avvio nella nota trailer1-d2-forma-m688v.md):
  - `strumenti/serie_ab`: le due serie A e B con `serie_sandbox` sugli stessi semi (melchior688 `--esperimento F`, con
    `--senza-interventi` per A, nella parte della principale), poi `confronto.toml`, le tre pagine in BMP e
    `rapporto.md`;
  - `app/confronto_ab`: il confronto a coppie (`nucleo/confronto_serie` del disassemblatore) e il punto di divergenza;
  - `app/schermata_confronto`, nello stile C, con tre pagine: esiti con Wilson e la differenza appaiata, numeri per
    parte con il bootstrap, coppie col punto di divergenza; E scrive il rapporto;
  - `app/replay_affiancati`: A e B alla stessa scala e allo stesso istante, fermi al punto di divergenza (D per
    tornarci);
  - `app/esperimento_ab`: le due funzioni per il menu;
  - i quattro esperimenti pronti della D1 in `data/motore/esperimenti`: il sonar della scorta guasto, la scorta che
    non c'è, l'allerta anticipata, la burrasca. Quelli del §5 con falsi ordini, spia e false flag arrivano con la D3 e
    la D4.
- 03/11/2026: D3, il primo pezzo del runtime: l'intervento `contatto_falso` (INGANNO), un'esca semovente con la firma
  della classe imitata e senza unita' vera (id 990000-994999, `Missione::contatto_falso`), presa per vera dai sonaristi
  di giocatore e IA finche' la firma rovinata o l'eco d'esca del ping non la smentiscono; col ping della classe per
  l'intercettazione. I transitori dell'artiglieria sono passati a 995000 + 10n. Stato e aperti nella nota condivisa
  runtime-carico-vivo-d3-03-11.md.
- 03/11/2026: il falso contatto per lo spettatore (trailer-1). Nella traccia c'era gia': un'unita' «arma» senza
  lanciatore nella fascia dei falsi, con la classe imitata e la parte creduta nel flag; il replay e il vivo lo
  riconoscono da li' (app/replay.cpp, `falso`), senza cambiare la traccia. Nella verita' la classe imitata nella parte
  creduta, col segno dell'inganno e la scritta «FALSO»; nelle letture di chi lo sente (quadro del battello, parte,
  unita') il segno e la doppia freccia del kit dalla sorgente vera a dove la si crede (col solo rilevamento: sulla
  linea creduta alla distanza vera, cosi' si vede anche lo specchio della schiera). Il falso non e' fra le unita'
  proprie di nessuna parte. Lo stesso giorno la vista dal vivo e' passata alla variante C (bozza vivo_C del kit).
- 03/11/2026, sera: D3 del runtime, altri due interventi del gruppo COMUNICAZIONI:
  - `disturbo_radio`: una parte perde e ritarda i messaggi (rapporti di formazione e di teatro, FLASH, ROE), anche solo
    in una zona;
  - `ordine_falso` (rientro, vai a, armi trattenute, armi libere; catalogo aperto): ogni unita' ci crede secondo il suo
    livello e la credibilita' dell'ordine. Chi dubita puo' chiedere conferma: trasmette, e le altre parti la localizzano
    con la stessa radiogoniometria del giocatore, ora anche verso l'IA.

  Restano gli incidenti provocati e la spia.
- 03/11/2026, sera: i segni delle comunicazioni della D3 per lo spettatore (trailer-1, app/replay.cpp). In alto a
  sinistra sulla carta gli interventi su tutta una parte (il disturbo radio senza zona, l'ordine falso col suo «vai a»;
  il disturbo con la zona ha il cerchio, come il meteo). Sotto il simbolo di ogni unita' che ha ricevuto l'ordine falso,
  finche' l'ordine dura, come l'ha preso, dalla reazione_ia del registro: CREDE in rosso chiaro, IGNORA e SMASCHERA in
  verde, VERIFICA in giallo, NON RICEVUTO in grigio. Per due minuti le localizzazioni radiogoniometriche (reazione
  «localizza») sono una linea gialla tratteggiata da chi localizza a chi trasmette, con «DF». La localizzazione della
  rete strategica (unita' -1) non entra nel registro, quindi non si vede.
- 03/11/2026, notte: la D3 del runtime è chiusa. Due interventi nuovi del gruppo INGANNO:
  - `incidente`:
    - lo **scoppio** senza arma si sente come quello di un'arma: la formazione entro 20 km va ad A3, il sottomarino
      entro 3 km evade dal punto;
    - il **siluro «di nessuno»** è un'arma col modello, della parte creduta e senza lanciatore, sul bersaglio vero se
      c'è. Chi lo sente reagisce come a un siluro vero, ma nessuno provoca la guerra: l'attribuzione è la D4.
  - `spia`: finché dura, una parte riceve ogni minuto il quadro vero dell'altra, come rapporti di contatto con la
    posizione e la classe vere.

  La radiogoniometria ha ora una regola sola, `regola_df` in nucleo/radio, che usano sia la sala radio del giocatore
  sia l'IA.
- 03/11/2026, sera: l'inganno della D3 per lo spettatore (trailer-1). La spia in alto a sinistra con gli altri interventi
  su tutta una parte; l'incidente provocato per due minuti, lassu' e sul punto (la stella dello scoppio, o il segno
  dell'inganno dove parte il siluro); il siluro «di nessuno» (un siluro senza lanciatore fuori dalla fascia dei falsi)
  nel colore della parte creduta, col segno e la scritta «SILURO DI NESSUNO». Tre esperimenti pronti della D3 sulla S5,
  10 semi: «Il falso ordine di rientro» (a 60 s, credibile a 0,5), «La nave spia che vede tutto» (il Patto vede la NATO
  da 0 s), «Lo scoppio nel transito» (300 kg a 40 m, 6 km a sud del convoglio, a 120 s; la S5 e' di giorno e il codice
  sandbox non porta l'ora). Una prova nuova (prove_confronto_ab) applica ogni esperimento pronto nella S5: ha trovato
  l'allerta anticipata al tempo 0, che falliva «senza le forze» (le forze nascono al primo passo), ora a 1 s.
- 03/11/2026, notte: D4, la false flag (runtime). L'intervento `false_flag` (INGANNO) fa lanciare a un'unità vera un
  siluro con la parte di un'altra. Per un'arma travestita, o senza lanciatore, la parte colpita deduce il colpevole dai
  suoi sensori (`colpevole_creduto`):
  1. un lanciatore identificato da poco (classificato per classe, o visto);
  2. la firma dell'arma;
  3. il contatto non amico più vicino al punto, entro 20 km;
  4. ignoto.

  La provocazione segue la deduzione, fino alla guerra con la parte sbagliata (registro: `reazione_ia`, evento
  «false_flag»). Insieme, una correzione generale: contro una nave il siluro dell'IA tira col soffitto a 0, perché prima
  la testa (non a scia) ignorava i bersagli in superficie.
- 03/11/2026, sera: la false flag della D4 per lo spettatore (trailer-1). Il siluro con la parte di un altro (flag 0x20
  con una parte diversa da quella di chi lancia) corre nel colore di chi l'ha lanciato davvero, col segno dell'inganno
  e «FALSE FLAG: SEMBRA NATO/PATTO»; per due minuti l'intervento e' in alto a sinistra, e sotto l'unita' colpita c'e' a
  chi da' la colpa (reazione «attribuisce_...»).
- 03/11/2026, sera: D3 chiusa dal runtime; della D4 sono dentro `false_flag` e `cambia_parte` (chi non lo sa la crede della
  parte di prima, `Missione::parte_nota`). Gli interventi a t=0 aspettano che il loro sistema esista. Strato e ghiaccio
  (proposta del disassemblatore, approvata): nomi `strato`, `ghiaccio_deriva`, `polynya`, gruppo AMBIENTE. Stato nella nota
  condivisa runtime-carico-vivo-d3-03-11.md.
- 04/11/2026: dentro le voci `strato`, `ghiaccio_deriva` e `polynya` (AMBIENTE, F8 §2.7b), l'avviso a chi è emerso e i lettori di
  `sld_in` e `copertura_ghiaccio` nelle missioni. Restano la vista del fondale, la carta del ghiaccio e i segni dello spettatore.
- 04/11/2026, sera (trailer-1): quattro serie A/B sulla S5 dopo la correzione dei missili (T12 §10); la spia non cambia nulla nella S5. Lo
  spettatore della D4: la scritta CREDUTA NATO/PATTO sotto chi ha cambiato parte, fino all'istante in cui lo sa tutta la parte
  (reazioni `cambio_saputo_usa` e `cambio_saputo_urss`, `app/parti_credute.hpp`), i colori nuovi nella verità e quelli creduti nelle
  letture. La false flag ha la stessa scritta, sul siluro con la doppia freccia e sotto chi ha lanciato dopo la colpa. I segni di
  `strato`, `ghiaccio_deriva` e `polynya` sulla carta del replay (`app/segni_ambiente.hpp`). Restano la vista del fondale e la
  carta del ghiaccio.
- 04/11/2026, sera (trailer-1): un'unità di riserva non ancora in scena non riceve l'ordine falso (`passo_ordini_falsi`; le arriva quando entra, se dura ancora).
  Scena M3 ed esperimento «La spia oltre l'orizzonte»: senza spia 0 lanci, con la spia 108 P-500 e 20 Perry su 20 affondate in dieci coppie (T12 §10).
  Far leggere il quadro radio ai sottomarini cambierebbe gli esiti delle missioni: resta alla principale, come punto di simmetria. *Chiuso il 05/11/2026: i sottomarini ricevono il quadro a quota d'antenna, e la spia verso di loro passa dal canale (`docs/18`, stessa data; manuale E6 §2.8 e §7, F8 §2.7b).*
- 04/11/2026, notte: il ghiaccio della D4 per chi guarda. La vista del periscopio segue il campo vero (`app/vista_ghiaccio.hpp`: `copertura_ghiaccio`, `spostamento(t)`).
  La carta dello spettatore è in `app/replay.cpp`, non in `carta.cpp`: ora disegna la polynya dai soli record X (`polynye_a`), anche nel replay da file. La carta del
  giocatore resta il clima, e lui sa solo la volta che misura: ora anche sotto una polynya aperta, dove sondatore e sonar prima tacevano (`volta_da_misurare`). Il pack sulla carta dello
  spettatore viene da una griglia del clima nei metadati (`missioni/griglia_ghiaccio.hpp`), solo nel vetrino di un teatro col ghiaccio; ne segue la deriva cambiata. Resta il profilo
  nella scheda.
- 06/11/2026: il profilo nella scheda (punto 5 del piano). Dal vivo, con un'unità scelta, sotto la scheda: strato e asse del canale con com'erano, e c(z) in piccolo, con quello del mese più scuro, lo strato a tratti e
  la tacca dell'unità. Nel replay da file una striscia accanto alle quote del battello, sulla stessa scala. Da `mese_acustica` nei metadati, solo nel vetrino, e dai record X (`profilo_spettatore`). STIMA: senza l'effetto pomeriggio.
- 06/11/2026, sera: la polynya che si apre disegna acqua solo dove c'era il pack; la griglia del ghiaccio si scrive prima del primo passo, così un intervento `meteo` non la rende uniforme.
- 06/11/2026, notte: un intervento `meteo` a meteo realistico ferma il mare in tutto il teatro ma non il pack, che resta dov'è (`Meteo::fissa_sopra`). Un ping dentro una polynya sente le chiglie oltre il bordo. Sull'Alfa il pulsante 7, al periscopio, accende la telecamera verticale della vela.
