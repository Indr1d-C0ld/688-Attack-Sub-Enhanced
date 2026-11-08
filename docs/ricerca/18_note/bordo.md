# Dossier 18 — Suoni esterni a bordo, senza cuffie: cosa passa attraverso lo scafo

Ricerca su fonti aperte, 25 ottobre 2026. Destinazione: simulatore di sommergibili d'attacco anni '80 (688 Attack Sub).
Nessun file del progetto è stato modificato. Script e scarichi di lavoro stanno in `.../scratchpad/dossier18/` (sottocartella `dl/`).

## Convenzioni

- **Confidenza**: alta / media / bassa.
- **[V]** = verificato (ho letto io il testo della fonte o la pagina della licenza); **[D]** = dedotto da fonti verificate con un passaggio logico mio; **[STIMA]** = numero mio, ricavato con calcolo o ordine di grandezza, da trattare come parametro di progetto e non come dato misurato; **[NV]** = non verificato (richiamato da memoria o riportato da una fonte secondaria).
- Livelli in acqua: dB re 1 µPa. Livelli in aria: dB re 20 µPa. A parità di intensità, 1 dB in acqua re 1 µPa vale 61,5 dB in meno in aria re 20 µPa (Leighton 2012 [V], vedi §2).
- Dove ho dovuto passare dal Wayback Machine (USNI, Warfare History Network, DTIC, Institute of Acoustics bloccano l'accesso diretto), lo segnalo; il testo è comunque quello dell'originale archiviato.

---

## 0. Sintesi per il progetto (leggere questa e basta, se si ha fretta)

1. **Si sente davvero molto meno di quanto il cinema faccia credere, ma non niente.** Le testimonianze di guerra e di Guerra Fredda concordano su quattro cose udibili a orecchio nudo: (a) i rotori/eliche di una nave sulla verticale, quando la barca è silenziosa; (b) lo splash e il colpo delle cariche di profondità, anche a distanza; (c) i ping delle scorte quando sono vicine (descritti come tintinnio/ghiaia/martellate sullo scafo, quasi mai come un "ping" musicale); (d) i rumori di un bastimento che affonda (cricchiolii, scoppi di caldaie). Non ho trovato nessuna testimonianza di prima mano che metta tra i suoni udibili a orecchio balene, gamberi, sonoboe, elicotteri o aerei.
2. **La funzione di trasferimento acqua → interno** è, in prima approssimazione, un passa basso: piatta fino a circa 500 Hz, poi circa −5,5 dB/ottava. Il numero utile: **livello interno [dB re 20 µPa] ≈ livello esterno [dB re 1 µPa] − K(f)**, con K ≈ 91 dB a 50–250 Hz, 96 dB a 1 kHz, 107 dB a 4 kHz, 113 dB a 8 kHz, con incertezza ±10–15 dB **[STIMA, §2.4]**. Un controllo incrociato con le misure NSMRL dei ping del proprio sonar nei compartimenti prodieri (93–108 dB) torna entro l'ordine di grandezza (§2.5).
3. **Il rumore di fondo interno decide tutto.** Compartimenti vivibili: circa 55–70 dBA (criterio NAVSHIPS 1970: 70 dB per gli alloggi); centrale e uffici da progetto 60–65 dBA; sala macchine 85–90+ dBA (misura di una barca RN: 87 dBA). Con la barca in assetto silenzioso (macchine spente) il fondo cala di parecchio, e allora eliche e cariche si sentono; in sala macchine con i diesel o le turbine in marcia non si sente nulla di esterno (testimonianza USS Gunnel: i macchinisti non si accorsero nemmeno delle bombe).
4. **Aerei ed elicotteri: no, non a orecchio.** Il suono entra in acqua solo entro ~13° dalla verticale (finestra di Snell, verificato), dura "un paio di secondi" e, anche per un jet da combattimento a 120–190 m di quota, in acqua a 33 m di profondità si misurano circa 134 dB re 1 µPa. Con K ≈ 91–96 dB all'interno restano circa 40 dB: sotto il fondo. Lo strumento della barca li vede (idrofono/array), l'orecchio no. Un rombo "attutito" all'interno sarebbe una scelta artistica, non realismo (§3).
5. **Campioni**: ci sono molti campioni **CC0** (nessun obbligo) e alcuni CC BY (obbligo di credito) di elicotteri, turboelica (Lockheed Electra, C-130) e jet su Freesound; un pacchetto CC0 "Vintage Submarine Effects" con interni di sommergibile (sala siluri a prua, centrale, sala macchine, ecc.). **Gli effetti BBC (RemArc) non vanno inclusi in un gioco distribuito** (§4.4). Per gli interni c'è poco di certificato: il pacchetto CC0 è di provenienza dichiarata dall'uploader (effetti da studio hollywoodiani anni '30–'40), non una registrazione documentata a bordo di un battello militare (§5).

---

## 1. Cosa si sente a bordo, evento per evento

### 1.1 Eliche di una nave che passa sulla verticale

| Affermazione | Fonte | Stato | Conf. |
|---|---|---|---|
| USS Tambor (SS-198), 3 feb 1944, in fondo a 268 piedi: a ogni passaggio del cacciatorpediniere "un rombo che cresceva fino a ruggire come una locomotiva a vapore su un cavalletto direttamente sopra"; poi, "attraverso lo scafo", il tonfo delle cariche che si inabissavano, l'attesa e lo scoppio. Il suono delle eliche "attraverso lo scafo" era a volte l'unico modo di seguire l'avversario con la barca sul fondo (i sonar a testa orientabile erano inutilizzabili sul fondo). | Robert Schultz e James Shell, *Cornered at the Bottom of the East China Sea*, Naval History Magazine 23(6), dic 2009. https://www.usni.org/magazines/naval-history-magazine/2009/december/cornered-bottom-east-china-sea (letto via Wayback) | [V] testimonianza raccolta dall'equipaggio, ricostruita da articolo | alta |
| Tambor: "il suono delle eliche attraverso lo scafo" dopo una pausa; "Bob [Hunt] sentì come se trapanassero dentro la testa"; altro marinaio: "tanto vicino che sentivamo quelle eliche attraverso lo scafo". | stesso articolo | [V] | alta |
| WWI: comandante di U-boot (riportato da Hersing/U-21) ricorda "il rombo cavo delle eliche dei cacciatorpediniere sopra" durante cinque ore di attacco. | Naval Submarine League, *Depth Charge: An Early Antisubmarine Warfare Weapon, Part II, WWI*, 2003. https://archive.navalsubleague.org/2003/depth-charge-an-early-antisubmarine-warfare-weapon-part-ii-world-war-i | [V] che l'articolo lo riporta; la citazione è di seconda mano | media |
| USS Gunnel, Nov 1942: i macchinisti in sala macchine poppiera con "tutti i motori in linea" non sentirono né seppero degli attacchi aerei, per "il rumore assordante dei motori". | Joe Vasey/Rudolph Velie in NSL, *USS Gunnel (SS253) First War Patrol*, 2007. https://archive.navalsubleague.org/2007/uss-gunnel-ss253-first-war-patrol-october-19-1942-december-7-1942 (via Wayback) | [V] | alta |
| Livello dell'elica a ~100 m di distanza: SL di una nave veloce 170–190 dB re 1 µPa @ 1 m, banda larga, dominata da 50–500 Hz → RL ≈ 130–150 dB → all'interno ≈ 40–60 dB. Udibile con il fondo di una barca in assetto silenzioso (35–45 dBA), mascherato con le macchine in marcia. | mio calcolo con K(f) del §2.4; SL tipico da letteratura generale, non riverificato | **[STIMA]** | bassa-media |

Note di gioco: la sequenza testimoniata è "rombo crescente → splash e click → botto". Ha la forma di un evento a tre fasi, non di un suono statico.

### 1.2 Ping attivi di navi (WWII e Guerra Fredda)

**Che cosa dicono le fonti**

| Affermazione | Fonte | Stato | Conf. |
|---|---|---|---|
| I sonar americani degli anni '30–'40 ("searchlight sonars") operavano a **24–30 kHz, sopra la gamma dell'udito**, in impulsi ("ping") di 20–200 ms in un fascio di ~10°. | Thomas G. Muir, *Underwater Acoustics: A Brief Historical Overview Through World War II*, Acoustics Today 17(1), 2021. https://acousticstoday.org/wp-content/uploads/2021/03/Underwater-Acoustics-A-Brief-Historical-Overview-Through-World-War-II-Thomas-G.-Muir.pdf | [V] | alta |
| Moderni sonar attivi trasmettono "entro la gamma udibile" (toni alti, convertiti a frequenze più basse per l'ascolto in cuffia). | Bureau of Naval Personnel, *Introduction to Sonar*, NAVPERS 10130-B (testo su archive.org, "tm-11-467-radar-system-fundamentals-apr.-1944/Introduction to sonar"). https://archive.org/details/tm-11-467-radar-system-fundamentals-apr.-1944 | [V] | media (edizione non datata con certezza) |
| AN/SQS-26 (cacciatorpediniere/fregate americane, anni '60–'70): sonar attivo a 3–4 kHz. | Wikipedia, AN/SQS-26. https://en.wikipedia.org/wiki/AN/SQS-26 | [V] come dato enciclopedico | media |
| USS Plunger (SS-179), prima pattuglia, acque giapponesi, dic 1941–gen 1942: il cacciatorpediniere "usava un potente sonar a eco... il 'pinging'. Sentire il pinging fu uno shock"; seguirono 24 cariche vicine. | Wikipedia, USS Plunger (SS-179), che cita Blair, *Silent Victory* (1975), p. 91. https://en.wikipedia.org/wiki/USS_Plunger_(SS-179) | [V] la frase su Wikipedia; il libro di Blair [NV] | media (non è detto che l'ascolto fosse "a orecchio" e non in cuffia) |
| USS Gunnel (SS-253), nov 1942, tre scorte alleate a ≤3.000 iarde: "il pinging dei loro ASDIC risuonava forte contro lo scafo del sommergibile". Joe Vasey era alla sala di tiro in centrale/torretta. | NSL, USS Gunnel (vedi sopra) | [V] testimonianza a posteriori | media-alta |
| WWII, USS Tambor: "Red Mayo [sul sonar] sentì cambiare il pinging a una scala più corta" — questo però era in cuffia/sul ponte, non a orecchio. | Schultz e Shell, 2009 | [V] | alta (ma non è un caso di ascolto a orecchio) |
| *Blind Man's Bluff*, episodio USS Gudgeon: "Gli uomini sentono i ping del sonar attivo risuonare attraverso la barca." | È la frase di una recensione (https://www.ahalbert.com/reviews/2023/07/01/blind_mans_bluff.html); il testo di Sontag e Drew non l'ho letto. | [NV] sul libro; [V] sulla recensione | bassa-media |
| Forum SUBSIM, thread "ASDIC clarification": ex RN classe "O" (fine anni '60): suono "attutito simile a ghiaia o sassolini contro lo scafo", solo entro una certa distanza della nave; "non si sente un ping come lo sente l'operatore"; seconde mani da racconti di sommergibilisti tedeschi e italiani: ticchettio di orologio (il più comune), ghiaia sullo scafo, martello; "il suono tipo ping non è mai descritto da chi lo subisce"; un altro utente (attivo su una barca d'attacco USA moderna) in vent'anni non l'ha mai sentito. | https://www.subsim.com/radioroom/showthread.php?t=203487 | [V] che i post dicono questo; non sono testimonianze verificabili | **bassa** |
| Lo stesso forum riporta (post di un utente non marinaio) che il manuale americano dice che il ping "non si può sentire senza apparecchi riceventi". | https://www.subsim.com/radioroom/showthread.php?p=270963 | [NV]: non ho recuperato il capitolo del manuale citato (maritime.org *Fleet Type Submarine*) | bassa |

**Misure di livelli: ping del proprio sonar nei compartimenti (le uniche cifre solide trovate)**

Il rapporto NSMRL/Naval Submarine Medical Center n. 682 di J. Donald Harris (13 ott 1971), *Hearing Conservation and Acoustic Habitability for Submariners* (DTIC ADA349945; letto via Wayback: https://apps.dtic.mil/sti/tr/pdf/ADA349945.pdf) riassume decenni di misure e prove su sonar attivi delle barche (impulsi tonali a 2,2–4,5 kHz, durata 0,04–3 s):

- Sulla USS Tigrone (AGSS-419), prova congiunta del 1965: impulsi misurati sulla barca a **93–108 dB** (aria, re 20 µPa), uno ogni 60 s; 108 dB portavano a perdita di udito in 70 min. [V], alta.
- USS Bergall (SSN-667), 1970, prove a pieno regime per 3,5 ore: **95 dB** accettabili in certi spazi; "oltre 100 dB nei locali di lavoro" per una settimana di crociera se si indossa protezione. [V], alta.
- Criteri proposti per gli spazi abitativi: 75–80 dB (BBN 1959/1961), 75 dB (NavUndersea R&D 1969, "Classe 688"), 70–75 dB (britannici, per impulsi lunghi), **85 dB** infine validato da NAVSUBMEDRSCHLAB nel 1971 (24 giorni a 85 dB, 1 s ogni 60 s). [V], alta.
- Il rapporto dice espressamente: i livelli pericolosi si registrano "nei locali del sonar vicini al trasduttore e in realtà in tutta la parte prodiera della nave". [V], alta.

Questo è il limite superiore realistico per il **proprio** sonar a bordo (trasduttore a pochi metri dalla sala siluri), non per il ping di un altro mezzo.

**Ping di un'altra nave: stima**

Con SL 230 dB re 1 µPa @ 1 m (ipotesi mia per un sonar attivo di scorta anni '60–'80; non verificata) e K(4 kHz) = 107 dB: a 500 m ≈ 69 dB all'interno; a 2 km ≈ 57 dB; a 5 km ≈ 52 dB; a 10 km ≈ 40 dB **[STIMA]**. In un compartimento con fondo 45–55 dBA, a 4 kHz (dove il fondo è basso) un tono a 52–69 dB si sente chiaramente. L'ipotesi SL 230 dB va abbassata di 20–30 dB per un ASDIC della WWII (livelli non verificati) e le frequenze 24–30 kHz sono comunque sopra l'udito: per i ping della WWII la sorgente "udibile" è probabilmente il transiente di inizio impulso e la risposta dello scafo (ticchettio, ghiaia, "ring"), non il tono (questa è una **deduzione [D]**, concorde con le descrizioni dei forum e con la frase di Vasey, ma nessuna fonte tecnica che io abbia letto la dimostra).

### 1.3 Ping di elicotteri a immersione e siluri in homing

- **Nessuna testimonianza di prima mano trovata** di equipaggi che sentano a orecchio un dipping sonar o un siluro in homing. Le uniche descrizioni sono in narrativa (Proceedings 2022, *A Tool for the Future Submarine Force*, https://www.usni.org/magazines/proceedings/2022/april/tool-future-submarine-force; recensione NSL di *Firing Point*): **non vanno usate come prova** [V che sono narrativa].
- Fatti verificabili: il bersaglio sente la piattaforma attiva "almeno al doppio della distanza a cui il suo sonar attivo vede l'eco" (propagazione a senso unico contro andata e ritorno), Fonte: *How Warships Above The Waves Hunt For Enemy Submarines* (articolo di un sommergibilista veterano), The War Zone (https://www.twz.com/36885/how-warships-above-the-waves-hunt-for-enemy-submarines-down-below, letto via Wayback; data di pubblicazione non verificata). Un elicottero in hovering "a volte suona come un cacciatorpediniere a un sommergibile", ma sul sonar: VADM George Steele, *Killing Nuclear Submarines*, Proceedings nov 1960, ripubblicato in Submarine Review (https://archive.navalsubleague.org/2004/killing-nuclear-submarines). [V], media-alta.
- Frequenze/livelli di AN/AQS-13, AQS-18 e dei pinger delle sonoboe attive (SSQ-47): **non trovati** in fonti aperte affidabili ("il lato alto della gamma di frequenze medie" per AQS-13, Wikipedia; niente di numerico). Siluri (Mk 46/48, homing a frequenze di decine di kHz): **[NV]**, non verificato.
- **Proposta [D]**: trattarli come i ping di navi, con SL 200–220 dB re 1 µPa @ 1 m **[STIMA, non verificata]**, quindi udibili a orecchio solo entro 1–2 km nei compartimenti prodieri.

### 1.4 Tonfi di sonoboe, siluri vicini

- Sonoboe: nessuna fonte aperta trovata sul rumore d'impatto in acqua. **[STIMA]** non affidabile: un oggetto che cade e si inabissa ha un transitorio a bassa e media frequenza; con K ≈ 91–96 dB serve almeno ~130–140 dB re 1 µPa all'esterno per superare il fondo in centrale: solo a poche centinaia di metri. Proposta: udibile a orecchio solo entro ~200 m e solo con barca silenziosa.
- Siluri: dal vecchio manuale *Introduction to Sonar* (NAVPERS 10130-B), un siluro in avvicinamento su cuffia è "un suono come di rumore di elica", "aumenta rapidamente", assomiglia al "sibilo di un motore a reazione, ma meno acuto"; si sente prima "l'aria che sfugge" quando lo lancia il tuo sommergibile. [V], ma **in cuffia, non a orecchio**. A orecchio: [NV], solo narrativa.
- Un siluro lanciato da un tubo del proprio battello si sente (e si sente la spinta) — è il suono del proprio lancio, con la rumorosa uscita d'aria compressa [V qualitativo: Tambor, "tutti sentono il familiare lieve strappo"; nessun livello].

### 1.5 Esplosioni e cariche di profondità

| Affermazione | Fonte | Stato | Conf. |
|---|---|---|---|
| Sequenza udita: avvicinamento (rombo/eliche) → "attraverso lo scafo" lo **splash** delle cariche → attesa dell'affondamento (30 s a 268 piedi) → botto + scossone. "Tre esplosioni a breve distanza frantumarono i vetri dei manometri e fecero volare i pezzi di sughero di isolamento." "Lo shock era come un immenso martello contro lo scafo". | Schultz e Shell 2009 (Tambor) | [V] | alta |
| Due cariche inesplose (duds) "abbastanza vicine da essere udite cadere nell'acqua"; alcuni ricordarono di averle sentite posarsi sul ponte, rotolare e rimbalzare contro il fianco. | stesso articolo | [V] | alta |
| USS Seal (SS-183), 4 mag 1943: "un clic appena prima dello scoppio" — era il meccanismo di innesco; "Click … KaBLOOM!"; vetro dei manometri in frantumi, lampadine fuse, sughero e vernice in volo. Il "clic" è un'informazione che a Smith aveva dato "qualcuno" prima: è dunque di seconda mano. | Ron Smith, *The Silent Service: A Submariner Aboard the USS Seal*, Warfare History Network, primavera 2013 (adattato da Smith e Whitlock, *The Depths of Courage*). https://warfarehistorynetwork.com/article/the-silent-service-a-submariner-aboard-the-uss-seal/ (via Wayback) | [V] il testo; la causa del clic [NV] | media |
| Cariche lontane, stessa pattuglia: "il debole rimbombo di cariche di profondità lontane riverberò attraverso tutto il sommergibile" (non per loro: "troppo lontane"). | stesso | [V] | media-alta |
| "Anche senza cuffie, gli uomini sentirono il San Clemente Maru spezzarsi mentre affondava… poi le caldaie scoppiarono… scoppiettii e crepitii mentre le paratie cedevano." | stesso | [V] | alta |
| Rumore di un relitto che si frantuma: "un forte crepitio... il suono dello scafo di una nave che si spezza" sentito a orecchio attraverso lo scafo (Tambor). | Schultz e Shell | [V] | alta |
| Danno ravvicinato (stime di guerra per cariche britanniche Mk VII da 290 lb): 20 piedi (6 m) per spaccare uno scafo di 7/8 pollice (amatolo); Torpex/Minol 26 piedi (8 m); 52 piedi (16 m) per costringere a emergere. Mk 9 americana: 200 lb di Torpex. | Wikipedia, *Depth charge*, che cita Campbell 1985. https://en.wikipedia.org/wiki/Depth_charge | [V] dato enciclopedico | media |
| Picco di pressione dell'esplosione (200 lb di esplosivo tipo TNT): a 100 m ≈ 244 dB re 1 µPa picco (1,6 MPa), a 300 m ≈ 233, a 1 km ≈ 221, a 3 km ≈ 211. Con la formula empirica di Cole, P(psi) ≈ 21.600·(W^(1/3)/R)^1,13 (W in libbre, R in piedi): l'esponente 1,13 è confermato da fonti aperte, il coefficiente 21.600 lo cito da memoria. Oltre 1–3 km è un'estrapolazione. | mio calcolo da formula Cole 1948 (*Underwater Explosions*, Princeton) | **[STIMA]**, coefficiente [NV] | bassa-media |
| Traduzione: l'urto è dominato dalla frusta e dall'accelerazione dello scafo (non solo dall'acustica). Entro ~1 km è un colpo da "martello"; a 3–10 km è ancora un tonfo netto (il picco equivalente interno resta oltre 100 dB), a decine di km "un rimbombo debole" come nella pattuglia di Seal. | [D] dalle due righe sopra | [D] | media |

### 1.6 Un altro sommergibile molto vicino

- Nessuna testimonianza di prima mano trovata che lo si senta a orecchio. Un sommergibile silenzioso a 100–300 m ha SL di 110–130 dB nelle bande basse **[NV/STIMA]**, RL ≈ 60–90 dB: tutto sotto la soglia utile con K ≈ 91 dB. **[D]**: udibile solo per contatto, collisione o strisciamento, o per rumori transitori ad alto livello (pompe, scarichi d'aria, una porta del tubo lanciasiluri, sonar attivo).
- Un racconto di blog (ripreso su Goodreads) di un "mostro" che scivola lungo lo scafo al largo delle Hawaii non è verificabile e non va usato come fonte.

### 1.7 Balene e gamberi

- **Gamberi pistola** (Alpheidae): il loro crepitio disturbò i sonar americani nella WWII; l'Università della California, Division of War Research, ne identificò la causa; la Marina fece un disco di addestramento per distinguerlo. Fonte: WHOI, *Eavesdropping on Shrimp's Snap Chat*, https://www.whoi.edu/oceanus/feature/snap-chat-eavesdropping-on-snapping-shrimp-communication/ [V], media-alta. Era un suono **da cuffia/sonar**; nessuna fonte che lo sentano a orecchio nudo dentro lo scafo (**[NV]**). [STIMA]: banda larga a 2–20 kHz, a K ≥ 100 dB resta sotto la soglia anche nei branchi densi → solo in cuffia/idrofono.
- **Balene**: nessuna fonte aperta trovata di equipaggi che le sentano a orecchio dentro lo scafo. **[STIMA/NV]**: il richiamo a 20 Hz della balenottera comune ha SL ~180–190 dB re 1 µPa @ 1 m (valore da memoria della letteratura; fonte da verificare: Charif et al. 2002, Watkins 1987); a 100 m → RL 140–150 dB → ~50–60 dB a 20 Hz all'interno, dove la soglia di udibilità è circa 78 dB SPL: **inudibile a orecchio**. I clic del capodoglio (broadband, SL fino a ~230 dB, valore da memoria [NV]) a poche centinaia di metri darebbero all'interno tick di 60–70 dB di picco: possibili, non documentati. Proposta di gioco: balene e gamberi solo sul sonar/idrofono, non attraverso lo scafo.

---

## 2. Funzione di trasferimento acqua → scafo → interno

### 2.1 Che cosa è verificato

1. **Interfaccia acqua–aria nuda**: la frazione di intensità trasmessa a incidenza normale è 1−R² ≈ 0,0012, cioè **−30 dB**; il livello in aria equivale al livello in acqua meno **61,5 dB** a parità di intensità (differenza di riferimento 26 dB più rapporto di impedenze 35,6 dB). In pratica le misure non danno esattamente 30 dB perché onda non piana, incidenza non normale, superficie increspata. T. G. Leighton, *How can humans, in air, hear sound generated underwater (and can goldfish hear their owners talking)?*, JASA 131(3), 2539–2542 (2012), DOI 10.1121/1.3681137. https://resource.isvr.soton.ac.uk/staff/pubs/PubPDFs/Pub12693.pdf (letto via Wayback). [V], alta. Nello stesso articolo: impulsi sonar a 6 kHz di 20 kPa (picco) "si sentono chiaramente sul microfono in aria" di una videocamera: qui senza una lastra d'acciaio di mezzo.
2. **Lastre d'acciaio immerse**: la trasmissione dipende dall'angolo e dal prodotto frequenza × spessore (R. D. Fay e O. V. Fortier, *Transmission of Sound through Steel Plates Immersed in Water*, JASA 23, 1951; abstract su https://labs.sonicfield.org/library/transmission-of-sound-through-steel-plates-immersed-in-water). Il testo integrale non è stato letto: [V] solo l'abstract, nessun valore numerico utilizzabile. Un'altra fonte: per una lastra acqua-acqua di un duomo sonar la perdita è <3 dB a f < fc/4 (DRDO Science Journal, https://publications.drdo.gov.in/ojs/index.php/dsj/article/download/6194/3299; [NV] il contesto non è il nostro).
3. **Perdita di trasmissione della struttura navale**: a bassa frequenza prevale la flessione; ad alta frequenza vale la legge di massa; il carico del fluido conta (Connelly, Craik, Steel, *Sound transmission through ship structures using statistical energy analysis*, Inter-Noise 75th Anniversary/Proc. IOA; https://www.ioa.org.uk/system/files/proceedings/t_connelly_rjm_craik_ja_steel_sound_transmission_through_ship_structures_using_statistical_energy_analy.pdf). [V] qualitativo, nessun valore da riusare.

### 2.2 Hull di riferimento (parametri)

HY-80: densità ≈ 7.850 kg/m³, velocità longitudinale ≈ 5.100 m/s, E ≈ 2,1·10¹¹ Pa [NV: valori standard dell'acciaio]. Spessore: i 3–6 cm indicati dal committente; lo spessore reale del 688 non l'ho verificato. Massa superficiale: 236 kg/m² (3 cm), 353 kg/m² (4 cm, caso usato per la tabella), 471 kg/m² (6 cm). Frequenza di coincidenza (per lastra libera): in aria ≈ 270 Hz (4 cm); **in acqua (c = 1.500 m/s) ≈ 5 kHz (4 cm)**, 3,8 kHz (6 cm), 7,6 kHz (3 cm). Frequenza anello di un cilindro di raggio 4–5 m: f_r = c_L/(2πa) ≈ 160–200 Hz **[STIMA]**.

### 2.3 Calcolo (STIMA, modello "lastra a sola massa", incidenza normale)

Intensità trasmessa da acqua a aria attraverso una lastra di massa superficiale m: T = 4·Zw·Za / |Zw + Za + iωm|², con Zw = 1,5·10⁶ rayl, Za = 415 rayl. Livello interno (aria, re 20 µPa) = livello esterno (re 1 µPa) − 61,5 − TL, TL = −10 log T. Per **4 cm**:

| f (Hz) | TL (dB) | K = 61,5 + TL (dB) |
|---|---|---|
| 50–250 | 29,6–30,1 | 91–92 |
| 500 | 31,5 | 93 |
| 1.000 | 34,6 | 96 |
| 2.000 | 39,5 | 101 |
| 4.000 | 45,1 | 107 |
| 8.000 | 51,1 | 113 |

(3 cm: HF circa 3–4 dB meno; 6 cm: circa 2–3 dB più; sotto 250 Hz non cambia.) Dunque: **passa basso**, piatto fino a ~500 Hz, poi ~5–6 dB/ottava (legge di massa). **[STIMA]**, alta nella forma, ±10–15 dB nel valore assoluto.

### 2.4 Correzioni da applicare al numero base K(f)

Tutte **[D/STIMA]**; nessuna è misurata nel mio materiale:

- **Riverbero interno**: la potenza irradiata dalla lastra si accumula nel compartimento: il livello medio nel locale è più alto di 0–9 dB rispetto al campo "libero" accanto alla lastra (dipende dall'assorbimento: Clampton 1994 misura assorbimento di 50 mm di fibra di vetro 0,5–1 sopra 250 Hz e quasi nulla sull'acciaio nudo, 0,01–0,03). → riduce K di 0–9 dB.
- **Risonanze della struttura** (ordinate/costole, anelli, coincidenza in acqua intorno a 3–8 kHz, risonanze di pannelli e porte stagne): picchi in cui la trasmissione migliora anche di 10–20 dB (K più basso). Per il gioco: un leggero "ring" casuale per stazione.
- **Sotto la frequenza dell'anello (~170 Hz)** la struttura è meno rigida e ha modi di respiro: trasmette più energia del modello a massa, quindi basse frequenze (rimbombo delle eliche, tonfi delle esplosioni) passano meglio. Per le esplosioni, prevale l'accelerazione dell'intero scafo (non è acustica d'aria).
- **Incidenza**: la parte non normale va peggio a bassa frequenza e meglio in coincidenza.
- Intervallo pratico suggerito: **K ≈ 80–105 dB a ≤1 kHz, 95–115 dB a 4–8 kHz**.

### 2.5 Controllo incrociato con le misure sui ping del proprio sonar (Harris 1971)

Un trasduttore da ~220–230 dB re 1 µPa @ 1 m (ipotesi mia) a 3–5 m dallo scafo prodiero dà all'esterno ~210–220 dB; K(4 kHz) = 107 dB ⇒ 103–113 dB all'interno. Le misure sulla Tigrone sono 93–108 dB. **Accordo entro l'ordine di grandezza** (e l'ipotesi SL è mia): dà un po' di fiducia al modello, non è una validazione. [STIMA], media.

### 2.6 Dipendenza dalla profondità

- La massa della lastra e la trasmissione acqua→aria non cambiano in modo apprezzabile con la pressione idrostatica: la compressione dello scafo sposta le frequenze di modo di poco (**[STIMA]**, <1–2 dB, nessuna fonte).
- Quello che cambia con la profondità è altro: (i) per sorgenti in aria o in superficie, l'angolo del cono e l'attenuazione geometrica (§3); (ii) l'assorbimento e i canali di propagazione; (iii) il rumore interno (velocità, regime delle pompe), che a profondità e velocità maggiori è più alto.
- Per le sorgenti subacquee (navi, cariche, ping) la profondità della barca quasi non conta per il livello trasmesso attraverso lo scafo: conta la distanza e il canale.

### 2.7 Da una stazione all'altra

| Stazione | Fondo interno (fonte) | Particolarità | Conf. |
|---|---|---|---|
| **Centrale / locale di manovra** | criteri di progetto 60–65 dBA per locali "quieti" (Clampton 1992; i locali con personale che deve comunicare: "il rumore non deve ostacolare la comunicazione chiara"), 70 dB come criterio NAVSHIPS 1970 per gli alloggi [Harris 1971]; assetto silenzioso: molto meno | esposto sul lato dei tubi di ventilazione, dove il rumore delle ventole passa dai condotti | media |
| **Locale siluri di prua** | simile alla centrale a macchine spente | vicino al trasduttore sonar: misure di **93–108 dB** per i ping propri (Tigrone), e "tutta la parte prodiera" è la più esposta (Harris 1971); tubi lanciasiluri pieni d'acqua con la porta interna sottile: **[STIMA]** trasmissione più diretta (nessuna fonte aperta letta) | media |
| **Sala macchine / compartimento macchine** | misurato **87 dBA** totale in un compartimento macchine di una barca RN (ventole 81 dBA), obiettivo di progetto 85 dBA (Clampton 1992); nei diesel, "comunemente ≥90 dBA" (Harris 1971); a macchine in marcia nessun suono esterno sotto ~90 dB circa si sente | maschera tutto; vedi il caso Gunnel | alta |
| **Cuccette/mensa** | criterio 70 dB (NAVSHIPS 1970; 2 dev. std sopra la media di un campione di sommergibili: la media vera è dunque sotto 70 dB, valore preciso non dato) | | media |

Fonti:
- S. J. Clampton, *Fan and ventilation duct noise in submarines*, INCE Symposium "Fan Noise", CETIM Senlis, 1–3 sett 1992, pp. 431–438 (VSEL, Barrow). https://www.aivc.org/resource/fan-and-ventilation-duct-noise-submarines — PDF: https://www.aivc.org/sites/default/files/airbase_10228.pdf [V], alta per i valori citati (87, 81, 85, 60, 65 dBA).
- S. J. Clampton, *Acoustic absorption in submarine compartments*, Proc. IOA 16(2), 1994, p. 545 e segg. (via Wayback: https://www.ioa.org.uk/system/files/proceedings/sj_clampton_acoustic_absorption_in_submarine_compartments.pdf) [V]; la tabella 5 (livelli per ottave in vari locali) mescola misure e calcoli e non è un fondo ambiente: non la uso.
- J. D. Harris, *Hearing Conservation and Acoustic Habitability for Submariners*, NSMC Report 682, 13 ott 1971, DTIC ADA349945 [V], alta.
- P. F. Smith, M. S. Harris, J. S. Russotti, C. K. Myers, *Effects of Exposure to Intense Low Frequency Tones on Hearing and Performance*, NSMRL/Naval Submarine Medical Center Report 610, 1970, DTIC AD0707364: locali con toni a bassa frequenza di 70–300 Hz a ~113 dB SPL sono abitabili per 24 h. https://apps.dtic.mil/sti/pdfs/AD0707364.pdf (via Wayback) [V], alta. Utile per sapere che a livello di gioco i rimbombi a bassa frequenza a ~100–110 dB sono tollerabili senza danno immediato.

### 2.8 Soglia di udibilità (da tenere in conto nel mascheramento)

Soglia assoluta binaurale ISO 226:2003 (dB SPL), **da memoria, non riverificata**: 20 Hz ≈ 78; 50 Hz ≈ 44; 100 Hz ≈ 26; 250 Hz ≈ 11; 1 kHz ≈ 2; 4 kHz ≈ −5; 8 kHz ≈ 13. Ne segue che a 20–50 Hz un livello interno di 50–60 dB è ancora sotto soglia, mentre a 4 kHz bastano pochi dB sopra il fondo spettrale (basso in quella banda). **[NV]**, media.

---

## 3. Aerei ed elicotteri: si sentono a orecchio?

**Risposta onesta: no, non a orecchio, non a quota periscopica né in profondità. Si rivelano sullo strumento, per pochi secondi, solo quando sono quasi sulla verticale.**

### 3.1 Cosa è verificato

| Affermazione | Fonte | Stato | Conf. |
|---|---|---|---|
| Rapporto velocità del suono aria/acqua ≈ 0,22: angolo critico d'incidenza **13°**. Il rumore dell'aereo passa in acqua solo per incidenza < 13°; oltre, è riflesso. L'area ensonificata è la base di un cono di semiangolo 13°. | Ferguson, *International Student Challenge Problem in Acoustic Signal Processing 2019*, Acoustics Today 15(1), 71–73. https://acousticstoday.org/wp-content/uploads/2019/03/International-Student-Challenge-Problem-in-Acoustic-Signal-Processing-2019-Brian-G.-Ferguson-1.pdf | [V] | alta |
| Riferimento originale: R. J. Urick, *Noise signature of an aircraft in level flight over a hydrophone in the sea*, JASA 52, 993–999 (1972). Urick prevede fino a quattro contributi: rifrazione diretta, una o più riflessioni sul fondo, onda evanescente (laterale), scattering da superficie increspata. | stesso documento, bibliografia | [V] il riferimento e le pagine; il testo di Urick non l'ho letto | alta |
| Un turboprop a 239 kn e 496 piedi (151 m) di quota, idrofono a 20 m: il segnale tonale (frequenza di pala ~63–74 Hz) va da −1,3 s (inbound, 73,8 Hz) a +1,2 s (outbound, 63,2 Hz), cioè **~2,5 s** attorno al passaggio. | stesso documento | [V] | alta |
| "On top" (gergo dei sommergibilisti per un aereo sulla verticale): la ricezione diretta "dura un paio di secondi", dipende da quota dell'aereo e profondità del ricevitore, e non dà preavviso. Il percorso diretto è più forte di quello riflesso dal fondo di 20–30 dB. | Acoustics Today, primavera 2019, p. 14 ("Acoustic Systems for Defense"; autore non leggibile nel testo recuperato). https://acousticstoday.org/issues/2019AT/Spring2019/files/basic-html/page14.html | [V] | media-alta |
| Un array rimorchiato da un sommergibile rivela aerei a grande distanza via riflessione sul fondo (preavviso di "molti minuti"); sistema US Navy ATOMS derivato da prove 1987. | stessa pagina; B. G. Ferguson e G. C. Speechley, *Acoustic detection and localization of an ASW aircraft by a submarine*, US Navy J. Underwater Acoustics 39, 25–41 (1989) [riferimento V, testo NV] | [V] | media-alta |
| Ferguson e Speechley 2009: l'energia raggiunge i sensori sommersi per due percorsi: riflessione sul fondo (rivelazione a grande distanza) e **percorso diretto "presente solo quando l'aereo passa sulla verticale"**. IEEE J. Oceanic Eng. 34, 75–82, DOI 10.1109/JOE.2008.2011173. | abstract da OpenAlex/IEEE | [V] | alta |
| Penetrazione dell'onda piana dall'aria all'acqua: ray theory (Young 1971, JASA 50, 1392): P₂/P₁ = 2·r₁·(c₁/c₂)/(c₁·h/c₂ + d), cioè sorgente virtuale a ~h/4,4 sopra la superficie, con raddoppio di pressione all'interfaccia. L'onda evanescente conta solo vicino alla superficie. | A. I. Eller e R. C. Cavanagh, *Subsonic Aircraft Noise At and Beneath the Ocean Surface*, USAF AFRL-HE-WP-TR-2000-0156 (giu 2000), DTIC ADA395453, App. A.6. https://apps.dtic.mil/sti/tr/pdf/ADA395453.pdf (via Wayback) | [V] | alta |
| Livelli in aria misurati dal modello: F-18 a 250 kn 164 dB re 1 µPa (=138 re 20 µPa) a 61 m, 153,8 a 152 m; B-1 a 360 kn 139,6 a 61 m; E-8A (J-STARS, jet quadrimotore) 136,7 a 61 m, 129,6 a 152 m. | stesso, tabella 3.3-4 | [V] (livelli in aria convertiti +26 dB) | media-alta |
| Misure reali in acqua: aerei militari a Whidbey Island, Washington (soprattutto EA-18G Growler in decollo a 120–190 m di quota): **134 ± 3 dB re 1 µPa rms a 33–35 m di profondità** (in aria 110 ± 4 dB re 20 µPa). | L. M. Kuehne, C. Erbe, E. Ashe, L. T. Bogaard, M. Salerno Collins, R. Williams, *Above and below: Military Aircraft Noise in Air and under Water at Whidbey Island, Washington*, J. Mar. Sci. Eng. 8(11), 923 (2020). https://tethys.pnnl.gov/sites/default/files/publications/jmse-08-00923-v2.pdf | [V] | alta |
| Altri dati: aerei di linea in acqua 84–132 dB re 1 µPa (Erbe e coll., Marine Pollution Bulletin 2018, "Underwater noise from airplanes: an overlooked source of ocean noise"); droni a 5–30 m: ≈86 dB re 1 µPa. | sintesi del motore di ricerca; articolo non letto | [NV] | media |

### 3.2 Calcolo (STIMA) per un aereo sulla verticale

Ray theory di Young con h = 150 m: a profondità d = 18 m (periscopica) il livello in acqua è ≈ +2,4 dB rispetto al livello in aria misurato a distanza h; a d = 100 m è −5,8 dB; per d→0 +6 dB. Esempio con un jet quadrimotore (E-8A, 129,6 dB re 1 µPa a 152 m): ≈ 132 dB re 1 µPa a d = 18 m. All'interno: 132 − 91 ... 96 ≈ **36–41 dB** (banda larga, dominata dai bassi): al di sotto del fondo in centrale e quasi uguale alla soglia di udibilità a 50 Hz. Per i Growler di Whidbey (134 dB a 33 m) ≈ 38–43 dB. Anche con K ridotto di 15 dB (risonanza favorevole) resterebbero ≈ 55 dB solo per ~2 s. **[STIMA]**, media: l'ordine di grandezza è robusto, la cifra no.

Elicottero: nessun livello subacqueo dedicato trovato in fonti aperte (cercati: Richardson e coll. 1995 *Marine Mammals and Noise*, Greene; non leggibili qui). Le frequenze tipiche (pale a ~10–25 Hz di rate, rotore di coda 50–100 Hz) cadono dove la soglia è alta e la K è piatta: l'esito qualitativo non cambia **[D]**.

### 3.3 Che cosa conviene fare nel gioco

- Aerei ed elicotteri **solo su sonar passivo/idrofono**, con la finestra di ~2–3 s sotto la verticale e il segnale (blade rate + armoniche) con Doppler.
- A orecchio, solo se la barca è **in superficie** (o con l'albero/periscopio fuori): allora si sentono come in aria.
- Se si vuole un rimbombo "attutito" dentro lo scafo per atmosfera, tenerlo ≤ 40 dB, solo a macchine spente, e sapere che non è realismo documentato.
- Elicotteri con il sonar a immersione in hovering: si "vedono" sul sonar passivo come un cacciatorpediniere a bassa velocità (Steele 1960, V), e poi si sentono i loro ping come quelli delle navi (§1.2–1.3).

---

## 4. Campioni di aerei con licenza libera

Tutte le licenze sotto sono state lette sulle pagine dei file il 25 ottobre 2026. Le licenze su Freesound sono **dichiarate dall'uploader**; conservare una copia del file e la URL con data. Le durate e le frequenze sono quelle riportate dalla pagina.

### 4.1 Freesound — CC0 (nessun obbligo, uso in gioco gratuito e commerciale consentito)

Licenza: Creative Commons 0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/).

| Soggetto | File (link) | Durata | Note |
|---|---|---|---|
| Elicottero RAF **Chinook**, sorvolo notturno, "rotor slap" | https://freesound.org/people/surrey_film/sounds/84472/ (surrey_film) | 22,8 s, 48 kHz stereo | rotore tandem, il battito tipico |
| **UH-1 Huey**, flyby, sorvolo e atterraggio su una nave | https://freesound.org/people/Rmutt/sounds/156906/ (Rmutt) | 2:14, 44,1 kHz stereo | riflessioni dalla nave e dal magazzino |
| **Sea King** britannico, sorvolo e atterraggio | https://freesound.org/people/Sandermotions/sounds/333505/ (Sandermotions) | 1:23, 48 kHz stereo | |
| **Sea King** rescue, flyby | https://freesound.org/people/SirWilliam77/sounds/443427/ | 30,8 s, 48 kHz mono | |
| Elicottero in atterraggio, folle (idle) | https://freesound.org/people/Fission9/sounds/693866/ | 2:24,8, 96 kHz stereo | file da 79,5 MB |
| **Black Hawk** (dimostrazione) | https://freesound.org/people/John%20Sipos/sounds/156678/ | 43,9 s | |
| **UH-60 Black Hawk**, hover loop | https://freesound.org/people/qubodup/sounds/854493/ | 2,6 s mono, loop | derivato da una registrazione dell'esercito USA; provenienza PD non verificata da me |
| **Bell 407** | https://freesound.org/people/bzamecnik/sounds/348991/ | 50,2 s | timbro ricco di armoniche |
| Elicottero che si avvicina da lontano | https://freesound.org/people/neilraouf/sounds/437148/ | 1:22, 48 kHz | utile per il "lontano" |
| Elicottero distante che passa | https://freesound.org/people/Filmscore/sounds/823082/ | 49,5 s, 48 kHz | "basso drone con lieve doppler"; utile per ovattare |
| Rotore militare in **loop** | https://freesound.org/people/UnderlinedDesigns/sounds/172667/ | 0,9 s mono | loop perfetto |
| **Lockheed L-188 Electra** (turboprop quadrimotore, parente civile del P-3 Orion; il P-3 ne deriva e usa gli Allison T56) | https://freesound.org/people/HootOwl/sounds/464893/ | 32,6 s, 44,1 kHz stereo | il più vicino al P-3 che ho trovato con licenza libera |
| **C-130 Hercules** (turboprop, T56) a distanza ravvicinata | https://freesound.org/people/Sandermotions/sounds/330816/ | 17,9 s, 48 kHz stereo | |
| Jet piccolo bimotore | https://freesound.org/people/rickbuzzin/sounds/131315/ | 30,3 s | |
| Aereo di linea in atterraggio | https://freesound.org/people/tbaucom/sounds/648468/ | 25,3 s | |
| Caccia F-18 (probabile), passaggio a bassa quota | https://freesound.org/people/JoanCalsina/sounds/851222/ | 58,9 s mono | |
| F-15 sorvolo | https://freesound.org/people/klangfabrik/sounds/324370/ | 30,6 s | |
| Due F-16 RNLAF in decollo | https://freesound.org/people/Rudmer_Rotteveel/sounds/343747/ | 1:11 | con uccelli di sottofondo |

Uso in un gioco gratuito distribuito: **permesso** (CC0 rinuncia ai diritti nella misura consentita). Avvertenza: la dichiarazione CC0 è dell'uploader; il rischio è che il file contenga materiale altrui (per esempio il Black Hawk loop è derivato).

### 4.2 Freesound — CC BY (attribuzione obbligatoria)

| Soggetto | File | Licenza | Durata | Obblighi |
|---|---|---|---|---|
| Turboprop che passa sopra, Ekaterinburg | https://freesound.org/people/newlocknew/sounds/610271/ | CC BY 4.0 | 1:42 | credito all'autore, titolo, link, licenza; indicare le modifiche |
| Elicottero lontano (tranquillo) | https://freesound.org/people/Speziax/sounds/645330/ | CC BY 4.0 | 48,9 s | idem |
| Boeing di linea, flyby (Gatwick) | https://freesound.org/people/InspectorJ/sounds/428086/ | CC BY 4.0 | 47,9 s | l'autore chiede esplicitamente il credito |

Uso in un gioco gratuito distribuito: **permesso con attribuzione** (https://creativecommons.org/licenses/by/4.0/). Prevedere un file `CREDITS` nel gioco.

### 4.3 Wikimedia Commons

| File | Licenza (Commons) | Note |
|---|---|---|
| Chinook helicopter flying over Greenwich in London.ogg (JoBrodie, 2016) — https://commons.wikimedia.org/wiki/File:Chinook_helicopter_flying_over_Greenwich_in_London.ogg | **CC0** | permesso, nessun obbligo |
| Helicopter over a lake.ogg, Helicopter over quiet neighbourhood.ogg (ezwa, 2007–2008, da pdsounds.org) — https://commons.wikimedia.org/wiki/File:Helicopter_over_a_lake.ogg | **Public domain** (dichiarato dall'autore tramite pdsounds) | mono, 22–45 s, qualità bassa (da MP3) |
| 2011-06-05 19-32 Berlin TXL Airplane Flyover plus Wingtip Vortex.ogg (Fritz Schlüter) | CC BY-SA 3.0 | **ShareAlike**: modifiche del suono vanno rilasciate con la stessa licenza; da evitare o da isolare |
| Mil Mi-8MTV-1 helicopter starting up.ogg (H.dav.are, 2026) | CC BY-SA 4.0 | idem |
| File:Helicopter flying overhead.wav | "Attribution" (dicitura Commons ambigua) | non l'ho disambiguata: da non usare finché non si controlla la versione di CC BY |

La ricerca Commons ha restituito pochissimo materiale di aerei militari con licenze utili.

### 4.4 BBC Sound Effects (RemArc) — **sconsigliato per un gioco distribuito**

Il testo della licenza "BBC's Content Licence for RemArc" (versione 2016, dal repository ufficiale BBC): https://github.com/bbcarchdev/Remarc/blob/master/doc/2016.09.27_RemArc_Content_licence_Terms_of_Use_final.pdf (letto per intero; il sito sound-effects.bbcrewind.co.uk/licensing è un'app JavaScript e non l'ho potuto leggere, quindi **non ho verificato se i termini attuali sul sito coincidono**).

Punti decisivi, [V]:
- §7: uso permesso **solo** "per scopi non commerciali, personali o di ricerca" e per istruzione formale come studente o personale di una scuola.
- §4h: **vietato "condividere i nostri contenuti"**, esempio "niente caricamenti su siti social"; condividere link va bene. Distribuire i file audio dentro un gioco scaricabile è una forma di condivisione.
- §4a: i contenuti vanno rimossi "quando lo chiediamo"; possono essere ritirati "in qualsiasi momento, senza preavviso" (licenza revocabile).
- §4d: non si può fare "un servizio che contenga solo i nostri contenuti"; §4f vieta di associarli a pubblicità e sponsorizzazioni; §5 impone di usare "l'ultima versione" e il credito "bbc.co.uk – © copyright [anno] BBC".
- §8: per qualsiasi altro uso (lavoro, scopo commerciale, organizzazioni non profit, enti pubblici) serve permesso, con possibile tariffa.

**Verdetto [D]**: un gioco gratuito distribuito al pubblico con i file BBC dentro non è coperto con sicurezza (il termine "condividere" lo esclude in modo ragionevole). Usabili al massimo come riferimento di ascolto personale, non da includere. Confidenza: media-alta (non ho un parere legale).

### 4.5 Archivi della US Navy / pubblico dominio

- I lavori dei dipendenti federali USA sono in pubblico dominio (17 U.S.C. §105), ma non ho trovato un archivio audio navale citabile di elicotteri o P-3 con singoli file e dichiarazione di pubblico dominio. Un uploader di Freesound (qubodup, file 854493) dichiara di derivare un hover loop da una registrazione di una unità dell'esercito USA (probabilmente pubblicata da DVIDS): **non verificata la licenza della fonte**.
- NASA e simili non hanno materiale pertinente. Se si vuole, un caso reale da approfondire: DVIDS (https://www.dvidshub.net) con filtro "Unrestricted/public domain" per i file audio di SH-60 o P-3; non l'ho fatto.

---

## 5. Registrazioni di suoni interni di sommergibile con licenza libera

| Risorsa | Licenza | Contenuto | Note |
|---|---|---|---|
| Pacchetto Freesound **"Vintage Submarine Effects"** di craigsmith, 12 file — https://freesound.org/people/craigsmith/packs/24916/ | **CC0** (verificata su ogni file) | G45-24 interno sala macchine (2:13), G45-25 caricabatterie (0:54), G45-26 avviamento motori (1:16), G45-27 interno centrale (0:21, con scheda tecnica parlata), G45-28 annunciatore (0:26), G45-29 avviamento motore (1:23), G45-30 apertura portello (0:16), G45-31 "churn" subacqueo (0:22), **G45-32 grande interno** (1:10), **G45-33 interno sala siluri prodiera** (0:24), G45-34 siluri riverberati (2:08), **G45-35 esplosioni subacquee distanti** (1:43, 26 eventi), 48 kHz mono | Descrizione dell'uploader: copie di prima generazione degli originali ottici su nitrato **anni '30–'40 creati per gli studi hollywoodiani**, raccolti da un montatore del suono, donati all'USC, trasferiti su nastro nei primi anni '70, digitalizzati da lui ("non restaurati"). Esempio: https://freesound.org/people/craigsmith/sounds/438735/ . **Non è dimostrato che siano registrazioni a bordo di un battello reale** (potrebbero essere effetti da studio); la CC0 è dichiarata dal digitalizzatore, il copyright sugli originali del 1930–40 è dubbio. Rischio: medio-basso. |
| Freesound "Submarine internal" (blaukreuz), 4:06, 5.1 | CC0 | ambiente progettato di una cabina di sommergibile ricerca | sintetico/progettato, non militare. https://freesound.org/people/blaukreuz/sounds/465472/ |
| Archive.org "NAVAL WARFARE EFFECTS – Submarine Depth Charges" (disco a 78 giri digitalizzato) | **non stabilita** | cariche di profondità da disco di effetti sonori | https://archive.org/details/78_naval-warfare-effects-submarine-depth-charges_gbia3000791a : da non usare finché non si stabilisce il copyright |

Registrazioni **documentate a bordo di un sommergibile militare reale** con licenza libera: **non ne ho trovate**. Un candidato da esplorare è un museo (USS Pampanito, USS Bowfin, USS Nautilus) con registrazioni proprie: da chiedere direttamente, fuori portata qui.

---

## 6. Lacune e cose NON verificate

- Testi originali di Sontag e Drew (*Blind Man's Bluff*) e di *Hunter Killers*: non letti; solo una recensione per l'episodio Gudgeon.
- Forum di veterani citati dal committente (subvetpaul, rontini): non raggiunti con la ricerca.
- Articoli della *Submarine Review* rilevanti trovati (USS Gunnel 2007; Steele 1960; Depth Charge parte II 2003); nessun articolo che descriva i ping di un elicottero a immersione udito dentro la barca.
- Fay e Fortier (1951): solo l'abstract; Urick (1972): solo bibliografia e abstract indiretti (non ho letto i livelli); Cole (1948): coefficiente della formula da memoria.
- Rapporti NSMRL sul rumore a bordo dei sommergibili a energia nucleare con misure per compartimento (dBA in centrale, sala macchine): **non trovato un documento aperto** con cifre per stazione. Le cifre disponibili sono quelle sopra (Harris 1971; Clampton 1992/94; NSMRL 610).
- Frequenze e livelli di AQS-13/18, pinger delle sonoboe attive, sonar di siluro: non trovati.
- Il testo del manuale *Fleet Type Submarine* sul sonar (maritime.org): non recuperato.
- BBC: non ho potuto leggere i termini correnti sul sito; ho letto la versione 2016 sul repository GitHub.
- Il valore di spessore dello scafo del 688 e l'HY-80: usati i 3–6 cm del committente.

## 7. Fonti principali (elenco)

- Schultz R., Shell J., *Cornered at the Bottom of the East China Sea*, Naval History 23(6), dic 2009 — https://www.usni.org/magazines/naval-history-magazine/2009/december/cornered-bottom-east-china-sea
- Smith R., *The Silent Service: A Submariner Aboard the USS Seal*, Warfare History Network, primavera 2013 — https://warfarehistorynetwork.com/article/the-silent-service-a-submariner-aboard-the-uss-seal/
- NSL, *USS Gunnel (SS253) First War Patrol*, Submarine Review 2007 — https://archive.navalsubleague.org/2007/uss-gunnel-ss253-first-war-patrol-october-19-1942-december-7-1942
- NSL, *Depth Charge: An Early ASW Weapon, Part II*, 2003 — https://archive.navalsubleague.org/2003/depth-charge-an-early-antisubmarine-warfare-weapon-part-ii-world-war-i
- Steele G. P., *Killing Nuclear Submarines*, Proceedings nov 1960 (rist. in NSL) — https://archive.navalsubleague.org/2004/killing-nuclear-submarines
- Harris J. D., NSMC Report 682 (1971), DTIC ADA349945 — https://apps.dtic.mil/sti/tr/pdf/ADA349945.pdf
- Smith P. F. e coll., NSMRL Report 610 (1970), DTIC AD0707364 — https://apps.dtic.mil/sti/pdfs/AD0707364.pdf
- Clampton S. J., 1992 (AIVC 10228) — https://www.aivc.org/sites/default/files/airbase_10228.pdf; 1994 (Proc. IOA 16(2)) — https://www.ioa.org.uk/system/files/proceedings/sj_clampton_acoustic_absorption_in_submarine_compartments.pdf
- Leighton T. G., JASA 131, 2539 (2012) — https://resource.isvr.soton.ac.uk/staff/pubs/PubPDFs/Pub12693.pdf
- Ferguson B. G., Acoustics Today 15(1), 2019 — https://acousticstoday.org/wp-content/uploads/2019/03/International-Student-Challenge-Problem-in-Acoustic-Signal-Processing-2019-Brian-G.-Ferguson-1.pdf; Acoustics Today p. 14 — https://acousticstoday.org/issues/2019AT/Spring2019/files/basic-html/page14.html
- Ferguson B. G., Speechley G. C., IEEE JOE 34(1), 75–82 (2009), DOI 10.1109/JOE.2008.2011173
- Eller A. I., Cavanagh R. C., AFRL-HE-WP-TR-2000-0156 — https://apps.dtic.mil/sti/tr/pdf/ADA395453.pdf
- Kuehne L. M. e coll., J. Mar. Sci. Eng. 8, 923 (2020) — https://tethys.pnnl.gov/sites/default/files/publications/jmse-08-00923-v2.pdf
- Muir T. G., Acoustics Today 17(1), 2021 — https://acousticstoday.org/wp-content/uploads/2021/03/Underwater-Acoustics-A-Brief-Historical-Overview-Through-World-War-II-Thomas-G.-Muir.pdf
- BBC RemArc licence 2016 — https://github.com/bbcarchdev/Remarc/blob/master/doc/2016.09.27_RemArc_Content_licence_Terms_of_Use_final.pdf
- Wikipedia: USS Plunger (SS-179); Depth charge; AN/SQS-26.
- Forum SUBSIM: https://www.subsim.com/radioroom/showthread.php?t=203487 ; https://www.subsim.com/radioroom/showthread.php?p=270963
