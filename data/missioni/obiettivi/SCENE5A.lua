-- SCENE5A: Fuga. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_0260 (ogni 20 s da 60 s): finche' il giocatore e' nel rettangolo dello stretto non decide; uscito a est
-- (X > 6419.7) esito 8; uscito altrove, se ha lanciato armi (variabile 1) esito 10 (la 9 vorrebbe la variabile 3,
-- mai scritta: irraggiungibile), altrimenti secondo i sovietici in coda (variabile 2, L_032b/L_0348):
-- 4, 3, 2, 1, 0 = esiti 3, 4, 5, 6, 7.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "fallimento", [4] = "fallimento",
                      [5] = "fallimento", [6] = "fallimento", [7] = "successo", [8] = "richiamato",
                      [9] = "successo", [10] = "richiamato", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "atlantico", tipo = "esito", codici = { 3, 4, 5, 6, 7 },
      testo = { it = "Superare lo Stretto di Gibilterra verso l'Atlantico", en = "Clear the Strait of Gibraltar into the Atlantic" } },
    { id = "seminare", tipo = "variabile", indice = 2, uguale = 0,
      testo = { it = "Uscire dallo stretto senza sottomarini sovietici in coda", en = "Leave the strait with no Soviet submarine on your tail" } },
    { id = "non_sparare", tipo = "variabile", indice = 1, uguale = 0, primario = false,
      testo = { it = "Non usare le armi se i sovietici non sparano per primi", en = "Do not use weapons unless the Soviets fire first" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
