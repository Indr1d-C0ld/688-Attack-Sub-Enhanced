-- SCENE2C: Scrollatevelo di dosso a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles,
-- unita' 0; il giocatore 1 e' l'Alfa, unita' 1). Obiettivi dal briefing; condizioni dagli script.
-- L_00df/L_0108: variabile 5 = 1 con il Los Angeles come bersaglio dell'Alfa, 2 con traccia solida entro 5 miglia;
-- contatto perso per 200 s, L_0117: esito 5 (giocatore 1: 6). L_0120 (1200 s): variabile 5 = 2 esito 7 (8),
-- = 1 esito 9 (10), altrimenti 5 (6). I codici 6, 8, 10 e 15..18 sono del giocatore 1: categorie dal suo punto di vista.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "richiamato",
                      [5] = "successo", [6] = "fallimento", [7] = "fallimento", [8] = "successo",
                      [9] = "fallimento", [10] = "fallimento", [15] = "battello_perso", [16] = "battello_perso",
                      [17] = "successo", [18] = "richiamato", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "seminare", tipo = "esito", codici = { 5 },
      testo = { it = "Far perdere le tracce all'Alfa sovietico", en = "Shake off the trailing Soviet Alfa" } },
    { id = "alfa", tipo = "proteggere", unita = { 1 }, primario = false,
      testo = { it = "Non lanciare armi se i russi non fanno sul serio", en = "Do not launch weapons unless the Russians get serious" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
