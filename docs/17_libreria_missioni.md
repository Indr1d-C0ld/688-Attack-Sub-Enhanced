# 17 — Libreria di missioni pronte: le schede dei soggetti

Roadmap punto 10, voce «Libreria di missioni pronte» (richiesta dell'utente, 25/10/2026). Le missioni si costruiscono
con l'editor (`missioni/scenario_missione.hpp`) e si distribuiscono come pacchetti `.m688m`
(`missioni/pacchetto_missione.hpp`). Questo documento contiene:
- il formato della scheda;
- i soggetti, da far approvare all'utente prima della costruzione.

Prima i soggetti **inediti** (§2), poi quelli **ispirati a fatti reali** (§2b), dal dossier
`docs/ricerca/23_episodi_guerra_fredda.md`.

**Totale previsto:** 20-24 missioni, metà col 688 e metà con l'Alfa, con difficoltà e situazioni varie.

**Niente doppioni:**
- con le 22 originali: TORPEX, l'Alfa da scrollarsi di dosso, la ricerca costiera a Poljarnyj, l'attacco alla Libia,
  la fuga da Gibilterra, il convoglio dell'Adriatico, il Typhoon di Kunashir, il gruppo di superficie e i rinforzi
  della Celtica, la traversata atlantica e l'agguato coi missili;
- con le tipologie delle missioni rapide: pattuglia, caccia, antinave, scorta, ricognizione, minamento, inserimento.

Qui contano la storia e le svolte.

## 1. Il formato della scheda

| Voce | Che cosa |
|---|---|
| **Titolo** e **codice** | il codice della libreria: `LIB-I01`… per le inedite, `LIB-S01`… per le storiche |
| **Battello** | 688 (USS, Flight I) o Alfa (Progetto 705) |
| **Teatro** | uno dei sette teatri del gioco, col luogo preciso |
| **Data e ora** | il giorno e l'ora UTC; la stagione e la luce (alba, giorno, tramonto, notte) |
| **Meteo** | generato (semplificato o realistico) o fissato: vento, mare, nubi, visibilità |
| **Situazione e ROE** | pace, crisi o guerra; le regole d'ingaggio all'inizio e come possono cambiare |
| **Premessa** | il soggetto, 5-10 righe di qualità narrativa: quello che il comandante sa quando legge gli ordini |
| **Obiettivi** | primari e secondari, coi tipi M1 (docs/09 §5.2) e i nuovi che servono |
| **Svolta** | il colpo di scena: che cosa cambia durante la missione, quando e perché |
| **Forze** | unità per parte con classe e ruolo; quelle nascoste e quelle di riserva (che entrano con una regola) |
| **Comportamenti e regole** | i modelli dell'IA (libreria 2) coi parametri, le regole «quando… allora…» principali |
| **Che cosa la rende diversa** | il gesto di gioco che la missione chiede e le altre no |
| **Difficoltà e durata** | 1-5, durata attesa di gioco in tempo simulato |
| **Ispirata a** | il fatto reale, oppure «inedita» |
| **Che cosa serve al motore** | i sistemi usati (c'è, in arrivo, manca) |

Legenda dei sistemi: **c'è** = nel motore e nell'editor; **in arrivo** = già pianificato (collisioni, artiglieria,
richieste E6 al runtime); **manca** = da chiedere (elenco al §3).

---

## 2. Soggetti inediti

### LIB-I01 — «La balena di ferro»

- **Battello:** 688.
- **Teatro:** Fær Øer, dorsale Islanda-Fær Øer.
- **Data:** 3 marzo 1989, 04:40Z (notte, poi alba).
- **Meteo:** realistico, fine inverno: mare 4-5, vento da SW, scrosci.
- **Situazione:** pace. ROE: armi trattenute per tutta la missione.
- **Premessa:** Da tre giorni il SOSUS di Keflavík segue un contatto che non torna con nessuna firma in archivio:
  un monoalbero a sette pale, quieto come un Victor III che non c'è, che scende la dorsale a sei nodi. A Norfolk lo
  chiamano «la balena di ferro». Potrebbe essere il primo Akula a uscire dal Mare di Barents. Il Los Angeles è il
  battello più vicino. Gli ordini sono semplici da scrivere e difficili da eseguire: trovarlo, classificarlo, registrarne
  la firma e restargli attaccato finché l'archivio non è pieno, senza che sappia di avere compagnia.
- **Obiettivi:**
  - primari: classificare il contatto (scoprire); tenerlo in contatto sonar per almeno 40 minuti cumulativi (nuovo
    tipo «contatto»); non essere scoperti;
  - secondario: classificare anche il secondo battello.
- **Svolta:** dopo circa 50 minuti il contatto accelera e accosta di 90 gradi, un «Crazy Ivan». Poco dopo si capisce
  che i battelli sovietici sono due. Il Victor III rumoroso che il SOSUS aveva preso per «la balena» è un'esca; l'Akula
  vero è entrato in scena dietro al giocatore e lo sta pedinando.
- **Forze:**
  - URSS: un Victor III, l'esca, rumorosa, su rotta di transito con zig-zag; un Akula di riserva, che entra con la
    regola «svolta» alle spalle del giocatore, col comportamento di pedinamento;
  - NATO: un P-3C amico di passaggio, coi rapporti radio.
- **Regole:**
  - a 50 minuti, o se il giocatore è entro 4 nm dal Victor, entra l'Akula e il Victor fa il Crazy Ivan;
  - se l'Akula perde il giocatore per 20 minuti, la missione chiude con l'esito «ha seminato il pedinatore».
- **Che cosa la rende diversa:** il cacciatore diventa preda. Si gioca d'orecchio, col LOFAR e la TMA, senza mai
  sparare, e premia chi capisce in tempo che le tracce sono due.
- **Difficoltà:** 4. **Durata:** 2-3 ore.
- **Ispirata a:** inedita, nello spirito dei pedinamenti degli anni '80.
- **Serve al motore:**
  - c'è: sonar Enhanced, TMA, sonarista, ASW aereo, meteo realistico, riserva con regola;
  - manca: obiettivo «contatto» (minuti di contatto cumulativi); comportamento «pedinamento» nella libreria 2 (è
    dell'editor, lo faccio io); condizione «unità che ha perso il contatto da N minuti».
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_I01_BALENA_DI_FERRO.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - la svolta scatta a 50 minuti, o dopo 15 minuti di contatto sul Victor (al posto di «entro 4 nm», una distanza fra
    due unità in moto): entra l'Akula, col pedinamento, e il Victor fa il Crazy Ivan a 14 nodi per dieci minuti;
  - l'Akula entra alle spalle della posizione di partenza del giocatore;
  - «seminato» vuol dire che l'Akula non vede il giocatore da 20 minuti (condizione «unità non vede», nuova
    nell'editor). Con la firma registrata (40 minuti di contatto, obiettivo «contatto») l'esito è 40;
  - «non farsi scoprire» è secondario, perché il pedinamento è già una scoperta: il primario è seminare.

### LIB-I02 — «Il pescatore di Teriberka»

- **Battello:** Alfa.
- **Teatro:** Barents, al largo di Teriberka, davanti al bastione.
- **Data:** 12 dicembre 1989, 10:30Z: notte polare, il sole non sorge.
- **Meteo:** fissato: mare 3, neve a scrosci, visibilità 2 km.
- **Situazione:** crisi. ROE: fuoco solo se attaccati; cambiano con la radio.
- **Premessa:** Il Typhoon TK-20 rientra a Zapadnaja Lica dopo novanta giorni sotto il ghiaccio. Da una settimana un
  peschereccio norvegese, il «Havørn», si ostina a pescare nel posto sbagliato: proprio sulla rotta d'ingresso del
  bastione, con un cavo a poppa troppo pesante per una rete. Il comando della Flotta del Nord vuole che l'Alfa sgombri
  il corridoio. Deve accertare se il peschereccio rimorchia un sonar, e scortare il rientro del Typhoon senza dare alla
  NATO il pretesto di un incidente. In zona c'è anche un sottomarino americano: non si sa dove.
- **Obiettivi:**
  - primari: proteggere il Typhoon fino all'ingresso del golfo (proteggere); classificare il Los Angeles (scoprire);
  - secondari: identificare l'Havørn al periscopio (nuovo tipo «foto»); nessuna arma contro unità neutrali.
- **Svolta:** a metà missione arriva un messaggio flash, «emergere e identificarsi presso il punto ALFA»: è un ordine
  falso, con l'autenticazione sbagliata. Chi lo esegue senza verificarlo espone l'Alfa e il Typhoon. Poco dopo arriva
  quello vero, autenticato: «armi libere contro il sottomarino straniero entro le 12 miglia».
- **Forze:**
  - URSS: il Typhoon su rotta di rientro, velocità 8 nodi; un Krivak II e un Ka-27 di pattuglia costiera;
  - NATO: un Los Angeles in agguato, nascosto, e l'Havørn, un mercantile al posto del peschereccio, neutrale.
- **Regole:** il messaggio falso a 45 minuti (radio, con `autentico = false`); il vero a 70 minuti, che cambia le ROE;
  se il Typhoon viene scoperto dal Los Angeles, una regola lo fa accostare verso il golfo.
- **Che cosa la rende diversa:** la sala radio è il campo di battaglia: autenticazione, inganno, ROE che cambiano.
- **Difficoltà:** 3. **Durata:** 2 ore.
- **Ispirata a:** inedita. Le navi spia travestite da pescherecci sono un tema dell'epoca, ma qui nessun fatto preciso.
- **Serve al motore:**
  - c'è: sala radio con autenticazione e inganno (R1-R2), periscopi;
  - in arrivo: chiave `radio` nella missione, `m688.radio`, meteo fissato (richieste E6);
  - manca: obiettivo «foto», cioè fotografare un'unità o una classe al periscopio; una classe «peschereccio» o «AGI»
    (oggi un mercantile).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_I02_PESCATORE_DI_TERIBERKA.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - la radio della missione (E6): a 45 minuti l'ordine falso «emergere al punto ALFA», con l'autenticazione sbagliata;
    a 70 quello vero, che libera le armi contro il sottomarino straniero (ROE); le armi dell'IA restano quelle della crisi;
  - l'Havørn è un `peschereccio` neutrale, che pesca a 3,5 nodi sulla rotta. Identificarlo vuol dire classificarlo
    (manca la «foto»). Il rumore della rete è pronto (`[sorgenti.rete_strascico]` in acustica.toml): il runtime lo
    piazzerà da solo dietro ogni peschereccio in pesca, senza chiavi nella missione. Fino ad allora resta la firma «in
    pesca»;
  - se il Los Angeles tiene il Typhoon per due minuti (condizione «unità vede»), il Typhoon accosta verso il golfo a
    12 nodi. All'ingresso del golfo l'esito è 40; se il Typhoon è perso, 41;
  - il rientro è di 28 km a 10 nodi; il meteo è fissato (mare 3, neve, visibilità 2 km).

### LIB-I03 — «Acque basse, nervi tesi»

- **Battello:** 688.
- **Teatro:** Baltico, Kadetrinne e baia di Meclemburgo.
- **Data:** 18 settembre 1989, 14:00Z.
- **Meteo:** generato, autunno: mare 2-3, foschia.
- **Situazione:** pace, che può diventare crisi. ROE: armi trattenute.
- **Premessa:** La Volksmarine e la Flotta del Baltico fanno la loro esercitazione d'autunno a nord di
  Rostock-Warnemünde: corvette, una Krivak II, un Kilo nuovo di fabbrica. Il Los Angeles deve registrarne le
  procedure: frequenze dei sonar attivi, tattiche di caccia, comunicazioni. È un compito di routine, se non fosse che
  qui il fondale è di 25 metri e la vela del 688 da sola ne misura nove. Ogni accostata va pensata due volte. Ogni ping
  sovietico rimbalza fra fondo e superficie come una pallina in una scatola.
- **Obiettivi:**
  - primari: classificare tutte le unità dell'esercitazione (scoprire, minimo 4); restare in zona 90 minuti
    (raggiungere la zona più la regola di permanenza); non essere scoperti;
  - secondario: non toccare mai il fondo.
- **Svolta:** a un'ora dall'inizio il Kilo lancia un siluro da esercitazione, inerte, contro un bersaglio. Il siluro
  perde il bersaglio e aggancia il 688. Lo scafo deve evitarlo come se fosse vero: non sa che è inerte. Se il
  giocatore usa l'esca o accelera, i sovietici capiscono che c'è un intruso e l'esercitazione diventa una caccia vera
  (ROE di crisi).
- **Forze:**
  - URSS e RDT: 2 Koni, 2 Nanuchka, 1 Krivak II, 1 Kilo, 1 Be-12;
  - neutrali: traghetti e mercantili sulla rotta Rostock-Gedser.
- **Regole:**
  - a 60 minuti entra il «siluro da esercitazione», un'arma della riserva con il bersaglio sul giocatore;
  - se c'è un transitorio del giocatore entro 5 minuti dal siluro, si accende l'allarme e la situazione passa a crisi;
  - toccare il fondo dà l'esito «incidente diplomatico».
- **Che cosa la rende diversa:** navigazione in acque basse, gestione della quota e del rumore, e una scelta
  morale-tattica sotto un siluro che forse non è vero.
- **Difficoltà:** 4. **Durata:** 1,5-2 ore.
- **Ispirata a:** inedita.
- **Serve al motore:**
  - c'è: fondale e incagli, sonar Enhanced, acustica nelle acque basse, paesaggio sonoro;
  - in arrivo: collisioni (urto col fondo e fra unità);
  - manca: un'arma «inerte» (siluro da esercitazione senza testata) creata da una regola; cambio della situazione (ARMI)
    da una regola: oggi la situazione è fissa nel file, con la radio arriverebbe da R1.

### LIB-I04 — «La porta di Ercole»

- **Battello:** Alfa.
- **Teatro:** Gibilterra, dal Mare di Alboran all'Atlantico.
- **Data:** 7 luglio 1989, 22:10Z (notte).
- **Meteo:** realistico, estate: levante teso nello stretto, mare 3.
- **Situazione:** guerra. ROE: armi libere contro le unità della NATO.
- **Premessa:** La guerra è cominciata da trentasei ore e il Mediterraneo è già una trappola. L'Alfa deve uscire in
  Atlantico prima che la NATO chiuda la porta. Lo stretto è largo dodici miglia, profondo trecento metri nel canale,
  e pattugliato da fregate, elicotteri e un P-3C partito da Rota. Il comandante ha una sola carta da giocare: il
  traffico. Una portacontainer da centomila tonnellate passa lo stretto verso ovest a mezzanotte. Sotto il frastuono
  delle sue eliche un Alfa può sembrare niente.
- **Obiettivi:**
  - primario: raggiungere il Golfo di Cadice entro 3 ore (raggiungere);
  - secondari: non essere scoperti; affondare la Perry di guardia senza farsi localizzare.
- **Svolta:** a metà stretto il sonarista sente il clic degli idrofoni delle mine: la NATO ha posato un campo di mine
  CAPTOR sul fondo del canale. Bisogna lasciare la «scia» della portacontainer e passare dal lato marocchino, più basso
  e più rischioso.
- **Forze:**
  - NATO: 2 Perry, 1 Spruance con la schiera filata, SH-60B, P-3C da Rota, un campo di mine CAPTOR nel canale;
  - neutrali: la portacontainer, la copertura acustica, più altro traffico.
- **Regole:** il cue SOSUS fa decollare il P-3C se l'Alfa supera i 20 nodi; il campo minato viene «rivelato» quando il
  giocatore è a 3 nm (messaggio di bordo).
- **Che cosa la rende diversa:** infiltrazione acustica sotto il rumore di un'altra nave, e il velocissimo Alfa
  costretto ad andare piano.
- **Difficoltà:** 5. **Durata:** 2-3 ore.
- **Ispirata a:** inedita.
- **Serve al motore:**
  - c'è: ASW aereo con cue, acustica (mascheramento dal rumore del traffico), mine del giocatore;
  - manca: **mine dell'IA già posate** (un campo minato ostile nello scenario); verificare che l'acustica dell'Enhanced
    mascheri davvero un sottomarino vicino a un mercantile.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_I04_PORTA_DI_ERCOLE.lua` e il pacchetto `.m688m`, con le
  mine dell'IA già posate (chiave `mine`). Semplificazioni:
  - il campo è di cinque Mk 60 CAPTOR della parte americana, da nord a sud nel canale a 35,97 N 5,60 W (fondali fra 250
    e 850 m); il sonarista le annuncia con un messaggio di bordo proprio quando il giocatore entra nelle 3 nm;
  - l'Alfa parte a 35,95 N 5,30 W e Cadice è un cerchio di 10 km a 36,0 N 6,2 W, entro 3 ore: a 15-16 nodi, cioè sotto
    la portacontainer che passa a sedici nodi verso ovest, ci si arriva; oltre i 20 nodi la condizione «sopra N nodi»
    fa entrare il P-3C da Rota con un cue sul canale;
  - nel classico le mine non ci sono: resta la corsa sotto la portacontainer.

### LIB-I05 — «Il disertore»

- **Battello:** 688.
- **Teatro:** Canale d'Otranto, dallo Ionio verso Brindisi.
- **Data:** 22 aprile 1989, 03:15Z (notte).
- **Meteo:** generato, primavera: calmo.
- **Situazione:** crisi. ROE: fuoco solo se le unità sovietiche aprono il fuoco per prime.
- **Premessa:** Alle due di notte una nave idrografica sovietica della 5ª Eskadra, la «Ekvator», ha lasciato
  l'ancoraggio dello Ionio senza ordini e ha messo la prua su Brindisi. Il suo comandante ha chiesto asilo per sé e per
  trentotto uomini, via radio, in chiaro, sulla frequenza internazionale. Mosca ha risposto mandandole dietro una
  Krivak II e chi sa cos'altro. Il Los Angeles è a venti miglia. Washington vuole la Ekvator nelle acque italiane, ma
  non vuole una guerra per arrivarci.
- **Obiettivi:**
  - primari: la Ekvator raggiunge le acque territoriali italiane, 12 nm da Brindisi (proteggere più raggiungere,
    seguendola); nessuna arma contro unità sovietiche finché non sparano;
  - secondari: individuare il sottomarino sovietico prima che lanci.
- **Svolta:** la Krivak non è la vera minaccia. Un Kilo appostato sulla rotta ha l'ordine di affondare la nave e farlo
  passare per un incidente. Quando lancia, le ROE diventano «armi libere» per sessanta secondi di decisione: affondare il
  Kilo, o deviare il siluro con le esche e lasciare la parola ai diplomatici.
- **Forze:**
  - URSS: la Ekvator (mercantile, schieramento URSS, non combattente, rotta per Brindisi), 1 Krivak II all'inseguimento,
    1 Kilo appostato e nascosto, 1 Ka-25 dalla Krivak;
  - NATO: una Perry italiana di riserva, che entra a metà strada.
- **Regole:**
  - al lancio del Kilo (nuova condizione «un'unità ostile ha lanciato») le ROE passano a guerra;
  - entrata nelle 12 nm: esito «asilo concesso»;
  - affondamento della Ekvator: esito di fallimento.
- **Che cosa la rende diversa:** si scorta una nave «nemica» contro i suoi; la decisione di sparare pesa sulla carriera.
- **Difficoltà:** 3. **Durata:** 1,5-2 ore.
- **Ispirata a:** inedita. Le diserzioni via mare sono un tema dell'epoca, ma qui nessun fatto preciso.
- **Serve al motore:**
  - c'è: esche, siluri dell'IA, carriera (ROE rispettate);
  - manca: condizione di regola «un'unità ostile ha lanciato» (oggi c'è solo «il giocatore ha lanciato»); cambio delle
    ROE da una regola (con la radio: richiesta E6).

### LIB-I06 — «Il ponte di Ekaterina»

- **Battello:** Alfa.
- **Teatro:** Kunashir, Stretto di Ekaterina e Mare di Okhotsk.
- **Data:** 30 ottobre 1989, 06:50Z (alba).
- **Meteo:** realistico, autunno del Pacifico: mare 5, pioggia.
- **Situazione:** guerra. ROE: armi libere.
- **Premessa:** Il Mare di Okhotsk è il bastione del Pacifico: dentro, i lanciamissili; sulla porta, le Curili. Gli
  americani lo sanno e mandano due sottomarini d'attacco a forzare lo Stretto di Ekaterina, coperti da un Orion da
  Misawa. L'Alfa è l'unico battello abbastanza veloce da arrivare in tempo. Deve tenere lo stretto: niente deve
  entrare. Ma chi tiene una porta non può inseguire chi scappa, e chi insegue lascia la porta aperta.
- **Obiettivi:**
  - primari: nessun sottomarino della NATO entra nel cerchio del bastione (proteggere una zona: nuovo tipo); affondare
    almeno un Los Angeles;
  - secondario: sopravvivere senza danni gravi.
- **Svolta:** a novanta minuti la radio annuncia un gruppo da battaglia americano a sud-est, con una Nimitz e la sua
  scorta, diretto verso le Curili. L'ordine cambia: «priorità alla portaerei». Lasciare lo stretto per la gloria, o
  restare a tenerlo come da ordini? La carriera premia entrambe le scelte in modo diverso.
- **Forze:**
  - NATO: 2 Los Angeles (uno nascosto), 1 P-3C; di riserva il gruppo della Nimitz con 2 Spruance, 1 Kidd e un SH-3H;
  - URSS: 1 Il-38 amico; la boa SOSUS sovietica sul fondo (i cue al giocatore).
- **Regole:** i due Los Angeles a ondate (il secondo dopo il primo affondamento o dopo 60 minuti); il gruppo della
  Nimitz entra a 90 minuti con il messaggio radio.
- **Che cosa la rende diversa:** difesa di una zona contro più attaccanti, e una scelta di priorità a metà missione.
- **Difficoltà:** 5. **Durata:** 3 ore.
- **Ispirata a:** inedita. La dottrina del bastione è reale (dossier 17 §5.6).
- **Serve al motore:**
  - c'è: ASW aereo per le due parti, riserve con regola, siluri dell'IA;
  - manca: obiettivo «zona inviolata» (nessuna unità di una parte entra nel cerchio); radio da script (richiesta E6).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_I06_PONTE_DI_EKATERINA.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - la «zona inviolata» è una regola: un Los Angeles nel bastione (25 km oltre lo stretto) chiude con l'esito 41;
  - il secondo Los Angeles entra dopo il primo affondamento, o a un'ora;
  - il gruppo della Nimitz entra a 90 minuti col FLASH «priorità alla portaerei» (radio della missione, E6); la
    priorità resta un obiettivo secondario;
  - l'Il-38 amico dà la caccia ai Los Angeles (ASW aereo per la parte del giocatore).

### LIB-I07 — «Lupo ferito»

- **Battello:** 688.
- **Teatro:** Piattaforma Celtica, dalla scarpata verso Biscaglia.
- **Data:** 9 novembre 1989, 19:20Z (notte).
- **Meteo:** realistico, burrasca d'autunno: mare 6, vento da W, raffiche.
- **Situazione:** guerra. ROE: armi libere.
- **Premessa:** Il Los Angeles ha affondato un Victor III a cento miglia da qui. L'ultimo siluro del Victor è esploso
  a duecento metri dalla poppa. Ora c'è acqua nel locale motori, la linea d'asse vibra oltre i dodici nodi e la schiera
  rimorchiata è persa. Brest è a duecentoquaranta miglia; una barriera di Krivak e un Oscar II stanno fra il battello e
  casa. E la burrasca, che a quota periscopica è una tortura, a trecento metri è l'unica alleata.
- **Obiettivi:**
  - primario: raggiungere il punto d'incontro con la fregata francese (raggiungere);
  - secondari: non oltrepassare i 12 nodi; segnalare via radio la posizione dell'Oscar (nuovo: «rapporto di contatto»
    trasmesso).
- **Svolta:** a metà rotta il sonarista sente l'Oscar aprire i portelli dei missili: sta per lanciare contro un
  convoglio a quaranta miglia. Il 688 ha due siluri e una sola possibilità. Fermarlo vuol dire avvicinarsi, rumoroso e
  lento, con la barriera dietro.
- **Forze:**
  - URSS: 2 Krivak II e un Kara in linea di barriera, Ka-27, Tu-142; l'Oscar II di riserva, che si rivela a metà rotta;
  - NATO: la fregata del punto d'incontro (una Perry).
- **Regole:** danni iniziali al battello; il «lancio» dell'Oscar a un'ora dalla rivelazione (esito «convoglio colpito»
  se non affondato); al punto d'incontro, esito di successo.
- **Che cosa la rende diversa:** si gioca con un battello menomato: danni, riparazioni, velocità limitata. E c'è una
  scelta fra salvarsi e salvare altri.
- **Difficoltà:** 4. **Durata:** 3-4 ore.
- **Ispirata a:** inedita.
- **Serve al motore:**
  - c'è: danni dei sottomarini e riparazioni, meteo realistico (burrasca), ASW aereo;
  - manca: **danni iniziali del giocatore** come chiave della missione (oggi `--urto-giocatore` è un'opzione di prova
    dell'app); dotazione ridotta (c'è); obiettivo «rapporto di contatto trasmesso» (la sala radio ha i rapporti R1:
    verificare); l'Oscar che «lancia» i missili su un bersaglio fuori carta.

### LIB-I08 — «Ombra lunga»

- **Battello:** Alfa.
- **Teatro:** Fær Øer, a nord delle isole verso il Mare di Norvegia.
- **Data:** 21 giugno 1989, 11:00Z: il giorno più lungo, luce piena.
- **Meteo:** fissato: calma piatta, mare 1, visibilità oltre 20 km.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:** La NATO porta una portaerei nel Mar di Norvegia per l'esercitazione d'estate, ed è il regalo che
  l'intelligence sovietica aspettava da due anni. L'Alfa deve infilarsi nello schermo della scorta, registrare il
  rumore delle eliche della Nimitz da meno di cinque miglia e fotografarne l'isola al periscopio. Poi deve tornare a
  casa senza che a Norfolk sappiano che è stato lì. Con il mare a specchio una testa di periscopio si vede da dieci
  miglia, e gli elicotteri a immersione non dormono mai.
- **Obiettivi:**
  - primari: arrivare entro 5 nm dalla Nimitz (raggiungere, su un'unità in moto: nuovo); fotografare la Nimitz (nuovo
    «foto»); non essere scoperti;
  - secondario: classificare tutte le scorte.
- **Svolta:** quando l'Alfa entra nello schermo, la Spruance di testa accende il sonar attivo e due SH-3H calano i
  sonar a immersione a cavallo della sua rotta: qualcuno ha passato un'informazione. Bisogna scegliere: rinunciare, o
  passare sotto lo strato termico a 30 nodi e sperare.
- **Forze:**
  - NATO: la Nimitz, 2 Spruance, 1 Kidd, 1 Perry, la Sacramento, 2 SH-3H, 1 S-3B;
  - URSS: un Tu-142 da ricognizione, che passa la posizione del gruppo.
- **Regole:** la Spruance passa all'attivo quando l'Alfa è entro 12 nm dal gruppo; con l'allarme gli SH-3H cambiano
  zona.
- **Che cosa la rende diversa:** penetrazione di uno schermo antisom vero, col mare calmo e la luce piena che rendono
  pericoloso ogni centimetro di periscopio.
- **Difficoltà:** 4. **Durata:** 2-3 ore.
- **Ispirata a:** inedita. Le avvicinate sovietiche ai gruppi portaerei sono un tema reale degli anni '80.
- **Serve al motore:**
  - c'è: periscopi con esposizione e foto, ASW aereo (sonar a immersione), strati termici;
  - in arrivo: meteo fissato;
  - manca: obiettivo «foto» di una classe; obiettivo «raggiungere» relativo a un'unità in moto.
- **Costruita il 25/10/2026:** `data/missioni/libreria/LIB_I08_OMBRA_LUNGA.lua` e il pacchetto `.m688m`. Semplificazioni,
  in attesa dei pezzi del §3:
  - la «foto» è la classificazione della Nimitz (scoprire);
  - il meteo è fissato (E6, dal 26/10/2026): calma piatta, mare 1, visibilità 40 km;
  - la Spruance «avvertita» è l'allarme a 45 minuti, che fa fare ping a tutte le navi (paranoia 2);
  - la missione si chiude al rientro nella zona di raccolta dopo l'allarme, o a tempo scaduto (3 ore).

### LIB-I09 — «Sotto il bastione»

- **Battello:** 688.
- **Teatro:** Barents, approcci a Zapadnaja Lica.
- **Data:** 15 gennaio 1989, 09:00Z: notte polare.
- **Meteo:** fissato: mare 4, neve, banchi di ghiaccio sparsi.
- **Situazione:** pace, con un rischio altissimo. ROE: armi trattenute; fuoco solo per salvare il battello.
- **Premessa:** Un satellite ha fotografato a Zapadnaja Lica una nave nuova, con una cupola di prua che nessun
  analista sa leggere: forse un sonar a bassa frequenza di una generazione nuova. Il Los Angeles deve entrare dove
  nessun sottomarino americano dovrebbe trovarsi. È la Flotta del Nord a casa sua: boe idrofoniche sul fondo,
  pattugliatori, aerei. Deve fotografare quella nave mentre esce per le prove in mare. Nessuno firmerà mai l'ordine
  che ha in mano.
- **Obiettivi:**
  - primari: fotografare la nave nuova (nuovo «foto»); uscire dalle 12 nm senza essere scoperti (non essere scoperti
    più raggiungere);
  - secondario: registrare 20 minuti del suo sonar attivo (nuovo «contatto»).
- **Svolta:** durante le prove la nave nuova accende il suo sonar a bassa frequenza, e il 688 sente il ping rimbalzargli
  addosso a trenta miglia: il sonar è davvero rivoluzionario. Da quel momento ogni minuto in zona è un rischio. Se
  qualcuno capisce, scatta la barriera: due Krivak e un Il-38 escono a chiudere l'uscita.
- **Forze:**
  - URSS: la nave nuova (uno Slava modificato, la «cupola»), 2 Krivak II di riserva, 1 Il-38, un Victor III di
    pattuglia, boe idrofoniche fisse (cue al Victor);
  - neutrali: nessuno.
- **Regole:** il sonar della nave nuova accende l'allarme se il giocatore è sopra lo strato entro 30 nm (con la
  condizione «in zona»); con l'allarme entrano i Krivak e l'Il-38 riceve un cue; l'uscita è chiusa a 90 minuti.
- **Che cosa la rende diversa:** l'intrusione più pericolosa della libreria. Notte polare, periscopio in condizioni
  limite, e un nemico che «vede» più lontano del giocatore.
- **Difficoltà:** 5. **Durata:** 3 ore.
- **Ispirata a:** inedita. Le intrusioni americane nel Mare di Barents sono reali; questo episodio no.
- **Serve al motore:**
  - c'è: periscopi (Type 18 notturno), ASW aereo, acustica, ghiaccio nella climatologia;
  - in arrivo: meteo fissato;
  - manca: un sonar attivo «speciale» con portata da scenario (una classe o un parametro); obiettivo «foto»; boe fisse
    di sorveglianza come unità dello scenario (verificare se il SOSUS dell'ASW basta).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_I09_SOTTO_IL_BASTIONE.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - la «foto» è classificare lo Slava; i 20 minuti del suo sonar sono un obiettivo «contatto» secondario;
  - il sonar nuovo è una regola: da 40 minuti, il giocatore sopra i 60 m entro 30 miglia dalle prove fa scattare
    l'allarme e un cue all'Il-38. Con l'allarme, o a 90 minuti, escono due Krivak a chiudere l'uscita;
  - le boe fisse non ci sono; il meteo è fissato (E6: mare 4, neve, visibilità 2 km);
  - con la foto, fuori dalle acque sovietiche l'esito è 40.

### LIB-I10 — «Corsa al relitto»

- **Battello:** Alfa.
- **Teatro:** Piattaforma Celtica, scarpata di Goban Spur (fondali da 2000 a 4000 m).
- **Data:** 2 agosto 1989, 16:40Z.
- **Meteo:** generato, estate: mare 2.
- **Situazione:** pace. ROE: armi trattenute; il sonar attivo è permesso.
- **Premessa:** Un sottomarino sovietico è affondato tre giorni fa sulla scarpata della Piattaforma Celtica, con i
  suoi codici e un reattore spento. Mosca dice «avaria, nessuna vittima» e ha mandato l'Alfa a sorvegliare il punto
  finché non arriva la nave di soccorso. Ma gli americani hanno sentito l'esplosione con il SOSUS molto prima del
  comunicato. Un loro sottomarino d'attacco è già sulla scarpata, e una nave «oceanografica» con bandiera di comodo
  sta arrivando da ovest con un sommergibile da grande profondità a bordo.
- **Obiettivi:**
  - primari: raggiungere il relitto (raggiungere); impedire che una unità della NATO resti entro 2 nm dal relitto per
    più di 10 minuti (nuovo «zona inviolata» col tempo); non sparare (proteggere, non colpire);
  - secondario: classificare il sottomarino americano.
- **Svolta:** la posizione data da Mosca è sbagliata di sei miglia: il relitto è più giù, sulla scarpata. Lo dice la
  radio a metà missione, «posizione corretta». Il sottomarino americano lo sapeva già. Senza armi, l'Alfa deve
  cacciarlo via con quello che ha: il sonar attivo, la velocità, il coraggio di passargli sotto la chiglia.
- **Forze:**
  - NATO: un Sturgeon, il «sommergibilista», nascosto; la nave oceanografica (un mercantile, schieramento USA, non
    combattente), un P-3C;
  - URSS: il relitto (un Charlie II fermo sul fondo, distrutto); la nave di soccorso di riserva, che entra alla fine.
- **Regole:**
  - la posizione corretta con la radio a 40 minuti, con la zona dell'obiettivo che si sposta;
  - il ping attivo dell'Alfa entro 2 nm dallo Sturgeon lo fa allontanare (regola: allarme più vai_a);
  - la nave di soccorso entra a 150 minuti: esito «il relitto è al sicuro».
- **Che cosa la rende diversa:** una missione senza armi, vinta con le manovre e il sonar attivo, nelle acque più
  profonde del gioco, dove l'Alfa dà il meglio.
- **Difficoltà:** 2. **Durata:** 2,5 ore.
- **Ispirata a:** inedita, con l'atmosfera delle corse ai relitti della Guerra Fredda. I casi storici veri (K-129,
  K-8, K-219) vanno nelle schede storiche.
- **Serve al motore:**
  - c'è: sonar attivo, unità ferme, riserve;
  - manca: un relitto (unità affondata e immobile sul fondo, rilevabile al sonar attivo); obiettivo «zona inviolata»
    col tempo; condizione «il giocatore ha fatto ping entro N nm da un'unità»; spostare una zona da una regola.
- **Costruita il 25/10/2026:** `data/missioni/libreria/LIB_I10_CORSA_AL_RELITTO.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - il relitto è un Charlie II fermo a 400 m (la quota massima della classe), non sul fondo;
  - la «posizione corretta» è nel briefing: Mosca dà un punto incerto di sei miglia, finché non c'è la radio da script;
  - la «zona inviolata» è fatta con le regole. Col presidio dell'Alfa sul relitto la nave oceanografica devia appena
    entra nelle 6 nm, e lo Sturgeon si allontana se c'è l'allarme. Se la nave arriva sul relitto, la missione fallisce
    (esito 41); col soccorso a 150 minuti si chiude con l'esito 40.

---

## 2b. Soggetti ispirati a fatti reali

Dal dossier `docs/ricerca/23_episodi_guerra_fredda.md`, citato come «E..» per episodio e «P..» per schema. Valgono le
sue regole di rispetto (§5):
- battelli e persone sono sempre di fantasia;
- nessuna morte è un obiettivo o uno spettacolo: nei soccorsi si contano i salvati;
- gli episodi contesi restano aperti;
- le guerre altrui entrano solo come dottrina.

Ogni briefing ha una «Nota storica» facoltativa, al passato, con la frase «ispirata a; nomi e unità sono di fantasia».

**La scelta:** dodici soggetti fra i 23 della rosa del dossier (§6), sei per lato.
- Scartati perché faranno doppione con le inedite già approvate: *Atrina* (I01), *Coda alla portaerei* (I08), *Linea di
  base* (I09), *La porta di Spartel* e *Seminare la soglia* (I04), *Ombra del boomer* e *Pietra Santa* (I01, I09),
  *Eskadra* (I08).
- Rinviati, perché chiedono troppi pezzi nuovi: *Arcipelago* e *Nebbia nel fiordo* (M2, M5, M7), *Fuori dalla zona*
  (M4).

Ci sono le difficoltà 1 e 2 d'ingresso.

### LIB-S01 — «Vestfjord»

- **Battello:** 688.
- **Teatro:** Fær Øer, un fiordo delle isole al posto del Vestfjord norvegese [SOST].
- **Data:** 14 settembre 1988, 08:00Z.
- **Meteo:** generato, fine estate: mare 2.
- **Situazione:** esercitazione (pace). ROE: armi d'esercitazione libere contro l'«arancione».
- **Premessa:** Northern Wedding '88. La NATO vuole provare un'idea nuova: nascondere una portaerei in un fiordo, dove
  i radar sovietici non la vedono e i missili non la trovano. Prima che entri, qualcuno deve bonificare il fiordo. Il
  Los Angeles ha quattro ore. Da qualche parte fra le pareti di roccia c'è un Trafalgar britannico che fa la parte del
  sovietico, l'«arancione», con l'ordine di aspettare la portaerei all'imboccatura. È una giornata di scuola, se tutto
  va bene: il fiordo è stretto, il fondale sale in fretta e il Trafalgar conosce il posto meglio di voi.
- **Obiettivi:**
  - primari: classificare l'«arancione» (scoprire) e «colpirlo» prima che la portaerei arrivi all'imboccatura;
  - secondari: non toccare mai il fondo; nessun colpo contro le navi amiche del gruppo.
- **Svolta:** a metà missione la portaerei anticipa di trenta minuti. C'è un messaggio a bordo, e da quel momento la
  sua scorta entra nel fiordo e riempie l'acqua di rumore.
- **Forze:**
  - NATO: il gruppo che entra (Nimitz, 2 Spruance, Perry), di riserva, con l'ingresso a tempo;
  - l'«arancione»: un Trafalgar, schieramento URSS per l'esercitazione, fermo vicino all'imboccatura, col pedinamento
    se sente il 688.
- **Che cosa la rende diversa:** è la missione d'ingresso della libreria, una scuola di caccia in acque strette, con un
  nemico che non spara davvero.
- **Difficoltà:** 1. **Durata:** 1-1,5 ore.
- **Ispirata a:** E30, le esercitazioni NATO e la portaerei nascosta nei fiordi (Lehman, *Oceans Ventured*).
- **Serve al motore:**
  - c'è: tutto, cioè zone, riserve, pedinamento;
  - manca: il siluro da esercitazione (richiesta 10). Finché non c'è, l'«arancione» affondato vale come «colpito» e il
    debriefing lo dice.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S01_VESTFJORD.lua` e il pacchetto `.m688m`. Semplificazioni:
  - l'«arancione» affondato vale come colpito (manca il siluro da esercitazione); classificarlo è un obiettivo a parte;
  - il fondo toccato lo registra il debriefing, ma non è un obiettivo;
  - la svolta: a 45 minuti un FLASH (radio della missione, E6) annuncia che la portaerei anticipa; il gruppo compare al
    largo e arriva all'imboccatura verso le 0915Z, mezz'ora prima del previsto (0945Z); se l'arancione è ancora vivo,
    esito 41;
  - il mare 2 è quello del meteo generato.

### LIB-S02 — «Nave mineraria»

- **Battello:** 688.
- **Teatro:** Kunashir, bacino delle Curili oltre i 3000 m, al posto del Pacifico centrale [SOST].
- **Data:** 4 luglio 1989, 05:30Z (alba).
- **Meteo:** realistico, estate del Pacifico: nebbia a banchi.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:** La «Deep Harvest» sta ferma da nove giorni sul bacino più profondo delle Curili. Ufficialmente cerca
  noduli di manganese con un braccio d'acciaio lungo cinque chilometri. Ufficialmente. Il Los Angeles la protegge senza
  poter dire da che cosa, né perché: gli ordini dicono soltanto che nessuno deve vedere che cosa sale dal fondo.
  All'alba all'orizzonte compare una nave di misura sovietica, e dietro di lei un rimorchiatore che non ha nessun motivo
  di essere lì.
- **Obiettivi:**
  - primari: proteggere la Deep Harvest (proteggere); non farsi scoprire; nessuna arma;
  - secondario: classificare le unità sovietiche.
- **Svolta:** proprio mentre il braccio sta per rientrare a bordo, il rimorchiatore si porta a duecento iarde e ci resta.
  Se il 688 si fa sentire, arriva un Victor III a vedere che cosa protegge.
- **Forze:**
  - NATO: la Deep Harvest (mercantile fermo, non combattente);
  - URSS: la nave di misura e il rimorchiatore (mercantili di riserva, a tempo), un Krivak II di riserva, un Victor III
    di riserva (con l'allarme).
- **Che cosa la rende diversa:** la guardia a una nave civile ferma, senza nemici da combattere: si vince stando zitti.
- **Difficoltà:** 2. **Durata:** 2 ore (dodici ore di guardia compresse).
- **Ispirata a:** E03, il progetto Azorian e la nave sovietica a duecento iarde (CIA, *The Glomar Explorer*). Nessun nome
  reale; il battello affondato non c'è.
- **Serve al motore:** c'è tutto (riserve a tempo, allarme, proteggere senza colpire). Il «sollevamento» è una variabile
  a tempo.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S02_NAVE_MINERARIA.lua` e il pacchetto `.m688m`.
  - All'alba (15 minuti) entrano la nave di misura e il rimorchiatore, a 40 minuti la Krivak, a un'ora il rimorchiatore
    si porta a duecento iarde; con l'allarme entra il Victor. A 1 h 50' il braccio è a bordo: esito 40, e il giudizio
    lo danno gli obiettivi.
  - Il Los Angeles parte di guardia a 14 km dalla Deep Harvest, fuori dal giro della scorta. Nel classico il sonar è
    quello dell'originale, e un battello fermo ha ancora il suo rumore: una Krivak lo sente anche da lontano.

### LIB-S03 — «Quarantena»

- **Battello:** 688.
- **Teatro:** Piattaforma Celtica, piana abissale, al posto del Mar dei Sargassi [SOST].
- **Data:** 27 ottobre 1989 (crisi di fantasia), 13:00Z.
- **Meteo:** realistico, autunno: mare 4.
- **Situazione:** crisi. ROE: vietato lanciare; si segnala col sonar attivo.
- **Premessa:** Il blocco navale è in vigore da quattro giorni e Mosca ha mandato verso sud quattro sottomarini diesel.
  Washington non vuole affondarli: vuole che emergano, davanti a tutti, con la bandiera. Il Los Angeles deve agganciarne
  uno e non lasciarlo più. Un diesel in immersione vive della sua batteria, e la batteria finisce. Il comandante
  sovietico lo sa meglio di voi, e farà di tutto perché finisca il più tardi possibile.
- **Obiettivi:**
  - primari: tenere il contatto col diesel finché emerge (contatto); nessuna arma;
  - secondario: ritrovare il secondo diesel, segnalato una volta sola.
- **Svolta:** il diesel lancia falsi bersagli e fa uno scatto a tutta velocità, che vuol dire una batteria che cala più
  in fretta. Poi c'è il secondo battello, «mai confermato», che compare e sparisce: è lì per davvero, o è un'eco?
- **Forze:**
  - NATO: 2 Perry, un P-3C in crisi (solo rapporti);
  - URSS: un Foxtrot che cerca di sfuggire e un secondo Foxtrot nascosto.
- **Che cosa la rende diversa:** la caccia fino all'esaurimento, senza colpi, con i segnali non letali.
- **Difficoltà:** 3. **Durata:** 3 ore.
- **Ispirata a:** E02, i Foxtrot dell'operazione Kama nel 1962 (*National Security Archive*). Il racconto controverso del
  siluro nucleare resta fuori.
- **Serve al motore:**
  - manca: batteria del diesel dell'IA che lo costringe a emergere (M6); obiettivo «contatto» (richiesta 3);
  - semplificazione possibile oggi: il Foxtrot emerge per regola a tempo se il giocatore gli è rimasto vicino (variabile
    di presidio).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S03_QUARANTENA.lua` e il pacchetto `.m688m`, con la batteria
  dei diesel dell'IA (M6):
  - dopo quattro giorni di caccia il Foxtrot ha la batteria al 6% e non lancia. Col modello del runtime un diesel in
    caccia si trascina a 2 nodi, e la batteria piena gli durerebbe 175 ore: per un'emersione dentro le 3 ore la carica
    deve partire quasi a terra. Il briefing lo dice;
  - sotto il 30% il Foxtrot va a snorkel e ricarica, ma non se è in caccia, cioè se un ostile ne conosce la posizione
    da meno di 10 minuti. Il giocatore che lo pinga e lo tiene lo costringe a emergere a 0,05 (evento
    `emersione_forzata`); con 5 minuti di contatto, esito 40. La caccia è di chiunque sia ostile, P-3C compreso: se
    nessuno lo tiene per dieci minuti, va a snorkel e la missione si allontana;
  - le due Perry seguono solo la loro rotta: col comportamento R_nave una si portava sopra il Foxtrot in caccia e lo
    speronava quando emergeva;
  - a un'ora il Foxtrot accosta verso ovest (la svolta);
  - il secondo Foxtrot (60%) è nascosto a sud-ovest: la radio lo segnala una volta a 90 minuti, con un contatto del P-3
    a 8 km d'errore;
  - nel classico la batteria non c'è: se il giocatore ha tenuto il Foxtrot almeno due ore, a 2 h 30' una regola lo fa
    «emergere» (esito 40);
  - niente falsi bersagli; i quattro sottomarini della premessa sono due.

### LIB-S04 — «Campane d'edera»

- **Battello:** 688.
- **Teatro:** Kunashir, dallo Stretto di Ekaterina verso il Mare di Okhotsk [SOST per il sito del cavo].
- **Data:** 20 ottobre 1989, 02:00Z (notte).
- **Meteo:** realistico, autunno del Pacifico: peggiora durante la missione.
- **Situazione:** pace. ROE: armi trattenute, fuoco solo per salvare il battello.
- **Premessa:** Sul fondo del Mare di Okhotsk, a centoventi metri, corre un cavo telefonico militare che i sovietici
  credono al sicuro dentro il loro mare. Da anni un dispositivo americano lo ascolta, e ogni tanto qualcuno deve
  cambiarne le registrazioni. Questa volta tocca al Los Angeles. Bisogna passare le Curili senza farsi sentire, posarsi
  quasi sul fondo sopra il cavo, restare due ore immobili mentre i palombari lavorano, poi uscire. La previsione dice
  tempo in peggioramento.
- **Obiettivi:**
  - primari: raggiungere il punto del cavo; restarci due ore (tempo cumulato nella zona, con una regola che conta);
    uscire dal Mare di Okhotsk senza essere scoperti;
  - secondario: classificare le unità che passano.
- **Svolta:** a metà del lavoro una nave sovietica arriva sul punto e si ferma, proprio sopra il cavo. Coincidenza o
  sospetto? Restare fermi sotto di lei, o abbandonare il lavoro?
- **Forze:**
  - URSS: una Krivak II nello stretto, un Il-38, un Ka-27 di pattuglia; una nave di riserva che si ferma sul cavo (a
    tempo), un Victor III di riserva (con l'allarme).
- **Che cosa la rende diversa:** la missione «da ladro»: entrare, restare fermi, uscire.
- **Difficoltà:** 5. **Durata:** 4 ore.
- **Ispirata a:** E08, Ivy Bells (*Blind Man's Bluff*). Nessun nome reale.
- **Serve al motore:**
  - oggi: il tempo nella zona si conta con una regola ripetuta che incrementa una variabile;
  - manca: la condizione sulla velocità e sulla quota del giocatore (M2), per «immobili quasi sul fondo». Il meteo
    peggiora già col meteo realistico; servirebbe fissarlo (richiesta E6).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S04_CAMPANE_D_EDERA.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - le due ore si contano con una regola ripetuta: una variabile cresce ogni 10 s col giocatore nella zona del cavo
    (720 controlli), senza la condizione su velocità e quota. L'obiettivo «variabile» (nuovo nell'editor) lo giudica;
  - la nave sovietica arriva sul cavo dopo un'ora di lavoro, si ferma e riparte dopo quaranta minuti. Dal 3/11/2026 parte a
    10 km ed entra a 28 minuti: prima partiva a 20 km e ripartiva prima di arrivare (3890 s contro 2400);
  - l'uscita conta solo a lavoro finito (un'altra variabile); la durata è di 4 h 30', perché il transito e le due ore
    non stanno in quattro;
  - il meteo è il realistico, non fissato.

### LIB-S05 — «Il guardiano del cavo»

- **Battello:** Alfa.
- **Teatro:** Kunashir, Mare di Okhotsk.
- **Data:** 22 ottobre 1989, 06:00Z.
- **Meteo:** realistico, dopo la burrasca.
- **Situazione:** pace, che può diventare crisi. ROE: armi trattenute; cambiano con la radio.
- **Premessa:** Un'informazione arrivata da molto lontano dice che sul cavo di Okhotsk c'è qualcosa che non dovrebbe
  esserci. Il comando manda una nave di soccorso a «ispezionare il cavo». L'Alfa la scorta e deve tenere lontani gli
  intrusi. Ma l'intruso, se c'è, non si muove: un sottomarino fermo sul fondo è il silenzio più perfetto che il mare
  conosca, e nessun sonar passivo lo sente.
- **Obiettivi:**
  - primari: proteggere la nave di soccorso fino al punto del cavo; scoprire l'intruso;
  - secondario: costringerlo ad andarsene senza sparare.
- **Svolta:** l'intruso c'è davvero, fermo a cento metri da dove la nave sta calando. Si trova solo col sonar attivo.
  Quando lo trovate scappa, e la radio cambia gli ordini: «costringerlo a emergere», oppure «lasciarlo andare».
- **Forze:**
  - URSS: la nave di soccorso (mercantile, con rotta), un Ka-27;
  - NATO: un Los Angeles fermo vicino al cavo, nascosto, che fugge col pedinamento rovesciato.
- **Che cosa la rende diversa:** è l'altra faccia di S04, con la ricerca col sonar attivo di un nemico immobile.
- **Difficoltà:** 3. **Durata:** 2 ore.
- **Ispirata a:** E08, il lato sovietico. Quando e come i sovietici scoprirono il dispositivo resta incerto (nota
  storica).
- **Serve al motore:**
  - c'è: unità ferma (silenziosa), sonar attivo;
  - in arrivo: cambio delle ROE e radio da regola (E6);
  - manca: la condizione «ping entro N nm» (richiesta 6), per la fuga dell'intruso.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S05_GUARDIANO_DEL_CAVO.lua` e il pacchetto `.m688m`, con E6 e
  le rapide v2:
  - il Los Angeles è fermo quasi sul fondo a 400 m dal punto del cavo; un ping del giocatore entro 3 km
    (condizione «ping vicino») lo fa fuggire allo Stretto di Ekaterina a 20 nodi;
  - al ping la radio della missione manda il FLASH «costringerlo ad andarsene», che porta le ROE dei sottomarini a
    «stretto». L'alternativa «lasciarlo andare» della scheda resta fuori: la radio dà un ordine solo;
  - uscito l'intruso dallo stretto, l'esito è 40; persa la nave di soccorso, 41;
  - un Ka-27 dà la caccia all'intruso (ASW aereo per la parte del giocatore).

### LIB-S06 — «Petroliere»

- **Battello:** 688.
- **Teatro:** Gibilterra, lo stretto di notte, al posto del Golfo Persico [SOST].
- **Data:** 24 luglio 1989 (crisi di fantasia), 23:00Z.
- **Meteo:** generato, estate: levante.
- **Situazione:** crisi. ROE: fuoco solo su ordine.
- **Premessa:** Tre petroliere, uno stretto, una guerra che nessuno ha dichiarato. Il Los Angeles scorta da sotto un
  convoglio di petroliere con la bandiera nuova, mentre le fregate lo scortano da sopra. Di notte un «mercantile» senza
  luci incrocia la rotta del convoglio: si ferma, riparte, si ferma di nuovo. Nessuno ammette di avere mine in quel
  mare. Eppure.
- **Obiettivi:**
  - primari: proteggere le tre petroliere; scoprire il posamine;
  - secondario: «riferire», cioè restare in contatto finché arriva l'ordine. Non sparare prima.
- **Svolta:** la prima petroliera urta una mina, l'unica posata prima della scorta. L'ordine arriva solo dopo:
  «intercettare il posamine». Le mine appena posate sono ancora fra voi e lui.
- **Forze:**
  - NATO: 3 petroliere, 2 Perry, un SH-60B;
  - avversario di fantasia: il posamine (mercantile, parte ostile), motoscafi non rappresentati, e le mine.
- **Che cosa la rende diversa:** una scorta in crisi contro un nemico che nega, con le mine come terreno di gioco.
- **Difficoltà:** 3. **Durata:** 2,5 ore (3 nella missione costruita).
- **Ispirata a:** E26, la guerra delle petroliere (1987-1988). Nessuna bandiera reale di quei paesi.
- **Serve al motore:**
  - manca: mine dell'IA posate durante la missione (richiesta 8, estesa: posa da una nave); il sonar di scoperta delle
    mine del 688 (BQS-15).
  - in arrivo: l'ordine radio che cambia le ROE (E6).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S06_PETROLIERE.lua` e il pacchetto `.m688m`, con le mine
  dell'IA (posate e da posare) ed E6. Semplificazioni:
  - le petroliere vanno a 12 nodi da 36,05 N 6,30 W verso est; una Mk 67 è già sul primo tratto della loro rotta (a mezz'ora
    dalla partenza, su un fondale fra 40 e 70 m, perché il raggio di scoppio è di 60 m), e le scorte e il Los
    Angeles passano a un chilometro di lato;
  - il «mercantile» aspetta fermo e senza luci a sud della rotta. Quando la prima petroliera è colpita riparte a 10
    nodi e posa tre Mk 67, una ogni 2 minuti, sulla rotta delle altre due, sei chilometri dopo la svolta di 36,06 N
    6,10 W: il giocatore ha circa mezz'ora per fermarlo;
  - l'ordine FLASH «intercettare» arriva con la condizione `se unita_colpita` sulla prima petroliera e porta le ROE dei
    mercantili a «stretto»; supera quello di riserva delle 1 h 30' («disregard»), che nel classico, senza mine, è il
    solo;
  - il posamine fuori gioco, o la terza petroliera fuori dallo stretto, chiudono con l'esito 40; la durata è di 3 ore
    perché il convoglio arrivi in fondo;
  - niente motoscafi; le mine si scoprono per gli eventi. Il BQS-15 del runtime (ordine `bqs15`) c'è, ma il giocatore
    non ha ancora il comando nelle stazioni: quando ci sarà, la missione lo usa senza cambiare.

### LIB-S07 — «Tappo»

- **Battello:** 688.
- **Teatro:** Barents, il canale d'uscita del Golfo di Kola.
- **Data:** 2 febbraio 1989, 03:00Z (notte polare).
- **Meteo:** realistico, inverno: neve, mare 4.
- **Situazione:** guerra. ROE: armi libere.
- **Premessa:** La guerra comincia all'alba. Se il canale di Kola resta aperto, entro sera i sottomarini lanciamissili
  saranno nei loro bastioni. Il Los Angeles porta dieci mine Mk 67 e un ordine: chiudere il tappo. Bisogna arrivare
  all'imboccatura del golfo, lanciare le mine lungo il canale da otto miglia di distanza, e andarsene prima che si
  armino e che la Flotta del Nord si svegli.
- **Obiettivi:**
  - primari: posare almeno 6 mine nel canale (minare, nel cerchio); lasciare la zona entro due ore;
  - secondario: non farsi scoprire durante la posa.
- **Svolta:** mentre i tubi si ricaricano, un Akula esce dal canale. Lasciarlo passare, e lui avvertirà la base, o
  attaccarlo e svegliare le Krivak di guardia?
- **Forze:**
  - URSS: 2 Krivak II e un Kara di guardia, un Il-38; un Akula di riserva che esce a tempo.
- **Che cosa la rende diversa:** il minamento offensivo, con il tempo e le proprie mine contro.
- **Difficoltà:** 4. **Durata:** 2-2,5 ore.
- **Ispirata a:** inedita sullo schema P07, con l'idea del porto minato di E09 (come dottrina).
- **Serve al motore:** c'è tutto (mine Mk 67, minare, riserve). Funziona nell'Enhanced.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S07_TAPPO.lua` e il pacchetto `.m688m`. Le dieci Mk 67 sono
  nel deposito dell'Enhanced; nel classico le mine non ci sono, e la missione si gioca ma il campo non si posa.
  - Il campo è un cerchio di 1500 m all'imboccatura, su fondali fra 30 e 90 m. L'Akula esce dal golfo a 45 minuti: se
    arriva al mare aperto, o se è colpito, scatta l'allarme. Al punto di ritirata, dopo mezz'ora, esito 40.
  - Le prove l'hanno giocata nell'Enhanced: sei Mk 67 posate nel campo, poi la ritirata, esito 40 e successo. Hanno
    trovato un difetto del motore: i messaggi di bordo delle mine (364-369) passavano da `Missione::messaggio` e la
    prima mina posata chiudeva la missione con l'esito 267. Il runtime l'ha corretto il 26/10/2026.

### LIB-S08 — «Valona»

- **Battello:** Alfa.
- **Teatro:** Otranto, baia di Valona e Pasha Liman (dentro il teatro così com'è).
- **Data:** 26 maggio 1989 (trasposizione di fantasia del 1961), 05:00Z.
- **Meteo:** generato, primavera: calmo.
- **Situazione:** crisi. ROE: non sparare a nessuno.
- **Premessa:** Il paese ospite ha rotto con Mosca, e la base di Pasha Liman è finita. Le truppe del porto non lasciano
  partire tutti: quattro sottomarini restano ai moli, «trattenuti». L'Alfa deve portare fuori dalla baia la nave
  appoggio e gli altri quattro battelli, uno dopo l'altro, nel canale stretto. Fuori, nel Canale d'Otranto, un 688 e un
  Orion della NATO guardano. Nessuno vuole sparare, e tutti hanno le armi pronte.
- **Obiettivi:**
  - primari: la nave appoggio e i quattro Foxtrot escono dalla baia (proteggere più raggiungere l'uscita); nessuna arma;
  - secondario: classificare il 688 della NATO.
- **Svolta:** a metà uscita un Foxtrot rallenta e accosta verso i moli: il suo comandante non vuole lasciare la baia. L'Alfa
  deve raggiungerlo e «convincerlo» con la sua presenza (regola: il giocatore nella sua zona lo fa tornare in convoglio).
- **Forze:**
  - URSS: la nave appoggio (mercantile), 4 Foxtrot in colonna;
  - il paese ospite: 4 Foxtrot «trattenuti» (neutrali, fermi);
  - NATO: un Los Angeles, un P-3C in crisi.
- **Che cosa la rende diversa:** una crisi senza nemico dichiarato, ed è anche la più facile delle missioni dell'Alfa.
- **Difficoltà:** 2. **Durata:** 1,5 ore.
- **Ispirata a:** E01, Valona 1961 (fatti certi: la ritirata e i quattro battelli trattenuti). «What if» al 1989.
- **Serve al motore:**
  - c'è: zone, regole, unità ferme;
  - manca: una parte «terza», ostile all'URSS ma non alla NATO (M7). Oggi i trattenuti sono neutrali.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S08_VALONA.lua` e il pacchetto `.m688m`. Semplificazioni:
  - i quattro trattenuti sono neutrali, fermi ai moli (manca la parte terza);
  - la colonna esce in superficie a 12 nodi; ogni unità fuori dalla baia conta una variabile, e a cinque l'esito è 40;
  - a 20 minuti il terzo Foxtrot torna verso i moli e si ferma: l'Alfa che entra nella sua zona lo riporta in colonna.

### LIB-S09 — «Aport»

- **Battello:** Alfa.
- **Teatro:** Fær Øer, la soglia Islanda-Fær Øer.
- **Data:** 18 maggio 1985 (trasposizione), 21:00Z.
- **Meteo:** realistico, primavera.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:** La Flotta del Nord manda fuori insieme cinque sottomarini d'attacco, per trovare dove pattugliano i
  lanciamissili americani e per vedere quanti contatti la NATO riesce a seguire nello stesso momento. L'Alfa è il più
  veloce, e il più rumoroso quando corre. Deve passare la soglia sotto il SOSUS. Ogni scatto oltre i venti nodi è un
  rapporto che parte per Keflavík, e un Orion che arriva un'ora dopo sul punto. Si gioca a scatti e derive, sotto lo
  strato, aspettando che il datum invecchi.
- **Obiettivi:**
  - primario: raggiungere l'area di pattugliamento a sud della soglia senza un fix (raggiungere più non essere scoperti);
  - secondario: arrivare entro 4 ore.
- **Svolta:** a metà soglia un Victor III del gruppo viene scoperto e gli Orion lo martellano. Restare in silenzio, o
  scattare mentre la NATO guarda altrove?
- **Forze:**
  - NATO: 2 P-3C coi cue SOSUS, ritardo 30-60 minuti; una Perry con la schiera;
  - URSS: un Victor III amico, che viene scoperto a tempo.
- **Che cosa la rende diversa:** l'infiltrazione di una barriera di sorveglianza, contro il tempo del datum.
- **Difficoltà:** 4. **Durata:** 4 ore.
- **Ispirata a:** E21, l'operazione Aport (1985).
- **Serve al motore:**
  - c'è: datum SOSUS col ritardo, P-3C, strati termici;
  - manca: la condizione sulla velocità del giocatore (M2), per il cue a ogni scatto. Oggi si fanno cue a tempo sulla
    rotta probabile.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S09_APORT.lua` e il pacchetto `.m688m`. Semplificazioni:
  - i cue SOSUS sono a tempo (1, 2 e 3 ore) sulla rotta probabile, senza la condizione sulla velocità;
  - a 1 h 30' il Victor è scoperto: un cue sulla sua posizione stimata e un secondo P-3C che gli dà la caccia;
  - nell'area di pattugliamento, esito 40. La durata è di 4 h 30' per dare senso al secondario «entro 4 ore».

### LIB-S10 — «Il cavo della fregata»

- **Battello:** Alfa.
- **Teatro:** Piattaforma Celtica, mare aperto, al posto del largo delle Bermuda [SOST].
- **Data:** 31 ottobre 1989, 15:00Z.
- **Meteo:** generato, autunno.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:** La fregata americana trascina un sonar nuovo, una schiera lunga quasi un chilometro alla fine di un
  cavo d'acciaio. Mosca ne vuole la firma e, se possibile, un pezzo. L'Alfa deve infilarsi nel cono sordo dietro la
  poppa della Perry, dove il suo sonar di scafo non sente niente, e restarci registrando. Senza avvicinarsi troppo:
  là dietro c'è il cavo, e un'elica che lo prende è un battello che non torna a casa da solo.
- **Obiettivi:**
  - primari: restare nel settore di poppa della Perry per 30 minuti (contatto); non toccare il cavo; non farsi scoprire;
  - secondario: classificare il gruppo.
- **Svolta:** la Perry accosta di colpo, il suo Crazy Ivan: il cono sordo si sposta e il cavo spazza l'acqua dove era
  l'Alfa.
- **Forze:**
  - NATO: una Perry con la schiera, una seconda Perry a 6 miglia, un SH-60B.
- **Che cosa la rende diversa:** il pedinamento a distanza ravvicinata di una nave ASW, con un ostacolo fisico che si
  muove.
- **Difficoltà:** 3. **Durata:** 2 ore.
- **Ispirata a:** E19, K-324 e la schiera della *McCloy* (1983). Nessun nome reale.
- **Serve al motore:**
  - manca: il cavo della schiera come ostacolo che si impiglia nell'elica (collisioni, dossier 16); obiettivo
    «contatto» col settore relativo (richiesta 3).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S10_CAVO_DELLA_FREGATA.lua` e il pacchetto `.m688m`, con il
  cavo della schiera dell'Enhanced:
  - la Perry fila la SQR-19 (quella dei modelli dell'originale) verso nord-est a 10 nodi; a un'ora fa il suo Crazy Ivan
    con un'accostata di 100° a 18 nodi;
  - la condizione «vicino al cavo» (`m688.distanza_cavo`) sotto i 50 m chiude con l'esito 41 (fallimento): sotto i 25 m il
    cavo resta pochi secondi prima di impigliarsi, e le regole si controllano ogni 10 s. 30 minuti di
    contatto con la Perry chiudono con il 40;
  - il «settore di poppa» non si controlla: contano i 30 minuti di contatto;
  - nel classico il cavo non c'è.
- **Il cavo vero (30/10/2026):** nelle collisioni il cavo a numeri fissi (360 m) lascia il posto alla forma della schiera
  del modello (`nucleo/schiera`, docs/ricerca/33; disassemblatore). Con lo sprint and drift della Perry (massima 15 nodi)
  l'IA accorcia la SQR-19 per la deriva a 6 nodi: la schiera sta fra 180 e 280 m, e il cavo scende in diagonale dalla
  poppa. La missione non cambia (il briefing non dà quote, la vittoria a 3 km per 31 minuti passa uguale); la prova della
  sconfitta mette l'Alfa a 150 m, sotto il cavo, invece che a 365 m, dove la schiera ora non arriva.
- **La firma da vicino e la copertura (30/10/2026):** il giro senza finestra della libreria ha mostrato che l'Alfa fermo
  alla partenza vinceva in 32 minuti: il contatto con la Perry nasceva a 4,3 nm e si accumulava, e le due Perry lo
  scoprivano senza conseguenze. Il briefing chiede il cono sordo, quindi due correzioni:
  - la firma si registra solo entro 5 km dal cavo della schiera (STIMA DI GIOCO: la distanza utile di registrazione, che
    il dossier non da'): la regola «registra» aggiunge 10 s alla variabile 4 a ogni controllo con la condizione «vicino al
    cavo», e l'obiettivo «firma» chiede 1800 s; con la schiera recuperata non si registra niente;
  - «occulto» chiude col fallimento (esito 42) quando una delle due Perry classifica l'Alfa, non al primo rilevamento: la
    condizione nuova «unità classifica» (`m688.classificato`, il sonarista dell'IA o un contatto lungo), con la confidenza
    minima POSSUB; la variabile 5, messa a 1 alla partenza, torna a 0, e l'obiettivo a variabile «occulto» fallisce;
  - il briefing lo dice: «entro 5 km dalla schiera», «se la fregata vi classifica, l'operazione e' bruciata»;
  - **la confidenza (runtime, 30/10):** `m688.classificato` guarda il terzo argomento, la confidenza minima. La
    classificazione NONSUB a 475 s della seconda Perry non chiude più; l'Alfa fermo alla partenza viene bruciato dopo,
    e non vince più: esito 42 a 665, 695 e 726 s coi semi 1, 2 e 3 (build del 06/11/2026). La prova lo controlla col seme 1.

### LIB-S11 — «Tenete il confine»

- **Battello:** Alfa.
- **Teatro:** Baltico, presso Bornholm, al posto di Karlskrona [SOST].
- **Data:** 28 ottobre 1989 (trasposizione), 07:00Z.
- **Meteo:** generato, autunno: foschia.
- **Situazione:** crisi. ROE: non sparare; vietato entrare nelle 12 miglia.
- **Premessa:** Un sottomarino diesel sovietico si è incagliato sugli scogli, dentro le acque di un paese neutrale, durante
  un'esercitazione della sua marina. Il paese non lo restituisce finché non avrà finito le sue ispezioni. Le sue navi
  gli girano attorno, e un sottomarino della NATO osserva da lontano. L'Alfa deve restare appena fuori dal confine delle
  dodici miglia, pronto e invisibile. Quando i rimorchiatori del paese lo porteranno fuori dalle acque territoriali,
  l'Alfa dovrà coprirlo fino a casa. Un passo oltre la linea, e la crisi diventa un'altra cosa.
- **Obiettivi:**
  - primari: proteggere il diesel (proteggere); mai dentro le 12 miglia (una regola chiude la missione col fallimento);
    nessuna arma;
  - secondario: classificare il sottomarino della NATO.
- **Svolta:** due eco radar sembrano avvicinarsi dalla flotta sovietica, e le navi neutrali vanno ai posti di
  combattimento. Sono due mercantili tedeschi, ma per dieci minuti nessuno lo sa.
- **Forze:**
  - URSS: il Foxtrot incagliato (fermo, poi rimorchiato con una rotta a tempo);
  - neutrali: 2 Koni e una Nanuchka, schieramento neutrale; due mercantili;
  - NATO: un Los Angeles.
- **Che cosa la rende diversa:** una crisi in cui nessuno vuole sparare e tutti hanno le armi pronte; il confine come muro.
- **Difficoltà:** 3. **Durata:** 2 ore.
- **Ispirata a:** E14, l'U-137 «Whiskey on the rocks» (1981), con un paese neutrale di fantasia. L'inchiesta e le versioni
  contrastanti restano nella nota storica.
- **Serve al motore:**
  - manca: il battello incagliato o rimorchiato (M6), oggi un'unità ferma che poi riparte; le acque territoriali con la
    penalità di carriera (M5), oggi la regola di fine; la parte terza armata (M7).
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S11_TENETE_IL_CONFINE.lua` e il pacchetto `.m688m`.
  Semplificazioni:
  - il Foxtrot «incagliato» è fermo sugli scogli; a 20 minuti parte il rimorchio, a 8 nodi verso il punto d'incontro
    (la velocità nuova dell'azione «vai a», aggiunta all'editor);
  - le acque territoriali sono una zona circolare di 12 miglia a sud di Bornholm (con le rapide v2 le zone delle regole
    sono cerchi veri): entrarci chiude con l'esito 41;
  - a 50 minuti entrano due mercantili da est e scatta l'allarme (le navi del paese neutrale fanno ping, con `R_vigila`,
    avviato dalla regola dal 3/11/2026: prima non partiva); i neutrali
    non sparano.
- **Aggiornata il 26/10/2026 con M6 del runtime** (avaria, incaglio, rimorchio): il Foxtrot parte davvero incagliato
  (`stato = "incagliata"`) e a 20 minuti un rimorchiatore del paese neutrale lo prende a rimorchio (azione «rimorchia»,
  7 nodi dal 30/10/2026, prima 6, che arrivavano a ridosso delle due ore; cavo di 300 m) e lo porta all'incontro; a rimorchio il Foxtrot è in superficie, come l'U-137. Nel classico
  la missione resta giocabile.

### LIB-S12 — «Il gruppo cacciatore»

- **Battello:** Alfa.
- **Teatro:** Otranto, acque medie vicino alla costa, al posto del largo di Diu [SOST].
- **Data:** 9 dicembre 1989 (guerra di fantasia), 19:30Z (notte).
- **Meteo:** generato, inverno: mare 3.
- **Situazione:** guerra. ROE: armi libere.
- **Premessa:** Due fregate e un elicottero vi cercano da sei ore. Hanno un datum vecchio e tanta pazienza. L'Alfa ha
  ordine di sopravvivere e, se può, di rompere la caccia. Poi una delle due fregate fa una cosa che nessuna dottrina
  consiglia: rallenta a dodici nodi per provare il suo sonar nuovo. Per qualche minuto, la preda è lei.
- **Obiettivi:**
  - primari: sopravvivere; affondare almeno una fregata;
  - secondario: uscire dalla zona di caccia.
- **Svolta:** la fregata lenta è un'occasione, o un'esca? La seconda si porta di lato a tutta forza appena l'Alfa lancia,
  e l'elicottero cala il sonar sul punto del lancio.
- **Forze:**
  - NATO: 2 Perry con un SH-60B e l'ASW dell'Enhanced. Una delle due cambia velocità a tempo (regola: vai a più
    comportamento).
- **Che cosa la rende diversa:** il sottomarino che ribalta la caccia, in una missione di guerra breve e tesa.
- **Difficoltà:** 3. **Durata:** 1,5 ore.
- **Ispirata a:** E10, *Hangor* e *Khukri* (1971), solo come dottrina: niente bandiere né navi di quei paesi.
- **Serve al motore:** c'è tutto (regole, ASW aereo, siluri dell'IA). Si può costruire subito.
- **Costruita il 26/10/2026:** `data/missioni/libreria/LIB_S12_GRUPPO_CACCIATORE.lua` e il pacchetto `.m688m`.
  - A 20 minuti la seconda fregata rallenta a 12 nodi per la prova del sonar, a 35 riprende a 20 nodi.
  - Al primo lancio dell'Alfa scatta l'allarme, la prima fregata accorre a 28 nodi (la Perry ne fa 29; 30 fino al 3/11/2026) e l'elicottero ha un cue sul punto
    della prova.
  - Con almeno una fregata affondata, uscire dalla zona dà l'esito 40.

### Riepilogo delle storiche

| Codice | Titolo | Battello | Teatro | Situazione | Diff. | Durata | Si costruisce |
|---|---|---|---|---|---|---|---|
| LIB-S01 | Vestfjord | 688 | Fær Øer | esercitazione | 1 | 1-1,5 h | oggi (semplificata) |
| LIB-S02 | Nave mineraria | 688 | Kunashir | pace | 2 | 2 h | oggi |
| LIB-S03 | Quarantena | 688 | Celtica | crisi | 3 | 3 h | costruita (M6, «contatto») |
| LIB-S04 | Campane d'edera | 688 | Kunashir | pace | 5 | 4 h | oggi (semplificata); meglio con M2 |
| LIB-S05 | Il guardiano del cavo | Alfa | Kunashir | pace → crisi | 3 | 2 h | con E6 e «ping entro N nm» |
| LIB-S06 | Petroliere | 688 | Gibilterra | crisi | 3 | 3 h | costruita (mine dell'IA, E6) |
| LIB-S07 | Tappo | 688 | Barents | guerra | 4 | 2-2,5 h | oggi |
| LIB-S08 | Valona | Alfa | Otranto | crisi | 2 | 1,5 h | oggi (semplificata) |
| LIB-S09 | Aport | Alfa | Fær Øer | pace | 4 | 4 h | oggi (semplificata); meglio con M2 |
| LIB-S10 | Il cavo della fregata | Alfa | Celtica | pace | 3 | 2 h | costruita (cavo della schiera; firma entro 5 km; classificati, bruciati) |
| LIB-S11 | Tenete il confine | Alfa | Baltico | crisi | 3 | 2 h | oggi (semplificata); meglio con M5-M7 |
| LIB-S12 | Il gruppo cacciatore | Alfa | Otranto | guerra | 3 | 1,5 h | oggi |

**Si possono costruire subito**, con le semplificazioni annotate: S01, S02, S04, S07, S08, S09, S11, S12. Più I08 e I10
già fatte, sono 10 missioni.

**Costruite il 26/10/2026** tutte e otto, con lo stesso strumento di I08 e I10: ognuna ha il file dell'editor, il
pacchetto e le prove di vittoria e sconfitta. Nella libreria le unità dell'IA partono dall'id 2, perché il motore tratta
gli id 0 e 1 come i due giocatori dell'originale. L'id 1 ha i controlli d'incaglio e, se è distrutto, dà l'esito 4.
Anche I08 e I10 sono state rinumerate così.

---

## 2c. Missioni coi missili

Approvate dall'utente il 28/10/2026 così come sono. Sono la riga «Missioni della libreria e dell'editor con missili» di
docs/21 §8; le fonti sono il dossier 27 e docs/21. Ordine di costruzione deciso dall'utente:
1. S13, subito;
2. I11, quando il runtime avrà l'IA che tira i missili;
3. I12, con l'ordine d'esecuzione non autentico rifiutato e l'obiettivo sull'esito del TLAM.

### LIB-I11 — «Il primo eco»

- **Battello:** 688 (Flight I, Harpoon UGM-84C nei tubi).
- **Teatro:** Fær Øer, dalla soglia Islanda-Fær Øer verso il Mare di Norvegia.
- **Data:** 6 marzo 1989, 10:30Z (giorno, luce bassa).
- **Meteo:** fissato: mare 4, vento da NE, visibilità 8 km a tratti.
- **Situazione:** guerra. ROE: armi libere contro le navi sovietiche; nessuna arma contro i neutrali.
- **Premessa:** Un gruppo di superficie sovietico, uno Slava con due Krivak II, scende verso sud-ovest per aprire la
  soglia ai sottomarini. Un P-3 islandese lo ha visto all'alba e ne manda la posizione ogni mezz'ora. Il 688 è a
  sessanta miglia: abbastanza vicino per un Harpoon, troppo lontano per vederlo. Sulla stessa rotta incrociano un
  traghetto norvegese e una flotta di pescherecci delle Fær Øer, che nessuno ha fatto rientrare. Il seeker dell'Harpoon
  non distingue un incrociatore da un traghetto: prende il primo eco che trova.
- **Obiettivi:**
  - primari: colpire lo Slava (affondare, almeno un colpo); nessun neutrale colpito (proteggere, `colpire = false`);
  - secondari: tirare con la scatola piccola o media, senza la grande (nuovo: la taglia della ricerca nel registro del
    lancio); non farsi trovare dalla Krivak.
- **Svolta:**
  - a quaranta minuti il gruppo accosta di sessanta gradi e si divide: lo Slava resta con una sola Krivak;
  - il rapporto del P-3 di quel momento è già vecchio quando arriva (venti minuti di catena);
  - tirare sul dato vecchio con la scatola grande vuol dire mettere il traghetto nell'area di ricerca.
- **Forze:**
  - URSS: Slava, 2 Krivak II, Ka-27 dello Slava;
  - NATO: il P-3 islandese (solo rapporti radio, fuori carta);
  - neutrali: un traghetto (Smyril Line), sei pescherecci.
- **Comportamenti e regole:**
  - il gruppo in `R_nave` sui punti, l'accostata a 40 minuti con una regola;
  - i rapporti radio del P-3 come contatti d'intelligence (`contatto`, `contatto_errore_m` 2-4 km), uno ogni 30 minuti;
  - il traghetto e i pescherecci sulla rotta del gruppo;
  - se scopre il 688, il gruppo risponde con le armi antisom: il Ka-27, gli RBU e la Krivak che si avvicina. Il P-500 è
    un antinave: lo Slava lo tira sul 688 solo se emerge (IA simmetrica, passo 4 del runtime; ritocco del 28/10/2026).
- **Che cosa la rende diversa:** il tiro oltre l'orizzonte. Si scelgono fonte del dato (rapporto o TMA), RBL o BOL,
  taglia della scatola o accensione del seeker, guardando la carta di tiro coi neutrali nell'area. La probabilità di
  successo del pannello è sulla stima, l'esito sulla verità.
- **Difficoltà:** 3. **Durata:** 1,5-2 ore.
- **Ispirata a:** inedita. Il problema del primo eco e dei neutrali è quello del dossier 27 §0.1 (punti 3-4) e di Becker
  [BECK86].
- **Serve al motore:**
  - c'è: stazione di tiro, modello del missile, fonte radio (rapporti come intelligence), carta di tiro, neutrali
    nell'area;
  - in arrivo: l'IA che tira i missili (passo 4 del runtime), qui solo sul 688 in superficie;
  - manca: l'obiettivo «taglia della scatola» (basta un dato del lancio nel registro); facoltativo, si può togliere.

### LIB-I12 — «Finestra di lancio»

- **Battello:** 688 (TLAM-C in tubo e in rastrelliera).
- **Teatro:** Barents, a nord di Capo Nord, al largo del Varangerfjord.
- **Data:** 17 gennaio 1989, 02:00Z (notte polare).
- **Meteo:** fissato: mare 3, neve, visibilità 3 km.
- **Situazione:** guerra. ROE: armi libere; TLAM solo con l'ordine d'esecuzione autenticato.
- **Premessa:** Il Los Angeles è in mare da diciannove giorni, e l'ultimo punto nave sicuro è di cinque giorni fa. In
  libreria ha due missioni TLAM contro i radar di allarme della penisola di Kola. Il comando le eseguirà nella notte, a
  un'ora che ancora non dice, e vuole i missili sul bersaglio entro dieci minuti dall'ora fissata. Il punto di lancio
  pianificato è a quaranta miglia, dentro un'area dove passano un Il-38 e una Krivak. Un punto nave col GPS o al
  periscopio costa esposizione; senza, la navigazione può sbagliare di due miglia, e il TERCOM non perdona.
- **Obiettivi:**
  - primari: i TLAM della missione ordinata sul bersaglio dentro la finestra (nuovo: obiettivo sull'esito del TLAM, dal
    registro `esito_tlam`); sopravvivere;
  - secondario: nessun lancio su un ordine non autenticato.
- **Svolta:**
  - dopo un'ora arriva un ordine d'esecuzione per la missione 0522, con un gruppo d'autenticazione che non torna (un
    inganno: `autentico = false`);
  - venti minuti dopo arriva l'ordine vero, per la 0521, con l'ora sul bersaglio anticipata di mezz'ora;
  - chi ha già lanciato sull'ordine falso ha sprecato i missili, e chi non ha fatto il punto nave arriva al limite.
- **Forze:**
  - URSS: Krivak II di pattuglia, Il-38, Ka-27 di riserva che entra all'allarme;
  - nessuna unità amica in carta.
- **Comportamenti e regole:**
  - libreria `tlam` con le due missioni, `navigazione = { sigma_iniziale_nm = 2.0 }`;
  - due messaggi radio con `esecuzione_tlam`: il falso con `autentico = false`, il vero dopo;
  - l'allarme della Krivak fa entrare il Ka-27.
- **Che cosa la rende diversa:**
  - la gestione del tempo e della posizione: arrivare al PLP, preparare i missili (15 minuti per il primo), decidere
    se un punto nave vale l'esposizione;
  - verificare l'autenticazione alla stazione radio prima di lanciare;
  - il pannello dice «VALIDA», ma è sulla posizione stimata, e il replay «verità» mostra chi ha mancato il landfall.
- **Difficoltà:** 4. **Durata:** 2,5-3 ore.
- **Ispirata a:** inedita. La catena dell'ordine e della preparazione è quella del dossier 27 §4 e di docs/21 §3.4.
- **Serve al motore:**
  - c'è: libreria TLAM ed esecuzione dalla radio, preparazione vera, navigazione incerta e punto nave, autenticazione
    alla stazione radio;
  - manca: un ordine d'esecuzione che il runtime rifiuti, o segni, se non è autentico (oggi `esecuzione_tlam` vale per
    qualunque messaggio ricevuto); l'obiettivo sull'esito del TLAM.

### LIB-S13 — «Outlaw Shark»

- **Battello:** 688 (TASM in tubo).
- **Teatro:** Otranto, Ionio settentrionale, al posto del Mediterraneo orientale [SOST].
- **Data:** 12 maggio 1987, 15:00Z (giorno).
- **Meteo:** generato, primavera: calmo.
- **Situazione:** esercitazione. ROE: tiro d'esercitazione contro la nave bersaglio; nessuna arma contro le altre.
- **Premessa:** La Sesta Flotta prova di nuovo il tiro antinave oltre l'orizzonte. Una vecchia nave rimorchiata, la
  «bersaglio», fa la parte di un incrociatore sovietico a duecento miglia, in mezzo al traffico dell'Adriatico. Il dato
  arriva da un aereo E-2 attraverso la catena di terra: venti minuti di ritardo, un aggiornamento ogni mezz'ora. Il
  Los Angeles deve tirare un TASM inerte e dimostrare che il missile può trovare la nave giusta. La storia dice che non
  sempre ci è riuscito.
- **Obiettivi:**
  - primari: il TASM d'esercitazione sulla nave bersaglio (colpire, armi inerti); nessuna nave civile «colpita»;
  - secondario: lanciare entro un'ora dal primo dato.
- **Svolta:** al secondo aggiornamento la nave bersaglio ha cambiato rotta, e un mercantile greco le passa a tre
  miglia. La serpentina del TASM con l'ESM cerca il radar giusto, ma il mercantile ha un radar di navigazione acceso.
- **Forze:**
  - USA: la nave bersaglio col rimorchiatore, un'unità di scorta del poligono;
  - neutrali: mercantili e traghetti della rotta Brindisi-Patrasso.
- **Comportamenti e regole:**
  - `esercitazione = true` sulle armi del giocatore;
  - i rapporti radio come contatti d'intelligence, ogni 30 minuti e in ritardo di 20;
  - il mercantile con il radar acceso (emissioni per l'ESM del TASM).
- **Che cosa la rende diversa:** il TASM e la sua ricerca a serpentina con l'ESM, il dato oltre l'orizzonte che
  invecchia, la carta di tiro che mostra quanto è grande l'incertezza a duecento miglia.
- **Difficoltà:** 2. **Durata:** 1,5 ore.
- **Ispirata a:** OUTLAW SHARK e le prove del TASM degli anni '70-'80 (dossier 27 §3.2 e §0.1 punto 7: 20 minuti o più
  di catena, una o due volte l'ora) [NSLBA] [GAO81]. Nomi e unità di fantasia; Otranto al posto del Mediterraneo
  orientale [SOST].
- **Serve al motore:**
  - c'è: TASM con serpentina ed ESM, fonte radio, armi d'esercitazione, obiettivo `colpire`;
  - verificato dal disassemblatore (28/10/2026): l'ESM del TASM sente fino a ~70 km solo i radar della sua lista (Top
    Sail, Head Net C) e ignora, giustamente, quelli di navigazione;
  - fatti dal runtime il 28/10/2026: i missili inerti (colpo d'esercitazione senza danno, obiettivo `colpire`) e la chiave
    d'unità `emcon` (EMCON di partenza). La nave bersaglio parte in «ricerca»: senza, in pace non accenderebbe i radar
    di ricerca e l'ESM non sentirebbe niente.
- **Costruzione (28/10/2026):** installata con le prove: nell'Enhanced il bersaglio accende Top Sail e Head Net C.
  Semplificazioni: il teatro di Otranto è di circa 330 km, quindi la nave bersaglio sta a circa 99 miglia invece di 200;
  rimorchiatore e traghetti sono della classe «mercantile», perché quelle classi non hanno il modello del livello
  classico.

---

## 2d. Missioni di riconoscimento visivo

Approvate dall'utente il 29/10/2026, con le armi da esercitazione: siluri senza testa, per provare la soluzione di tiro
sulle navi identificate. Vengono da docs/25 §4 (il libretto di riconoscimento e il telemetro). La proposta completa è
nella nota condivisa `scheda-riconoscimento-visivo-proposta.md`. Due correzioni dopo l'approvazione:
- le navi da guerra erano 8: per il «7 su 9» c'è una Nanuchka nella 5ª Eskadra;
- il secondario sulle AOB è l'obiettivo `rotta_stimata` del runtime.

### LIB-I13 — «Sagome nello Stretto»

- **Battello:** 688, con i siluri d'esercitazione (`esercitazione = true`).
- **Teatro:** Stretto di Gibilterra, lato del Mare di Alborán.
- **Data:** 12 giugno 1989, 15:00Z.
- **Meteo:** fissato: mare 2, vento debole da ovest, visibilità 20 km.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:** prima del Mediterraneo orientale, COMSUBLANT vuole che il comandante sappia riconoscere a vista quello
  che incontrerà. Nello Stretto, in un pomeriggio di giugno, passano:
  - un gruppo americano in entrata;
  - la 5ª Eskadra sovietica che lo aspetta;
  - una fregata algerina di costruzione sovietica;
  - mercantili e pescherecci.
- **Obiettivi:**
  - primari:
    - identificare a vista almeno 7 delle 9 navi da guerra (`identificare`);
    - tre distanze col telemetro entro il 10% (`telemetro`);
    - tornare a casa;
  - secondari:
    - tre rotte dall'AOB entro 20° (`rotta_stimata`);
    - la foto della nave spia entro 3 km (`foto`);
    - la soluzione di tiro provata coi siluri d'esercitazione (`colpire`);
    - non farsi scoprire.
- **Le coppie che ingannano:**
  - Kidd e Spruance: stesso scafo, e il telemetro sbaglia del 5% con la classe sbagliata;
  - la nave spia e i pescherecci;
  - la Koni algerina (neutrale) e la Krivak.
- **Svolta:** a 40 minuti Kidd e Spruance si scambiano di posto; a 50 minuti la Koni esce da sud e taglia la rotta del
  gruppo.
- **Forze:**
  - NATO: Kidd, Spruance, Perry, Sacramento, verso est;
  - Patto: Kara, Krivak II, Nanuchka, la nave spia (classe agi) che pedina;
  - neutrali: la Koni, cinque navi mercantili, quattro pescherecci.
- **Difficoltà:** 2. **Durata:** 1,5 ore.

### LIB-I14 — «Levante»

- **Come I13**, la mattina dopo alle 06:30Z, con la nebbia d'avvezione che il Levante porta nello Stretto: visibilità
  1,5 km, mare 3, vento da est 12 m/s. Il traffico è in fila e più lento.
- **Obiettivi:**
  - identificare 5 delle 9;
  - le distanze entro il 15%;
  - una collisione chiude in fallimento.
- **Difficoltà:** 3. **Durata:** 1,5 ore.
- **La notte:** scartata qui perché il motore non aveva le luci di navigazione; ora le ha (docs/27), ed è I15.

### LIB-I15 — «Fanali»

Approvata dall'utente il 29/10/2026 sera, con due scelte: la Kara e la nave spia a luci spente in pace (la
provocazione); gli errori d'identificazione solo nel debriefing, senza un obiettivo nuovo. La proposta completa è nella
nota condivisa `scheda-riconoscimento-notte-proposta.md`.

- **Battello:** 688, coi siluri d'esercitazione (`esercitazione = true`), come I13 e I14.
- **Teatro:** Stretto di Gibilterra, lato del Mare di Alborán: la stessa acqua di I13, con le stesse posizioni di
  partenza.
- **Data:** 3 giugno 1989, 23:30Z, notte senza luna. Il motore non usa ancora la fase della Luna: la sagoma notturna
  vale per una notte buia.
- **Meteo:** fissato: mare 2, vento debole da ovest (4 m/s), visibilità 15 km, nubi 2 ottavi.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:**
  - dopo il giorno e la nebbia, COMSUBLANT vuole la prova più difficile: riconoscere di notte, quando al periscopio si
    vedono solo luci;
  - il gruppo americano entra nello Stretto coi fanali accesi, come vuole il regolamento in pace;
  - la Kara e la nave spia della 5ª Eskadra navigano a luci spente vicino al gruppo NATO, contro il COLREG: si vedono
    solo come sagome, e solo da vicino;
  - fra le coste lavorano i pescherecci, illuminati dalle luci di coperta: di notte sembrano una flotta.
- **Che cosa si riconosce, e come:**
  - dalle luci, il tipo: il mercantile in moto (due fanali d'albero, il laterale, il coronamento), il peschereccio a
    strascico (verde sopra bianco), la nave alla fonda (bianchi tutt'intorno);
  - dalla sagoma contro il cielo, da vicino, la classe;
  - dai fanali l'AOB: il colore del laterale dice il fianco, l'allineamento dei fanali d'albero quanto la nave viene
    incontro;
  - il telemetro si legge sul fanale d'albero, con più incertezza che di giorno.
- **Obiettivi:**
  - primari:
    - identificare a vista almeno 5 delle 9 navi da guerra (`identificare`);
    - tre rotte dai fanali entro 20° (`rotta_stimata`): qui primario, perché è la capacità della notte;
    - tornare a casa;
  - secondari:
    - tre distanze col telemetro entro il 15% (`telemetro`);
    - non farsi scoprire.
  - Niente foto della nave spia (è buia) e niente prova di tiro: è la notte del riconoscimento.
  - Un peschereccio preso per una nave da guerra non ha un obiettivo: si vede nel confronto del debriefing.
  - Una collisione chiude in fallimento (esito 41), come col Levante.
- **Svolta:** a 30 minuti la Kara (a 20 nodi) e la nave spia (a 12) tagliano la scia del gruppo americano 2 km dietro
  il centro, da sud verso nord. Il punto di taglio è calcolato sulla rotta del gruppo, ed è a circa 3 km a nord del 688
  di partenza. La Kara passa a circa 3,4 km dal 688, appena oltre la portata della sua sagoma (2,7 km): per
  identificarla bisogna andarle incontro. Chi guarda solo le luci non le vede; chi ascolta il sonar le sente e le cerca al periscopio. Restano le
  svolte di I13: lo scambio di Kidd e Spruance a 40 minuti, la Koni a 50.
- **Forze:**
  - come in I13, con la chiave `luci` dello scenario: Kara e nave spia `luci = "spente"`; le altre navi da guerra coi
    fanali di serie;
  - cinque mercantili: quattro in moto, uno fermo in avaria 4 km a sud-est del 688, fuori corsia, con
    `luci = "non_governa"` (due rossi tutt'intorno);
  - sei pescherecci, due più che in I13: i falsi bersagli. Lavorano sul fondo del gambero rosso, fra 400 e 800 m, a
    6-12 km dal 688, nella portata delle loro luci;
  - la nave spia aspetta il gruppo nello Stretto, 4,5 km a nord-nord-est del 688: con 11,5 nodi non starebbe dietro a
    un gruppo che va a 14.
- **Tre correzioni dopo la prova al periscopio** (29/10/2026 sera), guardando le istantanee notturne e la portata di
  ogni nave dal 688:
  - **niente nave alla fonda:** sotto il 688 ci sono 500-900 m d'acqua, e i fondali da ancoraggio (20-60 m) sono a più
    di 20 km, fuori dalla portata dei fanali di fonda. Al suo posto un mercantile in avaria, senza governo: un altro
    schema del COLREG da riconoscere;
  - **i pescherecci sul fondo dello strascico profondo:** fra 40 e 200 m stavano a 13-33 km, oltre i loro 10-13 km di
    portata, e il «campo di luci» non si vedeva. Lo strascico del gambero rosso di Alborán lavora davvero a 400-800 m;
  - **la nave spia che aspetta:** partiva dietro il gruppo e restava a 12-17 km per tutta la missione.
- **Costruita e installata il 29/10/2026 sera** (trailer-1), dopo la riga del runtime sulla scoperta automatica a 9
  miglia (di notte vale solo entro la portata della sagoma e dei fanali). La prova «libreria I15» controlla: lo stato
  delle luci per unità; che la Kara, la Krivak e la Nanuchka non vedano il 688 a quota periscopica oltre i 3 km;
  identificazioni e rotte giuste raggiunte, sbagliate (rotte di 60° fuori) no.
- **La simmetria:** il 688 a quota periscopica non ha luci; in superficie coi fanali accesi si fa vedere lontano (la
  vedetta dell'IA, docs/27).
- **Difficoltà:** 4. **Durata:** 1,5 ore.
- **Ispirata a:** inedita. Le luci sono quelle del COLREG 1972 (regole 21-26, Annesso I). Le navi sovietiche a luci
  spente vicino ai gruppi NATO sono un episodio ricorrente degli anni Ottanta, il tipo d'incidente che l'accordo INCSEA
  del 1972 cercava di regolare.

### Serve al motore

- C'è:
  - il libretto con IDENTIFICA;
  - il telemetro col quadrante;
  - l'AOB alla TMA;
  - la classificazione visiva nel registro;
  - le armi da esercitazione;
  - gli obiettivi `foto`, `colpire` e `non_essere_scoperto`.
- Fatti da trailer-1 il 29/10/2026 nello scrittore, nell'editor e nella verifica: i tipi `identificare`, `telemetro` e
  `rotta_stimata`, col campo `errore` (relativo per il telemetro, in gradi per la rotta).
- In corso, del runtime:
  - la valutazione dei tre obiettivi nel registro;
  - l'ordine `misura_telemetro`, con la distanza letta, la vera e l'altezza usata nel registro.
- **Costruite e installate il 29/10/2026** (trailer-1), con le prove di vittoria e sconfitta: identificazioni
  sbagliate e letture al 20% non contano; giuste sì. Il 12% basta col Levante (15%), non di giorno (10%).

## 2e. Missioni sotto il ghiaccio

Approvata dall'utente il 29/10/2026 notte, con la scorta Victor III e la navigazione realistica di serie (il giocatore
può toglierla dalle opzioni). Usa l'opzione A del ghiaccio (nota condivisa `sotto-ghiaccio-piano.md`):
- la volta, il sondatore verso l'alto e il BQS-15 sulle chiglie;
- l'urto della vela e l'acustica del pack;
- sotto la banchisa solo ELF e niente punto nave;
- l'emersione solo in acqua libera o in una polynya.

La proposta completa è nella nota `scheda-ghiaccio-proposta.md`.

**Perché il Mare di Okhotsk.**
- Il teatro del Barents di oggi (69,75-71,25°N) non ha ghiaccio: i suoi punti ERA5 restano sotto il 3% tutto l'anno.
- Le Curili hanno il pack al 71-75% in marzo dal lato di Okhotsk, e lo 0% dal lato del Pacifico.
- Il bastione dei Delta III della Flotta del Pacifico è vero.
- Il Barents resta possibile con un teatro nuovo (72-76°N).

### LIB-I16 — «Il bastione bianco»

- **Battello:** 688.
  - Sotto il ghiaccio può andare, ma non sfonda.
  - Nel 1989 il lavoro sotto la banchisa era dei Sturgeon. Nel pack di primo anno di Okhotsk e al margine un 688 è
    credibile, e il briefing lo dice.
- **Teatro:** Curili, lato di Okhotsk: lo Stretto di Ekaterina (soglia di 270-380 m) e il Bacino delle Curili (oltre
  1700 m sotto il bastione).
- **Data:** 14 marzo 1989, 01:00Z (mezzogiorno locale), la copertura massima dell'anno.
- **Meteo:** del teatro e della stagione (`meteo_realistico`), non fisso.
  - Il ghiaccio viene dai punti ERA5 di marzo, col margine negli stretti: pack di primo anno, polynye e canali circa al
    10%, deriva sotto 1 nodo.
  - Col meteo fisso il margine non ci sarebbe, perché `Meteo::in` non interpola i punti.
  - Per le missioni della libreria il menu prende il meteo dallo scenario.
- **Navigazione:** realistica di serie (`navigazione = { sigma_iniziale_nm = 0.5, realistica_di_serie = true }`).
  Quando si sceglie la missione, il menu la propone e il giocatore la toglie con N (la principale, `scelta_missione.cpp`).
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:**
  - COMSUBPAC vuole sapere come pattugliano i Delta III sotto il pack: rotte, quote, velocità, e la scorta.
  - Il 688 entra dal Pacifico per lo Stretto di Ekaterina, trova il Delta, lo segue e riferisce.
  - A 2 minuti dall'inizio arriva un contatto d'intelligence sul Delta, con un errore di 5 miglia.
- **Forze:**
  - **Delta III**, in pattuglia sul circuito del bastione a 130 m e 4 nodi. Senza sonar del fondo, quindi lontano dalle
    chiglie. Il Crazy Ivan è del motore: ogni 30-120 minuti i sottomarini sovietici dell'IA accostano di 120-180° per
    ascoltare la poppa;
  - **Victor III di scorta**, su un circuito più largo a 90 m (evasione a 150), paranoia 2, con l'MG-70 sulla volta;
  - **Il-38 sul margine** del ghiaccio, dal lato del Pacifico (base di Burevestnik, Iturup), con l'ASW aereo contro il
    giocatore. Le boe vanno solo in acqua libera: è la regola del runtime;
  - **rompighiaccio** con una portarinfuse dietro, lungo la costa di Kunashir verso nord: una sorgente forte, un falso
    bersaglio;
  - due pescherecci d'altura (BMRT) al margine, dal lato del Pacifico.
- **Obiettivi:**
  - primari:
    - trovare e classificare il Delta III (`scoprire`);
    - seguirlo: 45 minuti di contatto (`contatto`);
    - trasmettere il rapporto di contatto entro le 0430Z, cioè 3 ore e 30 dall'inizio (`variabile` con `entro_s`);
    - rientrare nel Pacifico;
    - tornare a casa;
  - secondari:
    - classificare la scorta;
    - non farsi scoprire.
  - Gli urti della vela con le chiglie restano un dato del debriefing (il registro li scrive), senza obiettivo.
- **Regole:**
  - **la campana:** quando il pedinamento è raggiunto, o a 2 ore e 45 se non lo è, va in onda il messaggio FLASH
    «riferire entro le 0430Z». Sotto la banchisa ne arriva solo il campanello, sull'ELF col filo;
  - **il rapporto:** dopo la campana, una trasmissione «contatto». Sotto la banchisa non parte, perché non c'è antenna;
    serve acqua libera, a quota periscopica con l'antenna;
  - **il rientro:** dopo il rapporto, la zona del Pacifico fuori dallo stretto chiude con l'esito 140.
- **Rischi:**
  - le chiglie (media 3,8 m, 99° percentile 9,4 m, massimo circa 20 m): sotto i 30 m servono il sondatore e il BQS-15;
  - il rumore del pack, che copre il Delta;
  - l'attivo quasi inutile sotto la volta;
  - la stima che deriva senza punto nave, e lo stretto basso al ritorno;
  - la scorta e il Crazy Ivan;
  - la scadenza della campana.
- **Difficoltà:** 5. **Durata:** 4 ore.
- **Classi nuove nel livello classico:** i modelli «per analogia» di `delta3`, `rompighiaccio`, `portarinfuse` e
  `bmrt` in `missioni_rapide.toml`.

## 2f. Proposte del 30/10/2026: le novità degli ultimi giorni

**Approvate dall'utente il 30/10/2026**, tutte e tre; si costruiscono una alla volta (S15, poi S14, poi I17). Mettono in gioco le novità degli ultimi
giorni:
- i VDS e le schiere delle navi (dossier 33): SQS-35 e SQR-18A della Knox, Vega di Kara e Krivak II, Polinom
  dell'Udaloy, Shelon della Grisha III;
- l'SH-2F delle Knox;
- l'IA che vede le mine col suo sonar e le aggira (docs/18, 30/10);
- la banchisa e le polynye, anche nella vista del periscopio (kit, 30/10);
- le luci di notte, con i fanali di fonda che diventano di moto quando la nave salpa (docs/27);
- il libretto e il telemetro (docs/25).

| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Novità |
|---|---|---|---|---|---|---|
| LIB-S14 | Il pedinatore in attesa | Alfa | Celtica, notte | pace | 3 | luci, libretto, telemetro, VDS e SH-2F della Knox (**costruita**) |
| LIB-S15 | L'ancoraggio | 688 | Otranto, notte | crisi | 4 | fanali di fonda e di moto, libretto, telemetro, rotte dai fanali, VDS Vega (**costruita**) |
| LIB-I17 | La soglia di Ekaterina | Alfa | Curili, Okhotsk | pace | 3 | banchisa e polynya, le PMR-2 che l'IA aggira, VDS Polinom (**costruita**) |

### LIB-S14 — «Il pedinatore in attesa»

- **Battello:** Alfa, al posto del Victor I del fatto vero [SOST].
- **Teatro:** Piattaforma Celtica [SOST]: un canale dragato verso una base SSBN di fantasia, al posto del Clyde e di
  Holy Loch.
- **Data:** 3 novembre 1989 (il giorno del fatto, 15 anni dopo), 21:30Z, notte senza luna.
- **Meteo:** fissato: mare 2, vento 6 m/s da sud-ovest, visibilità 12 km, foschia leggera.
- **Situazione:** pace. ROE: armi trattenute.
- **Premessa:**
  - Il Comando della Flotta del Nord vuole la firma acustica dell'ultimo SSBN americano che esce in pattugliamento, e
    vuole sapere dove si immerge e che rotta prende.
  - L'Alfa aspetta fuori dal canale, a quota periscopica, da solo e fermo.
  - Di notte all'uscita passano un traghetto, i rimorchiatori e i pescherecci col loro campo di luci. Il battello esce
    in superficie con i fanali e la luce ambra lampeggiante dei sottomarini americani, ma l'orario non si sa.
  - Davanti a lui una Knox «pulisce» l'uscita, col VDS calato sotto lo strato, la schiera e l'SH-2F.
- **Obiettivi:**
  - primari:
    - riconoscere l'SSBN a vista, col libretto, fra le luci del traffico (`identificare`);
    - misurargli la distanza col telemetro sul fanale d'albero (`telemetro`, entro il 15%);
    - registrarlo dopo l'immersione: 30 minuti entro 5 km (come la S10, con una variabile a regola);
    - tornare a casa;
  - secondari:
    - stimarne la rotta dai fanali prima dell'immersione (`rotta_stimata`);
    - non farsi classificare dalla Knox (la condizione «unità classifica», come la S10).
- **Svolta:**
  - L'SSBN si immerge sull'isobata dei 100 m e accosta. Le memorie russe del 1974 dicono che il sovietico usò granate di
    segnalazione: qui è la Knox che, se sente qualcosa, ne getta una e accende l'SQS-35 in attivo.
  - Se l'Alfa è sulla rotta dell'immersione, c'è la collisione del fatto vero (il modello delle collisioni):
    l'esercitazione è chiusa male.
- **Forze:**
  - NATO:
    - l'SSBN, `ohio` al posto di un Lafayette di Holy Loch [SOST]: entra con una regola fra 20 e 50 minuti
      (`entra_con`, orario a caso), in superficie a 10 nodi;
    - una Knox con SQS-35, SQR-18A e l'SH-2F, davanti all'SSBN;
    - due rimorchiatori.
  - Neutrali: un traghetto, quattro pescherecci illuminati, un cargo all'ancora fuori dal canale (fanali di fonda).
- **Comportamenti e regole:** la Knox in `R_nave` col VDS in ascolto. Se classifica l'Alfa, l'SH-2F va sul datum; nel
  peggiore dei casi la missione resta aperta, ma la registrazione non conta più (variabile).
- **Che cosa la rende diversa:** è la prima missione da «pedinatore in attesa». Prima l'occhio (di notte trovare le
  luci giuste fra tante, la luce ambra, la sagoma della Ohio nel libretto), poi l'orecchio, con un VDS che sta sotto lo
  strato come il sottomarino.
- **Difficoltà:** 2 (la missione d'ingresso alle luci e al VDS). **Durata:** 2 ore.
- **Ispirata a:** E13, *James Madison* e il sottomarino sovietico in attesa fuori da Holy Loch (3 novembre 1974): il
  cablogramma della CIA declassificato nel 2017 `[WP-JM]` (alta); le granate di segnalazione dalle memorie russe
  (bassa-media).
- **Che cosa serve al motore:**
  - c'è: luci (la luce ambra della Ohio è in `luci.toml`), libretto (tavola `ohio`), telemetro, VDS e schiera della
    Knox, SH-2F, collisioni, `entra_con`, la condizione «unità classifica» (dalla S10);
  - da verificare: l'immersione dell'SSBN dell'IA in un punto (oggi un'unità della missione parte già alla sua quota;
    se manca, una piccola azione «quota» per un'unità, nello scenario e nel runtime);
  - piccolo: la granata di segnalazione come azione (un impulso nell'acqua, senza danno); in mancanza basta il ping.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S14_PEDINATORE_IN_ATTESA.lua` e il pacchetto `.m688m`.
  - **Il teatro senza costa:** la Piattaforma Celtica del gioco e' tutta piattaforma (117-270 m) col ciglio a sud-ovest
    (535 m). L'uscita diventa la rotta in superficie da nord-est fino al punto d'immersione sul ciglio, sull'isobata delle
    100 braccia (183 m, la pratica americana), 18 km di rotta a 12 nodi. L'SSBN e la Knox entrano a 25 minuti.
  - **L'immersione:** l'azione nuova «quota» al punto (`unita_in_zona`). La prima versione non la scriveva nell'unita':
    `m688.seleziona` carica l'unita' in un buffer, e ci vuole `m688.salva_modello`. Ora l'azione lo fa.
  - **La firma:** la condizione nuova «giocatore vicino a» (`m688.distanza_da`, runtime): 10 s a ogni controllo, dopo
    l'immersione ed entro 5 km, se la Knox non ha bruciato la copertura (variabile 6).
  - **La Knox:** se classifica l'Alfa almeno POSSUB, o lo tiene 10 minuti, getta la granata di segnalazione (azione
    «rumore»), da' l'allarme e la firma non conta piu'. A giocatore fermo (3 semi, build del 06/11/2026) scopre l'Alfa che aspetta a 8 km
    fra 78 e 138 s dopo l'entrata: la difficolta' passa da 2 a 3, e l'attesa va fatta piano e lontano dalla sua rotta.
  - **Gli SSBN non pedinano** (runtime, 30/10): prima l'Ohio che scopriva l'Alfa lo seguiva invece di immergersi.
  - **Le classi per analogia** nel livello classico (`missioni_rapide.toml`): `ohio`, `knox`, `sh2f`, `traghetto`.
  - **La prova:** l'SSBN in superficie coi fanali, l'immersione al punto, la firma che conta solo dopo, la vittoria a
    3 km per mezz'ora (esito 40); trovati dalla Knox, la firma non conta.

### LIB-S15 — «L'ancoraggio»

- **Battello:** 688.
- **Teatro:** Otranto [SOST]: il bassofondo al largo della costa, a più di 12 miglia, in acque internazionali, fra 60 e
  120 m d'acqua. Sta al posto degli ancoraggi della 5a Eskadra nel Mediterraneo orientale.
- **Data:** 26 ottobre 1989, 23:00Z (il giorno delle esercitazioni anti-portaerei del 1973), notte con la luna al
  primo quarto.
- **Meteo:** fissato: mare 1-2, sereno, visibilità 18 km.
- **Situazione:** crisi. ROE: armi trattenute, fuoco solo se attaccati.
- **Premessa:**
  - La crisi è di quelle in cui due flotte stanno a distanza di tiro e nessuno spara.
  - La Sesta Flotta vuole sapere che cosa c'è stanotte all'ancoraggio della squadra sovietica, e se qualcuno salpa.
  - All'ancoraggio, coi fanali di fonda, ci sono un Kara, una Krivak II, una nave appoggio, una petroliera di squadra e
    un AGI; in giro, a luci spente, una seconda Krivak col Vega calato.
  - Un Charlie II si è affiancato alla nave appoggio.
- **Obiettivi:**
  - primari:
    - riconoscere a vista almeno 4 unità dell'ancoraggio (`identificare`);
    - riferire («contatto») prima dell'alba;
    - non farsi classificare;
    - tornare a casa;
  - secondari:
    - tre distanze col telemetro (`telemetro`);
    - le rotte di tre navi che salpano, dai fanali (`rotta_stimata`).
- **Svolta:**
  - Dopo un'ora, come nel 1973, una nave sovietica getta una bomba a mano in acqua «come per comunicare con un nostro
    sommergibile». È il segnale.
  - L'ancoraggio salpa: i fanali di fonda diventano fanali di moto e le navi escono su rotte diverse.
  - Il Charlie II si stacca e va verso il gruppo portaerei americano.
  - Se il 688 è stato classificato, la crisi sale: il Kara e la Krivak mettono il VDS in attivo e gli danno la caccia
    senza sparare (`R_pedina`, la «scorta fuori»).
- **Forze:**
  - URSS:
    - all'ancoraggio: `kara`, `krivak2`, `nave_ricerca` come nave appoggio [SOST], `petroliera`, `agi`;
    - di guardia: `krivak2` a luci spente, col Vega, a 6 nodi (a bassa velocità il pesce pende ed è profondo);
    - `charlie2` affiancato alla nave appoggio, poi in moto;
    - un Il-38 a un'ora.
  - NATO: nessuna in vista; il gruppo portaerei è oltre il teatro.
- **Comportamenti e regole:**
  - le navi ferme all'ancoraggio mostrano da sole i fanali di fonda (sotto mezzo nodo), quelle in moto i fanali di
    moto (`luci::stato_predefinito`);
  - la Krivak di guardia ha `luci = "spente"`;
  - salpano con regole `vai_a` dopo l'evento della bomba a mano;
  - la classificazione del 688 porta la situazione a «tensione» (allerta) e i VDS all'attivo.
- **Che cosa la rende diversa:** tutta la missione è «guardare di notte». Prima le luci di fonda: chi è chi, a che
  distanza. Poi le luci di moto: la rotta di chi salpa. Intanto c'è un VDS a 100 m che rende inutile nascondersi sotto
  lo strato.
- **Difficoltà:** 4. **Durata:** 3 ore.
- **Ispirata a:** E11, la 5a Eskadra contro la Sesta Flotta nell'ottobre 1973 (Goldstein e Žukov, con i diari
  sovietici, `[NWCR04]`, media-alta). Dalla fonte vengono le esercitazioni anti-portaerei del 26 ottobre, la bomba a
  mano gettata in acqua per innervosire gli americani e il pedinamento ravvicinato dei gruppi portaerei. L'ancoraggio
  in acque internazionali è una trasposizione: i nomi degli ancoraggi veri (Citera, Hurd Bank) sono da verificare
  nelle fonti prima della costruzione.
- **Che cosa serve al motore:**
  - c'è: luci (fonda e moto da sole, `spente` a mano), libretto, telemetro, `rotta_stimata`, VDS Vega con la quota che
    dipende dalla velocità, situazione «crisi», la condizione «unità classifica», la trasmissione «contatto»;
  - piccolo: la bomba a mano come evento della missione (un rumore in acqua, senza danno), la stessa della S14;
  - da verificare: il Charlie II «affiancato» a una nave ferma, cioè a 100-200 m, senza collisioni.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S15_ANCORAGGIO.lua` e il pacchetto `.m688m`.
  - **Dove:** il bassofondo a 41,20°N 19,00°E, a circa 20 miglia dalla costa, fra 70 e 110 m d'acqua. Il 688 parte
    16 km a sud-ovest, a 60 m e 5 nodi.
  - **Gli ancoraggi veri:** Citera è il «punto» n. 1 della carta 1973 dei punti della 5a Eskadra, con Hammamet («punto
    3») ed es-Sollum («punto 52») (cyclowiki, «Флот России на Средиземном море»); per Sollum anche gli atti della Marina
    1996 «Il Mediterraneo quale elemento del Potere Marittimo». Hurd Bank non risulta nelle fonti e non c'e'.
  - **La nave appoggio:** la base galleggiante `ugra` (Progetto 1886) del kit, col suo modello; prima era un `cargo`.
  - **Le azioni nuove dello scenario** (`scenario_missione`, editor): «rumore» (`m688.rumore_transitorio`, la bomba a mano)
    e «quota» (`m688.quota` col rateo del modello, il Charlie che si immerge). Nessun pezzo nuovo del runtime.
  - **L'allarme:** una nave che classifica il 688 almeno POSSUB, o che lo tiene 5 minuti con un contatto utilizzabile.
    Il sonarista dell'IA oggi non porta il 688 oltre NONSUB, e il runtime lo sta guardando. Kara, Krivak all'ancora e
    Krivak di guardia passano a `R_nave`: accostano, corrono e pingano, ma in crisi le ROE dell'IA non liberano le armi
    contro gli americani. A giocatore fermo (3 semi, 3 ore, build del 06/11/2026): il sonarista avverte «ci stanno pingando» da 360 a 527 volte, nessun lancio.
  - **La prova** (`prove_libreria_missioni.cpp`):
    - i fanali di fonda all'inizio e quelli di moto dopo la partenza;
    - la Krivak di guardia a luci spente;
    - il Charlie affiancato che si immerge;
    - il rapporto a quota periscopica con l'antenna, poi il largo: esito 40;
    - tenuto a 1,5 km dalla guardia: allarme, ping, nessun lancio.

### LIB-I17 — «La soglia di Ekaterina»

- **Battello:** Alfa, al posto di un Victor III della Flotta del Pacifico [SOST]. Il briefing lo dice, come nella S10.
- **Teatro:** Curili, lato di Okhotsk: lo Stretto di Ekaterina e il margine del pack, come la I16, ma dall'altra parte.
- **Data:** 20 marzo 1989, 18:00Z (le 5 del mattino locali, prima dell'alba).
- **Meteo:** del teatro e della stagione (`meteo_realistico`): pack di primo anno, polynye e canali circa al 10%.
- **Navigazione:** realistica di serie, come la I16.
- **Situazione:** pace. ROE: armi trattenute; un messaggio può portarle a «crisi».
- **Premessa:**
  - Alle 0900Z un Delta III esce dalla soglia di Ekaterina verso il bastione.
  - Il comando sa, dal SOSUS sovietico e da un Il-38, che un sottomarino americano gira da giorni sotto il margine del
    pack. L'Alfa deve «ripulire» la soglia prima del Delta: trovarlo, classificarlo, riferire.
  - Sotto la banchisa passa solo l'ELF, e per trasmettere serve una polynya.
  - Nello stretto c'è lo sbarramento difensivo di PMR-2, con un varco noto solo ai sovietici. L'americano ha il BQS-15
    e vede le mine: le aggirerà.
- **Obiettivi:**
  - primari:
    - classificare il sottomarino americano (`scoprire`);
    - tenerlo 30 minuti (`contatto`);
    - riferire («contatto») da una polynya prima che esca il Delta (`variabile` con `entro_s`, come la I16);
    - tornare a casa;
  - secondari:
    - non farsi classificare dall'americano;
    - nessun urto della vela con le chiglie (un dato del debriefing).
- **Svolta:**
  - A un'ora l'americano va verso una polynya per il suo rapporto, al margine, dal lato del Pacifico.
  - Quando il Delta esce (entra con una regola alle 0900Z), l'americano lo cerca e passa vicino allo sbarramento: il
    BQS-15 vede le PMR-2 e lo fa accostare verso il varco, dove l'Alfa può aspettarlo.
  - Se l'Alfa riferisce in tempo, l'Il-38 e un Udaloy col Polinom calato arrivano al margine per tenerlo. Le boe vanno
    solo in acqua libera: sotto il pack il contatto resta dell'Alfa.
- **Forze:**
  - USA: `sturgeon`, il battello sotto il ghiaccio del 1989, a 120-150 m e 5 nodi, paranoia 2, col BQS-15 acceso
    nello stretto.
  - URSS:
    - il `delta3`, di riserva: entra alle 0900Z;
    - un `udaloy` col Polinom VDS e un `ka27`, al margine dopo il rapporto;
    - l'`il38` (`aerei_asw`, zona del margine);
    - un `rompighiaccio` con una `portarinfuse` dietro, lungo la costa di Kunashir, coi fanali (all'alba: libretto e
      telemetro per dire chi è);
    - lo sbarramento: 8-10 PMR-2 della parte sovietica nello stretto, a 300 m sotto la superficie, con un varco largo
      2 km.
- **Comportamenti e regole:**
  - lo Sturgeon ha un circuito sotto il margine, poi `vai_a` alla polynya e poi verso il Delta;
  - il pilota dell'IA aggira le mine viste (runtime, 30/10): è il gesto della missione, l'americano «incanalato» dalle
    mine;
  - la campana dell'ELF con l'ordine «riferire» a 2 ore;
  - il rapporto chiude la fase e fa entrare Udaloy e Il-38.
- **Che cosa la rende diversa:** è la I16 dall'altra parte, la simmetria fatta missione. Sotto la volta, contro un
  battello silenzioso, il campo minato amico diventa uno strumento di caccia: l'Alfa sa dove l'americano non può
  passare, perché lì le mine le vede.
- **Difficoltà:** 3 (l'Alfa è veloce e lo sbarramento aiuta; la volta e lo Sturgeon silenzioso no). **Durata:** 3 ore.
- **Ispirata a:**
  - E31: i bastioni dell'Okhotsk e la strategia americana di mandare gli SSN nell'Okhotsk e sotto il ghiaccio
    (`[NSL-BAS]`, `[CIMSEC-DIS]`, media);
  - E08: i battelli americani che entravano nell'Okhotsk per Ivy Bells (`[WP-IVY]`, `[BMB]`, media);
  - le PMR-2, mine a razzo antisom per i fondali di 200-1000 m (dossier 14 §2.12, `[AMPMR2]` `[WFPMR2]`).
  - Il soggetto è inedito.
- **Che cosa serve al motore:**
  - c'è: il ghiaccio dell'opzione A (volta, chiglie, polynye, solo ELF sotto la banchisa), la banchisa nella vista del
    periscopio, l'IA che aggira le mine, le PMR-2, il Polinom VDS, le boe solo in acqua libera, le luci, la navigazione
    realistica;
  - da verificare: le mine «della parte sovietica» che il giocatore sovietico conosce già (sulla carta, come campo
    minato noto) e che l'IA americana non conosce;
  - nessuna classe nuova.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_I17_SOGLIA_DI_EKATERINA.lua` e il pacchetto `.m688m`.
  - **L'americano** e' un Los Angeles, non uno Sturgeon: nel motore il sonar da mine e ghiaccio e' dei Los Angeles
    (`ha_bqs15`), e sotto la banchisa l'IA ora lo tiene acceso (disassemblatore). Gira a 200 m, vicino alla quota delle
    PMR-2 (300 m), cosi' il BQS-15 le vede da 900 m.
  - **Lo sbarramento:** PMR-2 ogni 300 m sul parallelo 44,50 nel canale profondo, in sicurezza (`sicura`, runtime: inerti,
    visibili ai sonar). Il varco e' di 2,5 km dal lato di Kunashir, dove il canale profondo prosegue a sud; dal lato di
    Iturup il fondo risale a 60 m. La carta mostra il campo della propria parte come «nota» (principale).
  - **Il gesto:** il pilota dell'IA cerca il varco davanti a una fila di mine viste (runtime). Nella prova il Los Angeles
    vede le mine, scivola lungo la fila e passa dal varco a 525 m dalla mina piu' vicina.
  - **Il Delta** entra a 2 ore dal Pacifico per il canale profondo (sud, varco, poi a nord-est nel bacino).
  - **Classi per analogia:** `udaloy` nel livello classico.
  - **Nella partita senza ordini** l'americano sente l'Alfa che va a 8 nodi e lo pedina invece di andare a sud: e'
    giusto, e il briefing chiede di restare silenziosi.
  - **La storia della costruzione:** per qualche ora e' stata fuori dalla libreria. Le PMR-2 con `armata = false` si
    armavano lo stesso, e in pace una ha affondato il Los Angeles; il pilota passava fra due mine a 200 m. I due pezzi del
    runtime (mine in sicurezza, ricerca del varco) l'hanno rimessa.

---

## 2g. Proposte del 30/10/2026 (seconde): tre episodi veri non ancora usati

**Approvate dall'utente il 30/10/2026**, tutte e tre (S16 e S17 subito, la S18 dopo la mina comandata). Vengono dagli episodi del dossier 23
che la libreria non usa ancora (E12, E20, E15) e mettono in gioco i pezzi di oggi:
- le azioni «rumore» (cariche e granate di segnalazione) e «quota»;
- le condizioni «unità classifica» e «giocatore vicino a»;
- le mine in sicurezza;
- le reazioni dell'IA in pace;
- le collisioni.

| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Episodio |
|---|---|---|---|---|---|---|
| LIB-S16 | La scala | Alfa | Fær Øer, un fiordo [SOST] | crisi | 4 | E12, il Hardangerfjord (1983) (**costruita**) |
| LIB-S17 | Sotto la prora | Alfa | Curili, lato Pacifico [SOST] | pace | 4 | E20, K-314 e la *Kitty Hawk* (1984) (**costruita**) |
| LIB-S18 | L'arcipelago | 688 | Baltico, un arcipelago neutrale [SOST] | crisi | 5 | E15, Hårsfjärden (1982) (**costruita**) |

### LIB-S16 — «La scala»

- **Battello:** Alfa. La nazionalità vera è contesa: per lo studio dell'accademia norvegese del 2013 erano per lo più
  diesel di origine sovietica, ma in qualche caso gli indizi puntavano a una potenza occidentale `[FHS13]`. Il briefing
  lo dice.
- **Teatro:** Fær Øer [SOST], un fiordo al posto del Hardangerfjord (il dossier: «i fiordi delle Faroe hanno lo stesso
  ambiente»). Il paese costiero è neutrale e di fantasia, senza nomi veri.
- **Data:** 27 aprile 1989 (sei anni dopo il fatto), 04:00Z, alba.
- **Meteo:** fissato: mare 1 nel fiordo, nubi basse, visibilità 6 km.
- **Situazione e ROE:** crisi. Armi trattenute: l'Alfa non risponde al fuoco.
- **Premessa:**
  - Entrare nel fiordo, fotografare l'installazione in fondo (una nave appoggio all'ancora) e uscire senza farsi
    prendere.
  - Il paese costiero applica la sua scala d'escalation, ed è quella vera del dossier `[CIMSEC]`:
    1. il segnale di emergere;
    2. se il sottomarino non obbedisce, cariche di segnalazione a 300 m, ogni due minuti;
    3. se ancora non obbedisce, l'attacco con le cariche di profondità e i razzi antisom. I siluri sono vietati.
- **Obiettivi:**
  - primari:
    - fotografare l'installazione (`foto`, entro 2 km);
    - uscire dal fiordo (zona);
    - tornare a casa;
  - secondari:
    - non farsi classificare (`unità classifica`);
    - non arrivare all'ultimo gradino della scala (variabile).
- **Svolta:**
  - Quando una fregata classifica l'Alfa, o lo tiene 5 minuti, parte la scala:
    1. un messaggio radio sulla frequenza di soccorso, «emergere»;
    2. dopo 5 minuti, le cariche di segnalazione: l'azione «rumore», ripetuta ogni 2 minuti, senza danno;
    3. dopo 15 minuti, l'attacco: `attacca_unita` con i razzi antisom.
  - Se l'Alfa emerge (`giocatore_in_superficie`), la scala si ferma e le fregate lo scortano fuori: esito «richiamato».
- **Forze:** neutrali:
  - due Koni [SOST: le Oslo norvegesi], che hanno i lanciarazzi antisom RBU come i Terne;
  - una motocannoniera (Nanuchka [SOST]);
  - un P-3 (`p3c`, parte neutrale);
  - il traffico del fiordo: traghetto e pescherecci costieri.
- **Che cosa la rende diversa:**
  - le ROE a gradini, che il giocatore sente arrivare;
  - l'acqua stratificata del fiordo, che nasconde, e le pareti di roccia;
  - una ritirata che è una scelta, cioè emergere e farsi scortare fuori.
- **Difficoltà:** 4. **Durata:** 2 ore.
- **Ispirata a:**
  - E12, il Hardangerfjord (fine aprile 1983): le fregate norvegesi sparano razzi e cariche di profondità contro un
    sottomarino sospetto all'imboccatura, senza conferme `[CSM83N]`;
  - la scala d'escalation norvegese, adottata dopo `[CIMSEC]`;
  - la conclusione dello studio del 2013 `[FHS13]`.
- **Che cosa serve al motore:**
  - c'è:
    - le azioni «rumore», `attacca_unita` (che ignora parte e ROE; I03, I05) e «situazione»;
    - la condizione «unità classifica»;
    - `foto`, le zone, `giocatore_in_superficie`;
    - le fregate neutrali armate con l'attacco forzato;
  - da verificare:
    - lo strato d'acqua dolce dei fiordi (oggi c'è solo lo strato termico del meteo: si rende con uno strato basso e
      forte);
    - le pareti del fiordo nella batimetria delle Fær Øer;
    - l'esito «richiamato» quando l'Alfa emerge.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S16_SCALA.lua` e il pacchetto `.m688m`.
  - **Il fiordo:** il braccio di mare fra 62,24 e 62,34°N attorno a 6,8°W, largo 600-1000 m e profondo 90-140 m nella
    batimetria a celle di 200 m. La bocca e' a nord-ovest, la nave appoggio in fondo.
  - **Il paese costiero** e' la parte del luogo (M7), in crisi con l'URSS e in pace con gli USA (`situazione_luogo`):
    fregate (`oslo`, dal 30/10/2026; prima Koni), motocannoniera (`hauk`, la classe vera del kit), P-3.
  - **La scala:** con la classificazione (almeno POSSUB) o 5 minuti di contatto, il segnale di emergere, e le fregate
    passano a `R_nave` (vengono addosso e pingano). Poi cinque cariche di segnalazione da 5 minuti, ogni 2 (azione
    «rumore»). Al quindicesimo minuto l'ultimo gradino: il luogo dichiara la guerra all'URSS (azione «situazione» fra
    luogo e urss) e le fregate sparano coi razzi: i Terne III delle Oslo, fra 425 e 1600 m, mai i siluri. `attacca_unita`
    vale solo per i sottomarini.
    Se le due fregate lo perdono per 10 minuti, la scala riparte da capo.
  - **Emergere:** dopo il segnale chiude con l'esito 43, «richiamato».
  - **La prova:** la scala nei tempi e l'ultimo gradino, coi ping delle fregate; emerso, richiamato; la foto da meno di
    2 km col periscopio dell'Alfa (il «signal», che fotografa) e il largo, esito 40.
  - **Da verificare in partita:** il tiro delle fregate a portata dopo la guerra del luogo; nella prova l'Alfa a 12 nodi
    e' gia' lontano.

### LIB-S17 — «Sotto la prora»

- **Battello:** Alfa, al posto del Victor I K-314 [SOST]. Il briefing lo dice.
- **Teatro:** il fatto vero è nel **Mar del Giappone**, durante l'esercitazione Team Spirit 84: **quel teatro nel gioco non
  c'è.** Il più vicino è il lato Pacifico delle Curili (`kunashir`), mare aperto e fondali profondi; il dossier propone lo
  stesso [SOST]. La scheda lo dichiara.
- **Data:** 21 marzo 1989 (cinque anni dopo, lo stesso giorno), 11:00Z (le 22 locali del fatto), notte.
- **Meteo:** del teatro e della stagione (`meteo_realistico`): in marzo sul lato Pacifico mare 4-5 e vento forte. È il
  maltempo che nel 1984 fece perdere il contatto.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Da tre giorni il battello pedina un gruppo portaerei americano in esercitazione. Il gruppo prova a scrollarselo di
    dosso: zig-zag, silenzio radio, cambi di velocità.
  - Col mare grosso il contatto si perde. Il comando vuole sapere dov'è il gruppo e che cosa fa.
- **Obiettivi:**
  - primari:
    - ritrovare il gruppo e tenere la portaerei 60 minuti (`contatto`);
    - riferire («contatto») da quota periscopica;
    - tornare a casa;
  - secondari:
    - non farsi classificare dalla scorta;
    - non venire a meno di 1000 m dalla portaerei, da ogni lato (`giocatore vicino a`; il briefing lo dice così dal 3/11/2026).
- **Svolta:**
  - All'inizio il gruppo è fuori portata. Dopo 40 minuti, con un'accostata di 60° e 15 nodi, viene incontro all'Alfa.
  - Il momento è quello del 1984: tornare su alla cieca col gruppo che converge. La portaerei a 15 nodi non si ferma.
  - Se l'Alfa sale davanti alla prora, è la collisione del fatto vero: esito 41. L'Alfa rotola, perde l'elica e deve
    essere rimorchiato.
- **Forze:** USA:
  - una `nimitz` [SOST: la *Kitty Hawk*];
  - una `knox` [la *Harold E. Holt* era una Knox], col VDS e l'SH-2F;
  - una `spruance` con la SQR-19;
  - un S-3B e un SH-3H.
  - Il gruppo zig-zaga con le regole.
- **Che cosa la rende diversa:** il pedinamento di un gruppo portaerei col mare che sale, e la decisione più pericolosa
  del mestiere: risalire senza sapere dove sono.
- **Difficoltà:** 4. **Durata:** 3 ore.
- **Ispirata a:** E20, K-314 e la *Kitty Hawk* (21 marzo 1984, verso le 22, Mar del Giappone, Team Spirit 84). Il
  Victor I aveva perso il contatto col maltempo; risalito a quota periscopica, vide il gruppo a pochi chilometri su una
  rotta convergente. La portaerei, a circa 15 nodi, lo urtò due volte: il primo urto lo rovesciò, il secondo gli staccò
  una pala dell'elica, rimasta nello scafo della portaerei. La *Harold E. Holt* restò vicino per giorni; il battello fu
  rimorchiato in patria `[WP-K314]` `[NI-K314]`.
- **Che cosa serve al motore:**
  - c'è:
    - le collisioni (il modello e l'evento);
    - le reazioni in pace, con gli SSBN esclusi (il gruppo non pedina);
    - le condizioni «giocatore vicino a» e «unità classifica»;
    - il meteo realistico, il VDS della Knox e l'SH-2F;
  - da verificare:
    - che il mare grosso faccia davvero perdere il contatto a distanza (l'acustica col mare 5);
    - la «collisione alla cieca»: l'urto con la portaerei a quota periscopica e i danni all'elica dell'Alfa (il modello
      dei danni del kit).

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S17_SOTTO_LA_PRORA.lua` e il pacchetto `.m688m`.
  - **Dove:** l'angolo d'oceano profondo a sud-est del teatro (43,3-43,9°N, 147,6-148,4°E, 2000-6800 m): il teatro finisce
    a 148,4°E, e il resto del lato Pacifico e' piattaforma.
  - **Il contatto perso:** a 15 nodi la portaerei si sente da 20 miglia, e l'Alfa a 8 nodi pure. Per questo il gruppo
    rientra dal bordo del teatro a 20 minuti (`entra_con`) zig-zagando, e a 40 minuti accosta verso il punto dell'Alfa.
    Ritrovarlo e' facile; restare nascosti e non finirgli davanti e' la missione.
  - **La prora:** la condizione «giocatore vicino a» sulla portaerei (1000 m) fa cadere l'obiettivo secondario; la
    collisione chiude con l'esito 41.
  - **La prova:** il gruppo che rientra, un'ora a 5 km a poppa (con la svolta), il rapporto, esito 40; 600 m davanti alla
    prora, l'obiettivo cade.
  - **La portaerei** e' la `kitty_hawk` del kit (dal 30/10, senza sonar nel modello per analogia), al posto della `nimitz`.
  - **L'antenna:** a quota periscopica l'Alfa sale a 5,5 m come il 688 (la chiglia periscopica della classe la porta il
    motore) e l'antenna esce con l'ordine; la prova aspetta i 6,5 m.

### LIB-S18 — «L'arcipelago»

- **Battello:** 688, una delle due varianti del dossier. La tesi minoritaria di Tunander vuole occidentali molte
  incursioni `[TUN]` (bassa); la variante Alfa resta possibile con la stessa scheda. Il paese è neutrale e di fantasia,
  senza nomi veri.
- **Teatro:** Baltico [SOST]: un arcipelago neutrale di fantasia, fra 8 e 50 m d'acqua come a Hårsfjärden.
- **Data:** 1° ottobre 1989 (sette anni dopo), 21:00Z, notte.
- **Meteo:** fissato: mare 1, foschia, visibilità 4 km.
- **Situazione e ROE:** crisi. ROE a zone, quelle della commissione del 1983 `[CIMSEC]`:
  - nelle acque esterne gli inseguitori avvertono e scortano fuori;
  - dentro l'arcipelago, se il contatto non esce, sparano per danneggiare o affondare.
- **Premessa:**
  - Osservare la base navale in fondo all'arcipelago (la nave appoggio e le corvette ai moli) e uscire.
  - All'ingresso c'è uno sbarramento di mine controllate da terra, con sensori: una stazione le fa scoppiare quando un
    sottomarino ci passa sopra.
- **Obiettivi:**
  - primari:
    - fotografare la base (`foto`);
    - uscire dall'arcipelago;
    - tornare a casa;
  - secondari:
    - non farsi classificare;
    - passare lo sbarramento senza far scoppiare le mine.
- **Svolta:**
  - Quando una corvetta classifica il 688 nelle acque interne, la Marina chiude l'area: le mine dello sbarramento
    passano «in mano alla stazione», e le corvette cominciano le cariche.
  - Dal fatto vero: 44 bombe di profondità e 4 mine `[WP-SWE]`; il battello passò la barriera e le mine furono fatte
    saltare da terra quando era già lontano `[CSM82]`.
  - Una variante a caso (bassa confidenza, `[UPI82]`): un «cessate il fuoco» di tre ore alla stazione delle mine.
- **Forze:** neutrali:
  - tre Koni [SOST: le corvette];
  - un elicottero ASW (`sh3h` [SOST: i Vertol svedesi]);
  - un sottomarino costiero (`type206`, parte neutrale);
  - le mine dello sbarramento;
  - il traffico dell'arcipelago.
- **Che cosa la rende diversa:**
  - l'acqua bassissima;
  - un nemico neutrale;
  - ROE che dipendono dalla linea su cui ti trovi;
  - le mine che non scoppiano da sole, ma quando qualcuno a terra decide.
- **Difficoltà:** 5. **Durata:** 2 ore e 30.
- **Ispirata a:** E15, Hårsfjärden (ottobre 1982):
  - la Marina chiude un'area con mine e sensori, riconosce un sottomarino nella trappola e fa esplodere 44 bombe e
    4 mine `[WP-SWE]`;
  - la commissione del 1983 attribuisce l'incursione all'URSS e cambia le ROE `[CIMSEC]` `[TUN]`;
  - la ricerca del 2008 attribuisce un suono del 1982 a un battello da diporto `[FM08]`;
  - è un fatto molto conteso, e il briefing lo dice.
- **Che cosa serve al motore:**
  - c'è:
    - le mine in sicurezza (runtime, 30/10);
    - zone e regole, `giocatore_a_quota`;
    - la condizione «unità classifica»;
    - le corvette neutrali armate con `attacca_unita`;
    - le cariche con «rumore»;
  - **manca (runtime): la mina comandata da terra.**
    - Una mina dello scenario «comandata» resta in sicurezza finché una regola non la passa alla stazione. Da quel
      momento scoppia solo su ordine, cioè quando un sottomarino entro il suo raggio è sopra il sensore della linea,
      con un ritardo d'operatore di 10-30 s (STIMA).
    - Serve anche l'azione dello scenario che le arma o le fa scoppiare in una zona.
    - **Stima:** runtime 1,5-2 ore (stato «comandata» in `CampoMinato`, `m688.comanda_mine(zona)`, la prova); dalla mia
      parte 30-45 minuti (il campo in `MinaScenario` e l'azione nello scenario e nell'editor, con la prova). Il campo
      in `MinaScenario` è un cambio d'header, da fare in una finestra concordata.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S18_ARCIPELAGO.lua` e il pacchetto `.m688m`.
  - **Dove:** l'arcipelago del teatro del Baltico fra 56,0 e 56,2°N, 15,0-16,0°E, isole e fondali da 0 a 45 m. Il 688 sta
    nella parte esterna, fra 30 e 45 m d'acqua; la base si fotografa da 3 km col Type 18, il periscopio che fotografa.
  - **Il paese costiero** e' la parte del luogo, in crisi con gli USA: corvette (`stockholm`, la classe vera del kit), l'elicottero
    (`hkp4`, il Vertol svedese col sonar a immersione, dal 30/10/2026; prima l'SH-3H), un sottomarino costiero
    (`nacken`, del kit), con traffico neutrale.
  - **Le ROE a zone:** dovunque, classificato o tenuto 5 minuti, l'avvertimento sulla frequenza di soccorso e la caccia
    col sonar (`R_nave` in crisi, senza sparare). Nelle acque interne, scoperto, l'arcipelago chiude: le mine comandate
    dell'ingresso passano alla stazione (azione nuova «comanda_mine», gruppo «ingresso») e il luogo passa alla guerra
    con gli USA. Emergere nelle acque esterne dopo la scoperta chiude con «richiamato» (43).
  - **Lo sbarramento:** 89 mine da fondo (`mdt`) comandate, ogni 300 m sul parallelo 56,02 dove ci sono fra 20 e 50 m.
    Restano in sicurezza finche' la stazione non le prende (runtime); poi scoppiano sotto un sottomarino che ci passa
    sopra, dopo 10-30 s.
  - **Classi per analogia** nel livello classico: `type206`, `rimorchiatore`.
  - **La prova:** la foto della corvetta ormeggiata e il largo, esito 40, nessuna mina scoppiata; scoperti nelle acque
    interne, l'arcipelago chiuso e la mina che scoppia sotto il 688.

---

## 2h. Proposte del 30/10/2026 (terze): sotto la coda, nell'Atlantico e a casa loro

**L'utente ha approvato S19 e S20 il 30/10/2026; la S21 non l'ha scelta e resta qui come proposta.** Tre episodi del
dossier 23 non ancora usati, tutti di pedinamento ravvicinato e di collisione; la terza e' per l'Alfa, che e' in minoranza (20 missioni contro
15). Usano i pezzi gia' nel motore:
- le collisioni;
- le condizioni «giocatore vicino a» e «unità classifica»;
- le zone e le regole;
- il ping che fa allontanare un sottomarino dell'IA;
- il Crazy Ivan dei sovietici.

| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Episodio |
|---|---|---|---|---|---|---|
| LIB-S19 | Pietra santa | 688 | Barents [SOST] | pace | 4 | E05, *Gato* e K-19 (1969) (**costruita**) |
| LIB-S20 | Cinquecento iarde | 688 | Celtica [SOST] | pace | 3 | E24, l'*Augusta* (1986) (**costruita**) |
| LIB-S21 | La linea di base | Alfa | Curili, lato di Okhotsk [SOST] | pace | 4 | E28, *Baton Rouge* e *Kostroma* (1992, trasposto al 1989) (non approvata) |

### LIB-S19 — «Pietra santa»

- **Battello:** 688, al posto del *Gato* (classe Thresher/Permit) [SOST].
- **Teatro:** Barents [SOST minima]. Il fatto vero e' vicino all'ingresso del Mar Bianco, in 60 m d'acqua. Il Barents del
  gioco (70-72°N) non arriva alla costa di Kola e ha fondali di 150-360 m: le 12 miglia sono una zona sul margine sud,
  al posto della costa.
- **Data:** 15 novembre 1989 (vent'anni dopo, lo stesso giorno), 04:00Z, notte polare.
- **Meteo:** del teatro e della stagione: mare 3-4, nessuno strato (d'inverno il Barents e' mescolato).
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Una pattuglia d'informazione della serie Holy Stone: stare sotto la coda di un SSBN sovietico che esce verso il suo
    settore e registrarne la firma.
  - Restare oltre le 12 miglia; non farsi sentire dalle navi antisom che pingano a caso.
  - L'SSBN fa i Crazy Ivan per guardarsi la poppa.
- **Obiettivi:**
  - primari:
    - registrare l'SSBN: 40 minuti entro 4 km (variabile con «giocatore vicino a», come la S14);
    - non entrare nella zona delle 12 miglia;
    - tornare a casa;
  - secondari:
    - non farsi classificare (dalle navi antisom o dall'SSBN);
    - mai a meno di 500 m dall'SSBN.
- **Svolta:**
  - L'SSBN accosta di 120-180° col Crazy Ivan, che c'e' gia' nel motore, e torna indietro sulla sua scia.
  - Se il 688 e' sulla sua rotta alla stessa quota, e' la collisione del 1969 (esito 41): il K-19 la prese a 60 m.
- **Forze:** URSS:
  - un `delta3` [SOST: l'Hotel K-19, che nel gioco non c'e'], a 60-100 m;
  - due Grisha III con lo Shelon, a caso sulla rotta;
  - un Il-38 alto;
  - i pescherecci della costa.
- **Che cosa la rende diversa:** il pedinamento piu' stretto: piu' si sta vicini, meglio si registra, e piu' il Crazy
  Ivan fa paura.
- **Difficoltà:** 4. **Durata:** 3 ore.
- **Ispirata a:** E05, la collisione *Gato*/K-19 (15 novembre 1969, 07:13, Barents, 60 m). Il K-19 perse il sonar di
  prora e i portelli dei tubi, emerse con l'aria d'emergenza e rientro'; il *Gato* prosegui' la pattuglia `[WP-K19]`
  `[WP-GATO]` (alta per il fatto). Le armi nucleari pronte a bordo del *Gato* sono una sola testimonianza `[MQ-GATO]`
  (bassa), e la scheda non le usa.
- **Che cosa serve al motore:**
  - c'e': collisioni, Crazy Ivan, «giocatore vicino a», zone, «unità classifica», VDS e Shelon;
  - niente di nuovo.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S19_PIETRA_SANTA.lua` e il pacchetto `.m688m`.
  - L'SSBN (`delta3`) esce da 70,25°N verso nord a 6 nodi e 80 m; il 688 parte 6 km di fianco. Il Barents del gioco va da
    70,15 a 71,5°N, fondali di 150-360 m, senza costa: le 12 miglia sono un cerchio sul margine sud.
  - La firma conta 10 s a ogni controllo entro 4 km, fino a 2400 (40 minuti); a meno di 500 m cade l'obiettivo
    secondario; dentro le 12 miglia cade il primario «dodici».
  - Due Grisha III (`R_nave`, VDS Shelon) e un Il-38; tre BMRT.
  - Classi per analogia: `grisha3` nel livello classico.
  - La prova: 40 minuti a 2 km di poppa, esito 40, col Crazy Ivan spento per la prova; dentro le 12 miglia e a 400 m i due
    obiettivi cadono. A giocatore fermo, a 6 km di fianco (3 semi, build del 06/11/2026), non vince, esito 21, e viene scoperto: la prima volta
    a 2979 s dal Delta col seme 1, a 4202 e 8174 s da una Grisha coi semi 2 e 3.
  - Il cerchio delle 12 miglia ha raggio 22 224 m (12 miglia nautiche), con centro a 69,95°N 37,0°E.

### LIB-S20 — «Cinquecento iarde»

- **Battello:** 688.
- **Teatro:** Celtica [SOST], al posto dell'Atlantico occidentale (le Bermuda, secondo la versione sovietica).
- **Data:** 20 ottobre 1989, 02:00Z, notte.
- **Meteo:** del teatro e della stagione: autunno, mare 4.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Un SSBN sovietico in pattugliamento nell'Atlantico: il comando vuole la firma piu' pulita possibile e il suo schema
    di pattuglia.
  - La firma migliora con la distanza: da lontano basta, da vicino e' oro. Sotto le 500 iarde (457 m) il rischio cresce,
    e nessuno sa dove l'SSBN girera'.
- **Obiettivi:**
  - primari:
    - la firma: un punteggio che sale piu' in fretta da vicino (20 al controllo entro 2 km, 10 entro 5 km), fino a 1800;
    - tornare a casa;
  - secondari:
    - mai sotto le 500 iarde;
    - non farsi classificare;
    - stimare la rotta dell'SSBN tre volte (`rotta_stimata` con `da_tma`, dal 3/11/2026): dalla soluzione del MATE,
      quando la TMA del contatto ne ha una, entro 20° e ad almeno 5 minuti l'una dall'altra. Prima contava l'AOB al
      periscopio su unità diverse, e con un solo SSBN sempre immerso non si poteva raggiungere.
      Questo secondario oggi è solo del giocatore: nel vetrino il comandante IA non lo fa. Senza piano non fa la TMA sull'SSBN
      (0 stime in 5400 s); con un piano di pedinamento sul `delta3`, 0 stime in 7200 s col seme 1, perché insegue il Victor di
      scorta e non fa le gambe (runtime, 4/11/2026).
- **Svolta:**
  - L'SSBN cambia schema dopo un'ora: rallenta, scende sotto lo strato e fa due Crazy Ivan ravvicinati.
  - A meno di 500 iarde la collisione e' possibile: esito 41, il sonar di prora del 688 da rifare.
- **Forze:**
  - URSS: un `delta3` [SOST: il Delta I degli autori americani, o lo Yankee della versione sovietica], un `victor3` di
    scorta a 10-15 km che puo' sentire il pedinatore;
  - neutrali: traffico civile.
- **Che cosa la rende diversa:** la distanza come scelta. Ogni chilometro in meno vale di piu', e costa di piu'.
- **Difficoltà:** 3. **Durata:** 2 ore e 30.
- **Ispirata a:** E24, l'*Augusta* nell'ottobre 1986.
  - Fatti certi: il Pentagono parlo' di danni da una collisione subacquea durante una pattuglia; la cupola del sonar fu
    sostituita (2,7 milioni di dollari); bacino alla Electric Boat dal 31 ottobre al 13 dicembre 1986 (Wikipedia, USS
    *Augusta*).
  - Contesto: con chi. Per la Marina sovietica il K-219, lo Yankee affondato poi al largo delle Bermuda, il 3 ottobre;
    per gli autori americani il Delta I K-279 il 20 ottobre `[WP-K279]` `[GW2RU]`. La scheda non sceglie: «un SSBN
    sovietico».
- **Che cosa serve al motore:**
  - c'e': collisioni, Crazy Ivan, «giocatore vicino a» (due soglie in due regole), `rotta_stimata`, la scorta col
    pedinamento in pace;
  - niente di nuovo.

- **Costruita il 30/10/2026:** `data/missioni/libreria/LIB_S20_CINQUECENTO_IARDE.lua` e il pacchetto `.m688m`.
  - L'SSBN (`delta3`) pattuglia un circuito di 9 km attorno a 48,55°N 9,30°W a 5 nodi e 90 m; un Victor III di scorta
    su un circuito a 13 km.
  - La firma: due regole, 10 a ogni controllo entro 5 km e 20 in piu' entro 2 km, fino a 1800. Da 1,5 km bastano circa
    dieci minuti, da 4 km mezz'ora. Sotto le 500 iarde (457 m) cade l'obiettivo secondario.
  - Dopo un'ora il nuovo schema: l'azione «quota» verso i 130 m e 3 nodi. Dove l'acqua e' poca, il controllo del fondo
    dell'IA lo tiene a circa 105 m.
  - La prova: da 1,5 km la firma in meno di 15 minuti, da 4 km in piu' di 28; a 400 m l'obiettivo cade; dopo un'ora
    l'SSBN scende e rallenta.
  - Il 688 parte con la prua lontano dal circuito: nella prima versione, puntato verso l'SSBN, vinceva senza ordini a 6070 s.

### LIB-S21 — «La linea di base»

*Proposta non approvata dall'utente (30/10/2026): non si costruisce.*


- **Battello:** Alfa, al posto del Sierra II *Kostroma* [SOST]. Il briefing lo dice.
- **Teatro:** il fatto vero e' al largo dell'isola di Kildin, vicino a Severomorsk. **Il Barents del gioco non arriva
  alla costa di Kola**: il teatro piu' vicino con una costa sovietica e le sue linee di base e' quello delle Curili, lato
  di Okhotsk, lungo Iturup e Kunashir [SOST]. La scheda lo dichiara.
- **Data:** 11 febbraio 1989 (trasposto: il fatto e' del 1992, dopo l'URSS), 09:00Z (le 20 locali, l'ora del fatto).
- **Meteo:** del teatro e della stagione: inverno, frangenti sotto costa che coprono i rumori (`meteo_realistico`).
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Un sottomarino americano si apposta vicino alla base. Per gli americani e' fuori dalle 12 miglia misurate dalla costa;
    per l'URSS e' dentro le acque territoriali misurate dalla linea di base retta. La stessa posizione e' legale per uno
    e illegale per l'altro.
  - Trovarlo, classificarlo e farlo uscire: il ping vicino lo fa allontanare, come una nave dell'IA scoperta.
- **Obiettivi:**
  - primari:
    - trovare e classificare il 688 (`scoprire`);
    - farlo uscire dalla zona rivendicata (variabile: il 688 dell'IA fuori dal cerchio);
    - tornare a casa;
  - secondari:
    - nessuna collisione;
    - farlo senza farsi classificare da lui.
- **Svolta:**
  - Il 688 dell'IA sta fermo e muto a quota periscopica. Quando lo si cerca risalendo, il suo cono poppiero e' cieco: e'
    il momento del 1992, quando il *Kostroma* in risalita urto' da sotto la poppa del *Baton Rouge*.
  - Una collisione chiude con un incidente diplomatico (esito 41), anche se nel folklore della Flotta fu una vittoria.
- **Forze:**
  - USA: un `los_angeles`, fermo a quota periscopica, paranoia 2, che si allontana se pingato vicino;
  - URSS: due Grisha III del pattugliamento costiero, un Il-38, il traffico locale.
  - Le due zone: le 12 miglia dalla costa e la zona rivendicata dalla linea retta, piu' larga.
- **Che cosa la rende diversa:** la caccia «dalla parte di casa», e una frontiera che dipende da chi la misura.
- **Difficoltà:** 4. **Durata:** 2 ore e 30.
- **Ispirata a:** E28, *Baton Rouge* e *Kostroma* (11 febbraio 1992, 20:16 locali, al largo di Kildin). Per la Marina
  americana oltre le 12 miglia; il *Baton Rouge* ebbe due tagli, ammaccature e graffi e fu radiato nel 1995 (Wikipedia,
  USS *Baton Rouge*); il *Kostroma* fu riparato entro giugno (Wikipedia, *Kostroma*). Per la Russia la collisione fu
  dentro le acque misurate dalla linea retta `[WP-KIL]` (media). Chi pedinava chi resta conteso.
- **Che cosa serve al motore:**
  - c'e': collisioni, zone, `scoprire`, «unità classifica», il ping che fa allontanare un sottomarino dell'IA, la condizione
    «unita' in zona» (per il 688 dell'IA dentro o fuori il cerchio rivendicato);
  - da verificare: che il 688 dell'IA fermo a quota periscopica non senta l'Alfa che risale nel suo cono poppiero.

---

## 2i. Proposte del 31/10/2026: tre per l'Alfa e una per il 688

**L'utente le ha approvate tutte e quattro il 31/10/2026.** Ordine di costruzione: S25, S22, S23, S24. L'Alfa era in
minoranza: 15 missioni contro 22. Quattro episodi del dossier 23 non
ancora usati: tre diventano missioni dell'Alfa, uno del 688. Come vuole il dossier 23 §0.2, le missioni dell'Alfa sono
**per ruolo**: nessun Alfa reale lasciò il Nord e fece queste cose. Il briefing lo dice. Le regole del dossier 23 §5
valgono per tutte:
- nomi di fantasia per battelli e persone;
- nessuna morte come obiettivo o come spettacolo, e il giocatore non vede affondare un battello ispirato a uno vero;
- gli episodi contesi restano contesi.

| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Episodio | Motore |
|---|---|---|---|---|---|---|---|
| LIB-S22 | La rotta lunga | Alfa | Celtica [SOST] | pace | 4 | E25, l'operazione Atrina (1987) (**costruita**) | quasi tutto c'è; una condizione nuova (0,5 giorni) |
| LIB-S23 | Acqua a due gradi | Alfa | Barents [SOST] | pace | 3 | E27, K-278 *Komsomolets* (7 aprile 1989) (**costruita**) | due pezzi piccoli (0,5 giorni); il soccorso vero è M8 (2-3 giorni) |
| LIB-S24 | Quota zero | Alfa | Curili [SOST] | pace | 3 | E06, *Tautog* e K-108 (1970) (**costruita**) | un comportamento da allargare (1 giorno) |
| LIB-S25 | Ombra del boomer | 688 | Celtica [SOST] | pace | 3 | E23, K-219 (1986) (**costruita**) | tutto c'è; facoltativo il fumo senza danno (0,5 giorni) |

Le stime sono in giorni di lavoro di una sessione e sono **STIMA**.

### LIB-S22 — «La rotta lunga»

- **Battello:** Alfa, al posto di uno dei cinque Victor III della 33ª divisione [SOST per ruolo]. Gli altri quattro sono
  battelli dell'IA.
- **Teatro:** Celtica [SOST], al posto dell'Atlantico occidentale e del Mar dei Sargassi. L'acqua è profonda e aperta,
  lontana dalla barriera Islanda-Faroe, che è già della S09 «Aport». La barriera è passata: la missione comincia dopo.
- **Data:** 20 aprile 1989 (il fatto vero è di marzo-maggio 1987), 06:00Z.
- **Meteo:** del teatro e della stagione (`meteo_realistico`): primavera, strato superficiale debole, mare 3-4.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Cinque sottomarini d'attacco sono usciti insieme da Zapadnaja Lica per una rotta lunga, attorno alla Groenlandia, per
    non passare sopra il SOSUS.
  - Ora sono nell'Atlantico aperto, divisi su cinque rotte verso ovest.
  - Il comando vuole sapere due cose: se la NATO li ritrova, e se nella zona pattuglia un SSBN americano.
- **Obiettivi:**
  - primari:
    - a fine missione almeno tre dei cinque battelli (Alfa compreso) senza un inseguitore NATO che li tenga;
    - nessuna arma;
  - secondari:
    - trovare e classificare l'SSBN americano (`ohio`), se c'è;
    - l'Alfa mai classificato dalla NATO per più di 20 minuti di seguito.
- **Svolta:**
  - Dalla prima ora il SOSUS dà un datum alla NATO su un battello a caso (l'azione «cue» c'è già). Un P-3C va sul datum
    e posa le boe; una Perry col TACTAS e un Los Angeles accorrono.
  - A metà missione un secondo datum: la NATO ha ritrovato un altro battello.
- **Forze:**
  - URSS: l'Alfa e quattro `victor3` dell'IA, su rotte a punti a 6-8 nodi e 150-250 m, che si allontanano quando li
    pingano (c'è già);
  - NATO: due `p3c` a turno, una `perry` col TACTAS, un `los_angeles`; l'`ohio` lento e profondo, a caso in un settore;
  - il traffico atlantico (`mercantile`, `portarinfuse`).
- **Che cosa la rende diversa:** si gioca per gli altri: conta quanti battelli arrivano in fondo senza nessuno in coda,
  Alfa compreso.
- **Difficoltà:** 4. **Durata:** 4 ore.
- **Ispirata a:** E25, l'operazione Atrina (marzo-maggio 1987). Cinque Victor III (K-244, K-255, K-298, K-299, K-524)
  uscirono da Zapadnaja Lica per una rotta più lunga attorno alla Groenlandia, per evitare il SOSUS del varco GIUK
  `[WP-ATR]`. Il 7 aprile 1987 la US Navy disse che operavano vicino alle Bermuda e che aveva mandato forze a
  sorvegliarli `[USNI20]`.
  - Conteso: per gli autori russi è un trionfo quasi leggendario; per le fonti occidentali almeno quattro battelli su
    cinque furono seguiti, con fatica `[WP-ATR]`. Un testimone americano scrive che la NATO li seguì «dal momento in cui
    lasciarono il porto» `[WH-ATR]`.
  - La missione non decide: con tre battelli su cinque l'esito è aperto per costruzione.
  - Nessun Alfa partecipò: la scheda lo dichiara (dossier 23 §0.2).
- **Che cosa serve al motore:**
  - c'è: la rotta a punti dei sottomarini dell'IA, il ping che li fa allontanare, il «cue» SOSUS, l'ASW aereo con le
    boe, il TACTAS, «unità classifica», l'obiettivo «scoprire»;
  - **una condizione nuova**, «unità tenuta da una parte»: una qualsiasi unità della parte X ha la traccia su
    un'unità Y. Oggi «unità classifica» vuole la coppia cacciatore-preda, e con cinque prede e quattro cacciatori
    servono venti regole. **STIMA 0,5 giorni** nello scenario e nell'editor (miei); cambia l'header di
    `scenario_missione`, quindi va fatta in una finestra concordata col runtime.

- **Costruita il 31/10/2026:** `data/missioni/libreria/LIB_S22_ROTTA_LUNGA.lua` e il pacchetto `.m688m`.
  - **Dove:** i quattro Victor III partono su 48,35°N fra 10,0 e 11,1°W, a 150-240 m e 7 nodi, e scendono per 70 km a
    ventaglio, a 180°, 205°, 230° e 255°. L'Alfa parte in mezzo, a 200 m e 4 nodi. Davanti cercano una Kidd e due Los
    Angeles; l'Ohio pattuglia lento a 47,3°N.
  - **Il SOSUS:** a 30 minuti e a 2 ore un datum (azione «cue») sulla posizione prevista di un Victor, ed entra un P-3C. I
    P-3 non hanno bersagli dichiarati, quindi cercano tutti i sottomarini sovietici, Alfa compreso.
  - **Tenuti e liberi:** un battello è tenuto se un cacciatore (i due P-3, la Kidd, i due Los Angeles) ha accumulato 70
    minuti di contatto su di lui («unità vede»). Con tre liberi su cinque, Alfa compreso, l'esito è 40; altrimenti 41.
    Un'arma lanciata chiude con l'esito 42.
  - **Ritarata dopo il giro lungo** (con le evasioni tarate del disassemblatore):
    - il criterio «nessuno lo sente da 20 minuti» non distingueva l'esca dal nascosto: con le zone di convergenza il Los
      Angeles sentiva anche i Victor a 65 km;
    - la Perry col TACTAS sentiva tutti dovunque andasse, quindi ora c'è una Kidd col sonar di scafo e senza schiera;
    - le rotte a ventaglio e il secondo Los Angeles.
  - **La prova:** i fatti che valgono su tutti i semi misurati, e l'esito che segue i liberi: a 20 nodi Kidd e Los Angeles
    tengono l'Alfa, a 4 nodi la Kidd non lo tiene (e tiene un Victor, dopo 222-234 minuti, in 4 semi su 36). Chi passa dipende molto dal seme. Semi 1-12, build del 06/11/2026: a 4 nodi i liberi sono
    2 2 2 3 1 2 2 2 2 2 1 2 (media 1,92), esito 40 in 1 seme su 12 (al 95%, 1-35%); a 20 nodi 4 2 3 4 1 0 4 0 2 2 4 2 (media 2,33), esito 40 in 5 su 12 (19-68%). Questi semi hanno suggerito che correre renda di più;
    la prova sono i semi 13-36, stessa build e stessa politica: a 4 nodi 3 2 2 2 3 2 1 3 3 3 3 1 2 1 2 2 3 2 3 2 1 3 2 3 (media 2,25), 10 su 24 (24-61%); a 20 nodi 2 3 4 4 4 1 3 4 2 3 2 2 3 2 2 3 1 2 4 2 2 4 0 1 (media 2,50), 11 su 24 (28-65%).
    Sei semi vincono solo di corsa, cinque solo da nascosto: lo scarto è di 4 punti, al 95% fra -22 e +30, e i liberi salgono in media di 0,25 (da -0,25 a +0,75). Il vantaggio non regge (T12 §5). Il 05/11, a 4 nodi: media 2,25, 4 su 12; con la stessa build l'1 su 12 dei semi 1-12 sta accanto al 10 su 24 dei 13-36, quindi non indica un peggioramento della missione, e la causa non è stata cercata.
  - **Lo scatto come esca è stato provato e tolto (05/11/2026, decisione dell'utente):** la scheda iniziale diceva che uno
    scatto a 40 nodi vicino a un Victor inseguito tirava via i cacciatori. Non succede: i contatti non si escludono, il
    conteggio di un Victor continua a crescere finché resta nel raggio, e «tenuto» non si cancella.
    - Giocatore che scatta, 12 semi, sul codice del mattino del 05/11: esito 40 in 0 semi, media 0,92 liberi, contro 4 semi
      e 2,25 da nascosto. Rigiocato sulla build del 06/11/2026: 1 seme e 1,42 liberi, contro 1 seme e 1,92.
    - Con in più una regola che dirotta sull'Alfa i Los Angeles che l'hanno davvero in contatto (solo su una copia, sul
      codice del 05/11): sempre 0 semi e 0,92 liberi, e il Victor perde il contatto in 1 scatto su 19. La regola non è
      entrata nella missione.
    - Limite: una sola politica di gioco, con un pilota che sa chi è inseguito; è a favore dell'esca, ma un giocatore
      potrebbe trovarne un'altra. Misure in `tools/re/prove/s22_esca/`.
  - **Da sapere:** i contatti dei P-3 dalle boe entrano solo in parte nella storia dei contatti delle unità, quindi
    pesano poco nel conteggio.

### LIB-S23 — «Acqua a due gradi»

- **Battello:** Alfa. Il battello in difficoltà è di fantasia: un sottomarino sperimentale, un `akula` [SOST: il Mike,
  unico, nel gioco non c'è].
- **Teatro:** Barents [SOST]. Il fatto vero è nel Mare di Norvegia, circa 180 km a sud-ovest dell'Isola dell'Orso
  (73,7°N); il Barents del gioco va da 70 a 71,5°N. Il freddo, il mare grosso e l'aria delle basi di Kola sono gli
  stessi.
- **Data:** 18 aprile 1989 (non il giorno del fatto, per rispetto), 09:00Z.
- **Meteo:** fisso: mare 5, vento 15 m/s, nubi 8/8 basse, visibilità 4 km; acqua a 2 °C.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Un battello sperimentale ha un incendio a poppa a 300 m ed è emerso con l'aria d'emergenza. Il reattore è fermo, il
    fuoco continua.
  - L'Alfa è il battello più vicino, a 60 km. Le navi sono molto più lontane: un peschereccio-madre a 40 miglia, un
    rimorchiatore della flotta a 8 ore.
  - Due Il-38 decollano da Kola. Un Orion norvegese è già in volo sulla zona.
- **Obiettivi:**
  - primari:
    - arrivare entro 2 km dal battello entro 90 minuti;
    - dare la posizione esatta al peschereccio-madre (trasmettere da quota periscopica, con il battello entro 5 km): da
      quel momento il peschereccio accosta e va a tutta;
  - secondari:
    - restare entro 5 km fino all'arrivo del peschereccio;
    - gli uomini salvati (il conteggio del debriefing, sotto).
- **Svolta:**
  - Alla terza ora il battello in difficoltà si appoppa e l'equipaggio va in acqua. Il giocatore non lo vede affondare:
    un messaggio radio lo dice, e il battello esce dal gioco.
  - Gli Il-38 lanciano le zattere, ma il mare le rovescia (un messaggio).
  - Arriva una proposta dalla Norvegia sulla frequenza di soccorso: una nave della guardia costiera può essere lì in
    circa un'ora (22 km a circa 11 nodi col mare 5). Il giocatore può accettarla. Il messaggio di risposta c'è già fra quelli «a richiesta» della sala radio. Se
    accetta, la nave entra prima del peschereccio. Il debriefing lo registra senza giudicare.
- **Il conteggio dei salvati (senza M8):**
  - gli uomini in acqua sono 40 (di fantasia);
  - il peschereccio li raccoglie quando arriva entro 300 m;
  - quanti ne trova vivi dipende da quanto tempo sono stati in acqua: 35 entro 20 minuti, 25 entro 45, 15 entro 80, 5
    oltre (**STIMA** dalla sopravvivenza in acqua a 2 °C);
  - lo fanno le regole a tempo, con una variabile: c'è già.
- **Forze:**
  - URSS: il battello in difficoltà (`akula`, in avaria, in superficie); due `il38`; il peschereccio-madre (`bmrt`);
    il rimorchiatore (`rimorchiatore`), troppo lontano per contare;
  - Norvegia: un `p3c` della parte del luogo, che osserva e non fa altro; la nave della guardia costiera (una `oslo`
    [SOST], di riserva, entra solo se il giocatore accetta).
- **Che cosa la rende diversa:** la prima missione senza nemici. L'unica ostilità è il mare, e si contano i salvati, non
  i colpi.
- **Difficoltà:** 3. **Durata:** 4 ore.
- **Ispirata a:** E27, K-278 *Komsomolets* (7 aprile 1989, l'anno del gioco).
  - A 335 m un corto circuito incendiò un compartimento di poppa; il battello emerse 11 minuti dopo e affondò circa
    quattro ore più tardi, in 1680 m d'acqua `[WP-K278]`.
  - Gli aerei lanciarono piccole zattere, che il vento e il mare resero inutilizzabili. Il peschereccio *Aleksej
    Chlobystov* arrivò 81 minuti dopo l'affondamento. Dei 69 uomini a bordo se ne salvarono 27; molti morirono di
    ipotermia nell'acqua a 2 °C `[WP-K278]` (alta).
  - L'equipaggio di un Orion norvegese che girava sopra il battello vide la tragedia; i comandi della Flotta del Nord non
    chiesero mai aiuto alla Norvegia `[BO-K278]` (media).
  - Battello e uomini sono di fantasia; i nomi veri restano solo nella nota storica facoltativa.
- **Che cosa serve al motore:**
  - c'è: l'avaria (M6) col battello in superficie, l'ASW aereo come pattugliatore, i messaggi «a richiesta» con
    `se_ricevuto`, le riserve, le regole a tempo, «giocatore ha trasmesso», «giocatore vicino a»;
  - **un'azione «togli unità»** (il battello esce dal gioco senza essere distrutto, come l'opcode 37h dell'originale):
    **STIMA 0,25 giorni**;
  - **il fumo senza danno** sul battello in superficie (gli effetti del fuoco ci sono, ma partono dai colpi): **STIMA
    0,25 giorni**, col kit;
  - **meglio con M8**, il soccorso vero: uomini in acqua con la curva dell'ipotermia, raccolti da navi ferme vicino,
    zattere visibili. **STIMA 2-3 giorni** del runtime, più le zattere del kit. Senza M8 basta il conteggio a tempo.

- **Costruita il 31/10/2026:** `data/missioni/libreria/LIB_S23_ACQUA_A_DUE_GRADI.lua` e il pacchetto `.m688m`.
  - **Dove:** il battello (`akula`, in avaria in superficie) a 71,3°N 35°E; l'Alfa 60 km a sud, a 10 nodi. Il
    peschereccio-madre (`bmrt`) parte 55 km a sud-est, a 8 nodi, verso una posizione sbagliata 15 km a nord del battello.
  - **La posizione:** entro 5 km dal battello, un rapporto di situazione, e il peschereccio accosta e va a tutta (col
    mare 5 arriva a 12 nodi). Senza, l'Il-38 dà la posizione mezz'ora dopo l'affondamento.
  - **L'equipaggio in acqua** alla terza ora. I salvati dipendono da quando arriva la prima nave: 40 se prima, poi 35,
    25, 15 e 5 secondo i minuti in acqua (STIMA). Ogni fascia ha il suo codice d'esito, 40 e 44-47 per il peschereccio,
    50-54 per la nave norvegese, e la categoria la decidono gli obiettivi.
  - **La Norvegia:** a 2 ore e mezza la proposta sulla frequenza di soccorso; si accetta con una trasmissione di tipo
    «richiesta». La nave della guardia costiera (`oslo`, neutrale) entra 22 km a ovest e va a 20 nodi.
  - **L'Orion norvegese** gira attorno al battello, neutrale. Il motore gli dà comunque le boe passive, ma niente ping e
    niente attacchi simulati.
  - **Il battello resta in superficie:** una regola ripetuta gli tiene la quota 0, perché l'evasione dai sorvoli lo
    farebbe scendere.
  - **«Togli unità»:** all'affondamento il battello esce dal gioco senza essere distrutto (azione nuova dello scenario,
    l'opcode 37h): il giocatore non lo vede andare giù.
  - **«Salvati»:** a ogni chiusura l'azione nuova `salvati` passa al motore gli uomini salvati sui 40 in acqua
    (`m688.salvati`), per il riconoscimento «tutti salvati» e per la carriera.
  - **La prova:** col rapporto da 3 km a 50 minuti il peschereccio arriva prima dell'affondamento, 40 salvati. Senza
    rapporto, e con l'Alfa lontano, 15 salvati, come nel 1989. Senza rapporto ma con l'aiuto norvegese chiesto subito,
    35. Senza ordini la partita si chiude con 15 salvati.

### LIB-S24 — «Quota zero»

- **Battello:** Alfa, al posto dell'Echo II K-108 [SOST per ruolo].
- **Teatro:** Curili [SOST], al posto delle acque di Petropavlovsk-Kamčatskij (53°N): fuori dallo Stretto di Ekaterina,
  verso il Pacifico, su fondali di 1000-3000 m.
- **Data:** 20 giugno 1989 (il fatto vero è del 20 giugno 1970; nessun morto), 02:00Z.
- **Meteo:** del teatro e della stagione: estate, strato a 40-60 m, nebbia.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - L'Alfa fa un'esercitazione d'attacco su una Krivak: tre passaggi, a orari dati, con un siluro d'esercitazione
    (l'azione «esercitazione» c'è già).
  - Il comando sospetta che un sottomarino americano segua le unità in esercitazione: se c'è, va trovato, classificato
    e allontanato.
- **Obiettivi:**
  - primari:
    - i tre passaggi d'esercitazione;
    - classificare il pedinatore;
    - nessuna collisione;
  - secondari:
    - allontanarlo: tre ping col contatto entro 2 km, poi lui se ne va (il ping che fa allontanare un sottomarino
      dell'IA c'è già);
    - riferire la classificazione al comando.
- **Svolta:**
  - Il pedinatore (`sturgeon`) sta nel cono sordo dell'Alfa, a 300-800 m. Il sonar passivo dà il rilevamento, non la
    quota. Il pedinatore cambia quota mentre l'Alfa accosta: sale, poi scende.
  - Col Crazy Ivan (accostare di 180° per guardarsi la poppa) l'Alfa lo sente, ma più stretto lo cerca, più rischia di
    passargli sopra o sotto alla stessa quota.
  - La collisione è l'esito 41, come nel 1970: le eliche dell'Alfa contro la falsa torre. Il registro scrive
    «il contatto si è spezzato», e la missione non dice di più.
- **Forze:**
  - URSS: la Krivak bersaglio (`krivak2`); un Ka-27 dell'esercitazione;
  - USA: lo `sturgeon` pedinatore;
  - pescherecci giapponesi e sovietici sul bordo del teatro.
- **Che cosa la rende diversa:** si gioca dal lato di chi è pedinato, e la difficoltà è la quota che il sonar non dà.
  È il contrario della S19 e della S20, dove il giocatore è il pedinatore.
- **Difficoltà:** 3. **Durata:** 2 ore.
- **Ispirata a:** E06, il *Tautog* e il K-108 (20 giugno 1970, al largo di Petropavlovsk-Kamčatskij).
  - Il *Tautog* pedinava l'Echo II K-108, che gli stette vicinissimo a lungo senza saperlo. Gli idrofoni del *Tautog* non
    davano la quota: l'altro sembrò salire, poi scendere, poi urtò col ventre la falsa torre `[WP-TAU]`.
  - Il K-108 perse un'elica e rientrò a Petropavlovsk. Il *Tautog* sentì rumori che credette lo sfasciarsi dell'altro e
    se ne andò: un pezzo d'elica era ancora incastrato nella vela a Pearl Harbor. Nessun morto; i due paesi negarono,
    il fatto uscì nel 1991 `[WP-TAU]` (media).
  - Qui il ruolo del K-108 è dell'Alfa, per ruolo e non per identità.
- **Che cosa serve al motore:**
  - c'è: collisioni, Crazy Ivan, l'esercitazione, il ping che fa allontanare un sottomarino, «unità classifica», la
    condizione su un evento (il ping proprio), «giocatore vicino a»;
  - **da allargare:** il pedinamento dell'IA a distanza ravvicinata (300-800 m nel cono sordo) e con la quota che
    cambia quando il pedinato accosta. **STIMA 1 giorno** del runtime. Senza questo il pedinatore resta a 2-4 km e la
    missione si fa più facile.

- **Costruita il 31/10/2026:** `data/missioni/libreria/LIB_S24_QUOTA_ZERO.lua` e il pacchetto `.m688m`.
  - **Dove:** le acque profonde a sud-est delle Curili, verso 43,5°N 147,65°E, con fondali di 2000-3000 m. La nave
    bersaglio gira a 5 km dal punto; l'Alfa parte 18 km a ovest, a 100 m e 8 nodi; lo Sturgeon (R_pedina, 0,4 miglia)
    1,5 km dietro.
  - **La nave bersaglio è un vecchio mercantile:** una Krivak col Vega trovava da sé lo Sturgeon, lo pingava e lo
    inseguiva, facendo il lavoro del giocatore.
  - **Le armi sono d'esercitazione** (`esercitazione`): l'obiettivo è un colpo sulla nave bersaglio. I «tre passaggi»
    della scheda sono uno solo, perché «colpire» conta le unità e non i colpi.
  - **Classificare il pedinatore** (obiettivo «scoprire», primario): il sonarista non classifica uno Sturgeon silenzioso
    nel cono sordo, quindi la classificazione la fa il giocatore a mano. Conta se giusta al primo tentativo (registro del
    runtime, dal 31/10/2026). Il primo passo, secondario, è sentirlo: 2 minuti di contatto (obiettivo «contatto»).
  - **Allontanarlo:** classificato e pingato, se l'Alfa non lo sente più per 15 minuti. Con l'esercitazione riuscita la
    missione chiude con l'esito 40; l'urto con l'esito 41.
  - **Il pedinamento ravvicinato** (comportamento nuovo `R_pedina_vicino`, col `m688.pedina_vicino` del runtime): con una
    buona traccia lo Sturgeon si porta a 400 m nel cono sordo, 20 m sotto l'Alfa come nel 1970, e ne segue rotta, velocità e
    quota con 20-60 s di ritardo. Evita l'urto da sé, ma un'accostata o un arresto brusco dell'Alfa possono portare
    all'urto. Se l'Alfa pinga entro 3 km smette, e l'evasione dell'IA lo porta via.
  - **La prova:** senza manovre lo Sturgeon si porta da 1500 a circa 300 m in dieci minuti (3 semi, build del 06/11/2026). Con un
    Crazy Ivan ogni 5 minuti i due minuti di contatto arrivano a 420 s; classificato a mano e pingato se ne va (a 1380 s è a 8,5 km; seme 1). L'Alfa sullo Sturgeon alla stessa quota:
    esito 41.

### LIB-S25 — «Ombra del boomer»

- **Battello:** 688, al posto dell'*Augusta* (che era un Los Angeles).
- **Teatro:** Celtica [SOST], al posto dell'Atlantico a circa 1090 km a nord-est delle Bermuda: acqua profonda e
  aperta come la piana abissale.
- **Data:** 10 ottobre 1989 (non il giorno del fatto), 04:00Z.
- **Meteo:** del teatro e della stagione (`meteo_realistico`): autunno, mare 4-5.
- **Situazione e ROE:** pace. Armi trattenute.
- **Premessa:**
  - Il 688 è in coda a un SSBN sovietico in pattuglia: è il compito normale dei Los Angeles negli anni '80.
  - Gli ordini sono di tenerlo, registrarlo e riferire ogni 4 ore, senza farsi sentire.
- **Obiettivi:**
  - primari:
    - tenere il contatto (traccia o rilevamento) per almeno l'80% del tempo;
    - non farsi classificare dall'SSBN;
  - dopo la svolta:
    - riferire subito l'emergenza (trasmettere entro 30 minuti);
    - non avvicinarsi a meno di 5 miglia dalle navi sovietiche che arrivano;
  - secondari:
    - mai a meno di 1000 iarde dall'SSBN.
- **Svolta:**
  - A un'ora a caso fra la seconda e la quarta l'SSBN emerge in emergenza e si ferma: c'è un incendio nel compartimento
    dei missili. Il 688 sente il transitorio, poi l'aria d'emergenza, poi niente.
  - COMSUBLANT ordina: restate nascosti, riferite, non interferite. Un P-3C arriva a guardare.
  - Arrivano le navi sovietiche: un mercantile, poi un rimorchiatore. Tentano il rimorchio, che non tiene. Quando lo
    mollano, COMSUBLANT richiama il 688 e la missione finisce: il giocatore non vede affondare il battello.
  - **La versione contesa:** se il 688 era a meno di 1000 iarde dall'SSBN nei 10 minuti prima dell'emergenza, arriva
    un messaggio: i sovietici dicono che c'è stata una collisione. La missione non decide; il debriefing riporta che la
    US Navy l'ha sempre negata.
- **Forze:**
  - URSS: un `delta3` [SOST: lo Yankee I, che nel gioco non c'è]; un `mercantile` e un `rimorchiatore` di riserva;
  - USA: un `p3c` di riserva, che arriva dopo l'emergenza;
  - il traffico atlantico.
- **Che cosa la rende diversa:** la domanda morale del dossier 23: che cosa fa il pedinatore quando il bersaglio è in
  emergenza? Niente, e niente è difficile.
- **Difficoltà:** 3. **Durata:** 6 ore (con la compressione del tempo).
- **Ispirata a:** E23, K-219 (3-6 ottobre 1986, circa 1090 km a nord-est delle Bermuda).
  - L'acqua entrò nel tubo 6 e si mescolò al propellente del missile; ci fu un'esplosione e gas tossici. I due reattori
    furono spenti a mano, e il marinaio che spense il secondo morì. Il comandante emerse a batteria. Un mercantile
    sovietico tentò di rimorchiarlo, ma il rimorchio non riuscì; il battello affondò il 6 ottobre, a circa 6000 m
    `[WP-K219]` (alta).
  - L'*Augusta* operava nella zona. I sovietici parlarono di una collisione; la US Navy lo nega «categoricamente», e lo
    negano anche il comandante Britanov e Kurdin `[WP-K219]`. Il dossier 23 tratta la collisione come **non avvenuta**.
  - Battello e uomini sono di fantasia; i nomi veri restano solo nella nota storica facoltativa.
- **Che cosa serve al motore:**
  - c'è: l'avaria (M6) e l'azione «quota» per far emergere e fermare l'SSBN; le riserve; «rimorchia» e «molla
    rimorchio»; «giocatore ha trasmesso»; «giocatore vicino a»; «contatto su» e «senza contatto su»;
  - **facoltativo:** il fumo senza danno sull'SSBN in superficie, lo stesso pezzo della S23 (**STIMA 0,25 giorni**, già
    contato là).

- **Costruita il 31/10/2026:** `data/missioni/libreria/LIB_S25_OMBRA_DEL_BOOMER.lua` e il pacchetto `.m688m`.
  - **Dove:** la piana abissale a sud-ovest del ciglio, 47,0-47,6°N e 9,5-11,5°W, fondali di 4300-4800 m. L'SSBN
    (`delta3`, 80 m, 4 nodi) scende da 20 km a nord-ovest verso il suo settore e ci gira; il 688 parte 8 km a sud del
    suo punto d'ingresso.
  - **L'emergenza:** dopo le due ore, con l'SSBN nel settore e il contatto tenuto almeno 60 minuti (cumulati); comunque
    a tre ore e mezza. L'SSBN perde il comportamento, emerge e, in superficie, va in avaria (M6). Una regola ripetuta
    gli tiene la quota 0, perché l'evasione dell'IA (sorvoli, tonfi) lo riporterebbe giù. Niente azione «rumore»
    sull'SSBN: il suo stesso tonfo lo faceva evadere.
  - **Le navi:** il mercantile sovietico entra quando l'SSBN è in superficie, 30 km a sud-est, a 14 nodi; quando arriva
    lo prende a rimorchio (3 nodi, cavo di 300 m) verso est. Dopo 50 minuti il cavo si spezza (azione «molla
    rimorchio»). Il rimorchiatore della flotta entra a un'ora dall'emergenza, troppo lontano per contare; il P-3C a 40
    minuti, come aereo ASW di pace sull'SSBN, senza cercare il 688.
  - **Il rapporto:** un rapporto di situazione entro 30 minuti dall'emergenza (la condizione «ha trasmesso» col tipo
    `situazione`); poi arrivano gli ordini di COMSUBLANT, che arrivano comunque a 45 minuti.
  - **La fine:** 5 minuti dopo il cavo spezzato COMSUBLANT richiama il 688 (esito 40). Il giocatore non vede affondare
    il battello. Se il 688 è stato a meno di 1000 iarde dall'SSBN, prima del richiamo arriva la versione sovietica
    della collisione.
  - **Semplificazioni:** l'ora dell'emergenza viene dal contatto e dalla zona, non dal caso (le regole non hanno il
    caso); nessun fumo.
  - **Trovato costruendo:** le condizioni sulle variabili ammettono solo `=`, `>` e `<`, e lo scrittore trattava `>=` e
    `<=` come `=` senza dirlo. Ora la verifica dello scenario dà un errore (`regola_confronto`).
  - **La prova:** in coda a 2,5 km fino all'emergenza (7220 s); il rapporto di situazione da 12 km a nord-ovest; il
    rimorchio a 11035 s, il cavo spezzato e il richiamo, esito 40 a 14305 s, con «riferito» raggiunto; a 800 m l'obiettivo
    delle 1000 iarde cade.

**Nota sulle fonti nuove** (le altre sono nel dossier 23 §8):
- `[WH-ATR]` David Chetlain, «Cold War Operation Atrina», *The War Horse*, 24/1/2024:
  https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/ (media: testimone diretto, lato
  USA).
- `[BO-K278]` «20 years since Komsomolets sank», *The Barents Observer*, 2009:
  https://www.thebarentsobserver.com/news/20-years-since-komsomolets-sank/242148 (media).

## 3. Che cosa serve al motore e all'editor (dalle schede)

| Esigenza | Schede | Di chi | Stato |
|---|---|---|---|
| Obiettivo «contatto»: minuti di contatto sonar cumulativi con un'unità | I01, I09, S03, S10 | runtime (registro M1) | fatto: tipo `contatto` (docs/09 §5.2) e `m688.contatto_da` |
| Obiettivo «foto»: fotografare al periscopio un'unità o una classe | I02, I08, I09 | runtime (registro M1, foto P2) | fatto: tipo `foto`, con `distanza_m` (docs/09 §5.2) |
| Obiettivo «zona inviolata»: nessuna unità di una parte nel cerchio (anche «per più di N minuti») | I06, I10 | runtime (registro M1) | fatto: tipo `zona_inviolata`, con `parte` e `minuti` |
| Obiettivo «raggiungere» relativo a un'unità in moto | I08, I05 | runtime | fatto: `raggiungere` con `unita` (o `classi`) e `distanza_m` |
| Condizione «un'unità ostile ha lanciato» | I05 | runtime (API) | fatto: `m688.ha_lanciato` |
| Condizione «il giocatore ha fatto ping entro N nm» | I10 | runtime (API) | fatto: `m688.ping_giocatore_entro` |
| Condizione «un'unità ha perso il contatto da N minuti» | I01 | runtime (API) | fatto: `m688.senza_contatto_da` |
| Cambio della situazione (ROE e armi dell'IA) da una regola o dalla radio | I03, I05, I02, S05, S06 | runtime (radio R1-R2) ed editor | radio della missione fatta (E6, `m688.radio`); l'azione dell'editor è sua |
| Radio nella missione, messaggio da regola, inganno | I02, I06, I10 | runtime | fatto (E6: chiave `radio`, `m688.radio`, `se`) |
| Meteo fissato | I02, I08, I09 | runtime | fatto (E6: `meteo = { fisso }`) |
| Mine dell'IA già posate (campo ostile) | I04 | runtime (mine M2b) | fatto: chiave `mine` (docs/09 §5.1c) |
| Danni iniziali del giocatore come chiave della missione | I07 | runtime (danni D6) | fatto: chiave `danni_iniziali` (urto, sistemi, acqua, falle, incendi) |
| Siluro inerte, da esercitazione, creato da una regola | I03, S01 | runtime ed editor | runtime fatto: chiavi `esercitazione` e `armi_esercitazione`, `m688.esercitazione`, evento `colpo_esercitazione`, obiettivo `colpire`; editor fatto: voce ARMI della missione e delle unità, azione ESERCITAZIONE nelle regole, obiettivo COLPIRE |
| Relitto: unità affondata e immobile sul fondo, rilevabile | I10 | runtime | fatto: `stato = "relitto"`; muto anche nell'acustica dell'Enhanced appena c'è il campo della sorgente (acustica); nell'editor è lo STATO «relitto» |
| Sonar attivo «speciale» con portata da scenario | I09 | runtime (acustica) | fatto: chiave d'unità `sonar_speciale` (SL, frequenza, impulso, FM, antenna, cadenza) |
| Comportamento «pedinamento» nella libreria 2 | I01 | editor | fatto (`R_pedina`, parametro distanza) |
| Spostare una zona da una regola | I10 | editor | fatto (azione «sposta zona») |
| Classe «peschereccio» o «AGI» | I02 | kit e classi | fatto: `peschereccio` e `agi`, coi modelli del classico |
| Condizioni sul giocatore: quota, velocità, alberi, superficie (dossier 23, M2) | S04, S09 | runtime (API) | fatto: `quota_entro`, `velocita_sotto`, `alberi_alzati`, `in_superficie`, `in_cerchio` |
| Diesel dell'IA con la batteria che si scarica; unità in avaria, incagliata o rimorchiata (M6) | S03, S11 | runtime | fatto: batteria e snorkel (chiave `batteria`, evento `emersione_forzata`); avaria, incaglio e rimorchio (chiave `stato`, `m688.avaria`, `m688.incaglia`, `m688.rimorchia`, eventi `avaria_ia` e `rimorchio`); nell'editor: riga STATO, azioni «avaria», «rimorchia», «molla il rimorchio», condizioni «in avaria» e «a rimorchio» |
| Parte «terza» armata, ostile a una sola parte (M7) | S08, S11 | runtime | manca |
| Acque territoriali con penalità di carriera (M5) | S11 | runtime e carriera | runtime fatto: chiave `acque_territoriali`, avvertimenti e bombe del paese costiero, evento `acque_territoriali`, `m688.in_acque_territoriali`; manca la penalità di carriera |
| Mine posate da una nave dell'IA durante la missione | S06 | runtime | fatto: `m688.posa_mina` |
| Sonar di scoperta delle mine del 688 (BQS-15) | S06 | runtime e stazioni | runtime fatto: ordine `bqs15`, `mine_scoperte()`, evento `mina_scoperta`, messaggi 450-452, `m688.bqs15`; manca il comando nelle stazioni e la resa sulla carta |
| Cavo della schiera come ostacolo che s'impiglia | S10 | runtime (collisioni) | fatto: SQR-19 di Perry e Spruance, eventi `cavo_impigliato` e `cavo_toccato`, `m688.distanza_cavo` |
| Collisioni | I03, I10, S10 | runtime | fatto (rilevamento, danni del kit, quasi collisioni, Crazy Ivan) |
| Artiglieria contro il sottomarino in superficie | I07 | runtime | fatto (G0-G2; danni G3 del kit) |

Le missioni si possono costruire anche prima che arrivi tutto. I08 e I10 si giocano già oggi, con qualche
semplificazione: la «foto» diventa «classificare», la «zona inviolata» diventa una regola di fine. Le altre aspettano
i pezzi segnati sopra. L'ordine di costruzione lo decide l'utente.

## 4. Riepilogo

| Codice | Titolo | Battello | Teatro | Situazione | Difficoltà | Durata |
|---|---|---|---|---|---|---|
| LIB-I01 | La balena di ferro | 688 | Fær Øer | pace | 4 | 2-3 h |
| LIB-I02 | Il pescatore di Teriberka | Alfa | Barents | crisi | 3 | 2 h |
| LIB-I03 | Acque basse, nervi tesi | 688 | Baltico | pace → crisi | 4 | 1,5-2 h |
| LIB-I04 | La porta di Ercole | Alfa | Gibilterra | guerra | 5 | 2-3 h |
| LIB-I05 | Il disertore | 688 | Otranto | crisi | 3 | 1,5-2 h |
| LIB-I06 | Il ponte di Ekaterina | Alfa | Kunashir | guerra | 5 | 3 h |
| LIB-I07 | Lupo ferito | 688 | Celtica | guerra | 4 | 3-4 h |
| LIB-I08 | Ombra lunga | Alfa | Fær Øer | pace | 4 | 2-3 h |
| LIB-I09 | Sotto il bastione | 688 | Barents | pace | 5 | 3 h |
| LIB-I10 | Corsa al relitto | Alfa | Celtica | pace | 2 | 2,5 h |
| LIB-I11 | Il primo eco | 688 | Fær Øer | guerra | 3 | 1,5-2 h |
| LIB-I12 | Finestra di lancio | 688 | Barents | guerra | 4 | 2,5-3 h |
| LIB-I13 | Sagome nello Stretto | 688 | Gibilterra | pace | 2 | 1,5 h |
| LIB-I14 | Levante | 688 | Gibilterra | pace | 3 | 1,5 h |
| LIB-I15 | Fanali | 688 | Gibilterra | pace | 4 | 1,5 h |
| LIB-I16 | Il bastione bianco | 688 | Kunashir (Okhotsk) | pace | 5 | 4 h |

Le storiche (§2b) sono riassunte nella loro tabella: LIB-S01 … LIB-S12. Con le missioni coi missili (§2c) c'è anche
LIB-S13 «Outlaw Shark» (688, Otranto, esercitazione, difficoltà 2, 1,5 h).

**La libreria intera** ha 41 missioni: 17 inedite e 24 storiche (con I17 e S14-S20 dei §2f, §2g e §2h, e S22-S25 del §2i).

| | 688 | Alfa | Totale |
|---|---|---|---|
| Missioni | 23 | 18 | 41 |

- **Situazione:** 23 di pace o esercitazione, 11 di crisi (o pace che diventa crisi), 7 di guerra.
- **Teatri:** Celtica 8, Kunashir 8, Fær Øer 6, Barents 6, Gibilterra 5, Otranto 5, Baltico 3.
- **Difficoltà:** 1 → 1 (S01), 2 → 5 (I10, I13, S02, S08, S13), 3 → 16, 4 → 13, 5 → 6.

**Nota di realismo:** nel 1989 gli Alfa servivano solo con la Flotta del Nord. Le schede I04 (Gibilterra) e I06
(Kunashir) sono «what-if» come le missioni originali dell'Alfa fuori dal Barents, e lo dicono nel briefing.

## Stato

- 25/10/2026: i dieci soggetti inediti (LIB-I01 … LIB-I10) sono approvati dall'utente. Si costruiscono prima I08 e I10, che
  il motore permette già; le altre aspettano le funzioni del §3. I soggetti storici arrivano col dossier 23.
- 25/10/2026: I08 e I10 sono costruite in `data/missioni/libreria/`, ognuna come file dell'editor (`<id>.lua`) più
  pacchetto (`<id>.m688m`), dallo strumento `engine/strumenti/libreria_missioni.cpp`. Rifarle dà gli stessi file.
- Prove in `engine/test/prove_libreria_missioni.cpp`:
  - i file sono aggiornati e i pacchetti si importano senza avvertimenti;
  - 30 minuti nel classico e nell'Enhanced senza errori;
  - vittoria e sconfitta di ognuna, preparate spostando il giocatore.
- Nell'editor ci sono anche il comportamento «pedinamento» (per I01) e l'azione «sposta zona» delle regole (per I10,
  quando arriverà la radio).
- 25/10/2026: le dodici schede storiche (LIB-S01 … LIB-S12, §2b) sono scritte dal dossier 23, in attesa
  dell'approvazione dell'utente.
- 26/10/2026: le dodici schede storiche sono approvate dall'utente così come sono. Si costruiscono subito S01, S02, S04,
  S07, S08, S09, S11 e S12; le altre quattro aspettano le funzioni del §3 (rapide v2 del runtime).
- 26/10/2026: costruite le otto storiche possibili (S01, S02, S04, S07, S08, S09, S11, S12) e, con le rapide v2 del
  runtime e le classi del kit, quattro inedite (I01, I02, I06, I09). La libreria ha 14 missioni, ognuna con il file
  dell'editor, il pacchetto e le prove di vittoria e sconfitta. Le unità dell'IA partono dall'id 2 (I08 e I10
  rinumerate).
- 26/10/2026: nelle missioni dell'Alfa il deposito ha 2 esche semoventi MG-74 (famiglia `esca_mobile`), come ha deciso
  l'utente. Le missioni del 688 della libreria sono tutte del 1989, quindi senza la MOSS, che è what-if del modo
  moderno. L'editor ha la riga delle esche semoventi (0, 2 o 4): sull'Alfa parte da 2.
- 26/10/2026, con E6 del runtime (radio della missione, meteo fissato, messaggi di bordo propri):
  - costruita S05;
  - la radio è in I02 (ordine falso e ordine vero con le ROE), S01 (la svolta) e I06 (la portaerei);
  - il meteo è fissato in I02, I08 e I09.
  La libreria ha 15 missioni.
- 26/10/2026, con le mine dell'IA (chiave `mine`, `m688.posa_mina`), la batteria dei diesel (M6) e il cavo della schiera
  del runtime: costruite S03, S06, S10 e I04, ognuna con le prove di vittoria e sconfitta nell'Enhanced e nel classico.
  Nella libreria anche i riferimenti alle unità dei messaggi radio (`se unita_colpita`, `contatto`) seguono la
  rinumerazione da 2. La libreria ha 19 missioni.
- 26/10/2026, con M6 avaria e rimorchio del runtime: S11 ha l'incaglio e il rimorchio veri. Nello scrittore e
  nell'editor la chiave d'unità `stato` (riga STATO: in mare, avaria, incagliata), le azioni «avaria», «rimorchia» e
  «molla il rimorchio», le condizioni «unità in avaria» e «unità a rimorchio».
- Restano bloccate:
  - I03, che aspetta il siluro inerte, il cambio di situazione e le collisioni;
  - I05: il Kilo deve attaccare una nave della propria parte, e le ROE cambiano col lancio (E6);
  - I07, che aspetta i danni iniziali del giocatore.
- 28/10/2026 (trailer-1): costruita **I07 «Lupo ferito»** (`LIB_I07_LUPO_FERITO`), con le prove di vittoria e sconfitta.
  La libreria ha 20 missioni.
  - Danni di partenza (`danni_iniziali`): urto a poppa, schiera persa, propulsione al 55%, acqua nel locale macchine.
  - Due Mk 48 e la burrasca fissata: mare 6, vento da W.
  - La barriera: due Krivak e un Kara, il Ka-27 e un Tu-142.
  - L'Oscar II di riserva si rivela a 90 minuti e un'ora dopo «lancia» sul convoglio, se non è affondato.
  - Il punto d'incontro con la fregata è a ~46 miglia: Brest resta fuori dal teatro.
  - I secondari «mai oltre i 12 nodi» e «fermare l'Oscar» stanno nelle variabili d'autore 1 e 2.
  - Il secondario «rapporto di contatto trasmesso» c'è dal 28/10/2026: la condizione delle regole «il giocatore ha
    trasmesso» (`m688.trasmesso` del runtime), dopo la rivelazione dell'Oscar, nella variabile d'autore 4.
- I03 e I05 aspettano dal runtime un'azione «attacca l'unità X», che ignori parte, ROE e non combattenti, e che per
  I03 lanci anche senza soluzione: in I05 il Kilo sulla Ekvator, in I03 il siluro inerte dell'esercitazione.
- 28/10/2026: approvate dall'utente le tre schede coi missili (§2c: LIB-I11, LIB-I12, LIB-S13), così come sono. Si
  costruisce prima S13, poi I11 (aspetta l'IA che tira i missili), infine I12 (aspetta l'ordine non autentico rifiutato
  e l'obiettivo sull'esito del TLAM).
- 28/10/2026 (trailer-1): costruita **S13 «Outlaw Shark»** (`LIB_S13_OUTLAW_SHARK`), con le prove. La libreria ha 21
  missioni.
  - La nave bersaglio è un vecchio incrociatore della classe kara, coi radar Top Sail e Head Net C che cerca l'ESM del
    TASM, rimorchiato a 6 nodi.
  - Il dato dell'E-2 arriva per radio a 10, 40 e 70 minuti, come contatto d'intelligence con 3 km d'errore.
  - Al secondo dato il rimorchio accosta e un mercantile greco le passa a 3 miglia. Tutte le armi sono inerti.
  - Semplificazioni: il teatro di Otranto è di 330 km, quindi il bersaglio sta a ~110 miglia e non a 200; rimorchiatore
    e traghetti sono della classe «mercantile», che ha il modello del livello classico.
  - Poi, coi due pezzi del runtime: missili inerti e bersaglio in EMCON «ricerca» (chiave `emcon`), verificato in prova.
- 28/10/2026 (trailer-1): costruita **I05 «Il disertore»** (`LIB_I05_IL_DISERTORE`), con le prove. La libreria ha 22
  missioni.
  - La Ekvator è della classe agi (nave idrografica, non combattente, parte URSS) e va verso Brindisi con una Krivak II
    all'inseguimento.
  - Il Kilo, appostato e nascosto sulla rotta, la silura quando gli passa vicino: è la nuova azione delle regole
    «attacca l'unità» (`m688.attacca_unita` del runtime), senza soluzione.
  - Al lancio del Kilo le ROE passano ad armi libere (situazione «guerra» e messaggio FLASH). A metà strada entra la
    Perry italiana.
  - Nelle 12 miglia: asilo concesso. Con la Ekvator affondata: fallimento.
  - Semplificazioni: la Ekvator parte a ~33 miglia da Brindisi; il secondario «trovare il Kilo prima che lanci» è la
    variabile d'autore 1.
- Lo schema dello scenario e lo scrittore hanno, dal 28/10/2026:
  - l'azione delle regole «attacca_unita» (unità, bersaglio, senza soluzione);
  - gli obiettivi `tlam` (missione, finestra, minimo) e `scatola` (taglia massima);
  - la condizione «il giocatore ha trasmesso»;
  - la chiave d'unità `emcon`.
  L'interfaccia dell'editor è della principale.
- 28/10/2026 (trailer-1): costruita **I03 «Acque basse, nervi tesi»** (`LIB_I03_ACQUE_BASSE`), con le prove. La libreria
  ha 23 missioni: mancano I11, che aspetta l'IA che tira i missili, e I12.
  - L'esercitazione a nord di Rostock è su 22-28 m d'acqua: due Koni, due Nanuchka, una Krivak II e un Kilo con le armi
    d'esercitazione.
  - Il Be-12 caccia solo in crisi; i traghetti Rostock-Gedser passano sul bordo; il meteo è fissato con la foschia.
  - A un'ora il Kilo tira un siluro inerte che aggancia il 688 (attacco forzato senza soluzione).
  - Se entro 5 minuti il giocatore lancia un siluro o va oltre i 12 nodi: allarme e crisi.
  - Restare 90 minuti nell'area è la variabile d'autore 2.
  - Poi, con la condizione delle regole «evento del registro» (`m688.evento` del runtime):
    - l'esca: un lancio qualunque del giocatore dopo un'ora, o il siluro sedotto, fa scattare allarme e crisi;
    - il secondario «non toccare mai il fondo» (variabile d'autore 6) fallisce all'incaglio, col messaggio radio;
    - se c'è stato l'incaglio e i sovietici sono all'allarme, l'hanno trovato: fine «incidente diplomatico».
    La scheda dice che toccare il fondo è già l'incidente; qui lo diventa solo se i sovietici cercano, come dice il
    messaggio radio, così il secondario ha senso.

- 28/10/2026 (trailer-1): costruita **I12 «Finestra di lancio»** (`LIB_I12_FINESTRA_DI_LANCIO`), con le prove. La
  libreria ha 24 missioni: manca I11, che aspetta l'IA che tira i missili.
  - In libreria le missioni TLAM 0521 e 0522; la navigazione parte con 2 miglia d'errore (`sigma_iniziale_nm`).
  - L'ordine falso per la 0522 parte a un'ora, con `autentico = false`; quello vero per la 0521 venti minuti dopo, con
    l'ora sul bersaglio anticipata. I due ordini arrivano alla prima finestra di trasmissione dopo l'ora fissata.
  - L'obiettivo primario è di tipo `tlam`: due missili della 0521 sul bersaglio dentro la finestra.
  - La Krivak e l'Il-38 pattugliano l'area del punto di lancio; all'allarme entra il Ka-27 di riserva.
  - Semplificazioni: il teatro del Barents comincia a 31,3° E, quindi il Varangerfjord è appena fuori carta e il punto
    di lancio è dentro; il secondario «nessun lancio su un ordine non autentico» lo garantisce il motore, che non esegue
    un ordine non autentico.
- Lo schema dello scenario e lo scrittore hanno anche (trailer-1, 28/10/2026):
  - la condizione delle regole «evento» (un evento del registro dopo tanti secondi, `m688.evento`);
  - M7, la parte del luogo: la parte d'unità «luogo» (`schieramento = 'luogo'`), la situazione di partenza col luogo
    (`situazione_luogo`) e l'azione «situazione» fra due parti.
  L'interfaccia dell'editor è della principale.
- 29/10/2026 (trailer-1): costruita **I11 «Il primo eco»** (`LIB_I11_IL_PRIMO_ECO`), con le prove. La libreria ha 25
  missioni: le schede approvate sono tutte costruite.
  - Il 688 ha quattro Harpoon UGM-84C, a sessanta miglia dal gruppo: lo Slava, una Krivak II di scorta e una di schermo
    avanzato, che scendono dal Mare di Norvegia verso la soglia.
  - Il P-3 islandese manda per radio quattro rapporti, a 5, 35, 65 e 95 minuti. Ogni rapporto è un contatto
    d'intelligence sul gruppo con 3 km d'errore.
  - A 40 minuti, subito dopo il secondo rapporto, lo Slava e la sua scorta accostano di 60 gradi a sinistra; lo schermo
    prosegue verso la soglia. Sulla rotta nuova passano un traghetto norvegese diretto in Islanda (al punto d'incrocio
    mezz'ora dopo la svolta) e sei pescherecci delle Fær Øer.
  - All'allarme entra il Ka-27 dello Slava.
  - Se lo Slava ha visto il 688 e il 688 è in superficie, lo Slava gli tira due P-500 del modello (azione nuova
    «lancia_missile», `m688.lancio_missile` del runtime). Il 688 immerso lo cercano le armi antisom.
  - Obiettivi primari: colpire lo Slava, nessun neutrale colpito, tornare a casa. Secondari: la scatola piccola o media
    (obiettivo `scatola`), non farsi scoprire.
  - Semplificazioni:
    - il ritardo della catena del P-3 non c'è: il contatto è la posizione del gruppo nel momento del messaggio, e il
      rapporto diventa vecchio per la svolta subito dopo;
    - «non farsi trovare dalla Krivak» diventa «non farsi scoprire», da qualunque unità;
    - lo Slava tira una sola salva. La condizione è mezzo minuto di contatto visibile; col mare 4 il radar lampeggia e
      ci arriva in circa sei minuti a 33 km. Il P-500 non parte sotto i 20 km: se il 688 emerge più vicino, la salva va
      sprecata e restano le armi antisom.
  - Da rivedere: main ha deciso (29/10/2026) che l'IA tirerà i missili antinave da sola, nel comandante IA e nelle ROE
    del runtime, anche fra unità dell'IA. Per ora la regola «p500» resta e garantisce il momento del tiro. Quando il
    tiro autonomo c'è, si valuta se lasciarla o toglierla, e con lei la semplificazione della salva unica.
- Lo schema dello scenario e lo scrittore hanno l'azione delle regole «lancia_missile» (trailer-1, 29/10/2026):
  l'unità tira `valore` missili antinave del modello (1 se non indicato) su `bersaglio` (−1: il giocatore). Il piano
  usa la posizione che l'unità ha del bersaglio. La verifica segnala un bersaglio sconosciuto o l'unità stessa.
- 29/10/2026 sera (trailer-1): **collaudo della libreria** col motore del giorno (profili WOA 1985-1994, sonarista con
  l'orecchio, TMA dell'IA, tiro autonomo dei missili). Le prove di vittoria e sconfitta sono verdi (33 casi), e c'è una
  corsa di 60 minuti nell'Enhanced per missione (`scene_interazioni --file`, nuova; uscite in
  `tools/taratura/collaudo_libreria/`). Corrette due missioni:
  - I07: la fregata del punto d'incontro entra a 12 miglia, perché dall'inizio affondava la Kara della barriera col
    tiro autonomo;
  - I08: la chiusura vale solo dopo che l'Alfa è uscito dalla zona di rientro, perché si vinceva restando fermi.

  Il resoconto è nella nota condivisa `collaudo-libreria-29-10.md`.
- 29/10/2026 sera (trailer-1): **installate I13 «Sagome nello Stretto» e I14 «Levante»** (§2d), con le prove. La
  libreria ha 27 missioni. Lo stesso giorno l'obiettivo «scoprire» conta anche la classificazione giusta del sonarista
  e l'identificazione visiva giusta col libretto, non solo la durata classica del contatto (`registro_missione.cpp`;
  prova «"scoprire" conta il libretto al periscopio (I03) e il sonarista sul Los Angeles che cavita (S05)»).
  - S05: il Los Angeles fugge a 12 nodi a 130 m e non cavita, e il sonarista non lo classifica nemmeno a 500 m coi ping.
    «Scoprire l'intruso» è diventato «Trovare l'intruso: tre minuti di contatto» (obiettivo `contatto`), che è quello
    che la scheda chiede. Con la caccia si raggiunge; ci si arriva anche in passivo a 2,5 km dal nascondiglio.
- 29/10/2026 sera (trailer-1): **installata I15 «Fanali»** (§2d), la notte del riconoscimento, con la prova. La libreria
  ha 28 missioni. Nel collaudo (`scene_interazioni --file`, che ora scrive anche il sensore della prima scoperta) il
  688 a quota periscopica non è più scoperto a vista nel buio. La Kara lo trova col sonar attivo (Titan-2) a 80 s, come
  in I13 di giorno: è il comportamento in pace dell'IA, non della notte.
- 29/10/2026 sera (trailer-1, diagnosi del runtime): **`R_nave` non va più a caccia antisom di una nave di superficie**
  (`scenario_missione.cpp`, `COMPORTAMENTI_2`: `bersaglio_nave` escluso come `bersaglio_aereo`). In I13 e I15 la Kara,
  con la Kidd solida al radar, la cacciava col Titan-2 attivo e illuminava il 688 a 80 s, in pace. Il ping su un
  sottomarino con traccia solida resta; il tiro fra navi è del motore e dei missili. Rigenerate le 28 missioni della
  libreria e le 16 scene di `data/missioni/prove`: cambia solo quella riga. Le rapide hanno la correzione dalla versione
  2 del generatore (`VERSIONE_RAPIDE`, F6); i codici della versione 1 rigenerano gli stessi byte di prima.
  - Nel collaudo col 688 fermo a quota periscopica, le navi sovietiche ora tengono la rotta e a volte lo investono (I13
    seme 1 la Kara a 5241 s; I15 la Nanuchka a 4455 s e il Sacramento a 2551 s, e in I15 la collisione chiude
    l'esercitazione). Il giocatore vero si muove; le navi dell'IA non scansano un periscopio che non sanno esserci.
  - La prova di I13 aspetta che i contatti sulle navi da guerra siano di nuovo abbastanza prima delle identificazioni
    giuste: senza il ping della Kara un contatto di passaggio può perdersi.
- 29/10/2026 notte (trailer-1): **installata I16 «Il bastione bianco»** (§2e), la prima sotto il ghiaccio, con due prove:
  - pack dal meteo, campana dopo il pedinamento, rapporto solo in acqua libera, rientro;
  - urto della vela con una chiglia attraversata a 20 m.

  La libreria ha 29 missioni. Prima di costruirla, quattro verifiche:
  1. **la campana:** l'ELF arriva sotto la banchisa col filo filato (`SalaRadio::ricezione`); un messaggio FLASH porta
     il campanello. Per leggerlo e rispondere serve acqua libera;
  2. **il Crazy Ivan:** è del motore (`missione_collisioni.cpp`) per ogni sottomarino sovietico dell'IA in moto;
  3. **le boe dell'ASW solo in acqua libera:** regola del runtime (`PercezioneAsw::acqua_libera`). Prova «libreria I16:
     l'Il-38 posa le boe solo in acqua libera»: col datum al margine le boe sono 4, tutte in acqua libera; col datum nel
     bastione sono 5, nelle polynye e nei canali del pack di primo anno (circa il 15% nei 10 km). L'aereo rinuncia
     («datum sul pack») solo se tutto il datum cade sul ghiaccio;
  4. **il margine del ghiaccio negli stretti:** non c'era. `Meteo::in` prendeva il massimo fra la copertura del teatro
     (75% in marzo) e quella dei punti ERA5, e il Pacifico restava coperto. Il disassemblatore l'ha corretto: con i
     punti il ghiaccio è quello del punto.

  Con la sonda è uscito anche un difetto dell'IA: con la navigazione realistica i sottomarini dell'IA salivano a 20 m
  in acqua profonda (`decidi_fondo`: sigma × pendenza della carta oltre la profondità). Il disassemblatore l'ha
  corretto: nella I16 il Delta resta a 130 m e il Victor a 90.
  - I modelli classici «per analogia» di `delta3`, `rompighiaccio`, `portarinfuse` e `bmrt` sono in
    `missioni_rapide.toml`. Le missioni della libreria si giocano anche nel classico.
