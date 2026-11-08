# 30 — Sonar: la grafica delle cascate (SDR moderni e consolle del 1989)

Progetto: remake di *688 Attack Sub* (1989). Scopo: base **visiva** per ridisegnare la stazione Sonar (F8) nei due
modi, «1989» (fosforo verde, manuale) e «moderno» (colore, aiuti). La stazione funziona già: qui si parla solo di
aspetto e leggibilità.

Convenzioni:
- I numeri tra parentesi quadre rimandano alle fonti del §5. Le sigle di `09` (per es. [SDRsharp], [Kraiss84]) e di
  `19` (per es. [FI-BQQ5]) valgono anche qui.
- **[alta]** = documentazione o codice del programma, rapporto o articolo primario letto. **[media]** = fonte
  secondaria credibile o estratto di ricerca. **[bassa]** = forum, ricordi, estratti non verificati.
- **STIMA** = deduzione dell'autore o scelta di gioco, senza fonte diretta.
- Nessuna immagine è stata scaricata o copiata: le foto sono descritte dalle loro schede.

---

## 0. Cosa c'era già e cosa aggiunge questo dossier

- **Già in `09`:** la mappatura funzione SDR → funzione sonar (FFT, finestra, media, picco, zoom, segnalibri, §4.1),
  le regole di resa (pipeline, pavimento/tetto, scala dei tempi, §4.2), la disposizione di massima (§4.3), e gli
  studi sul colore in sintesi (TRACOR 1979, Kraiss 1984, NUSC/Salafia, §2.6).
- **Già in `19`:** le consolle OJ del BQQ‑5, la luce blu della sala, i fosfori JEDEC, i caratteri MIL‑STD‑1472
  (§2.3, §6.6, §9).
- **Aggiunge:** come **appaiono** davvero gli SDR (mappe di colore con i valori, colori di griglia, banda passante,
  segnalibri, picchi, presi dal codice sorgente quando possibile); cosa si sa dell'**aspetto** delle consolle del
  1989; gli **studi di leggibilità** della Marina USA (NSMRL) e australiani (DSTO) sulle cascate; una **proposta
  concreta** per 640×480 a 256 colori indicizzati, elemento per elemento.

---

## 1. Programmi SDR moderni

### 1.1 SDR# (SDRSharp, Airspy)
- **Disposizione:** spettro RF sopra, cascata sotto; i dati nuovi entrano in alto e scorrono in basso [4].
- **Cursori laterali** a destra: *Zoom*, *Contrast*, *Range* (predefinito −70), *Offset* (predefinito 0). Consiglio
  della guida: mettere il **fondo del rumore vicino al bordo basso** dello spettro [4]. [alta]
- **Sintonia:** **linea verticale rossa** sulla frequenza; attorno, un **rettangolo ombreggiato** largo quanto la
  banda, con i bordi trascinabili [4]. [alta]
- **Opzioni:** *Gradient* (editor della mappa di colore), *Time Markers* (tacche orarie sulla cascata, spente di
  base), *Mark Peaks* (un **cerchietto su ogni picco**), *S‑Attack/S‑Decay* per lo spettro e *W‑Attack/W‑Decay* per
  la cascata, separati [4]. [alta]
- **Zoom:** ingrandisce sulla banda sintonizzata; la guida avverte che **abbassa la risoluzione apparente** [4].
- **Segnalibri:** il *Frequency Manager* con «Show on spectrum» disegna le frequenze salvate sullo spettro [4].
- **Band plan:** file XML con bande colorate e nominate; il colore è un esadecimale a **8 cifre con trasparenza**
  (es. `50AAAAAA`) [7] [8]. Le bande appaiono come **striscioni colorati con il nome**; i canali alternano due
  colori simili per separarli [7]. [media]
- **Mappa predefinita:** la mappa «Classic» di SDR++ porta come autore Youssef Touil, l'autore di SDR#; è quindi con
  ogni probabilità il gradiente storico di SDR# [6]. Valori: `#000020 → #000030 → #000050 → #000091 → #1E90FF →
  #FFFFFF → #FFFF00 → #FE6D16 → #FF0000 → #C60000 → #9F0000 → #750000 → #4A0000` [6]. [alta per i valori;
  **dedotto** il legame con SDR#]
  - Lettura: fondo **blu notte**, segnali medi **azzurri → bianchi**, forti **giallo → rosso → rosso scuro**. La
    luminosità sale fino al bianco e poi **ridiscende**: i segnali più forti sono più scuri dei medi. **STIMA**
    (lettura dei valori).

### 1.2 SDR++
- **Menu Display** [5]: *Show Waterfall*, *Full Waterfall Update* (ridisegna la storia quando cambiano zoom o
  min/max), *FFT Hold* con *FFT Hold Speed* (traccia del massimo che decade; nella figura del manuale è
  **arancio/gialla** sopra la traccia **ciano**), *FFT Size* (fino a 65536), *FFT Window* (Rectangular, Blackman,
  Nuttall predefinita dalla 1.1.0), *Color Map* (predefinita «Classic»). [alta]
- **VFO:** una «banda» colorata larga quanto il filtro, su spettro e cascata; colore scelto per contrasto con lo
  sfondo, uno diverso per ogni VFO, oppure *Auto Color* [5]. Nella figura il colore è un ciano chiaro `#80FFFF`. [alta]
- **Band plan:** si sceglie se mostrarlo e se **in alto o in basso** dello spettro; i tipi (es. «amateur») hanno un
  colore ciascuno, definito in `bandColors` [5]. [alta]
- **Temi:** cambiano sfondo, spettro, cascata, testo e menu [5]; nei pacchetti ci sono «army green», «dark», «deep
  blue», «grey», «light» (indice di un pacchetto Fedora, [media]).
- **Mappe incluse** (14 file) [6]: classic, classic_green, electric, gqrx, greyscale, inferno, magma, plasma, smoke,
  temper_colors, turbo, viridis, vivid, websdr. [alta]
  - «WebSDR»: `#000000 → #000050 → #FF00FF → #FFFF50 → #FFFFFF` (nero, blu notte, magenta, giallo pallido, bianco:
    luminosità quasi sempre crescente) [6].
  - «Classic Green»: come Classic ma con il **verde chiaro `#80FF80`** al posto del bianco [6].

### 1.3 GQRX
- **Disposizione:** a sinistra la frequenza, sotto lo spettro («pandapter»), sotto ancora la cascata; un cursore
  **«Pandapter ⇔ WF»** decide quanta altezza va all'uno e all'altra [9]. [alta]
- **Media:** adattiva, più forte sul rumore che sui segnali; agisce **solo sul grafico, non sulla cascata** [9].
- **Picchi:** «D» mette **cerchietti** sui picchi, «H» mostra **debole** l'inviluppo dei massimi [9].
- **Zoom:** trascinando o con la rotella sulla scala; pulsanti R (100%), C (centra), D (centra sul demodulatore) [9].
- **Dal codice** (`plotter.cpp`) [10], [alta]:
  - sfondo `#1F1D1D` (grigio quasi nero); la cascata parte **nera**;
  - griglia `#606060` al 50% di opacità, **a puntini**; testo `#DADADA` con **ombra nera**;
  - banda del filtro: **bianco al 16%** (`0x28FFFFFF`), linea centrale **rosso chiaro** al 69%;
  - linea di centro **azzurro** al 50%; marcatori A‑B **verdi** con riempimento **a tratteggio diagonale**;
  - traccia principale bianca; massimi/minimi in un colore **mescolato con lo sfondo** (più tenue);
  - **segnalibri** in alto: linea verticale **tratteggiata** al 40% di opacità, sottolineatura e testo pieno;
    se due etichette si sovrappongono, la seconda **scende di una riga**; se non c'è riga libera, non si disegna.
- **Mappe:** gqrx (predefinita), turbo, plasma, viridis, whitehot, whitehotcompressed, blackhot [10].
  - La «gqrx» ha 256 voci: **le prime ~20 sono nero puro**, poi blu, azzurro‑verde, giallo, arancio, rosso, e in
    cima rosa → bianco [6]. Il nero iniziale **taglia il rumore di fondo**: lo schermo resta scuro e le righe
    emergono. **STIMA** (lettura dei valori).

### 1.4 HDSDR
- **Nessun manuale ufficiale**; i pulsanti hanno suggerimenti a comparsa [14]. [alta]
- Due coppie spettro + cascata, **RF e audio**, ingrandite alla larghezza della finestra indipendentemente dalla
  risoluzione della FFT [14]. [media]
- Tasto **S** per scambiare cascata e spettro; tasti per la velocità di ciascuna cascata [14]; clic destro
  trascinato sulla scala delle frequenze per cambiare il **rapporto spettro/cascata** [14]; marca oraria sulla cascata
  disattivabile; palette personalizzabile da file [14]. [media]
- **Non trovato:** nomi delle palette, colori predefiniti.

### 1.5 SDR Console (SDR‑Radio.com)
- **Una sola palette** per spettro e cascata; l'editor ha due riquadri per il colore agli estremi, su una scala
  **0–255**; «Default 1» e «Default 2» come esempi [12]. [media]
- Lo spettro è **ombreggiato** apposta perché le **annotazioni si vedano dietro** al segnale (l'autore del
  programma) [12]. [media]
- Sovrapposizioni sulla cascata: **orologio, statistiche, frequenza**; ~40 righe/s considerate adeguate; S‑meter con
  **punto giallo** per il picco (1 s) e barra a gradiente per la media (0,5 s) [11]. [alta]
- **Band plan** mostrabile nello spettro principale e nelle finestre dei ricevitori, con colore per voce [13]. [media]

### 1.6 OpenWebRX / WebSDR
- **Mappe** (dal codice) [15], [alta]:
  - **Google Turbo**, predefinita dalla 0.20;
  - «teejeez» (predefinita prima): nero, blu, ciano, verde, giallo, rosso, magenta, bianco (un **arcobaleno**);
  - «HA7ILM»: nero, blu acciaio, azzurro, blu scuro, azzurro chiaro, giallo pallido, rosa, rosso scuro;
  - personalizzata.
- **Segnalibri:** «bolle» colorate **sopra la scala delle frequenze**; il colore dice **da dove vengono**: verde =
  piano di banda, giallo = del gestore, azzurro = dell'utente [16]. [alta]
- **Banda passante:** inviluppo del filtro disegnato sopra; si trascinano i bordi o il tutto [17]; sulla scala una
  **icona a trapezio** larga quanto la banda del modo [18]. [alta/media]
- **Colori:** pulsante di **regolazione automatica**, cursori di livello minimo e massimo, pulsante di reset [18].
- **WebSDR (PA3FWM):** non trovata documentazione delle opzioni; la mappa «websdr» di SDR++ (§1.2) ne imita l'aspetto
  (nero → blu → magenta → giallo → bianco) [6]. [media]

### 1.7 SDRangel
- **Disposizione:** spettro sopra o sotto la cascata (interruttore); anche 3D; riga di stato in alto a sinistra con
  zoom, **CF**, **SP**, e a scelta **RBW**, **Cur** (potenza e frequenza del cursore), **Pk** (picco) [1]. [alta]
- **Tracce:** linea, riempimento, gradiente; linea e riempimento **gialli**; il gradiente usa linea **bianca** con
  riempimento colorato dalla mappa; **max hold rosso**; «phosphor» = istogramma di persistenza con decadimento [1].
- **Media:** No, Mov, Fix, **Max**, **Min**; riferimento 0…−110 dB, intervallo 1–100 dB, *Autoscale* [1].
- **Griglia** con intensità regolabile; opzione per **troncare le cifre alte** della scala (con k, M, G) [1].
- **Tempo** sulla cascata: scarto, ora locale o UTC, formato hh:mm:ss [1].
- **Marcatori:** fino a 4 a mirino, il **mirino resta bianco**, il colore scelto va all'etichetta; il secondo mostra
  le **differenze** dal primo; **annotazioni** «Top» = trattini o strisce sottili **in cima allo spettro**, con testo
  fino a 36 caratteri; un clic passa a «Full» (linee di limite e testo) [2]. [alta]
- **Mappe** (28, dal codice) [3]: Angel (predefinita storica), Jet, Turbo, Parula, Hot, Cool, **Batlow, Hawaii,
  Acton, Imola, Tokyo, Lapaz, Buda, Devon, Lajolla, Bamako** (le mappe scientifiche di Crameri [46]), Plasma,
  Rainbow, Prism, Viridis, Loggray, Shrimp, Cubehelix e altre. [alta]

### 1.8 CubicSDR
- Spettro e cascata principali con un **misuratore della media** accanto allo spettro e uno della **velocità**
  (1–1024 righe/s) accanto alla cascata; **Peak Hold** (tasto P); **guadagno visivo** con clic destro trascinato,
  che cambia anche il contrasto della cascata [19]. [alta]
- I demodulatori («modem») sono riquadri sulla cascata: quello attivo è evidenziato; il cursore è **verde** quando
  ne crea uno nuovo e **rosso** quando ne sposta uno [19].
- **Temi** (dal codice) [20]: Default, DefaultJet, Black & White, Sharp, Rad, Touch, HD, Radar. Il **Default usa
  Turbo** a 256 passi; sfondo grigio scuro (10%), linea FFT grigio chiaro (90%), evidenziazione al passaggio
  **gialla**. [alta]

### 1.9 Elementi visivi ricorrenti

| Elemento | Come lo fanno | Quanti | Fonti |
|---|---|---|---|
| Spettro sopra, cascata sotto | proporzione regolabile (cursore, trascinamento, scambio) | tutti | [4][5][9][14][1] |
| Sfondo scuro | nero o grigio quasi nero (`#1F1D1D`, 10%) | tutti | [10][20] |
| Fondo del rumore scuro | mappe che partono da nero o blu notte; GQRX taglia a nero il primo 8% | tutti | [6][10] |
| Mappa predefinita | Classic (SDR#, SDR++), gqrx, Angel, **Turbo** (OpenWebRX, CubicSDR) | 5 diverse | [6][10][3][15][20] |
| Mappe percettive offerte | viridis, inferno/magma/plasma, greyscale, Crameri | 4 su 6 | [6][10][3] |
| Banda passante | rettangolo **semitrasparente** chiaro + linea centrale rossa | 5 | [4][10][5][17][19] |
| Segnalibri | etichette in alto, sopra la scala, **impilate** se si toccano, colore per origine o tipo | 5 | [4][10][16][2][7] |
| Band plan | striscia colorata in cima (o in fondo) allo spettro, con nome | 3 | [5][7][13] |
| Picco | traccia del massimo con decadimento, o cerchietti sui picchi | 5 | [5][1][9][4][19] |
| Media | solo sullo spettro, non sulla cascata (GQRX); separata per i due (SDR#) | 2 | [9][4] |
| Livelli | Range/Offset o Min/Max + **Auto** | tutti | [4][1][18] |
| Griglia | grigia, tenue, a puntini o con intensità regolabile | 2 documentati | [10][1] |
| Tempo | tacche orarie / orologio sulla cascata | 4 | [4][1][11][14] |
| Riga di stato | frequenza centrale, ampiezza, cursore, picco | 1 | [1] |

**Cosa li rende leggibili (sintesi):**
1. **Nero = rumore.** Il pavimento della mappa è nero o blu notte e il guadagno si regola perché il fondo stia lì
   [4][10]. Le righe sono quindi le uniche cose chiare.
2. **Due livelli d'informazione separati:** lo spettro (istantaneo, medio, con il picco) e la cascata (storia grezza)
   [9].
3. **Sovrapposizioni leggere:** griglia al 50%, banda al 16%, linee dei segnalibri al 40%, testo pieno con ombra
   [10]. Nulla copre davvero il segnale.
4. **Colore per categoria, non per valore,** nelle sovrapposizioni: segnalibri per origine [16], band plan per tipo
   [5], VFO per ricevitore [5].
5. **Etichette che non si accavallano** (righe a scalare) [10].

---

## 2. Le consolle del 1989

### 2.1 Stati Uniti
- **BQQ‑5 (Flight I e II, 1989):** tre consolle operatore **identiche** e una consolle del **capo sonar** [FI-BQQ5]
  [23]; «molte presentazioni su CRT» e le cascate come tratto caratteristico [23]; un solo calcolatore UYK‑7 [23].
  [media]
  - Versioni: 3 × OJ‑274 (iniziale), 3 × OJ‑431 (5A), OJ‑462 (5B), **4 × OJ‑544 (5C)** [FI-BQQ5] (vedi `19` §2.3).
  - **Sala:** luce **blu scuro** soffusa nei periodi tranquilli [23]; NOVA: blu per leggere i monitor e per non
    illuminare la centrale [24]. [media]
  - **Cascata BTR** come la descrive la NSMRL: **rilevamento in orizzontale, tempo in verticale, intensità del
    pixel = segnale** («PBB», *passive broadband*) [36]. [alta per il formato]
  - **Banco di prova NSMRL:** la cascata sperimentale usava **otto livelli di luminanza** [36]. È il riferimento più
    vicino al numero di livelli in flotta. [media: è il banco di prova, non per forza la flotta]
- **Famiglia BQQ‑5/6, foto del 1975** («Trident digital sonar system», Marina USA, pubblico dominio): la scheda dice
  solo la didascalia e la provenienza (audizione al Congresso, 1975) [25]. In `19` §9.1 è descritta così: due CRT
  sovrapposti con le cascate e accanto una **matrice di pulsanti quadrati a funzione variabile**. [media]
- **688I (dopo il 1989):** «Greeneville Sonar Room» (NTSB, pubblico dominio): ogni postazione («stack») ha **due
  monitor**; le due a destra mostrano le schiere di scafo [26]. È del 2001: utile per la forma, non per il colore.
- **Colore in arrivo:**
  - 1984: la NSMRL e il NUSC scrivono le regole del colore per i display di **SUBACS A** (poi BSY‑1): **sei colori**
    (rosso, giallo, verde, ciano, blu, bianco) e **luce ambiente bianca tenue** dove ci sono CRT a colori [37].
    [media, da sommario]
  - 1988: *Proceedings* annuncia che i **CRT a colori** arriveranno «presto» sui sottomarini USA [32]. [media, da
    estratto]
  - BSY‑1: GAO dice che la ristrutturazione cambiò la consolle e abbandonò l'idea di una **consolle comune** per
    tiro e acustica [30]. Primo battello SSN‑751 (1988–89). [media]
  - Il colore sul sonar dei 688 è documentato solo dal 5E del 688I (anni '90) [FI-BQQ5].
- **AN/UYQ‑21:** famiglia di consolle di **superficie** (Hughes), **CRT a colori**, comprata dal 1982, circa 3500
  pezzi [27]; tasti a funzione variabile, palla di comando, pannello dei comandi del display; programmata anche per
  presentazioni **acustiche** [29]. Nessuna fonte la mette sui sottomarini. [media]
- **Sostituzione dei CRT:** nel 2015 la Marina compra schermi piatti a colori 17″ 1280×1024 per **sostituire i
  CRT del sonar dei sottomarini** ancora in servizio [28]. [media]
- **Carta:** nella rete SOSUS il LOFARgram fu su **carta elettrostatica** fino al 1992 (`09` §2.2).

### 2.2 URSS
- **Skat / Skat‑KS:** rilevamento, classificazione e **inseguimento automatico** in scoperta passiva, anche
  infrasonica [34]; un calcolatore dedicato alla classificazione (`09` §1.2). [media]
- **Aspetto delle presentazioni:** **nessuna fonte aperta trovata** (né in russo né in inglese).
- Riferimento d'aspetto già scelto in `19`: pulto **verde giallino pallido** con CRT **tondo** (foto del museo B‑396,
  Pr. 641B, CC BY‑SA) e decisione dell'utente: fosforo **verde giallino** per l'Alfa (`19` §8).
- Altri sottomarini museo con sala sonar descritta su Commons: B‑39 (San Diego), B‑143 Foxtrot. Sono
  battelli diesel più vecchi [35].

### 2.3 Riquadro: certo / dedotto

| Affermazione | Stato |
|---|---|
| BQQ‑5: tre consolle operatore uguali + capo sonar | **certo** [FI-BQQ5][23] |
| Le presentazioni erano su CRT, la cascata era quella tipica | **certo** [23][24] |
| Sala con luce blu soffusa | **certo** [23][24] |
| BTR = rilevamento × tempo, intensità come luminosità | **certo** [36] |
| Otto livelli di luminanza nei display sperimentali NSMRL (1986) | **certo** per il banco di prova [36] |
| CRT a colori in arrivo alla fine degli anni '80 (SUBACS/BSY‑1) | **certo** come progetto [32][37] |
| UYQ‑21 a colori, sulle navi di superficie | **certo** [27] |
| BQQ‑5 di un 688 Flight I nel 1989 **monocromatico** | **dedotto** (colore solo col 5E) [FI-BQQ5] |
| Fosforo **verde** (e non ambra) sul BQQ‑5 | **dedotto** (fosfori comuni; NSMRL usava il verde fra i colori di prova) |
| Due CRT sovrapposti per consolle + matrice di tasti quadrati | **dedotto** dalla foto del 1975 e dal 688I |
| Otto‑sedici livelli di grigio in flotta | **dedotto** [36], TRACOR 1979 (`09` §2.6) |
| Aspetto dei display di Skat/Rubikon | **non trovato** |

---

## 3. Leggibilità: cosa dicono gli studi

### 3.1 Studi navali sulle cascate
- **Livelli contano più del contrasto.** DiVita (NSMRL 1078, 1986): con i livelli fissi a **otto**, aumentare il
  contrasto bersaglio/fondo **non migliora** la scoperta; meglio **aumentare il numero di livelli** [36]. [alta per
  la conclusione, da sommario]
- **Il colore della tinta monocromatica conta poco.** Dawe e Galbreath (DSTO, 1997), LOFARgram simulati, otto
  osservatori [43]: [alta]
  - soglia media: **verde 2,4 dB**, giallo 2,7, azzurro 2,8, rosso 3,0; differenze **non significative**;
  - perdita per **quantizzare a quattro livelli**: 0,6 dB;
  - perdita dell'occhio rispetto all'integrazione teorica su **24 righe**: 3,5 dB; la soglia teorica scende con il
    numero di righe visibili (`DT = 11,4 − 10·log(righe)`);
  - **colore per il rilevamento** sul LOFARgram (16 settori, 16 tinte): **guadagno di 1,2 dB**; conta soprattutto la
    **luminosità relativa** delle celle, non la tinta; settori di **verdi vicini** si confondono;
  - i colori cambiano da un monitor all'altro: ogni display va **tarato**.
- **Colore per il Doppler** su display attivi: +1,5 dB (Buratti, Rio e Witlin 1989, citati da [43]). [media]
- **Codici di colore e rumore.** Kraiss (1984): curve ROC diverse fra i codici; alcuni **crollano quando il fondo
  sale**, altri restano stabili [42]. [alta]
- **Fondo blu o nero:** nessuna differenza per tracce rosse, gialle, verdi (NSMRL 1040, 1985); gli operatori non si
  accordano sulla combinazione migliore [38]. [alta, introduzione]
- **Luce ambiente:** la luce **blu** non aiuta; una luce **bianca tenue** è migliore per il dettaglio fine (NSMRL
  1023, Kobus e Neri 1984) [39]; la guida SUBACS chiede luce bianca tenue con i CRT a colori [37]. [media]
- **Otto livelli a colori:** fra quattro schemi per gli otto livelli d'intensità di un sonar antimine, uno è
  **nettamente migliore**, soprattutto **a basso S/N** (NSMRL 1189, 1993) [40]. [media; i colori dello schema non
  sono stati letti]
- **Evidenziare il rilevamento del bersaglio in rosso** sulla cascata migliora la prestazione, senza aggiungere
  informazione (NSMRL 1176, 1991, citato in un estratto) [41]. [bassa: non verificato]
- **Prestazione degli operatori** sotto il potenziale (Luria, DiVita, Neri, 1988) [32]. [media, estratto]

### 3.2 Studi generali sulle mappe di colore
- **Forma contro valore.** Nei test psicofisici il **grigio** mostra meglio la **forma**, le mappe **colorate** il
  **valore**; il compromesso è una sequenza con **luminosità sempre crescente** e varie tinte (Ware 1988, citato in
  [44]). [alta]
- **Ecogrammi (Blackwell e altri, 2019)** [44], [alta]:
  - la mappa EK500 nasce con **4 piani di bit**, uno per le linee fisse: **12 colori**;
  - le mappe storiche sono molto colorate ma **non sequenziali**; la luminosità sale e scende;
  - raccomandazione: **grigio sequenziale** per riconoscere le strutture; **mappa colorata, sequenziale e
    percettiva** (viridis, parula) per leggere i valori;
  - una viridis **a 12 passi** resta leggibile, ma meno passi = meno risoluzione;
  - circa **8% degli uomini** ha un difetto della visione dei colori.
- **Arcobaleno:** manca di ordine percettivo, ha sbalzi di luminosità che nascondono dati e crea bordi falsi (Borland
  e Taylor 2007) [45]; è ancora il più usato e penalizza i daltonici (Crameri 2020) [46]; una replica del 2023 (Ware,
  Stone, Szafir) difende alcuni usi, come leggere un valore da una legenda [49]. [alta/media]
- **Turbo** [48]: sostituto di jet senza bande; pendenza di luminosità circa **doppia di viridis**, quindi dettagli
  piccoli più visibili; estremo basso **più scuro** dell'alto; non va in scala di grigi. Viridis è lineare e
  gradevole; inferno ha più contrasto ma «stanca» su sessioni lunghe. [alta]
- **Kovesi 2015** [47]: il criterio principale è il **passo uniforme di luminosità**; prova con la rampa sinusoidale:
  un'onda del 10% deve vedersi uguale su tutta la scala; jet ha zone piatte che la nascondono. [alta]
- **Dithering:** **nessuno studio** trovato su cascate o sonar. Principio generale: senza dithering si vedono **bordi
  fra livelli adiacenti** (bande) [53]. **STIMA**: su una cascata il dithering ordinato aggiunge una trama fissa che
  può somigliare a righe verticali; meglio un dithering **casuale** o nessuno, con abbastanza livelli.

### 3.3 Conseguenze per il disegno (sintesi)
1. **Molti livelli di luminanza, non più contrasto** [36]; 16 livelli bastano: a 4 la perdita è già solo 0,6 dB [43].
   **STIMA** per il 16.
2. **Nel 1989 la tinta è libera**: verde, ambra o verde giallino rendono uguale [43]. Si sceglie per l'epoca.
3. **Mostrare molte righe di storia**: l'occhio integra le righe [43].
4. **Il colore aggiunge valore se porta un'informazione in più** (rilevamento, Doppler) [43], non se ricolora
   l'intensità.
5. **Mappa sequenziale con luminosità crescente** come predefinita [44][47]; Turbo come scelta «contrasto» [48];
   grigio per chi cerca le forme [44].
6. **Fondo nero** e **sala non blu** per la leggibilità; il blu resta come atmosfera [38][39].

---

## 4. Proposte per la stazione (640×480, 256 colori indicizzati)

Vincoli: una palette di 256 voci per tutto lo schermo; niente vera trasparenza. La «semitrasparenza» si fa con una
**seconda rampa** precalcolata (la mappa mescolata a un colore) o con un **tratteggio** a scacchiera o diagonale, come
il riempimento A‑B di GQRX [10]. **STIMA** per tutte le misure in pixel.

### 4.1 Bilancio della palette (STIMA)

**Modo 1989**

| Voci | Uso |
|---|---|
| 0–15 | interfaccia (pannello, testo, tasti) |
| 16–31 | **rampa del fosforo**, 16 livelli |
| 32–47 | rampa **evidenziata** (fascio): stessi 16 livelli + 1 gradino |
| 48–63 | scala, griglia, cursori, testo del CRT (verde pieno, verde medio) |
| 64–255 | stanza, cornici, lampade, allarmi; rampa verde giallino dell'Alfa |

**Modo moderno**

| Voci | Uso |
|---|---|
| 0–15 | interfaccia |
| 16–79 | **mappa di colore**, 64 livelli (viridis, turbo, grigio, «Classic», «WebSDR»: si ricarica al cambio) |
| 80–143 | stessa mappa **mescolata al 20% con bianco** (fascio o banda evidenziata) |
| 144–207 | **colore per rilevamento**: 8 settori × 8 livelli (§4.3, punto 12) |
| 208–239 | band plan e tracker: 8 tipi di riga × 2 toni; 8 colori di tracker × 2 toni |
| 240–255 | griglia, cursori, picco, testo, allarmi |

### 4.2 Modo «1989» (fosforo verde, manuale)

| # | Elemento | Come | Perché / fonte |
|---|---|---|---|
| 1 | **Rampa monocromatica a 16 livelli** | dal nero al verde P1/P39 (~525 nm); livello 0 = nero | più livelli > più contrasto [36]; tinta indifferente, verde il migliore in media [43]; fosfori in `19` §9.1 |
| 2 | **Pavimento a nero** | il rumore normalizzato cade nei livelli 0–2 | come SDR# (fondo in basso) [4] e la «gqrx» (nero iniziale) [6] |
| 3 | **Due CRT sovrapposti** per consolle | sopra BTR, sotto LOFAR (o DEMON) | foto 1975 e 688I [25][26]; `19` §9.1 |
| 4 | **Matrice di tasti quadrati** accanto ai CRT | etichette maiuscole 5×7 / 7×9 | foto 1975 [25]; MIL‑STD‑1472 (`19` §9.1) |
| 5 | **Scala dei rilevamenti con tacche** | tacca ogni 5°, lunga ogni 10°, numero ogni 30° | formato PBB [36]; **STIMA** per il passo |
| 6 | **Fascio puntato sul BTR** | due **parentesi** sulla scala + bordi a puntini nella cascata; i pixel dentro usano la rampa evidenziata (+1 gradino) | banda passante delle SDR [4][10] resa senza trasparenza; **STIMA** |
| 7 | **Cursore** | linea piena al livello massimo, con lettura numerica in un angolo | mirino bianco di SDRangel [2]; **STIMA** |
| 8 | **Etichette dei tracker** (S1, S2…) | testo verde sopra la scala, a righe se si toccano, linea tratteggiata in giù | segnalibri GQRX [10]; nomi in `09` §2.7 |
| 9 | **Griglia del tempo** | tacca ogni minuto, ora di gioco ogni 5 min sul bordo sinistro | Time Markers di SDR# [4]; SDRangel [1] |
| 10 | **Molte righe di storia** | la cascata usa tutta l'altezza disponibile | integrazione visiva [43] |
| 11 | **Niente band plan colorato** | solo tacche fisse sulla scala delle frequenze (50, 60, 400 Hz) | colore non ancora in flotta [FI-BQQ5]; **STIMA** |
| 12 | **Persistenza leggera** | coda di 1–2 fotogrammi solo sul cursore e sull'A‑scan | effetto CRT; **STIMA** (i BTR digitali non ne avevano bisogno) |
| 13 | **Cornice blu della sala** | il blu solo fuori dagli schermi | luce blu [23][24]; non aiuta la lettura [39] |
| 14 | **Variante Alfa** | stessa logica, rampa **verde giallino**, CRT tondo a ventaglio | decisione in `19` §8, §6.6 |

### 4.3 Modo «moderno» (colore, aiuti)

| # | Elemento | Come | Perché / fonte |
|---|---|---|---|
| 1 | **A‑scan sopra, cascata sotto**, divisore regolabile | trascinare il bordo; tasto per scambiare | GQRX [9], HDSDR [14], SDRangel [1] |
| 2 | **Mappa predefinita: viridis** a 64 livelli | sequenziale, regge il daltonismo | [44][47]; `09` §2.6 |
| 3 | **Altre mappe**: turbo, grigio, «Classic SDR#», «WebSDR» | menu | turbo = contrasto [48], predefinita in OpenWebRX e CubicSDR [15][20]; grigio per le forme [44]; Classic per familiarità [6] |
| 4 | **Livelli: pavimento, tetto, AUTO** | due cursori + pulsante | Range/Offset [4]; min/max e auto [18]; Autoscale [1] |
| 5 | **Band plan colorato sopra il LOFAR** | striscia di 6–8 px con le righe attese della classe sovrapposta, colorate per tipo (rete, pompa, turbina, riduttore, elica…) | band plan SDR++/SDR# [5][7]; `09` §4.2 punto 8 |
| 6 | **Rettangolo semitrasparente del fascio sul BTR** | colonne del fascio con la rampa mescolata al bianco (voci 80–143); bordi rossi chiari | banda di SDR# e GQRX (bianco al 16%, linea rossa) [4][10] |
| 7 | **Stesso rettangolo sul LOFAR** per il verniero | mostra la banda che il verniero ingrandisce | VFO di SDR++ [5] |
| 8 | **Tracker come segnalibri** | bolle sopra la scala con «S3», colore per **schiera** (sfera, scafo, rimorchiata) o per **classificazione**; righe a scalare; linea tratteggiata al 40% | OpenWebRX (colore per origine) [16]; GQRX (righe, tratteggio) [10] |
| 9 | **Traccia del picco** sull'A‑scan | più tenue della traccia principale, decadimento regolabile | FFT Hold [5]; max hold [1][9] |
| 10 | **Cerchietti sui picchi** rilevati | marca le righe che l'analisi automatica ha trovato | Mark Peaks [4]; «D» di GQRX [9] |
| 11 | **Media solo sull'A‑scan** | la cascata resta grezza | GQRX [9]; `09` §4.2 |
| 12 | **Colore per rilevamento sul LOFAR** (aiuto opzionale) | 8 tinte ben distanti × 8 livelli; luminosità = S/N, tinta = settore | +1,2 dB [43]; tinte lontane, non verdi vicini [43] |
| 13 | **Bersaglio designato in rosso** sul BTR | i pixel del rilevamento del tracker scelto | NSMRL 1176 [41] (bassa) |
| 14 | **Barra dello zoom** sotto la scala | ×1–×16, pulsanti R / C / D (centra sul fascio) | GQRX [9]; SDR# avverte sulla risoluzione [4] |
| 15 | **Scala che tronca le cifre** | «245» invece di «245,0°»; Hz senza decimali oltre ×4 | SDRangel [1] |
| 16 | **Riga di stato** | fascio, banda, cursore (Hz, dB), picco | CF/SP/Cur/Pk di SDRangel [1] |
| 17 | **Marcatore delta** | secondo cursore che mostra Δf e Δt dal primo (passi d'armonica, Doppler) | SDRangel [2] |
| 18 | **Griglia tenue** | grigio a puntini al 50%; testo chiaro con ombra nera | GQRX [10] |
| 19 | **Ora sulla cascata** | come nel 1989, più le annotazioni degli eventi | [1][11] |
| 20 | **Sala con luce bianca tenue** (opzione) | cornice più chiara | [37][39] |

### 4.4 Disposizione (STIMA, da rifinire con la `09` §4.3)
- **Sinistra (≈ 320×330):** BTR con scala dei rilevamenti in alto (16 px), striscia dei tracker sopra (12 px), ora a
  sinistra.
- **Destra (≈ 300×330):** A‑scan (≈ 60 px), band plan (8 px), LOFAR (≈ 190 px), scala delle frequenze con barra di
  zoom (≈ 20 px), DEMON piccolo o al posto del LOFAR con un tasto.
- **Fascia in basso (≈ 640×120):** contatti, slot (solo moderno), tasti a matrice.
- Nel modo 1989 la stessa griglia, ma come **due CRT** con la cornice e i tasti a lato.

---

## 5. Fonti

**Programmi SDR**
1. SDRangel, documentazione dello spettro (disposizione, tracce, medie, riga di stato, tempo). https://github.com/f4exb/sdrangel/blob/master/sdrgui/gui/spectrum.md
2. SDRangel, marcatori e annotazioni (mirino bianco, delta, annotazioni «Top»). https://github.com/f4exb/sdrangel/blob/master/sdrgui/gui/spectrummarkers.md
3. SDRangel, codice delle mappe di colore (28 nomi). https://raw.githubusercontent.com/f4exb/sdrangel/master/sdrbase/util/colormap.cpp
4. RTL‑SDR.com, *SDRSharp Users Guide* (Range, Offset, Contrast, Zoom, banda, Time Markers, Mark Peaks, Frequency Manager). https://www.rtl-sdr.com/sdrsharp-users-guide/
5. *SDR++ User Guide*, pp. 34–37 (Display, FFT Hold, Color Map, VFO, Bandplans, Themes). https://www.sdrpp.org/manual.pdf
6. SDR++, cartella delle mappe e file `classic.json`, `classic_green.json`, `websdr.json`, `gqrx.json` (valori esadecimali). https://github.com/AlexandreRouma/SDRPlusPlus/tree/master/root/res/colormaps
7. Arrin‑KN1E, band plan per SDR#/SDR++ (striscioni colorati, colori a 8 cifre con trasparenza). https://github.com/Arrin-KN1E/SDRSharp-Band-Plans
8. RadioReference, discussione sul band plan di SDR# (voce `color="50AAAAAA"`, via estratto). https://forums.radioreference.com/threads/bandplan-should-respect-offset-of-starting-frequency.440170/
9. GQRX, «Practical tricks and tips» (divisore, media, picchi, zoom, banda grigia). https://gqrx.dk/doc/practical-tricks-and-tips
10. GQRX, `plotter.cpp` (colori di sfondo, griglia, banda, segnalibri, nomi delle mappe). https://raw.githubusercontent.com/csete/gqrx/master/src/qtgui/plotter.cpp
11. SDR‑Radio.com, Spectrum and Waterfall (sovrapposizioni, velocità, S‑meter). https://www.sdr-radio.com/SpectrumAndWaterfall
12. Forum SDR‑Radio, «Spectrum & Waterfall Coloring?» (palette unica, ombreggiatura per le annotazioni). https://forum.sdr-radio.com/viewtopic.php?p=7911
13. SDR‑Radio.com, Band Plan. https://sdr-radio.com/band-plan
14. HDSDR, FAQ (nessun manuale) e scorciatoie; guida G4ZFQ (rapporto spettro/cascata). https://hdsdr.de/faq.html ; https://hdsdr.de/hdsdr_keyboard_shortcuts.htm ; https://www.sites.google.com/site/g4zfqradio/installing-and-using-hdsdr
15. OpenWebRX, `owrx/waterfall.py` (Turbo predefinita, teejeez, HA7ILM). https://raw.githubusercontent.com/jketterl/openwebrx/develop/owrx/waterfall.py
16. OpenWebRX wiki, «How the bookmarks work» (bolle colorate per origine). https://github.com/jketterl/openwebrx/wiki/How-the-bookmarks-work
17. OpenWebRX wiki, Usage (inviluppo del filtro, zoom). https://github.com/jketterl/openwebrx/wiki/Usage
18. Palomar Amateur Radio Club, guide a OpenWebRX+ (auto‑colori, min/max, trapezio sulla scala). https://services.palomararc.org/article3.html ; https://services.palomararc.org/article4.html
19. CubicSDR, Main Application Window. https://cubicsdr.readthedocs.io/en/latest/application-window.html
20. CubicSDR, `ColorTheme.cpp` (temi; Default = Turbo). https://raw.githubusercontent.com/cjcliffe/CubicSDR/master/src/visual/ColorTheme.cpp
21. DL4YHF, Spectrum Lab, palette (arcobaleno predefinito, cursori B e C). https://qsl.net/d/dl4yhf/speclab/colorpal.htm

**Consolle e sale sonar**
22. Forecast International, AN/BQQ‑5(V) (versioni e consolle) = [FI-BQQ5] di `19`. https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641
23. «698 had the oldest…», ricordi di un sonarista del Bremerton (tre stazioni, CRT, cascate, luce blu, UYK‑7). https://bremertonreunion.net/2023/05/04/698-had-the-oldest/
24. NOVA, «Sonar room» e «Sonar display screen» (luce blu; cascata; disegno dal libro di Clancy). https://www.pbs.org/wgbh/nova/subsecrets/sprisonhiright.html ; https://www.pbs.org/wgbh/nova/subsecrets/sprisonlortsds.html
25. Wikimedia Commons, «Trident digital sonar system.jpg» (Marina USA, 1975, pubblico dominio). https://commons.wikimedia.org/wiki/File:Trident_digital_sonar_system.jpg
26. Wikimedia Commons, «Greeneville Sonar Room.jpg» (NTSB, due monitor per postazione). https://commons.wikimedia.org/wiki/File:Greeneville_Sonar_Room.jpg
27. Military & Aerospace Electronics, AN/UYQ‑21 (CRT a colori, dal 1982, navi di superficie). https://www.militaryaerospace.com/power/article/16720305/navy-chooses-behlman-power-supplies-to-keep-ageing-an-uyq-21v-shipboard-displays-in-working-order
28. Military & Aerospace Electronics, schermi piatti per sostituire i CRT del sonar dei sottomarini (2015). https://www.militaryaerospace.com/communications/article/16713721/navy-looks-to-argon-corp-to-provide-flat-panel-sonar-displays-to-replace-crts-aboard-submarines
29. Tesi NPS 1978 sulla UYQ‑21 (tasti variabili, palla, presentazioni acustiche; via estratto). https://apps.dtic.mil/sti/pdfs/ADA056383.pdf
30. GAO, *Development of the AN/BSY‑1 Combat System*, NSIAD‑92‑50 (consolle comune abbandonata; via estratto). https://www.gao.gov/assets/nsiad-92-50.pdf
31. *Proceedings*, «U.S. Navy Sonars, Part 2», 1981 (BQQ‑5B con consolle migliorate; via estratto). https://www.usni.org/magazines/proceedings/1981/september/us-navy-sonars-part-2
32. Luria, DiVita, Neri, «Improving Submarine Sonar Displays», *Proceedings*, febbraio 1988 (via estratto; pagina 403). https://www.usni.org/magazines/proceedings/1988/february/improving-submarine-sonar-displays
33. Naval Submarine League, storia dei sistemi di direzione del tiro (Mk 113 con quadranti, BSY‑1 «un solo quadro tattico»). https://archive.navalsubleague.org/1993/a-brief-history-of-the-push-pull-of-submarine-combat-control-systems
34. VPK.name, Pr. 671RTMK «Щука» (Skat‑KS: scoperta, classificazione, inseguimento automatico). https://vpk.name/library/f/671rtmk-shuka.html
35. Wikimedia Commons, ricerca «sonar room submarine» (B‑39, Foxtrot B‑143, HMAS Onslow con due schermi sonar d'attacco, CC BY‑SA). https://commons.wikimedia.org/wiki/File:HMAS_Onslow,_Oberon_class_submarine,_1968_(6698468135).jpg

**Leggibilità**
36. J. DiVita, NSMRL Report 1078, *The Effect of Contrast on Target Detection in PBB Displays*, 1986 (via estratto; DTIC rifiuta l'accesso diretto). https://apps.dtic.mil/sti/pdfs/ADA171807.pdf
37. D. F. Neri, D. Zannelli, NSMRL Report 1032, *Guidelines for the Use of Color in SUBACS A Displays*, 1984 (via sommario). https://apps.dtic.mil/sti/tr/pdf/ADA148924.pdf
38. Neri, Kobus, Luria, Fan, Zannelli, NSMRL 1040 / NUSC TR 7325, *Effect of Background/Foreground Color Coding on Detection in Acoustic Data Displays*, 1985. https://apps.dtic.mil/sti/tr/pdf/ADA154105.pdf
39. Kobus, Neri, NSMRL Report 1023, illuminazione ambiente e scoperta, 1984 (via estratto; numero DTIC non verificato).
40. Laxar, DiVita, Gallagher, Benoit, NSMRL Report 1189, codifica a colori degli otto livelli d'intensità, 1993 (via estratto). https://apps.dtic.mil/sti/pdfs/ADA275196.pdf
41. Luria, DiVita, Johnson, NSMRL Report 1176, 1991, pixel del rilevamento del bersaglio in rosso (solo estratto, non verificato).
42. Kraiss, Küttelwesch, «Color and grey scale in sonar displays», NASA Ames, 1984 (= [Kraiss84] di `09`). https://ntrs.nasa.gov/citations/19850006243
43. R. L. Dawe, E. R. Galbreath, «Detection Enhancement Using Colour for Sonar Displays», ICSV5, Adelaide, 1997 (letto per intero). https://www.acoustics.asn.au/conference_proceedings/ICSVS-1997/pdf/scan/sv970033.pdf
44. R. E. Blackwell e altri, «Colour maps for fisheries acoustic echograms», *ICES J. Mar. Sci.*, 2019 (letto in parte). https://nora.nerc.ac.uk/id/eprint/526342/1/fsz242.pdf
45. D. Borland, R. M. Taylor II, «Rainbow Color Map (Still) Considered Harmful», *IEEE CG&A* 27(2), 2007 (abstract). https://www.sci.utah.edu/~kpotter/Library/Papers/borland:2007:RCCH/index.html
46. F. Crameri, G. Shephard, P. Heron, «The misuse of colour in science communication», *Nat. Commun.* 11, 5444, 2020. https://pmc.ncbi.nlm.nih.gov/articles/PMC7595127
47. P. Kovesi, «Good Colour Maps: How to Design Them», 2015. https://arxiv.org/pdf/1509.03700
48. A. Mikhailov, «Turbo, an improved rainbow colormap», Google Research, 2019. https://research.google/blog/turbo-an-improved-rainbow-colormap-for-visualization/
49. C. Ware, M. Stone, D. A. Szafir, «Rainbow Colormaps Are Not All Bad», *IEEE CG&A*, 2023 (via estratto). https://repository.library.noaa.gov/view/noaa/58953
50. Viridis (BIDS) = [Viridis] di `09`. https://bids.github.io/colormap/
51. TRACOR 1979, colore nei display sonar = [TRACOR79] di `09`. https://apps.dtic.mil/sti/tr/pdf/ADA066691.pdf
52. Scale di colore regolabili, NUSC/Salafia = [HFS91] di `09`. https://journals.sagepub.com/doi/pdf/10.1177/154193129103502011
53. MIT 6.837, «Monochrome and Color Dithering» (bande senza dithering). https://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture10/dithering.html

**Non trovato** (vedi anche `09` §8 e `19` §8):
- aspetto delle presentazioni di MGK‑400 Rubikon e MGK‑500 Skat;
- colore del fosforo e numero di livelli di grigio delle OJ‑274/431/462/544;
- una sigla «SDU» legata a BSY‑1 o UYQ‑21;
- manuali NAVEDTRA aperti per sonaristi di sottomarino con le schermate;
- testo completo dei rapporti NSMRL (DTIC rifiuta l'accesso automatico) e dell'articolo di *Proceedings* 1988;
- nomi delle palette di HDSDR; documentazione delle opzioni del WebSDR di PA3FWM;
- studi sul dithering applicato a cascate o sonar.
