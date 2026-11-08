# 05 — Missili e contromisure acustiche: impiego reale 1985–1991

> Dossier per il remake di *688 Attack Sub* (ambientazione 1989). Si occupa di **come si impiegavano** le armi: che cosa fa l'equipaggio, tempi, vincoli, modi di guasto ed effetti osservabili. Le prestazioni di base (portate, velocità, testate di Harpoon, Tomahawk, SS-N-19, siluri) e la scheda sintetica delle contromisure sono in `01_piattaforme_e_armi.md` §6 e **non sono ripetute**. Tattica d'evasione di base e logica dei siluri: `02_acustica_oceanografia_dottrina.md` §6.
> Compilato: 23 ottobre 2026.

---

## 0. Convenzioni

- **Confidenza:** **alta** (documento ufficiale o almeno due fonti indipendenti serie concordi); **media** (una buona fonte secondaria, ad es. designation-systems, Wikipedia RU/EN con note, deepstorm, NavWeaps); **bassa** (fonte singola debole, forum, oppure valore ricordato dalla letteratura e **non riverificato** in questa sessione, segnato *n.v.*).
- **STIMA DI GIOCO:** nessun dato pubblicato; valore proposto dal compilatore, motivato e coerente con il resto dei dossier. Va trattato come parametro di bilanciamento, non come dato storico.
- Sigle delle fonti tra parentesi quadre: elenco completo nella §8.
- Unità: kn, nmi, m; tempi in secondi o minuti. **Velocità missili:** Harpoon ≈ 240 m/s (Mach 0,71 per Wikipedia; Mach 0,85 per designation-systems); Tomahawk ≈ 245 m/s (Mach 0,74, [WPTom]).

### 0.1 Le dieci scoperte che cambiano il design

1. **L'Harpoon da sottomarino non accende il motore sott'acqua.** La capsula esce dal tubo a circa 30 ft/s (≈ 9 m/s), sale **per galleggiabilità** con alette che ne fissano l'angolo, e solo **all'affioramento** espelle i tappi di prua e di poppa e accende il booster [USNI77][DS84]. Ne seguono una salita di 10–30 s, nessuna correzione possibile e una finestra in cui la capsula è un oggetto galleggiante che fa rumore.
2. **Il Tomahawk da tubo, invece, accende il booster sott'acqua** (spinta vettoriale, Mk 106 da 26,7 kN per 12 s) e risale al suo controllo [DS109][REU76][GAO95]. Prezzo: il missile lanciato da sottomarino ha **circa il 30% di portata in meno** perché è parzialmente "sfuelato" per compensare il booster [GAO95].
3. **TLAM: un'ora di preparazione a bordo** con missione già pianificata [GAO95]; la pianificazione a terra richiedeva **fino a 72 h** e la missione arrivava a bordo su supporto fisico o via radio [FASCMSA]. In gioco il TLAM non si spara "a vista": si carica una missione preconfezionata e si raggiunge un *launch basket*.
4. **TASM: arma di area, non di precisione.** Dopo fino a ~30 min di volo cerca il bersaglio con un percorso a serpentina, usando ESM passivo e radar [DS109][NG2]. In simulazioni anni '80 **17 missili su 139** colpirono il bersaglio designato, **53** altre navi nemiche e **8** navi neutrali [NG2]. Il gioco deve permettere al TASM di colpire **la nave sbagliata**.
5. **"Hit and Run" (4 Tomahawk dal Baltico al largo della RDT) è implausibile come posizione di lancio.** Gli stretti danesi non si attraversano in immersione (doc 03 §Baltico), e dallo Skagerrak Berlino dista circa 670 km, dentro la portata di un TLAM-C lanciato da sottomarino. Il TLAM-C esisteva nel 1989 (IOC 1986 [DS109]); il problema è il punto di lancio, non l'arma.
6. **La MOSS Mk 70 non stava sui 688:** fu imbarcata sugli **SSBN** dagli anni '80 fino al ritiro a metà anni '90 [WPMOSS]. Sul 688 del 1989 le contromisure sono gli **ADC Mk 2 da 3"** lanciati dall'eiettore interno (ICL, "signal ejector") [GOVT]. I lanciatori esterni da 6" (CSA Mk 2, ADC Mk 3/Mk 4) sono probabilmente post-1989 (bassa).
7. **L'Alfa non ha un missile ASW convenzionale.** La Vyuga (SS-N-15) che può imbarcare è **solo nucleare** [RWVyuga]. Il Vodopad (SS-N-16) con siluro UMGT-1 sta su Victor III, Akula e Oscar [DSRM671][DSRM971][NPK206]. Lo stand-off ASW convenzionale contro il giocatore va dato a Victor III/Akula, non all'Alfa.
8. **Legenda nel 1989 è cieca contro gruppi in silenzio radar:** l'ultimo satellite radar US-A fu lanciato il **14 marzo 1988** [WPUSA]. Restano gli US-P ELINT, che vedono solo chi emette. Un gruppo USA in EMCON nel 1989 non dà a un Oscar dati satellitari utili.
9. **Il Charlie II mira con il proprio sonar.** Il Malakhit (8 missili, lancio da 50 m, salva unica di 8) riceveva i dati dal sonar Rubikon (scoperta >150 km dichiarata) [RWP120]. Un Charlie II che ha il bersaglio in zona di convergenza può lanciare senza aiuti esterni.
10. **Dati storici di efficacia:** contro navi da guerra in allerta solo circa il **27–29%** dei missili antinave colpisce, mentre i bersagli indifesi vengono colpiti a circa il 90%. Contro bersagli difesi Exocet e Harpoon arrivano a circa il **44%** [DRAS su Schulte 1994]. Le contromisure "soft-kill" (chaff, inganno) furono storicamente la difesa più efficace [DRAS].

---

## 1. UGM-84 Sub-Harpoon sul 688 (1989)

### 1.1 Configurazione d'epoca

| Voce | Valore | Fonte | Conf. |
|---|---|---|---|
| Versioni USN da sottomarino | **UGM-84A** (Block 1, pop-up terminale), **UGM-84C** (Block 1B, consegne dal giugno 1982, solo radente), **UGM-84D** (Block 1C, dal 1985: terminale selezionabile, JP-10, ECCM migliorate) | [DS84] | media |
| In servizio sui SSN | IOC sui sottomarini d'attacco 1981; **ritirato dalla USN nel 1997** | [DS84][WPHarp] | alta |
| Capsula | Contenitore stagno con galleggiabilità positiva, dimensioni di un siluro da 21"; espulsa a ≈ 30 ft/s; alette che si aprono e fissano l'angolo di salita; tappi di prua e di poppa espulsi all'affioramento | [USNI77] (estratto) [DS84] | media |
| Direzione del tiro | **Mk 117** digitale (688 Flight I); **CCS Mk 1** sui 688 con VLS (SSN-719…750), derivato dal Mk 117 con computer UYK-7/UYK-44; **BSY-1** sui 688I (SSN-751+), che assorbe le funzioni CCS | [DS84][WP688][NSLCCS] | alta |
| Guida | Riferimento d'assetto a 3 assi (non inerziale completa) e radar attivo AN/DSQ-28 in banda J, acceso a distanza preimpostata dal punto bersaglio previsto | [DS84] | media |
| Modi di lancio | **RBL** (*range and bearing launch*: vola alla distanza comandata, poi cerca); **BOL** (*bearing only*: distanza ignota, scansione di ±45° dopo l'accensione del seeker); **LOS** solo per lanci navali | [DS84][PAT6763289] | media |
| Terminale | Block 1: **pop-up a ≈ 1.800 m** e picchiata; Block 1B: radente; Block 1C: selezionabile | [DS84][WPHarp] | media |

**Nota di coerenza 1989.** Il prompt del progetto attribuisce il CCS Mk 1 al 688I. In realtà il CCS Mk 1 nasce per i 688 Flight II con VLS (Providence, SSN-719, primo lancio VLS con "CCS MK1 / Program C4.1" [WPProv]), mentre il 688I usa il BSY-1, che integra sonar e combattimento [NSLCCS]. Il 688 Flight I di default ha il **Mk 117**.

### 1.2 Sequenza di lancio (cosa fa l'equipaggio)

| # | Fase | Cosa avviene | Durata | Fonte / conf. |
|---|---|---|---|---|
| 0 | Carico | Il tubo deve contenere la capsula; se c'è un siluro va scaricato e ricaricato | 2–4 min/tubo (doc 01 §2.1) | STIMA DI GIOCO |
| 1 | Soluzione | Il Mk 117 fornisce rilevamento (sempre) e distanza (se disponibile) → scelta RBL o BOL, schema di ricerca, distanza di accensione del seeker, modo terminale (1C) | 30–90 s | STIMA (flusso [DS84]) |
| 2 | Accensione e allineamento | Alimentazione del missile e allineamento del riferimento d'assetto con i dati di navigazione del battello | 60–120 s | STIMA DI GIOCO (non pubblicato) |
| 3 | Preparazione del tubo | Allagamento, equalizzazione, apertura della porta esterna (transitori udibili, doc 02 §6.2) | 60–120 s | STIMA (come siluro) |
| 4 | Lancio | Espulsione a impulso (≈ 9 m/s) | 1 s | [USNI77] media |
| 5 | Salita della capsula | Galleggiabilità positiva, alette, salita inclinata; nessun motore | **10–30 s** da 20–60 m | STIMA (cinematica: 30–70 m di percorso a 2–5 m/s) |
| 6 | Affioramento | Rilevamento della superficie, breve ritardo programmato (un forum cita ≈ 1,5 s per lasciar defluire l'acqua, bassa), espulsione dei tappi, accensione del booster | 1–3 s | [DS84] media; ritardo bassa |
| 7 | Boost | Booster a propellente solido ≈ 2,5–3 s, poi separazione; il turbogetto J402 subentra | 3 s | bassa (fonte singola non verificata) |
| 8 | Discesa in crociera | Il missile scende alla quota di crociera radente misurata dal radioaltimetro | 10–20 s | STIMA |
| 9 | Crociera | Rotta verso il punto di accensione del seeker | t = D / 240 m/s (50 km ≈ 3,5 min; 100 km ≈ 7 min) | calcolo |
| 10 | Ricerca e aggancio | Il seeker si accende alla distanza preimpostata. RBL: schema di ricerca circolare o a settore; BOL: scansione di ±45° | 10–60 s | [DS84] media |
| 11 | Terminale | Radente o pop-up (≈ 1.800 m sul Block 1) | 5–15 s | [DS84] media |

**Tempo dall'ordine al lancio** (tubo già caricato con capsula, soluzione pronta): **3–5 min** (STIMA DI GIOCO). Se il tubo contiene un siluro: +2–4 min. In un'emergenza si può tenere un tubo **pre-caricato e pre-allineato**: allora restano solo i passi 3–4 (≈ 1–2 min).

### 1.3 Vincoli di lancio (profondità e velocità)

- **Profondità di lancio:** **non pubblicata** per l'UGM-84. Il brevetto sulle capsule galleggianti descrive un lancio legato alla risalita, non a una quota fissa [PAT4147124]. Termine di paragone: l'SM39 Exocet francese esce dal tubo con un veicolo propulso (VSM) e si dichiara utilizzabile "in immersione" senza cifre pubbliche [WPExo].
  **STIMA DI GIOCO:** quota di lancio **20–60 m** (sotto i 20 m la capsula rischia di affiorare troppo vicino alla vela, oltre i 60 m la salita diventa lunga e la deriva laterale cresce). Oltre i 60 m: probabilità di malfunzionamento in salita crescente, +5%/10 m.
- **Velocità del battello:** **non pubblicata**. **STIMA DI GIOCO:** ≤ 8 kn ideale; 8–12 kn ammessi con +10% di probabilità di fallimento (all'uscita dal tubo la capsula subisce una forte corrente trasversale che ne disturba l'assetto di salita).
- **Stato del mare:** con mare forte (Sea State ≥ 6) la capsula può non stabilizzarsi in superficie. **STIMA:** +10% di fallimento per ogni grado oltre 5.
- **Distanza minima:** non pubblicata. **STIMA DI GIOCO:** 3 nmi (≈ 5,5 km), il tempo di boost e discesa più armamento del seeker.

### 1.4 Dati di tiro: cosa serve e da dove arriva

| Dato | Necessario per | Fonte a bordo / esterna | Note |
|---|---|---|---|
| Rilevamento | RBL e BOL | sonar passivo (BQQ-5, TB-16), periscopio, ESM (WLR-8) | Il BOL è il modo naturale del sottomarino che ha solo rilevamenti [NSLOS]: "bearings-only at short range, bearing plus range for distant targets" |
| Distanza | RBL | TMA (doc 02), periscopio (stadimetro o radar BPS-15, che però è rischioso), zona di convergenza (distanza nota dalla geometria della CZ) | Errore ±10–20% compatibile con un seeker acceso presto |
| Rotta e velocità del bersaglio | per la previsione del punto futuro | TMA | Con un tempo di volo di 3–7 min il bersaglio a 20 kn si sposta di 2–4 km |
| Bersaglio oltre l'orizzonte (OTH) | RBL a lunga portata | **OSIS/FOSIC → Shore Targeting Terminal → broadcast al sottomarino (SSIXS via UHF satellitare, ISABPS VLF ogni ~2 h)** | In OUTLAW SHARK il sottomarino ricevette dati d'intelligence **in appena 6 minuti** dalla scoperta; all'inizio i dati si inserivano a mano nel sistema di tiro, poi Mk 117/CCS Mk 1 accettarono l'ingresso diretto [NSLOS] (media). L'antenna va esposta: serve la quota periscopica o una boa |

**Errore di localizzazione → scelta del modo:** se l'errore di posizione previsto all'accensione del seeker è minore della metà dell'area coperta dalla ricerca, conviene l'RBL; altrimenti il BOL con accensione anticipata. Accendere tardi riduce l'esposizione all'ECM ma richiede una posizione più precisa [DS84] (media).

### 1.5 Profilo di volo (valori di gioco)

| Parametro | Valore | Conf. |
|---|---|---|
| Velocità di crociera | 240 m/s (Mach 0,71–0,85) | media |
| Quota di crociera | Block 1: ~15 m; Block 1B/1C: ~5 m (il 1B vola "più basso") | bassa (solo qualitativo in [DS84]) — STIMA |
| Accensione seeker | parametro: 5–20 km prima del punto previsto | STIMA |
| Portata di aggancio del DSQ-28 su un caccia o incrociatore | 15–25 km (limitata dall'orizzonte radar a 5–15 m di quota) | STIMA |
| Ricerca RBL | cerchio o settore centrato sul punto previsto (fonti in disaccordo sulla geometria) | bassa |
| Ricerca BOL | ±45° dalla rotta, fino a esaurimento del carburante o fino alla portata massima | media |
| Pop-up terminale (UGM-84A; opz. 1C) | salita a ≈ 1.800 m e picchiata (evita la difesa a tiro teso, ma espone al radar) | media |
| Testata | 221 kg WDU-18/B, spoletta a impatto ritardata | media [DS84] |

### 1.6 Dottrina di salva

- **4 tubi = salva massima di 4 Harpoon** (se tutti caricati con capsule). La ricarica dei tubi (2–4 min ciascuno) separa le salve successive di 5–10 min: niente arrivo simultaneo (TOT) fra salve diverse (STIMA).
- Il Block 1/1B da sottomarino **non aveva waypoint**; i waypoint del Block 1C sono documentati per le navi con AN/SWG-1A [DS84]. Il 688 del 1989 non poteva quindi "piegare" la rotta per mascherare il punto di lancio (media).
- **Numero di missili per un colpo:** con P(colpo) ≈ 0,44 per missile contro un bersaglio difeso (dati storici Exocet e Harpoon [DRAS]), per avere P ≥ 0,8 di almeno un colpo servono 3 missili (1 − 0,56³ = 0,82). Contro un gruppo di superficie (Kirov, Slava) servono 4 + siluri. **Regola di gioco consigliata:** salva da 2 per fregata o caccia, da 4 per incrociatore.
- **Lanci alternati di tubi:** tra un tubo e l'altro di una salva servono **10–20 s** (STIMA), per evitare collisioni fra capsule in salita.

### 1.7 Rischi di contro-scoperta

| Indizio | Chi lo rileva | Portata indicativa | Conf. |
|---|---|---|---|
| Transitori di preparazione del tubo e di lancio | sonar passivi nemici | come i transitori di un siluro (doc 02) | media (concetto) |
| Capsula che sale e affiora (spruzzo, tappi) | vedette, elicottero | visivo 2–5 km di giorno | STIMA |
| **Booster all'accensione:** lampo e colonna di fumo bianco | vedette, elicotteri, aerei ASW, IR | visivo 10–20 km di giorno; il lampo di notte anche oltre | STIMA |
| Missile in crociera radente | radar di scoperta navale | orizzonte radar: d ≈ 4,12·(√h_ant + √h_mis) km → antenna 20 m e missile 5 m: **≈ 28 km**; antenna 35 m: ≈ 33 km. Scoperta effettiva ~60–80% dell'orizzonte per il clutter | calcolo + STIMA |
| Seeker DSQ-28 acceso | ESM navali sovietici | da quando si accende: preavviso di 40–90 s | STIMA |
| **Retroproiezione della rotta** | ASW sovietico | il punto di lancio è entro pochi km lungo la rotta del missile, che va dritta per l'assenza di waypoint | ragionamento, media |

**Conseguenza tattica (*shoot and scoot*):** dopo il lancio l'IA sovietica conosce un *datum*. Elicottero Ka-25/Ka-27 sul datum in 10–20 min; nave a 30 kn da 20 nmi in ~40 min (STIMA). Il giocatore deve lasciare il datum di almeno 3–5 nmi o cambiare quota oltre il layer prima dell'arrivo dei mezzi aerei.

### 1.8 Effetti osservabili al periscopio

- **Lancio proprio** (osservato dal periscopio del 688 a quota periscopica, a 100–500 m): ribollio e spruzzo dove affiora la capsula; capsula inclinata di ~45° che sporge per qualche metro; due piccole espulsioni (tappi); lampo arancio, **getto di fumo bianco denso** lungo 50–150 m che sale con un angolo di 20–40°; separazione del booster (un piccolo cilindro che ricade) e missile che si livella a quota radente: dopo 2–3 km diventa invisibile a occhio. *(Ricostruzione dalla sequenza documentata e dalla foto DoD DNSC8307009; i dettagli visivi sono STIMA.)*
- **Impatto sul bersaglio** (a 5–20 km): lampo, colonna di fumo nero o grigio dall'incendio; se il bersaglio reagisce: **nubi di chaff** (punti grigi sospesi a 100–300 m di quota a 1–2 km dalla nave), traccianti dei CIWS (AK-630: flusso rosso continuo), lanci di SAM (Osa-M: due missili da un lanciatore binato sul ponte). Tutti visibili al periscopio se il bersaglio è entro l'orizzonte.

---

## 2. Tomahawk sul 688

### 2.1 Varianti e stato nel 1989

| Variante | Stato 1989 | Ruolo in gioco | Fonte / conf. |
|---|---|---|---|
| **UGM-109A TLAM-N** (W80, 5 o 150 kt) | in servizio (pronto marzo 1983); ritirato dalle navi nel 1991, in deposito per altri 20 anni | solo con regole d'ingaggio nucleari | [DS109][NG1] media |
| **UGM-109B TASM** | in servizio (OPEVAL sottomarini 1981–83); ritirato all'inizio degli anni '90 e convertito | antinave a lunghissima portata | [DS109][FASTom] media |
| **UGM-109C TLAM-C** | in servizio dal 1986 (Block II) | attacco al suolo convenzionale | [DS109] alta |
| UGM-109D TLAM-D (submunizioni) | in servizio dal 1988 | attacco al suolo, aree | [DS109] media |
| Block III (GPS) | **no** (primo lancio gennaio 1991, IOC maggio 1993) | anacronismo | [DS109][FASTom] alta |

### 2.2 Lancio da tubo e da VLS

| Voce | Tubo lanciasiluri (tutti i 688) | VLS a 12 celle (SSN-719+, "Mark 36") |
|---|---|---|
| Piattaforme 1989 | tutti i 688 | **8 Flight II** (SSN-719…725 e 750, da Providence 1985) + i primi 688I | 
| Contenitore | Il missile resta nella capsula finché non è fuori dal tubo, poi la capsula viene espulsa e **il booster si accende sott'acqua** spingendo il missile in superficie [DS109]; nel 1976 si descriveva una spinta subacquea con controllo vettoriale fino all'affioramento [REU76] | Espulsione a gas dalla cella, poi il booster porta il missile in superficie [WPTom] |
| Booster | Mk 106, **26,7 kN per 12 s** [DS109] | idem |
| Uscita dall'acqua | angolo ripido (STIMA 45–60° dal tubo; quasi verticale dal VLS) | — |
| Dopo il booster | Coperture espulse; si aprono le alette di coda e poi le ali; si estende la presa d'aria ventrale; il booster si sgancia e parte il turbofan F107 [DS109] | idem |
| Portata | **circa −30%** rispetto al lancio navale (rifornimento ridotto per il booster) [GAO95] | come da tubo (STIMA, stesso missile) |
| Costo in armi | **1 Tomahawk = 1 posto siluro** [GAO95] | non consuma posti in camera di lancio |
| Disposizione | — | Providence e Pittsburgh: 4 file di 3; gli altri: file interne da 4 ed esterne da 2 [WP688] |

**Preparazione:** con una missione preconfezionata a bordo, la piattaforma richiedeva **circa 1 h** per sparare [GAO95] (alta per il 1991, media per il 1989). La procedura comprende accensione del missile, **allineamento inerziale** e trasferimento della missione nel computer del missile [FASCMSA].

**Affidabilità osservata (Desert Storm):** 297 lanci tentati, 288 avvenuti, 282 in volo di crociera (95%); dei 6 caduti dopo il lancio, 5 per mancata apertura delle ali e 1 per mancata separazione del booster [GAO95] (alta). Sottomarini coinvolti: **USS Louisville e USS Pittsburgh**, 12 missili da sottomarino su 288 [WPTom] (media).

### 2.3 TASM: impiego antinave

- **Guida:** riferimento d'assetto strapdown e seeker AN/DSQ-28 in banda J derivato dall'Harpoon. Il missile si lancia verso **l'area** del bersaglio e la percorre **a serpentina**, cercando con **ESM passivo** (le emissioni radar del bersaglio) e con il radar attivo per l'aggancio. Dopo l'aggancio si avvicina radente, con pop-up e cambi di rotta opzionali [DS109] (media). Il ricevitore ESM era tarato su emissioni caratteristiche del bersaglio voluto (secondo alcuni sul radar altimetrico delle Moskva) [NG2] (bassa).
- **Portata:** aerodinamica circa 450 nmi, **efficace circa 250 nmi** per il carburante consumato nella ricerca [NG2]. Primo volo di OUTLAW SHARK: 175 nmi verso il bersaglio più 173 nmi di ricerca, bersaglio trovato a 240 nmi dal lancio [NSLOS] (media).
- **Tempo di volo:** fino a **circa 30 min**, durante i quali una nave veloce si sposta di circa 15 nmi [ricerca WebSearch su NG2/USNI] (media). Calcolo: 460 km / 245 m/s ≈ 31 min più la ricerca.
- **Catena di scoperta (kill chain):** satelliti, aerei di pattugliamento e radiogoniometria correlati a terra e inviati via satellite. Dall'inizio degli anni '70 agli anni '80 il ritardo fra sensore e tiratore scese da **"diverse ore" a "di solito meno di un'ora"** [NG2] (media). La piattaforma pianificava il volo per evitare i mercantili innocenti [NG2].
- **Problema di discriminazione:** nelle simulazioni anni '80, su **139 lanci, 17 colpi sul bersaglio designato, 53 su altre navi nemiche, 8 su navi neutrali**; l'obiettivo della USN era il 90% [NG2] (media). Se al momento dell'accensione del radar la nave nemica non era la più vicina, il seeker poteva agganciare un neutrale [ricerca WebSearch] (media).
- **Il 688 non ha un collegamento con il missile in volo:** la dottrina USA per i sottomarini escludeva l'aggiornamento in volo (a differenza del Tu-95RTs sovietico) [NG2] (media).

### 2.4 TLAM-C/N: impiego contro terra

| Elemento | Dettaglio | Fonte / conf. |
|---|---|---|
| Pianificazione | Il comandante unificato assegna, il Cruise Missile Support Activity pianifica la parte terrestre su dati cartografici (TERCOM) e immagini (DSMAC); il sistema di tiro della piattaforma prepara ed esegue | [FASTom][FASCMSA] media |
| Tempo di pianificazione | fino a **72 h** nel 1991; immagini DSMAC a volte non disponibili, quindi ritardi | [FASCMSA] media |
| Dati a bordo | Su "data transport device" (disco) o via radio | [FASCMSA] media |
| Navigazione | Inerziale sul mare → **landfall** e aggiornamenti **TERCOM** (radioaltimetro confrontato con mappe di quota) → **DSMAC** terminale (confronto ottico); CEP del TERCOM stimato in 50 m anche a 1.350 nmi | [NG1][FASTom] media |
| Vincolo del terreno | Il terreno piatto e senza elementi caratteristici rende difficilissime le mappe TERCOM e le scene DSMAC ("perhaps the most difficult task possible", deserto iracheno); rotte utilizzabili limitate, quindi più missili sulla stessa rotta e difese che imparano a prevederla | [GAO95] alta |
| Vulnerabilità | Alcuni abbattuti dall'artiglieria contraerea; nessuna prova di abbattimenti con SAM | [GAO95] media |
| Precisione osservata | Oltre il 75% dei TLAM-C che raggiunsero l'area colpì il punto di mira | [GAO95] alta |
| Quota | 30–50 m sul terreno | [WPTom] media |

**Concetto di *launch basket*.** Le missioni Block II sono pianificate da un'area di lancio definita (posizione e orientamento iniziale), perché la tratta sul mare fino al landfall è inerziale e prefissata. Il battello deve trovarsi **dentro l'area prevista** con la navigazione allineata. Fonti aperte che descrivono le dimensioni delle aree non trovate. **STIMA DI GIOCO:** cerchio di raggio 5–10 nmi attorno al punto pianificato; fuori dall'area la missione non è valida.

### 2.5 Missione 10 "Hit and Run" (Baltico, 4 Tomahawk al largo della RDT): plausibilità 1989

| Aspetto | Valutazione | Conf. |
|---|---|---|
| Arma | TLAM-C in servizio dal 1986 (o TLAM-N con regole nucleari): **plausibile** | alta |
| Bisogno di entrare nel Baltico | **Nessuno**: con circa 875 km di portata da sottomarino (1.250 km −30%) si raggiunge l'intera RDT dallo Skagerrak o dal Mare del Nord (Skagerrak–Berlino ≈ 670 km, calcolo geodetico) | media |
| Accesso | Soglie dell'Øresund a 7–9 m e del Grande Belt a 17–20 m: un 688 (pescaggio 9,5 m, altezza con la vela 15–16 m) **non passa in immersione** (doc 03) | alta |
| Prassi | Gli SSN americani di fatto non operavano nel Baltico (doc 03) | media |
| Terreno TERCOM | La pianura della Germania settentrionale è piatta: stesso problema del deserto iracheno, con poche rotte di landfall utilizzabili (Rügen, coste a falesia) | media (per analogia [GAO95]) |
| Pericolo | Difese sovietiche e della RDT dense e Flotta del Baltico in acque basse | alta |

**Raccomandazione:** tenere la missione ma **spostare l'area di lancio nello Skagerrak o nel Kattegat settentrionale** (profondità 50–100 m), con la fuga attraverso lo Skagerrak pattugliato. In alternativa mantenere la versione originale con una dichiarazione esplicita di **licenza poetica** nel briefing del museo.

### 2.6 Trade-off del carico (1989)

- 688: **24 armi da tubo** [WP688] (altre fonti 22–26, doc 01). Ogni Harpoon o Tomahawk toglie un siluro [GAO95].
- Flight II e 688I: **+12 Tomahawk in VLS** (solo Tomahawk; un Harpoon da VLS sul 688 non è documentato nel 1989, bassa).
- **Carico tipico 1989, STIMA DI GIOCO** (pubblicazioni con il carico reale non trovate):

| Missione | Flight I (24) | Flight II (24 + 12 VLS) |
|---|---|---|
| ASW / barriera | 20 Mk48, 4 Harpoon | 20 Mk48, 4 Harpoon; VLS: 8 TLAM-N/C, 4 TASM |
| Scorta / anti-SAG | 14 Mk48, 4 Harpoon, 6 TASM | 18 Mk48, 6 Harpoon; VLS: 8 TASM, 4 TLAM-C |
| Strike | 12 Mk48, 4 Harpoon, 8 TLAM-C/N | 20 Mk48, 4 Harpoon; VLS: 12 TLAM-C |
| Minamento | 8 Mk48, 2 Harpoon + mine Mk67/Mk60 (2 per posto, STIMA) | idem + VLS invariato |

---

## 3. Missili sovietici rilevanti per il gioco

### 3.1 Tabella d'impiego

| Sistema (NATO) | Piattaforme 1989 | Tubo | Profondità di lancio | Bersaglio, dati | Carico | Fonte / conf. |
|---|---|---|---|---|---|---|
| **RPK-2 Vyuga / 81R (SS-N-15)** | Pr.705 (K-64, primo battello armato), 671V, 671RT, 671RTM(K), 971, 661 | 533 | **≈ 50–60 m** | inerziale, nessuna autoguida; 10–42 km; la letalità dipende dalla grande area di effetto della carica nucleare | **solo nucleare**; 3–4 missili per battello (una fonte) | [RWVyuga] media; profondità e numero da estratto deepstorm/RU (media-bassa) |
| **RPK-6 Vodopad (SS-N-16)**: 83R (siluro UMGT-1) / 84R (nucleare) | 671RTM (18 armi da 533 incl. 83R/84R), 971, 949A, (945) | 533 | **fino a 150 m**; portata **5–50 km da 20–50 m, 5–35 km da 150 m**; sul Pr.941 solo fino a 55 m | dati dal sonar di bordo; il carico ammara con paracadute | UMGT-1: 41 kn, 8 km, fino a 500 m | [RWVodopad][DSRM671] media; profondità da estratto (media-bassa) |
| **RPK-7 Veter (SS-N-16)**: 86R/88R | 971 (12 armi da 650 incl. Veter), 949A, 671RTM | 650 | n.d. | **fino a ~100 km** (doc 01) | UMGT-1 o nucleare | [DSRM971][NPK206] media |
| **S-10 Granat (SS-N-21)** | 971, 945, 671RTM, Yankee Notch | 533 | n.d. | inerziale e TERCOM, 3.000 km; **strategico**, irrilevante contro navi | nucleare 200 kt | [WPRK55] media |
| **P-700 Granit (SS-N-19)** | **949/949A** (24), Kirov (20), Kuznetsov (12) | contenitori inclinati di ~40–60°, allagati prima del lancio | n.d. (da immersione) | vedi §3.3 | 518–750 kg HE o nucleare | [RWP700][NPK206] media |
| **P-70 Ametist (SS-N-7)** | Pr.670 Charlie I | contenitori | **≤ 30 m** | 65 km; boost di 7 s dall'uscita dal contenitore alla fase di crociera | 1.000 kg (HEAT o nucleare) | [WPP70] media |
| **P-120 Malakhit (SS-N-9)** | **Pr.670M Charlie II** (8) | 8 contenitori a prua inclinati di 38,5° | **≤ 50 m** (30 m sul Charlie I), "lancio umido" | 120 km (Wikipedia RU), 120–150 km (deepstorm); dati dal **sonar Rubikon** | salva di **tutti e 8** | [RWP120] media |

**Nota politica, contestata:** deepstorm afferma che, "in base ad accordi sovietico-americani del 1989", le armi nucleari (Shkval e Vodopad nucleari, Granat) furono tolte dai SSN [DSRM971]. Le iniziative unilaterali di ritiro delle armi nucleari tattiche navali sono però dell'autunno 1991. Per il gioco: armi nucleari ASW **disponibili nel 1989 solo con regole nucleari** (bassa sulla datazione).

### 3.2 L'Alfa e lo stand-off ASW contro il 688

- Pr.705: **6 tubi da 533 pneumoidraulici**, utilizzabili da quota periscopica fino alla massima [DSRM705] (media-alta). Questo è un vantaggio tattico reale: l'Alfa spara a qualunque quota.
- Dotazione documentata: **20 siluri (SAET-60, SET-65)** oppure 24 (36?) mine [DSRM705]. La Vyuga è attestata dalla RU wiki e da un estratto deepstorm (media). Il Vodopad sull'Alfa non è documentato (bassa: no).
- **In un conflitto convenzionale l'Alfa usa solo siluri.** Il giocatore deve temere stand-off ASW convenzionale (83R) da **Victor III e Akula**, non dall'Alfa.
- **Cosa sente il 688 quando arriva un 83R:** transitori di lancio del battello sovietico (se in portata); poi il motore a razzo all'uscita dall'acqua (probabilmente udibile); infine l'**ammaraggio del siluro con paracadute** vicino (splash e avvio dell'UMGT-1: tonali alte, WLR-9 che segnala il ping attivo). **STIMA DI GIOCO:** volo del vettore 1–3 min (per 10–50 km); avviso utile al giocatore 20–40 s dall'ammaraggio all'attivazione.

### 3.3 P-700 Granit e Oscar: tattica di salva

- **Profilo:** oltre circa 100–120 km il missile sale a **14–17 km** per la maggior parte del volo e, dopo la scoperta dei bersagli, scende a **circa 25 m**, sotto l'orizzonte radar della nave. Portata 550 (625) km combinata, **145 (200) km** tutta a bassa quota. Mach 2,5 in quota, 1,5 in basso [RWP700] (media).
- **Salva in rete:** i missili scambiano dati, identificano i bersagli per dimensione e posizione relativa, li ripartiscono e non sprecano più missili del necessario su un bersaglio. I bersagli secondari si attaccano solo dopo i prioritari [RWP700]. La versione EN dice che uno dei missili sale per designare e che, se viene distrutto, ne subentra un altro; la stessa Wikipedia avverte che sulla descrizione degli sciami esistono dubbi [WPGranit] (bassa sulla meccanica precisa).
- **Salva:** fino a **24**, "a tempo elevato", con ripartizione dei bersagli [RWP700/estratti] (media). Tempo fra un lancio e il successivo **non pubblicato**. **STIMA DI GIOCO:** 1 missile ogni 3–5 s, 24 missili in 1,5–2 min, a 10–30 m di quota e ≤ 6 kn.
- **Dati di tiro:** Uspekh (Tu-95RTs, Ka-25Ts) o Legenda (satelliti) [RWP700]; ricezione con l'antenna Korall nella vela o con una boa VLF rimorchiata [NPK206] (media).
- **Legenda nel 1989:** sistema accettato nel 1978 (US-A radar più US-P ELINT) [WPLeg]; **ultimo US-A lanciato il 14/3/1988** [WPUSA] (alta). Nel 1989, quindi, solo ELINT (US-P): **un gruppo in EMCON non viene localizzato dallo spazio**. Il Tu-95RTs resta, ma è vulnerabile ai caccia imbarcati [ricerca dfnc.ru/Legenda] (media).

### 3.4 Charlie II e Malakhit

- Sistema di controllo "Dunay-670M-1": verifica e lancia **tutti e 8** i missili in una sola salva; contenitori a prua inclinati di 38,5°; lancio umido fino a 50 m [RWP120] (media).
- Affidabilità alle prove (1974): dopo il primo lancio riuscito, solo 2 dei 6 successivi andarono in modo nominale (Wikipedia RU); deepstorm conta 4 riusciti su 8 [RWP120] (media). Gioco: **10–15% di guasto al lancio** (STIMA).
- **Dati:** sonar Rubikon (scoperta superiore a 150 km dichiarata; navi di superficie a 180–200 km) [RWP120]. In acque profonde significa un bersaglio in **1ª o 2ª zona di convergenza** (55 o 110 km): coerente con la portata del missile.

### 3.5 Che cosa osserva il 688 di un lancio sovietico

| Evento | Indizio | STIMA del rilevamento |
|---|---|---|
| Allagamento dei contenitori (Oscar, Charlie) | transitorio idraulico | 5–20 km in passivo |
| Accensione sott'acqua o all'uscita | rumore di razzo a larga banda, molto forte | 20–50 km (CZ inclusa) |
| Salva di 8 o 24 missili | serie regolare di transitori ogni 3–5 s | come sopra; classificazione certa |
| Missili in volo | ESM del 688 a quota periscopica: nessuna emissione finché il seeker è spento | — |

---

## 4. Difese di superficie contro i nostri missili

### 4.1 Difese USA (contro i missili sovietici, e per gli scenari di scorta)

| Sistema | Dati chiave | Fonte / conf. |
|---|---|---|
| **Phalanx Block 0** (1980) | M61 da 20 mm, 3.000 colpi/min, **989 colpi**, ricarica 10–30 min con 2 uomini; portata efficace massima **1.490 m** | [NWPhal] alta |
| **Phalanx Block 1** (1988, prima su Wisconsin) | 3.000 o 4.500 colpi/min, 1.550 colpi, ricarica < 5 min; dà priorità alle prime 6 minacce a circa 10.000 yd e ingaggia a **4.000 yd (3.600 m)**; antenna che copre fino alla verticale (contro il tuffo terminale) | [NWPhal] alta |
| SM-1MR / SM-2MR | Perry (SM-1), Ticonderoga e Kidd (SM-2); portate 40–75 km *n.v.* | media-bassa |
| Sea Sparrow (RIM-7M, Mk 29) | portaerei, Spruance, ausiliarie; ~15–20 km *n.v.* | bassa |
| SRBOC Mk 36 | lanciatori a 6 canne da 130 mm, chaff e IR; seduzione e confusione | *n.v.* media |
| AN/SLQ-25 Nixie | §5.5 | [FASSLQ25] alta |

### 4.2 Difese sovietiche (contro Harpoon e TASM del giocatore)

| Sistema | Dati chiave | Fonte / conf. |
|---|---|---|
| **AK-630 / AK-630M** | 30 mm a 6 canne, **4.000–5.000 colpi/min**, 2.000 colpi per affusto; ingaggio di bersagli aerei fino a **4.000 m**; direzione del tiro **MR-123 Vympel** (1 radar per 2 affusti); accettato nel 1976 (630M nel 1979) | [NWAK630] alta |
| **Osa-M / Osa-MA / Osa-MA-2 (SA-N-4)** | 1971 / 1979 / prima metà anni '80; portata massima **15 km**, quota massima 3,5–4 km; Osa-MA contro bersagli bassi a **25–60 m**; Osa-MA-2 contro missili antinave a **5 m sopra le creste**; **2 missili pronti**, ricarica 16–21 s, 2 colpi/min | [RWOsaM] media |
| **Kinzhal (SA-N-9)** | entrato in servizio **nel 1989**; 1,5–12 km, quota **10–6.000 m**; 8 missili guidati su 4 bersagli | [WPKinz] media. Nel 1989 operativo su poche unità (Udaloy tardi, Kalinin? → bassa) |
| S-300F Fort (SA-N-6) | Kirov, Slava: area; contro bersagli radenti limitato dall'orizzonte | *n.v.* bassa |
| **PK-2** (2 canne da 140 mm, ZIF-121) | chaff e IR di medio raggio; standard su caccia e incrociatori anni '70–'80 | *n.v.* bassa |
| **PK-16** (82 mm) | chaff a corto raggio; ad es. 2 lanciatori sulla Pauk | [ricerca naval-encyclopedia] bassa |
| **PK-10 "Smelyy"** (120 mm, 10 canne) | adottato nel **1984–85**; chaff e falsi bersagli IR "di corto raggio", da usare insieme ai complessi di medio e lungo raggio | [RWPK10 via estratto] media |
| Udav-1 (§5.5) | antisiluro, non antimissile | — |

### 4.3 Dati storici (base per le probabilità)

Da Schulte (NPS, 1994; dati 1967–1991) aggiornato da Prakash [DRAS] (media; la tesi originale [SCH94] non è stata letta integralmente):

| Categoria di bersaglio | Missili | Colpi | P(colpo) | Esiti |
|---|---|---|---|---|
| Indifeso (mercantili) | 63 | — | **0,907** | P(affondamento) 0,171 |
| Nave da guerra in bassa prontezza | 38 | 26 | **0,68** | 6 affondate, 13 fuori combattimento |
| Nave da guerra in allerta (difesa) | 124 | 34 | **0,27–0,29** | 14 affondate, 17 fuori combattimento |
| — solo 1ª generazione (Styx, Gabriel) contro difesi | — | — | 0,196 | — |
| — **Exocet e Harpoon contro difesi** | — | — | **0,438** | — |

Altri dati: in media **1,2 colpi mettono fuori combattimento e 1,8 affondano** una nave (studio NPS su 25 anni, corvette e fregate) [ricerca WebSearch] (media). Il **soft-kill** è stato "estremamente efficace" (Latakia 1973, Falkland 1982, Praying Mantis 1988); un solo abbattimento hard-kill confermato prima del 2016 (Sea Dart contro Silkworm, 1991) [DRAS] (media). Nella Praying Mantis l'Harpoon iraniano fu sconfitto da **chaff ed ECM** della Wainwright [WPHarp/ricerca] (media).

### 4.4 Modelli di letteratura utilizzabili

**(a) Equazioni di salva di Hughes** [HUG95] (forma: alta; parametri: STIMA):

```
ΔB = (σA·τA·α·A − δB·τB·b3·B) / b1        (perdite di B, in navi; ≥ 0)
ΔA = (σB·τB·β·B − δA·τA·a3·A) / a1
α, β   = missili ben mirati per tiratore
a3, b3 = missili intercettati per difensore
a1, b1 = colpi per mettere fuori combattimento una nave (staying power)
σ = efficacia della scoperta (0–1), τ = addestramento (0–1), δ = prontezza difensiva (0–1)
```

Uso in gioco: risoluzione rapida e **fuori schermo** di scontri missilistici fra gruppi (ad es. un Oscar contro una portaerei in background). Per i lanci del giocatore usare (b).

**(b) Modello a strati per singolo missile** (consigliato per l'esito visibile; STIMA DI GIOCO calibrata su §4.3):

```
P(colpo) = P_aff · P_acq · Π_strati (1 − Pk_i · n_i) · (1 − P_soft)
P_aff  = affidabilità (Harpoon/TASM 0,90; vedi Tomahawk 95% in crociera [GAO95])
P_acq  = aggancio del bersaglio giusto (RBL, errore < metà dell'area di ricerca: 0,9; BOL: 0,8; TASM: 0,5 sul bersaglio designato, 0,4 su un'altra nave)
strati: SAM d'area, SAM di punto, CIWS; n_i = ingaggi possibili nel tempo disponibile
P_soft = seduzione da chaff/ECM se il bersaglio è in allerta e ha lanciatori
```

Tempi d'ingaggio (calcolo): scoperta al 70% dell'orizzonte radar (≈ 20–23 km per antenna a 20–35 m e missile a 5 m) → **≈ 85–95 s** prima dell'impatto a 240 m/s. Osa-M (2 pronti, 2/min): 2–3 ingaggi; AK-630 (≤ 4 km): ≈ 15 s di fuoco.

| Difesa (bersaglio in allerta) | Pk per ingaggio contro subsonico radente | Ingaggi per missile | STIMA |
|---|---|---|---|
| Osa-M (1971) | 0,15 | 2 | bersagli < 25 m quasi fuori inviluppo |
| Osa-MA-2 | 0,35 | 2 | — |
| Kinzhal | 0,5 | 2 | quota minima 10 m: contro 1B/1C (5 m) ×0,6 |
| S-300F | 0,3 | 1 | limitato dall'orizzonte |
| AK-630 (per affusto) | 0,3 | 1 | 2 affusti/MR-123 |
| Phalanx Block 0/1 | 0,35 / 0,45 | 1 | contro supersonici (Granit) ×0,5 |
| Chaff PK-2/PK-16/PK-10 | P_soft = 0,3 / 0,25 / 0,4 | — | combinati: 1 − Π(1 − p) |
| Saturazione | ogni missile oltre il numero di canali di guida (Osa: 1 bersaglio per lanciatore; Kinzhal: 4) passa gli strati SAM | — | — |

Con questi valori una fregata Krivak (2 Osa-M e PK-16, senza CIWS) viene colpita da un Harpoon con P ≈ 0,9·0,9·(1 − 0,15)²·(1 − 0,25) ≈ 0,44, coerente con il dato storico di 0,438.

**Danni (STIMA DI GIOCO ispirata a [DRAS]/Hughes):** colpi per mettere fuori combattimento `b1 ≈ 1,2·(D/3.000 t)^(1/3)`; per affondare ≈ 1,5·b1. Esempi con Harpoon da 221 kg: Krivak (3.500 t) 1,3 / 1,9; Udaloy (8.500 t) 1,7 / 2,5; Slava (11.500 t) 1,9 / 2,8; Kirov (24.000 t) 2,4 / 3,6. Con il TASM (454 kg): ×0,7 colpi.

---

## 5. Contromisure acustiche

### 5.1 USA (lato giocatore)

| Contromisura | Fatti | Fonte / conf. |
|---|---|---|
| **3-inch signal ejector / Internal Countermeasure Launcher (ICL)** | Lanciatore interno ricaricabile da 3" per contromisure, pirotecnici, boe di comunicazione e batitermografi (XBT) | [GOVT][NSL07][PAT6341101] alta |
| **ADC Mk 2** | Contromisura acustica espendibile da 3". **Mod 5 = sottomarini, lanciata dall'ICL o signal ejector; Mod 4 = navi, lancio a mano**. Staziona in verticale a quota preimpostata con motore ed eliche intubate e batteria termica al litio | [GOVT] alta; descrizione doc 01 [ADC] media |
| Inviluppo dell'eiettore da 3" | Per gli espendibili "operational stances" un autore cita nominalmente **400–600 ft e 10–12 kn per 10–20 min** (2007, per boe di comunicazione) | [NSL07] media (riferito ad altri espendibili) |
| Numero di eiettori sul 688 | **Non trovato**. Letteratura ricordata: due eiettori da 3" *n.v.* | bassa |
| **ADC Mk 3** (6", esterno) e **CSA Mk 2** | Lanciatori esterni da 6" (ELC), i dispositivi sono all-up-round fuori dallo scafo resistente. Su 20 SSN 688 con 14 lanciatori sotto la linea di galleggiamento | [PAT6341101] media; conteggi da [SUBSIM] bassa |
| ADC Mk 4 | Jammer da 6,25", 120 lb: genera toni a larga banda che presentano al siluro molti falsi bersagli al secondo, il siluro gira attorno al jammer mentre il sottomarino si allontana | [TWZ] media. **Data di servizio probabilmente anni '90**, bassa |
| Durata degli espendibili | batterie ad acqua di mare per **6–45 min**; un ADC esterno ha 2 anni di vita operativa dopo l'installazione | [TWZ] media |
| **MOSS Mk 70** | Veicolo da 10", senza testata; eco attivo e firma passiva simili al battello; in servizio dal 1976; **SSBN** dagli anni '80 al ritiro a metà anni '90 | [WPMOSS] media |
| WLR-9 | Ricevitore acustico che rileva sonar attivi e siluri a autoguida acustica (preavviso) | [WP688] alta |

**Verdetto 1989 per il 688 Flight I:** ADC Mk 2 dall'eiettore interno (ricaricabile); niente lanciatori esterni (probabilmente retrofit successivi, bassa); niente MOSS. Un'esca mobile da tubo è al limite e andrebbe tolta o resa opzionale ("what-if").

### 5.2 URSS (lato nemico)

| Contromisura | Fatti | Fonte / conf. |
|---|---|---|
| **MG-74 "Korund"** (MG-74E da esportazione) | Adottata nel **1974**. Semovente, **lanciata da tubo da 533**, forma ed energia da siluro elettrico, lunga **3,9 m**, quota di corsa **fino a 250 m**. Modi: **disturbo mirato in frequenza**, **imitazione dell'eco** del sottomarino, **imitazione del rumore**; manovra programmata nei due piani "come un sottomarino"; quota e velocità impostate prima del lancio (ingresso a mandrino). Può cambiare modo e manovra da sola in base ai ping che riceve | [WARFOR][KREM] media |
| MG-74 "Korund-2" su Victor III | Imbarcabile **al posto dei siluri** sul Pr.671RTM | [DSRM671] media-alta |
| MG-44 "Korund-1" | Esca natante attiva e passiva da 400 mm (Delta) | [WPDecoy] bassa |
| MG-104 | Adottata nel **1987**, più piccola, lanciata dal **lanciatore esterno unificato (UZPU)**, non dal tubo | [WARFOR] media |
| Lanciatori esterni sugli Akula | Dal K-391 lanciatore in sovrastruttura per i mezzi di contromisura (Wikipedia EN: tubi esterni "REPS-324 Shlagbaum" per l'MG-74) | [DSRM971] media; [WPDecoy] bassa |
| **MG-34** | **Nessun dato pubblico trovato** (noisemaker a cartuccia, presunto) | — |
| **GIP / GPD** (generatori di bolle o rumore) | **Nessun dato pubblico trovato** sui nomi; il concetto (nube di bolle che dà eco e maschera) è documentato in generale | bassa |
| Limiti dichiarati delle esche sovietiche | "breve tempo di funzionamento", simulazione a bassa frequenza limitata, firma del proprio motore ed elica che le classifica come falsi bersagli | [CYB] media (riferito agli ultimi modelli) |

### 5.3 Efficacia per tipo di siluro (matrice qualitativa)

| Siluro / guida | Noisemaker statico (ADC Mk 2, MG-34) | Esca mobile (MG-74, MOSS) | Bolle (GPD) | Knuckle o cambio di quota |
|---|---|---|---|---|
| Passivo (ascolta il rumore) | **forte**: satura o attira | forte, se più rumorosa del battello | debole | medio (maschera brevemente) |
| Attivo (eco) | medio: eco o ripetitore puntuale, **Doppler zero** se staziona | **forte**: eco in movimento con Doppler plausibile | medio: falsa eco diffusa per 1–3 min | medio |
| Filoguidato con operatore (Mk48, TEST-71M) | debole: l'operatore vede una sorgente ferma che resta indietro rispetto alla TMA | medio: serve tempo per smascherarla | debole | debole |
| Segui-scia (53-65K, 65-76, USET-80 contro navi) | **nullo** | nullo | nullo | il cambio di rotta tardivo è l'unica difesa |

Fonti: principi da [TWZ] (verifica Doppler, jammer che riavvia l'algoritmo di verifica, ripristino dei siluri vecchi via filo da parte dell'operatore) e [PAT5235924] (rigetto per dimensione del bersaglio); valori qualitativi STIMA.

### 5.4 Tattiche combinate (complemento a doc 02 §6.4)

1. **Lancia l'ADC prima di cambiare tutto.** L'esca resta dove il siluro "si aspetta" il battello; poi knuckle con tutta la barra, aumento di velocità e cambio di quota oltre il layer. Lanci **sfalsati** (2–3 ADC a 30–60 s di distanza) allungano il mascheramento [TWZ] (media).
2. **Siluro attivo in arrivo:** porta l'esca o il knuckle **tra te e il siluro** e mettiti **al traverso** del siluro: il Doppler del battello rispetto al siluro scende verso zero, l'esca staziona, il siluro deve discriminare fra due echi simili. Non fermarti del tutto: un bersaglio a Doppler nullo vicino all'esca viene "unito" a essa nella verifica.
3. **Siluro passivo:** rallenta subito (5–8 kn) **dopo** il lancio dell'ADC, perché l'esca deve essere molto più rumorosa di te.
4. **Siluro filoguidato:** taglia il filo costringendo il lanciatore a manovrare (contro-tiro *snap shot*, doc 02) e solo dopo usa le esche; contro un operatore attento l'ADC da solo vale poco.
5. **Quota dell'esca:** impostala **sulla quota che stai lasciando** (dove il siluro ti cerca); se attraversi il layer, l'esca resta dall'altra parte del gradiente.

### 5.5 Esche e difese antisiluro delle navi di superficie (contro i siluri del giocatore)

| Sistema | Fatti | Fonte / conf. |
|---|---|---|
| **AN/SLQ-25 Nixie** (USA, adottato nel 1974) | Corpo rimorchiato con proiettore; mascheratore a larga banda più rumoroso della nave e **ripetitore del ping amplificato 2–3 volte**; singolo o in coppia; velocità di messa a mare e recupero 10–25 kn (consigliati ≤ 15); il siluro "gira finché esaurisce l'energia" | [FASSLQ25][TWZ][WARFOR] alta |
| T-Mk 6 "Fanfair" | predecessore elettromeccanico rimorchiato | [FASSLQ25] media |
| Type 182 (UK) | boa di 74 kg rimorchiata a **≈ 400 m**; rumore d'elica più eco CW | [WARFOR] media |
| **Udav-1 (RBU-12000)** | Razzi **111SG** (bombe di profondità), **111SZ** (barriera di "mine" con spoletta di prossimità acustica sulla rotta del siluro), **111SO** (esca acustica); portata 3.000 m, profondità 600 m, reazione 15 s; P dichiarata 0,9 contro siluri dritti e **0,76 contro siluri a autoguida**. Su Kirov (Frunze/Kalinin), Kiev, Kuznetsov, Udaloy | [WPRBU12000] media; date e unità al 1989 bassa |
| Esche rimorchiate sovietiche | **Non trovate** in fonti aperte per il 1989 | — |
| RBU-6000/1000 | uso antisiluro con salve sulla rotta prevista (vedi doc 01 §6.5) | media (concetto) |

**Vedere attraverso il Nixie con il Mk48 filoguidato (STIMA DI GIOCO):** la testa del Mk48 riporta via filo i rilevamenti di due sorgenti, la nave e il corpo rimorchiato a 300–600 m di poppa sulla stessa rotta. L'operatore riconosce la geometria fissa (la sorgente "più rumorosa" segue la nave alla sua stessa velocità) e ordina un'offset di prua di 5–10° verso la nave, abilitando la testa a meno di 1–2 km dal fianco. Senza filo (corsa autonoma) il Mk48 Mod 4 in passivo ha una P alta di inseguire il Nixie.

### 5.6 Contro-contromisure del siluro (logica)

- **Verifica del bersaglio:** dopo una scoperta il siluro conferma con più ping o ascolti (Doppler, intensità, coerenza del rilevamento). I jammer la riavviano di continuo con nuovi falsi bersagli [TWZ] (media).
- **Doppler:** è uno dei controlli della verifica [TWZ]. Un noisemaker che staziona dà eco a Doppler zero, il bersaglio vero no (se si muove a più di 3–5 kn, STIMA).
- **Dimensione o estensione:** un sottomarino dà un eco esteso con più "highlight" (100 m di scafo); un'esca è puntiforme. Un brevetto descrive il rigetto delle esche piccole: un errore di rilevamento voluto fa mancare l'esca e il siluro ricomincia a cercare [PAT5235924] (media).
- **Riattacco:** dopo un passaggio a vuoto il siluro ricerca a cerchio o a serpentina (doc 02 §6.2) finché ha autonomia.
- **Operatore via filo:** può ordinare un nuovo abilitamento, cambiare quota, ignorare un rilevamento e riassegnare [TWZ][doc 02] (media).

---

## 6. Aspetto visivo (periscopio e museo)

### 6.1 Che cosa disegnare e modellare

| Oggetto | Aspetto (STIMA basata su sequenze documentate e foto) | Durata a schermo |
|---|---|---|
| Affioramento della capsula Harpoon | Tubo nero od oliva di 0,53 m di diametro che emerge inclinato di ~45° per 2–3 m, ribollio bianco | 1–3 s |
| Separazione dei tappi | Due piccoli oggetti proiettati, spruzzo | < 1 s |
| Booster dell'Harpoon | Fiammata arancio-bianca, **scia di fumo bianco denso** che resta sospesa e si allarga (10–30 s di permanenza col vento) | boost 2,5–3 s; fumo 20–60 s |
| Harpoon in crociera | Corpo bianco o grigio di 3,8 m con ali a croce, a 5–15 m sull'acqua; turbogetto senza fumo visibile | visibile 1–3 km |
| Tomahawk da tubo o VLS | Esce **già spinto dal booster**: getto di vapore e acqua, poi fiammata lunga e fumo bianco-grigio per **12 s**; salita ripida, poi apertura di alette e ali, sgancio del booster (cilindro che ricade) e livellamento | 12–20 s |
| Tomahawk in crociera | Fusoliera cilindrica di 6,25 m (con booster), ali dritte a metà; nessun fumo | 1–3 km |
| Granit e Malakhit in arrivo | Grandi missili (10 m), alta quota → picchiata a 25 m; Granit con presa d'aria anulare in prua | — |
| Chaff sovietico (PK-2/PK-16/PK-10) | Esplosioni grigie a 100–300 m d'altezza attorno alla nave, nube che deriva | 30–120 s |
| AK-630 | Traccianti continui, rumore a "ronzio" | raffiche di 1–3 s |
| **Esche acustiche** | **Nessun indizio visivo** in immersione. Eccezione: ADC lanciato a quota periscopica e scie di bolle o ribollio in caso di generatore di gas vicino alla superficie (STIMA) | — |
| Nixie | Cavo che esce dallo specchio di poppa della nave; il corpo rimorchiato non si vede | — |

### 6.2 Foto di pubblico dominio (US Navy) su Wikimedia Commons

| Titolo | URL | Contenuto | Licenza |
|---|---|---|---|
| *Harpoon launched by submarine.jpg* | https://commons.wikimedia.org/wiki/File:Harpoon_launched_by_submarine.jpg | UGM-84 che esce dalla capsula all'affioramento, Pacific Missile Test Center, **1 aprile 1983**, DoD DNSC8307009, 3.000 × 2.653 px | PD (US Navy) |
| *UGM-84 Harpoon at Submarine Force Museum entrance.jpg* | https://commons.wikimedia.org/wiki/File:UGM-84_Harpoon_at_Submarine_Force_Museum_entrance.jpg | missile esposto (riferimento 3D per il museo) | verificare |
| *Tomahawk sublaunched.jpg* | https://commons.wikimedia.org/wiki/File:Tomahawk_sublaunched.jpg | Tomahawk lanciato da un 688 (fonte chinfo.navy.mil), bassa risoluzione 216 × 237 | PD (US Navy) |
| Categoria *Launches of BGM-109 Tomahawk missiles* | https://commons.wikimedia.org/wiki/Category:Launches_of_BGM-109_Tomahawk_missiles | molti lanci (navi, Florida 2003, HMS Astute 2011 ©Crown) | mista: verificare file per file |
| *RGM-84 Harpoon missile launch from USS Leahy (CG-16), in 1983.jpg* | https://commons.wikimedia.org/wiki/File:RGM-84_Harpoon_missile_launch_from_USS_Leahy_(CG-16),_in_1983.jpg | booster e fumo dell'Harpoon (lancio navale, utile come riferimento del fumo) | PD |
| Tomahawk dal **USS La Jolla (SSN-701), 14/4/1983** (NARA, via PICRYL) | https://picryl.com/media/a-tomahawk-cruise-missile-is-launched-from-the-nuclear-powered-attack-submarine-b455cf | **lancio da 688 d'epoca**: miglior riferimento per la scena | PD (US Navy, verificare su catalog.archives.gov) |
| Tomahawk in volo (dal USS Merrill, PMTC) | https://commons.wikimedia.org/wiki/File:An_air-to-air_side_view_of_a_BGM-109_Tomahawk_missile_in-flight._The_Tomahawk_was_launched_from_the_destroyer_USS_MERRILL_(DD-976)_on_the_Pacific_Missile_Test_Center_Range_-_DPLA_-_ccc23d9ffbd19292f36526be93c80e4e.jpeg | configurazione di crociera | PD |

---

## 7. Modello consigliato per il gioco

### 7.1 Macchine a stati

**A. Harpoon (UGM-84, tubo)**

```
TUBO_VUOTO → [carica capsula 120–240 s] → CARICATO
CARICATO → [preset: RBL/BOL, enable, schema, terminale; 30–90 s] → PRESETTATO
PRESETTATO → [power-up e allineamento 60–120 s; tenibile indefinitamente] → PRONTO_MISSILE
PRONTO_MISSILE + tubo allagato/equalizzato/porta aperta (60–120 s, transitori) → PRONTO_LANCIO
PRONTO_LANCIO → [FIRE] → CAPSULA_IN_SALITA (t = profondità/(v_salita·sin45°), v_salita 3 m/s ± 1)
  guasti: P = 0,03 + 0,05·max(0,(z−60)/10) + 0,10·[v>8 kn] + 0,10·max(0,SS−5)
CAPSULA_IN_SALITA → AFFIORAMENTO (1–3 s; evento visivo e acustico) → BOOST (3 s) → DISCESA (15 s) → CROCIERA
CROCIERA → [distanza di enable raggiunta] → RICERCA (RBL: cerchio di raggio R_s = 2–6 km; BOL: settore ±45°, fino a carburante)
RICERCA → AGGANCIO (sceglie l'eco più grande nel cancello; può prendere un neutrale) | PERSO (autodistruzione o ammaraggio)
AGGANCIO → TERMINALE (radente o pop-up) → risoluzione difese (§4.4b) → COLPO | ABBATTUTO | SEDOTTO
```

**B. Tomahawk**

```
TLAM: MISSIONE (dalla lista di missioni del briefing, con launch basket) → [battello entro il basket, ≤ 8 kn, 20–40 m (STIMA)]
      → PREPARAZIONE 60 min (riducibile a 20 min se "hot" da missione precedente, STIMA) → PRONTO
TASM: dati OTH (ricevuti a quota periscopica: età del dato) → piano di ricerca → PREPARAZIONE 15–20 min (STIMA) → PRONTO
PRONTO → FIRE → BOOST SUBACQUEO+AEREO 12 s → TRANSIZIONE (ali, presa, motore; 5% guasto [GAO95]) → CROCIERA 245 m/s
TLAM: landfall → TERCOM (P_fix 0,95 su terreno ondulato; 0,80 su pianura) → DSMAC → IMPATTO (P_aimpoint 0,75 [GAO95])
TASM: ricerca a serpentina; aggancio sul primo eco "plausibile" (ESM che preferisce emettitori militari) →
      P(bersaglio designato) ≈ 0,15–0,5 in base all'età dei dati e alla densità del traffico (vedi [NG2])
```

**C. Contromisura (ADC Mk 2)**

```
EIETTORE_CARICO → [FIRE ADC; quota impostata 30 m ↔ profondità del battello] → STAZIONAMENTO (raggiunge la quota in 20–40 s)
STAZIONAMENTO → ATTIVO (durata 6–15 min STIMA) → ESAURITO (affonda)
EIETTORE_VUOTO → [ricarica 60–120 s STIMA] → EIETTORE_CARICO
```

### 7.2 Tabelle parametri

**Missili del giocatore**

| Parametro | Harpoon UGM-84A/C | TASM UGM-109B | TLAM-C UGM-109C | Fonte |
|---|---|---|---|---|
| Portata da sottomarino | 110 km (60 nmi) | 460 km (efficace 250 nmi) | ≈ 875 km (1.250 −30%) | doc 01, [NG2], [GAO95] |
| Velocità | 240 m/s | 245 m/s | 245 m/s | [WPHarp][WPTom] |
| Quota di crociera | 15 m (A) / 5 m (C) | 5–10 m | 30–50 m sul terreno | STIMA / [WPTom] |
| Quota di lancio | 20–60 m | 20–60 m (tubo), 20–40 m (VLS) | idem | **STIMA** |
| Velocità di lancio | ≤ 8 kn (12 con penalità) | ≤ 8 kn | ≤ 8 kn | **STIMA** |
| Tempo ordine → lancio | 3–5 min (1–2 se pre-allineato) | 15–20 min | ≈ 60 min | STIMA / [GAO95] |
| Accensione seeker | 5–20 km dal punto previsto | in ricerca | — | STIMA |
| Testata | 221 kg | 454 kg | 454 kg | doc 01 |
| Indizio di contro-scoperta | capsula, fumo, datum | idem più booster di 12 s | idem | §1.7 |

**Missili nemici**

| Parametro | Malakhit (Charlie II) | Granit (Oscar) | Vodopad 83R (Victor III, Akula) | Vyuga (Alfa, Victor) |
|---|---|---|---|---|
| Quota di lancio | ≤ 50 m | 10–30 m (STIMA) | ≤ 150 m | 50–60 m |
| Salva | 8 insieme | fino a 24, 1 ogni 3–5 s (STIMA) | 1–2 | 1 |
| Portata | 120 km | 550 km (145 tutta bassa) | 50 km (35 da 150 m) | 42 km |
| Dati | sonar proprio (CZ) | aereo o satellite (ELINT nel 1989) | sonar proprio | sonar proprio |
| ROE | convenzionale o nucleare | convenzionale o nucleare | **convenzionale** (83R) | **solo nucleare** |

**Contromisure**

| Parametro | ADC Mk 2 (USA) | MG-74 (URSS) | Nixie (navi USA) | Udav-1 (navi URSS) |
|---|---|---|---|---|
| Lancio | ICL da 3", ricaricabile | tubo da 533 (occupa un posto arma) | rimorchiato, continuo | razzi da lanciatore a 10 canne |
| Dotazione | 2 eiettori (bassa), 12–16 ADC (STIMA) | 2–4 al posto dei siluri (STIMA) | 1–2 corpi | 1 lanciatore (Frunze/Kalinin) |
| Comportamento | staziona alla quota impostata (≤ 180 m, STIMA da [NSL07]) | si muove a 8–12 kn (STIMA) con manovre programmate, ≤ 250 m [KREM] | segue la nave a 300–600 m (STIMA) | barriera a ≤ 3 km |
| Durata | 6–15 min (STIMA, [TWZ] 6–45 min per la categoria) | 15–30 min (STIMA) | illimitata | istantanea |
| Livello acustico | +20–30 dB rispetto al battello silenzioso nella banda del seeker (STIMA, doc 01 §6.6) | rumore del battello a 8–12 kn ± eco simulato | più rumoroso della nave più ripetitore ×2–3 | — |
| Effetto | vedi §5.3 e §7.3 | vedi §5.3 | P_seduzione 0,7 contro un siluro autonomo, 0,25 contro un filoguidato con operatore (STIMA) | P_kill 0,76 contro un siluro a autoguida [WPRBU12000] (in gioco: 0,5, STIMA prudente) |

### 7.3 Regole di successo e fallimento (contromisure)

Per ogni siluro in fase di ricerca o aggancio, a ogni tick in cui un'esca è entro il cono del seeker:

```
P_seduzione = base[tipo_esca][modo_siluro] × f_geom × f_gen × f_filo
base (passivo / attivo):  noisemaker 0,7 / 0,4;  esca mobile 0,6 / 0,7;  bolle 0,1 / 0,4;  scia: 0
f_geom  = 1,0 se l'esca è più vicina al siluro del battello; 0,5 se il battello è più vicino;
          ×1,3 se il battello ha Doppler < 3 kn rispetto al siluro (al traverso) MA > 0 (in movimento)
          ×0,5 se il battello è fermo e a meno di 300 m dall'esca (fusione dei bersagli)
f_gen   = SET-65 1,2 | SAET-60 1,1 | TEST-71M 1,0 | USET-80 0,8 | Mk48 Mod 4 0,8 | ADCAP 0,5
f_filo  = 0,4 se il filo è integro e l'operatore è attivo (giocatore o IA "veterana"), altrimenti 1,0
Dopo la seduzione: il siluro insegue l'esca; al passaggio o all'esaurimento → riattacco (ricerca a cerchio)
con P_riacquisizione normale se il battello è entro la portata d'acquisizione e sopra la soglia.
```

Tutti i coefficienti sono **STIMA DI GIOCO**, ordinati qualitativamente secondo §5.3 e §5.6.

### 7.4 Uso da parte dell'IA

| Piattaforma IA | Comportamento con i missili | Comportamento con le contromisure |
|---|---|---|
| **Charlie II** | Con un bersaglio di superficie in CZ (55 o 110 km) e ROE di attacco: quota 50 m, ≤ 6 kn, **salva unica di 8** su un gruppo; poi sprint di disimpegno | MG-74 se ha un siluro attivo in coda (STIMA) |
| **Oscar II** | Solo con dati esterni (scenario: aereo Tu-95RTs presente o nave che emette per l'ELINT); salva di 12–24 da 300–500 km | MG-74 o MG-104 |
| **Victor III / Akula** | Se mantiene un contatto con il 688 a 10–50 km senza soluzione da siluro buona: **83R Vodopad** (anche da 150 m). Si fa precedere da una TMA di 5–10 min | MG-74; knuckle e cambio di quota; Akula 1989: lanciatori esterni solo da K-391 (bassa) |
| **Alfa** | Nessun missile convenzionale; Vyuga solo con ROE nucleari | Sprint a 40 kn e quota profonda (oltre l'inviluppo del Mk48 Mod 4), MG-74 dal tubo (STIMA) |
| **Navi sovietiche** | — | Dopo aver scoperto il lancio di un missile: chaff (PK) a 20–30 km, SAM all'ingresso nell'inviluppo, AK-630 sotto i 4 km. Contro i siluri: virata a 90° e accelerazione, RBU-6000 sulla rotta del siluro, Udav-1 se presente |
| **Navi USA (scorta)** | — | Nixie sempre filato in zona di minaccia sottomarina (velocità ≤ 15 kn per la messa a mare) |

### 7.5 Lacune principali (ordine d'impatto sul gioco)

1. **Profondità e velocità di lancio** dell'UGM-84 e dell'UGM-109 da tubo: non pubbliche (→ STIMA 20–60 m, ≤ 8 kn).
2. **Tempi di preparazione** dell'Harpoon (accensione e allineamento) e del TASM: non pubblici. Per il TLAM esiste solo il "circa 1 h" del 1991 [GAO95].
3. **ADC Mk 2:** livello sonoro, banda, durata, quote, data di servizio, numero imbarcato sul 688 e numero di eiettori: **classificati**.
4. **MG-34, GIP/GPD:** nessun dato pubblico, nemmeno nominale affidabile.
5. **MG-74:** velocità e durata non pubbliche (noti solo calibro, lunghezza, quota massima 250 m e modi).
6. **Geometria della ricerca RBL dell'Harpoon** e dimensione dei launch basket TLAM: fonti in disaccordo o assenti.
7. **Pk delle difese di punto** contro missili subsonici radenti: nessun dato aperto per sistema; c'è solo il dato aggregato di Schulte.
8. **Ritmo di lancio del Granit** e meccanica reale della salva in rete (le fonti stesse esprimono dubbi).
9. **Vodopad:** tempo di volo e velocità del vettore.
10. Verifica su testo (con pagina) di Friedman *U.S. Submarines Since 1945* e *World Naval Weapon Systems 1989* per il carico tipico 1989, gli eiettori di segnali del 688 e l'inviluppo di lancio delle capsule.

---

## 8. Fonti

- `[USNI77]` USNI *Proceedings*, febbraio 1977, "Harpoon": https://www.usni.org/magazines/proceedings/1977/february/harpoon (403: **solo estratto via motore di ricerca**: espulsione a ≈ 30 ft/s, alette, tappi all'affioramento)
- `[DS84]` designation-systems.net, AGM/RGM/UGM-84: https://designation-systems.net/dusrm/m-84.html
- `[WPHarp]` Wikipedia, *Harpoon (missile)*: https://en.wikipedia.org/wiki/Harpoon_(missile)
- `[PAT6763289]` brevetto USA 6763289 (modi RBL/BOL/LOS del Block I): https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6763289
- `[PAT4147124]` brevetto USA 4147124 (capsula galleggiante): https://patents.justia.com/patent/4147124
- `[WPExo]` Wikipedia, *Exocet* (SM39): https://en.wikipedia.org/wiki/Exocet
- `[NSLOS]` Naval Submarine League, "OUTLAW SHARK: the beginning of third party targeting at sea" (2012): https://archive.navalsubleague.org/2012/outlaw-shark-1-the-beginning-of-third-party-targeting-at-sea
- `[NSLCCS]` Naval Submarine League archive (Mk 117 e CCS Mk 1): https://archive.navalsubleague.org/?p=2022 (via estratto)
- `[NSL07]` J.H. Patton, NSL, gennaio 2007 (espendibili per il signal ejector da 3"): https://archive.navalsubleague.org/?p=4702
- `[DS109]` designation-systems.net, BGM/UGM-109: https://designation-systems.net/dusrm/m-109.html
- `[WPTom]` Wikipedia, *Tomahawk (missile)*: https://en.wikipedia.org/wiki/Tomahawk_(missile)
- `[FASTom]` FAS, BGM-109: https://man.fas.org/dod-101/sys/smart/bgm-109.htm
- `[NG1]` `[NG2]` Naval Gazing, *Tomahawk Part 1/2*: https://www.navalgazing.net/Tomahawk-Part-1 , https://www.navalgazing.net/Tomahawk-Part-2
- `[GAO95]` GAO/NSIAD-95-116, *Cruise Missiles: Proven Capability Should Affect Aircraft and Force Structure* (1995): https://www.govinfo.gov/content/pkg/GAOREPORTS-NSIAD-95-116/html/GAOREPORTS-NSIAD-95-116.htm
- `[FASCMSA]` FAS IRP, Cruise Missile Support Activity: https://irp.fas.org/agency/dod/usjfcom/cmsa
- `[REU76]` Reuters/Screenocean, "USA: first underwater launch and boost/glide test" (1976): https://reuters.screenocean.com/record/1062015 (via estratto)
- `[PAT6427574]` brevetto USA 6427574 (capsula orizzontale TACTOM): https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6427574
- `[WP688]` Wikipedia, *Los Angeles-class submarine*: https://en.wikipedia.org/wiki/Los_Angeles-class_submarine ; `[FAS688]` https://man.fas.org/dod-101/sys/ship/ssn-688.htm
- `[WPProv]` Wikipedia, *USS Providence (SSN-719)*: https://en.wikipedia.org/wiki/USS_Providence_(SSN-719)
- `[RWVyuga]` Wikipedia RU, *Вьюга (ракетный комплекс)*: https://ru.wikipedia.org/wiki/Вьюга_(ракетный_комплекс) ; Wikipedia EN *RPK-2 Vyuga* (inaffidabile: confonde carichi e piattaforme)
- `[RWVodopad]` Wikipedia RU, *Водопад (ракетный комплекс)*: https://ru.wikipedia.org/wiki/Водопад_(ракетный_комплекс)
- `[DSRM705]` `[DSRM671]` `[DSRM971]` deepstorm.ru, schede Pr.705, 671RTM, 971: https://www.deepstorm.ru/DeepStorm.files/45-92/nts/705/list.htm , …/671RTM/list.htm , …/971/list.htm
- `[NPK206]` Navypedia, K-206 (Pr.949A): https://navypedia.org/ships/russia/ru_ss_k206.htm
- `[WPRK55]` Wikipedia, *RK-55*: https://en.wikipedia.org/wiki/RK-55
- `[RWP700]` Wikipedia RU, *П-700 «Гранит»*: https://ru.wikipedia.org/wiki/П-700_«Гранит» ; `[WPGranit]` https://en.wikipedia.org/wiki/P-700_Granit ; `[NVO2000]` NVO, 7/4/2000: https://nvo.ng.ru/armament/2000-04-07/6_granite.html
- `[WPP70]` Wikipedia, *P-70 Ametist*: https://en.wikipedia.org/wiki/P-70_Ametist
- `[RWP120]` Wikipedia RU, *П-120* e *Подводные лодки проекта 670М «Чайка»*; deepstorm Pr.670M (via estratti): https://ru.wikipedia.org/wiki/П-120 , https://www.deepstorm.ru/DeepStorm.files/45-92/nsrs/670m/list.htm
- `[WPLeg]` Wikipedia, *Legenda (satellite system)*: https://en.wikipedia.org/wiki/Legenda_(satellite_system) ; `[WPUSA]` Wikipedia, *US-A*: https://en.wikipedia.org/wiki/US-A ; dfnc.ru (Uspekh e Legenda): https://dfnc.ru/en/kosmos/all-seeing-space-based-reconnaissance-and-targeting-system/
- `[NWPhal]` NavWeaps, Phalanx: https://www.navweaps.com/Weapons/WNUS_Phalanx.php
- `[NWAK630]` NavWeaps, AK-630: https://www.navweaps.com/Weapons/WNRussian_30mm-54_ak-630.php
- `[WPKinz]` Wikipedia, *3K95 Kinzhal*: https://en.wikipedia.org/wiki/3K95_Kinzhal
- `[RWOsaM]` Wikipedia RU, *Оса-М*: https://ru.wikipedia.org/wiki/Оса-М
- `[RWPK10]` ruwiki, *ПК-10* (via estratto): https://ru.ruwiki.ru/wiki/ПК-10
- `[DRAS]` Cmde B.R. Prakash, "Analysis of missile effectiveness: a historical perspective" (dati Schulte aggiornati): https://dras.in/analysis-of-missile-effectiveness-a-historical-perspective/
- `[SCH94]` J.C. Schulte, *An Analysis of the Historical Effectiveness of Anti-Ship Cruise Missiles in Littoral Warfare*, NPS 1994: https://calhoun.nps.edu/entities/publication/fa49adf3-5a8e-4e5e-8aca-5aad228317ab (non letta integralmente)
- `[HUG95]` W.P. Hughes Jr., "A Salvo Model of Warships in Missile Combat Used to Evaluate Their Staying Power", *Naval Research Logistics* 42 (1995) 267–289 (forma delle equazioni; *n.v.* sul testo) ; Lucas & McGunnigle, NRL 50 (2003) 197–217
- `[GOVT]` Avvisi di gara USN, *ADC MK 2 MOD 4 & MOD 5*: https://govtribe.com/opportunity/federal-contract-opportunity/acoustic-device-countermeasure-adc-mk-2-mod-4-mod-5-n0025315r0005 ; https://www.highergov.com/contract-opportunity/acoustic-device-countermeasure-adc-mk-2-mod-4-mo-n00024-14-r-6234-k-22d06/
- `[PAT6341101]` brevetto USA 6341101 (lanciatori esterni da 6", CSA Mk 2): https://patents.google.com/patent/US6341101
- `[SUBSIM]` forum SUBSIM, thread 247148 (CSA Mk II, conteggi; senza fonti citate): https://www.subsim.com/radioroom/showthread.php?t=247148
- `[TWZ]` The War Zone, "The Shadowy World of Submarine and Ship-Launched Torpedo Countermeasures": https://www.twz.com/33467/the-shadowy-world-of-submarine-and-ship-launched-torpedo-countermeasures-an-explainer
- `[WPMOSS]` Wikipedia, *Mobile submarine simulator*: https://en.wikipedia.org/wiki/Mobile_submarine_simulator
- `[WPDecoy]` Wikipedia, *Sonar decoy*: https://en.wikipedia.org/wiki/Sonar_decoy
- `[KREM]` scheda MG-74E (brochure da esportazione, riportata): http://kremalera.narod.ru/mg74.htm
- `[WARFOR]` "Гидроакустическое противодействие": https://warfor.me/gidroakusticheskoe-protivodeystvie/
- `[CYB]` "Совершенствование имитаторов подводных лодок", CyberLeninka: https://cyberleninka.ru/article/n/sovershenstvovanie-imitatorov-podvodnyh-lodok
- militaryrussia.ru, MG-74 (topic-602) e Vyuga (topic-447): pagine **non raggiungibili** in questa sessione (404); citate solo tramite estratti dei motori di ricerca
- `[FASSLQ25]` FAS, AN/SLQ-25 Nixie: https://man.fas.org/dod-101/sys/ship/weaps/an-slq-25.htm
- `[WPRBU12000]` Wikipedia, *RBU-12000 (Udav-1)*: https://en.wikipedia.org/wiki/RBU-12000
- `[PAT5235924]` brevetto USA 5235924, *Anticountermeasure system for torpedoes*: https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5235924
- Foto: vedi §6.2 (Wikimedia Commons, PICRYL/NARA)
