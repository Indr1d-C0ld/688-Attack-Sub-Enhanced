-- ASW aereo dell'Enhanced per SCENE7C, Passerella (docs/13_asw_aereo.md, A3; formato in docs/09_schema_missioni.md §5.4).
-- Il LAMPS vola in avanscoperta per passare al giocatore le coordinate del Typhoon (briefing): ricognizione in
-- guerra, nessuna arma, bersagli i sottomarini sovietici. La boa marcatrice dello script diventa una DICASS posata.
-- sensore_classico: lo script decide ancora col suo sensore classico, tarato sulla missione (docs/09 §5.4).
return {
  { unita = 6, missione = "ricognizione", situazione = "guerra",
    zona = { x = -101000, y = -80000, raggio_m = 40000 }, rotta_presunta = 180, sensore_classico = true },
}
