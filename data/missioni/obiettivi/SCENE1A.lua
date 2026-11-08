-- SCENE1A: TORPEX '89. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_02e1 (900 s) e la scoperta del giocatore (L_029b, L_01f5): esito 8/7/6/5 con 0/1/2/3 cacciatorpediniere 2..4 a galla.
-- L_0329: esito 8 appena mancano 2, 3 e 4; esito 3 se manca l'Alfa (unita' 6) nelle varianti che l'hanno (variabile 3 = 1).
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "richiamato", [4] = "successo",
                      [5] = "fallimento", [6] = "successo_parziale", [7] = "successo_parziale", [8] = "successo",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "cacciatorpediniere", tipo = "affondare", unita = { 2, 3, 4 }, minimo = 3,
      testo = { it = "Affondare i tre cacciatorpediniere Forrest Sherman entro quindici minuti",
                en = "Sink the three Forrest Sherman destroyers within fifteen minutes" } },
    -- la contro-scoperta (traccia solida di una nave o del LAMPS) chiude l'esercitazione con il conto delle navi
    { id = "occulto", tipo = "non_essere_scoperto", primario = false,
      testo = { it = "Non farsi individuare dalle navi o dal LAMPS", en = "Avoid detection by the ships and the LAMPS" } },
    { id = "alfa", tipo = "proteggere", unita = { 6 }, primario = false, varianti = { 3, 4, 7, 9, 11, 12 },
      testo = { it = "Non attaccare il sottomarino sovietico in zona", en = "Do not attack the Soviet submarine in the area" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
