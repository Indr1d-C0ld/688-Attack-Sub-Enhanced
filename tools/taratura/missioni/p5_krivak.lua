-- Missione di prova della taratura P5 (docs/14): il 688 a quota periscopica e una Krivak II col radar acceso.
-- strumenti/taratura.cpp sposta la fregata, cambia mare, ora, velocita' e periscopio e misura le scoperte.
return { id = 'P5_KRIVAK', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', data = '1989-10-15', ora_utc = 12,
  gestori_giocatore = { periscopio = 'PERI' },
  script = { PERI = function(u, p) m688.periscopio(u, p[1]); m688.salva_modello(u) end },
  taratura = { tipo = 'periscopi', osservatore = 1 },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = 0, y = 0 }, quota_m = 6.096, rotta = 90, velocita_kn = 4, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 19 } },
    { id = 1, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 6000 }, quota_m = 0, rotta = 270,
      velocita_kn = 0, modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 1, quota_m_s = 0, quota_max_m = 0, integrita = 100, rumore = 12, sonar = 15 } },
  } } } }
