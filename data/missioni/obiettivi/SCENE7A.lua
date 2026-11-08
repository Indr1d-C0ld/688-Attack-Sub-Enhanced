-- SCENE7A: Passerella. Obiettivi dal briefing; condizioni dagli script (funzioni che danno gli esiti).
-- L_01ce (ogni 30 s da 120 s): esito 6 appena manca il Typhoon (unita' 2).
-- L_032c/L_033c: il Typhoon arriva al punto (-101900, -80300) del settore di pattugliamento, esito 5.
-- L'esito 7 non e' emesso da nessuno script. Esito 19: affondata un'unita' di superficie amica (3..5).
-- Il codice 4 (altro giocatore distrutto, dato dal motore) non capita in partita singola.
return {
  categorie_esiti = { [1] = "battello_perso", [2] = "battello_perso", [4] = "successo", [5] = "fallimento",
                      [6] = "successo", [7] = "successo", [19] = "fuoco_amico" },
  obiettivi = {
    { id = "typhoon", tipo = "affondare", unita = { 2 }, minimo = 1,
      testo = { it = "Affondare il lanciamissili balistici Typhoon prima che raggiunga il suo settore",
                en = "Sink the Typhoon ballistic missile sub before it reaches its sector" } },
    { id = "scorta", tipo = "proteggere", unita = { 7 }, primario = false,
      testo = { it = "Non impegnarsi con i sottomarini di scorta, se possibile", en = "Avoid engaging the escorting attack subs if possible" } },
    { id = "mercantili", tipo = "proteggere", unita = { 3, 4, 5 }, primario = false,
      testo = { it = "Non colpire le navi amiche in zona", en = "Do not hit the friendly ships in the area" } },
    { id = "vivo", tipo = "sopravvivere", testo = { it = "Riportare a casa il battello", en = "Bring the boat home" } },
  },
}
