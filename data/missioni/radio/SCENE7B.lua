-- Contenuti radio dell'Enhanced per SCENE7B, CAT WALK (lato Alfa, Kunashir). R3 (docs/11): colore e contesto, obiettivi
-- ed esiti restano quelli dell'originale (proteggere la PLARB Krasnogvardeets). Sessioni ogni 10 minuti con la Flotta
-- del Pacifico.
-- Correzione storica (solo Enhanced, roadmap Fase 0): gli stretti di Kunashir collegano il Mare di Okhotsk al Pacifico,
-- non il Mar del Giappone come dice il manuale del 1989; il messaggio ROTTA lo dice dentro il gioco (la nota del
-- traduttore e' nel manuale italiano). Briefing, unita' e regole restano quelle dell'originale.
return {
  ordini = { da = "KP TOF", a = "PL «LENINSKII KOMSOMOL»", oggetto = "PRIKRYTIE", classifica = "SOV. SEKRETNO", data = { giorno = 6, mese = 7, anno = 1989, ora_utc = 18 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 400 },
  finestre = { ogni_s = 600, primo_s = 300, tolleranza_s = 90, obbligatorie = true, ricevuta = false, saltate_max = 2 },
  stato_iniziale = {
    -- distruggere le unita' da guerra minacciose; le navi di superficie in zona non si affondano
    roe = { sottomarini = "libero", navi = "libero", aerei = "libero", mercantili = "trattenuto", neutrali = "trattenuto" },
  },
  { id = "ROTTA", dopo_s = 60, precedenza = "priority", da = "KP TOF", oggetto = "MARSHRUT",
    classifica = "SOV. SEKRETNO",
    testo = { it = "LA PLARB ESCE DAL MARE DI OCHOTSK NEL PACIFICO PER LO STRETTO DI EKATERINA, FRA KUNASHIR E ITURUP. GLI AMERICANI LA ASPETTANO AL PASSO.",
              en = "THE PLARB IS LEAVING THE SEA OF OKHOTSK FOR THE PACIFIC THROUGH THE EKATERINA STRAIT, BETWEEN KUNASHIR AND ITURUP. THE AMERICANS WILL BE WAITING AT THE PASS." } },
  { id = "KONTAKT", dopo_s = 180, precedenza = "immediate", da = "KP TOF", oggetto = "KONTAKT",
    classifica = "SEKRETNO",
    testo = { it = "IDROFONI COSTIERI DI KUNASHIR: SOTTOMARINO AMERICANO PROBABILE A SUD DEL PASSO, SULLA ROTTA DELLA PLARB.",
              en = "KUNASHIR COASTAL HYDROPHONES: PROBABLE AMERICAN SUBMARINE SOUTH OF THE PASS, ON THE PLARB'S ROUTE." },
    contatto = { unita = { 7 }, eta_s = 900, errore_m = 8000, sensore = "costiera", rotta_err = 40, velocita_err_kn = 4 } },
  { id = "VERTOLET", dopo_s = 480, precedenza = "priority", da = "KP TOF", oggetto = "VERTOLET",
    classifica = "SEKRETNO",
    testo = { it = "ELICOTTERO ANTISOM AMERICANO IN VOLO SULLA ZONA: BOE IN ACQUA. LA PLARB PROSEGUE.",
              en = "AMERICAN ASW HELICOPTER OVER THE AREA: BUOYS IN THE WATER. THE PLARB CONTINUES." },
    contatto = { unita = { 6 }, eta_s = 300, errore_m = 5000, sensore = "radar", rotta_err = 40, velocita_err_kn = 10 } },
  { id = "POGODA", dopo_s = 780, precedenza = "routine", da = "GIDROMET TOF", oggetto = "POGODA",
    classifica = "DLYA SLUZHEBNOGO",
    testo = { it = "CURILI MERIDIONALI: VENTO DA NORD-OVEST 25 NODI, MARE 5, NEBBIA A BANCHI.",
              en = "SOUTHERN KURILS: WIND NORTHWEST 25 KNOTS, SEA STATE 5, PATCHY FOG." } },
  { id = "UTOCHNENIE", dopo_s = 1080, precedenza = "immediate", da = "KP TOF", oggetto = "UTOCHNENIE",
    classifica = "SEKRETNO",
    testo = { it = "UN IL-38 CONFERMA IL SOTTOMARINO STRANIERO. DISTRUGGERLO PRIMA CHE RAGGIUNGA LA PLARB.",
              en = "AN IL-38 CONFIRMS THE FOREIGN SUBMARINE. DESTROY IT BEFORE IT REACHES THE PLARB." },
    contatto = { unita = { 7 }, eta_s = 300, errore_m = 3000, sensore = "il38", rotta_err = 25, velocita_err_kn = 3 } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "PLARB", se = { unita_colpita = { 2 } }, precedenza = "flash", da = "KP TOF", oggetto = "PLARB ATAKOVANA",
    classifica = "SOV. SEKRETNO",
    testo = { it = "LA PLARB E' STATA COLPITA! TROVARE E DISTRUGGERE L'ATTACCANTE.",
              en = "THE PLARB HAS BEEN HIT! FIND AND DESTROY THE ATTACKER." } },
}
