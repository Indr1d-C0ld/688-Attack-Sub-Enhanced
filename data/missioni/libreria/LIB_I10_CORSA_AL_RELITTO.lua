-- LIB_I10_CORSA_AL_RELITTO: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_I10_CORSA_AL_RELITTO",
  titolo = { it = "Corsa al relitto", en = "Race to the Wreck" },
  lato = "urss",
  teatro = "celtica",
  posa = { metodo = "rapida", centro = { lat = 48.25, lon = -9.75 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-08-02", ora_utc = 16.667,
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
    { unita = 5, missione = "caccia", situazione = "pace", bersagli = "giocatore", equipaggio = "scarso",
      base = { x = 337295.9, y = 254119.6 }, zona = { x = -131172.1, y = 81646.7, raggio_m = 30000 } },
  },
  categorie_esiti = { [41] = "fallimento" },
  obiettivi = {
    { id = "relitto", tipo = "raggiungere", cerchio = { x = -131172.1, y = 81646.7, r = 3704 },
      testo = { it = "Raggiungere il relitto", en = "Reach the wreck" } },
    { id = "non_sparare", tipo = "proteggere", unita = { 3, 4, 5 }, colpire = false,
      testo = { it = "Non usare le armi", en = "Do not use weapons" } },
    { id = "sturgeon", tipo = "scoprire", unita = { 3 }, primario = false,
      testo = { it = "Classificare il sottomarino americano", en = "Classify the American submarine" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: CORSA AL RELITTO", "2 agosto 1989, ore 1640Z. Teatro: Margine della Piattaforma Celtica.", "Un nostro sottomarino e' affondato tre giorni fa sulla scarpata di Goban Spur, con i suoi codici e un reattore spento.", "Sorvegliate il relitto finche' non arriva la nave di soccorso, fra due ore e mezza. La posizione che vi diamo e' incerta di sei miglia: cercatelo col sonar.", "Gli americani hanno sentito l'esplosione col SOSUS. Un loro sottomarino potrebbe essere gia' sulla scarpata, e una nave «oceanografica» arriva da ovest.", "Non deve avvicinarsi al relitto. Fatevi sentire, mostratevi: il sonar attivo e' permesso, le armi no.", "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore e 40 minuti." },
    en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RACE TO THE WRECK", "2 August 1989, 1640Z. Theatre: Celtic Shelf margin.", "One of our submarines sank three days ago on the Goban Spur slope, with her codes and a shut-down reactor.", "Guard the wreck until the rescue ship arrives, in two and a half hours. The position we give is uncertain by six miles: find it by sonar.", "The Americans heard the explosion on SOSUS. One of their submarines may already be on the slope, and an 'oceanographic' ship is coming from the west.", "She must not get near the wreck. Make yourself heard, show yourself: active sonar is allowed, weapons are not.", "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours 40 minutes." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [41] = { it = "La nave americana e' sul relitto: il sommergibile da grande profondita' e' in acqua.", en = "The American ship is over the wreck: the deep-diving submersible is in the water." },
    [40] = { it = "La nave di soccorso e' sul relitto: i codici sono al sicuro.", en = "The rescue ship is over the wreck: the codes are safe." },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_I10_CORSA_AL_RELITTO", titolo = { it = "Corsa al relitto", en = "Race to the Wreck" }, autore = "Libreria di Melchior 688",
    teatro = "celtica", alfa = true, centro = { lat = 48.25, lon = -9.75 },
    data = { giorno = 2, mese = 8, anno = 1989 }, ora_utc = 16.666666666666668, meteo_realistico = false,
    situazione = "pace", paranoia = 2, difficolta = 2, durata_s = 9600, libreria = 2,
    carico = { dotazione = { { "type53", 18 }, { "esca", 4 }, }, deposito = { { "type53", 18 }, { "esca", 4 }, { "esca_mobile", 2 }, }, tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", parte = "urss", ruolo = "", flag = 17, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = -87242.224272131483, y = 86848.465007158942 }, quota = 80, rotta = 275, velocita = 6, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 2, classe = "charlie2", categoria = "sottomarino", parte = "urss", ruolo = "relitto", flag = 81, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -131172.091808078, y = 81646.738614513539 }, quota = 400, rotta = 0, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "sturgeon", categoria = "sottomarino", parte = "usa", ruolo = "sottomarino americano", flag = 9, giocatore = false, non_combattente = false, nascosta = true,
        pos = { x = -133634.11119060853, y = 81212.618170346206 }, quota = 200, rotta = 0, velocita = 3, punti = { { x = -133634.11119060853, y = 81212.618170346206 }, { x = -131606.21225224531, y = 84108.757997044057 }, { x = -128710.07242554748, y = 82080.859058680871 }, { x = -130737.97136391066, y = 79184.719231983021 }, },
        ciclica = true, esce = false, comportamento = "R_sottomarino", profonda = 350, entra_con = "",
        parametri = { } },
      { id = 4, classe = "mercantile", categoria = "nave", parte = "usa", ruolo = "nave oceanografica", flag = 74, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -166172.091808078, y = 81646.738614513539 }, quota = 0, rotta = 90, velocita = 9, punti = { { x = -131172.091808078, y = 81646.738614513539 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 5, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "pattugliatore", flag = 11, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -152822.72690268897, y = 94146.738614513539 }, quota = 0, rotta = 0, velocita = 200, punti = { { x = -117029.95618434704, y = 95788.874238244491 }, { x = -117029.95618434704, y = 67504.602990782587 }, { x = -145314.22743180895, y = 67504.602990782587 }, { x = -145314.22743180895, y = 95788.874238244491 }, },
        ciclica = true, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 6, classe = "mercantile", categoria = "nave", parte = "urss", ruolo = "nave di soccorso", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -128172.091808078, y = 81646.738614513539 }, quota = 0, rotta = 270, velocita = 4, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "soccorso",
        parametri = { } },
    },
    obiettivi = {
      { id = "relitto", tipo = "raggiungere", unita = {  }, minimo = -1, cerchio = { x = -131172.091808078, y = 81646.738614513539, r = 3704 }, entro_s = -1, primario = true, colpire = true, testo = { it = "Raggiungere il relitto", en = "Reach the wreck" } },
      { id = "non_sparare", tipo = "proteggere", unita = { 3, 4, 5 }, minimo = -1, entro_s = -1, primario = true, colpire = false, testo = { it = "Non usare le armi", en = "Do not use weapons" } },
      { id = "sturgeon", tipo = "scoprire", unita = { 3 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Classificare il sottomarino americano", en = "Classify the American submarine" } },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 5, base_unita = -1, base = { x = 337295.90665046079, y = 254119.57212878126 }, situazione = "pace", contro_giocatore = true, bersagli = {  }, zona = { x = -131172.091808078, y = 81646.738614513539 }, raggio = 30000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: CORSA AL RELITTO", "2 agosto 1989, ore 1640Z. Teatro: Margine della Piattaforma Celtica.", "Un nostro sottomarino e' affondato tre giorni fa sulla scarpata di Goban Spur, con i suoi codici e un reattore spento.", "Sorvegliate il relitto finche' non arriva la nave di soccorso, fra due ore e mezza. La posizione che vi diamo e' incerta di sei miglia: cercatelo col sonar.", "Gli americani hanno sentito l'esplosione col SOSUS. Un loro sottomarino potrebbe essere gia' sulla scarpata, e una nave «oceanografica» arriva da ovest.", "Non deve avvicinarsi al relitto. Fatevi sentire, mostratevi: il sonar attivo e' permesso, le armi no.", "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 2 ore e 40 minuti.", }, en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: RACE TO THE WRECK", "2 August 1989, 1640Z. Theatre: Celtic Shelf margin.", "One of our submarines sank three days ago on the Goban Spur slope, with her codes and a shut-down reactor.", "Guard the wreck until the rescue ship arrives, in two and a half hours. The position we give is uncertain by six miles: find it by sonar.", "The Americans heard the explosion on SOSUS. One of their submarines may already be on the slope, and an 'oceanographic' ship is coming from the west.", "She must not get near the wreck. Make yourself heard, show yourself: active sonar is allowed, weapons are not.", "Rules of engagement: peacetime. Weapons hold.", "Time available: 2 hours 40 minutes.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "relitto", nome = { it = "il relitto", en = "the wreck" }, cerchio = { x = -131172.091808078, y = 81646.738614513539, r = 3704 } },
      { id = "avvicinamento", nome = { it = "avvicinamento", en = "approach" }, cerchio = { x = -131172.091808078, y = 81646.738614513539, r = 11112 } },
    },
    regole = {
      { id = "presidio", una_volta = true,
        quando = {
          { tipo = "giocatore_in_zona", valore = 0, unita = -1, indice = 0, zona = "relitto", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "nave_devia", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 4, indice = 0, zona = "avvicinamento", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -183133.61603514431, y = 111646.73861451354 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "nave_arriva", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 4, indice = 0, zona = "relitto", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "fine", codice = 41, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "fallimento", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave americana e' sul relitto: il sommergibile da grande profondita' e' in acqua.", en = "The American ship is over the wreck: the deep-diving submersible is in the water." } },
        } },
      { id = "sturgeon_via", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 3, indice = 0, zona = "relitto", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "allarme", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -159362.87043165523, y = 71386.134314743453 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "soccorso", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 9000, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "al_sicuro", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 9300, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "regola_scattata", valore = 0, unita = -1, indice = 0, zona = "", regola = "soccorso", confronto = "=" },
        },
        allora = {
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave di soccorso e' sul relitto: i codici sono al sicuro.", en = "The rescue ship is over the wreck: the codes are safe." } },
        } },
    },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", schieramento = "urss", flag = 17, giocatore = true,
        posizione = { x = -87242.2, y = 86848.5 }, quota_m = 80, rotta = 275, velocita_kn = 6,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "charlie2", categoria = "sottomarino", schieramento = "urss", flag = 81,
        posizione = { x = -131172.1, y = 81646.7 }, quota_m = 400, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- relitto
      { id = 3, classe = "sturgeon", categoria = "sottomarino", schieramento = "usa", flag = 9,
        posizione = { x = -133634.1, y = 81212.6 }, quota_m = 200, rotta = 0, velocita_kn = 3,
        modello = { massima_kn = 26, virata_gradi_s = 3.6, accel_kn_s = 0.2, quota_m_s = 5.4864, quota_max_m = 400, integrita = 180, rumore = 8, sonar = 20, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -133634.1, y = 81212.6 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_sottomarino", reazione = nil, arrivo = "R_3_p2" } },   -- sottomarino americano (non nel briefing)
      { id = 4, classe = "mercantile", categoria = "nave", schieramento = "usa", flag = 74,
        posizione = { x = -166172.1, y = 81646.7 }, quota_m = 0, rotta = 90, velocita_kn = 9,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -131172.1, y = 81646.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- nave oceanografica
      { id = 5, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 11,
        posizione = { x = -152822.7, y = 94146.7 }, quota_m = 0, rotta = 0, velocita_kn = 200,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 19, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -117030, y = 95788.9 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = "R_5_p2" } },   -- pattugliatore
      { id = 6, classe = "mercantile", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = -128172.1, y = 2201646.7 }, quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- nave di soccorso, di riserva: entra con la regola soccorso
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 9600, unita = 0, gestore = "R_tempo" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 10, unita = 4, gestore = "R_regola_1" },
      { dopo_s = 10, unita = 4, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 3, gestore = "R_regola_3" },
      { dopo_s = 9000, unita = 0, gestore = "R_regola_4" },
      { dopo_s = 9300, unita = 0, gestore = "R_regola_5" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (2) e difficolta' (2)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 14          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [2] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 2, portata = 4, distanza = 2 },
  [3] = { parte = "usa", crociera = 0.126, comportamento = "R_sottomarino", rotta = true, profonda = 350,
      paranoia = 2, salve = 2, portata = 4, distanza = 2 },
  [4] = { parte = "usa", crociera = 0.492, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 2, distanza = 2 },
  [5] = { parte = "usa", crociera = 0.663, comportamento = "R_aereo", rotta = true, profonda = 0,
      paranoia = 2, salve = 2, portata = 2, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.219, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 2, portata = 2, distanza = 2 },
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
  m688.vai_a(u, -133634.1, 81212.6, 30, "R_3_p2")
  m688.salva_modello(u)
end
script.R_3_p2 = function(u)
  m688.vai_a(u, -131606.2, 84108.8, 30, "R_3_p3")
  m688.salva_modello(u)
end
script.R_3_p3 = function(u)
  m688.vai_a(u, -128710.1, 82080.9, 30, "R_3_p4")
  m688.salva_modello(u)
end
script.R_3_p4 = function(u)
  m688.vai_a(u, -130738, 79184.7, 30, "R_3_p1")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, -131172.1, 81646.7, 30, "")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, -117030, 95788.9, 30, "R_5_p2")
  m688.salva_modello(u)
end
script.R_5_p2 = function(u)
  m688.vai_a(u, -117030, 67504.6, 30, "R_5_p3")
  m688.salva_modello(u)
end
script.R_5_p3 = function(u)
  m688.vai_a(u, -145314.2, 67504.6, 30, "R_5_p4")
  m688.salva_modello(u)
end
script.R_5_p4 = function(u)
  m688.vai_a(u, -145314.2, 95788.9, 30, "R_5_p1")
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
  ["relitto"] = { -131172.1, 81646.7, 3704 },
  ["avvicinamento"] = { -131172.1, 81646.7, 11112 },
}
-- presidio
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) and m688.in_cerchio(u, table.unpack(ZONE["relitto"])) then
    m688.variabile(u, 200, 1)
    m688.variabile(u, 300, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- nave_devia
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) and m688.in_cerchio(u, table.unpack(ZONE["avvicinamento"])) and m688.variabile_uguale(u, 300, 1) then
    m688.variabile(u, 201, 1)
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.vai_a(u, -183133.6, 111646.7, 30, "")
      m688.salva_modello(u)
      UNITA[4].rotta = true
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- nave_arriva
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) and m688.in_cerchio(u, table.unpack(ZONE["relitto"])) then
    m688.variabile(u, 202, 1)
    m688.messaggio(u, 141)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- sturgeon_via
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) and m688.in_cerchio(u, table.unpack(ZONE["relitto"])) and m688.variabile_uguale(u, 300, 1) and m688.variabile_uguale(u, 11, 1) then
    m688.variabile(u, 203, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.vai_a(u, -159362.9, 71386.1, 30, "")
      m688.salva_modello(u)
      UNITA[3].rotta = true
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.nessun_comportamento(u)
      m688.salva_modello(u)
      UNITA[3].comportamento = ""
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- soccorso
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) then
    m688.variabile(u, 204, 1)
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.posizione(u, -128172.1, 81646.7)
      m688.rotta_ora(u, 270)
      m688.velocita_ora(u, 4)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- al_sicuro
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) and m688.variabile_uguale(u, 204, 1) then
    m688.variabile(u, 205, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end

return missione
