-- SCENE9A: Ritorno a casa. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_02a8 (arrivo di una nave del convoglio 2..8): esito 8. L_054e (ogni 20 s da 100 s): affondate tutte le navi
-- 2..8 esito 5; affondati l'Alfa e l'Oscar (unita' 9 e 10) esito 6; giocatore fuori dall'area esito 7.
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "fallimento",
                      [6] = "successo", [7] = "richiamato", [8] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "biscaglia", tipo = "esito", codici = { 6, 8 },
      testo = { it = "Scortare il convoglio fino al Golfo di Biscaglia", en = "Escort the convoy to the Bay of Biscay" } },
    { id = "convoglio", tipo = "proteggere", unita = { 2, 3, 4, 5, 6, 7, 8 }, primario = false,
      testo = { it = "Proteggere le navi del convoglio", en = "Protect the convoy ships" } },
    { id = "sovietici", tipo = "affondare", unita = { 9, 10 }, minimo = 2, primario = false,
      testo = { it = "Sconfiggere i sottomarini sovietici attaccanti", en = "Defeat the attacking Soviet submarines" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
