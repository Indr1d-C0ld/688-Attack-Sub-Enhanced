# 24 — Interazioni fra unità dell'IA: dottrina e dati (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. Il giocatore comanda un 688 Flight I (USA) o un Alfa, Progetto 705 (URSS).
> Domanda: come si comportano **fra loro** le unità dell'IA, non solo verso il giocatore? Nemico contro amico (un
> sottomarino alleato scoperto dall'ASW sovietica, una fregata amica che caccia un battello sovietico) e amico con
> amico (navi, aerei e sottomarini della stessa parte che cooperano; due sottomarini amici nella stessa zona).
> Scopo: dottrina e numeri per modellare queste interazioni secondo la situazione (pace, crisi, guerra), il teatro, i
> tipi di unità, i sensori, il meteo e il livello d'allerta («paranoia») di ciascuna forza. È la Fase 0 del punto 31
> della roadmap (`docs/00_ROADMAP.md`).
> Compilato: 26 ottobre 2026, da una sola ricerca (fonti web e file del progetto). Nessun altro file del progetto è
> stato modificato.

---

## 0. Convenzioni e punto di partenza

**Confidenza.**
- **alta:** fonte primaria o ufficiale (testo di un trattato, documento declassificato, inchiesta della US Navy), o
  fatto confermato dalle due parti;
- **media:** fonte secondaria seria (Coté, *Proceedings*, *Naval War College Review*, Wikipedia con fonti, memorie
  di un comandante con date e nomi);
- **bassa:** testimonianza isolata, stampa, forum, dato di una sola parte su un fatto conteso;
- **n.v.:** non trovato o non riverificato in questa sessione.

**Marcature:** **STIMA** = numero proposto, da tarare; **[GIOCO]** = proposta di design; **[CALC]** = calcolo da dati
citati.

**Sigle delle fonti** tra parentesi quadre; la tabella con gli URL è al §9. Le sigle già definite in altri dossier
(`[COTE]`, `[CIA-REG86]`, `[CIA-MT75]`, `[ART-Be12]`, `[NSA75]`, `[WP-SWE]`, `[TYL]`…) mantengono lo stesso
significato e si rimanda alla loro tabella.

**Accesso.** Il 26-10-2026 la CIA Reading Room era di nuovo in manutenzione e DTIC rispondeva 403 anche a `curl`;
archive.org non era raggiungibile dallo strumento. I documenti CIA e le tesi già letti per i dossier 02 e 17 si
citano da quei dossier; quelli nuovi solo dai titoli e dagli estratti dei motori di ricerca, e lo si dice caso per caso.

**Che cosa c'è già altrove** (non si ripete, si rimanda):
- **02** §7.2 (Crazy Ivan), §7.3 (bastioni), §7.4 (KPUG sovietici), §7.5 (difesa a strati NATO), §7.7 (EMCON), §8
  (meteo e sensori), §4.3 (rumore del traffico);
- **03** §C4: diciotto comportamenti dell'IA già derivati dalla dottrina (pulizia dei baffles, Alfa intercettore,
  «securing the port», pedinamento di gruppi, uscite di massa, scorta degli SSBN, KPUG, prosecuzione, pace e
  guerra, pedinamento USA, barriera su cue, ROE delle missioni originali);
- **08** §5.1 (precedenze dei messaggi, FLASH ≤ 10 min), §7 (radiogoniometria e catena dal fix alla minaccia);
- **10** §1.3 (definizioni NATO di WSM, PMI, *submarine action area*, *ASW free area*, *submarine haven*), §1.5
  (problema SSN-aerei), §1.7 (coordinamento SOSUS-aerei-SSN), §4.2-4.3 (ROE e zone che cambiano per radio), §0 (scala
  dei tempi compressa);
- **11** §3 e §6.2 (EMCON delle due parti e macchina a stati);
- **16** §1.1 (collisioni fra sottomarini), §1.3 (reti e cavi), §1.4 (urti voluti, accordo INCSEA), §4.3 (IA che
  evita le collisioni);
- **17** §5.2-5.6 (datum SOSUS, scorta della portaerei, coordinazione, criteri d'attacco, zone sovietiche), §7
  (macchina a stati dell'ASW aereo), §9 (efficacia storica);
- **21** §6 (effetti del meteo sui sensori);
- **22** §2.4-2.5 (scala pace-crisi-guerra per il cannone, segnali esplosivi);
- **23**: episodi E02, E11, E13, E19, E20, E22, E25, E30, E31 e schemi P02, P03, P11, P12.

**Il motore oggi** (letto il 26-10-2026, non modificato):
- l'originale non ha dottrina di gruppo: il coordinamento passa da variabili di scena e dalla marcatura dei contatti;
  l'elicottero va dove una nave amica è stata colpita (`docs/originale/comportamenti_ia.md` §1-4) `[IA-ORIG]`;
- l'editor ha `situazione` → `ARMI` = 0 pace, 1 crisi (l'IA spara solo dopo un lancio del giocatore), 2 guerra, e
  una `paranoia` 0-3 per missione e per unità che regola **probabilità di ping** 0,05 / 0,2 / 0,4 / 0,7,
  **aggiornamenti per l'aggancio** 8 / 5 / 3 / 2, inseguimento da 1, reazione alle tracce medie a 3, **vigilanza**
  ogni 180 / 90 / 45 s (`engine/missioni/src/scenario_missione.cpp`, righe 396-398 e 1452-1495) `[SCEN]`;
- le unità `neutrale` vengono scritte nello script come parte `usa` (riga 1490): l'IA sovietica le tratta come
  nemiche, quella americana come amiche. È un'asimmetria da togliere (§8.7).

Tutto ciò che segue riguarda l'**Enhanced**. Il classico resta identico all'originale.

---

## 1. Sintesi

1. **Il fuoco amico fra sottomarini e forze ASW è il rischio principale, non la collisione.** Nella Seconda guerra
   mondiale gli USA persero probabilmente due battelli per fuoco amico, il *Dorado* (1943) e il *Seawolf* (1944),
   tutti e due **dentro zone protette che l'attaccante conosceva male** `[WP-Dorado]` `[WP-Seawolf]`. Nel 1943 la
   zona mobile attorno a un battello in transito era di **50 nm avanti, 100 indietro, 15 per lato**, e all'aereo del
   *Dorado* fu data un'area spostata di 31 nm `[WP-Dorado]` (media-alta). La Guerra fredda non ha casi di fuoco amico
   perché nessuno sparò; ha invece una ventina di collisioni (16 §1.1).
2. **La separazione si fa nello spazio, non con l'identificazione.** Le zone (area d'azione del sottomarino, area
   libera ASW, zona mobile) dicono **chi può sparare a un sottomarino e dove**. Dentro l'area di un sottomarino amico,
   ogni contatto subacqueo è suo; fuori, gli aerei e le navi attaccano ciò che classificano ostile. L'identificazione
   acustica di un battello amico non esisteva in modo affidabile: Coté parla di problemi «di comunicazione e di
   identificazione» che hanno sempre ostacolato la cooperazione sottomarino-aereo `[COTE]` (alta).
3. **Le due parti cooperavano in modo diverso.** La NATO lavora a strati: SOSUS, aerei e sottomarini **separati**
   (aree avanzate agli SSN da soli, aree arretrate agli aerei), navi con schiera trainata ed elicotteri vicino al
   gruppo `[COTE]`. I sovietici lavorano con **forze eterogenee coordinate dal centro**: ricerca «a chiamata», aerei
   che passano sotto il comando del gruppo navale oltre una linea, navi messe fra le aree degli SSN e degli aerei
   perché il battello che le evita si faccia sentire da chi ascolta (17 §5.6, `[CIA-REG86]`, `[CIA-MT75]`).
4. **Le informazioni invecchiano a velocità diverse.** Dentro un gruppo un contatto passa in **0,5-2 minuti** (voce,
   Link 11); dal gruppo al comando di teatro in **≤ 10 minuti** se FLASH (08 §5.1); a un sottomarino in immersione
   solo al **ciclo di broadcast successivo** (10 §0); un cue SOSUS arriva all'aereo **2,5-4 ore** dopo il fix (17
   §5.2). I sovietici passano bene i contatti fra mezzi vicini (un Be-12 ai Ka-25 in 12 minuti) ma ne perdono molti:
   **12 passaggi riusciti su 74 scoperte** in un caso storico `[ART-Be12]` (alta).
5. **In pace nessuno spara, ma tutti si guardano.** L'accordo INCSEA del 1972 vieta attacchi simulati, puntamento delle
   armi, fari sui ponti e manovre che intralciano le formazioni; impone di stare discosti quando si pedina
   `[AVALON-INCSEA]` (alta). **Non regola i sottomarini in immersione** (16 §1.4): il pedinamento ravvicinato, i
   ping e le collisioni continuano. Nel giugno 1989 USA e URSS firmano un secondo accordo sulle attività militari
   pericolose (laser, interferenze, zone di cautela speciale) `[UNTS-DMA]` (alta).
6. **Il pedinamento è il mestiere normale del tempo di pace**, dalle due parti: navi con schiera trainata dietro ai
   sottomarini sovietici nel Mediterraneo dal 1970 `[IPMS-TAIL]`; AGI sovietici fissi fuori dalle basi degli SSBN,
   «marcati» a loro volta da navi britanniche `[PINS25]`; SSN sovietici sulle uscite degli SSBN americani (racconti
   sovietici: **17-20 ore** di contatto con l'*Ohio* nel 1982, **10 giorni** dentro un gruppo di tre portaerei nel
   1983) `[NVO16]` `[NVO19]` (bassa-media, una sola parte).
7. **Più paranoia significa più contatti, ma soprattutto più falsi contatti.** Nella FLEETEX 83-1 il gruppo USA
   ricevette circa **300 rapporti di contatto in cinque giorni, di cui solo 7 credibili** `[NVO16]` (bassa-media); i
   campi di boe del Korshun davano il 50% di falsi allarmi (17 §9); alle Falkland gli inglesi spesero gran parte delle
   armi ASW su balene e banchi di pesce `[NHF-SS]` (media-bassa). La paranoia deve alzare sia la scoperta sia gli
   errori.
8. **Nel 1989 l'asimmetria acustica si stava chiudendo.** Negli anni '70 un sovietico era **20-40 dB più rumoroso**
   del coevo americano, con portate di scoperta che potevano differire fino a un fattore 100 `[TYL]` (media). A metà
   anni '80 la parità con la maggior parte degli SSN americani, salvo i 688 migliorati, era «un esito molto
   probabile»; l'Akula era più silenzioso del Delta ma più rumoroso degli ultimi 688 `[COTE]` (alta). Nel 1989 la
   stampa riferiva battelli americani **messi più volte in rotta di collisione** con sovietici che non sentivano
   `[NSL89]` (media).
9. **In guerra la reazione a un alleato colpito è rapida e locale.** Chi è vicino e ha un elicottero va sul datum;
   chi è il bersaglio probabile si allontana; i sottomarini amici scendono e si fermano ad ascoltare. Il regolamento
   sovietico del 1983 ordina, se i contatti non si distinguono, di **colpirli tutti** (17 §5.5): il rischio di fuoco
   amico è più alto dal lato sovietico.
10. **Meteo e mare non sono simmetrici.** Gli elicotteri sovietici non immergono il sonar di notte e le loro boe Baku
    sono inutili oltre mare 3 (17 §7.3); gli americani hanno schiere trainate che soffrono meno il mare. Le stesse
    regole devono valere per l'IA e per il giocatore (§7.4).
11. **Per il gioco** (§8): una **allerta per parte e per teatro**, dinamica, che riusa le tabelle di `paranoia` già nel
    motore; **messaggi di contatto con ritardo ed ellisse** che crescono col tempo; **zone dello spazio d'acqua**
    rispettate dall'IA come dal giocatore; tabelle di reazione a «alleato scoperto / attaccato / affondato» per
    situazione; sette **scene** minuto per minuto da usare come casi di prova.

---

## 2. Gestione dello spazio d'acqua e prevenzione dell'interferenza

### 2.1 Il problema: chi spara a chi sotto l'acqua

Un sottomarino immerso non si identifica a vista e quasi mai al sonar. Per una nave, un aereo o un altro sottomarino
**ogni contatto subacqueo è sospetto**. I due rischi sono diversi:
- **collisione** fra battelli amici che non si sentono (PMI): il caso *Vanguard*/*Triomphant* del 2009, due SSBN
  alleati in aree sovrapposte, nessuno dei due sapeva dell'altro (16 §1.1);
- **fuoco amico** di forze ASW contro un battello amico (WSM), che è un problema solo in guerra.

**I casi della Seconda guerra mondiale** (i soli con armi vere):

| Caso | Che cosa successe | Lezione per il gioco | Fonte, conf. |
|---|---|---|---|
| USS *Dorado* (SS-248), 12-13 ottobre 1943, Caraibi | Zona di divieto di bombardamento attorno al battello: **50 nm avanti, 100 indietro, 15 per lato** della posizione programmata. All'equipaggio del PBM 210-P-9 fu descritta un'area **spostata di 31 nm**. L'aereo attacca un sottomarino «fuori zona» con tre bombe di profondità; un'ora dopo riceve l'avviso che c'è un amico in zona. La corte d'inchiesta: «molto probabile» che il *Dorado* sia stato perso così | La zona protetta vale quanto il messaggio che la descrive: **un errore o un ritardo nel messaggio = fuoco amico** | `[WP-Dorado]` media-alta (cita la corte d'inchiesta) |
| USS *Seawolf* (SS-197), 3 ottobre 1944, Morotai | Dopo il siluramento del *Shelton* da parte di un sommergibile giapponese, la scorta *Richard M. Rowell* e gli aerei della *Midway* cacciano l'attaccante e attaccano un battello **dentro una corsia di sicurezza per sottomarini americani**. Il *Rowell* riferisce segnali dal battello «che non somigliavano» ai segnali di riconoscimento. Il *Seawolf*, invitato a dare la posizione, non risponde | Dopo un attacco nemico **la paranoia del gruppo esplode** e le regole di zona cedono; i segnali di riconoscimento subacquei sono poco affidabili | `[WP-Seawolf]` `[NHHC-Seawolf]` media (la Marina lo dà come «possibile») |
| Royal Navy, 1939-45 | Aerei britannici attaccano il proprio sommergibile *Seahorse*; da qui le zone in cui agli aerei è vietato attaccare sottomarini e la scorta di superficie dei battelli in acque costiere | Le zone di divieto nascono dal fuoco amico | `[PROF-RU]` bassa-media (articolo divulgativo russo) |

### 2.2 Come si separa: spazio, quota, tempo

Le definizioni NATO sono già nel dossier 10 §1.3. Qui conta **come si usano**.

- **Per area.** L'area d'azione del sottomarino è la regola di guerra più semplice: **dentro, solo il sottomarino usa
  armi antisommergibile**; fuori, nell'area libera ASW, nessuna restrizione `[AAP6SAA]` `[AAP6ASWFA]` (alta, da 10
  §1.3). Coté descrive lo stesso principio nelle barriere della Guerra fredda: aree avanzate agli SSN «da soli»,
  aree arretrate vicino alle coste amiche agli aerei `[COTE]` (alta).
- **Per tempo e rotta (zona mobile).** Il battello in transito è protetto da una zona che si muove con la sua rotta
  programmata (10 §1.3, `[AAP6HAV]`). Le dimensioni del 1943 (50/100/15 nm) sono l'unico numero trovato; sono
  asimmetriche **perché il battello può essere in ritardo più facilmente che in anticipo** `[WP-Dorado]`. Le
  dimensioni NATO del 1989: **n.v.** (le fonti aperte del 2026 parlano solo di «moving havens» nei software di
  pianificazione `[SITA26]`).
- **Per quota.** Quando due sottomarini amici devono stare nella stessa acqua (addestramento, uscita da una base,
  sbarramento a strati) si assegnano **fasce di quota** separate. Prassi nota, numeri **n.v.** STIMA: fasce separate
  da almeno 30-50 m.
- **Chi decide.** Le autorità operative dei sottomarini (SUBOPAUTH) ricevono gli avvisi delle attività e deconflittano
  i movimenti subacquei, anche spostando i battelli alleati; una procedura permanente esiste dagli anni '60 `[HYDRO]`
  (media, da 10 §1.3). La collisione del 2009 è stata attribuita anche all'assenza della Francia dall'organizzazione
  NATO della gestione dello spazio d'acqua `[HYDRO]` (media).

**I limiti pratici:**
- il battello riceve gli aggiornamenti delle zone **solo quando copia il broadcast** (10 §1.3): può operare con una
  carta vecchia;
- il gruppo di superficie riceve la zona del suo SSN in supporto diretto, ma l'SSN «resta spesso fuori posizione»
  perché non comunica in tempo reale (10 §1.5, `[USNI87]`);
- dopo un attacco nemico la pressione per sparare cresce e si spara su contatti incerti (caso *Seawolf*).

### 2.3 Quando due sottomarini amici s'incontrano

Nessuna fonte aperta descrive una procedura. Da ciò che si sa (STIMA, media nel principio):
- **Non dovrebbe succedere**: la separazione per area e quota è fatta apposta. Se succede, è perché uno dei due è
  fuori posto o ha una carta vecchia.
- **In pace**: i due battelli si sentono, se si sentono, come contatti sconosciuti. Il primo che classifica l'altro
  come «sottomarino» ma non lo riconosce si comporta come con un estraneo: si allontana in silenzio e riferisce. Il
  pericolo è la collisione, non il fuoco.
- **In guerra, dentro la propria area d'azione**: il battello tratta ogni sottomarino come ostile, perché l'area è
  sua. È il caso in cui un amico fuori posto viene attaccato.
- **Classificazione per nazionalità.** Le righe della rete elettrica a bordo sono a **50 Hz sui sovietici e 60 Hz sugli
  americani** (01 §3.1, `[MIA1]`): un buon operatore con un LOFARgramma pulito distingue la parte. Per i britannici
  il dato è **n.v.** STIMA [GIOCO]: la riga 50/60 Hz è il primo indizio di parte, se il contatto è abbastanza forte
  da mostrare le righe di rete.
- **Telefono subacqueo.** L'AN/UQC lavora attorno a **8-11 kHz** e si sente a qualche miglio con buone condizioni
  `[WP-UQC]` (media-bassa sui numeri). Serve per parlare con una nave o un sottomarino amico, ma **chiunque nel raggio
  lo sente**: in guerra lo si usa solo se la scoperta è già avvenuta o l'area è sicura.

### 2.4 Il lato sovietico

- **Forme d'azione e aree.** Il regolamento del 1983 distingue azioni «in area», «su linea», «su direttrice» e «a
  chiamata»; il comandante del gruppo navale prende sotto controllo aerei ed elicotteri quando passano una linea
  stabilita (17 §5.6, `[CIA-REG86]` alta). Le aree sono quindi assegnate per mezzo e per compito.
- **Separazione deliberata.** Nell'operazione contro gli SSBN di Saakjan (1975) le navi stanno **fra le aree degli SSN
  e quelle degli aerei**, perché il battello che le evita acceleri verso chi ascolta in silenzio (17 §5.6,
  `[CIA-MT75]` alta). È una separazione per area usata come trappola.
- **SSBN e scorta.** L'SSN di scorta naviga 5-15 nm davanti o di lato all'SSBN, sull'asse della minaccia, e ingaggia
  per primo; il manuale dell'originale insiste sul rischio di fuoco amico (03 §C4 n. 6).
- **Il rischio accettato.** «Se non si distinguono più contatti, si colpiscono tutti» (§347) e, se l'elicottero
  rileva oltre la portata della nave, attacca per primo (§358) `[CIA-REG86]` (alta). Il regolamento non risulta
  avere (nelle parti lette) una regola esplicita contro il fuoco amico: **n.v.**
- **Fasce di quota e identificazione subacquea sovietica:** **n.v.** Nessuna fonte russa aperta trovata su
  «зоны запрета применения оружия» o sull'identificazione subacquea (ricerche su deepstorm.ru, militaryrussia.ru e
  nvo.ng.ru senza risultato).

### 2.5 Falkland 1982: separazione in guerra vera

- Tre SSN britannici operavano in **aree d'esclusione** proprie attorno alle isole, separate dal gruppo di superficie
  `[GAZ82]` (alta per il fatto, n.v. per le dimensioni).
- La flotta di superficie ebbe a che fare con un solo sottomarino argentino davvero attivo, il *San Luis*, e spese
  molte armi su contatti falsi; una recensione del libro di Sciaroni e Smith dice che la maggior parte delle risorse
  ASW andò su balene e banchi di pesce `[NHF-SS]` (media-bassa). Il dossier 17 §9 riporta ~50 Mk 46 spesi senza colpi.
- Nessun attacco britannico contro i propri sottomarini risulta nelle fonti trovate.

**Lezione [GIOCO]:** la separazione per area funziona finché ciascuno sa dove sono gli altri; il prezzo è che le navi
attaccano molto di più ciò che sta **fuori** dalle aree amiche, compresi i falsi contatti.

---

## 3. Coordinamento dentro ciascuna parte

### 3.1 La squadra ASW americana e NATO

Già trattati: catena SOSUS → aereo → SSN e strati alternati (10 §1.7); difesa a strati della portaerei con zone e
mezzi (02 §7.5, 17 §5.3); Link 11, «pouncer», SSN in supporto diretto (17 §5.4). Aggiunte:

- **La regola di base fino agli anni '80: ognuno per conto suo.** SOSUS rendeva meno necessaria la cooperazione
  stretta; sottomarini e aerei «andavano per conto loro» `[COTE]` (alta).
- **Anni '80: la cooperazione diventa obbligata.** Con l'Akula e la parità acustica la US Navy passa a tattiche
  **«intensive di ASW aereo»**: i primi distaccamenti di **due LAMPS** su caccia e fregate, la schiera trainata delle
  scorte (SQR-19, provata sul *Moosbrugger* nel 1982) che punta a **più zone di convergenza**, l'elicottero come
  unico mezzo per classificare in fretta e non perdere tempo sui falsi contatti `[COTE]` (alta).
- **Il collo di bottiglia è il canale.** L'ASW coordinata richiedeva molto più accesso ai circuiti UHF via satellite,
  già affollati, e connettività in tempo reale `[COTE]` (alta).
- **Il ciclo nave-elicottero** (STIMA, coerente con 17 §5.4): la nave rileva col TACTAS in prima o seconda zona di
  convergenza (25-60 nm); l'elicottero decolla, posa 3-6 DIFAR sul settore, passa i dati alla nave via Hawk Link;
  la nave li mette in Link 11 per il gruppo; l'S-3 o il P-3 rinforzano il campo; l'attacco è dell'elicottero.

### 3.2 Il sottomarino in supporto diretto

- **Posizione:** davanti al gruppo sull'asse della minaccia, nella zona esterna (17 §5.3), con un settore proprio in
  cui gli aerei amici non attaccano (17 §5.4 [GIOCO]: zona mobile di 10-20 nm).
- **Il problema** è sempre lo stesso: per riferire un contatto l'SSN deve salire alla quota di comunicazione o usare
  una boa (10 §1.5, `[USNI87]`, media). Nel frattempo il gruppo non sa se il contatto che sente è il suo SSN o il
  nemico.
- **Conseguenza [GIOCO]:** l'SSN in supporto diretto è il caso tipico di fuoco amico possibile. Il gruppo non attacca
  i contatti subacquei dentro la zona dell'SSN; l'SSN attacca solo ciò che ha classificato ostile.
- **Lato sovietico:** gli SSN di scorta degli SSBN (§2.4) e quelli che accompagnano un gruppo di superficie (Kirov,
  Kiev) seguono lo stesso schema, con il controllo più centralizzato (10 §1.6: la prassi sovietica preferiva le
  operazioni sostenute da aerei, navi e altri sottomarini, quella americana il «lupo solitario» `[NSL85]`).

### 3.3 La coordinazione sovietica

Già trattati: KPUG (02 §7.4, 03 §C4 n. 9-11), zone e bastioni (02 §7.3, 17 §5.6), procedure degli aerei (17 §6.6).
Aggiunte:

- **Forze eterogenee.** La parola chiave russa è «взаимодействие разнородных сил», interazione di forze diverse. Il
  regolamento del 1983 assegna la **ricerca** alle navi con un gruppo di elicotteri (Moskva, Kiev, coppia
  dell'Udaloy); l'elicottero singolo di Kresta II, Kara, Slava **classifica e passa il contatto** (17 §1,
  `[CIA-REG86]` §355, alta).
- **Passaggio aereo → navi.** Durante Okean-70 i Be-12 rilevano su una barriera e **in 12 minuti passano il contatto
  ai Ka-25**; su 74 scoperte del Be-12, **12 passaggi riusciti**, 26 inseguimenti interrotti per ordine, 18 contatti
  persi, 18 interruzioni per meteo, carburante o boe (17 §5.4, `[ART-Be12]` alta). Le navi «coprivano col loro
  rumore» le boe.
- **Contro le portaerei.** La catena antiportaerei sovietica unisce:
  - le **navi di pedinamento diretto** (unità di superficie, anche AGI, che restano vicino alla portaerei e ne danno
    la posizione in tempo reale): la letteratura americana degli anni '80 le considera la fonte di bersaglio più
    affidabile e, in guerra, bersagli prioritari `[GIL82]` (media, solo il riassunto) `[USNI84]` (bassa, estratto);
  - il **Tu-95RTs** «Bear-D» col sistema di designazione **Uspekh**: 52 costruiti fra il 1963 e il 1969, con radar di
    ricerca in carenatura ventrale e canale dati verso le unità lanciatrici `[RU-Tu95RTs]` (media);
  - i sottomarini lanciamissili (Charlie, Oscar, Echo II) e le navi con missili antinave; gli SSN (Victor) col
    siluro a scia.

  Nella crisi del 1973 i sovietici tennero le portaerei sotto pedinamento ravvicinato e simularono attacchi (23 E11).
- **Ritmo.** La centralizzazione rende i sovietici più lenti a decidere ma capaci di concentrare: «a chiamata», una
  forza va dove un'altra ha un contatto (17 §5.6). Dudko racconta l'operazione «Усатая синица» (1985), una ricerca
  coordinata in tutto il Pacifico settentrionale contro gli SSBN americani, senza dare risultati numerici `[NVO19]`
  (bassa).

### 3.4 Come passano i contatti: canali e ritardi

| Canale | Chi → chi | Ritardo tipico | Fonte, conf. |
|---|---|---|---|
| Voce UHF/VHF nel gruppo | nave ↔ nave, nave ↔ elicottero | 0,5-2 min dal contatto al messaggio (STIMA) | prassi; STIMA |
| **Link 11** | navi, E-2, S-3, P-3 (ogni boa posata va sul link, 17 §5.4) | 2250 o 1364 bit/s, interrogazione a giro (*roll call*) di tutte le unità dalla stazione di controllo `[PAT-L11]` `[WP-L11]` (alta per il protocollo). Ciclo della rete **5-20 s** con 10-20 unità; inserimento della traccia da parte dell'operatore 1-3 min (STIMA) | alta / STIMA |
| Hawk Link | SH-60B → la sua nave | dati delle boe in tempo reale; la nave ritrasmette in Link 11 (17 §5.4, STIMA) | media / STIMA |
| Messaggio FLASH | unità → comando di teatro | ≤ 10 min in stazione (08 §5.1) | alta |
| Messaggio IMMEDIATE | comando → altre forze | 30 min - 1 h (08 §5.1) | alta |
| Broadcast VLF | comando → sottomarino immerso | al ciclo successivo; 2 h nella realtà, 10-15 min nel gioco (10 §0) | alta / [GIOCO] |
| SOSUS → aereo | centro di valutazione → MPA | fix + allerta 1-2 h + transito ≈ **2,5-4 h** (17 §5.2) | STIMA da fonti alte |
| Radiogoniometria strategica | rete DF → forze | ore (08 §7.3) | bassa |
| Telefono subacqueo | sottomarino ↔ nave | immediato, qualche miglio, ma udibile da tutti (§2.3) | media-bassa |
| Be-12 → Ka-25 (sovietico) | aereo → elicotteri del gruppo | **12 min** (Okean-70); riuscita 12/74 | `[ART-Be12]` alta |
| Tu-142M → Tu-142M | aereo → aereo che lo rileva | via data link (17 §6.8) | alta (esistenza) |
| Uspekh | Tu-95RTs → lanciatori | n.v.; STIMA 5-15 min fra rilevamento e dati utili al lancio | n.v. / STIMA |

**L'informazione invecchia.** Ogni passaggio aggiunge ritardo, e il cerchio d'incertezza cresce col tempo e la
velocità del bersaglio (17 §5.2, tabella `r(t) = r0 + u·t`). Un contatto di 30 minuti su un battello a 10 kn ha un
raggio di 6 nm e un'area di oltre 100 nm². È il motivo per cui un contatto passato a voce è utile a un elicottero
vicino e quasi inutile a un gruppo lontano.

---

## 4. Reazioni, scala dell'escalation, pedinamento in pace

### 4.1 La scala pace-crisi-guerra

| Livello | Che cosa fanno le forze ASW | Che cosa fanno i sottomarini | Armi | Precedenti |
|---|---|---|---|---|
| **Pace** | Ricercano, classificano, pedinano, registrano la firma. Sonar attivo usato anche come segnale («ti tengo»). INCSEA per navi e aerei (§4.2). Attacchi simulati solo in esercitazione, SUS o rapporto (17 §7.1) | Pedinano e si fanno pedinare; si sottraggono in silenzio; riferiscono a fine pattuglia o se serve | Nessuna. Il ping è considerato «atto ostile» dal manuale dell'originale (03 §C4 n. 17) | Tailships 1970-73; FLEETEX 83-1; K-492 e *Ohio* 1982 |
| **Tensione** (esercitazioni dell'altro, crisi lontana) | Più voli, più pedinamenti, più ping; EMCON più severo; zig-zag dei gruppi | Più sottomarini in mare (uscite di massa: Aport 1985, Atrina 1987, 23 E21, E25) | Nessuna | Atrina: la US Navy dichiara di aver mandato forze a sorvegliare i battelli `[USNI20]` |
| **Crisi** | **Tenere sotto** il sottomarino con l'attivo continuo, segnali esplosivi non letali per farlo emergere; navi di pedinamento con armi pronte; DEFCON più alto | Evasione; batteria (diesel) che si scarica; sessioni radio obbligatorie | Trattenute; sparare solo per legittima difesa o su ordine. Colpi d'avvertimento lontano dal bersaglio (22 §2.4) | Cuba 1962 (23 E02, 22 §2.4); Mediterraneo 1973 (23 E11) |
| **Ostilità imminenti** | Pedinatori a distanza di tiro; designazione continua; «battaglia per il primo salvo» sovietica (03 §C4 n. 12) | Posizioni di lancio, quote d'attacco | Pronte; un atto ostile (radar di tiro acceso, siluro in acqua) basta per sparare | Sirte 1986: radar di tiro libici e attacchi USA (23 E22) |
| **Guerra** | Attacco su classificazione e fix (17 §5.5); i sovietici con «salva da distruzione sicura» e, se i contatti non si distinguono, su tutti `[CIA-REG86]` | Attacco libero fuori dalle zone amiche | Libere secondo le zone (§2) | Falkland 1982 (23 E16-E18) |

**Prontezza nazionale.**
- **USA:** DEFCON 5 → 1. Nel 1973 Washington passò a DEFCON 3 mentre le flotte erano a distanza di tiro (23 E11)
  (alta).
- **URSS:** quattro gradi di prontezza al combattimento delle forze armate: **постоянная** (permanente, il servizio
  di pace), **повышенная** (accresciuta: raccolta del personale, verifica delle armi), **военная опасность**
  (pericolo di guerra: dopo l'allarme di combattimento, uscita verso le aree di concentramento, munizioni), **полная**
  (piena: forze sulle posizioni e compiti di combattimento) `[RU-BG]` (media). Tempi: **n.v.**

### 4.2 L'accordo INCSEA del 1972 e quello del 1989

**INCSEA** (firmato a Mosca il 25 maggio 1972 da Warner e Gorškov, in vigore lo stesso giorno) `[AVALON-INCSEA]`
`[WP-INCSEA]` (alta):
- art. II: rispettare le regole di rotta (COLREG) nella lettera e nello spirito;
- art. III.1: stare **ben discosti** per evitare collisioni;
- art. III.2-3: non manovrare in modo da intralciare le formazioni dell'altro; le formazioni evitano le corsie di
  traffico;
- art. III.4: chi **sorveglia** deve stare a distanza tale da non mettere in imbarazzo o in pericolo le navi seguite;
- art. III.5: segnali con bandiere, suono e luce secondo le regole e il Codice internazionale;
- art. III.6: **niente attacchi simulati** (puntare cannoni, lanciamissili, tubi lanciasiluri), niente lanci di
  oggetti verso le navi dell'altro, **niente proiettori sui ponti di comando**;
- art. III.7: le navi che si esercitano con **sottomarini immersi** alzano i segnali del Codice che li indicano;
- art. III.8: stare discosti dalle navi che fanno operazioni di volo o rifornimento in moto;
- art. IV: gli aerei si avvicinano «con grande cautela», senza attacchi simulati, acrobazie o lanci pericolosi;
- art. V-VI: segnalare l'intenzione di far decollare o appontare; avvisi ai naviganti 3-5 giorni prima;
- art. VII: scambio d'informazioni sugli incidenti tramite gli addetti navali; riunioni annuali.

**Il protocollo del 22 maggio 1973** estende il divieto di attacchi simulati alle navi **non militari** dell'altra
parte `[WP-INCSEA]` (alta).

**Che cosa ha cambiato.** Winkler conta circa **250 incontri ostili** fra navi e aerei americani e sovietici fra il
1945 e il 1972 `[AU-WINK]` (media); dopo l'accordo gli incidenti diventano rari, ma le fonti trovate non danno una
serie annua (**n.v.**). Gli urti del Mar Nero del 1988 avvennero in un caso che l'accordo non copriva (acque
territoriali rivendicate; 16 §1.4).

**Che cosa non copre:** i **sottomarini in immersione** (16 §1.4: la Russia propose di estenderlo nel 1992). Il
pedinamento subacqueo, i ping, i Crazy Ivan e le collisioni restano fuori.

**L'accordo del 12 giugno 1989 sulla prevenzione delle attività militari pericolose** (Mosca, firmato dai capi di
stato maggiore) `[UNTS-DMA]` (alta per esistenza e definizioni):
- definisce le **zone di cautela speciale**, istituite di comune accordo dove le forze delle due parti sono vicine;
- definisce l'**interferenza con le reti di comando e controllo** (disturbo dei segnali);
- regola i **laser** pericolosi per il personale e l'ingresso involontario nel territorio dell'altro; fissa frequenze
  radio e segnali visivi concordati `[MT15]` (media).
- Entrata in vigore: 1° gennaio 1990 (memoria, **n.v.**). Per una missione del 1989 è un dettaglio di briefing, non
  una regola.

**Regole per l'IA in pace che ne derivano** (STIMA, numeri non nell'accordo):
- distanza minima di una nave o di un aereo da un'unità dell'altra parte: CPA ≥ 1000 yd per le navi che pedinano;
  nessun passaggio di aereo sotto 500 ft entro 1 nm;
- nessun puntamento d'arma, nessun proiettore, nessun lancio d'oggetti;
- la nave che si esercita con un sottomarino amico alza il segnale: un'informazione che il giocatore vede al
  periscopio.

### 4.3 Pedinamento e marcatura come attività di pace

**Navi che pedinano sottomarini.** Dal 1970 al 1973 tre cacciatorpediniere di scorta classe Dealey (*Hammerberg*,
*Courtney*, *Lester*, poi *Van Voorhis*) con la schiera trainata sperimentale ITASS, basati a Napoli, inseguono i
sottomarini sovietici nel Mediterraneo; dal 1974 la fregata *McCloy* col TASS. Il K-324 si impiglia nella schiera
della *McCloy* nel 1983 (23 E19) `[IPMS-TAIL]` (media).

**AGI e marcatori.** I sovietici tenevano pescherecci trasformati e poi AGI veri fuori dagli approdi delle basi
degli SSBN. Fuori da Malin Head, sulla rotta da e per il Clyde, l'AGI stava di solito **da marzo a novembre**; il
Ministero della Difesa britannico gli attribuiva questi compiti `[PINS25]` (media, da documenti d'archivio citati):
- localizzare gli SSBN in transito e riferirne la posizione;
- raccogliere le firme acustiche con idrofoni e boe, forse posate di nascosto (una boa sovietica Type 75 fu
  recuperata nel Massachusetts nel 1983);
- **molestare, seguire o tagliare le schiere trainate** delle navi di superficie;
- intercettazione elettronica e delle comunicazioni; tentativi di scoperta non acustica.

La risposta britannica: una nave **«marcatore»** che stava vicino all'AGI quando un SSBN usciva o rientrava, e un
Nimrod che lo localizzava prima del transito; dal pattugliamento dipendeva se il battello poteva uscire. Nel 1988 il
comando della flotta voleva marcare ogni AGI di continuo, ma mancavano i mezzi `[PINS25]`. Dal lato americano un
ex sonarista ricorda un AGI che seguì il suo SSBN all'uscita da Newport News nel 1987 `[WARHORSE]` (bassa).

**Sottomarini che pedinano sottomarini.** Il lato americano è noto (P02, 03 §C4 n. 13, 16 §1.1). Coté ricorda che
l'**80-90%** dei circa 65 SSBN sovietici restava in porto: il pedinamento in pace riguardava pochi battelli `[COTE]`
(alta). Il lato sovietico è raccontato soprattutto dai sovietici (bassa-media):
- **K-492 e l'*Ohio*, settembre 1982:** scoperta a 180 cavi (circa 18 nm), avvicinamento a 20 cavi (2 nm) con
  registrazione del «ritratto acustico», contatto per **circa 20 ore**; l'*Ohio* cerca di scrollarselo per 17 ore,
  accelera a **17 kn** e attiva un **simulatore del rumore del bersaglio**, poi va verso le acque canadesi. Il giorno
  prima dell'uscita c'erano in zona due navi della guardia costiera, 12 ore prima gli aerei ASW `[NVO16]` `[NVO19]`;
- **FLEETEX 83-1, aprile 1983:** il K-305 trova un gruppo di **tre portaerei e 28 navi** in due giorni e vi resta
  dentro **circa 10 giorni**. Dal registro del gruppo, secondo Dudko: il 4 aprile un breve contatto di un SSN *Los
  Angeles*; il 6 un Intruder vede alberi alzati; il 7 i Sea King fanno quattro «attacchi convenzionali» in 40 minuti.
  Dal 9 al 14 aprile **circa 300 rapporti di contatto, di cui solo 7 credibili**; gli americani credevano di seguire
  sei sottomarini `[NVO16]`;
- **K-147, metà anni '80:** più di cinque giorni sulla scia di un SSBN americano con il sistema non acustico di
  scoperta della scia (SOKS), secondo Černavin `[NVO16]` `[GG-K147]` (bassa-media).

Le contromisure del lato sovietico, nelle parole di Dudko: regime «Тишина» (silenzio), rotta scelta per non farsi
sentire, **navigazione sotto il rumore del traffico mercantile**, scambio dei macchinari per togliere le righe
discrete che servono alla classificazione `[NVO16]` (bassa-media).

**Il ping come segnale, la molestia.**
- **Cuba 1962:** le procedure americane di emersione e identificazione usavano segnali esplosivi da esercitazione e
  il sonar attivo per dire al battello «emergi» (22 §2.4, 23 E02) `[NSA75]` (alta).
- **Mediterraneo 1973:** un comandante sovietico fa gettare in acqua una bomba a mano «come per comunicare con un
  nostro sommergibile», per innervosire gli americani; i voli ASW aumentano (23 E11) (media-alta).
- **Il ping «ti tengo».** In pace l'attivo usato da vicino dice al sottomarino che è stato scoperto e spesso basta a
  farlo andar via: è il comportamento dell'Alfa in *Operazioni subacquee* (03 §C4 n. 3, manuale p. 21). Numeri **n.v.**
- **Crazy Ivan** del bersaglio e pulizia dei baffles: 02 §7.2, 03 §C4 n. 1.

### 4.4 Quando un alleato viene scoperto, attaccato o affondato

Nessuna fonte aperta dà una procedura completa; queste tabelle mettono insieme ciò che le fonti dicono (citato) e ciò
che ne segue (STIMA).

**Alleato scoperto (contatto nemico su un amico, senza armi)** — pace e crisi:

| Chi | Che cosa fa | Fonte, conf. |
|---|---|---|
| L'unità scoperta (sottomarino) | Scende sotto lo strato, rallenta o accelera secondo il rumore del cacciatore, si allontana dal datum; non chiede conferme (10 §1.6) | `[NSL05]` media |
| L'unità scoperta (nave) | Nulla di visibile; EMCON invariato se è già stata vista (11 §6.2) | 11 §6.2 STIMA |
| La sua forza | In pace: riferisce con ROUTINE o PRIORITY; non manda soccorsi. In crisi: IMMEDIATE, e un SSN amico vicino può essere spostato a «coprire» l'alleato (pedinare il pedinatore) | STIMA |
| Il cacciatore | Pedina, ping «ti tengo», chiama aerei e navi della stessa parte («a chiamata» per i sovietici) | 17 §5.6, §4.3 |

**Alleato attaccato (siluro in acqua, esplosione, «flaming datum»)** — guerra, o crisi che diventa guerra:

| Chi | Che cosa fa | Tempi (STIMA) | Fonte |
|---|---|---|---|
| Il bersaglio | Manovra antisiluro (02 §6.4), esche, Nixie | subito | 02 §6.4 |
| Scorta più vicina con elicottero | Va sul datum; elicottero in aria (in allerta di 5-15 min, STIMA); ASROC o SS-N-14 sul datum se il fix è buono | 1-15 min | 17 §5.5, `[NPS-FD00]` |
| Altre navi del gruppo | **Si allontanano** dal datum, zig-zag, velocità alta (il siluro va dove le navi erano) | 1-3 min | prassi; STIMA |
| Aerei in zona | Lasciano la ricerca d'area e vanno sul datum (17 §7.1: D) | ritardo di volo | 17 §7 |
| Sottomarino amico vicino | Fuori dall'area dell'attacco: si ferma ad ascoltare, cerca il lanciatore; **non entra** nell'area dove cadono le armi ASW amiche | 1-5 min | STIMA |
| Comando di teatro | FLASH entro 10 min; ROE di guerra se la crisi non lo era già | 10-30 min | 08 §5.1 |
| Lato sovietico | Attacco immediato sul punto con una salva «da distruzione sicura»; se i contatti non si distinguono, su tutti | subito | `[CIA-REG86]` |

**Alleato affondato.** Come «attaccato», più:
- **paranoia della forza al massimo** per ore: più contatti segnalati, più attacchi su falsi contatti (*Seawolf*
  1944, Falkland 1982);
- ricerca dei superstiti solo se la minaccia è giudicata bassa o per ordine; in guerra la scorta non si ferma vicino
  al relitto (STIMA);
- in pace (incidente, non attacco): soccorso (P10 del dossier 23) e arrivo di osservatori dell'altra parte (K-219, 23
  E23).

**Il caso della scorta che «si vendica» su un amico** (*Seawolf*): la probabilità di fuoco amico cresce nella prima
ora dopo un attacco nemico e dentro un'area dove l'amico non era atteso (§8.3).

### 4.5 Quanto in fretta si sparge l'allarme

| Da → a | Pace | Crisi | Guerra | Fonte |
|---|---|---|---|---|
| Dentro la formazione (navi, elicotteri) | 2-5 min | 1-2 min | 0,5-1 min | STIMA, da §3.4 |
| Formazione → comando di teatro | ROUTINE/PRIORITY, ore | IMMEDIATE, 30 min | FLASH, ≤ 10 min | 08 §5.1 |
| Comando → altre formazioni | ore | 30-60 min | 10-30 min | STIMA |
| Comando → sottomarino immerso | ciclo successivo | ciclo successivo | ciclo successivo | 10 §0 |
| Sovietici, dentro un KPUG | 1-3 min | 1-2 min | 1 min | STIMA |
| Sovietici, aereo → gruppo navale | 12 min, riuscita 0,16 | idem | idem | `[ART-Be12]` |

Moltiplicatore proposto per il lato sovietico **× 1,5** sui ritardi fra formazioni diverse (centralizzazione), **× 1**
dentro la formazione (STIMA).

### 4.6 Come cambiano le ROE

- **Un atto ostile** (siluro in acqua, missile lanciato, radar di tiro acceso sul gruppo) basta per la legittima
  difesa: in crisi l'IA risponde contro **chi** ha fatto l'atto, non contro tutti. È già la regola dell'editor
  (`ARMI = 1`: l'IA spara solo dopo un lancio del giocatore); va estesa agli atti fra unità dell'IA (§8.4).
- **Il passaggio a guerra** arriva per ordine radio (10 §4.2) o per regola di missione (23 §4 M4). L'IA che non ha
  copiato il broadcast resta con le regole vecchie: questo vale per l'IA come per il giocatore.
- **Lato sovietico:** all'inizio delle ostilità si attacca subito; con l'ordine, nucleari per primi (17 §5.5,
  `[CIA-REG86]` §359). Per il gioco i nucleari restano fuori.

---

## 5. Ingaggi fra unità dell'IA in guerra

### 5.1 Asimmetrie di scoperta nel 1989

| Fatto | Valore | Fonte | Conf. |
|---|---|---|---|
| Divario medio anni '60-'70 | sovietici **20-40 dB** più rumorosi dei coevi americani; portate di scoperta fino a ×100 | `[TYL]` (stime di Stefanick) | media |
| Calo del rumore sovietico | circa **−30 dB fra il 1975 e il 1988**; Victor III primo battello a basso rumore | `[TYL]` via 01 §3.1 | media |
| Victor III | «avevamo giudicato male i livelli assoluti» (ammiraglio Watkins) | `[COTE]` | alta |
| Akula | più silenzioso del Delta, più rumoroso degli ultimi 688; a metà anni '80 sfugge al SOSUS; livello a banda larga vicino o pari a Sturgeon e primi 688 | `[COTE]` | alta |
| Parità | con la maggior parte degli SSN americani, salvo i 688 migliorati, «molto probabile» a metà anni '80 | `[COTE]` | alta |
| 1989 | un pannello del Congresso giudica peggiorata la capacità di scoprire i sottomarini sovietici; battelli americani **messi più volte in rotta di collisione** con sovietici in un anno; un gruppo nel Pacifico non sente un sovietico finché non emerge dietro la portaerei | `[NSL89]` | media (rassegna stampa) |
| Lato sovietico | navigazione sotto il traffico, eliminazione delle righe discrete: nella FLEETEX 83 il gruppo non riesce a classificare | `[NVO16]` | bassa-media |

**Vantaggio di scoperta fra le classi del gioco** [CALC, STIMA]. Livelli a banda larga a velocità silenziosa dalla
tabella di 01 §3.2 (valori centrali), più un vantaggio di sensore e di elaborazione per il lato americano (BQQ-5 con
TB-16 contro Skat-KS e Skat-3, 01 §7.1-7.2), STIMA bassa: +3 dB contro Skat-3, +5 contro Skat-KS, +6 contro Rubikon,
+8 contro l'Okean dell'Alfa.

| Chi ascolta → chi è ascoltato | Δ rumore (dB) | Δ sensore (dB) | Vantaggio (dB) | Lettura |
|---|---|---|---|---|
| 688 → Victor III (5 kn) | +10 | +5 | **+15** | il 688 sente quasi sempre per primo |
| 688 → Akula I | 0 | +3 | **+3** | chi è più lento e più profondo sente per primo |
| 688 → Alfa a 5 kn | +15 | +8 | **+23** | l'Alfa lento è comunque rumoroso; veloce è un faro |
| 688 → Kilo a batteria | +2 | +6 | **+8** | il Kilo fermo è il più difficile dei sovietici |
| 688 → Typhoon | +12,5 | +5 | **+17,5** | — |
| Sturgeon → Victor III | +5 | +5 | **+10** | — |
| Trafalgar → Akula I | +2 | +3 | **+5** | — |

Il motore acustico converte il vantaggio in portata. Come ordine di grandezza: con perdita sferica e assorbimento
10 dB valgono un fattore 2-3 nella portata in via diretta, 3-10 in un canale o in convergenza (02 §3.4) [CALC].

### 5.2 Sottomarino contro sottomarino

- **Storia:** un solo affondamento di un sottomarino immerso da parte di un altro immerso, HMS *Venturer* contro
  U-864 (9 febbraio 1945, al largo della Norvegia), con TMA su un battello che navigava allo snorkel (memoria, **n.v.**
  in questa sessione). Nella Guerra fredda nessun ingaggio, solo collisioni durante i pedinamenti (16 §1.1).
- **Coté** pone come variabili decisive della guerra avanzata degli SSN «il rapporto di scambio negli scontri fra
  sottomarini sovietici e americani» e la scorta di siluri, senza dare numeri; le valutazioni della campagna erano
  «quasi universalmente pessimistiche» `[COTE]` (alta).
- **Sequenza tipica** (STIMA, con le armi di 01 §6):
  1. scoperta passiva in banda stretta a 5-20 nm in via diretta, o in prima zona di convergenza (25-30 nm `[COTE]`);
  2. TMA di 10-30 min (02 §5.1) con manovre di tratta;
  3. lancio a **5-10 nm** col Mk 48 guidato a filo, che ha portata ben maggiore; i sovietici con SET-65 (15-16 km) o
     USET-80 (18-20 km), oppure SS-N-15/16 a distanza (01 §6.1, §6.3);
  4. il lancio produce un **transitorio** (+10-30 dB per pochi secondi, 01 §3.3): il bersaglio, se sente, risponde con
     un lancio «di reazione» lungo il rilevamento (in gergo *snapshot*, media) ed evade.
- **Chi vince.** Chi ha il vantaggio del §5.1 spara per primo e spesso non viene sentito: con +15 dB il 688 contro un
  Victor III è nella situazione degli anni '70; con +3 dB contro un Akula vince chi è più lento.

### 5.3 Nave contro sottomarino

- **Scoperta:** sonar di scafo in via diretta 8-15 km, TACTAS in prima o seconda zona di convergenza; i sovietici con
  Titan-2, Polinom e il VDS Vega (01 §7.3, stime bassa). Il gruppo cerca in linea di fronte e a scatti (03 §C4 n. 9).
- **Armi:** ASROC 1-9 km; SS-N-14 ~50 km con designazione dell'elicottero; RBU-6000 ≤ 5,2 km; siluri leggeri dai tubi
  (01 §6.2-6.5).
- **Esiti storici:**
  - *San Luis*, 1982: fermo sul fondo, resiste a 20 ore di caccia di fregate e Sea King (17 §9);
  - *Hangor* contro *Khukri*, 1971: il sottomarino affonda la fregata ASW che lo cerca (23 E10);
  - elicottero su un «flaming datum» di 10 min contro un battello che dosa la velocità: **P ≈ 0,14** `[NPS-FD00]`
    (alta come modello).
- **Lezione [GIOCO]:** la nave ASW è pericolosa quando ha l'elicottero in aria e un datum fresco; da sola, contro un
  SSN silenzioso sotto lo strato, è più bersaglio che cacciatore.

### 5.4 Aereo contro sottomarino

Tutto nel dossier 17 (§6-7, §9). Per le interazioni fra unità dell'IA conta solo che:
- l'aereo riceve il cue da una nave, da un sottomarino o dal SOSUS con il ritardo del §3.4;
- **non attacca dentro una zona protetta di un sottomarino amico**, se la conosce;
- passa il contatto alle navi con la probabilità di 17 §7.2 (0,5 USA; 0,16 URSS).

### 5.5 Sottomarino contro nave

- **Storia:** *Conqueror* e *Belgrano* (23 E16): due Mk 8 a corta distanza, oltre 300 morti; *Hangor* e *Khukri*
  (23 E10).
- **Dottrina sovietica antiportaerei:** SSGN con missili a lunga portata su designazione (Uspekh, navi di pedinamento
  diretto; §3.3); SSN Victor con siluri a scia 53-65K e 65-76, immuni alle esche, contro cui conta solo la manovra
  (01 §6.1).
- **Lato americano:** Mk 48 e Harpoon (01 §6.4); l'SSN in supporto diretto attacca i sottomarini che minacciano il
  gruppo prima delle navi.
- **Reazione della scorta:** §4.4.

### 5.6 Esiti tipici [STIMA, da tarare]

| Ingaggio (guerra, 1989) | Chi scopre per primo | P lancio prima della controscoperta | Pk dell'arma | Note |
|---|---|---|---|---|
| 688 contro Victor III | 688: 0,85 | 0,7 | Mk 48: 0,5-0,7 non allertato, 0,3-0,5 allertato | dal vantaggio di +15 dB |
| 688 contro Akula I | 0,55 / 0,45 | 0,5 | idem | conta la velocità |
| Victor III contro 688 | Victor: 0,15 | 0,2 | SET-65, USET-80: 0,3-0,5 | |
| KPUG (2-3 navi + Ka-27) contro SSN americano lento sotto lo strato | P scoperta 0,05-0,15 per ora | — | RBU, SS-N-14, AT-1: 17 §6.7 | Il-38 in appoggio: 17 §9 |
| Fregata USA con TACTAS + SH-60B contro Victor III | P scoperta 0,3-0,5 per ora in acqua profonda | — | Mk 46: 0,2-0,4 allertato (17 §6.7) | |
| SSN contro scorta ASW in moto | SSN: 0,8 | 0,7 | Mk 48 o 53-65K: 0,6-0,8 | *Khukri*, *Belgrano* |

---

## 6. Traffico neutrale e civile, pescherecci, AGI

### 6.1 Come le unità militari li trattano

- **Mercantili e neutrali:** si applicano le regole di rotta; in guerra le armi sono **trattenute** sui neutrali (10
  §4.2: ROE per categoria). Un siluro con autoguida può però agganciare un neutrale: il rischio va tenuto (01 §6).
- **Neutrali armati:** la Svezia degli anni '80 (22 §2.4, 23 E14-E15) usa colpi d'avvertimento, bombe di profondità e
  mine contro i sottomarini nelle sue acque, indipendentemente dalla bandiera `[WP-SWE]` (alta).
- **AGI:** si marcano e si pedinano (§4.3); INCSEA vale anche per loro come navi della marina sovietica `[AVALON-INCSEA]`.
- **Pescherecci:** si evitano per le reti. Dopo il caso *Antares* (1990) la Royal Navy portò la distanza di sicurezza
  dai pescherecci da 2000 a **3000 yd**; nel Regno Unito 15 incidenti sottomarino-peschereccio nel 1980-90 (16 §1.3)
  (alta). Dudko racconta che nel 1982 l'*Ohio* finì nella rete di un peschereccio e fu rimorchiato in bacino; per lui
  era una storia di copertura americana `[NVO16]` `[NVO19]` (bassa).

### 6.2 Come interferiscono coi sensori

| Effetto | Valore | Fonte, conf. |
|---|---|---|
| Rumore del traffico | mercantili 175-185 dB, petroliere 180-190 dB a banda larga (01 §3.2); il traffico alza il rumore ambiente sotto i 500 Hz (02 §4.3) | media |
| Nascondersi nel traffico | accodarsi a un mercantile a 500-2000 yd (03 §C4 n. 8, bassa); «sotto il rumore del traffico» nei racconti sovietici `[NVO16]` | bassa-media |
| Falsi contatti | le righe discrete delle navi del gruppo generano molti falsi contatti; ~300 rapporti, 7 credibili `[NVO16]`; boe del Korshun 50% di falsi allarmi (17 §9); balene alle Falkland `[NHF-SS]` | bassa-media / alta |
| Navi che coprono le boe | le navi «coprono col loro rumore» le boe degli aerei sovietici (17 §5.4) | alta |
| Pescherecci come falsi bersagli | biologici e pescherecci nelle barriere (23 P01) | media |

### 6.3 Che cosa ne segue per l'IA [GIOCO]

- L'IA **classifica** i contatti di superficie come il giocatore: un mercantile è un contatto finché non è
  classificato; in guerra e con paranoia alta può essere attaccato per errore (§8.7).
- Il traffico denso alza i falsi contatti e abbassa la portata di scoperta dell'IA come quella del giocatore.
- I sottomarini dell'IA usano il traffico per nascondersi (comportamento 8 di 03 §C4).

---

## 7. Livelli d'allerta, «paranoia» e meteo

### 7.1 Condizioni di prontezza a bordo

| Lato | Livello | Significato | Fonte, conf. |
|---|---|---|---|
| USA | Condition I | posto di combattimento (*general quarters*): tutte le stazioni armate | `[NG-BS]` `[CIMSEC-GQ]` media |
| USA | Condition II | posto di combattimento modificato, per una sola minaccia (per esempio ASW) | idem |
| USA | Condition III | **navigazione di guerra**: circa un terzo dell'equipaggio di guardia, armi pronte per un attacco di sorpresa | idem |
| USA | Condition IV | navigazione di pace | idem |
| URSS | Боевая тревога, «готовность № 1» | allarme di combattimento: la nave si organizza per usare le armi e respingere gli attacchi | `[KU-VMF]` media (regolamento attuale, terminologia stabile) |
| URSS | готовность № 2 e № 3 | navigazione di guerra e di pace (memoria, **n.v.**) | bassa |

### 7.2 Come sale e scende la paranoia

**Che cosa la fa salire** (dalle fonti):
- una minaccia segnalata da un'altra forza (FLASH «sottomarino in zona», 10 §4.9);
- un gruppo che si sa osservato (AGI, Bear, pedinatore: 11 §3.2, EMCON che passa a E2);
- un contatto confermato o un attacco simulato in esercitazione: dopo la FLEETEX 83 il gruppo concluse che il
  controllo della situazione subacquea «non era più affidabile» `[NVO16]`;
- l'attacco a un alleato (§4.4);
- una crisi politica (DEFCON, 23 E11).

**Che cosa la fa scendere:** il tempo senza contatti (11 §6.2: dopo 20 min di contatto perso si torna allo stato di
partenza per l'EMCON; la prontezza resta più a lungo), l'ordine di rientro, il cambio di turno.

**Effetti** (dalle fonti e STIMA):
- più sortite, più boe, più ping (03 §C4 n. 11: tenere sotto con l'attivo);
- più falsi contatti segnalati e più attacchi su falsi contatti (§6.2);
- formazioni più strette, zig-zag, velocità del gruppo più alta;
- in pace, i sottomarini dell'altra parte vengono trattati con più aggressività (ping più vicini, pedinamenti
  più stretti).

### 7.3 Presenza di unità nemiche nel teatro

- **Lato americano:** il gruppo passa all'EMCON di gruppo scoperto (11 §6.2), lancia gli elicotteri, accende i sonar
  attivi delle scorte interne, rinforza la zona esterna con S-3 e P-3; un SSN in supporto diretto viene spostato
  sull'asse della minaccia (17 §5.3).
- **Lato sovietico:** «a chiamata»: si manda una forza dove un'altra ha il contatto; gli aerei «spazzano» le aree per
  chiarire la situazione (17 §5.6). In difesa del bastione la ricerca si fa più densa ai passaggi (02 §7.3).
- **Il caso dell'*Ohio* (1982):** dopo il pedinamento le forze ASW americane furono messe in allerta con pattuglie
  aeree intense; per Dudko il K-492 non fu più trovato `[NVO19]` (bassa).

### 7.4 Meteo e mare: le asimmetrie

Gli effetti fisici sono nel dossier 21 §6 e 02 §8; qui solo ciò che cambia **fra le parti**:

| Condizione | Lato USA | Lato URSS | Fonte |
|---|---|---|---|
| Notte | dip normale dell'SH-3H | **niente dip**: Ka-27 solo boe | 17 §7.3 |
| Mare > 3 | DIFAR ancora utili | **boe Baku inutili** | 17 §7.3 |
| Mare ≥ 5, vento ≥ 35 kn | dip impossibile, solo boe | idem | 17 §7.3, 02 §8 |
| Mare ≥ 5-6 | sonar di scafo degradati (+6-10 dB di rumore proprio); **schiere trainate meno sensibili** | sonar di scafo degradati; VDS Vega fino a 100 m meno sensibile | 21 §6.1, 02 §8 |
| Barents | — | boe attive del Korshun che non funzionano (VPK 1987); P di riuscita × 0,2 | 17 §5.6, §7.3 |
| Mare forte, perdita del contatto | il pedinatore perde il gruppo e risale alla cieca (K-314, 16 §1.2) | idem | 23 E20 |
| Calma piatta, sole | periscopio e piuma visibili a miglia (21 §6.1) | idem | 21 §6.1 |

**Simmetria:** gli stessi moltiplicatori (21 §6.1) valgono per l'IA e per il giocatore. Un KPUG col mare 6 rallenta e
perde i contatti come la sua controparte americana; la differenza la fanno solo i mezzi del lato.

---

## 8. Per il gioco [GIOCO]

Tutti i valori sono STIMA salvo dove indicato. Sono pensati per la scala di tempo compressa del dossier 10 §0.

### 8.0 Principi

1. **Simmetria** (roadmap 31): ciò che il giocatore può fare e vedere lo può fare l'IA, secondo i mezzi e il contesto,
   verso il giocatore e fra unità dell'IA.
2. **L'IA sa solo ciò che ha sentito o ricevuto.** Ogni unità ha il suo quadro: contatti propri, contatti ricevuti
   con ritardo ed ellisse, zone ricevute per radio. È la regola già scelta per le collisioni (16 §4.3).
3. **Le regole sono di forza, non di unità.** Situazione, ROE, zone e allerta appartengono a una forza (una parte in
   un teatro); le unità le applicano.
4. **Il classico non cambia.**

### 8.1 Allerta della forza

Ogni forza ha un'**allerta** A0-A3 che riusa le tabelle di `paranoia` già nel motore (§0): la `paranoia` della
missione diventa il **valore iniziale**, poi l'allerta si muove con gli eventi.

| Allerta | Nome | `paranoia` (ping, aggancio, inseguimento, tracce medie, vigilanza) | Prontezza | EMCON (11 §6.2) | Comportamento |
|---|---|---|---|---|---|
| A0 | Routine | 0 (0,05; 8; no; no; —) | Condition IV / постоянная | E1 | rotte di pattuglia, ricerca lenta |
| A1 | Vigilanza | 1 (0,2; 5; sì; no; 180 s) | Condition III | E1-E2 | elicottero in allerta 15 min; zig-zag lento |
| A2 | Minaccia | 2 (0,4; 3; sì; no; 90 s) | Condition II ASW | E2 | elicottero in allerta 5 min o in aria; zig-zag; KPUG in linea di fronte |
| A3 | Combattimento | 3 (0,7; 2; sì; sì; 45 s) | Condition I / № 1 | E2-E4 | tutto in aria, attivo continuo, reazione alle tracce medie |

**Eventi** (incremento dell'allerta della forza che li **conosce**, cioè dopo il ritardo di diffusione del §8.2):

| Evento | Formazione che lo vive | Resto della forza nel teatro |
|---|---|---|
| Contatto subacqueo classificato POSSUB | +1 (max A2) | +0 |
| PROBSUB o CERTSUB | +1 | +1 dopo il messaggio (max A2) |
| Periscopio o albero visto al radar o a vista | +1 | +0 |
| Avviso radio di minaccia (FLASH) | +1 | +1 |
| Ping nemico ricevuto | +1 | +0 |
| Siluro in acqua, esplosione, missile | → A3 | +1 |
| Alleato affondato | → A3 | → A2 almeno |
| Crisi dichiarata / guerra | almeno A1 / almeno A2 | idem |

**Discesa:** −1 ogni 30-60 min di gioco senza nuovi eventi, fino al valore iniziale; mai sotto A1 in crisi e sotto A2
in guerra se il teatro ha avuto un attacco.

**Falsi contatti:** il tasso di falsi contatti dell'IA cresce con l'allerta: × 1 / × 1,5 / × 2 / × 3 da A0 ad A3
(STIMA, dall'ordine di grandezza della FLEETEX 83 e delle boe del Korshun). In guerra, ad A3, una unità ASW attacca
un falso contatto classificato PROBSUB con P = 0,3 (Falkland).

### 8.2 Messaggi di contatto fra unità dell'IA

Ogni contatto passato è un **messaggio** con: classificazione, posizione stimata, ellisse, ora del fix, ora d'arrivo.
L'ellisse cresce con `r(t) = r0 + u·t` (17 §5.2), con u la velocità massima plausibile del contatto.

| Canale | Ritardo | P di perdita | Parte |
|---|---|---|---|
| Dentro la formazione (voce, Link 11) | 30-120 s | 0,02 | USA |
| Dentro un KPUG | 60-180 s | 0,05 | URSS |
| Aereo → nave della stessa formazione | 1-3 min (USA); 12 min, riuscita 0,16 (URSS, `[ART-Be12]`) | — | |
| Formazione → teatro | FLASH 5-10 min; IMMEDIATE 15-30 min (scala del gioco) | 0,05 | entrambe; URSS × 1,5 |
| Teatro → sottomarino | al ciclo di broadcast (10-15 min) | come 10 §4.8 | entrambe |
| SOSUS / sorveglianza fissa → forze | 30-60 min nella scala del gioco | — | USA (e sensori fissi sovietici nei bastioni) |

L'ellisse del messaggio, non la posizione vera, è ciò che l'IA che lo riceve usa per decidere (datum, ricerca,
zone d'attacco).

### 8.3 Zone e fuoco amico

Le zone del dossier 10 §4.3 valgono **per tutti**:
- **Area d'azione di un sottomarino amico:** navi e aerei della stessa parte non usano armi ASW dentro; il sottomarino
  sì, contro ogni sottomarino.
- **Zona mobile:** 50 nm avanti, 100 indietro, 15 per lato (`[WP-Dorado]`) come valore storico; nella scala del gioco
  10-20 nm (17 §5.4). Vale per navi e aerei amici.
- **Area libera ASW:** armi secondo le ROE.

**Probabilità di fuoco amico** (per ogni decisione d'attacco su un contatto subacqueo che è in realtà amico):
- se l'unità ha la zona giusta: **0**, salvo il caso del §4.4 (prima ora dopo un attacco nemico): 0,05 USA, 0,15 URSS;
- se ha una zona vecchia o sbagliata (messaggio perso): l'attacco procede con i criteri normali (17 §5.5);
- errore di classificazione amico/nemico quando il contatto mostra le righe di rete: 0,05 (equipaggio esperto) -
  0,15 (scarso); senza righe di rete, la classificazione di parte non è possibile.

**Due sottomarini amici nella stessa acqua:**
- aree adiacenti con fascia di separazione di 5-10 nm; fasce di quota separate di 30-50 m se l'area è condivisa;
- se si sentono: in pace si allontanano in silenzio e riferiscono; in guerra, dentro la propria area d'azione,
  l'attacco è permesso (è lì che nasce il fuoco amico);
- il telefono subacqueo è un'azione possibile anche per l'IA (portata qualche miglio, udibile da tutti).

### 8.4 Reazioni dell'IA a un alleato scoperto, attaccato, affondato

| Situazione | Evento | Unità ASW più vicina con elicottero | Altre unità di superficie | Sottomarini amici | Aerei | Comando |
|---|---|---|---|---|---|---|
| Pace | Alleato scoperto | — | — | il bersaglio si sottrae (P 0,8) o resta e accetta il contatto (0,2) | — | ROUTINE |
| Pace | Nemico scoperto | pedina; ping «ti tengo» con P = `ping` dell'allerta; CPA ≥ 1000 yd dalle navi | continuano | il sottomarino scoperto: §4.3 | boe, SUS in esercitazione | PRIORITY |
| Crisi | Nemico scoperto | **tiene sotto**: attivo continuo, segnali esplosivi se il manuale di missione li prevede (22 §2.4) | si tengono fra il contatto e l'unità di valore | un SSN amico si porta a coprire (P 0,5) | campo di boe | IMMEDIATE; allerta +1 |
| Crisi | Alleato attaccato | legittima difesa contro **chi ha lanciato**: ASROC/SS-N-14 sul datum | si allontanano | si fermano ad ascoltare; attaccano il lanciatore se lo classificano | sul datum | FLASH; la crisi può diventare guerra per regola |
| Guerra | Nemico scoperto | va sul contatto, attacca coi criteri del 17 §5.5 | continuano in zig-zag | attaccano se fuori dalle zone amiche | sul contatto | FLASH |
| Guerra | Alleato attaccato | sul datum entro 1-15 min | si allontanano a velocità alta per 10-20 min | fermi ad ascoltare; fuori dall'area delle armi amiche | sul datum | FLASH; allerta A3 |
| Guerra | Alleato affondato | come sopra; superstiti solo se nessun contatto da 30 min | come sopra | come sopra | come sopra | allerta A3 per il teatro; falsi contatti × 3 |

**Lato sovietico in guerra:** su un contatto non distinto, attacca tutti i contatti dell'area (§347); l'elicottero
attacca per primo se la nave è fuori portata (§358) `[CIA-REG86]`.

### 8.5 Pace: pedinamento, segnali, INCSEA

- **Distanze di pedinamento** (dalle fonti e 03 §C4): sottomarino su sottomarino 2-10 km con schiera, 500-2000 m senza
  (16 §4.3); sottomarino su gruppo 3-10 nm (03 §C4 n. 4); nave su sottomarino: quanto basta a tenere il contatto
  attivo (2-5 km, STIMA).
- **Ping «ti tengo»:** probabilità per valutazione = `ping` dell'allerta; da vicino (< 2 nm) il sottomarino che lo
  riceve si allontana con P 0,7 (03 §C4 n. 3 e n. 17).
- **Crazy Ivan** dei sovietici: parametri di 02 §7.2 e 03 §C4 n. 1; più frequente con allerta alta.
- **INCSEA per navi e aerei:** CPA ≥ 1000 yd fra navi di parti diverse quando pedinano; niente puntamento, niente
  proiettori; aerei non sotto i 500 ft entro 1 nm. Eccezione: lo **scenario di urto voluto** del Mar Nero (16 §4.3),
  solo per missione.
- **AGI:** unità non combattente sovietica che resta fuori dalle basi o nel gruppo, riferisce le uscite (messaggio di
  contatto alla forza sovietica con ritardo 15-30 min); la parte NATO può mandare un «marcatore».

### 8.6 Ingaggi in guerra

Parametri da usare per gli ingaggi fra unità dell'IA: vantaggio di scoperta del §5.1 (dati del motore acustico, non
regole a parte), distanze e armi del §5.2-5.5, esiti del §5.6 come **controllo di taratura** (le partite simulate
devono restare in quelle fasce).

**Transitorio di lancio:** ogni lancio dell'IA produce lo stesso transitorio del giocatore; chi lo sente risponde con
un *snapshot* lungo il rilevamento con P 0,5 (sottomarino allertato) e manovra antisiluro.

### 8.7 Neutrali e falsi contatti

- **Correggere l'asimmetria del motore:** le unità `neutrale` oggi diventano `usa` nello script (§0). Proposta: una
  parte `neutrale` vera, con armi trattenute su di essa per entrambe le parti, e con ROE proprie per i neutrali
  armati (23 §4 M7).
- **Falsi contatti dell'IA:** dal paesaggio sonoro (biologici, pescherecci, mercantili), moltiplicati dall'allerta
  (§8.1). Un falso contatto classificato PROBSUB in guerra ad A3 può essere attaccato (P 0,3): il giocatore può
  sentire esplosioni ASW lontane senza nessun sottomarino.
- **Siluri contro neutrali:** l'autoguida può agganciare un neutrale; l'IA non lancia se un neutrale è entro 2 nm dalla
  linea del bersaglio, salvo allerta A3 (P 0,3).

### 8.8 Meteo

Gli effetti del 21 §6 e le asimmetrie del §7.4 si applicano all'IA esattamente come al giocatore. In più:
- con mare ≥ 5 le navi ASW dell'IA rallentano a 10-15 kn e passano ai mezzi passivi e alle boe (02 §8);
- i Ka-27 dell'IA non immergono di notte; gli SH-3H sì;
- un pedinatore dell'IA che perde il contatto col mare forte segue lo schema B delle collisioni (16 §1.6): accelera
  verso l'ultimo punto o risale alla cieca.

### 8.9 Scene che il giocatore può vedere

Ogni scena è anche un caso di prova. I tempi sono di gioco.

**S1 — Un SSN alleato scoperto da un KPUG sovietico (Barents, crisi, mare 3, notte).**
Il giocatore (688) è a 15 nm a ovest; un Sturgeon alleato pattuglia l'area vicina.
- t = 0: il Krivak II di destra del KPUG (due Krivak II e un Udaloy con due Ka-27) ha un contatto POSSUB sul VDS a
  6 km. Allerta del gruppo A1 → A2.
- t = 1-3 min: il contatto passa agli altri due (voce). Il gruppo accosta a 30° dal rilevamento e rallenta a 8 kn.
- t = 5-15 min: di notte i Ka-27 non immergono; decollano e posano boe RGB in circolo attorno al datum (17 §7.3).
- t = 15 min: due boe reagiscono: PROBSUB. FLASH al comando della flotta (arriva in ≤ 10 min). Il Krivak accende
  l'attivo, ping ogni 20-30 s: «tenere sotto».
- t = 15-60 min: lo Sturgeon scende sotto lo strato e si allontana a 4-6 kn. Il giocatore **sente i ping** lontani
  e, se ascolta bene, i rumori delle boe e degli elicotteri.
- t = 30 min: il comando di teatro americano, al broadcast, avvisa il giocatore: «unità amica in contatto con forze
  ASW, area X» (10 §4.9).
- t = 60-120 min: se lo Sturgeon esce dal campo delle boe, il KPUG lo perde (P 0,6, 17 §7.2: P di riacquisizione 0,3
  in 30 min). Allerta che scende di 1 ogni 30-60 min.
- **In guerra** la stessa scena finisce con un attacco SS-N-14 o un AT-1 al t ≈ 20 min, e con l'allerta A3 in tutto il
  teatro sovietico.

**S2 — Una fregata amica pedina un Victor III (Mar di Norvegia, pace, mare 2, giorno).**
- t = 0: la Perry col TACTAS ha un contatto in seconda zona di convergenza (~55 nm). Allerta A1.
- t = 5-30 min: l'SH-60B decolla, posa 4 DIFAR; contatto passato in Link 11 al P-3 che arriva dopo 30-60 min.
- t = 60 min: il Victor fa un Crazy Ivan (02 §7.2); il P-3 rifà il campo davanti alla rotta.
- t = 90 min: la Perry si avvicina a 5 km e fa due ping «ti tengo». Il Victor accelera, cambia quota, lancia un'esca
  (che in pace si può usare).
- Nessun attacco; registrazione della firma. Rapporto PRIORITY.
- Il giocatore, se è il 688 in zona, riceve il contatto al broadcast e può prendere il pedinamento quando la Perry lo
  lascia.

**S3 — Navi, aerei e sottomarini sovietici insieme contro un intruso (Barents, guerra, inverno).**
- t = 0: un sensore fisso sovietico dà un'area di 20 × 10 nm (STIMA). Il comando manda «a chiamata» un Il-38 e il KPUG
  più vicino.
- t = 30-60 min: l'Il-38 posa una barriera di RGB-1 trasversale alla rotta presunta (17 §6.6). Il Victor III di
  difesa del bastione resta nella sua area d'azione e ascolta; le navi stanno fra le due aree (Saakjan).
- t = 60-90 min: una boa reagisce; barriera di precisione di RGB-2; il contatto passa al KPUG (P 0,16 di riuscita
  storica; ritardo 12 min).
- t = 90 min: il Ka-27 immerge il sonar; due misure d'eco; il capogruppo assegna l'attacco: AT-1 dall'elicottero o
  SS-N-14 dalla nave.
- Se l'intruso entra nell'area del Victor III, aerei e navi smettono di attaccare e il Victor va a cercarlo.
- Il giocatore che **è** l'intruso vive la scena da dentro; il giocatore dell'Alfa la vede dal lato sovietico e riceve
  i contatti con ritardo.

**S4 — Due SSN amici nella stessa acqua (canale Faroe-Shetland, guerra).**
- Il 688 del giocatore ha l'area A; un Trafalgar dell'IA l'area B, adiacente, con fascia di 5 nm.
- t = 0: il Trafalgar sente un contatto debole sul confine; righe di rete non visibili. Non è classificato.
- Regola: il contatto è nella fascia; il Trafalgar **non lancia**, riferisce al prossimo contatto radio e si allontana.
- Variante «messaggio perso»: il giocatore non ha copiato lo spostamento della propria area; il Trafalgar lo trova
  **dentro** l'area B e lo classifica come ostile con P 0,15: il fuoco amico diventa possibile. Il debriefing elenca
  il messaggio perso (10 §4.2).

**S5 — Un alleato silurato in un convoglio (Atlantico, guerra, mare 4).**
- t = 0: un mercantile del convoglio è colpito. Allerta del convoglio → A3; FLASH.
- t = 1-2 min: la Perry più vicina accosta verso il datum e manda l'SH-60B; le altre scorte si mettono fra il datum
  e il convoglio; il convoglio accosta di 45-90° e accelera.
- t = 5-15 min: l'elicottero posa un cerchio di DIFAR (17 §7.3); P di scoperta ≈ 0,14 sul datum di 10 min.
- t = 15-60 min: falsi contatti × 3; attacchi su falsi contatti possibili (P 0,3 sui PROBSUB). Il giocatore, se è il
  688 in supporto, deve **restare fuori** dall'area dove cadono le armi amiche o farsi riconoscere.
- t = 60 min +: se non ci sono nuovi contatti, l'allerta scende ad A2.

**S6 — Uscita di un SSBN con AGI e pescherecci (pace, Clyde o Kola).**
- t = 0: l'AGI è in stazione all'uscita; la sua presenza è nel briefing (o no, se il Nimrod non l'ha trovato).
- t = 0-30 min: l'SSBN esce in superficie; l'AGI lo vede e manda il messaggio alla sua forza (ritardo 15-30 min).
- t = 30-60 min: un SSN sovietico che aspettava fuori riceve il contatto al ciclo successivo e va verso l'uscita.
- Il giocatore (688) deve «pulire» l'uscita: trovare l'SSN che aspetta, senza armi. I pescherecci danno falsi contatti
  e rischio di rete (CPA 3000 yd).

**S7 — Un Victor III pedina un gruppo portaerei, la crisi diventa guerra (Mediterraneo, crisi).**
- t = 0: il Victor è a 5-8 nm dietro il gruppo (03 §C4 n. 4); un Kara di pedinamento diretto è a vista della
  portaerei. Allerta USA A2.
- Il gruppo zig-zaga, mette gli SH-3H sull'asse; i ping trovano il Victor a t ≈ 40 min; la scorta lo tiene sotto.
- t = 60 min: ordine radio di guerra (regola di missione). Il gruppo USA lo copia subito; il Victor solo alla sua
  sessione successiva (10-20 min dopo).
- Nel frattempo il Kara è il primo bersaglio della scorta (fonte di bersaglio più affidabile, `[GIL82]`); il Victor,
  se non ha ancora l'ordine, **non lancia**. È la «battaglia per il primo salvo» vista dal lato dei ritardi radio.

### 8.10 Dati e ordine di lavoro proposto

**Dati:** un file `interazioni.toml` (nome da decidere con la sessione principale) con:
- tabelle di allerta, eventi e discesa (§8.1);
- canali, ritardi e perdite dei messaggi per parte (§8.2);
- parametri delle zone e probabilità di fuoco amico (§8.3);
- tabelle di reazione per situazione (§8.4);
- distanze di pedinamento e regole INCSEA (§8.5);
- vantaggi di sensore per classe (§5.1).

**Ordine:**
1. **I1 Parte neutrale vera** e fine della mappatura `neutrale` → `usa` (§8.7). Piccolo, toglie un'asimmetria.
2. **I2 Allerta della forza** con gli eventi (§8.1), che riusa `paranoia`.
3. **I3 Messaggi di contatto fra unità dell'IA** con ritardo ed ellisse (§8.2), sugli stessi canali della sala radio.
4. **I4 Zone per l'IA** (§8.3), sullo stesso codice delle zone del giocatore (10 §4.3).
5. **I5 Tabelle di reazione** (§8.4) e atti ostili fra unità dell'IA (§4.6).
6. **I6 Scene S1-S7** come missioni di prova (`data/missioni/prove/`?), con partite simulate su più semi per tarare il
   §5.6.

Questo è un piano da approvare (roadmap 31): il dossier non tocca codice.

---

## 9. Fonti

| Sigla | Fonte | URL | Conf. |
|---|---|---|---|
| `[AVALON-INCSEA]` | *Agreement Between the Government of the USA and the Government of the USSR on the Prevention of Incidents On and Over the High Seas*, 25 maggio 1972, testo (Avalon Project, Yale) | https://avalon.law.yale.edu/20th_century/sov008.asp | alta |
| `[WP-INCSEA]` | Wikipedia, *U.S.–Soviet Incidents at Sea agreement* (storia, protocollo 1973) | https://en.wikipedia.org/wiki/U.S.%E2%80%93Soviet_Incidents_at_Sea_agreement | media |
| `[AU-WINK]` | Recensione di D. F. Winkler, *Cold War at Sea* (2000), *Journal of Indo-Pacific Affairs* (circa 250 incontri ostili 1945-72) | https://www.airuniversity.af.edu/Portals/10/JIPA/journals/Volume-02_Issue-2/Winkler-Bookreview.pdf | media |
| `[UNTS-DMA]` | *Agreement on the Prevention of Dangerous Military Activities*, Mosca, 12 giugno 1989, UN Treaty Series vol. 1566, n. 27309 (letto solo negli estratti: definizioni) | https://treaties.un.org/doc/Publication/UNTS/Volume%201566/volume-1566-I-27309-English.pdf | alta |
| `[MT15]` | T. Frear, «Preventing War Step by Step», *Moscow Times*, 30-8-2015 (contesto dell'accordo del 1989) | https://themoscowtimes.com/2015/08/30/preventing-war-step-by-step-a49236 | media |
| `[WP-Dorado]` | Wikipedia, USS *Dorado* (SS-248) (zona 50/100/15 nm, area spostata di 31 nm, corte d'inchiesta) | https://en.wikipedia.org/wiki/USS_Dorado_(SS-248) | media-alta |
| `[WP-Seawolf]` | Wikipedia, USS *Seawolf* (SS-197); USS *Richard M. Rowell* | https://en.wikipedia.org/wiki/USS_Seawolf_(SS-197) ; https://en.wikipedia.org/wiki/USS_Richard_M._Rowell | media |
| `[NHHC-Seawolf]` | NHHC, *United States Submarine Losses WWII*, Seawolf | https://history.navy.mil/research/library/online-reading-room/title-list-alphabetically/u/united-states-submarine-losses/seawolf-ss-197.html | alta |
| `[PROF-RU]` | «Огонь по своим: почему британские летчики топили британские же подлодки», *Профиль*, 2026 (solo estratto di ricerca) | https://profile.ru/military/ogon-po-svoim-pochemu-britanskie-letchiki-topili-britanskie-zhe-podlodki-1778202/ | bassa-media |
| `[HYDRO]` | «The Prevention of Mutual Interference Within the Subsea Littoral», *Hydro International* (estratto; la pagina risponde 403) | https://www.hydro-international.com/content/article/the-prevention-of-mutual-interference-within-the-subsea-littoral | media |
| `[SITA26]` | Comunicati di Systematic sul PMI in SitaWare Maritime, marzo 2026 (uso moderno dei «moving havens») | https://euro-sd.com/2026/03/allgemein/49962/sitaware-maritime-launches-pmi-planning-capability/ | bassa (commerciale) |
| `[AAP6SAA]` `[AAP6ASWFA]` `[AAP6HAV]` `[USNI87]` `[NSL05]` `[NSL85]` | come nel dossier 10 §8 | — | — |
| `[COTE]` | O. R. Coté Jr., *The Third Battle*, Newport Paper 16 (versione FAS, letta per intero il 26-10-2026 in tre parti) | https://man.fas.org/dod-101/sys/ship/docs/cold-war-asw.htm | alta |
| `[TYL]` | G. D. Tyler, «The Emergence of Low-Frequency Active Acoustics as a Critical ASW Technology», *Johns Hopkins APL Technical Digest* 13(1), 1992 (estratto; il PDF supera i limiti dello strumento) | https://secwww.jhuapl.edu/techdigest/Content/techdigest/pdf/V13-N01/13-01-Tyler.pdf | media |
| `[NSL89]` | Naval Submarine League, *The Submarine Review*, luglio 1989, «In the News» (rassegna stampa) | https://archive.navalsubleague.org/?p=1330 | media |
| `[NVO16]` | V. Dudko (contrammiraglio, ex comandante del K-492), «У нас были победы в холодной войне», *Независимое военное обозрение*, 9-9-2016 | https://nvo.ng.ru/history/2016-09-09/1_coldwar.html | bassa-media (una sola parte) |
| `[NVO19]` | V. Dudko, «Незримая подводная война», *НВО*, 15-3-2019 | https://nvo.ng.ru/concepts/2019-03-15/8_1037_war.html | bassa-media |
| `[GG-K147]` | Ripresa di *Popular Mechanics* sul rapporto CIA del 1972 e sul K-147 (SOKS) | https://blog.geogarage.com/2017/11/how-soviet-union-snooped-waters-for.html | bassa-media |
| `[PINS25]` | «Sir Humphrey», *How the Royal Navy countered Soviet AGIs*, 15-4-2025 (da documenti del MOD) | https://pinstripedline.substack.com/p/how-the-royal-navy-countered-soviet | media |
| `[WARHORSE]` | D. Chetlain, «Cold War Operation Atrina», *The War Horse*, 24-1-2024 | https://thewarhorse.org/cold-war-operation-atrina-when-nuclear-subs-reached-us-coast/ | bassa |
| `[IPMS-TAIL]` | Recensione di J. Rodgaard, *Tailships: The Hunt for Soviet Submarines in the Mediterranean, 1970-1973*, Helion | https://reviews.ipmsusa.org/review/tailships-hunt-soviet-submarines-mediterranean-1970-1973 | media |
| `[NHF-SS]` | Recensioni di M. Sciaroni e A. Smith, *«Go Find Him and Bring Me Back His Hat»*, Helion 2020 (Navy History Foundation) | https://navyhistory.org/2021/08/go-find-him-and-bring-me-back-his-hat/ ; https://navyhistory.org/?p=24012 | media-bassa |
| `[GAZ82]` | *London Gazette*, supplemento 49194 (1982), rapporto sulle operazioni nell'Atlantico del Sud | https://thegazette.co.uk/London/issue/49194/supplement/16113/data.pdf | alta (solo estratto) |
| `[GIL82]` | S. Gilchrist, articolo sulle navi sovietiche di sorveglianza delle portaerei, *Naval War College Review* 35(3), 1982 (solo il riassunto) | https://digital-commons.usnwc.edu/nwc-review/vol35/iss3/8 | media |
| `[USNI84]` | W. H. Gregory, *Proceedings*, febbraio 1984, «Professional Notes» (navi «tattletale», solo estratto) | https://www.usni.org/magazines/proceedings/1984/february/professional-notes | bassa |
| `[USNI20]` | *Proceedings*, ottobre 2020, «Russia Basks in Cold War Glory» (come nel dossier 23) | https://www.usni.org/magazines/proceedings/2020/october/russia-basks-cold-war-glory | media |
| `[RU-Tu95RTs]` | ru.wikipedia, Ту-95РЦ (Uspekh, 52 esemplari 1963-69) | https://ru.wikipedia.org/wiki/Ту-95РЦ | media |
| `[RU-BG]` | ru.wikipedia, Боевая готовность (quattro gradi di prontezza) | https://ru.wikipedia.org/wiki/Боевая_готовность | media |
| `[KU-VMF]` | Корабельный устав ВМФ (testo attuale, comandi d'allarme e готовность № 1; solo estratto) | https://www.consultant.ru/document/cons_doc_LAW_423277/1302fc492cf895cbec125ebe4680048c96124b54/ | media |
| `[NG-BS]` `[CIMSEC-GQ]` | Naval Gazing, «Battle Stations», 2021; CIMSEC, «The General Quarters Drill» (condizioni di prontezza USN) | https://www.navalgazing.net/Battle-Stations ; https://cimsec.org/the-general-quarters-drill/ | media |
| `[PAT-L11]` | Brevetto US 4199809, *Programmable data terminal set* (Link 11: 2250 e 1364 bit/s) | https://patents.google.com/patent/US4199809A/en | alta (protocollo) |
| `[WP-L11]` | Wikipedia, MIL-STD-6011 (Link 11, interrogazione a giro) | https://en.wikipedia.org/wiki/MIL-STD-6011 | media |
| `[WP-UQC]` | Wikipedia, *Underwater acoustic communication* e *Modulated ultrasound* (AN/UQC, 8-11 kHz) | https://en.wikipedia.org/wiki/Underwater_acoustic_communication | media-bassa |
| `[SCEN]` `[IA-ORIG]` | File del progetto: `engine/missioni/src/scenario_missione.cpp`; `docs/originale/comportamenti_ia.md` | — | alta (codice) |
| `[CIA-REG86]` `[CIA-MT75]` `[ART-Be12]` `[ART-Tu142]` `[NPS-FD00]` | come nel dossier 17 §11 | — | — |
| `[NSA75]` `[WP-SWE]` | come nel dossier 22 §11 | — | — |
| `[MIA1]` | come nel dossier 01 | — | — |
| [CALC] | Calcoli di questo dossier: vantaggi di scoperta del §5.1 dalla tabella di 01 §3.2 | — | come indicato |

**Libri citati da memoria, non riletti** (n.v., da verificare con la pagina): N. Polmar e K. J. Moore, *Cold War
Submarines* (2004), per l'U-864 e il *Venturer*; N. Friedman, *The Naval Institute Guide to World Naval Weapon
Systems*, per l'AN/UQC.

---

## 10. Lacune

1. **Dimensioni delle zone mobili NATO del 1989** e delle fasce di separazione per quota: solo il valore del 1943.
   Da cercare in ATP e nei manuali di WSM declassificati, nella tesi NWC del 1997 sul controllo operativo degli SSN
   (DTIC ADA312200, 403 il 26-10-2026).
2. **Procedura sovietica contro il fuoco amico** (aree interdette agli aerei, identificazione subacquea, segnali): nulla
   nelle fonti aperte trovate. Da cercare nel capitolo 2 («Il controllo delle forze») e 4 del regolamento del 1983
   tradotto dalla CIA, quando la Reading Room torna disponibile.
3. **Ritardi reali di Link 11 e di Uspekh**: solo il protocollo; i tempi del ciclo della rete sono in DTIC ADA239577
   e ADA252766 (non scaricati).
4. **Serie annua degli incidenti INCSEA** dopo il 1972: nei libri di Winkler, non letti.
5. **Testo operativo dell'accordo del 1989** (laser, zone di cautela): letto solo nelle definizioni; data d'entrata in
   vigore da verificare.
6. **Racconti sovietici** (Dudko, Černavin): una sola parte, non confermati da fonti americane; i 300 rapporti con 7
   credibili della FLEETEX 83-1 sono citati da Dudko come «dal registro del gruppo».
7. **Gradi di prontezza delle navi sovietiche** № 2 e № 3: n.v.
8. **Identificazione per nazionalità dalle righe di rete** sui battelli britannici: n.v.
9. **Esiti numerici degli scontri sottomarino-sottomarino**: nessuna fonte aperta; il §5.6 è tutto STIMA.
10. **Neutrali armati e pescherecci sovietici con compiti d'intelligence** nei teatri del gioco (Gibilterra,
    Otranto, Kunashir): non cercati a fondo.

**Rimandi interni:** 01 §3, §6, §7; 02 §3.4, §4.3, §5.1, §6.4, §7.2-7.5, §7.7, §8; 03 §C4; 08 §5.1, §7; 10 §0, §1.3,
§1.5-1.7, §4.2-4.3, §4.8-4.9; 11 §3, §6.2; 16 §1.1-1.6, §4.3; 17 §1, §5.2-5.6, §6.6-6.8, §7, §9; 21 §6; 22 §2.4-2.5;
23 E02, E10-E11, E16-E25, E30-E31, P01-P03, P10-P12, §4.
