-- Contenuti radio dell'Enhanced per SCENE3B, SUB OPS (lato Alfa, Barents presso Poljarnyj). R3 (docs/11), sull'esempio
-- B del dossier 10 §5.4: colore e contesto, obiettivi ed esiti restano quelli dell'originale.
return {
  ordini = { da = "KP SF", a = "PL «LENINSKII KOMSOMOL»", oggetto = "POISK", classifica = "SOV. SEKRETNO", data = { giorno = 18, mese = 4, anno = 1989, ora_utc = 9.5 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 400 },
  finestre = { ogni_s = 600, primo_s = 300, tolleranza_s = 90, obbligatorie = true, ricevuta = false, saltate_max = 2 },
  stato_iniziale = {
    -- trovato l'intruso, il compito e' il sonar attivo e scortarlo fuori: le armi solo su un bersaglio identificato
    roe = { sottomarini = "stretto", navi = "trattenuto", mercantili = "trattenuto", neutrali = "trattenuto", aerei = "trattenuto" },
    neutrali = { 5, 6 },
  },
  { id = "INT1", dopo_s = 240, precedenza = "immediate", da = "KP SF", oggetto = "KONTAKT",
    classifica = "SEKRETNO",
    testo = { it = "STAZIONE COSTIERA DI KIL'DIN: CONTATTO SUBACQUEO PROBABILE A NORD DELLA COSTA DI POLJARNYJ. CLASSE SCONOSCIUTA.",
              en = "KILDIN COASTAL STATION: PROBABLE SUBMERGED CONTACT NORTH OF THE POLYARNYY COAST. CLASS UNKNOWN." },
    contatto = { unita = { 2 }, eta_s = 600, errore_m = 9000, sensore = "costiera", rotta_err = 60, velocita_err_kn = 4 } },
  { id = "MOV1", dopo_s = 540, precedenza = "priority", da = "KP SF", oggetto = "DVIZHENIE PL",
    classifica = "SOV. SEKRETNO",
    testo = { it = "IL SOTTOMARINO PROGETTO 670M ESCE DAL PORTO. ZONA PROTETTA DI QUATTRO MIGLIA ATTORNO A LUI: NON IMPIEGARE ARMI.",
              en = "SUBMARINE PROJECT 670M SAILING. FOUR MILE PROTECTED ZONE AROUND HER: NO WEAPONS." },
    zone = { aggiungi = { { id = "CHARLIE", tipo = "protetta_mobile", unita = 7, avanti_m = 7400, dietro_m = 7400, lato_m = 7400 } } } },
  { id = "METEO", dopo_s = 840, precedenza = "routine", da = "GIDROMET SF", oggetto = "POGODA",
    classifica = "DLYA SLUZHEBNOGO",
    testo = { it = "MARE DI BARENTS, COSTA DI MURMANSK: VENTO DA NORD-EST 20 NODI, NEVE A ROVESCI, MARE 4.",
              en = "BARENTS SEA, MURMANSK COAST: WIND NORTHEAST 20 KNOTS, SNOW SHOWERS, SEA STATE 4." } },
  -- un inganno: chi lo crede rientra e lascia l'intruso al Charlie II
  { id = "FALSO", dopo_s = 1080, precedenza = "flash", da = "KP SF", oggetto = "OTBOJ",
    classifica = "SEKRETNO", autentico = false,
    testo = { it = "IL CONTATTO E' UN NOSTRO SOTTOMARINO. CESSARE LA RICERCA E RIENTRARE.",
              en = "THE CONTACT IS ONE OF OURS. CEASE SEARCH AND RETURN TO PORT." } },
  { id = "FL1", dopo_s = 1200, precedenza = "flash", da = "KP SF", oggetto = "DOLOZHIT'",
    classifica = "SEKRETNO",
    testo = { it = "UN IL-38 CONFERMA UN SOMMERGIBILE STRANIERO. AVVICINARSI, ACCENDERE IL SONAR E SCORTARLO FUORI DALL'AREA.",
              en = "IL-38 CONFIRMS A FOREIGN SUBMARINE. CLOSE, GO ACTIVE AND ESCORT HIM OUT OF THE AREA." },
    contatto = { unita = { 2 }, eta_s = 300, errore_m = 2500, sensore = "il38", rotta_err = 30, velocita_err_kn = 3 } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "ISTRUZIONI", se = { trasmesso = "contatto" }, precedenza = "immediate", da = "KP SF", oggetto = "UKAZANIYA",
    classifica = "SEKRETNO",
    ricevuta = true,
    testo = { it = "RAPPORTO RICEVUTO. SEGUIRE IL CONTATTO, SONAR ATTIVO, NON IMPIEGARE ARMI. SCORTARLO FUORI DALLE ACQUE TERRITORIALI.",
              en = "REPORT RECEIVED. FOLLOW THE CONTACT, GO ACTIVE, NO WEAPONS. ESCORT HIM OUT OF TERRITORIAL WATERS." } },
}
