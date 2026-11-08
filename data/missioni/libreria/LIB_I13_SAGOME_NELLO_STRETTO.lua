-- LIB_I13_SAGOME_NELLO_STRETTO: missione dell'editor (engine/missioni/src/scenario_missione.cpp), autore Libreria di Melchior 688.
-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "LIB_I13_SAGOME_NELLO_STRETTO",
  titolo = { it = "Sagome nello Stretto", en = "Silhouettes in the Strait" },
  lato = "usa",
  teatro = "gibilterra",
  posa = { metodo = "rapida", centro = { lat = 35.9, lon = -5.5 }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = "posizioni in metri sul teatro, velocita' vere anche nel classico" },
  data = "1989-06-12", ora_utc = 15,
  dotazione = { mk48 = 22, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "mk48" },
  deposito = { mk48 = 18, esca = 6 },
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
  obiettivi = {
    { id = "identifica", tipo = "identificare", unita = { 2, 3, 4, 5, 6, 7, 8, 9, 10 }, minimo = 7,
      testo = { it = "Identificare a vista almeno 7 delle 9 navi da guerra", en = "Identify by sight at least 7 of the 9 warships" } },
    { id = "telemetro", tipo = "telemetro", minimo = 3, errore = 0.1,
      testo = { it = "Tre distanze col telemetro entro il 10%", en = "Three stadimeter ranges within 10%" } },
    { id = "rotta", tipo = "rotta_stimata", minimo = 3, primario = false, errore = 20,
      testo = { it = "Tre rotte dall'angolo sulla prora entro 20 gradi", en = "Three courses from the angle on the bow within 20 degrees" } },
    { id = "foto_agi", tipo = "foto", unita = { 9 }, primario = false, distanza_m = 3000,
      testo = { it = "Fotografare la nave spia entro 3 km", en = "Photograph the spy ship within 3 km" } },
    { id = "tiro", tipo = "colpire", unita = { 2, 3, 4, 5, 6, 7, 8 }, primario = false,
      testo = { it = "Provare la soluzione di tiro (siluri d'esercitazione)", en = "Prove the firing solution (exercise torpedoes)" } },
    { id = "occulto", tipo = "non_essere_scoperto", primario = false,
      testo = { it = "Non farsi scoprire", en = "Stay undetected" } },
    { id = "vivo", tipo = "sopravvivere",
      testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ESERCITAZIONE DI RICONOSCIMENTO VISIVO - GIBILTERRA", "12 giugno 1989, ore 1500Z. Teatro: Stretto di Gibilterra. Pace. Mare calmo, visibilita' 20 km.", "Prima del Mediterraneo orientale, il comandante deve saper riconoscere a vista quello che incontrera'. Nello Stretto passa di tutto: un gruppo americano che entra, la 5a Eskadra sovietica che lo aspetta, una fregata algerina, mercantili e pescherecci.", "Identificare col libretto ogni nave da guerra. Misurare col telemetro la distanza di tre unita' e stimarne l'angolo sulla prora. Fotografare la nave spia sovietica.", "Attenzione alle somiglianze: Kidd e Spruance hanno lo stesso scafo; la nave spia ha lo scafo di un peschereccio; la Koni algerina e' di costruzione sovietica.", "Siluri d'esercitazione, senza testa: si puo' provare la soluzione di tiro sulle navi da guerra identificate. Regole d'ingaggio: armi trattenute.", "Non farsi scoprire: la nave spia e l'incrociatore hanno l'ESM.", "Tempo a disposizione: 1 ora e 30.", "Nota: inedita; unita' e nomi di fantasia." },
    en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: VISUAL RECOGNITION EXERCISE - GIBRALTAR", "12 June 1989, 1500Z. Theatre: Strait of Gibraltar. Peace. Calm sea, visibility 20 km.", "Before the eastern Mediterranean, the captain must be able to recognise by sight whatever he meets. Everything passes through the Strait: an American group coming in, the Soviet 5th Eskadra waiting for it, an Algerian frigate, merchants and fishing boats.", "Identify every warship with the recognition book. Range three units with the stadimeter and estimate their angle on the bow. Photograph the Soviet spy ship.", "Beware of look-alikes: Kidd and Spruance share the same hull; the spy ship has a trawler's hull; the Algerian Koni is Soviet-built.", "Exercise torpedoes, no warheads: the firing solution may be tried on identified warships. Rules of engagement: weapons tight.", "Stay undetected: the spy ship and the cruiser carry ESM.", "Time available: 1 hour 30.", "Note: original; units and names are fictional." },
  },
  esiti = {
    [20] = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." },
    [21] = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
  },
  radio = {
    ordini = { da = "COMSUBLANT", a = "USS LOS ANGELES (SSN 688)", oggetto = "VISUAL RECOGNITION EXERCISE - STRAIT OF GIBRALTAR", classifica = "CONFIDENTIAL", data = { giorno = 12, mese = 6, anno = 1989, ora_utc = 15 } },
    stato_iniziale = { roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, neutrali = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, } },
  },
  meteo = { fisso = { vento_m_s = 4, vento_da_gradi = 270, stato_mare = 2, visibilita_m = 20000, nubi_ottavi = 1, } },
  esercitazione = true,
  editor = { formato_editor = 1, scenario = {
    id = "LIB_I13_SAGOME_NELLO_STRETTO", titolo = { it = "Sagome nello Stretto", en = "Silhouettes in the Strait" }, autore = "Libreria di Melchior 688",
    teatro = "gibilterra", alfa = false, centro = { lat = 35.899999999999999, lon = -5.5 },
    data = { giorno = 12, mese = 6, anno = 1989 }, ora_utc = 15, meteo_realistico = false,
    situazione = "pace", paranoia = 1, difficolta = 2, durata_s = 5400, libreria = 2,
    carico = { dotazione = { { "mk48", 22 }, { "esca", 6 }, }, deposito = { { "mk48", 18 }, { "esca", 6 }, }, tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "mk48", }, tipo_missione = "antisom", ospiti = 0 },
    unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", parte = "usa", ruolo = "", flag = 9, giocatore = true, non_combattente = false, nascosta = false,
        pos = { x = 40588.887391395401, y = 7860.5619723866403 }, quota = 5.5, rotta = 270, velocita = 3, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 2, classe = "kidd", categoria = "nave", parte = "usa", ruolo = "cacciatorpediniere lanciamissili (scafo Spruance)", flag = 10, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 24959.521060363269, y = 10234.966226591583 }, quota = 0, rotta = 94, velocita = 14, punti = { { x = 84813.364075952719, y = 6049.5778019440631 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 3, classe = "spruance", categoria = "nave", parte = "usa", ruolo = "cacciatorpediniere (scafo come la Kidd)", flag = 10, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 25168.790481595646, y = 13227.658377371055 }, quota = 0, rotta = 94, velocita = 14, punti = { { x = 85022.633497185088, y = 9042.2699527235363 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 4, classe = "perry", categoria = "nave", parte = "usa", ruolo = "fregata in testa", flag = 10, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 28056.84792175893, y = 11522.042880748942 }, quota = 0, rotta = 94, velocita = 14, punti = { { x = 87910.69093734838, y = 7336.6544561014234 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 5, classe = "sacramento", categoria = "nave", parte = "usa", ruolo = "rifornitrice veloce al centro", flag = 10, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 25064.155770979458, y = 11731.312301981319 }, quota = 0, rotta = 94, velocita = 14, punti = { { x = 84917.998786568904, y = 7545.9238773337993 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 6, classe = "kara", categoria = "nave", parte = "urss", ruolo = "incrociatore della 5a Eskadra", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 58520.391957046821, y = 6291.7586029287922 }, quota = 0, rotta = 260, velocita = 8, punti = { { x = 28976.159366680578, y = 1082.3132729208819 }, },
        ciclica = false, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 7, classe = "krivak2", categoria = "nave", parte = "urss", ruolo = "scorta della Kara", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 58954.512401214146, y = 3829.7392203982722 }, quota = 0, rotta = 260, velocita = 8, punti = { { x = 29410.279810847904, y = -1379.7061096096381 }, },
        ciclica = false, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 8, classe = "nanuchka", categoria = "nave", parte = "urss", ruolo = "corvetta lanciamissili", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 57999.447424046026, y = 9246.181861965415 }, quota = 0, rotta = 260, velocita = 8, punti = { { x = 28455.214833679784, y = 4036.7365319575047 }, },
        ciclica = false, esce = false, comportamento = "R_nave", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 9, classe = "agi", categoria = "nave", parte = "urss", ruolo = "nave spia che pedina il gruppo americano", flag = 18, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 20257.847291287864, y = 10353.125522896325 }, quota = 0, rotta = 94, velocita = 9, punti = { },
        ciclica = false, esce = false, comportamento = "R_pedina", profonda = 0, entra_con = "",
        parametri = { distanza_nm = 2, } },
      { id = 10, classe = "koni", categoria = "nave", parte = "neutrale", ruolo = "fregata algerina di costruzione sovietica", flag = 2, giocatore = false, non_combattente = false, nascosta = false,
        pos = { x = 45719.189541280437, y = -6234.8273394019852 }, quota = 0, rotta = 320, velocita = 16, punti = { { x = 20007.685153818853, y = 24406.950385357126 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "koni",
        parametri = { } },
      { id = 11, classe = "mercantile", categoria = "nave", parte = "neutrale", ruolo = "mercantile verso est", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 30528.547112645687, y = 3008.8042006335868 }, quota = 0, rotta = 80, velocita = 12, punti = { { x = 89617.012293378168, y = 13427.694860649412 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 12, classe = "cargo", categoria = "nave", parte = "neutrale", ruolo = "mercantile verso ovest", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 50786.035727475377, y = 13088.196792454057 }, quota = 0, rotta = 260, velocita = 12, punti = { { x = -8302.4294532571075, y = 2669.3061324382361 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 13, classe = "petroliera", categoria = "nave", parte = "neutrale", ruolo = "mercantile verso est", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 21391.661220875285, y = 694.21650500248643 }, quota = 0, rotta = 80, velocita = 12, punti = { { x = 80480.126401607762, y = 11113.107165018311 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 14, classe = "petroliera", categoria = "nave", parte = "neutrale", ruolo = "mercantile verso ovest", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 59922.92161924579, y = 15402.784488085155 }, quota = 0, rotta = 260, velocita = 12, punti = { { x = 834.45643851330533, y = 4983.8938280693346 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 15, classe = "mercantile", categoria = "nave", parte = "neutrale", ruolo = "mercantile verso est", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 7754.7753291048757, y = 2879.6288093713865 }, quota = 0, rotta = 80, velocita = 12, punti = { { x = 66843.240509837357, y = 13298.519469387211 }, },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 16, classe = "peschereccio", categoria = "nave", parte = "neutrale", ruolo = "peschereccio spagnolo", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 20936.76653144139, y = -1501.2823028979092 }, quota = 0, rotta = 45, velocita = 3, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 17, classe = "peschereccio", categoria = "nave", parte = "neutrale", ruolo = "peschereccio spagnolo", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 21935.219320787961, y = 33132.022208074733 }, quota = 0, rotta = 135, velocita = 3, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 18, classe = "peschereccio", categoria = "nave", parte = "neutrale", ruolo = "peschereccio marocchino", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 23026.053792393024, y = -17002.707804723232 }, quota = 0, rotta = 225, velocita = 3, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
      { id = 19, classe = "peschereccio", categoria = "nave", parte = "neutrale", ruolo = "peschereccio marocchino", flag = 66, giocatore = false, non_combattente = true, nascosta = false,
        pos = { x = 22951.395643118056, y = -6655.6697914981851 }, quota = 0, rotta = 315, velocita = 3, punti = { },
        ciclica = false, esce = false, comportamento = "", profonda = 0, entra_con = "",
        parametri = { } },
    },
    obiettivi = {
      { id = "identifica", tipo = "identificare", unita = { 2, 3, 4, 5, 6, 7, 8, 9, 10 }, minimo = 7, entro_s = -1, primario = true, colpire = true, testo = { it = "Identificare a vista almeno 7 delle 9 navi da guerra", en = "Identify by sight at least 7 of the 9 warships" } },
      { id = "telemetro", tipo = "telemetro", unita = {  }, minimo = 3, entro_s = -1, primario = true, colpire = true, testo = { it = "Tre distanze col telemetro entro il 10%", en = "Three stadimeter ranges within 10%" }, errore = 0.10000000000000001 },
      { id = "rotta", tipo = "rotta_stimata", unita = {  }, minimo = 3, entro_s = -1, primario = false, colpire = true, testo = { it = "Tre rotte dall'angolo sulla prora entro 20 gradi", en = "Three courses from the angle on the bow within 20 degrees" }, errore = 20 },
      { id = "foto_agi", tipo = "foto", unita = { 9 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Fotografare la nave spia entro 3 km", en = "Photograph the spy ship within 3 km" }, distanza_m = 3000 },
      { id = "tiro", tipo = "colpire", unita = { 2, 3, 4, 5, 6, 7, 8 }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Provare la soluzione di tiro (siluri d'esercitazione)", en = "Prove the firing solution (exercise torpedoes)" } },
      { id = "occulto", tipo = "non_essere_scoperto", unita = {  }, minimo = -1, entro_s = -1, primario = false, colpire = true, testo = { it = "Non farsi scoprire", en = "Stay undetected" } },
      { id = "vivo", tipo = "sopravvivere", unita = {  }, minimo = -1, entro_s = -1, primario = true, colpire = true, testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
    },
    aerei_asw = {
    },
    briefing = { it = { "ORDINI DI MISSIONE", "DA: COMSUBLANT", "A: USS LOS ANGELES (SSN-688)", "OGG.: ESERCITAZIONE DI RICONOSCIMENTO VISIVO - GIBILTERRA", "12 giugno 1989, ore 1500Z. Teatro: Stretto di Gibilterra. Pace. Mare calmo, visibilita' 20 km.", "Prima del Mediterraneo orientale, il comandante deve saper riconoscere a vista quello che incontrera'. Nello Stretto passa di tutto: un gruppo americano che entra, la 5a Eskadra sovietica che lo aspetta, una fregata algerina, mercantili e pescherecci.", "Identificare col libretto ogni nave da guerra. Misurare col telemetro la distanza di tre unita' e stimarne l'angolo sulla prora. Fotografare la nave spia sovietica.", "Attenzione alle somiglianze: Kidd e Spruance hanno lo stesso scafo; la nave spia ha lo scafo di un peschereccio; la Koni algerina e' di costruzione sovietica.", "Siluri d'esercitazione, senza testa: si puo' provare la soluzione di tiro sulle navi da guerra identificate. Regole d'ingaggio: armi trattenute.", "Non farsi scoprire: la nave spia e l'incrociatore hanno l'ESM.", "Tempo a disposizione: 1 ora e 30.", "Nota: inedita; unita' e nomi di fantasia.", }, en = { "MISSION ORDERS", "FM: COMSUBLANT", "TO: USS LOS ANGELES (SSN-688)", "SUBJ: VISUAL RECOGNITION EXERCISE - GIBRALTAR", "12 June 1989, 1500Z. Theatre: Strait of Gibraltar. Peace. Calm sea, visibility 20 km.", "Before the eastern Mediterranean, the captain must be able to recognise by sight whatever he meets. Everything passes through the Strait: an American group coming in, the Soviet 5th Eskadra waiting for it, an Algerian frigate, merchants and fishing boats.", "Identify every warship with the recognition book. Range three units with the stadimeter and estimate their angle on the bow. Photograph the Soviet spy ship.", "Beware of look-alikes: Kidd and Spruance share the same hull; the spy ship has a trawler's hull; the Algerian Koni is Soviet-built.", "Exercise torpedoes, no warheads: the firing solution may be tried on identified warships. Rules of engagement: weapons tight.", "Stay undetected: the spy ship and the cruiser carry ESM.", "Time available: 1 hour 30.", "Note: original; units and names are fictional.", } },
    esito_fine = { it = "Missione conclusa: rientrare alla base per il rapporto.", en = "Mission over: return to base for debriefing." }, esito_tempo = { it = "Tempo scaduto: la missione si chiude qui.", en = "Time is up: the mission ends here." },
    chiusura = { tipo = "solo_tempo", unita = {  }, zona = { x = 0, y = 0, r = 0 }, fuori = { x = 0, y = 0, r = 0 } },
    zone = {
    },
    regole = {
      { id = "scambio", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 2400, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "vai_a", codice = 0, unita = 2, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 85022.633497185088, y = 9042.2699527235363 }, raggio = 1852, testo = { it = "", en = "" } },
          { tipo = "vai_a", codice = 0, unita = 3, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 84813.364075952719, y = 6049.5778019440631 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
      { id = "koni", una_volta = true,
        quando = {
          { tipo = "tempo", valore = 3000, unita = -1, indice = 0, zona = "", regola = "", confronto = "=" },
        },
        allora = {
          { tipo = "entra", codice = 0, unita = 10, indice = 0, valore = 0, comportamento = "", operazione = "=", categoria = "", zona = "", punto = { x = 0, y = 0 }, raggio = 1852, testo = { it = "", en = "" } },
        } },
    },
    radio = { ordini = { da = "COMSUBLANT", a = "USS LOS ANGELES (SSN 688)", oggetto = "VISUAL RECOGNITION EXERCISE - STRAIT OF GIBRALTAR", classifica = "CONFIDENTIAL" }, roe = { aerei = "trattenuto", mercantili = "trattenuto", navi = "trattenuto", neutrali = "trattenuto", sottomarini = "trattenuto", }, messaggi = {
    } },
    esercitazione = true,
    meteo_fisso = { vento_m_s = 4, vento_da_gradi = 270, stato_mare = 2, visibilita_m = 20000, nubi_ottavi = 1, nebbia = "" },
  } },
  script = script,
  varianti = {
    { probabilita = 1, unita = {
      { id = 0, classe = "los_angeles", categoria = "sottomarino", schieramento = "usa", flag = 9, giocatore = true,
        posizione = { x = 40588.9, y = 7860.6 }, quota_m = 5.5, rotta = 270, velocita_kn = 3,
        modello = { massima_kn = 37, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "kidd", categoria = "nave", schieramento = "usa", flag = 10,
        posizione = { x = 24959.5, y = 10235 }, quota_m = 0, rotta = 94, velocita_kn = 14,
        modello = { massima_kn = 33, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 84813.4, y = 6049.6 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- cacciatorpediniere lanciamissili (scafo Spruance)
      { id = 3, classe = "spruance", categoria = "nave", schieramento = "usa", flag = 10,
        posizione = { x = 25168.8, y = 13227.7 }, quota_m = 0, rotta = 94, velocita_kn = 14,
        modello = { massima_kn = 25, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 85022.6, y = 9042.3 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- cacciatorpediniere (scafo come la Kidd)
      { id = 4, classe = "perry", categoria = "nave", schieramento = "usa", flag = 10,
        posizione = { x = 28056.8, y = 11522 }, quota_m = 0, rotta = 94, velocita_kn = 14,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 10, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 87910.7, y = 7336.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- fregata in testa
      { id = 5, classe = "sacramento", categoria = "nave", schieramento = "usa", flag = 10,
        posizione = { x = 25064.2, y = 11731.3 }, quota_m = 0, rotta = 94, velocita_kn = 14,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 84918, y = 7545.9 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- rifornitrice veloce al centro
      { id = 6, classe = "kara", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 58520.4, y = 6291.8 }, quota_m = 0, rotta = 260, velocita_kn = 8,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 17, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 28976.2, y = 1082.3 } }, rotta_ciclica = false,
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = nil } },   -- incrociatore della 5a Eskadra
      { id = 7, classe = "krivak2", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 58954.5, y = 3829.7 }, quota_m = 0, rotta = 260, velocita_kn = 8,
        modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 17, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 29410.3, y = -1379.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = nil } },   -- scorta della Kara
      { id = 8, classe = "nanuchka", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 57999.4, y = 9246.2 }, quota_m = 0, rotta = 260, velocita_kn = 8,
        modello = { massima_kn = 32, virata_gradi_s = 1.8, accel_kn_s = 0.07, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 18, sonar = 5, difesa = 40, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 28455.2, y = 4036.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = "R_nave", reazione = nil, arrivo = nil } },   -- corvetta lanciamissili
      { id = 9, classe = "agi", categoria = "nave", schieramento = "urss", flag = 18,
        posizione = { x = 20257.8, y = 10353.1 }, quota_m = 0, rotta = 94, velocita_kn = 9,
        modello = { massima_kn = 11.5, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 30, rumore = 12, sonar = 7, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "R_pedina", reazione = nil, arrivo = nil } },   -- nave spia che pedina il gruppo americano
      { id = 10, classe = "koni", categoria = "nave", schieramento = "neutrale", flag = 2,
        posizione = { x = 45719.2, y = 2193765.2 }, quota_m = 0, rotta = 320, velocita_kn = 0,
        modello = { massima_kn = 25, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 12, difesa = 40, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- fregata algerina di costruzione sovietica, di riserva: entra con la regola koni
      { id = 11, classe = "mercantile", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 30528.5, y = 3008.8 }, quota_m = 0, rotta = 80, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 89617, y = 13427.7 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile verso est
      { id = 12, classe = "cargo", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 50786, y = 13088.2 }, quota_m = 0, rotta = 260, velocita_kn = 12,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -8302.4, y = 2669.3 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile verso ovest
      { id = 13, classe = "petroliera", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 21391.7, y = 694.2 }, quota_m = 0, rotta = 80, velocita_kn = 12,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 80480.1, y = 11113.1 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile verso est
      { id = 14, classe = "petroliera", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 59922.9, y = 15402.8 }, quota_m = 0, rotta = 260, velocita_kn = 12,
        modello = { massima_kn = 15, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 834.5, y = 4983.9 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile verso ovest
      { id = 15, classe = "mercantile", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 7754.8, y = 2879.6 }, quota_m = 0, rotta = 80, velocita_kn = 12,
        modello = { massima_kn = 20, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66843.2, y = 13298.5 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- mercantile verso est
      { id = 16, classe = "peschereccio", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 20936.8, y = -1501.3 }, quota_m = 0, rotta = 45, velocita_kn = 3,
        modello = { massima_kn = 14, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 18, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- peschereccio spagnolo
      { id = 17, classe = "peschereccio", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 21935.2, y = 33132 }, quota_m = 0, rotta = 135, velocita_kn = 3,
        modello = { massima_kn = 14, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 18, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- peschereccio spagnolo
      { id = 18, classe = "peschereccio", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 23026.1, y = -17002.7 }, quota_m = 0, rotta = 225, velocita_kn = 3,
        modello = { massima_kn = 14, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 18, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- peschereccio marocchino
      { id = 19, classe = "peschereccio", categoria = "nave", schieramento = "neutrale", flag = 66,
        posizione = { x = 22951.4, y = -6655.7 }, quota_m = 0, rotta = 315, velocita_kn = 3,
        modello = { massima_kn = 14, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 40, rumore = 18, sonar = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },   -- peschereccio marocchino
    },
    timer = {
      { dopo_s = 20, unita = 0, gestore = "R_chiusura" },
      { dopo_s = 5400, unita = 0, gestore = "R_tempo" },
      { dopo_s = 72, unita = 6, gestore = "R_vigila" },
      { dopo_s = 79, unita = 7, gestore = "R_vigila" },
      { dopo_s = 86, unita = 8, gestore = "R_vigila" },
      { dopo_s = 2400, unita = 0, gestore = "R_regola_0" },
      { dopo_s = 3000, unita = 0, gestore = "R_regola_1" },
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


-- Costanti della missione: regole d'ingaggio, paranoia (1) e difficolta' (2)
local ARMI = 0            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra
local COLPO = 14          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18
local CACCIA = 0.8          -- telegrafo delle navi in caccia
local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento
-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del
-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)
local UNITA = {
  [2] = { parte = "usa", crociera = 0.464, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [3] = { parte = "usa", crociera = 0.613, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [4] = { parte = "usa", crociera = 1, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [5] = { parte = "usa", crociera = 1, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [6] = { parte = "urss", crociera = 0.292, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [7] = { parte = "urss", crociera = 0.292, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [8] = { parte = "urss", crociera = 0.273, comportamento = "R_nave", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [9] = { parte = "urss", crociera = 0.856, comportamento = "R_pedina", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [10] = { parte = "neutrale", crociera = 0.7, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [11] = { parte = "neutrale", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [12] = { parte = "neutrale", crociera = 0.875, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [13] = { parte = "neutrale", crociera = 0.875, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [14] = { parte = "neutrale", crociera = 0.875, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [15] = { parte = "neutrale", crociera = 0.656, comportamento = "", rotta = true, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [16] = { parte = "neutrale", crociera = 0.234, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [17] = { parte = "neutrale", crociera = 0.234, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [18] = { parte = "neutrale", crociera = 0.234, comportamento = "", rotta = false, profonda = 0,
      paranoia = 1, salve = 2, portata = 2, distanza = 2 },
  [19] = { parte = "neutrale", crociera = 0.234, comportamento = "", rotta = false, profonda = 0,
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
  m688.vai_a(u, 84813.4, 6049.6, 30, "")
  m688.salva_modello(u)
end
script.R_3_p1 = function(u)
  m688.vai_a(u, 85022.6, 9042.3, 30, "")
  m688.salva_modello(u)
end
script.R_4_p1 = function(u)
  m688.vai_a(u, 87910.7, 7336.7, 30, "")
  m688.salva_modello(u)
end
script.R_5_p1 = function(u)
  m688.vai_a(u, 84918, 7545.9, 30, "")
  m688.salva_modello(u)
end
script.R_6_p1 = function(u)
  m688.vai_a(u, 28976.2, 1082.3, 30, "")
  m688.salva_modello(u)
end
script.R_7_p1 = function(u)
  m688.vai_a(u, 29410.3, -1379.7, 30, "")
  m688.salva_modello(u)
end
script.R_8_p1 = function(u)
  m688.vai_a(u, 28455.2, 4036.7, 30, "")
  m688.salva_modello(u)
end
script.R_10_p1 = function(u)
  m688.vai_a(u, 20007.7, 24407, 30, "")
  m688.salva_modello(u)
end
script.R_11_p1 = function(u)
  m688.vai_a(u, 89617, 13427.7, 30, "")
  m688.salva_modello(u)
end
script.R_12_p1 = function(u)
  m688.vai_a(u, -8302.4, 2669.3, 30, "")
  m688.salva_modello(u)
end
script.R_13_p1 = function(u)
  m688.vai_a(u, 80480.1, 11113.1, 30, "")
  m688.salva_modello(u)
end
script.R_14_p1 = function(u)
  m688.vai_a(u, 834.5, 4983.9, 30, "")
  m688.salva_modello(u)
end
script.R_15_p1 = function(u)
  m688.vai_a(u, 66843.2, 13298.5, 30, "")
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
-- scambio
script.R_regola_0 = function(u)
  if m688.variabile_uguale(u, 200, 0) then
    m688.variabile(u, 200, 1)
    if m688.unita_esiste(u, 2) then
      m688.seleziona(u, 2)
      m688.vai_a(u, 85022.6, 9042.3, 30, "")
      m688.salva_modello(u)
      UNITA[2].rotta = true
    end
    if m688.unita_esiste(u, 3) then
      m688.seleziona(u, 3)
      m688.vai_a(u, 84813.4, 6049.6, 30, "")
      m688.salva_modello(u)
      UNITA[3].rotta = true
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_0")
end
-- koni
script.R_regola_1 = function(u)
  if m688.variabile_uguale(u, 201, 0) then
    m688.variabile(u, 201, 1)
    if m688.unita_esiste(u, 10) then
      m688.seleziona(u, 10)
      m688.posizione(u, 45719.2, -6234.8)
      m688.rotta_ora(u, 320)
      m688.velocita_ora(u, 16)
      script.R_10_p1(u)
    end
    m688.seleziona(u, u)
    do return end
  end
  m688.dopo(u, 10, "R_regola_1")
end

return missione
