-- SCENE5B: Fuga (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0; il 688 e' l'unita' 2).
-- Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_0171: variabile 2 = 1 con il 688 come bersaglio, 2 con traccia solida entro 5 miglia, 0 se perso.
-- L_019c: quando il 688 esce dallo stretto, variabile 2 = 2 esito 6, = 1 esito 7, = 0 esito 5.
-- L_0306: giocatore fuori dall'area esito 8; senza il 688 esito 4 (la 3 vorrebbe la variabile 3, mai scritta).
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "richiamato",
                      [5] = "fallimento", [6] = "successo", [7] = "fallimento", [8] = "richiamato",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "scoprire", tipo = "scoprire", unita = { 2 }, primario = false,
      testo = { it = "Intercettare il 688 americano all'uscita dal Mediterraneo", en = "Intercept the American 688 leaving the Mediterranean" } },
    { id = "contatto", tipo = "variabile", indice = 2, uguale = 2,
      testo = { it = "Mantenere il contatto sonar con il 688 entro la portata di tiro",
                en = "Hold sonar contact on the 688 within weapons range" } },
    { id = "los_angeles", tipo = "proteggere", unita = { 2 }, primario = false,
      testo = { it = "Non lanciare armi se gli americani non agiscono per primi", en = "Do not launch weapons unless the Americans act first" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
