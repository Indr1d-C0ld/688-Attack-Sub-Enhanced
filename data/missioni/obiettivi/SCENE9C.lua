-- SCENE9C: Ritorno a casa a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles,
-- unita' 0; il giocatore 1 e' l'Alfa, unita' 1; l'Oscar e' l'unita' 10). Obiettivi dal briefing; condizioni
-- dagli script. L_0202/L_020d (arrivo del convoglio): esito 10 al giocatore 0 e 9 al giocatore 1; difetto
-- dell'originale: la 10 ha il testo sovietico e la 6 (testo americano) non e' mai emessa, quindi la 10 vale
-- come successo del giocatore 0. L_04d2 (ogni 20 s da 100 s): affondate tutte le navi 2..8 esito 5 (11);
-- giocatore 0 fuori dall'area esito 7. L_0518: giocatore 1 fuori, esito 8.
-- I codici 9, 11 e 15..18 sono del giocatore 1: categorie dal suo punto di vista.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "successo",
                      [5] = "fallimento", [6] = "successo", [7] = "fallimento", [8] = "successo",
                      [9] = "fallimento", [10] = "successo", [11] = "successo", [15] = "battello_perso",
                      [16] = "battello_perso", [17] = "successo", [18] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "biscaglia", tipo = "esito", codici = { 6, 10 },
      testo = { it = "Scortare il convoglio fino al Golfo di Biscaglia", en = "Escort the convoy to the Bay of Biscay" } },
    { id = "convoglio", tipo = "proteggere", unita = { 2, 3, 4, 5, 6, 7, 8 }, primario = false,
      testo = { it = "Proteggere le navi del convoglio", en = "Protect the convoy ships" } },
    { id = "sovietici", tipo = "affondare", unita = { 1, 10 }, minimo = 2, primario = false,
      testo = { it = "Sconfiggere i sottomarini sovietici attaccanti", en = "Defeat the attacking Soviet submarines" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
