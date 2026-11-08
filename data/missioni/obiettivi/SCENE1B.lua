-- SCENE1B: TORPEX '89 (contro l'USS Los Angeles guidato dal computer, unita' 7). Obiettivi dal briefing;
-- condizioni dagli script (funzioni che danno gli esiti).
-- L_0531 (ogni 10 s da 200 s): fuori dal poligono esito 7; senza l'unita' 7 esito 3; affondati i cacciatorpediniere
-- 2..6, confronta le uccisioni del giocatore (variabile 10) con quelle del Los Angeles (variabile 17):
-- piu' del Los Angeles esito 6, nessuna esito 4, meno esito 5 (alla pari nessun esito: difetto dell'originale).
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [3] = "fuoco_amico", [4] = "fallimento",
                      [5] = "fallimento", [6] = "successo", [7] = "richiamato" },
  obiettivi = {
    { id = "gara", tipo = "esito", codici = { 6 },
      testo = { it = "Affondare più cacciatorpediniere del Los Angeles", en = "Sink more destroyers than the Los Angeles" } },
    { id = "cacciatorpediniere", tipo = "affondare", unita = { 2, 3, 4, 5, 6 }, minimo = 5, primario = false,
      testo = { it = "Affondare i cinque cacciatorpediniere Forrest Sherman", en = "Sink the five Forrest Sherman destroyers" } },
    { id = "los_angeles", tipo = "proteggere", unita = { 7 }, primario = false,
      testo = { it = "Non attaccare l'USS Los Angeles", en = "Do not attack USS Los Angeles" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
