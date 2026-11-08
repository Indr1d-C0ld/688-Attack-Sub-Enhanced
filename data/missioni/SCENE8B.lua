-- SCENE8B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE8B",
  missione = 8,
  titolo = { it = "Festa a sorpresa", en = "SURPRISE PARTY" },
  lato = "urss",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 91.44, 152.4, 304.8 } },
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_06db" },
    esca = { gestore = "L_07d1" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0834",
    telegrafo = "L_0846",
    quota = "L_084b",
    timone = "L_0852",
    pilota_automatico_off = "L_08ae",
    periscopio = "L_08b6",
    carica_tubo_1 = "L_085a",
    carica_tubo_2 = "L_0863",
    carica_tubo_3 = "L_086c",
    carica_tubo_4 = "L_0875",
    carica_tubo_5 = "L_087e",
    carica_tubo_6 = "L_0887",
    carica_tubo_7 = "L_0890",
    schiera_cala = "L_08ba",
    schiera_recupera = "L_08ca",
    schiera_persa = "L_08d6",
    designa_bersaglio = "L_08e0",
    posizione = "L_08e5",
    antenna = "L_08c0",
    guida_e0 = "L_0807",
    guida_e1 = "L_080c",
    guida_e2 = "L_0816",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.171875, probabilita_originale = "11/64", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -22520.32, y = 4741.12 }, originale = { x = 4512, y = 3328 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -21335.04, y = -26076.16 }, originale = { x = 4544, y = 2496 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = "L_01b5" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01b5" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_040c", reazione = nil, arrivo = nil } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 45.72, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = 87710.72 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02b5", reazione = nil, arrivo = "L_01b0" } },
      },
      timer = { { dopo_s = 4000, unita = 0, gestore = "L_01b5" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0203" }, { dopo_s = 120, unita = 0, gestore = "L_01b8" }, { dopo_s = 60, unita = 1, gestore = "L_0221" }, { dopo_s = 200, unita = 2, gestore = "L_01ed" }, { dopo_s = 160, unita = 2, gestore = "L_01f4" }, { dopo_s = 300, unita = 3, gestore = "L_01ed" }, { dopo_s = 200, unita = 3, gestore = "L_01f4" }, { dopo_s = 400, unita = 5, gestore = "L_01ed" }, { dopo_s = 240, unita = 5, gestore = "L_01f4" }, { dopo_s = 500, unita = 6, gestore = "L_01ed" }, { dopo_s = 280, unita = 6, gestore = "L_01f4" }, { dopo_s = 120, unita = 7, gestore = "L_03d8" }, { dopo_s = 80, unita = 7, gestore = "L_01ed" } },
      variabili = { [1] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0 },
    },
  },
  briefing = {
    it = { "Compagni, è indispensabile impedire che i rinforzi americani raggiungano le forze NATO nel Mar di Norvegia. Pattugliate le rotte marittime e distruggete tutte le unità NATO che incontrate!" },
    en = { "COMRADES, IT IS IMPERATIVE THAT YOU PREVENT AMERICAN SUPPORT FROM REACHING NATO FORCES IN THE NORWEGIAN SEA. PATROL THE SEA LANES AND DESTROY ALL NATO VESSELS YOU ENCOUNTER!" },
  },
  esiti = {
    [1] = { it = "CODICE: 8-1-BKB\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 8-1-BKB\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [2] = { it = "CODICE: 8-2-BKB\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 8-2-BKB\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [5] = { it = "CODICE: 8-5-BKB\nCompagni! Rendiamo onore al Leninskii Komsomol per la sua prestazione senza precedenti in Atlantico! L'equipaggio del Leninskii Komsomol sia d'esempio per tutti i nostri valorosi combattenti!", en = "CODE: 8-5-BKB\nCOMRADES! WE HAIL THE LENINSKII KOMSOMOL FOR HER UNPRECEDENTED PERFORMANCE IN THE ATLANTIC! LET THE CREW OF THE LENINSKII KOMSOMOL BE AN EXAMPLE TO ALL OF OUR BRAVE FIGHTING MEN!" },
    [6] = { it = "CODICE: 8-6-BKB\nCompagni, gli imperialisti hanno scacciato il vostro attacco come fosse un moscerino. Forse il Kapitan del Leninskii Komsomol non è adeguatamente motivato? Se la prestazione di questo pietoso equipaggio non migliorerà, questo Kapitan si pentirà di aver avuto bisogno di motivazioni. Questo sottomarino rientri immediatamente in porto, ma nessuna parata accoglierà il Leninskii Komsomol, nessun bambino saluterà il ritorno del padre e nessuna giovane donna, calda e morbida, desidererà ardentemente gli uomini di questo equipaggio. Me ne occuperò io!", en = "CODE: 8-6-BKB\nCOMRADES, THE IMPERIALISTS BRUSHED AWAY YOUR ATTACK AS IF IT WERE A TINY SEA GNAT. PERHAPS THE KAPITAN OF THE LENINSKII KOMSOMOL IS NOT PROPERLY MOTIVATED? IF THE PERFORMANCE OF THIS PITIFUL CREW DOES NOT IMPROVE, THIS KAPITAN SHALL WISH HE DID NOT REQUIRE MOTIVATION. THIS SUBMARINE SHALL RETURN TO PORT IMMEDIATELY, BUT NO PARADE SHALL GREET THE LENINSKII KOMSOMOL, NO CHILDREN SHALL WAVE TO THEIR FATHER'S RETURN, AND NO WARM SOFT YOUNG WOMEN SHALL LONGINGLY DESIRE MEN OF THIS CREW. I WILL SEE TO THAT!" },
    [7] = { it = "CODICE: 8-7-BKB\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di determinare la posizione del proprio battello. Se leggesse le carte nautiche con la stessa attenzione con cui legge le etichette della sua vodka preferita, avremmo risultati accettabili. Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 8-7-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF HIS OWN VESSEL. IF HE WOULD READ HIS CHARTS AS CAREFULLY AS HE READS THE LABELS OF HIS FAVORITE VODKA, WE WOULD HAVE ACCEPTABLE RESULTS. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
    [19] = { it = "CODICE: 8-19-BKB\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di identificare i suoi bersagli prima di aprire il fuoco! Rientri immediatamente in porto.", en = "CODE: 8-19-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE IDENTITY OF HIS TARGETS BEFORE FIRING! RETURN TO PORT IMMEDIATELY." },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_01b0 = function(u)
  ::L_01b0::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_01b5 = function(u)
  ::L_01b5::
  m688.messaggio(u, 106)
  do return end
end

script.L_01b8 = function(u)
  ::L_01b8::
  if m688.unita_esiste(u, 2) then goto L_01d9 end
  if m688.unita_esiste(u, 3) then goto L_01d9 end
  if m688.unita_esiste(u, 4) then goto L_01d9 end
  if m688.unita_esiste(u, 5) then goto L_01d9 end
  if m688.unita_esiste(u, 6) then goto L_01d9 end
  if m688.unita_esiste(u, 7) then goto L_01d9 end
  m688.messaggio(u, 105)
  do return end
  ::L_01d9::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -96007.68) then goto L_01e7 end
  m688.messaggio(u, 107)
  do return end
  ::L_01e7::
  m688.dopo(u, 20, "L_01b8")
  do return end
end

script.L_01ed = function(u)
  ::L_01ed::
  m688.marca_contatti(u)
  m688.dopo(u, 40, "L_01ed")
  do return end
end

script.L_01f4 = function(u)
  ::L_01f4::
  if m688.flag_scena_bit(u, 1) then goto L_01fd end
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  ::L_01fd::
  m688.dopo(u, 50, "L_0211")
  do return end
end

script.L_0203 = function(u)
  ::L_0203::
  if m688.manomesso(u) then goto L_0207 end
  do return end
  ::L_0207::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0211 = function(u)
  ::L_0211::
  if m688.flag_scena_bit(u, 1) then goto L_021b end
  m688.impulso_sonar(u)
  m688.virata_relativa(u, -180)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  ::L_021b::
  m688.dopo(u, 320, "L_01f4")
  do return end
end

script.L_0221 = function(u)
  ::L_0221::
  m688.nuovo_caso(u)
  m688.posizione(u, -22520.32, 4741.12)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_02ae end
  m688.posizione(u, -45040.64, 11852.8)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_02ae end
  m688.posizione(u, -66375.68, 5926.4)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_02ae end
  m688.posizione(u, -66375.68, -13038.08)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_02ae end
  m688.posizione(u, -62819.84, -29632)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_02ae end
  m688.posizione(u, -41484.8, -30817.28)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_02ae end
  m688.posizione(u, -21335.04, -26076.16)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_02ae end
  m688.posizione(u, -17779.2, -9482.24)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_02ae end
  m688.posizione(u, -35558.4, -10667.52)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_02ae end
  m688.posizione(u, -52152.32, -18964.48)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_02ae end
  m688.posizione(u, -54522.88, -3555.84)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_02ae end
  m688.posizione(u, -42670.08, -21335.04)
  m688.rotta_ora(u, 300)
  ::L_02ae::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0221")
  do return end
end

script.L_02b5 = function(u)
  ::L_02b5::
  if m688.bersaglio_nave(u) then goto L_02fe end
  if m688.bersaglio_id(u, 0) then goto L_0310 end
  if m688.bersaglio_arma(u) then goto L_02c5 end
  ::L_02c0::
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  do return end
  ::L_02c5::
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  if m688.bersaglio_mi_punta(u) then goto L_02cd end
  do return end
  ::L_02cd::
  m688.flag_scena(u, 0)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 289.56, 1)
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_02de end
  do return end
  ::L_02de::
  if m688.bersaglio_vicino(u, 1.28) then goto L_02e4 end
  do return end
  ::L_02e4::
  m688.dopo(u, 20, "L_02f1")
  if m688.variabile_uguale(u, 7, 0) then goto L_039a end
  do return end
  ::L_02fe::
  if m688.flag_scena_uguale(u, 1) then goto L_02c0 end
  m688.vai_al_bersaglio(u, 60, "L_03ae")
  m688.flag_scena(u, 1)
  m688.telegrafo(u, 0.5)
  goto L_02c0
  ::L_0310::
  if m688.traccia_solida(u) then goto L_0344 end
  m688.annulla_timer(u, "L_03d8")
  m688.annulla_timer(u, "L_03e2")
  m688.annulla_timer(u, "L_03ed")
  m688.annulla_timer(u, "L_03fc")
  m688.ferma_navigazione(u)
  m688.flag_scena(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.7031)
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.salva_modello(u)
  m688.dopo(u, 160, "L_0333")
  do return end
  ::L_0344::
  m688.annulla_timer(u, "L_0333")
  m688.annulla_timer(u, "L_03e2")
  m688.annulla_timer(u, "L_03ed")
  m688.annulla_timer(u, "L_03fc")
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, 5.6) then goto L_0382 end
  if m688.variabile_uguale(u, 4, 1) then goto L_0377 end
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  script.L_068f(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e6")
  do return end
  ::L_0377::
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  do return end
  ::L_0382::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_03d8")
  if m688.variabile_uguale(u, 5, 3) then goto L_0399 end
  goto L_04cb
  ::L_0399::
  do return end
  ::L_039a::
  m688.variabile(u, 7, 1)
  m688.dopo(u, 200, "L_03a8")
  goto L_07d1
  ::L_045c::
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
  ::L_07d1::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_07cd", "L_07a5")
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
  m688.dopo(u, 2, "L_07fe")
  m688.dopo(u, 160, "L_0803")
  do return end
end

script.L_02f1 = function(u)
  ::L_02f1::
  m688.virata_relativa(u, -180)
  m688.quota(u, 45.72, 1)
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  do return end
end

script.L_0310 = function(u)
  ::L_0310::
  if m688.traccia_solida(u) then goto L_0344 end
  m688.annulla_timer(u, "L_03d8")
  m688.annulla_timer(u, "L_03e2")
  m688.annulla_timer(u, "L_03ed")
  m688.annulla_timer(u, "L_03fc")
  m688.ferma_navigazione(u)
  m688.flag_scena(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.7031)
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.salva_modello(u)
  m688.dopo(u, 160, "L_0333")
  do return end
  ::L_0344::
  m688.annulla_timer(u, "L_0333")
  m688.annulla_timer(u, "L_03e2")
  m688.annulla_timer(u, "L_03ed")
  m688.annulla_timer(u, "L_03fc")
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, 5.6) then goto L_0382 end
  if m688.variabile_uguale(u, 4, 1) then goto L_0377 end
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  script.L_068f(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e6")
  do return end
  ::L_0377::
  m688.ingaggia(u, 40, "L_0310", "L_0335")
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  do return end
  ::L_0382::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_03d8")
  if m688.variabile_uguale(u, 5, 3) then goto L_0399 end
  goto L_04cb
  ::L_0399::
  do return end
  ::L_045c::
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
end

script.L_0333 = function(u)
  ::L_0333::
  m688.impulso_sonar(u)
  do return end
end

script.L_0335 = function(u)
  ::L_0335::
  m688.annulla_timer(u, "L_0333")
  m688.dopo(u, 4, "L_03d8")
  m688.flag_scena(u, 0)
  m688.comportamento(u, "L_02b5")
  m688.salva_modello(u)
  do return end
end

script.L_03a8 = function(u)
  ::L_03a8::
  m688.variabile(u, 7, 0)
  do return end
end

script.L_03ae = function(u)
  ::L_03ae::
  m688.flag_scena(u, 0)
  m688.vai_a(u, -47411.2, 87710.72, 120, "L_01b0")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_03c0")
  do return end
end

script.L_03c0 = function(u)
  ::L_03c0::
  if m688.flag_scena_uguale(u, 0) then goto L_03c5 end
  do return end
  ::L_03c5::
  m688.telegrafo(u, 0.0195)
  m688.quota(u, 6.096, 0.9961)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_03d4")
  do return end
end

script.L_03d4 = function(u)
  ::L_03d4::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_03d8 = function(u)
  ::L_03d8::
  m688.telegrafo(u, 0.0078)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_03e2")
  do return end
end

script.L_03e2 = function(u)
  ::L_03e2::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 90, "L_03ed")
  do return end
end

script.L_03ed = function(u)
  ::L_03ed::
  m688.virata_relativa(u, -179.96)
  m688.quota(u, 182.88, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 90, "L_03fc")
  do return end
end

script.L_03fc = function(u)
  ::L_03fc::
  m688.riprendi_navigazione(u)
  m688.quota(u, 45.72, 0.5859)
  m688.telegrafo(u, 0.625)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_03d8")
  do return end
end

script.L_040c = function(u)
  ::L_040c::
  if m688.bersaglio_id(u, 0) then goto L_0439 end
  if m688.bersaglio_nave(u) then goto L_041c end
  if m688.bersaglio_arma(u) then goto L_0536 end
  m688.comportamento(u, "L_040c")
  m688.salva_modello(u)
  do return end
  ::L_041c::
  if m688.unita_id(u, 2) then goto L_042b end
  if m688.unita_id(u, 3) then goto L_042b end
  if m688.flag_scena_bit(u, 1) then goto L_042b end
  m688.accosta_verso_bersaglio(u)
  ::L_042b::
  m688.comportamento(u, "L_040c")
  m688.salva_modello(u)
  do return end
  ::L_0439::
  if m688.unita_id(u, 2) then goto L_0451 end
  if m688.unita_id(u, 3) then goto L_0451 end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_0430")
  m688.dopo(u, 90, "L_0430")
  ::L_0451::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_0439", "L_0434")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_045d end
  ::L_045c::
  do return end
  ::L_045d::
  if m688.bersaglio_vicino(u, 2.4) then goto L_04c4 end
  if m688.bersaglio_vicino(u, 5) then goto L_04cb end
  if m688.variabile_uguale(u, 4, 1) then goto L_047d end
  m688.variabile_piu(u, 4)
  script.L_068f(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e6")
  do return end
  ::L_047d::
  if m688.variabile_uguale(u, 3, 1) then goto L_045c end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_04f4", "L_04f4")
  m688.modello_tipo(u, 1)
  m688.quota_ora(u, 0)
  m688.modello_rateo_quota(u, 0.3429)
  m688.modello_virata(u, 19.8)
  m688.modello_accelerazione(u, 2.5714)
  m688.modello_avanti_tutta(u, 82.2857)
  m688.schiera_trainata(u, 1)
  m688.flag_scena(u, 0)
  m688.modello_rumore(u, 0)
  m688.modello_sonar(u, 35)
  m688.modello_integrita(u, 20)
  m688.modello_quota_max(u, 0)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_04ea")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_052f")
  do return end
  ::L_04c4::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
  ::L_0536::
  m688.comportamento(u, "L_040c")
  m688.salva_modello(u)
  if m688.bersaglio_mi_punta(u) then goto L_053e end
  do return end
  ::L_053e::
  if m688.traccia_solida(u) then goto L_0542 end
  do return end
  ::L_0542::
  if m688.bersaglio_vicino(u, 1.28) then goto L_0548 end
  do return end
  ::L_0548::
  m688.nuovo_caso(u)
  if m688.caso(u, 0.2539) then goto L_054e end
  do return end
  ::L_054e::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 19)
  do return end
end

script.L_0430 = function(u)
  ::L_0430::
  m688.flag_scena_spegni(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0434 = function(u)
  ::L_0434::
  m688.comportamento(u, "L_040c")
  m688.salva_modello(u)
  do return end
end

script.L_0439 = function(u)
  ::L_0439::
  if m688.unita_id(u, 2) then goto L_0451 end
  if m688.unita_id(u, 3) then goto L_0451 end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_0430")
  m688.dopo(u, 90, "L_0430")
  ::L_0451::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_0439", "L_0434")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_045d end
  ::L_045c::
  do return end
  ::L_045d::
  if m688.bersaglio_vicino(u, 2.4) then goto L_04c4 end
  if m688.bersaglio_vicino(u, 5) then goto L_04cb end
  if m688.variabile_uguale(u, 4, 1) then goto L_047d end
  m688.variabile_piu(u, 4)
  script.L_068f(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e6")
  do return end
  ::L_047d::
  if m688.variabile_uguale(u, 3, 1) then goto L_045c end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_04f4", "L_04f4")
  m688.modello_tipo(u, 1)
  m688.quota_ora(u, 0)
  m688.modello_rateo_quota(u, 0.3429)
  m688.modello_virata(u, 19.8)
  m688.modello_accelerazione(u, 2.5714)
  m688.modello_avanti_tutta(u, 82.2857)
  m688.schiera_trainata(u, 1)
  m688.flag_scena(u, 0)
  m688.modello_rumore(u, 0)
  m688.modello_sonar(u, 35)
  m688.modello_integrita(u, 20)
  m688.modello_quota_max(u, 0)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_04ea")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_052f")
  do return end
  ::L_04c4::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
end

script.L_04e2 = function(u)
  ::L_04e2::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_04e6 = function(u)
  ::L_04e6::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_04ea = function(u)
  ::L_045c::
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
  ::L_04ea::
  if m688.bersaglio_id(u, 0) then goto L_0505 end
  m688.comportamento(u, "L_04ea")
  m688.salva_modello(u)
  do return end
  ::L_0505::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_0512 end
  m688.impulso_sonar(u)
  ::L_050a::
  m688.ingaggia(u, 40, "L_0505", "L_04f4")
  m688.salva_modello(u)
  do return end
  ::L_0512::
  if m688.bersaglio_vicino(u, 3.2) then goto L_051a end
  goto L_050a
  ::L_051a::
  if m688.variabile_uguale(u, 5, 3) then goto L_050a end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_07cd")
  m688.salva_modello(u)
  goto L_04cb
end

script.L_04f4 = function(u)
  ::L_04f4::
  m688.dopo(u, 30, "L_04fa")
  do return end
end

script.L_04fa = function(u)
  ::L_04fa::
  m688.rotta(u, nil)
  m688.rateo_virata(u, -7.2)
  m688.comportamento(u, "L_04ea")
  m688.salva_modello(u)
  do return end
end

script.L_0505 = function(u)
  ::L_045c::
  do return end
  ::L_04cb::
  if m688.variabile_uguale(u, 5, 3) then goto L_045c end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_071e(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e2")
  do return end
  ::L_0505::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_0512 end
  m688.impulso_sonar(u)
  ::L_050a::
  m688.ingaggia(u, 40, "L_0505", "L_04f4")
  m688.salva_modello(u)
  do return end
  ::L_0512::
  if m688.bersaglio_vicino(u, 3.2) then goto L_051a end
  goto L_050a
  ::L_051a::
  if m688.variabile_uguale(u, 5, 3) then goto L_050a end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_07cd")
  m688.salva_modello(u)
  goto L_04cb
end

script.L_052f = function(u)
  ::L_052f::
  m688.variabile(u, 3, 0)
  m688.togli_unita(u)
  do return end
end

script.L_068f = function(u)
  ::L_068f::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_0737(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_classe(u, "spearfish")
  m688.modello_difesa(u, 40)
  m688.telegrafo(u, 0.7812)
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.quota_ora(u, 0)
  m688.collega(u, 1)
  m688.quota(u, 365.76, 0.5859)
  m688.nuova_unita(u)
  m688.dopo(u, 20, "L_06bd")
  m688.dopo(u, 10, "L_07c6")
  m688.dopo(u, 240, "L_07cd")
  do return end
end

script.L_06bd = function(u)
  ::L_06bd::
  m688.collega(u, 0)
  goto L_06c3
  ::L_06c3::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06cf")
  do return end
end

script.L_06cf = function(u)
  ::L_06cf::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06c3")
  do return end
end

script.L_06c3 = function(u)
  ::L_06c3::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06cf")
  do return end
end

script.L_06db = function(u)
  ::L_06db::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_07cd", "L_07a5")
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
  m688.dopo(u, 150, "L_0734")
  m688.dopo(u, 170, "L_076b")
  m688.dopo(u, 240, "L_07cd")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_071e = function(u)
  ::L_071e::
  script.L_0737(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_0734")
  m688.dopo(u, 190, "L_076b")
  m688.dopo(u, 280, "L_07cd")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0734 = function(u)
  ::L_0734::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0737 = function(u)
  ::L_0737::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_07cd", "L_07a5")
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

script.L_076b = function(u)
  ::L_076b::
  m688.annulla_timer(u, "L_076b")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_07a5 = function(u)
  ::L_07a5::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0776")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_07c6")
  m688.dopo(u, 10, "L_07b9")
  do return end
end

script.L_0776 = function(u)
  ::L_0776::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_06c3")
  m688.annulla_timer(u, "L_06cf")
  if m688.traccia_solida(u) then goto L_0789 end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 40, "L_0776", "L_07a5")
  m688.salva_modello(u)
  do return end
  ::L_0789::
  if m688.bersaglio_vicino(u, 10) then goto L_0793 end
  m688.comportamento(u, "L_0776")
  m688.salva_modello(u)
  do return end
  ::L_0793::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_07b9")
  m688.annulla_timer(u, "L_07a5")
  m688.insegui(u, 0, "L_07cd", "L_07a5")
  m688.salva_modello(u)
  do return end
end

script.L_07b9 = function(u)
  ::L_07b9::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_06c3")
  do return end
end

script.L_07c6 = function(u)
  ::L_07c6::
  m688.impulso_sonar(u)
  m688.dopo(u, 16, "L_07c6")
  do return end
end

script.L_07cd = function(u)
  ::L_07cd::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_07d1 = function(u)
  ::L_07d1::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_07cd", "L_07a5")
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
  m688.dopo(u, 2, "L_07fe")
  m688.dopo(u, 160, "L_0803")
  do return end
end

script.L_07fe = function(u)
  ::L_07fe::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0803 = function(u)
  ::L_0803::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0807 = function(u)
  ::L_0807::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_080c = function(u)
  ::L_080c::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_07a5")
  do return end
end

script.L_0816 = function(u, p)
  ::L_0816::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_0825")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0825 = function(u)
  ::L_0825::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0776")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0833 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0833::
  do return end
end

script.L_0834 = function(u, p)
  ::L_0834::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_083f")
  m688.salva_modello(u)
  do return end
end

script.L_083f = function(u)
  ::L_083f::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0846 = function(u, p)
  ::L_0846::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_084b = function(u, p)
  ::L_084b::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0852 = function(u, p)
  ::L_0852::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_085a = function(u)
  ::L_085a::
  m688.annulla_timer(u, "L_0899")
  m688.dopo(u, 20, "L_0899")
  do return end
end

script.L_0863 = function(u)
  ::L_0863::
  m688.annulla_timer(u, "L_089c")
  m688.dopo(u, 20, "L_089c")
  do return end
end

script.L_086c = function(u)
  ::L_086c::
  m688.annulla_timer(u, "L_089f")
  m688.dopo(u, 20, "L_089f")
  do return end
end

script.L_0875 = function(u)
  ::L_0875::
  m688.annulla_timer(u, "L_08a2")
  m688.dopo(u, 20, "L_08a2")
  do return end
end

script.L_087e = function(u)
  ::L_087e::
  m688.annulla_timer(u, "L_08a5")
  m688.dopo(u, 20, "L_08a5")
  do return end
end

script.L_0887 = function(u)
  ::L_0887::
  m688.annulla_timer(u, "L_08a8")
  m688.dopo(u, 20, "L_08a8")
  do return end
end

script.L_0890 = function(u)
  ::L_0890::
  m688.annulla_timer(u, "L_08ab")
  m688.dopo(u, 20, "L_08ab")
  do return end
end

script.L_0899 = function(u)
  ::L_0899::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_089c = function(u)
  ::L_089c::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_089f = function(u)
  ::L_089f::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_08a2 = function(u)
  ::L_08a2::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_08a5 = function(u)
  ::L_08a5::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_08a8 = function(u)
  ::L_08a8::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_08ab = function(u)
  ::L_08ab::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_08ae = function(u)
  ::L_08ae::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_08b6 = function(u, p)
  ::L_08b6::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08ba = function(u)
  ::L_08ba::
  m688.dopo(u, 60, "L_08c4")
  do return end
end

script.L_08c0 = function(u, p)
  ::L_08c0::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08c4 = function(u)
  ::L_08c4::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_08ca = function(u)
  ::L_08ca::
  m688.dopo(u, 40, "L_08d0")
  do return end
end

script.L_08d0 = function(u)
  ::L_08d0::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_08d6 = function(u)
  ::L_08d6::
  m688.annulla_timer(u, "L_08c4")
  m688.annulla_timer(u, "L_08d0")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_08e0 = function(u, p)
  ::L_08e0::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08e5 = function(u, p)
  ::L_08e5::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

return missione
