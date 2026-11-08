-- SCENE0A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE0A",
  missione = 10,
  titolo = { it = "Colpisci e fuggi", en = "HIT AND RUN" },
  lato = "usa",
  teatro = "baltico",
  posa = { metodo = "ricollocata", centro = { lat = 54.9086, lon = 15.0808 }, metri_per_unita = { x = 45.930, y = 45.930 }, moto_classico_m_s_per_nodo = 2.2076, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.67); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 60.96, 121.92, 182.88 } },
  dotazione = { mk48 = 20, tomahawk = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "tomahawk" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 54.8571, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_07aa" },
    tomahawk = { gestore = "L_0724" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_0849" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_08ac",
    telegrafo = "L_08be",
    quota = "L_08c3",
    timone = "L_08ca",
    pilota_automatico_off = "L_0926",
    periscopio = "L_092e",
    carica_tubo_1 = "L_08d2",
    carica_tubo_2 = "L_08db",
    carica_tubo_3 = "L_08e4",
    carica_tubo_4 = "L_08ed",
    carica_tubo_5 = "L_08f6",
    carica_tubo_6 = "L_08ff",
    carica_tubo_7 = "L_0908",
    schiera_cala = "L_0932",
    schiera_recupera = "L_0942",
    schiera_persa = "L_094e",
    designa_bersaglio = "L_0958",
    posizione = "L_095d",
    antenna = "L_0938",
    guida_e0 = "L_087f",
    guida_e1 = "L_0884",
    guida_e2 = "L_088e",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.25390625, probabilita_originale = "65/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 34572.48, y = -91639.2 }, originale = { x = 3456, y = 1760 },
        quota_m = 6.096, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "bqm74", tipo_originale = 15, nome_originale = "CRUISE MISSILE TARGET", categoria = "nave", schieramento = "neutrale", flag = 194, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 34572.48, y = -110746.08 }, originale = { x = 3456, y = 1344 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 31632.96, y = -50485.92 }, originale = { x = 3392, y = 2656 },
        quota_m = 23.75, quota_originale_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 8116.8, y = -26969.76 }, originale = { x = 2880, y = 3168 },
        quota_m = 36.5, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 4, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41855.04, y = -6393.12 }, originale = { x = 1792, y = 3616 },
        quota_m = 19, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 50.0, virata_gradi_s = 3.6, accel_kn_s = 0.643, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 5, classe = "kirov", tipo_originale = 6, nome_originale = "KIROV", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -41800, y = 24100 }, originale = { x = 1792, y = 4288 }, spostamento_m = 375.8915,
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 200, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -13929.6, y = -7862.88 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33400, y = 64100 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0234" } },
      { id = 6, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -15399.36, y = -7862.88 }, originale = { x = 2368, y = 3584 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27223.68, y = -31379.04 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -35000, y = 64700 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_023f" } },
      { id = 7, classe = "kara", tipo_originale = 5, nome_originale = "KARA CLASS GUIDED MISSILE CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 27223.68, y = -31379.04 }, originale = { x = 3296, y = 3072 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 69846.72, y = -68123.04 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_024a" } },
      { id = 8, classe = "krivak2", tipo_originale = 7, nome_originale = "KRIVAK II", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 69846.72, y = -68123.04 }, originale = { x = 4224, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 130106.88, y = -50485.92 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -34600, y = 63900 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0255" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 111000, y = -88699.68 }, originale = { x = 5120, y = 1824 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = 90, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02b6" }, { dopo_s = 300, unita = 0, gestore = "L_02fb" }, { dopo_s = 60, unita = 0, gestore = "L_0311" }, { dopo_s = 26, unita = 2, gestore = "L_02c4" }, { dopo_s = 30, unita = 2, gestore = "L_05bb" }, { dopo_s = 46, unita = 3, gestore = "L_02c4" }, { dopo_s = 50, unita = 3, gestore = "L_05bb" }, { dopo_s = 70, unita = 4, gestore = "L_02c4" }, { dopo_s = 50, unita = 4, gestore = "L_05bb" }, { dopo_s = 50, unita = 5, gestore = "L_020c" }, { dopo_s = 70, unita = 6, gestore = "L_020c" }, { dopo_s = 90, unita = 7, gestore = "L_020c" }, { dopo_s = 110, unita = 8, gestore = "L_020c" }, { dopo_s = 60, unita = 9, gestore = "L_05dd" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 4, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 81604.8, y = -94578.72 }, originale = { x = 4480, y = 1696 },
        quota_m = 6.096, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "bqm74", tipo_originale = 15, nome_originale = "CRUISE MISSILE TARGET", categoria = "nave", schieramento = "neutrale", flag = 194, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 81604.8, y = -113685.6 }, originale = { x = 4480, y = 1280 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 78665.28, y = -50485.92 }, originale = { x = 4416, y = 2656 },
        quota_m = 32.75, quota_originale_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35976, y = 2425.44 }, originale = { x = 1920, y = 3808 },
        quota_m = 22.5, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 4, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3641.28, y = -12272.16 }, originale = { x = 2624, y = 3488 },
        quota_m = 34.25, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 50.0, virata_gradi_s = 3.6, accel_kn_s = 0.643, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 5, classe = "kirov", tipo_originale = 6, nome_originale = "KIROV", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -41800, y = 24100 }, originale = { x = 1792, y = 4288 }, spostamento_m = 375.8915,
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 200, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -13929.6, y = -7862.88 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33400, y = 64100 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0234" } },
      { id = 6, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -15399.36, y = -7862.88 }, originale = { x = 2368, y = 3584 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27223.68, y = -31379.04 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -35000, y = 64700 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_023f" } },
      { id = 7, classe = "kara", tipo_originale = 5, nome_originale = "KARA CLASS GUIDED MISSILE CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 27223.68, y = -31379.04 }, originale = { x = 3296, y = 3072 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 69846.72, y = -68123.04 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_024a" } },
      { id = 8, classe = "krivak2", tipo_originale = 7, nome_originale = "KRIVAK II", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 69846.72, y = -68123.04 }, originale = { x = 4224, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 130106.88, y = -50485.92 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -34600, y = 63900 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0255" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -6580.8, y = -26969.76 }, originale = { x = 2560, y = 3168 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = 90, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02b6" }, { dopo_s = 300, unita = 0, gestore = "L_02fb" }, { dopo_s = 60, unita = 0, gestore = "L_0311" }, { dopo_s = 26, unita = 2, gestore = "L_02c4" }, { dopo_s = 30, unita = 2, gestore = "L_05bb" }, { dopo_s = 46, unita = 3, gestore = "L_02c4" }, { dopo_s = 50, unita = 3, gestore = "L_05bb" }, { dopo_s = 70, unita = 4, gestore = "L_02c4" }, { dopo_s = 50, unita = 4, gestore = "L_05bb" }, { dopo_s = 50, unita = 5, gestore = "L_020c" }, { dopo_s = 70, unita = 6, gestore = "L_020c" }, { dopo_s = 90, unita = 7, gestore = "L_020c" }, { dopo_s = 110, unita = 8, gestore = "L_020c" }, { dopo_s = 60, unita = 9, gestore = "L_05dd" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 4, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.25, probabilita_originale = "1/4", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 119800, y = -86300 }, originale = { x = 5280, y = 1824 }, spostamento_m = 2804.3619,
        quota_m = 6.096, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "bqm74", tipo_originale = 15, nome_originale = "CRUISE MISSILE TARGET", categoria = "nave", schieramento = "neutrale", flag = 194, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 118348.8, y = -107806.56 }, originale = { x = 5280, y = 1408 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 31632.96, y = -50485.92 }, originale = { x = 3392, y = 2656 },
        quota_m = 23.75, quota_originale_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 8116.8, y = -26969.76 }, originale = { x = 2880, y = 3168 },
        quota_m = 36.5, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 4, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -41855.04, y = -6393.12 }, originale = { x = 1792, y = 3616 },
        quota_m = 19, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 50.0, virata_gradi_s = 3.6, accel_kn_s = 0.643, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 5, classe = "kirov", tipo_originale = 6, nome_originale = "KIROV", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -41800, y = 24100 }, originale = { x = 1792, y = 4288 }, spostamento_m = 375.8915,
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 200, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -13929.6, y = -7862.88 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33400, y = 64100 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0234" } },
      { id = 6, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -15399.36, y = -7862.88 }, originale = { x = 2368, y = 3584 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27223.68, y = -31379.04 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -35000, y = 64700 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_023f" } },
      { id = 7, classe = "kara", tipo_originale = 5, nome_originale = "KARA CLASS GUIDED MISSILE CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 27223.68, y = -31379.04 }, originale = { x = 3296, y = 3072 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 69846.72, y = -68123.04 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_024a" } },
      { id = 8, classe = "krivak2", tipo_originale = 7, nome_originale = "KRIVAK II", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 69846.72, y = -68123.04 }, originale = { x = 4224, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 130106.88, y = -50485.92 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -34600, y = 63900 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0255" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 111000, y = -88699.68 }, originale = { x = 5120, y = 1824 },
        quota_m = 30.48, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = 90, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02b6" }, { dopo_s = 300, unita = 0, gestore = "L_02fb" }, { dopo_s = 60, unita = 0, gestore = "L_0311" }, { dopo_s = 26, unita = 2, gestore = "L_02c4" }, { dopo_s = 30, unita = 2, gestore = "L_05bb" }, { dopo_s = 46, unita = 3, gestore = "L_02c4" }, { dopo_s = 50, unita = 3, gestore = "L_05bb" }, { dopo_s = 70, unita = 4, gestore = "L_02c4" }, { dopo_s = 50, unita = 4, gestore = "L_05bb" }, { dopo_s = 50, unita = 5, gestore = "L_020c" }, { dopo_s = 70, unita = 6, gestore = "L_020c" }, { dopo_s = 90, unita = 7, gestore = "L_020c" }, { dopo_s = 110, unita = 8, gestore = "L_020c" }, { dopo_s = 60, unita = 9, gestore = "L_05dd" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 4, [5] = 0, [6] = 0 },
    },
    { probabilita = 0.24609375, probabilita_originale = "63/256", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 60200, y = -95100 }, originale = { x = 4064, y = 1728 }, spostamento_m = 3040.506,
        quota_m = 5, quota_originale_m = 6.096, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 180, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "bqm74", tipo_originale = 15, nome_originale = "CRUISE MISSILE TARGET", categoria = "nave", schieramento = "neutrale", flag = 194, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 62497.92, y = -112215.84 }, originale = { x = 4064, y = 1312 },
        quota_m = 6.096, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 78665.28, y = -50485.92 }, originale = { x = 4416, y = 2656 },
        quota_m = 32.75, quota_originale_m = 76.2, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 3, classe = "victor3", tipo_originale = 23, nome_originale = "VICTOR III CLASS FAST ATTACK SUB", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -35976, y = 2425.44 }, originale = { x = 1920, y = 3808 },
        quota_m = 22.5, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 160, rumore = 10, sonar = 18, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 4, classe = "akula", tipo_originale = 29, nome_originale = "AKULA CLASS FAST ATTACK", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = -3641.28, y = -12272.16 }, originale = { x = 2624, y = 3488 },
        quota_m = 34.25, quota_originale_m = 91.44, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 50.0, virata_gradi_s = 3.6, accel_kn_s = 0.643, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 255, rumore = 10, sonar = 20, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 30163.2, y = -41667.36 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_04d4", reazione = nil, arrivo = "L_023f" } },
      { id = 5, classe = "kirov", tipo_originale = 6, nome_originale = "KIROV", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -41800, y = 24100 }, originale = { x = 1792, y = 4288 }, spostamento_m = 375.8915,
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 200, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = -13929.6, y = -7862.88 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33400, y = 64100 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0234" } },
      { id = 6, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = -15399.36, y = -7862.88 }, originale = { x = 2368, y = 3584 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 27223.68, y = -31379.04 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -35000, y = 64700 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_023f" } },
      { id = 7, classe = "kara", tipo_originale = 5, nome_originale = "KARA CLASS GUIDED MISSILE CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 27223.68, y = -31379.04 }, originale = { x = 3296, y = 3072 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 150, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 69846.72, y = -68123.04 }, { x = 8116.8, y = -6393.12 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -33800, y = 62500 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_024a" } },
      { id = 8, classe = "krivak2", tipo_originale = 7, nome_originale = "KRIVAK II", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 69846.72, y = -68123.04 }, originale = { x = 4224, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 18.2857,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 100, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 130106.88, y = -50485.92 }, { x = 6647.04, y = -26969.76 }, { x = 25753.92, y = -29909.28 }, { x = 31632.96, y = -49016.16 }, { x = 47800.32, y = -40197.6 }, { x = 50739.84, y = -59304.48 }, { x = 66907.2, y = -50485.92 }, { x = 69846.72, y = -68123.04 }, { x = 87483.84, y = -68123.04 }, { x = -34600, y = 63900 }, { x = -41855.04, y = 24471.84 }, { x = -38915.52, y = 5364.96 }, { x = -18338.88, y = 6834.72 }, { x = -15399.36, y = -6393.12 }, { x = 8116.8, y = -6393.12 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0424", reazione = nil, arrivo = "L_0255" } },
      { id = 9, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 2, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -6580.8, y = -26969.76 }, originale = { x = 2560, y = 3168 },
        quota_m = 60.96, rotta = 0, velocita_kn = 1.8286,
        modello = { massima_kn = 1.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 10, rumore = 25, sonar = 18, difesa = 90, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_02b6" }, { dopo_s = 300, unita = 0, gestore = "L_02fb" }, { dopo_s = 60, unita = 0, gestore = "L_0311" }, { dopo_s = 26, unita = 2, gestore = "L_02c4" }, { dopo_s = 30, unita = 2, gestore = "L_05bb" }, { dopo_s = 46, unita = 3, gestore = "L_02c4" }, { dopo_s = 50, unita = 3, gestore = "L_05bb" }, { dopo_s = 70, unita = 4, gestore = "L_02c4" }, { dopo_s = 50, unita = 4, gestore = "L_05bb" }, { dopo_s = 50, unita = 5, gestore = "L_020c" }, { dopo_s = 70, unita = 6, gestore = "L_020c" }, { dopo_s = 90, unita = 7, gestore = "L_020c" }, { dopo_s = 110, unita = 8, gestore = "L_020c" }, { dopo_s = 60, unita = 9, gestore = "L_05dd" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0, [4] = 4, [5] = 0, [6] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "Comandante, la Marina è giunta alla conclusione che la difesa avanzata sia l'unica strategia in grado di farci vincere questa guerra. Come la Marina ha sempre sostenuto, la miglior difesa è l'attacco! Inizieremo una serie di attacchi contro obiettivi nell'entroterra, per indebolire lo sforzo bellico sovietico smantellandone la capacità di rappresaglia con missili balistici intercontinentali. Dalla sua posizione attuale nel Mar Baltico lancerà il suo carico di missili Tomahawk sugli obiettivi strategici dell'entroterra già concordati. Da quel momento, Comandante, sarà solo. Le raccomandiamo di riparare nelle acque di Copenaghen il più rapidamente possibile. Data la delicatezza di questa operazione, la Marina sarà costretta a negare qualsiasi conoscenza delle sue intenzioni e non potrà inviare alcun aiuto se il suo sottomarino verrà scoperto. Le auguro sinceramente tutta la fortuna possibile, e che Dio ci protegga tutti." },
    en = { "MISSION ORDERS", "CAPTAIN, THE NAVY HAS COME TO THE CONCLUSION THAT FORWARD DEFENSE IS THE ONLY STRATEGY THAT WILL WIN US THIS WAR. AS THE NAVY HAS ALWAYS MAINTAINED, THE BEST DEFENSE IS A GOOD OFFENSE! WE WILL BEGIN A SERIES OF STRIKES ON INLAND TARGETS IN ORDER TO WEAKEN THE SOVIET'S WAR EFFORT BY BREAKING DOWN THEIR ICBM RETALIATORY CAPABILITY. FROM YOUR CURRENT POSITION IN THE BALTIC SEA, YOU WILL DELIVER YOUR PAYLOAD OF TOMAHAWK MISSILES TO OUR PRE-ARRANGED STRATEGIC INLAND TARGETS. AT THAT POINT, CAPTAIN, YOU'RE ON YOUR OWN. WE RECOMMEND THAT YOU ESCAPE INTO COPENHAGEN WATERS AS QUICKLY AS POSSIBLE. BECAUSE OF THE SENSITIVITY OF THIS OPERATION, THE NAVY WILL BE FORCED TO DENY ANY KNOWLEDGE OF YOUR INTENT AND WILL NOT BE ABLE TO SEND ANY ASSISTANCE IF YOUR SUBMARINE IS DISCOVERED. I SINCERELY WISH YOU ALL THE LUCK, AND MAY GOD PROTECT US ALL." },
  },
  esiti = {
    [1] = { it = "CODICE: 10-1-ACP\nIl Los Angeles si è incagliato ed è affondato oggi durante manovre di routine in acque internazionali. Le accuse sovietiche di azioni ostili da parte di un sottomarino statunitense nel Baltico sono infondate. Per ulteriori informazioni sull'incidente, presentare il modulo DD 1096.5 (rev. Q) all'Ufficio Informazioni del Dipartimento della Marina. Le richieste saranno evase entro 120 giorni dal ricevimento, con un periodo di lavorazione di 90 giorni. Le nostre condoglianze vanno alle famiglie e agli amici di questo equipaggio valoroso e onorato.", en = "CODE: 10-1-ACP\nTHE LOS ANGELES RAN AGROUND AND SANK DURING ROUTINE MANEUVERS IN INTERNATIONAL WATERS TODAY. SOVIET CLAIMS OF HOSTILE ACTIONS BY A US SUB IN THE BALTIC ARE UNFOUNDED. FOR MORE INFORMATION ON THE INCIDENT, SUBMIT DD FORM 1096.5 (REV Q) TO THE BUREAU OF INFORMATION, DEPARTMENT OF THE NAVY. INQUIRIES WILL BE HANDLED IN NO MORE THAN 120 DAYS OF RECEIPT WITH A 90 DAY PROCESSING PERIOD. OUR CONDOLENCES GO OUT TO THE FAMILIES AND FRIENDS OF THIS BRAVE AND HONORABLE CREW." },
    [2] = { it = "CODICE: 10-2-ACP\nLe accuse sovietiche di un'infiltrazione statunitense nel Mar Baltico e di successivi attacchi a obiettivi terrestri e navali costituiscono un nuovo precedente nella propaganda sovietica. Il Los Angeles è andato perduto in mare, in acque internazionali, durante operazioni riservate. Il sottomarino non ha, ripeto, non ha ingaggiato alcuna unità da guerra nemica. La causa esatta dell'incidente è attualmente oggetto di indagine. Ci uniamo al dolore delle famiglie e degli amici di questo equipaggio valoroso e onorato. Lo sfruttamento sovietico di questo incidente è deplorevole. Per ulteriori informazioni, presentare il modulo DD 1096.5 (rev. Q) all'Ufficio Informazioni del Dipartimento della Marina. Le richieste saranno evase entro 120 giorni dal ricevimento, con un periodo di lavorazione di 90 giorni.", en = "CODE: 10-2-ACP\nSOVIET CLAIMS OF U.S. INFILTRATION OF THE BALTIC SEA AND SUBSEQUENT ATTACK OF LAND AND SEA TARGETS CREATES A NEW PRECEDENT IN SOVIET PROPAGANDA. THE LOS ANGELES WAS LOST AT SEA IN INTERNATIONAL WATERS ON CLASSIFIED OPERATIONS. THE SUBMARINE DID NOT, I REPEAT, DID NOT ENGAGE ANY ENEMY WARSHIPS. THE EXACT CAUSE OF THE INCIDENT IS CURRENTLY UNDER INVESTIGATION. WE SHARE IN THE GRIEF OF THE FAMILIES AND FRIENDS OF THIS BRAVE AND HONORABLE CREW. THE SOVIET EXPLOITATION OF THIS INCIDENT IS UNFORTUNATE. FOR MORE INFORMATION, SUBMIT DD FORM 1096.5 (REV Q) TO THE BUREAU OF INFORMATION, DEPARTMENT OF THE NAVY. INQUIRIES WILL BE HANDLED IN NO MORE THAN 120 DAYS OF RECEIPT WITH A 90 DAY PROCESSING PERIOD." },
    [3] = { it = "CODICE: 10-3-ACP\nComandante, aveva l'ordine esplicito di lanciare i suoi missili e, per qualche motivo, ha deciso altrimenti. Molti comandanti parlano di coraggio, ma a quanto pare alla sua gente manca il fegato per affrontare una missione davvero pericolosa. Sembra proprio che lei non abbia la stoffa necessaria. Rientri in porto.", en = "CODE: 10-3-ACP\nCAPTAIN, YOU WERE GIVEN EXPRESS ORDERS TO LAUNCH YOUR MISSILES, AND FOR SOME REASON YOU DECIDED TO DO OTHERWISE. A LOT OF COMMANDERS TALK ABOUT COURAGE BUT I GUESS YOUR PEOPLE JUST DON'T HAVE THE GUTS TO TAKE ON A TRULY DANGEROUS MISSION. IT SEEMS THAT YOU JUST DON'T HAVE WHAT IT TAKES. RETURN TO PORT." },
    [4] = { it = "CODICE: 10-4-ACP\nVorrei proprio sapere dove diavolo credeva di andare. A quanto pare alla sua gente manca il fegato per buttarsi nella mischia e giocare duro con i pezzi grossi. E invece scopro che state scappando da qualsiasi combattimento. Siete a corto di ormoni maschili, o cosa?", en = "CODE: 10-4-ACP\nI WOULD LIKE TO KNOW JUST WHERE THE HELL YOU THINK YOU WERE GOING. I GUESS YOU PEOPLE JUST DON'T HAVE THE GUTS TO HEAD FOR THE ACTION AND PLAY HARDBALL WITH THE BIG BOYS. INSTEAD, I FIND OUT YOU'RE RUNNING AWAY FROM ALL THE ACTION. ARE YOU PEOPLE RUNNING LOW ON MALE HORMONE OR WHAT?" },
    [5] = { it = "CODICE: 10-5-ACP\nIl nostro saluto al Los Angeles, di ritorno da quella che non si può che definire una missione estremamente pericolosa. Il comandante e l'equipaggio hanno portato a termine la missione e si sono distinti ingaggiando ed eludendo gran parte della flotta sovietica del Baltico durante il viaggio di ritorno. L'equipaggio del Los Angeles merita un encomio per il suo coraggio: riceverà la Navy Unit Commendation, e al comandante sarà conferita la Navy Cross. Ottimo lavoro!", en = "CODE: 10-5-ACP\nGREETINGS ON THE RETURN OF THE LOS ANGELES FROM WHAT CAN ONLY BE CALLED A HIGHLY DANGEROUS MISSION. THE CAPTAIN AND CREW SUCCESSFULLY COMPLETED THEIR MISSION AND DISTINGUISHED THEMSELVES BY ENGAGING AND EVADING THE MAJORITY OF THE SOVIET'S BALTIC FLEET DURING THEIR RETURN JOURNEY. THE CREW OF THE LOS ANGELES ARE TO BE COMMENDED FOR THEIR BRAVERY. THE CREW WILL RECEIVE A NAVY UNIT COMMENDATION AND THE CAPTAIN WILL BE AWARDED THE NAVY CROSS. GOOD WORK!" },
    [6] = { it = "CODICE: 10-6-ACP\nI miei più calorosi saluti al comandante e all'equipaggio del Los Angeles! Lasciate che esprima la mia ammirazione per il coraggio e la distinzione con cui il Los Angeles ha portato a termine una missione estremamente pericolosa. Il Los Angeles non solo ha colpito con successo i suoi obiettivi nell'entroterra, ma ha anche distrutto diverse unità sovietiche durante il rientro in acque amiche. All'equipaggio del Los Angeles è conferita la Navy Unit Commendation per un servizio ben oltre il proprio dovere. Al comandante è conferita la Medal of Honor, la più alta decorazione delle Forze Armate. Congratulazioni per una prestazione straordinaria!", en = "CODE: 10-6-ACP\nMY WARMEST GREETINGS TO THE CAPTAIN AND CREW OF THE LOS ANGELES! LET ME EXPRESS MY ADMIRATION FOR THE BRAVERY AND DISTINCTION WITH WHICH THE LOS ANGELES PERFORMED A HIGHLY DANGEROUS MISSION. NOT ONLY DID THE LOS ANGELES SUCCESSFULLY STRIKE HER INLAND TARGETS, SHE ALSO DESTROYED SEVERAL SOVIET VESSELS WHILE RETURNING TO FRIENDLY WATERS. THE CREW OF THE LOS ANGELES ARE HEREBY AWARDED THE NAVY UNIT COMMENDATION FOR SERVICE ABOVE AND BEYOND THE CALL OF DUTY. THE CAPTAIN IS AWARDED THE MEDAL OF HONOR, THE HIGHEST DECORATION AWARDED IN THE ARMED FORCES. CONGRATULATIONS FOR AN INCREDIBLE PERFORMANCE!" },
    [19] = { it = "CODICE: 10-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 10-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 6, punti = 33, spostamento_max_m = 3867.0602, non_risolte = 2 },
  script = script,
}

script.L_020c = function(u)
  ::L_020c::
  m688.marca_contatti(u)
  m688.dopo(u, 20, "L_020c")
  do return end
end

script.L_0234 = function(u)
  ::L_0234::
  m688.vai_a(u, -18338.88, 6834.72, 30, "L_023f")
  m688.salva_modello(u)
  do return end
end

script.L_023f = function(u)
  ::L_023f::
  m688.vai_a(u, -15399.36, -6393.12, 30, "L_024a")
  m688.salva_modello(u)
  do return end
end

script.L_024a = function(u)
  ::L_024a::
  m688.vai_a(u, 8116.8, -6393.12, 30, "L_0255")
  m688.salva_modello(u)
  do return end
end

script.L_0255 = function(u)
  ::L_0255::
  m688.vai_a(u, 6647.04, -26969.76, 30, "L_0260")
  m688.salva_modello(u)
  do return end
end

script.L_0260 = function(u)
  ::L_0260::
  m688.vai_a(u, 25753.92, -29909.28, 30, "L_026b")
  m688.salva_modello(u)
  do return end
end

script.L_026b = function(u)
  ::L_026b::
  m688.vai_a(u, 31632.96, -49016.16, 30, "L_0276")
  m688.salva_modello(u)
  do return end
end

script.L_0276 = function(u)
  ::L_0276::
  m688.vai_a(u, 47800.32, -40197.6, 30, "L_0281")
  m688.salva_modello(u)
  do return end
end

script.L_0281 = function(u)
  ::L_0281::
  m688.vai_a(u, 50739.84, -59304.48, 30, "L_028c")
  m688.salva_modello(u)
  do return end
end

script.L_028c = function(u)
  ::L_028c::
  m688.vai_a(u, 66907.2, -50485.92, 30, "L_0297")
  m688.salva_modello(u)
  do return end
end

script.L_0297 = function(u)
  ::L_0297::
  m688.vai_a(u, 69846.72, -68123.04, 30, "L_02a2")
  m688.salva_modello(u)
  do return end
end

script.L_02a2 = function(u)
  ::L_02a2::
  m688.vai_a(u, 87483.84, -68123.04, 30, "L_02ad")
  m688.salva_modello(u)
  do return end
end

script.L_02ad = function(u)
  ::L_0213::
  m688.vai_a(u, -33800, 62500, 30, "L_021e")
  m688.salva_modello(u)
  do return end
  ::L_02ad::
  m688.posizione(u, -30096.96, 90611.04)
  goto L_0213
end

script.L_021e = function(u)
  ::L_021e::
  m688.vai_a(u, -41855.04, 24471.84, 30, "L_0229")
  m688.salva_modello(u)
  do return end
end

script.L_0229 = function(u)
  ::L_0229::
  m688.vai_a(u, -38915.52, 5364.96, 30, "L_0234")
  m688.salva_modello(u)
  do return end
end

script.L_02b6 = function(u)
  ::L_02b6::
  if m688.manomesso(u) then goto L_02ba end
  do return end
  ::L_02ba::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_02c4 = function(u)
  ::L_02c4::
  if m688.in_rettangolo(u, -55082.88, 37699.68, -25687.68, -26969.76) then goto L_02f5 end
  if m688.in_rettangolo(u, -55082.88, 11244, 49270.08, -24030.24) then goto L_02f5 end
  if m688.in_rettangolo(u, 6647.04, 6834.72, 52209.6, -76941.6) then goto L_02f5 end
  if m688.in_rettangolo(u, 18405.12, -37258.08, 137455.68, -85760.16) then goto L_02f5 end
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.5)
  m688.salva_modello(u)
  ::L_02f5::
  m688.dopo(u, 40, "L_02c4")
  do return end
end

script.L_02fb = function(u)
  ::L_02fb::
  if m688.variabile_uguale(u, 4, 0) then goto L_0305 end
  m688.messaggio(u, 103)
  do return end
  ::L_0305::
  do return end
end

script.L_0311 = function(u)
  ::L_0311::
  if m688.unita_esiste(u, 5) then goto L_033e end
  if m688.flag_scena_bit(u, 1) then goto L_033e end
  m688.flag_scena_accendi(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 4, "L_0311")
  m688.modello_azzera(u)
  script.L_0687(u)
  m688.posizione(u, -30096.96, 90611.04)
  m688.vai_a(u, -33800, 62500, 30, "L_021e")
  m688.comportamento(u, "L_0424")
  m688.nuova_unita(u)
  m688.dopo(u, 30, "L_020c")
  do return end
  ::L_033e::
  if m688.unita_esiste(u, 6) then goto L_036b end
  if m688.flag_scena_bit(u, 2) then goto L_036b end
  m688.flag_scena_accendi(u, 2)
  m688.salva_modello(u)
  m688.dopo(u, 4, "L_0311")
  m688.modello_azzera(u)
  script.L_068f(u)
  m688.posizione(u, -30096.96, 90611.04)
  m688.vai_a(u, -33800, 62500, 30, "L_021e")
  m688.comportamento(u, "L_0424")
  m688.nuova_unita(u)
  m688.dopo(u, 30, "L_020c")
  do return end
  ::L_036b::
  if m688.unita_esiste(u, 7) then goto L_039d end
  if m688.flag_scena_bit(u, 4) then goto L_039d end
  m688.flag_scena_accendi(u, 4)
  m688.salva_modello(u)
  m688.dopo(u, 4, "L_0311")
  m688.modello_azzera(u)
  m688.lancia(u, "L_0845", "L_0822")
  script.L_0697(u)
  m688.posizione(u, -30096.96, 90611.04)
  m688.vai_a(u, -33800, 62500, 30, "L_021e")
  m688.comportamento(u, "L_0424")
  m688.nuova_unita(u)
  m688.dopo(u, 30, "L_020c")
  do return end
  ::L_039d::
  if m688.unita_esiste(u, 8) then goto L_03ca end
  if m688.flag_scena_bit(u, 8) then goto L_03ca end
  m688.flag_scena_accendi(u, 8)
  m688.salva_modello(u)
  m688.dopo(u, 4, "L_0311")
  m688.modello_azzera(u)
  script.L_069f(u)
  m688.posizione(u, -30096.96, 90611.04)
  m688.vai_a(u, -33800, 62500, 30, "L_021e")
  m688.comportamento(u, "L_0424")
  m688.nuova_unita(u)
  m688.dopo(u, 30, "L_020c")
  do return end
  ::L_03ca::
  if m688.in_rettangolo(u, -75659.52, 90611.04, -5111.04, 46518.24) then goto L_03e9 end
  if m688.in_rettangolo(u, -94766.4, 46518.24, 146274.24, -115155.36) then goto L_03e3 end
  m688.messaggio(u, 104)
  do return end
  ::L_03e3::
  m688.dopo(u, 20, "L_0311")
  do return end
  ::L_03e9::
  if m688.unita_esiste(u, 2) then goto L_03f1 end
  m688.messaggio(u, 106)
  do return end
  ::L_03f1::
  if m688.unita_esiste(u, 3) then goto L_03f9 end
  m688.messaggio(u, 106)
  do return end
  ::L_03f9::
  if m688.unita_esiste(u, 4) then goto L_0401 end
  m688.messaggio(u, 106)
  do return end
  ::L_0401::
  if m688.unita_esiste(u, 5) then goto L_0409 end
  m688.messaggio(u, 106)
  do return end
  ::L_0409::
  if m688.unita_esiste(u, 6) then goto L_0411 end
  m688.messaggio(u, 106)
  do return end
  ::L_0411::
  if m688.unita_esiste(u, 7) then goto L_0419 end
  m688.messaggio(u, 106)
  do return end
  ::L_0419::
  if m688.unita_esiste(u, 8) then goto L_0421 end
  m688.messaggio(u, 106)
  do return end
  ::L_0421::
  m688.messaggio(u, 105)
  do return end
end

script.L_0424 = function(u)
  ::L_0424::
  if m688.bersaglio_id(u, 0) then goto L_045c end
  if m688.bersaglio_arma(u) then goto L_0431 end
  ::L_042c::
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  do return end
  ::L_0431::
  if m688.bersaglio_mi_punta(u) then goto L_0437 end
  goto L_042c
  ::L_0437::
  if m688.traccia_solida(u) then goto L_0449 end
  ::L_043a::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.annulla_timer(u, "L_04ae")
  m688.dopo(u, 60, "L_04ae")
  do return end
  ::L_0449::
  if m688.bersaglio_vicino(u, 0.8) then goto L_0451 end
  goto L_043a
  ::L_0451::
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 16)
  do return end
  ::L_045c::
  if m688.variabile_uguale(u, 5, 4) then goto L_0466 end
  goto L_0484
  ::L_0466::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.annulla_timer(u, "L_04ae")
  m688.dopo(u, 60, "L_04ae")
  if m688.traccia_solida(u) then goto L_0479 end
  do return end
  ::L_0479::
  if m688.bersaglio_vicino(u, 3) then goto L_047f end
  do return end
  ::L_047f::
  m688.attacca(u, 6, 15)
  do return end
  ::L_0484::
  if m688.traccia_solida(u) then goto L_04b4 end
  ::L_0487::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.annulla_timer(u, "L_04ae")
  m688.dopo(u, 60, "L_04ae")
  do return end
  ::L_049a::
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  m688.variabile_piu(u, 5)
  script.L_073d(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_05b7")
  do return end
  ::L_04b4::
  if m688.bersaglio_vicino(u, 5.6) then goto L_049a end
  if m688.variabile_maggiore(u, 5, 1) then goto L_0487 end
  m688.variabile_piu(u, 5)
  m688.comportamento(u, "L_0424")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  script.L_0757(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_05b7")
  do return end
end

script.L_04ae = function(u)
  ::L_04ae::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.7812)
  m688.salva_modello(u)
  do return end
end

script.L_04d4 = function(u)
  ::L_04d4::
  if m688.bersaglio_id(u, 0) then goto L_0547 end
  if m688.bersaglio_arma(u) then goto L_04e1 end
  ::L_04dc::
  m688.comportamento(u, "L_04d4")
  m688.salva_modello(u)
  do return end
  ::L_04e1::
  if m688.bersaglio_mi_punta(u) then goto L_04e7 end
  goto L_04dc
  ::L_04e7::
  if m688.traccia_solida(u) then goto L_04f6 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_04e7", "L_04dc")
  m688.salva_modello(u)
  do return end
  ::L_04f6::
  if m688.bersaglio_vicino(u, 3) then goto L_0517 end
  ::L_04fb::
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_05bb")
  m688.dopo(u, 60, "L_04dc")
  m688.salva_modello(u)
  do return end
  ::L_0517::
  if m688.flag_scena_bit(u, 16) then goto L_04fb end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0527")
  do return end
  ::L_0547::
  if m688.traccia_solida(u) then goto L_056b end
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  m688.ingaggia(u, 20, "L_0547", "L_055e")
  m688.salva_modello(u)
  do return end
  ::L_056b::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_0547", "L_055e")
  if m688.bersaglio_vicino(u, 8) then goto L_0583 end
  goto L_05ab
  ::L_0583::
  if m688.variabile_uguale(u, 5, 4) then goto L_05ab end
  m688.salva_modello(u)
  m688.variabile_piu(u, 5)
  if m688.unita_id(u, 4) then goto L_059f end
  script.L_073d(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 242, "L_05b7")
  do return end
  ::L_059f::
  script.L_0748(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 242, "L_05b7")
  do return end
  ::L_05ab::
  if m688.bersaglio_vicino(u, 1.2) then goto L_05b2 end
  m688.salva_modello(u)
  do return end
  ::L_05b2::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_04dc = function(u)
  ::L_04dc::
  m688.comportamento(u, "L_04d4")
  m688.salva_modello(u)
  do return end
end

script.L_04e7 = function(u)
  ::L_04e7::
  if m688.traccia_solida(u) then goto L_04f6 end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_04e7", "L_04dc")
  m688.salva_modello(u)
  do return end
  ::L_04f6::
  if m688.bersaglio_vicino(u, 3) then goto L_0517 end
  ::L_04fb::
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_05bb")
  m688.dopo(u, 60, "L_04dc")
  m688.salva_modello(u)
  do return end
  ::L_0517::
  if m688.flag_scena_bit(u, 16) then goto L_04fb end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0527")
  do return end
end

script.L_0527 = function(u)
  ::L_0527::
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 182.88, 1)
  m688.salva_modello(u)
  m688.dopo(u, 80, "L_0542")
  m688.dopo(u, 120, "L_053e")
  goto L_0849
  ::L_0849::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0845", "L_0822")
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
  m688.dopo(u, 2, "L_0876")
  m688.dopo(u, 160, "L_087b")
  do return end
end

script.L_053e = function(u)
  ::L_053e::
  m688.flag_scena_spegni(u, 16)
  m688.salva_modello(u)
  do return end
end

script.L_0542 = function(u)
  ::L_0542::
  m688.comportamento(u, "L_04d4")
  m688.salva_modello(u)
  do return end
end

script.L_0547 = function(u)
  ::L_0547::
  if m688.traccia_solida(u) then goto L_056b end
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  m688.ingaggia(u, 20, "L_0547", "L_055e")
  m688.salva_modello(u)
  do return end
  ::L_056b::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_05bb")
  m688.annulla_timer(u, "L_05ce")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_0547", "L_055e")
  if m688.bersaglio_vicino(u, 8) then goto L_0583 end
  goto L_05ab
  ::L_0583::
  if m688.variabile_uguale(u, 5, 4) then goto L_05ab end
  m688.salva_modello(u)
  m688.variabile_piu(u, 5)
  if m688.unita_id(u, 4) then goto L_059f end
  script.L_073d(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 242, "L_05b7")
  do return end
  ::L_059f::
  script.L_0748(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 242, "L_05b7")
  do return end
  ::L_05ab::
  if m688.bersaglio_vicino(u, 1.2) then goto L_05b2 end
  m688.salva_modello(u)
  do return end
  ::L_05b2::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_055e = function(u)
  ::L_055e::
  m688.telegrafo(u, 0.5)
  m688.comportamento(u, "L_04d4")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_05bb")
  do return end
end

script.L_05b7 = function(u)
  ::L_05b7::
  m688.variabile_meno(u, 5)
  do return end
end

script.L_05bb = function(u)
  ::L_05bb::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 0.0195)
  m688.virata_relativa(u, -180)
  m688.quota(u, 274.32, 0.3906)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_05ce")
  do return end
end

script.L_05ce = function(u)
  ::L_05ce::
  m688.telegrafo(u, 0.5)
  m688.quota(u, 30.48, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_05bb")
  do return end
end

script.L_05dd = function(u)
  ::L_05dd::
  m688.nuovo_caso(u)
  m688.posizione(u, -40385.28, 15653.28)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_066a end
  m688.posizione(u, -27157.44, 955.68)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_066a end
  m688.posizione(u, -701.76, 9774.24)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_066a end
  m688.posizione(u, -22748.16, -19620.96)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_066a end
  m688.posizione(u, -5111.04, -21090.72)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_066a end
  m688.posizione(u, 18405.12, -4923.36)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_066a end
  m688.posizione(u, 41921.28, -1983.84)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_066a end
  m688.posizione(u, 36042.24, -16681.44)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_066a end
  m688.posizione(u, 18405.12, -38727.84)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_066a end
  m688.posizione(u, 28693.44, -50485.92)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_066a end
  m688.posizione(u, 25753.92, -63713.76)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_066a end
  m688.posizione(u, 52209.6, -47546.4)
  m688.rotta_ora(u, 300)
  ::L_066a::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_05dd")
  do return end
end

script.L_0687 = function(u)
  ::L_0687::
  script.L_06a7(u)
  m688.modello_classe(u, "kirov")
  m688.modello_integrita(u, 200)
  do return end
end

script.L_068f = function(u)
  ::L_068f::
  script.L_06a7(u)
  m688.modello_classe(u, "slava")
  m688.modello_integrita(u, 175)
  do return end
end

script.L_0697 = function(u)
  ::L_0697::
  script.L_06a7(u)
  m688.modello_classe(u, "kara")
  m688.modello_integrita(u, 150)
  do return end
end

script.L_069f = function(u)
  ::L_069f::
  script.L_06a7(u)
  m688.modello_classe(u, "krivak2")
  m688.modello_integrita(u, 100)
  do return end
end

script.L_06a7 = function(u)
  ::L_06a7::
  m688.modello_azzera(u)
  m688.modello_tipo(u, 2)
  m688.modello_flag(u, 18)
  m688.modello_3c(u, 0)
  m688.quota_ora(u, 0)
  m688.modello_rateo_quota(u, 0.6858)
  m688.modello_virata(u, 1.44)
  m688.modello_accelerazione(u, 0.0514)
  m688.modello_avanti_tutta(u, 27.4286)
  m688.velocita_ora(u, 18.2857)
  m688.modello_rumore(u, 24)
  m688.rotta_ora(u, 0)
  m688.modello_sonar(u, 18)
  m688.modello_difesa(u, 90)
  m688.flag_scena(u, 0)
  m688.schiera_trainata(u, 1)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  do return end
end

script.L_0724 = function(u)
  ::L_0724::
  m688.variabile_piu(u, 1)
  m688.variabile_meno(u, 4)
  m688.messaggio_al_giocatore(u, 49)
  m688.lancio_missile(u)
  do return end
end

script.L_073d = function(u)
  ::L_073d::
  m688.rumore_transitorio(u)
  script.L_077a(u)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0845")
  do return end
end

script.L_0748 = function(u)
  ::L_0748::
  m688.rumore_transitorio(u)
  script.L_077a(u)
  m688.modello_classe(u, "tipo_2d")
  m688.modello_difesa(u, 200)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_0845")
  do return end
end

script.L_0757 = function(u)
  ::L_0757::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_077a(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_classe(u, "type40")
  m688.modello_difesa(u, 40)
  m688.comportamento(u, "L_0802")
  m688.marca_contatti(u)
  m688.telegrafo(u, 0.7812)
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.nuova_unita(u)
  m688.dopo(u, 10, "L_083e")
  m688.dopo(u, 240, "L_0845")
  do return end
end

script.L_077a = function(u)
  ::L_077a::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_0845", "L_0822")
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

script.L_07aa = function(u)
  ::L_07aa::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_0845", "L_0822")
  script.L_07d1(u)
  m688.dopo(u, 170, "L_07ce")
  m688.dopo(u, 190, "L_07f7")
  m688.dopo(u, 280, "L_0845")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_07ce = function(u)
  ::L_07ce::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_07d1 = function(u)
  ::L_07d1::
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 0.7714)
  m688.modello_avanti_tutta(u, 54.8571)
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

script.L_07f7 = function(u)
  ::L_07f7::
  m688.annulla_timer(u, "L_07f7")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0802 = function(u)
  ::L_0802::
  if m688.flag_scena_uguale(u, 1) then goto L_0810 end
  if m688.bersaglio_id(u, 0) then goto L_0810 end
  m688.comportamento(u, "L_0802")
  m688.salva_modello(u)
  do return end
  ::L_0810::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0836")
  m688.annulla_timer(u, "L_0822")
  m688.insegui(u, 0, "L_0845", "L_0822")
  m688.salva_modello(u)
  do return end
end

script.L_0822 = function(u)
  ::L_0822::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0802")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_083e")
  m688.dopo(u, 10, "L_0836")
  do return end
end

script.L_0836 = function(u)
  ::L_0836::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_083e = function(u)
  ::L_083e::
  m688.impulso_sonar(u)
  m688.dopo(u, 20, "L_083e")
  do return end
end

script.L_0845 = function(u)
  ::L_0845::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_0849 = function(u)
  ::L_0849::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_0845", "L_0822")
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
  m688.dopo(u, 2, "L_0876")
  m688.dopo(u, 160, "L_087b")
  do return end
end

script.L_0876 = function(u)
  ::L_0876::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_087b = function(u)
  ::L_087b::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_087f = function(u)
  ::L_087f::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_0884 = function(u)
  ::L_0884::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0822")
  do return end
end

script.L_088e = function(u, p)
  ::L_088e::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -124161.6), par(p, 2, 120006.24), 20, "L_089d")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_089d = function(u)
  ::L_089d::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0802")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_08ab end
  m688.messaggio_al_giocatore(u, 42)
  ::L_08ab::
  do return end
end

script.L_08ac = function(u, p)
  ::L_08ac::
  m688.vai_a(u, par(p, 1, -124161.6), par(p, 2, 120006.24), 30, "L_08b7")
  m688.salva_modello(u)
  do return end
end

script.L_08b7 = function(u)
  ::L_08b7::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_08be = function(u, p)
  ::L_08be::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08c3 = function(u, p)
  ::L_08c3::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_08ca = function(u, p)
  ::L_08ca::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_08d2 = function(u)
  ::L_08d2::
  m688.annulla_timer(u, "L_0911")
  m688.dopo(u, 20, "L_0911")
  do return end
end

script.L_08db = function(u)
  ::L_08db::
  m688.annulla_timer(u, "L_0914")
  m688.dopo(u, 20, "L_0914")
  do return end
end

script.L_08e4 = function(u)
  ::L_08e4::
  m688.annulla_timer(u, "L_0917")
  m688.dopo(u, 20, "L_0917")
  do return end
end

script.L_08ed = function(u)
  ::L_08ed::
  m688.annulla_timer(u, "L_091a")
  m688.dopo(u, 20, "L_091a")
  do return end
end

script.L_08f6 = function(u)
  ::L_08f6::
  m688.annulla_timer(u, "L_091d")
  m688.dopo(u, 20, "L_091d")
  do return end
end

script.L_08ff = function(u)
  ::L_08ff::
  m688.annulla_timer(u, "L_0920")
  m688.dopo(u, 20, "L_0920")
  do return end
end

script.L_0908 = function(u)
  ::L_0908::
  m688.annulla_timer(u, "L_0923")
  m688.dopo(u, 20, "L_0923")
  do return end
end

script.L_0911 = function(u)
  ::L_0911::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0914 = function(u)
  ::L_0914::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0917 = function(u)
  ::L_0917::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_091a = function(u)
  ::L_091a::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_091d = function(u)
  ::L_091d::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0920 = function(u)
  ::L_0920::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0923 = function(u)
  ::L_0923::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0926 = function(u)
  ::L_0926::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_092e = function(u, p)
  ::L_092e::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0932 = function(u)
  ::L_0932::
  m688.dopo(u, 60, "L_093c")
  do return end
end

script.L_0938 = function(u, p)
  ::L_0938::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_093c = function(u)
  ::L_093c::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0942 = function(u)
  ::L_0942::
  m688.dopo(u, 40, "L_0948")
  do return end
end

script.L_0948 = function(u)
  ::L_0948::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_094e = function(u)
  ::L_094e::
  m688.annulla_timer(u, "L_093c")
  m688.annulla_timer(u, "L_0948")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0958 = function(u, p)
  ::L_0958::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_095d = function(u, p)
  ::L_095d::
  m688.posizione(u, par(p, 1, -124161.6), par(p, 2, 120006.24))
  m688.salva_modello(u)
  do return end
end

return missione
