-- SCENE7C: Passerella a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles, unita' 0; il
-- giocatore 1 e' l'Alfa, unita' 1, di scorta al Typhoon, unita' 2). Obiettivi dal briefing; condizioni dagli script.
-- L_01e8 (ogni 30 s da 120 s): giocatore 0 fuori dall'area esito 9; senza il Typhoon esito 6 (giocatore 1: 8).
-- L_01ce: giocatore 1 fuori dall'area, esito 10. L_0360/L_0370: il Typhoon arriva al settore, esito 5 (7).
-- I codici 7, 8 e 15..18 sono del giocatore 1: categorie dal suo punto di vista.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "successo",
                      [5] = "fallimento", [6] = "successo", [7] = "successo", [8] = "fallimento",
                      [9] = "fallimento", [10] = "successo", [15] = "battello_perso", [16] = "battello_perso",
                      [17] = "successo", [18] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "typhoon", tipo = "affondare", unita = { 2 }, minimo = 1,
      testo = { it = "Affondare il lanciamissili balistici Typhoon prima che raggiunga il suo settore",
                en = "Sink the Typhoon ballistic missile sub before it reaches its sector" } },
    { id = "scorta", tipo = "proteggere", unita = { 1 }, primario = false,
      testo = { it = "Non impegnarsi con il sottomarino di scorta, se possibile", en = "Avoid engaging the escorting attack sub if possible" } },
    { id = "mercantili", tipo = "proteggere", unita = { 3, 4, 5 }, primario = false,
      testo = { it = "Non colpire le navi amiche in zona", en = "Do not hit the friendly ships in the area" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
