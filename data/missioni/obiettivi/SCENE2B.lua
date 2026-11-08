-- SCENE2B: Scrollatevelo di dosso (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0).
-- Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_025b: variabile 5 = 1 con il Los Angeles (unita' 2) come bersaglio, 2 con traccia solida ed entro 5 miglia.
-- L_02a3: contatto perso per 200 s, esito 5. L_02fe (1200 s): variabile 5 = 2 esito 6, = 1 esito 7, altrimenti 4.
-- L_02f0: esito 3 se manca il Los Angeles.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "richiamato", [4] = "fallimento",
                      [5] = "fallimento", [6] = "successo", [7] = "fallimento", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "pedinare", tipo = "variabile", indice = 5, uguale = 2,
      testo = { it = "Seguire il sottomarino americano entro 5 miglia, in posizione di tiro",
                en = "Trail the American submarine within 5 miles, in firing position" } },
    { id = "los_angeles", tipo = "proteggere", unita = { 2 }, primario = false,
      testo = { it = "Non lanciare siluri se non attaccati", en = "Do not launch torpedoes unless attacked" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
