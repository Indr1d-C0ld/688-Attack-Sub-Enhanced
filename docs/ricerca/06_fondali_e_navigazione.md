# 06 — Sonar del fondale, navigazione in immersione e carte nautiche (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. Scopo: sostituire il "side scanning sonar / contour imaging display" del gioco originale (vista pseudo-3D del fondale a prua, dritta e sinistra, senza penalità di contro-scoperta) con un modello realistico: **quello che il battello sa del fondale dipende da cosa ha effettivamente misurato, da quanto bene sa dove si trova e da quanto è buona la carta.**
> Compilato: 23 ottobre 2026. Rimandi: `01_piattaforme_e_armi.md` (sensori e piattaforme), `02_acustica_oceanografia_dottrina.md` (equazione del sonar, perdite al fondo, riverberazione), `03_teatri_fondali_storia.md` (batimetria dei teatri, dataset).

---

## 0. Convenzioni

- **Confidenza**: **alta** = fonte ufficiale/primaria o ≥2 fonti indipendenti serie; **media** = una fonte secondaria seria o dato coerente fra fonti deboli; **bassa** = fonte debole (forum, database di giochi), dato ricordato dalla letteratura e non riverificato (*n.v.*), oppure calcolo/stima del compilatore.
- **STIMA DI GIOCO** = nessun dato pubblicato; valore proposto e motivato (sempre confidenza bassa, da trattare come parametro di taratura).
- Unità: nm = miglio nautico (1852 m); kn = nodi; kyd = 1000 yd (914 m); fm = braccio (fathom, 1,83 m). Livelli in dB re 1 µPa @ 1 m.
- Assorbimento usato nei calcoli (formula di Thorp, acqua fredda-temperata; calcolo del compilatore, **media**): 3,5 kHz ≈ 0,24 dB/km; 12 kHz ≈ 1,6 dB/km; 25 kHz ≈ 6,1 dB/km; 50 kHz ≈ 17 dB/km. È l'assorbimento che rende "corti" e relativamente discreti i sonar HF, non la potenza.
- Sigle delle fonti tra parentesi quadre, elenco completo al §9.

**Verdetto anticipato sul gioco originale.** Nel 1989 **nessun 688 aveva un sonar a scansione laterale né un display che costruisse un'immagine 3D del fondale**. Avevano: (1) un ecoscandaglio a fascio singolo verso il basso, (2) il sonar HF a corto raggio **AN/BQS-15** (ostacoli, mine, ghiaccio, più sondatori verso l'alto e verso il basso), (3) la carta nautica, (4) la stima della propria posizione da un navigatore inerziale. Il "contour imaging" era una licenza da videogioco; la scansione laterale esisteva solo su mezzi speciali (es. NR-1, battelli per missioni speciali come Parche con BQS-14A [MPBQS]) e su veicoli rimorchiati. Entrambi i sensori attivi reali **si possono intercettare**: l'assenza di penalità di contro-scoperta è il secondo errore da correggere.

---

## 1. Sonar che vedono il fondo e gli ostacoli (688, 1989)

### 1.1 AN/BQS-15 — sonar HF per prossimità, ghiaccio e mine

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Funzione | Sonar attivo **a corto raggio, alta frequenza**: evitamento di contatti ravvicinati, navigazione sotto il ghiaccio, iceberg, rilevamento ed evitamento di mine | [FASBQS15] [NCBQS15] [GOAEIS] | alta |
| Banda | "mid- e high-frequency", HF = **> 10 kHz**; frequenze esatte **classificate** | [GOAEIS] (via [NCBQS15]) | alta (il limite inferiore), nessun dato sul valore |
| Frequenza operativa | **STIMA DI GIOCO: 20–30 kHz** per il canale principale (prua/vela). Motivo: i sonar anti-mina occidentali lavorano a "diverse decine di kHz" per la risoluzione angolare; un brevetto USA di sonar sotto-ghiaccio usa 20–50 kHz per la banda alta | brevetto US 5,790,474 (non è il BQS-15) | bassa |
| Trasduttori | Elemento principale: **trasduttore cilindrico nella parte anteriore della vela**; sul 688 schiera ricevente principale di dimensioni ridotte; **sondatori superiore e inferiore** ("top and bottom sounders") aggiunti per la navigazione sotto il ghiaccio | [NCBQS15] (cita Glennon & Nye 1983, Polmar 2013, Jane's UWS 2000) | media |
| Uscite/display | Dimensione e posizione degli oggetti; distanza scafo–superficie (ghiaccio o acqua libera); dimensione e posizione delle **polynye**; distanza **chiglia–fondo**. Profondità di chiglia in lettura digitale (4 cifre BCD) verso il CSDC; distanza/rilevamento/marcatori verso il calcolatore centrale tramite il quadro NAV/IC. Console ridisegnata nell'attack center del 688 | [NCBQS15] | media |
| Storia | Contratto Ametek-Straza (El Cajon, CA) metà anni '60; programma terminato dal CNO nell'ott. 1969 ma 2 modelli XN-1 consegnati; prove su USS Dolphin (AGSS-555) gen.–apr. 1971; contratto 1972 per altri set (~0,6 M$ l'uno); primo battello: **USS Los Angeles (SSN-688), 1976** | [NCBQS15] | media |
| Piattaforme | SSN-688 (tutti), SSBN-726; poi Seawolf/Virginia (versioni successive) | [FASBQS15] [MPBQS] | alta |
| 688I (1988–89) | "Upgraded BQS-15 ice detection sonar" più vela rinforzata | [GS688] | media |
| Successore | **AN/BQS-24 MIDAS** (Mine and Ice Detection Avoidance System): **anni '90, non 1989** | [MPBQS] | media |
| Portata | **Nessun dato ufficiale.** Database di gioco: 0,7 km [CMANO]; forum: "raggio di 3 nm" [ACBQS]. Calcolo del compilatore (sotto): mine ~1 km, chiglie di ghiaccio ~2,5 km, pareti rocciose ~3 km | — | bassa |
| Livello di sorgente | **Non pubblicato.** STIMA DI GIOCO **205 dB** (intervallo plausibile 200–215) | — | bassa |
| Settore | **Non pubblicato.** STIMA DI GIOCO: canale prodiero **±60° in azimut**, elevazione da −30° a +45° (scansione per settori, non un fascio panoramico); più due sondatori verticali, verso l'alto e verso il basso, con fascio stretto (≈10–15°) | — | bassa |

**Calcolo di portata (compilatore, bassa).** Attivo limitato dal rumore, f = 25 kHz, α ≈ 6,1 dB/km; SL 205 dB, NL a 25 kHz con mare forza 3 ≈ 39 dB/Hz → 66 dB su 500 Hz; DI ≈ 20 dB; DT ≈ 10 dB.
- **Mina ormeggiata** (TS ≈ −15 dB): 2TL ≤ 134 dB → **≈ 1,1 km**.
- **Chiglia di ghiaccio / iceberg** (TS ≈ +20 dB): **≈ 2,6 km**.
- **Parete di un monte sottomarino** (TS ≥ +30 dB): **≈ 3,2 km**. Oltre i 3–4 km l'assorbimento chiude comunque la portata, qualunque sia la potenza.
- Il rumore proprio cresce con la velocità e degrada le cifre sopra: **STIMA DI GIOCO** −30% a 15 kn, −60% sopra i 20 kn.
- Vicino al fondo e in acqua bassa domina la **riverberazione di fondo**: i bersagli piccoli (mine di fondo) spariscono nel "clutter".

Questi ordini di grandezza sono coerenti con la testimonianza del 1960 (USS Sargo, Stretto di Bering): il "rilevatore di iceberg" della scala lunga vedeva il ghiaccio a **circa 600 yd** [NSL84] (**media**). Il BQS-15 del 1976 è più capace, ma resta nella stessa classe: **chilometri, non decine di chilometri**.

**Conseguenza di progetto.** A 33 kn (17 m/s) un preavviso di 3 km sono **3 minuti**; a 15 kn circa 6,5 minuti. Il raggio di accostata di un 688 è dell'ordine di grandezza di qualche centinaio di metri (**STIMA DI GIOCO**: 400–800 m secondo velocità e timone). Ne segue la regola: **in acque mal cartografate la velocità sicura è limitata dalla portata del BQS-15**, come per un'auto di notte coi fari.

### 1.2 Ecoscandaglio (fathometer)

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Designazione su sottomarini USA | **AN/BQN-17** citato come "submarine fathometer" in dimostrazioni MASINT geofisiche (insieme all'AN/UQN-4 di superficie). Nessun'altra documentazione pubblica trovata; non verificato che fosse il modello dei 688 nel 1989 | testo derivato da Wikipedia "Geophysical MASINT" (via ricerca) | bassa |
| Analogo di superficie | AN/UQN-4A: misura chiglia–fondo da **4 ft a 6.000 fm**; usa elementi trasduttori a **12 kHz** | [FASUQN4] [DTIC-UQN4] | media |
| Frequenza (sottomarino) | **STIMA DI GIOCO: 12 kHz** (standard degli ecoscandagli da altura dell'epoca, per analogia con l'UQN-4) | — | bassa |
| Fascio | **STIMA DI GIOCO: cono ≈ 30°** → impronta sul fondo di diametro ≈ 0,54 × la distanza chiglia–fondo (540 m con 1000 m sotto la chiglia). Restituisce il **punto più vicino** nell'impronta: i fianchi dei rilievi appaiono "arrotondati" e i picchi stretti vengono anticipati o sottostimati [BOW26 §2608] | [BOW26] (effetto), valore stimato | bassa (valore), alta (effetto) |
| Livello di sorgente | Non pubblicato. STIMA DI GIOCO: **210 dB a piena potenza, 190 dB in modalità a bassa potenza** | — | bassa |
| Errori | Velocità del suono non uniforme → errore di profondità di qualche %; falsi fondi da pesci, bolle, strato diffondente profondo (DSL), strati di salinità; penetrazione nei sedimenti molli [BOW26 §2605–2606] | [BOW26] | alta |
| Modalità "sicure" | **Nessun manuale pubblico.** È però documentata la preoccupazione: un brevetto della US Navy depositato nel **1959** (segreto fino al **1978**) nota che il segnale di un ecoscandaglio è "inequivocabilmente artificiale" e rilevabile "a grandi distanze", rivelando presenza e rilevamento della nave; propone di emettere rumore ambiente registrato e di correlare l'eco | [PAT4086560] | alta (sul problema); bassa (sull'uso operativo) |

**Uso operativo reale (ricostruito).** Nella prassi l'ecoscandaglio non è "acceso" in continuo: si prendono **sondaggi singoli o periodici** su ordine. Esempio documentato: USS San Francisco, 8 gen. 2005, a ~33 kn e 160 m di quota, registra sondaggi alle **11:00 (8.652 ft), 11:15 (5.988 ft) e 11:30 (~6.588 ft)**, poi urta alle 11:43 [DAY05] (**media**: articolo di stampa sul rapporto d'inchiesta, *n.v.* sul testo). Un sondaggio ogni 15 minuti a 33 kn significa **8 nm fra due misure**. L'inchiesta rimprovera di non aver riconosciuto l'acqua che si alzava e di aver fatto **troppo pochi sondaggi a prua** [SFCASE]. È il comportamento da riprodurre: l'ecoscandaglio misura solo **sotto** la chiglia e solo **quando lo si usa**.

### 1.3 688I (SSN-751 e successivi): cosa cambia per il fondale e il ghiaccio

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Battelli in servizio a fine 1989 | San Juan (SSN-751, 6 ago. 1988), Pasadena (SSN-752, 11 feb. 1989), Topeka (SSN-754, 21 ott. 1989) | [WPLA] | alta |
| Timoni orizzontali | Spostati **dalla vela alla prua, retrattili** | [WPLA] [GS688] | alta |
| Vela | **Rinforzata** per sfondare il ghiaccio | [GS688] | alta |
| Sonar | BQS-15 "upgraded ice detection"; BSY-1 | [GS688] | media |
| Nuovo sonar HF dedicato | **Nessuno nel 1989** (il MIDAS/BQS-24 arriva dopo) | [MPBQS] | media |
| Configurazione | "Configured for under-ice operations" (sui 688 Flight I/II la stessa fonte non lo dice) | [WPLA] | alta |

**688 Flight I/II sotto il ghiaccio.** I timoni di vela dei 688 pre-I **non ruotano in verticale** (a differenza degli Sturgeon) e la vela non è rinforzata: questi battelli possono operare nella **zona marginale del ghiaccio** e transitare sotto la banchisa, ma **non sono idonei a emergere attraverso il ghiaccio** (**media**: deduzione coerente con [WPLA] e [GS688]; nessuna fonte ufficiale esplicita trovata). STIMA DI GIOCO: Flight I/II emersione solo in **acqua libera o in polynya**, ghiaccio ≤ 0,15 m ("nilas"); 688I fino a **≈ 0,6–1 m** di ghiaccio livellato.

### 1.4 Cosa produrrebbe davvero un'"immagine a contorni"

Nel 1989, su un 688 standard, l'unico modo onesto di ottenere un quadro del fondale è un **mosaico** costruito dall'equipaggio, o dal calcolatore, a partire da:
1. **Carta nautica** (vedi §3): la base, con errori.
2. **Sondaggi** dell'ecoscandaglio: punti lungo la rotta già percorsa.
3. **BQS-15**: ritorni a prua entro ~1–3 km, settore limitato.
4. **Sonar sferico BQQ-5 in attivo** (~3,5 kHz, vedi doc 01/02): vedrebbe un monte sottomarino o una scarpata a decine di km, ma è un "ping" a bassa frequenza e alta potenza, **udibile a distanze enormi**. È una risorsa d'emergenza, non uno strumento di navigazione.
5. **Batimetria dedotta** dalla navigazione per contorni (§2.6).

Il "contour imaging display" del gioco originale va quindi reinterpretato come un **display composito "Carta + misure"** (§7.2): bello da vedere, ma onesto su cosa è misurato e cosa è supposto.

### 1.5 Contro-scoperta dei sonar HF e dell'ecoscandaglio

Intercettazione, solo andata: `SL − TL(r) − (NL_banda − DI_int) ≥ DT_int`. Calcoli del compilatore (**bassa**, perché SL e fasci non sono pubblicati):

| Emissione | Ipotesi | Portata d'intercettazione stimata | Portata utile propria | Rapporto |
|---|---|---|---|---|
| BQS-15, nel fascio | SL 205 dB, 25 kHz, NL 69 dB su 1 kHz, DT 10 | **≈ 8 km** (≈ 10 km con SL 215) | 1–3 km | 3–8× |
| BQS-15, fuori fascio | lobi laterali −20/−25 dB | **≈ 2–4 km** | — | — |
| Ecoscandaglio 12 kHz, lobi laterali orizzontali | SL 210, −25 dB laterale, NL 76 dB | **≈ 13 km** | — | — |
| Ecoscandaglio, energia riflessa dal fondo verso l'alto | SL 210, perdita al fondo 10 dB | **≈ 15–20 km** (geometria favorevole, acqua non profonda) | profondità sotto chiglia | — |
| Ecoscandaglio a bassa potenza (−20 dB) | idem | **≈ 4–8 km** | idem (fino a ~1–2 km sotto chiglia) | — |
| BQQ-5 attivo (sfera) | vedi doc 02 | decine di nm (zone di convergenza) | decine di km | ≫ |

**Chi ascolta.** I ricevitori d'intercettazione acustica esistevano su entrambi i lati: per esempio l'Alfa aveva un ricevitore del tipo «Rubin» (STIMA, dossier 28 §2). Il suo **«Enisej»** (МГК-1001) era la stazione passiva di scoperta, non l'intercettatore come diceva Wikipedia (dossier 19, da Sutyagin; correzione del 28/10/2026). Le unità ASW di superficie e gli elicotteri con sonar immerso sentono bene l'HF solo a breve distanza (assorbimento). Contano però due fatti: un singolo ping HF identifica **un sottomarino in quel punto, con il suo rilevamento**; e in acqua bassa (Barents, Baltico, Celtico) la geometria favorisce chi ascolta.

**EMCON (dottrina ricostruita, media).** L'attivo si usa quasi solo per il tiro, sotto il ghiaccio e contro le mine (doc 02 §dottrina [Hervey; Cote]). L'ecoscandaglio si usa con sondaggi singoli quando serve, a bassa potenza se disponibile; si sospende vicino al nemico. Il BQS-15 si accende in **transito in campo minato o sotto il ghiaccio**, accettando il rischio. **STIMA DI GIOCO** dei livelli EMCON (§7.1): SILENZIO (nessun attivo) / RIDOTTO (ecoscandaglio a ping singolo, bassa potenza) / NAVIGAZIONE (ecoscandaglio periodico, BQS-15 a settore) / SICUREZZA (tutto acceso).

---

## 2. Navigazione in immersione, 1989

### 2.1 Navigatore inerziale (SINS) sui 688

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| 688 Flight I (primi battelli, sistema di combattimento Mk 113/Mk 117) | **AN/WSN-1 DMINS** (Dual Miniature Inertial Navigation System) | [WPLA] (cita il sistema sostituito dal CCS Mk 1) | media |
| 688 con CCS Mk 1 (abilitati Tomahawk) | **"Gyro Static Navigator"**, cioè giroscopio elettrostatico; l'**AN/WSN-3** è una versione ridotta dell'ESGN del Trident "per gli SSN 637/688 con Tomahawk"; negli anni '90–2000 sostituito dall'AN/WSN-7A a laser ring | [WPLA] [RNSINS] [MAeroWSN] | media |
| Motivo dell'ESG sui battelli Tomahawk | Il TLAM richiede una posizione di lancio precisa | deduzione da [RNSINS] | media |
| ESGN Trident | Intervallo di reset dichiarato **14 giorni** | [RNSINS] | media |
| Classe di prestazione dei SINS navali | SINS Mk 2 britannico: classe **≈ 1 nm/giorno** (giroscopi con deriva molto migliore di 0,001°/h); AN/WSN-5 USA: classe 1 nm/giorno; Mk 29 Mod 2/3 (export): classe **3 nm/giorno**; due sistemi mediati: errore × 0,707 | [RNSINS] | media |
| Deriva del DMINS / WSN-3 | **Non pubblicata** | — | — |
| STIMA DI GIOCO | **DMINS: 1,5 nm/giorno** (1σ, errore radiale crescente circa linearmente); **WSN-3 ESG: 0,5 nm/giorno**; più un'oscillazione di Schuler (periodo 84,4 min) di ampiezza 0,1–0,2 nm | — | bassa |
| Alta latitudine | Oltre ~80°N l'allineamento azimutale dei SINS a piattaforma degrada (letteratura generale, *n.v.*); il SINS Mk 2 britannico ebbe "piena capacità polare" solo dal 1976 [RNSINS]; il Sargo nel 1960, a ~65°N, vide l'inerziale "andare fuori linea" e corresse a mano stimando la deriva [NSL84] | [RNSINS] [NSL84] | bassa–media (qualitativo) |

**Nota fisica utile per il gioco.** L'inerziale misura il moto **rispetto alla Terra**: le correnti **non** sporcano la stima (a differenza della navigazione stimata con solcometro e girobussola). In caso di guasto dell'inerziale, la stima solo con solcometro EM e girobussola accumula l'errore di corrente: con 0,5–1 kn di corrente non nota fanno **12–24 nm/giorno** (calcolo; **media**). È uno scenario di guasto interessante.

### 2.2 Transit (NAVSAT) a quota periscopica

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Scopo | Nato per gli SSBN: aggiornare l'inerziale | [APL-Danchik] [WPTransit] | alta |
| Precisione | Requisito iniziale 0,1 nm per fix; specifica 0,042 nm (3σ) per asse per sito fisso; passaggio singolo tipico ≈ **200 m**; modalità "submarine" migliorata < 20 m | [APL-Danchik] [WPTransit] | alta |
| Errore da velocità propria | ≈ **0,2 nm per nodo** di errore di velocità (≈ 400 m/kn) | [GSTransit] [TRB73] | alta |
| Esposizione | Il requisito della Marina si basava su un **segmento Doppler di 2 minuti**, perché il sottomarino esponeva l'antenna per circa 2 min; un passaggio completo dura 12–15 min | [WPTransit] | alta |
| Attesa fra passaggi | Requisito: attesa media ≤ **4 h**, < 5% del tempo oltre 8 h, mai oltre 24 h; in pratica alcune ore all'equatore, **1–2 h alle medie latitudini**; con orbite polari i passaggi si **infittiscono verso i poli** | [APL-Danchik] [WPTransit] | alta |
| Costellazione | 1988: **12 satelliti, 7–8 operativi** (gli altri in "magazzino" orbitale) | [APL-Danchik] | alta |
| Fine servizio | 31 dic. 1996 | [APL-Danchik] | alta |

STIMA DI GIOCO: attesa media fra passaggi utili ≈ **100 min a 40°N, 60 min a 60°N, 40 min a 70°N**; 25% dei passaggi scartati (geometria troppo alta o troppo bassa); fix = 2 min di antenna a quota periscopica a velocità ≤ 5–8 kn; errore 0,05–0,1 nm (1σ) se l'inerziale fornisce bene la velocità.

### 2.3 GPS nel 1989

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Primo Block II | **14 feb. 1989** (USA-35); altri Block II lanciati nel corso dell'anno | [USA35] | alta |
| Copertura | Nel 1990–91: 16–19 satelliti → **19–20 h/giorno di copertura 3D**; servono 24 satelliti per il 100% | [AFGPS] | alta |
| Fine 1989 | Costellazione mista Block I/II, copertura **parziale a finestre** | ricostruzione | media |
| Ricevitori sui sottomarini | AN/WRN-6, progettato e installato "a partire dagli anni '80" su unità e sottomarini; data di introduzione sui 688 **non trovata** | [WRN6] | bassa |

**Raccomandazione:** nel 1989 GPS **assente** sui 688 come default; opzione di scenario "1990+" con finestre di disponibilità (STIMA DI GIOCO per fine 1989: 3D disponibile 6–10 h/giorno in finestre irregolari; precisione P-code ≈ 20 m).

### 2.4 Omega (VLF) e Loran-C

| Sistema | Precisione | Ricezione | Copertura teatri | Fonte | Conf. |
|---|---|---|---|---|---|
| **Omega** (10–14 kHz; 8 stazioni, in Europa **Bratland**, Norvegia) | Obiettivo 4 nm; tipico 2–4 nm (≈ 1 nm nei casi migliori); errori più forti con disturbi ionosferici; ambiguità di corsia | VLF: ricevibile con antenna a filo galleggiante o loop a bassa immersione, **senza emergere** | globale | [WPOmega] [NYTID] | alta (precisione), bassa (profondità di ricezione: STIMA 10–20 m d'antenna) |
| **Loran-C** (100 kHz) | Assoluta **0,1–0,25 nm**; ripetibile 20–90 m | serve l'antenna d'albero → **quota periscopica** | Mar di Norvegia (catene con Bø, Ejde, Sylt, Jan Mayen), Mediterraneo (GRI 7990, Gibilterra marginale, Adriatico sì), Pacifico NW (Hokkaido); Barents solo occidentale; Baltico parziale | [WPLoran] | alta (precisione), media (copertura 1989) |

### 2.5 Navigazione astronomica al periscopio

- Esistono teste di periscopio con **sestante incorporato** (brevetto) e testimonianze di fix stellari tentati su un 688 [USNI21] (**media**).
- STIMA DI GIOCO: precisione **1–3 nm** con orizzonte artificiale; serve cielo sereno e crepuscolo o notte; 5–10 min di periscopio alzato. Nel Barents d'inverno, notte polare e nuvole la rendono rara.

### 2.6 Navigazione batimetrica (per contorni del fondo)

Fonte primaria: **Bowditch, cap. 26** [BOW26], basato su Cohen (1970, *Bathymetric Navigation and Charting*, USNI) e su materiale JHU-APL; più USNI *Proceedings* 1964 [USNI64]. Testimonianza d'uso: il Sargo nel 1960 seguiva le isobate da 25, 50 e 100 fm e un monte sottomarino ("Tall Gonzales") nello Stretto di Bering [NSL84].

| Tecnica | Come funziona | Precisione | Requisiti | Conf. |
|---|---|---|---|---|
| **Riconoscimento di un rilievo** | Passare sopra un monte sottomarino, un canyon o una scarpata cartografati | dipende dalla carta | rilievo isolato e ben rilevato; più rilievi allineati è meglio; ultima posizione già affidabile | alta |
| **Avanzamento del contorno** | Si traccia l'isobata incontrata su un lucido e la si "avanza" del cammino percorso fino all'isobata successiva; l'intersezione dà il punto | **±100 yd circa, o ¼ della spaziatura delle isobate** | pendenze **1°–4°** (max 5°) e variabili; carta alla scala più grande disponibile | alta |
| **Linea di sondaggi** | Rotta e velocità costanti; si fa scorrere il profilo misurato sulla carta finché combacia | buona dove le isobate sono variate | non funziona su fondali piatti | alta |
| **Eco laterale su un monte** | Due passaggi perpendicolari, avvicinamento da almeno **20–30 nm**, sondaggi ogni minuto; il punto di minima profondità indica il quadrante della vetta | discreta; attenzione: la profondità minima in carta può essere sbagliata | — | alta |
| **Automatica** | Programmi che confrontano i sondaggi con la batimetria digitale | — | non tipica nel 1989 a bordo dei 688 (non verificato) | bassa |

**Limiti**: la tecnica **rifinisce** una posizione già nota, non la crea; se la carta è sbagliata il fix è sbagliato (e il navigatore può non accorgersene); l'impronta del fascio arrotonda i rilievi [BOW26]. Richiede di "pingare" a intervalli regolari: c'è un **compromesso diretto fra navigazione e discrezione**.

### 2.7 Come l'incertezza di posizione condizionava tattica e sicurezza

- **Gestione dello spazio acqueo**: per evitare il fuoco amico e le collisioni fra sottomarini amici, ai battelli si assegnano aree esclusive con **zone cuscinetto** dimensionate sull'incertezza di navigazione (**media**, dottrina generale; i valori non sono pubblici). STIMA DI GIOCO: cuscinetto = 2 × σ di posizione + 2 nm.
- **Stretti e acque basse** (Gibilterra, Kunashir, stretti danesi, costa di Kola): un errore di 1–2 nm è la differenza fra il canale e la secca. Sono i luoghi dove l'equipaggio deve **pagare** in esposizione (quota periscopica per un fix) o in emissioni (sondaggi, BQS-15).
- **Armi**: Tomahawk e Harpoon sono tirati da coordinate assolute (TLAM soprattutto); i siluri lavorano in coordinate relative e non ne risentono.
- **Pedinamento** (trailing): la posizione relativa conta più di quella assoluta; il rischio vero è il fondale non visto mentre si segue un bersaglio (collisioni di Gato/K-19 1969, Baton Rouge/K-276 1992, doc 03).
- **Ritmo tipico (ricostruito, media)**: il battello sale a quota periscopica per comunicazioni e fix ogni 8–24 h (doc 02 per i vincoli ELF/VLF); in transito "sprint and drift" i fix si combinano con le salite per le comunicazioni.

---

## 3. Precisione delle carte (fine anni '80) — "la carta non è il territorio"

### 3.1 Fatti generali

| Fatto | Fonte | Conf. |
|---|---|---|
| A circa il 2010 i sondaggi navali coprivano solo **~10% dei fondali** alla risoluzione di 1′ necessaria per vedere monti alti 1 km; **~90% dei monti > 1 km** non era osservato da sondaggi né da gravimetria satellitare | [SANDWELL10] | alta |
| La carta usata dal San Francisco nel 2005 era una carta **DMA del 1989 mai aggiornata**; nessun ostacolo segnato entro 3 nm dal punto d'urto; il monte saliva ~2000 m dal fondo; un'immagine Landsat del 1999 mostrava "acqua scolorita". Altre carte disponibili a bordo indicavano il pericolo | [SFCASE] [SFWP] | alta |
| L'area era "non più prioritaria" dopo la Guerra fredda; la DMA non aveva le risorse per aggiornare sistematicamente le carte coi dati satellitari | [SFCASE] | alta |
| Le priorità di cartografia della Guerra fredda erano il **Pacifico settentrionale e i mari artici** (aree degli SSBN e della sorveglianza) | dichiarazioni USN citate in [SFCASE]/stampa | media |
| Artico: la **Dorsale di Lomonosov** era posizionata bene sulle prime carte sovietiche e "grossolanamente sbagliata" su quelle USA/canadesi successive, che si affidavano troppo ai pochi dati di sottomarini declassificati; avvertimento esplicito sulla GEBCO dove i sondaggi pubblici sono scarsi | [WEBER83] | alta |
| I sondaggi dei sottomarini USA e britannici **1958–1988** erano classificati (declassificati per IBCAO dopo il 1999); le aree russe in IBCAO 1.0 sono state digitalizzate da una carta russa del 1999 **derivata da rilievi classificati** | [IBCAO] | alta |
| Sulle carte da rilievi vecchi (scandaglio a filo, singlebeam) il cartografo **interpola** le isobate fra i sondaggi; le carte da multibeam le conoscono "esattamente" | [BOW26 §2607] | alta |

### 3.2 Incidenti utili (rocce, monti, banchi) — casi di taratura

| Data | Battello | Dove | Dinamica | Lezione | Fonte | Conf. |
|---|---|---|---|---|---|---|
| 27 ott. 1981 | **S-363** (Whiskey sovietico) | Gåsefjärden, a ~10 km dalla base di Karlskrona | in superficie; i sovietici dichiarano "guasti simultanei agli strumenti di navigazione"; la Svezia conclude per lo spionaggio | battello convenzionale senza inerziale: errore di stima nel Baltico (o copertura) | [S363] | alta (fatto), il movente resta controverso |
| 13 mar. 1986 | **USS Nathanael Greene** (SSBN-636) | Mare d'Irlanda | urta il fondo, gravi danni; radiato dic. 1986 | anche gli SSBN, con il miglior inerziale, toccano il fondo in acque basse | forum [NG86] + [WPNG] (radiazione) | bassa (dinamica), alta (radiazione) |
| 29 apr. 1986 | **USS Atlanta** (SSN-712) | **Stretto di Gibilterra** | incaglio, sonar e cassa zavorra prodiera danneggiati; rientro a Gibilterra con mezzi propri | **teatro del gioco**: correnti e acque strette | [ATL86] | alta (fatto), non verificato se in immersione |
| nov. 2002 | HMS Trafalgar | Skye | > 14 kn, ~50 m; carta coperta da un lucido durante l'addestramento | errore procedurale con la carta | [WP2000] | alta |
| 8 gen. 2005 | **USS San Francisco** (SSN-711) | 360 nm a SE di Guam | **~33 kn, 160 m**; monte non in carta (carta DMA del 1989); 1 morto, 98–115 feriti; casse zavorra prodiere sfondate, scafo resistente integro; 30 h di rientro | caso perfetto di "carta ≠ territorio" + sondaggi rari + velocità eccessiva | [SFWP] [SFCASE] | alta |
| 26 mag. 2008 | HMS Superb | Mar Rosso | pinnacolo roccioso | — | [WP2000] | alta |
| 2 ott. 2021 | USS Connecticut | Mar Cinese Meridionale | monte sottomarino | succede ancora con il multibeam | [WP2000] | alta |

### 3.3 Qualità della carta per teatro (1989) — STIME DI GIOCO motivate

| Teatro | Chi l'ha rilevato | Qualità stimata (lato NATO) | Spaziatura delle linee di rilievo equivalente | Offset orizzontale tipico | Conf. |
|---|---|---|---|---|---|
| Faroe – Mar di Norvegia / GIUK | Idrografi britannici, norvegesi, islandesi; rilievi legati all'installazione SOSUS | buona sulle soglie, discreta nei bacini profondi | 1–2 km (soglie), 5–10 km (bacini) | 0,1–0,3 nm | bassa |
| Barents / approcci di Kola | Sovietico (GUNiO), segreto; la NATO usa carte sovietiche pubblicate, rilievi vecchi e sondaggi propri dei sottomarini (classificati) | **media-scarsa** per il lato NATO, **ottima** per il lato sovietico | 2–5 km | 0,3–1 nm | bassa |
| Gibilterra | Spagna, Regno Unito, Marocco; traffico intensissimo | **buona** | 0,5–1 km | < 0,1 nm | bassa |
| Canale d'Otranto / Adriatico | Italia (Istituto Idrografico), Jugoslavia, Albania (scarso) | buona sul lato italiano, mediocre su quello albanese | 0,5–2 km | 0,1–0,3 nm | bassa |
| Kunashir / Okhotsk | Sovietico (segreto), Giappone a S | **scarsa** in Okhotsk, buona attorno all'Hokkaido | 3–10 km | 0,5–1 nm | bassa |
| Margine della piattaforma celtica | Regno Unito, Francia, Irlanda | piattaforma buona; **canyon della scarpata mal risolti** | 1 km (piattaforma), 3–5 km (scarpata) | 0,1–0,3 nm | bassa |
| Baltico (stretti danesi) | Danimarca, Svezia, RFG eccellenti; DDR, Polonia e URSS per la NATO meno noti | ottima a W, media a E; relitti e massi erratici | 0,2–1 km | < 0,1 nm (W) / 0,3 nm (E) | bassa |

Per l'**IA sovietica** la tabella si inverte nelle acque di casa (Barents, Okhotsk, Baltico orientale: carte ottime) e peggiora nelle acque NATO (GIUK, Celtico), dove però l'URSS aveva rilevato molto con le sue navi idrografiche (**bassa**).

---

## 4. Tattiche vicino al fondo e sfruttamento del terreno

| Tema | Fatti / ragionamento | Fonte | Conf. |
|---|---|---|---|
| **Posarsi sul fondo** (bottoming) | Tattica dei **convenzionali** (Kilo, Foxtrot, Whiskey, battelli svedesi), soprattutto in acque basse (Baltico, fiordi): motori fermi, solo rumore di bordo. Gli **SSN lo evitano**: le prese a mare del condensatore principale stanno sotto lo scafo e aspirerebbero fango e detriti; rischio anche per prese, rivestimenti e timoni | [WATM] + forum | media (convergenza di fonti divulgative; nessuna fonte ufficiale) |
| **Stare vicino al fondo** (SSN) | Contro un sonar attivo il battello si confonde con la **riverberazione di fondo**: in acqua bassa il fondo è spesso la fonte dominante di riverberazione; la riverberazione fluttua e crea finestre di mascheramento | [IOA-rev] [AA24] | alta (fisica), media (uso tattico) |
| **Ombra del rilievo** | Un monte o una dorsale fra il battello e un sensore interrompe il cammino diretto e quello per rimbalzo sul fondo; le basse frequenze diffrangono di più, le alte poco | fisica di base (doc 02) | media; come "tattica documentata" bassa (nessuna fonte aperta trovata) |
| **Canyon e fosse** | Profondità maggiore = acqua più fredda/stratificata, cammini diversi; utile per sottrarsi ai sonar a scafo di superficie in acqua bassa (es. fosse del Barents, canyon celtici) | ragionamento | bassa |
| **Rischi** | Margine chiglia–fondo piccolo + errore di carta + errore di posizione + controllo della quota a velocità; un 688 alto ~10 m di scafo + vela ha bisogno di margine. Incidenti del §3.2 | [SFCASE] | alta (il rischio) |
| **Dottrina del margine sotto chiglia** | Non pubblicata. Testimonianza Bering 1960: Sargo a 16–24 ft dal fondo e ~6 ft dalle chiglie di ghiaccio, a 7 kn (eccezionale) | [NSL84] | media |

**Acustica del fondo (solo richiamo, vedi doc 02).** Fondo duro (roccia, ghiaia) = forte riflessione e forte riverberazione, buon rimbalzo, pericoloso per la contro-scoperta attiva. Fondo molle (fango, argilla) = alta perdita al fondo e poco rimbalzo, ma ecoscandaglio meno affidabile (penetrazione) [BOW26].

**Fondali e sorveglianza svedese.** Negli anni '80 la Svezia riferì tracce sul fondo attribuite a mini-sottomarini nel Baltico; la vicenda è storiograficamente controversa (**bassa**): utile al più come colore per uno scenario baltico, non come dato.

---

## 5. Operazioni sotto il ghiaccio (Barents d'inverno)

**Dove c'è ghiaccio nel teatro.** La costa di Kola è **libera dai ghiacci tutto l'anno**; il bordo invernale del ghiaccio sta a circa **74–77°N** (doc 03 §A2). Il ghiaccio entra quindi solo nella variante settentrionale del teatro (72–76°N), nel Mar Bianco e nel Barents sud-orientale (Pečora). Il **bordo del ghiaccio (MIZ)** è una zona chiave: rumorosa, con forti gradienti, usata come rifugio.

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Chiglie di ghiaccio di primo anno, Barents SW (ULS 2014–2020) | media **3,8 m**; 99° percentile **9,4 m**; massimo **≈ 20 m**; massimi in apr.–mag.; rapporto chiglia/spessore del ghiaccio livellato ≈ 12 | [ERVIK23] | alta (dato moderno, ordine di grandezza valido per il 1989) |
| Banchisa permanente artica | spessore 0–8 m (media ~3 m); creste fino a **50 m**; chiglie di iceberg fino a 500 m (Groenlandia); quota di chiglia di **90 m** libera la cresta più profonda, se il fondale lo consente | [CNR08] (Sharpe 1988) | media |
| Iceberg nel Barents (Svalbard, Terra di Francesco Giuseppe) | STIMA DI GIOCO: pescaggio tipico 30–100 m, raro fino a ~150 m | — | bassa |
| Acqua libera in Artico | ~10% fra canali e polynye, in continuo movimento; trovare una polynya o ghiaccio sottile può richiedere **> 12 h** | [CNR08] | media |
| Sonar verso l'alto (topsounder) | Un ecoscandaglio verso l'alto **non distingue bene 3 ft di ghiaccio liscio dall'acqua libera** (esperienza del 1962) | [CNR08] | media |
| Attivo sotto la banchisa | "Il sonar attivo ad alta potenza è praticamente inutile per la riverberazione della volta di ghiaccio"; il passivo è buono lontano dai margini; al bordo il ghiaccio in movimento genera rumore a banda larga | [CNR08] | media |
| Emersione | Rilievo sonar dello spessore; stima della deriva del ghiaccio (di solito < 1 kn) e accordo con essa; salita lenta a velocità ~0 finché la vela tocca il ghiaccio; soffiata parziale delle casse per **qualche tonnellata** di spinta positiva; con la vela si sfonda | [CNR08] [TAGC] | media |
| Spessori sfondati (anni '60, Sargo) | 0,6 m, 0,9 m, 0,33 m; circa 13 t di zavorra extra per liberarsi dal ghiaccio al Polo; timoni bloccati dal gelo | [NSL84] | media |
| Navigazione sotto il ghiaccio | L'inerziale va aggiornato salendo, a volte sfondando il ghiaccio; strato di salinità a ~50 m che separa acusticamente due zone | [CNR08] | media |

**Rumore sotto il ghiaccio**: molto basso sotto la banchisa compatta e stabile; impulsivo e forte al margine e con ghiaccio in pressione (doc 02 §4.6). Va **legato al tipo di ghiaccio della cella**, non a un valore unico.

### 5.1 L'acustica sotto la volta (letture del 29/10/2026, pagine aperte)

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Il profilo artico | Quasi isotermo, la velocità cresce con la profondità e il minimo sta in superficie: il suono rimbalza sulla volta e torna giù (semicanale) | [DOSITS-ICE] | alta |
| La volta come superficie ruvida | Diffusione in avanti che dipende dalla rugosità totale e attenua forte le alte frequenze; rugosità efficace rms di 2-3 m, coerente coi profili misurati sotto il ghiaccio; riverberazione della volta fino a 40 dB sopra quella del mare libero | Mellen & Marsh 1965 (NUSL, DTIC AD0718140, sommario) [MM65] | media |
| Il filtro passa-basso | Col pack pluriennale e le chiglie di decine di metri, la propagazione lunga solo sotto ~30-40 Hz (Guerra fredda); col ghiaccio più sottile di oggi va più lontano anche più in alto | [DOSITS-ICE]; recensione citata in [MM65] | media |
| Il modello delle creste | Le creste come semicilindri ellittici disposti a caso: la perdita di riflessione dipende da pescaggio e larghezza delle chiglie, dalla densità delle creste e dall'angolo radente; validità 0-5 kHz, 0-45 gradi | Diachok 1976, JASA 59 pp. 1110-1120 (abstract e rapporti che lo citano) [DIA76] | media |
| Il modello empirico di Buck | Perdita di propagazione 10-500 Hz nel bacino centrale, da una carta della rugosità della volta (deviazione standard), cresce con la densità delle creste | Buck 1985, AEAS Report 2 (abstract) [BUC85] | media |
| Il rumore sotto il pack | Circa 70 dB re 1 µPa²/Hz a 50 Hz coi mesi di ghiaccio, ~65 col ghiaccio e poco vento, contro 80-83 a 20-50 Hz in acqua libera; sopra il 75% di copertura cresce col vento di ~0,5 dB per m/s (1 dB per m/s in acqua libera) | Roth et al. 2012, JASA (sommario) [ROT12] | media |
| Le sorgenti del ghiaccio | Creste in pressione e fratture termiche: 110-180 dB re 1 µPa²/Hz a 1 m; sopra 40 Hz dominano le fratture entro 6-30 km | studio del rumore del pack (arXiv 1702.01356, sommario) [ARX17] | media |

**STIMA DI GIOCO (proposta, da approvare):** nel campo dei raggi la riflessione in superficie sotto la volta usa la
rugosità del ghiaccio al posto delle onde: la coerenza di Eckart con σ = rms della volta (2,5 m nel pluriennale,
1 m nel primo anno, pesata con la copertura), più 0,5 dB per rimbalzo nel ghiaccio. A 30 Hz la perdita resta
piccola; a 1 kHz la volta è quasi un assorbitore, e così torna il filtro passa-basso delle misure. Il rumore resta
quello di oggi (il pack quieto 5 dB sotto il mare 0, il margine +10/+20), con la dipendenza dal vento dimezzata
sotto il 75% di copertura. Non ho le curve di Diachok né le tabelle di Buck: i testi DTIC non si aprono da qui.

[DOSITS-ICE] dosits.org, «How does sea ice affect how sound travels?». [MM65] K. Mellen, H. W. Marsh, *Underwater Sound in
the Arctic Ocean*, NUSL 1965 (DTIC AD0718140). [DIA76] O. Diachok, «Effects of sea-ice ridges on sound propagation in the
Arctic Ocean», JASA 59 (1976) 1110-1120. [BUC85] B. M. Buck, AEAS Report 2, Polar Research Laboratory, 1985 (DTIC
ADA156576). [ROT12] E. Roth et al., JASA 2012 (Beaufort). [ARX17] arXiv:1702.01356.

---

## 6. Lato sovietico

| Classe | Navigazione | Ostacoli / mine / ghiaccio | Altro | Fonte | Conf. |
|---|---|---|---|---|---|
| **Alfa** (Pr.705/705K) | sistema di navigazione **"Sož"** + controllo di rotta "Boksit"; automazione BIUS "Akkord" | sonar di rilevamento mine **MG-24 "Luč"** | complesso sonar "Okean" con la stazione passiva di scoperta **«Enisej»** (МГК-1001); intercettatore del tipo «Rubin» (STIMA, dossier 28 §2) | [WPAlfa] [RUWIKI705], dossier 19 | media |
| **Victor I/II** (Pr.671/671RT) | **"Sigma"** | sonar attivo di evitamento ghiaccio **MT-70** (per la classe) | Rubin / Skat | [NE-Victor] [WPVictor] | media |
| **Victor III** (Pr.671RTM) | **"Medvedica-671RTM"** (sostituisce Sigma): posizione, rotta, velocità rispetto all'acqua e al fondo, rollio e beccheggio in automatico, continuo | MT-70 (come sopra) | Skat-KS; BIUS "Omnibus" | [NE-Victor] [RUWIKI671RTM] [WPVictor] | media |
| **Akula** (Pr.971) | **"Medvedica-945"** [WPAkula]; la wiki russa indica **"Simfonija-U"** (probabilmente unità successive o ammodernate) | sonar di rilevamento mine **MG-70** (probabilmente lo stesso sistema dell'"MT-70" dei Victor, traslitterato in modo diverso) | MGK-540 Skat-3; rivelatore di scia MNK-200-1 "Tukan" da K-322 | [WPAkula] [RUWIKI971] | media |
| Tutti | Ecoscandagli di navigazione: **designazioni e dati non trovati** | — | — | — | — |
| Radionavigazione sovietica | **RSDN-20 "Alfa"** (VLF 11,9–14,9 kHz, 3 stazioni, portata fino a 10.000 km, errore 2,5–7 km, usato anche da sottomarini immersi); **Čajka** (equivalente del Loran-C); satelliti **Parus/Ciklon-B** (dal 1974) e **Cikada** (civile); GLONASS in costruzione dal 1982, non operativo nel 1989 | [WPAlpha] [WPLoran] [WPParus] | media |

**Prestazioni.** Nessuna precisione pubblicata per Sož o Medvedica. STIMA DI GIOCO: Sož/Sigma **3 nm/giorno**; Medvedica **1,5–2 nm/giorno** (un po' peggio del WSN-3, coerente con la generazione e con l'opinione occidentale sull'elettronica sovietica); fix satellitari Parus con caratteristiche simili al Transit; RSDN-20 come l'Omega ma più grossolano (2–4 km). Il vantaggio sovietico vero nei bastioni è la **carta** (rilievi propri, densi) e la **vicinanza a riferimenti costieri** (radar, Čajka), non lo strumento.

**Ghiaccio.** La VMF operava regolarmente sotto la banchisa (SSBN Delta e Typhoon con emersione nel ghiaccio; SSN di scorta) (**media**, letteratura generale *n.v.*). Per il gioco: Victor III e Akula "under-ice capable" con MT/MG-70; emersione attraverso il ghiaccio solo per gli SSBN progettati per farlo (**bassa**).

---

## 7. Modello consigliato per il gioco

### 7.1 Sensori (tabella di parametri, tutti tarabili)

| Sensore | Geometria | Portata | Aggiornamento | Uscita | Contro-scoperta (portata d'intercettazione) |
|---|---|---|---|---|---|
| **Ecoscandaglio** (688 e sovietici) | cono verticale di 30° verso il basso | fino a 6000 m sotto chiglia | **su ordine** (ping singolo), oppure periodico ogni N min (N scelto dal giocatore: 1–30) | profondità minima nell'impronta ± 1% + rumore; 2% di falsi fondi (DSL, bolle, scia) | piena potenza: 15–20 km; bassa potenza: 5–8 km (−20 dB, portata ≤ 2000 m sotto chiglia) |
| **BQS-15 canale prodiero** | ±60° az, −30°/+45° el, scansione a settori di 15°, una spazzata completa ogni ~8–10 s | mine 1 km; ghiaccio 2,5 km; rilievi 3 km (degradati da velocità e riverberazione) | continuo quando acceso | ritorni in distanza, azimut ed elevazione; risoluzione ~1° × ~5 m | nel fascio 8–10 km; fuori fascio 2–4 km |
| **BQS-15 sondatori su/giù** | fascio di 10–15° | 1000 m | 1 Hz | distanza dalla volta (ghiaccio o superficie), distanza dal fondo | 2–4 km |
| **688I** | BQS-15 migliorato: +20% di portata sul ghiaccio | — | — | stima dello spessore del ghiaccio ± 50% (ambiguità tra ghiaccio sottile e acqua libera) | come sopra |
| **BQQ-5 attivo** (vedi doc 02) | sfera, settore di prua | rilievi a decine di km | ping singolo | eco del rilievo come "contatto di fondo", senza profilo | doc 02 (decine di nm) |
| **Periscopio** | visivo | costa, ghiaccio, acqua scolorita (bassa profondità diurna) | — | rilevamento di punti cospicui | doc ESM/radar |
| **Radar di navigazione** (BPS-15) | in emersione o a quota periscopica | costa a 10–20 nm | — | fix radar (0,05–0,2 nm) | ESM nemico (doc 01/05) |

Livelli **EMCON** del giocatore e dell'IA: SILENZIO / RIDOTTO / NAVIGAZIONE / SICUREZZA (§1.5). L'IA nemica che intercetta un ping riceve un **contatto con rilevamento** (e una distanza grezza se ha più ascoltatori).

### 7.2 Quadro 3D progressivo del fondale (solo da ciò che è stato misurato)

Struttura dati (per teatro): **griglia di altezze** (es. 100–250 m) con, per cella:
`z_carta, σ_carta` (dalla carta degradata, §7.4) · `z_mis, σ_mis, t_mis, fonte` (fusione delle misure) · `flag_ostacolo` (pinnacolo, relitto, chiglia di ghiaccio) · `tipo_fondo` (doc 03 B3).

Aggiornamento:
1. **Ecoscandaglio**: proietta il cono sulla griglia **nella posizione stimata**. Sulle celle dell'impronta vale "fondo ≥ profondità misurata" (vincolo di minimo); la cella al centro riceve la misura con σ legata all'impronta. Il risultato è un **nastro** di celle note lungo la rotta.
2. **BQS-15**: ogni ritorno è un punto (r, az, el) → posizione → aggiorna celle e ostacoli; l'assenza di eco nel settore entro la portata utile **libera** le celle ("niente fino a X m") con una confidenza minore.
3. **Fusione**: filtro di Kalman scalare per cella (`z = media pesata`, σ decrescente); decadimento verso la carta se l'informazione è vecchia e la posizione incerta.
4. **Incertezza di posizione**: ogni misura si archivia con la **posa stimata** del momento e la sua σ. Quando arriva un fix, la correzione si distribuisce linearmente sulle misure prese dall'ultimo fix (come una SLAM semplificata): il "mosaico" **scorre** sulla carta. È visivamente efficace e didattico.
5. **Display**: (a) vista carta con isobate e annotazioni (ED/PA, "acqua scolorita", data del rilievo); (b) vista "misurato" con colore = profondità e trasparenza = confidenza; celle mai misurate grigie o assenti; (c) sezione verticale di prua: profilo previsto dalla carta + ritorni BQS-15 + quota propria + margine di sicurezza; (d) display del ghiaccio: profilo della volta dai sondatori verso l'alto, polynye candidate.
6. **Niente vista 3D "magica"**: la vista 3D esiste, ma rende **solo** la griglia fusa, con le parti non misurate chiaramente segnalate come "da carta".

### 7.3 Modello d'incertezza di posizione

Stato: posizione stimata `p̂`, covarianza `P` (ellisse 2D). Posizione vera `p` = `p̂` + errore simulato (mai mostrato al giocatore).

- **Crescita** (per tick dt): `P += Q·dt`, con errore dell'inerziale modellato come **random walk di velocità** più bias lento: `σ_r(t) ≈ k·t` (k = 1,5 nm/giorno DMINS; 0,5 WSN-3; Medvedica 2; Sož/Sigma 3) + oscillazione di Schuler (84 min, ampiezza 0,15 nm). Oltre 80°N: k × 2. Inerziale guasto → stima solo con solcometro e girobussola: errore di corrente non nota 0,2–1 kn per teatro (Gibilterra e Kunashir alti).
- **Fix** (aggiornamento di Kalman con R):

| Fix | σ (1σ) | Costo / esposizione |
|---|---|---|
| Transit | 0,05–0,1 nm (+0,2 nm per kn di errore di velocità) | quota periscopica, antenna 2 min, attesa del passaggio (tempo medio per latitudine §2.2), velocità ≤ 8 kn |
| GPS (opzione 1990+) | 0,01 nm | quota periscopica, < 1 min, finestre di disponibilità |
| Loran-C | 0,1–0,25 nm (solo nella copertura) | quota periscopica con albero, 2–5 min |
| Omega | 2–4 nm (peggio di notte o con disturbi) | antenna a bassa immersione (filo galleggiante), senza emergere; ambiguità di corsia se σ > 4 nm |
| Astronomico | 1–3 nm | periscopio 5–10 min, cielo sereno, crepuscolo o notte |
| Radar su costa | 0,05–0,2 nm | emissione radar (ESM), quota periscopica, entro 20 nm dalla costa |
| Visivo su punti cospicui | 0,05–0,1 nm | periscopio, costa visibile |
| Batimetrico (contorni) | max(¼ della spaziatura delle isobate, 100 yd) **+ errore della carta in quel punto** | serie di sondaggi (emissioni) su rotta costante, pendenze 1°–4° |

- Il fix batimetrico usa la **carta degradata**: se la carta è spostata, il fix eredita l'errore. È così che si ottiene, in modo emergente, un equipaggio "sicuro di sé e sbagliato".

### 7.4 Carta contro realtà (carta degradata derivata dalla batimetria vera)

Batimetria vera = GEBCO/EMODnet/IBCAO (doc 03 B1), eventualmente arricchita con rilievi procedurali sub-griglia (pinnacoli, monti piccoli) dove i dati sono grossolani. La **carta** per ciascuna parte (NATO e sovietica: due carte diverse) si genera così:
1. **Rilievo sintetico**: linee di rilievo virtuali con la spaziatura del teatro (§3.3), campionate con un ecoscandaglio a cono (minimo nell'impronta), più qualche sondaggio sparso.
2. **Errore di posizione del rilievo**: offset orizzontale correlato per "campagna" (blocchi di 20–50 nm) dall'intervallo del §3.3.
3. **Errore di profondità**: bias della velocità del suono ±1–3% per campagna + rumore 0,5%.
4. **Ricostruzione**: interpolazione (TIN/spline) fra le linee → i rilievi più stretti della spaziatura **spariscono** o si spostano; le pendenze si ammorbidiscono.
5. **Annotazioni**: con probabilità p, i rilievi mancanti compaiono solo come nota ("acqua scolorita", "ED", "PA", "profondità minore riportata 19xx") su **un'altra** carta della stessa zona (lezione del San Francisco); campo "anno del rilievo".
6. **Generalizzazione**: isobate a intervalli di scala (es. 100 m in altura, 10 m in piattaforma).

### 7.5 Incaglio e collisione

- **Contatto** = il volume del battello (scafo cilindrico ~10 m di diametro + vela) interseca la batimetria **vera** o un ostacolo (chiglia di ghiaccio, relitto). Si controlla con la posizione **vera**, non con quella stimata.
- **Danno** ∝ ½ m·v_n² (componente normale della velocità) **(STIMA DI GIOCO)**:
  - < 3 kn: graffio; possibili danni al sonar di prua o al rivestimento.
  - 3–10 kn: danni al duomo sonar e allagamento di una cassa zavorra prodiera.
  - 10–20 kn: più casse zavorra e sfera sonar persi, feriti, assetto appruato.
  - > 20 kn: scenario San Francisco (33 kn): casse prodiere sfondate, sonar distrutto, numerosi feriti, possibili morti, emersione d'emergenza; scafo resistente **integro** a meno di un urto ad angolo retto su roccia oltre i 30 kn.
- **Strisciamento sul fondo** (componente tangenziale): timoni ed elica; per gli **SSN, sedimenti nelle prese del condensatore** → perdita di potenza progressiva.
- **Posarsi sul fondo**: consentito senza penalità solo ai **convenzionali**; per gli SSN, ogni minuto di contatto aumenta il rischio sulle prese.
- **Ghiaccio**: urto di vela e alberi contro le chiglie (danni ad alberi e antenne); emersione attraverso il ghiaccio consentita entro lo spessore limite della classe (§1.3), altrimenti vela danneggiata. Rischio di congelamento dei timoni dopo una lunga sosta in superficie.
- **Avvisi all'equipaggio** (realistici): "fondo in salita" solo se un sondaggio recente lo mostra; "ostacolo a prua" solo da un ritorno BQS-15; mai dal terreno vero.

### 7.6 Tattica e uso nell'IA

- **IA USA/NATO**: in zone con carta scarsa limita la velocità a `v ≤ portata BQS-15 / tempo di reazione`, accende il BQS-15 o fa sondaggi frequenti, accetta l'esposizione; nei bastioni sovietici (Barents, Okhotsk) preferisce sondaggi rari e a bassa potenza + fix batimetrici sui rilievi noti + salite a quota periscopica pianificate per il Transit.
- **IA sovietica**: carte buone in casa → naviga veloce e vicina al fondo nei propri bastioni; i convenzionali (Kilo, Foxtrot) si **posano sul fondo** in acque basse (Baltico, piattaforma) per sparire dal passivo.
- **Rifugi**: l'IA cerca la cella con alta riverberazione di fondo (fondo duro e rugoso) e un rilievo fra sé e la minaccia; il giocatore può fare lo stesso, a suo rischio.
- **Contro-scoperta come informazione**: un ping intercettato genera un contatto per chi ascolta; l'IA ne tiene conto prima di "sondare" (variabile di prudenza per livello di difficoltà).
- **Stretti**: Gibilterra e Kunashir con corrente e carta buona ma acque strette → il rischio dominante è **l'errore di posizione**, non la carta: ritmo di fix più alto.

### 7.7 Lacune principali nei dati

1. **BQS-15**: frequenza, livello di sorgente, settore, portata, modalità; nessuna fonte aperta primaria (solo "> 10 kHz, classificato").
2. **Ecoscandaglio dei 688** (designazione certa nel 1989, frequenza, potenza, modalità "sicure"): solo il riferimento MASINT al BQN-17 e l'analogia con l'UQN-4.
3. **Deriva di DMINS (WSN-1) e WSN-3** e data di installazione del WSN-3 per singolo battello.
4. **Navigazione sovietica**: precisione di Sož, Sigma e Medvedica; designazioni degli ecoscandagli; dati di MG-24 e MT/MG-70.
5. **Qualità delle carte** per teatro: nessuna statistica pubblicata; il §3.3 è interamente una STIMA DI GIOCO.
6. **Dottrina dei margini sotto chiglia e dei cuscinetti di spazio acqueo**: classificata o non pubblicata.
7. Possibili fonti da cercare offline: Friedman, *Naval Institute Guide to World Naval Weapons Systems* (ed. 1989/1991, voci BQS-15, BQN-17, WSN-1/3); Polmar & Moore, *Cold War Submarines* (2004); Cohen, *Bathymetric Navigation and Charting* (USNI 1970); Glennon & Nye, *Data Systems Technician 1 & C* (1983); McLaren, *Unknown Waters* (sulle operazioni sotto il ghiaccio); per l'URSS, i volumi di Pavlov e di Apalkov sulle classi di SSN e le pagine di deepstorm.ru.

---

## 8. Sintesi delle scelte che cambiano il progetto

1. **Il "contour imaging" va eliminato** come sensore e sostituito da un display composito "carta + misure" con la confidenza resa visibile.
2. **Ecoscandaglio e BQS-15 sono attivi e intercettabili** (stima: 5–20 km); l'ecoscandaglio si usa a ping singolo o periodico, a scelta del giocatore.
3. **Il BQS-15 vede 1–3 km**: in acque mal cartografate fissa la **velocità sicura**.
4. **Due carte (NATO e sovietica) degradate in modo diverso** dalla batimetria vera; i monti più stretti della spaziatura dei rilievi possono mancare (caso San Francisco, carta DMA 1989).
5. **Posizione propria incerta** (0,5–3 nm/giorno) con fix che costano esposizione (Transit 2 min a quota periscopica; Loran-C; Omega senza emergere ma grossolano; fix batimetrici che costano emissioni ed ereditano gli errori della carta).
6. **GPS assente nel 1989** come default.
7. **688 Flight I/II senza capacità di emergere nel ghiaccio**; il 688I sì (vela rinforzata, timoni a prua).
8. **Gli SSN non si posano sul fondo**; i convenzionali sì.

---

## 9. Fonti

- **[FASBQS15]** FAS, *AN/BQS-15*, https://man.fas.org/dod-101/sys/ship/weaps/an-bqs-15.htm (agg. 1998).
- **[NCBQS15]** Nuclear Companion, *AN/BQS-15 Close Contact Avoidance and Under-ice Navigation Sonar*, https://nuclearcompanion.com/data/an-bqs-15-close-contact-avoidance-and-under-ice-navigation-sonar/ (cita Glennon & Nye 1983; Polmar 2013; Jane's UWS 2000–01; audizioni DoD FY1974).
- **[GOAEIS]** US Navy, *Gulf of Alaska Navy Training Activities FEIS*, 2011, App. H "Acoustic Systems Descriptions" (contenuto riportato via motore di ricerca; PDF non scaricabile, 403).
- **[MPBQS]** Military Periscope, *AN/BQS-14/14A/15/MIDAS overview* (solo l'anteprima gratuita).
- **[CMANO]** CMANO-DB, sensor 1663 (database di gioco).
- **[ACBQS]** armedconflicts.com, voce BQS-15 (forum).
- **[GS688]** GlobalSecurity, *SSN-688 Los Angeles-class Design*.
- **[WPLA]** Wikipedia EN, *Los Angeles-class submarine*.
- **[PAT4086560]** US Patent 4,086,560 "Secret depth sounder", US Navy, depositato 1959, concesso 1978.
- **[FASUQN4]** FAS, *AN/UQN-4A Sonar Sounding Set*; **[DTIC-UQN4]** DTIC ADA071185 (ARL, elementi a 12 kHz dell'UQN-4).
- **[APL-Danchik]** R.J. Danchik, "An Overview of Transit Development", *JHU APL Technical Digest* 19(1), 1998.
- **[WPTransit]** Wikipedia EN, *Transit (satellite)*.
- **[GSTransit]** GlobalSecurity, *Transit*; **[TRB73]** "Recent (1973) improvements in the Navy Navigation Satellite System", TRID 18432.
- **[RNSINS]** rnsubs.co.uk, *NATO Ships Inertial Navigation System (SINS)* (da un articolo IMarEST).
- **[MAeroWSN]** Military & Aerospace Electronics, articoli sull'AN/WSN-7/7A (sostituisce il WSN-3 sui 688).
- **[USA35]** Wikipedia EN, *USA-35*; **[AFGPS]** US Air Force, *Evolution of GPS: from Desert Storm to today's users* (2016); **[WRN6]** Navy SBIR / InsideGNSS sull'AN/WRN-6.
- **[WPOmega]** Wikipedia EN, *Omega (navigation system)*; **[NYTID]** Ny Tid (articolo storico norvegese su Omega/Bratland); **[WPLoran]** Wikipedia EN, *LORAN-C*; **[WPAlpha]** Wikipedia EN, *Alpha (navigation)*; **[WPParus]** Wikipedia EN, *Parus (satellite)*.
- **[USNI21]** USNI *Proceedings*, ott./nov. 2021, "Navigate by the Stars—From Beneath the Waves" (solo il riassunto).
- **[BOW26]** *The American Practical Navigator* (Bowditch), ed. 2017, cap. 26 "Bathymetric Navigation" (riprende Cohen 1970 e Cutler 2003).
- **[USNI64]** "Bathymetric Navigation", USNI *Proceedings*, ott. 1964.
- **[NSL84]** Naval Submarine League, "Through Bering Strait in Mid-Winter" (USS Sargo 1960), 1984.
- **[CNR08]** *Canadian Naval Review*, "Submarine under-ice operations", 2008 (da R. Sharpe, *Maritime Affairs Bulletin* 4/88).
- **[TAGC]** The Aviation Geek Club, testimonianza di un sonarista degli Sturgeon sull'emersione al Polo (divulgativo).
- **[ERVIK23]** Ervik et al., POAC 2023, statistiche delle chiglie di creste di primo anno nel Barents.
- **[SFWP]** Wikipedia EN, *USS San Francisco (SSN-711)*; **[SFCASE]** D. Wright, D. DiBiase, F. Harvey, *Case study: Submarine Crashes into Uncharted Seamount*, GIS Professional Ethics Project, 2009; **[DAY05]** *The Day* (New London), maggio 2005, sul rapporto d'inchiesta (via motore di ricerca, *n.v.*).
- **[SANDWELL10]** Wessel, Sandwell et al., *Oceanography* 23(1), 2010.
- **[WEBER83]** J.R. Weber, "Maps of the Arctic Basin Sea Floor", *Arctic* 36(2), 1983.
- **[IBCAO]** Jakobsson et al., IBCAO v2.0, *GRL* 2008, e materiali GEBCO/AGU.
- **[ATL86]** Wikipedia EN, *USS Atlanta (SSN-712)*; **[NG86]** forum irishmilitaryonline (dinamica); **[WPNG]** Wikipedia EN, *USS Nathanael Greene*; **[S363]** Wikipedia EN, *Soviet submarine S-363*; **[WP2000]** Wikipedia EN, *List of submarine and submersible incidents since 2000*.
- **[WATM]** We Are The Mighty, sui sottomarini nucleari e il fondale (divulgativo).
- **[IOA-rev]** Searing, Williams, Wood, "Bottom reverberation measurements in deep and shallow water", Institute of Acoustics; **[AA24]** *Archives of Acoustics*, 2024, sulla riverberazione di fondo in acque profonde.
- **[WPAlfa]** Wikipedia EN, *Alfa-class submarine*; **[RUWIKI705]** Wikipedia RU, Pr.705(K); **[WPAkula]** Wikipedia EN, *Akula-class submarine*; **[RUWIKI971]** Wikipedia RU, *Подводные лодки проекта 971 «Щука-Б»*; **[WPVictor]** Wikipedia EN, *Victor-class submarine*; **[RUWIKI671RTM]** Wikipedia RU, Pr.671RTM(K); **[NE-Victor]** naval-encyclopedia.com, *Victor III class*.
