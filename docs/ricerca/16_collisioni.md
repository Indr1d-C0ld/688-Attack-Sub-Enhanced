# 16 — Collisioni fra unità in mare (dossier di riferimento)

Progetto: remake realistico di *688 Attack Sub* (1989). Periodo: tarda Guerra Fredda, circa 1985–1991, con casi dagli anni
'60 ai 2010 dove insegnano qualcosa. Scopo: dare casi storici, fisica, frequenze e proposte per le **collisioni
accidentali (e qualche volta volute) fra unità**: sottomarino–sottomarino, sottomarino–nave, nave–nave, più reti, cavi e
siluri. Il fondo e il ghiaccio sono già trattati altrove (§0.2) e qui servono solo come termine di paragone.

---

## 0. Convenzioni, rimandi e punto di partenza

- Come in `02_acustica_oceanografia_dottrina.md` §0: **[alta]** = dato verificato su fonte primaria o secondaria
  autorevole; **[media]** = letteratura aperta credibile non controllata sulla primaria, o derivata con fisica standard;
  **[bassa]** = stima plausibile, fonte divulgativa o resoconto di una sola parte. **STIMA** = valore di taratura del
  gioco, non un dato; **PROPOSTA** = scelta di progetto da discutere.
- Le sigle fra parentesi quadre rimandano alla bibliografia (§5). «estratto» = fonte letta solo attraverso il riassunto
  di una ricerca o una citazione di seconda mano. Le fonti sono riassunte con parole mie.
- **Avvertenza sulle fonti.** Le collisioni della Guerra Fredda sono note quasi solo da libri giornalistici
  (*Blind Man's Bluff* [BMB]), da memorie di comandanti e da fonti russe; spesso le due parti danno versioni opposte
  (chi ha urtato chi, a che quota, se l'urto fu voluto). Dove le fonti divergono lo dico. Profondità e velocità sono
  quasi sempre ignote: le poche disponibili sono segnate.
- Unità: t metriche, m, kn (0,5144 m/s), MJ.

### 0.1 Che cosa esiste già nel progetto (24/10/2026)

- **Livello classico, fedele all'originale.** Fra unità **non esiste collisione**: navi e sottomarini si attraversano.
  L'originale controlla l'urto solo fra un'**arma** (flag 20h) e un'unità, nelle coppie osservatore/bersaglio dei
  sensori, con una finestra quadrata di 5 unità per asse (185 m, fino a 262 m in diagonale) e 100 piedi di quota;
  mai fra due armi, mai contro un aereo (`docs/originale/sensori_e_navigazione.md` §9.8). Il motore lo riproduce in
  `Missione::impatti_dell_originale` / `Missione::impatto` / `Missione::a_contatto` (`engine/missioni/src/missione.cpp`);
  la regola di simulazione (`--regola simulazione`) prova ogni coppia arma/unità a ogni passo con la stessa finestra.
  **[alta]**
- **Fondo.** `Missione::urta_fondo(u, v)`: danno = integrità × (v / 20 m/s)², nulla sotto 1 m/s; messaggi 30 e 9 ed
  esito 1 per il giocatore; incaglio orizzontale con velocità a zero; pilota dell'IA con franco, aggiramento e freno
  d'emergenza (`engine/README.md`, «Fondale e incagli»). Le regole a gradini per velocità (graffio, duomo, casse
  prodiere, scenario *San Francisco*) stanno in `06_fondali_e_navigazione.md` §7.5 e in `docs/08_fondali_e_navigazione.md` §7.
- **Danni.** Navi di superficie: fisica per celle e stazioni di D3 (`nucleo/danni`, `DanniNave`). Sottomarini:
  `BattelloDanni` (D6, `nucleo/battello.hpp`): compartimenti fra paratie, casse, scafo resistente per stazione, quota di
  collasso estratta fra 1,1 e 1,8 volte la nominale e abbassata dai danni, sistemi graduati con riparazioni.
  `BattelloDanni::urto(tnt, distanza, angolo, x, quota)` e `DanniSottomarini::urto(...)` sono però **urti da
  esplosione** (fattore d'urto KSF, dossier 02 §9.4): oggi non c'è un ingresso per un **urto meccanico** (energia,
  punto e direzione di contatto). Le opzioni `--urto-giocatore` / `--urto-unita` di `app/main.cpp` sono prove di
  esplosione. **[alta]** (lettura del codice)
- **Dati di forma.** `data/motore/classi.toml` ha `lunghezza_m`, `larghezza_m` e `pescaggio_m` per tutte le classi
  (per i sottomarini la larghezza è il diametro dello scafo: 688 10,06 m); `scafi/<classe>.toml` ha sezioni, celle,
  compartimenti e dislocamenti. **Manca l'altezza della vela** (falsatorre) e degli alberi alzati. **[alta]**
- **Strumenti.** `cpa()` (punto di massimo avvicinamento) esiste per la carta (`engine/test/prove_strumenti_carta.cpp`).
- **Quote.** Nel motore la quota è quella della **chiglia** (`engine/README.md`), ma la quota periscopica ereditata
  dall'originale è 20 ft (6,1 m, `nucleo/armi_giocatore.hpp`), che per un 688 vero è la quota dell'asse a cui la vela
  esce dall'acqua. Un test di collisione con le navi dipende da questo riferimento: va chiarito (§6, lacuna 1).

### 0.2 Che cosa non ripeto

- Crazy Ivan, pedinamento e baffles: `02` §5–§7 e `03` §C4 (manovra, intervalli, caso *Sovereign*–Delta 1978).
- Fondali, incagli, *San Francisco* 2005, ghiaccio: `06` §7.5, `docs/08` §7.
- Fattori d'urto da esplosione, allagamento da orifizio, compartimentazione del 688: `02` §9; collasso e implosione:
  `07` §8; il progetto D6: `docs/10` §10.
- CPA e allarmi della carta: `12` §5; gradi, rimozione dal comando, punteggio: `13` §2.3.

---

## 1. Casi storici

### 1.1 Sottomarino contro sottomarino (pedinamento e incontri)

| Data | Unità (chi segue chi) | Luogo, quota, velocità | Geometria e stato di scoperta | Danni | Esito e conseguenze | Fonti, fiducia |
|---|---|---|---|---|---|---|
| ott. 1968 | HMS *Warspite* (SSN) segue un Echo II | Mare di Barents (prob.); ignote | il seguito tocca poppa ed eliche del seguito | falsatorre del *Warspite* danneggiata | emerge per controllare, rientra; riparato a Barrow | [WP-Warspite] (da Ballantyne) — media |
| 15 nov. 1969 | USS *Gato* (SSN) in missione d'intelligence; K‑19 (SSBN Hotel) | Barents, ~60 m; K‑19 scendeva da 60 a 90 m | urto inatteso per entrambi: secondo la versione russa la manovra di quota del K‑19 lo porta sul *Gato*; colpo di striscio sul *Gato* all'altezza del reattore | K‑19: prua e sonar distrutti, portelli dei tubi lanciasiluri danneggiati; *Gato* quasi illeso | il K‑19 rientra in superficie; il *Gato* prosegue. Racconto non confermato: sul *Gato* si preparano SUBROC e siluri nucleari, il comandante annulla. Specialisti russi: con 2–3 kn in più il *Gato* sarebbe stato tagliato in due | [WP-Gato], [WP-ru-K19], [GW2RU], [MQ06] — media (fatti); bassa (armi, «tagliato in due») |
| 20 giu. 1970 | USS *Tautog* (SSN) segue K‑108 (Echo II) | presso Petropavlovsk; ignote | *Tautog* nei baffles a pochi metri per lungo tempo; senza idrofoni per la quota non sa se il K‑108 è sopra o sotto. Il K‑108 scende e gli si posa addosso (secondo [GW2RU] dopo un Crazy Ivan) | K‑108: un'elica persa (un pezzo resta conficcato nella vela del *Tautog*), turbina in fuga udita; *Tautog*: vela piegata | il *Tautog* sente rumori che crede di un affondamento; il K‑108 invece rientra (rimorchiato secondo [GW2RU]). Negato da entrambi fino al 1991–1999 | [WP-Tautog], [TI], [GW2RU], [BMB] estratto — media |
| mag. 1974 | USS *Pintado* (SSN) e un Yankee (K‑408 per le fonti russe) | approcci di Petropavlovsk; ignote | il *Pintado* urta di prua; non spiegato come il sonar non l'abbia visto | *Pintado*: sfera sonar schiacciata, un portello lanciasiluri bloccato, un timone orizzontale danneggiato | il Yankee emerge; il *Pintado* resta sotto e se ne va ad alta velocità | [WP-Pintado] estratto, [TI] — media |
| autunno 1974 | K‑306 (Victor I) e USS *James Madison* (SSBN) | ignote | il cacciatore sovietico si porta in coda all'SSBN appena immerso, sbaglia la distanza e lo urta a poppa | ignoti | entrambi emergono; il sovietico si reimmerge e rientra | [TI] — bassa (una fonte, versione russa) |
| 23 mag. 1981 | HMS *Sceptre* (SSN) segue K‑211 (Delta III) | Barents; ignote | il K‑211 ferma le macchine per ascoltare la poppa; il seguito arriva troppo vicino e lo colpisce da sotto e da dietro, più urti | K‑211: un'elica, lo stabilizzatore di poppa e lo scafo leggero; *Sceptre*: prua e casing anteriore strappati, vela danneggiata, plancia persa, **l'elica sovietica incide lo scafo resistente**; reattore tenuto in moto escludendo le protezioni | entrambi rientrano; storia di copertura «iceberg»; rivelato nel 1991. Alcune fonti russe attribuiscono l'urto al *Sturgeon* | [WP-Sceptre], [TI], [GW2RU] — media |
| 11 feb. 1992 | USS *Baton Rouge* (SSN 688) e K‑276 *Kostroma* (Sierra) | 12 nm da Kildin, ~21 m [RusNavy]; impatto ~7 kn secondo la stampa [SOFREP] | il *Kostroma* risale (forse per una sessione radio) e urta da sotto la parte poppiera del 688; acqua bassa e frangenti disturbano il passivo; probabile avvicinamento dal cono cieco di 60° a poppa del 688. Versione russa: il 688, perso il contatto, si lancia verso l'ultimo punto | *Kostroma*: falsatorre ammaccata davanti e duomo sonar; frammenti delle piastrelle anecoiche del 688 ritrovati. *Baton Rouge*: graffi e due tagli (uno ~1,5 m) nella cassa zavorra di sinistra | proteste russe, incontro Baker–Eltsin; gli USA sospendono alcune attività presso le basi; il 688 radiato nel 1993 (ufficialmente per il costo della ricarica del reattore). L'equipaggio russo dipinge una stella «1» sulla vela. Ammiraglio Aleksin: il 688 fu a «5–7 secondi» dall'affondare | [WP-Kildin], [WP-ru-K276], [CSM92], [GW2RU] — alta (fatto), media (dinamica) |
| 20 mar. 1993 | USS *Grayling* (SSN) segue K‑407 *Novomoskovsk* (Delta IV) | 105 nm a N di Murmansk; K‑407 a ~74 m e 16–18 kn dopo un'inversione; *Grayling* accelera a 8–15 kn | pedinamento a 11–13 km sul rilevamento 155–165°; il Delta inverte la rotta, il *Grayling* lo perde e accelera per ritrovarlo; il passivo lo risente a ~1 km di prua (rumore dei frangenti in acqua bassa); la virata e la risalita non bastano | *Grayling* urta la prua dritta del Delta: grande graffio; danni lievi su entrambi | una settimana prima del vertice Clinton–Eltsin: Clinton promette una revisione; attività nel Barents continuate a ritmo ridotto, procedure e addestramento dei comandanti irrigiditi | [WP-Kola93] (da Miasnikov e [BMB]) — alta (fatto), media (numeri) |
| 3–4 feb. 2009 | HMS *Vanguard* e *Le Triomphant* (due SSBN) | Atlantico orientale; immersi, «velocità molto bassa» | nessuno dei due sapeva dell'altro: solo passivo, rivestimenti anecoici, bassa velocità; zone di pattugliamento sovrapposte | *Triomphant*: duomo sonar di prua; *Vanguard*: casing a dritta nella zona dei missili. Stima di stampa fino a £50 M per i due | rientro con mezzi propri; Parigi parla prima di «oggetto immerso, forse un container»; accuse di insabbiamento; nessun accordo di coordinamento (proposto già nel 1984) | [WP-VgTr], [BBC09] — alta (fatto), media (cause) |

**Casi dubbi o falsi.** Il K‑219 (1986) e il *Kursk* (2000) furono attribuiti da parte russa a collisioni con sottomarini
americani o britannici (*Augusta*; *Memphis*, *Toledo*, *Splendid*): la US Navy nega, e per il K‑219 lo negano anche il
comandante Britanov e il suo secondo; per il *Kursk* l'accusa è giudicata infondata. **Per HMS *Splendid* non ho trovato
nessuna collisione documentata** (il caso del 1986 con un Typhoon citato a volte in rete non ha fonte). **[media]**
[WP-K219], [WP-Splendid]

**Conteggio.** Le fonti russe parlano di **10–20 collisioni** fra sottomarini sovietici e occidentali nella Guerra
Fredda [TI]; un elenco russo citato da Wikipedia numera la *Sceptre* come collisione n. 22 [WP-Sceptre]; l'appendice di
[BMB] ha una lista estesa (non consultata). Nel 1975 Seymour Hersh (NYT) contava due collisioni note nel programma
Holystone [Hersh75]. **[media]** per l'ordine di grandezza: **20–25 collisioni sottomarino–sottomarino in circa
25 anni**, quasi tutte nel Barents e presso Kamčatka, quasi tutte durante un pedinamento. **Nessuna ha fatto vittime**.

### 1.2 Sottomarino contro nave di superficie

| Data | Unità | Circostanze | Danni | Esito | Fonti, fiducia |
|---|---|---|---|---|---|
| 10 nov. 1966 | USS *Nautilus* e portaerei *Essex* | esercitazione, sottomarino a bassa quota | danni (foto della vela) | riparato a Portsmouth | [WP-Nautilus] — bassa (una fonte) |
| 13 giu. 1973 | K‑56 (Echo II) **in superficie** e nave ricerca *Akademik Berg* | notte, rientro dopo lanci; radar spento per manutenzione; riacceso, 4 contatti confondono; luci viste 5 min prima; entrambi a 9 kn | squarcio di ~4 m nei compartimenti 1 e 2; il 2 si allaga, acqua nelle batterie: **cloro** | **27 morti**; il comandante porta il battello in secca su un banco | [WP-K56] — media |
| 9 apr. 1981 | USS *George Washington* (SSBN) a quota periscopica e cargo *Nissho Maru* (2.350 t) | Mar Cinese Orientale, nebbia e pioggia; nave e aerei della US Navy avevano rilevato il cargo | vela del sottomarino lievemente danneggiata; il cargo affonda in ~15 min | **2 morti**; l'equipaggio non capì che la nave era in pericolo; Giappone avvisato dopo 24 h; scuse di Reagan; comandante e ufficiale di guardia sollevati | [WP-GW] — media/alta |
| 21 ott. 1981 | S‑178 (Whiskey) **in superficie** e *Refrižerator‑13* | Vladivostok, notte; luci della nave confuse con quelle della città; S‑178 a 9 kn, la nave a 8 kn; la nave doveva dare precedenza | colpo a 20–30° sul fianco sinistro, falla ~2 m² nel 6° compartimento, allagato in 15–20 s; porte stagne non chiuse; ~130 t imbarcate, **affonda in ~40 s** su 31 m | 31–32 morti; i due comandanti condannati a 10 anni | [WP-S178] — media |
| 21 mar. 1984 | K‑314 (Victor I) e portaerei *Kitty Hawk* | Mar del Giappone, Team Spirit; perso il gruppo nel maltempo, il sottomarino risale a ~10 m per guardare e si trova il gruppo a 4–5 km; poi sotto la portaerei di notte | K‑314 rovesciato dal primo urto, una pala dell'elica spezzata (resta nello scafo della portaerei); *Kitty Hawk*: ammaccatura superficiale | il K‑314 non può muovere: rimorchiato; la terza collisione USA–URSS in ~5 mesi secondo la stampa | [WP-K314], ricerca su [NI-K314] estratto — media |
| 9 feb. 2001 | USS *Greeneville* (688) e *Ehime Maru* (741 GT) | dimostrazione per civili; ricerca al periscopio di ~1,5 min invece della prassi, tratti di analisi del moto accorciati da 3 min a 90 s, display sonar analogico guasto, il contatto S‑13 non aggiornato; **emersione d'emergenza** da ~405 ft in ~50 s | il timone del 688 squarcia lo scafo: la nave perde energia in 5 s e affonda in 5–10 min; sul 688 piastrelle per 7 m e timone forato (2 M$) | **9 morti**; il comandante Waddle punito e costretto al congedo; scuse del Presidente, inviato speciale, ~28 M$ di indennizzi, recupero del relitto 60 M$ | [WP-Ehime], [NTSB01] estratto — alta |
| 13 nov. 2002 | USS *Oklahoma City* e metaniera *Norman Lady* | a E di Gibilterra | periscopio e vela danneggiati | comandante sollevato | [WP-Inc2000] — media |
| 5 set. 2005 | USS *Philadelphia* e mercantile turco | Golfo Persico | lievi | comandante sollevato | [WP-Inc2000] — media |
| 8 gen. 2007 | USS *Newport News* e petroliera *Mogamigawa* | stretto di Hormuz, sottomarino in immersione; la nave gli passa sopra e l'**effetto Venturi** lo risucchia verso l'alto | prua del 688; scafo ed eliche della petroliera | rientro in superficie scortato (non poteva immergersi); comandante sollevato; indennizzo | [WP-NN] — media/alta |
| 20 mar. 2009 | USS *Hartford* (688I) a quota periscopica e USS *New Orleans* (LPD) | Hormuz, notte; dopo 63 giorni in zone affollate: nessun piano per l'attraversamento, comandante assente dalla centrale, gestione dei contatti scadente, dati sonar non riconosciuti, navigatore con l'iPod | il 688 **rolla di ~85°**: vela, periscopio e timone di prua sinistro; *New Orleans*: falla 5×5,5 m, cisterna squarciata, 95 m³ di gasolio in mare, due casse zavorra | 15 feriti lievi; 120 M$ per il sottomarino; comandante e capo dell'equipaggio sollevati, altri 13 puniti | [WP-HNO], [NBC09] — alta |
| 13 ott. 2012 | USS *Montpelier* (688) e incrociatore *San Jacinto* | esercitazione ASW; il sottomarino risale a quota periscopica 100–200 yd davanti alla nave; «macchine indietro» dell'incrociatore e «emergency deep» del sottomarino tardivi | duomo sonar dell'incrociatore; 43–81 M$ | comandante sollevato: errore umano, squadra di guardia, procedure della quota periscopica non seguite; anche l'incrociatore distratto dalle operazioni di volo | [Mont13] estratto — media/alta |
| 20 lug. 2016 | HMS *Ambush* a quota periscopica e mercantile | Gibilterra, addestramento | parte alta della vela | — | [WP-Inc2000] — media |

**Quasi collisione documentata.** Nov. 2018, canale del Nord: un SSN britannico a quota periscopica stima il traghetto
*Stena Superfast VII* a 6.000 yd e ne sottostima la velocità; il traghetto vede il periscopio a prua e accosta, passaggio
a **50–100 m** (250 m per una fonte). La MAIB nota che il quadro tattico mostrava una situazione più sicura del vero; è
il terzo caso in quattro anni. **[media]** [MAIB20]

### 1.3 Reti, cavi di rimorchio, aspirazione

- **FV *Antares*** (22 nov. 1990, Arran): HMS *Trenchant*, in un corso per comandanti, accosta per evitare un contatto,
  sente un colpo, aggancia il cavo della rete; il peschereccio di 17 m si capovolge e affonda, **4 morti**; il sottomarino
  emerge con il cavo sul casing, non riesce a chiamare, riferisce e si reimmerge; ricerche partite con 8 h di ritardo.
  La MAIB conta **15 incidenti sottomarino–peschereccio nel 1980–1990** (Regno Unito); dopo l'inchiesta la distanza
  di sicurezza dai pescherecci passa da 2.000 a 3.000 yd; l'ufficiale responsabile è condannato al biasimo. **[alta]**
  [WP-Antares]
- **Rimorchiatore *Barcona*** (14 giu. 1989, Long Beach): l'USS *Houston* aggancia il cavo di rimorchio, il rimorchiatore
  affonda, 1 morto; un altro rimorchiatore trascinato all'indietro nel 1988. Una raccolta FOIA del 1989 conta **almeno
  42 collisioni di sottomarini USA dal 1983**, di cui 14 con rimorchiatori o pescherecci e loro reti, ancore e cavi;
  Greenpeace contava **36 agganci di pescherecci nel mondo dal 1952**. **[media]** [PT89]
- **FV *Karen*** (15 apr. 2015, Mar d'Irlanda): un sottomarino britannico aggancia la rete e trascina il peschereccio
  all'indietro, che resta a galla; contatto sonar classificato male. **[media]** [WP-Inc2000]
- **Aspirazione.** Il *Newport News* (2007) mostra l'interazione idrodinamica sotto una nave grande in acqua ristretta
  (§2.7). **[media]**

### 1.4 Urti deliberati in tempo di pace

- **Mar Nero, 12 feb. 1988.** L'incrociatore *Yorktown* e il cacciatorpediniere *Caron* esercitano il passaggio
  inoffensivo a ~10,5 nm dalla Crimea. Dopo un avviso su VHF 16 («lasciate le acque o vi urteremo»), la fregata
  *Bezzavetnyj* (Krivak, circa un terzo del dislocamento dell'incrociatore) si affianca a sinistra e accosta contro lo
  *Yorktown*; la *SKR‑6* (Mirka, circa un quarto del *Caron*) urta il *Caron*. Danni: due contenitori di Harpoon dello
  *Yorktown* (la prua della fregata scorre lungo l'anca sinistra), vernice graffiata sul *Caron*; la *Bezzavetnyj* perde
  l'ancora di dritta. Nessun ferito, nessuna falla. Seguono una protesta e una dichiarazione congiunta USA–URSS sul
  passaggio inoffensivo (1989). **[alta]** [WP-BlackSea]
- **Lezione per il gioco:** un urto voluto è **radente, a bassa velocità relativa, da poppavia del traverso, con la
  prua rinforzata dell'urtante contro la murata dell'urtato**: danni a sovrastrutture e armi di coperta, mai allo scafo
  sotto il galleggiamento. È un'azione politica: avviso radio, poi manovra, poi nota diplomatica.
- **Accordo INCSEA (25 mag. 1972).** Impone le regole di rotta, di stare ben discosti, di non simulare attacchi, di non
  intralciare le formazioni e di segnalare le esercitazioni con **sottomarini in immersione** (art. III.7) [INCSEA].
  Non regola gli incontri fra sottomarini immersi: nel 1992 la Russia propose di estenderlo ai sottomarini [CSM92].
  Vale solo in alto mare, quindi non nel caso del Mar Nero. **[alta]**
- **K‑276 «Udar Loktem».** Una versione russa sostiene che il *Kostroma* urtò di proposito per scrollarsi di dosso il
  pedinatore; l'inchiesta russa giudicò corrette le azioni del proprio equipaggio. **[bassa]** [WP-ru-K276]

### 1.5 Speronamento in guerra (per contrasto)

- **USS *Borie* contro U‑405** (1 nov. 1943): il cacciatorpediniere sperona, un'onda gli porta la prua sopra la coperta
  del sommergibile; restano incastrati a V e si combatte con armi leggere. Il U‑405 affonda con tutti i 49 uomini; il
  *Borie* ha le cuciture aperte a prua, il locale macchine prodiero allagato, perde l'energia e l'acqua per le caldaie e
  il giorno dopo va abbandonato e affondato (27 morti nel recupero). **[alta]** [WP-Borie]
- **HMS *Harvester* contro U‑444** (marzo 1943): sperona, danneggia le eliche, resta lento e viene silurato dal U‑432 il
  giorno dopo (145 morti). Con il caso *Hesperus* (tre mesi in bacino dopo il U‑357) spinse la Royal Navy a
  **scoraggiare lo speronamento dall'inizio del 1943** (fascicolo ADM 1/12591). **[media]** [WP-Harvester], [WP-Ram]
- **Lezione:** anche contro un battello in superficie, piccolo e già colpito, l'urtante leggero perde la prua o le
  eliche. Due navi della Guerra Fredda che si urtano a 10–20 kn non sono un «tocco».

### 1.6 Schemi ricorrenti

| Schema | Meccanismo | Casi | Rischio nel gioco |
|---|---|---|---|
| **A. Manovra del seguito** | Crazy Ivan, inversione, cambio di quota o arresto per ascoltare mentre il seguitore è vicino e in coda | *Tautog*, *Gato*, K‑211/*Sceptre*, *Pintado* | pedinamento in coda sotto 1–2 km |
| **B. Corsa per riprendere il contatto** | il seguitore perde il contatto dopo la manovra e accelera verso l'ultimo punto, sordo per la propria velocità | *Grayling*, *Baton Rouge* (versione russa), K‑314 | il caso più «colpa del giocatore» |
| **C. Risalita senza quadro** | risalita a quota periscopica o in superficie con analisi del moto incompleta, ricerca al periscopio breve, contatti non aggiornati | *Greeneville*, *Montpelier*, *Hartford*, *George Washington*, *Kostroma*, K‑314, *Ambush* | il più frequente in tempo di pace |
| **D. Due unità silenziose** | entrambe lente e coperte da rivestimenti, nessuna sa dell'altra, aree sovrapposte | *Vanguard*/*Triomphant* | raro, puro caso |
| **E. Superficie e traffico** | in superficie di notte, radar spento o confuso, luci fraintese | S‑178, K‑56 | battelli convenzionali, ingressi in porto |
| **F. Attrezzi da pesca e cavi** | reti, cavi di rimorchio, ancore | *Antares*, *Barcona*, *Karen* | acque costiere e basse |
| **G. Interazione idrodinamica** | una nave grande passa sopra un sottomarino poco profondo | *Newport News* | stretti (Hormuz, Gibilterra) |
| **H. Urto voluto** | radente, avviso radio, scopo politico | Mar Nero 1988 | missioni di tempo di pace |

**[media]** (sintesi dell'autore dai casi sopra).

---

## 2. Fisica e danni

### 2.1 Energia d'urto

- **Energia disponibile per la deformazione** (urto anelastico, componente normale al contatto):
  `E = ½ · μ · v_n²`, con `μ = m₁′·m₂′ / (m₁′ + m₂′)` e `m′ = m·(1 + k)`, k = coefficiente di **massa aggiunta**
  nella direzione dell'urto. La componente tangenziale si dissipa per attrito e strisciamento (graffi, piastrelle). **[alta]**
  (meccanica; impostazione di [Min59])
- **Masse aggiunte.**
  - Sottomarino ≈ ellissoide allungato con L/D ≈ 8–11: **k assiale ≈ 0,02–0,03**, **k trasversale (laterale e
    verticale) ≈ 0,95**. **[alta]** [Newman77]
  - Nave di superficie: **0,4** per il moto laterale (valore classico di Minorsky; dipende dalla durata dell'urto),
    ~0,05–0,1 longitudinale (STIMA), ~1 verticale (STIMA). **[media]** [Min59], [TRB-AM]
  - Conseguenza: chi urta di prua conta con la sua massa quasi nuda, chi è preso al traverso «pesa» il doppio.
- **Esempi** (calcolo dell'autore, **[media]**):

  | Caso | v_n | E |
  |---|---|---|
  | 688 (6.927 t) di prua contro il fianco di un Victor III (7.250 t) | 2 kn | 2,5 MJ |
  | idem | 5 kn | 16 MJ |
  | idem | 10 kn | 62 MJ |
  | idem | 20 kn | 250 MJ |
  | idem, radente a 20° (solo la componente normale) | 5 / 10 kn | 1,8 / 7,3 MJ |
  | 688 che risale a 2,5 m/s sotto una *Perry* (4.100 t) | 2,5 m/s | 16 MJ |
  | idem sotto una petroliera o una *Nimitz* (100.000 t) | 2,5 m/s | 40 MJ |
  | nave da 5.000 t a 8 kn di prua sul fianco di un 688 in superficie | 8 kn | 32 MJ |
  | *Spruance* a 15 kn, o *Kitty Hawk* a 10 kn, sul fianco di un 688 | — | ~155 MJ |
  | *San Francisco* (2005) contro un monte a ~33 kn (massa infinita) | 17 m/s | ~1.000 MJ |

  Per confronto: un Mk 48 porta ~1,2 t di TNT equivalente per la bolla, cioè ~5.000 MJ, ma a distanza; l'urto
  meccanico concentra pochi MJ in pochi metri quadrati.
- **Ripartizione.** L'energia si divide fra le due strutture secondo la loro rigidezza nel punto di contatto: una prua
  rinforzata (o la vela per il ghiaccio del Sierra e del Typhoon) cede meno di una cassa zavorra o di un duomo in
  vetroresina. STIMA di gioco: metà ciascuno, poi 70/30 a favore della parte più rigida (tabella §4.3).

### 2.2 Assorbimento: Minorsky e le zone di schiacciamento

- **Relazione di Minorsky** (collisioni fra navi ad alta energia): `E [MJ] = 47,2 · R_T + 32,7`, con R_T = volume di
  acciaio distrutto nelle due navi [m³]. Vale solo sopra qualche decina di MJ; sotto, la deformazione è elastica o
  locale (ammaccature, graffi). **[media]** [Min59] tramite [Min-sec]
- **Taratura con il *San Francisco*:** ~1 GJ in un urto frontale; la formula dà ~21 m³ di acciaio, e infatti duomo,
  sfera sonar e casse zavorra prodiere sono distrutti mentre lo scafo resistente resta integro. **La prua libera di un
  688 (duomo, sfera, casse: ~14 m davanti alla paratia di prua, cella `Z_PRUA` da x = 41 a 55 m in
  `scafi/los_angeles.toml`) è una zona di schiacciamento capace di ~1 GJ**. **[media]**
- **Sotto i 20–30 MJ** (pedinamenti a pochi nodi, casi *Grayling*, *Vanguard*, *Baton Rouge*): danni a duomo, vela,
  casing, eliche, piastrelle; nessuna falla nello scafo resistente salvo un'elica che lo incide (*Sceptre*). **[media]**

### 2.3 Danni per parte e per angolo

| Parte colpita | Chi la perde di solito | Effetto | Esempi |
|---|---|---|---|
| **Duomo e sfera sonar di prua** | l'urtante di prua | sonar sferico (BQS‑13 / MGK) fuori uso; portelli dei tubi bloccati; spesso una cassa zavorra prodiera allagata | K‑19, *Pintado*, *Triomphant*, *Sceptre*, *San Jacinto* |
| **Vela e alberi** | chi risale sotto un altro o sotto una nave | periscopi, antenne, ESM persi; timoni di vela (688 Flight I/II) bloccati; vela piegata | *Tautog*, *Kostroma*, *Hartford*, *George Washington*, *Oklahoma City*, *Ambush*, *Jacksonville* |
| **Eliche, timoni di poppa, stabilizzatori** | il seguito colpito da dietro, o chi passa sopra un'elica | elica persa o sbilanciata (vibrazioni, rumore, velocità ridotta), turbina in fuga, timone bloccato | K‑108, K‑211, K‑314, *Warspite* (urtante), *Harvester* |
| **Casing, casse zavorra, rivestimenti** | colpi radenti | piastrelle anecoiche strappate (firma più alta), casse zavorra forate (riserva di spinta minore) | *Vanguard*, *Baton Rouge*, *Greeneville*, *New Orleans* |
| **Scafo resistente** | colpo normale ad alta energia, o elica che lo taglia | falla con portata che cresce con la quota; scafo deformato (collasso più basso) | *Sceptre* (inciso), *Baton Rouge* (sospetto) |

- **Angolo.** Con l'angolo θ fra le rotte (o fra la rotta relativa e la superficie), `v_n = v_rel·sin θ`: a 20° resta il
  34% della velocità e il 12% dell'energia. I colpi radenti strisciano: graffi lunghi, piastrelle, timoni ed eliche
  sul lato. I colpi frontali o al traverso concentrano: duomo, prua, falla. **[alta]** (geometria)
- **Rollio e assetto.** Un sottomarino preso di lato vicino alla superficie, o sollevato da una chiglia, rolla
  violentemente: *Hartford* ~85°, K‑314 rovesciato; feriti a bordo per gli urti contro le strutture. **[media]**
- **Urto verticale.** Chi risale sotto una nave colpisce con la vela, i timoni di vela e (se la nave pesca molto) il
  dorso; chi scende su un altro (K‑108) colpisce con la chiglia e le eliche.

### 2.4 Che cosa rende letale una collisione

- **Lo scafo resistente.** Il casing, la vela, le casse e il duomo sono allagabili per progetto: danneggiarli non
  affonda il battello. **Muore chi apre lo scafo resistente o un compartimento stagno.** In tutti i casi
  sottomarino–sottomarino noti lo scafo resistente ha retto, perché le velocità relative erano di pochi nodi e i colpi
  radenti. **[media]**
- **La quota moltiplica la falla.** Portata `Q = C_d·A·sqrt(2 g h)` (`02` §9.1): un foro di 0,1 m² imbarca ~1,2 m³/s a
  20 m e ~3,3 m³/s a 150 m. La riserva di spinta di un 688 è ~14% (≈ 950 t, `scafi/los_angeles.toml`): a 150 m un foro
  così consuma la riserva in ~5 minuti, meno del tempo per risalire se la propulsione è persa. **[media]** (calcolo)
- **In superficie la falla è vicina al pelo dell'acqua ma enorme:** S‑178 (2 m² a pochi metri) imbarca 130 t e affonda
  in 40 s con le porte stagne aperte; K‑56 sopravvive solo perché le porte si chiudono e il battello va in secca. La
  differenza fra vita e morte è **la chiusura delle porte stagne nei primi secondi** (dottrina d'allagamento, `02` §9.2).
  **[media]**
- **Collasso.** Un'ammaccatura dello scafo resistente è un difetto di circolarità: in uno studio numerico, uno scarto di
  appena 0,5% del raggio toglie ~16% della pressione di collasso; la corrosione di uno scafo HY‑80 sposta il collasso dove
  sta il difetto maggiore. Un'ammaccatura da collisione è molto più grande di 0,5%: la quota di collasso scende in modo
  drastico e la quota di collaudo non è più sicura. **[media]** [OOC24], [Stel21]. Nel motore la quota di collasso si
  abbassa già con i danni allo scafo (`docs/10` §10.3 n. 6): la collisione è un nuovo ingresso per lo stesso meccanismo.
- **Effetti secondari letali:** acqua di mare nelle batterie → cloro (K‑56); reattore scramato e propulsione persa
  (*Sceptre* evitò lo SCRAM escludendo le protezioni); emersione d'emergenza in traffico (Ehime Maru). **[media]**

### 2.5 Navi di superficie

- **Sottomarino che risale sotto una nave piccola:** vela e timone tagliano lo scafo dal basso come una lama; la nave
  perde subito propulsione ed energia e affonda in minuti (*Ehime Maru* 5–10 min, *Nissho Maru* ~15 min). Il sottomarino
  se la cava con danni a vela, timoni e piastrelle. **[alta]**
- **Nave grande:** chiglia e opera viva con falla e casse forate, ma galleggia (*New Orleans*: 5×5,5 m, cisterna di
  gasolio e due casse; *Kitty Hawk*: ammaccatura; *Mogamigawa*: scafo ed eliche). **[alta]**
- **Navi con duomo sonar a prua** (*Spruance*, *Perry*, *Ticonderoga*, Krivak): il duomo sotto la chiglia è la prima
  cosa che tocca un sottomarino a quota periscopica (*San Jacinto*). **[media]**
- **Nave–nave:** urti voluti radenti (Mar Nero) danneggiano sovrastrutture e lanciatori; urti veri di prua sul traverso
  aprono falle sotto il galleggiamento (la fisica D3 lo gestisce già con le celle). **[alta]**
- **Pescherecci:** il pericolo non è lo scafo ma la **rete o il cavo**: il peschereccio viene trascinato all'indietro,
  si capovolge e affonda in pochi secondi (*Antares*). **[alta]**

### 2.6 Effetti su sensori, propulsione, governo e rumore

- **Sonar:** duomo perso = sonar di prua sordo o con settore cieco; la schiera rimorchiata si impiglia o si strappa se
  l'altro passa a poppa (`04` §3.3 per la schiera sul fondo). **[media]**
- **Propulsione:** elica piegata o persa → velocità massima ridotta, vibrazioni, nuove righe tonali alla frequenza
  delle pale e cavitazione precoce. K‑108 e K‑314 dovettero essere rimorchiati. **[media]**
- **Governo:** timone o piani bloccati alla posizione che avevano (come in D6). **[media]**
- **Rumore:**
  - l'urto è un **transitorio metallico forte** udibile da tutti nei dintorni (STIMA: 180–190 dB re 1 µPa a 1 m, sopra il
    transitorio di bordo più forte di `02` §10.3) **[bassa]**;
  - seguono stridii di strisciamento (l'equipaggio del K‑407 sentì un urto e uno stridio prima di scoprire il
    sottomarino) **[media]**, [WP-Kola93];
  - poi rumori di danno: turbina in fuga, elica sbilanciata, acqua che entra, aria per le casse; il *Tautog* scambiò
    questi rumori per un affondamento [WP-Tautog] **[media]**.
- **Scoperta reciproca:** dopo l'urto entrambi sanno, chiunque fosse il più silenzioso. È la fine del pedinamento e,
  in tempo di pace, l'inizio di un incidente politico.

### 2.7 Interazione idrodinamica e quota periscopica

- Due scafi vicini in moto si attraggono (depressione fra gli scafi, effetto Venturi), tanto più quanto sono vicini,
  veloci e in acqua ristretta. Una nave grande che passa sopra un sottomarino poco profondo lo risucchia verso
  l'alto (*Newport News* sotto la *Mogamigawa*). **[media]** [WP-NN]
- **Geometria a quota periscopica.** Un 688 a quota periscopica ha la chiglia a ~18–20 m (60–65 ft per *Greeneville* e
  *Hartford*) [WP-Ehime], [WP-HNO]; con scafo di 10 m il dorso sta a ~8–10 m, la vela arriva a 1–3 m dalla
  superficie (STIMA). Con i pescaggi di `classi.toml`: **ogni nave del gioco può prendere la vela; quelle con più di
  ~8 m (petroliera 14, Iowa e Sacramento 11,6, Nimitz 11,3, cargo 10,5, Kidd 9,6, Kirov 9,1, mercantile 9) arrivano
  al dorso dello scafo.** **[media]** (calcolo)
- **Emergency deep:** la risposta standard a quota periscopica a una nave troppo vicina è scendere subito con
  angolo di discesa sotto la chiglia della nave (*Montpelier*: tardi). **[media]** [Mont13]

---

## 3. Frequenza e cause

### 3.1 Quanto spesso

| Misura | Valore | Fonte, fiducia |
|---|---|---|
| Collisioni di sottomarini USA (tutte: navi, rimorchiatori in porto, reti, altri sottomarini) | ≥ 42 dal 1983 a metà 1989 (~7 all'anno); 5 con sottomarini, 15 con rimorchiatori | [PT89] — media (FOIA, elenco «non necessariamente completo») |
| Idem, anni '80 | ~6 all'anno su oltre 130 SSN e SSBN (~4,5% per battello all'anno) | [SP10] — bassa |
| Regno Unito | ~6,4% per battello all'anno; 14 collisioni dei sottomarini nucleari dal 1988 al 2009 (una sola con un sottomarino); 17 secondo una fonte del 2010 | [SP10], [BBC09] — media |
| Sottomarini britannici contro pescherecci | 15 nel 1980–1990 | [WP-Antares] (MAIB) — alta |
| Agganci di pescherecci nel mondo | 36 dal 1952 al 1989 | [PT89] (Greenpeace) — bassa |
| Sottomarino–sottomarino, Est–Ovest | 10–25 nella Guerra Fredda (circa 1 all'anno nei periodi intensi) | [TI], [WP-Sceptre] — media |
| Sottomarino contro nave militare in esercitazione | a ondate: «terza collisione USA–URSS in 5 mesi» nel 1984 | [WP-K314] estratto — bassa |

**Lettura per il gioco [media]:** per un battello in servizio, una collisione di qualche tipo all'anno ogni ~20 anni di
battello; di queste la grande maggioranza in porto, in acque costiere o con attrezzi da pesca, una piccola parte
durante i pedinamenti. Per ora di pedinamento ravvicinato non esistono numeri pubblici (`03` §C4 lacuna 6): **la
probabilità deve emergere dalla geometria**, non da un dado (§4).

### 3.2 Perché

1. **Cecità a poppa.** Cono cieco di ~60° del 688 a poppa (Miasnikov per *Baton Rouge*); il seguito non sente il
   seguitore e viceversa il seguitore che «sta nei baffles» non vede le manovre se non dal rilevamento. **[media]**
   [WP-Kildin]
2. **Distanze di pedinamento brevi.** Dai «pochi metri» del *Tautog* agli 800 yd del Crazy Ivan sul *Sovereign*
   (che seguiva con la schiera a 15–20 nm nei tratti tranquilli), agli 11–13 km del *Grayling*. Con il solo sonar di
   scafo il pedinamento negli anni '60–'70 si faceva a centinaia di metri; con la schiera si allunga. **[media]**
   [WP-Tautog], [Farn25], [WP-Kola93]
3. **Quota ignota.** Il passivo dà il rilevamento, non la quota: il *Tautog* non sapeva se il K‑108 fosse sopra o
   sotto. La separazione di quota, che è la difesa naturale, non si può verificare. **[media]** [WP-Tautog]
4. **Acqua bassa e rumorosa.** Nel Barents i frangenti riducono la scoperta a prua fino a ~1 km o meno: due battelli
   che si vengono incontro hanno pochi secondi (`02` §4). **[media]** [WP-Kola93], [WP-Kildin]
5. **Inerzia.** Un 688 a 10–15 kn non si ferma e non accosta in qualche centinaio di metri; il *Grayling* tentò di
   accostare e risalire senza riuscirci. «Macchine indietro tutta» è rumorosissimo e rivela il seguitore. **[media]**
   [WP-Kola93], [GW2RU]
6. **Corsa dopo il contatto perso** (schema B): si accelera, il proprio rumore acceca il sonar proprio nel momento più
   pericoloso. **[media]**
7. **Fattore umano a quota periscopica:** analisi del moto abbreviata, ricerca al periscopio breve, quadro tattico più
   ottimista del vero, stanchezza dopo settimane in zone affollate (*Greeneville*, *Hartford*, *Montpelier*, MAIB 2018).
   **[alta]**
8. **Segretezza:** le aree di pattugliamento degli SSBN alleati non si coordinavano fra nazioni (*Vanguard*/*Triomphant*,
   proposta francese del 1984 lasciata cadere). **[media]** [WP-VgTr]

### 3.3 Contromisure

- **Gestione dello spazio acqueo** fra amici: aree esclusive con cuscinetto (`06` §7, STIMA 2σ + 2 nm). **[media]**
- **Separazione di quota** fra unità amiche in zone vicine e regola «sopra/sotto lo strato» nelle esercitazioni:
  pratica generale, valori non pubblici. **[bassa]**
- **Pedinamento con la schiera** a distanze di miglia invece che nei baffles: il Crazy Ivan diventa un salto del
  rilevamento sulla schiera, non un pericolo di collisione (`04` §3, `02` §5). **[media]**
- **Risposta al Crazy Ivan:** fermare le macchine e restare in silenzio (rischio di scoperta minimo, ma l'inerzia porta
  avanti), oppure virare e cambiare quota; «indietro tutta» solo per evitare l'urto. Nessuna dottrina pubblica
  formale. **[bassa]** [GW2RU], [Baff]
- **Procedure di quota periscopica:** sgombro dei baffles, tratti di analisi del moto (3 minuti ciascuno era la prassi
  violata sul *Greeneville*), ricerca al periscopio a 360° prima di tutto, «emergency deep» se un contatto è troppo
  vicino. **[alta]** per l'esistenza, **[media]** per i tempi. [WP-Ehime], [Mont13]
- **Pescherecci:** distanza minima (Regno Unito: 3.000 yd dal 1991), esercitazioni lontano dalle zone di pesca. **[alta]**
  [WP-Antares]
- **Dopo il 1993:** pedinamenti nel Barents ridotti, procedure e addestramento dei comandanti irrigiditi. **[media]**
  [WP-Kola93]

### 3.4 Conseguenze di comando e politiche

- **US Navy, tempo di pace dichiarato:** il comandante è quasi sempre **sollevato per «perdita di fiducia»**
  (*George Washington*, *Greeneville*, *Oklahoma City*, *Philadelphia*, *Newport News*, *Hartford*, *Montpelier*,
  *Jacksonville*), con punizioni per ufficiale di guardia, navigatore e capi; corte marziale solo con negligenza grave e
  morti (*Antares*, ufficiale britannico). Coerente con `13` §2.3. **[alta]**
- **Operazioni segrete della Guerra Fredda:** l'urto si nasconde (storia dell'«iceberg» sulla *Sceptre*, registro di
  bordo del *Gato* che si voleva retrodatare, smentite di entrambe le parti per il *Tautog*) e il comandante non è
  necessariamente punito; ma la missione è finita e il battello va in bacino. **[media]**
- **Parte sovietica:** l'inchiesta giudica di solito corretto il proprio equipaggio (*Kostroma*, che si dipinge una
  stella); con morti, condanne pesanti (S‑178: 10 anni ai due comandanti). **[media]**
- **Politica:** protesta formale e contatti al massimo livello (Baker–Eltsin 1992, Clinton–Eltsin 1993), scuse
  presidenziali e indennizzi (Giappone 1981 e 2001), restrizioni alle operazioni, proposte di estendere INCSEA ai
  sottomarini. In acque territoriali contese (Kildin, Mar Nero) l'incidente diventa una disputa giuridica. **[alta]**

---

## 4. Proposte per il gioco

### 4.0 Classico ed Enhanced

- **Livello classico: nessuna modifica.** Le unità si attraversano come nell'originale; restano la finestra d'impatto
  delle armi e l'incaglio già fatti. Gli esiti delle 110 partite del rilievo non devono cambiare. **PROPOSTA**
- **Enhanced:** collisioni fisiche fra tutte le unità in acqua (sottomarini, navi, siluri, esche; aerei esclusi), con
  un'opzione nel menu delle regole («collisioni fra unità», attiva per difetto nell'Enhanced). **PROPOSTA**

Le proposte sono in ordine di valore per costo.

### 4.1 P1 — Rilevazione geometrica con le dimensioni vere (prima, indispensabile)

- **Forme** (STIMA, da `classi.toml` + un campo nuovo):
  - sottomarino: **capsula** (cilindro con estremi emisferici) lunga `lunghezza_m`, diametro `larghezza_m`, asse a
    `quota − D/2` (la quota è la chiglia); più la **vela** come scatola (lunghezza ~15–20% di L, larghezza ~3–4 m,
    altezza sopra il dorso: campo nuovo `altezza_vela_m`, STIMA 688 ≈ 6 m, Typhoon e Oscar ≈ 8–10 m) e, se alzati,
    **periscopio e alberi** come segmento verticale;
  - nave: **prisma** lungo L e largo B con chiglia a `pescaggio_m` e prua affusolata (triangolo nel 15% prodiero);
    duomo sonar come sporgenza sotto la chiglia per le classi che lo hanno (STIMA 2–3 m);
  - siluro, esca: capsula (Mk 48: 5,8 × 0,53 m, da `siluri.toml`);
  - rete o cavo di un peschereccio: segmento dietro la poppa (STIMA 200–600 m di lunghezza, 30–100 m di profondità),
    solo per i pescherecci, quando arriveranno.
- **Test a due fasi**, ogni passo di simulazione:
  1. scarto grossolano con griglia spaziale (celle da 500 m) e differenza di quota;
  2. **CPA nel passo** (moto relativo lineare fra t e t + dt, la funzione `cpa()` della carta in 3D) contro la somma
     dei raggi: così un siluro da 0,5 m a 50 kn non «attraversa» uno scafo fra due passi;
  3. per le coppie entro la distanza, test esatto capsula–capsula, capsula–prisma, segmento–prisma, e **punto di
     contatto** (stazione x lungo ciascuno scafo, lato, altezza) e **normale**.
- **Siluri nell'Enhanced:** il contatto geometrico con lo scafo sostituisce la finestra di 185 m per la spoletta a
  contatto; la spoletta d'influenza resta un test di distanza a parte. Un siluro **non ancora armato** che urta uno
  scafo fa un colpo sordo (rumore, piccola ammaccatura, nessuna esplosione). Due armi possono urtarsi (raro, ma
  coerente). **PROPOSTA**
- **Costo:** con ~50 unità le coppie dopo la griglia sono poche; il test esatto è economico. **[media]**

### 4.2 P2 — Danni: energia, punto e mappatura sui modelli esistenti (prima, con P1)

- **Ingresso nuovo** nei due modelli (accanto all'urto da esplosione): `urto_meccanico(x, lato, z, normale,
  E_n, E_t)` per `BattelloDanni` e `DanniNave`. `E_n` dal §2.1 con le masse aggiunte, `E_t` dalla componente
  tangenziale (strisciamento). **PROPOSTA**
- **Ripartizione fra i due scafi** per rigidezza della parte (STIMA):

  | Parte | Rigidezza | Capacità prima della falla (STIMA) |
  |---|---|---|
  | duomo e sfera sonar, timoni di vela, eliche | bassa | 1–3 MJ (il sistema si rompe sopra 0,5 MJ) |
  | casing, casse zavorra, scafo leggero dei doppi scafi | bassa | 5–20 MJ (falla nella cassa: riserva di spinta minore) |
  | vela | media | 5–15 MJ (alberi e periscopi persi sopra 2 MJ) |
  | prua rinforzata di una nave, vela da ghiaccio (Sierra, Typhoon, Delta IV) | alta | 30–100 MJ |
  | scafo resistente | alta | 20–60 MJ a quota zero, × (1 − 0,5·h/h_collaudo) come la soglia del KSF |
  | zona prodiera di un 688 dietro il duomo (casse + sfera) | — | ~1 GJ in totale prima della paratia (taratura *San Francisco*) |

  L'energia oltre la capacità di una parte passa alla successiva verso l'interno: duomo → cassa → scafo resistente.
- **Mappatura:**
  - sistemi graduati di D6 (sonar di prua, schiera, periscopio, alberi, idroplani, timone, propulsione) dalla parte
    colpita; elica danneggiata = velocità massima ridotta e **rumore +3…+10 dB** con righe nuove (STIMA);
  - falla: area `A = R_T / spessore` con R_T da Minorsky sopra i 33 MJ, e sotto una falla piccola (STIMA 0,01–0,1 m²)
    solo se lo scafo resistente ha superato la sua capacità; portata con la quota come già in D6;
  - **ammaccatura dello scafo resistente**: riduce la quota di collasso (STIMA −10…−40% secondo l'energia, `OOC24`),
    sempre, anche senza falla. È il pericolo nascosto: il comandante che scende dopo l'urto può implodere;
  - **scossa** a bordo dalla variazione di velocità: `a = Δv / t_contatto` (t = 0,2–1 s, STIMA) → stessa scossa e stessi
    feriti di D6 (palette, vista che sobbalza, sistemi non sospesi); rollio fino a 60–90° per un urto verticale
    vicino alla superficie (STIMA, casi *Hartford* e K‑314);
  - **navi** (`DanniNave`): falla nella cella al punto di contatto se sotto il galleggiamento, duomo sonar, eliche e
    timoni se il contatto è a poppa; urti sopra il galleggiamento (urto voluto) solo sovrastrutture e lanciatori;
  - **pescherecci agganciati**: forza di traino sul cavo; capovolgimento in 10–60 s (STIMA).
- **Livello classico dei danni nell'Enhanced?** No: l'Enhanced usa solo i modelli fisici, il classico non ha urti.

### 4.3 P3 — Intelligenza artificiale: il rischio nasce dalla dottrina (seconda)

- **L'IA evita le collisioni solo con quello che sa**, non con la verità: usa il proprio quadro sensori (contatti,
  qualità, quota stimata o ignota). È la regola già scelta per l'incaglio (avvisi «solo da misure recenti»). Senza
  questo, le collisioni non nascono mai. **PROPOSTA**
- **Crazy Ivan sovietico** (parametri in `03` §C4): virata di 120–180°, spesso con cambio di quota (K‑19: 60 → 90 m) o
  arresto per ascoltare (K‑211). Se un seguitore è in coda a meno di ~1.000 m, la geometria produce da sola il passaggio
  ravvicinato o l'urto. **Nessun bonus al rischio**: solo geometria. **PROPOSTA**
- **Seguitori dell'IA** (sottomarini sovietici che pedinano il giocatore o una portaerei, `03` §C4 n. 4): distanza di
  pedinamento per classe e per sonar (STIMA 2–10 km con la schiera, 500–2.000 m senza), corsa verso l'ultimo punto
  quando perdono il contatto (schema B, K‑314) e risalite a quota periscopica senza il quadro completo.
- **Navi vicino a un sottomarino a quota periscopica:** le navi non vedono il sottomarino se non con radar o vedette
  (periscopio a < 2–3 km, STIMA) e mantengono rotta e velocità; le navi da guerra in esercitazione possono essere
  «distratte» (operazioni di volo, *San Jacinto*). Le regole di rotta valgono fra navi in superficie: chi vede l'altro
  manovra secondo COLREG (dritta-dritta), con tempi di reazione da 30 s a 3 min (STIMA). **PROPOSTA**
- **Evitamento dell'IA:** sotto un TCPA di 5 min e un CPA minore di 2 × (somma delle mezze lunghezze) accosta e cambia
  quota; sotto 60 s manovra d'emergenza (accosta tutta, quota opposta, ferma le macchine). L'IA a pedinamento
  preferisce «macchine ferme e silenzio» (rischio d'urto) all'evitamento rumoroso (rischio di scoperta), secondo un
  parametro d'aggressività del comandante. **PROPOSTA**
- **Urto voluto** (schema H), solo negli scenari di tempo di pace dell'Enhanced: avviso radio, avvicinamento da poppavia
  del traverso, accostata contro la murata a 2–5 kn relativi. **PROPOSTA**

### 4.4 P4 — Lato giocatore: avvisi e manovre (seconda, con P3)

- **Classico:** nessun avviso nuovo.
- **Enhanced, avvisi dell'equipaggio** dal quadro sensori, mai dalla verità (STIMA delle soglie):
  - sonar: «Contatto vicino, rilevamento in rapida variazione» quando il tasso di variazione del rilevamento supera
    ~5°/min a un contatto con livello alto; «Contatto ravvicinato» (close aboard) sotto ~500 yd
    stimate;
  - fire control: allarme CPA sotto 1.000 yd con TCPA < 5 min su un contatto con soluzione (si lega a `12` §5, riga 5,
    allarme CPA già proposto a 1 nm o 2 kyd per la carta);
  - **quota ignota**: se il contatto vicino non ha quota stimata, l'avviso lo dice («quota sconosciuta»), come per il
    *Tautog*;
  - prima di salire a quota periscopica: il secondo ricorda tratti di analisi incompleti e contatti non aggiornati
    (schema C); a quota periscopica la vedetta al periscopio annuncia una nave a meno di 2.000 yd.
- **Manovre d'emergenza** (tasti nel pannello Manovra di C2, `docs/11` §3.1):
  - **Emergency deep**: tutto a scendere con angolo forte fino a una quota sotto la chiglia della nave più profonda;
  - accostata d'emergenza con timone tutto e quota opposta al contatto;
  - **indietro tutta**: frena ma produce un transitorio forte (scoperta);
  - nessun pilota automatico anti-collisione: la decisione è del giocatore.
- **Crazy Ivan in coda:** se il giocatore pedina nei baffles e il seguito vira, l'unico indizio è il rilevamento che si
  sposta rapidamente e il livello che cresce; il giocatore deve scegliere fra silenzio e manovra. **PROPOSTA**

### 4.5 P5 — Conseguenze (seconda)

- **Rumore e scoperta:** transitorio d'urto (§2.6) udito da tutti entro la portata; **contatto reciproco garantito**
  fra i due battelli per qualche minuto; rumore proprio aumentato dai danni (eliche, piastrelle strappate: +2…+6 dB,
  STIMA; nell'originale un colpo dà già rumore +2 e sonar −2, §9.8). **PROPOSTA**
- **Esito della missione:** un urto che impedisce l'immersione o il ritorno è un fallimento di missione; in tempo di
  guerra un urto con un nemico rivela la posizione e può aprire uno scontro a distanza zero (siluri non armati sotto la
  distanza minima: a quella distanza nessuno dei due può sparare utilmente). **PROPOSTA**
- **Carriera** (`13` §2.3): pace, nave neutrale o amica con vittime → sollevato dal comando; collisione con un
  sottomarino in operazione segreta → nessuna promozione per la missione, «biasimo» riservato, nessun nastrino;
  urto evitato con un passaggio ravvicinato e pedinamento continuato → nessuna penalità (nella realtà si annota, non si
  punisce). Parte sovietica: inchiesta che di solito dà ragione all'equipaggio. **PROPOSTA**
- **Incidente politico** nelle missioni di tempo di pace dell'Enhanced: nota diplomatica, sospensione delle operazioni
  nella zona (fine anticipata della missione), messaggio radio dal comando (`10_radio_ruolo_in_partita.md`); in acque
  contese l'incidente vale doppio. **PROPOSTA**

### 4.6 P6 — Quasi collisioni e debriefing (terza)

- **Quasi collisione come evento:** un passaggio entro (STIMA) 200 m fra due unità immerse, o entro 500 m fra un
  sottomarino a quota periscopica e una nave, si registra con distanza, quote, chi sapeva di chi (contatto presente o
  no, con quale qualità). **PROPOSTA**
- **Debriefing** (schermate di `docs/12`): cronologia con l'istantanea della carta vera al momento del CPA minimo,
  confrontata con il quadro che aveva il giocatore (come per gli incagli in `docs/08` §9: «posizione stimata contro
  vera»). Testo nello stile dei rapporti d'inchiesta: «il contatto S‑13 non era aggiornato da 12 minuti». **PROPOSTA**
- **Statistiche e achievement** (`docs/08` §9): passaggi ravvicinati non scoperti (*Sovereign*, 800 yd), pedinamento
  senza urti, urto evitato con «emergency deep». **PROPOSTA**

### 4.7 Casi di prova per la taratura

| Prova | Situazione | Atteso |
|---|---|---|
| *Grayling* | 688 a 12 kn contro un Delta che arriva a 16 kn, urto radente a prua | graffi, duomo o vela del più piccolo, nessuna falla nello scafo resistente |
| *Tautog* | Victor che scende a 2 m/s sopra un 688 lento | vela del 688 piegata, alberi persi; elica del Victor persa, Victor rumoroso e lento |
| *Vanguard* | due SSBN a 3–5 kn, contatto obliquo | duomo dell'urtante, casing dell'urtato |
| *Sceptre* | SSN di prua a 6–8 kn sulla poppa di un SSBN quasi fermo | prua dell'urtante distrutta, elica e stabilizzatore dell'urtato; falla piccola nello scafo resistente dell'urtante (elica) |
| *Greeneville* | 688 che risale a 2,5 m/s e 10 kn sotto una nave di ~900 t | la nave affonda in minuti; danni lievi al 688 (timone, piastrelle) |
| *Hartford* | 688 a quota periscopica preso al traverso da una nave da 25.000 t a 10 kn | rollio forte, vela e piani; falla e cisterna nella nave |
| S‑178 | sottomarino convenzionale in superficie colpito a 25° a 8 kn | falla ~2 m², affonda in < 1 min se le porte non si chiudono in tempo |
| *San Francisco* (già in `06`) | 688 a 33 kn contro roccia | ~1 GJ assorbito dalla prua, scafo resistente integro |
| Classico | 110 partite del rilievo | nessuna differenza (nessun urto fra unità) |

### 4.8 Ordine di realizzazione (PROPOSTA)

1. Dati: `altezza_vela_m`, sporgenza del duomo, quota di riferimento (chiglia/asse) coerente con la quota periscopica.
2. P1 (rilevazione) con registro degli urti e delle quasi collisioni, senza danni: misura sul rilievo dell'Enhanced
   quante collisioni produce la geometria con l'IA attuale.
3. P2 (danni) con i casi del §4.7.
4. P3 e P4 insieme (l'IA che sa e il giocatore che sa).
5. P5 e P6 (conseguenze, debriefing).

---

## 5. Bibliografia

**Progetto:** `docs/originale/sensori_e_navigazione.md` §9.8; `engine/README.md` («Fondale e incagli», «Impatti
dell'originale»); `engine/missioni/src/missione.cpp`; `engine/nucleo/src/battello.cpp`; `data/motore/classi.toml`;
`data/motore/scafi/*.toml`; dossier `02`, `03`, `04`, `06`, `07`, `12`, `13`; `docs/08`, `docs/10`.

**Web (consultate il 24/10/2026):**
- **[WP-Kildin]** Wikipedia EN, *Submarine incident off Kildin Island* (fonti: E. Miasnikov, *The Submarine Review* 1993;
  Sontag & Drew; NYT 19/2/1992; Izvestia). https://en.wikipedia.org/wiki/Submarine_incident_off_Kildin_Island
- **[WP-Kola93]** Wikipedia EN, *Submarine incident off Kola Peninsula* (1993; da Miasnikov, [BMB], NYT 23/3/1993).
  https://en.wikipedia.org/wiki/Submarine_incident_off_Kola_Peninsula
- **[WP-VgTr]** Wikipedia EN, *HMS Vanguard and Le Triomphant submarine collision*.
  https://en.wikipedia.org/wiki/HMS_Vanguard_and_Le_Triomphant_submarine_collision
- **[WP-Ehime]** Wikipedia EN, *Ehime Maru and USS Greeneville collision* (da NTSB MAB‑05‑01 e Court of Inquiry).
  https://en.wikipedia.org/wiki/Ehime_Maru_and_USS_Greeneville_collision
- **[NTSB01]** NTSB, advisory del 2/3/2001 sulla cronologia del *Greeneville* (estratto, via
  https://corpora.tika.apache.org/base/docs/govdocs1/105/105272.html e *Honolulu Star‑Bulletin*).
- **[WP-BlackSea]** Wikipedia EN, *1988 Black Sea bumping incident* (da Aceves, *International Law Studies* 68; Kraska &
  Pedrozo 2013). https://en.wikipedia.org/wiki/1988_Black_Sea_bumping_incident
- **[INCSEA]** *Agreement on the Prevention of Incidents On and Over the High Seas* (1972), Avalon Project, Yale.
  https://avalon.law.yale.edu/20th_Century/sov008.asp
- **[WP-Gato]** Wikipedia EN, *USS Gato (SSN-615)* (da D. Miller, *Submarine Disasters*, 2006).
  https://en.wikipedia.org/wiki/USS_Gato_(SSN-615) — **[WP-ru-K19]** Wikipedia RU, *К-19*.
  https://ru.wikipedia.org/wiki/К-19 — **[MQ06]** M. W. Pocock, MaritimeQuest, *Daily Event for November 14* (2006).
  https://maritimequest.com/daily_event_archive/2006/nov/14_uss_gato_ssn615.htm
- **[WP-Tautog]** Wikipedia EN, *USS Tautog (SSN-639)*. https://en.wikipedia.org/wiki/USS_Tautog_(SSN-639)
- **[WP-Pintado]** Wikipedia EN, *USS Pintado (SSN-672)* (estratto). https://en.wikipedia.org/wiki/USS_Pintado_(SSN-672)
- **[WP-Sceptre]** Wikipedia EN, *HMS Sceptre (S104)* (da submarine.id.ru e I. Ballantyne, *Hunter Killers*, 2013).
  https://en.wikipedia.org/wiki/HMS_Sceptre_(S104)
- **[WP-Warspite]** Wikipedia EN, *HMS Warspite (S103)* (da Ballantyne 2013). https://en.wikipedia.org/wiki/HMS_Warspite_(S103)
- **[WP-Splendid]** Wikipedia EN, *HMS Splendid (S106)*. https://en.wikipedia.org/wiki/HMS_Splendid_(S106)
- **[WP-ru-K276]** Wikipedia RU, *К-276*. https://ru.wikipedia.org/wiki/К-276
- **[RusNavy]** rusnavy.com, scheda del *Kostroma* (quota ~21 m). https://rusnavy.com/nowadays/strength/submarines/kostroma/
  — **[SOFREP]** SOFREP / We Are The Mighty, *In 1992 a Russian sub surfaced underneath a US spy sub* (velocità
  d'impatto ~8 mph). https://sofrep.com/news/in-1992-a-russian-sub-surfaced-underneath-a-us-spy-sub/
- **[WP-K219]** Wikipedia EN, *Soviet submarine K-219*. https://en.wikipedia.org/wiki/Soviet_submarine_K-219
- **[WP-K314]** Wikipedia EN, *Soviet submarine K-314*; **[NI-K314]** *The National Interest* e *Business Insider* sul
  *Kitty Hawk* (estratti). https://en.wikipedia.org/wiki/Soviet_submarine_K-314
- **[WP-K56]** Wikipedia EN, *Soviet submarine K-56 (1965)*. https://en.wikipedia.org/wiki/Soviet_submarine_K-56_(1965)
- **[WP-S178]** Wikipedia EN, *Soviet submarine S-178*. https://en.wikipedia.org/wiki/Soviet_submarine_S-178
- **[WP-GW]** Wikipedia EN, *USS George Washington (SSBN-598)* (da J. F. O'Connell, *Naval War College Review* 2013).
  https://en.wikipedia.org/wiki/USS_George_Washington_(SSBN-598)
- **[WP-Nautilus]** Wikipedia EN, *USS Nautilus (SSN-571)*. https://en.wikipedia.org/wiki/USS_Nautilus_(SSN-571)
- **[WP-NN]** Wikipedia EN, *USS Newport News (SSN-750)*. https://en.wikipedia.org/wiki/USS_Newport_News_(SSN-750)
- **[WP-HNO]** Wikipedia EN, *USS Hartford and USS New Orleans collision*.
  https://en.wikipedia.org/wiki/USS_Hartford_and_USS_New_Orleans_collision — **[NBC09]** NBC News, *Navy:
  'Ineffective' leadership in sub collision* (2009). https://www.nbcnews.com/id/wbna34019879
- **[Mont13]** US Navy e stampa sul *Montpelier* (Maritime Executive, ABC News, 2012–2013; estratti).
  https://maritime-executive.com/article/USS-Montpelier-Commanding-Officer-Relieved-of-Duty-Following-October-Collision
- **[WP-Inc2000]** Wikipedia EN, *List of submarine incidents since 2000*.
  https://en.wikipedia.org/wiki/List_of_submarine_incidents_since_2000
- **[MAIB20]** MAIB, rapporto sulla quasi collisione *Stena Superfast VII* (2020; estratti da Forces News, Nautilus
  Federation). https://www.forcesnews.com/news/navy-nuclear-submarine-was-serious-risk-collision-ferry
- **[WP-Antares]** Wikipedia EN, *FV Antares* (da MAIB e Fatal Accident Inquiry 1991). https://en.wikipedia.org/wiki/FV_Antares
- **[PT89]** Knight‑Ridder / *Long Beach Press‑Telegram*, 12/7/1989, documenti FOIA della US Navy.
  https://prop1.org/2000/accident/1989/890712a1.htm
- **[BBC09]** BBC, 2009, dichiarazione scritta del ministro Ainsworth (estratto via forum.subcommittee.com).
- **[SP10]** StrategyPage, *Submarines: The Many Things That Go Bump In The Deep*, 9/11/2010.
  https://www.strategypage.com/htmw/htsub/20101109.aspx
- **[CSM92]** D. Sneider, *Cold War Lingers In Underwater Cat-and-Mouse Game*, *Christian Science Monitor*, 31/3/1992.
  https://www.csmonitor.com/1992/0331/31032.html
- **[GW2RU]** *How Soviet & American submarines RAMMED one another*, gw2ru.com (versione russa dei fatti).
  https://www.gw2ru.com/history/2937-us-soviet-american-submarines-rammed
- **[TI]** *Что будет, если две подводные лодки столкнутся: 4 эпизода Холодной войны*, Techinsider.ru.
  https://www.techinsider.ru/weapon/1664593-chto-budet-esli-dve-podvodnye-lodki-stolknutsya-4-epizoda-holodnoi-voiny-dayut-otvet/
- **[Hersh75]** S. Hersh, *The New York Times*, 1975, su Holystone (estratto via clementspapers.org).
- **[Farn25]** Necrologio del cap. R. Farnfield (HMS *Sovereign*, 1978), GeoGarage blog, feb. 2025.
  https://blog.geogarage.com/2025/02/captain-richard-farnfield-submariner.html
- **[WP-Borie]** Wikipedia EN, *USS Borie (DD-215)*. https://en.wikipedia.org/wiki/USS_Borie_(DD-215)
- **[WP-Harvester]** Wikipedia EN, *HMS Harvester (H19)*; **[WP-Ram]** Wikipedia EN, *Ramming* (estratto); The National
  Archives, ADM 1/12591. https://en.wikipedia.org/wiki/HMS_Harvester_(H19)
- **[Min59]** V. U. Minorsky, «An analysis of ship collisions with reference to protection of nuclear power plants»,
  *Journal of Ship Research* 3 (1959) — non consultato; formula e coefficienti tramite **[Min-sec]** (articoli che la
  riportano, es. https://eudl.eu/pdf/10.4108/eetsmre.9723) e **[TRB-AM]** *Equivalent added mass of ships in collisions*
  (TRID 5345). https://trid.trb.org/view/5345
- **[Newman77]** J. N. Newman, *Marine Hydrodynamics*, MIT Press, 1977 (masse aggiunte dell'ellissoide allungato, da
  H. Lamb, *Hydrodynamics*).
- **[OOC24]** studio numerico su uno scafo HY‑100 (congresso degli ingegneri navali spagnoli, 2024): −16% della pressione
  di collasso con difetto di circolarità dello 0,5% del raggio.
  https://63congreso.ingenierosnavales.com/wp-content/uploads/2024/05/5.Presentation_Design-and-Conception-of-a-Military-Submarine-Pressure-Hull-Using-Genetic-Algorithms.pdf
  — **[Stel21]** Università di Stellenbosch, *The effect of corrosion thinning on the collapse pressure of a submarine
  pressure hull*, 2021. https://scholar.sun.ac.za/items/e58772f0-d09b-43f5-bfb4-cacbb136990e/full
- **[BMB]** S. Sontag, C. Drew, *Blind Man's Bluff*, PublicAffairs 1998 (non consultato direttamente; citato dalle voci
  sopra). **[Baff]** come in `02` §11.

---

## 6. Lacune note

1. **Quota di riferimento.** La quota del motore è la chiglia, ma la quota periscopica dell'originale (20 ft) è quella di
   un asse o di una vela: per collisioni con le navi serve una quota periscopica vera (~18–20 m di chiglia per il 688) o
   una conversione esplicita. Da decidere con chi lavora sulla dinamica (C2, C6).
2. **Altezza delle vele e degli alberi** delle 11 classi di sottomarini: nessuna fonte consultata; le STIME del §4.1
   vanno controllate sui disegni (`docs/06_kit_navi.md`, modelli).
3. **Velocità e quote** dei casi della Guerra Fredda: note solo per *Grayling* (in parte), *Baton Rouge* (~21 m),
   *Gato* (60–90 m). L'appendice di [BMB] e l'articolo di Miasnikov (1993) non sono stati letti.
4. **Elenco russo delle collisioni** (submarine.id.ru, ~25 voci): solo citato; da cercare in archivio.
5. **Dottrina di risposta al Crazy Ivan** e distanze di separazione di quota della US Navy: non pubbliche.
6. **Livello sonoro di un urto** fra scafi: nessuna misura pubblica trovata; la STIMA del §2.6 va tarata a orecchio.
7. **Capacità energetiche per parte** (§4.2): stime dell'autore ancorate solo al *San Francisco* e a casi qualitativi.
