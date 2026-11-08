-- SCENE0A: Colpisci e fuggi. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_02fb (300 s): esito 3 se la variabile 4 (Tomahawk ancora da lanciare, parte da 4, L_0724 la scala) non e' 0.
-- L_0311: nel rettangolo di Copenaghen esito 6 se manca una delle unita' 2..8, altrimenti 5; fuori dalla zona esito 4.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "fallimento", [4] = "fallimento",
                      [5] = "successo", [6] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "tomahawk", tipo = "variabile", indice = 4, uguale = 0,
      testo = { it = "Lanciare i quattro Tomahawk sugli obiettivi nell'entroterra entro cinque minuti",
                en = "Launch the four Tomahawks at the inland targets within five minutes" } },
    { id = "copenaghen", tipo = "esito", codici = { 5, 6 },
      testo = { it = "Riparare nelle acque di Copenaghen", en = "Withdraw to Copenhagen waters" } },
    -- l'esito 6 (Medal of Honor) richiede che manchi almeno una delle unita' 2..8
    { id = "flotta_baltico", tipo = "affondare", unita = { 2, 3, 4, 5, 6, 7, 8 }, minimo = 1, primario = false,
      testo = { it = "Affondare unità sovietiche durante il rientro", en = "Sink Soviet units on the way back" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
