# 20 — Periscopi: modelli, viste, rischio e dottrina (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. Scopo: implementare i **due periscopi del 688** (Type 2 d'attacco e Type 18 di ricerca),
> importanti per l'utente per il gameplay e le meccaniche; il periscopio dell'Alfa; quelli dei battelli dell'IA, cioè
> come i nemici vedono il giocatore. Coperte tutte le classi di sottomarino di `classi.toml`: Los Angeles, Sturgeon,
> Trafalgar; Alfa, Victor III, Akula I, Oscar II, Typhoon, Kilo, Foxtrot, Charlie II.
> Compilato: 25 ottobre 2026, da quattro ricerche parallele (688 e Alfa; Sturgeon, Trafalgar e dottrina; periscopi
> sovietici classe per classe; caratteri, nel dossier 19).

---

## 0. Convenzioni, fonti e punto di partenza

**Confidenza.** **alta** = manuale ufficiale, brevetto, rapporto governativo, costruttore; **media** = fonte
secondaria seria (Submarine Review, Apalkov, ru.wikipedia, NOVA, *The Silent Deep*); **bassa** = forum, gioco,
divulgazione; **n.v.** = non trovato. **STIMA** = valore proposto per il gioco. **[GIOCO]** = proposta.

**Le fonti forti.**
- Il **manuale ufficiale dei periscopi della US Navy del 1946** (NavPers 16165), capitolo del Type II `[PSM]`. Il
  Type 2 del 688 è lo stesso progetto aggiornato, ancora in servizio negli anni '90.
- Il **brevetto Kollmorgen** della testa a tre ingrandimenti del Type 18 `[PAT]`.
- Il **rapporto NTSB sul Greeneville** (procedura NWP del giro di periscopio e quote) `[NTSB]`.
- Il **manuale di dottrina dei sottomarini del 1944** `[USF25]`.
- Il **manuale sovietico di condotta della nave del 1974** `[SPR74]`.
- Per le classi sovietiche, il **repertorio di Apalkov** `[APA]`.

Dei periscopi sovietici del dopoguerra quasi nessun dato ottico è pubblico.

**Lo stato di oggi nel gioco.**
- Un solo periscopio con due ingrandimenti: 1,5× con 32° di campo e 6× con 8° (gli stessi del Type 2).
- Lo stadimetro, la notte con la luce ambiente e il livello ESM.
- Il requisito 8 della roadmap chiede «ingrandimenti reali, stadimetria, ESM, visione notturna/IR dell'epoca».
- La scoperta radar di alberi e periscopi è già modellata nel dossier 11, §5.

---

## 1. Che cos'è un periscopio nel 1989

- **Due ruoli.**
  - Il **periscopio d'attacco** ha la testa sottile: poca piuma, poca sezione radar, ottica semplice.
  - Quello **di ricerca** (o «universale») ha la testa grossa con più ingrandimenti, la visione notturna, la TV e la
    fotocamera, e le **antenne ESM** e radio.
  - Si usa il primo vicino al bersaglio e il secondo per cercare, ascoltare i radar e fotografare `[SD]` `[NMVM]`
    (media).
- **Due modi di misurare la distanza.**
  - **Stadimetro a immagine sdoppiata** (USA, Regno Unito): si sovrappone il galleggiamento dell'immagine fantasma
    alla testa d'albero di quella vera e si legge la distanza sul quadrante, data l'altezza dell'albero.
  - **Reticolo graduato** (sovietici, e di riserva ovunque): si contano le divisioni coperte dal bersaglio e si
    ricava la distanza dalle tabelle, sapendo l'altezza.
- **Notte.**
  - Occhio nudo, con le lenti grandi;
  - **intensificatore d'immagine** (ЭОП raffreddato nei sovietici dal 1965 `[MM]`; nel CH84 britannico);
  - **TV a bassa luminosità** (Type 18; complessi TV sovietici МТ-70 e ТВ-1);
  - **termico**, solo britannico nel 1989 (CH84) `[RNS]`.

---

## 2. Stati Uniti

### 2.1 SSN-688 Flight I: due periscopi

- **Dotazione:** **Type 2** d'attacco (n. 1) e **Type 18** di ricerca (n. 2) `[NSL2]` `[NOVA-P]` (media).
  - Il lato è incerto: NOVA mette il Type 2 a dritta, FAS a sinistra. Proposta: il Type 2 a dritta.
  - Il Type 18 entra in servizio nel 1975 sul Cavalla (SSN-684); 52 costruiti entro il 1981, montati sui 637 e sui
    688 `[NSL2]` (media-alta).
- **Quota periscopica** `[NTSB]` (alta):
  - **60 piedi alla chiglia**; 58 per «una guardata alta»;
  - l'occhio del n. 2 sta a **64 ft 7 in** sopra la chiglia, quindi la finestra è a ~1,4 m sull'acqua calma a 60 ft
    e a ~2 m a 58 ft `[GRIF]`.

  Il gioco oggi usa ~1,5 m: va bene.

#### Type 2, d'attacco `[PSM]` (alta, manuale 1946; ancora in uso negli anni '90 `[KOL]`)

| Dato | Valore |
|---|---|
| Ingrandimenti / campo vero | **1,5× / 32°**, **6× / 8°** |
| Elevazione della linea di mira | **da −10° a +74,5°**; il bordo del campo arriva a 90,5° (1,5×) e 78,5° (6×): tutto il cielo |
| Testa | punta da **1,414 in** (Type 2 di guerra); **2,4 in** nel Type 2D del 1959 `[NSL2]` |
| Tubo, lunghezza ottica | 7,5 in; 40 ft |
| Pupilla d'uscita | 4 mm |
| Notte | **solo diurno** `[NOVA-P]` |
| Fotografia | aggiunta nelle versioni del dopoguerra `[NSL2]` (media) |
| Filtri | disco a filtri (rosso, verde, giallo, chiaro `[JTM]`, bassa) e **polaroid a densità variabile** contro i riflessi del mare |

- **Impugnature** `[PSM]` (alta):
  - **sinistra**: girandola alza o abbassa il prisma della testa (elevazione), con fermi a molla a **14° e 44°**. La
    ricerca aerea si fa a basso ingrandimento in tre giri completi, a 14°, 44° e 74,5°.
  - **destra**: girandola cambia ingrandimento (1,5× ↔ 6×);
  - la messa a fuoco è un pomello a parte.
- **Stadimetro a immagine sdoppiata** `[PSM]` (alta):
  - l'obiettivo inferiore è tagliato in due; un volantino in basso a destra separa le metà e sdoppia l'immagine;
  - si porta il **galleggiamento di un'immagine sulla testa d'albero dell'altra**; sui quadranti concentrici di
    bachelite si imposta l'altezza (15-130 ft) e si legge la distanza (2,2-110 centinaia di yarde);
  - girando ancora, la lente ruota di 90°: portando la prua sulla poppa si ricava l'**angolo sulla prua** dalla
    lunghezza del bersaglio (100-1000 ft);
  - quadranti doppi sul retro per l'aiutante; angolo massimo 2° a 6×.
- **Reticolo** `[PSM]` (alta): lastrina incisa con graduazioni orizzontali e verticali in gradi di campo vero.
  - Divisioni grandi **1° a 6×** e **4° a 1,5×**; suddivisioni **15′ a 6×** e **1° a 1,5×**.
  - Col battello in moto la distanza col reticolo è scomoda: per questo c'è lo stadimetro.

#### Type 18, di ricerca

| Dato | Valore | Fonte |
|---|---|---|
| Ingrandimenti / campo | **1,5× / 32°, 6× / 8°, 12× / 4°** (testa a tre ingrandimenti); **24× / 2°** nelle schede più tarde (probabile 12× raddoppiato, n.v. nel 1989) | `[PAT]` alta; `[K18]` media-bassa |
| Elevazione | **da −10° a +60°** | `[K18]` |
| Stabilizzazione | su un asse | `[K18]` |
| Notte | **modo a bassa luminosità** fin dal progetto (1968) | `[NSL2]` media |
| TV | **sì**, al monitor in centrale | `[NSL2]` `[NOVA-P]` |
| Foto | 70 mm (poi digitale) | `[NOVA-P]` |
| ESM | sezione ESM nella testa dal contratto del 1972; goniometria a monoimpulso aggiunta più tardi (MDF) | `[NSL2]` alta; `[FY98]` alta |
| Telemetro | lo stadimetro (citato nel brevetto, «stadiametric ranging») | `[PAT]` |

- **Il modo notte nel 1989.** Intensificatore a visione diretta o TV a bassa luminosità: n.v. La TV e la fotocamera
  digitale **a colori** arrivano solo dopo il 1992, quindi nel 1989 il monitor era quasi certamente **in bianco e
  nero** (media, deduzione).
- **Rilevamento.** Col Type 2 di guerra si leggeva l'anello sul collare (relativo, tre cifre) e si chiamava
  «bearing… mark», con cicalino e lampada per il gruppo di tiro `[TFC]`. Sul 688 nel 1989 il rilevamento andava
  probabilmente al Mk 117 da un trasmettitore (n.v.). Più tardi il **pulsante MARK** sta sull'impugnatura destra
  (Proceedings 2021, estratto).

### 2.2 Sturgeon (SSN-637)

- **Due periscopi**: la classe ne aggiunge uno rispetto alla Permit `[WIKI-637]` (media).
  - **N. 1 d'attacco** (Type 2), con lo stadimetro in basso: testa più bassa e piuma minore `[NMVM]`.
  - **N. 2 di ricerca**, con la telecamera e le antenne radio ed ESM: testa più grossa, più piuma; era il più usato.
- Prima del Type 18 il n. 2 era probabilmente un **Type 8B o Type 15** (Type 8 con l'ESM) `[NSL2]` (n.v. sugli
  Sturgeon). Nel 1989 la maggior parte ha il Type 18 (media).
- Per confronto, il Type 8L del Trident: 1,5×/6×, 32°/8°, elevazione da −10° a +60° (bassa-media).

---

## 3. Regno Unito: Trafalgar

- **Nomenclatura Barr & Stroud/Pilkington: CH = attacco, CK = ricerca.** La Trafalgar ha **CH84 d'attacco e CK34 di
  ricerca** `[RNS]` (media), «i primi periscopi elettronici» (1983).
  - **CH84:** come il CH83, con **intensificatore d'immagine, TV e termico** di serie; tubo più largo in acciaio
    migliore, ottica antivibrazioni.
  - **CK34:** binoculare, con l'ESM «Morthoe», la camera e la TV a bassa luminosità; finestra riscaldata (Jane's,
    bassa-media).
  - Il **comando a distanza** arriva solo col CH85/CK35 degli Upholder (1986).
- **Contraddizione.** Un racconto dall'HMS Tireless (~2012) dice che l'attacco è «poco facile da guardarci» e senza
  sensori elettronici, e che la ricerca ha «una testa enorme» con sensori e GPS `[SD]`. Potrebbe essere un
  allestimento più tardo.
- **Ingrandimenti, campi e diametri** di CH84 e CK34: **n.v.** Proposta: 1,5×/6× come gli altri dell'epoca.
- **Quota periscopica della Trafalgar: 18 m** `[SD]` (media).

---

## 4. Unione Sovietica

### 4.1 Dotazione per classe (1985-1990)

| Classe | Periscopi | Altri alberi | Fonte |
|---|---|---|---|
| **Alfa** (705/705K) | **uno**: «Сигнал», universale e automatizzato (in servizio dal 1972), «del sistema **ТВ-1**» | ПМУ combinato (РКП con l'antenna «Айва»), radar «Чибис», radiogoniometro «Весло-П» | `[APA]` `[DS705]` media-alta; `[MMH]` bassa-media |
| **Victor III** (671РТМ) | **ПЗНГ-10М** e il periscopio del complesso TV **МТ-70-10** | radar «Каскад» МРК-50, ESM «Залив-П», radiogoniometro «Завеса» | `[APA]` media-alta |
| **Akula I** (971) | del comandante **«Лебедь-11» o «Лебедь-21»** e il TV **«Сигнал-3»** | «Радиан», ТК МТК-110 | `[APA]` `[RW-971]` media-alta |
| **Oscar II** (949A) | universale TV **«Сигнал-3»** e del comandante (d'attacco) **ПЗНС-10С o «Лебедь-11»** | «Радиан-У» | `[RW-949]` media |
| **Typhoon** (941) | TV **«Сигнал-3»** e del comandante **«Лебедь-21»** | МТК-110, «Радиан», ESM «Залив-П» | `[APA]` media-alta |
| **Kilo** (877) | d'attacco **ПК-8,5**, zenitale e di navigazione **ПЗНГ-8М** | «Каскад», ESM МРП-25, radiogoniometro «Рамка», presa d'aria (РДП) | `[APA]` media-alta |
| **Foxtrot** (641) | d'attacco **ПК-8,5**, zenitale e di navigazione **ПЗНГ-8** | ESM **«Накат»**, radar «Флаг», «Рамка», РДП | `[APA]` media-alta |
| **Charlie II** (670M) | **ПЗНС-10** e il TV **МТ-70** | «Каскад», «Корма», ESM «Залив-П», «Весло-П» | `[APA]` media-alta |

**Lo schema** (media):
- la 2a generazione nucleare ha ПЗНС-10 o ПЗНГ più il complesso TV МТ-70;
- la 3a ha «Лебедь» (comandante, attacco) più «Сигнал-3» (universale TV);
- i diesel hanno ПК-8,5 (attacco) più ПЗНГ-8 (zenitale e di navigazione).

### 4.2 Che cosa si sa dei modelli

- **ПЗНС:** «nel 1965 entrò in servizio sui sottomarini della seconda generazione il primo periscopio universale
  ПЗНС, con un **intensificatore raffreddato nel canale notturno**»; poco dopo lo «Зрачок» con la notte a TV `[MM]`
  (media-bassa). Sigla non sciolta: probabilmente «zenitale, di navigazione» (bassa).
- **МТ-70** (anche «Кефаль»?): complesso TV per la superficie e i ghiacci. Con il tempo buono funziona **fino a ~50
  m di profondità**: guarda la superficie e il ghiaccio da sotto `[PAV]` (media).
- **«Сигнал», «Сигнал-3», «Лебедь-11/21», ПК-8,5, ПЗНГ-8/10М:** ruolo noto; **ingrandimenti, campi, teste, tempi
  e velocità n.v.**
- **Costruttori:** LOMO (Leningrado) e KOMZ (Kazan). Un accademico dell'Электроприбор parla di un ritardo «di una
  decina d'anni» dei periscopi russi alla fine degli anni '90 `[RG]` (media-alta).
- **Dati di guerra, per l'ordine di grandezza:** i periscopi zenitali ПЗ hanno 1,5× e 6×, campo 40°/10° (20°/5°
  nell'oculare di misura), elevazione da −15° a +90°, e la testa più grossa di quelli del comandante (media-bassa).
  Il brevetto LOMO del 1997 descrive i modelli precedenti con un solo canale visivo e due ingrandimenti scambiati
  girando di 180° l'obiettivo della testa `[PAT-RU]` (alta).

### 4.3 Il reticolo sovietico e la misura

- **Griglia goniometrica a due scale**, verticale e orizzontale. Dall'altezza del bersaglio e dall'angolo
  verticale si ricava la distanza, dalla lunghezza e dall'angolo orizzontale l'angolo sulla prua. Nel mare mosso è
  imprecisa.
- Per la distanza precisa c'era un **micrometro a coincidenza** (immagine sdoppiata) nei modelli di misura
  (media-bassa). I periscopi del comandante ПА di guerra **non** avevano il telemetro a immagine sdoppiata, e i
  comandanti cercavano di avvicinarsi al minimo `[KP]` (media).
- Davanti all'oculare un **disco a tre settori**: vetro affumicato per il sole, rosso per la nebbia. C'erano anche
  la staffa della macchina fotografica e l'aria secca contro l'appannamento `[RAN]` (bassa-media).
- Nessuna foto di un reticolo sovietico del dopoguerra trovata.

### 4.4 Alfa: «Сигнал» e ТВ-1

- **Un solo periscopio**, il «Сигнал». Nessuna fonte dà al 705 un secondo periscopio `[DS705]` (media).
- Il **pulto del comandante** ha uno **schermo TV con «la situazione esterna dal periscopio»** e i **comandi a
  distanza del periscopio** (Собакин). C'è anche una **telecamera verticale nella vela**, mostrata sul pulto dopo
  aver abbassato il periscopio (Коляда) `[SOB]` `[KOL705]` (media-alta).
- Solo comandante, secondo e ufficiale di guardia vedevano la luce del giorno, «soltanto attraverso il periscopio».
  Una volta il periscopio gelò nell'Artico e si andò col solo radar `[KOL705]`.
- **Dati ottici, notte e telemetro: n.v.** Il canale notturno raffreddato esisteva sulla 2a generazione dal 1965,
  quindi è plausibile anche sul «Сигнал» del 1972 (STIMA).

---

## 5. Visibilità e rischio

- **Quanto esce.**
  - Un periscopio «sporge un metro» (3 ft) `[NSL-RAD]`; l'occhio del n. 2 del 688 sta a 1,4-2 m sull'acqua calma
    `[NTSB]` `[GRIF]`.
  - Sovietici: testa a 1-1,5 m, orizzonte a 21-25 cavi (bassa).
- **Sezione radar** (§5 del dossier 11 per il modello di scoperta):
  - un cilindro di raggio 0,1 m esposto per 1 m, in banda X, fa **~6,7 m² di traverso**, molto meno fuori dalla
    normale; nei calcoli si usa **1 m²** nominale `[DTIC-PDR]` (media). Un articolo coreano dà **0,01-0,1 m²**
    (media).
  - **Attacco contro ricerca** (calcolo, STIMA): la sezione del cilindro cresce col raggio e col quadrato
    dell'altezza esposta (σ ≈ 2π·r·L²/λ). La testa di ricerca (~7,5 in con le antenne ESM, che fanno da riflettori
    d'angolo) contro quella d'attacco (1,4-2,4 in) vale **3-5 volte** a parità d'esposizione. Il doppio
    dell'altezza vale ~4 volte.
- **Portate radar:**
  - dal punto di vista sovietico (Зарубежное военное обозрение 1983-84), con mare 2-3: teste della presa d'aria
    (РДП) a **12-15 nm**, **periscopi a 4-5 nm**, antenne di radiogoniometria ed ESM a **1-2 nm** `[RW-OBN]` (media);
  - APS-116/137 a 15,5 nm (bassa);
  - la scoperta aerea dei periscopi si ferma a ~20 nm con mare moderato e grosso, e degrada molto **da mare 5 in su**
    `[DTIC-PDR]`;
  - i radar per periscopi integrano più giri (300 rpm, 5 giri al secondo): **più resta su, più si vede** `[APL97]`
    (alta).
- **Piuma e scia.**
  - Cresce con la velocità; **a 10 nodi** è «una piuma enorme che si vede da miglia» `[SD]`.
  - Scia di ~25 m a 5 nodi e oltre 40 m a 8 (bassa).
  - Con vento e mare da poppa si vede poco `[USF25]` (alta).
  - Il trucco britannico: guardare, abbassare, poi accelerare appena sotto la quota periscopica `[SD]`.
  - La testa d'attacco fa una piuma più sottile.
- **A vista:**
  - un periscopio sopra l'orizzonte si vede col binocolo «a distanze sorprendenti» `[USF25]`;
  - **riflessi del sole** sulla testa;
  - un elicottero ASW col vento a 12 nodi cerca la piuma a occhio col radar spento `[SD]`.
- **Velocità col periscopio alzato:**
  - in guerra «alla minima possibile» `[USF25]`; limiti di vibrazione sotto 5 nodi per gli U-boot;
  - i periscopi nucleari reggono di più, ma nessun numero è pubblicato `[NSL1]`;
  - Greeneville: il comandante alzò il periscopio solo «dopo aver rallentato» `[NTSB]`.
  - Proposta: §7.

---

## 6. Estetica

### 6.1 Nell'oculare

- **USA:**
  - campo **tondo** con forte **vignettatura** ai bordi;
  - linea verticale sottile e una colonna di **tacche orizzontali graduate** (lunghe e corte) a intervalli di grado;
  - tacche sull'orizzontale in basso (il «telemetro» delle foto di guerra);
  - nelle foto dei 688 (Houston 2006, Tucson 1999): una **croce sottile** a tutto campo, una tinta calda e la
    vignettatura;
  - **nessun dato sovrapposto** (nel 1989 probabilmente non c'era).
- **Riferimenti:**
  - di pubblico dominio: «Valiant Shield … USS Houston (SSN-713) … periscope», «USSReubenJamesUSSTucsonPeriscopePhoto»,
    «Torpedoed Japanese destroyer Yamakaze photographed through periscope of USS Nautilus», «… Harusame …»,
    «… USS Independence (CV-62) seen through the periscope of … USS Batfish (SSN-681) … 1992»;
  - CC: «Periskopsicht.jpg» (scala a 5°/10°/15°; CC BY-SA 3.0/GFDL), «View from USS Growler Attack Periscope» (CC
    BY-SA 4.0).

  Tutte consultate.
- **Sovietici:** griglia a due scale e disco dei filtri (§4.3); nessuna foto.
- **TV in centrale:** monitor in bianco e nero (688, probabile); TV del pulto del comandante (Alfa).

### 6.2 Il posto del periscopio

- **688:**
  - pedana rialzata al centro della centrale, coi due periscopi;
  - il comandante coi gomiti sulle **due impugnature**: sinistra elevazione, destra ingrandimento (Type 2);
  - lo stadimetro in basso a destra, coi quadranti doppi sul retro per l'aiutante;
  - l'alzata la comanda un altro uomo, per accorciare l'esposizione `[USF25]`.
- **Foto** (pubblico dominio):
  - «US Navy 040820-N-6811L-013» (Portsmouth, SSN-707);
  - «US Navy 070318-N-3642E-031» (Alexandria);
  - «US Navy 100129-N-3560G-003» (Santa Fe, i due periscopi in uso);
  - «US Navy 990331-N-1556A-006» (Norfolk);
  - «980519-N-3149V-009 Periscope at Sea» (Tucson);
  - «USS PARGO Sturgeon Class» (1991).
- **Fuori:**
  - «USS Chicago (SSN-721) at periscope depth off Malaysia» e «Periscope Depth.jpg» (Key West), pubblico dominio;
  - «Periscope of USS Dallas and HMS Illustrious» e «HMS Talent Surfacing Drills», OGL v1.0, con attribuzione;
  - «Soviet Charlie II class submarine at periscope depth in 1986», pubblico dominio, preso da un S-3A.
- **Sovietici** (CC, consultate): «Submarine Periscope in B-396 Museum Front/Side» (CC BY-SA 3.0), «Periscope-varna1…4»
  (CC BY 3.0), «Typhoon class Schema.svg» (legenda coi ruoli dei periscopi).

---

## 7. Dottrina d'uso

**USA e NATO.**
- **Esposizioni brevi:**
  - in guerra «due o più esposizioni brevi sotto i 30 s sono meglio di una lunga» `[USF25]`;
  - ~10 s a guardata `[NWI]`; non oltre 15 s a 6× `[FTS]` (alta);
  - «sei o sette secondi» dalla guerra in poi (bassa).
- **Ordine:**
  - prima si **punta il periscopio sul rilevamento** e poi lo si alza;
  - **basso ingrandimento a giro d'orizzonte**, poi 6× sul bersaglio;
  - si abbassa subito dopo `[USF25]`.
- **Procedura moderna (NWP 3-13.10, dal rapporto NTSB)** (alta):
  1. appena la testa esce, **almeno tre giri di 360° a basso ingrandimento di ~8 s ciascuno** (~24 s): «nessun
     contatto vicino»;
  2. **ricerca aerea** dall'elevazione massima all'orizzonte: «nessun contatto aereo»;
  3. giro di 360° a basso ingrandimento di ~45 s, poi settore di 90° ad alto ingrandimento.

  - Ricerca completa ~3 minuti; settori ad alto ingrandimento ogni ~45 s.
  - Negli stessi 24 s, la **ricerca aurale ESM** (NWP 3-55.42).
  - Contatto vicino: ordine automatico di scendere a 150 ft o più, abbassare e immergersi `[GRIF]`.
- **Intervallo fra le guardate:**
  - in guerra ogni 3 minuti, per il plotting a mano `[USF25]`;
  - al Perisher britannico, «un giro che non vede niente ti tiene al sicuro per i prossimi dodici minuti»: è il tempo
    di una nave a 30 nodi dall'orizzonte; bisogna sempre tenere **un minuto** di margine per arrivare a quota sicura
    `[SD]`.
- **Attacco contro ricerca:** l'attacco vicino al bersaglio; la ricerca per la ricerca, l'ESM e le foto (al Perisher,
  le «perirecce» con la camera) `[SD]`.
- **Stadimetria e angolo sulla prua:**
  - lo stadimetro come al §2.1;
  - col reticolo (NWP 71-1-1): **R = K·MHH / divisioni verticali**, **sin(AOB) = (MHH · div. orizzontali / div.
    verticali) / lunghezza**;
  - lunghezze tipiche: fregata 400 ft, cacciatorpediniere 500, incrociatore 600 `[NSL-AOB]` (media).
- **Chiamate:**
  - «Bearing — mark» (l'aiutante legge le tre cifre);
  - «Range — mark» (si legge lo stadimetro, il tiro segna l'ora);
  - «Angle on the bow starboard 30»;
  - «No close contacts», «No airborne contacts»;
  - «Emergency deep»;
  - per alzare e abbassare, «Up periscope» / «Down periscope» (RN); «Up scope» / «Down scope» è colloquiale
    americano.

**URSS** `[SPR74]` (alta):
1. Prima della risalita, **giro col sonar passivo** a quota di sicurezza.
2. In risalita si alza **prima l'antenna dell'avvisatore radar a 16-18 m**, poi il periscopio a **12-14 m**.
3. Velocità «малый» (bassa), assetto zero.
4. Il comandante «guarda in fretta l'orizzonte e il cielo», col battello pronto ad allagare la cassa di rapida
   immersione.

- Con l'allarme di combattimento si sale con ~5° di assetto appoppato a velocità media.
- Col mare grosso, almeno media velocità e i mari al traverso.
- Rilevamento più distanza in ~10 s (bassa).

---

## 8. Proposta di gioco [GIOCO]

### 8.1 Il 688: due periscopi selezionabili

Un tasto sceglie il periscopio. Si alza uno alla volta, come davvero.

|  | **Type 2 (attacco)** | **Type 18 (ricerca)** |
|---|---|---|
| Ingrandimenti / campo | 1,5× (32°), 6× (8°) | 1,5× (32°), 6× (8°), **12× (4°)**; 24× (2°) solo come opzione «dopo il 1989» |
| Elevazione | fino a **+74,5°**, ricerca aerea in tre giri (14°, 44°, 74,5°) | fino a **+60°** |
| Distanza | **stadimetro a immagine sdoppiata** (l'attuale): galleggiamento sulla testa d'albero, quadrante, angolo sulla prua girando di 90° | stadimetro (lo stesso) |
| Notte | **niente** (solo l'occhio: di notte si vede quasi solo ciò che è illuminato) | **modo a bassa luminosità**: immagine verde o grigia granulosa, schiarita, rumore retinato (STIMA, forma n.v.) |
| TV e foto | no | **TV in bianco e nero** al monitor della centrale (la vista si vede anche dal pannello); **foto** (tasto): l'istantanea va nel registro M1 e diventa una minicartolina del debriefing |
| ESM | no: ESM solo dall'albero apposito | **sezione ESM nella testa**: ascolto radar mentre è alzato (la ricerca aurale dei 24 s) |
| Stabilizzazione | no: l'immagine trema con mare e velocità | su un asse: trema meno |
| Filtri | polaroid contro i riflessi; rosso, verde e giallo | gli stessi |
| Sezione radar (STIMA, 1 m esposto) | **0,2 m²** | **1,5 m²** |
| Piuma (STIMA) | sottile: metà di quella del Type 18 | piena |
| Alzata e discesa | **6 s** (STIMA) | **8 s** (STIMA) |

**Velocità** (STIMA da tarare):
- **fino a 6 nodi** si usa liberamente;
- **6-10 nodi**: l'immagine trema (il Type 18 meno) e la piuma diventa visibile;
- **10-15 nodi**: piuma enorme e immagine quasi inservibile;
- **oltre 15 nodi**: rischio di danno al periscopio (sistema «periscopio» nel kit dei danni).

**L'esposizione conta:**
- un **contatore dei secondi** di esposizione accanto alla scala dei rilevamenti;
- la probabilità di scoperta radar sale coi giri del radar nemico (modello del dossier 11 §5, con la sezione della
  testa e l'altezza esposta);
- col mare da 5 in su il nemico vede peggio.

**Aiuti al giocatore, nel modo moderno:** la sequenza NWP (tre giri da 8 s, aria, giro da 45 s) come lista di
controllo con le chiamate a voce o a messaggio: «NESSUN CONTATTO VICINO», «NESSUN CONTATTO AEREO».

**Grafica dell'oculare:**
- campo tondo con la vignettatura;
- croce sottile e tacche graduate (1° e 15′ a 6×, 4° e 1° a 1,5×);
- **niente numeri sovrapposti nella vista**: rilevamento, ingrandimento e secondi stanno fuori dal cerchio, sulla
  «cornice» della stazione, come i quadranti dell'aiutante.
- Lo stadimetro mostra l'**immagine fantasma** sovrapposta e spostata, come nel vero.

### 8.2 L'Alfa: «Сигнал» e TV

- **Un periscopio:** 1,5× e 6× (STIMA, come i sovietici di guerra), elevazione fino a +60° (STIMA).
- **Notte con l'intensificatore** (canale notturno, plausibile dal 1965; STIMA): immagine **verde giallina**
  granulosa, come gli schermi sovietici (scelta dell'utente).
- **Distanza col reticolo:** il giocatore sceglie l'altezza del bersaglio e conta le divisioni, oppure lo fa il
  calcolatore con un errore maggiore. Niente immagine sdoppiata: è meno preciso del 688.
- **TV-1:** la vista del periscopio sul **pulto del comandante** (stessa vista, monocroma, più grana), con i comandi
  a distanza. In più la **telecamera verticale della vela**, che guarda in alto: a quota bassa sotto il ghiaccio
  mostra il ghiaccio e la luce (STIMA, utile per l'emersione sotto i ghiacci).
- **ESM** non nel periscopio: alberi a parte (avvisatore radar, «Весло-П»).
- **Procedura sovietica** come aiuto: «antenna radar su a 16-18 m, periscopio a 12-14 m, velocità bassa».
- Sezione radar e piuma: come una testa universale, **1,0 m²** (STIMA).

### 8.3 I periscopi dell'IA

- **Che cosa usano** (dal §4.1):
  - 2a generazione (Victor III, Charlie II): periscopio più **TV МТ-70**, che di giorno e col tempo buono vede la
    superficie **fino a ~50 m di quota** (STIMA d'uso: un sovietico può guardare il cielo e la superficie senza
    esporsi);
  - 3a generazione (Akula, Oscar, Typhoon): «Лебедь» più «Сигнал-3»;
  - diesel (Kilo, Foxtrot): ПК-8,5 d'attacco più ПЗНГ-8 di navigazione;
  - USA e Regno Unito come il 688; la Trafalgar ha **intensificatore e termico** (CH84): **di notte vede meglio di
    tutti**.
- **Come vedono il giocatore:**
  - la portata visiva della nave o del battello nemico verso il periscopio del giocatore dipende da testa, altezza,
    piuma (velocità), sole, mare e notte;
  - di notte solo chi ha l'intensificatore o il termico, e a portate ridotte.
- **Come si espongono:**
  - guardate da ~10 s ogni **3-12 minuti** (USF 25 e Perisher; i sovietici col giro sonar prima);
  - l'avvisatore radar sempre su prima del periscopio (sovietici);
  - le loro teste sono visibili al **periscopio e al radar del giocatore** con le stesse regole.

### 8.4 Dati

Un file nuovo `data/motore/periscopi.toml`, nella pipeline. Ogni valore porta la fonte e la confidenza; le STIME
sono segnate.
- **Per modello:**
  - `ingrandimenti` e `campi`, `elevazione_max`;
  - `telemetro` («sdoppiato» o «reticolo»), `notte` («niente», «intensificatore», «tv», «termico»);
  - `tv`, `foto`, `esm`, `stabilizzato`;
  - `sezione_m2`, `piuma` (fattore), `alzata_s`, `velocita_libera_kn` e `velocita_danno_kn`;
  - `filtri`, `reticolo` (passi in gradi).
- **Per classe:** l'elenco dei modelli (`[classi.los_angeles] periscopi = ["type2", "type18"]`), le quote
  periscopiche (60 ft per il 688, 18 m per la Trafalgar) e il complesso TV con la sua quota massima (МТ-70: 50 m).

---

## 9. Fonti

**USA e Regno Unito.**
- `[PSM]` *Submarine Periscope Manual*, NavPers 16165 (1946), cap. 4 Type II:
  https://legacy.maritime.org/doc/fleetsub/pscope/index.php (chap4a, 4b, 4c, 4e, 4f)
- `[PAT]` brevetto Kollmorgen US 4,017,148 (1975): https://patents.google.com/patent/US4017148
- `[K18]` scheda Kollmorgen Type 18 (copia su una pagina di genealogia, offline; solo estratti): media-bassa
- `[NSL1]` `[NSL2]` Merrill, «Looking Around: A Short History of Submarine Periscopes», Submarine Review 2002, parti I e
  II: https://archive.navalsubleague.org/2002/looking-around-a-short-history-of-submarine-periscopes-part-ii
- `[NOVA-P]` PBS NOVA, periscopi dello Springfield: https://www.pbs.org/wgbh/nova/subsecrets/spriconlortper.html
- `[NTSB]` NTSB MAB-05/01 (Greeneville): https://www.ntsb.gov/investigations/AccidentReports/Reports/MAB0501.pdf
- `[GRIF]` deposizione dell'amm. Griffiths: https://www.pbs.org/newshour/nation/military-jan-june01-griffiths4
- `[USF25]` *Current Doctrine Submarines*, USF 25(A), 1944: http://www.ibiblio.org/hyperwar/USN/ref/SS-Doct/SS-Doct-4.html
- `[NWI]` Naval War Instructions: https://ibiblio.org/hyperwar/USN/ref/WarInst/WarInst-9.html
- `[FTS]` *The Fleet Type Submarine*, cap. 20: https://maritime.org/doc/fleetsub/chap20.php
- `[TFC]` *Submarine Torpedo Fire Control Manual*: https://legacy.maritime.org/doc/attack/index.php
- `[FY98]` FAS, bilancio FY98, «Periscope Monopulse DF»: https://irp.fas.org/budget/fy98_navy/0603562n.htm
- `[APL97]` Ousborne et al., JHU APL Technical Digest 18(1), 1997:
  https://secwww.jhuapl.edu/techdigest/content/techdigest/pdf/V18-N01/18-01-Ousbourn.pdf
- `[DTIC-PDR]` *A History of U.S. Navy Airborne and Shipboard Periscope Detection Radar*, ADA618106 (solo estratti)
- `[NSL-RAD]` Nisbett, Submarine Review 2000: https://archive.navalsubleague.org/2000/modem-radar-tech-on-sub-tactical-employment-lt-nisbett
- `[NSL-AOB]` Bornt, «Estimating Angle on the Bow», Submarine Review 1997:
  https://archive.navalsubleague.org/1997/estimating-angle-on-the-bow-lcdr-butch-bornt
- `[KOL]` *The History of Kollmorgen 1916-2016*: https://www.readkong.com/page/the-history-of-kollmorgen-1916-2016-100-years-of-innovation-2865491
- `[RNS]` rnsubs.co.uk, storia dei periscopi: https://rnsubs.co.uk/articles/development/scope.html; https://rnsubs.co.uk/index.php?PageID=1130
- `[SD]` Hennessy e Jinks, *The Silent Deep* (2015), estratto dell'editore:
  https://cdn.waterstones.com/images/1/3518/9781846145803.pdf
- `[NMVM]` https://www.nmvetsmemorial.org/submarine-periscope.html
- `[JTM]` https://www.jtmcdaniel.com/periscope.html (bassa)
- `[WIKI-637]` https://en.wikipedia.org/wiki/List_of_Sturgeon-class_submarines

**URSS.**
- `[APA]` Ю. В. Апальков, *Корабли ВМФ СССР*, t. I: https://tech.wikireading.ru/4830 , /4835 , /4842 , /4873 ,
  /4874 , /4877 , /4879 , /4881
- `[DS705]` deepstorm.ru, 705: https://www.deepstorm.ru/DeepStorm.files/45-92/nts/705/list.htm; 971: …/971/list.htm
- `[SOB]` Собакин, `[KOL705]` Коляда: https://tech.wikireading.ru/5514, /5520, /5523
- `[PAV]` Павлов, *Подводные лодки проекта 671*: https://military.wikireading.ru/17098
- `[RW-971]` `[RW-949]` ru.wikipedia, Pr. 971 e 949A
- `[MM]` `[MMH]` moremhod.info (solo estratti): ПЗНС del 1965, «Сигнал» del 1972
- `[SPR74]` *Справочник по управлению кораблем* (1974): https://flot.com/publications/books/shelf/conning/19.htm, /16.htm
- `[RW-OBN]` ru.wikipedia, «Обнаружение подводных лодок»
- `[KP]` https://kpopov.ru/military/d-2_42.htm
- `[RAN]` https://randewy.narod.ru/nk/periskop.html (bassa)
- `[RG]` rg.ru (intervista a Пешехонов): https://rg.ru/2024/09/06/segodnia-projdet-proshchanie-s-legendarnym-konstruktorom-podlodok-igorem-spasskim.html
- `[PAT-RU]` brevetto LOMO RU2138836: https://patents.google.com/patent/RU2138836C1/ru

**Foto:** elencate ai §6.1 e §6.2 con la licenza. Tutte **consultate** (riferimento per ridisegnare, niente
proiezione). Quelle OGL e CC andranno nei crediti se un'immagine ne derivasse.

---

## 10. Lacune

- **688:**
  - le lettere delle varianti nel 1989 (Type 2D/F? 18B/D?);
  - il lato dei due periscopi (fonti in disaccordo);
  - la forma del modo notte del Type 18 nel 1989 (intensificatore diretto o TV a bassa luminosità);
  - quale ricevitore servisse l'antenna ESM della testa;
  - i tempi d'alzata e la velocità massima col periscopio alzato.
- **Sturgeon:** il periscopio di ricerca prima del Type 18 (Friedman, *U.S. Submarines Since 1945*).
- **Trafalgar:** ingrandimenti, campi e teste di CH84 e CK34 (Jane's Underwater Warfare Systems 1989-90).
- **Sovietici:**
  - tutti i dati ottici dei modelli del dopoguerra;
  - la sigla ПЗНС;
  - il reticolo;
  - il «Сигнал» del 705 (da cercare su deepstorm.ru con la codifica giusta e nel manuale di ottica dell'ITMO).
- **Rischio:** una curva misurata della piuma con la velocità e una sezione radar misurata d'attacco contro ricerca.
