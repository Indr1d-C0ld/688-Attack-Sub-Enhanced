# Rotte, traffico, pesca e traghetti nei sette teatri (1985-1991)

Scopo: dati per generare traffico mercantile, peschereccio e di linea come contatti veri lungo le rotte di ogni teatro. I riquadri sono quelli della tabella riassuntiva di `03_teatri_fondali_storia.md`; il "paesaggio sonoro" per teatro è quello di `09_sonar_sistemi_e_interfacce.md` §5.7. Qui ci sono le **geometrie** (polilinee lat/lon), le **portate** (navi al giorno) e i **calendari** (pesca, traghetti, ghiaccio).

**Metodo.** Ho letto solo pagine aperte con WebSearch e WebFetch, senza scaricare nulla di proposito. Tre PDF (la circolare IMO sulle corsie di Ouessant, il rapporto HELCOM 2008 e il manuale danese *Navigation through Danish waters*) sono finiti nella cache dello strumento come risultato di WebFetch, e ne ho estratto il testo da lì. Le fonti d'epoca anni '80 in rete sono poche. Dove manca il dato d'epoca do quello moderno con la regola per riportarlo indietro. Le polilinee non tratte da un documento ufficiale le ho ricostruite io dalla geografia, dalle distanze dichiarate e da archi di cerchio massimo calcolati (script nello scratchpad, non nel progetto). Sono marcate **STIMA** e vanno controllate su una carta prima di usarle per qualcosa di più di un generatore di traffico.

Legenda della confidenza:
- **[alta]**: testo ufficiale (IMO, ordinanza, autorità marittima) o dato numerico primario letto per intero;
- **[media]**: articolo o studio letto per intero, Wikipedia, stampa d'epoca;
- **[bassa]**: fonte secondaria debole, o dato letto solo nell'estratto del motore di ricerca;
- **STIMA**: ricostruzione o deduzione mia, con il ragionamento indicato.

Formato delle coordinate: gradi decimali (N positivo, E positivo, W negativo) se non indicato altrimenti; nm = miglia nautiche.

---

## 0. Regole generali per il generatore (valide in tutti i teatri)

| Tipo (anni '80) | Quota sul traffico di transito | Velocità tipica | Note acustiche/di comportamento |
|---|---|---|---|
| Carico generale e piccole navi miste | 35–45 % | 12–15 kn | il tipo più numeroso negli anni '80: il container non ha ancora sostituito il "general cargo" |
| Petroliere e chimichiere | 15–25 % (più a Gibilterra e Ouessant) | 12–15 kn (VLCC 13–15) | VLCC lente in giri, elica grande; in zavorra pescaggio basso |
| Rinfusiere | 10–15 % | 12–14 kn | |
| Portacontainer e ro-ro | 5–10 % | 16–22 kn | le più veloci dopo i traghetti |
| Costiere (< 3000 t) | molto variabile: dominanti in Baltico | 9–12 kn | diesel veloci, righe alte |
| Traghetti e passeggeri | dipende dalle linee (§ per teatro) | 15–22 kn | orario fisso: contatti prevedibili |
| Pescherecci in transito | — | 8–11 kn | |
| Pescherecci in pesca | — | 2,5–4,5 kn (strascico), 0–2 (palangari, reti, lampare) | rumore di verricello, cavi; rotte a zig-zag sul fondale buono |

Le quote sono una **STIMA** dalla composizione delle flotte mondiali di fine anni '80. Il dato HELCOM 2008 (§6.2: 46 % carico, 14 % cisterne, 11 % passeggeri) conferma l'ordine di grandezza per un mare moderno. [STIMA]

**Regola di scala dal moderno agli anni '80.** Il numero di navi conta più della stazza. Negli anni '80 le navi erano più piccole e più numerose per tonnellata trasportata. Per i conteggi di navi che passano una linea, un fattore 0,6–0,8 sul dato 2005–2010 è ragionevole nei mari europei. Fanno eccezione Gibilterra, dove il traffico cresce molto dopo il 2000 con Algeciras e Tanger Med (fattore circa 0,5–0,6 rispetto al 2015), e il Barents, dove ai fini del gioco il traffico sovietico militare e di pesca anni '80 era *superiore* a quello moderno. [STIMA]

**Larghezza delle corsie nel generatore.** Dove non c'è uno schema di separazione (TSS) ufficiale, le navi si distribuiscono attorno all'asse con una gaussiana di σ ≈ 1–3 nm negli stretti e 5–15 nm in mare aperto. Il traffico costiero sta più vicino a terra (2–6 nm). [STIMA]

---

## 1. Gibilterra (35,0–36,8N; 7,5–3,5W)

### 1.1 Corsie

Lo schema di separazione del traffico (TSS) esisteva già nel 1979–80. Uno studio fatto a bordo della *Cornide de Saavedra* trovò che circa il **65 %** delle navi seguiva le corsie. [media, Mavor 1980] Il servizio di traffico (VTS) di Tarifa entra in prova il **3 settembre 1987**; la chiamata sul canale VHF 10 è obbligatoria. [bassa, Fundación MAPFRE, estratto] Lo schema fu emendato nel 2007 e poi nel 2015, quando fu aggiunta un'area di precauzione per Tanger Med; quest'area **non** esisteva negli anni '80. [media, COLREG.2/Circ.66, estratto] Il testo ufficiale con le coordinate non era leggibile: il server non rispondeva.

Dati certi di forma (dichiarazione spagnola all'IMO, 1996): larghezza minima fra le coste **7,5 nm**; distanza minima fra costa e corsie **2,0 nm**. [media, Spagna/IMO 1996] Vale la regola di circolazione a destra: chi va **verso ovest** usa la corsia **nord** (lato spagnolo), chi va **verso est** quella **sud** (lato marocchino).

Polilinee per il generatore (asse delle corsie, **STIMA**, da verificare sulla carta IHM 445 o sulla BA 142):

| Corsia | Senso | Punti (lat, lon) | Larghezza |
|---|---|---|---|
| G-W (verso Atlantico) | unico, verso ovest | (36.010, -5.250) → (36.005, -5.420) → (35.977, -5.600) → (35.960, -5.750) → (35.945, -5.950) | 2 nm |
| Zona di separazione | — | asse circa 35.945 a 5.60W, 35.935 a 5.95W | 0,5–1 nm |
| G-E (verso Mediterraneo) | unico, verso est | (35.910, -5.950) → (35.920, -5.750) → (35.915, -5.600) → (35.950, -5.450) → (35.975, -5.250) | 2 nm |

Prolungamenti fuori dallo schema (fasci di traffico che si aprono a ventaglio, **STIMA** da geografia):

| Fascio | Da → a | Punti | Quota del flusso |
|---|---|---|---|
| W1 Capo San Vincenzo e Nord Europa | uscita ovest del TSS → | (35.95, -5.95) → (36.10, -6.40) → (36.45, -7.50) [bordo del riquadro] → Capo S. Vincenzo (37.0, -9.1) | 55–60 % del traffico verso ovest |
| W2 Canarie, Africa occidentale, Sud America | → | (35.93, -5.95) → (35.80, -6.40) → (35.45, -7.50) | 30–35 % |
| W3 Golfo di Cadice (Cadice, Huelva, Siviglia) | → | (35.96, -5.95) → (36.25, -6.40) → (36.53, -6.35) Cadice; Huelva (37.10, -6.85) | 10 % |
| E1 Mediterraneo nord (Cartagena, Barcellona, Marsiglia, Genova) | uscita est → | (35.98, -5.25) → (36.10, -4.50) → (36.40, -3.50) [bordo] → Capo de Gata | circa 45 % del traffico verso est |
| E2 Mediterraneo centrale e Suez | → | (35.97, -5.25) → (35.95, -4.50) → (35.95, -3.50) [bordo] (asse dell'Alboran, passa a nord dell'isola di Alboran 35.94, -3.03) | circa 45 % |
| E3 Algeria e Marocco mediterraneo | → | (35.95, -5.25) → (35.60, -4.20) → (35.30, -3.50) | 10 % |

Traffico che taglia lo stretto da nord a sud (traghetti, rimorchiatori, bunkeraggio alla rada di Gibilterra e Algeciras). Tutte le traversate sono **STIMA** di rotta diretta:
- Algeciras (36.13, -5.43) → Ceuta (35.89, -5.31);
- Algeciras → Tangeri (35.79, -5.81);
- Tarifa (36.01, -5.60) → Tangeri;
- Gibilterra (36.14, -5.36) → Tangeri;
- Malaga (36.71, -4.42) → Melilla (35.29, -2.94), che esce dal riquadro a est.

### 1.2 Densità

| Periodo | Dato | Navi al giorno | Fonte |
|---|---|---|---|
| circa 1979–80 | media **5,8 navi/ora**, più denso del Passo di Calais | circa **140/g** (conto mio) | Mavor, J. Navigation 33(3), 1980 [media] |
| 1996 | 70 000 navi mercantili l'anno: 58 000 in transito diretto, 12 000 traversate nord-sud (soprattutto traghetti); 30 % dichiara merci pericolose; 2,6 milioni di passeggeri l'anno | circa 160 in transito + 33 traversate | dichiarazione spagnola all'IMO, 1996 [media] |
| oggi | "circa 300/g", circa 110 000 movimenti l'anno | — | fonti varie [bassa] |

Valori proposti per il 1985–91: **130–150 navi in transito al giorno** (circa 5,5–6 l'ora), divise 50/50 fra i due sensi, più **25–40 traversate di traghetti al giorno**, che d'estate (Operación Paso del Estrecho, dal 1986, 15 giugno–15 settembre) salgono al doppio. Quote per tipo: petroliere 20–25 % (greggio dal Golfo verso l'Europa via Capo di Buona Speranza e via Suez, che riapre nel 1975), carico generale 40 %, rinfusiere 15 %, container e ro-ro 8–10 %, altro 5 %. [STIMA]

### 1.3 Pesca

| Zona | Lat/lon (STIMA) | Flotta | Stagione |
|---|---|---|---|
| Almadrabe (tonnare fisse) di Barbate, Zahara e Tarifa | catena di reti ancorate a 1–3 nm da costa fra (36.17, -5.95) e (36.02, -5.65) | ostacolo fisso, barche di servizio; il tonno rosso passa verso il Mediterraneo | aprile–luglio [media, conoscenza generale] |
| Golfo di Cadice, piattaforma a 30–400 m | poligono (36.2, -6.4)–(37.1, -7.4)–(36.7, -7.5)–(36.0, -6.6) | strascicanti di Huelva, Isla Cristina, Sanlúcar e Puerto de Santa María (gamberi, nasello, seppie); cianciolo per sardina e acciuga | tutto l'anno; di notte le lampare |
| Alboran nord | fascia di 2–10 nm lungo la costa da Estepona a Malaga, 36.3–36.7N | ciancioli di Malaga ed Estepona (boquerón), artigianali | tutto l'anno, notte |
| Costa marocchina e flotta spagnola in acque marocchine | fascia 35.2–35.8N, 6.0–7.0W (Larache, Asilah) e costa del Rif | marocchini artigianali e strascicanti; spagnoli (Barbate, Algeciras) con l'accordo del 19.8.1983 | la flotta spagnola viene ritirata il **31.12.1987**; nuovo accordo CEE-Marocco il **26.5.1988** [media, Congreso de los Diputados, BOE 23.11.1988] |

Contatti di pesca: **20–60** nel riquadro di giorno, **50–120** di notte, soprattutto lampare e ciancioli sotto costa. [STIMA]

### 1.4 Traghetti (anni '80)

| Linea | Durata | Corse al giorno (per senso) | Confidenza |
|---|---|---|---|
| Algeciras–Ceuta (Trasmediterránea, Isnasa) | circa 1 h 30 con traghetti convenzionali | 6–10; d'estate fino a 15 | STIMA (oggi 20–30; il dato del 1996 di 12 000 traversate l'anno fa circa 16 al giorno per senso, sommando tutte le linee) |
| Algeciras–Tangeri (Limadet, Comanav, Trasmediterránea) | 2 h 30 | 3–5 | STIMA |
| Tarifa–Tangeri | 1 h 30 | 1–2 | [bassa] |
| Gibilterra–Tangeri | 2 h 30 | 0–1 (diventa più regolare dopo la riapertura della frontiera, 1985) | [bassa] |
| Malaga–Melilla | 7–8 h | 1 | STIMA |

---

## 2. Otranto (39,5–42,5N; 17,0–20,5E)

### 2.1 Corsie

Nel Canale d'Otranto **non c'è un TSS**, né oggi né negli anni '80 (non ne ho trovato traccia; il sistema di rapportazione ADRIREP è del 2000 e oltre). Il traffico si distribuisce per fasci. [bassa: assenza di prova]

| Fascio | Senso | Punti (STIMA) | Note |
|---|---|---|---|
| O1 asse dell'Adriatico (Trieste, Venezia, Rijeka, Koper, Ancona → Mediterraneo) | doppio, circa 4–6 nm per senso, verso sud sul lato italiano | (42.50, 17.40) → (41.60, 18.10) → (40.80, 18.75) → (40.20, 19.00) | ramo comune |
| O1a ramo Ionio, Suez e Grecia | doppio | (40.20, 19.00) → (39.50, 19.40) [bordo] → Capo Matapan | circa 55 % |
| O1b ramo Messina, Sicilia e Mediterraneo occidentale | doppio | (40.20, 19.00) → (39.75, 18.55) (al largo di S. Maria di Leuca, 39.79, 18.36) → (39.50, 18.10) [bordo] | circa 45 % |
| O2 Brindisi–Corfù–Igoumenitsa–Patrasso (traghetti) | doppio | Brindisi (40.65, 17.98) → (40.35, 18.70) → (39.85, 19.55) → canale nord di Corfù (39.78, 19.90) → Corfù porto (39.62, 19.93) / Igoumenitsa (39.50, 20.25) | Patrasso (38.25, 21.73) è fuori dal riquadro, verso sud |
| O3 Bari–Grecia | doppio | Bari (41.13, 16.87) [appena fuori a ovest] → (40.80, 17.80) → (40.25, 18.90) → poi come O2 | |
| O4 Bari–Dubrovnik e Bari–Bar | doppio | Bari → (41.70, 17.60) → Dubrovnik (42.64, 18.11); Bari → Bar (42.09, 19.08) | traghetti jugoslavi |
| O5 costiero italiano | doppio | 3–6 nm da costa, da Monopoli (40.95, 17.30) a Otranto (40.15, 18.49) e Leuca | costiere, pescherecci in transito |
| O6 Albania (Durazzo 41.31, 19.45; Valona 40.46, 19.48) | doppio, molto rado | Durazzo → (41.20, 18.70) → asse O1 | Albania chiusa: pochi mercantili di Stato, cargo cinesi e jugoslavi. Contatti albanesi rarissimi [STIMA] |

### 2.2 Densità

Dato moderno non verificato: circa **30 000 movimenti l'anno** nello stretto (circa 80 al giorno). [bassa, blog divulgativo] Per il 1985–91 propongo **40–60 navi al giorno** sull'asse O1, più i traghetti (§2.4). Dentro: petroliere 20 %, sia per Trieste (oleodotto TAL verso la Baviera) sia per le raffinerie di Venezia e Rijeka; carico generale 45 %; rinfusiere 15 %; ro-ro e traghetti fuori orario 10 %; navi militari NATO e jugoslave in più. [STIMA]

### 2.3 Pesca

| Zona | Lat/lon (STIMA) | Flotta | Stagione |
|---|---|---|---|
| Scarpata della Fossa Sud-Adriatica (200–800 m) | fascia 41.0–41.9N, 17.3–18.3E | strascicanti di Bari, Mola, Monopoli e Molfetta: nasello, gambero rosa (*Parapenaeus*), scampo | tutto l'anno, salvo il fermo estivo |
| Piattaforma pugliese (50–200 m) | 2–20 nm da costa, da 41.2N 17.0E a 40.1N 18.6E | strascico costiero, volante, cianciolo | tutto l'anno |
| Scarpata ionica (Leuca, Gallipoli), 400–700 m | (39.95, 18.10)–(39.50, 18.60) | strascico per gamberi rossi (*Aristaeomorpha*, *Aristeus*) e nasello | tutto l'anno |
| Lato greco (Corfù, Epiro) | 1–5 nm da costa | ciancioli e piccola pesca greca | notte, lampare |
| Lato albanese e montenegrino | sotto costa | pochi pescherecci di Stato | sporadici |

Il **fermo biologico** dello strascico in Adriatico: nel 1995 trenta giorni feriali; nel **1996 dal 31 luglio al 13 settembre**. [media, decreti ministeriali 1995–96] Negli anni '80 il fermo era sperimentale e a ondate; lo tratterei come assente nel 1985–86 e come agosto parziale dal 1987–88. [bassa] Le catture di acciuga, 53 000 t di media nel 1978–80, crollano nel **1987**. [media, studio UniBo, estratto] Ordine di grandezza della flotta: la Puglia nel 2012 aveva **522** strascicanti (33 % della flotta regionale). [media, ARPA Puglia] Negli anni '80 erano di più: 700–900 strascicanti regionali [STIMA]. Contatti di pesca nel riquadro: **40–100** di giorno nei giorni feriali, quasi zero la domenica e durante il fermo. [STIMA]

### 2.4 Traghetti (anni '80)

Brindisi era il porto principale per la Grecia. Fonte (guida non datata): Adriatica e HML fanno Brindisi–Patrasso "a giorni alterni, giornaliero da fine giugno alla prima settimana di settembre". [bassa, Reid's guide] Con tutte le compagnie degli anni '80 (Adriatica, Hellenic Mediterranean Lines, Fragline, European Seaways, Med Link, Kavounides, Libra):

| Linea | Durata | Corse al giorno per senso (inverno / estate) | Confidenza |
|---|---|---|---|
| Brindisi–Corfù–Igoumenitsa(–Patrasso) | Corfù 8–9 h; Patrasso 16–18 h | 1–3 / 8–15 | STIMA (partenze soprattutto la sera, arrivi al mattino) |
| Bari–Corfù–Igoumenitsa–Patrasso | Patrasso circa 18–20 h | 0–1 / 2–3 | [bassa] |
| Ancona–Patrasso e Venezia/Trieste–Grecia, Turchia, Israele (transito) | transitano sull'asse O1 | 1–3 / 4–6 | STIMA |
| Bari–Dubrovnik, Bari–Bar | 8–9 h | qualche corsa alla settimana / quasi giornaliera | [bassa] |
| Otranto–Corfù, Otranto–Igoumenitsa | 4–5 h | 0 / 1 | [bassa] |

---

## 3. Piattaforma Celtica (47,0–49,5N; 12,0–7,5W)

**Nota di geometria.** Lo schema di Ouessant sta **a est** del riquadro (5,0–6,1W). Nel riquadro entrano i flussi atlantici che **escono** dallo schema e il traffico Irlanda–Iberia. Riporto comunque le corsie di Ouessant, perché sono la sorgente dei flussi.

### 3.1 Lo schema di Ouessant (DST)

- **Storia.** Lo schema esiste dal 1968 [bassa]. Dopo l'*Amoco Cadiz* (16.3.1978) l'IMCO adotta il 20.4.1978 un nuovo schema [bassa, Senato francese 1978–79], che entra in vigore nel **1979–80** (bateaux.com dice 1980) [media]. Lo schema anni '80 ha **tre corsie**: due verso nord-est separate da una verso sud-ovest; le merci pericolose usano la corsia **esterna** verso nord-est; lo schema comincia **a 5 nm da Ouessant**. [media, La Libre Belgique 16.12.2002] Le petroliere grandi vengono tenute lontane dalla costa. [media, Chalk e Coupard, J. Navigation 33(3), 1980, abstract] Viene abolito dal **1° maggio 2003** perché aveva punti d'incrocio pericolosi a nord-est e a sud. [alta, SN/Circ.232; media, UK P&I 2003]
- **Polilinee dello schema anni '80 (STIMA).** Assi a 8, 15 e 22 nm dal faro di Créac'h (48°27,6'N 5°07,8'W) lungo il rilevamento 315°. A sud-ovest del punto di svolta le corsie corrono a 208°/028°, a nord-est a 060°/240° (stessi angoli dello schema 2003). Corsie larghe circa 3 nm. In tabella, punto sud-ovest → svolta → nord-est:

| Corsia anni '80 (STIMA) | Senso | Punti |
|---|---|---|
| NE interna, navi senza merci pericolose | verso nord-est | 48°11,2'N 5°34,1'W → 48°33,3'N 5°16,4'W → 48°45,8'N 4°43,6'W |
| SW, discendente | verso sud-ovest | 48°50,7'N 4°51,1'W → 48°38,2'N 5°23,8'W → 48°16,1'N 5°41,6'W |
| NE esterna, petroliere cariche e merci pericolose | verso nord-est | 48°21,1'N 5°49,1'W → 48°43,2'N 5°31,3'W → 48°55,7'N 4°58,5'W |

- **Schema dal 2003 (per confronto e per il "moderno") [alta, SN/Circ.232, allegato A, WGS84].** Corsie di **5 nm**, separate da zone di 5 nm. Rotta sul fondo per chi va verso sud: 240° fino alla linea a 315° da Créac'h, poi 208°. Per chi va verso nord: 028°, poi 060°. Vertici:
  - **corsia verso sud**: 49°01,10'N 5°36,05'W; 48°57,00'N 5°32,50'W; 48°52,05'N 5°45,00'W; 48°39,70'N 5°55,20'W; 48°42,00'N 6°01,60'W; 48°55,60'N 5°50,60'W;
  - **corsia verso nord**: 48°52,75'N 5°28,60'W; 48°48,60'N 5°25,10'W; 48°45,00'N 5°34,30'W; 48°35,10'N 5°42,30'W; 48°37,40'N 5°48,60'W; 48°48,60'N 5°39,60'W;
  - **corsia a doppio senso**, riservata ai passeggeri di linea e al cabotaggio fra Finisterre e La Hague senza merci pericolose: 48°39,70'N 5°14,70'W; 48°38,00'N 5°12,90'W; 48°29,80'N 5°23,50'W; 48°30,60'N 5°26,30'W;
  - **zona di traffico costiero**: fino a Men Korn (48°27,96'N 5°01,32'W) e La Jument (48°25,33'N 5°08,04'W).
  
  Rapportazione obbligatoria per navi sopra le 300 t di stazza lorda entro **40 nm** da Ouessant (OUESSANT TRAFIC, VHF 13). [alta]
- **Densità.** Nel 2002, circa **142 navi al giorno** nel "rail", di cui circa **20** cisterne pericolose (petroliere, chimichiere, gasiere). [media, La Libre 2002] Altra fonte: 120 cargo al giorno di 89 bandiere. [media, bateaux.com 2023] Per il 1985–91 propongo **110–140 al giorno**, cioè 55–70 per senso, con petroliere al 20–25 %. [STIMA]

### 3.2 Flussi dentro il riquadro (archi di cerchio massimo calcolati, **STIMA** di rotta)

| Fascio | Senso | Punti nel riquadro | Quota del traffico di Ouessant (STIMA) |
|---|---|---|---|
| C1 Ouessant–Nord America (Cape Race, New York) | doppio | (49.15, -8.52) → (49.52, -11.49) → bordo nord (49.5, -12.0); per New York (49.49, -9.98) | 15–20 % |
| C2 Ouessant–Azzorre, Caraibi, Panama, Sud America est | doppio | (47.94, -7.15) → (47.37, -8.58) → bordo sud (47.0, -9.4); per i Caraibi (47.65, -10.68) → bordo ovest (47.2, -12.0) | 15–20 % |
| C3 Irlanda (Cork, Shannon)–Finisterre, Lisbona, Mediterraneo | doppio, circa 10 nm per senso | lungo il meridiano 9.6W: (49.5, -9.6) → (47.0, -9.6) | 8–15 navi al giorno [STIMA] |
| C4 Mare d'Irlanda e Canale di San Giorgio (Smalls, 51.7N 5.7W)–Finisterre | doppio | (49.5, -7.6) → (48.0, -7.9) → (47.0, -8.3) | 10–20 navi al giorno [STIMA] |
| C5 Ouessant–Finisterre (Lisbona, Gibilterra, Africa) | doppio | passa a **est** del riquadro (47.0N circa 6.7W): fuori | 50–60 %: si sente solo ai bordi est |

Totale nel riquadro: **25–45 navi al giorno**, soprattutto nella metà orientale e lungo il bordo nord. [STIMA]

### 3.3 Pesca (il ciglio della piattaforma attraversa il riquadro)

Isobata dei 200 m (STIMA da geografia): (49.5, -11.6) → (49.0, -11.0) → (48.5, -9.8) → (48.0, -8.8) → (47.5, -7.9) → (47.2, -7.5).

| Flotta | Dove | Specie e stagione | Quanti |
|---|---|---|---|
| Spagnoli del "Gran Sol" (Vigo, Marín, A Coruña, Ondarroa, Pasajes), a strascico singolo e a coppia, più palangari | ciglio e scarpata 150–600 m, Porcupine, Goban Spur; nel riquadro la fascia di ±15 nm attorno all'isobata | nasello, rombo giallo (gallo), rana pescatrice, scampo; tutto l'anno | prima dell'adesione alla CEE (1986) erano circa **460** navi sul Gran Sol, ridotte poi a 179; dopo l'adesione esiste la lista dei "300" autorizzati nelle zone ICES VI, VII e VIIIabd [media, Pesca Internacional 2008 e OECD, estratti]. Nel riquadro **15–40** contemporaneamente [STIMA] |
| Francesi (Lorient, Concarneau, Douarnenez, Le Guilvinec), a strascico | piattaforma e ciglio, 100–400 m | nasello, scampo, rana, merlano; tutto l'anno | regole sulle maglie cambiate dal 1978 [bassa, IFREMER 1982]. Nel riquadro **10–30** [STIMA] |
| Pesca del tonno bianco (germon): francesi con reti derivanti dal 1987, spagnoli a traina (boniteros) | sopra la scarpata e la piana, a sud-ovest dell'isobata | giugno–ottobre | da decine a qualche centinaio in tutto il Golfo di Biscaglia; nel riquadro **10–50** d'estate [STIMA] |
| Pelagici invernali per sgombro e suro: olandesi, inglesi, navi-officina del blocco orientale ("klondykers") al largo della Cornovaglia | soprattutto a est del riquadro (50N 5W) | novembre–marzo | fuori dal riquadro: solo transiti [bassa] |

### 3.4 Traghetti

Nel riquadro non passa nessuna linea regolare. Roscoff–Cork, Le Havre/Cherbourg–Rosslare e Plymouth–Santander corrono tutte a est del 7,5W. [STIMA da geografia]

---

## 4. Baltico sud-occidentale (54,0–55,8N; 10,5–15,0E)

### 4.1 Corsie

- **Route T** (Skagen–Grande Belt–Langeland–Fehmarn–Kadetrinne–Gedser), la rotta di transito per acque profonde. Profondità massima ottenibile **17 m**; a nord-est di Gedser **16,4 m** per migrazione delle sabbie. Pilota raccomandato oltre 11 m di pescaggio. Le raccomandazioni IMO per gli accessi al Baltico partono dal 1975. [alta, DMA *Navigation through Danish waters* 2025; [media] per la data] Le navi pescano molto meno dei 17 m: la maggior parte del traffico sta *fuori* dalla rotta profonda.
- **Rotta profonda a nord-est di Gedser** (testo IMO SN.1/Circ.263, 2007, minimo 16,5 m) [alta]. Poligono: 54°27,10'N 12°10,50'E; 54°27,73'N 12°11,30'E; 54°31,30'N 12°12,80'E; 54°36,46'N 12°15,83'E; 54°46,86'N 12°43,23'E; 54°46,06'N 12°44,03'E; 54°35,36'N 12°16,93'E; 54°31,00'N 12°15,20'E; 54°27,40'N 12°13,10'E; 54°26,57'N 12°11,90'E. Chi non è obbligato dal pescaggio passa fuori: **verso est sul lato sud, verso ovest sul lato nord**. Negli anni '80 la rotta c'era, ma con una geometria forse un po' diversa. [STIMA]
- **Schemi di separazione oggi nel riquadro** (nomi dal DMA 2025, coordinate non estratte): "Between Korsør and Sprogø" (Grande Belt), "South of Gedser" più la rotta della Kadetrinne, "North of Rügen", "Bornholmsgat", "Adlergrund", "Off Falsterborev", e "Kiel Lighthouse", che è appena a ovest del riquadro. [alta per i nomi; data di istituzione non verificata: diversi sono successivi al 1991]

Polilinee per il generatore (**STIMA** da geografia e dai punti qui sopra; i tratti in profondità ricalcano la Route T):

| Corsia | Senso | Punti |
|---|---|---|
| B1 Route T, dal Grande Belt a Bornholm | doppio, separato in Kadetrinne e a Gedser | Korsør (55.30, 11.05) → (55.00, 10.95) Langeland est → (54.75, 11.05) → Fehmarn Belt (54.62, 11.30) → Kadetrinne (54.47, 11.95) → rotta profonda (54.45, 12.18) → (54.78, 12.72) → (54.85, 13.40) a nord di Rügen → Bornholmsgat (55.20, 14.30) → (55.30, 15.00) bordo est |
| B2 Kiel (Holtenau 54.37, 10.15)–Fehmarn Belt | doppio | (54.50, 10.50) → (54.55, 10.90) → si unisce a B1 a (54.60, 11.20). Il Canale di Kiel alimenta B2 |
| B3 Sund (Drogden 55.55, 12.70)–Baltico | doppio, Drogden 7–8 m | (55.55, 12.72) → (55.30, 12.78) al largo di Falsterbo → (55.05, 13.30) → si unisce a B1 o va a Bornholmsgat; un ramo (54.95, 12.90) verso Rügen e Swinoujscie |
| B4 Lubecca e Travemünde–Baltico | doppio | Travemünde (53.96, 10.88) → (54.20, 11.10) → Fehmarn Belt e si unisce a B1 |
| B5 costiero Rostock e Wismar | doppio | Warnemünde (54.18, 12.08) → (54.35, 12.10) → B1 |

### 4.2 Densità (moderna, dato primario) e riporto agli anni '80

Passaggi l'anno sulle linee fisse AIS HELCOM, 2008 [alta, HELCOM 2009]:

| Linea | Passeggeri | Carico | Cisterne | Altro | Totale | Al giorno |
|---|---|---|---|---|---|---|
| Kadet Fairway (Gedser–Darss) | 19 447 | 31 874 | 8 618 | 24 349 | **85 699** | 235 |
| Langeland est (Route T) | 1 546 | 8 648 | 4 643 | 4 472 | 19 712 | 54 |
| Grande Belt, ponte est | 1 506 | 9 160 | 4 791 | 6 048 | 21 966 | 60 |
| Drogden (Sund sud) | 9 965 | 20 219 | 4 991 | 22 370 | 58 487 | 160 |
| A nord di Bornholm | 2 402 | 32 869 | 8 509 | 12 597 | 57 579 | 158 |
| A sud di Bornholm | 928 | 8 655 | 1 743 | 3 852 | 15 452 | 42 |

La linea della Kadetrinne conta anche i traghetti Rostock–Gedser che la tagliano.

Il **Canale di Kiel** aveva circa 80 000 passaggi nel 1964 (picco), circa 35 000 nel 1999 (minimo) e 42 811 nel 2008. [media, de.wikipedia] Negli anni '80 erano **45 000–55 000 l'anno**, in maggioranza costiere piccole. [STIMA, interpolazione]

Proposta per il 1985–91, contando anche l'URSS, la DDR e la Polonia: Kadetrinne **150–190 al giorno** (circa 100 di transito più i traghetti); Route T Langeland e Grande Belt **35–45**; Sund (Drogden) **110–130**; nord di Bornholm **100–120**. Tra i mercantili: costiere molte (40 %), carico generale 30 %, cisterne 10–15 % (in crescita dai porti sovietici del Baltico), ro-ro e traghetti fuori orario, unità del Patto di Varsavia (navi spia AGI, mercantili sovietici). [STIMA, fattore 0,6–0,8 sul 2008]

### 4.3 Traghetti (anni '80)

| Linea | Durata | Corse al giorno per senso | Confidenza |
|---|---|---|---|
| **Puttgarden–Rødby** (Vogelfluglinie, DB e DSB) | circa 60 min | 1963: 18 coppie al giorno; anni '90: 48 coppie (ogni 30 min). Anni '80: **25–35 coppie** (ogni 40–60 min) | [media] per gli estremi, STIMA per gli anni '80 |
| **Warnemünde–Gedser** (DR e DSB, traghetto ferroviario *Warnemünde* dal 1963, linea chiusa a fine settembre 1995) | circa 2 h | 3–5 | [media] per la storia, STIMA per le frequenze |
| **Sassnitz–Trelleborg** (Königslinie, DR e SJ: *Sassnitz* fino al 1986, *Rügen*, *Rostock*, *Götaland*, *Trelleborg* dal 1982) | circa 4 h | a lungo "fino a 2 partenze per senso"; con i traghetti ferroviari anni '80 **3–5** | [media] per le navi, STIMA per le corse |
| **Mukran–Klaipėda** (traghetti ferroviari DDR-URSS dal **1986**, 5–6 navi gemelle) | circa 17 h | circa 1 al giorno, rotta est lungo il 55N, a sud di Bornholm | [media, conoscenza generale] |
| Travemünde–Trelleborg (TT-Line) | 7–8 h | 2 | [bassa] |
| Travemünde–Gedser | circa 3 h | 3–4 | [bassa] |
| Travemünde–Helsinki (*Finnjet*) | 22 h | circa 3–4 alla settimana | [bassa] |
| Kiel–Oslo (Jahre Line) e Kiel–Göteborg (Stena Line) | notturni | 1 ciascuno, ma nel riquadro passano solo all'imbocco della baia di Kiel | [bassa] |
| Kiel–Bagenkop (Langeland) | 2 h 30 | 2–4 | [bassa] |
| Grande Belt Halsskov–Knudshoved (DSB, treni e auto) | circa 60 min | ogni 30–60 min, 20–30 coppie | STIMA |
| Spodsbjerg–Tårs (Langeland–Lolland) | 45 min | circa ogni ora | [bassa] |
| Dragør–Limhamn; Copenaghen–Malmö (aliscafi) | 50 min; 35–45 min | circa ogni ora; ogni 30–60 min | [bassa] |
| Rønne–Copenaghen; Rønne–Ystad (Bornholmstrafikken) | 7 h; 2 h 30 | 1–2; 2–3 | [bassa] |
| Świnoujście–Ystad (Polferries) | 7 h | 1 | [bassa] |

### 4.4 Pesca

| Zona | Flotta | Stagione |
|---|---|---|
| Arkona e Bornholm (54.6–55.6N, 13.5–15E): bacini profondi 40–90 m | danesi di Bornholm, svedesi, tedeschi orientali e polacchi: merluzzo con strascico e reti da posta; pelagici per spratto e aringa | merluzzo: inverno e primavera (riproduzione a Bornholm, maggio–agosto); aringa di primavera a Rügen (Greifswalder Bodden), marzo–maggio |
| Baia di Kiel, Fehmarn, Mecklemburgo (15–30 m) | piccoli strascicanti e reti da posta tedeschi e danesi | tutto l'anno; reti da posta fisse sotto costa |

Contatti: **20–50** nel riquadro. Pescano di giorno e tornano in porto la sera. [STIMA]

### 4.5 Ghiaccio e rompighiaccio

- Inverni: il 1986/87 fu **molto severo** nel Baltico occidentale e meridionale, secondo solo al 1962/63 [media, BSH]. Severi anche il 1984/85 e il 1985/86 (si vedano anche `03` e §5.7 di `09`).
- Rompighiaccio presenti: **Stephan Jantzen** (DDR, costruito in URSS nel 1967 e in servizio dal 1968, Rostock e Warnemünde; sostituito dall'*Arkona* nel 2004); **Hanse** (RFT, costruito in Finlandia nel 1965–66, ritirato nel 1998); rompighiaccio statali danesi *Danbjørn* e *Isbjørn* (1965) e *Thorbjørn* (circa 1980); piccoli rompighiaccio della marina e della WSA tedesche (*Eisvogel*, *Eisbär*, *Widder*, *Stier*) per la baia di Kiel, il canale e i porti. [media, en.wikipedia "Icebreakers of Germany"; [bassa] per i danesi]
- Per il gioco: negli inverni severi, convogli dietro un rompighiaccio sulle corsie B1, B4 e B5 (Rostock, Warnemünde, Lubecca), traghetti di linea in ritardo, mercantili che restano fermi. Negli inverni normali il ghiaccio resta nei porti e nelle lagune (Bodden). [STIMA]

---

## 5. Barents e Kola (69,2–72,0N; 31–39E)

### 5.1 Corsie (nessun TSS sovietico; **STIMA** da geografia e da archi calcolati)

| Corsia | Senso | Punti | Note |
|---|---|---|---|
| K0 Golfo di Kola (Murmansk 68.97, 33.07 → imbocco 69.25, 33.55) | doppio, canale di circa 1–2 nm | Murmansk → Severomorsk (69.07, 33.42) → Polyarny (69.20, 33.45) → imbocco (69.30, 33.60) | porto commerciale, porto peschereccio, base Atomflot dei rompighiaccio nucleari, basi della Flotta del Nord |
| K1 costiera verso il Mar Bianco (Arcangelo, Kandalaksha) | doppio, 8–15 nm da costa | (69.35, 33.80) → (69.40, 35.20) Teriberka → (69.25, 36.50) → (68.95, 37.80) → (68.55, 39.00) [bordo est] → Svyatoy Nos e Gorlo | tutto l'anno; d'inverno, nel Mar Bianco, con il rompighiaccio |
| K2 Rotta del Mare del Nord (Kara Gate 70.40, 57.80) | doppio | (69.45, 33.75) → (69.74, 36.59) → (69.99, 39.00) [bordo] | nel 1987 picco storico di **6,58 Mt** l'anno [media, USNI 1999; Korabel]; Murmansk–Dudinka tutto l'anno dal 1978–79 con rompighiaccio nucleari |
| K3 verso ovest (Kirkenes, Vardø 70.35, 31.25, Europa) | doppio | (69.45, 33.75) → (69.79, 32.84) → (70.13, 31.90) → (70.24, 31.57) [bordo ovest 31E] | mercantili, pescherecci verso i banchi occidentali, transiti militari |
| K4 verso Svalbard e Barentsburg e i banchi del nord | doppio | (69.45, 33.75) → (70.60, 32.25) → (71.74, 30.57), che esce a ovest a 71.2N | navi sovietiche per le miniere di Barentsburg e Pyramiden; pescherecci |

### 5.2 Densità

Dati d'epoca sul numero di navi non ne ho trovati. Proposta per il 1985–91: Golfo di Kola **30–60 movimenti al giorno** (mercantili 10–20, pescherecci 15–30, servizio e militari 5–15); K1 **5–10 al giorno**; K2 **2–6 al giorno**, con picchi da luglio a ottobre; K3 **3–8**; K4 **1–3**. Composizione: rinfusiere e mercantili rinforzati per il ghiaccio della compagnia di navigazione di Murmansk (tipo SA-15 *Norilsk*: 20 000 t di portata, 15 unità costruite in Finlandia, **1982**, rompe circa 1 m di ghiaccio [media, CSMonitor 30.1.1985]), cisterne della Flotta del Nord, navi madri e officine, pescherecci. [STIMA]

### 5.3 Pesca

| Zona (STIMA) | Flotta | Stagione |
|---|---|---|
| Banchi della costa del Murman (Kildin, Rybachy), 69.4–70.3N, 32–37E, a 100–250 m | strascicanti sovietici di Murmansk (Murmansk Trawl Fleet, Sevryba): BMRT e RTM a poppa, SRT costieri | merluzzo e eglefino in inverno e primavera (gennaio–aprile); scorfano; gamberetto (*Pandalus*) tutto l'anno sui fondi più profondi |
| Capelin lungo la costa, in migrazione per deporre le uova | sovietici e norvegesi con circuizione e pelagico | gennaio–aprile; **pesca chiusa nel 1987–1990** per il crollo dello stock [media] |
| "Zona grigia" (accordo provvisorio 1978, circa 60 000 km², rinnovato ogni anno fino al 2010) | sovietici e norvegesi, ciascuno controllato dal proprio Stato | tutto l'anno [media, Wikipedia e FNI] |

La zona grigia sta fra la linea di settore (32°04'E) e la linea mediana, circa 70.5–74N e 32–37E. [STIMA]

Quanti pescherecci: Murmansk nel 1955 aveva **562** strascicanti [media, Popov e Zeller 2018]. Lo strascico domina dal 1950 al 1980 e cala negli anni '80 con il declino di merluzzo ed eglefino. Nel 1989 la quota di merluzzo è al minimo storico, **160 000 t**. [media] Nel riquadro **15–40** pescherecci contemporaneamente, raggruppati sui banchi. [STIMA]

### 5.4 Rompighiaccio

- Il Golfo di Kola **non gela** (corrente del Capo Nord). Il ghiaccio e le scorte servono per il Mar Bianco (Arcangelo: oggi circa 170 giorni di navigazione invernale assistita; Mar Bianco oltre 96 [media, Rosmorport]) e per il Kara, verso est.
- Flotta nucleare con base a Murmansk (Atomflot) negli anni '80: *Lenin* (1959–1989; nel 1985 aveva scortato **2800** navi [media, CSMonitor 1985]), *Arktika* (1975), *Sibir* (1977), *Rossiya* (1985, quarta nucleare), *Taymyr* (1989) e *Vaygach* (1990), questi due a pescaggio ridotto, *Sovetsky Soyuz* (1990), più la portachiatte nucleare *Sevmorput* (1988). [media]
- Diesel per il Mar Bianco e il Barents (classi Moskva, Kapitan Sorokin, Kapitan Belousov e altre): circa un centinaio fra rompighiaccio minori e mercantili rinforzati costruiti da URSS e Finlandia fra il 1959 e il 1989. [media, USNI 1999]
- Per il gioco: nel riquadro un rompighiaccio è un contatto **in transito** sulle corsie K1 e K2, da novembre a giugno, spesso in testa a un convoglio di 2–6 mercantili. Firma acustica molto forte (diesel-elettrico o turbo-elettrico nucleare, eliche in acqua aperta). [STIMA]

---

## 6. Faroe e varco Islanda–Faroe–Shetland (60,5–63,5N; 10,0–4,0W)

### 6.1 Corsie (archi di cerchio massimo calcolati, **STIMA**)

| Corsia | Senso | Punti nel riquadro | Note |
|---|---|---|---|
| F1 Fair Isle–Reykjavík | doppio | (60.57, -5.04) → (60.91, -6.24) → **a sud delle Faroe** (61.15, -7.10) → (61.55, -8.71) → (61.85, -9.98) | il cerchio massimo taglia Suðuroy: si devia di qualche miglio a sud dell'isola, fuori dal canale del Banco delle Faroe |
| F2 Pentland Firth–Reykjavík | doppio | (60.50, -7.55) → (60.66, -8.82) → (61.02, -9.94) | a sud del Banco delle Faroe (60.9N 8.5W) |
| F3 Bergen e Norvegia–Islanda | doppio | (62.40, -4.36) → (62.92, -7.70) → (63.36, -11.15) | a nord delle Faroe |
| F4 Tórshavn–Scrabster e Aberdeen | doppio | Tórshavn (62.00, -6.75) → (61.54, -6.18) → (61.12, -5.78) → (60.50, -5.20) | traghetti e cargo faroesi |
| F5 Tórshavn–Lerwick, Bergen, Danimarca | doppio | (61.95, -6.60) → (61.52, -5.17) → (61.30, -4.46) → bordo (61.2, -4.0) | Smyril Line, cargo |
| F6 Tórshavn–Seyðisfjörður | doppio | (62.38, -7.42) → (62.81, -8.27) → (63.24, -9.14) → bordo (63.5, -9.7) | solo d'estate (oggi pausa da metà novembre a metà marzo) [media] |
| F7 Fair Isle–Capo Farewell e Canada; Pentland–Belle Isle | doppio | passano a sud del riquadro (60.15N a 6.6W; 59.2N a 10.7W) | si sentono a distanza a sud, in zona di convergenza |

Le navi sovietiche da Murmansk verso i banchi di Terranova (NAFO) seguono il cerchio massimo per lo Stretto di Danimarca (69N 8W → 66N 20W), a nord dell'Islanda: fuori dal riquadro. Quando lo stretto è chiuso dal ghiaccio o dal tempo, passano nel varco Islanda–Faroe. [STIMA]

### 6.2 Densità

Dati diretti non trovati. Il Pentland Firth oggi ha circa **6000–6500** navi l'anno [bassa, caithness.org]; le cisterne cariche sono invitate a usare il Fair Isle Channel invece del Pentland in caso di cattivo tempo [media]. Proposta per il riquadro, 1985–91: **5–15 mercantili al giorno** in tutto (F1+F2 3–8, F3 1–3, F4–F6 1–3), più transiti di pescherecci sovietici e del blocco orientale verso l'Atlantico occidentale (**2–6 al giorno**). [STIMA]

### 6.3 Pesca

| Zona | Flotta | Stagione |
|---|---|---|
| Piattaforma delle Faroe (fino a 200 m, raggio di circa 30–60 nm dalle isole) | faroesi: palangari, strascico a coppia, strascicanti | merluzzo, eglefino, merluzzo carbonaro tutto l'anno. Le licenze di pesca arrivano nel **1987**; dal 1985 c'è un piano di costruzione di navi faroesi [media, FAO] |
| Banco delle Faroe (60.7–61.1N, 8.0–9.0W, 90–200 m) | faroesi; prima del 1977 anche britannici | tutto l'anno |
| Scarpata ovest delle Faroe e canale Faroe–Shetland (pesce azzurro, *blue whiting*) | faroesi, norvegesi, sovietici (pelagici, BMRT) | concentrazioni di riproduzione da **gennaio ad aprile**, picco da metà marzo a metà aprile, lungo il ciglio dal Porcupine fino alla dorsale Faroe–Shetland; nel 1980 circa 5,4 Mt stimate [media, ICES 1980 e IMR 1990] |

Nel 2002 la flotta faroese contava **514** navi, il 63 % sotto le 20 tsl. [media, FAO] Contatti di pesca nel riquadro: **10–30**, concentrati sulla piattaforma; in primavera si aggiungono 10–20 pelagici sul ciglio ovest. [STIMA]

### 6.4 Traghetti

| Linea | Frequenza | Confidenza |
|---|---|---|
| Smyril Line, *Norröna* (del 1973): Tórshavn–Lerwick–Bergen, Hanstholm, Seyðisfjörður; Lerwick da aprile **1983** | **settimanale**, d'estate; in inverno ridotta | [media] |
| Strandfaraskip Landsins, linee interne: Tórshavn–Tvøroyri (Suðuroy) e altre | 2–3 corse al giorno, circa 2 h | [bassa] |
| P&O Scottish Ferries Aberdeen–Lerwick, con estensioni estive verso Bergen e Tórshavn | fuori dal riquadro, salvo le estensioni estive | [bassa] |

---

## 7. Kunashir e Okhotsk meridionale (43,3–45,6N; 144,8–148,5E)

### 7.1 Stretti e corsie

| Stretto | Dati | Fonte |
|---|---|---|
| **Kunashir** (Nemuro superiore; giapp. Nemuro kaikyō), fra Kunashir e Shiretoko | lungo 74 km, largo 24–43 km; d'inverno pieno di ghiaccio; ci passa il confine di fatto URSS-Giappone | [media, ru.wikipedia] |
| **Izmeny** (Notsuke suidō), fra Kunashir e la penisola di Notsuke | lungo circa 24 km, largo 16,3 km nel punto più stretto; profondità 4,5–27 m, ma **fondale limitante 7 m** con canale che cambia; nebbie; solo naviglio piccolo; **gela a gennaio** | [media, ru.wikipedia] |
| **Ekaterina**, fra Kunashir e Iturup | largo circa 21 km, lungo circa 15 km, profondo oltre 400 m; non gela, ma da febbraio a marzo è pieno di ghiaccio alla deriva; scogli su entrambe le sponde | [media, ru.wikipedia] |
| **Curili meridionale**, fra Kunashir e Shikotan/Habomai | punto più stretto circa 34 km (fra Tanfilyev e Kunashir); Yuzhno-Kurilsk sulla riva | [media, ru.wikipedia] |

Corsie (**STIMA** da geografia):

| Corsia | Senso | Punti | Note |
|---|---|---|---|
| Ku1 Korsakov (Sakhalin)–Ekaterina–Yuzhno-Kurilsk | doppio | dall'Okhotsk (45.60, 146.00) → (44.70, 146.55) → stretto di Ekaterina (44.43, 146.72) → (44.10, 146.30) → Yuzhno-Kurilsk (44.03, 145.86) | navi di linea e di rifornimento sovietiche; nave passeggeri ogni 7–10 giorni [bassa] |
| Ku2 Yuzhno-Kurilsk–Malokurilskoye (Shikotan, 43.87, 146.82) | doppio | attraverso lo Stretto delle Curili meridionale | navi frigorifere, scatolifici del granchio e del saury |
| Ku3 Ekaterina–Pacifico (Petropavlovsk e oltre) | doppio | (44.43, 146.72) → (44.00, 147.20) → (43.50, 148.00) | ramo verso il Pacifico della flotta mercantile e militare sovietica; anche gli stretti di Frieza e Bussol, più a nord-est, fuori dal riquadro |
| Ku4 Kurilsk (Iturup, lato Okhotsk, 45.23, 147.88) | doppio | da Ekaterina lungo la costa nord-ovest di Iturup: (44.60, 146.90) → (45.00, 147.40) → (45.23, 147.85) | |
| Ku5 giapponese di Nemuro (lato giapponese del Kunashir) | doppio, sotto costa a Hokkaido | Rausu (44.02, 145.19) → Shibetsu (43.66, 145.13) → Notsuke (43.58, 145.30) → stretto di Izmeny lato giapponese → Nemuro (43.33, 145.58) | pescherecci e piccole costiere giapponesi; le navi grandi passano dal Sōya |
| Ku6 sovietica di sorveglianza della linea mediana del Kunashir | pattuglia | lungo la linea mediana, circa 44.0N 145.4E → 43.65N 145.45E | Guardia di frontiera del KGB: motovedette, fermo di pescherecci giapponesi |

### 7.2 Densità

Mercantili pochi: **2–8 movimenti al giorno** sovietici in tutto il riquadro (Ku1–Ku4), più 1–3 militari o della guardia di frontiera; sul lato giapponese, Ku5, **5–20 piccole costiere** al giorno. Il carico della scena viene dalla pesca (§7.3). [STIMA]

### 7.3 Pesca (la vera "massa" di contatti)

| Attività | Dove | Stagione | Flotta |
|---|---|---|---|
| **Kombu** giapponese presso Kaigara (Signalny, Habomai, 43.4N 145.9E) con l'accordo privato | acque costiere sovietiche delle Habomai | **settembre** | 1981: **330 barche al giorno**, primo raccolto in cinque anni; prima del 1976 se ne raccoglievano circa 960 t l'anno; circa 500 pescatori di Nemuro con licenze per le acque costiere sovietiche [media, CSMonitor 9.9.1981] |
| Saury (sanma) con lampade e bilancia | lato Pacifico delle Curili meridionali (43.3–44.5N, 145.5–148.5E) | **agosto–novembre**, di notte | giapponesi di Hanasaki e Nemuro (barche da 10 a 200 tsl) [media, NPFC]; sovietici con frigorifere |
| Salmone | Okhotsk e Pacifico; reti fisse giapponesi sotto costa di Shiretoko e Nemuro | maggio–agosto; la flotta giapponese con navi madri chiude prima del 1992 | [media, documenti dell'Alaska e NMFS] |
| Granchio | Shikotan, Habomai, Kunashir | primavera e autunno | sovietici e giapponesi; i fermi di pescherecci giapponesi sono frequenti [media, USNI 1959 per il precedente] |
| Merluzzo dell'Alaska (pollock) | Okhotsk, lato nord del riquadro | gennaio–aprile, ai margini del ghiaccio | strascicanti sovietici (BMRT); il grosso nell'Okhotsk nord-occidentale |

Contatti: d'estate e d'autunno **50–200** nel riquadro (di notte i saurieri accesi a decine, fermi o lentissimi); d'inverno **5–20**. [STIMA] Dal 1976 le acque sovietiche si estendono a 200 nm; nel mezzo dell'Okhotsk resta solo il "Peanut Hole", circa il 2,5 % del mare. [media]

### 7.4 Ghiaccio

Il ghiaccio dell'Okhotsk arriva a Kunashir e Shiretoko da **gennaio–febbraio ad aprile**. Il Kunashir è pieno di ghiaccio d'inverno; Izmeny gela a gennaio; Ekaterina ha ghiaccio alla deriva a febbraio e marzo. [media] Rompighiaccio locali: nessuna fonte. Lo stretto d'inverno è chiuso alla navigazione minore e il traffico sovietico si sposta sul lato Pacifico. [STIMA]

---

## 8. Sintesi per il generatore: navi al giorno nel riquadro, 1985–91

| Teatro | Mercantili in transito | Traghetti e linea | Pesca contemporanea | Speciali |
|---|---|---|---|---|
| Gibilterra | 130–150 | 25–40 traversate (estate ×2) | 20–60 di giorno, 50–120 di notte | almadrabe fisse da aprile a luglio; rimorchiatori e bunkeraggio |
| Otranto | 40–60 | 2–6 (inverno), 10–20 (estate) | 40–100 nei giorni feriali | navi NATO e jugoslave |
| Celtica (riquadro) | 25–45 | 0 | 25–70 sul ciglio | Ouessant (110–140/g) è a est del riquadro |
| Baltico SW | 150–190 alla Kadetrinne, più gli altri fasci | molto alto: Puttgarden circa 50–70 traversate, Grande Belt 40–60 | 20–50 | ghiaccio nel 1985–87 |
| Barents | 15–40 in tutto | locale | 15–40 | rompighiaccio da novembre a giugno su K1 e K2; Flotta del Nord |
| Faroe | 5–15 | settimanale più linee interne | 10–30, primavera +10–20 | transiti di pesca sovietici 2–6/g |
| Kunashir | 2–8 sovietici, 5–20 costiere giapponesi | ogni 7–10 giorni | 50–200 (estate e autunno), 5–20 (inverno) | Guardia di frontiera sovietica; ghiaccio da gennaio ad aprile |

Tutte le righe sono **STIMA**, costruite sui dati delle sezioni precedenti.

---

## 9. Fonti

- Mavor, J. W. Jr., "Ship Traffic through Gibraltar Strait", *The Journal of Navigation* 33(3), 1980, pp. 408–417 (abstract). https://www.cambridge.org/core/journals/journal-of-navigation/article/ship-traffic-through-gibraltar-strait/6394B298DB4777AADC4BC5A2BBF73A9E
- Spagna, delegazione all'IMO, annesso sul sistema di rapportazione di Gibilterra (1996), raccolta Nippon Foundation. https://nippon.zaidan.info/seikabutsu/1996/00179/contents/026.htm
- IMO COLREG.2/Circ.66 (emendamenti al TSS di Gibilterra, in vigore dall'1.6.2015), solo l'estratto: il server non rispondeva. https://www.vimawa.gov.vn/f/Circ%2066.pdf
- Fundación MAPFRE, sul VTS di Tarifa del 1987 e sui 70 000 transiti (estratto). https://documentacion.fundacionmapfre.org/documentacion/pt/media/group/1014372.do
- Porto di Algeciras e OPE: https://portusonline.org/background-the-crossing-of-the-strait-operation-as-a-configurative-fact-for-the-port-of-algeciras/ ; es.hispanopedia "Operación Paso del Estrecho".
- Congreso de los Diputados, diario delle sessioni, sul ritiro della flotta dal Marocco (31.12.1987): https://www.congreso.es/public_oficiales/L3/CONG/DS/CO/CO_255.PDF ; BOE 23.11.1988: https://www.boe.es/boe/dias/1988/11/23/pdfs/A33284-33285.pdf (letti negli estratti).
- IMO SN/Circ.232 (16.9.2003), ordinanza del Prefetto marittimo 2003/11, allegato A sul DST di Ouessant. https://fma.fo/wp-content/uploads/2016/09/2013_Nautical_02_SN-Circ.232.pdf
- La Libre Belgique, "Le rail d'Ouessant", 16.12.2002. https://www.lalibre.be/international/2002/12/16/le-rail-douessant-TCS6N2HFBZHEDL7DYZTSO5EPII/
- bateaux.com / boatnews, "Rail d'Ouessant: comment le naufrage de l'Amoco Cadiz a changé la circulation maritime" (2023). https://www.bateaux.com/article/43436/
- Chalk, R. F. e Coupard, A., "Mid-Channel Survey of Vessels Routed past Ushant and Cherbourg", *J. Navigation* 33(3), 1980 (abstract). https://www.cambridge.org/core/journals/journal-of-navigation/article/abs/midchannel-survey-of-vessels-routed-past-ushant-and-cherbourg/ECFBABF7BB18E9654329732BC4F4462C
- Senato francese, rapporto 1978–79 n. 65 (estratto). https://conferenceconsensuslogement.senat.fr/rap/1978-1979/i1978_1979_0065.pdf
- UK P&I Club, bollettino 300-04/03 (2003). https://ukpandi.com/news-and-resources/bulletins/2003/300-0403-amended-traffic-separation-scheme-ushant
- HELCOM, *Annual report on shipping accidents in the Baltic Sea area during 2008* (2009), tabelle 1–3. https://helcom.fi/wp-content/uploads/2019/08/Annual-report-on-shipping-accidents-in-the-Baltic-Sea-area-during-2008.pdf
- Danish Maritime Authority, *Navigation through Danish waters* v16.0 (2025), con IMO SN.1/Circ.263 (2007). https://www.soefartsstyrelsen.dk/Media/638743469510433298/Navigation%20through%20Danish%20Water%20version%2016.0%202025.pdf
- de.wikipedia: "Königslinie", "Vogelfluglinie", "Nord-Ostsee-Kanal"; en.wikipedia: "Icebreakers of Germany", "Smyril Line", "Grey Zone Agreement", "Kunashir Island".
- ice-treff.de, forum su Warnemünde–Gedser, che cita *Brückenschlag über die Ostsee* (Rostock 2003) [bassa]. https://www.ice-treff.de/index.php?id=259517
- BSH, rassegna degli inverni di ghiaccio (estratto). https://digitale-bibliothek.bsh.bund.de/
- Christian Science Monitor, "Soviets' merchant fleet a stiff competitor. Murmansk…", 30.1.1985. https://www.csmonitor.com/1985/0130/fsoviet.html
- USNI Proceedings, "Professional Notes", gennaio 1999 (traffico sulla Rotta del Mare del Nord, rompighiaccio). https://www.usni.org/magazines/proceedings/1999/january/professional-notes ; Korabel.ru sul record del 1987.
- Popov e Zeller, *Frontiers in Marine Science* 5:266 (2018), ricostruzione delle catture russe nel Barents. https://www.frontiersin.org/journals/marine-science/articles/10.3389/fmars.2018.00266/
- Études internationales 18(1), 1987, sulla pesca sovietica e la commissione mista. https://www.erudit.org/en/journals/ei/1987-v18-n1-ei3028/702129ar.pdf
- Rosmorport, filiale di Arcangelo (navigazione invernale, moderna). https://www.rosmorport.com/filials/arf_news_main/23815/
- ICES CM 1980/H:32 (rilievo acustico del pesce azzurro) e IMR Brage 11250/107826 (pesce azzurro 1980–88). https://www.ices.dk/sites/pub/CM%20Doccuments/1980/H/1980_H32.pdf
- FAO, profilo di pesca delle Faroe. https://www.fao.org/fishery/docs/DOCUMENT/fcp/en/FI_CP_FO.pdf
- caithness.org, traffico del Pentland Firth [bassa]. https://caithness.org/pentlandfirth2/shipping.htm ; gov.scot, *Shipping Study of the Pentland Firth and Orkney Waters*.
- Christian Science Monitor, "Japan seaweed harvesting gets Soviet approval", 9.9.1981. https://www.csmonitor.com/1981/0909/090944.html
- ru.wikipedia: "Пролив Екатерины", "Пролив Измены", "Южно-Курильский пролив", "Кунаширский пролив".
- NPFC 2019, *Fishery Status in Japan* (saury, bilancia). https://www.npfc.int/
- Pesca Internacional 2008 (flotta del Gran Sol: da 460 a 179 navi) e OECD sulla "flotta dei 300" (estratti). https://search.oecd.org/greengrowth/fisheries/34431325.pdf
- ARPA Puglia (flotta 2012) e decreti sul fermo biologico 1995–96 (edizionieuropee.it), estratti.

---

## 10. Lacune

1. **Coordinate ufficiali del TSS di Gibilterra**, sia anni '80 sia moderne. La circolare IMO non era raggiungibile. Serve la carta IHM 445 o la BA 142, oppure l'edizione d'epoca di *Ships' Routeing*. Le polilinee del §1.1 sono **STIMA**.
2. **Geometria dello schema di Ouessant 1979–2003.** È nota solo la struttura (tre corsie, inizio a 5 nm); le coordinate del §3.1 sono ricostruite. Servono un avviso ai naviganti SHOM del 1979 o *Ships' Routeing* 1980–90.
3. **Conteggi d'epoca di navi al giorno**: manca un dato anni '80 per Ouessant (lo studio Chalk e Coupard 1980 ce l'ha, ma l'articolo intero non era accessibile), Otranto, Baltico, Faroe, Barents e Curili. Ho trovato solo Gibilterra 1980 e il Baltico moderno.
4. **Orari dei traghetti anni '80**: per Brindisi, Algeciras e le linee baltiche ho solo storia e ordini di grandezza. Servono il Thomas Cook European Timetable (1985–90) o il Kursbuch DR/DB per i traghetti ferroviari.
5. **Numero di pescherecci d'epoca** per flotta (Murmansk, Puglia, Faroe, Gran Sol, saury giapponese): quasi solo dati moderni o di altri anni.
6. **Data di istituzione dei TSS baltici** (Gedser, Kadet, nord di Rügen, Bornholmsgat, faro di Kiel): non so quali esistessero prima del 1991.
7. **Rompighiaccio danesi e tedeschi occidentali**: basi e date d'impiego negli inverni 1985–87 non verificate. Il Far East sovietico per Kunashir non è coperto.
8. **Rotte sovietiche nelle Curili meridionali** (linee passeggeri da Korsakov, frequenza, uso militare di Ekaterina): nessuna fonte primaria; tutto **STIMA**.
9. **Poligoni di pesca precisi**: le zone sono descrittive e approssimate; mancano le carte di pesca d'epoca (ICES, PINRO, IREPA).
