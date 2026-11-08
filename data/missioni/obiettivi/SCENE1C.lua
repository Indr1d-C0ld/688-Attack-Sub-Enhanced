-- SCENE1C: TORPEX '89 a due giocatori: obiettivi dal punto di vista del giocatore 0 (Los Angeles, unita' 0;
-- l'avversario e' il Dallas, unita' 1). Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_037b (ogni 20 s da 200 s): giocatore 0 fuori dal poligono esito 7; affondati i cacciatorpediniere 2..6, piu'
-- uccisioni del giocatore 0 (variabile 10) che del giocatore 1 (variabile 11) esito 6, meno esito 5.
-- L_0360: il giocatore 1 fuori dal poligono, esito 8 (esercitazione annullata: vinta a tavolino).
-- Esito 3 (avversario incagliato): vinta a tavolino; esito 4 (avversario distrutto): solo i due sottomarini
-- sono armati, quindi fuoco amico.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "successo", [4] = "fuoco_amico",
                      [5] = "fallimento", [6] = "successo", [7] = "richiamato", [8] = "successo",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "gara", tipo = "esito", codici = { 6 },
      testo = { it = "Affondare più cacciatorpediniere dell'avversario", en = "Sink more destroyers than your opponent" } },
    { id = "cacciatorpediniere", tipo = "affondare", unita = { 2, 3, 4, 5, 6 }, minimo = 5, primario = false,
      testo = { it = "Affondare i cinque cacciatorpediniere Forrest Sherman", en = "Sink the five Forrest Sherman destroyers" } },
    { id = "avversario", tipo = "proteggere", unita = { 1 }, primario = false,
      testo = { it = "Non attaccare il sottomarino avversario", en = "Do not attack the opposing submarine" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
