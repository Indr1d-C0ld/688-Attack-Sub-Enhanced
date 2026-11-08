-- SCENE5A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE5A",
  missione = 5,
  titolo = { it = "Fuga", en = "ESCAPE" },
  lato = "usa",
  teatro = "gibilterra",
  posa = { metodo = "georef", centro = { lat = 35.7655, lon = -5.8445 }, metri_per_unita = { x = 22.969, y = 27.780 }, moto_classico_m_s_per_nodo = 1.2196, affidabilita = "alta", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 60.96, 121.92, 182.88 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_058e" },
    harpoon = { gestore = "L_0534" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_062c" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_068f",
    telegrafo = "L_06a1",
    quota = "L_06a6",
    timone = "L_06ad",
    pilota_automatico_off = "L_0709",
    periscopio = "L_0711",
    carica_tubo_1 = "L_06b5",
    carica_tubo_2 = "L_06be",
    carica_tubo_3 = "L_06c7",
    carica_tubo_4 = "L_06d0",
    carica_tubo_5 = "L_06d9",
    carica_tubo_6 = "L_06e2",
    carica_tubo_7 = "L_06eb",
    schiera_cala = "L_0715",
    schiera_recupera = "L_0725",
    schiera_persa = "L_0731",
    designa_bersaglio = "L_073b",
    posizione = "L_0740",
    antenna = "L_071b",
    guida_e0 = "L_0662",
    guida_e1 = "L_0667",
    guida_e2 = "L_0671",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.0568678379059, probabilita_originale = "238521/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0562065839767, probabilita_originale = "471495/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0562065839767, probabilita_originale = "471495/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0559861660004, probabilita_originale = "234823/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0555530190468, probabilita_originale = "932025/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0553351640701, probabilita_originale = "464185/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0553351640701, probabilita_originale = "464185/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0546917319298, probabilita_originale = "917575/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0284339189529, probabilita_originale = "238521/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0284339189529, probabilita_originale = "238521/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0281032919884, probabilita_originale = "471495/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0281032919884, probabilita_originale = "471495/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0279930830002, probabilita_originale = "234823/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0279930830002, probabilita_originale = "234823/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0277726650238, probabilita_originale = "116487/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0277726650238, probabilita_originale = "116487/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0276675820351, probabilita_originale = "464185/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0276675820351, probabilita_originale = "464185/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0274497270584, probabilita_originale = "230265/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0274497270584, probabilita_originale = "230265/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0273420810699, probabilita_originale = "114681/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0273420810699, probabilita_originale = "114681/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0270241498947, probabilita_originale = "226695/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 52.75, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0270241498947, probabilita_originale = "226695/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0142169594765, probabilita_originale = "238521/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0139965415001, probabilita_originale = "234823/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0138863325119, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0138863325119, probabilita_originale = "116487/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.013671040535, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -32531.8798, y = 10765.8939 }, originale = { x = 5056, y = 4128 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_048d" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -25949.2747, y = 1022.3651 }, originale = { x = 5344, y = 3776 },
        quota_m = 30.48, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -13452.956, y = -2540.6464 }, originale = { x = 5888, y = 3648 },
        quota_m = 30.48, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -3887.8074, y = 101.273 }, originale = { x = 6304, y = 3744 },
        quota_m = 30.48, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.013671040535, probabilita_originale = "114681/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -6091.7746, y = 3637.6258 }, originale = { x = 6208, y = 3872 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0414" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -16382.5773, y = 4534.5255 }, originale = { x = 5760, y = 3904 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0440" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -24468.673, y = 4553.2648 }, originale = { x = 5408, y = 3904 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_044b" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0135633945465, probabilita_originale = "56889/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2728.4114, y = 9822.422 }, originale = { x = 6592, y = 4096 },
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -32528.6112, y = 11649.6802 }, originale = { x = 5056, y = 4160 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
    { probabilita = 0.0133531093597, probabilita_originale = "56007/4194304", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 2400, y = 2200 }, originale = { x = 6592, y = 3808 }, spostamento_m = 468.4496,
        quota_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -28925.5624, y = -9573.6844 }, originale = { x = 5216, y = 3392 },
        quota_m = 60.96, rotta = 33, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 524.8556, y = 2751.7586 }, originale = { x = 6496, y = 3840 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -8295.6355, y = 5406.9849 }, { x = 1259.7256, y = 5403.1703 }, { x = 1260.231, y = 1868.0563 }, { x = -8298.9638, y = 1871.8714 }, { x = -8295.6355, y = 5406.9849 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_03fe" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -14922.8421, y = -1654.5105 }, originale = { x = 5824, y = 3680 },
        quota_m = 182.88, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -14174.4313, y = 6298.2632 }, { x = -14191.4871, y = -4307.0317 }, { x = -17134.8364, y = -4301.8096 }, { x = -17114.2431, y = 6303.4831 }, { x = -14174.4313, y = 6298.2632 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0435" } },
      { id = 4, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -23019.2248, y = -3404.654 }, originale = { x = 5472, y = 3616 },
        quota_m = 30.48, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -22256.6752, y = 7198.7596 }, { x = -22290.1527, y = -6057.8412 }, { x = -25234.085, y = -6049.9199 }, { x = -25196.186, y = 7206.6767 }, { x = -22256.6752, y = 7198.7596 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0461" } },
      { id = 5, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -31845.1716, y = -2493.4452 }, originale = { x = 5088, y = 3648 },
        quota_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = -33990.7898, y = 13422.8074 }, { x = -31787.7139, y = 13414.5728 }, { x = -31118.8275, y = -5147.3689 }, { x = -34062.4559, y = -5136.5038 }, { x = -33990.7898, y = 13422.8074 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_02b8", reazione = nil, arrivo = "L_0482" } },
      { id = 6, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -20050.0276, y = 8077.2486 }, originale = { x = 5600, y = 4032 },
        quota_m = 60.96, rotta = 134, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 7, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -9031.569, y = 4523.9274 }, originale = { x = 6080, y = 3904 },
        quota_m = 60.96, rotta = 209, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 8, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 2728.9595, y = 8054.8509 }, originale = { x = 6592, y = 4032 },
        quota_m = 60.96, rotta = 320, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 9, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -34749.5749, y = 7239.181 }, originale = { x = 4960, y = 4000 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      { id = 10, classe = "mercantile", tipo_originale = 1, nome_originale = "MERCHANT SHIP", categoria = "nave", schieramento = "neutrale", flag = 66, tipo = 2, attributi = { "non_combattente" },
        posizione = { x = -27406.3172, y = 5445.6971 }, originale = { x = 5280, y = 3936 },
        quota_m = 0, rotta = 0, velocita_kn = 7.3143,
        modello = { massima_kn = 15.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 50, rumore = 20, sonar = 0, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -11240.2531, y = 1875.13 }, { x = 8608.275, y = 7174.8502 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_0247" } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_0252" }, { dopo_s = 60, unita = 0, gestore = "L_0260" }, { dopo_s = 30, unita = 2, gestore = "L_03c2" }, { dopo_s = 50, unita = 3, gestore = "L_03c2" }, { dopo_s = 70, unita = 4, gestore = "L_03c2" }, { dopo_s = 90, unita = 5, gestore = "L_03c2" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
    },
  },
  briefing = {
    it = { "Comandante, poco dopo la sua partenza dal porto in Italia, due sottomarini sovietici di ritorno da pattugliamenti di routine hanno cambiato improvvisamente rotta. Sembra che abbiano virato a sud e siano diretti verso lo Stretto di Gibilterra. Riteniamo che questi battelli possano aver ricevuto l'ordine di seguirla fuori dal Mediterraneo e nell'Atlantico. Mantenendo la rotta attuale, i sottomarini sovietici la intercetteranno all'uscita dello stretto.", "Comandante, conto su di lei per superare i sovietici e sganciarsi verso ovest senza fare il minimo rumore. So che il suo equipaggio è stanco dopo il turno nel Mediterraneo, ma ultimamente ho avuto più orsi del dovuto a fiutare attorno ai miei battelli! Tenga duro: probabilmente potrà rilassarsi una volta in mare aperto. Credo che i ragazzi sovietici rinunceranno quando raggiungerà l'Atlantico, a meno che non le stiano ben attaccati in coda all'uscita. Non vedo la necessità di provare le armi, a meno che non siano loro a sparare per primi. Se ce la fa, la prossima settimana ne parleremo a cena al circolo ufficiali di Groton." },
    en = { "CAPTAIN, SHORTLY AFTER YOU LEFT PORT IN ITALY, TWO SOVIET SUBS RETURNING HOME FROM ROUTINE PATROLS MADE SUDDEN CHANGES IN COURSE. THEY APPEAR TO HAVE TURNED SOUTH AND ARE HEADING TOWARD THE STRAIT OF GIBRALTAR. WE BELIEVE THESE BOATS MAY HAVE BEEN INSTRUCTED TO FOLLOW THE YOU OUT OF THE MED AND INTO THE ATLANTIC. FOLLOWING THEIR PRESENT COURSE, THE SOVIET SUBS WILL INTERCEPT YOU AS YOU EXIT THE STRAIT.", "CAPTAIN, I'M COUNTING ON YOU TO GET PAST THE SOVIETS AND ESCAPE TO THE WEST WITHOUT SO MUCH AS A WHISPER. I KNOW YOUR CREW IS FATIGUED AFTER YOUR TOUR IN THE MED, BUT LATELY I'VE HAD MORE THAN MY SHARE OF BEARS SNIFFING AROUND MY BOATS! TOUGH IT OUT AND YOU CAN PROBABLY RELAX ONCE YOU'RE IN OPEN WATERS. I THINK THE SOVIET BOYS WILL BREAK OFF WHEN YOU REACH THE ATLANTIC, UNLESS THEY HAVE A CLEAN TAIL ON YOU AS YOU EXIT. I DON'T SEE ANY NEED FOR WEAPON TESTING UNLESS THEY DRAW FIRST. GET THROUGH THIS AND WE'LL DISCUSS YOUR ADVENTURES AT THE O CLUB IN GROTON NEXT WEEK OVER DINNER." },
  },
  esiti = {
    [1] = { it = "CODICE: 4-1-ACP\nIeri l'USS Los Angeles si è incagliato nei pressi dello Stretto di Gibilterra ed è andato distrutto. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 4-1-ACP\nTHE USS LOS ANGELES RAN AGROUND NEAR THE STRAIT OF GIBRALTAR AND WAS DESTROYED YESTERDAY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 4-2-ACP\nIeri l'USS Los Angeles è stato distrutto da siluri. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 4-2-ACP\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 4-3-ACP\nQuattro orsi al seguito: non ci posso credere.", en = "CODE: 4-3-ACP\nFOUR BEARS IN TOW, I DON'T BELIEVE IT." },
    [4] = { it = "CODICE: 4-4-ACP\nTre orsi al seguito: non ci posso credere.", en = "CODE: 4-4-ACP\nTHREE BEARS IN TOW, I DON'T BELIEVE IT." },
    [5] = { it = "CODICE: 4-5-ACP\nDue orsi al seguito: non ci posso credere.", en = "CODE: 4-5-ACP\nTWO BEARS IN TOW, I DON'T BELIEVE IT." },
    [6] = { it = "CODICE: 4-6-ACP\nUn orso al seguito: non ci posso credere.", en = "CODE: 4-6-ACP\nA BEAR IN TOW, I DON'T BELIEVE IT." },
    [7] = { it = "CODICE: 4-7-ACP\nComandante, mi congratulo per il lavoro ben fatto. Ha lasciato i nostri amici sovietici a cercarla freneticamente su e giù per la costa. Può scommettere che i loro alti papaveri andranno su tutte le furie quando scopriranno che se l'è cavata senza un graffio. Questi sono i risultati che mi piacciono: niente errori, niente sorprese. Continui così!", en = "CODE: 4-7-ACP\nCAPTAIN, LET ME CONGRATULATE YOU ON A JOB WELL DONE. YOU LEFT OUR SOVIET FRIENDS IN A FRANTIC SEARCH, UP AND DOWN THE BEACH. YOU CAN BET THEIR BRASS WILL BE IN AN UPROAR WHEN THEY FIND OUT THAT YOU GOT AWAY SCOTT FREE. THESE ARE THE KIND OF RESULTS I LIKE: NO MISTAKES, NO SURPRISES. KEEP IT UP!" },
    [8] = { it = "CODICE: 4-8-ACP\nVorrei sapere dove crede di andare. Non ho tempo da perdere mentre lei si diverte col mio battello. Superi lo stretto ed entri nell'Atlantico senza nessuno in coda, o darò al suo comandante in seconda l'occasione di comandare il suo sottomarino.", en = "CODE: 4-8-ACP\nI'D LIKE TO KNOW WERE YOU THINK YOU THINK YOU WHERE GOING. I DON'T HAVE TIME FOR YOU TO BE SCREWING AROUND WITH MY BOAT. GET THROUGH THE STRAIT AND INTO THE ATLANTIC WITHOUT A TAIL OR I'LL GIVE YOUR XO A SHOT AT COMMANDING YOUR SUB." },
    [9] = { it = "CODICE: 4-9-ACP\nAccidenti, quei ragazzi sovietici ci hanno davvero tenuti col fiato sospeso per un po'. Se ne è uscito vivo, è merito del suo eccellente addestramento. Al suo rientro voglio che faccia un resoconto allo stato maggiore dell'ammiraglio: sono tutti ansiosi di sentire i dettagli della sua avventura. Mi chiami quando arriva in porto.", en = "CODE: 4-9-ACP\nBOY, THOSE SOVIET BOYS REALLY HAD US GOING FOR A WHILE. IT'S A TESTIMONY TO YOUR EXCELLENT TRAINING THAT YOU CAME OUT OF THIS ONE ALIVE. I INTEND TO HAVE YOU BRIEF THE ADMIRAL'S STAFF WHEN YOU RETURN. THEY'RE ALL ANXIOUS TO HEAR THE DETAILS OF YOUR EPISODE. GIVE ME A CALL WHEN YOU GET INTO PORT." },
    [10] = { it = "CODICE: 4-10-ACP\nIl gioco è finito, Comandante. Ho seguito le sue azioni sulla rete SOSUS e abbiamo elementi per ritenere che lei abbia aperto il fuoco sui sottomarini sovietici senza provocazione. Come diavolo dovrei spiegarlo ai capi di stato maggiore? Comandante, i suoi giorni di navigazione sono finiti. Il comandante in seconda riporterà il battello in porto e mi consegnerà un rapporto completo. A meno che in qualche modo non si dimostri la sua innocenza, è nei guai fino al collo... d'acqua. Come promesso, la cena la offro io: in cella.", en = "CODE: 4-10-ACP\nTHE JIG IS UP CAPTAIN. I MONITORED YOUR ACTIONS ON THE SOSUS NET AND WE HAVE EVIDENCE TO SUGGEST THAT YOU FIRED ON THE SOVIET SUBS UNPROVOKED. HOW THE HELL AM I SUPPOSE TO EXPLAIN THIS TO THE CHIEFS OF STAFF? CAPTAIN, YOU'RE SAILING DAYS ARE OVER. THE XO WILL RETURN THE SHIP TO PORT AND HAVE A COMPLETE REPORT READY FOR ME. UNLESS SOMEHOW YOUR INNOCENCE CAN BE PROVEN, YOU'RE IN DEEP... WATER. AS PROMISED, DINNER WILL BE ON ME - IN THE BRIG." },
    [19] = { it = "CODICE: 4-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 4-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 16, punti = 0, spostamento_max_m = 468.4496, non_risolte = 0 },
  script = script,
}

script.L_0247 = function(u)
  ::L_0247::
  m688.vai_a(u, 8608.275, 7174.8502, 30, "L_04b4")
  m688.salva_modello(u)
  do return end
end

script.L_0252 = function(u)
  ::L_0252::
  if m688.manomesso(u) then goto L_0256 end
  do return end
  ::L_0256::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0260 = function(u)
  ::L_0260::
  if m688.in_rettangolo(u, -36887.3533, 23156.3906, 6422.8801, -19339.9937) then goto L_02b2 end
  if m688.x_maggiore(u, 6419.6778) then goto L_02af end
  if m688.variabile_uguale(u, 1, 0) then goto L_0284 end
  if m688.variabile_uguale(u, 3, 0) then goto L_0281 end
  m688.messaggio(u, 109)
  do return end
  ::L_0281::
  m688.messaggio(u, 110)
  do return end
  ::L_0284::
  if m688.variabile_uguale(u, 2, 4) then goto L_02a3 end
  if m688.variabile_uguale(u, 2, 3) then goto L_02a6 end
  if m688.variabile_uguale(u, 2, 2) then goto L_02a9 end
  if m688.variabile_uguale(u, 2, 1) then goto L_02ac end
  m688.messaggio(u, 107)
  do return end
  ::L_02a3::
  m688.messaggio(u, 103)
  do return end
  ::L_02a6::
  m688.messaggio(u, 104)
  do return end
  ::L_02a9::
  m688.messaggio(u, 105)
  do return end
  ::L_02ac::
  m688.messaggio(u, 106)
  do return end
  ::L_02af::
  m688.messaggio(u, 108)
  do return end
  ::L_02b2::
  m688.dopo(u, 20, "L_0260")
  do return end
end

script.L_02b8 = function(u)
  ::L_02b8::
  if m688.bersaglio_id(u, 0) then goto L_032b end
  if m688.bersaglio_arma(u) then goto L_02c5 end
  ::L_02c0::
  m688.comportamento(u, "L_02b8")
  m688.salva_modello(u)
  do return end
  ::L_02c5::
  if m688.bersaglio_mi_punta(u) then goto L_02cb end
  goto L_02c0
  ::L_02cb::
  if m688.traccia_solida(u) then goto L_02da end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_02cb", "L_02c0")
  m688.salva_modello(u)
  do return end
  ::L_02da::
  if m688.bersaglio_vicino(u, 3) then goto L_02fb end
  ::L_02df::
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_03c2")
  m688.dopo(u, 60, "L_02c0")
  m688.salva_modello(u)
  do return end
  ::L_02fb::
  if m688.flag_scena_bit(u, 16) then goto L_02df end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_030b")
  do return end
  ::L_032b::
  if m688.traccia_solida(u) then goto L_035a end
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 20, "L_032b", "L_0348")
  if m688.flag_scena_bit(u, 1) then goto L_0346 end
  m688.variabile_piu(u, 2)
  m688.flag_scena_accendi(u, 1)
  ::L_0346::
  m688.salva_modello(u)
  do return end
  ::L_035a::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 2)
  if m688.flag_scena_bit(u, 1) then goto L_036f end
  m688.flag_scena_accendi(u, 1)
  m688.variabile_piu(u, 2)
  ::L_036f::
  m688.ingaggia(u, 10, "L_032b", "L_0348")
  if m688.variabile_uguale(u, 1, 0) then goto L_037f end
  goto L_038d
  ::L_037f::
  if m688.variabile_uguale(u, 0, 0) then goto L_03b2 end
  m688.flag_scena_accendi(u, 128)
  m688.variabile(u, 0, 0)
  ::L_038d::
  if m688.bersaglio_vicino(u, 8) then goto L_0395 end
  goto L_03b2
  ::L_0395::
  if m688.variabile_uguale(u, 4, 4) then goto L_03b2 end
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  if m688.unita_id(u, 5) then goto L_03a6 end
  do return end
  ::L_03a6::
  script.L_0544(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03be")
  do return end
  ::L_03b2::
  if m688.bersaglio_vicino(u, 1.2) then goto L_03b9 end
  m688.salva_modello(u)
  do return end
  ::L_03b9::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_02c0 = function(u)
  ::L_02c0::
  m688.comportamento(u, "L_02b8")
  m688.salva_modello(u)
  do return end
end

script.L_02cb = function(u)
  ::L_02cb::
  if m688.traccia_solida(u) then goto L_02da end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_02cb", "L_02c0")
  m688.salva_modello(u)
  do return end
  ::L_02da::
  if m688.bersaglio_vicino(u, 3) then goto L_02fb end
  ::L_02df::
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_03c2")
  m688.dopo(u, 60, "L_02c0")
  m688.salva_modello(u)
  do return end
  ::L_02fb::
  if m688.flag_scena_bit(u, 16) then goto L_02df end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_030b")
  do return end
end

script.L_030b = function(u)
  ::L_030b::
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 182.88, 1)
  m688.salva_modello(u)
  m688.dopo(u, 80, "L_0326")
  m688.dopo(u, 120, "L_0322")
  goto L_062c
  ::L_062c::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0628", "L_0605")
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
  m688.dopo(u, 2, "L_0659")
  m688.dopo(u, 160, "L_065e")
  do return end
end

script.L_0322 = function(u)
  ::L_0322::
  m688.flag_scena_spegni(u, 16)
  m688.salva_modello(u)
  do return end
end

script.L_0326 = function(u)
  ::L_0326::
  m688.comportamento(u, "L_02b8")
  m688.salva_modello(u)
  do return end
end

script.L_032b = function(u)
  ::L_032b::
  if m688.traccia_solida(u) then goto L_035a end
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 20, "L_032b", "L_0348")
  if m688.flag_scena_bit(u, 1) then goto L_0346 end
  m688.variabile_piu(u, 2)
  m688.flag_scena_accendi(u, 1)
  ::L_0346::
  m688.salva_modello(u)
  do return end
  ::L_035a::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_03c2")
  m688.annulla_timer(u, "L_03dc")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.flag_scena_accendi(u, 2)
  if m688.flag_scena_bit(u, 1) then goto L_036f end
  m688.flag_scena_accendi(u, 1)
  m688.variabile_piu(u, 2)
  ::L_036f::
  m688.ingaggia(u, 10, "L_032b", "L_0348")
  if m688.variabile_uguale(u, 1, 0) then goto L_037f end
  goto L_038d
  ::L_037f::
  if m688.variabile_uguale(u, 0, 0) then goto L_03b2 end
  m688.flag_scena_accendi(u, 128)
  m688.variabile(u, 0, 0)
  ::L_038d::
  if m688.bersaglio_vicino(u, 8) then goto L_0395 end
  goto L_03b2
  ::L_0395::
  if m688.variabile_uguale(u, 4, 4) then goto L_03b2 end
  m688.salva_modello(u)
  m688.variabile_piu(u, 4)
  if m688.unita_id(u, 5) then goto L_03a6 end
  do return end
  ::L_03a6::
  script.L_0544(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03be")
  do return end
  ::L_03b2::
  if m688.bersaglio_vicino(u, 1.2) then goto L_03b9 end
  m688.salva_modello(u)
  do return end
  ::L_03b9::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_0348 = function(u)
  ::L_0348::
  m688.variabile_meno(u, 2)
  m688.flag_scena_spegni(u, 1)
  m688.telegrafo(u, 0.5)
  m688.comportamento(u, "L_02b8")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_03c2")
  do return end
end

script.L_03be = function(u)
  ::L_03be::
  m688.variabile_meno(u, 4)
  do return end
end

script.L_03c2 = function(u)
  ::L_03c2::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 0.0195)
  m688.virata_relativa(u, -180)
  if m688.flag_scena_bit(u, 2) then goto L_03d0 end
  goto L_03d5
  ::L_03d0::
  m688.quota(u, 274.32, 0.3906)
  ::L_03d5::
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_03dc")
  do return end
end

script.L_03dc = function(u)
  ::L_03dc::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.5)
  if m688.flag_scena_bit(u, 2) then goto L_03e7 end
  goto L_03ec
  ::L_03e7::
  m688.quota(u, 30.48, 0.5859)
  ::L_03ec::
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_03c2")
  do return end
end

script.L_03fe = function(u)
  ::L_03fe::
  m688.vai_a(u, 1259.7256, 5403.1703, 30, "L_0409")
  m688.salva_modello(u)
  do return end
end

script.L_0409 = function(u)
  ::L_0409::
  m688.vai_a(u, 1260.231, 1868.0563, 30, "L_0414")
  m688.salva_modello(u)
  do return end
end

script.L_0414 = function(u)
  ::L_0414::
  m688.vai_a(u, -8298.9638, 1871.8714, 30, "L_03f3")
  m688.salva_modello(u)
  do return end
end

script.L_03f3 = function(u)
  ::L_03f3::
  m688.vai_a(u, -8295.6355, 5406.9849, 30, "L_03fe")
  m688.salva_modello(u)
  do return end
end

script.L_0435 = function(u)
  ::L_0435::
  m688.vai_a(u, -14191.4871, -4307.0317, 30, "L_0440")
  m688.salva_modello(u)
  do return end
end

script.L_0440 = function(u)
  ::L_0440::
  m688.vai_a(u, -17134.8364, -4301.8096, 30, "L_041f")
  m688.salva_modello(u)
  do return end
end

script.L_041f = function(u)
  ::L_041f::
  m688.vai_a(u, -17114.2431, 6303.4831, 30, "L_042a")
  m688.salva_modello(u)
  do return end
end

script.L_042a = function(u)
  ::L_042a::
  m688.vai_a(u, -14174.4313, 6298.2632, 30, "L_0435")
  m688.salva_modello(u)
  do return end
end

script.L_044b = function(u)
  ::L_044b::
  m688.vai_a(u, -25196.186, 7206.6767, 30, "L_0456")
  m688.salva_modello(u)
  do return end
end

script.L_0456 = function(u)
  ::L_0456::
  m688.vai_a(u, -22256.6752, 7198.7596, 30, "L_0461")
  m688.salva_modello(u)
  do return end
end

script.L_0461 = function(u)
  ::L_0461::
  m688.vai_a(u, -22290.1527, -6057.8412, 30, "L_046c")
  m688.salva_modello(u)
  do return end
end

script.L_046c = function(u)
  ::L_046c::
  m688.vai_a(u, -25234.085, -6049.9199, 30, "L_044b")
  m688.salva_modello(u)
  do return end
end

script.L_0482 = function(u)
  ::L_0482::
  m688.vai_a(u, -31787.7139, 13414.5728, 30, "L_048d")
  m688.salva_modello(u)
  do return end
end

script.L_048d = function(u)
  ::L_048d::
  m688.vai_a(u, -31118.8275, -5147.3689, 30, "L_0498")
  m688.salva_modello(u)
  do return end
end

script.L_0498 = function(u)
  ::L_0498::
  m688.vai_a(u, -34062.4559, -5136.5038, 30, "L_0477")
  m688.salva_modello(u)
  do return end
end

script.L_0477 = function(u)
  ::L_0477::
  m688.vai_a(u, -33990.7898, 13422.8074, 30, "L_0482")
  m688.salva_modello(u)
  do return end
end

script.L_04b4 = function(u)
  ::L_04b4::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0534 = function(u)
  ::L_0534::
  m688.variabile_piu(u, 1)
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0541")
  do return end
end

script.L_0541 = function(u)
  ::L_0541::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0544 = function(u)
  ::L_0544::
  m688.rumore_transitorio(u)
  script.L_055e(u)
  m688.modello_classe(u, "tipo_2d")
  m688.modello_difesa(u, 200)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0628")
  do return end
end

script.L_055e = function(u)
  ::L_055e::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0628", "L_0605")
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

script.L_058e = function(u)
  ::L_058e::
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0628", "L_0605")
  script.L_05b4(u)
  m688.dopo(u, 170, "L_05b1")
  m688.dopo(u, 190, "L_05da")
  m688.dopo(u, 280, "L_0628")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_05b1 = function(u)
  ::L_05b1::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_05b4 = function(u)
  ::L_05b4::
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

script.L_05da = function(u)
  ::L_05da::
  m688.annulla_timer(u, "L_05da")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0605 = function(u)
  ::L_0605::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_05e5")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0621")
  m688.dopo(u, 10, "L_0619")
  do return end
end

script.L_05e5 = function(u)
  ::L_05e5::
  if m688.flag_scena_uguale(u, 1) then goto L_05f3 end
  if m688.bersaglio_id(u, 0) then goto L_05f3 end
  m688.comportamento(u, "L_05e5")
  m688.salva_modello(u)
  do return end
  ::L_05f3::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0619")
  m688.annulla_timer(u, "L_0605")
  m688.insegui(u, 0, "L_0628", "L_0605")
  m688.salva_modello(u)
  do return end
end

script.L_0619 = function(u)
  ::L_0619::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0621 = function(u)
  ::L_0621::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0621")
  do return end
end

script.L_0628 = function(u)
  ::L_0628::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_062c = function(u)
  ::L_062c::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0628", "L_0605")
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
  m688.dopo(u, 2, "L_0659")
  m688.dopo(u, 160, "L_065e")
  do return end
end

script.L_0659 = function(u)
  ::L_0659::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_065e = function(u)
  ::L_065e::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_0662 = function(u)
  ::L_0662::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0667 = function(u)
  ::L_0667::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0605")
  do return end
end

script.L_0671 = function(u, p)
  ::L_0671::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 20, "L_0680")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_0680 = function(u)
  ::L_0680::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_05e5")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_068e end
  m688.messaggio_al_giocatore(u, 42)
  ::L_068e::
  do return end
end

script.L_068f = function(u, p)
  ::L_068f::
  m688.vai_a(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448), 30, "L_069a")
  m688.salva_modello(u)
  do return end
end

script.L_069a = function(u)
  ::L_069a::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_06a1 = function(u, p)
  ::L_06a1::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06a6 = function(u, p)
  ::L_06a6::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_06ad = function(u, p)
  ::L_06ad::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_06b5 = function(u)
  ::L_06b5::
  m688.annulla_timer(u, "L_06f4")
  m688.dopo(u, 20, "L_06f4")
  do return end
end

script.L_06be = function(u)
  ::L_06be::
  m688.annulla_timer(u, "L_06f7")
  m688.dopo(u, 20, "L_06f7")
  do return end
end

script.L_06c7 = function(u)
  ::L_06c7::
  m688.annulla_timer(u, "L_06fa")
  m688.dopo(u, 20, "L_06fa")
  do return end
end

script.L_06d0 = function(u)
  ::L_06d0::
  m688.annulla_timer(u, "L_06fd")
  m688.dopo(u, 20, "L_06fd")
  do return end
end

script.L_06d9 = function(u)
  ::L_06d9::
  m688.annulla_timer(u, "L_0700")
  m688.dopo(u, 20, "L_0700")
  do return end
end

script.L_06e2 = function(u)
  ::L_06e2::
  m688.annulla_timer(u, "L_0703")
  m688.dopo(u, 20, "L_0703")
  do return end
end

script.L_06eb = function(u)
  ::L_06eb::
  m688.annulla_timer(u, "L_0706")
  m688.dopo(u, 20, "L_0706")
  do return end
end

script.L_06f4 = function(u)
  ::L_06f4::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_06f7 = function(u)
  ::L_06f7::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_06fa = function(u)
  ::L_06fa::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_06fd = function(u)
  ::L_06fd::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0700 = function(u)
  ::L_0700::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0703 = function(u)
  ::L_0703::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0706 = function(u)
  ::L_0706::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0709 = function(u)
  ::L_0709::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0711 = function(u, p)
  ::L_0711::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0715 = function(u)
  ::L_0715::
  m688.dopo(u, 60, "L_071f")
  do return end
end

script.L_071b = function(u, p)
  ::L_071b::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_071f = function(u)
  ::L_071f::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0725 = function(u)
  ::L_0725::
  m688.dopo(u, 40, "L_072b")
  do return end
end

script.L_072b = function(u)
  ::L_072b::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_0731 = function(u)
  ::L_0731::
  m688.annulla_timer(u, "L_071f")
  m688.annulla_timer(u, "L_072b")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_073b = function(u, p)
  ::L_073b::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0740 = function(u, p)
  ::L_0740::
  m688.posizione(u, par(p, 1, -147539.5639), par(p, 2, 73824.6448))
  m688.salva_modello(u)
  do return end
end

return missione
