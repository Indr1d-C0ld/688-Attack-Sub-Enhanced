# 23 — Episodi della Guerra fredda per una libreria di missioni (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**, con episodi dal 1960 al 1992 come fonte d'ispirazione. Il giocatore comanda sempre un
> **688 Flight I** (lato USA) o un **Alfa, Progetto 705** (lato URSS).
> Scopo: la base storica per una libreria di missioni pronte, fatte con l'editor, ispirate a fatti reali dei due blocchi,
> più alcuni scenari del tutto originali. Il dossier dice quali episodi e quali schemi operativi diventano buone missioni,
> che cosa è successo davvero, che cosa è noto e che cosa è controverso, come si traduce nei teatri e nelle classi del
> gioco e che cosa manca al motore.
> Compilato: 25 ottobre 2026, da una sola ricerca (fonti web e file del progetto). Nessun file del progetto è stato
> modificato oltre a questo.

---

## 0. Convenzioni e punto di partenza

**Confidenza** (per ogni affermazione o scheda):
- **alta:** documento primario o ufficiale (cablogrammi e diari di bordo declassificati, CIA *Studies in Intelligence*,
  rapporti d'inchiesta), oppure fatto confermato da entrambe le parti;
- **media:** fonte secondaria seria (*Naval War College Review*, *Proceedings*, *Blind Man's Bluff*, Wikipedia con
  fonti), o fatto riferito da una sola parte ma non smentito;
- **bassa:** testimonianza isolata, stampa d'epoca non verificata, tesi contestata, blog;
- **n.v.:** non trovato.

**Marcature:** **STIMA** = valore proposto, da tarare; **[GIOCO]** = proposta di design; **[SOST]** = sostituzione di
luogo o di classe rispetto al fatto reale; **[MOTORE+]** = funzione che il motore non ha (o non ha ancora).

**Sigle delle fonti** tra parentesi quadre; la tabella con gli URL è al §8.

### 0.1 Che cosa si può giocare oggi

**Teatri** (`data/motore/teatri/*.fdl`; riquadri dal dossier 03, tabella finale) `[D03]`:

| Teatro | Riquadro | Che cosa contiene di utile per gli episodi |
|---|---|---|
| `faroe` | 60,5–63,5 N; 10–4 W | cresta Islanda-Faroe e canale Faroe-Shetland (barriera GIUK, SOSUS), fiordi delle Faroe |
| `barents` | 69,2–72 N; 31–39 E | approcci alla penisola di Kola, isola di Kildin (69,3 N 34,1 E), costa del Murman, notte polare |
| `gibilterra` | 35–36,8 N; 7,5–3,5 W | soglia di Camarinal (284 m), Capo Spartel, correnti e onde interne |
| `otranto` | 39,5–42,5 N; 17–20,5 E | canale d'Otranto, **baia di Valona e Pasha Liman** (40,4 N 19,5 E), bocche di Cattaro al margine N |
| `kunashir` | 43,3–45,6 N; 144,8–148,5 E | stretti delle Curili meridionali (Ekaterina, Izmeny), bacino delle Curili oltre 3000 m, ghiaccio d'inverno |
| `celtica` | 47–49,5 N; 12–7,5 W | ciglio della piattaforma, canyon, piana abissale (4000–4800 m): l'«Atlantico aperto» del gioco |
| `baltico` | 54–55,8 N; 10,5–15 E | stretti danesi, Fehmarn, Bornholm, Simrishamn (55,6 N 14,4 E); fondali di 8–50 m |

Di conseguenza quasi ogni episodio richiede una **[SOST]** di luogo: Karlskrona (56,1 N 15,7 E) e Hårsfjärden
(59 N 18 E) stanno appena o molto fuori dal Baltico del gioco; Petropavlovsk e il sito di Ivy Bells (57,6 N 155,7 E)
fuori da Kunashir; Bermuda, Azzorre, Cuba, Creta e il Golfo Persico fuori da tutto. La regola proposta: **teatro più
vicino per ambiente fisico** (profondità, ghiaccio, strettoia, costa ostile), non per geografia, e briefing che dice
«ispirato a».

**Classi** (`data/motore/classi.toml`):
- navi NATO: Perry, Kidd, Spruance, Iowa, Nimitz, Sacramento;
- navi sovietiche: Krivak II, Kara, Kirov, Slava, Koni, Nanuchka I;
- civili: petroliera, mercantile, portacontainer;
- sottomarini: Los Angeles, Sturgeon, Trafalgar; Alfa, Victor III, Kilo, Akula I, Oscar II, Charlie II, Typhoon,
  Foxtrot;
- aerei: P-3C, S-3A/B, SH-60B, SH-60F, SH-3H; Il-38, Tu-142MK, Be-12, Ka-25PL, Ka-27PL;
- missili: Tomahawk, Harpoon, P-700, bersaglio BQM-74.

Mancano, e servono spesso: **SSBN Yankee e Delta** (sostituto: Typhoon, troppo silenzioso e troppo grande),
**SSGN Echo II** (sostituto: Charlie II), **piccole unità ASW di paesi neutrali** (sostituti: Koni, Nanuchka, SH-60B
in parte «neutrale»), **rimorchiatori e navi di ricerca** (sostituto: mercantile), **sottomarini diesel occidentali**
(sostituto: Foxtrot o Kilo con parte cambiata).

**Le 22 missioni originali** (titoli dagli header di `data/missioni/SCENE*.lua`, trame da `[ORIG-BR]`), da non
duplicare:

| Missione | Teatro | Trama |
|---|---|---|
| TORPEX '89 (1A-1C) | faroe | esercitazione contro cacciatorpediniere bersaglio; gara contro il *Dallas* |
| Scrollatevelo di dosso (2A-2C) | faroe | pace: il 688 deve scrollarsi di dosso un Alfa senza sparare; l'Alfa deve restargli entro 5 nm |
| Operazioni subacquee (3B) | barents | Alfa: pattuglia la costa di Polyarny, trova l'intruso, ping e accompagnalo fuori |
| Mumar Cadaver (4A) | gibilterra | affondare quattro petroliere libiche scortate da Koni |
| Fuga (5A-5C) | gibilterra | il 688 esce dal Mediterraneo superando due sottomarini sovietici |
| Gulasch (6A) | otranto | scorta di mercantili verso la Jugoslavia contro due navi sovietiche |
| Passerella (7A-7C) | kunashir | guerra: affondare un Typhoon scortato al passo / l'Alfa protegge il Typhoon |
| Festa a sorpresa (8A-8C) | celtica | esploratore di un gruppo di superficie verso il Mar di Norvegia / l'Alfa lo caccia |
| Ritorno a casa (9A-9C) | celtica | scorta di un convoglio verso il Golfo di Biscaglia / l'Alfa lo attacca |
| Colpisci e fuggi (0A) | baltico | guerra: Tomahawk contro bersagli a terra, poi fuga verso Copenaghen |

Quindi sono già coperti: caccia al boomer in guerra a Kunashir, scorta di convoglio in guerra, scrollarsi un
pedinatore in pace, difesa costiera dell'Alfa a Polyarny. **Non** sono coperti: raccolta d'informazioni, pedinamento
di SSBN in pace, cavi, mine, soccorso e recupero, incursioni in acque neutrali, crisi con ROE che cambiano, barriera
SOSUS, pedinamento di portaerei.

**Che cosa sa esprimere l'editor** (`engine/missioni/include/missioni/scenario_missione.hpp` `[SCEN]`, `docs/09` `[D09]`):
- teatro, data, ora, meteo realistico; parte del giocatore (688 o Alfa); durata;
- **situazione** `pace` / `crisi` / `guerra`: nello script diventa `ARMI = 0` (armi dell'IA trattenute), `1` (l'IA
  spara solo dopo un lancio del giocatore), `2` (guerra) `[SCEN]`;
- unità con rotta (ciclica, uscita), comportamento della libreria 2 con `paranoia`, `salve`, `portata_nm`, quota
  d'evasione, unità **nascoste** al briefing e unità **di riserva** che entrano quando scatta una regola (`entra_con`);
- obiettivi: `affondare`, `proteggere` (anche «non colpire»), `raggiungere` (cerchio ed entro_s), `scoprire`
  (classificare), `non_essere_scoperto`, `sopravvivere`, `variabile`, `esito`, `minare` (Enhanced) `[D09]`; ordini che
  arrivano **dalla sala radio** (`da_radio`) e valgono solo se ricevuti;
- aerei ASW con base, zona, situazione (in pace boe SUS al posto delle armi; in crisi solo rapporti) e **datum**
  (anche da SOSUS, con ritardo) `[D09]`;
- regole «quando… allora…»: condizioni su tempo, unità colpita o uscita, giocatore dentro o fuori una zona, unità in
  zona, **allarme**, giocatore ha lanciato, variabili, regola già scattata; azioni: messaggio, fine con esito,
  variabile, cambio di comportamento, vai a, entra, datum ASW, allarme;
- carico: dotazione, deposito, tubi, tipo di missione (`antisom`, `antinave`, `attacco_terra`, `minamento`, `scorta`),
  **ospiti** (incursori, 6 posti per squadra), carriera con `operazione_speciale` e `artico` `[D09]`.

**Sistemi già presenti o in corso** (rimandi): ASW aereo (`docs/13`), periscopi (`docs/14`), meteo e mare con nebbia e
ghiaccio (`docs/15`), schiera rimorchiata (`docs/07`), artiglieria contro sottomarini (`docs/16`, in corso), collisioni
(`docs/ricerca/16`, in corso), mine ormeggiate e posa di mine (roadmap), sala radio (dossier 10), carriera e decorazioni
(dossier 13).

### 0.2 Un vincolo da dire subito: l'Alfa reale

Le sette unità del Progetto 705/705K furono tutte della **Flotta del Nord**, quasi sempre tenute in porto pronte a uno
scatto verso l'Atlantico; cinque furono radiate il 19 aprile 1990 `[WP-ALFA]` (media). Le esercitazioni mostrarono che
sfuggivano ai siluri dell'epoca, il che spinse l'ADCAP e lo Spearfish `[WP-ALFA]` (media). Non risultano episodi
documentati di un Alfa in Baltico, Mediterraneo o Pacifico; anche gli aneddoti russi su pedinamenti di sottomarini NATO
per molte ore **non** sono stati trovati in fonti verificabili (n.v.). Il gioco originale mette già l'Alfa a Kunashir e
a Gibilterra; la libreria fa lo stesso, ma il briefing dell'Alfa fuori dal Nord deve essere un «what if» dichiarato. Gli
episodi sovietici reali (diesel, Victor, Yankee) diventano missioni dell'Alfa **per ruolo**, non per identità.

---

## 1. Sintesi

1. **Gli episodi migliori per il gioco sono quelli di pace e di crisi**, non di guerra: pedinamento ravvicinato, raccolta
   d'informazioni presso le basi, attraversamento di barriere, incursioni in acque neutrali. La tensione nasce da
   **farsi scoprire**, dalle **ROE che vietano di sparare**, dal **fondale basso** e dalla **collisione**. La Guerra
   fredda subacquea conta almeno otto collisioni tra sottomarini o con navi tra il 1969 e il 1993 (§2) e nessun siluro
   sparato tra le due superpotenze.
2. **I fatti certi sono pochi e molto concreti.** Per gli episodi chiave esistono documenti primari: i cablogrammi e i
   diari di bordo della crisi di Cuba `[NSA75]`, la storia interna della CIA su Azorian `[CIA-GLO]`, il cablogramma
   del 1974 sulla collisione all'uscita di Holy Loch `[WP-JM]`, le inchieste svedesi `[WP-SWE]`. Le operazioni di
   spionaggio USA (Holy Stone, Ivy Bells, i cavi del Barents) restano note quasi solo da *Blind Man's Bluff* `[BMB]`.
3. **Molti episodi sono contesi** e vanno trattati come tali: la nazionalità dei sottomarini in Svezia e in Norvegia;
   il «quasi lancio» nucleare del B-59; la presunta collisione Augusta/K-219; le cause di Scorpion e di K-129; la
   posizione del Kostroma a Kildin. La libreria deve dirlo nel briefing o usare nomi di fantasia.
4. **Gli schemi ricorrenti** (§3) valgono più dei singoli episodi: dodici schemi coprono tutto il catalogo e si
   prestano anche a scenari originali.
5. **Il motore copre già gran parte del necessario** (zone, regole, riserve, datum SOSUS, ordini dalla radio, posa di
   mine, ospiti incursori). Mancano soprattutto: condizioni su **quota, velocità e alberi** del giocatore, un obiettivo
   «**mantieni il contatto per T minuti**», **acque territoriali** con penalità di carriera, **cambio di ROE a metà
   missione**, unità IA **in emersione forzata o in avaria** (rimorchio, soccorso), **forze neutrali** con ROE proprie,
   le collisioni (dossier 16) e i segnali di superficie non letali (§4).
6. **La shortlist** (§6) propone 23 missioni: 11 per il 688, 10 per l'Alfa, 2 con le due varianti; 4 di guerra, 7 di
   crisi, 12 di pace o esercitazione; tutti e sette i teatri.

---

## 2. Catalogo degli episodi

Ogni scheda: **quando e dove** (con il teatro del gioco), **unità** (con le classi), **fatti** passo per passo,
**noto / conteso**, **esito**, **perché giocarlo**, **giocabilità** (parte, teatro, ROE, compito, tensione, motore).
Ordine cronologico. Le schede marcate «ispirazione» sono guerre fuori dai due blocchi, utili per la dottrina.

### E01 — Valona, Pasha Liman (marzo-giugno 1961)
- **Dove:** baia di Valona, Albania → **`otranto`** (dentro il riquadro, senza [SOST]).
- **Unità:** sommergibili Whiskey sovietici, nave appoggio; forze albanesi → `foxtrot`, `mercantile`.
- **Fatti:** dopo la rottura tra Hoxha e Chruščëv l'Albania esce di fatto dal Patto di Varsavia; la base sovietica di
  Pasha Liman diventa il punto caldo. Le truppe albanesi bloccano la base; i sovietici ritirano navi e sommergibili;
  l'Albania trattiene **quattro** Whiskey che considerava suoi. Le ultime navi sovietiche partono a fine maggio, gli
  ultimi uomini il 5 giugno 1961 `[WP-PL]` `[HIM]`.
- **Noto / conteso:** il fatto e i quattro battelli trattenuti sono certi (media); il numero totale (12?) e i dettagli
  dello stallo vengono da fonti di parte o deboli (bassa) `[HIM]` `[HOX]`.
- **Perché giocarlo:** una crisi **senza nemico dichiarato**: un ex alleato, una baia stretta, gli osservatori NATO
  fuori. È anche l'unico episodio vero che cade dentro un teatro del gioco così com'è.
- **Giocabilità:** [GIOCO] Alfa, `otranto`, **crisi**. Trasposto al 1989 come «what if»: un paese cliente espelle la
  flotta. L'Alfa scorta fuori dalla baia una nave appoggio (mercantile) e quattro Foxtrot; i battelli «trattenuti»
  (Foxtrot in parte neutrale) restano ormeggiati, un 688 e un P-3C osservano dal canale. Tensione: acque basse, ordine
  di non sparare a nessuno, un Foxtrot che si stacca dal convoglio (regola a tempo). Motore: c'è tutto; servirebbe una
  **parte «terza»** distinta da neutrale (ostile all'URSS ma non alla NATO) [MOTORE+].

### E02 — Cuba: i Foxtrot dell'operazione Kama (ottobre-novembre 1962)
- **Dove:** Mar dei Sargassi e approcci a Cuba → **`celtica`** [SOST] (acqua profonda e aperta).
- **Unità:** B-4, B-36, B-59, B-130 (Foxtrot, ciascuno con un siluro nucleare); gruppi della USS *Randolph*,
  cacciatorpediniere *Beale*, *Cony*, *Blandy*, *Cecil*; pattugliatori → `foxtrot`; `nimitz`, `spruance`, `perry`;
  `p3c`, `sh3h`, `s3a` `[NSA75]`.
- **Fatti:**
  1. il 1° ottobre i quattro battelli lasciano il Nord diretti a Mariel; il 23-24 ottobre McNamara autorizza le
     «procedure di emersione e identificazione» con **bombe di profondità da esercitazione** (PDC) e le notifica a Mosca;
     l'ordine è di non attaccare `[NSA75]` (alta);
  2. il B-130 (Šumkov) ha i diesel in avaria, è avvistato dagli aerei il 25-26 e riemerge la sera del 29 dopo circa
     14 ore di contatto continuo; poi è rimorchiato verso Murmansk `[NSA75]` (alta);
  3. il B-59 (Savickij), tracciato col MAD dal 26, riemerge la sera del 27 tra i cacciatorpediniere, con batterie quasi
     scariche, caldo e CO₂ alti a bordo `[NSA75]` `[WP-B59]` (alta);
  4. il B-36 (Dubivko) manovra in modo evasivo e lancia falsi bersagli almeno tre volte; riemerge il 31 per
     esaurimento dell'autonomia subacquea `[NSA75]` (alta);
  5. il B-4 (Ketov) non viene mai confermato `[NSA75]` (alta).
- **Noto / conteso:** il racconto di Orlov sull'ordine di preparare il siluro nucleare sul B-59 e sul no di Archipov è
  **controverso**; altri comandanti ne dubitano `[NSA75]` `[WP-B59]` (bassa-media).
- **Esito:** tre su quattro costretti in superficie senza un colpo sparato.
- **Perché giocarlo:** è il modello della **«caccia fino all'esaurimento»** (hunt to exhaustion) e dei segnali non letali.
- **Giocabilità:** [GIOCO] 688, `celtica`, **crisi**: con un P-3C e due fregate, tenere il contatto su un diesel finché
  riemerge; vietato lanciare; segnali col sonar attivo. Il lato sovietico è già simile a *Scrollatevelo di dosso*. Motore:
  aerei ASW in crisi «solo rapporti», regole e obiettivi ci sono. Mancano: **batteria del diesel IA** che si scarica e lo
  costringe a emergere [MOTORE+], obiettivo **«mantieni il contatto per T»** [MOTORE+], segnali PDC delle navi (dossier
  22 §1.2).

### E03 — K-129 e il progetto Azorian (1968; 1974)
- **Dove:** circa 1560 nm a NO di Oahu, 4900 m → **`kunashir`** [SOST] (bacino delle Curili profondo e lato Pacifico).
- **Unità:** K-129 (Golf II, SSB diesel con 3 missili); USS *Halibut*; *Hughes Glomar Explorer*; nave sovietica di
  misura dei poligoni *Čažma*, rimorchiatore SB-10 → `mercantile` (tutte le navi civili), `sturgeon` per *Halibut*.
- **Fatti:**
  1. K-129 parte il 24 febbraio 1968 e affonda verso l'8 marzo con 98 uomini; la ricerca sovietica fallisce `[WP-K129]`
     (alta per i fatti, media per la data);
  2. i dati acustici di SOSUS e AFTAC restringono il punto; *Halibut* trova il relitto il 20 agosto 1968 `[WP-K129]`
     (media);
  3. nel 1974 il *Glomar Explorer*, con la copertura dei «noduli di manganese», recupera una parte dello scafo;
     due settimane dopo il suo arrivo si presenta la *Čažma* (foto dal ponte e dall'elicottero, se ne va dopo 4
     giorni); il rimorchiatore SB-10 resta quasi due settimane, fino a 200 yard, proprio mentre la pinza sta per
     rientrare nello scafo `[CIA-GLO]` (alta);
  4. durante il sollevamento due bracci della pinza si rompono e gran parte della sezione prodiera ricade sul fondo
     `[CIA-GLO]` (alta); sei salme recuperate e sepolte in mare `[WP-K129]` (alta).
- **Noto / conteso:** la causa dell'affondamento (snorkel sotto quota, batterie, missile, collisione con *Swordfish*
  smentita dagli USA) è ignota `[WP-K129]`; la tesi che ci fossero SSN USA pronti a difendere la nave è di Polmar e
  White, riportata dalla CIA senza conferma `[CIA-GLO]` (bassa).
- **Perché giocarlo:** guardia a una nave civile ferma, **senza poter dire perché**: osservatori sovietici a 200 yard.
- **Giocabilità:** [GIOCO] 688, `kunashir`, **pace**: proteggere la «nave mineraria» (mercantile ferma) per 12-24 ore di
  gioco compresse; due unità sovietiche (mercantile e Krivak II) arrivano per regola a tempo; un Victor III può
  arrivare se scatta l'allarme. Obiettivi `proteggere` + `non_essere_scoperto`; vietato sparare. Motore: c'è tutto; un
  **evento di sollevamento** (variabile a tempo) dà il ritmo. Rispetto: nessun nome reale del battello sovietico.

### E04 — USS *Scorpion* (maggio-ottobre 1968)
- **Dove:** circa 400 nm a SO delle Azzorre, 3000 m → **`celtica`** [SOST] (piana abissale).
- **Unità:** *Scorpion* (Skipjack); un gruppo sovietico con un Echo II che *Scorpion* doveva osservare; nave
  oceanografica *Mizar* → `los_angeles`, `charlie2`, `mercantile` `[WP-SCO]`.
- **Fatti:** dopo il Mediterraneo, *Scorpion* è mandato a osservare unità sovietiche vicino alle Azzorre; l'ultimo
  traffico radio è del 21-22 maggio; perduto con 99 uomini. Le registrazioni SOSUS e l'analisi idroacustica restringono
  l'area; *Mizar* trova lo scafo a fine ottobre `[WP-SCO]` (alta).
- **Noto / conteso:** la corte d'inchiesta dice che la causa «non può essere accertata»; le ipotesi (siluro, batteria,
  scarico dei rifiuti, cedimento strutturale, attacco sovietico) restano tutte aperte `[WP-SCO]` (media).
- **Perché giocarlo:** non come tragedia, ma come **schema di ricerca**: un battello amico smette di rispondere e si
  cerca un segnale sul fondo.
- **Giocabilità:** [GIOCO] scenario **originale** ispirato al tipo di ricerca, non a *Scorpion*: 688, `celtica`, **pace**:
  un battello di fantasia manca un appuntamento; si percorre un'area a griglia ascoltando, mentre un gruppo sovietico
  è nella zona. Motore: `raggiungere` più `scoprire` su un oggetto fermo sul fondo; serve un **oggetto passivo sul
  fondo** (relitto, contenitore) con firma acustica debole [MOTORE+].

### E05 — Holy Stone e la collisione *Gato*/K-19 (15 novembre 1969)
- **Dove:** Mare di Barents, vicino all'ingresso del Mar Bianco, 60 m → **`barents`** [SOST minima].
- **Unità:** USS *Gato* (Thresher/Permit); K-19 (Hotel) → `sturgeon` o `los_angeles`; `typhoon` [SOST, l'Hotel non c'è].
- **Fatti:** *Gato* è in una missione di raccolta d'informazioni della serie Holy Stone (pattugliamenti di circa 90
  giorni, anche chiamati Pinnacle o Bollard) `[MQ-GATO]` `[CIA-HER]` (media); alle 07:13 K-19, a 60 m, la urta; K-19
  perde il sonar di prora e i portelli dei tubi, riemerge con l'aria d'emergenza e rientra; *Gato* è quasi intatto e
  continua il pattugliamento `[WP-K19]` `[WP-GATO]` (alta per il fatto, media per i dettagli).
- **Noto / conteso:** che a bordo del *Gato* l'ufficiale alle armi abbia preparato un SUBROC e tre siluri nucleari è una
  sola testimonianza `[MQ-GATO]` (bassa). La stampa del 1975 (Hersh) parlò di penetrazioni entro le 3 miglia, di due
  collisioni e di un incaglio; il Pentagono negò le penetrazioni `[CIA-HER]` (media).
- **Perché giocarlo:** è il cuore della Guerra fredda subacquea: **stare sotto la coda** di un battello sovietico in
  acqua bassa, a casa sua, senza farsi sentire.
- **Giocabilità:** [GIOCO] 688, `barents`, **pace**: raccogliere la firma acustica di un SSBN che esce (obiettivo
  `scoprire` dopo T minuti di contatto), restare oltre le 12 nm, non farsi scoprire. Tensione: fondali di 100-300 m,
  pesca, unità ASW di superficie che pingano a caso, il bersaglio che fa i «Crazy Ivan» (dossier 03 §C4). Motore: ci sono
  zone, `scoprire`, `non_essere_scoperto`; servono la **collisione** (dossier 16) e il **contatto mantenuto** [MOTORE+].

### E06 — *Tautog* e K-108 (20 giugno 1970)
- **Dove:** al largo di Petropavlovsk-Kamčatskij → **`kunashir`** [SOST].
- **Unità:** USS *Tautog* (Sturgeon); K-108 (Echo II) → `sturgeon`; `charlie2` [SOST].
- **Fatti:** *Tautog* pedina K-108, che le passa vicinissimo senza accorgersene; gli idrofoni non danno la quota
  dell'altro; K-108 sembra salire, poi scendere, poi scompare dall'ascolto e urta col ventre la falsa torre del *Tautog*;
  K-108 perde un'elica. Il *Tautog* sente rumori che interpreta come lo sfasciarsi dell'altro e se ne va credendolo
  affondato; K-108 rientra in porto. Un pezzo d'elica resta incastrato nella vela fino a Pearl Harbor `[WP-TAU]` (media:
  conferma del comandante sovietico vent'anni dopo).
- **Perché giocarlo:** mostra perché la **quota del contatto** è l'informazione che manca al sonar passivo e perché un
  pedinamento troppo stretto è una scommessa.
- **Giocabilità:** [GIOCO] scenario breve di addestramento sulla quota: 688, `kunashir`, **pace**, «tieni il contatto a
  meno di 1000 yard per 30 minuti senza toccarlo». Motore: serve la collisione [MOTORE+] e un indicatore d'incertezza
  di quota nel TMA (vedi dossier 09 e 12).

### E07 — K-8 durante Okean-70 (8-12 aprile 1970)
- **Dove:** Golfo di Biscaglia, circa 260 nm a NO della Spagna, fondale 4680 m → **`celtica`** (appena a S del riquadro).
- **Unità:** K-8 (November); navi sovietiche in esercitazione → `victor3` [SOST], `mercantile`, `kara`.
- **Fatti:** durante l'esercitazione Okean-70 scoppiano incendi in due compartimenti a 120 m; reattori fermi; la nave
  emerge; arriva un rimorchio, parte dell'equipaggio torna a bordo; dopo circa 80 ore di lotta con il mare grosso il
  battello affonda il 12 aprile; muoiono 52 uomini (cifre non del tutto coerenti) `[WP-K8]` (media).
- **Perché giocarlo:** la più grande esercitazione navale sovietica fino ad allora, a ridosso delle rotte NATO: lo
  sfondo ideale per una missione di **osservazione di un'esercitazione** (Okean-70, Okean-75).
- **Giocabilità:** [GIOCO] 688, `celtica`, **pace**: osservare e classificare le unità di un'esercitazione sovietica
  (Kirov, Slava, Kara, Victor III, Il-38) senza farsi scoprire; a metà missione un evento d'emergenza su un battello
  sovietico cambia il quadro (navi che accorrono, aerei che volano bassi). Rispetto: battello di fantasia, nessuna
  morte mostrata. Motore: le unità che «accorrono» sono riserve con `entra_con`; un battello IA **in superficie e in
  avaria** non c'è [MOTORE+].

### E08 — Ivy Bells: il cavo del Mare di Ochotsk (1971-1981)
- **Dove:** cavo Petropavlovsk-Vladivostok, 57°36' N 155°42' E, circa 120 m → **`kunashir`** [SOST] (gli stretti delle
  Curili e il lato Ochotsk).
- **Unità:** USS *Halibut* (1971), poi *Seawolf*, *Parche*, *Richard B. Russell* → `sturgeon`, `los_angeles`; navi
  sovietiche → `krivak2`, `mercantile`, `ka27`, `il38` `[WP-IVY]`.
- **Fatti:**
  1. nell'ottobre 1971 *Halibut* entra nell'Ochotsk; i palombari montano un dispositivo di circa 6 m attorno al cavo,
     senza forarlo; la copertura ufficiale è il recupero di frammenti del missile SS-N-12 `[WP-IVY]` (media);
  2. per anni i battelli tornano a cambiare le registrazioni; *Seawolf* resta bloccata sul fondo dopo una tempesta e
     quasi deve usare le cariche d'autodistruzione `[WP-IVY]` `[BMB]` (media);
  3. nel 1980 Ronald Pelton, ex NSA, vende l'informazione al KGB; nel 1981 navi sovietiche sono viste sul sito e
     *Parche* non trova più il dispositivo; Pelton è arrestato nel 1985 `[WP-IVY]` (alta per Pelton, media per il resto).
- **Noto / conteso:** il quando e il come della scoperta sovietica non sono chiari `[WP-IVY]`.
- **Perché giocarlo:** è l'unica missione «da ladro»: **entrare, restare fermi, uscire**. E ha due lati.
- **Giocabilità:**
  - [GIOCO] 688, `kunashir`, **pace**: passare uno stretto delle Curili sorvegliato, raggiungere il punto del cavo,
    restarci T ore sotto una certa velocità (i palombari), uscire; una tempesta (meteo) e una nave sovietica che si ferma
    sul punto alzano la tensione;
  - [GIOCO] Alfa, `kunashir`, **pace o crisi**: scortare la nave che va a «ripulire» il cavo e tenere lontano un intruso
    che forse è lì, senza sparare.

  Motore: `raggiungere`, zone e regole ci sono; servono condizioni su **velocità e quota del giocatore** e un **conteggio
  del tempo dentro una zona** [MOTORE+] (oggi si può approssimare con regole a tempo e variabili).

### E09 — PNS *Ghazi* a Visakhapatnam (3-4 dicembre 1971) — ispirazione
- **Dove:** golfo del Bengala → `kunashir` o `barents` [SOST] (un porto da minare).
- **Unità:** *Ghazi* (ex *Diablo*, classe Tench); INS *Rajput* → `foxtrot` [SOST], `koni` o `krivak2`.
- **Fatti:** *Ghazi* cerca la portaerei *Vikrant* e deve posare mine; non la trova e va a minare il porto di
  Visakhapatnam; affonda nella notte del 3-4 dicembre con 93 uomini `[WP-GHA]` (alta).
- **Conteso:** l'India attribuisce l'affondamento al *Rajput*; il Pakistan a un'esplosione interna o alle proprie mine;
  altre ipotesi: idrogeno delle batterie, siluri difettosi `[WP-GHA]` (media).
- **Perché giocarlo:** il **minamento offensivo di un porto** con il rischio delle proprie mine e una scorta che pattuglia
  l'imboccatura.
- **Giocabilità:** vedi lo schema P07 e la proposta «Tappo» (§6). Motore: l'obiettivo `minare` esiste.

### E10 — PNS *Hangor* contro INS *Khukri* (9 dicembre 1971) — ispirazione
- **Dove:** al largo di Diu → **`otranto`** [SOST] (acqua media, costa vicina).
- **Unità:** *Hangor* (Daphné); fregate *Khukri*, *Kirpan* (gruppo cacciatore) → il giocatore Alfa; `perry` o `koni`.
- **Fatti:** due vecchie fregate sono mandate a cacciare un sottomarino segnalato; *Khukri* va piano per provare un nuovo
  sonar, contro la dottrina. *Hangor* lancia contro *Kirpan* (mancato), poi colpisce *Khukri*, che affonda con 194
  uomini; il comandante Mulla resta a bordo. *Kirpan* contrattacca senza esito, schiva un altro siluro; *Hangor* si
  allontana `[WP-KHU]` (alta).
- **Perché giocarlo:** il sottomarino che **ribalta la caccia**: il gruppo ASW lento è vulnerabile.
- **Giocabilità:** [GIOCO] Alfa, `otranto`, **guerra**: un gruppo cacciatore (2 Perry e un SH-60B) ti cerca; una delle
  due rallenta (regola a tempo); attacca e sopravvivi. Diverso da *Scrollatevelo di dosso* perché qui si spara.
  Motore: c'è tutto.

### E11 — Mediterraneo, ottobre 1973: la 5ª Eskadra contro la Sesta Flotta
- **Dove:** a S di Creta, Golfo della Sirte, avvicinamenti a Gibilterra → **`otranto`** e **`gibilterra`** [SOST].
- **Unità:** fino a circa 95 unità sovietiche (69 il 14 ottobre), **16 sottomarini il 9 ottobre** con almeno 4 SSN
  (November), Echo II e Juliett vicino ai gruppi portaerei, un Charlie nella Sirte; incrociatori Kynda, Kashin e altri
  che pedinano le portaerei; tre gruppi USA (*Independence*, *Franklin D. Roosevelt*, poi *John F. Kennedy*) →
  `charlie2`, `oscar`, `victor3`, `foxtrot`; `slava`, `kara`, `krivak2`; `nimitz`, `spruance`, `kidd`, `perry`,
  `sacramento` `[NWCR04]` `[WP-5E]`.
- **Fatti** (da Goldstein e Žukov, con diari sovietici) `[NWCR04]` (media-alta):
  1. 6 ottobre: guerra del Kippur; la 5ª Eskadra si rinforza; le portaerei USA restano in un'area fissa a S di Creta
     e questo «semplifica la soluzione del tiro» ai sovietici; un diario sovietico nota il buon tempo e le basse
     velocità americane;
  2. dal 9-10 ottobre navi missilistiche sovietiche pedinano le portaerei (pedinamento ravvicinato per la scoperta del
     bersaglio); il 24 Washington passa a DEFCON 3; il 26 i sovietici fanno intense esercitazioni anti-portaerei;
  3. per tenere nervosi gli americani, un comandante sovietico fa lanciare una **bomba a mano in acqua «come per
     comunicare con un nostro sommergibile»**: i voli ASW aumentano;
  4. un Charlie racconta di due giorni di caccia ASW americana nella Sirte senza essere scoperto;
  5. i sottomarini sovietici dell'Atlantico sono mandati ad **aspettare i rinforzi USA a Gibilterra**;
  6. lo stesso articolo descrive come si **attraversava Gibilterra**: rotta e quote prefissate, a quota periscopica davanti
     a Capo Spartel per un rilevamento radar di un paio di minuti e un rilevamento visivo del faro, poi la traversata ad
     alta velocità per la corrente; dopo un battello che toccò il fondo, ecoscandaglio acceso anche a costo di farsi
     sentire.
- **Conteso:** i numeri dei sottomarini variano tra 11 e circa 23 a seconda delle fonti `[WP-5E]` (media).
- **Perché giocarlo:** la crisi navale più grave della Guerra fredda: **due flotte a distanza di tiro**, nessuno spara.
- **Giocabilità:**
  - [GIOCO] Alfa, `otranto`, **crisi**: fare da «pedinatore» di un gruppo Nimitz (restare entro X nm per T ore e
    trasmettere la posizione via radio) mentre gli SH-3H e gli S-3 cercano; un ordine radio può portare a guerra o a
    rientro (varianti);
  - [GIOCO] 688, `otranto`, **crisi**: proteggere il gruppo da un Charlie II e un Oscar II, sparando solo se lanciano
    (`ARMI = 1` per l'IA, ma serve la stessa regola per il giocatore);
  - [GIOCO] Alfa, `gibilterra`, **pace**: attraversare lo stretto «alla sovietica» (vedi P08).

  Motore: riserve, ordini radio, aerei ASW ci sono; servono il **cambio di situazione a metà missione** e la
  **trasmissione del giocatore** come azione con costo di scoperta [MOTORE+] (roadmap, meteo via radio).

### E12 — Norvegia: il Sognefjord (novembre 1972) e il Hardangerfjord (aprile-maggio 1983)
- **Dove:** fiordi della Norvegia occidentale → **`faroe`** [SOST] (i fiordi delle Faroe hanno lo stesso ambiente).
- **Unità:** fregate norvegesi (Oslo), motocannoniere, P-3B, aerei britannici → `koni` o `perry` in parte neutrale,
  `nanuchka`, `p3c`, `sh60b` [SOST].
- **Fatti:**
  - 1972, Sognefjord: per due settimane unità norvegesi con aerei britannici e norvegesi cercano di far emergere un
    contatto, con bombe a mano, poi bombe di profondità e razzi antisom; non lo identificano né lo affondano
    `[CIMSEC]` (media);
  - fine aprile 1983, Hardangerfjord: fregate norvegesi sparano razzi e bombe di profondità contro un sottomarino
    sospetto all'imboccatura; una chiazza d'olio, nessuna conferma `[CSM83N]` (media);
  - più tardi la Norvegia adotta una **scala d'escalation**: segnale di emergere; se obbedisce, scorta; se no, bombe di
    profondità di segnalazione a 300 m, ogni due minuti; se ancora no, attacco con bombe di profondità; **siluri
    vietati** `[CIMSEC]` (media).
- **Conteso:** uno studio dell'accademia militare norvegese (2013) conclude che sottomarini stranieri sono entrati senza
  dubbio, per lo più diesel di origine sovietica, ma in qualche caso gli indizi puntavano a una potenza occidentale
  `[FHS13]` (media). L'acqua dolce dei fiordi sopra l'acqua salata nasconde il battello al sonar `[CIMSEC]`.
- **Perché giocarlo:** **ROE a gradini**, acqua stratificata, pareti di roccia: una missione d'infiltrazione perfetta.
- **Giocabilità:** [GIOCO] varianti 688 e Alfa (la nazionalità reale è contesa), `faroe`, **crisi**: entrare in un fiordo,
  fotografare un'installazione (periscopio, `scoprire` su un'unità ferma), uscire. Le navi «neutrali» applicano la scala
  di escalation per regole: allarme → messaggio; dopo 5 min → bombe di segnalazione; dopo 15 → attacco. Motore: le regole
  ci sono; mancano le **bombe di segnalazione** come azione e una **parte neutrale armata** [MOTORE+].

### E13 — Holy Loch: *James Madison* e il «pedinatore in attesa» (3 novembre 1974)
- **Dove:** uscita della base SSBN di Holy Loch (Clyde) → **`celtica`** [SOST] o `faroe`.
- **Unità:** USS *James Madison* (SSBN); un sottomarino sovietico, per i russi K-306 (Victor I) → `typhoon` [SOST per il
  boomer USA: manca un SSBN americano], `victor3`, o il giocatore Alfa.
- **Fatti:** un cablogramma della CIA a Kissinger riferisce che l'SSBN in partenza per il pattugliamento entrò in
  collisione, durante l'immersione, con un sottomarino sovietico «in attesa fuori dal porto per prendere il pedinamento»;
  entrambi emersero, poi il sovietico si immerse di nuovo `[WP-JM]` (alta: documento declassificato nel 2017). Le
  memorie russe aggiungono che il compito era registrare la firma acustica e che usarono granate di segnalazione
  `[WP-JM]` (bassa-media).
- **Perché giocarlo:** è il compito più tipico di un SSN sovietico, e quello che *Aport* e *Atrina* cercheranno in grande.
- **Giocabilità:** [GIOCO] Alfa, `celtica`, **pace**: aspettare all'uscita di una base l'SSBN in partenza (orario incerto,
  riserva che entra a un'ora casuale), agganciarlo e registrarlo, non farsi scacciare dalle fregate di scorta. Motore:
  `entra_con` e regole ci sono; serve un **SSBN americano** (o un 688 «di fantasia» come boomer) [MOTORE+].

### E14 — U-137, «Whiskey on the rocks» (27 ottobre - 6 novembre 1981)
- **Dove:** a circa 10 km da Karlskrona, Svezia, 56,07 N 15,73 E → **`baltico`** [SOST] (appena fuori riquadro; Bornholm
  o la costa di Simrishamn come sostituto).
- **Unità:** S-363 (U-137, Whiskey); Marina svedese; flotta sovietica al largo → `foxtrot`; neutrali `koni`/`nanuchka`
  [SOST]; `krivak2`, `kara`.
- **Fatti** `[WP-U137]` (media salvo dove detto):
  1. il battello tocca uno scoglio ed emerge in acque svedesi, proprio durante un'esercitazione svedese;
  2. un ufficiale svedese disarmato sale a bordo; il comandante (Guščin) parla di guasti contemporanei agli strumenti;
     gli ispettori non trovano guasti e trovano un giornale di bordo alterato `[CIMSEC]`;
  3. misure gamma da una motovedetta: quasi certamente uranio, localizzato al tubo lanciasiluri di sinistra;
  4. quando due eco radar sembrano avvicinarsi dalla flotta sovietica, il primo ministro Fälldin ordina di «tenere il
     confine»: batterie costiere ai posti di combattimento, aerei in volo; poi gli eco risultano due mercantili tedeschi
     (bassa: «citation needed»);
  5. il 5 novembre rimorchiatori svedesi lo disincagliano e lo consegnano alla flotta sovietica in acque internazionali.
- **Noto / conteso:** l'inchiesta svedese conclude che il battello entrò di proposito; anni dopo il commissario politico
  conferma testate nucleari e l'ordine di autodistruzione in caso di abbordaggio (media); le sue versioni sull'errore di
  navigazione cambiano nel tempo.
- **Perché giocarlo:** una crisi in cui **nessuno vuole sparare** e tutti hanno le armi pronte; il sottomarino in
  superficie è un ostaggio.
- **Giocabilità:** [GIOCO] Alfa, `baltico`, **crisi**: proteggere da fuori le acque territoriali il rimorchio di un
  battello sovietico di fantasia, con navi neutrali e un SSN NATO che osserva; vietato entrare nella zona dei 12 nm.
  Motore: zona e regola «giocatore in zona → penalità» ci sono come messaggio; serve la **penalità di carriera** e il
  **battello incagliato o rimorchiato** [MOTORE+].

### E15 — Hårsfjärden e gli arcipelaghi svedesi (1° ottobre - inizio novembre 1982; 1980-1992)
- **Dove:** base navale di Muskö, arcipelago di Stoccolma → **`baltico`** [SOST] (fondali di 8-50 m come a Hårsfjärden).
- **Unità:** forze ASW svedesi (corvette, elicotteri), campi minati controllati da terra; il presunto intruso
  (sottomarino «madre» e mini sommergibili) → neutrali `koni`, `nanuchka`, `sh60b` [SOST]; il giocatore.
- **Fatti:**
  1. la Marina chiude un'area con mine e sensori; riconosce un sottomarino dentro la trappola; vengono fatte esplodere
     **44 bombe di profondità e 4 mine** `[WP-SWE]` (media);
  2. secondo la stampa d'epoca il battello passò la barriera e le mine furono fatte saltare da terra quando era già
     lontano `[CSM82]`; secondo voci riportate dalla UPI ci fu un ordine di **cessate il fuoco di tre ore** alla stazione
     delle mine il 7 ottobre `[UPI82]` (bassa);
  3. la commissione del 1983 attribuisce l'incursione all'URSS (sei sottomarini, tre dei quali mini, secondo la stampa
     del giorno dopo) e porta a nuove ROE: fuori dalle acque interne avvertire e scortare fuori; dentro gli arcipelaghi,
     se il contatto non esce, fuoco per danneggiare o affondare `[CIMSEC]` `[TUN]` (media).
- **Conteso:** molto. Nel 2008 l'FOI ha riattribuito un suono «di sottomarino» del 1982 a un battello da diporto
  `[FM08]` (alta); lo storico Tunander sostiene che la maggior parte delle incursioni fosse occidentale `[TUN]` (bassa:
  tesi minoritaria); la Svezia ha confermato solo U-137 e un battello tedesco occidentale a Simrishamn nel 1990, per
  errore di navigazione `[WP-SWE]` (alta).
- **Perché giocarlo:** acqua bassissima, mine controllate da terra, nemico neutrale, **ROE che dipendono dalla linea su
  cui ti trovi**.
- **Giocabilità:** [GIOCO] **due varianti, 688 e Alfa**, `baltico`, **crisi**: entrare in un arcipelago neutrale di fantasia,
  osservare una base, uscire. Zone a strati: acque esterne (avvertimento), interne (fuoco). Nessun nome di paese reale.
  Motore: zone e regole ci sono; servono le **mine controllate da terra** (mina che esplode per regola), condizioni sulla
  **quota** (si passa solo a quota bassa) e la parte neutrale armata [MOTORE+].

### E16 — *Conqueror* e il *General Belgrano* (30 aprile - 2 maggio 1982) — ispirazione
- **Dove:** a S delle Falkland, appena fuori dalla zona d'esclusione → **`celtica`** [SOST] (mare aperto, burrasca).
- **Unità:** HMS *Conqueror* (Churchill); incrociatore *General Belgrano*, cacciatorpediniere *Bouchard* e *Piedra Buena*
  → giocatore 688; `iowa` [SOST: un incrociatore con cannoni], `spruance` o `perry` in parte nemica.
- **Fatti** `[WP-BEL]` (media-alta):
  1. il 1° maggio *Conqueror* trova il gruppo mentre rifornisce; è fuori dalla TEZ e non attacca; lo pedina e riferisce;
  2. le ROE iniziali permettono fuori dalla zona solo l'attacco alla portaerei; Woodward ordina l'attacco, il governo le
     allarga; Northwood autorizza alle 13:30 del 2 maggio;
  3. il comandante sceglie i vecchi **Mk 8** invece dei Tigerfish; tre siluri da circa 1,4 km: due a segno, il terzo
     esplode vicino al *Bouchard*;
  4. le scorte non capiscono l'attacco, non trovano il battello e si allontanano; 323 morti argentini.
- **Conteso:** rotta del gruppo (verso la flotta o via), legittimità fuori zona, informazioni che non arrivarono al
  governo `[WP-BEL]`.
- **Perché giocarlo:** non l'affondamento in sé, ma il **pedinamento lungo con ROE che cambiano a metà**: il giocatore
  aspetta un ordine che può non arrivare.
- **Giocabilità:** [GIOCO] 688, `celtica`, **guerra con zona**: pedinare un gruppo di superficie di fantasia fuori da una
  zona dichiarata; l'ordine radio che allarga le ROE arriva (o no) in una finestra casuale; se si attacca prima, esito
  «fuoco senza ordine». Motore: `da_radio` e zone ci sono; serve il **cambio di ROE del giocatore a metà missione** e un
  esito di carriera per il fuoco non autorizzato (dossier 13) [MOTORE+]. Rispetto: nessun nome di nave reale.

### E17 — ARA *Santa Fe* a South Georgia (25 aprile 1982) — ispirazione
- **Dove:** Grytviken → **`faroe`** [SOST] (isole, fiordi, acqua fredda).
- **Unità:** *Santa Fe* (Guppy II); Wessex, Lynx, Wasp britannici → `foxtrot`; `sh3h`, `sh60b` [SOST].
- **Fatti:** lasciato il porto all'alba **in superficie**, il battello è scoperto dal radar di un Wessex e colpito con
  bombe di profondità (cassa zavorra squarciata); poi un Lynx lancia un Mk 46 che manca; elicotteri lo mitragliano; gli
  AS-12 dei Wasp attraversano la vela in vetroresina senza esplodere; il battello rientra al molo e viene abbandonato;
  la guarnigione si arrende dopo colpi d'avvertimento delle navi `[WP-SF]` (media).
- **Perché giocarlo:** un battello **in superficie in acque ristrette** contro elicotteri: tutto quello che il dossier 17
  e il 22 descrivono.
- **Giocabilità:** come schema di **evasione dopo la scoperta in superficie** (P12); il giocatore è il cacciato (Alfa che
  esce da un fiordo dopo uno sbarco). Motore: c'è quasi tutto; l'artiglieria contro il battello è in `docs/16`.

### E18 — ARA *San Luis* (1°-17 maggio 1982) — ispirazione
- **Dove:** a N delle Falkland, acque basse.
- **Unità:** *San Luis* (Type 209); fregate e Sea King britannici → il giocatore Alfa [SOST]; `perry`, `sh3h`.
- **Fatti:** il 1° maggio lancia un SST-4 contro due fregate e manca; è cacciato per circa 20 ore con bombe e siluri; il
  10 maggio lancia due siluri contro altre due fregate: uno non esce dal tubo, l'altro forse colpisce l'esca rimorchiata;
  sfugge a 25 ore di caccia. Dopo la guerra si scopre la **polarità invertita** dei cavi dei giroscopi. La Royal Navy
  consuma circa 50 Mk 46 senza mai localizzarlo `[WP-SL]` (media).
- **Perché giocarlo:** un battello solo, in acque basse e rumorose, che logora una forza ASW intera. Il caso perfetto per
  un **guasto d'arma** come evento (raro) e per l'esca rimorchiata.

### E19 — K-324 e la schiera della *McCloy* (31 ottobre - novembre 1983)
- **Dove:** 282 nm a O delle Bermuda → **`celtica`** [SOST].
- **Unità:** K-324 (Victor III); fregata USS *McCloy* con schiera rimorchiata → giocatore Alfa; `perry` (con schiera).
- **Fatti:** K-324 aggancia con l'elica il cavo della schiera della *McCloy*; danni all'elica; dal 5 novembre un
  rimorchiatore sovietico lo porta a Cienfuegos; i tecnici sovietici recuperano parti della schiera `[WP-K324]` (media);
  il dossier 03 riporta circa 400 m di cavo avvolti secondo fonti russe `[D03]` (bassa-media).
- **Perché giocarlo:** il **pedinamento a corta distanza di una fregata ASW** dietro la sua scia, dove il sonar della nave è
  sordo: il guadagno (la firma della schiera) e il rischio (il cavo).
- **Giocabilità:** [GIOCO] Alfa, `celtica`, **pace**: restare per T minuti nel settore poppiero di una Perry (sordo) senza
  avvicinarsi a meno di X yard dalla schiera. Motore: la schiera rimorchiata esiste (`docs/07`); serve la **geometria del
  cavo come ostacolo** [MOTORE+] e il battello in avaria.

### E20 — K-314 e la *Kitty Hawk* (14-21 marzo 1984)
- **Dove:** Mar del Giappone, esercitazione Team Spirit → **`kunashir`** lato Pacifico [SOST].
- **Unità:** K-314 (Victor I); gruppo *Kitty Hawk* con la fregata *Harold E. Holt* → giocatore Alfa; `nimitz`, `perry`,
  `spruance`, `sh3h`, `s3b`.
- **Fatti:** il Victor trova la portaerei verso il 14 marzo e la pedina; il gruppo pratica tecniche d'inganno per
  scrollarselo; col maltempo il battello perde il contatto (il 20 secondo una fonte); la sera del 21, verso le 22, risale
  a quota periscopica per ritrovarla e si trova sulla sua rotta; la portaerei a circa 15 kn lo urta due volte; il Victor
  rotola, perde un'elica (un pezzo resta nello scafo della portaerei) e deve essere rimorchiato; la *Holt* resta vicino
  per giorni `[WP-K314]` `[NI-K314]` (media). Il racconto del comandante (Evseenko) arriva anni dopo (bassa-media).
- **Perché giocarlo:** il **pedinamento di un gruppo portaerei** per giorni, la perdita del contatto col mare grosso e il
  momento più pericoloso: **tornare su alla cieca** per ritrovarlo.
- **Giocabilità:** [GIOCO] Alfa, `kunashir`, **pace**: mantenere il contatto con un gruppo Nimitz per 24 ore di gioco
  compresse con il meteo che peggiora; il gruppo fa zig-zag e silenzio radio (comportamenti a regola); ritrovarlo senza
  farsi scoprire e **senza salire davanti alla prora**. Motore: meteo dinamico e regole ci sono; servono collisione e
  contatto mantenuto [MOTORE+].

### E21 — Operazione Aport (maggio-luglio 1985)
- **Dove:** Atlantico occidentale → partenza dal Barents, passaggio della barriera **`faroe`**, poi `celtica` [SOST].
- **Unità:** 5-6 sottomarini sovietici (Victor I, II, III) → `victor3`, il giocatore Alfa.
- **Fatti:** uscita in massa di SSN per **trovare le aree di pattugliamento degli SSBN americani e pedinarli**, e per
  saturare la sorveglianza `[USNI20]` (media: letto solo l'estratto); precursore di Atrina.
- **Conteso:** il bilancio (successo per i russi, operazione seguita per gli americani) `[USNI20]`; i dettagli del
  tracciamento americano n.v.
- **Giocabilità:** [GIOCO] Alfa, `faroe`, **pace**: passare la barriera Islanda-Faroe sotto il SOSUS (datum con ritardo
  agli aerei), arrivare a un'area senza fix. L'Alfa è rumoroso ad alta velocità: si gioca a **scatti e derive** sfruttando
  il termoclino. Motore: c'è tutto (datum SOSUS, P-3C, non essere scoperto).

### E22 — Golfo della Sirte (23-25 marzo 1986)
- **Dove:** Golfo della Sirte → **`otranto`** o **`gibilterra`** [SOST].
- **Unità:** tre portaerei USA (*America*, *Coral Sea*, *Saratoga*), incrociatore *Ticonderoga* con *Scott* e *Caron*;
  corvette libiche Nanuchka, motomissilistiche La Combattante, siti SA-5 → `nimitz`, `kidd`, `spruance`; `nanuchka`
  (anche per le La Combattante).
- **Fatti:** il 23 marzo gli aerei USA passano la «linea della morte»; il 24 il *Ticonderoga* e le scorte vanno a S della
  linea; gli SA-5 sparano e mancano; da sera le unità libiche che si avvicinano sono colpite con Harpoon e Rockeye: una
  Nanuchka affondata, una gravemente danneggiata, motovedette affondate o danneggiate `[WP-SID]` (media).
- **Conteso:** le perdite libiche; la Libia aveva sottomarini Foxtrot, ma il loro ruolo in quei giorni n.v.
- **Giocabilità:** [GIOCO] 688, `otranto`, **crisi → guerra**: esploratore davanti al gruppo che attraversa una linea
  dichiarata; quando una Nanuchka accende il radar di tiro (regola), la situazione passa a guerra. Diverso da *Mumar
  Cadaver* (petroliere a Gibilterra). Motore: serve il **cambio di situazione per regola** [MOTORE+].

### E23 — K-219 al largo delle Bermuda (3-6 ottobre 1986)
- **Dove:** circa 680 nm a NE delle Bermuda; affonda a circa 6000 m → **`celtica`** [SOST] (piana abissale).
- **Unità:** K-219 (Yankee I); USS *Augusta* in zona; navi sovietiche di soccorso → `typhoon` [SOST: manca lo Yankee];
  `los_angeles`; `mercantile`.
- **Fatti:** la guarnizione di un portello di missile cede, l'acqua entra nel tubo, il propellente si incendia; due
  morti subito, un terzo poco dopo; il marinaio Preminin muore dopo aver spento a mano il secondo reattore; il
  comandante Britanov emerge, tenta il rimorchio, che fallisce; il battello affonda il 6 ottobre con le armi nucleari;
  Britanov, accusato e poi prosciolto `[WP-K219]` (alta).
- **Conteso:** la versione sovietica di una collisione con *Augusta* è smentita dalla US Navy, da Britanov e da Kurdin
  `[WP-K219]` (la collisione è da considerare **non avvenuta**).
- **Perché giocarlo:** il **pedinamento di un SSBN in pattugliamento** (il compito normale dei 688 negli anni '80) e la
  domanda morale: che cosa fa il pedinatore quando il bersaglio è in emergenza?
- **Giocabilità:** [GIOCO] 688, `celtica`, **pace**: pedinare un SSBN di fantasia per ore; a un'ora casuale l'SSBN emerge
  in emergenza (regola); ordini radio: restare nascosti, riferire, non avvicinarsi alle navi sovietiche che arrivano. Lato
  Alfa: vedi P10. Rispetto: nessun nome reale, nessuna morte mostrata (§5). Motore: servono l'**emersione forzata
  dell'IA** e il **contatto mantenuto** [MOTORE+].

### E24 — *Augusta* e K-279 (20 ottobre 1986)
- **Dove:** Atlantico orientale → **`celtica`**.
- **Unità:** USS *Augusta* (688); K-279 (Delta I) secondo gli autori americani, K-457 secondo la Marina sovietica →
  `los_angeles`; `typhoon` [SOST].
- **Fatti:** collisione durante, si ritiene, un pedinamento di un SSBN; *Augusta* richiede circa 3 milioni di dollari di
  riparazioni a prora e sonar `[WP-K279]` `[GW2RU]` (media per il fatto, bassa per i dettagli).
- **Giocabilità:** argomento a favore di un **modello di collisione** e di un rischio crescente sotto le 500 yard; nessuna
  missione dedicata.

### E25 — Operazione Atrina (marzo-maggio 1987)
- **Dove:** da Zapadnaja Lica attorno alla Groenlandia per evitare il SOSUS del GIUK, poi la costa USA → **`faroe`** [SOST]
  (la barriera), `celtica` (l'Atlantico).
- **Unità:** cinque Victor III (K-244, K-255, K-298, K-299, K-524) → `victor3`; P-3C, fregate, SSN USA → `p3c`, `perry`,
  `los_angeles` `[WP-ATR]`.
- **Fatti:** i battelli escono per una rotta più lunga per non essere sentiti; il 7 aprile 1987 la US Navy dice che operano
  vicino alle Bermuda e che ha mandato forze a sorvegliarli `[USNI20]` (media); le fonti occidentali parlano di un
  tracciamento complesso ma riuscito di almeno quattro su cinque `[WP-ATR]` (media).
- **Conteso:** per i russi un trionfo, per gli americani un insuccesso sovietico `[WP-ATR]`; c'entra anche l'aiuto della
  rete di spie Walker alla silenziosità sovietica `[WP-ATR]` (media).
- **Perché giocarlo:** è il miglior esempio di **caccia di barriera dal lato NATO** e di **infiltrazione dal lato
  sovietico**; e i due lati si contraddicono, quindi entrambi gli esiti sono plausibili.
- **Giocabilità:**
  - [GIOCO] 688, `faroe`, **pace**: il SOSUS perde cinque contatti; con un P-3C ritrovarne e classificarne almeno 3 su 5
    prima che escano dal teatro (`scoprire` con `minimo = 3`);
  - [GIOCO] Alfa: come Aport (E21), ma con un gruppo di Victor III IA da coprire.

### E26 — La guerra delle petroliere (luglio 1987 - aprile 1988) — ispirazione
- **Dove:** Golfo Persico → **`gibilterra`** [SOST] (stretto con traffico altissimo) o `otranto`.
- **Unità:** petroliere ribattezzate americane (*Bridgeton*), scorte USN (Perry, incrociatori), nave posamine iraniana
  *Iran Ajr*, motoscafi → `petroliera` (la classe è proprio la *Sea Isle City*, colpita da un missile iraniano nel 1987),
  `perry`, `kidd`; `mercantile` per la posamine.
- **Fatti:** il 24 luglio 1987 la *Bridgeton*, nel primo convoglio Earnest Will, urta una mina presso l'isola di Farsi;
  nel settembre 1987 l'*Iran Ajr* è catturata mentre posa mine; il 14 aprile 1988 la fregata *Samuel B. Roberts*
  urta una mina dopo averne avvistate tre in superficie; segue l'operazione Praying Mantis `[WP-BRI]` `[NHHC-SBR]` (alta).
- **Perché giocarlo:** la scorta in **crisi**, con un nemico che nega le proprie mine. Diversa da *Ritorno a casa* (guerra
  aperta).
- **Giocabilità:** [GIOCO] 688, `gibilterra`, **crisi**: scortare tre petroliere in uno stretto; una nave civile posa mine
  di notte (regola); scoprirla, riferire, non sparare finché non arriva l'ordine. Motore: mine ormeggiate (roadmap),
  `proteggere`, `scoprire`, ordini radio; serve un **sonar di scoperta delle mine** del 688 (BQS-15) [MOTORE+].

### E27 — K-278 *Komsomolets* (7 aprile 1989)
- **Dove:** circa 180 km a SO dell'Isola dell'Orso, poi 1680 m → **`barents`** [SOST] (margine N) o `faroe`.
- **Unità:** K-278 (Mike, unico); Il-38 sovietici; peschereccio-madre *Aleksej Chlobystov* → nessuna classe Mike
  (`akula` o `oscar` come sostituto); `il38`; `mercantile`.
- **Fatti:** a 335 m un corto circuito incendia un compartimento poppiero; il reattore si ferma; il battello emerge
  con l'aria d'emergenza circa 11 minuti dopo; l'incendio continua alimentato dall'aria compressa; affonda circa
  quattro ore dopo; gli aerei lanciano zattere che il vento e il mare rendono inutilizzabili; il peschereccio arriva 81
  minuti dopo l'affondamento; **42 morti su 69**, molti per ipotermia in acqua a 2 °C `[WP-K278]` (alta).
- **Perché giocarlo:** è **del 1989**, l'anno del gioco; mostra il soccorso nell'Artico e i limiti degli aerei.
- **Giocabilità:** [GIOCO] scenario **originale** ispirato, Alfa, `barents`, **pace**: un battello di fantasia segnala un
  incendio; l'Alfa è il più vicino; arrivare, guidare con la radio e il segnale subacqueo una nave civile, tenere lontano
  un osservatore NATO senza armi. Rispetto: nessun nome reale; si conta chi si salva, non chi muore. Motore: serve il
  **soccorso** (roadmap: «emersione e salvataggio») [MOTORE+].

### E28 — Kildin: *Baton Rouge* e *Kostroma* (11 febbraio 1992); *Grayling* e K-407 (marzo 1993)
- **Dove:** al largo dell'isola di Kildin, vicino a Severomorsk → **`barents`** (dentro il riquadro).
- **Unità:** USS *Baton Rouge* (688); K-276 *Kostroma* (Sierra II) → `los_angeles`; `akula` o `victor3` [SOST].
- **Fatti:** alle 20:16 il *Kostroma*, in risalita, urta da sotto la poppa del *Baton Rouge*; entrambi solo in ascolto
  passivo; acque basse con frangenti che coprono i rumori; il *Kostroma* ha la vela ammaccata, il *Baton Rouge* graffi e
  tagli; per gli USA la collisione avviene appena oltre le 12 nm, per la Russia 5 nm **dentro** le acque territoriali
  secondo la sua linea di base retta; protesta diplomatica e incontro Baker-Eltsin; la US Navy sospende alcune attività
  vicino alle basi russe; nel marzo 1993 il *Grayling* urta il Delta K-407 al largo di Kola `[WP-KIL]` (media).
- **Conteso:** chi pedinava chi; se il 688, cieco nel cono poppiero, abbia mai avuto il contatto `[WP-KIL]` (media).
- **Perché giocarlo:** dopo la fine dell'URSS (fuori dal 1989), ma è il caso più chiaro del **conflitto sulle linee di
  base**: la stessa posizione è legale per uno e illegale per l'altro.
- **Giocabilità:** [GIOCO] 688, `barents`, **pace**, «trasposto al 1989»: sorvegliare le uscite di Severomorsk stando fuori
  dalle 12 nm **misurate dalla costa**, ma dentro le acque rivendicate dalla linea retta; se si entra nella zona
  rivendicata, la Flotta del Nord reagisce (riserve, allarme). Lato Alfa: è *Operazioni subacquee*, ma con la
  collisione possibile. Motore: due zone sovrapposte e regole bastano; serve la collisione [MOTORE+].

### E29 — I cavi del Barents e le basi di Kola (fine anni '70 - anni '80)
- **Dove:** Mare di Barents → **`barents`**.
- **Unità:** USS *Parche* e altri battelli per operazioni speciali → `los_angeles` o `sturgeon`.
- **Fatti:** dopo Ivy Bells la stessa operazione sui cavi fu ripetuta nel Barents, «particolarmente audace» per la densità
  di sottomarini e navi sovietiche `[USNCVA]` `[BMB]` (media); date e dettagli n.v.
- **Giocabilità:** come E08, ma con notte polare, mare mescolato d'inverno senza strati e traffico militare continuo
  (dossier 03 A2): **la versione difficile** di «Campane d'edera».

### E30 — Le esercitazioni NATO: Ocean Safari '85, Northern Wedding, Team Work '88
- **Dove:** Mar di Norvegia, Islanda, fiordi norvegesi → **`faroe`**, `celtica`.
- **Unità:** nel 1985 più di 20 navi partono da Norfolk con la *Iowa* e la *America*, seguite da una nave spia sovietica
  `[UPI85]`; HMS *Warspite* fa la parte del sottomarino sovietico contro la portaerei *Foch* `[IWM85]` → `iowa`, `nimitz`,
  `trafalgar` (avversario «arancione»), `perry`, `spruance`, `sacramento`.
- **Fatti:** Ocean Safari si alterna con Northern Wedding negli anni dispari e pari `[WP-NW]` (media); la Marina sperimenta
  la portaerei **nascosta nel Vestfjord** dal radar, con i sottomarini che bonificano il fiordo prima e ne proteggono
  l'ingresso `[NSL-LEH]` (media, da Lehman, *Oceans Ventured*); Team Work '88 porta navi anfibie nei fiordi `[NWCR90]`
  (bassa: solo una didascalia).
- **Perché giocarlo:** le esercitazioni sono missioni **di pace con armi da esercitazione**, ideali per l'addestramento e
  per i punteggi; e mostrano la Maritime Strategy all'opera.
- **Giocabilità:** [GIOCO] 688, `faroe`, **esercitazione** (pace): bonificare un fiordo da un Trafalgar «arancione» prima
  che entri la portaerei; colpi con siluri da esercitazione (l'esito «colpito» senza danni c'è in TORPEX). Motore: c'è
  tutto.

### E31 — I bastioni del Barents e dell'Ochotsk e la «strategic ASW» (1973-1991)
- **Dove:** **`barents`**, **`kunashir`**.
- **Unità:** Delta e Typhoon in bastione, SSN di difesa (Victor III, Akula), Udaloy e Krivak, Il-38, Tu-142, Ka-27 →
  `typhoon`, `akula`, `victor3`, `krivak2`, `kara`, `il38`, `tu142`, `ka27`, `be12`; per gli USA `los_angeles`,
  `sturgeon` (più adatto sotto il ghiaccio).
- **Fatti:** dal 1973 (Delta I con SS-N-8) la Marina sovietica ha come compito principale difendere gli SSBN come riserva
  strategica in bastioni a strati; gli USA lo capiscono tardi (1980-81); la Maritime Strategy (pubblica dal gennaio
  1986) prevede di mandare SSN sotto il ghiaccio e nell'Ochotsk a distruggere gli SSBN; l'Akula (1983-86) è silenzioso
  quasi come i battelli USA `[CIMSEC-DIS]` `[NSL-BAS]` (media).
- **Conteso:** se la strategic ASW fosse realistica o sensata `[CIMSEC-DIS]`.
- **Giocabilità:** già coperto in parte da *Passerella* (Kunashir). Proposta: la versione **Barents d'inverno**, con il
  margine del ghiaccio e la notte polare, lato Alfa in difesa (P03). Motore: serve il **ghiaccio come ostacolo e come
  rumore** (docs/15 M2 lo prevede) [MOTORE+].

### E32 — Il SOSUS e la barriera GIUK (1962-1991)
- **Dove:** **`faroe`**.
- **Fatti:** il 26 giugno 1962 la prima scoperta SOSUS di un diesel sovietico; il 6 luglio 1962 la prima di un SSN
  sovietico che passa il varco GIUK `[GS-SOSUS]` (media); il sistema cambia nome in IUSS nel 1985 e il suo compito resta
  segreto fino al 1991 `[WP-SOSUS]` (media); i sovietici diventano più silenziosi anche grazie alla rete Walker
  `[WP-SOSUS]` (media).
- **Giocabilità:** è la cornice di E21, E25 e del pattern P01; il datum SOSUS c'è già (`docs/09`, `cue = { tipo =
  "sosus" }`).

---

## 3. Schemi operativi ricorrenti

Gli episodi si riducono a dodici schemi. Ogni schema vale per i due lati e può generare scenari **del tutto originali**.

| # | Schema | Episodi | Lati | ROE tipiche | Teatri migliori |
|---|---|---|---|---|---|
| P01 | Pattuglia di barriera | E21, E25, E32 | 688 caccia, Alfa passa | pace | faroe, gibilterra, kunashir |
| P02 | Pedinamento di un SSBN | E05, E13, E23, E24 | 688 (Yankee, Delta), Alfa (boomer USA) | pace | celtica, barents |
| P03 | Scorta di SSBN / difesa del bastione | E31 | Alfa | pace, guerra | barents, kunashir |
| P04 | Raccolta d'informazioni presso una base | E05, E28, E12, E15 | 688, Alfa | pace, crisi | barents, faroe, baltico |
| P05 | Cavo o dispositivo sul fondo | E08, E29 | 688 (posa), Alfa (difesa) | pace | kunashir, barents |
| P06 | Incursori | E15, E17 (Onyx) | 688, Alfa | pace, crisi | baltico, faroe |
| P07 | Minamento di uno stretto o di un porto | E09, E26 | 688, Alfa | guerra (o crisi negata) | gibilterra, barents, otranto |
| P08 | Attraversamento di uno stretto | E11 | Alfa, 688 | pace | gibilterra, kunashir, baltico |
| P09 | Attacco a un convoglio o a un gruppo | E10, E16 | entrambi | guerra | celtica, otranto |
| P10 | Soccorso e recupero | E03, E04, E07, E23, E27 | entrambi | pace | celtica, barents, kunashir |
| P11 | Pedinamento di un gruppo portaerei | E11, E19, E20 | Alfa (e 688 contro Kirov) | pace, crisi | kunashir, otranto, celtica |
| P12 | Evasione dopo la scoperta | E02, E17, E18 | entrambi | crisi, guerra | celtica, faroe, baltico |

### P01 — Pattuglia di barriera
- **Come funziona:** il SOSUS dà un contatto con ritardo e un'area d'incertezza; un P-3C posa boe; il 688 aspetta al
  punto di passaggio (soglia Islanda-Faroe, canale Faroe-Shetland) e deve **classificare**, non affondare.
- **Fasi:** allerta → datum → avvicinamento a bassa velocità → classificazione → seguire fino al passaggio di consegne.
- **Tensione:** più contatti che risorse (Atrina), biologici e pescherecci come falsi bersagli, cambio di strato.
- **Motore:** c'è (datum SOSUS, aerei, `scoprire` con `minimo`).

### P02 — Pedinamento di un SSBN
- **Come funziona:** agganciare all'uscita (E13) o in pattugliamento; restare nel settore poppiero, fuori dal cono di
  ascolto, compensare i «Crazy Ivan».
- **Tensione:** collisione, perdere il contatto, essere scoperti dal pedinatore del boomer; nel lato sovietico, il boomer
  USA è molto più silenzioso.
- **Motore:** serve l'obiettivo «**contatto mantenuto per T entro R**» [MOTORE+] e la collisione.

### P03 — Scorta di SSBN e difesa del bastione
- **Come funziona:** l'SSN di scorta precede il boomer di 5-15 nm sull'asse della minaccia (dossier 03 §C4); gli aerei
  coprono l'area; le navi stanno all'ingresso.
- **Tensione:** distinguere l'intruso dai propri; non attirare l'attenzione sul boomer.
- **Motore:** c'è; il ghiaccio e la notte polare lo rendono nuovo.

### P04 — Raccolta d'informazioni presso una base
- **Come funziona:** avvicinarsi alla costa, foto al periscopio, ascolto delle comunicazioni (albero ESM), firma acustica
  delle unità che escono; uscire senza contatti.
- **Tensione:** acque territoriali, fondale basso, esposizione degli alberi (dossier 20 e 11), pattuglie ASW.
- **Motore:** `scoprire` + `non_essere_scoperto` + zone ci sono; mancano condizioni sugli **alberi alzati** e sulla
  **quota** e la penalità per le acque territoriali [MOTORE+].

### P05 — Cavo o dispositivo sul fondo
- **Come funziona:** raggiungere un punto, restare quasi fermi a quota costante per ore (palombari), uscire.
- **Tensione:** il tempo, il meteo, la nave che si ferma sopra; nella versione sovietica, trovare l'intruso fermo.
- **Motore:** serve «**resta in zona per T sotto V nodi**» [MOTORE+].

### P06 — Incursori
- **Come funziona:** trasporto di una squadra (`ospiti`), avvicinamento notturno, quota periscopica o superficie per lo
  sbarco, attesa, recupero.
- **Riferimenti:** HMS *Onyx* (Oberon) imbarca SAS e SBS ad Ascensione nel 1982 e urta una secca non segnata a 46 m durante
  una di queste missioni `[WP-ONYX]` (media); i presunti mini sommergibili in Svezia (E15).
- **Motore:** `ospiti` e `operazione_speciale` ci sono; manca un **evento di sbarco e recupero** (stare fermi in superficie
  o a quota periscopica T minuti in un cerchio) [MOTORE+].

### P07 — Minamento
- **Come funziona:** il 688 posa Mk 67 SLMM, mine semoventi su corpo di siluro Mk 37, in servizio dal 1979, per fondali
  sotto i 100 m, con una corsa di circa 8 nm che evita di passare sopra il punto `[WP-SLMM]` `[USNI20M]` (media); l'Alfa
  posa mine dai tubi.
- **Tensione:** il canale sorvegliato, il tempo, la ricarica dei tubi rumorosa (roadmap), le proprie mine (E09).
- **Motore:** l'obiettivo `minare` c'è (Enhanced, M2b).

### P08 — Attraversamento di uno stretto
- **Come funziona:** la procedura sovietica di Gibilterra (E11): quota periscopica davanti al faro, un rilevamento radar
  di un paio di minuti, poi la traversata veloce con la corrente; o la traversata silenziosa sotto una nave mercantile.
- **Tensione:** l'esposizione per fare il punto, il fondale, le correnti e le onde interne (dossier 03 A3), il SOSUS dello
  stretto.
- **Motore:** c'è; serve la penalità di scoperta per l'ecoscandaglio attivo (se non c'è già).

### P09 — Attacco a un gruppo o a un convoglio
- **Note:** già coperto dalle missioni originali; i casi nuovi sono il **gruppo cacciatore lento** (E10) e le **ROE che si
  allargano** (E16).

### P10 — Soccorso e recupero
- **Come funziona:** un'unità in emergenza (fantasia), il più vicino accorre, guida le navi civili, tiene lontani gli
  osservatori; oppure la guardia a un recupero segreto (E03).
- **Motore:** serve il **battello IA in avaria** e il soccorso (roadmap) [MOTORE+].

### P11 — Pedinamento di un gruppo portaerei
- **Come funziona:** l'SSN sovietico fa da «pedinatore» per i missili (E11) o raccoglie informazioni (E20); il gruppo
  manovra per scrollarselo; in crisi un ordine può farlo diventare un attacco.
- **Tensione:** velocità del gruppo (l'Alfa può tenerla, ma fa rumore), meteo, salire alla cieca davanti alla prora.
- **Motore:** c'è; mancano il contatto mantenuto e la collisione.

### P12 — Evasione dopo la scoperta
- **Come funziona:** la caccia «fino all'esaurimento» (E02), lo scontro in superficie con elicotteri (E17), la lunga
  caccia in acqua bassa (E18).
- **Note:** il lato del 688 è già *Scrollatevelo di dosso* e *Fuga*; la novità è il lato dell'**Alfa in crisi** con le bombe
  di segnalazione e gli aerei che in crisi non sparano.

---

## 4. Che cosa serve al motore

Tabella delle mancanze che ricorrono negli episodi, in ordine di utilità per la libreria (STIMA del numero di missioni
della shortlist che ne beneficiano).

| # | Mancanza | Episodi | Missioni (§6) | Note |
|---|---|---|---|---|
| M1 | Obiettivo «contatto mantenuto per T entro R da un'unità» | E02, E05, E06, E19, E20, E23 | 7 | le zone sono fisse, quindi `unita_in_zona` non basta: serve una condizione nuova che misuri la distanza dall'unità |
| M2 | Condizioni sul giocatore: quota, velocità, alberi alzati, in superficie | E08, E12, E15, P05, P06 | 6 | si estende `CondizioneScenario` |
| M3 | Collisioni | E05, E06, E20, E24, E28 | 5 | dossier 16 in corso |
| M4 | Cambio di situazione (ROE) a metà missione, per regola o per ordine radio | E11, E16, E22, E26 | 5 | azione nuova «situazione»; oggi `ARMI` è una costante dello script |
| M5 | Acque territoriali e penalità di carriera | E14, E15, E28 | 4 | zona con attributo + esito di carriera (dossier 13) |
| M6 | Unità IA in emersione forzata, in avaria, rimorchiata | E02, E07, E14, E19, E23 | 4 | comportamento nuovo |
| M7 | Parte terza o neutrale armata con ROE proprie e bombe di segnalazione | E01, E12, E14, E15 | 4 | oggi neutrale = non combattente |
| M8 | Soccorso (zattere, superstiti, segnale subacqueo) | E07, E23, E27 | 2 | roadmap «emersione e salvataggio» |
| M9 | Ghiaccio come ostacolo e rumore | E31 | 1 | docs/15 M2 |
| M10 | Classi: SSBN Yankee/Delta, SSBN USA, Echo II, rimorchiatore, corvetta neutrale | quasi tutti | 8 | dati e sagome |
| M11 | Oggetto passivo sul fondo (relitto, cavo, dispositivo) | E03, E04, E08 | 3 | unità ferma con firma debole |

[GIOCO] Le missioni della shortlist sono scritte in modo da **giocarsi già oggi** con approssimazioni (regole a tempo,
zone fisse, unità ferme), e da migliorare quando arrivano M1-M4.

---

## 5. Sensibilità: morti, persone reali, ciò che resta incerto

1. **Nomi di fantasia per battelli e persone, sempre.** Gli episodi con morti (K-8, K-19, K-129, Scorpion, K-219,
   Komsomolets, Belgrano, Khukri, Ghazi) diventano scenari **«ispirati a»** con battelli inventati. I nomi reali restano
   solo nel testo storico del briefing facoltativo («Nota storica»), al passato, senza drammatizzazione.
2. **Nessuna morte come obiettivo o come spettacolo.** Nelle missioni di soccorso si contano **i salvati**; nelle
   missioni ispirate a una perdita il giocatore non assiste mai all'affondamento di un battello «reale»; i debriefing
   non usano toni trionfali per affondamenti ispirati a fatti veri (al contrario dei debriefing caricaturali
   dell'originale, che riguardano scenari inventati).
3. **Persone reali** (Archipov, Savickij, Britanov, Preminin, Mulla, Guščin, Pelton): non compaiono come personaggi né
   come voce di ordini. Si possono citare nella nota storica con fonte e con la parola «secondo».
4. **Episodi contesi** (nazionalità degli intrusi in Svezia e Norvegia, B-59, Augusta/K-219, cause di Scorpion e K-129,
   linea di base a Kildin): la missione **non decide** la verità. Strumenti: varianti per i due lati (E12, E15), paesi
   neutrali di fantasia, briefing che dice «non è mai stato accertato».
5. **Civili:** gli episodi con vittime civili (per esempio la ricerca dei resti del volo KAL 007 nel 1983, con navi USA e
   sovietiche sul posto `[KAL]`) **non** si usano come missioni.
6. **Guerre altrui** (Falkland, India-Pakistan, Golfo): solo come **dottrina** (ROE, gruppo cacciatore, mine negate), mai
   con le bandiere e le navi di quei paesi nel gioco.
7. **Lo speculativo resta speculativo:** i SSN americani a difesa del *Glomar* (Polmar e White, E03), le armi nucleari
   preparate sul *Gato* (E05), i mini sommergibili cingolati (E15) entrano solo come «voce» nel briefing o come variante
   dichiarata, mai come fatto.

---

## 6. Proposte per la libreria [GIOCO]

23 missioni. Difficoltà da 1 a 5, **STIMA**. «Oggi» = giocabile col motore attuale con approssimazioni; «M…» = migliora
con la mancanza del §4.

| # | Titolo | Ispirato a | Lato | Teatro | ROE | Diff. | Motore | Gancio |
|---|---|---|---|---|---|---|---|---|
| 1 | **Pietra Santa** | E05, E29 | 688 | barents | pace | 4 | oggi; M1, M3 | Registra la firma di un boomer che esce da Kola senza farti sentire e senza toccarlo. |
| 2 | **Campane d'edera** | E08 | 688 | kunashir | pace | 5 | oggi; M2, M11 | Passa le Curili, resta sei ore fermo sopra il cavo mentre una nave sovietica si ferma proprio lì. |
| 3 | **Il guardiano del cavo** | E08 | Alfa | kunashir | pace → crisi | 3 | oggi; M4 | Scorta la nave che va a ripulire il cavo: là sotto qualcuno sta zitto e immobile. |
| 4 | **Ombra del boomer** | E23, E24 | 688 | celtica | pace | 3 | M1, M6 | Sei ore in coda a un SSBN; poi il bersaglio emerge in emergenza e arrivano le sue navi. |
| 5 | **Atrina** | E25, E32 | 688 | faroe | pace | 3 | oggi | Il SOSUS ne ha persi cinque: ritrovane almeno tre prima che passino la barriera. |
| 6 | **Aport** | E21, E25 | Alfa | faroe | pace | 4 | oggi | Passa la soglia Islanda-Faroe a scatti e derive, con un P-3C che aspetta il datum. |
| 7 | **Quarantena** | E02 | 688 | celtica | crisi | 3 | oggi; M6 | Tieni il contatto su un diesel finché la batteria lo costringe a emergere; vietato lanciare. |
| 8 | **Nave mineraria** | E03 | 688 | kunashir | pace | 2 | oggi | Proteggi una «nave per noduli» ferma sul bacino profondo; un rimorchiatore sovietico arriva a 200 yard. |
| 9 | **Linea di base** | E28 | 688 | barents | pace | 4 | oggi; M3, M5 | Sorveglia Severomorsk: legale per la tua carta, illegale per la loro. |
| 10 | **Tappo** | E09, P07 | 688 | barents | guerra | 4 | oggi | Prima dell'alba, Mk 67 nel canale d'uscita di Kola, poi via prima che le proprie mine si armino. |
| 11 | **Fuori dalla zona** | E16 | 688 | celtica | guerra limitata | 3 | M4 | Pedina un gruppo fuori dalla zona d'esclusione e aspetta un ordine che forse non arriverà. |
| 12 | **Petroliere** | E26 | 688 | gibilterra | crisi | 3 | oggi; M4 | Tre petroliere, uno stretto, una nave civile che di notte posa mine che nessuno ammette. |
| 13 | **Vestfjord** | E30 | 688 | faroe | esercitazione | 1 | oggi | Bonifica un fiordo da un Trafalgar «arancione» prima che entri la portaerei. |
| 14 | **Eskadra** | E11 | Alfa (variante 688) | otranto | crisi | 4 | oggi; M4 | Resta in coda a una portaerei a distanza di tiro, trasmetti la posizione, aspetta: guerra o rientro. |
| 15 | **Il gruppo cacciatore** | E10 | Alfa | otranto | guerra | 3 | oggi | Due fregate ti cercano; una rallenta per provare il sonar. |
| 16 | **Valona** | E01 | Alfa | otranto | crisi | 2 | oggi; M7 | Porta fuori dalla baia la flotta espulsa, mentre quattro battelli «trattenuti» restano ai moli. |
| 17 | **La porta di Spartel** | E11, P08 | Alfa | gibilterra | pace | 3 | oggi | Entra nel Mediterraneo alla sovietica: due minuti di radar davanti al faro, poi a tutta con la corrente. |
| 18 | **Seminare la soglia** | P07 | Alfa | gibilterra | guerra | 4 | oggi | Prima ora di guerra: mine sulla soglia di Camarinal, tra onde interne e un gruppo NATO in arrivo. |
| 19 | **Il cavo della fregata** | E19 | Alfa | celtica | pace | 3 | oggi; M1, M6 | Stai nel cono sordo di una Perry e registra la sua schiera, senza finirci dentro con l'elica. |
| 20 | **Coda alla portaerei** | E20 | Alfa | kunashir | pace | 4 | oggi; M1, M3 | Ventiquattro ore dietro un gruppo Nimitz; il mare peggiora, il contatto sparisce: non risalire davanti alla prora. |
| 21 | **Arcipelago** | E15 | 688 e Alfa | baltico | crisi | 5 | oggi; M2, M5, M7 | Osserva una base in un arcipelago neutrale: fuori ti avvertono, dentro sparano. |
| 22 | **Nebbia nel fiordo** | E12 | 688 e Alfa | faroe | crisi | 4 | oggi; M7 | Il fiordo ha uno strato d'acqua dolce che ti nasconde, e una scala d'escalation che ti trova. |
| 23 | **Tenete il confine** | E14 | Alfa | baltico | crisi | 3 | oggi; M5, M6 | Un battello amico è sugli scogli in acque neutrali: copri il rimorchio senza passare la linea delle 12 miglia. |

**Equilibrio:**
- lato: 11 del 688 (1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13), 10 dell'Alfa (3, 6, 14-20, 23), 2 con le varianti (21, 22);
- ROE: pace o esercitazione 12, crisi 7, guerra 4;
- teatri: faroe 4, celtica 4, kunashir 4, barents 3, otranto 3, gibilterra 3, baltico 2;
- difficoltà: 1 → 1, 2 → 2, 3 → 10, 4 → 8, 5 → 2 (STIMA).

**Scenari del tutto originali** (non legati a un fatto preciso, costruiti sugli schemi): 10 *Tappo*, 13 *Vestfjord*,
17 *La porta di Spartel*, 18 *Seminare la soglia*, 16 *Valona* nella versione 1989. Altri tre da sviluppare dopo: un
**soccorso artico** dell'Alfa (E27, quando arriva M8), una **ricerca di un battello amico silenzioso** (E04, quando arriva
M11), un **bastione d'inverno** sotto il margine del ghiaccio (E31, quando arriva M9).

**Ordine di lavoro proposto:** prima le missioni «oggi» senza [MOTORE+] (5, 6, 8, 10, 13, 15, 17, 18), che collaudano
l'editor; poi quelle che migliorano con M1-M4, insieme a quelle funzioni; ultime le missioni che dipendono da M6-M8.

**Formato del briefing** (coerente col dossier sui briefing alla F-19): ordini in stile dell'epoca, «Nota storica»
facoltativa con le sigle delle fonti di questo dossier e la frase «ispirato a; nomi e unità sono di fantasia».

---

## 7. Riepilogo per l'editor

| Elemento ricorrente | Come si fa oggi | Come si farà |
|---|---|---|
| Datum SOSUS | `aereo_asw` con `cue = { tipo = "sosus", ritardo_s }` | idem |
| Ordine che cambia la missione | obiettivo con `da_radio` | + azione «situazione» (M4) |
| Rinforzi all'allarme | regola `allarme` → `entra` | idem |
| Acque territoriali | zona + regola `giocatore_in_zona` → messaggio, `allarme` | + penalità di carriera (M5) |
| Tenere il contatto | approssimazione: `scoprire` + durata | condizione nuova (M1) |
| Restare fermi in un punto | `raggiungere` + regola a tempo | condizione su velocità e quota (M2) |
| Nave civile ferma da proteggere | `mercantile` senza rotta, `proteggere` | idem |
| Mine | carico `minamento`, obiettivo `minare` | + mine controllate da terra (E15) |
| Emergenza di un'unità IA | messaggio + riserve che accorrono | comportamento «in avaria» (M6) |

---

## 8. Fonti

| Sigla | Fonte | URL | Conf. |
|---|---|---|---|
| `[ORIG-BR]` | Briefing originali di *688 Attack Sub*, `docs/originale/briefing_EN.txt` | locale | alta |
| `[D03]` | Dossier 03, teatri, fondali e casi di taratura (tabella dei riquadri, §C3, §C4) | locale | come indicato lì |
| `[D09]` | `docs/09_schema_missioni.md` (obiettivi, aerei ASW, carico, carriera) | locale | alta |
| `[SCEN]` | `engine/missioni/include/missioni/scenario_missione.hpp` e `scenario_missione.cpp` (costante `ARMI`) | locale | alta |
| `[NSA75]` | National Security Archive, EBB 75, «The Submarines of October» (cablogrammi, diari di bordo, memorie) | https://nsarchive2.gwu.edu/NSAEBB/NSAEBB75/ | alta |
| `[WP-B59]` | Wikipedia, Soviet submarine B-59 | https://en.wikipedia.org/wiki/Soviet_submarine_B-59 | media |
| `[WP-K129]` | Wikipedia, Soviet submarine K-129 (1960) | https://en.wikipedia.org/wiki/Soviet_submarine_K-129_(1960) | media |
| `[CIA-GLO]` | D. Robarge, «The Glomar Explorer in Film and Print», CIA *Studies in Intelligence* (con citazioni della storia interna del 1985) | https://www.cia.gov/resources/csi/static/The-Glomar-Explorer.pdf | alta |
| `[WP-AZ]` | Wikipedia, Project Azorian | https://en.wikipedia.org/wiki/Project_Azorian | media |
| `[WP-SCO]` | Wikipedia, USS *Scorpion* (SSN-589) | https://en.wikipedia.org/wiki/USS_Scorpion_(SSN-589) | media |
| `[WP-K19]` | Wikipedia, Soviet submarine K-19 | https://en.wikipedia.org/wiki/Soviet_submarine_K-19 | media |
| `[WP-GATO]` | Wikipedia, USS *Gato* (SSN-615) | https://en.wikipedia.org/wiki/USS_Gato_(SSN-615) | media |
| `[MQ-GATO]` | MaritimeQuest, «USS Gato SSN-615», 14 novembre | https://maritimequest.com/daily_event_archive/2006/nov/14_uss_gato_ssn615.htm | bassa |
| `[CIA-HER]` | CIA Reading Room, copie di stampa 1975 sull'inchiesta di S. Hersh (Holy Stone) | https://www.cia.gov/readingroom/node/1877518 | media |
| `[BMB]` | S. Sontag, C. Drew, *Blind Man's Bluff* (1998), tramite Wikipedia e recensioni | https://en.wikipedia.org/wiki/Blind_Man%27s_Bluff:_The_Untold_Story_of_American_Submarine_Espionage | media |
| `[USNCVA]` | US Naval Cryptologic Veterans Association, recensione di *Blind Man's Bluff* | https://usncva.org/programs/book-reviews/blind-man’s-bluff.html | media |
| `[WP-TAU]` | Wikipedia, USS *Tautog* (SSN-639) | https://en.wikipedia.org/wiki/USS_Tautog_(SSN-639) | media |
| `[WP-K8]` | Wikipedia, Soviet submarine K-8 | https://en.wikipedia.org/wiki/Soviet_submarine_K-8 | media |
| `[WP-IVY]` | Wikipedia, Operation Ivy Bells | https://en.wikipedia.org/wiki/Operation_Ivy_Bells | media |
| `[WP-GHA]` | Wikipedia, PNS *Ghazi* | https://en.wikipedia.org/wiki/PNS_Ghazi | media |
| `[WP-KHU]` | Wikipedia, INS *Khukri* (1958) | https://en.wikipedia.org/wiki/INS_Khukri_(1958) | media |
| `[NWCR04]` | L. Goldstein, Y. Zhukov, «A Tale of Two Fleets: A Russian Perspective on the 1973 Naval Standoff in the Mediterranean», *Naval War College Review* 57(2), 2004 (DTIC ADA422490) | https://archive.org/download/DTIC_ADA422490/DTIC_ADA422490.pdf | media-alta |
| `[WP-5E]` | Wikipedia, 5th Operational Squadron | https://en.wikipedia.org/wiki/5th_Operational_Squadron | media |
| `[WP-PL]` | Wikipedia, Pasha Liman Base | https://en.wikipedia.org/wiki/Pasha_Liman_Base | media |
| `[HIM]` | himara.net, «Porto Palermo submarine tunnel» (cronologia del 1961) | https://himara.net/blog/porto-palermo-submarine-tunnel | bassa |
| `[HOX]` | E. Hoxha, *The Khrushchevites* (1976), versione albanese ufficiale | https://www.marxists.org/reference/archive/hoxha/works/1976/khruschevites/14.htm | bassa (di parte) |
| `[CIMSEC]` | CIMSEC, «Rules of Engagement and Undersea Incursions: Reacting to Foreign Submarines in Territorial Waters» | https://cimsec.org/?p=55070 | media |
| `[CSM83N]` | *Christian Science Monitor*, 2/5/1983, «Norway fires 12 more missiles at foreign sub» | https://www.csmonitor.com/1983/0502/050218.html | media |
| `[FHS13]` | Forsvarets høgskole, «Fremmede ubåter i norske fjorder. Realitet eller myte?» (2013) | https://fhs.brage.unit.no/fhs-xmlui/handle/11250/99821 | media |
| `[WP-JM]` | Wikipedia, USS *James Madison* (con il cablogramma CIA declassificato nel 2017) | https://en.wikipedia.org/wiki/USS_James_Madison | alta (cablogramma), bassa-media (memorie russe) |
| `[WP-U137]` | Wikipedia, Soviet submarine S-363 (U-137) | https://en.wikipedia.org/wiki/Soviet_submarine_U-137 | media |
| `[WP-SWE]` | Wikipedia, Swedish submarine incidents | https://en.wikipedia.org/wiki/Swedish_submarine_incidents | media |
| `[CSM82]` | *Christian Science Monitor*, 14/10/1982, «Sweden may write off spy sub as net loss» | https://csmonitor.com/1982/1014/101403.html | media |
| `[UPI82]` | UPI, 29/12/1982, «Sources say Swedes let foreign sub go» | https://www.upi.com/Archives/1982/12/29/Sources-say-Swedes-let-foreign-sub-go/2273409986000/ | bassa |
| `[FM08]` | Försvarsmakten, «New analysis of sounds from submarine hunt» (2008) | https://www.forsvarsmakten.se/en/news/2008/02/new-analysis-of-sounds-from-submarine-hunt | alta |
| `[TUN]` | O. Tunander, «The Submarine War against Sweden», serie su Substack | https://olatunander.substack.com/p/the-swedish-submarine-war-part-i | bassa (tesi contestata) |
| `[WP-BEL]` | Wikipedia, Sinking of ARA *General Belgrano* | https://en.wikipedia.org/wiki/Sinking_of_ARA_General_Belgrano | media |
| `[WP-SF]` | Wikipedia, ARA *Santa Fe* (S-21) | https://en.wikipedia.org/wiki/ARA_Santa_Fe_(S-21) | media |
| `[WP-SL]` | Wikipedia, ARA *San Luis* (S-32) | https://en.wikipedia.org/wiki/ARA_San_Luis_(S-32) | media |
| `[WP-ONYX]` | Wikipedia, HMS *Onyx* (S21) | https://en.wikipedia.org/wiki/HMS_Onyx_(S21) | media |
| `[WP-K324]` | Wikipedia, Soviet submarine K-324 | https://en.wikipedia.org/wiki/Soviet_submarine_K-324 | media |
| `[WP-K314]` | Wikipedia, Soviet submarine K-314 | https://en.wikipedia.org/wiki/Soviet_submarine_K-314 | media |
| `[NI-K314]` | *The National Interest*, «"Sir, We Hit a Nuclear Submarine!"» | https://nationalinterest.org/node/211891 | bassa-media |
| `[USNI20]` | *Proceedings*, ottobre 2020, «Russia Basks in Cold War Glory» (letto solo l'estratto: l'articolo risponde 403) | https://www.usni.org/magazines/proceedings/2020/october/russia-basks-cold-war-glory | media |
| `[WP-ATR]` | Wikipedia, Operation Atrina | https://en.wikipedia.org/wiki/Operation_Atrina | media |
| `[WP-SID]` | Wikipedia, Action in the Gulf of Sidra (1986) | https://en.wikipedia.org/wiki/Action_in_the_Gulf_of_Sidra_(1986) | media |
| `[WP-K219]` | Wikipedia, Soviet submarine K-219 | https://en.wikipedia.org/wiki/Soviet_submarine_K-219 | media |
| `[WP-K279]` | Wikipedia, Soviet submarine K-279 | https://en.wikipedia.org/wiki/Soviet_submarine_K-279 | media |
| `[GW2RU]` | gw2ru.com, «How Soviet & American submarines RAMMED one another» | https://www.gw2ru.com/history/2937-us-soviet-american-submarines-rammed | bassa |
| `[WP-BRI]` | Wikipedia, Bridgeton incident | https://en.wikipedia.org/wiki/Bridgeton_incident | media |
| `[NHHC-SBR]` | US Navy History, «When Heritage Meets Initiative: The Story of Samuel B. Roberts (FFG-58)» | https://usnhistory.navylive.dodlive.mil/Recent/Article-View/Article/2686837/when-heritage-meets-initiative-the-story-of-samuel-b-roberts-ffg-58 | alta |
| `[WP-K278]` | Wikipedia, Soviet submarine K-278 *Komsomolets* | https://en.wikipedia.org/wiki/Soviet_submarine_K-278_Komsomolets | media |
| `[WP-KIL]` | Wikipedia, Submarine incident off Kildin Island | https://en.wikipedia.org/wiki/Submarine_incident_off_Kildin_Island | media |
| `[UPI85]` | UPI, 27/8/1985, «Ships begin NATO exercises» (Ocean Safari 85) | https://www.upi.com/Archives/1985/08/27/Ships-begin-NATO-exercises/1958493963200/ | media |
| `[IWM85]` | Imperial War Museum, film «Submarine: Ocean Safari – The Kill» | https://film.iwmcollections.org.uk/record/9544 | media |
| `[WP-NW]` | Wikipedia, Northern Wedding | https://en.wikipedia.org/wiki/Northern_Wedding | media |
| `[NSL-LEH]` | Naval Submarine League, recensione di J. Lehman, *Oceans Ventured* | https://archive.navalsubleague.org/?p=6858 | media |
| `[NWCR90]` | *Naval War College Review* 43(2), 1990 (didascalia su Team Work '88) | https://digital-commons.usnwc.edu/nwc-review/vol43/iss2/ | bassa |
| `[CIMSEC-DIS]` | CIMSEC, «Brad Dismukes on the Soviet Navy and Attacking Ballistic Missile Nuclear Submarines» | https://cimsec.org/?p=48233 | media |
| `[NSL-BAS]` | Naval Submarine League, «Bastion» (sui bastioni del Barents e dell'Ochotsk, con *Soviet Military Power* 1989) | https://archive.navalsubleague.org/?p=1589 | media |
| `[WP-SOSUS]` | Wikipedia, SOSUS | https://en.wikipedia.org/wiki/SOSUS | media |
| `[GS-SOSUS]` | GlobalSecurity, SOSUS (cronologia) | https://www.globalsecurity.org/intell/systems/sosus.htm | media |
| `[WP-SLMM]` | Wikipedia, Submarine Launched Mobile Mines | https://en.wikipedia.org/wiki/Submarine_Launched_Mobile_Mines | media |
| `[USNI20M]` | *Proceedings*, maggio 2020, «Weapons That Wait» | https://www.usni.org/magazines/proceedings/2020/may/damn-torpedoes | media |
| `[WP-ALFA]` | Wikipedia, Alfa-class submarine | https://en.wikipedia.org/wiki/Alfa-class_submarine | media |
| `[KAL]` | Wikipedia, Korean Air Lines Flight 007 (ricerca del 1983) | https://en.wikipedia.org/wiki/Korean_Air_Lines_Flight_007 | media |

---

## 9. Lacune

1. **Testo completo di *Blind Man's Bluff*** e delle fonti primarie su Holy Stone, sui cavi del Barents e sulle uscite
   di *Parche*: letti solo tramite sintesi e recensioni. Date dei cavi del Barents n.v.
2. **Operazione Aport:** solo l'estratto del *Proceedings* 2020 (403); servono le memorie russe (Černavin, articoli di
   podlodka.info) e il resoconto americano del tracciamento.
3. **Episodi operativi dell'Alfa:** nessuna fonte verificabile su pedinamenti o uscite particolari; da cercare in
   memorie di comandanti della 6ª divisione (Zapadnaja Lica) e nella stampa russa degli anni '90.
4. **Okean-70 e Okean-75:** composizione, aree e cronologia delle esercitazioni n.v. (servono per la missione 7 di E07).
5. **Norvegia 1983:** il rapporto dell'FHS (2013) non è stato letto per intero; dettagli e numero delle incursioni n.v.
6. **Commissione svedese del 1983 e SOU 1995:135:** non lette direttamente; i «sei sottomarini» vengono dalla stampa.
7. **Augusta e K-279:** dettagli della collisione e della posizione n.v.
8. **Libia 1986:** ruolo dei Foxtrot libici n.v.
9. **Distanze reali di pedinamento** (yard, settori) in pace: non pubbliche; i valori delle missioni restano STIMA e
   vanno coordinati col dossier 16 (collisioni).
10. **Holy Loch 1974:** identità del battello sovietico (K-306 per i russi) e circostanze esatte n.v. oltre al
    cablogramma.

**Rimandi interni:**
- dossier 03 §C3-C4 (casi di taratura, comportamenti dell'IA da dottrina);
- 10 e 08 (radio e ordini);
- 13 (carriera, ROE, esiti);
- 16 (collisioni);
- 17 §5 (barriera GIUK, datum SOSUS, ROE degli aerei);
- 20 (periscopio ed esposizione);
- 21 (meteo, ghiaccio, notte polare);
- 22 (bombe di segnalazione e colpi d'avvertimento in pace).
