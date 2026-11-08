-- SCENE9A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE9A",
  missione = 9,
  titolo = { it = "Ritorno a casa", en = "HOMECOMING" },
  lato = "usa",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 243.84, 365.76 } },
  dotazione = { mk48 = 16, sea_lance = 8, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "sea_lance" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_06d3" },
    sea_lance = { classe = "?", velocita_kn = 39.2857, corsa_s = 240, filo_s = 20, danno = 40, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_063d" },
    esca = { gestore = "L_0786" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_07e9",
    telegrafo = "L_07fb",
    quota = "L_0800",
    timone = "L_0807",
    pilota_automatico_off = "L_0863",
    periscopio = "L_086b",
    carica_tubo_1 = "L_080f",
    carica_tubo_2 = "L_0818",
    carica_tubo_3 = "L_0821",
    carica_tubo_4 = "L_082a",
    carica_tubo_5 = "L_0833",
    carica_tubo_6 = "L_083c",
    carica_tubo_7 = "L_0845",
    schiera_cala = "L_086f",
    schiera_recupera = "L_087f",
    schiera_persa = "L_088b",
    designa_bersaglio = "L_0895",
    posizione = "L_089a",
    antenna = "L_0875",
    guida_e0 = "L_07bc",
    guida_e1 = "L_07c1",
    guida_e2 = "L_07cb",
  },
  copertura = 0.0917228884537,
  varianti = {
    { probabilita = 0.0207644571137, probabilita_originale = "130881465/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0204450039274, probabilita_originale = "2013561/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0204450039274, probabilita_originale = "2013561/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0204425275461, probabilita_originale = "128852295/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0204425275461, probabilita_originale = "128852295/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0202925376339, probabilita_originale = "511627545/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0202925376339, probabilita_originale = "511627545/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0202925376339, probabilita_originale = "511627545/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0202925376339, probabilita_originale = "511627545/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0201280271223, probabilita_originale = "1982343/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0201280271223, probabilita_originale = "1982343/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0201280271223, probabilita_originale = "1982343/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0201280271223, probabilita_originale = "1982343/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0201255891345, probabilita_originale = "126854585/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.020125550741, probabilita_originale = "126854343/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199803447472, probabilita_originale = "7871193/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0199779246473, probabilita_originale = "503695335/274877906944", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198313435967, probabilita_originale = "1999998585/1099511627776", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198313435967, probabilita_originale = "1999998585/1099511627776", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198313435967, probabilita_originale = "1999998585/1099511627776", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198313435967, probabilita_originale = "1999998585/1099511627776", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.019820618154, probabilita_originale = "249864615/137438953472", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198159646863, probabilita_originale = "1951609/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198159646863, probabilita_originale = "1951609/1073741824", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198135266985, probabilita_originale = "124887609/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0198135266985, probabilita_originale = "124887609/68719476736", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0196705719604, probabilita_originale = "7749159/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0196705719604, probabilita_originale = "7749159/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0196705719604, probabilita_originale = "7749159/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0196705719604, probabilita_originale = "7749159/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
    { probabilita = 0.0196705719604, probabilita_originale = "7749159/4294967296", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04a2", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 70, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0421", reazione = nil, arrivo = "L_032d" } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_04e1", reazione = nil, arrivo = "L_032d" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c1" }, { dopo_s = 100, unita = 0, gestore = "L_054e" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_0494" }, { dopo_s = 30, unita = 8, gestore = "L_0494" }, { dopo_s = 60, unita = 9, gestore = "L_049b" }, { dopo_s = 120, unita = 9, gestore = "L_031d" }, { dopo_s = 600, unita = 9, gestore = "L_02ec" }, { dopo_s = 10, unita = 10, gestore = "L_0494" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: TRAVERSATA ATLANTICA", "(SEGRETISSIMO)", "Comandante, lei scorterà un convoglio di rifornimenti militari verso est, fino al Golfo di Biscaglia, in Francia. Lì un gruppo navale si ricongiungerà con lei e accompagnerà il convoglio in un porto sicuro. Non abbassi la guardia nell'ultimo tratto. Non serve che le dica quanto sia importante questo convoglio. Le armi sono libere, ma solo dopo un'identificazione certa. Buona fortuna, Comandante." },
    en = { "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: ATLANTIC CROSSING", "(TOP SECRET)", "CAPTAIN, YOU ARE TO ESCORT A CONVOY OF MILITARY SUPPLIES EAST TO THE BAY OF BISCAY IN FRANCE. THERE, A TASK FORCE WILL RENDEZVOUS WITH YOU AND LEAD YOUR CONVOY INTO SAFE PORT. DON'T LET UP YOUR GUARD ON THE LAST STRETCH. I DON'T HAVE TO TELL YOU HOW IMPORTANT THIS CONVOY IS. WEAPONS ARE FREE BUT ONLY ONCE YOU HAVE POSITIVE IDENTIFICATION. GOOD LUCK, CAPTAIN." },
  },
  esiti = {
    [1] = { it = "CODICE: 9-1-ACP\nL'USS Los Angeles è andato perduto in mare al largo delle coste francesi. Il Los Angeles scortava un convoglio di navi da rifornimento diretto in Francia quando è stato attaccato di sorpresa da sottomarini sovietici. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 9-1-ACP\nTHE USS LOS ANGELES WAS LOST AT SEA, OFF THE COAST OF FRANCE. THE LOS ANGELES WAS ESCORTING A CONVOY OF SUPPLY SHIPS ENROUTE TO FRANCE WHEN SHE CAME UNDER SURPRISE ATTACK BY SOVIET SUBMARINES. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 9-2-ACP\nIeri l'USS Los Angeles è stato distrutto da siluri mentre era impegnato in un combattimento difensivo contro forze sovietiche in acque internazionali. Unitevi a me nell'esprimere il nostro cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 9-2-ACP\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY, WHILE ENGAGED IN DEFENSIVE COMBAT WITH SOVIET FORCES IN INTERNATIONAL WATERS. JOIN ME IN EXPRESSING OUR GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [5] = { it = "CODICE: 9-5-ACP\nComandante, a lei è andata decisamente meglio che alle navi che doveva proteggere. Nessuna delle nostre navi è sopravvissuta all'attacco sovietico, nonostante lei avesse a disposizione un sottomarino armato di tutto punto. La sua prestazione ha confermato il detto secondo cui ci vuole una bella guerra per distinguere gli uomini dai ragazzini. Quando arriverete in porto voglio vedere i suoi ufficiali schierati davanti a me, per decidere chi è adatto a comandare sottomarini e chi farebbe meglio a cercarsi il prossimo incarico nella marina mercantile.", en = "CODE: 9-5-ACP\nCAPTAIN, YOU CERTAINLY FARED BETTER THAN THE SHIPS YOU WERE PROTECTING. NOT ONE OF OUR SHIPS SURVIVED THE SOVIET ATTACK DESPITE THE FACT THAT YOU HAD A FULLY ARMED SUBMARINE AT YOUR DISPOSAL. YOUR PERFORMANCE HAS PROVEN THE SAYING THAT IT TAKES A GOOD WAR TO FLUSH OUT THE MEN FROM THE BOYS. WHEN YOU PEOPLE GET IN PORT I WANT TO SEE YOUR WARDROOM STAFF LINED UP BEFORE ME SO I CAN DECIDE WHO'S FIT TO COMMAND SUBMARINES AND WHO SHOULD LOOK TO THE COMMERCIAL SECTOR FOR THEIR NEXT ASSIGNMENT." },
    [6] = { it = "CODICE: 9-6-ACP\nCongratulazioni, Comandante: è riuscito a resistere all'attacco a sorpresa sovietico. Il convoglio è sopravvissuto allo scontro grazie alla prontezza del suo equipaggio, e lei ha anche sconfitto le unità sovietiche attaccanti. Rientri immediatamente in porto per i lavori di raddobbo e il suo prossimo incarico. Siamo in guerra, Comandante: abbiamo del lavoro da fare!", en = "CODE: 9-6-ACP\nCONGRATULATIONS, CAPTAIN, YOU MANAGED TO WITHSTAND THE SURPRISE SOVIET ATTACK. THE CONVOY SURVIVED THE CONFRONTATION DUE TO YOUR CREW'S FAST REACTIONS. YOU ALSO DEFEATED THE ATTACKING SOVIET VESSELS. RETURN TO PORT IMMEDIATELY FOR REFIT AND YOUR NEXT ASSIGNMENT. THERE'S A WAR ON CAPTAIN, WE HAVE WORK TO DO!" },
    [7] = { it = "CODICE: 9-7-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete esattamente cosa stavate cercando di combinare là fuori. Dove stavate andando? Si porti dietro la sua roba, perché resterà qui per il prossimo mese.", en = "CODE: 9-7-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME EXACTLY WHAT YOU WERE TRYING TO PULL OFF OUT THERE. WHERE WERE YOU GOING? BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WILL BE HERE FOR THE NEXT MONTH." },
    [8] = { it = "CODICE: 9-8-ACP\nCongratulazioni, Comandante: è riuscito a resistere all'attacco a sorpresa sovietico. Grazie alla prontezza del suo equipaggio il convoglio ha subito solo danni parziali nello scontro. Rientri immediatamente in porto per i lavori di raddobbo e il suo prossimo incarico. Siamo in guerra, Comandante: abbiamo del lavoro da fare!", en = "CODE: 9-8-ACP\nCONGRATULATIONS, CAPTAIN, YOU MANAGED TO WITHSTAND THE SURPRISE SOVIET ATTACK. THE CONVOY SUSTAINED PARTIAL DAMAGE IN THE CONFRONTATION DUE TO YOUR CREW'S FAST REACTIONS. RETURN TO PORT IMMEDIATELY FOR REFIT AND YOUR NEXT ASSIGNMENT. THERE'S A WAR ON CAPTAIN, WE HAVE WORK TO DO!" },
    [19] = { it = "CODICE: 9-19-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Le abili tattiche con cui ha ingaggiato e distrutto unità amiche potrebbero pesare a suo favore davanti alla corte marziale.", en = "CODE: 9-19-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. THE CLEVER TACTICS YOU USED TO ENGAGE AND DESTROY FRIENDLY VESSELS MIGHT WEIGH IN YOUR FAVOR AT YOUR COURT-MARTIAL." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_0214 = function(u)
  ::L_0214::
  m688.nuovo_caso(u)
  m688.posizione(u, -14223.36, -47411.2)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_02a1 end
  m688.posizione(u, 5926.4, -47411.2)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_02a1 end
  m688.posizione(u, 4741.12, -65190.4)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_02a1 end
  m688.posizione(u, -14223.36, -65190.4)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_02a1 end
  m688.posizione(u, -30817.28, -30817.28)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_02a1 end
  m688.posizione(u, -3555.84, -27261.44)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_02a1 end
  m688.posizione(u, 28446.72, -32002.56)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_02a1 end
  m688.posizione(u, 27261.44, -56893.44)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_02a1 end
  m688.posizione(u, 22520.32, -84154.88)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_02a1 end
  m688.posizione(u, -28446.72, -84154.88)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_02a1 end
  m688.posizione(u, -35558.4, -56893.44)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_02a1 end
  m688.posizione(u, 1185.28, -55708.16)
  m688.rotta_ora(u, 300)
  ::L_02a1::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0214")
  do return end
end

script.L_02a8 = function(u)
  ::L_02a8::
  m688.messaggio(u, 108)
  m688.salva_modello(u)
  do return end
end

script.L_02c1 = function(u)
  ::L_02c1::
  if m688.manomesso(u) then goto L_02c5 end
  do return end
  ::L_02c5::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_02ec = function(u)
  ::L_02ec::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_02f7")
  do return end
end

script.L_02f7 = function(u)
  ::L_02f7::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_02ec")
  do return end
end

script.L_031d = function(u)
  ::L_031d::
  m688.riprendi_navigazione(u)
  m688.quota(u, 365.76, 1)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.dopo(u, 480, "L_0308")
  do return end
end

script.L_0308 = function(u)
  ::L_0308::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.telegrafo(u, 0.0195)
  m688.quota(u, 45.72, 0.3906)
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_031d")
  do return end
end

script.L_032d = function(u)
  ::L_032d::
  m688.annulla_tutti_i_timer(u)
  m688.telegrafo(u, 0.0195)
  m688.quota(u, 6.096, 1)
  m688.rotta(u, nil)
  m688.rateo_virata(u, -0.72)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  m688.dopo(u, 50, "L_0359")
  m688.dopo(u, 60, "L_034b")
  do return end
end

script.L_034b = function(u)
  ::L_034b::
  m688.vai_a(u, 88896, 58078.72, 30, "L_02ac")
  m688.telegrafo(u, 0.625)
  m688.salva_modello(u)
  do return end
end

script.L_02ac = function(u)
  ::L_02ac::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0359 = function(u)
  ::L_0359::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0494")
  do return end
end

script.L_0362 = function(u)
  ::L_0362::
  if m688.traccia_solida(u) then goto L_036a end
  ::L_0365::
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  do return end
  ::L_036a::
  if m688.bersaglio_nave(u) then goto L_0378 end
  if m688.bersaglio_arma(u) then goto L_03c8 end
  if m688.bersaglio_id(u, 0) then goto L_0378 end
  goto L_0365
  ::L_0378::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 5, 3) then goto L_03a6 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 5)
  if m688.unita_id(u, 10) then goto L_039a end
  script.L_0689(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_045f")
  do return end
  ::L_039a::
  script.L_0694(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_045f")
  do return end
  ::L_03a6::
  m688.annulla_timer(u, "L_03b8")
  m688.annulla_timer(u, "L_03c4")
  m688.quota(u, 365.76, 0.4688)
  m688.salva_modello(u)
  m688.dopo(u, 160, "L_03b8")
  do return end
  ::L_03c8::
  if m688.bersaglio_mi_punta(u) then goto L_03ce end
  goto L_0365
  ::L_03ce::
  if m688.bersaglio_vicino(u, 1.28) then goto L_03d6 end
  goto L_03fa
  ::L_03d6::
  if m688.variabile_uguale(u, 2, 1) then goto L_03fa end
  m688.variabile_piu(u, 2)
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 365.76, 0.9961)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0490")
  m688.dopo(u, 120, "L_040d")
  goto L_0786
  ::L_03fa::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 152.4, 0.9961)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_040d")
  do return end
  ::L_0786::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0782", "L_075a")
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
  m688.dopo(u, 2, "L_07b3")
  m688.dopo(u, 160, "L_07b8")
  do return end
end

script.L_03b8 = function(u)
  ::L_03b8::
  m688.quota(u, 6.096, 1)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_03c4")
  do return end
end

script.L_03c4 = function(u)
  ::L_03c4::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_040d = function(u)
  ::L_040d::
  m688.quota(u, 6.096, 1)
  m688.telegrafo(u, 0.0781)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_041d")
  do return end
end

script.L_041d = function(u)
  ::L_041d::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0421 = function(u)
  ::L_0421::
  if m688.bersaglio_id(u, 0) then goto L_042e end
  if m688.bersaglio_arma(u) then goto L_0463 end
  ::L_0429::
  m688.comportamento(u, "L_0421")
  m688.salva_modello(u)
  do return end
  ::L_042e::
  if m688.traccia_solida(u) then goto L_043a end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_0421")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  do return end
  ::L_043a::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.comportamento(u, "L_0421")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 10) then goto L_0448 end
  ::L_0447::
  do return end
  ::L_0448::
  if m688.variabile_uguale(u, 5, 3) then goto L_0447 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 5)
  script.L_0689(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_045f")
  do return end
  ::L_0463::
  if m688.bersaglio_mi_punta(u) then goto L_0469 end
  goto L_0429
  ::L_0469::
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 152.4, 0.9961)
  m688.comportamento(u, "L_0421")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 1.28) then goto L_047d end
  goto L_0447
  ::L_047d::
  if m688.variabile_uguale(u, 2, 1) then goto L_0447 end
  m688.variabile_piu(u, 2)
  m688.dopo(u, 120, "L_0490")
  script.L_0786(u)
  do return end
end

script.L_045f = function(u)
  ::L_045f::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_0490 = function(u)
  ::L_0490::
  m688.variabile_meno(u, 2)
  do return end
end

script.L_0494 = function(u)
  ::L_0494::
  m688.marca_contatti(u)
  m688.dopo(u, 28, "L_0494")
  do return end
end

script.L_049b = function(u)
  ::L_049b::
  m688.marca_contatti(u)
  m688.dopo(u, 28, "L_0494")
  do return end
end

script.L_04a2 = function(u)
  ::L_04a2::
  if m688.bersaglio_id(u, 9) then goto L_04b1 end
  if m688.bersaglio_id(u, 10) then goto L_04b1 end
  m688.comportamento(u, "L_04a2")
  m688.salva_modello(u)
  do return end
  ::L_04b1::
  m688.annulla_timer(u, "L_04db")
  m688.dopo(u, 60, "L_04db")
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_04c7 end
  m688.impulso_sonar(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_04a2")
  m688.salva_modello(u)
  do return end
  ::L_04c7::
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_04a2")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 5) then goto L_04d4 end
  do return end
  ::L_04d4::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
end

script.L_04db = function(u)
  ::L_04db::
  m688.telegrafo(u, 0.5)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  do return end
end

script.L_04e1 = function(u)
  ::L_04e1::
  if m688.traccia_solida(u) then goto L_04e9 end
  ::L_04e4::
  m688.comportamento(u, "L_04e1")
  m688.salva_modello(u)
  do return end
  ::L_04e9::
  if m688.bersaglio_nave(u) then goto L_04ef end
  goto L_04e4
  ::L_04ef::
  if m688.bersaglio_vicino(u, 20) then goto L_04f7 end
  goto L_04e4
  ::L_04f7::
  if m688.bersaglio_vicino(u, 4) then m688.dopo(u, 0.05, "L_04f7") do return end end  -- attesa: si riprova al passo successivo
  m688.flag_scena_piu(u)
  if m688.flag_scena_uguale(u, 8) then goto L_053a end
  m688.quota(u, 365.76, 1)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -60)
  m688.collega(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0516")
  goto L_0630
  ::L_053a::
  m688.telegrafo(u, 1.0547)
  m688.modello_tipo(u, 20)
  m688.quota(u, 365.76, 0.7812)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0494")
  do return end
  ::L_0630::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_063a")
  do return end
end

script.L_04f7 = function(u)
  ::L_04f7::
  if m688.bersaglio_vicino(u, 4) then m688.dopo(u, 0.05, "L_04f7") do return end end  -- attesa: si riprova al passo successivo
  m688.flag_scena_piu(u)
  if m688.flag_scena_uguale(u, 8) then goto L_053a end
  m688.quota(u, 365.76, 1)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -60)
  m688.collega(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0516")
  goto L_0630
  ::L_053a::
  m688.telegrafo(u, 1.0547)
  m688.modello_tipo(u, 20)
  m688.quota(u, 365.76, 0.7812)
  m688.comportamento(u, "L_0362")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0494")
  do return end
  ::L_0630::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_063a")
  do return end
end

script.L_0516 = function(u)
  ::L_0516::
  m688.quota(u, 365.76, 0.3906)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -60)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_0526")
  do return end
end

script.L_0526 = function(u)
  ::L_0526::
  m688.riprendi_navigazione(u)
  m688.quota(u, 6.096, 1)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0533")
  do return end
end

script.L_0533 = function(u)
  ::L_0533::
  m688.comportamento(u, "L_04e1")
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_054e = function(u)
  ::L_054e::
  if m688.unita_esiste(u, 2) then goto L_0574 end
  if m688.unita_esiste(u, 3) then goto L_0574 end
  if m688.unita_esiste(u, 4) then goto L_0574 end
  if m688.unita_esiste(u, 5) then goto L_0574 end
  if m688.unita_esiste(u, 6) then goto L_0574 end
  if m688.unita_esiste(u, 7) then goto L_0574 end
  if m688.unita_esiste(u, 8) then goto L_0574 end
  m688.messaggio(u, 105)
  do return end
  ::L_0574::
  if m688.unita_esiste(u, 9) then goto L_0581 end
  if m688.unita_esiste(u, 10) then goto L_0581 end
  m688.messaggio(u, 106)
  do return end
  ::L_0581::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 142233.6, -101934.08) then goto L_058f end
  m688.messaggio(u, 107)
  do return end
  ::L_058f::
  m688.dopo(u, 20, "L_054e")
  do return end
end

script.L_063a = function(u)
  ::L_063a::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_063d = function(u)
  ::L_063d::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_06ec(u)
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
  m688.dopo(u, 20, "L_066b")
  m688.dopo(u, 10, "L_077b")
  m688.dopo(u, 240, "L_0782")
  do return end
end

script.L_066b = function(u)
  ::L_066b::
  m688.collega(u, 0)
  goto L_0671
  ::L_0671::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_067d")
  do return end
end

script.L_067d = function(u)
  ::L_067d::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0671")
  do return end
end

script.L_0671 = function(u)
  ::L_0671::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_067d")
  do return end
end

script.L_0689 = function(u)
  ::L_0689::
  m688.rumore_transitorio(u)
  script.L_06a3(u)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0782")
  do return end
end

script.L_0694 = function(u)
  ::L_0694::
  m688.rumore_transitorio(u)
  script.L_06a3(u)
  m688.modello_classe(u, "tipo_2d")
  m688.modello_difesa(u, 200)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0782")
  do return end
end

script.L_06a3 = function(u)
  ::L_06a3::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0782", "L_075a")
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

script.L_06d3 = function(u)
  ::L_06d3::
  script.L_06ec(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_06e9")
  m688.dopo(u, 190, "L_0720")
  m688.dopo(u, 280, "L_0782")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_06e9 = function(u)
  ::L_06e9::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_06ec = function(u)
  ::L_06ec::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0782", "L_075a")
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

script.L_0720 = function(u)
  ::L_0720::
  m688.annulla_timer(u, "L_0720")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_075a = function(u)
  ::L_075a::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_072b")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_077b")
  m688.dopo(u, 10, "L_076e")
  do return end
end

script.L_072b = function(u)
  ::L_072b::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_0671")
  m688.annulla_timer(u, "L_067d")
  if m688.traccia_solida(u) then goto L_073e end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 30, "L_072b", "L_075a")
  m688.salva_modello(u)
  do return end
  ::L_073e::
  if m688.bersaglio_vicino(u, 10) then goto L_0748 end
  m688.comportamento(u, "L_072b")
  m688.salva_modello(u)
  do return end
  ::L_0748::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_076e")
  m688.annulla_timer(u, "L_075a")
  m688.insegui(u, 0, "L_0782", "L_075a")
  m688.salva_modello(u)
  do return end
end

script.L_076e = function(u)
  ::L_076e::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0671")
  do return end
end

script.L_077b = function(u)
  ::L_077b::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_077b")
  do return end
end

script.L_0782 = function(u)
  ::L_0782::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0786 = function(u)
  ::L_0786::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0782", "L_075a")
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
  m688.dopo(u, 2, "L_07b3")
  m688.dopo(u, 160, "L_07b8")
  do return end
end

script.L_07b3 = function(u)
  ::L_07b3::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_07b8 = function(u)
  ::L_07b8::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_07bc = function(u)
  ::L_07bc::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_07c1 = function(u)
  ::L_07c1::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_075a")
  do return end
end

script.L_07cb = function(u, p)
  ::L_07cb::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_07da")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_07da = function(u)
  ::L_07da::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_072b")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_07e8 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_07e8::
  do return end
end

script.L_07e9 = function(u, p)
  ::L_07e9::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_07f4")
  m688.salva_modello(u)
  do return end
end

script.L_07f4 = function(u)
  ::L_07f4::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_07fb = function(u, p)
  ::L_07fb::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0800 = function(u, p)
  ::L_0800::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0807 = function(u, p)
  ::L_0807::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_080f = function(u)
  ::L_080f::
  m688.annulla_timer(u, "L_084e")
  m688.dopo(u, 20, "L_084e")
  do return end
end

script.L_0818 = function(u)
  ::L_0818::
  m688.annulla_timer(u, "L_0851")
  m688.dopo(u, 20, "L_0851")
  do return end
end

script.L_0821 = function(u)
  ::L_0821::
  m688.annulla_timer(u, "L_0854")
  m688.dopo(u, 20, "L_0854")
  do return end
end

script.L_082a = function(u)
  ::L_082a::
  m688.annulla_timer(u, "L_0857")
  m688.dopo(u, 20, "L_0857")
  do return end
end

script.L_0833 = function(u)
  ::L_0833::
  m688.annulla_timer(u, "L_085a")
  m688.dopo(u, 20, "L_085a")
  do return end
end

script.L_083c = function(u)
  ::L_083c::
  m688.annulla_timer(u, "L_085d")
  m688.dopo(u, 20, "L_085d")
  do return end
end

script.L_0845 = function(u)
  ::L_0845::
  m688.annulla_timer(u, "L_0860")
  m688.dopo(u, 20, "L_0860")
  do return end
end

script.L_084e = function(u)
  ::L_084e::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0851 = function(u)
  ::L_0851::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0854 = function(u)
  ::L_0854::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0857 = function(u)
  ::L_0857::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_085a = function(u)
  ::L_085a::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_085d = function(u)
  ::L_085d::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0860 = function(u)
  ::L_0860::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0863 = function(u)
  ::L_0863::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_086b = function(u, p)
  ::L_086b::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_086f = function(u)
  ::L_086f::
  m688.dopo(u, 60, "L_0879")
  do return end
end

script.L_0875 = function(u, p)
  ::L_0875::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0879 = function(u)
  ::L_0879::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_087f = function(u)
  ::L_087f::
  m688.dopo(u, 40, "L_0885")
  do return end
end

script.L_0885 = function(u)
  ::L_0885::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_088b = function(u)
  ::L_088b::
  m688.annulla_timer(u, "L_0879")
  m688.annulla_timer(u, "L_0885")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0895 = function(u, p)
  ::L_0895::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_089a = function(u, p)
  ::L_089a::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

return missione
