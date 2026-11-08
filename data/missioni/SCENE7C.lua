-- SCENE7C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE7C",
  missione = 7,
  titolo = { it = "Passerella", en = "CAT WALK" },
  lato = "due_giocatori",
  teatro = "kunashir",
  posa = { metodo = "georef", centro = { lat = 43.8800, lon = 145.3190 }, metri_per_unita = { x = 20.256, y = 24.597 }, moto_classico_m_s_per_nodo = 1.0779, affidabilita = "media", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 67.9704, 159.4104, 257.556 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0544" },
    harpoon = { gestore = "L_04f4" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_05d5" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0638",
    telegrafo = "L_064a",
    quota = "L_064f",
    timone = "L_0656",
    pilota_automatico_off = "L_06b2",
    periscopio = "L_06ba",
    carica_tubo_1 = "L_065e",
    carica_tubo_2 = "L_0667",
    carica_tubo_3 = "L_0670",
    carica_tubo_4 = "L_0679",
    carica_tubo_5 = "L_0682",
    carica_tubo_6 = "L_068b",
    carica_tubo_7 = "L_0694",
    schiera_cala = "L_06be",
    schiera_recupera = "L_06ce",
    schiera_persa = "L_06da",
    designa_bersaglio = "L_06e4",
    posizione = "L_06e9",
    antenna = "L_06c4",
    guida_e0 = "L_060b",
    guida_e1 = "L_0610",
    guida_e2 = "L_061a",
    f0 = "L_06f0",
    f1 = "L_06f9",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.33203125, probabilita_originale = "85/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -108700, y = -70700 }, originale = { x = 5472, y = 2464 }, spostamento_m = 13196.0064,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_037f", reazione = nil, arrivo = "L_02c0" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0210" } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01e8" }, { dopo_s = 180, unita = 1, gestore = "L_01ce" }, { dopo_s = 60, unita = 7, gestore = "L_0133" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.234375, probabilita_originale = "15/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -99300, y = -62900 }, originale = { x = 5600, y = 3040 }, spostamento_m = 4227.8377,
        quota_m = 23.5, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_037f", reazione = nil, arrivo = "L_02c0" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0210" } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01e8" }, { dopo_s = 180, unita = 1, gestore = "L_01ce" }, { dopo_s = 60, unita = 7, gestore = "L_0133" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21875, probabilita_originale = "7/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -112700, y = -74500 }, originale = { x = 4864, y = 2688 }, spostamento_m = 501.953,
        quota_m = 16, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_037f", reazione = nil, arrivo = "L_02c0" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0210" } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01e8" }, { dopo_s = 180, unita = 1, gestore = "L_01ce" }, { dopo_s = 60, unita = 7, gestore = "L_0133" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.21484375, probabilita_originale = "55/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -106100, y = -68700 }, originale = { x = 5344, y = 2848 }, spostamento_m = 4370.5103,
        quota_m = 17.75, quota_originale_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 220, rumore = 8, sonar = 24, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -108811.1161, y = -54592.6381 }, originale = { x = 5024, y = 3520 },
        quota_m = 115.25, quota_originale_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "typhoon", tipo_originale = 31, nome_originale = "TYPHOON CLASS SSBN", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -106094.2206, y = -49144.8585 }, originale = { x = 5152, y = 3744 },
        quota_m = 289.56, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 19, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -109520.2498, y = -57719.2439 }, { x = -110203.8526, y = -59277.0255 }, { x = -110256.1204, y = -62414.4949 }, { x = -111598.4164, y = -63961.096 }, { x = -112954.6985, y = -66291.7817 }, { x = -112968.0688, y = -67076.1384 }, { x = -111677.7064, y = -68667.2498 }, { x = -109715.0261, y = -69484.7163 }, { x = -108423.3994, y = -71075.1787 }, { x = -105813.2429, y = -72686.6078 }, { x = -104544.7292, y = -75845.0056 }, { x = -101900, y = -80300 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_037f", reazione = nil, arrivo = "L_02c0" } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120840.088, y = -65367.5469 }, originale = { x = 4448, y = 3072 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122113.1365, y = -62990.2738 }, originale = { x = 4384, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -122199.9157, y = -67696.3478 }, originale = { x = 4384, y = 2976 },
        quota_m = 0, rotta = 70, velocita_kn = 6.4,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -97100, y = -63500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0478" } },
      { id = 6, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -123192.401, y = -85723.7427 }, originale = { x = 4352, y = 2240 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 70.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -122683.831, y = -58271.9596 }, { x = -117423.6512, y = -58367.7707 }, { x = -117910.4132, y = -85819.8097 }, { x = -112628.351, y = -85911.667 }, { x = -112030.2968, y = -50615.6593 }, { x = -106776.214, y = -50703.0065 }, { x = -107320.9307, y = -84430.6308 }, { x = -102039.9738, y = -84514.0561 }, { x = -101449.6453, y = -46079.7994 }, { x = -96199.1783, y = -46158.7196 }, { x = -96724.7479, y = -82240.2026 }, { x = -91445.5323, y = -82315.1927 }, { x = -91983.4036, y = -121532.1779 }, { x = -85657.2645, y = -43166.3193 }, { x = -86125.6194, y = -79248.4908 }, { x = -80189.1732, y = -79323.068 }, { x = -79753.1027, y = -43240.6344 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0210" } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -93818.6721, y = -63453.2858 }, originale = { x = 5760, y = 3168 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01e8" }, { dopo_s = 180, unita = 1, gestore = "L_01ce" }, { dopo_s = 60, unita = 7, gestore = "L_0133" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCPACFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: CACCIA AL LANCIAMISSILI", "La ricognizione satellitare indica che un sottomarino sovietico della classe Typhoon ha appena lasciato il porto ed è diretto verso il suo settore di pattugliamento, appena a nord del Giappone. Qui siamo in guerra, quindi voglio che lei e i suoi ragazzi sventriate questo pesce per me. Un LAMPS volerà in avanscoperta per passarle le coordinate, se riuscirà a determinare la posizione del lanciamissili balistici. Il Typhoon sarà sicuramente scortato da uno o più sottomarini d'attacco, ma non si impegni con loro se può evitarlo. Se riesce a cogliere Ivan al passaggio, può mandare a picco quel lanciamissili prima che i suoi compagni si accorgano di qualcosa! Facciamolo per il Gipper...", "VICEAMMIRAGLIO P. 'SMOKING GUNS' GRACE", "Kapitan, deve proteggere a ogni costo la PLARB Krasnogvardeets! Pattugli la zona antistante e distrugga qualsiasi unità da guerra minacciosa incontri! Non disonori la sua famiglia e non deluda il suo Paese!" },
    en = { "MISSION ORDERS", "FM: CINCPACFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: BOOMER BANGER", "SATELLITE RECON INDICATES THAT A SOVIET TYPHOON CLASS SUBMARINE HAS JUST LEFT PORT AND IS HEADING FOR YOUR PATROL SECTOR, JUST NORTH OF JAPAN. IT'S WAR OUT HERE, SO I WANT YOU AND YOUR BOYS TO GUT THIS FISH FOR ME. A LAMPS WILL FLY AHEAD TO PASS YOU COORDINATES IF THEY CAN GET A FIX ON THE BOOMER'S POSITION. THE TYPHOON IS SURE TO BE ESCORTED BY ONE OR MORE ATTACK BOATS, BUT DON'T TANGLE WITH THEM IF YOU CAN AVOID IT. IF YOU CAN CATCH IVAN AT THE PASS YOU CAN DEEP SIX THAT BOOMER BEFORE HIS PARTNERS KNOW ANYTHING HAPPENED! LET'S WIN THIS ONE FOR THE GIPPER...", "VADM P. 'SMOKING GUNS' GRACE", "KAPITAN, YOU MUST PROTECT THE PLARB KRASNOGVARDEETS AT ALL COSTS! PATROL THE AREA AHEAD AND DESTROY ANY THREATENING WARSHIPS YOU ENCOUNTER! DO NOT SHAME YOUR FAMILIES, NOR FAIL YOUR COUNTRY!." },
  },
  esiti = {
    [1] = { it = "CODICE: 7-1-CIN\nL'USS Los Angeles è andato distrutto durante una manovra. Sembra che il Los Angeles si sia incagliato e che l'intero equipaggio sia andato perduto. Abbiamo avviato un'indagine completa sull'incidente. Le nostre condoglianze vanno ai familiari e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 7-1-CIN\nTHE USS LOS ANGELES WAS DESTROYED WHILE ON MANEUVERS. IT APPEARS THE LOS ANGELES RAN AGROUND AND ALL HANDS WERE LOST. WE HAVE INITIATED A FULL INVESTIGATION OF THE INCIDENT. OUR CONDOLENCES GO OUT TO FAMILY AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 7-2-CIN\nL'USS Los Angeles è andato distrutto durante una manovra. Riteniamo che il Los Angeles abbia provocato un attacco aprendo il fuoco su un sottomarino sovietico. Abbiamo avviato un'indagine completa sull'incidente. Le nostre condoglianze vanno ai familiari e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 7-2-CIN\nTHE USS LOS ANGELES WAS DESTROYED WHILE ON MANEUVERS. IT IS OUR BELIEF THAT THE LOS ANGELES PROVOKED AN ATTACK BY FIRING ON A SOVIET SUBMARINE. WE HAVE INITIATED A FULL INVESTIGATION OF THE INCIDENT. OUR CONDOLENCES GO OUT TO FAMILY AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 7-3-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare.", en = "CODE: 7-3-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA." },
    [4] = { it = "CODICE: 7-4-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare.", en = "CODE: 7-4-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA." },
    [5] = { it = "CODICE: 7-5-CIN\nChe sfortuna, Comandante: mi sarebbe piaciuto vedere cosa avrebbero fatto un paio dei nostri siluri a quel lanciamissili. Ora gli ho messo alle calcagna un altro sottomarino. Rientri in porto: vorrei discutere questa operazione con i suoi uomini e capire come ha fatto a sgusciare via quella serpe.", en = "CODE: 7-5-CIN\nTOUGH BREAK CAPTAIN, I WOULD HAVE LIKED TO HAVE SEEN WHAT A COUPLE OF OUR FISH WOULD DO TO THAT BOOMER. I'VE GOT ANOTHER SUB ON ITS TRAIL NOW. COME ON BACK INTO PORT, I'D LIKE TO DISCUSS THIS OPERATION WITH YOUR MEN AND FIGURE OUT HOW THAT SNAKE GOT AWAY." },
    [6] = { it = "CODICE: 7-6-CIN\nBel lavoro, Comandante. Può scommettere che là fuori ci sono comandanti di SSN sovietici che cercano di pararsi il fondoschiena per aver perso quel lanciamissili! Già questo merita un encomio! La sua prestazione in questa missione sarà annotata nel suo stato di servizio. I miei complimenti all'equipaggio per il sangue freddo sotto pressione. Rientri in porto: dobbiamo prepararla per un'altra missione. Farò in modo che il suo battello riceva un encomio collettivo per i risultati ottenuti.", en = "CODE: 7-6-CIN\nFINE WORK, CAPTAIN. YOU CAN BET THERE ARE SOME SOVIET SSN COMMANDERS OUT THERE TRYING TO COVER THEIR BUTTS FOR LOSING THAT BOOMER! THAT IN ITSELF IS ENOUGH TO MERIT A COMMENDATION! YOUR PERFORMANCE ON THIS MISSION WILL GO DOWN ON YOUR PERMANENT RECORD. MY COMPLIMENTS TO THE CREW FOR COOL ACTION UNDER PRESSURE. COME ON BACK INTO PORT, WE'VE GOT TO GET YOU MOUNTED UP FOR ANOTHER MISSION. I WILL SEE TO IT THAT YOUR SHIP GETS A UNIT COMMENDATION FOR YOUR ACHIEVEMENTS." },
    [7] = { it = "CODICE: 7-7-CIN\nCi congratuliamo con l'equipaggio del Leninskii Komsomol per la rapida ed efficace gestione della minaccia imperialista. Il popolo e lo Stato sono fieri del valore e della competenza dell'equipaggio del Leninskii Komsomol. Il progresso del socialismo ne è uscito grandemente accelerato, e al Kapitan sarà conferito l'Ordine di Lenin per il suo coraggio.", en = "CODE: 7-7-CIN\nWE CONGRATULATE THE CREW OF THE LENINSKII KOMSOMOL FOR HER QUICK AND EFFECTIVE HANDLING OF THE IMPERIALIST THREAT. THE PEOPLE AND STATE ARE PROUD OF THE VALOR AND COMPETENCE OF THE CREW OF THE LENINSKII KOMSOMOL. THE PROGRESS OF SOCIALISM HAS BEEN GREATLY HASTENED AND THE KAPITAN WILL RECEIVE THE DECORATION OF LENIN FOR HIS COURAGE." },
    [8] = { it = "CODICE: 7-8-CIN\nIl Leninskii Komsomol ha disonorato il suo popolo permettendo la distruzione della PLARB Krasnogvardeets. Il Soviet Supremo non può passare sopra a questa azione. A monito per la Marina sovietica, e in rispetto delle vite perdute a bordo della PLARB Krasnogvardeets, lo zampolit assumerà il comando e abbandonerà questo Kapitan alla deriva senza nemmeno una zattera di salvataggio, né una copia del Capitale.", en = "CODE: 7-8-CIN\nTHE LENINSKY KOMSOMOL SHAMES ITS PEOPLE BY ALLOWING THE DESTRUCTION OF THE PLARB KRASNOGVARDEETS. THIS ACTION CAN NOT BE OVERLOOKED BY THE SUPREME SOVIET. AS A DEMONSTRATION TO THE SOVIET NAVY, AND IN RESPECT FOR THE LIVES LOST ABOARD THE PLARB KRASNOGVARDEETS, THE ZAMPOLIT WILL TAKE COMMAND AND SET THIS KAPITAN ADRIFT WITH NOT SO MUCH AS A LIFE RAFT, NOR A COPY OF DAS KAPITAL." },
    [9] = { it = "CODICE: 7-9-CIN\nLei ha lasciato l'area di gioco: missione annullata.", en = "CODE: 7-9-CIN\nYOU LEFT THE PLAYFIELD MISSION ABORTED." },
    [10] = { it = "CODICE: 7-10-CIN\nIl suo avversario ha lasciato la mappa: missione annullata.", en = "CODE: 7-10-CIN\nYOUR OPPONENT LEFT THE MAP, MISSION ABORTED." },
    [15] = { it = "CODICE: 7-15-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 7-15-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [16] = { it = "CODICE: 7-16-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 7-16-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [17] = { it = "CODICE: 7-17-CIN\nL'USS Los Angeles si è incagliato ed è andato distrutto.", en = "CODE: 7-17-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED." },
    [18] = { it = "CODICE: 7-18-CIN\nL'USS Los Angeles è stato distrutto da siluri.", en = "CODE: 7-18-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE." },
    [19] = { it = "CODICE: 7-19-CIN\nLa distruzione dell'unità di superficie è una pessima notizia. Accidenti, sono certo che lei ha fatto del suo meglio per garantire la sicurezza del suo battello. Diciamolo: sono bersagli grandi e difficili da mancare. Al suo rientro in porto vorrei ascoltare le registrazioni della centrale di comando. Sono certo che parleranno da sole e rafforzeranno la mia fiducia nelle sue capacità. Rientri presto: stiamo cercando di tenere nascosto l'episodio. Cerchi di non rimuginarci troppo, Comandante: non voglio che influisca sul suo rendimento.\nCODICE: 7-20-CIN\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di identificare i suoi bersagli prima di aprire il fuoco! Rientri immediatamente in porto.", en = "CODE: 7-19-CIN\nTHE DESTRUCTION OF THE SURFACE SHIP IS TERRIBLE NEWS. SHUCKS, I'M SURE YOU DID YOUR BEST TO ENSURE THE SAFETY OF YOUR SHIP. LET'S FACE IT, THEY'RE BIG TARGETS, AND HARD TO MISS. WHEN YOU RETURN TO PORT, I'D LIKE TO LISTEN TO THE RECORDINGS FROM THE CONN. I'M SURE THAT THEY'LL SPEAK FOR THEMSELVES AND REINFORCE MY CONFIDENCE IN YOUR ABILITIES. RETURN TO PORT QUICKLY, WE ARE TRYING TO KEEP A LID ON THIS EPISODE. TRY NOT TO DWELL ON THIS TOO MUCH CAPTAIN, I DON'T WANT IT AFFECTING YOUR PERFORMANCE.\nCODE: 7-20-CIN\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE IDENTITY OF HIS TARGETS BEFORE FIRING! RETURN TO PORT IMMEDIATELY." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 4, punti = 22, spostamento_max_m = 13196.0064, non_risolte = 0 },
  script = script,
}

script.L_0133 = function(u)
  ::L_0133::
  m688.nuovo_caso(u)
  m688.posizione(u, -118823.1876, -63050.3474)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_01c0 end
  m688.posizione(u, -122814.7517, -65331.1007)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_01c0 end
  m688.posizione(u, -119622.6699, -70881.9384)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_01c0 end
  m688.posizione(u, -115615.3119, -67814.9085)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_01c0 end
  m688.posizione(u, -115738.3869, -74874.0367)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_01c0 end
  m688.posizione(u, -109069.2918, -70279.98)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_01c0 end
  m688.posizione(u, -108500.2921, -75781.306)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_01c0 end
  m688.posizione(u, -108577.1453, -80487.399)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_01c0 end
  m688.posizione(u, -102615.3712, -79013.3961)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_01c0 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_01c0 end
  m688.posizione(u, -100565.3712, -74337.6152)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_01c0 end
  m688.posizione(u, -99153.3156, -68082.5834)
  m688.rotta_ora(u, 300)
  ::L_01c0::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0133")
  do return end
end

script.L_01ce = function(u)
  ::L_01ce::
  if m688.in_rettangolo(u, -197194.0226, 1623.198, -14990.1715, -126855.1211) then goto L_01e2 end
  if m688.giocatore_secondo(u) then goto L_01df end
  m688.messaggio(u, 110)
  do return end
  ::L_01df::
  m688.messaggio(u, 109)
  do return end
  ::L_01e2::
  m688.dopo(u, 20, "L_01ce")
  do return end
end

script.L_01e8 = function(u)
  ::L_01e8::
  if m688.in_rettangolo(u, -197194.0226, 1623.198, -14990.1715, -126855.1211) then goto L_01fc end
  if m688.giocatore_secondo(u) then goto L_01f9 end
  m688.messaggio(u, 109)
  do return end
  ::L_01f9::
  m688.messaggio(u, 110)
  do return end
  ::L_01fc::
  if m688.unita_esiste(u, 2) then goto L_020a end
  if m688.giocatore_secondo(u) then goto L_0207 end
  m688.messaggio(u, 106)
  do return end
  ::L_0207::
  m688.messaggio(u, 108)
  do return end
  ::L_020a::
  m688.dopo(u, 30, "L_01e8")
  do return end
end

script.L_0210 = function(u)
  ::L_0210::
  m688.vai_a(u, -117423.6512, -58367.7707, 40, "L_021b")
  m688.salva_modello(u)
  do return end
end

script.L_021b = function(u)
  ::L_021b::
  m688.vai_a(u, -118100, -85700, 40, "L_0226")
  m688.salva_modello(u)
  do return end
end

script.L_0226 = function(u)
  ::L_0226::
  m688.vai_a(u, -112628.351, -85911.667, 40, "L_0231")
  m688.salva_modello(u)
  do return end
end

script.L_0231 = function(u)
  ::L_0231::
  m688.vai_a(u, -112030.2968, -50615.6593, 40, "L_023c")
  m688.salva_modello(u)
  do return end
end

script.L_023c = function(u)
  ::L_023c::
  m688.vai_a(u, -106776.214, -50703.0065, 40, "L_0247")
  m688.salva_modello(u)
  do return end
end

script.L_0247 = function(u)
  ::L_0247::
  m688.vai_a(u, -107320.9307, -84430.6308, 40, "L_0252")
  m688.salva_modello(u)
  do return end
end

script.L_0252 = function(u)
  ::L_0252::
  m688.vai_a(u, -102300, -84500, 40, "L_025d")
  m688.salva_modello(u)
  do return end
end

script.L_025d = function(u)
  ::L_025d::
  m688.vai_a(u, -101449.6453, -46079.7994, 40, "L_0268")
  m688.salva_modello(u)
  do return end
end

script.L_0268 = function(u)
  ::L_0268::
  m688.vai_a(u, -96199.1783, -46158.7196, 40, "L_0273")
  m688.salva_modello(u)
  do return end
end

script.L_0273 = function(u)
  ::L_0273::
  m688.vai_a(u, -95700, -85500, 40, "L_027e")
  m688.salva_modello(u)
  do return end
end

script.L_027e = function(u)
  ::L_027e::
  m688.vai_a(u, -93300, -86100, 40, "L_0289")
  m688.salva_modello(u)
  do return end
end

script.L_0289 = function(u)
  ::L_0289::
  m688.vai_a(u, -91983.4036, -121532.1779, 40, "L_0294")
  m688.salva_modello(u)
  do return end
end

script.L_0294 = function(u)
  ::L_0294::
  m688.vai_a(u, -85657.2645, -43166.3193, 40, "L_029f")
  m688.salva_modello(u)
  do return end
end

script.L_029f = function(u)
  ::L_029f::
  m688.vai_a(u, -84700, -80300, 40, "L_02aa")
  m688.salva_modello(u)
  do return end
end

script.L_02aa = function(u)
  ::L_02aa::
  m688.vai_a(u, -80189.1732, -79323.068, 40, "L_02b5")
  m688.salva_modello(u)
  do return end
end

script.L_02b5 = function(u)
  ::L_02b5::
  m688.vai_a(u, -79753.1027, -43240.6344, 40, "L_0210")
  m688.salva_modello(u)
  do return end
end

script.L_02c0 = function(u)
  ::L_02c0::
  m688.vai_a(u, -110203.8526, -59277.0255, 30, "L_02d0")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_02d0 = function(u)
  ::L_02d0::
  m688.vai_a(u, -110256.1204, -62414.4949, 30, "L_02e0")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_02e0 = function(u)
  ::L_02e0::
  m688.vai_a(u, -111598.4164, -63961.096, 30, "L_02f0")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_02f0 = function(u)
  ::L_02f0::
  m688.vai_a(u, -112954.6985, -66291.7817, 30, "L_0300")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0300 = function(u)
  ::L_0300::
  m688.vai_a(u, -112968.0688, -67076.1384, 30, "L_0310")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0310 = function(u)
  ::L_0310::
  m688.vai_a(u, -111677.7064, -68667.2498, 30, "L_0320")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0320 = function(u)
  ::L_0320::
  m688.vai_a(u, -109715.0261, -69484.7163, 30, "L_0330")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0330 = function(u)
  ::L_0330::
  m688.vai_a(u, -108423.3994, -71075.1787, 30, "L_0340")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0340 = function(u)
  ::L_0340::
  m688.vai_a(u, -105813.2429, -72686.6078, 30, "L_0350")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0350 = function(u)
  ::L_0350::
  m688.vai_a(u, -104544.7292, -75845.0056, 30, "L_0360")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0360 = function(u)
  ::L_0360::
  m688.vai_a(u, -101900, -80300, 30, "L_0370")
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0400")
  do return end
end

script.L_0370 = function(u)
  ::L_0370::
  if m688.unita_id(u, 2) then goto L_0376 end
  do return end
  ::L_0376::
  if m688.giocatore_secondo(u) then goto L_037c end
  m688.messaggio(u, 105)
  do return end
  ::L_037c::
  m688.messaggio(u, 107)
  do return end
end

script.L_037f = function(u)
  ::L_037f::
  if m688.bersaglio_id(u, 0) then goto L_038c end
  if m688.bersaglio_arma(u) then goto L_03c0 end
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_038c::
  if m688.traccia_solida(u) then goto L_0397 end
  m688.ingaggia(u, 30, "L_038c", "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_0397::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_038c", "L_037f")
  if m688.bersaglio_vicino(u, 7.6) then goto L_03a7 end
  ::L_03a5::
  m688.salva_modello(u)
  do return end
  ::L_03a7::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03a5 end
  if m688.flag_scena_uguale(u, 1) then goto L_03a5 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_03bc")
  goto L_0501
  ::L_03c0::
  if m688.bersaglio_mi_punta(u) then goto L_03c8 end
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_03c8::
  m688.ingaggia(u, 10, "L_03c0", "L_03c3")
  if m688.traccia_solida(u) then goto L_03d5 end
  goto L_03ea
  ::L_03d5::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03dd end
  goto L_03ea
  ::L_03dd::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_03ea::
  m688.salva_modello(u)
  do return end
  ::L_0501::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_05d1", "L_05ae")
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
  m688.dopo(u, 150, "L_0568")
  m688.dopo(u, 170, "L_0591")
  m688.dopo(u, 240, "L_05d1")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_038c = function(u)
  ::L_038c::
  if m688.traccia_solida(u) then goto L_0397 end
  m688.ingaggia(u, 30, "L_038c", "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_0397::
  m688.telegrafo(u, 1.0547)
  m688.ingaggia(u, 30, "L_038c", "L_037f")
  if m688.bersaglio_vicino(u, 7.6) then goto L_03a7 end
  ::L_03a5::
  m688.salva_modello(u)
  do return end
  ::L_03a7::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03a5 end
  if m688.flag_scena_uguale(u, 1) then goto L_03a5 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_03bc")
  goto L_0501
  ::L_0501::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_05d1", "L_05ae")
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
  m688.dopo(u, 150, "L_0568")
  m688.dopo(u, 170, "L_0591")
  m688.dopo(u, 240, "L_05d1")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_03bc = function(u)
  ::L_03bc::
  m688.flag_scena(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_03c0 = function(u)
  ::L_03c0::
  if m688.bersaglio_mi_punta(u) then goto L_03c8 end
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_03c8::
  m688.ingaggia(u, 10, "L_03c0", "L_03c3")
  if m688.traccia_solida(u) then goto L_03d5 end
  goto L_03ea
  ::L_03d5::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03dd end
  goto L_03ea
  ::L_03dd::
  m688.quota(u, 9.144, 0.9961)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
  ::L_03ea::
  m688.salva_modello(u)
  do return end
end

script.L_03c3 = function(u)
  ::L_03c3::
  m688.comportamento(u, "L_037f")
  m688.salva_modello(u)
  do return end
end

script.L_0400 = function(u)
  ::L_0400::
  m688.telegrafo(u, 0.0078)
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_040a")
  do return end
end

script.L_040a = function(u)
  ::L_040a::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  if m688.unita_id(u, 2) then goto L_0418 end
  m688.quota(u, 152.4, 0.5859)
  ::L_0418::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_041f")
  do return end
end

script.L_041f = function(u)
  ::L_041f::
  m688.virata_relativa(u, -179.96)
  if m688.unita_id(u, 2) then goto L_042c end
  m688.quota(u, 60.96, 0.5859)
  ::L_042c::
  m688.salva_modello(u)
  m688.dopo(u, 70, "L_0433")
  do return end
end

script.L_0433 = function(u)
  ::L_0433::
  m688.riprendi_navigazione(u)
  m688.quota(u, 289.56, 0.5859)
  m688.telegrafo(u, 0.4688)
  m688.salva_modello(u)
  do return end
end

script.L_0478 = function(u)
  ::L_0478::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_04f4 = function(u)
  ::L_04f4::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_04fe")
  do return end
end

script.L_04fe = function(u)
  ::L_04fe::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0544 = function(u)
  ::L_0544::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_05d1", "L_05ae")
  script.L_056b(u)
  m688.dopo(u, 170, "L_0568")
  m688.dopo(u, 190, "L_0591")
  m688.dopo(u, 280, "L_05d1")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0568 = function(u)
  ::L_0568::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_056b = function(u)
  ::L_056b::
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

script.L_0591 = function(u)
  ::L_0591::
  m688.annulla_timer(u, "L_0591")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_05ae = function(u)
  ::L_05ae::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_059c")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_05ca")
  m688.dopo(u, 10, "L_05c2")
  do return end
end

script.L_059c = function(u)
  ::L_059c::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_05c2")
  m688.annulla_timer(u, "L_05ae")
  m688.insegui(u, 0, "L_05d1", "L_05ae")
  m688.salva_modello(u)
  do return end
end

script.L_05c2 = function(u)
  ::L_05c2::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_05ca = function(u)
  ::L_05ca::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_05ca")
  do return end
end

script.L_05d1 = function(u)
  ::L_05d1::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_05d5 = function(u)
  ::L_05d5::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_05d1", "L_05ae")
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
  m688.dopo(u, 2, "L_0602")
  m688.dopo(u, 60, "L_0607")
  do return end
end

script.L_0602 = function(u)
  ::L_0602::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0607 = function(u)
  ::L_0607::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_060b = function(u)
  ::L_060b::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0610 = function(u)
  ::L_0610::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_05ae")
  do return end
end

script.L_061a = function(u, p)
  ::L_061a::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 20, "L_0629")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0629 = function(u)
  ::L_0629::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_059c")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0637 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0637::
  do return end
end

script.L_0638 = function(u, p)
  ::L_0638::
  m688.vai_a(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468), 30, "L_0643")
  m688.salva_modello(u)
  do return end
end

script.L_0643 = function(u)
  ::L_0643::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_064a = function(u, p)
  ::L_064a::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_064f = function(u, p)
  ::L_064f::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0656 = function(u, p)
  ::L_0656::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_065e = function(u)
  ::L_065e::
  m688.annulla_timer(u, "L_069d")
  m688.dopo(u, 20, "L_069d")
  do return end
end

script.L_0667 = function(u)
  ::L_0667::
  m688.annulla_timer(u, "L_06a0")
  m688.dopo(u, 20, "L_06a0")
  do return end
end

script.L_0670 = function(u)
  ::L_0670::
  m688.annulla_timer(u, "L_06a3")
  m688.dopo(u, 20, "L_06a3")
  do return end
end

script.L_0679 = function(u)
  ::L_0679::
  m688.annulla_timer(u, "L_06a6")
  m688.dopo(u, 20, "L_06a6")
  do return end
end

script.L_0682 = function(u)
  ::L_0682::
  m688.annulla_timer(u, "L_06a9")
  m688.dopo(u, 20, "L_06a9")
  do return end
end

script.L_068b = function(u)
  ::L_068b::
  m688.annulla_timer(u, "L_06ac")
  m688.dopo(u, 20, "L_06ac")
  do return end
end

script.L_0694 = function(u)
  ::L_0694::
  m688.annulla_timer(u, "L_06af")
  m688.dopo(u, 20, "L_06af")
  do return end
end

script.L_069d = function(u)
  ::L_069d::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_06a0 = function(u)
  ::L_06a0::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_06a3 = function(u)
  ::L_06a3::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_06a6 = function(u)
  ::L_06a6::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_06a9 = function(u)
  ::L_06a9::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_06ac = function(u)
  ::L_06ac::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_06af = function(u)
  ::L_06af::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_06b2 = function(u)
  ::L_06b2::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_06ba = function(u, p)
  ::L_06ba::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06be = function(u)
  ::L_06be::
  m688.dopo(u, 60, "L_06c8")
  do return end
end

script.L_06c4 = function(u, p)
  ::L_06c4::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06c8 = function(u)
  ::L_06c8::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_06ce = function(u)
  ::L_06ce::
  m688.dopo(u, 40, "L_06d4")
  do return end
end

script.L_06d4 = function(u)
  ::L_06d4::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_06da = function(u)
  ::L_06da::
  m688.annulla_timer(u, "L_06c8")
  m688.annulla_timer(u, "L_06d4")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_06e4 = function(u, p)
  ::L_06e4::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06e9 = function(u, p)
  ::L_06e9::
  m688.posizione(u, par(p, 1, -209717.6327), par(p, 2, 17716.0468))
  m688.salva_modello(u)
  do return end
end

script.L_06f0 = function(u)
  ::L_06f0::
  if m688.giocatore_secondo(u) then goto L_06f6 end
  m688.messaggio(u, 25)
  do return end
  ::L_06f6::
  m688.messaggio(u, 29)
  do return end
end

script.L_06f9 = function(u)
  ::L_06f9::
  if m688.giocatore_secondo(u) then goto L_06ff end
  m688.messaggio(u, 29)
  do return end
  ::L_06ff::
  m688.messaggio(u, 25)
  do return end
end

return missione
