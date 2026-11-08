-- LIB_I09_SOTTO_IL_BASTIONE: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_I09_SOTTO_IL_BASTIONE",
  titolo = { it = "Sotto il bastione", en = "Under the Bastion" },
  lato = "usa",
  teatro = "barents",
  posa = { metodo = "rapida", centro = { lat = 70.15, lon = 35 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-01-15", ora_utc = 9,
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
    { unita = 4, missione = "caccia", situazione = "pace", bersagli = "giocatore", equipaggio = "esperto",
      base = { x = -63127, y = -124126.8 }, zona = { x = -65219.8, y = -26980.9, raggio_m = 30000 } },
  },
  obiettivi = {
    { id = "foto", tipo = "scoprire", unita = { 2 },
      testo = { it = "Fotografare la nave nuova (classificarla, nella versione semplificata)", en = "Photograph the new ship (classify her, in the simplified version)" } },
    { id = "occulto", tipo = "non_essere_scoperto",
      testo = { it = "Non farsi scoprire", en = "Avoid detection" } },
    { id = "fuori", tipo = "variabile", minimo = 1, indice = 301, almeno = 1,
      testo = { it = "Uscire dalle acque sovietiche", en = "Get out of Soviet waters" } },
    { id = "sonar", tipo = "contatto", unita = { 2 }, primario = false, minuti = 20,
      testo = { it = "Registrare 20 minuti della nave nuova", en = "Record 20 minutes of the new ship" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: SOTTO IL BASTIONE", "15 gennaio 1989, ore 0900Z. Teatro: Mare di Barents, approcci al golfo. Notte polare.", "Un satellite ha fotografato una nave nuova, con una cupola di prua che nessun analista sa leggere: forse un sonar a bassa frequenza di una generazione nuova.", "Entrate, fotografatela mentre esce per le prove in mare e registrate quanto potete del suo sonar. Poi uscite dalle acque sovietiche senza essere scoperti.", "Quando lo accendera', quel sonar potrebbe vedervi fino a trenta miglia: restate sotto lo strato.", "Nessuno firmera' mai l'ordine che avete in mano.", "Regole d'ingaggio: tempo di pace. Armi trattenute; fuoco solo per salvare il battello.", "Tempo a disposizione: 3 ore.", "Nota storica: missione inedita; le intrusioni americane nel Mare di Barents sono reali, questo episodio no; nomi e unita' sono di fantasia." },
    en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: UNDER THE BASTION", "15 January 1989, 0900Z. Theatre: Barents Sea, inlet approaches. Polar night.", "A satellite has photographed a new ship, with a bow dome no analyst can read: perhaps a low-frequency sonar of a new generation.", "Go in, photograph her as she comes out for sea trials and record as much of her sonar as you can. Then get out of Soviet waters undetected.", "Once it is switched on, that sonar may see you out to thirty miles: stay below the layer.", "Nobody will ever sign the orders you are holding.", "Rules of engagement: peacetime. Weapons hold; fire only to save the boat.", "Time available: 3 hours.", "Historical note: an original mission; American incursions into the Barents Sea are real, this episode is not; names and units are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [40] = { it = "Le foto sono a bordo e il Los Angeles e' fuori dalle acque sovietiche.", en = "The photographs are aboard and Los Angeles is out of Soviet waters." },
  },
  meteo = { fisso = { vento_m_s = 12, vento_da_gradi = 20, stato_mare = 4, visibilita_m = 2000, neve_mm_h = 2, nubi_ottavi = 8, } },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_I09_SOTTO_IL_BASTIONE", titolo = { it = "Sotto il bastione", en = "Under the Bastion" }, autore = "Libreria di Melchior 688",
    teatro = "barents", alfa = false, centro = { lat = 70.150000000000006, lon = 35 },
    data = { giorno = 15, mese = 1, anno = 1989 }, ora_utc = 9, meteo_realistico = true,
    situazione = "pace", paranoia = 3, difficolta = 5, durata_s = 10800, libreria = 2,
    carico = { dotazione = { { "mk48", 22 }, { "harpoon", 2 }, { "esca", 6 }, }, deposito = { { "mk48", 21 }, { "harpoon", 2 }, { "esca", 6 }, }, tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "", flag = 9, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = -90305.757016553762, y = 18514.532979904387 }, quota = 80, rotta = 151.12791064509403, velocita = 6, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 2, classe = "slava", categoria = "nave", parte = "urss", ruolo = "la nave nuova", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -71987.391917119283, y = -60266.272366661731 }, quota = 0, rotta = 11.492711320585553, velocita = 10, punti = { { x = -65219.821497040241, y = -20980.927983924488 }, { x = -59219.821497040241, y = -26980.927983924488 }, { x = -65219.821497040241, y = -32980.927983924485 }, { x = -71219.821497040248, y = -26980.927983924488 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "victor3", categoria = "sottomarino", parte = "urss", ruolo = "pattuglia", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -79710.202553806826, y = -20980.927983924485 }, quota = 100, rotta = 120, velocita = 5, punti = { { x = -52229.440440273662, y = -34480.927983924485 }, { x = -79710.202553806826, y = -20980.927983924485 }, },
        ciclica = true, esce = false, comportamento = "R_sottomarino", profonda = 150, entra_con = "",
        parametri = { } },
      { id = 4, classe = "il38", categoria = "aereo", parte = "urss", ruolo = "pattugliatore", flag = 19, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -65219.821497040241, y = 3019.0720160755118 }, quota = 0, rotta = 90, velocita = 250, punti = { { x = -47542.151967376558, y = -9303.2584542607983 }, { x = -47542.151967376551, y = -44658.597513588174 }, { x = -82897.491026703923, y = -44658.597513588182 }, { x = -82897.491026703938, y = -9303.2584542608056 }, },
        ciclica = true, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 5, classe = "krivak2", categoria = "nave", parte = "urss", ruolo = "barriera", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -71987.391917119283, y = -58266.272366661731 }, quota = 0, rotta = 20, velocita = 24, punti = { { x = -66704.859728801923, y = 17006.697477945519 }, },
        ciclica = false, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "barriera",
        parametri = { } },
      { id = 6, classe = "krivak2", categoria = "nave", parte = "urss", ruolo = "barriera", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -71987.391917119283, y = -56766.272366661731 }, quota = 0, rotta = 20, velocita = 24, punti = { { x = -45920.250037975391, y = 17006.697477945527 }, },
        ciclica = false, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "barriera",
        parametri = { } },
    },
    obiettivi = {
      { id = "foto", tipo = "scoprire", unita = { 2 }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Fotografare la nave nuova (classificarla, nella versione semplificata)", en = "Photograph the new ship (classify her, in the simplified version)" } },
      { id = "occulto", tipo = "non_essere_scoperto", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Non farsi scoprire", en = "Avoid detection" } },
      { id = "fuori", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Uscire dalle acque sovietiche", en = "Get out of Soviet waters" }, indice = 1 },
      { id = "sonar", tipo = "contatto", unita = { 2 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Registrare 20 minuti della nave nuova", en = "Record 20 minutes of the new ship" }, minuti = 20 },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 4, base_unita = -1, base = { x = -63126.990800130872, y = -124126.77103452636 }, situazione = "pace", contro_giocatore = true, bersagli = {  }, zona = { x = -65219.821497040241, y = -26980.927983924488 }, raggio = 30000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: SOTTO IL BASTIONE", "15 gennaio 1989, ore 0900Z. Teatro: Mare di Barents, approcci al golfo. Notte polare.", "Un satellite ha fotografato una nave nuova, con una cupola di prua che nessun analista sa leggere: forse un sonar a bassa frequenza di una generazione nuova.", "Entrate, fotografatela mentre esce per le prove in mare e registrate quanto potete del suo sonar. Poi uscite dalle acque sovietiche senza essere scoperti.", "Quando lo accendera', quel sonar potrebbe vedervi fino a trenta miglia: restate sotto lo strato.", "Nessuno firmera' mai l'ordine che avete in mano.", "Regole d'ingaggio: tempo di pace. Armi trattenute; fuoco solo per salvare il battello.", "Tempo a disposizione: 3 ore.", "Nota storica: missione inedita; le intrusioni americane nel Mare di Barents sono reali, questo episodio no; nomi e unita' sono di fantasia.", }, en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: UNDER THE BASTION", "15 January 1989, 0900Z. Theatre: Barents Sea, inlet approaches. Polar night.", "A satellite has photographed a new ship, with a bow dome no analyst can read: perhaps a low-frequency sonar of a new generation.", "Go in, photograph her as she comes out for sea trials and record as much of her sonar as you can. Then get out of Soviet waters undetected.", "Once it is switched on, that sonar may see you out to thirty miles: stay below the layer.", "Nobody will ever sign the orders you are holding.", "Rules of engagement: peacetime. Weapons hold; fire only to save the boat.", "Time available: 3 hours.", "Historical note: an original mission; American incursions into the Barents Sea are real, this episode is not; names and units are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "portata", nome = { it = "la portata del sonar nuovo", en = "the new sonar's reach" }, cerchio = { x = -65219.821497040241, y = -26980.927983924488, r = 55560 } },
      { id = "uscita", nome = { it = "fuori dalle acque sovietiche", en = "out of Soviet waters" }, cerchio = { x = -56312.554883388657, y = 23006.697477945523, r = 8000 } },
    },
    regole = {
      { id = "il_sonar_vede", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 2400, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "giocatore_in_zona", valore = 0, unita = -1, indice = 0, zona = "portata", regola = "", confronto = "=" },
          { tipo = "giocatore_a_quota", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", fino_a = 60 },
        },
        allora = {
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "cue_asw", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -65219.821497040241, y = -26980.927983924488 }, raggio = 15000, testo = { it = "", en = "" } },
        } },
      { id = "barriera", una_volta = true,
        quando = {
          { tipo = "allarme", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "barriera_a_tempo", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 5400, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "entra", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fuori", una_volta = true,
        quando = {
          { tipo = "obiettivo_compiuto", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", obiettivo = "foto" },
          { tipo = "giocatore_in_zona", valore = 0, unita = -1, indice = 0, zona = "uscita", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 1, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Le foto sono a bordo e il Los Angeles e' fuori dalle acque sovietiche.", en = "The photographs are aboard and Los Angeles is out of Soviet waters." } },
        } },
    },
    meteo_fisso = { vento_m_s = 12, vento_da_gradi = 20, stato_mare = 4, visibilita_m = 2000, neve_mm_h = 2, nubi_ottavi = 8, nebbia = "" },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9, giocatore = true,
        posizione = { x = -90305.8, y = 18514.5 }, quota_m = 80, rotta = 151.1, velocita_kn = 6,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "slava", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = -71987.4, y = -60266.3 }, quota_m = 0, rotta = 11.5, velocita_kn = 10,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -65219.8, y = -20980.9 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_2_p2" } },   -- la nave nuova
      { id = 3, classe = "victor3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -79710.2, y = -20980.9 }, quota_m = 100, rotta = 120, velocita_kn = 5,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 20, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -52229.4, y = -34480.9 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_sottomarino", reazione = nil, arrivo = "R_3_p2" } },   -- pattuglia
      { id = 4, classe = "il38", categoria = "aereo", schieramento = "urss", flag = 19,
        posizione = { x = -65219.8, y = 3019.1 }, quota_m = 0, rotta = 90, velocita_kn = 250,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 22, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -47542.2, y = -9303.3 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = "R_4_p2" } },   -- pattugliatore
      { id = 5, classe = "krivak2", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = -71987.4, y = 2041733.7 }, quota_m = 0, rotta = 20, velocita_kn = 0,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 20, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = nil } },   -- barriera, di riserva: entra con la regola barriera
      { id = 6, classe = "krivak2", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = -71987.4, y = 2063233.7 }, quota_m = 0, rotta = 20, velocita_kn = 0,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 20, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = nil } },   -- barriera, di riserva: entra con la regola barriera
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 10800, unita = 0, gestore = "R_tempo" },
      { dopo_s = 65, unita = 5, gestore = "R_vigila" },
      { dopo_s = 72, unita = 6, gestore = "R_vigila" },
      { dopo_s = 2400, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_1" },
      { dopo_s = 5400, unita = 0, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_3" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (3) e difficolta' (5)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 6          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [2] = { parte = "urss", crociera = 0.365, comportamento = "", rotta = true, profonda = 0,
      paranoia = 3, salve = 3, portata = 3, distanza = 2 },
  [3] = { parte = "urss", crociera = 0.13, comportamento = "R_sottomarino", rotta = true, profonda = 150,
      paranoia = 3, salve = 3, portata = 5, distanza = 2 },
  [4] = { parte = "urss", crociera = 0.829, comportamento = "R_aereo", rotta = true, profonda = 0,
      paranoia = 3, salve = 3, portata = 3, distanza = 2 },
  [5] = { parte = "urss", crociera = 0.875, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 3, salve = 3, portata = 3, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.875, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 3, salve = 3, portata = 3, distanza = 2 },
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
script.R_2_p1 = function(u)
  m688.vai_a(u, -65219.8, -20980.9, 30, "R_2_p2")
  m688.salva_modello(u)
end
script.R_2_p2 = function(u)
  m688.vai_a(u, -59219.8, -26980.9, 30, "R_2_p3")
  m688.salva_modello(u)
end
script.R_2_p3 = function(u)
  m688.vai_a(u, -65219.8, -32980.9, 30, "R_2_p4")
  m688.salva_modello(u)
end
script.R_2_p4 = function(u)
  m688.vai_a(u, -71219.8, -26980.9, 30, "R_2_p1")
  m688.salva_modello(u)
end
script.R_3_p1 = function(u)
  m688.vai_a(u, -52229.4, -34480.9, 30, "R_3_p2")
  m688.salva_modello(u)
end
script.R_3_p2 = function(u)
  m688.vai_a(u, -79710.2, -20980.9, 30, "R_3_p1")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, -47542.2, -9303.3, 30, "R_4_p2")
  m688.salva_modello(u)
end
script.R_4_p2 = function(u)
  m688.vai_a(u, -47542.2, -44658.6, 30, "R_4_p3")
  m688.salva_modello(u)
end
script.R_4_p3 = function(u)
  m688.vai_a(u, -82897.5, -44658.6, 30, "R_4_p4")
  m688.salva_modello(u)
end
script.R_4_p4 = function(u)
  m688.vai_a(u, -82897.5, -9303.3, 30, "R_4_p1")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, -66704.9, 17006.7, 30, "")
  m688.salva_modello(u)
end
script.R_6_p1 = function(u)
  m688.vai_a(u, -45920.3, 17006.7, 30, "")
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
  ["portata"] = { -65219.8, -26980.9, 55560 },
  ["uscita"] = { -56312.6, 23006.7, 8000 },
}
-- il_sonar_vede
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) and m688.in_cerchio(u, table.unpack(ZONE["portata"])) and m688.quota_entro(u, 0, 60) then
    m688.variabile(u, 200, 1)
    m688.variabile(u, 11, 1)
    m688.cue_asw(u, -1, -65219.8, -26980.9, 15000, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- barriera
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) and m688.variabile_uguale(u, 11, 1) and m688.variabile_uguale(u, 300, 0) then
    m688.variabile(u, 201, 1)
    m688.variabile(u, 300, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.posizione(u, -71987.4, -58266.3)
      m688.rotta_ora(u, 20)
      m688.velocita_ora(u, 24)
      script.R_5_p1(u)
    end
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.posizione(u, -71987.4, -56766.3)
      m688.rotta_ora(u, 20)
      m688.velocita_ora(u, 24)
      script.R_6_p1(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- barriera_a_tempo
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) and m688.variabile_uguale(u, 300, 0) then
    m688.variabile(u, 202, 1)
    m688.variabile(u, 300, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.posizione(u, -71987.4, -58266.3)
      m688.rotta_ora(u, 20)
      m688.velocita_ora(u, 24)
      script.R_5_p1(u)
    end
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.posizione(u, -71987.4, -56766.3)
      m688.rotta_ora(u, 20)
      m688.velocita_ora(u, 24)
      script.R_6_p1(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- fuori
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) and m688.obiettivo_compiuto(u, "foto") and m688.in_cerchio(u, table.unpack(ZONE["uscita"])) then
    m688.variabile(u, 203, 1)
    m688.variabile(u, 301, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end

return missione
