-- Missione di prova della taratura M6 (docs/15): il 688 a quota periscopica a 5 kn col periscopio su, solo, nel
-- teatro e nel mese che strumenti/taratura.cpp sostituisce. Misura il mare, la testa sott'acqua e la copertura.
return { id = 'M6_QUOTA', teatro = 'faroe', paesaggio_sonoro = false, lato = 'usa', data = '1989-01-15', ora_utc = 0,
  gestori_giocatore = { periscopio = 'PERI' },
  script = { PERI = function(u, p) m688.periscopio(u, p[1]); m688.salva_modello(u) end },
  taratura = { tipo = 'meteo' },
  varianti = { { probabilita = 1, unita = {
    { id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,
      posizione = { x = 0, y = 0 }, quota_m = 6.096, rotta = 90, velocita_kn = 5, modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 2, quota_max_m = 600, integrita = 100, rumore = 4, sonar = 19 } },
  } } } }
