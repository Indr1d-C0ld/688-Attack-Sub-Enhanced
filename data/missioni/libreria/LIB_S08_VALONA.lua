-- LIB_S08_VALONA: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_S08_VALONA",
  titolo = { it = "Valona", en = "Valona" },
  lato = "urss",
  teatro = "otranto",
  posa = { metodo = "rapida", centro = { lat = 41, lon = 18.75 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-05-26", ora_utc = 5,
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
    { unita = 12, missione = "caccia", situazione = "crisi", bersagli = "giocatore", equipaggio = "scarso",
      base = { x = -339241.8, y = -392407.6 }, zona = { x = 36466.7, y = -58766.4, raggio_m = 20000 } },
  },
  obiettivi = {
    { id = "fuori", tipo = "variabile", minimo = 5, indice = 300, almeno = 5,
      testo = { it = "La nave appoggio e i quattro Foxtrot escono dalla baia", en = "The tender and the four Foxtrots leave the bay" } },
    { id = "colonna", tipo = "proteggere", unita = { 2, 3, 4, 5, 6 },
      testo = { it = "Proteggere la colonna", en = "Protect the column" } },
    { id = "non_sparare", tipo = "proteggere", unita = { 7, 8, 9, 10, 11, 12 }, colpire = false,
      testo = { it = "Non sparare a nessuno", en = "Do not fire on anyone" } },
    { id = "nato", tipo = "scoprire", unita = { 11 }, primario = false,
      testo = { it = "Classificare il sottomarino della NATO", en = "Classify the NATO submarine" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: Comando della 5ª Eskadra", "A: K-463 (PROGETTO 705)", "OGG.: VALONA", "26 maggio 1989, ore 0500Z. Teatro: Canale d'Otranto, baia di Valona.", "Il paese ospite ha rotto con Mosca, e la base di Pasha Liman e' finita. Le truppe del porto non lasciano partire tutti: quattro sottomarini restano ai moli, trattenuti.", "Portate fuori dalla baia la nave appoggio e gli altri quattro Foxtrot, in colonna, fino al Canale d'Otranto.", "Fuori un sottomarino e un Orion della NATO guardano. Nessuno vuole sparare, e tutti hanno le armi pronte.", "Se un comandante esita, raggiungetelo: la vostra presenza bastera'.", "Regole d'ingaggio: crisi. Non sparate a nessuno.", "Tempo a disposizione: 1 ora e 30 minuti.", "Nota storica: ispirata alla ritirata sovietica da Valona (1961) e ai quattro sottomarini trattenuti, trasposta al 1989; nomi e unita' sono di fantasia." },
    en = { "MISSION ORDERS", "FM: 5TH ESKADRA HQ", "TO: K-463 (PROJECT 705)", "SUBJ: VALONA", "26 May 1989, 0500Z. Theatre: Strait of Otranto, Bay of Valona.", "The host country has broken with Moscow, and the Pasha Liman base is finished. The port troops will not let everyone go: four submarines stay at the piers, detained.", "Take the tender and the other four Foxtrots out of the bay, in column, as far as the Strait of Otranto.", "Outside, a NATO submarine and an Orion are watching. Nobody wants to shoot, and everyone has weapons ready.", "If a captain hesitates, go to him: your presence will be enough.", "Rules of engagement: crisis. Do not fire on anyone.", "Time available: 1 hour 30 minutes.", "Historical note: inspired by the Soviet withdrawal from Valona (1961) and the four detained submarines, moved to 1989; names and units are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [40] = { it = "La nave appoggio e i quattro battelli sono fuori dalla baia: la base di Pasha Liman e' finita.", en = "The tender and the four boats are out of the bay: the Pasha Liman base is finished." },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_S08_VALONA", titolo = { it = "Valona", en = "Valona" }, autore = "Libreria di Melchior 688",
    teatro = "otranto", alfa = true, centro = { lat = 41, lon = 18.75 },
    data = { giorno = 26, mese = 5, anno = 1989 }, ora_utc = 5, meteo_realistico = false,
    situazione = "crisi", paranoia = 1, difficolta = 2, durata_s = 5400, libreria = 2,
    carico = { dotazione = { { "type53", 18 }, { "esca", 4 }, }, deposito = { { "type53", 18 }, { "esca", 4 }, { "esca_mobile", 2 }, }, tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", parte = "urss", ruolo = "", flag = 17, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = 55148.475934239352, y = -61982.699292269055 }, quota = 15, rotta = 300, velocita = 6, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 2, classe = "cargo", categoria = "nave", parte = "urss", ruolo = "nave appoggio", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 57286.564311027629, y = -64187.491207578736 }, quota = 0, rotta = 307.71549569887941, velocita = 12, punti = { { x = 53019.529440339538, y = -60887.708110152955 }, { x = 46646.861117554501, y = -59264.71755770078 }, { x = 36466.742633972128, y = -58766.401876609896 }, { x = 29686.690779168555, y = -59907.022805158915 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "foxtrot", categoria = "sottomarino", parte = "urss", ruolo = "in colonna", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57318.139499945246, y = -64786.659804423412 }, quota = 0, rotta = 307.71549569887941, velocita = 12, punti = { { x = 53019.529440339538, y = -60887.708110152955 }, { x = 46646.861117554501, y = -59264.71755770078 }, { x = 36466.742633972128, y = -58766.401876609896 }, { x = 29686.690779168555, y = -59907.022805158915 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 4, classe = "foxtrot", categoria = "sottomarino", parte = "urss", ruolo = "in colonna", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57349.714688862863, y = -65385.828401268089 }, quota = 0, rotta = 307.71549569887941, velocita = 12, punti = { { x = 53019.529440339538, y = -60887.708110152955 }, { x = 46646.861117554501, y = -59264.71755770078 }, { x = 36466.742633972128, y = -58766.401876609896 }, { x = 29686.690779168555, y = -59907.022805158915 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 5, classe = "foxtrot", categoria = "sottomarino", parte = "urss", ruolo = "in colonna", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57381.28987778048, y = -65984.996998112765 }, quota = 0, rotta = 307.71549569887941, velocita = 12, punti = { { x = 53019.529440339538, y = -60887.708110152955 }, { x = 46646.861117554501, y = -59264.71755770078 }, { x = 36466.742633972128, y = -58766.401876609896 }, { x = 29686.690779168555, y = -59907.022805158915 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 6, classe = "foxtrot", categoria = "sottomarino", parte = "urss", ruolo = "in colonna", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57412.865066698098, y = -66584.165594957434 }, quota = 0, rotta = 307.71549569887941, velocita = 12, punti = { { x = 53019.529440339538, y = -60887.708110152955 }, { x = 46646.861117554501, y = -59264.71755770078 }, { x = 36466.742633972128, y = -58766.401876609896 }, { x = 29686.690779168555, y = -59907.022805158915 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 7, classe = "foxtrot", categoria = "sottomarino", parte = "neutrale", ruolo = "trattenuto", flag = 1, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 58043.587454739478, y = -73472.618508601649 }, quota = 0, rotta = 0, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 8, classe = "foxtrot", categoria = "sottomarino", parte = "neutrale", ruolo = "trattenuto", flag = 1, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57933.779833604145, y = -73882.426129736981 }, quota = 0, rotta = 0, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 9, classe = "foxtrot", categoria = "sottomarino", parte = "neutrale", ruolo = "trattenuto", flag = 1, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57523.972212468812, y = -73772.618508601649 }, quota = 0, rotta = 0, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 10, classe = "foxtrot", categoria = "sottomarino", parte = "neutrale", ruolo = "trattenuto", flag = 1, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57633.779833604145, y = -73362.810887466316 }, quota = 0, rotta = 0, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 11, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "sottomarino della NATO", flag = 9, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 29713.188260881256, y = -66569.568238633699 }, quota = 100, rotta = 0, velocita = 4, punti = { { x = 27965.312860591646, y = -53250.98524788169 }, { x = 31429.736628865656, y = -71004.253555202187 }, },
        ciclica = true, esce = false, comportamento = "R_sottomarino", profonda = 250, entra_con = "",
        parametri = { } },
      { id = 12, classe = "p3c", categoria = "aereo", parte = "usa", ruolo = "pattugliatore", flag = 11, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 16466.742633972128, y = -58766.401876609903 }, quota = 0, rotta = 90, velocita = 200, punti = { { x = 47073.344351770342, y = -48159.800158811682 }, { x = 47073.344351770342, y = -69373.003594408103 }, { x = 25860.140916173914, y = -69373.003594408117 }, { x = 25860.140916173914, y = -48159.800158811689 }, },
        ciclica = true, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "",
        parametri = { } },
    },
    obiettivi = {
      { id = "fuori", tipo = "variabile", unita = {  }, minimo = 5, entro_s = -1, primario = true, colpire = true, testo = { it = "La nave appoggio e i quattro Foxtrot escono dalla baia", en = "The tender and the four Foxtrots leave the bay" }, indice = 0 },
      { id = "colonna", tipo = "proteggere", unita = { 2, 3, 4, 5, 6 }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Proteggere la colonna", en = "Protect the column" } },
      { id = "non_sparare", tipo = "proteggere", unita = { 7, 8, 9, 10, 11, 12 }, minimo = -1, entro_s = -1, primario = true, colpire = false, testo = { it = "Non sparare a nessuno", en = "Do not fire on anyone" } },
      { id = "nato", tipo = "scoprire", unita = { 11 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Classificare il sottomarino della NATO", en = "Classify the NATO submarine" } },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 12, base_unita = -1, base = { x = -339241.81794576789, y = -392407.59104154981 }, situazione = "crisi", contro_giocatore = true, bersagli = {  }, zona = { x = 36466.742633972128, y = -58766.401876609896 }, raggio = 20000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: Comando della 5ª Eskadra", "A: K-463 (PROGETTO 705)", "OGG.: VALONA", "26 maggio 1989, ore 0500Z. Teatro: Canale d'Otranto, baia di Valona.", "Il paese ospite ha rotto con Mosca, e la base di Pasha Liman e' finita. Le truppe del porto non lasciano partire tutti: quattro sottomarini restano ai moli, trattenuti.", "Portate fuori dalla baia la nave appoggio e gli altri quattro Foxtrot, in colonna, fino al Canale d'Otranto.", "Fuori un sottomarino e un Orion della NATO guardano. Nessuno vuole sparare, e tutti hanno le armi pronte.", "Se un comandante esita, raggiungetelo: la vostra presenza bastera'.", "Regole d'ingaggio: crisi. Non sparate a nessuno.", "Tempo a disposizione: 1 ora e 30 minuti.", "Nota storica: ispirata alla ritirata sovietica da Valona (1961) e ai quattro sottomarini trattenuti, trasposta al 1989; nomi e unita' sono di fantasia.", }, en = { "MISSION ORDERS", "FM: 5TH ESKADRA HQ", "TO: K-463 (PROJECT 705)", "SUBJ: VALONA", "26 May 1989, 0500Z. Theatre: Strait of Otranto, Bay of Valona.", "The host country has broken with Moscow, and the Pasha Liman base is finished. The port troops will not let everyone go: four submarines stay at the piers, detained.", "Take the tender and the other four Foxtrots out of the bay, in column, as far as the Strait of Otranto.", "Outside, a NATO submarine and an Orion are watching. Nobody wants to shoot, and everyone has weapons ready.", "If a captain hesitates, go to him: your presence will be enough.", "Rules of engagement: crisis. Do not fire on anyone.", "Time available: 1 hour 30 minutes.", "Historical note: inspired by the Soviet withdrawal from Valona (1961) and the four detained submarines, moved to 1989; names and units are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "fuori", nome = { it = "fuori dalla baia", en = "out of the bay" }, cerchio = { x = 36466.742633972128, y = -58766.401876609896, r = 3000 } },
      { id = "ripensamento", nome = { it = "il Foxtrot che torna indietro", en = "the Foxtrot turning back" }, cerchio = { x = 53895.509640985889, y = -63229.21589106584, r = 1500 } },
    },
    regole = {
      { id = "fuori_1", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 2, indice = 0, zona = "fuori", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fuori_2", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 3, indice = 0, zona = "fuori", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fuori_3", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 4, indice = 0, zona = "fuori", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fuori_4", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 5, indice = 0, zona = "fuori", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "fuori_5", una_volta = true,
        quando = {
          { tipo = "unita_in_zona", valore = 0, unita = 6, indice = 0, zona = "fuori", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 0, valore = 1, comportamento = "", operazione = "+", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "ripensamento", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 1200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 53895.509640985889, y = -63229.21589106584 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 5 },
        } },
      { id = "convinto", una_volta = true,
        quando = {
          { tipo = "regola_scattata", valore = 0, unita = -1, indice = 0, zona = "", regola = "ripensamento", confronto = "=" },
          { tipo = "giocatore_in_zona", valore = 0, unita = -1, indice = 0, zona = "ripensamento", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 36466.742633972128, y = -58766.401876609896 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 12 },
        } },
      { id = "tutti_fuori", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 4, unita = -1, indice = 0, zona = "", regola = "", confronto = ">" },
        },
        allora = {
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "La nave appoggio e i quattro battelli sono fuori dalla baia: la base di Pasha Liman e' finita.", en = "The tender and the four boats are out of the bay: the Pasha Liman base is finished." } },
        } },
    },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "alfa", categoria = "sottomarino", schieramento = "urss", flag = 17, giocatore = true,
        posizione = { x = 55148.5, y = -61982.7 }, quota_m = 15, rotta = 300, velocita_kn = 6,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = 57286.6, y = -64187.5 }, quota_m = 0, rotta = 307.7, velocita_kn = 12,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 53019.5, y = -60887.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_2_p2" } },   -- nave appoggio
      { id = 3, classe = "foxtrot", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 57318.1, y = -64786.7 }, quota_m = 0, rotta = 307.7, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 53019.5, y = -60887.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_3_p2" } },   -- in colonna
      { id = 4, classe = "foxtrot", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 57349.7, y = -65385.8 }, quota_m = 0, rotta = 307.7, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 53019.5, y = -60887.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_4_p2" } },   -- in colonna
      { id = 5, classe = "foxtrot", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 57381.3, y = -65985 }, quota_m = 0, rotta = 307.7, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 53019.5, y = -60887.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_5_p2" } },   -- in colonna
      { id = 6, classe = "foxtrot", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 57412.9, y = -66584.2 }, quota_m = 0, rotta = 307.7, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 53019.5, y = -60887.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_6_p2" } },   -- in colonna
      { id = 7, classe = "foxtrot", categoria = "sottomarino", schieramento = "neutrale", flag = 1,
        posizione = { x = 58043.6, y = -73472.6 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- trattenuto
      { id = 8, classe = "foxtrot", categoria = "sottomarino", schieramento = "neutrale", flag = 1,
        posizione = { x = 57933.8, y = -73882.4 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- trattenuto
      { id = 9, classe = "foxtrot", categoria = "sottomarino", schieramento = "neutrale", flag = 1,
        posizione = { x = 57524, y = -73772.6 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- trattenuto
      { id = 10, classe = "foxtrot", categoria = "sottomarino", schieramento = "neutrale", flag = 1,
        posizione = { x = 57633.8, y = -73362.8 }, quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 20, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- trattenuto
      { id = 11, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9,
        posizione = { x = 29713.2, y = -66569.6 }, quota_m = 100, rotta = 0, velocita_kn = 4,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 21, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 27965.3, y = -53251 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_sottomarino", reazione = nil, arrivo = "R_11_p2" } },   -- sottomarino della NATO
      { id = 12, classe = "p3c", categoria = "aereo", schieramento = "usa", flag = 11,
        posizione = { x = 16466.7, y = -58766.4 }, quota_m = 0, rotta = 90, velocita_kn = 200,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 19, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 47073.3, y = -48159.8 } }, rotta_ciclica = true,
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = "R_12_p2" } },   -- pattugliatore
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 5400, unita = 0, gestore = "R_tempo" },
      { dopo_s = 10, unita = 2, gestore = "R_regola_0" },
      { dopo_s = 10, unita = 3, gestore = "R_regola_1" },
      { dopo_s = 10, unita = 4, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 5, gestore = "R_regola_3" },
      { dopo_s = 10, unita = 6, gestore = "R_regola_4" },
      { dopo_s = 1200, unita = 0, gestore = "R_regola_5" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_6" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_7" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (1) e difficolta' (2)
local ARMI = 1            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 14          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [2] = { parte = "urss", crociera = 0.875, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [3] = { parte = "urss", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [4] = { parte = "urss", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [5] = { parte = "urss", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [7] = { parte = "neutrale", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [8] = { parte = "neutrale", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [9] = { parte = "neutrale", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [10] = { parte = "neutrale", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [11] = { parte = "usa", crociera = 0.118, comportamento = "R_sottomarino", rotta = true, profonda = 250,
      paranoia = 1, salve = 2, portata = 4, distanza = 2 },
  [12] = { parte = "usa", crociera = 0.663, comportamento = "R_aereo", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
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
  m688.vai_a(u, 53019.5, -60887.7, 30, "R_2_p2")
  m688.salva_modello(u)
end
script.R_2_p2 = function(u)
  m688.vai_a(u, 46646.9, -59264.7, 30, "R_2_p3")
  m688.salva_modello(u)
end
script.R_2_p3 = function(u)
  m688.vai_a(u, 36466.7, -58766.4, 30, "R_2_p4")
  m688.salva_modello(u)
end
script.R_2_p4 = function(u)
  m688.vai_a(u, 29686.7, -59907, 30, "")
  m688.salva_modello(u)
end
script.R_3_p1 = function(u)
  m688.vai_a(u, 53019.5, -60887.7, 30, "R_3_p2")
  m688.salva_modello(u)
end
script.R_3_p2 = function(u)
  m688.vai_a(u, 46646.9, -59264.7, 30, "R_3_p3")
  m688.salva_modello(u)
end
script.R_3_p3 = function(u)
  m688.vai_a(u, 36466.7, -58766.4, 30, "R_3_p4")
  m688.salva_modello(u)
end
script.R_3_p4 = function(u)
  m688.vai_a(u, 29686.7, -59907, 30, "")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, 53019.5, -60887.7, 30, "R_4_p2")
  m688.salva_modello(u)
end
script.R_4_p2 = function(u)
  m688.vai_a(u, 46646.9, -59264.7, 30, "R_4_p3")
  m688.salva_modello(u)
end
script.R_4_p3 = function(u)
  m688.vai_a(u, 36466.7, -58766.4, 30, "R_4_p4")
  m688.salva_modello(u)
end
script.R_4_p4 = function(u)
  m688.vai_a(u, 29686.7, -59907, 30, "")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, 53019.5, -60887.7, 30, "R_5_p2")
  m688.salva_modello(u)
end
script.R_5_p2 = function(u)
  m688.vai_a(u, 46646.9, -59264.7, 30, "R_5_p3")
  m688.salva_modello(u)
end
script.R_5_p3 = function(u)
  m688.vai_a(u, 36466.7, -58766.4, 30, "R_5_p4")
  m688.salva_modello(u)
end
script.R_5_p4 = function(u)
  m688.vai_a(u, 29686.7, -59907, 30, "")
  m688.salva_modello(u)
end
script.R_6_p1 = function(u)
  m688.vai_a(u, 53019.5, -60887.7, 30, "R_6_p2")
  m688.salva_modello(u)
end
script.R_6_p2 = function(u)
  m688.vai_a(u, 46646.9, -59264.7, 30, "R_6_p3")
  m688.salva_modello(u)
end
script.R_6_p3 = function(u)
  m688.vai_a(u, 36466.7, -58766.4, 30, "R_6_p4")
  m688.salva_modello(u)
end
script.R_6_p4 = function(u)
  m688.vai_a(u, 29686.7, -59907, 30, "")
  m688.salva_modello(u)
end
script.R_11_p1 = function(u)
  m688.vai_a(u, 27965.3, -53251, 30, "R_11_p2")
  m688.salva_modello(u)
end
script.R_11_p2 = function(u)
  m688.vai_a(u, 31429.7, -71004.3, 30, "R_11_p1")
  m688.salva_modello(u)
end
script.R_12_p1 = function(u)
  m688.vai_a(u, 47073.3, -48159.8, 30, "R_12_p2")
  m688.salva_modello(u)
end
script.R_12_p2 = function(u)
  m688.vai_a(u, 47073.3, -69373, 30, "R_12_p3")
  m688.salva_modello(u)
end
script.R_12_p3 = function(u)
  m688.vai_a(u, 25860.1, -69373, 30, "R_12_p4")
  m688.salva_modello(u)
end
script.R_12_p4 = function(u)
  m688.vai_a(u, 25860.1, -48159.8, 30, "R_12_p1")
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
  ["fuori"] = { 36466.7, -58766.4, 3000 },
  ["ripensamento"] = { 53895.5, -63229.2, 1500 },
}
-- fuori_1
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) and m688.in_cerchio(u, table.unpack(ZONE["fuori"])) then
    m688.variabile(u, 200, 1)
    m688.variabile_piu(u, 300)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- fuori_2
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) and m688.in_cerchio(u, table.unpack(ZONE["fuori"])) then
    m688.variabile(u, 201, 1)
    m688.variabile_piu(u, 300)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- fuori_3
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) and m688.in_cerchio(u, table.unpack(ZONE["fuori"])) then
    m688.variabile(u, 202, 1)
    m688.variabile_piu(u, 300)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- fuori_4
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) and m688.in_cerchio(u, table.unpack(ZONE["fuori"])) then
    m688.variabile(u, 203, 1)
    m688.variabile_piu(u, 300)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- fuori_5
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) and m688.in_cerchio(u, table.unpack(ZONE["fuori"])) then
    m688.variabile(u, 204, 1)
    m688.variabile_piu(u, 300)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- ripensamento
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) then
    m688.variabile(u, 205, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.vai_a(u, 53895.5, -63229.2, 30, "")
      m688.telegrafo(u, 0.273)
      m688.salva_modello(u)
      UNITA[5].rotta = true
      UNITA[5].crociera = 0.273
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end
-- convinto
script.R_regola_6 = function(u)
  if m688.variabile_uguale(u, 206, 0) and m688.variabile_uguale(u, 205, 1) and m688.in_cerchio(u, table.unpack(ZONE["ripensamento"])) then
    m688.variabile(u, 206, 1)
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.vai_a(u, 36466.7, -58766.4, 30, "")
      m688.telegrafo(u, 0.656)
      m688.salva_modello(u)
      UNITA[5].rotta = true
      UNITA[5].crociera = 0.656
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_6")
end
-- tutti_fuori
script.R_regola_7 = function(u)
  if m688.variabile_uguale(u, 207, 0) and m688.variabile_maggiore(u, 300, 4) then
    m688.variabile(u, 207, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_7")
end

return missione
