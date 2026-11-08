-- SCENE2B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE2B",
  missione = 2,
  titolo = { it = "Scrollatevelo di dosso", en = "SHAKE 'EM" },
  lato = "urss",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 274.32, 457.2 } },
  dotazione = { type53 = 8, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_045c" },
    esca = { gestore = "L_0526" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0589",
    telegrafo = "L_059b",
    quota = "L_05a0",
    timone = "L_05a7",
    pilota_automatico_off = "L_0603",
    periscopio = "L_060b",
    carica_tubo_1 = "L_05af",
    carica_tubo_2 = "L_05b8",
    carica_tubo_3 = "L_05c1",
    carica_tubo_4 = "L_05ca",
    carica_tubo_5 = "L_05d3",
    carica_tubo_6 = "L_05dc",
    carica_tubo_7 = "L_05e5",
    schiera_cala = "L_060f",
    schiera_recupera = "L_061f",
    schiera_persa = "L_062b",
    designa_bersaglio = "L_0635",
    posizione = "L_063a",
    antenna = "L_0615",
    guida_e0 = "L_055c",
    guida_e1 = "L_0561",
    guida_e2 = "L_056b",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.50390625, probabilita_originale = "129/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -1720.48, y = -15720.48 }, originale = { x = 4832, y = 2912 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 250, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -973.76, y = -17960.64 }, originale = { x = 4864, y = 2816 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 548.64, integrita = 180, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -227.04, y = -20947.52 }, { x = 2013.12, y = -20947.52 }, { x = 3506.56, y = -24681.12 }, { x = -973.76, y = -24681.12 }, { x = 2759.84, y = -29908.16 }, { x = 7986.88, y = -38122.08 }, { x = 10973.76, y = -32148.32 }, { x = 22174.56, y = -36628.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_016d", reazione = nil, arrivo = "L_01a8" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0177" }, { dopo_s = 120, unita = 0, gestore = "L_02f0" }, { dopo_s = 1200, unita = 0, gestore = "L_02fe" }, { dopo_s = 30, unita = 1, gestore = "L_0326" }, { dopo_s = 120, unita = 2, gestore = "L_010e" } },
      variabili = { [1] = 0, [2] = 0 },
    },
    { probabilita = 0.49609375, probabilita_originale = "127/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 17694.24, y = -34388.48 }, originale = { x = 5664, y = 2112 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 250, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 15454.08, y = -33641.76 }, originale = { x = 5568, y = 2144 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 548.64, integrita = 180, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 9480.32, y = -30654.88 }, { x = 9480.32, y = -30654.88 }, { x = 7986.88, y = -38122.08 }, { x = 2759.84, y = -29908.16 }, { x = 4253.28, y = -25427.84 }, { x = 519.68, y = -24681.12 }, { x = -973.76, y = -17960.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_016d", reazione = nil, arrivo = "L_0219" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_039d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0177" }, { dopo_s = 120, unita = 0, gestore = "L_02f0" }, { dopo_s = 1200, unita = 0, gestore = "L_02fe" }, { dopo_s = 30, unita = 1, gestore = "L_0326" }, { dopo_s = 120, unita = 2, gestore = "L_010e" } },
      variabili = { [1] = 0, [2] = 0 },
    },
  },
  briefing = {
    it = { "La Marina sovietica si congratula con il Leninskii Komsomol per i successi finora ottenuti nel controllo del sottomarino americano. Stiamo cercando di inviarvi rinforzi. Il Leninskii Komsomol continui a seguire gli americani fino a nuovo ordine, restando entro 5 miglia per poter aprire il fuoco al comando. Kapitan, non disonori il suo Paese e la sua Marina perdendo la preda negli stretti canali delle Fær Øer. Non è autorizzato a lanciare siluri se non viene attaccato." },
    en = { "THE SOVIET NAVY CONGRATULATES THE LENINSKII KOMSOMOL ON HER SUCCESS SO FAR IN MONITORING THE AMERICAN SUBMARINE. WE ARE CURRENTLY TRYING TO SEND YOU SUPPORT. THE LENINSKII KOMSOMOL IS DIRECTED TO CONTINUE TRACKING THE AMERICANS UNTIL FURTHER NOTICE. STAY WITHIN 5 MILES SO THAT YOU MAY FIRE ON COMMAND. KAPITAN, DO NOT DISGRACE YOUR COUNTRY AND NAVY BY LOSING YOUR QUARRY IN THE NARROW CHANNELS OF THE FAEROE ISLANDS. YOU ARE NOT AUTHORIZED TO LAUNCH TORPEDOES UNLESS FIRED UPON." },
  },
  esiti = {
    [1] = { it = "CODICE: 2-1-BKB\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento nei pressi dell'arcipelago delle Fær Øer. Non dimenticheremo mai il coraggio e il valore di questi giovani valorosi che hanno dato la vita per la Patria nel nome del Soviet Supremo!", en = "CODE: 2-1-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE PATROLLING NEAR THE FAEROE ISLAND CHAIN. WE SHALL NEVER FORGET THE COURAGE AND VALOR OF THESE BRAVE YOUNG MEN WHO GAVE UP THEIR LIVES FOR THE MOTHERLAND IN THE NAME OF THE SUPREME SOVIET!" },
    [2] = { it = "CODICE: 2-2-BKB\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento nei pressi dell'arcipelago delle Fær Øer. Non dimenticheremo mai il coraggio e il valore di questi giovani valorosi che hanno dato la vita per la Patria nel nome del Soviet Supremo! La Pravda ha dichiarato che l'attacco al Leninskii Komsomol è stato un'esecuzione premeditata da parte dell'Occidente imperialista e una dimostrazione della sua politica di aggressione. Queste azioni non resteranno impunite!", en = "CODE: 2-2-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE PATROLLING NEAR THE FAEROE ISLAND CHAIN. WE SHALL NEVER FORGET THE COURAGE AND VALOR OF THESE BRAVE YOUNG MEN WHO GAVE UP THEIR LIVES FOR THE MOTHERLAND IN THE NAME OF THE SUPREME SOVIET! PRAVDA HAS STATED THAT THE ATTACK OF THE LENINSKII KOMSOMOL WAS A PREMEDITATED EXECUTION BY THE IMPERIALIST WEST AND A DEMONSTRATION OF THEIR POLICIES OF AGGRESSION. THESE ACTIONS SHALL NOT BE OVERLOOKED!" },
    [3] = { it = "CODICE: 2-3-BKB\nIl Leninskii Komsomol ha gravemente oltrepassato i propri poteri compiendo un attacco non provocato in acque internazionali! Se non emergeranno prove di un'aggressione americana, l'ultima nave che questo Kapitan vedrà sarà un traghetto nell'Arcipelago Gulag! Lo zampolit è incaricato di assumere il comando e di far emergere immediatamente il sottomarino per il ricongiungimento con il Cheliabinskaia Komsomolka!", en = "CODE: 2-3-BKB\nTHE LENINSKII KOMSOMOL HAS SEVERELY OVERSTEPPED AUTHORITY BY MAKING AN UNPROVOKED ATTACK IN INTERNATIONAL WATERS! UNLESS THERE IS EVIDENCE OF AGGRESSION ON THE PART OF THE AMERICANS, THE LAST SHIP THIS KAPITAN WILL EVER SEE WILL BE A FERRY BOAT IN THE GULAG ARCHIPELAGO! THE ZAMPOLIT IS INSTRUCTED TO TAKE COMMAND AND SURFACE THE SUBMARINE IMMEDIATELY FOR RENDEZVOUS WITH THE CHELIABINSKAIA KOMSOMOLKA!" },
    [4] = { it = "CODICE: 2-4-BKB\nKapitan, lei ha ostacolato la lotta del socialismo e gonfiato l'ego imperialista lasciandosi sfuggire quei porci. Rientri in porto, e in fretta.", en = "CODE: 2-4-BKB\nKAPITAN, YOU HAVE HINDERED THE FIGHT OF SOCIALISM AND INFLATED THE IMPERIALIST EGO BY ALLOWING THE SWINE TO SLIP FROM YOUR GRASP. RETURN TO PORT QUICKLY." },
    [5] = { it = "CODICE: 2-5-BKB\nKapitan, aspettava forse che gli americani accostassero per invitarla a bordo a vodka e caviale? Lei ha ostacolato la lotta del socialismo e gonfiato l'ego imperialista lasciandosi sfuggire quei porci. Rientri in porto, e in fretta: mi divertirò a stabilire la sua punizione.", en = "CODE: 2-5-BKB\nKAPITAN, WERE YOU WAITING FOR THE AMERICANS TO PULL OVER AND INVITE YOU IN FOR VODKA AND CAVIAR? YOU HAVE HINDERED THE FIGHT OF SOCIALISM AND INFLATED THE IMPERIALIST EGO BY ALLOWING THE SWINE TO SLIP FROM YOUR GRASP. RETURN TO PORT QUICKLY, I WILL ENJOY PRESCRIBING YOUR PUNISHMENT." },
    [6] = { it = "CODICE: 2-6-BKB\nLa Marina sovietica si congratula con il Leninskii Komsomol per aver portato a termine con successo la sua missione. Il Kapitan fa onore al socialismo e alla nostra Marina! Il cambio è in posizione e pronto a proseguire la sorveglianza. Salutiamo il Leninskii Komsomol e chiediamo che il sottomarino rientri immediatamente in porto!", en = "CODE: 2-6-BKB\nTHE SOVIET NAVY CONGRATULATES THE LENINSKII KOMSOMOL ON THE SUCCESSFUL COMPLETION OF HER MISSION. THE KAPITAN IS A CREDIT TO SOCIALISM AND OUR NAVY! RELIEF IS IN PLACE AND READY TO CONTINUE SURVEILLANCE. WE SALUTE THE LENINSKII KOMSOMOL AND REQUEST THAT THE SUBMARINE RETURN TO PORT IMMEDIATELY!" },
    [7] = { it = "CODICE: 2-7-BKB\nKapitan, lei non è riuscito a mantenere una posizione di tiro sul sottomarino americano. Aspettava forse che gli americani accostassero per invitarla a bordo a vodka e caviale? Lei ha ostacolato la lotta del socialismo e gonfiato l'ego imperialista lasciandosi sfuggire quei porci. Rientri in porto, e in fretta: mi divertirò a stabilire la sua punizione.", en = "CODE: 2-7-BKB\nKAPITAN, YOU FAILED TO MAINTAIN A FIRING POSITION ON THE AMERICAN SUB. WERE YOU WAITING FOR THE AMERICANS TO PULL OVER AND INVITE YOU IN FOR VODKA AND CAVIAR? YOU HAVE HINDERED THE FIGHT OF SOCIALISM AND INFLATED THE IMPERIALIST EGO BY ALLOWING THE SWINE TO SLIP FROM YOUR GRASP. RETURN TO PORT QUICKLY, I WILL ENJOY PRESCRIBING YOUR PUNISHMENT." },
    [19] = { it = "CODICE: 2-19-BKB\nIl Kapitan del Leninskii Komsomol è responsabile della morte di uomini innocenti. Il Kapitan è dichiarato colpevole di sovversione contro lo Stato, e lo zampolit assumerà il controllo del sottomarino. Non mi dispiacerebbe scoprire che questo Kapitan ha avuto un incidente lento e doloroso, e che non è sopravvissuto al rientro in porto.", en = "CODE: 2-19-BKB\nTHE KAPITAN OF THE LENINSKII KOMSOMOL IS RESPONSIBLE FOR THE DEATH OF INNOCENT MEN. THE KAPITAN IS HEREBY FOUND GUILTY OF SUBVERSION AGAINST THE STATE AND THE ZAMPOLIT WILL ASSUME CONTROL OF THE SUBMARINE. I WOULD NOT BE UNHAPPY TO FIND THAT THIS KAPITAN HAD A SLOW, PAINFUL ACCIDENT AND DID NOT SURVIVE THE RETURN TO PORT." },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 6, spostamento_max_m = 1984.5947, non_risolte = 0 },
  script = script,
}

script.L_010e = function(u)
  ::L_010e::
  m688.riprendi_navigazione(u)
  m688.schiera_trainata(u, 0)
  m688.telegrafo(u, 1.0547)
  m688.nuovo_caso(u)
  m688.quota(u, 45.72, 0.3125)
  if m688.caso(u, 0.2539) then goto L_0135 end
  m688.quota(u, 152.4, 0.3125)
  if m688.caso(u, 0.5039) then goto L_0135 end
  m688.quota(u, 304.8, 0.3125)
  if m688.caso(u, 0.7539) then goto L_0135 end
  m688.quota(u, 548.64, 0.3125)
  ::L_0135::
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_013c")
  do return end
end

script.L_013c = function(u)
  ::L_013c::
  m688.schiera_trainata(u, 1)
  m688.telegrafo(u, 0)
  m688.nuovo_caso(u)
  m688.quota(u, 45.72, 0.3125)
  if m688.caso(u, 0.2539) then goto L_0162 end
  m688.quota(u, 152.4, 0.3125)
  if m688.caso(u, 0.5039) then goto L_0162 end
  m688.quota(u, 304.8, 0.3125)
  if m688.caso(u, 0.7539) then goto L_0162 end
  m688.quota(u, 548.64, 0.3125)
  ::L_0162::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 160, "L_010e")
  do return end
end

script.L_016d = function(u)
  ::L_016d::
  if m688.bersaglio_id(u, 0) then goto L_0185 end
  ::L_0172::
  m688.comportamento(u, "L_016d")
  m688.salva_modello(u)
  do return end
  ::L_0185::
  m688.ingaggia(u, 20, "L_0185", "L_0172")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 0, 0) then goto L_019b end
  m688.variabile(u, 0, 0)
  goto L_01a2
  ::L_019b::
  if m688.variabile_uguale(u, 1, 0) then goto L_01a7 end
  ::L_01a2::
  if m688.bersaglio_vicino(u, 9) then goto L_0284 end
  ::L_01a7::
  do return end
  ::L_0284::
  if m688.variabile_uguale(u, 2, 4) then goto L_0172 end
  m688.comportamento(u, "L_04a2")
  m688.marca_contatti(u)
  m688.salva_modello(u)
  m688.variabile_piu(u, 2)
  script.L_04a2(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_029f")
  do return end
end

script.L_0172 = function(u)
  ::L_0172::
  m688.comportamento(u, "L_016d")
  m688.salva_modello(u)
  do return end
end

script.L_0177 = function(u)
  ::L_0177::
  if m688.manomesso(u) then goto L_017b end
  do return end
  ::L_017b::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0185 = function(u)
  ::L_0172::
  m688.comportamento(u, "L_016d")
  m688.salva_modello(u)
  do return end
  ::L_0185::
  m688.ingaggia(u, 20, "L_0185", "L_0172")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 0, 0) then goto L_019b end
  m688.variabile(u, 0, 0)
  goto L_01a2
  ::L_019b::
  if m688.variabile_uguale(u, 1, 0) then goto L_01a7 end
  ::L_01a2::
  if m688.bersaglio_vicino(u, 9) then goto L_0284 end
  ::L_01a7::
  do return end
  ::L_0284::
  if m688.variabile_uguale(u, 2, 4) then goto L_0172 end
  m688.comportamento(u, "L_04a2")
  m688.marca_contatti(u)
  m688.salva_modello(u)
  m688.variabile_piu(u, 2)
  script.L_04a2(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_029f")
  do return end
end

script.L_01a8 = function(u)
  ::L_01a8::
  m688.vai_a(u, 2013.12, -20947.52, 30, "L_01b3")
  m688.salva_modello(u)
  do return end
end

script.L_01b3 = function(u)
  ::L_01b3::
  m688.vai_a(u, 3506.56, -24681.12, 30, "L_01be")
  m688.salva_modello(u)
  do return end
end

script.L_01be = function(u)
  ::L_01be::
  m688.vai_a(u, -973.76, -24681.12, 30, "L_01c9")
  m688.salva_modello(u)
  do return end
end

script.L_01c9 = function(u)
  ::L_01c9::
  m688.vai_a(u, 2759.84, -29908.16, 30, "L_01e2")
  m688.nuovo_caso(u)
  if m688.caso(u, 0.5039) then goto L_01e0 end
  m688.vai_a(u, 3506.56, -24681.12, 30, "L_0203")
  ::L_01e0::
  m688.salva_modello(u)
  do return end
end

script.L_01e2 = function(u)
  ::L_01e2::
  m688.vai_a(u, 7986.88, -38122.08, 30, "L_01ed")
  m688.salva_modello(u)
  do return end
end

script.L_01ed = function(u)
  ::L_01ed::
  m688.vai_a(u, 10973.76, -32148.32, 30, "L_01f8")
  m688.salva_modello(u)
  do return end
end

script.L_01f8 = function(u)
  ::L_01f8::
  m688.vai_a(u, 22174.56, -36628.64, 30, "L_03d4")
  m688.salva_modello(u)
  do return end
end

script.L_0203 = function(u)
  ::L_0203::
  m688.vai_a(u, 7986.88, -29908.16, 30, "L_020e")
  m688.salva_modello(u)
  do return end
end

script.L_020e = function(u)
  ::L_020e::
  m688.vai_a(u, 16200.8, -34388.48, 30, "L_03d4")
  m688.salva_modello(u)
  do return end
end

script.L_0219 = function(u)
  ::L_0219::
  m688.vai_a(u, 9480.32, -30654.88, 30, "L_0224")
  m688.salva_modello(u)
  do return end
end

script.L_0224 = function(u)
  ::L_0224::
  m688.vai_a(u, 7986.88, -38122.08, 30, "L_022f")
  m688.salva_modello(u)
  do return end
end

script.L_022f = function(u)
  ::L_022f::
  m688.vai_a(u, 2759.84, -29908.16, 30, "L_023a")
  m688.salva_modello(u)
  do return end
end

script.L_023a = function(u)
  ::L_023a::
  m688.vai_a(u, 4253.28, -25427.84, 30, "L_0245")
  m688.salva_modello(u)
  do return end
end

script.L_0245 = function(u)
  ::L_0245::
  m688.vai_a(u, 519.68, -24681.12, 30, "L_0250")
  m688.salva_modello(u)
  do return end
end

script.L_0250 = function(u)
  ::L_0250::
  m688.vai_a(u, -973.76, -17960.64, 30, "L_03d4")
  m688.salva_modello(u)
  do return end
end

script.L_025b = function(u)
  ::L_025b::
  if m688.bersaglio_id(u, 2) then goto L_0265 end
  m688.comportamento(u, "L_025b")
  m688.salva_modello(u)
  do return end
  ::L_0265::
  m688.variabile(u, 5, 1)
  m688.ingaggia(u, 30, "L_0265", "L_02a3")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_02b2")
  if m688.traccia_solida(u) then goto L_0278 end
  do return end
  ::L_0278::
  if m688.bersaglio_vicino(u, 5) then goto L_027e end
  do return end
  ::L_027e::
  m688.variabile(u, 5, 2)
  do return end
end

script.L_0265 = function(u)
  ::L_0265::
  m688.variabile(u, 5, 1)
  m688.ingaggia(u, 30, "L_0265", "L_02a3")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_02b2")
  if m688.traccia_solida(u) then goto L_0278 end
  do return end
  ::L_0278::
  if m688.bersaglio_vicino(u, 5) then goto L_027e end
  do return end
  ::L_027e::
  m688.variabile(u, 5, 2)
  do return end
end

script.L_029f = function(u)
  ::L_029f::
  m688.variabile_meno(u, 2)
  do return end
end

script.L_02a3 = function(u)
  ::L_02a3::
  m688.variabile(u, 5, 0)
  m688.comportamento(u, "L_025b")
  m688.salva_modello(u)
  m688.dopo(u, 200, "L_02b2")
  do return end
end

script.L_02b2 = function(u)
  ::L_02b2::
  m688.messaggio(u, 105)
  do return end
end

script.L_02f0 = function(u)
  ::L_02f0::
  if m688.unita_esiste(u, 2) then goto L_02f8 end
  m688.messaggio(u, 103)
  do return end
  ::L_02f8::
  m688.dopo(u, 20, "L_02f0")
  do return end
end

script.L_02fe = function(u)
  ::L_02fe::
  if m688.variabile_uguale(u, 5, 2) then goto L_0312 end
  if m688.variabile_uguale(u, 5, 1) then goto L_030f end
  m688.messaggio(u, 104)
  do return end
  ::L_030f::
  m688.messaggio(u, 107)
  do return end
  ::L_0312::
  m688.messaggio(u, 106)
  do return end
end

script.L_0326 = function(u)
  ::L_0326::
  m688.nuovo_caso(u)
  m688.posizione(u, 5000, -22440.96)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.1602) then goto L_036b end
  m688.posizione(u, -973.76, -25427.84)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.3164) then goto L_036b end
  m688.posizione(u, -227.04, -29908.16)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.4727) then goto L_036b end
  m688.posizione(u, 519.68, -36628.64)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.6289) then goto L_036b end
  m688.posizione(u, 5746.72, -35135.2)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.7852) then goto L_036b end
  m688.posizione(u, 22174.56, -37375.36)
  m688.rotta_ora(u, 245)
  ::L_036b::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0326")
  do return end
end

script.L_039d = function(u)
  ::L_039d::
  m688.vai_a(u, 12467.2, -23934.4, 30, "L_03a8")
  m688.salva_modello(u)
  do return end
end

script.L_03a8 = function(u)
  ::L_03a8::
  m688.vai_a(u, 10227.04, -23187.68, 30, "L_03b3")
  m688.salva_modello(u)
  do return end
end

script.L_03b3 = function(u)
  ::L_03b3::
  m688.vai_a(u, 7240.16, -23187.68, 30, "L_03be")
  m688.salva_modello(u)
  do return end
end

script.L_03be = function(u)
  ::L_03be::
  m688.vai_a(u, 4253.28, -22440.96, 30, "L_03c9")
  m688.salva_modello(u)
  do return end
end

script.L_03c9 = function(u)
  ::L_03c9::
  m688.vai_a(u, -6947.52, -10493.44, 30, "L_03d4")
  m688.salva_modello(u)
  do return end
end

script.L_03d4 = function(u)
  ::L_03d4::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_045c = function(u)
  ::L_045c::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0522", "L_04ff")
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
  m688.dopo(u, 150, "L_04b9")
  m688.dopo(u, 170, "L_04e2")
  m688.dopo(u, 240, "L_0522")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_04a2 = function(u)
  ::L_04a2::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0522", "L_04ff")
  script.L_04bc(u)
  m688.dopo(u, 240, "L_0522")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_04b9 = function(u)
  ::L_04b9::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_04bc = function(u)
  ::L_04bc::
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

script.L_04e2 = function(u)
  ::L_04e2::
  m688.annulla_timer(u, "L_04e2")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_04ff = function(u)
  ::L_04ff::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_04ed")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_051b")
  m688.dopo(u, 10, "L_0513")
  do return end
end

script.L_04ed = function(u)
  ::L_04ed::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0513")
  m688.annulla_timer(u, "L_04ff")
  m688.insegui(u, 0, "L_0522", "L_04ff")
  m688.salva_modello(u)
  do return end
end

script.L_0513 = function(u)
  ::L_0513::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_051b = function(u)
  ::L_051b::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_051b")
  do return end
end

script.L_0522 = function(u)
  ::L_0522::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0526 = function(u)
  ::L_0526::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0522", "L_04ff")
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
  m688.dopo(u, 2, "L_0553")
  m688.dopo(u, 160, "L_0558")
  do return end
end

script.L_0553 = function(u)
  ::L_0553::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0558 = function(u)
  ::L_0558::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_055c = function(u)
  ::L_055c::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0561 = function(u)
  ::L_0561::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_04ff")
  do return end
end

script.L_056b = function(u, p)
  ::L_056b::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_057a")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_057a = function(u)
  ::L_057a::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_04ed")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0588 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0588::
  do return end
end

script.L_0589 = function(u, p)
  ::L_0589::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_0594")
  m688.salva_modello(u)
  do return end
end

script.L_0594 = function(u)
  ::L_0594::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_059b = function(u, p)
  ::L_059b::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_05a0 = function(u, p)
  ::L_05a0::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_05a7 = function(u, p)
  ::L_05a7::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_05af = function(u)
  ::L_05af::
  m688.annulla_timer(u, "L_05ee")
  m688.dopo(u, 20, "L_05ee")
  do return end
end

script.L_05b8 = function(u)
  ::L_05b8::
  m688.annulla_timer(u, "L_05f1")
  m688.dopo(u, 20, "L_05f1")
  do return end
end

script.L_05c1 = function(u)
  ::L_05c1::
  m688.annulla_timer(u, "L_05f4")
  m688.dopo(u, 20, "L_05f4")
  do return end
end

script.L_05ca = function(u)
  ::L_05ca::
  m688.annulla_timer(u, "L_05f7")
  m688.dopo(u, 20, "L_05f7")
  do return end
end

script.L_05d3 = function(u)
  ::L_05d3::
  m688.annulla_timer(u, "L_05fa")
  m688.dopo(u, 20, "L_05fa")
  do return end
end

script.L_05dc = function(u)
  ::L_05dc::
  m688.annulla_timer(u, "L_05fd")
  m688.dopo(u, 20, "L_05fd")
  do return end
end

script.L_05e5 = function(u)
  ::L_05e5::
  m688.annulla_timer(u, "L_0600")
  m688.dopo(u, 20, "L_0600")
  do return end
end

script.L_05ee = function(u)
  ::L_05ee::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_05f1 = function(u)
  ::L_05f1::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_05f4 = function(u)
  ::L_05f4::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_05f7 = function(u)
  ::L_05f7::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_05fa = function(u)
  ::L_05fa::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_05fd = function(u)
  ::L_05fd::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0600 = function(u)
  ::L_0600::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0603 = function(u)
  ::L_0603::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_060b = function(u, p)
  ::L_060b::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_060f = function(u)
  ::L_060f::
  m688.dopo(u, 60, "L_0619")
  do return end
end

script.L_0615 = function(u, p)
  ::L_0615::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0619 = function(u)
  ::L_0619::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_061f = function(u)
  ::L_061f::
  m688.dopo(u, 40, "L_0625")
  do return end
end

script.L_0625 = function(u)
  ::L_0625::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_062b = function(u)
  ::L_062b::
  m688.annulla_timer(u, "L_0619")
  m688.annulla_timer(u, "L_0625")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0635 = function(u, p)
  ::L_0635::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_063a = function(u, p)
  ::L_063a::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

return missione
