# 688: Attack Sub, edizione Enhanced: visione e roadmap

Progetto: remake single-player in italiano di *688 Attack Sub* (Electronic Arts, 1989,
John W. Ratcliff). Mantiene l'estetica VGA 256 colori a pixel e il suono AdLib/Sound Blaster
dell'epoca 486. Motore di simulazione riscritto da zero, realistico e documentato.

## Requisiti del progetto (dalle indicazioni del committente)

1. **Lingua italiana** per testi, voci di bordo, interfacce e manuali.
2. **Solo single player** (il codice modem/null-modem dell'originale non viene portato).
3. **Meccaniche riscritte** in modo realistico e profondo, basate su documentazione
   ufficiale, tecnica, accademica e storica dei due blocchi (vedi `docs/ricerca/`).
4. **Stealth, sonar, radar e oceanografia**: sonar passivo/attivo con equazione del sonar,
   waterfall broadband e narrowband (LOFAR/DEMON), firme acustiche e tonali per classe,
   termoclini e profili di velocità del suono, zone d'ombra, zone di convergenza, canale
   SOFAR, rumore ambiente (mare, pioggia, traffico, biologici, ghiaccio), batimetria reale.
   **IA** delle unità non controllate riscritta da zero, matura e ispirata alla dottrina.
5. **Estrazione** di tutti gli asset originali (in corso, vedi `01_analisi_tecnica_originale.md`).
6. **Manualistica**: traduzione italiana del manuale originale, affiancata al nuovo manuale
   tecnico completo dell'edizione Enhanced. **Statistiche**: report personale dettagliato
   (abitudini, tempi, profondità, velocità, esposizioni, efficacia armi, heatmap di
   posizione, esposizione acustica e rilevamenti) più statistiche globali cumulative.
   - *Statistiche (26/10/2026):* il disassembler ha fatto il nucleo (nucleo/statistiche): il report di missione dalla
     traccia del replay e le cumulate per comandante e globali, con primati e heatmap per teatro. La principale le
     registra a fine missione. Le schermate sono di trailer-1 (app/statistiche): S nel debriefing per la missione
     (quota nel tempo, livello irradiato coi ping e le trasmissioni, armi, prima scoperta nemica, heatmap sulla carta)
     e S nell'albo dei comandanti per le cumulate del comandante o le globali (totali, primati, heatmap per teatro).
   - Il manuale originale è tradotto (docs/traduzione/manuale).
   - *Manuale dell'Enhanced come compendio (richiesta dell'utente, 26/10/2026)*: deve essere un vero riferimento
     tecnico e storico, grande e completo, dove chi gioca trova qualunque informazione. Tre volumi in
     `docs/manuale_enhanced/`, curati dal kit:
     1. manuale d'uso, in 11 capitoli più le appendici;
     2. riferimento tecnico: ogni modello con formule, parametri, fonte e confidenza, limiti, differenze col classico;
        le tabelle dei dati generate da uno script dai file del motore;
     3. storia e unità: il gioco del 1989, la Guerra fredda sotto il mare, i teatri, gli episodi storici, ogni unità
        e arma con foto a licenza libera e immagini del gioco, le statistiche.
     I capitoli tecnici li scrivono le sessioni che possiedono ciascuna area.
7. **Meteo dinamico** realistico, con effetti su rumore ambiente, propagazione, periscopio,
   radar, operazioni aeree ed elicotteristiche, sonar delle navi di superficie.
   - *Stato del mare per tutti (richiesta dell'utente, 25/10/2026).* Le forze del mare (Douglas o Beaufort) vanno
     previste con tutto quello che comportano per **ogni** entità, sia nelle animazioni sia nelle capacità e nelle
     prestazioni. Oggi l'altezza delle onde esiste nella vista e nel rumore ambiente, e alcuni limiti sono già nei
     dossier (boe sovietiche inutili oltre mare 3, niente sonar a immersione da mare 5, radar nemico peggiore da mare
     5). Mancano un modello unico e le sue conseguenze ovunque:
     - **animazioni:** onde e creste, schiuma e spruzzi, beccheggio e rollio di navi, battelli in superficie e scialuppe;
       la vela e il periscopio che emergono e si affogano; elicotteri che tengono l'hover con difficoltà; la vista
       del periscopio che va sott'acqua tra le onde;
     - **capacità e prestazioni:**
       - tenuta della quota periscopica e rischio di affioramento;
       - piuma e scia meno visibili;
       - radar contro l'albero;
       - rumore ambiente e propagazione superficiale;
       - sonar a scafo delle navi (aerazione, slamming);
       - velocità massime e virate delle navi piccole;
       - lancio di armi e missili, siluri vicino alla superficie;
       - operazioni aeree, boe e sonar a immersione;
       - mine ormeggiate;
       - danni e allagamento in superficie;
       - emersione e salvataggio;
     - **dati:** stato del mare per teatro, stagione e ora, che cambia durante la missione con il vento; scelta nel
       briefing, nelle missioni rapide e nell'editor.
     - *Meteo dinamico (richiesta dell'utente al kit, 25/10/2026).* Ogni teatro ha un meteo realistico che cambia
       durante la missione col tempo e con la stagione, ed è riportato nel **pre-briefing**. Influisce su capacità,
       prestazioni e sensori di tutti i mezzi: foschia e nebbia riducono la visibilità per tutti; col maltempo (i
       temporali) gli elicotteri diventano difficili o impossibili da usare. Al periscopio l'acqua sale e scende sulla
       finestra con le onde, e restano gocce e spruzzi (il kit lo fa nella vista dei periscopi, P3).
     - *Meteo estremo e meteo via radio (richiesta dell'utente al kit, 25/10/2026).*
       - **Eventi rari ma possibili:** fulmini resi in modo realistico, drammatici di notte e in bassa luminosità, meno di
         giorno. Il lampo schiarisce la palette per poche decine di millisecondi con le 3-4 scariche ripetute, più lo
         sprite del fulmine. Il fulmine in mare si sente al sonar, e i temporali portano scariche in sala radio sull'HF.
       - **Meteo dalla centrale di comando:** avvisi di burrasca e previsioni sulla trasmissione programmata ai
         sottomarini. Si ricevono a quota periscopica o col filo galleggiante; chi non ascolta li perde.
       - **Chiedere una previsione trasmettendo:** è poco realistico per un SSN nascosto (EMCON). Sarà una scelta
         possibile, col suo costo di scoperta, sensata nelle missioni che non devono restare nascoste.
     - *Superficie del mare dinamica (richiesta dell'utente al kit, 25/10/2026; fatta nella vista dell'Enhanced).*
       Il mare non è più piatto: quattro onde lunghe attorno al picco di Pierson-Moskowitz e quattro corte del vento,
       ferme nel mondo e in moto alla loro velocità di fase. Ci sono i riflessi del cielo, la foschia verso
       l'orizzonte e la schiuma delle creste (Monahan). Al periscopio l'acqua sale sulla finestra, con gocce e
       spruzzi. Il classico resta il gradiente di oggi. L'onda è la stessa della simulazione (`nucleo/onde.hpp`):
       a mare 3 la finestra non si copre mai, a mare 6 spesso.
     - *Avviato (25/10/2026):* dossier 21 fatto, piano in `docs/15_meteo_e_mare.md`. I pezzi:
       - M0 dati e M3 grafica: kit;
       - M1 modello, M2 effetti e la radio di M4: runtime;
       - il pre-briefing di M4: principale;
       - M5 audio: disassemblatore;
       - M6 taratura: runtime col principale.
     - Prima un dossier unico, fatto dal kit: `docs/ricerca/21_meteo_e_mare.md`. Raccoglie le soglie reali per ogni
       mezzo e sistema, la climatologia dei teatri e il contratto di un modulo `nucleo/meteo` con il mare. Poi un
       piano a pezzi, come per l'ASW aereo e i periscopi. Il pre-briefing col meteo è del principale.
8. **Ciclo giorno/notte** (con astronomia reale per latitudine e data) e **periscopio**
   rifatto: ingrandimenti reali, stadimetria, ESM, visione notturna/IR dell'epoca,
   esposizione dell'albero al radar. Tutte le **stazioni di bordo** tecnicamente realistiche.
   - *Modalità di visione (richiesta dell'utente, 25/10/2026).* Oggi ci sono due ingrandimenti, lo stadimetro, la
     notte con la luce ambiente e l'ESM. Vanno aggiunte le modalità dei periscopi veri, da ricercare nel dossier 19:
     - 688: Type 2 d'attacco e Type 18 di ricerca, con ingrandimenti, ricerca in elevazione nel cielo, telemetro,
       intensificatore o TV a bassa luminosità, foto e video;
     - Alfa: i periscopi del Progetto 705.
     - **Da implementare nel 688 (richiesta dell'utente, 25/10/2026):** la centrale con due periscopi
       selezionabili, il Type 2 d'attacco e il Type 18 di ricerca. Ciascuno ha le sue prestazioni, le sue modalità di
       visione e la sua esposizione (testa sottile contro testa grande). È importante per il gameplay.
     - Prima il dossier `docs/ricerca/20_periscopi.md` (kit), sui periscopi di tutte le marine in gioco: dati
       tecnici, visibilità al radar, estetica dell'oculare e del posto, uso in dottrina. Comprende anche i periscopi
       dell'IA.
     - *Avviato (25/10/2026):* il dossier 20 è fatto e il piano è in `docs/14_periscopi.md`. I pezzi:
       - P0 dati e P3 vista: kit;
       - P1 modello e P2 missione: runtime;
       - P4 stazione: principale;
       - P5 taratura: runtime col principale.
   - *Caratteri delle postazioni (richiesta dell'utente, 25/10/2026).* Oggi tutte le stazioni usano il font
     dell'originale. Si studieranno nel dossier 19, dalle foto vere, i caratteri degli schermi (fosfori dei CRT,
     terminali), delle serigrafie e dei display. Ne nasceranno font bitmap disegnati da noi da zero: per esempio un
     «fosforo terminale» per il 688 e uno a stampino per l'Alfa.
9. **Armamento reale** e storicamente coerente con il teatro e con l'anno.
10. **Editor di missione** avanzato: scelta della mappa, posizionamento di unità amiche e
    nemiche, comportamenti, livello di "paranoia" e tensione, tipo di missione, meteo,
    stagione, ora, regole d'ingaggio, condizioni di vittoria e trigger. Obiettivo: la massima
    libertà di progettare qualsiasi situazione nel teatro operativo.
    - *Esportazione e importazione* per scambiare missioni con altri giocatori: un unico file
      (pacchetto con dati della missione, script, briefing in italiano, eventuale anteprima),
      con versione del formato, controllo di integrità e verifica delle dipendenze (teatro, classi
      di unità, versione del gioco). Gli script delle missioni importate girano in un ambiente
      **isolato** (Lua senza accesso a file, rete o sistema), perché arrivano da terzi.
    - *Missione rapida procedurale* generata a caso: teatro, stagione, ora, meteo, tipo di missione,
      forze in campo, comportamento e "paranoia" dell'IA, difficoltà. Il generatore è
      **deterministico da un seme**: stesso seme e stessi parametri danno la stessa missione, quindi
      basta scambiarsi un breve **codice missione** (seme + parametri) per giocare la stessa partita.
      Ogni missione generata si può aprire nell'editor per modificarla e salvarla.
    - *Libreria di missioni pronte (richiesta dell'utente, 25/10/2026, «per quando sarà possibile»).* Un buon numero di
      missioni dell'editor già fatte, da giocare o da usare come base da modificare, di qualità alta come soggetto:
      - ispirate a fatti reali della Guerra Fredda, per i due blocchi (pedinamenti, intercettazioni di cavi, recuperi,
        incidenti, crisi), più scenari del tutto nuovi;
      - difficoltà e situazioni diverse, per dare longevità alla simulazione.
      - Piano proposto:
        1. un dossier di ricerca sugli episodi storici utili e su cosa se ne può giocare;
        2. le schede dei soggetti, da far approvare all'utente;
        3. le missioni costruite con l'editor (scenario, regole, briefing) e distribuite come pacchetti;
        4. le prove: ogni missione si gioca senza errori, si vince e si perde nel modo previsto.
    - *Missioni rapide: fatte (25/10/2026).* Il generatore è della sessione del trailer (7 tipologie, codice di 13
      caratteri); il menu, l'avvio e la carriera sono del principale (docs/12). L'editor è avviato: la proposta è
      approvata e partono le fasi E1-E3.
    - *Codice partita col seme (richiesta dell'utente, 26/10/2026): fatto dalla principale.* Prima le originali e la
      libreria partivano dal menu sempre col seme 1, e le varianti dell'originale non cambiavano mai. Ora:
      - ogni partita ha un seme nuovo;
      - il codice di 18 caratteri (`app/codice_partita`) ha l'impronta del file della missione, il seme e le opzioni
        del menu;
      - si vede nel briefing, nell'esito del debriefing, nell'albo d'onore e nel replay;
      - si digita con P nella scelta della missione (anche quello delle rapide) o con `--codice`, e rifà la stessa
        partita;
      - se la missione è cambiata, il codice non la trova e lo dice.
      Le rapide tengono il loro codice di 13 caratteri.
    - *Fedeltà del classico nell'app (26/10/2026), trovata scrivendo il manuale.* Tre correzioni della principale:
      - la missione ora avanza a passi di 1/9 s come la VM dell'originale, non più a 1/30 s: prima le regole a passi
        correvano 3,3 volte più veloci e gli esiti differivano dal rilievo; ora coincidono;
      - le originali dell'Enhanced partono dalla data dei loro ordini;
      - il MATE legge le velocità in nodi di gioco.
11. **Danni e riparazioni** realistici: compartimenti, allagamento in funzione della
    profondità, incendi, urti e shock, squadre di controllo danni con priorità, sistemi
    degradati invece che spenti, emersione d'emergenza.
12. **Alfa sovietico**: niente cirillico illeggibile. Caratteri latini con un font bitmap
    dallo stile "sovietico" (strumentazione squadrata, stampino) per le stazioni dell'Alfa.
    - *Stazioni diverse nell'aspetto (richiesta dell'utente, 25/10/2026).* Oggi 688 e Alfa differiscono solo nelle
      funzioni: tubi, quote, velocità, tempi di ricarica, armi in acqua. La grafica delle stazioni è la stessa.
      - Vanno differenziati l'aspetto, la disposizione delle consolle e la presenza o assenza di sezioni e comandi,
        secondo le postazioni vere:
        - 688 Flight I: BQQ-5, Mk 117 per il tiro, consolle di governo, ESM WLR-8;
        - Progetto 705: sistema di combattimento automatizzato, 30 uomini d'equipaggio, comandi accentrati in
          centrale.
      - Prima un dossier di ricerca sulle consolle e sugli interni dei due battelli. Poi la passata grafica della
        Fase 3.
      - *Dossier fatto (25/10/2026):* `docs/ricerca/19_consolle_688_alfa.md`. Contiene la proposta per le 8 stazioni,
        un tema di colori per battello e i caratteri «Gorton» per il 688 e a stampino per l'Alfa. Scelte
        dell'utente:
        - fosforo verde giallino per gli schermi sovietici;
        - sull'Alfa gli slot del sonar solo nel modo moderno, perché a bordo non c'era un registratore;
        - un guasto dell'Akkord blocca tutte le armi, com'era davvero.
13. **Mezzi aerei ASW**: elicotteri (Ka-25 Hormone, SH-60B Seahawk LAMPS III) e
    pattugliatori (P-3C Orion) con sonar a immersione, sonoboe, MAD e siluri, modellati
    con equipaggiamento e tattiche reali.
    - *Nota del 25/10/2026, animazioni dell'ASW aereo.* Oggi né l'originale né il remake le hanno.
      - Nell'originale l'SH-60B pattuglia avanti e indietro. Quando una nave amica è colpita vola sul punto e ci gira
        in tondo, e la scoperta è solo un numero. La «boa marcatrice» della missione 7 è un'unità che compare sul
        bersaglio, senza lancio. I siluri da elicottero compaiono vicino al bersaglio, senza volo né caduta.
      - Nel remake l'elicottero ha il modello del kit, con i rotori resi a quadratini come nel 1989, e per radar ed
        ESM vola a 150 m fissi. Non si vedono stazionamento, cavo, lanci di boe o di armi.
    - *Com'era davvero (verificato).*
      - L'SH-60B LAMPS III non aveva il sonar a immersione, anche se il manuale del 1989 lo elenca. Aveva il MAD
        trainato («bird»), 25 tubi pneumatici per le sonoboe sul fianco sinistro e due Mk 46. Il sonar a immersione
        AN/AQS-13F era dell'SH-60F, che entrava in servizio proprio allora, e prima del Sea King SH-3H.
      - Il Ka-25PL e il Ka-27PL avevano il sonar a immersione, oppure il MAD, più fino a 36 boe RGB. Come carico
        portavano a scelta un siluro (AT-1M, Orlan, APR-2) oppure 6-8 cariche di profondità convenzionali, o una
        nucleare.
      - Gli elicotteri hanno davvero lanciato cariche di profondità. Il 25 aprile 1982 un Wessex lanciò due Mk 11
        contro il sottomarino argentino Santa Fe, quasi in superficie, e lo danneggiò. Un Lynx gli lanciò un Mk 46,
        che passò sotto.
      - Negli anni '80 la US Navy non usava più cariche convenzionali dagli elicotteri. Fonti:
        [Wikipedia SH-60](https://en.wikipedia.org/wiki/SH-60), [Wikipedia Ka-27](https://en.wikipedia.org/wiki/Kamov_Ka-27),
        [Royal Naval Association](https://royal-naval-association.co.uk/news/25-april-santa-fe-attacked-south-georgia-retaken).
    - *Da fare (Enhanced; prima un dossier di ricerca con le procedure e i tempi):*
      - stazionamento a 5-15 m, con l'anello di spruzzi del flusso dei rotori sull'acqua;
      - il cavo del sonar a immersione che scende e risale, i ping che si sentono al sonar, poi il «salto» al punto
        successivo;
      - sonoboe espulse dal fianco, col tonfo, il galleggiante con l'antenna e la macchia di colorante;
      - il MAD trainato sotto l'elicottero;
      - siluro col paracadute e tonfo in acqua;
      - cariche di profondità con le esplosioni della fisica che c'è già;
      - suoni: i rotori filtrati dalla superficie e le boe che cadono, come al punto 27.
      - Servono IA, sonar, effetti e kit insieme. Il classico resta com'è.
    - *Aerei ASW (richiesta del 25/10/2026).* Nelle 22 missioni originali c'è solo l'SH-60B. P-3C Orion, Il-38 May e
      Tu-142MK Bear-F hanno il modello nel kit, ma oggi non hanno un comportamento. Dalla dottrina della Guerra
      Fredda vanno ricavati:
      - le missioni tipiche: barriera fra Groenlandia, Islanda e Regno Unito, ricerca sul datum indicato dal SOSUS,
        scorta a un gruppo navale, sorveglianza dei bastioni sovietici;
      - gli schemi di sonoboe e di passaggi MAD, con quota, velocità e durata in zona;
      - la coordinazione con navi e sottomarini;
      - le manovre da fare con probabilità diverse, secondo il contatto, la fase e l'equipaggio;
      - le armi;
      - le differenze fra la dottrina americana e quella sovietica;
      - cosa se ne vede e se ne sente dal battello: il periscopio, le luci di notte, i radar all'ESM, il rumore
        filtrato in acqua, le boe che cadono.
      Prima un dossier di ricerca (`docs/ricerca/17`), poi l'IA e la grafica nell'Enhanced.
    - *Piano (25/10/2026).* Il dossier 17 è pronto. Il piano d'implementazione è in `docs/13_asw_aereo.md`:
      - pezzi da A0 ad A7: dati, modulo puro `nucleo/asw_aereo`, acustica di boe e sonar a immersione, missione,
        kit, app, taratura, ricerca;
      - chi fa cosa, il contratto dell'intestazione e le missioni di prova;
      - le decisioni: l'SH-60B delle originali nell'Enhanced passa al controllore, e la boa della missione 7 diventa
        una DICASS posata davvero.
14. **Museo**: sezione con tutte le unità, armi e sensori della simulazione: sprite originali
    da tutte le angolazioni, schede tecniche complete con fonti, descrizione storica,
    firma acustica ascoltabile e waterfall dimostrativo.
    - *Enciclopedia e manuale in simulazione (richiesta dell'utente, 25/10/2026).*
      - Il Museo diventa un'**enciclopedia di tutto ciò che è nella simulazione**: sottomarini, navi, aerei,
        elicotteri, armi, mine, esche, boe, sensori (sonar, radar, ESM, periscopi), sistemi di bordo, teatri e basi.
      - Ogni voce ha la scheda tecnica con fonti e confidenza, la descrizione storica e d'impiego, foto reali
        digitalizzate «alla 1989» (solo con licenza libera, dai crediti) e immagini in gioco: sprite, modelli 3D,
        sagome, effetti.
      - Le stesse schede si consultano **dentro la missione**: per esempio dalla stazione Armi e dalla sala siluri
        per le armi, dal periscopio o dalla libreria del sonar per le classi. Spiegano l'uso (modi, impostazioni,
        limiti) come un manuale tecnico di bordo.
      - Si genera dai cataloghi che già esistono (classi, carico, siluri, acustica, emettitori, ASW aereo,
        periscopi, crediti) più un testo per voce, da scrivere dai dossier di ricerca. Così ogni dato ha una sola
        fonte e resta coerente con la simulazione.
      - *Avviata (25/10/2026):* il kit fa formato, generatore, testi e immagini; il principale fa la schermata del
        Museo e la consultazione in missione.
      - Pezzi:
        - formato delle voci e generatore: fatto (kit);
        - testi: fatti (kit, 193 voci);
        - immagini: fatte (kit). Sprite, modelli, sagome, oculari, effetti, foto, 36 cascate della firma, 45 carte
          d'epoca e 31 spaccati col sistema;
        - schermata del Museo: fatta (principale, `app/museo`), con le palette di oculare, cascata, carta e spaccato;
          i caratteri che il font non ha (§, lettere straniere, cirillico) sono sostituiti o traslitterati;
        - consultazione in missione: F1 dal menu e in missione, a tempo fermo.
15. **Carriera e achievement**: ranking personale con gradi reali delle due marine
    (US Navy: Ensign → Lieutenant (jg) → Lieutenant → Lieutenant Commander → Commander →
    Captain; VMF: Leitenant → Starshij leitenant → Kapitan-leitenant → Kapitan 3-go ranga →
    Kapitan 2-go ranga → Kapitan 1-go ranga), promozioni e decorazioni legate al rendimento
    (anche medaglie dell'epoca), fascicolo personale. Sistema ampio e progressivo di
    achievement: tattici, di stealth, tecnici, storici, di abilità crescente, legati a sfide
    nascoste e all'editor.
    - *Albo, fascicolo e scelta del comandante: fatti (25/10/2026, principale)*, vedi docs/12. Il registro dei codici
      delle rapide già giocati è in albo.toml.
    - *Achievement e albo d'onore (26/10/2026)*:
      - il disassembler ha fatto il nucleo: 41 achievement in data/motore/achievement.toml, più i nascosti; l'albo
        d'onore con le 20 migliori missioni e le 10 migliori carriere; gli 8 comandanti storici senza punteggi;
      - la principale li registra a fine missione (`chiudi_partita`), con il fascicolo di prima perché la carriera
        conti solo quel che la missione porta, e passa l'origine «libreria»;
      - fatte (principale, coi medaglioni del kit): la bacheca dei riconoscimenti (B dall'albo dei comandanti, sette
        schede, G comandante o giocatore) e la pagina «NUOVI RICONOSCIMENTI» dopo l'esito;
      - l'albo d'onore del kit (app/albo_onore: migliori missioni, migliori carriere, comandanti storici) si apre con O
        dall'albo dei comandanti.
    - *Cerimonia di decorazione* (richiesta dell'utente, 24/10/2026), sul modello delle schermate di premiazione di
      *F-19* e *F-117A*: il comandante in uniforme della sua marina (US Navy: blu di servizio con le «dolphins»
      d'oro; VMF: uniforme da ufficiale di marina sovietico), la medaglia in un riquadro e la motivazione («per il
      valore dimostrato...»); il nastrino si aggiunge poi alla barretta sul petto e al fascicolo. Decorazioni
      dell'epoca, da verificare con una ricerca: per la US Navy dalla Navy Achievement e Commendation Medal alla
      Bronze Star, Legion of Merit, Silver Star, Navy Cross e, eccezionale, la Medal of Honor, piu' le citazioni di
      reparto; per la VMF dalle medaglie «Per merito militare» agli ordini della Stella Rossa, «Per il servizio alla
      Patria», della Bandiera Rossa, di Lenin e a Eroe dell'Unione Sovietica. Si assegnano secondo la riuscita e la
      completezza della missione, il rischio corso, gli ordini e le ROE rispettati; le missioni rapide e dell'editor
      contano per la carriera solo se la missione lo dichiara (per non regalare medaglie con missioni facili).
16. **Debriefing e replay** (stile F-19 Stealth Fighter / F-117A):
    - *Debriefing riepilogativo*: rapporto di pattuglia in stile d'epoca (US Navy o VMF) con
      gli eventi più significativi selezionati automaticamente in base a un punteggio di
      interesse (prima scoperta, contro-scoperta, lanci, colpi, evasioni, attraversamenti del
      termoclino, momenti critici). Per ogni evento: ora, descrizione, istantanea reale della
      stazione catturata in quel momento e miniatura della mappa tattica.
    - *Replay completo*: vista dall'alto del teatro con timeline libera, velocità variabile,
      salto agli eventi e profilo verticale (profondità, termoclino, fondale). Doppia lettura:
      "verità" (posizioni reali di tutte le unità) e "quadro del giocatore" (contatti e
      soluzioni TMA che il giocatore conosceva davvero). In sovrapposizione: coni sonar,
      portate di scoperta, scie dei siluri e filo di guida.
    - *Replay completo: fatto (26/10/2026).*
      - La traccia è del disassembler (nucleo/traccia_replay, missioni/traccia_missione): un fotogramma ogni 5 s (2 s
        per aerei e armi in acqua) e uno a ogni evento, coi testi degli eventi; circa 1,3 MB per due ore.
      - La principale la registra in `Partita`, solo nell'Enhanced e senza cambiare la simulazione, coi dati dell'app
        (sonar, stazione, bersaglio, soluzione MATE). A fine missione la salva: in carriera nella cartella del
        comandante, col nome in `VoceMissione::replay`; fuori carriera fra le ultime 20 libere.
      - Lo schermo è di trailer-1 (app/replay): vista dall'alto col fondale, «verità» o «quadro del giocatore», scie,
        timeline con gli eventi, velocità da 1× a 64×, profilo verticale.
      - Si apre con P dal debriefing; Invio riprende la partita dall'istante scelto. Senza finestra:
        `--salva-traccia FILE` e `--schermata replay --replay FILE`.
      - Resta: riaprire i replay dal fascicolo.
    - *Riprendi da qui*: ripartire al comando da un istante qualsiasi del replay.
      - *Fatto (25/10/2026, principale), dagli eventi del debriefing.* R o il pulsante «R RIPRENDI» sull'evento
        corrente, poi Invio. La partita si ricomincia uguale (missione, seme, opzioni, carico della Sala siluri) e si
        rigioca con lo storico degli ordini fino all'istante dell'evento, con una schermata d'attesa. Poi comanda di
        nuovo il giocatore.
        - Regole della carriera: prima della ripresa la missione va agli atti così com'è; la ripresa non conta per la
          carriera (lo dice la carta del debriefing) e non ha esito né cerimonie.
        - Durante la ricostruzione la cascata del sonar resta ferma fino agli ultimi 10 minuti, perché è solo
          interfaccia ed è il costo maggiore; l'audio di bordo riparte dall'istante della ripresa.
        - Verificato senza finestra con `--ordini FILE --riprendi-da T`: la partita ripresa dopo l'ultimo ordine finisce
          con la stessa impronta e la stessa immagine di quella intera.
        - Limite: rigiocare costa. Misurato: 1600 s di Enhanced (SCENE1A) in 169 s con la cascata ferma fino agli
          ultimi 10 minuti, contro 314 s con la cascata sempre accesa.
          Il profilo dà il 60% del tempo allo spettro acustico (sonarista dell'equipaggio e cascata): va velocizzato
          (disassembler). Le fotografie periodiche dello stato restano da fare per il replay completo.
    - Basato sulla simulazione deterministica: si registrano stato iniziale e comandi, più
      fotografie periodiche dello stato per scorrere rapidamente la timeline e una traccia
      campionata delle posizioni, così il replay resta leggibile anche dopo aggiornamenti
      del motore. Gli stessi dati alimentano statistiche, heatmap e achievement.
16b. **Artiglieria navale contro il sottomarino** (richiesta dell'utente, 25/10/2026). Le navi nemiche che hanno
    cannoni (e i CIWS) sparano al battello avvistato in superficie o a quota periscopica, se dottrina e regole
    d'ingaggio lo prevedono, da entrambe le parti.
    - Effetti richiesti:
      - colonne d'acqua dei colpi viste al periscopio alle loro distanze;
      - i colpi sentiti all'idrofono;
      - i colpi sentiti dentro il battello, almeno in superficie;
      - danni ad alberi, periscopi e falsatorre, e allo scafo in superficie.
    - Il dossier di ricerca è pronto: `docs/ricerca/22_artiglieria_contro_sottomarini.md`. Conclusioni:
      - il cannone conta solo in guerra, contro il battello in superficie o il periscopio entro 2-5 km da una nave in
        allarme;
      - in pace e in crisi solo colpi d'avvertimento;
      - il battello immerso più di 3 m è al sicuro;
      - il Phalanx del 1989 non spara al periscopio, l'AK-630 sì.
    - Il piano è in `docs/16_artiglieria.md`, pezzi G0-G7 (dati, modulo di tiro, IA delle navi, danni, grafica, audio,
      app, taratura). Si comincia dopo i lavori in corso di runtime, kit e disassembler.
    - *Stato (26/10/2026).* Fatti G0-G2 (runtime), G3 (kit), G4 (kit), G5 (disassembler) e G6 (principale): colonne,
      vampe e illuminanti nella vista; colpi sentiti al sonar e a bordo, pieni in plancia; danni a teste, alberi, vela
      e scafo; oculare spento o incrinato; avvisi 420-429; croci delle cadute sulla carta. Dettagli in docs/16
      «Stato».
    - *Taratura G7 (runtime, 26/10/2026)*: testa del periscopio colpita 0,58 in 30 s a 3 km da una Perry in allarme
      (atteso 0,5); il 688 sorpreso in superficie a 8 km da un Kara si immerge sempre prima dei danni gravi (0,02);
      una battaglia fra navi si sente al sonar anche a 40 km. Nuova l'artiglieria fra navi
      (`artiglieria = { fra_navi = true }`); i danni alle navi sono del kit.
    - *Estensioni (runtime, 26/10/2026)*: le navi sparano anche ai sottomarini dell'IA esposti; con la difficoltà
      «minaccia percepita» sparano sulla piuma del periscopio entro 1 km di giorno e 300 m di notte; le acque
      territoriali dichiarate dalla missione (`acque_territoriali`) portano colpi e bombe d'avvertimento in pace, coi
      messaggi 453-455. La carriera pesa lo sconfinamento (disassembler): in pace e in crisi ogni entrata toglie punti
      agli ordini; farsi scoprire dentro è un incidente diplomatico, con penalità e richiamo o biasimo.
17. **Effetti e danni visivi realistici**, coerenti con il modello dei danni:
    - *Esplosioni varie e non ripetitive*, diverse per causa: siluro a contatto sulla murata, siluro
      a influenza sotto la chiglia (colonna d'acqua enorme, scafo sollevato e spezzato), missile
      antinave, colpi d'artiglieria, deflagrazione di depositi munizioni, incendio di combustibile.
      Variazione procedurale (forma, durata, detriti, fumo) e animazione a palette in stile VGA.
    - *Esplosioni subacquee che sollevano acqua*: bombe di profondità e razzi RBU (gruppi di
      colonne), siluri che esplodono in profondità o contro esche, mine, esplosioni contro il
      sottomarino viste dal periscopio; forma e altezza della colonna in funzione di carica e
      profondità (fisica in `docs/ricerca/02_acustica_oceanografia_dottrina.md` §9.4).
    - *Varianti danneggiate dei modelli*: ogni unità del kit 3D può perdere o deformare parti
      (alberi abbattuti, torri divelte, falle, bruciature), sbandare e appoppare o appruarsi
      secondo l'allagamento calcolato dal motore, bruciare e fumare, spezzarsi e affondare.
      Gli sprite si generano dallo stesso modello, quindi il danno visivo segue quello simulato.
    - *Effetto sulle meccaniche*: il danno a una parte toglie ciò che la parte fa (radar, lanciatori,
      sonar, propulsione, timone, elicottero), l'allagamento cambia assetto, velocità massima,
      manovrabilità e firma acustica (rumori di falla, macchinari danneggiati), e l'IA reagisce
      (ritirata, controllo danni, richiesta di soccorso, abbandono nave).
18. **Tutte le entità del gioco con il kit grafico**, non solo le navi da guerra: mercantili, cargo
    e petroliere (presenti nell'originale come MERCH, CARGO e TANKER), elicotteri (SH-60B, Ka-25,
    Ka-27), pattugliatori e bombardieri (P-3C, Il-38, Tu-142, Tu-22M Backfire), missili in volo
    (Harpoon, Tomahawk, SS-N-19…), alberi e periscopi di sottomarini in superficie o a quota
    periscopica. Per i velivoli le viste includono anche l'elevazione (visti dal basso, in virata).
19. **Firme acustiche ben differenziate per ogni classe** (anche civili e biologici): livelli a
    banda larga in funzione di velocità e profondità, toni caratteristici (rete elettrica 50/60 Hz,
    ingranaggi, pompe), frequenza delle pale, cavitazione, transitori (portelli, lanci, variazioni di
    giri). Su queste si basano la waterfall broadband e narrowband (LOFAR/DEMON) e il processo di
    rilevazione, classificazione e identificazione: dal semplice contatto alla classe, fino all'ipotesi
    con grado di confidenza ("probabile Victor III"), con errori possibili e contatti ambigui.
20. **Towed array realistico** (specifica in `docs/07_towed_array.md`): filatura e recupero con
    verricello rumoroso, schiera che segue la scia e resta curva dopo le virate, profondità che
    dipende dalla velocità (sotto lo strato termico, o contro il fondo nei teatri di piattaforma),
    ambiguità destra/sinistra da risolvere con la manovra, rumore di flusso, rischi (fondale, reti
    da pesca, elica, fatica), danni e taglio d'emergenza; stesse regole per l'IA.
21. **Testi sempre leggibili per intero.** Nell'originale i messaggi dell'equipaggio scorrono da destra
    a sinistra nella barra informazioni (la barra spaziatrice li accelera, PREV/NEXT in sala radio li
    rileggono). In italiano le frasi sono più lunghe: nessun messaggio deve andare perso o troncato.
    Coda dei messaggi con priorità (gli avvisi critici scavalcano gli altri), velocità di scorrimento
    adattata alla lunghezza, pausa e ripresa, registro completo consultabile da ogni stazione, e
    alternativa a due righe senza scorrimento per chi la preferisce. Lo stesso vale per pulsanti, menu
    ed etichette: verifica automatica che ogni testo tradotto entri nello spazio disponibile.
    - *Fatto nell'Enhanced (26/10/2026, principale):*
      - la coda dei messaggi (app/coda_messaggi): nessuno si perde; ognuno resta 2,5-7 s secondo la lunghezza; gli
        avvisi critici (col «!») passano davanti e interrompono un messaggio normale già letto;
      - i messaggi lunghi vanno a capo su due righe invece di tagliarsi;
      - il registro dei messaggi da ogni stazione: Ctrl+L, con l'ora di missione e i critici in giallo;
      - il classico resta con l'ultimo messaggio, come l'originale.
    - Verifica automatica (26/10/2026): la prova «testi: ogni messaggio di bordo sta nella barra» controlla tutta la
      tabella dei messaggi, coi segnaposto ai valori più lunghi e con la formula sovietica: tutti stanno su due righe.
      Le stringhe dell'interfaccia restano verificate a mano in `docs/traduzione/verifica_spazi.md`.
22. **Sala radio e comunicazioni realistiche**, con la telescrivente (testo che si stampa riga per riga,
    da tradurre insieme ai briefing):
    - ricezione in profondità solo in VLF/ELF (antenna a filo galleggiante o boa trainata, messaggi
      brevi e lenti); HF, UHF e satellite solo a quota periscopica con l'antenna alzata, quindi con
      esposizione a radar ed ESM nemici;
    - trasmissioni dalla terraferma a orari prestabiliti (da ascoltare o perdere); formato dei messaggi
      come nei briefing originali (data-ora, FM/TO/SUBJ, classifica), precedenza (FLASH, IMMEDIATE,
      PRIORITY, ROUTINE) e autenticazione;
    - ordini che cambiano durante la missione (nuovi compiti, regole d'ingaggio, aggiornamenti di
      intelligence); rapporti di contatto e di situazione da inviare;
    - trasmettere espone alla radiogoniometria nemica; equivalenti sovietici per l'Alfa;
    - rilettura dei messaggi e degli ordini, come nell'originale.
23. **ESM ripensato.** Oltre al livello di segnale dell'originale, un vero ricevitore di allarme radar
    sull'albero ESM:
    - elenco delle emissioni intercettate con rilevamento, banda e identificazione da una banca dati
      (Top Sail, Head Net, Top Plate, SPS-49, i radar dei P-3 e degli Il-38…);
    - andamento del segnale (in avvicinamento o in allontanamento) e stima del rischio che chi emette
      veda il nostro albero; radar che si accendono e si spengono secondo l'EMCON dell'IA;
    - avviso precoce degli aerei antisommergibile.
24. **Elaborazione acustica approfondita** (insieme al punto 19): filtri passa-alto, passa-banda e
    bande selezionabili, presentazioni a banda larga e a banda stretta, analisi LOFAR e DEMON,
    analizzatore di firma ripensato (l'ANALYZE originale) con confronto tra le righe spettrali
    osservate e la banca dati delle classi; ascolto in cuffia del contatto attraverso il filtro
    scelto, con i suoni generati dalle firme.
25. **Missili e contromisure con impiego realistico** (ricerca in `docs/ricerca/05_missili_e_contromisure.md`):
    - *Missili lanciati dal sottomarino*: Harpoon in capsula dai tubi lanciasiluri (espulsione,
      risalita, emersione, accensione del razzo, crociera radente), Tomahawk antinave e d'attacco al
      suolo (dai tubi o, sui 688 Flight II, dai 12 pozzi verticali); dotazione mista a scapito dei
      siluri; dati di tiro necessari (rilevamento, distanza, designazione esterna oltre l'orizzonte),
      modi della testa di ricerca, salve; rischi di contro-scoperta (transitorio del lancio,
      emersione visibile, missile al radar). Missili sovietici equivalenti per l'IA e per l'Alfa.
    - *Difese delle navi bersaglio* (Phalanx, missili antiaerei, inganni elettronici e chaff, e gli
      equivalenti sovietici) per esiti realistici delle salve. *Fatto nell'Enhanced (05/11/2026, difese-navi):* per i
      missili del modello, canali di guida e Phalanx come regole all'impatto, scalate con l'allerta. Missili antiaerei,
      chaff e disturbo restano riassunti nel byte `difesa`, senza oggetti né effetti; niente AK-630, per decisione
      dell'utente (manuale D3 §2.6 e §7).
    - *Contromisure acustiche*: esche fisse e semoventi (ADC Mk2, MOSS; MG-34, MG-74), generatori di
      bolle, esche rimorchiate delle navi (Nixie) contro i nostri siluri; tempo di vita, quota,
      efficacia contro teste passive, attive e siluri filoguidati; tattiche combinate con le
      manovre d'evasione.
    - *Aspetto*: missili ed esche visibili ridisegnati con il kit (emersione della capsula, scia del
      razzo, missile in volo al periscopio), anche nel Museo.
    - *Dal dossier 05*: l'Harpoon risale in capsula senza motore (10–30 s non controllabili, capsula
      visibile, scia di fumo); il Tomahawk da tubo accende il razzo sott'acqua; il Tomahawk antinave
      cerca un'area e può colpire la nave sbagliata, anche neutrale; il Tomahawk d'attacco al suolo usa
      missioni pianificate e un'area di lancio; l'esca semovente MOSS era dei sottomarini lanciamissili,
      non dei 688 (che usano l'ADC Mk2); l'Alfa non ha missili antisommergibile convenzionali, mentre
      Victor III e Akula hanno l'SS-N-16; esiti delle salve tarati sui dati storici (circa 0,44 di
      probabilità di colpire un bersaglio difeso). Missione 10: area di lancio da spostare nello
      Skagerrak o nel Kattegat.
    - *Stazione di lancio dei missili (richiesta dell'utente, 28/10/2026).* Oggi nell'Enhanced il missile si lancia come
      nell'originale, su un contatto a traccia solida. Si vuole l'impiego storico: RBL/BOL, accensione del seeker,
      dati oltre l'orizzonte dal broadcast, missioni TLAM con area di lancio e navigazione allineata, ricerca del
      missile che può agganciare un neutrale. Ricerca mirata e proposta affidate a trailer-1
      (`docs/ricerca/27_missili_impiego.md`, `docs/21_stazione_missili.md`), poi l'approvazione dell'utente; il modello
      al runtime, l'interfaccia alla principale. Il classico non cambia.
    - *Esche semoventi (26/10/2026).*
      - Il runtime ha fatto l'MG-74 per l'IA (i nucleari sovietici con un siluro in coda) e la MOSS Mk 70: Pk del Mk 46
        0,33 con l'esca e 0,53 senza.
      - Decisione dell'utente per il giocatore:
        - 688 nel modo 1989: niente MOSS, solo gli ADC Mk 2;
        - 688 nel modo moderno: la MOSS Mk 70, marcata «what-if»;
        - Alfa in entrambi i modi: la MG-74 dal tubo, al posto di un siluro;
        - missioni dell'editor: stesse regole, decise dall'epoca e dal modo.
      - Fatto il 26/10/2026:
        - carico e tubi (disassembler, M2): famiglia `esca_mobile`, `SchedaArma::moderno`, Arma::esca_mobile;
        - lancio dal tubo con `lancia_esca_mobile` (runtime);
        - sagome ed enciclopedia (kit);
        - editor e missioni dell'Alfa della libreria (trailer-1);
        - modo moderno alla Sala siluri, «ESCA MOB.», «(WHAT-IF)» e la nota della MOSS (principale).
      - Nelle missioni originali dell'Enhanced l'Alfa parte con 2 MG-74 in rastrelliera. Il 688 nel modo moderno ha 2
        MOSS al deposito, ma non nel carico predefinito, che resta identico nei due modi. Le missioni con deposito le
        hanno solo se le dichiarano. Il classico non cambia.
26. **Fondali, scansione e navigazione** (specifica in `docs/08_fondali_e_navigazione.md`): il
    "Contour Imaging Display" dell'originale (che non aveva un equivalente reale sui 688 del 1989: il
    dossier 06 lo conferma) ripensato come quadro "carta + misure" con vista 3D costruita solo dai ritorni
    reali di ecoscandaglio e AN/BQS-15, con costo in contro-scoperta; due carte d'epoca, NATO e sovietica,
    degradate in modo diverso dal fondale vero; incertezza di posizione (navigazione inerziale, Transit,
    Loran-C, Omega, stelle o riconoscimento del fondale; GPS assente nel 1989); tattiche vicino al fondo,
    rischi di collisione, operazioni sotto il ghiaccio (emersione solo per il 688I).
    - *Incertezza di posizione (26/10/2026): affidata.* Il disassembler fa il modulo del nucleo con i dati:
      - deriva dell'inerziale per classe;
      - punti nave con precisione, condizioni ed esposizione.
      Poi il runtime lo aggancia alla missione, e la principale fa la carta: posizione stimata, ellisse, comandi dei
      punti nave. Lo stesso modello vale per le unità dell'IA (docs/18).
      Il nucleo è fatto il 26/10 (`nucleo/navigazione`, `data/motore/navigazione.toml`):
      - Kalman a 4 stati;
      - 7 sistemi, 11 punti nave, 7 teatri;
      - un giorno senza fix: DMINS 1,47 nm, WSN-3 0,50, Sož 2,93; dopo un Transit 0,11 nm.
      L'aggancio nella missione è del runtime (27/10). La carta e il menu sono della principale (27/10):
      - opzione «N NAVIGAZIONE: PERFETTA / REALISTICA» nel menu, spenta di norma, anche nel codice partita e con
        `--navigazione`;
      - la carta sta dove il giocatore crede di essere; battello, contatti, scie, linee di rilevamento, cadute e mine
        spostati dell'errore, con l'ellisse del 95% e «POSIZIONE ±x NM» nell'intestazione;
      - Ctrl+O alla carta apre il menu dei punti nave, con precisione, durata, emissione o il motivo per cui non si
        può fare; la cifra lo ordina.
      L'IA con l'errore di posizione è fatta il 29/10 (runtime: B, C, D, A1, A2 ed E, docs/18).
27. **Comparto sonoro realistico e generato dal modello acustico.** I 6 campioni PCM a 4 bit
    dell'originale (ping, eliche, siluro, esplosione, balena) vengono sostituiti da suoni **sintetizzati
    a partire dalla firma acustica** di ogni sorgente (punto 19), così ciò che si sente in cuffia e ciò che
    appare sulla waterfall vengono dalla stessa fisica:
    - *propulsione*: frequenza delle pale = giri dell'asse × numero di pale, con armoniche; cavitazione
      che cresce con la velocità e cala con la profondità; riduttori e turbine, motori diesel (frequenza
      di accensione), pompe del reattore, rete elettrica a 50 o 60 Hz;
    - *armi*: siluri (motori e eliche o pompe a getto, sibilo ad alta velocità, impulsi della testa
      attiva), lancio (espulsione dal tubo), missili (capsula, razzo), esche;
    - *esplosioni*: onda d'urto e pulsazioni della bolla, diverse per carica e profondità; scafi che si
      deformano e si spezzano; rumori di allagamento;
    - *aerei ed elicotteri*: rotori e motori che filtrano attraverso la superficie, sonar a immersione,
      boe che cadono in acqua;
    - *biologici e ambiente*: canti di balena, schiocchi di gamberi, delfini, pioggia, ghiaccio, traffico;
    - *variazioni individuali*: ogni unità ha piccole differenze (toni spostati di qualche percento,
      condizione dei macchinari, pale leggermente diverse), generate da un seme per unità. Due Victor III
      non suonano identici, e un operatore attento può riconoscere un singolo battello già incontrato;
    - *propagazione*: perdita delle alte frequenze con la distanza, effetto Doppler, interferenza con il
      fondo e la superficie, mascheramento dal rumore ambiente;
    - resa in stile Sound Blaster dell'epoca come opzione, accanto a una resa a qualità piena per
      l'ascolto analitico.
    - *Campionamenti reali (richiesta dell'utente, 25/10/2026).* Dove esistono registrazioni vere con una licenza che
      permette di usarle, si parte da quelle. Alla sintesi dal modello si mescolano «tessiture» registrate per
      biologici, ambiente e navi civili; per sottomarini e navi militari le registrazioni pubbliche sono rare.
      - Ogni ascolto cambia con i fattori reali: velocità, regime e stato dei macchinari (pompe del reattore lente o
        veloci, turbogeneratori, ausiliari), aspetto, profondità e cavitazione, danni, distanza e propagazione, mare.
        A questo si aggiunge la variazione individuale di ogni unità, legata al suo seme.
      - Prima serve un dossier di ricerca (`docs/ricerca/18_audio_reale.md`) su:
        - archivi e licenze (NOAA, Watkins, DOSITS, ShipsEar, DeepShip, registrazioni declassificate);
        - come mescolare campioni e modello senza che l'ascolto e la cascata si contraddicano;
        - quale motore audio usare. Oggi il gioco non ha nessuna uscita audio.
      - La libreria di riferimento consultabile alla stazione c'è già (S5b, tasto B): righe della classe sovrapposte
        al LOFAR, confronto per rapporti, annotazioni sulle cascate, 8 slot di confronto.
      - *Suoni a bordo senza cuffia (richiesta dell'utente, 25/10/2026).* Tutto ciò che è abbastanza vicino si sente
        anche dalle altre stazioni, ovattato dallo scafo, come nella realtà: eliche di navi e imbarcazioni che passano
        sopra, ping attivi, tonfi delle boe, siluri vicini, esplosioni. I mezzi aerei, con campioni realistici, solo
        quando si sentirebbero davvero. Per ogni stazione un ambiente sonoro alimentato dal modello acustico, con il
        percorso attraverso lo scafo. È nel dossier 18.
      - *Dossier fatto (25/10/2026):* `docs/ricerca/18_audio_reale.md`. Scelte dell'utente:
        - solo campioni con licenza CC0, pubblico dominio o CC BY, distribuiti dentro il gioco con i crediti;
        - ordine dei lavori: R1 uscita audio (SDL3, mixer deterministico), R2 cuffia del sonar in tempo reale,
          R3 ambiente di bordo per stazione; poi campioni, mescolamento, variazioni e transitori;
        - gli aerei a bordo non si sentono: solo al sonar, per pochi secondi.
        - Non esistono registrazioni libere di sottomarini nucleari della Guerra Fredda: le unità militari vengono
          tutte dal modello.
      - Da aggiungere: il **quaderno del sonarista**, cioè annotazioni, slot e registrazioni salvati su file e
        ripresi in una partita successiva, per riconoscere un battello già incontrato. Poi le altre funzioni del
        software delle postazioni vere che la ricerca troverà (S5d è in corso).
28. **Coerenza storica con l'anno dell'ambientazione (1989)**: varianti, revisioni,
    dotazioni e capacità in servizio allora. Gli anacronismi dell'originale (ad esempio
    Mk50, Spearfish, Sea Lance) vanno segnalati e gestiti esplicitamente.
    Deciso il 23/10/2026: il cacciatorpediniere **Forrest Sherman** (tipo 0x00 dell'originale, missione 1
    TORPEX '89; tutte radiate nel 1982–83) è sostituito dalla **Spruance** (DD-963), il cacciatorpediniere
    antisommergibile della US Navy nel 1989 (dossier 01).
29. **Danni, affondamento ed esplosioni** (progetto in `docs/10_danni_affondamento_esplosioni.md`, dati in
    `docs/ricerca/07_danni_affondamento_esplosioni.md`), sviluppo dei punti 11 e 17:
    - una sola fisica (idrostatica per stazioni, allagamento, stabilità, trave di scafo, incendi) decide meccanica,
      immagine e suono;
    - i modi di affondare emergono dalla simulazione: in assetto, di prua o di poppa, capovolgimento, nave spezzata
      in due tronconi, esplosione dei depositi, nave che brucia e resta a galla;
    - le navi danneggiate si disegnano con il modello 3D del kit nel motore;
    - le esplosioni subacquee sono scalate dalla fisica: slick, cupola, pennacchi, ricaduta, schiuma.
30. **Posarsi sul fondo** (proposta del 23/10/2026, con i comandi del giocatore): il comandante può ordinare di
    appoggiare il battello sul fondo dove la profondità e il tipo di fondo lo permettono, per nascondersi.
    - Il sonar attivo nemico fatica a separare l'eco dal riverbero del fondo, soprattutto su fondali rocciosi, e
      un bersaglio fermo non ha effetto Doppler.
    - Senza macchine (a batteria per i diesel-elettrici) il rumore irradiato è minimo.
    - Il MAD continua a rilevarlo.
    - Rischi: fango che trattiene lo scafo, rocce che danneggiano cupola del sonar e timoni, prese a mare che
      aspirano sedimento (per questo i sottomarini nucleari lo evitano), schiera trainata da recuperare prima.
    - Serve la mappa del tipo di fondo (Fase 0). Il motore ha già il contatto dolce: sotto 1 m/s nessun danno.
31. **Interazioni fra unità dell'IA, con la simmetria giocatore/IA** (richiesta dell'utente, 26/10/2026). Di regola tutto
    quello che il giocatore può fare e vedere, lo può fare anche l'IA, secondo i mezzi e il contesto, verso di noi e fra
    unità IA (nemico contro amico, amico con amico). Il contesto comprende teatro, pace/crisi/guerra, dottrine e ROE di
    ogni parte, unità e sensori, meteo e livello d'allerta («paranoia») secondo chi c'è nel teatro. Il giocatore deve
    poter assistere a queste interazioni: un sottomarino alleato scoperto, un gruppo ASW nemico che lo caccia, un
    alleato che attacca per conto suo. Il classico resta identico all'originale.
    - Fase 0 (avviata il 26/10/2026): l'inventario delle asimmetrie nel codice (runtime, con l'acustica) e il dossier
      di ricerca 24 sulle dottrine d'interazione (gestione dello spazio d'acqua, coordinamento ASW, reazioni a un
      alleato scoperto o colpito, scala pace/crisi/guerra, accordo INCSEA del 1972, pedinamento, prontezza): fatti.
    - *Piano approvato dall'utente (26/10/2026)*, in `docs/18_interazioni_ia.md`, in cinque fasi:
      1. correzioni (neutrali veri: fatti da trailer-1);
      2. percezione simmetrica;
      3. allerta e rete dei contatti;
      4. comportamento;
      5. spettatore e taratura.
      Le fasi 1-4 sono del runtime, col disassembler per acustica e armi; le scene di prova di trailer-1; lo
      spettatore nell'app è della principale.
32. **Sandbox: il «vetrino» dell'IA** (proposta dell'utente, 26/10/2026, «per quando avremo spazio»). Si sceglie una
    missione e si guarda svolgersi senza nessun intervento umano. La missione può essere:
    - originale;
    - della libreria;
    - rapida;
    - dell'editor.
    Anche il battello del giocatore è comandato dall'IA, con le stesse regole di tutti (punto 31). Serve a vedere come
    si concretizzano dottrine, ROE, allerta, meteo e sensori in tante situazioni diverse. Pezzi previsti:
    - *Regolazioni*:
      - situazione (pace, crisi, guerra) e ROE per parte;
      - dottrina e livello dell'IA per parte;
      - allerta di partenza, meteo, epoca dei sensori;
      - carico, seme;
      - compressione del tempo senza i limiti di bordo.
    - *Viste*:
      - la verità, con tutto;
      - il quadro di una parte, con quello che sa;
      - il quadro di una singola unità: contatti, ellissi, stime;
      - il registro degli eventi dell'IA (fase 5 del punto 31).
    - *Dopo*:
      - replay con la verità e con il quadro di ogni parte;
      - statistiche tecniche: chi ha scoperto chi, a che distanza e con quale sensore; tempi di reazione; armi
        lanciate e colpi; scambio di perdite; emissioni ed esposizione.
    - *Serie*: la stessa scena su più semi senza grafica, come `scene_interazioni --semi N`, con il riassunto
      statistico (medie e dispersioni).
    Condizioni:
    - le fasi 2-4 del punto 31, senza le quali il vetrino mostrerebbe poco;
    - un comandante IA per il battello che oggi è del giocatore.
    *Piano approvato (27/10/2026) in `docs/19_sandbox.md`, tre fasi V1-V3. V1 e V2 chiuse il 27/10/2026, V3
    (serie, vetrini pronti, manuale, tabella per le originali) il 28/10/2026: la sandbox è completa.*
    Il classico non cambia: la sandbox è solo dell'Enhanced. Chi la fa:
    - runtime: il comandante IA e la partita senza giocatore;
    - principale: menu, viste e regolazioni nell'app;
    - trailer-1: serie e riassunti, partendo dalle scene S1-S7;
    - disassembler: statistiche tecniche;
    - kit: il capitolo del manuale.

33. **Rifinitura estetica di tutta l'interfaccia** (richiesta dell'utente, 27/10/2026, «con calma»). Menu, schermate
    attorno alla missione e stazioni devono essere più belli e curati, sempre nel tema e nello stile VGA dell'epoca.
    Ispirazione dai simulatori DOS degli anni '90:
    - sottomarini, U-boot e navi;
    - aerei ed elicotteri;
    - soprattutto quelli di Sierra/Dynamix ed Electronic Arts.
    Si parte dal dossier 15 (`docs/ricerca/15_schermate_simulatori.md`), che descrive già le schermate di quei titoli,
    allargato all'estetica: cornici, materiali, illuminazione, tipografia, transizioni, pulsanti. Poi:
    - una proposta per schermata, con bozze da approvare;
    - i pezzi grafici del kit;
    - l'aggancio della principale.
    Il classico non cambia.
    Stato al 28/10/2026: scelta della missione e regolazioni della sandbox in variante A (lamiera); replay e
    statistiche della sandbox in variante C (`grafica/quadro`: consolle «SALA ANALISI», carta grande del replay con
    legenda e lettura in riquadri, fascia del tempo, piede comune coi tasti anche al clic). Il replay dal vivo della
    sandbox tiene per ora la sua disposizione.
    Gruppo 3 agganciato: briefing (ordini sulla telescrivente sopra la carta, livelli a spie, scheda nei vetri) e
    debriefing (carta incassata, rapporto dattiloscritto con la minicartolina e il timbro dell'esito, quattro tasti) in
    variante A coi documenti di B; serie della sandbox in variante C con la sintesi in fondo alla pagina 2.

34. **Libretto di riconoscimento e telemetro** (richiesta dell'utente, 29/10/2026; piano in
    `docs/25_libretto_riconoscimento.md`). Al periscopio dell'Enhanced, accanto alla vista:
    - le tavole delle classi in stile ONI, viste da quattro angoli, coi dati noti nel 1989 (altezza d'albero, armi,
      sensori, segni distintivi);
    - l'identificazione visiva del giocatore come ordine (ROE e registro);
    - il telemetro sull'altezza della classe scelta, con l'aiuto automatico solo nella difficoltà più facile;
    - l'angolo sulla prora (AOB) alla TMA;
    - l'identificazione visiva dell'IA, simmetrica.

    Stato (31/10/2026): fatto tutto il piano. Il filtro delle tavole c'è dal 29/10; il telemetro a reticolo
    dell'Alfa (H, [ ] a decimi di divisione), l'album sovietico dell'Alfa e le etichette che separano i sottomarini
    sono del kit, 31/10 (docs/25 §6).
    Tasti: Ctrl+Z libretto, Tab sfoglia, Invio identifica, [ ] altezza a mano, Z X AOB, Spazio AOB alla TMA.

35. **Luci di navigazione e di fonda al periscopio** (scheda di trailer-1, approvata dall'utente il 29/10/2026; piano
    in `docs/27_luci_navigazione.md`). Solo nell'Enhanced: i fanali COLREG per classe e stato (in moto, alla fonda, in
    pesca, a luci spente), nella vista coi settori e la foschia, la tavola «luci» del libretto e l'AOB dalle luci, la
    vedetta dell'IA con la portata dei fanali (simmetria), la luce gialla del 688 in superficie con l'ordine per
    spegnerla.
36. **Il «vetrino di Dio»** (idea dell'utente, 31/10/2026; progetto di importanza maggiore). Nel vetrino (punto 32)
    lo spettatore interviene dall'esterno, come nei «simulatori di Dio» e di città degli anni '90, e studia le
    conseguenze in modo quasi scientifico: inserire o togliere unità, cambiare ROE, allerta e ordini, guastare sensori
    e macchine, meteo e mare, disturbo delle comunicazioni, falsi ordini radio, falsi contatti, false flag (un'azione
    attribuita a un'altra parte), incidenti provocati; esperimenti ripetuti con e senza l'intervento e confronto
    statistico (le serie del V3). Opzioni molte e avanzate. Prima un documento di progetto da approvare, poi fasi.
37. **Video d'apertura all'avvio** (idea dell'utente, 31/10/2026): girato in tempo reale col motore, saltabile con un
    tasto, senza riquadri promozionali; un mini-racconto simbolico dal 688 e dall'Alfa che cresce fino al culmine.
    Scaletta in `docs/29_video_apertura.md` (trailer-1), scelte dell'utente fatte il 31/10; prima versione e giro di rifinitura del 06/11/2026, da provare in finestra.
38. **Musica ed effetti mancanti** (31/10/2026): stile d'epoca, FM AdLib/Sound Blaster con ymfm e PCM; campioni
    Roland MT-32 solo se l'utente li fornisce. Prima l'inventario di quello che manca.
39. **Manuale e compendio sempre al passo** (richiesta ripetuta dell'utente, 31/10/2026): ogni aspetto e meccanica,
    enciclopedia e riferimento tecnico, immagini (schermate del motore, foto vere, tavole), cenni storici, tattiche.
    Revisione capitolo per capitolo contro codice e dati; il kit cura.

    Messi da parte dall'utente il 31/10/2026: selettore di lingua italiano/inglese, preparazione al rilascio
    (Windows, pacchetto Linux), repository (quando sarà il momento: privato, non pubblico). Armi nucleari: escluse.

## Decisioni tecniche (22/10/2026)

- **Motore**: C++20 + SDL3, CMake, test con doctest. Linux come piattaforma principale, Windows come seconda.
  Nucleo di simulazione come libreria separata dalla grafica, a passo fisso e deterministico.
- **Vista del fondale** (23/10/2026): solo il quadro realistico "carta + misure" (`docs/08`); nessuna opzione con
  il display di contorno del 1989.
- **Navi danneggiate** (23/10/2026): sprite generati al volo dal modello 3D del kit, con lo stesso aspetto degli
  sprite precalcolati; le navi intatte restano con gli sprite di oggi (`docs/10_danni_affondamento_esplosioni.md`
  §5.1).
- **Strumenti e pipeline dati**: Python (asset originali, batimetria, profili del suono, conversione missioni).
- **Audio**: emulazione OPL2/OPL3 con *ymfm* (BSD) per l'AdLib; PCM a 8 bit stile Sound Blaster.
- **Dati**: database unità in TOML con fonte e affidabilità; missioni e trigger in Lua 5.4;
  carriera, statistiche e achievement in SQLite.
- **Grafica**: un'unica tela **640×480 a 256 colori indicizzati** (stile SVGA VESA 101h, pixel quadrati)
  per tutte le schermate. Una palette per scena; gli effetti di palette dell'originale restano
  (luce d'emergenza, giorno e notte). Output a schermo ingrandito a multipli interi, con opzione
  "sharp bilinear" ed effetto CRT facoltativo.
- **Asset originali a 320×200**: portati a 640×480 senza introdurre colori nuovi. Si raddoppiano
  (nitido o Scale2x/EPX) e si stirano in verticale 5:6 per recuperare l'aspetto 4:3 del monitor CRT.
  Niente upscaler IA: inventano dettagli fuori stile e fuori palette.
  Metodo predefinito per classe di asset (scelto nella pipeline, con override "tutto nitido"
  nelle opzioni): **Scale2x** per disegni (centrali, interfaccia, titolo, schermate finali) e
  sagome su sfondo (navi, elicotteri, missili); **nitido** per le foto a tutto campo
  (ritratti dell'equipaggio, fumo), dove Scale2x genera artefatti "a vermicello" sulla grana.
- **Asset nuovi** (stazioni realistiche, Museo, unità assenti nell'originale come Oscar, Akula,
  Typhoon): disegnati nativi a 640×480. Le foto delle navi vengono digitalizzate "alla 1989"
  (pochi livelli di grigio, retinatura) partendo da immagini di pubblico dominio (US Navy/DoD).
- **Licenze e crediti** (25/10/2026): `tools/pipeline/crediti.py` raccoglie autore, licenza e obblighi di ogni asset
  in `data/motore/crediti.toml` e `docs/CREDITI.md`. Tre conseguenze per una distribuzione:
  - **gli asset dell'originale** (font, immagini MAIN e TITLE, palette, testi di bordo, missioni SCENE convertite)
    non sono ridistribuibili: il gioco li estrarrà dalla copia del giocatore, oppure servono i diritti;
  - lo sprite del Be-12 è CC BY-SA 2.0 (foto di Alan Wilson): si distribuisce con l'attribuzione e la stessa
    licenza;
  - EMODnet (fondali e carte) va attribuito nei crediti del gioco, e le licenze MIT e BSD delle librerie vanno
    incluse con i binari.

## Fasi

### Fase 0: reverse engineering e documentazione (in corso)
- [x] Inventario dei file; formati MLB, PAK/PAL, ENC, LZW/LZA, audio `.T`
- [x] Decompressore EXEPACK; Ghidra headless su eseguibili decompressi
- [x] Estrazione di 140 immagini, 7 mappe batimetriche e 6 campioni audio
- [x] Individuata la VM degli scenari (118 opcode, tabella dei gestori a `DS:6505`)
- [x] Mappatura degli opcode `SCENE*.TOK` e formato `SCENE*.LZA`, schede di tutte le scene (`docs/04_vm_missioni.md`;
      restano alcune ipotesi sui campi, §6)
- [x] Formato `.CLS` (layout delle stazioni, `tools/re/cls_layout.py`)
- [x] `HELICOP.ENC`: non è un'animazione ma la base dati della protezione anticopia (89 domande, sala radio)
- [x] Tabella prestazioni di unità e armi (`docs/originale/prestazioni.md`, `docs/04` §8); modello di
      scoperta dei sensori (`docs/04` §2.2)
- [x] Livelli batimetrici reali e corrispondenza mappa↔teatro (heightmap e georeferenziazione)
- [x] Suoni in partita e firme di ANALYZE (`docs/originale/suoni.md`)
- [x] Musica dei titoli nelle tre versioni originali, AdLib, Tandy e Covox (`extracted/musica/`)
- [x] Dossier piattaforme, armi e sensori 1989 (`docs/ricerca/01_piattaforme_e_armi.md`)
- [x] Dossier acustica, oceanografia, dottrina e danni (`docs/ricerca/02_acustica_oceanografia_dottrina.md`)
- [x] Dossier teatri, fondali e storia (`docs/ricerca/03_teatri_fondali_storia.md`)
- [x] Dossier danni, affondamento ed esplosioni (`docs/ricerca/07_danni_affondamento_esplosioni.md`, 23/10/2026):
      esplosioni subacquee e fenomeni in superficie, falle, rottura della trave, incendi, petroliere, 21 casi
      storici con i tempi, implosioni, comando via filo (non documentato per i siluri del 1989)
- [x] Associare `SURFA..G` ai 7 teatri (certa dai briefing, verificata su ETOPO 2022)
- [x] Pipeline Python: ETOPO 2022 e WOA23 per i 7 teatri, profili mensili della velocità del suono
- [x] Testo dei briefing originali estratto (`docs/originale/briefing_EN.txt`)
- [x] Georeferenziazione delle mappe originali (affidabile per Gibilterra, plausibile per Otranto e Kunashir)
- [x] Heightmap a 16 bit dei 7 teatri (EMODnet per i 6 europei, ETOPO per Kunashir; celle da 200 m, proiezione locale)
- [ ] Raster della classe di fondo (EMODnet Geology, Dutkiewicz), linee di costa (GSHHG), climatologia meteo ERA5
- [x] Metodo grafico delle unità: kit 3D + proiezione multipla da foto calibrate, verificato sulla Perry (`docs/06_kit_navi.md`)
- [x] Kit applicato a 11 classi di superficie, sprite ogni 5° (`docs/06_kit_navi.md`)
- [x] Kit su Spruance (al posto del Forrest Sherman) e navi civili: mercantile, cargo, petroliera (15 classi in tutto)
- [x] Kit su elicotteri (SH-60B, Ka-25), aerei antisommergibile (P-3C, Il-38, Tu-142), primi sottomarini in
      superficie (688, Victor III, Kilo) e missili (Tomahawk, Harpoon, P-700, BQM-74): 27 classi in tutto
- [x] Altri sottomarini in superficie: Alfa, Akula I, Oscar II, Charlie II, Typhoon, Foxtrot, Sturgeon, Trafalgar
      (35 classi in tutto nel motore)
- [ ] Armi e unità senza sprite richieste dalle missioni (siluri Mk 48, Type 53, Spearfish, Sea Lance, Type 40;
      biologico): dati per il motore, non sprite (sott'acqua non si vedono al periscopio)
- [x] Correggere nelle missioni gli errori geografici del manuale 1989: missione 7 (gli stretti di
      Kunashir collegano il Mare di Okhotsk al Pacifico, non il Mar del Giappone) e missione 10
      (gli stretti danesi non sono percorribili in immersione: Drogden 7–9 m, Darss ~18 m)
      - *Fatto (26/10/2026), solo nell'Enhanced:* il classico non cambia. Le note del traduttore sono nel manuale
        italiano. Nella radio R3: in SCENE7A/7B/7C il messaggio ROTTA (il Typhoon esce dal Mare di Okhotsk per lo Stretto
        di Ekaterina); in SCENE0A USCITA e COLPITO mandano il battello nelle acque danesi a sud del Drogden (baia di
        Køge), con le soglie da passare in superficie. L'area di lancio nello Skagerrak o nel Kattegat resta fuori: il
        file della missione è anche il classico e il teatro Baltico finisce prima del Kattegat; servirebbe un
        meccanismo di varianti per l'Enhanced.
- [ ] Colmare le lacune: rumore irradiato assoluto, NIE CIA 11-15-82D/84D e Coté (lettura manuale),
      profili del suono reali da WOA/GDEM al posto di quelli archetipici, verifica dei valori *n.v.*

### Fase 1: fondamenta del nuovo motore (avviata il 23/10/2026, `engine/README.md`)
- [x] Progetto CMake (nucleo, grafica, piattaforma, app, prove doctest), compilabile senza sudo
- [x] Tela 640×480 a 256 colori indicizzati con palette, ingrandita a multipli interi senza filtri (SDL3)
- [x] Ciclo di simulazione a passo fisso (1/30 s), separato dal disegno; mondo deterministico
- [x] Database delle classi in TOML con fonte e affidabilità; sprite delle navi con ancore; font originale
      con le lettere accentate
- [x] Scena di prova al periscopio: scala per distanza e ingrandimento, orizzonte con curvatura e rifrazione
- [x] Palette per scena e luce del giorno dal Sole; Scale2x e 5:6 per gli asset originali; moto interpolato
- [ ] Emulazione OPL2/OPL3 (ymfm) per la musica AdLib e PCM 8 bit stile Sound Blaster
  - *Musica dei titoli (27/10/2026, principale):* le tre versioni dell'originale (AdLib resa con ymfm, Tandy, Covox) si
    suonano nel menu come all'avvio del 1989, da data/motore/musica (tools/pipeline/musica.py); M nella scelta della
    missione cambia versione o la spegne. Resta l'emulazione dal vivo, che serve solo se un giorno la musica cambia.
  - *Suoni del classico in partita: fatti (27/10/2026).* Il classico era muto. Ora i 6 campioni dell'originale e i bip
    partono agli stessi eventi dell'eseguibile del 1989 (nucleo/suoni_classici del disassembler, docs/originale/
    suoni.md), in coda sul canale dell'interfaccia senza fermare il gioco; Ctrl+N toglie i campioni, Ctrl+Q spegne
    tutto. `--audio-wav FILE` li registra senza finestra.
- [x] Missioni in Lua 5.4 (ambiente isolato) con il runtime del livello classico: le 22 scene originali girano
      (`docs/09_schema_missioni.md`, `engine/README.md`)
- [x] Fondale dei teatri nel motore: quote limitate dal fondo, incagli con danni ed esito 1, pilota dell'IA
      (rotte A* per acque navigabili, franco, freno); 110 partite di verifica senza urti dell'IA (23/10/2026)
- [x] Navi danneggiate disegnate dal modello 3D: opera viva nel kit, modelli con tessitura cotta per 26 classi,
      sprite generati al volo nel motore, identici a quelli precalcolati a nave intatta (23/10/2026, fase D2 di
      `docs/10_danni_affondamento_esplosioni.md`)
- [x] Fisica dei danni delle navi di superficie: idrostatica per stazioni a qualunque assetto, allagamento con
      superfici libere, trave di scafo e tronconi, incendi e depositi, controllo danni; tarata sui casi storici del
      dossier 07 (23/10/2026, fase D3)
- [x] Effetti visivi di danni ed esplosioni: esplosioni subacquee per carica e profondità (anello scuro, cupola,
      pennacchi, corona di sfiato, ricaduta, chiazza, fango), esplosioni nell'aria, fiamme e colonne di fumo
      piegate dal vento, acqua attorno alla nave e resti dell'affondamento (24/10/2026, fase D4); poi falle nere,
      fuliggine, alberi che cadono, nave sollevata e inarcata dalla bolla, onde e foschia
- [x] Danni nelle missioni (24/10/2026, fase D5):
  - regola d'impatto delle armi ricavata dall'EXE (prima i siluri non colpivano mai);
  - relitti realistici con gli script fedeli (livello classico);
  - regola di simulazione con reazioni dell'IA;
  - messaggi del motore.
- [x] Danni del nostro sottomarino (24/10/2026, fase D6, `docs/10_danni_affondamento_esplosioni.md` §10): urto dal
      fattore d'urto, dodici sistemi graduati con riparazioni, falle, paratie, esaurimento, SCRAM, rottura dello scafo
      resistente; sottomarini dell'IA colpiti con esiti e segni in superficie; stazione Stato con quadro danni, scossa e
      luci che tremolano (solo con la regola di simulazione)
- [x] Cadenza della scoperta come l'originale: un solo osservatore per passo (correzione del §3.1 di
      `docs/originale/sensori_e_navigazione.md`, 24/10/2026); le non combattenti non osservano, `TYPE` dalle
      missioni convertite; esiti e colpi rimisurati sulle 22 scene
- [x] Comandi e stazioni del giocatore (progetto in `docs/11_comandi_e_stazioni.md`, fasi C1–C6, tutte fatte; C1 fatta il
      24/10/2026: ordini con storico e replay, dispatcher dell'originale, gestori parametrici; C2 fatta il
      24/10/2026: posto di comando, tasti degli ordini rapidi, pannello Manovra, controlli dell'originale sul
      giocatore e accostate rallentate; C3 fatta il 24/10/2026: stazione Armi con tubi, scorte, lancio, guida a
      filo e limite delle armi in acqua come l'originale, pannello Armi; C4 fatta il 24/10/2026: stazione Sonar,
      guasti dei sistemi, contatti del giocatore, barra del bersaglio, carta con punti di rotta e proiezione, TARGET,
      quadro di stato, ESM); debriefing
- [x] Rotte e ratei delle missioni convertite in bussola (fatto il 24/10/2026, engine/README.md): l'originale usa angoli matematici
      (`docs/originale/numeri_eseguibile.md` §3.3–§4); convertitore e 22 missioni da correggere

### Fase 2: simulazione
- Dinamica del sottomarino (spinta, resistenza, timone e piani, assetto, cavitazione); riceve peso netto e
  momento d'assetto dal battello danneggiato della fase D6 (`docs/10_danni_affondamento_esplosioni.md` §10)
- Oceano: batimetria reale (GEBCO/EMODnet) ricampionata, SVP stagionali (World Ocean Atlas)
- Acustica: perdita di trasmissione, rumore ambiente (Wenz), equazione del sonar, TMA
- Armi: siluri filoguidati con logica di ricerca reale, missili, contromisure
- Danni e riparazioni
- **Collisioni fra le unita' in acqua** (richiesta dell'utente, 24/10/2026): sottomarino contro sottomarino, nave
  contro sottomarino, nave contro nave, soprattutto accidentali durante i pedinamenti ravvicinati della Guerra Fredda
  (i casi documentati sono molti), con le dimensioni vere degli scafi e delle vele, i quasi-incidenti, i danni nei
  modelli esistenti, le manovre «Crazy Ivan» dell'IA, gli avvisi di prossimita' e le conseguenze (danni, rumore,
  scoperta, esito e carriera, incidente politico in tempo di pace). Ricerca in `docs/ricerca/16_collisioni.md`.

### Fase 3: stazioni e interfaccia
- Centrale, sonar (waterfall), periscopio, navigazione, armi, radio/ESM, controllo danni
- Stile grafico originale; font "sovietico" in caratteri latini per l'Alfa
- **Interfaccia intera coerente con gli interni** (richiesta dell'utente, 25/10/2026, da fare dopo le funzioni):
  - stazioni, pannelli, barre e menu abbelliti nello stile delle postazioni vere di ciascun battello (requisito 12);
  - i menu ispirati ai simulatori degli anni '90 già indicati (dossier 15: *F-19*, *F-117A*, *Falcon*...).
  - *Avviata (25/10/2026), dossier 19.*
    - Il kit ha fatto:
      - 12 font disegnati da zero, letti dal nuovo formato FNT2;
      - i temi di colore nelle voci 16-47 della palette;
      - gli strumenti di consolle e i pannelli di governo e armi per 688 e Alfa (`grafica/sale`).
    - Il principale li ha collegati alle stazioni Manovra (F4) e Armi (F7) dell'Enhanced: intestazione, pannello in 88
      righe, messaggi sotto, clic sui comandi, luce rossa di notte sul 688. `--consolle no` riporta ai pannelli di
      prima; il classico non cambia.
    - Collegati poi (principale, 25/10/2026, sera):
      - la Centrale (F9) dei due battelli come vista, con i posti cliccabili che aprono le stazioni;
      - Ritm (Stato dell'Alfa);
      - la cornice del periscopio alla stazione Periscopio;
      - il Sozh attorno alla carta dell'Alfa;
      - il nuovo comando stazione_periscopio (F5), che mancava: la stazione si raggiungeva solo da riga di comando.
    - Collegati il 26/10/2026 (principale; disassembler per le cascate in un'area e per Okean):
      - Radio (F2) dell'Enhanced: il pannello radio/ESM del kit. La telescrivente stampa gli ultimi messaggi ricevuti e
        quello in ricezione. L'ESM (WLR-8 o Bukhta) mostra la panoramica per banda e le intercettazioni solo con
        l'albero su; la spia THREAT/UGROZA si accende col rischio probabile. I nove pulsanti della sala radio restano
        sugli stessi tasti 1-9.
      - Sonar (F8) dell'Enhanced: la sala sonar del 688 a luce blu con le cascate dentro la cornice; sull'Alfa Okean
        con lo schermo tondo di Enisej e il LOFAR a destra.
      - `--consolle no` riporta a prima; il classico e le impronte non cambiano (cambia solo l'immagine della stazione
        Sonar dell'Enhanced).
    - *Fatte (26/10/2026):*
      - le funzioni dell'Alfa che i pannelli già mostrano: Boksit a programma, Tan, ПОКС, Akkord a tre bersagli e
        l'avaria che blocca le armi. In corso dal 26/10/2026:
        - Akkord e Boksit (ordini e stazioni) li ha fatti il disassembler; il pilota a tappe e le casse (Tan, SFOGA,
          COMPENSA) li fa il runtime; i pulsanti mancanti il kit;
        - la principale ha collegato i pannelli (modi, tappe, slot, avaria), i comandi per nome dai pulti e i tasti
          Ctrl+B, U, T, N, G, K, J, A, 5, 6, 7.
- **Carta di navigazione più bella** (richiesta dell'utente, 24/10/2026, da fare quando si affina la grafica): stile
  VGA/SVGA in ricordo del classico e del seguito (*688(I) Hunter/Killer*), ma ben dettagliata e colorata: batimetria a
  più livelli con sfumature, coste e isole definite, simboli e griglia curati, contatti, ellissi e zone leggibili
  - *Avviata (25/10/2026).* Il kit ha fatto `grafica/carta_vga`: fondale al pixel con la retinatura, rilievo
    ombreggiato su fondali e terra (indici 48-79), coste lisce con la fascia di risacca, isole piccole sempre visibili,
    isobate principali coi numeri, cache che si trasla.
  - Il principale l'ha collegata nell'Enhanced, negli stili vga e nautica: stazione Navigazione, briefing e
    debriefing, coi numeri delle isobate in incisione (NATO) e stampino (URSS). La cartolina del debriefing è passata a
    16-47.
  - *Fatti (26-27/10/2026):* la «mobilia» (cornice coi minuti, rosa con la declinazione, scala, cartiglio, scandagli)
    anche nel debriefing, e i toponimi da Natural Earth (permesso dell'utente) con le aggiunte d'epoca dai dossier.
- **Quadro danni più bello** (richiesta dell'utente, 24/10/2026): lo spaccato del battello del giocatore (688, Alfa e
  le altre classi giocabili) come nel classico, ma molto più dettagliato e cromaticamente profondo, sempre in stile
  VGA/SVGA e in tributo all'originale: scafo e vela col loro profilo vero, ponti e compartimenti (siluri, comando,
  alloggi, reattore, macchine), macchinari riconoscibili, e sopra gli stati del danno (acqua che sale, fuoco, fumo,
  compartimenti persi, falle) già simulati dai danni dell'Enhanced
  - *Fatto:* lo spaccato del kit è agganciato alla stazione Stato dell'Enhanced (app/interfaccia.cpp), con acqua,
    fuoco, fumo e squadre.

### Fase 4: IA, campagna ed editor
- IA a livelli (dottrina → missione → tattica → manovra), stati di allerta, gruppi ASW
- Le 10 missioni originali reinterpretate, più l'editor di missione
- **Riferimenti per briefing, carico, debriefing, esito e onorificenze** (indicati dall'utente, 24/10/2026): le
  schermate dei simulatori di volo DOS e dei primi anni '90 di MicroProse e dintorni, da studiare per adattarle e
  reimmaginarle: *F-15 Strike Eagle* I-III, *F-19 Stealth Fighter*, *F-117A Nighthawk*, *Falcon 4.0* (e *Allied
  Force*), *Lock On: Modern Air Combat*, *Chuck Yeager's Air Combat*, *Tornado*, *TFX*, *X-Com: Interceptor*,
  *B-17 Flying Fortress*, *Eagles*, *F-14 Tomcat* (1989), *A-10 Tank Killer*. (Non sono tutti MicroProse: per esempio
  *Chuck Yeager* e' di Electronic Arts, *Tornado* di Digital Integration, *TFX* di Digital Image Design, *Lock On*
  di Eagle Dynamics, *A-10 Tank Killer* di Dynamix: vanno verificati uno per uno nella ricerca.)
- **Sala briefing prima della missione** (richiesta dell'utente, 24/10/2026), ispirata alle schermate di briefing dei
  simulatori DOS di fine anni '80 e primi '90 come *F-19 Stealth Fighter* e *F-117A Nighthawk* (MicroProse), adattate e
  reimmaginate per un sottomarino:
  - intestazione «CLASSIFIED» / «SEGRETO» e titolo («Intelligence Briefing»), carta del teatro nella grafica VGA della
    stazione di navigazione (fasce di profondità, terra a quote, reticolo), menu dei livelli a destra;
  - livelli accendibili uno per uno o tutti («Seleziona tutto» / «Pulisci tutto»): ordini e obiettivi della missione,
    area operativa e zone dello spazio d'acqua (R2), contatti d'intelligence con le ellissi, basi e radar costieri,
    portate delle armi nemiche (missili antinave, aerei ASW, campi di sorveglianza SOSUS), rotta pianificata con i
    punti di rotta a lettere come nel plotting di *F-19*, eventi speciali;
  - testo degli ordini (la serie 0 della sala radio) e voci «Scegli le armi» (dotazione dei tubi) e «Rifiuta la missione»;
  - generata dai dati della missione, così vale per le 10 missioni originali, per quelle generate in modo rapido e per
    quelle dell'editor; per le missioni scritte a mano un blocco facoltativo `briefing = { ... }` aggiunge livelli,
    testi e segni;
  - riusa il disegno della carta (N1), i simboli e le zone (N2-N4) e i dati della radio (R1-R2).
- **Sala debriefing dopo la missione** (richiesta dell'utente, 24/10/2026; completa il punto 16 dei requisiti), sullo
  stesso modello dei debriefing di *F-19* e *F-117A*, adattata e reimmaginata:
  - intestazione «CLASSIFIED» e «Mission Debriefing», carta VGA con le tracce del battello, dei contatti e delle armi
    (scie della missione, la verità sovrapponibile a quello che sapeva l'equipaggio);
  - **eventi uno alla volta** («Evento successivo», «Rivedi la missione», «Esci»): ora (DTG), descrizione (contatto
    classificato, lancio, colpo a segno, nave affondata, emersione d'emergenza del nemico, siluro evitato, messaggio
    radio perso, infrazione alle ROE...), punteggio parziale e finale;
  - le **«minicartoline»**: un riquadro illustrato sulla carta, accanto al punto dell'evento, con la scena saliente.
    Siccome le partite sono deterministiche e ripetibili, la cartolina si può ottenere rigiocando il registro degli
    ordini fino all'istante dell'evento e disegnando la vista del periscopio (modelli 3D, effetti del kit:
    esplosioni, affondamento, emersione violenta E1, lancio di missili E2), oppure, per gli eventi che non si vedono
    (contatto sonar, ping), un'icona nello stile dell'epoca (cascata con la riga, schermo del sonar, telescrivente);
  - valutazione: ordini eseguiti, ROE trasmesse e copiate (R1, `debriefing()`), serie perse, contatti classificati e
    con che anticipo, esposizione (DF, ESM, avvistamenti), danni subiti; il rapporto dell'originale (codice d'esito)
    resta per il classico;
  - generata dal registro della partita, quindi vale per le missioni originali, rapide e dell'editor.
- **Sala siluri: il carico prima della missione** (richiesta dell'utente, 24/10/2026), sul modello della schermata di
  carico di *F-117A*, reimmaginata come pianta del locale siluri:
  - a sinistra le armi disponibili al deposito con le quantita' (le decide la missione: originale, rapida o
    dell'editor); al centro la pianta del locale siluri con le rastrelliere e i 4 tubi del 688 (6 dell'Alfa, piu' i
    12 pozzi verticali dei 688 Flight II per i Tomahawk), dove si sceglie anche cosa e' gia' nei tubi alla partenza;
    in basso la scheda dell'arma scelta con la sagoma e i dati (guida, velocita', portata, testata), come la scheda
    del Sidewinder; pulsanti «Alla sala briefing» e «In mare»;
  - armi del 1989: Mk 48 (e ADCAP dal 1988), Harpoon e Tomahawk incapsulati, mine Mk 67, esche; per l'Alfa siluri da
    533 mm, missili antisommergibile e mine; piu' le boe SLOT della radio e le contromisure;
  - **cosa cambia davvero** (a differenza degli aerei, il peso non conta: una quarantina di tonnellate di armi su circa
    6900 di dislocamento, compensate dalle casse d'assetto): il numero di colpi per tipo, perche' i posti sono gli
    stessi per siluri, missili incapsulati e mine; cosa e' pronto nei tubi al primo contatto; i tempi e il rumore della
    ricarica (un transitorio che puo' tradire il battello); la portata delle missioni (mine per un minamento,
    Tomahawk per un attacco a terra); il giudizio nel debriefing se si e' partiti col carico sbagliato.
  - *Sagome di armi e mine (richiesta dell'utente, 25/10/2026).* Con le mine implementate, il munizionamento si deve
    vedere anche durante la missione nelle stazioni.
    - Ogni arma di carico.toml ha la sua sagoma VGA, riconoscibile:
      - Mk 67 SLMM sul corpo del Mk 37;
      - Mk 60 CAPTOR con la capsula e l'ancora;
      - PMR-1 con l'ancora;
      - siluri, capsule di Harpoon e Tomahawk, esche.
    - Le sagome compaiono in tre posti:
      - nella sala siluri, sulla scheda e sulle rastrelliere;
      - nel pannello Armi, con la sezione del tubo: l'arma che scorre dalla rastrelliera durante la ricarica, il tubo
        allagato, la porta aperta, la mina pronta alla posa;
      - nello spaccato dei danni, con le rastrelliere del carico vero.
    - Chi fa cosa: il kit fa il modulo delle sagome e lo spaccato; il principale lo aggancia nell'app; il
      disassemblatore completa le dimensioni del PMR-1 in carico.toml.

### Fase 5: contenuti di contorno
- Manuale originale tradotto, manuale Enhanced, Museo, statistiche con heatmap
- Carriera con gradi, decorazioni (con la cerimonia di decorazione del punto 15) e fascicolo personale; catalogo di achievement
- Debriefing riepilogativo con istantanee e replay tattico con "verità" e "quadro del giocatore"

## Struttura del repository

```
original-game/      file originali (sola lettura)
extracted/          asset estratti (png, wav, bathymetry, raw, raw_lzw)
re/                 reverse engineering: eseguibili decompressi, progetto Ghidra, decompilati
tools/              Ghidra, JDK, venv Python, dipendenze locali (tools/deps) e script tools/re/*.py
engine/             motore C++20/SDL3 (nucleo, grafica, piattaforma, app, test)
data/               dati preparati dalla pipeline (teatri, motore)
docs/               analisi tecnica, roadmap, ricerca/, originale/ (manuali in testo)
```
