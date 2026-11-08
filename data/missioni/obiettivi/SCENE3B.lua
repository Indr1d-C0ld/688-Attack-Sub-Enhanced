-- SCENE3B: Operazioni subacquee (lato sovietico: il giocatore e' l'Alfa Leninskii Komsomol, unita' 0; l'intruso
-- e' il Trafalgar, unita' 2). Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_0325: con traccia solida sul giocatore, se il giocatore ha fatto un impulso sonar (variabile 0) ed e' entro
-- 1.8 unita', il Trafalgar fugge e dopo 200 s da' l'esito 8 (L_03a7). L_02f8 (arrivo all'uscita): esito 6 se il
-- Trafalgar seguiva il Charlie (variabile 5 = 1), altrimenti 5. L_028b: fuori dall'area esito 3, senza il
-- Trafalgar esito 4, senza il Charlie II (unita' 7) esito 7.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "richiamato", [4] = "successo_parziale",
                      [5] = "successo", [6] = "fallimento", [7] = "fallimento", [8] = "successo",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "intruso", tipo = "scoprire", unita = { 2 }, primario = false,
      testo = { it = "Scoprire eventuali intrusi lungo la costa di Poljarnyj", en = "Detect any intruders along the Polyarny coast" } },
    { id = "scacciare", tipo = "esito", codici = { 8 },
      testo = { it = "Avvicinarsi all'intruso, attivare il sonar e scortarlo fuori dall'area",
                en = "Close the intruder, go active and escort it out of the area" } },
    { id = "charlie", tipo = "proteggere", unita = { 7 }, primario = false,
      testo = { it = "Proteggere il Charlie II in uscita dal porto", en = "Protect the Charlie II leaving port" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
