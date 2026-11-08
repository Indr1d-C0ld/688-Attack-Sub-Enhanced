-- SCENE1A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE1A",
  missione = 1,
  titolo = { it = "TORPEX '89", en = "TORPEX '89" },
  lato = "usa",
  teatro = "faroe",
  posa = { metodo = "ricollocata", centro = { lat = 61.9192, lon = -6.9048 }, metri_per_unita = { x = 23.335, y = 23.335 }, moto_classico_m_s_per_nodo = 1.1216, affidabilita = "media", nota = "terre emerse della carta originale sulle coste reali (ricolloca_surf.py, Jaccard 0.32); traslazione e scala, senza rotazione" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 121.92, 304.8, 457.2 } },
  dotazione = { mk48 = 22, harpoon = 2, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_041a" },
    harpoon = { gestore = "L_03cc" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_04ab" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_050e",
    telegrafo = "L_0520",
    quota = "L_0525",
    timone = "L_052c",
    pilota_automatico_off = "L_0588",
    periscopio = "L_0590",
    carica_tubo_1 = "L_0534",
    carica_tubo_2 = "L_053d",
    carica_tubo_3 = "L_0546",
    carica_tubo_4 = "L_054f",
    carica_tubo_5 = "L_0558",
    carica_tubo_6 = "L_0561",
    carica_tubo_7 = "L_056a",
    schiera_cala = "L_0594",
    schiera_recupera = "L_05a4",
    schiera_persa = "L_05b0",
    designa_bersaglio = "L_05ba",
    posizione = "L_05bf",
    antenna = "L_059a",
    guida_e0 = "L_04e1",
    guida_e1 = "L_04e6",
    guida_e2 = "L_04f0",
  },
  copertura = 1,
  varianti = {
    { probabilita = 0.153741538525, probabilita_originale = "2579355/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 6493.44, y = -34388.48 }, originale = { x = 5184, y = 2112 },
        quota_m = 6.096, rotta = 170, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.15135794878, probabilita_originale = "2539365/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 6493.44, y = -34388.48 }, originale = { x = 5184, y = 2112 },
        quota_m = 6.096, rotta = 170, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.15135794878, probabilita_originale = "2539365/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 6493.44, y = -34388.48 }, originale = { x = 5184, y = 2112 },
        quota_m = 6.096, rotta = 170, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
    { probabilita = 0.149011313915, probabilita_originale = "2499995/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 6493.44, y = -34388.48 }, originale = { x = 5184, y = 2112 },
        quota_m = 6.096, rotta = 170, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
    { probabilita = 0.0505859255791, probabilita_originale = "848691/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 19934.4, y = -47082.72 }, originale = { x = 5760, y = 1568 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.0498016476631, probabilita_originale = "835533/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 19934.4, y = -47082.72 }, originale = { x = 5760, y = 1568 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.0498016476631, probabilita_originale = "835533/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 19934.4, y = -47082.72 }, originale = { x = 5760, y = 1568 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
    { probabilita = 0.0495940446854, probabilita_originale = "416025/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -51563.04 }, originale = { x = 5568, y = 1376 },
        quota_m = 6.096, rotta = 320, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.0490295290947, probabilita_originale = "822579/16777216", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 19934.4, y = -47082.72 }, originale = { x = 5760, y = 1568 },
        quota_m = 6.096, rotta = 270, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
    { probabilita = 0.0488251447678, probabilita_originale = "409575/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -51563.04 }, originale = { x = 5568, y = 1376 },
        quota_m = 6.096, rotta = 320, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 0 },
    },
    { probabilita = 0.0488251447678, probabilita_originale = "409575/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -51563.04 }, originale = { x = 5568, y = 1376 },
        quota_m = 6.096, rotta = 320, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = -18895.04, y = -44095.84 }, originale = { x = 4096, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_017a" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
    { probabilita = 0.0480681657791, probabilita_originale = "403225/8388608", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 15454.08, y = -51563.04 }, originale = { x = 5568, y = 1376 },
        quota_m = 6.096, rotta = 320, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 6, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = 12467.2, y = -39615.52 }, originale = { x = 5440, y = 1888 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 18, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 7240.16, y = -41855.68 }, originale = { x = 5216, y = 1792 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = 5000, y = -38700 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 3, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 9480.32, y = -44095.84 }, originale = { x = 5312, y = 1696 },
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 4, classe = "spruance", tipo_originale = 0, nome_originale = "FORREST SHERMAN CLASS DESTROYER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 12400, y = -45700 }, originale = { x = 5440, y = 1600 }, spostamento_m = 639.5403,
        quota_m = 0, rotta = 0, velocita_kn = 9.1429,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 457.2, integrita = 100, rumore = 12, sonar = 11, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 3 },
        rotta_punti = { { x = -18895.04, y = -14973.76 } }, rotta_ciclica = false,
        gestori = { bersaglio = "L_029b", reazione = nil, arrivo = "L_0175" } },
      { id = 5, classe = "sh60b", tipo_originale = 18, nome_originale = "SEAHAWK-60B LAMPS III HELICOPTOR", categoria = "aereo", schieramento = "urss", flag = 19, tipo = 2,
        posizione = { x = 41589.28, y = -30654.88 }, originale = { x = 6688, y = 2272 },
        quota_m = 0, rotta = 0, velocita_kn = 82.2857,
        modello = { massima_kn = 90.0, virata_gradi_s = 9.0, accel_kn_s = 0.771, quota_m_s = 0.3353, quota_max_m = 457.2, integrita = 10, rumore = 0, sonar = 25, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 2 },
        rotta_punti = { { x = -18895.04, y = -44095.84 }, { x = 41589.28, y = -29908.16 }, { x = -18895.04, y = -44095.84 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0190", reazione = nil, arrivo = "L_0185" } },
      { id = 6, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 2759.84, y = -32895.04 }, originale = { x = 5024, y = 2176 },
        quota_m = 62, quota_originale_m = 121.92, rotta = 180, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 8, sonar = 18, difesa = 0, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = "L_0214", reazione = nil, arrivo = nil } },
      },
      timer = { { dopo_s = 240, unita = 0, gestore = "L_031b" }, { dopo_s = 200, unita = 0, gestore = "L_0329" }, { dopo_s = 900, unita = 0, gestore = "L_02e1" }, { dopo_s = 30, unita = 2, gestore = "L_0294" }, { dopo_s = 60, unita = 3, gestore = "L_0294" }, { dopo_s = 90, unita = 4, gestore = "L_0294" }, { dopo_s = 160, unita = 5, gestore = "L_0294" }, { dopo_s = 30, unita = 6, gestore = "L_0294" } },
      variabili = { [1] = 0, [3] = 1 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "OGG.: TORPEX NORD ATLANTICO", "Benvenuto nell'Atlantico del Nord, Comandante. Lei darà inizio a un'esercitazione al largo della costa dell'isola più meridionale dell'arcipelago delle Fær Øer. Tre cacciatorpediniere radiati della classe Forrest Sherman stanno attraversando l'arcipelago diretti a nord-ovest. Sono stati dotati di piloti automatici telecomandati, che consentono loro alcune semplici manovre durante l'esercitazione; anche radar e sonar delle navi sono comandati a distanza. Le navi di superficie e un elicottero LAMPS III presente in zona cercheranno di individuare il suo battello: a quel punto l'esercitazione terminerà. Il sistema di sorveglianza sonar subacquea (SOSUS) seguirà l'esercitazione da terra e farà da arbitro: chiuderà l'esercitazione con tre impulsi sonar se una delle navi o il LAMPS avranno individuato la sua posizione. L'esercitazione terminerà comunque dopo quindici minuti.", "La sua missione è distruggere tutte le unità da guerra entro il tempo assegnato. Potrebbe essere un compito arduo. Siluri attivi in acqua, o il troppo rumore delle manovre ad alta velocità, possono rivelare la sua posizione al sonar avversario, e un periscopio alzato la espone all'individuazione radar. Decida lei come avvicinarsi alle navi, ma resti profondo e silenzioso se vuole avere una possibilità.", "Il poligono è libero e le armi sono libere. Faccia vedere a quelli della superficie e ai piloti cosa significa davvero silenziosi e letali! Buona fortuna, Comandante." },
    en = { "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "SUBJ: TORPEX NLANT", "WELCOME TO THE NORTH ATLANTIC CAPTAIN. YOU ARE TO BEGIN A TRAINING EXERCISE OFF THE COAST OF THE SOUTHERN MOST ISLAND IN THE FAEROE ISLAND CHAIN. THERE ARE THREE DECOMMISSIONED FORREST SHERMAN CLASS DESTROYERS MAKING THEIR WAY NORTHWEST THROUGH THE ISLAND CHAIN. THEY HAVE BEEN CONFIGURED WITH REMOTE AUTO PILOT SYSTEMS TO ALLOW THEM SOME SIMPLE MANEUVERING CAPABILITIES DURING THE EXERCISE. RADAR AND SONAR ON THE SHIPS IS ALSO CONTROLLED REMOTELY. THE SURFACE SHIPS AND A LAMPS III HELICOPTER IN THE AREA WILL ATTEMPT TO IDENTIFY YOUR BOAT, AT WHICH POINT THE EXERCISE WILL END. THE UNDERWATER SONAR SURVEILLANCE SYSTEM (SOSUS) WILL BE MONITORING THE EXERCISE ASHORE AND ACT AS REFEREE. IT WILL END THE EXERCISE WITH THREE SONAR BLASTS IF ONE OF THE SHIPS OR THE LAMPS HAVE IDENTIFIED YOUR POSITION. THE EXERCISE WILL ALSO END AFTER FIFTEEN MINUTES.", "YOUR MISSION IS TO DESTROY EACH OF THE WARSHIPS WITHIN THE ALLOTTED TIME. IT MAY BE A TOUGH JOB. ACTIVE TORPEDOS IN THE WATER OR TOO MUCH NOISE CREATED BY HIGH SPEED MANEUVERING MAY ALERT YOUR OPPONENTS TO YOUR LOCATION BY SONAR AND A RAISED PERISCOPE WILL LEAVE YOU SUSCEPTIBLE TO IDENTIFICATION BY RADAR. YOU DECIDE HOW YOU APPROACH THE WARSHIPS BUT STAY LOW AND QUIET IF YOU WANT TO STAND A CHANCE.", "THE WEAPONS RANGE IS CLEAR AND WEAPONS ARE FREE. SHOW THOSE SKIMMERS AND FLY BOYS WHAT SILENT AND DEADLY REALLY MEANS! GOOD LUCK CAPTAIN." },
  },
  esiti = {
    [1] = { it = "CODICE: 1-1-ACP\nIeri l'USS Los Angeles si è incagliato ed è andato distrutto durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia commesso un grave errore di valutazione, causa della tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 1-1-ACP\nTHE USS LOS ANGELES RAN AGROUND AND WAS DESTROYED YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN MADE A SEVERE ERROR IN JUDGEMENT WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 1-2-ACP\nIeri l'USS Los Angeles è stato distrutto da siluri durante un'esercitazione condotta al largo delle isole Fær Øer. Si ritiene che il comandante abbia provocato un sottomarino sovietico presente in zona, causando la tragedia. Unitevi a me nell'esprimere il mio cordoglio alle famiglie e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 1-2-ACP\nTHE USS LOS ANGELES WAS DESTROYED BY TORPEDO FIRE YESTERDAY DURING AN EXERCISE CONDUCTED OFF THE COAST OF THE FAEROE ISLANDS. IT IS BELIEVED THAT THE CAPTAIN PROVOKED A SOVIET SUB IN THE AREA WHICH RESULTED IN THE TRAGEDY. JOIN ME IN EXPRESSING MY GRIEF FOR THE FAMILIES AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [3] = { it = "CODICE: 1-3-ACP\nIl Los Angeles rientri in porto, e il comandante si presenti immediatamente nel mio ufficio. Cosa stava cercando di fare là fuori, scatenare la terza guerra mondiale? Si porti dietro la sua roba, perché a bordo non le servirà più quando avrò finito con lei.", en = "CODE: 1-3-ACP\nTHE LOS ANGELES IS TO RETURN TO PORT, AND THE CAPTAIN IS TO REPORT TO MY OFFICE IMMEDIATELY. WHAT YOU WERE TRYING TO DO OUT THERE, START WORLD WAR THREE? BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WON'T NEED IT ON BOARD WHEN I'M THROUGH WITH YOU." },
    [5] = { it = "CODICE: 1-5-ACP\nIl Los Angeles rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Comandante, lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete la vostra fiacca prestazione al poligono (nessuna nave nemmeno danneggiata!). Si porti dietro la sua roba, perché per il prossimo mese a bordo non le servirà.", en = "CODE: 1-5-ACP\nTHE LOS ANGELES IS TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME YOUR LACKLUSTER (NO SHIPS DAMAGED AT ALL!) PERFORMANCE AT THE WEAPONS RANGE. BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WON'T NEED IT ON BOARD FOR THE NEXT MONTH." },
    [6] = { it = "CODICE: 1-6-ACP\nAffondare una sola nave durante l'esercitazione è considerato insufficiente. Vedo che lei e i suoi ufficiali avete bisogno di ulteriore addestramento prima che vi affidiamo a tempo pieno un sottomarino da un miliardo di dollari. Si presenti nel mio ufficio al rientro in porto.", en = "CODE: 1-6-ACP\nSINKING JUST ONE SHIP DURING THE TRAINING EXERCISE IS CONSIDERED SUBSTANDARD. I CAN SEE THAT YOU AND YOUR OFFICERS ARE IN NEED OF FURTHER TRAINING BEFORE WE GIVE YOU FULL-TIME CONTROL OF A BILLION DOLLAR SUBMARINE. REPORT TO MY OFFICE UPON YOUR RETURN TO PORT." },
    [7] = { it = "CODICE: 1-7-ACP\nCon due navi affondate, la prestazione del Los Angeles durante l'esercitazione è stata giudicata soddisfacente. Rientri in porto con un rapporto dettagliato delle sue azioni al poligono. La prossima settimana fisserò un incontro e analizzeremo insieme l'esercitazione.", en = "CODE: 1-7-ACP\nAFTER SINKING TWO SHIPS, THE PERFORMANCE OF THE LOS ANGELES DURING THE TRAINING EXERCISE WAS JUDGED SATISFACTORY. YOU ARE TO RETURN TO PORT WITH A REPORT DETAILING YOUR ACTIONS ON THE WEAPONS RANGE. I WILL ARRANGE A MEETING WITH YOU NEXT WEEK AND WE WILL GO OVER THE EXERCISE." },
    [8] = { it = "CODICE: 1-8-ACP\nPrestazione eccezionale oggi al poligono, Comandante. È rassicurante sapere che i nostri sottomarini sono in mani capaci. Porga la mia ammirazione al suo equipaggio. Veda se riesce a strappare sette giorni di licenza per i suoi ottimi uomini.", en = "CODE: 1-8-ACP\nOUTSTANDING PERFORMANCE ON THE RANGE TODAY, CAPTAIN. IT'S REASSURING TO KNOW THAT OUR SUBMARINES ARE IN CAPABLE HANDS. PLEASE EXPRESS MY ADMIRATION FOR YOUR CREW. SEE IF YOU CAN SELL SEVEN DAYS OF LEAVE TO YOUR FINE CREW." },
    [19] = { it = "CODICE: 1-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 1-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 12, punti = 12, spostamento_max_m = 639.5403, non_risolte = 0 },
  script = script,
}

script.L_0175 = function(u)
  ::L_0175::
  m688.telegrafo(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_017a = function(u)
  ::L_017a::
  m688.vai_a(u, -18895.04, -44095.84, 30, "L_0185")
  m688.salva_modello(u)
  do return end
end

script.L_0185 = function(u)
  ::L_0185::
  m688.vai_a(u, 41589.28, -29908.16, 30, "L_017a")
  m688.salva_modello(u)
  do return end
end

script.L_0190 = function(u)
  ::L_0190::
  if m688.bersaglio_id(u, 2) then goto L_01a4 end
  if m688.bersaglio_id(u, 3) then goto L_01ac end
  if m688.bersaglio_id(u, 4) then goto L_01b4 end
  ::L_019f::
  m688.comportamento(u, "L_0190")
  m688.salva_modello(u)
  do return end
  ::L_01a4::
  if m688.unita_colpita(u, 2) then goto L_01bc end
  goto L_019f
  ::L_01ac::
  if m688.unita_colpita(u, 3) then goto L_01bc end
  goto L_019f
  ::L_01b4::
  if m688.unita_colpita(u, 4) then goto L_01bc end
  goto L_019f
  ::L_01bc::
  m688.annulla_timer(u, "L_0294")
  m688.vai_al_bersaglio(u, 60, "L_01de")
  m688.salva_modello(u)
  do return end
end

script.L_01de = function(u)
  ::L_01de::
  m688.modello_sonar(u, 20)
  m688.rotta(u, nil)
  m688.rateo_virata(u, -1.8)
  m688.comportamento(u, "L_01f5")
  m688.salva_modello(u)
  m688.dopo(u, 20, "L_0294")
  m688.dopo(u, 120, "L_01c6")
  do return end
end

script.L_01c6 = function(u)
  ::L_0185::
  m688.vai_a(u, 41589.28, -29908.16, 30, "L_017a")
  m688.salva_modello(u)
  do return end
  ::L_01c6::
  m688.annulla_timer(u, "L_0294")
  m688.comportamento(u, "L_0190")
  m688.dopo(u, 36, "L_0294")
  m688.modello_sonar(u, 0)
  script.L_017a(u)
  if m688.caso(u, 0.5039) then goto L_01dd end
  goto L_0185
  ::L_01dd::
  do return end
end

script.L_01f5 = function(u)
  ::L_01f5::
  if m688.bersaglio_id(u, 0) then goto L_0205 end
  m688.rotta(u, nil)
  m688.rateo_virata(u, -3.6)
  m688.comportamento(u, "L_01f5")
  m688.salva_modello(u)
  do return end
  ::L_0205::
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0781)
  if m688.traccia_solida(u) then goto L_02d5 end
  m688.ingaggia(u, 30, "L_0205", "L_01fa")
  m688.salva_modello(u)
  do return end
  ::L_02d5::
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 32)
  goto L_02e3
  ::L_02e3::
  m688.flag_scena(u, 0)
  if m688.unita_esiste(u, 2) then goto L_030c end
  ::L_02ea::
  if m688.unita_esiste(u, 3) then goto L_0311 end
  ::L_02ef::
  if m688.unita_esiste(u, 4) then goto L_0316 end
  ::L_02f4::
  if m688.flag_scena_uguale(u, 1) then goto L_0303 end
  if m688.flag_scena_uguale(u, 2) then goto L_0306 end
  if m688.flag_scena_uguale(u, 3) then goto L_0309 end
  m688.messaggio(u, 108)
  do return end
  ::L_0303::
  m688.messaggio(u, 107)
  do return end
  ::L_0306::
  m688.messaggio(u, 106)
  do return end
  ::L_0309::
  m688.messaggio(u, 105)
  do return end
  ::L_030c::
  m688.flag_scena_piu(u)
  goto L_02ea
  ::L_0311::
  m688.flag_scena_piu(u)
  goto L_02ef
  ::L_0316::
  m688.flag_scena_piu(u)
  goto L_02f4
end

script.L_01fa = function(u)
  ::L_01fa::
  m688.rotta(u, nil)
  m688.rateo_virata(u, -3.6)
  m688.comportamento(u, "L_01f5")
  m688.salva_modello(u)
  do return end
end

script.L_0205 = function(u)
  ::L_0205::
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0781)
  if m688.traccia_solida(u) then goto L_02d5 end
  m688.ingaggia(u, 30, "L_0205", "L_01fa")
  m688.salva_modello(u)
  do return end
  ::L_02d5::
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 32)
  goto L_02e3
  ::L_02e3::
  m688.flag_scena(u, 0)
  if m688.unita_esiste(u, 2) then goto L_030c end
  ::L_02ea::
  if m688.unita_esiste(u, 3) then goto L_0311 end
  ::L_02ef::
  if m688.unita_esiste(u, 4) then goto L_0316 end
  ::L_02f4::
  if m688.flag_scena_uguale(u, 1) then goto L_0303 end
  if m688.flag_scena_uguale(u, 2) then goto L_0306 end
  if m688.flag_scena_uguale(u, 3) then goto L_0309 end
  m688.messaggio(u, 108)
  do return end
  ::L_0303::
  m688.messaggio(u, 107)
  do return end
  ::L_0306::
  m688.messaggio(u, 106)
  do return end
  ::L_0309::
  m688.messaggio(u, 105)
  do return end
  ::L_030c::
  m688.flag_scena_piu(u)
  goto L_02ea
  ::L_0311::
  m688.flag_scena_piu(u)
  goto L_02ef
  ::L_0316::
  m688.flag_scena_piu(u)
  goto L_02f4
end

script.L_0214 = function(u)
  ::L_0214::
  if m688.bersaglio_id(u, 0) then goto L_0224 end
  if m688.bersaglio_arma(u) then goto L_0243 end
  ::L_021c::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0214")
  m688.salva_modello(u)
  do return end
  ::L_0224::
  if m688.traccia_solida(u) then goto L_0233 end
  m688.telegrafo(u, 0.0391)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 30, "L_0214", "L_021c")
  m688.salva_modello(u)
  do return end
  ::L_0233::
  if m688.bersaglio_vicino(u, 8) then goto L_026d end
  ::L_0238::
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 30, "L_0214", "L_021c")
  m688.salva_modello(u)
  do return end
  ::L_0243::
  if m688.bersaglio_mi_punta(u) then goto L_0249 end
  goto L_021c
  ::L_0249::
  m688.virata_relativa(u, -180)
  m688.telegrafo(u, 1)
  m688.schiera_trainata(u, 0)
  m688.comportamento(u, "L_0214")
  m688.salva_modello(u)
  m688.dopo(u, 60, "L_025b")
  do return end
  ::L_0262::
  m688.telegrafo(u, 0)
  m688.ingaggia(u, 30, "L_0214", "L_021c")
  m688.salva_modello(u)
  do return end
  ::L_026d::
  if m688.flag_scena_uguale(u, 1) then goto L_0262 end
  if m688.variabile_uguale(u, 1, 0) then goto L_0238 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 240, "L_0290")
  m688.dopo(u, 2, "L_03d9")
  m688.dopo(u, 2, "L_03d9")
  m688.dopo(u, 2, "L_03d9")
  do return end
end

script.L_021c = function(u)
  ::L_021c::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0214")
  m688.salva_modello(u)
  do return end
end

script.L_025b = function(u)
  ::L_025b::
  m688.schiera_trainata(u, 1)
  m688.telegrafo(u, 0.5)
  m688.salva_modello(u)
  do return end
end

script.L_0290 = function(u)
  ::L_0290::
  m688.flag_scena(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_0294 = function(u)
  ::L_0294::
  m688.marca_contatti(u)
  m688.dopo(u, 24, "L_0294")
  do return end
end

script.L_029b = function(u)
  ::L_029b::
  if m688.bersaglio_arma(u) then goto L_02a8 end
  if m688.bersaglio_id(u, 0) then goto L_02b5 end
  ::L_02a3::
  m688.comportamento(u, "L_029b")
  m688.salva_modello(u)
  do return end
  ::L_02a8::
  m688.telegrafo(u, 1.0547)
  goto L_02a3
  ::L_02b5::
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.annulla_timer(u, "L_02af")
  m688.dopo(u, 40, "L_02af")
  if m688.traccia_solida(u) then goto L_02ca end
  m688.comportamento(u, "L_029b")
  m688.salva_modello(u)
  do return end
  ::L_02ca::
  if m688.bersaglio_vicino(u, 1.8) then goto L_02d5 end
  if m688.traccia_lunga(u) then goto L_02d5 end
  goto L_02a3
  ::L_02d5::
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 36)
  m688.messaggio(u, 32)
  goto L_02e3
  ::L_02e3::
  m688.flag_scena(u, 0)
  if m688.unita_esiste(u, 2) then goto L_030c end
  ::L_02ea::
  if m688.unita_esiste(u, 3) then goto L_0311 end
  ::L_02ef::
  if m688.unita_esiste(u, 4) then goto L_0316 end
  ::L_02f4::
  if m688.flag_scena_uguale(u, 1) then goto L_0303 end
  if m688.flag_scena_uguale(u, 2) then goto L_0306 end
  if m688.flag_scena_uguale(u, 3) then goto L_0309 end
  m688.messaggio(u, 108)
  do return end
  ::L_0303::
  m688.messaggio(u, 107)
  do return end
  ::L_0306::
  m688.messaggio(u, 106)
  do return end
  ::L_0309::
  m688.messaggio(u, 105)
  do return end
  ::L_030c::
  m688.flag_scena_piu(u)
  goto L_02ea
  ::L_0311::
  m688.flag_scena_piu(u)
  goto L_02ef
  ::L_0316::
  m688.flag_scena_piu(u)
  goto L_02f4
end

script.L_02af = function(u)
  ::L_02af::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.5)
  m688.salva_modello(u)
  do return end
end

script.L_02e1 = function(u)
  ::L_02e1::
  m688.messaggio_al_giocatore(u, 34)
  m688.flag_scena(u, 0)
  if m688.unita_esiste(u, 2) then goto L_030c end
  ::L_02ea::
  if m688.unita_esiste(u, 3) then goto L_0311 end
  ::L_02ef::
  if m688.unita_esiste(u, 4) then goto L_0316 end
  ::L_02f4::
  if m688.flag_scena_uguale(u, 1) then goto L_0303 end
  if m688.flag_scena_uguale(u, 2) then goto L_0306 end
  if m688.flag_scena_uguale(u, 3) then goto L_0309 end
  m688.messaggio(u, 108)
  do return end
  ::L_0303::
  m688.messaggio(u, 107)
  do return end
  ::L_0306::
  m688.messaggio(u, 106)
  do return end
  ::L_0309::
  m688.messaggio(u, 105)
  do return end
  ::L_030c::
  m688.flag_scena_piu(u)
  goto L_02ea
  ::L_0311::
  m688.flag_scena_piu(u)
  goto L_02ef
  ::L_0316::
  m688.flag_scena_piu(u)
  goto L_02f4
end

script.L_031b = function(u)
  ::L_031b::
  if m688.manomesso(u) then goto L_031f end
  do return end
  ::L_031f::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_0329 = function(u)
  ::L_0329::
  if m688.variabile_uguale(u, 3, 0) then goto L_0338 end
  if m688.unita_esiste(u, 6) then goto L_0338 end
  m688.messaggio(u, 103)
  do return end
  ::L_0338::
  if m688.unita_esiste(u, 2) then goto L_034a end
  if m688.unita_esiste(u, 3) then goto L_034a end
  if m688.unita_esiste(u, 4) then goto L_034a end
  m688.messaggio(u, 108)
  do return end
  ::L_034a::
  m688.dopo(u, 10, "L_0329")
  do return end
end

script.L_03cc = function(u)
  ::L_03cc::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_03d6")
  do return end
end

script.L_03d6 = function(u)
  ::L_03d6::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_03d9 = function(u)
  ::L_03d9::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_04a7", "L_0484")
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
  m688.dopo(u, 60, "L_0467")
  m688.dopo(u, 240, "L_04a7")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_041a = function(u)
  ::L_041a::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_04a7", "L_0484")
  script.L_0441(u)
  m688.dopo(u, 170, "L_043e")
  m688.dopo(u, 190, "L_0467")
  m688.dopo(u, 280, "L_04a7")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_043e = function(u)
  ::L_043e::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_0441 = function(u)
  ::L_0441::
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

script.L_0467 = function(u)
  ::L_0467::
  m688.annulla_timer(u, "L_0467")
  m688.collega(u, 0)
  m688.modello_sonar(u, 16)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_0484 = function(u)
  ::L_0484::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_0472")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_04a0")
  m688.dopo(u, 10, "L_0498")
  do return end
end

script.L_0472 = function(u)
  ::L_0472::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_0498")
  m688.annulla_timer(u, "L_0484")
  m688.insegui(u, 0, "L_04a7", "L_0484")
  m688.salva_modello(u)
  do return end
end

script.L_0498 = function(u)
  ::L_0498::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_04a0 = function(u)
  ::L_04a0::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_04a0")
  do return end
end

script.L_04a7 = function(u)
  ::L_04a7::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_04ab = function(u)
  ::L_04ab::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_04a7", "L_0484")
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
  m688.dopo(u, 2, "L_04d8")
  m688.dopo(u, 160, "L_04dd")
  do return end
end

script.L_04d8 = function(u)
  ::L_04d8::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_04dd = function(u)
  ::L_04dd::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_04e1 = function(u)
  ::L_04e1::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_04e6 = function(u)
  ::L_04e6::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0484")
  do return end
end

script.L_04f0 = function(u, p)
  ::L_04f0::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 20, "L_04ff")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_04ff = function(u)
  ::L_04ff::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_0472")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_050d end
  m688.messaggio_al_giocatore(u, 42)
  ::L_050d::
  do return end
end

script.L_050e = function(u, p)
  ::L_050e::
  m688.vai_a(u, par(p, 1, -114475.2), par(p, 2, 64925.28), 30, "L_0519")
  m688.salva_modello(u)
  do return end
end

script.L_0519 = function(u)
  ::L_0519::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0520 = function(u, p)
  ::L_0520::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0525 = function(u, p)
  ::L_0525::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_052c = function(u, p)
  ::L_052c::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0534 = function(u)
  ::L_0534::
  m688.annulla_timer(u, "L_0573")
  m688.dopo(u, 20, "L_0573")
  do return end
end

script.L_053d = function(u)
  ::L_053d::
  m688.annulla_timer(u, "L_0576")
  m688.dopo(u, 20, "L_0576")
  do return end
end

script.L_0546 = function(u)
  ::L_0546::
  m688.annulla_timer(u, "L_0579")
  m688.dopo(u, 20, "L_0579")
  do return end
end

script.L_054f = function(u)
  ::L_054f::
  m688.annulla_timer(u, "L_057c")
  m688.dopo(u, 20, "L_057c")
  do return end
end

script.L_0558 = function(u)
  ::L_0558::
  m688.annulla_timer(u, "L_057f")
  m688.dopo(u, 20, "L_057f")
  do return end
end

script.L_0561 = function(u)
  ::L_0561::
  m688.annulla_timer(u, "L_0582")
  m688.dopo(u, 20, "L_0582")
  do return end
end

script.L_056a = function(u)
  ::L_056a::
  m688.annulla_timer(u, "L_0585")
  m688.dopo(u, 20, "L_0585")
  do return end
end

script.L_0573 = function(u)
  ::L_0573::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_0576 = function(u)
  ::L_0576::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_0579 = function(u)
  ::L_0579::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_057c = function(u)
  ::L_057c::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_057f = function(u)
  ::L_057f::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0582 = function(u)
  ::L_0582::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_0585 = function(u)
  ::L_0585::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_0588 = function(u)
  ::L_0588::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0590 = function(u, p)
  ::L_0590::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0594 = function(u)
  ::L_0594::
  m688.dopo(u, 60, "L_059e")
  do return end
end

script.L_059a = function(u, p)
  ::L_059a::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_059e = function(u)
  ::L_059e::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_05a4 = function(u)
  ::L_05a4::
  m688.dopo(u, 40, "L_05aa")
  do return end
end

script.L_05aa = function(u)
  ::L_05aa::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_05b0 = function(u)
  ::L_05b0::
  m688.annulla_timer(u, "L_059e")
  m688.annulla_timer(u, "L_05aa")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_05ba = function(u, p)
  ::L_05ba::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_05bf = function(u, p)
  ::L_05bf::
  m688.posizione(u, par(p, 1, -114475.2), par(p, 2, 64925.28))
  m688.salva_modello(u)
  do return end
end

return missione
