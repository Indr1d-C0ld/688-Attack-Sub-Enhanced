# 11 — Comandi e stazioni del giocatore (progetto)

Stato: **progetto** (24/10/2026). Sviluppa i punti 8 (stazioni di bordo), 20 (schiera), 21 (testi leggibili),
25 (armi e contromisure), 26 (navigazione) e 30 (posarsi sul fondo) della roadmap. Viene prima della Fase 3, in
cui le stazioni si disegnano per intero.

**Fonti:**
- manuale del 1989 (`docs/originale/manuale_EN.txt`, righe citate come [r. N]);
- layout delle stazioni originali (`re/cls/*.CLS_it.png`, `tools/re/cls_layout.py`);
- dispatcher dei comandi e gestori degli script (`docs/originale/sensori_e_navigazione.md` §7, `docs/09` §6.1);
- regola d'impatto e danni (§9.4 e §9.8 dello stesso documento);
- dati reali dai dossier `docs/ricerca/01` e `02` (§5, §6).

## 0. Principi

- **Un solo strato di ordini.** Tastiera, mouse e pannelli producono *ordini*; il nucleo li applica al passo. Un
  ordine ha l'istante di simulazione e i suoi parametri.
  - **Registrazione:** la stessa sequenza di ordini dallo stesso seme ridà la stessa partita, quindi gli ordini si
    registrano. Servono al replay, al debriefing e alle statistiche (punto 16).
- **Due livelli, la stessa interfaccia.**
  - **Livello classico:** ogni ordine diventa il codice del dispatcher dell'originale e il suo gestore negli
    script della missione, con i parametri dell'originale. Le missioni restano identiche.
  - **Enhanced:** gli ordini vanno a un modello realistico. Sono previsti il ritardo dell'equipaggio, i limiti
    veri (velocità per alzare il periscopio, filatura della schiera, cavitazione) e le impostazioni reali dei
    siluri.
- **L'equipaggio risponde.** Ogni ordine ha la sua conferma, come «Timone 15 a dritta, Comandante»; ogni
  impedimento ha il suo motivo, come «Troppo veloci per alzare il periscopio». Tutto passa dalla coda dei
  messaggi con priorità, sempre leggibile per intero (punto 21).
- **Interfaccia e tasti:**
  - stazioni disegnate native a 640×480;
  - tutto si fa con la tastiera; i tasti dell'originale restano dove non confliggono (§5);
  - l'Alfa ha le stesse funzioni con la sua interfaccia e il font «sovietico» (punto 12).

## 1. Come funzionava nell'originale

### 1.1 Stazioni [r. 1296–2466]

| Stazione (tasto) | Contenuto | Note |
|---|---|---|
| CONN (F9) | menu delle stazioni | il gioco è **in pausa** [r. 447] |
| Radio (F2) | periscopio e antenna su/giù, rilettura dei messaggi (PREV/NEXT), TRANSMIT («MISSION» rilegge gli ordini), codice d'accesso | niente mappa |
| Stato (F3) | sistemi «OK» o «% operativo» (motori al 75% = velocità massima ai 3/4), diagramma dei danni | danni senza riparazione |
| Manovra (F4) | indicatore di cavitazione, barra di immersione proporzionale, 150 FT, QUOTA PERISCOPICA, EMERSIONE D'EMERGENZA, telegrafo (INDIETRO … MAX), timone incrementale, autopilota | il timone disattiva l'autopilota |
| Periscopio (F5) | livello ESM, su/giù, rotazione < >, antenna, TARGET, LANCIO | solo a 20 ft |
| Plottaggio (F6) | tutte le tracce, traccia del bersaglio, proiezione a N minuti, fino a 9 punti di rotta (SET/CLEAR), compressione del tempo, autopilota, TARGET | l'autopilota rallenta per virare; da FLANK torna solo a FULL |
| Armi (F7) | 4 tubi più 2 ausiliari per le esche (Alfa: 6 tubi, il 6° per le esche), missili, armamento in due tempi (selezione, poi ciclo), TARGET, LANCIO; controllo siluri SELEZ., GUIDA (DIRECT), CERCA, DETONATE | filo di 7,5 nm |
| Sonar (F8) | ANALIZZA (firma), FILTRO (passa banda o passa alto: solo i siluri), impulso attivo, schiera (cala, recupera, taglia), TARGET | la schiera limita la velocità: 2/3 sul 688, 50% sull'Alfa [r. 2275] |

**Elementi comuni a tutte le stazioni:**
- barra del bersaglio: classe, rilevamento relativo, distanza, rotta, velocità, quota;
- barra dei messaggi che scorrono (SPAZIO li accelera);
- mappa dall'alto con le «X» (solo direzione) e le lettere (posizione nota);
- quadro del fondale (contour imaging, solo 688).

### 1.2 Numeri e regole del manuale

- **Quote:** periscopica 20 ft, automatica 150 ft, massima 1500 ft (688) o 1800 (Alfa).
- **Velocità:**
  - massima 37 kn (688) o 42 kn (Alfa);
  - **STD, FULL e FLANK vietati sopra i 100 ft** [r. 2140];
  - REV molto rumoroso.
- **Siluri:**
  - portata 10 nm, filo di 7,5 nm, circa 10 s fra un lancio e l'altro;
  - nessuna impostazione prima del lancio: tutto si fa dopo, via filo (DIRECT, SEARCH, DETONATE).
- **Missili:** Harpoon e Tomahawk solo a quota periscopica e con identificazione positiva.
- **Sonar attivo:** rivela a quasi tutti ed è un atto ostile.
- **Non dati dal manuale, ricavati dall'eseguibile** (24/10/2026, `docs/originale/sensori_e_navigazione.md` §7,
  §9.6 e §10):
  - **scatti:** timone a 11, barra a 9, telegrafo a 7 posizioni (§7);
  - **armamento dei tubi:** in due tempi (selezione, poi armamento); dopo un lancio il tubo si ricarica da solo
    finché ci sono scorte. Armamento e ricarica durano 20 s (`AT +10` nel gestore `carica_tubo_n`), uguali in
    tutte le 22 scene e per ogni arma;
  - **nessun intervallo minimo fra i lanci:** i «circa 10 s» del manuale non sono nel codice;
  - **siluri in acqua al massimo:** 4 per il 688 e 6 per l'Alfa, esche comprese;
  - **missili dal tubo 7:** bersaglio con traccia solida, almeno 5 nm, quota periscopica; l'Harpoon solo contro
    le navi;
  - **periscopio e antenna:** si alzano solo a quota ≤ 20 ft e sotto si abbassano da soli; non c'è un limite di
    velocità per alzarli. Sopra i 100 ft la velocità massima è metà di avanti tutta, un quarto col periscopio su;
    l'antenna non conta;
  - **schiera:** si fila solo sotto i 3/4 di avanti tutta; con la schiera fuori, 3/4, TUTTA, MAX e l'arresto
    rapido chiedono conferma e la tagliano; si fila in 60 s e si recupera in 40 s;
  - **filo:** si taglia a tempo, a 190 s per il Mk 48 e a 170 s per il Type 53;
  - **quota di schiacciamento:** non esiste; la quota massima limita solo la quota ordinata.

### 1.3 Dispatcher e gestori (`docs/originale/sensori_e_navigazione.md` §7)

I comandi del giocatore sono codici (01h–43h) che il motore originale applica in parte da sé e in parte facendo
girare un gestore negli script della missione. Il gestore riceve i parametri scritti negli argomenti della sua
prima istruzione:

| Comando | Parametri |
|---|---|
| telegrafo | 01h–07h: arresto rapido, 0, 1/4, 1/2, 3/4, 1, 1,09 |
| timone | rateo di virata da −3,6 a +3,6 °/s in 9 scatti |
| quota | profondità e rateo in 9 scatti; quota periscopica, 150 ft, emersione rapida |
| periscopio, antenna, schiera | — |
| caricamento dei 7 tubi | — |
| lancio | per tipo d'arma |
| guida a filo | `guida_e0` = DETONATE, `guida_e1` = SEARCH, `guida_e2` = DIRECT con X, Y |
| punti di rotta, autopilota, bersaglio | — |

**Cosa fanno davvero gli script** (SCENE1A):
- **Caricamento di un tubo:** un timer di 20 s (`carica_tubo_n` → `dopo(u, 20, …)`).
- **Lancio del Mk 48:** rumore transitorio; il siluro parte con fine corsa a 280 s, avviso a 170 s e taglio del
  filo a 190 s.
- **DETONATE:** toglie il siluro con il messaggio 19 «Esplosione subacquea» e **nessun danno**. Il manuale dice
  invece che «danneggia anche senza impatto».
  - **Era una licenza del gioco.** Nella realtà i siluri convenzionali del 1989 non si potevano far esplodere a
    comando (dossier 07 §10, fiducia media):
    - Mk 48 e ADCAP avevano spolette di prossimità o a contatto; via filo si potevano guidare, cambiare quota e
      velocità, abilitare la ricerca e **spegnere**;
    - nessuna fonte pubblica cita la detonazione per Tigerfish, Spearfish, TEST-71 e gli altri;
    - l'unico caso noto è il Mk 45 ASTOR nucleare, ritirato a metà anni '70.
  - Il documento 10 (§4, decisione 4) è stato corretto di conseguenza.
- **SEARCH:** marca i contatti e riprende la ricerca. **DIRECT:** naviga verso il punto.

## 2. Architettura

- **`Ordine`** (nucleo): istante di simulazione, tipo, parametri. Una coda nel motore li applica all'inizio del
  passo successivo e li registra (punto 16).
- **Livello classico** (`missioni/`): `Missione::comando(codice, parametri)` riproduce il dispatcher.
  - **Gestore della missione:** esegue il gestore di `gestori_giocatore` con i parametri. Il convertitore
    (`tools/re/missioni_lua.py`) deve generare gestori parametrici, per esempio
    `script.L_0520 = function(u, p1) m688.telegrafo(u, p1 or 0) … end`, perché oggi scrive i valori di default.
  - **Parte del motore:** fa ciò che l'originale fa da sé, cioè il ping attivo (2Dh, evento acustico), il lancio
    per tipo d'arma, la schiera persa per velocità (36h) e i limiti di quota e velocità.
  - **Verifica nell'eseguibile:** ciò che il manuale non dice e serve al livello classico (scatti, tempi, limiti
    del §1.2), con `tools/re/disasm16.py` come per la regola d'impatto.
- **Enhanced:** gli ordini vanno al modello dinamico del sottomarino della Fase 2 (spinta, resistenza, timoni,
  assetto, cavitazione). Finché quel modello non c'è, si usa il moto del livello classico con i limiti reali del
  §3.
- **Contatti del giocatore:** la mappa, le «X» e le lettere vengono dai contatti dell'unità del giocatore nella
  missione (qualità, durata, posizione stimata), non dalla verità.

## 3. Catalogo degli ordini

Per ogni ordine: che cosa fa nel livello classico e che cosa nell'Enhanced. I numeri reali vengono dai dossier;
quelli segnati «stima» sono da confermare.

### 3.1 Manovra

| Ordine | Classico | Enhanced |
|---|---|---|
| **Timone** | 9 scatti di rateo, da −3,6 a +3,6 °/s; disattiva l'autopilota | angolo di barra da 0 a 35° (scatti da 5°) oppure «accostare per 270»; virata dalla dinamica: diametro tattico 4–6 lunghezze a 15–20 kn (stima, dossier 01) |
| **Telegrafo** | 7 posizioni (arresto rapido … massima) | Indietro tutta, 2/3, 1/3 · Stop · Avanti 1/3, 2/3, Standard, Tutta, Massima; oppure giri o nodi richiesti; accelerazione dalla dinamica |
| **Quota** | barra di immersione in 9 scatti, 150 ft, quota periscopica, emersione d'emergenza | quota ordinata o angolo di immersione (±5–30°); assetto; emersione d'emergenza = esaurimento rapido delle casse (rumore, salita incontrollabile) |
| **Silenzio** | — | «assetto silenzioso»: macchinari ausiliari al minimo, niente transitori, velocità silenziosa (5 kn sul 688, dossier 01) |
| **Posarsi sul fondo** | — | §6 |

**Vincoli:**
- **Livello classico:** valgono quelli dell'originale. STD, FULL e FLANK sono vietati sopra i 100 ft.
- **Enhanced:**
  - cavitazione `V_inc(z) = V_inc0 · sqrt((10,33 + z)/10,33)` (dossier 02 §2.3);
  - quota di collaudo 290 m e di collasso 450 m sul 688 (dossier 01);
  - danni sotto il collaudo come nel dossier 02 §9.

### 3.2 Sensori

| Ordine | Classico | Enhanced |
|---|---|---|
| **Periscopio** su/giù, rotazione, ingrandimento | solo a 20 ft; flag `+54` 02h | quota di 18–20 m; sopra ~8 kn vibrazioni e piuma visibile (stima); stadimetria (distanza dall'altezza dell'albero); esposizione al radar che cresce col tempo |
| **Antenna** | solo a 20 ft; flag 04h | come il periscopio; più esposta (§3.4 del documento sull'originale: +2,25 nm) |
| **ESM** | livello di segnale | ricevitore d'allarme (punto 23) |
| **Sonar attivo** | evento acustico di 33 s, ping che illumina il settore di prua | un impulso alla volta («one ping only»), con contro-scoperta 1,5–3 volte la portata (dossier 02 §5.3) |
| **Filtro** | passa banda o passa alto | bande, LOFAR e DEMON (punto 24) |
| **Schiera** cala/recupera/taglia | filata in 60 s, recuperata in 40, persa oltre il limite di velocità | specifica `docs/07_towed_array.md` (velocità di traino, curvatura dopo le virate, fondale, reti) |
| **Designa bersaglio** | TARGET: contatto corrente (`designa_bersaglio`) | come l'originale, più la soluzione TMA associata |

### 3.3 Armi

| Ordine | Classico | Enhanced |
|---|---|---|
| **Carica tubo** n | timer di 20 s dello script, poi «pronto» | ricarica realistica (tempo da stimare; i 688 caricano a mano con il sistema di movimentazione) con transitori udibili: allagamento, equalizzazione, apertura della porta esterna (dossier 02 §6.2) |
| **Impostazioni** | nessuna | modo (attivo, passivo, misto), velocità (bassa o alta), RTE (distanza di attivazione), soffitto e pavimento, quota di ricerca, schema, protezione anti-circolare |
| **Lancio** | gestore dell'arma (`armi.<tipo>.gestore`); nessun intervallo minimo fra due lanci (i «circa 10 s» del manuale non sono nel codice) | espulsione con transitorio; il filo vincola velocità e virate del battello |
| **Guida a filo** | SELEZ., poi DIRECT (punto), CERCA, DETONATE | rotta e quota, cambio di bersaglio, rifiuto di un'esca, spegnimento, taglio del filo; dati della testa che tornano lungo il filo |
| **Detonazione** | DETONATE come nell'originale: toglie il siluro, messaggio 19, nessun danno | **eliminata** (non storica, decisa il 24/10/2026): al suo posto lo **spegnimento** via filo, il siluro si ferma e affonda senza esplodere |
| **Esche** | tubi ausiliari (688) o 6° tubo (Alfa) | ADC Mk2 sul 688 (dossier 05), con la sua vita e la sua efficacia |
| **Missili** | Harpoon e Tomahawk da 20 ft con identificazione | capsula dell'Harpoon, Tomahawk; dati di tiro (punto 25) |

### 3.4 Navigazione e comando

- **Punti di rotta:** fino a 9 nel classico, percorsi in ordine. L'autopilota non controlla la quota né il
  fondale e si disattiva toccando il timone.
- **Proiezione:** di bersaglio e battello a N minuti; il siluro si può dirigere sul punto previsto.
- **TMA** (Enhanced): strumento MATE (distanza, rotta e velocità regolate a mano finché i rilevamenti previsti
  coincidono con quelli misurati). Aiuti: regola del 1934 ed Ekelund (dossier 02 §5.1).
- **Compressione del tempo:** si interrompe da sola a ogni evento importante (contatto nuovo, siluro in acqua,
  messaggio prioritario).
- **Pausa:** il classico sospende il gioco nel CONN come l'originale; l'Enhanced ha una pausa esplicita (tasto P) e
  il CONN non ferma il tempo (decisione 1 del §9).
- **Radio:** rilettura dei messaggi; trasmissione e ricezione con le regole del punto 22.

## 4. Stazioni

**Ordine di sviluppo:**
- **Fase 1 (C1–C4):** pannelli funzionali minimi.
- **Fase 3:** stazioni complete, nello stile originale ridisegnato a 640×480.

| Stazione | Fase 1 (minimo) | Fase 3 (completa) |
|---|---|---|
| **Centrale** | menu delle stazioni, pausa nel classico | plancia di comando con lo stato rapido (quota, rotta, velocità, contatti, armi in acqua) |
| **Manovra** | timone, telegrafo, barra o quota, automatismi, autopilota, cavitazione | indicatori della dinamica (assetto, angolo di barra, giri, casse) |
| **Periscopio** | quello di oggi, più su/giù, antenna, ESM, TARGET, LANCIO | stadimetria, notte, ESM nuovo |
| **Plottaggio** | mappa dall'alto con i contatti del giocatore, punti di rotta, proiezione, TARGET | quadro «carta + misure» (`docs/08`), TMA |
| **Armi** | tubi, armamento, lancio, controllo siluri | impostazioni reali, filo, esche e missili |
| **Sonar** | contatti, filtro, ping, schiera, ANALIZZA semplificato | waterfall broadband e narrowband, LOFAR/DEMON, cuffia (punti 19, 24, 27) |
| **Stato e danni** | sistemi e percentuali (§9.4 dell'originale) | controllo danni del nostro battello (fase D6) |
| **Radio/ESM** | messaggi, periscopio, antenna | sala radio del punto 22 |

**Barre comuni a tutte le stazioni:**
- **Barra del bersaglio:** classe, rilevamento, distanza, rotta, velocità, quota, con «sconosciuto» finché il
  contatto non è identificato.
- **Barra dei messaggi:** coda con priorità, rilettura, alternativa su due righe.
- **Barra in basso:** posizione e tempo di missione.

## 5. Tastiera e mouse

- **Tasti dell'originale** [r. 3274–3366]:
  - F2–F9 per le stazioni, F1 per l'aiuto;
  - Tab per i gruppi, frecce per i controlli, Invio per eseguire;
  - le lettere di ogni stazione: U periscopio, N antenna, L lancio, G bersaglio, W/K punti di rotta, `-`/`=`
    rotazione del periscopio, «.» autopilota.
- **Conflitti da risolvere:** O vale sia «esci» sia «zoom indietro»; F10 era il «tasto del capo», che non serve.
- **Ordini rapidi validi in ogni stazione** (nuovi):
  - tastierino: 4/6 timone a sinistra/dritta di uno scatto, 5 barra al centro, 8/2 su/giù di uno scatto di quota;
  - `+`/`-` telegrafo avanti/indietro di una posizione;
  - Ctrl+P quota periscopica, Ctrl+E emersione d'emergenza.
- **Mouse:** aree attive come nell'originale. La rotella regola i comandi continui: rotazione del periscopio,
  angolo di barra, quota ordinata.
- **Configurazione:** i tasti si possono rimappare. Nel menu c'è la tabella completa in italiano.

## 6. Posarsi sul fondo (punto 30)

**Procedura (Enhanced):**
1. Schiera recuperata.
2. Velocità sotto 2 kn.
3. Quota ordinata pari al fondo meno un margine.
4. Discesa lenta: il motore ha già il contatto dolce, sotto 1 m/s nessun danno.
5. Macchine ferme, assetto silenzioso.

L'equipaggio rifiuta l'ordine (e dice perché) con la schiera filata, su un pendio ripido o oltre la quota di
collaudo.

**Effetti:**
- **Sonar attivo nemico:** l'eco si confonde con il riverbero del fondo; peggio su roccia, meglio su fango. Un
  bersaglio fermo non ha effetto Doppler.
- **Rumore irradiato:** quello dell'assetto silenzioso, senza elica.
- **MAD:** continua a rilevare il battello.

**Rischi:**
- fango che trattiene lo scafo: il distacco costa tempo e rumore;
- rocce: danni alla cupola del sonar e ai timoni inferiori;
- prese a mare che aspirano sedimento: guasti dopo qualche tempo, per questo i nucleari lo evitano.

**Dati necessari:**
- **Tipo di fondo:** raster della Fase 0 (EMODnet Geology, Dutkiewicz), non ancora fatto. Fino ad allora si usa
  «sabbia», con la pendenza calcolata dalla heightmap.
- **Livello classico:** l'ordine non c'è, come nell'originale. Toccare il fondo resta quello dell'originale
  (§9.3–9.4, messaggi 30 e 9).

## 7. Danni ai sistemi del giocatore

- **Livello classico** (§9.4 dell'originale):
  - con i messaggi 20, 26 e 30, la metà delle volte si guasta un sistema a caso fra sei: calcolatore del sonar,
    schiera, periscopio, sonar di tiro, motori, antenna;
  - la stazione Stato lo mostra;
  - nessuna riparazione.
- **Enhanced:** fase D6 (`docs/10` §10, progetto del 24/10/2026; idea iniziale nel §4): shock dal fattore d'urto, guasti graduali, falle, squadre del controllo
  danni.

## 8. Piano di lavoro

Ogni fase si chiude con prove automatiche e istantanee.

| Fase | Contenuto | Verifica |
|---|---|---|
| C1 | `Ordine`, coda e registrazione; `Missione::comando` con il dispatcher; gestori parametrici nel convertitore e missioni rigenerate | stessa partita da stessi ordini e seme (impronta); ogni gestore riceve i parametri del §7 dell'originale |
| C2 | Manovra nel livello classico: telegrafo, timone, barra, automatismi, punti di rotta, autopilota, limiti del §1.2; pannello Manovra minimo e ordini rapidi | prove per ogni codice; autopilota che raggiunge i punti; nessuna velocità alta sopra i 100 ft |
| C3 | Armi nel livello classico: armamento dei tubi, lancio per tipo, guida (SELEZ., DIRECT, CERCA, DETONATE), esche, missili; pannello Armi minimo (**fatta il 24/10/2026**) | un Mk 48 lanciato dal giocatore colpisce un bersaglio di prova con la regola §9.8; fine corsa, taglio del filo e detonazione come negli script |
| C4 | Sensori del giocatore: periscopio, antenna, ESM, ping, filtro, schiera, TARGET; mappa dall'alto con «X» e lettere dai contatti del giocatore; barra del bersaglio (**fatta il 24/10/2026**) | scoperta reciproca coerente con il modello (§3 dell'originale); schiera persa oltre la velocità |
| C5 | Verifiche nell'eseguibile per i numeri mancanti del §1.2 (**fatte il 24/10/2026**: §7, §9.6 e §10 del documento sull'originale) | numeri documentati nel documento sull'originale |
| C6 | Enhanced: modello dinamico (Fase 2), impostazioni reali dei siluri, comandi via filo reali con lo spegnimento (niente detonazione), TMA (MATE), posarsi sul fondo | prove sulla dinamica (virata, quota, cavitazione) e sui casi del dossier |

Le fasi C1–C4 rendono giocabili le 22 missioni originali. Le stazioni complete restano nella Fase 3.

**Fase C1 fatta** (24/10/2026):
- **Ordini:** coda e storico nel nucleo (`nucleo/ordini.hpp`, prove `ordini:`).
  - Consegna per istante e per ordine di arrivo.
  - Un ordine in ritardo vale al passo in cui arriva.
  - Lo storico, salvato come testo, ridà le stesse consegne.
  - Il carico della Sala siluri (M2) fa parte dello storico (`RegistroOrdini::imposta_carico` e `carico()`): va in
    testa al testo su righe `carico posizioni|tipi N v1 ... vN` (con `-` per un posto vuoto) e
    `carico rastrelliera|pozzi|esche arma=numero ...`. I file senza queste righe restano validi e non hanno carico.
  - **Replay fino a un evento** del registro M1 (minicartoline, test/prove_replay.cpp):
    - servono lo stesso seme, lo stesso storico con il carico e lo stesso passo fisso dall'inizio;
    - si avanza finche' `registro().evento(id)` c'e', il che succede al piu' entro `passi_per_evento(e, passo)`
      passi; gli id e le pose sono uguali;
    - la sala radio va creata nello stesso momento nella partita giocata e nel replay, cioe' prima del primo passo
      come col briefing, oppure in nessuno dei due.
- **Dispatcher** (`Missione::comando`): esegue il gestore della missione con i parametri.
- **Traduzione degli ordini** (`Missione::applica`): telegrafo, timone, quote (anche periscopica, 150 ft ed
  emersione), punti di rotta (al massimo 9) con il pilota automatico che il timone disinserisce, periscopio,
  antenna, schiera, sonar attivo, bersaglio, caricamento dei tubi, lancio per tipo d'arma, guida a filo (SELEZ.,
  DIRECT, CERCA, DETONATE). Gli ordini dell'Enhanced non si applicano al livello classico.
- **Missioni:** gestori parametrici (`docs/09` §6.1); le 22 missioni rigenerate danno le stesse partite di prima
  senza ordini.
- **Eseguibile:** `--ordini FILE` ripete gli ordini salvati, `--registra FILE` li salva all'uscita. Due replay
  dello stesso file danno la stessa impronta.
- **Prove `comandi:`** su SCENE1A: comandi di manovra, pilota automatico, lancio e DETONATE, replay.
- **Difetto del runtime trovato con le prove e corretto:** `LAUNCH` (`2011:18B7`) non crea l'unita'. Prepara il
  modello e copia nel siluro il contatto del bersaglio a traccia solida; l'unita' la crea `nuova_unita`. Prima ogni
  lancio, anche dell'IA, produceva due siluri senza traccia.
  - **Effetto sulle missioni:** in SCENE1B (TORPEX '89) il 688 avversario ora affonda davvero le navi bersaglio
    nella gara con il giocatore, che lo script misura con le variabili 10 e 17.
  - **Esiti:** cambiano rispetto a prima in alcune missioni; `verifica_fondale` resta senza urti.
- **Prossima:** C2, la manovra con il pannello minimo e gli ordini rapidi da tastiera.

**Fase C2, prima parte fatta** (24/10/2026):
- **Posto di comando** (`nucleo/comando.hpp`, prove `posto di comando:`): le posizioni dell'originale.
  - Timone a 11 scatti da 0,72 °/s, che disinserisce il pilota automatico.
  - Barra di immersione a 9 scatti: |scatto|/4 del rateo verso la superficie o la quota massima; a zero tiene
    la quota.
  - Telegrafo a 7 posizioni: INDIETRO, STOP, 1/3, 2/3, STD, TUTTA, MAX.
  - Rifiuti dell'equipaggio, con i messaggi 210–212: STD e oltre sopra i 100 ft, periscopio e antenna fuori
    quota periscopica, pilota automatico senza punti di rotta.
  - Le posizioni seguono anche gli ordini di un replay.
- **Tasti:** ordini rapidi del §5 nella finestra (tastierino, `+`/`-`, Ctrl+P, Ctrl+1, Ctrl+E, `.`, `U`, `N`); la
  compressione del tempo passa a PagSu/PagGiù.
- **Pannello Manovra** in missione al posto di quello della scena di prova: telegrafo, timone, barra, pilota e
  punti, rotta, velocità e quota in piedi, ultimo messaggio o aiuto dei tasti.
- **Seconda parte fatta** (24/10/2026), nel runtime delle missioni, con le regole ricavate dall'eseguibile
  (`docs/originale/sensori_e_navigazione.md` §5 e §9.6):
  - **controlli dei giocatori a ogni visita:**
    - messaggio 31 sugli strati termici;
    - sopra i 100 ft velocità al massimo metà di avanti tutta, un quarto col periscopio su (messaggio 45);
    - periscopio e antenna giù sotto i 20 ft (messaggi 3 e 37);
  - **accostata rallentata** per tutte le unità che navigano verso un punto (anche l'IA e le armi): un quarto di
    avanti tutta oltre 3°, poi la velocità di prima, al più TUTTA (messaggio 47);
  - **`DEPTH_TO` fedele:** rateo 0 tiene la quota, e la barra a zero invia i valori del codice 17h.
  - **Effetto sulle missioni:** il rilievo cambia di poco; `verifica_fondale` resta senza urti.
- **Note sulla seconda parte, scritte prima di farla:**
  - il pilota automatico che rallenta per accostare e da MAX torna solo a TUTTA (manuale, r. 2012–2020);
  - **`DEPTH_TO` con rateo 0:** nell'originale lascia il battello alla sua quota (`2011:175D`, rateo in 1/256 di
    `+38`), mentre il runtime lo tratta come rateo pieno, con un minimo di 0,5 m/s nel pilota. Riguarda solo il
    gestore del giocatore: oggi la barra a zero ordina la quota attuale, con lo stesso effetto.

**Fase C3 fatta** (24/10/2026), con le regole del §10 di `docs/originale/sensori_e_navigazione.md` e del §1 di
`docs/originale/numeri_eseguibile.md`:
- **Stazione Armi** (`nucleo/armi_giocatore.hpp`, prove `armi:`): come nell'originale sta fuori dagli script.
  - **All'avvio** i tubi sono vuoti e le scorte sono la `dotazione` intera della missione.
  - **Armamento in due tempi:** il primo clic seleziona il tubo, il secondo arma un tubo vuoto (una scorta in meno
    e il gestore `carica_tubo_n`, 20 s) o libera un tubo in armamento o armato (la scorta torna).
  - **Messaggi 10–16 degli script:** con il tubo in armamento diventano 10 + tipo d'arma; altrimenti si sopprimono.
  - **Lancio:** i controlli nell'ordine dell'originale; i missili del tubo 7 vogliono bersaglio, contatto a traccia
    solida, almeno 5 nm e quota periscopica (l'Harpoon solo contro le navi). Poi 38h con il bersaglio, il gestore di
    lancio e la ricarica automatica. Nessun intervallo minimo, ma al massimo 4 armi in acqua (6 sull'Alfa), esche
    comprese; il posto si libera quando l'arma esce dal gioco.
  - **Tubo selezionato:** all'avvio il tubo 1 (`DS:02E5` = 0 nell'immagine, quindi il primo R lo arma subito);
    poi la regola di `7AF1` dopo il lancio e dopo l'armamento.
  - **Guida a filo:** CERCA, DIRECT e DETONATE solo per i propri Mk 48 e Type 53 ancora collegati.
- **Collegamento alla missione** (`missioni/stazioni_missione.hpp`, che dalla C4 tiene anche il Sonar): legge bersaglio, contatto, distanza vera e arma
  guidata, applica i comandi, segue i messaggi di armamento e le armi in acqua. Lo stato della stazione dipende
  solo dagli ordini consegnati e dai messaggi della missione: un replay rifà la stessa partita.
- **Messaggi:** le frasi dell'eseguibile sui tubi, sul lancio, sulla guida e sul TARGET, codici 220–239 di
  `messaggi_bordo.tsv`.
- **Pannello Armi** (F7; F4 torna alla Manovra; `--stazione 7` senza finestra): i tubi nei colori dell'originale,
  scorte, armi in acqua, bersaglio e siluro scelto. Tasti: 1–7 e R, A, M per i tubi, L lancio, G bersaglio, S
  siluro, C ricerca, D DETONATE.
- **Verifiche:**
  - un Mk 48 armato in 20 s colpisce una nave a 2 km con la regola del §9.8;
  - a 170 s arriva l'avviso, a 190 s il filo si taglia, a 280 s il siluro finisce la corsa;
  - dopo il taglio, CERCA è rifiutato.
- **Da notare:** un siluro che cerca dopo il taglio del filo non distingue più il lanciatore e può tornare sul
  proprio battello (messaggi 67 e 26). È fedele: la collisione (`2011:0CC9–0CD1`) guarda solo `+0C`, che `LINK 0`
  azzera, e non `+62` (§9.8 punto 6 del documento sull'originale). Nell'Enhanced lo eviterà la protezione
  anti-ritorno delle impostazioni del siluro (§3.3).
- **Restavano alla C4, fatti il 26/10/2026 (principale):**
  - DIRECT dalla carta: Ctrl+I (comando `guida_punto`) col siluro filoguidato scelto apre il cursore della carta sul
    siluro; frecce o clic, Invio conferma. Nel classico è l'ordine `guida_punto` col punto (`guida_e2`), come
    l'originale; nell'Enhanced la rotta via filo verso il punto;
  - il TARGET dell'originale (`1000:802A`, `numeri_eseguibile.md` §2): al periscopio, al sonar, alla carta e alle
    armi mette in pausa e fa scegliere, con la lettera o con il mouse, fra i segni dell'ultima vista (X comprese);
    oggi G prende il contatto più vicino al rilevamento del periscopio, e S passa al siluro filoguidato successivo.

**Fase C4 fatta** (24/10/2026), con le regole del §10 di `docs/originale/sensori_e_navigazione.md` e dei §3–§5 di
`docs/originale/numeri_eseguibile.md`:
- **Stazione Sonar** (`nucleo/sonar_giocatore.hpp`, prove `sonar:`), nel programma come le Armi:
  - **schiera:** si fila solo sotto i 3/4 di avanti tutta; durante le manovre «ancora in filatura/recupero»; con la
    schiera fuori o in manovra 3/4, TUTTA, MAX e l'arresto rapido chiedono di tagliarla (anche dal tastierino, per
    fedeltà); il tempo resta fermo durante la domanda e Invio la taglia (36h). Ai contatti conta solo il bit dello
    script: dopo 60 s in filatura, fino a fine recupero;
  - **FILTRO** passa alto o passa banda (solo le armi sulla carta e nella barra); **impulso attivo** senza limiti;
    **ANALIZZA** con bersaglio, contatto qualsiasi (anche una X) e niente aerei. La firma sonora è del suono.
- **Guasti dei sistemi** (`nucleo/guasti_giocatore.hpp`, prove `guasti:`): con i messaggi 20, 26 e 30, una volta
  su due, uno dei sei sistemi a caso con il `rand()` di Borland C; se è già guasto non succede nulla. Periscopio e
  antenna si abbassano e poi sono rifiutati; filtro e ANALIZZA, impulso attivo, schiera, MAX e arresto rapido
  rifiutati con i messaggi dell'originale (60–66). Nessuna riparazione.
- **Contatti del giocatore** (`missioni/contatti_giocatore.hpp`, prove `contatti:`): la regola di `990C`, cioè la
  lista del giocatore senza orfani e senza le tracce deboli spente dal lampeggio, gli indicatori e i colori della
  carta. **Barra del bersaglio** (`8150`): titolo dalla durata (nome della classe dell'originale, tradotto), sempre
  rilevamento relativo e rotta vera; distanza, velocità e quota vere con la traccia solida entro 20 nm, con il
  rumore dell'originale e gli ultimi valori che restano senza traccia; il bersaglio sparito si deseleziona.
- **Carta** (F6, `app/carta.hpp`; stato in `nucleo/carta_giocatore.hpp`): centrata sul battello, scala S da 2 a 52
  con I e O come l'originale, fondale del teatro (B: sfondo uniforme), X a 25 pixel originali e lettere, punti di
  rotta 1–9, proiezione a N minuti (J, `[` e `]`) del battello e del bersaglio. Un pixel originale sono due della
  tela: la scala è la stessa, l'area è più grande.
  - **SET (W):** ferma il tempo, cursore con le frecce, Invio aggiunge il punto in fondo (il primo accende il
    pilota), Esc annulla. **CLEAR (K)** toglie l'ultimo.
- **TARGET (G):** ferma il tempo e fa scegliere fra i segni dell'ultima vista (la carta o il campo del periscopio):
  una lettera prende l'ultima voce con quella lettera, Tab e Maiuscolo+Tab scorrono tutte le zone, X comprese
  (`numeri_eseguibile.md` §6); Invio conferma, Esc toglie il bersaglio. Con una X si ha un bersaglio senza traccia
  solida: niente distanza né missili.
- **Al periscopio:** X e lettere dei contatti nel campo, sotto la scala.
- **Quadro di stato** (F3): i sei sistemi e «sistemi di bordo al n%» (integrità dello scafo) o «scafo squarciato»
  dopo il messaggio 53.
- **ESM al periscopio:** il livello del runtime (`Missione::livello_esm`, `DS:476C`: l'ultima intercettazione radar
  di periscopio o antenna, 85 − (|dx| + |dy|)/32) in 9 riquadri (10 sull'Alfa) con i colori di `DS:029B`.
- **Scie** (`12EE`, `numeri_eseguibile.md` §6): il runtime registra l'anello di 150 punti (`Missione::scia()`, della
  sessione sulla cadenza); la carta li disegna con i pulsanti del Plotting sui tasti 1 (ALL TRACKS), 2 (TARGET
  TRACK, che vince) e 3 (PLOT PROJ): 4 il bersaglio, 8 il proprio battello, 0 gli altri. Senza finestra:
  `--scie tutte|bersaglio`.
- **Da fare:** il mouse; le spie dell'Alfa al Sonar.
- **Scoperto durante la C4** (`numeri_eseguibile.md` §3.3 e §4): nell'originale rotte e ratei sono angoli
  matematici (0 = est, positivo a sinistra), mentre le missioni convertite li usano come rotte bussola. Va
  corretto nel convertitore e nelle 22 missioni (lavoro a parte).

**Stazioni con i danni del battello nell'Enhanced** (24/10/2026, con la fase D6c della sessione kit, `docs/10` §10.5):
- con la regola di simulazione `DanniSottomarini` (`missioni/danni_sottomarini.hpp`, del kit) segue i sottomarini
  colpiti, giocatore compreso; l'app lo crea accanto a `DanniMissione`, unisce i suoi messaggi (270–281) e con
  l'emersione d'emergenza chiama anche l'esaurimento delle casse;
- `StazioniMissione::usa_danni`: il tiro 1/2 dei guasti dell'originale non c'è più; i sei sistemi dell'originale
  sono fuori uso con l'efficienza a zero o con i limiti del battello (periscopio, antenna, schiera, sonar), e i
  rifiuti tornano con i messaggi dell'originale; in più timone e timoni orizzontali bloccati (214, 215) e il
  telegrafo oltre la frazione della massima che motori e propulsione danno ancora;
- quadro di stato (F3): i dodici sistemi del battello con l'efficienza in percentuale.

## 10. Piano della fase C6 e ripartizione fra le sessioni (24/10/2026)

La C6 coincide in gran parte con la Fase 2 della roadmap (simulazione dell'Enhanced). Il livello classico non
cambia: ogni pezzo vale solo con la regola di simulazione, e il rilievo classico deve restare identico alla base.

| Pezzo | Contenuto | Chi | File propri |
|---|---|---|---|
| C6a | **Dinamica del sottomarino**: spinta, resistenza, timone e idroplani, assetto, cavitazione (dossier 01 e 02 §2.3); peso netto e momento dal battello danneggiato (D6); poi il moto dell'Enhanced in `missione.cpp`. **Fatta il 24/10/2026:** `DinamicaSottomarino` e `ComandiSottomarino`; nella missione `dinamica_sottomarini` (da DanniMissione con la regola di simulazione), `comandi_sottomarino(id, …)` per il giocatore, timoniere dell'IA, `dinamica(id)` per le stazioni; forze e governi da `OrdineMotore`. Rilievo classico identico | sessione sulla cadenza | `nucleo/dinamica_sottomarino.*`, prove |
| C6b | **Siluri dell'Enhanced**: impostazioni reali (modo, velocità, RTE, soffitto e pavimento, schema, anti-circolare), logica di ricerca e aggancio, filo (portata, vincoli al battello, spegnimento e taglio), esca ADC Mk2 (dossier 01 e 05). **Fatta il 24/10/2026:** `Siluro`, `Esca`, tabella delle classi con l'anno di servizio; `CampoSiluro` e `applica()` per l'ordine `imposta_siluro`; nella missione `siluri_enhanced(cartella)`, che legge `data/motore/siluri.toml` (generato da `tools/pipeline/siluri.py`; da DanniMissione con la regola di simulazione), `imposta_siluro_giocatore` prima del lancio, `comanda_siluro(id, ComandoFilo)`, `siluro(id)` per gli avvisi del filo, `eventi_siluri()`; nel `Colpo` la geometria vera dello scoppio per il kit. Rilievo classico identico | disassemblatore | `nucleo/siluro.*`, prove |
| C6c | **Ordini e stazioni dell'Enhanced**: impostazioni dei siluri, comandi via filo con lo spegnimento, TMA con lo strumento MATE (dossier 02 §5.1), posarsi sul fondo (§6, dopo C6a) | sessione principale | `nucleo/*_giocatore*`, `app/`, `piattaforma/`, `nucleo/ordini.*` |
| D6e, D6d | sottomarini dell'IA colpiti; poi stazione Stato completa, rapporti, luci e vista (`docs/10` §10) | kit, con la principale per l'interfaccia | `missioni/danni_*`, `nucleo/battello.*` |

**C6c, primo pezzo fatto: TMA** (24/10/2026):
- `nucleo/tma.hpp` (prove `tma:`): MATE (scarti fra i rilevamenti previsti da una soluzione e quelli misurati), rateo
  di rilevamento ai minimi quadrati, regola del 1934 e distanza di Ekelund su due gambe; su una gamba sola la
  distanza resta inosservabile, come deve;
- `StazioniMissione::raccogli_tma`: con la regola di simulazione un rilevamento del bersaglio ogni 5 s di
  simulazione finché c'è il contatto, con lo scarto del sonar (0,5° di deviazione); cambiando bersaglio si riparte;
- alla carta (F6) **T** apre il MATE: frecce sinistra e destra per la rotta, su e giù per la distanza, Maiuscolo con
  su e giù per la velocità; la carta mostra le linee di rilevamento, le posizioni della soluzione e la striscia degli
  scarti; il pannello la soluzione, lo scarto medio e il rateo degli ultimi 2 minuti. Senza finestra: `--mate`.

**Interfacce concordate per la C6** (24/10/2026):
- **ordini dell'Enhanced** (`nucleo/ordini.hpp`): `barra`, `accosta_per`, `giri`, `quota_con_angolo`,
  `angolo_immersione`, `imposta_siluro`, `comando_filo`. Li produce un posto di comando dell'Enhanced e li traduce
  `StazioniMissione` (principale) in `ComandiSottomarino` (C6a) e in impostazioni e comandi via filo (C6b);
  modello dinamico e siluri non leggono gli `Ordine`;
- **D6e:** i segni in superficie dei sottomarini colpiti (`DanniSottomarini::segni`) vanno agli effetti visivi
  (`Effetti::segno_superficie`: schiuma e bolle, nafta, rottami) dal loro inizio per la durata che resta;
- **D6d:** dal kit `rapporti(id)`, `scossa(id, tempo)` e `compartimenti(id)`; il pannello, le luci e la vista sono
  della principale.

**C6c, secondo pezzo: manovra dell'Enhanced** (24/10/2026, sopra la dinamica della C6a):
- `PostoEnhanced` (`nucleo/comando.hpp`, prove `enhanced:`): gli stessi tasti del classico danno barra in gradi a
  scatti di 5 fino a 35, le nove campane (IND. TUTTA … MASSIMA; avanti 1/3, 2/3, standard e tutta a 5, 10, 15 e 20
  nodi, tradotti in giri dalla classe), quota ordinata a passi di 15 m con angolo massimo di 15°, quota periscopica a
  18 m; segue anche gli ordini di un replay;
- `StazioniMissione` tiene lo stato intero dei comandi del giocatore e lo manda a `Missione::comandi_sottomarino`
  a ogni ordine; nel classico gli ordini dell'Enhanced non valgono. Rifiuti: timone e idroplani bloccati, giri oltre
  la velocità che resta. Pilota, periscopio, antenna e ping restano quelli del classico; la domanda del taglio della
  schiera no (è una regola dell'originale);
- pannello Manovra dell'Enhanced: campane, barra, giri, assetto, soglia di cavitazione e «CAVITA!», quota in metri.

**C6c, terzo pezzo: posarsi sul fondo** (24/10/2026, §6; Ctrl+F nell'Enhanced):
- rifiuti dell'equipaggio con il motivo: schiera fuori o in manovra (217), pendio oltre 5° misurato sul fondale
  attorno al battello (218), fondo oltre la quota di collaudo dello scafo (219);
- altrimenti (216) discesa lenta a 3 nodi con 10° al massimo verso un metro dal fondo, che la quota segue mentre il
  battello avanza; a 3 m dal fondo le macchine si fermano da sole e il battello si adagia con l'abbrivio;
- **appoggio:** a macchine ferme gli idroplani non portano; con i danni del battello attivi (regola di simulazione)
  al fermo si imbarcano 2 t nelle casse d'assetto del kit (`DanniSottomarini::appesantisci`, circa 40 s) e il
  battello si appoggia; il primo ordine di macchina o di quota le fa scaricare con le pompe (piu' lente in
  profondita'). Effetti sul sonar nemico (riverbero del fondo) della Fase 2 dell'acustica; i rischi (fango, rocce,
  prese a mare) sono in `missione_fondo.cpp` (28/10/2026, docs/18).

**C6c, quarto pezzo: siluri dell'Enhanced dal posto del giocatore** (24/10/2026, sopra la C6b):
- **impostazioni per tubo** (`StazioneArmi::impostazioni`, ordine `imposta_siluro`, regole di `applica` della C6b):
  alla stazione Armi Tab sceglie il campo (rotta, testa, velocità, RTE, quote di corsa e di ricerca, soffitto,
  pavimento, schema, anti-circolare) e `[` `]` cambiano il valore; al lancio il siluro le riceve
  (`Missione::imposta_siluro_giocatore`), altrimenti prende quelle ricavate dal LAUNCH dell'originale;
- **comandi via filo** (`comando_filo`, `spegni_siluro`, `taglia_filo`): C abilita la testa, D spegne (il siluro
  affonda senza esplodere: niente detonazione), Z e X correggono la rotta di 10°, 8 e 9 la quota di 15 m, Y taglia
  il filo; rifiutati senza
  siluro o senza filo;
- **rapporti** dagli eventi del modello: testa attiva, aggancio, contatto perso, spegnimento (263–266), filo
  tranciato, fine corsa ed esplosione con i messaggi 23, 24 e 19 dell'originale, e il 39 una volta per siluro
  quando il filo è a rischio; il pannello mostra la fase del siluro scelto e quanto filo è svolto.

Con questo la C6c è completa (alle Armi 8 e 9 cambiano anche la quota del siluro via filo di 15 m).

**Acustica dell'Enhanced nelle stazioni** (24/10/2026, dopo il modulo del disassemblatore e l'aggancio della sessione
sul runtime):
- carta, periscopio e TMA usano il rilevamento misurato del contatto (`Contatto::rilevamento`, con l'errore del
  sensore; nel classico e' quello vero), e la schiera mostra una seconda X per l'ambiguita' finche' non si risolve
  (`rilevamento_specchio`);
- lettera, distanza e missili seguono `posizione_nota`: nel classico la traccia solida, nell'Enhanced per il
  giocatore solo eco attivo, vista o radar; col solo passivo resta la X sul rilevamento;
- la barra del bersaglio dice il sensore dell'ultima scoperta e l'eccesso di segnale in dB;
- i messaggi del sonar (310–314: nuovo contatto, siluro in acqua, ci pingano, cavitazione, schiera cieca) vanno nella
  barra con gli altri.

**D6d fatta, lato interfaccia** (24/10/2026; i dati sono del kit, `DanniSottomarini::prepara`, `rapporti`,
`scossa`, `compartimenti`):
- con la regola di simulazione il modello del battello del giocatore nasce all'avvio, così il quadro si vede subito;
- **F3** mostra al posto della vista il quadro danni: spaccato dei compartimenti (prua a destra) con l'acqua
  imbarcata in percentuale (rosso se imploso) e gli ultimi 14 rapporti con l'ora di missione; sotto, i dodici
  sistemi e lo scafo;
- **incendi ed esplosioni interne** (kit, `docs/10` §10.13): nello spaccato il fumo scende dall'alto in grigio (pieno
  quando il compartimento non e' piu' abitabile), il fuoco e' una fascia arancio sopra l'acqua, una croce rossa
  segna i compartimenti sventrati, e sotto il 17% d'ossigeno compare «O2»;
- **scossa:** per 1,5 s dopo un urto la vista sobbalza fino a 8 pixel e le luci tremolano (palette scurita a scatti),
  in proporzione al fattore d'urto efficace;
- **prova senza finestra:** `--regola simulazione --urto-giocatore KG,DIST,GRADI` (l'urto di prova del kit), per
  esempio `135,15,90` per il quadro danni con falle e SCRAM, `135,30,0` per una scossa lieve, `295,8,90` per la
  rottura dello scafo.
- **pilota automatico nell'Enhanced:** inserito, rida' i comandi al timoniere automatico verso i punti di rotta; un
  ordine di barra o di accostata lo disinserisce, come il timone nel classico.

**Regole:**
- `missione.cpp` e `missione.hpp` a turni: chi li prende lo annuncia alla principale e li libera con un messaggio;
- dopo un cambio di header si tolgono gli oggetti più vecchi prima di dare la colpa al codice;
- prove con un `TMPDIR` proprio;
- ogni sessione tiene un controllo incrociato su chi tocca le sue interfacce.

## 11. Fase 3: stazioni complete, prima le funzioni e poi la grafica (24/10/2026)

Richiesta dell'utente: «ora lavoriamo anche alle varie stazioni e comandi, poi magari affineremo la grafica degli
stessi, nelle varie parti delle sezioni di comando impostabili». Prima ogni stazione fa tutto quello che deve
(funzioni, ordini, messaggi, prove); la grafica nello stile dell'originale ridisegnato a 640x480 viene dopo, su
stazioni gia' complete. I comandi sono impostabili: tasti rimappabili e aree del mouse.

| Pezzo | Contenuto | Chi |
|---|---|---|
| S1 | **Struttura delle stazioni**: Centrale (menu, stato rapido: quota, rotta, velocita', contatti, armi in acqua; pausa del classico), passaggio fra le stazioni, barre comuni; **tasti rimappabili** da file con la tabella in italiano; **mouse** con aree attive come nell'originale | principale |
| S2 | **Periscopio completo**: ingrandimenti reali, stadimetria (distanza dall'altezza dell'albero), notte, esposizione al radar che cresce col tempo; **ESM ripensato** (punto 23) lato interfaccia | principale |
| S3 | **Sonar completo** (punti 19 e 24): righe tonali per classe, LOFAR e DEMON, presentazioni a banda larga e stretta (cascata per rilevamento), analizzatore di firma con confronto con la banca dati; i dati dal modulo di acustica, la stazione dalla principale | disassemblatore (dati), principale (stazione) |
| S4 | **Sala radio** (punto 22; fatta il 24/10/2026: simulazione e stazione, contenuti e profondita' da pianificare): ricezione secondo quota e antenne (VLF/ELF in profondita', HF/UHF/satellite a quota periscopica), trasmissioni a orari, formato e precedenza dei messaggi, ordini che cambiano, trasmettere espone alla radiogoniometria; poi gli emettitori radar dell'**ESM** con l'EMCON dell'IA | sessione sul runtime (simulazione), principale (stazione) |
| E1 | **Emersione violenta del sottomarino nemico colpito** (richiesta dell'utente): col battello che esaurisce le casse, la prua esce dall'acqua con l'angolo e la spinta della risalita, ricade e solleva una massa d'acqua e schiuma; visibile al periscopio | kit (posa e modello), con un metodo nuovo negli effetti visivi |

**Direzione dell'utente sulle due stazioni piu' profonde** (24/10/2026, sera):
- **Sala radio (S4):** deve avere un vero motivo di esistere. Prima di scrivere i messaggi delle missioni si studia
  a fondo come darle importanza e profondita' di simulazione (cosa cambia nella partita quello che si riceve o si
  perde, quando conviene salire, quanto costa trasmettere). Ricerca: `docs/ricerca/10_radio_ruolo_in_partita.md`.
- **Sonar (S3+):** revisione completa. Cascate dall'aspetto e dalla leggibilita' di quelle delle radio SDR; una
  vasta libreria di firme acustiche (navi, sottomarini, biologici, geofisici, meteo, traffico, ghiaccio, attivita'
  umane), un mare molto piu' vario e realistico da ascoltare; le funzioni dei sonaristi veri (annotazione del
  segnale, salvataggio in slot, confronto, e quanto la ricerca trovera'). Ricerca su sistemi, software e interfacce
  reali: `docs/ricerca/09_sonar_sistemi_e_interfacce.md`. La stazione S3 di oggi resta la base funzionante.

Regole come sempre: file condivisi a turni annunciati, classico identico alla base, prove con `TMPDIR` proprio,
messaggi nelle fasce assegnate (principale 210–269, kit 270–309, runtime 310–339, disassemblatore 340–369).

**Piano della revisione di Sonar e Radio** (24/10/2026, dai dossier `docs/ricerca/09_sonar_sistemi_e_interfacce.md`
e `10_radio_ruolo_in_partita.md`). Scelte dell'utente: broadcast di missione con **ciclo di 15 minuti**; ROE
**severe** (vale cio' che il comando ha trasmesso, il debriefing elenca le serie perse); **inganno raro**, riconoscibile
dall'autenticazione; sonar con **due modi a scelta**, 1989 (fosforo verde, tutto a mano) e moderno (colori, aiuti).

| Pezzo | Contenuto | Chi |
|---|---|---|
| S5a | **Cascate in stile SDR** (fatta il 24/10/2026): pavimento, tetto e AUTO; palette 1989 e moderne; A-scan con massimo e minimo; scala dei tempi x1/x4/x16 con decimazione per massimo; griglia del tempo; **cursori** (lettura, delta, armonico); **tracker S1, S2...** con la lista dei contatti; classificazione a passi (NONSUB, POSSUB, PROBSUB, CERTSUB) e rapporti al comando | principale |
| S5b | **Libreria consultabile** (fatta il 24/10/2026) con le righe della classe sovrapposte al LOFAR e il fattore Doppler; confronto per rapporti; **annotazioni** sulle cascate; **8 slot** con confronto affiancato, sovrapposto e per differenza | principale (stazione), disassemblatore (API della libreria) |
| S5c | **Paesaggio sonoro** (dati fatti il 24/10/2026, aggancio in corso): sezione `[sorgenti.*]` (tonali, famiglie, bande, sweep, impulsi, canti, processi nel tempo), popolazione per teatro, stagione e ora; biologici, geofisici, meteo, attivita' umane, transitori | disassemblatore (dati e modelli), runtime (sorgenti nella scena) |
| S5d | Verniero e misura **Doppler/CPA** passata alla TMA; FRAZ della schiera; registratore e ACINT; ascolto sintetico; operatore IA con errori plausibili (calcolo fatto il 25/10/2026; stazione fatta, salvo l'operatore IA e il CPA nel MATE) | disassemblatore (moduli puri), principale (stazione e debriefing), runtime (operatore IA nella missione) |
| R1 | **Orologio radio di missione** (simulazione e interfaccia fatte il 24/10/2026) (ciclo 15 min, finestre in scala); **contatti d'intelligence** con ellisse che invecchia; **ordini in vigore e ROE** (trasmessi contro ricevuti, per il debriefing); rapporto di contatto con aiuto e rischio di radiogoniometria; boa SLOT | runtime (simulazione), principale (carta, stazione, debriefing) |
| R2 | **Zone dello spazio d'acqua** e zone protette mobili; compiti con scadenza e campanello ELF; guinzaglio sovietico (sessioni obbligatorie e ricevute); inganno raro; richiamo e rami | runtime, principale (carta) |
| R3 | **Contenuti radio delle 22 missioni** (`data/missioni/radio/<ID>.lua`, formato del dossier 10 §5), dopo R1 e R2. **Fatto il 26/10/2026**: 22 file con ROE di partenza, neutrali, contatti d'intelligence, zone, finestre sovietiche ogni 5-10 minuti e inganni rari; con E6 l'intestazione e la data della serie 0 e i messaggi legati agli eventi (`se`: primo lancio nemico, unità colpite o affondate, variabili degli script, rapporto trasmesso; `annulla`, `ricevuta`). Prove in `prove_scenario_missione.cpp` | libreria (contenuti), runtime (caricatore E6) |

**Piano della carta di navigazione** (24/10/2026, dal dossier `docs/ricerca/12_strumenti_plotting_e_carta.md`).
Scelte dell'utente: tutti e quattro i gruppi di strumenti; carta d'epoca o fondale vero **a scelta per partita**;
grafica VGA/SVGA **insieme** agli strumenti. Ogni strumento ha la forma 1989 (matita grassa, letture arrotondate, nessun
allarme) e la moderna (ECDIS-N/BYG-1).

| Pezzo | Contenuto | Chi |
|---|---|---|
| N1 | **Grafica VGA/SVGA** (fatta il 24/10/2026): a F6 le rampe del mare e del cielo ridefinite (acqua a 16 fasce per 3 ombre, rampe «VGA 1989» e «carta nautica», terra, isobate, matite, colori NTDS, zone); isobate, ombreggiatura, retinatura, rosa attorno al battello, simboli NTDS nel moderno; reticolo lat/lon | principale; centro della proiezione dal kit |
| N2 | **Righello EBL/VRM** (fatto il 24/10/2026), **linee di rilevamento** dai tracker con tacche di tempo, **eventi MARK**, **lucido** con simboli e note, **CPA/TCPA**, **anelli** fissi e d'arma, proiezione con tacche | principale |
| N3 | **Datum** (fatto il 24/10/2026) col cerchio di massimo avanzamento, **regolo di Ekelund** sulla carta, **pannello tempo-rilevamento** | principale |
| N4 | **Zone** (fatte il 24/10/2026) dell'utente e dello spazio d'acqua (R2), **contorno di sicurezza** legato alla quota, **livelli** | principale; zone R2 dal runtime |
| N5 | **Carta d'epoca** (fatta il 24/10/2026) per teatro e parte (docs/08 §5), opzione della partita contro il fondale vero | kit (dati), principale (scelta) |

**S1, primo pezzo: tasti rimappabili** (24/10/2026, `nucleo/mappa_tasti.hpp`, prove `tasti:`):
- ogni tasto ha un nome in italiano («Tastierino 4», «Ctrl+P», «F7», «W», «[») legato a un comando dal nome parlante;
  i predefiniti sono quelli di oggi; la finestra traduce il tasto SDL nel nome e il comando nei campi di `Comandi`;
- un file TOML (`--tasti FILE`, altrimenti `~/.config/melchior688/tasti.toml` se c'e') li cambia: un comando elencato
  perde i suoi tasti e prende quelli del file, `""` lo lascia senza tasto, un tasto ripreso viene tolto all'altro
  comando; errori con il motivo;
- `--scrivi-tasti FILE` scrive il modello con tutti i comandi, i tasti in uso e una descrizione; quello predefinito e'
  in `data/motore/tasti_predefiniti.toml`;
- restano fissi i tasti tenuti premuti (frecce per il periscopio e il cursore, Maiuscolo) e le lettere della scelta
  del bersaglio.

**S1, secondo pezzo: Centrale, pausa e mouse** (24/10/2026):
- **Centrale (F9):** stato rapido (quota, rotta, velocita' dalla dinamica nell'Enhanced, periscopio, antenna,
  schiera, contatti e quanti con la posizione, armi in acqua, sistemi guasti, scafo) e il menu delle stazioni;
- **pausa:** nel classico la Centrale ferma il tempo come il CONN dell'originale; nell'Enhanced no, e c'e' la pausa
  esplicita con P (Esc la toglie); in entrambi P ferma il tempo;
- **mouse:** i pannelli dichiarano le loro aree attive (tubi, pulsanti del sonar, menu della Centrale) e un clic vale
  il comando dell'area, con gli stessi nomi dei tasti; durante TARGET un clic sulla X o sulla lettera sceglie il
  contatto e chiude la scelta (cosi' anche le X, come nell'originale); durante SET un clic sulla carta mette il punto;
- **`--periscopio-su ID`:** il periscopio segue un'unita' (istantanee degli effetti, verifiche a vista).

**S2, primo pezzo: stadimetro** (24/10/2026, H al periscopio alzato): sul bersaglio di superficie nel campo la
distanza dall'angolo fra testa d'albero e galleggiamento; l'altezza dell'albero e' quella vera del modello 3D se la
classe e' nota (durata della traccia >= 1400), altrimenti 30 m presunti (errore sistematico se la nave e' piu' alta o
piu' bassa); errore di lettura di circa il 3% a 6x e l'8% a 1,5x. La misura compare nella barra del bersaglio
(«STADIMETRO 3,4 NM (ALBERO PRESUNTO 30 M)»); messaggi 267 e 268.

**S3: stazione Sonar dell'Enhanced** (24/10/2026, `app/sonar_enhanced.hpp`, prove `sonar enhanced:`): con
`--regola simulazione` la vista di F8 e' la stazione completa, sui dati di `nucleo/analisi_acustica` (disassemblatore)
e sulla scena di `Missione::scena_acustica` (sessione sul runtime):
- **cascata per rilevamento** (banda larga, a sinistra): 72 fasci da 5° sul sensore scelto, una riga ogni 2 s
  simulati, la piu' recente in alto, 5 minuti di storia; in blu i settori ciechi (baffles, o endfire della schiera);
  grana del rumore con un suo seme, quindi il replay da' le stesse righe;
- **LOFAR** (a destra): lo spettro del fascio puntato in bin da 0,1 Hz come la banca dati, filtro passa banda
  10-410 Hz o passa alto 100-410 Hz (pulsante 1); il fascio segue il rilevamento misurato del bersaglio, oppure
  Z e X lo fissano a mano a passi di 5° (6 lo riporta sul bersaglio); il titolo dice il rilevamento vero, mentre la
  barra del bersaglio resta relativa alla prua come nell'originale;
- **integrazione** (pulsante 7: 1, 2, 4, 8 righe, predefinita 4): ogni riga mostrata e' la media delle ultime N
  misurate, la grana del rumore (1,5 dB anche nel LOFAR) cala di radice di N e le righe deboli escono; il LOFAR
  riparte da capo solo se cambia il filtro o il fascio salta di oltre 10°;
- **normalizzatore a finestra divisa** nel LOFAR: a ogni pixel si toglie la media dei pixel da 4 a 24 di distanza ai
  due lati; il continuo a banda larga (anche la cavitazione) sparisce e restano le righe strette, come nei display
  veri;
- **DEMON** del bersaglio: asse, pala e numero di pale, solo quando cavita;
- **ANALIZZA** (pulsante 4): righe osservate nell'ultimo spettro, pale dal DEMON, confronto con le classi; le prime tre
  ipotesi con la confidenza (messaggio 342 se la prima supera il 50 per cento, altrimenti 343);
- **sensore** (pulsante 5): passa fra i passivi della suite (sfera, schiera rimorchiata se filata, ...); la storia
  riparte.
Un Victor III a 5 nodi a 3 km resta appena sopra la grana (circa 3,7 dB in una riga sola): senza integrazione si
perde nel rumore, con 8 righe e' quasi sempre il fascio piu' forte; un Charlie II a 8 nodi si legge e si classifica.

**S4: stazione Radio** (24/10/2026, `app/stazione_radio.hpp`, prove `stazione radio:`; la sala radio e' della
sessione sul runtime, `nucleo/radio.hpp` e `Missione::radio()`). F2, come la RADIO ROOM dell'originale:
- **classico:** periscopio e antenna su o giu' (pulsanti 1 e 2, solo a quota periscopica), archivio dei messaggi di
  bordo gia' passati con PREC. e SUCC. (3 e 4), il messaggio scelto per intero in alto;
- **Enhanced:** in alto data-ora (DTG), prossima finestra di comunicazione con la tolleranza, albero, filo
  galleggiante (recuperato, in filata, filato, rotto), ricevitore ELF se c'e', ricezione per banda (VLF, HF, UHF:
  SI, 50 PER CENTO, NO); il broadcast in onda (serie, precedenza, orari, ripetizione) con la barra della ricezione,
  quelli in programma, i buchi nella serie; la **telescrivente** che compone il messaggio in arrivo, altrimenti il
  messaggio scelto (serie, precedenza, DTG, classifica, da, a, oggetto, autenticazione verificata o no, testo);
  l'archivio dei ricevuti con la banda;
- **pulsanti dell'Enhanced:** 1 antenna, 2 cala o recupera il filo, 3 e 4 archivio, 5 solo gli ordini di missione,
  6 tipo della trasmissione (contatto, attacco, situazione, conferma, richiesta), 7 trasmetti, 8 scarico via
  satellite (SSIXS); ordini `radio_filo`, `radio_trasmetti`, `radio_ssixs` tradotti da `StazioniMissione`; senza
  antenna alzata il rifiuto 269; l'esito della trasmissione (il fix nemico) non si mostra, il comandante non lo sa.
Nota dalla prova: in SCENE1A dopo 30 minuti non va in onda nulla (ciclo di 2 ore, finestre ogni 12): e' il
problema di scala del dossier `docs/ricerca/10_radio_ruolo_in_partita.md`, da risolvere nel piano della sala radio.

**S5a: cascate in stile SDR, cursori e tracker** (24/10/2026, `app/sonar_enhanced.hpp`, prove `sonar enhanced:`;
dossier `docs/ricerca/09_sonar_sistemi_e_interfacce.md` §4 e §6.1). La stazione Sonar dell'Enhanced ha due modi,
scelti per partita con `--sonar 1989|moderno` (predefinito moderno):
- **resa:** scala dei colori fra **pavimento** e **tetto** in dB sopra il fondo (2 e 16 all'inizio), palette negli
  indici 200-231 (il cielo, che alla stazione non si vede): nel 1989 fosforo verde e grigio, nel moderno viridis,
  turbo, caldo, grigio e fosforo; **AUTO** (pulsante 8, solo moderno) ricava pavimento e tetto dall'ultima riga;
  Tab sceglie il campo (pavimento, tetto, palette, scala) e `[` `]` il valore;
- **normalizzatori:** a finestra divisa sui fasci del BTR (fasci da 2 a 6 di distanza) e sui pixel del LOFAR;
- **A-scan** sopra il LOFAR: l'ultima riga (verde), il massimo (giallo) e il minimo (grigio) delle ultime 8;
- **scala dei tempi** x1, x4, x16: una riga per 1, 4 o 16 misure, col massimo (i transitori brevi restano); griglia
  del tempo ogni 1, 5 o 15 minuti; scala delle frequenze in fondo al LOFAR;
- **zoom** del LOFAR (I e O, fino a x16) attorno al cursore;
- **cursori** (pulsante 9, a giro): lettura (frequenza e quanto tempo fa), delta rispetto al riferimento (dove era il
  cursore quando lo si e' acceso), famiglia armonica (k volte la frequenza, con i numeri); frecce per muoverli,
  Maiuscolo piu' veloce;
- **tracker S1, S2...** (al massimo 8): T ne mette uno sul fascio del LOFAR, K toglie quello seguito; nel modo
  moderno anche automatici sui picchi oltre 8 dB a piu' di 20° dagli altri, con i doppioni uniti; ognuno segue il
  baricentro dei fasci attorno al picco con un filtro alfa-beta (rilevamento e rateo in gradi al minuto), si perde
  dopo 30 s senza segnale; il pulsante 6 passa il LOFAR dal bersaglio ai tracker a giro;
- **classificazione a passi:** ANALIZZA sul fascio di un tracker gli da' classe e passo (NONSUB, POSSUB, PROBSUB,
  CERTSUB dalla somma delle confidenze delle classi sottomarine: 0,25, 0,6, 0,9) e il rapporto al comando quando il
  passo cambia (messaggi 370-376: nuovo tracker, perso, possibile, probabile, certo, non sottomarino, tracker pieni).
Prova sul Charlie II a 3 km: il tracker sta entro 1,5° dal vero e il rateo entro 2°/min (vero 6,5).

**S5b: libreria, analisi per rapporti, annotazioni e slot** (24/10/2026; API del disassemblatore in
`nucleo/analisi_acustica.hpp`, stazione in `app/sonar_enhanced.*`; prove `sonar enhanced:`):
- **libreria** (B, oppure Esc per chiuderla): schede navale, biologico, geofisico, meteo e umano (Tab), voci con le
  frecce; la scheda della voce ha nome, categoria, pale, righe con l'etichetta, ritmo, indizi, con che cosa si
  confonde, come suona, fonte e confidenza, e uno spettro di riferimento (righe in altezza per livello, bande in
  grigio, sweep come segmenti);
- **sovrapposizione** (Invio sulla voce): le righe della voce sul LOFAR, colorate per tipo (rete ciano, elica gialla,
  biologico verde...), spostate del fattore Doppler k, che viene dall'ultima analisi se la voce c'era o si regola nel
  campo «K» della presentazione (passi di 0,0005, circa 1,5 nodi); Invio fuori dalla libreria la toglie;
- **ANALIZZA** usa ora anche il confronto per rapporti (insensibile al Doppler): voce migliore con confidenza, k,
  velocita' radiale (positiva in avvicinamento) e righe concordi su attese; e le famiglie armoniche (rete 50/60/400,
  famiglie della libreria, elica con frequenza di pala e pale stimate);
- **annotazioni** (W al cursore, J toglie l'ultima): N1, N2... sulla cascata, che scendono col tempo;
- **8 slot** (S salva l'ultimo spettro nel primo slot vuoto, Y sceglie lo slot, C il confronto): affiancato (lo slot
  sotto l'ultima riga nell'A-scan), sovrapposto (in ciano) o differenza (ultima riga meno slot, verde sopra e rosso
  sotto lo zero).
Sul Charlie II a 3 km: charlie2 al 97 per cento, k 1,0009 (+2,7 nodi), 11 righe su 11; il tracker CERTSUB.

**R1 ed ESM nell'interfaccia** (24/10/2026; simulazione della sessione sul runtime):
- **stazione Radio:** riga delle ROE in vigore secondo i messaggi ricevuti (sottomarini, navi, mercantili, neutrali,
  aerei: libero in verde, stretto in giallo, trattenuto in rosso, fra parentesi la serie che le ha stabilite); boe
  SLOT rimaste, ritardo scelto con `[` `]` (5-30 minuti) e trasmissione in attesa; il pulsante 9 lancia la boa
  (ordine `radio_slot`, a = ritardo; rifiuto 377 senza boe o fuori dai limiti della missione);
- **carta:** i contatti d'intelligence come ellisse d'incertezza (asse lungo sulla rotta stimata, che si allarga con
  l'eta' del dato) con il centro, il sensore e i minuti;
- **periscopio:** con periscopio o antenna alzati, i 9 riquadri dal livello ESM dell'Enhanced e l'elenco delle
  intercettazioni (rilevamento, banda, identita' secondo l'operatore, ampiezza, tendenza, rischio colorato);
- messaggi ESM 334-338 nelle liste dell'app.
- la sala radio parte dall'ora del pannello (`--data`, altrimenti il 15/10/1989 alle 09:00Z): DTG e finestre
  coerenti con l'orologio della missione. Per ricevere serve l'albero (antenna o periscopio a quota periscopica) o il
  filo galleggiante: in profondita' senza filo i messaggi vanno in onda ma si perdono, e restano i buchi nella serie.

**N1 e N2: carta VGA/SVGA e primi strumenti** (24/10/2026; `app/carta.cpp`, `nucleo/strumenti_carta.hpp`; prove
`carta:` e `carta enhanced:`). Solo nell'Enhanced; il classico resta com'era.
- **grafica:** a F6 le rampe del mare e del cielo diventano la palette della carta (`applica_palette_carta`): acqua a
  16 fasce di profondita' (10, 20, 30, 50, 75, 100, 150, 200, 300, 400, 500, 750, 1000, 1500, 2000 m) per 3 ombre dalla
  luce di nord-ovest, retinatura 2x2 verso la fascia successiva, terra a 7 quote dal camoscio al bruno con la costa
  scura, isobate (principali a 100, 200, 500, 1000, 2000 m), rosa graduata attorno al battello come sul DRT; B (o
  `--stile-carta`) cambia lo stile: «VGA 1989» (piu' scuro = piu' profondo), «carta nautica» (piu' chiaro = piu'
  profondo), foglio di plottaggio;
- **righello EBL/VRM** (4): A sul battello e B a 3 nm sulla prua; frecce sull'estremo attivo (Maiuscolo piu' veloce),
  Tab cambia estremo (A si stacca dal battello), clic lo mette sul punto, F lo aggancia al battello o al contatto con
  la posizione piu' vicino, Invio fissa il tratto con l'etichetta, K toglie l'ultimo tratto; lettura «rilevamento,
  distanza, minuti alla velocita' attuale»; nel moderno il cerchio del VRM;
- **linee di rilevamento** (5: spente, bersaglio, anche i tracker S1...): dalla posizione del battello all'istante
  della misura, con fonte e ora; ogni 30 s nel moderno (sbiadite dopo 10 minuti), ogni 2 minuti dal plotter del 1989;
- **anelli** (6): 1, 2, 5 nm; poi anche il filo del Mk 48 (7,5 nm);
- **CPA/TCPA** (8) sul bersaglio con la traccia: punti di minima distanza, moto relativo e lettura;
- **eventi** (E) col triangolo, il numero e l'ora a quattro cifre; **datum** automatico quando il bersaglio si perde,
  col cerchio di massimo avanzamento a 30 nodi che si allarga col tempo; Maiuscolo+K pulisce il lucido;
- modo 1989 (`--sonar 1989`): letture arrotondate alla tacca della scala (0,1 nm fino a S = 12, 0,5 nm oltre) e al
  grado, niente cerchio del VRM.
**N2-N4 completati** (24/10/2026; prove `carta:` e `carta enhanced:`):
- **simboli e note** (7): un cursore (frecce, Maiuscolo piu' veloce, clic); Tab l'affiliazione (ostile, ignoto, amico,
  neutrale), `[` `]` il tipo (subacqueo, superficie, aereo, nota, croce, zona); Invio mette il segno e apre la scritta
  (fino a 24 caratteri: la finestra passa il testo digitato, Backspace cancella, Invio o Esc chiude); K toglie
  l'ultimo. Nel moderno i telai APP-6/NTDS colorati (amico tondo, ostile rombo, ignoto ottagono, neutrale quadrato; la
  meta' inferiore per il subacqueo, la superiore per l'aereo); nel 1989 le stesse forme con le matite;
- **zone da evitare**: il tipo «zona» del modo segno, raggio con Maiuscolo+su/giu', cerchio tratteggiato;
- **regolo di Ekelund** (Q): sulle linee del bersaglio (o della fonte con l'ultima linea), tacche alle posizioni che
  il bersaglio doveva avere all'istante di ogni linea; frecce per rotta e distanza sull'ultima linea, `[` `]` la
  velocita' a passi di 0,5 nodi, lo scarto in gradi; Invio passa rotta, velocita' e distanza al MATE;
- **pannello tempo-rilevamento** (9): a destra, gli ultimi 20 minuti di linee per fonte, la retta ai minimi quadrati
  e il rateo in gradi al minuto;
- **contorno di sicurezza**: l'isobata della quota del battello piu' 30 m (STIMA), in magenta sulla carta; nel moderno
  l'allarme (378) se la rotta dei prossimi 5 minuti lo tocca e (379) se entra in una zona da evitare. L'allarme guarda
  la carta della parte: con la carta d'epoca ne eredita gli errori.
**R2 nell'interfaccia** (24/10/2026): la carta disegna le zone dello spazio d'acqua ricevute dalla radio
(`Missione::zone_ricevute()`): l'area d'azione in verde acqua, la zona libera ASW in rosso a trattini, le zone protette
(fisse e mobili attorno a un battello amico) in magenta tratteggiato, col nome e la serie del messaggio; le protette
entrano nell'allarme della rotta (379). L'autenticazione si verifica a richiesta: nella stazione Radio Invio sul
messaggio scelto (ordine `radio_verifica`, n = serie) e il gruppo si confronta con la tabella del giorno; prima resta
«da verificare». Un falso da' l'avviso 339 e le sue ROE e zone restano fra le ricevute marcate DUBBIA (in giallo); chi
non verifica crede all'ordine come agli altri. Il giudizio resta sugli ordini autentici («ordine non autenticato,
creduto» nel debriefing). La Radio mostra anche il messaggio ricevuto dopo la scadenza del compito e, per l'Alfa, le sessioni
obbligatorie (in corso o prossima, fatte, mancate).

**Ricarica realistica nel pannello Armi** (24/10/2026; simulazione del disassemblatore in `nucleo/armi_giocatore`,
dossier `docs/ricerca/14_carico_armi.md`): nell'Enhanced i tubi partono carichi dalla dotazione; il clic su un tubo
ricarica (vuoto), annulla (in ricarica), arma (carico) o disarma (armato); dopo un lancio il tubo si ricarica da solo
con la stessa arma finche' ce n'e'. Il pannello mostra i tubi in ricarica in ciano col conto alla rovescia, IN CODA
quando ce ne sono gia' due, CARICO in grigio; Ctrl+S (comando `silenzio_a_bordo`) accende e spegne il silenzio a bordo:
ricariche lunghe il doppio ma senza i colpi che l'acustica nemica sente. Tempi: 688 5 minuti, Alfa 75 s col caricatore
automatico, Sturgeon 7 minuti, Trafalgar e Victor III 4.

**Quadro danni con lo spaccato** (24/10/2026; renderer del kit in `grafica/spaccato.*`, proposta approvata
dall'utente): nell'Enhanced F3 mostra lo spaccato del battello del giocatore in tributo a quello del classico (profilo
vero dal modello 3D, scafo resistente, compartimenti e macchinari per classe; 688 e Alfa disegnati, le altre con un
allestimento generico), con sopra lo stato dei danni (acqua inclinata con l'assetto, falle, fuoco col bagliore, fumo,
compartimenti sventrati o implosi, paratie cedute, casse in esaurimento, SCRAM, sistemi guasti che lampeggiano), le
didascalie per compartimento, il dettaglio ingrandito del compartimento piu' in pericolo (Tab lo sceglie a mano, a giro,
fino a tornare all'automatico) e gli ultimi tre rapporti; i 12 sistemi restano nel pannello sotto. Le squadre di
controllo danni al lavoro arriveranno dal kit. Dal 06/11/2026 (docs/10 §10.19): il pulsante 1 isola, chiude subito o riapre il
compartimento mostrato; il 5 apre la pagina delle squadre, dove 2 e 3 scelgono un sistema e 4 lo porta in cima alle riparazioni.
Dal 06/11/2026: F11 apre la schermata dei comandi da ogni stazione, sulla scheda di quella stazione (`app/schermata_comandi`, disegno del kit, dati generati
da `tools/pipeline/tasti_schermate.toml`); il tempo si ferma e alla partita non passa niente. La quota a schermo scende senza scalini da 60 m alla
quota periscopica della classe; la spia PERI usa la stessa soglia del rifiuto del periscopio; il rifiuto dell'Enhanced dice come fare (messaggio 500), e
Ctrl+P sotto i 3 nodi lo dice il timoniere (469). La soglia resta 6,6 m della missione.

**N5 e reticolo: carta d'epoca e gradi** (24/10/2026; dati del kit, `nucleo/fondale.hpp`, docs/08 §5): nell'Enhanced la
carta mostra la carta nautica della parte del giocatore (NATO, edizione 1988; sovietica 1987 con l'Alfa), derivata dal
fondale vero e degradata; `--carta vera` mostra invece il fondale della simulazione. Sulla carta d'epoca: tratteggio
diagonale sulle zone non rilevate, note in magenta (ED, PA, acqua scolorita, profondita' minore riportata con l'anno),
cartiglio con l'edizione. Su entrambe il reticolo di paralleli e meridiani dalla georeferenziazione (passo tondo da 1
primo a 2 gradi secondo la scala, etichette sui bordi). Simulazione, IA e incagli restano sul fondale vero: chi si fida
della carta d'epoca in acque poco rilevate rischia il fondo.


**S5d nella stazione sonar** (25/10/2026; moduli del disassemblatore `nucleo/misure_sonar`, `acint`, `sonarista`,
`ascolto`; prova `sonar enhanced: verniero, curva a S e CPA, registratore ACINT, ascolto in WAV`):
- **F** col cursore acceso aggancia il verniero alla riga, e un secondo F lo sgancia. Ogni 10 s il verniero misura la
  riga nello spettro di zoom (0,01 Hz) e la segue. Un riquadro sul LOFAR mostra:
  - frequenza e incertezza;
  - la curva a S con i punti misurati e la curva del CPA;
  - il CPA (minuti, distanza, velocità relativa, frequenza a riposo);
  - il FRAZ con la sua incertezza, oppure il motivo per cui non vale.
  Nella prova il Charlie II dà il CPA atteso (circa 5,5 minuti, 2,5 km).
- **Registratore:** un campione ogni 5 s del contatto sul fascio (il bersaglio, o il contatto più vicino al fascio
  entro 5°). A fine missione il **rapporto ACINT** è una pagina del debriefing dopo l'esito (Invio).
- **E** registra 8 s d'ascolto del fascio in un WAV nella cartella `ascolti/`, accanto a quella dei fascicoli,
  finché il gioco non ha l'uscita audio. Messaggi 390-394.
- Da fare:
  - lo scarto del CPA nel MATE (`confronta_cpa`), con la misura legata al contatto;
  - l'operatore IA (`Sonarista`), che va alimentato dalla missione (runtime);
  - la persistenza del «quaderno del sonarista» (requisito 27).

**Audio R1 e R2** (25/10/2026; moduli del disassemblatore `piattaforma/audio`, `nucleo/mixer_audio`, `CuffiaSonar` in
`nucleo/ascolto`):
- L'uscita SDL3 si apre con la finestra. `--audio piena|sound_blaster|originale|spento` sceglie la resa. Senza
  dispositivo si gioca in silenzio. Il mixer di main tiene 100 ms in coda, va in pausa col gioco e si svuota a fine
  missione.
- La **cuffia del sonarista** si sente alla stazione Sonar, a tempo normale. Suona il fascio puntato del sensore
  scelto, con la banda del filtro (passa alto da 100 Hz). La scena si rilegge ogni 0,25 s. Con la pausa, il tempo
  accelerato o un'altra stazione la cuffia tace.
- Restano da fare i tasti per l'ottava su e per l'ascolto accelerato ×4 e ×16 (già nel modulo).

**Sonar: CPA nel MATE, modi della cuffia, quaderno** (25/10/2026):
- Nel MATE della carta, quando il verniero ha un CPA valido sul bersaglio, sotto la striscia degli scarti compare il
  CPA misurato (distanza, velocità) contro quello che la soluzione prevede, con lo scarto di tempo e il giudizio
  «coerente» o no (`confronta_cpa`).
- **Q** alla stazione Sonar cambia il modo della cuffia: normale, ottava su (righe sotto 50 Hz), accelerata ×4 e ×16.
  Il modo scelto è scritto sopra il LOFAR.
- **Quaderno del sonarista:** a fine missione gli slot pieni si salvano in `quaderno_sonar.txt` (accanto ai
  fascicoli), ciascuno con la sua origine («SCENE1A 00:12»); alla missione successiva si ricaricano, così uno spettro
  registrato si confronta con un contatto nuovo. Prova `il quaderno del sonarista porta gli slot da una missione
  all'altra`.

**Sonarista dell'equipaggio** (S5d, 25/10/2026; modulo del disassemblatore, missione del runtime):
- `--sonarista nessuno|recluta|medio|esperto`, e S nella scelta della missione.
- I rapporti del sonarista («Comandante, sonar: nuovo contatto, rilevamento 313, designato Sierra 1», classificato,
  perso, ritrovato) vanno nella barra e nello storico dei messaggi. Nel registro le sue classificazioni contano a
  parte, fuori dal giudizio del giocatore.
- Il predefinito è «medio». Col sonarista attivo tacciono i vecchi avvisi di contatto della missione (310, 340, 341,
  346); restano l'allarme del siluro in acqua (311) e gli indizi della sala sonar (344, 345, 347).
- Da rivedere: nella prova su SCENE1A la recluta classifica come «possibile sottomarino» tutte e quattro le navi
  (segnalato al disassemblatore).
