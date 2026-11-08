# C11. Radio

*Autore: kit, per il runtime. Revisione del 27/10/2026. Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Decide che cosa il comandante sa degli ordini, e quanto gli costa saperlo. Il comando parla al battello con una
trasmissione circolare (il broadcast) che non aspetta nessuno: un messaggio va in onda a un'ora, si ripete qualche
volta e poi sparisce. Per riceverlo bisogna avere l'antenna giusta in quel momento. In profondità arriva solo la VLF,
col filo galleggiante e andando piano; tutto il resto chiede la quota periscopica e l'albero alzato, cioè esposizione
al radar e alla vista. I messaggi cambiano le regole d'ingaggio, portano contatti d'intelligence e zone, e a volte
sono falsi. Trasmettere è peggio che ricevere: ogni emissione si può rilevare con la radiogoniometria nemica. È la
scelta fra restare nascosti con ordini vecchi e salire a leggere quelli nuovi.

## 2. Il modello

Il codice è in `nucleo/radio.hpp` e `nucleo/src/radio.cpp` (la classe `SalaRadio`, modulo puro). La missione lo crea e
lo alimenta in `missioni/src/missione.cpp` (`Missione::radio_enhanced`, `passo_radio`, `radio_trasmetti`,
`radio_slot`). La stazione è `app/stazione_radio.cpp`. I parametri sono in `ParametriRadio`; i contenuti di ogni
missione in `data/missioni/radio/<ID>.lua` (formato in `docs/09_schema_missioni.md` §5.1).

La sala radio esiste solo con la regola di simulazione dell'Enhanced (`DanniMissione::avanza`, `partita.cpp`).

### 2.1 Le bande

| Banda | Verso | Con che cosa si riceve nel gioco | Uso |
|---|---|---|---|
| ELF | solo dal comando | filo o albero, fino a 300 m e 15 nodi, con il ricevitore | il campanello dei FLASH |
| VLF | solo dal comando | albero, o filo galleggiante fino a 120 m | il broadcast |
| HF | nei due sensi | solo albero | il broadcast in parallelo; le trasmissioni senza satellite |
| UHF | nei due sensi | albero, solo lato USA, fino a 72° di latitudine | il satellite (SSIXS) |

`Banda` e `SalaRadio::ricezione(Banda)`. La ricezione ha tre valori (`Ricezione`): sì, al 50 per cento, no.

### 2.2 Le antenne

**Sotto la banchisa** (`sotto_banchisa()`, capitolo E11 §2.5) nessuna antenna arriva all'aria: gli alberi non si alzano,
il filo galleggiante e la boa restano sotto il ghiaccio, e si riceve solo l'ELF. Niente VLF né HF, quindi niente
rapporti né conferme finché non si trova acqua libera.

Lo stato delle antenne del giocatore (`StatoAntenne`, da `Missione::antenne_giocatore`): quota, velocità, rateo di
virata, albero alzato, latitudine. «Albero alzato» vale per l'antenna o per il periscopio (flag `+54`, bit 2 o 4).

**Albero o periscopio.** Riceve tutte le bande se la quota è al più `quota_albero_m` = 18 m. Fino a
`velocita_albero_kn` = 10 nodi la ricezione è piena; oltre, al 50 per cento.

**Filo galleggiante** (l'OE-315 del 688). Solo VLF. È una macchina a stati (`Filo`): recuperato, in filata, filato,
rotto.
- Si fila in `filo_filata_s` = 120 s (`cala_filo`); si recupera subito (`recupera_filo`).
- Riceve fino a `quota_filo_m` = 120 m. Pieno fino a `filo_pieno_kn` = 8 nodi, al 50 per cento fino a
  `filo_meta_kn` = 12 nodi, oltre niente.
- È cieco per `filo_cieco_dopo_virata_s` = 120 s dopo ogni passo con un rateo di virata oltre
  `filo_virata_deg_s` = 0,5 °/s.
- Oltre `filo_rottura_kn` = 20 nodi per più di 60 s si può spezzare: probabilità `filo_rottura_al_minuto` = 0,3 al
  minuto, riportata al passo come `1 − (1 − 0,3)^(dt/60)`. Rotto resta rotto fino a fine missione.
- Quando il filo è filato ma non riceve (troppo profondo, troppo veloce, cieco) c'è un avviso, una volta per episodio.

**ELF.** Il ricevitore c'è solo per il lato sovietico (`elf_`, vero con `LatoRadio::urss`: lo ZEVS della Flotta del
Nord). Serve il filo filato o l'albero alzato, quota fino a `quota_elf_m` = 300 m, velocità fino a
`velocita_elf_kn` = 15 nodi. L'ELF non porta testo: solo il campanello (§2.5).

In VLF vale la migliore fra albero e filo.

### 2.3 Il broadcast

Il comando tiene una coda di messaggi (`SalaRadio::programma`). Ogni messaggio riceve un numero di serie progressivo
e, se manca, il gruppo d'autenticazione (due lettere a caso) e il mittente (COMSUBLANT per gli USA, SHTAB SF per i
sovietici).

**Lo spazio in onda.** Un messaggio alla volta (`prossimo_spazio`). Fra quelli già dovuti si sceglie la precedenza più
alta, poi il più vecchio. La durata è:

```
durata = acquisizione_vlf + caratteri / velocità_vlf
caratteri = testo inglese + oggetto + mittente + destinatario + 60     (se il messaggio non li dà)
```

- `acquisizione_vlf_s` = 60 s: sincronismo e cifra, in cui non si stampa niente.
- `caratteri_s_vlf` = 6 caratteri al secondo (50 baud).
- Esempio: il bollettino meteo del traffico di fondo, 63 caratteri di testo, fa 135 caratteri e 82,5 s in onda.

**Le ripetizioni.** Un messaggio torna in onda un ciclo dopo l'inizio della messa in onda precedente
(`q.dovuto = inizio + ciclo_s`). Quante volte dipende dalla precedenza (`ripetizioni`), salvo che il messaggio non dica
`ripetizioni`:

| Precedenza | Prosegno | Messe in onda |
|---|---|---|
| FLASH | Z | 3 |
| IMMEDIATE | O | 3 |
| PRIORITY | P | 2 |
| ROUTINE | R | 1 |

**Il gruppo data-ora** (DTG, «241530Z OCT 89») si fissa alla prima messa in onda, dall'ora UTC e dalla data della
missione (`SalaRadio::dtg`).

**Il traffico di fondo.** Bollettini senza classifica (meteo, sintesi d'intelligence, «nessuna variazione agli
ordini», avviso ai naviganti, prova del broadcast; `FONDO` in `radio.cpp`) in media ogni `fondo_ogni_s`, con un
intervallo a caso fra 0,5 e 1,5 volte. Danno vita alla sala e numeri di serie: senza di loro un buco nella serie non
si vedrebbe. Si programmano con un intervallo d'anticipo, così la stazione vede sempre il prossimo. Con il meteo
dell'Enhanced vanno in coda anche i bollettini veri alle ore UTC del teatro e l'avviso di burrasca (PRIORITY) quando
la previsione vede vento da burrasca entro 24 ore (`Missione::bollettini_meteo`, capitolo sul meteo).

### 2.4 Ricevere e perdere

Durante uno spazio, a ogni passo si guarda la ricezione VLF (`avanza`):
- un solo passo senza ricezione interrompe lo spazio: quella messa in onda è persa;
- un passo al 50 per cento la segna a metà: alla fine si riceve con probabilità 0,5;
- altrimenti alla fine il messaggio è ricevuto. La banda registrata è HF se l'albero è su, altrimenti VLF.

Dopo l'ultima messa in onda non ricevuta il messaggio è perso. Il giocatore lo scopre solo quando riceve un numero
più alto: allora ogni serie precedente finita e mai ricevuta dà l'avviso (`persi()`, evento `perso`).

**Per i sottomarini dell'IA** vale la regola dell'albero: a quota d'antenna, niente banchisa, il messaggio copiato per
intero, al 50 per cento oltre 10 nodi. Il filo galleggiante e il campanello ELF restano del giocatore: l'IA non li ha, per scelta (capitolo E6 §2.8 e §7).

**La telescrivente.** Mentre si riceve, `testo_in_arrivo` restituisce la parte del testo già arrivata: niente durante
l'acquisizione, poi i caratteri a velocità costante fino alla fine dello spazio. `progresso` dà la frazione ricevuta.

**Gli ordini di missione** sono la serie 0: già ricevuti all'inizio, rileggibili, con l'intestazione del briefing o
della chiave `ordini` del file radio e le ROE di partenza.

### 2.5 Il campanello ELF

Un FLASH suona il campanello una volta sola (evento `campanello`, messaggio 320). Suona al primo passo con la
ricezione ELF piena da quando è dovuto: subito, se il battello ascolta già; altrimenti più tardi, quando il filo o
l'albero tornano utili, anche se il FLASH è già andato in onda. Dice di salire a quota di comunicazione: il testo
arriva solo col broadcast.

### 2.6 Il satellite (SSIXS)

Solo lato USA. La «richiesta» (`TipoTrasmissione::richiesta`, pulsante 8, `Missione::radio_ssixs`) è una trasmissione
breve (§2.8). Se il satellite c'è (albero, quota periscopica, latitudine entro `latitudine_satellite` = 72°), dopo
`acquisizione_uhf_s` = 180 s scarica tutti i messaggi già andati in onda almeno una volta, persi compresi. Se nei tre
minuti la ricezione UHF cade (albero giù, quota, latitudine) lo scarico si annulla con l'avviso «satellite non
disponibile». Oltre 72° il satellite non c'è mai.

I messaggi in onda per la prima volta in quel momento non sono ancora nello scarico.

### 2.7 Finestre e sessioni

**Le finestre di comunicazione** (`prossima_finestra`). Con gli orari veri: USA alle 00Z e alle 12Z con tolleranza di
2 ore; URSS alle 00:00 e alle 12:00 di Mosca (21Z e 09Z) con 30 minuti. Nelle missioni vale l'orologio compresso
(§2.10): una finestra ogni `finestra_ogni_s` dal tempo `finestra_primo_s`, con tolleranza `tolleranza_finestra_s`.

**Le sessioni** (`Sessione`, `passo_sessioni`). Ogni finestra diventa una sessione quando si entra nella tolleranza.
È copiata se in qualche istante c'è ricezione VLF; con `ricevuta_richiesta` serve anche una trasmissione nella
finestra. Alla fine è fatta o mancata. Con `sessioni_obbligatorie` una sessione mancata dà l'avviso, e dopo
`saltate_max` = 2 mancate di fila il «guinzaglio»: la missione esegue il gestore `gestore_saltate` (per esempio il
comando che manda a cercare il battello). Una sessione fatta azzera il conto.

### 2.8 Trasmettere

`SalaRadio::trasmetti`. Serve l'albero alzato a quota periscopica; altrimenti la trasmissione non si fa (messaggio
269). Il tipo decide la lunghezza:

| Tipo | Caratteri | HF (s) | UHF (s) |
|---|---|---|---|
| contatto | 300 | 90 | 1,5 |
| attacco | 400 | 100 | 1,7 |
| situazione | 1000 | 160 | 2,7 |
| conferma | 100 | 70 | 1,2 |
| richiesta | 50 | 65 | 1,1 |
| previsione | 150 | 75 | 1,3 |

```
USA col satellite:      UHF,  durata = 1 + caratteri / 600
USA senza satellite:    HF,   durata = 60 + caratteri / 10
URSS:                   HF,   durata = 0,72 s (la raffica «Akula»), qualunque tipo
```

I caratteri sono `CARATTERI` in `radio.cpp`; 600 e 10 sono `caratteri_s_uhf` e `caratteri_s_hf`.

Una trasmissione vale anche come ricevuta per i messaggi ricevuti che la chiedevano (`ricevuta = true`) e per la
sessione in corso. La «previsione» chiede il bollettino meteo al comando (`Missione::radio_trasmetti`,
`previsione_chiesta_`): la risposta va in coda mezzo ciclo dopo (`bollettini_meteo`). Serve il meteo dell'Enhanced.
Dalla stazione l'ordine porta tutti e sei i tipi (`radio_trasmetti` in `stazioni_missione.cpp`, corretto il
27/10/2026: prima la «previsione» partiva come «richiesta»).

### 2.9 La radiogoniometria

La probabilità che il nemico localizzi la trasmissione:

```
P_fix = P_rete · (1 − e^(−t / τ))
```

- `t`: durata dell'emissione in secondi.
- `P_rete`, `τ`, errore e ritardo dipendono dalla banda e da chi ascolta:

| Emissione | P_rete | τ (s) | Errore del fix (1σ) | Ritardo della minaccia |
|---|---|---|---|---|
| HF, rete strategica nemica | 0,9 | 1,5 | 65 km | da 1 a 3 ore, a caso |
| HF, senza rete | 0,2 | 1,5 | 65 km | da 1 a 3 ore |
| UHF, ESM nemico vicino | 0,6 | 3 | solo il rilevamento | subito |
| UHF, nessun ESM vicino | 0,05 | 3 | 185 km | da 3 a 6 ore, a caso |

- «ESM vicino»: una nave o un aereo nemico entro `ORIZZONTE_ESM_M` = 50 km (`Missione::esm_nemico_vicino`).
- La rete strategica (`ContestoDF::rete_nemica`) nella missione è sempre presente: il caso «senza rete» oggi non si
  usa.
- Un'emissione HF lunga è quasi sempre localizzata. Il burst sovietico di 0,72 s dà P = 0,34. Il rapporto di contatto
  via satellite dà 0,02 senza ESM vicino, 0,24 con.

**Le conseguenze** (`Missione::esito_radiogoniometria`, `contatto_da_radiogoniometria`):
- con l'ESM vicino quella sola unità ha subito un contatto col rilevamento della trasmissione, senza distanza;
- altrimenti, passato il ritardo, tutte le unità avversarie hanno un contatto con la posizione del fix (la vera più
  l'errore gaussiano), alla superficie.
- Il contatto nasce con qualità almeno 400 e il sensore «radiogoniometria»; l'unità rivede il bersaglio e la sua
  EMCON passa allo stato «scoperti» (capitolo C10).

Il comandante non sa se è stato localizzato: la stazione non lo mostra.

**I sottomarini dell'IA** (solo Enhanced) sono sotto la stessa regola quando riferiscono un contatto: al più una
trasmissione al minuto, a quota d'antenna, senza salire apposta. La raffica sovietica ha il 34%; l'UHF degli altri il 2%, o il 24%
con una nave o un aereo nemico dell'IA entro 50 km. Il fix arriva al teatro nemico e al giocatore di quella parte, come
contatto d'intelligence con il suo errore (§2.8). Tabella e limiti: capitolo E6 §2.8 e §7.

### 2.10 L'orologio radio di missione

Con gli orari veri (ciclo di 2 ore, finestre ogni 12) in una missione di un'ora non andrebbe in onda quasi niente
(dossier 10 §0). La missione usa quindi un orologio compresso (`parametri_di_missione`):

| Parametro | Valore |
|---|---|
| `ciclo_s` | 900 s (15 minuti) se il file radio non lo cambia |
| `fondo_ogni_s` | un terzo del ciclo |
| `finestra_ogni_s` | due cicli |
| `finestra_primo_s` | un ciclo |
| `tolleranza_finestra_s` | un terzo del ciclo |

Il ciclo di 15 minuti vale per le missioni nuove (rapide, editor, libreria) che non lo cambiano. I file radio delle 22
missioni originali danno tutti un ciclo più corto: 300 s (SCENE0A, 1A-1C, 2A-2C) o 600 s (le altre), col traffico di
fondo ogni 200-400 s. Il ciclo del file ricalcola anche finestre e tolleranza (`parametri_di_missione`), salvo le chiavi
date a parte. Sei missioni hanno le sessioni obbligatorie, senza ricevuta: SCENE2B
(ogni 300 s, ±60 s) e SCENE3B, 5B, 7B, 8B, 9B (ogni 600 s, ±90 s).

### 2.11 Che cosa portano i messaggi

`MessaggioRadio` ha, oltre all'intestazione e al testo in italiano e in inglese:
- **gestore** (`ordine`): una funzione degli script eseguita alla ricezione, se il messaggio è autentico e non è
  scaduto. È il modo in cui gli ordini cambiano la missione;
- **regole d'ingaggio** (`roe`, §2.12);
- **contatto d'intelligence** (`richiesta`, §2.13);
- **zone** (`zone`, §2.14);
- **scadenza** (`scade_s`): il compito vale fino a `dovuto + scade_s`. Ricevuto dopo, è marcato tardi e non esegue il
  gestore; se scade senza essere ricevuto parte `gestore_scaduto`;
- **ricevuta** (`ricevuta`): il comando vuole una trasmissione dopo la ricezione; il debriefing elenca le ricevute
  mancate;
- **annulla** («disregard»): quando va in onda la prima volta, toglie dalla coda il messaggio con quell'id se non è
  ancora andato;
- **autentico**: falso per un inganno (§2.15).

Nella missione un messaggio può partire a un tempo (`dopo_s`), su condizione (`se`: ricevuto un altro messaggio,
unità colpite o fuori, una variabile degli script, una trasmissione del giocatore, un evento del registro), o solo
dagli script (`su_richiesta`, `m688.radio(u, "ID")`). `Missione::passo_radio`, `condizione_radio`, `manda_radio`.

### 2.12 Le regole d'ingaggio

Cinque categorie (`CategoriaRoe`: sottomarini, navi, mercantili, neutrali, aerei), tre stati (`StatoRoe`):
- **libero**: si spara;
- **stretto**: solo su un bersaglio identificato ostile;
- **trattenuto**: non si spara.

La categoria di un'unità (`Missione::categoria_roe`): neutrale se lo dice il file radio (`stato_iniziale.neutrali`),
poi aereo, sottomarino, mercantile per le classi civili (mercantile, cargo, petroliera, peschereccio), altrimenti nave.

Ci sono tre insiemi di ROE: quelle di partenza, quelle **trasmesse** dal comando fino a un istante (solo messaggi
autentici) e quelle **ricevute** dal giocatore. Ogni stato ricorda la serie che l'ha stabilito.

**Il giudizio di un lancio** (`SalaRadio::giudica_lancio`, chiamato da `Missione::giudica_lancio_giocatore`):
1. il bersaglio è **identificato** se il contatto ha durata almeno 1400 ed è dell'altro schieramento;
2. infrazione piena se viola le ROE ricevute autentiche;
3. infrazione «non copiato» se viola ROE trasmesse da almeno un ciclo;
4. solo una nota se viola ROE trasmesse da meno di un ciclo;
5. per un bersaglio immerso si guardano anche le zone protette, con le stesse tre regole, e vale il peggiore.

Il giudizio ricorda anche come il giocatore credeva il bersaglio: con durata almeno 1400 la categoria e la classe (un
neutrale si vede come un mercantile), oltre 800 solo sopra o sotto la superficie. Così il debriefing distingue chi ha
sparato a un mercantile scambiato per una fregata. Ricorda se il lancio era permesso solo da un ordine falso creduto.
Le due infrazioni danno il messaggio 333.

### 2.13 I contatti d'intelligence

Un messaggio con `contatto` porta un'area dove stava qualcuno. La missione lo compila con la verità quando il
messaggio va in onda la prima volta (`Missione::compila_intelligence`):
- posizione: la media delle unità indicate all'ora dell'osservazione (`eta_s` prima), più un errore gaussiano di
  deviazione `errore_m`. La posizione di allora si ricava a ritroso dalla posizione, dalla rotta e dalla velocità
  dell'istante della messa in onda;
- rotta: la media delle rotte più un errore gaussiano di `rotta_err`; velocità: la media più `velocita_err_kn`;
- un falso allarme (`falso`, o nessuna unità viva) cade a caso fra 10 e 30 km dal giocatore, rotta a caso, da 5 a 15
  nodi.

Con le forze dell'IA il contatto viene invece dal rapporto che la forza amica ha davvero (`intelligence_della_forza`):
quello che sapeva, non la verità. La posizione è quella del rapporto, l'errore è `r0_m` della forza, rotta e velocità
stimate restano a zero, con l'incertezza di 180° sulla rotta e di 10 nodi sulla velocità.

Sulla carta il contatto è un'ellisse (`ContattoIntelligence`) che avanza con la rotta stimata e si allarga col tempo:

```
semiasse lungo la rotta   = errore + σ_v · età
semiasse di traverso      = errore + v · sin(σ_rotta) · età
```

- `età` = tempo dall'osservazione; `v`, `σ_v` in m/s con la scala del moto della missione (`m_s_per_nodo`).
- Con la rotta del tutto ignota (incertezza di 180°, come nei rapporti della forza) l'ellisse cresce come un cerchio:
  il semiasse di traverso è uguale a quello lungo la rotta (`ContattoIntelligence::semiasse_traverso`).
- Esempio (SCENE3B, stazione costiera): errore 9 km, σ_v 4 nodi. Con la scala vera di 0,514 m/s per nodo, mezz'ora
  dopo l'osservazione l'asse lungo è di 9 + 3,7 = 12,7 km.
- Un contatto nuovo sulle stesse unità sostituisce il vecchio. Dopo `vita_intelligence_cicli` = 3 cicli dalla
  ricezione il contatto si toglie.

### 2.14 Le zone dello spazio d'acqua

`Zona`: un cerchio (centro e raggio) o un poligono, oppure una **zona protetta mobile**, un rettangolo orientato sulla
rotta di un battello amico (`avanti_m`, `dietro_m`, `lato_m`; `poligono_zona`). Tipi: azione, libera ASW, protetta,
protetta mobile. Ogni zona ha una validità (`dal_s`, `al_s`). Un messaggio aggiunge, sposta (stesso id) o toglie zone.
Come per le ROE ci sono zone di partenza, trasmesse e ricevute. Il punto dentro un poligono si decide col conteggio
dei lati attraversati (`dentro_zona`). Una zona mobile sparisce col suo battello.

Entrare in una zona protetta ricevuta dà il messaggio 328 e una riga nel debriefing.

### 2.15 L'autenticazione e l'inganno

Ogni messaggio ha un gruppo d'autenticazione. Un messaggio con `autentico = false` è un inganno:
- si riceve come gli altri, ma non esegue il gestore;
- le sue ROE e zone entrano fra le ricevute, finché il giocatore non lo verifica;
- la verifica (`SalaRadio::verifica`, Invio sul messaggio) dice se il gruppo torna; se non torna, messaggio 339 e le sue
  ROE e zone restano marcate «dubbie»;
- il giudizio dei lanci usa solo gli ordini autentici: sparare fidandosi di un falso è un'infrazione, e il debriefing
  lo dice («ordine non autenticato, creduto»).

Quattro missioni originali hanno un inganno: SCENE2A, 3B, 5A, 9A.

### 2.16 Il rapporto di contatto e la boa SLOT

**Il premio del rapporto.** Il file radio può dare un gestore per il rapporto di contatto
(`rapporti.contatto = { gestore, ritardo_s, una_volta }`). Dopo la trasmissione e il ritardo la missione lo esegue
(supporto, aggiornamento, permesso): messaggio 331.

**La boa SLOT** (`Missione::radio_slot`). Una boa che trasmette da sola un rapporto di contatto dopo un ritardo, dal
punto dove è stata lanciata:
- parametri del file radio (`slot`): `boe` (predefinito 0), ritardo fra `ritardo_min_s` = 300 s e `ritardo_max_s` =
  1800 s, `portata_vhf_m` = 40 km;
- non serve l'albero e non serve la quota periscopica;
- la trasmissione segue le regole del §2.8 e §2.9 dal punto della boa: il fix nemico, se c'è, cade sulla boa;
- il premio del rapporto scatta solo se un aereo amico è entro la portata VHF della boa;
- senza boe, o con un ritardo fuori dai limiti, la stazione rifiuta (messaggio 377). Alla trasmissione, messaggio 332.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Bande e loro uso; per trasmettere bisogna esporre un'antenna | dossier 08 §1 | alta |
| Broadcast VLF: 50 baud, 6 caratteri al secondo, ciclo di 2 ore | dossier 08 §2.2 | alta |
| Precedenze FLASH, IMMEDIATE, PRIORITY, ROUTINE e prosegni Z, O, P, R | dossier 08 §5.1 | alta |
| DTG, intestazione, numeri di serie e buchi | dossier 08 §5.2-5.3 | alta |
| Ripetizioni 3, 3, 2, 1 | dossier 08 §9.3 | **STIMA** |
| Acquisizione 60, 120, 180 s; HF 10 caratteri al secondo; UHF 600 | dossier 08 §9.2 | **STIMA** |
| Quota periscopica 18 m | dossier 08 §3.4 | media |
| Albero al 50 per cento oltre 10 nodi | scelta di gioco | **STIMA**: il dossier 08 §9.1 proponeva 6 nodi |
| Filo: 120 m, 8 e 12 nodi, rottura a 20 nodi | dossier 08 §3.4, §9.1 | **STIMA**: OE-315 classificato (nota 1) |
| Filo cieco dopo le virate | dossier 08 §9.1 | **STIMA** (nota 2) |
| ELF fino a 300 m e 15 nodi; ZEVS a 82 Hz | dossier 08 §6.1, §9.1 | media; **STIMA** i limiti |
| SSIXS, interrogazione dei messaggi in coda | dossier 08 §4.2 | alta |
| Copertura del satellite fino a 72° | dossier 08 §4.3 | media: calcolo dell'elevazione sui FLTSATCOM |
| Raffica «Akula» di 0,72 s | dossier 08 §6.4 | alta |
| Radiogoniometria: P_rete, τ, errori, ritardi | dossier 08 §7, §9.5 | **STIMA** (nota 3) |
| Orizzonte ESM di 50 km | dossier 10 §4.4 | **STIMA** |
| Finestre USA ±2 ore, URSS 00:00 e 12:00 di Mosca ±30 min | dossier 08 §6.4, §9.6 | media; **STIMA** i valori |
| Orologio compresso: ciclo di 15 min, finestre ogni due cicli | dossier 10 §0, scelta utente | **STIMA** (nota 4) |
| Contatti d'intelligence con ellisse che invecchia | dossier 10 §4.1 | **STIMA** (modello) |
| ROE libero, stretto, trattenuto e giudizio in tre gradi | dossier 10 §4.2 | media (stati), **STIMA** (giudizio) |
| Zone dello spazio d'acqua e protette mobili | dossier 10 §4.3 | media |
| Guinzaglio sovietico, due sessioni saltate | dossier 10 §4.6; dossier 08 §9.6 | bassa per l'Alfa |
| Inganno radio | dossier 10 §4.8 | **finzione di design**: non documentato in fonti aperte |
| Boa SLOT: ritardo 5-30 minuti, portata VHF | dossier 10 §4.4 | **STIMA**: il dossier dà 30-50 km, il codice 40 |

Note:
1. Le prestazioni vere del filo OE-315 sono classificate. I 4-8 nodi di lavoro e i 20 di traino massimo vengono da un
   analogo moderno non americano (dossier 08 §3.4).
2. Il dossier dice «virata oltre 30°»; il codice usa il rateo di virata oltre 0,5 °/s.
3. Forma e valori sono la proposta del dossier 08 §9.5. I 65 e 185 km sono le 35 e 100 miglia del dossier.
4. Col ritmo vero (finestre ogni 12 ore) in una missione di un'ora non arriverebbe nessuna finestra.

## 4. Classico ed Enhanced

Nel classico la sala radio (F2, la RADIO ROOM del 1989) è una stanza di servizio:
- alza e abbassa periscopio e antenna, solo a quota periscopica (20 piedi);
- rilegge i messaggi di bordo già passati (PREV e NEXT);
- le missioni cominciano e finiscono lì, con gli ordini e il giudizio del comando.

L'antenna alzata nel classico conta solo per l'esposizione: allunga di 450 quarti di unità (2,25 nm) la portata a cui
navi e aerei intercettano il battello (`docs/originale/sensori_e_navigazione.md` §3). Non c'è broadcast, non c'è
ricezione per banda, non si trasmette (il manuale del 1989 cita TRANSMIT per rileggere gli ordini e per la chat via
modem). Il remake disegna il classico con `StazioneRadio::disegna_classico`.

Tutto il modello di questo capitolo vale solo nell'Enhanced con la regola di simulazione.

## 5. Come si vede in gioco

- **Stazione Radio** (F2; volume 1, §4.7). In alto il DTG e la prossima finestra con la tolleranza; albero, filo e
  ricevitore ELF; la ricezione per banda (SI, 50 PER CENTO, NO). Poi il broadcast in onda (serie, precedenza, orari,
  quale messa in onda) con la barra della ricezione, i prossimi cinque in programma, le sessioni (fatte e mancate),
  la serie completa o i numeri che mancano, la riga delle ROE ricevute con la serie che le ha stabilite (DUBBIA in
  giallo). Al centro la telescrivente, sotto l'archivio dei messaggi ricevuti con la banda.
- **Tasti** (volume 1, appendice A): 1 antenna, 2 cala o recupera il filo, 3 e 4 archivio, 5 solo gli ordini, 6 tipo
  di trasmissione, 7 trasmetti, 8 satellite, 9 boa SLOT; `[` `]` il ritardo della boa (da 5 a 30 minuti, passi di 5);
  Invio verifica l'autenticazione del messaggio scelto.
- **Messaggi di bordo**: 315 in arrivo, 316 FLASH ricevuto, 317 IMMEDIATE ricevuto, 318 ricevuto, 319 manca un
  messaggio nella serie, 320 chiamata ELF, 321 filo spezzato, 322 filo senza segnale, 323 satellite non disponibile,
  324 trasmissione completata, 325 traffico del satellite ricevuto; 326 ordini cambiati, 327 contatto d'intelligence,
  328 zona protetta, 329 compito scaduto, 330 sessione mancata, 331 supporto in arrivo, 332 la boa SLOT ha trasmesso,
  333 lancio contro le ROE, 339 autenticazione errata; 269 serve l'antenna, 377 nessuna boa.
- **Carta** (volume 1, cap. 5): le ellissi dei contatti d'intelligence col sensore e i minuti; le zone ricevute
  (azione in verde acqua, libera ASW in rosso, protette in magenta), che entrano nell'allarme della rotta.
- **Esposizione**: albero e periscopio alzati sono visibili al radar e all'ESM nemico (capitoli C9 e C10).
- **Suono**: i temporali portano scariche in sala radio (capitolo C8).
- **Debriefing e registro** (capitolo F1): messaggi trasmessi e ricevuti, le serie perse, i lanci giudicati con le ROE
  e le zone, gli ingressi nelle zone protette, le ricevute mancate, le trasmissioni con il fix nemico.

## 6. Taratura e prove

La taratura è sul ritmo di gioco, non su casi storici: in una missione di un'ora il giocatore deve vedere passare
qualche ciclo, perdere almeno un messaggio se resta in profondità senza filo, e poterlo recuperare (ciclo dopo o
satellite). La nota di `docs/11_comandi_e_stazioni.md` (S4) ricorda il caso che ha portato all'orologio compresso: in
SCENE1A, con i tempi veri, in 30 minuti non andava in onda niente.

Prove in `test/prove_radio.cpp`:
- «radio: gruppo data-ora dall'ora e dalla data della missione»;
- «radio: con l'albero alzato il messaggio si riceve per intero, e prima i FLASH»;
- «radio: in profondita' senza antenne si perde; la ripetizione del ciclo dopo si puo' ricevere»;
- «radio: filo galleggiante, VLF fino a 120 m e 8 nodi, al 50% fino a 12, cieco dopo le virate, si rompe a 20»;
- «radio: la richiesta al satellite scarica tutto, persi compresi; oltre 72 gradi niente satellite»;
- «radio: trasmettere espone alla radiogoniometria, P = P_rete (1 - e^(-t/tau))» (UHF 1,5 s con ESM vicino, HF di
  160 s a nord di 72°, raffica sovietica);
- «radio: finestre di comunicazione e campanello ELF dei FLASH per chi ha l'ELF»;
- «radio: il traffico di fondo da' i numeri di serie e gli ordini restano rileggibili»;
- «radio R1: orologio di missione, ciclo di 15 minuti e finestre in scala»;
- «radio R1: ROE trasmesse e ricevute, giudizio dei lanci e debriefing con le serie perse»;
- «radio R1: contatto d'intelligence compilato alla messa in onda, con l'ellisse che cresce»;
- «radio nella missione: niente nel classico; ordini di missione dal briefing come serie 0»;
- «radio nella missione: all'antenna si ricevono il traffico e gli ordini nuovi, che eseguono il gestore»;
- «radio nella missione: in profondita' senza filo non si riceve niente»;
- «radio nella missione: trasmettere con una nave nemica vicina le da' il rilevamento del giocatore»;
- «radio R1 nella missione: file radio, intelligence con la verita', ROE ricevute, premio del rapporto e boa SLOT»;
- «radio nella missione vera: il broadcast va in onda e si annuncia; senza antenne non si riceve»;
- «radio R2: zone trasmesse e ricevute, cerchio e zona mobile attorno al battello amico»;
- «radio R2: inganno riconoscibile dall'autenticazione; compito scaduto; ricevuto tardi non vale»;
- «radio R2: sessioni obbligatorie, mancate e il guinzaglio dopo due»;
- «radio R2 nella missione: zona protetta mobile, ingresso, verifica di un falso e richiamo con l'esito».

Prove in `test/prove_stazione_radio.cpp`:
- «stazione radio: archivio dal piu' recente, ordini di missione a parte, scelta ritagliata»;
- «stazione radio: il tipo di trasmissione gira sui sei, previsione meteo compresa»;
- «stazione radio: a capo sulle parole e sui ritorni»;
- «stazione radio: trasmettere senza antenna e' rifiutato con il motivo; il filo si cala».

Altre: `test/prove_missioni.cpp` («radio E6 e R3: la chiave radio della missione, ordini, messaggi su condizione, su
richiesta e annullati»); `test/prove_scenario_missione.cpp` («scenario: i file radio delle 22 originali si caricano e
vanno in onda», «scenario: nei file radio delle originali gli ordini hanno l'intestazione e i messaggi a evento
partono»); `test/prove_libreria_missioni.cpp` («libreria I02 e S01: la radio della missione…»);
`test/prove_strumenti_carta.cpp` («carta: punto dentro un poligono (zone dello spazio d'acqua)»).

## 7. Limiti noti e in arrivo

- **Ricezione a metà.** Con la ricezione al 50 per cento il messaggio arriva intero o non arriva, una volta su due. Il
  dossier 10 §4.8 proponeva testi con gruppi mancanti; non è fatto.
- **Un solo broadcast.** VLF e HF portano la stessa coda; non ci sono canali diversi né relè aerei (TACAMO, Tu-142MR).
- **Satellite.** La copertura è una soglia di latitudine (72°), non l'elevazione del satellite sul teatro. Lo
  scarico prende i messaggi già andati in onda, non quelli in onda in quel momento.
- **ELF.** Solo i sovietici l'hanno; il 688 no, anche nelle missioni dopo la fine del 1989 (il dossier 08 §9.6 lo
  vedeva come opzione di scenario). Il filo galleggiante non distingue le classi: l'Alfa ce l'ha come il 688.
- **Albero.** Trasmettere non guarda la velocità: basta l'albero a quota periscopica.
- **Radiogoniometria.** La rete strategica nemica è sempre presente; il fix porta solo un contatto alle unità già in
  missione, non manda aerei nuovi.
- **Previsione col meteo fisso.** Con un meteo fisso dato dalla missione la «previsione» parte e si paga
  l'esposizione, ma la risposta non arriva: i bollettini non girano.
- **Boe SLOT.** Nessuna delle 22 missioni originali le dà (`slot.boe` assente, quindi 0): oggi le usano solo le
  missioni che le dichiarano.
- **In arrivo**: nulla di deciso oltre R1-R3, che sono fatti (`docs/11_comandi_e_stazioni.md` §11).

## 8. Riferimenti

- Codice: `nucleo/radio.hpp`, `nucleo/src/radio.cpp` (`SalaRadio`, `ParametriRadio`, `parametri_di_missione`,
  `ricezione`, `avanza`, `trasmetti`, `giudica_lancio`, `ContattoIntelligence`, `dentro_zona`);
  `missioni/src/missione.cpp` (`radio_enhanced`, `passo_radio`, `radio_trasmetti`, `radio_slot`,
  `compila_intelligence`, `giudica_lancio_giocatore`, `contatto_da_radiogoniometria`, `ORIZZONTE_ESM_M`);
  `missioni/src/missione_meteo.cpp` (`bollettini_meteo`); `missioni/src/missione_forze.cpp`
  (`intelligence_della_forza`); `missioni/src/stazioni_missione.cpp` (ordini `radio_*`);
  `app/stazione_radio.cpp`, `app/interfaccia.cpp` (pulsanti della Radio).
- Dati: `data/missioni/radio/<ID>.lua`; testi dei messaggi in `tools/pipeline/esporta_motore.py`.
- Ricerca: dossier 08 (§1-§9), dossier 10 (§0, §4, §5).
- Progetto: `docs/09_schema_missioni.md` §5.1; `docs/11_comandi_e_stazioni.md` §11 (S4, R1-R3);
  `docs/00_ROADMAP.md` punto 22.
- Originale: manuale del 1989, «Radio Room» (`docs/originale/manuale_EN.txt`);
  `docs/originale/sensori_e_navigazione.md` §2-§3.
- Volume 1: `1_uso/04_periscopi_esm_radio.md` §4.7; appendice A.
