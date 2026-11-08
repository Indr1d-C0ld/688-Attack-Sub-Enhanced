-- LIB_S22_ROTTA_LUNGA: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_S22_ROTTA_LUNGA",
  titolo = { it = "La rotta lunga", en = "The Long Way Round" },
  lato = "urss",
  teatro = "celtica",
  posa = { metodo = "rapida", centro = { lat = 48.25, lon = -9.75 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-04-20", ora_utc = 6,
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
    { unita = 7, missione = "caccia", situazione = "pace", bersagli = {  }, equipaggio = "esperto",
      zona = { x = -79082.6, y = 7435.9, raggio_m = 20000 } },
    { unita = 8, missione = "caccia", situazione = "pace", bersagli = {  }, equipaggio = "esperto",
      zona = { x = -55428.7, y = -12205.3, raggio_m = 20000 } },
  },
  categorie_esiti = { [42] = "fallimento", [41] = "fallimento" },
  obiettivi = {
    { id = "liberi", tipo = "variabile", minimo = 1, indice = 308, almeno = 1,
      testo = { it = "Almeno tre battelli su cinque (Alfa compreso) senza inseguitore", en = "At least three boats out of five (Alfa included) with no pursuer" } },
    { id = "ohio", tipo = "scoprire", unita = { 11 }, primario = false,
      testo = { it = "Trovare e classificare l'SSBN americano", en = "Find and classify the American SSBN" } },
    { id = "alfa_libero", tipo = "variabile", minimo = 1, primario = false, indice = 309, almeno = 1,
      testo = { it = "L'Alfa tenuto meno di 30 minuti in tutto", en = "The Alfa held for less than 30 minutes in all" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: ATRINA", "20 aprile 1989, ore 0600Z. Teatro: Atlantico, a sud-ovest della Piattaforma Celtica. Pace.", "Cinque sottomarini d'attacco sono usciti insieme e hanno preso la rotta lunga, attorno alla Groenlandia, per non passare sopra il SOSUS. Ora siete nell'Atlantico aperto, su cinque rotte parallele verso sud-ovest.", "Il comando vuole sapere se la NATO vi ritrova, e se nel settore pattuglia un lanciamissili americano.", "Contano gli altri quattro quanto voi: almeno tre battelli su cinque devono arrivare in fondo senza nessuno in coda.", "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 4 ore.", "Nota storica: ispirata all'operazione Atrina (marzo-maggio 1987), con cinque Victor III. Per gli autori russi un trionfo, per le fonti occidentali almeno quattro battelli su cinque furono seguiti. Nessun Alfa vi prese parte: qui l'Alfa ha il ruolo di uno di loro. Nomi e unita' di fantasia." },
    en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: ATRINA", "20 April 1989, 0600Z. Theatre: Atlantic, south-west of the Celtic Shelf. Peace.", "Five attack submarines have sailed together and taken the long way round, around Greenland, to stay off the SOSUS. Now you are in the open Atlantic, on five parallel tracks heading south-west.", "The command wants to know whether NATO finds you again, and whether an American missile boat patrols the sector.", "The other four count as much as you do: at least three boats out of five must reach the end with nobody on their tail.", "Rules of engagement: peacetime. Weapons hold.", "Time available: 4 hours.", "Historical note: inspired by Operation Atrina (March-May 1987), with five Victor IIIs. For Russian authors a triumph; for Western sources at least four boats out of five were tracked. No Alfa took part: here the Alfa plays the role of one of them. Names and units are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [42] = { it = "Un'arma in acqua in tempo di pace: l'operazione e' finita, e da domani sara' un'altra storia.", en = "A weapon in the water in peacetime: the operation is over, and tomorrow will be another story." },
    [40] = { it = "Almeno tre battelli su cinque senza nessuno in coda. A Mosca diranno che la NATO non li ha visti; a Norfolk diranno il contrario.", en = "At least three boats out of five with nobody on their tail. Moscow will say NATO never saw them; Norfolk will say otherwise." },
    [41] = { it = "La NATO ne tiene troppi. Il rapporto dira' che l'operazione e' riuscita; chi era a bordo sa che non e' vero.", en = "NATO is holding too many. The report will say the operation succeeded; those aboard know it did not." },
  },
  radio = {
    ordini = { da = "Comando della Flotta del Nord", a = "K-463 (PROGETTO 705)", oggetto = "ATRINA", classifica = "SEGRETISSIMO", data = { giorno = 20, mese = 4, anno = 1989, ora_utc = 6 } },
    stato_iniziale = { roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, neutrali = { 12, 13, } },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_S22_ROTTA_LUNGA", titolo = { it = "La rotta lunga", en = "The Long Way Round" }, autore = "Libreria di Melchior 688",
    teatro = "celtica", alfa = true, centro = { lat = 48.25, lon = -9.75 },
    data = { giorno = 20, mese = 4, anno = 1989 }, ora_utc = 6, meteo_realistico = true,
    situazione = "pace", paranoia = 2, difficolta = 4, durata_s = 14400, libreria = 2,
    carico = { dotazione = { { "type53", 18 }, { "esca", 4 }, }, deposito = { { "type53", 18 }, { "esca", 4 }, { "esca_mobile", 2 }, }, tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", parte = "urss", ruolo = "", flag = 17, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = -59178.295279796665, y = 22547.978394902795 }, quota = 200, rotta = 210, velocita = 4, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "victor3", categoria = "sottomarino", parte = "urss", ruolo = "Victor III del gruppo", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -18529.774593404658, y = 11149.791114675421 }, quota = 150, rotta = 180, velocita = 7, punti = { { x = -18529.774593404651, y = -58850.20888532458 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { paranoia = 2, } },
      { id = 4, classe = "victor3", categoria = "sottomarino", parte = "urss", ruolo = "Victor III del gruppo", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -44471.084572792861, y = 11293.426812572476 }, quota = 180, rotta = 205, velocita = 7, punti = { { x = -74054.362894641818, y = -52148.118279993025 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { paranoia = 2, } },
      { id = 5, classe = "victor3", categoria = "sottomarino", parte = "urss", ruolo = "Victor III del gruppo", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -74117.131702178856, y = 11602.423781513598 }, quota = 210, rotta = 230, velocita = 7, punti = { { x = -127740.24272050732, y = -33392.708896544165 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { paranoia = 2, } },
      { id = 6, classe = "victor3", categoria = "sottomarino", parte = "urss", ruolo = "Victor III del gruppo", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -100055.79842431066, y = 11999.53149319001 }, quota = 240, rotta = 255, velocita = 7, punti = { { x = -166670.60626454544, y = -7117.8016639864327 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { paranoia = 2, } },
      { id = 7, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "P-3C sul datum del SOSUS", flag = 11, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 25457.293795619436, y = 116823.96733361014 }, quota = 0, rotta = 220, velocita = 250, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "sosus_1",
        parametri = { } },
      { id = 8, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "P-3C sul datum del SOSUS", flag = 11, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 3636.7687914605635, y = 116766.83158383006 }, quota = 0, rotta = 220, velocita = 250, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "sosus_2",
        parametri = { } },
      { id = 9, classe = "kidd", categoria = "nave", parte = "usa", ruolo = "cacciatorpediniere sui datum", flag = 10, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -41238.709611718034, y = -55321.260085283837 }, quota = 0, rotta = 270, velocita = 10, punti = { { x = -81238.709611718034, y = -55321.260085283844 }, { x = -31238.709611718034, y = -55321.260085283837 }, },
        ciclica = true, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 10, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "SSN di barriera", flag = 9, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -56238.709611718034, y = -43321.260085283837 }, quota = 200, rotta = 30, velocita = 5, punti = { { x = -46339.21467510637, y = -45421.765148672173 }, { x = -46339.21467510637, y = -65220.7550218955 }, { x = -66138.204548329697, y = -65220.7550218955 }, { x = -66138.204548329697, y = -45421.765148672173 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 15, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "secondo SSN di barriera", flag = 9, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -77733.178026306909, y = -21597.394471305586 }, quota = 180, rotta = 0, velocita = 5, punti = { { x = -85733.178026306909, y = -12597.394471305586 }, { x = -76733.178026306909, y = -21597.394471305586 }, { x = -85733.178026306909, y = -30597.394471305586 }, { x = -94733.178026306909, y = -21597.394471305586 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 11, classe = "ohio", categoria = "sottomarino", parte = "usa", ruolo = "SSBN americano in pattuglia", flag = 9, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = -86974.392619865306, y = -98978.64366726717 }, quota = 150, rotta = 90, velocita = 3, punti = { { x = -80974.392619865306, y = -104978.64366726717 }, { x = -86974.392619865306, y = -110978.64366726717 }, { x = -92974.392619865306, y = -104978.64366726717 }, { x = -86974.392619865306, y = -98978.64366726717 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 12, classe = "mercantile", categoria = "nave", parte = "neutrale", ruolo = "traffico atlantico", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = -104307.23688185347, y = 71287.989048315765 }, quota = 0, rotta = 120, velocita = 13, punti = { { x = -35025.204579098383, y = 31287.98904831578 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 13, classe = "portarinfuse", categoria = "nave", parte = "neutrale", ruolo = "traffico atlantico", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 11366.385651808379, y = -116733.17677760584 }, quota = 0, rotta = 300, velocita = 13, punti = { { x = -57915.646650946714, y = -76733.17677760584 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
    },
    obiettivi = {
      { id = "liberi", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Almeno tre battelli su cinque (Alfa compreso) senza inseguitore", en = "At least three boats out of five (Alfa included) with no pursuer" }, indice = 8 },
      { id = "ohio", tipo = "scoprire", unita = { 11 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Trovare e classificare l'SSBN americano", en = "Find and classify the American SSBN" } },
      { id = "alfa_libero", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = false, colpire = true, testo = { it = "L'Alfa tenuto meno di 30 minuti in tutto", en = "The Alfa held for less than 30 minutes in all" }, indice = 9 },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 7, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = {  }, zona = { x = -79082.631782476077, y = 7435.8744955254497 }, raggio = 20000 },
      { unita = 8, base_unita = -1, situazione = "pace", contro_giocatore = false, bersagli = {  }, zona = { x = -55428.73086320571, y = -12205.321489713786 }, raggio = 20000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: Comando della Flotta del Nord", "A: K-463 (PROGETTO 705)", "OGG.: ATRINA", "20 aprile 1989, ore 0600Z. Teatro: Atlantico, a sud-ovest della Piattaforma Celtica. Pace.", "Cinque sottomarini d'attacco sono usciti insieme e hanno preso la rotta lunga, attorno alla Groenlandia, per non passare sopra il SOSUS. Ora siete nell'Atlantico aperto, su cinque rotte parallele verso sud-ovest.", "Il comando vuole sapere se la NATO vi ritrova, e se nel settore pattuglia un lanciamissili americano.", "Contano gli altri quattro quanto voi: almeno tre battelli su cinque devono arrivare in fondo senza nessuno in coda.", "Regole d'ingaggio: tempo di pace. Armi trattenute.", "Tempo a disposizione: 4 ore.", "Nota storica: ispirata all'operazione Atrina (marzo-maggio 1987), con cinque Victor III. Per gli autori russi un trionfo, per le fonti occidentali almeno quattro battelli su cinque furono seguiti. Nessun Alfa vi prese parte: qui l'Alfa ha il ruolo di uno di loro. Nomi e unita' di fantasia.", }, en = { "MISSION ORDERS", "FM: NORTHERN FLEET HQ", "TO: K-463 (PROJECT 705)", "SUBJ: ATRINA", "20 April 1989, 0600Z. Theatre: Atlantic, south-west of the Celtic Shelf. Peace.", "Five attack submarines have sailed together and taken the long way round, around Greenland, to stay off the SOSUS. Now you are in the open Atlantic, on five parallel tracks heading south-west.", "The command wants to know whether NATO finds you again, and whether an American missile boat patrols the sector.", "The other four count as much as you do: at least three boats out of five must reach the end with nobody on their tail.", "Rules of engagement: peacetime. Weapons hold.", "Time available: 4 hours.", "Historical note: inspired by Operation Atrina (March-May 1987), with five Victor IIIs. For Russian authors a triumph; for Western sources at least four boats out of five were tracked. No Alfa took part: here the Alfa plays the role of one of them. Names and units are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
    },
    regole = {
      { id = "partenza", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "sosus_1", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 1800, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 7, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "cue_asw", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -79082.631782476077, y = 7435.8744955254497 }, raggio = 8000, testo = { it = "", en = "" } },
        } },
      { id = "sosus_2", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 7200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "cue_asw", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -55428.73086320571, y = -12205.321489713786 }, raggio = 8000, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_0_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_0_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_0_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_0_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_0_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "libero_0", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 2, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_2_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 3 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_2_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 3 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_2_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 3 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_2_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 3 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_2_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 3 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "libero_2", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 3, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_3_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 4 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_3_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 4 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_3_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 4 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_3_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 4 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_3_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 4 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 4, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "libero_3", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 4, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_4_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 5 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_4_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 5 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_4_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 5 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_4_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 5 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_4_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 5 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "libero_4", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_5_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 6 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_5_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 6 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_5_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 6 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_5_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 6 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "tenuto_5_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 70, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 6 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 6, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "libero_5", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14100, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 0, unita = -1, indice = 6, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 7, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_tenuto_6", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 30, unita = 7, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_tenuto_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 30, unita = 8, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_tenuto_8", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 30, unita = 9, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_tenuto_9", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 30, unita = 10, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "alfa_tenuto_14", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 30, unita = 15, indice = 0, zona = "", regola = "", confronto = "=", bersaglio = 0 },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 9, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "lancio", una_volta = true,
        quando = {
          { tipo = "giocatore_ha_lanciato", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "fine", codice = 42, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "fallimento", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Un'arma in acqua in tempo di pace: l'operazione e' finita, e da domani sara' un'altra storia.", en = "A weapon in the water in peacetime: the operation is over, and tomorrow will be another story." } },
        } },
      { id = "liberi", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14280, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 2, unita = -1, indice = 7, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 8, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Almeno tre battelli su cinque senza nessuno in coda. A Mosca diranno che la NATO non li ha visti; a Norfolk diranno il contrario.", en = "At least three boats out of five with nobody on their tail. Moscow will say NATO never saw them; Norfolk will say otherwise." } },
        } },
      { id = "ritrovati", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 14280, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 3, unita = -1, indice = 7, zona = "", regola = "", confronto = "<" },
        },
        allora = {
          { tipo = "fine", codice = 41, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "fallimento", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La NATO ne tiene troppi. Il rapporto dira' che l'operazione e' riuscita; chi era a bordo sa che non e' vero.", en = "NATO is holding too many. The report will say the operation succeeded; those aboard know it did not." } },
        } },
    },
    radio = { ordini = { da = "Comando della Flotta del Nord", a = "K-463 (PROGETTO 705)", oggetto = "ATRINA", classifica = "SEGRETISSIMO" }, roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, messaggi = {
    } },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", schieramento = "urss", flag = 17, giocatore = true,
        posizione = { x = -59178.3, y = 22548 }, quota_m = 200, rotta = 210, velocita_kn = 4,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "victor3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -18529.8, y = 11149.8 }, quota_m = 150, rotta = 180, velocita_kn = 7,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -18529.8, y = -58850.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- Victor III del gruppo
      { id = 4, classe = "victor3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -44471.1, y = 11293.4 }, quota_m = 180, rotta = 205, velocita_kn = 7,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -74054.4, y = -52148.1 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- Victor III del gruppo
      { id = 5, classe = "victor3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -74117.1, y = 11602.4 }, quota_m = 210, rotta = 230, velocita_kn = 7,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -127740.2, y = -33392.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- Victor III del gruppo
      { id = 6, classe = "victor3", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = -100055.8, y = 11999.5 }, quota_m = 240, rotta = 255, velocita_kn = 7,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -166670.6, y = -7117.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- Victor III del gruppo
      { id = 7, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 11,
        posizione = { x = 25457.3, y = 2256824 }, quota_m = 0, rotta = 220, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 21, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- P-3C sul datum del SOSUS, di riserva: entra con la regola sosus_1
      { id = 8, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 11,
        posizione = { x = 3636.8, y = 2276766.8 }, quota_m = 0, rotta = 220, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 21, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- P-3C sul datum del SOSUS, di riserva: entra con la regola sosus_2
      { id = 9, classe = "kidd", categoria = "nave", schieramento = "usa", flag = 10,
        posizione = { x = -41238.7, y = -55321.3 }, quota_m = 0, rotta = 270, velocita_kn = 10,
        modello = { massima_kn = 33, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 9, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -81238.7, y = -55321.3 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = "R_9_p2" } },   -- cacciatorpediniere sui datum
      { id = 10, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9,
        posizione = { x = -56238.7, y = -43321.3 }, quota_m = 200, rotta = 30, velocita_kn = 5,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 23, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -46339.2, y = -45421.8 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_10_p2" } },   -- SSN di barriera
      { id = 15, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9,
        posizione = { x = -77733.2, y = -21597.4 }, quota_m = 180, rotta = 0, velocita_kn = 5,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 23, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -85733.2, y = -12597.4 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_15_p2" } },   -- secondo SSN di barriera
      { id = 11, classe = "ohio", categoria = "sottomarino", schieramento = "usa", flag = 9,
        posizione = { x = -86974.4, y = -98978.6 }, quota_m = 150, rotta = 90, velocita_kn = 3,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 21, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -80974.4, y = -104978.6 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_11_p2" } },   -- SSBN americano in pattuglia
      { id = 12, classe = "mercantile", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = -104307.2, y = 71288 }, quota_m = 0, rotta = 120, velocita_kn = 13,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -35025.2, y = 31288 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- traffico atlantico
      { id = 13, classe = "portarinfuse", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 11366.4, y = -116733.2 }, quota_m = 0, rotta = 300, velocita_kn = 13,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -57915.6, y = -76733.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- traffico atlantico
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 14400, unita = 0, gestore = "R_tempo" },
      { dopo_s = 93, unita = 9, gestore = "R_vigila" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 1800, unita = 0, gestore = "R_regola_1" },
      { dopo_s = 7200, unita = 0, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_3" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_4" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_5" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_6" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_7" },
      { dopo_s = 14100, unita = 0, gestore = "R_regola_8" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_9" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_10" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_11" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_12" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_13" },
      { dopo_s = 14100, unita = 0, gestore = "R_regola_14" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_15" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_16" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_17" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_18" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_19" },
      { dopo_s = 14100, unita = 0, gestore = "R_regola_20" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_21" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_22" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_23" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_24" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_25" },
      { dopo_s = 14100, unita = 0, gestore = "R_regola_26" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_27" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_28" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_29" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_30" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_31" },
      { dopo_s = 14100, unita = 0, gestore = "R_regola_32" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_33" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_34" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_35" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_36" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_37" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_38" },
      { dopo_s = 14280, unita = 0, gestore = "R_regola_39" },
      { dopo_s = 14280, unita = 0, gestore = "R_regola_40" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (2) e difficolta' (4)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 9          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [3] = { parte = "urss", crociera = 0.182, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [4] = { parte = "urss", crociera = 0.182, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [5] = { parte = "urss", crociera = 0.182, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.182, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [7] = { parte = "usa", crociera = 0.829, comportamento = "R_aereo", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [8] = { parte = "usa", crociera = 0.829, comportamento = "R_aereo", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [9] = { parte = "usa", crociera = 0.331, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [10] = { parte = "usa", crociera = 0.148, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [15] = { parte = "usa", crociera = 0.148, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [11] = { parte = "usa", crociera = 0.078, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [12] = { parte = "neutrale", crociera = 0.711, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [13] = { parte = "neutrale", crociera = 0.948, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
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
  m688.vai_a(u, -18529.8, -58850.2, 30, "")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, -74054.4, -52148.1, 30, "")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, -127740.2, -33392.7, 30, "")
  m688.salva_modello(u)
end
script.R_6_p1 = function(u)
  m688.vai_a(u, -166670.6, -7117.8, 30, "")
  m688.salva_modello(u)
end
script.R_9_p1 = function(u)
  m688.vai_a(u, -81238.7, -55321.3, 30, "R_9_p2")
  m688.salva_modello(u)
end
script.R_9_p2 = function(u)
  m688.vai_a(u, -31238.7, -55321.3, 30, "R_9_p1")
  m688.salva_modello(u)
end
script.R_10_p1 = function(u)
  m688.vai_a(u, -46339.2, -45421.8, 30, "R_10_p2")
  m688.salva_modello(u)
end
script.R_10_p2 = function(u)
  m688.vai_a(u, -46339.2, -65220.8, 30, "R_10_p3")
  m688.salva_modello(u)
end
script.R_10_p3 = function(u)
  m688.vai_a(u, -66138.2, -65220.8, 30, "R_10_p4")
  m688.salva_modello(u)
end
script.R_10_p4 = function(u)
  m688.vai_a(u, -66138.2, -45421.8, 30, "R_10_p1")
  m688.salva_modello(u)
end
script.R_15_p1 = function(u)
  m688.vai_a(u, -85733.2, -12597.4, 30, "R_15_p2")
  m688.salva_modello(u)
end
script.R_15_p2 = function(u)
  m688.vai_a(u, -76733.2, -21597.4, 30, "R_15_p3")
  m688.salva_modello(u)
end
script.R_15_p3 = function(u)
  m688.vai_a(u, -85733.2, -30597.4, 30, "R_15_p4")
  m688.salva_modello(u)
end
script.R_15_p4 = function(u)
  m688.vai_a(u, -94733.2, -21597.4, 30, "R_15_p1")
  m688.salva_modello(u)
end
script.R_11_p1 = function(u)
  m688.vai_a(u, -80974.4, -104978.6, 30, "R_11_p2")
  m688.salva_modello(u)
end
script.R_11_p2 = function(u)
  m688.vai_a(u, -86974.4, -110978.6, 30, "R_11_p3")
  m688.salva_modello(u)
end
script.R_11_p3 = function(u)
  m688.vai_a(u, -92974.4, -104978.6, 30, "R_11_p4")
  m688.salva_modello(u)
end
script.R_11_p4 = function(u)
  m688.vai_a(u, -86974.4, -98978.6, 30, "R_11_p1")
  m688.salva_modello(u)
end
script.R_12_p1 = function(u)
  m688.vai_a(u, -35025.2, 31288, 30, "")
  m688.salva_modello(u)
end
script.R_13_p1 = function(u)
  m688.vai_a(u, -57915.6, -76733.2, 30, "")
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
    m688.variabile(u, 309, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- sosus_1
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) then
    m688.variabile(u, 201, 1)
    if m688.unita_esiste(u, 7) then
      m688.seleziona(u, 7)
      m688.posizione(u, 25457.3, 116824)
      m688.rotta_ora(u, 220)
      m688.velocita_ora(u, 250)
      m688.salva_modello(u)
    end
    m688.cue_asw(u, -1, -79082.6, 7435.9, 8000, 0)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- sosus_2
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) then
    m688.variabile(u, 202, 1)
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.posizione(u, 3636.8, 116766.8)
      m688.rotta_ora(u, 220)
      m688.velocita_ora(u, 250)
      m688.salva_modello(u)
    end
    m688.cue_asw(u, -1, -55428.7, -12205.3, 8000, 0)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- tenuto_0_6
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) and su_unita(u, 7, m688.contatto_da, 0, 70) then
    m688.variabile(u, 203, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- tenuto_0_7
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) and su_unita(u, 8, m688.contatto_da, 0, 70) then
    m688.variabile(u, 204, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- tenuto_0_8
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) and su_unita(u, 9, m688.contatto_da, 0, 70) then
    m688.variabile(u, 205, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end
-- tenuto_0_9
script.R_regola_6 = function(u)
  if m688.variabile_uguale(u, 206, 0) and su_unita(u, 10, m688.contatto_da, 0, 70) then
    m688.variabile(u, 206, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_6")
end
-- tenuto_0_14
script.R_regola_7 = function(u)
  if m688.variabile_uguale(u, 207, 0) and su_unita(u, 15, m688.contatto_da, 0, 70) then
    m688.variabile(u, 207, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_7")
end
-- libero_0
script.R_regola_8 = function(u)
  if m688.variabile_uguale(u, 208, 0) and m688.variabile_uguale(u, 302, 0) then
    m688.variabile(u, 208, 1)
    m688.variabile_piu(u, 307)
    do return end
  end
  m688.dopo(u, 10, "R_regola_8")
end
-- tenuto_2_6
script.R_regola_9 = function(u)
  if m688.variabile_uguale(u, 209, 0) and su_unita(u, 7, m688.contatto_da, 3, 70) then
    m688.variabile(u, 209, 1)
    m688.variabile(u, 303, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_9")
end
-- tenuto_2_7
script.R_regola_10 = function(u)
  if m688.variabile_uguale(u, 210, 0) and su_unita(u, 8, m688.contatto_da, 3, 70) then
    m688.variabile(u, 210, 1)
    m688.variabile(u, 303, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_10")
end
-- tenuto_2_8
script.R_regola_11 = function(u)
  if m688.variabile_uguale(u, 211, 0) and su_unita(u, 9, m688.contatto_da, 3, 70) then
    m688.variabile(u, 211, 1)
    m688.variabile(u, 303, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_11")
end
-- tenuto_2_9
script.R_regola_12 = function(u)
  if m688.variabile_uguale(u, 212, 0) and su_unita(u, 10, m688.contatto_da, 3, 70) then
    m688.variabile(u, 212, 1)
    m688.variabile(u, 303, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_12")
end
-- tenuto_2_14
script.R_regola_13 = function(u)
  if m688.variabile_uguale(u, 213, 0) and su_unita(u, 15, m688.contatto_da, 3, 70) then
    m688.variabile(u, 213, 1)
    m688.variabile(u, 303, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_13")
end
-- libero_2
script.R_regola_14 = function(u)
  if m688.variabile_uguale(u, 214, 0) and m688.variabile_uguale(u, 303, 0) then
    m688.variabile(u, 214, 1)
    m688.variabile_piu(u, 307)
    do return end
  end
  m688.dopo(u, 10, "R_regola_14")
end
-- tenuto_3_6
script.R_regola_15 = function(u)
  if m688.variabile_uguale(u, 215, 0) and su_unita(u, 7, m688.contatto_da, 4, 70) then
    m688.variabile(u, 215, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_15")
end
-- tenuto_3_7
script.R_regola_16 = function(u)
  if m688.variabile_uguale(u, 216, 0) and su_unita(u, 8, m688.contatto_da, 4, 70) then
    m688.variabile(u, 216, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_16")
end
-- tenuto_3_8
script.R_regola_17 = function(u)
  if m688.variabile_uguale(u, 217, 0) and su_unita(u, 9, m688.contatto_da, 4, 70) then
    m688.variabile(u, 217, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_17")
end
-- tenuto_3_9
script.R_regola_18 = function(u)
  if m688.variabile_uguale(u, 218, 0) and su_unita(u, 10, m688.contatto_da, 4, 70) then
    m688.variabile(u, 218, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_18")
end
-- tenuto_3_14
script.R_regola_19 = function(u)
  if m688.variabile_uguale(u, 219, 0) and su_unita(u, 15, m688.contatto_da, 4, 70) then
    m688.variabile(u, 219, 1)
    m688.variabile(u, 304, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_19")
end
-- libero_3
script.R_regola_20 = function(u)
  if m688.variabile_uguale(u, 220, 0) and m688.variabile_uguale(u, 304, 0) then
    m688.variabile(u, 220, 1)
    m688.variabile_piu(u, 307)
    do return end
  end
  m688.dopo(u, 10, "R_regola_20")
end
-- tenuto_4_6
script.R_regola_21 = function(u)
  if m688.variabile_uguale(u, 221, 0) and su_unita(u, 7, m688.contatto_da, 5, 70) then
    m688.variabile(u, 221, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_21")
end
-- tenuto_4_7
script.R_regola_22 = function(u)
  if m688.variabile_uguale(u, 222, 0) and su_unita(u, 8, m688.contatto_da, 5, 70) then
    m688.variabile(u, 222, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_22")
end
-- tenuto_4_8
script.R_regola_23 = function(u)
  if m688.variabile_uguale(u, 223, 0) and su_unita(u, 9, m688.contatto_da, 5, 70) then
    m688.variabile(u, 223, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_23")
end
-- tenuto_4_9
script.R_regola_24 = function(u)
  if m688.variabile_uguale(u, 224, 0) and su_unita(u, 10, m688.contatto_da, 5, 70) then
    m688.variabile(u, 224, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_24")
end
-- tenuto_4_14
script.R_regola_25 = function(u)
  if m688.variabile_uguale(u, 225, 0) and su_unita(u, 15, m688.contatto_da, 5, 70) then
    m688.variabile(u, 225, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_25")
end
-- libero_4
script.R_regola_26 = function(u)
  if m688.variabile_uguale(u, 226, 0) and m688.variabile_uguale(u, 305, 0) then
    m688.variabile(u, 226, 1)
    m688.variabile_piu(u, 307)
    do return end
  end
  m688.dopo(u, 10, "R_regola_26")
end
-- tenuto_5_6
script.R_regola_27 = function(u)
  if m688.variabile_uguale(u, 227, 0) and su_unita(u, 7, m688.contatto_da, 6, 70) then
    m688.variabile(u, 227, 1)
    m688.variabile(u, 306, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_27")
end
-- tenuto_5_7
script.R_regola_28 = function(u)
  if m688.variabile_uguale(u, 228, 0) and su_unita(u, 8, m688.contatto_da, 6, 70) then
    m688.variabile(u, 228, 1)
    m688.variabile(u, 306, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_28")
end
-- tenuto_5_8
script.R_regola_29 = function(u)
  if m688.variabile_uguale(u, 229, 0) and su_unita(u, 9, m688.contatto_da, 6, 70) then
    m688.variabile(u, 229, 1)
    m688.variabile(u, 306, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_29")
end
-- tenuto_5_9
script.R_regola_30 = function(u)
  if m688.variabile_uguale(u, 230, 0) and su_unita(u, 10, m688.contatto_da, 6, 70) then
    m688.variabile(u, 230, 1)
    m688.variabile(u, 306, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_30")
end
-- tenuto_5_14
script.R_regola_31 = function(u)
  if m688.variabile_uguale(u, 231, 0) and su_unita(u, 15, m688.contatto_da, 6, 70) then
    m688.variabile(u, 231, 1)
    m688.variabile(u, 306, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_31")
end
-- libero_5
script.R_regola_32 = function(u)
  if m688.variabile_uguale(u, 232, 0) and m688.variabile_uguale(u, 306, 0) then
    m688.variabile(u, 232, 1)
    m688.variabile_piu(u, 307)
    do return end
  end
  m688.dopo(u, 10, "R_regola_32")
end
-- alfa_tenuto_6
script.R_regola_33 = function(u)
  if m688.variabile_uguale(u, 233, 0) and su_unita(u, 7, m688.contatto_da, 0, 30) then
    m688.variabile(u, 233, 1)
    m688.variabile(u, 309, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_33")
end
-- alfa_tenuto_7
script.R_regola_34 = function(u)
  if m688.variabile_uguale(u, 234, 0) and su_unita(u, 8, m688.contatto_da, 0, 30) then
    m688.variabile(u, 234, 1)
    m688.variabile(u, 309, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_34")
end
-- alfa_tenuto_8
script.R_regola_35 = function(u)
  if m688.variabile_uguale(u, 235, 0) and su_unita(u, 9, m688.contatto_da, 0, 30) then
    m688.variabile(u, 235, 1)
    m688.variabile(u, 309, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_35")
end
-- alfa_tenuto_9
script.R_regola_36 = function(u)
  if m688.variabile_uguale(u, 236, 0) and su_unita(u, 10, m688.contatto_da, 0, 30) then
    m688.variabile(u, 236, 1)
    m688.variabile(u, 309, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_36")
end
-- alfa_tenuto_14
script.R_regola_37 = function(u)
  if m688.variabile_uguale(u, 237, 0) and su_unita(u, 15, m688.contatto_da, 0, 30) then
    m688.variabile(u, 237, 1)
    m688.variabile(u, 309, 0)
    do return end
  end
  m688.dopo(u, 10, "R_regola_37")
end
-- lancio
script.R_regola_38 = function(u)
  if m688.variabile_uguale(u, 238, 0) and m688.variabile_maggiore(u, 1, 0) then
    m688.variabile(u, 238, 1)
    m688.messaggio(u, 142)
    do return end
  end
  m688.dopo(u, 10, "R_regola_38")
end
-- liberi
script.R_regola_39 = function(u)
  if m688.variabile_uguale(u, 239, 0) and m688.variabile_maggiore(u, 307, 2) then
    m688.variabile(u, 239, 1)
    m688.variabile(u, 308, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_39")
end
-- ritrovati
script.R_regola_40 = function(u)
  if m688.variabile_uguale(u, 240, 0) and m688.variabile_minore(u, 307, 3) then
    m688.variabile(u, 240, 1)
    m688.messaggio(u, 141)
    do return end
  end
  m688.dopo(u, 10, "R_regola_40")
end

return missione
