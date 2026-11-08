-- Missione di prova della taratura G7.1 (docs/16, dossier 22 §9.10): l'Alfa del giocatore a quota periscopica alza il
-- periscopio per 30 s a 3 km da una Perry in allarme (posto di combattimento). Atteso: colpito circa una volta su due.
-- Non e' una missione del gioco: la usa engine/strumenti/taratura.cpp (taratura artiglieria).
return { id = 'G7_1_PERRY', teatro = 'faroe', paesaggio_sonoro = false, lato = 'urss', data = '1989-06-15', ora_utc = 12,
  carriera = { situazione = 'guerra', difficolta = 3 },
  gestori_giocatore = { periscopio = 'PERI' },
  script = { PERI = function(u, p) m688.periscopio(u, p[1]); m688.salva_modello(u) end },
  taratura = { tipo = 'artiglieria', prova = 'periscopio', osservatore = 1, durata_s = 30 },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true,
      posizione = { x = 0, y = 0 }, quota_m = 6.096, rotta = 90, velocita_kn = 3, modello = { massima_kn = 41, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 8, sonar = 19 } },
    { id = 1, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 3000 }, quota_m = 0, rotta = 270,
      velocita_kn = 5, modello = { massima_kn = 29, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 12, sonar = 15 } },
  } } } }
