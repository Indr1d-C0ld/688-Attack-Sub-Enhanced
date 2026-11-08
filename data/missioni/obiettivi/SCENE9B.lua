-- SCENE9B: Ritorno a casa (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0; l'Oscar
-- Pskovskii Komsomolets e' l'unita' 10). Obiettivi dal briefing; condizioni dagli script.
-- L_061d (ogni 20 s da 100 s): affondate tutte le navi del convoglio 2..8 esito 5; giocatore fuori dall'area
-- esito 7. L_02a8 (arrivo di una nave del convoglio): esito 6.
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "successo",
                      [6] = "fallimento", [7] = "richiamato", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "convoglio", tipo = "affondare", unita = { 2, 3, 4, 5, 6, 7, 8 }, minimo = 7,
      testo = { it = "Distruggere tutto ciò che del convoglio resta a galla", en = "Destroy whatever is left afloat of the convoy" } },
    { id = "oscar", tipo = "proteggere", unita = { 10 }, primario = false,
      testo = { it = "Non colpire il Pskovskii Komsomolets", en = "Do not hit the Pskovskii Komsomolets" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
