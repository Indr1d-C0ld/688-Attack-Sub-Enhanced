-- LIB_S25_OMBRA_DEL_BOOMER: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_S25_OMBRA_DEL_BOOMER",
  titolo = { it = "Ombra del boomer", en = "Boomer's Shadow" },
  lato = "usa",
  teatro = "celtica",
  posa = { metodo = "rapida", centro = { lat = 48.25, lon = -9.75 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-10-10", ora_utc = 4,
  dotazione = { mk48 = 22, harpoon = 2, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  deposito = { mk48 = 21, harpoon = 2, esca = 6 },
  tipo_missione = "antisom",
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "KU1A_041a" },
    harpoon = { gestore = "KU1A_03cc" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "KU1A_04ab" },   -- senza modello di corsa (missile o esca: vedi lo script)
    tomahawk = { gestore = "KU1A_03cc" },   -- TASM antinave: il gestore dell'Harpoon
  },
  gestori_giocatore = {
    punto_di_rotta = "KU1A_050e",
    telegrafo = "KU1A_0520",
    quota = "KU1A_0525",
    timone = "KU1A_052c",
    pilota_automatico_off = "KU1A_0588",
    periscopio = "KU1A_0590",
    carica_tubo_1 = "KU1A_0534",
    carica_tubo_2 = "KU1A_053d",
    carica_tubo_3 = "KU1A_0546",
    carica_tubo_4 = "KU1A_054f",
    carica_tubo_5 = "KU1A_0558",
    carica_tubo_6 = "KU1A_0561",
    carica_tubo_7 = "KU1A_056a",
    schiera_cala = "KU1A_0594",
    schiera_recupera = "KU1A_05a4",
    schiera_persa = "KU1A_05b0",
    designa_bersaglio = "KU1A_05ba",
    posizione = "KU1A_05bf",
    antenna = "KU1A_059a",
    guida_e0 = "KU1A_04e1",
    guida_e1 = "KU1A_04e6",
    guida_e2 = "KU1A_04f0",
  },
  carriera = false,   -- missione fuori carriera
  aereo_asw = {
    { unita = 6, missione = "caccia", situazione = "pace", bersagli = { 3 }, equipaggio = "medio",
      zona = { x = -56723.6, y = -105351, raggio_m = 15000 } },
  },
  categorie_esiti = { [41] = "fallimento" },
  obiettivi = {
    { id = "tenuto", tipo = "variabile", minimo = 1, indice = 304, almeno = 1,
      testo = { it = "Tenere il contatto sull'SSBN (almeno un'ora)", en = "Hold contact on the SSBN (at least an hour)" } },
    { id = "occulto", tipo = "variabile", minimo = 1, indice = 305, almeno = 1,
      testo = { it = "Non farsi classificare dall'SSBN", en = "Avoid being classified by the SSBN" } },
    { id = "riferito", tipo = "variabile", minimo = 1, indice = 306, almeno = 1,
      testo = { it = "Riferire l'emergenza entro 30 minuti (rapporto di situazione)", en = "Report the emergency within 30 minutes (situation report)" } },
    { id = "lontano", tipo = "variabile", minimo = 1, indice = 307, almeno = 1,
      testo = { it = "Restare a 5 miglia dalle navi sovietiche", en = "Stay 5 miles from the Soviet ships" } },
    { id = "iarde", tipo = "variabile", minimo = 1, primario = false, indice = 308, almeno = 1,
      testo = { it = "Mai a meno di 1000 iarde dall'SSBN", en = "Never inside 1000 yards of the SSBN" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: PEDINAMENTO DI UN SSBN - ATLANTICO OCCIDENTALE", "10 ottobre 1989, ore 0400Z. Teatro: Atlantico, sulla piana abissale. Pace. Mare d'autunno.", "Un SSBN sovietico scende verso il suo settore di pattuglia. Mettetevi in coda, tenetelo e registratelo senza farvi sentire.", "Distanza: oltre le 1000 iarde. In caso di qualsiasi evento anomalo dell'SSBN, riferite subito con un rapporto di situazione e attendete ordini.", "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 6 ore.", "Nota storica: ispirata al K-219, lo Yankee che il 3 ottobre 1986, a circa 1090 km dalle Bermuda, emerse dopo un'esplosione in un tubo dei missili; il rimorchio falli' e il battello affondo' il 6 ottobre. Morirono quattro uomini. Secondo i sovietici una collisione con l'USS Augusta; la US Navy e il comandante del K-219 lo hanno sempre negato. Nomi e unita' di fantasia." },
    en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SSBN TRAIL - WESTERN ATLANTIC", "10 October 1989, 0400Z. Theatre: Atlantic, on the abyssal plain. Peace. Autumn sea.", "A Soviet SSBN is heading down to her patrol sector. Get in her baffles, hold her and record her without being heard.", "Range: beyond 1000 yards. In case of any unusual event on the SSBN, report at once with a situation report and await orders.", "Rules of engagement: peacetime. Weapons hold. Time available: 6 hours.", "Historical note: inspired by K-219, the Yankee that on 3 October 1986, some 1,090 km from Bermuda, surfaced after an explosion in a missile tube; the tow failed and the boat sank on 6 October. Four men died. According to the Soviets it was a collision with USS Augusta; the US Navy and K-219's commander always denied it. Names and units are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [40] = { it = "COMSUBLANT vi richiama. L'equipaggio sovietico e' sul mercantile; il battello resta dove il mare lo tiene. Avete fatto quello che vi era chiesto: guardare, riferire, restare fuori.", en = "COMSUBLANT recalls you. The Soviet crew is aboard the freighter; the boat stays where the sea holds her. You did what was asked: watch, report, stay out." },
    [41] = { it = "Collisione con l'SSBN. La pattuglia finisce qui, e il rapporto lo scriveranno altri.", en = "Collision with the SSBN. The patrol ends here, and others will write the report." },
  },
  radio = {
    ordini = { da = "COMSUBLANT", a = "USS LOS ANGELES (SSN 688)", oggetto = "SSBN TRAIL - WESTERN ATLANTIC", classifica = "TOP SECRET", data = { giorno = 10, mese = 10, anno = 1989, ora_utc = 4 } },
    stato_iniziale = { roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, neutrali = { 7, 8, } },
    { id = "ORDINI", su_richiesta = true, precedenza = "flash", da = "COMSUBLANT", oggetto = "SOVIET SSBN EMERGENCY", classifica = "TOP SECRET",
      testo = { it = "RICEVUTO. SSBN SOVIETICO IN EMERGENZA IN SUPERFICIE. RESTATE NASCOSTI, MANTENETE IL CONTATTO A DISTANZA. NON AVVICINATEVI A MENO DI 5 MIGLIA DALLE UNITA' SOVIETICHE IN ARRIVO. NESSUNA INTERFERENZA. P-3 IN ARRIVO.", en = "ROGER. SOVIET SSBN IN EMERGENCY ON THE SURFACE. REMAIN COVERT, MAINTAIN CONTACT AT A DISTANCE. DO NOT CLOSE WITHIN 5 MILES OF ARRIVING SOVIET UNITS. NO INTERFERENCE. P-3 EN ROUTE." } },
    { id = "COLLISIONE", su_richiesta = true, precedenza = "immediate", da = "COMSUBLANT", oggetto = "SOVIET CLAIM", classifica = "SECRET",
      testo = { it = "FONTI SOVIETICHE ATTRIBUISCONO L'INCIDENTE A UNA COLLISIONE CON UN SOTTOMARINO AMERICANO. LA MARINA RESPINGE L'ACCUSA. MANTENETE IL SILENZIO SULL'ARGOMENTO.", en = "SOVIET SOURCES ATTRIBUTE THE CASUALTY TO A COLLISION WITH AN AMERICAN SUBMARINE. THE NAVY REJECTS THE CLAIM. MAINTAIN SILENCE ON THE SUBJECT." } },
    { id = "RICHIAMO", su_richiesta = true, precedenza = "immediate", da = "COMSUBLANT", oggetto = "RECALL", classifica = "TOP SECRET",
      testo = { it = "IL RIMORCHIO E' FALLITO. INTERROMPETE IL PEDINAMENTO E RIENTRATE. BEN FATTO.", en = "TOW HAS FAILED. BREAK OFF TRAIL AND RETURN. WELL DONE." } },
  },
  messaggi = {
    [1] = { it = "Sonar: transitorio forte dal contatto, Comandante. Poi aria ad alta pressione: sta dando aria a tutte le casse.", en = "Sonar: loud transient from the contact, Captain. Then high-pressure air: she is blowing all tanks." },
    [2] = { it = "Sonar: transitorio forte dal contatto, Comandante. Poi aria ad alta pressione: sta dando aria a tutte le casse.", en = "Sonar: loud transient from the contact, Captain. Then high-pressure air: she is blowing all tanks." },
    [3] = { it = "Sonar: il contatto e' in superficie e fermo. Le macchine tacciono. Si sentono solo pompe e il mare sullo scafo.", en = "Sonar: the contact is on the surface and stopped. Her machinery is silent. Only pumps and the sea on her hull." },
    [4] = { it = "Sonar: un'elica vicino al contatto, poi un cavo in tensione. Lo stanno prendendo a rimorchio.", en = "Sonar: a screw close to the contact, then a cable under strain. They are taking her in tow." },
    [5] = { it = "Sonar: il cavo si e' spezzato. Il mercantile ha fermato le macchine e mette le lance in mare.", en = "Sonar: the cable has parted. The freighter has stopped engines and is putting boats in the water." },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_S25_OMBRA_DEL_BOOMER", titolo = { it = "Ombra del boomer", en = "Boomer's Shadow" }, autore = "Libreria di Melchior 688",
    teatro = "celtica", alfa = false, centro = { lat = 48.25, lon = -9.75 },
    data = { giorno = 10, mese = 10, anno = 1989 }, ora_utc = 4, meteo_realistico = true,
    situazione = "pace", paranoia = 2, difficolta = 3, durata_s = 21600, libreria = 2,
    carico = { dotazione = { { "mk48", 22 }, { "harpoon", 2 }, { "esca", 6 }, }, deposito = { { "mk48", 21 }, { "harpoon", 2 }, { "esca", 6 }, }, tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "", flag = 9, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = -74044.138354052819, y = -103351.02947634054 }, quota = 120, rotta = 120.00000000000001, velocita = 5, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "delta3", categoria = "sottomarino", parte = "urss", ruolo = "SSBN in pattuglia (al posto dello Yankee I)", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -74044.138354052819, y = -95351.02947634054 }, quota = 80, rotta = 120.00000000000001, velocita = 4, punti = { { x = -56723.630278364049, y = -105351.02947634054 }, { x = -56723.630278364049, y = -101351.02947634054 }, { x = -52723.630278364049, y = -105351.02947634054 }, { x = -56723.630278364049, y = -109351.02947634054 }, { x = -60723.630278364049, y = -105351.02947634054 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 4, classe = "mercantile", categoria = "nave", parte = "urss", ruolo = "mercantile sovietico deviato sull'emergenza", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -33742.29698479471, y = -124634.65776693673 }, quota = 0, rotta = 310, velocita = 14, punti = { { x = -56723.630278364049, y = -105351.02947634054 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "superficie",
        parametri = { } },
      { id = 5, classe = "rimorchiatore", categoria = "nave", parte = "urss", ruolo = "rimorchiatore della flotta", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -13422.360089142123, y = -80351.02947634054 }, quota = 0, rotta = 240, velocita = 12, punti = { { x = -56723.630278364049, y = -105351.02947634054 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "rimorchiatore",
        parametri = { } },
      { id = 6, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "P-3C che viene a guardare", flag = 11, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -56723.630278364049, y = -65351.02947634054 }, quota = 0, rotta = 180, velocita = 250, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "orion",
        parametri = { } },
      { id = 7, classe = "mercantile", categoria = "nave", parte = "neutrale", ruolo = "traffico atlantico", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -44752.925261965647, y = -72461.78774883374 }, quota = 0, rotta = 110, velocita = 12, punti = { { x = 21025.558193047938, y = -96403.197781630559 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 8, classe = "portarinfuse", categoria = "nave", parte = "neutrale", ruolo = "traffico atlantico", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -68694.335294762452, y = -138240.27120384734 }, quota = 0, rotta = 290, velocita = 12, punti = { { x = -134472.81874977605, y = -114298.86117105056 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
    },
    obiettivi = {
      { id = "tenuto", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Tenere il contatto sull'SSBN (almeno un'ora)", en = "Hold contact on the SSBN (at least an hour)" }, indice = 4 },
      { id = "occulto", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Non farsi classificare dall'SSBN", en = "Avoid being classified by the SSBN" }, indice = 5 },
      { id = "riferito", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riferire l'emergenza entro 30 minuti (rapporto di situazione)", en = "Report the emergency within 30 minutes (situation report)" }, indice = 6 },
      { id = "lontano", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Restare a 5 miglia dalle navi sovietiche", en = "Stay 5 miles from the Soviet ships" }, indice = 7 },
      { id = "iarde", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = false, colpire = true, testo = { it = "Mai a meno di 1000 iarde dall'SSBN", en = "Never inside 1000 yards of the SSBN" }, indice = 8 },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 6, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = { 3 }, zona = { x = -56723.630278364049, y = -105351.02947634054 }, raggio = 15000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: PEDINAMENTO DI UN SSBN - ATLANTICO OCCIDENTALE", "10 ottobre 1989, ore 0400Z. Teatro: Atlantico, sulla piana abissale. Pace. Mare d'autunno.", "Un SSBN sovietico scende verso il suo settore di pattuglia. Mettetevi in coda, tenetelo e registratelo senza farvi sentire.", "Distanza: oltre le 1000 iarde. In caso di qualsiasi evento anomalo dell'SSBN, riferite subito con un rapporto di situazione e attendete ordini.", "Regole d'ingaggio: tempo di pace. Armi trattenute. Tempo a disposizione: 6 ore.", "Nota storica: ispirata al K-219, lo Yankee che il 3 ottobre 1986, a circa 1090 km dalle Bermuda, emerse dopo un'esplosione in un tubo dei missili; il rimorchio falli' e il battello affondo' il 6 ottobre. Morirono quattro uomini. Secondo i sovietici una collisione con l'USS Augusta; la US Navy e il comandante del K-219 lo hanno sempre negato. Nomi e unita' di fantasia.", }, en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SSBN TRAIL - WESTERN ATLANTIC", "10 October 1989, 0400Z. Theatre: Atlantic, on the abyssal plain. Peace. Autumn sea.", "A Soviet SSBN is heading down to her patrol sector. Get in her baffles, hold her and record her without being heard.", "Range: beyond 1000 yards. In case of any unusual event on the SSBN, report at once with a situation report and await orders.", "Rules of engagement: peacetime. Weapons hold. Time available: 6 hours.", "Historical note: inspired by K-219, the Yankee that on 3 October 1986, some 1,090 km from Bermuda, surfaced after an explosion in a missile tube; the tow failed and the boat sank on 6 October. Four men died. According to the Soviets it was a collision with USS Augusta; the US Navy and K-219's commander always denied it. Names and units are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "pattuglia", nome = { it = "il settore di pattuglia dell'SSBN", en = "the SSBN's patrol sector" }, cerchio = { x = -56723.630278364049, y = -105351.02947634054, r = 6000 } },
      { id = "luogo", nome = { it = "il punto dell'emergenza", en = "the emergency position" }, cerchio = { x = -56723.630278364049, y = -105351.02947634054, r = 5000 } },
    },
    regole = {
      { id = "partenza", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "variabile", codice = 0, unita = -1, indice = 8, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto", una_volta = true,
        quando = {
          { tipo = "contatto_su", valore = 60, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "mille_iarde", una_volta = true,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 914, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 8, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "classificati", una_volta = true,
        quando = {
          { tipo = "unita_classifica", valore = 1, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "emergenza", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 7200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "unita_in_zona", valore = 0, unita = 3, indice = 0, zona = "pattuglia", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 4, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 10, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "quota", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: transitorio forte dal contatto, Comandante. Poi aria ad alta pressione: sta dando aria a tutte le casse.", en = "Sonar: loud transient from the contact, Captain. Then high-pressure air: she is blowing all tanks." } },
        } },
      { id = "emergenza_tardi", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 12600, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 10, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "quota", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: transitorio forte dal contatto, Comandante. Poi aria ad alta pressione: sta dando aria a tutte le casse.", en = "Sonar: loud transient from the contact, Captain. Then high-pressure air: she is blowing all tanks." } },
        } },
      { id = "superficie", una_volta = true,
        quando = {
          { tipo = "unita_in_superficie", valore = 0, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "avaria", codice = 0, unita = 3, indice = 0, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: il contatto e' in superficie e fermo. Le macchine tacciono. Si sentono solo pompe e il mare sullo scafo.", en = "Sonar: the contact is on the surface and stopped. Her machinery is silent. Only pumps and the sea on her hull." } },
        } },
      { id = "orologio", una_volta = false,
        quando = {
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 11, valore = 10, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "resta_su", una_volta = false,
        quando = {
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "quota", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "riferito", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 0, unita = -1, indice = 10, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 1810, unita = -1, indice = 11, zona = "", regola = "", confronto = "<" },
          { tipo = "giocatore_ha_trasmesso", valore = 7200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", trasmissione = "situazione" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "ORDINI", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "ordini_comunque", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 2690, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 6, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "ORDINI", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "orion", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 2390, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "rimorchiatore", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 3590, unita = -1, indice = 11, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "navi_3", una_volta = true,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 9260, unita = 4, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "navi_4", una_volta = true,
        quando = {
          { tipo = "giocatore_vicino_a", valore = 9260, unita = 5, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "rimorchio", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 4, indice = 0, zona = "luogo", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
          { tipo = "unita_in_avaria", valore = 0, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 10, valore = 2, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "rimorchia", codice = 0, unita = 4, indice = 0, valore = 300, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 3, bersaglio = 3 },
          { tipo = "vai_a", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -4762.1060512977347, y = -75351.02947634054 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 3 },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: un'elica vicino al contatto, poi un cavo in tensione. Lo stanno prendendo a rimorchio.", en = "Sonar: a screw close to the contact, then a cable under strain. They are taking her in tow." } },
        } },
      { id = "orologio_rimorchio", una_volta = false,
        quando = {
          { tipo = "variabile", valore = 1, unita = -1, indice = 10, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 12, valore = 10, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "rimorchio_mollato", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 2990, unita = -1, indice = 12, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 10, valore = 3, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "molla_rimorchio", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: il cavo si e' spezzato. Il mercantile ha fermato le macchine e mette le lance in mare.", en = "Sonar: the cable has parted. The freighter has stopped engines and is putting boats in the water." } },
        } },
      { id = "versione_sovietica", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 3, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 8, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "COLLISIONE", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "richiamo", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 3, unita = -1, indice = 10, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "radio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "RICHIAMO", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fine_pattuglia", una_volta = true,
        quando = {
          { tipo = "regola_scattata", valore = 0, unita = -1, indice = 0, zona = "", regola = "richiamo", confronto = "=" },
          { tipo = "variabile", valore = 3290, unita = -1, indice = 12, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "COMSUBLANT vi richiama. L'equipaggio sovietico e' sul mercantile; il battello resta dove il mare lo tiene. Avete fatto quello che vi era chiesto: guardare, riferire, restare fuori.", en = "COMSUBLANT recalls you. The Soviet crew is aboard the freighter; the boat stays where the sea holds her. You did what was asked: watch, report, stay out." } },
        } },
      { id = "collisione", una_volta = true,
        quando = {
          { tipo = "evento", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", evento = "collisione" },
        },
        allora = {
          { tipo = "fine", codice = 41, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "fallimento", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Collisione con l'SSBN. La pattuglia finisce qui, e il rapporto lo scriveranno altri.", en = "Collision with the SSBN. The patrol ends here, and others will write the report." } },
        } },
    },
    radio = { ordini = { da = "COMSUBLANT", a = "USS LOS ANGELES (SSN 688)", oggetto = "SSBN TRAIL - WESTERN ATLANTIC", classifica = "TOP SECRET" }, roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, messaggi = {
      { id = "ORDINI", dopo_s = 0, su_richiesta = true, precedenza = "flash", da = "COMSUBLANT", oggetto = "SOVIET SSBN EMERGENCY", classifica = "TOP SECRET", testo = { it = "RICEVUTO. SSBN SOVIETICO IN EMERGENZA IN SUPERFICIE. RESTATE NASCOSTI, MANTENETE IL CONTATTO A DISTANZA. NON AVVICINATEVI A MENO DI 5 MIGLIA DALLE UNITA' SOVIETICHE IN ARRIVO. NESSUNA INTERFERENZA. P-3 IN ARRIVO.", en = "ROGER. SOVIET SSBN IN EMERGENCY ON THE SURFACE. REMAIN COVERT, MAINTAIN CONTACT AT A DISTANCE. DO NOT CLOSE WITHIN 5 MILES OF ARRIVING SOVIET UNITS. NO INTERFERENCE. P-3 EN ROUTE." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "COLLISIONE", dopo_s = 0, su_richiesta = true, precedenza = "immediate", da = "COMSUBLANT", oggetto = "SOVIET CLAIM", classifica = "SECRET", testo = { it = "FONTI SOVIETICHE ATTRIBUISCONO L'INCIDENTE A UNA COLLISIONE CON UN SOTTOMARINO AMERICANO. LA MARINA RESPINGE L'ACCUSA. MANTENETE IL SILENZIO SULL'ARGOMENTO.", en = "SOVIET SOURCES ATTRIBUTE THE CASUALTY TO A COLLISION WITH AN AMERICAN SUBMARINE. THE NAVY REJECTS THE CLAIM. MAINTAIN SILENCE ON THE SUBJECT." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
      { id = "RICHIAMO", dopo_s = 0, su_richiesta = true, precedenza = "immediate", da = "COMSUBLANT", oggetto = "RECALL", classifica = "TOP SECRET", testo = { it = "IL RIMORCHIO E' FALLITO. INTERROMPETE IL PEDINAMENTO E RIENTRATE. BEN FATTO.", en = "TOW HAS FAILED. BREAK OFF TRAIL AND RETURN. WELL DONE." }, autentico = true, roe = { }, se_evento = "", se_ricevuto = "", se_unita_colpita = {  }, annulla = "" },
    } },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9, giocatore = true,
        posizione = { x = -74044.1, y = -103351 }, quota_m = 120, rotta = 120, velocita_kn = 5,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "delta3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -74044.1, y = -95351 }, quota_m = 80, rotta = 120, velocita_kn = 4,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 13, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -56723.6, y = -105351 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_3_p2" } },   -- SSBN in pattuglia (al posto dello Yankee I)
      { id = 4, classe = "mercantile", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = -33742.3, y = 1955365.3 }, quota_m = 0, rotta = 310, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile sovietico deviato sull'emergenza, di riserva: entra con la regola superficie
      { id = 5, classe = "rimorchiatore", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = -13422.4, y = 2019649 }, quota_m = 0, rotta = 240, velocita_kn = 0,
        modello = { massima_kn = 13, virata_gradi_s = 2, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 16, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- rimorchiatore della flotta, di riserva: entra con la regola rimorchiatore
      { id = 6, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 11,
        posizione = { x = -56723.6, y = 2054649 }, quota_m = 0, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- P-3C che viene a guardare, di riserva: entra con la regola orion
      { id = 7, classe = "mercantile", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = -44752.9, y = -72461.8 }, quota_m = 0, rotta = 110, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 21025.6, y = -96403.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- traffico atlantico
      { id = 8, classe = "portarinfuse", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = -68694.3, y = -138240.3 }, quota_m = 0, rotta = 290, velocita_kn = 12,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -134472.8, y = -114298.9 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- traffico atlantico
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 21600, unita = 0, gestore = "R_tempo" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_1" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_3" },
      { dopo_s = 7200, unita = 3, gestore = "R_regola_4" },
      { dopo_s = 12600, unita = 0, gestore = "R_regola_5" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_6" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_7" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_8" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_9" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_10" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_11" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_12" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_13" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_14" },
      { dopo_s = 10, unita = 4, gestore = "R_regola_15" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_16" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_17" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_18" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_19" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_20" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_21" },
    },
    variabili = { [10] = 0, [11] = 0 },
    },
  },
}

-- kit del 688 (data/motore/missioni_rapide/, dalle missioni originali)
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

-- kit dell'Alfa: i gestori di lancio usati dall'IA
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


-- Costanti della missione: regole d'ingaggio, paranoia (2) e difficolta' (3)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 12          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [3] = { parte = "urss", crociera = 0.104, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 4, distanza = 2 },
  [4] = { parte = "urss", crociera = 0.766, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 3, distanza = 2 },
  [5] = { parte = "urss", crociera = 1, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 3, distanza = 2 },
  [6] = { parte = "usa", crociera = 0.829, comportamento = "R_aereo", rotta = false, profonda = 0,
      paranoia = 2, salve = 2, portata = 3, distanza = 2 },
  [7] = { parte = "neutrale", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 3, distanza = 2 },
  [8] = { parte = "neutrale", crociera = 0.875, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 3, distanza = 2 },
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
script.R_3_p1 = function(u)
  m688.vai_a(u, -56723.6, -105351, 30, "R_3_p2")
  m688.salva_modello(u)
end
script.R_3_p2 = function(u)
  m688.vai_a(u, -56723.6, -101351, 30, "R_3_p3")
  m688.salva_modello(u)
end
script.R_3_p3 = function(u)
  m688.vai_a(u, -52723.6, -105351, 30, "R_3_p4")
  m688.salva_modello(u)
end
script.R_3_p4 = function(u)
  m688.vai_a(u, -56723.6, -109351, 30, "R_3_p5")
  m688.salva_modello(u)
end
script.R_3_p5 = function(u)
  m688.vai_a(u, -60723.6, -105351, 30, "")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, -56723.6, -105351, 30, "")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, -56723.6, -105351, 30, "")
  m688.salva_modello(u)
end
script.R_7_p1 = function(u)
  m688.vai_a(u, 21025.6, -96403.2, 30, "")
  m688.salva_modello(u)
end
script.R_8_p1 = function(u)
  m688.vai_a(u, -134472.8, -114298.9, 30, "")
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
  ["pattuglia"] = { -56723.6, -105351, 6000 },
  ["luogo"] = { -56723.6, -105351, 5000 },
}
local function su_unita(u, id, domanda, ...)
  m688.seleziona(u, id)
  local r = domanda(u, ...)
  m688.seleziona(u, u)
  return r
end
-- partenza
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) then
    m688.variabile(u, 200, 1)
    m688.variabile(u, 305, 1)
    m688.variabile(u, 307, 1)
    m688.variabile(u, 308, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- tenuto
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) and m688.contatto_da(u, 3, 60) then
    m688.variabile(u, 201, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- mille_iarde
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) and (m688.distanza_da(u, 3) or 1e18) < 914 then
    m688.variabile(u, 202, 1)
    m688.variabile(u, 308, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- classificati
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) and su_unita(u, 3, m688.classificato, 0, 1) then
    m688.variabile(u, 203, 1)
    m688.variabile(u, 305, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- emergenza
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) and m688.in_cerchio(u, table.unpack(ZONE["pattuglia"])) and m688.variabile_uguale(u, 304, 1) and m688.variabile_uguale(u, 310, 0) then
    m688.variabile(u, 204, 1)
    m688.variabile(u, 310, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.nessun_comportamento(u)
      m688.salva_modello(u)
      UNITA[3].comportamento = ""
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.quota(u, 0, 1)
      m688.salva_modello(u)
    end
    m688.messaggio_al_giocatore(u, 1001)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- emergenza_tardi
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) and m688.variabile_uguale(u, 310, 0) then
    m688.variabile(u, 205, 1)
    m688.variabile(u, 310, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.nessun_comportamento(u)
      m688.salva_modello(u)
      UNITA[3].comportamento = ""
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.quota(u, 0, 1)
      m688.salva_modello(u)
    end
    m688.messaggio_al_giocatore(u, 1002)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end
-- superficie
script.R_regola_6 = function(u)
  if m688.variabile_uguale(u, 206, 0) and su_unita(u, 3, m688.in_superficie) and m688.variabile_uguale(u, 310, 1) then
    m688.variabile(u, 206, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.avaria(u)
    end
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.posizione(u, -33742.3, -124634.7)
      m688.rotta_ora(u, 310)
      m688.velocita_ora(u, 14)
      script.R_4_p1(u)
    end
    m688.messaggio_al_giocatore(u, 1003)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_6")
end
-- orologio
script.R_regola_7 = function(u)
  if true and m688.variabile_maggiore(u, 310, 0) then
    m688.variabile(u, 207, 1)
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
  m688.dopo(u, 10, "R_regola_7")
end
-- resta_su
script.R_regola_8 = function(u)
  if true and m688.variabile_maggiore(u, 310, 0) then
    m688.variabile(u, 208, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.quota(u, 0, 1)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
  end
  m688.dopo(u, 10, "R_regola_8")
end
-- riferito
script.R_regola_9 = function(u)
  if m688.variabile_uguale(u, 209, 0) and m688.variabile_maggiore(u, 310, 0) and m688.variabile_minore(u, 311, 1810) and m688.trasmesso(u, "situazione", 7200) then
    m688.variabile(u, 209, 1)
    m688.variabile(u, 306, 1)
    m688.radio(u, "ORDINI")
    do return end
  end
  m688.dopo(u, 10, "R_regola_9")
end
-- ordini_comunque
script.R_regola_10 = function(u)
  if m688.variabile_uguale(u, 210, 0) and m688.variabile_maggiore(u, 311, 2690) and m688.variabile_uguale(u, 306, 0) then
    m688.variabile(u, 210, 1)
    m688.radio(u, "ORDINI")
    do return end
  end
  m688.dopo(u, 10, "R_regola_10")
end
-- orion
script.R_regola_11 = function(u)
  if m688.variabile_uguale(u, 211, 0) and m688.variabile_maggiore(u, 311, 2390) then
    m688.variabile(u, 211, 1)
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.posizione(u, -56723.6, -65351)
      m688.rotta_ora(u, 180)
      m688.velocita_ora(u, 250)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_11")
end
-- rimorchiatore
script.R_regola_12 = function(u)
  if m688.variabile_uguale(u, 212, 0) and m688.variabile_maggiore(u, 311, 3590) then
    m688.variabile(u, 212, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.posizione(u, -13422.4, -80351)
      m688.rotta_ora(u, 240)
      m688.velocita_ora(u, 12)
      script.R_5_p1(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_12")
end
-- navi_3
script.R_regola_13 = function(u)
  if m688.variabile_uguale(u, 213, 0) and (m688.distanza_da(u, 4) or 1e18) < 9260 then
    m688.variabile(u, 213, 1)
    m688.variabile(u, 307, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_13")
end
-- navi_4
script.R_regola_14 = function(u)
  if m688.variabile_uguale(u, 214, 0) and (m688.distanza_da(u, 5) or 1e18) < 9260 then
    m688.variabile(u, 214, 1)
    m688.variabile(u, 307, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_14")
end
-- rimorchio
script.R_regola_15 = function(u)
  if m688.variabile_uguale(u, 215, 0) and m688.in_cerchio(u, table.unpack(ZONE["luogo"])) and m688.variabile_uguale(u, 310, 1) and su_unita(u, 3, m688.in_avaria) then
    m688.variabile(u, 215, 1)
    m688.variabile(u, 310, 2)
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.rimorchia(u, 3, 3, 300)
    end
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.vai_a(u, -4762.1, -75351, 30, "")
      m688.telegrafo(u, 0.164)
      m688.salva_modello(u)
      UNITA[4].rotta = true
      UNITA[4].crociera = 0.164
    end
    m688.messaggio_al_giocatore(u, 1004)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_15")
end
-- orologio_rimorchio
script.R_regola_16 = function(u)
  if true and m688.variabile_maggiore(u, 310, 1) then
    m688.variabile(u, 216, 1)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
    m688.variabile_piu(u, 312)
  end
  m688.dopo(u, 10, "R_regola_16")
end
-- rimorchio_mollato
script.R_regola_17 = function(u)
  if m688.variabile_uguale(u, 217, 0) and m688.variabile_maggiore(u, 312, 2990) then
    m688.variabile(u, 217, 1)
    m688.variabile(u, 310, 3)
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.molla_rimorchio(u)
    end
    m688.messaggio_al_giocatore(u, 1005)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_17")
end
-- versione_sovietica
script.R_regola_18 = function(u)
  if m688.variabile_uguale(u, 218, 0) and m688.variabile_uguale(u, 310, 3) and m688.variabile_uguale(u, 308, 0) then
    m688.variabile(u, 218, 1)
    m688.radio(u, "COLLISIONE")
    do return end
  end
  m688.dopo(u, 10, "R_regola_18")
end
-- richiamo
script.R_regola_19 = function(u)
  if m688.variabile_uguale(u, 219, 0) and m688.variabile_uguale(u, 310, 3) then
    m688.variabile(u, 219, 1)
    m688.radio(u, "RICHIAMO")
    do return end
  end
  m688.dopo(u, 10, "R_regola_19")
end
-- fine_pattuglia
script.R_regola_20 = function(u)
  if m688.variabile_uguale(u, 220, 0) and m688.variabile_uguale(u, 219, 1) and m688.variabile_maggiore(u, 312, 3290) then
    m688.variabile(u, 220, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_20")
end
-- collisione
script.R_regola_21 = function(u)
  if m688.variabile_uguale(u, 221, 0) and m688.evento(u, "collisione", 0) then
    m688.variabile(u, 221, 1)
    m688.messaggio(u, 141)
    do return end
  end
  m688.dopo(u, 10, "R_regola_21")
end

return missione
