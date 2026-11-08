-- Contenuti radio dell'Enhanced per SCENE4A, MUMAR CADAVER (lato 688, Gibilterra, limite 2700 s). R3 (docs/11), dall'
-- esempio A del dossier 10 §5.3: colore e contesto, obiettivi ed esiti restano quelli dell'originale. Le Koni che
-- attaccano arrivano col primo colpo su una petroliera (E6); il richiamo dell'esempio no, per non toccare gli esiti.
return {
  ordini = { da = "CINCLANTFLT", a = "USS LOS ANGELES (SSN 688)", oggetto = "MUMAR CADAVER", classifica = "SECRET", data = { giorno = 9, mese = 5, anno = 1989, ora_utc = 11 } },
  parametri = { ciclo_s = 600, fondo_ogni_s = 300 },
  stato_iniziale = {
    roe = { mercantili = "libero",      -- le petroliere libiche: il bersaglio della missione
            navi = "trattenuto",        -- le Koni: solo se necessario (briefing originale)
            sottomarini = "stretto",    -- i Foxtrot: solo se identificati
            neutrali = "trattenuto", aerei = "trattenuto" },
    neutrali = { 8 },
  },
  { id = "INT1", dopo_s = 180, precedenza = "priority", da = "CTF 69", oggetto = "TANKER GROUP POSIT",
    classifica = "SECRET",
    testo = { it = "P-3 DA ROTA: QUATTRO PETROLIERE IN COLONNA ALL'IMBOCCO OCCIDENTALE DELLO STRETTO, ROTTA 270, VELOCITA' 12. DUE FREGATE KONI DI SCORTA.",
              en = "P-3 FROM ROTA: FOUR TANKERS IN COLUMN AT WESTERN ENTRANCE OF STRAIT, COURSE 270, SPEED 12. TWO KONI FRIGATES ESCORTING." },
    contatto = { unita = { 4, 5, 6, 7 }, eta_s = 900, errore_m = 3000, sensore = "p3", rotta_err = 15, velocita_err_kn = 2 } },
  { id = "NEU1", dopo_s = 420, precedenza = "immediate", da = "CINCLANTFLT", oggetto = "NEUTRAL IN COMPANY",
    classifica = "SECRET",
    testo = { it = "MERCANTILE DI BANDIERA NEUTRALE IN COMPAGNIA DEL GRUPPO. CLASSIFICARE PRIMA DEL LANCIO.",
              en = "NEUTRAL FLAG MERCHANT IN COMPANY WITH GROUP. CLASSIFY BEFORE LAUNCH." },
    roe = { neutrali = "trattenuto", mercantili = "stretto" } },
  { id = "FOXTROT", dopo_s = 900, precedenza = "priority", da = "CTF 69", oggetto = "SUBMARINE ESCORT",
    classifica = "SECRET",
    testo = { it = "INTELLIGENCE: DUE SOTTOMARINI DIESEL CLASSE FOXTROT POTREBBERO SEGUIRE LE PETROLIERE. QUOTA PERISCOPICA PROBABILE.",
              en = "INTELLIGENCE: TWO FOXTROT CLASS DIESEL SUBMARINES MAY BE ACCOMPANYING THE TANKERS. PROBABLY AT PERISCOPE DEPTH." },
    contatto = { unita = { 2, 3 }, eta_s = 1200, errore_m = 6000, sensore = "elint", rotta_err = 30, velocita_err_kn = 3 } },
  { id = "METEO", dopo_s = 1200, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX GIBRALTAR",
    classifica = "UNCLASSIFIED",
    testo = { it = "STRETTO DI GIBILTERRA: LEVANTE 25 NODI, MARE 4. CORRENTE SUPERFICIALE VERSO EST, ACQUA ATLANTICA SOPRA I 100 METRI.",
              en = "STRAIT OF GIBRALTAR: LEVANTER 25 KNOTS, SEA STATE 4. SURFACE CURRENT EASTBOUND, ATLANTIC WATER ABOVE 330 FEET." } },
  { id = "INT2", dopo_s = 1500, precedenza = "priority", da = "CTF 69", oggetto = "TANKER GROUP UPDATE",
    classifica = "SECRET",
    testo = { it = "AGGIORNAMENTO P-3: IL GRUPPO DELLE PETROLIERE MANTIENE ROTTA E VELOCITA'.",
              en = "P-3 UPDATE: TANKER GROUP MAINTAINING COURSE AND SPEED." },
    contatto = { unita = { 4, 5, 6, 7 }, eta_s = 600, errore_m = 2000, sensore = "p3", rotta_err = 10, velocita_err_kn = 2 } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "KONI", se = { unita_colpita = { 4, 5, 6, 7 } }, precedenza = "flash", da = "CINCLANTFLT", oggetto = "ROE CHANGE",
    classifica = "SECRET",
    roe = { navi = "stretto" },
    testo = { it = "INTERCETTATO ORDINE ALLE KONI DI CACCIA ANTISOM. DIFENDERSI: ARMI SULLE KONI SOLO SE IDENTIFICATE E NECESSARIO.",
              en = "KONI UNITS ORDERED TO PROSECUTE. DEFEND YOURSELF: WEAPONS ON KONI ONLY IF IDENTIFIED AND NECESSARY." } },
}
