-- Missione di prova della taratura G7.2 (docs/16, dossier 22 §9.10): il 688 in superficie a 8 km da un Kara in
-- condizione III; al primo colpo (con 10 s di reazione) l'immersione d'emergenza. Atteso: quasi sempre sotto prima dei
-- danni gravi. Non e' una missione del gioco: la usa engine/strumenti/taratura.cpp (taratura artiglieria).
return { id = 'G7_2_KARA', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', data = '1989-06-15', ora_utc = 12,
  carriera = { situazione = 'guerra', difficolta = 3 },
  script = {},
  taratura = { tipo = 'artiglieria', prova = 'emersione', osservatore = 1, durata_s = 900 },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 8, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 } },
    { id = 1, classe = 'kara', categoria = 'nave', flag = 18, posizione = { x = 0, y = 8000 }, quota_m = 0, rotta = 180,
      velocita_kn = 12, modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 12, sonar = 15 } },
  } } } }
