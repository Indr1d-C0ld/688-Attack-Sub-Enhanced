-- SCENE5C: Fuga a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles, unita' 0; il
-- giocatore 1 e' l'Alfa, unita' 1). Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_0175: quando il Los Angeles esce dallo stretto verso ovest, variabile 2 (contatto dell'Alfa: 1 bersaglio,
-- 2 traccia solida entro 5 miglia) = 0 esito 9, = 2 esito 10, = 1 esito 11; uscito a est esito 8.
-- L_015b: l'Alfa fuori dall'area, esito 7. I codici 5, 6, 12 e 15..18 sono del giocatore 1: categorie dal suo
-- punto di vista.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "successo",
                      [5] = "fallimento", [6] = "successo", [7] = "successo", [8] = "fallimento",
                      [9] = "successo", [10] = "fallimento", [11] = "fallimento", [12] = "fallimento",
                      [15] = "battello_perso", [16] = "battello_perso", [17] = "successo", [18] = "successo",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "atlantico", tipo = "esito", codici = { 9, 10, 11 },
      testo = { it = "Superare lo Stretto di Gibilterra verso l'Atlantico", en = "Clear the Strait of Gibraltar into the Atlantic" } },
    { id = "seminare", tipo = "variabile", indice = 2, uguale = 0,
      testo = { it = "Uscire dallo stretto senza il sottomarino sovietico in coda", en = "Leave the strait with no Soviet submarine on your tail" } },
    { id = "alfa", tipo = "proteggere", unita = { 1 }, primario = false,
      testo = { it = "Non usare le armi se i sovietici non sparano per primi", en = "Do not use weapons unless the Soviets fire first" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
