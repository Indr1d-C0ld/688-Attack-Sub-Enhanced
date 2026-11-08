# Ascolto e classificazione sonar, anni '70 - primi '90 (per il dossier 688 Attack Sub, ambientazione 1989)

Ricerca su fonti aperte. Data della ricerca: 25 ottobre 2026. Lingua del dossier: italiano. Citazioni brevi in lingua originale quando servono.

## 0. Come leggere questo documento

Ogni affermazione porta il link alla fonte e due etichette.

**Stato della verifica**

- **[V] verificato**: ho letto direttamente il testo della fonte (pagina aperta nel browser, PDF/OCR scaricato, testo estratto). Per i documenti DTIC ho usato le copie OCR di archive.org (identificatori `DTIC_ADA...`), perché `apps.dtic.mil` era offline per manutenzione.
- **[R] riportato**: l'ho letto soltanto attraverso un riassunto automatico della pagina o lo snippet di un motore di ricerca. Il contenuto è plausibile ma non l'ho riletto sul testo originale.
- **[D] dedotto**: è una mia inferenza, non c'è una fonte che lo dica. La segno sempre in modo esplicito.

**Confidenza**: alta / media / bassa. Considera sia l'affidabilità della fonte sia quanto la fonte si applica davvero al 1989 e a una nave classe 688 (molte fonti sono del 1998-2020 e descrivono sistemi più tardi).

Cose che NON ho potuto verificare, in sintesi (dettagli nella sezione 7): il testo di «Blind Man's Bluff», «Hunter Killers», «Submarine» di Clancy (biblioteca prestito, non accessibile); qualunque sistema BQR-25 o MG-10; funzioni di annotazione, marcatori, salvataggio e richiamo sulle console BQQ-5 del 1989; memorie in prima persona di sonaristi sovietici.

---

## 1. Sintesi operativa per il simulatore (le dieci cose che contano)

1. **La classificazione era in due tempi.** Impressione iniziale all'ascolto, poi analisi di circa 30-45 secondi su motore, pompe idrauliche, configurazione delle pale. [V, TWZ] Nel protocollo sperimentale del 1974 l'operatore classificava quattro volte: dopo 10 secondi di ascolto, dopo analisi libera (fino a 5 minuti), dopo analisi sistematica di 8 «clue» standard più conteggio delle pale, e infine dopo aver visto i valori corretti. [V, Mackie 1974] Confidenza: alta per la struttura a due tempi, media per i tempi.
2. **L'accuratezza era bassa e migliorava con il metodo.** Nel test di Mackie la percentuale di classificazioni corrette è salita 39%, 44%, 49%, 57% nei quattro passaggi (militari esperti, tracce registrate). [V] L'albero logico delle clue ha aiutato in modo misurabile. [V] Confidenza: alta (come dato storico, non come tasso «di bordo»).
3. **Audio + video battevano il solo audio.** Mackie (1974) lo misura sui sonaristi dei sommergibili. Già allora «in some of today's systems, visual pattern recognition is the sole means of analysis». [V] Confidenza: alta.
4. **Nel 1983-1988 la presentazione era «primarily visual».** L'NSMRL scrive che i nuovi sonar (fino al BQQ-6) producono «a profusion of information which is displayed, primarily visually». [V, Kinney 1983] Nel 1988 si annuncia che i display CRT a colori «will soon appear on board U.S. submarines». [V, Luria 1988] Confidenza: alta.
5. **L'orecchio restava il canale di allerta e di disambiguazione**, soprattutto per banda larga, transienti e biologici, e nella «fusione mentale» lofargramma + cuffie, che è descritta come mestiere da insegnare. [V, Barron 2001] Al Conn (ufficiali, 1983) i «raw auditory signals» erano al 13° posto su 15 per importanza e i «raw visual displays» al 7°. [V, Laxar 1983] Confidenza: media per il peso relativo nel 1989 (è una deduzione dalle fonti, [D]).
6. **Nel 1989 la libreria di firme era soprattutto umana e di terra.** Esisteva una libreria di firme sovietiche, creata con LOFAR e array rimorchiati. [V, Cote] I tonali erano noti in anticipo perché i calcolatori elaboravano poche righe spettrali. [V] Il confronto in tempo reale con una libreria su bordo era un lavoro dello specialista ACINT, del supervisore sonar e dei «ship rider» ONI. [V parziale] La classificazione automatica delle minacce fu esplicitamente tolta dal BSY-1. [V, GAO 1992] Confidenza: media-alta.
7. **Registrazione**: carta (BQQ-3 con stilo termico, poi LOFARgram elettrostatici), nastro magnetico (anche «multiple tape recorders running as backup» su un SSBN nel 1986-91). [V] Registrazione digitale e riproduzione dentro la catena del sonar (TARPU) compaiono con A-RCI, fine anni '90. [V, Barron 2001] Confidenza: media-alta.
8. **Numero di operatori**: un'indagine NSMRL del 1981 su 12 sommergibili dice turni di 6 ore di guardia e 12 di riposo, tre sonaristi che ruotano di norma ogni ora, più il supervisore. [V] Il BQQ-5 fu ridisegnato da una a tre console. [V, GlobalSecurity] Confidenza: alta per il 1981, media per il 1989.
9. **Sala sonar**: luce rossa, blu o bianca tenue (da meno di 0,01 a 0,28 foot-candle sulle console), CRT a distanza da 18 a 31 pollici (media 24), cuffie scomode «infrequently» citate, ventilazione scarsa, sedili scomodi, fumo. [V, Kinney 1981] Confidenza: alta.
10. **Sul lato sovietico** si sa poco di verificato: la famiglia Skat (sviluppo dal 1968, ZNII Morfizpribor) ebbe quattro calcolatori Karat, uno dedicato alla classificazione; due monitor programmabili NPO Mars; un «magnetofono di servizio» ad anello di 5 secondi per riascoltare i segnali sonar nemici intercettati. [V, Kazakov 2019; ru.wikipedia] Nessuna fonte aperta che io abbia trovato descrive la classificazione acustica a orecchio degli operatori sovietici. Confidenza: media sui fatti tecnici, bassa sull'uso operativo.

---

## 2. Classificazione a orecchio

### 2.1 Cosa sentiva il sonarista (vocabolario e fisica)

| Elemento | Cosa dice la fonte | Fonte | Stato / Confidenza |
|---|---|---|---|
| Cavitazione (fisica) | Compare prima alle punte delle pale («blade tip cavitation»), poi sul dorso («sheet cavitation»). Nell'esempio idealizzato: a 90 giri/min nessuna cavitazione (solo rumore di flusso e di fondo), a 110 giri/min la cavitazione parte e il livello sale di molti dB con picco intorno a 400 Hz; a velocità maggiore il picco scende in frequenza e la banda si allarga. A profondità maggiore il rumore cala e si sposta verso le alte frequenze, come se la velocità fosse diminuita. Il manuale avverte che le curve sono «idealized» e non rappresentano un sommergibile specifico. | [NAVEDTRA 10130-C, 1976, cap. 3](https://archive.org/details/DTIC_ADA091009) | [V] alta |
| Rumore di flusso | Funzione della velocità con soglia netta, essenzialmente a bassa frequenza, alto dentro la zona turbolenta e basso nel campo irradiato: «almost exclusively a self-noise problem». | stessa | [V] alta |
| Macchinari | Sorgenti elencate: pompe antincendio e di lavaggio, compressori, impianto frigorifero, condizionamento, generatori diesel, ventole. «Circuit noise» interno: ronzio a 60 Hz, fruscii da cavi. | stessa | [V] alta |
| Biologici | Delfini: fischio e «a sound like a chuckle»; gamberetti «snapping shrimp»: ronzio, poi «fat sizzling on a fire», poi «burning brush» avvicinandosi, comuni fra 45°N e 45°S in acque sotto 30 fathoms; balene: colpi, gemiti, «pings», «a swinging rusty gate»; capodoglio: «hammering», di rado sotto 100 fathoms; globicefali: fischio come il delfino ma più chiaro. Il manuale invita a rivedere nastri e film periodicamente «to maintain proficiency». Timoni e alette dei sommergibili «sometimes also give off a whistling sound». | stessa | [V] alta |
| Pioggia e mare | «A heavy rain, for instance, will add greatly to the ambient noise»; rumore ambientale legato a vento e stato del mare. | stessa | [V] alta |
| Siluro | Primo indizio: forse il suono dell'aria che esce, poi un rumore «propellerlike», con battiti troppo rapidi da contare, intensità che sale in fretta, simile a un jet ma meno acuto. | stessa | [V] alta |
| Aggettivi dell'aula di classificazione | Navi mercantili più «laboring», meno «galloping», tono più basso e meno «charging» delle navi da guerra. Quattro termini dava risultati coerenti: «hiss», «high-pitched», «pulsing», «squeal». Gli istruttori «tend to overrate» la capacità degli allievi di collegare i termini alle caratteristiche reali. | [Mackie 1974](https://archive.org/details/DTIC_ADA002307) | [V] alta |
| Elenco RN di suoni da riconoscere | «Whine, Futter, in/out, Shaft Rub, Diesel Roar, Trawl Noise, Cavitation»; differenza fra cavitazione e «compressed cavitation»; suoni dei diesel; rumori biologici di pesci e mammiferi sonici. | [Royal Navy, Basic Aural Analysis](https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/basic-aural-analysis) | [V] alta sul vocabolario, ma è un corso attuale (non dimostra che sia lo stesso del 1989) |
| Conteggio di giri e pale | Calcolo di giri d'asse e pale; variazioni di velocità da variazione dei giri, numero di assi, configurazione delle pale. | stessa | [V] alta (contenuto del corso attuale) |
| Transienti | «A sharp, metallic transient object is out of place in the natural undersea world.» Il Royal Navy LOFAR course distingue i tonali di motore o asse da «signature from auxiliary source, swaths or transients». | [TWZ, Amick 2020](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see); [RN, Basic Low Frequency Array](https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/basic-low-frequency-array) | [V] alta sulla citazione; non ho trovato un catalogo di transienti con descrizioni (porta che sbatte, avvio pompa) |
| Tonali, pale, DEMON | Le frequenze di pale e macchine formano la firma di una classe o di una nave. Pale: tonali un po' più bassi dei tonali di macchinario (questo dettaglio l'ho letto solo in un riassunto automatico di Cote, [R]). Dopo il Victor III l'ammiraglio Watkins (1984) parla di spingere il lavoro a bassa frequenza «to go after the propeller blade rates». | [Wikipedia, LOFAR](https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder); [Cote, The Third Battle](https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm) | [V] alta |
| Ghiaccio | Il TWZ dice solo che «Ice caps and marginal ice zones are noisy areas that can mask and reflect sonar signals». Per il carattere dei suoni: rumore del pack da fratture termiche (transienti acustici) e da pressione/creste («ridging»); sopra circa 40 Hz domina lo scricchiolio, sotto 40 Hz le creste attive. | TWZ (sopra); [Makris e Dyer, JASA 1986](https://acoustics.mit.edu/faculty/makris/Environmental%20correlates%20of%20pack%20ice%20noise.pdf); [Greening e Zakarauskas, 1992](https://jcaa.caa-aca.ca/index.php/jcaa/article/view/740) | TWZ [V]; ricerca sul ghiaccio [R]. Confidenza media. Sono articoli di oceanografia, non descrizioni da sonarista. |

Nota di confidenza: i manuali e gli studi sopra sono del 1974-1983 (americani) e quindi sono più vicini al 1989 delle descrizioni attuali RN/USN. I corsi RN e gli articoli TWZ/Navy.mil sono del 2015-2020 e descrivono pratica recente.

### 2.2 Come ci si addestrava

**Scuole e durata (USA)**

- La scuola «A» per Sonar Technician era a Fleet Anti-Submarine Warfare Training Center, Pacific, **San Diego**: 5 settimane per STG (superficie), **9 settimane per STS (sommergibile)**; requisiti: udito normale, percezione cromatica normale, punteggi ASVAB. [V, tesi SDSU 1992, su archive.org](https://archive.org/details/asvabscoreaspred00rayk) Confidenza: alta, ma per il 1990-92.
- Il manuale di avanzamento NAVEDTRA 10130-C (1976) dice che per la sua redazione hanno dato assistenza tecnica FLEASWTRACENPAC (San Diego), **Naval Submarine School, Groton**, **Naval Underwater Sound Laboratory, New London** e Naval Intelligence Support Center. [V, NAVEDTRA 10130-C](https://archive.org/details/DTIC_ADA091009) Confidenza: alta sul fatto, ma non dice che il corso aurale fosse tenuto a Groton.
- Gli studi sul corso di analisi aurale parlano di «Fleet ASW School» di San Diego: allievi sommergibilisti e «senior Petty Officers at the Fleet ASW School». [V, Mackie 1974](https://archive.org/details/DTIC_ADA002307); [Lewis e Rimland 1980](https://archive.org/details/DTIC_ADA085030). Confidenza alta.
- **Groton/New London**: le fonti trovate mettono lì ricerca (NSMRL a Groton) e la Naval Submarine School, ma non ho una fonte che dica che il corso di classificazione aurale per sonaristi STS si svolgesse a Groton. Le due ipotesi del brief vanno quindi trattate come non confermate. [D] Confidenza bassa.
- Test di selezione dell'udito: ricerca NSMRL dal 1939 (Shilling ed Everley) che porta a test per scegliere gli uomini per il servizio di ascolto sui sommergibili. [R, Daniel 2005](https://archive.navalsubleague.org/2005/nsmrl-a-small-command-with-a-huge-presence-for-the-submarine-force) Confidenza media-alta.
- Un sonarista STS entrato nel 1983 racconta che il reclutatore disse: «if you can pass the hearing test, you can be a submarine sonar technician». [V, Chetlain 2024](https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/) Confidenza media (aneddoto).
- Moderno (non 1989): «yearlong shore-based training before putting on a set of headphones at sea». [V, TWZ](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see) Confidenza media per l'applicazione al 1989.

**Metodo didattico (Mackie, ONR, 1967-1974)** [V, tutte da Mackie 1974](https://archive.org/details/DTIC_ADA002307)

- Il «sonar aural analysis course» usava registrazioni su nastro di contatti reali e un vocabolario descrittivo standard.
- Dispositivo sperimentale di «memory aid»: deck a nastro magnetico a **20 canali, accesso casuale**, con esempi registrati di più classi; permetteva di confrontare un contatto ignoto con esempi noti; i tecnici esperti andavano meglio con l'aiuto, soprattutto i meno bravi. L'autore scrive di non conoscere alcun sistema sonar in sviluppo che l'avesse incorporato. Confidenza alta (esperimento, non dotazione di bordo).
- **SCATS (Small Computer Auditory Training System)**: calcolatore da 12K di memoria a nuclei e unità a nastro; fino a **8 terminali** indipendenti con tastiera, display limitato e cuffie; segnali su cartucce a nastro «endless loop», **fino a 32 canali** hi-fi; risposta piatta 20-25.000 Hz, cuffie 20-15.000 Hz (laboratorio). Il 75% del tempo era ascolto di contatti con giudizi percettivi; 15 ore per gruppo; clue riconosciute dai novizi al 76% circa, comparabile con quella degli esperti (68-92%).
- **Albero logico (logic tree)** per combinare le clue migliorava la classificazione: +19% sul caso contro circa +10% senza. Era già stato dimostrato da Abrams e Winchell (1972).
- **Stimulus error**: i sonaristi esperti «sentono» le clue suggerite dall'istruttore; i suggerimenti sbagliati peggioravano i risultati. Le percezioni dipendono dagli stereotipi della classe di bersaglio.
- Grandi differenze individuali; raccomandato un test di classificazione annuale per tutti i sonaristi, i supervisori e gli istruttori, e collegare l'avanzamento ai risultati. Non risulta implementato (nel 1974).
- Il feedback a bordo è debole perché molti bersagli non sono mai identificati con certezza, quindi gli errori si consolidano.
- Un rapporto Honeywell/NAVTRAEQUIPCEN del 1975-76 propone un OJT al pontile con **registratore a nastro e libreria di nastri di segnali sonar** con descrizione delle circostanze di registrazione. [V, ADA053179](https://archive.org/details/DTIC_ADA053179) Confidenza media (proposta, non dotazione).

**Corsi RN (attuali, per confronto)**: Basic Aural Analysis (5 giorni, HMS Raleigh, con moduli di formazione al computer) e Advanced Aural Analysis (5 giorni, classificazione di sonar attivi), Basic LOFAR (DEMON, giri d'asse, ritmi di cilindri e albero, torpedini). [V, RN](https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/advanced-aural-analysis) Confidenza alta sul contenuto, bassa sull'equivalenza con il 1989.

**Fattori interni 1980-83**: l'NSMRL (Groton) cercò test visivi per prevedere chi sarebbe stato bravo sui display; sei misure combinate (acuità da vicino, forìa, tempo di servizio, discriminazione di texture, test di locus of control, GCT) davano correlazioni multiple di circa 0,44-0,53. Valutazione di competenza data dal capo sonar e dai pari; 103 tecnici su BQQ-5 e BQQ-6 (6 sommergibili d'attacco e 2 Trident). [V, Kinney 1983](https://archive.org/details/DTIC_ADA125059) Confidenza alta. Significato: l'Marina trattava l'abilità sul display visivo come la competenza critica.

### 2.3 Quanto contava l'orecchio rispetto alla vista nel 1989

Risposta sintetica (con dedotto indicato): nel 1989 **la vista guidava la rivelazione e la ricerca a banda stretta; l'orecchio guidava la rivelazione a banda larga per marcamento, la disambiguazione e la classificazione iniziale, e restava insostituibile per transienti, biologici, sonar attivi e siluri.** Il peso reale per una nave 688 non è quantificato in nessuna fonte aperta che ho letto. [D]

| Evidenza | Fonte | Stato / Confidenza |
|---|---|---|
| «The ear was the primary analyser» nei primi sonar; poi l'occhio ha assunto un ruolo «highly important»; «in some of today's systems, visual pattern recognition is the sole means of analysis; in others, aural and visual analyses play complementary roles». Prestazioni migliori con audio+video che con il solo audio; le procedure di classificazione sono state cambiate perché usassero di più il video «which, at the time of this study, was seldom used». | [Mackie 1974](https://archive.org/details/DTIC_ADA002307) | [V] alta |
| Nuovi sonar fino al BQQ-6: una «profusion of information which is displayed, primarily visually». | [Kinney 1983, NSMRL 994](https://archive.org/details/DTIC_ADA125059) | [V] alta |
| Gli ufficiali al Conn classificano «Raw Auditory Signals» al 13° posto su 15 (fra le informazioni «Unnecessary» da mostrare), «Raw Visual Displays» al 7°. (Stima: 95 ufficiali di sommergibili, 1983.) | [Laxar et al., NSMRL 1010](https://archive.org/details/DTIC_ADA135857) | [V] alta (ma riguarda gli ufficiali al Conn, non i sonaristi) |
| «The mental fusion of acoustic information gleaned from lofargrams and headphones is a trade skill that has to be taught.» Lofargrammi: sondaggio 1998 su circa 200 operatori, «universal problem» nel riconoscere contatti. | [Barron 2001](https://archive.navalsubleague.org/2001/the-operator-is-part-of-the-system) | [V] alta sulla citazione, ma data 1998-2000 |
| «We detect the sound with our ears, we track it through our system, and then we classify it.» | [Navy.mil 2015](https://www.navy.mil/DesktopModules/ArticleCS/Print.aspx?PortalId=1&ModuleId=523&Article=2265525) | [V] alta, ma 2015 |
| Operatori in cuffia che ascoltano «white noise» dell'oceano mentre guardano la schermata; l'operatore di banda larga cerca a 360° e se sente qualcosa «puts a tracker on it». | stesso; [TWZ](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see) | [V] alta, ma 2015-2020 |
| Specialisti ACINT in ogni operazione ASW importante perché «The work is simply too important to trust to computer software». | [Krokel, Proceedings 2016](https://www.usni.org/magazines/proceedings/2016/february/nobody-asked-me-navy-should-expand-acoustic-intelligence) | [V] alta, ma 2016 |
| Anni '50-'60: «Sonar operators with good ears were valued people.» | [Andrews 2006](https://archive.navalsubleague.org/2006/early-days-of-submarine-development-squadron-12) | [V] alta (è per il BQR-4, non per il 1989) |
| Anni '80 (RN): da una «faint line on a sonar display» a un «firm aural contact emitting a range of machinery and other noises» in avvicinamento; HMS Valiant 1985. | [Conley 2009](https://archive.navalsubleague.org/2009/the-black-pig-and-the-red-banner-fleet) | [V] alta, memoria di comandante, non di sonarista |

**Contesto tattico 1980-90** (utile per decidere che cosa «si sente» in una missione): il Victor III fu una sorpresa acustica: «We had misjudged the absolute sound and pressure levels»; Watkins 1984 dice che l'array rimorchiato a bassa frequenza «was effective every time» e che occorreva andare a cercare i «propeller blade rates». Secondo Cote, già a metà anni '80 l'Akula riduceva i tonali a banda stretta sotto il livello del rumore a banda larga, con livello broadband vicino a quello degli Sturgeon e dei primi 688. [V, Cote](https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm) Confidenza alta (Cote scrive nel 2000 con fonti pubbliche; la stima dei livelli è sua).

Quindi nel 1989 un contatto sovietico «moderno» (Victor III, Sierra, Akula) è principalmente un problema di tonali di pale a bassa frequenza e di array rimorchiato, non di suono a banda larga in cuffia. [D]

### 2.4 Memorie e testimonianze (quali ho verificato)

| Fonte | Contenuto utile | Stato / Confidenza |
|---|---|---|
| [Aaron Amick, TWZ, 2020](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see) | Sonarista in congedo su 688 e Ohio; classificazione in due tempi; broadband e narrowband; Sonar Supervisor; ONI; divisione sonar di 14 uomini. Descrive sistemi recenti (algoritmi di allerta). | [V] alta sul testo; media come testimonianza del 1989 |
| [David Chetlain, The War Horse, 2024](https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/) e [ristampa SOFmag, 2025](https://sofmag.com/soviet-submarines-hunted-us/) | STS su Georgia (SSBN 729) e Nevada (SSBN 733), servizio dal 1986 al 1991; «multiple tape recorders running as backup»; «I could hear a pin drop at a hundred yards»; Trident come «hole in the ocean»; Victor III e Sierra «significantly harder to track». | [V] alta sul testo; media come testimonianza (SSBN, non 688) |
| [Dan Conley, Naval Submarine League, 2009](https://archive.navalsubleague.org/2009/the-black-pig-and-the-red-banner-fleet) | Comandante di HMS Valiant 1984-86; Echo II con «very loud whines and thumps of machinery»; sala sonar accanto alla cabina; array rimorchiato per l'inseguimento. | [V] alta; RN, non USN |
| [Capt. Frank Andrews, 2006](https://archive.navalsubleague.org/2006/early-days-of-submarine-development-squadron-12) | Anni '50-'60, BQR-4 (analogico), «Sonar operators with good ears were valued people»; analisi spettrale con plotter di carta. | [V] alta; epoca precedente |
| [Intrepid Museum, oral history Richard Harrison](https://intrepidmuseum.org/oral-history-richard-harrison) | Sonarman su USS Growler (diesel), «BQR-2 workhorse». | [R] bassa; epoca precedente |
| «Blind Man's Bluff» (Sontag e Drew, 1998), «Hunter Killers/Undersea Warriors» (Ballantyne), «Submarine» (Clancy, 1993), Weir e Boyne «Rising Tide» | Non sono riuscito ad aprire il testo (prestito su archive.org). Non cito pagine né citazioni. Gli esiti di ricerca confermano soltanto temi generali (Ivy Bells, Scorpion, ecc.) e che una recensione di lettore parla di conteggi di giri d'elica per identificare le unità sovietiche. | [R] bassa; **da verificare con copia cartacea** |
| Sonaristi sovietici | Non ho trovato memorie in prima persona che descrivano la classificazione a orecchio. Pointer non verificati: Cherkashin, «Povsednevnaja zhizn' podvodnikov» (ModernLib), che contiene episodi di acustici che registrano su nastro i rumori di un gruppo navale USA, ma sono episodi di un sottomarino diesel degli anni '50-'60 e non riguardano gli anni '80. Un volume di memorie tecniche: «History of Russian Underwater Acoustics» (JASA review, capitoli personali sui sistemi subacquei). | [R] bassa; **da verificare** |

---

## 3. Registratori, confronti con una libreria e salvataggio

### 3.1 Registratori a bordo (cosa esisteva)

| Cosa | Dettaglio | Fonte | Stato / Confidenza |
|---|---|---|---|
| BQQ-3 su carta termica | I BQQ-3 «use a heat stylus to literally burn narrowband tonals onto a moving sheet of paper»: «the burning smell so familiar to a generation of American submariners». Cote scrive che gli analizzatori sperimentali tipo BQQ-3 furono sostituiti nei primi anni '70 da sistemi digitali su tutti i sommergibili. | [Cote, nota 51](https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm) | [V] alta sul BQQ-3 e la carta |
| Ma nel 1981 il BQQ-3 era ancora a bordo | Nell'indagine NSMRL su 12 sommergibili, 7 avevano un BQQ-3 con «illuminated chart» (luminanza da 1,5 fL di luce rossa a 50,5 fL di luce bianca); lo descrivono come sistema di classificazione che richiede il dettaglio visivo fine. | [Kinney 1981](https://archive.org/details/DTIC_ADA100767) | [V] alta. Le due fonti si contraddicono su «early 1970s»: il BQQ-3 era evidentemente ancora usato nel 1981 (o la nota di Cote riguarda la sostituzione parziale). |
| LOFARgram a carta elettrostatica | L'analisi passò al digitale negli anni '60-'70, ma i display rimasero elettrostatici fino agli anni '90. | [Wikipedia, LOFAR](https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder) | [R] media |
| BQR-22A | Display color waterfall (LOFARgram), A-scan a spettro singolo e DIMUS a banda larga; elabora dati multibeam da array di scafo e rimorchiati. | [FAS, BQR-20 series](https://man.fas.org/dod-101/sys/ship/weaps/an-bqr-20.htm) | [R] media-bassa per il 1989 (pagina aggiornata nel 1998; non dice quando apparve il «color») |
| BQR-23 / BQR-23A | Processore per array rimorchiato BQR-15 degli SSBN: primo contratto 1974, Spectral Dynamics; almeno 64 canali LOFAR simultanei in tempo reale; display LOFAR e frequenza-azimut; BQR-23A con tracker a interpolazione di rilevamento. | [Nuclear Companion](https://nuclearcompanion.com/data/an-bqr-23-towed-array-processor/); [FAS](https://man.fas.org/dod-101/sys/ship/weaps/an-bqr-20.htm) | [R] media (sito secondario, cita Friedman 1989) |
| Nastro magnetico per la raccolta | «Multiple tape recorders running as backup» durante un pattugliamento di un SSBN, 1986-91. | [Chetlain](https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/) | [V] media (aneddoto) |
| Nastri ONI e TARPU | ONI compilò 21 nastri di «element-level» di contatti reali; **TARPU** (Towed Array Record/Playback Unit) rimandava i dati registrati nella testa del sistema A-RCI; «One of the most important aspects of these training tapes is that they remain on the ship». | [Barron 2001](https://archive.navalsubleague.org/2001/the-operator-is-part-of-the-system) | [V] alta, ma 1998-2000 |
| Registratore sovietico (intercettazione) | Nel sottosistema OGS («обнаружение гидроакустических сигналов», cioè intercettazione dei sonar attivi) del complesso Skat: magnetofono con nastro ad anello da 5 secondi che registrava automaticamente «l'ultimo segnale rilevato»; l'operatore poteva riascoltarlo più volte per classificare la sorgente; nome «дежурный магнитофон». | [Kazakov, Nauka, tekhnika i obrazovanie, 2019](https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody) | [V] alta sul testo |

### 3.2 Libreria di firme e confronto di un contatto

- **Esisteva una libreria di firme di sommergibili sovietici**: «One of the first and most important uses for both LOFAR and the towed array in the submarine force was the creation and maintenance of a library of Soviet submarine signatures.» «Each class of submarine, and indeed each submarine, generated its own unique blend of narrow and broadband sound.» I calcolatori erano limitati nel numero di righe spettrali da elaborare, perciò occorreva conoscere in anticipo le frequenze da guardare. I sommergibili erano gli unici a poter raccogliere e mantenere quella base di dati. [V, Cote](https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm) Confidenza alta.
- I tonali agiscono da «acoustic fingerprint»: possono identificare una classe e a volte una singola unità. [V, Howard 2011](https://archive.navalsubleague.org/2011/fixed-sonar-systems-the-history-and-future-of-the-underwater-silent-sentinel); [Wikipedia, LOFAR](https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder). Confidenza alta.
- **Chi faceva il confronto**: specialisti ACINT («principal advisors to Commanding Officers in ACINT collection tactics»), forniti dal programma SSEP; due o tre sonaristi ONI imbarcati per «tracking and recording the mission targets»; supervisore sonar come collegamento verso l'ufficiale all'approccio; insegnamento ONI delle firme nei corsi A-RCI. [V, FAS SSEP 1998](https://irp.fas.org/program/collect/ssep.htm); [V, TWZ](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see); [V, Barron](https://archive.navalsubleague.org/2001/the-operator-is-part-of-the-system). Confidenza alta per l'esistenza dei ruoli, media per il 1989.
- Il programma SSEP: squadre di 5 ST, 2 ET e 1 PH in missioni di 21-30 giorni, circa 27-30 sommergibili all'anno; i dati raccolti sono «reduced in 10 working days at an off-site data reduction facility» e producono un rapporto di sensibilità del sonar della piattaforma. [V, FAS SSEP](https://irp.fas.org/program/collect/ssep.htm) Confidenza alta (pagina del 1998). Attenzione: SSEP descrive la raccolta di sensibilità e calibrazione del sonar, non il confronto di singoli contatti.
- Il lavoro ACINT è definito «more art than science»; gli esperti ACINT derivano la competenza da anni di esperienza su navi e sottomarini stranieri. [V, Barron 2001]. Oggi è un NEC con qualifica da 18-32 mesi e attrito storico del 50%. [R, cool.osd.mil]. Confidenza alta per la prima, media per la seconda.
- **Classificazione automatica a bordo, 1989**: non verificata. Anzi: nel passaggio SUBACS > BSY-1 furono **cancellati** «nontraditional processing, automatic threat detection and classification, and automatic performance prediction»; le funzioni sono nel BSY-1 ma «require more staffing than originally planned». [V, GAO/NSIAD-92-50](https://archive.org/details/DTIC_ADA248051) Confidenza alta. Quindi è corretto, per il 1989, rappresentare la classificazione come attività dell'operatore, con tracker e cursori, non come riconoscimento automatico di firme.
- La descrizione moderna («Automated and manually designated threat events are preloaded into the sonar computers... the sonar display will mark the detection») riguarda sistemi del 2020 e non va retrodatata. [V, TWZ] Confidenza alta come lettura; applicazione al 1989: nessuna.

### 3.3 Salvataggio di segmenti e richiamo

- Nel 1989: non ho trovato nessuna fonte che descriva funzioni di salvataggio, richiamo, marcatori o annotazioni digitali nelle console BQQ-5/BQQ-6. [non trovato] Confidenza: nessuna sul 1989.
- Cosa si può dire: i tracker memorizzano storia di contatto («stores contact history... When the sonarman assigns his tracker, the sonar computer already has the recent history») ma è descrizione del 2020. [V, TWZ] Annotazione e verifica per la registrazione («recording verification, annotation, and documenting the events») compaiono sempre in TWZ come lavoro delle guardie rafforzate. [V, TWZ] Confidenza: media per l'uso, bassa per la datazione.
- Per il 1989: carta (LOFARgram, grafici), nastro (backup audio/hydrophone), e il materiale ACINT portato a terra per la riduzione. [D da quanto sopra] Confidenza media-bassa.

---

## 4. Sistemi e postazioni

### 4.1 AN/BQQ-5 e versioni (A-E)

Fonte principale: [Forecast International, BQQ-5(V), archivio 2024](https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641) [V, letto da PDF]. Confidenza alta sui componenti, media sulle datazioni.

| Versione | Descrizione secondo Forecast | Note |
|---|---|---|
| Storico | Primo BQQ-5 sviluppato contro la minaccia sovietica degli anni '70; approvato per il servizio nel 1974; primi esemplari sugli ultimi tre Sturgeon e sui primi 688; in seguito rimontato su sommergibili con BQQ-2; programma di aggiornamento avviato nell'AF 1980. | Discordanza con altre fonti sulle date di prima installazione: nessuna. |
| BQQ-5 base (688) | BQS-13; CP-1122 elaboratore passivo di banda larga; CP-1123 elaboratore attivo; CP-1124 classificazione del segnale; CP-1125 post-classificazione; J-3107 analizzatore di spettro; **3 consoles OJ-274**; 1 console OH-275 per monitor delle prestazioni; **1 display video analogico IP-1124**; 1 calcolatore UYK-7. | |
| BQQ-5A(V)1/2/3 | Primo aggiornamento: CP-1359 banda larga, CP-1362 attivo, CP-1365 classificatore; **3 consoles OJ-431**; un UYK-7 aggiuntivo; display video analogici IP-1325 e IP-1417. (A(V)2 per Sturgeon, A(V)3 per Permit con BQS-11 e OJ-432). | |
| BQQ-5B(V)1/2/3 | Beamformer a scafo orientabile; console di controllo migliorata OJ-462 (Los Angeles e Sturgeon) e OJ-463 (Permit). | |
| BQQ-5C(V)1/2/3 | «expanded direction low-frequency analysis and ranging (DIFAR) reception» con unità di interfaccia multipla e analizzatore di spettro digitale; **tre UYK-44**; CP-1556 banda larga; **4 console OJ-544**. | La dicitura «DIFAR» (termine delle sonoboe) è probabilmente un errore del testo; la tratto come dato da verificare. |
| BQQ-5D | Blocco per i 688 senza BSY-1; array rimorchiato TB-23 a linea sottile, beamformer multi-array, analizzatore di spettro; sviluppo concluso nel **1988**, poi in servizio. | In pratica nel 1989 è appena in entrata. |
| BQQ-5E(V) | Per i 688I non BSY-1, poi Ohio; array TB-29; integrazione LFA, **display a colori per la console OJ-544**, elaborazione doppia del rimorchiato, full spectrum, vernier spaziale. Prima produzione a bassa cadenza AF 1994. | Postdata il 1989 di anni: **non usare per il 1989**. |

Altro:

- Sul 688: sfera BQS-11/12/13 con 1.241 trasduttori; array conformale di scafo con 104-156 idrofoni; due array rimorchiati (TB-12 poi TB-16, e TB-23 o TB-29); cinque versioni A-E; processore **BQR-24** per l'array conformale con FLIT (frequency line integration tracking) per soluzioni su bersagli molto silenziosi. [R, Wikipedia, Los Angeles-class](https://en.wikipedia.org/wiki/Los_Angeles-class_submarine) Confidenza media (enciclopedia; il BQR-24 non l'ho trovato altrove).
- Il BQQ-5 originale fu progettato con **una sola console**, poi si dovette ridisegnarlo a **tre console**, con problemi di spazio, organico e cuccette. [V, GlobalSecurity](https://www.globalsecurity.org/military/systems/ship/systems/an-bqq-5.htm) Confidenza alta.
- Calcolatori: BQQ-5 con UYK-7; il fire control Mk 113 gira su UYK-7. [V, Forecast; R, Wikipedia] Confidenza media-alta.
- Il BQQ-5 originale prodotto da IBM; il BQQ-5 è derivato dal BQQ-2. [R, FAS/Wikipedia] Confidenza media.

### 4.2 AN/BSY-1 (688I, SSN-751 e seguenti)

Fonte principale: [GAO/NSIAD-92-50, gennaio 1992](https://archive.org/details/DTIC_ADA248051) [V, testo OCR integrale].

- SUBACS nato nel 1980; riorganizzato in 3 fasi nel 1983; dopo i problemi di costo, ritardi e tecnici fu diviso: SUBACS Basic è diventato AN/BSY-1 (688I) e SUBACS A/B la base di BSY-2 (Seawolf). Contratto IBM del dicembre 1983 da 772 milioni di dollari. Tolto il bus a fibra ottica, sostituito il processamento distribuito con calcolatori Navy standard **AN/UYK-43**; BSY-1 «retains the AN/BQQ-5 and the CCS MK-1 software codes».
- Sonar: «replacement of SUBACS equipment with less capable AN/BQQ-5 equipment» (più la riduzione del numero di processori e tracker).
- **Operator aids cancellati**: nontraditional acoustic processing, threat detection and classification, torpedo detection, performance prediction, medium frequency post-processing. L'obiettivo originale era ridurre del 25% l'organico del sistema di combattimento. Organico: **25 arruolati previsti per SUBACS Basic; 27 per BSY-1**.
- A novembre 1991: 17 sistemi consegnati, 10 installati, **otto 688 con BSY-1 consegnati**. Prime valutazioni tecniche (aprile-ottobre 1990).
- Confidenza: alta. Nota: il BSY-1 ereditò display e software del BQQ-5/CCS Mk 1, quindi la «postazione» di un 688I del 1989 è probabilmente simile a quella del BQQ-5 [D], non una console completamente nuova.
- [R, FAS BSY-1](https://man.fas.org/dod-101/sys/ship/weaps/an-bsy-1.htm): menziona i display dei dati della nave e il lanciatore orizzontale; la pagina è del 2000. Confidenza media.

### 4.3 AN/BQQ-6, BQR-23, BQR-25, WLR-9

- **BQQ-6** (Ohio): «essentially the BQQ-5 of SSNs, but without an active mode»; Trident con componente rimorchiato del BQQ-6; BQQ-9 TASPE in arrivo. [V, Polmar, Proceedings ottobre 1987](https://www.usni.org/magazines/proceedings/1987/october/us-navy-tails-warships) Confidenza alta. Nel 1983 era appena entrato sulle prime Trident (Ohio e Michigan); i suoi operatori erano più anziani di quelli del Q-5. [V, Kinney 1983](https://archive.org/details/DTIC_ADA125059) Confidenza alta.
- **BQR-23**: vedi sopra (3.1). Per gli SSBN, non per i 688 [R]; non ho una fonte che dica che i 688 lo usassero.
- **BQR-25**: [R, secretprojects] un forum cita «BQR-25 - "STASS" Passive Towed Sonar (1978?)». Non verificato. Confidenza bassa.
- **WLR-9** (ricevitore di intercettazione acustica): «first of a series of acoustic intercept receivers (AIR) for submarines»; due cupole di idrofoni, una nella vela; classifica per frequenza, durata dell'impulso, livello, rilevamento relativo; le versioni correnti sono automatiche. [R, Military Periscope](https://www.militaryperiscope.com/weapons/electronics/electronic-warfare/anwlr-9/overview/) Confidenza media-bassa (pagina a pagamento troncata). Wikipedia lo elenca per i 688 come ricevitore «for detection of active search sonar and acoustic homing torpedoes» con rimando a Polmar, Proceedings ottobre 1979, p. 137. [R, Wikipedia](https://en.wikipedia.org/wiki/Los_Angeles-class_submarine) Confidenza media-bassa. Il WLY-1 lo avrebbe sostituito su 688. [R, FAS WLY-1](https://man.fas.org/dod-101/sys/ship/weaps/an-wly-1.htm) Per il confronto sovietico, vedi 4.6.

### 4.4 Presentazioni e trattamento del segnale: BTR, LOFAR, DEMON, ALI

- **LOFAR / LOFARgram**: spettro di frequenza contro tempo; le righe forti compaiono come linee contro il fondo; i tonali di pale o macchine formano la firma. [V, Wikipedia](https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder) Alta.
- **Waterfall banda larga e narrowband** su un sonar di sommergibile (descrizione moderna): banda larga = energia per rilevamento nel tempo; narrowband = energia divisa per frequenza, pagine di dati esaminate in continuo. [V, TWZ] Alta, ma 2020.
- **BTR** (bearing time record): definizione da brevetto: display di rilevamento contro tempo usato per inizializzare i tracker a banda stretta. [R, snippet di ricerca su US 6,980,486](https://patents.google.com/patent/US6980486). Confidenza media. L'elenco BTR, BFI, NALI, DEMON e LOFAR compare nella scheda L3Harris LFATS (sonar attivo rimorchiato moderno) e non è una fonte per BQQ-5. [V, L3Harris](https://www.l3harris.com/sites/default/files/2022-11/ims-maritime-ocean-systems-data-sheet-LFATS.pdf)
- **DEMON** (Detection of Envelope Modulation On Noise): analisi della modulazione dell'inviluppo del rumore a banda larga, che dà numero di pale, giri d'asse e di elica. [V per la definizione, Mashoshin e Shafranyuk 2014](https://hydrophysics.spbrc.ru/jour/article/download/985/454) (testo estratto dal PDF): «первые работы... появились в начале 1960-х гг. практически одновременно в нашей стране и за рубежом» (i primi lavori comparvero all'inizio degli anni '60 quasi simultaneamente in URSS e all'estero) e l'acronimo inglese DEMON. Dalla stessa fonte: due tipi di modulazione, albero-pala da cavitazione a velocità supercritica e modulazione dovuta al rollio della nave; consente tipo di nave, dislocamento, tipo di propulsore, numero di assi, numero di pale, velocità e cambi di rotta o velocità. Il corso RN include DEMON. Confidenza alta come principio; **nessuna fonte che ho letto prova che una console BQQ-5 del 1989 avesse un display DEMON**: non asserirlo. [non verificato]
- **ALI**: un brevetto USA descrive una «automatic line integrator (ALI)» per rinforzare righe spettrali di sottomarini sepolte nel rumore; il brevetto definisce il LOFARgram come traccia «B» di frequenza contro tempo. [R, snippet su US 5,008,630](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5008630) Confidenza bassa. «Adaptive» line integrator, nome che ricorre in altre fonti, non è stato verificato.
- **FLIT** (frequency line integration tracking): vedi 4.1. [R, Wikipedia] Media-bassa.
- **Tracker e consegna al fire control**: i tracker sono «signal followers that generate bearing, arrival angle, and frequency reports» (CCS Mk 1: elabora i tracker dell'array rimorchiato e della sfera). [R, Wikipedia](https://en.wikipedia.org/wiki/Los_Angeles-class_submarine) Il sonar manda i dati al centro di controllo: l'operatore, se sente qualcosa, «puts a tracker on it so we can send the data to the control room». [V, Navy.mil 2015](https://www.navy.mil/DesktopModules/ArticleCS/Print.aspx?PortalId=1&ModuleId=523&Article=2265525) Il supervisore sonar calcola una soluzione indipendente dai dati grezzi per verificare quella del fire control tracking party; il tracking party ha il «Fusion Plot» (moderno). [V, TWZ] Confidenza media (le due ultime sono descrizioni post-2000).
- **Fire control e calcolatori**: Mk 113 Mod 10 (UYK-7) sui primi 688; **Mk 117**, «the first "all digital" fire-control system», trasferì le funzioni del Mk 75 analogico all'UYK-7, con le console digitali Mk 81, prima nave costruita con il Mk 117 la USS Dallas; poi Mk 1 CCS (Lockheed Martin) con Tomahawk e Mk 2 (Raytheon), e «QE-2» = CCS Mk 2 + BQQ-5E. [R, Wikipedia](https://en.wikipedia.org/wiki/Los_Angeles-class_submarine). Confidenza media-alta (l'enciclopedia cita fonti ufficiali; la ricostruzione dell'ordine di installazione non è verificata).

### 4.5 Operatori per console e organizzazione della guardia

| Dato | Fonte | Stato / Confidenza |
|---|---|---|
| Guardia **6 ore sì, 12 no** su tutti e 12 i sommergibili intervistati nel 1981; alcune navi a 6/6 per carenza di sonaristi; un caso con supervisori a 8-12 ore. | [Kinney 1981](https://archive.org/details/DTIC_ADA100767) | [V] alta |
| Rotazione dei compiti durante la guardia: da ogni mezz'ora a ogni due ore; **la maggioranza, tutti e tre i tecnici ogni ora**. | stessa | [V] alta |
| Equipaggio sonar tipico: «the 12 men on a typical submarine» come campione di valutazione. | [Kinney 1983](https://archive.org/details/DTIC_ADA125059) | [V] alta |
| Alcune navi non avevano nemmeno uno sgabello per il supervisore. | [Kinney 1981](https://archive.org/details/DTIC_ADA100767) | [V] alta |
| BQQ-5: da una console a tre; BQQ-5C ha quattro OJ-544. | [GlobalSecurity](https://www.globalsecurity.org/military/systems/ship/systems/an-bqq-5.htm); [Forecast](https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641) | [V] alta |
| Organico del sistema di combattimento BSY-1: 27 arruolati (SUBACS ne prevedeva 25). | [GAO 1992](https://archive.org/details/DTIC_ADA248051) | [V] alta (non solo sonar) |
| Sala sonar con cinque persone (2015); divisione sonar di 14 uomini con metà di guardia (2020); in missione, 8 ore sì e 8 no «Port and Starboard». | [Navy.mil 2015](https://www.navy.mil/DesktopModules/ArticleCS/Print.aspx?PortalId=1&ModuleId=523&Article=2265525); [TWZ 2020](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see) | [V] alta, ma moderni |
| Polmar (1981): il BQQ-5 digitale consentirebbe di ridurre i sonaristi di guardia a «perhaps four». | Snippet di ricerca da [Proceedings, settembre 1981](https://www.usni.org/magazines/proceedings/1981/september/us-navy-sonars-part-2) (pagina a pagamento) | [R] bassa; **non verificato** |

### 4.6 Sistemi sovietici dell'epoca

**Quello che ho verificato**

- **Skat (famiglia)**: sviluppo avviato nel **1968** presso ЦНИИ «Морфизприбор» (V. V. Gromkovskij, direttore, capoprogettista; A. I. Paperno, ingegneria). Costruito su circuiti integrati sovietici, a differenza di «Rubin» (valvole) e «Rubikon» (semiconduttori). **Quattro calcolatori «Karat»** (Kiev), «due delle quali usate nel sistema di ascolto del rumore a bassa frequenza (шумопеленгование), una per la classificazione dei bersagli e una per più sottosistemi». [V, ru.wikipedia, «Скат (гидроакустический комплекс)»](https://ru.wikipedia.org/wiki/Скат_(гидроакустический_комплекс)) Confidenza media-alta. Coerente con Kazakov (2019), che dice che i quattro Karat servivano: uno per OGS, шумопеленгование e гидролокация; due per il шумопеленгование a bassa frequenza; uno per «подсистема классификации». [V, Kazakov](https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody) Confidenza alta sul testo, media come descrizione di capacità reali.
- **Interfaccia di Skat**: per la prima volta una «programmable information display system» (СОИ) su monitor NPO «Mars»; **due monitor** su cui si potevano portare a turno i dati dei sottosistemi, ognuno con il proprio «frame»; il frame OGS aveva moduli (formulari) con parametri numerici dei bersagli e sviluppi lineari degli spettri di frequenza e spaziali, con due modi: oscilloscopico continuo e con memoria degli spettri al momento della rivelazione. [V, Kazakov 2019] Confidenza alta.
- **Funzioni automatiche di Skat OGS**: puntamento fine (точный пеленгатор) con inseguimento automatico, calcolatore per raggruppare i segnali per bersaglio, in modo che all'operatore arrivassero i parametri dei bersagli e non dei singoli segnali. [V, Kazakov] Alta. Riguarda l'intercettazione di sonar attivi, non l'identificazione delle navi dal rumore.
- **Varianti Skat**: MGK-500 «Skat» (Skat-B grande / Skat-M piccolo); MGK-503 «Skat-KS» (con antenna rimorchiata, su progetti 671RTM, 945 e 941; portata di rivelazione 2-3 volte quella di «Rubikon»); MGK-520 «Skat-BDRM»; poi MGK-540 «Skat-3» digitale (prove su K-284, progetto 971, nel 1986-87). [V, ru.wikipedia](https://ru.wikipedia.org/wiki/Скат_(гидроакустический_комплекс)) Confidenza media (enciclopedia; i numeri fra MGK-500 e MGK-503 variano fra le fonti: un sito ricorda «МГК-500 (503?)»). [R, deepstorm.ru](https://www.deepstorm.ru/DeepStorm.files/45-92/nts/945/list.htm)
- **Skat sui Victor III**: Wikipedia inglese elenca MGK-503 Skat-KS («Shark Gill»), antenna passiva di fianco, array rimorchiato «Barrakuda» (solo Victor III), MT-70 per ghiaccio. [R, Wikipedia Victor-class](https://en.wikipedia.org/wiki/Victor-class_submarine). L'Akula usa MGK-540 Skat-3 con array rimorchiato «Pelamida», array laterali e MG-70. [R, Wikipedia Akula-class](https://en.wikipedia.org/wiki/Akula-class_submarine) Confidenza media (le fonti non concordano su MGK-500 o MGK-540 per gli Akula). Nota del 28/10/2026: per l'Akula
en.wikipedia chiama l'MT-70 «sonar intercept receiver», non sonar da ghiaccio; resta da chiarire (dossier 28 §2).
- **Rubikon (MGK-400)**: modernizzazione di MGK-300 «Rubin», in servizio dal 1976; versione di esportazione MGK-400E; sui progetti 877/636. [R, riassunto di ricerca con rimando a Wikipedia russa; Military Periscope sulla versione MGK-400EM](https://www.militaryperiscope.com/weapons/electronics/sonars/mgk-400em-passiveactive-sonar-system/overview/). La versione moderna MGK-400EM «automatizza il processo di acquisizione e classificazione dei bersagli» e segue automaticamente fino a 12 bersagli; ha ridotto gli operatori con console condivise. Confidenza media per la versione moderna; **nulla verificato sulla versione del 1989**. Dalla relazione del 2019 di Kazakov: «Rubikon» installato su diesel e atomici ed esportato, siglato MGK-400 (con trattamento AGC, schema ШОУ). [V, Kazakov] Confidenza alta.
- **Funzioni del complesso sommergibile** (struttura tipica): шумопеленгование, гидролокация, обнаружение гидроакустических сигналов, связь и опознавание, миноискание. [R, brevetto RU20388U1](https://patents.google.com/patent/RU20388U1/ru) Confidenza media (documento moderno).
- **Classificazione per modulazione del rumore (DEMON)**: l'idea è nota in URSS dai primi anni '60 e gli autori (ЦНИИ «Электроприбор») descrivono nel 2014 un algoritmo automatico di classificazione di bersagli basato sull'AM del rumore. [V, Mashoshin 2014](https://hydrophysics.spbrc.ru/jour/article/download/985/454) Alta come principio; **non** è una descrizione dei complessi del 1989.

**Cosa non ho trovato**

- **MGK-500 come complesso separato dal Skat** e **MG-10**: nessuna fonte aperta verificabile. Per MG-10 una ricerca in russo non ha dato nulla di utile. [non verificato]
- Qualsiasi dettaglio d'interfaccia dei pannelli degli operatori sovietici per la classificazione del rumore (cuffie, altoparlanti, display), oltre a quanto sopra sui monitor NPO Mars.
- Il numero di operatori per console nelle ПЛ sovietiche. (Solo: MGK-400EM riduce gli operatori con consolle condivisa. [R])
- Una fonte aperta che dica quanto fosse efficace la classificazione automatica del Karat: l'unica frase è che uno dei quattro calcolatori serviva alla classificazione. Confidenza bassa sulla capacità.

---

## 5. Dettagli d'interfaccia d'epoca (colori, monitor, cuffie, controlli, ambiente)

| Dato | Fonte | Stato / Confidenza |
|---|---|---|
| Illuminazione sala sonar (12 sommergibili, 1981): quattro con luce blu sopra la testa, quattro con rosso, due con rosso più luce bianca di riflesso (da lampade di lettura o BQQ-3), due «rig for black» (solo luce parassita degli strumenti). Illuminamento sulle console principali da **meno di 0,01 a 0,28 fc**; sull'area di lavoro da <0,01 a 0,62 fc. Sui CRT dei BQQ-5 da 0,02 fc di luce rossa a 0,28 fc di luce blu. | [Kinney 1981](https://archive.org/details/DTIC_ADA100767) | [V] alta |
| Distanza operatore-console da 18 a 31 pollici (media 24, dev. standard 3,7); sedie spesso fissate al ponte; controlli di contrasto sui CRT senza indicazione del livello e regolati «as dark as possible» o «whatever looks good». Particolari da distinguere generalmente di 5 minuti d'arco o più. Pochissimo uso del codice colore nella sala. | stessa | [V] alta |
| Lamentele: ventilazione (9 navi su 12), sedili (5), dimensione della sala, luce rossa (mal di testa); rumore e cuffie scomode «mentioned infrequently»; alcuni equipaggi avevano messo la moquette. Fumo: su otto navi metà o più degli uomini fumava. | stessa | [V] alta |
| Blu preferito al rosso perché dà più luce effettiva; la luce rossa era un residuo dei periscopi. | stessa | [V] alta |
| Display CRT: «waterfall» in cui il rumore è una distribuzione casuale di punti luminosi e ogni bersaglio una linea lungo il suo rilevamento; dati del bersaglio possono essere scritti ai margini. Fino a sette colori è il limite tradizionale; esperimenti con più colori; fondo nero tipico, il blu non migliora il rilevamento. «Color-coded CRT displays will soon appear on board U.S. submarines.» | [Luria, DiVita, Neri, Proceedings, febbraio 1988](https://www.usni.org/magazines/proceedings/1988/february/improving-submarine-sonar-displays) | [V] alta. Implica che molti display del 1988-89 non erano a colori [D] |
| Colore sui display BQQ-5E (console OJ-544): «color displays for the OJ-544 control console» (BQQ-5E, anni '90). | [Forecast](https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641) | [V] alta; non applicabile al 1989 |
| Cuffie: il programma NSMRL-NAVSEA sviluppò cuffie con cancellazione del rumore «to allow sonar operators to hear acoustic sounds much more clearly»; il sistema binaurale migliora la rivelazione di quasi 7 dB. | [Daniel 2005](https://archive.navalsubleague.org/2005/nsmrl-a-small-command-with-a-huge-presence-for-the-submarine-force) | [R] media. Data di quel lavoro non indicata |
| Sala sonar scura, «green and black lines», cuffie, scrutinio del 360° (Trident, 2015). | [Navy.mil 2015](https://www.navy.mil/DesktopModules/ArticleCS/Print.aspx?PortalId=1&ModuleId=523&Article=2265525) | [V] media; non 1989 |
| Console BQQ-3: carta illuminata con luce propria bianca e rossa o bianca attenuata. | [Kinney 1981](https://archive.org/details/DTIC_ADA100767) | [V] alta |
| Skat sovietico: due monitor NPO Mars grafici/alfanumerici a «frame» per sottosistema; nastro ad anello di 5 s per il riascolto. | [Kazakov 2019](https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody) | [V] alta |
| Telefoni a energia sonora per le comunicazioni silenziose fra le stazioni; segnale «quick quiet» con ciclo delle luci. | [TWZ](https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see) | [V] media; descrizione moderna |
| Rumore dell'ambiente interno: sul BQQ-3 il «burning smell» dello stilo termico. | [Cote](https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm) | [V] alta |

Nota: nessuna fonte che ho letto descrive i controlli fisici delle console BQQ-5 (manopole, tracker ball) o l'impedenza e il tipo delle cuffie in dotazione nel 1989. [non trovato]

---

## 6. Come si potrebbe riflettere tutto questo nel gioco (dedotto, da usare con giudizio)

Tutto questo è [D] (mia sintesi); le fonti sono quelle citate sopra.

1. **Flusso a due tempi**: impressione iniziale + analisi di 30-45 secondi con clue standard (motore, pompe, pale, conteggio) e tracker. Il ritmo dei quattro passaggi di Mackie (impressione dopo 10 s, analisi libera, clue una per una, conferma) è un buon modello per un'interfaccia di classificazione assistita.
2. **Fiducia graduale** e possibilità di errore realistica: i tassi 39-57% di Mackie sono per esercizi con nastri, ma suggeriscono che l'identificazione a orecchio non dovrebbe mai essere certa, soprattutto fra bersagli di minaccia e non minaccia (Mackie sottolinea la sovrapposizione).
3. **Stimulus error**: se il giocatore ha già un'ipotesi (da intelligence), il gioco può rendere più facile «sentire» ciò che si aspetta e più difficile accorgersi del contrario.
4. **Libreria di firme**: modellarla come materiale di intelligence portato a bordo (briefing, ACINT, nastri), non come riconoscimento automatico in tempo reale (nel BSY-1 fu cancellato).
5. **Tonali di pale** per i sottomarini moderni sovietici (Victor III, Sierra, Akula): a bassa frequenza, deboli, richiedono array rimorchiato; i tonali di macchinario sono più alti e già ridotti.
6. **Display**: monocromatici o a pochi colori per il 1989, waterfall banda larga e LOFARgram a banda stretta; i display a colori sono del 1988-90 in avanti (Luria 1988 li annuncia).
7. **Ambiente**: luce rossa o blu bassa, console a circa 60 cm (24 pollici), tre operatori che ruotano ogni ora e un supervisore in piedi dietro di loro.
8. **Sovietici**: due monitor grafici con frame per sottosistema, nastro ad anello di 5 s per i sonar nemici intercettati, calcolatori Karat con un modulo di classificazione; evitare di inventare funzioni di riconoscimento automatico spinto.

---

## 7. Lacune, discrepanze e cose da ricontrollare

**Non trovato / non verificato**

- Testo di «Blind Man's Bluff» (Sontag, Drew, Drew, PublicAffairs, 1998), «Hunter Killers» (Ballantyne, Orion, 2013; titolo USA «Undersea Warriors»), Clancy, «Submarine: A Guided Tour Inside a Nuclear Warship» (Berkley, 1993), Weir e Boyne, «Rising Tide» (Basic Books, 2003): non accessibili, nessuna citazione con pagina. Per un dossier serio consultare una copia.
- Naval Submarine League «Submarine Review»: nessun articolo del 1980-89 sul tema trovato; gli unici pertinenti sono Barron 2001, Andrews 2006, Daniel 2005, Howard 2011, Conley 2009 (tutti pubblicati dopo il 1989 o su epoche diverse).
- Proceedings USNI: Polmar, «The US Navy: Sonars» parte I (luglio 1981) e parte II (settembre 1981), O'Rourke «The End of the Submarine's Era?» (febbraio 1988) e Peppe «Acoustic Showdown for the SSNs» (luglio 1987) sono dietro paywall. Contengono materiale probabilmente rilevante (BQQ-5, organici, parità acustica).
- Funzioni di annotazione, marcatori, salvataggio e richiamo delle registrazioni nelle console BQQ-5/BQQ-6/BSY-1 del 1989: non trovate.
- Display DEMON, ALI, BTR su BQQ-5: non verificati per il 1989.
- BQR-25: solo un forum (bassa); WLR-9: pagina a pagamento (Military Periscope) e Wikipedia; MG-10: nulla.
- Scuola STS a Groton/New London: non confermata; la scuola «A» era a San Diego nel 1990-92.
- Memorie di sonaristi sovietici: nessuna verificata.

**Discrepanze**

- BQQ-3: Cote lo dà come sostituito nei primi anni '70; Kinney 1981 lo trova su 7 dei 12 sommergibili visitati.
- BSY-1: Wikipedia dice che usa un sistema sensore «AN/BQQ-5E»; Forecast International dice che l'upgrade BQQ-5E(V)3 nacque per i 688I «not equipped with» BSY-1; GAO dice che il BSY-1 mantiene software BQQ-5 e CCS Mk 1. Meglio dire: BSY-1 = hardware nuovo (parzialmente tagliato) con software acustico BQQ-5.
- Skat: MGK-500 / MGK-503 / MGK-540 compaiono con assegnazioni diverse nelle fonti (un sito usa «МГК-500 (503?)»; la voce inglese sull'Akula cita MGK-540 e altre referenze MGK-500).
- Forecast chiama «DIFAR» l'ampliamento del BQQ-5C; DIFAR è un termine delle sonoboe; probabile refuso.

**Priorità consigliate per ricerca futura**

1. Procurarsi «Blind Man's Bluff», «Hunter Killers» e Clancy in cartaceo e citare con pagina.
2. Accesso (biblioteca/abbonamento) a Polmar 1981 (Sonars parti I e II) e a Friedman, «The Naval Institute Guide to World Naval Weapons Systems» (1989) per BQR-23/24/25, BQQ-5 e WLR-9.
3. Archivio NSL «Submarine Review» 1982-1990 con ricerca testuale su «sonar», «ACINT», «LOFAR».
4. Per i sovietici: libro «History of Russian Underwater Acoustics» (Brekhovskikh e Lysanov, World Scientific, citato in recensione JASA) e fonti navali russe.

---

## 8. Elenco delle fonti (per riferimento rapido)

Fonti primarie o tecniche lette integralmente (V):

- Mackie, R. R., *Research on Factors Influencing the Interpretation of Sonar Signals*, Human Factors Research Inc. per ONR, giugno 1974: https://archive.org/details/DTIC_ADA002307
- NAVEDTRA 10130-C, *Introduction to Sonar* (rate training manual STG/STS), 1976: https://archive.org/details/DTIC_ADA091009
- Kinney et al., NSMRL Report 955, *Surveys of Lighting and Working Conditions in Submarine Sonar Shacks*, giugno 1981: https://archive.org/details/DTIC_ADA100767
- Kinney et al., NSMRL Report 994, *Predicting Proficiency on Visual Sonar Displays*, gennaio 1983: https://archive.org/details/DTIC_ADA125059
- Laxar et al., NSMRL Report 1010, *The Cognitive Organization of Submarine Sonar Information*, 1983: https://archive.org/details/DTIC_ADA135857
- GAO/NSIAD-92-50, *Navy Acquisition: Development of the AN/BSY-1 Combat System*, gennaio 1992: https://archive.org/details/DTIC_ADA248051
- Cote, O. R., *The Third Battle*, Naval War College Newport Paper (marzo 2000, testo FAS): https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm
- Forecast International, *BQQ-5(V)*, archivio agosto 2024: https://www.forecastinternational.com/archive/disp_pdf.cfm?DACH_RECNO=1641
- Luria, DiVita, Neri, «Improving Submarine Sonar Displays», Proceedings, febbraio 1988: https://www.usni.org/magazines/proceedings/1988/february/improving-submarine-sonar-displays
- Polmar, «The U.S. Navy: Tails for Warships», Proceedings, ottobre 1987: https://www.usni.org/magazines/proceedings/1987/october/us-navy-tails-warships
- Krokel, Proceedings, febbraio 2016: https://www.usni.org/magazines/proceedings/2016/february/nobody-asked-me-navy-should-expand-acoustic-intelligence
- Barron, «The Operator Is Part of the System», Submarine Review, gennaio 2001: https://archive.navalsubleague.org/2001/the-operator-is-part-of-the-system
- Andrews, «Early Days of Submarine Development Squadron 12», 2006: https://archive.navalsubleague.org/2006/early-days-of-submarine-development-squadron-12
- Conley, «The Black Pig and the Red Banner Fleet», 2009: https://archive.navalsubleague.org/2009/the-black-pig-and-the-red-banner-fleet
- Amick, TWZ, 2020: https://www.twz.com/35603/veteran-submariner-on-how-sonar-crews-tirelessly-hunt-enemies-they-cant-even-see
- Navy.mil, «Submarine Sonar Technicians Detect, Track, Classify», 18 marzo 2015: https://www.navy.mil/DesktopModules/ArticleCS/Print.aspx?PortalId=1&ModuleId=523&Article=2265525
- Chetlain, The War Horse, 2024: https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/
- FAS, Submarine Surveillance Equipment Program (1998): https://irp.fas.org/program/collect/ssep.htm
- GlobalSecurity, AN/BQQ-5: https://www.globalsecurity.org/military/systems/ship/systems/an-bqq-5.htm
- Royal Navy, Basic Aural Analysis / Advanced Aural Analysis / Basic Low Frequency Array: https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/basic-aural-analysis , https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/advanced-aural-analysis , https://www.royalnavy.mod.uk/international-defence-training/onsite-courses/submarines/basic-low-frequency-array
- Kazakov, «Развитие аппаратуры обнаружения гидроакустических сигналов в период с 1950 по 2005 годы», Наука, техника и образование, 2019: https://cyberleninka.ru/article/n/razvitie-apparatury-obnaruzheniya-gidroakusticheskih-signalov-v-period-s-1950-po-2005-gody
- Mashoshin e Shafranyuk, *Fundamental and Applied Hydrophysics* 7(4), 2014: https://hydrophysics.spbrc.ru/jour/article/download/985/454
- ru.wikipedia, «Скат (гидроакустический комплекс)»: https://ru.wikipedia.org/wiki/Скат_(гидроакустический_комплекс)
- SDSU, *ASVAB Score as a Predictor of Academic Success in Sonar Technician A School*, 1992: https://archive.org/details/asvabscoreaspred00rayk
- Honeywell/NAVTRAEQUIPCEN, *Generalized Sonar Operator Training*, 1975-76: https://archive.org/details/DTIC_ADA053179
- Lewis e Rimland, NPRDC TR 80-26, 1980: https://archive.org/details/DTIC_ADA085030

Fonti secondarie o solo riportate (R):

- Wikipedia, Los Angeles-class submarine: https://en.wikipedia.org/wiki/Los_Angeles-class_submarine
- Wikipedia, LOFAR: https://en.wikipedia.org/wiki/Low_Frequency_Analyzer_and_Recorder
- Wikipedia, Victor-class e Akula-class: https://en.wikipedia.org/wiki/Victor-class_submarine , https://en.wikipedia.org/wiki/Akula-class_submarine
- Nuclear Companion, AN/BQR-23: https://nuclearcompanion.com/data/an-bqr-23-towed-array-processor/
- FAS, BQR-20 series: https://man.fas.org/dod-101/sys/ship/weaps/an-bqr-20.htm ; FAS BSY-1: https://man.fas.org/dod-101/sys/ship/weaps/an-bsy-1.htm ; FAS WLY-1: https://man.fas.org/dod-101/sys/ship/weaps/an-wly-1.htm
- Military Periscope, WLR-9 e MGK-400EM: https://www.militaryperiscope.com/weapons/electronics/electronic-warfare/anwlr-9/overview/ , https://www.militaryperiscope.com/weapons/electronics/sonars/mgk-400em-passiveactive-sonar-system/overview/
- Daniel, NSMRL, 2005: https://archive.navalsubleague.org/2005/nsmrl-a-small-command-with-a-huge-presence-for-the-submarine-force
- Howard, «Fixed Sonar Systems», 2011: https://archive.navalsubleague.org/2011/fixed-sonar-systems-the-history-and-future-of-the-underwater-silent-sentinel
- Makris e Dyer, JASA 79(5), 1986: https://acoustics.mit.edu/faculty/makris/Environmental%20correlates%20of%20pack%20ice%20noise.pdf ; Greening e Zakarauskas, Canadian Acoustics 20(3), 1992: https://jcaa.caa-aca.ca/index.php/jcaa/article/view/740
- Brevetti US 5,008,630 (ALI) e US 6,980,486 (BTR), da snippet di ricerca
- Polmar, Proceedings, settembre 1981 (paywall): https://www.usni.org/magazines/proceedings/1981/september/us-navy-sonars-part-2
