# 03 — Teatri, fondali, oceanografia e dottrina storica (remake "688 Attack Sub")

Dossier per il remake realistico di *688 Attack Sub* (EA, 1989): SSN-688 Los Angeles contro Project 705 "Alfa", 1985–1991.
Ambito: **teatri e fondali** (Parte A), **fonti dati per generare le mappe** (Parte B) e **letteratura storico-dottrinale tradotta in comportamenti IA** (Parte C).
Non trattati qui, perché coperti da altri due dossier: specifiche di piattaforme, armi e sensori; fisica del sonar ed equazioni di propagazione; rumore ambiente generico; TMA; dottrina generica.

Legenda della confidenza: **[A]** alta (fonte primaria o più fonti concordanti), **[M]** media (una sola fonte affidabile, oppure conoscenza consolidata non verificata in questa sessione), **[B]** bassa (stima, fonte debole o in conflitto: da verificare prima dell'uso).
Le profondità sono in metri, le distanze in miglia nautiche (nm) e le velocità in nodi (kt).

---

## 0. Le missioni del gioco originale (fonte primaria)

Dal manuale DOS/Amiga (testo trascritto: https://amigaland.de/wp-content/uploads/2024/02/688-Attack-Sub-Manual-ENG.pdf, cap. 3 "Missions", pp. 18–28) **[A]**:

| # | Nome | Teatro (figura) | Lato 688 | Lato Alfa |
|---|---|---|---|---|
| 1 | TORPEX '89 | Isole Faroe – Mar di Norvegia (Fig. 3.1) | addestramento: affondare 3 DD Forrest Sherman radiati in 15 minuti, con elicotteri ASW amici a caccia del sottomarino | (gioco via modem 688 contro 688, il Dallas) |
| 2 | Shake 'Em | Faroe (Fig. 3.1) | seminare un Alfa che ti pedina: almeno 3 minuti senza contatto | mantenere il contatto, mai perso per più di 3 minuti, stando a distanza di tiro e nei baffles |
| 3 | Sub Ops | Mare di Barents (Fig. 3.2) | — | "securing the port" prima del rientro a Severomorsk: cacciare gli intrusi; pingare solo sotto 1 nm |
| 4 | Mumar Cadaver | Stretto di Gibilterra (Fig. 3.3) | affondare 4 petroliere libiche scortate, in mezzo al traffico neutrale | — |
| 5 | Escape | Gibilterra (Fig. 3.3) | uscire verso ovest dallo stretto senza contatto sovietico | intercettare e pedinare il 688 senza usare l'attivo |
| 6 | Goulash | Imboccatura dell'Adriatico (Fig. 3.4) | scortare 6 mercantili verso nord-est fino a "Trivat" (evidentemente Tivat, Bocche di Cattaro) contro un blocco navale sovietico | — |
| 7 | Cat Walk | Passo di Kunashir (Fig. 3.5) | in guerra: SSBN uscito da Vladivostok; un Seahawk sgancia una boa marcatrice | proteggere l'SSBN |
| 8 | Surprise Party | Margine della Piattaforma Celtica (Fig. 3.6) | scorta di un gruppo navale NATO diretto a nord | attacco al gruppo da solo |
| 9 | Homecoming | Piattaforma Celtica (Fig. 3.6) | difesa di un convoglio diretto in Francia | imboscata di un Alfa con un Oscar SSGN |
| 10 | Hit and Run | Mar Baltico (Fig. 3.7) | lancio di 4 Tomahawk al largo della RDT, poi fuga dallo "stretto passaggio pattugliato" | — |

Note di progetto dal manuale **[A]**:
- Ogni missione si svolge su un "playfield" ristretto. Se il giocatore se ne allontana, la missione termina.
- Il designer (Paul Grace, "Designer's Notes", p. 68) dichiara un "combat range" di circa 15 miglia, con portate delle armi volutamente errate.
- Il modello acustico originale è uno strato termico singolo con zone d'ombra, e il consiglio al giocatore è nascondersi "appena sotto lo strato" (pp. 55–56).

Errori storico-geografici dell'originale, da correggere o motivare nel remake:
1. **Missione 7.** Il manuale dice che il passo di Kunashir collega il Mar del Giappone al Pacifico. È falso. Gli stretti attorno a Kunashir (Kunashirskij/Nemuro, Izmeny, Ekaterina) collegano il **Mare di Okhotsk** al Pacifico. Dal Mar del Giappone si esce da La Pérouse/Sōya, Tsugaru o Tsushima. **[A]** (geografia; Wikipedia "Nemuro Strait"; BRE "Курильские проливы", https://bigenc.ru/c/kuril-skie-prolivy-c67205)
2. **Missione 10.** Un 688 non può attraversare in immersione gli stretti danesi: Drogden 7–8 m, soglia di Darss circa 18 m (§A7). Inoltre, nelle acque territoriali danesi il diritto internazionale impone ai sottomarini la navigazione in superficie con bandiera (Conv. di Ginevra 1958 sul mare territoriale, art. 14(6); UNCLOS art. 20). **[A]**

---

## PARTE A — I sette teatri

Per ogni teatro: confini della mappa (circa 100–200 nm), batimetria, fondo, masse d'acqua e profilo di velocità del suono (SSP), correnti e maree, rumore ambiente, meteo, significato operativo.
I valori di velocità del suono sono **indicativi [B]**: li ho calcolati con l'equazione di Mackenzie (1981) su profili T/S rappresentativi scelti da me, solo per mostrare la forma del profilo. In produzione vanno sostituiti con valori calcolati da WOA23 (Parte B).

### A1. Isole Faroe – Mar di Norvegia (varco GIUK, cresta Islanda–Faroe, canale Faroe–Shetland)

**Confini consigliati:** 60.5–63.5°N, 10.0°W–4.0°W, cioè circa 180 × 170 nm, centrati sull'arcipelago (62.0°N 6.8°W).
Per una variante "GIUK": 61–65°N, 16°W–4°W (circa 240 × 350 nm, fuori scala da playfield).

**Batimetria e strutture**
- Piattaforma delle Faroe: in genere meno di 200 m, con banchi costieri da 50 a 150 m. **[M]**
- **Faroe Bank Channel** (FBC): soglia principale circa **840 m**, larga circa 15 km. Una seconda soglia di circa 850 m si trova 95 km a NW. È il passaggio più profondo della dorsale Groenlandia–Scozia e porta circa un terzo dell'overflow totale. **[A]** (Ocean Sci. 12, 2016, https://os.copernicus.org/articles/12/451/2016/os-12-451-2016.pdf; Hansen et al., os-12-1205-2016)
- **Wyville Thomson Ridge** (WTR): sella fra il FSC e il Rockall Trough. La soglia è circa 600 m nelle fonti datate (ICES 1971); l'overflow è debole, meno di 0.3 Sv. **[M]**
- **Canale Faroe–Shetland** (FSC): più di 1000 m nell'asse, con approfondimento verso NE fino al bacino norvegese (oltre 3000 m). **[M]**
- **Cresta Islanda–Faroe** (IFR): soglie tipiche di circa 400–500 m. **[M]**
- Faroe Bank: cima di circa 100 m. Bill Bailey Bank e Lousy Bank a SW. **[M]**

**Fondo**
- Piattaforma e banchi: sabbia grossolana e ghiaia, fondo duro, bassa perdita al fondo.
- Pendii e canali: fanghi sabbiosi e contouriti.
- Creste: ghiaia o roccia, ripulite dalle correnti di overflow. **[M]** (verificare con EMODnet Geology o dbSEABED)

**Masse d'acqua e profilo del suono**
- Fino a circa 400–500 m: Acqua Atlantica (T 8–10 °C, S circa 35.25).
- Sotto: acqua profonda norvegese e acqua di fondo del FSC, sotto 0 °C (fino a −0.5 °C). Il termoclino profondo a 500–700 m crea un gradiente acustico fortissimo. **[A]** (Hansen & Østerhus; Ocean Sci. 2016)
- Lungo la IFR corre il **fronte Islanda–Faroe**, che separa l'Acqua Atlantica dall'acqua artica e del Mare d'Islanda orientale: è una discontinuità di T e S, quindi un "muro" acustico mobile (meandri e vortici). **[M]**
- **Estate:** superficie 10–12 °C, termoclino stagionale a 30–60 m. SSP indicativo (FSC): 0 m 1494, 50 m 1487, 400 m 1489, 600 m 1468, 900 m 1461 m/s. Ne risultano un canale debole in superficie e un forte gradiente negativo sotto i 450 m: un sottomarino sotto i 600 m è acusticamente separato da chi sta nell'Acqua Atlantica. **[B]**
- **Inverno:** strato mescolato fino a 200–400 m, con duct superficiale efficace (0 m 1483, 300 m 1488, 700 m 1460). **[B]**

**Correnti e maree**
- Fra le isole ci sono correnti di marea forti, oltre 3–5 kt negli stretti interni. **[M]**
- Il ramo atlantico scorre verso NE nel FSC; l'overflow freddo va verso NW nel FBC, a 1–2 kt sul fondo. **[A]**
- Sulla piattaforma ci sono fronti tidali. **[M]**

**Rumore ambiente**
- Traffico moderato: rotte nord-europee e pescherecci faroesi e scozzesi.
- Biologici: capodogli, globicefali, orche.
- In inverno domina il rumore da vento e mare forte (sea state 5–6 frequente). **[M]**

**Meteo e luce**
- Burrasche atlantiche da ottobre a marzo, con vento medio invernale di circa 10–12 m/s e Hs media invernale di 3.5–4.5 m. **[M]**
- Nebbia estiva. **[M]**
- Ore di luce a Tórshavn: circa 5 h al solstizio d'inverno, circa 19–20 h in estate. **[A]** (astronomia)

**Significato operativo**
- Il varco Islanda–Faroe era la rotta principale dei sottomarini della Flotta del Nord verso l'Atlantico. **[A]**
- Sorveglianza SOSUS: NAVFAC Keflavik (dal 1966); il sito di Höfn era collegato via cavo alle Faroe per coprire il GIUK fino al 1996. **[A]** (https://en.wikipedia.org/wiki/SOSUS; https://en.wikipedia.org/wiki/Hofn_Air_Station)
- Catena di scoperta: P-3 da Keflavik, Nimrod da Kinloss, barriere di SSN alleati. **[A]** (Cote, *The Third Battle*)
- Radar NATO di Sornfelli sulle Faroe. **[M]**
- Operation Atrina (1987): 5 Victor III aggirarono il GIUK passando attorno alla Groenlandia proprio per evitare il SOSUS. **[A]** (https://en.wikipedia.org/wiki/Operation_Atrina; USNI Proceedings, ott. 2020)

### A2. Mare di Barents (approcci alla Penisola di Kola)

**Confini consigliati:** 69.2–72.0°N, 31.0°E–39.0°E, circa 168 × 160 nm.
L'area comprende l'ingresso del Golfo di Kola (69.2°N 33.4°E), Severomorsk, l'isola Kildin, la penisola Rybachij a W, Teriberka e il sito del Kursk (69.6°N 37.6°E).
Per includere il fronte polare serve una variante più a nord: 72–76°N, 20–40°E.

**Batimetria**
- Profondità media del Barents circa 230 m; massimo circa 500–600 m nella Bear Island Trough (W). **[A]**
- Davanti a Kola: costa ripida che scende rapidamente a 150–300 m, con depressioni e fosse oltre 300 m. **[M]**
- Il Kursk affondò su circa 108 m. **[A]**
- Il Central Bank e i banchi più a nord sono sotto 100–200 m. **[M]**
- Fronte polare: segue grossomodo l'isobata dei **250 m**, ancorato topograficamente alla Bear Island Trough e alla Hopen Trench (vicino a 77°N 30°E nella sezione Nansen Legacy). **[A]** (Harris et al. 1998; EGUsphere 2023-2864; Ocean Sci. 20, 895, 2024)

**Fondo**
- Fanghi e fanghi sabbiosi glaciomarini nelle depressioni; sabbia e ghiaia sui banchi.
- Roccia e massi lungo la costa del Murman.
- Nel complesso è un fondo misto e riflettente, quindi la propagazione è dominata dal fondo. **[M]**

**Masse d'acqua e profilo del suono**
- La Corrente Costiera del Murman (più dolce) e la Corrente di Capo Nord (atlantica, 3–7 °C) mantengono **senza ghiaccio** tutta la costa di Kola, inclusa Murmansk, tutto l'anno. **[A]**
- Il bordo del ghiaccio d'inverno sta molto più a NE, a circa 74–77°N. **[M]**
- **Inverno:** colonna mescolata fino al fondo (2–4 °C). Il profilo è debolmente crescente con la profondità per effetto della pressione (0 m 1459, 250 m 1466). Ne derivano un duct dal fondo alla superficie con molti rimbalzi e portate buone a bassa frequenza, ma con forti perdite sul fondo. **[B]**
- **Estate:** strato caldo e meno salato di 20–40 m (6–10 °C), gradiente negativo (0 m 1485, 60 m 1467, 250 m 1464). Ne derivano zona d'ombra superficiale e propagazione via rimbalzo sul fondo. **[B]**
- Nella zona frontale, nei mesi freddi, entrambi i lati sono verticalmente mescolati; sotto i 100 m il fronte è stabile. **[A]** (Ocean Sci. 2024)

**Correnti e maree**
- Marea semidiurna di circa 1–3 m sulla costa di Kola. **[M]**
- Correnti costiere verso E di 0.5–1 kt. **[M]**

**Rumore ambiente**
- Traffico navale sovietico intenso (Severomorsk, Polyarny, Gadzhievo), flottiglie da pesca di Murmansk, rompighiaccio.
- Ghiaccio solo al bordo: rumore impulsivo da rottura, oppure ambiente molto silenzioso sotto banchisa compatta. **[M]**

**Meteo e luce**
- **Notte polare** a Murmansk dal 2 dicembre all'11 gennaio circa; sole di mezzanotte da fine maggio a fine luglio. **[A]**
- Da novembre a marzo: polar low (cicloni mesoscala con venti oltre 25 m/s), nevicate e ghiaccio sulle sovrastrutture. **[M]**
- Estate: nebbia frequente. **[M]**

**Significato operativo**
- Cuore della Flotta del Nord: Severomorsk (comando), Zapadnaya Litsa (base degli **Alfa**, Project 705, e dei Victor), Gadzhievo e Gremikha (SSBN). **[A]** (Polmar & Moore 2004)
- Per l'intelligence occidentale era il **"bastione"** degli SSBN Delta e Typhoon. **[A]** (CIA, *Soviet Navy Intelligence and Analysis During the Cold War*, 2017, pp. 26–27: i Delta con SS-N-8 non avevano più bisogno di superare le barriere ASW alleate; https://www.cia.gov/static/Soviet-Navy-Intelligence-and-Analysis-During-the-Cold-War.pdf)
- Operazioni avanzate di SSN americani per raccolta informazioni e pedinamento (Holystone e simili). **[A]** (Sontag & Drew 1998)
- Collisioni da pedinamento ravvicinato in acque basse:
  - USS Gato con K-19, 15 nov. 1969, al largo di Kola **[A]**;
  - USS Baton Rouge con K-276 "Kostroma", 11 feb. 1992, vicino a Kildin, fuori periodo ma indicativo **[A]**;
  - USS Grayling con K-407, 20 mar. 1993 **[A]**.

### A3. Stretto di Gibilterra (con Mare di Alborán e Golfo di Cadice)

**Confini consigliati:** 35.0–36.8°N, 7.5°W–3.5°W, circa 108 × 194 nm.
La mappa è allungata E–W perché le missioni 4 e 5 si giocano in uscita verso ovest.
Comprende il Golfo di Cadice orientale, lo stretto (Tarifa a 36.0°N 5.6°W), Ceuta e Algeciras, l'Alborán occidentale e l'isola di Alborán (35.94°N 3.03°W, appena fuori: estendere a 3.0°W se serve l'ancoraggio sovietico).

**Batimetria**
- **Soglia di Camarinal: 284 m**, il punto minimo dello stretto. **[A]** (Sannino, Pratt & Carrillo 2009, J. Phys. Oceanogr., https://www2.whoi.edu/staff/lpratt/wp-content/uploads/sites/120/2019/07/JPO_Sannino_Pratt_Carillo_2009_55743.pdf)
- **Soglia di Espartel:** circa 360 m nel canale sud; il canale nord arriva a circa 250 m. **[A]** (ibid.)
- **Stretto di Tarifa** (a E di Camarinal): oltre 800 m, largo circa 14 km. **[A]** (profondità) / **[M]** (larghezza)
- Alborán occidentale: 1000–1500 m, con dorsale di Alborán. Golfo di Cadice: scarpata e canali contouritici fino a oltre 1000 m. **[M]**

**Fondo**
- Soglie: roccia, ghiaia e sabbia grossolana, ripulite da correnti di marea di diversi nodi.
- Alborán: fanghi. Golfo di Cadice: sabbie e fanghi contouritici. **[M]**

**Masse d'acqua e profilo del suono**
- Scambio a **due strati**: in superficie entra Acqua Atlantica (S circa 36.2–36.4) verso E; sotto esce l'**Acqua Mediterranea** (S circa 38.4, T circa 13 °C) verso W. **[A]**
- Lo scambio è controllato idraulicamente a Camarinal e a Espartel. Sul fianco ovest di Camarinal l'outflow diventa supercritico, con salto idraulico nel bacino di Tangeri. **[A]** (Farmer & Armi 1988; arXiv 2311.01377)
- Interfaccia a circa 100–150 m, con oscillazioni tidali di decine di metri. **[M]**
- **Onde interne** di ampiezza **oltre 100 m**, generate a Camarinal dalla marea semidiurna, si propagano verso E nell'Alborán. Per il gioco: fluttuazioni rapide della perdita di trasmissione e "strati" mobili. **[A]** (Sánchez-Garrido et al. 2011, https://www2.whoi.edu/staff/lpratt/wp-content/uploads/sites/120/2019/07/Sanchez-Garrido_et_al-2011_93504.pdf)
- **Profilo mediterraneo:** sotto i 150–300 m l'acqua è quasi isoterma (circa 13 °C), quindi la velocità del suono **cresce con la profondità**. Non c'è un minimo profondo atlantico: l'asse del canale è basso, a circa 100–200 m.
  - Estate: 0 m 1526, 100 m 1508, 150 m 1507, 800 m 1518. Il forte gradiente negativo superficiale crea una zona d'ombra; conviene mettere l'asse del canale a circa 150 m. **[B]**
  - Inverno: quasi isoveloce in superficie (1510), poi crescente.
- **Golfo di Cadice:** la Mediterranean Outflow Water scende a **1000–1200 m** e crea un massimo relativo di velocità del suono, più le "meddies" (lenti calde e salate). Nel calcolo indicativo: 500 m 1506, 1000 m 1513, 1500 m 1508. Ne risultano un canale doppio e una deviazione delle zone di convergenza. **[M]** (fenomeno) / **[B]** (valori)

**Correnti**
- Getto atlantico verso l'Alborán di 1–2 m/s (2–4 kt). Correnti di marea nello stretto fino a 2–3 m/s sulla soglia. Outflow profondo di circa 0.8 Sv verso W. **[M]**
- **Regola tattica storica:** nella Seconda guerra mondiale gli U-boot entravano in Mediterraneo in immersione, a motori quasi fermi, trasportati dalla corrente superficiale verso E. Per uscire verso W conviene stare **sotto l'interfaccia (oltre 150–200 m)**, nell'outflow. **[A]** (fatto storico WWII, noto) / **[M]** (applicazione al 1985–91)
- Analogo documentato: i sottomarini sovietici spegnevano i motori prima delle stazioni giapponesi nello stretto di Tsushima e si lasciavano portare dalla corrente. **[M]** (War on the Rocks, https://warontherocks.com/russian-pacific-fleet-redux-japans-north-as-a-new-center-of-gravity/)

**Rumore ambiente**
- Fra i più alti al mondo: dell'ordine di 100.000 transiti l'anno (circa 300 al giorno), più traghetti Algeciras–Tangeri e Tarifa–Tangeri. **[M]**
- Biologici: delfini, globicefali, capodogli. **[M]**
- Il traffico consente il "masking" sotto la scia di mercantili (tattica plausibile, IA). **[B]**

**Meteo**
- Regimi **Levante** (da E, incanalato, 30–40 kt, con nebbie e nubi di Levante in estate) e **Poniente** (da W). Mare corto e ripido con vento contro corrente. **[M]**

**Significato operativo**
- Chokepoint NATO (comando GIBMED); base USA di Rota, sito SSBN fino al 1979. **[A]**/**[M]**
- Ancoraggio della 5ª Eskadra sovietica presso l'isola di Alborán. **[M]** (fonte amatoriale: https://robertsspaceindustries.com/orgs/USSRRFOS5, da verificare; la 5ª Eskadra in generale: https://nuke.fas.org/guide/russia/agency/mf-med.htm)
- Contesto libico: El Dorado Canyon (aprile 1986). Le petroliere libiche del gioco sono finzione. **[A]**

### A4. Imboccatura dell'Adriatico (Canale d'Otranto)

**Confini consigliati:** 39.5–42.5°N, 17.0°E–20.5°E, circa 180 × 160 nm.
Comprende Taranto (appena a W), Brindisi, Otranto, Valona/Karaburun, Corfù e la Fossa Sud-Adriatica fino alle Bocche di Cattaro e Tivat (42.43°N 18.70°E), destinazione dei mercantili della missione 6.

**Batimetria**
- **Soglia di Otranto:** circa **780 m**; altre fonti danno da 740 a 800 m, fino a 850–935 m secondo la linea di misura. **[A]** per l'ordine di grandezza, **[M]** per il valore esatto (Frontiers Mar. Sci. 2025, doi 10.3389/fmars.2025.1516780; EGU2010-9392)
- Larghezza 70–80 km, 72 km secondo Springer. **[A]**
- **Fossa Sud-Adriatica:** circa 1200–1230 m. **[M]**
- Sistema di canyon di Bari (circa 41.3°N 17.3°E). **[M]**
- Piattaforma pugliese stretta; piattaforma albanese più ampia e bassa. A S il Mar Ionio scende rapidamente a 2000–3000 m. **[M]**

**Fondo:** fanghi e argille siltose quasi ovunque (fondo assorbente, alta perdita al fondo); sabbia sulle piattaforme; roccia a Karaburun e Saseno. **[M]**

**Masse d'acqua e profilo del suono**
- Entrata lungo il lato E (albanese) dell'**Acqua Levantina Intermedia** (LIW) a 200–600 m, calda e salata (circa 14–15 °C, S circa 38.75). **[A]** (CIESM monografia CSS-2)
- Uscita dell'**Acqua Densa Adriatica** sul fondo lungo il margine W (italiano), con massimo in primavera e secondario in autunno. **[A]** (https://ciesm.org/online/monographs/CSS-2/CSS_2_117_145.pdf)
- **Estate:** SST 24–26 °C, termoclino a 20–50 m, gradiente negativo violento (0 m 1538, 60 m 1515, 200 m 1513, 800 m 1519). Ne derivano portate dirette corte e un ottimo nascondiglio sotto lo strato. **[B]**
- **Inverno:** mescolamento a 100–200 m. Nella Fossa Sud-Adriatica la convezione profonda arriva a 600–800 m e oltre negli inverni con Bora. Profilo quasi isoveloce poi crescente (0 m 1508, 800 m 1519). **[A]** (convezione) / **[B]** (valori)

**Correnti e maree**
- Entrata sul lato E, uscita sul lato W, a 0.2–0.5 kt. **[M]**
- Marea microtidale (10–20 cm). **[A]**

**Rumore ambiente:** traffico medio-alto (Brindisi e Bari verso la Grecia; l'Albania era chiusa negli anni '80), pesca a strascico italiana, rumore da pioggia con Scirocco. **[M]**

**Meteo:** **Bora** (NE, invernale, raffiche oltre 50 kt nel N Adriatico, più debole a Otranto) e **Scirocco** (SE, mare lungo e pioggia). Estate calma e afosa, visibilità buona. **[M]**

**Significato operativo**
- Basi italiane a Taranto e Brindisi. Marina jugoslava a Tivat e Cattaro (Boka) e a Lora (Spalato). **[A]**
- Pasha Liman (Saseno/Valona) fu base sovietica di Whiskey dal 1958 al 1961; dopo la rottura l'Albania trattenne 4 battelli. **[A]**
- Il blocco sovietico della Jugoslavia nella missione 6 è finzione del 1989.
- Precedente storico: lo sbarramento del Canale d'Otranto (1915–18). **[A]**

### A5. Passo di Kunashir / Mare di Okhotsk (Curili meridionali)

**Confini consigliati:** 43.3–45.6°N, 144.8°E–148.5°E, circa 138 × 158 nm.
Comprende Kunashir, la punta SW di Iturup, lo stretto di Ekaterina (fra Kunashir e Iturup), lo stretto di Nemuro/Kunashirskij e lo stretto di Izmeny verso Hokkaido, Shikotan e le Habomai, il bordo S del Bacino delle Curili (Okhotsk) e la fossa Curili-Kamchatka a SE.
Per il contesto di La Pérouse/Sōya serve una mappa separata: 44.5–46.5°N, 140.5–143.5°E.

**Batimetria**
- Stretti delle Curili: larghezza da 1.8 km (Secondo Stretto) a 55 km (Kruzenshtern); profondità prevalenti fino a 500 m. **[A]** (BRE, https://bigenc.ru/c/kuril-skie-prolivy-c67205)
- Soli due stretti profondi: **Bussol' circa 2200 m** e **Kruzenshtern circa 1900 m**, entrambi più a NE, fuori mappa. **[A]** (Ohshima et al., JGR 107, 2002; Kashiwai PICES 2018)
- Stretto di Ekaterina: profondità non trovata in questa ricerca. È probabilmente nell'ordine di centinaia di metri, quindi transitabile in immersione: **verificare con GEBCO/JODC [B]**.
- Stretto di Nemuro: massimo citato 2500 m ma, all'altezza della penisola di Notsuke, **meno di 20 m**: il ramo N–S verso Hokkaido è chiuso ai sottomarini. **[M]** (https://en.wikipedia.org/wiki/Nemuro_Strait; fonti contrastanti)
- Bacino delle Curili oltre 3000 m (circa 3300 m). **[M]**
- Fossa Curili-Kamchatka: oltre 9000 m (circa 9600 m). **[M]**

**Fondo:** roccia vulcanica e ghiaia negli stretti, sabbie vulcaniche sulle piattaforme, fanghi diatomitici nel bacino di Okhotsk. **[M]**

**Masse d'acqua e profilo del suono**
- Lato Pacifico: Oyashio freddo. Lato Okhotsk: **Corrente Calda di Sōya**, calda e salata, che d'estate e d'autunno corre da La Pérouse lungo la costa nord di Hokkaido fino a Kunashir. Ne risultano fronti acustici marcati. **[M]**
- Nell'Okhotsk c'è uno **strato dicotermico** (minimo di T, circa −1.5/+1 °C, a 50–150 m) residuo dell'inverno. Crea un **canale sonoro superficiale molto marcato** con asse a circa 80–100 m. Profilo estivo indicativo: 0 m 1494, 30 m 1473, 80 m 1448, 150 m 1454, 500 m 1466, 1500 m 1482. **[M]** (fenomeno) / **[B]** (valori)
- **Inverno:** ghiaccio marino dell'Okhotsk da gennaio ad aprile; raggiunge la costa nord di Hokkaido e lo stretto di Nemuro a febbraio–marzo. Il lato Pacifico resta libero. **[M]**

**Correnti e maree:** correnti di marea forti negli stretti, **2–12 km/h (circa 1–6.5 kt)**. **[A]** (BRE) Maree interne intense. **[M]**

**Rumore ambiente:** pesca intensa (salmone, granchio, saury; flotte giapponesi e sovietiche), cetacei, rumore del ghiaccio in inverno e primavera (frattura, attrito di lastre). **[M]**

**Meteo e luce**
- **Nebbia da avvezione** d'estate (giugno–agosto) molto frequente; visibilità spesso sotto 1 nm. **[M]**
- Inverno: venti da NW freddi, ghiaccio sulle sovrastrutture. **[M]**
- Luce a dicembre circa 9 h. **[A]**

**Significato operativo**
- L'Okhotsk era il **bastione SSBN della Flotta del Pacifico**: Delta da Rybachij (Kamchatka) e dalla Primorye. **[A]** (CIA 2017, pp. 26–27; https://ussnautilus.org/cold-war-cat-and-mouse-part-iii/)
- Nel 1978 l'URSS rimilitarizzò le Curili meridionali con una divisione e missili antinave e contraerei. **[M]**
- **Ivy Bells:** il pod d'intercettazione del cavo sottomarino nell'Okhotsk, posato dall'USS Halibut nel 1971 e tradito da R. Pelton (venduto nel 1980, recuperato dai sovietici nel 1981). **[A]** (Sontag & Drew 1998, capp. 8–9)
- Abbattimento del KAL 007 (1983) presso Sakhalin: contesto di tensione. **[A]**
- Lo stretto di Sōya era sorvegliato dal Giappone (radar di Wakkanai, P-3 da Hachinohe e Misawa). **[M]**

### A6. Margine della Piattaforma Celtica (Western Approaches)

**Confini consigliati:** 47.0–49.5°N, 12.0°W–7.5°W, circa 150 × 180 nm.
Comprende la piattaforma celtica esterna (Great Sole e Little Sole Bank), il ciglio a circa 200 m, i canyon (Whittard, Shamrock, Black Mud, Petite e Grande Sole), il Goban Spur a NW e la Piana Abissale di Porcupine e del Biscaglia (4000–4800 m).

**Batimetria**
- Piattaforma 100–180 m, con creste sabbiose tidali lunghe decine di km (Celtic Sea sand ridges). **[M]**
- Ciglio a circa 200 m. Scarpata fino a oltre 4000 m in 50–100 km, incisa da canyon dendritici: il **Whittard Canyon** convoglia e intensifica sul fondo l'energia di marea interna. **[A]** (Aslam et al., UEA, https://ueaeprints.uea.ac.uk/65159)
- Per rappresentare la marea interna semidiurna servono dati batimetrici ad almeno 1.5 km di risoluzione. **[A]** (ibid.)

**Fondo:** piattaforma in sabbia e sabbia ghiaiosa (bassa perdita, forte riverbero); scarpata in fanghi; pareti dei canyon in roccia e fango consolidato. **[M]**

**Masse d'acqua e profilo del suono**
- **Estate** (da aprile a ottobre): piattaforma fortemente stratificata, superficie 16–18 °C e "cold pool" a 10–11 °C sotto un termoclino a 30–50 m (0 m 1513, 40 m 1495, 120 m 1494). Effetto: zona d'ombra a breve distanza e sottomarino invisibile sotto il termoclino a chi pinga da superficie. **[M]**
- **Inverno:** piattaforma mescolata fino al fondo (circa 10.5 °C, isoveloce intorno a 1492–1494). **[M]**
- **Onde interne e solitoni** al ciglio: il fascio di marea interna rimbalza sul picnoclino stagionale e genera pacchetti di onde solitarie al largo. Effetto: fluttuazioni della perdita di trasmissione di diversi dB su scale di decine di minuti. **[A]** (Vlasenko & Stashchuk 2015, https://pearl.plymouth.ac.uk/handle/10026.1/4404)
- Al largo: acque centrali del NE Atlantico e **lingua di MOW a circa 1000 m** (massimo di salinità), con asse SOFAR a circa 1000–1500 m (500 m 1502, 1000 m 1505, 2000 m 1500, 4000 m 1528). In acqua profonda sono possibili **zone di convergenza** (circa 30–35 nm). Il passaggio scarpata–piattaforma "accoppia" o "spegne" le zone di convergenza. **[M]** (fenomeno) / **[B]** (valori)

**Correnti:** marea forte in piattaforma (1–2 kt); corrente di pendio verso il polo. **[M]**

**Rumore ambiente:** traffico molto alto (Western Approaches, schema di separazione di Ouessant), flotte da pesca francesi e spagnole al ciglio (nasello), alta densità di delfini comuni, pioggia. **[M]**

**Meteo:** burrasche atlantiche in inverno (Hs media invernale 3–4 m, estremi oltre 10 m); estate relativamente calma. **[M]** Luce: dicembre circa 8.5 h, giugno circa 16 h. **[A]**

**Significato operativo**
- Linee di comunicazione USA–Europa (convogli REFORGER). **[A]**
- NAVFAC Brawdy (Galles) del SOSUS. **[A]** per l'esistenza (https://www.globalsecurity.org/intell/systems/sosus.htm); date 1974–1995 **[M]**
- Nimrod da St Mawgan e Kinloss; Atlantic francesi da Lann-Bihoué. **[M]**
- Uscite degli SSBN francesi dell'Île Longue (Brest), sorvegliate da SSN sovietici. **[M]**
- Missione 9: imboscata di un Oscar SSGN e un Alfa a un convoglio. È plausibile come dottrina: la CIA stimava che pochi SSN sovietici sarebbero stati destinati alle linee di comunicazione. **[M]** (CIA 2017, IIM 1981 *Soviet Intentions and Capabilities for Interdicting SLOCs*)

### A7. Mar Baltico (uscita verso gli stretti danesi)

**Confini consigliati:** 54.0–55.8°N, 10.5°E–15.0°E, circa 108 × 155 nm.
Comprende la costa della RDT (Rügen, Warnemünde e Sassnitz), il Bacino di Arkona, la soglia di Darss, la Kadetrinne, la Fehmarn Belt, l'ingresso del Grande Belt e dell'Øresund, e Bornholm sul margine E.

**Batimetria (vincolo critico)**
- **Soglia di Drogden** (Øresund): **7–9 m**. **[A]**
- **Soglia di Darss:** **18–19 m**, piana sabbiosa. **[A]** (https://en.wikipedia.org/wiki/Baltic_Sea; idw-online.de/de/news626770)
- Kadetrinne: canale fino a circa 25–30 m. **[M]**
- Grande Belt: soglie di circa 17–20 m sulle rotte profonde. **[M]**
- Bacino di Arkona: massimo circa 50 m. Bacino di Bornholm: circa 100 m. Fossa del Gotland 249 m; Fossa di Landsort 459 m, fuori mappa. **[A]**
- **Conseguenza di design:** un 688 ha un pescaggio di circa 9.5 m e un'altezza totale dello scafo con la vela di circa 15–16 m. Per stare in quota periscopica con margine dal fondo servono almeno 25–30 m. L'uscita "furtiva" va quindi modellata come navigazione a quota periscopica o in superficie lungo la Kadetrinne e il Grande Belt, oppure il remake deve dichiarare la licenza poetica. **[A]** (dati di quota) / **[M]** (dimensioni del 688, da confrontare col dossier piattaforme)

**Fondo:** sabbia, till glaciale e massi erratici (forte riverbero e rischio di urto); fango nei bacini di Arkona e Bornholm. **[M]**

**Masse d'acqua e profilo del suono**
- In superficie acqua salmastra, con S 7–8 psu; sotto l'**alocline**, acqua di afflusso salina a 15–20 psu.
- Profondità dell'alocline: circa 40–50 m ad Arkona e Bornholm, 60–80 m nel Gotland. **[M]**; la cifra 60–80 m non è stata verificata in questa sessione **[B]**.
- **Estate:** termoclino a 10–25 m (superficie 17–20 °C) sopra uno **strato intermedio freddo** (2–5 °C, acqua invernale residua). Per effetto della salinità sotto l'alocline la velocità del suono risale, quindi si forma un **canale sonoro a circa 30–50 m** nei bacini (0 m 1484, 30 m 1436, 50 m 1433, 90 m 1452). È un nascondiglio forte, ma solo dove il fondo è profondo abbastanza. **[M]** (fenomeno) / **[B]** (valori)
- **Inverno:** acqua fredda fino all'alocline, che resta l'unica discontinuità. **[M]**
- **Ghiaccio:** nel Baltico meridionale è raro, ma ci furono inverni severi nel periodo (1985 e 1987 con ghiaccio esteso). **[M]**

**Correnti e maree:** maree trascurabili. Le correnti indotte dal vento negli stretti arrivano a 1–3 kt, con inversioni rapide. Ci sono afflussi salini maggiori occasionali. **[M]**

**Rumore ambiente:** traffico molto alto (traghetti Rostock–Gedser e Sassnitz–Trelleborg), pesca; poca biologia. L'acqua bassa limita le portate e alza il riverbero. **[M]**

**Meteo e luce:** burrasche da W e SW in inverno, con mare corto (Hs 1–3 m); nebbia primaverile. Luce: dicembre circa 7 h, giugno circa 17 h. **[A]**

**Significato operativo**
- Patto di Varsavia: Flotta del Baltico sovietica (Baltijsk), Volksmarine (Peenemünde, Warnemünde, Sassnitz), Marina polacca (Gdynia). **[A]**
- NATO (comando BALTAP): marine danese e tedesca (Kiel, Eckernförde con U-Boot Type 205/206), mine pianificate negli stretti. **[A]**
- Incidenti in Svezia: U 137, il Whiskey incagliato presso Karlskrona nel 1981, e Hårsfjärden (1982). **[A]**
- Gli SSN americani di fatto non operavano nel Baltico. **[M]**

---

## PARTE B — Fonti dati per costruire le mappe

### B1. Batimetria

| Dataset | Risoluzione | Copertura | Licenza | Accesso |
|---|---|---|---|---|
| **GEBCO_2025** (esiste anche GEBCO_2026) | 15″ (circa 460 m alla latitudine dell'equatore), griglia 43200×86400 | globale, con griglie TID di qualità/provenienza | pubblico dominio con attribuzione richiesta **[M]** | globale netCDF; ritagli netCDF, GeoTIFF o ESRI ASCII da https://download.gebco.net/ ; https://www.gebco.net/data-products-gridded-bathymetry-data/gebco2025-grid **[A]** |
| **ETOPO 2022** (NOAA NCEI) | 15″, 30″, 60″; versioni bedrock e ice surface | globale | dati del governo USA, uso libero **[A]** | DOI 10.25921/fd45-gt74; ERDDAP `ETOPO_2022_v1_15s` (dimensioni latitude, longitude; variabile `z`) **[A]** |
| **EMODnet Bathymetry DTM 2024** | 1/16′ (circa 115 m), riferito al LAT | mari europei, compresi Faroe, Barents meridionale, Mediterraneo, Celtico e Baltico | CC-BY 4.0 **[M]** | 59 tile (ESRI ASCII, XYZ, NetCDF CF, GeoTIFF) da https://emodnet.ec.europa.eu/en/bathymetry ; servizi OGC WMS/WCS, EPSG:4326 e 3857 **[A]**; endpoint WCS probabile `https://ows.emodnet-bathymetry.eu/wcs`, copertura media `emodnet:mean` **[B]** |
| **IBCAO v5.2** | 100/200/400 m, polare stereografica EPSG:3996 | a N di 64°N: copre il Barents, non le Faroe | CC-BY **[M]** | https://www.gebco.net/data-products/gridded-bathymetry-data/arctic-ocean (100 m circa 12.6 GB in GeoTIFF o 16 tile) **[A]**; riferimento Jakobsson et al. 2024, Sci Data 11:1420 |
| **Baltic Sea Bathymetry Database (BSBD)** | 500 m | Baltico | CC-BY-NC secondo le presentazioni del 2013; HELCOM dice "nessuna limitazione" **[B]** | http://data.bshc.pro ; metadati HELCOM. Nota: EMODnet 2024 è più fine e contiene questi dati, quindi **conviene usare EMODnet** |
| **JODC J-EGG500 / JTOPO30** | 500 m / 30″ | Giappone e Curili meridionali | J-EGG500 gratuito dal sito JODC con registrazione **[M]**; JTOPO30 distribuito da MIRC/JHA, probabilmente a pagamento **[B]** | https://www.jodc.go.jp ; per le Curili GEBCO è sufficiente |

### B2. Temperatura e salinità, quindi velocità del suono

- **NOAA World Ocean Atlas 2023**: griglie a 0.25°, 1° e 5°; periodo `decav` (media delle decadi 1955–2022) o normali climatologiche 1971–2000, 1981–2010, 1991–2020. **[A]**
  - File mensili per la temperatura a 0.25°: `woa23_decav_t01_04.nc` … `t12_04.nc`; stagionali `t13–t16`; annuale `t00`. Per la salinità `s01_04` e seguenti. Variabili `t_an` e `s_an`. **[A]** (catalogo THREDDS https://www.ncei.noaa.gov/thredds-ocean/catalog/woa23/DATA/temperature/netcdf/decav/0.25/catalog.html)
  - HTTP: `https://www.ncei.noaa.gov/data/oceans/woa/WOA23/DATA/temperature/netcdf/decav/0.25/woa23_decav_t01_04.nc` **[M]**
  - OPeNDAP: `https://www.ncei.noaa.gov/thredds-ocean/dodsC/woa23/DATA/temperature/netcdf/decav/0.25/woa23_decav_t01_04.nc` **[M]** (schema THREDDS standard)
  - I campi mensili arrivano fino a 1500 m, gli stagionali fino a 5500 m **[M]**. Per i canali profondi (Celtico al largo, Kuril) unire i mensili sopra i 1500 m con gli stagionali sotto.
  - Esiste anche ERDDAP AOML (es. `WOA_TEMP_d48d_916b_3478` per dicembre a 0.25°), ma è parziale (la salinità solo fino a 500 m): **non usarlo per profili completi**. **[A]**
- Per **anni specifici 1985–1991** (missioni datate): Met Office **EN4** (analisi mensili a 1°, dal 1900) oppure i profili grezzi del **World Ocean Database** filtrati per periodo. **[M]**
- **Copernicus Marine**, solo dal 1993: GLORYS12 (`GLOBAL_MULTIYEAR_PHY_001_030`), più le rianalisi regionali MED, IBI, NWS, BAL e ARC MULTIYEAR. Si scaricano con `copernicusmarine subset --dataset-id … -x/-X/-y/-Y/-z/-Z` (toolbox Python, account gratuito). Utile per variabilità e fronti a 1/12°, non per gli anni della Guerra fredda. **[M]**
- Conversione: TEOS-10 (`gsw`) oppure Mackenzie o Chen-Millero, a cura del dossier di fisica.

### B3. Sedimenti, quindi classi di perdita al fondo

- **Dutkiewicz et al. 2015**, *Geology* 43(9), doi 10.1130/G36883.1: 13 litologie da circa 14.500 campioni, interpolate con SVM. Dati e notebook su EarthByte (https://www.earthbyte.org, sezione "Seafloor lithology"; ftp.earthbyte.org/papers/Dutkiewicz_etal_seafloor_lithology/). Copre solo i bacini profondi, **non le piattaforme**. **[A]** (contenuto) / **[M]** (stato del link)
- **NOAA NCEI deck41**: descrizioni dei sedimenti superficiali. **dbSEABED** (INSTAAR): granulometria in punti, anche su piattaforma. **[M]**
- **EMODnet Geology – Seabed substrate** (classi Folk, fino a 1:250k): la scelta migliore per Faroe, Celtico, Baltico e Adriatico. **[M]**
- Mappatura consigliata verso le classi geoacustiche: roccia, ghiaia, sabbia, silt, argilla.

### B4. Meteo, mare e ghiaccio (climatologia 1985–1991)

- **ERA5** (dal 1940), dataset `reanalysis-era5-single-levels-monthly-means` sul CDS (https://cds.climate.copernicus.eu, `cdsapi`). Variabili: `10m_u/v_component_of_wind`, `significant_height_of_combined_wind_waves_and_swell`, `mean_wave_period`, `sea_ice_cover`, `total_cloud_cover`. Licenza Copernicus (CC-BY). **[M]**
- **HadISST1** (ghiaccio e SST mensili a 1°) o **NSIDC** (dal 1979) per i bordi del ghiaccio nel Barents e nell'Okhotsk. **[M]**
- Nebbia e visibilità: **ICOADS** (osservazioni da nave) per le climatologie per box. **[M]**
- Ore di luce e notte polare: calcolo astronomico in motore (NOAA Solar Calculator), nessun dataset.

### B5. Linee di costa

- **GSHHG 2.3.7** (risoluzione "full", licenza LGPL) per poligoni consistenti. **[A]**
- **Natural Earth** 1:10m (pubblico dominio) per la UI. **[A]**
- **OSM land polygons** (ODbL) se serve dettaglio portuale. **[M]**

### B6. Pipeline consigliata

1. **Definizione dei teatri** in YAML: id, bbox, mesi giocabili, proiezione locale (azimutale equidistante centrata, oppure UTM), cella di 150–250 m.
2. **Batimetria:** EMODnet 2024 per i teatri europei e Barents meridionale; GEBCO o ETOPO per le Curili (da IBCAO solo se servono i dati polari più a nord).
   - Esempio ERDDAP:
     `https://coastwatch.pfeg.noaa.gov/erddap/griddap/ETOPO_2022_v1_15s.nc?z[(60.5):1:(63.5)][(-10.0):1:(-4.0)]` (circa 720 × 1440 celle)
   - Poi `gdalwarp` verso la proiezione locale. Esportare un heightmap a 16 bit (PNG o GeoTIFF; scala 0.25 m per unità, offset −11000 m) e un raster della classe di fondo a 8 bit.
3. **Profili:** per ogni teatro e mese, ritagliare WOA23 a 0.25° (OPeNDAP con `xarray.open_dataset(url)[['t_an']].sel(lat=…, lon=…)`), calcolare la velocità del suono con `gsw`, estrapolare verso il fondo locale ed esportare JSON `{lat, lon, depth[], c[]}`. Opzionalmente aggiungere perturbazioni sintetiche: fronti, onde interne a Gibilterra e al ciglio celtico, meddies.
4. **Campi ambiente:** da ERA5 mensile, distribuzioni di vento, Hs e ghiaccio per teatro e mese (percentili 10, 50 e 90) in JSON, da cui il motore estrae sea state, ghiaccio e visibilità.
5. **Sedimenti:** rasterizzazione di EMODnet substrate e Dutkiewicz sulla stessa griglia della batimetria.
6. **Costa:** GSHHG ritagliato in GeoJSON.
7. Tutto in **Zarr o netCDF** per gli strumenti e in **PNG16 + JSON** per il runtime. Le tabelle di perdita di trasmissione precalcolate (BELLHOP o RAM) sono a carico del dossier di fisica.

---

## PARTE C — Letteratura storico-dottrinale e comportamenti estraibili

### C1. Fonti governative declassificate

- **CIA, NIE 11-15-82/D** e **NIE 11-15-84/D**, *Soviet Naval Strategy and Programs Through the 1990s* (marzo 1983 e marzo 1985). Reading Room: doc. 0000268225 (82/D) e 0000262011 (84/D), più una versione CREST CIA-RDP87T00126R000600700004-3. **[A]** (metadati)
  - Indice: protezione della forza SSBN e ASW strategica. Rimanda a **SNIE 11-20-84/D *Soviet Submarine Warfare Trends*** (marzo 1985). **[A]** (rimando) / **[B]** (contenuto non letto)
- **CIA Historical Collection** *CIA Analysis of the Soviet Navy* (2017; 82 documenti, circa 2000 pagine) e opuscolo *Soviet Navy Intelligence and Analysis During the Cold War*, https://www.cia.gov/static/Soviet-Navy-Intelligence-and-Analysis-During-the-Cold-War.pdf **[A]**. Punti estratti:
  - Fine anni '70: speculazione occidentale su una "bastion strategy". Kresta II, Kara e Krivak erano navi **ASW contro SSN americani**, inizialmente scambiate per navi antinave (pp. 26–27). **[A]**
  - **Saakyan, *Military Thought* 1975**, "operazione per distruggere gli SSBN nemici" (p. 49 circa). Prima fase: **ricerca massiva congiunta** di SSN, sottomarini diesel, aerei ASW a lungo raggio e gruppi hunter-killer di superficie con elicotteri, armati di siluri, mine, cariche di profondità e testate "speciali". Seconda fase: impiego di missili strategici contro SSBN localizzati e stretti lontani. **[A]**
  - **Combat Regulations of the Soviet Navy** (1983, traduzione CIA del 14 nov. 1986, doc. 0001430734):
    - cap. 7: le forze ASW (navi, sottomarini, aerei, elicotteri) coordinano le azioni contro SSBN e forze ASW nemiche in oceano aperto e nei mari costieri;
    - cap. 8: gruppi di sottomarini "ricognizione-attacco" e gruppi d'attacco di superficie contro portaerei;
    - cap. 10: difesa delle aree di basamento e protezione di sottomarini e navi **in uscita e in ingresso** dalle basi. Questo corrisponde esattamente alla missione 3 "Sub Ops". **[A]** (sintesi CIA) / **[B]** (testo completo non letto: il sito CIA blocca il recupero automatico)
- **US Navy, *The Maritime Strategy*** (versioni 1982–1986; pubblica: Watkins, *USNI Proceedings*, suppl. gennaio 1986). Documenti in **Hattendorf & Swartz, *U.S. Naval Strategy in the 1980s: Selected Documents*, Newport Paper 33 (2008)**: https://digital-commons.usnwc.edu/usnwc-newport-papers/33/ (DTIC ADA522552). Storia: **Hattendorf, *The Evolution of the U.S. Navy's Maritime Strategy 1977–1986*, Newport Paper 19 (2004)**, DTIC ADA422147. **[A]**
  - Fasi: (I) deterrenza e transizione alla guerra; (II) presa dell'iniziativa; (III) portare la lotta al nemico.
  - Gli SSN sono schierati avanti già nella fase I, nel Mar di Norvegia e nel Barents, per logorare gli SSN sovietici e **minacciare gli SSBN**, alterando la correlazione delle forze nucleari. **[A]**
- **Ketron/ONR, R. W. Herrick**, serie *Soviet Naval Mission Assignments* (1979–80). Parte II *Pro-SSBN Mission* (DTIC ADB082798); parte III *A Protracted Withholding Role for Soviet SSBNs?* (ADB082799). **[A]**
- **CNA, J. D. Perse, CRM 84-29** (maggio 1986), sulla politica dichiaratoria USA sulla sicurezza degli SSBN sovietici e la sua percezione in URSS. DTIC ADA175532. **[A]**
- **Critica al concetto di bastione:** articolo della Naval Submarine League (https://archive.navalsubleague.org/?p=1589). Sostiene che la letteratura sovietica non parla esplicitamente di "bastioni"; il termine sovietico era "stabilità di combattimento" degli SSBN (боевая устойчивость). **[M]**
- **RAND:** in questa sessione non ho trovato un rapporto RAND specifico sull'ASW sovietica. RAND P-7388 *The Soviet Presence in the Mediterranean* (https://www.rand.org/pubs/papers/P7388.html) è utile per i teatri 3–4. **[A]**

### C2. Saggistica e storiografia

- **O. R. Cote Jr., *The Third Battle: Innovation in the U.S. Navy's Silent Cold War Struggle with Soviet Submarines*, Newport Paper 16 (2003)**, DTIC ADA421957 (non scaricabile automaticamente in questa sessione). Tesi chiave **[M]**, da ricordo del testo da verificare per pagina:
  - Anni '60–'70: il vantaggio USA nascondeva sui **toni a banda stretta** dei battelli sovietici rumorosi. La catena era SOSUS (scoperta a lunga distanza), poi P-3 (localizzazione), poi SSN (pedinamento o uccisione).
  - I **Victor III** (dal 1979) e soprattutto gli **Akula** e **Sierra** (metà anni '80) ridussero drasticamente il margine.
  - La risposta fu la nuova generazione di schiere rimorchiate (TB-16 e TB-23), SURTASS, e più tardi la ricerca attiva a bassa frequenza.
  - Gli Alfa, veloci e rumorosi, erano un problema di **armi** (ne derivarono le specifiche del Mk 48 ADCAP), non di **scoperta**.
- **Sontag & Drew, *Blind Man's Bluff*** (PublicAffairs 1998): Holystone, pedinamenti di SSBN e navi in prova, collisioni, Ivy Bells, crazy Ivan. **[A]**
- **Polmar & Moore, *Cold War Submarines*** (Potomac 2004): Project 705 Alfa (7 unità, reattore a metallo liquido, concetto di "intercettore"), cronologia del silenziamento. **[A]**
- **Weir & Boyne, *Rising Tide*** (Basic Books 2003): prospettiva sovietica, Aport e Atrina, pedinamenti di portaerei. **[M]**
- **Huchthausen, Kurdin & White, *Hostile Waters*** (1997), su K-219 (3 ott. 1986, a NE delle Bermuda, Yankee I, presenza dell'USS Augusta). **Huchthausen, *October Fury*** (2002): i Foxtrot nella crisi di Cuba costretti in superficie da gruppi ASW che li tenevano giù con l'attivo fino all'esaurimento delle batterie. **[A]**
- **Gorshkov, *The Sea Power of the State*** (1976; trad. Pergamon e Naval Institute Press 1979, pp. 119–23 citate dalla CIA): priorità agli SSBN come forza principale, flotta "bilanciata", lotta per il "primo salvo". **[A]**
- **T. Stefanick, *Strategic Antisubmarine Warfare and Naval Strategy*** (Lexington/IDDS 1987, 390 pp.): otto appendici tecniche su scoperta, barriere e portate; focus sugli SSBN. **[A]** (bibliografia) / **[M]** (contenuto)
- **D. F. Winkler, *Incidents at Sea*** (NIP 2017): accordo INCSEA del 1972, che **non** regola i sottomarini in immersione. **[A]**
- **Toshiba–Kongsberg:** fresatrici CNC a 9 assi (1982–84) per eliche silenziose al Cantiere del Baltico; caso scoperto nel 1986–87. Una fonte afferma che i nuovi battelli erano rilevabili solo entro circa 10 nm. **[A]** (fatto) / **[B]** (cifra) (https://en.wikipedia.org/wiki/Toshiba%E2%80%93Kongsberg_scandal; https://www.globalsecurity.org/intell/world/russia/toshiba.htm)
- **Rete Walker** (1967–85): passò ai sovietici informazioni sulla sorveglianza acustica americana, considerate un movente del silenziamento. **[M]**

### C3. Incidenti utili come casi di taratura

| Data | Evento | Lezione per l'IA | Fonte | Conf. |
|---|---|---|---|---|
| 15 nov. 1969 | USS Gato contro K-19, Barents | il pedinamento ravvicinato in acqua bassa porta a collisione | Sontag & Drew | A |
| ott.–nov. 1983 | K-324 (Victor III) aggancia la schiera rimorchiata della fregata USS McCloy (correzione dal dossier 04: era una SQR-15/TASS, non la TACTAS; circa 400 m di cavo avvolti sull'elica secondo le fonti russe), ovest delle Bermuda; emerge e viene rimorchiato a Cuba | un SSN sovietico pedina una fregata ASW a breve distanza, dietro la sua scia | https://en.wikipedia.org/wiki/Soviet_submarine_K-324 | A |
| 21 mar. 1984 | K-314 (Victor I) contro USS Kitty Hawk, Mar del Giappone (Team Spirit 84) | pedinamento di portaerei per giorni; perso il contatto col maltempo, risale a quota periscopica davanti alla portaerei | https://en.wikipedia.org/wiki/Soviet_submarine_K-314 | A |
| mag.–lug. 1985 | Operazione Aport | uscita in massa di SSN per saturare la sorveglianza | USNI Proceedings ott. 2020 | M |
| 3–6 ott. 1986 | K-219 (Yankee) | un SSN americano pedina un SSBN in pattugliamento atlantico | *Hostile Waters* | A |
| mar.–mag. 1987 | Operazione Atrina (5 Victor III) | rotta attorno alla Groenlandia per evitare il SOSUS del GIUK; in apr. 1987 la US Navy li ritrova presso le Bermuda | https://en.wikipedia.org/wiki/Operation_Atrina | A |
| 1971–1981 | Ivy Bells | missioni speciali in zone "bastione" | Sontag & Drew | A |
| 12 ago. 2000 | Kursk, Barents, 108 m | esercitazione monitorata da SSN occidentali; fondali reali del teatro A2 | fonti diffuse | A (fatto) |

### C4. Comportamenti IA derivati dalla dottrina

Formato: **comportamento → parametri suggeriti → fonte e confidenza.** I parametri numerici marcati **[B]** sono calibrazioni di gioco, non dati storici.

**Sottomarini sovietici (Alfa, Victor, Akula)**

1. **Pulizia dei baffles ("crazy Ivan")**
   - Virata brusca di 120–180° con lieve aumento di velocità. A volte l'unità risale a ripercorrere la rotta, a volte pinga in attivo.
   - Intervallo casuale di 30–120 minuti di gioco (scalato alla compressione temporale), più frequente dopo un cambio di rotta, prima di un'immersione o di un'emersione, o se si sospetta un pedinatore.
   - Sui Yankee la manovra era detta "Yankee Doodle". La pratica diventa meno frequente dopo l'adozione delle schiere rimorchiate.
   - Fonti: Wikipedia "Baffles (submarine)"; caso HMS Sovereign contro un Delta (1978), che passò a circa 800 yd senza scoprirlo. **[M]** per la manovra / **[B]** per la frequenza
2. **Alfa come "intercettore"**
   - Resta a bassa velocità in zona d'attesa vicino alla base. Al contatto (cue esterno da SOSUS sovietico, aereo o nave) scatta ad alta velocità per chiudere la distanza, poi rallenta per un aggancio passivo.
   - Contro i siluri usa velocità e profondità per superarli in corsa o scendere sotto il loro limite, anziché la sola furtività.
   - Fonti: Polmar & Moore; Cote. **[M]**
3. **"Securing the port" (missione 3, cap. 10 delle Combat Regulations)**
   - Pattuglia un corridoio di uscita di 10–20 nm. All'ingaggio si avvicina in passivo, poi pinga a meno di 1 nm per "scacciare" l'intruso.
   - Se l'intruso è aggressivo, lo segnala alla rete ASW di superficie e aerea.
   - Fonti: manuale p. 21; CIA 2017, p. 49. **[M]**
4. **Pedinamento di portaerei o gruppi in tempo di pace**
   - Resta a 3–10 nm in coda o sul fianco del gruppo, con frequenti risalite a quota periscopica per ESM e periscopio.
   - Se perde il contatto, risale e accelera verso l'ultima posizione stimata, con rischio di collisione.
   - Fonti: K-314, K-324. **[A]** per il comportamento / **[B]** per le distanze
5. **Uscite di massa**
   - Più SSN partono insieme; ognuno fa scatti seguiti da deriva lenta e devia dai varchi sorvegliati.
   - Fonte: Aport e Atrina. **[M]**
6. **Protezione degli SSBN (missione 7)**
   - L'SSN di scorta naviga 5–15 nm davanti o di lato all'SSBN, lungo l'asse di minaccia, e ingaggia per primo. L'SSBN procede a 3–6 kt.
   - L'IA deve evitare il fuoco amico: il manuale insiste su questo punto, p. 25.
   - Fonti: concetto di "stabilità di combattimento"; Herrick parte II. **[M]** per il concetto / **[B]** per le distanze
7. **Sfruttamento delle correnti negli stretti**
   - A Gibilterra la corrente superficiale entra verso E e l'outflow profondo esce verso W.
   - Per transitare silenziosi si sceglie lo strato a favore e si va a 3–5 kt o a motori quasi fermi.
   - Fonti: pratica WWII; analogia Tsushima (War on the Rocks). **[M]**
8. **Mascheramento nel traffico**
   - L'unità si accoda a un mercantile a 500–2000 yd, nella sua scia, per coprire la propria firma.
   - Fonte: plausibilità, molto citata nella saggistica divulgativa. **[B]**

**Forze ASW sovietiche (superficie e aria)**

9. **Gruppo di ricerca-attacco (KPUG)**
   - 2–4 unità (Grisha in zona costiera; Krivak o Udaloy in zona media) in linea di fronte con spaziatura pari a circa 1–1.5 volte la portata del sonar di scafo. Usano il VDS (Ox Tail) e si muovono a scatti con soste a 3–5 kt per ascoltare.
   - Gli elicotteri Ka-25 o Ka-27 immergono il sonar a salti di 3–5 nm attorno al datum.
   - Fonti: Wikipedia "Krivak-class frigate"; CIA, *Moskva* nel Mediterraneo (sonar a immersione). **[M]** per la composizione / **[B]** per i numeri
10. **Difesa a strati attorno a Kola e Okhotsk**
    - Zona interna: Grisha, Poti, Be-12, Mi-14, campi di mine.
    - Zona media: Udaloy, Krivak, Il-38, Ka-27.
    - Zona esterna: SSN e Tu-142.
    - Fonti: CIA (documento sulle forze miste con sensori fissi e mine per i bastioni); NIE 11-15-84. **[M]**
11. **Prosecuzione di un contatto**
    - Al primo contatto (boa, MAD, scafo): vettoriamento degli aerei con barriera di sonoboe; arrivo del KPUG; **"tenuta sotto" con attivo continuo** per saturare e costringere a manovre rumorose o a emergere (dottrina simile a quella USA del 1962).
    - In guerra: lancio di razzi ASW (RBU) e siluri SS-N-14 su datum.
    - Fonti: *October Fury* (lato USA); CIA, Saakyan (ricerca congiunta). **[M]**
12. **Tempo di pace contro tempo di guerra**
    - In pace: pinga per segnalare, ostacola, ma non attacca. L'accordo INCSEA vale per navi e aerei, non per i sottomarini in immersione.
    - In guerra: "battaglia per il primo salvo", attacco immediato a SSBN e portaerei.
    - Fonti: Winkler; CIA 2017; Gorshkov. **[A]**

**SSN americani e alleati**

13. **Pedinamento di SSBN**
    - Si posiziona fuori dai baffles nemici, su un settore di 30–60° di poppa, a distanza di sicurezza dalle virate di pulizia.
    - Solo passivo (banda stretta). Nessun attivo, nessuna risalita non necessaria.
    - Alla virata del bersaglio: riduce la velocità al minimo (scendendo di quota se c'è uno strato) e lascia passare.
    - Con le schiere rimorchiate le distanze di pedinamento si allungano.
    - Fonti: Sontag & Drew; Cote. **[A]** per il comportamento / **[B]** per le distanze
14. **Barriera cue-based**
    - Pattuglia lenta (3–6 kt) su una linea di stazione nel GIUK o nel FSC. Riceve un cue SOSUS o P-3 (ellisse di 10–30 nm), si avvicina, conferma in passivo e poi pedina o ingaggia secondo le regole d'ingaggio.
    - Fonte: Cote. **[A]**
15. **Operazioni avanti (Maritime Strategy)**
    - Nelle fasi I e II gli SSN penetrano nel Barents e nell'Okhotsk. Priorità agli SSN nemici di scorta, poi agli SSBN; uso del fondo basso e dei fronti per nascondersi.
    - Fonte: Newport Paper 33. **[A]**
16. **Uso dell'ambiente**
    - Si mette sotto il termoclino o l'alocline rispetto alla minaccia di superficie.
    - Nel Barents d'inverno (colonna mescolata) non ci sono strati: si punta sulla distanza e sul rumore dei pescherecci.
    - Al ciglio celtico sfrutta le onde interne e il riverbero dei canyon.
    - Fonte: fisica acustica, dossier (b), e Parte A. **[M]**

**Regole d'ingaggio (per il motore di missione)**

17. Missioni 1–5: tempo di pace o crisi.
    - Armi vietate. L'attivo è considerato "atto ostile" (manuale pp. 20–22).
    - Contatto ravvicinato tollerato.
    - L'IA sovietica risponde a un ping lontano con evasione o scorta aggressiva, e a un ping vicino con allontanamento (manuale p. 21).
18. Missioni 6–10: guerra.
    - Fuoco libero. Identificazione obbligatoria per evitare il fuoco amico.
    - Controllo delle emissioni (EMCON) totale per i sottomarini; emissioni radar ridotte per i gruppi di superficie fino al contatto.
    - **[M]** (prassi generale; dettaglio sovietico da ricavare dalle Combat Regulations, cap. 6–8, documento non letto)

---

## Tabella riassuntiva dei teatri

| # | Teatro | Bbox (lat; lon) | nm (N×E) | Profondità tipiche | Strutture chiave | Fondo | SSP estate / inverno | Ghiaccio e luce | Rumore | Missioni |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | Faroe / GIUK | 60.5–63.5N; 10.0–4.0W | 180×170 | 100–1500 m | FBC sill 840 m, WTR circa 600 m, IFR circa 450 m, FSC oltre 1000 m | sabbia e ghiaia sui banchi, fango nei canali | termoclino a 30–60 m, poi forte gradiente negativo a 500–700 m (acqua sotto 0 °C) / mescolato a 200–400 m | niente ghiaccio; 5 h di luce a dicembre | moderato, pesca, cetacei, mare forte d'inverno | 1, 2 |
| 2 | Barents / Kola | 69.2–72.0N; 31–39E | 168×160 | 100–350 m | costa ripida del Murman, fosse oltre 300 m, fronte polare più a N sull'isobata 250 m | fango glaciale, sabbia e ghiaia, roccia sotto costa | strato a 20–40 m con gradiente negativo / isotermo e crescente | costa libera dal ghiaccio; notte polare 2 dic.–11 gen. | traffico militare, pesca | 3 |
| 3 | Gibilterra | 35.0–36.8N; 7.5–3.5W | 108×194 | 284 m (soglia) fino a oltre 1000 m | Camarinal 284 m, Espartel 360 m, Tarifa oltre 800 m | roccia e ghiaia sulle soglie, fango | interfaccia AW/MW a 100–150 m, onde interne oltre 100 m, MOW a 1000 m nel Golfo di Cadice | Levante e Poniente | altissimo | 4, 5 |
| 4 | Otranto | 39.5–42.5N; 17.0–20.5E | 180×160 | 100–1230 m | soglia circa 780 m, Fossa Sud-Adriatica circa 1200 m, canyon di Bari | fango e argilla | termoclino a 20–50 m molto forte / mescolato, convezione profonda | Bora e Scirocco | medio-alto | 6 |
| 5 | Kunashir / Okhotsk | 43.3–45.6N; 144.8–148.5E | 138×158 | da meno di 20 m (Nemuro) a oltre 3000 m (bacino) e oltre 9000 m (fossa) | stretti di Ekaterina e Izmeny, bacino delle Curili, fossa | roccia vulcanica, fango diatomitico | canale dicotermico a circa 80–100 m / ghiaccio nell'Okhotsk da gennaio ad aprile | nebbia estiva, ghiaccio | pesca, ghiaccio | 7 |
| 6 | Piattaforma Celtica | 47.0–49.5N; 12.0–7.5W | 150×180 | 120–200 m sulla piattaforma, 4000–4800 m nella piana | ciglio a 200 m, Whittard e altri canyon, Goban Spur, creste sabbiose | sabbia sulla piattaforma, fango sulla scarpata | termoclino a 30–50 m e solitoni / isoveloce; al largo canale SOFAR e zone di convergenza | burrasche d'inverno | altissimo, delfini | 8, 9 |
| 7 | Baltico SW | 54.0–55.8N; 10.5–15.0E | 108×155 | 8–50 m (fino a 100 m a Bornholm) | Drogden 7–9 m, Darss 18 m, Kadetrinne circa 25–30 m, Fehmarn Belt | sabbia, till, massi, fango | termoclino a 10–25 m e canale a 30–50 m sull'alocline / freddo con alocline | ghiaccio raro (1985, 1987); 7 h di luce a dicembre | altissimo | 10 |

---

## Bibliografia essenziale

**Primarie e governative**
- Electronic Arts (design: Paul Grace e collaboratori), *688 Attack Sub – Manual* (1989). Trascrizione: https://amigaland.de/wp-content/uploads/2024/02/688-Attack-Sub-Manual-ENG.pdf
- CIA, NIE 11-15-82/D (doc. 0000268225) e NIE 11-15-84/D (doc. 0000262011); SNIE 11-20-84/D; *Combat Regulations of the Soviet Navy* (doc. 0001430734, 1986). Reading Room: https://www.cia.gov/readingroom/collection/cia-analysis-soviet-navy
- CIA, *Soviet Navy Intelligence and Analysis During the Cold War* (2017): https://www.cia.gov/static/Soviet-Navy-Intelligence-and-Analysis-During-the-Cold-War.pdf
- Hattendorf & Swartz (a cura di), *U.S. Naval Strategy in the 1980s*, Newport Paper 33 (2008): https://digital-commons.usnwc.edu/usnwc-newport-papers/33/
- Hattendorf, *The Evolution of the U.S. Navy's Maritime Strategy, 1977–1986*, Newport Paper 19 (2004): https://apps.dtic.mil/sti/pdfs/ADA422147.pdf
- Cote, *The Third Battle*, Newport Paper 16 (2003): https://apps.dtic.mil/sti/pdfs/ADA421957.pdf
- Herrick, *Soviet Naval Mission Assignments*, parti II e III (Ketron/ONR 1979–80): DTIC ADB082798, ADB082799
- Perse, CNA CRM 84-29 (1986): https://apps.dtic.mil/sti/pdfs/ADA175532.pdf

**Libri**
- Sontag & Drew, *Blind Man's Bluff* (1998)
- Polmar & Moore, *Cold War Submarines* (2004)
- Weir & Boyne, *Rising Tide* (2003)
- Huchthausen et al., *Hostile Waters* (1997); Huchthausen, *October Fury* (2002)
- Gorshkov, *The Sea Power of the State* (1979)
- Stefanick, *Strategic Antisubmarine Warfare and Naval Strategy* (1987)
- Winkler, *Incidents at Sea* (2017)

**Oceanografia**
- Sannino, Pratt & Carrillo, JPO 2009 (Gibilterra)
- Sánchez-Garrido et al. 2011 (onde interne a Gibilterra)
- Hansen et al. / Ocean Sci. 12 (2016) (Faroe Bank Channel)
- Harris et al. 1998 e Ocean Sci. 20:895 (2024) (fronte polare del Barents)
- Frontiers Mar. Sci. 2025 e CIESM CSS-2 (Otranto)
- Ohshima et al. JGR 2002 e BRE "Курильские проливы" (Curili)
- Aslam et al. UEA e Vlasenko & Stashchuk 2015 (ciglio celtico)
- Dutkiewicz et al., Geology 2015

**Dati**
- GEBCO: https://www.gebco.net
- ETOPO 2022: https://www.ncei.noaa.gov/products/etopo-global-relief-model
- EMODnet Bathymetry: https://emodnet.ec.europa.eu/en/bathymetry
- IBCAO: https://www.gebco.net/data-products/gridded-bathymetry-data/arctic-ocean
- BSBD: http://data.bshc.pro
- WOA23: https://www.ncei.noaa.gov/products/world-ocean-atlas
- Copernicus Marine: https://marine.copernicus.eu
- ERA5: https://cds.climate.copernicus.eu
- GSHHG: https://www.soest.hawaii.edu/pwessel/gshhg/
- Natural Earth: https://www.naturalearthdata.com

---

## Lacune note (da colmare)

1. Profondità dello stretto di Ekaterina e degli stretti di Izmeny e Kunashirskij: non trovate. Ricavarle da GEBCO o JODC.
2. Profondità dell'alocline nel Baltico sud-occidentale: valori non verificati.
3. Testo completo delle *Combat Regulations* (cap. 7) e della SNIE 11-20-84: il sito CIA blocca il recupero automatico. Serve lettura manuale per ricavare distanze, velocità e formazioni ASW sovietiche reali.
4. *The Third Battle* (DTIC risponde 403): le citazioni per pagina restano da completare.
5. Tutti i numeri dei profili di velocità del suono sono illustrativi: rigenerarli da WOA23.
6. Frequenza reale del crazy Ivan e distanze tipiche di pedinamento: nessuna fonte quantitativa pubblica trovata. Restano parametri di calibrazione di gioco.
7. Licenze di BSBD, JTOPO30 ed endpoint WCS EMODnet: da confermare sui portali.
