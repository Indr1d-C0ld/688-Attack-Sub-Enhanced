-- SCENE3B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE3B",
  missione = 3,
  titolo = { it = "Operazioni subacquee", en = "SUB OPS" },
  lato = "urss",
  teatro = "barents",
  posa = { metodo = "scala_di_gioco", centro = { lat = 70.1500, lon = 35.0000 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 304.8, 457.2 } },
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "esca" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 45.7143, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0631" },
    esca = { gestore = "L_0700" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0763",
    telegrafo = "L_0775",
    quota = "L_077a",
    timone = "L_0781",
    pilota_automatico_off = "L_07dd",
    periscopio = "L_07e5",
    carica_tubo_1 = "L_0789",
    carica_tubo_2 = "L_0792",
    carica_tubo_3 = "L_079b",
    carica_tubo_4 = "L_07a4",
    carica_tubo_5 = "L_07ad",
    carica_tubo_6 = "L_07b6",
    carica_tubo_7 = "L_07bf",
    schiera_cala = "L_07e9",
    schiera_recupera = "L_07f9",
    schiera_persa = "L_0805",
    designa_bersaglio = "L_080f",
    posizione = "L_0814",
    antenna = "L_07ef",
    guida_e0 = "L_0736",
    guida_e1 = "L_073b",
    guida_e2 = "L_0745",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.0255126953125, probabilita_originale = "209/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0249328613281, probabilita_originale = "817/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0249328613281, probabilita_originale = "817/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0248413085938, probabilita_originale = "407/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0248413085938, probabilita_originale = "407/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0243530273438, probabilita_originale = "399/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0243530273438, probabilita_originale = "399/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0243530273438, probabilita_originale = "399/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0242767333984, probabilita_originale = "1591/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0242767333984, probabilita_originale = "1591/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0242767333984, probabilita_originale = "1591/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0242767333984, probabilita_originale = "1591/65536", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.024169921875, probabilita_originale = "99/4096", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.024169921875, probabilita_originale = "99/4096", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.024169921875, probabilita_originale = "99/4096", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0237121582031, probabilita_originale = "777/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0236206054688, probabilita_originale = "387/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0230712890625, probabilita_originale = "189/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 32002.56, y = -55708.16 }, originale = { x = 5984, y = 1696 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0120849609375, probabilita_originale = "99/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0120849609375, probabilita_originale = "99/8192", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 52152.32, y = -52152.32 }, originale = { x = 6528, y = 1792 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0118103027344, probabilita_originale = "387/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0118103027344, probabilita_originale = "387/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 33187.84, y = -47411.2 }, originale = { x = 6016, y = 1920 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0118103027344, probabilita_originale = "387/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0118103027344, probabilita_originale = "387/32768", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 34373.12, y = -56893.44 }, originale = { x = 6048, y = 1664 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 15408.64, y = -46225.92 }, originale = { x = 5536, y = 1952 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -54522.88 }, originale = { x = 5728, y = 1728 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02e2" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.0115356445312, probabilita_originale = "189/16384", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 39114.24, y = -67560.96 }, originale = { x = 6176, y = 1376 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -11852.8, y = -48596.48 }, originale = { x = 4800, y = 1888 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "trafalgar", tipo_originale = 34, nome_originale = "TRAFALGAR CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 34373.12, y = -40299.52 }, originale = { x = 6048, y = 2112 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 8, sonar = 22, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0325", reazione = nil, arrivo = nil } },
      { id = 3, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 13038.08, y = -47411.2 }, originale = { x = 5472, y = 1920 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 4, classe = "koni", tipo_originale = 8, nome_originale = "KONI CLASS FRIGATE", categoria = "nave", schieramento = "urss", flag = 82, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 2370.56, y = -37928.96 }, originale = { x = 5184, y = 2176 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -34373.12 }, originale = { x = 4960, y = 2272 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 6, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 66375.68, y = -77043.2 }, originale = { x = 6912, y = 1120 },
        quota_m = 0, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 20.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 50, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 66375.68, y = -77043.2 }, { x = -5926.4, y = -34373.12 }, { x = 66375.68, y = -77043.2 } }, rotta_ciclica = true,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02d7" } },
      { id = 7, classe = "charlie2", tipo_originale = 30, nome_originale = "CHARLIE II CLASS GUIDED MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 52152.32, y = -69931.52 }, originale = { x = 6528, y = 1312 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 29632, y = -54522.88 }, { x = -23705.6, y = 15408.64 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ed" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02c9" }, { dopo_s = 100, unita = 0, gestore = "L_028b" }, { dopo_s = 30, unita = 1, gestore = "L_04c7" }, { dopo_s = 40, unita = 2, gestore = "L_0305" }, { dopo_s = 30, unita = 2, gestore = "L_031e" } },
      variabili = { [1] = 0, [2] = 4, [3] = 0, [4] = 0, [5] = 0 },
    },
  },
  briefing = {
    it = { "Compagni, le prove di manovra sono concluse. Il Leninskii Komsomol esegua le procedure di ricerca lungo la costa di Poljarnyj. Se scoprite degli intrusi, avvicinatevi e attivate il sonar, quindi seguite l'intruso fuori dall'area. Se non viene rilevato traffico non autorizzato, rientrate in porto come previsto." },
    en = { "COMRADES, TESTING MANEUVERS ARE NOW COMPLETE. THE LENINSKII KOMSOMOL IS INSTRUCTED TO EXECUTE SEARCH PROCEDURES ALONG THE POLYARNYY COASTLINE. SHOULD YOU DISCOVER INTRUDERS, MOVE CLOSE TO THE INTRUDER AND ACTIVATE SONAR. FOLLOW THE INTRUDER OUT OF THE AREA. IF NO UNAUTHORIZED TRAFFIC IS DETECTED, RETURN TO PORT AS SCHEDULED." },
  },
  esiti = {
    [1] = { it = "CODICE: 3-1-BKB\nIl Leninskii Komsomol è andato perduto in mare durante un pattugliamento nei pressi di Poljarnyj. Non dimenticheremo mai il coraggio e il valore di questi giovani valorosi che hanno dato la vita per la Patria nel nome del Soviet Supremo!", en = "CODE: 3-1-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE PATROLLING NEAR POLYARNYY. WE SHALL NEVER FORGET THE COURAGE AND VALOR OF THESE BRAVE YOUNG MEN WHO GAVE UP THEIR LIVES FOR THE MOTHERLAND IN THE NAME OF THE SUPREME SOVIET!" },
    [2] = { it = "CODICE: 3-2-BKB\nCompagni, il Leninskii Komsomol è stato attaccato ed è andato perduto in mare mentre difendeva il porto di Severomorsk. La Marina sovietica non avrà pace finché gli assassini non saranno identificati e la vendetta compiuta. A Poljarnyj sarà eretta una statua, perché nessuno dimentichi il coraggio di questo valoroso equipaggio!", en = "CODE: 3-2-BKB\nCOMRADES, THE LENINSKII KOMSOMOL WAS ATTACKED AND LOST AT SEA WHILE PROTECTING THE PORT OF SEVEROMORSK. THE SOVIET NAVY SHALL NOT SLEEP UNTIL THE KILLERS HAVE BEEN IDENTIFIED AND RETRIBUTION HAS BEEN PAID. A STATUE WILL BE ERECTED IN POLYARNYY SO THAT NONE SHALL FORGET THE BRAVERY OF THIS FINE CREW!" },
    [3] = { it = "CODICE: 3-3-BKB\nNon capisco perché il Kapitan del Leninskii Komsomol si rifiuti di eseguire i miei ordini. Aveva l'ordine di sorvegliare la costa alla ricerca di intrusi, e invece si è allontanato lasciando indifeso il suo porto. O forse esegue le istruzioni di un corrotto governo occidentale e abbandona il suo posto di proposito. Non le faccio tanto credito. Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non sia entrato per errore in un porto nemico! Rientri immediatamente: disporrò una scorta che la guidi in porto, così da essere certo che non si perda.", en = "CODE: 3-3-BKB\nI DO NOT UNDERSTAND WHY THE KAPITAN OF THE LENINSKII KOMSOMOL REFUSES TO FOLLOW MY ORDERS. YOU WERE INSTRUCTED TO OBSERVE THE COASTLINE IN SEARCH OF INTRUDERS AND INSTEAD YOU DRIFT AWAY AND LEAVE YOUR HOME PORT UNPROTECTED. OR PERHAPS YOU FOLLOW THE INSTRUCTIONS OF A CORRUPT WESTERN GOVERNMENT AND LEAVE YOUR POST INTENTIONALLY. I DO NOT GIVE YOU THAT MUCH CREDIT. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SAIL INTO AN ENEMY PORT BY MISTAKE! RETURN IMMEDIATELY, I WILL ARRANGE AN ESCORT TO DIRECT YOU INTO PORT TO ENSURE THAT YOU DO NOT GET LOST." },
    [4] = { it = "CODICE: 3-4-BKB\nCongratulazioni, compagni. Il Leninskii Komsomol ha messo in sicurezza questo porto sovietico e ha dimostrato di essere, insieme alla Marina sovietica, un avversario temibile. Immagino che l'equipaggio abbia buone ragioni per aver attaccato i nemici dell'Unione Sovietica, ma sono questioni da discutere davanti a una vodka ghiacciata. Severomorsk dà il benvenuto al Leninskii Komsomol!", en = "CODE: 3-4-BKB\nCONGRATULATIONS, COMRADES. THE LENINSKII KOMSOMOL HAS SECURED THIS SOVIET PORT AND PROVEN THEMSELVES AND THE SOVIET NAVY A WORTHY ADVERSARY. I EXPECT THE CREW HAS GOOD REASONS FOR ATTACKING THE ENEMIES OF THE SOVIET UNION, BUT THESE ARE MATTERS TO BE DISCUSSED OVER CHILLED VODKA. SEVEROMORSK WELCOMES THE LENINSKII KOMSOMOL!" },
    [5] = { it = "CODICE: 3-5-BKB\nSembra che il Leninskii Komsomol abbia concluso il pattugliamento senza incidenti. Le nostre navi e i nostri sottomarini hanno lasciato il porto e hanno raggiunto il mare aperto. Pare che oggi i decadenti occidentali non abbiano osato violare il nostro Stato sovrano: saranno a casa a consumare sostanze proibite sognando la caduta dello Stato socialista. Rientri in porto come previsto, fiero del suo contributo alla difesa del Soviet Supremo.", en = "CODE: 3-5-BKB\nIT WOULD SEEM THAT THE LENINSKII KOMSOMOL HAS COMPLETED THE PATROL WITHOUT INCIDENT. OUR SHIPS AND SUBMARINES HAVE LEFT PORT AND SUCCESSFULLY MADE IT OUT TO SEA. IT WOULD APPEAR THAT THE DECADENT WESTERNERS WOULD NOT DARE INTRUDE ON OUR SOVEREIGN STATE ON THIS DAY. THEY MUST BE IN THEIR HOMES CONSUMING CONTROLLED SUBSTANCES AND DREAMING OF THE FALL OF THE SOCIALIST STATE. RETURN TO PORT AS SCHEDULED AND BE PROUD OF YOUR CONTRIBUTION TO THE PROTECTION OF THE SUPREME SOVIET." },
    [6] = { it = "CODICE: 3-6-BKB\nL'equipaggio del Leninskii Komsomol è pietoso e indegno di chiamarsi marinai sovietici. Un intruso è stato individuato al largo della costa di Poljarnyj e il Leninskii Komsomol gli è passato accanto alla cieca! L'intruso ha seguito uno dei nostri sottomarini fino in mare aperto. Il Leninskii Komsomol ha tradito la Marina. Quando avrò finito con questo equipaggio, le madri piangeranno i loro figli!!", en = "CODE: 3-6-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL IS PITIFUL AND UNWORTHY OF CALLING THEMSELVES SOVIET SAILORS. AN INTRUDER WAS IDENTIFIED OFF THE POLYARNYY COAST AND THE LENINSKII KOMSOMOL SAILED BLINDLY PAST! THIS INTRUDER FOLLOWED ONE OF OUR SUBMARINES OUT TO SEA. THE LENINSKII KOMSOMOL HAS FAILED THE NAVY. WHEN I AM THROUGH WITH THIS CREW, MOTHERS WILL WEEP FOR THEIR CHILDREN!!" },
    [7] = { it = "CODICE: 3-7-BKB\nL'equipaggio del Leninskii Komsomol è pietoso e indegno di chiamarsi marinai sovietici. Un sottomarino nemico ha distrutto uno dei nostri sottomarini. Ritengo questo Kapitan pienamente responsabile della perdita di vite sovietiche, per non parlare di sofisticate apparecchiature sovietiche. Quando avrò finito con questo equipaggio, le madri piangeranno i loro figli!!", en = "CODE: 3-7-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL IS PITIFUL AND UNWORTHY OF CALLING THEMSELVES SOVIET SAILORS. AN ENEMY SUBMARINE HAS DESTROYED ONE OF OUR SUBMARINES. I CONSIDER THIS KAPITAN FULLY RESPONSIBLE FOR THE LOSS OF SOVIET LIVES NOT TO MENTION SOPHISTICATED SOVIET EQUIPMENT. WHEN I AM THROUGH WITH THIS CREW, MOTHERS WILL WEEP FOR THEIR CHILDREN!!" },
    [8] = { it = "CODICE: 3-8-BKB\nCompagni, il Partito e il popolo della società sovietica sono fieri delle imprese del Leninskii Komsomol. Avete fatto scappare il sottomarino britannico come un cucciolo di foca verso il seno della madre! Il porto è ora sicuro per il traffico sovietico. Tanto coraggio e tanta dedizione non devono restare senza ricompensa: gli ufficiali godranno di due giorni di licenza a terra e l'equipaggio di un'intera giornata libera dal servizio! Severomorsk saluta il Leninskii Komsomol!", en = "CODE: 3-8-BKB\nCOMRADES, THE PARTY AND PEOPLE OF THE SOVIET SOCIETY ARE PROUD OF THE LENINSKII KOMSOMOL'S ACHIEVEMENTS. YOU SENT THE BRITISH SUBMARINE SWIMMING AWAY LIKE A BABY SEAL AFTER ITS MOTHER'S BOSOM! THE PORT IS NOW SAFE FOR SOVIET SHIPPING. SUCH BRAVERY AND DEDICATION SHOULD NOT BE LEFT UNREWARDED. THE OFFICERS SHALL ENJOY TWO DAYS OF SHORE LEAVE AND THE CREW CAN EXPECT A FULL DAY ASHORE, DUTY FREE! SEVEROMORSK SALUTES THE LENINSKII KOMSOMOL!" },
    [19] = { it = "CODICE: 3-19-BKB\nLe azioni del Leninskii Komsomol sono una vergogna per questa Marina. Un'innocente unità sovietica è stata affondata! Innocenti vite sovietiche sono andate perdute! Al Kapitan è ordinato di cedere il comando e di essere rinchiuso nella cella frigorifera del battello. Così quel porco potrà abituarsi al suo ambiente futuro.", en = "CODE: 3-19-BKB\nTHE ACTIONS OF THE LENINSKII KOMSOMOL ARE A DISGRACE TO THIS NAVY. AN INNOCENT SOVIET VESSEL HAS BEEN SUNK! INNOCENT SOVIET LIVES HAVE BEEN LOST! THE KAPITAN IS HEREBY ORDERED TO SURRENDER HIS COMMAND FOR IMPRISONMENT IN THE SHIP'S ICE LOCKER. THIS SHOULD GIVE THE SWINE A CHANCE TO IDENTIFY WITH HIS FUTURE ENVIRONMENT." },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_028b = function(u)
  ::L_028b::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -96007.68) then goto L_0299 end
  m688.messaggio(u, 103)
  do return end
  ::L_0299::
  if m688.unita_esiste(u, 2) then goto L_02a1 end
  m688.messaggio(u, 104)
  do return end
  ::L_02a1::
  if m688.unita_esiste(u, 7) then goto L_02a9 end
  m688.messaggio(u, 107)
  do return end
  ::L_02a9::
  m688.dopo(u, 20, "L_028b")
  do return end
end

script.L_02c9 = function(u)
  ::L_02c9::
  if m688.manomesso(u) then goto L_02cd end
  do return end
  ::L_02cd::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_02d7 = function(u)
  ::L_02d7::
  m688.vai_a(u, -5926.4, -34373.12, 30, "L_02e2")
  m688.salva_modello(u)
  do return end
end

script.L_02e2 = function(u)
  ::L_02e2::
  m688.vai_a(u, 66375.68, -77043.2, 30, "L_02d7")
  m688.salva_modello(u)
  do return end
end

script.L_02ed = function(u)
  ::L_02ed::
  m688.vai_a(u, -23705.6, 15408.64, 30, "L_02f8")
  m688.salva_modello(u)
  do return end
end

script.L_02f8 = function(u)
  ::L_02f8::
  if m688.variabile_uguale(u, 5, 1) then goto L_0302 end
  m688.messaggio(u, 105)
  do return end
  ::L_0302::
  m688.messaggio(u, 106)
  do return end
end

script.L_0305 = function(u)
  ::L_0305::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 0.0391)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0313")
  do return end
end

script.L_0313 = function(u)
  ::L_0313::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.0039)
  m688.salva_modello(u)
  m688.dopo(u, 400, "L_0305")
  do return end
end

script.L_031e = function(u)
  ::L_031e::
  m688.marca_contatti(u)
  m688.dopo(u, 20, "L_031e")
  do return end
end

script.L_0325 = function(u)
  ::L_0325::
  if m688.bersaglio_id(u, 0) then goto L_0337 end
  if m688.bersaglio_id(u, 7) then goto L_03ae end
  if m688.bersaglio_arma(u) then goto L_03dd end
  ::L_0332::
  m688.comportamento(u, "L_0325")
  m688.salva_modello(u)
  do return end
  ::L_0337::
  if m688.traccia_solida(u) then goto L_033e end
  goto L_0332
  ::L_033e::
  if m688.variabile_uguale(u, 0, 0) then goto L_034f end
  m688.variabile(u, 0, 0)
  if m688.bersaglio_vicino(u, 1.8) then goto L_0389 end
  ::L_034f::
  if m688.bersaglio_vicino(u, 9) then goto L_0358 end
  goto L_0332
  ::L_0358::
  if m688.variabile_uguale(u, 0, 0) then goto L_0367 end
  m688.variabile(u, 0, 0)
  goto L_036e
  ::L_0367::
  if m688.variabile_uguale(u, 1, 0) then goto L_0332 end
  ::L_036e::
  if m688.variabile_uguale(u, 4, 4) then goto L_0332 end
  m688.comportamento(u, "L_0325")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  m688.marca_contatti(u)
  script.L_0677(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03aa")
  do return end
  ::L_0389::
  m688.annulla_tutti_i_timer(u)
  m688.comportamento(u, "L_033d")
  m688.telegrafo(u, 1.0547)
  m688.vai_a(u, 165939.2, 93637.12, 30, "L_02f8")
  m688.schiera_trainata(u, 0)
  m688.variabile(u, 5, 0)
  m688.salva_modello(u)
  m688.dopo(u, 200, "L_03a7")
  do return end
  ::L_03ae::
  if m688.flag_scena_uguale(u, 1) then goto L_03ba end
  m688.flag_scena(u, 1)
  m688.annulla_timer(u, "L_0305")
  m688.annulla_timer(u, "L_0313")
  ::L_03ba::
  m688.annulla_timer(u, "L_045a")
  m688.annulla_timer(u, "L_0479")
  m688.variabile(u, 5, 1)
  if m688.traccia_solida(u) then goto L_03d4 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.25)
  m688.schiera_trainata(u, 1)
  m688.comportamento(u, "L_0325")
  m688.salva_modello(u)
  do return end
  ::L_03d4::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.comportamento(u, "L_0325")
  m688.salva_modello(u)
  do return end
  ::L_03dd::
  if m688.bersaglio_mi_punta(u) then goto L_03e3 end
  goto L_0332
  ::L_03e3::
  if m688.bersaglio_vicino(u, 2) then goto L_03eb end
  goto L_0332
  ::L_03eb::
  m688.quota(u, 304.8, 1.0547)
  m688.telegrafo(u, 1.0547)
  m688.vai_a(u, 60449.28, -29632, 30, "L_0410")
  m688.variabile(u, 5, 0)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_0415")
  m688.dopo(u, 300, "L_0440")
  goto L_048b
  ::L_048b::
  if m688.variabile_uguale(u, 2, 0) then goto L_04aa end
  if m688.variabile_uguale(u, 3, 1) then goto L_04aa end
  m688.variabile(u, 3, 1)
  m688.variabile_meno(u, 2)
  m688.dopo(u, 120, "L_04ab")
  goto L_0700
  ::L_04aa::
  do return end
  ::L_0700::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_06fc", "L_06d9")
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
  m688.dopo(u, 2, "L_072d")
  m688.dopo(u, 160, "L_0732")
  do return end
end

script.L_033d = function(u)
  ::L_033d::
  do return end
end

script.L_03a7 = function(u)
  ::L_03a7::
  m688.messaggio(u, 108)
  do return end
end

script.L_03aa = function(u)
  ::L_03aa::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_0410 = function(u)
  ::L_0410::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0415 = function(u)
  ::L_0415::
  m688.telegrafo(u, 0)
  m688.quota(u, 304.8, 1)
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -140)
  m688.nuovo_caso(u)
  if m688.caso(u, 0.5039) then goto L_0429 end
  m688.virata_relativa(u, -320)
  ::L_0429::
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_0430")
  do return end
end

script.L_0430 = function(u)
  ::L_0430::
  m688.riprendi_navigazione(u)
  m688.quota(u, 45.72, 1)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0415")
  do return end
end

script.L_0440 = function(u)
  ::L_0440::
  m688.annulla_timer(u, "L_0415")
  m688.annulla_timer(u, "L_0430")
  m688.telegrafo(u, 0.5)
  m688.schiera_trainata(u, 1)
  m688.comportamento(u, "L_0325")
  m688.quota(u, 45.72, 0.3906)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_045a")
  do return end
end

script.L_045a = function(u)
  ::L_045a::
  m688.telegrafo(u, 0)
  m688.nuovo_caso(u)
  m688.quota(u, 304.8, 0.1953)
  m688.virata_relativa(u, -179.96)
  if m688.caso(u, 0.5039) then goto L_0472 end
  m688.quota(u, 45.72, 0.1953)
  m688.virata_relativa(u, -180)
  ::L_0472::
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0479")
  do return end
end

script.L_0479 = function(u)
  ::L_0479::
  m688.telegrafo(u, 0.5)
  m688.quota(u, 45.72, 0.1953)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_045a")
  do return end
end

script.L_04ab = function(u)
  ::L_04ab::
  m688.variabile(u, 3, 0)
  do return end
end

script.L_04c7 = function(u)
  ::L_04c7::
  m688.nuovo_caso(u)
  m688.posizione(u, 1185.28, -28446.72)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_0554 end
  m688.posizione(u, 10667.52, -20149.76)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_0554 end
  m688.posizione(u, 29632, -29632)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_0554 end
  m688.posizione(u, 42670.08, -21335.04)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_0554 end
  m688.posizione(u, 60449.28, -20149.76)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_0554 end
  m688.posizione(u, 54522.88, -42670.08)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_0554 end
  m688.posizione(u, 60449.28, -54522.88)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_0554 end
  m688.posizione(u, 50967.04, -54522.88)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_0554 end
  m688.posizione(u, 58078.72, -77043.2)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_0554 end
  m688.posizione(u, 39114.24, -56893.44)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_0554 end
  m688.posizione(u, 33187.84, -43855.36)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_0554 end
  m688.posizione(u, 10667.52, -47411.2)
  m688.rotta_ora(u, 300)
  ::L_0554::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_04c7")
  do return end
end

script.L_0631 = function(u)
  ::L_0631::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_06fc", "L_06d9")
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
  m688.dopo(u, 150, "L_0693")
  m688.dopo(u, 170, "L_06bc")
  m688.dopo(u, 240, "L_06fc")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0677 = function(u)
  ::L_0677::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "spearfish")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_06fc", "L_06d9")
  script.L_0696(u)
  m688.dopo(u, 60, "L_06bc")
  m688.dopo(u, 240, "L_06fc")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0693 = function(u)
  ::L_0693::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0696 = function(u)
  ::L_0696::
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

script.L_06bc = function(u)
  ::L_06bc::
  m688.annulla_timer(u, "L_06bc")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_06d9 = function(u)
  ::L_06d9::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_06c7")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06f5")
  m688.dopo(u, 10, "L_06ed")
  do return end
end

script.L_06c7 = function(u)
  ::L_06c7::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_06ed")
  m688.annulla_timer(u, "L_06d9")
  m688.insegui(u, 0, "L_06fc", "L_06d9")
  m688.salva_modello(u)
  do return end
end

script.L_06ed = function(u)
  ::L_06ed::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_06f5 = function(u)
  ::L_06f5::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06f5")
  do return end
end

script.L_06fc = function(u)
  ::L_06fc::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0700 = function(u)
  ::L_0700::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_06fc", "L_06d9")
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
  m688.dopo(u, 2, "L_072d")
  m688.dopo(u, 160, "L_0732")
  do return end
end

script.L_072d = function(u)
  ::L_072d::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0732 = function(u)
  ::L_0732::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0736 = function(u)
  ::L_0736::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_073b = function(u)
  ::L_073b::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_06d9")
  do return end
end

script.L_0745 = function(u, p)
  ::L_0745::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_0754")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0754 = function(u)
  ::L_0754::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_06c7")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0762 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0762::
  do return end
end

script.L_0763 = function(u, p)
  ::L_0763::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_076e")
  m688.salva_modello(u)
  do return end
end

script.L_076e = function(u)
  ::L_076e::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0775 = function(u, p)
  ::L_0775::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_077a = function(u, p)
  ::L_077a::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0781 = function(u, p)
  ::L_0781::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0789 = function(u)
  ::L_0789::
  m688.annulla_timer(u, "L_07c8")
  m688.dopo(u, 20, "L_07c8")
  do return end
end

script.L_0792 = function(u)
  ::L_0792::
  m688.annulla_timer(u, "L_07cb")
  m688.dopo(u, 20, "L_07cb")
  do return end
end

script.L_079b = function(u)
  ::L_079b::
  m688.annulla_timer(u, "L_07ce")
  m688.dopo(u, 20, "L_07ce")
  do return end
end

script.L_07a4 = function(u)
  ::L_07a4::
  m688.annulla_timer(u, "L_07d1")
  m688.dopo(u, 20, "L_07d1")
  do return end
end

script.L_07ad = function(u)
  ::L_07ad::
  m688.annulla_timer(u, "L_07d4")
  m688.dopo(u, 20, "L_07d4")
  do return end
end

script.L_07b6 = function(u)
  ::L_07b6::
  m688.annulla_timer(u, "L_07d7")
  m688.dopo(u, 20, "L_07d7")
  do return end
end

script.L_07bf = function(u)
  ::L_07bf::
  m688.annulla_timer(u, "L_07da")
  m688.dopo(u, 20, "L_07da")
  do return end
end

script.L_07c8 = function(u)
  ::L_07c8::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_07cb = function(u)
  ::L_07cb::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_07ce = function(u)
  ::L_07ce::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_07d1 = function(u)
  ::L_07d1::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_07d4 = function(u)
  ::L_07d4::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_07d7 = function(u)
  ::L_07d7::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_07da = function(u)
  ::L_07da::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_07dd = function(u)
  ::L_07dd::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_07e5 = function(u, p)
  ::L_07e5::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07e9 = function(u)
  ::L_07e9::
  m688.dopo(u, 60, "L_07f3")
  do return end
end

script.L_07ef = function(u, p)
  ::L_07ef::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07f3 = function(u)
  ::L_07f3::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_07f9 = function(u)
  ::L_07f9::
  m688.dopo(u, 40, "L_07ff")
  do return end
end

script.L_07ff = function(u)
  ::L_07ff::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0805 = function(u)
  ::L_0805::
  m688.annulla_timer(u, "L_07f3")
  m688.annulla_timer(u, "L_07ff")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_080f = function(u, p)
  ::L_080f::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0814 = function(u, p)
  ::L_0814::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

return missione
