-- SCENE9B: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE9B",
  missione = 9,
  titolo = { it = "Ritorno a casa", en = "HOMECOMING" },
  lato = "urss",
  teatro = "celtica",
  posa = { metodo = "scala_di_gioco", centro = { lat = 48.2500, lon = -9.7500 }, metri_per_unita = { x = 37.040, y = 37.040 }, moto_classico_m_s_per_nodo = 1.7803, affidabilita = "bassa", nota = "collocazione della carta poco affidabile: scala di gioco (1 nm = 50 unita) centrata sul centro del teatro; da ricollocare sui briefing" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 243.84, 365.76 } },
  dotazione = { type53 = 18, esca = 4 },
  tubi = { "type53", "type53", "type53", "type53", "type53", "esca", "sea_lance" },
  armi = {
    type53 = { classe = "type53", velocita_kn = 50.2857, corsa_s = 240, filo_s = 170, danno = 100, rumore = 18, testa = 15, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_074b" },
    esca = { gestore = "L_0854" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_08b7",
    telegrafo = "L_08c9",
    quota = "L_08ce",
    timone = "L_08d5",
    pilota_automatico_off = "L_0931",
    periscopio = "L_0939",
    carica_tubo_1 = "L_08dd",
    carica_tubo_2 = "L_08e6",
    carica_tubo_3 = "L_08ef",
    carica_tubo_4 = "L_08f8",
    carica_tubo_5 = "L_0901",
    carica_tubo_6 = "L_090a",
    carica_tubo_7 = "L_0913",
    schiera_cala = "L_093d",
    schiera_recupera = "L_094d",
    schiera_persa = "L_0959",
    designa_bersaglio = "L_0963",
    posizione = "L_0968",
    antenna = "L_0943",
    guida_e0 = "L_088a",
    guida_e1 = "L_088f",
    guida_e2 = "L_0899",
  },
  copertura = 0.686084270477,
  varianti = {
    { probabilita = 0.0216969008697, probabilita_originale = "15609/1048576", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0213605148097, probabilita_originale = "15367/1048576", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0212037894863, probabilita_originale = "61017/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0212037894863, probabilita_originale = "61017/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0212037894863, probabilita_originale = "61017/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0212037894863, probabilita_originale = "61017/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.020875048564, probabilita_originale = "60071/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.020875048564, probabilita_originale = "60071/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.020875048564, probabilita_originale = "60071/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.020875048564, probabilita_originale = "60071/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207218851798, probabilita_originale = "238521/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207218851798, probabilita_originale = "238521/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207218851798, probabilita_originale = "238521/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207218851798, probabilita_originale = "238521/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207106781029, probabilita_originale = "29799/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207106781029, probabilita_originale = "29799/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207106781029, probabilita_originale = "29799/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0207106781029, probabilita_originale = "29799/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0204006156421, probabilita_originale = "234823/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0204006156421, probabilita_originale = "234823/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0204006156421, probabilita_originale = "234823/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0204006156421, probabilita_originale = "234823/16777216", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0203895823183, probabilita_originale = "29337/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0203895823183, probabilita_originale = "29337/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0203895823183, probabilita_originale = "29337/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0203895823183, probabilita_originale = "29337/2097152", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -30817.28, y = -30817.28 }, originale = { x = 4288, y = 2368 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -47411.2 }, originale = { x = 4736, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0202399808733, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 27261.44, y = -56893.44 }, originale = { x = 5856, y = 1664 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0199261827202, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 5926.4, y = -47411.2 }, originale = { x = 5280, y = 1920 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0197692836436, probabilita_originale = "56889/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0197692836436, probabilita_originale = "56889/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0197692836436, probabilita_originale = "56889/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.019462783122, probabilita_originale = "56007/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35558.4, y = -56893.44 }, originale = { x = 4160, y = 1664 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.019462783122, probabilita_originale = "56007/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.019462783122, probabilita_originale = "56007/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = -28446.72, y = -84154.88 }, originale = { x = 4352, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 28446.72, y = -32002.56 }, originale = { x = 5888, y = 2336 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 11852.8, y = -62819.84 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0103553390514, probabilita_originale = "29799/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = -14223.36, y = -65190.4 }, originale = { x = 4736, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.0103553390514, probabilita_originale = "29799/4194304", unita = {
      { id = 0, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1, giocatore = true,
        posizione = { x = 22520.32, y = -84154.88 }, originale = { x = 5728, y = 928 },
        quota_m = 152.4, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 9, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 5926.4, y = -60449.28 }, originale = { x = 5280, y = 1568 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -10667.52, y = -55708.16 }, originale = { x = 4832, y = 1696 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -55708.16 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -1185.28, y = -50967.04 }, originale = { x = 5088, y = 1824 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -50967.04 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = 1185.28, y = -60449.28 }, originale = { x = 5152, y = 1568 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -8296.96, y = -61634.56 }, originale = { x = 4896, y = 1536 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -61634.56 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 1, attributi = { "non_combattente" },
        posizione = { x = -5926.4, y = -52152.32 }, originale = { x = 4960, y = 1792 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -52152.32 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_02a8" } },
      { id = 7, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -4741.12, y = -66375.68 }, originale = { x = 4992, y = 1408 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -66375.68 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 8, classe = "perry", tipo_originale = 13, nome_originale = "OLIVER HAZARD PERRY CLASS FRIGATE", categoria = "nave", schieramento = "usa", flag = 10, tipo = 1,
        posizione = { x = -7111.68, y = -47411.2 }, originale = { x = 4928, y = 1920 },
        quota_m = 0, rotta = 70, velocita_kn = 13.7143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 75, rumore = 15, sonar = 0, difesa = 128, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 88896, y = -47411.2 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_059c", reazione = nil, arrivo = "L_02a8" } },
      { id = 9, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1,
        posizione = { x = 4741.12, y = -65190.4 }, originale = { x = 5248, y = 1440 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 200, rumore = 6, sonar = 22, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = "L_02b1", reazione = nil, arrivo = nil } },
      { id = 10, classe = "oscar", tipo_originale = 28, nome_originale = "OSCAR CLASS (SSGN) CRUISE MISSILE SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3555.84, y = -27261.44 }, originale = { x = 5024, y = 2464 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 14, sonar = 18, difesa = 128, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 5926.4, y = -60449.28 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_05db", reazione = nil, arrivo = "L_0464" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0206" }, { dopo_s = 100, unita = 0, gestore = "L_061d" }, { dopo_s = 30, unita = 1, gestore = "L_0214" }, { dopo_s = 50, unita = 7, gestore = "L_058e" }, { dopo_s = 30, unita = 8, gestore = "L_058e" }, { dopo_s = 120, unita = 9, gestore = "L_058e" }, { dopo_s = 160, unita = 9, gestore = "L_0436" }, { dopo_s = 120, unita = 9, gestore = "L_0423" }, { dopo_s = 10, unita = 10, gestore = "L_058e" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [5] = 0, [6] = 0 },
    },
  },
  briefing = {
    it = { "Compagni. Gli americani si avvicinano come tante anatre sull'acqua. Presto saranno abbastanza vicini perché i cacciatori entrino in azione! Il Leninskii Komsomol attenderà mentre il Pskovskii Komsomolets distrugge il convoglio con una salva di missili SS-NX-21. Una volta che i missili avranno colpito, il Leninskii Komsomol si avvicinerà e distruggerà tutto ciò che sarà rimasto a galla. Compagni, attendiamo con impazienza la notizia della vostra vittoria!" },
    en = { "COMRADES. THE AMERICANS APPROACH LIKE SO MANY DUCKS ON THE WATER. THEY WILL SOON BE CLOSE ENOUGH FOR THE HUNTERS TO MOVE IN! THE LENINSKII KOMSOMOL WILL WAIT AS THE PSKOVSKII KOMSOMOLETS CRUSHES THE CONVOY WITH A BARRAGE OF SS-NX-21 MISSILES. ONCE THE MISSILES HAVE IMPACTED, THE LENINSKII KOMSOMOL WILL MOVE IN AND DESTROY ANYTHING LEFT FLOATING IN THE WATER. COMRADES, WE EAGERLY AWAIT NEWS OF YOUR VICTORY!" },
  },
  esiti = {
    [1] = { it = "CODICE: 9-1-BKB\nIl Leninskii Komsomol è andato perduto in mare mentre attaccava coraggiosamente un convoglio di navi nemiche. Gli eroici uomini del Leninskii Komsomol, e gli ideali di progresso e socialismo per cui sono morti, non saranno mai dimenticati! Salutiamo il Leninskii Komsomol!", en = "CODE: 9-1-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE COURAGEOUSLY ATTACKING A CONVOY OF ENEMY SHIPS. THE HEROIC MEN OF THE LENINSKII KOMSOMOL AND THE IDEALS OF PROGRESS AND SOCIALISM FOR WHICH THEY DIED SHALL NEVER BE FORGOTTEN! WE SALUTE THE LENINSKII KOMSOMOL!" },
    [2] = { it = "CODICE: 9-2-BKB\nIl Leninskii Komsomol è andato perduto in mare mentre attaccava coraggiosamente un convoglio di navi nemiche. Gli eroici uomini del Leninskii Komsomol, e gli ideali di progresso e socialismo per cui sono morti, non saranno mai dimenticati! Salutiamo il Leninskii Komsomol!", en = "CODE: 9-2-BKB\nTHE LENINSKII KOMSOMOL WAS LOST AT SEA WHILE COURAGEOUSLY ATTACKING A CONVOY OF ENEMY SHIPS. THE HEROIC MEN OF THE LENINSKII KOMSOMOL AND THE IDEALS OF PROGRESS AND SOCIALISM FOR WHICH THEY DIED SHALL NEVER BE FORGOTTEN! WE SALUTE THE LENINSKII KOMSOMOL!" },
    [5] = { it = "CODICE: 9-5-BKB\nCompagni, la Stavka si congratula con l'equipaggio del Leninskii Komsomol per il successo dell'attacco! L'oceano ha inghiottito il convoglio come se non fosse mai esistito! L'Occidente decadente imparerà a non scherzare con la potente Marina sovietica! Salutiamo il valore dell'equipaggio del Leninskii Komsomol! Per tanta dedizione al dovere, a questo Kapitan sarà conferito l'ambito titolo di Eroe dell'Unione Sovietica.", en = "CODE: 9-5-BKB\nCOMRADES, THE STAVKA CONGRATULATES THE CREW OF THE LENINSKII KOMSOMOL ON THEIR SUCCESSFUL ATTACK! THE OCEAN HAS SWALLOWED THE CONVOY AS IF SHE NEVER EXISTED! THE DECADENT WEST SHALL LEARN NOT TO TRIFLE WITH THE MIGHTY SOVIET NAVY! WE SALUTE THE VALOR OF THE CREW OF THE LENINSKII KOMSOMOL! FOR SUCH DEDICATION TO DUTY, THIS KAPITAN SHALL RECEIVE THE COVETED AWARD OF HERO OF THE U.S.S.R." },
    [6] = { it = "CODICE: 9-6-BKB\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di localizzare così tanti bersagli rumorosi e lenti! Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 9-6-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF SO MANY LOUD, SLOW TARGETS!. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
    [7] = { it = "CODICE: 9-7-BKB\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di determinare la posizione del proprio battello. Se leggesse le carte nautiche con la stessa attenzione con cui legge le etichette della sua vodka preferita, avremmo risultati accettabili. Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 9-7-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF HIS OWN VESSEL. IF HE WOULD READ HIS CHARTS AS CAREFULLY AS HE READS THE LABELS OF HIS FAVORITE VODKA, WE WOULD HAVE ACCEPTABLE RESULTS. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
    [19] = { it = "CODICE: 9-19-BKB\nL'equipaggio del Leninskii Komsomol ha commesso un errore gravissimo! Questo Kapitan non è stato nemmeno capace di determinare la posizione del proprio battello. Se leggesse le carte nautiche con la stessa attenzione con cui legge le etichette della sua vodka preferita, avremmo risultati accettabili. Con un tale eccesso di stupidità a bordo del Leninskii Komsomol, dovrei rallegrarmi che non abbiate affondato nessuna delle nostre navi! Rientri immediatamente in porto e si prepari a un lungo, freddo inverno.", en = "CODE: 9-19-BKB\nTHE CREW OF THE LENINSKII KOMSOMOL HAS MADE A GRAVE MISTAKE! THIS KAPITAN COULD NOT EVEN DETERMINE THE POSITION OF HIS OWN VESSEL. IF HE WOULD READ HIS CHARTS AS CAREFULLY AS HE READS THE LABELS OF HIS FAVORITE VODKA, WE WOULD HAVE ACCEPTABLE RESULTS. WITH SUCH AN EXCESS OF STUPIDITY ABOARD THE LENINSKII KOMSOMOL, I SHOULD BE PLEASED THAT YOU DID NOT SINK ANY OF OUR SHIPS! RETURN TO PORT IMMEDIATELY AND MAKE PLANS FOR A LONG COLD WINTER." },
  },
  classi_mancanti = { "biologico", "type53" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_0206 = function(u)
  ::L_0206::
  if m688.manomesso(u) then goto L_020a end
  do return end
  ::L_020a::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

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
  m688.messaggio(u, 106)
  m688.salva_modello(u)
  do return end
end

script.L_02b1 = function(u)
  ::L_02b1::
  if m688.bersaglio_id(u, 10) then goto L_02bb end
  m688.comportamento(u, "L_02b1")
  m688.salva_modello(u)
  do return end
  ::L_02bb::
  m688.annulla_timer(u, "L_0329")
  m688.dopo(u, 400, "L_0329")
  m688.annulla_timer(u, "L_0436")
  m688.annulla_timer(u, "L_0423")
  m688.annulla_timer(u, "L_042e")
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 1.0547)
  m688.schiera_trainata(u, 1)
  m688.ingaggia(u, 15, "L_02db", "L_02f5")
  m688.salva_modello(u)
  do return end
end

script.L_02db = function(u)
  ::L_02db::
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_02e7 end
  m688.ingaggia(u, 15, "L_02db", "L_02f5")
  m688.salva_modello(u)
  do return end
  ::L_02e7::
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, 10) then goto L_0304 end
  m688.ingaggia(u, 15, "L_02db", "L_02f5")
  m688.salva_modello(u)
  do return end
  ::L_0304::
  if m688.variabile_uguale(u, 3, 4) then goto L_0329 end
  m688.ingaggia(u, 5, "L_02db", "L_02f5")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0329")
  m688.dopo(u, 200, "L_0329")
  m688.variabile_piu(u, 3)
  script.L_07a1(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 280, "L_03f2")
  do return end
  ::L_0329::
  m688.annulla_tutti_i_timer(u)
  m688.comportamento(u, "L_0342")
  m688.vai_a(u, 88896, -54522.88, 30, "L_0352")
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_058e")
  m688.dopo(u, 80, "L_0423")
  do return end
end

script.L_02f5 = function(u)
  ::L_02f5::
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.comportamento(u, "L_02b1")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_0329")
  m688.dopo(u, 400, "L_0329")
  do return end
end

script.L_0329 = function(u)
  ::L_0329::
  m688.annulla_tutti_i_timer(u)
  m688.comportamento(u, "L_0342")
  m688.vai_a(u, 88896, -54522.88, 30, "L_0352")
  m688.salva_modello(u)
  m688.dopo(u, 40, "L_058e")
  m688.dopo(u, 80, "L_0423")
  do return end
end

script.L_0342 = function(u)
  ::L_0342::
  if m688.bersaglio_id(u, 0) then goto L_0397 end
  if m688.bersaglio_id(u, 10) then goto L_0397 end
  if m688.bersaglio_arma(u) then goto L_0372 end
  if m688.bersaglio_nave(u) then goto L_0357 end
  ::L_0352::
  m688.comportamento(u, "L_0342")
  m688.salva_modello(u)
  do return end
  ::L_0357::
  if m688.variabile_uguale(u, 6, 1) then goto L_0362 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  ::L_0362::
  m688.comportamento(u, "L_0342")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_036f")
  m688.dopo(u, 300, "L_036f")
  do return end
  ::L_0372::
  if m688.bersaglio_mi_punta(u) then goto L_0378 end
  goto L_0352
  ::L_0378::
  if m688.flag_scena_uguale(u, 1) then goto L_0352 end
  m688.flag_scena(u, 1)
  m688.comportamento(u, "L_0342")
  m688.telegrafo(u, 0.5)
  m688.quota(u, 6.096, 0.9961)
  m688.salva_modello(u)
  m688.dopo(u, 140, "L_0393")
  goto L_0854
  ::L_0397::
  m688.variabile(u, 6, 1)
  m688.annulla_timer(u, "L_0423")
  m688.annulla_timer(u, "L_042e")
  m688.annulla_timer(u, "L_03da")
  m688.ferma_navigazione(u)
  m688.dopo(u, 200, "L_03da")
  m688.comportamento(u, "L_0342")
  m688.accosta_verso_bersaglio(u)
  m688.salva_modello(u)
  if m688.traccia_solida(u) then goto L_03b4 end
  do return end
  ::L_03b4::
  if m688.bersaglio_vicino(u, 9) then goto L_03bc end
  goto L_0352
  ::L_03bc::
  if m688.variabile_uguale(u, 3, 4) then goto L_0352 end
  if m688.bersaglio_vicino(u, 10) then goto L_03cb end
  goto L_0352
  ::L_03cb::
  m688.variabile_piu(u, 3)
  script.L_07a1(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03f2")
  do return end
  ::L_0854::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0850", "L_0828")
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
  m688.dopo(u, 2, "L_0881")
  m688.dopo(u, 160, "L_0886")
  do return end
end

script.L_0352 = function(u)
  ::L_0352::
  m688.comportamento(u, "L_0342")
  m688.salva_modello(u)
  do return end
end

script.L_036f = function(u)
  ::L_036f::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  do return end
end

script.L_0393 = function(u)
  ::L_0393::
  m688.flag_scena(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_03da = function(u)
  ::L_03da::
  m688.telegrafo(u, 1.0547)
  m688.vai_a(u, 106675.2, -66375.68, 30, "L_02ac")
  m688.variabile(u, 6, 0)
  m688.salva_modello(u)
  m688.dopo(u, 80, "L_0423")
  do return end
end

script.L_02ac = function(u)
  ::L_02ac::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_03f2 = function(u)
  ::L_03f2::
  m688.variabile_meno(u, 3)
  do return end
end

script.L_0423 = function(u)
  ::L_0423::
  m688.ferma_navigazione(u)
  m688.virata_relativa(u, -180)
  m688.salva_modello(u)
  m688.dopo(u, 30, "L_042e")
  do return end
end

script.L_042e = function(u)
  ::L_042e::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_0423")
  do return end
end

script.L_0436 = function(u)
  ::L_0436::
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 40, "L_0436")
  do return end
end

script.L_0464 = function(u)
  ::L_0464::
  m688.annulla_tutti_i_timer(u)
  m688.telegrafo(u, 0.0195)
  m688.quota(u, 6.096, 1)
  m688.rotta(u, nil)
  m688.rateo_virata(u, -0.72)
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  m688.dopo(u, 50, "L_0490")
  m688.dopo(u, 60, "L_0482")
  do return end
end

script.L_0482 = function(u)
  ::L_0482::
  m688.vai_a(u, 88896, 58078.72, 30, "L_02ac")
  m688.telegrafo(u, 0.625)
  m688.salva_modello(u)
  do return end
end

script.L_0490 = function(u)
  ::L_0490::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_058e")
  do return end
end

script.L_0499 = function(u)
  ::L_0499::
  if m688.traccia_solida(u) then goto L_04a1 end
  ::L_049c::
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  do return end
  ::L_04a1::
  if m688.bersaglio_nave(u) then goto L_04af end
  if m688.bersaglio_arma(u) then goto L_04ee end
  if m688.bersaglio_id(u, 9) then goto L_04af end
  goto L_049c
  ::L_04af::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 5, 1) then goto L_04cc end
  m688.marca_contatti(u)
  m688.variabile_piu(u, 5)
  script.L_0762(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_0559")
  do return end
  ::L_04cc::
  m688.annulla_timer(u, "L_04de")
  m688.annulla_timer(u, "L_04ea")
  m688.quota(u, 365.76, 0.4688)
  m688.salva_modello(u)
  m688.dopo(u, 160, "L_04de")
  do return end
  ::L_04ee::
  if m688.bersaglio_mi_punta(u) then goto L_04f4 end
  goto L_049c
  ::L_04f4::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 152.4, 0.9961)
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_0507")
  do return end
end

script.L_04de = function(u)
  ::L_04de::
  m688.quota(u, 6.096, 1)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_04ea")
  do return end
end

script.L_04ea = function(u)
  ::L_04ea::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0507 = function(u)
  ::L_0507::
  m688.quota(u, 6.096, 1)
  m688.telegrafo(u, 0.0781)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0517")
  do return end
end

script.L_0517 = function(u)
  ::L_0517::
  m688.periscopio(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0559 = function(u)
  ::L_0559::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_058e = function(u)
  ::L_058e::
  m688.marca_contatti(u)
  m688.dopo(u, 28, "L_058e")
  do return end
end

script.L_059c = function(u)
  ::L_059c::
  if m688.bersaglio_id(u, 0) then goto L_05ab end
  if m688.bersaglio_id(u, 10) then goto L_05ab end
  m688.comportamento(u, "L_059c")
  m688.salva_modello(u)
  do return end
  ::L_05ab::
  m688.annulla_timer(u, "L_05d5")
  m688.dopo(u, 60, "L_05d5")
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.traccia_solida(u) then goto L_05c1 end
  m688.impulso_sonar(u)
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_059c")
  m688.salva_modello(u)
  do return end
  ::L_05c1::
  m688.telegrafo(u, 1.0547)
  m688.comportamento(u, "L_059c")
  m688.salva_modello(u)
  if m688.bersaglio_vicino(u, 5) then goto L_05ce end
  do return end
  ::L_05ce::
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 9)
  do return end
end

script.L_05d5 = function(u)
  ::L_05d5::
  m688.telegrafo(u, 0.5)
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  do return end
end

script.L_05db = function(u)
  ::L_05db::
  if m688.traccia_solida(u) then goto L_05e3 end
  ::L_05de::
  m688.comportamento(u, "L_05db")
  m688.salva_modello(u)
  do return end
  ::L_05e3::
  if m688.bersaglio_nave(u) then goto L_05e9 end
  goto L_05de
  ::L_05e9::
  if m688.bersaglio_vicino(u, 20) then goto L_05f1 end
  goto L_05de
  ::L_05f1::
  if m688.bersaglio_vicino(u, 4) then m688.dopo(u, 0.05, "L_05f1") do return end end  -- attesa: si riprova al passo successivo
  m688.flag_scena_piu(u)
  if m688.flag_scena_uguale(u, 8) then goto L_0609 end
  m688.collega(u, 1)
  m688.salva_modello(u)
  goto L_06f2
  ::L_0609::
  m688.telegrafo(u, 1.0547)
  m688.modello_tipo(u, 20)
  m688.quota(u, 365.76, 0.7812)
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_058e")
  do return end
  ::L_06f2::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_06fc")
  do return end
end

script.L_05f1 = function(u)
  ::L_05f1::
  if m688.bersaglio_vicino(u, 4) then m688.dopo(u, 0.05, "L_05f1") do return end end  -- attesa: si riprova al passo successivo
  m688.flag_scena_piu(u)
  if m688.flag_scena_uguale(u, 8) then goto L_0609 end
  m688.collega(u, 1)
  m688.salva_modello(u)
  goto L_06f2
  ::L_0609::
  m688.telegrafo(u, 1.0547)
  m688.modello_tipo(u, 20)
  m688.quota(u, 365.76, 0.7812)
  m688.comportamento(u, "L_0499")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_058e")
  do return end
  ::L_06f2::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 10, "L_06fc")
  do return end
end

script.L_061d = function(u)
  ::L_061d::
  if m688.unita_esiste(u, 2) then goto L_0643 end
  if m688.unita_esiste(u, 3) then goto L_0643 end
  if m688.unita_esiste(u, 4) then goto L_0643 end
  if m688.unita_esiste(u, 5) then goto L_0643 end
  if m688.unita_esiste(u, 6) then goto L_0643 end
  if m688.unita_esiste(u, 7) then goto L_0643 end
  if m688.unita_esiste(u, 8) then goto L_0643 end
  m688.messaggio(u, 105)
  do return end
  ::L_0643::
  if m688.in_rettangolo(u, -165939.2, 93637.12, 142233.6, -101934.08) then goto L_0651 end
  m688.messaggio(u, 107)
  do return end
  ::L_0651::
  m688.dopo(u, 20, "L_061d")
  do return end
end

script.L_06fc = function(u)
  ::L_06fc::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_074b = function(u)
  ::L_074b::
  m688.rumore_transitorio(u)
  script.L_0771(u)
  m688.nuova_unita(u)
  m688.dopo(u, 150, "L_07b7")
  m688.dopo(u, 170, "L_07ee")
  m688.dopo(u, 240, "L_0850")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0762 = function(u)
  ::L_0762::
  m688.rumore_transitorio(u)
  script.L_0771(u)
  m688.modello_classe(u, "tipo_2d")
  m688.modello_difesa(u, 200)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0850")
  do return end
end

script.L_0771 = function(u)
  ::L_0771::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0850", "L_0828")
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

script.L_07a1 = function(u)
  ::L_07a1::
  script.L_07ba(u)
  m688.nuova_unita(u)
  m688.dopo(u, 170, "L_07b7")
  m688.dopo(u, 190, "L_07ee")
  m688.dopo(u, 280, "L_0850")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_07b7 = function(u)
  ::L_07b7::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_07ba = function(u)
  ::L_07ba::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0850", "L_0828")
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

script.L_07ee = function(u)
  ::L_07ee::
  m688.annulla_timer(u, "L_07ee")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0828 = function(u)
  ::L_0828::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_07f9")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0849")
  m688.dopo(u, 10, "L_083c")
  do return end
end

script.L_07f9 = function(u)
  ::L_07f9::
  m688.azzera_rateo_quota(u)
  m688.annulla_timer(u, "L_0733")
  m688.annulla_timer(u, "L_073f")
  if m688.traccia_solida(u) then goto L_080c end
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 30, "L_07f9", "L_0828")
  m688.salva_modello(u)
  do return end
  ::L_080c::
  if m688.bersaglio_vicino(u, 10) then goto L_0816 end
  m688.comportamento(u, "L_07f9")
  m688.salva_modello(u)
  do return end
  ::L_0816::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_083c")
  m688.annulla_timer(u, "L_0828")
  m688.insegui(u, 0, "L_0850", "L_0828")
  m688.salva_modello(u)
  do return end
end

script.L_0733 = function(u)
  ::L_0733::
  m688.quota(u, 45.72, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_073f")
  do return end
end

script.L_073f = function(u)
  ::L_073f::
  m688.quota(u, 365.76, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_0733")
  do return end
end

script.L_083c = function(u)
  ::L_083c::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0733")
  do return end
end

script.L_0849 = function(u)
  ::L_0849::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0849")
  do return end
end

script.L_0850 = function(u)
  ::L_0850::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0854 = function(u)
  ::L_0854::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0850", "L_0828")
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
  m688.dopo(u, 2, "L_0881")
  m688.dopo(u, 160, "L_0886")
  do return end
end

script.L_0881 = function(u)
  ::L_0881::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_0886 = function(u)
  ::L_0886::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_088a = function(u)
  ::L_088a::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_088f = function(u)
  ::L_088f::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0828")
  do return end
end

script.L_0899 = function(u, p)
  ::L_0899::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 20, "L_08a8")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_08a8 = function(u)
  ::L_08a8::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_07f9")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_08b6 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_08b6::
  do return end
end

script.L_08b7 = function(u, p)
  ::L_08b7::
  m688.vai_a(u, par(p, 1, -189644.8), par(p, 2, 117342.72), 30, "L_08c2")
  m688.salva_modello(u)
  do return end
end

script.L_08c2 = function(u)
  ::L_08c2::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_08c9 = function(u, p)
  ::L_08c9::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08ce = function(u, p)
  ::L_08ce::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08d5 = function(u, p)
  ::L_08d5::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_08dd = function(u)
  ::L_08dd::
  m688.annulla_timer(u, "L_091c")
  m688.dopo(u, 20, "L_091c")
  do return end
end

script.L_08e6 = function(u)
  ::L_08e6::
  m688.annulla_timer(u, "L_091f")
  m688.dopo(u, 20, "L_091f")
  do return end
end

script.L_08ef = function(u)
  ::L_08ef::
  m688.annulla_timer(u, "L_0922")
  m688.dopo(u, 20, "L_0922")
  do return end
end

script.L_08f8 = function(u)
  ::L_08f8::
  m688.annulla_timer(u, "L_0925")
  m688.dopo(u, 20, "L_0925")
  do return end
end

script.L_0901 = function(u)
  ::L_0901::
  m688.annulla_timer(u, "L_0928")
  m688.dopo(u, 20, "L_0928")
  do return end
end

script.L_090a = function(u)
  ::L_090a::
  m688.annulla_timer(u, "L_092b")
  m688.dopo(u, 20, "L_092b")
  do return end
end

script.L_0913 = function(u)
  ::L_0913::
  m688.annulla_timer(u, "L_092e")
  m688.dopo(u, 20, "L_092e")
  do return end
end

script.L_091c = function(u)
  ::L_091c::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_091f = function(u)
  ::L_091f::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0922 = function(u)
  ::L_0922::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0925 = function(u)
  ::L_0925::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0928 = function(u)
  ::L_0928::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_092b = function(u)
  ::L_092b::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_092e = function(u)
  ::L_092e::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0931 = function(u)
  ::L_0931::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0939 = function(u, p)
  ::L_0939::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_093d = function(u)
  ::L_093d::
  m688.dopo(u, 60, "L_0947")
  do return end
end

script.L_0943 = function(u, p)
  ::L_0943::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0947 = function(u)
  ::L_0947::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_094d = function(u)
  ::L_094d::
  m688.dopo(u, 40, "L_0953")
  do return end
end

script.L_0953 = function(u)
  ::L_0953::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0959 = function(u)
  ::L_0959::
  m688.annulla_timer(u, "L_0947")
  m688.annulla_timer(u, "L_0953")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0963 = function(u, p)
  ::L_0963::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0968 = function(u, p)
  ::L_0968::
  m688.posizione(u, par(p, 1, -189644.8), par(p, 2, 117342.72))
  m688.salva_modello(u)
  do return end
end

return missione
