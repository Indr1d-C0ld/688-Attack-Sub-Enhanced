-- LIB_S15_ANCORAGGIO: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_S15_ANCORAGGIO",
  titolo = { it = "L'ancoraggio", en = "The Anchorage" },
  lato = "usa",
  teatro = "otranto",
  posa = { metodo = "rapida", centro = { lat = 41, lon = 18.75 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-10-26", ora_utc = 23,
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
    { unita = 9, missione = "caccia", situazione = "pace", bersagli = "giocatore", equipaggio = "esperto",
      zona = { x = 20970.1, y = 22241.2, raggio_m = 15000 } },
  },
  categorie_esiti = { [41] = "fallimento" },
  obiettivi = {
    { id = "identifica", tipo = "identificare", unita = { 2, 3, 4, 5, 6, 7 }, minimo = 4,
      testo = { it = "Identificare a vista almeno 4 unita' dell'ancoraggio", en = "Identify by sight at least 4 units at the anchorage" } },
    { id = "rapporto", tipo = "variabile", minimo = 1, indice = 302, almeno = 1,
      testo = { it = "Riferire la partenza della squadra (rapporto di contatto)", en = "Report the squadron's departure (contact report)" } },
    { id = "occulto", tipo = "variabile", minimo = 1, indice = 305, almeno = 1,
      testo = { it = "Non farsi classificare ne' tenere dalla squadra", en = "Avoid being classified or held by the squadron" } },
    { id = "telemetro", tipo = "telemetro", minimo = 3, primario = false, errore = 0.15,
      testo = { it = "Tre distanze col telemetro entro il 15%", en = "Three stadimeter ranges within 15%" } },
    { id = "rotta", tipo = "rotta_stimata", minimo = 3, primario = false, errore = 20,
      testo = { it = "Le rotte di tre navi che salpano, dai fanali, entro 20 gradi", en = "The courses of three departing ships, from their lights, within 20 degrees" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CTF 69", "A: USS LOS ANGELES (SSN-688)", "OGG.: SORVEGLIANZA DELL'ANCORAGGIO SOVIETICO", "26 ottobre 1989, ore 2300Z. Teatro: Canale d'Otranto, sul bassofondo a 20 miglia dalla costa, in acque internazionali. Crisi. Notte serena, mare quasi calmo.", "Due flotte a distanza di tiro, e nessuno spara. Stanotte la squadra sovietica e' all'ancora: la Sesta Flotta vuole sapere chi c'e' e, soprattutto, se qualcuno salpa.", "All'ancoraggio, coi fanali di fonda: un incrociatore Kara, una Krivak II, la base galleggiante Ugra con un Charlie II affiancato, una petroliera, la nave spia. In giro, a luci spente, una seconda Krivak col sonar a profondita' variabile calato: lo strato non vi nasconde.", "Riconoscere a vista almeno quattro unita'. Se la squadra salpa, riferire con un rapporto di contatto e portarsi al largo. Col telemetro, sul fanale d'albero, tre distanze; dai fanali di moto, la rotta di chi salpa.", "Non farsi classificare ne' tenere: se la squadra vi trova, la crisi sale. Regole d'ingaggio: armi trattenute, fuoco solo se attaccati.", "Tempo a disposizione: 3 ore.", "Nota storica: ispirata alla crisi navale dell'ottobre 1973 fra la 5a Eskadra e la Sesta Flotta; l'ancoraggio sta al posto del «punto» di Citera. Nomi e unita' di fantasia." },
    en = { "MISSION ORDERS", "FM: CTF 69", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SOVIET ANCHORAGE SURVEILLANCE", "26 October 1989, 2300Z. Theatre: Strait of Otranto, on the bank 20 miles offshore, in international waters. Crisis. Clear night, near-calm sea.", "Two fleets within firing range, and nobody shoots. Tonight the Soviet squadron is at anchor: the Sixth Fleet wants to know who is there and, above all, whether anyone sails.", "At the anchorage, showing anchor lights: a Kara cruiser, a Krivak II, the Ugra submarine tender with a Charlie II alongside, a tanker, the spy ship. Patrolling darkened, a second Krivak with her variable-depth sonar down: the layer will not hide you.", "Identify at least four units by sight. If the squadron sails, send a contact report and head for the open sea. With the stadimeter, on the masthead light, three ranges; from the running lights, the course of those sailing.", "Avoid being classified or held: if the squadron finds you, the crisis escalates. Rules of engagement: weapons tight, fire only if fired upon.", "Time available: 3 hours.", "Historical note: inspired by the October 1973 naval standoff between the 5th Eskadra and the Sixth Fleet; the anchorage stands in for the Kithira «point». Names and units are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    [40] = { it = "Il rapporto e' alla Sesta Flotta: la squadra ha salpato, e il Los Angeles e' al largo.", en = "The report is with the Sixth Fleet: the squadron has sailed, and Los Angeles is clear." },
    [41] = { it = "Collisione nel buio, in mezzo alla squadra: la crisi diventa un'altra cosa.", en = "Collision in the dark, among the squadron: the crisis becomes something else." },
  },
  radio = {
    ordini = { da = "CTF 69", a = "USS LOS ANGELES (SSN 688)", oggetto = "SOVIET ANCHORAGE SURVEILLANCE", classifica = "SECRET", data = { giorno = 26, mese = 10, anno = 1989, ora_utc = 23 } },
    stato_iniziale = { roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", } },
  },
  meteo = { fisso = { vento_m_s = 3, vento_da_gradi = 320, stato_mare = 1.5, visibilita_m = 18000, nubi_ottavi = 1, } },
  messaggi = {
    [1] = { it = "Sonar: un colpo secco vicino all'ancoraggio, piccolo. Come una bomba a mano in acqua.", en = "Sonar: a sharp, small bang near the anchorage. Like a hand grenade in the water." },
    [2] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
    [3] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
    [4] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
    [5] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
    [6] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
    [7] = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." },
  },
  editor = { formato_editor = 1, scenario = {
    id = "LIB_S15_ANCORAGGIO", titolo = { it = "L'ancoraggio", en = "The Anchorage" }, autore = "Libreria di Melchior 688",
    teatro = "otranto", alfa = false, centro = { lat = 41, lon = 18.75 },
    data = { giorno = 26, mese = 10, anno = 1989 }, ora_utc = 23, meteo_realistico = false,
    situazione = "crisi", paranoia = 2, difficolta = 4, durata_s = 10800, libreria = 2,
    carico = { dotazione = { { "mk48", 22 }, { "harpoon", 2 }, { "esca", 6 }, }, deposito = { { "mk48", 21 }, { "harpoon", 2 }, { "esca", 6 }, }, tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "", flag = 9, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = 7863.6680735520295, y = 13064.00001587714 }, quota = 60, rotta = 54.999999999999986, velocita = 5, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 2, classe = "kara", categoria = "nave", parte = "urss", ruolo = "incrociatore, la nave di bandiera", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 20970.100782175894, y = 22241.222997493882 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "krivak2", categoria = "nave", parte = "urss", ruolo = "fregata all'ancora", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 21870.100782175894, y = 22241.222997493882 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 4, classe = "ugra", categoria = "nave", parte = "urss", ruolo = "base galleggiante dei sommergibili (Progetto 1886)", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 20628.080638850224, y = 21301.530376707975 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 5, classe = "petroliera", categoria = "nave", parte = "urss", ruolo = "petroliera di squadra", flag = 82, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 20017.472838013011, y = 22791.222997493882 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 6, classe = "agi", categoria = "nave", parte = "urss", ruolo = "nave spia", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 21620.100782175894, y = 23367.056022413653 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 7, classe = "charlie2", categoria = "sottomarino", parte = "urss", ruolo = "sottomarino lanciamissili affiancato all'appoggio", flag = 17, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 20928.080638850224, y = 21301.530376707975 }, quota = 0, rotta = 340, velocita = 0, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 8, classe = "krivak2", categoria = "nave", parte = "urss", ruolo = "fregata di guardia a luci spente", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 20970.100782175894, y = 19241.222997493882 }, quota = 0, rotta = 90, velocita = 6, punti = { { x = 18372.024570822578, y = 20741.222997493882 }, { x = 18372.024570822578, y = 23741.222997493882 }, { x = 20970.100782175894, y = 25241.222997493882 }, { x = 23568.17699352921, y = 23741.222997493882 }, { x = 23568.17699352921, y = 20741.222997493885 }, { x = 20970.100782175894, y = 19241.222997493882 }, },
        ciclica = true, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { }, luci = "spente" },
      { id = 9, classe = "il38", categoria = "aereo", parte = "urss", ruolo = "pattugliatore della squadra", flag = 19, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 60970.100782175898, y = 22241.222997493885 }, quota = 0, rotta = 270, velocita = 250, punti = { },
        ciclica = false, esce = false, comportamento = "R_aereo", profonda = 0, entra_con = "salpano",
        parametri = { } },
    },
    obiettivi = {
      { id = "identifica", tipo = "identificare", unita = { 2, 3, 4, 5, 6, 7 }, minimo = 4, entro_s = -1, primario = true, colpire = true, testo = { it = "Identificare a vista almeno 4 unita' dell'ancoraggio", en = "Identify by sight at least 4 units at the anchorage" } },
      { id = "rapporto", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riferire la partenza della squadra (rapporto di contatto)", en = "Report the squadron's departure (contact report)" }, indice = 2 },
      { id = "occulto", tipo = "variabile", unita = {  }, minimo = 1, entro_s = -1, primario = true, colpire = true, testo = { it = "Non farsi classificare ne' tenere dalla squadra", en = "Avoid being classified or held by the squadron" }, indice = 5 },
      { id = "telemetro", tipo = "telemetro", unita = {  }, minimo = 3, entro_s = -1, primario = false, colpire = true, testo = { it = "Tre distanze col telemetro entro il 15%", en = "Three stadimeter ranges within 15%" }, errore = 0.14999999999999999 },
      { id = "rotta", tipo = "rotta_stimata", unita = {  }, minimo = 3, entro_s = -1, primario = false, colpire = true, testo = { it = "Le rotte di tre navi che salpano, dai fanali, entro 20 gradi", en = "The courses of three departing ships, from their lights, within 20 degrees" }, errore = 20 },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
      { unita = 9, base_unita = -1, situazione = "pace", contro_giocatore = true, bersagli = {  }, zona = { x = 20970.100782175894, y = 22241.222997493882 }, raggio = 15000 },
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: CTF 69", "A: USS LOS ANGELES (SSN-688)", "OGG.: SORVEGLIANZA DELL'ANCORAGGIO SOVIETICO", "26 ottobre 1989, ore 2300Z. Teatro: Canale d'Otranto, sul bassofondo a 20 miglia dalla costa, in acque internazionali. Crisi. Notte serena, mare quasi calmo.", "Due flotte a distanza di tiro, e nessuno spara. Stanotte la squadra sovietica e' all'ancora: la Sesta Flotta vuole sapere chi c'e' e, soprattutto, se qualcuno salpa.", "All'ancoraggio, coi fanali di fonda: un incrociatore Kara, una Krivak II, la base galleggiante Ugra con un Charlie II affiancato, una petroliera, la nave spia. In giro, a luci spente, una seconda Krivak col sonar a profondita' variabile calato: lo strato non vi nasconde.", "Riconoscere a vista almeno quattro unita'. Se la squadra salpa, riferire con un rapporto di contatto e portarsi al largo. Col telemetro, sul fanale d'albero, tre distanze; dai fanali di moto, la rotta di chi salpa.", "Non farsi classificare ne' tenere: se la squadra vi trova, la crisi sale. Regole d'ingaggio: armi trattenute, fuoco solo se attaccati.", "Tempo a disposizione: 3 ore.", "Nota storica: ispirata alla crisi navale dell'ottobre 1973 fra la 5a Eskadra e la Sesta Flotta; l'ancoraggio sta al posto del «punto» di Citera. Nomi e unita' di fantasia.", }, en = { "MISSION ORDERS", "FM: CTF 69", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: SOVIET ANCHORAGE SURVEILLANCE", "26 October 1989, 2300Z. Theatre: Strait of Otranto, on the bank 20 miles offshore, in international waters. Crisis. Clear night, near-calm sea.", "Two fleets within firing range, and nobody shoots. Tonight the Soviet squadron is at anchor: the Sixth Fleet wants to know who is there and, above all, whether anyone sails.", "At the anchorage, showing anchor lights: a Kara cruiser, a Krivak II, the Ugra submarine tender with a Charlie II alongside, a tanker, the spy ship. Patrolling darkened, a second Krivak with her variable-depth sonar down: the layer will not hide you.", "Identify at least four units by sight. If the squadron sails, send a contact report and head for the open sea. With the stadimeter, on the masthead light, three ranges; from the running lights, the course of those sailing.", "Avoid being classified or held: if the squadron finds you, the crisis escalates. Rules of engagement: weapons tight, fire only if fired upon.", "Time available: 3 hours.", "Historical note: inspired by the October 1973 naval standoff between the 5th Eskadra and the Sixth Fleet; the anchorage stands in for the Kithira «point». Names and units are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
      { id = "largo", nome = { it = "il largo, a sud-ovest", en = "the open sea, to the south-west" }, cerchio = { x = 7863.6680735520295, y = 13064.00001587714, r = 5000 } },
    },
    regole = {
      { id = "bomba", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 3600, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 1, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "rumore", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: un colpo secco vicino all'ancoraggio, piccolo. Come una bomba a mano in acqua.", en = "Sonar: a sharp, small bang near the anchorage. Like a hand grenade in the water." } },
        } },
      { id = "salpano", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 3720, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 7289.2950491491483, y = -15346.481833942453 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 14 },
          { tipo = "vai_a", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 20916.027888853103, y = -24151.087122994439 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 14 },
          { tipo = "vai_a", codice = 0, unita = 5, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -7220.6778414013534, y = 11980.618697723801 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 10 },
          { tipo = "vai_a", codice = 0, unita = 6, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 894.92550988928997, y = -6429.0985526208387 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 11 },
          { tipo = "quota", codice = 0, unita = 7, indice = 0, valore = 60, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "vai_a", codice = 0, unita = 7, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = -7314.1704652860062, y = -6043.0482499680256 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 12 },
          { tipo = "entra", codice = 0, unita = 9, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "salpa_appoggio", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 4200, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 4, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 20230.705081945962, y = -16949.555626083369 }, raggio = 1852, testo = { it = "", en = "" }, velocita = 10 },
        } },
      { id = "copertura", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "classificati_1", una_volta = true,
        quando = {
          { tipo = "unita_classifica", valore = 1, unita = 2, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "tenuti_1", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 5, unita = 2, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "classificati_2", una_volta = true,
        quando = {
          { tipo = "unita_classifica", valore = 1, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "tenuti_2", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 5, unita = 3, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "classificati_7", una_volta = true,
        quando = {
          { tipo = "unita_classifica", valore = 1, unita = 8, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "tenuti_7", una_volta = true,
        quando = {
          { tipo = "unita_vede", valore = 5, unita = 8, indice = 0, zona = "", regola = "", confronto = "=" },
          { tipo = "variabile", valore = 1, unita = -1, indice = 5, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 5, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "allarme", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "comportamento", codice = 0, unita = 8, indice = 0, valore = 0, comportamento = "R_nave", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "messaggio", codice = 0, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Sonar: impulsi attivi della squadra, su di noi. Ci hanno trovato.", en = "Sonar: active pings from the squadron, on us. They have found us." } },
        } },
      { id = "rapporto", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 1, unita = -1, indice = 1, zona = "", regola = "", confronto = "=" },
          { tipo = "giocatore_ha_trasmesso", valore = 3600, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", trasmissione = "contatto" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 2, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "al_largo", una_volta = true,
        quando = {
          { tipo = "variabile", valore = 1, unita = -1, indice = 2, zona = "", regola = "", confronto = "=" },
          { tipo = "giocatore_in_zona", valore = 0, unita = -1, indice = 0, zona = "largo", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "variabile", codice = 0, unita = -1, indice = 3, valore = 1, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "fine", codice = 40, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Il rapporto e' alla Sesta Flotta: la squadra ha salpato, e il Los Angeles e' al largo.", en = "The report is with the Sixth Fleet: the squadron has sailed, and Los Angeles is clear." } },
        } },
      { id = "collisione", una_volta = true,
        quando = {
          { tipo = "evento", valore = 0, unita = -1, indice = 0, zona = "", regola = "", confronto = "=", evento = "collisione" },
        },
        allora = {
          { tipo = "fine", codice = 41, unita = -1, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "fallimento", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "Collisione nel buio, in mezzo alla squadra: la crisi diventa un'altra cosa.", en = "Collision in the dark, among the squadron: the crisis becomes something else." } },
        } },
    },
    radio = { ordini = { da = "CTF 69", a = "USS LOS ANGELES (SSN 688)", oggetto = "SOVIET ANCHORAGE SURVEILLANCE", classifica = "SECRET" }, roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, messaggi = {
    } },
    meteo_fisso = { vento_m_s = 3, vento_da_gradi = 320, stato_mare = 1.5, visibilita_m = 18000, nubi_ottavi = 1, nebbia = "" },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9, giocatore = true,
        posizione = { x = 7863.7, y = 13064 }, quota_m = 60, rotta = 55, velocita_kn = 5,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "kara", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 20970.1, y = 22241.2 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 19, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- incrociatore, la nave di bandiera
      { id = 3, classe = "krivak2", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 21870.1, y = 22241.2 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 19, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- fregata all'ancora
      { id = 4, classe = "ugra", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 20628.1, y = 21301.5 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 17, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 90, rumore = 15, sonar = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- base galleggiante dei sommergibili (Progetto 1886)
      { id = 5, classe = "petroliera", categoria = "nave", schieramento = "urss", flag = 82,
        posizione = { x = 20017.5, y = 22791.2 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- petroliera di squadra
      { id = 6, classe = "agi", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 21620.1, y = 23367.1 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 11.5, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 30, rumore = 12, sonar = 9, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- nave spia
      { id = 7, classe = "charlie2", categoria = "sottomarino", schieramento = "urss", flag = 17,
        posizione = { x = 20928.1, y = 21301.5 }, quota_m = 0, rotta = 340, velocita_kn = 0,
        modello = { massima_kn = 42, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 19, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- sottomarino lanciamissili affiancato all'appoggio
      { id = 8, classe = "krivak2", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 20970.1, y = 19241.2 }, quota_m = 0, rotta = 90, velocita_kn = 6, luci = "spente",
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 19, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 18372, y = 20741.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "R_8_p2" } },   -- fregata di guardia a luci spente
      { id = 9, classe = "il38", categoria = "aereo", schieramento = "urss", flag = 19,
        posizione = { x = 60970.1, y = 2202241.2 }, quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 21, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = "R_aereo", reazione = nil, arrivo = nil } },   -- pattugliatore della squadra, di riserva: entra con la regola salpano
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 10800, unita = 0, gestore = "R_tempo" },
      { dopo_s = 3600, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 3720, unita = 0, gestore = "R_regola_1" },
      { dopo_s = 4200, unita = 0, gestore = "R_regola_2" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_3" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_4" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_5" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_6" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_7" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_8" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_9" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_10" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_11" },
      { dopo_s = 10, unita = 0, gestore = "R_regola_12" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (2) e difficolta' (4)
local ARMI = 1            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 9          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [2] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [3] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [4] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [5] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [6] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [7] = { parte = "urss", crociera = 0, comportamento = "", rotta = false, profonda = 0,
      paranoia = 2, salve = 3, portata = 5, distanza = 2 },
  [8] = { parte = "urss", crociera = 0.219, comportamento = "", rotta = true, profonda = 0,
      paranoia = 2, salve = 3, portata = 3, distanza = 2 },
  [9] = { parte = "urss", crociera = 0.829, comportamento = "R_aereo", rotta = false, profonda = 0,
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
script.R_8_p1 = function(u)
  m688.vai_a(u, 18372, 20741.2, 30, "R_8_p2")
  m688.salva_modello(u)
end
script.R_8_p2 = function(u)
  m688.vai_a(u, 18372, 23741.2, 30, "R_8_p3")
  m688.salva_modello(u)
end
script.R_8_p3 = function(u)
  m688.vai_a(u, 20970.1, 25241.2, 30, "R_8_p4")
  m688.salva_modello(u)
end
script.R_8_p4 = function(u)
  m688.vai_a(u, 23568.2, 23741.2, 30, "R_8_p5")
  m688.salva_modello(u)
end
script.R_8_p5 = function(u)
  m688.vai_a(u, 23568.2, 20741.2, 30, "R_8_p6")
  m688.salva_modello(u)
end
script.R_8_p6 = function(u)
  m688.vai_a(u, 20970.1, 19241.2, 30, "R_8_p1")
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
  ["largo"] = { 7863.7, 13064, 5000 },
}
local function su_unita(u, id, domanda, ...)
  m688.seleziona(u, id)
  local r = domanda(u, ...)
  m688.seleziona(u, u)
  return r
end
-- bomba
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) then
    m688.variabile(u, 200, 1)
    m688.variabile(u, 301, 1)
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.rumore_transitorio(u)
    end
    m688.messaggio_al_giocatore(u, 1001)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- salpano
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) then
    m688.variabile(u, 201, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.vai_a(u, 7289.3, -15346.5, 30, "")
      m688.telegrafo(u, 0.51)
      m688.salva_modello(u)
      UNITA[2].rotta = true
      UNITA[2].crociera = 0.51
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.vai_a(u, 20916, -24151.1, 30, "")
      m688.telegrafo(u, 0.51)
      m688.salva_modello(u)
      UNITA[3].rotta = true
      UNITA[3].crociera = 0.51
    end
    if m688.unita_esiste(u, 5) then
      m688.seleziona(u, 5)
      m688.vai_a(u, -7220.7, 11980.6, 30, "")
      m688.telegrafo(u, 0.729)
      m688.salva_modello(u)
      UNITA[5].rotta = true
      UNITA[5].crociera = 0.729
    end
    if m688.unita_esiste(u, 6) then
      m688.seleziona(u, 6)
      m688.vai_a(u, 894.9, -6429.1, 30, "")
      m688.telegrafo(u, 1)
      m688.salva_modello(u)
      UNITA[6].rotta = true
      UNITA[6].crociera = 1
    end
    if m688.unita_esiste(u, 7) then
      m688.seleziona(u, 7)
      m688.quota(u, 60, 1)
      m688.salva_modello(u)
    end
    if m688.unita_esiste(u, 7) then
      m688.seleziona(u, 7)
      m688.vai_a(u, -7314.2, -6043, 30, "")
      m688.telegrafo(u, 0.312)
      m688.salva_modello(u)
      UNITA[7].rotta = true
      UNITA[7].crociera = 0.312
    end
    if m688.unita_esiste(u, 9) then
      m688.seleziona(u, 9)
      m688.posizione(u, 60970.1, 22241.2)
      m688.rotta_ora(u, 270)
      m688.velocita_ora(u, 250)
      m688.salva_modello(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end
-- salpa_appoggio
script.R_regola_2 = function(u)
  if m688.variabile_uguale(u, 202, 0) then
    m688.variabile(u, 202, 1)
    if m688.unita_esiste(u, 4) then
      m688.seleziona(u, 4)
      m688.vai_a(u, 20230.7, -16949.6, 30, "")
      m688.telegrafo(u, 0.643)
      m688.salva_modello(u)
      UNITA[4].rotta = true
      UNITA[4].crociera = 0.643
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_2")
end
-- copertura
script.R_regola_3 = function(u)
  if m688.variabile_uguale(u, 203, 0) then
    m688.variabile(u, 203, 1)
    m688.variabile(u, 305, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_3")
end
-- classificati_1
script.R_regola_4 = function(u)
  if m688.variabile_uguale(u, 204, 0) and su_unita(u, 2, m688.classificato, 0, 1) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 204, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1002)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_4")
end
-- tenuti_1
script.R_regola_5 = function(u)
  if m688.variabile_uguale(u, 205, 0) and su_unita(u, 2, m688.contatto_da, 0, 5) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 205, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1003)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_5")
end
-- classificati_2
script.R_regola_6 = function(u)
  if m688.variabile_uguale(u, 206, 0) and su_unita(u, 3, m688.classificato, 0, 1) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 206, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1004)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_6")
end
-- tenuti_2
script.R_regola_7 = function(u)
  if m688.variabile_uguale(u, 207, 0) and su_unita(u, 3, m688.contatto_da, 0, 5) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 207, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1005)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_7")
end
-- classificati_7
script.R_regola_8 = function(u)
  if m688.variabile_uguale(u, 208, 0) and su_unita(u, 8, m688.classificato, 0, 1) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 208, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1006)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_8")
end
-- tenuti_7
script.R_regola_9 = function(u)
  if m688.variabile_uguale(u, 209, 0) and su_unita(u, 8, m688.contatto_da, 0, 5) and m688.variabile_uguale(u, 305, 1) then
    m688.variabile(u, 209, 1)
    m688.variabile(u, 305, 0)
    m688.variabile(u, 11, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[2].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[3].comportamento = "R_nave"
    end
    if m688.unita_esiste(u, 8) then
      m688.seleziona(u, 8)
      m688.comportamento(u, "R_nave")
      m688.salva_modello(u)
      UNITA[8].comportamento = "R_nave"
    end
    m688.messaggio_al_giocatore(u, 1007)
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_9")
end
-- rapporto
script.R_regola_10 = function(u)
  if m688.variabile_uguale(u, 210, 0) and m688.variabile_uguale(u, 301, 1) and m688.trasmesso(u, "contatto", 3600) then
    m688.variabile(u, 210, 1)
    m688.variabile(u, 302, 1)
    do return end
  end
  m688.dopo(u, 10, "R_regola_10")
end
-- al_largo
script.R_regola_11 = function(u)
  if m688.variabile_uguale(u, 211, 0) and m688.variabile_uguale(u, 302, 1) and m688.in_cerchio(u, table.unpack(ZONE["largo"])) then
    m688.variabile(u, 211, 1)
    m688.variabile(u, 303, 1)
    m688.messaggio(u, 140)
    do return end
  end
  m688.dopo(u, 10, "R_regola_11")
end
-- collisione
script.R_regola_12 = function(u)
  if m688.variabile_uguale(u, 212, 0) and m688.evento(u, "collisione", 0) then
    m688.variabile(u, 212, 1)
    m688.messaggio(u, 141)
    do return end
  end
  m688.dopo(u, 10, "R_regola_12")
end

return missione
