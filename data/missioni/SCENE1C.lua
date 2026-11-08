-- SCENE1C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE1C",
  missione = 1,
  titolo = { it = "TORPEX '89", en = "TORPEX '89" },
  lato = "due_giocatori",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 91.44, 243.84, 457.2 } },
  dotazione = { mk48 = 24, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0495" },
    esca = { gestore = "L_0526" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0589",
    telegrafo = "L_059b",
    quota = "L_05a0",
    timone = "L_05a7",
    pilota_automatico_off = "L_0603",
    periscopio = "L_060b",
    carica_tubo_1 = "L_05af",
    carica_tubo_2 = "L_05b8",
    carica_tubo_3 = "L_05c1",
    carica_tubo_4 = "L_05ca",
    carica_tubo_5 = "L_05d3",
    carica_tubo_6 = "L_05dc",
    carica_tubo_7 = "L_05e5",
    schiera_cala = "L_060f",
    schiera_recupera = "L_061f",
    schiera_persa = "L_062b",
    designa_bersaglio = "L_0635",
    posizione = "L_063a",
    antenna = "L_0615",
    guida_e0 = "L_055c",
    guida_e1 = "L_0561",
    guida_e2 = "L_056b",
    f0 = "L_0641",
    f1 = "L_064a",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.25390625, probabilita_originale = "65/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 17000, y = 29500 }, originale = { x = 5664, y = 4832 }, spostamento_m = 809.9949,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 27401.6, y = 40283.52 }, originale = { x = 6080, y = 5312 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 44576.16, y = 35056.48 }, originale = { x = 6816, y = 5088 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_018e" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 46816.32, y = 35803.2 }, originale = { x = 6912, y = 5120 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_018e" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 24414.72, y = 16388.48 }, originale = { x = 5952, y = 4288 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01db" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 22921.28, y = 17135.2 }, originale = { x = 5888, y = 4320 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01db" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 9480.32, y = 44017.12 }, originale = { x = 5312, y = 5472 },
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 13213.92, y = 40283.52 }, { x = 5746.72, y = 39536.8 }, { x = 16000, y = 30500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0233" } },
      },
      timer = { { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_037b" }, { dopo_s = 300, unita = 1, gestore = "L_0360" }, { dopo_s = 30, unita = 2, gestore = "L_0359" }, { dopo_s = 60, unita = 3, gestore = "L_0359" }, { dopo_s = 90, unita = 4, gestore = "L_0359" }, { dopo_s = 90, unita = 5, gestore = "L_0359" }, { dopo_s = 90, unita = 6, gestore = "L_0359" } },
      variabili = { [1] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 8600, y = 12700 }, originale = { x = 5312, y = 4160 }, spostamento_m = 1125.7024,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 9400, y = 11900 }, originale = { x = 5376, y = 4160 }, spostamento_m = 2175.2064,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 21427.84, y = 707.36 }, originale = { x = 5824, y = 3616 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0249" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 23668, y = -786.08 }, originale = { x = 5920, y = 3552 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0249" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -7000, y = 34500 }, originale = { x = 4576, y = 4960 }, spostamento_m = 2527.6102,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0275" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -6800, y = 34500 }, originale = { x = 4640, y = 4896 }, spostamento_m = 3969.3275,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0275" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 6800, y = 15500 }, originale = { x = 5056, y = 4192 }, spostamento_m = 3560.0261,
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 5000, y = 17500 }, { x = -2000, y = 25300 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ac" } },
      },
      timer = { { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_037b" }, { dopo_s = 300, unita = 1, gestore = "L_0360" }, { dopo_s = 30, unita = 2, gestore = "L_0359" }, { dopo_s = 60, unita = 3, gestore = "L_0359" }, { dopo_s = 90, unita = 4, gestore = "L_0359" }, { dopo_s = 90, unita = 5, gestore = "L_0359" }, { dopo_s = 90, unita = 6, gestore = "L_0359" } },
      variabili = { [1] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 27401.6, y = 40283.52 }, originale = { x = 6080, y = 5312 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 17000, y = 29500 }, originale = { x = 5664, y = 4832 }, spostamento_m = 809.9949,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 44576.16, y = 35056.48 }, originale = { x = 6816, y = 5088 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_018e" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 46816.32, y = 35803.2 }, originale = { x = 6912, y = 5120 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 39349.12, y = 35056.48 }, { x = 39349.12, y = 40283.52 }, { x = 36362.24, y = 43270.4 }, { x = 31135.2, y = 48497.44 }, { x = 25908.16, y = 49244.16 }, { x = 24414.72, y = 44763.84 }, { x = 25908.16, y = 41776.96 }, { x = 32628.64, y = 35056.48 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_018e" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 24414.72, y = 16388.48 }, originale = { x = 5952, y = 4288 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01db" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 22921.28, y = 17135.2 }, originale = { x = 5888, y = 4320 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27401.6, y = 17135.2 }, { x = 27401.6, y = 17135.2 }, { x = 25400, y = 20100 }, { x = 24600, y = 26900 }, { x = 24600, y = 27700 }, { x = 24200, y = 34300 }, { x = 26200, y = 37900 }, { x = 17694.24, y = 41030.24 }, { x = 9480.32, y = 49990.88 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01db" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 9480.32, y = 44017.12 }, originale = { x = 5312, y = 5472 },
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 13213.92, y = 40283.52 }, { x = 5746.72, y = 39536.8 }, { x = 16000, y = 30500 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0233" } },
      },
      timer = { { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_037b" }, { dopo_s = 300, unita = 1, gestore = "L_0360" }, { dopo_s = 30, unita = 2, gestore = "L_0359" }, { dopo_s = 60, unita = 3, gestore = "L_0359" }, { dopo_s = 90, unita = 4, gestore = "L_0359" }, { dopo_s = 90, unita = 5, gestore = "L_0359" }, { dopo_s = 90, unita = 6, gestore = "L_0359" } },
      variabili = { [1] = 0 },
    },
    { probabilita = 0.24609375, probabilita_originale = "63/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 9400, y = 11900 }, originale = { x = 5376, y = 4160 }, spostamento_m = 2175.2064,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 8600, y = 12700 }, originale = { x = 5312, y = 4160 }, spostamento_m = 1125.7024,
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 21427.84, y = 707.36 }, originale = { x = 5824, y = 3616 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0249" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 23668, y = -786.08 }, originale = { x = 5920, y = 3552 },
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 14707.36, y = 5187.68 }, { x = 9600, y = 12900 }, { x = 6600, y = 16700 }, { x = -2000, y = 25300 }, { x = -7400, y = 34100 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0249" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -7000, y = 34500 }, originale = { x = 4576, y = 4960 }, spostamento_m = 2527.6102,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0275" } },
      { id = 5, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -6800, y = 34500 }, originale = { x = 4640, y = 4896 }, spostamento_m = 3969.3275,
        quota_m = 0, rotta = 90, velocita_kn = 16.4571,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -1600, y = 25100 }, { x = -1600, y = 25100 }, { x = 7000, y = 16300 }, { x = 11400, y = 10700 }, { x = 14707.36, y = 5187.68 }, { x = 24414.72, y = -1532.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0275" } },
      { id = 6, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 82, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = 6800, y = 15500 }, originale = { x = 5056, y = 4192 }, spostamento_m = 3560.0261,
        quota_m = 0, rotta = 90, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.8, accel_kn_s = 0.386, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 9, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 5000, y = 17500 }, { x = -2000, y = 25300 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02ac" } },
      },
      timer = { { dopo_s = 200, assoluto = true, unita = 0, gestore = "L_037b" }, { dopo_s = 300, unita = 1, gestore = "L_0360" }, { dopo_s = 30, unita = 2, gestore = "L_0359" }, { dopo_s = 60, unita = 3, gestore = "L_0359" }, { dopo_s = 90, unita = 4, gestore = "L_0359" }, { dopo_s = 90, unita = 5, gestore = "L_0359" }, { dopo_s = 90, unita = 6, gestore = "L_0359" } },
      variabili = { [1] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: TORPEX NORD ATLANTICO", "Benvenuto nell'Atlantico del Nord, Comandante. Lei darà inizio a un'esercitazione al largo dell'arcipelago delle Fær Øer. Si tratta di una competizione contro l'USS Dallas. Là fuori ci sono 5 cacciatorpediniere della classe Forrest Sherman. Vince chi ne affonda di più, e conta solo il colpo di grazia.", "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS DALLAS (SSN 700)", "OGG.: TORPEX NORD ATLANTICO", "Benvenuto nell'Atlantico del Nord, Comandante. Lei darà inizio a un'esercitazione al largo dell'arcipelago delle Fær Øer. Si tratta di una competizione contro l'USS Los Angeles. Là fuori ci sono 5 cacciatorpediniere della classe Forrest Sherman. Vince chi ne affonda di più, e conta solo il colpo di grazia." },
    en = { "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: TORPEX NLANT", "WELCOME TO THE NORTH ATLANTIC CAPTAIN. YOU ARE TO BEGIN A TRAINING EXERCISE OFF THE COAST OF THE FAEROE ISLAND CHAIN. THIS IS A COMPETITION AGAINST THE USS DALLAS. THERE ARE 5 FOREST SHERMAN CLASS DESTROYERS OUT THERE. WHOEVER SCORES THE MOST KILLS WINS, AND ONLY THE KILLING BLOW COUNTS.", "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS DALLAS (SSN 700)", "SUBJ: TORPEX NLANT", "WELCOME TO THE NORTH ATLANTIC CAPTAIN. YOU ARE TO BEGIN A TRAINING EXERCISE OFF THE COAST OF THE FAEROE ISLAND CHAIN. THIS IS A COMPETITION AGAINST THE USS LOS ANGELES. THERE ARE 5 FOREST SHERMAN CLASS DESTROYERS OUT THERE. WHOEVER SCORES THE MOST KILLS WINS, AND ONLY THE KILLING BLOW" },
  },
  esiti = {
    [1] = { it = "CODICE: 1-1-CIN\nIeri un sottomarino si è incagliato ed è andato distrutto durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio.", en = "CODE: 1-1-CIN\nA SUBMARINE RAN AGROUND AND WAS DESTROYED YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW" },
    [2] = { it = "CODICE: 1-2-CIN\nIeri un sottomarino è stato colpito da armi e distrutto durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio.", en = "CODE: 1-2-CIN\nA SUBMARINE WAS STRUCK BY WEAPONS FIRE AND WAS DESTROYED YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW" },
    [3] = { it = "CODICE: 1-3-CIN\nComandante, il suo avversario si è incagliato ed è andato distrutto durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il suo comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio.", en = "CODE: 1-3-CIN\nCAPTAIN, YOUR OPPONENT RAN AGROUND AND WAS DESTROYED DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW" },
    [4] = { it = "CODICE: 1-4-CIN\nComandante, il suo avversario è stato colpito da armi ed è andato distrutto ieri durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il suo comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio.", en = "CODE: 1-4-CIN\nCAPTAIN, YOUR OPPONENT WAS STRUCK BY WEAPONS FIRE AND WAS DESTROYED YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW" },
    [5] = { it = "CODICE: 1-5-CIN\nAffondare meno navi del suo avversario durante l'esercitazione è considerato insufficiente. Vedo che lei e i suoi ufficiali avete bisogno di ulteriore addestramento prima che vi affidiamo a tempo pieno un sottomarino da un miliardo di dollari. Si presenti nel mio ufficio al rientro in porto.", en = "CODE: 1-5-CIN\nSINKING FEWER SHIPS THAN YOUR OPPONENT DURING THE TRAINING EXERCISE IS CONSIDERED SUBSTANDARD. I CAN SEE THAT YOU AND YOUR OFFICERS ARE IN NEED OF FURTHER TRAINING BEFORE WE GIVE YOU FULL-TIME CONTROL OF A BILLION DOLLAR SUBMARINE. REPORT TO MY OFFICE UPON YOUR RETURN TO PORT." },
    [6] = { it = "CODICE: 1-6-CIN\nPrestazione eccezionale oggi al poligono, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 1-6-CIN\nOUTSTANDING PERFORMANCE ON THE RANGE TODAY, CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS OF LEAVE TO YOUR FINE CREW." },
    [7] = { it = "CODICE: 1-7-CIN\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete esattamente cosa stavate cercando di combinare là fuori.", en = "CODE: 1-7-CIN\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME EXACTLY WHAT YOU WERE TRYING TO PULL OFF OUT THERE." },
    [8] = { it = "CODICE: 1-8-CIN\nComandante, sembra che il suo avversario abbia perso il controllo del suo battello e stia uscendo dall'area dell'esercitazione. Annulliamo TORPEX 89. Rientri in porto.", en = "CODE: 1-8-CIN\nCAPTAIN, YOUR OPPONENT APPEARS TO HAVE LOST CONTROL OF HIS VESSEL, AND IS SAILING OUT OF THE EXERCISE AREA. WE ARE SCRUBBING TORPEX 89. RETURN TO PORT." },
    [19] = { it = "CODICE: 1-19-CIN\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!\nCODICE: 1-20-CIN\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 1-19-CIN\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!\nCODE: 1-20-CIN\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 12, punti = 73, spostamento_max_m = 3969.3275, non_risolte = 0 },
  script = script,
}

script.L_018e = function(u)
  ::L_018e::
  m688.vai_a(u, 39349.12, 40283.52, 30, "L_0199")
  m688.salva_modello(u)
  do return end
end

script.L_0199 = function(u)
  ::L_0199::
  m688.vai_a(u, 36362.24, 43270.4, 30, "L_01a4")
  m688.salva_modello(u)
  do return end
end

script.L_01a4 = function(u)
  ::L_01a4::
  m688.vai_a(u, 31135.2, 48497.44, 30, "L_01af")
  m688.salva_modello(u)
  do return end
end

script.L_01af = function(u)
  ::L_01af::
  m688.vai_a(u, 25908.16, 49244.16, 30, "L_01ba")
  m688.salva_modello(u)
  do return end
end

script.L_01ba = function(u)
  ::L_01ba::
  m688.vai_a(u, 24414.72, 44763.84, 30, "L_01c5")
  m688.salva_modello(u)
  do return end
end

script.L_01c5 = function(u)
  ::L_01c5::
  m688.vai_a(u, 25908.16, 41776.96, 30, "L_01d0")
  m688.salva_modello(u)
  do return end
end

script.L_01d0 = function(u)
  ::L_01d0::
  m688.vai_a(u, 32628.64, 35056.48, 30, "L_0189")
  m688.salva_modello(u)
  do return end
end

script.L_0189 = function(u)
  ::L_0189::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_01db = function(u)
  ::L_01db::
  m688.vai_a(u, 27401.6, 17135.2, 30, "L_01e6")
  m688.salva_modello(u)
  do return end
end

script.L_01e6 = function(u)
  ::L_01e6::
  m688.vai_a(u, 25600, 20100, 30, "L_01f1")
  m688.salva_modello(u)
  do return end
end

script.L_01f1 = function(u)
  ::L_01f1::
  m688.vai_a(u, 24600, 26900, 30, "L_01fc")
  m688.salva_modello(u)
  do return end
end

script.L_01fc = function(u)
  ::L_01fc::
  m688.vai_a(u, 24600, 27900, 30, "L_0207")
  m688.salva_modello(u)
  do return end
end

script.L_0207 = function(u)
  ::L_0207::
  m688.vai_a(u, 24200, 34300, 30, "L_0212")
  m688.salva_modello(u)
  do return end
end

script.L_0212 = function(u)
  ::L_0212::
  m688.vai_a(u, 26200, 37900, 30, "L_021d")
  m688.salva_modello(u)
  do return end
end

script.L_021d = function(u)
  ::L_021d::
  m688.vai_a(u, 17694.24, 41030.24, 30, "L_0228")
  m688.salva_modello(u)
  do return end
end

script.L_0228 = function(u)
  ::L_0228::
  m688.vai_a(u, 9480.32, 49990.88, 30, "L_0189")
  m688.salva_modello(u)
  do return end
end

script.L_0233 = function(u)
  ::L_0233::
  m688.vai_a(u, 5746.72, 39536.8, 30, "L_023e")
  m688.salva_modello(u)
  do return end
end

script.L_023e = function(u)
  ::L_023e::
  m688.vai_a(u, 16000, 30500, 30, "L_0189")
  m688.salva_modello(u)
  do return end
end

script.L_0249 = function(u)
  ::L_0249::
  m688.vai_a(u, 9600, 12900, 30, "L_0254")
  m688.salva_modello(u)
  do return end
end

script.L_0254 = function(u)
  ::L_0254::
  m688.vai_a(u, 6800, 16700, 30, "L_025f")
  m688.salva_modello(u)
  do return end
end

script.L_025f = function(u)
  ::L_025f::
  m688.vai_a(u, -2000, 25300, 30, "L_026a")
  m688.salva_modello(u)
  do return end
end

script.L_026a = function(u)
  ::L_026a::
  m688.vai_a(u, -7400, 34100, 30, "L_0189")
  m688.salva_modello(u)
  do return end
end

script.L_0275 = function(u)
  ::L_0275::
  m688.vai_a(u, -1600, 25100, 30, "L_0280")
  m688.salva_modello(u)
  do return end
end

script.L_0280 = function(u)
  ::L_0280::
  m688.vai_a(u, 6800, 16700, 30, "L_028b")
  m688.salva_modello(u)
  do return end
end

script.L_028b = function(u)
  ::L_028b::
  m688.vai_a(u, 11400, 10900, 30, "L_0296")
  m688.salva_modello(u)
  do return end
end

script.L_0296 = function(u)
  ::L_0296::
  m688.vai_a(u, 14707.36, 5187.68, 30, "L_02a1")
  m688.salva_modello(u)
  do return end
end

script.L_02a1 = function(u)
  ::L_02a1::
  m688.vai_a(u, 24414.72, -1532.8, 30, "L_0189")
  m688.salva_modello(u)
  do return end
end

script.L_02ac = function(u)
  ::L_02ac::
  m688.vai_a(u, -2000, 25300, 30, "L_0189")
  if m688.caso(u, 0.7539) then goto L_02c2 end
  m688.vai_a(u, 10000, 12500, 30, "L_0189")
  ::L_02c2::
  m688.salva_modello(u)
  do return end
end

script.L_0359 = function(u)
  ::L_0359::
  m688.marca_contatti(u)
  m688.dopo(u, 24, "L_0359")
  do return end
end

script.L_0360 = function(u)
  ::L_0360::
  if m688.in_rettangolo(u, -99540.8, 57458.08, 109540.8, -69484.32) then goto L_0374 end
  if m688.giocatore_secondo(u) then goto L_0371 end
  m688.messaggio(u, 108)
  do return end
  ::L_0371::
  m688.messaggio(u, 107)
  do return end
  ::L_0374::
  m688.dopo(u, 50, "L_0360")
  do return end
end

script.L_037b = function(u)
  ::L_037b::
  if m688.in_rettangolo(u, -99540.8, 57458.08, 109540.8, -69484.32) then goto L_038f end
  if m688.giocatore_secondo(u) then goto L_038c end
  m688.messaggio(u, 107)
  do return end
  ::L_038c::
  m688.messaggio(u, 108)
  do return end
  ::L_038f::
  if m688.unita_esiste(u, 2) then goto L_03c3 end
  if m688.unita_esiste(u, 3) then goto L_03c3 end
  if m688.unita_esiste(u, 4) then goto L_03c3 end
  if m688.unita_esiste(u, 5) then goto L_03c3 end
  if m688.unita_esiste(u, 6) then goto L_03c3 end
  do local c = m688.confronta_variabili(u, 10, 11) if c > 0 then goto L_03ba elseif c == 0 then goto L_03bc end end  -- difetto originale: uguali -> etichetta + 2
  if m688.giocatore_secondo(u) then goto L_03b7 end
  m688.messaggio(u, 105)
  do return end
  ::L_03b7::
  m688.messaggio(u, 106)
  do return end
  ::L_03ba::
  if m688.giocatore_secondo(u) then goto L_03c0 end
  m688.messaggio(u, 106)
  do return end
  ::L_03c0::
  m688.messaggio(u, 105)
  do return end
  ::L_03c3::
  m688.dopo(u, 20, "L_037b")
  do return end
  ::L_03bc:: do return end  -- destinazione fuori dal flusso
end

script.L_0495 = function(u)
  ::L_0495::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0522", "L_04ff")
  script.L_04bc(u)
  m688.dopo(u, 170, "L_04b9")
  m688.dopo(u, 190, "L_04e2")
  m688.dopo(u, 280, "L_0522")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_04b9 = function(u)
  ::L_04b9::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_04bc = function(u)
  ::L_04bc::
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

script.L_04e2 = function(u)
  ::L_04e2::
  m688.annulla_timer(u, "L_04e2")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_04ff = function(u)
  ::L_04ff::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_04ed")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_051b")
  m688.dopo(u, 10, "L_0513")
  do return end
end

script.L_04ed = function(u)
  ::L_04ed::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0513")
  m688.annulla_timer(u, "L_04ff")
  m688.insegui(u, 0, "L_0522", "L_04ff")
  m688.salva_modello(u)
  do return end
end

script.L_0513 = function(u)
  ::L_0513::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_051b = function(u)
  ::L_051b::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_051b")
  do return end
end

script.L_0522 = function(u)
  ::L_0522::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0526 = function(u)
  ::L_0526::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0522", "L_04ff")
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
  m688.dopo(u, 2, "L_0553")
  m688.dopo(u, 160, "L_0558")
  do return end
end

script.L_0553 = function(u)
  ::L_0553::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0558 = function(u)
  ::L_0558::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_055c = function(u)
  ::L_055c::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0561 = function(u)
  ::L_0561::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_04ff")
  do return end
end

script.L_056b = function(u, p)
  ::L_056b::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_057a")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_057a = function(u)
  ::L_057a::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_04ed")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0588 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0588::
  do return end
end

script.L_0589 = function(u, p)
  ::L_0589::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_0594")
  m688.salva_modello(u)
  do return end
end

script.L_0594 = function(u)
  ::L_0594::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_059b = function(u, p)
  ::L_059b::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_05a0 = function(u, p)
  ::L_05a0::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_05a7 = function(u, p)
  ::L_05a7::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_05af = function(u)
  ::L_05af::
  m688.annulla_timer(u, "L_05ee")
  m688.dopo(u, 20, "L_05ee")
  do return end
end

script.L_05b8 = function(u)
  ::L_05b8::
  m688.annulla_timer(u, "L_05f1")
  m688.dopo(u, 20, "L_05f1")
  do return end
end

script.L_05c1 = function(u)
  ::L_05c1::
  m688.annulla_timer(u, "L_05f4")
  m688.dopo(u, 20, "L_05f4")
  do return end
end

script.L_05ca = function(u)
  ::L_05ca::
  m688.annulla_timer(u, "L_05f7")
  m688.dopo(u, 20, "L_05f7")
  do return end
end

script.L_05d3 = function(u)
  ::L_05d3::
  m688.annulla_timer(u, "L_05fa")
  m688.dopo(u, 20, "L_05fa")
  do return end
end

script.L_05dc = function(u)
  ::L_05dc::
  m688.annulla_timer(u, "L_05fd")
  m688.dopo(u, 20, "L_05fd")
  do return end
end

script.L_05e5 = function(u)
  ::L_05e5::
  m688.annulla_timer(u, "L_0600")
  m688.dopo(u, 20, "L_0600")
  do return end
end

script.L_05ee = function(u)
  ::L_05ee::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_05f1 = function(u)
  ::L_05f1::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_05f4 = function(u)
  ::L_05f4::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_05f7 = function(u)
  ::L_05f7::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_05fa = function(u)
  ::L_05fa::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_05fd = function(u)
  ::L_05fd::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0600 = function(u)
  ::L_0600::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0603 = function(u)
  ::L_0603::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_060b = function(u, p)
  ::L_060b::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_060f = function(u)
  ::L_060f::
  m688.dopo(u, 60, "L_0619")
  do return end
end

script.L_0615 = function(u, p)
  ::L_0615::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0619 = function(u)
  ::L_0619::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_061f = function(u)
  ::L_061f::
  m688.dopo(u, 40, "L_0625")
  do return end
end

script.L_0625 = function(u)
  ::L_0625::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_062b = function(u)
  ::L_062b::
  m688.annulla_timer(u, "L_0619")
  m688.annulla_timer(u, "L_0625")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0635 = function(u, p)
  ::L_0635::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_063a = function(u, p)
  ::L_063a::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

script.L_0641 = function(u)
  ::L_0641::
  if m688.giocatore_secondo(u) then goto L_0647 end
  m688.messaggio(u, 25)
  do return end
  ::L_0647::
  m688.messaggio(u, 29)
  do return end
end

script.L_064a = function(u)
  ::L_064a::
  if m688.giocatore_secondo(u) then goto L_0650 end
  m688.messaggio(u, 29)
  do return end
  ::L_0650::
  m688.messaggio(u, 25)
  do return end
end

return missione
