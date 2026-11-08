-- Contenuti radio dell'Enhanced per SCENE8B, SURPRISE PARTY (lato Alfa, Piattaforma Celtica). R3 (docs/11): colore e
-- contesto, obiettivi ed esiti restano quelli dell'originale. Sessioni ogni 10 minuti con la Flotta del Nord.
return {
  ordini = { da = "KP SF", a = "PL «LENINSKII KOMSOMOL»", oggetto = "PEREKHVAT", classifica = "SOV. SEKRETNO", data = { giorno = 21, mese = 8, anno = 1989, ora_utc = 13 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 400 },
  finestre = { ogni_s = 600, primo_s = 300, tolleranza_s = 90, obbligatorie = true, ricevuta = false, saltate_max = 2 },
  stato_iniziale = {
    roe = { sottomarini = "libero", navi = "libero", aerei = "libero", mercantili = "libero", neutrali = "trattenuto" },
  },
  { id = "RAZVEDKA", dopo_s = 180, precedenza = "immediate", da = "KP SF", oggetto = "RAZVEDKA",
    classifica = "SOV. SEKRETNO",
    testo = { it = "UN TU-95 DA RICOGNIZIONE HA VISTO IL GRUPPO DELLA NATO: PORTAEREI, CORAZZATA, RIFORNITRICE E DUE SCORTE, ROTTA NORD-EST.",
              en = "A TU-95 RECONNAISSANCE AIRCRAFT HAS SIGHTED THE NATO GROUP: CARRIER, BATTLESHIP, OILER AND TWO ESCORTS, COURSE NORTHEAST." },
    contatto = { unita = { 2, 3, 4, 5, 6 }, eta_s = 1200, errore_m = 8000, sensore = "tu95", rotta_err = 15, velocita_err_kn = 3 } },
  { id = "PL", dopo_s = 480, precedenza = "priority", da = "KP SF", oggetto = "PL PROTIVNIKA",
    classifica = "SEKRETNO",
    testo = { it = "UN SOTTOMARINO AMERICANO PROCEDE DAVANTI AL GRUPPO COME AVANSCOPERTA.",
              en = "AN AMERICAN SUBMARINE IS SCOUTING AHEAD OF THE GROUP." } },
  { id = "POGODA", dopo_s = 780, precedenza = "routine", da = "GIDROMET SF", oggetto = "POGODA",
    classifica = "DLYA SLUZHEBNOGO",
    testo = { it = "ATLANTICO, PIATTAFORMA CELTICA: VENTO DA OVEST 20 NODI, MARE 4.",
              en = "ATLANTIC, CELTIC SHELF: WIND WEST 20 KNOTS, SEA STATE 4." } },
  { id = "UTOCHNENIE", dopo_s = 1500, precedenza = "immediate", da = "KP SF", oggetto = "UTOCHNENIE",
    classifica = "SOV. SEKRETNO",
    testo = { it = "NUOVO AVVISTAMENTO DEL TU-95: IL GRUPPO MANTIENE LA ROTTA. NESSUNA UNITA' NATO DEVE RAGGIUNGERE IL MARE DI NORVEGIA.",
              en = "NEW TU-95 SIGHTING: THE GROUP HOLDS ITS COURSE. NO NATO UNIT IS TO REACH THE NORWEGIAN SEA." },
    contatto = { unita = { 2, 3, 4, 5, 6 }, eta_s = 600, errore_m = 5000, sensore = "tu95", rotta_err = 10, velocita_err_kn = 2 } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "ESKORT", se = { unita_fuori = { 7 } }, precedenza = "priority", da = "KP SF", oggetto = "PL PROTIVNIKA",
    classifica = "SEKRETNO",
    testo = { it = "IL SOTTOMARINO AMERICANO DI AVANSCOPERTA NON SI SENTE PIU'. IL GRUPPO E' SCOPERTO.",
              en = "THE AMERICAN SCOUT SUBMARINE IS NO LONGER HEARD. THE GROUP IS UNCOVERED." } },
}
