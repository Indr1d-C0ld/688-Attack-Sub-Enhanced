-- SCENE4A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE4A",
  missione = 4,
  titolo = { it = "Mumar Cadaver", en = "MUMAR CADAVER" },
  lato = "usa",
  teatro = "gibilterra",
  posa = { metodo = "georef", centro = { lat = 35.7655, lon = -5.8445 }, metri_per_unita = { x = 22.969, y = 27.780 }, moto_classico_m_s_per_nodo = 1.2196, affidabilita = "alta", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 457.2 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0745" },
    harpoon = { gestore = "L_06fc" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_07e4" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0847",
    telegrafo = "L_0859",
    quota = "L_085e",
    timone = "L_0865",
    pilota_automatico_off = "L_08c1",
    periscopio = "L_08c9",
    carica_tubo_1 = "L_086d",
    carica_tubo_2 = "L_0876",
    carica_tubo_3 = "L_087f",
    carica_tubo_4 = "L_0888",
    carica_tubo_5 = "L_0891",
    carica_tubo_6 = "L_089a",
    carica_tubo_7 = "L_08a3",
    schiera_cala = "L_08cd",
    schiera_recupera = "L_08dd",
    schiera_persa = "L_08e9",
    designa_bersaglio = "L_08f3",
    posizione = "L_08f8",
    antenna = "L_08d3",
    guida_e0 = "L_081a",
    guida_e1 = "L_081f",
    guida_e2 = "L_0829",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.203125, probabilita_originale = "13/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52339.1326, y = 15280.4532 }, originale = { x = 4192, y = 4288 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -64079.8855, y = 16241.9821 }, originale = { x = 3680, y = 4320 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69980.9964, y = 12751.6056 }, originale = { x = 3424, y = 4192 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69399.4906, y = -6697.0881 }, originale = { x = 3456, y = 3488 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1953125, probabilita_originale = "25/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -36259.2858, y = -2476.4134 }, originale = { x = 4896, y = 3648 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32551.4824, y = 5463.2007 }, originale = { x = 5056, y = 3936 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -32574.333, y = -723.2213 }, originale = { x = 5056, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -32551.4824, y = 5463.2007 }, originale = { x = 5056, y = 3936 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50262.3901, y = -6826.725 }, originale = { x = 4288, y = 3488 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1484375, probabilita_originale = "19/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -45804.8109, y = 1103.1362 }, originale = { x = 4480, y = 3776 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -37711.7609, y = 1948.606 }, originale = { x = 4832, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -64311.4333, y = -15573.5713 }, originale = { x = 3680, y = 3168 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1484375, probabilita_originale = "19/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -40616.3272, y = 9915.6778 }, originale = { x = 4704, y = 4096 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -37711.7609, y = 1948.606 }, originale = { x = 4832, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -72955.2851, y = 8356.5749 }, originale = { x = 3296, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69980.9964, y = 12751.6056 }, originale = { x = 3424, y = 4192 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69399.4906, y = -6697.0881 }, originale = { x = 3456, y = 3488 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -45804.8109, y = 1103.1362 }, originale = { x = 4480, y = 3776 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32574.333, y = -723.2213 }, originale = { x = 5056, y = 3712 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -37711.7609, y = 1948.606 }, originale = { x = 4832, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -73043.113, y = -2248.6217 }, originale = { x = 3296, y = 3648 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.08984375, probabilita_originale = "23/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -40616.3272, y = 9915.6778 }, originale = { x = 4704, y = 4096 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32574.333, y = -723.2213 }, originale = { x = 5056, y = 3712 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -73043.113, y = -2248.6217 }, originale = { x = 3296, y = 3648 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69980.9964, y = 12751.6056 }, originale = { x = 3424, y = 4192 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69399.4906, y = -6697.0881 }, originale = { x = 3456, y = 3488 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.05078125, probabilita_originale = "13/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52339.1326, y = 15280.4532 }, originale = { x = 4192, y = 4288 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -37711.7609, y = 1948.606 }, originale = { x = 4832, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -72955.2851, y = 8356.5749 }, originale = { x = 3296, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69980.9964, y = 12751.6056 }, originale = { x = 3424, y = 4192 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -69399.4906, y = -6697.0881 }, originale = { x = 3456, y = 3488 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -36259.2858, y = -2476.4134 }, originale = { x = 4896, y = 3648 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -37711.7609, y = 1948.606 }, originale = { x = 4832, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -64311.4333, y = -15573.5713 }, originale = { x = 3680, y = 3168 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.01953125, probabilita_originale = "5/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -36259.2858, y = -2476.4134 }, originale = { x = 4896, y = 3648 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -17121.1111, y = 2768.3667 }, originale = { x = 5728, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -43581.1686, y = 4627.0479 }, originale = { x = 4576, y = 3904 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -32574.333, y = -723.2213 }, originale = { x = 5056, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -32551.4824, y = 5463.2007 }, originale = { x = 5056, y = 3936 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -11979.1757, y = -775.2243 }, originale = { x = 5952, y = 3712 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -64311.4333, y = -15573.5713 }, originale = { x = 3680, y = 3168 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.01171875, probabilita_originale = "3/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -40616.3272, y = 9915.6778 }, originale = { x = 4704, y = 4096 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -82399.6191, y = 19929.9911 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32574.333, y = -723.2213 }, originale = { x = 5056, y = 3712 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 3, classe = "foxtrot", tipo_originale = 33, nome_originale = "FOXTROT CLASS DIESEL SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 3.6, accel_kn_s = 0.129, quota_m_s = 6.858, quota_max_m = 182.88, integrita = 110, rumore = 5, sonar = 23, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0440", reazione = nil, arrivo = "L_0322" } },
      { id = 4, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -50947.4022, y = 2015.1441 }, originale = { x = 4256, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 5, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -45087.4859, y = -2435.7264 }, originale = { x = 4512, y = 3648 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 6, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -29608.4182, y = 6336.6104 }, originale = { x = 5184, y = 3968 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 7, classe = "petroliera", tipo_originale = 16, nome_originale = "TANKER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -24476.0345, y = 1901.9347 }, originale = { x = 5408, y = 3808 },
        quota_m = 0, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 75, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 8, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -73043.113, y = -2248.6217 }, originale = { x = 3296, y = 3648 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0322" } },
      { id = 9, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -72999.2161, y = 3053.9556 }, originale = { x = 3296, y = 3840 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      { id = 10, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 18, tipo = 3,
        posizione = { x = -63403.0862, y = 8282.6316 }, originale = { x = 3712, y = 4032 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 182.88, integrita = 50, rumore = 20, sonar = 18, difesa = 5, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -45804.8109, y = 1103.1362 }, { x = -64898.7063, y = 4758.2397 }, { x = -69135.1022, y = 26886.4308 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0529", reazione = nil, arrivo = "L_0322" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0299" }, { dopo_s = 120, unita = 0, gestore = "L_02f7" }, { dopo_s = 2700, unita = 0, gestore = "L_02a7" }, { dopo_s = 50, unita = 1, gestore = "L_0373" }, { dopo_s = 60, unita = 2, gestore = "L_036c" }, { dopo_s = 160, unita = 2, gestore = "L_0407" }, { dopo_s = 40, unita = 3, gestore = "L_036c" }, { dopo_s = 120, unita = 3, gestore = "L_0407" }, { dopo_s = 50, unita = 9, gestore = "L_036c" }, { dopo_s = 70, unita = 10, gestore = "L_036c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, lei è al corrente del recente attentato terroristico contro un volo paneuropeo, che è costato la vita a centinaia di civili. Fazioni libiche ne hanno rivendicato la responsabilità, e un'indagine ha concluso che la Libia è pienamente responsabile dell'attività terroristica.", "Le è ordinato di intercettare e affondare quattro petroliere libiche che hanno preso il mare di recente e stanno per entrare nello Stretto di Gibilterra. Le petroliere potrebbero essere scortate da fregate della classe Koni avvistate nella zona. Non attacchi le fregate, a meno che non sia necessario per raggiungere l'obiettivo principale." },
    en = { "CAPTAIN, YOU ARE AWARE OF THE RECENT TERRORIST ATTACK ON A PAN-EUROPEAN FLIGHT WHICH CLAIMED HUNDREDS OF CIVILIAN LIVES. LIBYAN FACTIONS CLAIMED RESPONSIBILITY, AND AN INVESTIGATION CONCLUDED THAT LIBYA IS FULLY RESPONSIBLE FOR THE TERRORIST ACTIVITY.", "YOU ARE INSTRUCTED TO INTERCEPT AND SINK FOUR LIBYAN OIL TANKERS WHICH HAVE RECENTLY SET OUT TO SEA AND SHOULD BE ENTERING THE STRAIT OF GIBRALTAR AT ANY TIME. THE TANKERS MAY BE ESCORTED BY KONI CLASS FRIGATES THAT HAVE BEEN SIGHTED IN THE AREA. DO NOT ATTACK THE FRIGATES UNLESS IT IS NECESSARY TO COMPLETE THE MAIN OBJECTIVE." },
  },
  esiti = {
    [1] = { it = "CODICE: 4-1-ACP\nL'USS Los Angeles e il suo equipaggio sono tragicamente andati perduti durante un'audace missione contro i terroristi libici. La natura esatta della perdita è attualmente oggetto di indagine, ma si ritiene che il Los Angeles abbia urtato il fondo. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 4-1-ACP\nTHE USS LOS ANGELES AND HER CREW WERE TRAGICALLY LOST WHILE EXECUTING A DARING MISSION AGAINST LIBYAN TERRORISTS. THE EXACT NATURE OF THE LOSS IS CURRENTLY UNDER INVESTIGATION, BUT IT IS BELIEVED THAT THE LOS ANGELES STRUCK BOTTOM. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 4-2-ACP\nL'USS Los Angeles e il suo equipaggio sono tragicamente andati perduti durante un'audace missione contro i terroristi libici. La natura esatta della perdita è attualmente oggetto di indagine. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 4-2-ACP\nTHE USS LOS ANGELES AND HER CREW WERE TRAGICALLY LOST WHILE EXECUTING A DARING MISSION AGAINST LIBYAN TERRORISTS. THE EXACT NATURE OF THE LOSS IS CURRENTLY UNDER INVESTIGATION. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [5] = { it = "CODICE: 4-5-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio una spiegazione completa del suo comportamento. Vorrei sapere esattamente cosa credeva di combinare là fuori. Dove pensava di andare? Quando arriva in porto si porti dietro la sua roba, perché non sono affatto convinto che lei debba comandare uno dei miei sottomarini.", en = "CODE: 4-5-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL EXPLANATION FOR YOUR BEHAVIOR. I WOULD LIKE TO KNOW JUST EXACTLY WHAT YOU THINK YOU WERE TRYING TO PULL OFF OUT THERE. WHERE DO YOU THINK YOU WERE GOING? WHEN YOU GET INTO PORT, BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE I'M NOT CONVINCED THAT YOU SHOULD BE IN COMMAND OF ONE OF MY SUBMARINES." },
    [6] = { it = "CODICE: 4-6-ACP\nComandante, le foto satellitari che ho appena ricevuto confermano le petroliere in fiamme a ovest dello Stretto di Gibilterra. Mi congratulo con lei e con l'equipaggio del Los Angeles per l'ottima mira. Farò personalmente in modo che il Los Angeles riceva un encomio e una licenza. Al circolo ufficiali offro io da bere.", en = "CODE: 4-6-ACP\nCAPTAIN, THE SATELLITE PHOTOS I JUST RECEIVED CONFIRM THE BURNING TANKERS WEST OF THE STRAIT OF GIBRALTAR. LET ME CONGRATULATE YOU AND THE CREW OF THE LOS ANGELES ON SOME FINE SHOOTING. I'LL PERSONALLY SEE THAT THE LOS ANGELES RECEIVES A COMMENDATION AND SHORE LEAVE FOR THIS. DRINKS ARE ON ME AT THE OFFICERS CLUB." },
    [7] = { it = "CODICE: 4-7-ACP\nComandante, le immagini satellitari mostrano le petroliere libiche che avanzano senza nemmeno cambiare rotta. La Marina la ringrazia per averla resa lo zimbello delle Forze Armate! Ora sembra che toccherà all'Aeronautica occuparsi dei libici! Comandante, quando metterà piede a terra il suo fondoschiena incontrerà il mio stivale! Preveda una lunga vacanza.", en = "CODE: 4-7-ACP\nCAPTAIN, SATELLITE PICTURES SHOW LIBYAN TANKERS STEAMING AHEAD WITHOUT SO MUCH AS A CHANGE OF COURSE. THE NAVY THANKS YOU FOR MAKING IT THE LAUGHING STOCK OF THE SERVICES! NOW IT LOOKS LIKE THE AIR FORCE MIGHT HAVE A CRACK AT THE LIBYANS! CAPTAIN, YOUR BUTTS GONNA BE AT THE RECEIVING END OF MY BOOT WHEN YOU STEP ASHORE! PLAN ON A LONG VACATION." },
    [8] = { it = "CODICE: 4-8-ACP\nLe immagini satellitari rivelano che lei ha affondato una sola petroliera. Comandante, dobbiamo ispirare fiducia nella Marina: una sola petroliera in fiamme non ci frutta molto rispetto. Voglio il suo rapporto sulla mia scrivania non appena attracca, insieme alla sua domanda di trasferimento a terra! Ho in mente l'incarico perfetto per lei: comandante del carcere militare della Marina di Anchorage, Alaska. Mi risulta che abbia ben 3 detenuti.", en = "CODE: 4-8-ACP\nSATELLITE PICTURES REVEAL THAT YOU SANK ONLY ONE TANKER. CAPTAIN, WE NEED TO INSPIRE CONFIDENCE IN THE NAVY - ONE BURNING TANKER DOESN'T EARN US A LOT OF RESPECT. I WANT YOUR REPORT ON MY DESK THE MOMENT YOU DOCK ALONG WITH YOUR TRANSFER REQUEST FOR SHORE DUTY! I HAVE THE PERFECT POSSITION IN MIND FOR YOU - COMMANDER IN CHARGE OF THE NAVAL CORRECTIONAL FACILITY, ANCHORAGE, ALASKA. I UNDERSTAND THEY HAVE A TOTAL OF 3 INMATES." },
    [9] = { it = "CODICE: 4-9-ACP\nIl Los Angeles ha svolto la sua missione in modo adeguato. Da lei mi sarei aspettato risultati migliori, Comandante, ma sono certo che i libici hanno capito il messaggio. Il Los Angeles è autorizzato a rientrare. Prepari un rapporto e si presenti da me appena arrivato.", en = "CODE: 4-9-ACP\nTHE LOS ANGELES PERFORMED HER MISSION ADEQUATELY. I WOULD HAVE HOPED FOR BETTER RESULTS FROM YOU CAPTAIN, BUT I'M SURE THE LIBYAN'S GOT THE MESSAGE. THE LOS ANGELES IS CLEARED TO HEAD FOR HOME. PREPARE A REPORT AND SEE ME FIRST THING UPON YOUR ARRIVAL." },
    [19] = { it = "CODICE: 4-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Idiota! Come ha potuto essere così stupido da affondare un'innocente nave mercantile?! Ha brillantemente portato questa missione alla peggiore conclusione possibile. In questo momento ho deputati, funzionari dell'ONU e giornalisti che chiedono sangue: e sangue avranno. Se dipendesse da me, non vivrebbe abbastanza per pentirsene. Ceda immediatamente il comando al comandante in seconda per il rientro in porto!", en = "CODE: 4-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. YOU IDIOT! HOW COULD YOU BE SO STUPID AS TO SINK AN INNOCENT MERCHANT SHIP?! YOU BRILLIANTLY OVERSAW THE ABSOLUTE WORST POSSIBLE CONCLUSION TO THIS MISSION. AT THIS MOMENT I HAVE CONGRESSMEN, U.N. OFFICIALS, AND REPORTERS ALL OUT FOR BLOOD: AND BLOOD THEY'LL GET. IF I HAVE MY WAY YOU WON'T LIVE TO REGRET THIS. TURN YOUR COMMAND OVER TO THE XO IMMEDIATELY FOR RETURN TO PORT!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_0299 = function(u)
  ::L_0299::
  if m688.manomesso(u) then goto L_029d end
  do return end
  ::L_029d::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_02a7 = function(u)
  ::L_02a7::
  m688.variabile(u, 7, 0)
  if m688.unita_esiste(u, 4) then goto L_02b4 end
  m688.variabile_piu(u, 7)
  ::L_02b4::
  if m688.unita_esiste(u, 5) then goto L_02bc end
  m688.variabile_piu(u, 7)
  ::L_02bc::
  if m688.unita_esiste(u, 6) then goto L_02c4 end
  m688.variabile_piu(u, 7)
  ::L_02c4::
  if m688.unita_esiste(u, 7) then goto L_02cc end
  m688.variabile_piu(u, 7)
  ::L_02cc::
  if m688.variabile_uguale(u, 7, 0) then goto L_02eb end
  if m688.variabile_uguale(u, 7, 1) then goto L_02ee end
  if m688.variabile_uguale(u, 7, 2) then goto L_02f1 end
  if m688.variabile_uguale(u, 7, 3) then goto L_02f4 end
  m688.messaggio(u, 106)
  do return end
  ::L_02eb::
  m688.messaggio(u, 107)
  do return end
  ::L_02ee::
  m688.messaggio(u, 108)
  do return end
  ::L_02f1::
  m688.messaggio(u, 109)
  do return end
  ::L_02f4::
  m688.messaggio(u, 109)
  do return end
end

script.L_02f7 = function(u)
  ::L_02f7::
  if m688.in_rettangolo(u, -133224.7073, 55913.1335, 72565.9805, -86210.0444) then goto L_0305 end
  m688.messaggio(u, 105)
  do return end
  ::L_0305::
  if m688.unita_esiste(u, 4) then goto L_031c end
  if m688.unita_esiste(u, 5) then goto L_031c end
  if m688.unita_esiste(u, 6) then goto L_031c end
  if m688.unita_esiste(u, 7) then goto L_031c end
  m688.messaggio(u, 106)
  do return end
  ::L_031c::
  m688.dopo(u, 20, "L_02f7")
  do return end
end

script.L_0322 = function(u)
  ::L_0322::
  m688.vai_a(u, -64898.7063, 4758.2397, 20, "L_032d")
  m688.salva_modello(u)
  do return end
end

script.L_032d = function(u)
  ::L_032d::
  m688.vai_a(u, -69135.1022, 26886.4308, 20, "L_0628")
  if m688.caso(u, 0.2031) then goto L_036a end
  m688.vai_a(u, -88923.2136, 27948.4887, 20, "L_0628")
  if m688.caso(u, 0.4023) then goto L_036a end
  m688.vai_a(u, -98741.6018, 2420.8264, 20, "L_0628")
  if m688.caso(u, 0.6055) then goto L_036a end
  m688.vai_a(u, -90161.1693, -21534.6527, 20, "L_0628")
  if m688.caso(u, 0.793) then goto L_036a end
  m688.vai_a(u, -69538.1212, -24371.9473, 20, "L_0628")
  ::L_036a::
  m688.salva_modello(u)
  do return end
end

script.L_036c = function(u)
  ::L_036c::
  m688.marca_contatti(u)
  m688.dopo(u, 30, "L_036c")
  do return end
end

script.L_0373 = function(u)
  ::L_0373::
  m688.nuovo_caso(u)
  m688.posizione(u, -45735.89, 14359.8284)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_0400 end
  m688.posizione(u, -42854.6701, 2855.8942)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_0400 end
  m688.posizione(u, -51724.1353, -5050.7048)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_0400 end
  m688.posizione(u, -39225.5643, -7766.4141)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_0400 end
  m688.posizione(u, -34059.0476, -4252.7388)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_0400 end
  m688.posizione(u, -31084.5544, 4574.1148)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_0400 end
  m688.posizione(u, -32528.6112, 11649.6802)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_0400 end
  m688.posizione(u, -27395.3158, 8980.8238)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_0400 end
  m688.posizione(u, -24476.0345, 1901.9347)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_0400 end
  m688.posizione(u, -25223.9858, -2514.8533)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_0400 end
  m688.posizione(u, -20811.9371, -3410.1344)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_0400 end
  m688.posizione(u, -18600.9231, -1647.5236)
  m688.rotta_ora(u, 300)
  ::L_0400::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0373")
  do return end
end

script.L_0407 = function(u)
  ::L_0407::
  m688.telegrafo(u, 0.0078)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_0411")
  do return end
end

script.L_0411 = function(u)
  ::L_0411::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.quota(u, 182.88, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 90, "L_0421")
  do return end
end

script.L_0421 = function(u)
  ::L_0421::
  m688.virata_relativa(u, -179.96)
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 90, "L_0430")
  do return end
end

script.L_0430 = function(u)
  ::L_0430::
  m688.riprendi_navigazione(u)
  m688.quota(u, 106.68, 0.5859)
  m688.telegrafo(u, 0.625)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_0407")
  do return end
end

script.L_0440 = function(u)
  ::L_0440::
  if m688.bersaglio_id(u, 0) then goto L_044d end
  if m688.bersaglio_arma(u) then goto L_04bb end
  ::L_0448::
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  do return end
  ::L_044d::
  if m688.traccia_solida(u) then goto L_0475 end
  script.L_0462(u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.3906)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_0597")
  do return end
  ::L_0475::
  if m688.bersaglio_vicino(u, 9) then goto L_0485 end
  ::L_047a::
  script.L_0462(u)
  m688.accosta_verso_bersaglio(u)
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  do return end
  ::L_0485::
  if m688.variabile_uguale(u, 2, 4) then goto L_04a5 end
  if m688.variabile_uguale(u, 0, 0) then goto L_049b end
  m688.variabile(u, 0, 0)
  goto L_04a2
  ::L_049b::
  if m688.variabile_uguale(u, 1, 0) then goto L_04a5 end
  ::L_04a2::
  goto L_05b1
  ::L_04a5::
  if m688.bersaglio_vicino(u, 1.8) then goto L_04ad end
  goto L_047a
  ::L_04ad::
  script.L_0462(u)
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 0.0195)
  m688.accosta_verso_bersaglio(u)
  m688.uguaglia_quota(u)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  do return end
  ::L_04bb::
  if m688.bersaglio_mi_punta(u) then goto L_04c1 end
  goto L_0448
  ::L_04c1::
  if m688.traccia_solida(u) then goto L_04da end
  script.L_0462(u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.impulso_sonar(u)
  m688.telegrafo(u, 0.0195)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0597")
  m688.dopo(u, 120, "L_0597")
  do return end
  ::L_04da::
  if m688.bersaglio_vicino(u, 3) then goto L_04f8 end
  ::L_04df::
  script.L_0462(u)
  m688.accosta_opposto(u)
  m688.quota(u, 182.88, 1)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0597")
  m688.dopo(u, 120, "L_0597")
  do return end
  ::L_04f8::
  if m688.bersaglio_vicino(u, 1.2) then goto L_051c end
  ::L_04fd::
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 3, 1) then goto L_04df end
  script.L_04df(u)
  m688.variabile(u, 3, 1)
  script.L_07e4(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 60, "L_0611")
  do return end
  ::L_051c::
  m688.rotta(u, nil)
  m688.rateo_virata(u, -1.44)
  m688.telegrafo(u, 1.0547)
  goto L_04fd
  ::L_05b1::
  if m688.variabile_uguale(u, 2, 4) then goto L_05c7 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_0709(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_05ad")
  ::L_05c7::
  do return end
end

script.L_0462 = function(u)
  ::L_0462::
  if m688.flag_scena_uguale(u, 1) then goto L_0472 end
  m688.annulla_timer(u, "L_0407")
  m688.annulla_timer(u, "L_0411")
  m688.annulla_timer(u, "L_0421")
  m688.annulla_timer(u, "L_0430")
  ::L_0472::
  m688.flag_scena(u, 1)
  do return end
end

script.L_04df = function(u)
  ::L_04df::
  script.L_0462(u)
  m688.accosta_opposto(u)
  m688.quota(u, 182.88, 1)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0440")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0597")
  m688.dopo(u, 120, "L_0597")
  do return end
end

script.L_0529 = function(u)
  ::L_0529::
  if m688.bersaglio_id(u, 0) then goto L_0536 end
  if m688.bersaglio_arma(u) then goto L_05c8 end
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  do return end
  ::L_0536::
  if m688.traccia_solida(u) then goto L_0544 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.impulso_sonar(u)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  do return end
  ::L_0544::
  m688.ferma_navigazione(u)
  m688.uguaglia_velocita(u)
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, 9) then goto L_055c end
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0597")
  m688.dopo(u, 120, "L_0597")
  do return end
  ::L_055c::
  if m688.bersaglio_vicino(u, 3) then goto L_059f end
  if m688.variabile_uguale(u, 2, 4) then goto L_0586 end
  if m688.variabile_uguale(u, 0, 0) then goto L_0577 end
  m688.variabile(u, 0, 0)
  goto L_057e
  ::L_0577::
  if m688.variabile_uguale(u, 1, 0) then goto L_0586 end
  ::L_057e::
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  goto L_05b1
  ::L_0586::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0597")
  do return end
  ::L_059f::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 10)
  m688.telegrafo(u, 0.0195)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  do return end
  ::L_05b1::
  if m688.variabile_uguale(u, 2, 4) then goto L_05c7 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_0709(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_05ad")
  ::L_05c7::
  do return end
  ::L_05c8::
  if m688.bersaglio_mi_punta(u) then goto L_05d0 end
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  do return end
  ::L_05d0::
  if m688.traccia_solida(u) then goto L_05db end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.impulso_sonar(u)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  do return end
  ::L_05db::
  if m688.bersaglio_vicino(u, 3) then goto L_05ef end
  ::L_05e0::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0597")
  do return end
  ::L_05ef::
  if m688.bersaglio_vicino(u, 0.6) then goto L_05e0 end
  script.L_05e0(u)
  if m688.variabile_uguale(u, 3, 1) then goto L_05e0 end
  m688.variabile(u, 3, 1)
  m688.modello_classe(u, "tipo_2a")
  script.L_07e4(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 60, "L_0611")
  do return end
end

script.L_0597 = function(u)
  ::L_0597::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.5)
  m688.flag_scena(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_05ad = function(u)
  ::L_05ad::
  m688.variabile_meno(u, 2)
  do return end
end

script.L_05e0 = function(u)
  ::L_05e0::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0529")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0597")
  do return end
end

script.L_0611 = function(u)
  ::L_0611::
  m688.variabile(u, 3, 0)
  do return end
end

script.L_0628 = function(u)
  ::L_0628::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_06fc = function(u)
  ::L_06fc::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0706")
  do return end
end

script.L_0706 = function(u)
  ::L_0706::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0709 = function(u)
  ::L_0709::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type40")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_07e0", "L_07bd")
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 1.0286)
  m688.modello_avanti_tutta(u, 45.7143)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 15)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 40)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 60, "L_0792")
  m688.dopo(u, 240, "L_07e0")
  do return end
end

script.L_0745 = function(u)
  ::L_0745::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_07e0", "L_07bd")
  script.L_076c(u)
  m688.dopo(u, 170, "L_0769")
  m688.dopo(u, 190, "L_0792")
  m688.dopo(u, 280, "L_07e0")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0769 = function(u)
  ::L_0769::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_076c = function(u)
  ::L_076c::
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
  m688.flag_scena(u, 1)
  m688.nuova_unita(u)
  do return end
end

script.L_0792 = function(u)
  ::L_0792::
  m688.annulla_timer(u, "L_0792")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_07bd = function(u)
  ::L_07bd::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_079d")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_07d9")
  m688.dopo(u, 10, "L_07d1")
  do return end
end

script.L_079d = function(u)
  ::L_079d::
  if m688.flag_scena_uguale(u, 1) then goto L_07ab end
  if m688.bersaglio_id(u, 0) then goto L_07ab end
  m688.comportamento(u, "L_079d")
  m688.salva_modello(u)
  do return end
  ::L_07ab::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_07d1")
  m688.annulla_timer(u, "L_07bd")
  m688.insegui(u, 0, "L_07e0", "L_07bd")
  m688.salva_modello(u)
  do return end
end

script.L_07d1 = function(u)
  ::L_07d1::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_07d9 = function(u)
  ::L_07d9::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_07d9")
  do return end
end

script.L_07e0 = function(u)
  ::L_07e0::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_07e4 = function(u)
  ::L_07e4::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_07e0", "L_07bd")
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
  m688.dopo(u, 2, "L_0811")
  m688.dopo(u, 60, "L_0816")
  do return end
end

script.L_0811 = function(u)
  ::L_0811::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0816 = function(u)
  ::L_0816::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_081a = function(u)
  ::L_081a::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_081f = function(u)
  ::L_081f::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_07bd")
  do return end
end

script.L_0829 = function(u, p)
  ::L_0829::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 20, "L_0838")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0838 = function(u)
  ::L_0838::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_079d")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0846 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0846::
  do return end
end

script.L_0847 = function(u, p)
  ::L_0847::
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 30, "L_0852")
  m688.salva_modello(u)
  do return end
end

script.L_0852 = function(u)
  ::L_0852::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0859 = function(u, p)
  ::L_0859::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_085e = function(u, p)
  ::L_085e::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0865 = function(u, p)
  ::L_0865::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_086d = function(u)
  ::L_086d::
  m688.annulla_timer(u, "L_08ac")
  m688.dopo(u, 20, "L_08ac")
  do return end
end

script.L_0876 = function(u)
  ::L_0876::
  m688.annulla_timer(u, "L_08af")
  m688.dopo(u, 20, "L_08af")
  do return end
end

script.L_087f = function(u)
  ::L_087f::
  m688.annulla_timer(u, "L_08b2")
  m688.dopo(u, 20, "L_08b2")
  do return end
end

script.L_0888 = function(u)
  ::L_0888::
  m688.annulla_timer(u, "L_08b5")
  m688.dopo(u, 20, "L_08b5")
  do return end
end

script.L_0891 = function(u)
  ::L_0891::
  m688.annulla_timer(u, "L_08b8")
  m688.dopo(u, 20, "L_08b8")
  do return end
end

script.L_089a = function(u)
  ::L_089a::
  m688.annulla_timer(u, "L_08bb")
  m688.dopo(u, 20, "L_08bb")
  do return end
end

script.L_08a3 = function(u)
  ::L_08a3::
  m688.annulla_timer(u, "L_08be")
  m688.dopo(u, 20, "L_08be")
  do return end
end

script.L_08ac = function(u)
  ::L_08ac::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_08af = function(u)
  ::L_08af::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_08b2 = function(u)
  ::L_08b2::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_08b5 = function(u)
  ::L_08b5::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_08b8 = function(u)
  ::L_08b8::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_08bb = function(u)
  ::L_08bb::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_08be = function(u)
  ::L_08be::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_08c1 = function(u)
  ::L_08c1::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_08c9 = function(u, p)
  ::L_08c9::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08cd = function(u)
  ::L_08cd::
  m688.dopo(u, 60, "L_08d7")
  do return end
end

script.L_08d3 = function(u, p)
  ::L_08d3::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08d7 = function(u)
  ::L_08d7::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_08dd = function(u)
  ::L_08dd::
  m688.dopo(u, 40, "L_08e3")
  do return end
end

script.L_08e3 = function(u)
  ::L_08e3::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_08e9 = function(u)
  ::L_08e9::
  m688.annulla_timer(u, "L_08d7")
  m688.annulla_timer(u, "L_08e3")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_08f3 = function(u, p)
  ::L_08f3::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08f8 = function(u, p)
  ::L_08f8::
  m688.posizione(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448))
  m688.salva_modello(u)
  do return end
end

return missione
