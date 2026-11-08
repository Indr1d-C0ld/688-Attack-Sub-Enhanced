-- Contenuti radio dell'Enhanced per SCENE0A, HIT AND RUN (lato 688, Baltico). R3 (docs/11), formato del dossier 10 §5
-- con le chiavi che il motore legge (docs/09 §5.1). Colore e contesto: obiettivi ed esiti restano quelli dell'originale.
-- La Marina «nega ogni conoscenza»: pochi messaggi, nessun aiuto, la finestra di lancio dei Tomahawk e la via d'uscita.
-- Correzione storica (solo Enhanced, roadmap Fase 0): gli stretti danesi non si passano in immersione (Drogden 7-9 m,
-- Darss circa 18 m); USCITA e COLPITO mandano il battello nelle acque danesi a sud del Drogden, non nell'Øresund.
-- L'area di lancio nello Skagerrak o nel Kattegat resta fuori: il file della missione e' anche il classico e il teatro
-- Baltico finisce prima del Kattegat.
return {
  ordini = { da = "COMSUBLANT", a = "USS LOS ANGELES (SSN 688)", oggetto = "HIT AND RUN", classifica = "TOP SECRET", data = { giorno = 3, mese = 10, anno = 1989, ora_utc = 1 } },
  parametri = { ciclo_s = 300, fondo_ogni_s = 300 },
  stato_iniziale = {
    roe = { sottomarini = "libero", navi = "libero", mercantili = "trattenuto", neutrali = "trattenuto", aerei = "libero" },
  },
  { id = "FINESTRA", dopo_s = 20, precedenza = "flash", da = "COMSUBLANT", oggetto = "STRIKE EXECUTE",
    classifica = "TOP SECRET",
    testo = { it = "ESEGUIRE ATTACCO PRESTABILITO. FINESTRA DI LANCIO TOMAHAWK APERTA PER CINQUE MINUTI. NESSUNA CONFERMA RICHIESTA. NESSUN SUPPORTO DISPONIBILE.",
              en = "EXECUTE PREARRANGED STRIKE. TOMAHAWK LAUNCH WINDOW OPEN FOR FIVE MINUTES. NO ACKNOWLEDGEMENT REQUIRED. NO SUPPORT AVAILABLE." } },
  { id = "FLOTTA", dopo_s = 120, precedenza = "immediate", da = "COMSUBLANT", oggetto = "BALTIC FLEET DISPOSITION",
    classifica = "SECRET",
    testo = { it = "SATELLITE: GRUPPO DI SUPERFICIE DELLA FLOTTA DEL BALTICO IN MARE, INCROCIATORE KIROV CON SLAVA, KARA E KRIVAK. DATO VECCHIO DI VENTI MINUTI.",
              en = "SATELLITE: BALTIC FLEET SURFACE GROUP AT SEA, KIROV CRUISER WITH SLAVA, KARA AND KRIVAK. DATA TWENTY MINUTES OLD." },
    contatto = { unita = { 5, 6, 7, 8 }, eta_s = 1200, errore_m = 6000, sensore = "satellite", rotta_err = 20, velocita_err_kn = 3 } },
  { id = "SUB", dopo_s = 600, precedenza = "priority", da = "COMSUBLANT", oggetto = "SUBMARINE THREAT",
    classifica = "SECRET",
    testo = { it = "INDICAZIONI ELINT DI SOTTOMARINI D'ATTACCO SOVIETICI IN USCITA DA LIEPAJA. CLASSI PROBABILI ALFA, VICTOR III, AKULA. POSIZIONE INCERTA.",
              en = "ELINT INDICATES SOVIET ATTACK SUBMARINES SAILING FROM LIEPAJA. PROBABLE CLASSES ALFA, VICTOR III, AKULA. POSITION UNCERTAIN." },
    contatto = { unita = { 2, 3, 4 }, eta_s = 1800, errore_m = 15000, sensore = "elint", rotta_err = 45, velocita_err_kn = 5 } },
  { id = "USCITA", dopo_s = 900, precedenza = "priority", da = "COMSUBLANT", oggetto = "EGRESS",
    classifica = "TOP SECRET",
    testo = { it = "VIA D'USCITA: ACQUE DANESI A SUD DI COPENAGHEN, BAIA DI KØGE. LE SOGLIE DI DROGDEN (7-9 M) E DARSS (18 M) SI PASSANO SOLO IN SUPERFICIE. I DANESI NON SONO INFORMATI.",
              en = "EGRESS: DANISH WATERS SOUTH OF COPENHAGEN, KOEGE BAY. THE DROGDEN (7-9 M) AND DARSS (18 M) SILLS CAN ONLY BE CROSSED SURFACED. THE DANES ARE NOT INFORMED." } },
  { id = "METEO", dopo_s = 1200, precedenza = "routine", da = "NAVOCEANCOM", oggetto = "WX BALTIC",
    classifica = "UNCLASSIFIED",
    testo = { it = "BALTICO MERIDIONALE: VENTO DA OVEST 15 NODI, MARE 3, FOSCHIA IN AUMENTO VERSO SERA. STRATO TERMICO DEBOLE A 20 METRI.",
              en = "SOUTHERN BALTIC: WIND WEST 15 KNOTS, SEA STATE 3, HAZE INCREASING TOWARD EVENING. WEAK LAYER AT 20 METERS." } },
  -- legati agli eventi (E6): vanno in onda la prima volta che la condizione e' vera
  { id = "COLPITO", se = { variabile = { 4, "==", 0 } }, precedenza = "immediate", da = "COMSUBLANT", oggetto = "STRIKE CONFIRMED",
    classifica = "TOP SECRET",
    testo = { it = "TUTTI E QUATTRO I TOMAHAWK IN VOLO. USCIRE SUBITO VERSO LE ACQUE DANESI, IN IMMERSIONE FINO ALLA BAIA DI KØGE. DA QUI IN POI NESSUN CONTATTO.",
              en = "ALL FOUR TOMAHAWKS AIRBORNE. EGRESS NOW TOWARD DANISH WATERS, SUBMERGED AS FAR AS KOEGE BAY. NO FURTHER CONTACT." } },
}
