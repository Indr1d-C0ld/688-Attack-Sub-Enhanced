# F2. Carriera

*Autore: disassemblatore. Revisione del 29/10/2026 (realismo della carriera). Stato: rivisto (curatore: kit).*

## 1. A che serve

Dà un seguito alle missioni. Ogni missione ha un **punteggio**, e il punteggio fa la carriera: le qualifiche, le
commissioni che promuovono o lasciano indietro, le decorazioni, e i provvedimenti quando le cose vanno male (un
neutrale colpito, un battello perso, uno sconfinamento in acque straniere). La carriera è deterministica: niente dadi,
a parità di partite la stessa carriera. Le due marine hanno i loro gradi, le loro commissioni e le loro medaglie.

## 2. Il modello

Il codice è in `nucleo/carriera.hpp` (punteggio, carriera, decorazioni) e `nucleo/fascicolo.hpp` (il fascicolo
salvato); l'adattatore dalla missione è `missioni/carriera_missione.hpp`. I dati sono in `data/motore/carriera.toml`,
da `tools/pipeline/carriera.py`.

### 2.1 Il punteggio di missione

```
P = max(0, (R + C + S + O + E + D + V) × M)
```

| Voce | Massimo | Come si calcola |
|---|---|---|
| R riuscita | 400 | gli obiettivi della missione |
| C completezza | 150 | obiettivi (0,5), classificazioni giuste (0,3), rapporti radio (0,2) |
| S discrezione | 150 | `150 · max(0, 1 − esposizione / 100)` |
| O ordini | 100 | `100 · (0,6 · ordini eseguiti + 0,4 · serie radio copiate)` − 30 per infrazione alle regole d'ingaggio (ROE) − 50 per fuoco senza ordine − 30 per entrata in acque territoriali |
| E efficienza | 100 | `100 · (0,7 · colpi / lanci + 0,3 · (1 − scarto dal carico standard))` − 10 per arma incoerente; senza lanci la parte dei colpi vale 0,7 |
| D danni | fino a −300 | −150 × danno allo scafo, −20 per ferito, −50 per morto, −10 per arma persa; −300 col battello perso |
| V violazioni | fino a −200 | −150 per neutrale colpito, −200 per amico, −100 per fuoco senza ordine, −50 per zona vietata, −150 per l'incidente nelle acque territoriali; sotto −400 la missione è squalificata (P = 0) |

L'**esposizione** somma quello che ha tradito il battello: ping 5, fix 25, avvistamento 20, intercettazione ESM 15,
transitorio 2, boa 10. Dal 31/10/2026 anche le luci: 10 ogni volta che il battello accende le sue luci in superficie
di notte (`luci_accese_notte`, `Missione::conti_giocatore`), in crisi e in guerra. In pace le luci sono la regola
(COLREG) e non pesano. A 100 la discrezione vale zero.

Il **moltiplicatore**:

```
M = situazione × avversario × realismo × (1 + pericolo)
```

- situazione: pace 1, crisi 1,3, guerra 1,6;
- avversario: impreparato 0,8, regolare 1, veterano 1,25, élite 1,5. Vale il minore fra quello dichiarato dalla
  missione e quello calcolato dalle unità ostili (`minaccia_calcolata`);
- realismo: fra 0,5 e 1, dagli aiuti della partita (`realismo_partita` in app/main.cpp; docs/26 §1). Si parte da 1
  e ogni aiuto toglie il suo peso:
  - navigazione esatta, senza posizione incerta: −0,10; se la navigazione è incerta, il GPS «1990+»: −0,05;
  - difficoltà ASW facile, con l'aiuto del telemetro: −0,10;
  - verità dei contatti (V) accesa almeno una volta: −0,05;
  - scie di tutte le unità: −0,05;
  - sonarista esperto: −0,05;
  - LOFAR colorato per rilevamento (RIL, sonar moderno): −0,05, solo se il giocatore ha guardato il LOFAR con RIL
    acceso (`InterfacciaMissione::ril_visto`); acceso di serie non conta, se la stazione non si è mai aperta;
  - senza variazioni acustiche: −0,05.

  Le opzioni più dure non alzano oltre 1, e il classico resta a 1. Il rapporto di pattuglia mostra la catena intera,
  fattore per fattore («somma esatta × situazione × avversario × realismo × pericolo = totale»), e sotto gli aiuti
  accesi col loro peso, o «REALISMO PIENO» se non ce n'è nessuno (`app/esito.cpp`);
- pericolo: fino a +25%, il più alto fra un siluro passato entro 5 km, mezz'ora di contatto con l'ASW e un margine dal
  collasso sotto il 20%.

Col punteggio c'è un **giudizio** a parole, con la voce più forte e la più debole:

<!-- generato: giudizi_carriera -->
| Da punti | Giudizio |
|---|---|
| 850 | eccezionale |
| 600 | ottimo |
| 400 | buono |
| 250 | adeguato |
| 150 | mediocre |
| 0 | insufficiente |
<!-- fine: giudizi_carriera -->

**Le acque territoriali straniere** (UNCLOS art. 20), dichiarate dalla missione, in pace e in crisi:
- ogni entrata toglie 30 agli ordini, come un'infrazione alle ROE;
- farsi scoprire dentro (avvistati, un fix, un ping, l'avvertimento o il fuoco delle navi) è un incidente diplomatico:
  −150 alle violazioni, e il richiamo (USN) o il biasimo (VMF) con il blocco delle promozioni;
- nelle operazioni speciali l'entrata è l'ordine: la scoperta vale −75, senza richiamo; in guerra niente.

L'esempio è lo S-363, il «U-137» incagliato a Karlskrona nel 1981.

### 2.2 Le fasi e le commissioni

La carriera (`Carriera::chiudi_missione`) va per fasi compresse, dal grado più basso al comando: dall'Ensign per la
US Navy, dal lejtenant per la VMF. Oppure la **carriera breve**: si parte da Commander o kapitan 2-go ranga con un
fascicolo precompilato (`Carriera::nuovo`).

<!-- generato: fasi_carriera -->
| Marina | Grado | Incarico | Missioni | Uscita | Media in zona / sotto zona (punti) |
|---|---|---|---|---|---|
| US Navy | guardiamarina (ENS) | scuola e prototipo | 2 | dopo le missioni | — |
| US Navy | sottotenente di vascello (LTJG) | ufficiale di guardia in addestramento | 3 | con la qualifica (dolphins) | media almeno 150 |
| US Navy | tenente di vascello (LT) | ufficiale di guardia in immersione | 4 | con la commissione | 250 / 400 |
| US Navy | capitano di corvetta (LCDR) | ufficiale in seconda | 4 | con la commissione (comando) | 300 / 450 |
| US Navy | capitano di fregata (CDR) | comandante | 10 | con la commissione | 400 / 550 |
| US Navy | capitano di vascello (CAPT) | comandante di squadriglia | — | fine della carriera | — |
| Marina sovietica (VMF) | tenente (л-т) | comandante del gruppo siluri | 1 | dopo le missioni | — |
| Marina sovietica (VMF) | tenente anziano (ст. л-т) | comandante della БЧ-3 | 2 | con la qualifica (esami guardia) | media almeno 150 |
| Marina sovietica (VMF) | capitano-tenente (к-л) | pomoščnik | 3 | con la commissione | 250 / 400 |
| Marina sovietica (VMF) | capitano di 3° rango (к3р) | starpom | 3 | con la commissione (vsok) | 300 / 450 |
| Marina sovietica (VMF) | capitano di 2° rango (к2р) | comandante | 10 | con la commissione | 400 / 550 |
| Marina sovietica (VMF) | capitano di 1° rango (к1р) | comandante anziano di brigata | — | fine della carriera | — |
<!-- fine: fasi_carriera -->

- **Uscite**: dopo un numero di missioni; con una qualifica (le dolphins, gli esami di guardia sovietici, la
  qualifica al comando); con una **commissione** ogni 3 missioni.
- **Commissioni**: con la media sopra la soglia «sotto zona» la promozione è anticipata, sopra «in zona» è regolare,
  sotto si resta «sopra zona». Due commissioni mancate portano al congedo, o alla pensione dopo 20 anni.
- **VMF**: la caratteristica dello zampolit sposta la media del ±10% secondo ordini e ROE; la promozione anticipata
  vuole anche una decorazione dalla Stella Rossa in su.
- **Calendario**: 2 mesi per missione, 3 al comando, dal giugno 1989. La stessa missione rifatta rende sempre meno: 1,
  0,5, 0,25, poi 0.

### 2.3 I provvedimenti

| Fatto | Provvedimento |
|---|---|
| neutrale o amico colpito | rimozione dal comando; corte marziale con infrazioni alle ROE; nella VMF la degradazione |
| battello perso, equipaggio salvo | inchiesta: colpa o fatalità |
| battello perso con tutti | la morte del comandante |
| incaglio, danno al reattore, fuoco senza ordine in pace | richiamo e due commissioni ferme |
| incidente nelle acque territoriali | richiamo (USN) o biasimo (VMF), promozioni bloccate |
| tre esiti mediocri (sotto 150) | trasferimento a terra |

L'**indulgenza** si sceglie per carriera: realistica; indulgente, con una seconda possibilità; arcade, senza
rimozioni ma con un malus.

**Che cosa conta** (`Carriera::conta`): le missioni originali; le rapide col seme registrato e non modificate; le
missioni dell'editor col blocco `carriera`, solo nella carriera «aperta». Una missione ripresa da un istante del
replay non conta.

### 2.4 Le decorazioni

**Personali**: una per missione, dalla tabella della marina per la situazione (pace e crisi insieme, guerra a parte).

<!-- generato: soglie_decorazioni -->
| Marina | Situazione | Punti | Decorazione | Note |
|---|---|---|---|---|
| US Navy | pace e crisi | 250 | Navy Achievement Medal | — |
| US Navy | pace e crisi | 400 | Navy Commendation Medal | — |
| US Navy | pace e crisi | 600 | Meritorious Service Medal | — |
| US Navy | pace e crisi | 850 | Legion of Merit | — |
| US Navy | guerra | 250 | Navy Commendation Medal | con la «V» del combattimento |
| US Navy | guerra | 400 | Bronze Star Medal | con la «V» del combattimento |
| US Navy | guerra | 600 | Silver Star | — |
| US Navy | guerra | 850 | Navy Cross | — |
| US Navy | guerra | 1100 | Medal of Honor | con il pericolo |
| Marina sovietica (VMF) | pace e crisi | 250 | Medaglia «Per distinzione nel servizio militare» di 2ª classe | — |
| Marina sovietica (VMF) | pace e crisi | 400 | Ordine «Per il servizio alla Patria» di 3ª classe | — |
| Marina sovietica (VMF) | pace e crisi | 600 | Ordine della Stella Rossa | — |
| Marina sovietica (VMF) | pace e crisi | 850 | Ordine della Bandiera Rossa | — |
| Marina sovietica (VMF) | pace e crisi | 1100 | Ordine di Lenin | solo nelle operazioni speciali |
| Marina sovietica (VMF) | guerra | 250 | Medaglia «Per merito in combattimento» | — |
| Marina sovietica (VMF) | guerra | 400 | Ordine della Stella Rossa | — |
| Marina sovietica (VMF) | guerra | 600 | Ordine della Bandiera Rossa | — |
| Marina sovietica (VMF) | guerra | 850 | Ordine di Ušakov di 2ª classe | — |
| Marina sovietica (VMF) | guerra | 1100 | Eroe dell'Unione Sovietica | con il pericolo |
<!-- fine: soglie_decorazioni -->

- la seconda decorazione uguale vuole 1,5 volte la soglia, la terza 2 volte; le classi del Servizio alla Patria e
  della Distinzione salgono di classe;
- l'Eroe dell'Unione Sovietica porta con sé l'Ordine di Lenin;
- nessuna decorazione con violazioni.

**Al coraggio** (Navy and Marine Corps Medal, Ordine «Per il coraggio personale»): per un incendio o una falla domati
fuori dalla guerra, se è più alta della personale. Dal 31/10/2026 anche per un soccorso in pace, quando la missione
conta uomini salvati (`m688.salvati`, rapide ed editor col blocco carriera). Allora la motivazione dice il
salvataggio, sobria (formula `soccorso`): «per l'eroismo dimostrato nel soccorso di uomini in mare», e per la VMF «Per
il coraggio e l'abnegazione dimostrati nel salvataggio di vite umane in mare».

**Di reparto**: MUC, NUC e PUC per la US Navy; l'encomio e la Bandiera Rossa al battello per la VMF.

**Automatiche**: Sea Service Deployment, Expeditionary, Arctic, Combat Action Ribbon, Purple Heart; il Servizio
impeccabile per gli anni. **Distintivi**: dolphins, Command at Sea, Comandante di sottomarino. Alla fine del comando,
dopo sei missioni, una decorazione secondo la media.

Il **Combat Action Ribbon** vuole una missione di guerra in cui il battello è stato attaccato: un'arma lanciata contro
di noi, un colpo subito o un siluro evitato. Una cannonata a segno, diretta o di schegge, è un colpo subito; i colpi
d'avvertimento non contano (capitolo F1).

**Postuma**, dopo la perdita del battello, solo per un'azione valorosa (P ≥ 600).

La **motivazione** (`motivazione()`) è scritta in italiano e in inglese dai modelli del dossier, con i fatti della
missione; è vaga per le operazioni speciali. La **barretta** (`barretta()`) mette i nastrini in ordine di precedenza
col numero e il dispositivo, coi colori a righe di `carriera.toml`.

Le decorazioni una per una, con la medaglia disegnata, sono nell'appendice G11.

### 2.5 Il fascicolo

Un file TOML per comandante, con la versione del formato, in `$XDG_DATA_HOME/melchior688/fascicoli/` (o
`~/.local/share/melchior688/fascicoli/`). `salva_fascicolo` scrive in un file temporaneo e lo rinomina. Contiene
anagrafica, grado, fase, calendario, incarichi, qualifiche, note caratteristiche (con la graduatoria «1 di N» fra i
pari grado simulati), decorazioni con le motivazioni, commissioni, provvedimenti e stato.

### 2.6 Dalla missione

`riassumi(m, opzioni)` (`missioni/carriera_missione.hpp`) ricava il riassunto dall'esito e dal registro: armi
incoerenti dai lanci, fatti dai momenti salienti, scarto dal carico (capitolo D5), armi perse, entrate e scoperte
nelle acque territoriali. `contesto_carriera(m, regole)` dà situazione, avversario e difficoltà dal blocco `carriera`
della missione, o per le originali dalla tabella qui sotto, e li confronta con quelli calcolati dalle unità ostili.

<!-- generato: carriera_originali -->
| Missione | Situazione | Avversario | Difficoltà | Operazione speciale | Artico |
|---|---|---|---|---|---|
| Colpisci e fuggi (SCENE0A) | guerra | regolare | 4 | sì | — |
| TORPEX '89 (SCENE1A) | pace | impreparato | 1 | — | — |
| TORPEX '89 (SCENE1B) | pace | impreparato | 1 | — | — |
| TORPEX '89 (SCENE1C) | pace | impreparato | 1 | — | — |
| Scrollatevelo di dosso (SCENE2A) | crisi | veterano | 2 | — | — |
| Scrollatevelo di dosso (SCENE2B) | crisi | veterano | 2 | — | — |
| Scrollatevelo di dosso (SCENE2C) | crisi | veterano | 2 | — | — |
| Operazioni subacquee (SCENE3B) | pace | regolare | 2 | — | sì |
| Mumar Cadaver (SCENE4A) | crisi | regolare | 3 | sì | — |
| Fuga (SCENE5A) | crisi | veterano | 3 | — | — |
| Fuga (SCENE5B) | crisi | veterano | 3 | — | — |
| Fuga (SCENE5C) | crisi | veterano | 3 | — | — |
| Gulasch (SCENE6A) | crisi | regolare | 3 | — | — |
| Passerella (SCENE7A) | guerra | veterano | 4 | — | — |
| Passerella (SCENE7B) | guerra | veterano | 4 | — | — |
| Passerella (SCENE7C) | guerra | veterano | 4 | — | — |
| Festa a sorpresa (SCENE8A) | guerra | regolare | 4 | — | sì |
| Festa a sorpresa (SCENE8B) | guerra | regolare | 4 | — | sì |
| Festa a sorpresa (SCENE8C) | guerra | regolare | 4 | — | sì |
| Ritorno a casa (SCENE9A) | guerra | elite | 5 | — | — |
| Ritorno a casa (SCENE9B) | guerra | elite | 5 | — | — |
| Ritorno a casa (SCENE9C) | guerra | elite | 5 | — | — |
<!-- fine: carriera_originali -->

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Gradi, incarichi, fasi delle due marine | dossier 13 §2 | alta (gradi), media (fasi compresse) |
| Commissioni, «up or out», 20 anni | dossier 13 §2.3 | alta (regole USN), media (VMF) |
| Decorazioni, precedenze, nastrini | dossier 13 §3 (per decorazione, appendice G11) | per decorazione |
| Soglie di punteggio delle decorazioni | dossier 13 §3.7 | **STIMA DI GIOCO** |
| Motivazioni | dossier 13 §5.1-5.2 | media (modelli) |
| Pesi del punteggio e dell'esposizione | scelta di gioco, dossier 13 §5 | **STIMA** |
| Acque territoriali, incidente diplomatico | UNCLOS art. 20; dossier 22; dossier 23 E14 | alta (regola), **STIMA** (pesi) |
| Situazione e avversario delle originali | lettura delle 22 missioni | media |

## 4. Classico ed Enhanced

Nel classico c'è il rapporto d'esito dell'originale, senza punteggio né carriera. La carriera vale con la regola di
simulazione dell'Enhanced.

## 5. Come si vede in gioco

- **Debriefing** (capitolo F1): il punteggio per voce, il giudizio, la decorazione con la motivazione, la cerimonia.
- **Fascicolo e albo dei comandanti**: grado, decorazioni con la barretta, note, provvedimenti (volume 1, cap. 10).
- **Briefing**: situazione, avversario e difficoltà della missione.
- **Albo d'onore** (capitolo F3): le carriere chiuse migliori.

## 6. Taratura e prove

Prove in `test/prove_carriera.cpp` (`carriera:`):
- «punteggio di missione»;
- «dall'Ensign alla commissione, con le dolphins»;
- «carriera breve, decorazioni in guerra, ripetizioni e rendimenti»;
- «pace, violazioni e soglie»;
- «acque territoriali straniere in pace, l'incidente se ci scoprono dentro (S-363/U-137, 1981)»;
- «rimozione, inchiesta, indulgenza e morte»;
- «commissioni, «up or out» e trasferimento»;
- «VMF, classi del Servizio alla Patria e Stella Rossa»;
- «quali missioni contano»;
- «fascicolo salvato e riletto»;
- «riassunto e contesto da una missione originale» e «fuoco senza provocazione nell'esito di una missione originale
  (SCENE2A)».

## 7. Limiti noti e in arrivo

- Le fasi sono compresse: una carriera vera dura vent'anni, qui qualche decina di missioni.
- Le soglie delle decorazioni sono di gioco: le decorazioni vere non dipendono da un punteggio.
- I pari grado della graduatoria sono simulati, non altri giocatori.

## 8. Riferimenti

- Codice: `nucleo/carriera.hpp`, `nucleo/src/carriera.cpp`, `nucleo/fascicolo.hpp`, `missioni/carriera_missione.hpp`.
- Dati: `data/motore/carriera.toml` (da `tools/pipeline/carriera.py`).
- Ricerca: dossier 13 §2, §3, §5; dossier 22; dossier 23 E14.
