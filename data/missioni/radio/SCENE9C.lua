-- Contenuti radio dell'Enhanced per SCENE9C, HOMECOMING a due (lato del 688; l'Alfa e' l'id 1, Piattaforma Celtica).
-- R3 (docs/11): colore e contesto, obiettivi ed esiti restano quelli dell'originale.
return {
  ordini = { da = "CINCLANTFLT", a = "USS LOS ANGELES (SSN 688)", oggetto = "ATLANTIC CROSSING", classifica = "TOP SECRET", data = { giorno = 12, mese = 9, anno = 1989, ora_utc = 5 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 300 },
  stato_iniziale = {
    roe = { sottomarini = "stretto", navi = "stretto", aerei = "stretto", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "CONVOGLIO", dopo_s = 120, precedenza = "priority", da = "CINCLANTFLT", oggetto = "CONVOY STATUS",
    classifica = "SECRET",
    testo = { it = "CONVOGLIO DI CINQUE NAVI DA CARICO CON DUE FREGATE PERRY, ROTTA EST PER IL GOLFO DI BISCAGLIA. TASK FORCE ALL'APPUNTAMENTO COME PREVISTO.",
              en = "CONVOY OF FIVE CARGO SHIPS WITH TWO PERRY FRIGATES, COURSE EAST FOR THE BAY OF BISCAY. TASK FORCE RENDEZVOUS AS SCHEDULED." } },
  { id = "OSCAR", dopo_s = 420, precedenza = "flash", da = "COMSUBLANT", oggetto = "SSGN THREAT",
    classifica = "SECRET",
    testo = { it = "SOSUS: SOTTOMARINO LANCIAMISSILI CLASSE OSCAR A OVEST DI GOBAN SPUR. MINACCIA MISSILISTICA CONTRO IL CONVOGLIO.",
              en = "SOSUS: OSCAR CLASS SSGN WEST OF GOBAN SPUR. MISSILE THREAT TO THE CONVOY." },
    contatto = { unita = { 10 }, eta_s = 1200, errore_m = 9000, sensore = "sosus", rotta_err = 30, velocita_err_kn = 4 } },
  { id = "ALFA", dopo_s = 780, precedenza = "immediate", da = "COMSUBLANT", oggetto = "SSN THREAT",
    classifica = "SECRET",
    testo = { it = "SOSUS: SECONDO CONTATTO, PROBABILE SOTTOMARINO D'ATTACCO CLASSE ALFA, IN AVVICINAMENTO DA NORD-OVEST.",
              en = "SOSUS: SECOND CONTACT, PROBABLE ALFA CLASS ATTACK SUBMARINE, CLOSING FROM THE NORTHWEST." },
    contatto = { unita = { 1 }, eta_s = 900, errore_m = 8000, sensore = "sosus", rotta_err = 35, velocita_err_kn = 5 } },
  { id = "METEO", dopo_s = 1080, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX BAY OF BISCAY",
    classifica = "UNCLASSIFIED",
    testo = { it = "APPROCCI DI BISCAGLIA: VENTO DA SUD-OVEST 25 NODI, MARE 5, PIOGGIA. STRATO TERMICO A 70 METRI.",
              en = "BISCAY APPROACHES: WIND SOUTHWEST 25 KNOTS, SEA STATE 5, RAIN. LAYER AT 230 FEET." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "MISSILI", se = { evento = "lancio_nemico" }, precedenza = "flash", da = "CINCLANTFLT", oggetto = "MISSILE ATTACK",
    classifica = "SECRET",
    testo = { it = "LANCIO DI MISSILI CONTRO IL CONVOGLIO. IL LANCIATORE E' VICINO: TROVARLO.",
              en = "MISSILE LAUNCH AGAINST THE CONVOY. THE LAUNCHER IS CLOSE: FIND IT." } },
}
