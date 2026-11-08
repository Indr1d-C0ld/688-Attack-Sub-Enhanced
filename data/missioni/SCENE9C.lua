-- SCENE9C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE9C",
  missione = 9,
  titolo = { it = "Ritorno a casa", en = "HOMECOMING" },
  lato = "due_giocatori",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 243.84, 365.76 } },
  dotazione = { mk48 = 16, sea_lance = 8, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "sea_lance" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_067c" },
    sea_lance = { classe = "?", velocita_kn = 39.2857, corsa_s = 240, filo_s = 20, danno = 40, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_05da" },
    esca = { classe = "los_angeles", velocita_kn = 0.9143, corsa_s = 0, filo_s = nil, danno = nil, rumore = 0, testa = nil, virata_gradi_s = nil, quota_max_m = nil, gestore = "L_072f" },
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0794",
    telegrafo = "L_07a6",
    quota = "L_07ab",
    timone = "L_07b2",
    pilota_automatico_off = "L_080e",
    periscopio = "L_0816",
    carica_tubo_1 = "L_07ba",
    carica_tubo_2 = "L_07c3",
    carica_tubo_3 = "L_07cc",
    carica_tubo_4 = "L_07d5",
    carica_tubo_5 = "L_07de",
    carica_tubo_6 = "L_07e7",
    carica_tubo_7 = "L_07f0",
    schiera_cala = "L_081a",
    schiera_recupera = "L_082a",
    schiera_persa = "L_0836",
    designa_bersaglio = "L_0840",
    posizione = "L_0845",
    antenna = "L_0820",
    guida_e0 = "L_0767",
    guida_e1 = "L_076c",
    guida_e2 = "L_0776",
    f0 = "L_084c",
    f1 = "L_0855",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.171875, probabilita_originale = "11/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -111416.32, y = -16593.92 }, originale = { x = 2112, y = 2752 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -74672.64, y = 16593.92 }, originale = { x = 3104, y = 3648 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -114972.16, y = -13038.08 }, originale = { x = 2016, y = 2848 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -117342.72, y = -3555.84 }, originale = { x = 1952, y = 3104 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -80599.04, y = -1185.28 }, originale = { x = 2944, y = 3168 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -148160, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.16796875, probabilita_originale = "43/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -144604.16, y = 0 }, originale = { x = 1216, y = 3200 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -151715.84, y = -34373.12 }, originale = { x = 1024, y = 2272 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -128010.24, y = -16593.92 }, originale = { x = 1664, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -129195.52, y = -1185.28 }, originale = { x = 1632, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -160012.8, y = -36743.68 }, originale = { x = 800, y = 2208 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -85340.16, y = -1185.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.16796875, probabilita_originale = "43/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -114972.16, y = 1185.28 }, originale = { x = 2016, y = 3232 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -155271.68, y = 14223.36 }, originale = { x = 928, y = 3584 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -128010.24, y = -16593.92 }, originale = { x = 1664, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -129195.52, y = -1185.28 }, originale = { x = 1632, y = 3168 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -80599.04, y = -1185.28 }, originale = { x = 2944, y = 3168 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -148160, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -144604.16, y = -17779.2 }, originale = { x = 1216, y = 2720 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -72302.08, y = -32002.56 }, originale = { x = 3168, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -139863.04, y = -14223.36 }, originale = { x = 1344, y = 2816 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -141048.32, y = -3555.84 }, originale = { x = 1312, y = 3104 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -160012.8, y = -36743.68 }, originale = { x = 800, y = 2208 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -85340.16, y = -1185.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -146974.72, y = -9482.24 }, originale = { x = 1152, y = 2944 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -124454.4, y = -46225.92 }, originale = { x = 1760, y = 1952 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -139863.04, y = -14223.36 }, originale = { x = 1344, y = 2816 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -141048.32, y = -3555.84 }, originale = { x = 1312, y = 3104 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -160012.8, y = -36743.68 }, originale = { x = 800, y = 2208 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -85340.16, y = -1185.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -110231.04, y = -7111.68 }, originale = { x = 2144, y = 3008 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -123269.12, y = 26076.16 }, originale = { x = 1792, y = 3904 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -120898.56, y = -10667.52 }, originale = { x = 1856, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -124454.4, y = -10667.52 }, originale = { x = 1760, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -125639.68, y = -7111.68 }, originale = { x = 1728, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -129195.52, y = -10667.52 }, originale = { x = 1632, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -131566.08, y = -7111.68 }, originale = { x = 1568, y = 3008 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -88896, y = -1185.28 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01d6" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -114972.16, y = -13038.08 }, originale = { x = 2016, y = 2848 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -117342.72, y = -3555.84 }, originale = { x = 1952, y = 3104 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -91266.56, y = -4741.12 }, { x = -72302.08, y = -36743.68 }, { x = -30817.28, y = -18964.48 }, { x = -10667.52, y = -53337.6 }, { x = 30817.28, y = -36743.68 }, { x = 56893.44, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_046a", reazione = nil, arrivo = "L_01d6" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -128010.24, y = -24890.88 }, originale = { x = 1664, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 0, difesa = 15, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -80599.04, y = -1185.28 }, originale = { x = 2944, y = 3168 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 15, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -148160, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_048f", reazione = nil, arrivo = "L_0216" } },
      },
      timer = { { dopo_s = 100, unita = 0, gestore = "L_04d2" }, { dopo_s = 100, unita = 1, gestore = "L_0518" }, { dopo_s = 50, unita = 7, gestore = "L_045c" }, { dopo_s = 30, unita = 8, gestore = "L_045c" }, { dopo_s = 10, unita = 10, gestore = "L_045c" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: TRAVERSATA ATLANTICA", "(SEGRETISSIMO)", "Comandante, lei scorterà un convoglio di rifornimenti militari verso est, fino al Golfo di Biscaglia, in Francia. Lì un gruppo navale si ricongiungerà con lei e accompagnerà il convoglio in un porto sicuro. Non abbassi la guardia nell'ultimo tratto. Non serve che le dica quanto sia importante questo convoglio. Le armi sono libere, ma solo dopo un'identificazione certa. Buona fortuna, Comandante.", "VICEAMMIRAGLIO J.W. RATCLIFF III", "Compagni. Gli americani si avvicinano come tante anatre sull'acqua. Presto saranno abbastanza vicini perché i cacciatori entrino in azione! Il Leninskii Komsomol attenderà mentre il Pskovskii Komsomolets distrugge il convoglio con una salva di missili SS-NX-21. Una volta che i missili avranno colpito, il Leninskii Komsomol si avvicinerà e distruggerà tutto ciò che sarà rimasto a galla. Compagni, attendiamo con impazienza la notizia della vostra vittoria!" },
    en = { "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: ATLANTIC CROSSING", "(TOP SECRET)", "CAPTAIN, YOU ARE TO ESCORT A CONVOY OF MILITARY SUPPLIES EAST TO THE BAY OF BISCAY IN FRANCE. THERE A TASK FORCE WILL RENDEZVOUS WITH YOU AND LEAD YOUR CONVOY INTO SAFE PORT. DON'T LET UP YOUR GUARD ON THE LAST STRETCH. I DON'T HAVE TO TELL YOU HOW IMPORTANT THIS CONVOY IS. WEAPONS ARE FREE BUT ONLY ONCE YOU HAVE POSITIVE IDENTIFICATION. GOOD LUCK, CAPTAIN.", "VADM J.W. RATCLIFF III", "COMRADES. THE AMERICANS APPROACH LIKE SO MANY DUCKS ON THE WATER. THEY WILL SOON BE CLOSE ENOUGH FOR THE HUNTERS TO MOVE IN! THE LENINSKII KOMSOMOL WILL WAIT AS THE PSKOVSKII KOMSOMOLETS CRUSHES THE CONVOY WITH A BARRAGE OF SS-NX-21 MISSILES. ONCE THE MISSILES HAVE IMPACTED, THE LENINSKII KOMSOMOL WILL MOVE IN AND DESTROY ANYTHING LEFT FLOATING IN THE WATER. COMRADES, WE EAGERLY AWAIT NEWS OF YOUR VICTORY!" },
  },
  esiti = {
    [1] = { it = "CODICE: 9-1-CIN\nIeri l'USS Los Angeles si è incagliato ed è andato distrutto. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 9-1-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED YESTERDAY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 9-2-CIN\nIeri l'USS Los Angeles è stato distrutto da siluri, in seguito a un attacco a sorpresa dell'Unione Sovietica in acque internazionali. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 9-2-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY, AFTER A SURPRISE ATTACK BY THE SOVIET UNION IN INTERNATIONAL WATERS. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 9-3-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare.", en = "CODE: 9-3-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA." },
    [4] = { it = "CODICE: 9-4-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare.", en = "CODE: 9-4-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA." },
    [5] = { it = "CODICE: 9-5-CIN\nComandante, a lei è andata decisamente meglio che alle navi che doveva proteggere. Nessuna delle nostre navi è sopravvissuta all'attacco sovietico, nonostante lei avesse a disposizione un sottomarino armato di tutto punto. La sua prestazione ha confermato il detto secondo cui ci vuole una bella guerra per distinguere gli uomini dai ragazzini. Quando arriverete in porto voglio vedere i suoi ufficiali schierati davanti a me, per decidere chi è adatto a comandare sottomarini e chi farebbe meglio a cercarsi il prossimo incarico nella marina mercantile.", en = "CODE: 9-5-CIN\nCAPTAIN, YOU CERTAINLY FARED BETTER THAN THE SHIPS YOU WERE PROTECTING. NOT ONE OF OUR SHIPS SURVIVED THE SOVIET ATTACK DESPITE THE FACT THAT YOU HAD A FULLY ARMED SUBMARINE AT YOUR DISPOSAL. YOUR PERFORMANCE HAS PROVEN THE SAYING THAT IT TAKES A GOOD WAR TO FLUSH OUT THE MEN FROM THE BOYS. WHEN YOU PEOPLE GET IN PORT I WANT TO SEE YOUR WARDROOM STAFF LINED UP BEFORE ME SO I CAN DECIDE WHO'S FIT TO COMMAND SUBMARINES AND WHO SHOULD LOOK TO THE COMMERCIAL SECTOR FOR THEIR NEXT ASSIGNMENT." },
    [6] = { it = "CODICE: 9-6-CIN\nCongratulazioni, Comandante: è riuscito a resistere all'attacco a sorpresa sovietico. Grazie alla prontezza del suo equipaggio il convoglio ha subito solo danni parziali nello scontro. Rientri immediatamente in porto per i lavori di raddobbo e il suo prossimo incarico. Siamo in guerra, Comandante: abbiamo del lavoro da fare!", en = "CODE: 9-6-CIN\nCONGRATULATIONS, CAPTAIN, YOU MANAGED TO WITHSTAND THE SURPRISE SOVIET ATTACK. THE CONVOY SUSTAINED PARTIAL DAMAGE IN THE CONFRONTATION DUE TO YOUR CREW'S FAST REACTIONS. RETURN TO PORT IMMEDIATELY FOR REFIT AND YOUR NEXT ASSIGNMENT. THERE'S A WAR ON CAPTAIN, WE HAVE WORK TO DO!" },
    [7] = { it = "CODICE: 9-7-CIN\nLei ha lasciato l'area di gioco: missione annullata.", en = "CODE: 9-7-CIN\nYOU LEFT THE PLAYFIELD MISSION ABORTED." },
    [8] = { it = "CODICE: 9-8-CIN\nIl suo avversario ha lasciato la mappa: missione annullata.", en = "CODE: 9-8-CIN\nYOUR OPPONENT LEFT THE MAP, MISSION ABORTED." },
    [9] = { it = "CODICE: 9-9-CIN\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di localizzare così tanti bersagli rumorosi e lenti! Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 9-9-CIN\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF SO MANY LOUD, SLOW TARGETS!. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
    [10] = { it = "CODICE: 9-10-CIN\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di localizzare così tanti bersagli rumorosi e lenti! Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 9-10-CIN\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF SO MANY LOUD, SLOW TARGETS!. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
    [11] = { it = "CODICE: 9-11-CIN\nCompagni, la Stavka si congratula con l'equipaggio del Leninskii Komsomol per il successo dell'attacco! L'oceano ha inghiottito il convoglio come se non fosse mai esistito! L'Occidente decadente imparerà a non scherzare con la potente Marina sovietica! Salutiamo il valore dell'equipaggio del Leninskii Komsomol! Per tanta dedizione al dovere, a questo Kapitan sarà conferito l'ambito titolo di Eroe dell'Unione Sovietica.", en = "CODE: 9-11-CIN\nCOMRADES, THE STAVKA CONGRATULATES THE CREW OF THE LENINSKII KOMSOMOL ON THEIR SUCCESSFUL ATTACK! THE OCEAN HAS SWALLOWED THE CONVOY AS IF SHE NEVER EXISTED! THE DECADENT WEST SHALL LEARN NOT TO TRIFLE WITH THE MIGHTY SOVIET NAVY! WE SALUTE THE VALOR OF THE CREW OF THE LENINSKII KOMSOMOL! FOR SUCH DEDICATION TO DUTY, THIS KAPITAN SHALL RECEIVE THE COVETED AWARD OF HERO OF THE U.S.S.R." },
    [15] = { it = "CODICE: 9-15-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 9-15-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [16] = { it = "CODICE: 9-16-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 9-16-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [17] = { it = "CODICE: 9-17-CIN\nL'USS Los Angeles si è incagliato ed è andato distrutto.", en = "CODE: 9-17-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED." },
    [18] = { it = "CODICE: 9-18-CIN\nIeri l'USS Los Angeles è stato distrutto da siluri.", en = "CODE: 9-18-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY." },
    [19] = { it = "CODICE: 9-19-CIN\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. L'ho seguita sul SOSUS: sembra che lei abbia molte cose da spiegare. Per esempio, come diavolo è riuscito ad affondare una delle navi di superficie che stava scortando...\nCODICE: 9-20-CIN\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di identificare i suoi bersagli prima di aprire il fuoco! Rientri immediatamente in porto.", en = "CODE: 9-19-CIN\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, I MONITORED YOU ON SOSUS, LOOKS LIKE YOU HAVE A LOT OF EXPLAINING TO DO. LIKE HOW IN HELL YOU MANAGED TO SINK A SKIMMER YOU WERE ESCORTING, FOR ONE...\nCODE: 9-20-CIN\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE IDENTITY OF HIS TARGETS BEFORE FIRING! RETURN TO PORT IMMEDIATELY." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_01d6 = function(u)
  ::L_01d6::
  m688.vai_a(u, -72302.08, -36743.68, 20, "L_01e1")
  m688.salva_modello(u)
  do return end
end

script.L_01e1 = function(u)
  ::L_01e1::
  m688.vai_a(u, -30817.28, -18964.48, 20, "L_01ec")
  m688.salva_modello(u)
  do return end
end

script.L_01ec = function(u)
  ::L_01ec::
  m688.vai_a(u, -10667.52, -53337.6, 20, "L_01f7")
  m688.salva_modello(u)
  do return end
end

script.L_01f7 = function(u)
  ::L_01f7::
  m688.vai_a(u, 30817.28, -36743.68, 20, "L_0202")
  m688.salva_modello(u)
  do return end
end

script.L_0202 = function(u)
  ::L_0202::
  m688.vai_a(u, 56893.44, -55708.16, 20, "L_020d")
  m688.salva_modello(u)
  do return end
end

script.L_020d = function(u)
  ::L_020d::
  if m688.giocatore_secondo(u) then goto L_0213 end
  m688.messaggio(u, 110)
  do return end
  ::L_0213::
  m688.messaggio(u, 109)
  do return end
end

script.L_0216 = function(u)
  ::L_0216::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_045c = function(u)
  ::L_045c::
  m688.marca_contatti(u)
  m688.dopo(u, 12, "L_045c")
  do return end
end

script.L_046a = function(u)
  ::L_046a::
  if m688.traccia_solida(u) then goto L_0472 end
  ::L_046d::
  m688.comportamento(u, "L_046a")
  m688.salva_modello(u)
  do return end
  ::L_0472::
  if m688.bersaglio_id(u, 0) then goto L_046d end
  if m688.bersaglio_nave(u) then goto L_046d end
  if m688.bersaglio_vicino(u, 0.6) then goto L_0484 end
  m688.comportamento(u, "L_046a")
  m688.salva_modello(u)
  do return end
  ::L_0484::
  m688.comportamento(u, "L_046a")
  m688.salva_modello(u)
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 5)
  do return end
end

script.L_048f = function(u)
  ::L_048f::
  if m688.traccia_solida(u) then goto L_0497 end
  ::L_0492::
  m688.comportamento(u, "L_048f")
  m688.salva_modello(u)
  do return end
  ::L_0497::
  if m688.bersaglio_nave(u) then goto L_049d end
  goto L_0492
  ::L_049d::
  if m688.bersaglio_vicino(u, 20) then goto L_04a5 end
  goto L_0492
  ::L_04a5::
  m688.flag_scena_piu(u)
  if m688.flag_scena_uguale(u, 18) then goto L_04be end
  m688.telegrafo(u, 0)
  m688.collega(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_04b9")
  goto L_05cd
  ::L_04be::
  m688.telegrafo(u, 0.9961)
  m688.modello_tipo(u, 20)
  m688.quota(u, 304.8, 0.7812)
  m688.comportamento(u, "L_03a1")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_045c")
  do return end
  ::L_05cd::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_bersaglio(u)
  m688.dopo(u, 10, "L_05d7")
  do return end
end

script.L_03a1 = function(u)
  ::L_03a1::
  if m688.traccia_solida(u) then goto L_03a9 end
  ::L_03a4::
  m688.comportamento(u, "L_03a1")
  m688.salva_modello(u)
  do return end
  ::L_03a9::
  if m688.bersaglio_nave(u) then goto L_03b7 end
  if m688.bersaglio_arma(u) then goto L_03cb end
  if m688.bersaglio_id(u, 1) then goto L_03b7 end
  goto L_03a4
  ::L_03b7::
  if m688.flag_scena_uguale(u, 2) then goto L_03a4 end
  m688.flag_scena_piu(u)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_03fc")
  m688.dopo(u, 320, "L_040c")
  goto L_0626
  ::L_03cb::
  if m688.bersaglio_mi_punta(u) then goto L_03d1 end
  goto L_03a4
  ::L_03d1::
  if m688.variabile_uguale(u, 2, 1) then goto L_03ef end
  m688.variabile_piu(u, 2)
  m688.telegrafo(u, 1)
  m688.quota(u, 304.8, 0.9961)
  m688.comportamento(u, "L_03a1")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_040f")
  goto L_072f
  ::L_03ef::
  m688.telegrafo(u, 0.9961)
  m688.quota(u, 152.4, 0.9961)
  m688.comportamento(u, "L_03a1")
  m688.salva_modello(u)
  do return end
  ::L_0626::
  m688.rumore_transitorio(u)
  script.L_064c(u)
  m688.nuova_unita(u)
  m688.dopo(u, 150, "L_0692")
  m688.dopo(u, 170, "L_06c9")
  m688.dopo(u, 240, "L_072b")
  m688.messaggio_al_giocatore(u, 2)
  do return end
  ::L_072f::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "los_angeles")
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_072b", "L_0703")
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
  m688.dopo(u, 2, "L_075e")
  m688.dopo(u, 160, "L_0763")
  do return end
end

script.L_03fc = function(u)
  ::L_03fc::
  m688.comportamento(u, "L_03a1")
  m688.salva_modello(u)
  goto L_0626
  ::L_0626::
  m688.rumore_transitorio(u)
  script.L_064c(u)
  m688.nuova_unita(u)
  m688.dopo(u, 150, "L_0692")
  m688.dopo(u, 170, "L_06c9")
  m688.dopo(u, 240, "L_072b")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_040c = function(u)
  ::L_040c::
  m688.flag_scena_meno(u)
  m688.salva_modello(u)
  do return end
end

script.L_040f = function(u)
  ::L_040f::
  m688.variabile_meno(u, 2)
  m688.salva_modello(u)
  do return end
end

script.L_04b9 = function(u)
  ::L_04b9::
  m688.comportamento(u, "L_048f")
  m688.salva_modello(u)
  do return end
end

script.L_04d2 = function(u)
  ::L_04d2::
  if m688.unita_esiste(u, 2) then goto L_04fe end
  if m688.unita_esiste(u, 3) then goto L_04fe end
  if m688.unita_esiste(u, 4) then goto L_04fe end
  if m688.unita_esiste(u, 5) then goto L_04fe end
  if m688.unita_esiste(u, 6) then goto L_04fe end
  if m688.unita_esiste(u, 7) then goto L_04fe end
  if m688.unita_esiste(u, 8) then goto L_04fe end
  if m688.giocatore_secondo(u) then goto L_04fb end
  m688.messaggio(u, 105)
  do return end
  ::L_04fb::
  m688.messaggio(u, 111)
  do return end
  ::L_04fe::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -101934.08) then goto L_0512 end
  if m688.giocatore_secondo(u) then goto L_050f end
  m688.messaggio(u, 107)
  do return end
  ::L_050f::
  m688.messaggio(u, 108)
  do return end
  ::L_0512::
  m688.dopo(u, 20, "L_04d2")
  do return end
end

script.L_0518 = function(u)
  ::L_0518::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -101934.08) then goto L_052c end
  if m688.giocatore_secondo(u) then goto L_0529 end
  m688.messaggio(u, 108)
  do return end
  ::L_0529::
  m688.messaggio(u, 107)
  do return end
  ::L_052c::
  m688.dopo(u, 20, "L_0518")
  do return end
end

script.L_05d7 = function(u)
  ::L_05d7::
  m688.attacco_casuale(u, 100)
  do return end
end

script.L_05da = function(u)
  ::L_05da::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_0695(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_classe(u, "tipo_2c")
  m688.modello_difesa(u, 40)
  m688.telegrafo(u, 0.7812)
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.quota_ora(u, 0)
  m688.collega(u, 1)
  m688.quota(u, 365.76, 0.5859)
  m688.nuova_unita(u)
  m688.dopo(u, 20, "L_0608")
  m688.dopo(u, 10, "L_0724")
  m688.dopo(u, 240, "L_072b")
  do return end
end

script.L_0608 = function(u)
  ::L_0608::
  m688.collega(u, 0)
  goto L_060e
  ::L_060e::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_061a")
  do return end
end

script.L_061a = function(u)
  ::L_061a::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_060e")
  do return end
end

script.L_060e = function(u)
  ::L_060e::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_061a")
  do return end
end

script.L_064c = function(u)
  ::L_064c::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_072b", "L_0703")
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 1.0286)
  m688.modello_avanti_tutta(u, 50.2857)
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
  do return end
end

script.L_067c = function(u)
  ::L_067c::
  script.L_0695(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_0692")
  m688.dopo(u, 190, "L_06c9")
  m688.dopo(u, 280, "L_072b")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0692 = function(u)
  ::L_0692::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0695 = function(u)
  ::L_0695::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_072b", "L_0703")
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
  do return end
end

script.L_06c9 = function(u)
  ::L_06c9::
  m688.annulla_timer(u, "L_06c9")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0703 = function(u)
  ::L_0703::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_06d4")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0724")
  m688.dopo(u, 10, "L_0717")
  do return end
end

script.L_06d4 = function(u)
  ::L_06d4::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_060e")
  m688.annulla_timer(u, "L_061a")
  if m688.traccia_solida(u) then goto L_06e7 end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 30, "L_06d4", "L_0703")
  m688.salva_modello(u)
  do return end
  ::L_06e7::
  if m688.bersaglio_vicino(u, 10) then goto L_06f1 end
  m688.comportamento(u, "L_06d4")
  m688.salva_modello(u)
  do return end
  ::L_06f1::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0717")
  m688.annulla_timer(u, "L_0703")
  m688.insegui(u, 0, "L_072b", "L_0703")
  m688.salva_modello(u)
  do return end
end

script.L_0717 = function(u)
  ::L_0717::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_060e")
  do return end
end

script.L_0724 = function(u)
  ::L_0724::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0724")
  do return end
end

script.L_072b = function(u)
  ::L_072b::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_072f = function(u)
  ::L_072f::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "los_angeles")
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_072b", "L_0703")
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
  m688.dopo(u, 2, "L_075e")
  m688.dopo(u, 160, "L_0763")
  do return end
end

script.L_075e = function(u)
  ::L_075e::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0763 = function(u)
  ::L_0763::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0767 = function(u)
  ::L_0767::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_076c = function(u)
  ::L_076c::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0703")
  do return end
end

script.L_0776 = function(u, p)
  ::L_0776::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_0785")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0785 = function(u)
  ::L_0785::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_06d4")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0793 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0793::
  do return end
end

script.L_0794 = function(u, p)
  ::L_0794::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_079f")
  m688.salva_modello(u)
  do return end
end

script.L_079f = function(u)
  ::L_079f::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_07a6 = function(u, p)
  ::L_07a6::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07ab = function(u, p)
  ::L_07ab::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07b2 = function(u, p)
  ::L_07b2::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_07ba = function(u)
  ::L_07ba::
  m688.annulla_timer(u, "L_07f9")
  m688.dopo(u, 20, "L_07f9")
  do return end
end

script.L_07c3 = function(u)
  ::L_07c3::
  m688.annulla_timer(u, "L_07fc")
  m688.dopo(u, 20, "L_07fc")
  do return end
end

script.L_07cc = function(u)
  ::L_07cc::
  m688.annulla_timer(u, "L_07ff")
  m688.dopo(u, 20, "L_07ff")
  do return end
end

script.L_07d5 = function(u)
  ::L_07d5::
  m688.annulla_timer(u, "L_0802")
  m688.dopo(u, 20, "L_0802")
  do return end
end

script.L_07de = function(u)
  ::L_07de::
  m688.annulla_timer(u, "L_0805")
  m688.dopo(u, 20, "L_0805")
  do return end
end

script.L_07e7 = function(u)
  ::L_07e7::
  m688.annulla_timer(u, "L_0808")
  m688.dopo(u, 20, "L_0808")
  do return end
end

script.L_07f0 = function(u)
  ::L_07f0::
  m688.annulla_timer(u, "L_080b")
  m688.dopo(u, 20, "L_080b")
  do return end
end

script.L_07f9 = function(u)
  ::L_07f9::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_07fc = function(u)
  ::L_07fc::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_07ff = function(u)
  ::L_07ff::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0802 = function(u)
  ::L_0802::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0805 = function(u)
  ::L_0805::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0808 = function(u)
  ::L_0808::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_080b = function(u)
  ::L_080b::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_080e = function(u)
  ::L_080e::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0816 = function(u, p)
  ::L_0816::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_081a = function(u)
  ::L_081a::
  m688.dopo(u, 60, "L_0824")
  do return end
end

script.L_0820 = function(u, p)
  ::L_0820::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0824 = function(u)
  ::L_0824::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_082a = function(u)
  ::L_082a::
  m688.dopo(u, 40, "L_0830")
  do return end
end

script.L_0830 = function(u)
  ::L_0830::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0836 = function(u)
  ::L_0836::
  m688.annulla_timer(u, "L_0824")
  m688.annulla_timer(u, "L_0830")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0840 = function(u, p)
  ::L_0840::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0845 = function(u, p)
  ::L_0845::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

script.L_084c = function(u)
  ::L_084c::
  if m688.giocatore_secondo(u) then goto L_0852 end
  m688.messaggio(u, 25)
  do return end
  ::L_0852::
  m688.messaggio(u, 29)
  do return end
end

script.L_0855 = function(u)
  ::L_0855::
  if m688.giocatore_secondo(u) then goto L_085b end
  m688.messaggio(u, 29)
  do return end
  ::L_085b::
  m688.messaggio(u, 25)
  do return end
end

return missione
