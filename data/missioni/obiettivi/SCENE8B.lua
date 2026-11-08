-- SCENE8B: Festa a sorpresa (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0).
-- Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_01b8 (ogni 20 s da 120 s): affondate tutte le unita' NATO 2..7 esito 5; giocatore fuori dall'area esito 7.
-- L_01b5 (arrivo di una delle navi 2..4 o 4000 s): esito 6.
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "successo",
                      [6] = "fallimento", [7] = "richiamato", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "nato", tipo = "affondare", unita = { 2, 3, 4, 5, 6, 7 }, minimo = 6,
      testo = { it = "Distruggere tutte le unità NATO dirette al Mar di Norvegia",
                en = "Destroy all NATO units bound for the Norwegian Sea" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
