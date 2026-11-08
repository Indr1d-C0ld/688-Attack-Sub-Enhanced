-- LIB_S23_ACQUA_A_DUE_GRADI: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_S23_ACQUA_A_DUE_GRADI",
  titolo = { it = "Acqua a due gradi", en = "Two-Degree Water" },
  lato = "urss",
  teatro = "barents",
  posa = { metodo = "rapida", centro = { lat = 70.15, lon = 35 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-04-18", ora_utc = 9,
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  deposito = { type53 = 18, esca = 4, esca_mobile = 2 },
  tipo_missione = "antisom",
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "KR3B_0631" },
    esca = { gestore = "KR3B_0700" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "KR3B_0763",
    telegrafo = "KR3B_0775",
    quota = "KR3B_077a",
    timone = "KR3B_0781",
    pilota_automatico_off = "KR3B_07dd",
    periscopio = "KR3B_07e5",
    carica_tubo_1 = "KR3B_0789",
    carica_tubo_2 = "KR3B_0792",
    carica_tubo_3 = "KR3B_079b",
    carica_tubo_4 = "KR3B_07a4",
    carica_tubo_5 = "KR3B_07ad",
    carica_tubo_6 = "KR3B_07b6",
    carica_tubo_7 = "KR3B_07bf",
    schiera_cala = "KR3B_07e9",
    schiera_recupera = "KR3B_07f9",
    schiera_persa = "KR3B_0805",
    designa_bersaglio = "KR3B_080f",
    posizione = "KR3B_0814",
    antenna = "KR3B_07ef",
    guida_e0 = "KR3B_0736",
    guida_e1 = "KR3B_073b",
    guida_e2 = "KR3B_0745",
  },
  carriera = false,   -- missione fuori carriera
  aereo_asw = {
    { unita = 4, missione = "caccia", situazione = "pace", bersagli = { 3 }, equipaggio = "medio",
      zona = { x = 0, y = 128306.7, raggio_m = 8000 } },
    { unita = 5, missione = "caccia", situazione = "pace", bersagli = { 3 }, equipaggio = "medio",
      zona = { x = 0, y = 128306.7, raggio_m = 8000 } },
    { unita = 7, missione = "caccia", situazione = "pace", bersagli = { 3 }, equipaggio = "medio",
      zona = { x = 0, y = 128306.7, raggio_m = 10000 } },
  },
  obiettivi = {
    { id = "arrivato", tipo = "variabile", minimo = 1, indice = 302, almeno = 1,
      testo = { it = "Arrivare entro 2 km dal battello in 90 minuti", en = "Reach 2 km from the boat within 90 minutes" } },
    { id = "posizione", tipo = "variabile", minimo = 1, indice = 303, almeno = 1,
      testo = { it = "Dare la posizione esatta al peschereccio (rapporto di situazione entro 5 km)", en = "Give the trawler the exact position (situation report within 5 km)" } },
    { id = "vicino", tipo = "variabile", minimo = 1, primario = false, indice = 304, almeno = 1,
      testo = { it = "Essere entro 5 km quando arriva la nave", en = "Be within 5 km when the ship arrives" } },
    { id = "salvati", tipo = "variabile", minimo = 25, primario = false, indice = 305, almeno = 25,
      testo = { it = "Gli uomini salvati (almeno 25)", en = "The men saved (at least 25)" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: SOCCORSO", "18 aprile 1989, ore 0900Z. Teatro: Mare di Barents. Pace. Mare 5, nubi basse, acqua a due gradi.", "Un battello sperimentale ha un incendio a poppa ed e' emerso con l'aria d'emergenza. Il reattore e' fermo, il fuoco continua. Siete il battello piu' vicino, a 60 km a sud.", "Le navi sono lontane: un peschereccio-madre e' stato deviato, ma ha una posizione sbagliata. Arrivate sul posto, avvicinatevi entro 5 km e mandate un rapporto di situazione: la posizione esatta gli fara' guadagnare un'ora.", "Due Il-38 decollano da Kola. Un aereo norvegese e' gia' in volo sulla zona.", "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 4 ore e 30.", "Nota storica: ispirata alla K-278 Komsomolets, che il 7 aprile 1989 emerse in fiamme a sud-ovest dell'Isola dell'Orso e affondo' circa quattro ore dopo. Le zattere lanciate dagli aerei furono inutilizzabili; il peschereccio Aleksej Chlobystov arrivo' 81 minuti dopo l'affondamento. Si salvarono 27 uomini su 69. Un Orion norvegese guardava; il comando non chiese aiuto alla Norvegia. Qui battello e uomini sono di fantasia; l'Alfa ha il ruolo dei soccorritori." },
    en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RESCUE", "18 April 1989, 0900Z. Theatre: Barents Sea. Peace. Sea state 5, low cloud, water at two degrees.", "An experimental boat has a fire aft and has surfaced on emergency air. Her reactor is shut down, the fire goes on. You are the nearest boat, 60 km to the south.", "The ships are far away: a factory trawler has been diverted, but she has a wrong position. Get on scene, close within 5 km and send a situation report: the exact position will gain her an hour.", "Two Il-38s are taking off from Kola. A Norwegian aircraft is already over the area.", "Rules of engagement: peacetime. Weapons hold. Time available: 4 hours 30.", "Historical note: inspired by K-278 Komsomolets, which on 7 April 1989 surfaced on fire south-west of Bear Island and sank about four hours later. The rafts dropped by the aircraft were unusable; the trawler Aleksey Khlobystov arrived 81 minutes after the sinking. 27 men of 69 survived. A Norwegian Orion was watching; the command did not ask Norway for help. Here the boat and the men are fictional; the Alfa plays the rescuers." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [40] = { it = "Il peschereccio e' arrivato prima che il battello andasse giu': l'equipaggio e' passato a bordo. Tutti e quaranta.", en = "The factory trawler arrived before the boat went down: the crew stepped across. All forty." },
    [44] = { it = "Il peschereccio li ha raccolti dopo pochi minuti in acqua: trentacinque uomini sono vivi.", en = "The factory trawler picked them up after a few minutes in the water: thirty-five men are alive." },
    [45] = { it = "Il peschereccio li ha raccolti dopo piu' di venti minuti in acqua a due gradi: venticinque uomini sono vivi.", en = "The factory trawler picked them up after more than twenty minutes in two-degree water: twenty-five men are alive." },
    [46] = { it = "Il peschereccio li ha raccolti dopo quasi un'ora: quindici uomini sono vivi.", en = "The factory trawler picked them up after almost an hour: fifteen men are alive." },
    [47] = { it = "Il peschereccio e' arrivato troppo tardi per quasi tutti: cinque uomini sono vivi.", en = "The factory trawler arrived too late for almost all of them: five men are alive." },
    [50] = { it = "La nave norvegese e' arrivato prima che il battello andasse giu': l'equipaggio e' passato a bordo. Tutti e quaranta.", en = "The Norwegian ship arrived before the boat went down: the crew stepped across. All forty." },
    [51] = { it = "La nave norvegese li ha raccolti dopo pochi minuti in acqua: trentacinque uomini sono vivi.", en = "The Norwegian ship picked them up after a few minutes in the water: thirty-five men are alive." },
    [52] = { it = "La nave norvegese li ha raccolti dopo piu' di venti minuti in acqua a due gradi: venticinque uomini sono vivi.", en = "The Norwegian ship picked them up after more than twenty minutes in two-degree water: twenty-five men are alive." },
    [53] = { it = "La nave norvegese li ha raccolti dopo quasi un'ora: quindici uomini sono vivi.", en = "The Norwegian ship picked them up after almost an hour: fifteen men are alive." },
    [54] = { it = "La nave norvegese e' arrivato troppo tardi per quasi tutti: cinque uomini sono vivi.", en = "The Norwegian ship arrived too late for almost all of them: five men are alive." },
  },
  radio = {
    ordini = { da = "Comando della Flotta del Nord", a = "K-463 (PROGETTO 705)", oggetto = "SOCCORSO", classifica = "SEGRETO", data = { giorno = 18, mese = 4, anno = 1989, ora_utc = 9 } },
    stato_iniziale = { roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, neutrali = { 8, } },
    { id = "RICEVUTO", su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "POSIZIONE", classifica = "SEGRETO",
      testo = { it = "POSIZIONE RICEVUTA. IL PESCHERECCIO-MADRE ACCOSTA E VA A TUTTA. RESTATE SUL POSTO.", en = "POSITION RECEIVED. THE FACTORY TRAWLER IS TURNING AND COMING AT FULL SPEED. STAY ON SCENE." } },
    { id = "A_MARE", su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "EQUIPAGGIO IN ACQUA", classifica = "SEGRETO",
      testo = { it = "IL BATTELLO E' AFFONDATO. L'EQUIPAGGIO E' IN ACQUA. OGNI MINUTO CONTA.", en = "THE BOAT HAS SUNK. THE CREW IS IN THE WATER. EVERY MINUTE COUNTS." } },
    { id = "IL38", su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "POSIZIONE DALL'AEREO", classifica = "SEGRETO",
      testo = { it = "L'IL-38 HA DATO LA POSIZIONE. IL PESCHERECCIO-MADRE ACCOSTA.", en = "THE IL-38 HAS GIVEN THE POSITION. THE FACTORY TRAWLER IS TURNING." } },
    { id = "NORVEGIA", su_richiesta = true, precedenza = "immediate", da = "Guardia costiera norvegese", oggetto = "OFFERTA DI ASSISTENZA", classifica = "NON CLASSIFICATO", autentico = false,
      testo = { it = "SULLA FREQUENZA DI SOCCORSO: UNA NAVE DELLA GUARDIA COSTIERA NORVEGESE PUO' ESSERE SUL POSTO IN UN'ORA. SE LA VOLETE, CHIEDETELA.", en = "ON THE DISTRESS FREQUENCY: A NORWEGIAN COAST GUARD SHIP CAN BE ON SCENE IN AN HOUR. IF YOU WANT IT, ASK FOR IT." } },
    { id = "ACCETTATO", su_richiesta = true, precedenza = "immediate", da = "Guardia costiera norvegese", oggetto = "IN ARRIVO", classifica = "NON CLASSIFICATO", autentico = false,
      testo = { it = "RICEVUTO. LA NAVE E' IN ARRIVO A TUTTA FORZA.", en = "ROGER. THE SHIP IS COMING AT FULL SPEED." } },
  },
  meteo = { fisso = { vento_m_s = 15, vento_da_gradi = 240, stato_mare = 5, visibilita_m = 4000, nubi_ottavi = 8, } },
  messaggi = {
    [1] = { it = "Sonar: dal battello aria e acqua, tanta. Si sta appoppando. Sentiamo le voci sull'idrofono.", en = "Sonar: air and water from the boat, a lot of it. She is going down by the stern. We can hear voices on the hydrophone." },
    [2] = { it = "Gli Il-38 lanciano le zattere. Il vento le rovescia prima che qualcuno le raggiunga.", en = "The Il-38s drop life rafts. The wind flips them before anyone can reach them." },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_S23_ACQUA_A_DUE_GRADI", titolo = { it = "Acqua a due gradi", en = "Two-Degree Water" }, autore = "Libreria di Melchior 688",
    teatro = "barents", alfa = true, centro = { lat = 70.150000000000006, lon = 35 },
    data = { giorno = 18, mese = 4, anno = 1989 }, ora_utc = 9, meteo_realistico = false,
    situazione = "pace", paranoia = 1, difficolta = 3, durata_s = 16200, libreria = 2,
    carico = { dotazione = { { "type53", 18 }, { "esca", 4 }, }, deposito = { { "type53", 18 }, { "esca", 4 }, { "esca_mobile", 2 }, }, tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", parte = "urss", ruolo = "", flag = 17, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = 0, y = 66940.591851288555 }, quota = 100, rotta = 0, velocita = 10, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "akula", categoria = "sottomarino", parte = "urss", ruolo = "battello sperimentale in avaria, incendio a poppa (al posto del Mike)", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 0, y = 128306.67640905583 }, quota = 0, rotta = 300, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { }, stato = "avaria" },
      { id = 4, classe = "il38", categoria = "aereo", parte = "urss", ruolo = "Il-38 della Flotta del Nord", flag = 19, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 1.1021821192326178e-11, y = 38306.676409055828 }, quota = 0, rotta = 0, velocita = 300, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "il38_1",
        parametri = { } },
      { id = 5, classe = "il38", categoria = "aereo", parte = "urss", ruolo = "Il-38 della Flotta del Nord", flag = 19, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 1.1021821192326178e-11, y = 38306.676409055828 }, quota = 0, rotta = 0, velocita = 300, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "il38_2",
        parametri = { } },
      { id = 6, classe = "bmrt", categoria = "nave", parte = "urss", ruolo = "peschereccio-madre deviato sull'emergenza", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 47631.397208144132, y = 100806.67640905584 }, quota = 0, rotta = 320, velocita = 8, punti = { { x = -5130.3021498850294, y = 142402.06572084446 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 7, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "Orion norvegese che osserva", flag = 75, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -6000, y = 128306.67640905583 }, quota = 0, rotta = 0, velocita = 220, punti = { { x = 0, y = 134306.67640905583 }, { x = 5196.152422706632, y = 131306.67640905583 }, { x = 5196.152422706632, y = 125306.67640905583 }, { x = 7.347880794884119e-13, y = 122306.67640905583 }, { x = -5196.1524227066302, y = 125306.67640905583 }, { x = -5196.152422706632, y = 131306.67640905583 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 8, classe = "oslo", categoria = "nave", parte = "neutrale", ruolo = "nave della guardia costiera norvegese (al posto della Nordkapp)", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -22000, y = 128306.67640905583 }, quota = 0, rotta = 90, velocita = 18, punti = { { x = 0, y = 128306.67640905583 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "accetta",
        parametri = { } },
    },
    obiettivi = {
      { id = "arrivato", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Arrivare entro 2 km dal battello in 90 minuti", en = "Reach 2 km from the boat within 90 minutes" }, indice = 2 },
      { id = "posizione", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Dare la posizione esatta al peschereccio (rapporto di situazione entro 5 km)", en = "Give the trawler the exact position (situation report within 5 km)" }, indice = 3 },
      { id = "vicino", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = false, colpire = true, testo = { it = "Essere entro 5 km quando arriva la nave", en = "Be within 5 km when the ship arrives" }, indice = 4 },
      { id = "salvati", tipo = "variabile", unita = {  }, minimo = 25, entro_s = -1, primario = false, colpire = true, testo = { it = "Gli uomini salvati (almeno 25)", en = "The men saved (at least 25)" }, indice = 5 },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 4, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = { 3 }, zona = { x = 0, y = 128306.67640905583 }, raggio = 8000 },
      { unita = 5, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = { 3 }, zona = { x = 0, y = 128306.67640905583 }, raggio = 8000 },
      { unita = 7, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = { 3 }, zona = { x = 0, y = 128306.67640905583 }, raggio = 10000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: SOCCORSO", "18 aprile 1989, ore 0900Z. Teatro: Mare di Barents. Pace. Mare 5, nubi basse, acqua a due gradi.", "Un battello sperimentale ha un incendio a poppa ed e' emerso con l'aria d'emergenza. Il reattore e' fermo, il fuoco continua. Siete il battello piu' vicino, a 60 km a sud.", "Le navi sono lontane: un peschereccio-madre e' stato deviato, ma ha una posizione sbagliata. Arrivate sul posto, avvicinatevi entro 5 km e mandate un rapporto di situazione: la posizione esatta gli fara' guadagnare un'ora.", "Due Il-38 decollano da Kola. Un aereo norvegese e' gia' in volo sulla zona.", "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 4 ore e 30.", "Nota storica: ispirata alla K-278 Komsomolets, che il 7 aprile 1989 emerse in fiamme a sud-ovest dell'Isola dell'Orso e affondo' circa quattro ore dopo. Le zattere lanciate dagli aerei furono inutilizzabili; il peschereccio Aleksej Chlobystov arrivo' 81 minuti dopo l'affondamento. Si salvarono 27 uomini su 69. Un Orion norvegese guardava; il comando non chiese aiuto alla Norvegia. Qui battello e uomini sono di fantasia; l'Alfa ha il ruolo dei soccorritori.", }, en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RESCUE", "18 April 1989, 0900Z. Theatre: Barents Sea. Peace. Sea state 5, low cloud, water at two degrees.", "An experimental boat has a fire aft and has surfaced on emergency air. Her reactor is shut down, the fire goes on. You are the nearest boat, 60 km to the south.", "The ships are far away: a factory trawler has been diverted, but she has a wrong position. Get on scene, close within 5 km and send a situation report: the exact position will gain her an hour.", "Two Il-38s are taking off from Kola. A Norwegian aircraft is already over the area.", "Rules of engagement: peacetime. Weapons hold. Time available: 4 hours 30.", "Historical note: inspired by K-278 Komsomolets, which on 7 April 1989 surfaced on fire south-west of Bear Island and sank about four hours later. The rafts dropped by the aircraft were unusable; the trawler Aleksey Khlobystov arrived 81 minutes after the sinking. 27 men of 69 survived. A Norwegian Orion was watching; the command did not ask Norway for help. Here the boat and the men are fictional; the Alfa plays the rescuers.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "luogo", nome = { it = "il battello in avaria", en = "the stricken boat" }, cerchio = { x = 0, y = 128306.67640905583, r = 600 } },
    },
    regole = {
      { id = "fermo", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "resta_su", una_volta = false,
        quando = {
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "quota", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "il38_1", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 1800, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "il38_2", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 3600, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tardi", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 5400, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "arrivato", una_volta = true,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 2000, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 9, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "posizione", una_volta = true,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 5000, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "giocatore_ha_trasmesso", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", trasmissione = "situazione" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "vai_a", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 128306.67640905583 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 14 },
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "RICEVUTO", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "si_appoppa", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 10200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: dal battello aria e acqua, tanta. Si sta appoppando. Sentiamo le voci sull'idrofono.", en = "Sonar: air and water from the boat, a lot of it. She is going down by the stern. We can hear voices on the hydrophone." } },
        } },
      { id = "a_mare", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 10800, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 10, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "togli", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "A_MARE", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "zattere", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 11100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Gli Il-38 lanciano le zattere. Il vento le rovescia prima che qualcuno le raggiunga.", en = "The Il-38s drop life rafts. The wind flips them before anyone can reach them." } },
        } },
      { id = "orologio", una_volta = false,
        quando = {
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 11, valore = 10, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "posizione_dall_aereo", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 1790, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 3, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 128306.67640905583 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 14 },
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "IL38", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "norvegia", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 9000, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "NORVEGIA", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "accetta", una_volta = true,
        quando = {
          { tipo = "giocatore_ha_trasmesso", valore = 9000, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", trasmissione = "richiesta" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 12, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "vai_a", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 128306.67640905583 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 20 },
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "ACCETTATO", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "vicino_azzera", una_volta = false,
        quando = {
          { tipo = "tempo", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 13, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "vicino_ora", una_volta = false,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 5000, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 13, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_vicino_5", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 13, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "salvati_5_prima", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 40, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 40, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il peschereccio e' arrivato prima che il battello andasse giu': l'equipaggio e' passato a bordo. Tutti e quaranta.", en = "The factory trawler arrived before the boat went down: the crew stepped across. All forty." } },
        } },
      { id = "salvati_5_35", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1210, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 35, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 35, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 44, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il peschereccio li ha raccolti dopo pochi minuti in acqua: trentacinque uomini sono vivi.", en = "The factory trawler picked them up after a few minutes in the water: thirty-five men are alive." } },
        } },
      { id = "salvati_5_25", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1200, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 2710, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 25, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 25, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 45, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il peschereccio li ha raccolti dopo piu' di venti minuti in acqua a due gradi: venticinque uomini sono vivi.", en = "The factory trawler picked them up after more than twenty minutes in two-degree water: twenty-five men are alive." } },
        } },
      { id = "salvati_5_15", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 2700, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 4810, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 15, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 15, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 46, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il peschereccio li ha raccolti dopo quasi un'ora: quindici uomini sono vivi.", en = "The factory trawler picked them up after almost an hour: fifteen men are alive." } },
        } },
      { id = "salvati_5_5", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 4800, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 5, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 5, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 47, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il peschereccio e' arrivato troppo tardi per quasi tutti: cinque uomini sono vivi.", en = "The factory trawler arrived too late for almost all of them: five men are alive." } },
        } },
      { id = "alfa_vicino_7", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 13, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "salvati_7_prima", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 40, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 40, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 50, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave norvegese e' arrivato prima che il battello andasse giu': l'equipaggio e' passato a bordo. Tutti e quaranta.", en = "The Norwegian ship arrived before the boat went down: the crew stepped across. All forty." } },
        } },
      { id = "salvati_7_35", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1210, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 35, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 35, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 51, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave norvegese li ha raccolti dopo pochi minuti in acqua: trentacinque uomini sono vivi.", en = "The Norwegian ship picked them up after a few minutes in the water: thirty-five men are alive." } },
        } },
      { id = "salvati_7_25", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1200, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 2710, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 25, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 25, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 52, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave norvegese li ha raccolti dopo piu' di venti minuti in acqua a due gradi: venticinque uomini sono vivi.", en = "The Norwegian ship picked them up after more than twenty minutes in two-degree water: twenty-five men are alive." } },
        } },
      { id = "salvati_7_15", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 2700, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 4810, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 15, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 15, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 53, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave norvegese li ha raccolti dopo quasi un'ora: quindici uomini sono vivi.", en = "The Norwegian ship picked them up after almost an hour: fifteen men are alive." } },
        } },
      { id = "salvati_7_5", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 8, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 4800, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 5, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "salvati", codice = 0, unita = -1, indice = 40, valore = 5, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 54, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave norvegese e' arrivato troppo tardi per quasi tutti: cinque uomini sono vivi.", en = "The Norwegian ship arrived too late for almost all of them: five men are alive." } },
        } },
    },
    radio = { ordini = { da = "Comando della Flotta del Nord", a = "K-463 (PROGETTO 705)", oggetto = "SOCCORSO", classifica = "SEGRETO" }, roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, messaggi = {
      { id = "RICEVUTO", dopo_s = 0, su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "POSIZIONE", classifica = "SEGRETO", testo = { it = "POSIZIONE RICEVUTA. IL PESCHERECCIO-MADRE ACCOSTA E VA A TUTTA. RESTATE SUL POSTO.", en = "POSITION RECEIVED. THE FACTORY TRAWLER IS TURNING AND COMING AT FULL SPEED. STAY ON SCENE." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "A_MARE", dopo_s = 0, su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "EQUIPAGGIO IN ACQUA", classifica = "SEGRETO", testo = { it = "IL BATTELLO E' AFFONDATO. L'EQUIPAGGIO E' IN ACQUA. OGNI MINUTO CONTA.", en = "THE BOAT HAS SUNK. THE CREW IS IN THE WATER. EVERY MINUTE COUNTS." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "IL38", dopo_s = 0, su_richiesta = true, precedenza = "flash", da = "Comando della Flotta del Nord", oggetto = "POSIZIONE DALL'AEREO", classifica = "SEGRETO", testo = { it = "L'IL-38 HA DATO LA POSIZIONE. IL PESCHERECCIO-MADRE ACCOSTA.", en = "THE IL-38 HAS GIVEN THE POSITION. THE FACTORY TRAWLER IS TURNING." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "NORVEGIA", dopo_s = 0, su_richiesta = true, precedenza = "immediate", da = "Guardia costiera norvegese", oggetto = "OFFERTA DI ASSISTENZA", classifica = "NON CLASSIFICATO", testo = { it = "SULLA FREQUENZA DI SOCCORSO: UNA NAVE DELLA GUARDIA COSTIERA NORVEGESE PUO' ESSERE SUL POSTO IN UN'ORA. SE LA VOLETE, CHIEDETELA.", en = "ON THE DISTRESS FREQUENCY: A NORWEGIAN COAST GUARD SHIP CAN BE ON SCENE IN AN HOUR. IF YOU WANT IT, ASK FOR IT." }, autentico = false, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "ACCETTATO", dopo_s = 0, su_richiesta = true, precedenza = "immediate", da = "Guardia costiera norvegese", oggetto = "IN ARRIVO", classifica = "NON CLASSIFICATO", testo = { it = "RICEVUTO. LA NAVE E' IN ARRIVO A TUTTA FORZA.", en = "ROGER. THE SHIP IS COMING AT FULL SPEED." }, autentico = false, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
    } },
    meteo_fisso = { vento_m_s = 15, vento_da_gradi = 240, stato_mare = 5, visibilita_m = 4000, nubi_ottavi = 8, nebbia = "" },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", schieramento = "urss", flag = 17, giocatore = true,
        posizione = { x = 0, y = 66940.6 }, quota_m = 100, rotta = 0, velocita_kn = 10,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "akula", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 0, y = 128306.7 }, quota_m = 0, rotta = 300, velocita_kn = 0, stato = "avaria",
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- battello sperimentale in avaria, incendio a poppa (al posto del Mike)
      { id = 4, classe = "il38", categoria = "aereo", schieramento = "urss", flag = 19,
        posizione = { x = 0, y = 2118306.7 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- Il-38 della Flotta del Nord, di riserva: entra con la regola il38_1
      { id = 5, classe = "il38", categoria = "aereo", schieramento = "urss", flag = 19,
        posizione = { x = 0, y = 2138306.7 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- Il-38 della Flotta del Nord, di riserva: entra con la regola il38_2
      { id = 6, classe = "bmrt", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = 47631.4, y = 100806.7 }, quota_m = 0, rotta = 320, velocita_kn = 8,
        modello = { massima_kn = 13, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 18, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5130.3, y = 142402.1 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- peschereccio-madre deviato sull'emergenza
      { id = 7, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 75,
        posizione = { x = -6000, y = 128306.7 }, quota_m = 0, rotta = 0, velocita_kn = 220,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 0, y = 134306.7 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_7_p2" } },   -- Orion norvegese che osserva
      { id = 8, classe = "oslo", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = -22000, y = 2288306.7 }, quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 14, difesa = 50, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- nave della guardia costiera norvegese (al posto della Nordkapp), di riserva: entra con la regola accetta
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 16200, unita = 0, gestore = "R_tempo" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_1" },
      { dopo_s = 1800, unita = 0, gestore = "R_regola_2" },
      { dopo_s = 3600, unita = 0, gestore = "R_regola_3" },
      { dopo_s = 5400, unita = 0, gestore = "R_regola_4" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_5" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_6" },
      { dopo_s = 10200, unita = 0, gestore = "R_regola_7" },
      { dopo_s = 10800, unita = 0, gestore = "R_regola_8" },
      { dopo_s = 11100, unita = 0, gestore = "R_regola_9" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_10" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_11" },
      { dopo_s = 9000, unita = 0, gestore = "R_regola_12" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_13" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_14" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_15" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_16" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_17" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_18" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_19" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_20" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_21" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_22" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_23" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_24" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_25" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_26" },
      { dopo_s = 10, unita = 8, gestore = "R_regola_27" },
    },
    variabili = { [10] = 0, [11] = 0 },
    },
  },
}

-- kit dell'Alfa (data/motore/missioni_rapide/, dalle missioni originali)
script.KR3B_0631 = function(u)
  ::KR3B_0631::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "KR3B_06fc", "KR3B_06d9")
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 1.0286)
  m688.modello_avanti_tutta(u, 45.7143)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 15)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 100)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 150, "KR3B_0693")
  m688.dopo(u, 170, "KR3B_06bc")
  m688.dopo(u, 240, "KR3B_06fc")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end
script.KR3B_0693 = function(u)
  ::KR3B_0693::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end
script.KR3B_06bc = function(u)
  ::KR3B_06bc::
  m688.annulla_timer(u, "KR3B_06bc")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end
script.KR3B_06c7 = function(u)
  ::KR3B_06c7::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "KR3B_06ed")
  m688.annulla_timer(u, "KR3B_06d9")
  m688.insegui(u, 0, "KR3B_06fc", "KR3B_06d9")
  m688.salva_modello(u)
  do return end
end
script.KR3B_06d9 = function(u)
  ::KR3B_06d9::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "KR3B_06c7")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "KR3B_06f5")
  m688.dopo(u, 10, "KR3B_06ed")
  do return end
end
script.KR3B_06ed = function(u)
  ::KR3B_06ed::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end
script.KR3B_06f5 = function(u)
  ::KR3B_06f5::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "KR3B_06f5")
  do return end
end
script.KR3B_06fc = function(u)
  ::KR3B_06fc::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end
script.KR3B_0700 = function(u)
  ::KR3B_0700::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "KR3B_06fc", "KR3B_06d9")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 0)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 2, "KR3B_072d")
  m688.dopo(u, 160, "KR3B_0732")
  do return end
end
script.KR3B_072d = function(u)
  ::KR3B_072d::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end
script.KR3B_0732 = function(u)
  ::KR3B_0732::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end
script.KR3B_0736 = function(u)
  ::KR3B_0736::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end
script.KR3B_073b = function(u)
  ::KR3B_073b::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "KR3B_06d9")
  do return end
end
script.KR3B_0745 = function(u, p)
  ::KR3B_0745::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "KR3B_0754")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end
script.KR3B_0754 = function(u)
  ::KR3B_0754::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "KR3B_06c7")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto KR3B_0762 end
  m688.messaggio_al_giocatore(u, 42)
  ::KR3B_0762::
  do return end
end
script.KR3B_0763 = function(u, p)
  ::KR3B_0763::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "KR3B_076e")
  m688.salva_modello(u)
  do return end
end
script.KR3B_076e = function(u)
  ::KR3B_076e::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end
script.KR3B_0775 = function(u, p)
  ::KR3B_0775::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KR3B_077a = function(u, p)
  ::KR3B_077a::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end
script.KR3B_0781 = function(u, p)
  ::KR3B_0781::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end
script.KR3B_0789 = function(u)
  ::KR3B_0789::
  m688.annulla_timer(u, "KR3B_07c8")
  m688.dopo(u, 20, "KR3B_07c8")
  do return end
end
script.KR3B_0792 = function(u)
  ::KR3B_0792::
  m688.annulla_timer(u, "KR3B_07cb")
  m688.dopo(u, 20, "KR3B_07cb")
  do return end
end
script.KR3B_079b = function(u)
  ::KR3B_079b::
  m688.annulla_timer(u, "KR3B_07ce")
  m688.dopo(u, 20, "KR3B_07ce")
  do return end
end
script.KR3B_07a4 = function(u)
  ::KR3B_07a4::
  m688.annulla_timer(u, "KR3B_07d1")
  m688.dopo(u, 20, "KR3B_07d1")
  do return end
end
script.KR3B_07ad = function(u)
  ::KR3B_07ad::
  m688.annulla_timer(u, "KR3B_07d4")
  m688.dopo(u, 20, "KR3B_07d4")
  do return end
end
script.KR3B_07b6 = function(u)
  ::KR3B_07b6::
  m688.annulla_timer(u, "KR3B_07d7")
  m688.dopo(u, 20, "KR3B_07d7")
  do return end
end
script.KR3B_07bf = function(u)
  ::KR3B_07bf::
  m688.annulla_timer(u, "KR3B_07da")
  m688.dopo(u, 20, "KR3B_07da")
  do return end
end
script.KR3B_07c8 = function(u)
  ::KR3B_07c8::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end
script.KR3B_07cb = function(u)
  ::KR3B_07cb::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end
script.KR3B_07ce = function(u)
  ::KR3B_07ce::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end
script.KR3B_07d1 = function(u)
  ::KR3B_07d1::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end
script.KR3B_07d4 = function(u)
  ::KR3B_07d4::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end
script.KR3B_07d7 = function(u)
  ::KR3B_07d7::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end
script.KR3B_07da = function(u)
  ::KR3B_07da::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end
script.KR3B_07dd = function(u)
  ::KR3B_07dd::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end
script.KR3B_07e5 = function(u, p)
  ::KR3B_07e5::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KR3B_07e9 = function(u)
  ::KR3B_07e9::
  m688.dopo(u, 60, "KR3B_07f3")
  do return end
end
script.KR3B_07ef = function(u, p)
  ::KR3B_07ef::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KR3B_07f3 = function(u)
  ::KR3B_07f3::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end
script.KR3B_07f9 = function(u)
  ::KR3B_07f9::
  m688.dopo(u, 40, "KR3B_07ff")
  do return end
end
script.KR3B_07ff = function(u)
  ::KR3B_07ff::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end
script.KR3B_0805 = function(u)
  ::KR3B_0805::
  m688.annulla_timer(u, "KR3B_07f3")
  m688.annulla_timer(u, "KR3B_07ff")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end
script.KR3B_080f = function(u, p)
  ::KR3B_080f::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KR3B_0814 = function(u, p)
  ::KR3B_0814::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

-- kit del 688: i gestori di lancio usati dall'IA
script.KU1A_03cc = function(u)
  ::KU1A_03cc::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "KU1A_03d6")
  do return end
end
script.KU1A_03d6 = function(u)
  ::KU1A_03d6::
  m688.attacco_casuale(u, 75)
  do return end
end
script.KU1A_041a = function(u)
  ::KU1A_041a::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "KU1A_04a7", "KU1A_0484")
  script.KU1A_0441(u)
  m688.dopo(u, 170, "KU1A_043e")
  m688.dopo(u, 190, "KU1A_0467")
  m688.dopo(u, 280, "KU1A_04a7")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end
script.KU1A_043e = function(u)
  ::KU1A_043e::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end
script.KU1A_0441 = function(u)
  ::KU1A_0441::
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 0.7714)
  m688.modello_avanti_tutta(u, 50.2857)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 17)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 75)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  do return end
end
script.KU1A_0467 = function(u)
  ::KU1A_0467::
  m688.annulla_timer(u, "KU1A_0467")
  m688.collega(u, 0)
  m688.modello_sonar(u, 16)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end
script.KU1A_0472 = function(u)
  ::KU1A_0472::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "KU1A_0498")
  m688.annulla_timer(u, "KU1A_0484")
  m688.insegui(u, 0, "KU1A_04a7", "KU1A_0484")
  m688.salva_modello(u)
  do return end
end
script.KU1A_0484 = function(u)
  ::KU1A_0484::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "KU1A_0472")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "KU1A_04a0")
  m688.dopo(u, 10, "KU1A_0498")
  do return end
end
script.KU1A_0498 = function(u)
  ::KU1A_0498::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end
script.KU1A_04a0 = function(u)
  ::KU1A_04a0::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "KU1A_04a0")
  do return end
end
script.KU1A_04a7 = function(u)
  ::KU1A_04a7::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end
script.KU1A_04ab = function(u)
  ::KU1A_04ab::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "KU1A_04a7", "KU1A_0484")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 0)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 2, "KU1A_04d8")
  m688.dopo(u, 160, "KU1A_04dd")
  do return end
end
script.KU1A_04d8 = function(u)
  ::KU1A_04d8::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end
script.KU1A_04dd = function(u)
  ::KU1A_04dd::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end
script.KU1A_04e1 = function(u)
  ::KU1A_04e1::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end
script.KU1A_04e6 = function(u)
  ::KU1A_04e6::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "KU1A_0484")
  do return end
end
script.KU1A_04f0 = function(u, p)
  ::KU1A_04f0::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "KU1A_04ff")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end
script.KU1A_04ff = function(u)
  ::KU1A_04ff::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "KU1A_0472")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto KU1A_050d end
  m688.messaggio_al_giocatore(u, 42)
  ::KU1A_050d::
  do return end
end
script.KU1A_050e = function(u, p)
  ::KU1A_050e::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "KU1A_0519")
  m688.salva_modello(u)
  do return end
end
script.KU1A_0519 = function(u)
  ::KU1A_0519::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end
script.KU1A_0520 = function(u, p)
  ::KU1A_0520::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KU1A_0525 = function(u, p)
  ::KU1A_0525::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end
script.KU1A_052c = function(u, p)
  ::KU1A_052c::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end
script.KU1A_0534 = function(u)
  ::KU1A_0534::
  m688.annulla_timer(u, "KU1A_0573")
  m688.dopo(u, 20, "KU1A_0573")
  do return end
end
script.KU1A_053d = function(u)
  ::KU1A_053d::
  m688.annulla_timer(u, "KU1A_0576")
  m688.dopo(u, 20, "KU1A_0576")
  do return end
end
script.KU1A_0546 = function(u)
  ::KU1A_0546::
  m688.annulla_timer(u, "KU1A_0579")
  m688.dopo(u, 20, "KU1A_0579")
  do return end
end
script.KU1A_054f = function(u)
  ::KU1A_054f::
  m688.annulla_timer(u, "KU1A_057c")
  m688.dopo(u, 20, "KU1A_057c")
  do return end
end
script.KU1A_0558 = function(u)
  ::KU1A_0558::
  m688.annulla_timer(u, "KU1A_057f")
  m688.dopo(u, 20, "KU1A_057f")
  do return end
end
script.KU1A_0561 = function(u)
  ::KU1A_0561::
  m688.annulla_timer(u, "KU1A_0582")
  m688.dopo(u, 20, "KU1A_0582")
  do return end
end
script.KU1A_056a = function(u)
  ::KU1A_056a::
  m688.annulla_timer(u, "KU1A_0585")
  m688.dopo(u, 20, "KU1A_0585")
  do return end
end
script.KU1A_0573 = function(u)
  ::KU1A_0573::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end
script.KU1A_0576 = function(u)
  ::KU1A_0576::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end
script.KU1A_0579 = function(u)
  ::KU1A_0579::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end
script.KU1A_057c = function(u)
  ::KU1A_057c::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end
script.KU1A_057f = function(u)
  ::KU1A_057f::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end
script.KU1A_0582 = function(u)
  ::KU1A_0582::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end
script.KU1A_0585 = function(u)
  ::KU1A_0585::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end
script.KU1A_0588 = function(u)
  ::KU1A_0588::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end
script.KU1A_0590 = function(u, p)
  ::KU1A_0590::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KU1A_0594 = function(u)
  ::KU1A_0594::
  m688.dopo(u, 60, "KU1A_059e")
  do return end
end
script.KU1A_059a = function(u, p)
  ::KU1A_059a::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KU1A_059e = function(u)
  ::KU1A_059e::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end
script.KU1A_05a4 = function(u)
  ::KU1A_05a4::
  m688.dopo(u, 40, "KU1A_05aa")
  do return end
end
script.KU1A_05aa = function(u)
  ::KU1A_05aa::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end
script.KU1A_05b0 = function(u)
  ::KU1A_05b0::
  m688.annulla_timer(u, "KU1A_059e")
  m688.annulla_timer(u, "KU1A_05aa")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end
script.KU1A_05ba = function(u, p)
  ::KU1A_05ba::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end
script.KU1A_05bf = function(u, p)
  ::KU1A_05bf::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end


-- Costanti della missione: regole d'ingaggio, paranoia (1) e difficolta' (3)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 12          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [3] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [4] = { parte = "urss", crociera = 0.994, comportamento = "R_aereo", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 3, distanza = 2 },
  [5] = { parte = "urss", crociera = 0.994, comportamento = "R_aereo", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 3, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.673, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 3, distanza = 2 },
  [7] = { parte = "usa", crociera = 0.729, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 3, distanza = 2 },
  [8] = { parte = "neutrale", crociera = 0.787, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 3, distanza = 2 },
}

-- Comportamenti dell'IA (API classica, docs/09 §6), con i parametri per unita' della tabella UNITA. Variabili di scena:
-- 1 siluri lanciati dal giocatore (kit), 10 fase della missione, 11 allarme, 100 + id salve lanciate da un'unita',
-- 200 + n regole scattate, 300 + i variabili dell'autore, 400 + id passaggi d'attesa di una nave dopo una salva,
-- 450 + id passaggi di caccia di una nave (nel classico la prima salva dopo PRIMO_ATTACCO), 460 + id le sue salve
-- dall'ultima ripresa, 470 + id i passaggi d'ascolto dopo il tetto delle salve, 480 + id lo stato del pedinamento
-- ravvicinato (1 in corso, 2 scoperto col ping: non torna).
-- Nell'Enhanced (docs/18 fase 3) le regole d'ingaggio sono quelle dell'IA (m688.armi_libere: crisi solo dopo un atto
-- ostile fra le parti, zone amiche) e l'allerta sale con gli eventi, mai sotto la paranoia dell'editor; nel classico
-- quelle funzioni danno nil e valgono le costanti della missione.
local PING, AGGANCIO, VIGILA = { 0.05, 0.2, 0.4, 0.7 }, { 8, 5, 3, 2 }, { 60, 180, 90, 60 }
local ATTESA_SALVA = 20   -- passaggi sul contatto fra due salve antisom di una nave (riposizionarsi, ricaricare)
-- la caccia fino al PROBSUB: la prima salva di una nave dopo PRIMO_ATTACCO_S secondi sul contatto attuale (Enhanced,
-- m688.tempo_contatto), nel classico dopo PRIMO_ATTACCO passaggi; dopo SALVE_MASSIME salve si torna ad ascoltare e si
-- riattacca dopo RIPRESA passaggi sul contatto (STIME)
local PRIMO_ATTACCO_S, PRIMO_ATTACCO = 780, 330
local SALVE_MASSIME, RIPRESA = 6, 120
local function armi_libere(u)
  local a = m688.armi_libere(u)
  if a ~= nil then return a end
  return ARMI == 2 or (ARMI == 1 and m688.variabile_maggiore(u, 1, 0))
end
local function allerta(u)
  local a = m688.allerta(u)
  if a == nil then return UNITA[u].paranoia end
  return math.max(a, UNITA[u].paranoia)
end
local function contatto_buono(u)
  return m688.traccia_solida(u) or (allerta(u) >= 3 and m688.traccia_media(u))
end
-- nell'Enhanced si lancia su una posizione stimata (eco, vista, radar, TMA), non sul solo rilevamento: finche' non
-- c'e', il motore fa da se' le gambe di TMA (fase 4) e lo script non le disturba con rotta e velocita'
local function soluzione(u)
  return m688.allerta(u) == nil or m688.soluzione(u)
end
-- siluri nei tubi o di riserva (Enhanced); nil nel classico o senza i dati di carico: si lancia come prima
local function ha_armi(u)
  local n = m688.armi_rimaste(u)
  return n == nil or n > 0
end
-- i non combattenti (mercantili, pescherecci, navi spia: flag 40h) non si ingaggiano finche' le armi non sono libere
-- (il peschereccio in pace); in guerra un convoglio nemico resta un bersaglio. Nel classico la funzione da' nil
local function non_combattente(u) return m688.bersaglio_non_combattente(u) == true end
local function caccia_finita(u)
  local t = m688.tempo_contatto(u)
  if t ~= nil then return t >= PRIMO_ATTACCO_S end
  return m688.variabile_maggiore(u, 450 + u, PRIMO_ATTACCO)
end
local function riprendi_rotta(u)
  local f = script["R_" .. u .. "_p1"]
  if f ~= nil then f(u) end
end

-- nave da guerra: segue la sua rotta; su un contatto nemico subacqueo accosta, fa ping e, se le regole lo consentono,
-- attacca con le armi antisom (ATTACK: riesce con probabilita' (18 - COLPO) / 18). Le navi nemiche no: la caccia
-- antisom su una nave di superficie illuminava col ping i sottomarini vicini (il tiro fra navi e' del motore e dei missili)
script.R_nave = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) or m688.bersaglio_nave(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, CACCIA)
  if m688.caso(u, PING[allerta(u) + 1]) then m688.impulso_sonar(u) end
  -- una salva, poi ATTESA_SALVA passaggi prima della successiva; nell'Enhanced solo con una posizione stimata
  m688.variabile_piu(u, 450 + u)   -- i passaggi di caccia
  if m688.variabile_maggiore(u, 400 + u, 0) then
    m688.variabile_meno(u, 400 + u)
  elseif m688.variabile_maggiore(u, 470 + u, 0) then
    m688.variabile_meno(u, 470 + u)   -- dopo il tetto delle salve: la caccia ricomincia
  elseif armi_libere(u) and not m688.bersaglio_nave(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and soluzione(u) and
         caccia_finita(u) then
    if not m688.variabile_minore(u, 460 + u, SALVE_MASSIME) then
      -- tante salve senza affondarlo: si torna ad ascoltare, e si riattacca solo dopo RIPRESA passaggi sul contatto
      m688.variabile(u, 460 + u, 0)
      m688.variabile(u, 470 + u, RIPRESA)
      goto ascolta
    end
    m688.messaggio(u, 19)
    m688.attacca(u, UNITA[u].portata, COLPO)
    m688.variabile(u, 400 + u, ATTESA_SALVA)
    m688.variabile_piu(u, 460 + u)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_nave", "R_persa")
  m688.salva_modello(u)
  do return end
  ::ascolta::
  if UNITA[u].rotta then m688.riprendi_navigazione(u) end
  m688.telegrafo(u, UNITA[u].crociera)
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
  m688.dopo(u, 90, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
end

-- sottomarino: pattuglia lento; sul contatto nemico si avvicina (se insegue) e lancia entro la portata, al piu' `salve`
-- volte; davanti a un siluro scappa e scende
script.R_sottomarino = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  if armi_libere(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and m688.variabile_minore(u, 100 + u, UNITA[u].salve) and
     soluzione(u) and ha_armi(u) then goto lancia end
  if allerta(u) >= 1 and soluzione(u) then
    m688.ferma_navigazione(u)
    m688.accosta_verso_bersaglio(u)
    m688.telegrafo(u, AVVICINAMENTO)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  do return end
  ::lancia::
  m688.variabile_piu(u, 100 + u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, AVVICINAMENTO)
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  -- il gestore di lancio crea il siluro dal modello corrente: dopo non si salva piu' il lanciatore
  script["R_siluro_" .. UNITA[u].parte](u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.quota(u, UNITA[u].profonda, 1)
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
  m688.dopo(u, 120, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
end

-- aereo antisom (nel classico; nell'Enhanced lo guida il controllore di docs/13 e vai_al_bersaglio diventa un datum)
script.R_aereo = function(u)
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_nave(u) or m688.bersaglio_aereo(u) or m688.bersaglio_arma(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.vai_al_bersaglio(u, 60, "R_aereo_sopra")
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
  do return end
  ::ignora::
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
end

script.R_aereo_sopra = function(u)
  -- sopra il bersaglio: attacca solo se quello che osserva adesso e' ancora un sottomarino nemico (l'osservazione gira
  -- su tutte le unita', anche amiche e navi)
  if armi_libere(u) and m688.bersaglio_nemico(u) and not non_combattente(u) and not (m688.bersaglio_nave(u) or m688.bersaglio_aereo(u) or m688.bersaglio_arma(u)) and
     contatto_buono(u) then
    m688.messaggio(u, 19)
    m688.attacca(u, 1, COLPO)
  end
  m688.telegrafo(u, UNITA[u].crociera)
  riprendi_rotta(u)
  m688.salva_modello(u)
end

-- bersaglio perso: la rotta di prima alla velocita' di crociera
script.R_persa = function(u)
  if UNITA[u].rotta then m688.riprendi_navigazione(u) end
  m688.telegrafo(u, UNITA[u].crociera)
  if UNITA[u].comportamento ~= "" then m688.comportamento(u, UNITA[u].comportamento) end
  m688.salva_modello(u)
end

-- pedinamento ravvicinato (S24): sul giocatore, con una traccia buona, il motore lo tiene nel cono sordo a `distanza`
-- miglia e 20 m sotto la quota che vede (m688.pedina_vicino), e ne segue le manovre con ritardo; se il giocatore pinga
-- entro 3 km smette e non torna, e l'evasione dell'IA lo porta via. Solo il giocatore: lo script non sa l'id del bersaglio
script.R_pedina_vicino = function(u)
  if m688.variabile_uguale(u, 480 + u, 1) and m688.ping_giocatore_entro(u, 3000, 60) then
    m688.pedina_vicino(u, -1)
    m688.variabile(u, 480 + u, 2)
  end
  if m688.variabile_uguale(u, 480 + u, 2) or not m688.bersaglio_id(u, 0) or not contatto_buono(u) then goto ignora end
  if m688.variabile_uguale(u, 480 + u, 0) and m688.pedina_vicino(u, 0, UNITA[u].distanza * 1852, -20) then
    m688.variabile(u, 480 + u, 1)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina_vicino", "R_persa")
  m688.salva_modello(u)
  do return end
  ::ignora::
  m688.comportamento(u, "R_pedina_vicino")
  m688.salva_modello(u)
end

-- pedinamento: sul contatto nemico si porta a `distanza` miglia e ci resta, alla stessa velocita', senza farsi notare;
-- spara solo se le regole lo consentono, come il sottomarino; davanti a un siluro scappa
script.R_pedina = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  if armi_libere(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and m688.variabile_minore(u, 100 + u, UNITA[u].salve) and
     soluzione(u) and ha_armi(u) then goto lancia end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, UNITA[u].distanza) then
    m688.uguaglia_velocita(u)
  else
    m688.telegrafo(u, AVVICINAMENTO * 1.6)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina", "R_persa")
  m688.salva_modello(u)
  do return end
  ::lancia::
  m688.variabile_piu(u, 100 + u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina", "R_persa")
  m688.salva_modello(u)
  script["R_siluro_" .. UNITA[u].parte](u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_pedina")
  m688.salva_modello(u)
  m688.dopo(u, 120, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_pedina")
  m688.salva_modello(u)
end

-- in allarme le navi fanno ping
script.R_vigila = function(u)
  if m688.variabile_uguale(u, 11, 1) and m688.caso(u, PING[allerta(u) + 1]) then m688.impulso_sonar(u) end
  m688.dopo(u, VIGILA[allerta(u) + 1], "R_vigila")
end

-- fine della rotta di transito: l'unita' esce dal teatro
script.R_esce = function(u)
  m688.togli_unita(u)
end

-- lanci dei sottomarini dell'IA: i gestori di lancio dei kit senza il conto del giocatore
script.R_siluro_usa = function(u)
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "KU1A_04a7", "KU1A_0484")
  script.KU1A_0441(u)
  m688.dopo(u, 170, "KU1A_043e")
  m688.dopo(u, 190, "KU1A_0467")
  m688.dopo(u, 280, "KU1A_04a7")
  do return end
end
script.R_siluro_urss = function(u)
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "KR3B_06fc", "KR3B_06d9")
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 1.0286)
  m688.modello_avanti_tutta(u, 45.7143)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 15)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 100)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 150, "KR3B_0693")
  m688.dopo(u, 170, "KR3B_06bc")
  m688.dopo(u, 240, "KR3B_06fc")
  do return end
end

-- rotte: all'arrivo a ogni punto il successivo (le pattuglie ricominciano, i transiti escono)
script.R_6_p1 = function(u)
  m688.vai_a(u, -5130.3, 142402.1, 30, "")
  m688.salva_modello(u)
end
script.R_7_p1 = function(u)
  m688.vai_a(u, 0, 134306.7, 30, "R_7_p2")
  m688.salva_modello(u)
end
script.R_7_p2 = function(u)
  m688.vai_a(u, 5196.2, 131306.7, 30, "R_7_p3")
  m688.salva_modello(u)
end
script.R_7_p3 = function(u)
  m688.vai_a(u, 5196.2, 125306.7, 30, "R_7_p4")
  m688.salva_modello(u)
end
script.R_7_p4 = function(u)
  m688.vai_a(u, 0, 122306.7, 30, "R_7_p5")
  m688.salva_modello(u)
end
script.R_7_p5 = function(u)
  m688.vai_a(u, -5196.2, 125306.7, 30, "R_7_p6")
  m688.salva_modello(u)
end
script.R_7_p6 = function(u)
  m688.vai_a(u, -5196.2, 131306.7, 30, "R_7_p1")
  m688.salva_modello(u)
end
script.R_8_p1 = function(u)
  m688.vai_a(u, 0, 128306.7, 30, "")
  m688.salva_modello(u)
end

-- chiusura: codice 120 a missione conclusa, 121 a tempo scaduto (il giudizio lo danno gli obiettivi)
script.R_chiusura = function(u)
  m688.dopo(u, 20, "R_chiusura")
  do return end
  ::fine::
  m688.messaggio(u, 120)
end
script.R_tempo = function(u)
  m688.messaggio(u, 34)
  m688.messaggio(u, 121)
end

-- regole «quando... allora...» dell'editor: un controllo ogni 10 s sull'unita' della regola
local ZONE = {
  ["luogo"] = { 0, 128306.7, 600 },
}
-- fermo
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) then
    m688.variabile(u, 200, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.nessun_comportamento(u)
      m688.salva_modello(u)
      UNITA[3].comportamento = ""
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- resta_su
script.R_regola_1 = function(u)
  if true and m688.variabile_uguale(u, 310, 0) then
    m688.variabile(u, 201, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.quota(u, 0, 1)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- il38_1
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) then
    m688.variabile(u, 202, 1)
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.posizione(u, 0, 38306.7)
      m688.rotta_ora(u, 0)
      m688.velocita_ora(u, 300)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- il38_2
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) then
    m688.variabile(u, 203, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.posizione(u, 0, 38306.7)
      m688.rotta_ora(u, 0)
      m688.velocita_ora(u, 300)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- tardi
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) then
    m688.variabile(u, 204, 1)
    m688.variabile(u, 309, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- arrivato
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) and (m688.distanza_da(u, 3) or 1e18) < 2000 and m688.variabile_uguale(u, 309, 0) then
    m688.variabile(u, 205, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end
-- posizione
script.R_regola_6 = function(u)
  if m688.variabile_uguale(u, 206, 0) and (m688.distanza_da(u, 3) or 1e18) < 5000 and m688.trasmesso(u, "situazione", 0) then
    m688.variabile(u, 206, 1)
    m688.variabile(u, 303, 1)
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.vai_a(u, 0, 128306.7, 30, "")
      m688.telegrafo(u, 1)
      m688.salva_modello(u)
      UNITA[6].rotta = true
      UNITA[6].crociera = 1
    end
    m688.radio(u, "RICEVUTO")
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_6")
end
-- si_appoppa
script.R_regola_7 = function(u)
  if m688.variabile_uguale(u, 207, 0) then
    m688.variabile(u, 207, 1)
    m688.messaggio_al_giocatore(u, 1001)
    do return end
  end
  m688.dopo(u, 10, "R_regola_7")
end
-- a_mare
script.R_regola_8 = function(u)
  if m688.variabile_uguale(u, 208, 0) then
    m688.variabile(u, 208, 1)
    m688.variabile(u, 310, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.togli_unita(u)
    end
    m688.radio(u, "A_MARE")
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_8")
end
-- zattere
script.R_regola_9 = function(u)
  if m688.variabile_uguale(u, 209, 0) then
    m688.variabile(u, 209, 1)
    m688.messaggio_al_giocatore(u, 1002)
    do return end
  end
  m688.dopo(u, 10, "R_regola_9")
end
-- orologio
script.R_regola_10 = function(u)
  if true and m688.variabile_uguale(u, 310, 1) then
    m688.variabile(u, 210, 1)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
    m688.variabile_piu(u, 311)
  end
  m688.dopo(u, 10, "R_regola_10")
end
-- posizione_dall_aereo
script.R_regola_11 = function(u)
  if m688.variabile_uguale(u, 211, 0) and m688.variabile_maggiore(u, 311, 1790) and m688.variabile_uguale(u, 303, 0) then
    m688.variabile(u, 211, 1)
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.vai_a(u, 0, 128306.7, 30, "")
      m688.telegrafo(u, 1)
      m688.salva_modello(u)
      UNITA[6].rotta = true
      UNITA[6].crociera = 1
    end
    m688.radio(u, "IL38")
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_11")
end
-- norvegia
script.R_regola_12 = function(u)
  if m688.variabile_uguale(u, 212, 0) then
    m688.variabile(u, 212, 1)
    m688.radio(u, "NORVEGIA")
    do return end
  end
  m688.dopo(u, 10, "R_regola_12")
end
-- accetta
script.R_regola_13 = function(u)
  if m688.variabile_uguale(u, 213, 0) and m688.trasmesso(u, "richiesta", 9000) then
    m688.variabile(u, 213, 1)
    m688.variabile(u, 312, 1)
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.posizione(u, -22000, 128306.7)
      m688.rotta_ora(u, 90)
      m688.velocita_ora(u, 18)
      script.R_8_p1(u)
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.vai_a(u, 0, 128306.7, 30, "")
      m688.telegrafo(u, 0.875)
      m688.salva_modello(u)
      UNITA[8].rotta = true
      UNITA[8].crociera = 0.875
    end
    m688.radio(u, "ACCETTATO")
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_13")
end
-- vicino_azzera
script.R_regola_14 = function(u)
  if true then
    m688.variabile(u, 214, 1)
    m688.variabile(u, 313, 0)
  end
  m688.dopo(u, 10, "R_regola_14")
end
-- vicino_ora
script.R_regola_15 = function(u)
  if true and (m688.distanza_da(u, 3) or 1e18) < 5000 then
    m688.variabile(u, 215, 1)
    m688.variabile(u, 313, 1)
  end
  m688.dopo(u, 10, "R_regola_15")
end
-- alfa_vicino_5
script.R_regola_16 = function(u)
  if m688.variabile_uguale(u, 216, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 313, 1) then
    m688.variabile(u, 216, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_16")
end
-- salvati_5_prima
script.R_regola_17 = function(u)
  if m688.variabile_uguale(u, 217, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 310, 0) then
    m688.variabile(u, 217, 1)
    m688.variabile(u, 305, 40)
    m688.salvati(u, 40, 40)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_17")
end
-- salvati_5_35
script.R_regola_18 = function(u)
  if m688.variabile_uguale(u, 218, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 310, 1) and m688.variabile_minore(u, 311, 1210) then
    m688.variabile(u, 218, 1)
    m688.variabile(u, 305, 35)
    m688.salvati(u, 35, 40)
    m688.messaggio(u, 144)
    do return end
  end
  m688.dopo(u, 10, "R_regola_18")
end
-- salvati_5_25
script.R_regola_19 = function(u)
  if m688.variabile_uguale(u, 219, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 1200) and m688.variabile_minore(u, 311, 2710) then
    m688.variabile(u, 219, 1)
    m688.variabile(u, 305, 25)
    m688.salvati(u, 25, 40)
    m688.messaggio(u, 145)
    do return end
  end
  m688.dopo(u, 10, "R_regola_19")
end
-- salvati_5_15
script.R_regola_20 = function(u)
  if m688.variabile_uguale(u, 220, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 2700) and m688.variabile_minore(u, 311, 4810) then
    m688.variabile(u, 220, 1)
    m688.variabile(u, 305, 15)
    m688.salvati(u, 15, 40)
    m688.messaggio(u, 146)
    do return end
  end
  m688.dopo(u, 10, "R_regola_20")
end
-- salvati_5_5
script.R_regola_21 = function(u)
  if m688.variabile_uguale(u, 221, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 4800) then
    m688.variabile(u, 221, 1)
    m688.variabile(u, 305, 5)
    m688.salvati(u, 5, 40)
    m688.messaggio(u, 147)
    do return end
  end
  m688.dopo(u, 10, "R_regola_21")
end
-- alfa_vicino_7
script.R_regola_22 = function(u)
  if m688.variabile_uguale(u, 222, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 313, 1) then
    m688.variabile(u, 222, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_22")
end
-- salvati_7_prima
script.R_regola_23 = function(u)
  if m688.variabile_uguale(u, 223, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 310, 0) then
    m688.variabile(u, 223, 1)
    m688.variabile(u, 305, 40)
    m688.salvati(u, 40, 40)
    m688.messaggio(u, 150)
    do return end
  end
  m688.dopo(u, 10, "R_regola_23")
end
-- salvati_7_35
script.R_regola_24 = function(u)
  if m688.variabile_uguale(u, 224, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 310, 1) and m688.variabile_minore(u, 311, 1210) then
    m688.variabile(u, 224, 1)
    m688.variabile(u, 305, 35)
    m688.salvati(u, 35, 40)
    m688.messaggio(u, 151)
    do return end
  end
  m688.dopo(u, 10, "R_regola_24")
end
-- salvati_7_25
script.R_regola_25 = function(u)
  if m688.variabile_uguale(u, 225, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 1200) and m688.variabile_minore(u, 311, 2710) then
    m688.variabile(u, 225, 1)
    m688.variabile(u, 305, 25)
    m688.salvati(u, 25, 40)
    m688.messaggio(u, 152)
    do return end
  end
  m688.dopo(u, 10, "R_regola_25")
end
-- salvati_7_15
script.R_regola_26 = function(u)
  if m688.variabile_uguale(u, 226, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 2700) and m688.variabile_minore(u, 311, 4810) then
    m688.variabile(u, 226, 1)
    m688.variabile(u, 305, 15)
    m688.salvati(u, 15, 40)
    m688.messaggio(u, 153)
    do return end
  end
  m688.dopo(u, 10, "R_regola_26")
end
-- salvati_7_5
script.R_regola_27 = function(u)
  if m688.variabile_uguale(u, 227, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_maggiore(u, 311, 4800) then
    m688.variabile(u, 227, 1)
    m688.variabile(u, 305, 5)
    m688.salvati(u, 5, 40)
    m688.messaggio(u, 154)
    do return end
  end
  m688.dopo(u, 10, "R_regola_27")
end

return missione
