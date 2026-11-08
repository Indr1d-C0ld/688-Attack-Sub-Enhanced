-- ASW aereo dell'Enhanced per SCENE1A, TORPEX '89 (docs/13_asw_aereo.md, A3; formato in docs/09_schema_missioni.md §5.4).
-- L'SH-60B dell'esercitazione caccia il giocatore in pace: boe, MAD e attacco simulato col SUS, niente armi. Lo
-- script decide il quando (pattuglia fra i due punti, datum sulla nave bersaglio colpita); il controllore il come.
-- sensore_classico: lo script decide ancora col suo sensore classico, tarato sulla missione (docs/09 §5.4).
return {
  { unita = 5, missione = "caccia", situazione = "pace", bersagli = "giocatore",
    zona = { x = 11347, y = -37002, raggio_m = 31000 }, rotta_presunta = 77, sensore_classico = true },
}
