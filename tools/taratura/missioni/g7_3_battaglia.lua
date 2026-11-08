-- Missione di prova della taratura G7.3 (docs/16, dossier 22 §9.10): un Kara e una Perry si cannoneggiano a 7 km
-- (artiglieria fra navi); il 688 immerso a 100 m ascolta da lontano. La taratura sposta il 688 a 10, 20 e 40 km e
-- misura l'eccesso di segnale dei colpi al sonar sferico. Non e' una missione del gioco.
return { id = 'G7_3_BATTAGLIA', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', data = '1989-06-15', ora_utc = 12,
  carriera = { situazione = 'guerra', difficolta = 3 }, artiglieria = { fra_navi = true },
  script = {},
  taratura = { tipo = 'artiglieria', prova = 'battaglia', osservatore = 1, durata_s = 600 },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = 0, y = -20000 }, quota_m = 100, rotta = 90, velocita_kn = 3, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 } },
    { id = 1, classe = 'kara', categoria = 'nave', flag = 18, posizione = { x = 0, y = 3500 }, quota_m = 0, rotta = 90,
      velocita_kn = 12, modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 12, sonar = 15 } },
    { id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = -3500 }, quota_m = 0, rotta = 90,
      velocita_kn = 12, modello = { massima_kn = 29, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 12, sonar = 15 } },
  } } } }
