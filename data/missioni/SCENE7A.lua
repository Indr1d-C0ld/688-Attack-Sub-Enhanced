-- SCENE7A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE7A",
  missione = 7,
  titolo = { it = "Passerella", en = "CAT WALK" },
  lato = "usa",
  teatro = "kunashir",
  posa = { metodo = "georef", centro = { lat = 43.8800, lon = 145.3190 }, metri_per_unita = { x = 20.256, y = 24.597 }, moto_classico_m_s_per_nodo = 1.0779, affidabilita = "media", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 105.156, 167.64, 195.072 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_05ab" },
    harpoon = { gestore = "L_055b" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_063c" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_069f",
    telegrafo = "L_06b1",
    quota = "L_06b6",
    timone = "L_06bd",
    pilota_automatico_off = "L_0719",
    periscopio = "L_0721",
    carica_tubo_1 = "L_06c5",
    carica_tubo_2 = "L_06ce",
    carica_tubo_3 = "L_06d7",
    carica_tubo_4 = "L_06e0",
    carica_tubo_5 = "L_06e9",
    carica_tubo_6 = "L_06f2",
    carica_tubo_7 = "L_06fb",
    schiera_cala = "L_0725",
    schiera_recupera = "L_0735",
    schiera_persa = "L_0741",
    designa_bersaglio = "L_074b",
    posizione = "L_0750",
    antenna = "L_072b",
    guida_e0 = "L_0672",
    guida_e1 = "L_0677",
    guida_e2 = "L_0681",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.33203125, probabilita_originale = "85/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -108700, y = -70700 }, originale = { x = 5472, y = 2464 }, spostamento_m = 13196.0064,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -209717.6327, y = 17716.0468 }, originale = { x = 0, y = 6368 },
        quota_m = 0, rotta = 0, velocita_kn = 54.8571,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 55, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0159", reazione = nil, arrivo = "L_01dc" } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01c0" }, { dopo_s = 120, unita = 0, gestore = "L_01ce" }, { dopo_s = 60, unita = 1, gestore = "L_043a" }, { dopo_s = 2, unita = 6, gestore = "L_043a" }, { dopo_s = 4, unita = 6, gestore = "L_0155" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.234375, probabilita_originale = "15/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -99300, y = -62900 }, originale = { x = 5600, y = 3040 }, spostamento_m = 4227.8377,
        quota_m = 23.5, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -209717.6327, y = 17716.0468 }, originale = { x = 0, y = 6368 },
        quota_m = 0, rotta = 0, velocita_kn = 54.8571,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 55, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0159", reazione = nil, arrivo = "L_01dc" } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01c0" }, { dopo_s = 120, unita = 0, gestore = "L_01ce" }, { dopo_s = 60, unita = 1, gestore = "L_043a" }, { dopo_s = 2, unita = 6, gestore = "L_043a" }, { dopo_s = 4, unita = 6, gestore = "L_0155" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21875, probabilita_originale = "7/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -112700, y = -74500 }, originale = { x = 4864, y = 2688 }, spostamento_m = 501.953,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -209717.6327, y = 17716.0468 }, originale = { x = 0, y = 6368 },
        quota_m = 0, rotta = 0, velocita_kn = 54.8571,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 55, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0159", reazione = nil, arrivo = "L_01dc" } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01c0" }, { dopo_s = 120, unita = 0, gestore = "L_01ce" }, { dopo_s = 60, unita = 1, gestore = "L_043a" }, { dopo_s = 2, unita = 6, gestore = "L_043a" }, { dopo_s = 4, unita = 6, gestore = "L_0155" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21484375, probabilita_originale = "55/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -106100, y = -68700 }, originale = { x = 5344, y = 2848 }, spostamento_m = 4370.5103,
        quota_m = 17.75, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04df" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -209717.6327, y = 17716.0468 }, originale = { x = 0, y = 6368 },
        quota_m = 0, rotta = 0, velocita_kn = 54.8571,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 55, difesa = nil, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0159", reazione = nil, arrivo = "L_01dc" } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0345", reazione = nil, arrivo = "L_028c" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01c0" }, { dopo_s = 120, unita = 0, gestore = "L_01ce" }, { dopo_s = 60, unita = 1, gestore = "L_043a" }, { dopo_s = 2, unita = 6, gestore = "L_043a" }, { dopo_s = 4, unita = 6, gestore = "L_0155" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCPACFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: CACCIA AL LANCIAMISSILI", "La ricognizione satellitare indica che un sottomarino sovietico della classe Typhoon ha appena lasciato il porto ed è diretto verso il suo settore di pattugliamento, appena a nord del Giappone. Qui siamo in guerra, quindi voglio che lei e i suoi ragazzi sventriate questo pesce per me. Un LAMPS volerà in avanscoperta per passarle le coordinate, se riuscirà a determinare la posizione del lanciamissili balistici. Il Typhoon sarà sicuramente scortato da uno o più sottomarini d'attacco, ma non si impegni con loro se può evitarlo. Se riesce a cogliere Ivan al passaggio, può mandare a picco quel lanciamissili prima che i suoi compagni si accorgano di qualcosa! Facciamolo per il Gipper..." },
    en = { "MISSION ORDERS", "FM: CINCPACFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: BOOMER BANGER", "SATELLITE RECON INDICATES THAT A SOVIET TYPHOON CLASS SUBMARINE HAS JUST LEFT PORT AND IS HEADING FOR YOUR PATROL SECTOR, JUST NORTH OF JAPAN. IT'S WAR OUT HERE, SO I WANT YOU AND YOUR BOYS TO GUT THIS FISH FOR ME. A LAMPS WILL FLY AHEAD TO PASS YOU COORDINATES IF THEY CAN GET A FIX ON THE BOOMER'S POSITION. THE TYPHOON IS SURE TO BE ESCORTED BY ONE OR MORE ATTACK BOATS, BUT DON'T TANGLE WITH THEM IF YOU CAN AVOID IT. IF YOU CAN CATCH IVAN AT THE PASS YOU CAN DEEP SIX THAT BOOMER BEFORE HIS PARTNERS KNOW ANYTHING HAPPENED! LET'S WIN THIS ONE FOR THE GIPPER..." },
  },
  esiti = {
    [1] = { it = "CODICE: 7-1-ACP\nL'USS Los Angeles è andato distrutto durante una manovra. Sembra che il Los Angeles si sia incagliato e che l'intero equipaggio sia andato perduto. Abbiamo avviato un'indagine completa sull'incidente. Le nostre condoglianze vanno ai familiari e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 7-1-ACP\nTHE USS LOS ANGELES WAS DESTROYED WHILE ON MANEUVERS. IT APPEARS THE LOS ANGELES RAN AGROUND AND ALL HANDS WERE LOST. WE HAVE INITIATED A FULL INVESTIGATION OF THE INCIDENT. OUR CONDOLENCES GO OUT TO FAMILY AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 7-2-ACP\nL'USS Los Angeles è andato distrutto durante una manovra. Riteniamo che il Los Angeles abbia provocato un attacco aprendo il fuoco su un sottomarino sovietico. Abbiamo avviato un'indagine completa sull'incidente. Le nostre condoglianze vanno ai familiari e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 7-2-ACP\nTHE USS LOS ANGELES WAS DESTROYED WHILE ON MANEUVERS. IT IS OUR BELIEF THAT THE LOS ANGELES PROVOKED AN ATTACK BY FIRING ON A SOVIET SUBMARINE. WE HAVE INITIATED A FULL INVESTIGATION OF THE INCIDENT. OUR CONDOLENCES GO OUT TO FAMILY AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [5] = { it = "CODICE: 7-5-ACP\nChe sfortuna, Comandante: mi sarebbe piaciuto vedere cosa avrebbero fatto un paio dei nostri siluri a quel lanciamissili. Ora gli ho messo alle calcagna un altro sottomarino. Rientri in porto: vorrei discutere questa operazione con i suoi uomini e capire come ha fatto a sgusciare via quella serpe.", en = "CODE: 7-5-ACP\nTOUGH BREAK CAPTAIN, I WOULD HAVE LIKED TO HAVE SEEN WHAT A COUPLE OF OUR FISH WOULD DO TO THAT BOOMER. I'VE GOT ANOTHER SUB ON ITS TRAIL NOW. COME ON BACK INTO PORT, I'D LIKE TO DISCUSS THIS OPERATION WITH YOUR MEN AND FIGURE OUT HOW THAT SNAKE GOT AWAY." },
    [6] = { it = "CODICE: 7-6-ACP\nBel lavoro, Comandante. Può scommettere che là fuori ci sono comandanti di SSN sovietici che cercano di pararsi il fondoschiena per aver perso quel lanciamissili! Già questo merita un encomio! La sua prestazione in questa missione sarà annotata nel suo stato di servizio. I miei complimenti all'equipaggio per il sangue freddo sotto pressione. Rientri in porto: dobbiamo prepararla per un'altra missione. Farò in modo che il suo battello riceva un encomio collettivo per i risultati ottenuti.", en = "CODE: 7-6-ACP\nFINE WORK, CAPTAIN. YOU CAN BET THERE ARE SOME SOVIET SSN COMMANDERS OUT THERE TRYING TO COVER THEIR BUTTS FOR LOSING THAT BOOMER! THAT IN ITSELF IS ENOUGH TO MERIT A COMMENDATION! YOUR PERFORMANCE ON THIS MISSION WILL GO DOWN ON YOUR PERMANENT RECORD. MY COMPLIMENTS TO THE CREW FOR COOL ACTION UNDER PRESSURE. COME ON BACK INTO PORT, WE'VE GOT TO GET YOU MOUNTED UP FOR ANOTHER MISSION. I WILL SEE TO IT THAT YOUR SHIP GETS A UNIT COMMENDATION FOR YOUR ACHIEVEMENTS." },
    [7] = { it = "CODICE: 7-7-ACP\nPrestazione eccellente, Comandante! L'ammiraglio voleva promuoverla commodoro e metterla a capo di una squadriglia, ma ho bisogno di tutti i buoni comandanti che riesco a trovare, quindi per ora sono riuscito a farlo desistere. Sarà pronto per una scrivania fin troppo presto: sapevo che avrebbe preferito così. Ci vediamo in banchina. Si prepari a un'ispezione del personale e a una cerimonia di premiazione: per i suoi risultati sarà decorato con la Navy Cross.", en = "CODE: 7-7-ACP\nEXCELLENT PERFORMANCE, CAPTAIN! THE ADMIRAL WANTED TO PROMOTE YOU TO COMMODORE AND PUT YOU IN CHARGE OF A SQUADRON, BUT I NEED ALL THE GOOD CAPTAINS I CAN GET SO I MANAGED TO PUT HIM OFF FOR THE TIME BEING. YOU'LL BE READY FOR DESK JOB SOON ENOUGH. I KNEW YOU'D PREFER IT THAT WAY. SEE YOU ON THE DOCKS. BE PREPARED FOR A PERSONNEL INSPECTION AND AWARDS CEREMONY, YOU WILL BE DECORATED WITH THE NAVY CROSS FOR YOUR ACHIEVEMENTS." },
    [19] = { it = "CODICE: 7-19-ACP\nLa distruzione dell'unità di superficie è una pessima notizia. Accidenti, sono certo che lei ha fatto del suo meglio per garantire la sicurezza del suo battello. Diciamolo: sono bersagli grandi e difficili da mancare. Al suo rientro in porto vorrei ascoltare le registrazioni della centrale di comando. Sono certo che parleranno da sole e rafforzeranno la mia fiducia nelle sue capacità. Rientri presto: stiamo cercando di tenere nascosto l'episodio. Cerchi di non rimuginarci troppo, Comandante: non voglio che influisca sul suo rendimento.", en = "CODE: 7-19-ACP\nTHE DESTRUCTION OF THE SURFACE SHIP IS TERRIBLE NEWS. SHUCKS, I'M SURE YOU DID YOUR BEST TO ENSURE THE SAFETY OF YOUR SHIP. LET'S FACE IT, THEY'RE BIG TARGETS, AND HARD TO MISS. WHEN YOU RETURN TO PORT, I'D LIKE TO LISTEN TO THE RECORDINGS FROM THE CONN. I'M SURE THAT THEY'LL SPEAK FOR THEMSELVES AND REINFORCE MY CONFIDENCE IN YOUR ABILITIES. RETURN TO PORT QUICKLY, WE ARE TRYING TO KEEP A LID ON THIS EPISODE. TRY NOT TO DWELL ON THIS TOO MUCH CAPTAIN, I DON'T WANT IT AFFECTING YOUR PERFORMANCE." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 4, punti = 26, spostamento_max_m = 13196.0064, non_risolte = 0 },
  script = script,
}

script.L_0155 = function(u)
  ::L_0155::
  m688.annulla_timer(u, "L_043a")
  do return end
end

script.L_0159 = function(u)
  ::L_0159::
  if m688.bersaglio_id(u, 2) then goto L_0168 end
  if m688.bersaglio_id(u, 7) then goto L_0168 end
  m688.comportamento(u, "L_0159")
  m688.salva_modello(u)
  do return end
  ::L_0168::
  if m688.traccia_solida(u) then goto L_0176 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 20, "L_0168", "L_01b3")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  do return end
  ::L_0176::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 400, "L_01ae")
  m688.messaggio(u, 55)
  m688.memorizza_bersaglio(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.modello_classe(u, "boa_marcatrice")
  m688.lancia(u, "L_0638", "L_0615")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 255)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0631")
  m688.dopo(u, 40, "L_066e")
  do return end
end

script.L_0168 = function(u)
  ::L_0168::
  if m688.traccia_solida(u) then goto L_0176 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 20, "L_0168", "L_01b3")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  do return end
  ::L_0176::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 400, "L_01ae")
  m688.messaggio(u, 55)
  m688.memorizza_bersaglio(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.modello_classe(u, "boa_marcatrice")
  m688.lancia(u, "L_0638", "L_0615")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 255)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0631")
  m688.dopo(u, 40, "L_066e")
  do return end
end

script.L_01ae = function(u)
  ::L_01ae::
  m688.comportamento(u, "L_0159")
  m688.salva_modello(u)
  do return end
end

script.L_01b3 = function(u)
  ::L_01b3::
  m688.riprendi_navigazione(u)
  m688.comportamento(u, "L_0159")
  m688.salva_modello(u)
  do return end
end

script.L_01c0 = function(u)
  ::L_01c0::
  if m688.manomesso(u) then goto L_01c4 end
  do return end
  ::L_01c4::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_01ce = function(u)
  ::L_01ce::
  if m688.unita_esiste(u, 2) then goto L_01d6 end
  m688.messaggio(u, 106)
  do return end
  ::L_01d6::
  m688.dopo(u, 30, "L_01ce")
  do return end
end

script.L_01dc = function(u)
  ::L_01dc::
  m688.vai_a(u, -117423.6512, -58367.7707, 40, "L_01e7")
  m688.salva_modello(u)
  do return end
end

script.L_01e7 = function(u)
  ::L_01e7::
  m688.vai_a(u, -118100, -85700, 40, "L_01f2")
  m688.salva_modello(u)
  do return end
end

script.L_01f2 = function(u)
  ::L_01f2::
  m688.vai_a(u, -112628.351, -85911.667, 40, "L_01fd")
  m688.salva_modello(u)
  do return end
end

script.L_01fd = function(u)
  ::L_01fd::
  m688.vai_a(u, -112030.2968, -50615.6593, 40, "L_0208")
  m688.salva_modello(u)
  do return end
end

script.L_0208 = function(u)
  ::L_0208::
  m688.vai_a(u, -106776.214, -50703.0065, 40, "L_0213")
  m688.salva_modello(u)
  do return end
end

script.L_0213 = function(u)
  ::L_0213::
  m688.vai_a(u, -107320.9307, -84430.6308, 40, "L_021e")
  m688.salva_modello(u)
  do return end
end

script.L_021e = function(u)
  ::L_021e::
  m688.vai_a(u, -102300, -84500, 40, "L_0229")
  m688.salva_modello(u)
  do return end
end

script.L_0229 = function(u)
  ::L_0229::
  m688.vai_a(u, -101449.6453, -46079.7994, 40, "L_0234")
  m688.salva_modello(u)
  do return end
end

script.L_0234 = function(u)
  ::L_0234::
  m688.vai_a(u, -96199.1783, -46158.7196, 40, "L_023f")
  m688.salva_modello(u)
  do return end
end

script.L_023f = function(u)
  ::L_023f::
  m688.vai_a(u, -95700, -85500, 40, "L_024a")
  m688.salva_modello(u)
  do return end
end

script.L_024a = function(u)
  ::L_024a::
  m688.vai_a(u, -93300, -86100, 40, "L_0255")
  m688.salva_modello(u)
  do return end
end

script.L_0255 = function(u)
  ::L_0255::
  m688.vai_a(u, -91983.4036, -121532.1779, 40, "L_0260")
  m688.salva_modello(u)
  do return end
end

script.L_0260 = function(u)
  ::L_0260::
  m688.vai_a(u, -85657.2645, -43166.3193, 40, "L_026b")
  m688.salva_modello(u)
  do return end
end

script.L_026b = function(u)
  ::L_026b::
  m688.vai_a(u, -84700, -80300, 40, "L_0276")
  m688.salva_modello(u)
  do return end
end

script.L_0276 = function(u)
  ::L_0276::
  m688.vai_a(u, -80189.1732, -79323.068, 40, "L_0281")
  m688.salva_modello(u)
  do return end
end

script.L_0281 = function(u)
  ::L_0281::
  m688.vai_a(u, -79753.1027, -43240.6344, 40, "L_01dc")
  m688.salva_modello(u)
  do return end
end

script.L_028c = function(u)
  ::L_028c::
  m688.vai_a(u, -110203.8526, -59277.0255, 30, "L_029c")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_029c = function(u)
  ::L_029c::
  m688.vai_a(u, -110256.1204, -62414.4949, 30, "L_02ac")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02ac = function(u)
  ::L_02ac::
  m688.vai_a(u, -111598.4164, -63961.096, 30, "L_02bc")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02bc = function(u)
  ::L_02bc::
  m688.vai_a(u, -112954.6985, -66291.7817, 30, "L_02cc")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02cc = function(u)
  ::L_02cc::
  m688.vai_a(u, -112968.0688, -67076.1384, 30, "L_02dc")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02dc = function(u)
  ::L_02dc::
  m688.vai_a(u, -111677.7064, -68667.2498, 30, "L_02ec")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02ec = function(u)
  ::L_02ec::
  m688.vai_a(u, -109715.0261, -69484.7163, 30, "L_02fc")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_02fc = function(u)
  ::L_02fc::
  m688.vai_a(u, -108423.3994, -71075.1787, 30, "L_030c")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_030c = function(u)
  ::L_030c::
  m688.vai_a(u, -105813.2429, -72686.6078, 30, "L_031c")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_031c = function(u)
  ::L_031c::
  m688.vai_a(u, -104544.7292, -75845.0056, 30, "L_032c")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_032c = function(u)
  ::L_032c::
  m688.vai_a(u, -101900, -80300, 30, "L_033c")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_03d3")
  do return end
end

script.L_033c = function(u)
  ::L_033c::
  if m688.unita_id(u, 2) then goto L_0342 end
  do return end
  ::L_0342::
  m688.messaggio(u, 105)
  do return end
end

script.L_0345 = function(u)
  ::L_0345::
  if m688.bersaglio_id(u, 0) then goto L_0352 end
  if m688.bersaglio_arma(u) then goto L_0393 end
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_0352::
  if m688.traccia_solida(u) then goto L_035d end
  m688.ingaggia(u, 30, "L_0352", "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_035d::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_0352", "L_0345")
  if m688.bersaglio_vicino(u, 7.6) then goto L_036d end
  ::L_036b::
  m688.salva_modello(u)
  do return end
  ::L_036d::
  if m688.bersaglio_vicino(u, 1.28) then goto L_036b end
  if m688.variabile_uguale(u, 5, 6) then goto L_036b end
  m688.ingaggia(u, 2, "L_0352", "L_0345")
  m688.salva_modello(u)
  m688.variabile_piu(u, 5)
  script.L_0568(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_038f")
  do return end
  ::L_0393::
  if m688.bersaglio_mi_punta(u) then goto L_039b end
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_039b::
  m688.ingaggia(u, 10, "L_0393", "L_0396")
  if m688.traccia_solida(u) then goto L_03a8 end
  goto L_03bd
  ::L_03a8::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03b0 end
  goto L_03bd
  ::L_03b0::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_03bd::
  m688.salva_modello(u)
  do return end
end

script.L_0352 = function(u)
  ::L_0352::
  if m688.traccia_solida(u) then goto L_035d end
  m688.ingaggia(u, 30, "L_0352", "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_035d::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_0352", "L_0345")
  if m688.bersaglio_vicino(u, 7.6) then goto L_036d end
  ::L_036b::
  m688.salva_modello(u)
  do return end
  ::L_036d::
  if m688.bersaglio_vicino(u, 1.28) then goto L_036b end
  if m688.variabile_uguale(u, 5, 6) then goto L_036b end
  m688.ingaggia(u, 2, "L_0352", "L_0345")
  m688.salva_modello(u)
  m688.variabile_piu(u, 5)
  script.L_0568(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_038f")
  do return end
end

script.L_038f = function(u)
  ::L_038f::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_0393 = function(u)
  ::L_0393::
  if m688.bersaglio_mi_punta(u) then goto L_039b end
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_039b::
  m688.ingaggia(u, 10, "L_0393", "L_0396")
  if m688.traccia_solida(u) then goto L_03a8 end
  goto L_03bd
  ::L_03a8::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03b0 end
  goto L_03bd
  ::L_03b0::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
  ::L_03bd::
  m688.salva_modello(u)
  do return end
end

script.L_0396 = function(u)
  ::L_0396::
  m688.comportamento(u, "L_0345")
  m688.salva_modello(u)
  do return end
end

script.L_03d3 = function(u)
  ::L_03d3::
  m688.telegrafo(u, 0.0078)
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_03dd")
  do return end
end

script.L_03dd = function(u)
  ::L_03dd::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  if m688.unita_id(u, 2) then goto L_03eb end
  m688.quota(u, 152.4, 0.5859)
  ::L_03eb::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_03f2")
  do return end
end

script.L_03f2 = function(u)
  ::L_03f2::
  m688.virata_relativa(u, -179.96)
  if m688.unita_id(u, 2) then goto L_03ff end
  m688.quota(u, 60.96, 0.5859)
  ::L_03ff::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_0406")
  do return end
end

script.L_0406 = function(u)
  ::L_0406::
  m688.riprendi_navigazione(u)
  m688.quota(u, 289.56, 0.5859)
  m688.telegrafo(u, 0.6641)
  m688.salva_modello(u)
  do return end
end

script.L_043a = function(u)
  ::L_043a::
  m688.nuovo_caso(u)
  m688.posizione(u, -118823.1876, -63050.3474)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_04c7 end
  m688.posizione(u, -122814.7517, -65331.1007)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_04c7 end
  m688.posizione(u, -119622.6699, -70881.9384)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_04c7 end
  m688.posizione(u, -115615.3119, -67814.9085)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_04c7 end
  m688.posizione(u, -115738.3869, -74874.0367)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_04c7 end
  m688.posizione(u, -109069.2918, -70279.98)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_04c7 end
  m688.posizione(u, -108500.2921, -75781.306)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_04c7 end
  m688.posizione(u, -108577.1453, -80487.399)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_04c7 end
  m688.posizione(u, -102615.3712, -79013.3961)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_04c7 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_04c7 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_04c7 end
  m688.posizione(u, -99153.3156, -68082.5834)
  m688.rotta_ora(u, 300)
  ::L_04c7::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_043a")
  do return end
end

script.L_04df = function(u)
  ::L_04df::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_055b = function(u)
  ::L_055b::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0565")
  do return end
end

script.L_0565 = function(u)
  ::L_0565::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0568 = function(u)
  ::L_0568::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0638", "L_0615")
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
  m688.dopo(u, 150, "L_05cf")
  m688.dopo(u, 170, "L_05f8")
  m688.dopo(u, 240, "L_0638")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_05ab = function(u)
  ::L_05ab::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0638", "L_0615")
  script.L_05d2(u)
  m688.dopo(u, 170, "L_05cf")
  m688.dopo(u, 190, "L_05f8")
  m688.dopo(u, 280, "L_0638")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_05cf = function(u)
  ::L_05cf::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_05d2 = function(u)
  ::L_05d2::
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

script.L_05f8 = function(u)
  ::L_05f8::
  m688.annulla_timer(u, "L_05f8")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0615 = function(u)
  ::L_0615::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0603")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0631")
  m688.dopo(u, 10, "L_0629")
  do return end
end

script.L_0603 = function(u)
  ::L_0603::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0629")
  m688.annulla_timer(u, "L_0615")
  m688.insegui(u, 0, "L_0638", "L_0615")
  m688.salva_modello(u)
  do return end
end

script.L_0629 = function(u)
  ::L_0629::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0631 = function(u)
  ::L_0631::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0631")
  do return end
end

script.L_0638 = function(u)
  ::L_0638::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_063c = function(u)
  ::L_063c::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0638", "L_0615")
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
  m688.dopo(u, 2, "L_0669")
  m688.dopo(u, 60, "L_066e")
  do return end
end

script.L_0669 = function(u)
  ::L_0669::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_066e = function(u)
  ::L_066e::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0672 = function(u)
  ::L_0672::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0677 = function(u)
  ::L_0677::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0615")
  do return end
end

script.L_0681 = function(u, p)
  ::L_0681::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 20, "L_0690")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0690 = function(u)
  ::L_0690::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0603")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_069e end
  m688.messaggio_al_giocatore(u, 42)
  ::L_069e::
  do return end
end

script.L_069f = function(u, p)
  ::L_069f::
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 30, "L_06aa")
  m688.salva_modello(u)
  do return end
end

script.L_06aa = function(u)
  ::L_06aa::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_06b1 = function(u, p)
  ::L_06b1::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06b6 = function(u, p)
  ::L_06b6::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06bd = function(u, p)
  ::L_06bd::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_06c5 = function(u)
  ::L_06c5::
  m688.annulla_timer(u, "L_0704")
  m688.dopo(u, 20, "L_0704")
  do return end
end

script.L_06ce = function(u)
  ::L_06ce::
  m688.annulla_timer(u, "L_0707")
  m688.dopo(u, 20, "L_0707")
  do return end
end

script.L_06d7 = function(u)
  ::L_06d7::
  m688.annulla_timer(u, "L_070a")
  m688.dopo(u, 20, "L_070a")
  do return end
end

script.L_06e0 = function(u)
  ::L_06e0::
  m688.annulla_timer(u, "L_070d")
  m688.dopo(u, 20, "L_070d")
  do return end
end

script.L_06e9 = function(u)
  ::L_06e9::
  m688.annulla_timer(u, "L_0710")
  m688.dopo(u, 20, "L_0710")
  do return end
end

script.L_06f2 = function(u)
  ::L_06f2::
  m688.annulla_timer(u, "L_0713")
  m688.dopo(u, 20, "L_0713")
  do return end
end

script.L_06fb = function(u)
  ::L_06fb::
  m688.annulla_timer(u, "L_0716")
  m688.dopo(u, 20, "L_0716")
  do return end
end

script.L_0704 = function(u)
  ::L_0704::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0707 = function(u)
  ::L_0707::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_070a = function(u)
  ::L_070a::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_070d = function(u)
  ::L_070d::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0710 = function(u)
  ::L_0710::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0713 = function(u)
  ::L_0713::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0716 = function(u)
  ::L_0716::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0719 = function(u)
  ::L_0719::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0721 = function(u, p)
  ::L_0721::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0725 = function(u)
  ::L_0725::
  m688.dopo(u, 60, "L_072f")
  do return end
end

script.L_072b = function(u, p)
  ::L_072b::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_072f = function(u)
  ::L_072f::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0735 = function(u)
  ::L_0735::
  m688.dopo(u, 40, "L_073b")
  do return end
end

script.L_073b = function(u)
  ::L_073b::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0741 = function(u)
  ::L_0741::
  m688.annulla_timer(u, "L_072f")
  m688.annulla_timer(u, "L_073b")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_074b = function(u, p)
  ::L_074b::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0750 = function(u, p)
  ::L_0750::
  m688.posizione(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468))
  m688.salva_modello(u)
  do return end
end

return missione
