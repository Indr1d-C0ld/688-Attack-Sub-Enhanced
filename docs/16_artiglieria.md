# 16 — Artiglieria navale contro il sottomarino: piano d'implementazione

Richiesta dell'utente del 25/10/2026: le navi nemiche che hanno cannoni sparano al battello avvistato in superficie o
a quota periscopica, se dottrina e armi lo permettono, da entrambe le parti. Effetti richiesti:
- colonne d'acqua viste al periscopio alle loro distanze;
- colpi sentiti all'idrofono e dentro il battello, almeno in superficie.

Requisito 16b di `docs/00_ROADMAP.md`; ricerca `docs/ricerca/22_artiglieria_contro_sottomarini.md` (il «dossier 22»,
citato per paragrafo).

Vale **solo con la regola di simulazione**. Il classico resta identico: nel gioco del 1989 i cannoni sono solo
difesa antimissile e gli script non sparano al sottomarino (dossier 22 §8).

## Le conclusioni della ricerca che decidono il piano

- **Dove è realistico:** solo in guerra, e in due casi.
  - Il battello in superficie: qui il cannone è l'arma principale.
  - Il periscopio o un albero visti entro 2-5 km da una nave già in allarme: si spara insieme all'arma ASW sul punto
    (§2, §9.2).
- **In pace e in crisi:** solo colpi d'avvertimento e segnali esplosivi, mai sul bersaglio (§2.4).
- **Il battello immerso è al sicuro:** il proietto scoppia sull'acqua, rimbalza o si capovolge in pochi metri.
  - Tutto ciò che sta 3 m o più sotto la superficie è al sicuro (§5.2).
  - A quota periscopica rischiano solo le teste dei periscopi, gli alberi e la cima della falsatorre (§5.3).
- **Le armi non sono uguali:**
  - Il Phalanx del 1989 non spara a bersagli di superficie.
  - L'AK-630 sovietico sì, entro circa 2 km dal periscopio.
  - L'Alfa quindi rischia più del 688 (§3.2).
- **Il cannone è un rischio di contorno.** Punisce il periscopio alzato troppo a lungo vicino a una scorta in allarme
  e il battello sorpreso in superficie. Non deve diventare il modo normale di uccidere il battello (§9.1).

## Che cosa c'è oggi nel codice

- **Nessuna IA di dottrina per le navi.** Le navi attaccano solo come dicono gli script della VM dell'originale (i
  colpi di `Missione::colpi()`). L'unico strato di dottrina dell'Enhanced è quello degli aerei ASW
  (`ControlloreAsw`, docs/13), e il suo schema era pensato anche per le navi: l'artiglieria sarà il primo pezzo di
  dottrina navale.
- **Scoperta:** radar, vedette, ESM ed esposizione del periscopio ci sono già.
  - L'esposizione del periscopio sta in `StatoPeriscopiMissione` (docs/14).
  - L'avvistamento del giocatore è un evento del registro M1.
- **Effetti:** ci sono le esplosioni in aria e sotto l'acqua (`Effetti::esplosione_aerea`, `esplosione_subacquea`) e
  le colonne d'acqua delle cariche. Mancano le colonne dei proietti, i rimbalzi, le vampe, gli illuminanti e i
  traccianti.
- **Danni:**
  - Il modello dei sottomarini colpiti (`DanniSottomarini`) lavora sulle esplosioni subacquee.
  - I guasti dei periscopi esistono già come sistemi (docs/14, P3).
  - Mancano il danno da schegge alle teste e agli alberi e i colpi diretti sulla falsatorre in superficie.
- **Acustica:** le sorgenti transitorie esistono già (tonfi, SUS, ping delle boe: `ascolta_transitorio`). Mancano il
  colpo di partenza e lo scoppio in superficie.

## Pezzi e chi li fa

| Pezzo | Cosa | Chi |
|---|---|---|
| **G0 Dati** | `data/motore/artiglieria.toml`, con fonte e confidenza per riga come `periscopi.toml`: pezzi (calibro, cadenza, colpi pronti, v0, carica, depressione, portate contro la superficie e il periscopio, reazione, σ) e classi (pezzi, CIWS contro la superficie). Le tavole di volo del §3.4 precalcolate (tempo di volo e angolo di caduta per distanza). Il caricatore in `nucleo/` | kit (dati) e runtime (caricatore), dopo i loro lavori in corso |
| **G1 Modulo puro** | `nucleo/artiglieria.{hpp,cpp}`, senza missione: il tiro di un colpo (§9.4) e l'impatto con la sagoma esposta (diretto, scoppio, rimbalzo, VT), con le schegge entro r_eff. Il caso entra dal chiamante, quindi il modulo è deterministico. Prove sulle statistiche: periscopio a 3 km da un Mk 75 colpito circa una volta su due in 30 s | runtime |
| **G2 IA delle navi** | La regola di fuoco del §9.2 nella missione, con regola di simulazione: ROE (pace, acque territoriali, crisi, guerra), prontezza (posto di combattimento o crociera, reazione del §4.2), bersaglio classificato con traccia solida, portata, fuoco amico (nessuna nave amica entro 15° dalla linea di tiro), cessate il fuoco 5 s dopo la sparizione del bersaglio, munizioni. È il primo pezzo della dottrina navale, sullo schema di `ControlloreAsw` | runtime |
| **G3 Danni** | §9.5: teste dei periscopi (cieco, bloccato, degradato; col Type 18 anche l'ESM nella testa), alberi ESM e radio, falsatorre e plancia in superficie, scafo resistente in superficie per calibro. Il 16" lascia una falla certa | kit (danni) |
| **G4 Grafica** | §9.6: colonne d'acqua per calibro (anche tinte per l'Iowa), lampo alla base, spruzzi del rimbalzo, sbuffo dei VT; vampe e fumo sulle navi; illuminanti a paracadute con la luce sul mare; traccianti dell'AK-630. Nell'oculare: spruzzi e acqua sul vetro entro ~100 m, immagine che si spegne o vetro incrinato al colpo diretto | kit (effetti) |
| **G5 Audio** | §9.7: sorgenti transitorie nel modello acustico (colpo di partenza 40-150 Hz, scoppio in superficie tagliato in basso dallo specchio, raffica dell'AK-630, tonfo dell'inerte); a bordo lo schiocco sullo scafo entro 1-2 km e, in superficie, boato col ritardo R/340 s, sibilo, scoppi e spruzzi | disassembler |
| **G6 App** | Messaggi nella fascia **420-429** (sonarista: «esplosioni in superficie, cadenza tre secondi: tiro d'artiglieria»; vedetta: «vampe a dritta», «colonne d'acqua vicine»; ufficiale di rotta: «il periscopio d'attacco non risponde»). Gli effetti e i suoni passati alla vista e al bordo. Le cadute osservate sulla carta come piccole croci (solo se viste). Gli eventi nel debriefing e nelle minicartoline. La difficoltà del §9.9 nelle opzioni | principale |
| **G7 Taratura** | Le tre missioni di prova del §9.10: periscopio alzato 30 s a 3 km da una Perry in allarme (colpito circa una volta su due); emersione a 8 km da un Kara (il battello quasi sempre si immerge prima dei danni gravi); battaglia di superficie lontana sentita al sonar | runtime, con `strumenti/taratura` |

## Ordine e tempi

- Si comincia dopo i lavori in corso, per non incrociare i file:
  - il runtime finisce taratura ASW, modulo d'evasione e aerei ad ala fissa;
  - il kit finisce i pannelli della Fase 3;
  - il disassembler finisce la velocità dello spettro acustico.
- Poi G0 e G1 insieme, G2 dopo G1, e G3, G4, G5 in parallelo appena c'è l'intestazione di G1 (il contratto degli
  impatti: posizione, quota, calibro, tipo, colpo diretto su cosa). G6 e G7 per ultimi.
- Il contratto dell'intestazione di G1 va approvato dal principale prima del codice, come per l'ASW aereo.

## Decisioni prese

- **Classico identico:** l'artiglieria esiste solo con la regola di simulazione, e un file di dati assente la spegne.
- **Fascia dei messaggi:** 420-429, al principale (avvisi e voci dell'equipaggio).
- **Il Phalanx non spara al periscopio**, perché nel 1989 non poteva. L'AK-630 sì, entro 2 km, col radar agganciato.
- **L'Iowa:** al periscopio solo i 5"/38; il 16" al battello in superficie solo oltre i 2 km, come evento raro.
- **Difficoltà del §9.9:**
  - principiante: niente fuoco al periscopio, metà della precisione contro la superficie e reazione doppia;
  - esperto: reazione × 0,7 e fuoco sulla piuma entro 1 km.
- **Missioni:** le regole delle acque territoriali in pace (colpi e bombe d'avvertimento) valgono solo per le
  missioni che le dichiarano. Nelle 22 originali non ce n'è nessuna.

## Stato

- 25/10/2026: dossier 22 pronto; piano scritto (questo file). Nessun pezzo avviato.
- 26/10/2026 (runtime): **G0, G1 e G2 fatti**, col contratto approvato dal principale (nota
  `artiglieria-contratto-g1.md` nella cartella delle sessioni).
  - G0: `tools/pipeline/artiglieria.py` → `data/motore/artiglieria.toml`. Ci sono 10 pezzi con fonte e confidenza,
    tavole di volo, 10 classi armate e le sagome dei sottomarini in superficie. La prima versione è del runtime; il
    kit la rivede con G3.
  - G1: `nucleo/artiglieria.{hpp,cpp}`. Contiene `spara()` (mira con l'anticipo, errore N(0, σ) con le correzioni
    del §3.6, caduta nella sagoma con lo spazio pericoloso, rimbalzo sotto 8°, VT, raffiche, schegge entro r_eff con
    il cono in avanti di 50 m, STIMA), `SagomaEsposta`, `ImpattoArtiglieria` ed `eventi_di()` verso gli
    `EventoArtiglieria`.
  - Tiro misurato in prova: testa del periscopio colpita o raggiunta dalle schegge del Mk 75 nel 24% dei colpi a
    1 km e nel 6,7% a 3 km. Il 688 in superficie a 3 km dal Mk 45 è colpito nel 68% dei colpi di traverso e nel 72%
    di prora: col tiro radente lo spazio pericoloso fa assorbire l'errore in distanza ai 110 m di scafo, mentre il
    §3.6 stimava 25-40% di prora.
  - G2: `missioni/src/missione_artiglieria.cpp`, con la regola del §9.2:
    - situazione dal blocco carriera o da `carriera.toml`; un lancio del giocatore fa della crisi una guerra;
    - prontezza e reazione del §4.2 per pezzo; bersaglio esposto, visto da radar o vedetta con traccia solida;
    - portata e distanza minima, fuoco amico entro 15°, cessate il fuoco 5 s dopo la sparizione con un ultimo colpo
      sul punto;
    - munizioni e pause di ricarica; in crisi tre colpi d'avvertimento ogni 2 min; di notte gli illuminanti, che
      durano 50 s;
    - difficoltà dell'ASW: principiante senza fuoco al periscopio, P × 0,5 e reazione × 2; minaccia percepita:
      reazione × 0,7.
  - Le uscite sono `Missione::impatti_artiglieria()` ed `eventi_artiglieria()`, i transitori nella scena (id 990000
    + 10·n) e il registro M1 (`artiglieria_salva`, `artiglieria_colpo`, `artiglieria_vicino`).
  - Restavano il fuoco sulla piuma dell'esperto, le acque territoriali dichiarate dalla missione e le navi contro i
    sottomarini dell'IA: fatti il 26/10/2026, sotto.
- 26/10/2026 (runtime): **G7 fatto**. Le tre missioni di prova del dossier 22 §9.10 stanno in
  `tools/taratura/missioni/g7_*.lua`; si lanciano con `taratura artiglieria SEMI file`, e i risultati sono in
  `tools/taratura/risultati/artiglieria_g7_*.tsv`.
  - **G7.1**, l'Alfa che alza il periscopio 30 s a 3 km da una Perry al posto di combattimento: testa colpita, diretto
    o schegge, **0,58** su 200 semi (atteso circa 0,5); primo colpo mediano 22 s dopo la testa fuori.
    Dal 27/10/2026 sera l'esito di una granata si verifica all'arrivo: quelle che cadono dopo che il periscopio è
    sceso vanno in acqua, e G7.1 scende a **0,41** su 100 semi (`tools/taratura/risultati/fondo_2710/`). L'atteso del
    dossier 22 §9.10 va rivisto per questa ragione (nota lì).
  - **G7.2**, il 688 in superficie a 8 km da un Kara in condizione III, che si immerge d'emergenza 10 s dopo il primo
    colpo: sempre sotto, in media 18 s dopo il primo colpo, con 0,7 colpi diretti; scafo colpito 0,38 (76 mm,
    ammaccature); **danni gravi 0,02** (non si immerge più, o una falla). Atteso: quasi sempre sotto prima dei danni
    gravi.
  - **G7.3**, Kara contro Perry a 7 km con `artiglieria = { fra_navi = true }`, e il 688 immerso a 100 m che ascolta: i
    colpi in scena si sentono nel 97% dei campioni a 10, 20 e 40 km, con un eccesso di segnale massimo di 58, 48 e
    38 dB.
  - Le scelte che la taratura ha fissato, tutte STIMA:
    - il tiro d'aggiustamento sui primi 3 colpi di un pezzo;
    - la reazione 0,9-1,7 volte quella del pezzo;
    - il posto di combattimento 120 s dopo il primo contatto in guerra, se la nave era in condizione III;
    - `Missione::posto_di_combattimento(nave)`, anche dagli script con `m688.posto_di_combattimento(u)`, per le navi
      già in allarme.
  - Nuovo: **artiglieria fra navi** con la chiave della missione `artiglieria = { fra_navi = true }`. In guerra le
    navi sparano alle navi ostili di cui hanno traccia solida e posizione; le 22 originali non l'hanno. I danni alle
    navi sono del kit: gli impatti sono già in `impatti_artiglieria()`, col bersaglio nave.
- 26/10/2026 (principale): **G6 fatta**. Il G3 del kit è nel suo stato a parte.
  - Gli eventi vanno agli effetti (`Effetti::artiglieria`, da `Partita`) e al bordo (`AmbienteBordo::artiglieria`,
    dall'interfaccia, accanto agli scoppi); nella ripresa si saltano quelli già passati.
  - Avvisi 420-429 (`tools/pipeline/esporta_motore.py`), composti in `Partita` col rilevamento:
    - col battello esposto: vampe una volta al minuto per nave, colonne d'acqua entro 150 m, illuminanti (senza
      rilevamento se sono sopra di noi);
    - in immersione: il sonarista che sente gli scoppi entro 15 km;
    - colpi sul battello: periscopio d'attacco o di ricerca, schegge sulla testa, alberi, vela; lo stesso rapporto
      al più ogni 30 s; i colpi d'avvertimento ogni 5 minuti.
  - Carta: le cadute viste (battello esposto, entro 10 km) sono croci ostili in `CartaGiocatore::cadute`, le ultime
    40, a parte dai segni del giocatore.
  - Oculare:
    - spento, incrinato e seme dallo stato dei danni G3 del periscopio in uso (`DanniSottomarini::artiglieria`);
    - l'acqua delle colonne sul vetro (`Effetti::acqua_sul_vetro`);
    - la luce degli illuminanti sul mare dove guarda il periscopio, sommata alla luce della vista.
  - La difficoltà del §9.9 è l'opzione K del menu, ora «NEMICI (ASW, CANNONI)»; il debriefing usa i testi del
    registro del runtime.
  - Provato con la scena del Krivak II a 4 km dal 688 in superficie, di giorno e di notte (vampe, colonne, colpi
    sulla vela, schegge sulla testa, immagine spenta e vetro incrinato, illuminanti). Classico e impronte identici.
- 26/10/2026 (runtime): **le tre estensioni del G2** (`missioni/src/missione_artiglieria.cpp`).
  - **Navi contro i sottomarini dell'IA.** In guerra una nave spara anche a un sottomarino dell'IA ostile ed esposto,
    se lo vede da radar o vedetta con traccia solida. Prima viene il giocatore, poi il sottomarino più vicino, poi le
    navi con `fra_navi`.
    - Sotto il tiro, entro 300 m, il sottomarino ammaina gli alberi e scende ad almeno 60 m (22 §9.8, STIMA).
    - I danni sono quelli di `DanniSottomarini::colpo_artiglieria`, come per il giocatore.
  - **La piuma dell'esperto** (`vede_la_piuma`; difficoltà «minaccia percepita», 22 §9.9).
    - In guerra la nave spara sulla piuma della testa alzata anche senza averla vista: entro 1 km di giorno, 300 m di
      notte.
    - Condizioni: mare fino a 4; piuma della testa alla sua velocità, `nucleo/periscopi`, almeno 0,2 (circa 5 nodi
      col periscopio d'attacco).
    - L'errore sulla stima del punto è di 25 m (STIMA).
  - **Acque territoriali dichiarate** (chiave `acque_territoriali`, docs/09 §4).
    - Vale in pace, col giocatore straniero dentro, e solo per le navi del paese costiero.
    - Battello esposto e visto: i colpi d'avvertimento della crisi.
    - Battello immerso con la posizione nota (un ping, per esempio): due bombe d'avvertimento ogni 2 minuti, a
      500-1000 m dalla stima; dopo 20 minuti dentro a 200-400 m. Precedenti: Svezia 1982-83, 22 §2.4 e §9.2.
    - Le bombe vere non ci sono: le bombe sono impatti d'avvertimento (`pezzo = "bomba_avvertimento"`, 1 kg) con lo
      scoppio per audio e grafica.
    - Registro `acque_territoriali` (entrata e uscita); messaggi 453 entrata, 454 uscita, 455 bombe.
  - Prove: `test/prove_artiglieria.cpp`, i tre casi nuovi.
