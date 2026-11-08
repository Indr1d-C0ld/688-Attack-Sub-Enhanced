# F1. Registro e debriefing

*Autori: runtime (registro degli eventi), principale (messaggi, debriefing, esito). Revisione del 29/10/2026. Stato:
rivisto (curatore: kit) per la parte della principale; la parte del registro (§2.1): rivisto (runtime, 27/10/2026),
curatore: kit.*

## 1. A che serve

Durante la missione l'equipaggio parla: i messaggi di bordo entrano in una coda, così nessuno si perde, e restano in
un registro che si rilegge da ogni stazione. Intanto il motore annota i fatti della missione, uno per uno. Alla fine il
debriefing li ripercorre come nei simulatori di volo di fine anni '80 (F-19, F-117A): la carta con la traccia, un
evento alla volta con la sua minicartolina, la verità che a bordo non si sapeva, l'esito. Poi il rapporto di
pattuglia e le cerimonie chiudono la missione nella carriera.

## 2. Il modello

### 2.1 Il registro degli eventi

*§2.1 rivista dal runtime sul codice (`registro_missione.hpp` e `.cpp`), 27/10/2026.*

Il codice è in `missioni/include/missioni/registro_missione.hpp` e `missioni/src/registro_missione.cpp`
(`RegistroMissione`, pezzo M1 di `docs/12`). Il registro è la base del debriefing evento per evento, della carriera
(capitolo F2) e delle statistiche (capitolo F4). Gira nel classico e nell'Enhanced.

**Il registro osserva soltanto.** Non usa il generatore casuale e non cambia lo stato della missione: il classico
resta identico, e lo stesso seme con gli stessi ordini e lo stesso passo dà gli stessi eventi con gli stessi id. Il
replay fino a un evento (minicartoline, «Riprendi da qui») si ferma entro `passi_per_evento(e, passo)` =
arrotondamento di `tempo / passo`, più uno.

**Chi scrive e quando.**
- `osserva(m, dt)`: alla fine di ogni sottopasso della missione (1/9 s, `PASSO_S` in `missione.cpp`), dopo tutti i
  moduli salvo le forze, che leggono subito gli eventi appena scritti, e prima che le unità uscite si tolgano. Difesa
  e reazioni dell'IA vengono prima e leggono gli eventi del passo precedente. Legge le liste pubbliche della missione
  dall'ultima posizione vista (colpi, lanci, messaggi del sonar, dell'ESM e dell'ASW, eventi d'artiglieria, urti, foto, mine, fatti dell'IA) e
  lo stato delle unità (contatti, affondamenti, armi finite). Il commento in testa all'intestazione parla di «passo di
  2 s»; il codice osserva a ogni sottopasso.
- **ganci alla fonte**, chiamati dalla missione quando il fatto accade: `radio` (messaggio ricevuto, ordini cambiati,
  perso, compito scaduto, sessione mancata), `giudizio` (regole d'ingaggio), `trasmissione` (con il fix di
  radiogoniometria), `classifica` (la classe data dal giocatore), `ingresso` (zona protetta), `lancio_senza_arma`
  (TLAM-C), `mina_posata` e `mina_esplosa`;
- `danni`, da `Missione::raccogli_danni`, che l'app chiama dopo i modelli dei danni: i rapporti del battello del
  giocatore, le emersioni d'emergenza dell'IA, la camera di lancio persa;
- `carica`, all'avvio: gli obiettivi dalla chiave `obiettivi` della missione, altrimenti da
  `data/missioni/obiettivi/<ID>.lua` (le 22 originali), e `categorie_esiti`, la categoria di ogni codice d'esito
  dell'originale (formato in `docs/09` §5.2).

Accanto al registro, per le statistiche tecniche della sandbox, la missione tiene sei liste (R1-R6,
`missione_quadro.cpp`). Si accendono con `Missione::registra_statistiche(true)` prima del primo passo; la partita
normale non le registra:
- R1, `scoperte_contatti()`: contatto nuovo, solido e perso di ogni coppia, al passo esatto;
- R2, `scoperte_possibili()`: il primo istante in cui la scoperta era fisicamente possibile, per osservatore,
  bersaglio e sensore;
- R3, `lanci()`: siluri e missili di tutti, con la stima del lanciatore (TMA, contatto o soluzione del giocatore);
- R4, `evasioni()`: l'inizio di ogni manovra d'evasione dei sottomarini dell'IA, con la minaccia;
- R5, `emissioni()`: ping, radar (dall'accensione allo spegnimento) e radio;
- R6, `punti_nave_ia()`.

Ogni evento nasce in `aggiungi`: id progressivo, istante (mai oltre il tempo della missione: un impatto o un danno
può portare il suo), le pose delle unità coinvolte, e dall'unità principale posizione, quota, classe, categoria, lato
rispetto al giocatore, categoria ROE e distanza dal giocatore. Gli eventi stanno in ordine di tempo, poi
d'inserimento. Poi `componi` calcola interesse, testi in italiano e in inglese e inquadratura. Nella sandbox
(`Missione::sandbox()` vero) i testi detti dal battello passano in terza persona col nome della sua classe
(`RegistroMissione::in_terza_persona`): per esempio «Spruance ha scoperto Los Angeles». Il registro tiene una
fotografia di ogni unità (`PosaEvento`), così il debriefing disegna anche le unità già uscite dal gioco.

**I campi** (`EventoMissione`):

| Campo | Che cosa |
|---|---|
| `id`, `tipo`, `tempo` | il fatto, stabile nel replay; il tipo (`nome_evento` dà la chiave per file e prove); i secondi di missione |
| `pos`, `quota` | dove succede: l'unità principale, il punto dello scoppio, il giocatore |
| `giocatore`, `unita`, `altra`, `arma` | gli id coinvolti: l'unità principale (il contatto, la vittima, chi lancia, chi ci avvista), l'altra (il bersaglio, il lanciatore), l'arma |
| `classe`, `categoria`, `classe_arma`, `lato` | dell'unità principale e dell'arma; il lato rispetto al giocatore (ostile, neutrale, amico) |
| `categoria_roe`, `giudizio` | per i lanci del giocatore con la sala radio |
| `distanza_m` | dal giocatore all'unità principale |
| `classe_creduta`, `livello`, `giusta`, `tempo_scoperta` | le classificazioni: la classe data, 1 categoria o 2 classe, giusta o no, la prima scoperta |
| `codice`, `valore` | secondo il tipo: il messaggio collegato, la serie radio, il codice d'esito; la gravità, la distanza, il calibro, l'energia |
| `testo_it`, `testo_en` | la descrizione breve già composta |
| `interesse` | da 0 a 100, per i momenti salienti |
| `noto` | il giocatore lo sapeva allora (la «verità» del debriefing) |
| `pose`, `inquadratura` | le unità coinvolte all'istante; il suggerimento per la minicartolina |

**I tipi.** Sono 72 (`TipoEvento`, da `inizio` a `comandante_ia`; `numero` chiude l'elenco). I tipi nuovi vanno sempre
in coda, perché gli id sono chiavi del replay e dei file. Il registro li scrive così (interesse di base, e le
aggiunte):

| Famiglia | Tipi, e da dove | Interesse |
|---|---|---|
| Missione | `inizio`; `fine` col codice d'esito (≥ 100 nell'originale) | 10; 100 |
| Contatti del giocatore | `scoperta` (primo contatto con un'unità); `classificazione` (durata del contatto oltre 800: categoria, oltre 1400: classe; o la classe data dal giocatore; o il sonarista, codice 411); `scambio_tracce` | scoperta 55 sottomarino, 30 altro, +15 la prima, +10 entro 5 nm; classificazione 45 o 25, +5 la classe, +20 se errata; 30 |
| Armi | `lancio_giocatore`, `lancio_nemico`, `colpo`, `affondamento`, `siluro_evitato` (arma ostile diretta su di noi finita senza colpirci), `esca_sedotta`, `fuoco_amico`, `colpo_esercitazione`, `fine_missile` (un missile che finisce senza impatto: `codice` 0 esaurito, 1 guasto in volo; `valore` = secondi di volo; mai noto) | lancio 60 il primo, 50; nemico 70 su di noi, 40; colpo 85 su di noi, 75 nostro, 45, 20 se mancato; affondamento 90 sottomarino o nave maggiore, 70, +10 nostro; evitato 80, +10 se ci aveva agganciati; esca 70; fuoco amico 90; esercitazione 70 o 40; fine del missile 40 |
| Danni | `danno_subito` (rapporti del battello, con la gravità), `danno_domato`, `scram`, `camera_di_lancio_persa`, `incaglio`, `emersione_emergenza` (dell'IA) | danno 20 sotto gravità 0,35, poi 40 + 50 · gravità; 45; 70; 75; 50; 85 |
| Radio e ordini | `messaggio_ricevuto`, `messaggio_perso`, `compito_scaduto`, `sessione_mancata`, `ordine_ricevuto`, `trasmissione`, `fix_radiogoniometria`, `infrazione_roe`, `ingresso_zona` | ricevuto 50 con ordini cambiati, 35 FLASH, 15; perso 45 se portava ordini, ROE o zone, 10; 40; 30; 55; 20; fix 55, +10 localizzati; infrazione 75, +15 consapevole; 50 |
| Esposizione | `avvistato` (un'unità ostile ha la nostra posizione: la prima volta, o dopo 10 minuti), `esm_rischio_alto` (336, 337), `ping_attivo`, `ping_subito` (312, o la boa attiva), `transitorio_proprio` (313), `boa_vicina` (entro 2 nm), `foto_periscopio`, `esposizione_lunga` (testa fuori oltre 30 s) | 60, +15 il primo; 45, +10 da un aereo; 20; 35, +10 il primo; 25; 50; foto 40, +20 un sottomarino, +10 un'unità; 35, +15 oltre 60 s |
| ASW aereo | `aereo_in_sorvolo`, `ping_sonar_immersione`, `marcatore_avvistato`, `attacco_simulato`, dagli avvisi 382-385 | 45, +10 il primo; 50, +10 il primo; 60; 70 |
| Artiglieria | `artiglieria_salva` (la prima salva di ogni nave su di noi; codice 1 d'avvertimento), `artiglieria_colpo` (diretto o schegge), `artiglieria_vicino` (il primo colpo entro 50 m), `acque_territoriali` (entrata e uscita) | 65, 50 d'avvertimento, +10 la prima; 80, +10 da 100 mm; 70; 70 entrata, 40 uscita |
| Collisioni | `collisione`, `quasi_collisione`, `cavo_impigliato`, `cavo_toccato` | 85, +10 oltre 10 MJ; 55, +20 sotto 100 m, +10 se non lo sapevamo; 90; 60 |
| Mine | `mina_posata`, `mina_esplosa`, `mina_scoperta` (BQS-15) | 50 la prima, 40, 35 inerte; 75, +10 sotto un sottomarino; 65 la prima, 45 |
| Unità dell'IA | `emersione_forzata`, `avaria_ia`, `rimorchio` | 70; 60 se nota, 40; 55, 40 alla fine, +10 noto |
| Lo spettatore (fase 5 di `docs/18`) | `lancio_ia`, `scoperta_ia`, `ingaggio_ia`, `evasione_ia`, `artiglieria_ia`, `collisione_ia`, `classificazione_ia` | 45, +15 noto; 20; 35; 30; 40, +15 noto; 50, +15 noto; 20 giusta, 30 errata |
| Le forze (fase 3) | `allerta_forza`, `messaggio_ia`, `provocazione`, `fuoco_amico_ia` | forza intera 25 + 10 · livello, formazione 15 + 5 · livello; 15; 75; 80 |
| Difesa e armi dell'IA (fase 4) | `esca_ia`, `ritirata_ia`, `tma_ia`, `reazione_ia`, `incsea` | 35, +15 noto; 40 se colpito, 25, +10 noto; 35 la soluzione, 15; 20 + 10 · codice, +10 noto; 45, +10 noto |
| Navigazione incerta | `punto_nave` (del giocatore) | 45 se la stima si sposta oltre 1 nm, 25; 15 non riuscito |
| Sandbox (`docs/19`) | `comandante_ia` | 40 in attacco o difesa, 15 |
| Missili e attacco a terra | `esito_tlam` (un TLAM all'ora d'arrivo: la verità, che il giocatore sa solo dal BDA alla radio) | 70 arrivato, 55 no |
| Periscopio e fondo (docs/26) | `misura_telemetro` (una lettura del telemetro, con la distanza vera); `osservazione_aob` (un AOB, con l'errore); `ping_ecoscandaglio` | 25; 15; 5 |
| Intercettatore, mine e radio | `allarme_siluro` (`codice` 1 siluro in acqua, 2 in acquisizione); `attraversamento_mine` (un campo ostile col sonar da mine acceso, senza danni); `verifica_radio` (`giusta` = autentico) | 70 + 10 · codice; 60; 10 autentico, 55 non autentico |
| Luci e fuori scafo | `luci` (le luci di un'unità cambiano: `valore` = lo stato); `punto_nave_ia` (di un sottomarino dell'IA, mai noto); `sonar_traino` (una nave fila o recupera il VDS o la schiera, mai noto) | 20; 10; 10 |
| Il battello del giocatore | `posti_di_combattimento` (livello 1 ai posti, 0 alla condizione normale; `classe_arma` = la causa) | 65 ai posti, 20 al ritorno |
| Vetrino di Dio (docs/28) | `intervento` (`codice` = id dell'intervento, `classe_arma` = il tipo, livello 1 applicato, 2 finito, 3 fallito; noto falso: lo vede lo spettatore, non le parti) | 70 applicato, 40 |

Le «navi maggiori» dell'affondamento sono Kirov, Kiev, Slava, Kara, Nimitz e Ticonderoga. Il significato di `codice`,
`valore` e `livello` per ogni tipo è scritto accanto al tipo nell'intestazione.

**I tipi dell'IA e i loro campi** (fasi 3-5 di `docs/18`, navigazione incerta, sandbox; commenti di `TipoEvento`).
Vengono dalle liste pubbliche della missione (capitolo E6 §2.16); `pos` è quella dell'unità principale, salvo dove
detto.

**Tutti i tipi,** generati dal codice (l'elenco completo; la tabella sopra li raggruppa per famiglia col loro interesse):

<!-- generato: eventi_registro -->
*85 tipi, dall'enum `TipoEvento` di `missioni/registro_missione.hpp`, con il commento del codice.*

| Tipo | Che cosa |
|---|---|
| `inizio` | — |
| `fine` | fine: esito della missione (codice >= 100 dell'originale) |
| `scoperta` | il giocatore ha un contatto nuovo (la prima volta per unità) |
| `classificazione` | durata del contatto oltre 800 (categoria) e oltre 1400 (classe), o il giocatore |
| `lancio_giocatore` | siluro, missile, esca, boa; missile del modello: livello = taglia della scatola + 1 (1 piccola, 2 media, 3 grande), codice = modo (0 RBL, 1 BOL) |
| `lancio_nemico` | arma di un'unità ostile al giocatore (verso chiunque) |
| `colpo` | a segno (Missione::colpi), anche sul giocatore |
| `affondamento` | unità distrutta (regola classica, fisica dei danni, urto col fondo) |
| `emersione_emergenza` | E1 di un sottomarino dell'IA |
| `siluro_evitato` | arma ostile diretta sul giocatore finita senza colpirlo |
| `esca_sedotta` | un siluro del modello ha preso un'esca |
| `danno_subito` | rapporti di DanniSottomarini sul giocatore |
| `danno_domato` | incendio spento o falla tamponata |
| `scram` | reattore del giocatore fermo |
| `camera_di_lancio_persa` | il compartimento dei tubi allagato o esploso: le armi a bordo sono perse |
| `messaggio_ricevuto` | — |
| `messaggio_perso` | — |
| `compito_scaduto` | — |
| `sessione_mancata` | — |
| `ordine_ricevuto` | messaggio radio che porta un obiettivo (`da_radio` nel file degli obiettivi) |
| `trasmissione` | il giocatore (o una sua boa) ha trasmesso |
| `infrazione_roe` | lancio contro le regole d'ingaggio o in una zona protetta |
| `ingresso_zona` | zona protetta conosciuta |
| `fix_radiogoniometria` | il nemico ci ha localizzati (o rilevati) dopo una trasmissione |
| `avvistato` | un'unità ostile ha la posizione del giocatore (prima volta o dopo 10 minuti) |
| `esm_rischio_alto` | 336 e 337 |
| `ping_attivo` | ping del giocatore |
| `ping_subito` | ping nemico che arriva al giocatore (312) |
| `transitorio_proprio` | cavitazione del giocatore (313) |
| `boa_vicina` | boa ostile entro 2 nm dal giocatore |
| `incaglio` | urto col fondo del giocatore |
| `fuoco_amico` | il giocatore ha colpito un amico o un neutrale |
| `mina_posata` | M2b: una mina arrivata sul fondo (livello 1: inerte, fondale inadatto) |
| `mina_esplosa` | M2b: una mina è scoppiata sotto un bersaglio o ha lanciato il suo siluro |
| `aereo_in_sorvolo` | un aereo è passato nella finestra di Snell sopra il giocatore |
| `ping_sonar_immersione` | il giocatore sente il ping di un sonar a immersione |
| `marcatore_avvistato` | un fumogeno o un marcatore aereo in vista del periscopio |
| `attacco_simulato` | SUS in pace: attacco simulato contro il giocatore |
| `foto_periscopio` | foto del Type 18: valore = rilevamento vero, livello = indice dell'ingrandimento |
| `esposizione_lunga` | la testa fuori oltre 30 s in una guardata: valore = secondi |
| `scambio_tracce` | due tracce si sono scambiate in un incrocio (verità): unità = ora, altra = prima |
| `artiglieria_salva` | una nave apre il fuoco sul giocatore (codice 1: d'avvertimento) |
| `artiglieria_colpo` | colpo diretto o schegge sul giocatore, uno per proietto: classe_arma = pezzo, valore = calibro mm, codice = sparo |
| `artiglieria_vicino` | il primo colpo caduto entro 50 m dal giocatore: valore = distanza |
| `collisione` | urto del giocatore: valore = energia normale (MJ), livello 1 se radente |
| `quasi_collisione` | passaggio ravvicinato: valore = distanza fra gli scafi, codice 1 se lo sapevamo |
| `emersione_forzata` | M6: un diesel dell'IA emerge con la batteria scarica (unità) |
| `cavo_impigliato` | S10: il cavo della schiera di una nave nell'elica del giocatore (unità = la nave) |
| `cavo_toccato` | S10: lo scafo del giocatore tocca il cavo (unità = la nave) |
| `mina_scoperta` | S06: il BQS-15 vede una mina (pos stimata, valore = distanza, livello 1 ormeggiata) |
| `avaria_ia` | M6: un'unità dell'IA perde la propulsione (unità) |
| `rimorchio` | M6: unità = il rimorchiato, altra = il rimorchiatore; livello 1 alla fine |
| `acque_territoriali` | il giocatore nelle acque di un altro paese (codice = indice), livello 1 all'uscita |
| `colpo_esercitazione` | un'arma da esercitazione a segno: unità = la vittima, altra = il lanciatore |
| `lancio_ia` | un lancio di un'unità dell'IA: unità = lanciatore, altra = bersaglio, arma |
| `scoperta_ia` | il primo contatto visibile di un'IA su un'unità ostile: unità = chi scopre |
| `ingaggio_ia` | un'IA aggancia un'unità ostile (ENGAGE): unità = chi attacca, altra = bersaglio |
| `evasione_ia` | una manovra d'evasione nuova di un sottomarino dell'IA (valore = quante finora) |
| `artiglieria_ia` | una nave apre il fuoco su un'unità che non è il giocatore: altra = bersaglio |
| `collisione_ia` | urto fra due unità dell'IA: valore = energia normale (MJ) |
| `classificazione_ia` | il sonarista di un'IA classifica: unità = chi ascolta, altra = la sorgente vera, classe_creduta, giusta; livello = confidenza (1 POSSUB, 2 PROBSUB, 3 CERTSUB) |
| `allerta_forza` | l'allerta cambia: codice = lato (8 usa, 16 urss, 0 neutrali), valore = di prima, livello = nuovo, classe_arma = causa; unità = chi l'ha vissuto (la sua formazione), -1 la forza intera |
| `messaggio_ia` | un rapporto di contatto fra unità dell'IA (fase 3 B): unità = chi riferisce, altra = su chi; codice = canale (0 formazione, 1 aereo-nave, 2 teatro, 3 broadcast ai sottomarini, 4 al giocatore), valore = ritardo s, giusta = arrivato |
| `provocazione` | in crisi, un atto ostile porta in guerra due parti: unità = chi l'ha fatto, altra = su chi; codice = lato provocato, livello = lato che provoca; classe_arma = lancio, colpo, salva |
| `fuoco_amico_ia` | un'IA attacca un'unità della sua parte (fase 3 C): unità = chi, altra = su chi, classe_arma = lancio, colpo, salva |
| `esca_ia` | un sottomarino dell'IA lancia un'esca: unità = il battello, arma = l'esca, classe_arma = adc_mk2 o mg74; noto se l'equipaggio ha un contatto sul battello |
| `ritirata_ia` | un'IA si allontana: unità = chi, altra = da chi (il lanciatore, il siluro, l'altra unità, chi ha pingato); classe_arma = colpito, siluro, collisione, ping |
| `tma_ia` | le gambe di TMA di un sottomarino dell'IA: unità = chi, altra = il bersaglio, livello = gambe fatte; codice 0 gamba nuova, 1 soluzione, 2 rinuncia |
| `reazione_ia` | una reazione per situazione (fase 4 C): unità = chi reagisce, altra = la causa (l'attaccato, l'affondato, il contatto); classe_arma = reazione (datum, tiene_sotto, allontana, ascolta, copre, cue, pedina); classe_creduta = ruolo (asw, nave, sottomarino, aereo); codice 0 scoperto, 1 attaccato, 2 affondato, 3 il pedinamento del giocatore fatto dallo script |
| `incsea` | pace (fase 4 D): due navi di parti diverse sotto i 1000 yd: unità, altra; valore = m |
| `punto_nave` | navigazione incerta: classe_arma = tipo del punto nave; giusta = fatto; valore = spostamento della stima (m); distanza_m = sigma dopo (m); codice = MotivoPuntoNave se non fatto |
| `comandante_ia` | sandbox: il comandante IA del battello cambia fase: classe_arma = fase (transito, pattuglia, tma, attacco, difesa, punto_nave, ritirata), classe_creduta = motivo, altra = il contatto su cui lavora |
| `esito_tlam` | un TLAM all'ora d'arrivo, la verità (docs/21 §11.8): classe_arma = missione, classe_creduta = bersaglio, giusta = arrivato, valore = s di volo, pos = landfall o caduta; codice 1 se l'arrivo è nella finestra (tot +- finestra_s); il giocatore lo sa solo dal BDA alla radio, più tardi |
| `fine_missile` | un missile antinave che finisce senza impatto (docs/21 §11.9): unità = il lanciatore, arma = il missile, classe_arma = la classe; codice 0 esaurito (combustibile o ricerca finita), 1 guasto in volo; valore = s di volo, pos = dove; noto falso (senza collegamento col missile nel 1989: semplicemente non arriva). Codice 2: fermato dalla difesa della nave (T13 del kit): altra = la nave, pos e quota (m sul mare) dello scoppio in aria, valore = kg d'esplosivo (0 inerte) |
| `misura_telemetro` | una lettura del telemetro del giocatore (docs/25 §3.3): unità = il contatto, valore = la distanza letta, distanza_m = la vera, misura_m = l'altezza d'albero usata |
| `osservazione_aob` | un AOB al periscopio: unità = il contatto, valore = errore in gradi dalla rotta vera |
| `ping_ecoscandaglio` | un ping dell'ecoscandaglio (quadro del fondale) |
| `allarme_siluro` | l'allarme dell'intercettatore che sale: codice 1 siluro in acqua, 2 in acquisizione |
| `attraversamento_mine` | un campo di mine ostili attraversato col BQS-15 acceso, senza danni |
| `verifica_radio` | il gruppo d'autenticazione verificato: giusta = autentico (falso: riconosciuto non autentico) |
| `luci` | le luci di navigazione d'un'unità cambiano (docs/27): valore = lo stato (luci::Stato) |
| `punto_nave_ia` | un punto nave d'un sottomarino dell'IA (navigazione incerta): unità = chi, valore = lo spostamento della stima (m), distanza_m = la sigma dopo, classe_arma = il tipo; noto falso |
| `sonar_traino` | una nave dell'IA fila o recupera il VDS o la schiera (docs/ricerca/33): unità = la nave, livello 1 filato (in quota) o 0 recuperato, valore = la quota (m; < 0 non ancora), classe_arma = il sistema; noto falso (il giocatore non lo vede) |
| `intervento` | il vetrino di Dio (docs/28): unità = quella toccata, codice = id dell'intervento, classe_arma = il tipo, livello 1 applicato, 2 finito, 3 fallito; noto falso (lo spettatore lo vede, le parti no) |
| `posti_di_combattimento` | il battello del giocatore va ai posti di combattimento (livello 1) o torna alla condizione normale (0); classe_arma = la causa (siluro, lancio, contatto, normale) |
<!-- fine: eventi_registro -->

| Tipo | `unita` | `altra` | Gli altri campi |
|---|---|---|---|
| `lancio_ia` | il lanciatore | il bersaglio | `arma`; `classe_arma` = classe dell'arma |
| `scoperta_ia` | chi scopre | lo scoperto | il primo contatto usabile (traccia solida o posizione) su un'unità ostile, guardato una volta al secondo |
| `ingaggio_ia` | chi attacca | il bersaglio | l'aggancio (ENGAGE) |
| `evasione_ia` | il sottomarino | — | `valore` = manovre d'evasione finora |
| `artiglieria_ia` | la nave che spara | il bersaglio | la prima salva su un'unità che non è il giocatore, per coppia; `valore` = calibro (mm) |
| `collisione_ia` | una delle due | l'altra | `valore` = energia normale (MJ); `pos` = il punto dell'urto |
| `classificazione_ia` | chi ascolta | la sorgente vera | `classe_creduta`; `giusta`; `livello` = confidenza: 1 POSSUB, 2 PROBSUB, 3 CERTSUB |
| `allerta_forza` | chi l'ha vissuto (la sua formazione); −1 la forza intera | — | `codice` = lato (8 usa, 16 urss, 0 neutrali); `valore` = livello di prima; `livello` = nuovo; `classe_arma` = causa (l'evento, «situazione», «discesa») |
| `messaggio_ia` | chi riferisce | su chi | `codice` = canale: 0 formazione, 1 aereo-nave, 2 teatro, 3 broadcast ai sottomarini, 4 al giocatore; `valore` = ritardo (s); `giusta` = arrivato; il tempo è la partenza |
| `provocazione` | chi l'ha fatto | su chi | `codice` = lato provocato; `livello` = lato che provoca; `classe_arma` = lancio, colpo, salva |
| `fuoco_amico_ia` | chi | su chi | `classe_arma` = lancio, colpo, salva |
| `esca_ia` | il sottomarino | — | `arma` = l'esca; `classe_arma` = adc_mk2 o mg74 |
| `ritirata_ia` | chi si allontana | da chi: il lanciatore, il siluro, l'altra unità, chi ha pingato | `classe_arma` = colpito, siluro, collisione, ping |
| `tma_ia` | il sottomarino | il bersaglio | `livello` = gambe fatte; `codice` 0 gamba nuova, 1 soluzione, 2 rinuncia |
| `reazione_ia` | chi reagisce | la causa: l'attaccato, l'affondato, il contatto | `classe_arma` = reazione (datum, tiene_sotto, allontana, ascolta, copre, cue, pedina); `classe_creduta` = ruolo (asw, nave, sottomarino, aereo); `codice` 0 scoperto, 1 attaccato, 2 affondato, 3 pedinamento del giocatore fatto dallo script |
| `incsea` | una nave, o l'aereo | l'altra nave | due navi di parti diverse, non in guerra, sotto i 1000 yd; o un aereo dell'ASW aereo sotto i 500 piedi entro 1 nm da una nave dell'altra parte («sorvola … contro l'accordo INCSEA»); `valore` = distanza (m) |
| `punto_nave` | il giocatore | — | `classe_arma` = tipo del punto nave; `giusta` = fatto; `valore` = spostamento della stima (m); `distanza_m` = sigma dopo (m); `codice` = `MotivoPuntoNave` se non fatto |
| `comandante_ia` | il giocatore (il battello del comandante) | il contatto su cui lavora | `classe_arma` = fase (transito, pattuglia, tma, attacco, difesa, punto_nave, ritirata); `classe_creduta` = motivo |

Una salita dell'allerta della formazione si scrive; la sua discesa no. Il punto nave dei sottomarini dell'IA non si
scrive.

**L'interesse** è una funzione pura (`interesse(e, primo)`), così carriera e statistiche possono ricalcolarlo.
`primo` vuol dire il primo evento del suo tipo nella missione. Il risultato sta fra 0 e 100. I valori sono una STIMA
da tarare (note di progetto del registro M1, §3).

**Noto.** Un evento è noto se il giocatore lo sapeva allora. Le regole:
- noti per costruzione: quelli del giocatore stesso (lanci, foto, ping, danni, trasmissioni, radio, mine posate da
  lui, acque territoriali, mine viste dal BQS-15, punti nave, fasi del comandante IA);
- noti se il giocatore ha un contatto visibile su chi agisce: lanci nemici e dell'IA, salve fra unità dell'IA,
  avarie, rimorchi, emersioni forzate, esche, ritirate e reazioni dell'IA, collisioni fra unità dell'IA (su una delle
  due). Un lancio diventa noto anche dopo, quando il giocatore sente l'arma;
- `colpo` è noto se il giocatore è la vittima o il lanciatore; `affondamento` anche se aveva un contatto sulla nave.
  Anche i colpi d'artiglieria fra unità dell'IA sono `colpo` (dal 27/10), così contano nel Pk delle statistiche: i
  mancati col valore 0, che il testo dice «Colpo mancato» (interesse 20);
- mai noti: `fine_missile` (per chi lancia il missile semplicemente non arriva), `avvistato`, `fix_radiogoniometria`, `scambio_tracce`, `scoperta_ia`, `ingaggio_ia`, `evasione_ia`,
  `classificazione_ia`, `allerta_forza`, `tma_ia`. Sono decisioni del nemico o la verità del motore;
- `messaggio_ia` è noto solo sul canale diretto al giocatore, `quasi_collisione` solo se lo sapevamo, `incsea` e
  `provocazione` se il giocatore è coinvolto, `fuoco_amico_ia` se la vittima è il giocatore.

**L'inquadratura** (`Inquadratura`) suggerisce la minicartolina: il soggetto (l'unità principale, o il giocatore),
una seconda unità da tenere in campo, la distanza della camera, l'elevazione (negativa sott'acqua) e l'azimut
rispetto alla prua del soggetto. Il punto di partenza è dal traverso di dritta, 5° sopra l'orizzonte, a tre volte la
lunghezza tipica della categoria (nave 130 m, sottomarino 100 m, aereo 35 m, altro 6 m); ogni tipo la corregge (il
lancio da poppavia del lanciatore, la foto dall'occhio del periscopio col rilevamento dello scatto). Quello che non si
vede ha un'icona: `radio`, `esm`, `radiogoniometro`, `danni`, `zona`, `sonar`, `mina`, `marcatore`, `sus`,
`periscopio`.

**I momenti salienti** (`momenti_salienti(n = 12, spazio_s = 60)`):
1. si escludono gli eventi `inizio`;
2. passano davanti gli obbligati: fine, colpi, affondamenti, danni con gravità almeno 0,6, scoperte di sottomarini,
   foto al periscopio; poi l'interesse, poi l'id;
3. si prendono al più `n` eventi, scartando un non obbligato entro 60 s da uno già scelto;
4. si rendono in ordine di tempo.

**Gli obiettivi** (`Obiettivo`). Tipi: `affondare`, `proteggere`, `raggiungere`, `scoprire`, `non_essere_scoperto`,
`sopravvivere`, `variabile`, `esito`, `roe`, `minare`, `contatto`, `foto`, `zona_inviolata`, `colpire`. Una
definizione può dare le unità (per id o per classe, con un `minimo`), un cerchio, un termine `entro_s`, la variabile e
il valore dello script, i codici d'esito, i minuti di contatto o di tolleranza, la distanza, la parte, le varianti
della missione, `primario` e `peso`, e `da_radio` (l'obiettivo vale solo dopo che il suo ordine è arrivato via radio).
Lo stato è in corso, raggiunto, parziale o fallito. Alcune regole:
- `scoprire` conta le unità classificate (durata del contatto oltre 800);
- `non_essere_scoperto` fallisce al primo `avvistato` o `fix_radiogoniometria`;
- `proteggere` fallisce con l'unità affondata, o colpita dal giocatore se `colpire = false`;
- `zona_inviolata` fallisce quando un'unità della parte resta nel cerchio oltre i minuti di tolleranza;
- `colpire` conta anche i colpi delle armi da esercitazione;
- `minare` conta le mine del giocatore arrivate sul fondo e non inerti, nel cerchio se c'è.
Senza obiettivi dichiarati vale la forma generica, «Riportare a casa il battello» (`sopravvivere`). Con la sala radio
si aggiunge l'obiettivo secondario «Rispettare le regole d'ingaggio e le zone protette».

**L'esito dettagliato** (`esito(m)`, `EsitoDettagliato`):
- **completezza**: gli obiettivi attivi (tutti tranne sopravvivere, proteggere, non essere scoperti e ROE) col loro
  peso, doppio per i primari; ognuno vale 1 se raggiunto, fatti/richiesti se parziale, 0 altrimenti. I vincoli
  (proteggere, non essere scoperti, ROE) non aggiungono nulla se rispettati, ma tolgono fino a metà se violati:

  ```
  completezza = Σ(w · q) / Σw · (1 − 0,5 · (1 − vincoli))      vincoli = 1 − violati / peso dei vincoli
  ```

  Senza obiettivi attivi la completezza è la quota dei vincoli rispettati, a missione finita;
- **riuscita**: la media pesata dei soli primari attivi;
- **categoria**: quella di `categorie_esiti` per il codice d'esito dell'originale; altrimenti battello perso, in
  corso, successo (tutti i primari raggiunti), successo parziale (almeno un primario con qualcosa di fatto) o
  fallimento. Un neutrale o un amico affondato dal giocatore la cambia in fuoco amico, salvo col battello perso;
- **i conti**: lanci e colpi per arma, bersagli colpiti e affondati per lato, siluri evitati, colpi subiti (le armi a
  segno e gli spari a segno, diretti o di schegge: una raffica vale uno, capitolo E4; i colpi d'avvertimento non contano), classificazioni giuste ed errate con l'anticipo medio (quelle del sonarista a parte), esposizione (ping subiti, fix,
  avvistamenti, rischi ESM, transitori, boe vicine, sorvoli, foto, esposizioni lunghe, secondi col periscopio e con un
  albero su o in superficie), radio e ordini, mine, infrazioni e zone, danno massimo, incaglio, danno nucleare,
  battello perso ed equipaggio salvo, la distanza minima da un'arma ostile, i secondi con un'unità antisommergibile in
  contatto, il margine di collasso (1 − quota / quota di collasso; senza il modello dei danni, collasso = 1,5 volte la
  quota massima, STIMA). Feriti e morti valgono 0 e sono segnati come stima finché l'equipaggio non ha un modello.

### 2.2 I messaggi di bordo

`app/partita.cpp`, `Partita::storico_messaggi`. Il registro dei messaggi raccoglie, in ordine di tempo, i messaggi
destinati al battello del giocatore:
- della missione e degli script, del sonar, della radio, dell'ESM, delle stazioni, dell'equipaggio;
- dei danni delle navi e del battello;
- gli avvisi composti, col rilevamento al posto di `{RIL}`: ASW aereo (380-389), sonarista (410-413), artiglieria
  (420-429), collisioni (431-437), unità dell'IA (460-463, §2.4).

I testi sono in `data/motore/testi/messaggi_bordo.tsv`; da 1000 in su sono quelli della missione.

### 2.3 La coda e il registro (Enhanced)

`app/coda_messaggi.cpp`, `CodaMessaggi`. La barra in basso non perde messaggi:

```
durata = clamp(2 + 0,05 · n, 2,5, 7)        [s reali]     n = lunghezza del testo
critico = il testo contiene «!»
```

- Ogni messaggio nuovo entra in coda; un messaggio uguale al precedente si scarta.
- I **critici** passano davanti, dopo gli altri critici già in coda. Interrompono un messaggio normale che è in barra
  da almeno 1 s.
- La coda tiene al più **6** messaggi: oltre, saltano i normali più vecchi (restano nel registro).
- Finita la coda, l'ultimo resta in barra, come nell'originale. Il testo va su due righe.
- I tempi sono reali, non di missione: con la compressione alta molti messaggi passano solo nel registro.

**La compressione si ferma da sola** (torna a 1×) a ogni evento del registro di tipo scoperta, lancio nemico, colpo,
avvistamento, danno subito, ping subito, aereo in sorvolo, salva d'artiglieria, e a ogni messaggio critico che entra
in barra (`app/main.cpp`).

**Il registro dei messaggi** (Ctrl+L, da ogni stazione; `app/interfaccia.cpp`, `disegna_registro`): una finestra
sopra la vista con l'ora di missione (`HH:MM:SS`) e il testo; i critici in giallo, gli altri in verde. Mostra le
ultime 29 righe. Ctrl+L o Esc la chiude.

### 2.4 Gli avvisi dello spettatore

`app/partita.cpp`, `Partita::interazioni_ia` (fase 5 di `docs/18_interazioni_ia.md`). Dei fatti fra unità dell'IA
l'equipaggio riporta solo quelli che può sapere: l'evento del registro deve essere **noto**, cioè il giocatore ha un
contatto visibile su chi agisce.

| Evento del registro | Condizione | Messaggio |
|---|---|---|
| `lancio_ia` | l'arma non è diretta a noi (quelle su di noi hanno i loro avvisi) | 460 «Sonar: un'arma in acqua a rilevamento {RIL}, non è diretta a noi.» |
| `artiglieria_ia` | battello in immersione, alberi giù | 461 «Sonar: colpi d'artiglieria, rilevamento {RIL}.» |
| `artiglieria_ia` | periscopio o albero su, o in superficie (quota ≤ 2 m) | 462 «Vedetta: vampe di cannone, rilevamento {RIL}.» |
| `collisione_ia` | — | 463 «Sonar: rumore d'urto fra due scafi, rilevamento {RIL}.» |

- Lo stesso codice al più ogni 30 s di missione.
- Il rilevamento è dal battello al punto dell'evento.
- Il resto (scoperte, ingaggi, evasioni dell'IA) si vede solo nel debriefing e nel replay, con la verità.

### 2.5 Il debriefing

`app/debriefing.cpp`, `SchermataDebriefing`. Si apre alla fine della missione, nell'Enhanced avviato dal menu o col
briefing.

**La pagina** (640 × 480) è nella grafica dei documenti del gruppo 3 (capitolo E9 §2.9.4): la consolle con la
testata classificata, il foglio a destra, il piede comune.
- **testata**: il timbro «SECRET» («SEKRETNO» per l'Alfa) a sinistra; al centro la targa «DEBRIEFING - »
  («RAPPORTO DI CAMPAGNA - ») col titolo della missione; a destra l'ora dell'evento: il GDO se c'è la sala radio,
  altrimenti `T+HH:MM:SS`;
- **carta** a sinistra, incassata nel vetro del pannello, nello stile della carta scelto: la traccia del battello (un
  punto ogni 20 s di missione) fino all'evento corrente, gli eventi già visti come segnaposto numerati, quello
  corrente in giallo, con la cornice, fuori dai gruppi e sopra tutti. Le etichette che si toccherebbero alla scala di
  adesso diventano una sola (`app/etichette_carta.hpp`): «12-18» se i numeri sono di seguito, «12+6» se no, cioè il 12 e
  altri 6, con la cornice della parte dell'evento più interessante del gruppo; avvicinandosi tornano separate. Quelle
  fuori dal riquadro non si raccolgono. La scala iniziale è la più grande che contiene la traccia e gli eventi. La carta
  va sull'evento solo quando se ne sceglie uno fuori dal riquadro, con un margine di 24 pixel; sullo stesso evento resta
  dove l'ha messa chi guarda. In basso a sinistra la nota «I O O LA ROTELLA ZOOM, TRASCINA SPOSTA»;
- **foglio** a destra, l'evento a macchina (righe di 13 pixel): «EVENTO i DI n» con l'ora `T+` a penna; il tipo nel
  colore della parte; la minicartolina incollata, larga quanto il foglio e alta 80 pixel (sotto); il testo, al più quattro righe; la distanza in miglia come voce a puntini; l'interesse come barra a
  tacche di penna;
- **elenco** sotto, sullo stesso foglio: «MOMENTI SALIENTI» o «TUTTI GLI EVENTI», con l'evento corrente al centro e
  in negativo (fondo nero, numero e ora in giallo, tipo in bianco);
- **riga d'aiuto** sopra il piede: «FRECCE EVENTO  TAB TUTTI/SALIENTI  V VERITA'/QUADRO  I O ZOOM  S STATISTICHE
  INVIO AVANTI»; Tab e V scrivono la lettura a cui si passa;
- **piede**: quattro tasti, sempre negli stessi posti, che funzionano anche col clic.

**Il piede e i tasti:**

| Pulsante | Tasto | Effetto |
|---|---|---|
| ESC CHIUDI | Esc | chiude il debriefing |
| R RIPRENDI DA QUI | R | «Riprendi da qui» dall'evento corrente (capitolo F5); spento nell'esito e nel rapporto ACINT |
| P REPLAY | P | il replay completo (capitolo F5) |
| E ESITO | E | la pagina dell'esito |
| (riga d'aiuto) | frecce, Invio | evento precedente o successivo; dall'esito, Invio va al rapporto ACINT o chiude |
| (riga d'aiuto) | Tab | tutti gli eventi o i soli momenti salienti |
| (riga d'aiuto) | V | la verità o il quadro del giocatore (sotto) |
| (riga d'aiuto) | S | le statistiche della missione (capitolo F4) |
| (riga d'aiuto) | I, O | lo zoom della carta |
| (sulla carta) | rotella | lo zoom attorno al punto sotto il puntatore (`app/trascina_carta.hpp`); fuori dalla carta non fa nulla |
| (sulla carta) | trascinare | sposta la carta |

Un clic su una riga dell'elenco porta a quell'evento.

**Verità e quadro del giocatore (V).**
- Senza V: solo gli eventi noti allora; sulla carta solo la traccia del battello.
- Con V: anche gli eventi non noti, col testo preceduto da «(NON NOTO ALLORA)»; le tracce vere di tutte le unità
  (ostili in rosso, amiche in ciano, neutrali in verde, armi in bianco tratteggiato) e la loro posizione all'istante;
  le boe sonore posate (croce bianca se ancora attive, grigia se finite).
- Cambiando lettura, l'elenco si ricompone tenendo l'evento più vicino nel tempo.

**Le minicartoline** (`app/partita.cpp`, `Partita::avanza`; `app/figurina_evento.hpp`; `grafica/cartolina`). Ogni evento
ha la sua figurina, mai un riquadro vuoto (scelta dell'utente del 07/11/2026). La sceglie `figurina_evento`:
l'icona per ciò che non si vede; la scena se c'è un soggetto da inquadrare; se no l'icona di riserva, scelta dal tipo.
- durante la missione la partita fotografa il mondo e gli effetti all'istante di ogni evento che si vede; per colpi,
  affondamenti e lanci 4 s dopo, quando la colonna d'acqua o il missile sono su;
- gli eventi di routine dell'IA non hanno fotografia (`classificazione_ia`, `comandante_ia`, `reazione_ia`, `tma_ia`,
  `scoperta_ia`, `ingaggio_ia`, `ritirata_ia`, `evasione_ia`): nella S5 prendevano 68 fotografie su 127, a danno di quelli
  veri. La loro cartolina si fa dalle pose;
- al più 120 fotografie; oltre, solo gli eventi con interesse ≥ 60; oltre 300, solo la fine e gli affondamenti. I colpi
  d'artiglieria di un tiratore dividono la sua prima fotografia: la cartolina di un colpo successivo mostra quella
  (`app/scelta_fotografie.hpp`);
- la scena viene dalla fotografia, se c'è, con il soggetto e il secondo rimessi dalle pose (`EventoMissione::pose`) se nel
  frattempo sono usciti dal mondo. Senza fotografia viene dalle sole pose: è una figurina, non la vista vera, e non ha
  fumo né colonne d'acqua (l'affondamento ha i suoi, qui sotto);
- l'**affondamento** non è una fotografia dell'istante, e la cartolina si compone (`scena_affondamento`,
  `posa_affondamento`, `effetti_affondamento`). Anche 4 s dopo la fisica non ha fatto quasi nulla (la petroliera della S5:
  sbandamento 1,4°, immersione 0,6 m), o il battello è a 200 m. Il resto del mondo viene come sopra. Il soggetto:
  - una nave, o un battello fino a 3 m di quota: lo scafo dal modello della sua classe, con una posa scritta sopra quella
    della fisica. Il modo dipende dall'unità (`unita % 3`), sempre lo stesso per la stessa: di prua o di poppa (assetto
    16°), sul fianco (sbandamento 30°, verso da come sbandava). Il battello solo di poppa, con la prua alta: sul fianco non
    affiorerebbe nulla. Falle, fuliggine e rottura restano quelle della fisica; una nave spezzata sta in due tronconi a V,
    appoggiati alla rottura. Tutta la nave in campo: camera a `max(3,4 · lunghezza, 350)` m, 3° di alzo;
  - un battello oltre i 3 m o un aereo: nessuno scafo, e la camera a 500 m, 5° di alzo;
  - attorno, schiuma, chiazza di nafta e rottami; sopra una nave o un battello a galla fumo e fuoco, a un battello immerso
    o a un aereo una colonna d'acqua. Sono effetti di questa scena sola, dallo stesso motore, col seme dall'evento:
    sempre la stessa figurina, e **né la colonna né il fumo vengono dalla simulazione**. Angoli, distanze ed effetti sono
    **STIMA**, scelte di aspetto;
  - misura del 07/11/2026, ghiaccio-d4, partite senza giocatore, seme 1: S5 1 affondamento, S5 con la regola di simulazione
    1, S07 1, S1G 2, T2 1, M3 nessuno. Un battello a galla che affonda e una nave spezzata non capitano in queste partite:
    sono visti solo in casi forzati;
- il punto di vista è quello suggerito dal registro (soggetto, secondo, distanza, elevazione, azimut), con queste
  correzioni, solo nella scena (**STIMA**, scelte di aspetto): un siluro in acqua non ha sagoma, e si inquadra chi lo
  riguarda, purché si veda (l'esca che lo ha sedotto non si vede nemmeno lei, e si passa al battello), da 800 m a pelo
  d'acqua; la camera sta al più a 1500 m, e oltre 1000 m senza alzo; l'alzo sta al più a 5°, perché il soggetto cade alla
  riga `180 + tan(alzo) · 1146` e la vista finisce a 360: a 10°, quelli di `comandante_ia`, restava il mare; un aereo nel
  campo stretto (alzo sotto 1,2°) si guarda da 1500 m; il secondo resta in campo solo entro 500 m dal soggetto, e nel campo
  stretto solo entro 70 m, perché già 400 m porterebbero la camera a 9 km; un battello immerso sta a galla, un aereo a 20 m
  di quota, salvo che nell'affondamento;
- la foto al periscopio si rifà dall'occhio del battello, col rilevamento e l'ingrandimento dello scatto: è la sola
  nell'oculare, col suo reticolo. Le figurine viste da fuori non hanno croce né tacche del periscopio (`disegna_cartolina`,
  `reticolo` spento): non sono viste dall'occhio del battello;
- quello che non si vede ha un'icona animata d'epoca: telescrivente, ESM, radiogoniometro, danni, zona, cascata,
  mina, aereo, boa, elicottero, marcatore, sonar e periscopio, per il periscopio esposto. Il SUS dell'attacco simulato
  usa il marcatore;
- l'icona di riserva (`icona_di_riserva`; **STIMA**, una scelta di aspetto), anche quando il soggetto non ha sagoma, come
  un contatto biologico: danni per colpi, affondamenti, incagli,
  collisioni ed emersioni d'emergenza; cascata per scoperte, classificazioni dell'IA e TMA dell'IA; telescrivente per
  comandante, reazioni e ingaggi dell'IA; periscopio per la foto al periscopio; sonar per il resto;
- misura del 07/11/2026, ghiaccio-d4, partite senza giocatore, seme 1: nessun riquadro blu in S5, S5 con la regola di
  simulazione, M3, S07 e SCENE1A, e una sola riserva, la scoperta di un contatto biologico in SCENE1A. Nella S5, 336
  eventi: 39 con fotografia, 233 dalle sole pose, 64 con icona. Nella S07, ferma a 3764 s, 334 eventi: 77, 134 e 123. Fra
  le scene, 65 erano mare vuoto e ora mostrano il soggetto o l'icona: `comandante_ia` 60 (S5 32, S07 26, M3 2),
  `aereo_in_sorvolo` 2, `esca_sedotta` 2, quel contatto biologico 1. Restano piccoli, da 500-800 m, elicotteri e piccole
  unità in una decina di scene; di un battello in `comandante_ia` si leggono la vela e una riga di scafo. I rami di
  `reazione_ia` a 12° non capitano in queste partite: coperti da una prova, non visti;
- i colori della cartolina, al più 32 (i più frequenti; gli altri sul più vicino), vanno negli indici 160-191 della
  palette, che né la carta né la lamiera della consolle (16-47) usano. Nell'affondamento passano prima i grigi dello scafo
  (144-158): fumo, mare e cielo finirebbero i posti, e lo scafo cadrebbe sul celeste.

### 2.6 La pagina dell'esito (E)

Il «RAPPORTO DI MISSIONE» («RAPPORTO DI CAMPAGNA» per l'Alfa), dattiloscritto sul foglio di destra:
- **ESITO** col timbro della categoria: verde il successo, marrone il successo parziale o il richiamo, rosso il resto;
- completezza in per cento, durata e meteo a fine missione (mare, visibilità, temporale, pioggia o nebbia, se c'è il
  meteo dell'Enhanced), come voci a puntini coi valori a penna;
- «OBIETTIVI» con le caselle: la X se raggiunto; il fallito in rosso, i secondari in grigio, il primario in nero;
  «fatti SU richiesti» dove serve;
- «ARMI E BERSAGLI»: lanci e a segno, affondati ostili, neutrali e amici (in rosso se ce ne sono), siluri evitati,
  colpi subiti (lo stesso conto dell'esito), classificazioni giuste ed errate, danno massimo,
  «ORDINI NON RISPETTATI»;
- il messaggio del comando, con lo spazio che resta (quello che non entra finisce coi puntini);
- in fondo il **codice partita** (o della rapida), da ricopiare per rigiocare la stessa partita (capitolo A1), e
  «INVIO: RAPPORTO ACINT» o «INVIO CHIUDE».

### 2.7 Il rapporto ACINT

Una pagina dopo l'esito, se il sonar ha un registratore (`nucleo/acint.hpp`): per ogni contatto registrato a lungo la
durata, la classificazione dalle righe stabili con la scala NATO (NONSUB, POSSUB, PROBSUB, CERTSUB), le pale, e le
righe che la libreria non conosceva, da passare all'intelligence. Titolo «ACINT REPORT», o «RAPPORTO ACINT (GRU)» per
l'Alfa. Senza contatti: «NESSUN CONTATTO REGISTRATO A LUNGO». Invio chiude.

### 2.8 Esito e cerimonie

`app/esito.cpp`. Alla prima uscita dal debriefing la carriera si chiude, una volta sola (`chiudi_partita`):
riassunto della missione, contesto, punteggio, decorazioni, promozione, provvedimenti, riconoscimenti e albo d'onore;
il fascicolo si salva subito. La traccia del replay va nella cartella del comandante se la missione conta, altrimenti
fra le ultime 20 libere. Poi le pagine, una dopo l'altra:

1. **rapporto di pattuglia** («PATROL REPORT», per la VMF «DONESENIE O POKHODE»): missione, comandante, battello, data,
   situazione, esito e motivo del fallimento, le sette voci del punteggio (R, C, S, O, E, D, V), il moltiplicatore
   scomposto, il totale timbrato, il peso di una missione ripetuta, l'equipaggio, le decorazioni automatiche;
2. **giornale** d'epoca o comunicato TASS, se l'esito fa notizia (battello perduto, neutrale o amico colpito), mai per
   le operazioni speciali;
3. **nuovi riconoscimenti**, sei per pagina;
4. **cerimonia** della decorazione personale, poi di quella di reparto;
5. **promozione**; 6. **provvedimento**; 7. **In memoriam**, se il comandante è caduto.

Le pagine senza contenuto non compaiono. Invio va avanti (sull'ultima finisce); Esc torna indietro, e dalla prima al
debriefing; i pulsanti in basso fanno lo stesso. Il punteggio e le regole sono del capitolo F2.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Debriefing evento per evento, minicartoline, esito alla F-19 | dossier 15 §7.7-7.9 (`docs/ricerca/15_schermate_simulatori.md`) | media (modello di gioco d'epoca) |
| Coda dei messaggi: 2,5-7 s, 6 in coda, critici davanti | `docs/11_comandi_e_stazioni.md` §4, roadmap punto 21 | **STIMA** di gioco |
| Avvisi dello spettatore: solo i fatti noti, 30 s fra due uguali | `docs/18_interazioni_ia.md` fase 5 | **STIMA** di gioco |
| Momenti salienti: 12, a 60 s | registro del runtime | **STIMA** di gioco |
| Fotografie: 120, interesse ≥ 60, 4 s dopo | scelta di gioco | **STIMA** |

## 4. Classico ed Enhanced

- **Classico**: nella barra resta l'ultimo messaggio, come nell'originale; niente coda, niente registro Ctrl+L, niente
  avvisi dello spettatore. Niente debriefing né cerimonie: a fine missione la barra mostra «Missione conclusa» con
  la prima riga del rapporto d'esito dell'originale.
- **Enhanced**: tutto il capitolo. Debriefing, esito e cerimonie solo per le partite avviate dal menu o col briefing
  (`--menu`, `--briefing`).
- Il registro degli eventi gira in tutti e due i modi e non cambia il classico.

## 5. Come si vede in gioco

- Volume 1, §2.8 (messaggi di bordo e Ctrl+L), §10.2 (debriefing), §10.3 (rapporto di pattuglia e cerimonie), §11.4
  (codice partita).
- Senza finestra: `--secondi N --schermata debriefing [--evento K] [--tutti] [--verita] --istantanea FILE` (K = 0
  l'esito); `--schermata esito [--pagina K]` su un fascicolo nuovo, che non si salva.

## 6. Taratura e prove

- `test/prove_coda_messaggi.cpp`: «coda dei messaggi: nessuno si perde, i critici passano davanti, tempo per
  lunghezza».
- `test/prove_registro.cpp`: scoperte, contro-scoperta, esposizione e obiettivi; l'esito dell'originale con la sua
  categoria; il lancio del giocatore con l'arma e il bersaglio; «registro: deterministico, e il classico non cambia»;
  i danni subiti con la gravità.
- `test/prove_cerimonia.cpp`: cartolina dal suggerimento del registro, icone, la scena dell'affondamento; decorazioni,
  ritratto, giornale e comunicato TASS.
- `test/prove_interazioni_scene.cpp`: le scene dello spettatore (S1-S7) si giocano su due semi.
- `test/prove_artiglieria.cpp`: «artiglieria: le cannonate a segno contano nei colpi subiti dell'esito, una volta sola,
  e arrivano a riconoscimenti e carriera», «artiglieria: i colpi d'avvertimento non contano nei colpi subiti» e
  «artiglieria: una raffica vale un colpo subito, ogni canna di un binato per se', ogni proiettile resta nel registro, e i
  riconoscimenti non cambiano».

## 7. Limiti noti e in arrivo

- **Il registro Ctrl+L non scorre**: mostra le ultime 29 righe. Una riga troppo lunga si taglia al margine.
- **La durata in barra** conta i byte del testo: le lettere accentate valgono due.
- **Gli avvisi dello spettatore** danno il rilevamento vero del punto dell'evento, non quello misurato dal sonar.
- **I momenti salienti** sono al più 12, obbligati compresi: in una missione con più di 12 eventi obbligati gli ultimi
  per interesse restano fuori (Tab li mostra tutti).
- **Le tracce della verità** hanno un punto ogni 20 s: le armi veloci si vedono a segmenti.
- **Ripresa**: il debriefing di una partita ripresa mostra il codice della partita d'origine, che la ricrea da capo
  (capitolo F5).

## 8. Riferimenti

- Codice: `app/debriefing.{hpp,cpp}`, `app/esito.{hpp,cpp}`, `app/coda_messaggi.{hpp,cpp}`, `app/partita.cpp`
  (`storico_messaggi`, `interazioni_ia`, fotografie e tracce), `app/interfaccia.cpp` (`disegna_registro`),
  `app/main.cpp` (compressione, flusso debriefing-esito), `missioni/include/missioni/registro_missione.hpp`,
  `nucleo/acint.hpp`, `grafica/cartolina`.
- Dati: `data/motore/testi/messaggi_bordo.tsv`.
- Progetto: `docs/12_schermate_di_missione.md` (M1, M4, M5), `docs/11_comandi_e_stazioni.md` §4,
  `docs/18_interazioni_ia.md` fase 5, `docs/00_ROADMAP.md` punti 16 e 21.
- Ricerca: dossier 15 §7.7-7.9.
- Capitoli: A1 (codice partita), F2 (carriera e punteggio), F5 (replay e ripresa).
