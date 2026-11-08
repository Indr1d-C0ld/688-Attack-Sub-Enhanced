# 04 — Schiere rimorchiate (towed arrays), 1985–1991

Progetto: remake realistico di *688 Attack Sub* (1989).
Scopo: dati, fisica e regole di gioco per le schiere acustiche rimorchiate di sottomarini e navi di superficie, USA, NATO e URSS.

Convenzioni come in `02_acustica_oceanografia_dottrina.md` §0:
- **[alta]** = fonte primaria o autorevole verificata.
- **[media]** = fonte aperta credibile, oppure calcolo dell'autore con fisica standard e dati pubblicati.
- **[bassa]** = forum, giochi, fonti divulgative o interpolazioni.
- **STIMA DI GIOCO** = nessun dato pubblicato; è un valore proposto e difendibile, da trattare come parametro di taratura.
- Le sigle tra parentesi quadre rimandano alla bibliografia (§8).

Dati già acquisiti e non ri‑ricercati, solo citati:
- **TB‑16**: schiera ~73 m (240 ft), Ø 3,5 in, cavo ~730–790 m, ~50 idrofoni.
- **TB‑23**: thin‑line di ~290 m.
- Ambiguità destra/sinistra risolta con una virata di 20–30°.
- Raddrizzamento della schiera in 2–5 min dopo una virata.
- La schiera elimina in pratica il problema dei *baffles*.

Fonti: `[FAS-TB16]`; `01_piattaforme_e_armi.md` §TB‑16/TB‑23; `02_acustica…` §5.2.

> **Correzione a `03_teatri_fondali_storia.md`** (riga sul K‑324). La fregata USS *McCloy* (FF‑1038) non trainava una TACTAS SQR‑19: aveva un sistema sperimentale **SQR‑15 / "TASS" / ITASS** su furgone nell'hangar DASH `[DMN]` `[USNI87]` (media). Va corretto anche il meccanismo dell'incidente: fu l'elica del sottomarino a impigliarsi nel cavo della fregata (§2.2).

---

## 1. Schiere USA per sottomarini (situazione 1989)

### 1.1 Famiglia e cronologia

| Sistema | Tipo | Dimensioni | Piattaforme e periodo | Fonte | Conf. |
|---|---|---|---|---|---|
| **AN/BQR‑15** (array + processore BQR‑23) | "fat" passiva, riavvolgibile | schiera **47 m (156 ft)**, Ø 9 cm (3,5"), **42 idrofoni**; cavo **670 m (2.200 ft)**; più tardi variante thin‑line SPALT 9080 da **1" di diametro esterno** | SSBN 608/616/627/640 dal 1974–76, poi Ohio. "Più tardi adottato anche sui 688" | `[NC-BQR15]` | media |
| BQR‑15, messa in mare | — | "**15 minuti** in condizioni ideali"; verricello idraulico che permette di filare, recuperare o **regolare** la schiera in immersione | — | `[MP-BQR15]` `[NC-BQR15]` | media |
| **TB‑16** (fat line) | passiva, sottosistema del BQQ‑5 | 73 m, Ø 3,5", **1.400 lb**; cavo 2.400 ft (731 m), **Ø 0,37"**, **450 lb** | 637 (in retrofit) e tutti i 688. Variante **TB‑16D** elencata per i 688 | `[FAS-TB16]` `[FAS-688]` | alta (dimensioni), media (TB‑16D) |
| TB‑16 "E" | — | nessuna fonte trovata | — | — | — |
| **TB‑23** (thin line) | passiva, bassissima frequenza | ~290 m (~960 ft). Cavo coassiale singolo **1–2 km**. Messa in mare **fino a 1,6 miglia** (~2,6 km) dietro il battello. Stivata **su bobine nelle casse zavorra** | 688 con BQQ‑5D/E e **688I con BSY‑1** (SSN‑751 *San Juan*, in servizio 6‑8‑1988) | `[FAS-TB23]` `[MA-TB23]` `[FAS-688]` `[SC-forum]` | alta (stivaggio, cavo), media (piattaforme) |
| TB‑29 | thin line, successore | più lunga della TB‑23, con localizzatori di forma | OPEVAL nel 1998: **fuori periodo** | `[FAS-TB29]` | alta |

**Introduzione della TB‑23.**
- Non ho trovato una data primaria. Le fonti la collegano al BQQ‑5D/E e al BSY‑1 `[MA-TB23]` `[FAS-688]`.
- Ipotesi difendibile per il 1989: **TB‑23 presente sui 688I** (da SSN‑751, 1988) e su una parte dei 688 Flight II con BQQ‑5D. **TB‑16 standard ovunque.** [bassa]
- I 688I portano due schiere: la TB‑16, che esce dal piano poppiero di **dritta**, e la TB‑23, che esce da quello di **sinistra** `[SC-forum]` [bassa, forum]. Un rapporto DTMB sul 688 conferma che la classe ha **tubi per schiera a sinistra e a dritta** `[DTIC-B180966]` [media, solo l'abstract].

### 1.2 Stivaggio e sistema di manovra (688)
- **Bobina o verricello nella cassa zavorra** (scafo singolo: lo spazio libero dall'acqua sta nelle casse).
  - Per la TB‑16 un forum colloca il verricello nella **cassa zavorra prodiera**: il tubo corre all'esterno lungo lo scafo dentro la **carenatura** (*towed array fairing*, libera di allagarsi), entra nelle casse poppiere e attraversa lo stabilizzatore orizzontale. Esce da un tubetto sull'**estremità esterna del piano di dritta** `[SC-forum]` [bassa].
  - Per la TB‑23 la bobina è nelle **casse poppiere** `[SC-forum]` `[MA-TB23]` [media].
  - Un brevetto USN conferma lo schema generale: sistema di manovra "nella cassa zavorra poppiera o a centro nave", da cui la schiera esce attraverso un **lungo tubo guida curvo** `[PAT-5855179]` [alta per lo schema].
- **OK‑276** = sistema di manovra della fat line (TB‑16). **Spinge** il cavo nel tubo con un carico assiale di compressione di **220 lb** (~1 kN). Il cavo leggero originale soffriva per **resistenza a compressione insufficiente** e per danni da *fish‑hook* (fili rotti o piegati) `[SBIR-037]` [media, testo SBIR letto tramite estratti].
- **Storia:** le prime schiere SSN erano **agganciate all'uscita dal porto** (*clip‑on*). Sui Sturgeon la carenatura della TB‑16 fu **aggiunta in retrofit** sul lato sinistro `[SC-forum]` `[RC-forum]` [bassa]. I 688 nascono con la carenatura.

### 1.3 Tempi di messa in mare e recupero
- Nessun dato pubblicato per la TB‑16. Il riferimento più vicino è la **BQR‑15: ~15 min "in condizioni ideali"** per circa 720 m tra schiera e cavo `[MP-BQR15]` [media].
- **STIMA DI GIOCO**, velocità di filata o recupero **≈ 0,8–1,0 m/s (50–60 m/min)**:
  - TB‑16 (~800 m) circa **14–17 min** in uscita e **16–20 min** in recupero;
  - TB‑23 (~1,5–2,5 km di scope) circa **30–45 min**.
  
  Che la TB‑23 sia più lenta lo dice anche il manuale del gioco *Fast Attack* (1996) `[FA96]` [bassa].
- **La schiera esce per prima**, perché sta in fondo al cavo:
  - è "bagnata" dopo pochi minuti, ma finché lo scope è corto resta immersa nel rumore e nella scia del battello;
  - al recupero rientra per **ultima**, quindi resta utile fino agli ultimi minuti (vedi §6).
- Lo scope si può **regolare** (BQR‑15: "streamed, retrieved or adjusted" `[MP-BQR15]`). Uno scope corto serve in acque basse.

### 1.4 Velocità
| Grandezza | Valore | Fonte | Conf. |
|---|---|---|---|
| Velocità minima efficace (fat line) | "**non efficaci sotto 3 kt**": la schiera **si incurva** (*droop*) da prua a poppa e perde rettilineità. A velocità molto basse aumenta il rischio di **contatto con il fondo**. L'obiettivo era tenere TB‑16 e TB‑34 dritte fino a 1,5 kt | `[SBIR-074]` | media‑alta (documento NAVSEA) |
| Requisito di straightness (2012) | droop prua‑poppa < 20 ft e deformazioni < 1 ft ogni 10 ft, tra 1,5 e 3 kt | `[SBIR-074]` (proposta) | media |
| Velocità massima di traino (meccanica) | **non pubblicata**. A velocità alte nei transiti nel Pacifico le schiere "si danneggiavano o si perdevano" | `[SC-forum]` | bassa |
| Velocità oltre cui il rumore di flusso la rende inutile | **non pubblicata**. La TB‑23 è "saturata sopra circa 12 kt" e la TB‑16 è "usabile più veloce" | `[FA96]` | bassa (gioco) |
| Superficie, per confronto | SQR‑18A "usabile solo a basse velocità, ma trainabile e filabile o recuperabile anche a velocità più alte" | `[USNI81]` | media |
| Superficie, per confronto | SQR‑19 operativa "a velocità tatticamente significative" fino a mare forza 4 | `[FI-SQR19]` | media |
| Superficie, per confronto | SQR‑19 "inutile sopra 19 kt, ottimale a 5 kt o meno" | forum HarpGamer | bassa |

**Fisica del rumore di flusso.**
- Il rumore di flusso cresce in modo ripido con la velocità: "aumenta drammaticamente con la velocità di traino" `[FFI-flow]`.
- Le leggi di scala vanno da ~U³ (`[PAT-6058072]`, citato in ricerca) a **+24 dB per raddoppio di velocità** (modello tipo Carpenter‑Kewley, ≈ 80·log v) `[JAC-2008]`. [media per la forma, bassa per l'esponente]
- **Strum** (vibrazione del cavo indotta da vortici): interferenza forte nel lobo principale, che "trapela" verso l'**endfire anteriore** `[PAT-6408978]` `[MIT-LL]` [media].

**Effetto sulla velocità del battello: la regola dei 2/3 del gioco originale.** Calcolo dell'autore [media]:
- Dati di ingresso:
  - superficie bagnata di cavo e schiera TB‑16 ≈ 42 m²;
  - Ct ≈ 0,006 (attrito tangenziale);
  - resistenza normale del cavo inclinato all'angolo critico ≈ peso in acqua del cavo;
  - resistenza di scafo del 688 ricavata da 35.000 shp a ~32 kt, con P ∝ v³ ed efficienza propulsiva 0,7.
- Risultato: la resistenza di schiera e cavo è **~1,2 kN a 5 kt, ~14 kN a 20 kt e ~35 kN a 32 kt**, cioè **circa il 3% della resistenza di scafo**.
- Effetto sulla velocità massima: **< 1%**, circa −0,3 kt.

**Conclusione.**
- Il limite dei 2/3 della velocità massima (~21 kt su 32) **non è realistico come effetto idrodinamico**.
- È però una buona **regola d'uso**: oltre ~18–22 kt la schiera è sorda e il cavo e la schiera subiscono carichi e fatica, quindi una schiera filata impone di fatto un limite dottrinale.
- Nel remake: **nessuna penalità di resistenza** (al più −1%), ma **danno cumulativo** sopra la velocità massima di traino (§6).

### 1.5 Quota della schiera sotto il battello (catenaria, angolo critico)
**Fisica [media].** Su un cavo lungo, pesante e sottile, trainato in corrente uniforme, la maggior parte del cavo si dispone all'**angolo critico** φ_c, dove la componente normale del peso eguaglia la resistenza normale:
- legge del seno quadrato di Pode `[Pode]` `[PAT-8047709]`;
- per un cavo neutro φ_c = 0 `[PAT-8047709]`.

```
w·cos φ_c = R·sin² φ_c ,   R = ½·ρ·C_n·d·v²
cos φ_c = (−w + sqrt(w² + 4R²)) / (2R)        (φ_c misurato dall'orizzontale)
Δz_cavo ≈ L_scope · sin φ_c                    (per v ≫ v0;  sin φ_c ≈ v0/v)
v0 = sqrt( w / (½·ρ·C_n·d) )
```

Dati TB‑16 `[FAS-TB16]`, con ρ = 1025 kg/m³ e C_n = 1,2 (cilindro, Re ~10⁴):
- cavo: d = 0,0094 m, massa in aria 0,279 kg/m, peso in acqua **w ≈ 2,0 N/m**, quindi **v0 ≈ 1,15 kt**;
- schiera: 1.400 lb su 73 m, Ø 3,5". Risulta negativa di circa 23 N/m se i 1.400 lb sono in aria. Il dato è incerto: le schiere sono riempite di fluido per avvicinarsi alla galleggiabilità neutra.

| v (kt) | φ_c | Δz cavo 730 m | Δz scope 1.500 m | Droop della schiera (73 m) | Tempo di raddrizzamento (803 m / v) |
|---|---|---|---|---|---|
| 2 | 32° | ~390 m | ~800 m | ~40 m | 13 min |
| 3 | 22° | ~270 m | ~560 m | ~29 m | 8,7 min |
| 4 | 16° | ~205 m | ~425 m | ~22 m | 6,5 min |
| 5 | 13° | ~165 m | ~340 m | ~18 m | 5,2 min |
| 6 | 11° | ~140 m | ~285 m | ~15 m | 4,3 min |
| 8 | 8,3° | ~105 m | ~215 m | ~11 m | 3,3 min |
| 10 | 6,6° | ~85 m | ~175 m | ~9 m | 2,6 min |
| 12 | 5,5° | ~70 m | ~145 m | ~8 m | 2,2 min |
| 15 | 4,4° | ~55 m | ~115 m | ~6 m | 1,7 min |
| 20 | 3,3° | ~40 m | ~85 m | ~5 m | 1,3 min |

Verifiche di coerenza:
- Il droop a 3 kt (~30 m ≈ 100 ft) è coerente con lo SBIR, che chiede < 20 ft e dichiara le fat line "inefficaci sotto 3 kt".
- Il tempo di raddrizzamento a 5–10 kt (2,6–5,2 min) riproduce i "2–5 min" già noti.
- Sulla SQR‑19 (cavo da 1", 1.700 m) la stessa fisica dà ~380 m di Δz a 10 kt, coerente con la "quota di traino fino a 360 m (1.200 ft)" `[FI-SQR19]`.

Incertezze:
- C_n con strum può valere 1,5–2, cioè −10/−20% di Δz;
- peso del cavo in aria o in acqua;
- carico del tubo guida.

**Errore stimato ±30%.** Nessuna curva ufficiale pubblicata.

**Conseguenza tattica.** A 3–6 kt la TB‑16 lavora **150–250 m sotto il battello**, quindi spesso **sotto il layer anche quando il battello è sopra**. È un vantaggio reale, e al tempo stesso il motivo del rischio di urto col fondo in acque basse. [media]

### 1.5 bis Rumori propri del cavo: vibrazione e sfregamento sul fondo (aggiunta del 28/10/2026, blocco 1 del sonar)

**Vibrazione del cavo (strum) [media per la fisica, STIMA per i livelli].**
- Un cilindro in corrente stacca i vortici di von Kármán a `f = St · v / d`, con St ≈ 0,2 per i numeri di Reynolds
  del cavo (10³–10⁵). Un cavo lungo e teso vibra alla frequenza di distacco: si chiama *strumming*. Le schiere hanno
  moduli d'isolamento dalle vibrazioni (VIM), che la attenuano ma non la tolgono.
- Per il cavo della TB-16 (d = 9,4 mm): circa 22 Hz a 2 kn, 55 Hz a 5 kn, 110 Hz a 10 kn, più la seconda armonica.
  Sono righe proprie, uguali in tutti i fasci, che salgono con la velocità; la corrente diversa lungo il cavo le
  allarga di un poco.
- **Nel gioco (STIMA):**
  - la fondamentale sta 6 dB sopra la densità del rumore del fascio, la seconda armonica 6 dB sotto;
  - sono sparse dell'1%, da 2 kn in su, solo col cavo filato (`nucleo/schiera.hpp`, `frequenze_vibrazione`);
  - il sonarista le riconosce come proprie, come le righe del battello.

**Sfregamento sul fondo [STIMA].** Nessuna misura aperta: lo SBIR (§2.1) parla solo del danno. Il cavo e la schiera
che strisciano danno un rumore impulsivo a larga banda, forte sotto qualche centinaio di Hz. Nel gioco:
- **livello:** 95 dB re 1 µPa²/Hz a pieno contatto su sabbia a 5 kn, 20-25 dB sopra il rumore proprio della TB-16;
- **velocità:** 30 log V;
- **contatto:** 10 log della frazione di schiera sul fondo;
- **fondo:** roccia +6 dB, fango −8;
- **forma:** piatto fino a 300 Hz, poi −20 dB per decade.

Entra nel rumore proprio della schiera (`rumore_sfregamento_db`), che quindi diventa quasi sorda in basso finché
striscia.

### 1.6 Prassi operativa
- **Messa in mare quasi permanente in pattuglia**: si recupera solo per i transiti veloci, per l'ingresso in acque basse o ristrette e prima di attraccare `[FA96]` [bassa]; coerente con `[SC-forum]`.
- **Sprint‑and‑drift:**
  - sprint a 15–20+ kt, in cui la schiera è sorda ma resta filata;
  - drift a 3–8 kt per 20–60 min, con ascolto sulla schiera;
  - poi di nuovo sprint.
  
  Anche le navi di superficie lo praticavano. L'episodio McCloy (1983) è esplicito: "recuperato il tail, sprint per una presa in percorso diretto, poi filato di nuovo" `[DMN]` [media]. Durata delle fasi: **STIMA DI GIOCO**.
- **In manovra la schiera è inutilizzabile:** "il beamforming non si può fare mentre la nave manovra, con tempi morti fino al **50%**" (TB‑23) `[FAS-TB23]` [alta]. La deformazione si misurava solo grossolanamente, con sensori di rotta e profondità.

### 1.7 Bande di frequenza, risoluzione e precisione
- **Bande:** non pubblicate per TB‑16 e TB‑23.
  - La SQR‑19 ha **aperture annidate VLF, MF e HF** `[FI-SQR19]` [media].
  - Il processore BQR‑23 gestiva **≥ 64 canali LOFAR** in tempo reale `[NC-BQR15]` [media]. La schiera è uno strumento **narrowband/LOFAR**: tonali di 50/60 Hz, frequenza di pala, ausiliari.
- **Perché le schiere lunghe scendono più in basso** (formule in `02…` §1.2):
  - θ₃dB ≈ 50,8°·λ/L al traverso e DI ≈ 10·log(2L/λ). Per avere un fascio utile serve L ≫ λ (a 50 Hz λ = 30 m);
  - TB‑16 (73 m) a 50 Hz: θ ≈ 21°, DI ≈ 7 dB;
  - TB‑23 (290 m) a 50 Hz: θ ≈ 5°, DI ≈ 13 dB. **[media, calcolo]**
  - La spaziatura degli idrofoni (≤ λ/2) fissa invece il limite **superiore**. Per la TB‑16, circa 50 idrofoni su 73 m danno ~1,5 m di spaziatura media, cioè ~500 Hz se uniforme. Per la BQR‑15, 42 su 47 m danno ~670 Hz.
- **STIMA DI GIOCO** per le bande: TB‑16 **10–600 Hz**, ottimale 30–300 Hz; TB‑23 **5–300 Hz**, ottimale 10–150 Hz. I sovietici chiamavano "**infrasonica**" la propria ГПБА `[RA-671]`.
- **Precisione di rilevamento:**
  - un sistema rimorchiato brevettato dichiara **σ ≈ 0,5° vicino al traverso e ≈ 6° vicino all'endfire** `[PAT-10448152]` [media, sistema diverso];
  - modello: σ_B ≈ θ₃dB(f)/(sin θ_rel·sqrt(SNR)), limitato a 0,3°.
- **Angolo conico:**
  - la schiera misura cos β = cos θ_az·cos ε, dove ε è l'elevazione del bersaglio vista dalla schiera;
  - vicino all'endfire una differenza di quota tra schiera e bersaglio produce **errori di azimut grandi** e "sdoppiamento" del fascio `[IEEE-9389083]` `[CPB-2022]` [media];
  - con la schiera 150–250 m più in basso del battello (§1.5) l'errore è concreto.
- **Endfire anteriore:**
  - è degradato dal rumore del proprio battello, che arriva da prua lungo l'asse della schiera;
  - è degradato anche da strum e vibrazioni del cavo, che "trapelano" nel cono acustico anteriore `[PAT-6408978]` [media];
  - in esperimenti il rumore della nave rimorchiante si distribuisce **fino a 10–90° dall'endfire** in acque basse (multipath) `[CPB-2022]` [media].
  
  **STIMA DI GIOCO:** settore cieco di ±20° a 5 kt, che cresce di +1° per nodo oltre i 5 kt (massimo ±40°), più ±10° di "risoluzione scarsa" all'endfire posteriore.
- **Ambiguità sinistra/destra:** già nota. Si risolve con una virata di 20–40° più il tempo di raddrizzamento, o con le gambe del TMA. Le schiere doppie (twinline) la eliminano, ma sono fuori periodo per i sottomarini `[APL-Twinline]`.

---

## 2. Rischi e guasti

### 2.1 Fondo e acque basse
- Lo SBIR NAVSEA lo dice esplicitamente: il traino a velocità molto bassa "**contribuisce grandemente alla probabilità di danno per contatto con il fondo**"; l'uso in acque basse e a velocità quasi in hovering è il caso critico `[SBIR-074]` [media‑alta].
- Brevetto USN: a bassa velocità la schiera affonda e c'è "maggiore possibilità di danno per collisione con il fondo o con ostacoli" `[PAT-5855179]` / ricerca ghiaccio [media].
- Con la tabella §1.5: in 200 m d'acqua, con il battello a 60 m e lo scope pieno, la TB‑16 tocca il fondo **sotto ~5 kt**.

### 2.2 Reti, pescherecci e altre navi (incidenti documentati)
| Data | Evento | Fonte | Conf. |
|---|---|---|---|
| ott.–nov. 1983 | Il Victor III **K‑324**, a ~282 nm W delle Bermuda, si impiglia con l'**elica** nel cavo della schiera della fregata USS *McCloy* mentre la pedina. Le fonti russe parlano di **~400 m di "cavo‑antenna corazzato TA58"** avvolti sull'elica. Il sottomarino è immobilizzato e fotografato con il pod esteso, poi rimorchiato a Cienfuegos; parti della schiera finiscono in URSS. Sulla *McCloy* il cavo si tende e si spezza, schizza sopra l'albero e si aggroviglia sul verricello | `[WP-K324]` (cita `[CWS]` p.160); `[RA-671]`; `[DMN]` | alta (fatto), media (dettagli) |
| 1982 | "Operazione Barmaid": HMS *Conqueror* taglia con cesoie montate a prua il **cavo da 3" (76 mm)** e porta via una schiera sovietica trainata da un AGI con bandiera polacca | `[WP-Conq]` (Telegraph 2012) | media |
| ~1983, 1985 | Gamberiere USA (*Howard M*, *Camaron*, quest'ultima affondata) agganciate al largo di Washington e Oregon da presunti sottomarini sovietici | `[ST91]` | media |
| 1‑11‑1989 | La SSBN USS *Alaska* aggancia la rete del peschereccio *Recruit* e lo **trascina all'indietro per ~6 min**; la Marina paga 42.000 $ | `[ST91]` | alta |
| 1989 | Un Foxtrot sovietico resta preso nella rete del peschereccio norvegese *T.O. Senior* (Barents) e riemerge subito. Il 13 agosto un altro sommergibile trascina un peschereccio all'indietro e taglia | `[DN89]` | media |
| 2009 (fuori periodo) | Un sottomarino cinese urta la schiera del USS *John S. McCain* presso Subic | `[CNN09]` | alta |

**Valutazione.**
- Non ho trovato un caso documentato di schiera di sottomarino **agganciata** da un peschereccio nel 1985–91. Il rischio è però fisicamente ovvio: le reti a strascico lavorano sul fondo delle piattaforme, e una schiera che pende 150–300 m sotto il battello vi entra.
- Il caso inverso è documentato ed è più frequente: è il sottomarino a impigliarsi nel cavo altrui.
- **STIMA DI GIOCO** per la probabilità (§6).

### 2.3 Virate strette, marcia indietro, elica
- L'uscita sull'estremità del piano poppiero "potrebbe impigliarsi nell'elica se non si fa attenzione; ci sono procedure per evitarlo" `[SC-forum]` [bassa, testimonianza].
- Il caso K‑324 mostra l'effetto di un cavo nell'elica: perdita di propulsione e vibrazione forte.
- **Marcia indietro con schiera filata:** il cavo perde tensione e risale o ricade verso il battello. Rischio di avvolgimento sull'elica **molto alto** [STIMA DI GIOCO].
- **Virate:** la schiera segue la scia; finché il battello non ritaglia la propria traccia entro la lunghezza dello scope, il rischio è modesto. Durante e dopo la virata la schiera è comunque **cieca** `[FAS-TB23]`.

### 2.4 Rottura, fatica, taglio d'emergenza
- **Fatica e compressione:** il cavo leggero della TB‑16 aveva problemi di compressione nel sistema OK‑276 e di *fish‑hook*. La Marina cercava un cavo più robusto accettando fino al +30% di peso `[SBIR-037]` [media].
- **Moto della coda:** la coda della fat line in flusso turbolento ha rollio e moti laterali e verticali, che degradano prestazioni e **affidabilità** (anche del sensore di rotta in coda) `[SBIR-035]` [media]. Lo stesso problema è affrontato con i *drogue* di coda contro *whipping* e *strumming* `[PAT-4734891]`.
- **Tensione eccessiva:** sulla *McCloy* "il tail si è allungato e spezzato" `[DMN]`.
- **Taglio d'emergenza:** nessuna fonte aperta su un dispositivo di taglio a bordo dei 688. **Lacuna.** Nel gioco conviene un comando "taglia schiera", istantaneo, con schiera persa: si giustifica come rilascio d'emergenza o recupero impossibile.

### 2.5 Ghiaccio
- Le schiere si usano sotto il pack nei bacini profondi (Hawkbill, SCICEX: "in acque profonde può filare la schiera per distanze di scoperta molto maggiori") `[Hawkbill]` [media].
- Nessuna procedura pubblica per l'emersione nel ghiaccio. Fisica: a velocità ~0 (emersione verticale) la schiera pende quasi in verticale, **~800 m sotto** il battello.
- **STIMA DI GIOCO:** emersione verticale o hovering con schiera filata uguale a recupero obbligatorio; altrimenti si applica la regola del fondo, e sulle piattaforme artiche (Barents, Chukchi) il danno è certo.

---

## 3. Schiere rimorchiate sovietiche

| Classe | Complesso sonar | Schiera rimorchiata | Stivaggio | Fonte | Conf. |
|---|---|---|---|---|---|
| Pr. 667AK (K‑403, Yankee convertito) | — | **banco prova** della ГПБА (flessibile, estesa, trainata) | contenitore sul timone verticale | `[RU-671]` `[RU-SKAT]` | alta |
| **Victor III** (671RTM/RTMK, dal 1979) | **MGK‑503 Skat‑KS** ("Shark Gill"; ru.wiki dice MGK‑500 per il prototipo) | ГПБА "**infrasonica**". Skat‑KS ha portata **2–3 volte** quella del Rubikon; "fino a **230 km** in condizioni idrologiche normali" | **pod a bulbo in cima al timone verticale** (*обтекаемая наделка на верхнем пере вертикального руля*). Il pod di grande raggio serviva anche a evitare le **crepe nel rivestimento in gomma** della schiera avvolta | `[RU-671]` `[RA-671]` `[RU-SKAT]` `[WP-Victor]` | alta (pod = schiera), bassa (230 km) |
| **Sierra I/II** (945/945A) | Skat‑KS su 2 unità (945); Skat‑3 su 945A | sì; pod sulla deriva, **molto più grande** sul Sierra II | pod | `[RU-SKAT]` `[WP-Sierra]` | media |
| **Akula I** (971, dal 30‑12‑1984) | **MGK‑540 Skat‑3** (digitale; prua + 2 antenne laterali + ГПБА) | "**Pelamida**" (Пеламида) | bulbo sul timone ("can") | `[RU-971]` `[RU-SKAT]` `[WP-Akula]` | alta (bulbo = schiera), bassa (nome "Pelamida": solo fonti secondarie) |
| **Oscar I/II** (949/949A) | MGK‑540 Skat‑3 | sì, perché fa parte di Skat‑3 | **nessun pod visibile**; punto d'uscita non trovato | `[RU-SKAT]` | media (presenza), bassa (geometria) |
| **Typhoon** (941) | Skat‑KS (TK‑208 poi Skat‑3) | sì ("antenna rimorchiata flessibile estesa" di Skat‑KS); più tardi "Pelamida" | non trovato | `[RU-SKAT]` `[WP-Typhoon]` | media |
| **Alfa** (705/705K) | **"Okean"** attivo/passivo, MG‑24 Luch; «Enisej» (МГК-1001) stazione passiva di scoperta, non intercettatore (dossier 19 e 28, 28/10/2026) | **nessuna** ("high speed precluded towed array") | — | `[WP-Alfa]` `[Legion-705]` | media‑alta |

Note:
- **Alfa con schiera nel gioco originale = errore o anacronismo da correggere.** Nessuna fonte attribuisce una schiera ai Pr. 705. [media‑alta]
- **Victor III, scoperta occidentale:** le foto del 1981 (USS *Drum*) e del 1983 (K‑324 con il pod esteso) chiarirono la funzione del pod. Prima si ipotizzavano propulsione MHD o un'arma `[WP-Victor]`. Una minoranza (Naval Submarine League) contestava che fosse una schiera.
- **Prestazioni:** frequenze e lunghezze sovietiche non pubblicate. **STIMA DI GIOCO**:
  - Victor III/Sierra I: schiera di ~150–250 m, banda 5–200 Hz, rumore di flusso peggiore di quelle USA (soglia ~6 kt);
  - Akula/Sierra II (Skat‑3): prestazioni circa da TB‑16 avanzata;
  - DT di 2–3 dB peggiore di quella USA per il processing.
- **Navi da ricognizione AGI:** le unità classe *Kolguev* con schiere lunghe supportarono nel 1987 l'operazione **"Atrina"** (5 Victor III nell'Atlantico) `[RA-671]` [media].

---

## 4. Navi di superficie

| Sistema | Dati | Classi al 1989 | Fonte | Conf. |
|---|---|---|---|---|
| **AN/SQR‑19 TACTAS** | schiera **242 m (800 ft)**, **Ø 82 mm**; cavo **1.700 m (5.600 ft)**, Ø 25 mm; **quota di traino fino a 360 m (1.200 ft)**; aperture VLF/MF/HF; mare forza 4 "a velocità tattiche". Peso: schiera ~4,5 t, manovra 7,6 t, elettronica 12,4 t | Prototipo su *Moosbrugger* (DD‑980) nel 1982. Installazioni operative dall'inizio FY87 su due DD‑963; 14 sistemi per **FFG‑7 LAMPS III**; **CG‑47 dal CG‑54 *Antietam*** (1987); DDG‑51. **Kidd: solo "considerata"** nel 1987, quindi assente nel 1989 | `[FI-SQR19]` `[USNI87]` `[FAS-SQR19]` | alta (dati), media (date per classe) |
| **AN/SQR‑18A(V)1 / (V)2** | versione senza VDS: schiera **730 ft (222 m)** su **cavo da 5.000 ft (1.524 m)**. La (V)1 è trainata **dal pesce VDS SQS‑35**. "Usabile solo a basse velocità" | (V)1 su **35 Knox** con VDS; (V)2 su Knox senza VDS e FFG‑7 in attesa della SQR‑19 | `[USNI87]` `[USNI81]` `[FAS-SQR18]` | media |
| SQR‑15 / TASS / ITASS | sperimentale e di transizione | *McCloy* e Bronstein; poi trasferita | `[DMN]` `[USNI87]` | media |
| **Type 2031 (I) / 2031Z** (UK) | passiva **VLF**, ~**500 m** | Leander convertite dal 1980; **Type 22 Batch 2** (scafo allungato per ospitarla; prima su HMS *Beaver*); Type 23 dal 1989 | `[Helis-2031]` `[Dowty-2031Z]` `[WP-T22]` | media |
| **URSS: VDS, non schiere** | **MG‑325 Vega** ("Mare Tail") su Krivak e Kara: corpo rimorchiato **a quota variabile, fino a ~100 m**. Wikipedia lo chiama "towed array", con portata 15 km: è **impreciso**. **Polinom** (MGK‑355) su Udaloy I: scafo "Horse Jaw" più VDS "Horse Tail" | — | `[WP-Svirepyy]` `[WP-Udaloy]` `01_…` | media |
| URSS, schiere lineari su unità combattenti | **nessuna in servizio nel 1989** fra quelle trovate. Zvezda M‑2 solo su Udaloy II (impostata 1989, in servizio 1999) | — | `[WP-Udaloy]` | media |

Nel gioco le navi di superficie con TACTAS devono:
- avere ambiguità sinistra/destra e un settore cieco a prua;
- applicare lo sprint‑and‑drift: drift ≤ 10 kt per l'ascolto, sprint 18–25 kt con schiera filata ma sorda [STIMA DI GIOCO];
- funzionare da innesco per il LAMPS III.

---

## 5. Tattica

### 5.1 SSN contro SSBN: pedinamento e scoperta in CZ
- **Batfish (SSN‑681), 1978**, operazione "Evening Star":
  - contatto con uno Yankee I nel Mare di Norvegia dopo l'innesco di SOSUS e dell'intelligence;
  - 51 h e 350 nm di pedinamento; contatto perso in tempesta e ritrovato da un P‑3;
  - poi **44 giorni e 8.870 nm** di trail.
  
  Il battello aveva una schiera riavvolgibile di "1.100 ft" (lunghezza probabilmente totale; Wikipedia la attribuisce alla TB‑16) `[WP-Batfish]` (cita Polmar & Moore) [media].
- Lo Yankee si tradiva con l'uso dell'**attivo MGK‑100** e con manovre di controllo dei baffles e quota periscopica **regolari, due volte al giorno** (ricerca, [media]).
- Schema: rilevamento del contatto LF sulla schiera (tonali) a distanze di **CZ** (prima CZ ~30–35 nm in Atlantico; vedi `02…` §2), oppure in percorso diretto. Seguono la chiusura in sprint‑and‑drift, poi il trail dal settore poppiero del bersaglio.
  - La schiera rende inutile il "Crazy Ivan" del bersaglio come rivelatore: il cacciatore vede la virata come un salto di variazione del rilevamento senza dover stare nei baffles stretti.
  - Le schiere sovietiche (Victor III dal 1979) tolsero agli SSN USA parte di questo vantaggio unilaterale.
- **Distanze di trail** tipiche di qualche migliaio di yard (con la sfera BQQ‑5 per la soluzione fine); con la schiera si allungano (`03…` §trail). [bassa]

### 5.2 Come si sfruttano i punti deboli di una schiera
1. **Avvicinamento dall'endfire anteriore**, cioè da prua al rimorchiatore, dove il suo rumore e lo strum mascherano. Oppure dalla poppa esatta, dove la risoluzione è pessima. [media, concetto]
2. **Ambiguità:**
   - finché il cacciatore non vira e non attende il raddrizzamento, la soluzione ha due specchi;
   - un bersaglio che **manovra spesso** costringe il cacciatore a virare a sua volta, e quindi a restare cieco fino al 50% del tempo `[FAS-TB23]`.
3. **Velocità:** costringere il cacciatore a correre oltre ~10–15 kt lo rende sordo sulla schiera.
4. **Acque basse e zone di pesca:** il cacciatore deve accorciare lo scope o recuperare. La quota della schiera si avvicina allora a quella del battello: rumore proprio maggiore, schiera spesso sopra il layer.
5. **Passaggio a poppa del rimorchiatore** per tranciarne o sporcarne il cavo: K‑324 1983, Cina 2009. È rischioso anche per chi lo fa, perché può finire con l'elica avvolta.
6. **Differenza di quota:** un bersaglio a quota molto diversa da quella della schiera, vicino all'endfire, produce errori d'azimut (angolo conico, §1.7).

### 5.3 Contro‑scoperta legata alla schiera
- **Messa in mare e recupero:** verricello idraulico, spinta del cavo nel tubo (OK‑276), urti meccanici a fine corsa. Livelli non pubblicati. **STIMA DI GIOCO:** transitorio meccanico all'inizio e alla fine (equivalente all'apertura di un portello di lancio) e, durante la manovra, **+3 dB** sul rumore a banda larga del battello sopra i 200 Hz.
- **Schiera filata, in sé:** la schiera è passiva e non irradia in modo apprezzabile. Lo strum del cavo è un problema di **auto‑rumore**, non di segnatura (nessuna fonte contraria). [bassa]
- **Contro‑scoperta visiva o radar:** da sottomarino nessuna. Su una nave di superficie il cavo a poppa e la tipologia sono noti all'avversario (le classi con TACTAS sono identificabili).

---

## 6. Modello consigliato per il gioco

### 6.1 Stati
```
STIVATA ──(cmd: fila, v ≤ V_manovra)──► IN_FILATA ──(scope = scope_cmd)──► FILATA
FILATA  ──(cmd: recupera, v ≤ V_manovra)──► IN_RECUPERO ──(scope = 0)──► STIVATA
FILATA/IN_FILATA/IN_RECUPERO ──(evento danno)──► DANNEGGIATA (prestazioni ridotte, recuperabile)
qualsiasi stato fuori bordo ──(rottura | taglio)──► PERSA (stato terminale per la missione)
DANNEGGIATA ──(danno ulteriore)──► PERSA
Sotto‑stato di FILATA: IN_VIRATA → IN_RADDRIZZAMENTO (timer) → STABILE
```
- In **IN_FILATA** la schiera è acusticamente utile quando scope ≥ L_schiera + 250 m, con +6 dB di auto‑rumore finché scope < 50% di quello comandato.
- In **IN_RECUPERO** resta utile finché scope ≥ L_schiera + 250 m.

### 6.2 Parametri

| Parametro | TB‑16 | TB‑23 | Sovietica (Skat‑KS / Skat‑3) | SQR‑19 | Base |
|---|---|---|---|---|---|
| L_schiera | 73 m | 290 m | 200 m (S) | 242 m | §1, §4 |
| Scope massimo del cavo | 730–790 m | 2.300 m | 800 m (S) | 1.700 m | idem |
| Velocità di filata | 0,9 m/s | 1,0 m/s | 0,8 m/s (S) | 1,2 m/s (S) | BQR‑15 15 min |
| Velocità di recupero | 0,75 m/s | 0,85 m/s | 0,7 m/s (S) | 1,0 m/s (S) | S |
| V_manovra (massima per filare o recuperare) | 12 kt | 10 kt | 10 kt | 15 kt | S; `[USNI81]` |
| V_min efficace | 3 kt (sotto: +droop, −6 dB DI) | 3 kt | 3 kt | 3 kt | `[SBIR-074]` |
| V_knee (inizio degrado da flusso) | 8 kt | 6 kt | 6 kt | 10 kt | S; `[FA96]` |
| Auto‑rumore oltre il knee | +60·log10(v/V_knee) dB | idem | +70·log10(v/V_knee) dB | idem | `[JAC-2008]` `[FFI-flow]`, S |
| "Sorda" (pratica) | ≈ 18 kt (+40 dB) | ≈ 12 kt | ≈ 12 kt | ≈ 20 kt | S |
| V_max di traino (meccanica) | **20 kt** | **16 kt** | 18 kt | 25 kt | S. La "regola dei 2/3" dell'originale ≈ 21 kt è accettabile come proxy |
| Penalità di velocità o resistenza | 0 (al più −1%) | 0 | 0 | 0 | §1.4 calcolo |
| v0 (angolo critico) | 1,15 kt | 1,0 kt (S, cavo più leggero) | 1,2 kt (S) | 2,2 kt | §1.5 |
| Quota massima della schiera (schiacciamento o danno) | 700 m | 700 m | 600 m | 360 m (operativa) | S; `[FI-SQR19]` |
| Banda (Hz) | 10–600 | 5–300 | 5–200 / 5–400 | 10–1.000 (S) | §1.7 |
| Settore cieco a prua (semi‑angolo) | 20° + 1°/kt oltre 5 kt (massimo 40°) | idem | 25° + 1°/kt | 25° + 1°/kt | S, `[CPB-2022]` |
| Settore poppiero a risoluzione scarsa | ±10° | ±8° | ±10° | ±10° | S |

(S) = **STIMA DI GIOCO**.

**Formule:**
```
# quota della schiera (centro schiera), φ dall'orizzontale
R      = 0.5*rho*Cn*d*v^2            # oppure in forma ridotta:
cosφ   = (-w + sqrt(w^2 + 4R^2)) / (2R)
z_ant  = z_boat + scope*sin(φ) + 0.5*droop(v)
droop(v) = L_schiera * min(1, vD/v_kt)   con vD ≈ 1,2 kt (TB-16, riproduce la tabella §1.5 entro ~20%), 0,6 kt (thin line)   [S]
#   forma semplice equivalente: sinφ ≈ min(1, v0/v_kt)
# fondo: punto più basso della schiera
z_low  = z_boat + scope*sin(φ) + droop(v)
```
- **Tempo di raddrizzamento** dopo una virata di Δψ gradi:
  `T_str = k·(scope + L_schiera)/v`, con k = min(1, Δψ/60°) e v in m/s.
  - Fino alla fine di T_str: rilevamenti con errore ×3 e ambiguità irrisolta.
  - Durante la virata (velocità angolare > 0,3°/s): uscita **non valida**.
  - Esempi: TB‑16 a 5 kt circa 5 min; a 10 kt circa 2,6 min.
- **Ambiguità:** due tracce speculari rispetto all'asse della schiera. Quella falsa si scarta quando, dopo una virata ≥ 20° e T_str, il suo rilevamento predetto differisce dal misurato di > 3σ.
- **Errore conico:** l'azimut misurato è β_m = acos(cos θ_az·cos ε).

### 6.3 Regole di danno e perdita (tutte STIMA DI GIOCO)
Le probabilità si calcolano per tick di 1 min, salvo indicazione.

| Evento | Condizione | Esito |
|---|---|---|
| **Contatto col fondo** | z_low ≥ fondale − 5 m | p_danno = 0,15 + 0,02·v_kt per minuto di contatto (×2 su fondo roccioso). Esiti: 70% DANNEGGIATA (−6 dB su DI e niente banda sopra 300 Hz); 30% PERSA. Primo contatto: avviso "schiera a contatto col fondo" |
| **Sopra V_max** | v > V_max | p = 0,02·(v − V_max) per minuto: DANNEGGIATA. Se v > V_max + 6 kt, p ×3 e 50% PERSA |
| **Fatica** | ore cumulative con v > 0,75·V_max | ogni 10 h, p = 5% di DANNEGGIATA |
| **Marcia indietro** | spinta negativa o v < 1 kt con macchina indietro, durata > 30 s | p = 0,4 per evento di avvolgimento sull'elica: schiera PERSA + **danno all'elica** (velocità massima −50%, rumore +10 dB) |
| **Virata stretta a bassa velocità** | Δψ > 120° entro T_str e v < 6 kt | p = 0,05 di contatto cavo‑piani o elica: 50% DANNEGGIATA, 50% come marcia indietro |
| **Reti e pesca** | cella "zona di pesca" (piattaforme Barents, Mare del Nord, GIUK, Grand Banks, Okhotsk) e z_low entro 0–600 m e ≤ fondale | p = 0,01 all'ora a 3–8 kt, ×2 di notte (flotte attive). Esiti: 60% PERSA (cavo spezzato), 40% DANNEGGIATA. Il peschereccio ha probabilità 0,3 di "segnalare" la posizione (contatto per l'IA avversaria) |
| **Cavo altrui** | passaggio a meno di 300 m dalla poppa di una nave con TACTAS filata, a quota < sua z_ant + 50 m | p = 0,3 di avvolgimento sull'elica (caso K‑324): il battello resta senza propulsione |
| **Ghiaccio** | emersione verticale o hovering con schiera filata | se fondale < scope + z_boat: contatto col fondo certo; altrimenti DANNEGGIATA al 20% (urto con chiglie di ghiaccio durante la risalita) |
| **Taglio volontario** | comando | PERSA all'istante, nessun transitorio rilevante |
| **Siluro o esplosione vicina** | ≤ 500 m dalla schiera | p = 0,5 di PERSA |

**Contro‑scoperta:**
- inizio e fine di filata o recupero = transitorio meccanico (come l'apertura di un portello);
- durante la manovra: +3 dB a banda larga sopra 200 Hz.

**Ricambi:** un 688 porta **una sola** schiera per tipo. PERSA vale per tutta la missione [bassa: nessuna fonte su ricambi a bordo; impossibile cambiarla in immersione].

### 6.4 Correzioni al gioco originale
1. **Alfa senza schiera.**
2. Nessuna penalità di velocità per la resistenza. Al suo posto: V_max di traino con danno cumulativo, più la sordità sopra il knee.
3. Va modellata la **quota della schiera ≠ quota del battello**: abilita il "sotto il layer" a bassa velocità e crea il rischio del fondo.
4. Il K‑324 (1983) va ricordato come incidente con la SQR‑15/TASS, non con la TACTAS.

### 6.5 Maggiori lacune di dati
1. **Velocità massima di traino e velocità oltre cui le schiere USA o sovietiche diventano sorde.** Nessun numero ufficiale; ci sono solo il "< 3 kt inefficace" di NAVSEA e il dato del gioco *Fast Attack* (TB‑23 ~12 kt).
2. **Tempi e velocità di filata e recupero** di TB‑16, TB‑23 e OK‑276. Unico riferimento: BQR‑15, 15 min.
3. **Bande di frequenza** e numero e spaziatura degli idrofoni di TB‑16 e TB‑23. Diametro della TB‑23: probabilmente ~1" come la SPALT 9080, non verificato.
4. **Data e piattaforme di introduzione della TB‑23** sui 688 non‑I.
5. **Peso in acqua del cavo e della schiera** TB‑16, quindi la curva quota‑velocità reale (qui ±30%).
6. **Schiere sovietiche:** lunghezze, bande, designazioni ("Pelamida" e "Barrakuda" solo da fonti secondarie); punto d'uscita su Oscar e Typhoon.
7. **Taglio d'emergenza e weak‑link** del cavo sui 688.
8. **Livelli di rumore** del verricello e della manovra della schiera.
9. Fonti chiave non lette direttamente:
   - Friedman, *Naval Institute Guide to World Naval Weapons Systems* (1989/1991), voci TB‑16/TB‑23/BQQ‑5;
   - Polmar & Moore, *Cold War Submarines*, oltre a p.160;
   - testo completo di Polmar, Proceedings ott. 1987 (bloccato);
   - SBIR NAVSEA (geobloccati, letti per estratti).

---

## 7. Sintesi in una riga per i sistemi
- **TB‑16**: 73 m, 3,5", ~800 m di scope. Sotto 3 kt si incurva, quasi sorda sopra ~15–18 kt; a 5 kt sta ~165 m sotto il battello.
- **TB‑23**: 290 m, thin line, fino a 2,6 km dietro. Più bassa in frequenza e più lenta da manovrare, sorda già verso 10–12 kt.
- **Victor III, Sierra, Akula**: schiera nel pod sul timone.
- **Oscar, Typhoon**: schiera presente, con geometria ignota.
- **Alfa**: nessuna schiera.
- **SQR‑19**: 242 m su 1.700 m di cavo, fino a 360 m di quota; FFG‑7 LAMPS III, Spruance, CG‑47 da CG‑54. Kidd senza.

---

## 8. Bibliografia
- **[FAS-TB16]** FAS, TB‑16 Fat Line Towed Array — https://man.fas.org/dod-101/sys/ship/weaps/tb-16.htm
- **[FAS-TB23]** FAS, TB‑23 — https://man.fas.org/dod-101/sys/ship/weaps/tb-23.htm
- **[FAS-TB29]** FAS, TB‑29 — https://man.fas.org/dod-101/sys/ship/weaps/tb-29.htm
- **[FAS-688]** FAS, SSN‑688 — https://man.fas.org/dod-101/sys/ship/ssn-688.htm
- **[FAS-SQR19]** https://man.fas.org/dod-101/sys/ship/weaps/an-sqr-19.htm ; **[FAS-SQR18]** https://man.fas.org/dod-101/sys/ship/weaps/an-sqr-18.htm
- **[FI-SQR19]** Forecast International, "SQR‑19 TACTAS – Archived 11/2003" — https://www.forecastinternational.com/archive/disp_old_pdf.cfm?ARC_ID=1381
- **[NC-BQR15]** Nuclear Companion, AN/BQR‑15 — https://nuclearcompanion.com/data/an-bqr-15-towed-array-sonar/
- **[MP-BQR15]** Military Periscope, AN/BQR‑15 (estratto) — https://militaryperiscope.com/weapons/sensorselectronics/sonars/anbqr-15/overview
- **[MA-TB23]** Military & Aerospace Electronics, 2005, TB‑23/BQ — https://www.militaryaerospace.com/communications/article/16713072/navy-chooses-l-3-to-upgrade-tb-23-bq-towed-sonar-array
- **[SBIR-074]** NAVSEA SBIR N121‑074 "Fat Line Towed Array Straightening System" — https://navysbir.com/n12_1/N121-074.htm (pagina geobloccata, letta tramite estratti di ricerca)
- **[SBIR-037]** NAVSEA SBIR N151‑037 "Fat Line Tow Cable" — https://navysbir.com/n15_1/N151-037.htm (idem)
- **[SBIR-035]** NAVSEA SBIR N191‑035 "Fat Line Towed Array Aft Stabilizer" — https://navysbir.com/n19_1/N191-035.htm (idem)
- **[USNI81]** "The U.S. Navy: Sonars, Part I", *Proceedings*, luglio 1981 — https://www.usni.org/magazines/proceedings/1981/july/u-s-nay-sonars-part-i (via estratti)
- **[USNI87]** N. Polmar, "The U.S. Navy: Tails for Warships", *Proceedings*, ottobre 1987 — https://www.usni.org/magazines/proceedings/1987/october/us-navy-tails-warships (via estratti)
- **[DMN]** Defense Media Network, "Bronstein‑class escorts introduced new sensors and weapons", p.3 — https://www.defensemedianetwork.com/stories/bronstein-class-escorts-introduced-new-sensors-and-weapons/3/
- **[CWS]** N. Polmar, K. J. Moore, *Cold War Submarines*, Brassey's 2004 (p.160 via `[WP-K324]`)
- **[WP-K324]** https://en.wikipedia.org/wiki/Soviet_submarine_K-324 ; **[WP-Batfish]** https://en.wikipedia.org/wiki/USS_Batfish_(SSN-681) ; **[WP-Conq]** https://en.wikipedia.org/wiki/HMS_Conqueror_(S48)
- **[WP-Victor]** https://en.wikipedia.org/wiki/Victor-class_submarine ; **[WP-Akula]** https://en.wikipedia.org/wiki/Akula-class_submarine ; **[WP-Sierra]** https://en.wikipedia.org/wiki/Sierra-class_submarine ; **[WP-Typhoon]** https://en.wikipedia.org/wiki/Typhoon-class_submarine ; **[WP-Alfa]** https://en.wikipedia.org/wiki/Alfa-class_submarine
- **[RU-671]** ru.wikipedia, «Подводные лодки проекта 671РТМ(К) «Щука»» ; **[RU-971]** ru.wikipedia, «Подводные лодки проекта 971 «Щука-Б»» ; **[RU-SKAT]** https://ru.wikipedia.org/wiki/Скат-3
- **[RA-671]** rusarmy.com, АПЛ проекта 671РТМ — https://rusarmy.com/vmf/apl_pr_671.rtm.htm
- **[Legion-705]** https://legion.wplus.net/guide/navy/su/705.shtml
- **[WP-Svirepyy]** https://en.wikipedia.org/wiki/Soviet_frigate_Svirepyy ; **[WP-Udaloy]** https://en.wikipedia.org/wiki/Udaloy-class_destroyer ; **[WP-T22]** https://en.wikipedia.org/wiki/Type_22_frigate
- **[Helis-2031]** https://www.helis.com/database/sys/274/ (via estratti) ; **[Dowty-2031Z]** https://www.dowtyheritage.org.uk/content/dowty-group/electronics/sonar-2031z-development-at-waverley-electronics
- **[ST91]** AP / Seattle Times, 11‑2‑1991 — https://archive.seattletimes.com/archive/19910211/1265738/fishermen-say-they-sometimes-snag-subs----navy-denies-any-knowledge
- **[DN89]** Deseret News, 2‑11‑1989 — https://www.deseret.com/1989/11/2/18830615/fishing-boat-catches-a-soviet-sub-in-its-nets
- **[CNN09]** https://edition.cnn.com/2009/US/06/12/china.submarine/index.html
- **[SC-forum]** Subcommittee forum, "LA Towed array??" (2005) — https://forum.subcommittee.com/forum/general-forums/general/3103-la-towed-array
- **[RC-forum]** rc‑sub forum, "637 SSNs towed array details" — https://forum.rc-sub.com/forum/general-topics/1158-637-ssns-towed-array-details
- **[DTIC-B180966]** "Hydrodynamic Towing Stability Evaluation of SSN‑688 Class Submarines" (DTMB), abstract — https://apps.dtic.mil/sti/html/tr/ADB180966/index.html
- **[PAT-5855179]** US 5,855,179, "Mid ships tow point…" — https://patents.google.com/patent/US5855179
- **[PAT-8047709]** US 8,047,709 (angolo critico del cavo) — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/8047709
- **[PAT-6408978]** US 6,408,978, "Non‑acoustic self noise canceller…" — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6408978
- **[PAT-10448152]** US 10,448,152 (σ di rilevamento al traverso e all'endfire) — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/10448152
- **[PAT-4734891]** US 4,734,891, "Aft drogue module for towed sonar array" — https://patents.google.com/patent/US4734891
- **[PAT-6058072]** US 6,058,072, "Velocity reduction method to reduce the flow‑induced noise of towed sensor systems" — https://patents.google.com/patent/US6058072
- **[Pode]** L. Pode, DTMB, legge del seno quadrato per la forza normale su cavo — https://apps.dtic.mil/sti/pdfs/AD0652335.pdf (e il classico DTMB Report 687, 1951)
- **[FFI-flow]** FFI, "The impact of flow noise on towed and hull mounted acoustical sensors" (rapporto 04/00858) — https://ffi-publikasjoner.archive.knowledgearc.net/handle/20.500.12242/1717
- **[JAC-2008]** *Acta Acustica* (cinese), 2008, n.5, p.003 — https://jac.ac.cn/en/article/cstr/32049.14.11-2065.2008.05.003 (≈ +24 dB per raddoppio di velocità, via estratti)
- **[MIT-LL]** MIT Lincoln Lab, beamforming adattivo contro l'auto‑rumore delle schiere (strum nel lobo principale) — https://apps.dtic.mil/sti/pdfs/ADA422438.pdf (via estratti)
- **[CPB-2022]** *Chinese Physics B* 31(5) 054301 (2022) — https://cpb.iphy.ac.cn/EN/10.1088/1674-1056/ac364e
- **[IEEE-9389083]** https://ieeexplore.ieee.org/document/9389083
- **[APL-Twinline]** R. Henrick, "SURTASS Twinline", *JHU APL Tech. Digest* 34(2) — https://jhuapl.edu/sites/default/files/2024-09/34-02-Henrick.pdf
- **[Hawkbill]** "SCICEX 99", *Professional Mariner* — https://usshawkbill.com/scicex99.htm
- **[FA96]** guida al gioco *Fast Attack* (Sierra, 1996) — https://www.cyberly.org/game-cheats/pc-walkthroughs/fast-attack.html (fonte ludica: modella erroneamente la TB‑23 come più spessa della TB‑16)
