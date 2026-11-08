-- SCENE4A: Mumar Cadaver. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_02f7 (ogni 20 s da 120 s): fuori dall'area esito 5; affondate le petroliere 4..7 esito 6.
-- L_02a7 (2700 s): conta le petroliere affondate (variabile 7): 0 esito 7, 1 esito 8, 2 o 3 esito 9, 4 esito 6.
-- Esito 19: affondato il mercantile neutrale (unita' 8).
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "richiamato",
                      [6] = "successo", [7] = "fallimento", [8] = "successo_parziale", [9] = "successo_parziale",
                      [19] = "fuoco_amico" },
  obiettivi = {
    { id = "petroliere", tipo = "affondare", unita = { 4, 5, 6, 7 }, minimo = 4,
      testo = { it = "Affondare le quattro petroliere libiche", en = "Sink the four Libyan tankers" } },
    { id = "fregate", tipo = "proteggere", classi = { "koni" }, primario = false, colpire = false,
      testo = { it = "Non attaccare le fregate Koni se non necessario", en = "Do not attack the Koni frigates unless necessary" } },
    { id = "mercantile", tipo = "proteggere", unita = { 8 }, primario = false,
      testo = { it = "Non affondare il mercantile neutrale", en = "Do not sink the neutral merchant" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
