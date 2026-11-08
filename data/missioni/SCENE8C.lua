-- SCENE8C: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE8C",
  missione = 8,
  titolo = { it = "Festa a sorpresa", en = "SURPRISE PARTY" },
  lato = "due_giocatori",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 101.4984, 259.9944, 300.5328 } },
  dotazione = { mk48 = 24, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_05fc" },
    esca = { gestore = "L_06af" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0712",
    telegrafo = "L_0724",
    quota = "L_0729",
    timone = "L_0730",
    pilota_automatico_off = "L_078c",
    periscopio = "L_0794",
    carica_tubo_1 = "L_0738",
    carica_tubo_2 = "L_0741",
    carica_tubo_3 = "L_074a",
    carica_tubo_4 = "L_0753",
    carica_tubo_5 = "L_075c",
    carica_tubo_6 = "L_0765",
    carica_tubo_7 = "L_076e",
    schiera_cala = "L_0798",
    schiera_recupera = "L_07a8",
    schiera_persa = "L_07b4",
    designa_bersaglio = "L_07be",
    posizione = "L_07c3",
    antenna = "L_079e",
    guida_e0 = "L_06e5",
    guida_e1 = "L_06ea",
    guida_e2 = "L_06f4",
    f0 = "L_07ca",
    f1 = "L_07d3",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.171875, probabilita_originale = "11/64", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -22520.32, y = 4741.12 }, originale = { x = 4512, y = 3328 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1640625, probabilita_originale = "21/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -21335.04, y = -26076.16 }, originale = { x = 4544, y = 2496 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1328125, probabilita_originale = "17/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.1015625, probabilita_originale = "13/128", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -35558.4, y = -10667.52 }, originale = { x = 4160, y = 2912 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -47411.2, y = -7111.68 }, originale = { x = 3840, y = 3008 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41484.8, y = -30817.28 }, originale = { x = 4000, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.06640625, probabilita_originale = "17/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -52152.32, y = -18964.48 }, originale = { x = 3712, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -66375.68, y = 5926.4 }, originale = { x = 3328, y = 3360 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -59264, y = -18964.48 }, originale = { x = 3520, y = 2688 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -62819.84, y = -30817.28 }, originale = { x = 3424, y = 2368 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
    { probabilita = 0.03125, probabilita_originale = "1/32", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = -54522.88, y = -3555.84 }, originale = { x = 3648, y = 3104 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -45040.64, y = 11852.8 }, originale = { x = 3904, y = 3520 },
        quota_m = 304.8, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 19, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "nimitz", tipo_originale = 10, nome_originale = "NIMITZ CLASS AIRCRAFT CARRIER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -49781.76, y = -10667.52 }, originale = { x = 3776, y = 2912 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 220, rumore = 25, sonar = 4, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -10667.52 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 3, classe = "iowa", tipo_originale = 11, nome_originale = "IOWA CLASS BATTLESHIP", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -16593.92 }, originale = { x = 3872, y = 2752 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 15, sonar = 8, difesa = 17, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -16593.92 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = "L_019b" } },
      { id = 4, classe = "sacramento", tipo_originale = 14, nome_originale = "SACRAMENTO CLASS OILER", categoria = "nave", schieramento = "usa", flag = 74, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -43855.36, y = -11852.8 }, originale = { x = 3936, y = 2880 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 22, sonar = 5, difesa = 5, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 30817.28, y = -11852.8 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_019b" } },
      { id = 5, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -46225.92, y = -4741.12 }, originale = { x = 3872, y = 3072 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 29.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 15, sonar = 8, difesa = 15, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 6, classe = "kidd", tipo_originale = 12, nome_originale = "KIDD CLASS GUIDED MISSILE DESTROYER", categoria = "nave", schieramento = "usa", flag = 10, tipo = 2,
        posizione = { x = -42670.08, y = -21335.04 }, originale = { x = 3968, y = 2624 },
        quota_m = 0, rotta = 90, velocita_kn = 13.7143,
        modello = { massima_kn = 33.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 15, sonar = 8, difesa = 16, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = "L_02e8", reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -47411.2, y = -24890.88 }, originale = { x = 3840, y = 2528 },
        quota_m = 0, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 8, difesa = 16, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 3 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 120, unita = 0, gestore = "L_01be" }, { dopo_s = 180, unita = 1, gestore = "L_01a4" }, { dopo_s = 200, unita = 2, gestore = "L_01fa" }, { dopo_s = 160, unita = 2, gestore = "L_0208" }, { dopo_s = 300, unita = 3, gestore = "L_01fa" }, { dopo_s = 200, unita = 3, gestore = "L_0208" }, { dopo_s = 400, unita = 5, gestore = "L_01fa" }, { dopo_s = 240, unita = 5, gestore = "L_0208" }, { dopo_s = 500, unita = 6, gestore = "L_01fa" }, { dopo_s = 280, unita = 6, gestore = "L_0208" }, { dopo_s = 60, unita = 7, gestore = "L_0254" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0, [5] = 0, [6] = 0, [7] = 0, [8] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, il Los Angeles farà da esploratore per il gruppo di superficie diretto a nord-est, verso il Mar di Norvegia. È indispensabile che il gruppo di superficie arrivi a destinazione senza danni. La responsabilità di proteggere le navi di superficie è sua.", "VICEAMMIRAGLIO R.W. RATCLIFF III", "Compagni, è indispensabile impedire che i rinforzi americani raggiungano le forze NATO nel Mar di Norvegia. Pattugliate le rotte marittime e distruggete tutte le unità NATO che incontrate!" },
    en = { "CAPTAIN, THE LOS ANGELES IS INSTRUCTED TO ACT AS SCOUT FOR THE SURFACE GROUP HEADING NORTHEAST FOR THE NORWEGIAN SEA. IT IS IMPERATIVE THAT THE SURFACE GROUP ARRIVES AT ITS DESTINATION WITHOUT DAMAGE. IT IS YOUR RESPONSIBILITY TO PROTECT THE SKIMMERS.", "VADM R.W. RATCLIFF III", "COMRADES, IT IS IMPERATIVE THAT YOU PREVENT AMERICAN SUPPORT FROM REACHING NATO FORCES IN THE NORWEGIAN SEA. PATROL THE SEA LANES AND DESTROY ALL NATO VESSELS YOU ENCOUNTER!" },
  },
  esiti = {
    [1] = { it = "CODICE: 8-1-CIN\nIeri l'USS Los Angeles si è incagliato ed è andato distrutto. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 8-1-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED YESTERDAY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 8-2-CIN\nIeri l'USS Los Angeles è stato distrutto da siluri, in seguito a un attacco a sorpresa dell'Unione Sovietica in acque internazionali. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 8-2-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY, AFTER A SURPRISE ATTACK BY THE SOVIET UNION IN INTERNATIONAL WATERS. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 8-3-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare.", en = "CODE: 8-3-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA." },
    [4] = { it = "CODICE: 8-4-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare.", en = "CODE: 8-4-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA." },
    [5] = { it = "CODICE: 8-5-CIN\nPrestazione eccezionale oggi, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 8-5-CIN\nOUTSTANDING PERFORMANCE TODAY CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS ON LEAVE TO YOUR FINE CREW." },
    [6] = { it = "CODICE: 8-6-CIN\nCompagni, gli imperialisti hanno scacciato il vostro attacco come fosse un moscerino. Forse il Kapitan del Leninskii Komsomol non è adeguatamente motivato? Se la prestazione di questo pietoso equipaggio non migliorerà, questo Kapitan si pentirà di aver avuto bisogno di motivazioni. Questo sottomarino rientri immediatamente in porto, ma nessuna parata accoglierà il Leninskii Komsomol, nessun bambino saluterà il ritorno del padre e nessuna giovane donna, calda e morbida, desidererà ardentemente gli uomini di questo equipaggio. Me ne occuperò io!", en = "CODE: 8-6-CIN\nCOMRADES, THE IMPERIALISTS BRUSHED AWAY YOUR ATTACK AS IF IT WERE A TINY SEA GNAT. PERHAPS THE KAPITAN OF THE LENINSKII KOMSOMOL IS NOT PROPERLY MOTIVATED? IF THE PERFORMANCE OF THIS PITIFUL CREW DOES NOT IMPROVE, THIS KAPITAN SHALL WISH HE DID NOT REQUIRE MOTIVATION. THIS SUBMARINE SHALL RETURN TO PORT IMMEDIATELY, BUT NO PARADE SHALL GREET THE LENINSKII KOMSOMOL, NO CHILDREN SHALL WAVE TO THEIR FATHER'S RETURN, AND NO WARM SOFT YOUNG WOMEN SHALL LONGINGLY DESIRE MEN OF THIS CREW. I WILL SEE TO THAT!" },
    [7] = { it = "CODICE: 8-7-CIN\nLei ha lasciato l'area di gioco: missione annullata.", en = "CODE: 8-7-CIN\nYOU LEFT THE PLAYFIELD MISSION ABORTED." },
    [8] = { it = "CODICE: 8-8-CIN\nIl suo avversario ha lasciato la mappa: missione annullata.", en = "CODE: 8-8-CIN\nYOUR OPPONENT LEFT THE MAP MISSION ABORTED." },
    [9] = { it = "CODICE: 8-9-CIN\nOggi un gruppo navale statunitense è stato attaccato in Atlantico. Stiamo cercando di stabilire la natura dell'incidente che ha coinvolto il gruppo da battaglia, ma si ritiene che il responsabile sia un branco di sottomarini sovietici. Il Los Angeles è sopravvissuto alla tragedia, ma non ha ancora presentato il suo rapporto sull'incidente. Condurremo un'indagine completa prima di decidere se procedere contro il comandante del Los Angeles. Fino ad allora il comandante è sollevato dall'incarico.", en = "CODE: 8-9-CIN\nA U.S. SURFACE GROUP IN THE ATLANTIC WAS ATTACKED TODAY. WE ARE CURRENTLY TRYING TO DETERMINE THE NATURE OF THE INCIDENT INVOLVING THE BATTLE GROUP, BUT IT IS BELIEVED THAT A WOLFPACK OF SOVIET SUBMARINES IS RESPONSIBLE. THE U.S. LOS ANGELES SURVIVED THIS TRAGEDY, BUT HAS NOT YET SUBMITTED ITS REPORT OF THE INCIDENT. WE WILL CONDUCT A FULL INVESTIGATION BEFORE WE DECIDE TO PRESS CHARGES AGAINST THE CAPTAIN OF THE L.A. UNTIL THAT TIME, THE CAPTAIN IS BEING RELIEVED OF DUTY." },
    [10] = { it = "CODICE: 8-10-CIN\nCompagni! Rendiamo onore al Leninskii Komsomol per la sua prestazione senza precedenti in Atlantico! L'equipaggio del Leninskii Komsomol sia d'esempio per tutti i nostri valorosi combattenti!", en = "CODE: 8-10-CIN\nCOMRADES! WE HAIL THE LENINSKII KOMSOMOL FOR HER UNPRECEDENTED PERFORMANCE IN THE ATLANTIC! LET THE CREW OF THE LENINSKII KOMSOMOL BE AN EXAMPLE TO ALL OF OUR BRAVE FIGHTING MEN!" },
    [15] = { it = "CODICE: 8-15-CIN\nIl Leninskii Komsomol si è incagliato ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 8-15-CIN\nTHE LENINSKII KOMSOMOL RAN AGROUND AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [16] = { it = "CODICE: 8-16-CIN\nIl Leninskii Komsomol è stato colpito da siluri americani ed è andato perduto in mare mentre difendeva coraggiosamente i diritti del popolo sovietico. Piangiamo la perdita di questo equipaggio valoroso e onorato.", en = "CODE: 8-16-CIN\nTHE LENINSKII KOMSOMOL WAS HIT BY AMERICAN TORPEDO FIRE AND WAS LOST AT SEA WHILE COURAGEOUSLY DEFENDING THE RIGHTS OF THE SOVIET PEOPLE. WE MOURN THE LOSS OF THIS BRAVE AND HONORABLE CREW." },
    [17] = { it = "CODICE: 8-17-CIN\nL'USS Los Angeles si è incagliato ed è andato distrutto.", en = "CODE: 8-17-CIN\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED." },
    [18] = { it = "CODICE: 8-18-CIN\nIeri l'USS Los Angeles è stato distrutto da siluri.", en = "CODE: 8-18-CIN\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY." },
    [19] = { it = "CODICE: 8-19-CIN\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. L'ho seguita sul SOSUS: sembra che lei abbia molte cose da spiegare. Per esempio, come diavolo è riuscito ad affondare una delle navi di superficie che stava scortando...\nCODICE: 8-20-CIN\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di identificare i suoi bersagli prima di aprire il fuoco! Rientri immediatamente in porto.", en = "CODE: 8-19-CIN\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. I MONITORED YOU ON SOSUS, LOOKS LIKE YOU HAVE A LOT OF EXPLAINING TO DO. LIKE HOW IN HELL YOU MANAGED TO SINK A SKIMMER YOU WERE ESCORTING, FOR ONE...\nCODE: 8-20-CIN\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE IDENTITY OF HIS TARGETS BEFORE FIRING! RETURN TO PORT IMMEDIATELY." },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_019b = function(u)
  ::L_019b::
  if m688.giocatore_secondo(u) then goto L_01a1 end
  m688.messaggio(u, 105)
  do return end
  ::L_01a1::
  m688.messaggio(u, 106)
  do return end
end

script.L_01a4 = function(u)
  ::L_01a4::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -96007.68) then goto L_01b8 end
  if m688.giocatore_secondo(u) then goto L_01b5 end
  m688.messaggio(u, 108)
  do return end
  ::L_01b5::
  m688.messaggio(u, 107)
  do return end
  ::L_01b8::
  m688.dopo(u, 20, "L_01a4")
  do return end
end

script.L_01be = function(u)
  ::L_01be::
  if m688.unita_esiste(u, 2) then goto L_01e0 end
  if m688.unita_esiste(u, 3) then goto L_01e0 end
  if m688.unita_esiste(u, 4) then goto L_01e0 end
  if m688.unita_esiste(u, 5) then goto L_01e0 end
  if m688.unita_esiste(u, 6) then goto L_01e0 end
  if m688.giocatore_secondo(u) then goto L_01dd end
  m688.messaggio(u, 109)
  do return end
  ::L_01dd::
  m688.messaggio(u, 110)
  do return end
  ::L_01e0::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 165939.2, -96007.68) then goto L_01f4 end
  if m688.giocatore_secondo(u) then goto L_01f1 end
  m688.messaggio(u, 107)
  do return end
  ::L_01f1::
  m688.messaggio(u, 108)
  do return end
  ::L_01f4::
  m688.dopo(u, 20, "L_01be")
  do return end
end

script.L_01fa = function(u)
  ::L_01fa::
  m688.marca_contatti(u)
  m688.dopo(u, 40, "L_01fa")
  do return end
end

script.L_0208 = function(u)
  ::L_0208::
  if m688.flag_scena_bit(u, 1) then goto L_0211 end
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  ::L_0211::
  m688.dopo(u, 50, "L_0217")
  do return end
end

script.L_0217 = function(u)
  ::L_0217::
  if m688.flag_scena_bit(u, 1) then goto L_0220 end
  m688.virata_relativa(u, -180)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  ::L_0220::
  m688.dopo(u, 320, "L_0208")
  do return end
end

script.L_0254 = function(u)
  ::L_0254::
  m688.nuovo_caso(u)
  m688.posizione(u, -22520.32, 4741.12)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_02e1 end
  m688.posizione(u, -45040.64, 11852.8)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_02e1 end
  m688.posizione(u, -66375.68, 5926.4)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_02e1 end
  m688.posizione(u, -66375.68, -13038.08)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_02e1 end
  m688.posizione(u, -62819.84, -29632)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_02e1 end
  m688.posizione(u, -41484.8, -30817.28)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_02e1 end
  m688.posizione(u, -21335.04, -26076.16)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_02e1 end
  m688.posizione(u, -17779.2, -9482.24)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_02e1 end
  m688.posizione(u, -35558.4, -10667.52)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_02e1 end
  m688.posizione(u, -52152.32, -18964.48)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_02e1 end
  m688.posizione(u, -54522.88, -3555.84)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_02e1 end
  m688.posizione(u, -42670.08, -21335.04)
  m688.rotta_ora(u, 300)
  ::L_02e1::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0254")
  do return end
end

script.L_02e8 = function(u)
  ::L_02e8::
  if m688.bersaglio_id(u, 1) then goto L_0315 end
  if m688.bersaglio_nave(u) then goto L_02f8 end
  if m688.bersaglio_arma(u) then goto L_0414 end
  m688.comportamento(u, "L_02e8")
  m688.salva_modello(u)
  do return end
  ::L_02f8::
  if m688.unita_id(u, 2) then goto L_0307 end
  if m688.unita_id(u, 3) then goto L_0307 end
  if m688.flag_scena_bit(u, 1) then goto L_0307 end
  m688.accosta_verso_bersaglio(u)
  ::L_0307::
  m688.comportamento(u, "L_02e8")
  m688.salva_modello(u)
  do return end
  ::L_0315::
  if m688.unita_id(u, 2) then goto L_032d end
  if m688.unita_id(u, 3) then goto L_032d end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_030c")
  m688.dopo(u, 90, "L_030c")
  ::L_032d::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_0315", "L_0310")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0339 end
  ::L_0338::
  do return end
  ::L_0339::
  if m688.bersaglio_vicino(u, 1.8) then goto L_03a3 end
  if m688.bersaglio_vicino(u, 5) then goto L_03aa end
  if m688.variabile_uguale(u, 4, 1) then goto L_0359 end
  m688.variabile_piu(u, 4)
  script.L_056d(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c5")
  do return end
  ::L_0359::
  if m688.variabile_uguale(u, 3, 1) then goto L_0338 end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_03d3", "L_03d3")
  m688.modello_tipo(u, 2)
  m688.quota_ora(u, 0)
  m688.modello_rateo_quota(u, 0.3429)
  m688.modello_virata(u, 19.8)
  m688.modello_accelerazione(u, 2.5714)
  m688.modello_avanti_tutta(u, 82.2857)
  m688.velocita_ora(u, 82.2857)
  m688.schiera_trainata(u, 1)
  m688.flag_scena(u, 0)
  m688.modello_rumore(u, 0)
  m688.modello_sonar(u, 35)
  m688.modello_integrita(u, 20)
  m688.modello_quota_max(u, 0)
  m688.modello_4c(u, 2)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_03c9")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_040d")
  do return end
  ::L_03a3::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_03aa::
  if m688.variabile_uguale(u, 5, 1) then goto L_0338 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_05fc(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c1")
  do return end
  ::L_0414::
  m688.comportamento(u, "L_02e8")
  m688.salva_modello(u)
  if m688.bersaglio_mi_punta(u) then goto L_041c end
  do return end
  ::L_041c::
  if m688.traccia_solida(u) then goto L_0420 end
  do return end
  ::L_0420::
  if m688.bersaglio_vicino(u, 1.28) then goto L_0426 end
  do return end
  ::L_0426::
  m688.nuovo_caso(u)
  if m688.caso(u, 0.2539) then goto L_042c end
  do return end
  ::L_042c::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 19)
  do return end
end

script.L_030c = function(u)
  ::L_030c::
  m688.flag_scena_spegni(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0310 = function(u)
  ::L_0310::
  m688.comportamento(u, "L_02e8")
  m688.salva_modello(u)
  do return end
end

script.L_0315 = function(u)
  ::L_0315::
  if m688.unita_id(u, 2) then goto L_032d end
  if m688.unita_id(u, 3) then goto L_032d end
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 1)
  m688.telegrafo(u, 0.5)
  m688.annulla_timer(u, "L_030c")
  m688.dopo(u, 90, "L_030c")
  ::L_032d::
  m688.impulso_sonar(u)
  m688.ingaggia(u, 40, "L_0315", "L_0310")
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_0339 end
  ::L_0338::
  do return end
  ::L_0339::
  if m688.bersaglio_vicino(u, 1.8) then goto L_03a3 end
  if m688.bersaglio_vicino(u, 5) then goto L_03aa end
  if m688.variabile_uguale(u, 4, 1) then goto L_0359 end
  m688.variabile_piu(u, 4)
  script.L_056d(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c5")
  do return end
  ::L_0359::
  if m688.variabile_uguale(u, 3, 1) then goto L_0338 end
  m688.variabile(u, 3, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "sh60b")
  m688.modello_flag(u, 11)
  m688.lancia(u, "L_03d3", "L_03d3")
  m688.modello_tipo(u, 2)
  m688.quota_ora(u, 0)
  m688.modello_rateo_quota(u, 0.3429)
  m688.modello_virata(u, 19.8)
  m688.modello_accelerazione(u, 2.5714)
  m688.modello_avanti_tutta(u, 82.2857)
  m688.velocita_ora(u, 82.2857)
  m688.schiera_trainata(u, 1)
  m688.flag_scena(u, 0)
  m688.modello_rumore(u, 0)
  m688.modello_sonar(u, 35)
  m688.modello_integrita(u, 20)
  m688.modello_quota_max(u, 0)
  m688.modello_4c(u, 2)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 0)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_03c9")
  m688.nuova_unita(u)
  m688.dopo(u, 600, "L_040d")
  do return end
  ::L_03a3::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
  ::L_03aa::
  if m688.variabile_uguale(u, 5, 1) then goto L_0338 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_05fc(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c1")
  do return end
end

script.L_03c1 = function(u)
  ::L_03c1::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_03c5 = function(u)
  ::L_03c5::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_03c9 = function(u)
  ::L_0338::
  do return end
  ::L_03aa::
  if m688.variabile_uguale(u, 5, 1) then goto L_0338 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_05fc(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c1")
  do return end
  ::L_03c9::
  if m688.bersaglio_id(u, 1) then goto L_03e4 end
  m688.comportamento(u, "L_03c9")
  m688.salva_modello(u)
  do return end
  ::L_03e4::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_03f0 end
  ::L_03e8::
  m688.ingaggia(u, 40, "L_03e4", "L_03d3")
  m688.salva_modello(u)
  do return end
  ::L_03f0::
  if m688.bersaglio_vicino(u, 3.2) then goto L_03f8 end
  goto L_03e8
  ::L_03f8::
  if m688.variabile_uguale(u, 5, 1) then goto L_03e8 end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_06ab")
  m688.salva_modello(u)
  goto L_03aa
end

script.L_03d3 = function(u)
  ::L_03d3::
  m688.dopo(u, 40, "L_03d9")
  do return end
end

script.L_03d9 = function(u)
  ::L_03d9::
  m688.rotta(u, nil)
  m688.rateo_virata(u, -7.2)
  m688.comportamento(u, "L_03c9")
  m688.salva_modello(u)
  do return end
end

script.L_03e4 = function(u)
  ::L_0338::
  do return end
  ::L_03aa::
  if m688.variabile_uguale(u, 5, 1) then goto L_0338 end
  m688.variabile_piu(u, 5)
  m688.marca_contatti(u)
  script.L_05fc(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03c1")
  do return end
  ::L_03e4::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_03f0 end
  ::L_03e8::
  m688.ingaggia(u, 40, "L_03e4", "L_03d3")
  m688.salva_modello(u)
  do return end
  ::L_03f0::
  if m688.bersaglio_vicino(u, 3.2) then goto L_03f8 end
  goto L_03e8
  ::L_03f8::
  if m688.variabile_uguale(u, 5, 1) then goto L_03e8 end
  m688.vai_a(u, -47411.2, 99563.52, 30, "L_06ab")
  m688.salva_modello(u)
  goto L_03aa
end

script.L_040d = function(u)
  ::L_040d::
  m688.variabile(u, 3, 0)
  m688.togli_unita(u)
  do return end
end

script.L_056d = function(u)
  ::L_056d::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_0615(u)
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
  m688.dopo(u, 20, "L_059b")
  m688.dopo(u, 10, "L_06a4")
  m688.dopo(u, 240, "L_06ab")
  do return end
end

script.L_059b = function(u)
  ::L_059b::
  m688.collega(u, 0)
  goto L_05a1
  ::L_05a1::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_05ad")
  do return end
end

script.L_05ad = function(u)
  ::L_05ad::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_05a1")
  do return end
end

script.L_05a1 = function(u)
  ::L_05a1::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_05ad")
  do return end
end

script.L_05fc = function(u)
  ::L_05fc::
  script.L_0615(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_0612")
  m688.dopo(u, 190, "L_0649")
  m688.dopo(u, 280, "L_06ab")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0612 = function(u)
  ::L_0612::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0615 = function(u)
  ::L_0615::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_06ab", "L_0683")
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

script.L_0649 = function(u)
  ::L_0649::
  m688.annulla_timer(u, "L_0649")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0683 = function(u)
  ::L_0683::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0654")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06a4")
  m688.dopo(u, 10, "L_0697")
  do return end
end

script.L_0654 = function(u)
  ::L_0654::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_05a1")
  m688.annulla_timer(u, "L_05ad")
  if m688.traccia_solida(u) then goto L_0667 end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 40, "L_0654", "L_0683")
  m688.salva_modello(u)
  do return end
  ::L_0667::
  if m688.bersaglio_vicino(u, 10) then goto L_0671 end
  m688.comportamento(u, "L_0654")
  m688.salva_modello(u)
  do return end
  ::L_0671::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0697")
  m688.annulla_timer(u, "L_0683")
  m688.insegui(u, 0, "L_06ab", "L_0683")
  m688.salva_modello(u)
  do return end
end

script.L_0697 = function(u)
  ::L_0697::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_05a1")
  do return end
end

script.L_06a4 = function(u)
  ::L_06a4::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_06a4")
  do return end
end

script.L_06ab = function(u)
  ::L_06ab::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_06af = function(u)
  ::L_06af::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_06ab", "L_0683")
  m688.modello_tipo(u, 20)
  m688.modello_rumore(u, 0)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 20)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 2, "L_06dc")
  m688.dopo(u, 160, "L_06e1")
  do return end
end

script.L_06dc = function(u)
  ::L_06dc::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_06e1 = function(u)
  ::L_06e1::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_06e5 = function(u)
  ::L_06e5::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_06ea = function(u)
  ::L_06ea::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0683")
  do return end
end

script.L_06f4 = function(u, p)
  ::L_06f4::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_0703")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0703 = function(u)
  ::L_0703::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0654")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0711 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0711::
  do return end
end

script.L_0712 = function(u, p)
  ::L_0712::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_071d")
  m688.salva_modello(u)
  do return end
end

script.L_071d = function(u)
  ::L_071d::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0724 = function(u, p)
  ::L_0724::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0729 = function(u, p)
  ::L_0729::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0730 = function(u, p)
  ::L_0730::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0738 = function(u)
  ::L_0738::
  m688.annulla_timer(u, "L_0777")
  m688.dopo(u, 20, "L_0777")
  do return end
end

script.L_0741 = function(u)
  ::L_0741::
  m688.annulla_timer(u, "L_077a")
  m688.dopo(u, 20, "L_077a")
  do return end
end

script.L_074a = function(u)
  ::L_074a::
  m688.annulla_timer(u, "L_077d")
  m688.dopo(u, 20, "L_077d")
  do return end
end

script.L_0753 = function(u)
  ::L_0753::
  m688.annulla_timer(u, "L_0780")
  m688.dopo(u, 20, "L_0780")
  do return end
end

script.L_075c = function(u)
  ::L_075c::
  m688.annulla_timer(u, "L_0783")
  m688.dopo(u, 20, "L_0783")
  do return end
end

script.L_0765 = function(u)
  ::L_0765::
  m688.annulla_timer(u, "L_0786")
  m688.dopo(u, 20, "L_0786")
  do return end
end

script.L_076e = function(u)
  ::L_076e::
  m688.annulla_timer(u, "L_0789")
  m688.dopo(u, 20, "L_0789")
  do return end
end

script.L_0777 = function(u)
  ::L_0777::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_077a = function(u)
  ::L_077a::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_077d = function(u)
  ::L_077d::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0780 = function(u)
  ::L_0780::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0783 = function(u)
  ::L_0783::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0786 = function(u)
  ::L_0786::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0789 = function(u)
  ::L_0789::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_078c = function(u)
  ::L_078c::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0794 = function(u, p)
  ::L_0794::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0798 = function(u)
  ::L_0798::
  m688.dopo(u, 60, "L_07a2")
  do return end
end

script.L_079e = function(u, p)
  ::L_079e::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07a2 = function(u)
  ::L_07a2::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_07a8 = function(u)
  ::L_07a8::
  m688.dopo(u, 40, "L_07ae")
  do return end
end

script.L_07ae = function(u)
  ::L_07ae::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_07b4 = function(u)
  ::L_07b4::
  m688.annulla_timer(u, "L_07a2")
  m688.annulla_timer(u, "L_07ae")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_07be = function(u, p)
  ::L_07be::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07c3 = function(u, p)
  ::L_07c3::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

script.L_07ca = function(u)
  ::L_07ca::
  if m688.giocatore_secondo(u) then goto L_07d0 end
  m688.messaggio(u, 25)
  do return end
  ::L_07d0::
  m688.messaggio(u, 29)
  do return end
end

script.L_07d3 = function(u)
  ::L_07d3::
  if m688.giocatore_secondo(u) then goto L_07d9 end
  m688.messaggio(u, 29)
  do return end
  ::L_07d9::
  m688.messaggio(u, 25)
  do return end
end

return missione
