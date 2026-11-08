# 11 — Emettitori radar, EMCON ed ESM dei sottomarini, circa 1989 (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. SSN-688 *Los Angeles* Flight I contro unità sovietiche, soprattutto Pr.705 *Alfa*.
> Scopo: dare al gioco un database degli emettitori radar per ogni classe, le regole EMCON per l'IA, un modello del ricevitore ESM e un modello di scoperta radar di periscopi, alberi e snorkel.
> Compilato: 24 ottobre 2026.

---

## 0. Convenzioni e avvertenze

**Unità.** nm = miglia nautiche; kn = nodi; rpm = giri al minuto; "periodo" = tempo di un giro d'antenna (60/rpm s). Altezze in m (piedi tra parentesi se la fonte è in piedi).

**Confidenza.**
- **alta**: fonte ufficiale o declassificata, oppure ≥2 fonti indipendenti serie concordi.
- **media**: una sola fonte secondaria seria (FAS, Wikipedia con fonti, radartutorial, estratti di Military Periscope).
- **bassa**: fonte debole, stima del compilatore, oppure dato ricordato dalla letteratura e **non riverificato** in questa sessione (*n.v.*).

**STIMA** = valore proposto per il gioco. Non è un dato storico. Va tarato. `[CALC]` = calcolo del compilatore con fisica standard.

**Sigle delle fonti** tra parentesi quadre, es. `[FASWLR8]`. Elenco con URL nella §7.

**Avvertenza generale.** Potenze, PRF e soglie dei radar militari sono ancora in gran parte riservate. Per i radar sovietici le fonti aperte danno quasi solo nome NATO, ruolo e banda. Molte celle sono quindi *n.v.* o STIMA. Le pagine di Military Periscope `[MP…]` sono a pagamento: qui se ne usa solo il riassunto pubblico.

### 0.1 Bande di frequenza (lettere NATO/ECM, usate nel resto del dossier)

| Lettera NATO | Frequenze | Equivalente IEEE indicativo | Uso tipico nel 1989 |
|---|---|---|---|
| B | 0,25–0,5 GHz | UHF | ricerca aerea a lungo raggio (SPS-40) |
| C | 0,5–1 GHz | UHF/L | ricerca aerea (SPS-49) |
| D | 1–2 GHz | L | ricerca aerea sovietica (Top Sail, Top Plate) |
| E/F | 2–4 GHz | S | ricerca 3D (SPS-48), Head Net, radar S mercantili |
| G | 4–6 GHz | C | superficie (SPS-10, SPS-67) |
| H/I | 6–10 GHz | C/X | navigazione, superficie, tiro, radar aerei ASW |
| J | 10–20 GHz | X/Ku | tiro, seeker dei missili, CIWS |
| K | 20–40 GHz | K/Ka | tiro fine (Kite Screech in tracking) |

Fonte: convenzione NATO standard, `[CALC]` per l'abbinamento IEEE — alta.

### 0.2 Orizzonte radar (coerente con dossier 02 §7.7)

`d (km) = 4,12·(√h_r + √h_t)` con altezze in metri; equivale a `d (nm) = 1,23·(√h_r ft + √h_t ft)` del dossier 02. Bersaglio o antenna ESM a **1,5 m** sull'acqua (testa del periscopio o albero ESM) `[CALC]`:

| Antenna radar a… | Esempio | Orizzonte vs 1,5 m |
|---|---|---|
| 10 m | radar del sottomarino in emersione, motovedetta | 18 km (9,8 nm) |
| 20 m | navigazione di fregata, Nanuchka | 23,5 km (12,7 nm) |
| 25–30 m | mercantile, petroliera, radar di superficie di caccia | 26–28 km (14–15 nm) |
| 40–50 m | ricerca aerea di incrociatore, Kirov, portaerei | 31–34 km (17–18,5 nm) |
| 150 m (500 ft) | elicottero in ricerca | 55 km (30 nm) |
| 300 m (1000 ft) | MPA in ricerca bassa | 76 km (41 nm) |
| 1500 m (5000 ft) | MPA in transito basso | 165 km (89 nm) |
| 6000 m (20.000 ft) | Tu-142 o P-3C in transito alto | 324 km (175 nm) |

L'orizzonte vale **in entrambe le direzioni**. Il radar non vede il periscopio oltre l'orizzonte. Ma anche l'ESM non sente il radar oltre l'orizzonte (salvo diffrazione e condotti, §4.3).

---

## 1. Database degli emettitori per classe (fit 1989)

Colonne: **Banda** = lettera NATO (GHz); **Pot.** = potenza di picco; **h** = altezza d'antenna sul mare; **Per.** = periodo di rotazione; **Portata** = portata contro un bersaglio grande (nave o aereo). Le altezze d'antenna sono quasi sempre **STIMA** dalle sagome (bassa).

### 1.1 Navi USA

| Classe | Radar | Ruolo | Banda (GHz) | Pot. | h | Per. | Portata | Fonte | Conf. |
|---|---|---|---|---|---|---|---|---|---|
| **perry** (FFG-7) | **SPS-49** | ricerca aerea 2D | C (0,851–0,942) | 360 kW (versione A) | ~30 m | 5 s / 10 s (12/6 rpm) | fino a 256 nm | `[WPSPS49]` | alta |
| perry | **SPS-55** | superficie | I/J (9,05–10,0) | ≥130 kW (alcune fonti 160) | ~25 m | 3,75 s (16 rpm) | orizzonte; fascio 1,5° | `[FASSPS55]` | alta |
| perry | **Mk 92 CAS** | ricerca e inseguimento combinati (deriva dal WM-25 olandese) | I/J *n.v.* | n.v. | ~25 m | ricerca rapida *n.v.* | ~20–30 nm *STIMA* | `[FASMK92]` | media (ruolo), bassa (banda) |
| perry | **STIR** | illuminatore (deriva dall'SPG-60) | I/J *n.v.* | n.v. | ~20 m | solo puntamento | oltre la portata del CAS | `[FASMK92]` | media |
| **kidd** (DDG-993) | **SPS-48C** (SPS-48E e SPS-49(V)5 con la New Threat Upgrade 1988–90) | ricerca aerea 3D | E/F (2–4) | n.v. (35 kW medi) | ~35 m | 8 s / 4 s (7,5/15 rpm) | >250 nm | `[WPSPS48]` `[WPKIDD]` | alta |
| kidd | SPS-55 | superficie | I/J | 130 kW | ~25 m | 3,75 s | orizzonte | `[WPKIDD]` | alta |
| kidd | 2 × **SPG-51D**, **SPG-60**, **SPQ-9A** | tiro Standard / tiro cannone / ricerca-tiro di superficie | G/I (SPG-51) *n.v.*; I/J (SPG-60, SPQ-9) *n.v.* | n.v. | 20–30 m | SPQ-9: rotazione rapida *n.v.* | 10–20 nm | `[WPKIDD]` | media |
| **spruance** (DD-963) | **SPS-40** (SPS-49 su alcune unità a fine anni '80 *n.v.*) | ricerca aerea 2D | B (0,40–0,45) | 200–300 kW | ~30 m | 8 s / 4 s (7,5/15 rpm) | 250 nm | `[WPSPS40]` `[WPSPRU]` | alta |
| spruance | SPS-55, SPG-60, SPQ-9A, Mk 23 TAS | superficie / tiro / ricerca bassa di autodifesa | I/J; TAS in D *n.v.* | n.v. | 20–30 m | TAS ~2 s *n.v.* | — | `[WPSPRU]` | media |
| **iowa** (BB-61) | **SPS-49**, **SPS-67** (Mk 13 sulle direzioni di tiro, radar dei Phalanx) | ricerca aerea / superficie | C; G (5,45–5,825) | 360 kW; 280 kW | ~40 m | 5–10 s; 2–4 s | 256 nm; 56 nm | `[WPSPS49]` `[WPSPS67]` `[NAVYSPS67]` | media (fit) |
| **nimitz** (CVN) | **SPS-48C/E**, **SPS-49**, SPS-10 o SPS-67, Mk 91/Mk 95 (Sea Sparrow), radar d'avvicinamento SPN-43/41/46 | 3D / 2D / superficie / tiro / controllo aereo | E/F; C; G; I/J; SPN-43 in E/F *n.v.* | — | 40–50 m | 4–10 s | >250 nm | `[WPSPS48]` `[WPSPS49]` | media |
| **sacramento** (AOE) | SPS-40 *n.v.*, SPS-10 o SPS-67 *n.v.*, Mk 91 Sea Sparrow | ricerca aerea / superficie / tiro | B; G; I/J | — | ~35 m | 4–8 s | 250 nm / orizzonte | `[WPSAC]` (solo Mk 56/Sea Sparrow) | bassa |
| — | **SPS-10** (in sostituzione con SPS-55/67) | superficie | G (5,45–5,825) | 190–280 kW | — | 4 s (15 rpm) | 43 nm | `[WPSPS10]` | alta |

**Nota.** Lo **SPY-1** (Aegis) non è nel database del gioco e non serve qui.

### 1.2 Navi sovietiche

Le bande dei radar sovietici vengono quasi tutte da riassunti di Military Periscope, radartutorial e dalla lista di qsl.net `[QSL]`. Le fonti a volte si contraddicono.

| Radar (NATO / designazione) | Ruolo | Banda | Pot. / Per. | Portata | Classi del gioco | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| **Top Sail** (MR-600 Voskhod) | ricerca aerea 3D a lungo raggio | D (1–2 GHz) | n.v. | ~300 km *n.v.* | kara | `[RW1134B]` `[QSL]` `[MPTOPSAIL]` | media (banda bassa) |
| **Top Pair** (MR-800 Voskhod) | ricerca aerea 3D a lungo raggio (Top Sail + Big Net schiena contro schiena) | C/D *n.v.* | n.v. | ~300 km *n.v.* | kirov, slava | `[WPSLAVA]` | media (ruolo) |
| **Top Steer** (MR-700 Fregat) / **Top Plate** (MR-710/750 Fregat-M/MA) | ricerca aerea 3D | D/E | Top Plate: 5 s / 10 s (12/6 rpm) | 150 / 300 km | kirov, slava | `[WPSLAVA]` `[RTTOP]` | media |
| **Head Net-C** (MR-310A Angara-A) | ricerca aria-superficie | **E/F** secondo `[MPHEAD]`, **L** (≈D) secondo `[RTHEAD]` | n.v. | 200 km | kara, krivak2 | `[RW1134B]` `[RW1135]` | media (ruolo), bassa (banda) |
| **Don Kay** (Volga) / **Don-2** (Don) | navigazione | I (~9,4 GHz) | ~2,5–3 s *n.v.* | orizzonte | kara, krivak2, koni | `[RW1134B]` `[QSL]` | media |
| **Palm Frond** (MR-212 Vaygach) | navigazione / superficie | I *n.v.* | ~3 s *n.v.* | orizzonte | kirov, slava | `[WPNATO]` | media (ruolo) |
| **Strut Curve** (MR-302 Rubka) | ricerca aria-superficie | E/F (banda S) | n.v. | ~100 km *n.v.* | koni | `[WPKONI]` `[QSL]` | media |
| **Peel Pair** | superficie | I/J (8–10 GHz) | n.v. | orizzonte | nanuchka (III) | `[MPPEEL]` | media |
| **Plank Shave** (Garpun) | superficie e designazione bersagli, in servizio dal 1979 | I/J *n.v.* (con canale passivo) | n.v. | oltre l'orizzonte in condotto *n.v.* | nanuchka (III) *n.v.* | `[MPPLANK]` | bassa (fit) |
| **Band Stand** (Titanit) | designazione bersagli per SS-N-9 | D/E/F *n.v.* | n.v. | n.v. | nanuchka | `[WPNAN]` `[GSNAN]` | bassa (banda) |
| **Head Light** (Grom-M) | guida SA-N-3 / SS-N-14 | G/H | solo puntamento | ~40 km *n.v.* | kara | `[WPKARA]` `[QSL]` | media |
| **Eye Bowl** (Drakon) | guida SS-N-14 Metel | F *n.v.* | solo puntamento | n.v. | krivak2, kirov | `[WPKRIVAK]` `[WPKIROV]` | media (ruolo) |
| **Pop Group** (MPZ-301 Baza / 4R-33) | tiro SA-N-4 Osa-M | H/I/J | solo puntamento | ~15 km | kara, krivak2, kirov, slava, koni, nanuchka | `[QSL]` `[WPSLAVA]` | media |
| **Top Dome** (3R41 Volna) | tiro SA-N-6 | J *n.v.* | solo puntamento | ~75–90 km *n.v.* | kirov, slava | `[WPSLAVA]` `[WPKIROV]` | media (ruolo) |
| **Front Door-C** (Argument) | guida e designazione SS-N-12 Bazalt | F *n.v.* | n.v. | n.v. | slava | `[WPSLAVA]` `[MPFRONT]` | media (ruolo) |
| **Kite Screech** (MR-184 Lev; Lev su 1135M) | tiro AK-100/AK-130 | I in ricerca grossolana, K in inseguimento fine | n.v. | ~30 km *n.v.* | kirov, slava, krivak2 | `[MPKITE]` `[RW1135]` | media |
| **Owl Screech** (MR-105 Turel; la lista NATO di Wikipedia dà il Turel come *Hawk Screech*) | tiro AK-726 | I | n.v. | n.v. | kara, krivak (I) | `[RW1134B]` `[QSL]` `[WPNATO]` | bassa (nome) |
| **Hawk Screech** / **Drum Tilt** (MR-104 Rys) | tiro AK-726 / AK-230 | I | n.v. | n.v. | koni | `[WPKONI]` `[QSL]` | media |
| **Muff Cob** (MR-103 Bars) | tiro AK-725/AK-257 | I | n.v. | n.v. | nanuchka (I) *n.v.* | `[QSL]` | bassa (fit) |
| **Bass Tilt** (MR-123 Vympel) | tiro AK-630 / AK-176 | H/I *n.v.* | 200 kW; 4 s (15 rpm) in ricerca | ~5–8 km | kara, kirov, slava, nanuchka (III) | `[MPBASS]` `[RW1134B]` | media |

**Fit per classe del gioco (1989).**

| Classe | Emettitori sempre presenti | Tiro (solo in ingaggio) | Note | Conf. |
|---|---|---|---|---|
| **kara** (Pr.1134B) | Top Sail, Head Net-C, Don Kay ×2, Don-2 | Head Light ×2, Pop Group ×2, Owl Screech ×2, Bass Tilt ×2 | Azov con Top Dome di prova; RTR **MRP-15-16 "Zaliv"**, MRP-11-12, MRP-13-14; jammer Gurzuf (Side Globe) | media `[WPKARA]` `[RW1134B]` |
| **krivak2** (Pr.1135M) | Head Net-C, Don Kay, Don-2 | Eye Bowl ×2, Pop Group ×2, Kite Screech | Krivak I: Owl Screech invece di Kite Screech | media `[RW1135]` `[WPKRIVAK]` |
| **kirov** (Pr.1144) | Top Pair, Top Steer (Frunze/Kalinin: Top Plate *n.v.*), Palm Frond ×3 | Top Dome ×2, Pop Group ×2, Eye Bowl ×2 (solo Kirov), Kite Screech, Bass Tilt ×4 (Frunze: Cross Sword) | Granit guidato dal proprio seeker; dati via satellite | media `[WPKIROV]` |
| **slava** (Pr.1164) | Top Pair, Top Steer, Palm Frond ×3 | Front Door-C, Top Dome, Pop Group ×2, Kite Screech, Bass Tilt ×3 | suite EW "Kol'chuga" (Side Globe, Rum Tub, Bell) | media `[WPSLAVA]` |
| **koni** (Pr.1159) | Strut Curve, Don-2 | Pop Group, Hawk Screech, Drum Tilt | unità export | media `[WPKONI]` |
| **nanuchka** (Pr.1234) | Peel Pair (o Don-2), Band Stand | Pop Group, Muff Cob (I) / Bass Tilt (III) | Band Stand acceso per designare gli SS-N-9 | media (fit), bassa (bande) `[WPNAN]` |

### 1.3 Navi civili

| Classe | Radar | Banda (GHz) | Pot. | h | Per. | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| **mercantile**, **cargo** | 1 radar di navigazione in banda X (IEEE), a volte un secondo in banda S | I (9,3–9,5) e E/F (2,9–3,1) | 10–30 kW *n.v.* | 25–35 m | 2,5–3 s (20–24 rpm) *n.v.* | norme IMO ricordate, *n.v.* | bassa |
| **petroliera** | come sopra; grandi petroliere quasi sempre X + S | I + E/F | 25–30 kW *n.v.* | 30–40 m | 2,5–3 s | *n.v.* | bassa |

**Nota di gioco.** I radar civili restano **sempre accesi** in navigazione. Creano rumore di fondo per l'ESM. Il Don-2 e il Palm Frond sono radar di navigazione in banda I come i civili: l'operatore può confonderli (§2).

### 1.4 Aerei ed elicotteri

| Classe | Radar | Ruolo | Banda (GHz) | Pot. / Per. | Dati utili | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| **p3c** (P-3C Update II/III) | **APS-115** (2 antenne, muso e coda, copertura 360°) | ricerca di superficie ASW | I/J (~8,5–9,6 *n.v.*) | n.v.; magnetron; derivato dall'APS-80 (143 kW) | Portata vs nave grande 100–150 nm *n.v.* | `[WPAPS]` `[RTAPS]` | media (esistenza), bassa (numeri) |
| p3c (opzione 1990+) | **APS-137** (ISAR) | ricerca, ISAR, **modo periscopio** | I/J | modo periscopio: **0,2 s** (300 rpm), 2000 impulsi/s, fascio 2,4°, risoluzione 1 ft | Nato per l'S-3; sui P-3 "in numero limitato" ancora nel 1997 | `[APL97]` | alta (parametri), **marginale nel 1989** |
| **sh60b** (LAMPS III) | **APS-124** | ricerca di superficie | I/J | 350 kW; fino a 120 rpm (0,5 s); PRF 470/940/1880 Hz; impulsi 2/1/0,5 µs; fascio 1,2° | portata strumentale 160 nm; IOC 1983 | `[ARMSAPS124]` `[MPAPS124]` | media |
| **il38** | **Wet Eye** (radar della PPS "Berkut-38") | ricerca di superficie, "abbastanza definito per periscopi e snorkel" | I *n.v.* | n.v. | IOC 1959; radome ventrale; scansione circolare o a settore, modo "microplan" | `[MPWETEYE]` `[WPIL38]` `[RWIL38]` | media (ruolo), bassa (banda) |
| **tu142** | Wet Eye (Tu-142 "Berkut-95"); sul Tu-142M/MK radar della PPS "Korshun" | ricerca di superficie e navigazione | I *n.v.* | n.v. | radome ventrale sotto la sezione centrale | `[RWTU142]` `[MPWETEYE]` | bassa |
| **ka25** (Ka-25PL Hormone-A) | **Initsiativa-2K** (sistema "Baikal"), NATO probabilmente **Puff Ball** | ricerca panoramica in onde centimetriche | I (centimetrica) | n.v. | **navi e aerei grandi fino a 220 km; periscopi a 10–20 km**; zattera a ~15 km | `[RWKA25]` `[MPPUFF]` | media (dati RU), bassa (nome NATO) |

**Verifiche richieste.** "Big Bulge" è il radar del **Ka-25Ts Hormone-B** (designazione bersagli per missili), non del Ka-25PL `[MPBULGE]` `[RWKA25]` — alta. Il **P-3C del 1989 monta l'APS-115**: l'APS-137 arriva sui P-3 con gli aggiornamenti degli anni '90 `[APL97]` — media.

### 1.5 Sottomarini

| Classe | Radar | Banda | Pot. | ESM (vedi §4) | Fonte | Conf. |
|---|---|---|---|---|---|---|
| **los_angeles** (688 Flight I) | **BPS-15** (navigazione e superficie) | I (banda X) | 35 kW picco, 13 W medi; PRF 750/1500 Hz; impulsi 0,5/0,1 µs; risoluzione 30 m | WLR-8(V)2, WLR-10, WLR-1H *n.v.*, BRD-7 | `[NAVYBPS15]` `[ARMSBPS]` `[NT688]` | alta (banda), bassa (potenza) |
| **sturgeon** | BPS-14 o BPS-15 *n.v.* | I | n.v. | WLR-8 *n.v.* | `[FAS688]` (dossier 01) | bassa |
| **trafalgar** | Type 1006 (Kelvin Hughes) *n.v.* | I | n.v. | UAP *n.v.* | — | bassa |
| **alfa** (Pr.705) | **RLK "Chibis"** (Wikipedia EN: "Topol MRK-50, Snoop Tray") | I | n.v. | **"Bukhta"** (deepstorm la elenca come "РЛС", Wikipedia EN come ESM/ECM) | `[DSRM705]` `[WPALFA]` | media (nomi), bassa (NATO) |
| **akula** | "Chibis" | I | n.v. | "Bukhta" ESM/ECM | `[WPAKULA]` | media |
| **victor3**, **charlie2**, **oscar**, **typhoon** | serie **Snoop**: Snoop Tray (RLK-101 Albatros) o Snoop Pair (MRK-50 Kaskad) | I | n.v. | Brick Pulp / Brick Group / Rim Hat *n.v.*; Oscar: antenna dati satellitari **Punch Bowl** | `[MPSNOOP]` `[WPORANGE]` `[NAVYP387]` `[WPOSCAR]` | media (famiglia), bassa (fit per classe) |
| **kilo**, **foxtrot** | Snoop Tray / Snoop Plate *n.v.* | I | n.v. | Stop Light *n.v.* (o Brick Pulp sul Kilo) | `[MPSNOOP]` `[CMANO]` | bassa |

La serie Snoop è "in banda I, montata su albero, per ricerca di superficie", dal Whiskey dei primi anni '50 in poi `[MPSNOOP]` — media. Le fonti abbinano in modo non coerente MRK-50 a Snoop Tray o Snoop Pair; l'Albatros viene dato come RLK-101 o RLK-50 `[WPORANGE]` `[NAVYP387]` — bassa sull'abbinamento.

### 1.6 Missili

| Missile | Emettitore | Banda | Quando emette | Fonte | Conf. |
|---|---|---|---|---|---|
| **harpoon** (Block 1B/1C) | seeker attivo Texas Instruments **AN/DSQ-28**, agile in frequenza, a due assi | J | dall'accensione a distanza preimpostata (RBL) o subito (BOL, settore ±45°) | `[DSHARP]` `[PROC77]` | alta (banda) |
| harpoon | **PR-53/DSQ-58** | J *n.v.* | seeker di versioni successive: **non usarlo per il 1989** | — | bassa |
| **tomahawk** (TASM) | seeker **DSQ-28** derivato dall'Harpoon + ricevitore ESM passivo | J (attivo) | ricerca a serpentina; attivo solo nella fase finale | dossier 05 `[DS84]` `[DS109]` | media |
| **p700** (Granit) | seeker radar attivo con home-on-jam; in salva un missile sale e designa (descrizione dubbia) | J *n.v.* | fase terminale; lo "sciame" alterna missile alto emittente e missili bassi | `[WPGRANIT]`, dossier 05 §3.3 | bassa (banda), bassa (sciame) |
| **bqm74** (Chukar) | nessun radar proprio; può portare augmentatori passivi o attivi | — | — | `[DSBQM74]` | media |

---

## 2. Firma degli emettitori per l'ESM

Che cosa misura un ESM del 1989. Il WLR-8(V) misura **direzione d'arrivo, frequenza, tipo di modulazione, frequenza di ripetizione degli impulsi (PRF), durata d'impulso e ampiezza** `[FASWLR8]` — alta. Il periodo di rotazione dell'antenna si ricava dall'intervallo fra due passaggi del fascio principale.

Per il gioco servono quattro attributi: **banda**, **periodo di scansione**, **classe di PRF**, **comportamento** (rotante, settore, puntato fisso). Valori STIMA dove non indicato.

| Firma | Banda | Periodo | PRF | Comportamento | Identifica… | Ambiguità | Conf. |
|---|---|---|---|---|---|---|---|
| SPS-40 | B | 4–8 s | bassa | rotante | DD Spruance, AOE | — | media |
| SPS-49 | C | 5 / 10 s | bassa | rotante | FFG-7, CVN, BB | — | alta (banda, periodo) |
| SPS-48 | E/F, scansione elettronica in elevazione | 4 / 8 s | media | rotante 3D | CVN, Kidd | Head Net, Strut Curve (stessa banda) | media |
| SPS-55 | I/J | 3,75 s | media | rotante | FFG, DD, DDG | navigazione sovietica e civile | alta |
| SPS-10 / SPS-67 | G | 4 s (SPS-67 anche 2 s) | media | rotante | BB, CVN, AOE | — | media |
| Mk 92 CAS / STIR, SPG-60, SPG-51 | I/J | — / puntato | alta | inseguimento o illuminazione | **ingaggio aereo in corso** | — | media |
| Top Sail / Top Pair / Top Steer / Top Plate | C/D/E | 5–10 s | bassa | rotante | grande unità sovietica (Kara, Kirov, Slava) | fra loro: si distinguono per numero di bande simultanee | media |
| Head Net-C | E/F (o D) | ~6 s *STIMA* | bassa | rotante | Kara, Krivak | SPS-48 | bassa |
| Don-2, Don Kay, Palm Frond | I | 2,5–3 s | media-alta | rotante | qualsiasi nave sovietica | **radar civili** | media |
| Strut Curve | E/F | ~6 s *STIMA* | bassa | rotante | Koni (e corvette) | Head Net | bassa |
| Peel Pair, Plank Shave | I/J | ~3 s *STIMA* | media | rotante | Nanuchka | navigazione | bassa |
| Band Stand | D/E/F *n.v.* | settore | — | settore / puntato | **Nanuchka che designa: lancio SS-N-9 possibile** | — | bassa |
| Pop Group, Head Light, Eye Bowl, Top Dome, Front Door, Kite Screech, Bass Tilt | G–K | puntato | alta | inseguimento | **ingaggio in corso**; Eye Bowl = SS-N-14 contro un sottomarino | — | media |
| APS-115 | I/J | ~6 s *n.v.* | bassa-media | rotante | P-3C | Wet Eye (stessa banda) | bassa |
| APS-137, modo periscopio | I/J | **0,2 s** | 2000/s | rotazione velocissima | **P-3C (o S-3) che cerca periscopi** | nessuna: firma unica | alta |
| APS-124 | I/J | 0,5–1 s | 470–1880 Hz | rotante veloce | SH-60B | — | media |
| Wet Eye | I | ~6 s *STIMA* | bassa *STIMA* | rotante o settore | Il-38, Tu-142 | APS-115 | bassa |
| Initsiativa-2K | I | ~4 s *STIMA* | — | rotante | Ka-25PL | navigazione | bassa |
| Navigazione civile X / S | I + E/F | 2,5–3 s | 1–3 kHz *n.v.* | rotante; spesso due radar insieme | mercantile, petroliera | Don-2 | bassa |
| BPS-15, Snoop | I | ~3 s *STIMA* | 750/1500 Hz (BPS-15) | rotante | sottomarino a quota periscopica o in emersione | navigazione | media |
| DSQ-28 (Harpoon, TASM), seeker Granit | J | settore rapido, poi puntato | alta | ricerca a settore, poi aggancio | **missile in arrivo** | — | media |

**Regola di gioco.** Un emettitore "puntato" (illuminatore, seeker agganciato) vale molto più di uno rotante: è un **allarme di minaccia**, non un contatto.

---

## 3. Dottrina EMCON nel 1989

### 3.1 Principi comuni
- **EMCON** = uso selettivo e controllato degli emettitori elettromagnetici e acustici. È "un variatore più che un interruttore", con molti livelli fra tutto acceso e tutto spento `[PROC23]` — media.
- **Nomi delle condizioni.** Le marine NATO usano condizioni a lettere fissate dall'ordine operativo. "EMCON Alpha" indica di solito il livello più restrittivo (tutto spento). Il significato di Bravo, Charlie e Delta **varia secondo la fonte** e secondo il piano EMCON del gruppo `[PROC23]` — bassa sui nomi. Per il gioco conviene usare nomi propri (§6.2) e non attribuire significati falsi alle lettere.

### 3.2 Gruppi di superficie USA
- **Cultura dell'EMCON spinto.** Dagli anni '50 la US Navy pratica operazioni disperse in EMCON contro la sorveglianza oceanica sovietica `[NWCANG]` — media. Negli anni '80 si aggiungono inganni: unità minori che simulano la portaerei (ad esempio una nave rifornimento con firma radar aumentata e TACAN acceso) `[CIMSEC2]` — media.
- **NORPAC 82 (esempio documentato).** Il gruppo transitò in **silenzio radio, radar ed elettronico completo**. Navi solo in ascolto. Aerei lanciati e recuperati senza trasmettere. E-2 in profilo passivo, attivo solo su chiamata. Per **quattro giorni** non fu identificato, pur operando a meno di 200 nm da un aeroporto sovietico delle Curili. Si scoprì da solo intercettando un gruppo di Badger `[NWNORPAC]` — media (resoconto di un partecipante).
- **Eccezioni pratiche.** Per sicurezza (uomo in mare, traffico, nebbia) si accesero radar e UHF in chiaro `[NWNORPAC]`. In generale si usavano radar di navigazione simili a quelli civili per non farsi classificare `[NWNORPAC]` — media.
- **Comportamento da modellare** (STIMA, coerente con le fonti):
  - **Transito sotto minaccia di ricognizione:** ricerca aerea e tiro **spenti**. Navigazione a bassa potenza permessa, oppure spenta. Sonar attivo spento. L'ASW si fa con schiere trainate ed elicotteri.
  - **Scoperta nemica accertata** (ombra sovietica, Bear in vista, attacco iniziato): ricerca aerea **accesa** (SPS-49/48/40). Non c'è più nulla da nascondere.
  - **Minaccia aerea o missilistica:** tutto acceso, tiro compreso.
  - **Siluro in acqua:** non cambia i radar. Cambia il sonar (attivo e Nixie accesi). Lo SPS-55 acceso aiuta a vedere il periscopio dell'attaccante.
  - **Fregate ASW in schermo avanzato:** EMCON più severo del corpo principale, per non indicare l'asse del gruppo.

### 3.3 Aerei ed elicotteri USA
- **P-3C.** Il radar rivela l'aereo all'ESM del sottomarino, che si immerge prima di essere visto (dossier 02 §7.7). Da qui l'uso **intermittente** del radar nella ricerca di periscopi e snorkel: brevi accensioni ("flash") a intervalli irregolari, invece della rotazione continua — bassa (prassi nota, ma nessuna fonte aperta con numeri trovata in questa sessione, *n.v.*).
- **Uso continuo voluto ("flooding").** Al contrario, un radar acceso di continuo **costringe il sottomarino diesel a restare sotto**, scaricando la batteria. È la tattica alleata del 1943 contro gli U-boot dotati di Metox — media (ricordato, *n.v.*). Nel gioco vale contro Kilo e Foxtrot in snorkel.
- **Radar ottimizzati.** La risposta tecnologica all'ESM fu il radar a scansione velocissima (APS-116/137): 25 giri in 5 s, con integrazione su più giri, per cogliere esposizioni brevi `[APL97]` — alta. Nel 1989 è sull'S-3, non ancora sul P-3C.
- **SH-60B.** Il radar serve soprattutto alla scoperta di superficie e alla designazione oltre l'orizzonte (fascio stretto, rotazione fino a 120 rpm) `[ARMSAPS124]`. In ASW il LAMPS lavora con boe e MAD; il radar si accende a cicli — STIMA.

### 3.4 Lato sovietico
- **Navi.** Jon Solomon descrive l'EORSAT sovietico come dipendente dalle emissioni delle navi NATO: l'EMCON occidentale lo rendeva cieco `[CIMSEC1]` — media. Che le navi sovietiche emettessero di abitudine è un giudizio corrente nella letteratura occidentale, ma senza fonte primaria trovata — bassa. Per il gioco: i **KPUG** (gruppi di ricerca e attacco ASW, dossier 02 §7.4) tengono accesi in modo quasi continuo navigazione e ricerca aria-superficie. Spengono solo in condizioni particolari (scorta ai bastioni, imboscata) — STIMA.
- **ESM navale sovietico.** Le grandi unità imbarcano stazioni di intercettazione dedicate (Kara: MRP-15-16 "Zaliv", MRP-11-12, MRP-13-14) `[RW1134B]` — media. Un BPS-15 acceso dal 688 vicino a un KPUG viene intercettato quasi sicuramente.
- **Il-38 e Tu-142.** Le PPS "Berkut" e "Korshun" combinano radar, boe RGB e MAD `[RWIL38]` `[RWTU142]`. Nessuna fonte aperta trovata sul ciclo d'uso del radar. Per il gioco: radar **più continuo** del P-3C nelle fasi di ricerca (dottrina meno attenta all'ESM avversario, sensori più vecchi), spento nella localizzazione finale con boe e MAD — STIMA, bassa.
- **Ka-25PL.** Radar Initsiativa-2K per la ricerca a vista di periscopi (10–20 km dichiarati) e per guidare i lanci; in hovering col sonar a immersione il radar non serve `[RWKA25]` — media (dati), STIMA (ciclo).
- **Sottomarini sovietici.** Radar quasi mai usato in pattuglia. ESM sempre in ascolto a quota periscopica e in emersione — STIMA, coerente con dossier 02 §7.7.

---

## 4. ESM dei sottomarini

### 4.1 688 Flight I

| Parametro | Valore | Fonte | Conf. |
|---|---|---|---|
| Ricevitore tattico | **AN/WLR-8(V)2**, ESM tattico dei 688; unità in *pool* ruotate fra i battelli | `[FASWLR8]` | alta |
| Copertura | **da 50 MHz alla banda J** (≈ fino a 18–20 GHz); kit di modifica successivi la estendono all'EHF | `[FASWLR8]` | alta (estremi in lettere), media (GHz) |
| Architettura | 7 ricevitori supereterodina accordati a YIG e vettoriali; scansione sequenziale o simultanea delle bande | `[FASWLR8]` | alta |
| IFM | modifica con misura istantanea di frequenza: probabilità d'intercetto "vicina al 100%" in banda SHF, anche per emissioni brevi | `[FASWLR8]` | alta (esistenza), *data d'installazione n.v.* |
| Avviso radar | **AN/WLR-10** "radar warning" | `[NT688]` `[WP688]` | media |
| Altri | WLR-1H (intercettatore più vecchio), BRD-7 (radiogoniometro comunicazioni) | `[NT688]` | media |
| Antenne | sul periscopio di ricerca **Type 18** (ESM integrato) e su albero dedicato; il segnale entra dall'albero d'immagine | `[FAS688]` `[FY98MDF]` `[DOTEBLQ10]` | media |
| Precisione di rilevamento | non pubblicata. Il DF monopulso sul Type 18 è un miglioramento successivo (bilancio FY98) → nel 1989 DF ad ampiezza, **5–10° *STIMA*** | `[FY98MDF]` | bassa |
| Sensibilità | non pubblicata | — | — |

**Coerenza con il dossier 01.** Il dossier 01 §7.1 dà 0,5–18 GHz *n.v.*: la fonte FAS dice 50 MHz–banda J. Le due cose sono compatibili.

### 4.2 Lato sovietico (per giocare l'Alfa)
- **Alfa:** "Bukhta". Akula: "Bukhta". Nomi da deepstorm e Wikipedia EN `[DSRM705]` `[WPAKULA]` — media. Banda e sensibilità non trovate. *STIMA:* coprire 2–18 GHz con avviso in banda I/J, rilevamento grossolano (±10–15°).
- **Nomi NATO** degli ESM sovietici (Brick Pulp, Brick Group, Rim Hat, Stop Light) sono citati in letteratura; Stop Light compare solo in un database di gioco (`[CMANO]`) — bassa, *n.v.*
- **Zaliv** è una stazione RTR **di superficie** (Kara), non di sottomarino `[RW1134B]` — media. **Nakat** (Stop Light?) — *n.v.*

### 4.3 Principio del vantaggio a senso unico
- Il radar riceve un'eco che ha fatto andata e ritorno: la sua portata cresce con la **radice quarta** della potenza. L'ESM riceve il segnale diretto: la portata cresce con la **radice quadrata** `[RFESM]` — alta (fisica).
- **Esempio [CALC].** Radar in banda X da 143 kW, antenna da 30 dB, ESM omnidirezionale con soglia −60 dBm. Potenza ricevuta = 59,7 − 20·log₁₀(R in m) dBm. Lobo principale: soglia superata fino a **~970 km**. Lobi laterali a −25 dB: **~55 km**. Quindi contro i radar d'aereo e di nave **l'ESM è limitato dall'orizzonte, non dalla sensibilità**.
- **Oltre l'orizzonte.** Diffrazione e condotti d'evaporazione permettono intercetti un po' oltre l'orizzonte geometrico, soprattutto in banda I/J con mare calmo — STIMA: fattore 1,1–1,3; con condotto forte fino a 2.
- **Conseguenza.** Contro un periscopio il radar ha portata di pochi nm (§5). L'ESM sente lo stesso radar a tutto l'orizzonte (30–90 nm per un aereo). Il vantaggio dell'ESM è quindi di **5–20 volte** contro un aereo e di **2–4 volte** contro una nave. È il caso "fino all'orizzonte" del dossier 02 §7.7. Il fattore "1,5–3 volte" dello stesso dossier resta il limite prudente per radar bassi, lobi laterali e mare mosso.

### 4.4 Come l'equipaggio stima la minaccia (prassi plausibile, STIMA)
1. **Intercetto:** banda, rilevamento, periodo di scansione, PRF → **identificazione** dalla libreria (§2).
2. **Ampiezza:** il segnale ricevuto nei passaggi del lobo principale si confronta con la potenza nota dell'emettitore → **distanza stimata** con errore di ±50–100% (potenza, guadagno e altezza incerti).
3. **Tendenza:** ampiezza che cresce di più di 3 dB in un minuto, o rilevamento che si sposta → "in avvicinamento".
4. **Soglia di contro-scoperta:** l'equipaggio confronta l'ampiezza con l'ampiezza alla quale quel radar vedrebbe l'albero esposto. Se il segnale è sopra soglia, il radar **può già vedere** il sottomarino → abbassare tutto e scendere.
5. **Preavviso dei pattugliatori:** un radar d'aereo appare a 40–90 nm. Un P-3 a 300 kn copre 40 nm in 8 min: è il tempo per immergersi e cambiare quota (dossier 02 §7.7).

---

## 5. Scoperta radar di periscopio, albero e snorkel

### 5.1 Fatti dalle fonti
- Ogni albero esposto è scopribile da un radar aereo. Nei radar APS-116/137 la decisione spetta a un operatore attento sullo schermo PPI. In acque costiere il traffico e i detriti saturano lo schermo `[APL97]` — alta.
- Modo periscopio APS-116/137: banda X, risoluzione 1 ft, fascio 2,4°, 2000 impulsi/s, **300 rpm**; rivelatore automatico ARPDD a 13 su 25 giri (5 s) `[APL97]` — alta.
- La curva teorica di sensibilità RCS (piattaforma a **1000 ft**, mare 1–5) mostra che la soglia sale molto col mare e con la distanza; oltre una certa distanza il limite è il rumore e non il clutter `[APL97]` (lettura del grafico: bassa).
- I "picchi" di clutter di mare possono durare **molti secondi** e muoversi come bersagli: sono il principale falso periscopio `[APL97]` — alta.
- Un equipaggio disciplinato tiene il periscopio fuori **meno di 10 s** in attacco `[SBIR-mast]` — alta (dossier 02). I radar per periscopi lavorano da quote di 150–450 m `[SBIR-mast]`.
- Ka-25PL Initsiativa-2K: periscopi a **10–20 km** (dato sovietico, condizioni non specificate) `[RWKA25]` — media.
- Le misure di RCS dei veri alberi di sottomarino sono **riservate**; in letteratura aperta solo pochi lavori (2012). La retrodiffusione comprende albero, scia dell'albero e clutter circostante `[SBIR-mast]` — alta.

### 5.2 Ordini di grandezza (STIMA, coerenti col dossier 02 §7.7)

| Bersaglio | RCS banda X | Note |
|---|---|---|
| Periscopio d'attacco Type 2, 1 m fuori | 0,1–0,3 m² | testa piccola; quasi invisibile con mare ≥ 4 |
| Periscopio di ricerca o albero ESM, 1–2 m fuori | 0,3–1 m² | |
| Più alberi insieme (radio + ESM + periscopio) | 1–3 m² | |
| Snorkel con scarico e scia | 3–10 m² | scarico e scia visibili anche al radar |
| Pennacchio (*feather*) a velocità > 5–8 kn | ×2–3 | dossier 02 §7.7 |
| Sottomarino in emersione | 100–1000 m² | |

Con radar limitato dal rumore, la portata scala come **RCS^¼**. Se un radar vede una nave da 5000 m² a 100 nm, vede 1 m² a ~12 nm e 0,1 m² a ~7 nm `[CALC]`. Il clutter di mare abbassa molto questi valori.

### 5.3 Il rischio cresce col tempo d'esposizione
- La probabilità cumulativa è `P = 1 − Π(1 − p_i)` sui giri d'antenna durante l'esposizione.
- Esposizione di 8 s contro un radar a 6 s di periodo: **1–2 occasioni**. Esposizione di 60 s: **10 occasioni**. Snorkel di 1–2 ore: certezza se il radar è nel raggio.
- Contro un radar a 0,2 s (APS-137) anche 5 s di esposizione danno **25 occasioni**: è la ragione del modo periscopio `[APL97]`.

---

## 6. Proposta per il gioco

### 6.1 Tabella degli emettitori (STIMA salvo dove indicato)

`R50` = distanza alla quale la probabilità per giro contro un periscopio/albero da **0,5 m²** vale 0,5, con operatore allertato. Orizzonte sempre applicato. ESM = portata d'intercetto massima contro un'antenna ESM a 1,5 m.

| Chiave radar | Piattaforme | Banda | Periodo (s) | h (m) | R50 periscopio SS2 (nm) | R50 SS4 (nm) | R50 snorkel SS2 (nm) | Vs nave grande (nm) | ESM intercetto |
|---|---|---|---|---|---|---|---|---|---|
| `aps115` | p3c | I/J | 6 | quota aereo | 6 | 2 | 15 | 100 | orizzonte ×1,1 |
| `aps137_peri` (opz.) | p3c 1990+ | I/J | 0,2 | quota | 8 | 3,5 | 20 | 120 | orizzonte ×1,1 |
| `aps124` | sh60b | I/J | 0,75 | quota | 5 | 2 | 12 | 60 | orizzonte ×1,1 |
| `wet_eye` | il38, tu142 | I | 6 | quota | 4 | 1,5 | 12 | 80 | orizzonte ×1,1 |
| `initsiativa2k` | ka25 | I | 4 | quota | 4 | 1,5 | 10 | 60 (220 km dichiarati) | orizzonte ×1,1 |
| `sps55` | perry, kidd, spruance | I/J | 3,75 | 25 | 5 | 2 | 10 | orizzonte | 14 nm ×1,2 |
| `sps10_67` | iowa, nimitz, sacramento | G | 4 | 35 | 4 | 1,5 | 9 | orizzonte | 16 nm ×1,2 |
| `mk92_cas` | perry | I/J | 1 | 25 | 4 | 1,5 | 9 | 25 | 14 nm ×1,2 |
| `spq9` | kidd, spruance | I/J | 1 | 25 | 4 | 1,5 | 9 | 20 | 14 nm ×1,2 |
| `sps49` | perry, iowa, nimitz | C | 5 (10 lungo raggio) | 30–40 | 1 | 0 | 4 | orizzonte | 15–17 nm ×1,3 |
| `sps48` | kidd, nimitz | E/F | 4–8 | 35–45 | 1,5 | 0,5 | 5 | orizzonte | 16–17 nm ×1,3 |
| `sps40` | spruance, sacramento | B | 4–8 | 30 | 0 | 0 | 2 | orizzonte | 15 nm ×1,3 |
| `top_sail_pair_steer` | kara, kirov, slava | C/D/E | 6–10 | 40–50 | 1 | 0 | 4 | orizzonte | 17–18 nm ×1,3 |
| `head_net_c`, `strut_curve` | kara, krivak2, koni | E/F | 6 | 25–30 | 1,5 | 0,5 | 5 | orizzonte | 14–15 nm ×1,3 |
| `nav_sov` (Don-2, Don Kay, Palm Frond) | tutte le navi sovietiche | I | 3 | 20–30 | 4 | 1,5 | 9 | orizzonte | 13–15 nm ×1,2 |
| `peel_pair`, `plank_shave` | nanuchka | I/J | 3 | 20 | 4 | 1,5 | 9 | orizzonte | 13 nm ×1,2 |
| `nav_civ` (X + S) | mercantile, cargo, petroliera | I + E/F | 2,5 | 25–40 | 2 (operatore non allertato) | 0,5 | 6 | orizzonte | 14–17 nm ×1,2 |
| `fc_*` (tiro: SPG-60, STIR, Pop Group, Head Light, Eye Bowl, Top Dome, Front Door, Kite Screech, Bass Tilt) | varie | G–K | puntato | 15–25 | solo se puntato sul bersaglio già noto | — | — | 10–40 | 12–14 nm |
| `bps15`, `snoop` | sottomarini | I | 3 | 6–10 | — (uso del giocatore: vede navi a 10–12 nm) | — | — | 10–12 | 10 nm ×1,2 |
| `dsq28` (Harpoon, TASM), `granit_seeker` | missili | J | settore 0,5 | 5–15 (quota del missile) | — | — | — | 10–20 | orizzonte missile-albero: 10–18 km |

Per mare intermedio interpolare in modo lineare; per mare ≥ 5 usare `R50 periscopio = 0` e `R50 snorkel = 0,3 × SS2`.

### 6.2 Macchina a stati EMCON per l'IA

| Stato | Emettitori accesi | Chi lo usa |
|---|---|---|
| **E0 SILENZIO** | nessuno (solo ESM e passivo) | gruppi USA in transito sotto minaccia; sottomarini; Nanuchka in imboscata |
| **E1 NAVIGAZIONE** | solo radar di navigazione/superficie, eventualmente a bassa potenza | default delle navi USA in tempo di pace; default minimo sovietico |
| **E2 RICERCA** | navigazione + ricerca aerea/superficie | default dei KPUG sovietici; gruppi USA scoperti |
| **E3 INTERMITTENTE** (solo aerei) | radar acceso per `t_on`, spento per `t_off` | P-3C, Il-38, Tu-142, SH-60B, Ka-25 in ricerca |
| **E4 INGAGGIO** | tutto, tiro compreso, puntato sul bersaglio | attacco in corso, difesa aerea |

**Transizioni** (STIMA):

| Evento | Navi USA | Navi sovietiche | Aerei |
|---|---|---|---|
| Inizio missione | E0 (minaccia) o E1 | E2 | E3 nell'area, radar spento in transito sopra 5000 ft |
| Gruppo scoperto dal nemico (ombra, ESM, missile) | → E2 | — | — |
| Contatto aereo ostile / missile in arrivo | → E4 | → E4 | — |
| Contatto sonar o datum su sottomarino | resta; attiva sonar ed elicotteri | → E2 + sonar attivo | P-3C → radar **spento** e boe/MAD (localizzazione); Il-38 → E3 rapido |
| Periscopio o snorkel visto al radar | → E4 sul settore | → E4 sul settore | → radar **continuo** finché il contatto è tenuto |
| Siluro nemico in acqua | invariato (sonar attivo, Nixie) | invariato | invariato |
| Lancio di missili antinave proprio | E4 | Nanuchka/Slava: designatore (Band Stand, Front Door) acceso 1–3 min prima del lancio | — |
| Contatto perso da > 20 min | ritorno allo stato di partenza | ritorno a E2 | ritorno a E3 |
| Nebbia, traffico, notte vicino costa | almeno E1 | E2 | — |

**Cicli intermittenti E3** (STIMA):

| Aereo | t_on | t_off | Note |
|---|---|---|---|
| P-3C, ricerca di periscopi | 1–3 giri (6–18 s) | 2–8 min casuali | "flash" irregolari per non dare un ritmo all'ESM |
| P-3C, modo *flooding* contro diesel | continuo | 0 | dichiarato nel briefing della missione |
| Il-38 / Tu-142 | 2–5 min | 2–5 min | dottrina meno attenta all'ESM |
| SH-60B | 30–60 s | 3–5 min | più continuo in sorveglianza di superficie |
| Ka-25PL | continuo nelle tratte di ricerca | spento in hovering col sonar | |

Il radar d'aereo si accende **sempre** quando l'aereo scende sotto i 1000 ft per un attacco su un contatto in superficie o a quota periscopica.

### 6.3 Modello del ricevitore ESM

**Condizione d'intercetto** (verificata a ogni passaggio del fascio, cioè una volta per periodo dell'emettitore):
1. il sottomarino ha un'antenna ESM fuori dall'acqua (periscopio Type 18 o albero ESM) — quota ≤ 18 m di chiglia (dossier 01);
2. l'emettitore è acceso e la sua banda è coperta (WLR-8: 0,05–20 GHz; "Bukhta": 2–18 GHz STIMA);
3. `d ≤ k·d_orizzonte(h_radar, 1,5 m)`, con k dalla tabella 6.1;
4. tiro casuale con `P_int` per passaggio: **0,95** in E–J con IFM (688), **0,8** in B–D, **0,7** per l'ESM sovietico (STIMA).
Per emettitori puntati (tiro, seeker): intercetto immediato se il bersaglio puntato è il sottomarino o se l'antenna sta nel fascio; altrimenti `P_int` × 0,3 sui lobi laterali entro 1/3 della portata (STIMA).

**Rilevamento.** Errore gaussiano σ = **5°** (688, I/J), **8°** (E–H), **12°** (B–D); ESM sovietico ×1,5. Ogni nuovo passaggio riduce l'errore del fix medio come σ/√n fino a un minimo di 2°.

**Identificazione.** Servono **2 passaggi** per misurare il periodo e **3** per proporre un'identità.
- Firma unica (APS-137 a 0,2 s, SPS-49, SPS-40, Top Sail/Pair, Band Stand): `P_id = 0,9`.
- Firma condivisa (navigazione in banda I, APS-115 contro Wet Eye, Head Net contro SPS-48): `P_id = 0,6`; altrimenti il gioco mostra la **classe generica** ("radar di navigazione banda I", "radar ASW aereo banda X").
- Errore: con `P = 0,1` mostra un'identità sbagliata dello stesso gruppo d'ambiguità.

**Stima di distanza e tendenza.**
- `R_stim = R_vera · 10^(N(0; 0,15))` (errore di circa ±40% a 1σ).
- Tendenza dopo ≥ 60 s: "in avvicinamento" se l'ampiezza sale > 3 dB/min; "in allontanamento" se scende > 3 dB/min; altrimenti "stabile".
- Mostrare l'ampiezza su scala 1–5 (1 = appena sopra soglia, 5 = vicino), più utile al giocatore di un valore in dBm.

**Rischio di contro-scoperta.**
`ρ = R_stim / R50(radar, esposizione, mare)`
- `ρ > 2`: **SICURO** — il radar non vede l'albero.
- `1 < ρ ≤ 2`: **ATTENZIONE** — limitare l'esposizione a < 10 s.
- `ρ ≤ 1`: **CONTRO-SCOPERTA PROBABILE** — abbassare gli alberi, scendere.
- Emettitore puntato o firma APS-137: **SCOPERTO** a prescindere da ρ.

### 6.4 Scoperta radar dell'albero per giro d'antenna

Per ogni giro del radar durante l'esposizione:

`p_giro = F_op · F_vel / (1 + (R / R50_eff)^4)`

- `R50_eff = R50(tabella 6.1, mare) · (σ / 0,5 m²)^¼`, con σ dalla tabella §5.2.
- `F_op` = fattore operatore: **0,3** non allertato (mercantile, radar in navigazione), **0,7** allertato, **0,9** con datum noto e settore ristretto.
- `F_vel` = **1** a ≤ 5 kn; **1,5** a 5–8 kn; **2** oltre 8 kn (pennacchio), limitato a `p_giro ≤ 0,95`.
- Cumulata: `P = 1 − Π(1 − p_giro)` sui giri durante l'esposizione.
- Notte: invariato (il radar non dipende dalla luce). Pioggia forte: R50 × 0,7 in banda I/J.
- **Falsi allarmi** (picchi di clutter): con mare ≥ 3, ogni radar allertato genera un falso "periscopio" ogni 10–20 min, che l'IA insegue come un contatto reale per 2–5 min (riprende `[APL97]`).

**Esempio [CALC].** P-3C con APS-115 a 1000 ft, mare 2, operatore allertato (`F_op` 0,7), periscopio di ricerca 0,5 m², sottomarino a 5 kn, distanza 5 nm. `p_giro = 0,7 / (1 + (5/6)^4) = 0,7 / 1,48 = 0,47`. Esposizione di 8 s (1–2 giri) → P ≈ 0,47–0,72. Esposizione di 60 s (10 giri) → P ≈ 0,998. Nello stesso momento l'ESM del 688 sente l'APS-115 già a ~41 nm (orizzonte a 1000 ft): il giocatore che guarda l'ESM ha **8 minuti** di preavviso.

---

## 7. Fonti

### 7.1 Radar USA
- `[WPSPS49]` Wikipedia, *AN/SPS-49*: https://en.wikipedia.org/wiki/AN/SPS-49
- `[WPSPS48]` Wikipedia, *AN/SPS-48*: https://en.wikipedia.org/wiki/AN/SPS-48
- `[WPSPS10]` Wikipedia, *AN/SPS-10*: https://en.wikipedia.org/wiki/AN/SPS-10
- `[WPSPS40]` Wikipedia, *AN/SPS-40*: https://en.wikipedia.org/wiki/AN/SPS-40
- `[FASSPS55]` FAS, *AN/SPS-55*: https://man.fas.org/dod-101/sys/ship/weaps/an-sps-55.htm
- `[WPSPS67]` Wikipedia, *AN/SPS-67*: https://en.wikipedia.org/wiki/AN/SPS-67 ; radartutorial: https://www.radartutorial.eu/19.kartei/07.naval/karte036.en.html
- `[NAVYSPS67]` US Navy fact file, *AN/SPS-67(V)*: https://www.navy.mil/Resources/Fact-Files/Display-FactFiles/Article/2167975/ansps-67v-radar-set/
- `[FASMK92]` FAS, *Mk 92 FCS*: https://man.fas.org/dod-101/sys/ship/weaps/mk-92-fcs.htm
- `[WPKIDD]` Wikipedia, *Kidd-class destroyer*: https://en.wikipedia.org/wiki/Kidd-class_destroyer
- `[WPSPRU]` Wikipedia, *Spruance-class destroyer*: https://en.wikipedia.org/wiki/Spruance-class_destroyer
- `[WPSAC]` Wikipedia, *Sacramento-class fast combat support ship*: https://en.wikipedia.org/wiki/Sacramento-class_fast_combat_support_ship
- `[NAVYBPS15]` US Navy fact file, *AN/BPS-15/16*: https://www.navy.mil/Resources/Fact-Files/Display-FactFiles/Article/2166767/anbps-1516-radar/
- `[ARMSBPS]` Armsnet, scheda BPS-15 (fonte debole): https://armsnet.duckdns.org/equipment/62f4faf4-e282-4772-8baf-08decb838d08

### 7.2 Radar aerei
- `[APL97]` J.J. Ousborne, D. Griffith, R.W. Yuan, "A Periscope Detection Radar", *JHU APL Technical Digest* 18(1), 1997: https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V18-N01/18-01-Ousbourn.pdf
- `[SBIR-mast]` Navy SBIR N192-059, *Submarine Mast Discrimination Techniques*: https://www.navysbir.com/n19_2/N192-059.htm
- `[WPAPS]` Wikipedia, *AN/APY-10* (storia APS-80/115): https://en.wikipedia.org/wiki/AN/APY-10
- `[RTAPS]` radartutorial, schede radar aerei: https://www.radartutorial.eu/19.kartei/11.ancient3/karte050.en.html
- `[ARMSAPS124]` Armsnet, scheda APS-124: https://armsnet.duckdns.org/equipment/bf7d9ad5-b8a0-46d8-8b9d-08decb838d08
- `[MPAPS124]` Military Periscope, *AN/APS-124*: https://www.militaryperiscope.com/weapons/sensorselectronics/airborne-radars/anaps-124/
- `[MPWETEYE]` Military Periscope, *Wet Eye*: https://www.militaryperiscope.com/weapons/sensorselectronics/airborne-radars/wet-eye/
- `[MPPUFF]` Military Periscope, *Puff Ball*: https://www.militaryperiscope.com/weapons/electronics/airborne-radars/puff-ball/overview/
- `[MPBULGE]` Military Periscope, *Big Bulge*: https://www.militaryperiscope.com/weapons/sensorselectronics/airborne-radars/big-bulge/
- `[WPIL38]` Wikipedia, *Ilyushin Il-38*: https://en.wikipedia.org/wiki/Ilyushin_Il-38
- `[RWIL38]` Wikipedia RU, *Ил-38*: https://ru.wikipedia.org/wiki/Ил-38
- `[RWTU142]` Wikipedia RU, *Ту-142*: https://ru.wikipedia.org/wiki/Ту-142
- `[RWKA25]` Wikipedia RU, *Ка-25*: https://ru.wikipedia.org/wiki/Ка-25
- `[NPS04]` J.M. Landa Borges, *Radar search and detection with the CASA 212 S43 aircraft*, tesi NPS, 2004 (modello di probabilità cumulativa; non letto per intero): https://calhoun.nps.edu/entities/publication/13b5f94f-c31f-4b97-ac22-200db41e12fd

### 7.3 Radar e navi sovietiche
- `[WPKARA]` Wikipedia, *Kara-class cruiser*: https://en.wikipedia.org/wiki/Kara-class_cruiser
- `[RW1134B]` Wikipedia RU, *Большие противолодочные корабли проекта 1134Б*: https://ru.wikipedia.org/wiki/Большие_противолодочные_корабли_проекта_1134Б
- `[WPKIROV]` Wikipedia, *Kirov-class battlecruiser*: https://en.wikipedia.org/wiki/Kirov-class_battlecruiser
- `[WPSLAVA]` Wikipedia, *Slava-class cruiser*: https://en.wikipedia.org/wiki/Slava-class_cruiser
- `[WPKRIVAK]` Wikipedia, *Krivak-class frigate*: https://en.wikipedia.org/wiki/Krivak-class_frigate
- `[RW1135]` Wikipedia RU, *Сторожевые корабли проекта 1135*: https://ru.wikipedia.org/wiki/Сторожевые_корабли_проекта_1135
- `[WPKONI]` Wikipedia, *Koni-class frigate*: https://en.wikipedia.org/wiki/Koni-class_frigate
- `[WPNAN]` Wikipedia, *Nanuchka-class corvette*: https://en.wikipedia.org/wiki/Nanuchka-class_corvette
- `[GSNAN]` GlobalSecurity, *Pr.1234 specs*: https://www.globalsecurity.org/military/world/russia/1234-specs.htm
- `[WPNATO]` Wikipedia, *List of NATO reporting names for equipment*: https://en.wikipedia.org/wiki/List_of_NATO_reporting_names_for_equipment
- `[QSL]` N9ZIA, *Soviet Radars* (lista con bande): https://www.qsl.net/n/n9zia/soviet_radars.html
- `[RTTOP]` radartutorial, *Fregat-MA (Top Plate)*: https://www.radartutorial.eu/19.kartei/07.naval/karte091.en.html
- `[RTHEAD]` radartutorial, *MR-310 Angara-A*: https://www.radartutorial.eu/19.kartei/11.ancient3/karte090.en.html
- `[MPHEAD]` Military Periscope, *MR-310 Head Net*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/mr-310-head-net/
- `[MPTOPSAIL]` Military Periscope, *Top Sail*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/top-sail/
- `[MPKITE]` Military Periscope, *MP 184 Kite Screech*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/mp-184-kite-screech/
- `[MPBASS]` Military Periscope, *Bass Tilt*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/bass-tilt/
- `[MPPEEL]` Military Periscope, *Peel Pair*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/peel-pair/
- `[MPPLANK]` Military Periscope, *Plank Shave*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/plank-shave/
- `[MPFRONT]` Military Periscope, *Front Door/Front Piece*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/front-doorfront-piece/

### 7.4 Sottomarini ed ESM
- `[FASWLR8]` FAS, *AN/WLR-8*: https://irp.fas.org/program/collect/an-wlr-8.htm
- `[FASWLR10]` FAS, *AN/WLR-10*: https://fas.org/irp/program/collect/an-wlr-10.htm
- `[NT688]` Naval Technology, *Los Angeles class*: https://www.naval-technology.com/?p=14646
- `[WP688]` Wikipedia, *Los Angeles-class submarine*: https://en.wikipedia.org/wiki/Los_Angeles-class_submarine
- `[FAS688]` FAS, *SSN-688*: https://man.fas.org/dod-101/sys/ship/ssn-688.htm
- `[FY98MDF]` FAS, bilancio Navy FY98, PE 0603562N (DF monopulso del Type 18): https://irp.fas.org/budget/fy98_navy/0603562n.htm
- `[DOTEBLQ10]` DOT&E FY2016, *AN/BLQ-10*: https://www.dote.osd.mil/Portals/97/pub/reports/FY2016/navy/2016anblq10.pdf
- `[RFESM]` RF Essentials, sensibilità ESM ed equazione a una via: https://rfessentials.com/rf-knowledge-base/what-is-the-minimum-detectable-signal-of-an-esm-receiver-and-how-does-it-determi/
- `[WPALFA]` Wikipedia, *Alfa-class submarine*: https://en.wikipedia.org/wiki/Alfa-class_submarine
- `[DSRM705]` deepstorm.ru, scheda Pr.705 ("РЛС Бухта", "РЛК Чибис", periscopio "Сигнал", ТВ-1): https://www.deepstorm.ru/DeepStorm.files/45-92/nts/705/list.htm
- `[WPAKULA]` Wikipedia, *Akula-class submarine*: https://en.wikipedia.org/wiki/Akula-class_submarine
- `[WPOSCAR]` Wikipedia, *Oscar-class submarine*: https://en.wikipedia.org/wiki/Oscar-class_submarine
- `[MPSNOOP]` Military Periscope, *Snoop series*: https://www.militaryperiscope.com/weapons/sensorselectronics/naval-radars/snoop-series/
- `[WPORANGE]` Wikipedia, *Orange Harvest*: https://en.wikipedia.org/wiki/Orange_Harvest
- `[NAVYP387]` Navypedia, K-387 (MRK-50 Kaskad → RLK-101M Albatros): https://navypedia.org/ships/russia/ru_ss_k387.htm
- `[CMANO]` cmano-db (database di gioco, solo indizio): https://cmano-db.com/submarine/250/

### 7.5 Dottrina e missili
- `[PROC23]` USNI Proceedings, giugno 2023, "Drill Emission Control…": https://www.usni.org/magazines/proceedings/2023/june/drill-emission-control-main-battery
- `[NWCANG]` R.G. Angevine, "Hiding in Plain Sight — The U.S. Navy and Dispersed Operations under EMCON, 1956–1972", *NWC Review* 64(2), 2011: https://digital-commons.usnwc.edu/nwc-review/vol64/iss2/6
- `[NWCSOL]` J.F. Solomon, "Maritime Deception and Concealment", *NWC Review* 66(4), 2013: https://digital-commons.usnwc.edu/nwc-review/vol66/iss4/7
- `[CIMSEC1]` J. Solomon, "Deception and the Backfire Bomber: Part One", CIMSEC 2016: https://cimsec.org/deception-and-the-backfire-bomber-part-one/
- `[CIMSEC2]` J. Solomon, "Deception and the Backfire Bomber: Part Two", CIMSEC 2016: https://cimsec.org/deception-and-the-backfire-bomber-part-two/
- `[NWNORPAC]` A. Pico, "How to Hide a Task Force" (NORPAC 82), NavWeaps: https://navweaps.com/index_tech/tech-031.php
- `[DSHARP]` Designation-Systems, *AGM/RGM/UGM-84 Harpoon*: https://designation-systems.net/dusrm/m-84.html
- `[PROC77]` USNI Proceedings, febbraio 1977, "Harpoon": https://www.usni.org/magazines/proceedings/1977/february/harpoon
- `[WPGRANIT]` Wikipedia, *P-700 Granit*: https://en.wikipedia.org/wiki/P-700_Granit
- `[DSBQM74]` Designation-Systems, *MQM/BQM-74 Chukar*: https://designation-systems.net/dusrm/m-74.html

### Rimandi interni
- Dossier 01 §7.1 e §7.4 (sensori del 688, sintesi radar di superficie).
- Dossier 02 §7.4 (KPUG, MPA sovietici) e §7.7 (EMCON, esposizione del periscopio, orizzonte radar, principio ESM).
- Dossier 05 (`[DS84]` `[DS109]`: DSQ-28 di Harpoon e TASM; §3.3 salva del Granit).
- Dossier 08 §7 (rischio di radiogoniometria delle trasmissioni).

### Lacune principali
- Banda, potenza e periodo esatti dei radar sovietici: solo riassunti a pagamento o liste amatoriali.
- Prestazioni dell'APS-115 (potenza, periodo, portata vs periscopio): non trovate.
- Precisione di rilevamento e sensibilità del WLR-8(V)2: non pubblicate.
- Nomi e caratteristiche degli ESM sovietici ("Bukhta", Brick Pulp, Rim Hat, Stop Light): fonti deboli o contraddittorie.
- Cicli d'uso reali dei radar di P-3C, Il-38 e Tu-142: nessuna fonte aperta con numeri.
- RCS misurate di periscopi e snorkel: riservate.
