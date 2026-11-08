# 21 — Meteo e mare: un sistema unico (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1985-1990**. Scopo: un meteo **realistico e dinamico** per i sette teatri e lo **stato del mare** con
> tutto ciò che comporta per ogni mezzo, sensore, arma e animazione. Il meteo compare nel **pre-briefing**, cambia
> durante la missione con l'ora e la stagione, e arriva via radio quando si è in ascolto. Prevede foschia, nebbia,
> temporali con lampi e fulmini, eventi estremi rari e la superficie del mare mossa.
> Compilato il 25 ottobre 2026 da quattro ricerche parallele, le cui note complete stanno in `docs/ricerca/21_note/`:
> - **A** `a_climatologia.md`: climatologia e generatore, con le tabelle mensili ERA5 1981-1990 di ogni teatro;
> - **B** `b_sensori.md`: effetti sui sensori;
> - **C** `c_mezzi.md`: limiti dei mezzi e soglie per ogni stato del mare;
> - **D** `d_briefing_grafica.md`: briefing d'epoca, radio, grafica, animazioni, fulmini, immagini con licenza.
>
> Qui stanno la sintesi, il modello unico, le soglie raccolte e corrette, la proposta di gioco e il contratto per
> `nucleo/meteo`. Le tabelle lunghe restano nelle note e sono citate come «A §2.1», «C §12.4» e così via.

---

## 0. Convenzioni e punto di partenza

**Confidenza:**
- **alta:** manuale ufficiale, rianalisi, articolo con revisione;
- **media:** fonte secondaria seria, oppure nostra elaborazione di dati di confidenza alta;
- **bassa:** forum, gioco;
- **n.v.:** non trovato.

**Marcature:**
- **STIMA:** numero proposto, da tarare;
- **[GIOCO]:** proposta;
- **[CALC]:** calcolato da una formula citata.

**Dati ERA5.** La spina dorsale numerica è ERA5, la rianalisi ECMWF/Copernicus, oraria 1981-1990 (Hersbach et al.
2020), scaricata via Open-Meteo con un punto in mare aperto per teatro (A §0.1).
- Gli script e le tabelle elaborate sono in `docs/ricerca/21_note/era5/`; i dati grezzi si riscaricano con
  `scarica.py`.
- ERA5 **sottostima i venti forti** e non dà la visibilità: le correzioni sono in A §0.2 e qui al §4.

**Stato del mare.** Si usa la scala **Douglas/WMO** (codice 3700), sempre legata ad Hs. Le fonti sovietiche usano i
**баллы**, una scala propria che non coincide con quella WMO: le conversioni sono in C §1.

| WMO | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Hs (m) | 0 | 0-0,1 | 0,1-0,5 | 0,5-1,25 | 1,25-2,5 | 2,5-4 | 4-6 | 6-9 | 9-14 | > 14 |
| Beaufort tipico (mare formato) | 0 | 1 | 2-3 | 4 | 5 | 6-7 | 8-9 | 10 | 11 | 12 |

**Che cosa c'è oggi nel motore:**
- `missione.cpp` usa una costante `STATO_MARE = 3` per ESM e radar.
- `acustica` ha:
  - `Ambiente` con vento, pioggia e ghiaccio;
  - `stato_del_mare(vento)` istantaneo;
  - una tabella `altezza_onda_m(ss)`.
- `periscopi` ha una seconda tabella `altezza_significativa_m(ss)` e la finestra coperta dall'onda
  (`nucleo/onde.hpp`).
- La vista ha `altezza_onde_m` e `visibilita_m`; nell'Enhanced ha il mare mosso, l'acqua sulla finestra e la pioggia
  (P3).
- `asw_aereo` ha i limiti `mare_max` di boe e sonar a immersione.
- **Non c'è un modello del tempo:** niente vento che cambia, niente nebbia, niente temporali, niente previsione.

**Il classico resta identico.** Tutto ciò che segue vale solo con la regola di simulazione.

---

## 1. Sintesi

1. **Il vento è la variabile madre.** Fa il mare (con ore di ritardo) e il rumore ambiente, rimescola lo strato
   superficiale, alza il clutter radar, copre la piuma del periscopio, scuote gli aerei. Lo **swell** (onda lunga) è
   un secondo strato indipendente, che resta dopo che il vento è calato (A §3.2).
2. **Il maltempo colpisce chi caccia più di chi è cacciato** (C §0):
   - perdono capacità gli elicotteri, gli aerei bassi, i sonar di scafo, le boe e le vedette;
   - un SSN a 100 m o più con mare 7 è quasi immune;
   - in cambio non può stare a quota periscopica, comunica male, e un diesel non può ricaricare.

   Il meteo diventa così **una scelta tattica**: aspettare il fronte, oppure attaccare col mare calmo che però
   rivela la piuma.
3. **I teatri sono molto diversi** (§3):
   - Faroe e Celtica d'inverno stanno al 70% del tempo in mare 5-6;
   - Gibilterra e Otranto hanno mare quasi calmo per un terzo dell'anno;
   - Kunashir ha la nebbia più frequente del mondo d'estate e il ghiaccio d'inverno;
   - il Barents ha le polar low;
   - il Baltico ha mare corto e, negli inverni 1985-87, ghiaccio.
4. **Le scale di tempo sono lente e regolari** (A §6):
   - nell'Atlantico e nel Pacifico nord-occidentale passa un minimo ogni **5-6 giorni**;
   - una burrasca dura in mediana **5-8 h**;
   - il vento perde memoria in **15-25 h**, il mare in **30-50 h**.

   In una missione di poche ore il tempo **evolve**, ma non salta.
5. **Nel 1989 si prevedeva peggio di oggi:** una previsione a 24 h valeva quanto una a 4 giorni di oggi. Le polar low
   non si prevedevano affatto. Il bollettino del gioco può e deve sbagliare (§5).
6. **Nessun gioco d'epoca aveva il meteo:** non 688 Attack Sub, non Red Storm Rising, non F-19. Il riferimento
   leggibile è **Harpoon II**, con celle che si muovono e lo stato del mare che toglie portata al sonar passivo.
   L'effetto più apprezzato nei simulatori moderni è **l'acqua sulla lente** del periscopio (D §0).

---

## 2. Il modello fisico unico

**Un solo stato fisico**, da cui derivano tutte le grandezze visibili. Niente dadi separati per pioggia, nubi e
visibilità (A §7.1).

| Grandezza | Unità | Legame | Fonte |
|---|---|---|---|
| Vento a 10 m: media, direzione, raffica | m/s, gradi | Regime sinottico, più un'anomalia AR(1) con τ 15-25 h, più quantile mapping sulla Weibull del teatro e del mese. Raffica = U × 1,3-1,5, di più nei rovesci | A §6.2-6.4 (media), STIMA |
| Mare da vento: Hs, Tp, direzione | m, s | Rilassamento verso Hs_eq = min(H_CEM(durata, fetch), 0,22 U²/g); τ crescita 4-8 h, decadimento 12-24 h; Tp ≈ 5 √Hs (PM formato) | A §6.5 (alta per le relazioni, STIMA per τ) |
| Swell: Hs, Tp, direzione | m, s | Processo separato nei teatri oceanici (Faroe, Celtica, Curili): Hs 1-4 m, Tp 10-16 s, giorni di persistenza | A §3.2 (STIMA) |
| Hs totale, stato del mare | m, 0-9 | Hs = √(Hs_vento² + Hs_swell²); classe Douglas da Hs (non dal vento) | WMO 3700 (alta) |
| Nubi e plafond | ottavi, m | Dal regime; catena asciutto/bagnato alla WGEN | A §7.2 (alta come metodo) |
| Pioggia, neve | mm/h | Intensità gamma per regime; neve con aria sotto 0,5-2 °C | A §7.2, STIMA |
| Visibilità meteorologica | m | Koschmieder: V = 3,912/σ con σ = σ_nebbia + σ_pioggia + σ_neve + σ_aerosol, più spruzzi da Beaufort 9 | A §6.6, B §3.1 (alta) |
| Nebbia: tipo e spessore | — , m | Avvezione (Td ≥ SST, vento 2-8 m/s); fumo di mare (aria 9-10 °C sotto il mare, strato di 5-50 m); radiazione (costa, notte serena) | A §6.8 (media) |
| Temporale e lampi | lampi/min | Celle con 1-4 scariche al minuto; 3,67 colpi per scarica, a 57 ms l'uno dall'altro | A §5.8, D B-bis (media) |
| Temperatura dell'aria e del mare | °C | Media mensile, più un'anomalia di regime, più un AR(1) lento (τ circa 3 giorni); il mare cambia in settimane | A §7.2 |
| Pressione e tendenza | hPa, hPa/3 h | Scende prima del fronte (1-3 hPa/3 h; oltre 8 nei casi esplosivi): il barometro serve al giocatore | A §7.3 |
| Ghiaccio | frazione, bordo | Bordo mensile climatologico, più una deriva del 2-3% del vento; dentro il pack Hs = 0 | A §6.5, §7.2 |
| Spruzzo gelante | cm/h | Indice di Overland PPR = V (Tf − Ta) / (1 + 0,3 (Tw − Tf)): leggero, moderato, forte, estremo | A §6.7 (classi alta, soglie media) |
| Schiuma delle creste | frazione | Monahan: W = 3,84·10⁻⁶ U^3,41 (1% a 10 m/s) | B §3.2 (alta) |

**Vincoli di coerenza, sempre** (A §7.3):
- Vento forte e mare basso non convivono a lungo.
- La nebbia d'avvezione e il vento oltre 10 m/s si escludono.
- Cielo sereno significa niente precipitazione.
- I rovesci durano minuti, la pioggia frontale ore.
- La pressione scende prima del fronte.

**Il mare nel punto della testa** (già nel motore, `nucleo/onde.hpp`): cinque componenti attorno al picco di
Pierson-Moskowitz, con varianza (Hs/4)².
- La probabilità che una finestra a quota h sia coperta è Q(4h/Hs) (B §6.1, [CALC]).
- La vista Enhanced e il posto del periscopio usano la stessa funzione.

---

## 3. Climatologia dei teatri

### 3.1 In breve (A §1; tabelle mensili complete in A §2)

| Teatro | Inverno | Estate | Pericolo dominante | Visibilità |
|---|---|---|---|---|
| 1 Faroe - Norvegia | Vento 10,7 m/s, Hs 4 m; Hs ≥ 4 m per il 42-48% delle ore; burrasca ≥ Bft 8 circa ogni 6 giorni | Hs 1,6 m | Mare lungo e alto, swell atlantico (10 s) | Pioggia e rovesci d'inverno, nebbia d'avvezione d'estate |
| 2 Barents (Kola) | Vento 9,2 m/s, Hs 2,4 m; neve per il 30-40% delle ore | Calmo, Hs 1,1 m | Polar low improvvise, spruzzo gelante, fumo di mare | Neve e fumo di mare d'inverno; nebbia d'estate (Murmansk: 15-16% dei giorni in agosto-settembre) |
| 3 Gibilterra | Levante e Poniente alternati (E 36%, W 43% delle ore); Hs 1 m, ma mare corto e ripido | Levante frequente | Getto incanalato nello stretto (vento ×1,3-1,6) | Nebbia di Levante in luglio-agosto (< 2 nm per il 6-8% delle osservazioni) |
| 4 Otranto | Bora (improvvisa, 12 h) e scirocco (graduale, 10-12 h); Hs 0,9 m | Calmo e limpido | Bora, scirocco, temporali autunnali, trombe marine | Pioggia di scirocco; foschia estiva |
| 5 Kunashir - Okhotsk | Monsone da NW; neve per il 30% delle ore; **pack da febbraio ad aprile**; spruzzo gelante forte nel 7% delle ore di febbraio | **La nebbia più frequente del mondo** | Ghiaccio e spruzzo gelante d'inverno, nebbia d'estate | < 1 nm fino al **60-70%** del tempo d'estate nelle Curili centrali |
| 6 Celtica | Quasi come le Faroe: vento 10 m/s, Hs 3,9 m | Moderato | Burrasche atlantiche in serie, mare lungo | Pioviggine e pioggia frontale |
| 7 Baltico SW | Vento 8,5-9 m/s, ma Hs ≤ 1,4 m (fetch corto); ghiaccio solo negli inverni rigidi (**1985, 1986, 1987**) | Calmo, temporali sulle coste (2-4 giorni al mese) | Mare corto; ghiaccio negli inverni rigidi | Nebbia di marzo-aprile su mare freddo |

### 3.2 Stato del mare per teatro e stagione (% delle ore, A §3.1)

Classi: 0-2 (Hs < 0,5 m), 3-4 (0,5-2,5 m), 5-6 (2,5-6 m), 7 e oltre (≥ 6 m).

| Teatro | Inverno | Primavera | Estate | Autunno |
|---|---|---|---|---|
| 1 Faroe | 0 / 17 / 71 / 12 | 0 / 51 / 46 / 3 | 0 / 89 / 11 / 0 | 0 / 42 / 55 / 4 |
| 2 Barents | 0 / 62 / 37 / 1 | 1 / 82 / 17 / 0 | 5 / 92 / 3 / 0 | 1 / 76 / 22 / 1 |
| 3 Gibilterra | 14 / 82 / 5 / 0 | 24 / 73 / 3 / 0 | 37 / 63 / 0 / 0 | 32 / 67 / 1 / 0 |
| 4 Otranto | 26 / 71 / 3 / 0 | 39 / 60 / 1 / 0 | 58 / 42 / 0 / 0 | 44 / 55 / 1 / 0 |
| 5 Kunashir | 0 / 67 / 32 / 1 | 4 / 87 / 9 / 0 | 12 / 87 / 1 / 0 | 1 / 79 / 20 / 1 |
| 6 Celtica | 0 / 20 / 69 / 11 | 0 / 56 / 42 / 3 | 0 / 88 / 12 / 0 | 0 / 48 / 49 / 3 |
| 7 Baltico | 13 / 79 / 8 / 0 | 39 / 59 / 2 / 0 | 47 / 53 / 1 / 0 | 21 / 74 / 5 / 0 |

Il mare quasi calmo (0-2) esiste solo nei mari chiusi. Nell'Atlantico e nel Pacifico lo swell tiene Hs sopra 0,5 m
quasi sempre.

### 3.3 Eventi estremi: rari ma possibili (A §5)

| Evento | Teatri | Stagione | Frequenza nel riquadro (STIMA salvo dove citato) | Durata | Firma |
|---|---|---|---|---|---|
| Burrasca ≥ Bft 8 | 1, 6 | inverno | 11-15 episodi per inverno (ERA5) × 1,5 | mediana 5-7 h, fino a 36-39 h | fronte freddo, rotazione SW → NW |
| Tempesta ≥ Bft 10 | 1, 6 | inverno | ≈ 0,3 per inverno alle Faroe | 4-7 h | ciclone profondo |
| **Polar low** | 2 (1) | ottobre-aprile, massimo a marzo | 0,5-1 al mese nel Barents, metà alle Faroe (da 45 per stagione su tutta l'area, Smirnova 2015, alta) | 15 h (9-18) | esordio in 1-3 h, rovesci di neve, vento ≈ 20 m/s, **mai previste nel 1989** |
| Medicane | 4 (3) | settembre-gennaio | 1-3% al mese | 1-2 giorni | vento ≥ 34 kt, pioggia, mare 4-6 m |
| Bora in burrasca | 4 | novembre-marzo | — | media 12 h, fino a 2 giorni | crescita in 1-2 h |
| Nebbia persistente oltre 3 giorni | 5 | giugno-agosto | 1-2 per estate (massimi di 6-9 giorni) | 3-9 giorni | aria da S sull'Oyashio |
| Fumo di mare artico denso | 2 (5) | dicembre-febbraio | potenziale nel 7-15% delle ore | ore, a volte giorni | aria ≤ −10 °C |
| Spruzzo gelante forte | 5 (2) | gennaio-febbraio | 2-7% delle ore a Kunashir, 0,1-0,4% nel Barents | — | burrasca da NW, aria ≤ −10 °C |
| Temporale | tutti | vedi sotto | giorni per stagione: Otranto 4-7 d'autunno; Baltico 6-12 d'estate; Faroe 1-3 d'inverno; Kunashir quasi mai | ore | celle mobili |
| Tromba marina | 4 (3) | agosto-ottobre | 5-10% per giorno di temporale | minuti | sotto la cella |

- **Scenari storici utilizzabili:** la Great Storm del 15-16 ottobre 1987 (Manica), che fu anche un celebre errore di
  previsione. La tempesta del 25 gennaio 1990 va verificata.
- **Correzione dei venti ERA5** (A §0.2): per le code alzare la frequenza delle burrasche di **1,5-3** (STIMA), di
  più negli stretti. A Gibilterra portare la riga Bft 8 ai valori dell'atlante US Navy (2-4% in inverno).

### 3.4 Nella giornata (A §4)

- In mare aperto il ciclo diurno è **debole**: il vento varia di 0,2-0,9 m/s.
- Conta vicino alle coste: fattore costa c = 1 entro 5 nm, che scende a 0 a 25 nm (STIMA).
  - **Brezza:** ampiezza 2-3 m/s d'estate nel Mediterraneo e nel Baltico.
  - **Nebbia di radiazione** dopo mezzanotte, che si dissolve 1-3 h dopo l'alba.
  - **Temporali:** in mare il massimo è fra le 00 e le 09 locali; sotto le coste montuose fra le 14 e le 20.
- La nebbia d'avvezione è più fitta del 20% di notte e meno fitta del 20% nel primo pomeriggio (STIMA dai dati delle
  Curili).

---

## 4. Dinamica: il generatore deterministico (A §7)

[GIOCO]
- **Seme** = hash(teatro, data d'inizio, missione). Un generatore casuale per ogni processo, così aggiungerne uno non
  cambia gli altri.
- **Passo di 1 h** per lo stato sinottico, interpolato al secondo per il motore.

**I livelli del generatore:**
1. **Regime sinottico** (semi-Markov), diverso per teatro:
   - **Atlantico, Barents, Curili e Baltico:** anticiclonico → pre-frontale → settore caldo → fronte freddo →
     post-frontale → (anticiclonico), più un regime bloccato. Il ciclo dura **5-6 giorni**, con durate gamma
     (B 12-24 h, C 6-24 h, D 3-8 h, E 12-48 h; STIMA).
   - **Gibilterra:** Levante, Poniente, Vendaval, calma, *cut-off* (che dura 7-10 giorni).
   - **Otranto:** bora chiara e bora scura, scirocco, maestrale ed etesio, calma estiva, temporale autunnale.
   - **Curili:** monsone da NW, ciclone esplosivo (prima burrasca da SE, poi da NW), flusso da S con nebbia, calma
     autunnale.
2. **Vento:** media e direzione dal regime, più un'anomalia AR(1) con τ per teatro e stagione (A §6.2), più
   quantile mapping sulla Weibull (A §6.4).
3. **Onde:** mare da vento che rilassa verso Hs_eq col fetch del teatro (Baltico 50-250 km, Otranto N-S 150-800 km,
   Atlantico illimitato), più swell come processo separato.
4. **Nubi e precipitazione:** catena asciutto/bagnato per regime; intensità gamma; neve sotto 1 °C.
5. **Temperature:** dalle tabelle, più l'anomalia di regime (fronte freddo d'inverno da −2 a −6 °C).
6. **Visibilità:** σ totale (§2), più la foschia di fondo per massa d'aria:
   - polare marittima: 30-50 km;
   - Mediterraneo estivo: 10-20 km;
   - polvere con lo scirocco: 2-8 km.
7. **Ghiaccio, spruzzo gelante, eventi rari:** gli eventi rari sono processi di Poisson con i tassi del §3.3, che
   sovrascrivono il regime per la loro durata.
8. **Modulazione nel riquadro** (A §7.6):
   - ridosso entro 10-20 nm sottovento: vento ×0,6-0,8;
   - stretti (Gibilterra, Kunashir, Belt): vento ×1,3-1,6 lungo l'asse;
   - bordo del ghiaccio: più freddo, con fumo di mare, onde smorzate;
   - fronte termico Islanda-Faroe: nebbia sul lato freddo.

**Taratura:** cento anni sintetici per teatro da confrontare con A. La tolleranza è ±20%:
- frequenze mensili di Bft 7 e 8 e di Hs ≥ 4 m;
- classi Douglas;
- durate degli episodi;
- autocorrelazioni;
- matrici di Markov a 6 h (A §6.3).

**Per le missioni:**
- l'editor e la missione rapida possono **fissare il meteo** (o un regime) oppure lasciarlo al generatore;
- le missioni del 1989 riconvertite ricevono teatro e stagione, e quindi il loro meteo;
- un'opzione di realismo «meteo: realistico / semplificato (mare 3, sereno) / fisso» resta disponibile.

---

## 5. Previsione, radio e pre-briefing

### 5.1 Quanto valeva una previsione nel 1989 (A §8)

- ECMWF: previsione utile fino a circa il **giorno 5** nei primi anni '80, poi circa un giorno guadagnato per
  decennio. Met Office, 2017: «la previsione a 4 giorni di oggi vale quella a un giorno di 30 anni fa» (alta).
- US Navy: NOGAPS operativo dal 1982 alla FNOC di Monterey; previsioni dello strato rimescolato dal 1981 (media).
- **Modello d'errore** [GIOCO]: la previsione è lo stesso generatore con una perturbazione che cresce col tempo.

| Orizzonte | Errore sull'ora dei fronti | Vento (σ) | Hs (σ) | Note |
|---|---|---|---|---|
| 12 h | ±2 h | 1,5 m/s | 0,3 m | |
| 24 h | ±4 h | 2,5 m/s | 0,5 m | |
| 48 h | ±10 h | 3,5 m/s | — | |
| 72 h | ±18 h | 4,5 m/s | — | il 20-30% delle burrasche arriva 6-12 h dopo il previsto, o con una forza in meno |

- **Mai nella previsione:** polar low, getti degli stretti, singoli temporali, trombe marine. Al massimo «possibili
  rovesci di neve con raffiche». La nebbia compare solo come «possibili banchi di nebbia».
- Il giocatore può fare meglio del bollettino col **barometro** (tendenza su 3 h), col vento, con le nubi e con lo
  swell, come facevano gli ufficiali di rotta.

### 5.2 Come arrivava (D §A1-A2)

- **USA:** la FNOC faceva i modelli; i Fleet Weather Central (Norfolk, Rota, Pearl Harbor, Guam) facevano i prodotti
  su misura:
  - **OTSR**, la rotta ottima con le deviazioni consigliate;
  - **WEAX**, la previsione lungo la rotta;
  - avvisi *small craft* (18-33 kn), *gale* (34-47 kn), *storm*;
  - condizioni di prontezza per i cicloni (COR 5-1).
  - I formati d'aviazione erano SA e FT: METAR e TAF entrano negli USA solo nel 1996.
- **Ai sottomarini:**
  - il broadcast VLF/LF va solo da terra al battello, ed è copiato a orari fissi a quota periscopica o col filo;
  - che contenesse il meteo è **n.v.**, ma è plausibile per gli avvisi e per le carte del ghiaccio;
  - il servizio dedicato SUBWEAX è del **2008**, fuori epoca.
- **Chiedere una previsione trasmettendo:** in EMCON **no** (alta come principio). È accettabile solo:
  - in trasferimento in superficie;
  - in esercitazione;
  - accodata a una trasmissione obbligatoria (con il satellitare SSIXS);
  - in emergenza.
- **URSS:** idrometeorologia della flotta presso lo stato maggiore (struttura del Nord in guerra; n.v. per gli anni
  '80), sedute di collegamento programmate.
  - I nomi da usare nel gioco: «метеосводка», «штормовое предупреждение», «прогноз ГАУ» (previsione della portata di
    scoperta).

### 5.3 Proposta [GIOCO]

- **Pre-briefing** (stile F-19 e F-117A, un evento per volta come nella memoria del progetto): un pannello **METEO E
  MARE** su una schermata, con sette righe:
  - vento;
  - mare e swell;
  - visibilità e precipitazione;
  - plafond;
  - ghiaccio e spruzzo gelante;
  - astronomia (alba, tramonto, luna in %, crepuscoli);
  - **acustica** (strato, rumore ambiente, portate previste in barrette).

  Sotto, la **tendenza**: per esempio «PEGGIORA 0300Z», con una freccia. Un messaggio in stile WEAX d'epoca, inventato
  e marcato come tale (D §A3), fa da cartolina. Per l'Alfa le stesse voci, con i nomi russi.
- **In missione:**
  - alle finestre di copia del broadcast (dossier 10, sala radio) arrivano gli aggiornamenti WEAX e il GALE
    WARNING, solo se l'evento è probabile; **chi non ascolta li perde**;
  - in superficie, o fuori dall'EMCON, si può **chiedere una previsione** pagando l'esposizione della trasmissione
    (dossier 08/10).
- **Sulla carta:** gli strati meteo alla Harpoon II, cioè tacche di vento e stato del mare, celle temporalesche,
  bordo del ghiaccio, banchi di nebbia **noti** (non la verità). Nel debriefing il meteo vero e quello previsto, a
  confronto.
- **Radio sotto il temporale:**
  - l'HF ha +10 dB di rumore e crepitii, quindi messaggi corrotti e ripetizioni;
  - il VLF in immersione resta ricevibile (B §4).

---

## 6. Effetti sui sensori (B, sintesi)

### 6.1 Per stato del mare (B §8.1, riferimento mare 2)

| Effetto | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7+ | Conf. |
|---|---|---|---|---|---|---|---|---|---|
| ΔNL a 0,5-25 kHz (dB, Knudsen) | −14 | −5 | 0 | +4 | +7 | +9 | +11 | +12 | alta [CALC] |
| Perdita delle bolle vicino alla superficie, 1-5 kHz (dB) | 0 | 0 | 0 | 0 | 0 | +2 | +8 | +12 | media |
| Perdita nel condotto superficiale a 3,5 kHz (dB/salto, ½ Schulkin) | 0,5 | 1 | 1,5 | 2,7 | 3,8 | 5 | 6,2 | 8 | media, STIMA |
| R50 radar contro periscopio, impulso normale | 1,3 | 1,15 | 1 | 0,6 | 0,35 | 0 | 0 | 0 | media (dossier 11) |
| R50 radar contro periscopio, APS-116/137 | 1,2 | 1,1 | 1 | 0,8 | 0,6 | 0,4 | 0,2 | 0,1 | media (dossier 11) |
| Vista su periscopio e piuma | 1,3 | 1,3 | 1 | 0,8 | 0,5 | 0,3 | 0,2 | 0,1 | STIMA su IAMSAR |
| Vista sulle navi | 1 | 1 | 1 | 1 | 0,9 | 0,9 | 0,8 | 0,6 | STIMA |
| Rumore proprio del sonar di scafo (dB) | 0 | 0 | 0 | 0 | +3 | +6 | +10 | +15 | STIMA |
| Rumore proprio delle boe sotto 100 Hz (dB) | 0 | 0 | 0 | 0 | +3 | +6 | +10 | guasti | STIMA |
| Portata del MAD | 1 | 1 | 1 | 1 | 0,8 | 0,8 | 0,6 | 0,6 | media (legge della radice cubica) |

- **Il mare d'olio rivela la piuma** a miglia; da mare 4 la piuma si confonde con le creste bianche, ma dura di più
  (US Navy 1917, alta). La scia di un siluro sparisce da mare 4-5.
- **Lo strato rimescolato si approfondisce col vento** (B §1.5): a bassa frequenza la portata nel condotto può
  persino **allungarsi**.

### 6.2 Pioggia, nebbia, notte, temporali

- **Pioggia** (B §8.2):
  - a 5 kHz il rumore va da circa 45 dB (pioviggine, con un picco a 15 kHz) a 70-80 dB (temporale);
  - il radar X perde fino a 3,3 dB/km sopra 50 mm/h, e l'eco della pioggia nasconde i piccoli bersagli;
  - la visibilità va da 16 km (1 mm/h) a meno di 1 km (> 50 mm/h).
- **Classi di visibilità** (B §3.1): moltiplicatori della portata per occhio e periscopio, intensificatore e termico.
  - Nella nebbia di radiazione a 610 m il termico LWIR vede **circa 4 volte** l'occhio.
  - Sotto i 300 m non c'è differenza.
  - Con la **nebbia d'avvezione marina** e con la pioggia il termico vale come l'occhio.
  - Lo usano il CH84 della Trafalgar (dossier 20) e i FLIR di S-3 e P-3.
- **Notte** (B §3.3):

  | Luce | Occhio | Intensificatore |
  |---|---|---|
  | Luna piena e serena | ×0,3 | ×0,7 |
  | Stelle | ×0,05 | ×0,3 |
  | Coperto senza luna | ×0,02 | ×0,1 |

  La luna bassa dietro il bersaglio raddoppia la portata sulla sagoma.
- **Temporali** (B §7, D B-bis):
  - celle visibili al radar fino a circa 20 nm, con l'ombra dietro;
  - i fulmini in mare si sentono al sonar fino a 30-40 km come transitori, che sono anche falsi allarmi (esistenza
    alta, livelli n.v.);
  - il tuono si sente in superficie entro circa 10 miglia, il lampo si vede a circa 100;
  - gli aerei girano al largo delle celle (10-20 nm).
  - Che il lampo notturno aiuti a vedere una sagoma **non ha prove**. Se lo si usa, è una licenza dichiarata: un
    tiro di avvistamento solo sulle navi, portata ×0,3, solo entro 15 km (STIMA).
- **Fumo di mare:** uno strato basso di 5-50 m. La testa del periscopio può esserne dentro, oppure uscirne sopra,
  con gli alberi delle navi visibili e gli scafi nascosti.

---

## 7. Effetti sui mezzi (C, sintesi)

Notazione: N normale, D degradato, X impossibile. In **grassetto** i dati documentati; il resto è STIMA calibrata su
quelli (C §8 e §12).

### 7.1 Matrice per stato del mare (C §8)

| Mezzo | 3 | 4 | 5 | 6 | 7+ | Ghiaccio moderato | Temporale vicino | Nebbia |
|---|---|---|---|---|---|---|---|---|
| SH-60B (FFG/DD con RAST) | N | N | D (**limite di progetto RAST**) | D/X (moto nave **±8°/±3°**) | X | D | X | D |
| SH-60F / SH-3H col sonar a immersione | N | D | D/X (X con **onde oltre 10 ft**, bassa) | X | X | X | X | D |
| Ka-25 / Ka-27 | N | N | D (**Ka-27 fino a 5 баллов**) | X | X | D (pale riscaldate) | X | D |
| P-3C / Il-38 / Tu-142: boe e MAD | N | N | N | D (**SSQ-53 fino a SS6**) | D/X | D | X localmente | D (vista X) |
| S-3A/B dalla portaerei | N | N | N | D (ponte che beccheggia) | X | D | X | D, CASE III |
| Be-12 in acqua | D (**3 баллa**) | X | X | X | X | — | X | D |
| Fregata da 130 m: elicottero | N | N | D (**operabilità circa 33%**) | X quasi sempre | X | D | D | N |
| Fregata da 130 m: sonar di scafo | N | D | D | D | X ad alta velocità | N | N | N |
| Nanuchka: armi | N | N | D (**fino a 5 баллов**) | X | X, cerca ridosso | D/X stabilità | N | N |
| 688 a quota periscopica | N | D | D | X consigliato | X | — | N | periscopio X |
| Kilo/Foxtrot allo snorkel | N | D | D | X | X | — | N | N |
| Missili radenti | N | N | D (quota più alta) | D | D/X | N | N | N |

**Elicotteri: appontaggio** (C §2.2).
- Ogni ciclo di avvicinamento (2-3 min) estrae rollio e beccheggio dal moto della nave. Se restano entro i limiti per
  10-15 s («quiescent period»), l'appontaggio riesce; altrimenti c'è il wave-off.
- I limiti sono FFG ±8°/±3°; Ka-25 10°/3° e 18 m/s.
- Senza RAST (le navi sovietiche) il rischio d'incidente cresce verso il limite.
- Il giocatore che vuole riprendere l'elicottero deve **mettere il mare al giardinetto e rallentare**.
- **Temporale:** niente volo entro 10 nm dalle celle; sonar a immersione e verricello sospesi con fulmini entro 5 nm.

**Aerei:** minimi CASE I/II/III per la portaerei. Con plafond sotto 300 ft di notte niente passaggi MAD. Lo spruzzo
gelante degrada in quota bassa. Col vento oltre 30 kn le boe derivano e il campo si sposta.

### 7.2 Navi: moto e velocità (C §12.4, §4.2)

**Rollio e beccheggio RMS** (gradi):
- calibrati sul dato **DD-963 in SS5: rollio 4,56° RMS** (MIT per la NASA, alta);
- ampiezza significativa = 2 × RMS; massimo orario = 3,5 × RMS;
- criterio **STANAG 4154: rollio 4°, beccheggio 1,5° RMS** (alta).

| WMO | Nanuchka 700 t | Fregata 3600 t | Incrociatore 8000 t | Kirov 24 000 t | Portaerei 80 000 t |
|---|---|---|---|---|---|
| 3 | 2,5 / 1,5 | 1,0 / 0,5 | 0,8 / 0,4 | 0,5 / 0,2 | 0,2 / 0,1 |
| 5 | 7 / 4 | 5 / 2,0 | **4,5** / 1,5 | 2,5 / 0,9 | 1,0 / 0,6 |
| 7 | 14 / 7 | 10 / 4,0 | 8 / 3,2 | 6 / 2,3 | 3,0 / 1,6 |

**Velocità sostenibile col mare di prua** (kn):

| Classe | SS4 | SS5 | SS6 | SS7 | SS8 |
|---|---|---|---|---|---|
| Nanuchka | 28 | 18 | 12 | 8 | 5 |
| Fregata | 29 | 25 | 18 | 12 | 8 |
| Spruance/Kara | 32 | 28 | 22 | 15 | 10 |
| Kirov | 31 | 30 | 26 | 20 | 14 |
| Portaerei | 30 | 30 | 28 | 24 | 18 |

- Oltre questi limiti c'è lo slamming: il sonar di prua va fuori uso e ci sono danni in coperta (2% al minuto per
  nodo oltre il limite, STIMA).
- I mercantili usano la formula di Kwon (C §4.1), che non va applicata alle navi da guerra veloci.

**Sonar di scafo** (brevetto US4333169, media): «virtualmente inutilizzabile» oltre 18 kn col mare grosso.
- Con mare 6 c'è il «quenching» quando la prua emerge (10-30% del tempo, STIMA).
- Il rimorchiato regge fino a SS5-6; il VDS non si recupera da SS6.

**Navi piccole:** la Nanuchka da SS6 cerca ridosso o mette il mare di prua. È una **finestra** in cui la minaccia
missilistica costiera sparisce. Con lo spruzzo gelante forte le navi sotto i 100 m perdono stabilità.

### 7.3 Sottomarini (C §5, §12.2-12.3; B §6)

- **Quota periscopica.**
  - Il risucchio verso la superficie cresce esponenzialmente avvicinandosi ad essa e a bassa velocità, e spesso
    limita l'inviluppo (alta, NPS e DST).
  - Serve velocità per avere autorità sui timoni, ma oltre circa 5 kn la piuma si vede.
  - Col mare in poppa il battello è il più difficile da governare (rapporti di guerra, media).
- **Testa sott'acqua** (riconciliazione: §8, punto 8):
  - frazione di tempo = Q(4h/Hs) calcolata sull'altezza vera della testa (B §6.1, [CALC]);
  - più l'**errore di quota** σ della tabella sotto, che abbassa e alza la testa.

| WMO | σ quota (688) | Affioramento della vela (v < 4 kn o mare al traverso) | Quota sotto cui è «calmo» | Note |
|---|---|---|---|---|
| ≤ 3 | 0,3-1 m | 0 | 20-40 m | piuma visibilissima oltre 3 kn |
| 4 | 1,5 m | 0,2% al minuto | 60 m | ESM e vista a scatti |
| 5 | 2-3 m | 1% al minuto | 80 m | si tiene ≥ 6 kn; antenna a intermittenza |
| 6 | 3-5 m | 5% al minuto | 120 m | il comandante scende a 25-30 m; il radar nemico vede la vela se affiora |
| ≥ 7 | X | oltre 10% al minuto | 150-200 m | solo boa o filo |

- **Fattore di taglia** (C §12.2):
  - l'Alfa (1,3) e il Foxtrot (1,4) ballano di più;
  - Oscar (0,7) e Typhoon (0,6) sono più stabili col mare corto, ma subiscono di più il mare lungo (1,3-1,4).
- **Moto in profondità:** l'ampiezza è ∝ Hs·e^(−2πz/λ). Sotto λ/2 resta meno del 4%. Quota di fuga tipica: SS5 circa
  75 m, SS6 circa 120 m, SS7 circa 175 m.
- **Snorkel** (Kilo, Foxtrot):

  | SS | Ricarica | Note |
  |---|---|---|
  | ≤ 3 | 100% | |
  | 4 | 70% | |
  | 5 | 40% | la valvola di testa si chiude di continuo; depressione a bordo, timpani |
  | ≥ 6 | non si ricarica | **finestra per l'ASW** |

- **In superficie** (C §12.3):
  - rollio del 688 ±10-15° a mare 4 e ±25-35° a mare 6 (Foxtrot e Kilo ×1,3, Typhoon e Oscar ×0,6);
  - la plancia si sgombra da mare 6;
  - l'allagamento da un portello aperto cresce col mare (lezioni di K-8 e K-278: **zattere inutilizzabili, acqua a
    2 °C, 30 morti in acqua**).
- **Sotto il ghiaccio:**
  - solo il 688I (dal 1988) è attrezzato per l'emersione nel ghiaccio (record: **155 cm**); i 688 Flight I emergono
    nelle polinie;
  - le chiglie del pack misurano in media **4,5 m**, fino a 45 m;
  - quota di sicurezza 60 m nel Barents, 45-50 m nell'Okhotsk (STIMA).

### 7.4 Armi, mine, danni e soccorso (C §6, §12.5-12.7)

- **Siluri:**
  - tetto minimo automatico 10 m + Hs;
  - da SS5 rischio di affioramento (2-5%) e homing ×0,9-0,8 contro bersagli in superficie;
  - per i siluri lanciati da aereo, guasto all'ingresso in acqua del 3% fino a SS4, 10% a SS6, 25% a SS7.
- **Missili radenti:** quota di crociera = max(base, 1,5 Hs + 2 m). Il radar del bersaglio li vede 1-2 nm prima con
  mare 6; il seeker soffre il clutter da SS5.
- **Missili da sottomarino:**
  - Sub-Harpoon: tenere la regola del dossier 05, +10% di guasto per ogni grado oltre 5;
  - P-700 Granit lanciabile in immersione fino a WMO 6 da 30-40 m (STIMA, profondità limite n.v.).
- **Mine ormeggiate:** la mina beccheggia e la sua quota effettiva varia. Da SS6 l'ormeggio si rompe (0,5-5% al
  giorno) e dopo la tempesta compaiono **mine alla deriva**. La caccia alle mine è impossibile da SS4.
- **Danni e soccorso:**
  - l'allagamento di una nave avariata cresce da +10% a mare 4 fino a ×3 a mare 8-9;
  - abbandono nave: 10% di zattere perse a mare 5, 30% a mare 6, impossibile a mare 7;
  - sopravvivenza a 5 °C: 1,9 h senza protezione, 4,8 con la tuta anti-esposizione, 12,6 con la muta stagna (DTIC,
    alta); ×0,8 col mare grosso.

---

## 8. Soglie già nel progetto: raccolta e correzioni

| # | Dove | Oggi | Proposta | Perché |
|---|---|---|---|---|
| 1 | `missione.cpp` | `STATO_MARE = 3` costante | lo stato del mare per passo dal meteo (Hs, Douglas) | C, A |
| 2 | `acustica` e `periscopi` | due tabelle Douglas: `altezza_onda_m(ss)` e `altezza_significativa_m(ss)` | una sola, in `nucleo/meteo`; il rumore resta legato al vento (Knudsen), le onde a Hs | coerenza |
| 3 | `acustica::stato_del_mare(vento)` | istantaneo dal vento | lo stato del mare viene da Hs, col ritardo del mare (§2) | A §3.2 |
| 4 | Boe USA (`asw_aereo.toml`, SSQ-53B) | `mare_max` 9 | 6 (SSQ-53D fino a SS6; nessun dato per la B del 1989), rumore proprio come nel §6.1 | C §3.1, B §8.1 |
| 5 | Boe sovietiche | RGB-NM fino a mare 3; RGB-75/25 inefficienti 3-5 | confermate (decisione VPK 1987) | dossier 17 |
| 6 | Sonar a immersione | `mare_max` 4, vento 35 kn | USA (AQS-13): N fino a 3, D a 4, D/X a 5 (X con onde oltre 3 m), X da 6. Ka-27: D fino a 5, X da 6. Vento X oltre 35 kn in entrambi | C §2.2, §12.5 |
| 7 | MAD (dossier 17 §6.3) | portata ×0,6-0,3 col mare grosso | ×0,8 a SS4-5 e ×0,6 da SS6, per la legge della radice cubica | B §5 |
| 8 | Testa del periscopio sott'acqua | C dava 20-30% (mare 4) e 40-50% (mare 5), STIMA | il calcolo gaussiano Q(4h/Hs) sull'altezza vera, più l'errore di quota del §7.3: con 1,5 m esposti circa 1-3% a mare 4 e 7-15% a mare 5, di più a bassa velocità e col mare in poppa. Il runtime ha già `finestra_coperta` con `onde.hpp`: basta aggiungere l'errore di quota | B §6 [CALC]; P1 |
| 9 | Radar contro periscopio (dossier 11 §6.4) | già per stato del mare | moltiplicatori del §6.1, allineati al dossier 11; aggiungere la pioggia (§6.2) | B §2 |
| 10 | Periscopio nemico che vede il giocatore (P1) | portata con mare e notte | i moltiplicatori di vista del §6.1 e §6.2, più la classe di visibilità | B §3 |
| 11 | Dossier 20 §8.1 | velocità del periscopio senza il mare | il tremolio si somma al moto del mare (σ di quota) | §7.3 |

---

## 9. Grafica e animazioni (D §B, §B6)

**Già fatto** (docs/14, P3, solo Enhanced):
- il **mare mosso** sotto l'orizzonte: onde lunghe PM e corte del vento, ferme nel mondo, facce verso l'occhio, schiuma
  di Monahan, foschia;
- l'**acqua sulla finestra**: il fianco dell'onda che sale, la finestra sommersa, il velo che scola, le gocce, gli
  spruzzi;
- le gocce di **pioggia** sulla finestra;
- l'occhio che sale e scende con l'onda.
- **creste e schiuma per stato del mare** (kit, 26/10/2026): il mare vicino come campo d'altezze, le creste che coprono
  l'orizzonte e le navi, la schiuma per forza Beaufort (riga della tabella qui sotto).

**Da fare** [GIOCO], con le ricette in 256 colori di D §B2:

| Elemento | Resa |
|---|---|
| **Cielo e nubi** | Rampa per copertura in ottavi e per plafond; base scura del cumulonembo, cortine di pioggia e virga come colonne retinate; il mare sotto la cella più cupo |
| **Foschia e nebbia** | Tabella di sfumatura per distanza (8-16 livelli) verso il colore della foschia; con la nebbia d'avvezione cielo e mare in una sola rampa e navi che compaiono come forme piatte a 0,5-1 km |
| **Fumo di mare** | Il contrario della nebbia: una fascia bassa a 5-15 righe dall'orizzonte con colonne retinate che salgono; cielo nitido sopra; scafi nascosti e alberi visibili |
| **Pioggia e neve** | Strisce diagonali di 1 px su 2-3 grigi che scorrono; fiocchi di 1-2 px lenti con deriva; contrasto ridotto |
| **Lampi e fulmini** | Lampo come schiarita della palette (la stessa del lampo delle esplosioni): 1-2 fotogrammi per colpo, 3-4 colpi a circa 50 ms, ogni colpo al 60-80% del precedente. Il canale come spezzata di 1-2 px con rami, da 6-10 varianti, solo entro circa 15 km. Oltre circa 20 km solo un bagliore dietro le nubi, senza tuono. Di notte drammatico (cielo e mare al bianco-azzurro, creste «congelate», navi in sagoma); di giorno al 20-30%. Tuono ritardato di 3 s/km. Con le gocce sulla finestra, ogni goccia brilla |
| **Creste e schiuma per stato del mare** (fatto) | Tabella per animatori di D §B6.2 (per Beaufort: creste sparse da 3, strisce di schiuma da 7, spruzzaglia da 8, mare bianco da 10), con la foto Beaufort NWS di pubblico dominio per ogni riga |
| **Moto delle navi** | Rollio, beccheggio e sussulto col **periodo d'incontro** Te = T / (1 + V cos μ / c) (alta), ampiezze del §7.2; la nave sparisce dietro la cresta vicina; prua che si pianta e spruzzo a prua (sprite di 3-4 fotogrammi) da SS5 sulle navi medie e piccole; eliche fuori sulle navi sotto 80 m |
| **Elicotteri in hover** | Muso al vento, leggermente basso; oscillazioni con le raffiche; cavo del sonar inclinato sottovento di 5-15°; anello di spruzzo sotto il rotore spostato sottovento (foto PD dell'AQS-13 in D §B6.4) |
| **Ghiaccio sulle sovrastrutture** | Variante bianca della palette dei modelli con lo spruzzo gelante |
| **Carta** | Strati meteo come tacche e colori (vento, mare, celle, ghiaccio, nebbia nota) |

**Palette.**
- Il meteo cambia **la palette e le tabelle di sfumatura**, non i pixel (D §B2). È la tecnica più economica e la più
  fedele all'epoca.
- Gli indici liberi vanno coordinati con quelli già usati: la rampa 160-174 dei modi monocromi dell'oculare, i 200-231
  della carta e del sonar Enhanced. Proposta: un blocco «lampo» e un blocco «foschia» da assegnare in P3/M3.

**Immagini di riferimento:** D §B5 elenca una trentina di file Wikimedia Commons con licenza, consultate e non
scaricate:
- la serie Beaufort NWS (pubblico dominio; le foto da 0 a 4 sono dei Grandi Laghi);
- il sonar a immersione, navi col mare grosso, fumo di mare, temporali in mare.

Nessuna vista dal periscopio con l'acqua sulla lente è su Commons.

---

## 10. Proposta di gioco [GIOCO], in breve

1. **Un meteo per missione** dal generatore del teatro, della stagione e dell'ora, oppure fissato dall'editor o dalla
   missione rapida. Cambia lentamente: un fronte passa in 6-12 h di gioco.
2. **Pre-briefing con il pannello METEO E MARE** e la previsione che può sbagliare; **aggiornamenti radio** solo nelle
   finestre di copia; richiesta di previsione col costo dell'esposizione.
3. **Effetti leggibili, alla Harpoon II, ma fisici sotto:**
   - il mare alza il rumore e nasconde la piuma;
   - la pioggia e la nebbia tagliano la vista e il radar;
   - i temporali disturbano HF e sonar.
   - Gli elicotteri si fermano per primi, poi gli aerei bassi e i sonar delle navi.
   - Il sottomarino guadagna copertura ma perde la quota periscopica.
4. **Eventi estremi rari ma possibili**, con la loro stagione: polar low senza preavviso nel Barents, bora improvvisa a
   Otranto, nebbie di giorni alle Curili, spruzzo gelante a Kunashir, temporali estivi nel Baltico.
5. **La grafica racconta il meteo:** mare per Beaufort, acqua sulla lente, cielo per nubi, nebbia e fumo di mare,
   lampi e fulmini, navi che rollano e piantano la prua, elicotteri che lottano col vento.
6. **Opzione di realismo:** meteo realistico, semplificato o fisso.

---

## 11. Contratto proposto: `nucleo/meteo`

Puro e deterministico, come `nucleo/periscopi`. Vale solo con la regola di simulazione.

```cpp
enum class Regime : std::uint8_t { anticiclonico, prefrontale, settore_caldo, fronte_freddo, postfrontale, bloccato,
                                   levante, poniente, vendaval, bora_chiara, bora_scura, scirocco, etesio,
                                   monsone, ciclone_esplosivo, flusso_nebbioso, calma };
enum class Nebbia : std::uint8_t { nessuna, avvezione, fumo_di_mare, radiazione, precipitazione };

struct StatoMeteo {
    Regime regime = Regime::anticiclonico;
    double vento_m_s = 5.0, vento_da_gradi = 270.0, raffica_m_s = 7.0;
    double hs_vento_m = 0.5, tp_vento_s = 4.0;                    // mare da vento
    double hs_swell_m = 0.0, tp_swell_s = 0.0, swell_da_gradi = 270.0;
    double nubi_ottavi = 4.0, plafond_m = 1500.0;
    double pioggia_mm_h = 0.0, neve_mm_h = 0.0;
    double visibilita_m = 30000.0;                                // Koschmieder, contrasto 0,02
    Nebbia nebbia = Nebbia::nessuna; double strato_nebbia_m = 0.0;  // spessore (fumo di mare)
    double lampi_al_minuto = 0.0;                                 // 0: niente temporale
    double t_aria_c = 10.0, t_mare_c = 10.0;
    double pressione_hpa = 1013.0, tendenza_hpa_3h = 0.0;
    double ghiaccio = 0.0;                                        // frazione nel punto
    double spruzzo_gelante_cm_h = 0.0;                            // Overland
    double hs_m() const;                                          // sqrt(vento^2 + swell^2)
    int stato_mare() const;                                       // Douglas da hs_m()
    double schiuma() const;                                       // Monahan
};

struct Lampo { Vec2 pos; double t = 0.0; int colpi = 1; bool nube_mare = false; };   // intervalli ~57 ms

class Meteo {
public:
    Meteo(const DatiMeteo& dati, const std::string& teatro, int mese, double ora_locale, std::uint64_t seme);
    void avanza(double t);                                        // passo orario interno, interpolato
    const StatoMeteo& stato() const;                              // nel centro del riquadro
    StatoMeteo in(Vec2 pos) const;                                // con ridosso, stretti, ghiaccio, costa
    std::vector<Lampo> lampi(double t0, double t1) const;         // deterministici nell'intervallo
    StatoMeteo previsione(double emessa_t, double valida_t) const;  // con l'errore del 1989 (§5.1)
    void fissa(const StatoMeteo& s);                              // editor, missione rapida, opzione «fisso»
};
DatiMeteo carica_meteo(const std::string& percorso);              // data/motore/meteo.toml
```

**Dati:** `tools/pipeline/meteo.py` scrive `data/motore/meteo.toml` dalle tabelle di A. Per teatro e mese:
- quantili e Weibull del vento, rosa a 8 settori;
- Hs e periodo;
- temperature, nubi, precipitazione e neve;
- frequenze dei regimi e durate, τ dell'AR(1);
- tassi degli eventi rari, bordo del ghiaccio, fetch per direzione;
- ciclo diurno e fattore costa.

Ogni valore porta fonte, confidenza e stime, come `periscopi.toml`.

### 11.1 Pezzi e chi li fa (da confermare con la sessione principale)

| Pezzo | Contenuto | Chi (proposta) |
|---|---|---|
| **M0 Dati** | `meteo.py` → `meteo.toml`; gli script ERA5 in `docs/ricerca/21_note/era5/` estesi a 3-5 punti per teatro | kit |
| **M1 Modello** | `nucleo/meteo.{hpp,cpp}`: generatore, eventi rari, lampi, previsione, `in(pos)`; prove con cento anni sintetici contro le tabelle di A (±20%) | runtime |
| **M2 Effetti** | Correzioni del §8 e:<br>• lo stato del mare per passo al posto di `STATO_MARE`;<br>• `Ambiente` acustico dal meteo (vento, pioggia, ghiaccio) e transitori dei fulmini;<br>• radar con mare e pioggia;<br>• vista con visibilità, notte, termico;<br>• ASW aereo (sonar a immersione, boe, MAD, celle);<br>• navi (velocità, sonar, armi, elicotteri e appontaggio);<br>• sottomarino (errore di quota, affioramento, snorkel, superficie, ghiaccio);<br>• armi e mine;<br>• danni e soccorso | runtime |
| **M3 Grafica** | Vista Enhanced: cielo e nubi, foschia e nebbia, fumo di mare, pioggia e neve, lampi e fulmini, moto delle navi col periodo d'incontro, spruzzi a prua, elicotteri in hover; carta con gli strati meteo. L'oculare ha già mare, onde sulla finestra e pioggia | kit |
| **M4 Briefing e radio** | Pannello METEO E MARE nel pre-briefing; WEAX e GALE WARNING sul broadcast; richiesta di previsione con costo; nomi sovietici; meteo nel debriefing; scelta nell'editor e nella missione rapida | principale (briefing), runtime (sala radio) |
| **M5 Audio** | Pioggia e vento sul rumore ambiente (già nel modello acustico), tuono in superficie, fulmini al sonar, scariche in sala radio | sessione dell'audio |
| **M6 Taratura** | Missioni di prova per teatro e stagione; equilibrio fra copertura del maltempo e perdita della quota periscopica | runtime col principale |

**Verifiche:** il classico resta **IDENTICO**. Nell'Enhanced, con il meteo «fisso a mare 3 e sereno», le missioni di
prova devono restare vicine a quelle di oggi.

---

## 12. Lacune principali (dettagli nelle note)

- **Visibilità da osservazioni** (ICOADS) per Faroe, Barents, Otranto e Celtica; giorni di temporale per stazione;
  densità di fulmini da satellite (LIS/OTD) per teatro (A §9).
- **Fulmini sott'acqua:** livello, spettro, durata (Arnold 1984 e Hill 1985 a pagamento) (B §10).
- **Manuali di volo** (NATOPS, SHOL, schede di limite sovietiche): quasi tutto n.v.
  - Sette rapporti DTIC da recuperare (C §11): l'archivio era in manutenzione.
  - Le soglie dello snorkel sovietico, dei sonar a immersione d'epoca e delle boe SSQ-41/47/53B col mare.
- **Meteo nel broadcast ai sottomarini** negli anni '80, USA e URSS (D §A2).
- **Da verificare sulle fonti originali:** le soglie di Overland, i coefficienti di Kwon, la tempesta del 25 gennaio
  1990, i fattori IAMSAR.

---

## 13. Fonti

Le fonti con URL e confidenza stanno nelle quattro note: A §10, B §9, C (in linea e §11), D §F. Le principali:
- ERA5 (Hersbach et al. 2020) via Open-Meteo;
- *Marine Climatic Atlas* e atlanti US Navy (Mediterraneo, Europa, Nord Pacifico, ver. 1.1 1995);
- Smirnova et al. 2015 (polar low);
- Romero ed Emanuel 2013 (medicane);
- Enno e Sugier 2018 (fulmini in Europa);
- Monahan e O'Muircheartaigh 1980;
- Pierson-Moskowitz 1964 e il Coastal Engineering Manual;
- WMO-No. 8 e 306;
- Overland 1990;
- STANAG 4154;
- Triantafyllou e Athans 1982 (DD-963);
- NPS e DST sulla quota periscopica;
- OEG Report 51;
- ITU-R P.838 e P.1814;
- *Aerographer's Mate* 1976;
- i manuali di Harpoon II, Gunship, F-19, Red Storm Rising e 688 Attack Sub.
