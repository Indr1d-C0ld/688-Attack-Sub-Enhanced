-- Contenuti radio dell'Enhanced per SCENE8C, SURPRISE PARTY a due (lato del 688; l'Alfa e' l'id 1, Piattaforma Celtica).
-- R3 (docs/11): colore e contesto, obiettivi ed esiti restano quelli dell'originale.
return {
  ordini = { da = "CINCLANTFLT", a = "USS LOS ANGELES (SSN 688)", oggetto = "SURPRISE PARTY", classifica = "SECRET", data = { giorno = 21, mese = 8, anno = 1989, ora_utc = 13 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 300 },
  stato_iniziale = {
    roe = { sottomarini = "stretto", navi = "trattenuto", aerei = "trattenuto", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "GRUPPO", dopo_s = 120, precedenza = "priority", da = "CINCLANTFLT", oggetto = "SURFACE GROUP MOVEMENT",
    classifica = "SECRET",
    testo = { it = "GRUPPO DI SUPERFICIE CON NIMITZ E IOWA IN ROTTA PER NORD-EST, 16 NODI. LOS ANGELES DI AVANSCOPERTA DAVANTI ALLA FORMAZIONE.",
              en = "SURFACE GROUP WITH NIMITZ AND IOWA COURSE NORTHEAST, 16 KNOTS. LOS ANGELES SCOUTING AHEAD OF THE FORMATION." } },
  { id = "SOSUS", dopo_s = 600, precedenza = "immediate", da = "COMSUBLANT", oggetto = "SOVIET SSN TRANSIT",
    classifica = "SECRET",
    testo = { it = "SOSUS: SOTTOMARINO SOVIETICO IN TRANSITO VERSO SUD, PROBABILE ALFA. POSSIBILE INTERCETTAZIONE DEL GRUPPO.",
              en = "SOSUS: SOVIET SUBMARINE TRANSITING SOUTH, PROBABLE ALFA. POSSIBLE INTERCEPT OF THE GROUP." },
    contatto = { unita = { 1 }, eta_s = 1200, errore_m = 9000, sensore = "sosus", rotta_err = 30, velocita_err_kn = 5 } },
  { id = "METEO", dopo_s = 1200, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX CELTIC SEA",
    classifica = "UNCLASSIFIED",
    testo = { it = "MARGINE DELLA PIATTAFORMA CELTICA: VENTO DA OVEST 20 NODI, MARE 4. STRATO TERMICO A 80 METRI SULLA SCARPATA.",
              en = "CELTIC SHELF MARGIN: WIND WEST 20 KNOTS, SEA STATE 4. LAYER AT 260 FEET OVER THE SLOPE." } },
  { id = "ALLERTA", dopo_s = 1800, precedenza = "flash", da = "CINCLANTFLT", oggetto = "INDICATIONS AND WARNING",
    classifica = "SECRET",
    testo = { it = "INDICAZIONI DI UN POSSIBILE ATTACCO SOVIETICO CONTRO IL GRUPPO. ARMI LIBERE SOLO SUI SOTTOMARINI IDENTIFICATI OSTILI.",
              en = "INDICATIONS OF POSSIBLE SOVIET ATTACK ON THE GROUP. WEAPONS FREE ONLY ON SUBMARINES IDENTIFIED HOSTILE." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "FUOCO", se = { evento = "lancio_nemico" }, precedenza = "flash", da = "CINCLANTFLT", oggetto = "GROUP UNDER ATTACK",
    classifica = "SECRET",
    annulla = "ALLERTA",   -- l'attacco vero supera l'avviso, se non e' ancora andato in onda
    roe = { sottomarini = "libero" },
    testo = { it = "UN SOTTOMARINO SOVIETICO HA APERTO IL FUOCO SUL GRUPPO. ARMI LIBERE CONTRO I SOTTOMARINI SOVIETICI.",
              en = "SOVIET SUBMARINE HAS OPENED FIRE ON THE GROUP. WEAPONS FREE ON SOVIET SUBMARINES." } },
}
