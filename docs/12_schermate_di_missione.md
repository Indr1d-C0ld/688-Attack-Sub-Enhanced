# 12 — Schermate attorno alla missione (Fase 4a)

Richiesta dell'utente (24/10/2026): briefing, carico, debriefing, esito e cerimonie ispirati ai simulatori di volo
DOS e dei primi anni '90 (*F-19*, *F-117A*, *F-15 Strike Eagle*, *Falcon*, …), adattati a un sottomarino, per le
missioni originali, rapide e dell'editor. Requisiti 15 e 16 di `docs/00_ROADMAP.md`; ricerche
`docs/ricerca/13_carriera_gradi_decorazioni.md`, `14_carico_armi.md` e `15_schermate_simulatori.md` (in corso).

## Flusso

Fascicolo del comandante → scelta della missione (originale, rapida, editor) → **briefing** (carta del teatro con i
livelli, ordini della serie 0, «Scegli le armi», «Rifiuta») → **sala siluri** (carico e tubi alla partenza) →
missione → **debriefing** (carta con le tracce, eventi uno alla volta con le minicartoline, verità sovrapponibile) →
**esito e punteggio** → **cerimonia** (promozione, decorazione, motivazione) → fascicolo aggiornato.

## Pezzi e chi li fa

| Pezzo | Contenuto | Chi |
|---|---|---|
| M1 | **Registro degli eventi di missione**, deterministico: scoperte e classificazioni, lanci, colpi, affondamenti, emersioni d'emergenza nemiche, siluri evitati, danni subiti, messaggi ricevuti e persi, infrazioni alle ROE e ingressi nelle zone protette, esposizione (fix di radiogoniometria, avvistamenti, ESM a rischio alto, ping); per ognuno ora, posizione, unita', descrizione e interesse; gli obiettivi e la completezza della missione | sessione sul runtime |
| M2 | **Carico**: deposito della missione, tipi negati, anno di servizio delle armi, carichi standard per tipo di missione, `carico.toml` dalla pipeline; API per la sala siluri (armi disponibili, posti, tubi alla partenza) applicata alla stazione Armi all'avvio; schede delle armi (dossier 14) | disassemblatore |
| M3 | **Carriera** (dossier 13): punteggio di missione per riuscita, completezza, discrezione, ordini e ROE, efficienza, danni, per il rischio; gradi USN e VMF, commissioni simulate, rimozione dal comando, decorazioni con le soglie per pace, crisi e guerra; fascicolo personale salvato in TOML (SQLite piu' avanti) | disassemblatore, dopo M2 |
| M4 | **Grafica**: ritratto del comandante in uniforme (US Navy e VMF, gradi, nastrini, dolphins o distintivo); medaglie e nastrini; cornici «CLASSIFIED»; **minicartoline** (la scena dell'evento ridisegnata dal replay deterministico con i modelli e gli effetti, o un'icona d'epoca per gli eventi che non si vedono) | kit |
| M5 | **Schermate e flusso** nell'app: briefing, sala siluri, debriefing, esito, cerimonia, fascicolo, scelta della missione; riuso della carta VGA, dei simboli e delle zone; il replay fino all'istante dell'evento per le minicartoline | principale, dopo la ricerca 15 |

Regole come sempre: classico identico (le schermate nuove valgono con `--regola simulazione`; il classico tiene il
rapporto d'esito dell'originale), file condivisi a turni, messaggi nelle fasce assegnate, prove con missioni di prova.

## Stato

- 24/10/2026: la partita (missione, danni, sottomarini, stazioni, posto, ordini, mondo, effetti) e' uscita da
  `main.cpp` in `app/partita.{hpp,cpp}` (`Partita`), per giocare una missione scelta dalle schermate e per
  rigiocarla senza finestra fino all'istante di un evento (le minicartoline); le istantanee di regressione (classico
  ed Enhanced, con ordini, urti e periscopio puntato) sono identiche bit per bit a prima.
- M1 (registro), M2 (carico) e M3 (carriera): API approvate, in implementazione. M4 (grafica): proposta in corso.
- 25/10/2026: **sala briefing** (`app/briefing.{hpp,cpp}`, proposta del dossier 15 §7.4): `--briefing` la apre prima
  della missione con `--regola simulazione` (Invio in mare, Esc rifiuta, F7 la sala siluri quando ci sara' la M2;
  `--schermata briefing|briefing-carta --istantanea FILE` la disegna senza finestra). Testata col timbro SECRET o
  SEKRETNO, titolo e DTG; carta del teatro nella grafica N1 (carta d'epoca, reticolo, note) con la «P» della partenza,
  pan con le frecce e zoom con I e O; livelli 1-8 (9 tutto o niente; in grigio quelli senza dati: basi e radar, portate
  ASW, SOSUS ed eventi aspettano i dati delle missioni; gli obiettivi O, S, R arrivano con la M1); riquadro con
  battello, teatro, finestra radio e ROE; ordini della serie 0 a telescrivente (Tab), con le sezioni fisse ROE e
  comunicazioni. La sala radio nasce prima del primo passo, la stessa che il primo passo creerebbe.
- 25/10/2026: **scelta della missione** (`app/scelta_missione.{hpp,cpp}`, dossier 15 §7.3) e **avvio dalle schermate**:
  `melchior688 --menu` apre la scelta (le 10 missioni originali lette dall'intestazione dei file, con le colonne 688 e
  Alfa come nell'originale e «-» dove la versione non c'e'; la scheda con titolo, battello, teatro e l'anteprima del
  fondale; le opzioni della partita: Tab stazioni 1989 o moderne, `[` `]` carta d'epoca o fondale vero; le schede
  Carriera, Missione rapida ed Editor in grigio finche' non ci sono), poi il briefing, poi la missione con la regola di
  simulazione. `--schermata scelta --istantanea FILE` la disegna senza finestra. Le istantanee di regressione restano
  identiche.
- 25/10/2026: **debriefing**, prima forma (`app/debriefing.{hpp,cpp}`, dossier 15 §7.7), senza minicartoline ne'
  punti (arrivano con la M4 e la M3): si apre quando la missione finisce (Esc) nel flusso `--menu` o `--briefing`;
  `--secondi N --schermata debriefing [--evento K] [--verita] --istantanea FILE` lo disegna senza finestra (K = 0:
  l'esito). Carta del teatro (grafica N1) inquadrata sulla traccia del battello e sugli eventi, con la traccia che si
  disegna fino all'istante dell'evento corrente, i segnaposto numerati degli eventi gia' visti (colore del lato) e
  quello corrente in giallo; la carta segue l'evento quando esce dal riquadro, zoom con I e O. Le tracce vengono da
  `Partita::tracce()`: la posizione vera di ogni unita' ogni 20 s, raccolta in sola osservazione. Riquadro dell'evento
  (numero, T+ e DTG, tipo, il posto della minicartolina con l'icona o la classe, descrizione, distanza, barra
  dell'interesse) ed elenco compatto (i momenti salienti; Tab tutti gli eventi; clic su una riga). **Verita'** (V):
  gli eventi che il giocatore non sapeva allora e le tracce vere di tutte le unita' (rosso ostili, azzurro amici,
  verde neutrali, grigio le altre, bianco a tratti le armi). Frecce o Invio da un evento all'altro; dopo l'ultimo
  (o E) l'**esito**: categoria, completezza, durata, obiettivi col quadratino dello stato (pieno raggiunto, vuoto
  fallito, mezzo parziale; i secondari piu' piccoli e in grigio), armi e bersagli, classificazioni, danno massimo,
  ordini violati e il messaggio del comando; Invio chiude. Regressione identica, prove 276/276.
- 25/10/2026: **Sala siluri** (`app/schermata_siluri.{hpp,cpp}`, dossier 15 §7.5 e 14 §5.3, sulla `SalaSiluri` della
  M2): F7 dal briefing la apre, Esc torna al briefing, Invio va in mare (con gli avvisi chiede prima «pronti a
  partire?»); `--schermata siluri [--arma ID] --istantanea FILE` la disegna senza finestra. A sinistra il deposito
  (le armi della marina del battello con quante ne restano; in grigio le negate, col motivo nella scheda); al centro la
  pianta del locale: tubi a prua con l'arma, pozzi verticali (Flight II, 688I), esche, rastrelliera a caselle (ogni
  arma prende i suoi posti) con la legenda, i totali (posti usati sul totale, pronti nei tubi, pozzi, peso
  informativo), gli avvisi della sala e l'ultimo rifiuto; in basso la scheda: sagoma di profilo in scala (lunghezza e
  diametro), guida, velocita', portata, testata, servizio, posti, peso, diametro, fonte con la confidenza e nota.
  Comandi: frecce su e giu' l'arma, destra la mette (rastrelliera, pozzi o esche secondo l'arma), sinistra la toglie,
  1-6 la mette nel tubo (o la toglie, o la scambia con quella che c'e'); Tab il carico standard successivo
  (l'asterisco segna il consigliato dalla missione), S lo applica, X svuota, D la dotazione dell'originale; i pulsanti
  del piede fanno lo stesso col mouse. Anche senza aprirla la partita del briefing parte con `carico_iniziale()`:
  `Partita::imposta_carico` applica il carico prima del primo passo e tiene vivo il catalogo; nelle istantanee con
  `--briefing` vale lo stesso. Non ancora: la tabella d'efficacia per bersaglio (mancano i dati), «come l'ultima
  missione» (con la carriera), il trascinamento col mouse; il carico non e' ancora nello storico degli ordini, quindi
  il replay delle minicartoline dovra' salvarlo accanto agli ordini. Regressione identica, prove 276/276.
- 25/10/2026: **esito e cerimonie** (`app/esito.{hpp,cpp}`, dossier 15 §7.8-7.9 e 13 §5.4), sulla carriera della M3
  e sulla grafica della M4. All'uscita dal debriefing (flusso `--menu` o `--briefing`) la carriera si chiude una volta
  sola (`chiudi_partita`: `riassumi` col carico della Sala siluri, `contesto_carriera` coi nomi di battello e teatro,
  `Carriera::chiudi_missione`) e il fascicolo del comandante si salva subito (`fascicolo_comandante`: quello salvato
  della marina, altrimenti uno nuovo in «carriera breve»; finche' non c'e' l'albo dei comandanti ce n'e' uno per
  marina). Le pagine, con Invio avanti ed Esc indietro (dalla prima si torna al debriefing): il **rapporto d'esito**
  (PATROL REPORT, per la VMF DONESENIE O POKHODE) su carta e scrivania, con timbro, missione, comandante, battello,
  data e situazione, la frase d'esito dell'originale, il motivo del fallimento piu' evidente (da Falcon 3.0), le voci
  R C S O E D V, la somma, il moltiplicatore scomposto, il totale nel riquadro, il giudizio, il peso delle missioni
  ripetute, l'equipaggio, le decorazioni automatiche e la commissione; poi una **cerimonia** per la decorazione
  personale e una per quella di reparto (tenda, intestazione con chi conferisce, ritratto del comandante con la
  barretta aggiornata, medaglia nell'astuccio, nastrino, motivazione su carta timbrata), la **promozione** (le due
  insegne) e il **provvedimento** (lettera timbrata). Le pagine senza contenuto non compaiono. Ogni pagina si compone
  una volta (il ritratto costa decimi di secondo) e poi si copia. `--secondi N --schermata esito [--pagina K]
  --istantanea FILE` le disegna senza finestra su un fascicolo nuovo, che non si salva. Non ancora: il ritratto di
  gruppo per le decorazioni di reparto, gli esiti narrativi (giornale, TASS), il fascicolo e l'albo (grafica chiesta
  al kit), «Replay». Regressione identica, prove 276/276.
- 25/10/2026: **minicartoline e replay.** La partita applica il carico della Sala siluri anche nello storico degli
  ordini (`RegistroOrdini::imposta_carico`, il runtime), e `--ordini FILE` lo riapplica prima del primo passo; la sala
  radio dell'Enhanced nasce sempre nel costruttore di `Partita`, cosi' partita giocata e replay partono uguali (le
  istantanee di regressione restano identiche). Il replay fino a un evento (`RegistroMissione::evento`,
  `passi_per_evento`) e' pronto per «Rivedi da qui», ma per le cartoline non serve: rigiocare un'ora di Enhanced costa
  minuti. La partita scatta invece una **fotografia** (mondo ed effetti) a ogni evento che si vede, all'istante
  dell'evento o 4 s dopo per colpi, affondamenti e lanci (la colonna d'acqua e il missile su), al massimo 120 (oltre,
  solo gli eventi d'interesse 60 e piu'); sola osservazione. Il riquadro dell'evento nel debriefing (172x78) mostra la
  scena dal punto di vista del registro (`dal_registro`, `disegna_cartolina` del kit), o l'icona animata per quello che
  non si vede; la scena si rifa' solo quando l'evento cambia. I colori della cartolina (della palette della vista, che
  la carta ridefinisce in parte) vanno negli indici 16-79, al massimo 64, i piu' frequenti, gli altri sul piu' vicino.
- 25/10/2026: **equipaggio e giornale** (grafica del kit, M4 fase 3): le decorazioni di reparto mostrano l'equipaggio
  schierato davanti alla vela (`disegna_equipaggio`: 18 uomini in foto, feriti e caduti in proporzione a 129 a bordo
  del 688 e 31 dell'Alfa); dopo il rapporto, per gli esiti che fanno notizia (neutrale o amico colpito, battello
  perduto, ma non nelle operazioni speciali), la pagina del «Norfolk Herald» o il comunicato TASS sulla Pravda con i
  testi scritti per il caso (protesta diplomatica, incidente con una nave alleata, «sottomarino in ritardo» o
  equipaggio salvato; per la VMF la smentita o l'incidente «senza pericolo per l'ambiente», sul modello dei comunicati
  dell'epoca).
- 25/10/2026: la scheda della Sala siluri mostra l'**efficacia** per bersaglio (sottomarini, navi, terra, mine) da 0
  a 3 stelle, STIMA del disassemblatore in `carico.toml` con la sua nota (dossier 15 §7.5, da F-117A). Corretto un
  crash a fine partita col battello perduto: il nome del battello si leggeva dal giocatore, che a quel punto e' gia'
  uscito dalla missione; ora la classe si prende all'avvio.
- 25/10/2026: pagina **In memoriam** quando il comandante cade (`EsitoCarriera::stato == "morto"`): tenda, ritratto
  col nastro nero del lutto, «On Eternal Patrol» o «Vechnaya pamyat'», il testo su carta con missioni e decorazioni.
  Il disassemblatore ha corretto la carriera per le missioni originali: il fuoco senza ordine che gli script puniscono
  con l'esito (2-7 e simili, `esiti_fuoco_senza_ordine` in carriera.toml) conta come infrazione, e l'esito
  «richiamato» porta il richiamo.
- 25/10/2026: **sagome di armi e mine** (kit, `grafica/sagome_armi`) agganciate in tre punti:
  - Sala siluri: la scheda ha la sagoma vera in scala comune (22 px/m), i tubi sono disegnati in sezione con l'arma
    caricata, la rastrelliera ha una piccola sagoma per posto (4 px/m), le rampe stanno in 96-143 (`palette()`).
  - Pannello Armi in missione: ogni tubo in sezione con l'arma della variante del carico. La ricarica fa scorrere
    l'arma dalla rastrelliera secondo i secondi della classe (allungati dal silenzio). In armamento il tubo è
    allagato; quando è armato, la porta è aperta e la mina ha il suo segno. Sotto, il nome dal catalogo e lo stato.
  - Quadro danni: le rastrelliere dello spaccato col carico vero.
  - Corretta anche la tabella dei nomi del pannello Armi, che era senza le famiglie «mina» e «attacco a terra».
  - Regressione: impronte identiche; cambiano solo le immagini e2 (pannello Armi) ed e3 (quadro danni).
- 25/10/2026: **mine sulla carta e cambio d'arma nel tubo.**
  - La carta dell'Enhanced disegna le mine posate dal giocatore. Il simbolo è un cerchio con quattro punte, con
    l'ombra, nel colore dello stato: in corsa bianco col tratteggio fino al punto di posa, armandosi giallo con
    l'ora d'armamento, armata rosso chiaro col cerchio d'innesco, inerte grigio, scoppiata una stella.
  - Alla stazione Armi, **Ctrl+V** (comando `variante_arma`, rimappabile) mette nel tubo scelto l'arma successiva
    della rastrelliera, esche escluse, con l'ordine `scegli_arma_tubo`: lo scambio costa una ricarica, un tubo
    armato va prima disarmato.
  - La classificazione data dal giocatore va nello storico come ordine `classifica_contatto` (del runtime). Dopo
    ANALIZZA, un tracker arrivato a PROBSUB o CERTSUB, o una nave con confidenza ≥ 0,5, classifica il contatto del
    giocatore più vicino al rilevamento, entro 5°, una volta per classe. Il registro M1 la segna giusta o sbagliata
    per il debriefing e la carriera.
- 25/10/2026: **Museo ed enciclopedia** (requisito 14):
  - `nucleo/enciclopedia` legge `data/motore/enciclopedia.toml` (190 voci del kit). `app/museo` è la schermata:
    - a sinistra l'elenco della sezione; le sezioni in ordine sottomarini, navi, aerei, elicotteri, armi, sensori,
      periscopi, sistemi, teatri e basi;
    - a destra la voce: un'immagine alla volta (sprite, modello 3D, sagoma, oculare, boa o marcatore in mare, foto
      alla 1989), la scheda tecnica con le STIME segnate e il valore del gioco del 1989, il testo, il manuale di bordo
      in giallo, i rimandi numerati, le fonti e i crediti delle foto.
  - Comandi: frecce, Tab per l'immagine, Pag per il testo, 1-9 per i rimandi, L per la lingua (italiano o inglese),
    Esc per tornare indietro.
  - I caratteri che il font non ha («», apici, ×, ≈...) diventano equivalenti; i nomi in cirillico usano la
    traslitterazione.
  - Cascata, carta e spaccato sono annunciati, i disegni arriveranno.
  - Il Museo si apre con **F1** dalla scelta della missione. In missione F1 apre la scheda pertinente, a tempo fermo:
    l'arma del tubo scelto alla stazione Armi, il periscopio alla stazione Periscopio, altrimenti il bersaglio o il
    proprio battello.
  - Senza finestra: `--schermata museo --voce ID --evento N`.
- 25/10/2026: nella scelta della missione anche **W meteo** (semplificato o del teatro e della stagione) e **K ASW
  aereo** (facile, normale, realistico, minaccia percepita). Nel debriefing il meteo a fine missione.
- 25/10/2026, per il trailer e le prove: `--istantanee T1,T2,...` (più istantanee dalla stessa partita) e
  `--tasti-a-tempo FILE` (righe «secondi comando...», coi nomi della mappa dei tasti più su, giù, sinistra e destra).
- 25/10/2026, sera: **albo dei comandanti, fascicolo personale e scelta del comandante** (dossier 15 §7.1 e 13 §5.4 C;
  `nucleo/albo`, `app/albo`).
  - **Albo:** fino a 8 carriere come cartellini sulla scrivania, ognuno con:
    - foto ufficiale, nome, sigla del grado, marina;
    - missioni, punti di carriera, decorazioni, carriera breve;
    - stato e timbro «IN CARICA».

    Chi è in carica (uno per marina, in `albo.toml` accanto ai fascicoli) va in mare con la prossima missione della
    sua parte: 688 la US Navy, Alfa la VMF. I caduti («ON ETERNAL PATROL», «POGIB PRI ISPOLNENII», col nastro
    nero), i rimossi e i congedati si consultano ma non si caricano.
  - **Comandi dell'albo:**
    - il cartellino vuoto apre una carriera nuova: nome, marina, carriera breve o completa, indulgenza;
    - F il fascicolo; Backspace due volte elimina; il mouse sceglie.
  - **Fascicolo** («OFFICER SERVICE RECORD», «LICHNOE DELO»), con 4 o 5 pagine:
    - copertina timbrata con la foto in cornice e l'anagrafica;
    - stato di servizio: incarichi e qualifiche;
    - note caratteristiche o attestazioni, col giudizio e la graduatoria fra i pari grado;
    - decorazioni, con la barretta e le motivazioni;
    - commissioni e provvedimenti, se ci sono.
  - **Nel menu:** C apre l'albo, e la scheda delle opzioni dice chi va in mare. Al primo avvio, senza carriere,
    l'albo si apre da solo, come in *F-19*.
  - **Chiusura della carriera:** usa il comandante in carica, poi il primo in servizio della marina, infine uno nuovo
    in «carriera breve» con un id libero; il comandante resta in carica se è ancora in servizio.
  - **Testi:** gradi e decorazioni sovietici passano per `testo_per_font` (traslitterazione del cirillico, «» e ª),
    la stessa funzione del Museo.
  - **Senza finestra:** `--schermata albo|fascicolo [--evento N] [--pagina K] --istantanea FILE`, sull'albo
    dell'utente (o di `XDG_DATA_HOME`).
  - **Non verificato a mano** nella finestra: modulo della carriera nuova, eliminazione, clic.
- 25/10/2026, sera: **missioni rapide nel menu** (generatore della sessione del trailer, `missioni/missione_rapida`).
  - **Scheda «Missione rapida»** della scelta della missione (R, o il clic sulla scheda). A sinistra 13 parametri:
    - teatro, stagione, momento, tipo, battello, forze, paranoia, difficoltà;
    - ASW aereo, meteo, carriera chiesta, seme;
    - il codice missione, che si digita con Invio.
    
    I campi «a caso» mostrano fra parentesi cosa ha scelto il seme.
  - **A destra l'operazione generata:** nome, codice, parte, teatro con data e ora e l'anteprima del fondale, il
    briefing senza l'intestazione, «CONTA PER LA CARRIERA» o «FUORI CARRIERA». Sotto, le opzioni della partita come
    per le originali.
  - **Invio:** il file `.lua` va in `cartella_missioni_rapide()` (accanto ai fascicoli). La partita parte da lì col
    seme, la data e l'ora della missione, dal briefing come le originali.
  - **Carriera:** una rapida conta col suo seme solo se l'ha chiesta, con soglie rispettate, e se il comandante non
    ha già giocato quel codice (registro `[codici]` in `albo.toml`, `nucleo/albo`). Altrimenti va fuori carriera.
  - **Senza finestra:**
    - `--codice XXXX-XXXX-XXXXX` gioca una rapida;
    - `--schermata scelta-rapida [--codice ...] --istantanea FILE` disegna la scheda.
  - **Limite:** il sole delle istantanee e della luce usa ancora `--lat/--lon` (61,5° N, 7° W di serie), anche nelle
    rapide del Pacifico.
  - **Non verificato a mano** nella finestra: digitazione del codice e chiusura della carriera di una rapida.
- 25/10/2026, sera: **editor di missione nel menu** (`app/editor_missione` della sessione del trailer).
  - **Scheda «Editor»** della scelta della missione: «+ Nuova missione» e le missioni salvate (`elenco_scenari`);
    Invio apre l'editor. R passa da una scheda all'altra.
  - **Dalla scheda «Missione rapida»:** E apre nell'editor la rapida in vista.
  - **«Prova in mare»** (G nell'editor) salva e gioca la missione come le rapide, dal briefing. In carriera conta
    come origine «editor», cioè solo nella carriera aperta.
  - **Senza finestra:** `--schermata scelta-editor` e `--schermata editor [--codice X] [--pagina N]`.
- 25/10/2026, sera: **carta nautica** (mobilia di `grafica/carta_vga`, livello `mobilia`).
  - Nella stazione Navigazione e nel briefing:
    - cornice coi minuti;
    - rosa con la declinazione (STIMA per teatro, 1990);
    - scala in miglia;
    - cartiglio DMAHTC o GUNIO MO SSSR con l'edizione;
    - scandagli nelle zone rilevate.
  - Con la cornice, il reticolo tiene le linee e lascia a lei i gradi.
- 25/10/2026, sera: nella scheda Editor anche **«+ Da una originale»** (sinistra e destra scelgono la missione,
  Invio la apre nell'editor con `scenario_da_originale`). Riprende la variante più probabile con unità, rotte,
  dotazione, tubi e briefing; la logica degli script originali no. Le note della conversione compaiono nella scheda.
  Con **I** si importano i pacchetti `.m688m` (vedi sopra).
