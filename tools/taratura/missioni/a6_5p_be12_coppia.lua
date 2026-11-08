-- Missione di prova della taratura (docs/13 A6, docs/14 P5, docs/15 M6): la usa engine/strumenti/taratura.cpp.
-- Nel Barents, sul banco a (130, -55) km (92-150 m): nel Baltico (35-45 m) l'AT-1 non si sgancia (fondale >= 60 m,
-- dossier 17 §6.7). Il Foxtrot va a ovest e resta sul banco. Be-12 in coppia, come sempre (dossier 17 §3.3): il campo di
-- 60-70 boe del §9 e' quello di due aerei.
-- Non e' una missione del gioco. «taratura» dice allo strumento chi e' l'aereo, chi il bersaglio e quanto dura.
return { id = 'A6_5P_BE12', teatro = 'barents', paesaggio_sonoro = false, lato = 'usa', data = '1989-10-15', ora_utc = 12,
  script = {},
  taratura = { tipo = 'asw', aereo = 1, compagno = 3, bersaglio = 2, durata_s = 14400 },
  aereo_asw = { { unita = 1, missione = 'caccia', situazione = 'guerra', zona = { x = 130000, y = -55000, raggio_m = 40000 } },
                { unita = 3, missione = 'caccia', situazione = 'guerra', zona = { x = 130000, y = -55000, raggio_m = 40000 } } },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = -300000, y = 0 }, quota_m = 100, rotta = 90, velocita_kn = 5, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 } },
    { id = 1, classe = 'be12', categoria = 'aereo', flag = 19, posizione = { x = 130000, y = -85000 }, quota_m = 0, rotta = 0,
      velocita_kn = 200, modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 } },
    { id = 3, classe = 'be12', categoria = 'aereo', flag = 19, posizione = { x = 125000, y = -86000 }, quota_m = 0, rotta = 0,
      velocita_kn = 200, modello = { massima_kn = 330, virata_gradi_s = 3, accel_kn_s = 2, quota_m_s = 1, quota_max_m = 500, integrita = 10, rumore = 0, sonar = 20 } },
    { id = 2, classe = 'foxtrot', categoria = 'sottomarino', flag = 9, posizione = { x = 130000, y = -55000 }, quota_m = 60, rotta = 270,
      velocita_kn = 4, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 }, rotta_punti = { { x = -400000, y = -55000 } } },
  } } } }
