-- SCENE8A: Festa a sorpresa. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_01bf (arrivo di una delle navi 2..4 o 4000 s): conta le navi del gruppo 2..6 affondate (variabile 3):
-- piu' di 2 esito 3, altrimenti 4. L_01f9 (ogni 20 s da 120 s): senza l'Alfa (unita' 7) esito 6; affondate
-- tutte le navi 2..6 esito 5; giocatore fuori dall'area esito 7.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "fallimento", [4] = "successo",
                      [5] = "fallimento", [6] = "successo", [7] = "richiamato", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "destinazione", tipo = "esito", codici = { 4, 6 },
      testo = { it = "Portare a destinazione il gruppo di superficie", en = "Bring the surface group to its destination" } },
    { id = "gruppo", tipo = "proteggere", unita = { 2, 3, 4, 5, 6 }, primario = false,
      testo = { it = "Proteggere le navi di superficie da ogni danno", en = "Keep the surface ships from harm" } },
    { id = "alfa", tipo = "affondare", unita = { 7 }, minimo = 1, primario = false,
      testo = { it = "Eliminare la minaccia sottomarina al gruppo", en = "Eliminate the submarine threat to the group" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
