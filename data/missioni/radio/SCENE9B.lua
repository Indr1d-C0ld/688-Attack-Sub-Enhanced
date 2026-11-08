-- Contenuti radio dell'Enhanced per SCENE9B, HOMECOMING (lato Alfa, Piattaforma Celtica). R3 (docs/11): colore e
-- contesto, obiettivi ed esiti restano quelli dell'originale (l'Oscar lancia, l'Alfa finisce cio' che resta). Sessioni
-- ogni 10 minuti con la Flotta del Nord.
return {
  ordini = { da = "KP SF", a = "PL «LENINSKII KOMSOMOL»", oggetto = "UDAR PO KONVOYU", classifica = "SOV. SEKRETNO", data = { giorno = 12, mese = 9, anno = 1989, ora_utc = 5 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 400 },
  finestre = { ogni_s = 600, primo_s = 300, tolleranza_s = 90, obbligatorie = true, ricevuta = false, saltate_max = 2 },
  stato_iniziale = {
    roe = { sottomarini = "libero", navi = "libero", aerei = "libero", mercantili = "libero", neutrali = "trattenuto" },
  },
  { id = "KONVOJ", dopo_s = 180, precedenza = "immediate", da = "KP SF", oggetto = "KONVOJ",
    classifica = "SOV. SEKRETNO",
    testo = { it = "UN TU-95 DA RICOGNIZIONE HA VISTO IL CONVOGLIO: CINQUE MERCANTILI E DUE FREGATE, ROTTA EST.",
              en = "A TU-95 RECONNAISSANCE AIRCRAFT HAS SIGHTED THE CONVOY: FIVE MERCHANTS AND TWO FRIGATES, COURSE EAST." },
    contatto = { unita = { 2, 3, 4, 5, 6 }, eta_s = 1200, errore_m = 8000, sensore = "tu95", rotta_err = 15, velocita_err_kn = 3 } },
  { id = "PSKOVSKII", dopo_s = 420, precedenza = "priority", da = "KP SF", oggetto = "VZAIMODEJSTVIE",
    classifica = "SOV. SEKRETNO",
    testo = { it = "IL PSKOVSKII KOMSOMOLETS E' IN POSIZIONE DI LANCIO. NON AVVICINARSI A LUI. ATTACCARE DOPO L'IMPATTO DEI MISSILI.",
              en = "PSKOVSKII KOMSOMOLETS IS IN LAUNCH POSITION. KEEP CLEAR OF HER. ATTACK AFTER THE MISSILES IMPACT." } },
  { id = "ESKORT", dopo_s = 720, precedenza = "priority", da = "KP SF", oggetto = "PL PROTIVNIKA",
    classifica = "SEKRETNO",
    testo = { it = "UN SOTTOMARINO AMERICANO SCORTA IL CONVOGLIO.", en = "AN AMERICAN SUBMARINE IS ESCORTING THE CONVOY." },
    contatto = { unita = { 9 }, eta_s = 900, errore_m = 9000, sensore = "tu95", rotta_err = 40, velocita_err_kn = 5 } },
  { id = "POGODA", dopo_s = 1020, precedenza = "routine", da = "GIDROMET SF", oggetto = "POGODA",
    classifica = "DLYA SLUZHEBNOGO",
    testo = { it = "ATLANTICO, APPROCCI DI BISCAGLIA: VENTO DA SUD-OVEST 25 NODI, MARE 5, PIOGGIA.",
              en = "ATLANTIC, BISCAY APPROACHES: WIND SOUTHWEST 25 KNOTS, SEA STATE 5, RAIN." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "UDAR", se = { unita_colpita = { 2, 3, 4, 5, 6 } }, precedenza = "flash", da = "KP SF", oggetto = "UDAR",
    classifica = "SOV. SEKRETNO",
    testo = { it = "I MISSILI HANNO COLPITO IL CONVOGLIO. AVVICINARSI E DISTRUGGERE CIO' CHE RESTA A GALLA.",
              en = "THE MISSILES HAVE STRUCK THE CONVOY. CLOSE IN AND DESTROY ANYTHING LEFT AFLOAT." } },
}
