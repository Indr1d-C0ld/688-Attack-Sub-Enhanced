-- SCENE5B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE5B",
  missione = 5,
  titolo = { it = "Fuga", en = "ESCAPE" },
  lato = "urss",
  teatro = "gibilterra",
  posa = { metodo = "georef", centro = { lat = 35.7655, lon = -5.8445 }, metri_per_unita = { x = 22.969, y = 27.780 }, moto_classico_m_s_per_nodo = 1.2196, affidabilita = "alta", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 60.96, 121.92, 182.88 } },
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 280, filo_s = 190, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_058e" },
    esca = { gestore = "L_0662" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_06c5",
    telegrafo = "L_06d7",
    quota = "L_06dc",
    timone = "L_06e3",
    pilota_automatico_off = "L_073f",
    periscopio = "L_0747",
    carica_tubo_1 = "L_06eb",
    carica_tubo_2 = "L_06f4",
    carica_tubo_3 = "L_06fd",
    carica_tubo_4 = "L_0706",
    carica_tubo_5 = "L_070f",
    carica_tubo_6 = "L_0718",
    carica_tubo_7 = "L_0721",
    schiera_cala = "L_074b",
    schiera_recupera = "L_075b",
    schiera_persa = "L_0767",
    designa_bersaglio = "L_0771",
    posizione = "L_0776",
    antenna = "L_0751",
    guida_e0 = "L_0698",
    guida_e1 = "L_069d",
    guida_e2 = "L_06a7",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.169281005859, probabilita_originale = "5547/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -12714.7188, y = -774.195 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.16731262207, probabilita_originale = "10965/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -12714.7188, y = -774.195 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.166656494141, probabilita_originale = "5461/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -11971.9734, y = 4527.4309 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.16471862793, probabilita_originale = "10795/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -11971.9734, y = 4527.4309 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0846405029297, probabilita_originale = "5547/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -12714.7188, y = -774.195 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0833282470703, probabilita_originale = "5461/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -11971.9734, y = 4527.4309 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0826721191406, probabilita_originale = "2709/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -12714.7188, y = -774.195 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0813903808594, probabilita_originale = "2667/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0171", reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 34, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -11971.9734, y = 4527.4309 }, { x = -28178.0145, y = -6041.0172 }, { x = -38470.1906, y = -3350.8363 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_032c", reazione = nil, arrivo = "L_0231" } },
      { id = 3, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 4, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 345, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 5, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 147, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      { id = 7, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02fb" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0223" }, { dopo_s = 60, unita = 0, gestore = "L_0306" }, { dopo_s = 60, unita = 2, gestore = "L_019c" }, { dopo_s = 30, unita = 2, gestore = "L_0204" }, { dopo_s = 120, unita = 2, gestore = "L_029b" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
  },
  briefing = {
    it = { "Compagni, informazioni dell'intelligence rivelano che un sottomarino d'attacco americano classe 688 ha appena lasciato Napoli ed è diretto verso lo Stretto di Gibilterra. L'occasione di raccogliere dati sul 688 non può essere lasciata al vento! Il Leninskii Komsomol intercetti gli americani all'uscita dal Mediterraneo. Se gli americani vengono scoperti, mantenga il contatto sonar con il 688 il più a lungo possibile e registri le informazioni con il suo registratore giapponese. Deve restare entro la portata di tiro, e può lanciare armi solo nel caso in cui gli imperialisti si spaventino e agiscano per primi." },
    en = { "COMRADES, INTELLIGENCE INFORMATION REVEALS THAT AN AMERICAN 688 ATTACK SUBMARINE HAS JUST LEFT NAPLES AND IS HEADING FOR THE STRAIT OF GIBRALTAR. THE OPPORTUNITY TO GATHER DATA ON THE 688 CANNOT BE SET FREE WITH THE WIND! THE LENINSKII KOMSOMOL IS DIRECTED TO INTERCEPT THE AMERICANS AS THEY EXIT THE MEDITERRANEAN. IF THE AMERICANS ARE DISCOVERED, MAINTAIN SONAR CONTACT WITH THE 688 AS LONG AS POSSIBLE AND TAPE THE INFORMATION ON YOUR JAPANESE SOUND RECORDING DEVICE. YOU MUST STAY WITHIN FIRING RANGE, AND YOU MAY LAUNCH WEAPONS ONLY IN THE EVENT THAT THE IMPERIALISTS GROW FRIGHTENED AND TAKE ACTION FIRST." },
  },
  esiti = {
    [1] = { it = "CODICE: 5-1-BKB\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare durante un pattugliamento. Piangiamo la perdita di un equipaggio coraggioso, appassionatamente devoto agli ideali del progresso e del socialismo! Non saranno dimenticati, né considerati deboli o stupidi.", en = "CODE: 5-1-BKB\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE ON PATROL. WE MOURN THE LOSS OF A BRAVE CREW, PASSIONATELY COMMITTED TO THE IDEALS OF PROGRESS AND SOCIALISM! THEY WILL NOT BE FORGOTTEN, OR THOUGHT OF AS WEAK OR STUPID." },
    [2] = { it = "CODICE: 5-2-BKB\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento. Mentre indaghiamo sull'accaduto, piangiamo tutti la perdita di un equipaggio coraggioso, appassionatamente devoto agli ideali del progresso e del socialismo! Non saranno dimenticati, né considerati deboli o stupidi.", en = "CODE: 5-2-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE ON PATROL. AS WE INVESTIGATE THE MATTER, LET US ALL MOURN THE LOSS OF A BRAVE CREW PASSIONATELY COMMITTED TO THE IDEALS OF PROGRESS AND SOCIALISM! THEY WILL NOT BE FORGOTTEN OR THOUGHT OF AS WEAK OR STUPID." },
    [3] = { it = "CODICE: 5-3-BKB\nCompagni, è un peccato che gli agenti capitalisti abbiano dovuto essere distrutti, ma sono soddisfatto del vostro lavoro. Il sottomarino americano non poteva sfuggire alla presa della superiore tecnologia sovietica. I dati che avete raccolto aiuteranno la vostra Marina e il vostro Stato! Il vostro Paese non dimenticherà la prestazione esemplare e il sacrificio di questo equipaggio!", en = "CODE: 5-3-BKB\nCOMRADES, IT IS UNFORTUNATE THAT THE CAPITALIST AGENTS HAD TO BE DESTROYED, BUT I AM PLEASED BY YOUR WORK. THE AMERICAN SUBMARINE COULD NOT ESCAPE THE GRASP OF SUPERIOR SOVIET TECHNOLOGY. THE DATA YOU COLLECTED WILL HELP YOUR NAVY AND YOUR STATE! YOUR COUNTRY SHALL NOT FORGET THIS CREW'S DUTIFUL PERFORMANCE AND SACRIFICE IN THIS SITUATION!" },
    [4] = { it = "CODICE: 5-4-BKB\nAvete tradito il popolo e il Partito lanciando armi in aperta violazione dei miei ordini! Sarò giusto: farò in modo che l'equipaggio del Leninskii Komsomol sia processato prima di essere fucilato per questa violazione dell'autorità e questo eccesso di stupidità! Vi è ordinato di attendere il ricongiungimento con la Kiev e di seguire questa nave in porto!", en = "CODE: 5-4-BKB\nYOU HAVE BETRAYED THE PEOPLE AND PARTY BY LAUNCHING WEAPONS IN DIRECT OPPOSITION TO MY ORDERS! I WILL BE FAIR AND SEE THAT THE CREW OF THE LENINSKII KOMSOMOL SHALL BE TRIED BEFORE THEY ARE SHOT FOR THIS BREACH OF AUTHORITY AND EXCESS OF STUPIDITY! YOU ARE HEREBY ORDERED TO AWAIT RENDEZVOUS WITH THE KIEV AND FOLLOW THIS SHIP INTO PORT!" },
    [5] = { it = "CODICE: 5-5-BKB\nIl suo fallimento rappresenta una perdita di informazioni insostituibile per la Marina sovietica e comporterà severi provvedimenti disciplinari! Forse il Leninskii Komsomol ha bisogno di un cambio al comando? Rientri subito in porto, così potremo liberarci della zavorra in eccesso: peso morto che non rappresenta un successo per il Soviet Supremo!", en = "CODE: 5-5-BKB\nYOUR FAILURE REPRESENTS AN IRREPLACEABLE LOSS OF INFORMATION TO THE SOVIET NAVY AND WILL RESULT IN STRONG DISCIPLINARY MEASURES! PERHAPS THE LENINSKII KOMSOMOL NEEDS A CHANGE IN LEADERSHIP? RETURN TO PORT AT ONCE SO THAT WE CAN REMOVE THE EXCESS BALLAST: DEAD WEIGHT WHICH DOES NOT REPRESENT SUCCESS TO THE SUPREME SOVIET!" },
    [6] = { it = "CODICE: 5-6-BKB\nCompagni, ancora una volta sono soddisfatto del vostro eccellente lavoro. Il sottomarino americano non poteva sfuggire alla presa della superiore tecnologia sovietica. I dati che avete raccolto aiuteranno la vostra Marina e il vostro Stato! Il vostro Paese non dimenticherà la prestazione esemplare e il sacrificio di questo equipaggio!", en = "CODE: 5-6-BKB\nCOMRADES, ONCE AGAIN I AM PLEASED BY YOUR EXCELLENT WORK. THE AMERICAN SUBMARINE COULD NOT ESCAPE THE GRASP OF SUPERIOR SOVIET TECHNOLOGY. THE DATA YOU COLLECTED WILL HELP YOUR NAVY AND YOUR STATE! YOUR COUNTRY SHALL NOT FORGET THIS CREW'S DUTIFUL PERFORMANCE AND SACRIFICE IN THIS SITUATION!" },
    [7] = { it = "CODICE: 5-7-BKB\nIl suo fallimento rappresenta una perdita di informazioni insostituibile per la Marina sovietica e comporterà severi provvedimenti disciplinari! Il Leninskii Komsomol non avrebbe mai potuto infliggere il colpo di grazia al battello americano, se fosse stato necessario.", en = "CODE: 5-7-BKB\nYOUR FAILURE REPRESENTS AN IRREPLACEABLE LOSS OF INFORMATION TO THE SOVIET NAVY AND WILL RESULT IN STRONG DISCIPLINARY MEASURES! THERE IS NO WAY THE LENINSKII KOMSOMOL COULD HAVE DELIVERED A KILLING BLOW TO THE AMERICAN BOAT IF IT HAD BEEN NEEDED." },
    [8] = { it = "CODICE: 5-8-BKB\nQuesto Kapitan non sa nemmeno dove si trovi il suo battello: come posso aspettarmi che trovi i suoi nemici? Se passasse sulle carte nautiche tanto tempo quanto, ne sono certo, ne passa con la vodka, forse saprebbe dove sta andando. Se non resta in posizione come ordinato, vedremo se lo zampolit sa eseguire le istruzioni meglio di lei.", en = "CODE: 5-8-BKB\nTHIS KAPITAN DOES NOT EVEN REALIZE THE POSITION OF HIS OWN VESSEL, HOW CAN I EXPECT HIM TO FIND HIS ENEMIES. IF HE WERE TO SPEND AS MUCH TIME WITH HIS CHARTS AS I'M SURE HE SPENDS WITH HIS VODKA, HE MIGHT KNOW WHERE HE WAS GOING. IF YOU DON'T STAY ON STATION AS DIRECTED, WE WILL SEE IF THE ZAMPOLIT CAN DO A BETTER JOB OF FOLLOWING INSTRUCTION." },
    [19] = { it = "CODICE: 5-19-BKB\nTraditori! Agenti del fascismo! Nemici del popolo! Così le madri di questo equipaggio chiamano i loro figli! Uomini che hanno ucciso i loro compagni d'armi e mandato in fondo al mare apparecchiature innocenti. E per giunta in acque capitaliste! Il Leninskii Komsomol emerga immediatamente. All'intero equipaggio è ordinato di consegnare le proprie misere vite e di salire a bordo della Kiev. A quel punto un equipaggio sostitutivo assumerà il comando del Leninskii Komsomol.", en = "CODE: 5-19-BKB\nTRAITORS! AGENTS OF FASCISM! ENEMIES OF THE PEOPLE! THESE ARE THE NAMES THE MOTHERS OF THIS CREW USE TO DESCRIBE THEIR SONS! MEN THAT HAVE KILLED THEIR BROTHERS IN ARMS AND SENT INNOCENT EQUIPMENT TO THE BOTTOM OF THE SEA. AND IN CAPITALIST WATERS NO LESS! THE LENINSKII KOMSOMOL IS INSTRUCTED TO SURFACE IMMEDIATELY. THE ENTIRE CREW IS ORDERED TO SURRENDER THEIR MISERABLE LIVES OVER AND BOARD THE KIEV. AT THAT TIME A REPLACEMENT CREW WILL TAKE COMMAND OF THE LENINSKII KOMSOMOL." },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 4, punti = 0, spostamento_max_m = 468.4496, non_risolte = 0 },
  script = script,
}

script.L_0171 = function(u)
  ::L_0171::
  if m688.bersaglio_id(u, 2) then goto L_0180 end
  m688.variabile(u, 2, 0)
  m688.comportamento(u, "L_0171")
  m688.salva_modello(u)
  do return end
  ::L_0180::
  m688.variabile(u, 2, 1)
  m688.ingaggia(u, 30, "L_0180", "L_0176")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0190 end
  do return end
  ::L_0190::
  if m688.bersaglio_vicino(u, 5) then goto L_0196 end
  do return end
  ::L_0196::
  m688.variabile(u, 2, 2)
  do return end
end

script.L_0176 = function(u)
  ::L_0176::
  m688.variabile(u, 2, 0)
  m688.comportamento(u, "L_0171")
  m688.salva_modello(u)
  do return end
end

script.L_0180 = function(u)
  ::L_0180::
  m688.variabile(u, 2, 1)
  m688.ingaggia(u, 30, "L_0180", "L_0176")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0190 end
  do return end
  ::L_0190::
  if m688.bersaglio_vicino(u, 5) then goto L_0196 end
  do return end
  ::L_0196::
  m688.variabile(u, 2, 2)
  do return end
end

script.L_019c = function(u)
  ::L_019c::
  if m688.in_rettangolo(u, -36887.3533, 23156.3906, 6422.8801, -19339.9937) then goto L_01c3 end
  if m688.x_maggiore(u, 6419.6778) then goto L_01c3 end
  if m688.variabile_uguale(u, 2, 2) then goto L_01c0 end
  if m688.variabile_uguale(u, 2, 1) then goto L_01bd end
  m688.messaggio(u, 105)
  do return end
  ::L_01bd::
  m688.messaggio(u, 107)
  do return end
  ::L_01c0::
  m688.messaggio(u, 106)
  do return end
  ::L_01c3::
  if m688.in_rettangolo(u, -8292.3055, 8942.1172, 7142.8123, 1870.8591) then goto L_01fe end
  if m688.in_rettangolo(u, -8298.9638, 1871.8714, -2417.3514, -1666.7979) then goto L_01fe end
  if m688.in_rettangolo(u, -22254.441, 8082.5424, -9041.527, -5197.5763) then goto L_01fe end
  if m688.in_rettangolo(u, -31794.1047, 11646.9965, -23033.0446, -8707.2411) then goto L_01fe end
  if m688.in_rettangolo(u, -39845.6021, 17866.4182, -32600.4226, -7793.3476) then goto L_01fe end
  script.L_03b1(u)
  m688.salva_modello(u)
  ::L_01fe::
  m688.dopo(u, 30, "L_019c")
  do return end
end

script.L_0204 = function(u)
  ::L_0204::
  m688.marca_contatti(u)
  m688.dopo(u, 16, "L_0204")
  do return end
end

script.L_0223 = function(u)
  ::L_0223::
  if m688.manomesso(u) then goto L_0227 end
  do return end
  ::L_0227::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0231 = function(u)
  ::L_0231::
  m688.variabile_piu(u, 4)
  script.L_0239(u)
  m688.salva_modello(u)
  do return end
end

script.L_0239 = function(u)
  ::L_0239::
  m688.nuovo_caso(u)
  m688.vai_a(u, -28178.0145, -6041.0172, 30, "L_025e")
  if m688.caso(u, 0.3359) then goto L_025d end
  m688.vai_a(u, -27411.8157, 3678.1408, 30, "L_025e")
  if m688.caso(u, 0.668) then goto L_025d end
  m688.vai_a(u, -26663.2636, 8094.7853, 30, "L_025e")
  ::L_025d::
  do return end
end

script.L_025e = function(u)
  ::L_025e::
  m688.variabile_piu(u, 4)
  script.L_0266(u)
  m688.salva_modello(u)
  do return end
end

script.L_0266 = function(u)
  ::L_0266::
  m688.nuovo_caso(u)
  m688.vai_a(u, -38470.1906, -3350.8363, 30, "L_0298")
  if m688.caso(u, 0.2539) then goto L_0297 end
  m688.vai_a(u, -39170.6076, 4606.3424, 30, "L_0298")
  if m688.caso(u, 0.5039) then goto L_0297 end
  m688.vai_a(u, -38408.5138, 10789.5713, 30, "L_0298")
  if m688.caso(u, 0.7539) then goto L_0297 end
  m688.vai_a(u, -38385.3521, 16092.3017, 30, "L_0298")
  ::L_0297::
  do return end
end

script.L_0298 = function(u)
  ::L_0298::
  m688.messaggio(u, 105)
  do return end
end

script.L_029b = function(u)
  ::L_029b::
  m688.telegrafo(u, 0)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_02a9")
  do return end
end

script.L_02a9 = function(u)
  ::L_02a9::
  if m688.flag_scena_bit(u, 128) then goto L_02dc end
  m688.nuovo_caso(u)
  m688.quota(u, 30.48, 1)
  m688.flag_scena_accendi(u, 1)
  if m688.caso(u, 0.2539) then goto L_02dc end
  m688.flag_scena_spegni(u, 1)
  m688.flag_scena_accendi(u, 2)
  m688.quota(u, 91.44, 1)
  if m688.caso(u, 0.5039) then goto L_02dc end
  m688.flag_scena_spegni(u, 2)
  m688.flag_scena_accendi(u, 4)
  m688.quota(u, 152.4, 1)
  if m688.caso(u, 0.5039) then goto L_02dc end
  m688.flag_scena_spegni(u, 4)
  m688.flag_scena_accendi(u, 8)
  m688.quota(u, 243.84, 1)
  ::L_02dc::
  m688.telegrafo(u, 0.0391)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_02e7")
  do return end
end

script.L_02e7 = function(u)
  ::L_02e7::
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_02f1")
  do return end
end

script.L_02f1 = function(u)
  ::L_02f1::
  m688.telegrafo(u, 0.3906)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_029b")
  do return end
end

script.L_02fb = function(u)
  ::L_02fb::
  m688.vai_a(u, 8608.275, 7174.8502, 30, "L_04fe")
  m688.salva_modello(u)
  do return end
end

script.L_0306 = function(u)
  ::L_0306::
  if m688.in_rettangolo(u, -52993.1839, 28541.8035, 20439.7662, -24621.2179) then goto L_0314 end
  m688.messaggio(u, 108)
  do return end
  ::L_0314::
  if m688.unita_esiste(u, 2) then goto L_0326 end
  if m688.variabile_uguale(u, 3, 0) then goto L_0323 end
  m688.messaggio(u, 103)
  do return end
  ::L_0323::
  m688.messaggio(u, 104)
  do return end
  ::L_0326::
  m688.dopo(u, 20, "L_0306")
  do return end
end

script.L_032c = function(u)
  ::L_032c::
  if m688.bersaglio_id(u, 0) then goto L_03ed end
  if m688.bersaglio_arma(u) then goto L_0339 end
  ::L_0334::
  m688.comportamento(u, "L_032c")
  m688.salva_modello(u)
  do return end
  ::L_0339::
  if m688.bersaglio_mi_punta(u) then goto L_033f end
  goto L_0334
  ::L_033f::
  if m688.traccia_solida(u) then goto L_034e end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_033f", "L_0334")
  m688.salva_modello(u)
  do return end
  ::L_034e::
  if m688.bersaglio_vicino(u, 3) then goto L_036e end
  ::L_0353::
  m688.annulla_timer(u, "L_029b")
  m688.annulla_timer(u, "L_02a9")
  m688.annulla_timer(u, "L_02e7")
  m688.annulla_timer(u, "L_02f1")
  m688.ferma_navigazione(u)
  script.L_0382(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 120, "L_03c7")
  m688.salva_modello(u)
  do return end
  ::L_036e::
  if m688.flag_scena_bit(u, 16) then goto L_0353 end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_03d4")
  do return end
  ::L_03ed::
  script.L_0382(u)
  script.L_03b1(u)
  if m688.traccia_solida(u) then goto L_03fb end
  m688.comportamento(u, "L_032c")
  m688.salva_modello(u)
  do return end
  ::L_03fb::
  if m688.variabile_uguale(u, 1, 0) then goto L_0405 end
  goto L_0411
  ::L_0405::
  if m688.variabile_uguale(u, 0, 0) then goto L_0434 end
  m688.variabile(u, 0, 0)
  ::L_0411::
  if m688.bersaglio_vicino(u, 8) then goto L_0419 end
  goto L_0434
  ::L_0419::
  if m688.variabile_uguale(u, 4, 4) then goto L_0434 end
  m688.comportamento(u, "L_032c")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  m688.variabile_piu(u, 4)
  script.L_05d0(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_0439")
  do return end
  ::L_0434::
  m688.comportamento(u, "L_032c")
  m688.salva_modello(u)
  do return end
end

script.L_0334 = function(u)
  ::L_0334::
  m688.comportamento(u, "L_032c")
  m688.salva_modello(u)
  do return end
end

script.L_033f = function(u)
  ::L_033f::
  if m688.traccia_solida(u) then goto L_034e end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_033f", "L_0334")
  m688.salva_modello(u)
  do return end
  ::L_034e::
  if m688.bersaglio_vicino(u, 3) then goto L_036e end
  ::L_0353::
  m688.annulla_timer(u, "L_029b")
  m688.annulla_timer(u, "L_02a9")
  m688.annulla_timer(u, "L_02e7")
  m688.annulla_timer(u, "L_02f1")
  m688.ferma_navigazione(u)
  script.L_0382(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 120, "L_03c7")
  m688.salva_modello(u)
  do return end
  ::L_036e::
  if m688.flag_scena_bit(u, 16) then goto L_0353 end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_03d4")
  do return end
end

script.L_0382 = function(u)
  ::L_0382::
  if m688.flag_scena_bit(u, 128) then goto L_0390 end
  m688.annulla_timer(u, "L_037e")
  m688.dopo(u, 240, "L_037e")
  m688.flag_scena_accendi(u, 128)
  ::L_0390::
  if m688.flag_scena_bit(u, 1) then goto L_03a2 end
  if m688.flag_scena_bit(u, 2) then goto L_03a8 end
  if m688.flag_scena_bit(u, 4) then goto L_03a8 end
  ::L_039c::
  m688.quota(u, 30.48, 1)
  do return end
  ::L_03a2::
  m688.quota(u, 274.32, 1)
  do return end
  ::L_03a8::
  m688.nuovo_caso(u)
  if m688.caso(u, 0.5039) then goto L_039c end
  goto L_03a2
end

script.L_037e = function(u)
  ::L_037e::
  m688.flag_scena_spegni(u, 128)
  m688.salva_modello(u)
  do return end
end

script.L_03b1 = function(u)
  ::L_020b::
  m688.nuovo_caso(u)
  m688.vai_a(u, -12714.7188, -774.195, 30, "L_0231")
  if m688.caso(u, 0.5039) then goto L_0222 end
  m688.vai_a(u, -11971.9734, 4527.4309, 30, "L_0231")
  ::L_0222::
  do return end
  ::L_0239::
  m688.nuovo_caso(u)
  m688.vai_a(u, -28178.0145, -6041.0172, 30, "L_025e")
  if m688.caso(u, 0.3359) then goto L_025d end
  m688.vai_a(u, -27411.8157, 3678.1408, 30, "L_025e")
  if m688.caso(u, 0.668) then goto L_025d end
  m688.vai_a(u, -26663.2636, 8094.7853, 30, "L_025e")
  ::L_025d::
  do return end
  ::L_0266::
  m688.nuovo_caso(u)
  m688.vai_a(u, -38470.1906, -3350.8363, 30, "L_0298")
  if m688.caso(u, 0.2539) then goto L_0297 end
  m688.vai_a(u, -39170.6076, 4606.3424, 30, "L_0298")
  if m688.caso(u, 0.5039) then goto L_0297 end
  m688.vai_a(u, -38408.5138, 10789.5713, 30, "L_0298")
  if m688.caso(u, 0.7539) then goto L_0297 end
  m688.vai_a(u, -38385.3521, 16092.3017, 30, "L_0298")
  ::L_0297::
  do return end
  ::L_03b1::
  if m688.variabile_uguale(u, 4, 0) then goto L_020b end
  if m688.variabile_uguale(u, 4, 1) then goto L_0239 end
  if m688.variabile_uguale(u, 4, 2) then goto L_0266 end
  do return end
end

script.L_03c7 = function(u)
  ::L_03c7::
  m688.comportamento(u, "L_032c")
  script.L_03b1(u)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_029b")
  do return end
end

script.L_03d4 = function(u)
  ::L_03d4::
  m688.telegrafo(u, 1.0547)
  script.L_0382(u)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_03c7")
  m688.dopo(u, 120, "L_03e9")
  goto L_0662
  ::L_0662::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_065e", "L_063b")
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
  m688.dopo(u, 2, "L_068f")
  m688.dopo(u, 160, "L_0694")
  do return end
end

script.L_03e9 = function(u)
  ::L_03e9::
  m688.flag_scena_spegni(u, 16)
  m688.salva_modello(u)
  do return end
end

script.L_0439 = function(u)
  ::L_0439::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_04fe = function(u)
  ::L_04fe::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_058e = function(u)
  ::L_058e::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_065e", "L_063b")
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
  m688.dopo(u, 170, "L_05e7")
  m688.dopo(u, 190, "L_0610")
  m688.dopo(u, 280, "L_065e")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_05d0 = function(u)
  ::L_05d0::
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_065e", "L_063b")
  script.L_05ea(u)
  m688.dopo(u, 240, "L_065e")
  do return end
end

script.L_05e7 = function(u)
  ::L_05e7::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_05ea = function(u)
  ::L_05ea::
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

script.L_0610 = function(u)
  ::L_0610::
  m688.annulla_timer(u, "L_0610")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_063b = function(u)
  ::L_063b::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_061b")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0657")
  m688.dopo(u, 10, "L_064f")
  do return end
end

script.L_061b = function(u)
  ::L_061b::
  if m688.flag_scena_uguale(u, 1) then goto L_0629 end
  if m688.bersaglio_id(u, 0) then goto L_0629 end
  m688.comportamento(u, "L_061b")
  m688.salva_modello(u)
  do return end
  ::L_0629::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_064f")
  m688.annulla_timer(u, "L_063b")
  m688.insegui(u, 0, "L_065e", "L_063b")
  m688.salva_modello(u)
  do return end
end

script.L_064f = function(u)
  ::L_064f::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0657 = function(u)
  ::L_0657::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0657")
  do return end
end

script.L_065e = function(u)
  ::L_065e::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0662 = function(u)
  ::L_0662::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_065e", "L_063b")
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
  m688.dopo(u, 2, "L_068f")
  m688.dopo(u, 160, "L_0694")
  do return end
end

script.L_068f = function(u)
  ::L_068f::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0694 = function(u)
  ::L_0694::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0698 = function(u)
  ::L_0698::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_069d = function(u)
  ::L_069d::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_063b")
  do return end
end

script.L_06a7 = function(u, p)
  ::L_06a7::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 20, "L_06b6")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_06b6 = function(u)
  ::L_06b6::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_061b")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_06c4 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_06c4::
  do return end
end

script.L_06c5 = function(u, p)
  ::L_06c5::
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 30, "L_06d0")
  m688.salva_modello(u)
  do return end
end

script.L_06d0 = function(u)
  ::L_06d0::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_06d7 = function(u, p)
  ::L_06d7::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06dc = function(u, p)
  ::L_06dc::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06e3 = function(u, p)
  ::L_06e3::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_06eb = function(u)
  ::L_06eb::
  m688.annulla_timer(u, "L_072a")
  m688.dopo(u, 20, "L_072a")
  do return end
end

script.L_06f4 = function(u)
  ::L_06f4::
  m688.annulla_timer(u, "L_072d")
  m688.dopo(u, 20, "L_072d")
  do return end
end

script.L_06fd = function(u)
  ::L_06fd::
  m688.annulla_timer(u, "L_0730")
  m688.dopo(u, 20, "L_0730")
  do return end
end

script.L_0706 = function(u)
  ::L_0706::
  m688.annulla_timer(u, "L_0733")
  m688.dopo(u, 20, "L_0733")
  do return end
end

script.L_070f = function(u)
  ::L_070f::
  m688.annulla_timer(u, "L_0736")
  m688.dopo(u, 20, "L_0736")
  do return end
end

script.L_0718 = function(u)
  ::L_0718::
  m688.annulla_timer(u, "L_0739")
  m688.dopo(u, 20, "L_0739")
  do return end
end

script.L_0721 = function(u)
  ::L_0721::
  m688.annulla_timer(u, "L_073c")
  m688.dopo(u, 20, "L_073c")
  do return end
end

script.L_072a = function(u)
  ::L_072a::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_072d = function(u)
  ::L_072d::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0730 = function(u)
  ::L_0730::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0733 = function(u)
  ::L_0733::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0736 = function(u)
  ::L_0736::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0739 = function(u)
  ::L_0739::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_073c = function(u)
  ::L_073c::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_073f = function(u)
  ::L_073f::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0747 = function(u, p)
  ::L_0747::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_074b = function(u)
  ::L_074b::
  m688.dopo(u, 60, "L_0755")
  do return end
end

script.L_0751 = function(u, p)
  ::L_0751::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0755 = function(u)
  ::L_0755::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_075b = function(u)
  ::L_075b::
  m688.dopo(u, 40, "L_0761")
  do return end
end

script.L_0761 = function(u)
  ::L_0761::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0767 = function(u)
  ::L_0767::
  m688.annulla_timer(u, "L_0755")
  m688.annulla_timer(u, "L_0761")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0771 = function(u, p)
  ::L_0771::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0776 = function(u, p)
  ::L_0776::
  m688.posizione(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448))
  m688.salva_modello(u)
  do return end
end

return missione
