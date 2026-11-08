-- SCENE7B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE7B",
  missione = 7,
  titolo = { it = "Passerella", en = "CAT WALK" },
  lato = "urss",
  teatro = "kunashir",
  posa = { metodo = "georef", centro = { lat = 43.8800, lon = 145.3190 }, metri_per_unita = { x = 20.256, y = 24.597 }, moto_classico_m_s_per_nodo = 1.0779, affidabilita = "media", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 105.156, 167.64, 195.072 } },
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0695" },
    esca = { gestore = "L_0769" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_07cc",
    telegrafo = "L_07de",
    quota = "L_07e3",
    timone = "L_07ea",
    pilota_automatico_off = "L_0846",
    periscopio = "L_084e",
    carica_tubo_1 = "L_07f2",
    carica_tubo_2 = "L_07fb",
    carica_tubo_3 = "L_0804",
    carica_tubo_4 = "L_080d",
    carica_tubo_5 = "L_0816",
    carica_tubo_6 = "L_081f",
    carica_tubo_7 = "L_0828",
    schiera_cala = "L_0852",
    schiera_recupera = "L_0862",
    schiera_persa = "L_086e",
    designa_bersaglio = "L_0878",
    posizione = "L_087d",
    antenna = "L_0858",
    guida_e0 = "L_079f",
    guida_e1 = "L_07a4",
    guida_e2 = "L_07ae",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.33203125, probabilita_originale = "85/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047f", reazione = nil, arrivo = "L_03c6" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0316" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -108700, y = -70700 }, originale = { x = 5472, y = 2464 }, spostamento_m = 13196.0064,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -112309.6754, y = -67087.3699 }, { x = -112309.6754, y = -67087.3699 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_01b8", reazione = nil, arrivo = "L_0170" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01a3" }, { dopo_s = 120, unita = 0, gestore = "L_0300" }, { dopo_s = 60, unita = 1, gestore = "L_0567" }, { dopo_s = 50, unita = 7, gestore = "L_01b1" }, { dopo_s = 60, unita = 7, gestore = "L_0152" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.234375, probabilita_originale = "15/64", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047f", reazione = nil, arrivo = "L_03c6" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0316" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -99300, y = -62900 }, originale = { x = 5600, y = 3040 }, spostamento_m = 4227.8377,
        quota_m = 23.5, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -112309.6754, y = -67087.3699 }, { x = -112309.6754, y = -67087.3699 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_01b8", reazione = nil, arrivo = "L_0170" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01a3" }, { dopo_s = 120, unita = 0, gestore = "L_0300" }, { dopo_s = 60, unita = 1, gestore = "L_0567" }, { dopo_s = 50, unita = 7, gestore = "L_01b1" }, { dopo_s = 60, unita = 7, gestore = "L_0152" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21875, probabilita_originale = "7/32", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047f", reazione = nil, arrivo = "L_03c6" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0316" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -112700, y = -74500 }, originale = { x = 4864, y = 2688 }, spostamento_m = 501.953,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -112309.6754, y = -67087.3699 }, { x = -112309.6754, y = -67087.3699 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_01b8", reazione = nil, arrivo = "L_0170" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01a3" }, { dopo_s = 120, unita = 0, gestore = "L_0300" }, { dopo_s = 60, unita = 1, gestore = "L_0567" }, { dopo_s = 50, unita = 7, gestore = "L_01b1" }, { dopo_s = 60, unita = 7, gestore = "L_0152" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21484375, probabilita_originale = "55/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047f", reazione = nil, arrivo = "L_03c6" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_060c" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0316" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -106100, y = -68700 }, originale = { x = 5344, y = 2848 }, spostamento_m = 4370.5103,
        quota_m = 17.75, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -112309.6754, y = -67087.3699 }, { x = -112309.6754, y = -67087.3699 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_01b8", reazione = nil, arrivo = "L_0170" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01a3" }, { dopo_s = 120, unita = 0, gestore = "L_0300" }, { dopo_s = 60, unita = 1, gestore = "L_0567" }, { dopo_s = 50, unita = 7, gestore = "L_01b1" }, { dopo_s = 60, unita = 7, gestore = "L_0152" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
  },
  briefing = {
    it = { "Kapitan, deve proteggere a ogni costo la PLARB Krasnogvardeets! Pattugli la zona antistante e distrugga qualsiasi unità da guerra minacciosa incontri! Non disonori la sua famiglia e non deluda il suo Paese!" },
    en = { "KAPITAN, YOU MUST PROTECT THE PLARB KRASNOGVARDEETS AT ALL COSTS! PATROL THE AREA AHEAD AND DESTROY ANY THREATENING WARSHIPS YOU ENCOUNTER! DO NOT SHAME YOUR FAMILIES, NOR FAIL YOUR COUNTRY!." },
  },
  esiti = {
    [1] = { it = "CODICE: 7-1-BKB\nIl Leninskii Komsomol è andato perduto in mare mentre difendeva coraggiosamente la Patria dagli aggressori fascisti. Questo equipaggio ha eroicamente sconfitto i fuchi del capitalismo prima di cadere per mano dei nemici. Gli uomini del Leninskii Komsomol, e gli ideali di progresso e socialismo per cui sono morti, non saranno mai dimenticati! Salutiamo il Leninskii Komsomol!", en = "CODE: 7-1-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE MOTHERLAND FROM FASCIST AGGRESSORS. THIS CREW HEROICALLY VANQUISHED THE DRONES OF CAPITALISM BEFORE FALLING TO ITS FOES. THE MEN OF THE LENINSKII KOMSOMOL AND THE IDEALS OF PROGRESS AND SOCIALISM FOR WHICH THEY DIED SHALL NEVER BE FORGOTTEN! WE SALUTE THE LENINSKII KOMSOMOL!" },
    [2] = { it = "CODICE: 7-2-BKB\nIl Leninskii Komsomol è andato perduto in mare mentre difendeva coraggiosamente la Patria dagli aggressori fascisti. Questo equipaggio ha eroicamente sconfitto i fuchi del capitalismo prima di cadere per mano dei nemici. Gli uomini del Leninskii Komsomol, e gli ideali di progresso e socialismo per cui sono morti, non saranno mai dimenticati! Salutiamo il Leninskii Komsomol!", en = "CODE: 7-2-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE MOTHERLAND FROM FASCIST AGGRESSORS. THIS CREW HEROICALLY VANQUISHED THE DRONES OF CAPITALISM BEFORE FALLING TO ITS FOES. THE MEN OF THE LENINSKII KOMSOMOL AND THE IDEALS OF PROGRESS AND SOCIALISM FOR WHICH THEY DIED SHALL NEVER BE FORGOTTEN! WE SALUTE THE LENINSKII KOMSOMOL!" },
    [5] = { it = "CODICE: 7-5-BKB\nCi congratuliamo con l'equipaggio del Leninskii Komsomol per la rapida ed efficace gestione della minaccia imperialista. Il popolo e lo Stato sono fieri del valore e della competenza dell'equipaggio del Leninskii Komsomol. Il progresso del socialismo ne è uscito grandemente accelerato, e al Kapitan sarà conferito l'Ordine di Lenin per il suo coraggio.", en = "CODE: 7-5-BKB\nWE CONGRATULATE THE CREW OF THE LENINSKII KOMSOMOL FOR HER QUICK AND EFFECTIVE HANDLING OF THE IMPERIALIST THREAT. THE PEOPLE AND STATE ARE PROUD OF THE VALOR AND COMPETENCE OF THE CREW OF THE LENINSKII KOMSOMOL. THE PROGRESS OF SOCIALISM HAS BEEN GREATLY HASTENED AND THE KAPITAN WILL RECEIVE THE DECORATION OF LENIN FOR HIS COURAGE." },
    [6] = { it = "CODICE: 7-6-BKB\nIl Leninskii Komsomol ha disonorato il suo popolo permettendo la distruzione della PLARB Krasnogvardeets. Il Soviet Supremo non può passare sopra a questa azione. A monito per la Marina sovietica, e in rispetto delle vite perdute a bordo della PLARB Krasnogvardeets, lo zampolit assumerà il comando e abbandonerà questo Kapitan alla deriva senza nemmeno una zattera di salvataggio, né una copia del Capitale.", en = "CODE: 7-6-BKB\nTHE LENINSKY KOMSOMOL SHAMES ITS PEOPLE BY ALLOWING THE DESTRUCTION OF THE PLARB KRASNOGVARDEETS. THIS ACTION CAN NOT BE OVERLOOKED BY THE SUPREME SOVIET. AS A DEMONSTRATION TO THE SOVIET NAVY, AND IN RESPECT FOR THE LIVES LOST ABOARD THE PLARB KRASNOGVARDEETS, THE ZAMPOLIT WILL TAKE COMMAND AND SET THIS KAPITAN ADRIFT WITH NOT SO MUCH AS A LIFE RAFT, NOR A COPY OF DAS KAPITAL." },
    [7] = { it = "CODICE: 7-7-BKB\nCi congratuliamo con l'equipaggio del Leninskii Komsomol per la rapida ed efficace gestione della minaccia imperialista. Il popolo e lo Stato sono fieri del valore e della competenza dell'equipaggio del Leninskii Komsomol. La distruzione dello strumento imperialista è stata sfortunata, ma tali sono le tribolazioni della guerra. Al Kapitan sarà conferito l'Ordine di Lenin per il suo coraggio.", en = "CODE: 7-7-BKB\nWE CONGRATULATE THE CREW OF THE LENINSKII KOMSOMOL FOR HER QUICK AND EFFECTIVE HANDLING OF THE IMPERIALIST THREAT. THE PEOPLE AND STATE ARE PROUD OF THE VALOR AND COMPETENCE OF THE CREW OF THE LENINSKII KOMSOMOL. THE DESTRUCTION OF THE IMPERIALIST TOOL WAS UNFORTUNATE BUT SUCH ARE THE TRIBULATIONS OF WAR. THE KAPITAN WILL RECEIVE THE DECORATION OF LENIN FOR HIS COURAGE." },
    [19] = { it = "CODICE: 7-19-BKB\nSono tentato di cacciare l'intero equipaggio del Leninskii Komsomol dal paradiso dei lavoratori e di gettarlo tra le fauci dei capitalisti! Così però negherei alle famiglie della nave che avete appena distrutto la possibilità di vedervi tutti quanti ai lavori forzati nelle miniere di carbone. Lo zampolit assume il comando del Leninskii Komsomol e ha l'ordine di rientrare subito in porto!", en = "CODE: 7-19-BKB\nI'M TEMPTED TO CAST THE ENTIRE CREW OF THE LENINSKII KOMSOMOL OUT OF THE WORKER'S PARADISE AND INTO THE JAWS OF THE CAPITALISTS! HOWEVER, THAT WOULD DENY THE FAMILIES OF THE SHIP THAT YOU JUST DESTROYED THE OPPORTUNITY TO SEE THE LOT OF YOU LABOR IN THE COAL MINES. THE ZAMPOLIT IS HEREBY GIVEN COMMAND OF THE LENINSKII KOMSOMOL AND ORDERED TO RETURN TO PORT AT ONCE!" },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 4, punti = 22, spostamento_max_m = 13196.0064, non_risolte = 0 },
  script = script,
}

script.L_0152 = function(u)
  ::L_0152::
  m688.telegrafo(u, 0.0195)
  m688.quota(u, 304.8, 1)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0165")
  do return end
end

script.L_0165 = function(u)
  ::L_0165::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.dopo(u, 520, "L_0152")
  do return end
end

script.L_0170 = function(u)
  ::L_0170::
  m688.telegrafo(u, 0.1953)
  m688.vai_a(u, -112309.6754, -67087.3699, 15, "L_0184")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0152")
  m688.annulla_timer(u, "L_0165")
  do return end
end

script.L_0184 = function(u)
  ::L_0184::
  m688.rotta(u, 0)
  m688.rateo_virata(u, -1.8)
  m688.schiera_trainata(u, 1)
  m688.quota(u, 304.8, 1)
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  m688.variabile(u, 6, 1)
  script.L_0222(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
end

script.L_01a3 = function(u)
  ::L_01a3::
  if m688.manomesso(u) then goto L_01a7 end
  do return end
  ::L_01a7::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_01b1 = function(u)
  ::L_01b1::
  m688.marca_contatti(u)
  m688.dopo(u, 28, "L_01b1")
  do return end
end

script.L_01b8 = function(u)
  ::L_01b8::
  if m688.bersaglio_arma(u) then goto L_01da end
  if m688.bersaglio_id(u, 2) then goto L_022f end
  if m688.bersaglio_id(u, 0) then goto L_01ca end
  ::L_01c5::
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  do return end
  ::L_01ca::
  if m688.traccia_solida(u) then goto L_01d0 end
  goto L_01c5
  ::L_01d0::
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 9) then goto L_0282 end
  do return end
  ::L_01da::
  if m688.bersaglio_mi_punta(u) then goto L_01e0 end
  goto L_01c5
  ::L_01e0::
  m688.ingaggia(u, 10, "L_01da", "L_01c5")
  if m688.traccia_solida(u) then goto L_01ec end
  goto L_021a
  ::L_01ec::
  if m688.bersaglio_vicino(u, 1.28) then goto L_01f4 end
  goto L_021a
  ::L_01f4::
  if m688.variabile_uguale(u, 4, 1) then goto L_021a end
  m688.quota(u, 9.144, 1)
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  m688.variabile(u, 4, 1)
  m688.dopo(u, 120, "L_021c")
  script.L_0769(u)
  script.L_0222(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
  ::L_021a::
  m688.salva_modello(u)
  do return end
  ::L_022f::
  script.L_0222(u)
  if m688.traccia_solida(u) then goto L_0261 end
  if m688.variabile_uguale(u, 6, 0) then goto L_0240 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.25)
  ::L_0240::
  m688.variabile_piu(u, 7)
  if m688.variabile_uguale(u, 7, 10) then goto L_0252 end
  m688.ingaggia(u, 15, "L_022f", "L_02a2")
  m688.salva_modello(u)
  do return end
  ::L_0252::
  m688.variabile(u, 7, 0)
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
  ::L_0261::
  if m688.variabile_uguale(u, 6, 0) then goto L_026b end
  m688.uguaglia_quota(u)
  m688.uguaglia_velocita(u)
  m688.accosta_verso_bersaglio(u)
  ::L_026b::
  m688.variabile_piu(u, 7)
  if m688.variabile_uguale(u, 7, 10) then goto L_0252 end
  m688.ingaggia(u, 15, "L_022f", "L_02a2")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 9) then goto L_0282 end
  do return end
  ::L_0282::
  if m688.variabile_uguale(u, 2, 4) then goto L_029d end
  m688.variabile_piu(u, 2)
  m688.marca_contatti(u)
  script.L_06d8(u)
  m688.seleziona(u, 1)
  m688.dopo(u, 240, "L_029e")
  m688.dopo(u, 60, "L_02b8")
  ::L_029d::
  do return end
end

script.L_01c5 = function(u)
  ::L_01c5::
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  do return end
end

script.L_01da = function(u)
  ::L_01c5::
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  do return end
  ::L_01da::
  if m688.bersaglio_mi_punta(u) then goto L_01e0 end
  goto L_01c5
  ::L_01e0::
  m688.ingaggia(u, 10, "L_01da", "L_01c5")
  if m688.traccia_solida(u) then goto L_01ec end
  goto L_021a
  ::L_01ec::
  if m688.bersaglio_vicino(u, 1.28) then goto L_01f4 end
  goto L_021a
  ::L_01f4::
  if m688.variabile_uguale(u, 4, 1) then goto L_021a end
  m688.quota(u, 9.144, 1)
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  m688.variabile(u, 4, 1)
  m688.dopo(u, 120, "L_021c")
  script.L_0769(u)
  script.L_0222(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
  ::L_021a::
  m688.salva_modello(u)
  do return end
end

script.L_021c = function(u)
  ::L_021c::
  m688.variabile(u, 4, 0)
  do return end
end

script.L_0222 = function(u)
  ::L_0222::
  m688.annulla_timer(u, "L_02b8")
  m688.annulla_timer(u, "L_02c6")
  m688.annulla_timer(u, "L_02da")
  m688.annulla_timer(u, "L_02ed")
  do return end
end

script.L_022f = function(u)
  ::L_022f::
  script.L_0222(u)
  if m688.traccia_solida(u) then goto L_0261 end
  if m688.variabile_uguale(u, 6, 0) then goto L_0240 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.25)
  ::L_0240::
  m688.variabile_piu(u, 7)
  if m688.variabile_uguale(u, 7, 10) then goto L_0252 end
  m688.ingaggia(u, 15, "L_022f", "L_02a2")
  m688.salva_modello(u)
  do return end
  ::L_0252::
  m688.variabile(u, 7, 0)
  m688.comportamento(u, "L_01b8")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
  ::L_0261::
  if m688.variabile_uguale(u, 6, 0) then goto L_026b end
  m688.uguaglia_quota(u)
  m688.uguaglia_velocita(u)
  m688.accosta_verso_bersaglio(u)
  ::L_026b::
  m688.variabile_piu(u, 7)
  if m688.variabile_uguale(u, 7, 10) then goto L_0252 end
  m688.ingaggia(u, 15, "L_022f", "L_02a2")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 9) then goto L_0282 end
  do return end
  ::L_0282::
  if m688.variabile_uguale(u, 2, 4) then goto L_029d end
  m688.variabile_piu(u, 2)
  m688.marca_contatti(u)
  script.L_06d8(u)
  m688.seleziona(u, 1)
  m688.dopo(u, 240, "L_029e")
  m688.dopo(u, 60, "L_02b8")
  ::L_029d::
  do return end
end

script.L_029e = function(u)
  ::L_029e::
  m688.variabile_meno(u, 2)
  do return end
end

script.L_02a2 = function(u)
  ::L_02a2::
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.comportamento(u, "L_01b8")
  m688.telegrafo(u, 0.0195)
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 6, 0) then goto L_02b7 end
  m688.dopo(u, 60, "L_02b8")
  ::L_02b7::
  do return end
end

script.L_02b8 = function(u)
  ::L_02b8::
  m688.impulso_sonar(u)
  m688.rateo_virata(u, -0.72)
  m688.rotta(u, 0)
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_02c6")
  do return end
end

script.L_02c6 = function(u)
  ::L_02c6::
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.rateo_virata(u, -0.72)
  m688.rotta(u, 180)
  m688.quota(u, 304.8, 1)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02da")
  do return end
end

script.L_02da = function(u)
  ::L_02da::
  m688.impulso_sonar(u)
  m688.quota(u, 304.8, 1)
  m688.rateo_virata(u, -0.72)
  m688.rotta(u, 90)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02ed")
  do return end
end

script.L_02ed = function(u)
  ::L_02ed::
  m688.impulso_sonar(u)
  m688.rateo_virata(u, -0.72)
  m688.rotta(u, 270)
  m688.quota(u, 304.8, 1)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02b8")
  do return end
end

script.L_0300 = function(u)
  ::L_0300::
  if m688.unita_esiste(u, 2) then goto L_0308 end
  m688.messaggio(u, 106)
  do return end
  ::L_0308::
  if m688.unita_esiste(u, 7) then goto L_0310 end
  m688.messaggio(u, 107)
  do return end
  ::L_0310::
  m688.dopo(u, 30, "L_0300")
  do return end
end

script.L_0316 = function(u)
  ::L_0316::
  m688.vai_a(u, -117423.6512, -58367.7707, 40, "L_0321")
  m688.salva_modello(u)
  do return end
end

script.L_0321 = function(u)
  ::L_0321::
  m688.vai_a(u, -118100, -85700, 40, "L_032c")
  m688.salva_modello(u)
  do return end
end

script.L_032c = function(u)
  ::L_032c::
  m688.vai_a(u, -112628.351, -85911.667, 40, "L_0337")
  m688.salva_modello(u)
  do return end
end

script.L_0337 = function(u)
  ::L_0337::
  m688.vai_a(u, -112030.2968, -50615.6593, 40, "L_0342")
  m688.salva_modello(u)
  do return end
end

script.L_0342 = function(u)
  ::L_0342::
  m688.vai_a(u, -106776.214, -50703.0065, 40, "L_034d")
  m688.salva_modello(u)
  do return end
end

script.L_034d = function(u)
  ::L_034d::
  m688.vai_a(u, -107320.9307, -84430.6308, 40, "L_0358")
  m688.salva_modello(u)
  do return end
end

script.L_0358 = function(u)
  ::L_0358::
  m688.vai_a(u, -102300, -84500, 40, "L_0363")
  m688.salva_modello(u)
  do return end
end

script.L_0363 = function(u)
  ::L_0363::
  m688.vai_a(u, -101449.6453, -46079.7994, 40, "L_036e")
  m688.salva_modello(u)
  do return end
end

script.L_036e = function(u)
  ::L_036e::
  m688.vai_a(u, -96199.1783, -46158.7196, 40, "L_0379")
  m688.salva_modello(u)
  do return end
end

script.L_0379 = function(u)
  ::L_0379::
  m688.vai_a(u, -95700, -85500, 40, "L_0384")
  m688.salva_modello(u)
  do return end
end

script.L_0384 = function(u)
  ::L_0384::
  m688.vai_a(u, -93300, -86100, 40, "L_038f")
  m688.salva_modello(u)
  do return end
end

script.L_038f = function(u)
  ::L_038f::
  m688.vai_a(u, -91983.4036, -121532.1779, 40, "L_039a")
  m688.salva_modello(u)
  do return end
end

script.L_039a = function(u)
  ::L_039a::
  m688.vai_a(u, -85657.2645, -43166.3193, 40, "L_03a5")
  m688.salva_modello(u)
  do return end
end

script.L_03a5 = function(u)
  ::L_03a5::
  m688.vai_a(u, -84700, -80300, 40, "L_03b0")
  m688.salva_modello(u)
  do return end
end

script.L_03b0 = function(u)
  ::L_03b0::
  m688.vai_a(u, -80189.1732, -79323.068, 40, "L_03bb")
  m688.salva_modello(u)
  do return end
end

script.L_03bb = function(u)
  ::L_03bb::
  m688.vai_a(u, -79753.1027, -43240.6344, 40, "L_0316")
  m688.salva_modello(u)
  do return end
end

script.L_03c6 = function(u)
  ::L_03c6::
  m688.vai_a(u, -110203.8526, -59277.0255, 30, "L_03d6")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_03d6 = function(u)
  ::L_03d6::
  m688.vai_a(u, -110256.1204, -62414.4949, 30, "L_03e6")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_03e6 = function(u)
  ::L_03e6::
  m688.vai_a(u, -111598.4164, -63961.096, 30, "L_03f6")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_03f6 = function(u)
  ::L_03f6::
  m688.vai_a(u, -112954.6985, -66291.7817, 30, "L_0406")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0406 = function(u)
  ::L_0406::
  m688.vai_a(u, -112968.0688, -67076.1384, 30, "L_0416")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0416 = function(u)
  ::L_0416::
  m688.vai_a(u, -111677.7064, -68667.2498, 30, "L_0426")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0426 = function(u)
  ::L_0426::
  m688.vai_a(u, -109715.0261, -69484.7163, 30, "L_0436")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0436 = function(u)
  ::L_0436::
  m688.vai_a(u, -108423.3994, -71075.1787, 30, "L_0446")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0446 = function(u)
  ::L_0446::
  m688.vai_a(u, -105813.2429, -72686.6078, 30, "L_0456")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0456 = function(u)
  ::L_0456::
  m688.vai_a(u, -104544.7292, -75845.0056, 30, "L_0466")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0466 = function(u)
  ::L_0466::
  m688.vai_a(u, -101900, -80300, 30, "L_0476")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0500")
  do return end
end

script.L_0476 = function(u)
  ::L_0476::
  if m688.unita_id(u, 2) then goto L_047c end
  do return end
  ::L_047c::
  m688.messaggio(u, 105)
  do return end
end

script.L_047f = function(u)
  ::L_047f::
  if m688.bersaglio_id(u, 7) then goto L_048c end
  if m688.bersaglio_arma(u) then goto L_04c0 end
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_048c::
  if m688.traccia_solida(u) then goto L_0497 end
  m688.ingaggia(u, 30, "L_048c", "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_0497::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_048c", "L_047f")
  if m688.bersaglio_vicino(u, 7.6) then goto L_04a7 end
  ::L_04a5::
  m688.salva_modello(u)
  do return end
  ::L_04a7::
  if m688.bersaglio_vicino(u, 1.28) then goto L_04a5 end
  if m688.flag_scena_uguale(u, 1) then goto L_04a5 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_04bc")
  goto L_0695
  ::L_04c0::
  if m688.bersaglio_mi_punta(u) then goto L_04c8 end
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_04c8::
  m688.ingaggia(u, 10, "L_04c0", "L_04c3")
  if m688.traccia_solida(u) then goto L_04d5 end
  goto L_04ea
  ::L_04d5::
  if m688.bersaglio_vicino(u, 1.28) then goto L_04dd end
  goto L_04ea
  ::L_04dd::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_04ea::
  m688.salva_modello(u)
  do return end
  ::L_0695::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0765", "L_0742")
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
  m688.dopo(u, 150, "L_06fc")
  m688.dopo(u, 170, "L_0725")
  m688.dopo(u, 240, "L_0765")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_048c = function(u)
  ::L_048c::
  if m688.traccia_solida(u) then goto L_0497 end
  m688.ingaggia(u, 30, "L_048c", "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_0497::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_048c", "L_047f")
  if m688.bersaglio_vicino(u, 7.6) then goto L_04a7 end
  ::L_04a5::
  m688.salva_modello(u)
  do return end
  ::L_04a7::
  if m688.bersaglio_vicino(u, 1.28) then goto L_04a5 end
  if m688.flag_scena_uguale(u, 1) then goto L_04a5 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_04bc")
  goto L_0695
  ::L_0695::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0765", "L_0742")
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
  m688.dopo(u, 150, "L_06fc")
  m688.dopo(u, 170, "L_0725")
  m688.dopo(u, 240, "L_0765")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_04bc = function(u)
  ::L_04bc::
  m688.flag_scena(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_04c0 = function(u)
  ::L_04c0::
  if m688.bersaglio_mi_punta(u) then goto L_04c8 end
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_04c8::
  m688.ingaggia(u, 10, "L_04c0", "L_04c3")
  if m688.traccia_solida(u) then goto L_04d5 end
  goto L_04ea
  ::L_04d5::
  if m688.bersaglio_vicino(u, 1.28) then goto L_04dd end
  goto L_04ea
  ::L_04dd::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
  ::L_04ea::
  m688.salva_modello(u)
  do return end
end

script.L_04c3 = function(u)
  ::L_04c3::
  m688.comportamento(u, "L_047f")
  m688.salva_modello(u)
  do return end
end

script.L_0500 = function(u)
  ::L_0500::
  m688.telegrafo(u, 0.0078)
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_050a")
  do return end
end

script.L_050a = function(u)
  ::L_050a::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  if m688.unita_id(u, 2) then goto L_0518 end
  m688.quota(u, 152.4, 0.5859)
  ::L_0518::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_051f")
  do return end
end

script.L_051f = function(u)
  ::L_051f::
  m688.virata_relativa(u, -179.96)
  if m688.unita_id(u, 2) then goto L_052c end
  m688.quota(u, 60.96, 0.5859)
  ::L_052c::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_0533")
  do return end
end

script.L_0533 = function(u)
  ::L_0533::
  m688.riprendi_navigazione(u)
  m688.quota(u, 289.56, 0.5859)
  m688.telegrafo(u, 0.4688)
  m688.salva_modello(u)
  do return end
end

script.L_0567 = function(u)
  ::L_0567::
  m688.nuovo_caso(u)
  m688.posizione(u, -118823.1876, -63050.3474)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_05f4 end
  m688.posizione(u, -122814.7517, -65331.1007)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_05f4 end
  m688.posizione(u, -119622.6699, -70881.9384)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_05f4 end
  m688.posizione(u, -115615.3119, -67814.9085)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_05f4 end
  m688.posizione(u, -115738.3869, -74874.0367)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_05f4 end
  m688.posizione(u, -109069.2918, -70279.98)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_05f4 end
  m688.posizione(u, -108500.2921, -75781.306)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_05f4 end
  m688.posizione(u, -108577.1453, -80487.399)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_05f4 end
  m688.posizione(u, -102615.3712, -79013.3961)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_05f4 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_05f4 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_05f4 end
  m688.posizione(u, -99153.3156, -68082.5834)
  m688.rotta_ora(u, 300)
  ::L_05f4::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0567")
  do return end
end

script.L_060c = function(u)
  ::L_060c::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0695 = function(u)
  ::L_0695::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0765", "L_0742")
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
  m688.dopo(u, 150, "L_06fc")
  m688.dopo(u, 170, "L_0725")
  m688.dopo(u, 240, "L_0765")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_06d8 = function(u)
  ::L_06d8::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0765", "L_0742")
  script.L_06ff(u)
  m688.dopo(u, 170, "L_06fc")
  m688.dopo(u, 190, "L_0725")
  m688.dopo(u, 280, "L_0765")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_06fc = function(u)
  ::L_06fc::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_06ff = function(u)
  ::L_06ff::
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

script.L_0725 = function(u)
  ::L_0725::
  m688.annulla_timer(u, "L_0725")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0742 = function(u)
  ::L_0742::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0730")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_075e")
  m688.dopo(u, 10, "L_0756")
  do return end
end

script.L_0730 = function(u)
  ::L_0730::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0756")
  m688.annulla_timer(u, "L_0742")
  m688.insegui(u, 0, "L_0765", "L_0742")
  m688.salva_modello(u)
  do return end
end

script.L_0756 = function(u)
  ::L_0756::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_075e = function(u)
  ::L_075e::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_075e")
  do return end
end

script.L_0765 = function(u)
  ::L_0765::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0769 = function(u)
  ::L_0769::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0765", "L_0742")
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
  m688.dopo(u, 2, "L_0796")
  m688.dopo(u, 60, "L_079b")
  do return end
end

script.L_0796 = function(u)
  ::L_0796::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_079b = function(u)
  ::L_079b::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_079f = function(u)
  ::L_079f::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_07a4 = function(u)
  ::L_07a4::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0742")
  do return end
end

script.L_07ae = function(u, p)
  ::L_07ae::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 20, "L_07bd")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_07bd = function(u)
  ::L_07bd::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0730")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_07cb end
  m688.messaggio_al_giocatore(u, 42)
  ::L_07cb::
  do return end
end

script.L_07cc = function(u, p)
  ::L_07cc::
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 30, "L_07d7")
  m688.salva_modello(u)
  do return end
end

script.L_07d7 = function(u)
  ::L_07d7::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_07de = function(u, p)
  ::L_07de::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07e3 = function(u, p)
  ::L_07e3::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07ea = function(u, p)
  ::L_07ea::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_07f2 = function(u)
  ::L_07f2::
  m688.annulla_timer(u, "L_0831")
  m688.dopo(u, 20, "L_0831")
  do return end
end

script.L_07fb = function(u)
  ::L_07fb::
  m688.annulla_timer(u, "L_0834")
  m688.dopo(u, 20, "L_0834")
  do return end
end

script.L_0804 = function(u)
  ::L_0804::
  m688.annulla_timer(u, "L_0837")
  m688.dopo(u, 20, "L_0837")
  do return end
end

script.L_080d = function(u)
  ::L_080d::
  m688.annulla_timer(u, "L_083a")
  m688.dopo(u, 20, "L_083a")
  do return end
end

script.L_0816 = function(u)
  ::L_0816::
  m688.annulla_timer(u, "L_083d")
  m688.dopo(u, 20, "L_083d")
  do return end
end

script.L_081f = function(u)
  ::L_081f::
  m688.annulla_timer(u, "L_0840")
  m688.dopo(u, 20, "L_0840")
  do return end
end

script.L_0828 = function(u)
  ::L_0828::
  m688.annulla_timer(u, "L_0843")
  m688.dopo(u, 20, "L_0843")
  do return end
end

script.L_0831 = function(u)
  ::L_0831::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0834 = function(u)
  ::L_0834::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0837 = function(u)
  ::L_0837::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_083a = function(u)
  ::L_083a::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_083d = function(u)
  ::L_083d::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0840 = function(u)
  ::L_0840::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0843 = function(u)
  ::L_0843::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0846 = function(u)
  ::L_0846::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_084e = function(u, p)
  ::L_084e::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0852 = function(u)
  ::L_0852::
  m688.dopo(u, 60, "L_085c")
  do return end
end

script.L_0858 = function(u, p)
  ::L_0858::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_085c = function(u)
  ::L_085c::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0862 = function(u)
  ::L_0862::
  m688.dopo(u, 40, "L_0868")
  do return end
end

script.L_0868 = function(u)
  ::L_0868::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_086e = function(u)
  ::L_086e::
  m688.annulla_timer(u, "L_085c")
  m688.annulla_timer(u, "L_0868")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0878 = function(u, p)
  ::L_0878::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_087d = function(u, p)
  ::L_087d::
  m688.posizione(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468))
  m688.salva_modello(u)
  do return end
end

return missione
