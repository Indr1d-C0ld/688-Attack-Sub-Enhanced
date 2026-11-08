-- SCENE6A: Gulasch. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_04ec (arrivo di un mercantile a Tivat, o affondate le unita' sovietiche 2, 8 e 9): conta le sovietiche
-- affondate (variabile 3): 0, 1, 2, 3 = esiti 4, 5, 8, 9.
-- L_01c6 (ogni 20 s da 200 s): affondati tutti i mercantili 3..7 esito 7; giocatore fuori dall'area esito 3.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "richiamato", [4] = "successo",
                      [5] = "successo", [7] = "fallimento", [8] = "successo", [9] = "successo",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "tivat", tipo = "esito", codici = { 4, 5, 8, 9 },
      testo = { it = "Scortare le navi da carico verso nord fino a Tivat", en = "Escort the cargo ships north to Tivat" } },
    { id = "carico", tipo = "proteggere", unita = { 3, 4, 5, 6, 7 }, primario = false,
      testo = { it = "Proteggere le navi da carico dall'aggressione sovietica", en = "Protect the cargo ships from Soviet aggression" } },
    { id = "sovietici", tipo = "affondare", unita = { 2, 8, 9 }, minimo = 3, primario = false,
      testo = { it = "Affondare le unità da guerra sovietiche", en = "Sink the Soviet warships" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
