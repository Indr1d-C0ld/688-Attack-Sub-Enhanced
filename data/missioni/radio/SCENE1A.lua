-- Contenuti radio dell'Enhanced per SCENE1A, TORPEX '89 (lato 688, Fær Øer). R3 (docs/11): colore e contesto,
-- obiettivi ed esiti restano quelli dell'originale. L'esercitazione dura 15 minuti: ciclo del broadcast di 5 minuti.
return {
  ordini = { da = "CINCLANTFLT", a = "USS LOS ANGELES (SSN 688)", oggetto = "TORPEX NLANT", classifica = "CONFIDENTIAL", data = { giorno = 14, mese = 3, anno = 1989, ora_utc = 10 } },
  parametri = { ciclo_s = 300, fondo_ogni_s = 200 },
  stato_iniziale = {
    -- i cacciatorpediniere radiati sono il bersaglio dell'esercitazione; il LAMPS e un sottomarino sovietico no
    roe = { navi = "libero", aerei = "trattenuto", sottomarini = "trattenuto", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "POLIGONO", dopo_s = 30, precedenza = "priority", da = "CINCLANTFLT", oggetto = "TORPEX NLANT RANGE STATUS",
    classifica = "CONFIDENTIAL",
    testo = { it = "POLIGONO LIBERO. TORPEX NLANT IN CORSO. ARBITRO: SOSUS. FINE DELL'ESERCITAZIONE CON TRE IMPULSI SONAR O DOPO QUINDICI MINUTI.",
              en = "RANGE CLEAR. TORPEX NLANT IN PROGRESS. REFEREE: SOSUS. EXERCISE ENDS ON THREE SONAR BLASTS OR AFTER FIFTEEN MINUTES." } },
  { id = "INTRUSO", dopo_s = 150, precedenza = "immediate", da = "CINCLANTFLT", oggetto = "UNIDENTIFIED SUBMERGED CONTACT",
    classifica = "SECRET",
    testo = { it = "SOSUS: POSSIBILE SOTTOMARINO SOVIETICO NEI PRESSI DEL POLIGONO. NON E' UN BERSAGLIO DELL'ESERCITAZIONE. NON IMPEGNARE.",
              en = "SOSUS: POSSIBLE SOVIET SUBMARINE IN VICINITY OF RANGE. NOT REPEAT NOT AN EXERCISE TARGET. DO NOT ENGAGE." },
    contatto = { unita = { 6 }, eta_s = 600, errore_m = 8000, sensore = "sosus", rotta_err = 40, velocita_err_kn = 4 } },
  { id = "METEO", dopo_s = 400, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX FAEROES",
    classifica = "UNCLASSIFIED",
    testo = { it = "FÆR ØER: VENTO DA SUD-OVEST 20 NODI, MARE 4, PIOVASCHI. STRATO TERMICO A 120 METRI.",
              en = "FAEROES: WIND SOUTHWEST 20 KNOTS, SEA STATE 4, SHOWERS. LAYER AT 400 FEET." } },
  { id = "TEMPO", dopo_s = 600, precedenza = "priority", da = "CINCLANTFLT", oggetto = "TORPEX TIME CHECK",
    classifica = "CONFIDENTIAL",
    testo = { it = "CINQUE MINUTI ALLA FINE DELL'ESERCITAZIONE.", en = "FIVE MINUTES REMAINING IN EXERCISE." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "BERSAGLI", se = { unita_fuori = { 2, 3, 4 } }, precedenza = "priority", da = "CINCLANTFLT", oggetto = "TORPEX TARGET STATUS",
    classifica = "CONFIDENTIAL",
    testo = { it = "SOSUS CONFERMA UN BERSAGLIO AFFONDATO SUL POLIGONO. CONTINUARE.",
              en = "SOSUS CONFIRMS ONE TARGET SUNK ON RANGE. CONTINUE." } },
}
