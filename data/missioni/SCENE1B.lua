-- SCENE1B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE1B",
  missione = 1,
  titolo = { it = "TORPEX '89", en = "TORPEX '89" },
  lato = "urss",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 91.44, 243.84, 457.2 } },
  dotazione = { mk48 = 22, harpoon = 2, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_064b" },
    harpoon = { gestore = "L_05f8" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_06e4" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0747",
    telegrafo = "L_0759",
    quota = "L_075e",
    timone = "L_0765",
    pilota_automatico_off = "L_07c1",
    periscopio = "L_07c9",
    carica_tubo_1 = "L_076d",
    carica_tubo_2 = "L_0776",
    carica_tubo_3 = "L_077f",
    carica_tubo_4 = "L_0788",
    carica_tubo_5 = "L_0791",
    carica_tubo_6 = "L_079a",
    carica_tubo_7 = "L_07a3",
    schiera_cala = "L_07cd",
    schiera_recupera = "L_07dd",
    schiera_persa = "L_07e9",
    designa_bersaglio = "L_07f3",
    posizione = "L_07f8",
    antenna = "L_07d3",
    guida_e0 = "L_071a",
    guida_e1 = "L_071f",
    guida_e2 = "L_0729",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.25390625, probabilita_originale = "65/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 17000, y = 29500 }, originale = { x = 5664, y = 4832 }, spostamento_m = 809.9949,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 44576.16, y = 35056.48 }, originale = { x = 6816, y = 5088 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02be" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 46816.32, y = 35803.2 }, originale = { x = 6912, y = 5120 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02be" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 24414.72, y = 16388.48 }, originale = { x = 5952, y = 4288 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_030b" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 22921.28, y = 17135.2 }, originale = { x = 5888, y = 4320 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_030b" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 9480.32, y = 44017.12 }, originale = { x = 5312, y = 5472 },
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 13213.92, y = 40283.52 }, { x = 5746.72, y = 39536.8 }, { x = 16000, y = 30500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0363" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 27401.6, y = 40283.52 }, originale = { x = 6080, y = 5312 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = 0, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 41030.24 }, { x = 27401.6, y = 40283.52 }, { x = 26654.88, y = 43270.4 }, { x = 27401.6, y = 49990.88 }, { x = 31135.2, y = 49990.88 }, { x = 33375.36, y = 48497.44 }, { x = 31135.2, y = 49990.88 }, { x = 27401.6, y = 49990.88 }, { x = 26654.88, y = 43270.4 }, { x = 23668, y = 35803.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_03f4", reazione = nil, arrivo = "L_0253" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0522" }, { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_0531" }, { dopo_s = 30, unita = 2, gestore = "L_051b" }, { dopo_s = 60, unita = 3, gestore = "L_051b" }, { dopo_s = 90, unita = 4, gestore = "L_051b" }, { dopo_s = 90, unita = 5, gestore = "L_051b" }, { dopo_s = 90, unita = 6, gestore = "L_051b" }, { dopo_s = 30, unita = 7, gestore = "L_01f2" }, { dopo_s = 300, unita = 7, gestore = "L_01f8" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 12467.2, y = 8921.28 }, originale = { x = 5440, y = 3968 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 21427.84, y = 707.36 }, originale = { x = 5824, y = 3616 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0379" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 23668, y = -786.08 }, originale = { x = 5920, y = 3552 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0379" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -7000, y = 34500 }, originale = { x = 4576, y = 4960 }, spostamento_m = 2527.6102,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03a5" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -6800, y = 34500 }, originale = { x = 4640, y = 4896 }, spostamento_m = 3969.3275,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03a5" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 6800, y = 15500 }, originale = { x = 5056, y = 4192 }, spostamento_m = 3560.0261,
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 5000, y = 17500 }, { x = -2000, y = 25300 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03dc" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 11600, y = 9500 }, originale = { x = 5472, y = 4064 }, spostamento_m = 2316.273,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = 0, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11400, y = 34100 }, { x = 17694.24, y = 29082.72 }, { x = 22600, y = 24700 }, { x = 24400, y = 27700 }, { x = 24400, y = 27900 }, { x = 24200, y = 34300 }, { x = 23600, y = 36500 }, { x = 27401.6, y = 40283.52 }, { x = 26654.88, y = 43270.4 }, { x = 27401.6, y = 49990.88 }, { x = 31135.2, y = 49990.88 }, { x = 33375.36, y = 48497.44 }, { x = 31135.2, y = 49990.88 }, { x = 27401.6, y = 49990.88 }, { x = 26654.88, y = 43270.4 }, { x = 23668, y = 35803.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047e", reazione = nil, arrivo = "L_020e" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0522" }, { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_0531" }, { dopo_s = 30, unita = 2, gestore = "L_051b" }, { dopo_s = 60, unita = 3, gestore = "L_051b" }, { dopo_s = 90, unita = 4, gestore = "L_051b" }, { dopo_s = 90, unita = 5, gestore = "L_051b" }, { dopo_s = 90, unita = 6, gestore = "L_051b" }, { dopo_s = 30, unita = 7, gestore = "L_01f2" }, { dopo_s = 300, unita = 7, gestore = "L_01f8" } },
      variabili = { [1] = 0, [2] = 2, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 27401.6, y = 40283.52 }, originale = { x = 6080, y = 5312 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 44576.16, y = 35056.48 }, originale = { x = 6816, y = 5088 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02be" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 46816.32, y = 35803.2 }, originale = { x = 6912, y = 5120 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02be" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 24414.72, y = 16388.48 }, originale = { x = 5952, y = 4288 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_030b" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 22921.28, y = 17135.2 }, originale = { x = 5888, y = 4320 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_030b" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 9480.32, y = 44017.12 }, originale = { x = 5312, y = 5472 },
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 13213.92, y = 40283.52 }, { x = 5746.72, y = 39536.8 }, { x = 16000, y = 30500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0363" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 17000, y = 29500 }, originale = { x = 5664, y = 4832 }, spostamento_m = 809.9949,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = 0, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11400, y = 34100 }, { x = 17694.24, y = 29082.72 }, { x = 22600, y = 24700 }, { x = 24400, y = 27700 }, { x = 24400, y = 27900 }, { x = 24200, y = 34300 }, { x = 23600, y = 36500 }, { x = 27401.6, y = 40283.52 }, { x = 26654.88, y = 43270.4 }, { x = 27401.6, y = 49990.88 }, { x = 31135.2, y = 49990.88 }, { x = 33375.36, y = 48497.44 }, { x = 31135.2, y = 49990.88 }, { x = 27401.6, y = 49990.88 }, { x = 26654.88, y = 43270.4 }, { x = 23668, y = 35803.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0439", reazione = nil, arrivo = "L_020e" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0522" }, { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_0531" }, { dopo_s = 30, unita = 2, gestore = "L_051b" }, { dopo_s = 60, unita = 3, gestore = "L_051b" }, { dopo_s = 90, unita = 4, gestore = "L_051b" }, { dopo_s = 90, unita = 5, gestore = "L_051b" }, { dopo_s = 90, unita = 6, gestore = "L_051b" }, { dopo_s = 30, unita = 7, gestore = "L_01f2" }, { dopo_s = 300, unita = 7, gestore = "L_01f8" } },
      variabili = { [1] = 0, [2] = 1, [3] = 0, [4] = 0, [5] = 0 },
    },
    { probabilita = 0.24609375, probabilita_originale = "63/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 11600, y = 9500 }, originale = { x = 5472, y = 4064 }, spostamento_m = 2316.273,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 21427.84, y = 707.36 }, originale = { x = 5824, y = 3616 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0379" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 23668, y = -786.08 }, originale = { x = 5920, y = 3552 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0379" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -7000, y = 34500 }, originale = { x = 4576, y = 4960 }, spostamento_m = 2527.6102,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03a5" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -6800, y = 34500 }, originale = { x = 4640, y = 4896 }, spostamento_m = 3969.3275,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03a5" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 6800, y = 15500 }, originale = { x = 5056, y = 4192 }, spostamento_m = 3560.0261,
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 5000, y = 17500 }, { x = -2000, y = 25300 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_03dc" } },
      { id = 7, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 12467.2, y = 8921.28 }, originale = { x = 5440, y = 3968 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = 0, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11400, y = 34100 }, { x = 17694.24, y = 29082.72 }, { x = 22600, y = 24700 }, { x = 24400, y = 27700 }, { x = 24400, y = 27900 }, { x = 24200, y = 34300 }, { x = 23600, y = 36500 }, { x = 27401.6, y = 40283.52 }, { x = 26654.88, y = 43270.4 }, { x = 27401.6, y = 49990.88 }, { x = 31135.2, y = 49990.88 }, { x = 33375.36, y = 48497.44 }, { x = 31135.2, y = 49990.88 }, { x = 27401.6, y = 49990.88 }, { x = 26654.88, y = 43270.4 }, { x = 23668, y = 35803.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_047e", reazione = nil, arrivo = "L_020e" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0522" }, { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_0531" }, { dopo_s = 30, unita = 2, gestore = "L_051b" }, { dopo_s = 60, unita = 3, gestore = "L_051b" }, { dopo_s = 90, unita = 4, gestore = "L_051b" }, { dopo_s = 90, unita = 5, gestore = "L_051b" }, { dopo_s = 90, unita = 6, gestore = "L_051b" }, { dopo_s = 30, unita = 7, gestore = "L_01f2" }, { dopo_s = 300, unita = 7, gestore = "L_01f8" } },
      variabili = { [1] = 0, [2] = 2, [3] = 0, [4] = 0, [5] = 0 },
    },
  },
  briefing = {
    it = { "Benvenuto nell'Atlantico del Nord, Comandante. Lei darà inizio a un'esercitazione al largo dell'arcipelago delle Fær Øer. Si tratta di una competizione contro l'USS Los Angeles. Là fuori ci sono 5 cacciatorpediniere della classe Forrest Sherman, e vince chi ne affonda di più. Conta solo il colpo di grazia. Ricordi che ha a disposizione anche i missili." },
    en = { "WELCOME TO THE NORTH ATLANTIC CAPTAIN. YOU ARE TO BEGIN A TRAINING EXERCISE OFF THE COAST OF THE FAEROE ISLAND CHAIN. THIS IS A COMPETITION AGAINST THE USS LOS ANGELES. THERE ARE 5 FOREST SHERMAN CLASS DESTROYERS OUT THERE, AND WHOEVER SCORES THE MOST KILLS WINS. ONLY THE KILLING BLOW COUNTS. REMEMBER, YOU HAVE MISSILES" },
  },
  esiti = {
    [1] = { it = "CODICE: 1-1-BKB\nIeri l'USS Dallas si è incagliato ed è andato distrutto durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Dallas.", en = "CODE: 1-1-BKB\nTHE USS DALLAS RAN AGROUND AND WAS DESTROYED YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE DALLAS." },
    [2] = { it = "CODICE: 1-2-BKB\nIeri l'USS Dallas è stato distrutto da siluri durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Dallas.", en = "CODE: 1-2-BKB\nTHE USS DALLAS WAS DESTROYED BY TORPEDO FIRE YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE DALLAS." },
    [3] = { it = "CODICE: 1-3-BKB\nComandante, stiamo conducendo le operazioni di soccorso per l'USS Los Angeles, ma senza dubbio avrà sulla coscienza molte vittime americane. Sono lieto che la sua carriera si sia interrotta qui, prima che potesse esercitare la sua incompetenza in una situazione più critica. Le ordino di cedere il comando al comandante in seconda durante il rientro in porto.", en = "CODE: 1-3-BKB\nCAPTAIN, WE ARE CURRENTLY PERFORMING RESCUE OPERATIONS FOR THE USS LOS ANGELES, BUT UNDOUBTEDLY YOU WILL HAVE MANY AMERICAN CASUALTIES TO YOUR CREDIT. I'M GLAD YOUR CAREER WAS CUT SHORT HERE AND YOU WEREN'T ALLOWED TO EXERCISE YOUR INCOMPETENCE IN A MORE CRITICAL SITUATION. YOU ARE HEREBY INSTRUCTED TO HAND OVER YOUR COMMAND TO THE XO DURING YOUR RETURN TO PORT." },
    [4] = { it = "CODICE: 1-4-BKB\nIl Dallas rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Comandante, lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete la vostra fiacca prestazione al poligono (nessuna nave nemmeno danneggiata!). Si porti dietro la sua roba, perché per il prossimo mese non sarà a bordo.", en = "CODE: 1-4-BKB\nTHE DALLAS IS TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME YOUR LACKLUSTER (NO SHIPS DAMAGED AT ALL!) PERFORMANCE AT THE WEAPONS RANGE. BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WON'T BE ON BOARD FOR THE NEXT MONTH." },
    [5] = { it = "CODICE: 1-5-BKB\nAffondare meno navi del Los Angeles durante l'esercitazione è considerato insufficiente. Vedo che lei e i suoi ufficiali avete bisogno di ulteriore addestramento prima che vi affidiamo a tempo pieno un sottomarino da un miliardo di dollari. Si presenti nel mio ufficio al rientro in porto.", en = "CODE: 1-5-BKB\nSINKING FEWER SHIPS THAN THE LOS ANGELES DURING THE TRAINING EXERCISE IS CONSIDERED SUBSTANDARD. I CAN SEE THAT YOU AND YOUR OFFICERS ARE IN NEED OF FURTHER TRAINING BEFORE WE GIVE YOU FULL-TIME CONTROL OF A BILLION DOLLAR SUBMARINE. REPORT TO MY OFFICE UPON YOUR RETURN TO PORT." },
    [6] = { it = "CODICE: 1-6-BKB\nPrestazione eccezionale oggi al poligono, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 1-6-BKB\nOUTSTANDING PERFORMANCE ON THE RANGE TODAY, CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS OF LEAVE TO YOUR FINE CREW." },
    [7] = { it = "CODICE: 1-7-BKB\nIl Dallas rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Comandante, lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete esattamente cosa stavate cercando di combinare là fuori. Si porti dietro la sua roba, perché per il prossimo mese non sarà a bordo.", en = "CODE: 1-7-BKB\nTHE DALLAS IS TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME EXACTLY WHAT YOU WERE TRYING TO PULL OFF OUT THERE. BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WON'T BE ON BOARD FOR THE NEXT MONTH." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 10, punti = 99, spostamento_max_m = 3969.3275, non_risolte = 0 },
  script = script,
}

script.L_01f2 = function(u)
  ::L_01f2::
  m688.variabile(u, 5, 1)
  do return end
end

script.L_01f8 = function(u)
  ::L_01f8::
  if m688.variabile_uguale(u, 2, 0) then goto L_041e end
  if m688.variabile_uguale(u, 2, 1) then goto L_0463 end
  if m688.variabile_uguale(u, 2, 2) then goto L_04ac end
  do return end
  ::L_041e::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 27401.6, 40283.52, 30, "L_0261")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_0463::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 17694.24, 29082.72, 30, "L_0219")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_04ac::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.nuovo_caso(u)
  m688.vai_a(u, 5000, 17500, 30, "L_04d5")
  if m688.caso(u, 0.5039) then goto L_04c9 end
  m688.vai_a(u, 14707.36, 5187.68, 30, "L_04d5")
  ::L_04c9::
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
end

script.L_020e = function(u)
  ::L_020e::
  m688.vai_a(u, 17694.24, 29082.72, 30, "L_0219")
  m688.salva_modello(u)
  do return end
end

script.L_0219 = function(u)
  ::L_0219::
  m688.vai_a(u, 22600, 24700, 30, "L_0224")
  m688.salva_modello(u)
  do return end
end

script.L_0224 = function(u)
  ::L_0224::
  m688.quota_ora(u, 0)
  m688.vai_a(u, 24400, 27700, 30, "L_0232")
  m688.salva_modello(u)
  do return end
end

script.L_0232 = function(u)
  ::L_0232::
  m688.vai_a(u, 24400, 27900, 30, "L_023d")
  m688.salva_modello(u)
  do return end
end

script.L_023d = function(u)
  ::L_023d::
  m688.vai_a(u, 24200, 34300, 30, "L_0248")
  m688.salva_modello(u)
  do return end
end

script.L_0248 = function(u)
  ::L_0248::
  m688.vai_a(u, 23600, 36500, 30, "L_0253")
  m688.salva_modello(u)
  do return end
end

script.L_0253 = function(u)
  ::L_0253::
  m688.quota_ora(u, 6.096)
  m688.vai_a(u, 27401.6, 40283.52, 30, "L_0261")
  m688.salva_modello(u)
  do return end
end

script.L_0261 = function(u)
  ::L_0261::
  m688.vai_a(u, 26654.88, 43270.4, 30, "L_026c")
  m688.salva_modello(u)
  do return end
end

script.L_026c = function(u)
  ::L_026c::
  m688.vai_a(u, 27401.6, 49990.88, 30, "L_0277")
  m688.salva_modello(u)
  do return end
end

script.L_0277 = function(u)
  ::L_0277::
  m688.vai_a(u, 31135.2, 49990.88, 30, "L_0282")
  m688.salva_modello(u)
  do return end
end

script.L_0282 = function(u)
  ::L_0282::
  m688.vai_a(u, 33375.36, 48497.44, 30, "L_028d")
  m688.salva_modello(u)
  do return end
end

script.L_028d = function(u)
  ::L_028d::
  m688.vai_a(u, 31135.2, 49990.88, 30, "L_0298")
  m688.salva_modello(u)
  do return end
end

script.L_0298 = function(u)
  ::L_0298::
  m688.vai_a(u, 27401.6, 49990.88, 30, "L_02a3")
  m688.salva_modello(u)
  do return end
end

script.L_02a3 = function(u)
  ::L_02a3::
  m688.vai_a(u, 26654.88, 43270.4, 30, "L_02ae")
  m688.salva_modello(u)
  do return end
end

script.L_02ae = function(u)
  ::L_02ae::
  m688.vai_a(u, 23668, 35803.2, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_02b9 = function(u)
  ::L_02b9::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_02be = function(u)
  ::L_02be::
  m688.vai_a(u, 39349.12, 40283.52, 30, "L_02c9")
  m688.salva_modello(u)
  do return end
end

script.L_02c9 = function(u)
  ::L_02c9::
  m688.vai_a(u, 36362.24, 43270.4, 30, "L_02d4")
  m688.salva_modello(u)
  do return end
end

script.L_02d4 = function(u)
  ::L_02d4::
  m688.vai_a(u, 31135.2, 48497.44, 30, "L_02df")
  m688.salva_modello(u)
  do return end
end

script.L_02df = function(u)
  ::L_02df::
  m688.vai_a(u, 25908.16, 49244.16, 30, "L_02ea")
  m688.salva_modello(u)
  do return end
end

script.L_02ea = function(u)
  ::L_02ea::
  m688.vai_a(u, 24414.72, 44763.84, 30, "L_02f5")
  m688.salva_modello(u)
  do return end
end

script.L_02f5 = function(u)
  ::L_02f5::
  m688.vai_a(u, 25908.16, 41776.96, 30, "L_0300")
  m688.salva_modello(u)
  do return end
end

script.L_0300 = function(u)
  ::L_0300::
  m688.vai_a(u, 32628.64, 35056.48, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_030b = function(u)
  ::L_030b::
  m688.vai_a(u, 27401.6, 17135.2, 30, "L_0316")
  m688.salva_modello(u)
  do return end
end

script.L_0316 = function(u)
  ::L_0316::
  m688.vai_a(u, 25600, 20100, 30, "L_0321")
  m688.salva_modello(u)
  do return end
end

script.L_0321 = function(u)
  ::L_0321::
  m688.vai_a(u, 24600, 26900, 30, "L_032c")
  m688.salva_modello(u)
  do return end
end

script.L_032c = function(u)
  ::L_032c::
  m688.vai_a(u, 24600, 27900, 30, "L_0337")
  m688.salva_modello(u)
  do return end
end

script.L_0337 = function(u)
  ::L_0337::
  m688.vai_a(u, 24200, 34300, 30, "L_0342")
  m688.salva_modello(u)
  do return end
end

script.L_0342 = function(u)
  ::L_0342::
  m688.vai_a(u, 26200, 37900, 30, "L_034d")
  m688.salva_modello(u)
  do return end
end

script.L_034d = function(u)
  ::L_034d::
  m688.vai_a(u, 17694.24, 41030.24, 30, "L_0358")
  m688.salva_modello(u)
  do return end
end

script.L_0358 = function(u)
  ::L_0358::
  m688.vai_a(u, 9480.32, 49990.88, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_0363 = function(u)
  ::L_0363::
  m688.vai_a(u, 5746.72, 39536.8, 30, "L_036e")
  m688.salva_modello(u)
  do return end
end

script.L_036e = function(u)
  ::L_036e::
  m688.vai_a(u, 16000, 30500, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_0379 = function(u)
  ::L_0379::
  m688.vai_a(u, 9600, 12900, 30, "L_0384")
  m688.salva_modello(u)
  do return end
end

script.L_0384 = function(u)
  ::L_0384::
  m688.vai_a(u, 6800, 16700, 30, "L_038f")
  m688.salva_modello(u)
  do return end
end

script.L_038f = function(u)
  ::L_038f::
  m688.vai_a(u, -2000, 25300, 30, "L_039a")
  m688.salva_modello(u)
  do return end
end

script.L_039a = function(u)
  ::L_039a::
  m688.vai_a(u, -7400, 34100, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_03a5 = function(u)
  ::L_03a5::
  m688.vai_a(u, -1600, 25100, 30, "L_03b0")
  m688.salva_modello(u)
  do return end
end

script.L_03b0 = function(u)
  ::L_03b0::
  m688.vai_a(u, 6800, 16700, 30, "L_03bb")
  m688.salva_modello(u)
  do return end
end

script.L_03bb = function(u)
  ::L_03bb::
  m688.vai_a(u, 11400, 10900, 30, "L_03c6")
  m688.salva_modello(u)
  do return end
end

script.L_03c6 = function(u)
  ::L_03c6::
  m688.vai_a(u, 14707.36, 5187.68, 30, "L_03d1")
  m688.salva_modello(u)
  do return end
end

script.L_03d1 = function(u)
  ::L_03d1::
  m688.vai_a(u, 24414.72, -1532.8, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_03dc = function(u)
  ::L_03dc::
  m688.vai_a(u, -2000, 25300, 30, "L_02b9")
  if m688.caso(u, 0.7539) then goto L_03f2 end
  m688.vai_a(u, 10000, 12500, 30, "L_02b9")
  ::L_03f2::
  m688.salva_modello(u)
  do return end
end

script.L_03f4 = function(u)
  ::L_03f4::
  if m688.bersaglio_id(u, 6) then goto L_03fe end
  m688.comportamento(u, "L_03f4")
  m688.salva_modello(u)
  do return end
  ::L_03fe::
  if m688.traccia_solida(u) then goto L_0409 end
  m688.ingaggia(u, 30, "L_03fe", "L_03f9")
  m688.salva_modello(u)
  do return end
  ::L_0409::
  if m688.variabile_uguale(u, 4, 2) then goto L_041e end
  m688.ingaggia(u, 30, "L_03fe", "L_03f9")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  goto L_05f8
  ::L_041e::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 27401.6, 40283.52, 30, "L_0261")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_03f9 = function(u)
  ::L_03f9::
  m688.comportamento(u, "L_03f4")
  m688.salva_modello(u)
  do return end
end

script.L_03fe = function(u)
  ::L_03fe::
  if m688.traccia_solida(u) then goto L_0409 end
  m688.ingaggia(u, 30, "L_03fe", "L_03f9")
  m688.salva_modello(u)
  do return end
  ::L_0409::
  if m688.variabile_uguale(u, 4, 2) then goto L_041e end
  m688.ingaggia(u, 30, "L_03fe", "L_03f9")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  goto L_05f8
  ::L_041e::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 27401.6, 40283.52, 30, "L_0261")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_0439 = function(u)
  ::L_0439::
  if m688.bersaglio_id(u, 6) then goto L_0443 end
  m688.comportamento(u, "L_0439")
  m688.salva_modello(u)
  do return end
  ::L_0443::
  if m688.traccia_solida(u) then goto L_044e end
  m688.ingaggia(u, 30, "L_0443", "L_043e")
  m688.salva_modello(u)
  do return end
  ::L_044e::
  if m688.variabile_uguale(u, 4, 2) then goto L_0463 end
  m688.ingaggia(u, 30, "L_0443", "L_043e")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  goto L_05f8
  ::L_0463::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 17694.24, 29082.72, 30, "L_0219")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_043e = function(u)
  ::L_043e::
  m688.comportamento(u, "L_0439")
  m688.salva_modello(u)
  do return end
end

script.L_0443 = function(u)
  ::L_0443::
  if m688.traccia_solida(u) then goto L_044e end
  m688.ingaggia(u, 30, "L_0443", "L_043e")
  m688.salva_modello(u)
  do return end
  ::L_044e::
  if m688.variabile_uguale(u, 4, 2) then goto L_0463 end
  m688.ingaggia(u, 30, "L_0443", "L_043e")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  goto L_05f8
  ::L_0463::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.vai_a(u, 17694.24, 29082.72, 30, "L_0219")
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_047e = function(u)
  ::L_047e::
  if m688.bersaglio_id(u, 6) then goto L_0490 end
  m688.comportamento(u, "L_047e")
  m688.salva_modello(u)
  do return end
  ::L_0488::
  m688.ingaggia(u, 30, "L_047e", "L_0483")
  m688.salva_modello(u)
  do return end
  ::L_0490::
  if m688.variabile_uguale(u, 5, 0) then goto L_0488 end
  if m688.variabile_uguale(u, 4, 2) then goto L_04ac end
  m688.ingaggia(u, 30, "L_047e", "L_0483")
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  goto L_05f8
  ::L_04ac::
  m688.annulla_timer(u, "L_01f8")
  m688.telegrafo(u, 0.5)
  m688.nuovo_caso(u)
  m688.vai_a(u, 5000, 17500, 30, "L_04d5")
  if m688.caso(u, 0.5039) then goto L_04c9 end
  m688.vai_a(u, 14707.36, 5187.68, 30, "L_04d5")
  ::L_04c9::
  m688.salva_modello(u)
  m688.dopo(u, 180, "L_04e0")
  m688.dopo(u, 200, "L_051b")
  do return end
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_0483 = function(u)
  ::L_0483::
  m688.comportamento(u, "L_047e")
  m688.salva_modello(u)
  do return end
end

script.L_04d5 = function(u)
  ::L_04d5::
  m688.vai_a(u, 10000, 12500, 30, "L_02b9")
  m688.salva_modello(u)
  do return end
end

script.L_04e0 = function(u)
  ::L_04e0::
  m688.marca_contatti(u)
  m688.comportamento(u, "L_04e6")
  m688.salva_modello(u)
  do return end
end

script.L_04e6 = function(u)
  ::L_04e6::
  if m688.bersaglio_nave(u) then goto L_04ee end
  ::L_04e9::
  m688.comportamento(u, "L_04e6")
  m688.salva_modello(u)
  do return end
  ::L_04ee::
  if m688.traccia_solida(u) then goto L_04f4 end
  goto L_04e9
  ::L_04f4::
  if m688.bersaglio_vicino(u, 8) then goto L_04fc end
  goto L_04e9
  ::L_04fc::
  if m688.variabile_uguale(u, 3, 4) then goto L_04e9 end
  m688.comportamento(u, "L_04e6")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  m688.variabile_piu(u, 3)
  script.L_064b(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_0517")
  do return end
end

script.L_0517 = function(u)
  ::L_0517::
  m688.variabile_meno(u, 3)
  do return end
end

script.L_051b = function(u)
  ::L_051b::
  m688.marca_contatti(u)
  m688.dopo(u, 24, "L_051b")
  do return end
end

script.L_0522 = function(u)
  ::L_0522::
  if m688.manomesso(u) then goto L_0526 end
  do return end
  ::L_0526::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0531 = function(u)
  ::L_0531::
  if m688.in_rettangolo(u, -99540.8, 57458.08, 109540.8, -69484.32) then goto L_053f end
  m688.messaggio(u, 107)
  do return end
  ::L_053f::
  if m688.unita_esiste(u, 7) then goto L_0547 end
  m688.messaggio(u, 103)
  do return end
  ::L_0547::
  if m688.unita_esiste(u, 2) then goto L_0579 end
  if m688.unita_esiste(u, 3) then goto L_0579 end
  if m688.unita_esiste(u, 4) then goto L_0579 end
  if m688.unita_esiste(u, 5) then goto L_0579 end
  if m688.unita_esiste(u, 6) then goto L_0579 end
  do local c = m688.confronta_variabili(u, 10, 17) if c > 0 then goto L_0576 elseif c == 0 then goto L_0578 end end  -- difetto originale: uguali -> etichetta + 2
  if m688.variabile_uguale(u, 10, 0) then goto L_0573 end
  m688.messaggio(u, 105)
  do return end
  ::L_0573::
  m688.messaggio(u, 104)
  do return end
  ::L_0576::
  m688.messaggio(u, 106)
  ::L_0578::
  do return end
  ::L_0579::
  m688.dopo(u, 10, "L_0531")
  do return end
end

script.L_05f8 = function(u)
  ::L_05f8::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0602")
  do return end
end

script.L_0602 = function(u)
  ::L_0602::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_064b = function(u)
  ::L_064b::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_06e0", "L_06bd")
  script.L_0672(u)
  m688.dopo(u, 170, "L_066f")
  m688.dopo(u, 190, "L_0698")
  m688.dopo(u, 280, "L_06e0")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_066f = function(u)
  ::L_066f::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0672 = function(u)
  ::L_0672::
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

script.L_0698 = function(u)
  ::L_0698::
  m688.annulla_timer(u, "L_0698")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_06bd = function(u)
  ::L_06bd::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_06a3")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06d9")
  m688.dopo(u, 10, "L_06d1")
  do return end
end

script.L_06a3 = function(u)
  ::L_06a3::
  if m688.bersaglio_nave(u) then goto L_06ab end
  m688.comportamento(u, "L_06a3")
  m688.salva_modello(u)
  do return end
  ::L_06ab::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_06d1")
  m688.annulla_timer(u, "L_06bd")
  m688.insegui(u, 0, "L_06e0", "L_06bd")
  m688.salva_modello(u)
  do return end
end

script.L_06d1 = function(u)
  ::L_06d1::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_06d9 = function(u)
  ::L_06d9::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06d9")
  do return end
end

script.L_06e0 = function(u)
  ::L_06e0::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_06e4 = function(u)
  ::L_06e4::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_06e0", "L_06bd")
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
  m688.dopo(u, 2, "L_0711")
  m688.dopo(u, 160, "L_0716")
  do return end
end

script.L_0711 = function(u)
  ::L_0711::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0716 = function(u)
  ::L_0716::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_071a = function(u)
  ::L_071a::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_071f = function(u)
  ::L_071f::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_06bd")
  do return end
end

script.L_0729 = function(u, p)
  ::L_0729::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_0738")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0738 = function(u)
  ::L_0738::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_06a3")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0746 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0746::
  do return end
end

script.L_0747 = function(u, p)
  ::L_0747::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_0752")
  m688.salva_modello(u)
  do return end
end

script.L_0752 = function(u)
  ::L_0752::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0759 = function(u, p)
  ::L_0759::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_075e = function(u, p)
  ::L_075e::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0765 = function(u, p)
  ::L_0765::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_076d = function(u)
  ::L_076d::
  m688.annulla_timer(u, "L_07ac")
  m688.dopo(u, 20, "L_07ac")
  do return end
end

script.L_0776 = function(u)
  ::L_0776::
  m688.annulla_timer(u, "L_07af")
  m688.dopo(u, 20, "L_07af")
  do return end
end

script.L_077f = function(u)
  ::L_077f::
  m688.annulla_timer(u, "L_07b2")
  m688.dopo(u, 20, "L_07b2")
  do return end
end

script.L_0788 = function(u)
  ::L_0788::
  m688.annulla_timer(u, "L_07b5")
  m688.dopo(u, 20, "L_07b5")
  do return end
end

script.L_0791 = function(u)
  ::L_0791::
  m688.annulla_timer(u, "L_07b8")
  m688.dopo(u, 20, "L_07b8")
  do return end
end

script.L_079a = function(u)
  ::L_079a::
  m688.annulla_timer(u, "L_07bb")
  m688.dopo(u, 20, "L_07bb")
  do return end
end

script.L_07a3 = function(u)
  ::L_07a3::
  m688.annulla_timer(u, "L_07be")
  m688.dopo(u, 20, "L_07be")
  do return end
end

script.L_07ac = function(u)
  ::L_07ac::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_07af = function(u)
  ::L_07af::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_07b2 = function(u)
  ::L_07b2::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_07b5 = function(u)
  ::L_07b5::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_07b8 = function(u)
  ::L_07b8::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_07bb = function(u)
  ::L_07bb::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_07be = function(u)
  ::L_07be::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_07c1 = function(u)
  ::L_07c1::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_07c9 = function(u, p)
  ::L_07c9::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07cd = function(u)
  ::L_07cd::
  m688.dopo(u, 60, "L_07d7")
  do return end
end

script.L_07d3 = function(u, p)
  ::L_07d3::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07d7 = function(u)
  ::L_07d7::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_07dd = function(u)
  ::L_07dd::
  m688.dopo(u, 40, "L_07e3")
  do return end
end

script.L_07e3 = function(u)
  ::L_07e3::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_07e9 = function(u)
  ::L_07e9::
  m688.annulla_timer(u, "L_07d7")
  m688.annulla_timer(u, "L_07e3")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_07f3 = function(u, p)
  ::L_07f3::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07f8 = function(u, p)
  ::L_07f8::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

return missione
