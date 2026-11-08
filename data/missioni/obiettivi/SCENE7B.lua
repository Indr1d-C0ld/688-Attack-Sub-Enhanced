-- SCENE7B: Passerella (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0; la PLARB
-- Krasnogvardeets e' il Typhoon, unita' 2; la minaccia e' il Los Angeles, unita' 7). Obiettivi dal briefing;
-- condizioni dagli script (funzioni che danno gli esiti).
-- L_0466/L_0476: il Typhoon arriva al suo settore, esito 5. L_0300 (ogni 30 s da 120 s): senza il Typhoon
-- esito 6, senza il Los Angeles esito 7. Esito 19: distrutta una nave di superficie (unita' 3..5).
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "successo",
                      [6] = "fallimento", [7] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "typhoon", tipo = "proteggere", unita = { 2 },
      testo = { it = "Proteggere a ogni costo la PLARB Krasnogvardeets", en = "Protect the SSBN Krasnogvardeets at all costs" } },
    { id = "minaccia", tipo = "affondare", unita = { 7 }, minimo = 1, primario = false,
      testo = { it = "Distruggere le unità da guerra nemiche nella zona", en = "Destroy any threatening warship in the area" } },
    { id = "mercantili", tipo = "proteggere", unita = { 3, 4, 5 }, primario = false,
      testo = { it = "Non affondare le navi di superficie in zona", en = "Do not sink the surface ships in the area" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
