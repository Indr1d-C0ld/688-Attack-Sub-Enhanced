-- Contenuti radio dell'Enhanced per SCENE1B, TORPEX '89 (USS Dallas contro il Los Angeles, Fær Øer). R3 (docs/11):
-- colore e contesto, obiettivi ed esiti restano quelli dell'originale.
return {
  ordini = { da = "CINCLANTFLT", a = "USS DALLAS (SSN 700)", oggetto = "TORPEX NLANT", classifica = "CONFIDENTIAL", data = { giorno = 14, mese = 3, anno = 1989, ora_utc = 10 } },
  parametri = { ciclo_s = 300, fondo_ogni_s = 200 },
  stato_iniziale = {
    roe = { navi = "libero", sottomarini = "trattenuto", aerei = "trattenuto", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "POLIGONO", dopo_s = 30, precedenza = "priority", da = "CINCLANTFLT", oggetto = "TORPEX NLANT RANGE STATUS",
    classifica = "CONFIDENTIAL",
    testo = { it = "POLIGONO LIBERO. GARA FRA DALLAS E LOS ANGELES SU CINQUE BERSAGLI. CONTA SOLO IL COLPO CHE AFFONDA. I MISSILI SONO AMMESSI.",
              en = "RANGE CLEAR. COMPETITION DALLAS VERSUS LOS ANGELES ON FIVE TARGETS. ONLY THE KILLING BLOW COUNTS. MISSILES ARE AUTHORIZED." } },
  { id = "SPAZIO", dopo_s = 120, precedenza = "immediate", da = "COMSUBLANT", oggetto = "WATER SPACE",
    classifica = "SECRET",
    testo = { it = "USS LOS ANGELES IN ESERCITAZIONE NELLO STESSO SPAZIO D'ACQUA. POSIZIONE AL VIA. NON IMPEGNARE IL BATTELLO AMICO.",
              en = "USS LOS ANGELES EXERCISING IN SAME WATER SPACE. POSITION AT START. DO NOT ENGAGE FRIENDLY SUBMARINE." },
    contatto = { unita = { 7 }, eta_s = 120, errore_m = 4000, sensore = "comando", rotta_err = 30, velocita_err_kn = 4 } },
  { id = "METEO", dopo_s = 400, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX FAEROES",
    classifica = "UNCLASSIFIED",
    testo = { it = "FÆR ØER: VENTO DA SUD-OVEST 20 NODI, MARE 4, PIOVASCHI. STRATO TERMICO A 120 METRI.",
              en = "FAEROES: WIND SOUTHWEST 20 KNOTS, SEA STATE 4, SHOWERS. LAYER AT 400 FEET." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "BERSAGLI", se = { unita_fuori = { 2, 3, 4, 5, 6 } }, precedenza = "priority", da = "CINCLANTFLT", oggetto = "TORPEX TARGET STATUS",
    classifica = "CONFIDENTIAL",
    testo = { it = "SOSUS CONFERMA UN BERSAGLIO AFFONDATO SUL POLIGONO. LA GARA CONTINUA.",
              en = "SOSUS CONFIRMS ONE TARGET SUNK ON RANGE. COMPETITION CONTINUES." } },
}
