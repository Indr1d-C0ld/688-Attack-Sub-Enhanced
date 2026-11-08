-- SCENE8A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE8A",
  missione = 8,
  titolo = { it = "Festa a sorpresa", en = "SURPRISE PARTY" },
  lato = "usa",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 101.4984, 259.9944, 300.5328 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_0722" },
    harpoon = { gestore = "L_0686" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_07d5" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0838",
    telegrafo = "L_084a",
    quota = "L_084f",
    timone = "L_0856",
    pilota_automatico_off = "L_08b2",
    periscopio = "L_08ba",
    carica_tubo_1 = "L_085e",
    carica_tubo_2 = "L_0867",
    carica_tubo_3 = "L_0870",
    carica_tubo_4 = "L_0879",
    carica_tubo_5 = "L_0882",
    carica_tubo_6 = "L_088b",
    carica_tubo_7 = "L_0894",
    schiera_cala = "L_08be",
    schiera_recupera = "L_08ce",
    schiera_persa = "L_08da",
    designa_bersaglio = "L_08e4",
    posizione = "L_08e9",
    antenna = "L_08c4",
    guida_e0 = "L_080b",
    guida_e1 = "L_0810",
    guida_e2 = "L_081a",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.171875, probabilita_originale = "11/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -22520.32, y = 4741.12 }, originale = { x = 4512, y = 3328 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -21335.04, y = -26076.16 }, originale = { x = 4544, y = 2496 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = "L_01bf" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_01bf" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_0411", reazione = nil, arrivo = nil } },
      { id = 7, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = 16, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -47411.2, y = -13038.08 }, { x = 30817.28, y = -13038.08 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_025b", reazione = nil, arrivo = "L_024d" } },
      },
      timer = { { dopo_s = 4000, assoluto = true, unita = 0, gestore = "L_01bf" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0238" }, { dopo_s = 120, unita = 0, gestore = "L_01f9" }, { dopo_s = 60, unita = 1, gestore = "L_037d" }, { dopo_s = 200, unita = 2, gestore = "L_0231" }, { dopo_s = 160, unita = 2, gestore = "L_0331" }, { dopo_s = 300, unita = 3, gestore = "L_0231" }, { dopo_s = 200, unita = 3, gestore = "L_0331" }, { dopo_s = 400, unita = 5, gestore = "L_0231" }, { dopo_s = 240, unita = 5, gestore = "L_0331" }, { dopo_s = 500, unita = 6, gestore = "L_0231" }, { dopo_s = 280, unita = 6, gestore = "L_0331" }, { dopo_s = 60, unita = 7, gestore = "L_0246" }, { dopo_s = 50, unita = 7, gestore = "L_034f" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, il Los Angeles farà da esploratore per il gruppo di superficie diretto a nord-est, verso il Mar di Norvegia. È indispensabile che il gruppo di superficie arrivi a destinazione senza danni. La responsabilità di proteggere le navi di superficie è sua." },
    en = { "CAPTAIN, THE LOS ANGELES IS INSTRUCTED TO ACT AS SCOUT FOR THE SURFACE GROUP HEADING NORTHEAST FOR THE NORWEGIAN SEA. IT IS IMPERATIVE THAT THE SURFACE GROUP ARRIVES AT ITS DESTINATION WITHOUT DAMAGE. IT IS YOUR RESPONSIBILITY TO PROTECT THE SKIMMERS." },
  },
  esiti = {
    [1] = { it = "CODICE: 8-1-ACP\nIeri l'USS Los Angeles si è incagliato ed è andato distrutto. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 8-1-ACP\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED YESTERDAY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 8-2-ACP\nIeri l'USS Los Angeles è stato distrutto da siluri, in seguito a un attacco a sorpresa dell'Unione Sovietica in acque internazionali. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 8-2-ACP\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY, AFTER A SURPRISE ATTACK BY THE SOVIET UNION IN INTERNATIONAL WATERS. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 8-3-ACP\nOggi un gruppo navale statunitense è stato attaccato in Atlantico. Stiamo cercando di stabilire la natura dell'incidente che ha coinvolto il gruppo da battaglia, ma si ritiene che il responsabile sia un branco di sottomarini sovietici. Il Los Angeles è sopravvissuto alla tragedia, ma non ha ancora presentato il suo rapporto sull'incidente. Condurremo un'indagine completa prima di decidere se procedere contro il comandante del Los Angeles. Fino ad allora il comandante è sollevato dall'incarico.", en = "CODE: 8-3-ACP\nA U.S. SURFACE GROUP IN THE ATLANTIC WAS ATTACKED TODAY. WE ARE CURRENTLY TRYING TO DETERMINE THE NATURE OF THE INCIDENT INVOLVING THE BATTLE GROUP, BUT IT IS BELIEVED THAT A WOLFPACK OF SOVIET SUBMARINES IS RESPONSIBLE. THE U.S. LOS ANGELES SURVIVED THIS TRAGEDY, BUT HAS NOT YET SUBMITTED ITS REPORT OF THE INCIDENT. WE WILL CONDUCT A FULL INVESTIGATION BEFORE WE DECIDE TO PRESS CHARGES AGAINST THE CAPTAIN OF THE L.A. UNTIL THAT TIME, THE CAPTAIN IS BEING RELIEVED OF DUTY." },
    [4] = { it = "CODICE: 8-4-ACP\nPrestazione eccezionale oggi, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 8-4-ACP\nOUTSTANDING PERFORMANCE TODAY CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS ON LEAVE TO YOUR FINE CREW." },
    [5] = { it = "CODICE: 8-5-ACP\nOggi un gruppo navale statunitense è stato attaccato in Atlantico. Stiamo cercando di stabilire la natura dell'incidente che ha coinvolto il gruppo da battaglia, ma si ritiene che il responsabile sia un branco di sottomarini sovietici. Il Los Angeles è sopravvissuto alla tragedia, ma non ha ancora presentato il suo rapporto sull'incidente. Condurremo un'indagine completa prima di decidere se procedere contro il comandante del Los Angeles. Fino ad allora il comandante è sollevato dall'incarico.", en = "CODE: 8-5-ACP\nA U.S. SURFACE GROUP IN THE ATLANTIC WAS ATTACKED TODAY. WE ARE CURRENTLY TRYING TO DETERMINE THE NATURE OF THE INCIDENT INVOLVING THE BATTLE GROUP, BUT IT IS BELIEVED THAT A WOLFPACK OF SOVIET SUBMARINES IS RESPONSIBLE. THE U.S. LOS ANGELES SURVIVED THIS TRAGEDY, BUT HAS NOT YET SUBMITTED ITS REPORT OF THE INCIDENT. WE WILL CONDUCT A FULL INVESTIGATION BEFORE WE DECIDE TO PRESS CHARGES AGAINST THE CAPTAIN OF THE L.A. UNTIL THAT TIME, THE CAPTAIN IS BEING RELIEVED OF DUTY." },
    [6] = { it = "CODICE: 8-6-ACP\nPrestazione eccezionale oggi, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 8-6-ACP\nOUTSTANDING PERFORMANCE TODAY CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS ON LEAVE TO YOUR FINE CREW." },
    [7] = { it = "CODICE: 8-7-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete esattamente dove diavolo stavate andando. Si porti dietro la sua roba, perché resterà qui per il prossimo mese.", en = "CODE: 8-7-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME EXACTLY WHERE IN HELL YOU WERE GOING? BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WILL BE HERE FOR THE NEXT MONTH." },
    [19] = { it = "CODICE: 8-19-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. L'ho seguita sul SOSUS: sembra che lei abbia molte cose da spiegare. Per esempio, come diavolo è riuscito ad affondare una delle navi di superficie che stava scortando...", en = "CODE: 8-19-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. I MONITORED YOU ON SOSUS, LOOKS LIKE YOU HAVE A LOT OF EXPLAINING TO DO. LIKE HOW IN HELL YOU MANAGED TO SINK A SKIMMER YOU WERE ESCORTING, FOR ONE..." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_01bf = function(u)
  ::L_01bf::
  m688.variabile(u, 3, 0)
  if m688.unita_esiste(u, 2) then goto L_01cc end
  m688.variabile_piu(u, 3)
  ::L_01cc::
  if m688.unita_esiste(u, 3) then goto L_01d4 end
  m688.variabile_piu(u, 3)
  ::L_01d4::
  if m688.unita_esiste(u, 4) then goto L_01dc end
  m688.variabile_piu(u, 3)
  ::L_01dc::
  if m688.unita_esiste(u, 5) then goto L_01e4 end
  m688.variabile_piu(u, 3)
  ::L_01e4::
  if m688.unita_esiste(u, 6) then goto L_01ec end
  m688.variabile_piu(u, 3)
  ::L_01ec::
  if m688.variabile_maggiore(u, 3, 2) then goto L_01f6 end
  m688.messaggio(u, 104)
  do return end
  ::L_01f6::
  m688.messaggio(u, 103)
  do return end
end

script.L_01f9 = function(u)
  ::L_01f9::
  if m688.unita_esiste(u, 7) then goto L_0201 end
  m688.messaggio(u, 106)
  do return end
  ::L_0201::
  if m688.unita_esiste(u, 2) then goto L_021d end
  if m688.unita_esiste(u, 3) then goto L_021d end
  if m688.unita_esiste(u, 4) then goto L_021d end
  if m688.unita_esiste(u, 5) then goto L_021d end
  if m688.unita_esiste(u, 6) then goto L_021d end
  m688.messaggio(u, 105)
  do return end
  ::L_021d::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -96007.68) then goto L_022b end
  m688.messaggio(u, 107)
  do return end
  ::L_022b::
  m688.dopo(u, 20, "L_01f9")
  do return end
end

script.L_0231 = function(u)
  ::L_0231::
  m688.marca_contatti(u)
  m688.dopo(u, 40, "L_0231")
  do return end
end

script.L_0238 = function(u)
  ::L_0238::
  if m688.manomesso(u) then goto L_023c end
  do return end
  ::L_023c::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0246 = function(u)
  ::L_0246::
  m688.marca_contatti(u)
  m688.dopo(u, 28, "L_0246")
  do return end
end

script.L_024d = function(u)
  ::L_024d::
  m688.telegrafo(u, 1.0547)
  m688.vai_a(u, 30817.28, -13038.08, 20, "L_01ba")
  m688.salva_modello(u)
  do return end
end

script.L_01ba = function(u)
  ::L_01ba::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_025b = function(u)
  ::L_025b::
  if m688.bersaglio_id(u, 0) then goto L_02df end
  if m688.bersaglio_arma(u) then goto L_026b end
  if m688.bersaglio_nave(u) then goto L_02a5 end
  m688.comportamento(u, "L_025b")
  m688.salva_modello(u)
  do return end
  ::L_026b::
  m688.comportamento(u, "L_025b")
  m688.salva_modello(u)
  if m688.bersaglio_mi_punta(u) then goto L_0273 end
  do return end
  ::L_0273::
  if m688.traccia_solida(u) then goto L_0277 end
  do return end
  ::L_0277::
  if m688.bersaglio_vicino(u, 2) then goto L_027d end
  do return end
  ::L_027d::
  m688.quota(u, 304.8, 1.0547)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 2, 0) then goto L_028e end
  do return end
  ::L_028e::
  m688.variabile(u, 2, 1)
  script.L_07d5(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 200, "L_029f")
  do return end
  ::L_02a5::
  m688.comportamento(u, "L_025b")
  if m688.flag_scena_bit(u, 1) then goto L_02b1 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 1.0547)
  ::L_02b1::
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_02b6 end
  do return end
  ::L_02b6::
  if m688.bersaglio_vicino(u, 1.8) then goto L_02cc end
  if m688.bersaglio_vicino(u, 9) then goto L_02c1 end
  do return end
  ::L_02c1::
  if m688.variabile_uguale(u, 8, 4) then goto L_02cb end
  goto L_030b
  ::L_02cb::
  do return end
  ::L_02cc::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.quota(u, 304.8, 1.0547)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  do return end
  ::L_02df::
  m688.annulla_timer(u, "L_02da")
  m688.dopo(u, 80, "L_02da")
  m688.comportamento(u, "L_025b")
  m688.flag_scena_accendi(u, 1)
  if m688.traccia_solida(u) then goto L_02f7 end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  m688.azzera_rateo_quota(u)
  m688.salva_modello(u)
  do return end
  ::L_02f7::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.uguaglia_quota(u)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 9) then goto L_0304 end
  do return end
  ::L_0304::
  if m688.variabile_uguale(u, 8, 4) then goto L_031b end
  ::L_030b::
  m688.variabile_piu(u, 8)
  m688.marca_contatti(u)
  script.L_06df(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_032d")
  do return end
  ::L_031b::
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 304.8, 1.0547)
  m688.flag_scena_spegni(u, 1)
  m688.rotta(u, nil)
  m688.rateo_virata(u, -0.72)
  m688.salva_modello(u)
  do return end
end

script.L_029f = function(u)
  ::L_029f::
  m688.variabile(u, 2, 0)
  do return end
end

script.L_02da = function(u)
  ::L_02da::
  m688.flag_scena_spegni(u, 1)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  do return end
end

script.L_032d = function(u)
  ::L_032d::
  m688.variabile_meno(u, 8)
  do return end
end

script.L_0331 = function(u)
  ::L_0331::
  if m688.flag_scena_bit(u, 1) then goto L_033a end
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  ::L_033a::
  m688.dopo(u, 50, "L_0340")
  do return end
end

script.L_0340 = function(u)
  ::L_0340::
  if m688.flag_scena_bit(u, 1) then goto L_0349 end
  m688.virata_relativa(u, -180)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  ::L_0349::
  m688.dopo(u, 320, "L_0331")
  do return end
end

script.L_034f = function(u)
  ::L_034f::
  if m688.flag_scena_bit(u, 1) then goto L_0363 end
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, -5.4)
  m688.rotta(u, nil)
  m688.quota(u, 45.72, 0.7031)
  m688.telegrafo(u, 0.3125)
  m688.salva_modello(u)
  ::L_0363::
  m688.dopo(u, 90, "L_0369")
  do return end
end

script.L_0369 = function(u)
  ::L_0363::
  m688.dopo(u, 90, "L_0369")
  do return end
  ::L_0369::
  if m688.flag_scena_bit(u, 1) then goto L_0363 end
  m688.riprendi_navigazione(u)
  m688.quota(u, 304.8, 0.7812)
  m688.telegrafo(u, 1.0547)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_034f")
  do return end
end

script.L_037d = function(u)
  ::L_037d::
  m688.nuovo_caso(u)
  m688.posizione(u, -22520.32, 4741.12)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_040a end
  m688.posizione(u, -45040.64, 11852.8)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_040a end
  m688.posizione(u, -66375.68, 5926.4)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_040a end
  m688.posizione(u, -66375.68, -13038.08)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_040a end
  m688.posizione(u, -62819.84, -29632)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_040a end
  m688.posizione(u, -41484.8, -30817.28)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_040a end
  m688.posizione(u, -21335.04, -26076.16)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_040a end
  m688.posizione(u, -17779.2, -9482.24)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_040a end
  m688.posizione(u, -35558.4, -10667.52)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_040a end
  m688.posizione(u, -52152.32, -18964.48)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_040a end
  m688.posizione(u, -54522.88, -3555.84)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_040a end
  m688.posizione(u, -42670.08, -21335.04)
  m688.rotta_ora(u, 300)
  ::L_040a::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_037d")
  do return end
end

script.L_0411 = function(u)
  ::L_0411::
  if m688.bersaglio_id(u, 7) then goto L_043e end
  if m688.bersaglio_nave(u) then goto L_0421 end
  if m688.bersaglio_arma(u) then goto L_053a end
  m688.comportamento(u, "L_0411")
  m688.salva_modello(u)
  do return end
  ::L_0421::
  if m688.unita_id(u, 2) then goto L_0430 end
  if m688.unita_id(u, 3) then goto L_0430 end
  if m688.flag_scena_bit(u, 1) then goto L_0430 end
  m688.accosta_verso_bersaglio(u)
  ::L_0430::
  m688.comportamento(u, "L_0411")
  m688.salva_modello(u)
  do return end
  ::L_043e::
  if m688.unita_id(u, 2) then goto L_0456 end
  if m688.unita_id(u, 3) then goto L_0456 end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_0435")
  m688.dopo(u, 90, "L_0435")
  ::L_0456::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_043e", "L_0439")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0462 end
  ::L_0461::
  do return end
  ::L_0462::
  if m688.bersaglio_vicino(u, 1.8) then goto L_04c9 end
  if m688.bersaglio_vicino(u, 5) then goto L_04d0 end
  if m688.variabile_uguale(u, 4, 1) then goto L_0482 end
  m688.variabile_piu(u, 4)
  script.L_0693(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04eb")
  do return end
  ::L_0482::
  if m688.variabile_uguale(u, 3, 1) then goto L_0461 end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_04f9", "L_04f9")
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
  m688.comportamento(u, "L_04ef")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_0533")
  do return end
  ::L_04c9::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_04d0::
  if m688.variabile_uguale(u, 5, 1) then goto L_0461 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_0722(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e7")
  do return end
  ::L_053a::
  m688.comportamento(u, "L_0411")
  m688.salva_modello(u)
  if m688.bersaglio_mi_punta(u) then goto L_0542 end
  do return end
  ::L_0542::
  if m688.traccia_solida(u) then goto L_0546 end
  do return end
  ::L_0546::
  if m688.bersaglio_vicino(u, 1.28) then goto L_054c end
  do return end
  ::L_054c::
  m688.nuovo_caso(u)
  if m688.caso(u, 0.2539) then goto L_0552 end
  do return end
  ::L_0552::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 19)
  do return end
end

script.L_0435 = function(u)
  ::L_0435::
  m688.flag_scena_spegni(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0439 = function(u)
  ::L_0439::
  m688.comportamento(u, "L_0411")
  m688.salva_modello(u)
  do return end
end

script.L_043e = function(u)
  ::L_043e::
  if m688.unita_id(u, 2) then goto L_0456 end
  if m688.unita_id(u, 3) then goto L_0456 end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_0435")
  m688.dopo(u, 90, "L_0435")
  ::L_0456::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_043e", "L_0439")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0462 end
  ::L_0461::
  do return end
  ::L_0462::
  if m688.bersaglio_vicino(u, 1.8) then goto L_04c9 end
  if m688.bersaglio_vicino(u, 5) then goto L_04d0 end
  if m688.variabile_uguale(u, 4, 1) then goto L_0482 end
  m688.variabile_piu(u, 4)
  script.L_0693(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04eb")
  do return end
  ::L_0482::
  if m688.variabile_uguale(u, 3, 1) then goto L_0461 end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_04f9", "L_04f9")
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
  m688.comportamento(u, "L_04ef")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_0533")
  do return end
  ::L_04c9::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_04d0::
  if m688.variabile_uguale(u, 5, 1) then goto L_0461 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_0722(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e7")
  do return end
end

script.L_04e7 = function(u)
  ::L_04e7::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_04eb = function(u)
  ::L_04eb::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_04ef = function(u)
  ::L_0461::
  do return end
  ::L_04d0::
  if m688.variabile_uguale(u, 5, 1) then goto L_0461 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_0722(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e7")
  do return end
  ::L_04ef::
  if m688.bersaglio_id(u, 7) then goto L_050a end
  m688.comportamento(u, "L_04ef")
  m688.salva_modello(u)
  do return end
  ::L_050a::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_0516 end
  ::L_050e::
  m688.ingaggia(u, 40, "L_050a", "L_04f9")
  m688.salva_modello(u)
  do return end
  ::L_0516::
  if m688.bersaglio_vicino(u, 3.2) then goto L_051e end
  goto L_050e
  ::L_051e::
  if m688.variabile_uguale(u, 5, 1) then goto L_050e end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_07d1")
  m688.salva_modello(u)
  goto L_04d0
end

script.L_04f9 = function(u)
  ::L_04f9::
  m688.dopo(u, 40, "L_04ff")
  do return end
end

script.L_04ff = function(u)
  ::L_04ff::
  m688.rotta(u, nil)
  m688.rateo_virata(u, -7.2)
  m688.comportamento(u, "L_04ef")
  m688.salva_modello(u)
  do return end
end

script.L_050a = function(u)
  ::L_0461::
  do return end
  ::L_04d0::
  if m688.variabile_uguale(u, 5, 1) then goto L_0461 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_0722(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_04e7")
  do return end
  ::L_050a::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_0516 end
  ::L_050e::
  m688.ingaggia(u, 40, "L_050a", "L_04f9")
  m688.salva_modello(u)
  do return end
  ::L_0516::
  if m688.bersaglio_vicino(u, 3.2) then goto L_051e end
  goto L_050e
  ::L_051e::
  if m688.variabile_uguale(u, 5, 1) then goto L_050e end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_07d1")
  m688.salva_modello(u)
  goto L_04d0
end

script.L_0533 = function(u)
  ::L_0533::
  m688.variabile(u, 3, 0)
  m688.togli_unita(u)
  do return end
end

script.L_0686 = function(u)
  ::L_0686::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_0690")
  do return end
end

script.L_0690 = function(u)
  ::L_0690::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0693 = function(u)
  ::L_0693::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_073b(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_classe(u, "sea_lance")
  m688.modello_difesa(u, 40)
  m688.telegrafo(u, 0.7812)
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.quota_ora(u, 0)
  m688.collega(u, 1)
  m688.quota(u, 365.76, 0.5859)
  m688.nuova_unita(u)
  m688.dopo(u, 20, "L_06c1")
  m688.dopo(u, 10, "L_07ca")
  m688.dopo(u, 240, "L_07d1")
  do return end
end

script.L_06c1 = function(u)
  ::L_06c1::
  m688.collega(u, 0)
  goto L_06c7
  ::L_06c7::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06d3")
  do return end
end

script.L_06d3 = function(u)
  ::L_06d3::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06c7")
  do return end
end

script.L_06c7 = function(u)
  ::L_06c7::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_06d3")
  do return end
end

script.L_06df = function(u)
  ::L_06df::
  m688.rumore_transitorio(u)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_07d1", "L_07a9")
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
  m688.dopo(u, 150, "L_0738")
  m688.dopo(u, 170, "L_076f")
  m688.dopo(u, 240, "L_07d1")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0722 = function(u)
  ::L_0722::
  script.L_073b(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_0738")
  m688.dopo(u, 190, "L_076f")
  m688.dopo(u, 280, "L_07d1")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0738 = function(u)
  ::L_0738::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_073b = function(u)
  ::L_073b::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_07d1", "L_07a9")
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

script.L_076f = function(u)
  ::L_076f::
  m688.annulla_timer(u, "L_076f")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_07a9 = function(u)
  ::L_07a9::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_077a")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_07ca")
  m688.dopo(u, 10, "L_07bd")
  do return end
end

script.L_077a = function(u)
  ::L_077a::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_06c7")
  m688.annulla_timer(u, "L_06d3")
  if m688.traccia_solida(u) then goto L_078d end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 40, "L_077a", "L_07a9")
  m688.salva_modello(u)
  do return end
  ::L_078d::
  if m688.bersaglio_vicino(u, 10) then goto L_0797 end
  m688.comportamento(u, "L_077a")
  m688.salva_modello(u)
  do return end
  ::L_0797::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_07bd")
  m688.annulla_timer(u, "L_07a9")
  m688.insegui(u, 0, "L_07d1", "L_07a9")
  m688.salva_modello(u)
  do return end
end

script.L_07bd = function(u)
  ::L_07bd::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_06c7")
  do return end
end

script.L_07ca = function(u)
  ::L_07ca::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_07ca")
  do return end
end

script.L_07d1 = function(u)
  ::L_07d1::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_07d5 = function(u)
  ::L_07d5::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_07d1", "L_07a9")
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
  m688.dopo(u, 2, "L_0802")
  m688.dopo(u, 160, "L_0807")
  do return end
end

script.L_0802 = function(u)
  ::L_0802::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0807 = function(u)
  ::L_0807::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_080b = function(u)
  ::L_080b::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0810 = function(u)
  ::L_0810::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_07a9")
  do return end
end

script.L_081a = function(u, p)
  ::L_081a::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_0829")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0829 = function(u)
  ::L_0829::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_077a")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0837 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0837::
  do return end
end

script.L_0838 = function(u, p)
  ::L_0838::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_0843")
  m688.salva_modello(u)
  do return end
end

script.L_0843 = function(u)
  ::L_0843::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_084a = function(u, p)
  ::L_084a::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_084f = function(u, p)
  ::L_084f::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0856 = function(u, p)
  ::L_0856::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_085e = function(u)
  ::L_085e::
  m688.annulla_timer(u, "L_089d")
  m688.dopo(u, 20, "L_089d")
  do return end
end

script.L_0867 = function(u)
  ::L_0867::
  m688.annulla_timer(u, "L_08a0")
  m688.dopo(u, 20, "L_08a0")
  do return end
end

script.L_0870 = function(u)
  ::L_0870::
  m688.annulla_timer(u, "L_08a3")
  m688.dopo(u, 20, "L_08a3")
  do return end
end

script.L_0879 = function(u)
  ::L_0879::
  m688.annulla_timer(u, "L_08a6")
  m688.dopo(u, 20, "L_08a6")
  do return end
end

script.L_0882 = function(u)
  ::L_0882::
  m688.annulla_timer(u, "L_08a9")
  m688.dopo(u, 20, "L_08a9")
  do return end
end

script.L_088b = function(u)
  ::L_088b::
  m688.annulla_timer(u, "L_08ac")
  m688.dopo(u, 20, "L_08ac")
  do return end
end

script.L_0894 = function(u)
  ::L_0894::
  m688.annulla_timer(u, "L_08af")
  m688.dopo(u, 20, "L_08af")
  do return end
end

script.L_089d = function(u)
  ::L_089d::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_08a0 = function(u)
  ::L_08a0::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_08a3 = function(u)
  ::L_08a3::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_08a6 = function(u)
  ::L_08a6::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_08a9 = function(u)
  ::L_08a9::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_08ac = function(u)
  ::L_08ac::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_08af = function(u)
  ::L_08af::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_08b2 = function(u)
  ::L_08b2::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_08ba = function(u, p)
  ::L_08ba::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08be = function(u)
  ::L_08be::
  m688.dopo(u, 60, "L_08c8")
  do return end
end

script.L_08c4 = function(u, p)
  ::L_08c4::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08c8 = function(u)
  ::L_08c8::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_08ce = function(u)
  ::L_08ce::
  m688.dopo(u, 40, "L_08d4")
  do return end
end

script.L_08d4 = function(u)
  ::L_08d4::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_08da = function(u)
  ::L_08da::
  m688.annulla_timer(u, "L_08c8")
  m688.annulla_timer(u, "L_08d4")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_08e4 = function(u, p)
  ::L_08e4::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08e9 = function(u, p)
  ::L_08e9::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

return missione
