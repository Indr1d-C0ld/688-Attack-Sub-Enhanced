# 21-A. Climatologia dei teatri e modello di meteo e mare dinamico

Nota di ricerca per il remake di *688 Attack Sub* (ambientazione 1985–1990). Parte A del dossier "meteo e stato del mare come un unico sistema".
Contenuto: climatologia per teatro e per mese o stagione, eventi estremi e loro rarità, ciclo diurno, basi fisiche e statistiche per un generatore di meteo deterministico (con seme), previsioni del 1989.

## 0. Metodo, legenda e avvertenze

**Etichette di fiducia** (per ogni dato):
- **alta**: atlante ufficiale, rianalisi o articolo con revisione paritaria;
- **media**: fonte secondaria seria, abstract, guida tecnica, oppure nostra elaborazione di dati di fiducia alta;
- **bassa**: forum, guida turistica o gioco.

**Altre marcature:**
- **[GIOCO]**: proposta per il motore di gioco.
- **STIMA**: numero inventato o ricavato per analogia, da tarare.
- **n.v.**: dato non trovato.

### 0.1 Dati ERA5 elaborati per questa nota

La spina dorsale numerica sono le **rianalisi ERA5** (ECMWF/Copernicus; Hersbach et al. 2020, https://doi.org/10.1002/qj.3803), scaricate tramite le API gratuite di Open-Meteo:
- atmosfera: https://open-meteo.com/en/docs/historical-weather-api, modello `era5`, griglia 0,25°;
- onde: https://open-meteo.com/en/docs/marine-weather-api, modello `era5_ocean`, griglia 0,5°.

Caratteristiche dell'estrazione:
- **Periodo:** 1° gennaio 1981 – 31 dicembre 1990, dati orari (87 648 ore per punto). Il decennio comprende l'ambientazione del gioco.
- **Variabili:** vento a 10 m (velocità, direzione, raffica), temperatura e punto di rugiada a 2 m, umidità relativa, pressione al livello del mare, nuvolosità totale e bassa, precipitazione, neve, temperatura del primo strato di superficie (sul mare approssima la SST; vedi avvertenze), altezza significativa Hs, periodo medio e direzione dell'onda.
- **Elaborazione:** script Python di questa sessione. Gli script non sono nel repository; si rigenerano in pochi minuti.
- **Fiducia:** dato alta; statistiche derivate media.

Punti usati: uno per teatro, in mare aperto dentro il riquadro di `tools/pipeline/teatri.yaml`.

| Teatro | Punto atmosfera | Punto onde | Note |
|---|---|---|---|
| 1 Faroe – Mar di Norvegia | 61,75 N 8,00 W | 62,0 N 8,0 W | a W delle Faroe, verso il Faroe Bank Channel |
| 2 Barents (approcci a Kola) | 70,5 N 35,0 E | idem | mare aperto a NNE di Murmansk |
| 3 Gibilterra | 36,0 N 5,0 W | idem | imbocco orientale (Alborán occidentale). Il punto nello stretto (35,75 N 5,5 W) è contaminato dalla costa e dà venti troppo deboli: scartato |
| 4 Otranto | 40,5 N 18,75 E | 40,5 N 19,0 E | centro del canale |
| 5 Kunashir – Okhotsk | 45,0 N 146,5 E | idem | lato Okhotsk, a N dello stretto di Ekaterina |
| 5b Curili, lato Pacifico | 43,75 N 147,5 E | 43,5 N 147,5 E | a SE di Kunashir/Iturup, per confronto |
| 6 Piattaforma Celtica | 48,5 N 10,0 W | idem | ciglio della piattaforma |
| 7 Baltico SW | 55,0 N 14,0 E | idem | bacino di Arkona, a W di Bornholm |

### 0.2 Avvertenze sui dati ERA5 (leggere prima delle tabelle)

1. **I venti forti sono sottostimati.** ERA5 è una media su celle di circa 30 km e su un'ora. Burrasche locali, raffiche nei groppi, getti negli stretti e polar low piccoli sono smussati.
   - Confronto: l'atlante US Navy dà burrasche (≥ Bft 8) per il **12–14 %** del tempo in inverno nel Mare del Nord centrale e nella Manica occidentale (fonte §2.6). ERA5 al ciglio celtico dà 4–5 %.
   - A Gibilterra l'atlante dà burrasche per il **2–4 %** del tempo in inverno (§2.3); ERA5 all'imbocco dà 0,2–0,8 %.
   - **[GIOCO]** Usare ERA5 per forma, stagionalità e persistenza. Per le code alzare la frequenza delle burrasche di un fattore **1,5–3 (STIMA)**, maggiore negli stretti.
2. **Visibilità e nebbia non sono in ERA5** (nell'archivio Open-Meteo non c'è la visibilità ERA5).
   - L'indice "% ore con UR ≥ 97 %" è solo un **indicatore debole e sottostimato**: ERA5 satura raramente a 2 m. Usarlo per il ciclo annuo e giornaliero, non per il valore assoluto.
   - I valori assoluti di nebbia vengono dalle osservazioni da nave (atlante US Navy, ICOADS) citate teatro per teatro.
3. **La temperatura del mare è un proxy.** È la temperatura del primo strato di superficie ERA5 sul punto marino. I valori sono coerenti con le SST note (Faroe 7–10,5 °C, Barents 2,2–8,7 °C, Otranto 14–25 °C). Con ghiaccio diventa la temperatura del ghiaccio: per esempio −3 °C a Kunashir in febbraio–marzo.
4. **Ghiaccio:** dove il modello d'onda ERA5 non dà Hs in mare aperto, il punto è coperto da ghiaccio. La riga "% ore senza dato d'onda" è quindi un **proxy di presenza del ghiaccio** (Kunashir, Baltico).
5. **Un punto non è il teatro.** Dentro un riquadro di 150–300 km ci sono gradienti forti: ridossi, stretti, bordo del ghiaccio, fronti termici. **[GIOCO]** Modulare spazialmente con regole semplici (§7.6).

### 0.3 Corrispondenze usate

Beaufort, in m/s (vento medio a 10 m):

| Forza | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
|---|---|---|---|---|---|---|---|
| m/s | 10,8–13,8 | 13,9–17,1 | 17,2–20,7 | 20,8–24,4 | 24,5–28,4 | 28,5–32,6 | ≥ 32,7 |

Stato del mare Douglas/WMO da Hs:

| Classe | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Hs (m) | 0 | 0–0,1 | 0,1–0,5 | 0,5–1,25 | 1,25–2,5 | 2,5–4 | 4–6 | 6–9 | 9–14 | > 14 |

Fonte: WMO Code 3700 (https://library.wmo.int/idurl/4/35713, WMO-No. 306). Fiducia alta.

---

## 1. Sintesi comparativa

| Teatro | Inverno (DGF) | Estate (GLA) | Pericolo meteo-marino dominante | Visibilità: problema principale |
|---|---|---|---|---|
| 1 Faroe | Il più duro dei sette. Vento medio 10,7 m/s, Hs media 4 m, Hs ≥ 4 m nel 42–48 % delle ore; burrasca ≥ Bft 8 circa ogni 6 giorni (ERA5) | Hs 1,6 m, raramente burrasca | Mare lungo e alto in inverno (periodo 10 s), swell atlantico | Pioggia e piovaschi in inverno; nebbia d'avvezione estiva |
| 2 Barents | Vento 9,2 m/s, Hs 2,4 m; neve per il 30–40 % delle ore; spruzzo gelante; polar low | Calmo, Hs 1,1 m | Polar low improvvisi, icing, fumo di mare artico | Neve e fumo di mare in inverno, nebbia estiva |
| 3 Gibilterra | Levante e Poniente alternati (E 36 %, W 43 % delle ore); Hs 1 m ma mare corto e ripido | Levante frequente | Vento incanalato a getto, mare ripido contro corrente | Nebbie di Levante (luglio–agosto: < 2 nm per il 6–8 % delle osservazioni) |
| 4 Otranto | Bora e scirocco; Hs 0,9 m; temporali autunnali | Calmo e limpido, maestrale di Otranto da N | Bora improvvisa, scirocco con pioggia e mare; temporali e trombe marine | Pioggia e scirocco in inverno, foschia estiva |
| 5 Kunashir | Monsone da NW, neve per il 30 % delle ore, **ghiaccio** da febbraio ad aprile, spruzzo gelante molto forte | Nebbia più frequente al mondo | Ghiaccio e icing in inverno, nebbia in estate | Nebbia estiva (< 1 nm fino al 60–70 % del tempo nelle Curili centrali) |
| 6 Celtica | Quasi come le Faroe: vento 10 m/s, Hs 3,9 m, swell 10 s | Moderato | Mare lungo e alto, burrasche atlantiche in serie | Pioviggine e pioggia frontale |
| 7 Baltico SW | Vento 8,5–9 m/s ma Hs ≤ 1,4 m (fetch corto); ghiaccio solo negli inverni rigidi (1985, 1986, 1987) | Calmo e limpido | Mare corto, ghiaccio negli inverni rigidi, icing raro | Nebbie di marzo–aprile su mare freddo; nebbie da terra in autunno e inverno |


## 2. Schede per teatro

Le tabelle mensili vengono da ERA5 1981–1990, elaborazione nostra (fiducia: dato alta, elaborazione media).
Gli altri dati portano la propria fonte e la propria fiducia.

### 2.1 Isole Faroe – Mar di Norvegia (GIUK)

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 10,7 | 10,8 | 10,4 | 8,5 | 7,3 | 6,4 | 6,6 | 7,0 | 8,6 | 9,7 | 9,6 | 10,7 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 27,7 | 23,3 | 21,2 | 6,2 | 3,0 | 0,8 | 0,6 | 2,1 | 7,9 | 15,1 | 16,2 | 23,8 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 8,4 | 7,2 | 4,8 | 0,9 | 0,5 | 0,1 | 0,0 | 0,1 | 1,7 | 4,0 | 3,8 | 5,1 |
| Hs media (m) | 4,2 | 3,9 | 3,7 | 2,7 | 1,9 | 1,6 | 1,6 | 1,8 | 2,6 | 3,2 | 3,3 | 3,9 |
| Hs 90° percentile (m) | 6,6 | 6,1 | 5,6 | 4,2 | 3,1 | 2,5 | 2,4 | 2,8 | 4,1 | 5,1 | 5,2 | 5,9 |
| % ore Hs ≥ 4 m | 47,8 | 40,3 | 39,3 | 12,7 | 3,7 | 0,8 | 0,7 | 2,2 | 10,8 | 22,5 | 25,0 | 41,7 |
| Periodo medio d'onda (s) | 10,0 | 9,7 | 9,4 | 8,4 | 7,4 | 7,2 | 6,9 | 7,2 | 8,0 | 8,9 | 9,0 | 9,6 |
| T aria media (°C) | 5,1 | 5,3 | 5,0 | 5,7 | 7,2 | 8,8 | 10,4 | 10,6 | 9,5 | 8,3 | 6,6 | 5,8 |
| T superficie mare (proxy, °C) | 7,4 | 7,3 | 7,2 | 7,3 | 7,9 | 9,0 | 10,1 | 10,5 | 9,9 | 9,2 | 8,4 | 7,8 |
| % ore cielo coperto ≥ 6/8 | 68 | 65 | 63 | 64 | 64 | 69 | 72 | 70 | 63 | 63 | 69 | 64 |
| Nubi basse medie (%) | 50 | 51 | 47 | 55 | 58 | 61 | 67 | 60 | 56 | 52 | 57 | 50 |
| % ore con precipitazione ≥ 0,1 mm/h | 37 | 32 | 37 | 24 | 17 | 12 | 16 | 19 | 25 | 31 | 31 | 34 |
| % ore con neve | 6,9 | 3,6 | 6,2 | 3,1 | 0,1 | 0,0 | 0,0 | 0,0 | 0,0 | 0,1 | 2,5 | 5,4 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 0 | 0 | 1 | 1 | 2 | 3 | 8 | 8 | 3 | 1 | 0 | 1 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 10 | 10 | 8 | 10 | 17 | 20 | 18 | 8 |
| Primavera (MAM) | 15 | 13 | 11 | 11 | 14 | 17 | 12 | 7 |
| Estate (GLA) | 13 | 10 | 10 | 11 | 14 | 19 | 13 | 9 |
| Autunno (SON) | 13 | 9 | 9 | 11 | 15 | 20 | 14 | 9 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 0,0 | 0,8 | 16,7 | 39,3 | 31,7 | 10,4 | 1,2 |
| MAM | 0,0 | 9,5 | 41,1 | 30,8 | 15,7 | 2,8 | 0,0 |
| GLA | 0,0 | 31,5 | 57,6 | 9,8 | 1,2 | 0,0 | 0,0 |
| SON | 0,0 | 1,7 | 39,9 | 39,0 | 15,5 | 3,5 | 0,4 |

**Lettura e dati da altre fonti**
- **Vento e burrasche.** Il ciclo annuo è fortissimo: le burrasche (≥ Bft 8) occupano il 5–8 % delle ore da dicembre a febbraio e lo 0–0,1 % in estate. Fuori dall'estate il vento viene da SW–W.
  - Episodi ≥ Bft 8 in inverno: circa 15 per stagione, durata mediana 7 h, massimo 36 h (ERA5, §5).
  - Il **minimo barico** passa in media **ogni 5,1 giorni** in inverno e ogni 5,6 in estate (§6.1).
- **Mare.** Hs media invernale 4 m con periodo medio 10 s: mare dominato da onda lunga e swell atlantico. Il p99 di gennaio è 10 m; il massimo orario del decennio è 13,5 m (§5). In estate lo stato del mare 4 occupa il 58 % del tempo.
- **Nebbia.** La nebbia d'avvezione estiva domina, con aria mite su acque più fredde del fronte Islanda–Faroe.
  - La climatologia ICOADS (1950–2007) dà, a ovest del Regno Unito, un picco in GLA vicino al **4 %** delle osservazioni con nebbia. Lungo la Norvegia a N di 62° N il picco è in GLA, sotto il **3 %** (Dorman & Koračin, abstract AMS 2017, https://ams.confex.com/ams/97Annual/webprogram/Paper307556.html; fiducia media).
  - Una fonte faroese descrive Tórshavn con precipitazione circa tre giorni su quattro e "nebbia frequente" (Fróðskaparrit 1998, https://timarit.is/page/931167; fiducia media).
  - In ERA5 l'indice UR ≥ 97 % tocca il massimo in luglio–agosto (8 % delle ore).
  - Frequenza mensile < 1 km in mare aperto: **n.v.** (l'atlante US Navy del Nord Atlantico, NAVAIR 50-1C-528, rev. 1974, https://openpolar.no/Record/ftdtic:ADA021933, ha le carte ma non è stato estratto). **[GIOCO] STIMA:** visibilità < 1 km per il 5–10 % del tempo in GLA e per l'1–3 % in inverno, dove è dovuta soprattutto a pioggia, rovesci e neve.
- **Precipitazione.** Precipitazione ≥ 0,1 mm/h nel 32–37 % delle ore da dicembre a marzo. Neve nel 4–7 % delle ore invernali (rovesci di neve e graupel con aria polare marittima).
- **Ghiaccio e icing.** Nessuno in mare aperto: l'aria scende raramente sotto −2 °C e il mare resta sopra 7 °C.
- **Temporali.** In mare sono rari: nel Nord Atlantico e nelle zone artiche ampie aree hanno meno di un giorno di temporale l'anno (Enno & Sugier 2018, ATDnet; fiducia media).
  - Esistono i temporali invernali nelle irruzioni d'aria fredda su mare caldo (cumulonembi nei rovesci post-frontali). Frequenza **n.v.**; **[GIOCO] STIMA:** 1–3 giorni a stagione da ottobre a marzo, quasi zero in estate.

### 2.2 Mare di Barents, approcci a Kola

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 9,2 | 9,4 | 9,0 | 7,7 | 6,7 | 6,3 | 6,3 | 6,4 | 7,0 | 8,6 | 8,9 | 9,1 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 9,7 | 10,4 | 6,7 | 2,5 | 1,0 | 1,2 | 0,5 | 0,8 | 2,6 | 7,9 | 7,7 | 9,4 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 0,6 | 0,8 | 0,6 | 0,0 | 0,0 | 0,0 | 0,0 | 0,2 | 0,1 | 0,9 | 1,1 | 1,8 |
| Hs media (m) | 2,4 | 2,3 | 2,1 | 1,7 | 1,3 | 1,2 | 1,1 | 1,2 | 1,4 | 2,1 | 2,3 | 2,4 |
| Hs 90° percentile (m) | 3,8 | 3,8 | 3,4 | 2,7 | 2,1 | 1,9 | 1,8 | 1,9 | 2,4 | 3,4 | 3,6 | 3,8 |
| % ore Hs ≥ 4 m | 8,2 | 8,8 | 5,1 | 1,5 | 0,6 | 0,6 | 0,0 | 0,7 | 1,4 | 5,1 | 6,6 | 8,1 |
| Periodo medio d'onda (s) | 6,8 | 6,6 | 6,4 | 5,9 | 5,5 | 5,3 | 5,1 | 5,4 | 5,8 | 6,6 | 6,7 | 6,8 |
| % ore senza dato d'onda (ghiaccio) | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| T aria media (°C) | -3,1 | -2,5 | -1,4 | -0,3 | 2,1 | 4,7 | 7,7 | 8,2 | 6,8 | 3,7 | 0,2 | -1,8 |
| T superficie mare (proxy, °C) | 2,8 | 2,5 | 2,3 | 2,2 | 3,0 | 4,6 | 7,4 | 8,7 | 7,5 | 5,6 | 4,1 | 3,4 |
| % ore cielo coperto ≥ 6/8 | 80 | 79 | 78 | 74 | 68 | 66 | 67 | 67 | 65 | 70 | 72 | 76 |
| Nubi basse medie (%) | 65 | 66 | 66 | 58 | 56 | 51 | 53 | 57 | 55 | 59 | 55 | 56 |
| % ore con precipitazione ≥ 0,1 mm/h | 42 | 34 | 24 | 21 | 13 | 13 | 12 | 15 | 19 | 27 | 33 | 39 |
| % ore con neve | 40,6 | 31,2 | 22,7 | 18,3 | 6,9 | 2,9 | 0,0 | 0,0 | 0,8 | 7,3 | 26,2 | 36,2 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 0 | 0 | 0 | 1 | 1 | 4 | 9 | 4 | 3 | 1 | 0 | 0 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 12 | 10 | 9 | 9 | 14 | 22 | 13 | 11 |
| Primavera (MAM) | 12 | 9 | 12 | 14 | 12 | 15 | 14 | 13 |
| Estate (GLA) | 12 | 11 | 14 | 23 | 10 | 7 | 8 | 14 |
| Autunno (SON) | 12 | 8 | 8 | 12 | 16 | 18 | 13 | 13 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 0,0 | 12,1 | 50,2 | 29,4 | 7,5 | 0,8 | 0,0 |
| MAM | 1,1 | 37,0 | 45,1 | 14,4 | 2,3 | 0,1 | 0,0 |
| GLA | 5,0 | 64,6 | 26,9 | 3,1 | 0,4 | 0,0 | 0,0 |
| SON | 1,3 | 26,4 | 49,8 | 18,1 | 3,7 | 0,6 | 0,1 |

**Lettura e dati da altre fonti**
- **Vento.** In inverno domina il SW (22 % delle ore), sul fianco dell'anticiclone siberiano e della depressione islandese estesa. In estate prevalgono SE e NW, con vento debole.
  - ERA5 dà poche ore ≥ Bft 8 (0,6–1,8 %), ma le **polar low** sfuggono in parte alla rianalisi (§5.2).
- **Mare.** Hs invernale 2,4 m con periodo 6,8 s: mare più corto che alle Faroe. Hs ≥ 4 m per l'8–9 % del tempo in inverno.
  - Secondo una fonte secondaria, le onde > 4 m generate da polar low si verificano fino a 6 volte l'anno, quelle > 10 m fino a circa una volta l'anno (Yurovskaya et al., citato nei risultati di ricerca; fiducia bassa-media, testo originale non letto).
- **Ghiaccio.** Il punto 70,5 N 35 E non ha mai avuto ghiaccio nel 1981–1990 (0 % di ore senza onda).
  - La parte meridionale del Barents, con Murmansk, resta libera dal ghiaccio tutto l'anno grazie alla corrente del Capo Nord e alla corrente costiera di Murman (https://en.wikipedia.org/wiki/Barents_Sea; fiducia media).
  - Il bordo del ghiaccio avanza fra ottobre e dicembre, raggiunge la posizione più meridionale entro gennaio e la mantiene fino a maggio (NIPR, https://nipr.repo.nii.ac.jp/record/2000185/files/OM-O-03.pdf; fiducia media). La latitudine media in marzo per 31–39 E è **n.v.**
  - **[GIOCO]** Nel riquadro (fino a 71,55 N) il pack marino non entra. Ghiaccio solo in baie interne e fiordi (Kola, Teriberka) e verso il Gorlo del Mar Bianco (fuori riquadro).
- **Neve e precipitazione.** Neve nel 36–41 % delle ore in dicembre–gennaio, quasi sempre debole ma persistente, con visibilità ridotta.
- **Fumo di mare artico** (*sea smoke*, nebbia da vapore). Si forma quando aria molto fredda scorre su mare più caldo.
  - Soglia classica: aria almeno circa 9–10 °C più fredda dell'acqua (Saunders 1964, https://literature.wolkersdorfer.info/literature/Sea%20smoke%20and%20steam%20fog.pdf; https://en.wikipedia.org/wiki/Sea_smoke; fiducia media).
  - Nel punto ERA5 la condizione "mare − aria ≥ 9 °C e aria ≤ 0 °C" si verifica nel **15 % delle ore in gennaio**, nel 10 % in febbraio e nel 7 % in dicembre. Si tratta di potenziale, non di osservazioni.
  - Lo strato è spesso pochi metri o poche decine di metri. Il periscopio a quota di periscopio può trovarsi dentro lo strato.
- **Nebbia estiva.** Murmansk: giorni con nebbia pari al 4 % in gennaio, 5 % in giugno, **15–16 % in agosto–settembre** (https://www.climates.com/ASIA/PDF/RUS04EUR.pdf; fiducia bassa-media, definizione non dichiarata).
  - In ERA5 l'indice UR ≥ 97 % è massimo in luglio (9 % delle ore). **[GIOCO] STIMA:** in mare aperto, visibilità < 1 km per il 5–15 % del tempo da giugno a settembre.
- **Spruzzo gelante (icing).** Moderato o peggiore nel 10–11 % delle ore in gennaio–febbraio; forte nello 0,1–0,4 % (indice di Overland, §6.7).
  - In polar low e nelle irruzioni da N–NE l'icing può diventare severo (guida DTN, https://www.dtn.com/wp-content/uploads/2020/03/wp_offshore_north-norwegian-barents_1019.pdf; fiducia media).
- **Temporali.** Rari. In estate sulla costa n.v.; in inverno cumulonembi nelle polar low.

### 2.3 Stretto di Gibilterra (imbocco orientale, Alborán)

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 7,2 | 7,5 | 6,9 | 6,5 | 6,4 | 5,9 | 6,1 | 5,4 | 5,3 | 5,8 | 6,6 | 7,5 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 2,3 | 4,6 | 3,0 | 0,8 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 1,2 | 3,1 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 0,2 | 0,2 | 0,8 | 0,1 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,1 | 0,0 |
| Hs media (m) | 1,0 | 1,1 | 1,0 | 0,9 | 0,8 | 0,7 | 0,7 | 0,6 | 0,6 | 0,7 | 0,9 | 1,1 |
| Hs 90° percentile (m) | 1,8 | 2,1 | 2,0 | 1,5 | 1,3 | 1,1 | 1,1 | 1,1 | 1,0 | 1,3 | 1,6 | 2,0 |
| % ore Hs ≥ 4 m | 0,2 | 0,7 | 1,1 | 0,1 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,4 | 0,2 |
| Periodo medio d'onda (s) | 5,7 | 5,7 | 5,7 | 5,1 | 4,6 | 4,4 | 4,2 | 4,4 | 4,6 | 5,0 | 5,4 | 5,8 |
| T aria media (°C) | 14,4 | 14,3 | 15,2 | 15,7 | 17,3 | 19,8 | 21,8 | 22,5 | 21,9 | 19,7 | 17,3 | 15,6 |
| T superficie mare (proxy, °C) | 15,9 | 15,4 | 15,7 | 16,2 | 17,3 | 19,3 | 21,1 | 21,9 | 21,5 | 19,9 | 18,1 | 16,9 |
| % ore cielo coperto ≥ 6/8 | 29 | 36 | 30 | 33 | 25 | 11 | 6 | 9 | 21 | 31 | 40 | 31 |
| Nubi basse medie (%) | 19 | 23 | 16 | 13 | 10 | 6 | 5 | 8 | 10 | 15 | 20 | 21 |
| % ore con precipitazione ≥ 0,1 mm/h | 15 | 14 | 8 | 13 | 5 | 1 | 1 | 1 | 2 | 8 | 18 | 14 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 0 | 0 | 0 | 0 | 1 | 1 | 2 | 3 | 2 | 1 | 0 | 0 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 1 | 2 | 36 | 5 | 2 | 6 | 43 | 4 |
| Primavera (MAM) | 1 | 3 | 34 | 5 | 3 | 5 | 47 | 2 |
| Estate (GLA) | 1 | 5 | 39 | 5 | 3 | 3 | 44 | 1 |
| Autunno (SON) | 1 | 6 | 45 | 6 | 3 | 5 | 32 | 2 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 13,9 | 56,3 | 25,3 | 4,2 | 0,4 | 0,0 | 0,0 |
| MAM | 24,1 | 57,6 | 15,6 | 2,3 | 0,3 | 0,1 | 0,0 |
| GLA | 36,6 | 58,6 | 4,7 | 0,1 | 0,0 | 0,0 | 0,0 |
| SON | 32,0 | 54,8 | 12,5 | 0,5 | 0,1 | 0,0 | 0,0 |

**Lettura e dati da altre fonti** (atlante US Navy, Mediterraneo, Ver. 1.1 1995, testo: https://www.seekrieg.com/MediterraneanWeather.pdf; fiducia alta)
- **Regime binario.** Il vento è quasi solo da E (Levante) o da W (Poniente o Vendaval): in ERA5, E circa 36–39 % e W circa 43–44 % delle ore.
  - Secondo un abstract AEMET Levante e Poniente compaiono nello stretto in circa il 30 % dei giorni (non è chiaro se ciascuno o insieme), con cicli annuali che hanno massimo in inverno e minimo in estate (https://repositorio.aemet.es/handle/20.500.11765/16954; fiducia media).
- **Levante invernale** (novembre–aprile): meno frequente che d'estate ma più forte. Di solito 15–25 kt; a volte una fascia larga circa 2 nm arriva a burrasca e si estende fino a 60 nm a W dello stretto, a N di 36° N.
  - Onde > 13 ft (circa 4 m) nel 2 % delle osservazioni.
  - È accompagnato da nubi basse e pioggia.
- **Burrasche invernali:** 2–4 % del tempo. Onde ≥ 8 ft (2,4 m) nel 10–20 % del tempo; swell da NW > 12 ft nel 4 %.
  - Nel canale di Alborán, in inverno, mare ripido di 12 ft o più con periodo ≤ 5 s nell'1–2 % del tempo.
  - Una depressione isolata (*cut-off low*) può restare ferma **7–10 giorni**.
- **Vendaval** (SW, prima dei fronti, fine autunno e inizio primavera): raramente resta a lungo in burrasca, ma l'arrivo porta groppi, temporali e anche trombe marine.
- **Poniente** (NW dietro i fronti): tempo buono e buona visibilità. Regola pratica dell'atlante: se la pressione a Rota supera di 4 hPa quella di Malaga, ci si aspettano forti venti da W nello stretto.
- **Estate:** Levante frequente (10–20 kt). L'aria calda su acque fredde dello stretto produce **nebbia e strato basso**, più frequenti nel canale di Alborán a S di 36° N.
  - **Luglio e agosto sono i mesi più nebbiosi:** visibilità < 2 nm nel **6–8 %** delle osservazioni.
  - Un Levante prolungato porta acqua superficiale calda nello stretto e fa arretrare lentamente verso W il bordo orientale della nebbia.
  - La nube di Levante sulla Rocca è il caso tipico (https://en.wikipedia.org/wiki/Levant_(wind); fiducia media).
- **[GIOCO]** Nello stretto vero e proprio applicare un fattore d'incanalamento sul vento ERA5 di **1,3–1,6 (STIMA)** quando il vento è E o W. La riga Bft 8 ERA5 (0–0,8 %) va portata ai valori dell'atlante (2–4 % in inverno).

### 2.4 Imboccatura dell'Adriatico (Canale d'Otranto)

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 6,8 | 7,1 | 6,7 | 6,2 | 5,5 | 5,4 | 5,4 | 5,4 | 5,4 | 5,8 | 6,6 | 6,9 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 4,0 | 5,2 | 2,6 | 1,9 | 0,6 | 0,1 | 0,0 | 0,0 | 0,3 | 0,9 | 2,9 | 3,7 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 0,6 | 0,6 | 0,2 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,3 | 0,3 |
| Hs media (m) | 0,9 | 1,0 | 0,9 | 0,8 | 0,6 | 0,5 | 0,5 | 0,5 | 0,5 | 0,7 | 0,9 | 0,9 |
| Hs 90° percentile (m) | 1,7 | 1,9 | 1,7 | 1,4 | 1,1 | 1,0 | 0,9 | 0,9 | 1,0 | 1,3 | 1,7 | 1,8 |
| % ore Hs ≥ 4 m | 0,3 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,1 |
| Periodo medio d'onda (s) | 4,7 | 4,7 | 4,5 | 4,3 | 3,8 | 3,5 | 3,4 | 3,4 | 3,6 | 3,9 | 4,5 | 4,6 |
| T aria media (°C) | 11,8 | 11,5 | 12,7 | 14,8 | 17,6 | 20,9 | 23,8 | 24,3 | 22,6 | 19,6 | 15,7 | 13,1 |
| T superficie mare (proxy, °C) | 14,5 | 14,3 | 14,4 | 15,5 | 18,0 | 21,4 | 24,3 | 25,2 | 23,6 | 21,1 | 18,1 | 15,8 |
| % ore cielo coperto ≥ 6/8 | 41 | 42 | 39 | 33 | 25 | 11 | 3 | 4 | 13 | 30 | 37 | 42 |
| Nubi basse medie (%) | 24 | 26 | 16 | 9 | 5 | 4 | 2 | 4 | 7 | 17 | 23 | 25 |
| % ore con precipitazione ≥ 0,1 mm/h | 20 | 24 | 19 | 10 | 5 | 4 | 2 | 4 | 8 | 17 | 24 | 23 |
| % ore con neve | 0,0 | 0,0 | 0,4 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 0 | 0 | 0 | 0 | 1 | 2 | 1 | 2 | 0 | 1 | 0 | 0 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 30 | 12 | 5 | 10 | 22 | 7 | 4 | 10 |
| Primavera (MAM) | 26 | 8 | 3 | 11 | 31 | 4 | 3 | 13 |
| Estate (GLA) | 45 | 6 | 2 | 6 | 19 | 2 | 1 | 19 |
| Autunno (SON) | 38 | 10 | 4 | 11 | 24 | 4 | 2 | 8 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 26,1 | 47,8 | 22,9 | 3,0 | 0,2 | 0,0 | 0,0 |
| MAM | 39,2 | 45,6 | 14,4 | 0,9 | 0,0 | 0,0 | 0,0 |
| GLA | 57,6 | 38,8 | 3,6 | 0,0 | 0,0 | 0,0 | 0,0 |
| SON | 44,0 | 43,0 | 12,1 | 0,9 | 0,0 | 0,0 | 0,0 |

**Lettura e dati da altre fonti** (atlante US Navy, Mediterraneo; fiducia alta, salvo diversa indicazione)
- **Direzioni.** Asse N–S quasi esclusivo. In inverno: N 30 %, S 22 %, NE 12 %. In estate: N 45 % e NW 19 %, cioè il vento etesio secondario che esce da Otranto verso SE.
- **Bora** (novembre–marzo, NNE–NE): può estendersi per tutto l'Adriatico fino a circa 60 nm a S di Otranto. È più estesa quando la depressione si trova a S (Ionio).
  - **Durata media di una bora in burrasca: 12 h; può durare 2 giorni.**
  - Se innescata da una depressione a S (*bora scura*) porta, dall'Adriatico meridionale a Otranto, nubi basse, nebbia e pioggia. La bora anticiclonica è secca e limpida e resta più vicina alla costa dalmata.
  - Nel canale esce come "fascia stretta di vento forte" diretta verso la Libia nord-occidentale.
- **Scirocco** (SE–SW, nel settore caldo delle depressioni genovesi o nordafricane): esordio graduale.
  - Burrasche probabili in inverno e primavera, **di durata media 10–12 h, fino a 36 h**; massimi di 55 kt.
  - Più frequente nella parte meridionale della rotta. Porta nubi basse, nebbia e pioggia, peggiori a N di Pelagosa da ottobre a gennaio.
- **Stagioni.** Inverno (novembre–febbraio) peggiore; estate (giugno–settembre) calda, secca, con vento debole. Ottobre è un passaggio brusco all'inverno.
- **Burrasche nel bacino in inverno:** circa 10 % del tempo nell'Adriatico (atlante). ERA5 a Otranto dà Bft ≥ 8 per lo 0,3–0,6 % e Bft ≥ 7 per il 3,7–5,2 % delle ore: a centro canale l'Adriatico meridionale è meno ventoso.
- **Visibilità.** D'estate è "eccezionale" sul mare aperto. La foschia o la polvere riduce sotto 5 nm soprattutto da aprile a settembre, con nebbia mattutina vicino alle città.
  - D'inverno la scarsa visibilità è dovuta a pioggia e nebbia di scirocco. Valori < 2 nm nel Mediterraneo centrale: circa 1–2 % in estate. Valore specifico per Otranto **n.v.**
- **Temporali.** Sul Mediterraneo il massimo di fulmini è in **autunno** (settembre–ottobre a W, novembre–dicembre a E). Sul mare il massimo giornaliero cade di notte o al mattino, a differenza della terra (pomeriggio) (Enno & Sugier 2018, https://www.vaisala.com/sites/default/files/documents/Lightning%20flash%20density%20in%20Europe%20on%20the%20basis%20of%2010%20years%20of%20ATDnet%20data_S.-E.%20Enno%20et%20al.pdf; fiducia media).
  - Densità annua sui mari attorno all'Italia: circa 1,2–3,0 fulmini/km²/anno.
  - Coerente con ERA5: precipitazione intensa (≥ 2 mm/h) massima in ottobre–novembre (3,1–3,4 % delle ore).
  - **[GIOCO] STIMA:** giorni con temporale nel canale: 1–2 in inverno, 1–2 in primavera, 2–4 in estate (soprattutto fine agosto), 4–7 in autunno.
- **Trombe marine.** Nell'Adriatico orientale (Croazia) sono state segnalate 359 trombe marine nel 2001–2013, con massimo in **agosto** e nelle ore del **mattino (8–10)**; il 51,5 % era legato a temporali (Renko et al. 2016, https://link.springer.com/article/10.1007/s11069-016-2192-5; fiducia alta; banca dati basata su segnalazioni).
  - Le coste ionie e occidentali della Grecia sono più soggette a tornado che a trombe marine; il massimo greco di trombe marine è nell'Egeo meridionale (Sioutas, https://www.essl.org/ECSS/2009/preprints/O03-09-sioutas.pdf; fiducia media).
- **Medicane:** §5.3.

### 2.5 Passo di Kunashir – Mare di Okhotsk (Curili meridionali)

**Lato Okhotsk (punto 45,0 N 146,5 E)**

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 9,4 | 7,6 | 7,0 | 7,0 | 6,5 | 5,4 | 5,2 | 5,9 | 6,4 | 8,3 | 9,4 | 9,7 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 10,0 | 5,1 | 5,5 | 3,3 | 1,2 | 0,2 | 0,4 | 1,0 | 1,6 | 6,8 | 11,1 | 12,3 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 1,4 | 0,2 | 1,5 | 0,7 | 0,0 | 0,0 | 0,0 | 0,2 | 0,3 | 1,4 | 1,8 | 2,4 |
| Hs media (m) | 2,3 | 1,8 | 1,6 | 1,4 | 1,2 | 0,8 | 0,8 | 1,0 | 1,2 | 1,9 | 2,3 | 2,5 |
| Hs 90° percentile (m) | 3,7 | 3,4 | 2,9 | 2,3 | 1,9 | 1,3 | 1,2 | 1,5 | 2,0 | 3,2 | 3,9 | 4,2 |
| % ore Hs ≥ 4 m | 7,4 | 4,0 | 3,5 | 0,9 | 0,2 | 0,0 | 0,1 | 0,3 | 0,7 | 3,7 | 9,4 | 11,9 |
| Periodo medio d'onda (s) | 6,7 | 6,1 | 6,4 | 6,0 | 5,8 | 5,6 | 5,6 | 5,8 | 6,1 | 6,7 | 7,0 | 7,1 |
| % ore senza dato d'onda (ghiaccio) | 0 | 49 | 75 | 28 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| T aria media (°C) | -3,9 | -5,6 | -3,6 | 0,9 | 3,6 | 7,2 | 11,1 | 14,6 | 13,8 | 10,0 | 4,3 | -0,1 |
| T superficie mare (proxy, °C) | 1,1 | -3,0 | -3,3 | -0,1 | 2,5 | 6,5 | 10,5 | 13,9 | 13,6 | 10,8 | 7,0 | 3,5 |
| % ore cielo coperto ≥ 6/8 | 70 | 74 | 64 | 59 | 64 | 73 | 74 | 78 | 54 | 40 | 47 | 60 |
| Nubi basse medie (%) | 64 | 66 | 49 | 39 | 48 | 57 | 63 | 59 | 37 | 30 | 42 | 53 |
| % ore con precipitazione ≥ 0,1 mm/h | 33 | 19 | 16 | 15 | 15 | 11 | 12 | 14 | 15 | 19 | 34 | 36 |
| % ore con neve | 32,9 | 18,0 | 14,8 | 5,7 | 0,8 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 16,1 | 31,4 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 1 | 1 | 2 | 8 | 17 | 20 | 33 | 32 | 7 | 1 | 0 | 1 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 12 | 6 | 4 | 4 | 5 | 7 | 21 | 41 |
| Primavera (MAM) | 10 | 8 | 6 | 11 | 21 | 11 | 15 | 18 |
| Estate (GLA) | 7 | 10 | 9 | 21 | 33 | 7 | 5 | 7 |
| Autunno (SON) | 6 | 6 | 5 | 11 | 18 | 13 | 22 | 19 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 0,2 | 21,1 | 46,0 | 24,9 | 7,0 | 0,8 | 0,0 |
| MAM | 3,6 | 54,2 | 32,7 | 7,9 | 1,2 | 0,4 | 0,0 |
| GLA | 11,6 | 76,1 | 11,2 | 1,0 | 0,1 | 0,0 | 0,0 |
| SON | 1,0 | 34,1 | 44,8 | 15,5 | 4,1 | 0,5 | 0,0 |

**Lato Pacifico (punto 43,75 N 147,5 E), per confronto**

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 9,7 | 8,6 | 8,3 | 7,5 | 6,7 | 5,6 | 5,3 | 5,7 | 6,1 | 7,9 | 9,1 | 9,4 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 12,0 | 9,4 | 9,5 | 4,9 | 1,8 | 0,7 | 0,3 | 1,1 | 2,0 | 5,5 | 10,5 | 13,1 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 1,9 | 1,3 | 2,2 | 0,7 | 0,1 | 0,0 | 0,1 | 0,3 | 0,2 | 0,9 | 2,1 | 1,7 |
| Hs media (m) | 2,4 | 2,2 | 2,2 | 1,9 | 1,6 | 1,3 | 1,3 | 1,5 | 1,7 | 2,0 | 2,3 | 2,4 |
| Hs 90° percentile (m) | 3,9 | 3,7 | 3,9 | 3,1 | 2,6 | 2,0 | 1,7 | 2,2 | 2,6 | 3,2 | 3,7 | 3,9 |
| % ore Hs ≥ 4 m | 9,0 | 7,0 | 8,9 | 3,1 | 0,7 | 0,6 | 0,3 | 1,6 | 2,0 | 4,2 | 7,0 | 8,3 |
| Periodo medio d'onda (s) | 7,4 | 7,5 | 7,5 | 7,3 | 7,0 | 7,0 | 7,2 | 7,6 | 7,6 | 7,4 | 7,3 | 7,3 |
| T aria media (°C) | -2,3 | -3,2 | -1,1 | 2,2 | 4,2 | 6,9 | 10,3 | 13,8 | 13,3 | 10,3 | 5,4 | 1,1 |
| T superficie mare (proxy, °C) | 2,0 | 1,2 | 1,0 | 1,5 | 3,1 | 5,7 | 8,8 | 12,0 | 12,4 | 10,1 | 7,1 | 4,2 |
| % ore cielo coperto ≥ 6/8 | 57 | 68 | 62 | 59 | 64 | 76 | 79 | 79 | 62 | 42 | 46 | 52 |
| Nubi basse medie (%) | 53 | 62 | 52 | 42 | 49 | 63 | 71 | 65 | 42 | 29 | 40 | 47 |
| % ore con precipitazione ≥ 0,1 mm/h | 33 | 22 | 20 | 15 | 15 | 12 | 12 | 15 | 15 | 16 | 26 | 30 |
| % ore con neve | 32,0 | 21,1 | 16,5 | 3,7 | 0,5 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 7,8 | 22,9 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 1 | 1 | 2 | 9 | 21 | 30 | 46 | 47 | 18 | 2 | 0 | 1 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 11 | 5 | 4 | 5 | 5 | 8 | 19 | 44 |
| Primavera (MAM) | 8 | 7 | 7 | 10 | 14 | 16 | 13 | 24 |
| Estate (GLA) | 6 | 12 | 16 | 16 | 20 | 16 | 6 | 8 |
| Autunno (SON) | 7 | 7 | 8 | 10 | 14 | 17 | 15 | 22 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 0,0 | 11,5 | 50,8 | 29,6 | 7,7 | 0,4 | 0,0 |
| MAM | 0,0 | 23,5 | 56,0 | 16,2 | 3,9 | 0,3 | 0,0 |
| GLA | 0,0 | 53,8 | 41,4 | 3,9 | 0,8 | 0,1 | 0,0 |
| SON | 0,0 | 20,0 | 56,4 | 19,2 | 3,7 | 0,7 | 0,0 |

**Lettura e dati da altre fonti** (atlante US Navy, Nord Pacifico, Ver. 1.1 1995, https://www.seekrieg.com/NorthPacificWeather.pdf; fiducia alta)
- **Inverno.** Domina il monsone da NW: in ERA5 NW 41–44 % e W 19–21 % delle ore.
  - Sulle coste di Kunashir le tormente da NW sono frequenti. Vicino a Iturup forti venti da NW sollevano mare grosso da novembre ad aprile; burrasche da SE a febbraio e marzo nella baia di Kasatka.
  - A Nemuro e nello stretto di Nemuro le burrasche con pioggia o neve sono comuni da novembre a marzo; vento ≥ 19 kt circa 12 giorni al mese in inverno e 2 in estate.
- **Nebbia: la più frequente del mondo in estate.**
  - Atlante: visibilità < 1 nm fino al **60–70 %** del tempo attorno alle Curili e sulla costa E della Kamchatka. Visibilità < 5 nm fino all'80 %.
  - La frequenza cresce da fine maggio al picco di fine luglio. In settembre < 1 nm ancora fino al 35 % nelle Curili centrali.
  - Già in maggio < 1 nm ≥ 35 % nelle Curili centrali; il massimo si sposta da S verso N in primavera.
  - In inverno < 5 nm per il 35–40 % del tempo (neve e precipitazione) e < 1 nm per poco più del 5 %, nel SE dell'Okhotsk e a E delle Curili.
- **Lato Pacifico e lato Okhotsk sono molto diversi.**
  - A Iturup, giorni con nebbia (osservazioni di A. I. Butenko, tabella IKIP, https://www.burkemuseum.org/static/okhotskia/ikip/Results/publications/entobook/Img/table15.htm; fiducia media): Kasatka (Pacifico) 12/16/23/**25/25**/10/5 da aprile a ottobre; Kuril'sk (Okhotsk) 4/7/11/12/8/2/1.
  - Una fonte giapponese conferma che sul lato Pacifico la nebbia è umida e persistente (maggio–ottobre), mentre sul lato Okhotsk è più secca e breve (maggio–luglio) (sintesi da risultati di ricerca; fiducia media).
  - ICOADS: circa il 60 % delle osservazioni con nebbia nella stagione calda; nel NW Pacifico la frequenza estiva (GLA) è 59,8 % e quella annua circa 23 % (Dorman et al. 2019, https://openpolar.no/Record/crwiley:10.1002%2Fjoc.6200; Frontiers 2022, https://www.frontiersin.org/journals/earth-science/articles/10.3389/feart.2022.942846/full; fiducia alta).
  - ERA5 conferma: UR ≥ 97 % nel 46–47 % delle ore in luglio–agosto sul lato Pacifico e nel 32–33 % sul lato Okhotsk.
- **Durata degli episodi** (ERA5, indice UR ≥ 97 %, giugno–agosto): mediana 10–12 h, 90° percentile 40–55 h, massimo 155–217 h (6–9 giorni); circa 40 episodi per estate; vento medio durante gli episodi 5 m/s.
- **Nubi.** Tra le regioni più nuvolose del Pacifico. In luglio cielo coperto 9/10 a E delle Curili; settembre–ottobre sono i mesi più sereni.
- **Ghiaccio.** Sul punto lato Okhotsk le ore senza onda (ghiaccio) sono 49 % in febbraio, 75 % in marzo e 28 % in aprile (ERA5, 1981–1990).
  - Il ghiaccio alla deriva entra nel Pacifico dallo stretto di Nemuro e dal passo di Kunashir in febbraio (JMA/TCC, stagione 2006, https://ds.data.jma.go.jp/tcc/tcc/news/topics_20060526.html; fiducia alta, singola stagione). Una fonte giapponese lo indica come evento non frequente ma di grande impatto.
  - Normali 1981–2010 a Wakkanai: primo ghiaccio 13 febbraio, ultimo 12 marzo (https://www.data.jma.go.jp/wakkanai/kishou/zyouhou/wakkanai_iceinf2020-1.pdf; fiducia alta).
  - **[GIOCO]** Pack sul lato Okhotsk da inizio febbraio a metà aprile, con bordo mobile. Lato Pacifico normalmente libero; lingue di ghiaccio attraverso i passi in febbraio–marzo.
- **Spruzzo gelante** (solo ore di mare libero, ERA5 con indice di Overland):

| Punto | Dic | Gen | Feb | Mar |
|---|---|---|---|---|
| Kunashir (Okhotsk), PPR ≥ moderato | 2,0 % | 26,1 % | 32,2 % | 0,7 % |
| Kunashir, PPR ≥ forte | 0,1 % | 2,3 % | **7,2 %** | 0 |
| Kunashir, PPR estremo (≥ 83) | 0 | 0,1 % | 1,8 % | 0 |
| Curili lato Pacifico, PPR ≥ moderato | 0,5 % | 9,2 % | 16,8 % | 4,1 % |
| Barents, PPR ≥ moderato (confronto) | 2,4 % | 9,7 % | 11,2 % | 2,8 % |
| Baltico SW, PPR ≥ moderato | 0 | 1,8 % | 1,5 % | 0 |

  Nota: vicino al bordo del ghiaccio la proxy della SST è incerta; i valori di febbraio sono indicativi. Fiducia media.
  - **Kunashir in gennaio–febbraio è il teatro con l'icing peggiore del gioco.** Quando aria siberiana a −10 / −20 °C soffia su acqua vicina al congelamento, nave e sovrastrutture si coprono di ghiaccio.
- **Temporali.** Nelle Curili sono rari; possono arrivare a 3 giorni in un mese estivo o autunnale. Ad Hakodate 1–3 giorni al mese da giugno a novembre; lungo il mare di Okhotsk sono rari (atlante).
- **Cicloni.** Flusso quasi continuo di cicloni extratropicali, più intensi in autunno e inizio inverno e più numerosi in primavera (atlante).
  - Cicloni esplosivi frequenti nel NW Pacifico fra 30–60° N e 130–170° E; nel Mar del Giappone e nell'Okhotsk il picco è in **novembre** (Yoshida & Asuma 2004, via https://www.frontiersin.org/articles/10.3389/feart.2022.968736/full; fiducia alta).
  - In 7 inverni (dicembre–marzo, 1999/2000–2005/06) i casi esplosivi nel NW Pacifico sono stati 282, cioè circa 40 per inverno su tutto il bacino (https://www2.jpgu.org/meeting/2007/program/pdf/F210/F210-002_e.pdf; fiducia media).
  - Da luglio a settembre possono arrivare tifoni in transizione extratropicale, con massimo in settembre.

### 2.6 Margine della Piattaforma Celtica

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 10,1 | 10,3 | 9,2 | 7,9 | 7,5 | 6,7 | 6,1 | 6,5 | 7,4 | 8,8 | 9,2 | 9,9 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 18,6 | 19,7 | 12,7 | 3,8 | 2,4 | 1,0 | 0,7 | 1,7 | 3,4 | 10,9 | 11,3 | 18,1 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 5,2 | 5,2 | 2,3 | 0,5 | 0,3 | 0,0 | 0,0 | 0,2 | 0,4 | 1,8 | 1,9 | 3,9 |
| Hs media (m) | 4,0 | 3,9 | 3,4 | 2,5 | 2,1 | 1,7 | 1,5 | 1,8 | 2,2 | 3,1 | 3,2 | 3,8 |
| Hs 90° percentile (m) | 6,2 | 6,2 | 5,4 | 4,0 | 3,3 | 2,6 | 2,4 | 2,9 | 3,6 | 4,9 | 5,1 | 6,1 |
| % ore Hs ≥ 4 m | 45,1 | 41,8 | 31,5 | 10,0 | 4,1 | 1,3 | 0,7 | 2,1 | 6,6 | 21,9 | 22,7 | 37,7 |
| Periodo medio d'onda (s) | 10,1 | 9,8 | 9,6 | 8,4 | 7,6 | 7,2 | 7,0 | 7,3 | 8,1 | 9,0 | 9,0 | 9,9 |
| T aria media (°C) | 10,4 | 9,9 | 10,2 | 10,4 | 12,0 | 14,0 | 16,2 | 16,8 | 16,1 | 14,2 | 12,3 | 11,3 |
| T superficie mare (proxy, °C) | 11,7 | 11,4 | 11,2 | 11,5 | 12,5 | 14,2 | 16,3 | 17,3 | 16,8 | 15,1 | 13,6 | 12,4 |
| % ore cielo coperto ≥ 6/8 | 66 | 65 | 60 | 52 | 46 | 55 | 48 | 51 | 59 | 58 | 60 | 62 |
| Nubi basse medie (%) | 57 | 55 | 53 | 45 | 41 | 50 | 46 | 49 | 52 | 49 | 52 | 54 |
| % ore con precipitazione ≥ 0,1 mm/h | 26 | 26 | 22 | 16 | 15 | 12 | 8 | 12 | 14 | 24 | 22 | 25 |
| % ore con neve | 0,5 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 0 | 0 | 1 | 1 | 1 | 2 | 2 | 1 | 2 | 1 | 0 | 0 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 6 | 7 | 9 | 8 | 13 | 22 | 22 | 12 |
| Primavera (MAM) | 12 | 14 | 11 | 6 | 10 | 15 | 16 | 17 |
| Estate (GLA) | 16 | 11 | 7 | 5 | 9 | 16 | 18 | 18 |
| Autunno (SON) | 8 | 8 | 12 | 9 | 12 | 19 | 19 | 13 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 0,0 | 0,6 | 19,0 | 38,9 | 30,4 | 10,1 | 1,0 |
| MAM | 0,0 | 8,0 | 47,9 | 29,0 | 12,7 | 2,5 | 0,1 |
| GLA | 0,0 | 33,2 | 54,7 | 10,7 | 1,3 | 0,1 | 0,0 |
| SON | 0,0 | 6,0 | 42,4 | 34,5 | 14,1 | 2,7 | 0,2 |

**Lettura e dati da altre fonti** (atlante US Navy, Europa, Ver. 1.1 1995, https://www.seekrieg.com/EuropeWeather.pdf; fiducia alta)
- **Burrasche.** Da novembre a febbraio occupano il 12–14 % del tempo nella Manica occidentale e nel Mare del Nord centrale. In estate scendono sotto il 2 % (meno dell'1 % da maggio–giugno ad agosto).
  - Velocità ≥ 48 kt: 1–3 % del tempo nei mari esposti da dicembre a febbraio.
  - Vento estremo con ricorrenza di 5 anni fino a 80 kt nel Mare del Nord centrale; 115 kt con ricorrenza di 100 anni.
- **Sequenza tipica di un fronte freddo:**
  1. aumentano nuvolosità e vento;
  2. pioggia ed eventuale burrasca da SW;
  3. schiarita con forti venti da NW.
  - Le depressioni arrivano "in famiglie", che accorciano le schiarite.
  - Velocità di traslazione delle depressioni fino a 40 kt, più comunemente 5–10 kt. Una depressione stazionaria presso l'Islanda dà più giorni di SW.
- **Visibilità.** Nella Manica occidentale < 2 nm il 10 % del tempo in febbraio e circa il 6 % negli altri mesi; in estate dal 13–15 % (giugno) all'8–10 % (agosto) per la Manica nel suo insieme.
  - A W del Regno Unito la nebbia ha un picco in GLA vicino al 4 % (Dorman & Koračin, fiducia media).
  - Al ciglio celtico, in mare aperto, si aspettano valori più bassi che nella Manica (nebbia di terra assente). **[GIOCO] STIMA:** < 1 km per il 2–4 % in estate e l'1–2 % in inverno.
- **Mare.** Hs ≥ 4 m per il 38–45 % delle ore da dicembre a febbraio. Il periodo medio di 10 s indica lo swell atlantico: al ciglio della piattaforma, swell lungo e onde interne.
  - Episodi Hs ≥ 4 m in inverno: circa 18 per stagione, durata mediana 29 h, massimo 281 h (quasi 12 giorni di fila).
- **Temporali.** Rari sull'Atlantico aperto: la densità di fulmini sull'oceano è quasi trascurabile. A W dell'Iberia il massimo è in autunno (Enno & Sugier 2018). **[GIOCO] STIMA:** 1–3 giorni a stagione, legati a fronti freddi e rovesci invernali.

### 2.7 Mar Baltico sud-occidentale

| Variabile | Gen | Feb | Mar | Apr | Mag | Giu | Lug | Ago | Set | Ott | Nov | Dic |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vento medio a 10 m (m/s) | 9,2 | 7,9 | 7,5 | 6,5 | 5,9 | 5,6 | 6,4 | 6,3 | 7,5 | 8,2 | 8,6 | 8,7 |
| % ore vento ≥ Bft 7 (≥ 13,9 m/s) | 9,2 | 4,7 | 3,4 | 1,0 | 0,1 | 0,5 | 0,4 | 0,5 | 2,5 | 3,9 | 5,8 | 4,7 |
| % ore vento ≥ Bft 8 (≥ 17,2 m/s) | 1,0 | 0,5 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,5 | 0,2 | 1,0 | 0,5 |
| Hs media (m) | 1,4 | 1,2 | 1,0 | 0,7 | 0,6 | 0,6 | 0,7 | 0,7 | 1,0 | 1,1 | 1,3 | 1,3 |
| Hs 90° percentile (m) | 2,6 | 2,3 | 2,0 | 1,5 | 1,2 | 1,1 | 1,4 | 1,4 | 1,9 | 2,1 | 2,2 | 2,2 |
| % ore Hs ≥ 4 m | 0,9 | 0,4 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,4 | 0,0 | 0,8 | 0,3 |
| Periodo medio d'onda (s) | 4,6 | 4,2 | 4,1 | 3,7 | 3,4 | 3,4 | 3,5 | 3,6 | 3,9 | 4,2 | 4,4 | 4,4 |
| % ore senza dato d'onda (ghiaccio) | 3 | 11 | 13 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| T aria media (°C) | 1,6 | 0,8 | 2,2 | 4,7 | 9,0 | 13,3 | 16,4 | 16,6 | 13,9 | 10,9 | 6,5 | 3,4 |
| T superficie mare (proxy, °C) | 3,0 | 1,7 | 1,8 | 4,0 | 7,8 | 12,6 | 16,2 | 16,8 | 14,2 | 11,3 | 8,1 | 5,0 |
| % ore cielo coperto ≥ 6/8 | 72 | 69 | 66 | 51 | 40 | 45 | 39 | 43 | 49 | 54 | 60 | 70 |
| Nubi basse medie (%) | 65 | 60 | 49 | 31 | 21 | 22 | 18 | 19 | 30 | 40 | 53 | 62 |
| % ore con precipitazione ≥ 0,1 mm/h | 23 | 15 | 16 | 10 | 9 | 12 | 11 | 13 | 17 | 20 | 21 | 20 |
| % ore con neve | 10,4 | 7,2 | 4,2 | 1,1 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 0,0 | 1,6 | 6,0 |
| % ore UR ≥ 97 % (indice nebbia, debole) | 3 | 4 | 4 | 5 | 4 | 2 | 0 | 1 | 1 | 2 | 1 | 2 |

Direzione di provenienza del vento (% delle ore):

| Stagione | N | NE | E | SE | S | SW | W | NW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Inverno (DGF) | 8 | 9 | 9 | 8 | 11 | 20 | 25 | 9 |
| Primavera (MAM) | 4 | 11 | 16 | 13 | 10 | 14 | 23 | 9 |
| Estate (GLA) | 4 | 6 | 10 | 9 | 9 | 14 | 36 | 12 |
| Autunno (SON) | 6 | 5 | 6 | 11 | 12 | 20 | 27 | 13 |

Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):

| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |
|---|---:|---:|---:|---:|---:|---:|---:|
| DGF | 13,2 | 42,5 | 36,4 | 7,3 | 0,5 | 0,0 | 0,0 |
| MAM | 39,3 | 43,1 | 15,7 | 1,9 | 0,0 | 0,0 | 0,0 |
| GLA | 46,5 | 41,9 | 11,1 | 0,5 | 0,0 | 0,0 | 0,0 |
| SON | 21,1 | 43,3 | 30,7 | 4,4 | 0,4 | 0,0 | 0,0 |

**Lettura e dati da altre fonti** (atlante US Navy, Europa; fiducia alta)
- **Vento.** Il flusso da S a W prevale da settembre a gennaio–febbraio; in ERA5 W 25 % e SW 20 % delle ore in inverno, W 36 % in estate.
  - Burrasche nei porti esposti fino a 5 giorni al mese da novembre a febbraio; 2 giorni al mese o meno da maggio a luglio.
  - Burrasche estive di solito da NW. Temporali e groppi danno raffiche brevi di burrasca.
  - In primavera possono arrivare burrasche da NE (depressione sulla Polonia contro alta pressione scandinava).
- **Mare.** Fetch corto e profondità bassa: anche con 9 m/s di vento medio invernale Hs ≤ 1,4 m, con periodo 4–4,6 s. Mare **corto e ripido**; Hs ≥ 4 m quasi mai (massimo del decennio 6,1 m).
- **Ghiaccio.** Nel punto ERA5 le ore senza onda sono 11 % in febbraio e 13 % in marzo, concentrate negli inverni rigidi. Il decennio di gioco è eccezionale:
  - **1984/85, 1985/86 e 1986/87 sono stati inverni severi.** Estensione massima del ghiaccio baltico: 355 000 km² (febbraio 1985), 337 000 km² (1985/86), **405 000 km² (marzo 1987)**. Nel 1985 il ghiaccio raggiunse gli stretti danesi (https://www.eesti.ca/ice/article12829, fiducia media; HELCOM BSEP 111, https://helcom.fi/publications/bsep111/, fiducia alta; soglia "severo" > 280 000 km², https://czasopisma.bg.ug.edu.pl/index.php/oandhs/article/download/7784/6917/11620, fiducia media).
  - Negli inverni severi il ghiaccio si forma prima nell'Öresund e nei Belt, con estensione massima di solito a metà febbraio (SMHI 1987, https://www.smhi.se/en/publications-from-smhi/publications/1987-09-02-isforhallandena-i-sveriges-sodra-och-vastra-farvatten.; fiducia alta).
  - **[GIOCO]** Per una missione baltica nel febbraio 1985, 1986 o 1987: ghiaccio costiero e nei Belt, banchi alla deriva nel bacino di Arkona. Negli inverni normali solo ghiaccio costiero sottile.
- **Visibilità.**
  - Inverno: nel Baltico occidentale < 2 nm circa il 7 % del tempo; altrove oltre il 10 % in febbraio.
  - **Marzo** è il mese più insidioso, dallo Skagerrak al Baltico meridionale: < 2 nm per il 10–20 % del tempo in marzo e il 3–10 % in maggio. È nebbia d'avvezione di aria mite su acqua fredda o ghiaccio in fusione, spesso a banchi.
  - Da giugno a settembre < 2 nm meno del 5 % (meno del 2 % in agosto).
  - Autunno: 2–6 %, 4–6 % in novembre.
  - Sulle coste nebbia di radiazione notturna, che scivola sul mare e si dissolve dopo l'alba. Fra ottobre e marzo la nebbia si osserva 2–10 giorni al mese nella maggior parte delle stazioni.
  - ICOADS: mediana baltica di nebbia massima in MAM, **5,4 %** (Dorman & Koračin; fiducia media).
- **Precipitazione e neve.** Precipitazione su 8–14 giorni al mese in autunno e inverno; neve 7–14 giorni al mese da dicembre a marzo sulle coste.
- **Temporali.** In estate rovesci 8–12 giorni al mese, di cui 2–4 con temporale (coste); i fulmini sul Baltico hanno il massimo a tarda sera (Enno & Sugier 2018). **[GIOCO] STIMA** in mare aperto: 2–4 giorni con temporale per mese estivo, 0–1 in inverno.
- **Icing.** Potenziale di icing moderato da novembre ad aprile, minore nel Baltico occidentale e nello Skagerrak che nel Kattegat e nel Sound; nel Baltico settentrionale e orientale 8–10 % del tempo in gennaio–febbraio (atlante). ERA5 a Arkona: PPR ≥ moderato nell'1,5–1,8 % delle ore in gennaio–febbraio.


## 3. Meteo e mare come un unico sistema

### 3.1 Distribuzione degli stati del mare per teatro e stagione

Classi Douglas/WMO da Hs, ERA5 1981–1990 (% delle ore): **0–2** (Hs < 0,5 m), **3–4** (0,5–2,5 m), **5–6** (2,5–6 m), **7+** (≥ 6 m).

| Teatro | Inverno 0–2 / 3–4 / 5–6 / 7+ | Primavera | Estate | Autunno |
|---|---|---|---|---|
| 1 Faroe | 0 / 17 / 71 / 12 | 0 / 51 / 46 / 3 | 0 / 89 / 11 / 0 | 0 / 42 / 55 / 4 |
| 2 Barents | 0 / 62 / 37 / 1 | 1 / 82 / 17 / 0 | 5 / 92 / 3 / 0 | 1 / 76 / 22 / 1 |
| 3 Gibilterra | 14 / 82 / 5 / 0 | 24 / 73 / 3 / 0 | 37 / 63 / 0 / 0 | 32 / 67 / 1 / 0 |
| 4 Otranto | 26 / 71 / 3 / 0 | 39 / 60 / 1 / 0 | 58 / 42 / 0 / 0 | 44 / 55 / 1 / 0 |
| 5 Kunashir (Okhotsk) | 0 / 67 / 32 / 1 | 4 / 87 / 9 / 0 | 12 / 87 / 1 / 0 | 1 / 79 / 20 / 1 |
| 5b Curili (Pacifico) | 0 / 62 / 37 / 0 | 0 / 80 / 20 / 0 | 0 / 95 / 5 / 0 | 0 / 76 / 23 / 1 |
| 6 Celtica | 0 / 20 / 69 / 11 | 0 / 56 / 42 / 3 | 0 / 88 / 12 / 0 | 0 / 48 / 49 / 3 |
| 7 Baltico | 13 / 79 / 8 / 0 | 39 / 59 / 2 / 0 | 47 / 53 / 1 / 0 | 21 / 74 / 5 / 0 |

Fiducia: dato alta, elaborazione media.

Osservazioni:
- Le classi 0–2 (mare quasi calmo) esistono solo nei mari chiusi o semichiusi: Mediterraneo e Baltico.
- In Atlantico e nel Pacifico c'è sempre swell: Hs non scende quasi mai sotto 0,5 m.
- Faroe e Celtica in inverno passano il **70 %** del tempo in stato 5–6 e l'11–12 % in stato 7 o più.

### 3.2 Legame vento → onda nei dati

Hs mediana osservata in ERA5 per classe di vento (dettagli in §6.2):

| Teatro | Vento 10–15 m/s | 15–20 m/s | 20–25 m/s | Nota |
|---|---|---|---|---|
| Faroe (inverno) | 4,0 m | 5,8 m | 8,6 m | Swell sempre presente; Hs già 2,9 m con vento 5–10 m/s |
| Celtica (inverno) | 4,1 m | 6,2 m | 9,2 m | Idem |
| Barents (inverno) | 2,8 m | 4,7 m | – | Fetch e durata minori; mare più giovane |
| Otranto (inverno) | 1,7 m | 2,9 m | – | Fetch limitato (N–S: circa 150–800 km) |
| Baltico (inverno) | 1,9 m | 3,4 m | 5,4 m (pochi casi) | Fetch corto, bassi fondali |

- **Ritardo vento→onda** (correlazione massima fra vento e Hs successiva): **2–6 h** in tutti i teatri. Il mare risponde al vento con qualche ora di ritardo, poi resta alto più a lungo del vento (§6.2).
- **Swell.** Il periodo medio in inverno è circa 10 s a Faroe e Celtica, 7 s in Barents e Curili, 4,5–5,8 s in Mediterraneo e Baltico. Un periodo lungo con vento debole indica swell da tempeste lontane.
  - Lo swell si dissipa lentamente: la scala di decadimento dell'energia va da circa 3 000 km a oltre 20 000 km secondo la ripidità (Ardhuin et al. 2009, GRL, https://doi.org/10.1029/2008GL037030; fiducia media perché il numero viene dal ricordo dell'articolo, non riletto).
  - **[GIOCO]** In Atlantico (Faroe, Celtica) aggiungere una componente di swell indipendente dal vento locale: Hs 1–4 m, Tp 10–16 s, direzione W–NW, con persistenza di giorni.

### 3.3 Mare e sottomarino (richiamo)

Effetti dello stato del mare su quota periscopica, sonar di scafo e radar: già in `02_acustica_oceanografia_dottrina.md`, §8. Qui basta notare che il generatore deve produrre **Hs, Tp e direzione** coerenti con il vento, perché da essi dipendono rumore ambiente (Knudsen), clutter radar e tenuta della quota periscopica.

---

## 4. Ciclo diurno: come cambiano meteo e mare nella giornata

### 4.1 Cosa dicono i dati ERA5 in mare aperto

Medie per fascia di **ora solare locale**, estate (GLA) e inverno (DGF):

| Teatro | Stagione | Variabile | 00–06 | 06–12 | 12–18 | 18–24 |
|---|---|---|---:|---:|---:|---:|
| 1 Faroe | estate | vento medio (m/s) | 6,7 | 6,7 | 6,7 | 6,6 |
| 1 Faroe | estate | % ore con precipitazione | 19 | 17 | 13 | 13 |
| 1 Faroe | estate | nubi basse (%) | 64 | 66 | 61 | 60 |
| 1 Faroe | estate | % ore UR ≥ 97 % | 8 | 7 | 4 | 5 |
| 1 Faroe | estate | Hs media (m) | 1,66 | 1,67 | 1,66 | 1,66 |
| 1 Faroe | inverno | vento medio (m/s) | 10,7 | 10,7 | 10,8 | 10,7 |
| 1 Faroe | inverno | % ore con precipitazione | 35 | 33 | 33 | 35 |
| 1 Faroe | inverno | nubi basse (%) | 50 | 50 | 50 | 50 |
| 1 Faroe | inverno | % ore UR ≥ 97 % | 0 | 0 | 0 | 0 |
| 1 Faroe | inverno | Hs media (m) | 4,00 | 4,01 | 4,00 | 4,00 |
| 2 Barents | estate | vento medio (m/s) | 6,4 | 6,3 | 6,2 | 6,4 |
| 2 Barents | estate | % ore con precipitazione | 14 | 15 | 12 | 13 |
| 2 Barents | estate | nubi basse (%) | 55 | 57 | 51 | 50 |
| 2 Barents | estate | % ore UR ≥ 97 % | 7 | 6 | 5 | 5 |
| 2 Barents | estate | Hs media (m) | 1,14 | 1,13 | 1,12 | 1,14 |
| 2 Barents | inverno | vento medio (m/s) | 9,2 | 9,3 | 9,3 | 9,2 |
| 2 Barents | inverno | % ore con precipitazione | 38 | 40 | 38 | 38 |
| 2 Barents | inverno | nubi basse (%) | 62 | 63 | 62 | 63 |
| 2 Barents | inverno | % ore UR ≥ 97 % | 0 | 0 | 0 | 0 |
| 2 Barents | inverno | Hs media (m) | 2,36 | 2,36 | 2,37 | 2,35 |
| 3 Gibilterra | estate | vento medio (m/s) | 5,9 | 5,6 | 5,5 | 6,1 |
| 3 Gibilterra | estate | % ore con precipitazione | 1 | 1 | 1 | 1 |
| 3 Gibilterra | estate | nubi basse (%) | 8 | 11 | 3 | 3 |
| 3 Gibilterra | estate | % ore UR ≥ 97 % | 1 | 3 | 2 | 1 |
| 3 Gibilterra | estate | Hs media (m) | 0,67 | 0,66 | 0,64 | 0,68 |
| 3 Gibilterra | inverno | vento medio (m/s) | 7,3 | 7,4 | 7,5 | 7,4 |
| 3 Gibilterra | inverno | % ore con precipitazione | 14 | 16 | 12 | 14 |
| 3 Gibilterra | inverno | nubi basse (%) | 21 | 25 | 21 | 17 |
| 3 Gibilterra | inverno | % ore UR ≥ 97 % | 0 | 0 | 0 | 0 |
| 3 Gibilterra | inverno | Hs media (m) | 1,05 | 1,08 | 1,11 | 1,08 |
| 4 Otranto | estate | vento medio (m/s) | 5,0 | 5,1 | 6,0 | 5,5 |
| 4 Otranto | estate | % ore con precipitazione | 4 | 4 | 2 | 2 |
| 4 Otranto | estate | nubi basse (%) | 4 | 6 | 2 | 2 |
| 4 Otranto | estate | % ore UR ≥ 97 % | 0 | 1 | 3 | 1 |
| 4 Otranto | estate | Hs media (m) | 0,48 | 0,47 | 0,56 | 0,55 |
| 4 Otranto | inverno | vento medio (m/s) | 6,8 | 6,8 | 7,1 | 7,0 |
| 4 Otranto | inverno | % ore con precipitazione | 23 | 23 | 21 | 21 |
| 4 Otranto | inverno | nubi basse (%) | 25 | 26 | 25 | 23 |
| 4 Otranto | inverno | % ore UR ≥ 97 % | 0 | 0 | 0 | 0 |
| 4 Otranto | inverno | Hs media (m) | 0,94 | 0,93 | 0,98 | 0,98 |
| 5 Kunashir | estate | vento medio (m/s) | 5,7 | 5,4 | 5,4 | 5,5 |
| 5 Kunashir | estate | % ore con precipitazione | 14 | 14 | 11 | 10 |
| 5 Kunashir | estate | nubi basse (%) | 66 | 66 | 52 | 55 |
| 5 Kunashir | estate | % ore UR ≥ 97 % | 35 | 31 | 22 | 27 |
| 5 Kunashir | estate | Hs media (m) | 0,87 | 0,86 | 0,85 | 0,86 |
| 5 Kunashir | inverno | vento medio (m/s) | 9,0 | 8,8 | 9,2 | 8,8 |
| 5 Kunashir | inverno | % ore con precipitazione | 31 | 28 | 30 | 31 |
| 5 Kunashir | inverno | nubi basse (%) | 61 | 60 | 61 | 60 |
| 5 Kunashir | inverno | % ore UR ≥ 97 % | 1 | 1 | 1 | 0 |
| 5 Kunashir | inverno | Hs media (m) | 2,27 | 2,26 | 2,31 | 2,28 |
| 5b Curili Pac. | estate | vento medio (m/s) | 5,6 | 5,5 | 5,6 | 5,5 |
| 5b Curili Pac. | estate | % ore con precipitazione | 14 | 14 | 11 | 13 |
| 5b Curili Pac. | estate | nubi basse (%) | 71 | 70 | 61 | 63 |
| 5b Curili Pac. | estate | % ore UR ≥ 97 % | 47 | 44 | 34 | 39 |
| 5b Curili Pac. | estate | Hs media (m) | 1,37 | 1,37 | 1,37 | 1,37 |
| 5b Curili Pac. | inverno | vento medio (m/s) | 9,3 | 9,2 | 9,5 | 9,1 |
| 5b Curili Pac. | inverno | % ore con precipitazione | 31 | 28 | 28 | 29 |
| 5b Curili Pac. | inverno | nubi basse (%) | 55 | 54 | 54 | 53 |
| 5b Curili Pac. | inverno | % ore UR ≥ 97 % | 1 | 1 | 1 | 1 |
| 5b Curili Pac. | inverno | Hs media (m) | 2,34 | 2,33 | 2,38 | 2,36 |
| 6 Celtica | estate | vento medio (m/s) | 6,4 | 6,5 | 6,4 | 6,3 |
| 6 Celtica | estate | % ore con precipitazione | 13 | 11 | 8 | 10 |
| 6 Celtica | estate | nubi basse (%) | 50 | 52 | 46 | 45 |
| 6 Celtica | estate | % ore UR ≥ 97 % | 3 | 2 | 0 | 1 |
| 6 Celtica | estate | Hs media (m) | 1,66 | 1,67 | 1,67 | 1,66 |
| 6 Celtica | inverno | vento medio (m/s) | 10,1 | 10,2 | 10,2 | 10,0 |
| 6 Celtica | inverno | % ore con precipitazione | 26 | 27 | 25 | 24 |
| 6 Celtica | inverno | nubi basse (%) | 55 | 55 | 56 | 55 |
| 6 Celtica | inverno | % ore UR ≥ 97 % | 0 | 0 | 0 | 0 |
| 6 Celtica | inverno | Hs media (m) | 3,91 | 3,92 | 3,93 | 3,92 |
| 7 Baltico | estate | vento medio (m/s) | 6,1 | 6,2 | 6,2 | 6,1 |
| 7 Baltico | estate | % ore con precipitazione | 15 | 14 | 9 | 11 |
| 7 Baltico | estate | nubi basse (%) | 21 | 26 | 17 | 15 |
| 7 Baltico | estate | % ore UR ≥ 97 % | 2 | 1 | 1 | 1 |
| 7 Baltico | estate | Hs media (m) | 0,65 | 0,68 | 0,68 | 0,65 |
| 7 Baltico | inverno | vento medio (m/s) | 8,5 | 8,7 | 8,6 | 8,7 |
| 7 Baltico | inverno | % ore con precipitazione | 21 | 19 | 18 | 20 |
| 7 Baltico | inverno | nubi basse (%) | 61 | 65 | 62 | 60 |
| 7 Baltico | inverno | % ore UR ≥ 97 % | 3 | 2 | 3 | 2 |
| 7 Baltico | inverno | Hs media (m) | 1,28 | 1,30 | 1,31 | 1,31 |

Fiducia: dato alta, elaborazione media.

**Lettura:**
- In **mare aperto il ciclo diurno è debole.** Ad alte latitudini e in inverno è nullo: vento, Hs e precipitazione non cambiano con l'ora alle Faroe, nel Barents, al ciglio celtico e alle Curili.
- **Otranto, estate:** il vento rinforza di circa 1 m/s nel pomeriggio (12–18) e Hs cresce da 0,48 a 0,56 m. È la brezza termica e il rinforzo pomeridiano dei venti da N sulle coste pugliesi e albanesi.
- **Gibilterra, estate:** il vento è massimo la sera e la notte (6,1 m/s) e minimo nel pomeriggio (5,5 m/s). Le nubi basse sono massime al mattino (11 %) e minime nel pomeriggio (3 %).
- **Curili ed Okhotsk, estate:** saturazione e nubi basse massime di notte e al mattino (lato Pacifico: UR ≥ 97 % nel 47 % delle ore fra 00 e 06 e nel 34 % fra 12 e 18). La nebbia d'avvezione non sparisce di giorno ma si alza o si dirada parzialmente nel pomeriggio.
- **Precipitazione estiva:** in mare è più frequente di notte e al mattino che nel pomeriggio, in tutti i teatri (Faroe 19 % contro 13 %; Celtica 13 % contro 8 %; Baltico 15 % contro 9 %). È coerente con il massimo notturno o mattutino dei fulmini sul mare.

### 4.2 Fenomeni costieri da fonti

| Fenomeno | Dove e quando | Ciclo | Fonte | Fiducia |
|---|---|---|---|---|
| Brezza di mare e di terra | Baltico, coste meridionali, da marzo all'estate | Vento costiero medio 4–8 kt al mattino, 10–12 kt nel pomeriggio; brezza di terra notturna da NW–N nel Baltico occidentale | Atlante US Navy, Europa (https://www.seekrieg.com/EuropeWeather.pdf) | alta |
| Brezza sulla Manica | Manica, estate con gradiente debole | Venti da N frequenti al mattino presto, quasi assenti nel pomeriggio; brezza pomeridiana da E–S | idem | alta |
| Brezza e bora | Adriatico, entrambe le coste | "La brezza di mare rinforza o contrasta la bora": variazione diurna sulla costa | Atlante US Navy, Mediterraneo | alta |
| Brezze nel NW Pacifico | S dell'area (Hokkaido), estate | Brezze di terra e di mare avvertibili fino a 15 nm al largo | Atlante US Navy, Nord Pacifico | alta |
| Nebbia di radiazione | Coste baltiche (estate: porti continentali; autunno e inverno: diffusa) | Si forma con cielo sereno notturno, scivola sul mare e si dissolve poco dopo l'alba | Atlante US Navy, Europa | alta |
| Nebbia mattutina vicino alle città | Mediterraneo, estate | Solo all'alba, vicino a grandi città costiere | Atlante US Navy, Mediterraneo | alta |
| Fulmini: terra contro mare | Europa | Terra: massimo nel pomeriggio, minimo di notte. Mare: massimo di notte o al mattino; Baltico: tarda sera | Enno & Sugier 2018 | media |
| Trombe marine | Adriatico orientale | Massimo in agosto, ore 8–10 | Renko et al. 2016 | alta |
| Convezione pomeridiana estiva | Coste mediterranee e baltiche | Rovesci e temporali pomeridiani sulla costa, che in mare arrivano la sera o la notte | Atlante (Baltico: "afternoon shower or thunderstorm" in estate); Enno & Sugier | alta e media |

### 4.3 [GIOCO] Regole del ciclo diurno

- **Fattore costa** `c` = 1 entro 5 nm dalla costa, decresce linearmente a 0 a 25 nm (STIMA, coerente con le 15 nm dell'atlante per il NW Pacifico).
- **Vento:** `U(t) = U_sinottico + c · A_brezza · cos(2π (h − 15)/24)`, con `A_brezza` = 2–3 m/s in estate in Mediterraneo e Baltico, 0,5–1 m/s nelle mezze stagioni, 0 in inverno sopra 60° N (STIMA).
  - In mare aperto usare l'ampiezza ERA5: 0,5 m/s a Otranto e Gibilterra in estate, circa 0 altrove.
- **Nebbia di radiazione:** solo con `c > 0`, cielo ≤ 2/8, vento < 3 m/s e notte lunga. Si forma dopo mezzanotte e si dissolve fra 1 e 3 h dopo l'alba (STIMA).
- **Nebbia d'avvezione:** nessun obbligo diurno. Densità +20 % di notte e −20 % nel primo pomeriggio (STIMA dai dati ERA5 delle Curili, §4.1).
- **Temporali estivi:** in mare probabilità massima fra 00 e 09 locali. Entro 10 nm da coste montuose massimo fra 14 e 20.

---

## 5. Eventi estremi e loro rarità

### 5.1 Massimi del decennio e periodi di ritorno (ERA5, punto per punto)

Massimi annui 1981–1990 con adattamento di Gumbel (10 valori: le ricorrenze a 50 anni sono indicative). Vento: medio orario a 10 m. Raffica: parametro di raffica ERA5.

| Teatro | Vento max 10 a (m/s) | Vento 10 a / 50 a | Raffica max (m/s) | Raffica 10 a / 50 a | Hs max (m) | Hs 10 a / 50 a | % ore raffica ≥ 24,5 m/s DGF | % ore raffica ≥ 32,7 m/s DGF |
|---|---|---|---|---|---|---|---|---|
| 1 Faroe | 31,0 | 28,5 / 32,5 | 48,9 | 44,5 / 52,1 | 13,5 | 13,4 / 16,1 | 7,1 | 0,57 |
| 2 Barents | 23,2 | 23,1 / 25,2 | 33,9 | 34,2 / 37,6 | 10,2 | 10,0 / 12,0 | 1,3 | 0,005 |
| 3 Gibilterra (imbocco E) | 21,4 | 20,5 / 22,5 | 29,9 | 29,5 / 32,8 | 7,4 | 6,1 / 7,5 | 0,18 | 0 |
| 4 Otranto | 20,8 | 20,6 / 22,4 | 29,6 | 29,5 / 32,3 | 4,8 | 4,7 / 5,5 | 0,44 | 0 |
| 5 Kunashir (Okhotsk) | 24,9 | 23,9 / 25,9 | 37,4 | 36,3 / 39,9 | 10,2 | 9,3 / 10,5 | 1,9 | 0,02 |
| 5b Curili (Pacifico) | 23,8 | 23,6 / 25,3 | 36,1 | 35,7 / 38,2 | 9,2 | 8,8 / 10,1 | 2,3 | 0,05 |
| 6 Celtica | 25,0 | 24,8 / 26,6 | 38,8 | 37,6 / 41,2 | 12,1 | 12,4 / 14,0 | 4,4 | 0,22 |
| 7 Baltico SW | 23,2 | 22,7 / 25,4 | 37,5 | 36,2 / 41,8 | 6,1 | 5,9 / 6,7 | 0,9 | 0,05 |

Fiducia media: periodo breve e sottostima ERA5 dei picchi.

**Lettura:**
- Il vento medio di **forza 10 o più (≥ 24,5 m/s)** si raggiunge in ERA5 solo alle Faroe (circa 0,2 % delle ore in gennaio) e quasi mai altrove.
- Le osservazioni da nave e da stazione danno valori più alti. Esempio: Mare del Nord centrale, 80 kt (41 m/s) con ricorrenza di 5 anni (atlante US Navy, Europa).
- **[GIOCO]** Per fissare il tetto degli eventi rari usare i valori di raffica ERA5 come "vento medio massimo plausibile" in un evento eccezionale.

Tempeste storiche del periodo, utilizzabili come scenari:
- la **Great Storm** dell'ottobre 1987 (Manica e Inghilterra meridionale; Met Office, https://www.metoffice.gov.uk/about-us/who/our-history/lessons-and-legacy-of-the-great-storm-of-1987; fiducia alta);
- la tempesta del 25 gennaio 1990 (*Burns' Day storm*; data e nome da memoria, fiducia media, da verificare).

### 5.2 Polar low (Mar di Norvegia e Barents: teatri 1 e 2)

| Caratteristica | Valore | Fonte | Fiducia |
|---|---|---|---|
| Numero per stagione (settembre–aprile), Nordici + Barents, da microonde passive | **45,5** (min 35, max 62); 637 casi in 14 stagioni 1995–2009 | Smirnova et al. 2015, GRL, https://archimer.ifremer.fr/doc/00275/38639/37161.pdf | alta |
| Ripartizione | Mar di Norvegia 42,5 %, **Barents 41 %**, Groenlandia 16,5 %; area di genesi principale a NE di Capo Nord, cioè l'approccio a Kola | idem | alta |
| Mese di massimo | **marzo** (10,1 casi in media), poi gennaio (6,7); il minimo di febbraio non è significativo | idem; Noer et al. 2011, https://openpolar.no/Record/crwiley:10.1002%2Fqj.846 | alta |
| Da maggio ad agosto | "estremamente rari" | Smirnova 2015 | alta |
| Diametro | media **298 km** (dev. std 110); metà dei casi fra 200 e 400 km; nessuno oltre 800 km | Smirnova 2015 | alta |
| Durata | media **14,7 h** (dev. std 7,7); la maggioranza 9–18 h; solo il 10 % oltre 1 giorno | idem | alta |
| Traslazione | media 8,1 m/s (4–10 m/s); distanza media 284 km, per lo più 100–300 km (quasi stazionarie) | idem; Rojo et al. 2015: 5–13 m/s, https://hal.archives-ouvertes.fr/hal-01872789 | alta |
| Vento al suolo | media del massimo **19,6 m/s** (dev. std 3,6); massimo 33,4 m/s; soglia di definizione 15 m/s; con i rovesci convettivi fino a Bft 10 | Smirnova 2015; DTN | alta e media |
| Altre stime di frequenza | 12 eventi per anno (IR 2000–2009, Noer 2011); 14 per inverno (AVHRR 1999–2013, Rojo 2015); circa 6,5 per anno nelle climatologie da carte meteorologiche più vecchie | — | alta |
| Prevedibilità | "raramente individuate sulle carte meteorologiche" per dimensioni e durata ridotte: nel 1989 di fatto non previste | Smirnova 2015 (intro) | alta |
| Tempo associato | rovesci intensi di neve, graupel e grandine; spruzzo gelante severo; mare che cresce in poche ore | DTN | media |

**[GIOCO]**
- Processo di Poisson nei teatri 1 e 2, da ottobre ad aprile.
- Tasso nel riquadro di gioco (circa 300 × 300 km): circa 45 casi l'anno su un'area di circa 2–3 milioni di km², cioè **≈ 0,5–1 polar low al mese nel riquadro del Barents** in gennaio–marzo e ≈ 0,3 al mese in ottobre–dicembre (STIMA da Smirnova, ripartita per area). Ai Faroe metà di questi valori (STIMA: la zona è al bordo S dell'area di genesi).
- Parametri d'evento: durata lognormale (mediana 13 h, 90° percentile 25 h); raggio 150 km; vento massimo normale (media 20, dev. std 4 m/s); traslazione 8 m/s verso S–SE.
- Esordio in 1–3 h (STIMA). Visibilità < 1 km nei rovesci di neve.

### 5.3 Medicane e cicloni mediterranei (teatri 3 e 4)

- **Frequenza.** Da 0,5 casi l'anno (criteri stretti da satellite) a circa **1,5–1,6 l'anno** (ibridi inclusi) in tutto il Mediterraneo (Romero & Emanuel 2013, JGR, https://texmex.mit.edu/pub/emanuel/PAPERS/Romero_Emanuel_2013.pdf, che cita Cavicchia 2013 e Tous & Romero 2012; fiducia alta).
- **Stagione.** In autunno l'incidenza è doppia rispetto all'inverno; d'estate quasi assenti. La genesi decresce da W a E.
- **Rischio di vento.** Il minimo periodo di ritorno di venti ≥ 34 kt da medicane si trova attorno alla Sicilia e a S della penisola italiana (Ionio). Venti ≥ 60 kt hanno periodi di ritorno di **alcuni secoli** nella maggior parte del bacino e **meno di un secolo** nell'Italia meridionale, in Sicilia e nei mari vicini (idem).
- **[GIOCO]** A Otranto un medicane nel riquadro ha una probabilità di circa 1–3 % per mese da settembre a gennaio (STIMA dalle mappe di ritorno ≥ 34 kt, non lette in dettaglio). Evento di 1–2 giorni con vento 20–30 m/s, pioggia molto intensa e mare 4–6 m.
- **Altre depressioni.**
  - Le *cut-off low* a Gibilterra possono restare 7–10 giorni.
  - Il gregale da NE (con depressione nel Mediterraneo centrale) dura a lungo a 20–30 kt e solleva mare oltre 12 ft (atlante US Navy, Mediterraneo; fiducia alta).

### 5.4 Bora e scirocco nel canale d'Otranto (teatro 4)

Riassunto dall'atlante US Navy, Mediterraneo (fiducia alta):

| Vento | Stagione | Esordio | Durata della burrasca | Massimi | Tempo associato |
|---|---|---|---|---|---|
| Bora (NNE–NE) | novembre–marzo | improvviso, spesso con groppo al passaggio del fronte freddo | media 12 h, fino a 2 giorni | a Trieste 70 kt orari, raffiche 110 kt; nel canale molto meno | bora anticiclonica limpida; bora scura con nubi basse, nebbia e pioggia |
| Scirocco (SE–SW) | autunno–primavera, burrasche soprattutto in inverno e primavera | graduale, in 24–48 h | media 10–12 h, fino a 36 h | 55 kt | nubi basse, nebbia, pioggia; mare lungo da SE |

- **[GIOCO]** A Otranto la bora è un'"ondata" di vento: crescita in 1–2 h, picco, decadimento esponenziale in 6–12 h. Lo scirocco cresce in rampa di 24–48 h.

### 5.5 Trombe marine e groppi

- Adriatico orientale: 359 eventi segnalati nel 2001–2013, cioè circa **28 l'anno lungo la costa croata**, quasi tutti in estate e autunno, con massimo in agosto e fra le 8 e le 10 (Renko et al. 2016; fiducia alta, sottostima per mancanza di osservazioni).
- Gibilterra e Alborán: trombe marine all'esordio del Vendaval, fine autunno e inizio primavera (atlante; frequenza n.v.).
- Baltico: n.v.
- **[GIOCO]** Probabilità di avvistare una tromba marina nel riquadro di Otranto, per giorno con temporale estivo o autunnale: 5–10 % (STIMA). In tutti gli altri teatri ≤ 1 % (STIMA).

### 5.6 Tempeste invernali dell'Okhotsk e delle Curili (teatro 5)

- ERA5 (Kunashir lato Okhotsk, inverno): circa 14 episodi l'anno ≥ Bft 7 e 3,3 ≥ Bft 8, durata mediana 6–9 h, massimo 28 h.
  - Raffiche ≥ 24,5 m/s nell'1,9–2,3 % delle ore invernali.
  - Hs massima del decennio 10,2 m.
- Ottobre porta un "aumento radicale" di vento e burrasche. Autunno e inizio inverno sono molto ventosi anche nell'Okhotsk; a SE della Kamchatka le burrasche superano il 20 % del tempo in inverno (atlante; fiducia alta).
- Cicloni esplosivi: §2.5. **[GIOCO]** 1–2 "bombe" al mese nel quadrante NW Pacifico da novembre a marzo, di cui 0,3–0,5 al mese con effetti nel riquadro (STIMA). Nella stessa sequenza: burrasca da SE, poi rotazione a NW con tormenta e spruzzo gelante.

### 5.7 Spruzzo gelante: classi e frequenze

- Formula e classi in §6.7. Frequenze per teatro nella tabella del §2.5.
- Categorie empiriche dell'atlante US Navy (fiducia alta):
  - icing **moderato** con aria < −2 °C e vento ≥ 13 kt;
  - **severo** con aria < −9 °C e vento ≥ 30 kt (oltre 4 t/h su un peschereccio di 300–500 t).
- Le situazioni pericolose sono burrasche di più giorni con aria ≤ 28 °F (−2 °C), di solito con vento dai quadranti settentrionali.

### 5.8 Temporali e fulmini: quadro per il gioco

| Teatro | DGF | MAM | GLA | SON | Base |
|---|---|---|---|---|---|
| 1 Faroe | 1–3 | 0–1 | 0–1 | 1–2 | Enno & Sugier: Nord Atlantico < 1 giorno di temporale l'anno su ampie aree; temporali invernali da aria fredda su mare caldo (STIMA) |
| 2 Barents | 0–1 (polar low con CB) | 0 | 0–1 | 0–1 | STIMA |
| 3 Gibilterra | 1–3 | 1–3 | 0–1 | 2–4 | Vendaval con temporali; massimo autunnale a W dell'Iberia (Enno & Sugier) |
| 4 Otranto | 1–2 | 1–2 | 2–4 | 4–7 | Massimo autunnale mediterraneo; 1,2–3 fulmini/km²/anno attorno all'Italia (Enno & Sugier) |
| 5 Kunashir | 0 | 0 | 0–2 | 0–3 | Atlante: nelle Curili rari, al massimo 3 giorni in un mese estivo o autunnale |
| 6 Celtica | 1–3 | 0–2 | 0–2 | 1–3 | STIMA |
| 7 Baltico | 0–1 | 1–2 | 6–12 | 1–3 | Atlante: in estate 2–4 giorni di temporale al mese sulle coste |

Valori in giorni con temporale per stagione, sul riquadro. Tutti STIMA ancorate alle fonti indicate (fiducia bassa-media).

### 5.9 Tabella degli eventi rari per il generatore

| Evento | Teatri | Stagione | Tasso nel riquadro (STIMA salvo dove citato) | Durata | Firma |
|---|---|---|---|---|---|
| Burrasca ≥ Bft 8 | 1, 6 | DGF | 11–15 episodi per inverno (ERA5) × 1,5 (correzione) | 5–7 h mediana, fino a 36–39 h | fronte freddo, rotazione SW→NW |
| Burrasca ≥ Bft 8 | 2, 5 | DGF | 2–4 per inverno (ERA5) × 2 | 6–7 h | 2: SW e polar low; 5: NW monsonico |
| Burrasca ≥ Bft 8 | 3, 4, 7 | DGF | 0,5–2 per inverno (ERA5) × 2–3 | 4–6 h (Otranto: bora 12 h, scirocco 10–12 h secondo l'atlante) | Levante, bora o scirocco, SW baltico |
| Tempesta ≥ Bft 10 | 1, 6 | DGF | ≈ 0,3 episodi per inverno alle Faroe (ERA5); 1 ogni 2–3 inverni al ciglio celtico (STIMA) | 4–7 h | ciclone profondo |
| Polar low | 2 (1) | ottobre–aprile, massimo marzo | 0,5–1 al mese (Barents), metà alle Faroe | 15 h (9–18) | rovesci di neve, vento 20 m/s, esordio rapido |
| Medicane | 4 (3) | settembre–gennaio | 1–3 % al mese | 1–2 giorni | vento ≥ 34 kt, pioggia, mare 4–6 m |
| Nebbia persistente > 3 giorni | 5 | giugno–agosto | 1–2 per estate (ERA5: massimi di 6–9 giorni) | 3–9 giorni | aria da S su Oyashio |
| Fumo di mare artico denso | 2 (5) | dicembre–febbraio | 7–15 % delle ore con potenziale (ERA5) | ore, a volte giorni | aria ≤ −10 °C, vento debole o moderato da terra o dal ghiaccio |
| Icing forte (PPR ≥ 53) | 5 (2) | gennaio–febbraio | 2–7 % delle ore a Kunashir, 0,1–0,4 % nel Barents | — | burrasca da NW, aria ≤ −10 °C |
| Ghiaccio nel Baltico SW | 7 | febbraio–marzo | inverni 1985, 1986 e 1987 sì; altri anni raro | settimane | banchi e ghiaccio costiero |
| Tromba marina | 4 (3) | agosto–ottobre | 5–10 % per giorno con temporale | minuti | sotto temporale o linea di groppi |

---

## 6. Scale temporali e statistiche per il modello

### 6.1 Cicli sinottici, burrasche e mare grosso (ERA5)

"Minimi barici": minimi della pressione al livello del mare mediata su 24 h, almeno 3 hPa sotto la media su 5 giorni e separati da almeno 3 giorni. L'intervallo è la durata della stagione divisa per il numero di minimi. Un episodio è una serie continua di ore sopra soglia, lunga almeno 3 h.

| Teatro | Stagione | Episodi ≥ Bft 7 /stagione | durata mediana (h) | p90 (h) | max (h) | Episodi ≥ Bft 8 /stagione | durata mediana (h) | max (h) | Episodi Hs ≥ 4 m /stagione | durata mediana (h) | p90 (h) | Minimi barici /stagione | intervallo medio (giorni) |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 Faroe | inverno | 36,0 | 11 | 30 | 89 | 14,7 | 7 | 36 | 21,5 | 25 | 104 | 17,7 | 5,1 |
| 1 Faroe | primavera | 16,5 | 10 | 28 | 59 | 5,4 | 6 | 25 | 13,7 | 21 | 68 | 16,1 | 5,6 |
| 1 Faroe | estate | 2,6 | 6 | 18 | 23 | 0,1 | 5 | 5 | 1,7 | 14 | 29 | 16,1 | 5,6 |
| 1 Faroe | autunno | 19,5 | 10 | 30 | 76 | 7,5 | 6 | 29 | 14,5 | 24 | 60 | 17,8 | 5,1 |
| 2 Barents | inverno | 17,1 | 9 | 26 | 68 | 2,4 | 6 | 24 | 9,4 | 15 | 39 | 16,7 | 5,4 |
| 2 Barents | primavera | 6,6 | 8 | 22 | 57 | 0,4 | 6 | 24 | 3,1 | 14 | 29 | 17,1 | 5,3 |
| 2 Barents | estate | 2,0 | 8 | 14 | 28 | 0,1 | 12 | 12 | 0,8 | 12 | 24 | 13,6 | 6,6 |
| 2 Barents | autunno | 10,8 | 8 | 23 | 51 | 1,3 | 8 | 33 | 5,9 | 13 | 32 | 16,9 | 5,3 |
| 3 Gibilterra | inverno | 4,5 | 8 | 37 | 57 | 0,5 | 4 | 6 | 0,5 | 15 | 17 | 10,3 | 8,8 |
| 3 Gibilterra | primavera | 1,7 | 9 | 33 | 67 | 0,5 | 8 | 39 | 0,3 | 18 | 52 | 7,1 | 12,7 |
| 3 Gibilterra | estate | 0,0 | – | – | – | 0,0 | – | – | 0,0 | – | – | 2,8 | 32,2 |
| 3 Gibilterra | autunno | 0,7 | 8 | 25 | 31 | 0,2 | 4 | 5 | 0,2 | 15 | 20 | 6,3 | 14,3 |
| 4 Otranto | inverno | 7,2 | 8 | 27 | 54 | 1,2 | 6 | 16 | 0,5 | 7 | 9 | 13,8 | 6,5 |
| 4 Otranto | primavera | 3,4 | 7 | 18 | 37 | 0,3 | 4 | 4 | 0,0 | – | – | 10,6 | 8,5 |
| 4 Otranto | estate | 0,1 | 5 | 5 | 5 | 0,0 | – | – | 0,0 | – | – | 5,7 | 15,8 |
| 4 Otranto | autunno | 2,3 | 10 | 24 | 34 | 0,3 | 5 | 12 | 0,0 | – | – | 11,4 | 7,9 |
| 5 Kunashir (Okhotsk) | inverno | 14,3 | 9 | 28 | 63 | 3,3 | 6 | 28 | 7,9 | 16 | 36 | 16,5 | 5,5 |
| 5 Kunashir (Okhotsk) | primavera | 6,3 | 8 | 21 | 41 | 1,5 | 6 | 27 | 1,0 | 8 | 36 | 17,3 | 5,2 |
| 5 Kunashir (Okhotsk) | estate | 1,0 | 12 | 23 | 26 | 0,3 | 6 | 7 | 0,4 | 8 | 9 | 10,9 | 8,3 |
| 5 Kunashir (Okhotsk) | autunno | 12,3 | 7 | 25 | 53 | 2,9 | 6 | 22 | 5,8 | 13 | 35 | 18,1 | 5,0 |
| 5b Curili (Pacifico) | inverno | 19,0 | 8 | 27 | 63 | 3,4 | 7 | 27 | 10,8 | 12 | 34 | 17,3 | 5,2 |
| 5b Curili (Pacifico) | primavera | 10,0 | 8 | 24 | 41 | 1,8 | 8 | 26 | 5,3 | 17 | 33 | 17,5 | 5,2 |
| 5b Curili (Pacifico) | estate | 1,4 | 10 | 20 | 25 | 0,4 | 4 | 10 | 1,1 | 18 | 23 | 11,6 | 7,8 |
| 5b Curili (Pacifico) | autunno | 11,8 | 8 | 24 | 41 | 2,5 | 7 | 22 | 5,2 | 15 | 38 | 18,6 | 4,9 |
| 6 Celtica | inverno | 26,6 | 11 | 33 | 58 | 10,7 | 7 | 39 | 17,7 | 29 | 118 | 15,3 | 5,9 |
| 6 Celtica | primavera | 11,5 | 9 | 25 | 38 | 3,1 | 6 | 21 | 9,6 | 25 | 76 | 15,1 | 6,0 |
| 6 Celtica | estate | 2,4 | 6 | 20 | 24 | 0,2 | 6 | 6 | 2,0 | 10 | 31 | 12,3 | 7,3 |
| 6 Celtica | autunno | 13,9 | 9 | 24 | 65 | 3,1 | 5 | 26 | 12,1 | 23 | 66 | 16,0 | 5,6 |
| 7 Baltico | inverno | 11,0 | 8 | 24 | 57 | 1,7 | 5 | 18 | 1,5 | 4 | 16 | 15,8 | 5,7 |
| 7 Baltico | primavera | 3,3 | 6 | 19 | 35 | 0,0 | – | – | 0,0 | – | – | 15,2 | 5,9 |
| 7 Baltico | estate | 1,0 | 8 | 12 | 18 | 0,0 | – | – | 0,0 | – | – | 13,2 | 6,8 |
| 7 Baltico | autunno | 8,2 | 6 | 22 | 56 | 1,0 | 8 | 32 | 0,7 | 9 | 21 | 17,1 | 5,3 |

Fiducia: dato alta, elaborazione media.

**Lettura:**
- Nel Nord Atlantico e nel NW Pacifico passa un minimo **ogni 5–6 giorni in tutte le stagioni**: la classica serie di fronti, in linea con l'intervallo di 3–7 giorni della letteratura.
- In Mediterraneo l'intervallo si allunga molto d'estate: Otranto 16 giorni, Gibilterra 48 giorni (regime quasi stagnante).
- Una burrasca ≥ Bft 8 ha durata mediana **5–8 h**; il 90° percentile è 15–20 h.
- Gli episodi Hs ≥ 4 m durano molto di più, fino a 25–29 h di mediana alle Faroe e al ciglio celtico in inverno: il mare resta grosso tra una burrasca e l'altra.

### 6.2 Persistenza: autocorrelazione delle anomalie

Calcolata sulle anomalie dalla media mensile.

| Teatro | Stagione | r(1 h) | r(6 h) | r(12 h) | r(24 h) | r(48 h) | r(72 h) | Hs r(12 h) | Hs r(24 h) | Hs r(48 h) | ritardo vento→onda (h) | Hs mediana per U 10–15 / 15–20 / 20–25 m/s | escursione diurna vento (m/s; ora max locale) |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 Faroe | inverno | 0,98 | 0,74 | 0,49 | 0,28 | 0,15 | 0,13 | 0,77 | 0,55 | 0,36 | 4 | 4,0 / 5,8 / 8,6 | 0,3 (7h) |
| 1 Faroe | estate | 0,98 | 0,80 | 0,55 | 0,25 | 0,06 | -0,02 | 0,79 | 0,52 | 0,22 | 4 | 2,5 / 4,6 / – | 0,2 (5h) |
| 2 Barents | inverno | 0,98 | 0,75 | 0,51 | 0,28 | 0,12 | 0,05 | 0,72 | 0,45 | 0,21 | 2 | 2,8 / 4,7 / – | 0,3 (12h) |
| 2 Barents | estate | 0,98 | 0,75 | 0,48 | 0,18 | 0,00 | 0,00 | 0,75 | 0,46 | 0,15 | 4 | 2,1 / 4,6 / – | 0,2 (0h) |
| 3 Gibilterra | inverno | 0,99 | 0,83 | 0,62 | 0,30 | 0,02 | -0,03 | 0,75 | 0,44 | 0,08 | 2 | 1,7 / 3,4 / – | 0,4 (17h) |
| 3 Gibilterra | estate | 0,98 | 0,76 | 0,56 | 0,28 | 0,03 | 0,04 | 0,73 | 0,40 | 0,02 | 2 | 1,2 / – / – | 0,9 (20h) |
| 4 Otranto | inverno | 0,99 | 0,80 | 0,56 | 0,27 | 0,09 | 0,06 | 0,72 | 0,43 | 0,21 | 4 | 1,7 / 2,9 / – | 0,5 (17h) |
| 4 Otranto | estate | 0,97 | 0,71 | 0,48 | 0,23 | 0,00 | 0,00 | 0,65 | 0,38 | 0,04 | 2 | 1,4 / – / – | 1,3 (16h) |
| 5 Kunashir (Okhotsk) | inverno | 0,98 | 0,77 | 0,52 | 0,21 | 0,05 | 0,02 | 0,71 | 0,38 | 0,08 | 2 | 2,8 / 4,7 / 7,1 | 0,6 (14h) |
| 5 Kunashir (Okhotsk) | estate | 0,98 | 0,83 | 0,62 | 0,30 | 0,10 | -0,01 | 0,74 | 0,44 | 0,16 | 4 | 1,7 / 3,9 / – | 0,4 (6h) |
| 5b Curili (Pacifico) | inverno | 0,98 | 0,77 | 0,50 | 0,15 | 0,03 | 0,01 | 0,70 | 0,35 | 0,08 | 2 | 2,8 / 4,5 / 6,5 | 0,6 (15h) |
| 5b Curili (Pacifico) | estate | 0,98 | 0,83 | 0,64 | 0,30 | 0,06 | -0,03 | 0,81 | 0,52 | 0,19 | 6 | 2,4 / 5,1 / – | 0,3 (18h) |
| 6 Celtica | inverno | 0,98 | 0,81 | 0,60 | 0,37 | 0,22 | 0,17 | 0,84 | 0,63 | 0,40 | 6 | 4,1 / 6,2 / 9,2 | 0,3 (8h) |
| 6 Celtica | estate | 0,98 | 0,84 | 0,66 | 0,40 | 0,19 | 0,09 | 0,83 | 0,59 | 0,29 | 4 | 2,7 / 4,7 / – | 0,3 (9h) |
| 7 Baltico | inverno | 0,99 | 0,81 | 0,60 | 0,37 | 0,23 | 0,19 | 0,64 | 0,38 | 0,21 | 2 | 1,9 / 3,4 / – | 0,3 (19h) |
| 7 Baltico | estate | 0,98 | 0,77 | 0,55 | 0,33 | 0,14 | 0,07 | 0,68 | 0,41 | 0,16 | 2 | 1,6 / 3,1 / – | 0,2 (16h) |

**Lettura:**
- La **scala di decorrelazione del vento** è τ ≈ 15–25 h: r(12 h) ≈ 0,5–0,65, r(24 h) ≈ 0,15–0,4. Da r(24 h) = e^(−24/τ) si ottiene τ = 13–25 h; le scale più lunghe sono al ciglio celtico e nel Baltico (24 h).
- **Hs è più persistente:** r(24 h) ≈ 0,4–0,6, cioè τ ≈ 30–50 h in Atlantico.
- **[GIOCO]** Anomalia del vento come processo AR(1) orario con φ = e^(−1/τ). Per τ = 20 h, φ = 0,951 (variabile per teatro e stagione dalla tabella). Sopra si sovrappone il regime sinottico (§7).

### 6.3 Catena di Markov delle classi di vento (passo di 6 h)

Probabilità (%) di passare dalla classe di riga a quella di colonna in 6 h. Classi: Calmo < 5,5 m/s; Moderato 5,5–10,8; Forte 10,8–17,2 (Bft 6–7); Burrasca ≥ 17,2 (Bft ≥ 8). "–" indica una classe con meno di 5 casi.

| Teatro | Stagione | da \ a | Calmo < 5,5 | Moderato 5,5–10,8 | Forte 10,8–17,2 | Burrasca ≥ 17,2 | quota del tempo nella classe (%) |
|---|---|---|---:|---:|---:|---:|---:|
| 1 Faroe | inverno | Calmo | 50 | 41 | 9 | 0 | 12,8 |
| 1 Faroe | inverno | Moderato | 14 | 62 | 22 | 1 | 38,6 |
| 1 Faroe | inverno | Forte | 2 | 22 | 69 | 8 | 41,4 |
| 1 Faroe | inverno | Burrasca | 1 | 3 | 46 | 50 | 7,2 |
| 1 Faroe | estate | Calmo | 75 | 24 | 0 | 0 | 36,9 |
| 1 Faroe | estate | Moderato | 16 | 76 | 7 | 0 | 54,3 |
| 1 Faroe | estate | Forte | 2 | 43 | 55 | 0 | 8,7 |
| 1 Faroe | estate | Burrasca | – | – | – | – | 0,0 |
| 2 Barents | inverno | Calmo | 55 | 39 | 6 | 0 | 16,1 |
| 2 Barents | inverno | Moderato | 14 | 70 | 16 | 0 | 49,5 |
| 2 Barents | inverno | Forte | 1 | 26 | 71 | 2 | 33,5 |
| 2 Barents | inverno | Burrasca | 0 | 3 | 59 | 38 | 0,9 |
| 2 Barents | estate | Calmo | 72 | 28 | 0 | 0 | 40,1 |
| 2 Barents | estate | Moderato | 20 | 76 | 4 | 0 | 54,2 |
| 2 Barents | estate | Forte | 0 | 40 | 59 | 0 | 5,7 |
| 2 Barents | estate | Burrasca | – | – | – | – | 0,1 |
| 3 Gibilterra | inverno | Calmo | 75 | 24 | 0 | 0 | 30,4 |
| 3 Gibilterra | inverno | Moderato | 14 | 76 | 9 | 0 | 52,8 |
| 3 Gibilterra | inverno | Forte | 0 | 31 | 68 | 1 | 16,6 |
| 3 Gibilterra | inverno | Burrasca | 0 | 0 | 100 | 0 | 0,2 |
| 3 Gibilterra | estate | Calmo | 78 | 22 | 0 | 0 | 44,4 |
| 3 Gibilterra | estate | Moderato | 18 | 80 | 2 | 0 | 54,2 |
| 3 Gibilterra | estate | Forte | 0 | 76 | 24 | 0 | 1,5 |
| 3 Gibilterra | estate | Burrasca | – | – | – | – | 0,0 |
| 4 Otranto | inverno | Calmo | 76 | 23 | 1 | 0 | 39,1 |
| 4 Otranto | inverno | Moderato | 20 | 69 | 10 | 0 | 45,1 |
| 4 Otranto | inverno | Forte | 2 | 30 | 66 | 2 | 15,3 |
| 4 Otranto | inverno | Burrasca | 0 | 28 | 33 | 39 | 0,5 |
| 4 Otranto | estate | Calmo | 78 | 21 | 0 | 0 | 51,7 |
| 4 Otranto | estate | Moderato | 24 | 73 | 2 | 0 | 46,2 |
| 4 Otranto | estate | Forte | 3 | 62 | 36 | 0 | 2,1 |
| 4 Otranto | estate | Burrasca | – | – | – | – | 0,0 |
| 5 Kunashir (Okhotsk) | inverno | Calmo | 62 | 36 | 2 | 0 | 19,0 |
| 5 Kunashir (Okhotsk) | inverno | Moderato | 14 | 71 | 15 | 0 | 50,2 |
| 5 Kunashir (Okhotsk) | inverno | Forte | 1 | 27 | 70 | 2 | 29,5 |
| 5 Kunashir (Okhotsk) | inverno | Burrasca | 0 | 2 | 54 | 44 | 1,4 |
| 5 Kunashir (Okhotsk) | estate | Calmo | 84 | 16 | 0 | 0 | 52,9 |
| 5 Kunashir (Okhotsk) | estate | Moderato | 19 | 78 | 3 | 0 | 43,5 |
| 5 Kunashir (Okhotsk) | estate | Forte | 2 | 34 | 62 | 2 | 3,5 |
| 5 Kunashir (Okhotsk) | estate | Burrasca | – | – | – | – | 0,1 |
| 5b Curili (Pacifico) | inverno | Calmo | 58 | 37 | 5 | 0 | 17,8 |
| 5b Curili (Pacifico) | inverno | Moderato | 16 | 68 | 16 | 0 | 47,0 |
| 5b Curili (Pacifico) | inverno | Forte | 1 | 25 | 72 | 2 | 33,6 |
| 5b Curili (Pacifico) | inverno | Burrasca | 0 | 4 | 44 | 53 | 1,6 |
| 5b Curili (Pacifico) | estate | Calmo | 85 | 15 | 0 | 0 | 54,3 |
| 5b Curili (Pacifico) | estate | Moderato | 20 | 76 | 4 | 0 | 41,8 |
| 5b Curili (Pacifico) | estate | Forte | 0 | 39 | 59 | 2 | 3,8 |
| 5b Curili (Pacifico) | estate | Burrasca | – | – | – | – | 0,1 |
| 6 Celtica | inverno | Calmo | 63 | 34 | 3 | 0 | 13,8 |
| 6 Celtica | inverno | Moderato | 11 | 70 | 19 | 0 | 43,4 |
| 6 Celtica | inverno | Forte | 1 | 22 | 72 | 6 | 37,8 |
| 6 Celtica | inverno | Burrasca | 0 | 4 | 43 | 53 | 5,0 |
| 6 Celtica | estate | Calmo | 80 | 20 | 0 | 0 | 39,8 |
| 6 Celtica | estate | Moderato | 15 | 80 | 5 | 0 | 53,1 |
| 6 Celtica | estate | Forte | 0 | 38 | 61 | 1 | 7,0 |
| 6 Celtica | estate | Burrasca | – | – | – | – | 0,1 |
| 7 Baltico | inverno | Calmo | 66 | 33 | 1 | 0 | 19,4 |
| 7 Baltico | inverno | Moderato | 12 | 75 | 13 | 0 | 52,8 |
| 7 Baltico | inverno | Forte | 0 | 26 | 72 | 2 | 27,2 |
| 7 Baltico | inverno | Burrasca | 0 | 8 | 62 | 29 | 0,7 |
| 7 Baltico | estate | Calmo | 77 | 23 | 0 | 0 | 43,9 |
| 7 Baltico | estate | Moderato | 20 | 77 | 4 | 0 | 50,9 |
| 7 Baltico | estate | Forte | 2 | 38 | 60 | 0 | 5,2 |
| 7 Baltico | estate | Burrasca | – | – | – | – | 0,0 |

Fiducia: dato alta, elaborazione media.

**Uso:** è la catena di riserva più semplice. Riproduce frequenze e persistenze medie ma non la sequenza fisica di un fronte. Esempio dalle Faroe in inverno: una burrasca ha il 50 % di probabilità di durare altre 6 h e il 46 % di calare a "forte". **[GIOCO]** È preferibile il modello a regimi del §7 con il vento AR(1) dentro il regime. Questa tabella serve a **verificare** che il generatore produca matrici simili.

### 6.4 Distribuzione del vento (Weibull, adattamento ai momenti)

| Teatro | Weibull inverno k / c (m/s) | Weibull estate k / c (m/s) |
|---|---|---|
| 1 Faroe | 2,65 / 12,1 | 2,39 / 7,5 |
| 2 Barents | 2,80 / 10,4 | 2,44 / 7,2 |
| 3 Gibilterra | 2,25 / 8,3 | 2,37 / 6,5 |
| 4 Otranto | 1,97 / 7,8 | 2,28 / 6,1 |
| 5 Kunashir | 2,61 / 10,1 | 2,12 / 6,2 |
| 5b Curili Pac. | 2,63 / 10,4 | 2,16 / 6,2 |
| 6 Celtica | 2,64 / 11,4 | 2,37 / 7,2 |
| 7 Baltico | 2,69 / 9,7 | 2,31 / 6,9 |

Fiducia media. k ≈ 2–2,8 è tipico del vento marino. **[GIOCO]** Generare z gaussiano AR(1) e trasformarlo con la Weibull del teatro e del mese (*quantile mapping*): si conservano insieme la distribuzione e la persistenza.

### 6.5 Crescita e decadimento del mare

**Relazioni del Coastal Engineering Manual** (CEM, USACE EM 1110-2-1100; riportate da https://planetcalc.com/4442; fiducia media. Sono le stesse dello *Shore Protection Manual* 1984, che secondo Bishop, Donelan & Kahma 1988 tende a sovrastimare, https://publications.gc.ca/site/eng/9.849522/publication.html):
- coefficiente di attrito: `C_D = 0,001 (1,1 + 0,035 U10)`, `u* = √C_D · U10`;
- fetch limitato: `g·Hm0/u*² = 4,13·10⁻² (g·X/u*²)^½`; `g·Tp/u* = 0,751 (g·X/u*²)^⅓`;
- durata limitata, fetch equivalente: `g·X/u*² = 5,23·10⁻³ (g·t/u*)^1,5`;
- mare pienamente sviluppato (CEM): `g·Hm0/u*² = 211,5`; `g·Tp/u* = 239,8`.

**Pierson–Moskowitz** (1964, JGR 69, 5181, https://doi.org/10.1029/JZ069i024p05181; fiducia alta): mare pienamente sviluppato `Hs ≈ 0,21 U²/g` (U a 19,5 m; con U10 il coefficiente è circa 0,24–0,27).

Valori calcolati per il gioco (CEM; ultima colonna PM):

| U10 (m/s) | Hs dopo 6 h | 12 h | 24 h | 48 h | Hs con fetch 50 km | 200 km | 500 km | Pienamente sviluppato CEM (tempo, fetch) | Pienamente sviluppato PM |
|---|---|---|---|---|---|---|---|---|---|
| 10 | 0,9 | 1,5 | 2,5* | 4,3* | 1,1 | 2,2* | 3,6* | 3,1 m (32 h, 390 km) | 2,1 m |
| 15 | 1,6 | 2,7 | 4,5 | 7,6* | 1,8 | 3,6 | 5,6* | 7,9 m (50 h, 980 km) | 4,8 m |
| 20 | 2,4 | 4,1 | 6,9 | 11,6* | 2,5 | 5,0 | 7,9 | 15,5 m (70 h, 1 900 km) | 8,6 m |
| 25 | 3,4 | 5,8 | 9,7 | 16,3* | 3,3 | 6,6 | 10,4 | 26,6 m (92 h, 3 300 km) | 13,4 m |

\* oltre il limite di sviluppo pieno di Pierson–Moskowitz.

- **Confronto con i dati:** con vento 20–25 m/s ERA5 dà Hs mediana 8,6 m alle Faroe e 9,2 m al ciglio celtico, cioè il valore **PM**. Il "pienamente sviluppato" CEM è troppo alto.
- **[GIOCO]** `Hs_eq = min(H_CEM(durata, fetch), 0,22 · U10² / g)`; il coefficiente 0,22 è una STIMA fra 0,21 di PM e i valori ERA5 osservati.
- **Dinamica:** `dHs/dt = (Hs_eq − Hs) / τ`, con τ_crescita ≈ 4–8 h (coerente con il ritardo vento→onda di 2–6 h) e τ_decadimento ≈ 12–24 h per il mare di vento. Lo swell decade più lentamente, in giorni (STIMA coerente con le autocorrelazioni del §6.2).
- **Fetch per teatro** (STIMA dalla geografia):
  - Baltico: 50–250 km secondo la direzione.
  - Otranto: N–S circa 150–800 km, E–W < 100 km.
  - Gibilterra: E–W 100–600 km.
  - Barents e Okhotsk: 300–1 000 km.
  - Atlantico: illimitato, quindi durata limitata più swell.
- **Ghiaccio:** dentro il pack Hs = 0. Nel bordo del ghiaccio (*marginal ice zone*) Hs è smorzato di circa il 50 % ogni 10–50 km (STIMA).

### 6.6 Fisica della visibilità

- **Legge di Koschmieder:** `V = −ln(ε)/σ`. Con soglia di contrasto ε = 0,02, `V = 3,912/σ`; con ε = 0,05 (portata ottica meteorologica, MOR, standard WMO), `V = 2,996/σ`. σ è il coefficiente di estinzione in km⁻¹ (WMO-No. 8, CIMO Guide, https://library.wmo.int/idurl/4/68695; fiducia alta).
- **Coefficienti di estinzione** (codice NOAA/NCEP CALVIS, https://www.nco.ncep.noaa.gov/pmb/codes/nwprod/rap.v5.1.19/sorc/rap_sndp.fd/CALVIS.f e CALVIS_GSD.f; fiducia alta come uso operativo, coefficienti da articoli citati nel codice). C è la concentrazione in g/m³:
  - **goccioline di nube o nebbia:** `σ = 144,7 · C^0,88` (Kunkel 1984);
  - **neve:** `σ = 10,36 · C^0,7776` (Stallabrass 1985);
  - **pioggia:** `σ = 2,24 · C^0,75` (distribuzione Marshall–Palmer);
  - si sommano: `σ_tot = σ_nebbia + σ_pioggia + σ_neve + σ_aerosol`.
- **Per il gioco**, in funzione della quantità di precipitazione R (mm/h). Il contenuto d'acqua in aria per la pioggia è circa `C ≈ 0,072 · R^0,88` g/m³ (relazione M–P; STIMA da formula nota, fiducia media):

| Condizione | Visibilità tipica | Fonte |
|---|---|---|
| Pioviggine forte | ≤ 0,4 km (≤ ¼ di miglio) | FMH-1, https://webharvest.gov/peth04/20041015105833/http:/www.ofcm.gov/fmh-1/pdf/ch8a.pdf (alta) |
| Neve forte / moderata / debole | ≤ 0,4 km / 0,4–0,8 km / > 0,8 km | FMH-1 (alta) |
| Pioggia debole / moderata / forte | ≤ 2,5 / 2,6–7,6 / > 7,6 mm/h (classi di intensità, non visibilità) | FMH-1 (alta) |
| Pioggia moderata (5 mm/h) | circa 4 km (σ ≈ 0,9 km⁻¹) | calcolo con σ pioggia (media) |
| Pioggia forte (25 mm/h) | circa 1,5 km (σ ≈ 2,6 km⁻¹) | calcolo (media) |
| Nebbia (LWC 0,05–0,2 g/m³) | 0,1–0,4 km | Kunkel (alta) |
| Foschia marina con UR 80–95 % | 5–20 km | STIMA (fiducia bassa); l'aerosol marino cresce per igroscopia con l'umidità |
| Polvere sahariana con scirocco (Mediterraneo) | < 5 nm, vicino all'Africa anche ≤ ½ nm | Atlante US Navy, Mediterraneo (alta) |

- Parametrizzazioni probabilistiche moderne della visibilità in funzione di intensità di pioggia e UR: Gultepe & Milbrandt 2010, JAMC 49, 36–46 (formule **n.v.**: non lette).

### 6.7 Spruzzo gelante: indice di Overland

- `PPR = Va · (Tf − Ta) / (1 + 0,3 · (Tw − Tf))`, con Va vento (m/s), Ta aria (°C), Tw mare (°C) e Tf = −1,7 °C (congelamento dell'acqua di mare). PPR ≤ 0 indica nessun icing.
- Classi: 0–22,4 debole (< 0,7 cm/h); 22,4–53,3 moderato (0,7–2 cm/h); 53,3–83 forte (2–4 cm/h); > 83 estremo (> 4 cm/h).
- Fonti: Overland 1990, *Weather and Forecasting* 5, 62–77; categorie in cm/h confermate da NOAA OPC, https://ocean.weather.gov/freezingSpray/ (fiducia alta per le classi in cm/h, media per le soglie di PPR, riportate da memoria). Taratura per una nave a circa 20 kt.
- **Effetti:**
  - Sulle navi: baricentro più alto, minore altezza metacentrica e più superficie al vento; in casi estremi capovolgimento (atlante US Navy).
  - Su aerei ed elicotteri: icing in nube a 0 / −20 °C (DTN; fiducia media).
  - **[GIOCO]** Un sottomarino in superficie o a quota periscopica con alberi esposti accumula ghiaccio su vela, periscopi e antenne. Proposte: blocco o rallentamento degli alberi, maggiore rumore idrodinamico, rischio d'immersione con ghiaccio (STIMA).

### 6.8 Fisica della nebbia per il generatore

| Tipo | Condizione fisica | Teatri e stagioni | Regola [GIOCO] |
|---|---|---|---|
| Avvezione (nebbia marina) | aria più calda e umida su acqua più fredda; punto di rugiada ≥ SST; vento 2–8 m/s | Curili (estate, Oyashio); Faroe, Barents, Celtica (estate); Gibilterra (Levante estivo su acqua fredda); Baltico (marzo–aprile, su acqua fredda o ghiaccio) | Probabilità ∝ (Td − SST) e durata del flusso dal settore caldo; dissipazione con rotazione a vento di terra o freddo (atlante: "la nebbia sparisce con il flusso settentrionale") |
| Fumo di mare (vapore) | aria molto più fredda del mare (≥ 9–10 °C), vento debole o moderato | Barents e Okhotsk e Baltico in inverno, vicino a ghiaccio o costa | Strato di 5–50 m, visibilità 0,2–1 km; il periscopio può emergere sopra lo strato (STIMA) |
| Radiazione | notte serena, vento debole, terra fredda; scivola sul mare costiero | coste baltiche (autunno e inverno), coste mediterranee (estate, vicino alle città) | Solo con fattore costa > 0; si dissolve dopo l'alba |
| Precipitazione o frontale | pioggia, pioviggine, scirocco umido | Adriatico (scirocco), Atlantico (fronte caldo) | Visibilità da σ_pioggia più componente di nebbia nel settore caldo |
| Bora scura o gregale | bora con depressione a S | Otranto | Nubi basse e pioggia; visibilità 2–5 km (STIMA) |

---

## 7. [GIOCO] Architettura del generatore deterministico

### 7.1 Principi

- Tutto deriva da `seme = hash(teatro, data d'inizio, id missione)`. Un PRNG (per esempio PCG32 o xorshift) per ciascun processo, così aggiungere un processo non cambia gli altri.
- **Passo di 1 h** per lo stato sinottico, interpolato al minuto per l'uso nel motore.
- **Un unico stato fisico** da cui si derivano tutte le grandezze visibili: niente dadi separati per pioggia, nubi e visibilità.

### 7.2 Livelli

1. **Regime sinottico** (semi-Markov), stati per teatro:
   - **Atlantico, Barents, Curili, Baltico:**
     - A. anticiclonico;
     - B. pre-frontale o fronte caldo (vento in aumento da S–SW, nubi in abbassamento, pioviggine, nebbia d'avvezione possibile in estate);
     - C. settore caldo (SW, nubi basse, pioggia debole);
     - D. fronte freddo (picco di vento, rotazione, groppi);
     - E. post-frontale freddo (NW, rovesci, buona visibilità fuori dai rovesci; in inverno neve, fumo di mare, icing);
     - F. bloccato o stazionario.
     - Sequenza tipica A→B→C→D→E→A in **5–6 giorni** (§6.1). Durata dei singoli stati da distribuzioni gamma: mediana B 12–24 h, C 6–24 h, D 3–8 h, E 12–48 h (STIMA da tarare sulle autocorrelazioni).
   - **Gibilterra:** Levante, Poniente, Vendaval, calma, *cut-off* (7–10 giorni).
   - **Otranto:** bora chiara, bora scura, scirocco, maestrale ed etesio di Otranto, calma estiva, temporale autunnale.
   - **Curili:** monsone NW, ciclone esplosivo (sequenza burrasca SE → NW), flusso da S con nebbia (estate), calma autunnale.
   - Il **Großwetterlage** di Hess–Brezowsky dura per definizione almeno 3 giorni (PIK Report 100, https://www.pik-potsdam.de/en/output/publications/pikreports/.files/pr100.pdf; fiducia alta). I tipi di Lamb sono giornalieri, con persistenza misurata come probabilità di ripetersi il giorno dopo (Jones et al., https://ueaeprints.uea.ac.uk/48576/1/lwt_weather_hhl100_accepted_version.pdf; fiducia alta). Il nostro regime è più fine, in ore; la sua durata media va tarata sulle matrici del §6.3.
2. **Vento:** media e direzione dal regime, più un'anomalia AR(1) (τ dal §6.2), poi quantile mapping Weibull (§6.4). Raffica = U · (1,3–1,5) (STIMA), più ampia nei rovesci e nel regime E.
3. **Onde:** mare di vento con rilassamento verso Hs_eq (§6.5), più swell. Lo swell è un processo separato in Atlantico, con eventi generati da tempeste fuori dal riquadro (Hs 1–4 m, Tp 10–16 s).
4. **Nubi e precipitazione:** catena oraria a due stati, asciutto o bagnato, alla maniera di WGEN, con probabilità di transizione dipendenti dal regime.
   - Richardson 1981, *Water Resources Research* 17(1), 182–190, https://doi.org/10.1029/WR017i001p00182 (catena di Markov del primo ordine per il giorno bagnato o asciutto e processi AR per le altre variabili; fiducia alta).
   - Intensità da gamma o esponenziale. Neve se Ta < 1 °C (STIMA 0,5–2 °C).
   - Tarare la frequenza oraria sui valori "% ore con precipitazione" delle schede.
5. **Temperatura aria e mare:** media mensile delle schede, più un'anomalia di regime (E: −2 / −6 °C d'inverno; C: +2 °C) e un'anomalia AR(1) lenta (τ circa 3 giorni, STIMA). La SST cambia solo su scala di settimane.
6. **Visibilità:** σ_tot dal §6.6, con nebbia dal §6.8 e foschia di fondo per massa d'aria. Polare marittima: 30–50 km. Mediterraneo estivo: 10–20 km. Polvere con scirocco: 2–8 km (STIMA).
7. **Ghiaccio:** bordo mensile da climatologia (§2.2, 2.5, 2.7) più deriva sotto vento: circa 2–3 % della velocità del vento (regola empirica nota, fiducia media). Per il Baltico, opzione "inverno severo" per il 1985–1987.
8. **Icing:** PPR orario (§6.7) integrato nel tempo, con accumulo e fusione.
9. **Eventi rari:** processi di Poisson con i tassi del §5.9, che sovrascrivono il regime per la loro durata.

### 7.3 Coerenza fra variabili (vincoli da rispettare sempre)

- Vento forte e Hs bassa non possono coesistere a lungo: Hs segue il vento con ritardo di 2–6 h. In mare chiuso Hs è limitata dal fetch.
- Nebbia d'avvezione e vento ≥ 10 m/s si escludono: la nebbia si alza in strato basso. Il fumo di mare richiede aria molto fredda.
- Il cielo sereno (≤ 2/8) esclude la precipitazione e permette la nebbia di radiazione notturna vicino alla costa.
- I rovesci (regime E) riducono la visibilità per minuti o decine di minuti; la pioggia frontale (regimi B e C) per ore.
- La pressione deve scendere prima del fronte: 1–3 hPa/3 h normale, ≥ 8 hPa/3 h nei casi esplosivi (STIMA). Al giocatore il barometro serve come strumento previsionale (§8).

### 7.4 Taratura e test

Generare 100 anni sintetici per teatro e confrontarli con le tabelle di questa nota:
- frequenze mensili di Bft 7 e 8 e di Hs ≥ 4 m;
- distribuzione delle classi Douglas (§3.1);
- durate degli episodi (§6.1);
- autocorrelazioni (§6.2);
- matrici di Markov (§6.3).

Tolleranza proposta: ±20 % relativo (STIMA).

### 7.5 Dati da esportare in JSON per il motore

Per teatro e mese:
- quantili del vento (p10, p50, p90), parametri Weibull, rosa a 8 settori;
- media e p90 di Hs, periodo, quota di swell;
- T aria, SST, nuvolosità, frequenza di precipitazione e di neve;
- frequenze dei regimi, tassi degli eventi rari, bordo del ghiaccio.

Si possono rigenerare dagli script di questa nota estendendo il punto singolo a 3–5 punti per teatro.

### 7.6 Modulazione spaziale nel riquadro

- **Ridosso:** sottovento a un'isola o a una costa entro 10–20 nm, vento × 0,6–0,8 e fetch ridotto, quindi Hs minore (STIMA).
- **Stretti** (Gibilterra, passo di Kunashir, Belt): vento × 1,3–1,6 lungo l'asse (STIMA).
  - L'onda contro corrente diventa più ripida a parità di Hs: a Gibilterra è il "mare corto e ripido" dell'atlante.
- **Bordo del ghiaccio:** aria più fredda (−5 / −10 °C rispetto al mare aperto), fumo di mare, onde smorzate. L'atlante sconsiglia di cercare riparo sottovento al bordo del ghiaccio: un cambio di vento lascia la nave esposta.
- **Fronte termico** Islanda–Faroe: nebbia d'avvezione più probabile sul lato freddo.

---

## 8. Previsioni nel 1989: qualità e comunicazione

### 8.1 Qualità

| Indicatore | Valore | Fonte | Fiducia |
|---|---|---|---|
| ECMWF, correlazione di anomalia a 500 hPa al 60 % (limite della previsione "utile") | circa **giorno 5** nei primi anni '80; circa giorno 8 nel 2010; miglioramento di circa **1 giorno per decennio** | ECMWF, https://www.ecmwf.int/node/25701 | alta |
| Met Office | "la previsione a quattro giorni è oggi accurata quanto quella a un giorno di 30 anni fa" (comunicato 2017, quindi riferito al 1987) | https://www.metoffice.gov.uk/about-us/press-office/news/corporate/2017/100-years-of-scientific-forecasting | alta |
| Risoluzione dei modelli globali | Met Office: griglia da 90 km (circa 1987) a 10 km (2017) | idem | alta |
| US Navy | NOGAPS operativo dall'agosto 1982 (9 livelli, differenze finite); FNOC a Monterey; prime previsioni operative di strato rimescolato oceanico (1981) | Wikipedia FNMOC, https://en.wikipedia.org/wiki/Fleet_Numerical_Meteorology_and_Oceanography_Center; rapporti DTIC citati nei risultati di ricerca | media |
| Fallimento celebre | Great Storm del 15–16 ottobre 1987: avvisi di maltempo nei giorni precedenti, ma la sera prima i modelli davano la tempesta in sfioramento alla costa e l'intensità fu sbagliata. Dopo l'inchiesta furono potenziate le osservazioni da navi, aerei e boe a SW del Regno Unito | Met Office, https://www.metoffice.gov.uk/about-us/who/our-history/lessons-and-legacy-of-the-great-storm-of-1987 | alta |
| Polar low | quasi mai sulle carte; nessuna previsione affidabile nel 1989 | Smirnova 2015 | alta |
| Errori quantitativi su pressione al suolo e vento a 24–72 h nel 1989 | **n.v.** | — | — |

### 8.2 [GIOCO] Modello d'errore della previsione

Il giocatore riceve un bollettino alle 00, 06, 12 e 18 UTC solo se in ascolto radio (vedi `10_radio_ruolo_in_partita.md`). La previsione si ottiene facendo girare lo stesso generatore con una **perturbazione** il cui errore cresce col tempo.

Errore di fase dei fronti e dei regimi (STIMA ricavata dalla regola Met Office "1 giorno 1987 ≈ 4 giorni oggi" e dalle verifiche moderne):

| Orizzonte | Errore di fase dei fronti | Errore sul vento | Errore su Hs | Fenomeni mancati |
|---|---|---|---|---|
| 12 h | ±2 h | σ 1,5 m/s | σ 0,3 m | — |
| 24 h | ±4 h | σ 2,5 m/s | σ 0,5 m | — |
| 48 h | ±10 h | σ 3,5 m/s | — | — |
| 72 h | ±18 h | σ 4,5 m/s | — | il 20–30 % delle burrasche è previsto 6–12 h in ritardo o con 1 forza in meno |

- Le **polar low**, i getti locali degli stretti, i temporali e le trombe marine **non compaiono mai** nella previsione: al più si annuncia "possibili rovesci di neve con raffiche".
- La nebbia si dà solo come "possibile banchi di nebbia" o "visibilità moderata o scarsa".
- Lessico del bollettino, ispirato allo *Shipping Forecast* della BBC (fiducia media): direzione e forza, "in aumento" o "in calo", "più tardi", stato del mare, tempo, visibilità buona, moderata, scarsa o molto scarsa.
- Il giocatore può migliorare la previsione per conto proprio con barometro (tendenza su 3 h), vento, nubi e swell, come facevano gli ufficiali di rotta.

---

## 9. Lacune (n.v.) e lavoro futuro

- Frequenza mensile della visibilità < 1 km e < 5 km da osservazioni (ICOADS) **per le Faroe, il Barents, Otranto e il ciglio celtico**. Da estrarre dai file IMMA di ICOADS R3 (codici *present weather* 40–49 per la nebbia, visibilità VV) per riquadri di 2°. Lavoro lungo.
- Giorni con temporale per stazione costiera (Tórshavn, Murmansk, Brindisi, Tarifa, Kuril'sk, Scilly, Bornholm): n.v.
- Densità di fulmini LIS/OTD per teatro e stagione: il file HRMC (0,5°) non era scaricabile senza account Earthdata.
- Latitudine media del bordo del ghiaccio nel Barents in marzo (31–39 E) e date normali del ghiaccio a Nemuro e Kunashir: n.v. (dati NSIDC o JMA da scaricare).
- Mappe di ritorno dei medicane per il canale d'Otranto (figura 8 di Romero & Emanuel 2013 non letta in dettaglio).
- Errori numerici delle previsioni di pressione e vento a 24–72 h nel 1985–1990 (verifiche ECMWF e NMC dell'epoca).
- Validazione delle soglie numeriche di PPR di Overland sull'articolo originale.

---

## 10. Fonti principali (riepilogo)

| Fonte | URL | Fiducia |
|---|---|---|
| ERA5, Hersbach et al. 2020 | https://doi.org/10.1002/qj.3803 | alta |
| Open-Meteo, API storiche e marine | https://open-meteo.com/en/docs/historical-weather-api ; https://open-meteo.com/en/docs/marine-weather-api | alta (dato) |
| US Navy Marine Climatic Atlas of the World, Ver. 1.1 (1995), testo: Mediterraneo | https://www.seekrieg.com/MediterraneanWeather.pdf | alta (copia su sito terzo) |
| idem, Europa | https://www.seekrieg.com/EuropeWeather.pdf | alta |
| idem, Nord Pacifico | https://www.seekrieg.com/NorthPacificWeather.pdf | alta |
| Dorman et al. 2019, nebbia marina mondiale (ICOADS) | https://openpolar.no/Record/crwiley:10.1002%2Fjoc.6200 | alta |
| Dorman & Koračin, nebbia nel Nord Europa (abstract AMS 2017) | https://ams.confex.com/ams/97Annual/webprogram/Paper307556.html | media |
| Frontiers 2022, nebbia nel NW Pacifico | https://www.frontiersin.org/journals/earth-science/articles/10.3389/feart.2022.942846/full | alta |
| Tabella climatica di Iturup (IKIP) | https://www.burkemuseum.org/static/okhotskia/ikip/Results/publications/entobook/Img/table15.htm | media |
| Smirnova et al. 2015, polar low | https://archimer.ifremer.fr/doc/00275/38639/37161.pdf | alta |
| Noer et al. 2011; Rojo et al. 2015 | https://openpolar.no/Record/crwiley:10.1002%2Fqj.846 ; https://hal.archives-ouvertes.fr/hal-01872789 | alta |
| Romero & Emanuel 2013, medicane | https://texmex.mit.edu/pub/emanuel/PAPERS/Romero_Emanuel_2013.pdf | alta |
| Renko et al. 2016, trombe marine in Adriatico | https://link.springer.com/article/10.1007/s11069-016-2192-5 | alta |
| Enno & Sugier 2018, fulmini ATDnet | URL Vaisala nel testo | media |
| Ghiaccio baltico 1985–1987 | https://www.eesti.ca/ice/article12829 ; https://helcom.fi/publications/bsep111/ | media e alta |
| JMA, ghiaccio di Hokkaido | https://ds.data.jma.go.jp/tcc/tcc/news/topics_20060526.html ; https://www.data.jma.go.jp/wakkanai/kishou/zyouhou/wakkanai_iceinf2020-1.pdf | alta |
| NOAA OPC, spruzzo gelante | https://ocean.weather.gov/freezingSpray/ | alta |
| Formule d'onda CEM | https://planetcalc.com/4442 | media |
| NOAA CALVIS (visibilità) | https://www.nco.ncep.noaa.gov/pmb/codes/nwprod/rap.v5.1.19/sorc/rap_sndp.fd/CALVIS.f | alta |
| FMH-1 (intensità delle precipitazioni) | https://webharvest.gov/peth04/20041015105833/http:/www.ofcm.gov/fmh-1/pdf/ch8a.pdf | alta |
| ECMWF, qualità delle previsioni | https://www.ecmwf.int/node/25701 | alta |
| Met Office, 100 anni e Great Storm 1987 | URL nel §8 | alta |
| Richardson 1981, WGEN | https://doi.org/10.1029/WR017i001p00182 | alta |
| Hess–Brezowsky (PIK 100) | https://www.pik-potsdam.de/en/output/publications/pikreports/.files/pr100.pdf | alta |
| Murmansk, giorni di nebbia | https://www.climates.com/ASIA/PDF/RUS04EUR.pdf | bassa-media |

