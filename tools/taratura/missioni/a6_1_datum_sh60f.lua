-- Missione di prova della taratura (docs/13 A6, docs/14 P5, docs/15 M6): la usa engine/strumenti/taratura.cpp.
-- Non e' una missione del gioco. «taratura» dice allo strumento chi e' l'aereo, chi il bersaglio e quanto dura.
return { id = 'A6_1_DATUM', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', data = '1989-10-15', ora_utc = 12,
  script = {},
  taratura = { tipo = 'asw', aereo = 1, bersaglio = 2, durata_s = 3600, sparpaglia_m = 2000, anticipo_s = 600 },
  aereo_asw = { { unita = 1, missione = 'caccia', situazione = 'guerra', zona = { x = 0, y = 0, raggio_m = 15000 }, cue = { tipo = 'siluro_sentito', x = 0, y = 0, raggio_m = 2000, ritardo_s = 600 } } },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = -300000, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 } },
    { id = 1, classe = 'sh60f', categoria = 'aereo', flag = 11, posizione = { x = 0, y = -15000 }, quota_m = 0, rotta = 0,
      velocita_kn = 200, modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 } },
    -- il Victor ha lanciato 10 min prima (il cue): fugge dal datum a velocita' dosata, come nel riferimento NPS (§9)
    { id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 150, rotta = 90,
      velocita_kn = 12, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 }, rotta_punti = { { x = 400000, y = 0 } } },
  } } } }
