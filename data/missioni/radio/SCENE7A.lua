-- Contenuti radio dell'Enhanced per SCENE7A, CAT WALK (lato 688, Kunashir). R3 (docs/11): colore e contesto, obiettivi ed
-- esiti restano quelli dell'originale. Il LAMPS «vola avanti e passa le coordinate»: tre fix sul Typhoon.
-- Correzione storica (solo Enhanced, roadmap Fase 0): gli stretti di Kunashir collegano il Mare di Okhotsk al Pacifico,
-- non il Mar del Giappone come dice il manuale del 1989; il messaggio ROTTA lo dice dentro il gioco (la nota del
-- traduttore e' nel manuale italiano). Briefing, unita' e regole restano quelle dell'originale.
return {
  ordini = { da = "CINCPACFLT", a = "USS LOS ANGELES (SSN 688)", oggetto = "BOOMER BANGER", classifica = "SECRET", data = { giorno = 6, mese = 7, anno = 1989, ora_utc = 18 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 300 },
  stato_iniziale = {
    -- guerra; le navi in zona sono amiche (non colpirle); con la scorta meglio non impegnarsi
    roe = { sottomarini = "libero", navi = "libero", aerei = "trattenuto", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "ROTTA", dopo_s = 60, precedenza = "priority", da = "CTF 74", oggetto = "BOOMER TRANSIT",
    classifica = "SECRET",
    testo = { it = "INTELLIGENCE: IL TYPHOON LASCIA IL BASTIONE DEL MARE DI OCHOTSK E PUNTA AL PACIFICO PER LO STRETTO DI EKATERINA, FRA KUNASHIR E ITURUP. E' IL PASSO.",
              en = "INTELLIGENCE: THE TYPHOON IS LEAVING THE SEA OF OKHOTSK BASTION FOR THE PACIFIC THROUGH THE EKATERINA STRAIT, BETWEEN KUNASHIR AND ITURUP. THAT IS THE PASS." } },
  { id = "LAMPS1", dopo_s = 300, precedenza = "immediate", da = "CTF 74", oggetto = "BOOMER POSIT",
    classifica = "SECRET",
    testo = { it = "LAMPS: SOTTOMARINO LANCIAMISSILI CLASSE TYPHOON PRESO CON LE BOE A SUD DEL PASSO. DATO INCERTO.",
              en = "LAMPS: TYPHOON CLASS BOOMER HELD ON SONOBUOYS SOUTH OF THE PASS. DATA UNCERTAIN." },
    contatto = { unita = { 2 }, eta_s = 600, errore_m = 6000, sensore = "lamps", rotta_err = 30, velocita_err_kn = 3 } },
  { id = "SCORTA", dopo_s = 600, precedenza = "priority", da = "CTF 74", oggetto = "ESCORT SSN",
    classifica = "SECRET",
    testo = { it = "SOSUS: SOTTOMARINO D'ATTACCO DI SCORTA, PROBABILE ALFA, A NORD DEL TYPHOON. NON IMPEGNARSI SE SI PUO' EVITARE.",
              en = "SOSUS: ESCORTING ATTACK SUBMARINE, PROBABLE ALFA, NORTH OF THE TYPHOON. DO NOT ENGAGE IF AVOIDABLE." },
    contatto = { unita = { 7 }, eta_s = 900, errore_m = 8000, sensore = "sosus", rotta_err = 35, velocita_err_kn = 4 } },
  { id = "LAMPS2", dopo_s = 900, precedenza = "immediate", da = "CTF 74", oggetto = "BOOMER POSIT UPDATE",
    classifica = "SECRET",
    testo = { it = "LAMPS: NUOVO FIX SUL TYPHOON. ROTTA E VELOCITA' INVARIATE.",
              en = "LAMPS: NEW FIX ON THE TYPHOON. COURSE AND SPEED UNCHANGED." },
    contatto = { unita = { 2 }, eta_s = 300, errore_m = 3000, sensore = "lamps", rotta_err = 20, velocita_err_kn = 2 } },
  { id = "METEO", dopo_s = 1200, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX KURILS",
    classifica = "UNCLASSIFIED",
    testo = { it = "CURILI MERIDIONALI: VENTO DA NORD-OVEST 25 NODI, MARE 5, NEBBIA A BANCHI. STRATO TERMICO A 60 METRI.",
              en = "SOUTHERN KURILS: WIND NORTHWEST 25 KNOTS, SEA STATE 5, PATCHY FOG. LAYER AT 200 FEET." } },
  { id = "LAMPS3", dopo_s = 1500, precedenza = "immediate", da = "CTF 74", oggetto = "BOOMER POSIT UPDATE",
    classifica = "SECRET",
    testo = { it = "LAMPS: ULTIMO FIX PRIMA DEL RIENTRO PER IL CARBURANTE.", en = "LAMPS: LAST FIX BEFORE RETURNING TO REFUEL." },
    contatto = { unita = { 2 }, eta_s = 240, errore_m = 2000, sensore = "lamps", rotta_err = 15, velocita_err_kn = 2 } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "AFFONDATO", se = { unita_fuori = { 2 } }, precedenza = "immediate", da = "CTF 74", oggetto = "BOOMER DOWN",
    classifica = "SECRET",
    testo = { it = "SOSUS HA SENTITO IL TYPHOON IMPLODERE. LASCIARE LA ZONA PRIMA CHE ARRIVI LA SCORTA.",
              en = "SOSUS HEARD THE TYPHOON BREAK UP. CLEAR THE AREA BEFORE THE ESCORT ARRIVES." } },
}
