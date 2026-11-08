-- SCENE5C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE5C",
  missione = 5,
  titolo = { it = "Fuga", en = "ESCAPE" },
  lato = "due_giocatori",
  teatro = "gibilterra",
  posa = { metodo = "georef", centro = { lat = 35.7655, lon = -5.8445 }, metri_per_unita = { x = 22.969, y = 27.780 }, moto_classico_m_s_per_nodo = 1.2196, affidabilita = "alta", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 60.96, 121.92, 182.88 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_02a0" },
    harpoon = { gestore = "L_024e" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_033e" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_03a1",
    telegrafo = "L_03b3",
    quota = "L_03b8",
    timone = "L_03bf",
    pilota_automatico_off = "L_041b",
    periscopio = "L_0423",
    carica_tubo_1 = "L_03c7",
    carica_tubo_2 = "L_03d0",
    carica_tubo_3 = "L_03d9",
    carica_tubo_4 = "L_03e2",
    carica_tubo_5 = "L_03eb",
    carica_tubo_6 = "L_03f4",
    carica_tubo_7 = "L_03fd",
    schiera_cala = "L_0427",
    schiera_recupera = "L_0437",
    schiera_persa = "L_0443",
    designa_bersaglio = "L_044d",
    posizione = "L_0452",
    antenna = "L_042d",
    guida_e0 = "L_0374",
    guida_e1 = "L_0379",
    guida_e2 = "L_0383",
    f0 = "L_0459",
    f1 = "L_0462",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.169281005859, probabilita_originale = "5547/32768", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.16731262207, probabilita_originale = "10965/65536", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.166656494141, probabilita_originale = "5461/32768", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.16471862793, probabilita_originale = "10795/65536", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0846405029297, probabilita_originale = "5547/65536", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0833282470703, probabilita_originale = "5461/65536", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0826721191406, probabilita_originale = "2709/32768", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0813903808594, probabilita_originale = "2667/32768", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0130", reazione = nil, arrivo = nil } },
      { id = 2, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0125" } },
      },
      timer = { { dopo_s = 90, unita = 0, gestore = "L_0175" }, { dopo_s = 60, unita = 1, gestore = "L_015b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, poco dopo la sua partenza dal porto in Italia, due sottomarini sovietici di ritorno da pattugliamenti di routine hanno cambiato improvvisamente rotta. Sembra che abbiano virato a sud e siano diretti verso lo Stretto di Gibilterra. Riteniamo che questi battelli possano aver ricevuto l'ordine di seguirla fuori dal Mediterraneo e nell'Atlantico. Mantenendo la rotta attuale, i sottomarini sovietici la intercetteranno all'uscita dello stretto.", "Comandante, conto su di lei per superare i sovietici e sganciarsi verso ovest senza fare il minimo rumore. So che il suo equipaggio è stanco dopo il turno nel Mediterraneo, ma ultimamente ho avuto più orsi del dovuto a fiutare attorno ai miei battelli! Tenga duro: probabilmente potrà rilassarsi una volta in mare aperto. Credo che i ragazzi sovietici rinunceranno quando raggiungerà l'Atlantico, a meno che non le stiano ben attaccati in coda all'uscita. Non vedo la necessità di provare le armi, a meno che non siano loro a sparare per primi. Se ce la fa, la prossima settimana ne parleremo a cena al circolo ufficiali di Groton.", "VICEAMMIRAGLIO J.W. RATCLIFF III", "Compagni, informazioni dell'intelligence rivelano che un sottomarino d'attacco americano classe 688 ha appena lasciato Napoli ed è diretto verso lo Stretto di Gibilterra. L'occasione di raccogliere dati sul 688 non può essere lasciata al vento! Il Leninskii Komsomol intercetti gli americani all'uscita dal Mediterraneo. Se gli americani vengono scoperti, mantenga il contatto sonar con il 688 il più a lungo possibile e registri le informazioni con il suo registratore giapponese. Deve restare entro la portata di tiro, e può lanciare armi solo nel caso in cui gli imperialisti si spaventino e agiscano per primi." },
    en = { "CAPTAIN, SHORTLY AFTER YOU LEFT PORT IN ITALY, TWO SOVIET SUBS RETURNING HOME FROM ROUTINE PATROLS MADE SUDDEN CHANGES IN COURSE. THEY APPEAR TO HAVE TURNED SOUTH AND ARE HEADING TOWARD THE STRAIT OF GIBRALTAR. WE BELIEVE THESE BOATS MAY HAVE BEEN INSTRUCTED TO FOLLOW THE YOU OUT OF THE MED AND INTO THE ATLANTIC. FOLLOWING THEIR PRESENT COURSE, THE SOVIET SUBS WILL INTERCEPT YOU AS YOU EXIT THE STRAIT.", "CAPTAIN, I'M COUNTING ON YOU TO GET PAST THE SOVIETS AND ESCAPE TO THE WEST WITHOUT SO MUCH AS A WHISPER. I KNOW YOUR CREW IS FATIGUED AFTER YOUR TOUR IN THE MED, BUT LATELY I'VE HAD MORE THAN MY SHARE OF BEARS SNIFFING AROUND MY BOATS! TOUGH IT OUT AND YOU CAN PROBABLY RELAX ONCE YOU'RE IN OPEN WATERS. I THINK THE SOVIET BOYS WILL BREAK OFF WHEN YOU REACH THE ATLANTIC, UNLESS THEY HAVE A CLEAN TAIL ON YOU AS YOU EXIT. I DON'T SEE ANY NEED FOR WEAPON TESTING UNLESS THEY DRAW FIRST. GET THROUGH THIS AND WE'LL DISCUSS YOUR ADVENTURES AT THE O CLUB IN GROTON NEXT WEEK OVER DINNER.", "VADM J.W. RATCLIFF III", "COMRADES, INTELLIGENCE INFORMATION REVEALS THAT AN AMERICAN 688 ATTACK SUBMARINE HAS JUST LEFT NAPLES AND IS HEADING FOR THE STRAIT OF GIBRALTAR. THE OPPORTUNITY TO GATHER DATA ON THE 688 CANNOT BE SET FREE WITH THE WIND! THE LENINSKII KOMSOMOL IS DIRECTED TO INTERCEPT THE AMERICANS AS THEY EXIT THE MEDITERRANEAN. IF THE AMERICANS ARE DISCOVERED, MAINTAIN SONAR CONTACT WITH THE 688 AS LONG AS POSSIBLE AND TAPE THE INFORMATION ON YOUR JAPANESE SOUND RECORDING DEVICE. YOU MUST STAY WITHIN FIRING RANGE, AND YOU MAY LAUNCH WEAPONS ONLY IN THE EVENT THAT THE IMPERIALISTS GROW FRIGHTENED AND TAKE ACTION FIRST." },
  },
  esiti = {
    [1] = { it = "CODICE: 5-1-CIN\nIeri l'USS Los Angeles si è incagliato nei pressi dello Stretto di Gibilterra ed è andato distrutto. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 5-1-CIN\nTHE USS LOS ANGELES RAN AGROUND NEAR THE STRAIT OF GIBRALTAR AND WAS DESTROYED YESTERDAY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 5-2-CIN\nIeri l'USS Los Angeles è stato distrutto da siluri nemici. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 5-2-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY ENEMY TORPEDO FIRE YESTERDAY. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 5-3-CIN\nSembra che uno dei nostri amici sovietici non ce l'abbia fatta. Il SOSUS indica di aver rilevato un sottomarino che ha urtato il fondo ed è imploso. Vorrei sentire il suo racconto dell'incidente. Mi chiami quando arriva in porto.", en = "CODE: 5-3-CIN\nIT LOOKS LIKE ONE OF OUR SOVIET FRIENDS DIDN'T MAKE IT. SOSUS INDICATED THAT IT DETECTED A SUB HIT BOTTOM AND IMPLODE. I WOULD LIKE TO HEAR YOUR DESCRIPTION OF THE INCIDENT. GIVE ME A CALL WHEN YOU GET INTO PORT." },
    [4] = { it = "CODICE: 5-4-CIN\nAccidenti, quei ragazzi sovietici ci hanno davvero tenuti col fiato sospeso per un po'. Se ne è uscito vivo, è merito del suo eccellente addestramento; loro non sono stati altrettanto fortunati. Al suo rientro voglio che faccia un resoconto allo stato maggiore dell'ammiraglio: sono tutti ansiosi di sentire i dettagli della sua avventura. Mi chiami quando arriva in porto.", en = "CODE: 5-4-CIN\nBOY, THOSE SOVIET BOYS REALLY HAD US GOING FOR A WHILE. IT'S A TESTIMONY TO YOUR EXCELLENT TRAINING THAT YOU CAME OUT OF THIS ONE ALIVE. THEY WERE NOT AS FORTUNATE. I INTEND TO HAVE YOU BRIEF THE ADMIRAL'S STAFF WHEN YOU RETURN. THEY'RE ALL ANXIOUS TO HEAR THE DETAILS OF YOUR EPISODE. GIVE ME A CALL WHEN YOU GET INTO PORT." },
    [5] = { it = "CODICE: 5-5-CIN\nNon essere riuscito a seguire gli americani rappresenta una perdita di informazioni insostituibile per la Marina sovietica e comporterà severi provvedimenti disciplinari! Forse il Leninskii Komsomol ha bisogno di un cambio al comando? Rientri subito in porto, così potremo liberarci della zavorra in eccesso: peso morto che non rappresenta un successo per il Soviet Supremo!", en = "CODE: 5-5-CIN\nYOUR FAILURE TO TRACK THE AMERICANS REPRESENTS AN IRREPLACEABLE LOSS OF INFORMATION TO THE SOVIET NAVY AND WILL RESULT IN STRONG DISCIPLINARY MEASURES! PERHAPS THE LENINSKII KOMSOMOL NEEDS A CHANGE IN LEADERSHIP? RETURN TO PORT AT ONCE SO THAT WE CAN REMOVE THE EXCESS BALLAST: DEAD WEIGHT WHICH DOES NOT REPRESENT SUCCESS TO THE SUPREME SOVIET!" },
    [6] = { it = "CODICE: 5-6-CIN\nCompagni, ancora una volta sono soddisfatto del vostro eccellente lavoro. Il sottomarino americano non poteva sfuggire alla presa della superiore tecnologia sovietica. I dati che avete raccolto aiuteranno la vostra Marina e il vostro Stato! Il vostro Paese non dimenticherà la prestazione esemplare e il sacrificio di questo equipaggio!", en = "CODE: 5-6-CIN\nCOMRADES, ONCE AGAIN I AM PLEASED BY YOUR EXCELLENT WORK. THE AMERICAN SUBMARINE COULD NOT ESCAPE THE GRASP OF SUPERIOR SOVIET TECHNOLOGY. THE DATA YOU COLLECTED WILL HELP YOUR NAVY AND YOUR STATE! YOUR COUNTRY SHALL NOT FORGET THIS CREW'S DUTIFUL PERFORMANCE AND SACRIFICE IN THIS SITUATION!" },
    [7] = { it = "CODICE: 5-7-CIN\nLa partita è terminata. Il suo avversario ha lasciato l'area di gioco.", en = "CODE: 5-7-CIN\nTHE GAME HAS COME TO AN END. YOUR OPPONENT LEFT THE PLAYFIELD." },
    [8] = { it = "CODICE: 5-8-CIN\nLa partita è terminata. Lei ha lasciato l'area di gioco.", en = "CODE: 5-8-CIN\nTHE GAME HAS COME TO AN END. YOU LEFT THE PLAYFIELD." },
    [9] = { it = "CODICE: 5-9-CIN\nComandante, mi congratulo per il lavoro ben fatto. Ha lasciato i nostri amici sovietici a cercarla freneticamente su e giù per la costa. Può scommettere che i loro alti papaveri andranno su tutte le furie quando scopriranno che se l'è cavata senza un graffio. Questi sono i risultati che mi piacciono: niente errori, niente sorprese. Continui così!", en = "CODE: 5-9-CIN\nCAPTAIN, LET ME CONGRATULATE YOU ON A JOB WELL DONE. YOU LEFT OUR SOVIET FRIENDS IN A FRANTIC SEARCH, UP AND DOWN THE BEACH. YOU CAN BET THEIR BRASS WILL BE IN AN UPROAR WHEN THEY FIND OUT THAT YOU GOT AWAY SCOTT FREE. THESE ARE THE KIND OF RESULTS I LIKE: NO MISTAKES, NO SURPRISES. KEEP IT UP!" },
    [10] = { it = "CODICE: 5-10-CIN\nComandante, perché non emerge e non offre ai sovietici una visita guidata del suo battello? Ha regalato una quantità incalcolabile di dati estremamente riservati sul 688. Considero questa una gestione completamente sbagliata di un'operazione quasi di routine. Ora devo richiamare altre unità dal pattugliamento perché le tolgano di torno i suoi orsi. Quando rientrerà in porto, esamineremo insieme ciascuno dei suoi errori, e anche la sua carriera!", en = "CODE: 5-10-CIN\nCAPTAIN, HOW ABOUT SURFACING AND GIVING THE SOVIETS A WALKING TOUR OF YOUR SHIP? YOU'VE GIVEN AWAY AN IMMEASURABLE AMOUNT OF EXTREMELY SENSITIVE DATA CONCERNING THE 688. I CONSIDER THIS A COMPLETE MISHANDLING OF A SEMI-ROUTINE OPERATION. NOW I HAVE TO CALL IN OTHER SHIPS OFF PATROL TO SPOOK YOUR BEARS AWAY FOR YOU. WHEN YOU PULL INTO PORT, THE TWO OF US WILL REVIEW EACH OF YOUR ERRORS, AS WELL AS YOUR CAREER!" },
    [11] = { it = "CODICE: 5-11-CIN\nComandante, quello è stato il più misero tentativo di evasione che abbia mai visto! Immagino che l'unica buona notizia sia che lui era fuori portata di tiro. Rientri subito in porto, dove seguirò personalmente il suo corso di recupero di tattica subacquea.", en = "CODE: 5-11-CIN\nCAPTAIN, THAT WAS THE WEAKEST EXCUSE FOR AN ATTEMPT AT EVASION THAT I'VE SEEN! I GUESS THE ONLY GOOD NEWS WAS THAT HE WAS OUT OF FIRING RANGE. RETURN TO PORT AT ONCE, WHERE I CAN PERSONALLY OVERSEE YOUR REMEDIAL EDUCATION IN SUBMARINE TACTICS." },
    [12] = { it = "CODICE: 5-12-CIN\nKapitan, lei non è riuscito a mantenere una posizione di tiro sul sottomarino americano. Aspettava forse che gli americani accostassero per invitarla a bordo a vodka e caviale? Lei ha ostacolato la lotta del socialismo e gonfiato l'ego imperialista lasciandosi sfuggire quei porci. Rientri in porto, e in fretta: mi divertirò a stabilire la sua punizione.", en = "CODE: 5-12-CIN\nKAPITAN, YOU FAILED TO MAINTAIN A FIRING POSITION ON THE AMERICAN SUB. WERE YOU WAITING FOR THE AMERICANS TO PULL OVER AND INVITE YOU IN FOR VODKA AND CAVIAR? YOU HAVE HINDERED THE FIGHT OF SOCIALISM AND INFLATED THE IMPERIALIST EGO BY ALLOWING THE SWINE TO SLIP FROM YOUR GRASP. RETURN TO PORT QUICKLY, I WILL ENJOY PRESCRIBING YOUR PUNISHMENT." },
    [15] = { it = "CODICE: 5-15-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare durante un pattugliamento. Piangiamo la perdita di un equipaggio coraggioso, appassionatamente devoto agli ideali del progresso e del socialismo! Non saranno dimenticati, né considerati deboli o stupidi.", en = "CODE: 5-15-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE ON PATROL. WE MOURN THE LOSS OF A BRAVE CREW, PASSIONATELY COMMITTED TO THE IDEALS OF PROGRESS AND SOCIALISM! THEY WILL NOT BE FORGOTTEN, OR THOUGHT OF AS WEAK OR STUPID." },
    [16] = { it = "CODICE: 5-16-CIN\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento. Il mostro capitalista è certamente responsabile della loro morte! Piangiamo tutti la perdita di un equipaggio coraggioso, appassionatamente devoto agli ideali del progresso e del socialismo! Non saranno dimenticati, né considerati deboli o stupidi.", en = "CODE: 5-16-CIN\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE ON PATROL. THE CAPITALIST MONSTER IS SURELY RESPONSIBLE FOR THEIR DEATHS! LET US ALL MOURN THE LOSS OF A BRAVE CREW PASSIONATELY COMMITTED TO THE IDEALS OF PROGRESS AND SOCIALISM! THEY WILL NOT BE FORGOTTEN OR THOUGHT OF AS WEAK OR STUPID." },
    [17] = { it = "CODICE: 5-17-CIN\nCompagni, è un peccato che gli agenti capitalisti abbiano preferito affondare piuttosto che farsi seguire, ma sono comunque soddisfatto dei vostri sforzi. Il sottomarino americano non poteva sfuggire alla presa della superiore tecnologia sovietica. I dati che siete riusciti a raccogliere aiuteranno la vostra Marina e il vostro Stato! Il vostro Paese non dimenticherà la prestazione esemplare e il sacrificio di questo equipaggio!", en = "CODE: 5-17-CIN\nCOMRADES, IT IS UNFORTUNATE THAT THE CAPITALIST AGENTS CHOSE TO SINK THEMSELVES RATHER THAN BE FOLLOWED, BUT I AM PLEASED BY YOUR EFFORTS NONETHELESS. THE AMERICAN SUBMARINE COULD NOT ESCAPE THE GRASP OF SUPERIOR SOVIET TECHNOLOGY. THE DATA YOU DID MANAGE TO COLLECTED WILL HELP YOUR NAVY AND YOUR STATE! YOUR COUNTRY SHALL NOT FORGET THIS CREW'S DUTIFUL PERFORMANCE AND SACRIFICE IN THIS SITUATION!" },
    [18] = { it = "CODICE: 5-18-CIN\nCompagni, è un peccato che gli agenti capitalisti abbiano scelto la morte piuttosto che fornirci informazioni, ma sono comunque soddisfatto dei vostri sforzi. Il sottomarino americano non poteva sfuggire alla presa della superiore tecnologia sovietica, né alla velocità di un siluro da 533 mm. Il vostro Paese non dimenticherà la prestazione esemplare e il sacrificio di questo equipaggio!", en = "CODE: 5-18-CIN\nCOMRADES, IT IS UNFORTUNATE THAT THE CAPITALIST AGENTS CHOSE THEIR DEATH RATHER THAN PROVIDE US WITH INFORMATION, BUT I AM PLEASED BY YOUR EFFORTS NONETHELESS. THE AMERICAN SUBMARINE COULD NOT ESCAPE THE GRASP OF SUPERIOR SOVIET TECHNOLOGY NOR THE SPEED OF A 533MM TORPEDO. YOUR COUNTRY SHALL NOT FORGET THIS CREW'S DUTIFUL PERFORMANCE AND SACRIFICE IN THIS SITUATION!" },
    [19] = { it = "CODICE: 5-19-CIN\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!\nCODICE: 5-20-CIN\nTraditori! Agenti del fascismo! Nemici del popolo! Così le madri di questo equipaggio chiamano i loro figli! Uomini che hanno ucciso marinai innocenti e mandato in fondo al mare mezzi civili. Il Leninskii Komsomol emerga immediatamente. All'intero equipaggio è ordinato di consegnare le proprie misere vite e di salire a bordo della Kiev. A quel punto un equipaggio sostitutivo assumerà il comando del Leninskii Komsomol.", en = "CODE: 5-19-CIN\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!\nCODE: 5-20-CIN\nTRAITORS! AGENTS OF FASCISM! ENEMIES OF THE PEOPLE! THESE ARE THE NAMES THE MOTHERS OF THIS CREW USE TO DESCRIBE THEIR SONS! MEN THAT HAVE KILLED INNOCENT SAILORS AND SENT CIVILIAN EQUIPMENT TO THE BOTTOM OF THE SEA. THE LENINSKII KOMSOMOL IS INSTRUCTED TO SURFACE IMMEDIATELY. THE ENTIRE CREW IS ORDERED TO SURRENDER THEIR MISERABLE LIVES OVER AND BOARD THE KIEV. AT THAT TIME A REPLACEMENT CREW WILL TAKE COMMAND OF THE LENINSKII KOMSOMOL." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 4, punti = 0, spostamento_max_m = 468.4496, non_risolte = 0 },
  script = script,
}

script.L_0125 = function(u)
  ::L_0125::
  m688.vai_a(u, 8608.275, 7174.8502, 30, "L_01ce")
  m688.salva_modello(u)
  do return end
end

script.L_0130 = function(u)
  ::L_0130::
  if m688.bersaglio_id(u, 0) then goto L_013f end
  m688.variabile(u, 2, 0)
  m688.comportamento(u, "L_0130")
  m688.salva_modello(u)
  do return end
  ::L_013f::
  m688.ingaggia(u, 30, "L_013f", "L_0135")
  m688.salva_modello(u)
  m688.variabile(u, 2, 1)
  if m688.traccia_solida(u) then goto L_014f end
  do return end
  ::L_014f::
  if m688.bersaglio_vicino(u, 5) then goto L_0155 end
  do return end
  ::L_0155::
  m688.variabile(u, 2, 2)
  do return end
end

script.L_0135 = function(u)
  ::L_0135::
  m688.variabile(u, 2, 0)
  m688.comportamento(u, "L_0130")
  m688.salva_modello(u)
  do return end
end

script.L_013f = function(u)
  ::L_013f::
  m688.ingaggia(u, 30, "L_013f", "L_0135")
  m688.salva_modello(u)
  m688.variabile(u, 2, 1)
  if m688.traccia_solida(u) then goto L_014f end
  do return end
  ::L_014f::
  if m688.bersaglio_vicino(u, 5) then goto L_0155 end
  do return end
  ::L_0155::
  m688.variabile(u, 2, 2)
  do return end
end

script.L_015b = function(u)
  ::L_015b::
  if m688.in_rettangolo(u, -52993.1839, 28541.8035, 27815.0352, -24601.1372) then goto L_016f end
  if m688.giocatore_secondo(u) then goto L_016c end
  m688.messaggio(u, 107)
  do return end
  ::L_016c::
  m688.messaggio(u, 108)
  do return end
  ::L_016f::
  m688.dopo(u, 20, "L_015b")
  do return end
end

script.L_0175 = function(u)
  ::L_0175::
  if m688.in_rettangolo(u, -36887.3533, 23156.3906, 27798.4151, -19298.6796) then goto L_01b7 end
  if m688.x_maggiore(u, 27784.555) then goto L_01ae end
  if m688.variabile_uguale(u, 2, 2) then goto L_01a5 end
  if m688.variabile_uguale(u, 2, 1) then goto L_019c end
  if m688.giocatore_secondo(u) then goto L_0199 end
  m688.messaggio(u, 109)
  do return end
  ::L_0199::
  m688.messaggio(u, 105)
  do return end
  ::L_019c::
  if m688.giocatore_secondo(u) then goto L_01a2 end
  m688.messaggio(u, 111)
  do return end
  ::L_01a2::
  m688.messaggio(u, 112)
  do return end
  ::L_01a5::
  if m688.giocatore_secondo(u) then goto L_01ab end
  m688.messaggio(u, 110)
  do return end
  ::L_01ab::
  m688.messaggio(u, 106)
  do return end
  ::L_01ae::
  if m688.giocatore_secondo(u) then goto L_01b4 end
  m688.messaggio(u, 108)
  do return end
  ::L_01b4::
  m688.messaggio(u, 107)
  do return end
  ::L_01b7::
  m688.dopo(u, 20, "L_0175")
  do return end
end

script.L_01ce = function(u)
  ::L_01ce::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_024e = function(u)
  ::L_024e::
  m688.variabile_piu(u, 1)
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_025b")
  do return end
end

script.L_025b = function(u)
  ::L_025b::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_02a0 = function(u)
  ::L_02a0::
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_033a", "L_0317")
  script.L_02c6(u)
  m688.dopo(u, 170, "L_02c3")
  m688.dopo(u, 190, "L_02ec")
  m688.dopo(u, 280, "L_033a")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_02c3 = function(u)
  ::L_02c3::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_02c6 = function(u)
  ::L_02c6::
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

script.L_02ec = function(u)
  ::L_02ec::
  m688.annulla_timer(u, "L_02ec")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0317 = function(u)
  ::L_0317::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_02f7")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0333")
  m688.dopo(u, 10, "L_032b")
  do return end
end

script.L_02f7 = function(u)
  ::L_02f7::
  if m688.flag_scena_uguale(u, 1) then goto L_0305 end
  if m688.bersaglio_id(u, 0) then goto L_0305 end
  m688.comportamento(u, "L_02f7")
  m688.salva_modello(u)
  do return end
  ::L_0305::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_032b")
  m688.annulla_timer(u, "L_0317")
  m688.insegui(u, 0, "L_033a", "L_0317")
  m688.salva_modello(u)
  do return end
end

script.L_032b = function(u)
  ::L_032b::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0333 = function(u)
  ::L_0333::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0333")
  do return end
end

script.L_033a = function(u)
  ::L_033a::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_033e = function(u)
  ::L_033e::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_033a", "L_0317")
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
  m688.dopo(u, 2, "L_036b")
  m688.dopo(u, 160, "L_0370")
  do return end
end

script.L_036b = function(u)
  ::L_036b::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0370 = function(u)
  ::L_0370::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0374 = function(u)
  ::L_0374::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0379 = function(u)
  ::L_0379::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0317")
  do return end
end

script.L_0383 = function(u, p)
  ::L_0383::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 20, "L_0392")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0392 = function(u)
  ::L_0392::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_02f7")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_03a0 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_03a0::
  do return end
end

script.L_03a1 = function(u, p)
  ::L_03a1::
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 30, "L_03ac")
  m688.salva_modello(u)
  do return end
end

script.L_03ac = function(u)
  ::L_03ac::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_03b3 = function(u, p)
  ::L_03b3::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_03b8 = function(u, p)
  ::L_03b8::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_03bf = function(u, p)
  ::L_03bf::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_03c7 = function(u)
  ::L_03c7::
  m688.annulla_timer(u, "L_0406")
  m688.dopo(u, 20, "L_0406")
  do return end
end

script.L_03d0 = function(u)
  ::L_03d0::
  m688.annulla_timer(u, "L_0409")
  m688.dopo(u, 20, "L_0409")
  do return end
end

script.L_03d9 = function(u)
  ::L_03d9::
  m688.annulla_timer(u, "L_040c")
  m688.dopo(u, 20, "L_040c")
  do return end
end

script.L_03e2 = function(u)
  ::L_03e2::
  m688.annulla_timer(u, "L_040f")
  m688.dopo(u, 20, "L_040f")
  do return end
end

script.L_03eb = function(u)
  ::L_03eb::
  m688.annulla_timer(u, "L_0412")
  m688.dopo(u, 20, "L_0412")
  do return end
end

script.L_03f4 = function(u)
  ::L_03f4::
  m688.annulla_timer(u, "L_0415")
  m688.dopo(u, 20, "L_0415")
  do return end
end

script.L_03fd = function(u)
  ::L_03fd::
  m688.annulla_timer(u, "L_0418")
  m688.dopo(u, 20, "L_0418")
  do return end
end

script.L_0406 = function(u)
  ::L_0406::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0409 = function(u)
  ::L_0409::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_040c = function(u)
  ::L_040c::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_040f = function(u)
  ::L_040f::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0412 = function(u)
  ::L_0412::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0415 = function(u)
  ::L_0415::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0418 = function(u)
  ::L_0418::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_041b = function(u)
  ::L_041b::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0423 = function(u, p)
  ::L_0423::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0427 = function(u)
  ::L_0427::
  m688.dopo(u, 60, "L_0431")
  do return end
end

script.L_042d = function(u, p)
  ::L_042d::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0431 = function(u)
  ::L_0431::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0437 = function(u)
  ::L_0437::
  m688.dopo(u, 40, "L_043d")
  do return end
end

script.L_043d = function(u)
  ::L_043d::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0443 = function(u)
  ::L_0443::
  m688.annulla_timer(u, "L_0431")
  m688.annulla_timer(u, "L_043d")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_044d = function(u, p)
  ::L_044d::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0452 = function(u, p)
  ::L_0452::
  m688.posizione(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448))
  m688.salva_modello(u)
  do return end
end

script.L_0459 = function(u)
  ::L_0459::
  if m688.giocatore_secondo(u) then goto L_045f end
  m688.messaggio(u, 25)
  do return end
  ::L_045f::
  m688.messaggio(u, 29)
  do return end
end

script.L_0462 = function(u)
  ::L_0462::
  if m688.giocatore_secondo(u) then goto L_0468 end
  m688.messaggio(u, 29)
  do return end
  ::L_0468::
  m688.messaggio(u, 25)
  do return end
end

return missione
