-- SCENE2A: Scrollatevelo di dosso. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_01d8: l'Alfa (unita' 2) perde il contatto e non lo riprende per 200 s: esito 5 (L_01ea).
-- L_0245 (1200 s): esito 6 se la variabile 5 vale 1 (Alfa con traccia solida ma oltre 9 unita'), altrimenti 4.
-- L_022f: esito 7 se manca l'Alfa o il Victor (unita' 6). La variabile 1 conta le armi lanciate dal giocatore:
-- l'Alfa risponde al fuoco solo se non e' 0. Codici 8..18: testi vuoti, mai emessi.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "richiamato", [4] = "fallimento",
                      [5] = "successo", [6] = "successo", [7] = "richiamato",
                      [8] = "fallimento", [9] = "fallimento", [10] = "fallimento", [11] = "fallimento",
                      [12] = "fallimento", [13] = "fallimento", [14] = "fallimento", [15] = "fallimento",
                      [16] = "fallimento", [17] = "fallimento", [18] = "fallimento", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "seminare", tipo = "esito", codici = { 5, 6 },
      testo = { it = "Far perdere le tracce all'Alfa sovietico", en = "Shake off the trailing Soviet Alfa" } },
    { id = "non_sparare", tipo = "variabile", indice = 1, uguale = 0, primario = false,
      testo = { it = "Non lanciare armi se i russi non fanno sul serio", en = "Do not launch weapons unless the Russians get serious" } },
    { id = "sovietici", tipo = "proteggere", unita = { 2, 6 }, primario = false,
      testo = { it = "Non affondare i sottomarini sovietici", en = "Do not sink the Soviet submarines" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
