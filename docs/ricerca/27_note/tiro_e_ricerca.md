# 27 — Note: sistemi di tiro dei missili e ricerca del missile (688, 1989)

> Appunti di ricerca per il remake di *688 Attack Sub*. Completano `05_missili_e_contromisure.md` §1–§2 (sequenza di lancio,
> RBL/BOL, profilo, TASM, statistiche OUTLAW SHARK e Naval Gazing) e `19_consolle_688_alfa.md` §2.4 e §6.5 (aspetto
> delle consolle Mk 81 e del WCP). **Quello che è già lì non è ripetuto.**
> Compilato il 28/10/2026. Solo pagine web aperte; nessun file scaricato. I PDF trovati sono segnati «da chiedere» in §5.

**Convenzioni** (come nel doc 05 §0). **Confidenza alta:** documento ufficiale o due fonti serie concordi. **Media:** una
buona fonte secondaria. **Bassa:** fonte singola debole, un forum o un simulatore commerciale. *(estr.)* = letto solo
nell'estratto del motore di ricerca, perché la pagina risponde 403 o 402. **STIMA DI GIOCO** = nessun dato pubblico:
valore proposto e motivato, da trattare come parametro di bilanciamento.

---

## 0. Le scoperte in breve

1. **La catena dei comandi è a tre livelli**, e un brevetto della Marina la descrive a livello di segnali [PAT5551875]:
   - consolle di direzione del tiro (Mk 81 sul Mk 117), dove si impostano i dati;
   - **Weapons Launch Console (WLC)** in centrale, che dà l'ordine di fuoco, la tensione di sparo e la pressione differenziale;
   - **Tube Control Panel (TCP)** nel locale siluri, che manovra valvole e porte e restituisce lo stato dei tubi.

   Il «pulsante rosso» del Weapons Control Panel citato da NOVA sta su questa catena.
2. **Il Tomahawk entrò nel Mk 117, non in un sistema a parte.** Si decise di integrarlo nel Mk 117 aggiungendo **una
   consolle operatore in più**, con un processore nella metà inferiore per rinfrescare lo schermo e per il puntamento oltre
   l'orizzonte (OTH) [FOX01] *(estr.)*. Il CCS Mk 1 è l'«All Digital Attack Center» che sostituì il Mk 117 da cui derivava
   [WP688].
3. **Il menu dell'Harpoon Block 1 è documentato.** Ci sono tre modi:
   - **RBL**, con lo schema di ricerca **piccolo, medio o grande**;
   - **BOL**, con l'opzione **MRS** (*maximum range search*, ricerca fino alla portata massima);
   - **LOS**, senza dati.

   Si impostano anche l'istante o la distanza d'accensione del seeker e il modo d'attacco [PAT6763289] [DSTO97] *(estr.)*.
4. **La regola di aggancio è «il primo bersaglio incontrato».** In LOS il missile attacca «il primo bersaglio incontrato
   con le impostazioni di default» [PAT6763289]. Nel tiro navale il seeker attaccava **il primo bersaglio trovato sul
   rilevamento assegnato oltre la distanza di accensione** [NAVS] *(estr.)*. Il seeker è un **radar a ricerca
   sequenziale** [SBIR10] *(estr.)*: non c'è classificazione, conta il primo eco valido nel cancello.
5. **Il BOL allarga la ricerca col tempo.** Dopo l'istante d'inizio impostato dall'operatore, **ampiezza in azimut e
   profondità in distanza crescono gradualmente**, finché il missile aggancia o raggiunge la portata massima [USNI77]
   *(estr.)*. Senza bersaglio si autodistrugge [DS84].
6. **I tre schemi RBL valgono 10, 20 e 30 km:** a tanto dal punto previsto il missile comincia a cercare (SML, MED, LRG).
   Il dato viene da un simulatore commerciale dell'AGM-84D [VRS] (bassa-media), con la stessa logica raccomandata per la
   scelta: piccolo se ci sono altre navi vicine, grande se il bersaglio è veloce e lontano.
7. **Probabilmente il Block 1C non era sfruttabile da un 688 nel 1989.** Il programma CCS Mk 2 «D0», degli anni '90,
   «introduce» le capacità Harpoon Block 1C nel controllo di combattimento dei sottomarini [FAS98RDT]. Nel 1989 si
   tirava quindi l'UGM-84A con pop-up o l'UGM-84C radente, senza scelta del terminale (deduzione, bassa-media).
8. **Gli interblocchi dei tubi sono pubblici** [PAT5551875]:
   - la porta esterna non si apre se la culatta non è bloccata;
   - **in ogni gruppo di tubi una sola slide valve può essere aperta alla volta**: è il motivo fisico dell'intervallo fra
     i lanci di una salva;
   - dopo lo sparo la porta resta bloccata finché il pistone non ha finito la corsa.
9. **Il TASM vola basso e sale a una quota di ricerca a una distanza programmata**, poi cerca col radar attivo [LOC92]
   *(estr.)*. La ricerca è a serpentina con ESM passivo [DS109], a «quadrati che si allargano» [HGF] (bassa). Gli schemi
   di ricerca a più passate furono studiati da APL e McDonnell Douglas dal 1974 [LOC92] *(estr.)* [NSLOS].
10. **Il comandante è responsabile del tiro del Tomahawk**, e la distanza d'accensione del radar terminale è un «cancello
    in distanza» impostato dalla direzione del tiro [NSL85].

---

## 1. Sistemi di tiro e consolle

### 1.1 Quadro per scafo (1989)

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| 688 Flight I (SSN-688…718) | **Mk 117 FCS** su calcolatore UYK-7; le consolle digitali **Mk 81 sono «weapon control consoles»** (WCC); il Mk 117 tolse le due conversioni analogiche del Mk 113/Mk 75. Primo 688 nato col Mk 117: Dallas (SSN-700) | [WP688] [NSLCCS] | media-alta |
| Integrazione Harpoon | «Il Mk 117 digitale integra pienamente l'Harpoon» | [DS84] | media |
| Integrazione Tomahawk sul Flight I | Si discusse fra un aggiornamento del Mk 117 e un sistema Tomahawk autonomo; **vinse l'integrazione nel Mk 117**, con **una consolle operatore aggiuntiva** e un processore nella sua metà inferiore (refresh dello schermo, puntamento OTH). In cambio della rinuncia al siluro nucleare dal SSN-700 in poi, l'ufficio del programma 688 si impegnò a dotare di Tomahawk **cinque** 688 | [FOX01] *(estr.)* | media |
| 688 Flight II con VLS (SSN-719…725, 750) | **CCS Mk 1** = «Mark 1 Combat Control System / All Digital Attack Center», derivato dal Mk 117 e che lo sostituisce, con UYK-7 e UYK-44 (Lockheed Martin). Software C4.x (C4.1 al primo lancio VLS; **CCS Mk 1 Mod 3 con C4.2** provato nel dicembre 1990) | [WP688] [NSLCCS] [FASTWCS] [WPProv] | media-alta |
| 688I (SSN-751+, 1988-89) | **AN/BSY-1**: integra sonar e controllo di combattimento. Il sottosistema di controllo di combattimento fa **impostazione e controllo di armi e mine, puntamento, gestione del sistema, pilotaggio e navigazione**, con le apparecchiature di lancio per i tubi orizzontali. Conserva il codice software del CCS Mk 1 (doc 18) | [FASBSY1] [NSLCCS] | media-alta |
| Equivalente sottomarino del TWCS | **Non c'è un AN/SWG-2/-3 sui sottomarini.** Pianificazione dell'ingaggio, inizializzazione e controllo di lancio del Tomahawk li fa il **Combat Control System** (sottomarini) o il **TWCS** (navi) | [FASTom] [WPTom] | alta |
| TWCS di superficie (per confronto) | Il TWCS v2 del 1983 era detto «green screens» (schermi verdi). AN/SWG-3A sulla pagina FAS; OPEVAL su un Spruance nel 1983–85 | [WPTom] [FASTWCS] | media |
| Harpoon di superficie (per confronto) | **AN/SWG-1(V) HSCLCS** (Harpoon Ship Command Launch Control System): prepara e lancia; l'SWG-1A aggiunge grafica e **pianificazione automatica dell'ingaggio**. Dalla consolle Harpoon nel CIC si programmava il **rilevamento di volo** e, a scelta, una **distanza minima o prevista** | [FASSWG1] [NAVS] *(estr.)* | media |
| Sigla EHCLS | *Encapsulated Harpoon Command and Launch System*: esiste come voce in un elenco giapponese dei sistemi di controllo Harpoon. Forse è il kit dei battelli senza Mk 117 (637 col Mk 113) | [MYNAVI] *(estr.)* | bassa |

**Nota sui due livelli di consolle.** Per il Tomahawk, la Submarine Review del 1985 distingue due cose
[NSL85]:
- il **controllo del tiro** (lancio e volo), di cui risponde il comandante del SSN;
- il **controllo dell'impiego** (chi decide il bersaglio).

Per il gioco: le Mk 81 sono le consolle delle soluzioni e delle preimpostazioni; la WLC (o Weapons Control Panel) è quella
dello sparo.

### 1.2 Catena consolle → tubo (dal brevetto del simulatore di tubi, scafi SSN-751)

| Elemento | Funzione e segnali | Fonte | Conf. |
|---|---|---|---|
| **Weapons Launch Console (WLC)** | Interfaccia fra la direzione del tiro e il sistema di lancio. Verso il **TCP** per i tubi lanciasiluri: **ordine di fuoco, tensione di sparo, pressione differenziale del missile**. Per i **tubi missile (VLS)**: ordini di fuoco. Dal TCP riceve lo stato dei tubi | [PAT5551875] | alta (brevetto USN), solo 688I |
| **Tube Control Panel (TCP)** | Due segmenti, sinistro e destro, con strumenti digitali, manometri, LED con targhette e interruttori. Allagamento, pressurizzazione, controllo dello stato dell'arma, drenaggio, ripristino | [PAT5551875] | alta |
| Valvole del tubo (con finecorsa aperto/chiuso) | **Flood/Drain, Vent, Blow, Breech Ring, Equalize, Muzzle Door, Slide Valve, Ejection Pump Door**; sensori di allagamento e drenaggio (115 V c.a. quando sono immersi) | [PAT5551875] | alta |
| Tubo VLS | Portello, interruttore «Hatch Blocked», interruttore «Valve Unblocked», solenoidi PVC | [PAT5551875] | alta |
| Trainer NAVAIR | *688 2nd Flight Weapons Launch Console* e *688i WLC*: l'operatore svolge le **procedure di pre-lancio, arma l'arma e la lancia** con valvole, interruttori, leve, WLC, sistema di lancio orizzontale (HLS) e verticale (VLS). La versione Flight II ha **più comandi analogici** della 688i | [NAWC] *(estr.)* | media |
| Consolle in centrale | Fila di consolle d'arma a dritta; quella **più a poppavia è il Weapons Control Panel**, col pulsante rosso che lancia siluro o Tomahawk **«una volta soddisfatti i prerequisiti»** (già nel doc 19) | [NOVA-C] | media |

### 1.3 Che cosa vede e imposta l'operatore per l'Harpoon

Dati di inizializzazione documentati (lato aereo e navale, con lo stesso missile Block 1):

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Modo di lancio | **RBL**, **BOL**, **LOS** | [PAT6763289] [DSTO97] *(estr.)* | alta (due fonti concordi) |
| Dati RBL | Distanza e rilevamento del bersaglio, **opzione d'attacco**, **schema di ricerca piccolo, medio o grande**; più assetto, quota e velocità della piattaforma (sul sottomarino: rotta e assetto dall'inerziale) | [PAT6763289] | alta |
| Dati BOL | Solo il rilevamento (distanza ignota), **opzione d'attacco e schema di ricerca BOL**; sul lato navale l'opzione **MRS** | [PAT6763289] [DSTO97] *(estr.)* | alta |
| LOS | Nessun dato, bus inattivo; il missile tiene la rotta di lancio e **attacca il primo bersaglio incontrato con le impostazioni di default** | [PAT6763289] | alta (per l'aereo); sul 688 non documentato |
| Accensione del seeker | Parametro di missione («seeker turn-on time»); dopo l'accensione il simulatore DSTO disegna **l'area di ricerca** sulla traccia | [DSTO97] *(estr.)* | media |
| Nel BOL | L'operatore inserisce **l'istante in cui il seeker comincia la ricerca** | [USNI77] *(estr.)* | media |
| Campi del simulatore AGM-84D (analogia) | MODE (RBL/BOL), FLT (profilo), TERM (SKIM/POP), SEEK (SML/MED/LRG, solo RBL), SRCH (distanza d'inizio ricerca, 0–105 nmi), DSTR (distanza di autodistruzione, da SRCH a 172 nmi), BRG; avvisi «fuori zona» (OFF AXIS oltre 90°, INV TGT, SCH/DSTR troppo vicini) | [VRS] | bassa-media (simulatore) |
| Terminale | Block 1C selezionabile, ma sui sottomarini l'integrazione 1C arriva col CCS Mk 2 «D0» | [DS84] [FAS98RDT] | media (deduzione per il 1989) |
| Waypoint | Solo navi con SWG-1A; non sul 688 del 1989 (già nel doc 05) | [DS84] | media |

**Che cosa NON si trova in fonti aperte:** la pagina-schermo della Mk 81 per l'Harpoon, la disposizione dei comandi della
WLC e la sigla delle lampade. Va disegnata come ricostruzione (vedi §4).

### 1.4 Che cosa imposta l'operatore per il TASM e il TLAM

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Funzioni a bordo | Pianificazione dell'ingaggio, inizializzazione del missile, controllo del lancio (CCS) | [FASTom] | alta |
| Dati OTH | All'inizio i dati del bersaglio (posizione e direzione di moto) **si inserivano a mano** nella direzione del tiro; poi Mk 117 e CCS Mk 1 li accettarono direttamente. La direzione del tiro «formulava gli ordini di fuoco e li inviava al missile nel tubo» | [NSLOS] | media-alta (NSL, 2012) |
| Schemi di ricerca | Il calcolatore di bordo «correlava i dati d'intelligence con la propria posizione e i contatti» per **preparare gli schemi di ricerca** | [NSLOS] | media |
| Responsabilità | Per il TASM «la nave controllava tutto il puntamento e pianificava l'intera missione antinave» | [NSLOS] | media |
| Cancello d'accensione | Una «range gate» per attivare il radar di guida terminale; contro un bersaglio che manovra si mira **al punto stimato all'arrivo** del missile; in alternativa si vola con il radar acceso «tutto il tempo», a costo della sorpresa | [NSL85] | media |
| Pianificazione anti-mercantili | La piattaforma pianificava il volo per vedere il bersaglio e non un mercantile innocente (già nel doc 05) | [NG2] | media |

---

## 2. Sequenza, tempi, interblocchi

### 2.1 Tempi documentati e stime

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Riscaldamento del seeker Harpoon | **25 s** dall'alimentazione di standby (lato aereo, AGM-84D) | [VRS] | bassa-media (analogia) |
| Riferimento d'assetto | Strapdown a 3 assi (ARA) più calcolatore e alimentatore (DC/PS); **il DC/PS viene inizializzato dal sistema di comando e lancio prima del lancio**. Nessun tempo pubblicato | [FAS84] [DS84] | media |
| Tomahawk dal sottomarino | Circa **1 h** di preparazione con la missione già a bordo (già nel doc 05); comprende accensione, **allineamento inerziale** e caricamento della missione | [GAO95] [FASCMSA] | alta / media |
| Ritmo dei lanciatori di superficie | Mk 140 e Mk 141: un missile ogni 2 s (navi, solo per confronto) | [CSIS] *(estr.)* | bassa |
| **STIMA DI GIOCO: Harpoon, dall'ordine «missile pronto» alla capsula in tubo** | Alimentazione e riscaldamento: **30 s**. Trasferimento d'assetto e rotta dall'inerziale (DMINS o WSN-3): **20–40 s**. Inserimento dei dati di tiro RBL o BOL: **30–60 s**, in parallelo. Totale **~60–90 s** dopo che la soluzione è pronta | analogia con [VRS] e [FAS84] | — |
| **STIMA DI GIOCO: TASM** | Preriscaldamento e allineamento inerziale **8–12 min** (i giroscopi del Tomahawk devono allinearsi molto meglio di quelli dell'Harpoon); preparazione della missione di ricerca dai dati OTH **5–15 min**; totale sotto l'ora del TLAM perché non serve una missione TERCOM | ragionamento su [GAO95] [NSLOS] | — |
| **STIMA DI GIOCO: TLAM** | **45–60 min** con la missione a bordo; la missione dev'essere nel carico (scelta prima della partenza o ricevuta via radio) | [GAO95] | — |
| Preparazione del tubo | Allagare, equalizzare, aprire la porta (già nel doc 05: 60–120 s, STIMA) | — | — |
| Intervallo fra i tubi di una salva | **Vincolo fisico:** una sola slide valve aperta per gruppo di tubi [PAT5551875]. I 688 hanno 2 tubi per lato: si possono lanciare a breve distanza un tubo di sinistra e uno di dritta, mentre il secondo tubo dello stesso lato aspetta che il primo chiuda la slide valve. **STIMA:** 3–5 s fra due tubi di lati opposti, **15–25 s** fra due tubi dello stesso lato | [PAT5551875] + STIMA | alta (vincolo) / STIMA (tempi) |

### 2.2 Interblocchi e condizioni

| Interblocco o vincolo | Valore | Fonte | Conf. |
|---|---|---|---|
| Porta esterna | Non si apre se l'anello di culatta non è bloccato | [PAT5551875] | alta |
| Equalizzazione | La valvola di equalizzazione porta il tubo allagato alla pressione del mare, condizione per aprire la porta | [PAT5551875] | alta |
| Sparo | La «Firing Solenoid Voltage» rilascia l'aria di sparo a 400 psi, che ruota lo stopbolt e blocca la porta; se la slide valve è aperta, l'aria d'impulso a 2.000 psi muove il pistone (lancio a espulsione). Col lancio *swim-out* la slide valve resta chiusa | [PAT5551875] | alta |
| Dopo lo sparo | La porta resta bloccata finché il pistone non ha finito la corsa o non scatta il ritorno in batteria | [PAT5551875] | alta |
| Porta della pompa d'espulsione | Si chiude senza tensione del solenoide, salvo durante una sequenza di sparo | [PAT5551875] | alta |
| Capsula Harpoon | Espulsa (non *swim-out*), quindi serve la slide valve | ragionamento su [USNI77] [FAS84] | media |
| Quota di lancio | La configurazione idrodinamica della capsula **consente il lancio da quota periscopica**, grazie alla virata rapida verso l'alto; il lancio dal ponte richiede più quota. **La quota di sparo non è limitata dal missile**, perché la capsula regge le pressioni del sottomarino. Uscita dall'acqua **≥ 45°**, coda fuori dall'acqua **12–15 ft** | [PAT4033225] | alta (brevetto USN, 1977) |
| Velocità del battello | Le tattiche correnti per i siluri prevedono il lancio a **5–10 kn**; a 25 kn le forze di lancio crescono in modo esponenziale | [USNI97] *(estr.)* | media |
| Esposizione | La pubblicità MBDA sostiene che, al contrario del «concorrente principale», con l'SM39 il battello non deve salire a quota periscopica | [ADU] *(estr.)* | bassa (marketing; contraddetta da [PAT4033225], che parla di possibilità, non di obbligo) |
| Consenso del comandante | Per il Tomahawk «il comandante di un SSN assicura il controllo del tiro». Nessuna fonte aperta su chiavi di sparo per le armi tattiche dei 688. Per gli SSBN Polaris le fonti descrivono l'interruttore di sparo tolto dalla cassaforte e l'autorizzazione del comandante | [NSL85] [IWM] *(estr.)* | media / bassa |
| Prerequisiti | NOVA dice solo «una volta soddisfatti i prerequisiti» | [NOVA-C] | media |

**STIMA DI GIOCO: prerequisiti dello sparo** (lampade sulla WLC, tutte verdi per abilitare il pulsante coperto):
1. ARMA PRONTA (missile alimentato, riscaldato, allineato, dati accettati);
2. TUBO ALLAGATO;
3. EQUALIZZATO;
4. PORTA APERTA;
5. SLIDE VALVE APERTA (una per lato);
6. FUOCO ABILITATO: il consenso del comandante, a voce o con un interruttore, che il giocatore dà come comandante;
7. quota ≤ 150 ft e velocità ≤ 10 kn (sopra 10 kn soltanto con un avviso e una penalità; vedi doc 05 §1.3).

---

## 3. Ricerca del missile

### 3.1 Il seeker AN/DSQ-28 (Harpoon Block 1 e TASM)

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Tipo | Radar attivo **monopulse, agile in frequenza, a due assi** (Texas Instruments), banda J | [USNI77] *(estr.)* [DS84] [WTF] | alta (concordi) |
| Funzionamento | **Radar a ricerca sequenziale**: esplora l'area d'incertezza e insegue l'eco che giudica bersaglio. Per migliorare la scelta dei bersagli in mare affollato furono studiati algoritmi *track-while-scan*, che avrebbero richiesto grosse modifiche | [SBIR10] *(estr.)* | media |
| Limiti di puntamento | ± 45° | [WTF] (forum) e il settore BOL di [DS84] | bassa-media |
| Potenza | 35 kW di picco, 684 W medi | [WTF] (forum, fonte non indicata) | bassa |
| Stato del mare | Acquisizione e inseguimento efficaci **fino al mare forza 5** | [WTF] (cita le audizioni DoD Appropriations 1974) | bassa-media |
| Discriminazione | Il Block 1C ha difficoltà con navi vicine fra loro, traffico mercantile ed echi della costa; non ha le funzioni di scelta del bersaglio del Block II+ | [AR26] *(estr.)* | media |
| **Portata d'acquisizione** | **Non pubblicata.** È limitata dall'orizzonte radar: missile a 5–15 m, bersaglio con la sovrastruttura a 15–30 m: d ≈ 4,12 (√h_m + √h_t) km | calcolo | — |
| **STIMA DI GIOCO: portata d'acquisizione** | Motovedetta o corvetta piccola: **10–12 km**. Fregata o caccia: **15–20 km**. Incrociatore o Kirov: **20–25 km**. Mercantile grande: **20–25 km** (ha un eco grande e alto: è il problema). Con mare forza 6+: −30% e probabilità di perdere l'aggancio | calcolo dell'orizzonte con un margine di potenza | — |

### 3.2 Harpoon RBL: geometria

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Logica | Vola al rilevamento comandato fino a una distanza pari a quella comandata, poi esegue la ricerca comandata e l'attacco comandato | [PAT6763289] | alta |
| Taglie | **Piccola, media e grande** (SML, MED, LRG) | [PAT6763289] [DSTO97] *(estr.)* [VRS] | alta |
| Inizio della ricerca | SML **10 km**, MED **20 km**, LRG **30 km** prima del punto bersaglio designato | [VRS] | bassa-media (simulatore AGM-84D, valori coerenti col doc 05) |
| Scelta della taglia | Piccola o media con **altre navi vicine** (discriminazione migliore); grande o media per un bersaglio **veloce e lontano** | [VRS] | bassa-media (concorda con la logica di [DS84]) |
| Regola d'aggancio | Primo eco valido nel cancello (vedi §3.4) | [NAVS] *(estr.)* [PAT6763289] [SBIR10] *(estr.)* | media |
| Larghezza del cancello in azimut e profondità in distanza | **Non pubblicate** | — | — |
| **STIMA DI GIOCO: «scatola» RBL** | Un **rettangolo centrato sul punto previsto**, orientato lungo la rotta del missile. Profondità in distanza = **2 × la distanza d'inizio ricerca** (SML 20 km, MED 40 km, LRG 60 km). Semilarghezza trasversale: **SML ±3 km, MED ±6 km, LRG ±10 km**. Il seeker esplora a settore (±45° al massimo) ma **accetta solo gli echi dentro la scatola**. Il rettangolo cresce di 1,5 km per ogni minuto di volo oltre i 4 min, come l'incertezza di un bersaglio a 20 kn | ragionamento: ±45° a 10 km coprirebbero ±10 km, e la «discriminazione migliore» della taglia piccola implica un cancello più stretto | — |

### 3.3 Harpoon BOL: geometria

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Accensione | All'istante o alla distanza impostata dall'operatore (SRCH) | [USNI77] *(estr.)* [VRS] | media |
| Settore | ± 45° davanti alla rotta | [DS84] | media |
| Evoluzione | **Ampiezza in azimut e profondità in distanza crescono gradualmente** fino all'aggancio o alla portata massima | [USNI77] *(estr.)* | media |
| Fine ricerca | Distanza di autodistruzione (DSTR) o portata massima (opzione MRS); poi **autodistruzione** | [VRS] [DSTO97] *(estr.)* [DS84] | media |
| Variante successiva | Block 1D, xGM-84F: rifà l'attacco con uno schema **a quadrifoglio** fino all'esaurimento del carburante (**non** nel 1989: è un programma degli anni '90, poi cancellato) | [DS84] | media |
| **STIMA DI GIOCO: BOL** | All'accensione: cancello **±15°** in azimut e **5 km** di profondità davanti al missile. Ogni 10 s di volo cresce di **+7,5°** e **+2,5 km**, fino a **±45° e 20 km** (dopo ~40 s). La ricerca continua fino a DSTR o all'esaurimento: con 5 min di volo rimasti, ~70 km. Nessun quadrifoglio | forma da [USNI77], valori calibrati sulle portate di §3.1 | — |

### 3.4 Regole d'aggancio: perché si colpisce la nave sbagliata

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Harpoon | **Il primo bersaglio trovato sul rilevamento assegnato oltre la distanza d'accensione**. In LOS: «il primo bersaglio incontrato» | [NAVS] *(estr.)* [PAT6763289] | media (due fonti indipendenti, una sola letta) |
| Effetto | In mare affollato il missile prende **il primo eco del cancello**: una scorta più vicina, un mercantile, la costa. Per questo si sceglie la taglia piccola con altre navi vicine | [VRS] [AR26] *(estr.)* | media |
| Esempio recente | Nel 2020 la fregata iraniana Jamaran colpì la nave d'appoggio Konarak, rimasta **troppo vicina** al bersaglio d'esercitazione, con un Noor (C-802) | [MT20] *(estr.)* | alta (fatto); è un'analogia, non riguarda l'Harpoon |
| TASM: ESM | Ricevitore ESM regolato sulle **emissioni radar caratteristiche del bersaglio voluto**, forse cablato sui radar d'altezza delle *Moskva* | [NG2] | media / bassa per *Moskva* |
| TASM: PI/DF | «Passive Identification/Direction Finding»: doveva dirigere il TASM sulle navi maggiori, «pare» captandone i grandi radar di scoperta aerea | [HGF] | bassa |
| TASM: niente IFF | Il seeker non aveva l'identificazione amico-nemico: problemi di regole d'ingaggio | [HGF] | bassa |
| TASM: bilancio dei 139 tiri simulati | **17** sul bersaglio designato (12%), **53** su altre navi nemiche (38%), **8** su neutrali (6%); i **61** restanti (44%) non sono attribuiti (persi, abbattuti o guasti) | [NG2] (e il conto) | media |

### 3.5 TASM: profilo e ricerca

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Profilo | Crociera bassa; **a una distanza programmata sale a quota di ricerca** e cerca col radar attivo | [LOC92] *(estr.)* | media |
| Quota di ricerca | ~**100 m**, poi ridiscende dopo l'aggancio, evade, riaggancia nel terminale e fa la manovra finale | [HGF] | bassa |
| Schema | **Serpentina** con radar passivo (emissioni nemiche) e attivo per l'aggancio; dopo l'aggancio, brevi pop-up per una posizione migliore | [DS109] | media |
| Schema (altra descrizione) | Serie di **«expanding box»** (quadrati che si allargano), a serpentina, per coprire tutta l'area d'incertezza | [HGF] | bassa |
| Origine | Dall'autunno 1974 APL e McDonnell Douglas studiarono l'uso della portata in più per **ricerche a più passate** («multipass»); APL «sviluppò gli schemi di ricerca» nel 1975 | [LOC92] *(estr.)* [NSLOS] | media |
| Prova | Primo volo antinave: **175 nmi** verso il bersaglio, poi **173 nmi** di ricerca, bersaglio trovato a **240 nmi** dal lancio, senza collegamento dati (già nel doc 05) | [NSLOS] | media |
| Portata | Pianificata in 140 mi, portata oltre 300 nmi dopo il missile sovietico da 250 mi; efficace ~250 nmi | [NSLOS] [NG2] | media |
| Il problema | Il seeker è dell'Harpoon, che fatica a discriminare a grande distanza, e i sensori della Marina avevano meno portata del missile | [WPTom] | media |
| Limite OTH (1981) | Rilevamento, classificazione e puntamento OTH «seriamente limitati»; debolissima l'identificazione delle navi; navi e sottomarini non attrezzati da soli | [GAO81] | alta (abstract GAO) |
| Freschezza dei dati | Nei '70 e primi '80 i rapporti di contatto si aggiornavano **una o al massimo due volte l'ora**; la consegna al tiratore richiedeva **≥ 20 min**, scesi poi a secondi | [NSLOS2] | media |
| **STIMA DI GIOCO: TASM** | Crociera a 15 m fino a **40 km** dal punto previsto; salita a 100 m in 15 s; ESM acceso dalla salita. Seeker attivo: portata **35 km** su un incrociatore, **25 km** su un caccia, **30 km** su un mercantile grande. Schema: gambe **perpendicolari** alla linea di tiro, lunghe **40 km** e spaziate di **25 km**, che avanzano verso l'area; una spirale a quadrati che si allargano se l'area è un cerchio. Durata limitata dal carburante: fino a **300 km di ricerca** se il bersaglio era a 175 nmi | proporzioni da [NSLOS] e calcolo dell'orizzonte da 100 m (≈ 62 km geometrici, ridotti per la potenza) | — |
| **STIMA DI GIOCO: regola TASM** | Se l'ESM capta un emettitore della classe programmata (radar di scoperta aerea dei grandi: Top Sail, Top Steer, Head Net), il missile vira sul rilevamento ESM e accende l'attivo; aggancia l'eco più vicino a quel rilevamento entro ±5°. Senza emissioni (EMCON): **primo eco grande** nel settore, mercantili compresi | [NG2] [DS109] [HGF] | — |

---

## 4. Proposte di gioco

**In 4 righe.** Il giocatore imposta l'Harpoon su una schermata Mk 81 con quattro campi:
- MODO (RBL o BOL);
- RICERCA (PICCOLA, MEDIA o GRANDE in RBL; SRCH e DSTR in BOL);
- ACCENSIONE DEL SEEKER (distanza o tempo);
- RILEVAMENTO e DISTANZA.

Sulla carta compare la «scatola» di ricerca prevista. Dopo il lancio il missile aggancia **il primo eco valido nel cancello**,
senza riconoscere chi colpisce. Una scatola stretta e un'accensione tarda proteggono i neutrali ma rischiano il nulla; una
scatola larga trova sempre qualcosa, non sempre la nave giusta. È la stessa scelta che facevano davvero gli operatori.

Dettagli:
1. **Schermata Mk 81 «HARPOON SETUP»** (fosforo verde, carattere 5×7 del doc 19). Quattro righe di parametri regolati con
   gli encoder: `MODE RBL`, `SRCH MED`, `SKR ON 20 KM`, `BRG 047 RNG 38.5 KM`. Riga di stato: `PWR` → `WARMUP 25` (conto alla
   rovescia) → `ALIGN` → `MSL RDY`. Nel 1989 manca il campo `TERM`: il terminale dipende dal missile caricato (A pop-up, C
   radente).
2. **Pannello WLC**: sette lampade di prerequisito (§2.2) per tubo e il pulsante rosso coperto, abilitato solo con tutte
   verdi. La slide valve unica per lato rende naturale l'intervallo della salva: il gioco la mostra e non la nasconde.
3. **Carta (F6)**: dopo il lancio la scatola RBL, o il cono BOL che si allarga, si disegna in tratteggio sulla rotta del
   missile. All'accensione del seeker compare un cerchio rosso; al momento dell'aggancio una linea sottile verso il
   contatto agganciato, **senza nome** se il contatto non è classificato.
4. **Esito narrato** nel debriefing (stile F-19, doc «briefing»): «Harpoon tubo 2: aggancio su eco grande a 17 km, 3° a
   sinistra del centro. Bersaglio colpito: *mercantile panamense*». Per il TASM la stessa logica, più l'ESM. Il bilancio
   storico dei 139 tiri fa da riferimento di taratura per una scatola grande in mare affollato: colpo sul bersaglio
   designato ~10–20%.
5. **IA simmetrica** (regola del progetto): un Charlie o un Oscar sovietico obbedisce alla stessa regola del primo eco e
   può colpire un mercantile o una scorta al posto della portaerei.
6. **Modo «museo»**: una nota storica sotto la schermata spiega che i parametri reali erano classificati e che la scatola
   è una ricostruzione.

Parametri consigliati (riassunto delle STIME di §2–§3):

| Parametro | SML | MED | LRG |
|---|---|---|---|
| Inizio ricerca prima del punto previsto | 10 km | 20 km | 30 km |
| Profondità della scatola | 20 km | 40 km | 60 km |
| Semilarghezza della scatola | ±3 km | ±6 km | ±10 km |
| Uso consigliato | bersaglio in un gruppo o vicino a costa o traffico | caso normale | bersaglio veloce, dati vecchi (> 10 min) |

---

## 5. Fonti (consultate il 28/10/2026)

Sigle già definite nel doc 05 §8 e riusate qui: [DS84], [DS109], [USNI77], [NSLOS], [NSLCCS], [NG2], [GAO95], [FASCMSA],
[FASTom], [WPTom], [WP688], [WPProv], [PAT6763289]. Sigla del doc 19: [NOVA-C].

| Sigla | Titolo | URL | Note |
|---|---|---|---|
| [PAT5551875] | US 5551875, *Land based submarine weapons system simulator with control panel tester and trainer* (USN, scafi SSN-751) | https://patents.google.com/patent/US5551875 | letto |
| [PAT6763289] | US 6763289 (modi RBL, BOL, LOS, scelta piccola/media/grande, «primo bersaglio incontrato») | https://patents.google.com/patent/US6763289B2/en | letto (HTML) |
| [PAT4033225] | US 4033225, *Hydrodynamic configuration to be used on underwater launched, unpropelled bodies* (USN, 1977) | https://patents.google.com/patent/US4033225A/en | letto |
| [PAT4288049] | US 4288049, *Remote targeting system for guided missiles* (USN, depositato 1971) | https://patents.google.com/patent/US4288049A/en | letto; fonti d'errore del puntamento; non riguarda l'Harpoon |
| [FOX01] | R.F. Fox, «Build It and They Will Come», USNI *Proceedings*, aprile 2001 | https://www.usni.org/magazines/proceedings/2001/april/build-it-and-they-will-come | 403: estratto |
| [NSL85] | W.J.R., «C2T of the Antiship Tomahawk Missile», *Submarine Review*, gennaio 1985 | https://archive.navalsubleague.org/?p=912 | letto (OCR) |
| [NSLOS] | J. Holland, «OUTLAW SHARK 1», *Submarine Review*, 2012 | https://archive.navalsubleague.org/?p=5196 | letto |
| [NSLOS2] | R.L. Hess, J. Holland, «Before and After OUTLAW SHARK», *Submarine Review*, estate 2012 | https://archive.navalsubleague.org/?p=5205 | letto |
| [NSLCCS] | D.A. Curran, «A Brief History of the Push-Pull of Submarine Combat Control Systems», *Submarine Review*, gennaio 1993 | https://archive.navalsubleague.org/?p=2022 | letto |
| [NSLULM] | D.M. Ulmer, «After SUBACS?», *Submarine Review*, ottobre 1983 | https://archive.navalsubleague.org/?p=1002 | letto (solo il contesto Mk 75 e Mk 117) |
| [LOC92] | W.M. Locke, «Tomahawk Tactics—The Midway Connection», USNI *Proceedings*, giugno 1992 | https://www.usni.org/magazines/proceedings/1992/june/tomahawk-tactics-midway-connection | 403: estratto |
| [USNI77] | M. Hewish, «Harpoon», USNI *Proceedings*, febbraio 1977 | https://www.usni.org/magazines/proceedings/1977/february/harpoon | 403: estratto |
| [USNI97] | «Stern-Fired Torpedoes: Engineering and Tactical Approach», USNI *Proceedings*, luglio 1997 | https://www.usni.org/magazines/proceedings/1997/july/stern-fired-torpedoes-engineering-and-tactical-approach | estratto |
| [HURA85] | M. Hura, D. Miller, «Cruise Missile Warfare», USNI *Proceedings*, ottobre 1985 | https://www.usni.org/magazines/proceedings/1985/october/cruise-missile-warfare | estratto; problema OTH a più stadi (inviluppo di scoperta, HVU, saturazione) |
| [WP688] | Wikipedia, *Los Angeles-class submarine* (Mk 81 «weapon control consoles», CCS Mk 1 «All Digital Attack Center») | https://en.wikipedia.org/wiki/Los_Angeles-class_submarine | letto |
| [WPTom] | Wikipedia, *Tomahawk (missile)* | https://en.wikipedia.org/wiki/Tomahawk_(missile) | letto |
| [DS84] | designation-systems, AGM/RGM/UGM-84 | https://designation-systems.net/dusrm/m-84.html | letto |
| [DS109] | designation-systems, BGM/UGM-109 | https://designation-systems.net/dusrm/m-109.html | letto |
| [FAS84] | FAS, AGM-84 Harpoon | https://man.fas.org/dod-101/sys/smart/agm-84.htm | letto |
| [FASSWG1] | FAS, AN/SWG-1 HSCLCS | https://man.fas.org/dod-101/sys/ship/weaps/an-swg-1.htm | letto |
| [FASTWCS] | FAS, Tomahawk Weapon Control System | https://man.fas.org/dod-101/sys/ship/weaps/twcs.htm | letto |
| [FASBSY1] | FAS, AN/BSY-1 | https://man.fas.org/dod-101/sys/ship/weaps/an-bsy-1.htm | letto |
| [FAS98RDT] | FAS IRP, scheda di bilancio RDT&E 0604562N (FY1998/99) | https://irp.fas.org/budget/fy98_navy/0604562n.htm | letto |
| [NAWC] | NAVAIR NAWCTSD, *688 2nd Flight Weapons Launch Console (WLC)* e *688i WLC* | https://www.navair.navy.mil/nawctsd/688-2nd-Flight-Weapons-Launch-Console-WLC ; https://www.navair.navy.mil/nawctsd/688i-Weapons-Launch-Console-WLC | 403: estratto |
| [NOVA-C] | PBS NOVA, *Control room* | https://www.pbs.org/wgbh/nova/subsecrets/spriconhiright.html | letto |
| [VRS] | VRS (simulatore F/A-18), *AGM-84D Harpoon* | https://forums.vrsimulations.com/support/index.php/AGM-84D_Harpoon | letto; prodotto commerciale |
| [DSTO97] | J. McFarlane, DSTO-GD-0128, *Harpoon Shipboard Simulator* (manuale d'uso) | https://apps.dtic.mil/sti/pdfs/ADA327797.pdf | **PDF: da chiedere**; usato solo l'estratto |
| [SBIR10] | Navy SBIR N102-131, *Increased Target Selectivity Harpoon Seeker* | https://navysbir.com/n10_2/N102-131.htm | 403: estratto |
| [NAVS] | NavSource, pagina con la descrizione dell'Harpoon (archives/01/57s1) | https://navsource.org/archives/01/57s1.htm | DNS non raggiungibile: estratto |
| [AR26] | Army Recognition, F-101 spagnola e Harpoon a RIMPAC 2026 (limiti del Block 1C) | https://www.armyrecognition.com/news/navy-news/2026/spanish-f-101-aegis-frigate-launches-harpoon-anti-ship-missile-during-us-led-rimpac-2026 | estratto |
| [WTF] | Forum War Thunder, *AGM-84 Harpoon: technical data* (cita DTIC ADA045182 e le audizioni del 1974) | https://forum.warthunder.com/t/agm-84-harpoon-technical-data-and-discussion/102427 | letto; forum |
| [HGF] | HarpGamer, thread 2580 *BGM-109 Tomahawk intended use* | https://harpgamer.com/harpforum/topic/2580-bgm-109-tomahawk-intended-use/ | letto; forum |
| [NG2] | Naval Gazing, *Tomahawk Part 2* | https://navalgazing.net/Tomahawk-Part-2 | letto |
| [GAO81] | GAO C-MASAD-81-11, *Issues Affecting the Navy's Antiship Cruise Missile Programs* (1981): abstract | http://gao.justia.com/department-of-defense/1981/2/issues-affecting-the-navy-s-antiship-cruise-missile-programs-c-masad-81-11/ | abstract letto; testo completo **PDF: da chiedere** https://gao.gov/assets/c-masad-81-11.pdf |
| [MYNAVI] | Mynavi Tech+, *military IT* n. 446 (sigla EHCLS) | https://news.mynavi.jp/techplus/article/military_it-446 | estratto |
| [CSIS] | CSIS Missile Threat, *Harpoon* | https://missilethreat.csis.org/missile/harpoon/ | estratto |
| [ADU] | Aviation-Defence-Universe, «MBDA's Exocet SM39 is Indian Navy's pride» | https://www.aviation-defence-universe.com/mbdas-exocet-sm39-is-indian-navys-pride/ | estratto; marketing |
| [IWM] | IWM film, *Polaris submarine goes to condition 1SQ* | https://film.iwmcollections.org.uk/record/7696 | estratto |
| [MT20] | Military Times, incidente Jamaran–Konarak, 11/5/2020 | https://www.militarytimes.com/news/your-military/2020/05/11/iran-friendly-fire-missile-strike-in-drill-kills-19-sailors/ | estratto |
| [BECK] | K.J. Becker, *An analytical high value target acquisition model*, tesi NPS (cancello in distanza ottimo del seeker contro un HVU fra le scorte) | https://calhoun.nps.edu/handle/10945/21667 | abstract via estratto; **PDF: da chiedere** |

**Da chiedere** (PDF che non ho scaricato e che probabilmente colmano le lacune):
- DTIC **ADA327797**, DSTO-GD-0128: parametri del simulatore Harpoon, forse con le dimensioni delle aree RBL.
- DTIC **ADA045182**, *Test and Evaluation of Container Mk 619 Mod 0 for Harpoon Guidance Section AN/DSQ-28* (1977):
  https://apps.dtic.mil/sti/tr/pdf/ADA045182.pdf
- Tesi NPS di Becker (sopra).
- JHU APL *Technical Digest* vol. 13 n. 1, articolo di Hatch, storia dei missili da crociera (titolo da verificare):
  https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V13-N01/13-01-Hatch.pdf
- JHU APL *Technical Digest* vol. 2 n. 2, articolo di Shade (contenuto da verificare):
  https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V02-N02/02-02-Shade.pdf
- GAO **C-MASAD-81-11**, testo completo.
- Admiralty Trilogy, *Fire Control in Harpoon V* (gioco, non storico):
  https://www.admiraltytrilogy.com/pdf/Historicon2021_Fire%20Control_In_HarpoonV.pdf
- Il testo completo degli articoli USNI [USNI77], [FOX01], [LOC92] e [HURA85]: pagine HTML a pagamento, 403.

**Lacune rimaste:**
- disposizione e lampade reali della WLC e della pagina Mk 81 per i missili;
- dimensioni reali delle aree RBL e larghezza del cancello in azimut;
- portata d'acquisizione del DSQ-28;
- tempi di riscaldamento e allineamento dell'UGM-84 e del TASM;
- quota e velocità massime di lancio;
- esistenza di chiavi o consensi di sparo per le armi tattiche dei 688;
- geometria esatta della serpentina del TASM.
