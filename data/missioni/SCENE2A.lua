-- SCENE2A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE2A",
  missione = 2,
  titolo = { it = "Scrollatevelo di dosso", en = "SHAKE 'EM" },
  lato = "usa",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 135.636, 269.1384, 366.0648 } },
  dotazione = { mk48 = 8, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_03d3" },
    harpoon = { gestore = "L_0316" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_0464" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_04c7",
    telegrafo = "L_04d9",
    quota = "L_04de",
    timone = "L_04e5",
    pilota_automatico_off = "L_0541",
    periscopio = "L_0549",
    carica_tubo_1 = "L_04ed",
    carica_tubo_2 = "L_04f6",
    carica_tubo_3 = "L_04ff",
    carica_tubo_4 = "L_0508",
    carica_tubo_5 = "L_0511",
    carica_tubo_6 = "L_051a",
    carica_tubo_7 = "L_0523",
    schiera_cala = "L_054d",
    schiera_recupera = "L_055d",
    schiera_persa = "L_0569",
    designa_bersaglio = "L_0573",
    posizione = "L_0578",
    antenna = "L_0553",
    guida_e0 = "L_049a",
    guida_e1 = "L_049f",
    guida_e2 = "L_04a9",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.25390625, probabilita_originale = "65/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -973.76, y = -17960.64 }, originale = { x = 4864, y = 2816 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -1720.48, y = -15720.48 }, originale = { x = 4832, y = 2912 },
        quota_m = 30.48, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0125", reazione = nil, arrivo = nil } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 6, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 15454.08, y = -33641.76 }, originale = { x = 5568, y = 2144 },
        quota_m = 47.5, quota_originale_m = 243.84, rotta = 300, velocita_kn = 9.1429,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_018b", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0221" }, { dopo_s = 120, unita = 0, gestore = "L_022f" }, { dopo_s = 1200, unita = 0, gestore = "L_0245" }, { dopo_s = 40, unita = 1, gestore = "L_0252" } },
      variabili = { [1] = 0, [2] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -33641.76 }, originale = { x = 5568, y = 2144 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 17694.24, y = -34388.48 }, originale = { x = 5664, y = 2112 },
        quota_m = 30.48, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0125", reazione = nil, arrivo = nil } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 6, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 4253.28, y = 49990.88 }, originale = { x = 5088, y = 5728 },
        quota_m = 45.72, rotta = 140, velocita_kn = 9.1429,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_018b", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0221" }, { dopo_s = 120, unita = 0, gestore = "L_022f" }, { dopo_s = 1200, unita = 0, gestore = "L_0245" }, { dopo_s = 40, unita = 1, gestore = "L_0252" } },
      variabili = { [1] = 0, [2] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -973.76, y = -17960.64 }, originale = { x = 4864, y = 2816 },
        quota_m = 6.096, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -1720.48, y = -15720.48 }, originale = { x = 4832, y = 2912 },
        quota_m = 30.48, rotta = 140, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0125", reazione = nil, arrivo = nil } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 6, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 5746.72, y = -33641.76 }, originale = { x = 5152, y = 2144 },
        quota_m = 67.75, quota_originale_m = 243.84, rotta = 300, velocita_kn = 9.1429,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_018b", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0221" }, { dopo_s = 120, unita = 0, gestore = "L_022f" }, { dopo_s = 1200, unita = 0, gestore = "L_0245" }, { dopo_s = 40, unita = 1, gestore = "L_0252" } },
      variabili = { [1] = 0, [2] = 0 },
    },
    { probabilita = 0.24609375, probabilita_originale = "63/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -33641.76 }, originale = { x = 5568, y = 2144 },
        quota_m = 6.096, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -24681.12 }, originale = { x = 5440, y = 2528 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 17694.24, y = -34388.48 }, originale = { x = 5664, y = 2112 },
        quota_m = 30.48, rotta = 300, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0125", reazione = nil, arrivo = nil } },
      { id = 3, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 8733.6, y = -39615.52 }, originale = { x = 5280, y = 1888 },
        quota_m = 0, rotta = 0, velocita_kn = 13.7143,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 4, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7986.88, y = -37375.36 }, originale = { x = 5248, y = 1984 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 5, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 7240.16, y = -35881.92 }, originale = { x = 5216, y = 2048 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 15, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 20 },
        rotta_punti = { { x = 11800, y = -20100 }, { x = 12467.2, y = -23934.4 }, { x = 10227.04, y = -23187.68 }, { x = 7240.16, y = -23187.68 }, { x = 4253.28, y = -22440.96 }, { x = -6947.52, y = -10493.44 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02da" } },
      { id = 6, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -973.76, y = -17213.92 }, originale = { x = 4864, y = 2848 },
        quota_m = 45.72, rotta = 140, velocita_kn = 9.1429,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_018b", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0221" }, { dopo_s = 120, unita = 0, gestore = "L_022f" }, { dopo_s = 1200, unita = 0, gestore = "L_0245" }, { dopo_s = 40, unita = 1, gestore = "L_0252" } },
      variabili = { [1] = 0, [2] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, o si scrolla di dosso l'Alfa sovietico che ha in coda, o la richiamo da questa missione. Non serve che le dica quanto sia in gioco con il suo successo, compresa la sua carriera. Faccia tutto il necessario per far perdere le tracce a Ivan. Purtroppo non posso autorizzarla a lanciare armi, a meno che i russi non decidano di fare sul serio." },
    en = { "CAPTAIN, EITHER YOU SHAKE THE SOVIET ALFA ON YOUR TAIL OR I'M CALLING YOU OFF THIS MISSION. I DON'T HAVE TO TELL YOU THERE'S A LOT RIDING ON YOUR SUCCESS - INCLUDING YOUR CAREER. NOW DO WHATEVER IT TAKES TO THROW IVAN FOR A LOOP. UNFORTUNATELY, I CAN'T ALLOW YOU TO LAUNCH WEAPONS UNLESS THE RUSSIANS DECIDE THEY WANT TO PLAY HARDBALL." },
  },
  esiti = {
    [1] = { it = "CODICE: 2-1-ACP\nL'USS Los Angeles si è incagliato ed è andato perduto in mare durante un contatto con un sottomarino sovietico. I dettagli esatti dell'incidente sono attualmente oggetto di indagine, ma tutte le unità sono state poste in allerta gialla. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 2-1-ACP\nTHE USS LOS ANGELES RAN AGROUND AND WAS LOST AT SEA DURING AN ENCOUNTER WITH A SOVIET SUBMARINE. THE EXACT DETAILS PERTAINING TO THE ACCIDENT ARE CURRENTLY UNDER INVESTIGATION, BUT ALL SHIPS ARE BEING PUT ON YELLOW ALERT. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 2-2-ACP\nL'USS Los Angeles è andato perduto in mare durante un contatto con un sottomarino sovietico. La causa esatta dell'incidente è attualmente oggetto di indagine, ma tutte le unità sono state poste in allerta gialla. Se verrà accertato che la responsabilità è della minaccia rossa, farò in modo che paghino per le loro azioni. Le nostre condoglianze vanno alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 2-2-ACP\nTHE USS LOS ANGELES WAS LOST AT SEA DURING AN ENCOUNTER WITH A SOVIET SUBMARINE. THE EXACT CAUSE OF THE ACCIDENT IS CURRENTLY UNDER INVESTIGATION, BUT ALL SHIPS ARE BEING PUT ON YELLOW ALERT. IF THE RED MENACE IS FOUND TO BE RESPONSIBLE FOR THIS, I WILL SEE TO IT THAT THEY PAY FOR THEIR ACTIONS. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 2-3-ACP\nCongratulazioni, Comandante: da solo è riuscito a minacciare la pace mondiale e a mettere in imbarazzo la Marina. Il SOSUS indica che lei ha lanciato armi senza alcuna provocazione! Farà bene ad avere una buona giustificazione, perché ho intenzione di farle la pelle. Il comandante in seconda assuma il comando del Los Angeles e rientri immediatamente in porto! Potrà riflettere sulle sue azioni agli arresti nel suo alloggio.", en = "CODE: 2-3-ACP\nCONGRATULATIONS, CAPTAIN, YOU'VE SINGLE-HANDEDLY THREATENED WORLD PEACE AND EMBARRASSED THE NAVY. SOSUS INDICATED THAT YOU FIRED WEAPONS WITHOUT PROVOCATION! YOU'D BETTER HAVE A GOOD EXCUSE BECAUSE I INTEND TO HAVE YOUR BUTT FOR BREAKFAST. THE XO WILL TAKE COMMAND OF THE LOS ANGELES FOR RETURN TO PORT IMMEDIATELY! YOU MAY REFLECT ON YOUR ACTIONS AS YOU ARE RESTRICTED TO QUARTERS." },
    [4] = { it = "CODICE: 2-4-ACP\nComandante, quello è stato il più misero tentativo di evasione che abbia visto dai tempi delle esercitazioni da allievo nella sala giochi dell'Accademia! Non ho tempo per lasciarla esercitare le procedure di evasione di base in uno scenario reale. Per colpa della sua incompetenza devo prolungare il pattugliamento di un altro sottomarino. Ha compromesso completamente la sua missione, Comandante. Rientri subito in porto, dove seguirò personalmente il suo corso di recupero di tattica subacquea.", en = "CODE: 2-4-ACP\nCAPTAIN, THAT WAS THE WEAKEST EXCUSE FOR AN ATTEMPT AT EVASION THAT I'VE SEEN SINCE CADET TRAINING IN THE GAME ROOM OF THE ACADEMY! I DON'T HAVE TIME FOR YOU TO PRACTICE ROUTINE EVASION PROCEDURES IN A REAL WORLD SCENARIO. BECAUSE OF YOUR INCOMPETENCE, I MUST EXTEND ANOTHER SUBMARINE'S PATROL PERIOD. YOU HAVE COMPLETELY COMPROMISED YOUR MISSION CAPTAIN. RETURN TO PORT AT ONCE, WHERE I CAN PERSONALLY OVERSEE YOUR REMEDIAL EDUCATION IN SUBMARINE TACTICS." },
    [5] = { it = "CODICE: 2-5-ACP\nOttimo lavoro, Comandante. La sua prestazione sarà annotata nel suo stato di servizio. È riuscito a eludere la sorveglianza sovietica. Al suo rientro dovrà illustrare le sue tecniche di evasione ai nostri comandanti di squadriglia. Prosegua la missione come previsto: sembra proprio che sia partito alla grande.", en = "CODE: 2-5-ACP\nEXCELLENT WORK, CAPTAIN. YOUR PERFORMANCE WILL GO DOWN ON YOUR PERMANENT RECORD. YOU SUCCESSFULLY EVADED SOVIET SURVEILLANCE. I MUST HAVE YOU BRIEF OUR SQUADRON COMMANDERS ON YOUR EVASION TECHNIQUES WHEN YOU RETURN. PROCEED WITH YOUR MISSION AS PLANNED, IT LOOKS LIKE YOUR OFF TO A GREAT START." },
    [6] = { it = "CODICE: 2-6-ACP\nOttimo lavoro, Comandante: sono lieto di vedere che ne è uscito tutto intero. Le mosse di Ivan ci hanno tenuti col fiato sospeso per un po', ma sembra che tecnologia e addestramento abbiano ancora una volta avuto la meglio su dogmatismo e forza bruta.", en = "CODE: 2-6-ACP\nGOOD WORK CAPTAIN, I'M GLAD TO SEE YOU CAME OUT OF THIS ONE IN ONE PIECE. IVAN'S ACTIONS HAD US GOING FOR A WHILE, BUT IT LOOKS LIKE TECHNOLOGY AND TRAINING HAVE ONCE AGAIN PREVAILED OVER DOGMATISM AND BRUTE FORCE." },
    [7] = { it = "CODICE: 2-7-ACP\nCongratulazioni, Comandante: da solo è riuscito a minacciare la pace mondiale e a mettere in imbarazzo la Marina. Il SOSUS indica che lei ha lanciato armi senza alcuna provocazione! Farà bene ad avere una buona giustificazione, perché ho intenzione di farle la pelle. Il comandante in seconda assuma il comando del Los Angeles e rientri immediatamente in porto! Potrà riflettere sulle sue azioni agli arresti nel suo alloggio.", en = "CODE: 2-7-ACP\nCONGRATULATIONS, CAPTAIN, YOU'VE SINGLE-HANDEDLY THREATENED WORLD PEACE AND EMBARRASSED THE NAVY. SOSUS INDICATED THAT YOU FIRED WEAPONS WITHOUT PROVOCATION! YOU'D BETTER HAVE A GOOD EXCUSE BECAUSE I INTEND TO HAVE YOUR BUTT FOR BREAKFAST. THE XO WILL TAKE COMMAND OF THE LOS ANGELES FOR RETURN TO PORT IMMEDIATELY! YOU MAY REFLECT ON YOUR ACTIONS AS YOU ARE RESTRICTED TO QUARTERS." },
    [8] = { it = "CODICE: 2-8-ACP", en = "CODE: 2-8-ACP" },
    [9] = { it = "CODICE: 2-9-ACP", en = "CODE: 2-9-ACP" },
    [10] = { it = "CODICE: 2-10-ACP", en = "CODE: 2-10-ACP" },
    [11] = { it = "CODICE: 2-11-ACP", en = "CODE: 2-11-ACP" },
    [12] = { it = "CODICE: 2-12-ACP", en = "CODE: 2-12-ACP" },
    [13] = { it = "CODICE: 2-13-ACP", en = "CODE: 2-13-ACP" },
    [14] = { it = "CODICE: 2-14-ACP", en = "CODE: 2-14-ACP" },
    [15] = { it = "CODICE: 2-15-ACP", en = "CODE: 2-15-ACP" },
    [16] = { it = "CODICE: 2-16-ACP", en = "CODE: 2-16-ACP" },
    [17] = { it = "CODICE: 2-17-ACP", en = "CODE: 2-17-ACP" },
    [18] = { it = "CODICE: 2-18-ACP", en = "CODE: 2-18-ACP" },
    [19] = { it = "CODICE: 2-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 2-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 12, spostamento_max_m = 1984.5947, non_risolte = 0 },
  script = script,
}

script.L_0125 = function(u)
  ::L_0125::
  if m688.bersaglio_id(u, 0) then goto L_012f end
  m688.comportamento(u, "L_0125")
  m688.salva_modello(u)
  do return end
  ::L_012f::
  m688.variabile(u, 5, 0)
  m688.annulla_tutti_i_timer(u)
  if m688.traccia_solida(u) then goto L_0155 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.25)
  m688.ingaggia(u, 10, "L_012f", "L_01d8")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  ::L_0154::
  do return end
  ::L_0155::
  m688.variabile(u, 5, 0)
  if m688.bersaglio_vicino(u, 9) then goto L_0165 end
  m688.variabile(u, 5, 1)
  do return end
  ::L_0165::
  m688.uguaglia_quota(u)
  m688.uguaglia_velocita(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_012f", "L_01d8")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 0, 0) then goto L_017e end
  m688.variabile(u, 0, 0)
  goto L_0185
  ::L_017e::
  if m688.variabile_uguale(u, 1, 0) then goto L_018a end
  ::L_0185::
  if m688.bersaglio_vicino(u, 9) then goto L_01bd end
  ::L_018a::
  do return end
  ::L_01bd::
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_039a(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_01d4")
  do return end
end

script.L_012f = function(u)
  ::L_012f::
  m688.variabile(u, 5, 0)
  m688.annulla_tutti_i_timer(u)
  if m688.traccia_solida(u) then goto L_0155 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.25)
  m688.ingaggia(u, 10, "L_012f", "L_01d8")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  ::L_0154::
  do return end
  ::L_0155::
  m688.variabile(u, 5, 0)
  if m688.bersaglio_vicino(u, 9) then goto L_0165 end
  m688.variabile(u, 5, 1)
  do return end
  ::L_0165::
  m688.uguaglia_quota(u)
  m688.uguaglia_velocita(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_012f", "L_01d8")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 0, 0) then goto L_017e end
  m688.variabile(u, 0, 0)
  goto L_0185
  ::L_017e::
  if m688.variabile_uguale(u, 1, 0) then goto L_018a end
  ::L_0185::
  if m688.bersaglio_vicino(u, 9) then goto L_01bd end
  ::L_018a::
  do return end
  ::L_01bd::
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_039a(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_01d4")
  do return end
end

script.L_018b = function(u)
  ::L_0154::
  do return end
  ::L_018b::
  if m688.bersaglio_id(u, 0) then goto L_0194 end
  m688.comportamento(u, "L_018b")
  do return end
  ::L_0194::
  m688.ingaggia(u, 10, "L_0194", "L_0190")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_01b0 end
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  do return end
  ::L_01b0::
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  if m688.bersaglio_vicino(u, 9) then goto L_01bd end
  do return end
  ::L_01bd::
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_039a(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_01d4")
  do return end
end

script.L_0190 = function(u)
  ::L_0190::
  m688.comportamento(u, "L_018b")
  do return end
end

script.L_0194 = function(u)
  ::L_0154::
  do return end
  ::L_0194::
  m688.ingaggia(u, 10, "L_0194", "L_0190")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_01b0 end
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  do return end
  ::L_01b0::
  if m688.variabile_uguale(u, 1, 0) then goto L_0154 end
  if m688.bersaglio_vicino(u, 9) then goto L_01bd end
  do return end
  ::L_01bd::
  if m688.variabile_uguale(u, 2, 6) then goto L_0154 end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 2)
  script.L_039a(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_01d4")
  do return end
end

script.L_01d4 = function(u)
  ::L_01d4::
  m688.variabile_meno(u, 2)
  do return end
end

script.L_01d8 = function(u)
  ::L_01d8::
  m688.comportamento(u, "L_0125")
  m688.telegrafo(u, 0.0195)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_01ed")
  m688.dopo(u, 200, "L_01ea")
  do return end
end

script.L_01ea = function(u)
  ::L_01ea::
  m688.messaggio(u, 105)
  do return end
end

script.L_01ed = function(u)
  ::L_01ed::
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_01f7")
  do return end
end

script.L_01f7 = function(u)
  ::L_01f7::
  m688.quota(u, 182.88, 1)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0203")
  do return end
end

script.L_0203 = function(u)
  ::L_0203::
  m688.quota(u, 365.76, 1)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_0212")
  do return end
end

script.L_0212 = function(u)
  ::L_0212::
  m688.quota(u, 45.72, 1)
  m688.telegrafo(u, 0.5)
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_01ed")
  do return end
end

script.L_0221 = function(u)
  ::L_0221::
  if m688.manomesso(u) then goto L_0225 end
  do return end
  ::L_0225::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_022f = function(u)
  ::L_022f::
  if m688.unita_esiste(u, 2) then goto L_0237 end
  goto L_023c
  ::L_0237::
  if m688.unita_esiste(u, 6) then goto L_023f end
  ::L_023c::
  m688.messaggio(u, 107)
  do return end
  ::L_023f::
  m688.dopo(u, 20, "L_022f")
  do return end
end

script.L_0245 = function(u)
  ::L_0245::
  if m688.variabile_uguale(u, 5, 1) then goto L_024f end
  m688.messaggio(u, 104)
  do return end
  ::L_024f::
  m688.messaggio(u, 106)
  do return end
end

script.L_0252 = function(u)
  ::L_0252::
  m688.nuovo_caso(u)
  m688.posizione(u, 5000, -22440.96)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.1602) then goto L_0297 end
  m688.posizione(u, -973.76, -25427.84)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.3164) then goto L_0297 end
  m688.posizione(u, -227.04, -29908.16)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.4727) then goto L_0297 end
  m688.posizione(u, 519.68, -36628.64)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.6289) then goto L_0297 end
  m688.posizione(u, 5746.72, -35135.2)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.7852) then goto L_0297 end
  m688.posizione(u, 22174.56, -37375.36)
  m688.rotta_ora(u, 245)
  ::L_0297::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0252")
  do return end
end

script.L_02da = function(u)
  ::L_02da::
  m688.vai_a(u, 12467.2, -23934.4, 30, "L_02e5")
  m688.salva_modello(u)
  do return end
end

script.L_02e5 = function(u)
  ::L_02e5::
  m688.vai_a(u, 10227.04, -23187.68, 30, "L_02f0")
  m688.salva_modello(u)
  do return end
end

script.L_02f0 = function(u)
  ::L_02f0::
  m688.vai_a(u, 7240.16, -23187.68, 30, "L_02fb")
  m688.salva_modello(u)
  do return end
end

script.L_02fb = function(u)
  ::L_02fb::
  m688.vai_a(u, 4253.28, -22440.96, 30, "L_0306")
  m688.salva_modello(u)
  do return end
end

script.L_0306 = function(u)
  ::L_0306::
  m688.vai_a(u, -6947.52, -10493.44, 30, "L_0311")
  m688.salva_modello(u)
  do return end
end

script.L_0311 = function(u)
  ::L_0311::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0316 = function(u)
  ::L_0316::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_0320")
  do return end
end

script.L_0320 = function(u)
  ::L_0320::
  m688.attacco_casuale(u, 100)
  do return end
end

script.L_039a = function(u)
  ::L_039a::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0460", "L_043d")
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
  m688.dopo(u, 200, "L_0460")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_03d3 = function(u)
  ::L_03d3::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0460", "L_043d")
  script.L_03fa(u)
  m688.dopo(u, 170, "L_03f7")
  m688.dopo(u, 190, "L_0420")
  m688.dopo(u, 280, "L_0460")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_03f7 = function(u)
  ::L_03f7::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_03fa = function(u)
  ::L_03fa::
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
  m688.nuova_unita(u)
  do return end
end

script.L_0420 = function(u)
  ::L_0420::
  m688.annulla_timer(u, "L_0420")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_043d = function(u)
  ::L_043d::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_042b")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0459")
  m688.dopo(u, 10, "L_0451")
  do return end
end

script.L_042b = function(u)
  ::L_042b::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0451")
  m688.annulla_timer(u, "L_043d")
  m688.insegui(u, 0, "L_0460", "L_043d")
  m688.salva_modello(u)
  do return end
end

script.L_0451 = function(u)
  ::L_0451::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0459 = function(u)
  ::L_0459::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0459")
  do return end
end

script.L_0460 = function(u)
  ::L_0460::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0464 = function(u)
  ::L_0464::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0460", "L_043d")
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
  m688.dopo(u, 2, "L_0491")
  m688.dopo(u, 160, "L_0496")
  do return end
end

script.L_0491 = function(u)
  ::L_0491::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0496 = function(u)
  ::L_0496::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_049a = function(u)
  ::L_049a::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_049f = function(u)
  ::L_049f::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_043d")
  do return end
end

script.L_04a9 = function(u, p)
  ::L_04a9::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_04b8")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_04b8 = function(u)
  ::L_04b8::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_042b")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_04c6 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_04c6::
  do return end
end

script.L_04c7 = function(u, p)
  ::L_04c7::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_04d2")
  m688.salva_modello(u)
  do return end
end

script.L_04d2 = function(u)
  ::L_04d2::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_04d9 = function(u, p)
  ::L_04d9::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_04de = function(u, p)
  ::L_04de::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_04e5 = function(u, p)
  ::L_04e5::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_04ed = function(u)
  ::L_04ed::
  m688.annulla_timer(u, "L_052c")
  m688.dopo(u, 20, "L_052c")
  do return end
end

script.L_04f6 = function(u)
  ::L_04f6::
  m688.annulla_timer(u, "L_052f")
  m688.dopo(u, 20, "L_052f")
  do return end
end

script.L_04ff = function(u)
  ::L_04ff::
  m688.annulla_timer(u, "L_0532")
  m688.dopo(u, 20, "L_0532")
  do return end
end

script.L_0508 = function(u)
  ::L_0508::
  m688.annulla_timer(u, "L_0535")
  m688.dopo(u, 20, "L_0535")
  do return end
end

script.L_0511 = function(u)
  ::L_0511::
  m688.annulla_timer(u, "L_0538")
  m688.dopo(u, 20, "L_0538")
  do return end
end

script.L_051a = function(u)
  ::L_051a::
  m688.annulla_timer(u, "L_053b")
  m688.dopo(u, 20, "L_053b")
  do return end
end

script.L_0523 = function(u)
  ::L_0523::
  m688.annulla_timer(u, "L_053e")
  m688.dopo(u, 20, "L_053e")
  do return end
end

script.L_052c = function(u)
  ::L_052c::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_052f = function(u)
  ::L_052f::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0532 = function(u)
  ::L_0532::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0535 = function(u)
  ::L_0535::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0538 = function(u)
  ::L_0538::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_053b = function(u)
  ::L_053b::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_053e = function(u)
  ::L_053e::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0541 = function(u)
  ::L_0541::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0549 = function(u, p)
  ::L_0549::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_054d = function(u)
  ::L_054d::
  m688.dopo(u, 60, "L_0557")
  do return end
end

script.L_0553 = function(u, p)
  ::L_0553::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0557 = function(u)
  ::L_0557::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_055d = function(u)
  ::L_055d::
  m688.dopo(u, 40, "L_0563")
  do return end
end

script.L_0563 = function(u)
  ::L_0563::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0569 = function(u)
  ::L_0569::
  m688.annulla_timer(u, "L_0557")
  m688.annulla_timer(u, "L_0563")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0573 = function(u, p)
  ::L_0573::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0578 = function(u, p)
  ::L_0578::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

return missione
