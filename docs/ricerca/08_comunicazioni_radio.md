# 08 — Comunicazioni radio dei sottomarini, circa 1989 (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. SSN-688 *Los Angeles* Flight I contro SSN sovietici, soprattutto Pr.705 *Alfa*.
> Teatri: Atlantico del Nord, Mar di Norvegia, Barents, Mediterraneo, Pacifico (Curili).
> Scopo: dare numeri citabili per il modello di comunicazioni del gioco: chi trasmette, su quale banda, a che quota e velocità si riceve, quanto ci vuole, quanto si rischia.
> Compilato: 24 ottobre 2026.

---

## 0. Convenzioni e avvertenze

**Unità.** kn = nodi; nmi = miglia nautiche; m = metri (piedi tra parentesi se la fonte è in piedi); baud ≈ bit/s per i segnali binari citati qui.

**Confidenza.**
- **alta**: fonte ufficiale o declassificata, oppure ≥2 fonti indipendenti serie concordi.
- **media**: una sola fonte secondaria seria (FAS, GlobalSecurity, Wikipedia con fonti, riviste di settore).
- **bassa**: fonte debole, stima del compilatore, oppure dato ricordato dalla letteratura e **non riverificato** in questa sessione (*n.v.*).

**STIMA** = valore proposto per il gioco. Non è un dato storico. Va tarato.

**Sigle delle fonti** tra parentesi quadre, es. `[FASVLF]`. Elenco completo con URL nella §10. `[CALC]` = calcolo del compilatore con fisica standard.

**Avvertenza generale.** Profondità e velocità operative di antenne e boe USA sono ancora classificate. Le fonti aperte danno solo ordini di grandezza. Molti numeri delle §3 e §9 sono quindi STIMA.

---

## 1. Quadro generale delle bande

| Banda | Frequenze | Senso | Penetrazione in mare | Velocità dati (1989) | Uso tipico | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| ELF | 76 Hz (USA), 82 Hz (URSS) | solo terra → sottomarino | centinaia di metri | pochi caratteri ogni 15 min | "campanello" (*bell-ringer*) | `[WPELF]` `[WPCOM]` | alta |
| VLF | 14–30 kHz (broadcast USA fino a 60 kHz con LF) | solo terra/aereo → sottomarino | ~10–20 m utili (alcune fonti 10–40 m) | 50 baud per canale | broadcast di comando | `[FASVLF]` `[WPCOM]` | alta |
| LF | 30–60 kHz | solo terra → sottomarino | pochi metri | 50 baud | broadcast, aree costiere | `[FASVLF]` | media |
| HF | 2–30 MHz | bidirezionale | nessuna: antenna fuori acqua | teletype; burst sovietici 500 bit/s | rapporti, ritorno a terra | `[FASBSQ5]` `[WPBOR]` | media |
| UHF SATCOM | 240–400 MHz | bidirezionale | nessuna: albero fuori acqua | SSIXS 4800 baud | traffico ad alta capacità | `[NSL99]` `[WPFLT]` | media |
| UHF/VHF LOS | 225–400 / 30–300 MHz | bidirezionale | nessuna | voce e dati | coordinamento con P-3, navi | `[FASBSQ5]` | media |

**Regola d'oro.** Il sottomarino in immersione **riceve** (ELF, VLF/LF). Per **trasmettere** deve esporre un'antenna sopra l'acqua, cioè stare a quota periscopica. `[SCMP]` `[WPCOM]` — alta.

---

## 2. Broadcast terra → sottomarino, lato USA/NATO

### 2.1 Stazioni VLF/LF attive nel 1989

| Nominativo | Luogo | Freq. | Potenza | Note | Fonte | Conf. |
|---|---|---|---|---|---|---|
| NAA | Cutler, Maine | 24,0 kHz | 750/1000 kW (altre fonti 1,8 MW) | Flotta atlantica; la stazione più potente | `[FASVLF]` `[WPCUT]` | alta (freq.), media (pot.) |
| NLK | Jim Creek, Washington | 24,8 kHz | 192 kW | Flotta del Pacifico (nord-est) | `[FASVLF]` | alta |
| NPM | Lualualei, Hawaii | 21,4 kHz | 480 kW | Pacifico centrale | `[FASVLF]` | alta |
| NSS | Annapolis, Maryland | n.v. | n.v. | Chiusa nel 1996: attiva nel 1989 | `[WPVLFL]` | media |
| NAU | Aguada, Porto Rico | 40,75 kHz | 100 kW | LF, Caraibi/Atlantico | `[FASVLF]` | alta |
| NWC | H.E. Holt, Exmouth (Australia) | 19,8 kHz | n.v. (~1 MW) | Oceano Indiano / Pacifico ovest | `[WPVLFL]` | media |
| NRK / TFK | Grindavík (Keflavík), Islanda | 37,5 kHz | 100 kW | Gap GIUK, Mar di Norvegia | `[REEVE]` `[WPVLFL]` | media |
| GQD | Anthorn, Regno Unito | 19,6 kHz | 500–550 kW | Royal Navy/NATO, "ordini ai sottomarini" | `[WPANT]` | media |
| GBZ | Criggion, Galles | 19,6 kHz *n.v.* | n.v. | Stazione RN storica; nominativo citato spesso | — | bassa (*n.v.*) |
| NSY | Niscemi, Sicilia | 45,9 kHz | n.v. | US Navy, Mediterraneo; data di attivazione *n.v.* | `[WPVLFL]` | bassa |
| SXA | Kato Souli, Grecia | 49,0 kHz | n.v. | Mediterraneo orientale | `[WPVLFL]` | bassa |
| DHO38 | Rhauderfehn, Germania | 23,4 kHz | n.v. | Marina tedesca; anno di entrata in servizio *n.v.* | `[WPVLFL]` | bassa |

**Nota.** L'elenco FAS del 1998 parla di **5–6 siti VLF fissi (FVLF)** ad alta potenza, multicanale MSK, e di **5–7 siti LF**. Il broadcast opera tra **14 e 60 kHz**. `[FASVLF]` — alta.

### 2.2 Formato del broadcast VLF

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Velocità per canale | **50 baud** ("submarine command and control broadcast") | `[FASVLF]` `[NSL99]` | alta |
| Modulazione | MSK (Minimum Shift Keying) | `[FASVLF]` | alta |
| Multiplex | VERDIN: fino a **4 canali da 50 bit/s** sullo stesso trasmettitore | `[FASVLF]` | alta |
| Ciclo | Sequenza continua di messaggi in ordine di precedenza, che **di norma dura 2 ore** | `[FASVLF]` `[GSVLF]` | alta |
| Elaborazione | Messaggi decifrati e caricati nell'**ISABPS** (*Integrated Submarine Automated Broadcast Processor System*) in ogni sito | `[GSVLF]` (estratto di ricerca) | media |
| Capacità oraria | 50 bit/s ≈ 6 caratteri/s ≈ **22.000 caratteri/ora** per canale (lordo, senza codici correttori) | `[CALC]` | media |
| Voce | Impossibile: solo testo | `[WPCOM]` | alta |

### 2.3 TACAMO (relè aereo VLF)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Aereo nel 1989 | **EC-130Q** (12 esemplari dal 1969); in transizione all'**E-6A** | `[WPTAC]` | alta |
| E-6A | Primo volo feb. 1987; prima consegna a VQ-3 **agosto 1989** | `[FASE6]` | alta |
| Antenna | Filo VLF rimorchiato lungo **~8 km** ("circa 5 miglia") con zavorra; l'E-6 ne ha due (lungo e corto) | `[TWZE6]` `[FASE6]` | media |
| Trasmettitore | 200 kW (TACAMO IV dal 1971; amplificatore E-6A da 200 kW) | `[WPTAC]` `[FASE6]` | media |
| Orbita | Virate strette a bassa velocità per tenere il filo quasi verticale | `[TWZE6]` `[FASE6]` | alta |
| Autonomia E-6A | >15 h senza rifornimento; fino a 72 h con rifornimento | `[FASE6]` | alta |
| Profondità di ricezione | Il segnale "arriva a circa 60 ft (18 m)" | `[TWZE6]` | bassa |
| Missione | Ritrasmettere gli **Emergency Action Messages** alle forze strategiche (SSBN). Gli SSN possono ascoltare | `[WPTAC]` | alta / media |

**Per il gioco.** TACAMO serve come broadcast di riserva quando le stazioni a terra sono distrutte. È rilevante solo in scenari di guerra generale.

### 2.4 ELF (Clam Lake e Republic)

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Siti | Clam Lake (Wisconsin), Republic (Michigan) | `[WPELF]` | alta |
| Frequenza | **76 Hz** (MSK tra 72 e 80 Hz); alternativa 45 Hz | `[WPELF]` `[ALT]` | alta |
| Prima trasmissione | **Maggio 1982**: ricevuta da un sottomarino a **400 ft (122 m)** al largo della Florida | `[ALT]` | media |
| Piena operatività | Republic in funzione intermittente dal 1986. **Pienamente operativo a ottobre 1989**. Sistema "nominalmente operativo" nel 1989 | `[ALT]` `[WPELF]` | alta |
| Potenza | 2,6 MW in ingresso; **~8 W irradiati** | `[WPELF]` | media |
| Velocità | Circa **15 minuti per un gruppo di 3 lettere** | `[WPELF]` | media |
| Funzione | *Bell-ringer*: ordina a un battello preciso di salire e copiare un messaggio completo su altra banda | `[WPELF]` `[ALT]` | alta |
| Destinatari | Trident (USA, UK) e **SSN** ("Fast Attack") | `[ALT]` | media |
| Ricevitore 688 | OR-279/BRR, installato sui 688 con SHIPALT A3125; usa il filo galleggiante OE-315. **Data di installazione sui Flight I n.v.** | `[SCMP]` | media (sistema), bassa (data) |

**Conseguenza per il 1989.** L'ELF USA esiste ma è nuovo. Un 688 Flight I del 1989 *potrebbe* non avere ancora il ricevitore. Per il gioco: opzione di scenario, attiva di default solo da fine 1989. STIMA.

---

## 3. Ricezione in immersione

### 3.1 Fisica: profondità di pelle in acqua di mare

Formula: `δ = 1 / sqrt(π · f · μ0 · σ)`, con σ ≈ 4 S/m. L'ampiezza del campo cala di 8,686 dB per ogni δ. `[CALC]` — alta (formula).

| Frequenza | δ (σ = 4 S/m) | Attenuazione | 20 m d'acqua | 100 m d'acqua |
|---|---|---|---|---|
| 76 Hz (ELF USA) | 28,9 m | 0,30 dB/m | 6 dB | 30 dB |
| 82 Hz (ZEVS) | 27,8 m | 0,31 dB/m | 6 dB | 31 dB |
| 19,6 kHz (Anthorn) | 1,80 m | 4,8 dB/m | 97 dB | — |
| 24 kHz (Cutler) | 1,62 m | 5,3 dB/m | 107 dB | — |
| 37,5 kHz (Islanda) | 1,30 m | 6,7 dB/m | 134 dB | — |

Con acqua meno salina (Baltico, foci) σ ≈ 3 S/m e δ cresce di circa il 15%. `[CALC]`

**Lettura.** Ogni metro conta in VLF. Pochi metri in più di antenna sott'acqua costano decine di dB. In ELF 100 m costano solo ~30 dB. Per questo l'ELF passa anche sotto il ghiaccio (il ghiaccio è poco conduttivo). `[CALC]` — media.

### 3.2 Profondità di ricezione dichiarate

| Banda | Profondità dell'antenna | Fonte | Conf. |
|---|---|---|---|
| VLF | "Poco più di 30 m" (introduzione); "alcune decine di metri" | `[WPCOM]` | media |
| VLF | 10–30 m | `[WPELF]` | media |
| VLF | fino a 20 m | `[RUCOM]` | media |
| VLF | 30–100 ft (9–30 m) | presentazione LIMARC `[LIMARC]` | bassa |
| VLF (da TACAMO) | ~60 ft (18 m) | `[TWZE6]` | bassa |
| ELF USA | 400 ft (122 m) nel test del 1982 | `[ALT]` | media |
| ELF/КНЧ sovietico | **100–160 m** | `[RUSNAVY]` | media |
| ELF | "centinaia di metri" | `[WPCOM]` `[WPELF]` | alta |

**Attenzione.** Queste sono profondità **dell'antenna**, non della chiglia. Con il filo galleggiante l'antenna sta alla superficie e il battello può stare più giù. Vedi §3.3.

### 3.3 Antenne del 688 (fit anni '80–'90)

Fonte principale: il *Submarine Communications Master Plan*, App. A, circa 1995–96 `[SCMP]`. Rispecchia la flotta di metà anni '90. Il fit del 1989 era simile ma **non verificato voce per voce**.

| Sistema | Funzione | Sul 688? | Fonte | Conf. |
|---|---|---|---|---|
| AN/BRA-34 (×2) | Albero multifunzione: HF e UHF bidirezionali, VLF/LF solo ricezione, GPS, IFF. Si usa a quota periscopica | sì | `[SCMP]` | media (anno di introduzione *n.v.*) |
| Periscopio Type 18 | Riceve VLF/LF/VHF/UHF; trasmissione VHF marginale | sì | `[SCMP]`, dossier 01 | media |
| OE-315 filo galleggiante | Antenna a filo galleggiante per VLF/LF e ELF, con OR-279 | sì | `[SCMP]` | media |
| OE-315(V) HSBCA | "High-speed buoyant cable antenna" | sì (anni '90) | `[SCMP]` | bassa per il 1989 |
| AN/BRR-6 | **Boa rimorchiata** ricevente LF/VLF/HF | **no**: solo SSBN Ohio (2 per battello) | `[SCMP]` `[TWZBRR6]` | alta |
| AN/BSQ-5 | Boa rimorchiata bidirezionale (VLF/LF/MF ricezione; HF 0,01–100 W; UHF 100 W, SSIXS) | no: prova su USS *Pogy* (SSN-647) dal 1971/74 | `[FASBSQ5]` | alta |
| AN/BRT-6 | Boa UHF SATCOM a perdere | 3 SSN-637 a metà anni '80; poi 688 | `[FASBRT6]` `[SCMP]` | media |
| AN/WSC-3 + TD-1271 | Terminale UHF SATCOM / LOS | sì | `[SCMP]` | alta |
| R-1051 | Ricevitore HF | sì | `[SCMP]` | media |
| URT-23 | Trasmettitore HF | sì | `[SCMP]` | media |
| AN/WRR-3 | Ricevitore VLF/LF (la SCMP lo cita come apparato da sostituire) | probabile | `[SCMP]` | bassa |
| "Loop" di scafo | Antenna a telaio VLF a bassa quota | *n.v.* per il 688 | — | bassa |

**Conclusione per il gioco.** Il 688 Flight I del 1989 **non ha boa rimorchiata**. Riceve VLF in due modi: (a) filo galleggiante dalla quota operativa lenta; (b) albero/periscopio a quota periscopica. Per trasmettere deve stare a quota periscopica con l'albero BRA-34 alzato. — media.

### 3.4 Limiti di velocità e quota del filo galleggiante

Dati specifici del 688: **non pubblici**. Dati indiretti:

| Indizio | Valore | Fonte | Conf. |
|---|---|---|---|
| Lunghezza del cavo per tenere l'antenna in superficie | "oltre 305 m" | `[APL99]` | alta |
| Perdite del cavo coassiale | ~80 dB, servono amplificatori | `[APL99]` | alta |
| Diametro / sporgenza | 1,7–7,6 cm; sporge al massimo ~5 cm sopra l'acqua | `[APL99]` | alta |
| Prestazioni | Ricezione solo VLF/HF "limitata"; niente UHF; dilavamento delle onde | `[APL99]` `[NAVNEWS]` | alta |
| Vincolo pratico | Il battello deve andare **piano** e tenere rotta stabile con il filo fuori | `[NSL99]` `[ALT]` | alta |
| Analogo moderno (BWA445) | 730–950 m; velocità operativa **4–8 kn**; trascinamento massimo 20 kn | `[BWA]` | media (non è un apparato USA) |
| Brevetto USA anni '70 | Il filo galleggiante ha limiti di quota di chiglia e velocità; la boa lavora più profonda alla stessa velocità | `[PAT3972046]` | media |
| Boa rimorchiata ad alta velocità (DTIC anni '70) | "soddisfacente da 0 a 25 kn" | `[DTICBUOY]` (estratto) | bassa |

**Quota periscopica.** Circa **60 ft (18 m)** di chiglia. `[FASBRT6]` — media. Esposizione dell'albero e del periscopio: vedi dossier 02 §7.7.

---

## 4. Sottomarino → terra

### 4.1 Mezzi disponibili al 688 nel 1989

| Mezzo | Requisiti | Prestazioni | Fonte | Conf. |
|---|---|---|---|---|
| UHF SATCOM (SSIXS) via FLTSATCOM | Quota periscopica, albero alzato | 4800 baud, buffer 64 kB, 15 pagine "in pochi secondi" | `[NSL99]` | media |
| HF (URT-23) | Quota periscopica, albero alzato | Teletype; ritorno a terra senza satellite | `[SCMP]` | media |
| "Circuit Mayflower" | HF unidirezionale nave → terra (AN/BRT-2, KY-766A) | Esistenza nel 1989 *n.v.* | `[SCMP]` | bassa |
| UHF/VHF LOS | Albero o periscopio | Coordinamento con P-3, navi, elicotteri | `[FASBSQ5]` | media |
| Boa a perdere BRT-6 | Lancio in immersione | Solo pochi battelli; per il 688 nel 1989 *n.v.* | `[FASBRT6]` | bassa |

### 4.2 SSIXS

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Introduzione | Metà anni '70 | `[NSL99]` | media |
| Satelliti | Un canale da 25 kHz su ciascuno dei 4 FLTSATCOM; fino a **120 sottomarini** per rete | `[GSSSIXS]` (estratto) | media |
| Modi | **Group broadcast** a intervalli programmati dalla terra; tra un broadcast e l'altro il battello può trasmettere o chiedere i messaggi in coda (**query/response**) | `[FASSSIXS]` | alta |
| Postura | I due modi permettono al comandante di restare **attivo** o **passivo** | `[FASSSIXS]` | alta |
| Centri di controllo | 5 *Broadcast Control Authority* (BCA) | `[GSSI]` | media |
| Tempo a quota periscopica | Con i sistemi del 1999 un battello ha finito il traffico in 15–20 min; prima "di solito un'ora o più" | `[NSL99]` | media |

### 4.3 Copertura FLTSATCOM alle alte latitudini

Satelliti geostazionari. Lanci: FLTSATCOM 1–4 (1978–80), 7 (1986), 8 (**25 set. 1989**); il 5 danneggiato, il 6 distrutto. `[WPFLT]` — alta. Posizioni: 23°W (Atlantico) e 100°W confermate da tabelle del 1997 `[GSUHF]`; 72,5°E e 172°E *n.v.*

Elevazione del satellite sull'orizzonte (`[CALC]`, sfera, senza rifrazione):

| Punto | Sat. 23°W | Sat. 72,5°E (*n.v.*) | Sat. 172°E (*n.v.*) |
|---|---|---|---|
| Mar di Norvegia 65°N 0° | 14,5° | sotto | sotto |
| Capo Nord 70°N 20°E | 5,8° | 3,3° | sotto |
| Barents 72°N 35°E | 0,7° | 5,5° | sotto |
| Barents nord 75°N 40°E | sotto | 3,9° | sotto |
| Islanda 60°N 20°W | 21,9° | sotto | sotto |
| Mediterraneo 38°N 15°E | 30,9° | 16,7° | sotto |
| Curili 45°N 150°E | sotto | 0,1° | 33,7° |

**Lettura.** Nel Barents il SATCOM UHF è marginale o assente. Lì il 688 deve ripiegare su HF. Questo è un vincolo di gioco reale e interessante. — media.

### 4.4 Dottrina: EMCON

- Il sottomarino di norma **non trasmette**. Riceve il broadcast e resta in silenzio radio. `[SCMP]` `[dossier 02 §7.7]` — alta.
- Trasmette solo per ordine o per notizie urgenti (contatto, attacco, avaria). — media.
- Il SATCOM UHF richiede l'albero esposto a quota periscopica e "degrada la furtività". `[SCMP]` — alta.

---

## 5. Formato dei messaggi e precedenza

### 5.1 Precedenza (US Navy / alleati)

| Precedenza | Prosegno | Obiettivo medio | Limiti | Uso | Fonte | Conf. |
|---|---|---|---|---|---|---|
| FLASH OVERRIDE | Y | — | — | Autorità nazionale (EAM); non è una precedenza formale | `[WPPREC]` | media |
| FLASH | **Z** | "il più presto possibile" | ≤10 min in stazione (CCEB) | Primo contatto col nemico, combattimento | `[NRTRAF]` `[WPPREC]` | alta |
| IMMEDIATE | **O** | 30 min | 30 min – 1 h | Situazioni gravi | `[NRTRAF]` | alta |
| PRIORITY | **P** | 3 h | 1–6 h | Azione rapida, SITREP | `[NRTRAF]` | alta |
| ROUTINE | **R** | 6 h | 3 h – inizio del giorno lavorativo successivo | Tutto il resto | `[NRTRAF]` | alta |

### 5.2 Struttura del messaggio

Esempio reale (Z-gram del 1970): `R 251505Z SEP 70` / `FM CNO` / `TO NAVOP` / `UNCLAS //N11101//` / testo / `BT`. `[NRTRAF]` — alta.

- **DTG** (*date-time group*): giorno, ora, minuti, fuso (Z = UTC), mese, anno. Es. `251505Z SEP 70`. — alta.
- Campi: precedenza, DTG, FM, TO, INFO, classifica (UNCLAS, CONFIDENTIAL, SECRET), SUBJ, testo, BT. — media (FM/TO/BT verificati; INFO/SUBJ da prassi, *n.v.* sulla fonte).
- **Autenticazione**: tabelle di autenticazione e codici per i messaggi d'azione. Formato esatto *n.v.* — bassa.

### 5.3 Numerazione e messaggi persi

| Regola | Valore | Fonte | Conf. |
|---|---|---|---|
| Numerazione | Ogni messaggio di un canale broadcast ha un **numero di serie progressivo** | `[NRTRAF]` | alta |
| Sistema "a guardia" | L'utente legge tutte le intestazioni e copia per intero i messaggi a lui indirizzati | `[NRTRAF]` | alta |
| Ritrasmissione programmata | Nel broadcast di flotta (navi) alcuni canali ripetono i canali primari con **1 ora di ritardo** | `[NRTRAF]` | alta (navi), bassa per i sottomarini |
| Recupero | Chiedere a un'altra unità; poi "screen request" alla stazione, che ripete i messaggi indirizzati; si serve prima la precedenza più alta | `[NRTRAF]` | alta (navi) |
| Sottomarini | Il ciclo VLF di 2 ore ripete la sequenza; via SSIXS si possono chiedere i messaggi in coda | `[FASVLF]` `[FASSSIXS]` | media |

**Lettura per il gioco.** Un buco nei numeri di serie dice al giocatore "hai perso un messaggio". Per recuperarlo: aspettare il ciclo successivo, o salire a quota periscopica e interrogare SSIXS (ma così si espone). — media.

---

## 6. Lato sovietico (per giocare l'Alfa)

### 6.1 Stazioni a terra

| Stazione | Luogo | Dati | Fonte | Conf. |
|---|---|---|---|---|
| Goliath (Голиаф) | Kstovo/Druzhny, oblast' di Nižnij Novgorod (non Kaliningrad) | Trasmettitore tedesco del 1943, rimontato 1949–52; 1000 kW | `[RUCOM]` `[RUSNAVY]` | media |
| 43° centro comunicazioni "Antej" | Vilejka (Bielorussia) | Costruito dal 1964; tralicci 3×330 m e 15×270 m; "trasmissione di ordini ai sottomarini"; potenza ~1 MW (`[RUCOM]`) | `[WPVIL]` | media |
| Rete "Beta" | Krasnodar, Vilejka, Arcangelo, Biškek, Nižnij Novgorod, Chabarovsk | 20,5–25,5 kHz; ~1000 kW stimati; quasi tutte costruite negli anni '70; segnali orari alternati ad "altre comunicazioni" | `[WPBETA]` | media |
| "Gerkules", "Taran-M", "Okean" | Centri di comando costieri | Stazioni VLF ad alta capacità; "Okean" la più potente | `[RUSNAVY]` | bassa |
| ZEVS (ЗЕВС) | Kola, presso Severomorsk-3 (~69°N 33°E) | **82 Hz**; dipolo di terra con elettrodi a ~60 km; 10–14 MW in ingresso (stima da immagini); "pochi watt" irradiati | `[WPZEVS]` `[RUCOM]` | media |
| ZEVS — data | Ricevuto da Stanford in Groenlandia, Giappone, Nuova Zelanda e Antartide a **gennaio 1990** | `[FSB98]` | alta (esistenza nel gen. 1990) |
| Sistemi automatici | "Glubina" (1959, dati a sottomarini immersi, antenna a telaio magnetico); "Integral" e "Dal'nost'" (1969); "Molnija" (1972, primo sistema interamente automatico della flotta) | `[RUSNAVY]` | media |
| Copertura КНЧ | Ricezione dei comandi a **100–160 m** | `[RUSNAVY]` | media |

### 6.2 Relè aereo

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Aereo | Tu-142MR (Bear-J), sistema "Orël" | `[NUKC]` `[AVW]` | media |
| Trasmettitore | R-826PL "Fregat" VLF | `[RUCOM]` | bassa |
| Filo | 7.680 m `[AVW]` oppure 8,6 km `[RUCOM]` | — | media |
| Autonomia | Fino a 17 h | `[RUCOM]` | bassa |
| Entrata in servizio | Anni '80 *n.v.* | — | bassa |

### 6.3 Antenne sui sottomarini sovietici

| Sistema | Dati | Fonte | Conf. |
|---|---|---|---|
| "Paravan" (VBAU) | Boa rimorchiata "a pesce gatto", biplano con impennaggi; introdotta 1966 (`[RUSNAVY]`) o ~1968 (`[HIS]`); riceve VLF "a quota di lancio dei missili"; su Golf, Yankee, Delta I | `[HIS]` `[RUSNAVY]` `[ONL]` | media |
| Antenne a filo successive | Ricevono КНЧ, VLF e HF | `[RUSNAVY]` | media |
| Pr.705 Alfa | Nome del complesso di comunicazione, boa e antenne: **n.v.** Nessuna fonte aperta trovata | — | bassa |
| Satelliti | "Parus": navigazione Tsiklon-B più **store-and-forward** (dal 1974, orbita ~1000 km, 83°). Uso da sottomarini *n.v.* "Tsunami": nessuna fonte trovata | `[WPPARUS]` | bassa |

### 6.4 Trasmissione e procedure

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Burst HF "Akula" | Raffiche di **0,72 s**, FSK con shift di 1000 Hz a **500 bit/s**; in uso dal **novembre 1960** | `[WPBOR]` | alta |
| Collegamento "Akula" | HF ad altissima velocità; trasmettitori di bordo fino a 15 kW; ricezione a 8–10.000 km | `[RUSNAVY]` | media |
| "Birjuza" | Sistema burst; impiegabile solo in superficie o forse a quota periscopica con antenna alzata; trasmettitori di bordo da 1 kW | `[CIASOV]` (estratto di ricerca) | bassa |
| Precedenze | Categorie dei radiogrammi militari: **"Monolit"** (mnl), **"Vozduch"** (vzd), **"Raketa"** (rkt), **"Samolët"** (sml); poi i radiogrammi ordinari in ordine d'arrivo. Ordine esatto della gerarchia *n.v.* | `[NUMODD]` | bassa |
| Sessioni di comunicazione | Il comando sovietico era più centralizzato. I battelli dovevano fare **sessioni di comunicazione (сеансы связи) a orari fissi**. Esempio: lo Yankee saliva a quota periscopica **due volte al giorno**, in modo regolare | `[dossier 04 §3]` | media (Yankee), bassa (Alfa) |

---

## 7. Rischio di radiogoniometria (DF)

### 7.1 Reti occidentali

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| AN/FRD-10 (CDAA, "Wullenweber") | Anello esterno di 120 dipoli, anello interno di 40; ~1,5–30 MHz | `[ERADIO]` `[CRIP]` | media |
| Rete | 12 antenne USA (1962–64) + 2 Canada (Gander, Masset, 1971); altre fonti 16–18 | `[WPCDAA]` `[CRIP]` | media |
| Programma | "Classic Bullseye": SIGINT strategica e localizzazione di trasmettitori | `[ERADIO]` | media |
| Portata nominale | **3200 nmi** (1–2 salti ionosferici) | `[ERADIO]` | media |
| Contro i burst | **Project Boresight**: il registratore AN/FRA-44 registra il segnale e calcola il rilevamento dopo. Localizza burst da 0,7 s | `[WPBOR]` | alta |
| Prova storica | Ottobre 1962: le stazioni USA localizzano l'origine dei burst dei Foxtrot diretti a Cuba, pur senza decifrarli | `[NSA399]` | alta |
| Precisione | I fix sono **ellissi**, a volte così grandi che si mandava un mezzo lungo la linea di rilevamento. Valori numerici *n.v.* | `[ERADIO]` (estratto) | media (qualitativo) |

### 7.2 Reti sovietiche

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| "Krug" | CDAA tipo Wullenweber, 40 elementi; almeno **30 antenne** in URSS e paesi alleati negli anni '50 | `[WPCDAA]` | media |
| Organizzazione | Ministero della Difesa; 12 complessi strategici, 8 stazioni DF isolate, più centri esteri (Cuba, Vietnam, Birmania, Mongolia) | `[MILPER]` (estratto) | bassa |
| VLF-DF | La CIA nel 1974 segnala possibili antenne DF VLF in 4 siti Krug | `[CIA74]` | media |
| Contro i SATCOM UHF | Un'emissione UHF verso un satellite è intercettabile da ESM entro l'orizzonte radio (navi, aerei). Probabilità reale *n.v.* | — | bassa |

### 7.3 Dal fix alla minaccia

Nessuna fonte aperta dà tempi. Catena logica: intercettazione → rilevamenti da ≥2 stazioni → ellisse → aereo MPA inviato verso l'ellisse → ricerca con boe/radar. Durata della catena: **ore**, non minuti, per una rete strategica; **minuti** per un ESM locale (nave o aereo vicino). `[CALC]` — bassa.

---

## 8. Ritmo pratico delle comunicazioni

| Aspetto | Valore | Fonte | Conf. |
|---|---|---|---|
| Copia del broadcast | Con il filo galleggiante a bassa velocità, o all'albero a quota periscopica | `[NSL99]` | alta |
| Attesa del vecchio metodo VLF | Il battello poteva restare **ore** a quota periscopica per copiare il traffico | `[NSL99]` | media |
| Durata di una sosta a quota periscopica per comunicare | Tipicamente **un'ora o più** (prima dei sistemi del 1999) | `[NSL99]` | media |
| Cadenza delle soste | Orari prefissati dagli ordini d'operazione ("preassigned time schedule") | `[DRDO]` | media (principio) |
| Intervallo tipico 688 | Ogni **8–24 h** secondo la missione. Nessuna fonte aperta trovata | — | bassa (*n.v.*) |
| Contenuto in uscita | Rapporti di contatto, SITREP, conferme di ricezione, rapporti di attacco | prassi | bassa (*n.v.*) |
| Desiderio della forza subacquea | Soste "brevi e rare" | `[NSL99]` | alta |

---

## 9. Proposta per il gioco

Tutto in questa sezione è **STIMA**, salvo i riferimenti alle sezioni precedenti.

### 9.1 Modi di ricezione del 688 Flight I

| Modo | Bande | Quota chiglia max | Velocità max | Note |
|---|---|---|---|---|
| Albero BRA-34 / periscopio | VLF, LF, HF, UHF (rx/tx) | **18 m** (quota periscopica, §3.4) | **6 kn** (oltre: pennacchio visibile, dossier 02) | Unico modo per trasmettere |
| Filo galleggiante OE-315, VLF/LF | solo rx | **120 m** | **8 kn** pieno; **8–12 kn** ricezione al 50%; **>12 kn** filo sommerso | Rotta stabile: virata >30° = 2 min senza segnale |
| Filo galleggiante, ELF | solo rx | **300 m** | **15 kn** | Solo se il ricevitore OR-279 è installato (opzione, §2.4) |
| Nessuna antenna | — | — | — | Silenzio totale |

Rischio del filo: a >20 kn per più di 1 min, 30% di rottura per minuto. Filo rotto = niente VLF fino a fine missione (salvo albero). STIMA.

### 9.2 Tempo di ricezione per messaggio

| Banda | Velocità di gioco | Messaggio corto (300 car.) | Medio (1500 car.) | Lungo (5000 car.) |
|---|---|---|---|---|
| ELF | 3 lettere / 15 min | solo codici: 1 gruppo = 15 min | — | — |
| VLF/LF | 6 car./s (50 baud) | 50 s | 4 min | 14 min |
| HF (rx) | 10 car./s (75 baud teletype, *n.v.*) | 30 s | 2,5 min | 8 min |
| UHF SATCOM | 600 car./s (4800 baud) | <1 s | 3 s | 9 s |

Più un tempo fisso di **acquisizione** (sincronismo, cifra): VLF 1 min, HF 2 min, UHF 3 min. STIMA.

### 9.3 Ciclo del broadcast e messaggi persi

- Ciclo VLF: **120 min** (§2.2). Ogni messaggio va in onda una volta per ciclo.
- I messaggi FLASH e IMMEDIATE vanno in testa al ciclo e si ripetono per **3 cicli**. PRIORITY: 2 cicli. ROUTINE: 1 ciclo. STIMA.
- Ogni messaggio ha un numero di serie. Se manca un numero, il giocatore vede "SERIE 0412 MANCANTE".
- Recupero: (a) ascoltare il ciclo successivo, se il messaggio si ripete; (b) query SSIXS a quota periscopica: 1 trasmissione breve (§9.4).
- ELF (se attivo): il campanello indica "sali e copia". Il messaggio vero arriva entro il ciclo VLF successivo.

### 9.4 Esposizione in trasmissione

| Fase | Durata di gioco | Base |
|---|---|---|
| Salita a quota periscopica e giro di periscopio di sicurezza | 5–10 min | dossier 02 §7.7 |
| Albero alzato, sincronizzazione UHF | 2–5 min | `[NSL99]` (soste da 15 min a >1 h) |
| Emissione UHF SSIXS | 1–10 s | 4800 baud |
| Emissione HF (teletype) | 1–3 min | STIMA |
| Ricezione traffico in coda | 2–20 min | §9.2 |
| **Totale a quota periscopica (1989)** | **20–60 min** | `[NSL99]` |

Nel Barents oltre ~72°N il SATCOM UHF non è disponibile (§4.3). Si passa a HF, con emissione più lunga.

### 9.5 Modello DF

Probabilità che una trasmissione sia **localizzata**:

`P_fix = P_rete · (1 − exp(−t / τ))`

| Emissione | P_rete | τ | Errore del fix (1σ) | Ritardo minaccia |
|---|---|---|---|---|
| HF verso rete strategica (Krug / Bullseye) | 0,9 in copertura | 1,5 s | 2° del rilevamento × distanza (es. 35 nmi a 1000 nmi) | 1–3 h (MPA dalla base più vicina) |
| UHF SATCOM, ESM nemico entro orizzonte | 0,6 | 3 s | 5° di rilevamento, nessuna distanza (un solo sensore) | immediato (unità già vicina) |
| UHF SATCOM, nessun ESM vicino | 0,05 | 3 s | 100 nmi | 3–6 h |
| Burst sovietico "Akula" (0,7 s) contro Bullseye | 0,9 | 1,5 s | come HF | 1–3 h |

Esempi: HF di 0,7 s → P ≈ 0,33. HF di 10 s → P ≈ 0,9. UHF di 5 s con ESM vicino → P ≈ 0,49. Il fix crea in mappa un'**ellisse** sospetta per il nemico; dopo il ritardo arriva un MPA che cerca dentro l'ellisse. STIMA.

### 9.6 Cadenze di missione

| Parametro | USA (688) | URSS (Alfa) |
|---|---|---|
| Finestra di comunicazione obbligatoria | Ogni **12 h** (variabile 8–24 h per scenario) | **2 sessioni al giorno** a orari fissi (es. 00:00 e 12:00 ora di Mosca) |
| Tolleranza | ±2 h a scelta del comandante | ±30 min |
| Penalità se saltata | Ordini persi fino alla finestra dopo | Il comando considera il battello in difficoltà dopo 2 sessioni saltate |
| ELF | Solo da fine 1989, opzione di scenario | ZEVS sempre attivo nella Flotta del Nord, opzione nel Pacifico |
| Ricezione sotto il ghiaccio | Solo ELF | Solo КНЧ (ZEVS) |
| Relè aereo di riserva | TACAMO (EC-130Q / E-6A) | Tu-142MR |

### 9.7 Contenuti dei messaggi in gioco

- **In arrivo:** cambio di area, regole d'ingaggio, dati di contatto da SOSUS o P-3, ordine di attacco, richiamo.
- **In uscita:** rapporto di contatto (FLASH), rapporto d'attacco (IMMEDIATE), SITREP (PRIORITY), conferma di ricezione (ROUTINE).
- Ogni messaggio in uscita sceglie banda e precedenza. La precedenza non cambia l'esposizione, ma cambia quanto presto il comando reagisce.

---

## 10. Fonti

### Web (consultate il 24 ottobre 2026)
- `[FASVLF]` FAS, *Very Low Frequency (VLF)*: https://nuke.fas.org/guide/usa/c3i/vlf.htm
- `[GSVLF]` GlobalSecurity, VLF (ISABPS, ciclo di 2 ore; letto da estratto di ricerca): https://www.globalsecurity.org/wmd/systems/vlf.htm
- `[SCMP]` *Submarine Communications Master Plan*, App. A: https://man.fas.org/dod-101/navy/docs/scmp/part06.htm
- `[WPCOM]` Wikipedia, *Communication with submarines*: https://en.wikipedia.org/wiki/Communication_with_submarines
- `[WPELF]` Wikipedia, *Project Sanguine / Project ELF*: https://en.wikipedia.org/wiki/Project_Sanguine
- `[ALT]` C.A. Altgelt, *The World's Largest "Radio" Station*: https://pages.hep.wisc.edu/~prepost/ELF.pdf
- `[WPTAC]` Wikipedia, *TACAMO*: https://en.wikipedia.org/wiki/TACAMO
- `[FASE6]` FAS, *E-6 Mercury (TACAMO)*: https://nuke.fas.org/guide/usa/c3i/e-6.htm
- `[TWZE6]` The War Zone, E-6B in orbita: https://www.twz.com/31477/heres-why-an-e-6b-doomsday-plane-was-flying-tight-circles-off-the-jersey-shore-today
- `[FASSSIXS]` FAS, *SSIXS*: https://nuke.fas.org/guide/usa/c3i/ssixs.htm
- `[GSSSIXS]` GlobalSecurity, SSIXS (estratto di ricerca): https://www.globalsecurity.org/wmd/systems/ssixs.htm
- `[GSSI]` GlobalSecurity, *SI SSIXS*: https://www.globalsecurity.org/intell/systems/ssixs_si.htm
- `[NSL99]` F.C. Borik, "The Silent Service Is on the Air", Naval Submarine League, luglio 1999: https://archive.navalsubleague.org/?p=3481
- `[FASBRT6]` FAS, AN/BRT-6: https://man.fas.org/dod-101/sys/ship/weaps/an-brt-6.htm
- `[FASBSQ5]` FAS, AN/BSQ-5: https://man.fas.org/dod-101/sys/ship/weaps/an-bsq-5.htm
- `[TWZBRR6]` The War Zone, boa AN/BRR-6: https://www.twz.com/this-buoy-helps-ballistic-missile-submarines-get-nuclear-strike-orders
- `[APL99]` G.R. Thompson et al., "Buoyant Cable Antenna Technology for Enhancing Submarine Communications at Speed and Depth", *JHU APL Technical Digest* 20(3), 1999: https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V20-N03/20-03-Thompson.pdf
- `[BWA]` ALSEAMAR, scheda BWA445: https://www.alseamar-alcen.com/wp-content/uploads/2024/12/BAT-BWA445.pdf
- `[NAVNEWS]` Naval News, "Communication Antenna: A Key System of Modern Submarines", 2022: https://www.navalnews.com/naval-news/2022/04/communication-antenna-a-key-system-of-modern-submarines/
- `[PAT3972046]` Brevetto USA 3.972.046: https://patents.justia.com/patent/3972046
- `[DTICBUOY]` DTIC ADA046024 (estratto di ricerca, testo non letto): https://apps.dtic.mil/sti/pdfs/ADA046024.pdf
- `[LIMARC]` Presentazione VLF: https://limarc.org/wp-content/uploads/2020/04/VLF-Presentation.pdf
- `[NRTRAF]` Navy Radio, *Traffic Handling, Circuits, and Message Processing*: https://www.navy-radio.com/traffic.htm
- `[WPPREC]` Wikipedia, *Message precedence*: https://en.wikipedia.org/wiki/Message_precedence
- `[WPVLFL]` Wikipedia, *List of VLF transmitters*: https://en.wikipedia.org/wiki/List_of_VLF_transmitters
- `[REEVE]` Reeve, elenco stazioni VLF: https://www.reeve.com/Documents/Articles%20Papers/Reeve_VLFStationList_Abrdg.pdf
- `[WPANT]` Wikipedia, *Anthorn Radio Station*: https://en.wikipedia.org/wiki/Anthorn_Radio_Station
- `[WPCUT]` Wikipedia, *VLF Transmitter Cutler*: https://en.wikipedia.org/wiki/VLF_Transmitter_Cutler
- `[WPFLT]` Wikipedia, *Fleet Satellite Communications System*: https://en.wikipedia.org/wiki/Fleet_Satellite_Communications_System
- `[GSUHF]` GlobalSecurity, UHF SATCOM: https://www.globalsecurity.org/space/systems/uhf.htm
- `[WPBOR]` Wikipedia, *Project Boresight*: https://en.wikipedia.org/wiki/Project_Boresight
- `[WPCDAA]` Wikipedia, *Circularly disposed antenna array*: https://en.wikipedia.org/wiki/Circularly_disposed_antenna_array
- `[CRIP]` J.B. Crawford, "huff-duff", 2023: https://computer.rip/2023-01-16-huff-duff.html
- `[ERADIO]` Engineering Radio, "How the Cold War was won": https://www.engineeringradio.us/blog/2010/10/how-the-cold-war-was-won/
- `[MILPER]` Military Periscope, Krug (estratto): https://www.militaryperiscope.com/weapons/electronics/electronic-warfare/krug/overview/
- `[CIA74]` CIA, *Possible VLF-DF arrays at Soviet Krug communications support facilities*: https://www.cia.gov/readingroom/node/2016512
- `[CIASOV]` CIA, *Communications procedures and equipment on a Soviet destroyer and other Soviet naval vessels* (estratto di ricerca; sito in manutenzione): https://www.cia.gov/readingroom/node/1609927
- `[NSA399]` National Security Archive, EBB 399 (sottomarini a Cuba, 1962): https://nsarchive2.gwu.edu/NSAEBB/NSAEBB399/
- `[WPZEVS]` Wikipedia, *ZEVS (transmitter)*: https://en.wikipedia.org/wiki/ZEVS_(transmitter)
- `[RUCOM]` Wikipedia RU, *Связь с подводными лодками* (copia): https://dic.academic.ru/dic.nsf/ruwiki/224680
- `[FSB98]` A.C. Fraser-Smith, P.R. Bannister, "Reception of ELF signals at antipodal distances", *Radio Science* 33(1), 1998, pp. 83–88; scheda: https://ee.stanford.edu/~acfs/fraser-smith-research.html
- `[WPVIL]` Wikipedia, *Vileyka VLF transmitter*: https://en.wikipedia.org/wiki/Vileyka_VLF_transmitter
- `[WPBETA]` Wikipedia, *Beta (time signal)*: https://en.wikipedia.org/wiki/Beta_(time_signal)
- `[RUSNAVY]` RusNavy, storia delle comunicazioni della Marina: https://rusnavy.com/science/electronics/rv6.htm
- `[HIS]` H.I. Sutton, boa sovietica sulla spiaggia delle Hawaii: https://hisutton.com/Soviet-20submarine-20device-20washes-20up-20on-20Hawaiian-20beach.html
- `[ONL]` Onliner, "Paravan", 2016: https://tech.onliner.by/2016/05/19/paravan
- `[AVW]` Aviation Week, modernizzazione dell'aviazione navale russa: https://aviationweek.com/defense/budget-policy-operations/how-russia-modernizing-its-naval-aviation-fleet
- `[NUKC]` Nuclear Companion, "Tupolev Tu-142MR: Inside Soviet Union's TACAMO": https://nuclearcompanion.com/tupolev-tu-142mr-inside-soviet-unions-tacamo/
- `[WPPARUS]` Wikipedia, *Parus (satellite)*: https://en.wikipedia.org/wiki/Parus_(satellite)
- `[NUMODD]` Numbers & Oddities, *Monolyth* (categorie dei radiogrammi): https://www.numbersoddities.nl/Monolyth.pdf
- `[DRDO]` *Defence Science Journal* (DRDO), comunicazioni con sottomarini: https://publications.drdo.gov.in/ojs/index.php/dsj/article/download/4209/2463

### Rimandi interni
- Dossier 01 (`[FAS688]`: periscopi Type 2 / Type 18, ESM WLR-8).
- Dossier 02 §7.7 (EMCON, esposizione del periscopio, orizzonte radar).
- Dossier 04 §3 (cadenza di quota periscopica dello Yankee).

### Lacune principali
- Quota e velocità reali del filo OE-315 sul 688: classificate.
- Fit radio esatto dei 688 Flight I nel 1989 (BRA-34 sì/no, data dell'OR-279).
- Intervalli reali delle finestre di comunicazione degli SSN: solo memorialistica, non verificata.
- Apparati di comunicazione del Pr.705 e procedure sovietiche dettagliate.
- Precisione numerica dei fix HFDF.
