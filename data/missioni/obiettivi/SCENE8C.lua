-- SCENE8C: Festa a sorpresa a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles,
-- unita' 0; il giocatore 1 e' l'Alfa, unita' 1). Obiettivi dal briefing; condizioni dagli script.
-- L_019b (arrivo di una delle navi 2..4): esito 5 (giocatore 1: 6). L_01be (ogni 20 s da 120 s): affondate
-- tutte le navi 2..6 esito 9 (10); giocatore 0 fuori dall'area esito 7. L_01a4: giocatore 1 fuori, esito 8.
-- I codici 6, 10 e 15..18 sono del giocatore 1: categorie dal suo punto di vista.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "successo",
                      [5] = "successo", [6] = "fallimento", [7] = "fallimento", [8] = "successo",
                      [9] = "fallimento", [10] = "successo", [15] = "battello_perso", [16] = "battello_perso",
                      [17] = "successo", [18] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "destinazione", tipo = "esito", codici = { 5 },
      testo = { it = "Portare a destinazione il gruppo di superficie", en = "Bring the surface group to its destination" } },
    { id = "gruppo", tipo = "proteggere", unita = { 2, 3, 4, 5, 6 }, primario = false,
      testo = { it = "Proteggere le navi di superficie da ogni danno", en = "Keep the surface ships from harm" } },
    { id = "alfa", tipo = "affondare", unita = { 1 }, minimo = 1, primario = false,
      testo = { it = "Eliminare la minaccia sottomarina al gruppo", en = "Eliminate the submarine threat to the group" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
