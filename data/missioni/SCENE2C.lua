-- SCENE2C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE2C",
  missione = 2,
  titolo = { it = "Scrollatevelo di dosso", en = "SHAKE 'EM" },
  lato = "due_giocatori",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 274.32, 457.2 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_02d9" },
    harpoon = { gestore = "L_0286" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_036a" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_03cd",
    telegrafo = "L_03df",
    quota = "L_03e4",
    timone = "L_03eb",
    pilota_automatico_off = "L_0447",
    periscopio = "L_044f",
    carica_tubo_1 = "L_03f3",
    carica_tubo_2 = "L_03fc",
    carica_tubo_3 = "L_0405",
    carica_tubo_4 = "L_040e",
    carica_tubo_5 = "L_0417",
    carica_tubo_6 = "L_0420",
    carica_tubo_7 = "L_0429",
    schiera_cala = "L_0453",
    schiera_recupera = "L_0463",
    schiera_persa = "L_046f",
    designa_bersaglio = "L_0479",
    posizione = "L_047e",
    antenna = "L_0459",
    guida_e0 = "L_03a0",
    guida_e1 = "L_03a5",
    guida_e2 = "L_03af",
    f0 = "L_0485",
    f1 = "L_048e",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.50390625, probabilita_originale = "129/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -973.76, y = -17960.64 }, originale = { x = 4864, y = 2816 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -1720.48, y = -15720.48 }, originale = { x = 4832, y = 2912 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 250, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_00df", reazione = nil, arrivo = nil } },
      { id = 2, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 1200, unita = 0, gestore = "L_0120" }, { dopo_s = 30, unita = 5, gestore = "L_015a" } },
      variabili = { [1] = 0, [2] = 0 },
    },
    { probabilita = 0.49609375, probabilita_originale = "127/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -33641.76 }, originale = { x = 5568, y = 2144 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 17694.24, y = -34388.48 }, originale = { x = 5664, y = 2112 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 250, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_00df", reazione = nil, arrivo = nil } },
      { id = 2, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d1" } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 1200, unita = 0, gestore = "L_0120" }, { dopo_s = 30, unita = 5, gestore = "L_015a" } },
      variabili = { [1] = 0, [2] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, o si scrolla di dosso l'Alfa sovietico che ha in coda, o la richiamo da questa missione. Non serve che le dica quanto sia in gioco con il suo successo, compresa la sua carriera. Faccia tutto il necessario per far perdere le tracce a Ivan. Purtroppo non posso autorizzarla a lanciare armi, a meno che i russi non decidano di fare sul serio.", "VICEAMMIRAGLIO J.W. RATCLIFF III", "La Marina sovietica si congratula con il Leninskii Komsomol per i successi finora ottenuti nel controllo del sottomarino americano. Stiamo cercando di inviarvi rinforzi. Il Leninskii Komsomol continui a seguire gli americani fino a nuovo ordine, restando entro 5 miglia per poter aprire il fuoco al comando. Kapitan, non disonori il suo Paese e la sua Marina perdendo la preda negli stretti canali delle Fær Øer. Non è autorizzato a lanciare siluri se non viene attaccato." },
    en = { "CAPTAIN, EITHER YOU SHAKE THE SOVIET ALFA ON YOUR TAIL OR I'M CALLING YOU OFF THIS MISSION. I DON'T HAVE TO TELL YOU THERE'S A LOT RIDING ON YOUR SUCCESS - INCLUDING YOUR CAREER. NOW DO WHATEVER IT TAKES TO THROW IVAN FOR A LOOP. UNFORTUNATELY, I CAN'T ALLOW YOU TO LAUNCH WEAPONS UNLESS THE RUSSIANS DECIDE THEY WANT TO PLAY HARDBALL.", "VADM J.W. RATCLIFF III", "THE SOVIET NAVY CONGRATULATES THE LENINSKII KOMSOMOL ON HER SUCCESS SO FAR IN MONITORING THE AMERICAN SUBMARINE. WE ARE CURRENTLY TRYING TO SEND YOU SUPPORT. THE LENINSKII KOMSOMOL IS DIRECTED TO CONTINUE TRACKING THE AMERICANS UNTIL FURTHER NOTICE. STAY WITHIN 5 MILES SO THAT YOU MAY FIRE ON COMMAND. KAPITAN, DO NOT DISGRACE YOUR COUNTRY AND NAVY BY LOSING YOUR QUARRY IN THE NARROW CHANNELS OF THE FAEROE ISLANDS. YOU ARE NOT AUTHORIZED TO LAUNCH TORPEDOES UNLESS FIRED UPON." },
  },
  esiti = {
    [1] = { it = "CODICE: 2-1-CIN\nL'USS Los Angeles si è incagliato ed è andato perduto in mare durante un contatto con un sottomarino sovietico. I dettagli esatti dell'incidente sono attualmente oggetto di indagine, ma tutte le unità sono state poste in allerta gialla. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 2-1-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS LOST AT SEA DURING AN ENCOUNTER WITH A SOVIET SUBMARINE. THE EXACT DETAILS PERTAINING TO THE ACCIDENT ARE CURRENTLY UNDER INVESTIGATION, BUT ALL SHIPS ARE BEING PUT ON YELLOW ALERT. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 2-2-CIN\nL'USS Los Angeles è andato perduto in mare durante un contatto con un sottomarino sovietico. La causa esatta dell'incidente è attualmente oggetto di indagine, ma tutte le unità sono state poste in allerta gialla. Se verrà accertato che la responsabilità è della minaccia rossa, farò in modo che paghino per le loro azioni. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 2-2-CIN\nTHE USS LOS ANGELES WAS LOST AT SEA DURING AN ENCOUNTER WITH A SOVIET SUBMARINE. THE EXACT CAUSE OF THE ACCIDENT IS CURRENTLY UNDER INVESTIGATION, BUT ALL SHIPS ARE BEING PUT ON YELLOW ALERT. IF THE RED MENACE IS FOUND TO BE RESPONSIBLE FOR THIS, I WILL SEE TO IT THAT THEY PAY FOR THEIR ACTIONS. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 2-3-CIN\nOttimo lavoro, Comandante: sono lieto di vedere che ne è uscito tutto intero. Le mosse di Ivan ci hanno tenuti col fiato sospeso per un po', ma sembra che tecnologia e addestramento abbiano ancora una volta avuto la meglio su dogmatismo e forza bruta. Abbiamo già delle unità dirette sul luogo dell'incidente per salvare l'equipaggio di quella balena spiaggiata ed eventualmente recuperare il battello sovietico.", en = "CODE: 2-3-CIN\nGOOD WORK CAPTAIN, I'M GLAD TO SEE YOU CAME OUT OF THIS ONE IN ONE PIECE. IVAN'S ACTIONS HAD US GOING FOR A WHILE, BUT IT LOOKS LIKE TECHNOLOGY AND TRAINING HAVE ONCE AGAIN PREVAILED OVER DOGMATISM AND BRUTE FORCE. WE CURRENTLY HAVE SHIPS ON THEIR WAY TO THE SIGHT OF THE INCIDENT TO SAVE THE CREW OF THAT BEACHED WHALE, AND POSSIBLY SALVAGE THE SOVIET VESSEL." },
    [4] = { it = "CODICE: 2-4-CIN\nCongratulazioni, Comandante: da solo è riuscito a minacciare la pace mondiale e a mettere in imbarazzo la Marina. Farà bene ad avere una buona giustificazione, perché se scopro che ha aperto il fuoco senza provocazione le farò la pelle. Potrà riflettere sulle sue azioni e mettere in ordine la sua versione dei fatti durante il rientro in porto.", en = "CODE: 2-4-CIN\nCONGRATULATIONS, CAPTAIN, YOU'VE SINGLE-HANDEDLY THREATENED WORLD PEACE AND EMBARRASSED THE NAVY. YOU'D BETTER HAVE A GOOD EXCUSE BECAUSE IF I FIND OUT THAT YOU FIRED UNPROVOKED I'M GOING TO HAVE YOUR BUTT FOR BREAKFAST. YOU MAY REFLECT ON YOUR ACTIONS AND GET YOUR STORY STRAIGHT ON YOUR RETURN TO PORT." },
    [5] = { it = "CODICE: 2-5-CIN\nOttimo lavoro, Comandante. La sua eccellente prestazione sarà annotata nel suo stato di servizio. È riuscito a eludere la sorveglianza sovietica. Al suo rientro dovrà illustrare le sue tecniche di evasione ai nostri comandanti di squadriglia. Prosegua la missione come previsto: sembra proprio che sia partito alla grande.", en = "CODE: 2-5-CIN\nGOOD WORK, CAPTAIN. YOUR EXCELLENT PERFORMANCE WILL GO DOWN ON YOUR PERMANENT RECORD. YOU SUCCESSFULLY EVADED SOVIET SURVEILLANCE. I MUST HAVE YOU BRIEF OUR SQUADRON COMMANDERS ON YOUR EVASION TECHNIQUES WHEN YOU RETURN. PROCEED WITH YOUR MISSION AS PLANNED, IT LOOKS LIKE YOUR OFF TO A GREAT START." },
    [6] = { it = "CODICE: 2-6-CIN\nKapitan, aspettava forse che gli americani le dessero indicazioni e una cartina stradale? Lei ha ostacolato la lotta per il socialismo privando il suo Paese di informazioni vitali! Mi aspetto un rapporto completo che descriva i suoi errori, insieme al nome del peschereccio che preferirebbe comandare!", en = "CODE: 2-6-CIN\nKAPITAN, WERE YOU WAITING FOR THE AMERICANS TO GIVE YOU DIRECTIONS AND A ROADMAP? YOU HAVE HINDERED THE FIGHT FOR SOCIALISM BY DENYING YOUR COUNTRY VITAL INTELLIGENCE INFORMATION! I EXPECT A FULL REPORT DETAILING YOUR BLUNDERS ALONG WITH THE NAME OF THE TROLLER YOU WOULD PREFER TO COMMAND!" },
    [7] = { it = "CODICE: 2-7-CIN\nComandante, quello è stato il più misero tentativo di evasione che abbia visto dai tempi delle esercitazioni da allievo nella sala giochi dell'Accademia! Non ho tempo per lasciarla esercitare le procedure di evasione di base in uno scenario reale. Per colpa della sua incompetenza devo prolungare il pattugliamento di un altro sottomarino. Ha compromesso completamente la sua missione, Comandante. Rientri subito in porto, dove seguirò personalmente il suo corso di recupero di tattica subacquea.", en = "CODE: 2-7-CIN\nCAPTAIN, THAT WAS THE WEAKEST EXCUSE FOR AN ATTEMPT AT EVASION THAT I'VE SEEN SINCE CADET TRAINING IN THE GAME ROOM OF THE ACADEMY! I DON'T HAVE TIME FOR YOU TO PRACTICE ROUTINE EVASION PROCEDURES IN A REAL WORLD SCENARIO. BECAUSE OF YOUR INCOMPETENCE, I MUST EXTEND ANOTHER SUBMARINE'S PATROL PERIOD. YOU HAVE COMPLETELY COMPROMISED YOUR MISSION CAPTAIN. RETURN TO PORT AT ONCE, WHERE I CAN PERSONALLY OVERSEE YOUR REMEDIAL EDUCATION IN SUBMARINE TACTICS." },
    [8] = { it = "CODICE: 2-8-CIN\nLa Marina sovietica si congratula con il Leninskii Komsomol per aver portato a termine con successo la sua missione. Il Kapitan fa onore al socialismo e alla nostra Marina! Il cambio è in posizione e pronto a proseguire la sorveglianza. Salutiamo il Leninskii Komsomol e chiediamo che il sottomarino rientri immediatamente in porto!", en = "CODE: 2-8-CIN\nTHE SOVIET NAVY CONGRATULATES THE LENINSKII KOMSOMOL ON THE SUCCESSFUL COMPLETION OF HER MISSION. THE KAPITAN IS A CREDIT TO SOCIALISM AND OUR NAVY! RELIEF IS IN PLACE AND READY TO CONTINUE SURVEILLANCE. WE SALUTE THE LENINSKII KOMSOMOL AND REQUEST THAT THE SUBMARINE RETURN TO PORT IMMEDIATELY!" },
    [9] = { it = "CODICE: 2-9-CIN\nComandante, quello è stato il più misero tentativo di evasione che abbia visto dai tempi delle esercitazioni da allievo nella sala giochi dell'Accademia! Non ho tempo per lasciarla esercitare le procedure di evasione di base in uno scenario reale. I sovietici le stavano addosso con un battello; immagino che l'unica buona notizia sia che era fuori portata di tiro. Ha compromesso completamente la sua missione, Comandante. Rientri subito in porto, dove seguirò personalmente il suo corso di recupero di tattica subacquea.", en = "CODE: 2-9-CIN\nCAPTAIN, THAT WAS THE WEAKEST EXCUSE FOR AN ATTEMPT AT EVASION THAT I'VE SEEN SINCE CADET TRAINING IN THE GAME ROOM OF THE ACADEMY! I DON'T HAVE TIME FOR YOU TO PRACTICE ROUTINE EVASION PROCEDURES IN A REAL WORLD SCENARIO. THE SOVIETS HAD A BOAT ON YOU, I GUESS THE ONLY GOOD NEWS WAS THAT HE WAS OUT OF FIRING RANGE. YOU HAVE COMPLETELY COMPROMISED YOUR MISSION CAPTAIN. RETURN TO PORT AT ONCE, WHERE I CAN PERSONALLY OVERSEE YOUR REMEDIAL EDUCATION IN SUBMARINE TACTICS." },
    [10] = { it = "CODICE: 2-10-CIN\nKapitan, lei non è riuscito a mantenere una posizione di tiro sul sottomarino americano. Aspettava forse che gli americani accostassero per invitarla a bordo a vodka e caviale? Lei ha ostacolato la lotta del socialismo e gonfiato l'ego imperialista lasciandosi sfuggire quei porci. Rientri in porto, e in fretta: mi divertirò a stabilire la sua punizione.", en = "CODE: 2-10-CIN\nKAPITAN, YOU FAILED TO MAINTAIN A FIRING POSITION ON THE AMERICAN SUB. WERE YOU WAITING FOR THE AMERICANS TO PULL OVER AND INVITE YOU IN FOR VODKA AND CAVIAR? YOU HAVE HINDERED THE FIGHT OF SOCIALISM AND INFLATED THE IMPERIALIST EGO BY ALLOWING THE SWINE TO SLIP FROM YOUR GRASP. RETURN TO PORT QUICKLY, I WILL ENJOY PRESCRIBING YOUR PUNISHMENT." },
    [15] = { it = "CODICE: 2-15-CIN\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento nei pressi dell'arcipelago delle Fær Øer. Non dimenticheremo mai il coraggio e il valore di questi giovani valorosi che hanno dato la vita per la Patria nel nome del Soviet Supremo!", en = "CODE: 2-15-CIN\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE PATROLLING NEAR THE FAEROE ISLAND CHAIN. WE SHALL NEVER FORGET THE COURAGE AND VALOR OF THESE BRAVE YOUNG MEN WHO GAVE UP THEIR LIVES FOR THE MOTHERLAND IN THE NAME OF THE SUPREME SOVIET!" },
    [16] = { it = "CODICE: 2-16-CIN\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento nei pressi dell'arcipelago delle Fær Øer. Non dimenticheremo mai il coraggio e il valore di questi giovani valorosi che hanno dato la vita per la Patria nel nome del Soviet Supremo! La Pravda ha dichiarato che l'attacco al Leninskii Komsomol è stato un'esecuzione premeditata da parte dell'Occidente imperialista e una dimostrazione della sua politica di aggressione. Queste azioni non resteranno impunite!", en = "CODE: 2-16-CIN\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE PATROLLING NEAR THE FAEROE ISLAND CHAIN. WE SHALL NEVER FORGET THE COURAGE AND VALOR OF THESE BRAVE YOUNG MEN WHO GAVE UP THEIR LIVES FOR THE MOTHERLAND IN THE NAME OF THE SUPREME SOVIET! PRAVDA HAS STATED THAT THE ATTACK OF THE LENINSKII KOMSOMOL WAS A PREMEDITATED EXECUTION BY THE IMPERIALIST WEST AND A DEMONSTRATION OF THEIR POLICIES OF AGGRESSION. THESE ACTIONS SHALL NOT BE OVERLOOKED!" },
    [17] = { it = "CODICE: 2-17-CIN\nQuesto comando è lieto di constatare che il Leninskii Komsomol è sopravvissuto allo scontro con gli strumenti dell'impero imperialista. È un peccato che il sottomarino occidentale abbia provocato la propria distruzione prima che si potessero raccogliere altre informazioni. La missione del Leninskii Komsomol è ora compiuta, e l'equipaggio può rientrare in porto fiero della Marina e della causa rivoluzionaria che difende con tanta virtù.", en = "CODE: 2-17-CIN\nTHIS COMMAND IS PLEASED TO SEE THAT THE LENINSKII KOMSOMOL HAS SURVIVED THE ENCOUNTER WITH THE TOOLS OF THE IMPERIALIST EMPIRE. IT IS UNFORTUNATE THAT THE WESTERN SUBMARINE FORCED ITS OWN DESTRUCTION BEFORE MORE INFORMATION COULD BE GAINED. THE MISSION OF THE LENINSKII KOMSOMOL IS NOW COMPLETE AND THE CREW OF THIS SUBMARINE MAY RETURN TO PORT PROUD OF THE NAVY AND THE REVOLUTIONARY CAUSE THEY VIRTUOUSLY PROTECT." },
    [18] = { it = "CODICE: 2-18-CIN\nIl Leninskii Komsomol ha gravemente oltrepassato i propri poteri compiendo un attacco non provocato in acque internazionali! Se non emergeranno prove di un'aggressione americana, l'ultima nave che questo Kapitan vedrà sarà un traghetto nell'Arcipelago Gulag! Lo zampolit è incaricato di assumere il comando e di far emergere immediatamente il sottomarino per il ricongiungimento con il Cheliabinskaia Komsomolka!", en = "CODE: 2-18-CIN\nTHE LENINSKII KOMSOMOL HAS SEVERELY OVERSTEPPED AUTHORITY BY MAKING AN UNPROVOKED ATTACK IN INTERNATIONAL WATERS! UNLESS THERE IS EVIDENCE OF AGGRESSION ON THE PART OF THE AMERICANS, THE LAST SHIP THIS KAPITAN WILL EVER SEE WILL BE A FERRY BOAT IN THE GULAG ARCHIPELAGO! THE ZAMPOLIT IS INSTRUCTED TO TAKE COMMAND AND SURFACE THE SUBMARINE IMMEDIATELY FOR RENDEZVOUS WITH THE CHELIABINSKAIA KOMSOMOLKA!" },
    [19] = { it = "CODICE: 2-19-CIN\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!\nCODICE: 2-20-CIN\nIl Kapitan del Leninskii Komsomol è responsabile della morte di uomini innocenti. Il Kapitan è dichiarato colpevole di sovversione contro lo Stato, e lo zampolit assumerà il controllo del sottomarino. Non mi dispiacerebbe scoprire che questo Kapitan ha avuto un incidente lento e doloroso, e che non è sopravvissuto al rientro in porto.", en = "CODE: 2-19-CIN\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!\nCODE: 2-20-CIN\nTHE KAPITAN OF THE LENINSKII KOMSOMOL IS RESPONSIBLE FOR THE DEATH OF INNOCENT MEN. THE KAPITAN IS HEREBY FOUND GUILTY OF SUBVERSION AGAINST THE STATE AND THE ZAMPOLIT WILL ASSUME CONTROL OF THE SUBMARINE. I WOULD NOT BE UNHAPPY TO FIND THAT THIS KAPITAN HAD A SLOW, PAINFUL ACCIDENT AND DID NOT SURVIVE THE RETURN TO PORT." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 6, spostamento_max_m = 1984.5947, non_risolte = 0 },
  script = script,
}

script.L_00df = function(u)
  ::L_00df::
  if m688.bersaglio_id(u, 0) then goto L_00e9 end
  m688.comportamento(u, "L_00df")
  m688.salva_modello(u)
  do return end
  ::L_00e9::
  m688.variabile(u, 5, 1)
  m688.ingaggia(u, 30, "L_00e9", "L_0108")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0117")
  if m688.traccia_solida(u) then goto L_00fc end
  do return end
  ::L_00fc::
  if m688.bersaglio_vicino(u, 5) then goto L_0102 end
  do return end
  ::L_0102::
  m688.variabile(u, 5, 2)
  do return end
end

script.L_00e9 = function(u)
  ::L_00e9::
  m688.variabile(u, 5, 1)
  m688.ingaggia(u, 30, "L_00e9", "L_0108")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0117")
  if m688.traccia_solida(u) then goto L_00fc end
  do return end
  ::L_00fc::
  if m688.bersaglio_vicino(u, 5) then goto L_0102 end
  do return end
  ::L_0102::
  m688.variabile(u, 5, 2)
  do return end
end

script.L_0108 = function(u)
  ::L_0108::
  m688.variabile(u, 5, 0)
  m688.comportamento(u, "L_00df")
  m688.salva_modello(u)
  m688.dopo(u, 200, "L_0117")
  do return end
end

script.L_0117 = function(u)
  ::L_0117::
  if m688.giocatore_secondo(u) then goto L_011d end
  m688.messaggio(u, 105)
  do return end
  ::L_011d::
  m688.messaggio(u, 106)
  do return end
end

script.L_0120 = function(u)
  ::L_0120::
  if m688.variabile_uguale(u, 5, 2) then goto L_0140 end
  if m688.variabile_uguale(u, 5, 1) then goto L_0137 end
  if m688.giocatore_secondo(u) then goto L_0134 end
  m688.messaggio(u, 105)
  do return end
  ::L_0134::
  m688.messaggio(u, 106)
  do return end
  ::L_0137::
  if m688.giocatore_secondo(u) then goto L_013d end
  m688.messaggio(u, 109)
  do return end
  ::L_013d::
  m688.messaggio(u, 110)
  do return end
  ::L_0140::
  if m688.giocatore_secondo(u) then goto L_0146 end
  m688.messaggio(u, 107)
  do return end
  ::L_0146::
  m688.messaggio(u, 108)
  do return end
end

script.L_015a = function(u)
  ::L_015a::
  m688.nuovo_caso(u)
  m688.posizione(u, 5000, -22440.96)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.1602) then goto L_019f end
  m688.posizione(u, -973.76, -25427.84)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.3164) then goto L_019f end
  m688.posizione(u, -227.04, -29908.16)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.4727) then goto L_019f end
  m688.posizione(u, 519.68, -36628.64)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.6289) then goto L_019f end
  m688.posizione(u, 5746.72, -35135.2)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.7852) then goto L_019f end
  m688.posizione(u, 22174.56, -37375.36)
  m688.rotta_ora(u, 245)
  ::L_019f::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_015a")
  do return end
end

script.L_01d1 = function(u)
  ::L_01d1::
  m688.vai_a(u, 12467.2, -23934.4, 30, "L_01dc")
  m688.salva_modello(u)
  do return end
end

script.L_01dc = function(u)
  ::L_01dc::
  m688.vai_a(u, 10227.04, -23187.68, 30, "L_01e7")
  m688.salva_modello(u)
  do return end
end

script.L_01e7 = function(u)
  ::L_01e7::
  m688.vai_a(u, 7240.16, -23187.68, 30, "L_01f2")
  m688.salva_modello(u)
  do return end
end

script.L_01f2 = function(u)
  ::L_01f2::
  m688.vai_a(u, 4253.28, -22440.96, 30, "L_01fd")
  m688.salva_modello(u)
  do return end
end

script.L_01fd = function(u)
  ::L_01fd::
  m688.vai_a(u, -6947.52, -10493.44, 30, "L_0208")
  m688.salva_modello(u)
  do return end
end

script.L_0208 = function(u)
  ::L_0208::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0286 = function(u)
  ::L_0286::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_0290")
  do return end
end

script.L_0290 = function(u)
  ::L_0290::
  m688.attacco_casuale(u, 100)
  do return end
end

script.L_02d9 = function(u)
  ::L_02d9::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0366", "L_0343")
  script.L_0300(u)
  m688.dopo(u, 170, "L_02fd")
  m688.dopo(u, 190, "L_0326")
  m688.dopo(u, 280, "L_0366")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_02fd = function(u)
  ::L_02fd::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0300 = function(u)
  ::L_0300::
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

script.L_0326 = function(u)
  ::L_0326::
  m688.annulla_timer(u, "L_0326")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0343 = function(u)
  ::L_0343::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0331")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_035f")
  m688.dopo(u, 10, "L_0357")
  do return end
end

script.L_0331 = function(u)
  ::L_0331::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0357")
  m688.annulla_timer(u, "L_0343")
  m688.insegui(u, 0, "L_0366", "L_0343")
  m688.salva_modello(u)
  do return end
end

script.L_0357 = function(u)
  ::L_0357::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_035f = function(u)
  ::L_035f::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_035f")
  do return end
end

script.L_0366 = function(u)
  ::L_0366::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_036a = function(u)
  ::L_036a::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0366", "L_0343")
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
  m688.dopo(u, 2, "L_0397")
  m688.dopo(u, 160, "L_039c")
  do return end
end

script.L_0397 = function(u)
  ::L_0397::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_039c = function(u)
  ::L_039c::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_03a0 = function(u)
  ::L_03a0::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_03a5 = function(u)
  ::L_03a5::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0343")
  do return end
end

script.L_03af = function(u, p)
  ::L_03af::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_03be")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_03be = function(u)
  ::L_03be::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0331")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_03cc end
  m688.messaggio_al_giocatore(u, 42)
  ::L_03cc::
  do return end
end

script.L_03cd = function(u, p)
  ::L_03cd::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_03d8")
  m688.salva_modello(u)
  do return end
end

script.L_03d8 = function(u)
  ::L_03d8::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_03df = function(u, p)
  ::L_03df::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_03e4 = function(u, p)
  ::L_03e4::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_03eb = function(u, p)
  ::L_03eb::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_03f3 = function(u)
  ::L_03f3::
  m688.annulla_timer(u, "L_0432")
  m688.dopo(u, 20, "L_0432")
  do return end
end

script.L_03fc = function(u)
  ::L_03fc::
  m688.annulla_timer(u, "L_0435")
  m688.dopo(u, 20, "L_0435")
  do return end
end

script.L_0405 = function(u)
  ::L_0405::
  m688.annulla_timer(u, "L_0438")
  m688.dopo(u, 20, "L_0438")
  do return end
end

script.L_040e = function(u)
  ::L_040e::
  m688.annulla_timer(u, "L_043b")
  m688.dopo(u, 20, "L_043b")
  do return end
end

script.L_0417 = function(u)
  ::L_0417::
  m688.annulla_timer(u, "L_043e")
  m688.dopo(u, 20, "L_043e")
  do return end
end

script.L_0420 = function(u)
  ::L_0420::
  m688.annulla_timer(u, "L_0441")
  m688.dopo(u, 20, "L_0441")
  do return end
end

script.L_0429 = function(u)
  ::L_0429::
  m688.annulla_timer(u, "L_0444")
  m688.dopo(u, 20, "L_0444")
  do return end
end

script.L_0432 = function(u)
  ::L_0432::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0435 = function(u)
  ::L_0435::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0438 = function(u)
  ::L_0438::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_043b = function(u)
  ::L_043b::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_043e = function(u)
  ::L_043e::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0441 = function(u)
  ::L_0441::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0444 = function(u)
  ::L_0444::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0447 = function(u)
  ::L_0447::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_044f = function(u, p)
  ::L_044f::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0453 = function(u)
  ::L_0453::
  m688.dopo(u, 60, "L_045d")
  do return end
end

script.L_0459 = function(u, p)
  ::L_0459::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_045d = function(u)
  ::L_045d::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0463 = function(u)
  ::L_0463::
  m688.dopo(u, 40, "L_0469")
  do return end
end

script.L_0469 = function(u)
  ::L_0469::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_046f = function(u)
  ::L_046f::
  m688.annulla_timer(u, "L_045d")
  m688.annulla_timer(u, "L_0469")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0479 = function(u, p)
  ::L_0479::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_047e = function(u, p)
  ::L_047e::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

script.L_0485 = function(u)
  ::L_0485::
  if m688.giocatore_secondo(u) then goto L_048b end
  m688.messaggio(u, 25)
  do return end
  ::L_048b::
  m688.messaggio(u, 29)
  do return end
end

script.L_048e = function(u)
  ::L_048e::
  if m688.giocatore_secondo(u) then goto L_0494 end
  m688.messaggio(u, 29)
  do return end
  ::L_0494::
  m688.messaggio(u, 25)
  do return end
end

return missione
